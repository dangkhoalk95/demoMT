/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
 
#include "wfc_stub.h"
#include "wfc_sdio.h"
 
 /* hal includes */
#include "memory_attribute.h"
#include "mt7686.h"
#include "hal.h"
#include "lwip/pbuf.h"
#include "wifi_api.h"
#include "wifi_os_api.h"
#include "os.h"
 
#include "hal_gpio_internal.h"
#include "hal_gpio.h"
 
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "wifi_init.h"
#endif
 
#if 0 //For MT7686
#define WFC_HRX_EINT  HAL_GPIO_17
#else //For MT5932
 //#define WFC_HRX_EINT  HAL_GPIO_2
 //#define WFC_HRX_EINT  HAL_GPIO_17
#define WFC_HRX_EINT  HAL_GPIO_4 //HAL_GPIO_4
#endif
 
 
#define SPIM_TEST_PORT          HAL_SPI_SLAVE_0
#define SPIS_TEST_PORT          HAL_SPI_SLAVE_0
#define SPI_TX_DATA_SIZE      1600
#define SPI_RX_DATA_SIZE      256

/* Private variables ---------------------------------------------------------*/

static volatile bool spi_slave_poweron_flag;
static volatile bool spi_slave_cfg_write_flag;
static volatile bool spi_slave_write_data_flag;
static volatile bool spi_slave_cfg_read_flag;
static volatile bool spi_slave_read_data_flag;
static volatile bool spi_slave_timeout_flag;
static volatile bool spi_slave_read_error_flag;
static volatile bool spi_slave_write_error_flag;

static uint8_t g_tx_tansfer_buffer[SPI_TX_DATA_SIZE];
static uint8_t g_rx_tansfer_buffer[SPI_RX_DATA_SIZE];

static uint32_t txdata_length;
static uint32_t rxdata_length;

T_WFC_QBUF   g_htx_free_qbuf;

T_WFC_QBUF  *g_hrx_qbuf;
T_WFC_QBUF  *plist_hrx;

os_semaphore_t g_sem_qbuf;

