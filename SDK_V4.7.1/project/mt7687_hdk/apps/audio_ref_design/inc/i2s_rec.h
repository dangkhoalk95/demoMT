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



#ifndef __I2S_REC_H__
#define __I2S_REC_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#ifdef HAL_I2S_MODULE_ENABLED

#include "nau8810.h"

#include "hal_i2c_master.h"
#include "hal_i2s.h"
#include "hal_log.h"
#include "hal_gpt.h"



#ifdef __cplusplus
extern "C" {
#endif

#define     I2S_REC_LOG_ENABLE
#define     MAX_I2S_REC_FUNCTIONS          3       /** The maximum number of callback function. */
#define     I2S_REC_QUEUE_SIZE             10      /** The maximum number of items the queue can hold at any one time.*/
#define     MAXIMUM(A,B)                  (((A)>(B))?(A):(B))
#define     MINIMUM(A,B)                  (((A)<(B))?(A):(B))


/**  This enum define the queue events. */
typedef enum {
    I2S_REC_QUEUE_EVENT_NONE = 0,              /**  The default value. */
    I2S_REC_QUEUE_EVENT_FILL_DATA_PROCESS,     /**  Fill data event. */
    I2S_REC_QUEUE_EVENT_GET_DATA_PROCESS,     /**  Get data event. */
    I2S_REC_QUEUE_EVENT_TOTAL
} i2s_rec_queue_event_id_t;


/**  This struct define the queue event struct. */
typedef struct {
    i2s_rec_queue_event_id_t id;
    void *parameter;
} i2s_rec_queue_event_t;


/**  This function defines the queue callback function prototype.
 *    The user should register a callback function before sending the queue event.
 *    param[in] parameter is a parameter given by the user with #i2s_rec_callback_t().
 */
typedef void (*i2s_rec_callback_t)(void *parameter);



void i2s_rec_demo(void *arg);



#ifdef __cplusplus
}
#endif
#endif /*HAL_I2S_MODULE_ENABLED*/

#endif /*__I2S_RECORD_H__*/


