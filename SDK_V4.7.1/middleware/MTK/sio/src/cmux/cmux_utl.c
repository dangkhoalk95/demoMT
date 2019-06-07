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
#include "cmux_vp_num.h"
#include "cmux_custom.h"
#include "cmux_lib.h"
#include "cmux_struct.h"
#include "cmux_utl.h"
#include "cmux_prot.h"
#include "cmux_uh.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_utl.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_utl.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_utl.c] "fmt,##arg)

CmuxDlcT *cmux_findDlcByDlci(kal_uint8 dlci)
{
    kal_uint32 i;
    for (i = 0; i < CMUX_CHANNEL_NUM; i++) {
        if ( cmux_ptr->dlcs[i].pn_dlci == dlci ) {
            return & (cmux_ptr->dlcs[i]);
        }
    }
    return NULL;
}

CmuxDlcT *cmux_findDlcByDlciState(kal_uint32 dlci, CmuxDlcStateE state)
{
    CmuxDlcT *dlc = cmux_findDlcByDlci( dlci );

    if ( dlc == NULL ) {
        return NULL;
    } else {
        return ( dlc->state == state ) ? dlc : NULL;
    }
}

CmuxDlcT *cmux_locateSuitableDlc( kal_uint32 dlci )
{
    kal_uint32 i;
    CmuxDlcT *dlc;

    // if PrePn is turned on in custom file, find the slot by setting in custom file
    if ( cmux_custom_isPrePn() == KAL_TRUE ) {
        for ( i = 1; i <= CMUX_DLC_NUM ; i++ ) {
            if ( dlci == cmux_custom_getChannelDlci(i) ) {
                dlc = &(cmux_ptr->dlcs[i]);
                if ( dlc->state == DLC_INACTIVE ) {
                    return dlc;
                } else {
                    return NULL;
                }
            }
        }

        // find free slot in channel list
        for ( i = 1; i <= CMUX_DLC_NUM ; i++ ) {
            dlc = &(cmux_ptr->dlcs[i]);

            if ( ( dlc->state == DLC_INACTIVE) && ( cmux_custom_getChannelDlci(i) == 0 ) ) {
                return dlc;
            }
        }
    } else {
        // find free slot in channel list
        for ( i = 1; i <= CMUX_DLC_NUM ; i++ ) {
            dlc = &(cmux_ptr->dlcs[i]);
            if ( dlc->state == DLC_INACTIVE ) {
                return dlc;
            }
        }
    }

    return NULL;
}

void cmux_initializeDlc( CmuxDlcT *pDlc, kal_uint32 dlci )
{
    kal_uint8 id = CMUX_MAP_PDLC2ID(pDlc);

    if ( cmux_custom_isPrePn() == KAL_TRUE ) {
        pDlc->pn_cltype = ( CmuxClTypeE ) cmux_custom_getChannelClType(id);
        pDlc->pn_priority = cmux_custom_getChannelPriority(id);
        pDlc->pn_n1 = cmux_custom_getChannelN1(id);
    } else {
        pDlc->pn_cltype = ( CmuxClTypeE ) cmux_custom_getChannelClType(id);
        pDlc->pn_priority = cmux_lookupPriority(dlci);
        pDlc->pn_n1 = CMUX_DEFAULT_N1;
    }

    //  set dlc parameters
    pDlc->pn_dlci = dlci;

    // Initialize secheduling variable
    pDlc->u1AssignedPriority = CMUX_SUSPEND_PRIORITY;

    // Initialize flags
    pDlc->fPeerBusy = KAL_FALSE;
    pDlc->fLocalBusy = KAL_FALSE;
    pDlc->fSignalDcd = KAL_FALSE;
    pDlc->fSignalRi = KAL_FALSE;
    pDlc->fSignalBreak = KAL_FALSE;

    pDlc->fNotifyReadyToRead = KAL_TRUE;
    pDlc->fNotifyReadyToWrite = KAL_FALSE;

    // Initialize statistic variables
    pDlc->maxRxBufferUsage = 0;

#ifdef __CMUX_CL4_SUPPORT__
    // Initialize CL4 related context
    if ( pDlc->pn_cltype == CMUX_CL4) {
        pDlc->stateCL4Rx = CMUX_CL4_BEGIN;
        pDlc->stateCL4Tx = CMUX_CL4_BEGIN;
    }
#else
    // CMUX_CL4 is not support anymore
    CMUX_ASSERT(pDlc->pn_cltype != CMUX_CL4);
#endif

    return;
}

void cmux_allocateBufferSpace( CmuxDlcT *pDlc )
{
    /* how do we decide the buffer size of the DLC ?*/
    kal_uint32 size1, size2;
    kal_uint8 *pMem;
    kal_uint32 lowThreshold = 0, highThreshold = 0;

    kal_uint8 id = (kal_uint8) (pDlc - cmux_ptr->dlcs);

    pMem = cmux_getDlcTxBuffer(id, & size1);
    cmux_initBuffer(& pDlc->txBuffer, pMem, size1);

    pMem = cmux_getDlcRxBuffer(id, & size2, &lowThreshold, &highThreshold);
    cmux_initBuffer(& pDlc->rxBuffer, pMem, size2);

    LOGI("id = %d, Tx size = %d, Rx size = %d", id, size1, size2);

    pDlc->highThreshold = highThreshold;
    pDlc->lowThreshold = lowThreshold;

}

