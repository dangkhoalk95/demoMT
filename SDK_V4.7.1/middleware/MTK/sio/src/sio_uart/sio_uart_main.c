/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "syslog.h"
#include "queue.h"
#include "task.h"
#include "task_def.h"

#include "sio_uart_internal.h"
#include "sio_gprot.h"
#include "hal_uart.h"

#include "sio_uart_port.h"
#include "sio_uart_adapter.h"
#include "msm.h"
#include "hal_sleep_manager.h"

#ifdef SIO_UART_MAIN_PRINTF
#define LOGE(fmt,arg...)   printf(("[SIO HANDLE]: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("[SIO HANDLE]: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("[SIO HANDLE]: "fmt), ##arg)
#else
log_create_module(sio_uart, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(sio_uart, "[SIO HANDLE]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(sio_uart, "[SIO HANDLE]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(sio_uart ,"[SIO HANDLE]: "fmt,##arg)
#endif

#if XA_CONTEXT_PTR == XA_ENABLED
sio_uart_context_struct  sio_uart_temp;
sio_uart_context_struct *sio_uart_context = &sio_uart_temp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
sio_uart_context_struct  sio_uart_context;
#endif /* XA_CONTEXT_PTR */

void sio_uart_process(void *arg);
sio_uart_ret_t sio_uart_local_init();
sio_uart_ret_t sio_uart_eint_modem_exception_handler(void);

TimerHandle_t g_sio_uart_timer_modem_exception_handle = NULL;
TimerHandle_t g_sio_uart_timer_ut_handle = NULL;
TimerHandle_t g_sio_uart_timer_wait_sleep_handle = NULL;
TimerHandle_t g_sio_uart_timer_wait_response_handle = NULL;
TimerHandle_t g_sio_uart_timer_wait_ready_handle = NULL;

static uint8_t g_sio_uart_send_buf[SIO_UART_RX_FIFO_BUFFER_SIZE] = {0};
static uint32_t g_sio_uart_send_buf_length = 0;
static uint8_t g_sio_uart_resend_count = 0;
extern void sio_emit_uart_data(char *data, int len);

static void
sio_uart_send_wait_response_timeout_callback( )
{
    LOGW("sio_uart_send_wait_response_timeout_callback");  
    
    sio_uart_timer_stop(SIO_UART_TIMER_WAIT_RESPONSE);

    if (g_sio_uart_send_buf_length > 0 && (g_sio_uart_send_buf!=NULL)) {

        if (g_sio_uart_resend_count > 10) {
            char* ERROR_BUF = "ERROR\r\n";
            LOGI("~~~~~~~timeout, return ERROR ~~~~~~");
            sio_emit_uart_data(ERROR_BUF, strlen(ERROR_BUF));
        } else {
            // resend ATCMD
            g_sio_uart_resend_count++;
            sio_uart_port_write_data(SIO_UART(modem_port), (uint8_t*)g_sio_uart_send_buf, g_sio_uart_send_buf_length);
            LOGI("~~~~~~~timeout, resend send count: %d buf: %s~~~~~~~~~~", g_sio_uart_resend_count, g_sio_uart_send_buf);
        }
    }
    if (g_sio_uart_timer_wait_sleep_handle) {
        sio_uart_timer_create_start(SIO_UART_TIMER_WAIT_SLEEP);
    }
}

static void
sio_uart_send_wait_sleep_timeout_callback( )
{
    LOGW("sio_uart_send_wait_sleep_timeout_callback"); 

    if (g_sio_uart_timer_wait_response_handle != 0) {        
        sio_uart_timer_create_start(SIO_UART_TIMER_WAIT_SLEEP);
    } else {
        sio_uart_timer_stop(SIO_UART_TIMER_WAIT_SLEEP);
        sio_uart_trigger_modem_sleep();
    }
}

int32_t sio_uart_msm_event_handler(msm_event_t event, void *data)
{
    sio_uart_ret_t ret = SIO_UART_RET_OK;
    sio_uart_general_msg_t msg_queue_item;
    if(event == MSM_EVENT_EXCETION) {
        msg_queue_item.msg_id = MSG_ID_SIO_EINT_MODEM_EXCEPTION_IND;
        msg_queue_item.msg_data = NULL;
        LOGI("msm_eint_callback, send message(%d) to MSM task \r\n", msg_queue_item.msg_id);
        sio_uart_queue_send_from_isr(SIO_UART(input_queue), (void*)&msg_queue_item);
    } else if (event == MSM_EVENT_WAKEUP) {
        msg_queue_item.msg_id = MSG_ID_SIO_EINT_MODEM_WAKEUP_IND;
        msg_queue_item.msg_data = NULL;
        LOGI("msm_eint_callback, send message(%d) to MSM task \r\n", msg_queue_item.msg_id);
        sio_uart_queue_send_from_isr(SIO_UART(input_queue), (void*)&msg_queue_item);
    } else {
        ret = SIO_UART_RET_ERROR;
    }
    return ret;
}


sio_uart_ret_t sio_uart_init(hal_uart_port_t uart_port)
{
    sio_uart_ret_t ret = SIO_UART_RET_ERROR;
    TaskHandle_t task_handle = NULL;

    if (task_handle == NULL) {
        xTaskCreate(
            sio_uart_process,
            SIO_UART_THREAD_NAME,
            SIO_UART_THREAD_STACKSIZE / sizeof(portSTACK_TYPE),
            NULL,
            SIO_UART_THREAD_PRIO,
            &task_handle);

        if (task_handle == NULL) {
            LOGW("sio_uart_init() error. \r\n");
            return SIO_UART_RET_ERROR;
        }
    } else {
        LOGW("sio_uart_init() has been ready. \r\n");
        return SIO_UART_RET_OK;
    }

    if (SIO_UART_RET_OK == sio_uart_local_init() && SIO_UART_RET_OK == sio_uart_port_init(uart_port)){
        ret = SIO_UART_RET_OK;
    }

    if (MSM_RET_OK == msm_register_callback(sio_uart_msm_event_handler)) {
        ret = SIO_UART_RET_OK;    }


    if (ret == SIO_UART_RET_OK) {
        LOGI("sio_uart_init() success \r\n");
        sio_uart_set_status(SIO_UART_STATUS_INIT);
    } else {
        LOGW("sio_uart_init() fail \r\n");
        ret = SIO_UART_RET_ERROR;
        sio_uart_set_status(SIO_UART_STATUS_EXCEPTION);
#ifdef SIO_DEBUG
        configASSERT(0);
#endif
    }

    return ret;
}

sio_uart_ret_t sio_uart_set_uart_event_callback(sio_uart_uart_event_callback event_callback)
{
    sio_uart_event_callback_struct *h = SIO_UART(uart_event_cb_list);
    sio_uart_event_callback_struct *p = h;

    while (p) {
        p = p->next;
    }

    if (p == NULL) {
        sio_uart_event_callback_struct *n = (sio_uart_event_callback_struct *)sio_uart_mem_alloc(sizeof(sio_uart_event_callback_struct));
        if (n == NULL) {
            LOGI("uart event register failed callback=%p \n", event_callback);
            return SIO_UART_RET_REGISTER_FAIL;
        }
        n->func = event_callback;
        n->next = SIO_UART(uart_event_cb_list);
        SIO_UART(uart_event_cb_list) = n;
        LOGI("register success  callback=%p \n", event_callback);
    }
    
    return SIO_UART_RET_OK;
}


sio_uart_ret_t sio_uart_set_notification_callback(uint8_t enabled, int32_t app_id,
    sio_uart_event_t events, sio_uart_notifier_callback callback)
{
    sio_uart_notifier_callback_struct *h = SIO_UART(notifier_cb_list);
    sio_uart_notifier_callback_struct *p = h;
    sio_uart_notifier_callback_struct *prev = h; 

    if (enabled) {
        while (p) {
            if (p->app_id == app_id && p->func == callback) {
                p->events = events;
                break;
            }
            prev = p;
            p = p->next;
        }

        if (p == NULL) {
            sio_uart_notifier_callback_struct *n = (sio_uart_notifier_callback_struct *)sio_uart_mem_alloc(sizeof(sio_uart_notifier_callback_struct));
            if (n == NULL) {
                LOGI("register failed app_id=%d, callback=%p \n", app_id, callback);
                return SIO_UART_RET_REGISTER_FAIL;
            }
            n->events = events;
            n->app_id = app_id;
            n->func = callback;
            n->next = SIO_UART(notifier_cb_list);
            SIO_UART(notifier_cb_list) = n;
            LOGI("register success app_id=%d, callback=%p \n", app_id, callback);
        }
    } else { 
        while (p) {
            if ((p->func == callback) && (p->events & events)) {
                break;
            }
            prev = p;
            p = p->next;
        }

        if (p) {
            if (p->events != events) {
                p->events &= ~events;
                return SIO_UART_RET_OK;
            }
            prev->next = p->next;
            if (p == SIO_UART(notifier_cb_list)) {
                if (p->next == NULL) {
                    SIO_UART(notifier_cb_list) = NULL;
                } else {
                    SIO_UART(notifier_cb_list) = p->next;
                }
            }
            sio_uart_mem_free(p);
            p = NULL;
        } else {
            LOGW("No match found.\n");
            return SIO_UART_RET_UNREGISTER_FAIL;
        }
    }

    if (sio_uart_get_status() == SIO_UART_STATUS_READY) {
        LOGI("modem is ready");
        callback(SIO_UART_EVENT_MODEM_READY, NULL);
    }

    return SIO_UART_RET_OK;
}

void sio_uart_process(void *arg)
{
    sio_uart_general_msg_t msg_queue_data;

    sio_uart_timer_create_start(SIO_UART_TIMER_WAIT_READY);

    msm_notify_host_status_to_modem(MSM_STATUS_ACTIVE);

    sio_uart_trigger_modem_wakeup();

    for (;;) {
        if (xQueueReceive((QueueHandle_t)(SIO_UART(input_queue)), &msg_queue_data, portMAX_DELAY) == pdPASS) {

            LOGI("sio_uart_process() handle input message, msg type = %d\r\n", msg_queue_data.msg_id);

            if (MSG_ID_SIO_UART_IND == msg_queue_data.msg_id) {
                sio_uart_port_msg_handler((sio_uart_input_cmd_msg_t *)(msg_queue_data.msg_data));
            } else if (MSG_ID_SIO_EINT_MODEM_EXCEPTION_IND == msg_queue_data.msg_id) {
                sio_uart_eint_modem_exception_handler();
            } else if (MSG_ID_SIO_EINT_MODEM_WAKEUP_IND == msg_queue_data.msg_id) {
                sio_uart_eint_modem_wakeup_handler();
            } else if (MSG_ID_SIO_MODEM_READY_IND == msg_queue_data.msg_id) {                
                sio_uart_mutex_lock(SIO_UART(mutex));               
                sio_uart_set_status(SIO_UART_STATUS_READY);
                sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_READY, NULL);
                sio_uart_mutex_unlock(SIO_UART(mutex));
            }
        }
    }
}

sio_uart_ret_t sio_uart_context_init(void)
{
    uint8_t *ptr;

#if XA_CONTEXT_PTR == XA_ENABLED
    ptr = (uint8_t *)sio_uart_context;
#else
    ptr = (uint8_t *)&sio_uart_context;
#endif /* XA_CONTEXT_PTR */

    memset(ptr, 0, (uint32_t)sizeof(sio_uart_context_struct));

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_local_init()
{
    sio_uart_ret_t ret =  SIO_UART_RET_ERROR;

    if (SIO_UART_RET_OK != sio_uart_context_init()) {
        return ret;
    }

    SIO_UART(mutex) = sio_uart_mutex_create();
    if (NULL == (void *)SIO_UART(mutex)) {
        return ret;
    }

    SIO_UART(semaphore) = sio_uart_semaphore_create(10, 0);
    if (NULL == (void *)SIO_UART(semaphore)) {
        return ret;
    }

    SIO_UART(input_queue) = sio_uart_queue_create(SIO_UART_LOCAL_QUEUE_LENGTH, sizeof(sio_uart_general_msg_t));
    if (NULL == (void *)SIO_UART(input_queue)) {
        return ret;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    SIO_UART(sleep_manager_handler) = hal_sleep_manager_set_sleep_handle("sio_uart");
#endif

    LOGI("sio_uart_local_init() success \r\n");
    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_at_response_handler(uint8_t *payload, uint32_t length, uint8_t is_end)
{
    sio_rx_data_to_read_struct rx_data;
    sio_uart_rtx_data_struct *n;   

    n = SIO_UART(rtx_data);
    if (NULL == n) {
        LOGW("tx_data is NULL, drop package\r\n");
        return SIO_UART_RET_ERROR;
    }

    sio_uart_timer_stop(SIO_UART_TIMER_WAIT_RESPONSE);

    memset(g_sio_uart_send_buf, 0, SIO_UART_RX_FIFO_BUFFER_SIZE);
    g_sio_uart_send_buf_length = 0;
    g_sio_uart_resend_count = 0;

    if (n->payload == NULL) {
        n->payload = payload;

        rx_data.app_id = n->app_id;
        rx_data.read_length = length;
    } else {
        if (strlen((const char*)(n->payload)) + length + strlen("\r\n") > SIO_UART_RX_FIFO_BUFFER_SIZE)
            configASSERT(0);

        memcpy(n->payload + strlen((const char*)(n->payload)), "\r\n", strlen("\r\n"));  
        memcpy(n->payload + strlen((const char*)(n->payload)), payload, length);
        rx_data.app_id = n->app_id;
        rx_data.read_length = strlen((const char*)(n->payload));
    }

    if (is_end) {
        sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_DATA_TO_READ, &rx_data); 
        return SIO_UART_RET_OK;       
    }

    return SIO_UART_RET_BUSY;
}

sio_uart_ret_t sio_uart_event_proxy_handler(sio_uart_event_t event, void *data)
{
    sio_uart_notifier_callback_struct *h = SIO_UART(notifier_cb_list);

    sio_uart_notifier_callback notifier_callback[SIO_MAX_NOTIFIER_CALLBACK_CNT];
    int32_t notifier_callback_cnt = 0;

    LOGI("sio_uart_event_proxy_handler() event = %d. h %d\r\n", event, h);

    while (h) {
        if (event == SIO_UART_EVENT_MODEM_DATA_TO_READ) {
            uint32_t app_id;
            sio_rx_data_to_read_struct *rx_data;
            rx_data = (sio_rx_data_to_read_struct *)data;
            app_id = rx_data->app_id;
            LOGI("sio_uart_event_proxy_handler() app_id = %d \r\n", app_id);

            if (h->app_id == app_id) {
                notifier_callback[notifier_callback_cnt] = h->func;
                notifier_callback_cnt++;
            }
         } else {
            notifier_callback[notifier_callback_cnt] = h->func;
            notifier_callback_cnt++;
        }
        h = h->next;
    }

    configASSERT(notifier_callback_cnt <= SIO_MAX_NOTIFIER_CALLBACK_CNT);
    LOGI("--notifier_callback_cnt = %d-- \r\n", notifier_callback_cnt);
    while (notifier_callback_cnt > 0) {
        notifier_callback_cnt--;
        LOGI("--begin notifier id = %d, ptr = %p -- \r\n", event, notifier_callback[notifier_callback_cnt]);
        notifier_callback[notifier_callback_cnt](event, data);
        LOGI("--finish notifier id = %d, ptr = %p -- \r\n", event, notifier_callback[notifier_callback_cnt]);
    }

    return SIO_UART_RET_OK;
}

void sio_uart_reset_send_data()
{
    if (SIO_UART(rtx_data)) {
        sio_uart_mem_free(SIO_UART(rtx_data));
        SIO_UART(rtx_data) = NULL;
    }
}

uint32_t sio_uart_send_data(int32_t app_id, const uint8_t *data, uint32_t size)
{
    uint32_t ret_len;
    sio_uart_rtx_data_struct *n;
    sio_uart_status_t status;
    
    sio_uart_mutex_lock(SIO_UART(mutex));

    n = SIO_UART(rtx_data);
    status = sio_uart_get_status();
    LOGI("[output] app id=%d, status = %d. \r\n", (int32_t)app_id, status);

    if (SIO_UART_STATUS_READY != status) {
        sio_uart_mutex_unlock(SIO_UART(mutex));
        return (uint32_t)SIO_UART_RET_MODEM_NO_READY;
    }
    
    if (SIO_UART(rtx_data) == NULL) {
        n = (sio_uart_rtx_data_struct *)sio_uart_mem_alloc(sizeof(sio_uart_rtx_data_struct));
        SIO_UART(rtx_data) = n;
        
        if (MSM_STATUS_INACTIVE == msm_query_status_from_modem()) {
        #ifdef MODEM_SUPPORT_DTE  
            hal_uart_set_dte_dtr_active(sio_uart_get_modem_port());
        #else
            if (MSM_STATUS_INACTIVE == msm_query_status_from_modem()) {
                sio_uart_trigger_modem_wakeup();
            }

            while (MSM_STATUS_INACTIVE == msm_query_status_from_modem()) {
                vTaskDelay(10); //delay 10ms for waiting
                #ifdef SIO_DEBUG
                    configASSERT(0);
                #else
                    LOGW("wakeup modem timeout, so reset modem. \r\n");
                    msm_trigger_to_modem(MSM_TRIGGER_TYPE_RESET);
                    sio_uart_set_status(SIO_UART_STATUS_EXCEPTION);
                    sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_ABNORMAL, NULL);
                    sio_uart_timer_create_start(SIO_UART_TIMER_MODEM_EXCEPTION);
                    sio_uart_mutex_unlock(SIO_UART(mutex));
                    return SIO_UART_RET_ERROR;
                #endif
            }

        #endif
        }
        sio_uart_timer_create_start(SIO_UART_TIMER_WAIT_SLEEP);
        ret_len = sio_uart_port_write_data(SIO_UART(modem_port), (uint8_t*)data, size);

        memset(g_sio_uart_send_buf, 0, SIO_UART_RX_FIFO_BUFFER_SIZE);
        g_sio_uart_send_buf_length = size;
        sio_copy_data((char*)g_sio_uart_send_buf, (char*)data, size);
        g_sio_uart_resend_count = 0;

        if (ret_len == size) {
            n->app_id = app_id;
            n->payload = NULL;
            sio_uart_timer_create_start(SIO_UART_TIMER_WAIT_RESPONSE);
        } else {
            sio_uart_mem_free(n);
            SIO_UART(rtx_data) = NULL;
            LOGW("sio_uart_send_data() fail. \r\n");
        }

        sio_uart_mutex_unlock(SIO_UART(mutex));
        return ret_len;
    } else {
        LOGW("sio_uart_send_data() busy, \r\n");
        sio_uart_mutex_unlock(SIO_UART(mutex));
        return (uint32_t)SIO_UART_RET_BUSY;
    }
}

uint32_t sio_uart_receive_data(int32_t app_id, uint8_t *buffer, uint32_t size)
{
    sio_uart_rtx_data_struct *n = SIO_UART(rtx_data);

    if (NULL == n) {
        LOGE("rtx_data is NULL, drop package\r\n");
        return SIO_UART_RET_OK;
    }

    if (n->app_id == app_id) {
        memcpy(buffer, n->payload, size);

        sio_uart_mem_free(SIO_UART(rtx_data));
        SIO_UART(rtx_data) = NULL;
    } else {
        size = 0;
        LOGE("app id fail n->app_id %d, app_id %d",n->app_id, app_id);
    }

    return size;
}


sio_uart_ret_t sio_uart_eint_modem_exception_handler(void)
{
    sio_uart_mutex_lock(SIO_UART(mutex));

    LOGW("sio_uart_eint_modem_exception_handler. \r\n");

    msm_trigger_to_modem(MSM_TRIGGER_TYPE_RESET);

    sio_uart_set_status(SIO_UART_STATUS_EXCEPTION);

    if (SIO_UART(rtx_data)) {
        sio_uart_mem_free(SIO_UART(rtx_data));
        SIO_UART(rtx_data) = NULL;
    }

    sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_ABNORMAL, NULL);

    sio_uart_mutex_unlock(SIO_UART(mutex));

    sio_uart_timer_create_start(SIO_UART_TIMER_MODEM_EXCEPTION);

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_eint_modem_wakeup_handler(void)
{
    sio_uart_mutex_lock(SIO_UART(mutex));

    LOGW("sio_uart_eint_modem_wakeup_handler. \r\n");

    sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_WAKEUP, NULL);

    sio_uart_mutex_unlock(SIO_UART(mutex));

    return SIO_UART_RET_OK;
}


sio_uart_ret_t sio_uart_eint_modem_sleep_handler(void)
{
    sio_uart_mutex_lock(SIO_UART(mutex));

    LOGW("sio_uart_eint_modem_sleep_handler. \r\n");

    sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_SLEEP, NULL);

    sio_uart_mutex_unlock(SIO_UART(mutex));

    return SIO_UART_RET_OK;
}


void sio_uart_register_urc_callback(sio_uart_urc_callback callback)
{
    SIO_UART(urc_cb) = callback;
}

sio_uart_ret_t sio_uart_unregister_urc_callback(sio_uart_urc_callback callback)
{
    if (SIO_UART(urc_cb) == callback) {
        SIO_UART(urc_cb) = NULL;
        return SIO_UART_RET_OK;
    } else {
        return SIO_UART_RET_PARAMETER_ERROR;
    }    
}

sio_uart_status_t sio_uart_get_status(void)
{
    return SIO_UART(modem_status);
}

sio_uart_ret_t sio_uart_set_status(sio_uart_status_t status)
{
    SIO_UART(modem_status) = status;
    return SIO_UART_RET_OK;
}

hal_uart_port_t sio_uart_get_modem_port()
{
    return SIO_UART(modem_port);
}

void sio_uart_set_trigger_modem_ready()
{
    SIO_UART(is_trigger_modem_ready) = 1;
}

sio_uart_ret_t sio_uart_timer_modem_exception_timeout_handle(void)
{
    sio_uart_ret_t ret = SIO_UART_RET_OK;

    sio_uart_mutex_lock(SIO_UART(mutex));

    LOGW("sio_uart_timer_modem_exception_timeout_handle. \r\n");

    ret = msm_trigger_to_modem(MSM_TRIGGER_TYPE_RESET);

    sio_uart_set_status(SIO_UART_STATUS_EXCEPTION);

    sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_ABNORMAL, NULL);

    sio_uart_mutex_unlock(SIO_UART(mutex));

    sio_uart_timer_reset(SIO_UART_TIMER_MODEM_EXCEPTION);

    return ret;
}

