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
** $Id: //Project/WLAN/MT5921_BRANCHES/MT5921_11AW1112MP/include/os/gl_kal.h#1 $
*/

/*! \file   gl_kal.h
    \brief  Declaration of KAL functions - kal*() which is provided by GLUE Layer.

    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/





#ifndef _GL_KAL_H
#define _GL_KAL_H


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "wlan_config.h"
#include "gl_typedef.h"
#include "gl_os.h"
//#include "wlan_lib.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* WCP Modification */ 
typedef enum _ENUM_SPIN_LOCK_CATEGORY_E {
    SPIN_LOCK_FSM = 0,

#ifdef WCN_MAUI
    SPIN_LOCK_SDIO_DDK_TX_QUE,
    SPIN_LOCK_SDIO_DDK_RX_QUE,
#endif

    /* FIX ME */
    SPIN_LOCK_RX_QUE,
    SPIN_LOCK_TX_QUE,
    SPIN_LOCK_CMD_QUE,
    SPIN_LOCK_TX_RESOURCE,
    SPIN_LOCK_CMD_RESOURCE,
    SPIN_LOCK_QM_TX_QUEUE,
    SPIN_LOCK_CMD_PENDING,
    SPIN_LOCK_CMD_SEQ_NUM,
    SPIN_LOCK_TX_MSDU_INFO_LIST,
    SPIN_LOCK_TXING_MGMT_LIST,
    SPIN_LOCK_TX_SEQ_NUM,
    SPIN_LOCK_TX_COUNT,
    SPIN_LOCK_TXS_COUNT,
    /* end    */
    SPIN_LOCK_TX,
    SPIN_LOCK_IO_REQ,
    SPIN_LOCK_INT,

    SPIN_LOCK_MGT_BUF,
    SPIN_LOCK_MSG_BUF,
    SPIN_LOCK_STA_REC,

    SPIN_LOCK_MAILBOX,
    SPIN_LOCK_TIMER,

    SPIN_LOCK_BOW_TABLE,

    SPIN_LOCK_EHPI_BUS, /* only for EHPI */
    SPIN_LOCK_NUM
} ENUM_SPIN_LOCK_CATEGORY_E;

/* event for assoc infomation update */
typedef struct _EVENT_ASSOC_INFO {
    UINT_8      ucAssocReq; /* 1 for assoc req, 0 for assoc rsp */
    UINT_8      ucReassoc;  /* 0 for assoc, 1 for reassoc */
    UINT_16     u2Length;
    PUINT_8     pucIe;
} EVENT_ASSOC_INFO, *P_EVENT_ASSOC_INFO;

typedef enum _ENUM_KAL_NETWORK_TYPE_INDEX_T {
    KAL_NETWORK_TYPE_AIS_INDEX = 0,
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    KAL_NETWORK_TYPE_P2P_INDEX,
#endif
#if CFG_ENABLE_BT_OVER_WIFI
    KAL_NETWORK_TYPE_BOW_INDEX,
#endif
    KAL_NETWORK_TYPE_INDEX_NUM
} ENUM_KAL_NETWORK_TYPE_INDEX_T;

typedef enum _ENUM_KAL_MEM_ALLOCATION_TYPE_E {
    PHY_MEM_TYPE,   /* physically continuous */
    VIR_MEM_TYPE,   /* virtually continous */
    MEM_TYPE_NUM
} ENUM_KAL_MEM_ALLOCATION_TYPE;


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
/* Macros of SPIN LOCK operations for using in Driver Layer                   */
/*----------------------------------------------------------------------------*/
#define KAL_SPIN_LOCK_DECLARATION()

#define KAL_ACQUIRE_SPIN_LOCK(_prAdapter, _rLockCategory)   \
            kalAcquireSpinLock(((P_ADAPTER_T)_prAdapter)->prGlueInfo, _rLockCategory)

#define KAL_RELEASE_SPIN_LOCK(_prAdapter, _rLockCategory)   \
            kalReleaseSpinLock(((P_ADAPTER_T)_prAdapter)->prGlueInfo, _rLockCategory)

