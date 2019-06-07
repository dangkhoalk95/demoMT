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

#include "cmux.h"
#include "cmux_def.h"
#include "cmux_struct.h"
#include "cmux_vp.h"
#include "cmux_utl.h"
#include "cmux_prot.h"
#include "cmux_hdlr.h"
#include "sio_uart_gprot.h"

log_create_module(cmux, PRINT_LEVEL_ERROR);

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux.c] "fmt,##arg)

#define CMUX_QUEUE_SIZE         (20)

#define CMUX_TX_VFIFO_SIZE      (2048)
#define CMUX_RX_VFIFO_SIZE      (2048)

typedef struct {
    hal_uart_port_t port;
    cmux_role_t role;
    TaskHandle_t task_handle;
    SemaphoreHandle_t mutex_handle, mutex_handle2;
    cmux_channel_id_t channel_list[CMUX_DLC_NUM];
    cmux_callback_t callback_list[CMUX_DLC_NUM];
    uint32_t channel_index;
} cmux_context_t;

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_cmux_tx_vfifo[CMUX_TX_VFIFO_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_cmux_rx_vfifo[CMUX_RX_VFIFO_SIZE];
static cmux_context_t g_cmux_context;
cmux_context_t *const g_cmux_ptr = &g_cmux_context;
QueueHandle_t g_cmux_queue;

int32_t cmux_register_callback(cmux_channel_type_t channel_type, cmux_callback_t callback)
{
    cmux_channel_id_t channel_id;
    kal_uint32 i;

    LOGI("channel_type: %d", channel_type);

    if (channel_type >= CMUX_CHANNEL_TYPE_MAX || callback == NULL) {
        return CMUX_STATUS_INVALID_PARAMETER;
    }

    if (g_cmux_ptr->mutex_handle == NULL) {
        g_cmux_ptr->mutex_handle = xSemaphoreCreateMutex();
    }

    xSemaphoreTake(g_cmux_ptr->mutex_handle, portMAX_DELAY);

    for (i = 1; i <= CMUX_DLC_NUM; i++) {
        if (cmux_getDlcType(i) == channel_type + 1) {
            if (g_cmux_ptr->channel_list[i - 1] == 0) {
                channel_id = cmux_custom_getChannelDlci(i);
                g_cmux_ptr->channel_list[i - 1] = channel_id;
                g_cmux_ptr->callback_list[i - 1] = callback;

                xSemaphoreGive(g_cmux_ptr->mutex_handle);

                LOGI("channel_id: %d", channel_id);

                return channel_id;
            }
        }
    }

    xSemaphoreGive(g_cmux_ptr->mutex_handle);

    return CMUX_STATUS_NO_FREE_CHANNEL_ID;
}

kal_bool cmux_send_message(msg_type msg_id, local_para_struct *local_para_ptr)
{
    ilm_struct ilm;

    LOGI("msg_id = %d", msg_id);

    ilm.msg_id = msg_id;
    ilm.local_para_ptr = local_para_ptr;

    if (xQueueSend(g_cmux_queue, &ilm, portMAX_DELAY) != pdPASS) {
        LOGE("xQueueSend != pdPASS");
        return KAL_FALSE;
    }

    return KAL_TRUE;
}

kal_bool cmux_send_message_from_isr(msg_type msg_id, local_para_struct *local_para_ptr)
{
    ilm_struct ilm;
    BaseType_t xHigherPriorityTaskWoken;

    LOGI("msg_id = %d", msg_id);

    ilm.msg_id = msg_id;
    ilm.local_para_ptr = local_para_ptr;

    if (xQueueSendFromISR(g_cmux_queue, &ilm, &xHigherPriorityTaskWoken) != pdPASS) {
        LOGE("xQueueSendFromISR != pdPASS");
        return KAL_FALSE;
    }

    return KAL_TRUE;
}

static void cmux_uart_callback(hal_uart_callback_event_t status, void *user_data)
{
    if (status == HAL_UART_EVENT_READY_TO_WRITE) {
        cmux_send_message_from_isr(MSG_ID_UART_READY_TO_WRITE_IND, (local_para_struct *) NULL);
    } else if (status == HAL_UART_EVENT_READY_TO_READ) {
        cmux_send_message_from_isr(MSG_ID_UART_READY_TO_READ_IND, (local_para_struct *) NULL);
    } else if (status == HAL_UART_EVENT_TRANSACTION_ERROR) {
        cmux_send_message_from_isr(MSG_ID_UART_PLUGOUT_IND, (local_para_struct *) NULL);
    }
}

static hal_uart_status_t cmux_uart_init(hal_uart_port_t port)
{
    hal_uart_status_t status;

    hal_uart_config_t uart_config = {
        .baudrate = HAL_UART_BAUDRATE_921600,
        .word_length = HAL_UART_WORD_LENGTH_8,
        .stop_bit = HAL_UART_STOP_BIT_1,
        .parity = HAL_UART_PARITY_NONE
    };

    status = hal_uart_init(port, &uart_config);
    LOGI("hal_uart_init = %d", status);
    if (status != HAL_UART_STATUS_OK) {
        return status;
    }

    hal_uart_dma_config_t dma_config = {
        .send_vfifo_buffer              = g_cmux_tx_vfifo,
        .send_vfifo_buffer_size         = CMUX_TX_VFIFO_SIZE,
        .send_vfifo_threshold_size      = 128,
        .receive_vfifo_buffer           = g_cmux_rx_vfifo,
        .receive_vfifo_buffer_size      = CMUX_RX_VFIFO_SIZE,
        .receive_vfifo_threshold_size   = 128,
        .receive_vfifo_alert_size       = 50
    };

    status = hal_uart_set_dma(port, &dma_config);
    LOGI("hal_uart_set_dma = %d", status);
    if (status != HAL_UART_STATUS_OK) {
        hal_uart_deinit(port);
        return status;
    }

    status = hal_uart_register_callback(port, cmux_uart_callback, NULL);
    LOGI("hal_uart_register_callback = %d", status);
    if (status != HAL_UART_STATUS_OK) {
        hal_uart_deinit(port);
        return status;
    }

    return HAL_UART_STATUS_OK;
}

static hal_uart_status_t cmux_uart_deinit(hal_uart_port_t port)
{
    hal_uart_status_t status;

    status = hal_uart_deinit(port);
    LOGI("hal_uart_deinit = %d", status);
    if (status != HAL_UART_STATUS_OK) {
        return status;
    }

    return HAL_UART_STATUS_OK;
}

cmux_status_t cmux_create_task(hal_uart_port_t port, cmux_role_t role)
{
    LOGI("port: %d, role: %d", port, role);

    if (port >= HAL_UART_MAX || role >= CMUX_ROLE_MAX) {
        return CMUX_STATUS_INVALID_PARAMETER;
    }

    g_cmux_ptr->port = port;
    g_cmux_ptr->role = role;

    if (g_cmux_queue == NULL) {
        g_cmux_queue = xQueueCreate(CMUX_QUEUE_SIZE, sizeof(ilm_struct));
    }

    if (g_cmux_ptr->task_handle == NULL) {
        cmux_task_create(&g_cmux_ptr->task_handle);
    }

    if (g_cmux_ptr->mutex_handle2 == NULL) {
        g_cmux_ptr->mutex_handle2 = xSemaphoreCreateMutex();
    }

    xSemaphoreTake(g_cmux_ptr->mutex_handle2, portMAX_DELAY);

    return CMUX_STATUS_OK;
}

cmux_status_t cmux_enable(void)
{
#ifdef __CMUX_UT__
    const char *startupCmd = "AT+CMUX=0";
    uint32_t write_data_len;
#else
    cmux_startup_req_struct *startup_req;
#endif

    CMUX_ASSERT(g_cmux_ptr->port < HAL_UART_MAX && g_cmux_ptr->role < CMUX_ROLE_MAX);
    CMUX_ASSERT(g_cmux_queue != NULL);
    CMUX_ASSERT(g_cmux_ptr->task_handle != NULL);

    if (cmux_uart_init(g_cmux_ptr->port) != HAL_UART_STATUS_OK) {
        return CMUX_STATUS_UART_OPEN_ERROR;
    }

#ifdef __CMUX_UT__
    if (g_cmux_ptr->role == CMUX_ROLE_CLIENT) {
        write_data_len = strlen(startupCmd);
        hal_uart_send_dma(g_cmux_ptr->port, (const uint8_t *)startupCmd, write_data_len);
        LOGI("send data (len: %d) to uart: %s", write_data_len, startupCmd);
    }
#else
    /* Client send "AT+CMUX=0" and receive OK */
    startup_req = (cmux_startup_req_struct *) pvPortMalloc((kal_uint16) sizeof(cmux_startup_req_struct));
    startup_req->port = g_cmux_ptr->port;
    startup_req->role = g_cmux_ptr->role;
    cmux_send_message(MSG_ID_CMUX_STARTUP_REQ, (local_para_struct *) startup_req);
#endif

    return CMUX_STATUS_OK;
}

cmux_status_t cmux_disable(void)
{
#ifdef __CMUX_UT__
    const char *closedownCmd = "AT+CMUX=2";
    uint32_t write_data_len;
#endif

    CMUX_ASSERT(g_cmux_ptr->port < HAL_UART_MAX && g_cmux_ptr->role < CMUX_ROLE_MAX);
    CMUX_ASSERT(g_cmux_queue != NULL);
    CMUX_ASSERT(g_cmux_ptr->task_handle != NULL);

#ifdef __CMUX_UT__
    write_data_len = strlen(closedownCmd);
    hal_uart_send_dma(g_cmux_ptr->port, (const uint8_t *)closedownCmd, write_data_len);
    LOGI("send data (len: %d) to uart: %s", write_data_len, closedownCmd);
#else
    if (sio_uart_get_status() == SIO_UART_STATUS_EXCEPTION) {
        if (cmux_handleUartPlugoutInd() == false) {
            return CMUX_STATUS_OK;
        }
    } else {
    cmux_send_message(MSG_ID_CMUX_CLOSE_DOWN_REQ, (local_para_struct *) NULL);
    }
    xSemaphoreTake(g_cmux_ptr->mutex_handle2, portMAX_DELAY);
#endif

    return CMUX_STATUS_OK;
}

uint32_t cmux_write_data(cmux_channel_id_t channel_id, const uint8_t *data, uint32_t size)
{
    uint32_t write_data_len;

    write_data_len = CPort_PutBytes(channel_id, (uint8_t *)data, size);

    LOGI("channel_id: %d, data: 0x%x, size: %d, write_data_len: %d", channel_id, data, size, write_data_len);

    return write_data_len;
}

uint32_t cmux_read_data(cmux_channel_id_t channel_id, uint8_t *buffer, uint32_t size)
{
    uint32_t read_data_len;

    read_data_len = CPort_GetBytes(channel_id, buffer, size);

    LOGI("channel_id: %d, buffer: 0x%x, size: %d, read_data_len: %d", channel_id, buffer, size, read_data_len);

    return read_data_len;
}

uint32_t cmux_get_available_write_space(cmux_channel_id_t channel_id)
{
    uint32_t write_space;

    write_space = CPort_GetTxRoomLeft(channel_id);

    LOGI("channel_id: %d, write_space: %d", channel_id, write_space);

    return write_space;
}

uint32_t cmux_get_available_read_length(cmux_channel_id_t channel_id)
{
    uint32_t read_length;

    read_length = CPort_GetRxDataAvail(channel_id);

    LOGI("channel_id: %d, read_length: %d", channel_id, read_length);

    return read_length;
}

void cmux_establish_connection(void)
{
    kal_uint32 i;
    cmux_channel_id_t channel_id;

    for (i = 0; i < CMUX_DLC_NUM; i++) {
        g_cmux_ptr->channel_index = i;
        channel_id = g_cmux_ptr->channel_list[i];
        if (channel_id != 0) {
            cmux_sendSabmProcedure(channel_id);
            return;
        }
    }
}

void cmux_continue_establish_connection(void)
{
    kal_uint32 i;
    cmux_channel_id_t channel_id;

    for (i = g_cmux_ptr->channel_index + 1; i < CMUX_DLC_NUM; i++) {
        g_cmux_ptr->channel_index = i;
        channel_id = g_cmux_ptr->channel_list[i];
        if (channel_id != 0) {
            cmux_sendSabmProcedure(channel_id);
            return;
        }
    }
}

void cmux_negotiate_and_set_parameter(void)
{
#ifdef __CMUX_PN_SUPPORT__
    kal_uint32 i;
    cmux_channel_id_t channel_id;

    for (i = 0; i < CMUX_DLC_NUM; i++) {
        g_cmux_ptr->channel_index = i;
        channel_id = g_cmux_ptr->channel_list[i];
        if (channel_id != 0) {
            cmux_sendPnMessage(channel_id);
            return;
        }
    }

    if (i == CMUX_DLC_NUM) {
        cmux_establish_connection();
    }
#else
    cmux_establish_connection();
#endif
}

void cmux_continue_negotiate_and_set_parameter(void)
{
    kal_uint32 i;
    cmux_channel_id_t channel_id;

    for (i = g_cmux_ptr->channel_index + 1; i < CMUX_DLC_NUM; i++) {
        g_cmux_ptr->channel_index = i;
        channel_id = g_cmux_ptr->channel_list[i];
        if (channel_id != 0) {
            cmux_sendPnMessage(channel_id);
            return;
        }
    }

    if (i == CMUX_DLC_NUM) {
        cmux_establish_connection();
    }
}

void cmux_check_and_handle_startup_req(void)
{
#ifdef __CMUX_UT__
    const char *startupCmd = "AT+CMUX=0";
    const char *startupCmdNotSupport = "AT+CMUX=1";
    const char *startupRsp = "OK";
    const char *startupRspError = "ERROR";
    uint32_t read_data_len, write_data_len;
    uint8_t buffer[100] = {0};

    read_data_len = hal_uart_get_available_receive_bytes(g_cmux_ptr->port);
    if (read_data_len > 100) {
        read_data_len = 100;
    }
    hal_uart_receive_dma(g_cmux_ptr->port, buffer, read_data_len);
    LOGI("receive data (len: %d) from uart: %s", read_data_len, buffer);

    if (strncasecmp((const char *)buffer, startupCmd, strlen(startupCmd)) == 0) {
        cmux_startup_req_struct *startup_req;

        write_data_len = strlen(startupRsp);
        hal_uart_send_dma(g_cmux_ptr->port, (const uint8_t *)startupRsp, write_data_len);
        LOGI("send data (len: %d) to uart: %s", write_data_len, startupRsp);

        startup_req = (cmux_startup_req_struct *) pvPortMalloc((kal_uint16) sizeof(cmux_startup_req_struct));
        startup_req->port = g_cmux_ptr->port;
        startup_req->role = g_cmux_ptr->role;
        cmux_send_message(MSG_ID_CMUX_STARTUP_REQ, (local_para_struct *) startup_req);
    } else if (strncasecmp((const char *)buffer, startupRsp, strlen(startupRsp)) == 0) {
        cmux_startup_req_struct *startup_req;

        /* Client send "AT+CMUX=0" and receive OK */
        startup_req = (cmux_startup_req_struct *) pvPortMalloc((kal_uint16) sizeof(cmux_startup_req_struct));
        startup_req->port = g_cmux_ptr->port;
        startup_req->role = g_cmux_ptr->role;
        cmux_send_message(MSG_ID_CMUX_STARTUP_REQ, (local_para_struct *) startup_req);
    } else if (strncasecmp((const char *)buffer, startupCmdNotSupport, strlen(startupCmdNotSupport)) == 0) {
        write_data_len = strlen(startupRspError);
        hal_uart_send_dma(g_cmux_ptr->port, (const uint8_t *)startupRspError, write_data_len);
        LOGI("send data (len: %d) to uart: %s", write_data_len, startupRspError);
    }
#endif
}

kal_bool cmux_dispatch_app_event(ilm_struct *ilm_ptr)
{
    kal_uint32 i;
    cmux_channel_id_t channel_id;
    cmux_event_connection_disconnection_read_write_t *event_param;

    switch (ilm_ptr->msg_id) {
        /* APP */
        case CMUX_EVENT_START_UP:
            for (i = 0; i < CMUX_DLC_NUM; i++) {
                channel_id = g_cmux_ptr->channel_list[i];
                if (channel_id != 0) {
                    g_cmux_ptr->callback_list[i](ilm_ptr->msg_id, NULL);
                }
            }
            if (g_cmux_ptr->role == CMUX_ROLE_CLIENT) {
                cmux_sendSabmProcedure(0);
            }
            break;
        case CMUX_EVENT_CLOSE_DOWN:
            for (i = 0; i < CMUX_DLC_NUM; i++) {
                channel_id = g_cmux_ptr->channel_list[i];
                if (channel_id != 0) {
                    g_cmux_ptr->callback_list[i](ilm_ptr->msg_id, NULL);
                }
            }
            CMUX_ASSERT(cmux_uart_deinit(g_cmux_ptr->port) == HAL_UART_STATUS_OK);
            xSemaphoreGive(g_cmux_ptr->mutex_handle2);
            break;
        case CMUX_EVENT_CHANNEL_CONNECTION:
        case CMUX_EVENT_CHANNEL_DISCONNECTION:
        case CMUX_EVENT_READY_TO_READ:
        case CMUX_EVENT_READY_TO_WRITE:
            event_param = (cmux_event_connection_disconnection_read_write_t *) ilm_ptr->local_para_ptr;
            for (i = 0; i < CMUX_DLC_NUM; i++) {
                channel_id = g_cmux_ptr->channel_list[i];
                if (channel_id == event_param->channel_id) {
                    if (ilm_ptr->msg_id == CMUX_EVENT_READY_TO_READ && cmux_get_available_read_length(channel_id) == 0) {
                        break;
                    }
                    g_cmux_ptr->callback_list[i](ilm_ptr->msg_id, event_param);
                }
            }
            break;
    }

    return KAL_TRUE;
}

