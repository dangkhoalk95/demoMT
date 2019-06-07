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
** $Id: //Project/WLAN/MT5921_BRANCHES/MT5921_11AW1112MP/include/os/wndrv_param.h#1 $
*/
/*******************************************************************************
GENERAL DESCRIPTION
********************
We define the external interfaces to the MediaTek 802.11 Wireless LAN
driver. These routines are only external to permit separate compilation.
Given a truely fast compiler they could all reside in a single file and
be static.
*/

#ifndef _WNDRV_PARAM_H
#define _WNDRV_PARAM_H

/*******************************************************************************
*                     C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                          C O N S T A N T S
********************************************************************************
*/
/* Return status (WLAN_STATUS) of functon call */
#define WLAN_STATUS_SUCCESS                     ((WLAN_STATUS) 0x00000000L)
#define WLAN_STATUS_PENDING                     ((WLAN_STATUS) 0x00000103L)
#define WLAN_STATUS_NOT_ACCEPTED                ((WLAN_STATUS) 0x00010003L)
#define WLAN_STATUS_MEDIA_CONNECT               ((WLAN_STATUS) 0x4001000BL)
#define WLAN_STATUS_MEDIA_DISCONNECT            ((WLAN_STATUS) 0x4001000CL)
#define WLAN_STATUS_MEDIA_SPECIFIC_INDICATION   ((WLAN_STATUS) 0x40010012L)

#define WLAN_STATUS_SCAN_COMPLETE               ((WLAN_STATUS) 0x60010001L)
#define WLAN_STATUS_HARD_ERRORS                 ((WLAN_STATUS) 0x80010004L)
#define WLAN_STATUS_FAILURE                     ((WLAN_STATUS) 0xC0000001L)
#define WLAN_STATUS_RESOURCES                   ((WLAN_STATUS) 0xC000009AL)
#define WLAN_STATUS_NOT_SUPPORTED               ((WLAN_STATUS) 0xC00000BBL)
#define WLAN_STATUS_DEVICE_FAILED               ((WLAN_STATUS) 0xC0010008L)
#define WLAN_STATUS_MULTICAST_FULL              ((WLAN_STATUS) 0xC0010009L)
#define WLAN_STATUS_REQUEST_ABORTED             ((WLAN_STATUS) 0xC001000CL)
#define WLAN_STATUS_RESET_IN_PROGRESS           ((WLAN_STATUS) 0xC001000DL)
#define WLAN_STATUS_INVALID_PACKET              ((WLAN_STATUS) 0xC001000FL)
#define WLAN_STATUS_ADAPTER_NOT_READY           ((WLAN_STATUS) 0xC0010011L)
#define WLAN_STATUS_INVALID_LENGTH              ((WLAN_STATUS) 0xC0010014L)
#define WLAN_STATUS_INVALID_DATA                ((WLAN_STATUS) 0xC0010015L)
#define WLAN_STATUS_BUFFER_TOO_SHORT            ((WLAN_STATUS) 0xC0010016L)
#define WLAN_STATUS_NO_CABLE                    ((WLAN_STATUS) 0xC001001FL)

/* Packet filter bit definitioin (UINT_32 bit-wise definition) */
#define PARAM_PACKET_FILTER_DIRECTED            0x00000001
#define PARAM_PACKET_FILTER_MULTICAST           0x00000002
#define PARAM_PACKET_FILTER_ALL_MULTICAST       0x00000004
#define PARAM_PACKET_FILTER_BROADCAST           0x00000008
#define PARAM_PACKET_FILTER_PROMISCUOUS         0x00000020
#define PARAM_PACKET_FILTER_ALL_LOCAL           0x00000080

/*******************************************************************************
*                         D A T A   T Y P E S
***************************************************************************************
*/
typedef UINT_8          PARAM_MAC_ADDRESS[6];

/*--------------------------------------------------------------*/
/* Struct definition to indicate specific event.                */
/*--------------------------------------------------------------*/
typedef enum _PARAM_STATUS_TYPE {
    Param_StatusType_Authentication,
    Param_StatusType_MediaStreamMode,
    Param_StatusType_PMKID_CandidateList,
    Param_StatusType_DWMax = 65536                     /* Upper bound, not real case */
} PARAM_STATUS_TYPE, *PPARAM_STATUS_TYPE;

typedef struct _PARAM_STATUS_INDICATION {
    PARAM_STATUS_TYPE   StatusType;
} PARAM_STATUS_INDICATION, *PPARAM_STATUS_INDICATION;

