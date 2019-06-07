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

/*
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/aa_fsm.h#1 $
*/

/*! \file   aa_fsm.h
    \brief  Declaration of functions and finite state machine for SAA/AAA Module.

    Declaration of functions and finite state machine for SAA/AAA Module.
*/





#ifndef _AA_FSM_H
#define _AA_FSM_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* Retry interval for retransmiting authentication-request MMPDU. */
#define TX_AUTHENTICATION_RETRY_TIMEOUT_TU          100 // TU.

/* Retry interval for retransmiting association-request MMPDU. */
#define TX_ASSOCIATION_RETRY_TIMEOUT_TU             100 // TU.

/* Wait for a response to a transmitted authentication-request MMPDU. */
#define DOT11_AUTHENTICATION_RESPONSE_TIMEOUT_TU    512 // TU.

/* Wait for a response to a transmitted association-request MMPDU. */
#define DOT11_ASSOCIATION_RESPONSE_TIMEOUT_TU       512 // TU.

/* The maximum time to wait for JOIN process complete. */
#define JOIN_FAILURE_TIMEOUT_BEACON_INTERVAL        20 // Beacon Interval, 20 * 100TU = 2 sec.

/* Retry interval for next JOIN request. */
#define JOIN_RETRY_INTERVAL_SEC                     10 // Seconds

/* Maximum Retry Count for accept a JOIN request. */
#define JOIN_MAX_RETRY_FAILURE_COUNT                3 // Times

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_AA_STATE_T {
    AA_STATE_IDLE = 0,
    SAA_STATE_SEND_AUTH1,
    SAA_STATE_WAIT_AUTH2,
    SAA_STATE_SEND_AUTH3,
    SAA_STATE_WAIT_AUTH4,
    SAA_STATE_SEND_ASSOC1,
    SAA_STATE_WAIT_ASSOC2,
    AAA_STATE_SEND_AUTH2,
    AAA_STATE_SEND_AUTH4, // We may not use, because P2P GO didn't support WEP and 11r
    AAA_STATE_SEND_ASSOC2,
    AA_STATE_RESOURCE, // A state for debugging the case of out of msg buffer.
    AA_STATE_NUM
} ENUM_AA_STATE_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/* Routines in saa_fsm.c                                                      */
/*----------------------------------------------------------------------------*/
VOID
saaFsmSteps(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN ENUM_AA_STATE_T eNextState,
    IN P_SW_RFB_T prRetainedSwRfb
);

WLAN_STATUS
saaFsmSendEventJoinComplete(
    IN P_ADAPTER_T prAdapter,
    WLAN_STATUS rJoinStatus,
    P_STA_RECORD_T prStaRec,
    P_SW_RFB_T prSwRfb
);

VOID
saaFsmRunEventStart(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

WLAN_STATUS
saaFsmRunEventTxDone(
    IN P_ADAPTER_T              prAdapter,
    IN P_MSDU_INFO_T            prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T    rTxDoneStatus
);

VOID
saaFsmRunEventTxReqTimeOut(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

VOID
saaFsmRunEventRxRespTimeOut(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

VOID
saaFsmRunEventRxAuth(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

WLAN_STATUS
saaFsmRunEventRxAssoc(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

WLAN_STATUS
saaFsmRunEventRxDeauth(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

WLAN_STATUS
saaFsmRunEventRxDisassoc(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

VOID
saaFsmRunEventAbort(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

/*----------------------------------------------------------------------------*/
/* Routines in aaa_fsm.c                                                      */
/*----------------------------------------------------------------------------*/
VOID
aaaFsmRunEventRxAuth(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

WLAN_STATUS
aaaFsmRunEventRxAssoc(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

WLAN_STATUS
aaaFsmRunEventTxDone(
    IN P_ADAPTER_T              prAdapter,
    IN P_MSDU_INFO_T            prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T    rTxDoneStatus
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _AA_FSM_H */




