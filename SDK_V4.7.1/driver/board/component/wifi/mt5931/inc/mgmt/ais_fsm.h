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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/ais_fsm.h#22 $
*/

/*! \file   ais_fsm.h
    \brief  Declaration of functions and finite state machine for AIS Module.

    Declaration of functions and finite state machine for AIS Module.
*/




#ifndef _AIS_FSM_H
#define _AIS_FSM_H

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
#define AIS_BG_SCAN_INTERVAL_MIN_SEC        2 //30 // exponential to 960
#define AIS_BG_SCAN_INTERVAL_MAX_SEC        2 //960 // 16min

#define AIS_DELAY_TIME_OF_DISCONNECT_SEC    10

#define AIS_IBSS_ALONE_TIMEOUT_MSEC         500 // mili-seconds
#if 1
#define AIS_IBSS_ALONE_TIMEOUT_SEC          2  // seconds
#else
#define AIS_IBSS_ALONE_TIMEOUT_SEC          20  // seconds
#endif

#define AIS_BEACON_TIMEOUT_COUNT_ADHOC      30
#define AIS_BEACON_TIMEOUT_COUNT_INFRA      10
#define AIS_BEACON_TIMEOUT_GUARD_TIME_SEC   1 /* Second */

#define AIS_BEACON_MAX_TIMEOUT_TU           100
#define AIS_BEACON_MIN_TIMEOUT_TU           5
#define AIS_BEACON_MAX_TIMEOUT_VALID        TRUE
#define AIS_BEACON_MIN_TIMEOUT_VALID        TRUE

#define AIS_BMC_MAX_TIMEOUT_TU              100
#define AIS_BMC_MIN_TIMEOUT_TU              5
#define AIS_BMC_MAX_TIMEOUT_VALID           TRUE
#define AIS_BMC_MIN_TIMEOUT_VALID           TRUE

#define AIS_JOIN_CH_GRANT_THRESHOLD         10
#define AIS_JOIN_CH_REQUEST_INTERVAL        2000

#define AIS_BEACON_TIMEOUT_CONFIRM_MSEC     5000

#define AIS_BEACON_TIMEOUT_POLL_SEC         2

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_AIS_STATE_T {
    AIS_STATE_IDLE = 0,
    AIS_STATE_SEARCH,
    AIS_STATE_SCAN,
    AIS_STATE_ONLINE_SCAN,
    AIS_STATE_LOOKING_FOR,
    AIS_STATE_WAIT_FOR_NEXT_SCAN,
    AIS_STATE_REQ_CHANNEL_JOIN,
    AIS_STATE_JOIN,
    AIS_STATE_IBSS_ALONE,
    AIS_STATE_IBSS_MERGE,
    AIS_STATE_NORMAL_TR,
    AIS_STATE_DISCONNECTING,
    AIS_STATE_NUM
} ENUM_AIS_STATE_T;


typedef struct _MSG_AIS_ABORT_T {
    MSG_HDR_T           rMsgHdr;        /* Must be the first member */
    UINT_8              ucReasonOfDisconnect;
    BOOLEAN             fgDelayIndication;
} MSG_AIS_ABORT_T, *P_MSG_AIS_ABORT_T;


typedef struct _MSG_AIS_IBSS_PEER_FOUND_T {
    MSG_HDR_T       rMsgHdr;    /* Must be the first member */
    UINT_8          ucNetTypeIndex;
    BOOLEAN         fgIsMergeIn; /* TRUE: Merge In, FALSE: Merge Out */
    P_STA_RECORD_T  prStaRec;
} MSG_AIS_IBSS_PEER_FOUND_T, *P_MSG_AIS_IBSS_PEER_FOUND_T;

typedef enum _ENUM_AIS_REQUEST_TYPE_T {
    AIS_REQUEST_SCAN,
    AIS_REQUEST_RECONNECT,
    AIS_REQUEST_ROAMING_SEARCH,
    AIS_REQUEST_ROAMING_CONNECT,
    AIS_REQUEST_NUM
} ENUM_AIS_REQUEST_TYPE_T;

typedef struct _AIS_REQ_HDR_T {
    LINK_ENTRY_T            rLinkEntry;
    ENUM_AIS_REQUEST_TYPE_T eReqType;
} AIS_REQ_HDR_T, *P_AIS_REQ_HDR_T;


typedef struct _AIS_FSM_INFO_T {
    ENUM_AIS_STATE_T    ePreviousState;
    ENUM_AIS_STATE_T    eCurrentState;

    BOOLEAN             fgTryScan;

    BOOLEAN             fgIsInfraChannelFinished;
    BOOLEAN             fgIsChannelRequested;
    BOOLEAN             fgIsChannelGranted;

#if CFG_SUPPORT_ROAMING
    BOOLEAN             fgIsRoamingScanPending;
#endif /* CFG_SUPPORT_ROAMING */

    UINT_8              ucAvailableAuthTypes;       /* Used for AUTH_MODE_AUTO_SWITCH */

    P_BSS_DESC_T        prTargetBssDesc;            /* For destination */

    P_STA_RECORD_T      prTargetStaRec;             /* For JOIN Abort */

    UINT_32             u4SleepInterval;

    TIMER_T             rBGScanTimer;

    TIMER_T             rIbssAloneTimer;

    TIMER_T             rIndicationOfDisconnectTimer;

    TIMER_T             rJoinTimeoutTimer;

    UINT_8              ucSeqNumOfReqMsg;
    UINT_8              ucSeqNumOfChReq;
    UINT_8              ucSeqNumOfScanReq;

    UINT_32             u4ChGrantedInterval;

    UINT_8              ucConnTrialCount;

    UINT_8              ucScanSSIDLen;
    UINT_8              aucScanSSID[ELEM_MAX_LEN_SSID];

    /* Pending Request List */
    LINK_T              rPendingReqList;

} AIS_FSM_INFO_T, *P_AIS_FSM_INFO_T;


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
#define aisChangeMediaState(_prAdapter, _eNewMediaState) \
            (_prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].eConnectionState = (_eNewMediaState));


