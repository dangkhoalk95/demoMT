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
** $Id: //Project/WLAN/MT5921_BRANCHES/MT5921_11AW1112MP/include/os/wlan_kal.h#1 $
*/

#ifndef __WLAN_KAL_H
#define __WLAN_KAL_H

/*******************************************************************************
*                     C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
//#include "wndrv_typedef.h"

/*******************************************************************************
*                          C O N S T A N T S
********************************************************************************
*/
/* If the Ethernet backbone is considered, the RX_SKB_BUF_SZ must be
 * at least 1586 bytes (1544+20+20+2), which includes encryption(20),
 * driver reserved space(20=12 + 8 bubble) and VLAN Tag(2).
 */

#if 0
#define RX_SKB_BUF_SZ               1600

#if BUILD_WMM
#define MAX_PKT_NUM                 32
#define MAX_NUM_MSDU_INFO           32      /* Larger than NUM_MSDU_INFO_MGMT
                                             * plus max activeQ length
                                             */
#else
#define MAX_PKT_NUM                 10
#define MAX_NUM_MSDU_INFO           10      /* Larger than NUM_MSDU_INFO_MGMT
                                             * plus max activeQ length */
#endif
#define MAX_NUM_STA_RECORD          8

#define NUM_MSDU_INFO_MGMT          4       /* Smaller than MAX_NUM_MSDU_INFO */
#define WLAN_MAX_PKT_SZ             1572    /* Changed by CMC 5/5 */

/* Polling interval for handling network connection */
#define CONNECTION_HANDLING_INTERVAL        500     /* in milliseconds */

/* Following are for Scan parameter */
/* Maximum number of scan results */
#define MAX_NUM_SCAN_RESULTS           16

/* MAX (120 + 12) * 14 = 1848 TU almost Windows's 2 sec requirement */
#define SCAN_MIN_CH_TIME                12     /* MinChannelTime: 12 TU */
#define SCAN_MAX_CH_TIME                24     /* MaxChannelTime for full scanning */
#define BACKGROUND_SCAN_MIN_CH_TIME      7     /* Background Scan MinChannelTime: 7 TU */
#define BACKGROUND_SCAN_MAX_CH_TIME     11     /* Background Scan MaxChannelTime for full scanning */


/* This number should be less than NUM_MSDU_INFO_MGMT */
#define SCAN_MAX_NUM_PROBE_REQ_PER_CH                3       /* max number of Probe Req per channel */
#define BACKGROUND_SCAN_MAX_NUM_PROBE_REQ_PER_CH     1       /* max number of Probe Req per channel */

/* Roam parameter */
#define ROAM_IN_FASK_NORMAL_THRESHHOLD    500      /* in unit of msec If within ROAM_IN_FASK_NORMAL_THRESHHOLD, a frame is sent or received, then fast roaming is applied */

/* Beacon (or connection) lost timeout in the BSS state */
#define ROAM_FAIL_MIN_INTERVAL_SEC         10       /* NDIS is 10 sec      */
//  James modify for roaming
#define BACKGROUND_SCAN_MIN_DUR_SINCE_LAST_TX  0   /* ms */

#define ROAM_RSSI_HIGH_THRESHOLD              (-55)
#define ROAM_RSSI_LOW_THRESHOLD               (-75)
#define SCAN_RSSI_HIGH_THRESHOLD              (-55)
#define SCAN_RSSI_LOW_THRESHOLD               (-75)
#define SUP_RSSI_HIGH_THRESHOLD               (5)
#define SUP_RSSI_LOW_THRESHOLD                (1)
#define INIT_SCAN_INTERVAL                    (60000)
#define HIGH_THRESHOLD_SCAN_INTERVAL          (6000)
#define LOW_THRESHOLD_SCAN_INTERVAL           (4000)
#define HIGH_THRESHOLD_SCAN_COUNT             (1)
#define LOW_THRESHOLD_SCAN_COUNT              (0)

// James modify for PTA on/off
#define WNDRV_SELF_PTA_ON_THRESHOLD                (10)
#define WNDRV_SELF_PTA_OFF_THRESHOLD               (0)
#define WNDRV_SELF_PTA_DEAUTH_SCORE                (5)
#define WNDRV_SELF_PTA_CONNECT_SCORE               (1)
#define WNDRV_SELF_PTA_CONNECT_KEEP_TIME           (5000)

// James modify for loading
#if defined(MT6225)
#define WNDRV_CAPABILITY_AP_COUNT             (10)
#else  /* MT6228, MT6229 */
#define WNDRV_CAPABILITY_AP_COUNT             (20)
#endif

#endif

