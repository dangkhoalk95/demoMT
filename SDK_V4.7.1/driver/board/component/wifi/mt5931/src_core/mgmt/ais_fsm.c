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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/ais_fsm.c#90 $
*/

/*! \file   "aa_fsm.c"
    \brief  This file defines the FSM for SAA and AAA MODULE.

    This file defines the FSM for SAA and AAA MODULE.
*/




/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"
#include "wndrv_api.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define AIS_ROAMING_CONNECTION_TRIAL_LIMIT  2

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
extern kal_uint32     BT_STATE_Report2WLAN;

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
#if DBG
/*lint -save -e64 Type mismatch */
static PUINT_8 apucDebugAisState[AIS_STATE_NUM] = {
    (PUINT_8)DISP_STRING("AIS_STATE_IDLE"),
    (PUINT_8)DISP_STRING("AIS_STATE_SEARCH"),
    (PUINT_8)DISP_STRING("AIS_STATE_SCAN"),
    (PUINT_8)DISP_STRING("AIS_STATE_ONLINE_SCAN"),
    (PUINT_8)DISP_STRING("AIS_STATE_LOOKING_FOR"),
    (PUINT_8)DISP_STRING("AIS_STATE_WAIT_FOR_NEXT_SCAN"),
    (PUINT_8)DISP_STRING("AIS_STATE_REQ_CHANNEL_JOIN"),
    (PUINT_8)DISP_STRING("AIS_STATE_JOIN"),
    (PUINT_8)DISP_STRING("AIS_STATE_IBSS_ALONE"),
    (PUINT_8)DISP_STRING("AIS_STATE_IBSS_MERGE"),
    (PUINT_8)DISP_STRING("AIS_STATE_NORMAL_TR"),
    (PUINT_8)DISP_STRING("AIS_STATE_DISCONNECTING")
};
/*lint -restore */
#endif /* DBG */

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
extern VOID
kalScanDone(
    IN P_GLUE_INFO_T                 prGlueInfo,
    IN ENUM_KAL_NETWORK_TYPE_INDEX_T eNetTypeIdx,
    IN WLAN_STATUS                   status
);
/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/*!
* @brief the function is used to initialize the value of the connection settings for
*        AIS network
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisInitializeConnectionSettings(
    IN P_ADAPTER_T prAdapter,
    IN P_REG_INFO_T prRegInfo
)
{
    P_CONNECTION_SETTINGS_T prConnSettings;
    UINT_8 aucAnyBSSID[] = BC_BSSID;
    UINT_8 aucZeroMacAddr[] = NULL_MAC_ADDR;

    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    /* Setup default values for operation */
    COPY_MAC_ADDR(prConnSettings->aucMacAddress, aucZeroMacAddr);

    prConnSettings->ucDelayTimeOfDisconnectEvent = AIS_DELAY_TIME_OF_DISCONNECT_SEC;

    COPY_MAC_ADDR(prConnSettings->aucBSSID, aucAnyBSSID);
    prConnSettings->fgIsConnByBssidIssued = FALSE;

    prConnSettings->fgIsConnReqIssued = FALSE;

    prConnSettings->ucSSIDLen = 0;

    prConnSettings->eOPMode = NET_TYPE_INFRA;

    prConnSettings->eConnectionPolicy = CONNECT_BY_SSID_BEST_RSSI;

    if (prRegInfo) {
        prConnSettings->ucAdHocChannelNum = (UINT_8) nicFreq2ChannelNum(prRegInfo->u4StartFreq);
        prConnSettings->eAdHocBand =  prRegInfo->u4StartFreq < 5000000 ? BAND_2G4 : BAND_5G;
        prConnSettings->eAdHocMode = (ENUM_PARAM_AD_HOC_MODE_T)(prRegInfo->u4AdhocMode);
    }

    prConnSettings->eAuthMode = AUTH_MODE_OPEN;

    prConnSettings->eEncStatus = ENUM_ENCRYPTION_DISABLED;

    prConnSettings->fgIsScanReqIssued = FALSE;

    /* MIB attributes */
    prConnSettings->u2BeaconPeriod = DOT11_BEACON_PERIOD_DEFAULT;

    prConnSettings->u2RTSThreshold = DOT11_RTS_THRESHOLD_DEFAULT;

    prConnSettings->u2DesiredNonHTRateSet = RATE_SET_ALL_ABG;

    //prConnSettings->u4FreqInKHz; /* Center frequency */


    /* Set U-APSD AC */
    prConnSettings->bmfgApsdEnAc = PM_UAPSD_NONE;

    secInit(prAdapter, NETWORK_TYPE_AIS_INDEX);

    /* Features */
    prConnSettings->fgIsEnableRoaming = FALSE;

#if CFG_SUPPORT_ROAMING
    prConnSettings->fgIsEnableRoaming = TRUE; //((prRegInfo->fgEnRoaming > 0)?(TRUE):(FALSE));
#endif /* CFG_SUPPORT_ROAMING */

    prConnSettings->fgIsAdHocQoSEnable = FALSE;

    prConnSettings->eDesiredPhyConfig = PHY_CONFIG_802_11ABGN;

    return;
} /* end of aisFsmInitializeConnectionSettings() */


/*----------------------------------------------------------------------------*/
/*!
* @brief the function is used to initialize the value in AIS_FSM_INFO_T for
*        AIS FSM operation
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmInit(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    //P_AIS_SPECIFIC_BSS_INFO_T prAisSpecificBssInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "aisFsmInit()");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    //prAisSpecificBssInfo = &(prAdapter->rWifiVar.rAisSpecificBssInfo);

    //4 <1> Initiate FSM
    prAisFsmInfo->ePreviousState = AIS_STATE_IDLE;
    prAisFsmInfo->eCurrentState = AIS_STATE_IDLE;

    prAisFsmInfo->ucAvailableAuthTypes = 0;

    prAisFsmInfo->prTargetBssDesc = (P_BSS_DESC_T)NULL;

    prAisFsmInfo->ucSeqNumOfReqMsg = 0;
    prAisFsmInfo->ucSeqNumOfChReq = 0;
    prAisFsmInfo->ucSeqNumOfScanReq = 0;

    prAisFsmInfo->fgIsInfraChannelFinished = TRUE;
#if CFG_SUPPORT_ROAMING
    prAisFsmInfo->fgIsRoamingScanPending = FALSE;
#endif /* CFG_SUPPORT_ROAMING */
    prAisFsmInfo->fgIsChannelRequested = FALSE;
    prAisFsmInfo->fgIsChannelGranted = FALSE;

    //4 <1.1> Initiate FSM - Timer INIT
    cnmTimerInitTimer(prAdapter,
                      &prAisFsmInfo->rBGScanTimer,
                      (PFN_MGMT_TIMEOUT_FUNC)aisFsmRunEventBGSleepTimeOut,
                      (UINT_32)NULL);

    cnmTimerInitTimer(prAdapter,
                      &prAisFsmInfo->rIbssAloneTimer,
                      (PFN_MGMT_TIMEOUT_FUNC)aisFsmRunEventIbssAloneTimeOut,
                      (UINT_32)NULL);

    cnmTimerInitTimer(prAdapter,
                      &prAisFsmInfo->rIndicationOfDisconnectTimer,
                      (PFN_MGMT_TIMEOUT_FUNC)aisPostponedEventOfDisconnTimeout,
                      (UINT_32)NULL);

    cnmTimerInitTimer(prAdapter,
                      &prAisFsmInfo->rJoinTimeoutTimer,
                      (PFN_MGMT_TIMEOUT_FUNC)aisFsmRunEventJoinTimeout,
                      (UINT_32)NULL);

    //4 <1.2> Initiate PWR STATE
    SET_NET_PWR_STATE_IDLE(prAdapter, NETWORK_TYPE_AIS_INDEX);


    //4 <2> Initiate BSS_INFO_T - common part
    BSS_INFO_INIT(prAdapter, NETWORK_TYPE_AIS_INDEX);
    COPY_MAC_ADDR(prAisBssInfo->aucOwnMacAddr, prAdapter->rWifiVar.aucMacAddress);

    //4 <3> Initiate BSS_INFO_T - private part
    /* TODO */
    prAisBssInfo->eBand = BAND_2G4;
    prAisBssInfo->ucPrimaryChannel = 1;
    prAisBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;

    //4 <4> Allocate MSDU_INFO_T for Beacon
    prAisBssInfo->prBeacon = cnmMgtPktAlloc(prAdapter,
                                            OFFSET_OF(WLAN_BEACON_FRAME_T, aucInfoElem[0]) + MAX_IE_LENGTH);

    if (prAisBssInfo->prBeacon) {
        prAisBssInfo->prBeacon->eSrc = TX_PACKET_MGMT;
    } else {
        ASSERT(0);
    }

#if 0
    prAisBssInfo->rPmProfSetupInfo.ucBmpDeliveryAC = PM_UAPSD_ALL;
    prAisBssInfo->rPmProfSetupInfo.ucBmpTriggerAC = PM_UAPSD_ALL;
    prAisBssInfo->rPmProfSetupInfo.ucUapsdSp = WMM_MAX_SP_LENGTH_2;
#else
    if (prAdapter->u4UapsdAcBmp == 0) {
        prAdapter->u4UapsdAcBmp = CFG_INIT_UAPSD_AC_BMP;
        //ASSERT(prAdapter->u4UapsdAcBmp);
    }
    prAisBssInfo->rPmProfSetupInfo.ucBmpDeliveryAC = (UINT_8)prAdapter->u4UapsdAcBmp;
    prAisBssInfo->rPmProfSetupInfo.ucBmpTriggerAC = (UINT_8) prAdapter->u4UapsdAcBmp;
    prAisBssInfo->rPmProfSetupInfo.ucUapsdSp = (UINT_8)prAdapter->u4MaxSpLen;
#endif

    /* request list initialization */
    LINK_INITIALIZE(&prAisFsmInfo->rPendingReqList);

    //DBGPRINTF("[2] ucBmpDeliveryAC:0x%x, ucBmpTriggerAC:0x%x, ucUapsdSp:0x%x",
    //prAisBssInfo->rPmProfSetupInfo.ucBmpDeliveryAC,
    //prAisBssInfo->rPmProfSetupInfo.ucBmpTriggerAC,
    //prAisBssInfo->rPmProfSetupInfo.ucUapsdSp);

    return;
} /* end of aisFsmInit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief the function is used to uninitialize the value in AIS_FSM_INFO_T for
*        AIS FSM operation
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmUninit(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    //P_AIS_SPECIFIC_BSS_INFO_T prAisSpecificBssInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DEINIT, "aisFsmUninit()");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    //prAisSpecificBssInfo = &(prAdapter->rWifiVar.rAisSpecificBssInfo);

    //4 <1> Stop all timers
    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rBGScanTimer);
    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rIbssAloneTimer);
    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rIndicationOfDisconnectTimer);
    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rJoinTimeoutTimer);

    //4 <2> flush pending request
    aisFsmFlushRequest(prAdapter);

    //4 <3> Reset driver-domain BSS-INFO
    if (prAisBssInfo->prBeacon) {
        cnmMgtPktFree(prAdapter, prAisBssInfo->prBeacon);
        prAisBssInfo->prBeacon = NULL;
    }

    return;
} /* end of aisFsmUninit() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Initialization of JOIN STATE
*
* @param[in] prBssDesc  The pointer of BSS_DESC_T which is the BSS we will try to join with.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateInit_JOIN(
    IN P_ADAPTER_T prAdapter,
    P_BSS_DESC_T prBssDesc
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    P_AIS_SPECIFIC_BSS_INFO_T prAisSpecificBssInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_STA_RECORD_T prStaRec;
    P_MSG_JOIN_REQ_T prJoinReqMsg;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "aisFsmStateInit_JOIN()");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prAisSpecificBssInfo = &(prAdapter->rWifiVar.rAisSpecificBssInfo);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    ASSERT(prBssDesc);

    //4 <1> We are going to connect to this BSS.
    prBssDesc->fgIsConnecting = TRUE;


    //4 <2> Setup corresponding STA_RECORD_T
    prStaRec = bssCreateStaRecFromBssDesc(prAdapter,
                                          STA_TYPE_LEGACY_AP,
                                          NETWORK_TYPE_AIS_INDEX,
                                          prBssDesc);

    prAisFsmInfo->prTargetStaRec = prStaRec;

    //4 <2.1> sync. to firmware domain
    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

    //4 <3> Update ucAvailableAuthTypes which we can choice during SAA
    if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_DISCONNECTED) {

        prStaRec->fgIsReAssoc = FALSE;

        switch (prConnSettings->eAuthMode) {
            case AUTH_MODE_OPEN:                /* Note: Omit break here. */
            case AUTH_MODE_WPA:
            case AUTH_MODE_WPA_PSK:
            case AUTH_MODE_WPA2:
            case AUTH_MODE_WPA2_PSK:
                prAisFsmInfo->ucAvailableAuthTypes = (UINT_8)AUTH_TYPE_OPEN_SYSTEM;
                break;


            case AUTH_MODE_SHARED:
                prAisFsmInfo->ucAvailableAuthTypes = (UINT_8)AUTH_TYPE_SHARED_KEY;
                break;


            case AUTH_MODE_AUTO_SWITCH:
                prAisFsmInfo->ucAvailableAuthTypes = (UINT_8)(AUTH_TYPE_OPEN_SYSTEM |
                                                     AUTH_TYPE_SHARED_KEY);
                break;

            default:
                ASSERT(!(prConnSettings->eAuthMode == AUTH_MODE_WPA_NONE));
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "JOIN INIT: Auth Algorithm : %d was not supported by JOIN\n",
                                 prConnSettings->eAuthMode);
                /* TODO(Kevin): error handling ? */
                return;
        }

        /* TODO(tyhsu): Assume that Roaming Auth Type is equal to ConnSettings eAuthMode */
        prAisSpecificBssInfo->ucRoamingAuthTypes = prAisFsmInfo->ucAvailableAuthTypes;

        prStaRec->ucTxAuthAssocRetryLimit = TX_AUTH_ASSOCI_RETRY_LIMIT;

    } else {
        ASSERT(prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE);
        ASSERT(!prBssDesc->fgIsConnected);

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "JOIN INIT: AUTH TYPE = %d for Roaming\n",
                         prAisSpecificBssInfo->ucRoamingAuthTypes);


        prStaRec->fgIsReAssoc = TRUE; /* We do roaming while the medium is connected */

        /* TODO(Kevin): We may call a sub function to acquire the Roaming Auth Type */
        prAisFsmInfo->ucAvailableAuthTypes = prAisSpecificBssInfo->ucRoamingAuthTypes;

        prStaRec->ucTxAuthAssocRetryLimit = TX_AUTH_ASSOCI_RETRY_LIMIT_FOR_ROAMING;
    }


    //4 <4> Use an appropriate Authentication Algorithm Number among the ucAvailableAuthTypes
    if (prAisFsmInfo->ucAvailableAuthTypes &
            (UINT_8)AUTH_TYPE_OPEN_SYSTEM) {

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "JOIN INIT: Try to do Authentication with AuthType == OPEN_SYSTEM.\n");
        prAisFsmInfo->ucAvailableAuthTypes &=
            ~(UINT_8)AUTH_TYPE_OPEN_SYSTEM;

        prStaRec->ucAuthAlgNum = (UINT_8)AUTH_ALGORITHM_NUM_OPEN_SYSTEM;
    } else if (prAisFsmInfo->ucAvailableAuthTypes &
               (UINT_8)AUTH_TYPE_SHARED_KEY) {

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "JOIN INIT: Try to do Authentication with AuthType == SHARED_KEY.\n");

        prAisFsmInfo->ucAvailableAuthTypes &=
            ~(UINT_8)AUTH_TYPE_SHARED_KEY;

        prStaRec->ucAuthAlgNum = (UINT_8)AUTH_ALGORITHM_NUM_SHARED_KEY;
    } else if (prAisFsmInfo->ucAvailableAuthTypes &
               (UINT_8)AUTH_TYPE_FAST_BSS_TRANSITION) {

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "JOIN INIT: Try to do Authentication with AuthType == FAST_BSS_TRANSITION.\n");

        prAisFsmInfo->ucAvailableAuthTypes &=
            ~(UINT_8)AUTH_TYPE_FAST_BSS_TRANSITION;

        prStaRec->ucAuthAlgNum = (UINT_8)AUTH_ALGORITHM_NUM_FAST_BSS_TRANSITION;
    } else {
        ASSERT(0);
    }

    //4 <5> Overwrite Connection Setting for eConnectionPolicy == ANY (Used by Assoc Req)
    if (prConnSettings->eConnectionPolicy == CONNECT_BY_SSID_ANY) {

        if (prBssDesc->ucSSIDLen) {
            COPY_SSID(prConnSettings->aucSSID,
                      prConnSettings->ucSSIDLen,
                      prBssDesc->aucSSID,
                      prBssDesc->ucSSIDLen);
        }
    }

    //4 <6> Send a Msg to trigger SAA to start JOIN process.
    prJoinReqMsg = (P_MSG_JOIN_REQ_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_JOIN_REQ_T));
    if (!prJoinReqMsg) {

        ASSERT(0); // Can't trigger SAA FSM
        return;
    }

    prJoinReqMsg->rMsgHdr.eMsgId = MID_AIS_SAA_FSM_START;
    prJoinReqMsg->ucSeqNum = ++prAisFsmInfo->ucSeqNumOfReqMsg;
    prJoinReqMsg->prStaRec = prStaRec;

    if (1) {
        int j;
        P_FRAG_INFO_T prFragInfo;
        for (j = 0; j < MAX_NUM_CONCURRENT_FRAGMENTED_MSDUS; j++) {
            prFragInfo = &prStaRec->rFragInfo[j];

            if (prFragInfo->pr1stFrag) {
                //nicRxReturnRFB(prAdapter, prFragInfo->pr1stFrag);
                prFragInfo->pr1stFrag = (P_SW_RFB_T)NULL;
            }
        }
    }

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prJoinReqMsg,
                MSG_SEND_METHOD_BUF);

    return;
} /* end of aisFsmInit_JOIN() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Retry JOIN for AUTH_MODE_AUTO_SWITCH
*
* @param[in] prStaRec       Pointer to the STA_RECORD_T
*
* @retval TRUE      We will retry JOIN
* @retval FALSE     We will not retry JOIN
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
aisFsmStateInit_RetryJOIN(
    IN P_ADAPTER_T prAdapter,
    P_STA_RECORD_T prStaRec
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_MSG_JOIN_REQ_T prJoinReqMsg;


    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    /* Retry other AuthType if possible */
    if (!prAisFsmInfo->ucAvailableAuthTypes) {
        return FALSE;
    }

    if (prAisFsmInfo->ucAvailableAuthTypes &
            (UINT_8)AUTH_TYPE_SHARED_KEY) {

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "RETRY JOIN INIT: Retry Authentication with AuthType == SHARED_KEY.\n");

        prAisFsmInfo->ucAvailableAuthTypes &=
            ~(UINT_8)AUTH_TYPE_SHARED_KEY;

        prStaRec->ucAuthAlgNum = (UINT_8)AUTH_ALGORITHM_NUM_SHARED_KEY;
    } else {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "RETRY JOIN INIT: Retry Authentication with Unexpected AuthType.\n");
        ASSERT(0);
    }

    prAisFsmInfo->ucAvailableAuthTypes = 0; /* No more available Auth Types */

    /* Trigger SAA to start JOIN process. */
    prJoinReqMsg = (P_MSG_JOIN_REQ_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_JOIN_REQ_T));
    if (!prJoinReqMsg) {

        ASSERT(0); // Can't trigger SAA FSM
        return FALSE;
    }

    prJoinReqMsg->rMsgHdr.eMsgId = MID_AIS_SAA_FSM_START;
    prJoinReqMsg->ucSeqNum = ++prAisFsmInfo->ucSeqNumOfReqMsg;
    prJoinReqMsg->prStaRec = prStaRec;

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prJoinReqMsg,
                MSG_SEND_METHOD_BUF);

    return TRUE;

}/* end of aisFsmRetryJOIN() */


