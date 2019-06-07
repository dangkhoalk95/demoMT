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

#include "cmux_def.h"
#include "cmux_lib.h"
#include "cmux_vp_num.h"
#include "cmux_uh.h"
#include "cmux_hdlr.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_main.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_main.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_main.c] "fmt,##arg)

static CmuxContextT cmux_context;
CmuxContextT *const cmux_ptr = & cmux_context;

static char *const cmux_timer_name[] = { "T1", "T2", "T3" };
static uint32_t cmux_timer_id[] = { CMUX_T1, CMUX_T2, CMUX_T3 };

kal_bool cmux_init(void)
{
    static lcd_lqueue w_queue;
    kal_uint8 i;

    // Initialize CMUX's context
    CMUX_INIT_CONTEXT(cmux_ptr);

    CMUX_MODE_SETNORMAL(cmux_ptr);

    cmux_ptr->port = HAL_UART_MAX;
    cmux_ptr->writeQueue = lcd_create_lqueue_with_mem(5, pvPortMalloc, vPortFree,
                           &(w_queue)); // 5 is bin size

    // Initialize each timer: CMUX_T1 and CMUX_T2 use same timer instance
    for (i = 0; i < CMUX_TOT_TIMER; i++) {
        cmux_ptr->timer[i] = xTimerCreate(cmux_timer_name[i],
                                          0xffff, pdFALSE,
                                          (void *)cmux_timer_id[i],
                                          cmux_handleTimerExpiry);
    }

    // Initialize each DLC context
    for (i = 0; i < CMUX_CHANNEL_NUM; i++) {
        cmux_ptr->dlcs[i].state = DLC_INACTIVE;
    }

    // Initialize CMUX_UH's Context
    cmux_uh_init();

    return KAL_TRUE;
}

kal_bool cmux_dispatchMsg( ilm_struct *ilm_ptr )
{
    switch (ilm_ptr->msg_id) {
        /* CMUX */
        case MSG_ID_CMUX_STARTUP_REQ:
            cmux_handleCmuxStartupReq(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_CMUX_CLOSE_DOWN_REQ:
            cmux_handleCmuxCloseDownReq();
            break;

        case MSG_ID_CMUX_UART_CLOSE_DOWN_REQ:
            cmux_uh_handleCmuxUartCloseDownReq();
            break;

        case MSG_ID_CMUX_UART_CLOSE_DOWN_CNF:
            cmux_handleCmuxUartCloseDownCnf();
            break;

        case MSG_ID_CMUX_DLC_DATA_READY_REQ:
            cmux_handleCmuxDlcDataReadyReq(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_CMUX_DLC_READY_TO_RECEIVE_REQ:
            cmux_handleCmuxDlcReadyToReceiveReq(ilm_ptr->local_para_ptr);
            break;

        /* UART */
        case MSG_ID_UART_READY_TO_READ_IND:
            cmux_uh_handleUartReadyToReadInd();
            break;

        case MSG_ID_UART_READY_TO_WRITE_IND:
            cmux_uh_handleUartReadyToWriteInd();
            break;

        case MSG_ID_UART_PLUGOUT_IND:
            cmux_handleUartPlugoutInd();
            break;
    }

    return KAL_TRUE;
}

