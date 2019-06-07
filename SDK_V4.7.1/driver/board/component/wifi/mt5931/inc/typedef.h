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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/typedef.h#5 $
*/

/*! \file   typedef.h
    \brief  Declaration of data type and return values of internal protocol stack.

    In this file we declare the data type and return values which will be exported
    to the GLUE Layer.
*/




#ifndef _TYPEDEF_H
#define _TYPEDEF_H
/* WCP Modification */ #include "precomp.h"
/* WCP Modification */ typedef struct wndrv_pkt_descriptor_struct wndrv_pkt_descriptor_struct;
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

/* ieee80211.h of linux has duplicated definitions */
#if defined(WLAN_STATUS_SUCCESS)
#undef WLAN_STATUS_SUCCESS
#endif

#define WLAN_STATUS_SUCCESS                     ((WLAN_STATUS) 0x00000000L)
#define WLAN_STATUS_PENDING                     ((WLAN_STATUS) 0x00000103L)
#define WLAN_STATUS_NOT_ACCEPTED                ((WLAN_STATUS) 0x00010003L)

#define WLAN_STATUS_MEDIA_CONNECT               ((WLAN_STATUS) 0x4001000BL)
#define WLAN_STATUS_MEDIA_DISCONNECT            ((WLAN_STATUS) 0x4001000CL)
#define WLAN_STATUS_MEDIA_SPECIFIC_INDICATION   ((WLAN_STATUS) 0x40010012L)

#define WLAN_STATUS_SCAN_COMPLETE               ((WLAN_STATUS) 0x60010001L)
#define WLAN_STATUS_MSDU_OK                     ((WLAN_STATUS) 0x60010002L)

/* TODO(Kevin): double check if 0x60010001 & 0x60010002 is proprietary */
#define WLAN_STATUS_ROAM_OUT_FIND_BEST          ((WLAN_STATUS) 0x60010101L)
#define WLAN_STATUS_ROAM_DISCOVERY              ((WLAN_STATUS) 0x60010102L)

#define WLAN_STATUS_FAILURE                     ((WLAN_STATUS) 0xC0000001L)
#define WLAN_STATUS_RESOURCES                   ((WLAN_STATUS) 0xC000009AL)
#define WLAN_STATUS_NOT_SUPPORTED               ((WLAN_STATUS) 0xC00000BBL)

#define WLAN_STATUS_MULTICAST_FULL              ((WLAN_STATUS) 0xC0010009L)
#define WLAN_STATUS_INVALID_PACKET              ((WLAN_STATUS) 0xC001000FL)
#define WLAN_STATUS_ADAPTER_NOT_READY           ((WLAN_STATUS) 0xC0010011L)
#define WLAN_STATUS_NOT_INDICATING              ((WLAN_STATUS) 0xC0010013L)
#define WLAN_STATUS_INVALID_LENGTH              ((WLAN_STATUS) 0xC0010014L)
#define WLAN_STATUS_INVALID_DATA                ((WLAN_STATUS) 0xC0010015L)
#define WLAN_STATUS_BUFFER_TOO_SHORT            ((WLAN_STATUS) 0xC0010016L)

#define WLAN_STATUS_BWCS_UPDATE            ((WLAN_STATUS) 0xC0010017L)

/* NIC status flags */
#define ADAPTER_FLAG_HW_ERR                     0x00400000

/* Type Length */
#define TL_IPV4     0x0008
#define TL_IPV6     0xDD86

#define WNDRV_DUMP_PMU_REG      (0x0001)
#define WNDRV_DUMP_RST_REG      (0x0002)
#define WNDRV_DUMP_26M_REG      (0x0004)
#define WNDRV_DUMP_32K_REG      (0x0008)
#define WNDRV_DUMP_EINT_REG     (0x0010)
#define WNDRV_DUMP_ALL_REG      (0xFFFF)


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* Type definition for GLUE_INFO structure */
typedef struct _GLUE_INFO_T     GLUE_INFO_T, *P_GLUE_INFO_T;