sio_uart_ret_t sio_uart_timer_ut_timeout_handle(void)
{
    sio_uart_ret_t ret = SIO_UART_RET_OK;

#ifdef SIO_UART_UT
    ret = sio_uart_ut_uart_handle();
    sio_uart_timer_reset(SIO_UART_TIMER_UT);
#endif /* SIO_UART_UT */

    return ret;
}

sio_uart_ret_t sio_uart_timer_ready_timeout_handle(void)
{
    sio_uart_ret_t ret = SIO_UART_RET_OK;

    sio_uart_timer_stop(SIO_UART_TIMER_WAIT_READY);

    while(1) {
        ret = sio_uart_trigger_modem_ready();

        LOGI("ret %d", ret);
        
        if (SIO_UART_RET_BUSY == ret) {  
            vTaskDelay(200);
            continue;
        }
        else if(SIO_UART_RET_OK == ret) {
            return SIO_UART_RET_OK;
        }
    }
}

void sio_uart_timer_timeout_callback(TimerHandle_t tmr)
{
    if (tmr == g_sio_uart_timer_modem_exception_handle) {
        sio_uart_timer_modem_exception_timeout_handle();
    } else if (tmr == g_sio_uart_timer_ut_handle) {
        sio_uart_timer_ut_timeout_handle();
    } else if (tmr == g_sio_uart_timer_wait_sleep_handle){
        sio_uart_send_wait_sleep_timeout_callback();
    } else if (tmr == g_sio_uart_timer_wait_response_handle){
        sio_uart_send_wait_response_timeout_callback();
    } else {
        return;
    }
}

