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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/nic/adapter.h#60 $
*/

/*! \file   adapter.h
    \brief  Definition of internal data structure for driver manipulation.

    In this file we define the internal data structure - ADAPTER_T which stands
    for MiniPort ADAPTER(From Windows point of view) or stands for Network ADAPTER.
*/




#ifndef _ADAPTER_H
#define _ADAPTER_H

#include "wndrv_abm_msg.h"

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

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef struct _ENHANCE_MODE_DATA_STRUCT_T SDIO_CTRL_T, *P_SDIO_CTRL_T;

typedef struct _WLAN_INFO_T {
    PARAM_BSSID_EX_T                rCurrBssId;

    // Scan Result
    PARAM_BSSID_EX_T                arScanResult[CFG_MAX_NUM_BSS_LIST];
    PUINT_8                         apucScanResultIEs[CFG_MAX_NUM_BSS_LIST];
    UINT_32                         u4ScanResultNum;

    // IE pool for Scanning Result
    UINT_8                          aucScanIEBuf[CFG_MAX_COMMON_IE_BUF_LEN];
    UINT_32                         u4ScanIEBufferUsage;

    OS_SYSTIME                      u4SysTime;

    // connection parameter (for Ad-Hoc)
    UINT_16                     u2BeaconPeriod;
    UINT_16                     u2AtimWindow;

    PARAM_RATES                 eDesiredRates;
    CMD_LINK_ATTRIB             eLinkAttr;
//    CMD_PS_PROFILE_T         ePowerSaveMode;
    CMD_PS_PROFILE_T         arPowerSaveMode[NETWORK_TYPE_INDEX_NUM];

    // trigger parameter
    ENUM_RSSI_TRIGGER_TYPE      eRssiTriggerType;
    PARAM_RSSI                  rRssiTriggerValue;

    // Privacy Filter
    ENUM_PARAM_PRIVACY_FILTER_T ePrivacyFilter;

    // RTS Threshold
    PARAM_RTS_THRESHOLD         eRtsThreshold;

    // Network Type
    UINT_8                      ucNetworkType;

    // Network Type In Use
    UINT_8                      ucNetworkTypeInUse;

} WLAN_INFO_T, *P_WLAN_INFO_T;

/* Session for CONNECTION SETTINGS */
typedef struct _CONNECTION_SETTINGS_T {

    UINT_8                          aucMacAddress[MAC_ADDR_LEN];

    UINT_8                          ucDelayTimeOfDisconnectEvent;

    BOOLEAN                         fgIsConnByBssidIssued;
    UINT_8                          aucBSSID[MAC_ADDR_LEN];

    BOOLEAN                         fgIsConnReqIssued;
    UINT_8                          ucSSIDLen;
    UINT_8                          aucSSID[ELEM_MAX_LEN_SSID];

    ENUM_PARAM_OP_MODE_T            eOPMode;

    ENUM_PARAM_CONNECTION_POLICY_T  eConnectionPolicy;

    ENUM_PARAM_AD_HOC_MODE_T        eAdHocMode;

    ENUM_PARAM_AUTH_MODE_T          eAuthMode;

    ENUM_PARAM_ENCRYPTION_STATUS_T  eEncStatus;

    BOOLEAN                         fgIsScanReqIssued;


    /* MIB attributes */
    UINT_16                         u2BeaconPeriod;

    UINT_16                         u2RTSThreshold; /* User desired setting */

    UINT_16                         u2DesiredNonHTRateSet; /* User desired setting */

    UINT_8                          ucAdHocChannelNum; /* For AdHoc */

    ENUM_BAND_T                     eAdHocBand; /* For AdHoc */

    UINT_32                         u4FreqInKHz; /* Center frequency */

    /* ATIM windows using for IBSS power saving function */
    UINT_16                         u2AtimWindow;

    /* Features */
    BOOLEAN                         fgIsEnableRoaming;

    BOOLEAN                         fgIsAdHocQoSEnable;

    ENUM_PARAM_PHY_CONFIG_T         eDesiredPhyConfig;

    /* Used for AP mode for desired channel and bandwidth */
    UINT_16                         u2CountryCode;
    UINT_8                          uc2G4BandwidthMode; /* 20/40M or 20M only */
    UINT_8                          uc5GBandwidthMode;  /* 20/40M or 20M only */

    BOOLEAN                         fgTxShortGIDisabled;
    BOOLEAN                         fgRxShortGIDisabled;

#if CFG_SUPPORT_802_11D
    BOOLEAN                         fgMultiDomainCapabilityEnabled;
#endif /* CFG_SUPPORT_802_11D*/


#if 1 //CFG_SUPPORT_WAPI
    BOOL                            fgWapiMode;
    UINT_32                         u4WapiSelectedGroupCipher;
    UINT_32                         u4WapiSelectedPairwiseCipher;
    UINT_32                         u4WapiSelectedAKMSuite;
#endif

    /* CR1486, CR1640 */
    /* for WPS, disable the privacy check for AP selection policy */
    BOOLEAN                         fgPrivacyCheckDisable;

    /* b0~3: trigger-en AC0~3. b4~7: delivery-en AC0~3 */
    UINT_8                          bmfgApsdEnAc;

} CONNECTION_SETTINGS_T, *P_CONNECTION_SETTINGS_T;

struct _BSS_INFO_T {

