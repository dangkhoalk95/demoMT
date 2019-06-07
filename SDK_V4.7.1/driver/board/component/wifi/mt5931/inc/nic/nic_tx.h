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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/nic/nic_tx.h#13 $
*/

/*! \file   nic_tx.h
    \brief  Functions that provide TX operation in NIC's point of view.

    This file provides TX functions which are responsible for both Hardware and
    Software Resource Management and keep their Synchronization.

*/




#ifndef _NIC_TX_H
#define _NIC_TX_H

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
#define NIC_TX_RESOURCE_POLLING_TIMEOUT     256
#define NIC_TX_RESOURCE_POLLING_DELAY_MSEC  50

/* Maximum buffer count for individual HIF TCQ */

#if defined(MT6620)
#if CFG_SLT_SUPPORT
/* 20101215 mtk01725 Redistributed the initial TC resources for SLT operation */
#define NIC_TX_BUFF_COUNT_TC0       0   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC1       16   // First connection: 32
#define NIC_TX_BUFF_COUNT_TC2       0   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC3       0   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC4       4   // First connection: 2
#define NIC_TX_BUFF_COUNT_TC5       0   // First connection: 0
#else
/* 20100302 mtk02468 Redistributed the initial TC resources for normal operation */
#define NIC_TX_BUFF_COUNT_TC0       6   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC1       8   // First connection: 32
#define NIC_TX_BUFF_COUNT_TC2       8   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC3       8   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC4       4   // First connection: 2
#define NIC_TX_BUFF_COUNT_TC5       2   // First connection: 0
#endif
#elif defined(MT5931)
#define NIC_TX_BUFF_COUNT_TC0       1   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC1       14  // First connection: 32
#define NIC_TX_BUFF_COUNT_TC2       1   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC3       1   // First connection: 0
#define NIC_TX_BUFF_COUNT_TC4       4   // First connection: 2
#define NIC_TX_BUFF_COUNT_TC5       1   // First connection: 0
#endif

#define NIC_TX_BUFF_SUM                     (NIC_TX_BUFF_COUNT_TC0 + \
                                            NIC_TX_BUFF_COUNT_TC1 + \
                                            NIC_TX_BUFF_COUNT_TC2 + \
                                            NIC_TX_BUFF_COUNT_TC3 + \
                                            NIC_TX_BUFF_COUNT_TC4 + \
                                            NIC_TX_BUFF_COUNT_TC5)
#if CFG_ENABLE_FW_DOWNLOAD

#define NIC_TX_INIT_BUFF_COUNT_TC0               8
#define NIC_TX_INIT_BUFF_COUNT_TC1               0
#define NIC_TX_INIT_BUFF_COUNT_TC2               0
#define NIC_TX_INIT_BUFF_COUNT_TC3               0
#define NIC_TX_INIT_BUFF_COUNT_TC4               0
#define NIC_TX_INIT_BUFF_COUNT_TC5               0

#define NIC_TX_INIT_BUFF_SUM                    (NIC_TX_INIT_BUFF_COUNT_TC0 + \
                                                    NIC_TX_INIT_BUFF_COUNT_TC1 + \
                                                    NIC_TX_INIT_BUFF_COUNT_TC2 + \
                                                    NIC_TX_INIT_BUFF_COUNT_TC3 + \
                                                    NIC_TX_INIT_BUFF_COUNT_TC4 + \
                                                    NIC_TX_INIT_BUFF_COUNT_TC5)

#endif

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
//3 /* Session for TX QUEUES */
/* The definition in this ENUM is used to categorize packet's Traffic Class according
 * to the their TID(User Priority).
 * In order to achieve QoS goal, a particular TC should not block the process of
 * another packet with different TC.
 * In current design we will have 5 categories(TCs) of SW resource.
 */
typedef enum _ENUM_TRAFFIC_CLASS_INDEX_T {
    TC0_INDEX = 0,   /* HIF TX0: AC0 packets */
    TC1_INDEX,       /* HIF TX0: AC1 packets & non-QoS packets */
    TC2_INDEX,       /* HIF TX0: AC2 packets */
    TC3_INDEX,       /* HIF TX0: AC3 packets */
    TC4_INDEX,       /* HIF TX1: Command packets or 802.1x packets */
    TC5_INDEX,       /* HIF TX0: BMCAST packets */
    TC_NUM           /* Maximum number of Traffic Classes. */
} ENUM_TRAFFIC_CLASS_INDEX_T;

typedef struct _TX_TCQ_STATUS_T {
    UINT_8                  aucFreeBufferCount[TC_NUM];
    UINT_8                  aucMaxNumOfBuffer[TC_NUM];
} TX_TCQ_STATUS_T, *P_TX_TCQ_STATUS_T;

