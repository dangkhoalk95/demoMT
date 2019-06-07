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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/wlan_lib.h#23 $
*/

/*! \file   "wlan_lib.h"
    \brief  The declaration of the functions of the wlanAdpater objects

    Detail description.
*/




#ifndef _WLAN_LIB_H
#define _WLAN_LIB_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "CFG_Wifi_File.h"
#include "rlm_domain.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define MAX_NUM_GROUP_ADDR                      32      /* max number of group addresses */



#define TX_CS_TCP_UDP_GEN        BIT(1)
#define TX_CS_IP_GEN             BIT(0)


#define CSUM_OFFLOAD_EN_TX_TCP      BIT(0)
#define CSUM_OFFLOAD_EN_TX_UDP      BIT(1)
#define CSUM_OFFLOAD_EN_TX_IP       BIT(2)
#define CSUM_OFFLOAD_EN_RX_TCP      BIT(3)
#define CSUM_OFFLOAD_EN_RX_UDP      BIT(4)
#define CSUM_OFFLOAD_EN_RX_IPv4     BIT(5)
#define CSUM_OFFLOAD_EN_RX_IPv6     BIT(6)
#define CSUM_OFFLOAD_EN_TX_MASK     BITS(0,2)
#define CSUM_OFFLOAD_EN_ALL         BITS(0,6)

/* TCP, UDP, IP Checksum */
#define RX_CS_TYPE_UDP           BIT(7)
#define RX_CS_TYPE_TCP           BIT(6)
#define RX_CS_TYPE_IPv6          BIT(5)
#define RX_CS_TYPE_IPv4          BIT(4)

#define RX_CS_STATUS_UDP         BIT(3)
#define RX_CS_STATUS_TCP         BIT(2)
#define RX_CS_STATUS_IP          BIT(0)

#define CSUM_NOT_SUPPORTED      0x0

/* MT5931 default TX power offset */
#define TX_TSSI_OFFSET          0x3ba
#define TX_TSSI_SLOPE           0x41
#define TX_CH_OFFSET_L          0xE0
#define TX_CH_OFFSET_M          0x0
#define TX_CH_OFFSET_H          0x20
#define TX_ALL_CH_OFFSET        0x0

/* MT5931 EFUSE offset */
#define EFUSE_BLOCK_OFFSET_3_1  (28)
#define EFUSE_BLOCK_OFFSET_4_0  (32)
#define EFUSE_BLOCK_OFFSET_7_0  (56)

#define EFUSE_CH_OFFSET_VLD_1       (1)
#define EFUSE_CH_OFFSET_VLD_2       (1)
#define EFUSE_CRYSTAL_TRIM_1_VLD    (1)
#define EFUSE_CRYSTAL_TRIM_2_VLD    (1)

/* MT5931 EVENT/DATA port */
#if defined(__WIFI_HIF_SDIO__)
#define HIF_RX_EVENT_PORT_NUM       (1)
#else
#define HIF_RX_EVENT_PORT_NUM       (0)     /* 0: workaround for EHPI HW issue, 1: for normal case */
#endif

/* MT5931 CR address */
#define PD_SLOPE_CR_ADDR        0x60205c68

/* MT5931 TX bandedge max power */
#define DefaultBandEdgeMaxPwrCCK       (127)
#define DefaultBandEdgeMaxPwrOFDM20    (127)
#define DefaultBandEdgeMaxPwrOFDM40    (127)