    ENUM_PARAM_MEDIA_STATE_T  eConnectionState;           /* Connected Flag used in AIS_NORMAL_TR */
    ENUM_PARAM_MEDIA_STATE_T  eConnectionStateIndicated;  /* The Media State that report to HOST */

    ENUM_OP_MODE_T          eCurrentOPMode;             /* Current Operation Mode - Infra/IBSS */
#if CFG_ENABLE_WIFI_DIRECT
    ENUM_OP_MODE_T          eIntendOPMode;
#endif

    BOOLEAN                 fgIsNetActive;              /* TRUE if this network has been activated */

    UINT_8                  ucNetTypeIndex;             /* ENUM_NETWORK_TYPE_INDEX_T */

    UINT_8                  ucReasonOfDisconnect;       /* Used by media state indication */

    BOOLEAN                 ucSSIDLen;                  /* Length of SSID */
    UINT_8                  aucSSID[ELEM_MAX_LEN_SSID]; /* SSID used in this BSS */

    UINT_8                  aucBSSID[MAC_ADDR_LEN];     /* The BSSID of the associated BSS */

    UINT_8                  aucOwnMacAddr[MAC_ADDR_LEN];/* Owned MAC Address used in this BSS */

    P_STA_RECORD_T          prStaRecOfAP;               /* For Infra Mode, and valid only if
                                                         * eConnectionState == MEDIA_STATE_CONNECTED
                                                         */
    LINK_T                  rStaRecOfClientList;        /* For IBSS/AP Mode, all known STAs in current BSS */

    UINT_16                 u2CapInfo;                  /* Change Detection */

    UINT_16                 u2BeaconInterval;           /* The Beacon Interval of this BSS */


    UINT_16                 u2ATIMWindow;               /* For IBSS Mode */

    UINT_16                 u2AssocId;                  /* For Infra Mode, it is the Assoc ID assigned by AP.
                                                         */


    UINT_8                  ucDTIMPeriod;               /* For Infra/AP Mode */

    UINT_8                  ucDTIMCount;                /* For AP Mode, it is the DTIM value we should carried in
                                                         * the Beacon of next TBTT.
                                                         */

    UINT_8                  ucPhyTypeSet;               /* Available PHY Type Set of this peer
                                                         * (This is deduced from received BSS_DESC_T)
                                                         */

    UINT_8                  ucNonHTBasicPhyType;        /* The Basic PHY Type Index, used to setup Phy Capability */

    UINT_8                  ucConfigAdHocAPMode;        /* The configuration of AdHoc/AP Mode. e.g. 11g or 11b */

    UINT_8                  ucBeaconTimeoutCount;       /* For Infra/AP Mode, it is a threshold of Beacon Lost Count to
                                                           confirm connection was lost */

    BOOLEAN                 fgHoldSameBssidForIBSS;     /* For IBSS Mode, to keep use same BSSID to extend the life cycle of an IBSS */

    BOOLEAN                 fgIsBeaconActivated;        /* For AP/IBSS Mode, it is used to indicate that Beacon is sending */

    P_MSDU_INFO_T           prBeacon;                   /* For AP/IBSS Mode - Beacon Frame */

    BOOLEAN                 fgIsIBSSMaster;             /* For IBSS Mode - To indicate that we can reply ProbeResp Frame.
                                                           In current TBTT interval */

    BOOLEAN                 fgIsShortPreambleAllowed;   /* From Capability Info. of AssocResp Frame AND of Beacon/ProbeResp Frame */
    BOOLEAN                 fgUseShortPreamble;         /* Short Preamble is enabled in current BSS. */
    BOOLEAN                 fgUseShortSlotTime;         /* Short Slot Time is enabled in current BSS. */

    UINT_16                 u2OperationalRateSet;       /* Operational Rate Set of current BSS */
    UINT_16                 u2BSSBasicRateSet;          /* Basic Rate Set of current BSS */


    UINT_8                  ucAllSupportedRatesLen;     /* Used for composing Beacon Frame in AdHoc or AP Mode */
    UINT_8                  aucAllSupportedRates[RATE_NUM];

    UINT_8                  ucAssocClientCnt;           /* TODO(Kevin): Number of associated clients */
    UINT_8                  ucERP;

    BOOLEAN                 fgIsProtection;
    BOOLEAN                 fgIsQBSS; /* fgIsWmmBSS; */ /* For Infra/AP/IBSS Mode, it is used to indicate if we support WMM in
                                                         * current BSS. */
    BOOLEAN                 fgIsNetAbsent;              /* TRUE: BSS is absent, FALSE: BSS is present */

    UINT_32                 u4RsnSelectedGroupCipher;
    UINT_32                 u4RsnSelectedPairwiseCipher;
    UINT_32                 u4RsnSelectedAKMSuite;

    /*------------------------------------------------------------------------*/
    /* Power Management related information                                   */
    /*------------------------------------------------------------------------*/
    PM_PROFILE_SETUP_INFO_T rPmProfSetupInfo;


    /*------------------------------------------------------------------------*/
    /* WMM/QoS related information                                            */
    /*------------------------------------------------------------------------*/
    UINT_8                  ucWmmParamSetCount;         /* Used to detect the change of EDCA parameters. For AP mode, the value is used in WMM IE */

    AC_QUE_PARMS_T          arACQueParms[WMM_AC_INDEX_NUM];

