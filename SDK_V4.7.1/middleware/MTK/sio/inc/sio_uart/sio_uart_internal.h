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

#ifndef __SIO_UART_INTERNAL_H__
#define __SIO_UART_INTERNAL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "nvdm.h"
#include "hal_platform.h"
#include "timers.h"

#include "sio_uart_gprot.h"
#include "msm.h"

#define SIO_DEBUG


#define SIO_MAX_NOTIFIER_CALLBACK_CNT            (16)
#define SIO_MAX_UART_EVENT_CALLBACK_CNT          (16)
#define SIO_UART_LOCAL_QUEUE_LENGTH              (30)

/* UART related */
#define SIO_UART_RX_FIFO_ALERT_SIZE         (50)
#define SIO_UART_RX_FIFO_BUFFER_SIZE        (2048)
#define SIO_UART_RX_FIFO_THRESHOLD_SIZE     (80)
#define SIO_UART_TX_FIFO_THRESHOLD_SIZE     (51)
#define SIO_UART_TX_FIFO_BUFFER_SIZE        (2048)
#define SIO_TX_DATA_BUFFER_SIZE             (2048)

// nvdm setting
#define SIO_NVDM_GROUP_NAME                 "sio-uart"
#define SIO_NVDM_DATA_ITEM_TYPE_A           "modem_exception_timer"

#define SIO_UART_MSG_NUM 10

extern TimerHandle_t g_sio_uart_timer_modem_exception_handle;
extern TimerHandle_t g_sio_uart_timer_ut_handle;
extern TimerHandle_t g_sio_uart_timer_wait_sleep_handle;
extern TimerHandle_t g_sio_uart_timer_wait_response_handle;
extern TimerHandle_t g_sio_uart_timer_wait_ready_handle;

typedef enum {
    SIO_UART_TIMER_MODEM_EXCEPTION = 0,
    SIO_UART_TIMER_WAIT_RESPONSE,
    SIO_UART_TIMER_WAIT_SLEEP,
    SIO_UART_TIMER_WAIT_READY,
    SIO_UART_TIMER_UT,
    SIO_UART_TIMER_MAX_NUMBER
} sio_uart_timer_t;


typedef enum {
    MSG_ID_SIO_UART_IND = 2000,
    MSG_ID_SIO_MODEM_READY_IND,
    MSG_ID_SIO_EINT_MODEM_EXCEPTION_IND,
    MSG_ID_SIO_EINT_MODEM_WAKEUP_IND,
    MSG_ID_SIO_CMUX_DATA_IND,
    MSG_ID_SIO_URC_DATA_IND,
    MSG_ID_SIO_END
} sio_msg_id_t;


typedef enum {
    SIO_UART_TIMER_STATUS_RUNNING = 0,
    SIO_UART_TIMER_STATUS_DELETED,
    SIO_UART_TIMER_STATUS_INVALID,
    SIO_UART_TIMER_STATUS_NUMBER
} sio_uart_timer_status_t;

typedef enum {
    INPUT_STATE_NONE    = 0,
    INPUT_STATE_START   = 1,
    INPUT_STATE_END     = 2
}input_state_t;

typedef struct {
    sio_msg_id_t msg_id;
    uint8_t*     msg_data;
} sio_uart_general_msg_t;

#ifdef __CMUX_SUPPORT__
typedef struct {
    cmux_event_t event;
    cmux_channel_id_t channel_id;
} sio_cmux_msg_t;
#endif

typedef struct {
    uint8_t     buf[SIO_UART_RX_FIFO_BUFFER_SIZE];
    uint16_t    len;
    uint8_t     is_used;
} sio_uart_input_cmd_msg_t;

typedef struct {
    uint8_t             need_reass;
    sio_uart_input_cmd_msg_t *pre_data;
} sio_reass_data_t;

typedef struct{
    input_state_t state;
    int8_t send_flag;
    int16_t len;
    char buf[SIO_UART_RX_FIFO_BUFFER_SIZE];
}sio_reass_buff_t;


typedef struct
{
    uint8_t rx_buff[SIO_UART_RX_FIFO_BUFFER_SIZE];
    uint8_t tx_buff[SIO_UART_TX_FIFO_BUFFER_SIZE];
} sio_uart_context_buf_struct;

typedef struct sio_uart_notifier_callback_struct {
    sio_uart_event_t    events;
    int32_t             app_id;
    sio_uart_notifier_callback   func;
    struct sio_uart_notifier_callback_struct *next;
} sio_uart_notifier_callback_struct;

typedef struct sio_uart_rtx_data_struct {
    int32_t     app_id;
    uint8_t     *payload;
    void        *user_data;                       
} sio_uart_rtx_data_struct;

typedef struct sio_uart_event_callback_struct {
    sio_uart_uart_event_callback   func;
    struct sio_uart_event_callback_struct *next;
} sio_uart_event_callback_struct;

typedef struct sio_modem_response_struct {
    char* modem_response;
    uint8_t is_end;
    struct sio_modem_response_struct* next;
}sio_modem_response_struct;

typedef struct sio_uart_context_struct {
    uint32_t                 mutex;
    uint32_t                 semaphore;
    uint32_t                 input_queue;
    uint8_t                  is_trigger_modem_ready;
    sio_uart_status_t        modem_status;
    hal_uart_port_t          modem_port;    
    sio_uart_rtx_data_struct       *rtx_data;
    sio_uart_notifier_callback_struct  *notifier_cb_list;        // for user to listern sio-uart notify
    sio_uart_urc_callback              urc_cb;                   // for user to receive URC
    sio_uart_event_callback_struct     *uart_event_cb_list;      // for user to listen UART EVENT directly
    sio_modem_response_struct          *modem_response_list;
#ifdef HAL_SLEEP_MANAGER_ENABLED
    uint8_t                            sleep_manager_handler;
#endif
} sio_uart_context_struct;

#if XA_CONTEXT_PTR == XA_ENABLED
extern sio_uart_context_struct *sio_uart_context;
#define SIO_UART(s) (sio_uart_context->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern sio_uart_context_struct sio_uart_context;
#define SIO_UART(s) (sio_uart_context.s)
#endif /* XA_CONTEXT_PTR */

extern sio_uart_ret_t sio_uart_port_msg_handler(sio_uart_input_cmd_msg_t* input);

extern sio_uart_ret_t sio_uart_timer_create_start(sio_uart_timer_t timer);
extern sio_uart_ret_t sio_uart_timer_delete(sio_uart_timer_t timer);
extern sio_uart_ret_t sio_uart_timer_stop(sio_uart_timer_t timer);

extern sio_uart_ret_t sio_uart_event_proxy_handler(sio_uart_event_t event, void *data);
extern sio_uart_ret_t sio_uart_at_response_handler(uint8_t *payload, uint32_t length, uint8_t is_end);

extern sio_uart_ret_t sio_uart_set_status(sio_uart_status_t status);
extern void sio_uart_reset_send_data();

extern void sio_uart_timer_timeout_callback(TimerHandle_t tmr);
extern TickType_t sio_uart_timer_get_tick(sio_uart_timer_t timer);
extern sio_uart_ret_t sio_uart_eint_modem_wakeup_handler(void);
extern sio_uart_ret_t sio_uart_eint_modem_sleep_handler(void);


#endif /* __SIO_UART_INTERNAL_H__ */