#if CFG_SUPPORT_ADHOC
/*----------------------------------------------------------------------------*/
/*!
* @brief State Initialization of AIS_STATE_IBSS_ALONE
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateInit_IBSS_ALONE(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    //P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_INFO_T prAisBssInfo;

    DEBUGFUNC("aisFsmStateInit_IBSS_ALONE");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    //prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    //4 <1> Check if IBSS was created before ?
    if (prAisBssInfo->fgIsBeaconActivated) {

        //4 <2> Start IBSS Alone Timer for periodic SCAN and then SEARCH
#if !CFG_SLT_SUPPORT
        cnmTimerStartTimer(prAdapter,
                           &prAisFsmInfo->rIbssAloneTimer,
                           SEC_TO_MSEC(AIS_IBSS_ALONE_TIMEOUT_SEC));
#endif
    }

    aisFsmCreateIBSS(prAdapter);

    return;
} /* end of aisFsmStateInit_IBSS_ALONE() */


/*----------------------------------------------------------------------------*/
/*!
* @brief State Initialization of AIS_STATE_IBSS_MERGE
*
* @param[in] prBssDesc  The pointer of BSS_DESC_T which is the IBSS we will try to merge with.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateInit_IBSS_MERGE(
    IN P_ADAPTER_T prAdapter,
    P_BSS_DESC_T prBssDesc
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    //P_CONNECTION_SETTINGS_T prConnSettings;
    //P_BSS_INFO_T prAisBssInfo;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;

    DEBUGFUNC("aisFsmStateInit_IBSS_MERGE");

    ASSERT(prBssDesc);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    //prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    //prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    //4 <1> We will merge with to this BSS immediately.
    prBssDesc->fgIsConnecting = FALSE;
    prBssDesc->fgIsConnected = TRUE;

    //4 <2> Setup corresponding STA_RECORD_T
    prStaRec = bssCreateStaRecFromBssDesc(prAdapter,
                                          STA_TYPE_ADHOC_PEER,
                                          NETWORK_TYPE_AIS_INDEX,
                                          prBssDesc);

    prStaRec->fgIsMerging = TRUE;

    prAisFsmInfo->prTargetStaRec = prStaRec;

    //4 <2.1> sync. to firmware domain
    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

    //4 <3> IBSS-Merge
    aisFsmMergeIBSS(prAdapter, prStaRec);

    return;
} /* end of aisFsmStateInit_IBSS_MERGE() */

#endif /* CFG_SUPPORT_ADHOC */


/*----------------------------------------------------------------------------*/
/*!
* @brief Process of JOIN Abort
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateAbort_JOIN(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_MSG_JOIN_ABORT_T prJoinAbortMsg;
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "aisFsmStateAbort_JOIN()");
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    /* 1. Abort JOIN process */
    prJoinAbortMsg = (P_MSG_JOIN_ABORT_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_JOIN_ABORT_T));
    if (!prJoinAbortMsg) {

        ASSERT(0); // Can't abort SAA FSM
        return;
    }

    prJoinAbortMsg->rMsgHdr.eMsgId = MID_AIS_SAA_FSM_ABORT;
    prJoinAbortMsg->ucSeqNum = prAisFsmInfo->ucSeqNumOfReqMsg;
    prJoinAbortMsg->prStaRec = prAisFsmInfo->prTargetStaRec;

    scanRemoveConnFlagOfBssDescByBssid(prAdapter, prAisFsmInfo->prTargetStaRec->aucMacAddr);

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prJoinAbortMsg,
                MSG_SEND_METHOD_BUF);

    /* 2. Return channel privilege */
    aisFsmReleaseCh(prAdapter);

    /* 3.1 stop join timeout timer */
    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rJoinTimeoutTimer);

    /* 3.2 reset local variable */
    prAisFsmInfo->fgIsInfraChannelFinished = TRUE;

    return;
} /* end of aisFsmAbortJOIN() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Process of SCAN Abort
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateAbort_SCAN(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_MSG_SCN_SCAN_CANCEL prScanCancelMsg;

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    /* Abort JOIN process. */
    prScanCancelMsg = (P_MSG_SCN_SCAN_CANCEL)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SCN_SCAN_CANCEL));
    if (!prScanCancelMsg) {

        ASSERT(0); // Can't abort SCN FSM
        return;
    }

    prScanCancelMsg->rMsgHdr.eMsgId = MID_AIS_SCN_SCAN_CANCEL;
    prScanCancelMsg->ucSeqNum = prAisFsmInfo->ucSeqNumOfScanReq;
    prScanCancelMsg->ucNetTypeIndex = (UINT_8)NETWORK_TYPE_AIS_INDEX;
#if CFG_ENABLE_WIFI_DIRECT
    if (prAdapter->fgIsP2PRegistered) {
        prScanCancelMsg->fgIsChannelExt = FALSE;
    }
#endif

    /* unbuffered message to guarantee scan is cancelled in sequence */
    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prScanCancelMsg,
                MSG_SEND_METHOD_UNBUF);

    return;
} /* end of aisFsmAbortSCAN() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Process of NORMAL_TR Abort
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateAbort_NORMAL_TR(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "aisFsmStateAbort_NORMAL_TR()");
    ASSERT(prAdapter);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    /* TODO(Kevin): Do abort other MGMT func */

    /* 1. Release channel to CNM */
    aisFsmReleaseCh(prAdapter);

    /* 2.1 stop join timeout timer */
    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rJoinTimeoutTimer);

    /* 2.2 reset local variable */
    prAisFsmInfo->fgIsInfraChannelFinished = TRUE;

    return;
} /* end of aisFsmAbortNORMAL_TR() */


#if CFG_SUPPORT_ADHOC
/*----------------------------------------------------------------------------*/
/*!
* @brief Process of NORMAL_TR Abort
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateAbort_IBSS(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_DESC_T prBssDesc;

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    // reset BSS-DESC
    if (prAisFsmInfo->prTargetStaRec) {
        prBssDesc = scanSearchBssDescByTA(prAdapter,
                                          prAisFsmInfo->prTargetStaRec->aucMacAddr);

        if (prBssDesc) {
            prBssDesc->fgIsConnected = FALSE;
            prBssDesc->fgIsConnecting = FALSE;
        }
    }

    // release channel privilege
    aisFsmReleaseCh(prAdapter);

    return;
}
#endif /* CFG_SUPPORT_ADHOC */


/*----------------------------------------------------------------------------*/
/*!
* @brief The Core FSM engine of AIS(Ad-hoc, Infra STA)
*
* @param[in] eNextState Enum value of next AIS STATE
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmSteps(
    IN P_ADAPTER_T prAdapter,
    ENUM_AIS_STATE_T eNextState
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_DESC_T prBssDesc;
    P_MSG_CH_REQ_T prMsgChReq;
    P_MSG_SCN_SCAN_REQ prScanReqMsg;
    P_AIS_REQ_HDR_T prAisReq;
    ENUM_BAND_T eBand;
    UINT_8 ucChannel;
    UINT_16 u2ScanIELen;

    BOOLEAN fgIsTransition = (BOOLEAN)FALSE;

#ifdef __WPS_SUPPORT__
    PUINT_8 pucFrameBody;
    PUINT_8 pWpsAttr = NULL;
    UINT_16 BlankByte = 0;
#endif

    //DEBUGFUNC("aisFsmSteps()");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    do {

        /* Do entering Next State */
        prAisFsmInfo->ePreviousState = prAisFsmInfo->eCurrentState;

#if 1
        DBGLOG(AIS, STATE, ("TRANSITION: [%s] -> [%s]\n",
                            apucDebugAisState[prAisFsmInfo->eCurrentState],
                            apucDebugAisState[eNextState]));
#endif
        MT5931_TRACE(TRACE_STATE, MT5931_INFO_14, "AIS_STATE_TRANSITION: %d -> %d",
                     prAisFsmInfo->eCurrentState, eNextState);

        /* NOTE(Kevin): This is the only place to change the eCurrentState(except initial) */
        prAisFsmInfo->eCurrentState = eNextState;

        fgIsTransition = (BOOLEAN)FALSE;

        /* Do tasks of the State that we just entered */
        switch (prAisFsmInfo->eCurrentState) {
            /* NOTE(Kevin): we don't have to rearrange the sequence of following
             * switch case. Instead I would like to use a common lookup table of array
             * of function pointer to speed up state search.
             */
            case AIS_STATE_IDLE:

#ifdef __WIFI_SNIFFER_SUPPORT__
                if (prAdapter->fgDelayIndDisc) {
                    prAdapter->fgDelayIndDisc = 0;

                    if (prAisBssInfo->prStaRecOfAP) {
                        prAisBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;
                    }

#if (CFG_SUPPORT_ADHOC) || (CFG_SUPPORT_AAA)
                    bssClearClientList(prAdapter, prAisBssInfo);
#endif
                    aisIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED, FALSE);

                    kal_wap_trace(MOD_WNDRV, TRACE_INFO, "AIS is idle,do aisIndicationOfMediaStateToHost");
                }
#endif

                prAisReq = aisFsmGetNextRequest(prAdapter);

                if (prAisReq == NULL || prAisReq->eReqType == AIS_REQUEST_RECONNECT) {
                    if (prConnSettings->fgIsConnReqIssued) {

                        prAisFsmInfo->fgTryScan = TRUE;

                        SET_NET_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX);
                        SET_NET_PWR_STATE_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX);

                        // sync with firmware
                        nicActivateNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);

                        // reset trial count
                        prAisFsmInfo->ucConnTrialCount = 0;

                        eNextState = AIS_STATE_SEARCH;
                        fgIsTransition = TRUE;
                    } else {
                        UNSET_NET_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX);
                        SET_NET_PWR_STATE_IDLE(prAdapter, NETWORK_TYPE_AIS_INDEX);

                        // sync with firmware
                        nicDeactivateNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);

                        // check for other pending request
                        if (prAisReq) {
                            if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_SCAN, TRUE) == TRUE) {
                                /* NOTE(Nelson): Avoid to no scan result to SUPC while fast switch to join/disjoin on MMI,
                                                 do not clear scan result here. */
                                //wlanClearScanningResult(prAdapter);
                                eNextState = AIS_STATE_SCAN;

                                fgIsTransition = TRUE;
                            }
                        }
                    }

                    if (prAisReq) {
                        /* free the message */
                        cnmMemFree(prAdapter, prAisReq);
                    }
                } else if (prAisReq->eReqType == AIS_REQUEST_SCAN) {
#if CFG_SUPPORT_ROAMING
                    prAisFsmInfo->fgIsRoamingScanPending = FALSE;
#endif /* CFG_SUPPORT_ROAMING */
                    wlanClearScanningResult(prAdapter);

                    eNextState = AIS_STATE_SCAN;
                    fgIsTransition = TRUE;

                    /* free the message */
                    cnmMemFree(prAdapter, prAisReq);
                } else if (prAisReq->eReqType == AIS_REQUEST_ROAMING_CONNECT || prAisReq->eReqType == AIS_REQUEST_ROAMING_SEARCH) {
                    /* ignore */
                    /* free the message */
                    cnmMemFree(prAdapter, prAisReq);
                }

                prAisFsmInfo->u4SleepInterval = AIS_BG_SCAN_INTERVAL_MIN_SEC;

                break;

            case AIS_STATE_SEARCH:
                //4 <1> Search for a matched candidate and save it to prTargetBssDesc.
#if CFG_SLT_SUPPORT
                prBssDesc = prAdapter->rWifiVar.rSltInfo.prPseudoBssDesc;
#else
                prBssDesc = scanSearchBssDescByPolicy(prAdapter, NETWORK_TYPE_AIS_INDEX);
#endif

                // we are under Roaming Condition.
                if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {
                    if (prAisFsmInfo->ucConnTrialCount > AIS_ROAMING_CONNECTION_TRIAL_LIMIT) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "Reach romaing LIMIT(%d)", AIS_ROAMING_CONNECTION_TRIAL_LIMIT);
#if CFG_SUPPORT_ROAMING
                        roamingFsmRunEventFail(prAdapter, ROAMING_FAIL_REASON_CONNLIMIT);