    UINT_8                  aucCWminLog2ForBcast[WMM_AC_INDEX_NUM];        /* For AP mode, broadcast the CWminLog2 */
    UINT_8                  aucCWmaxLog2ForBcast[WMM_AC_INDEX_NUM];        /* For AP mode, broadcast the CWmaxLog2 */
    AC_QUE_PARMS_T          arACQueParmsForBcast[WMM_AC_INDEX_NUM];        /* For AP mode, broadcast the value */

    /*------------------------------------------------------------------------*/
    /* 802.11n HT operation IE when (prStaRec->ucPhyTypeSet & PHY_TYPE_BIT_HT)*/
    /* is true. They have the same definition with fields of                  */
    /* information element (CM)                                               */
    /*------------------------------------------------------------------------*/
    ENUM_BAND_T             eBand;
    UINT_8                  ucPrimaryChannel;
    UINT_8                  ucHtOpInfo1;
    UINT_16                 u2HtOpInfo2;
    UINT_16                 u2HtOpInfo3;

    /*------------------------------------------------------------------------*/
    /* Required protection modes (CM)                                         */
    /*------------------------------------------------------------------------*/
    BOOLEAN                 fgErpProtectMode;
    ENUM_HT_PROTECT_MODE_T  eHtProtectMode;
    ENUM_GF_MODE_T          eGfOperationMode;
    ENUM_RIFS_MODE_T        eRifsOperationMode;

    BOOLEAN                 fgObssErpProtectMode;       /* GO only */
    ENUM_HT_PROTECT_MODE_T  eObssHtProtectMode;         /* GO only */
    ENUM_GF_MODE_T          eObssGfOperationMode;       /* GO only */
    BOOLEAN                 fgObssRifsOperationMode;    /* GO only */

    /*------------------------------------------------------------------------*/
    /* OBSS to decide if 20/40M bandwidth is permitted.                       */
    /* The first member indicates the following channel list length.          */
    /*------------------------------------------------------------------------*/
    BOOLEAN                 fgAssoc40mBwAllowed;
    BOOLEAN                 fg40mBwAllowed;
    ENUM_CHNL_EXT_T         eBssSCO;    /* Real setting for HW
                                         * 20/40M AP mode will always set 40M,
                                         * but its OP IE can be changed.
                                         */
    UINT_8                  auc2G_20mReqChnlList[CHNL_LIST_SZ_2G + 1];
    UINT_8                  auc2G_NonHtChnlList[CHNL_LIST_SZ_2G + 1];
    UINT_8                  auc2G_PriChnlList[CHNL_LIST_SZ_2G + 1];
    UINT_8                  auc2G_SecChnlList[CHNL_LIST_SZ_2G + 1];

    UINT_8                  auc5G_20mReqChnlList[CHNL_LIST_SZ_5G + 1];
    UINT_8                  auc5G_NonHtChnlList[CHNL_LIST_SZ_5G + 1];
    UINT_8                  auc5G_PriChnlList[CHNL_LIST_SZ_5G + 1];
    UINT_8                  auc5G_SecChnlList[CHNL_LIST_SZ_5G + 1];

    TIMER_T                 rObssScanTimer;
    UINT_16                 u2ObssScanInterval;     /* in unit of sec */

    BOOLEAN                 fgObssActionForcedTo20M;    /* GO only */
    BOOLEAN                 fgObssBeaconForcedTo20M;    /* GO only */

    /*------------------------------------------------------------------------*/
    /* HW Related Fields (Kevin)                                              */
    /*------------------------------------------------------------------------*/
    UINT_8                  ucHwDefaultFixedRateCode;   /* The default rate code copied to MAC TX Desc */
    UINT_16                 u2HwLPWakeupGuardTimeUsec;


    UINT_8                  ucBssFreeQuota;              /* The value is updated from FW  */
};


struct _AIS_SPECIFIC_BSS_INFO_T {
    UINT_8                  ucRoamingAuthTypes;         /* This value indicate the roaming type used in AIS_JOIN */

    BOOLEAN                 fgIsIBSSActive;

    /*! \brief Global flag to let arbiter stay at standby and not connect to any network */
    BOOLEAN                 fgCounterMeasure;
    UINT_8                  ucWEPDefaultKeyID;
    BOOLEAN                 fgTransmitKeyExist; /* Legacy wep Transmit key exist or not */

    /* While Do CounterMeasure procedure, check the EAPoL Error report have send out */
    BOOLEAN                 fgCheckEAPoLTxDone;

    UINT_32                 u4RsnaLastMICFailTime;

    /* Stored the current bss wpa rsn cap filed, used for roaming policy */
    //UINT_16                 u2RsnCap;
    TIMER_T                 rPreauthenticationTimer;

    /* By the flow chart of 802.11i,
               wait 60 sec before associating to same AP
               or roaming to a new AP
               or sending data in IBSS,
               keep a timer for handle the 60 sec counterMeasure */
    TIMER_T                 rRsnaBlockTrafficTimer;
    TIMER_T                 rRsnaEAPoLReportTimeoutTimer;

    /* For Keep the Tx/Rx Mic key for TKIP SW Calculate Mic */
    /* This is only one for AIS/AP */
    UINT_8                  aucTxMicKey[8];
    UINT_8                  aucRxMicKey[8];