#define MEM_QUEUE_SIZE             1 //8
#define WFC_CMD_RSP_SIZE        256
#define WFC_HRX_DATA_SIZE      1600
#define WFC_HTX_DATA_SIZE      1900 //1600 increase the buffer size to 1900, because of the wifi initial setting is 1870 bytes.
#define WFC_SLAVE_EVT_SIZE     1800
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_cmd_rsp_buf[WFC_CMD_RSP_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_host_rx_buf[MEM_QUEUE_SIZE][WFC_HRX_DATA_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_host_tx_buf[MEM_QUEUE_SIZE][WFC_HTX_DATA_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_slave_rx_buf[MEM_QUEUE_SIZE][WFC_SLAVE_EVT_SIZE];

uint8_t g_memq_list[MEMQ_CTRL_MAX_NUM][MEM_QUEUE_SIZE];

void wfc_dma_memcpy(uint8_t *dst, uint8_t *src, unsigned int len)
{
    hal_gdma_status_t ret;
    //hal_gdma_running_status_t running_status;
    hal_gdma_channel_t channel = HAL_GDMA_CHANNEL_0;

    hal_gdma_init(HAL_GDMA_CHANNEL_0);

    ret = hal_gdma_start_polling(channel, (uint32_t)dst, (uint32_t)src, len);
    if (HAL_GDMA_STATUS_OK != ret) {
        //printf("DMA unavailable...\n");
        os_memcpy(dst, src, len);
    }
    //hal_gdma_get_running_status(channel, &running_status);
    hal_gdma_deinit(HAL_GDMA_CHANNEL_0);   //De-initialize the GDMA.
}

void wfc_mq_free(T_WFC_QBUF *qbuf)
{
    if (qbuf->ctrl == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_take(g_sem_qbuf, NULL);
    }
    if((qbuf->ctrl != MEMQ_CTRL_HOST_RX) && (qbuf->ctrl != MEMQ_CTRL_SLAVE_EVT) &&
        (qbuf->ctrl < MEMQ_CTRL_MAX_NUM) && (qbuf->qidx < MEM_QUEUE_SIZE)) { //memory leakage protection
        g_memq_list[qbuf->ctrl][qbuf->qidx] = 0; //free
        qbuf->qidx = WFC_MEMQ_INVALID;
    }
    qbuf->size = 0;
    if (qbuf->ctrl == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_give(g_sem_qbuf);
    }
}

int32_t wfc_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx)
{
    uint32_t idx;

    if (ctrl_idx == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_take(g_sem_qbuf, NULL);
    }
    for (idx = 0; idx < MEM_QUEUE_SIZE; idx++) {
        if (g_memq_list[ctrl_idx][idx] == 0) {
            g_memq_list[ctrl_idx][idx] = 1; //allocated
            *qidx = idx;
            if (ctrl_idx == MEMQ_CTRL_HOST_RX) {
                wifi_os_semphr_give(g_sem_qbuf);
            }
            return idx;
        }
    }
    *qidx = WFC_MEMQ_INVALID;
    if (ctrl_idx == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_give(g_sem_qbuf);
    }
    return -1;
}

int wfc_spi_get_htx_buf(void)
{
    g_htx_free_qbuf.ctrl = MEMQ_CTRL_HOST_TX;

    // Get free index from TX receive queue buffer
    if (g_htx_free_qbuf.qidx == WFC_MEMQ_INVALID) {
        if (wfc_mq_get_buf(g_htx_free_qbuf.ctrl, &(g_htx_free_qbuf.qidx)) < 0) {
            printf("Get htx buf fail.\r\n");
        }
    }

    return 0;
}

void wfc_spi_htx_cp(T_WFC_QBUF *qbuf, uint8_t *pbuf, uint32_t offset)
{
    if (((uint32_t)pbuf) % 4) {
        os_memcpy(pbuf, &(g_host_tx_buf[qbuf->qidx][offset]), qbuf->size);
    } else {
        wfc_dma_memcpy(pbuf, &(g_host_tx_buf[qbuf->qidx][offset]), qbuf->size);
    }
}

void wfc_spi_hevt_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize)
{
    uint8_t     *pdst;
    uint32_t    *pmagic;

    qbuf->ctrl = MEMQ_CTRL_SLAVE_EVT;
    qbuf->qidx = 0;// it is not used for MEMQ_CTRL_SLAVE_EVT, just give a initial setting.
    qbuf->size = hrxSize+WFC_MAGIC_LEN*2;

    qbuf->ptr = (uint8_t*)wifi_os_malloc(qbuf->size+1);
    if(qbuf->ptr == NULL) {
        printf("malloc evt buffer fail.\r\n");
        return;
    }
    qbuf->ptr[qbuf->size] = 0;

    // Get free index from Host RX receive queue buffer
//    while (wfc_mq_get_buf(qbuf->ctrl, &(qbuf->qidx)) < 0) {
//        hal_gpt_delay_ms(1);
//    }

    //Add Magic Number at the head of Event Notify
    pdst = qbuf->ptr;
    pmagic = (uint32_t *)pdst;
    *pmagic = WFC_MAGIC_NUM;
    pdst += WFC_MAGIC_LEN;

    //Copy Notify Event and Parameter
    os_memcpy(pdst, (uint8_t *)hrxptr, hrxSize);
    pdst += hrxSize;

    //Append Magic Number at the end of Event Notify
    pmagic = (uint32_t *)pdst;
    *pmagic = WFC_MAGIC_NUM;

    //wfc_dump_buf(g_host_rx_buf[qbuf->qidx], qbuf->size);
}

void wfc_mq_push_hrx(T_WFC_QBUF *qbuf)
{
    T_WFC_QBUF   *pcur;
    T_WFC_QBUF   *pnext;

    wifi_os_semphr_take(g_sem_qbuf, NULL);
    qbuf->next = NULL;

    if (!plist_hrx) {
        plist_hrx = qbuf;
    } else {
        pcur  = plist_hrx;
        pnext = plist_hrx->next;
        while (pnext) {
            pcur = pnext;
            pnext = pcur->next;
        }
        pcur->next = qbuf;
    }
    wifi_os_semphr_give(g_sem_qbuf);
}

T_WFC_QBUF  *wfc_mq_pop_hrx(void)
{
    T_WFC_QBUF *pdst;

    wifi_os_semphr_take(g_sem_qbuf, NULL);
    pdst = plist_hrx;
    if (pdst) {
        plist_hrx = pdst->next;
    }
    wifi_os_semphr_give(g_sem_qbuf);

    return pdst;
}

void wfc_rx_notify(void)
{
    hal_gpio_set_output(WFC_HRX_EINT, HAL_GPIO_DATA_LOW);
    hal_gpt_delay_us(70);
    hal_gpio_set_output(WFC_HRX_EINT, HAL_GPIO_DATA_HIGH);
}

void wfc_mq_clear(T_WFC_QBUF *qbuf)
{
    qbuf->qidx = WFC_MEMQ_INVALID;
    qbuf->size = 0;
}

uint8_t wfc_mq_opcmd_process(T_WFC_QBUF *qbuf, uint8_t *isDTW)
{
    uint8_t   *ptr;
    uint32_t   ptrSize;
    uint16_t  *opcmd;

    opcmd   = (uint16_t *)g_host_tx_buf[qbuf->qidx];
    ptrSize = qbuf->size - 2;

    //printf("%s,%d, opcmd,=0x%x,  qbuf->qidx=%d \n",__FUNCTION__,__LINE__,opcmd, qbuf->qidx);
    if ((*opcmd) == WFC_CMD_DTW_DATA)
    {
        *isDTW = 1;
        return true;
    }
    *isDTW = 0;

    if (ptrSize <= 0)  //OpCmd Only
    {
        wfc_send_queue(WFC_OP_HOST_CMD, *opcmd, NULL, 0, 1);    //Queue Modify Later
    }
    else
    {
        ptr = (uint8_t*)wifi_os_malloc(ptrSize+1);
        if (!ptr)
            return false;

        os_memcpy(ptr, &(g_host_tx_buf[qbuf->qidx][2]), ptrSize);
        ptr[ptrSize] = 0;
        //printf("[JCB]%s,%d, malloc  ptr=0x%x\n",__FUNCTION__,__LINE__, ptr);

        //Send Host Cmd and Parameter to WFC queue
        if(wfc_send_queue(WFC_OP_HOST_CMD, *opcmd, (uint8_t *)ptr, qbuf->size - 2, 1) == false)
        {
           /*if send queue fail,  the ptr need to be free here*/
           if (ptr != NULL)
           {
                printf("%s,%d,  wfc send queue fail !\n",__FUNCTION__,__LINE__);
                wifi_os_free(ptr);
                ptr = NULL;
           }
        }
    }

    return true;
}

extern uint32_t wfc_tx_sleep_lock;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_tansfer_temp_buffer[1600] = {0};
static void wfc_spi_slave_callback(hal_spi_slave_transaction_status_t status, void *user_data)
{
    uint16_t slave_status;
    uint32_t address;
    uint32_t length;
    //hal_spi_slave_config_t spi_configure;

    //printf( "<WFC> : fsm(0x%x)...%s-%d\r\n",status.fsm_status,__FUNCTION__,__LINE__);
   // printf( "F(0x%x, 0x%x)\r\n",status.fsm_status,status.interrupt_status);
    if (HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION == (status.fsm_status)) {
        /* Normal fsm behavior */
        slave_status = status.interrupt_status;
        switch (slave_status) {
            case SPIS_INT_POWER_ON_MASK:
                /* PDN is turned on, initializes spi slave controller here */
                spi_slave_poweron_flag = true;
                printf( "---Receive POWERON command----\r\n\r\n");
                break;
            case SPIS_INT_POWER_OFF_MASK:
                //hal_spi_slave_deinit(SPIM_TEST_PORT);
                spi_slave_poweron_flag = false;
                printf( "---Receive POWEROFF command----\r\n\r\n");
                break;
            case SPIS_INT_RD_CFG_FINISH_MASK:
                /* Call SPI slave send function here to set data address and size*/
                spi_slave_cfg_read_flag = true;
                //hal_spi_slave_get_master_read_config(SPIS_TEST_PORT, &address, &length);
                //hal_spi_slave_send(SPIS_TEST_PORT, g_tansfer_temp_buffer, length);
                printf( "---Receive CRD_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_WR_CFG_FINISH_MASK:
                /* call spi slave read function here to set data address and size*/
                spi_slave_cfg_write_flag = true;
                hal_spi_slave_get_master_write_config(SPIS_TEST_PORT, &address, &length);
                //hal_spi_slave_receive(SPIS_TEST_PORT, g_tansfer_temp_buffer, length);
                g_htx_free_qbuf.size = length;
                wfc_spi_get_htx_buf();
                hal_spi_slave_receive(SPIS_TEST_PORT, &g_host_tx_buf[g_htx_free_qbuf.qidx][0], length);                
                printf( "---Receive CWR_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_RD_TRANS_FINISH_MASK:
                spi_slave_read_data_flag = true;
                printf( "---Receive RD_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_WR_TRANS_FINISH_MASK:
                /* User can now get the data from the address set before */
                spi_slave_write_data_flag = true;
                printf( "---Receive WR_FINISH command----\r\n\r\n");
                wfc_mq_send_queue(&g_htx_free_qbuf, 1);
                wfc_mq_clear(&g_htx_free_qbuf);
#ifdef HAL_SLEEP_MANAGER_ENABLED
                if(wfc_tx_sleep_lock != 0xff) {
                    wifi_unlock_sleep(wfc_tx_sleep_lock);
                }
#endif
                //hex_dump("rcv data",g_tansfer_temp_buffer,100);
                break;
            case SPIS_INT_RD_DATA_ERR_MASK:
                /* Data buffer may be reserved for retransmit depending on user's usage */
                spi_slave_read_error_flag = true;
                //printf( "---detect RD_ERR----\r\n\r\n");
                break;
            case SPIS_INT_WR_DATA_ERR_MASK:
                /* Data in the address set before isn't correct, user may abandon them */
                spi_slave_write_error_flag = true;
                //printf( "---detect WR_ERR----\r\n\r\n");
                break;
            case SPIS_INT_TMOUT_ERR_MASK:
                /* timeout happen */
                spi_slave_timeout_flag = true;
                //printf( "---detect TMOUT_ERR----\r\n\r\n");
                break;
            default:
                printf( "[WFC]: Error - FSM(0x%x, 0x%x)\r\n",status.fsm_status,status.interrupt_status);
                break;
        }
    } else if (HAL_SPI_SLAVE_FSM_INVALID_OPERATION != (status.fsm_status)) {
        switch (status.fsm_status) {
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, fsm is poweroff\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, fsm is poweroff\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, fsm is CR\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW, fsm is CR\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, fsm is CW\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR, fsm is CW\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR, fsm is poweron\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, fsm is poweron\r\n");
                break;
            default:
            	//printf( "<WFC>...%s-%d\n",__FUNCTION__,__LINE__);
                break;
        }
    } else {
        //printf( "HAL_SPI_SLAVE_FSM_INVALID_OPERATION, fsm is poweron\r\n");
    }
}

#define SPIS_TEST_PORT          HAL_SPI_SLAVE_0
#define SPIS_PIN_NUMBER_CS      HAL_GPIO_17
#define SPIS_PIN_FUNC_CS        HAL_GPIO_17_SPISLV_B_CS
#define SPIS_PIN_NUMBER_CLK     HAL_GPIO_16
#define SPIS_PIN_FUNC_CLK       HAL_GPIO_16_SPISLV_B_SCK
#define SPIS_PIN_NUMBER_SIO_0   HAL_GPIO_15
#define SPIS_PIN_FUNC_SIO_0     HAL_GPIO_15_SPISLV_B_SIO0
#define SPIS_PIN_NUMBER_SIO_1   HAL_GPIO_14
#define SPIS_PIN_FUNC_SIO_1     HAL_GPIO_14_SPISLV_B_SIO1

#define WFC_WAKEUP_BY_HOST_EINT   (HAL_GPIO_3)
void gpio3_handler(void *parameter)
{
#ifdef HAL_EINT_FEATURE_MASK
    /* Mask EINT */
    hal_eint_mask(WFC_WAKEUP_BY_HOST_EINT);
#endif
    if(wfc_tx_sleep_lock != 0xff) {
        wifi_lock_sleep(wfc_tx_sleep_lock);
    }

    if(wfc_send_queue(WFC_OP_HOST_CMD, WFC_CMD_WAKEUP_ACK, NULL, 0, 1) == false)
    {
      printf("%s,%d,  wfc send queue fail !\n",__FUNCTION__,__LINE__);
    }

#ifdef HAL_EINT_FEATURE_MASK
    /* Please unmask the EINT if you want to recieve EINT interrupt  */
    hal_eint_unmask(WFC_WAKEUP_BY_HOST_EINT);
#endif
}

void wfc_spi_mq_reset(uint32_t ctrl)
{
    if (ctrl == MEMQ_CTRL_HOST_TX) {
        g_htx_free_qbuf.ctrl = MEMQ_CTRL_HOST_TX;
        g_htx_free_qbuf.qidx = WFC_MEMQ_INVALID;
    }
}

void wfc_eint_init(void)
{
    hal_eint_config_t eint_config;
    
    hal_gpio_init(WFC_HRX_EINT);
    hal_pinmux_set_function(WFC_HRX_EINT, 0);
    hal_gpio_set_direction(WFC_HRX_EINT, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(WFC_HRX_EINT, HAL_GPIO_DATA_HIGH);

    eint_config.trigger_mode = HAL_EINT_EDGE_FALLING; //HAL_EINT_LEVEL_LOW;
    eint_config.debounce_time = 0;

    hal_eint_mask(WFC_WAKEUP_BY_HOST_EINT);
    if (HAL_EINT_STATUS_OK !=  hal_eint_init(WFC_WAKEUP_BY_HOST_EINT, &eint_config)) {
        // error handle
    }

    hal_eint_set_debounce_count(WFC_WAKEUP_BY_HOST_EINT, 1); /* (1/32k) * 2 = 66us*/

    if (HAL_EINT_STATUS_OK !=  hal_eint_register_callback(WFC_WAKEUP_BY_HOST_EINT, gpio3_handler, NULL)) {
        // error handle
    }
    hal_eint_unmask(WFC_WAKEUP_BY_HOST_EINT);

    
    os_memset(g_cmd_rsp_buf, 0, WFC_CMD_RSP_SIZE);
    os_memset(g_host_rx_buf, 0, MEM_QUEUE_SIZE * WFC_HRX_DATA_SIZE);
    os_memset(g_host_tx_buf, 0, MEM_QUEUE_SIZE * WFC_HTX_DATA_SIZE);
    os_memset(g_slave_rx_buf, 0, MEM_QUEUE_SIZE *WFC_SLAVE_EVT_SIZE);
    os_memset(g_memq_list,   0, MEMQ_CTRL_MAX_NUM*MEM_QUEUE_SIZE);

    g_hrx_qbuf = plist_hrx = NULL;

    g_sem_qbuf  = wifi_os_semphr_create_binary();
    wifi_os_semphr_give(g_sem_qbuf);

    //g_sem_rx0   = wifi_os_semphr_create_binary();
    //wifi_os_semphr_give(g_sem_rx0);

    // Reset Global Memq Information
    wfc_spi_mq_reset(MEMQ_CTRL_HOST_TX);
}

void wfc_if_spi_init(void)
{
    printf( "wfc_if_spi_init\r\n");
	hal_spi_slave_config_t spi_configure;

    wfc_eint_init();
    // Reset SPI slaver status
    spi_slave_poweron_flag     = false;
    spi_slave_cfg_write_flag   = false;
    spi_slave_write_data_flag  = false;
    spi_slave_write_error_flag = false;
    spi_slave_cfg_read_flag    = false;
    spi_slave_read_data_flag   = false;
    spi_slave_read_error_flag  = false;    
    spi_slave_timeout_flag     = false;        
    
    // Reset transfer buffer
    txdata_length = 0;
    rxdata_length = 0;
    /* Step2: GPIO configuration for SPI master */
    hal_gpio_init(SPIS_PIN_NUMBER_CS);
    hal_gpio_init(SPIS_PIN_NUMBER_CLK);
    hal_gpio_init(SPIS_PIN_NUMBER_SIO_0);
    hal_gpio_init(SPIS_PIN_NUMBER_SIO_1);
    //hal_gpio_init(SPIS_PIN_NUMBER_SIO_2);
    //hal_gpio_init(SPIS_PIN_NUMBER_SIO_3);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_CS, SPIS_PIN_FUNC_CS);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_CLK, SPIS_PIN_FUNC_CLK);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_SIO_0, SPIS_PIN_FUNC_SIO_0);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_SIO_1, SPIS_PIN_FUNC_SIO_1);
    //hal_pinmux_set_function(SPIS_PIN_NUMBER_SIO_2, SPIS_PIN_FUNC_SIO_2);
    //hal_pinmux_set_function(SPIS_PIN_NUMBER_SIO_3, SPIS_PIN_FUNC_SIO_3);

    spi_configure.bit_order = HAL_SPI_SLAVE_LSB_FIRST;
    spi_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
    spi_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
    spi_configure.timeout_threshold = 0xFFFFFFFF;
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_init(SPIS_TEST_PORT, &spi_configure)) {
        printf("SPI slave init fail\r\n");
    }

    // Register callback function to SPI slaver driver
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_register_callback(SPIS_TEST_PORT, wfc_spi_slave_callback, NULL)) {
        printf( "SPI slave register callback failed\r\n");
    }

#if 0
    // Wait SPI master to send power on command.
    printf( "SPI Slave is waiting for PWON command\r\n");
    while (spi_slave_poweron_flag == false) {
        if (spi_slave_timeout_flag == true) {
            //printf( "Slave detect PWON command error\r\n");
            return;
        }
    }	
#endif
    
    printf("wfc_if_spi_init done.\r\n");
}

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_host_rx_size[4] = {0};
void wfc_spi_send_data_size(uint32_t data_length)
{
    uint32_t address,length;
    
    os_memcpy(g_host_rx_size,&data_length,sizeof(data_length));
    while(spi_slave_cfg_read_flag == false);
    spi_slave_cfg_read_flag = false;
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_get_master_read_config(SPIS_TEST_PORT, &address, &length)) {
        printf("SPI slave get config error.\r\n");
    }
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_send(SPIS_TEST_PORT, g_host_rx_size, sizeof(g_host_rx_size))) {
        printf("Prepare Host RX Buffer Error\r\n");
    }        
    while(spi_slave_read_data_flag == false);
    spi_slave_read_data_flag = false;
    return;
}
void wfc_spi_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize)
{
    uint32_t  slen = sizeof(rspcode);
    uint32_t address;
    uint32_t length;

    // Set DTH data response code into SPI response
    rspcode = (hsize) ? (rspcode | WFC_RSP_DTH_DATA) : rspcode;

    // Assign response code to tx transfer buffer
    pwfc->rspcode = (uint16_t *)&g_cmd_rsp_buf[0];
    *(pwfc->rspcode) = rspcode;

    // Copy DTH data size info into tx transfer buffer
    pwfc->txptr = &g_cmd_rsp_buf[sizeof(rspcode)];
    if (hsize) {
        os_memcpy(pwfc->txptr, &hsize, sizeof(hsize));
        pwfc->txptr +=  sizeof(hsize);
        slen += sizeof(hsize);
    }

    // Copy response content into tx transfer buffer
    if (txptr && txsize) {
        os_memcpy(pwfc->txptr, txptr, txsize);
        slen += txsize;
    }

    // Dump data received (Debug)
    // wfc_dump_buf(&g_cmd_rsp_buf[0], slen);

    wfc_spi_send_data_size(slen);
    
    // Wait SPI master to send config read command.
    while(spi_slave_cfg_read_flag == false);
    spi_slave_cfg_read_flag = false;
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_get_master_read_config(SPIS_TEST_PORT, &address, &length)) {
        printf("SPI slave get config error.\r\n");
        return;
    }
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_send(SPIS_TEST_PORT, &g_cmd_rsp_buf[0], slen)) {
        printf("SPI slave send data error.\r\n");
        return;
    }

    while(spi_slave_read_data_flag == false);
    spi_slave_read_data_flag = false;
    //Wait Send Response Done (Option)

    return;
}

