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

#include "cmux_struct.h"
#include "cmux_def.h"
#include "cmux_lib.h"
#include "cmux_custom.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_vp.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_vp.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_vp.c] "fmt,##arg)

static void cmux_notifyDataAvailable( kal_uint16 port )
{
    cmux_dlc_data_ready_req_struct *pLocal =
        (cmux_dlc_data_ready_req_struct *) pvPortMalloc(sizeof( cmux_dlc_data_ready_req_struct));
    pLocal->port = port;

    cmux_send_message( MSG_ID_CMUX_DLC_DATA_READY_REQ, (local_para_struct *) pLocal);
}

static void cmux_notifyBufferAvailable( kal_uint16 port)
{
    cmux_dlc_ready_to_receive_req_struct *pLocal =
        (cmux_dlc_ready_to_receive_req_struct *) pvPortMalloc(sizeof(cmux_dlc_ready_to_receive_req_struct));
    pLocal->port = port;

    cmux_send_message( MSG_ID_CMUX_DLC_READY_TO_RECEIVE_REQ, (local_para_struct *) pLocal);
}

kal_uint16 CPort_GetBytes(kal_uint16 port, kal_uint8 *buffaddr, kal_uint16 length)
{
    kal_uint16 u2DataRead;
    CmuxDlcT *dlc;

    if ( cmux_ptr->state != CMUX_ACTIVE ) {
        return 0;
    }

    /* Get DLC contexts by port number */
    dlc = & cmux_ptr->dlcs[CMUX_MAP_VP2ID(port)];

    // ok get byte here
    u2DataRead = cmux_getBufferDataAvail(& dlc->rxBuffer);
    if (u2DataRead >= length) {
        u2DataRead = length;
    } else {
        dlc->fNotifyReadyToRead = KAL_TRUE;
        LOGW("fNotifyReadyToRead = KAL_TRUE");
    }

    if (u2DataRead > 0) {
        /* This will update global pRead */
        cmux_readBuffer(& dlc->rxBuffer, buffaddr, u2DataRead);
    }

    if (dlc->fLocalBusy == KAL_TRUE
            && (cmux_getBufferDataAvail(&dlc->rxBuffer) <= dlc->lowThreshold)) {
        // Notify CMUX about resumption of receiving
        cmux_notifyBufferAvailable(port);
    }

    return u2DataRead;
}

kal_uint16 CPort_PutBytes(kal_uint16 port, kal_uint8 *buffaddr, kal_uint16 length)
{
    kal_bool fSignalDataAvail = KAL_FALSE;
    kal_uint16 u2WriteLength;
    CmuxDlcT *dlc;

    /* Use this to judge if fSignalDataAvail is true after context switched by CMUX */
    kal_uint8 *pPreWrite = NULL;

    if ( cmux_ptr->state != CMUX_ACTIVE ) {
        return 0;
    }

    /* Get DLC contexts by port number */
    dlc = & cmux_ptr->dlcs[CMUX_MAP_VP2ID(port)];

    // ok get byte here
    if (cmux_isBufferEmpty(& dlc->txBuffer)) {
        fSignalDataAvail = KAL_TRUE;
        /* Note: don't send message here, or if CMUX task has higher
         *  priority, it will be awaken here, when data is not yet copied
         */
    } else {
        /* Save write poniter before write */
        pPreWrite = dlc->txBuffer.pWrite;
    }

    u2WriteLength = cmux_getBufferSpaceAvail(& dlc->txBuffer);
    if (u2WriteLength >= length) {
        u2WriteLength = length;
    } else {
        dlc->fNotifyReadyToWrite = KAL_TRUE;
        LOGW("fNotifyReadyToWrite = KAL_TRUE");
    }

//====> context switch check point A (pRead might be updated)

    cmux_writeBuffer(& dlc->txBuffer, buffaddr, u2WriteLength);

//====> context switch check point B (pWrite was updated befor pRead updated)

    if ((fSignalDataAvail) || (dlc->txBuffer.pRead == pPreWrite)) {
        cmux_notifyDataAvailable(port);
    }

    return u2WriteLength;
}

kal_uint16 CPort_GetRxDataAvail(kal_uint16 port)
{
    CmuxDlcT *dlc;

    if ( cmux_ptr->state != CMUX_ACTIVE ) {
        return 0;
    }

    dlc = & cmux_ptr->dlcs[CMUX_MAP_VP2ID(port)];

    if (DLC_ACTIVE != dlc->state) {
        return 0;
    } else {
        // ok get byte here
        return cmux_getBufferDataAvail(& dlc->rxBuffer);
    }
}

kal_uint16 CPort_GetTxRoomLeft(kal_uint16 port)
{
    CmuxDlcT *dlc;

    if ( cmux_ptr->state != CMUX_ACTIVE ) {
        return 0;
    }

    dlc = & cmux_ptr->dlcs[CMUX_MAP_VP2ID(port)];

    if (DLC_ACTIVE != dlc->state) {
        return 0;
    } else {
        // ok get byte here
        return cmux_getBufferSpaceAvail(& dlc->txBuffer);
    }
}

