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

#ifndef __CMUX_PORTING_H__
#define __CMUX_PORTING_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "syslog.h"
#include "hal_uart.h"
#include "memory_attribute.h"
#include "cmux.h"

#ifndef __CMUX_CL4_SUPPORT__
//#define __CMUX_CL4_SUPPORT__
#endif

#ifndef __CMUX_UT__
//#define __CMUX_UT__
#endif

#ifndef __CMUX_IT__
//#define __CMUX_IT__
#endif

typedef uint8_t kal_uint8;
typedef int8_t kal_int8;
typedef uint16_t kal_uint16;
typedef int16_t kal_int16;
typedef uint32_t kal_uint32;
typedef int32_t kal_int32;
typedef char kal_char;
typedef bool kal_bool;

#define KAL_TRUE 1
#define KAL_FALSE 0

typedef struct {
    kal_uint8 ref_count;
    kal_uint8 lp_reserved;
    kal_uint16 msg_len;
} local_para_struct;

typedef struct {
    kal_uint8 *writingBuffer;
    kal_uint16 u2ToWrite;
} peer_buff_struct;

typedef int32_t msg_type;

typedef struct {
    msg_type msg_id; /* Message identifier */
    local_para_struct *local_para_ptr; /* local_para pointer */
} ilm_struct;

typedef void **lcd_lqueue_bin;
typedef void *(*malloc_fp_t)(unsigned int size);
typedef void (*free_fp_t)(void *ptr);

typedef struct {
    unsigned int       bin_size;
    lcd_lqueue_bin     start;
    unsigned int       head;
    lcd_lqueue_bin     end;
    unsigned int       tail;
    unsigned int       bin_count;
    kal_bool           is_empty;
    kal_bool           is_cntxt_mem_usr_mem;
    malloc_fp_t        alloc_fn_p;
    free_fp_t          free_fn_p;
} lcd_lqueue;

extern lcd_lqueue *lcd_create_lqueue_with_mem(unsigned int bin_size, malloc_fp_t alloc_fn_p, free_fp_t free_fn_p, void *mem_ptr);
extern kal_bool lcd_lqueue_insert_at_rear(lcd_lqueue *q, void *item);
extern void *lcd_lqueue_remove(lcd_lqueue *q, void **mem_to_free);
extern kal_bool lcd_lqueue_is_empty(lcd_lqueue *q);

extern void cmux_task_create(TaskHandle_t *task_handle);
extern kal_bool cmux_send_message(msg_type msg_id, local_para_struct *local_para_ptr);
extern kal_bool cmux_dispatch_app_event(ilm_struct *ilm_ptr);
extern void cmux_negotiate_and_set_parameter(void);
extern void cmux_continue_negotiate_and_set_parameter(void);
extern void cmux_continue_establish_connection(void);
extern void cmux_check_and_handle_startup_req(void);

#endif /* __CMUX_PORTING_H__ */