void wfc_spi_send_data(uint8_t *txptr, uint32_t txsize)
{
    uint32_t address,length;
    if (g_hrx_qbuf == NULL) {
        printf("[HRX]: No HRX Data in Queue\r\n");
        goto Exit;
    }

    // Send Host RX Data
    if ( g_hrx_qbuf->ctrl == MEMQ_CTRL_SLAVE_EVT )
    {
        os_memcpy(&g_slave_rx_buf[0][0], g_hrx_qbuf->ptr, g_hrx_qbuf->size);
        wifi_os_free(g_hrx_qbuf->ptr);

        wfc_spi_send_data_size(g_hrx_qbuf->size);

        while(spi_slave_cfg_read_flag == false);
        spi_slave_cfg_read_flag = false;
        if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_get_master_read_config(SPIS_TEST_PORT, &address, &length)) {
            printf("SPI slave get config error.\r\n");
            goto Exit;
        }
        printf("send evt size = %d.\r\n",g_hrx_qbuf->size);
        if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_send(SPIS_TEST_PORT, & (g_slave_rx_buf[0][0]), g_hrx_qbuf->size)) {
            printf("Prepare Host RX Buffer Error\r\n");
            goto Exit;
        }
    } else {
        struct pbuf *q;
        uint8_t     *pdst;

        pdst = g_host_rx_buf[0];
        for (q = (struct pbuf *)g_hrx_qbuf->ptr; q != NULL; q = q->next) {
            if ((((uint32_t)q->payload) % 4) || (((uint32_t)pdst) % 4)) {
                os_memcpy(pdst, (uint8_t *)q->payload, q->len);
            } else {
                wfc_dma_memcpy(pdst, (uint8_t *)q->payload, q->len);
            }
            pdst += q->len;
        }
        pbuf_free((struct pbuf *)(g_hrx_qbuf->ptr));

        wfc_spi_send_data_size(g_hrx_qbuf->size);

        while(spi_slave_cfg_read_flag == false);
        spi_slave_cfg_read_flag = false;
        if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_get_master_read_config(SPIS_TEST_PORT, &address, &length)) {
            printf("SPI slave get config error.\r\n");
            goto Exit;
        }
        if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_send(SPIS_TEST_PORT, & (g_host_rx_buf[0][0]), g_hrx_qbuf->size)) {
            printf("Prepare Host RX Buffer Error\r\n");
            goto Exit;
        }        
    }

    // Wait HRX Done
    while(spi_slave_read_data_flag == false);
    spi_slave_read_data_flag = false;
    goto Exit;

    //while (!g_rx0_done_event);
    //sdio_rx0_done_free(0);