/*-------------------------------------------------*/
typedef struct _PARAM_AUTH_REQUEST {
    UINT_32             Length;             /* Length of this struct */
    PARAM_MAC_ADDRESS   Bssid;
    UINT_32             Flags;              /* Definitions are as follows */
} PARAM_AUTH_REQUEST, *PPARAM_AUTH_REQUEST;

#define PARAM_AUTH_REQUEST_REAUTH               0x01
#define PARAM_AUTH_REQUEST_KEYUPDATE            0x02
#define PARAM_AUTH_REQUEST_PAIRWISE_ERROR       0x06
#define PARAM_AUTH_REQUEST_GROUP_ERROR          0x0E

typedef struct _PARAM_AUTH_EVENT {
    PARAM_STATUS_INDICATION     Status;
    PARAM_AUTH_REQUEST          Request[1];
} PARAM_AUTH_EVENT, *PPARAM_AUTH_EVENT;

/*-------------------------------------------------*/

/* PMKID candidate lists. */
typedef struct _PARAM_PMKID_CANDIDATE {
    PARAM_MAC_ADDRESS   BSSID;
    UINT_32             Flags;
} PARAM_PMKID_CANDIDATE, *PPARAM_PMKID_CANDIDATE;

/* Flags for PMKID Candidate list structure */
#define PARAM_PMKID_CANDIDATE_PREAUTH_ENABLED   0x01

typedef struct _PARAM_PMKID_CANDIDATE_LIST {
    UINT_32             Version;            /* Version */
    UINT_32             NumCandidates;      /* How many candidates follow */
    PARAM_PMKID_CANDIDATE   CandidateList[1];
} PARAM_PMKID_CANDIDATE_LIST, *PPARAM_PMKID_CANDIDATE_LIST;

/*--------------------------------------------------------------*/
/* Set/Query network type list                                  */
/*--------------------------------------------------------------*/
/* Network types include OFDM 5G(a) and 2.4G(g) */
typedef enum _PARAM_NETWORK_TYPE {
    Param_FH,
    Param_DS,
    Param_OFDM5,
    Param_OFDM24,
    Param_Automode,
    Param_NetworkTypeMax,                    /* Upper bound, not real case */
    Param_NetworkTypeDWMax = 65536
} PARAM_NETWORK_TYPE, *PPARAM_NETWORK_TYPE;

#if 0 /* Defined in wlan_oid.h */
typedef struct _PARAM_NETWORK_TYPE_LIST {
    UINT_32             NumberOfItems;      /* At least 1 */
    PARAM_NETWORK_TYPE  NetworkType [1];
} PARAM_NETWORK_TYPE_LIST, *PPARAM_NETWORK_TYPE_LIST;
#endif

/*--------------------------------------------------------------*/
/* Set/Query power saving mode                                  */
/*--------------------------------------------------------------*/

#if 0 /* Defined in wlan_oid.h */
typedef enum _PARAM_POWER_MODE {
    Param_PowerModeCAM,
    Param_PowerModeMAX_PSP,
    Param_PowerModeFast_PSP,
    Param_PowerModeMax                      /* Upper bound, not real case */
} PARAM_POWER_MODE, *PPARAM_POWER_MODE;
#endif

/* For wlanpmSetTriggerEvent() and wlanpmQueryTriggerEvent */
typedef enum _PARAM_POWER_TRIGGER {
    Param_PowerTrigger_802_11,
    Param_PowerTrigger_IPv4_ARP,
    Param_PowerTrigger_Next_Time,
    Param_PowerTriggerMax,                   /* Upper bound, not real case */
    Param_PowerTriggerDWMax = 65536
} PARAM_POWER_TRIGGER, *PPARAM_POWER_TRIGGER;

typedef struct _PARAM_POWER_TRIGGER_INFO {
    PARAM_POWER_TRIGGER TriggerType;
    UINT_32             TriggerTime;        /* This value is based on kalGetTimeTick() */
    UINT_32             NumIPv4Addr;
    UINT_32             IPv4Addr[1];
} PARAM_POWER_TRIGGER_INFO, *PPARAM_POWER_TRIGGER_INFO;

/* Received signal strength indication (in unit of dBm) */
typedef INT_32          PARAM_RSSI;

/* In unit of milliwatts */
typedef UINT_32         PARAM_TX_POWER_LEVEL;

