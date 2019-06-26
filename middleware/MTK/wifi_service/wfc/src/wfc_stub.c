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
#include "queue.h"
#include "sys_init.h"

#include "wifi_api.h"
#include "wifi_private_api.h"
#include "wifi_os_api.h"
#include "wfc_at_api.h"

#include "wfc_stub.h"
#include "wfc_sdio.h"

#include "lwip/pbuf.h"

#include "hal_gpt.h"
#include "hal_gpio_internal.h"
#include "hal_gpio.h"
#include "task_def.h"

#include "bwcs_wifi.h"
#include "os.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "wifi_init.h"
#endif

#define WFC_STUB_TASK_NAME        "wfc"
#define WFC_STUB_TASK_STACK_SIZE   512
#define WFC_STUB_TASK_PRIORITY       4

//#define WFC_HIGH_PERFORMANCE_FLOW_ENABLE
/*
#define DTH_STS_SET_SIZE    0
#define DTH_STS_SEND_DATA   1
*/

//static TaskHandle_t  wfc_stub_task_handle  = NULL;
static TaskHandle_t  wfc_queue_task_handle = NULL;
static TaskHandle_t  wfc_mq_task_handle    = NULL;
static QueueHandle_t wfc_queue             = NULL;
//static QueueHandle_t wfc_ip_queue          = NULL;
static QueueHandle_t wfc_memq_queue        = NULL;

//uint32_t g_dth_size  = 0;

extern os_semaphore_t g_sem_qbuf;
extern T_WFC_QBUF  *g_hrx_qbuf;
extern unsigned char g_ip_ready;

/*the semaphore to get wifi host setting,  then start wifi_init in device*/
os_semaphore_t g_sem_wfc_host_cfg = NULL;

void wfc_rx_notify(void);

#ifdef MTK_WFC_AT_SLAVE_ENABLE
void wfc_at_process(uint8_t *pcmd,  uint32_t cmdSize);
#endif

//#if 0
//uint8_t _cli_iperf_remote_server(void);
//uint8_t _cli_iperf_remote_client(void);
//#endif

void wfc_stub_handler(T_WFC_OPCODE *pwfc);

struct netif *netif_find_by_type(uint32_t netif_type);
void ethernetif_intr_enhance_mode_dispatch(struct pbuf *p, struct netif *netif);
int wifi_firmware_is_ready(void);
unsigned char send_txdata_to_wifi_thread(void *pbuffer, struct netif * netif);

log_create_module(wfc, PRINT_LEVEL_INFO);

T_WFC_CONFIG g_wfc_stub_opcode[] =
{
    //WFC System Cmd
    {WFC_CT_SYSTEM, WFC_CMD_DTH_READY,         false},   //Need wait HRX data size
    {WFC_CT_SYSTEM, WFC_CMD_DTW_DATA,          false},
#ifdef HAL_SLEEP_MANAGER_ENABLED
    {WFC_CT_SYSTEM, WFC_CMD_WAKEUP_ACK,        false},
#endif

    //WiFi Configure Cmd
    {WFC_CT_USER, WFC_CMD_WFC_READY,           false},
    {WFC_CT_USER, WFC_CMD_OPMODE,              true},
    {WFC_CT_USER, WFC_CMD_STA_SSID,            true},
    {WFC_CT_USER, WFC_CMD_STA_PASSWD,          true},
    {WFC_CT_USER, WFC_CMD_RELOAD,              true},

    //OS Cmd
    {WFC_CT_USER, WFC_CMD_IP_READY,            false},
    {WFC_CT_USER, WFC_CMD_OS_INFO,             true},

    //Platform Cmd
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF,        true},
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF_CLIENT, true},

    //AT Cmd
    #ifdef MTK_WFC_AT_SLAVE_ENABLE
    {WFC_CT_USER, WFC_CMD_AT,                  false},
    #endif

    {WFC_CT_USER, WFC_CMD_API,                  false},
    {WFC_CT_USER, WFC_CMD_BWCS,	                false},

    {WFC_CT_USER, WFC_CMD_MAX_SUPPORT,         true},
};