    /* Buffer for WPA2 PMKID */
    /* The PMKID cache lifetime is expire by media_disconnect_indication */
    UINT_32                 u4PmkidCandicateCount;
    PMKID_CANDICATE_T       arPmkidCandicate[CFG_MAX_PMKID_CACHE];
    UINT_32                 u4PmkidCacheCount;
    PMKID_ENTRY_T           arPmkidCache[CFG_MAX_PMKID_CACHE];
    BOOLEAN                 fgIndicatePMKID;

};

struct _BOW_SPECIFIC_BSS_INFO_T {
    UINT_16                 u2Reserved; /* Reserved for Data Type Check */
};

#if CFG_SLT_SUPPORT
typedef struct _SLT_INFO_T {

    P_BSS_DESC_T prPseudoBssDesc;
    UINT_16 u2SiteID;
    UINT_8 ucChannel2G4;
    UINT_8 ucChannel5G;
    BOOLEAN fgIsDUT;
    UINT_32 u4BeaconReceiveCnt;
    /////////Deprecated/////////
    P_STA_RECORD_T prPseudoStaRec;
} SLT_INFO_T, *P_SLT_INFO_T;
#endif


/* Major member variables for WiFi FW operation.
   Variables within this region will be ready for access after WIFI function is enabled.
*/
typedef struct _WIFI_VAR_T {
    BOOLEAN                 fgIsRadioOff;

    BOOLEAN                 fgIsEnterD3ReqIssued;

    BOOLEAN                 fgDebugCmdResp;

    CONNECTION_SETTINGS_T   rConnSettings;

    SCAN_INFO_T             rScanInfo;

#if CFG_SUPPORT_ROAMING
    ROAMING_INFO_T          rRoamingInfo;
#endif /* CFG_SUPPORT_ROAMING */

    AIS_FSM_INFO_T          rAisFsmInfo;

    ENUM_PWR_STATE_T        aePwrState[NETWORK_TYPE_INDEX_NUM];

    BSS_INFO_T              arBssInfo[NETWORK_TYPE_INDEX_NUM];

    AIS_SPECIFIC_BSS_INFO_T rAisSpecificBssInfo;

#if CFG_ENABLE_WIFI_DIRECT
    P_P2P_CONNECTION_SETTINGS_T prP2PConnSettings;

    P_P2P_SPECIFIC_BSS_INFO_T prP2pSpecificBssInfo;

    P_P2P_FSM_INFO_T prP2pFsmInfo;
#endif /* CFG_ENABLE_WIFI_DIRECT */

#if CFG_ENABLE_BT_OVER_WIFI
    BOW_SPECIFIC_BSS_INFO_T rBowSpecificBssInfo;
    BOW_FSM_INFO_T rBowFsmInfo;
#endif /* CFG_ENABLE_BT_OVER_WIFI */

    DEAUTH_INFO_T           arDeauthInfo[MAX_DEAUTH_INFO_COUNT];

    /* Current Wi-Fi Settings and Flags */
    UINT_8                  aucPermanentAddress[MAC_ADDR_LEN];
    UINT_8                  aucMacAddress[MAC_ADDR_LEN];
    UINT_8                  aucDeviceAddress[MAC_ADDR_LEN];
    UINT_8                  aucInterfaceAddress[MAC_ADDR_LEN];

    UINT_8                  ucAvailablePhyTypeSet;

    ENUM_PHY_TYPE_INDEX_T   eNonHTBasicPhyType2G4; /* Basic Phy Type used by SCN according
                                                    * to the set of Available PHY Types
                                                    */

    ENUM_PARAM_PREAMBLE_TYPE_T  ePreambleType;
    ENUM_REGISTRY_FIXED_RATE_T  eRateSetting;

    BOOLEAN                 fgIsShortSlotTimeOptionEnable;
    /* User desired setting, but will honor the capability of AP */

    BOOLEAN                 fgEnableJoinToHiddenSSID;
    BOOLEAN                 fgSupportWZCDisassociation;

    BOOLEAN                 fgSupportQoS;
    BOOLEAN                 fgSupportAmpduTx;
    BOOLEAN                 fgSupportAmpduRx;
    BOOLEAN                 fgSupportTspec;
    BOOLEAN                 fgSupportUAPSD;
    BOOLEAN                 fgSupportULPSMP;

#if CFG_SLT_SUPPORT
    SLT_INFO_T      rSltInfo;
#endif

} WIFI_VAR_T, *P_WIFI_VAR_T;/* end of _WIFI_VAR_T */

/* cnm_timer module */
typedef struct {
    LINK_T          rLinkHead;
    OS_SYSTIME      rNextExpiredSysTime;
} ROOT_TIMER, *P_ROOT_TIMER;


/* FW/DRV/NVRAM version information */
typedef struct {

    /* NVRAM or Registry */
    UINT_16     u2Part1CfgOwnVersion;
    UINT_16     u2Part1CfgPeerVersion;
    UINT_16     u2Part2CfgOwnVersion;
    UINT_16     u2Part2CfgPeerVersion;

    /* Firmware */
    UINT_16     u2FwProductID;
    UINT_16     u2FwOwnVersion;
    UINT_16     u2FwPeerVersion;

} WIFI_VER_INFO_T, *P_WIFI_VER_INFO_T;


#if CFG_ENABLE_WIFI_DIRECT
/*
* p2p function pointer structure
*/