void cmux_releaseDlc(CmuxDlcT *pDlc)
{
    pDlc->state = DLC_PARNEG;

    cmux_initBuffer(& pDlc->txBuffer, NULL, 0);
    cmux_initBuffer(& pDlc->rxBuffer, NULL, 0);

}

void cmux_schedulerDlcReady(CmuxDlcT *pDlc)
{
    pDlc->u1AssignedPriority = pDlc->pn_priority;

    return;
}

kal_bool cmux_schedulerIsDlcReady(CmuxDlcT *pDlc)
{
    if (pDlc->u1AssignedPriority == CMUX_SUSPEND_PRIORITY) {
        return KAL_FALSE;
    } else {
        return KAL_TRUE;
    }
}

void cmux_schedulerDlcSuspend(CmuxDlcT *pDlc)
{
    pDlc->u1AssignedPriority = CMUX_SUSPEND_PRIORITY;

    return;
}

CmuxDlcT *cmux_schedulerPickReadyDlc(void)
{
    kal_uint32 i;
    CmuxDlcT *target = NULL;
    kal_uint32 priority = CMUX_SUSPEND_PRIORITY;

    for (i = 0; i < CMUX_CHANNEL_NUM; i++) {
        if (cmux_ptr->dlcs[i].state == DLC_ACTIVE) {
            if (priority > cmux_ptr->dlcs[i].u1AssignedPriority) {
                priority = cmux_ptr->dlcs[i].u1AssignedPriority;
                target = & cmux_ptr->dlcs[i];
            }
        }
    }

    return target;
}

kal_uint32 cmux_lookupPriority(kal_uint32 dlci)
{
    /* base on table 27 */
    if (dlci == 0) {
        return 0;
    }
    /* else */
    return (dlci / 8) + 7;
}

void cmux_notifyReadyToRead(CmuxDlcT *pDlc)
{
    cmux_event_ready_to_read_t *local_para =
        (cmux_event_ready_to_read_t *) pvPortMalloc(sizeof(cmux_event_ready_to_read_t));
    local_para->channel_id = pDlc->pn_dlci;

    cmux_send_message( CMUX_EVENT_READY_TO_READ, (local_para_struct *) local_para );
}

void cmux_notifyReadyToWrite(CmuxDlcT *pDlc)
{
    cmux_event_ready_to_write_t *local_para =
        (cmux_event_ready_to_write_t *) pvPortMalloc(sizeof(cmux_event_ready_to_write_t));
    local_para->channel_id = pDlc->pn_dlci;

    cmux_send_message( CMUX_EVENT_READY_TO_WRITE, (local_para_struct *) local_para );
}

void cmux_notifyDlcConnectInd(CmuxDlcT *pDlc)
{
    cmux_event_channel_connection_t *local_para =
        (cmux_event_channel_connection_t *)pvPortMalloc(sizeof(cmux_event_channel_connection_t));
    local_para->channel_id = pDlc->pn_dlci;

    cmux_send_message( CMUX_EVENT_CHANNEL_CONNECTION, (local_para_struct *) local_para );
}

void cmux_notifyCPortPlugout(CmuxDlcT *pDlc)
{
    cmux_event_channel_disconnection_t *local_para =
        (cmux_event_channel_disconnection_t *)pvPortMalloc(sizeof(cmux_event_channel_disconnection_t));
    local_para->channel_id = pDlc->pn_dlci;

    cmux_send_message( CMUX_EVENT_CHANNEL_DISCONNECTION, (local_para_struct *) local_para );
}

void cmux_writeToUart(kal_uint8 *pPdu, kal_uint16 u2ToWrite)
{
    if (cmux_ptr->fUartSendSuspend == KAL_FALSE) {
        cmux_ptr->fUartSendSuspend = KAL_TRUE;

        cmux_uh_handleCmuxDataReq(pPdu, u2ToWrite);
    } else {
        peer_buff_struct *pPeer = (peer_buff_struct *) pvPortMalloc(sizeof(peer_buff_struct));
        pPeer->writingBuffer = pPdu;
        pPeer->u2ToWrite = u2ToWrite;
        kal_bool ret_val = lcd_lqueue_insert_at_rear(cmux_ptr->writeQueue, pPeer);

        CMUX_ASSERT( ret_val == KAL_TRUE );
    }
}

void cmux_startTimer(CmuxTimerIdE timerId, kal_uint32 value)
{
    kal_uint32 idx = timerId;
    xTimerChangePeriod( cmux_ptr->timer[idx], value / portTICK_PERIOD_MS, 0 );
    xTimerStart( cmux_ptr->timer[idx], 0 );
}

void cmux_stopTimer(CmuxTimerIdE timerId)
{
    kal_uint32 idx = timerId;
    xTimerStop( cmux_ptr->timer[idx], 0 );
}