typedef struct _TX_TCQ_ADJUST_T {
    INT_8                   acVariation[TC_NUM];
} TX_TCQ_ADJUST_T, *P_TX_TCQ_ADJUST_T;

typedef struct _TX_CTRL_T {
    UINT_32                 u4TxCachedSize;
    PUINT_8                 pucTxCached;

    /* Elements below is classified according to TC (Traffic Class) value. */

    TX_TCQ_STATUS_T         rTc;

    PUINT_8                 pucTxCoalescingBufPtr;

    QUE_T                   rFreeMsduInfoList;

    /* Management Frame Tracking */
    /* number of management frames to be sent */
    INT_32                  i4TxMgmtPendingNum;

    /* to tracking management frames need TX done callback */
    QUE_T                   rTxMgmtTxingQueue;

#if CFG_HIF_STATISTICS
    UINT_32                 u4TotalTxAccessNum;
    UINT_32                 u4TotalTxPacketNum;
#endif

    /* Number to track forwarding frames */
    INT_32                  i4PendingFwdFrameCount;

} TX_CTRL_T, *P_TX_CTRL_T;

typedef enum _ENUM_TX_PACKET_SRC_T {
    TX_PACKET_OS,
    TX_PACKET_OS_OID,
    TX_PACKET_FORWARDING,
    TX_PACKET_MGMT,
    TX_PACKET_NUM
} ENUM_TX_PACKET_SRC_T;

typedef enum _ENUM_HIF_TX_PACKET_TYPE_T {
    HIF_TX_PACKET_TYPE_DATA = 0,
    HIF_TX_PACKET_TYPE_COMMAND,
    HIF_TX_PACKET_TYPE_HIF_LB,
    HIF_TX_PACKET_TYPE_MGMT
} ENUM_HIF_TX_PACKET_TYPE_T, *P_ENUM_HIF_TX_PACKET_TYPE_T;

typedef enum _ENUM_TX_RESULT_CODE_T {
    TX_RESULT_SUCCESS,
    TX_RESULT_LIFE_TIMEOUT,
    TX_RESULT_DROPPED_IN_DRIVER,
    TX_RESULT_NUM
} ENUM_TX_RESULT_CODE_T, *P_ENUM_TX_RESULT_CODE_T;

/* TX Call Back Function  */
typedef WLAN_STATUS(*PFN_TX_DONE_HANDLER)(
    IN P_ADAPTER_T              prAdapter,
    IN P_MSDU_INFO_T            prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T    rTxDoneStatus
);

/* TX transactions could be divided into 4 kinds:
 *
 * 1) 802.1X / Bluetooth-over-Wi-Fi Security Frames
 *    [CMD_INFO_T] - [prPacket] - in skb or NDIS_PACKET form
 *
 * 2) MMPDU
 *    [CMD_INFO_T] - [prPacket] - [MSDU_INFO_T] - [prPacket] - direct buffer for frame body
 *
 * 3) Command Packets
 *    [CMD_INFO_T] - [pucInfoBuffer] - direct buffer for content of command packet
 *
 * 4) Normal data frame
 *    [MSDU_INFO_T] - [prPacket] - in skb or NDIS_PACKET form
 */


/* PS_FORWARDING_TYPE_NON_PS means that the receiving STA is in Active Mode
*   from the perspective of host driver (maybe not synchronized with FW --> SN is needed)
*/

struct _MSDU_INFO_T {
    QUE_ENTRY_T                 rQueEntry;
    P_NATIVE_PACKET             prPacket;

    ENUM_TX_PACKET_SRC_T        eSrc;       /* specify OS/FORWARD packet */
    UINT_8                      ucUserPriority;

    /* For composing HIF TX header */
    UINT_8  ucTC;                   /* Traffic Class: 0~4 (HIF TX0), 5 (HIF TX1) */
    UINT_8  ucPacketType;           /* 0: Data, 1: Command, 2: HIF Loopback 3: Management Frame */
    UINT_8  ucStaRecIndex;
    UINT_8  ucNetworkType;          /* See ENUM_NETWORK_TYPE_T */
    UINT_8  ucFormatID;             /* 0: MAUI, Linux, Windows NDIS 5.1 */
    BOOLEAN fgIs802_1x;             /* TRUE: 802.1x frame */
    BOOLEAN fgIs802_11;             /* TRUE: 802.11 header is present */
    UINT_16 u2PalLLH;               /* PAL Logical Link Header (for BOW network) */
    UINT_16 u2AclSN;                /* ACL Sequence Number (for BOW network) */
    UINT_8  ucPsForwardingType;     /* See ENUM_PS_FORWARDING_TYPE_T */
    UINT_8  ucPsSessionID;          /* PS Session ID specified by the FW for the STA */
    BOOLEAN fgIsBurstEnd;           /* TRUE means this is the last packet of the burst for (STA, TID) */
    BOOLEAN fgIsBIP;                /* Management Frame Protection */
    BOOLEAN fgIsBasicRate;          /* Force Basic Rate Transmission */