/* MT5931 customization setting default values */
#define WNDRV_CUST_DEFAULT_REG_DOMAIN               (('J' << 8) | 'P')  /* Japan */
#define WNDRV_CUST_DEFAULT_PS_MODE                  (2)  /*PS_FAST_PSP*/
#define WNDRV_CUST_DEFAULT_OSC_STABLE_TIME          (1900)
#define WNDRV_CUST_DEFAULT_WMMPS_CONFIG             (0)
#define WNDRV_CUST_DEFAULT_EINT_POLARITY            (0)
#define WNDRV_CUST_DEFAULT_LOWEST_RSSI              (-85)
#define WNDRV_CUST_DEFAULT_PATH_LOSS                (0)
#define WNDRV_CUST_DEFAULT_CONNECTION_KEEP_TIME     (20)
#define WNDRV_CUST_DEFAULT_CONNECTION_RETRY_LIMIT   (2)
#define WNDRV_CUST_DEFAULT_DTIM_PERIOD              (2)
#if defined(__BTMODULE_MT6236__) || defined(__BTMODULE_MT6276__)
#define WNDRV_CUST_BT_1WIRE_MODE_T6                 (0x8)
#define WNDRV_CUST_BT_1WIRE_MODE_T8                 (0x94)
#define WNDRV_CUST_BT_1WIRE_MODE_BT_DELAY           (0x1C)
#elif defined(__BTMODULE_MT6622__) || defined(__BTMODULE_MT6626__) || defined(__BTMODULE_MT6255__) || defined(__BTMODULE_MT6256__) || defined(__BTMODULE_MT6250__)
#define WNDRV_CUST_BT_1WIRE_MODE_T6                 (0x8)
#define WNDRV_CUST_BT_1WIRE_MODE_T8                 (0x6E)
#define WNDRV_CUST_BT_1WIRE_MODE_BT_DELAY           (0x1A)
#elif defined(__BTMODULE_MT6261__)
//#define WNDRV_CUST_BT_1WIRE_MODE_T6                 (8)
//#define WNDRV_CUST_BT_1WIRE_MODE_T8                 (14)
//#define WNDRV_CUST_BT_1WIRE_MODE_BT_DELAY           (14)
#define WNDRV_CUST_BT_1WIRE_MODE_T6                 (0x8)
#define WNDRV_CUST_BT_1WIRE_MODE_T8                 (0x6E)
#define WNDRV_CUST_BT_1WIRE_MODE_BT_DELAY           (0x1A)

#else
// default setting
#define WNDRV_CUST_BT_1WIRE_MODE_T6                 (8)
#define WNDRV_CUST_BT_1WIRE_MODE_T8                 (110)
#define WNDRV_CUST_BT_1WIRE_MODE_BT_DELAY           (26)
#endif

#define WNDRV_CUST_SET_DCXO_CONFIG                  (0)
#define WNDRV_CUST_FUNC1_SDIO_BLK_SIZE (512) /* Block Size of WIFI function */
#define WNDRV_CUST_SDIO_CLK_FREQ (22000) /* 22MHz */
#define WNDRV_CUST_SDIO_SEL_CARD 0x1 /* CARD1 */
#define WNDRV_CUST_DEC_PWR                          (3)
#define WNDRV_CUST_PWR_CTRL_TIMEOUT                 (20)

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef WLAN_STATUS(*PFN_OID_HANDLER_FUNC)(
    IN  P_ADAPTER_T prAdapter,
    IN  PVOID       pvBuf,
    IN  UINT_32     u4BufLen,
    OUT PUINT_32    pu4OutInfoLen
);

typedef enum _ENUM_CSUM_TYPE_T {
    CSUM_TYPE_IPV4,
    CSUM_TYPE_IPV6,
    CSUM_TYPE_TCP,
    CSUM_TYPE_UDP,
    CSUM_TYPE_NUM
} ENUM_CSUM_TYPE_T, *P_ENUM_CSUM_TYPE_T;

typedef enum _ENUM_CSUM_RESULT_T {
    CSUM_RES_NONE,
    CSUM_RES_SUCCESS,
    CSUM_RES_FAILED,
    CSUM_RES_NUM
} ENUM_CSUM_RESULT_T, *P_ENUM_CSUM_RESULT_T;

typedef enum _ENUM_PHY_MODE_T {
    ENUM_PHY_2G4_CCK,
    ENUM_PHY_2G4_OFDM_BPSK,
    ENUM_PHY_2G4_OFDM_QPSK,
    ENUM_PHY_2G4_OFDM_16QAM,
    ENUM_PHY_2G4_OFDM_48M,
    ENUM_PHY_2G4_OFDM_54M,
    ENUM_PHY_2G4_HT20_BPSK,
    ENUM_PHY_2G4_HT20_QPSK,
    ENUM_PHY_2G4_HT20_16QAM,
    ENUM_PHY_2G4_HT20_MCS5,
    ENUM_PHY_2G4_HT20_MCS6,
    ENUM_PHY_2G4_HT20_MCS7,
    ENUM_PHY_2G4_HT40_BPSK,
    ENUM_PHY_2G4_HT40_QPSK,
    ENUM_PHY_2G4_HT40_16QAM,
    ENUM_PHY_2G4_HT40_MCS5,
    ENUM_PHY_2G4_HT40_MCS6,
    ENUM_PHY_2G4_HT40_MCS7,
    ENUM_PHY_5G_OFDM_BPSK,
    ENUM_PHY_5G_OFDM_QPSK,
    ENUM_PHY_5G_OFDM_16QAM,
    ENUM_PHY_5G_OFDM_48M,
    ENUM_PHY_5G_OFDM_54M,
    ENUM_PHY_5G_HT20_BPSK,
    ENUM_PHY_5G_HT20_QPSK,
    ENUM_PHY_5G_HT20_16QAM,
    ENUM_PHY_5G_HT20_MCS5,
    ENUM_PHY_5G_HT20_MCS6,
    ENUM_PHY_5G_HT20_MCS7,
    ENUM_PHY_5G_HT40_BPSK,
    ENUM_PHY_5G_HT40_QPSK,
    ENUM_PHY_5G_HT40_16QAM,
    ENUM_PHY_5G_HT40_MCS5,
    ENUM_PHY_5G_HT40_MCS6,
    ENUM_PHY_5G_HT40_MCS7,
    ENUM_PHY_MODE_NUM
} ENUM_PHY_MODE_T, *P_ENUM_PHY_MODE_T;