#endif /* CFG_SUPPORT_ROAMING */
                        // reset retry count
                        prAisFsmInfo->ucConnTrialCount = 0;

                        // abort connection trial
                        prConnSettings->fgIsConnReqIssued = FALSE;

                        eNextState = AIS_STATE_NORMAL_TR;
                        fgIsTransition = TRUE;

                        break;
                    }
                }

                //4 <2> We are not under Roaming Condition.
                if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_DISCONNECTED) {

                    //4 <2.a> If we have the matched one
                    if (prBssDesc) {

                        //4 <A> Stored the Selected BSS security cipher. For later asoc req compose IE
                        prAisBssInfo->u4RsnSelectedGroupCipher =
                            prBssDesc->u4RsnSelectedGroupCipher;
                        prAisBssInfo->u4RsnSelectedPairwiseCipher =
                            prBssDesc->u4RsnSelectedPairwiseCipher;
                        prAisBssInfo->u4RsnSelectedAKMSuite =
                            prBssDesc->u4RsnSelectedAKMSuite;

                        //4 <B> Do STATE transition and update current Operation Mode.
                        if (prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE) {

                            prAisBssInfo->eCurrentOPMode = OP_MODE_INFRASTRUCTURE;

                            /* Record the target BSS_DESC_T for next STATE. */
                            prAisFsmInfo->prTargetBssDesc = prBssDesc;

                            /* Transit to channel acquire */
                            eNextState = AIS_STATE_REQ_CHANNEL_JOIN;
                            fgIsTransition = TRUE;

                            // increase connection trial count
                            prAisFsmInfo->ucConnTrialCount++;
                        }
#if CFG_SUPPORT_ADHOC
                        else if (prBssDesc->eBSSType == BSS_TYPE_IBSS) {

                            prAisBssInfo->eCurrentOPMode = OP_MODE_IBSS;

                            /* Record the target BSS_DESC_T for next STATE. */
                            prAisFsmInfo->prTargetBssDesc = prBssDesc;

                            eNextState = AIS_STATE_IBSS_MERGE;
                            fgIsTransition = TRUE;
                        }
#endif /* CFG_SUPPORT_ADHOC */
                        else {
                            ASSERT(0);
                            eNextState = AIS_STATE_WAIT_FOR_NEXT_SCAN;
                            fgIsTransition = TRUE;
                        }
                    }
                    //4 <2.b> If we don't have the matched one
                    else {

                        // increase connection trial count for infrastructure connection
                        if (prConnSettings->eOPMode == NET_TYPE_INFRA) {
                            prAisFsmInfo->ucConnTrialCount++;
                        }

                        //4 <A> Try to SCAN
                        if (prAisFsmInfo->fgTryScan) {
                            eNextState = AIS_STATE_LOOKING_FOR;

                            fgIsTransition = TRUE;
                        }
                        //4 <B> We've do SCAN already, now wait in some STATE.
                        else {
                            if (prConnSettings->eOPMode == NET_TYPE_INFRA) {

                                /* issue reconnect request, and retreat to idle state for scheduling */
                                aisFsmInsertRequest(prAdapter, AIS_REQUEST_RECONNECT);

                                eNextState = AIS_STATE_IDLE;
                                fgIsTransition = TRUE;
                            }
#if CFG_SUPPORT_ADHOC
                            else if ((prConnSettings->eOPMode == NET_TYPE_IBSS)
                                     || (prConnSettings->eOPMode == NET_TYPE_AUTO_SWITCH)
                                     || (prConnSettings->eOPMode == NET_TYPE_DEDICATED_IBSS)) {

                                prAisBssInfo->eCurrentOPMode = OP_MODE_IBSS;
                                prAisFsmInfo->prTargetBssDesc = NULL;

                                eNextState = AIS_STATE_IBSS_ALONE;
                                fgIsTransition = TRUE;
                            }
#endif /* CFG_SUPPORT_ADHOC */
                            else {
                                ASSERT(0);
                                eNextState = AIS_STATE_WAIT_FOR_NEXT_SCAN;
                                fgIsTransition = TRUE;
                            }
                        }
                    }
                }
                //4 <3> We are under Roaming Condition.
                else { // prAdapter->eConnectionState == MEDIA_STATE_CONNECTED.

                    //4 <3.a> This BSS_DESC_T is our AP.
                    /* NOTE(Kevin 2008/05/16): Following cases will go back to NORMAL_TR.
                     * CASE I: During Roaming, APP(WZC/NDISTEST) change the connection
                     *         settings. That make we can NOT match the original AP, so the
                     *         prBssDesc is NULL.
                     * CASE II: The same reason as CASE I. Because APP change the
                     *          eOPMode to other network type in connection setting
                     *          (e.g. NET_TYPE_IBSS), so the BssDesc become the IBSS node.
                     * (For CASE I/II, before WZC/NDISTEST set the OID_SSID, it will change
                     * other parameters in connection setting first. So if we do roaming
                     * at the same time, it will hit these cases.)
                     *
                     * CASE III: Normal case, we can't find other candidate to roam
                     * out, so only the current AP will be matched.
                     */
                    if ((!prBssDesc) || /* CASE I */
                            (prBssDesc->eBSSType != BSS_TYPE_INFRASTRUCTURE) || /* CASE II */
                            (prBssDesc->fgIsConnected) /* CASE III */) {
#if DBG
                        if ((prBssDesc) &&
                                (prBssDesc->fgIsConnected)) {
                            ASSERT(EQUAL_MAC_ADDR(prBssDesc->aucBSSID, prAisBssInfo->aucBSSID));
                        }
#endif /* DBG */
                        /* We already associated with it, go back to NORMAL_TR */
                        /* TODO(Kevin): Roaming Fail */
#if CFG_SUPPORT_ROAMING
                        roamingFsmRunEventFail(prAdapter, ROAMING_FAIL_REASON_NOCANDIDATE);
#endif /* CFG_SUPPORT_ROAMING */

                        /* Retreat to NORMAL_TR state */
                        eNextState = AIS_STATE_NORMAL_TR;
                        fgIsTransition = TRUE;
                    }
                    //4 <3.b> Try to roam out for JOIN this BSS_DESC_T.
                    else {
#if 0//DBG
                        ASSERT(UNEQUAL_MAC_ADDR(prBssDesc->aucBSSID, prAisBssInfo->aucBSSID));
#endif /* DBG */
                        if (EQUAL_MAC_ADDR(prBssDesc->aucBSSID, prAisBssInfo->aucBSSID)) {
                            MT5931_TRACE(TRACE_ROAMING, MT5931_INFO_15, "prBssDesc->aucBSSID:["MACSTR"] prAisBssInfo->aucBSSID:["MACSTR"]",
                                         prBssDesc->aucBSSID, prAisBssInfo->aucBSSID);
                        }
                        //4 <A> Record the target BSS_DESC_T for next STATE.
                        prAisFsmInfo->prTargetBssDesc = prBssDesc;

                        // tyhsu: increase connection trial count
                        prAisFsmInfo->ucConnTrialCount++;

                        /* Transit to channel acquire */
                        eNextState = AIS_STATE_REQ_CHANNEL_JOIN;
                        fgIsTransition = TRUE;
                    }
                }

                break;

            case AIS_STATE_WAIT_FOR_NEXT_SCAN:

                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_SCAN, "SCAN: Idle Begin - Current Time = %ld\n", kalGetTimeTick());

                cnmTimerStartTimer(prAdapter,
                                   &prAisFsmInfo->rBGScanTimer,
                                   SEC_TO_MSEC(prAisFsmInfo->u4SleepInterval));

                SET_NET_PWR_STATE_IDLE(prAdapter, NETWORK_TYPE_AIS_INDEX);

                if (prAisFsmInfo->u4SleepInterval < AIS_BG_SCAN_INTERVAL_MAX_SEC) {
                    prAisFsmInfo->u4SleepInterval <<= 1;
                }
                break;

            case AIS_STATE_SCAN:
            case AIS_STATE_ONLINE_SCAN:
            case AIS_STATE_LOOKING_FOR:

                if (!IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX)) {
                    SET_NET_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX);

                    // sync with firmware
                    nicActivateNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);
                }

                /* IE length decision */
#ifdef __WPS_SUPPORT__
                if (prAdapter->WpsProbe) {
                    pucFrameBody = (PUINT_8) & (prAdapter->prGlueInfo->aucWSCIE);

                    WPS_PROBE_IE(pucFrameBody)->ucId = ELEM_ID_VENDOR;
                    /*Cacluate ucLen after build attrites done*/
                    WPA_PUT_BE32((PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->Oui), WPS_DEV_OUI_WFA);
                    /*compose version attr*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->Ver);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_VER_ST(pWpsAttr)->Hdr.Id), ATTR_VERSION);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_VER_ST(pWpsAttr)->Hdr.Len), WPS_VER_LEN);
                    WPS_ATTR_VER_ST(pWpsAttr)->Ver = prAdapter->WpsVer;
                    //((WPS_ATTR_VER_T*)pWpsAttr)->Ver = prAdapter->WpsVer;
                    /*Reqtype*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->ReqType);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_REQTYPE_ST(pWpsAttr)->Hdr.Id), ATTR_REQUEST_TYPE);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_REQTYPE_ST(pWpsAttr)->Hdr.Len), WPS_REQ_TYPE_LEN);
                    WPS_ATTR_REQTYPE_ST(pWpsAttr)->ReqType = prAdapter->ReqType;

                    /*cfg mehtod*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->CfgMethod);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_CFG_METHOD_ST(pWpsAttr)->Hdr.Id), ATTR_CONFIG_METHODS);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_CFG_METHOD_ST(pWpsAttr)->Hdr.Len), WPS_CFG_METHOD_LEN);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_CFG_METHOD_ST(pWpsAttr)->CfgMethod), prAdapter->CfgMethod);

                    /*uuid*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->Uuid);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_UUID_ST(pWpsAttr)->Hdr.Id), ATTR_UUID_E);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_UUID_ST(pWpsAttr)->Hdr.Len), WPS_UUID_LEN);
                    kal_mem_cpy((PUINT_8) & (WPS_ATTR_UUID_ST(pWpsAttr)->Uuid), prAdapter->Uuid, WPS_UUID_LEN);

                    /*primary dev type*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->PrimDev);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_PRIDEV_ST(pWpsAttr)->Hdr.Id), ATTR_PRIMARY_DEV_TYPE);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_PRIDEV_ST(pWpsAttr)->Hdr.Len), WPS_PRIDEV_TYPE_LEN);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_PRIDEV_ST(pWpsAttr)->CatId), prAdapter->Cate);
                    WPA_PUT_BE32((PUINT_8) & (WPS_ATTR_PRIDEV_ST(pWpsAttr)->Oui), prAdapter->Oui);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_PRIDEV_ST(pWpsAttr)->SubCat), prAdapter->SubCate);

                    /*rf bands*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->RfBands);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_RF_ST(pWpsAttr)->Hdr.Id), ATTR_RF_BANDS);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_RF_ST(pWpsAttr)->Hdr.Len), WPS_RFBANDS_LEN);
                    WPS_ATTR_RF_ST(pWpsAttr)->RF = prAdapter->RfBands;

                    /*assoc state*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->AssocStat);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_ASSOC_STAT_ST(pWpsAttr)->Hdr.Id), ATTR_ASSOC_STATE);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_ASSOC_STAT_ST(pWpsAttr)->Hdr.Len), WPS_ASSOC_STAT_LEN);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_ASSOC_STAT_ST(pWpsAttr)->State), prAdapter->AssocStat);

                    /*cfg error*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->CfgError);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_CFG_ERR_ST(pWpsAttr)->Hdr.Id), ATTR_CONFIG_ERROR);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_CFG_ERR_ST(pWpsAttr)->Hdr.Len), WPS_CFG_ERR_LEN);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_CFG_ERR_ST(pWpsAttr)->Error), prAdapter->CfgErr);

                    /*dev password id*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->DevPswdId);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_DEV_PSWD_ID_ST(pWpsAttr)->Hdr.Id), ATTR_DEV_PASSWORD_ID);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_DEV_PSWD_ID_ST(pWpsAttr)->Hdr.Len), WPS_DEV_PSWD_ID_LEN);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_DEV_PSWD_ID_ST(pWpsAttr)->PwsdId), prAdapter->DevPswdId);

                    /*Manufacturer*/
                    pWpsAttr = (PUINT_8) & (WPS_PROBE_IE(pucFrameBody)->Manuf);
                    kal_mem_cpy((UCHAR *)WPS_ATTR_MANUF_ST(pWpsAttr)->Name, prAdapter->Manufact, prAdapter->ManufLen);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_MANUF_ST(pWpsAttr)->Hdr.Id), ATTR_MANUFACTURER);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_MANUF_ST(pWpsAttr)->Hdr.Len), prAdapter->ManufLen);
                    BlankByte += WPS_MANUFACTURE_LEN - prAdapter->ManufLen;

                    /*Model name*/
                    //pWpsAttr = (PUINT_8)&(WPS_PROBE_IE(pucFrameBody)->ModelName);
                    pWpsAttr = pWpsAttr + WPS_ATTR_HDR_LEN + prAdapter->ManufLen;
                    kal_mem_cpy((UCHAR *)WPS_ATTR_MODEL_NAME_ST(pWpsAttr)->Name, prAdapter->ModelName, prAdapter->ModelNameLen);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_MODEL_NAME_ST(pWpsAttr)->Hdr.Id), ATTR_MODEL_NAME);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_MODEL_NAME_ST(pWpsAttr)->Hdr.Len), prAdapter->ModelNameLen);
                    BlankByte += WPS_MODEL_NAME_LEN - prAdapter->ModelNameLen;

                    /*Model num*/
                    //pWpsAttr = (PUINT_8)&(WPS_PROBE_IE(pucFrameBody)->ModelNum);
                    pWpsAttr = pWpsAttr + WPS_ATTR_HDR_LEN + prAdapter->ModelNameLen;
                    kal_mem_cpy((UCHAR *)WPS_ATTR_MODEL_NUM_ST(pWpsAttr)->Num, prAdapter->ModelNum, prAdapter->ModelNumLen);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_MODEL_NUM_ST(pWpsAttr)->Hdr.Id), ATTR_MODEL_NUMBER);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_MODEL_NUM_ST(pWpsAttr)->Hdr.Len), prAdapter->ModelNumLen);
                    BlankByte += WPS_MODEL_NUM_LEN - prAdapter->ModelNumLen;

                    /*Dev Name*/
                    //pWpsAttr = (PUINT_8)&(WPS_PROBE_IE(pucFrameBody)->DevName);
                    pWpsAttr = pWpsAttr + WPS_ATTR_HDR_LEN + prAdapter->ModelNumLen;
                    kal_mem_cpy((UCHAR *)WPS_ATTR_DEV_NAME_ST(pWpsAttr)->Name, prAdapter->DevName, prAdapter->DevNameLen);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_DEV_NAME_ST(pWpsAttr)->Hdr.Id), ATTR_DEV_NAME);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_DEV_NAME_ST(pWpsAttr)->Hdr.Len), prAdapter->DevNameLen);
                    BlankByte += WPS_DEV_NAME_LEN - prAdapter->DevNameLen;

                    /*WPA VENDOR EXT*/
                    //pWpsAttr = (PUINT_8)&(WPS_PROBE_IE(pucFrameBody)->WfaExt);
                    pWpsAttr = pWpsAttr + WPS_ATTR_HDR_LEN + prAdapter->DevNameLen;
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_WFA_EXT_ST(pWpsAttr)->Hdr.Id), ATTR_VENDOR_EXT);
                    WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_WFA_EXT_ST(pWpsAttr)->Hdr.Len),
                                 (WPS_WFA_ATTR_VID_LEN + WPS_WFA_ATTR_SUB_HDR_LEN
                                  + WPS_WFA_ATTR_SUB_VER_LEN));
                    WPA_PUT_BE24((PUINT_8) & (WPS_ATTR_WFA_EXT_ST(pWpsAttr)->VID), ATTR_WFA_VENDOR_ID);

                    /*put WFA payload*/
                    /* TODO put ver2.0 */
                    //pWpsAttr = (PUINT_8)&(WPS_PROBE_IE(pucFrameBody)->WfaVer);
                    pWpsAttr = pWpsAttr + sizeof(WPS_ATTR_WFA_EXT);
                    WFA_SUB_ATTR_VER_ST(pWpsAttr)->Id = ATTR_WFA_SUB_VER_ID;
                    WFA_SUB_ATTR_VER_ST(pWpsAttr)->Len = WPS_WFA_ATTR_SUB_VER_LEN;
                    WFA_SUB_ATTR_VER_ST(pWpsAttr)->Ver = WPS_VERSION_V2;
                    WPS_PROBE_IE(pucFrameBody)->ucLength =
                        VENDOR_OUI_LEN + WPS_PROBE_REQ_ATTRS_LEN - BlankByte;

                    prAdapter->prGlueInfo->u2WSCIELen = ELEM_WPS_PROBE_REQ_IE_LEN - BlankByte;
                    u2ScanIELen = prAdapter->prGlueInfo->u2WSCIELen;
                } else
