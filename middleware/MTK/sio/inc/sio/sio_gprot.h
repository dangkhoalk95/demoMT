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

#ifndef SIO_GPORT_H
#define SIO_GPORT_H

#ifdef __CMUX_SUPPORT__
#include "cmux.h"
#endif

#include "sio_uart_gprot.h"

//#define SIO_PRINTF

#define SIO_MODE_TYPE_UART            (0x01)
#define SIO_MODE_TYPE_CMUX            (0x02)
#define SIO_DATA_TYPE_COMMAND         (0x10)
#define SIO_DATA_TYPE_DATA            (0x20)
#define sio_get_channel_id(app_id)    (app_id & 0x0F)

typedef enum {
    SIO_APP_TYPE_CMUX_AT_CMD = 0x01,
    SIO_APP_TYPE_CMUX_IP_DATA = 0x02,
    SIO_APP_TYPE_UART_AT_CMD = 0x10,
    SIO_APP_TYPE_MAX
} sio_app_type_t;


typedef enum {
    SIO_RET_OK = 0,                 /**< No error occurred during the function call. */
    SIO_RET_BUSY = -1,             /**< Error occurred during the function call. */
    SIO_RET_ERROR = -2,             /**< Error occurred during the function call. */
    SIO_RET_END = -100              /**< The end enum item. */
} sio_ret_t;


typedef enum {
    SIO_UART_EVENT_CALLBACK_CONTINUE = 0,                 /**< No error occurred during the function call. */
    SIO_UART_EVENT_CALLBACK_DONE = -1,             
} sio_uart_event_cb_ret_t;


typedef struct sio_rx_data_to_read_struct {
    int32_t     app_id;
    int32_t     read_length;
} sio_rx_data_to_read_struct;


typedef void (* sio_event_handler)(uint32_t event, void *data);
 
extern sio_ret_t sio_init(hal_uart_port_t uart_port);

// need for CMUX
extern sio_ret_t sio_set_mode(int32_t app_id, int32_t flag);

extern int32_t sio_register_event_notifier(sio_app_type_t type, sio_event_handler notifier);

extern uint32_t sio_send_data(int32_t app_id, const uint8_t *data, uint32_t size);

extern uint32_t sio_receive_data(int32_t app_id, uint8_t *buffer, uint32_t size);
 
extern void sio_add_modem_response(const char* modem_response, uint8_t is_end);
#endif
