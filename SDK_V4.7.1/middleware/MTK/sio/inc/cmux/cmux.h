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

#ifndef __CMUX_H__
#define __CMUX_H__

#include <stdint.h>

#include "hal_uart.h"

typedef enum {
    CMUX_CHANNEL_TYPE_URC,
    CMUX_CHANNEL_TYPE_COMMAND,
    CMUX_CHANNEL_TYPE_IP_DATA,
    CMUX_CHANNEL_TYPE_MAX
} cmux_channel_type_t;

typedef enum {
    CMUX_STATUS_OK = 0,
    CMUX_STATUS_INVALID_PARAMETER = -1,
    CMUX_STATUS_UART_OPEN_ERROR = -2,
    CMUX_STATUS_NO_FREE_CHANNEL_ID = -3
} cmux_status_t;

typedef enum {
    CMUX_EVENT_START_UP = 1,
    CMUX_EVENT_CLOSE_DOWN = 2,
    CMUX_EVENT_CHANNEL_CONNECTION = 3,
    CMUX_EVENT_CHANNEL_DISCONNECTION = 4,
    CMUX_EVENT_READY_TO_READ = 5,
    CMUX_EVENT_READY_TO_WRITE = 6,
    CMUX_EVENT_MODEM_ABNORMAL = 7,
    CMUX_EVENT_MAX
} cmux_event_t;

typedef enum {
    CMUX_ROLE_SERVER,
    CMUX_ROLE_CLIENT,
    CMUX_ROLE_MAX
} cmux_role_t;

typedef uint32_t cmux_channel_id_t;

typedef struct {
    cmux_channel_id_t channel_id;
} cmux_event_connection_disconnection_read_write_t;

typedef cmux_event_connection_disconnection_read_write_t cmux_event_channel_connection_t;
typedef cmux_event_connection_disconnection_read_write_t cmux_event_channel_disconnection_t;
typedef cmux_event_connection_disconnection_read_write_t cmux_event_ready_to_read_t;
typedef cmux_event_connection_disconnection_read_write_t cmux_event_ready_to_write_t;

typedef void (*cmux_callback_t)(cmux_event_t event, void *param);

int32_t cmux_register_callback(cmux_channel_type_t channel_type, cmux_callback_t callback);

cmux_status_t cmux_create_task(hal_uart_port_t port, cmux_role_t role);

cmux_status_t cmux_enable(void);

cmux_status_t cmux_disable(void);

uint32_t cmux_write_data(cmux_channel_id_t channel_id, const uint8_t *data, uint32_t size);

uint32_t cmux_read_data(cmux_channel_id_t channel_id, uint8_t *buffer, uint32_t size);

uint32_t cmux_get_available_write_space(cmux_channel_id_t channel_id);

uint32_t cmux_get_available_read_length(cmux_channel_id_t channel_id);

#endif /* __CMUX_H__ */

