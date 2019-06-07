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
#include "FreeRTOSConfig.h"
#include "sio_gprot.h"
#include "task.h"
#include "msm.h"
#include "timers.h"
#include "hal_sleep_manager.h"
#include "hal_uart.h"

#ifdef __CMUX_SUPPORT__
typedef struct sio_context_struct {
    uint32_t                 data_type;
    uint32_t                 uart_mode;
} sio_context_struct;

static sio_context_struct sio_cnt = {0};
#endif

#ifdef __CMUX_SUPPORT__
#define  sio_is_uart_type(type)       ((type & 0xF0) ? 1 : 0)
#define  sio_is_uart_app(app_id)      ((app_id & 0xF0) ? 1 : 0)
#endif

#ifdef SIO_PRINTF
#define LOGE(fmt,arg...)   printf(("[SIO MAIN]: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("[SIO MAIN]: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("[SIO MAIN]: "fmt), ##arg)
#else
log_create_module(sio, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(sio, "[SIO MAIN]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(sio, "[SIO MAIN]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(sio, "[SIO MAIN]: "fmt,##arg)
#endif


extern uint32_t sio_uart_port_write_data(hal_uart_port_t uart_index, uint8_t *buf, uint32_t buf_len);
extern hal_uart_status_t hal_uart_set_dte_dtr_active(hal_uart_port_t uart_port);
extern hal_uart_status_t hal_uart_set_dte_dtr_deactive(hal_uart_port_t uart_port);

#ifdef __CMUX_SUPPORT__
static cmux_channel_type_t sio_app_type_to_cmux_type(sio_app_type_t type)
{
    uint32_t app_type = type & 0x0F;

    if (app_type == SIO_APP_TYPE_CMUX_AT_CMD) {
        return CMUX_CHANNEL_TYPE_COMMAND;
    } else if (app_type == SIO_APP_TYPE_CMUX_IP_DATA) {
        return CMUX_CHANNEL_TYPE_IP_DATA;
    } else {
        return CMUX_CHANNEL_TYPE_MAX;
    }
}
#endif /* __CMUX_SUPPORT__ */

#ifdef __CMUX_SUPPORT__
int32_t sio_get_current_mode(void)
{
    return sio_cnt.uart_mode;
}

sio_ret_t sio_set_current_mode(int32_t mode)
{
    sio_cnt.uart_mode = mode;
    return SIO_RET_OK;
}
#endif


void sio_from_uart_event_handler(uint32_t event, void *data)
{
    if (SIO_UART_EVENT_MODEM_ABNORMAL == event) {
        msm_trigger_to_modem(MSM_TRIGGER_TYPE_RESET);
    }
}


sio_ret_t sio_init(hal_uart_port_t uart_port)
{
    LOGI("~~~~~~~~~~~~~SIO INIT~~~~~~~~~~~~~");
#ifdef __CMUX_SUPPORT__
    cmux_create_task(HAL_UART_1, CMUX_ROLE_CLIENT);
#endif /* __CMUX_SUPPORT__ */
    
    if (SIO_UART_RET_OK != sio_uart_init(uart_port)) {
    return SIO_RET_ERROR;
}

#ifdef __CMUX_SUPPORT__
    sio_set_current_mode(SIO_MODE_TYPE_UART | SIO_DATA_TYPE_COMMAND);  
#endif

    sio_uart_set_notification_callback(1, 0, SIO_UART_EVENT_MAX_NUMBER, sio_from_uart_event_handler);
    return SIO_RET_OK;
} 

sio_ret_t sio_set_mode(int32_t app_id, int32_t flag)
{
#ifdef __CMUX_SUPPORT__
    int32_t mode_type, data_type;
    int32_t current_type;
    int32_t current_mode, current_data;
    sio_uart_ret_t ret = SIO_UART_RET_ERROR;
    LOGI("sio_cnt.sleep_manager_handler = %d.\r\n", sio_cnt.sleep_manager_handler);

    LOGI("sio_set_mode() app_id = %d,  flag = %d. \r\n", app_id, flag);
    if (sio_is_uart_app(app_id) == 0) {
        return SIO_RET_ERROR;
    }

    mode_type = flag & 0x0F;
    data_type = flag & 0xF0;
    current_type = sio_get_current_mode();
    current_mode = current_type & 0x0F;
    current_data = current_type & 0xF0;

    if (mode_type != current_mode) {
        cmux_status_t cmux_status = CMUX_STATUS_UART_OPEN_ERROR;
        if (mode_type == SIO_MODE_TYPE_CMUX) {
            ret = sio_uart_port_deinit();
            if (ret == SIO_UART_RET_OK) {
                cmux_status = cmux_enable();
            }
        } else if (mode_type == SIO_MODE_TYPE_UART) {
            cmux_status = cmux_disable();
            if (cmux_status == CMUX_STATUS_OK) {
                ret = sio_uart_port_init();
            }
        } else {
            ret = SIO_UART_RET_ERROR;
        }

        if (cmux_status == CMUX_STATUS_OK && ret == SIO_UART_RET_OK) {
            current_type = (current_type & 0xF0) | mode_type;
            ret = SIO_UART_RET_OK;
        }
    }

    if (data_type != current_data) {
        if (data_type == SIO_DATA_TYPE_COMMAND || data_type == SIO_DATA_TYPE_DATA) {
            current_type = (current_type & 0x0F) | data_type;
            ret = SIO_UART_RET_OK;
        }
    }
    sio_set_current_mode(current_type);

    if (ret == SIO_UART_RET_ERROR)
        return SIO_RET_ERROR;
    else 
        return SIO_RET_OK;
#else /* __CMUX_SUPPORT__ */

    return SIO_RET_OK;
#endif /* __CMUX_SUPPORT__ */
}


uint32_t sio_send_data(int32_t app_id, const uint8_t *data, uint32_t size)
{
    uint32_t sent_length = 0;
#ifdef __CMUX_SUPPORT__
    int32_t current_type;
    int32_t mode_type, data_type;

    current_type = sio_get_current_mode();
    mode_type = current_type & 0x0F;
    data_type = current_type & 0xF0;
    if (mode_type == SIO_MODE_TYPE_UART) {
        if ((data_type == SIO_DATA_TYPE_COMMAND && sio_is_uart_app(app_id) == 1) ||
                (data_type == SIO_DATA_TYPE_DATA)) {
            sent_length = sio_uart_send_data(app_id, data, size);
        } else {
            LOGE("app_id = %d Can't send data. \r\n", app_id);
        }
    }
    else if (mode_type == SIO_MODE_TYPE_CMUX) {
        sent_length = cmux_write_data(sio_get_channel_id(app_id), data, size);
    }
#else
    sent_length = sio_uart_send_data(app_id, data, size);
#endif /* __CMUX_SUPPORT__ */
    return sent_length;
}


uint32_t sio_receive_data(int32_t app_id, uint8_t *buffer, uint32_t size)
{
    uint32_t read_size = 0;
#ifdef __CMUX_SUPPORT__
    int32_t mode;
    int32_t mode_type, data_type;
    uint32_t total_size = 0;
    cmux_channel_id_t channel_id;

    mode = sio_get_current_mode();
    LOGI("sio_receive_data() app_id = %d,  mode = %d, size = %d. \r\n", app_id, mode, size);
    mode_type = mode & 0x0F;
    data_type = mode & 0xF0;
    if (mode_type == SIO_MODE_TYPE_UART) {
        if((data_type == SIO_DATA_TYPE_COMMAND && sio_is_uart_app(app_id) == 1) ||
            (data_type == SIO_DATA_TYPE_DATA)) {
        read_size = sio_uart_receive_data(app_id, buffer, size);
        }
    } 
    else if (mode_type == SIO_MODE_TYPE_CMUX) {
        channel_id = sio_get_channel_id(app_id);
        total_size = cmux_get_available_read_length(channel_id);
        size = size < total_size ? size : total_size;
        read_size = cmux_read_data(channel_id, buffer, size);
    }
#else
    read_size = sio_uart_receive_data(app_id, buffer, size);
#endif /* __CMUX_SUPPORT__ */
    LOGI("sio_receive_data() read_size = %d, buffer = %s. \r\n", read_size, buffer);
    return read_size;
}


int32_t sio_register_event_notifier(sio_app_type_t type, sio_event_handler notifier)
{
    int32_t app_id = 0;
    int32_t channel_id = 0;
#ifdef __CMUX_SUPPORT__
    cmux_channel_type_t channel_type;
#endif /* __CMUX_SUPPORT__ */

    LOGI("sio_register_app_id. type = %d notifier=%p\r\n", type, notifier);

    if ((type & SIO_APP_TYPE_UART_AT_CMD) == SIO_APP_TYPE_UART_AT_CMD) {
        app_id = 1;
    }
    
#ifdef __CMUX_SUPPORT__
    channel_type = sio_app_type_to_cmux_type(type);        
    channel_id = cmux_register_callback(channel_type, (cmux_callback_t)notifier);
#endif /* __CMUX_SUPPORT__ */

    // app_id = type(4) | channel(4);    0X1a mean: app is uart type and channel id = a
    app_id = (app_id << 4) | channel_id;

    sio_uart_set_notification_callback(1, app_id, SIO_UART_EVENT_MAX_NUMBER, notifier);
    LOGI("sio_register_app_id. app_id = %d \r\n", app_id);
    return app_id;
}

void sio_add_modem_response(const char* modem_response, uint8_t is_end)
{
    sio_uart_add_modem_response(modem_response, is_end);
}