typedef struct _P2P_FUNCTION_LINKER {
    P2P_REMOVE                                  prP2pRemove;
    KAL_P2P_GET_CIPHER                          prKalP2pGetCipher;
    KAL_P2P_GET_WSC_MODE                        prKalP2pGetWscMode;
    KAL_P2P_GET_DEV_HDLR                        prKalP2pGetDevHdlr;
    NIC_P2P_MEDIA_STATE_CHANGE                  prNicP2pMediaStateChange;
    KAL_P2P_UPDATE_ASSOC_INFO                   prKalP2pUpdateAssocInfo;
    P2P_VALIDATE_AUTH                           prP2pValidateAuth;
    P2P_VALIDATE_ASSOC_REQ                      prP2pValidateAssocReq;
    P2P_RUN_EVENT_AAA_TX_FAIL                   prP2pRunEventAAATxFail;
    P2P_RUN_EVENT_AAA_COMPLETE                  prP2pRunEventAAAComplete;
    P2P_PARSE_CHECK_FOR_P2P_INFO_ELEM           prP2pParseCheckForP2pInfoElem;
    SCAN_UPDATE_P2P_DEVICE_DESC                 prScanUpdateP2pDeviceDesc;
    P2P_FSM_RUN_EVENT_RX_PROBE_RESPONSE_FRAME   prP2pFsmRunEventRxProbeResponseFrame;
    P2P_PROCESS_EVENT_UPDATE_NOA_PARAM          prP2pProcessEvent_UpdateNOAParam;
    SCAN_P2P_PROCESS_BEACON_AND_PROBE_RESP      prScanP2pProcessBeaconAndProbeResp;
    P2P_RX_PUBLIC_ACTION_FRAME                  prP2pRxPublicActionFrame;
    RLM_RSP_GENERATE_OBSS_SCAN_IE               prRlmRspGenerateObssScanIE;
    RLM_PROCESS_PUBLIC_ACTION                   prRlmProcessPublicAction;
    RLM_PROCESS_HT_ACTION                       prRlmProcessHtAction;
    RLM_UPDATE_PARAMS_FOR_AP                    prRlmUpdateParamsForAp;
    RLM_HANDLE_OBSS_STATUS_EVENT_PKT            prRlmHandleObssStatusEventPkt;
    P2P_CALCULATE_P2P_IE_LEN         prP2pCalculateP2p_IELenForBeacon;
    P2P_GENERATE_P2P_IE              prP2pGenerateP2p_IEForBeacon;
    P2P_CALCULATE_P2P_IE_LEN         prP2pCalculateP2p_IELenForAssocReq;
    P2P_GENERATE_P2P_IE              prP2pGenerateP2p_IEForAssocReq;
    P2P_CALCULATE_WSC_IE_LEN_FOR_BEACON         prP2pCalculateWSC_IELenForBeacon;
    P2P_GENERATE_WSC_IE_FOR_BEACON              prP2pGenerateWSC_IEForBeacon;
    P2P_CALCULATE_WSC_IE_LEN_FOR_PROBE_RSP      prP2pCalculateWSC_IELenForProbeRsp;
    P2P_GENERATE_WSC_IE_FOR_PROBE_RSP           prP2pGenerateWSC_IEForProbeRsp;
    P2P_FUNC_VALIDATE_PROBE_REQ                 prP2pFuncValidateProbeReq;
    RLM_BSS_INIT_FOR_AP                         prRlmBssInitForAP;
    P2P_GET_PROB_RSP_IE_TABLE_SIZE              prP2pGetTxProbRspIETAbleSize;
    P2P_BUILD_REASSOC_REQ_FRAME_COMMON_IES      prP2pBuildReassocReqFrameCommIEs;
    SCAN_REMOVE_P2P_BSS_DESC                    prScanRemoveP2pBssDesc;
    P2P_FUNC_DISCONNECT                              prP2pFuncDisconnect;
    P2P_FSM_RUN_EVENT_RX_DEAUTH                prP2pFsmRunEventRxDeauthentication;
    P2P_FSM_RUN_EVENT_RX_DISASSOC            prP2pFsmRunEventRxDisassociation;
    P2P_HANDLE_SEC_CHECK_RSP                    prP2pHandleSecCheckRsp;
    P2P_FUN_IS_AP_MODE                                 prP2pFuncIsApMode;
    P2P_FSM_RUN_EVENT_BEACON_TIMEOUT      prP2pFsmRunEventBeaconTimeout;
    P2P_RX_PUBLIC_ACTION_FRAME                  prP2pRxActionFrame;
} P2P_FUNCTION_LINKER, *P_P2P_FUNCTION_LINKER;


#endif

#ifdef __WIFI_SNIFFER_SUPPORT__
typedef struct SMRAT_CONFIG_T {
    UINT_8 u1ListenChannelIndex;//range 0~13
    UINT_8 u1ListenChannelTable[19];
    UINT_8 u1ListenState; //0: do not handdle RX packet, 1: handdle RX packet
} SMRAT_CONFIG_T, *P_SMRAT_CONFIG_T;
#endif

/*
 * Major ADAPTER structure
 * Major data structure for driver operation
 */
struct _ADAPTER_T {
    UINT_8                  ucRevID;

    UINT_16                 u2NicOpChnlNum;