Exit:
    if(g_hrx_qbuf != NULL){
        //Free HRX Buffer
        //printf("%s,%d, free g_hrx_qbuf = 0x%x \r\n",__FUNCTION__,__LINE__);

        wfc_mq_free(g_hrx_qbuf);
        wifi_os_free(g_hrx_qbuf);
        g_hrx_qbuf = NULL;
    }
}

#if 0
int wfc_get_opcode(T_WFC_OPCODE  *op)
{
    uint16_t *opcode = (uint16_t *)&g_rx_tansfer_buffer[0];

    // Init SPI rx transfer buffer	
//    memset(g_rx_tansfer_buffer,0,sizeof(g_rx_tansfer_buffer));        

    // Wait SPI master to send config write command.
    //printf( "SPI Slave is waiting for CW command\r\n");
    while (spi_slave_cfg_write_flag == false) hal_gpt_delay_us(0);

    // Reset CWR_FINISH flag
    spi_slave_cfg_write_flag = false; 

    // Start to receive rx data
    rxdata_length = *(volatile uint32_t *)(0xA00B0014);
    hal_spi_slave_receive(SPIM_TEST_PORT, g_rx_tansfer_buffer, rxdata_length);

    // Wait SPI master to send write data command.
    //printf( "SPI Slave is waiting for WR command\r\n");
    while (spi_slave_write_data_flag == false) {
        if (spi_slave_write_error_flag == true) {
            //printf( "Slave detect WR command error\r\n");
            op->opcode = WFC_INVALID_OPCODE;
            return -1;
        }
    }
                      
    spi_slave_write_data_flag = false;
    
    // Dump data received (Debug)
    //printf("\r\nData Length == %d\r\n",rxdata_length);         
    //for (int i = 0; i < rxdata_length; i++)
    //    printf("0x%02x ", g_rx_tansfer_buffer[i]);
                
    // Get opcode from received data
    //opcode = &g_rx_tansfer_buffer[0];
    op->opcode = *opcode;  
    op->rxptr  =  wifi_os_malloc(rxdata_length-1);
    memcpy(op->rxptr, &g_rx_tansfer_buffer[2], rxdata_length-2);
    op->rxptr[rxdata_length-2] = 0;
    
    return 0;      
}

