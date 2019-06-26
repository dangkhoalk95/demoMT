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

/* hal includes */
#include "memory_attribute.h"
#include "task.h"
#include "queue.h"
#include "sys_init.h"

#include "wifi_host_api.h"
#include "wifi_host_private_api.h"
#include "wifi_host_event.h"
#include "wifi_host_os_api.h"
#include "wifi_host_init.h"
#include "wfcm_sdio.h"
#include "wfcm_stub.h"

#include "lwip/pbuf.h"

#include "hal_gpt.h"
#include "hal_gpio_internal.h"
#include "hal_gpio.h"

#include "hal_eint.h"
#include "host_os_utils.h"


#if (PRODUCT_VERSION == 2523)
#define  WFC_HRX_EINT           5 //EINT5, HAL_GPIO_6
#elif (PRODUCT_VERSION == 2625)
#define  WFC_HRX_EINT           8 //HAL_GPIO_8   6 //EINT6, HAL_GPIO_6
//#define  WFC_HRX_EINT           19 //EINT19, HAL_GPIO_23
#else //For MT7686
#define  WFC_HRX_EINT           HAL_GPIO_17
#endif


#define WFC_SAMPLE_AP           "CastroAP"

#define MAX_CMD_RSP_LENGTH          256
#define MAX_PARAM_LENGTH            256

#define WFCM_STUB_TASK_NAME        "wfm"
#define WFCM_STUB_TASK_STACK_SIZE   512
#define WFCM_STUB_TASK_PRIORITY       7 //6

#define WFCM_WIFI_CONF_TASK_NAME         "wfc"
#define WFCM_WIFI_CONF_TASK_STACK_SIZE    512
#define WFCM_WIFI_CONF_TASK_PRIORITY        6

#define WFCM_EVT_TASK_NAME         "wfv"
#define WFCM_EVT_TASK_STACK_SIZE    512
#define WFCM_EVT_TASK_PRIORITY        7 //6

#define WFCM_MQ_TASK_NAME                "wfm"
#define WFCM_MQ_TASK_STACK_SIZE           2048
#define WFCM_MQ_TASK_PRIORITY                6
#define WFCM_MQ_SIZE                       100

#define WFCM_QUEUE_SIZE                      20

//#define WFC_HIGH_PERFORMANCE_FLOW_ENABLE

static TaskHandle_t  wfcm_stub_task_handle = NULL;
#if 0
static TaskHandle_t  wfcm_wifi_conf_handle = NULL;
#endif
static TaskHandle_t  wfcm_evt_handle        = NULL;
static QueueHandle_t wfcm_queue             = NULL;
static QueueHandle_t wfcm_evt_queue         = NULL;


#ifdef HAL_SLEEP_MANAGER_ENABLED
uint32_t wfcm_sleep_lock = 0xff;
#endif

os_semaphore_t g_wfcm_sem_rsp = NULL;

extern const wifi_event_handler_t wifi_rx_event_handler[];
bool wfcm_sdio_check_init(void);

//true - no wait response, false - need wait response from slave
T_WFC_CONFIG g_wfcm_stub_opcode[] =
{
	//WFC System Cmd
    {WFC_CT_SYSTEM, WFC_CMD_DTH_READY,         false},   //Need wait HRX data size
    {WFC_CT_SYSTEM, WFC_CMD_DTW_DATA,          false},   //No Response Need
    {WFC_CT_SYSTEM, WFC_CMD_TASK_DEINIT,       false},   //No Response Need

    //WiFi Configure Cmd
    {WFC_CT_USER, WFC_CMD_WFC_READY,          false},

    //OS Cmd
    {WFC_CT_USER, WFC_CMD_IP_READY,            false},
    {WFC_CT_USER, WFC_CMD_OS_INFO,             true},

    //Platform Cmd
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF,        true},
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF_CLIENT, true},

    //AT Cmd
    #ifdef MTK_ATCI_ENABLE
    {WFC_CT_USER, WFC_CMD_AT,                  false},
    #endif


    {WFC_CT_USER, WFC_CMD_API,                  false},
    {WFC_CT_USER, WFC_CMD_BWCS,                 false},

    {WFC_CT_USER, WFC_CMD_MAX_SUPPORT,         true},
};