/*----------------------------------------------------------------------------*/
/* Macros for accessing Reserved Fields of native packet                      */
/*----------------------------------------------------------------------------*/
#define KAL_GET_PKT_QUEUE_ENTRY(_p)             GLUE_GET_PKT_QUEUE_ENTRY(_p)
#define KAL_GET_PKT_DESCRIPTOR(_prQueueEntry)   GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)
#define KAL_GET_PKT_TID(_p)                     GLUE_GET_PKT_TID(_p)
#define KAL_GET_PKT_IS1X(_p)                    GLUE_GET_PKT_IS1X(_p)
#define KAL_GET_PKT_HEADER_LEN(_p)              GLUE_GET_PKT_HEADER_LEN(_p)
#define KAL_GET_PKT_PAYLOAD_LEN(_p)             GLUE_GET_PKT_PAYLOAD_LEN(_p)
#define KAL_GET_PKT_ARRIVAL_TIME(_p)            GLUE_GET_PKT_ARRIVAL_TIME(_p)

/* defined for wince sdio driver only, eHPI does not need */
#define kalDevSetPowerState(prGlueInfo, ePowerMode)


/* WCP Modification */ //#define kalUpdateReAssocRspInfo(prGlueInfo, pucFrameBody, u4FrameBodyLen)
//#define kalUpdateReAssocReqInfo(prGlueInfo, pucFrameBody, u4FrameBodyLen, fgReassocRequest)

VOID
kalUpdateReAssocRspInfo(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PUINT_8          pucFrameBody,
    IN UINT_32          u4FrameBodyLen
);

/*----------------------------------------------------------------------------*/
/*!
* \brief Cache memory allocation
*
* \param[in] u4Size Required memory size.
*
* \return Pointer to allocated memory
*         or NULL
*/
/*----------------------------------------------------------------------------*/
/* WCP Modification */ //#define kalMemAlloc(u4Size)                         kmalloc(u4Size, GFP_ATOMIC)
PVOID kalMemAlloc(IN UINT_32 u4Size, IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType);

/*----------------------------------------------------------------------------*/
/*!
* \brief Free allocated cache memory
*
* \param[in] pvAddr Required memory size.
* \param[in] u4Size Allocated memory size.
*
* \return -
*/
/*----------------------------------------------------------------------------*/

/* WCP Modification *///#define kalMemFree(pvAddr, u4Size)                  kfree(pvAddr)
VOID kalMemFree(IN PVOID pvAddr, IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType, IN UINT_32 u4Size);
/* WCP Modification *///#define kalUdelay(u4USec)                           udelay(u4USec)
VOID kalUdelay(IN UINT_32 u4MicroSec);
/* WCP Modification *///#define kalMdelay(u4MSec)                           mdelay(u4MSec)
VOID kalMdelay(IN UINT_32 u4MilliSec);
VOID kalMsleep(IN UINT_32 u4MilliSec);

/* WCP Modification */ UINT_32 kalGetTimeTick(VOID);

#if 0 /* WCP Modification */
/* Copy memory from user space to kernel space */
#define kalMemCopyFromUser(_pvTo, _pvFrom, _u4N)    copy_from_user(_pvTo, _pvFrom, _u4N)

/* Copy memory from kernel space to user space */
#define kalMemCopyToUser(_pvTo, _pvFrom, _u4N)      copy_to_user(_pvTo, _pvFrom, _u4N)
#endif

/* Copy memory block with specific size */
#define kalMemCopy(pvDst, pvSrc, u4Size)            kal_mem_cpy((PVOID)(pvDst), (PVOID)(pvSrc), u4Size)

/* Set memory block with specific pattern */
#define kalMemSet(pvAddr, ucPattern, u4Size)        kal_mem_set((PVOID)(pvAddr), ucPattern, u4Size)

/* Compare two memory block with specific length.
 * Return zero if they are the same.
 */
/* WCP Modification *///#define kalMemCmp(pvAddr1, pvAddr2, u4Size)         memcmp((PVOID)(pvAddr1), (PVOID)(pvAddr2), u4Size)
INT_32 kalMemCmp(IN PVOID pvAddr1, IN PVOID pvAddr2, IN UINT_32 u4Size);