typedef enum _ENUM_POWER_SAVE_POLL_MODE_T {
    ENUM_POWER_SAVE_POLL_DISABLE,
    ENUM_POWER_SAVE_POLL_LEGACY_NULL,
    ENUM_POWER_SAVE_POLL_QOS_NULL,
    ENUM_POWER_SAVE_POLL_NUM
} ENUM_POWER_SAVE_POLL_MODE_T, *P_ENUM_POWER_SAVE_POLL_MODE_T;

typedef enum _ENUM_AC_TYPE_T {
    ENUM_AC_TYPE_AC0,
    ENUM_AC_TYPE_AC1,
    ENUM_AC_TYPE_AC2,
    ENUM_AC_TYPE_AC3,
    ENUM_AC_TYPE_AC4,
    ENUM_AC_TYPE_AC5,
    ENUM_AC_TYPE_AC6,
    ENUM_AC_TYPE_BMC,
    ENUM_AC_TYPE_NUM
} ENUM_AC_TYPE_T, *P_ENUM_AC_TYPE_T;

typedef enum _ENUM_ADV_AC_TYPE_T {
    ENUM_ADV_AC_TYPE_RX_NSW,
    ENUM_ADV_AC_TYPE_RX_PTA,
    ENUM_ADV_AC_TYPE_RX_SP,
    ENUM_ADV_AC_TYPE_TX_PTA,
    ENUM_ADV_AC_TYPE_TX_RSP,
    ENUM_ADV_AC_TYPE_NUM
} ENUM_ADV_AC_TYPE_T, *P_ENUM_ADV_AC_TYPE_T;

typedef struct _SET_TXPWR_CTRL_T {
    INT_8    c2GLegacyStaPwrOffset;  /* Unit: 0.5dBm, default: 0*/
    INT_8    c2GHotspotPwrOffset;
    INT_8    c2GP2pPwrOffset;
    INT_8    c2GBowPwrOffset;
    INT_8    c5GLegacyStaPwrOffset;   /* Unit: 0.5dBm, default: 0*/
    INT_8    c5GHotspotPwrOffset;
    INT_8    c5GP2pPwrOffset;
    INT_8    c5GBowPwrOffset;
    UINT_8  ucConcurrencePolicy;   /* TX power policy when concurrence
                                                            in the same channel
                                                            0: Highest power has priority
                                                            1: Lowest power has priority */
    INT_8    acReserved1[3];            /* Must be zero */

    /* Power limit by channel for all data rates */
    INT_8    acTxPwrLimit2G[14];     /* Channel 1~14, Unit: 0.5dBm*/
    INT_8    acTxPwrLimit5G[4];       /* UNII 1~4 */
    INT_8    acReserved2[2];            /* Must be zero */
} SET_TXPWR_CTRL_T, *P_SET_TXPWR_CTRL_T;