int32_t wifi_station_port_secure_event_handler(uint32_t event, uint8_t *payload, uint32_t length);
int32_t wifi_station_disconnected_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

struct netif *netif_find_by_type(uint32_t netif_type);

#if MTK_WIFI_STUB_CONF_SPI_ENABLE
    #define FLASH_MT5932_BIN_ADDRESS  (0x08346800)
    #define FLASH_MT5932_BIN_SIZE     (378*1024)
    bool xboot_over_spi_fw_download(uint8_t *address, uint32_t length);
#else
    #define FLASH_MT5932_BIN_ADDRESS  (0x08210000)
    #define FLASH_MT5932_BIN_SIZE     (378*1024)
    bool xboot_over_sdio_fw_download(uint8_t *address, uint32_t length);
#endif

#if (PRODUCT_VERSION == 2523)
void mt5932_low_level_input(struct netif *netif, struct pbuf *p);
#else
void ethernetif_intr_enhance_mode_dispatch(struct pbuf *p, struct netif *netif);
#endif

uint16_t wfcm_evt_send(uint16_t evt_id, uint8_t *ptr, uint32_t ptrSize, uint8_t fromISR);

T_WFC_CONFIG *wfcm_get_config(uint16_t opcode)
{
    int            idx = 0;
    T_WFC_CONFIG  *pstub;

    pstub = &g_wfcm_stub_opcode[idx];

    while (pstub->op != WFC_CMD_MAX_SUPPORT)
    {
        if (pstub->op == opcode )
        {
            return pstub;
        }
        pstub = &g_wfcm_stub_opcode[++idx];
    }

    return NULL;
}

int wfcm_if_init(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    return wfcm_if_spi_init();
	#else
	    return wfcm_if_sdio_init();
	#endif
}

void wfcm_if_deinit(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    wfcm_if_spi_deinit();
	#else
	    wfcm_if_sdio_deinit();
	#endif
}

void wfcm_fw_download(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    xboot_over_spi_fw_download((uint8_t *)FLASH_MT5932_BIN_ADDRESS, FLASH_MT5932_BIN_SIZE);
	#else
	    xboot_over_sdio_fw_download((uint8_t *)FLASH_MT5932_BIN_ADDRESS, FLASH_MT5932_BIN_SIZE);
	#endif
}


uint8_t *wfcm_hrx_data_alloc(uint32_t bufSize)
{
    //return pbuf_alloc(PBUF_RAW, bufSize, PBUF_RAM);
    return (uint8_t*)pbuf_alloc(PBUF_RAW, bufSize, PBUF_POOL);
}


uint8_t *wfcm_hrx_data_buffer(uint8_t *pbuf)
{
    return (pbuf)?((struct pbuf *)pbuf)->payload:NULL;
}