/*--------------------------------------------------------------*/
/* Set/Query basic 802.11 configuration.                        */
/*--------------------------------------------------------------*/
typedef struct _PARAM_802_11_CONFIG_FH {
    UINT_32             Length;             /* Length of structure */
    UINT_32             HopPattern;         /* Defined as 802.11 */
    UINT_32             HopSet;             /* to one if non-802.11 */
    UINT_32             DwellTime;          /* In unit of Kusec */
} PARAM_802_11_CONFIG_FH, *PPARAM_802_11_CONFIG_FH;

typedef struct _PARAM_802_11_CONFIG {
    UINT_32             Length;             /* Length of structure */
    UINT_32             BeaconPeriod;       /* In unit of Kusec */
    UINT_32             ATIMWindow;         /* In unit of Kusec */
    UINT_32             DSConfig;           /* Channel frequency in unit of kHz */
    PARAM_802_11_CONFIG_FH    FHConfig;
} PARAM_802_11_CONFIG, *PPARAM_802_11_CONFIG;

/*--------------------------------------------------------------*/
/* Query statistics of transmited and received packets          */
/*--------------------------------------------------------------*/
typedef struct _PARAM_STATISTICS {
    UINT_32             Length;             /* Length of structure */
    LARGE_INTEGER       TransmittedFragmentCount;
    LARGE_INTEGER       MulticastTransmittedFrameCount;
    LARGE_INTEGER       FailedCount;
    LARGE_INTEGER       RetryCount;
    LARGE_INTEGER       MultipleRetryCount;
    LARGE_INTEGER       RTSSuccessCount;
    LARGE_INTEGER       RTSFailureCount;
    LARGE_INTEGER       ACKFailureCount;
    LARGE_INTEGER       FrameDuplicateCount;
    LARGE_INTEGER       ReceivedFragmentCount;
    LARGE_INTEGER       MulticastReceivedFrameCount;
    LARGE_INTEGER       FCSErrorCount;
    LARGE_INTEGER       TKIPLocalMICFailures;
    LARGE_INTEGER       TKIPRemoteMICErrors;
    LARGE_INTEGER       TKIPICVErrors;
    LARGE_INTEGER       TKIPCounterMeasuresInvoked;
    LARGE_INTEGER       TKIPReplays;
    LARGE_INTEGER       CCMPFormatErrors;
    LARGE_INTEGER       CCMPReplays;
    LARGE_INTEGER       CCMPDecryptErrors;
    LARGE_INTEGER       FourWayHandshakeFailures;
} PARAM_STATISTICS, *PPARAM_STATISTICS;

/*--------------------------------------------------------------*/
/* Set/Query encryption key                                     */
/*--------------------------------------------------------------*/
typedef UINT_32         PARAM_KEY_INDEX;
typedef UINT_64         PARAM_KEY_RSC;

/* Key mapping of BSSID */
typedef struct _PARAM_KEY {
    UINT_32             Length;             /* Length of structure */
    UINT_32             KeyIndex;           /* KeyID */
    UINT_32             KeyLength;          /* Key length in bytes */
    PARAM_MAC_ADDRESS   BSSID;              /* MAC address */
    PARAM_KEY_RSC       KeyRSC;
    UINT_8              KeyMaterial[1];     /* Key content by above setting */
} PARAM_KEY, *PPARAM_KEY;

typedef struct _PARAM_REMOVE_KEY {
    UINT_32             Length;             /* Length of structure */
    UINT_32             KeyIndex;           /* KeyID */
    PARAM_MAC_ADDRESS   BSSID;              /* MAC address */
} PARAM_REMOVE_KEY, *PPARAM_REMOVE_KEY;

typedef struct _PARAM_WEP {
    UINT_32             Length;             /* Length of structure */
    UINT_32             KeyIndex;           /* 0: pairwise key, others group keys */
    UINT_32             KeyLength;          /* Key length in bytes */
    UINT_8              KeyMaterial[1];     /* Key content by above setting */
} PARAM_WEP, *PPARAM_WEP;

/*--------------------------------------------------------------*/
/* Set/Query network operation mode.                            */
/*--------------------------------------------------------------*/
typedef enum _PARAM_NETWORK_INFRASTRUCTURE {
    Param_NetTypeIBSS,                      /* Ad Hoc */
    Param_NetTypeInfra,                     /* Infrastructure STA */
    Param_NetTypeAutoUnknown,               /* Automatic to join BSS */
    Param_NetTypeInfraMax,                   /* Upper bound, not real case */
    Param_NetTypeInfraDWMax = 65536
} PARAM_NETWORK_INFRASTRUCTURE, *PPARAM_NETWORK_INFRASTRUCTURE;