    BOOLEAN                 fgIsEnableWMM;
    BOOLEAN                 fgIsWmmAssoc; /* This flag is used to indicate that WMM is enable in current BSS */

    UINT_32                 u4OsPacketFilter;     // packet filter used by OS


#if CFG_TCP_IP_CHKSUM_OFFLOAD
    UINT_32                 u4CSUMFlags;
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */


    ENUM_BAND_T             aePreferBand[NETWORK_TYPE_INDEX_NUM];

    /* ADAPTER flags */
    UINT_32                 u4Flags;
    UINT_32                 u4HwFlags;

    BOOLEAN                 fgIsRadioOff;

    BOOLEAN                 fgIsEnterD3ReqIssued;

    UINT_8                  aucMacAddress[MAC_ADDR_LEN];

    ENUM_PHY_TYPE_INDEX_T   eCurrentPhyType; /* Current selection basing on the set of Available PHY Types */

#if CFG_COALESCING_BUFFER_SIZE || CFG_SDIO_RX_AGG
    UINT_32                 u4CoalescingBufCachedSize;
    PUINT_8                 pucCoalescingBufCached;
#endif /* CFG_COALESCING_BUFFER_SIZE */

    /* Buffer for CMD_INFO_T, Mgt packet and mailbox message */
    BUF_INFO_T              rMgtBufInfo;
    BUF_INFO_T              rMsgBufInfo;
    PUINT_8                 pucMgtBufCached;
    UINT_32                 u4MgtBufCachedSize;
    UINT_8                  aucMsgBuf[MSG_BUFFER_SIZE];
#if CFG_DBG_MGT_BUF
    UINT_32                 u4MemAllocDynamicCount;     /* Debug only */
    UINT_32                 u4MemFreeDynamicCount;      /* Debug only */
#endif

    STA_RECORD_T            arStaRec[CFG_STA_REC_NUM];

    /* Element for TX PATH */
    TX_CTRL_T               rTxCtrl;
    QUE_T                   rFreeCmdList;
    CMD_INFO_T              arHifCmdDesc[CFG_TX_MAX_CMD_PKT_NUM];

    /* Element for RX PATH */
    RX_CTRL_T               rRxCtrl;

    P_SDIO_CTRL_T           prSDIOCtrl;

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
    /* Element for MT6620 E1 HIFSYS workaround */
    BOOLEAN                 fgIsClockGatingEnabled;
#endif

    /* Buffer for Authentication Event */
    /* <Todo> Move to glue layer and refine the kal function */
    /* Reference to rsnGeneratePmkidIndication function at rsn.c */
    UINT_8                  aucIndicationEventBuffer[(CFG_MAX_PMKID_CACHE * 20)  + 8 ];

    UINT_32                 u4IntStatus;

    ENUM_ACPI_STATE_T       rAcpiState;

    BOOLEAN                 fgIsIntEnable;
    BOOLEAN                 fgIsIntEnableWithLPOwnSet;

    BOOLEAN                 fgIsFwOwn;
    BOOLEAN                 fgWiFiInSleepyState;

    UINT_32                 u4PwrCtrlBlockCnt;

    QUE_T                   rPendingCmdQueue;

    P_GLUE_INFO_T           prGlueInfo;

    UINT_8                  ucCmdSeqNum;
    UINT_8                  ucTxSeqNum;

#if 1//CFG_SUPPORT_WAPI
    BOOLEAN                 fgUseWapi;
#endif

    /* RF Test flags */
    BOOLEAN         fgTestMode;

    /* WLAN Info for DRIVER_CORE OID query */
    WLAN_INFO_T     rWlanInfo;

#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN             fgIsP2PRegistered;
    BOOLEAN             fgIsWlanLaunched;
    P_P2P_INFO_T        prP2pInfo;
    P2P_FUNCTION_LINKER rP2pFuncLkr;
#endif
#if CFG_DEQUEUE_OPT
    kal_int32              u4TC_Count; //Count NonTC1 Frame:TC0,TC2,TC3
#endif

    /* Online Scan Option */
    BOOLEAN         fgEnOnlineScan;

    /* Online Scan Option */
    BOOLEAN         fgDisBcnLostDetection;

    /* MAC address */
    PARAM_MAC_ADDRESS rMyMacAddr;

    /* Wake-up Event for WOL */
    UINT_32         u4WakeupEventEnable;

    /* Event Buffering */
    EVENT_STATISTICS    rStatStruct;
    OS_SYSTIME          rStatUpdateTime;
    BOOLEAN             fgIsStatValid;

    EVENT_LINK_QUALITY  rLinkQuality;
    OS_SYSTIME          rLinkQualityUpdateTime;
    BOOLEAN             fgIsLinkQualityValid;
    OS_SYSTIME          rLinkRateUpdateTime;
    BOOLEAN             fgIsLinkRateValid;

    /* WIFI_VAR_T */
    WIFI_VAR_T          rWifiVar;

    /* MTK WLAN NIC driver IEEE 802.11 MIB */
    IEEE_802_11_MIB_T   rMib;

    /* RF parameters */
    ENUM_BAND_T         eRfBand;            //??
    UINT_8              ucRfPriChannel;     //??

    /* Mailboxs for inter-module communication */
    MBOX_T arMbox[MBOX_ID_TOTAL_NUM];