void wfcm_hrx_handler(uint8_t *pbuf, uint32_t bufSize)
{
    struct pbuf  *p;
    struct netif *sta_if;

    uint32_t    *pmagic_s;
    uint32_t    *pmagic_e;

    if (!pbuf)
    {
        printf("[HRX]: Null Packet Received!\r\n");
        return;
    }

    p = (struct pbuf *)pbuf;

    //Check if Event Packet or Data Packet
    pmagic_s = (uint32_t*)p->payload;
    if ( (*pmagic_s) == WFC_MAGIC_NUM )
    {
        pmagic_e = (uint32_t*)(p->payload + bufSize - WFC_MAGIC_LEN);
        if ((*pmagic_e) == WFC_MAGIC_NUM)
        {
            uint8_t  *ptr, *param=NULL;
            uint16_t *peid;
            uint32_t  ptrSize;

            ptr = (uint8_t *)(p->payload+WFC_MAGIC_LEN);
            peid = (uint16_t*)ptr;
            ptr += sizeof(uint16_t);
            ptrSize = bufSize - WFC_MAGIC_LEN*2 - sizeof(uint16_t);
            if (ptrSize)
            {
                //Allocate Event Parameter, Need to be free by event handler
                param = (uint8_t*)wifi_os_malloc(ptrSize+1);
                if (param == NULL)
                {
                    printf("malloc fail!\r\n");
                    return;
                }
                //printf("Event alloc buf:0x%x, size=%d\n",  param,  ptrSize+1);
                os_memcpy(param, ptr, ptrSize);
                param[ptrSize] = 0;
            }
            //printf("Eid(%u), param(%p), size(%u)...%s-%d\r\n",*peid, param, ptrSize,__FUNCTION__,__LINE__);
            //wfcm_dump_buf(p->payload+WFC_MAGIC_LEN, bufSize-(WFC_MAGIC_LEN*2));

            // Send Event to Event Queue
            /* The param will be freed in wfcm_evt_task() while deal with the peid*/
            if(wfcm_evt_send(*peid, param, ptrSize, 0) != WFC_RSP_OK){
                if(param != NULL){
                    wifi_os_free(param);
                    param = NULL;
                }
            }

            //Free SDIO received Buffer
            pbuf_free(p);
            
#ifdef HAL_SLEEP_MANAGER_ENABLED
#ifndef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
            if(wfcm_sleep_lock != 0xff) {
                wifi_unlock_sleep(wfcm_sleep_lock);
            }
#endif
#endif
            return;
        }
    }

    sta_if = netif_find_by_type(2);  //NETIF_TYPE_STA = 2

    //printf("lwip <- S(%d)\r\n", bufSize);
    //wfcm_dump_buf(p->payload, bufSize);

    mt5932_low_level_input(sta_if, p);
#ifdef HAL_SLEEP_MANAGER_ENABLED
#ifndef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
    if(wfcm_sleep_lock != 0xff) {
        wifi_unlock_sleep(wfcm_sleep_lock);
    }
#endif 
#endif
    //pbuf_free(p);
}


uint16_t wfcm_send_cmd_to_slave(T_WFC_OPCODE *pwfc, uint8_t qbuf_release)
{
    uint16_t      *op;
    uint16_t       rsp;
    uint32_t       rspSize;
    uint8_t        rspbuf[MAX_CMD_RSP_LENGTH];
    T_WFC_CONFIG  *pwfg;

    pwfg    = pwfc->pwfg;
    op      = (uint16_t*)&rspbuf[0];
    rspSize = MAX_CMD_RSP_LENGTH;

    os_memset(rspbuf, 0, MAX_CMD_RSP_LENGTH);

#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
    // Send Host Command
    wfcm_spi_send_data(&(pwfc->qbuf), qbuf_release);

    // Receive Command Response
    wfcm_spi_receive_cmdrsp(rspbuf, &rspSize);
#else
    // Send Host Command
    wfcm_sdio_send_data(&(pwfc->qbuf), qbuf_release);

    // Receive Command Response
    wfcm_sdio_receive_cmdrsp(rspbuf, &rspSize);
#endif

    // Get Response Code
    rsp = *op;

    if ( !pwfg->auto_rsp )
    {
    	// Copy Parameter Value
        os_memcpy(pwfc->rxptr, &rspbuf[2], rspSize-2);
        *(pwfc->rspsize) = rspSize-2;
        //wfcm_dump_buf(rspbuf, rspSize);
    }

    return rsp;
}


uint16_t wfcm_send_data_to_slave(T_WFC_QBUF *qbuf)
{
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
    wfcm_spi_send_data(qbuf, true);    
#else
    wfcm_sdio_send_data(qbuf, true);
#endif

    return WFC_RSP_OK;
}

T_WFC_OPCODE *plist_htx = NULL;
extern SemaphoreHandle_t g_wfcm_sem_htx;
void wfcm_push_htx_buff(T_WFC_OPCODE *qbuf)
{
    T_WFC_OPCODE   *pnext;
    wifi_os_semphr_take(g_wfcm_sem_htx, NULL);
    qbuf->next = NULL;

    if (!plist_htx) {
        plist_htx = qbuf;
    } else {
        pnext = plist_htx;
        while (pnext->next) {
            pnext = pnext->next;
        }
        pnext->next = qbuf;
    }

    wifi_os_semphr_give(g_wfcm_sem_htx);
}