#endif
                {
                    u2ScanIELen = 0;
                    prAdapter->prGlueInfo->u2WSCIELen = 0;
                }

                prScanReqMsg = (P_MSG_SCN_SCAN_REQ)cnmMemAlloc(prAdapter,
                               RAM_TYPE_MSG,
                               OFFSET_OF(MSG_SCN_SCAN_REQ, aucIE) + u2ScanIELen);
                if (!prScanReqMsg) {
                    ASSERT(0); // Can't trigger SCAN FSM
                    return;
                }

                prScanReqMsg->rMsgHdr.eMsgId    = MID_AIS_SCN_SCAN_REQ;
                prScanReqMsg->ucSeqNum          = ++prAisFsmInfo->ucSeqNumOfScanReq;
                prScanReqMsg->ucNetTypeIndex    = (UINT_8)NETWORK_TYPE_AIS_INDEX;
                prScanReqMsg->eScanType         = SCAN_TYPE_ACTIVE_SCAN;

                if (prAisFsmInfo->eCurrentState == AIS_STATE_SCAN
                        || prAisFsmInfo->eCurrentState == AIS_STATE_ONLINE_SCAN) {
                    if (prAisFsmInfo->ucScanSSIDLen == 0) {
                        /* Scan for all available SSID */
                        prScanReqMsg->ucSSIDType        = SCAN_REQ_SSID_WILDCARD;
                    } else {
                        prScanReqMsg->ucSSIDType        = SCAN_REQ_SSID_SPECIFIED;
                        COPY_SSID(prScanReqMsg->aucSSID,
                                  prScanReqMsg->ucSSIDLength,
                                  prAisFsmInfo->aucScanSSID,
                                  prAisFsmInfo->ucScanSSIDLen);
                    }
                } else {
                    /* Scan for determined SSID */
                    prScanReqMsg->ucSSIDType        = SCAN_REQ_SSID_SPECIFIED;
                    COPY_SSID(prScanReqMsg->aucSSID,
                              prScanReqMsg->ucSSIDLength,
                              prConnSettings->aucSSID,
                              prConnSettings->ucSSIDLen);
                }

                /* check if tethering is running and need to fix on specific channel */
                if (cnmAisInfraChannelFixed(prAdapter, &eBand, &ucChannel) == TRUE) {
                    prScanReqMsg->eScanChannel      = SCAN_CHANNEL_SPECIFIED;
                    prScanReqMsg->ucChannelListNum  = 1;
                    prScanReqMsg->arChnlInfoList[0].eBand
                        = eBand;;
                    prScanReqMsg->arChnlInfoList[0].ucChannelNum
                        = ucChannel;
                } else if (prAdapter->aePreferBand[NETWORK_TYPE_AIS_INDEX] == BAND_NULL) {
                    if (prAdapter->fgEnable5GBand == TRUE) {
                        prScanReqMsg->eScanChannel      = SCAN_CHANNEL_FULL;
                    } else {
                        prScanReqMsg->eScanChannel      = SCAN_CHANNEL_2G4;
                    }
                } else if (prAdapter->aePreferBand[NETWORK_TYPE_AIS_INDEX] == BAND_2G4) {
                    prScanReqMsg->eScanChannel      = SCAN_CHANNEL_2G4;
                } else if (prAdapter->aePreferBand[NETWORK_TYPE_AIS_INDEX] == BAND_5G) {
                    prScanReqMsg->eScanChannel      = SCAN_CHANNEL_5G;
                } else {
                    prScanReqMsg->eScanChannel      = SCAN_CHANNEL_FULL;
                }

//#if CFG_SUPPORT_WPS2
                if (prAdapter->prGlueInfo->u2WSCIELen > 0) {
                    kalMemCopy(prScanReqMsg->aucIE, &prAdapter->prGlueInfo->aucWSCIE, prAdapter->prGlueInfo->u2WSCIELen);
                }
//#endif

                prScanReqMsg->u2IELen = u2ScanIELen;

                mboxSendMsg(prAdapter,
                            MBOX_ID_0,
                            (P_MSG_HDR_T) prScanReqMsg,
                            MSG_SEND_METHOD_BUF);

                prAisFsmInfo->fgTryScan = FALSE; /* Will enable background sleep for infrastructure */

                break;

            case AIS_STATE_REQ_CHANNEL_JOIN:
                /* send message to CNM for acquiring channel */
                prMsgChReq = (P_MSG_CH_REQ_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_CH_REQ_T));
                if (!prMsgChReq) {
                    ASSERT(0); // Can't indicate CNM for channel acquiring
                    return;
                }

                prMsgChReq->rMsgHdr.eMsgId      = MID_MNY_CNM_CH_REQ;
                prMsgChReq->ucNetTypeIndex      = NETWORK_TYPE_AIS_INDEX;
                prMsgChReq->ucTokenID           = ++prAisFsmInfo->ucSeqNumOfChReq;
                prMsgChReq->eReqType            = CH_REQ_TYPE_JOIN;
                prMsgChReq->u4MaxInterval       = AIS_JOIN_CH_REQUEST_INTERVAL;
                prMsgChReq->ucPrimaryChannel    = prAisFsmInfo->prTargetBssDesc->ucChannelNum;
                prMsgChReq->eRfSco              = prAisFsmInfo->prTargetBssDesc->eSco;
                prMsgChReq->eRfBand             = prAisFsmInfo->prTargetBssDesc->eBand;
                COPY_MAC_ADDR(prMsgChReq->aucBSSID, prAisFsmInfo->prTargetBssDesc->aucBSSID);

                mboxSendMsg(prAdapter,
                            MBOX_ID_0,
                            (P_MSG_HDR_T) prMsgChReq,
                            MSG_SEND_METHOD_BUF);

                prAisFsmInfo->fgIsChannelRequested = TRUE;
                break;

            case AIS_STATE_JOIN:
                aisFsmStateInit_JOIN(prAdapter, prAisFsmInfo->prTargetBssDesc);
                break;

#if CFG_SUPPORT_ADHOC
            case AIS_STATE_IBSS_ALONE:
                aisFsmStateInit_IBSS_ALONE(prAdapter);
                break;

            case AIS_STATE_IBSS_MERGE:
                aisFsmStateInit_IBSS_MERGE(prAdapter, prAisFsmInfo->prTargetBssDesc);
                break;
#endif /* CFG_SUPPORT_ADHOC */

            case AIS_STATE_NORMAL_TR:
                if (prAisFsmInfo->fgIsInfraChannelFinished == FALSE) {
                    /* Don't do anything when rJoinTimeoutTimer is still ticking */
                } else {
                    /* 1. Process for pending scan */
                    if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_SCAN, TRUE) == TRUE) {
                        wlanClearScanningResult(prAdapter);
                        eNextState = AIS_STATE_ONLINE_SCAN;
                        fgIsTransition = TRUE;
                    }
                    /* 2. Process for pending roaming scan */
                    else if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_ROAMING_SEARCH, TRUE) == TRUE) {
                        eNextState = AIS_STATE_LOOKING_FOR;
                        fgIsTransition = TRUE;
                    }
                    /* 3. Process for pending roaming scan */
                    else if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_ROAMING_CONNECT, TRUE) == TRUE) {
                        eNextState = AIS_STATE_SEARCH;
                        fgIsTransition = TRUE;
                    }
                }

                break;

            case AIS_STATE_DISCONNECTING:
                /* send for deauth frame for disconnection */
                if (authSendDeauthFrame(prAdapter,
                                        prAisBssInfo->prStaRecOfAP,
                                        (P_SW_RFB_T)NULL,
                                        REASON_CODE_DEAUTH_LEAVING_BSS,
                                        aisDeauthXmitComplete) != WLAN_STATUS_SUCCESS) {
                    MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_16, "authSendDeauthFrame fail");
                }
                break;

            default:
                ASSERT(0); /* Make sure we have handle all STATEs */
                break;

        }
    } while (fgIsTransition);

    return;

} /* end of aisFsmSteps() */


/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventScanDone(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_SCN_SCAN_DONE prScanDoneMsg;
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;
    UINT_8 ucSeqNumOfCompMsg;
    P_CONNECTION_SETTINGS_T prConnSettings;

    DEBUGFUNC("aisFsmRunEventScanDone()");

    ASSERT(prAdapter);
    ASSERT(prMsgHdr);

    DBGLOG(AIS, LOUD, ("EVENT-SCAN DONE: Current Time = %ld\n", kalGetTimeTick()));

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    prScanDoneMsg = (P_MSG_SCN_SCAN_DONE)prMsgHdr;
    ASSERT(prScanDoneMsg->ucNetTypeIndex == (UINT_8)NETWORK_TYPE_AIS_INDEX);

    ucSeqNumOfCompMsg = prScanDoneMsg->ucSeqNum;
    cnmMemFree(prAdapter, prMsgHdr);

    eNextState = prAisFsmInfo->eCurrentState;

    if (ucSeqNumOfCompMsg != prAisFsmInfo->ucSeqNumOfScanReq) {
        DBGLOG(AIS, WARN, ("SEQ NO of AIS SCN DONE MSG is not matched.\n"));
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_17, "SEQ NO(%d) of AIS SCN DONE MSG(%d) is not matched",
                     ucSeqNumOfCompMsg, prAisFsmInfo->ucSeqNumOfChReq);
    } else {
        switch (prAisFsmInfo->eCurrentState) {
            case AIS_STATE_SCAN:
                prConnSettings->fgIsScanReqIssued = FALSE;

                kalScanDone(prAdapter->prGlueInfo, KAL_NETWORK_TYPE_AIS_INDEX, WLAN_STATUS_SUCCESS);
                eNextState = AIS_STATE_IDLE;
                break;

            case AIS_STATE_ONLINE_SCAN:
                prConnSettings->fgIsScanReqIssued = FALSE;

                kalScanDone(prAdapter->prGlueInfo, KAL_NETWORK_TYPE_AIS_INDEX, WLAN_STATUS_SUCCESS);
#if CFG_SUPPORT_ROAMING
                eNextState = aisFsmRoamingScanResultsUpdate(prAdapter);
#else
                eNextState = AIS_STATE_NORMAL_TR;
#endif /* CFG_SUPPORT_ROAMING */
                break;

            case AIS_STATE_LOOKING_FOR:
#if CFG_SUPPORT_ROAMING
                eNextState = aisFsmRoamingScanResultsUpdate(prAdapter);
#else
                eNextState = AIS_STATE_SEARCH;
#endif /* CFG_SUPPORT_ROAMING */
                break;

            case AIS_STATE_IDLE:
                prConnSettings->fgIsScanReqIssued = FALSE;
                kalScanDone(prAdapter->prGlueInfo, KAL_NETWORK_TYPE_AIS_INDEX, WLAN_STATUS_SUCCESS);
                break;
            default:
                MT5931_TRACE(TRACE_ERROR, MT5931_INFO_18, "aisFsmRunEventScanDone: abnoraml AIS STATE %d", prAisFsmInfo->eCurrentState);
                break;

        }
    }

    if (eNextState != prAisFsmInfo->eCurrentState) {
        aisFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of aisFsmRunEventScanDone() */


/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventAbort(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_AIS_ABORT_T prAisAbortMsg;
    P_AIS_FSM_INFO_T prAisFsmInfo;
    UINT_8 ucReasonOfDisconnect;
    BOOLEAN fgDelayIndication;


    ASSERT(prAdapter);
    ASSERT(prMsgHdr);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    //4 <1> Extract information of Abort Message and then free memory.
    prAisAbortMsg = (P_MSG_AIS_ABORT_T)prMsgHdr;
    ucReasonOfDisconnect = prAisAbortMsg->ucReasonOfDisconnect;
    fgDelayIndication = prAisAbortMsg->fgDelayIndication;

    cnmMemFree(prAdapter, prMsgHdr);


    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_19, "EVENT-ABORT: Current State %d",
                 prAisFsmInfo->eCurrentState);

    //4 <2> clear previous pending connection request and insert new one
    aisFsmIsRequestPending(prAdapter, AIS_REQUEST_RECONNECT, TRUE);
    aisFsmInsertRequest(prAdapter, AIS_REQUEST_RECONNECT);

    if (prAisFsmInfo->eCurrentState != AIS_STATE_DISCONNECTING) {
        //4 <3> invoke abort handler
        aisFsmStateAbort(prAdapter, ucReasonOfDisconnect, fgDelayIndication);
    }

    return;
} /* end of aisFsmRunEventAbort() */


/*----------------------------------------------------------------------------*/
/*!
* \brief        This function handles AIS-FSM abort event/command
*
* \param[in] prAdapter              Pointer of ADAPTER_T
*            ucReasonOfDisconnect   Reason for disonnection
*            fgDelayIndication      Option to delay disconnection indication
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmStateAbort(
    IN P_ADAPTER_T prAdapter,
    UINT_8         ucReasonOfDisconnect,
    BOOLEAN        fgDelayIndication
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    //P_CONNECTION_SETTINGS_T prConnSettings;
    BOOLEAN fgIsCheckConnected;

    ASSERT(prAdapter);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    //prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    fgIsCheckConnected = FALSE;

    //4 <1> Save information of Abort Message and then free memory.
    prAisBssInfo->ucReasonOfDisconnect = ucReasonOfDisconnect;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_STATE, "aisFsmStateAbort() AIS_STATE=%d,Reason=%d", prAisFsmInfo->eCurrentState, ucReasonOfDisconnect);
    //4 <2> Abort current job.
    switch (prAisFsmInfo->eCurrentState) {
        case AIS_STATE_IDLE:
            break;
        case AIS_STATE_SEARCH:
            fgIsCheckConnected = TRUE;
            break;

        case AIS_STATE_WAIT_FOR_NEXT_SCAN:
            /* Do cancel timer */
            cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rBGScanTimer);
            fgIsCheckConnected = TRUE;
            break;

        case AIS_STATE_SCAN:
            /* Do abort SCAN */
            aisFsmStateAbort_SCAN(prAdapter);

            /* queue for later handling */
            if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_SCAN, FALSE) == FALSE) {
                aisFsmInsertRequest(prAdapter, AIS_REQUEST_SCAN);
            }

            break;

        case AIS_STATE_LOOKING_FOR:
            /* Do abort SCAN */
            aisFsmStateAbort_SCAN(prAdapter);
            fgIsCheckConnected = TRUE;
            break;

        case AIS_STATE_REQ_CHANNEL_JOIN:
            /* Release channel to CNM */
            aisFsmReleaseCh(prAdapter);
            fgIsCheckConnected = TRUE;
            break;

        case AIS_STATE_JOIN:
            /* Do abort JOIN */
            aisFsmStateAbort_JOIN(prAdapter);
            fgIsCheckConnected = TRUE;
            break;

#if CFG_SUPPORT_ADHOC
        case AIS_STATE_IBSS_ALONE:
        case AIS_STATE_IBSS_MERGE:
            aisFsmStateAbort_IBSS(prAdapter);
            break;
#endif /* CFG_SUPPORT_ADHOC */

        case AIS_STATE_ONLINE_SCAN:
            /* Do abort SCAN */
            aisFsmStateAbort_SCAN(prAdapter);

            /* queue for later handling */
            if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_SCAN, FALSE) == FALSE) {
                aisFsmInsertRequest(prAdapter, AIS_REQUEST_SCAN);
            }

            fgIsCheckConnected = TRUE;
            break;

        case AIS_STATE_NORMAL_TR:
            fgIsCheckConnected = TRUE;
            break;

        case AIS_STATE_DISCONNECTING:
            /* Do abort NORMAL_TR */
            aisFsmStateAbort_NORMAL_TR(prAdapter);

            break;

        default:
            break;
    }

    if (fgIsCheckConnected &&
            (PARAM_MEDIA_STATE_CONNECTED == prAisBssInfo->eConnectionState)) {

        /* switch into DISCONNECTING state for sending DEAUTH if necessary */
        if (prAisBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE &&
                prAisBssInfo->ucReasonOfDisconnect == DISCONNECT_REASON_CODE_NEW_CONNECTION &&
                prAisBssInfo->prStaRecOfAP &&
                prAisBssInfo->prStaRecOfAP->fgIsInUse) {
            aisFsmSteps(prAdapter, AIS_STATE_DISCONNECTING);

            return;
        } else {
            /* Do abort NORMAL_TR */
            aisFsmStateAbort_NORMAL_TR(prAdapter);
        }
    }

    aisFsmDisconnect(prAdapter, fgDelayIndication);

    return;

} /* end of aisFsmStateAbort() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will handle the Join Complete Event from SAA FSM for AIS FSM
*
* @param[in] prMsgHdr   Message of Join Complete of SAA FSM.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventJoinComplete(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_JOIN_COMP_T prJoinCompMsg;
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;
    P_STA_RECORD_T prStaRec;
    P_SW_RFB_T prAssocRspSwRfb;


    ASSERT(prMsgHdr);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prJoinCompMsg = (P_MSG_JOIN_COMP_T)prMsgHdr;
    prStaRec = prJoinCompMsg->prStaRec;
    prAssocRspSwRfb = prJoinCompMsg->prSwRfb;

    eNextState = prAisFsmInfo->eCurrentState;

    // Check State and SEQ NUM
    if (prAisFsmInfo->eCurrentState == AIS_STATE_JOIN) {

        // Check SEQ NUM
        if (prJoinCompMsg->ucSeqNum == prAisFsmInfo->ucSeqNumOfReqMsg) {


            //4 <1> JOIN was successful
            if (prJoinCompMsg->rJoinStatus == WLAN_STATUS_SUCCESS) {
                P_BSS_INFO_T prAisBssInfo;

                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "aisFsmRunEventJoinComplete SUCCESS \n");
                prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

                //1. Reset retry count
                prAisFsmInfo->ucConnTrialCount = 0;

                // Completion of roaming
                if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {

#if CFG_SUPPORT_ROAMING
                    //2. Deactivate previous BSS
                    aisFsmRoamingDisconnectPrevAP(prAdapter, prStaRec);

                    //3. Update bss based on roaming staRec
                    aisUpdateBssInfoForRoamingAP(prAdapter, prStaRec, prAssocRspSwRfb);
#endif /* CFG_SUPPORT_ROAMING */
                } else {
                    //4 <1.1> Change FW's Media State immediately.
                    aisChangeMediaState(prAdapter, PARAM_MEDIA_STATE_CONNECTED);

                    //4 <1.2> Deactivate previous AP's STA_RECORD_T in Driver if have.
                    if ((prAisBssInfo->prStaRecOfAP) &&
                            (prAisBssInfo->prStaRecOfAP != prStaRec) &&
                            (prAisBssInfo->prStaRecOfAP->fgIsInUse)) {

                        cnmStaRecChangeState(prAdapter, prAisBssInfo->prStaRecOfAP, STA_STATE_1);
                    }

                    //4 <1.3> Update BSS_INFO_T
                    aisUpdateBssInfoForJOIN(prAdapter, prStaRec, prAssocRspSwRfb);

                    //4 <1.4> Activate current AP's STA_RECORD_T in Driver.
                    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);

                    //4 <1.5> Update RSSI if necessary
                    nicUpdateRSSI(prAdapter, NETWORK_TYPE_AIS_INDEX, (INT_8)(RCPI_TO_dBm(prStaRec->ucRCPI)), 0);

                    //4 <1.6> Indicate Connected Event to Host immediately.
                    /* Require BSSID, Association ID, Beacon Interval.. from AIS_BSS_INFO_T */
                    aisIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_CONNECTED, FALSE);
                }