/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID
aisInitializeConnectionSettings(
    IN P_ADAPTER_T prAdapter,
    IN P_REG_INFO_T prRegInfo
);

VOID
aisFsmInit(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmUninit(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmStateInit_JOIN(
    IN P_ADAPTER_T prAdapter,
    P_BSS_DESC_T prBssDesc
);

BOOLEAN
aisFsmStateInit_RetryJOIN(
    IN P_ADAPTER_T      prAdapter,
    IN P_STA_RECORD_T   prStaRec
);

VOID
aisFsmStateInit_IBSS_ALONE(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmStateInit_IBSS_MERGE(
    IN P_ADAPTER_T prAdapter,
    P_BSS_DESC_T prBssDesc
);

VOID
aisFsmStateAbort(
    IN P_ADAPTER_T prAdapter,
    UINT_8         ucReasonOfDisconnect,
    BOOLEAN        fgDelayIndication
);

VOID
aisFsmStateAbort_JOIN(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmStateAbort_SCAN(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmStateAbort_NORMAL_TR(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmStateAbort_IBSS(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmSteps(
    IN P_ADAPTER_T prAdapter,
    ENUM_AIS_STATE_T eNextState
);

/*----------------------------------------------------------------------------*/
/* Mailbox Message Handling                                                   */
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventScanDone(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

VOID
aisFsmRunEventAbort(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

VOID
aisFsmRunEventJoinComplete(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

VOID
aisFsmRunEventFoundIBSSPeer(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

/*----------------------------------------------------------------------------*/
/* Handling for Ad-Hoc Network                                                */
/*----------------------------------------------------------------------------*/
VOID
aisFsmCreateIBSS(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmMergeIBSS(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

/*----------------------------------------------------------------------------*/
/* Handling of Incoming Mailbox Message from CNM                              */
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventChGrant(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);


/*----------------------------------------------------------------------------*/
/* Generating Outgoing Mailbox Message to CNM                                 */
/*----------------------------------------------------------------------------*/
VOID
aisFsmReleaseCh(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* Event Indication                                                           */
/*----------------------------------------------------------------------------*/
VOID
aisIndicationOfMediaStateToHost(
    IN P_ADAPTER_T prAdapter,
    ENUM_PARAM_MEDIA_STATE_T eConnectionState,
    BOOLEAN fgDelayIndication
);

VOID
aisPostponedEventOfDisconnTimeout(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
);

VOID
aisUpdateBssInfoForJOIN(
    IN P_ADAPTER_T prAdapter,
    P_STA_RECORD_T prStaRec,
    P_SW_RFB_T prAssocRspSwRfb
);

VOID
aisUpdateBssInfoForCreateIBSS(
    IN P_ADAPTER_T prAdapter
);

VOID
aisUpdateBssInfoForMergeIBSS(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

BOOLEAN
aisValidateProbeReq(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT PUINT_32 pu4ControlFlags
);

/*----------------------------------------------------------------------------*/
/* Disconnection Handling                                                     */
/*----------------------------------------------------------------------------*/
VOID
aisFsmDisconnect(
    IN P_ADAPTER_T prAdapter,
    IN BOOLEAN     fgDelayIndication
);


/*----------------------------------------------------------------------------*/
/* Event Handling                                                             */
/*----------------------------------------------------------------------------*/
VOID
aisBssBeaconTimeout(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
aisDeauthXmitComplete(
    IN P_ADAPTER_T              prAdapter,
    IN P_MSDU_INFO_T            prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T    rTxDoneStatus
);

#if CFG_SUPPORT_ROAMING
VOID
aisFsmRunEventRoamingDiscovery(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4ReqScan
);

ENUM_AIS_STATE_T
aisFsmRoamingScanResultsUpdate(
    IN P_ADAPTER_T prAdapter
);

VOID
aisFsmRoamingDisconnectPrevAP(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prTargetStaRec
);

VOID
aisUpdateBssInfoForRoamingAP(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN P_SW_RFB_T prAssocRspSwRfb
);
#endif /*CFG_SUPPORT_ROAMING */

/*----------------------------------------------------------------------------*/
/* Timeout Handling                                                           */
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventBGSleepTimeOut(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
);

VOID
aisFsmRunEventIbssAloneTimeOut(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
);

VOID
aisFsmRunEventJoinTimeout(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
);

/*----------------------------------------------------------------------------*/
/* OID/IOCTL Handling                                                         */
/*----------------------------------------------------------------------------*/
VOID
aisFsmScanRequest(
    IN P_ADAPTER_T prAdapter,
    IN P_PARAM_SSID_T prSsid
);

/*----------------------------------------------------------------------------*/
/* Internal State Checking                                                    */
/*----------------------------------------------------------------------------*/
BOOLEAN
aisFsmIsRequestPending(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_AIS_REQUEST_TYPE_T eReqType,
    IN BOOLEAN bRemove
);

P_AIS_REQ_HDR_T
aisFsmGetNextRequest(
    IN P_ADAPTER_T prAdapter
);

BOOLEAN
aisFsmInsertRequest(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_AIS_REQUEST_TYPE_T eReqType
);

VOID
aisFsmFlushRequest(
    IN P_ADAPTER_T prAdapter
);


#if defined(CFG_TEST_MGMT_FSM) && (CFG_TEST_MGMT_FSM != 0)
VOID
aisTest(
    VOID
);
#endif /* CFG_TEST_MGMT_FSM */
/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _AIS_FSM_H */




