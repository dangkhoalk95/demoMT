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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/nic/nic.h#18 $
*/

/*! \file   "nic.h"
    \brief  The declaration of nic functions

    Detail description.
*/




#ifndef _NIC_H
#define _NIC_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
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


struct _REG_ENTRY_T {
    UINT_32      u4Offset;
    UINT_32      u4Value;
};

struct _TABLE_ENTRY_T {
    P_REG_ENTRY_T     pu4TablePtr;
    UINT_16      u2Size;
};

/*! INT status to event map */
typedef struct _INT_EVENT_MAP_T {
    UINT_32     u4Int;
    UINT_32     u4Event;
} INT_EVENT_MAP_T, *P_INT_EVENT_MAP_T;


enum ENUM_INT_EVENT_T {
    INT_EVENT_ABNORMAL,
    INT_EVENT_SW_INT,
    INT_EVENT_TX,
    INT_EVENT_RX,
    INT_EVENT_NUM
};

typedef enum _ENUM_IE_UPD_METHOD_T {
    IE_UPD_METHOD_UPDATE_RANDOM,
    IE_UPD_METHOD_UPDATE_ALL,
    IE_UPD_METHOD_DELETE_ALL
} ENUM_IE_UPD_METHOD_T, *P_ENUM_IE_UPD_METHOD_T;

typedef enum _ENUM_WNDRV_BUF_CACHE_T {
    WNDRV_BUF_MGT_CACHE,
    WNDRV_BUF_RX_PKT,
    WNDRV_BUF_TX_PKT
} ENUM_WNDRV_BUF_CACHE_T;


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

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
/* Routines in nic.c                                                          */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicAllocateAdapterMemory(
    IN P_ADAPTER_T prAdapter
);

VOID
nicReleaseAdapterMemory(
    IN P_ADAPTER_T prAdapter
);

VOID
nicDisableInterrupt(
    IN P_ADAPTER_T prAdapter
);