    /* Timers for OID Pending Handling */
    TIMER_T             rOidTimeoutTimer;

    /* Root Timer for cnm_timer module */
    ROOT_TIMER          rRootTimer;

    /* RLM maintenance */
    ENUM_CHNL_EXT_T             eRfSco;
    ENUM_SYS_PROTECT_MODE_T     eSysProtectMode;
    ENUM_GF_MODE_T              eSysHtGfMode;
    ENUM_RIFS_MODE_T            eSysTxRifsMode;
    ENUM_SYS_PCO_PHASE_T        eSysPcoPhase;

    P_DOMAIN_INFO_ENTRY         prDomainInfo;

    /* QM */
    QUE_MGT_T                   rQM;

    CNM_INFO_T                  rCnmInfo;

    UINT_32 u4PowerMode;

    UINT_32 u4CtiaPowerMode;
    BOOLEAN fgEnCtiaPowerMode;

    UINT_32 fgEnArpFilter;

    UINT_32 u4UapsdAcBmp;

    UINT_32 u4MaxSpLen;

    UINT_32 u4PsCurrentMeasureEn;

    /* Version Information */
    WIFI_VER_INFO_T             rVerInfo;

    /* 5GHz support (from F/W) */
    BOOLEAN fgIsHw5GBandDisabled;
    BOOLEAN fgEnable5GBand;
    BOOLEAN fgIsEepromUsed;
    BOOLEAN fgIsEfuseValid;
    BOOLEAN fgIsEmbbededMacAddrValid;

    /* Packet Forwarding Tracking */
    INT_32  i4PendingFwdFrameCount;

    /* Nelson: workaround for interrupt polling */
    BOOLEAN fgIsTxIntPoll;
    BOOLEAN fgIsRxIntPoll;

#if PTA_ENABLED
    PTA_INFO_T rPtaInfo;
    BOOLEAN    fgIsAntBT;   /* TRUE:BT, FALSE:WIFI*/
    BOOLEAN    fgIsAntBT_beforeDPD;   /* TRUE:BT, FALSE:WIFI*/
#endif

    BOOLEAN WpsProbe;
    BOOLEAN WpsAssoc;
#ifdef __WPS_SUPPORT__
    UINT_8   WpsVer;
    UINT_8   ReqType;
    UINT_16  CfgMethod;
    UINT_8   Uuid[16];
    UINT_16  Cate;
    UINT_32  Oui;
    UINT_16  SubCate;
    UINT_8   RfBands;
    UINT_16  AssocStat;
    UINT_16  CfgErr;
    UINT_16  DevPswdId;
    UCHAR    Manufact[WPS_MANUFACTURE_LEN];
    UINT_8   ManufLen;
    UCHAR    ModelName[WPS_MODEL_NAME_LEN];
    UINT_8   ModelNameLen;
    UCHAR    ModelNum[WPS_MODEL_NUM_LEN];
    UINT_8   ModelNumLen;
    UCHAR    DevName[WPS_DEV_NAME_LEN];
    UINT_8   DevNameLen;
    UINT_8   Ver2;
#endif

    UINT_32  u4RxDataResvTime;

#ifdef __WIFI_SNIFFER_SUPPORT__
    UINT_32  u4CurrentPacketFilter; // store the packet filter of u4OsPacketFilter
    UINT_8   u1SnifferOnOff; // 0: off;  1: on with para,driver indicate packet to upper 2:on without para,driver analize packet to find ap info which indicate to upper
    SMRAT_CONFIG_T   smart_config;
    UINT_8   fgDelayIndDisc; //0: indicate disc in timer handle func, 1:delay indicat untill ais is idle
#endif
};/* end of _ADAPTER_T */

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
/*----------------------------------------------------------------------------*/
/* Macros for BSS_INFO_T - Flag of Net Active                                 */
/*----------------------------------------------------------------------------*/
#define IS_NET_ACTIVE(_prAdapter, _NetTypeIndex) \
                (_prAdapter->rWifiVar.arBssInfo[(_NetTypeIndex)].fgIsNetActive)
#define IS_BSS_ACTIVE(_prBssInfo)     ((_prBssInfo)->fgIsNetActive)

#define IS_AIS_ACTIVE(_prAdapter)     IS_NET_ACTIVE(_prAdapter, NETWORK_TYPE_AIS_INDEX)
#define IS_P2P_ACTIVE(_prAdapter)     IS_NET_ACTIVE(_prAdapter, NETWORK_TYPE_P2P_INDEX)
#define IS_BOW_ACTIVE(_prAdapter)     IS_NET_ACTIVE(_prAdapter, NETWORK_TYPE_BOW_INDEX)

#define SET_NET_ACTIVE(_prAdapter, _NetTypeIndex) \
                {_prAdapter->rWifiVar.arBssInfo[(_NetTypeIndex)].fgIsNetActive = TRUE;}

#define UNSET_NET_ACTIVE(_prAdapter, _NetTypeIndex) \
                {_prAdapter->rWifiVar.arBssInfo[(_NetTypeIndex)].fgIsNetActive = FALSE;}