/*--------------------------------------------------------------*/
/* Set/Query authentication mode.                               */
/*--------------------------------------------------------------*/
/* Authentication modes */
typedef enum _PARAM_AUTH_MODE {
    Param_AuthModeOpen,                     /* Open system */
    Param_AuthModeShared,                   /* Shared key */
    Param_AuthModeAutoSwitch,               /* Either open system or shared key */
    Param_AuthModeWPA,
    Param_AuthModeWPAPSK,
    Param_AuthModeWPANone,                  /* For Ad hoc */
    Param_AuthModeWPA2,
    Param_AuthModeWPA2PSK,
    Param_AuthModeMax,                       /* Upper bound, not real case */
    Param_AuthModeDWMax = 65536
} PARAM_AUTH_MODE, *PPARAM_AUTH_MODE;

/*--------------------------------------------------------------*/
/* Set/Query data rates.                                        */
/*--------------------------------------------------------------*/
#define PARAM_MAX_LEN_RATES                     8
#define PARAM_MAX_LEN_RATES_EX                  16

typedef UINT_8              PARAM_RATES[PARAM_MAX_LEN_RATES];
typedef UINT_8              PARAM_RATES_EX[PARAM_MAX_LEN_RATES_EX];

/*--------------------------------------------------------------*/
/* Set/Query SSID setting                                       */
/*--------------------------------------------------------------*/
#define PARAM_MAX_LEN_SSID                      32
#if 0 /* move to wlan_oid.h */
typedef struct _PARAM_SSID_T {
    UINT_32  u4SsidLen;      /* SSID length in bytes */
    /* Zero length is broadcast(any) SSID */
    UINT_8   aSsid[PARAM_MAX_LEN_SSID];
} PARAM_SSID_T, *PPARAM_SSID_T;
#endif
/*--------------------------------------------------------------*/
/* Query scan result.                                           */
/*--------------------------------------------------------------*/
typedef struct _PARAM_BSSID {
    UINT_32                         Length;             /* Length of structure */
    PARAM_MAC_ADDRESS               MacAddress;         /* BSSID */
    UINT_8                          Reserved[2];
    PARAM_SSID_T                    Ssid;               /* SSID */
    UINT_32                         Privacy;            /* Need WEP encryption */
    PARAM_RSSI                      Rssi;               /* in dBm */
    PARAM_NETWORK_TYPE              NetworkTypeInUse;
    PARAM_802_11_CONFIG             Configuration;
    PARAM_NETWORK_INFRASTRUCTURE    InfrastructureMode;
    PARAM_RATES                     SupportedRates;
} PARAM_BSSID, *PPARAM_BSSID;

typedef struct _PARAM_BSSID_LIST {
    UINT_32             NumberOfItems;                  /* at least 1 */
    PARAM_BSSID         Bssid[1];
} PARAM_BSSID_LIST, *PPARAM_BSSID_LIST;

/* Capabilities, privacy, rssi and IEs of each BSSID */
typedef struct _PARAM_BSSID_EX {
    UINT_32                         Length;             /* Length of structure */
    PARAM_MAC_ADDRESS               MacAddress;         /* BSSID */
    UINT_8                          Reserved[2];
    PARAM_SSID_T                    Ssid;               /* SSID */
    UINT_32                         Privacy;            /* Need WEP encryption */
    PARAM_RSSI                      Rssi;               /* in dBm */
    PARAM_NETWORK_TYPE              NetworkTypeInUse;
    PARAM_802_11_CONFIG             Configuration;
    PARAM_NETWORK_INFRASTRUCTURE    InfrastructureMode;
    PARAM_RATES_EX                  SupportedRates;
    UINT_32                         IELength;
    UINT_8                          IEs[1];
} PARAM_BSSID_EX, *PPARAM_BSSID_EX;

#if 0 /* Defined in wlan_oid.h */
typedef struct _PARAM_BSSID_LIST_EX {
    UINT_32                         NumberOfItems;      /* at least 1 */
    PARAM_BSSID_EX                  Bssid[1];
} PARAM_BSSID_LIST_EX, *PPARAM_BSSID_LIST_EX;
#endif
/*--------------------------------------------------------------*/
/* Threshold of some parameters                                 */
/*--------------------------------------------------------------*/
typedef  UINT_32        PARAM_FRAGMENTATION_THRESHOLD;
typedef  UINT_32        PARAM_RTS_THRESHOLD;