/* Type definition for WLAN STATUS */
typedef UINT_32                 WLAN_STATUS, *P_WLAN_STATUS;

/* Type definition for ADAPTER structure */
typedef struct _ADAPTER_T       ADAPTER_T, *P_ADAPTER_T;

/* Type definition for MESSAGE HEADER structure */
typedef struct _MSG_HDR_T       MSG_HDR_T, *P_MSG_HDR_T;

/* Type definition for Pointer to OS Native Packet */
typedef void                    *P_NATIVE_PACKET;

/* Type definition for STA_RECORD_T structure to handle the connectivity and packet reception
 * for a particular STA.
 */
typedef struct _STA_RECORD_T    STA_RECORD_T, *P_STA_RECORD_T, * *PP_STA_RECORD_T;

/* CMD_INFO_T is used by Glue Layer to send a cluster of Command(OID) information to
 * the TX Path to reduce the parameters of a function call.
 */
typedef struct _CMD_INFO_T      CMD_INFO_T, *P_CMD_INFO_T;

/* Following typedef should be removed later, because Glue Layer should not
 * be aware of following data type.
 */
typedef struct _SW_RFB_T        SW_RFB_T, *P_SW_RFB_T, * *PP_SW_RFB_T;

typedef struct _MSDU_INFO_T     MSDU_INFO_T, *P_MSDU_INFO_T;

typedef struct _REG_ENTRY_T     REG_ENTRY_T, *P_REG_ENTRY_T;

/* IST handler definition */
typedef VOID (*IST_EVENT_FUNCTION)(P_ADAPTER_T);

/* Type definition for function pointer of timer handler */
typedef VOID (*PFN_TIMER_CALLBACK)(IN P_GLUE_INFO_T);


/* PACKET_INFO_T is used by Glue Layer to send a cluster of packet information to
 * the Internal Protocol Stack to reduce the parameters of a function call.
 */

typedef struct _PACKET_INFO_T {
    P_NATIVE_PACKET prPacket;
    /* WCP Modification */ wndrv_pkt_descriptor_struct    *wndrv_pkt_p;
    BOOLEAN         fgIs802_11;
    BOOLEAN         fgIs1x;
    UINT_8          ucTID;
    UINT_8          ucMacHeaderLength;
    UINT_16         u2PayloadLength;
    PUINT_8         pucDestAddr;
    /* WCP Modification */ kal_uint8    reserved0;
    BOOLEAN         fgIsP2P;
    BOOLEAN         fgIsPAL;
} PACKET_INFO_T, *P_PACKET_INFO_T;


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
/* WCN Modification */ #define PACKET_INFO_INIT(_prPacketInfo, \
        _fgIs802_11, \
        _fgIs802_1x, \
        _prPacketDescriptor, \
        _ucTID, \
        _ucMacHeaderLength, \
        _u2PayloadLength, \
        _pucDestAddr \
                                               ) \
{ \
    ((P_PACKET_INFO_T)(_prPacketInfo))->prPacket = (P_NATIVE_PACKET)(_prPacketDescriptor); \
    ((P_PACKET_INFO_T)(_prPacketInfo))->fgIs802_11 = (BOOLEAN)(_fgIs802_11); \
    ((P_PACKET_INFO_T)(_prPacketInfo))->fgIs1x = (BOOLEAN)(_fgIs802_1x); \
    ((P_PACKET_INFO_T)(_prPacketInfo))->ucTID = (UINT_8)(_ucTID); \
    ((P_PACKET_INFO_T)(_prPacketInfo))->ucMacHeaderLength = (UINT_8)(_ucMacHeaderLength); \
    ((P_PACKET_INFO_T)(_prPacketInfo))->u2PayloadLength = (UINT_16)(_u2PayloadLength); \
    ((P_PACKET_INFO_T)(_prPacketInfo))->pucDestAddr = (PUINT_8)(_pucDestAddr); \
};

#define PACKET_INFO_PAYLOAD_LEN(prPacketInfo)   (((P_PACKET_INFO_T)prPacketInfo)->u2PayloadLength)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#endif /* _TYPEDEF_H */