#define BSS_INFO_INIT(_prAdapter, _NetTypeIndex) \
                {   UINT_8 _aucZeroMacAddr[] = NULL_MAC_ADDR; \
                    P_BSS_INFO_T _prBssInfo = &(_prAdapter->rWifiVar.arBssInfo[(_NetTypeIndex)]); \
                    \
                    _prBssInfo->eConnectionState = PARAM_MEDIA_STATE_DISCONNECTED; \
                    _prBssInfo->eConnectionStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED; \
                    _prBssInfo->eCurrentOPMode = OP_MODE_INFRASTRUCTURE; \
                    _prBssInfo->fgIsNetActive = FALSE; \
                    _prBssInfo->ucNetTypeIndex = (_NetTypeIndex); \
                    _prBssInfo->ucReasonOfDisconnect = DISCONNECT_REASON_CODE_RESERVED; \
                    COPY_MAC_ADDR(_prBssInfo->aucBSSID, _aucZeroMacAddr); \
                    LINK_INITIALIZE(&_prBssInfo->rStaRecOfClientList); \
                    _prBssInfo->fgIsBeaconActivated = FALSE; \
                    _prBssInfo->ucHwDefaultFixedRateCode = RATE_CCK_1M_LONG; \
                    _prBssInfo->fgIsNetAbsent = FALSE; \
                }

#if CFG_ENABLE_BT_OVER_WIFI
#define BOW_BSS_INFO_INIT(_prAdapter, _NetTypeIndex) \
                {  \
                    P_BSS_INFO_T _prBssInfo = &(_prAdapter->rWifiVar.arBssInfo[(_NetTypeIndex)]); \
                    \
                    _prBssInfo->eConnectionState = PARAM_MEDIA_STATE_DISCONNECTED; \
                    _prBssInfo->eConnectionStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED; \
                    _prBssInfo->eCurrentOPMode = OP_MODE_BOW; \
                    _prBssInfo->ucNetTypeIndex = (_NetTypeIndex); \
                    _prBssInfo->ucReasonOfDisconnect = DISCONNECT_REASON_CODE_RESERVED; \
                    LINK_INITIALIZE(&_prBssInfo->rStaRecOfClientList); \
                    _prBssInfo->fgIsBeaconActivated = TRUE; \
                    _prBssInfo->ucHwDefaultFixedRateCode = RATE_CCK_1M_LONG; \
                    _prBssInfo->fgIsNetAbsent = FALSE; \
                }
#endif

#if CFG_ENABLE_WIFI_TETHERING
#define WT_BSS_INFO_INIT(_prAdapter, _NetTypeIndex) \
                {  \
                    P_BSS_INFO_T _prBssInfo = &(_prAdapter->rWifiVar.arBssInfo[(_NetTypeIndex)]); \
                    \
                    _prBssInfo->eConnectionState = PARAM_MEDIA_STATE_DISCONNECTED; \
                    _prBssInfo->eConnectionStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED; \
                    _prBssInfo->eCurrentOPMode = OP_MODE_ACCESS_POINT; \
                    _prBssInfo->ucNetTypeIndex = (_NetTypeIndex); \
                    _prBssInfo->ucReasonOfDisconnect = DISCONNECT_REASON_CODE_RESERVED; \
                    LINK_INITIALIZE(&_prBssInfo->rStaRecOfClientList); \
                    _prBssInfo->fgIsBeaconActivated = TRUE; \
                    _prBssInfo->ucHwDefaultFixedRateCode = RATE_CCK_1M_LONG; \
                    _prBssInfo->fgIsNetAbsent = FALSE; \
                }
#endif /* CFG_ENABLE_WIFI_TETHERING */

#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
typedef  struct _SSID_LIST {
    LINK_ENTRY_T            rLinkEntry;
    abm_wndrv_ssid_struct   *ssid_list;

} SSID_LIST_T, *P_SSID_LIST_T;
#endif

/*----------------------------------------------------------------------------*/
/* Macros for Power State                                                     */
/*----------------------------------------------------------------------------*/
#define SET_NET_PWR_STATE_IDLE(_prAdapter, _NetTypeIndex) \
                {_prAdapter->rWifiVar.aePwrState[(_NetTypeIndex)] = PWR_STATE_IDLE;}

#define SET_NET_PWR_STATE_ACTIVE(_prAdapter, _NetTypeIndex) \
                {_prAdapter->rWifiVar.aePwrState[(_NetTypeIndex)] = PWR_STATE_ACTIVE;}

#define SET_NET_PWR_STATE_PS(_prAdapter, _NetTypeIndex) \
                {_prAdapter->rWifiVar.aePwrState[(_NetTypeIndex)] = PWR_STATE_PS;}

#define IS_NET_PWR_STATE_ACTIVE(_prAdapter, _NetTypeIndex) \
                (_prAdapter->rWifiVar.aePwrState[(_NetTypeIndex)] == PWR_STATE_ACTIVE)

#define IS_NET_PWR_STATE_IDLE(_prAdapter, _NetTypeIndex) \
                (_prAdapter->rWifiVar.aePwrState[(_NetTypeIndex)] == PWR_STATE_IDLE)

#define IS_SCN_PWR_STATE_ACTIVE(_prAdapter) \
                (_prAdapter->rWifiVar.rScanInfo.eScanPwrState == SCAN_PWR_STATE_ACTIVE)

#define IS_SCN_PWR_STATE_IDLE(_prAdapter) \
                (_prAdapter->rWifiVar.rScanInfo.eScanPwrState == SCAN_PWR_STATE_IDLE)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _ADAPTER_H */