/* For storing driver initialization value from glue layer */
typedef struct _REG_INFO_T {
    UINT_32     u4SdBlockSize;                      /* SDIO block size */
    UINT_32     u4SdBusWidth;                       /* SDIO bus width. 1 or 4 */
    UINT_32     u4SdClockRate;                      /* SDIO clock rate. (in unit of HZ) */
    UINT_32     u4StartAddress;                     /* Starting address of Wi-Fi Firmware */
    UINT_32     u4LoadAddress;                      /* Load address of Wi-Fi Firmware */
    UINT_16     aucFwImgFilename[65];               /* Firmware filename */
    UINT_32     u4StartFreq;                        /* Start Frequency for Ad-Hoc network : in unit of KHz */
    UINT_32     u4AdhocMode;                        /* Default mode for Ad-Hoc network : ENUM_PARAM_AD_HOC_MODE_T */
    UINT_32     u4RddStartFreq;
    UINT_32     u4RddTestMode;
    UINT_32     u4RddDebug;
    INT_32      i4HighRssiThreshold;
    INT_32      i4MediumRssiThreshold;
    INT_32      i4LowRssiThreshold;
    INT_32      au4TxPriorityTag[ENUM_AC_TYPE_NUM];
    INT_32      au4RxPriorityTag[ENUM_AC_TYPE_NUM];
    INT_32      au4AdvPriorityTag[ENUM_ADV_AC_TYPE_NUM];
    UINT_32     u4FastPSPoll;
    UINT_32     u4PTA;                              /* 0: disable, 1: enable */
    UINT_32     u4TXLimit;                          /* 0: disable, 1: enable */
    UINT_32     u4SilenceWindow;                    /* range: 100 - 625, unit: us */
    UINT_32     u4TXLimitThreshold;                 /* range: 250 - 1250, unit: us */
    UINT_32     u4PowerMode;
    UINT_32     fgEnArpFilter;
    UINT_32     u4PsCurrentMeasureEn;
    UINT_32     u4UapsdAcBmp;
    UINT_32     u4MaxSpLen;
    UINT_32     fgDisOnlineScan;                    /* 0: enable online scan, non-zero: disable online scan*/
    UINT_32     fgDisBcnLostDetection;                    /* 0: enable online scan, non-zero: disable online scan*/
    UINT_32     u4FixedRate;                        /* 0: automatic, non-zero: fixed rate */
    UINT_32     u4ArSysParam0;
    UINT_32     u4ArSysParam1;
    UINT_32     u4ArSysParam2;
    UINT_32     u4ArSysParam3;
    UINT_32     fgEnRoaming;                        /* 0:disable 1:enable roaming */

    // NVRAM - MP Data -START-
    UINT_8              aucMacAddr[6];
    UINT_16             au2CountryCode[4];          /* Country code (in ISO 3166-1 expression, ex: "US", "TW")  */
    TX_PWR_PARAM_T      rTxPwr;
    UINT_8              aucEFUSE[144];
    UINT_8              ucTxPwrValid;
    UINT_8              ucSupport5GBand;
    UINT_8              fg2G4BandEdgePwrUsed;
    INT_8               cBandEdgeMaxPwrCCK;
    INT_8               cBandEdgeMaxPwrOFDM20;
    INT_8               cBandEdgeMaxPwrOFDM40;
    DOMAIN_INFO_ENTRY   rDomainInfo;
    // NVRAM - MP Data -END-

    // NVRAM - Functional Data -START-
    UINT_8              uc2G4BwFixed20M;
    UINT_8              uc5GBwFixed20M;
    UINT_8              ucEnable5GBand;
    // NVRAM - Functional Data -END-

} REG_INFO_T, *P_REG_INFO_T;


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

P_ADAPTER_T
wlanAdapterCreate(
    IN P_GLUE_INFO_T prGlueInfo
);

VOID
wlanAdapterDestroy(
    IN P_ADAPTER_T prAdapter
);

VOID
wlanCardEjected(
    IN P_ADAPTER_T         prAdapter
);

VOID
wlanIST(
    IN P_ADAPTER_T prAdapter
);

BOOL
wlanISR(
    IN P_ADAPTER_T prAdapter,
    IN BOOLEAN fgGlobalIntrCtrl
);

WLAN_STATUS
wlanProcessCommandQueue(
    IN P_ADAPTER_T  prAdapter,
    IN P_QUE_T      prCmdQue
);

WLAN_STATUS
wlanSendCommand(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
);

VOID
wlanReleaseCommand(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
);

