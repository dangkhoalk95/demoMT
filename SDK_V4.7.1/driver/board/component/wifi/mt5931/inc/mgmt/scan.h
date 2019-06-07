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
** $Id: @(#)
*/

/*! \file   "scan.h"
    \brief

*/




#ifndef _SCAN_H
#define _SCAN_H

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
/*! Maximum buffer size of SCAN list */
#define SCN_MAX_BUFFER_SIZE                 (CFG_MAX_NUM_BSS_LIST * ALIGN_4(sizeof(BSS_DESC_T)))

#define SCN_RM_POLICY_EXCLUDE_CONNECTED     BIT(0) // Remove SCAN result except the connected one.
#define SCN_RM_POLICY_TIMEOUT               BIT(1) // Remove the timeout one
#define SCN_RM_POLICY_OLDEST_HIDDEN         BIT(2) // Remove the oldest one with hidden ssid
#define SCN_RM_POLICY_SMART_WEAKEST         BIT(3) /* If there are more than half BSS which has the
                                                    * same ssid as connection setting, remove the weakest one from them
                                                    * Else remove the weakest one.
                                                    */
#define SCN_RM_POLICY_ENTIRE                BIT(4) // Remove entire SCAN result

#define SCN_RM_POLICY_EXCLUDE_DESIRED_SSID_LIST                BIT(5) // Remove list no in desired ssid list

#define SCN_RM_POLICY_WEAKEST_BSSID_OF_MAXNUM_BSSID_OF_SAME_SSID_IN_DESIRED_SSID_LIST                BIT(6) // 

#define SCN_RM_POLICY_EXCLUDE_SCANREQ_SSID                BIT(7) //Do not remove scan ssid which is in scan req

#define SCN_BSS_DESC_SAME_SSID_THRESHOLD    3 /* This is used by POLICY SMART WEAKEST,
                                               * If exceed this value, remove weakest BSS_DESC_T
                                               * with same SSID first in large network.
                                               */

#define SCN_BSS_DESC_REMOVE_TIMEOUT_SEC     5 // Second.
/* This is used by POLICY TIMEOUT,
 * If exceed this value, remove timeout BSS_DESC_T.
 */




#define SCN_PROBE_DELAY_MSEC                0

#define SCN_ADHOC_BSS_DESC_TIMEOUT_SEC      5 // Second.

/*----------------------------------------------------------------------------*/
/* MSG_SCN_SCAN_REQ                                                           */
/*----------------------------------------------------------------------------*/
#define SCAN_REQ_SSID_WILDCARD              BIT(0)
#define SCAN_REQ_SSID_P2P_WILDCARD          BIT(1)
#define SCAN_REQ_SSID_SPECIFIED             BIT(2)


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_SCAN_TYPE_T {
    SCAN_TYPE_PASSIVE_SCAN = 0,
    SCAN_TYPE_ACTIVE_SCAN,
    SCAN_TYPE_NUM
} ENUM_SCAN_TYPE_T, *P_ENUM_SCAN_TYPE_T;

typedef enum _ENUM_SCAN_STATE_T {
    SCAN_STATE_IDLE = 0,
    SCAN_STATE_SCANNING,
    SCAN_STATE_NUM
} ENUM_SCAN_STATE_T;

typedef enum _ENUM_SCAN_CHANNEL_T {
    SCAN_CHANNEL_FULL = 0,
    SCAN_CHANNEL_2G4,
    SCAN_CHANNEL_5G,
    SCAN_CHANNEL_P2P_SOCIAL,
    SCAN_CHANNEL_SPECIFIED,
    SCAN_CHANNEL_NUM
} ENUM_SCAN_CHANNEL, *P_ENUM_SCAN_CHANNEL;

typedef struct _MSG_SCN_FSM_T {
    MSG_HDR_T       rMsgHdr;        /* Must be the first member */
    UINT_32         u4Dummy;
} MSG_SCN_FSM_T, *P_MSG_SCN_FSM_T;



/*----------------------------------------------------------------------------*/
/* BSS Descriptors                                                            */
/*----------------------------------------------------------------------------*/
struct _BSS_DESC_T {
    LINK_ENTRY_T            rLinkEntry;

    UINT_8                  aucBSSID[MAC_ADDR_LEN];
    UINT_8                  aucSrcAddr[MAC_ADDR_LEN]; /* For IBSS, the SrcAddr is different from BSSID */

    BOOLEAN                 fgIsConnecting; /* If we are going to connect to this BSS
                                             * (JOIN or ROAMING to another BSS), don't
                                             * remove this record from BSS List.
                                             */
    BOOLEAN                 fgIsConnected; /* If we have connected to this BSS (NORMAL_TR),
                                            * don't removed this record from BSS list.
                                            */

    BOOLEAN                 fgIsHiddenSSID; /* When this flag is TRUE, means the SSID
                                             * of this BSS is not known yet.
                                             */
    UINT_8                  ucSSIDLen;
    UINT_8                  aucSSID[ELEM_MAX_LEN_SSID];

    OS_SYSTIME              rUpdateTime;

    ENUM_BSS_TYPE_T         eBSSType;

    UINT_16                 u2CapInfo;

    UINT_16                 u2BeaconInterval;
    UINT_16                 u2ATIMWindow;

    UINT_16                 u2OperationalRateSet;
    UINT_16                 u2BSSBasicRateSet;
    BOOLEAN                 fgIsUnknownBssBasicRate;

    BOOLEAN                 fgIsERPPresent;
    BOOLEAN                 fgIsHTPresent;

    UINT_8                  ucPhyTypeSet;           /* Available PHY Type Set of this BSS */

    UINT_8                  ucChannelNum;

    ENUM_CHNL_EXT_T         eSco;                   /* Record bandwidth for association process
                                                       Some AP will send association resp by 40MHz BW */
    ENUM_BAND_T             eBand;

    UINT_8                  ucDTIMPeriod;

    BOOLEAN                 fgIsLargerTSF; /* This BSS's TimeStamp is larger than us(TCL == 1 in RX_STATUS_T) */

    UINT_8                  ucRCPI;

    UINT_8                  ucWmmFlag; /* A flag to indicate this BSS's WMM capability */

    /*! \brief The srbiter Search State will matched the scan result,
               and saved the selected cipher and akm, and report the score,
               for arbiter join state, join module will carry this target BSS
               to rsn generate ie function, for gen wpa/rsn ie */
    UINT_32                 u4RsnSelectedGroupCipher;
    UINT_32                 u4RsnSelectedPairwiseCipher;
    UINT_32                 u4RsnSelectedAKMSuite;

    UINT_16                 u2RsnCap;

    RSN_INFO_T              rRSNInfo;
    RSN_INFO_T              rWPAInfo;
#if 1//__WAPI_SUPPORT__
    WAPI_INFO_T             rIEWAPI;
    BOOL                    fgIEWAPI;
#endif
    BOOL                    fgIERSN;
    BOOL                    fgIEWPA;

    BOOL                    fgPSK;

    UINT_8                  aucIERSN[ WNDRV_MAX_RSN_IE_LEN ];
    UINT_8                  aucIEWPA[ WNDRV_MAX_WPA_IE_LEN ];
    UINT_8                  aucIEWAPI[ WNDRV_MAX_WAPI_IE_LEN ];

    /*! \brief RSN parameters selected for connection */
    /*! \brief The Select score for final AP selection,
               0, no sec, 1,2,3 group cipher is WEP, TKIP, CCMP */
    UINT_8                  ucEncLevel;

#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN               fgIsP2PPresent;
    P_P2P_DEVICE_DESC_T  prP2pDesc;

    UINT_8                  aucIntendIfAddr[MAC_ADDR_LEN]; /* For IBSS, the SrcAddr is different from BSSID */
//    UINT_8 ucDevCapabilityBitmap;  /* Device Capability Attribute. (P2P_DEV_CAPABILITY_XXXX) */
//    UINT_8 ucGroupCapabilityBitmap;  /* Group Capability Attribute. (P2P_GROUP_CAPABILITY_XXXX) */

    LINK_T rP2pDeviceList;

//    P_LINK_T prP2pDeviceList;

    /* For
      *    1. P2P Capability.
      *    2. P2P Device ID. ( in aucSrcAddr[] )
      *    3. NOA   (TODO:)
      *    4. Extend Listen Timing. (Probe Rsp)  (TODO:)
      *    5. P2P Device Info. (Probe Rsp)
      *    6. P2P Group Info. (Probe Rsp)
      */
#endif

    BOOLEAN                 fgIsIEOverflow; /* The received IE length exceed the maximum IE buffer size */
    UINT_16                 u2IELength; /* The byte count of aucIEBuf[] */

    ULARGE_INTEGER          u8TimeStamp; /* Place u8TimeStamp before aucIEBuf[1] to force DW align */

    UINT_8                  aucIEBuf[CFG_IE_BUFFER_SIZE];
};


typedef struct _SCAN_PARAM_T { /* Used by SCAN FSM */

    ENUM_SCAN_TYPE_T            eScanType;                          /* Active or Passive */
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex;                      /* To fill MSDU_INFO_T */
    UINT_8                      ucSSIDType;                         /* Specified SSID */
    UINT_8                      ucSpecifiedSSIDLen;                 /* Length of Specified SSID */
    UINT_8                      aucSpecifiedSSID[ELEM_MAX_LEN_SSID];/* Specified SSID */
    UINT_8                      aucSrcAddress[MAC_ADDR_LEN];
    UINT_8                      ucNumOfPrbReq;
    UINT_8                      ucNumOfSpecifiedSsidPrbReq;
    UINT_8                      ucNumOfScanChnl;

#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN                     fgFindSpecificDev;                  /* P2P: Discovery Protocol */
    UINT_8                      aucDiscoverDevAddr[MAC_ADDR_LEN];
    BOOLEAN                     fgIsDevType;
    P2P_DEVICE_TYPE_T           rDiscoverDevType;

    UINT_16                     u2PassiveListenInterval;
    // TODO: Find Specific Device Type.
#endif /* CFG_SUPPORT_P2P */

    BOOLEAN                     fgIsObssScan;

    /* Run time flags */
    /* channel information */
    ENUM_SCAN_CHANNEL   eScanChannel;
    UINT_8              ucChannelListNum;
    RF_CHANNEL_INFO_T   arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];

    /* Feedback information */
    UINT_8              ucSeqNum;

    /* Information Element */
    UINT_16             u2IELen;
    UINT_8              aucIE[MAX_IE_LENGTH];

} SCAN_PARAM_T, *P_SCAN_PARAM_T;