// 20080903 saker modify for ARM9 DCACHE DEV
#if 0//#if defined (__ARM9_DCACHEABLE__) || defined (__DYNAMIC_SWITCH_CACHEABILITY__)
#define CPU_CACHE_LINE_SIZE 32
#define CPU_CACHE_LINE_SIZE_MASK 0x0000001F
#define SWITCH_TO_NONCACHEABLE 0
#define SWITCH_TO_CACHEABLE 1
#define DMA_THRESHOLD 128
#endif

// James modify for PTA on/off
#define WNDRV_SELF_PTA_ON_THRESHOLD                (10)
#define WNDRV_SELF_PTA_OFF_THRESHOLD               (0)
#define WNDRV_SELF_PTA_DEAUTH_SCORE                (5)
#define WNDRV_SELF_PTA_CONNECT_SCORE               (1)
#define WNDRV_SELF_PTA_CONNECT_KEEP_TIME           (5000)

//20080930 modify by saker for balance TX/RX in a. heavy traffic b. RX using DMA
//Because the maximum RX packets in one RX interrupt ILM are 8
#define TX_RX_DIFFERENCE_TO_BALANCE 8
/*******************************************************************************
*                         D A T A   T Y P E S
********************************************************************************
*/
/*--------------------------------------------------------------*/
/* Don't change all the following typedef                       */
/*--------------------------------------------------------------*/

/* Defined in typedef.h */
//typedef INT_32  WLAN_STATUS, *PWLAN_STATUS;

/* Enumeration for priority to alloc packet. High priority should
 * do what we can do to get packet, so we should reserve at least
 * 4 packets of 2k bytes size for high priority.
 */
typedef enum {
    KAL_PKT_PRI_LOW,
    KAL_PKT_PRI_HIGH,
    KAL_PKT_PRI_DWMAX = 65536
} KAL_PKT_PRIORITY_T;

typedef struct {
    UINT_32             addr;
    UINT_32             len;
} MBUF_PHY_INFO_T;

typedef struct {
    PVOID               buf_p;      /* Ptr to Mbuf itself location */
    PVOID               data_p;     /* Virtual address of data buffer */
    UINT_32             dataLen;    /* Total length of entire Mbuf */
    UINT_32             offset;     /* Effective data is offset to data_p */
} MBUF_INFO_T;

#define MAX_NUM_OFDM_DATA_RATES              8

#if 0

/* transmit power gain entry structure for each channel */
typedef struct _TX_PWR_GAIN {
    BOOLEAN      tpcEnabled;
    UINT_8       maxGain;               /* Maximum gain value of the channel */
    UINT_8       maxTxPwr_dBm;          /* Maximum Tx power, in dBm, correspond to maxGain */
} TX_PWR_GAIN, *PTX_PWR_GAIN;

typedef struct _NIC_PER_CH_CONFIG {
    TX_PWR_GAIN  txPowerGainCCK;        /* Tx power gain information for CCK */
    TX_PWR_GAIN  txPowerGainOFDM;       /* Tx power gain information for OFDM */
    UINT_8       alcValid;
    UINT_8       txAlcCCK;
    UINT_8       txOutputPowerCCK;
    UINT_8       txAlcOFDM[MAX_NUM_OFDM_DATA_RATES];
    UINT_8       txOutputPowerOFDM[MAX_NUM_OFDM_DATA_RATES];
} NIC_PER_CH_CONFIG, *PNIC_PER_CH_CONFIG;

#define NUM_MAX_ALC_HISTORY         40

typedef struct _GAIN_SWITCH_HISTORY {
    UINT_8      numRecord;
    UINT_8      numIndex;
    UINT_8      numMaxRecord;
    UINT_16     targetAlc;
    UINT_16     alcValues[NUM_MAX_ALC_HISTORY];
    UINT_16     avgAlc;
    UINT_16     lastAvgAlc;
    UINT_8      lastGainControl;
    UINT_8      leap;
    UINT_8      InitLeap;
} GAIN_SWITCH_HISTORY, *PGAIN_SWITCH_HISTORY;

typedef struct _ALC_CONTROL_INFO {
    BOOLEAN             enableAlcTracking;
    BOOLEAN             bUseSlope;
    UINT_8              alcSlop1Divider;
    UINT_8              alcSlop1Dividend;
    UINT_8              alcSlop2Divider;
    UINT_8              alcSlop2Dividend;
    GAIN_SWITCH_HISTORY cckHistory;
    GAIN_SWITCH_HISTORY ofdmHistory[MAX_NUM_OFDM_DATA_RATES];
} ALC_CONTROL_INFO, *PALC_CONTROL_INFO;

#endif

/*******************************************************************************
*                             M A C R O S
********************************************************************************
*/

/*******************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/*--------------------------------------------------------------*/
/* Routines to indicate OS with speical events.                 */
/*--------------------------------------------------------------*/
/* Indicate OS with changing of media state */