void wfc_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize)
{
    // Init SPI tx transfer buffer
    //memset(g_tx_tansfer_buffer,0,sizeof(g_tx_tansfer_buffer));     

    // Set DTH data response code into SPI response
    rspcode = (hsize)?(rspcode|WFC_RSP_DTH_DATA):rspcode;
    	
    // Assign response code to tx transfer buffer
    pwfc->rspcode = (uint16_t *)&g_tx_tansfer_buffer[0];
    *(pwfc->rspcode) = rspcode;
    
    // Copy DTH data size info into tx transfer buffer
    pwfc->txptr = &g_tx_tansfer_buffer[sizeof(rspcode)];  
    if (hsize)
    {
    	memcpy(pwfc->txptr, &hsize, sizeof(hsize));
    	pwfc->txptr +=  sizeof(hsize);
    }
    
    // Copy response content into tx transfer buffer	  	
    if ( txptr && txsize) {	
        memcpy(pwfc->txptr, txptr, txsize);
    }    

    // Dump data received (Debug)
    //if (hsize)
    //{	   
    //  printf("rsp:\r\n");
    //  for (int i = 0; i < 8; i++)
    //    printf("0x%02x ", g_tx_tansfer_buffer[i]);
    //  printf("\r\n\r\n");  
    //}


    // Wait SPI master to send config read command.
    //printf( "SPI Slave is waiting for CR command\r\n");
    while (spi_slave_cfg_read_flag == false) hal_gpt_delay_us(0);


    // Reset CRD_FINISH flag
    spi_slave_cfg_read_flag = false;

    // Assign response length
    txdata_length = *(volatile uint32_t *)(0xA00B0014);    
    hal_spi_slave_send(SPIM_TEST_PORT, g_tx_tansfer_buffer, txdata_length);                
    
    // Wait SPI master to send read data command.
    //printf( "SPI Slave is waiting for RD command\r\n");
    while (spi_slave_read_data_flag == false) {
        if (spi_slave_read_error_flag == true) {
            log_hal_error("SPI Slave detect RD error\r\n");  
            break;
        }
    }       	
    
    // Reset SPI slaver status
    spi_slave_read_data_flag = false;
}