VOID
wlanReleasePendingCMDbyNetwork(
    IN P_ADAPTER_T  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

VOID
wlanReleasePendingOid(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4Data
);

VOID
wlanReturnPacket(
    IN P_ADAPTER_T prAdapter,
    IN PVOID pvPacket
);

WLAN_STATUS
wlanQueryInformation(
    IN P_ADAPTER_T          prAdapter,
    IN PFN_OID_HANDLER_FUNC pfOidQryHandler,
    IN PVOID                pvInfoBuf,
    IN UINT_32              u4InfoBufLen,
    OUT PUINT_32            pu4QryInfoLen
);

WLAN_STATUS
wlanSetInformation(
    IN P_ADAPTER_T          prAdapter,
    IN PFN_OID_HANDLER_FUNC pfOidSetHandler,
    IN PVOID                pvInfoBuf,
    IN UINT_32              u4InfoBufLen,
    OUT PUINT_32            pu4SetInfoLen
);


WLAN_STATUS
wlanAdapterStart(
    IN P_ADAPTER_T  prAdapter,
    IN P_REG_INFO_T prRegInfo,
    IN PVOID        pvFwImageMapFile,
    IN UINT_32      u4FwImageFileLength
);

WLAN_STATUS
wlanAdapterStop(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
wlanAdapterStopForAPorSTA(
    IN P_ADAPTER_T  prAdapter,
    IN kal_bool     fgIsAPmode
);

#ifdef __WAPI_SUPPORT__
BOOLEAN
wlanQueryWapiMode(
    IN P_ADAPTER_T          prAdapter
);
#endif

VOID
wlanReturnRxPacket(
    IN PVOID pvAdapter,
    IN PVOID pvPacket
);

VOID
wlanRxSetBroadcast(
    IN P_ADAPTER_T  prAdapter,
    IN BOOLEAN      fgEnableBroadcast
);

BOOLEAN
wlanIsHandlerNeedHwAccess(
    IN PFN_OID_HANDLER_FUNC pfnOidHandler,
    IN BOOLEAN              fgSetInfo
);

VOID
wlanSetPromiscuousMode(
    IN P_ADAPTER_T  prAdapter,
    IN BOOLEAN      fgEnablePromiscuousMode
);

#if CFG_ENABLE_FW_DOWNLOAD
WLAN_STATUS
wlanImageSectionDownload(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4DestAddr,
    IN UINT_32      u4ImgSecSize,
    IN PUINT_8      pucImgSecBuf
);

WLAN_STATUS
wlanImageSectionDownload_Temp(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4DestAddr,
    IN UINT_32      u4ImgSecSize,
    IN PUINT_8      pucImgSecBuf
);

#if !CFG_ENABLE_FW_DOWNLOAD_ACK
WLAN_STATUS
wlanImageQueryStatus(
    IN P_ADAPTER_T  prAdapter
);
#else
WLAN_STATUS
wlanImageSectionDownloadStatus(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucCmdSeqNum
);
#endif

WLAN_STATUS
wlanConfigWifiFunc(
    IN P_ADAPTER_T  prAdapter,
    IN BOOLEAN      fgEnable,
    IN UINT_32      u4StartAddress
);

UINT_32 wlanCRC32(
    PUINT_8 buf,
    UINT_32 len
);

#endif

WLAN_STATUS
wlanSendNicPowerCtrlCmd(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucPowerMode
);

BOOLEAN
wlanIsHandlerAllowedInRFTest(
    IN PFN_OID_HANDLER_FUNC pfnOidHandler,
    IN BOOLEAN              fgSetInfo
);

WLAN_STATUS
wlanProcessQueuedSwRfb(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfbListHead
);

WLAN_STATUS
wlanProcessQueuedMsduInfo(
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfoListHead
);

BOOLEAN
wlanoidTimeoutCheck(
    IN P_ADAPTER_T prAdapter,
    IN PFN_OID_HANDLER_FUNC pfnOidHandler
);

VOID
wlanoidClearTimeoutCheck(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
wlanUpdateNetworkAddress(
    IN P_ADAPTER_T prAdapter
);

BOOLEAN
wlanQueryTestMode(
    IN P_ADAPTER_T          prAdapter
);

/* Security Frame Handling */
BOOLEAN
wlanProcessSecurityFrame(
    IN P_ADAPTER_T      prAdapter,
    IN P_NATIVE_PACKET  prPacket
);

VOID
wlanSecurityFrameTxDone(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
);

VOID
wlanSecurityFrameTxTimeout(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
);

/*----------------------------------------------------------------------------*/
/* OID/IOCTL Handling                                                         */
/*----------------------------------------------------------------------------*/
VOID
wlanClearScanningResult(
    IN P_ADAPTER_T  prAdapter
);

VOID
wlanClearBssInScanningResult(
    IN P_ADAPTER_T      prAdapter,
    IN PUINT_8          arBSSID
);

#if CFG_TEST_WIFI_DIRECT_GO
VOID
wlanEnableP2pFunction(
    IN P_ADAPTER_T prAdapter
);

VOID
wlanEnableATGO(
    IN P_ADAPTER_T prAdapter
);
#endif


/*----------------------------------------------------------------------------*/
/* Address Retreive by Polling                                                */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryPermanentAddress(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* NIC Capability Retrieve by Polling                                         */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryNicCapability(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* Crystal Trimming Retrieve by Polling                                       */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryCrystalTrim(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* Loading Manufacture Data                                                   */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanLoadManufactureData(
    IN P_ADAPTER_T prAdapter,
    IN P_REG_INFO_T prRegInfo
);


/*----------------------------------------------------------------------------*/
/* Media Stream Mode                                                          */
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanResetMediaStreamMode(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* Timer Timeout Check (for Glue Layer)                                       */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanTimerTimeoutCheck(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* Mailbox Message Check (for Glue Layer)                                     */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanProcessMboxMessage(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* TX Pending Packets Handling (for Glue Layer)                               */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanEnqueueTxPacket(
    IN P_ADAPTER_T      prAdapter,
    IN P_NATIVE_PACKET  prNativePacket
);

WLAN_STATUS
wlanFlushTxPendingPackets(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
wlanTxPendingPackets(
    IN      P_ADAPTER_T prAdapter,
    IN OUT  PBOOLEAN    pfgHwAccess
);


/*----------------------------------------------------------------------------*/
/* Low Power Acquire/Release (for Glue Layer)                                 */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanAcquirePowerControl(
    IN P_ADAPTER_T prAdapter
);

WLAN_STATUS
wlanReleasePowerControl(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* Pending Packets Number Reporting (for Glue Layer)                          */
/*----------------------------------------------------------------------------*/
UINT_32
wlanGetTxPendingFrameCount(
    IN P_ADAPTER_T prAdapter
);


/*----------------------------------------------------------------------------*/
/* ACPI state inquiry (for Glue Layer)                                        */
/*----------------------------------------------------------------------------*/
ENUM_ACPI_STATE_T
wlanGetAcpiState(
    IN P_ADAPTER_T prAdapter
);

VOID
wlanSetAcpiState(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_ACPI_STATE_T ePowerState
);

#if CFG_SUPPORT_OSC_SETTING && defined(MT5931)
WLAN_STATUS
wlanSetMcuOscStableTime(
    IN P_ADAPTER_T      prAdapter,
    IN UINT_16          u2OscStableTime
);
#endif


/*----------------------------------------------------------------------------*/
/* TX target power control                                                    */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanUpdateTxPower(
    IN P_ADAPTER_T  prAdapter,
    IN INT_8        c11b_TxPower,
    IN INT_8        c11g_TxPower,
    IN INT_8        c11n_bw20_TxPower,
    IN INT_8        c11n_bw40_TxPower
);

VOID
wlanDefTxPowerCfg(
    IN P_ADAPTER_T      prAdapter
);

/*----------------------------------------------------------------------------*/
/* Whole chip reset                                                           */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanWholeChipReset(
    IN P_ADAPTER_T  prAdapter
);


/*----------------------------------------------------------------------------*/
/* MCR Read/Write                                                             */
/*----------------------------------------------------------------------------*/
UINT_32
wlanQueryMcr(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32 mcr_offset
);


WLAN_STATUS
wlanSetMcr(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32 mcr_offset,
    IN UINT_32 mcr_value
);


/*----------------------------------------------------------------------------*/
/* WPS utilities                                                              */
/*----------------------------------------------------------------------------*/
UINT_16
wlanCopyWPSAttr(
    IN PUINT_8 pWpsIE,
    IN PUINT_8 Dest,
    IN UINT_16 size
);

/*----------------------------------------------------------------------------*/
/* set preferred band configuration corresponding to network type             */
/*----------------------------------------------------------------------------*/
VOID
wlanSetPreferBandByNetwork(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BAND_T eBand,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
);

WLAN_STATUS
wlanSendPacket(
    IN P_GLUE_INFO_T                prGlueInfo,
    IN wndrv_pkt_descriptor_struct *wndrv_pkt_p
);

#ifndef CMD_PKT_SIZE_FOR_IMAGE
#define CMD_PKT_SIZE_FOR_IMAGE              2048
#endif

#ifdef __ICCARM__
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN extern  unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#else
extern ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#endif

#endif /* _WLAN_LIB_H */