typedef struct _SCAN_INFO_T {
    ENUM_SCAN_STATE_T       eCurrentState;  /* Store the STATE variable of SCAN FSM */

    OS_SYSTIME              rLastScanCompletedTime;

    SCAN_PARAM_T            rScanParam;

    UINT_32                 u4NumOfBssDesc;

    UINT_8                  aucScanBuffer[SCN_MAX_BUFFER_SIZE];

    LINK_T                  rBSSDescList;

    LINK_T                  rFreeBSSDescList;

    LINK_T                  rPendingMsgList;

    /* Sparse Channel Detection */
    BOOLEAN                 fgIsSparseChannelValid;
    RF_CHANNEL_INFO_T       rSparseChannel;
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    LINK_T                  rscanDesiredSSIDList;
#endif
    UINT_16                 u2ChannelDwellTime;
    PARAM_SSID_T            rSSIDOfScanReq;
} SCAN_INFO_T, *P_SCAN_INFO_T;


/* Incoming Mailbox Messages */
typedef struct _MSG_SCN_SCAN_REQ_T {
    MSG_HDR_T           rMsgHdr;        /* Must be the first member */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
    ENUM_SCAN_TYPE_T    eScanType;
    UINT_8              ucSSIDType;     /* BIT(0) wildcard / BIT(1) P2P-wildcard / BIT(2) specific */
    UINT_8              ucSSIDLength;
    UINT_8              aucSSID[PARAM_MAX_LEN_SSID];
#if CFG_ENABLE_WIFI_DIRECT
    UINT_16            u2ChannelDwellTime;  /* In TU. 1024us. */
#endif
    ENUM_SCAN_CHANNEL   eScanChannel;
    UINT_8              ucChannelListNum;
    RF_CHANNEL_INFO_T   arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];
    UINT_16             u2IELen;
    UINT_8              aucIE[MAX_IE_LENGTH];
} MSG_SCN_SCAN_REQ, *P_MSG_SCN_SCAN_REQ;

