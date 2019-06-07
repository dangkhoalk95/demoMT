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

#include "cmux_uh.h"
#include "cmux_def.h"
#include "cmux_utl.h"
#include "cmux_vp_num.h"
#include "cmux_struct.h"
#include "cmux_hdlr.h"
#include "cmux_prot.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_hdlr.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_hdlr.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_hdlr.c] "fmt,##arg)

void cmux_handleCmuxStartupReq(local_para_struct *localPara)
{
    cmux_startup_req_struct *starupReq = (cmux_startup_req_struct *)localPara;

    // CMUX Context Initialization
    cmux_ptr->port = starupReq->port;
    cmux_ptr->role = starupReq->role;
    cmux_ptr->state = CMUX_STARTUP;

    // RX
    cmux_ptr->readState  = CMUX_WAIT_FLAG;
    cmux_ptr->readingDlc = NULL;
    cmux_ptr->cchUIHPoll = 0;

    // TX
    cmux_ptr->fUartSendSuspend = KAL_FALSE;

    // try to read UART
    cmux_uh_handleStartupReq(localPara);
}

void cmux_handleCmuxCloseDownReq(void)
{
    // First, close all DLC
    // Then start CLD procedure
    // after UART finish send CLD response, Notify L4C
    cmux_ptr->state = CMUX_CLOSING;
    cmux_closeDownProcedure();
}

void cmux_handleCmuxDlcDataReadyReq( local_para_struct *localPtr )
{
    cmux_dlc_data_ready_req_struct *dataReady =
        (cmux_dlc_data_ready_req_struct *) localPtr;
    CmuxDlcT *pDlc;

    pDlc = & cmux_ptr->dlcs[CMUX_MAP_VP2ID(dataReady->port)];

    if ( pDlc->fPeerBusy == KAL_FALSE ) {
        cmux_schedulerDlcReady(pDlc);
    } else {
        LOGI("pn_dlci: %d", pDlc->pn_dlci);
    }

    if (cmux_ptr->fUartSendSuspend == KAL_FALSE) {
        CMUX_ASSERT(lcd_lqueue_is_empty(cmux_ptr->writeQueue) == KAL_TRUE);

        LOGI("schedule to write");
        cmux_scheduleWrite();
    } else {
        LOGW("fUartSendSuspend = KAL_TRUE");
    }
}

void cmux_handleCmuxDlcReadyToReceiveReq(local_para_struct *localPtr)
{
    cmux_dlc_ready_to_receive_req_struct *spaceReady =
        (cmux_dlc_ready_to_receive_req_struct *) localPtr;
    CmuxDlcT *pDlc;

    pDlc = & cmux_ptr->dlcs[CMUX_MAP_VP2ID(spaceReady->port)];

    pDlc->fLocalBusy = KAL_FALSE;

    // Resume peer by MSC msg
    cmux_resumePeerDlc(pDlc->pn_dlci);

    // Send flow control message by uih frame for CL2 only
    if (pDlc->pn_cltype == CMUX_CL2) {
        cmux_schedulerDlcReady(pDlc);
    }
    if (cmux_ptr->fUartSendSuspend == KAL_FALSE) {
        LOGI("schedule to write");
        cmux_scheduleWrite();
    } else {
        LOGW("fUartSendSuspend = KAL_TRUE");
    }

    // This CMUX_READY_TO_SEND_IND may be redundant
    cmux_uh_handleCmuxReadyToSendInd();
}

void cmux_handleReadyToSendInd(void)
{
    if (lcd_lqueue_is_empty(cmux_ptr->writeQueue)) {
        cmux_ptr->fUartSendSuspend = KAL_FALSE;

        LOGI("schedule to write");

        cmux_scheduleWrite();
    } else {
        peer_buff_struct *pPeer = (peer_buff_struct *)
                                  lcd_lqueue_remove(cmux_ptr->writeQueue, NULL);

        LOGI("lcd_lqueue_remove");

        cmux_uh_handleCmuxDataReq(pPeer->writingBuffer, pPeer->u2ToWrite);
        vPortFree(pPeer);
    }
}

void cmux_handleUartDataInd(kal_uint8 *pPdu, kal_uint16 u2Length)
{
    // Decode frame structure
    cmux_processUartData(pPdu, u2Length);

    // Maybe flow control condition of some DLC changes
    if ( cmux_ptr->fUartSendSuspend == KAL_FALSE ) {
        LOGI("schedule to write");
        cmux_scheduleWrite();
    }

    cmux_uh_handleCmuxReadyToSendInd();
}

void cmux_handleCmuxUartCloseDownCnf(void)
{
    kal_uint32 i;

    for (i = 0; i < CMUX_CHANNEL_NUM; i++) {
        cmux_ptr->dlcs[i].state = DLC_INACTIVE;
    }
    cmux_ptr->port = HAL_UART_MAX;
    cmux_ptr->state = CMUX_INACTIVE;

    cmux_send_message( CMUX_EVENT_CLOSE_DOWN, NULL );
}

void cmux_handleTimerExpiry (TimerHandle_t xTimer)
{
    uint32_t timer_id = (uint32_t)pvTimerGetTimerID(xTimer);

    switch (timer_id) {
        case CMUX_T1:
            cmux_handleT1Expiry();
            break;
        case CMUX_T2:
            cmux_handleT2Expiry();
            break;
        case CMUX_T3:
            cmux_handleT3Expiry();
            break;
    }
}

bool cmux_handleUartPlugoutInd( void )
{
    if ( cmux_ptr->state != CMUX_INACTIVE ) {
        cmux_restoreTask();
        return true;
    } else {
        return false;
    }
}