#if CFG_SUPPORT_ROAMING
                roamingFsmRunEventStart(prAdapter);
#endif /* CFG_SUPPORT_ROAMING */

                //4 <1.7> Set the Next State of AIS FSM
                eNextState = AIS_STATE_NORMAL_TR;
            }
            //4 <2> JOIN was not successful
            else {
                P_BSS_INFO_T prAisBssInfo;
                prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "aisFsmRunEventJoinComplete FAILURE \n");
                //4 <2.1> Redo JOIN process with other Auth Type if possible
                if (aisFsmStateInit_RetryJOIN(prAdapter, prStaRec) == FALSE) {
                    P_BSS_DESC_T prBssDesc;

                    /* 1. Increase Failure Count */
                    prStaRec->ucJoinFailureCount++;

                    /* 2. release channel */
                    aisFsmReleaseCh(prAdapter);
                    prAisFsmInfo->fgIsInfraChannelFinished = TRUE;

                    /* 3.1 stop join timeout timer */
                    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rJoinTimeoutTimer);

                    /* 3.2 reset local variable */
                    prAisFsmInfo->fgIsInfraChannelFinished = TRUE;

                    prBssDesc = scanSearchBssDescByBssid(prAdapter, prStaRec->aucMacAddr);

                    ASSERT(prBssDesc);
                    //ASSERT(prBssDesc->fgIsConnecting); /* NOTE(Nelson): assert will be handled as follow */

                    if (prBssDesc) {
                        prBssDesc->fgIsConnecting = FALSE;
                    }

                    /* 3.3 Free STA-REC */
                    if (prStaRec != prAisBssInfo->prStaRecOfAP) {
                        cnmStaRecFree(prAdapter, prStaRec, FALSE);
                    }

                    /* 4. send reconnect request */
                    aisFsmInsertRequest(prAdapter, AIS_REQUEST_RECONNECT);

                    eNextState = AIS_STATE_IDLE;
                }
            }
        }
#if 1
        else {
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "SEQ NO of AIS JOIN COMP MSG is not matched.\n");
        }
#endif /* DBG */

    }

    if (eNextState != prAisFsmInfo->eCurrentState) {
        aisFsmSteps(prAdapter, eNextState);
    }

    if (prAssocRspSwRfb) {
        nicRxReturnRFB(prAdapter, prAssocRspSwRfb);
    }

    cnmMemFree(prAdapter, prMsgHdr);

    return;
} /* end of aisFsmRunEventJoinComplete() */


#if CFG_SUPPORT_ADHOC
/*----------------------------------------------------------------------------*/
/*!
* @brief This function will handle the Grant Msg of IBSS Create which was sent by
*        CNM to indicate that channel was changed for creating IBSS.
*
* @param[in] prAdapter  Pointer of ADAPTER_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmCreateIBSS(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;

    DEBUGFUNC("aisFsmCreateIBSS");

    ASSERT(prAdapter);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    do {
        // Check State
        if (prAisFsmInfo->eCurrentState == AIS_STATE_IBSS_ALONE) {
            aisUpdateBssInfoForCreateIBSS(prAdapter);
        }
    } while (FALSE);

    return;
} /* end of aisFsmCreateIBSS() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will handle the Grant Msg of IBSS Merge which was sent by
*        CNM to indicate that channel was changed for merging IBSS.
*
* @param[in] prAdapter  Pointer of ADAPTER_T
* @param[in] prStaRec   Pointer of STA_RECORD_T for merge
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmMergeIBSS(
    IN P_ADAPTER_T      prAdapter,
    IN P_STA_RECORD_T   prStaRec
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;
    P_BSS_INFO_T prAisBssInfo;

    DEBUGFUNC("aisFsmMergeIBSS");

    ASSERT(prAdapter);
    ASSERT(prStaRec);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    do {

        eNextState = prAisFsmInfo->eCurrentState;

        switch (prAisFsmInfo->eCurrentState) {
            case AIS_STATE_IBSS_MERGE: {
                P_BSS_DESC_T prBssDesc;

                //4 <1.1> Change FW's Media State immediately.
                aisChangeMediaState(prAdapter, PARAM_MEDIA_STATE_CONNECTED);

                //4 <1.2> Deactivate previous Peers' STA_RECORD_T in Driver if have.
#if (CFG_SUPPORT_ADHOC) || (CFG_SUPPORT_AAA)
                bssClearClientList(prAdapter, prAisBssInfo);
#endif

                //4 <1.3> Unmark connection flag of previous BSS_DESC_T.
                if ((prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID)) != NULL) {
                    prBssDesc->fgIsConnecting = FALSE;
                    prBssDesc->fgIsConnected = FALSE;
                }

                //4 <1.4> Update BSS_INFO_T
                aisUpdateBssInfoForMergeIBSS(prAdapter, prStaRec);

                //4 <1.5> Add Peers' STA_RECORD_T to Client List
                bssAddStaRecToClientList(prAdapter, prAisBssInfo, prStaRec);

                //4 <1.6> Activate current Peer's STA_RECORD_T in Driver.
                cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);
                prStaRec->fgIsMerging = FALSE;

                //4 <1.7> Enable other features

                //4 <1.8> Indicate Connected Event to Host immediately.
                aisIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_CONNECTED, FALSE);

                //4 <1.9> Set the Next State of AIS FSM
                eNextState = AIS_STATE_NORMAL_TR;

                //4 <1.10> Release channel privilege
                aisFsmReleaseCh(prAdapter);

#if CFG_SLT_SUPPORT
                prAdapter->rWifiVar.rSltInfo.prPseudoStaRec = prStaRec;
#endif
            }
            break;

            default:
                break;
        }

        if (eNextState != prAisFsmInfo->eCurrentState) {
            aisFsmSteps(prAdapter, eNextState);
        }

    } while (FALSE);

    return;
} /* end of aisFsmMergeIBSS() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will handle the Notification of existing IBSS was found
*        from SCN.
*
* @param[in] prMsgHdr   Message of Notification of an IBSS was present.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventFoundIBSSPeer(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_AIS_IBSS_PEER_FOUND_T prAisIbssPeerFoundMsg;
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;
    P_STA_RECORD_T prStaRec;
    P_BSS_INFO_T prAisBssInfo;
    P_BSS_DESC_T prBssDesc;
    BOOLEAN fgIsMergeIn;

    DEBUGFUNC("aisFsmRunEventFoundIBSSPeer");

    ASSERT(prMsgHdr);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    prAisIbssPeerFoundMsg = (P_MSG_AIS_IBSS_PEER_FOUND_T)prMsgHdr;

    ASSERT(prAisIbssPeerFoundMsg->ucNetTypeIndex == NETWORK_TYPE_AIS_INDEX);

    prStaRec = prAisIbssPeerFoundMsg->prStaRec;
    ASSERT(prStaRec);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_20, "aisFsmRunEventFoundIBSSPeer: prAisIbssPeerFoundMsg->fgIsMergeIn = %d",
                 prAisIbssPeerFoundMsg->fgIsMergeIn);

    fgIsMergeIn = prAisIbssPeerFoundMsg->fgIsMergeIn;

    cnmMemFree(prAdapter, prMsgHdr);


    eNextState = prAisFsmInfo->eCurrentState;
    switch (prAisFsmInfo->eCurrentState) {
        case AIS_STATE_IBSS_ALONE: {
            //4 <1> An IBSS Peer 'merged in'.
            if (fgIsMergeIn) {

                //4 <1.1> Change FW's Media State immediately.
                aisChangeMediaState(prAdapter, PARAM_MEDIA_STATE_CONNECTED);

                //4 <1.2> Add Peers' STA_RECORD_T to Client List
                bssAddStaRecToClientList(prAdapter, prAisBssInfo, prStaRec);

#if CFG_SLT_SUPPORT
                //4 <1.3> Mark connection flag of BSS_DESC_T.
                if ((prBssDesc = scanSearchBssDescByTA(prAdapter, prStaRec->aucMacAddr)) != NULL) {
                    prBssDesc->fgIsConnecting = FALSE;
                    prBssDesc->fgIsConnected = TRUE;
                } else {
                    ASSERT(0); // Should be able to find a BSS_DESC_T here.
                }

                //4 <1.4> Activate current Peer's STA_RECORD_T in Driver.
                prStaRec->fgIsQoS = TRUE; /* TODO(Kevin): TBD */
#else
                //4 <1.3> Mark connection flag of BSS_DESC_T.
                if ((prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID)) != NULL) {
                    prBssDesc->fgIsConnecting = FALSE;
                    prBssDesc->fgIsConnected = TRUE;
                } else {
                    ASSERT(0); // Should be able to find a BSS_DESC_T here.
                }


                //4 <1.4> Activate current Peer's STA_RECORD_T in Driver.
                prStaRec->fgIsQoS = FALSE; /* TODO(Kevin): TBD */