struct pbuf *wfc_sim_pbuf_alloc(uint32_t pbufSize)
{

    #if 0 //pass lwip
    return (uint8_t *)wifi_os_malloc(pbufSize);
    #else
    //return pbuf_alloc(PBUF_RAW, pbufSize, PBUF_RAM);
    return pbuf_alloc(PBUF_RAW, pbufSize, PBUF_POOL);
    #endif
}


void wfc_host_tx_cb(struct pbuf *p, uint32_t bufSize)
{
    //printf("host lwip -> S(%d)\r\n", bufSize);
    //wfc_dump_buf(p->payload, bufSize);

    struct netif *sta_if;
    sta_if = netif_find_by_type(2);  //NETIF_TYPE_STA = 2

    send_txdata_to_wifi_thread((void *)p, sta_if);
}

void wfc_if_init(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    wfc_if_spi_init();
	#else
	    wfc_if_sdio_init();
	#endif
}


void wfc_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize)
{
	#ifdef MTK_WIFI_STUB_CONF_SPI_ENABLE
	    wfc_spi_send_response(pwfc, rspcode, txptr, txsize, hsize);
	#else
	    wfc_sdio_send_response(pwfc, rspcode, txptr, txsize, hsize);
	#endif
}


void wfc_send_data(uint8_t *txptr, uint32_t txsize)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    wfc_spi_send_data(txptr, txsize);
	#else
	    wfc_sdio_send_data(txptr, txsize);
	#endif
}

/*
uint8_t wfc_req_to_send(uint16_t sts)
{
	if (sts == DTH_STS_SET_SIZE)
	{
	    wifi_os_semphr_take(g_sem_dth_size, NULL);
	}
	else if (sts == DTH_STS_SEND_DATA)
	{
		wifi_os_semphr_take(g_sem_dth_send, NULL);
    }

	return true;
}

uint8_t wfc_clr_to_send( uint16_t sts )
{
	if (sts == DTH_STS_SET_SIZE)
	{
	    wifi_os_semphr_give(g_sem_dth_size);
    }
    else if (sts == DTH_STS_SEND_DATA)
    {
    	wifi_os_semphr_give(g_sem_dth_send);
    }

	return true;
}
*/


void wfc_free_opcode(T_WFC_OPCODE *pwfc)
{
    if (pwfc && pwfc->rxptr)
    {
        wifi_os_free(pwfc->rxptr);
        pwfc->rxptr = NULL;
    }
}


T_WFC_CONFIG *wfc_get_config(uint16_t opcode)
{
    int            idx = 0;
    T_WFC_CONFIG  *pstub;

    pstub = &g_wfc_stub_opcode[idx];

    while (pstub->op != WFC_CMD_MAX_SUPPORT)
    {
        if (pstub->op == opcode )
        {
            return pstub;
        }
        pstub = &g_wfc_stub_opcode[++idx];
    }

    return NULL;
}


void wfc_data_free(struct pbuf *p)
{
	pbuf_free(p);
}


uint8_t wfc_mq_send_queue(T_WFC_QBUF *qbuf, uint32_t fromISR)
{
    T_WFC_QBUF qBuf;

    os_memcpy(&qBuf, qbuf, sizeof(T_WFC_QBUF));

    // Send to WFC mem Queue
    if (fromISR)
    {
    	if ( wifi_os_queue_send_from_isr(wfc_memq_queue, &qBuf) != 0 )
    		return false;
    }
    else if ( wifi_os_queue_send(wfc_memq_queue, &qBuf, 0) != 0 )
    {
        return false;
    }

    return true;
}

uint8_t g_wfc_init_status = false;
void wfc_set_init_status(uint8_t status){
    g_wfc_init_status = status;
}