T_WFC_OPCODE  *wfcm_pop_htx_buff(void)
{
    T_WFC_OPCODE *pdst;

    wifi_os_semphr_take(g_wfcm_sem_htx, NULL);
    pdst = plist_htx;
    if (pdst) {
        plist_htx = pdst->next;
    }
    wifi_os_semphr_give(g_wfcm_sem_htx);

    return pdst;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
#if (PRODUCT_VERSION == 2523)
#define WFCM_WAKEUP_SLAVE_EINT  (HAL_GPIO_24)
#elif (PRODUCT_VERSION == 2625)
#define WFCM_WAKEUP_SLAVE_EINT  (HAL_GPIO_10) //(HAL_GPIO_13)
#endif

void wfcm_wakeup_notify(void)
{
    hal_gpio_set_output(WFCM_WAKEUP_SLAVE_EINT, HAL_GPIO_DATA_LOW);
    hal_gpt_delay_us(100);
    hal_gpio_set_output(WFCM_WAKEUP_SLAVE_EINT, HAL_GPIO_DATA_HIGH);
}
#endif

volatile uint32_t g_slave_rcv_eint_flag = 0;
uint16_t wfcm_to_slave(uint16_t op, uint8_t *ptr, uint32_t ptrSize, uint8_t *rsp, uint32_t *rspSize, uint8_t fromISR)
{
    uint16_t      rspcode = WFC_RSP_OK;
    T_WFC_OPCODE  *wfcm;
    T_WFC_OPCODE  wfcm_dth;
    T_WFC_CONFIG *pwfg;

    if (!wfcm_queue || !g_wfcm_sem_rsp)
    {
    	printf("[WFCM]: To Slave Fail - %u, %u\r\n", (unsigned int)wfcm_queue, (unsigned int)g_wfcm_sem_rsp);
    	return WFC_RSP_QUEUE_ERR;
    }

    // Get wifi configure opcode information
    pwfg = wfcm_get_config(op);
    if (!pwfg)
    {
    	return WFC_RSP_INVALID_OPCODE;
    }

    if (fromISR)
    {
        wfcm_dth.opcode  = op;
        wfcm_dth.txptr   = ptr;
        wfcm_dth.txsize  = ptrSize;
        wfcm_dth.rxptr   = rsp;
        wfcm_dth.rspsize = rspSize;
        wfcm_dth.pwfg    = pwfg;
        wfcm_dth.rspcode = &rspcode;

        // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
        wfcm_dth.qbuf.ctrl = MEMQ_CTRL_HOST_TX;

        wfcm_dth.qbuf.qidx = WFC_MEMQ_INVALID;
        if ( wifi_os_queue_send_from_isr(wfcm_queue, &wfcm_dth) != 0 )
        {
            printf("Send Q fail.\r\n");
            return WFC_RSP_QUEUE_ERR;
        }
          wifi_os_switch_context_from_isr();
    }
    else
    {

        wfcm = (T_WFC_OPCODE *)wifi_os_malloc(sizeof(T_WFC_OPCODE));
        os_memset(wfcm, 0, sizeof(T_WFC_OPCODE));

        wfcm->opcode  = op;
        wfcm->txptr   = ptr;
        wfcm->txsize  = ptrSize;
        wfcm->rxptr   = rsp;
        wfcm->rspsize = rspSize;
        wfcm->pwfg    = pwfg;
        wfcm->rspcode = &rspcode;

        // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
        wfcm->qbuf.ctrl = MEMQ_CTRL_HOST_TX;

        wfcm_memq_get_qbuf(&(wfcm->qbuf));
        if ( op == WFC_CMD_DTW_DATA)
        {
            // Copy Host TX data into HTX buffer
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
            wfcm_spi_htx_cp(WFC_CMD_DTW_DATA, &(wfcm->qbuf), ptr, ptrSize); //Copy Host TX Data to SDIO TX Buffer,
#else
            wfcm_sdio_htx_cp(WFC_CMD_DTW_DATA, &(wfcm->qbuf), ptr, ptrSize); //Copy Host TX Data to SDIO TX Buffer,
#endif
        }
        else
        {
            // Copy Cmd Parameter into HTX buffer
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
            wfcm_spi_cmd_cp(op, &(wfcm->qbuf), ptr, ptrSize, 0);
#else
            wfcm_sdio_cmd_cp(op, &(wfcm->qbuf), ptr, ptrSize, 0);
#endif
        }

        wfcm_push_htx_buff(wfcm);
#ifdef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
   // wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 1);   //Rx Interrupt
        wfcm_dth.opcode  = WFC_CMD_DTW_DATA;        
        wfcm_dth.txptr   = NULL;
        wfcm_dth.txsize  = 0;
        wfcm_dth.rxptr   = NULL;
        wfcm_dth.rspsize = 0;
        wfcm_dth.pwfg    = wfcm_get_config(WFC_CMD_DTW_DATA);
        wfcm_dth.rspcode = &rspcode;

        // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
        wfcm_dth.qbuf.ctrl = MEMQ_CTRL_HOST_TX;

        wfcm_dth.qbuf.qidx = WFC_MEMQ_INVALID;
        if ( wifi_os_queue_send(wfcm_queue, &wfcm_dth, 0) != 0 )
        {
            printf("Send Q fail.\r\n");
            return WFC_RSP_QUEUE_ERR;
        }

#else
    //uint32_t wait_eint_ack_count = 0;
    //EINT:
        wfcm_wakeup_notify();
        //PF: In case of 5932 missed the interrupt in low power, add this action
        //Add this in 2625 will lead abnormal
        /*while((g_slave_rcv_eint_flag == 0) && (wait_eint_ack_count++<50000));

        if(g_slave_rcv_eint_flag == 0){
            printf("RCV EINT ACK timeout.\r\n");
            wait_eint_ack_count = 0;
            goto EINT;
        }else {
            wait_eint_ack_count = 0;
            g_slave_rcv_eint_flag = 0;
        }*/
#endif
        // Send WiFi Configure Response Code (Response No Wait)
        if ( !pwfg->auto_rsp && (op!=WFC_CMD_DTH_READY) && (op!=WFC_CMD_DTW_DATA) )
        {
            wifi_os_semphr_take(g_wfcm_sem_rsp, NULL);
            return rspcode;
        }
    }
    return WFC_RSP_OK;
}


void wfcm_data_free(struct pbuf *p)
{
	pbuf_free(p);
}

void wfcm_tx_handler(T_WFC_OPCODE *wfcm_qbuf)
{
    if((wfcm_qbuf->pwfg->type == WFC_CT_SYSTEM) && (wfcm_qbuf->opcode == WFC_CMD_DTW_DATA)) {
        wfcm_send_data_to_slave(&(wfcm_qbuf->qbuf));
    }else if(wfcm_qbuf->pwfg->type != WFC_CT_SYSTEM){
        *(wfcm_qbuf->rspcode) = wfcm_send_cmd_to_slave(wfcm_qbuf, true);
        if (!(wfcm_qbuf->pwfg->auto_rsp))
        {
            wifi_os_semphr_give(g_wfcm_sem_rsp);
        }
    }else{
        printf("[PF]TX unkown type.%d,%d.\r\n",wfcm_qbuf->pwfg->type,wfcm_qbuf->opcode);
    }
    
#ifdef HAL_SLEEP_MANAGER_ENABLED
#ifndef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
    if(wfcm_sleep_lock != 0xff) {
        wifi_unlock_sleep(wfcm_sleep_lock);
    }
#endif
#endif
}

void wfcm_queue_handler(void)
{
    uint8_t       bExit = false;
    uint8_t       rspbuf[MAX_CMD_RSP_LENGTH];
    uint32_t      rspSize;
    T_WFC_OPCODE *wfcm_buff;

    T_WFC_OPCODE  wfcq;
    T_WFC_CONFIG *pwfg;

    wfcm_queue = wifi_os_queue_create(WFCM_QUEUE_SIZE, sizeof(T_WFC_OPCODE));
    if (wfcm_queue==NULL)
    {
        //printf( "<<ERROR>> :: wfcm_queue create fail!\r\n");
        vTaskDelete(NULL);
        return;
    }

    g_wfcm_sem_rsp = wifi_os_semphr_create_binary();

    os_memset(rspbuf, 0, sizeof(rspbuf));

    while (!bExit)
    {
        if (wifi_os_queue_receive(wfcm_queue, &wfcq, NULL) == 0)
        {
            //printf("<WFCM>: opcode(%d)\r\n", wfcq.opcode);
            pwfg = wfcq.pwfg;

            if (pwfg->type == WFC_CT_SYSTEM)
            {
                // OpCode Process
                switch (wfcq.opcode)
                {
                	//WFC System Cmd
//#ifdef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
                	case WFC_CMD_DTW_DATA:
                    {                       
                        wfcm_buff = wfcm_pop_htx_buff();
                        wfcm_tx_handler(wfcm_buff);
                        wfcm_mq_free(&(wfcm_buff->qbuf));
                        wifi_os_free(wfcm_buff);
                        break;
                    }
//#endif                    
                    case WFC_CMD_DTH_READY:
                    {
                    	uint8_t    *p;
                    	uint32_t    hrxSize;
#ifdef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
                        //Get HRX Data                       
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
                        wfcm_spi_receive_data_size(&hrxSize);
#else
                        wfcm_sdio_receive_data_size(&hrxSize);
#endif
                        //Allocate Data Buffer (pbuf/mem)
                        p = wfcm_hrx_data_alloc(hrxSize);
                        if (!p)
                        {
                            printf("[WFCM]: Data Buffer Allocate Fail!,size = %d\r\n", (int)hrxSize);
                            //break;
                        }
                        
                        while(!p) {    //Pengfei: workaround for the case that allocate buffer when do iperf
                            p = wfcm_hrx_data_alloc(hrxSize);
                            vTaskDelay(10);
                        }

#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
                        wfcm_spi_receive_data_payload((uint8_t*)wfcm_hrx_data_buffer(p), hrxSize);
#else
                        wfcm_sdio_receive_data_payload((uint8_t*)wfcm_hrx_data_buffer(p), hrxSize);
#endif
                        //HRX Receiver
                        wfcm_hrx_handler(p, hrxSize);  
                        break;
#else //[PF] new flow
                        uint16_t      *op;
                        rspSize = MAX_CMD_RSP_LENGTH;
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
                        wfcm_spi_cmd_cp(WFC_CMD_DTH_READY, &wfcq.qbuf, NULL, 0, 1);
                        wifi_lock_bus();
                        wfcm_spi_receive_cmdrsp(rspbuf, &rspSize); //need to improve: wfcm_sdio_receive_data() can get the size?
#else
                        wfcm_sdio_cmd_cp(WFC_CMD_DTH_READY, &wfcq.qbuf, NULL, 0, 1);
                        wfcm_sdio_receive_cmdrsp(rspbuf, &rspSize); //need to improve: wfcm_sdio_receive_data() can get the size?
#endif
                        op = (uint16_t*)&rspbuf[0]; // need to improve: use the structure format

                        if (*op == WFC_RSP_NO_HRX)   // No HRX Data for Host
                        {
                            printf("No HRX Data\r\n");
                            break;
                        }

                        if(*op == (WFC_RSP_DTH_DATA|WFC_RSP_OK)) {
                            //Do RX process
                            hrxSize = *((uint32_t *)&rspbuf[2]);
                            if (!hrxSize)
                            {
                                printf("[Error]: Get HRX Size Fail\r\n");
                                break;
                            }

                            //Allocate Data Buffer (pbuf/mem)
                            p = wfcm_hrx_data_alloc(hrxSize);
                            if (!p)
                            {
                                printf("[WFCM]: Data Buffer Allocate Fail!,size = %d\r\n", (int)hrxSize);
                                //break;
                            }
                            while(!p) {    //Pengfei: workaround for the case that allocate buffer when do iperf
                                p = wfcm_hrx_data_alloc(hrxSize);
                                vTaskDelay(10);
                            }

#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
                            //Get HRX Data
                            printf("rcv data size = %d.\r\n",hrxSize);
                            wfcm_spi_receive_data((uint8_t*)wfcm_hrx_data_buffer(p), hrxSize);
#else
                            //Get HRX Data
                            wfcm_sdio_receive_data((uint8_t*)wfcm_hrx_data_buffer(p), hrxSize);
#endif
                            //HRX Receiver
                            wfcm_hrx_handler(p, hrxSize);
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
                            wifi_unlock_bus();
#endif
                            break;
                        }else if(*op == WFC_RSP_OK) {
                            //Do TX provess
                            g_slave_rcv_eint_flag = 1;
                            wfcm_buff = wfcm_pop_htx_buff();
                            wfcm_tx_handler(wfcm_buff);
#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
                            wifi_unlock_bus();
#endif
                            wfcm_mq_free(&(wfcm_buff->qbuf));
                            wifi_os_free(wfcm_buff);
                            break;
                        }else {
                            printf("Unkown response code,%d.\r\n",*op);
                            break;
                        }
#endif
                    }
                    case WFC_CMD_TASK_DEINIT:
                    {
                        bExit = true;
                        wfcm_buff = wfcm_pop_htx_buff();
                        while(wfcm_buff != NULL) {
                            wfcm_mq_free(&(wfcm_buff->qbuf));
                            wifi_os_free(wfcm_buff);
                            wfcm_buff = wfcm_pop_htx_buff();
                        }
                        if(wfcm_queue != NULL) {
                            wifi_os_queue_delete(wfcm_queue);
                            wfcm_queue = NULL;
                        }
                        if(g_wfcm_sem_rsp != NULL) {
                            wifi_os_semphr_delete(g_wfcm_sem_rsp);
                            g_wfcm_sem_rsp = NULL;
                        }
                        printf("Delete wfcm_stub_task task.\r\n");                       
                    }
                    default:
                    	break;
                }
            }
        }
    }

    vTaskDelete(NULL);
}


void wfcm_get_slave_notify_handler(void *parameter)
{
#ifdef HAL_EINT_FEATURE_MASK
    /* Mask EINT */
    hal_eint_mask(WFC_HRX_EINT);
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
    if(wfcm_sleep_lock != 0xff) {
        wifi_lock_sleep(wfcm_sleep_lock);
    }
#endif
    wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 1);   //Rx Interrupt

#ifdef HAL_EINT_FEATURE_MASK
    /* Please unmask the EINT if you want to recieve EINT interrupt  */
    hal_eint_unmask(WFC_HRX_EINT);
#endif
}