#endif

                cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);
                prStaRec->fgIsMerging = FALSE;

                //4 <1.6> sync. to firmware
                nicUpdateBss(prAdapter, NETWORK_TYPE_AIS_INDEX);

                //4 <1.7> Indicate Connected Event to Host immediately.
                aisIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_CONNECTED, FALSE);

                //4 <1.8> indicate PM for connected
                nicPmIndicateBssConnected(prAdapter, NETWORK_TYPE_AIS_INDEX);

                //4 <1.9> Set the Next State of AIS FSM
                eNextState = AIS_STATE_NORMAL_TR;

                //4 <1.10> Release channel privilege
                aisFsmReleaseCh(prAdapter);
            }
            //4 <2> We need 'merge out' to this IBSS
            else {

                //4 <2.1> Get corresponding BSS_DESC_T
                prBssDesc = scanSearchBssDescByTA(prAdapter, prStaRec->aucMacAddr);

                prAisFsmInfo->prTargetBssDesc = prBssDesc;

                //4 <2.2> Set the Next State of AIS FSM
                eNextState = AIS_STATE_IBSS_MERGE;
            }
        }
        break;

        case AIS_STATE_NORMAL_TR: {

            //4 <3> An IBSS Peer 'merged in'.
            if (fgIsMergeIn) {

                //4 <3.1> Add Peers' STA_RECORD_T to Client List
                bssAddStaRecToClientList(prAdapter, prAisBssInfo, prStaRec);

#if CFG_SLT_SUPPORT
                //4 <3.2> Activate current Peer's STA_RECORD_T in Driver.
                prStaRec->fgIsQoS = TRUE; /* TODO(Kevin): TBD */
#else
                //4 <3.2> Activate current Peer's STA_RECORD_T in Driver.
                prStaRec->fgIsQoS = FALSE; /* TODO(Kevin): TBD */
#endif

                cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);
                prStaRec->fgIsMerging = FALSE;

            }
            //4 <4> We need 'merge out' to this IBSS
            else {

                //4 <4.1> Get corresponding BSS_DESC_T
                prBssDesc = scanSearchBssDescByTA(prAdapter, prStaRec->aucMacAddr);

                prAisFsmInfo->prTargetBssDesc = prBssDesc;

                //4 <4.2> Set the Next State of AIS FSM
                eNextState = AIS_STATE_IBSS_MERGE;

            }
        }
        break;

        default:
            break;
    }

    if (eNextState != prAisFsmInfo->eCurrentState) {
        aisFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of aisFsmRunEventFoundIBSSPeer() */
#endif /* CFG_SUPPORT_ADHOC */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indicate the Media State to HOST
*
* @param[in] eConnectionState   Current Media State
* @param[in] fgDelayIndication  Set TRUE for postponing the Disconnect Indication.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisIndicationOfMediaStateToHost(
    IN P_ADAPTER_T prAdapter,
    ENUM_PARAM_MEDIA_STATE_T eConnectionState,
    BOOLEAN fgDelayIndication
)
{
    EVENT_CONNECTION_STATUS rEventConnStatus;
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_INFO_T prAisBssInfo;
    P_AIS_FSM_INFO_T prAisFsmInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "aisIndicationOfMediaStateToHost(),fgDelayIndication=%d", fgDelayIndication);

    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    // NOTE(Kevin): Move following line to aisChangeMediaState() macro per CM's request.
    //prAisBssInfo->eConnectionState = eConnectionState;

    /* For indicating the Disconnect Event only if current media state is
     * disconnected and we didn't do indication yet.
     */
    if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_DISCONNECTED) {
        if (prAisBssInfo->eConnectionStateIndicated == eConnectionState) {
            return;
        }
    }

    if (!fgDelayIndication) {
        //4 <0> Cancel Delay Timer
        cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rIndicationOfDisconnectTimer);

        //4 <1> Fill EVENT_CONNECTION_STATUS
        rEventConnStatus.ucMediaStatus = (UINT_8)eConnectionState;

        if (eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {
            rEventConnStatus.ucReasonOfDisconnect = DISCONNECT_REASON_CODE_RESERVED;

            if (prAisBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE) {
                rEventConnStatus.ucInfraMode = (UINT_8)NET_TYPE_INFRA;
                rEventConnStatus.u2AID = prAisBssInfo->u2AssocId;
                rEventConnStatus.u2ATIMWindow = 0;
            } else if (prAisBssInfo->eCurrentOPMode == OP_MODE_IBSS) {
                rEventConnStatus.ucInfraMode = (UINT_8)NET_TYPE_IBSS;
                rEventConnStatus.u2AID = 0;
                rEventConnStatus.u2ATIMWindow = prAisBssInfo->u2ATIMWindow;
            } else {
                ASSERT(0);
            }

            COPY_SSID(rEventConnStatus.aucSsid,
                      rEventConnStatus.ucSsidLen,
                      prConnSettings->aucSSID,
                      prConnSettings->ucSSIDLen);

            COPY_MAC_ADDR(rEventConnStatus.aucBssid, prAisBssInfo->aucBSSID);

            rEventConnStatus.u2BeaconPeriod = prAisBssInfo->u2BeaconInterval;
            rEventConnStatus.u4FreqInKHz = nicChannelNum2Freq(prAisBssInfo->ucPrimaryChannel);

            switch (prAisBssInfo->ucNonHTBasicPhyType) {
                case PHY_TYPE_HR_DSSS_INDEX:
                    rEventConnStatus.ucNetworkType = (UINT_8)PARAM_NETWORK_TYPE_DS;
                    break;

                case PHY_TYPE_ERP_INDEX:
                    rEventConnStatus.ucNetworkType = (UINT_8)PARAM_NETWORK_TYPE_OFDM24;
                    break;

                case PHY_TYPE_OFDM_INDEX:
                    rEventConnStatus.ucNetworkType = (UINT_8)PARAM_NETWORK_TYPE_OFDM5;
                    break;

                default:
                    ASSERT(0);
                    rEventConnStatus.ucNetworkType = (UINT_8)PARAM_NETWORK_TYPE_DS;
                    break;
            }
        } else {
#if CFG_PRIVACY_MIGRATION
            /* Clear the pmkid cache while media disconnect */
            secClearPmkid(prAdapter);
#endif

            rEventConnStatus.ucReasonOfDisconnect = prAisBssInfo->ucReasonOfDisconnect;

        }

        //4 <2> Indication
        nicMediaStateChange(prAdapter, NETWORK_TYPE_AIS_INDEX, &rEventConnStatus);
        prAisBssInfo->eConnectionStateIndicated = eConnectionState;
    } else {
        /* NOTE: Only delay the Indication of Disconnect Event */
        ASSERT(eConnectionState == PARAM_MEDIA_STATE_DISCONNECTED);

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DISCONN, "Postpone the indication of Disconnect for %d seconds\n",
                         prConnSettings->ucDelayTimeOfDisconnectEvent);

        cnmTimerStartTimer(prAdapter,
                           &prAisFsmInfo->rIndicationOfDisconnectTimer,
                           SEC_TO_MSEC(prConnSettings->ucDelayTimeOfDisconnectEvent));
    }

    return;
} /* end of aisIndicationOfMediaStateToHost() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indicate an Event of "Media Disconnect" to HOST
*
* @param[in] u4Param  Unused timer parameter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisPostponedEventOfDisconnTimeout(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
)
{
    P_BSS_INFO_T prAisBssInfo;

#ifdef __WIFI_SNIFFER_SUPPORT__
    prAdapter->fgDelayIndDisc = 1;
    kal_wap_trace(MOD_WNDRV, TRACE_INFO, "timer timeout,delay aisIndicationOfMediaStateToHost until AIS idle");
    prAdapter->rWifiVar.rConnSettings.fgIsConnReqIssued = FALSE;
    return;
#endif

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    //4 <1> Deactivate previous AP's STA_RECORD_T in Driver if have.
    if (prAisBssInfo->prStaRecOfAP) {
        //cnmStaRecChangeState(prAdapter, prAisBssInfo->prStaRecOfAP, STA_STATE_1);

        prAisBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;
    }

    //4 <2> Deactivate previous Peers' STA_RECORD_T in Driver if have.
#if (CFG_SUPPORT_ADHOC) || (CFG_SUPPORT_AAA)
    bssClearClientList(prAdapter, prAisBssInfo);
#endif

    //4 <3> Indicate Disconnected Event to Host immediately.
    aisIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED, FALSE);

    /* NOTE(Nelson): Do not retry to connect to the SSID if disconnect timeout */
    prAdapter->rWifiVar.rConnSettings.fgIsConnReqIssued = FALSE;

    return;
} /* end of aisPostponedEventOfDisconnTimeout() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will update the contain of BSS_INFO_T for AIS network once
*        the association was completed.
*
* @param[in] prStaRec               Pointer to the STA_RECORD_T
* @param[in] prAssocRspSwRfb        Pointer to SW RFB of ASSOC RESP FRAME.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisUpdateBssInfoForJOIN(
    IN P_ADAPTER_T prAdapter,
    P_STA_RECORD_T prStaRec,
    P_SW_RFB_T prAssocRspSwRfb
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
    P_BSS_DESC_T prBssDesc;
    UINT_16 u2IELength;
    PUINT_8 pucIE;

    DEBUGFUNC("aisUpdateBssInfoForJOIN()");

    ASSERT(prStaRec);
    ASSERT(prAssocRspSwRfb);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T) prAssocRspSwRfb->pvHeader;


    DBGLOG(AIS, INFO, ("Update AIS_BSS_INFO_T and apply settings to MAC\n"));


    //3 <1> Update BSS_INFO_T from AIS_FSM_INFO_T or User Settings
    //4 <1.1> Setup Operation Mode
    prAisBssInfo->eCurrentOPMode = OP_MODE_INFRASTRUCTURE;

    //4 <1.2> Setup SSID
    COPY_SSID(prAisBssInfo->aucSSID,
              prAisBssInfo->ucSSIDLen,
              prConnSettings->aucSSID,
              prConnSettings->ucSSIDLen);

    //4 <1.3> Setup Channel, Band
    prAisBssInfo->ucPrimaryChannel = prAisFsmInfo->prTargetBssDesc->ucChannelNum;
    prAisBssInfo->eBand = prAisFsmInfo->prTargetBssDesc->eBand;


    //3 <2> Update BSS_INFO_T from STA_RECORD_T
    //4 <2.1> Save current AP's STA_RECORD_T and current AID
    prAisBssInfo->prStaRecOfAP = prStaRec;
    prAisBssInfo->u2AssocId = prStaRec->u2AssocId;

    //4 <2.2> Setup Capability
    prAisBssInfo->u2CapInfo = prStaRec->u2CapInfo; /* Use AP's Cap Info as BSS Cap Info */

    if (prAisBssInfo->u2CapInfo & CAP_INFO_SHORT_PREAMBLE) {
        prAisBssInfo->fgIsShortPreambleAllowed = TRUE;
        prAisBssInfo->fgUseShortPreamble = TRUE;
    } else {
        prAisBssInfo->fgIsShortPreambleAllowed = FALSE;
        prAisBssInfo->fgUseShortPreamble = FALSE;
    }

    if (prAisBssInfo->u2CapInfo & CAP_INFO_SHORT_SLOT_TIME) {
        prAisBssInfo->fgUseShortSlotTime = TRUE;
    } else {
        prAisBssInfo->fgUseShortSlotTime = FALSE;
    }

    //4 <2.3> Setup PHY Attributes and Basic Rate Set/Operational Rate Set
    prAisBssInfo->ucPhyTypeSet = prStaRec->ucDesiredPhyTypeSet;

    prAisBssInfo->ucNonHTBasicPhyType = prStaRec->ucNonHTBasicPhyType;

    prAisBssInfo->u2OperationalRateSet = prStaRec->u2OperationalRateSet;
    prAisBssInfo->u2BSSBasicRateSet = prStaRec->u2BSSBasicRateSet;


    //3 <3> Update BSS_INFO_T from SW_RFB_T (Association Resp Frame)
    //4 <3.1> Setup BSSID
    COPY_MAC_ADDR(prAisBssInfo->aucBSSID, prAssocRspFrame->aucBSSID);


    u2IELength = (UINT_16)((prAssocRspSwRfb->u2PacketLen - prAssocRspSwRfb->u2HeaderLen) -
                           (OFFSET_OF(WLAN_ASSOC_RSP_FRAME_T, aucInfoElem[0]) - WLAN_MAC_MGMT_HEADER_LEN));
    pucIE = prAssocRspFrame->aucInfoElem;


    //4 <3.2> Parse WMM and setup QBSS flag
    /* Parse WMM related IEs and configure HW CRs accordingly */
    mqmProcessAssocRsp(prAdapter, prAssocRspSwRfb, pucIE, u2IELength);

    prAisBssInfo->fgIsQBSS = prStaRec->fgIsQoS;

    //3 <4> Update BSS_INFO_T from BSS_DESC_T
    prBssDesc = scanSearchBssDescByBssid(prAdapter, (UINT_8 *)prAssocRspFrame->aucBSSID);
    if (prBssDesc) {
        prBssDesc->fgIsConnecting = FALSE;
        prBssDesc->fgIsConnected = TRUE;

        //4 <4.1> Setup MIB for current BSS
        prAisBssInfo->u2BeaconInterval = prBssDesc->u2BeaconInterval;
    } else {
        // should never happen
        ASSERT(0);
    }

    /* NOTE: Defer ucDTIMPeriod updating to when beacon is received after connection */
    prAisBssInfo->ucDTIMPeriod = 0;
    prAisBssInfo->u2ATIMWindow = 0;

    prAisBssInfo->ucBeaconTimeoutCount = AIS_BEACON_TIMEOUT_COUNT_INFRA;

    //4 <4.2> Update HT information and set channel
    /* Record HT related parameters in rStaRec and rBssInfo
     * Note: it shall be called before nicUpdateBss()
     */
    rlmProcessAssocRsp(prAdapter, prAssocRspSwRfb, pucIE, u2IELength);

    //4 <4.3> Sync with firmware for BSS-INFO
    nicUpdateBss(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //4 <4.4> *DEFER OPERATION* nicPmIndicateBssConnected() will be invoked
    //inside scanProcessBeaconAndProbeResp() after 1st beacon is received

    return;
} /* end of aisUpdateBssInfoForJOIN() */


#if CFG_SUPPORT_ADHOC
/*----------------------------------------------------------------------------*/
/*!
* @brief This function will create an Ad-Hoc network and start sending Beacon Frames.
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisUpdateBssInfoForCreateIBSS(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;

    DEBUGFUNC("aisUpdateBssInfoForCreateIBSS");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_21, "aisUpdateBssInfoForCreateIBSS: prConnSettings->aucBSSID["MACSTR"]",
                 MAC2STR(prConnSettings->aucBSSID));

    if (prAisBssInfo->fgIsBeaconActivated) {
        return;
    }

    //3 <1> Update BSS_INFO_T per Network Basis
    //4 <1.1> Setup Operation Mode
    prAisBssInfo->eCurrentOPMode = OP_MODE_IBSS;

    //4 <1.2> Setup SSID
    COPY_SSID(prAisBssInfo->aucSSID,
              prAisBssInfo->ucSSIDLen,
              prConnSettings->aucSSID,
              prConnSettings->ucSSIDLen);

    //4 <1.3> Clear current AP's STA_RECORD_T and current AID
    prAisBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;
    prAisBssInfo->u2AssocId = 0;

    //4 <1.4> Setup Channel, Band and Phy Attributes
    prAisBssInfo->ucPrimaryChannel = prConnSettings->ucAdHocChannelNum;
    prAisBssInfo->eBand = prConnSettings->eAdHocBand;

    if (prAisBssInfo->eBand == BAND_2G4) {

        prAisBssInfo->ucPhyTypeSet =
            prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11BGN; /* Depend on eBand */

        prAisBssInfo->ucConfigAdHocAPMode = AD_HOC_MODE_MIXED_11BG; /* Depend on eCurrentOPMode and ucPhyTypeSet */
        //prAisBssInfo->ucConfigAdHocAPMode = AD_HOC_MODE_11B; /* Depend on eCurrentOPMode and ucPhyTypeSet */
    } else {

        prAisBssInfo->ucPhyTypeSet =
            prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11AN; /* Depend on eBand */

        prAisBssInfo->ucConfigAdHocAPMode = AD_HOC_MODE_11A; /* Depend on eCurrentOPMode and ucPhyTypeSet */
    }

    //4 <1.5> Setup MIB for current BSS
    prAisBssInfo->u2BeaconInterval = prConnSettings->u2BeaconPeriod;
    prAisBssInfo->ucDTIMPeriod = 0;
    prAisBssInfo->u2ATIMWindow = prConnSettings->u2AtimWindow;

    prAisBssInfo->ucBeaconTimeoutCount = AIS_BEACON_TIMEOUT_COUNT_ADHOC;

#if CFG_PRIVACY_MIGRATION
    if (prConnSettings->eEncStatus == ENUM_ENCRYPTION1_ENABLED ||
            prConnSettings->eEncStatus == ENUM_ENCRYPTION2_ENABLED ||
            prConnSettings->eEncStatus == ENUM_ENCRYPTION3_ENABLED) {
        prAisBssInfo->fgIsProtection = TRUE;
    } else {
        prAisBssInfo->fgIsProtection = FALSE;
    }
#else
    prAisBssInfo->fgIsProtection = FALSE;