uint8_t wfc_get_init_status(){
    return g_wfc_init_status;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern uint32_t wfc_sleep_lock;
#endif
void wfc_memq_handler(void)
{
    uint8_t       bExit = false;
    struct pbuf  *p;
    T_WFC_QBUF    qBuf;
    uint32_t      dSize;

    printf("[PF]enter memq handler.\r\n");
    wfc_memq_queue =  wifi_os_queue_create(WFC_MQ_SIZE, sizeof(T_WFC_QBUF));
    if (wfc_memq_queue==NULL)
    {
        vTaskDelete(NULL);
        return;
    }

    wfc_if_init();

    wfc_set_init_status(true);

    while (!bExit)
    {
        if (wifi_os_queue_receive(wfc_memq_queue, &qBuf, NULL) == 0 )
        {
            uint8_t isDTW = 0;

            wfc_mq_opcmd_process(&qBuf, &isDTW);

            if (isDTW)
            {
                dSize = qBuf.size-4;
                //TBD: Need modify for pbuf and os_mem
                p = pbuf_alloc(PBUF_RAW, dSize, PBUF_POOL);
                //p = pbuf_alloc(PBUF_RAW, dSize, PBUF_RAM);

                if (p!=NULL)
                {
                    //printf("[HTX]: SDIO buffer -> Payload(%p)\r\n",p->payload);
#ifdef  MTK_WIFI_STUB_CONF_SPI_ENABLE
                    wfc_spi_htx_cp(&qBuf, p->payload, 4); //take 135us
#else
                    wfc_sido_htx_cp(&qBuf, p->payload, 4); //take 135us
#endif
                }

                //Free HTX Queue
                //printf("m(%d,%d)\r\n",qBuf.ctrl, qBuf.qidx);
                wfc_mq_free(&qBuf);

#ifndef  MTK_WIFI_STUB_CONF_SPI_ENABLE
                //Set HTX Buffer for Next HTX
                wfc_sdio_set_htx_buf();
#endif

                //printf("[HTX]: Send -> lwip\r\n");
                if (p!=NULL)
                {
                    wfc_host_tx_cb(p, dSize);
                }
            }
            else
            {
                //Free HTX Queue
                wfc_mq_free(&qBuf);

#ifndef  MTK_WIFI_STUB_CONF_SPI_ENABLE
                //Set HTX Buffer for Next HTX
                wfc_sdio_set_htx_buf();
#endif
            }
        }
    }

    vTaskDelete(NULL);
}


uint8_t wfc_send_queue(uint16_t op, uint16_t cmd, uint8_t *ptr, uint32_t ptrSize, uint8_t fromISR)
{
	T_WFC_OPCODE  wfc;

	wfc.opcode = op;
	wfc.opcmd  = cmd;
	wfc.rxptr  = ptr;
	wfc.rxsize = ptrSize;

    // Send to WFC Queue
    while (!wfc_queue)
    {
       hal_gpt_delay_ms(1);
    }

    if (fromISR)
    {
    	if ( wifi_os_queue_send_from_isr(wfc_queue, &wfc) != 0 )
    		return false;
    }
    else if ( wifi_os_queue_send(wfc_queue, &wfc, 0) != 0 )
    {
        return false;
    }

    return true;
}

uint8_t wfc_send_to_host(uint8_t *pbuf, uint32_t bufSize, uint8_t isEvt)
{
    T_WFC_QBUF  *qbuf;

#ifdef HAL_SLEEP_MANAGER_ENABLED
            if(wfc_sleep_lock != 0xff) {
                wifi_lock_sleep(wfc_sleep_lock);
            }
#endif

    if (!wfc_queue || !g_sem_qbuf)
    {
    	printf("[WFC]: To Host Fail - %x, %x\r\n", (unsigned int)wfc_queue, (unsigned int)g_sem_qbuf);
    	return false;
    }

    qbuf = (T_WFC_QBUF *)wifi_os_malloc(sizeof(T_WFC_QBUF));

    if (isEvt)
    {
#ifdef  MTK_WIFI_STUB_CONF_SPI_ENABLE
        wfc_spi_hevt_cp(qbuf, pbuf, bufSize); //Copy Host Event to Host RX Buffer
#else
        // Copy Host Event Notify to HRX buffer
        wfc_sdio_hevt_cp(qbuf, pbuf, bufSize); //Copy Host Event to Host RX Buffer
#endif
    }
    else
    {
        pbuf_ref((struct pbuf *)pbuf);

        qbuf->ctrl = MEMQ_CTRL_HOST_RX;
        qbuf->qidx = 0; // it is not used for MEMQ_CTRL_HOST_RX, just give a initial setting.
        qbuf->size = bufSize;
        qbuf->ptr  = pbuf;

        // Copy Host RX data into HRX buffer
        //wfc_sdio_hrx_cp(qbuf, pbuf, bufSize); //Copy Host RX Data to SDIO RX Buffer

        wfc_data_free((struct pbuf *)pbuf);   //at ip<->ip, it will free by lwip ???
    }

    // Push to HRX Queue List
    wfc_mq_push_hrx(qbuf);

#if 1 //[PF] new flow
        if(wfc_send_queue(WFC_OP_HOST_CMD, WFC_CMD_DTH_READY, NULL, 0, 0) == false)
        {
           printf("%s,%d,  wfc send queue fail !\n",__FUNCTION__,__LINE__);
        } else {
            //printf("enqueue success\n");
        }
#endif

    return true;
}

void wfc_stub_handler(T_WFC_OPCODE *pwfc)
{
    uint8_t  *param = NULL;

    T_WFC_CONFIG  *pwfg = NULL;


    //printf("OpCode(%d), OpCmd(%d)\r\n", pwfc->opcode, pwfc->opcmd);

    // Get wifi configure opcode information
    pwfg = wfc_get_config(pwfc->opcmd);
    if (!pwfg)
    {
        wfc_send_response(pwfc, WFC_RSP_INVALID_OPCODE, NULL, 0, 0);
        //wfc_send_response(pwfc, WFC_RSP_INVALID_OPCODE, NULL, 0, g_dth_size);
        return;
    }

    if ((pwfc->opcmd != WFC_CMD_DTW_DATA) && pwfc->rxsize)
    {
        //printf("Cmd Qidx(%d), Size(%d)\r\n",(uint32_t)pwfc->rxptr,pwfc->rxsize);
        param = pwfc->rxptr;
    }

    switch(pwfc->opcmd)
    {
        case WFC_CMD_DTH_READY: //Host Receive Buffer Ready
        {
            g_hrx_qbuf = wfc_mq_pop_hrx();
            if ( g_hrx_qbuf == NULL )
            {
      	        //printf("[HRX]: Error - No HRX Data to Send\r\n");
      	        //wfc_send_response(pwfc, WFC_RSP_NO_HRX, NULL, 0, 0);
                break;
            }
            wfc_rx_notify();
#ifdef WFC_HIGH_PERFORMANCE_FLOW_ENABLE
            wfc_send_data(pwfc->rxptr, pwfc->rxsize);  //pwfc->rxptr, pwfc->rxsize not used
#else
            //printf("[HRX]: Size(%d)\r\n",g_hrx_qbuf->size);
            wfc_send_response(pwfc, WFC_RSP_OK, NULL, 0, g_hrx_qbuf->size);
            wfc_send_data(pwfc->rxptr, pwfc->rxsize);  //pwfc->rxptr, pwfc->rxsize not used
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if(wfc_sleep_lock != 0xff) {
                wifi_unlock_sleep(wfc_sleep_lock);
            }
#endif
            break;
        }

#ifdef HAL_SLEEP_MANAGER_ENABLED
        case WFC_CMD_WAKEUP_ACK:
        {
            wfc_rx_notify();
            wfc_send_response(pwfc, WFC_RSP_OK, NULL, 0, 0);  //send wake up response;
            break;
        }
#endif
        //WiFi Configure Cmd
        case WFC_CMD_WFC_READY:
        {
            uint8_t is_ready;
            is_ready = (wfc_get_init_status()?1:0);
            printf( "<WFC> - is_ready(%d)\r\n",is_ready);
            wfc_send_response(pwfc, WFC_RSP_OK, (uint8_t *)&is_ready, 1, 0);
            break;
        }
        case WFC_CMD_API:
        {
            uint8_t       rspbuf[256];
            uint32_t      rspSize;
            uint16_t      rspcode;
            param = pwfc->rxptr;
            int32_t ret = wfc_api_process(param, pwfc->rxsize, rspbuf, &rspSize);
            rspcode = ret >= 0 ? WFC_RSP_OK : WFC_RSP_UNKNOWN_ERR;
            wfc_send_response(pwfc, rspcode, rspbuf, rspSize, 0);
            break;
        }
        //OS Cmd
        case WFC_CMD_IP_READY:
        {
            //printf("<WFC> - g_ip_ready(%d)\r\n",g_ip_ready);
            wfc_send_response(pwfc, WFC_RSP_OK, &g_ip_ready, 1, 0);
            break;
        }
        case WFC_CMD_OS_INFO:
        {
            printf( "heap:\n");
            printf( "\ttotal: %u\n", configTOTAL_HEAP_SIZE);
            printf( "\tfree:  %u\n", xPortGetFreeHeapSize());
            printf( "\tlow:   %u\n", xPortGetMinimumEverFreeHeapSize());
            printf( "\r\n\r\n");
            wifi_connection_inform_ip_ready();
            break;
        }

        //Platform Cmd
        //#if 0
        //case WFC_CMD_REMOTE_IPERF:
        //{
        //    _cli_iperf_remote_server();
        //    break;
        //}
        //case WFC_CMD_REMOTE_IPERF_CLIENT:
        //{
        //    //hal_gpt_delay_ms(3000);
        //    _cli_iperf_remote_client();
        //    break;
        //}
        //#endif

        //AT Cmd
        #ifdef MTK_WFC_AT_SLAVE_ENABLE
        case WFC_CMD_AT:
        {
            if(param != NULL) {
                wfc_at_process(param, pwfc->rxsize);
                wfc_send_response(pwfc, WFC_RSP_OK, NULL, 0, 0);
            }
            //wfc_send_response(pwfc, WFC_RSP_OK, NULL, 0, g_dth_size);
            break;
        }
        #endif	
        case WFC_CMD_BWCS:
        {
            uint16_t      rspcode;
            param = pwfc->rxptr;
            int32_t ret = bwcs_to_wifi_setting_param((bwcs_req_struct *)param);
            rspcode = ret >= 0 ? WFC_RSP_OK : WFC_RSP_UNKNOWN_ERR;
            wfc_send_response(pwfc, rspcode, NULL, 0, 0);
            break;
        }
        default:
            break;
    }

    // Send WiFi Configure Response Code (Auto Response)
    if ( pwfg->auto_rsp )
    {
        wfc_send_response(pwfc, WFC_RSP_OK, NULL, 0, 0);
        //wfc_send_response(pwfc, WFC_RSP_OK, NULL, 0, g_dth_size);
    }

    // Release OpCode Tx Buffer
    if (pwfc->opcmd != WFC_CMD_DTW_DATA )
    {
        //printf("%s,%d, free pwfc=0x%x, pwfc->rxptr=0x%x\n",__FUNCTION__,__LINE__,  pwfc, pwfc->rxptr);
        wfc_free_opcode(pwfc);
    }
}


void wfc_queue_handler(void)
{
    uint8_t       bExit = false;
    T_WFC_OPCODE  wfcq;


    wfc_queue =  wifi_os_queue_create(WFC_QUEUE_SIZE, sizeof(T_WFC_OPCODE));
    if (wfc_queue == NULL)
    {
        vTaskDelete(NULL);
        return;
    }

    while (!bExit)
    {
        if (wifi_os_queue_receive(wfc_queue, &wfcq, NULL) == 0)
        {
            // OpCode Process
            switch (wfcq.opcode)
            {
                case WFC_OP_HOST_CMD:
                {
                    wfc_stub_handler(&wfcq);
                    //Prepare HTX Buffer for next Host TX
                    //wfc_sdio_set_htx_buf();
                    break;
                }
                //case WFC_OP_DTH_SET: //Set Host Buffer Size
                //{
                //    //printf( "<WFC> - DTH Size(%d)\r\n",*((uint32_t*)pwfc->rxptr));
                //    g_dth_size = *((uint32_t*)pwfc->rxptr);
                //    break;
                //}
                //case WFC_OP_DTH_DATA:
                //{
                //    wfc_send_data(wfcq.rxptr, wfcq.rxsize);
                //    break;
                //}
                default:
                {
                    //printf( "<WFC>:%s-%d\n",__FUNCTION__,__LINE__);
                    break;
                }
            }
        }
    }

    vTaskDelete(NULL);
}

#if 0
static void wfc_queue_task_init(void)
{
    /*wfc memq is for data path Tx from Host to Device WiFi Stack*/
    if ( xTaskCreate((TaskFunction_t)wfc_memq_handler,
                    WFC_MQ_TASK_NAME,
                    WFC_MQ_TASK_STACK_SIZE,
                    NULL,
                    WFC_MQ_TASK_PRIORITY,
                    &wfc_mq_task_handle) != pdPASS )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }

    /*wfc queue is for data path Rx from Device WiFi Stack/Lwip to Host*/
    if ( xTaskCreate((TaskFunction_t)wfc_queue_handler,
                    WFC_QUEUE_TASK_NAME,
                    WFC_QUEUE_TASK_STACK_SIZE,
                    NULL,
                    WFC_QUEUE_TASK_PRIORITY,
                    &wfc_queue_task_handle) != pdPASS )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }
}