/*--------------------------------------------------------------*/
/* Set/Query encryption mode.                                   */
/*--------------------------------------------------------------*/
/* Encryption types */
typedef enum _PARAM_WEP_STATUS {
    Param_WEPEnabled,
    Param_Encryption1Enabled = Param_WEPEnabled,
    Param_WEPDisabled,
    Param_EncryptionDisabled = Param_WEPDisabled,
    Param_WEPKeyAbsent,
    Param_Encryption1KeyAbsent = Param_WEPKeyAbsent,
    Param_WEPNotSupported,
    Param_EncryptionNotSupported = Param_WEPNotSupported,
    Param_Encryption2Enabled,
    Param_Encryption2KeyAbsent,
    Param_Encryption3Enabled,
    Param_Encryption3KeyAbsent,
    Param_EncryptionKeyDWMAX = 65536
} PARAM_WEP_STATUS, *PPARAM_WEP_STATUS,
PARAM_ENCRYPTION_STATUS, *PPARAM_ENCRYPTION_STATUS;
#if 0
typedef enum _PARAM_RELOAD_DEFAULTS {
    Param_ReloadWEPKeys
} PARAM_RELOAD_DEFAULTS, *PPARAM_RELOAD_DEFAULTS;
#endif

/*--------------------------------------------------------------*/
/* Set/Query authentication and encryption capability           */
/*--------------------------------------------------------------*/
typedef UINT_8   PARAM_PMKID_VALUE[16];

typedef struct _PARAM_BSSID_INFO {
    PARAM_MAC_ADDRESS   BSSID;
    PARAM_PMKID_VALUE   PMKID;
} PARAM_BSSID_INFO, *PPARAM_BSSID_INFO;

typedef struct _PARAM_PMKID {
    UINT_32             Length;
    UINT_32             BSSIDInfoCount;
    PARAM_BSSID_INFO    BSSIDInfo[1];
} PARAM_PMKID, *PPARAM_PMKID;

typedef struct _PARAM_AUTH_ENCRYPTION {
    PARAM_AUTH_MODE         AuthModeSupported;
    PARAM_ENCRYPTION_STATUS EncryptStatusSupported;
} PARAM_AUTH_ENCRYPTION, *PPARAM_AUTH_ENCRYPTION;

typedef struct _PARAM_CAPABILITY {
    UINT_32                Length;
    UINT_32                Version;
    UINT_32                NoOfPMKIDs;
    UINT_32                NoOfAuthEncryptPairsSupported;
    PARAM_AUTH_ENCRYPTION  AuthenticationEncryptionSupported[1];
} PARAM_CAPABILITY, *PPARAM_CAPABILITY;

/*--------------------------------------------------------------*/
/* Set/Query testing type                                       */
/*--------------------------------------------------------------*/
typedef struct _PARAM_802_11_TEST {
    UINT_32             Length;
    UINT_32             Type;
    union {
        PARAM_AUTH_EVENT    AuthenticationEvent;
        PARAM_RSSI          RssiTrigger;
    } u;
} PARAM_802_11_TEST, *PPARAM_802_11_TEST;

/*--------------------------------------------------------------*/
/* Set/Query device power states.                               */
/*--------------------------------------------------------------*/
#if 0 /* defined in wlan_oid.h */
typedef enum _PARAM_DEVICE_POWER_STATE {
    ParamDeviceStateUnspecified = 0,
    ParamDeviceStateD0,
    ParamDeviceStateD1,
    ParamDeviceStateD2,
    ParamDeviceStateD3,
    ParamDeviceStateMaximum
} PARAM_DEVICE_POWER_STATE, *PPARAM_DEVICE_POWER_STATE;
#endif

typedef struct _IPC_SUPPORTED_CH_LIST {
    UINT_32             numberOfItems;
    UINT_8              channel[64];
} IPC_SUPPORTED_CH_LIST, *PIPC_SUPPORTED_CH_LIST;

/* The enumeration definitions for OID_IPN_MULTI_DOMAIN_CAPABILITY */
typedef enum _IPN_MULTI_DOMAIN_CAPABILITY {
    multiDomainCapDisabled,
    multiDomainCapEnabled,
    multiDomainCapDWMAX = 65536
} IPN_MULTI_DOMAIN_CAPABILITY, *PIPN_MULTI_DOMAIN_CAPABILITY;