#endif

    //3 <2> Update BSS_INFO_T common part
    ibssInitForAdHoc(prAdapter, prAisBssInfo);



    //3 <3> Set MAC HW
    //4 <3.1> Setup channel and bandwidth
    rlmBssInitForAPandIbss(prAdapter, prAisBssInfo);

    //4 <3.2> use command packets to inform firmware
    nicUpdateBss(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //4 <3.3> enable beaconing
    bssUpdateBeaconContent(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //4 <3.4> Update AdHoc PM parameter
    nicPmIndicateBssCreated(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //3 <4> Set ACTIVE flag.
    prAisBssInfo->fgIsBeaconActivated = TRUE;
    prAisBssInfo->fgHoldSameBssidForIBSS = TRUE;

    //3 <5> Start IBSS Alone Timer
    cnmTimerStartTimer(prAdapter,
                       &prAisFsmInfo->rIbssAloneTimer,
                       SEC_TO_MSEC(AIS_IBSS_ALONE_TIMEOUT_SEC));

    return;

} /* end of aisCreateIBSS() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will update the contain of BSS_INFO_T for AIS network once
*        the existing IBSS was found.
*
* @param[in] prStaRec               Pointer to the STA_RECORD_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisUpdateBssInfoForMergeIBSS(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_BSS_INFO_T prAisBssInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_DESC_T prBssDesc;
    //UINT_16 u2IELength;
    //PUINT_8 pucIE;

    DEBUGFUNC("aisUpdateBssInfoForMergeIBSS");

    ASSERT(prStaRec);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_22, "aisUpdateBssInfoForMergeIBSS: prConnSettings->aucBSSID["MACSTR"]",
                 MAC2STR(prConnSettings->aucBSSID));

    cnmTimerStopTimer(prAdapter, &prAisFsmInfo->rIbssAloneTimer);

    if (!prAisBssInfo->fgIsBeaconActivated) {

        //3 <1> Update BSS_INFO_T per Network Basis
        //4 <1.1> Setup Operation Mode
        prAisBssInfo->eCurrentOPMode = OP_MODE_IBSS;

        //4 <1.2> Setup SSID
        COPY_SSID(prAisBssInfo->aucSSID,
                  prAisBssInfo->ucSSIDLen,
                  prConnSettings->aucSSID,
                  prConnSettings->ucSSIDLen);

        //4 <1.3> Clear current AP's STA_RECORD_T and current AID
        prAisBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;
        prAisBssInfo->u2AssocId = 0;
    }

    //3 <2> Update BSS_INFO_T from STA_RECORD_T
    //4 <2.1> Setup Capability
    prAisBssInfo->u2CapInfo = prStaRec->u2CapInfo; /* Use Peer's Cap Info as IBSS Cap Info */

    if (prAisBssInfo->u2CapInfo & CAP_INFO_SHORT_PREAMBLE) {
        prAisBssInfo->fgIsShortPreambleAllowed = TRUE;
        prAisBssInfo->fgUseShortPreamble = TRUE;
    } else {
        prAisBssInfo->fgIsShortPreambleAllowed = FALSE;
        prAisBssInfo->fgUseShortPreamble = FALSE;
    }

    // 7.3.1.4 For IBSS, the Short Slot Time subfield shall be set to 0.
    prAisBssInfo->fgUseShortSlotTime = FALSE; /* Set to FALSE for AdHoc */
    prAisBssInfo->u2CapInfo &= ~CAP_INFO_SHORT_SLOT_TIME;

    if (prAisBssInfo->u2CapInfo & CAP_INFO_PRIVACY) {
        prAisBssInfo->fgIsProtection = TRUE;
    } else {
        prAisBssInfo->fgIsProtection = FALSE;
    }

    //4 <2.2> Setup PHY Attributes and Basic Rate Set/Operational Rate Set
    prAisBssInfo->ucPhyTypeSet = prStaRec->ucDesiredPhyTypeSet;

    prAisBssInfo->ucNonHTBasicPhyType = prStaRec->ucNonHTBasicPhyType;
    //prAisBssInfo->ucNonHTBasicPhyType = PHY_TYPE_HR_DSSS_INDEX;

    prAisBssInfo->u2OperationalRateSet = prStaRec->u2OperationalRateSet;
    prAisBssInfo->u2BSSBasicRateSet = prStaRec->u2BSSBasicRateSet;

    rateGetDataRatesFromRateSet(prAisBssInfo->u2OperationalRateSet,
                                prAisBssInfo->u2BSSBasicRateSet,
                                prAisBssInfo->aucAllSupportedRates,
                                &prAisBssInfo->ucAllSupportedRatesLen);
    kal_wap_trace(MOD_WNDRV, TRACE_INFO, "Merge IBSS:%02x:%04x:%04x", prAisBssInfo->ucNonHTBasicPhyType, prAisBssInfo->u2BSSBasicRateSet, prAisBssInfo->u2OperationalRateSet);

    //3 <3> X Update BSS_INFO_T from SW_RFB_T (Association Resp Frame)


    //3 <4> Update BSS_INFO_T from BSS_DESC_T
    prBssDesc = scanSearchBssDescByTA(prAdapter, prStaRec->aucMacAddr);
    if (prBssDesc) {
        prBssDesc->fgIsConnecting = FALSE;
        prBssDesc->fgIsConnected = TRUE;

        //4 <4.1> Setup BSSID
        COPY_MAC_ADDR(prAisBssInfo->aucBSSID, prBssDesc->aucBSSID);

        //4 <4.2> Setup Channel, Band
        prAisBssInfo->ucPrimaryChannel = prBssDesc->ucChannelNum;
        prAisBssInfo->eBand = prBssDesc->eBand;

        //4 <4.3> Setup MIB for current BSS
        prAisBssInfo->u2BeaconInterval = prBssDesc->u2BeaconInterval;
        prAisBssInfo->ucDTIMPeriod = 0;
        prAisBssInfo->u2ATIMWindow = 0; /* TBD(Kevin) */

        prAisBssInfo->ucBeaconTimeoutCount = AIS_BEACON_TIMEOUT_COUNT_ADHOC;
    } else {
        // should never happen
        ASSERT(0);
    }


    //3 <5> Set MAC HW
    //4 <5.1> Find Lowest Basic Rate Index for default TX Rate of MMPDU
    {
        UINT_8 ucLowestBasicRateIndex;

        if (!rateGetLowestRateIndexFromRateSet(prAisBssInfo->u2BSSBasicRateSet,
                                               &ucLowestBasicRateIndex)) {

            if (prAisBssInfo->ucPhyTypeSet & PHY_TYPE_BIT_OFDM) {
                ucLowestBasicRateIndex = RATE_6M_INDEX;
            } else {
                ucLowestBasicRateIndex = RATE_1M_INDEX;
            }
        }

        prAisBssInfo->ucHwDefaultFixedRateCode =
            aucRateIndex2RateCode[prAisBssInfo->fgUseShortPreamble][ucLowestBasicRateIndex];
    }

    //4 <5.2> Setup channel and bandwidth
    rlmBssInitForAPandIbss(prAdapter, prAisBssInfo);

    //4 <5.3> use command packets to inform firmware
    nicUpdateBss(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //4 <5.4> enable beaconing
    bssUpdateBeaconContent(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //4 <5.5> Update AdHoc PM parameter
    nicPmIndicateBssConnected(prAdapter, NETWORK_TYPE_AIS_INDEX);

    //3 <6> Set ACTIVE flag.
    prAisBssInfo->fgIsBeaconActivated = TRUE;
    prAisBssInfo->fgHoldSameBssidForIBSS = TRUE;

    return;
} /* end of aisUpdateBssInfoForMergeIBSS() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will validate the Rx Probe Request Frame and then return
*        result to BSS to indicate if need to send the corresponding Probe Response
*        Frame if the specified conditions were matched.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prSwRfb            Pointer to SW RFB data structure.
* @param[out] pu4ControlFlags   Control flags for replying the Probe Response
*
* @retval TRUE      Reply the Probe Response
* @retval FALSE     Don't reply the Probe Response
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
aisValidateProbeReq(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT PUINT_32 pu4ControlFlags
)
{
    //P_WLAN_MAC_MGMT_HEADER_T prMgtHdr;
    P_BSS_INFO_T prBssInfo;
    P_IE_SSID_T prIeSsid = (P_IE_SSID_T)NULL;
    PUINT_8 pucIE;
    UINT_16 u2IELength;
    UINT_16 u2Offset = 0;
    BOOLEAN fgReplyProbeResp = FALSE;


    ASSERT(prSwRfb);
    ASSERT(pu4ControlFlags);

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    //4 <1> Parse Probe Req IE and Get IE ptr (SSID, Supported Rate IE, ...)
    //prMgtHdr = (P_WLAN_MAC_MGMT_HEADER_T)prSwRfb->pvHeader;

    u2IELength = prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen;
    pucIE = (PUINT_8)((UINT_32)prSwRfb->pvHeader + prSwRfb->u2HeaderLen);

    IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
        if (ELEM_ID_SSID == IE_ID(pucIE)) {
            if ((!prIeSsid) &&
                    (IE_LEN(pucIE) <= ELEM_MAX_LEN_SSID)) {
                prIeSsid = (P_IE_SSID_T)pucIE;
            }
            break;
        }
    } /* end of IE_FOR_EACH */

    //4 <2> Check network conditions

    if (prBssInfo->eCurrentOPMode == OP_MODE_IBSS) {

        if ((prIeSsid) &&
                ((prIeSsid->ucLength == BC_SSID_LEN) || /* WILDCARD SSID */
                 EQUAL_SSID((void *)prBssInfo->aucSSID, prBssInfo->ucSSIDLen, /* CURRENT SSID */
                            (void *)prIeSsid->aucSSID, prIeSsid->ucLength))) {
            fgReplyProbeResp = TRUE;
        }
    }

    return fgReplyProbeResp;

} /* end of aisValidateProbeReq() */

#endif /* CFG_SUPPORT_ADHOC */

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will modify and update necessary information to firmware
*        for disconnection handling
*
* @param[in] prAdapter          Pointer to the Adapter structure.
*
* @retval None
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmDisconnect(
    IN P_ADAPTER_T prAdapter,
    IN BOOLEAN     fgDelayIndication
)
{
    P_BSS_INFO_T prAisBssInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DISCONN, "aisFsmDisconnect(),fgDelayIndication =%d", fgDelayIndication);

    ASSERT(prAdapter);

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    nicPmIndicateBssAbort(prAdapter, NETWORK_TYPE_AIS_INDEX);

#if CFG_SUPPORT_ADHOC
    if (prAisBssInfo->fgIsBeaconActivated) {
        nicUpdateBeaconIETemplate(prAdapter,
                                  IE_UPD_METHOD_DELETE_ALL,
                                  NETWORK_TYPE_AIS_INDEX,
                                  0,
                                  NULL,
                                  0);

        prAisBssInfo->fgIsBeaconActivated = FALSE;
    }
#endif

    rlmBssAborted(prAdapter, prAisBssInfo);

    //4 <3> Unset the fgIsConnected flag of BSS_DESC_T and send Deauth if needed.
    if (PARAM_MEDIA_STATE_CONNECTED == prAisBssInfo->eConnectionState) {

        if (prAisBssInfo->ucReasonOfDisconnect == DISCONNECT_REASON_CODE_RADIO_LOST) {
            scanRemoveBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);

            /* remove from scanning results as well */
            wlanClearBssInScanningResult(prAdapter, prAisBssInfo->aucBSSID);
        } else {
            scanRemoveConnFlagOfBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
        }

        if (fgDelayIndication) {
            if (OP_MODE_IBSS != prAisBssInfo->eCurrentOPMode) {
                prAisBssInfo->fgHoldSameBssidForIBSS = FALSE;
            } else {
                /* NOTE(Nelson): Do not retry to connect to IBSS network if disconnected  */
                prAdapter->rWifiVar.rConnSettings.fgIsConnReqIssued = FALSE;
            }
        } else {
            prAisBssInfo->fgHoldSameBssidForIBSS = FALSE;
        }
    } else {
        prAisBssInfo->fgHoldSameBssidForIBSS = FALSE;
    }


    //4 <4> Change Media State immediately.
    aisChangeMediaState(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED);

    //4 <4.1> sync. with firmware
    nicUpdateBss(prAdapter, NETWORK_TYPE_AIS_INDEX);

    if (!fgDelayIndication) {
        //4 <5> Deactivate previous AP's STA_RECORD_T or all Clients in Driver if have.
        if (prAisBssInfo->prStaRecOfAP) {
            //cnmStaRecChangeState(prAdapter, prAisBssInfo->prStaRecOfAP, STA_STATE_1);

            prAisBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;
        }
    }

#if CFG_SUPPORT_ROAMING
    roamingFsmRunEventAbort(prAdapter);

    /* clear pending roaming connection request */
    aisFsmIsRequestPending(prAdapter, AIS_REQUEST_ROAMING_SEARCH, TRUE);
    aisFsmIsRequestPending(prAdapter, AIS_REQUEST_ROAMING_CONNECT, TRUE);
#endif /* CFG_SUPPORT_ROAMING */

    //4 <6> Indicate Disconnected Event to Host
    aisIndicationOfMediaStateToHost(prAdapter,
                                    PARAM_MEDIA_STATE_DISCONNECTED,
                                    fgDelayIndication);


    //4 <7> Trigger AIS FSM
    aisFsmSteps(prAdapter, AIS_STATE_IDLE);

    return;
} /* end of aisFsmDisconnect() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indicate an Event of "Background Scan Time-Out" to AIS FSM.
*
* @param[in] u4Param  Unused timer parameter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventBGSleepTimeOut(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;

    DEBUGFUNC("aisFsmRunEventBGSleepTimeOut()");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    eNextState = prAisFsmInfo->eCurrentState;

    switch (prAisFsmInfo->eCurrentState) {
        case AIS_STATE_WAIT_FOR_NEXT_SCAN:
            DBGLOG(AIS, LOUD, ("EVENT - SCAN TIMER: Idle End - Current Time = %ld\n", kalGetTimeTick()));

            eNextState = AIS_STATE_LOOKING_FOR;

            SET_NET_PWR_STATE_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX);

            break;

        default:
            break;
    }

    /* Call aisFsmSteps() when we are going to change AIS STATE */
    if (eNextState != prAisFsmInfo->eCurrentState) {
        aisFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of aisFsmBGSleepTimeout() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indicate an Event of "IBSS ALONE Time-Out" to AIS FSM.
*
* @param[in] u4Param  Unused timer parameter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventIbssAloneTimeOut(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;

    DEBUGFUNC("aisFsmRunEventIbssAloneTimeOut()");

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    eNextState = prAisFsmInfo->eCurrentState;

    switch (prAisFsmInfo->eCurrentState) {
        case AIS_STATE_IBSS_ALONE:

            /* There is no one participate in our AdHoc during this TIMEOUT Interval
             * so go back to search for a valid IBSS again.
             */

            DBGLOG(AIS, LOUD, ("EVENT-IBSS ALONE TIMER: Start pairing\n"));

            prAisFsmInfo->fgTryScan = TRUE;

            /* abort timer */
            aisFsmReleaseCh(prAdapter);

            /* Pull back to SEARCH to find candidate again */
            eNextState = AIS_STATE_SEARCH;

            break;

        default:
            break;
    }


    /* Call aisFsmSteps() when we are going to change AIS STATE */
    if (eNextState != prAisFsmInfo->eCurrentState) {
        aisFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of aisIbssAloneTimeOut() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indicate an Event of "Join Time-Out" to AIS FSM.
*
* @param[in] u4Param  Unused timer parameter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventJoinTimeout(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4Param
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    ENUM_AIS_STATE_T eNextState;


    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    eNextState = prAisFsmInfo->eCurrentState;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "aisFsmRunEventJoinTimeout(),STATE=%d", prAisFsmInfo->eCurrentState);

    switch (prAisFsmInfo->eCurrentState) {
        case AIS_STATE_JOIN:

            /* 1. Do abort JOIN */
            aisFsmStateAbort_JOIN(prAdapter);

            /* 2. Increase Join Failure Count */
            prAisFsmInfo->prTargetStaRec->ucJoinFailureCount++;

            if (prAisFsmInfo->prTargetStaRec->ucJoinFailureCount < JOIN_MAX_RETRY_FAILURE_COUNT) {
                /* 3.1 Retreat to AIS_STATE_SEARCH state for next try */
                eNextState = AIS_STATE_SEARCH;
            } else {
                /* 3.2 Retreat to AIS_STATE_WAIT_FOR_NEXT_SCAN state for next try */
                eNextState = AIS_STATE_WAIT_FOR_NEXT_SCAN;
            }
            break;

        case AIS_STATE_NORMAL_TR:
            /* 1. release channel */
            aisFsmReleaseCh(prAdapter);
            prAisFsmInfo->fgIsInfraChannelFinished = TRUE;

            /* 2. process if there is pending scan */
            if (aisFsmIsRequestPending(prAdapter, AIS_REQUEST_SCAN, TRUE) == TRUE) {
                wlanClearScanningResult(prAdapter);
                eNextState = AIS_STATE_ONLINE_SCAN;
            }

            break;

        default:
            /* release channel */
            aisFsmReleaseCh(prAdapter);
            break;

    }


    /* Call aisFsmSteps() when we are going to change AIS STATE */
    if (eNextState != prAisFsmInfo->eCurrentState) {
        aisFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of aisFsmRunEventJoinTimeout() */


#if defined(CFG_TEST_MGMT_FSM) && (CFG_TEST_MGMT_FSM != 0)
/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisTest(
    VOID
)
{
    P_MSG_AIS_ABORT_T prAisAbortMsg;
    P_CONNECTION_SETTINGS_T prConnSettings;
    UINT_8 aucSSID[] = "pci-11n";
    UINT_8 ucSSIDLen = 7;

    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    /* Set Connection Request Issued Flag */
    prConnSettings->fgIsConnReqIssued = TRUE;
    prConnSettings->ucSSIDLen = ucSSIDLen;
    kalMemCopy(prConnSettings->aucSSID, aucSSID, ucSSIDLen);

    prAisAbortMsg = (P_MSG_AIS_ABORT_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_AIS_ABORT_T));
    if (!prAisAbortMsg) {

        ASSERT(0); // Can't trigger SCAN FSM
        return;
    }

    prAisAbortMsg->rMsgHdr.eMsgId = MID_HEM_AIS_FSM_ABORT;

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prAisAbortMsg,
                MSG_SEND_METHOD_BUF);

    wifi_send_msg(INDX_WIFI, MSG_ID_WIFI_IST, 0);

    return;
}
#endif /* CFG_TEST_MGMT_FSM */


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is used to handle OID_802_11_BSSID_LIST_SCAN
*
* \param[in] prAdapter  Pointer of ADAPTER_T
* \param[in] prAdapter  Pointer of SSID_T if specified
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmScanRequest(
    IN P_ADAPTER_T prAdapter,
    IN P_PARAM_SSID_T prSsid
)
{
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_INFO_T prAisBssInfo;
    P_AIS_FSM_INFO_T prAisFsmInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_SCAN, "aisFsmScanRequest()");

    ASSERT(prAdapter);

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    if (!prConnSettings->fgIsScanReqIssued) {
        prConnSettings->fgIsScanReqIssued = TRUE;

        if (prSsid == NULL) {
            prAisFsmInfo->ucScanSSIDLen = 0;
        } else {
            COPY_SSID(prAisFsmInfo->aucScanSSID,
                      prAisFsmInfo->ucScanSSIDLen,
                      prSsid->aucSsid,
                      (UINT_8)prSsid->u4SsidLen);
        }

        if (prAisFsmInfo->eCurrentState == AIS_STATE_NORMAL_TR) {
            if (prAisBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE
                    && prAisFsmInfo->fgIsInfraChannelFinished == FALSE) {
                // 802.1x might not finished yet, pend it for later handling ..
                MT5931_TRACE(TRACE_SCAN, MT5931_INFO_23, "prAisFsmInfo->eCurrentState[%d] prAisFsmInfo->fgIsInfraChannelFinished = %d",
                             prAisFsmInfo->eCurrentState, prAisFsmInfo->fgIsInfraChannelFinished);
                aisFsmInsertRequest(prAdapter, AIS_REQUEST_SCAN);
            } else {
                if (prAisFsmInfo->fgIsChannelGranted == TRUE) {
                    MT5931_TRACE(TRACE_WARNING, MT5931_INFO_24, "Scan Request with channel granted for join operation: %d, %d",
                                 prAisFsmInfo->fgIsChannelGranted,
                                 prAisFsmInfo->fgIsChannelRequested);
                }

                /* start online scan */
                wlanClearScanningResult(prAdapter);
                aisFsmSteps(prAdapter, AIS_STATE_ONLINE_SCAN);
            }
        } else if (prAisFsmInfo->eCurrentState == AIS_STATE_IDLE) {
            wlanClearScanningResult(prAdapter);
            aisFsmSteps(prAdapter, AIS_STATE_SCAN);
        } else {
            MT5931_TRACE(TRACE_SCAN, MT5931_INFO_25, "aisFsmScanRequest: prAisFsmInfo->eCurrentState[%d]", prAisFsmInfo->eCurrentState);
            aisFsmInsertRequest(prAdapter, AIS_REQUEST_SCAN);
        }
    } else {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_26, "prConnSettings->fgIsScanReqIssued %d", prConnSettings->fgIsScanReqIssued);
    }

    return;
} /* end of aisFsmScanRequest() */


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is invoked when CNM granted channel privilege
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventChGrant(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    //P_BSS_INFO_T prAisBssInfo;
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_MSG_CH_GRANT_T prMsgChGrant;
    UINT_8 ucTokenID;
    UINT_32 u4GrantInterval;

    ASSERT(prAdapter);
    ASSERT(prMsgHdr);
    //prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prMsgChGrant = (P_MSG_CH_GRANT_T)prMsgHdr;

    ucTokenID = prMsgChGrant->ucTokenID;
    u4GrantInterval = prMsgChGrant->u4GrantInterval;

    /* 1. free message */
    cnmMemFree(prAdapter, prMsgHdr);

    if (prAisFsmInfo->eCurrentState == AIS_STATE_REQ_CHANNEL_JOIN &&
            prAisFsmInfo->ucSeqNumOfChReq == ucTokenID) {
        /* 2. channel privilege has been approved */
        prAisFsmInfo->u4ChGrantedInterval = u4GrantInterval;

        /* 3. state transition to join/ibss-alone/ibss-merge */
        /* 3.1 set timeout timer in cases join could not be completed */
        cnmTimerStartTimer(prAdapter,
                           &prAisFsmInfo->rJoinTimeoutTimer,
                           prAisFsmInfo->u4ChGrantedInterval - AIS_JOIN_CH_GRANT_THRESHOLD);
        /* 3.2 set local variable to indicate join timer is ticking */
        prAisFsmInfo->fgIsInfraChannelFinished = FALSE;

        /* 3.3 switch to join state */
        aisFsmSteps(prAdapter, AIS_STATE_JOIN);

        prAisFsmInfo->fgIsChannelGranted = TRUE;
    } else { /* mismatched grant */
        /* 2. return channel privilege to CNM immediately */
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_27, "mismatched grant: prAisFsmInfo->ucSeqNumOfChReq %d ucTokenID %d",
                     prAisFsmInfo->ucSeqNumOfChReq, ucTokenID);
        aisFsmReleaseCh(prAdapter);
    }

    return;
} /* end of aisFsmRunEventChGrant() */


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is to inform CNM that channel privilege
*           has been released
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmReleaseCh(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_MSG_CH_ABORT_T prMsgChAbort;

    ASSERT(prAdapter);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    if (prAisFsmInfo->fgIsChannelGranted == TRUE
            || prAisFsmInfo->fgIsChannelRequested == TRUE) {

        prAisFsmInfo->fgIsChannelRequested = FALSE;
        prAisFsmInfo->fgIsChannelGranted = FALSE;

        /* 1. return channel privilege to CNM immediately */
        prMsgChAbort = (P_MSG_CH_ABORT_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_CH_ABORT_T));
        if (!prMsgChAbort) {
            ASSERT(0); // Can't release Channel to CNM
            return;
        }

        prMsgChAbort->rMsgHdr.eMsgId  = MID_MNY_CNM_CH_ABORT;
        prMsgChAbort->ucNetTypeIndex  = NETWORK_TYPE_AIS_INDEX;
        prMsgChAbort->ucTokenID       = prAisFsmInfo->ucSeqNumOfChReq;

        mboxSendMsg(prAdapter,
                    MBOX_ID_0,
                    (P_MSG_HDR_T) prMsgChAbort,
                    MSG_SEND_METHOD_BUF);
    }

    return;
} /* end of aisFsmReleaseCh() */


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is to inform AIS that corresponding beacon has not
*           been received for a while and probing is not successful
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
aisBssBeaconTimeout(
    IN P_ADAPTER_T prAdapter
)
{
    P_BSS_INFO_T prAisBssInfo;
    BOOLEAN fgDoAbortIndication = FALSE;
    //UINT_32 current_time = kalGetTimeTick();

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DISCONN, "aisBssBeaconTimeout()");

    ASSERT(prAdapter);

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    //4 <1> Diagnose Connection for Beacon Timeout Event
    if (PARAM_MEDIA_STATE_CONNECTED == prAisBssInfo->eConnectionState) {
        if (OP_MODE_INFRASTRUCTURE == prAisBssInfo->eCurrentOPMode) {
            P_STA_RECORD_T prStaRec = prAisBssInfo->prStaRecOfAP;

            if (prStaRec) {
                fgDoAbortIndication = TRUE;
            }

#if 0   /* Remove it for the AP has power off but handset still show connected issue */
            /* NOTE(Nelson): workaround for Wi-Fi diconnection by beacon timeout while SCO coext start */
            if ((current_time - prAdapter->u4RxDataResvTime) < MSEC_TO_SYSTIME(AIS_BEACON_TIMEOUT_CONFIRM_MSEC)) {
                MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "(current_time - prAdapter->u4RxDataResvTime) < MSEC_TO_SYSTIME(AIS_BEACON_TIMEOUT_CONFIRM_MSEC)");
                fgDoAbortIndication = FALSE;
            }
            /* NOTE(Nelson): workaround for Wi-Fi diconnection by beacon timeout while SCO coext end */
#endif
            if (BT_STATE_Report2WLAN & BT_STATE_INQUIRY) {
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_28, "BT_STATE_Report2WLAN=0x%x", BT_STATE_Report2WLAN);
                fgDoAbortIndication = FALSE;

                kal_event_start_timer(&prAdapter->prGlueInfo->prParamWndrvEs[PARAM_WNDRV_ES_BEACON_TIMR],
                                      AIS_BEACON_TIMEOUT_POLL_SEC * KAL_TICKS_1_SEC);
            }
        } else if (OP_MODE_IBSS == prAisBssInfo->eCurrentOPMode) {
            fgDoAbortIndication = TRUE;
        }
    }

    //4 <2> invoke abort handler
    if (fgDoAbortIndication) {
        aisFsmStateAbort(prAdapter, DISCONNECT_REASON_CODE_RADIO_LOST, TRUE);
    }

    return;
} /* end of aisBssBeaconTimeout() */


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is to inform AIS that DEAUTH frame has been
*           sent and thus state machine could go ahead
*
* \param[in] prAdapter  Pointer of ADAPTER_T
* \param[in] prMsduInfo Pointer of MSDU_INFO_T for DEAUTH frame
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
aisDeauthXmitComplete(
    IN P_ADAPTER_T              prAdapter,
    IN P_MSDU_INFO_T            prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T    rTxDoneStatus
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DISCONN, "aisDeauthXmitComplete:TX Done \n");

    ASSERT(prAdapter);

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    if (prAisFsmInfo->eCurrentState == AIS_STATE_DISCONNECTING) {
        if (rTxDoneStatus != TX_RESULT_DROPPED_IN_DRIVER) {
            aisFsmStateAbort(prAdapter, DISCONNECT_REASON_CODE_NEW_CONNECTION, FALSE);
        } else {
            MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_29, "rTxDoneStatus == TX_RESULT_DROPPED_IN_DRIVER");
        }
    } else {
        //DBGLOG(AIS, WARN, ("DEAUTH frame transmitted without further handling"));
        MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_30, "DEAUTH frame transmitted without further handling");
    }

    return WLAN_STATUS_SUCCESS;

} /* end of aisDeauthXmitComplete() */