typedef struct _MSG_SCN_SCAN_CANCEL_T {
    MSG_HDR_T           rMsgHdr;        /* Must be the first member */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN           fgIsChannelExt;
#endif
} MSG_SCN_SCAN_CANCEL, *P_MSG_SCN_SCAN_CANCEL;

/* Outgoing Mailbox Messages */
typedef enum _ENUM_SCAN_STATUS_T {
    SCAN_STATUS_DONE = 0,
    SCAN_STATUS_CANCELLED,
    SCAN_STATUS_FAIL,
    SCAN_STATUS_BUSY,
    SCAN_STATUS_NUM
} ENUM_SCAN_STATUS, *P_ENUM_SCAN_STATUS;

typedef struct _MSG_SCN_SCAN_DONE_T {
    MSG_HDR_T           rMsgHdr;        /* Must be the first member */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
    ENUM_SCAN_STATUS    eScanStatus;
} MSG_SCN_SCAN_DONE, *P_MSG_SCN_SCAN_DONE;

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
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/* Routines in scan.c                                                         */
/*----------------------------------------------------------------------------*/
VOID
scnInit(
    IN P_ADAPTER_T prAdapter
);

VOID
scnUninit(
    IN P_ADAPTER_T prAdapter
);

/* BSS-DESC Search */
P_BSS_DESC_T
scanSearchBssDescByBssid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
);