#if 0 /* move to wlan_oid.h */
typedef struct _COUNTRY_STRING_ENTRY {
    UINT_8              countryCode[2];
    UINT_8              environmentCode[2];
} COUNTRY_STRING_ENTRY, *PCOUNTRY_STRING_ENTRY;
#endif

typedef enum _IPC_PHY_PREAMBLE_TYPE {
    phyPreambleTypeLong,
    phyPreambleTypeShort,
    phyPreambleTypeAuto,
    phyPreambleTypeDWMAX = 65536
} IPC_PHY_PREAMBLE_TYPE, *PIPC_PHY_PREAMBLE_TYPE;

typedef struct _IPC_MCR_RW_STRUC {
    BOOLEAN             bRead;              /* read: 1, write: 0 */
    UINT_32             mcrIndex;
    UINT_32             mcrData;
} IPC_MCR_RW_STRUC, *PIPC_MCR_RW_STRUC;

typedef struct _IPC_BBCR_RW_STRUC {
    BOOLEAN             bRead;              /* read: 1, write: 0 */
    UINT_8              bbcrIndex;
    UINT_8              bbcrData;
} IPC_BBCR_RW_STRUC, *PIPC_BBCR_RW_STRUC;

typedef struct _IPC_EEPROM_RW_STRUC {
    BOOLEAN             bRead;              /* read: 1, write: 0 */
    UINT_8              ucEEPROMMethod;
    UINT_8              eepromIndex;
    UINT_16             eepromData;
} IPC_EEPROM_RW_STRUC, *PIPC_EEPROM_RW_STRUC;

/* RF Test for rx status */
typedef struct _TEST_RX_STATUS_STRUC {
    UINT_32             intRxOk;            /* number of packets that Rx ok from interrupt */
    UINT_32             intCrcErr;          /* number of packets that CRC error from interrupt */
    UINT_32             intShort;           /* number of packets that is short preamble from interrupt */
    UINT_32             intLong;            /* number of packets that is long preamble from interrupt */
    UINT_32             pauRxPktCount;      /* number of packets that Rx ok from PAU */
    UINT_32             pauCrcErrCount;     /* number of packets that CRC error from PAU */
    UINT_32             pauRxFifoFullCount; /* number of packets that is short preamble from PAU */
    UINT_32             pauCCACount;        /*CCA rising edge count */
    /*WCP modification*/  UINT_32   intLongPreambleNum;
    /*WCP modification*/  UINT_32   intShortPreambleNum;
    /*WCP modification*/  UINT_32   intRateOkNum[ WNDRV_RATE_COUNT ];
    /*WCP modification*/  UINT_32   intRateCrcErrNum[ WNDRV_RATE_COUNT ];
    /*WCP modification*/  INT_32    intRssiMax;
    /*WCP modification*/  INT_32    intRssiMin;
    /*WCP modification*/  INT_32    intRssiMean;
    /*WCP modification*/  INT_32    intRssiVariance;
    /*WCP modification*/  UINT_32   packetSize[ WNDRV_PACKET_SIZE_COUNT];
    /*WCP modification*/  UINT_32   packetType[ WNDRV_PACKET_TYPE_COUNT];
} TEST_RX_STATUS_STRUC, *PTEST_RX_STATUS_STRUC;

/* RF test RSSI statistic */
typedef struct _RX_RSSI_STRUC {
    INT                 rssiMean;           /* RSSI mean, mean of BBCR62 */
    INT                 rssiVariance;       /* RSSI variance, mean of |CR62-mean(CR62)| */
} RX_RSSI_STRUC, *PRX_RSSI_STRUC;

/* MediaTek WEP statistics */
typedef struct _IPC_WEP_STATISTICS {
    UINT_32             length;             /* Length of the IPC_WEP_STATISTICS structure in bytes */
    UINT_32             wepIcvErrorCount;
    UINT_32             wepExcludedCount;
    UINT_32             wepUndecryptableCount;
} __KAL_ATTRIB_PACKED__ IPC_WEP_STATISTICS, *PIPC_WEP_STATISTICS;