TickType_t sio_uart_timer_get_tick(sio_uart_timer_t timer)
{
#ifndef MODEM_SUPPORT_DTE
    TickType_t tick = 600*1000 / portTICK_PERIOD_MS;
#else
    nvdm_status_t ret = NVDM_STATUS_OK;
    TickType_t tick = 0;

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION: {
            char SIO_RESET_MODEM_TIMEOUT_VALUE[] = "1";
            
            tick = atoi(SIO_RESET_MODEM_TIMEOUT_VALUE) * 1000 / portTICK_PERIOD_MS;
            if (NVDM_STATUS_OK == ret) {
                char buff[10];
                uint32_t len = sizeof(buff);
                ret = nvdm_read_data_item(
                          SIO_NVDM_GROUP_NAME,
                          SIO_NVDM_DATA_ITEM_TYPE_A,
                          (uint8_t *)buff,
                          &len);
                if (ret == NVDM_STATUS_OK) {
                    tick = atoi(buff) * 1000 / portTICK_PERIOD_MS;
                    if (tick == 0) {
                        uint32_t default_len = sizeof(SIO_RESET_MODEM_TIMEOUT_VALUE);
                        ret = nvdm_write_data_item(
                                  SIO_NVDM_GROUP_NAME,
                                  SIO_NVDM_DATA_ITEM_TYPE_A,
                                  NVDM_DATA_ITEM_TYPE_STRING,
                                  (uint8_t *)&SIO_RESET_MODEM_TIMEOUT_VALUE,
                                  default_len);
                        tick = atoi(SIO_RESET_MODEM_TIMEOUT_VALUE) * 1000 / portTICK_PERIOD_MS;
                    }
                } else {
                    uint32_t default_len = sizeof(SIO_RESET_MODEM_TIMEOUT_VALUE);
                    ret = nvdm_write_data_item(
                              SIO_NVDM_GROUP_NAME,
                              SIO_NVDM_DATA_ITEM_TYPE_A,
                              NVDM_DATA_ITEM_TYPE_STRING,
                              (uint8_t *)&SIO_RESET_MODEM_TIMEOUT_VALUE,
                              default_len);
                }
            }
            break;
        }
        default:
            break;
    }
