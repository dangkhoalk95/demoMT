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

#ifndef _WATCH_H
#define _WATCH_H

#include "task.h"
#include "queue.h"

/* Type define */

typedef enum _tEventType {
    EVENT_NO,
    EVEINT_1MIN,
    EVENT_BUTTON_1,
    EVENT_BUTTON_2,
    EVENT_BUTTON_LEFT_LONG_PRESS,
    EVENT_BUTTON_PWR_LONG_PRESS,
    EVENT_BUTTON_3,
    EVENT_BUTTON_4,
    EVENT_BUTTON_5,
    EVENT_BUTTON_6,
    EVENT_UPDATE_LCD,
    EVENT_UPDATE_PEDO,
    EVENT_UPDATE_HR,
    EVENT_UPDATE_HRV,
    EVENT_UPDATE_SLEEP,
    EVENT_UPDATE_BP,
    EVENT_INIT_APP,
    EVENT_TASK_END,
    EVENT_TASK_FNBUTTON,
    EVENT_TASK_DONE,
    EVENT_KEY1_PRESS,
    EVENT_KEY2_PRESS,
    EVENT_KEY3_PRESS,
    EVENT_TIMER,
} tEventType;

typedef struct {
    tEventType event;
    void *userdata;
} tEvent;


/* ------------- Golbal variable --------------------------------*/
/***********************************************************************************
 *  Task for all app
 ***********************************************************************************/
#define STACK_SIZE_FOR_TASK    	(configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          	(tskIDLE_PRIORITY + 1)
#define SYS_TICK				( portTICK_RATE_MS )
#define DELAY_MAX				( portMAX_DELAY )


extern xQueueHandle queAppTask;		// queue to AppTask
extern xQueueHandle queAppMgr;		// queue to AppMgr
//extern xQueueHandle queDevMgr;		// queue to DevMgr
extern TimerHandle_t vTestTimer;

void battery_mgr_Task(void *pParameters);

#endif