/* WCP modification, #pragma pack(1) */
/* MTK HW statistics */
typedef struct _MTK_HW_STATISTICS {
    UINT_32             length;             /* Length of the MTK_HW_STATISTICS structure in bytes */
    UINT_64             rxFcsErrorCount;
    UINT_64             rxFifoFullCount;
    UINT_64             rxPacketCount;
    UINT_64             channelIdleCount;
    UINT_64             ccaActiveCount;
    UINT_64             ccaSampledActiveCount;
    UINT_64             rtsRetryCount;
    UINT_64             frameRetryCount;
    UINT_64             txFrameCount;
    UINT_64             txBeaconCount;
} __KAL_ATTRIB_PACKED__ MTK_HW_STATISTICS, *PMTK_HW_STATISTICS;
/* WCP modification, #pragma pack() */
/* MTK HW statistics */
#if 0 /* move to gl_os.h in MT5921 */
typedef struct _CONNECTION_STATISTICS {
    UINT_32  associateCount;
    UINT_32  reassociateCount;
    UINT_32  authenticationCount;
    UINT_32  deauthenticationCount;
    UINT_32  associationType;                           /* 0: idle  1: associate */
    UINT_32  associateTime;
} CONNECTION_STATISTICS, *PCONNECTION_STATISTICS;
#endif
/* Debug level and Bebug Module setting */
typedef struct _DEBUG_LEVELMODULES_STRUC {
    UINT_32             dwLevel;
    UINT_32             dwModules;
} DEBUG_LEVELMODULES_STRUC, *PDEBUG_LEVELMODULES_STRUC;

/*WCP modification*///#define MAX_BUF_SZ                              2048
/*WCP modification*/ #define MAX_BUF_SZ                              2000
/*WCP modification*/ //#define UINT UINT32

typedef struct _TX_PACKET_V0_STRUC {
    UINT_32                pktLength;                  /* Length of the Tx Packet */
    UINT_8              pktContent[MAX_BUF_SZ];  /* Content of the Tx Packet */
    UINT_32             pktStatus;
    UINT_32             pktCount;
    UINT_32                pktInterval;                /* Interval between each Tx Packet */
    UINT_8              txPower;
    UINT_8              txAnt;
} TX_PACKET_V0_STRUC, *PTX_PACKET_V0_STRUC;

#define TX_PACKET_FLAG_ENABLE_ALC_TRACK         BIT(0)
#define TX_PACKET_FLAG_TARGET_ALC_PROVIDE       BIT(1)
#define TX_PACKET_FLAG_NO_TXGAIN                BIT(2)     /* TX Packet Struc has no gain informatioin. txGain must be set as 0xFF */

typedef struct _TX_PACKET_STRUC {
    UINT_32                pktLength;               /* Length of the Tx Packet */
    UINT_8              pktContent[MAX_BUF_SZ];  /* Content of the Tx Packet */
    UINT_32             pktStatus;
    UINT_32             pktCount;
    UINT_32                pktInterval;             /* Interval between each Tx Packet */
    UINT_8              txGain;                  /* Gain control value */
    UINT_8              txAnt;                   /* No longer used. use OID_802_11_TX_ANTENNA_SELECTED */
    UINT_8              reserved[2];
    UINT_32             txFlags;
    UINT_32             targetAlc;
    //20081021 add by saker
    BOOL                is_short_preamble; /* 0 for long preamble and 1 for short preamble */
    //20081023 add by saker
    UINT_32             txRate;
} TX_PACKET_STRUC, *PTX_PACKET_STRUC;

/* Original TX status. There is no ALC field. */
typedef struct _TX_STATUS_V0_STRUC {
    UINT_32             pktSentStatus;
    UINT_32             pktSentCount;
} TX_STATUS_V0_STRUC, *PTX_STATUS_V0_STRUC;

typedef struct _TX_STATUS_STRUC {
    UINT_32             pktSentStatus;
    UINT_32             pktSentCount;
    UINT_16             avgAlc;
    UINT_8              cckGainControl;
    UINT_8              ofdmGainControl;
} TX_STATUS_STRUC, *PTX_STATUS_STRUC;

typedef struct _PWR_MGT_STRUC {
    UINT_32             pwrMode;
} PWR_MGT_STRUC, *PPWR_MGT_STRUC;