P_BSS_DESC_T
scanSearchBssDescByBssidAndSsid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
);

P_BSS_DESC_T
scanSearchBssDescByTA(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucSrcAddr[]
);

P_BSS_DESC_T
scanSearchBssDescByTAAndSsid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucSrcAddr[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
);


/* BSS-DESC Search - Alternative */
P_BSS_DESC_T
scanSearchExistingBssDesc(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T eBSSType,
    IN UINT_8 aucBSSID[],
    IN UINT_8 aucSrcAddr[]
);

P_BSS_DESC_T
scanSearchExistingBssDescWithSsid(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T eBSSType,
    IN UINT_8 aucBSSID[],
    IN UINT_8 aucSrcAddr[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
);


/* BSS-DESC Allocation */
P_BSS_DESC_T
scanAllocateBssDesc(
    IN P_ADAPTER_T prAdapter
);

/* BSS-DESC Removal */
VOID
scanRemoveBssDescsByPolicy(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4RemovePolicy
);

VOID
scanRemoveBssDescByBssid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
);

VOID
scanRemoveBssDescByBandAndNetwork(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BAND_T eBand,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
);

/* BSS-DESC State Change */
VOID
scanRemoveConnFlagOfBssDescByBssid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
);

#if 0
/* BSS-DESC Insertion */
P_BSS_DESC_T
scanAddToInternalScanResult(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSWRfb,
    IN P_BSS_DESC_T prBssDesc
);
#endif

/* BSS-DESC Insertion - ALTERNATIVE */
P_BSS_DESC_T
scanAddToBssDesc(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
);

WLAN_STATUS
scanProcessBeaconAndProbeResp(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSWRfb
);

VOID
scanBuildProbeReqFrameCommonIEs(
    IN P_MSDU_INFO_T    prMsduInfo,
    IN PUINT_8          pucDesiredSsid,
    IN UINT_32          u4DesiredSsidLen,
    IN UINT_16          u2SupportedRateSet
);

WLAN_STATUS
scanSendProbeReqFrames(
    IN P_ADAPTER_T prAdapter,
    IN P_SCAN_PARAM_T prScanParam
);

VOID
scanUpdateBssDescForSearch(
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prBssDesc
);

P_BSS_DESC_T
scanSearchBssDescByPolicy(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
);

WLAN_STATUS
scanAddScanResult(
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prBssDesc,
    IN P_SW_RFB_T prSwRfb
);


/*----------------------------------------------------------------------------*/
/* Routines in scan_fsm.c                                                     */
/*----------------------------------------------------------------------------*/
VOID
scnFsmSteps(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_SCAN_STATE_T eNextState
);

/*----------------------------------------------------------------------------*/
/* RX Event Handling                                                          */
/*----------------------------------------------------------------------------*/
VOID
scnEventScanDone(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucSeqNum
);

/*----------------------------------------------------------------------------*/
/* Mailbox Message Handling                                                   */
/*----------------------------------------------------------------------------*/
VOID
scnFsmMsgStart(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

VOID
scnFsmMsgAbort(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

/*----------------------------------------------------------------------------*/
/* Mailbox Message Generation                                                 */
/*----------------------------------------------------------------------------*/
VOID
scnFsmGenerateScanDoneMsg(
    IN P_ADAPTER_T          prAdapter,
    IN UINT_8               ucSeqNum,
    IN UINT_8               ucNetTypeIndex,
    IN ENUM_SCAN_STATUS     eScanStatus
);

#endif /* _SCAN_H */


