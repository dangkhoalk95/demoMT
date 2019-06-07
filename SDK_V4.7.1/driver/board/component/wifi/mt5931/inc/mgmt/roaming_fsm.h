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
** $Id:
*/

/*! \file   "roaming_fsm.h"
    \brief  This file defines the FSM for Roaming MODULE.

    This file defines the FSM for Roaming MODULE.
*/




#ifndef _ROAMING_FSM_H
#define _ROAMING_FSM_H

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
/* Roaming Discovery interval, SCAN result need to be updated */
#define ROAMING_DISCOVERY_TIMEOUT_SEC               5 // Seconds.

//#define ROAMING_NO_SWING_RCPI_STEP                  5 //rcpi
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_ROAMING_FAIL_REASON_T {
    ROAMING_FAIL_REASON_CONNLIMIT = 0,
    ROAMING_FAIL_REASON_NOCANDIDATE,
    ROAMING_FAIL_REASON_NUM
} ENUM_ROAMING_FAIL_REASON_T;

/* events of roaming between driver and firmware */
typedef enum _ENUM_ROAMING_EVENT_T {
    ROAMING_EVENT_START = 0,
    ROAMING_EVENT_DISCOVERY,
    ROAMING_EVENT_ROAM,
    ROAMING_EVENT_FAIL,
    ROAMING_EVENT_ABORT,
    ROAMING_EVENT_NUM
} ENUM_ROAMING_EVENT_T;

typedef struct _ROAMING_PARAM_T {
    UINT_16     u2Event;
    UINT_16     u2Data;
} ROAMING_PARAM_T, *P_ROAMING_PARAM_T;

/**/
typedef enum _ENUM_ROAMING_STATE_T {
    ROAMING_STATE_IDLE = 0,
    ROAMING_STATE_DECISION,
    ROAMING_STATE_DISCOVERY,
    ROAMING_STATE_ROAM,
    ROAMING_STATE_NUM
} ENUM_ROAMING_STATE_T;

typedef struct _ROAMING_INFO_T {
    BOOLEAN                 fgIsEnableRoaming;

    ENUM_ROAMING_STATE_T    eCurrentState;

    OS_SYSTIME              rRoamingDiscoveryUpdateTime;

    /* brief disable/enable roaming dynamically by ignore FW roaming event,  TRUE: enable, FALSE:disable*/
    kal_bool                fgBriefEnableRoamingDynamically;
} ROAMING_INFO_T, *P_ROAMING_INFO_T;

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

#if CFG_SUPPORT_ROAMING
#define IS_ROAMING_ACTIVE(prAdapter) \
        (prAdapter->rWifiVar.rRoamingInfo.eCurrentState == ROAMING_STATE_ROAM)
#else
#define IS_ROAMING_ACTIVE(prAdapter) FALSE
#endif /* CFG_SUPPORT_ROAMING */

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID
roamingFsmInit(
    IN P_ADAPTER_T prAdapter
);

VOID
roamingFsmUninit(
    IN P_ADAPTER_T prAdapter
);

VOID
roamingFsmSendCmd(
    IN P_ADAPTER_T prAdapter,
    IN P_ROAMING_PARAM_T prParam
);

VOID
roamingFsmScanResultsUpdate(
    IN P_ADAPTER_T prAdapter
);

VOID
roamingFsmSteps(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_ROAMING_STATE_T eNextState
);

VOID
roamingFsmRunEventStart(
    IN P_ADAPTER_T prAdapter
);

VOID
roamingFsmRunEventDiscovery(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4Param
);

VOID
roamingFsmRunEventRoam(
    IN P_ADAPTER_T prAdapter
);

VOID
roamingFsmRunEventFail(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4Reason
);

VOID
roamingFsmRunEventAbort(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
roamingFsmProcessEvent(
    IN P_ADAPTER_T prAdapter,
    IN P_ROAMING_PARAM_T prParam
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _ROAMING_FSM_H */