int wfc_get_data(T_WFC_OPCODE  *op)
{
    //uint16_t *opcode;
    
    // Wait SPI master to send config write command.
    while (spi_slave_cfg_write_flag == false) hal_gpt_delay_us(0);

    // Reset CWR_FINISH flag
    spi_slave_cfg_write_flag = false; 

    // Prepare Buffer
    rxdata_length = *(volatile uint32_t *)(0xA00B0014);
    //printf("rxdata_length:%d\r\n",rxdata_length);
        
  	op->rxsize = rxdata_length;
   	op->rxptr  = wifi_os_malloc(op->rxsize+1);
   	//memset(op->rxptr, 0, op->rxsize+1);
    
    // Start to receive rx data
    hal_spi_slave_receive(SPIM_TEST_PORT, op->rxptr, op->rxsize);

    // Wait SPI master to send write data command.
    //printf( "SPI Slave is waiting for WR command\r\n");
    while (spi_slave_write_data_flag == false) {
        if (spi_slave_write_error_flag == true) {
            //printf( "Slave detect WR command error\r\n");
            op->opcode = WFC_INVALID_OPCODE;
            return -1;
        }
    }
                         
    // Reset SPI slaver status
    spi_slave_write_data_flag = false;
  
    // Dump data received (Debug)
    //printf("\r\n[Dump]: Data Length == %d\r\n",rxdata_length);         
    //for (int i = 0; i < rxdata_length; i++)
    //    printf("0x%02x ", op->rxptr[i]);    
    //printf("\r\n\r\n\r\n");             
         
    return 0;      
}