    /* flattened from PACKET_INFO_T */
    UINT_8  ucMacHeaderLength;
    UINT_8  ucLlcLength; /* w/o EtherType */
    UINT_16 u2FrameLength;
    UINT_8  aucEthDestAddr[MAC_ADDR_LEN]; /* Ethernet Destination Address */

    /* for TX done tracking */
    UINT_8                      ucTxSeqNum;
    PFN_TX_DONE_HANDLER         pfTxDoneHandler;
};


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
VOID
nicTxInitialize(
    IN P_ADAPTER_T  prAdapter
);

WLAN_STATUS
nicTxAcquireResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucTC
);

WLAN_STATUS
nicTxPollingResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucTC
);

BOOLEAN
nicTxReleaseResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8      *aucTxRlsCnt
);

WLAN_STATUS
nicTxResetResource(
    IN P_ADAPTER_T  prAdapter
);

UINT_8
nicTxGetResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucTC
);

WLAN_STATUS
nicTxMsduInfoList(
    IN P_ADAPTER_T      prAdapter,
    IN P_MSDU_INFO_T    prMsduInfoListHead
);

WLAN_STATUS
nicTxMsduQueue(
    IN P_ADAPTER_T  prAdapter,
    UINT_8          ucPortIdx,
    P_QUE_T         prQue
);

WLAN_STATUS
nicTxCmd(
    IN P_ADAPTER_T      prAdapter,
    IN P_CMD_INFO_T     prCmdInfo,
    IN UINT_8           ucTC
);

VOID
nicTxRelease(
    IN P_ADAPTER_T  prAdapter
);

VOID
nicProcessTxInterrupt(
    IN P_ADAPTER_T  prAdapter
);

VOID
nicTxFreeMsduInfoPacket(
    IN P_ADAPTER_T    prAdapter,
    IN P_MSDU_INFO_T  prMsduInfoListHead
);

VOID
nicTxReturnMsduInfo(
    IN P_ADAPTER_T    prAdapter,
    IN P_MSDU_INFO_T  prMsduInfoListHead
);

BOOLEAN
nicTxFillMsduInfo(
    IN P_ADAPTER_T    prAdapter,
    IN P_MSDU_INFO_T  prMsduInfo,
    IN P_NATIVE_PACKET   prNdisPacket
);

WLAN_STATUS
nicTxAdjustTcq(
    IN P_ADAPTER_T  prAdapter
);

WLAN_STATUS
nicTxFlush(
    IN P_ADAPTER_T  prAdapter
);

#if CFG_ENABLE_FW_DOWNLOAD
WLAN_STATUS
nicTxInitCmd(
    IN P_ADAPTER_T      prAdapter,
    IN P_CMD_INFO_T     prCmdInfo,
    IN UINT_8           ucTC
);

WLAN_STATUS
nicTxInitResetResource(
    IN P_ADAPTER_T  prAdapter
);
#endif

WLAN_STATUS
nicTxEnqueueMsdu(
    IN P_ADAPTER_T      prAdapter,
    IN P_MSDU_INFO_T    prMsduInfo
);

UINT_32
nicTxGetFreeCmdCount(
    IN P_ADAPTER_T  prAdapter
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/


#if defined (MASS_PRODUCTION)
#define EHPI_BUS_ERROR_RECOVERY(TC_INDEX) prTxCtrl->rTc.aucFreeBufferCount[TC_INDEX] = prTxCtrl->rTc.aucMaxNumOfBuffer[TC_INDEX]
#else
#define EHPI_BUS_ERROR_RECOVERY(TC_INDEX) ASSERT(0)
#endif


#define EHPI_BUS_ERROR(TC_INDEX, a, b, c) MT5931_NEW_TRACE(MT5931_INFO_602, TRACE_ERROR, "EHPI BUS ERROR @ TC_INDEX: %d --- %x: %x, %X: %X, %X: %X", TC_INDEX,\
                                                a, *((volatile unsigned short*)(a)),\
                                                b, *((volatile unsigned int *)(b)), \
                                                c, *((volatile unsigned int *)(c)));\
                                                EHPI_BUS_ERROR_RECOVERY(TC_INDEX);



#endif /* _NIC_TX_H */