static void wfc_stub_task(void)
{
    // Wifi configure queue task init (Opcode Process)
    wfc_queue_task_init();

    // Wifi configure interface init (SPI/SDIO/....)
    wfc_if_init();

    while ( !wfc_queue )
    {
       hal_gpt_delay_ms(1000);
    }

    vTaskDelete(NULL);
}
#endif

void wfc_stub_init(void)
{
    /*the semaphore to get wifi host setting,  then start wifi_init in device*/
    g_sem_wfc_host_cfg = wifi_os_semphr_create_binary();

    wfc_set_init_status(false);

    // Wifi configure queue task init (Opcode Process)
    /*wfc memq is for data path Tx from Host to Device WiFi Stack*/
    if ( wifi_os_task_create((TaskFunction_t)wfc_memq_handler,
                            WFC_MQ_TASK_NAME,
                            WFC_MQ_TASK_STACK_SIZE*4,
                            NULL,
                            WFC_MQ_TASK_PRIORITY,
                            &wfc_mq_task_handle) != 0 )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }

    /*wfc queue is for data path Rx from Device WiFi Stack/Lwip to Host*/
    if ( wifi_os_task_create((TaskFunction_t)wfc_queue_handler,
                            WFC_QUEUE_TASK_NAME,
                            WFC_QUEUE_TASK_STACK_SIZE*4,
                            NULL,
                            WFC_QUEUE_TASK_PRIORITY,
                            &wfc_queue_task_handle) != 0 )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }

#if 0
    if ( xTaskCreate((TaskFunction_t)wfc_stub_task, WFC_STUB_TASK_NAME, WFC_STUB_TASK_STACK_SIZE, NULL, WFC_STUB_TASK_PRIORITY, &wfc_stub_task_handle) != pdPASS )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }
#endif
	return;
}