#if CFG_SUPPORT_ROAMING
/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indicate an Event of "Looking for a candidate due to weak signal" to AIS FSM.
*
* @param[in] u4ReqScan  Requesting Scan or not
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRunEventRoamingDiscovery(
    IN P_ADAPTER_T prAdapter,
    UINT_32 u4ReqScan
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;
    ENUM_AIS_REQUEST_TYPE_T eAisRequest;

    DBGLOG(AIS, LOUD, ("aisFsmRunEventRoamingDiscovery()\n"));

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    /* search candidates by best rssi */
    prConnSettings->eConnectionPolicy = CONNECT_BY_SSID_BEST_RSSI;

    /* results are still new */
    if (!u4ReqScan) {
        roamingFsmRunEventRoam(prAdapter);
        eAisRequest = AIS_REQUEST_ROAMING_CONNECT;
    } else {
        if (prAisFsmInfo->eCurrentState == AIS_STATE_ONLINE_SCAN
                || prAisFsmInfo->eCurrentState == AIS_STATE_LOOKING_FOR) {
            eAisRequest = AIS_REQUEST_ROAMING_CONNECT;
        } else {
            eAisRequest = AIS_REQUEST_ROAMING_SEARCH;
        }
    }

    if (prAisFsmInfo->eCurrentState == AIS_STATE_NORMAL_TR && prAisFsmInfo->fgIsInfraChannelFinished == TRUE) {
        if (eAisRequest == AIS_REQUEST_ROAMING_SEARCH) {
            aisFsmSteps(prAdapter, AIS_STATE_LOOKING_FOR);
        } else {
            aisFsmSteps(prAdapter, AIS_STATE_SEARCH);
        }
    } else {
        aisFsmIsRequestPending(prAdapter, AIS_REQUEST_ROAMING_SEARCH, TRUE);
        aisFsmIsRequestPending(prAdapter, AIS_REQUEST_ROAMING_CONNECT, TRUE);

        aisFsmInsertRequest(prAdapter, eAisRequest);
    }

    return;
} /* end of aisFsmRunEventRoamingDiscovery() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Update the time of ScanDone for roaming and transit to Roam state.
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
ENUM_AIS_STATE_T
aisFsmRoamingScanResultsUpdate(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_ROAMING_INFO_T prRoamingFsmInfo;
    ENUM_AIS_STATE_T eNextState;

    DBGLOG(AIS, LOUD, ("->aisFsmRoamingScanResultsUpdate()\n"));

    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);
    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    roamingFsmScanResultsUpdate(prAdapter);

    eNextState = prAisFsmInfo->eCurrentState;
    if (prRoamingFsmInfo->eCurrentState == ROAMING_STATE_DISCOVERY) {
        roamingFsmRunEventRoam(prAdapter);
        eNextState = AIS_STATE_SEARCH;
    } else if (prAisFsmInfo->eCurrentState == AIS_STATE_LOOKING_FOR) {
        eNextState = AIS_STATE_SEARCH;
    } else if (prAisFsmInfo->eCurrentState == AIS_STATE_ONLINE_SCAN) {
        eNextState = AIS_STATE_NORMAL_TR;
    }

    return eNextState;
} /* end of aisFsmRoamingScanResultsUpdate() */

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will modify and update necessary information to firmware
*        for disconnection of last AP before switching to roaming bss.
*
* @param IN prAdapter          Pointer to the Adapter structure.
*           prTargetStaRec     Target of StaRec of roaming
*
* @retval None
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmRoamingDisconnectPrevAP(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prTargetStaRec
)
{
    P_BSS_INFO_T prAisBssInfo;

    DEBUGFUNC("aisFsmRoamingDisconnectPrevAP()");

    ASSERT(prAdapter);

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    nicPmIndicateBssAbort(prAdapter, NETWORK_TYPE_AIS_INDEX);

    /* Not invoke rlmBssAborted() here to avoid prAisBssInfo->fg40mBwAllowed
     * to be reset. RLM related parameters will be reset again when handling
     * association response in rlmProcessAssocRsp(). 20110413
     */
    //rlmBssAborted(prAdapter, prAisBssInfo);

    //4 <3> Unset the fgIsConnected flag of BSS_DESC_T and send Deauth if needed.
    if (PARAM_MEDIA_STATE_CONNECTED == prAisBssInfo->eConnectionState) {
        scanRemoveConnFlagOfBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
    }

    //4 <4> Change Media State immediately.
    aisChangeMediaState(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED);

    //4 <4.1> sync. with firmware
    prTargetStaRec->ucNetTypeIndex = 0xff; /* Virtial NetType */
    nicUpdateBss(prAdapter, NETWORK_TYPE_AIS_INDEX);
    prTargetStaRec->ucNetTypeIndex = NETWORK_TYPE_AIS_INDEX; /* Virtial NetType */

    return;
} /* end of aisFsmRoamingDisconnectPrevAP() */

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will update the contain of BSS_INFO_T for AIS network once
*        the roaming was completed.
*
* @param IN prAdapter          Pointer to the Adapter structure.
*           prStaRec           StaRec of roaming AP
*           prAssocRspSwRfb
*
* @retval None
*/
/*----------------------------------------------------------------------------*/
VOID
aisUpdateBssInfoForRoamingAP(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN P_SW_RFB_T prAssocRspSwRfb
)
{
    P_BSS_INFO_T prAisBssInfo;

    DEBUGFUNC("aisUpdateBssInfoForRoamingAP()");

    ASSERT(prAdapter);

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    //4 <1.1> Change FW's Media State immediately.
    aisChangeMediaState(prAdapter, PARAM_MEDIA_STATE_CONNECTED);

    //4 <1.2> Deactivate previous AP's STA_RECORD_T in Driver if have.
    if ((prAisBssInfo->prStaRecOfAP) &&
            (prAisBssInfo->prStaRecOfAP != prStaRec) &&
            (prAisBssInfo->prStaRecOfAP->fgIsInUse)) {
        cnmStaRecChangeState(prAdapter, prAisBssInfo->prStaRecOfAP, STA_STATE_1);
    }

    //4 <1.3> Update BSS_INFO_T
    aisUpdateBssInfoForJOIN(prAdapter, prStaRec, prAssocRspSwRfb);

    //4 <1.4> Activate current AP's STA_RECORD_T in Driver.
    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);

    //4 <1.6> Indicate Connected Event to Host immediately.
    /* Require BSSID, Association ID, Beacon Interval.. from AIS_BSS_INFO_T */
    aisIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_CONNECTED, FALSE);

    return;
} /* end of aisFsmRoamingUpdateBss() */

#endif /* CFG_SUPPORT_ROAMING */


/*----------------------------------------------------------------------------*/
/*!
* @brief Check if there is any pending request and remove it (optional)
*
* @param prAdapter
*        eReqType
*        bRemove
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
aisFsmIsRequestPending(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_AIS_REQUEST_TYPE_T eReqType,
    IN BOOLEAN bRemove
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_AIS_REQ_HDR_T prPendingReqHdr, prPendingReqHdrNext;
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "aisFsmIsRequestPending");
    ASSERT(prAdapter);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    /* traverse through pending request list */
    LINK_FOR_EACH_ENTRY_SAFE(prPendingReqHdr,
                             prPendingReqHdrNext,
                             &(prAisFsmInfo->rPendingReqList),
                             rLinkEntry,
                             AIS_REQ_HDR_T) {
        /* check for specified type */
        if (prPendingReqHdr->eReqType == eReqType) {
            /* check if need to remove */
            if (bRemove == TRUE) {
                LINK_REMOVE_KNOWN_ENTRY(&(prAisFsmInfo->rPendingReqList), &(prPendingReqHdr->rLinkEntry));

                cnmMemFree(prAdapter, prPendingReqHdr);
            }

            return TRUE;
        }
    }

    return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Get next pending request
*
* @param prAdapter
*
* @return P_AIS_REQ_HDR_T
*/
/*----------------------------------------------------------------------------*/
P_AIS_REQ_HDR_T
aisFsmGetNextRequest(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_FSM_INFO_T prAisFsmInfo;
    P_AIS_REQ_HDR_T prPendingReqHdr;

    ASSERT(prAdapter);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    LINK_REMOVE_HEAD(&(prAisFsmInfo->rPendingReqList), prPendingReqHdr, P_AIS_REQ_HDR_T);

    return prPendingReqHdr;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Insert a new request
*
* @param prAdapter
*        eReqType
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
aisFsmInsertRequest(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_AIS_REQUEST_TYPE_T eReqType
)
{
    P_AIS_REQ_HDR_T prAisReq;
    P_AIS_FSM_INFO_T prAisFsmInfo;

    DEBUGFUNC("aisFsmInsertRequest");
    ASSERT(prAdapter);
    prAisFsmInfo = &(prAdapter->rWifiVar.rAisFsmInfo);

    prAisReq = (P_AIS_REQ_HDR_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(AIS_REQ_HDR_T));

    if (!prAisReq) {
        ASSERT(0); // Can't generate new message
        return FALSE;
    }

    prAisReq->eReqType = eReqType;

    /* attach request into pending request list */
    LINK_INSERT_TAIL(&prAisFsmInfo->rPendingReqList, &prAisReq->rLinkEntry);

    return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Flush all pending requests
*
* @param prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
aisFsmFlushRequest(
    IN P_ADAPTER_T prAdapter
)
{
    P_AIS_REQ_HDR_T prAisReq;

    ASSERT(prAdapter);

    while ((prAisReq = aisFsmGetNextRequest(prAdapter)) != NULL) {
        cnmMemFree(prAdapter, prAisReq);
    }

    return;
}


