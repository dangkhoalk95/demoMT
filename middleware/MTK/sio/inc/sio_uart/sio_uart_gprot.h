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

#ifndef __SIO_UART_GPROT_H__
#define __SIO_UART_GPROT_H__

#include "hal_platform.h"

#ifdef __CMUX_SUPPORT__
#include "cmux.h"
#endif

//#define MODEM_SUPPORT_DTE    

#define  SIO_UART_MAX_APP_SUPPORT                 (5)
#define  SIO_UART_EVENT_ALL                       (0xFFFF)

typedef enum {
    SIO_UART_STATUS_INIT = 0,
    SIO_UART_STATUS_READY, 
    SIO_UART_STATUS_EXCEPTION
} sio_uart_status_t;

typedef enum {
#ifdef __CMUX_SUPPORT__
    SIO_UART_EVENT_MODEM_ABNORMAL = CMUX_EVENT_MODEM_ABNORMAL,   /* 7 */
    SIO_UART_EVENT_MODEM_READY = CMUX_EVENT_MAX,  /* 8 */
#else /* __CMUX_SUPPORT__ */
    SIO_UART_EVENT_MODEM_ABNORMAL,   /* 0 */
    SIO_UART_EVENT_MODEM_READY,  /* 1 */
#endif /* __CMUX_SUPPORT__ */
    SIO_UART_EVENT_MODEM_WAKEUP,
    SIO_UART_EVENT_MODEM_SLEEP,
    SIO_UART_EVENT_MODEM_DATA_TO_READ,
    SIO_UART_EVENT_MODEM_URC,
    SIO_UART_EVENT_MAX_NUMBER
} sio_uart_event_t;

 
typedef enum {
    SIO_UART_RET_OK = 0,                 /**< No error occurred during the function call. */
    SIO_UART_RET_ERROR = -1,             /**< Error occurred during the function call. */
    SIO_UART_RET_REGISTER_FAIL = -2,     /**< Fail to register command handler table. */
    SIO_UART_RET_UNREGISTER_FAIL = -3,   /**< Fail to unregister command handler table. */
    SIO_UART_RET_BUSY = -4,
    SIO_UART_RET_MODEM_NO_READY = -5,
    SIO_UART_RET_PARAMETER_ERROR = -6,
    SIO_UART_RET_STATUS_ERROR = -7,
    SIO_UART_RET_END = -100              /**< The end enum item. */
} sio_uart_ret_t;


typedef enum {
    SIO_UART_RET_CONTINUE = 0,                 /**< No error occurred during the function call. */
    SIO_UART_RET_DONE = -1,             
} sio_uart_event_cb_t;


typedef void (* sio_uart_notifier_callback)(uint32_t event, void *data);   // sio-uart event

typedef sio_uart_event_cb_t (* sio_uart_uart_event_callback)(hal_uart_callback_event_t event, void *parameter);   // uart event directly

typedef void (* sio_uart_urc_callback)(void *data, int32_t length);

extern sio_uart_ret_t sio_uart_init(hal_uart_port_t uart_port);
extern sio_uart_ret_t sio_uart_port_init(hal_uart_port_t uart_port);
extern sio_uart_ret_t sio_uart_port_deinit(void);

extern sio_uart_ret_t sio_copy_data(char *dest, char *src, int len);

extern uint32_t sio_uart_send_data(int32_t app_id, const uint8_t *data, uint32_t size); 
extern uint32_t sio_uart_receive_data(int32_t app_id, uint8_t *buffer, uint32_t size);

// user can receive UART EVENT directly
// SIO_UART_RET_CONTINUE: this uart event will send to other
// SIO_UART_RET_DONE: this uart event will not send to other
extern sio_uart_ret_t sio_uart_set_uart_event_callback(sio_uart_uart_event_callback event_callback);

extern sio_uart_ret_t sio_uart_set_notification_callback(uint8_t enabled, int32_t app_id, 
                 sio_uart_event_t events, sio_uart_notifier_callback callback);

extern void sio_uart_register_urc_callback(sio_uart_urc_callback callback);
extern sio_uart_ret_t sio_uart_unregister_urc_callback(sio_uart_urc_callback callback);

extern sio_uart_status_t sio_uart_get_status(void);
extern hal_uart_port_t sio_uart_get_modem_port();
extern void sio_uart_set_trigger_modem_ready();

extern void sio_uart_add_modem_response(const char* modem_response, uint8_t is_end);

extern void sio_uart_trigger_modem_wakeup();

extern void sio_uart_trigger_modem_sleep();

extern sio_uart_ret_t sio_uart_trigger_modem_ready();

#endif /* __SIO_UART_GPROT_H__ */

