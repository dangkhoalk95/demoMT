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

#include "cmux_porting.h"
#include "cmux_msg_id.h"
#include "task_def.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_create.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_create.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_create.c] "fmt,##arg)


extern QueueHandle_t g_cmux_queue;

#ifdef __CMUX_SUPPORT__
extern kal_bool cmux_init(void);
extern kal_bool cmux_dispatchMsg(ilm_struct *ilm_ptr);
#endif

static void cmux_task_main(void *arg)
{
    ilm_struct current_ilm;

    cmux_init();

    while (1) {
        if (xQueueReceive(g_cmux_queue, (void *)&current_ilm, portMAX_DELAY)) {
            LOGI("msg_id: %d", current_ilm.msg_id);

#ifdef __CMUX_SUPPORT__
            cmux_dispatchMsg(&current_ilm);
            cmux_dispatch_app_event(&current_ilm);
#endif
            if (current_ilm.local_para_ptr != NULL) {
                vPortFree(current_ilm.local_para_ptr);
            }
        }
    }
}

void cmux_task_create(TaskHandle_t *task_handle)
{
    xTaskCreate(cmux_task_main,
                CMUX_TASK_NAME,
                CMUX_TASK_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL,
                CMUX_TASK_PRIORITY,
                task_handle);
}