/* Zero specific memory block */
#define kalMemZero(pvAddr, u4Size)                  kal_mem_set(pvAddr, 0, u4Size)


/*----------------------------------------------------------------------------*/
/*!
* \brief Notify OS with SendComplete event of the specific packet. Linux should
*        free packets here.
*
* \param[in] prGlueInfo     Pointer of GLUE Data Structure
* \param[in] pvPacket       Pointer of Packet Handle
* \param[in] status         Status Code for OS upper layer
*
* \return -
*/
/*----------------------------------------------------------------------------*/
#if 0
#define kalSendComplete(prGlueInfo, pvPacket, status)   \
            kalSendCompleteAndAwakeQueue(prGlueInfo, pvPacket)
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is used to locate the starting address of incoming ethernet
*        frame for skb.
*
* \param[in] prGlueInfo     Pointer of GLUE Data Structure
* \param[in] pvPacket       Pointer of Packet Handle
*
* \return starting address of ethernet frame buffer.
*/
/*----------------------------------------------------------------------------*/
#if 0 /* WCP Modification */
#define kalQueryBufferPointer(prGlueInfo, pvPacket)     \
            ((PUINT_8)((struct sk_buff *)pvPacket)->data)
#endif
PUINT_8
kalQueryBufferPointer(IN P_GLUE_INFO_T prGlueInfo,
                      IN PVOID pvPacket);

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is used to copy the entire frame from skb to the destination
*        address in the input parameter.
*
* \param[in] prGlueInfo     Pointer of GLUE Data Structure
* \param[in] pvPacket       Pointer of Packet Handle
* \param[in] pucDestBuffer  Destination Address
*
* \return -
*/
/*----------------------------------------------------------------------------*/
#if 0 /* WCP Modification */
#define kalCopyFrame(prGlueInfo, pvPacket, pucDestBuffer)   \
            {struct sk_buff *skb = (struct sk_buff *)pvPacket; \
             memcpy(pucDestBuffer, skb->data, skb->len);}

#endif
VOID
kalCopyFrame(IN P_GLUE_INFO_T prGlueInfo,
             IN PVOID pvPacket, IN OUT PUINT_8  pucDestBuffer);

/*----------------------------------------------------------------------------*/
/*!
* \brief This function returns the current OS_SYSTIME that have elapsed since
*        elapsed since the system was booted. The unit of OS_SYSTIME is OS
*        dependent.
*
* \param -
*
* \return The LSB 32-bits of the system uptime
*/
/*----------------------------------------------------------------------------*/

/*WCP Modification*/ //#define kalGetTimeTick()                            jiffies


#define PRINTF_ARG(...)                             __VA_ARGS__
/*WCP Modification*/ #define SPRINTF(buf, arg)                           {buf += sprintf((char*) buf, PRINTF_ARG arg);}

/* Get driver initialized default value */
/*WCP Modification*/ //#define KAL_GET_PARAM_INIT_VALUE(_prGlueInfo, eParam)   kalGetParamDefaultValue(_prGlueInfo, eParam)

#define USEC_TO_SYSTIME(_usec)      ((_usec) / USEC_PER_MSEC)
#define MSEC_TO_SYSTIME(_msec)      ((_msec) * KAL_HZ / MSEC_PER_SEC)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/* Routines in gl_kal.c                                                       */
/*----------------------------------------------------------------------------*/
VOID
kalAcquireSpinLock(
    IN P_GLUE_INFO_T                prGlueInfo,
    IN ENUM_SPIN_LOCK_CATEGORY_E    rLockCategory
);

VOID
kalReleaseSpinLock(
    IN P_GLUE_INFO_T                prGlueInfo,
    IN ENUM_SPIN_LOCK_CATEGORY_E    rLockCategory
);

VOID
kalUpdateMACAddress(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PUINT_8 pucMacAddr
);

VOID
kalPacketFree(
    IN  P_GLUE_INFO_T       prGlueInfo,
    IN PVOID pvPacket
);