#if 0  /* Move to gl_kal.h */
VOID
kalIndicateStatusAndComplete(IN PVOID   pvAdapter,
                             IN INT_32  i4Status,
                             IN PVOID   pvBuf,
                             IN UINT_32 u4BufLen);

VOID
kalUpdateReAssocRspInfo(IN PVOID   adapter_p,
                        IN PUINT_8 frameBody_p,
                        IN UINT_32 frameBodyLen);

VOID
kalUpdateReAssocReqInfo(IN PVOID   pvAdapter,
                        IN PUINT_8 frameBody_p,
                        IN UINT_32 frameBodyLen,
                        IN BOOLEAN reassocRequest);
#endif

/*--------------------------------------------------------------*/
/* Functions acting on socket buffers to transmit packets       */
/*                                                              */
/* A tx packet consists of multiple Mbuf in Windows.            */
/* Linux skb only contains one Mbuf, which is the exactly same  */
/* with packet descriptor (skb).                                */
/*--------------------------------------------------------------*/
#if 0 /* Not used in 5921 */
/* Retrive the packet information */
VOID
kalPktGetInfo(IN  PVOID    pvAdapter,
              IN  PVOID    pvPacket,
              OUT PVOID   *ppvFirstBufDes,
              OUT PUINT_32 pu4PacketLen,
              OUT PUINT_32 pu4MBufCount,
              OUT PUINT_32 pu4PhyBufCount);

/* Packet has been sent out */
VOID
kalSendComplete(IN PVOID  pvAdapter,
                IN PVOID  pvPacket,
                IN INT_32 i4Status);

VOID
OsSendComplete(IN PVOID  pvAdapter,
               IN PVOID  pvPacket,
               IN INT_32 i4Status);
/*--------------------------------------------------------------*/
/* Routines to process Mbuf descriptor. Each Mbuf's buffer may  */
/* consist of several not-consecutive physical memory block.    */
/*--------------------------------------------------------------*/

/* Retrive the starting address and length of Mbuf's data */
BOOL
kalMbufGetInfo(IN PVOID    pvAdapter,
               IN PVOID    pvMbuf,
               OUT PVOID   *ppvData,
               OUT PUINT_32 pu4Len);

/* Get the next Mbuf in link list */
PVOID
kalMbufGetNext(IN PVOID pvAdapter,
               IN PVOID pvMbuf);
#endif

/*--------------------------------------------------------------*/
/* Functions acting on socket buffers for receive packets and   */
/* TX management packets.                                       */
/*                                                              */
/* Linux skb only contains one rx buf, which is the exactly     */
/* same with packet descriptor (skb). In Windows, OS glue layer */
/* will create NDIS buffer descriptor and bind rx buffer and    */
/* and previous NDIS packet descriptor.                         */
/*--------------------------------------------------------------*/

/* Get memory block (buffer) from coalesced buffer for mgmt packet
 * and received packets.
 */
#if 0
PVOID
kalPacketAlloc(IN  PVOID              pvAdapter,
               IN  UINT_32            u4Size,
               IN  KAL_PKT_PRIORITY_T ePktPriority,
               OUT PUINT_8            *ppucData);

VOID
kalPacketFree(IN PVOID pvAdapter,
              IN PVOID pvPacket);

/* Adjust data starting address and length of received packets */
VOID
kalPacketPut(IN PVOID   pvAdapter,
             IN PVOID   pvPacket,
             IN PUINT_8 pucPacketStart,
             IN UINT_32 u4PacketLen);

/* Return TID associated with a given packet */
UINT_8
kalPacketTid(IN PVOID   pvAdapter,
             IN PVOID   pvPacket);

/* Indicate OS with some received packets (buffers) */
VOID
kalIndicatePackets(IN PVOID   pvAdapter,
                   IN PVOID   *apvPackets,
                   IN UINT_32 u4PacketNum,
                   IN BOOL    fgRetainPacket);

/*--------------------------------------------------------------*/
/* Cache and non-cache memory routines.                         */
/*--------------------------------------------------------------*/

/* Non-cache memory allocate/free for descriptors */
PVOID
kalSharedMemAllocSz(IN  PVOID    pvAdapter,
                    IN  UINT_32  u4Size,
                    OUT PUINT_32 pu4Dma,
                    OUT PVOID    *pvOsDep);

VOID
kalSharedMemFreeSz(IN PVOID   pvAdapter,
                   IN UINT_32 u4Size,
                   IN PVOID   pvVirtAddr,
                   IN UINT_32 u4DmaAddr,
                   IN PVOID   pvOsDep);

/* Cache memory allocation/free. */
PVOID
kalMemAlloc(IN UINT_32 u4Size);

VOID
kalMemFree(IN PVOID   pvAddr,
           IN UINT_32 u4Size);

/* Set memory block with specific pattern */
VOID
kalMemSet(IN PVOID   pvAddr,
          IN UINT_8  ucPattern,
          IN UINT_32 u4Size);