#endif
    LOGI("sio_uart_timer_get_tick, timer:%d, tick:%d \r\n", timer, tick);

    return tick;
}


void sio_uart_trigger_modem_wakeup()
{    
#ifdef HAL_SLEEP_MANAGER_ENABLED
    LOGI("sio_trigger_modem_wakeup %d", SIO_UART(sleep_manager_handler));
    hal_sleep_manager_lock_sleep(SIO_UART(sleep_manager_handler));
#endif
    
    msm_trigger_to_modem(MSM_TRIGGER_TYPE_WAKEUP);
#ifdef MODEM_SUPPORT_DTE  
    hal_uart_set_dte_dtr_active(sio_uart_get_modem_port());
#endif
    vTaskDelay(10);
}

void sio_uart_trigger_modem_sleep()
{    
#ifdef HAL_SLEEP_MANAGER_ENABLED
    LOGI("sio_trigger_modem_wakeup %d", SIO_UART(sleep_manager_handler));
    hal_sleep_manager_unlock_sleep(SIO_UART(sleep_manager_handler));
#endif
    
    msm_trigger_to_modem(MSM_TRIGGER_TYPE_SLEEP);
#ifdef MODEM_SUPPORT_DTE  
    hal_uart_set_dte_dtr_deactive(sio_uart_get_modem_port());
#endif
    vTaskDelay(10);
}

sio_uart_ret_t sio_uart_trigger_modem_ready()
{
    const char *data = "at\r\n";
    int32_t size = strlen((char *)data);

    LOGI("enter");
    if (msm_query_status_from_modem() == MSM_STATUS_INACTIVE)
        sio_uart_trigger_modem_wakeup();

    if (sio_uart_get_status() == SIO_UART_STATUS_READY) {
        return SIO_UART_RET_OK;
    }
    else if (sio_uart_get_status() == SIO_UART_STATUS_INIT) {
        sio_uart_set_trigger_modem_ready();
        sio_uart_port_write_data(sio_uart_get_modem_port(), (uint8_t *)data, size);
        return SIO_UART_RET_BUSY;
    }
    else {
        return SIO_UART_RET_ERROR;
    }
}