PVOID
kalPacketAlloc(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32 u4Size,
    OUT PUINT_8 *ppucData
);

VOID
kalOsTimerInitialize(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID prTimerHandler
);

BOOL
kalSetTimer(
    IN P_GLUE_INFO_T prGlueInfo,
    IN OS_SYSTIME rInterval
);

/* WCP Modification */ BOOL
kalCheckDevRegWR(
    IN P_GLUE_INFO_T  prGlueInfo,
    IN UINT_32        u4Register,
    IN UINT_32        u4Value,
    IN UINT_32        u4Count
);

WLAN_STATUS
kalProcessRxPacket(
    IN P_GLUE_INFO_T      prGlueInfo,
    IN PVOID              pvPacket,
    IN PUINT_8            pucPacketStart,
    IN UINT_32            u4PacketLen,
    //IN PBOOLEAN           pfgIsRetain,
    IN BOOLEAN            fgIsRetain,
    IN ENUM_CSUM_RESULT_T aerCSUM[]
);

WLAN_STATUS
kalRxIndicatePkts(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PVOID apvPkts[],
    IN UINT_8 ucPktNum
);

#if SUPPORT_WPS
VOID
kalRxIndicateMgt(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Subtype,
    IN PVOID            pvPacket
);
#endif /* SUPPORT_WPS */

VOID
kalIndicateStatusAndComplete(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN WLAN_STATUS      eStatus,
    IN PVOID            pvBuf,
    IN UINT_32          u4BufLen
);

VOID
kalUpdateReAssocReqInfo(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PUINT_8          pucFrameBody,
    IN UINT_32          u4FrameBodyLen,
    IN BOOLEAN          fgReassocRequest
);

#if CFG_TX_FRAGMENT
BOOLEAN
kalQueryTxPacketHeader(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket,
    OUT PUINT_16 pu2EtherTypeLen,
    OUT PUINT_8 pucEthDestAddr
);
#endif /* CFG_TX_FRAGMENT */

VOID
kalSendCompleteAndAwakeQueue(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket
);

#if CFG_TCP_IP_CHKSUM_OFFLOAD
VOID
kalQueryTxChksumOffloadParam(
    IN  PVOID     pvPacket,
    OUT PUINT_8   pucFlag);

VOID
kalUpdateRxCSUMOffloadParam(
    IN PVOID               pvPacket,
    IN ENUM_CSUM_RESULT_T  eCSUM[]
);
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */

P_REG_INFO_T
kalGetConfiguration(
    IN P_GLUE_INFO_T    prGlueInfo
);

VOID
kalOidComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN BOOLEAN fgSetQuery,
    IN UINT_32 u4SetQueryInfoLen,
    IN WLAN_STATUS rOidStatus
);

VOID
kalOidClearance(
    IN P_GLUE_INFO_T prGlueInfo
);

BOOL
kalQoSFrameClassifierAndPacketInfo(
    IN P_GLUE_INFO_T prGlueInfo,
    IN P_NATIVE_PACKET prPacket,
    OUT PUINT_8 pucPriorityParam,
    OUT PUINT_32 pu4PacketLen,
    OUT PUINT_8 pucEthDestAddr,
    OUT PBOOLEAN pfgIs1X,
    OUT PBOOLEAN pfgIsPAL,
    OUT PUINT_8 pucNetworkType
);

VOID
kalClearSecurityFrames(
    IN P_GLUE_INFO_T prGlueInfo
);

VOID
kalClearMgmtFrames(
    IN P_GLUE_INFO_T prGlueInfo
);

VOID
kalClearMgmtFramesByNetType(
    IN P_GLUE_INFO_T prGlueInfo,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

VOID
kalUpdateMACAddress(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PUINT_8 pucMacAddr
);

PVOID
kalPacketAlloc(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Size,
    OUT PUINT_8         *ppucData
);

BOOLEAN
kalIsCardRemoved(
    IN P_GLUE_INFO_T    prGlueInfo
);

VOID
kalSecurityFrameSendComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket,
    IN WLAN_STATUS rStatus
);