VOID
nicEnableInterrupt(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
nicProcessIST(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
nicProcessIST_impl(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4IntStatus
);

WLAN_STATUS
nicInitializeAdapter(
    IN P_ADAPTER_T prAdapter
);

VOID
nicMCRInit(
    IN P_ADAPTER_T prAdapter
);

BOOL
nicVerifyChipID(
    IN P_ADAPTER_T prAdapter
);


#if CFG_SDIO_INTR_ENHANCE
VOID
nicSDIOInit(
    IN P_ADAPTER_T prAdapter
);

VOID
nicSDIOReadIntStatus(
    IN P_ADAPTER_T prAdapter,
    OUT PUINT_32   pu4IntStatus
);
#endif

BOOLEAN
nicpmSetDriverOwn(
    IN P_ADAPTER_T prAdapter
);

VOID
nicpmSetFWOwn(
    IN P_ADAPTER_T prAdapter,
    IN BOOLEAN     fgEnableGlobalInt
);

BOOLEAN
nicpmSetAcpiPowerD0(
    IN P_ADAPTER_T prAdapter
);

BOOLEAN
nicpmSetAcpiPowerD3(
    IN P_ADAPTER_T prAdapter
);

#if defined(_HIF_SPI)
void
nicRestoreSpiDefMode(
    IN P_ADAPTER_T prAdapter
);
#endif

VOID
nicProcessSoftwareInterrupt(
    IN P_ADAPTER_T prAdapter
);

VOID
nicProcessAbnormalInterrupt(
    IN  P_ADAPTER_T prAdapter
);

VOID
nicPutMailbox(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4MailboxNum,
    IN UINT_32 u4Data);

VOID
nicGetMailbox(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4MailboxNum,
    OUT PUINT_32 pu4Data);

VOID
nicSetSwIntr(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4SwIntrBitmap
);

P_CMD_INFO_T
nicGetPendingCmdInfo(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucSeqNum
);

P_MSDU_INFO_T
nicGetPendingTxMsduInfo(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucSeqNum
);

P_MSDU_INFO_T
nicGetPendingStaMMPDU(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucStaRecIdx
);

VOID
nicFreePendingTxMsduInfoByNetwork(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetworkType
);

UINT_8
nicIncreaseCmdSeqNum(
    IN P_ADAPTER_T prAdapter
);

UINT_8
nicIncreaseTxSeqNum(
    IN P_ADAPTER_T prAdapter
);

/* Media State Change */
WLAN_STATUS
nicMediaStateChange(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetworkType,
    IN P_EVENT_CONNECTION_STATUS    prConnectionStatus
);

/* Utility function for channel number conversion */
UINT_32
nicChannelNum2Freq(
    IN UINT_32 u4ChannelNum
);

UINT_32
nicFreq2ChannelNum(
    IN UINT_32 u4FreqInKHz
);

/* firmware command wrapper */
/* NETWORK (WIFISYS) */
WLAN_STATUS
nicActivateNetwork(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

WLAN_STATUS
nicDeactivateNetwork(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);


/* BSS-INFO */
WLAN_STATUS
nicUpdateBss(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

/* BSS-INFO Indication (PM) */
WLAN_STATUS
nicPmIndicateBssCreated(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

WLAN_STATUS
nicPmIndicateBssConnected(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

WLAN_STATUS
nicPmIndicateBssAbort(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

/* Beacon Template Update */
WLAN_STATUS
nicUpdateBeaconIETemplate(
    IN  P_ADAPTER_T prAdapter,
    IN  ENUM_IE_UPD_METHOD_T eIeUpdMethod,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex,
    IN UINT_16 u2Capability,
    IN PUINT_8 aucIe,
    IN UINT_16 u2IELen
);

WLAN_STATUS
nicQmUpdateWmmParms(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);


/*----------------------------------------------------------------------------*/
/* Calibration Control                                                        */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicUpdateTxPower(
    IN P_ADAPTER_T      prAdapter,
    IN P_CMD_TX_PWR_T   prTxPwrParam
);

WLAN_STATUS
nicUpdate5GOffset(
    IN P_ADAPTER_T              prAdapter,
    IN P_CMD_5G_PWR_OFFSET_T    pr5GPwrOffset
);

WLAN_STATUS
nicUpdateDPD(
    IN P_ADAPTER_T          prAdapter,
    IN P_CMD_PWR_PARAM_T    prDpdCalResult
);


/*----------------------------------------------------------------------------*/
/* PHY configuration                                                          */
/*----------------------------------------------------------------------------*/
VOID
nicSetAvailablePhyTypeSet(
    IN P_ADAPTER_T prAdapter
);

/*----------------------------------------------------------------------------*/
/* MGMT and System Service Control                                            */
/*----------------------------------------------------------------------------*/
VOID
nicInitSystemService(
    IN P_ADAPTER_T prAdapter
);

VOID
nicResetSystemService(
    IN P_ADAPTER_T prAdapter
);

VOID
nicUninitSystemService(
    IN P_ADAPTER_T prAdapter
);

VOID
nicInitMGMT(
    IN P_ADAPTER_T prAdapter,
    IN P_REG_INFO_T prRegInfo
);

VOID
nicUninitMGMT(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
nicConfigPowerSaveProfile(
    IN  P_ADAPTER_T prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex,
    PARAM_POWER_MODE ePwrMode,
    BOOLEAN fgEnCmdEvent
);

/*----------------------------------------------------------------------------*/
/* Scan Result Processing                                                     */
/*----------------------------------------------------------------------------*/
VOID
nicAddScanResult(
    IN P_ADAPTER_T                  prAdapter,
    IN PARAM_MAC_ADDRESS            rMacAddr,
    IN P_PARAM_SSID_T               prSsid,
    IN UINT_32                      u4Privacy,
    IN PARAM_RSSI                   rRssi,
    IN ENUM_PARAM_NETWORK_TYPE_T    eNetworkType,
    IN P_PARAM_802_11_CONFIG_T      prConfiguration,
    IN ENUM_PARAM_OP_MODE_T         eOpMode,
    IN PARAM_RATES_EX               rSupportedRates,
    IN UINT_16                      u2IELength,
    IN PUINT_8                      pucIEBuf
);

VOID
nicFreeScanResultIE(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4Idx
);

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
/*----------------------------------------------------------------------------*/
/* Workaround Control                                                         */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicEnableClockGating(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
nicDisableClockGating(
    IN P_ADAPTER_T prAdapter
);
#endif


/*----------------------------------------------------------------------------*/
/* Fixed Rate Hacking                                                         */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicUpdateRateParams(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_REGISTRY_FIXED_RATE_T   eRateSetting,
    IN PUINT_8                      pucDesiredPhyTypeSet,
    IN PUINT_16                     pu2DesiredNonHTRateSet,
    IN PUINT_16                     pu2BSSBasicRateSet,
    IN PUINT_8                      pucMcsSet,
    IN PUINT_8                      pucSupMcs32,
    IN PUINT_16                     u2HtCapInfo
);

/*----------------------------------------------------------------------------*/
/* Write registers                                                            */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicWriteMcr(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32   u4Address,
    IN  UINT_32  u4Value
);

/*----------------------------------------------------------------------------*/
/* Update auto rate                                                           */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRlmArUpdateParms(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4ArSysParam0,
    IN UINT_32 u4ArSysParam1,
    IN UINT_32 u4ArSysParam2,
    IN UINT_32 u4ArSysParam3
);

/*----------------------------------------------------------------------------*/
/* Enable/Disable Roaming                                                     */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRoamingUpdateParams(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4EnableRoaming
);


VOID
nicPrintFirmwareAssertInfo(
    IN P_ADAPTER_T prAdapter
);

/*----------------------------------------------------------------------------*/
/* Link Quality Updating                                                      */
/*----------------------------------------------------------------------------*/
VOID
nicUpdateLinkQuality(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetTypeIdx,
    IN P_EVENT_LINK_QUALITY         prEventLinkQuality
);

VOID
nicUpdateRSSI(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetTypeIdx,
    IN INT_8                        cRssi,
    IN INT_8                        cLinkQuality
);

VOID
nicUpdateLinkSpeed(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetTypeIdx,
    IN UINT_16                      u2LinkSpeed
);

#endif /* _NIC_H */

