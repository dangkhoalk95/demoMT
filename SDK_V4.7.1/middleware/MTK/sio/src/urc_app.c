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
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "urc_app.h"
#ifdef __CMUX_SUPPORT__
#include "cmux.h"
#endif
#include "sio_gprot.h"
#include "sio_uart_gprot.h"
#include "task_def.h"

//#define URC_PRINTF

#ifdef URC_PRINTF
#define LOGE(fmt,arg...)   printf(("[URC]: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("[URC]: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("[URC]: "fmt), ##arg)
#else
log_create_module(urc, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   //LOG_E(urc, "[URC]: "fmt,##arg)
#define LOGW(fmt,arg...)   //LOG_W(urc, "[URC]: "fmt,##arg)
#define LOGI(fmt,arg...)   //LOG_I(urc ,"[URC]: "fmt,##arg)
#endif

#define urc_queue_length                  (20)
#define URC_CALLBACK_NUMBER               (10)

typedef enum {
    MSG_ID_UART_MSG,
    MSG_ID_CMUX_MSG,
    MSG_ID_MSM_END
} urc_app_msg_id_t;

typedef struct {
    urc_app_msg_id_t msg_id;
    uint8_t         *msg_data;
} urc_app_msg_t;

#ifdef __CMUX_SUPPORT__
typedef struct {
    cmux_event_t      event;
} urc_app_cmux_msg_t;
#endif /* __CMUX_SUPPORT__ */

typedef struct {
    uint8_t         *data;
    uint32_t        length;
} urc_app_uart_msg_t;

typedef struct {
    int                 is_used;
    urc_callback_t      callback;
} urc_callback_func_t;

urc_callback_func_t g_urc_app_callbcak[URC_CALLBACK_NUMBER];
int urc_channel_id;
QueueHandle_t q_id;
SemaphoreHandle_t urc_mutex;

static void urc_app_process(void *arg);

static void urc_app_handle_uart_msg(urc_app_uart_msg_t *msg);

static void urc_app_uart_callback(void *data, int32_t length);

#ifdef __CMUX_SUPPORT__
static void urc_app_cmux_read_data(void);

static void urc_app_handle_cmux_msg(urc_app_cmux_msg_t *msg);

static void urc_app_cmux_callback(cmux_event_t event, void *param);
#endif /* __CMUX_SUPPORT__ */


static void urc_app_handle_uart_msg(urc_app_uart_msg_t *msg)
{
    int32_t i;
    urc_cb_ret ret = RET_OK_CONTINUE;
    uint32_t length;

    length = msg->length;
    for (i = 0; i < URC_CALLBACK_NUMBER; i++) {
        if (g_urc_app_callbcak[i].is_used == 1 && ret == RET_OK_CONTINUE) {
            ret = g_urc_app_callbcak[i].callback(msg->data, length);
        }
    }
    vPortFree(msg->data);
    vPortFree(msg);
}

static uint32_t urc_app_queue_send(QueueHandle_t q_id, void *data)
{
    BaseType_t ret = 0;

    ret = xQueueSend((QueueHandle_t)q_id, data, 0);
    if (pdFAIL != ret) {
        return 0;
    } else {
        LOGW("urc_app_queue_send, error %d \r\n", ret);
        return 0;
    }
}


static void urc_app_uart_callback(void *data, int32_t length)
{
    urc_app_msg_t msg_queue_item;
    urc_app_uart_msg_t *uart_msg;

    uart_msg = pvPortMalloc(sizeof(urc_app_uart_msg_t));
    uart_msg->data = pvPortMalloc(length + 1);
    memcpy(uart_msg->data, data, length);
    uart_msg->data[length] = '\0';
    uart_msg->length = length;
    msg_queue_item.msg_id = MSG_ID_UART_MSG;
    msg_queue_item.msg_data = (void *)uart_msg;
    urc_app_queue_send(q_id, (void *)&msg_queue_item);
}

#ifdef __CMUX_SUPPORT__
static void urc_app_cmux_read_data(void)
{
    uint32_t read_length;
    uint8_t *data;
    int32_t i;
    urc_cb_ret ret = RET_OK_CONTINUE;
    
    read_length = cmux_get_available_read_length(urc_channel_id);
    LOGI("urc_app_cmux_read_data() channel_id = %d, read_length = %d. \r\n", urc_channel_id, read_length);

    data = pvPortMalloc(read_length);
    cmux_read_data(urc_channel_id, data, read_length);
    LOGI("cmux urc = %s. \r\n", data);
    for (i = 0; i < URC_CALLBACK_NUMBER; i++) {
        if (g_urc_app_callbcak[i].is_used == 1 && ret == RET_OK_CONTINUE) {
            ret = g_urc_app_callbcak[i].callback(data, read_length);
        }
    }
    vPortFree(data);
}


static void urc_app_handle_cmux_msg(urc_app_cmux_msg_t *msg)
{
    cmux_event_t event = msg->event;

    if (event == CMUX_EVENT_READY_TO_READ) {
        urc_app_cmux_read_data();
    }
    vPortFree(msg);
}

#endif /* __CMUX_SUPPORT__ */

static void urc_app_process(void *arg)
{
    urc_app_msg_t msg_queue_data;

    /* Loop, processing httpd cmds. */
    for (;;) {
        if (xQueueReceive(q_id, &msg_queue_data, portMAX_DELAY) == pdPASS) {
            LOGI("urc_app_process() handle input message, msg type = %d\r\n", msg_queue_data.msg_id);
            /* message handling */
            if (MSG_ID_UART_MSG == msg_queue_data.msg_id) {
                urc_app_handle_uart_msg((urc_app_uart_msg_t *)(msg_queue_data.msg_data));
            }
        #ifdef __CMUX_SUPPORT__
             else if (MSG_ID_CMUX_MSG == msg_queue_data.msg_id) {
                urc_app_handle_cmux_msg((urc_app_cmux_msg_t *)(msg_queue_data.msg_data));
            }
        #endif /* __CMUX_SUPPORT__ */
        }
    }
}


static void urc_sio_event_handler(uint32_t event, void *data)
{
    if (event == SIO_UART_EVENT_MODEM_ABNORMAL) {
        LOGI("exception");
        
        sio_uart_unregister_urc_callback(urc_app_uart_callback);
    }
}

void urc_app_init()
{    
    LOGI("~~~~~~~~~~~~~URC APP INIT~~~~~~~~~~~~~");
    
    xTaskCreate(
        urc_app_process,
        URC_TASK_NAME,
        URC_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
        NULL,
        URC_TASK_PRIO,
        NULL);
    sio_uart_register_urc_callback(urc_app_uart_callback);
    sio_register_event_notifier(SIO_APP_TYPE_CMUX_AT_CMD, urc_sio_event_handler);
#ifdef __CMUX_SUPPORT__
    urc_channel_id = cmux_register_callback(CMUX_CHANNEL_TYPE_URC, urc_app_cmux_callback);
#endif /* __CMUX_SUPPORT__ */
    q_id = xQueueCreate(urc_queue_length, sizeof(urc_app_msg_t));
    urc_mutex = xSemaphoreCreateMutex();
    LOGI("urc_app_init() urc_channel_id = %d. \r\n", urc_channel_id);
    for (int i = 0; i < URC_CALLBACK_NUMBER; i++) {
        g_urc_app_callbcak[i].is_used = 0;
    }
}

#ifdef __CMUX_SUPPORT__
static void urc_app_cmux_callback(cmux_event_t event, void *param)
{
    cmux_channel_id_t channel_id;
    urc_app_msg_t msg_queue_item;
    urc_app_cmux_msg_t *cmux_msg;

    cmux_msg = pvPortMalloc(sizeof(urc_app_cmux_msg_t));
    cmux_msg->event = event;
    LOGI("urc_app_cmux_callback() event = %d. \r\n", event);
    switch (event) {
            cmux_event_connection_disconnection_read_write_t *cmux_data;
        case CMUX_EVENT_START_UP:
        case CMUX_EVENT_CLOSE_DOWN:
        case CMUX_EVENT_MODEM_ABNORMAL:
        case CMUX_EVENT_MAX:
            return;
            
        case CMUX_EVENT_CHANNEL_CONNECTION:
        case CMUX_EVENT_CHANNEL_DISCONNECTION:
        case CMUX_EVENT_READY_TO_READ:
        case CMUX_EVENT_READY_TO_WRITE:
            cmux_data = (cmux_event_connection_disconnection_read_write_t *)param;
            channel_id = cmux_data->channel_id;
            LOGI("urc_app_cmux_callback() urc_channel_id=%d, channel_id = %d. \r\n", urc_channel_id, channel_id);
            if (urc_channel_id != channel_id) {
                return;
            }
            break;
    }
    msg_queue_item.msg_id = MSG_ID_CMUX_MSG;
    msg_queue_item.msg_data = (void *)cmux_msg;
    urc_app_queue_send(q_id, (void *)&msg_queue_item);
}
#endif /* __CMUX_SUPPORT__ */

void urc_register_callback(urc_callback_t callback)
{
    int i;
    
    xSemaphoreTake(urc_mutex, portMAX_DELAY);
    
    for (i = 0; i < URC_CALLBACK_NUMBER; i++) {
        if (g_urc_app_callbcak[i].is_used == 1 &&  g_urc_app_callbcak[i].callback == callback) {
            return;
        }
    }
    for (i = 0; i < URC_CALLBACK_NUMBER; i++) {
        if (g_urc_app_callbcak[i].is_used == 0) {
            g_urc_app_callbcak[i].callback = callback;
            g_urc_app_callbcak[i].is_used = 1;
            break;
        }
    }
    
    xSemaphoreGive(urc_mutex);
}

void urc_unregister_callback(urc_callback_t callback) {
    int i;
    
    xSemaphoreTake(urc_mutex, portMAX_DELAY);
    
    for (i = 0; i < URC_CALLBACK_NUMBER; i++) {
        if (g_urc_app_callbcak[i].is_used == 1 &&  g_urc_app_callbcak[i].callback == callback) {
            g_urc_app_callbcak[i].is_used = 0;
            g_urc_app_callbcak[i].callback = NULL;
            LOGI("%d", i);
            return;
        }
    }
    
    xSemaphoreGive(urc_mutex);
}