VOID
kalOidCmdClearance(
    IN P_GLUE_INFO_T prGlueInfo
);

BOOLEAN
kalRetrieveNetworkAddress(
    IN P_GLUE_INFO_T prGlueInfo,
    IN OUT PARAM_MAC_ADDRESS *prMacAddr
);

VOID
kalEnqueueCommand(
    IN P_GLUE_INFO_T prGlueInfo,
    IN P_QUE_ENTRY_T prQueueEntry
);

ENUM_PARAM_MEDIA_STATE_T
kalGetMediaStateIndicated(
    IN P_GLUE_INFO_T    prGlueInfo
);

UINT_32
kalGetTxPendingFrameCount(
    IN P_GLUE_INFO_T    prGlueInfo
);

VOID
kalSendComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket,
    IN INT_32 i4Status
);

VOID
kalSetEvent(
    IN P_GLUE_INFO_T pr
);

VOID
kalMainThread(
    IN P_GLUE_INFO_T pr
);

VOID
kalSetMediaStateIndicated(
    IN P_GLUE_INFO_T            prGlueInfo,
    IN ENUM_PARAM_MEDIA_STATE_T eParamMediaStateIndicate
);


/*----------------------------------------------------------------------------*/
/* NVRAM/Registry Service                                                     */
/*----------------------------------------------------------------------------*/
BOOLEAN
kalIsConfigurationExist(
    IN P_GLUE_INFO_T    prGlueInfo
);

P_REG_INFO_T
kalGetConfiguration(
    IN P_GLUE_INFO_T    prGlueInfo
);

VOID
kalGetConfigurationVersion(
    IN P_GLUE_INFO_T    prGlueInfo,
    OUT PUINT_16        pu2Part1CfgOwnVersion,
    OUT PUINT_16        pu2Part1CfgPeerVersion,
    OUT PUINT_16        pu2Part2CfgOwnVersion,
    OUT PUINT_16        pu2Part2CfgPeerVersion
);

BOOLEAN
kalCfgDataRead16(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Offset,
    OUT PUINT_16        pu2Data
);

BOOLEAN
kalCfgDataWrite16(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Offset,
    IN UINT_16          u2Data
);

/*----------------------------------------------------------------------------*/
/* Security Frame Clearance                                                   */
/*----------------------------------------------------------------------------*/
VOID
kalClearSecurityFramesByNetType(
    IN P_GLUE_INFO_T prGlueInfo,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);

VOID
set_bit(
    IN UINT_32 nr,
    OUT UINT_32 *addr
);

UINT_32
test_and_clear_bit(
    IN UINT_32 nr,
    OUT UINT_32 *addr
);

VOID
kalGetChannelList(
    IN P_GLUE_INFO_T           prGlueInfo,
    IN ENUM_BAND_T             eSpecificBand,
    IN UINT_8                  ucMaxChannelNum,
    IN PUINT_8                 pucNumOfChannel,
    IN P_RF_CHANNEL_INFO_T     paucChannelList
);

PVOID
kalAllocateIOBuffer(
    IN UINT_32 u4AllocSize
);

VOID
kalReleaseIOBuffer(
    IN PVOID pvAddr,
    IN UINT_32 u4Size
);

VOID
kalUpdateRSSI(
    IN P_GLUE_INFO_T                    prGlueInfo,
    IN ENUM_KAL_NETWORK_TYPE_INDEX_T    eNetTypeIdx,
    IN INT_8                            cRssi,
    IN INT_8                            cLinkQuality
);

UINT_32
kalGetFwLoadAddress(
    IN P_GLUE_INFO_T    prGlueInfo
);

VOID
kalFlushPendingTxPackets(
    IN P_GLUE_INFO_T    prGlueInfo
);

VOID
kalFlushPendingTxPacketsByNetwork(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
);


BOOLEAN
kalWSCGetActiveState(
    IN P_GLUE_INFO_T    prGlueInfo
);

BOOLEAN
kalInitIOBuffer(
    VOID
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _GL_KAL_H */