/* This supports static U-APSD setting. If per AC setting is required, TSpec should be used. */
typedef struct _PARAM_QOS_UAPSD_PS_MODE {
    UINT_32     length;             /* Length of structure */
    UINT_32     servicePeriod;      /* service period. Only 0, 2, 4 and 6 are allowed */
    UINT_32     acCategory[4];      /* 4 AC's setting. 0 for BE, 1 for BK, 2 for VI and 3 for VO */
    /* BIT(0): 1 Legacy 0 UAPSD(Trigger/Delivery enabled) Others reserved. */
} PARAM_QOS_UAPSD_PS_MODE, *PPARAM_QOS_UAPSD_PS_MODE;
typedef enum _PARAM_BACKGROUND_SCAN_TYPE {
    Param_BackGroundScanType_ScanWhenDisconnect,
    Param_BackGroundScanType_UpdateBssInfo,         /* Only scan known channels with BSS records */
    Param_BackGroundScanType_BuildSsidList,
    Param_BackGroundScanTypeMax,                     /* Upper bound, not real case */
    Param_BackGroundScanTypeDWMax = 65536
} PARAM_BACKGROUND_SCAN_TYPE, *PPARAM_BACKGROUND_SCAN_TYPE;

typedef enum _PARAM_SCAN_TYPE {
    Param_ScanType_Auto,
    Param_ScanType_ActiveScan,
    Param_ScanType_PassiveScan,
    Param_ScanTypeMax,                     /* Upper bound, not real case */
    Param_ScanTypeDWMax = 65536
} PARAM_SCAN_TYPE, *PPARAM_SCAN_TYPE;

typedef struct _PARAM_SCAN_CHANNEL_LIST {
    UINT_32     Length;             /* Length of structure */
    UINT_32     frequencies[1];     /* Frequency in unit of frequency kHz */
} PARAM_SCAN_CHANNEL_LIST, *PPARAM_SCAN_CHANNEL_LIST;

typedef struct _PARAM_BACKGROUND_SCAN_INFO {
    UINT_32                 Length;       /* Length of structure */
    UINT_32                 enable;        /* TRUE: Enable BG scan. FALSE: else */
    UINT_32                 bgScanType;   /* PARAM_BACKGROUND_SCAN_TYPE */
    UINT_32                 scanType;     /* PARAM_SCAN_TYPE */
    UINT_32                 minIdleTime;  /* in unit of msec */
    UINT_32                 maxIdleTime;  /* in unit of msec */
    UINT_32                 scanInterval; /* in unit of msec */
    PARAM_SCAN_CHANNEL_LIST channelList;  /* If Length is sizeof(UINT_32), driver has no
                                               restriction which channel to scan. If bgScanType =
                                               Param_BackGroundScanType_UpdateBssInfo, this field
                                               has no use. */
} PARAM_BACKGROUND_SCAN_INFO, *PPARAM_BACKGROUND_SCAN_INFO;

typedef struct _PARAM_EXPLICIT_SCAN_INFO {
    UINT_32                 Length;       /* Length of structure */
    UINT_32                 enable;     /* TRUE: overwrite driver's scan setting. FALSE: driver set */
    UINT_32                 scanType;     /* PARAM_SCAN_TYPE */
    UINT_32                 probeDelay;   /* in unit of msec */
    UINT_32                 minIdleTime;  /* in unit of msec */
    UINT_32                 maxIdleTime;  /* in unit of msec */
    UINT_32                 trafficAllowed; /* TRUE, if traffic is allowed and scan will be temporarly stopped */
    PARAM_SCAN_CHANNEL_LIST channelList;  /* If Length is sizeof(UINT_32), driver has no restriction whhich channel to scan */
} PARAM_EXPLICIT_SCAN_INFO, *PPARAM_EXPLICIT_SCAN_INFO;

#define ROAM_OUT_FLAG_RSSI_THRESHOLD BIT(0)
#define ROAM_OUT_FLAG_LOST_BEACON    BIT(1)
#define ROAM_OUT_FLAG_RETRY_FAIL     BIT(2)

typedef struct _PARAM_ROAM_CONFIG {
    UINT_32    Length;       /* Length of structure */
    UINT_32    roamFlag;
    UINT_8     minRssiCalDuration; /* in unit of sec */
    UINT_8     roamOutRssiTrigger; /* in unit of dBm */
    UINT_16    roamOutNumOfLostBeacon;
    UINT_32    roamOutTxRetryLimit;
    UINT_32    minRoamOutStayDuration; /* in unit of sec */
    UINT_32    minScanIntervalLostConnection; /* in unit of sec */
} PARAM_ROAM_CONFIG, *PPARAM_ROAM_CONFIG;

/*******************************************************************************
*                             M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                    D A T A   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                 F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

#endif  /* _WNDRV_PARAM_H */