static void wfcm_stub_task(void)
{
    int  ret;

    /*Download Slave/Device FW*/
    wfcm_fw_download();

#ifdef HAL_SLEEP_MANAGER_ENABLED
    wfcm_sleep_lock = wifi_set_sleep_handle("wfcm_low_power");
#endif

#ifndef MTK_WIFI_STUB_CONF_SPI_ENABLE
    wfcm_if_deinit();

    /*waiting Boot ROM release*/
    wfcm_sdio_check_init();
#endif

    ret = wfcm_if_init();
    if (ret)
    {
    	printf("WFCm Interface Init Fail!\r\n");
    	vTaskDelete(NULL);
    	return;
    }

    hal_eint_config_t eint_config;

    //eint_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint_config.debounce_time = 0;

    hal_eint_mask(WFC_HRX_EINT);
    if (HAL_EINT_STATUS_OK !=  hal_eint_init(WFC_HRX_EINT, &eint_config)) {
        // error handle
    }

    hal_eint_set_debounce_count(WFC_HRX_EINT, 1); /* (1/32k) * 2 = 66us*/

    if (HAL_EINT_STATUS_OK !=  hal_eint_register_callback(WFC_HRX_EINT, wfcm_get_slave_notify_handler, NULL)) {
        // error handle
    }
    hal_eint_unmask(WFC_HRX_EINT);

    wfcm_queue_handler();


    vTaskDelete(NULL);
}