/* Zero specific memory block */
VOID
kalMemZero(IN PVOID   pvAddr,
           IN UINT_32 u4Size);

/* Copy memory block with specific size */
VOID
kalMemCopy(IN PVOID   pvDst,
           IN PVOID   pvSrc,
           IN UINT_32 u4Size);

/* Compare two memory block with specific length.
 * Return zero if they are the same.
 */
INT_32
kalMemCmp(IN PVOID   pvAddr1,
          IN PVOID   pvAddr2,
          IN UINT_32 u4Size);

/*--------------------------------------------------------------*/
/* Timer related functions                                      */
/*--------------------------------------------------------------*/

/* Get 32-bit system time tick to record event time */
UINT_32
kalGetTimeTick(VOID);

/* Process delay time in unit of micro-second */
VOID
kalUdelay(IN UINT_32 u4MicroSec);

/* Process delay time in unit of milli-second */
VOID
kalMdelay(IN UINT_32 u4MilliSec);

/* Set system timer with timeout interval. When the timeout event
 * occurs, wlanDoTimeOutCheck() is invoked with argument pvAdapter,
 * which is the same with pvAdapter in this function.
 */
BOOL
kalSetTimer(IN PVOID   pvAdapter,
            IN UINT_32 u4Interval);

#endif

VOID
kal_event_start_timer(P_ENUM_PARAM_WNDRV_ES es, kal_uint32 uInterval);

VOID
kal_event_stop_timer(ENUM_PARAM_WNDRV_ES_T es);

/*--------------------------------------------------------------*/
/* Bus Driver Prototype                                         */
/*--------------------------------------------------------------*/
#if 0
/*WCP modification*/void glBusInit(void);
/*WCP modification*/void wndrv_eint_register(kal_bool ACTpolarity);
#endif

UINT_8
mpMapTidToQueue(IN UINT_8 tid);

/*--------------------------------------------------------------*/
/* Read/Write device registers                                  */
/*--------------------------------------------------------------*/
#if 0 /* Not used in 5921 */
BOOL
kalDevRegRd16(IN  PVOID    pvAdapter,
              IN  UINT_32  u4Register,
              OUT PUINT_16 pu2Value);

BOOL
kalDevRegWr16(IN PVOID   pvAdapter,
              IN UINT_32 u4Register,
              IN UINT_16 u2Value);

BOOL
kalDevRegRd32(IN  PVOID    pvAdapter,
              IN  UINT_32  u4Register,
              OUT PUINT_32 pu4Value);

BOOL
kalDevRegWr32(IN PVOID   pvAdapter,
              IN UINT_32 u4Register,
              IN UINT_32 u4Value);
#endif
#if 0
BOOL
kalDevRegRead(
    //IN  P_GLUE_INFO_T    prGlueInfo,
    IN  UINT_32          u4Register,
    OUT PUINT_32         pu4Value
);

BOOL
kalDevRegWrite(
    //IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Register,
    IN UINT_32          u4Value
);
#endif
/*--------------------------------------------------------------*/
/* Read/Write device ports                                      */
/*--------------------------------------------------------------*/
#if 0 /* Not used in 5921 */
BOOL
kalDevPortRd16(IN  PVOID   pvAdapter,
               IN  UINT_16 u2Port,
               IN  UINT_16 u2Len,
               OUT PUINT_8 pucBuf);

BOOL
kalDevPortWr16(IN PVOID   pvAdapter,
               IN UINT_16 u2Port,
               IN UINT_16 u2Len,
               IN PUINT_8 pucBuf);

BOOL
kalDevPortRd32(IN  PVOID    pvAdapter,
               IN  UINT_16  u2Port,
               OUT PUINT_32 pu4Value);
BOOL
kalDevPortWr32(IN PVOID   pvAdapter,
               IN UINT_16 u2Port,
               IN UINT_32 u4Value);


VOID
wndrv_init_send_Q(void);

VOID
wndrv_purge_send_Q(IN UINT_8 queue_num);

VOID
wndrv_send_packet(IN PVOID pvPacket);

VOID
wndrv_send_Q_handler(IN UINT_8 queue_num);
#endif
#if 0
BOOL
kalDevPortRead(
    IN  P_GLUE_INFO_T   prGlueInfo,
    IN  UINT_16         u2Port,
    IN  UINT_16         u2Len,
    OUT PUINT_8         pucBuf,
    IN  UINT_16         u2ValidOutBufSize
);

BOOL
kalDevPortWrite(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_16 u2Port,
    IN UINT_16 u2Len,
    IN PUINT_8 pucBuf,
    IN UINT_16 u2ValidInBufSize
);
#endif
/*******************************************************************************
*           I N L I N E   F U N C T I O N   D E F I N I T I O N S
********************************************************************************
*/

#endif  /* __WLAN_KAL_H */