void wfc_send_data(T_WFC_OPCODE *pwfc, uint8_t *txptr, uint32_t txsize)
{
    // Init SPI tx transfer buffer
//    memset(g_tx_tansfer_buffer,0,sizeof(g_tx_tansfer_buffer));     

    // Set DTH data response code into SPI response
    memcpy( &g_tx_tansfer_buffer[0], txptr, txsize);
        	
    // Copy response content into tx transfer buffer
    pwfc->txptr = &g_tx_tansfer_buffer[0];

    // Wait SPI master to send config read command.
    //printf( "SPI Slave is waiting for CR command\r\n");
    while (spi_slave_cfg_read_flag == false) hal_gpt_delay_us(0);

    // Reset CRD_FINISH flag
    spi_slave_cfg_read_flag = false;
   
    // Send Tx Data
    hal_spi_slave_send(SPIM_TEST_PORT, pwfc->txptr, txsize);                
    
    // Wait SPI master to send read data command.
    //printf( "SPI Slave is waiting for RD command\r\n");
    while (spi_slave_read_data_flag == false) {
        if (spi_slave_read_error_flag == true) {
            log_hal_error("SPI Slave detect RD error\r\n");  
            break;
        }
    }       	
                          
    // Reset SPI slaver status
    spi_slave_read_data_flag = false;
}
#endif