uint16_t wfcm_evt_send(uint16_t evt_id, uint8_t *ptr, uint32_t ptrSize, uint8_t fromISR)
{
    T_WFC_EVT  evt;

    if (!wfcm_evt_queue)
    {
    	printf("[WFCM]: Event Send Fail - %u\r\n", (unsigned int)wfcm_evt_queue);
    	return WFC_RSP_QUEUE_ERR;
    }

    os_memset(&evt, 0, sizeof(T_WFC_EVT));

    evt.eid    = evt_id;
    evt.param  = ptr;
    evt.size   = ptrSize;

    // Send to WFC Queue
    if (fromISR)
    {
    	if ( wifi_os_queue_send_from_isr(wfcm_evt_queue, &evt) != 0 )
    		return WFC_RSP_QUEUE_ERR;
    }
    else if ( wifi_os_queue_send(wfcm_evt_queue, &evt, 0) != 0 )
    {
        return WFC_RSP_QUEUE_ERR;
    }

    return WFC_RSP_OK;
}


static void wfcm_evt_task(void)
{
    uint8_t       bExit = false;
    T_WFC_EVT     evt;

    while ((!wfcm_queue || !g_wfcm_sem_rsp)) {
        hal_gpt_delay_ms(10);
    }

    wfcm_evt_queue = wifi_os_queue_create(WFCM_QUEUE_SIZE, sizeof(T_WFC_EVT));
    if (wfcm_evt_queue == NULL)
    {
        vTaskDelete(NULL);
        return;
    }

    while (!bExit)
    {
        if (wifi_os_queue_receive(wfcm_evt_queue, &evt, NULL) == 0)
        {
            // Event Process
            switch (evt.eid)
            {
            	//WFC System Cmd
                case WFC_EVT_WIFI_CONNECTED:
                {
                    wifi_station_port_secure_event_handler(WIFI_EVENT_IOT_CONNECTED, NULL, 0);
                    break;
                }
                case WFC_EVT_WIFI_DISCONNECT:
                {
                    wifi_station_disconnected_event_handler(WIFI_EVENT_IOT_DISCONNECTED, NULL, 0);
                    break;
                }
                case WFC_EVT_SCAN_RESULTS:
                {
                    wifi_host_get_scan_result(evt.param);
                    //wifi_rx_event_handler[WIFI_EVENT_IOT_SCAN_COMPLETE](WIFI_EVENT_IOT_SCAN_COMPLETE, evt.param, evt.size);
                    break;
                }
                case WFC_EVT_INIT_DONE:
                {
                    wifi_rx_event_handler[WIFI_EVENT_IOT_INIT_COMPLETE](WIFI_EVENT_IOT_INIT_COMPLETE, NULL, 0);
                    break;
                }
                case WFC_EVT_TASK_DEINIT:
                {
                    bExit = true;
                    if(wfcm_evt_queue != NULL) {
                        wifi_os_queue_delete(wfcm_evt_queue);
                        wfcm_evt_queue = NULL;
                    }
                    printf("Delete evt task.\r\n");
                    break;
                }
                default:
                	break;
            }

            /*JCB0613, free event buffer which alloc in wfcm_hrx_handler()*/
            if(evt.param != NULL){
                //printf("Event free buf:0x%x \n",  evt.param);
                wifi_os_free(evt.param);
                evt.param = NULL;
            }
        }
    }

    vTaskDelete(NULL);
}


int32_t wifi_host_wfcm_init(void)
{
	// WFC Host Task
    if ( wifi_os_task_create((TaskFunction_t)wfcm_stub_task,
                            WFCM_STUB_TASK_NAME,
                            WFCM_STUB_TASK_STACK_SIZE*4,
                            NULL,
                            WFCM_STUB_TASK_PRIORITY,
                            &wfcm_stub_task_handle) != 0 )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return -1;
    }

    if ( wifi_os_task_create((TaskFunction_t)wfcm_evt_task,
                            WFCM_EVT_TASK_NAME,
                            WFCM_EVT_TASK_STACK_SIZE*4,
                            NULL,
                            WFCM_EVT_TASK_PRIORITY,
                            &wfcm_evt_handle) != 0 )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return -1;
    }

	return 0;
}

void wfcm_stub_deinit(void)
{
    T_WFC_OPCODE delete_cmd;
    T_WFC_EVT    delete_evt;

    os_memset(&delete_cmd, 0, sizeof(delete_cmd));
    os_memset(&delete_evt, 0, sizeof(delete_evt));
    delete_cmd.opcode = WFC_CMD_TASK_DEINIT;
    delete_cmd.pwfg = wfcm_get_config(delete_cmd.opcode);;
    delete_evt.eid = WFC_EVT_TASK_DEINIT;
            
    if ( wifi_os_queue_send(wfcm_queue, &delete_cmd, 0) != 0 )
    {
        printf("Send delete command fail.\r\n");
    }
    
    if ( wifi_os_queue_send(wfcm_evt_queue, &delete_evt, 0) != 0 )
    {
        printf("Send delete evt fail.\r\n");
    }
    
    wfcm_if_deinit(); //deinit SDIO
    hal_eint_deinit(WFC_HRX_EINT);
}

