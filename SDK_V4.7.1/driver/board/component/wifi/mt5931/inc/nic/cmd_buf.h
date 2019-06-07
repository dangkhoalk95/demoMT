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
** $Id:
*/

/*! \file   "cmd_buf.h"
    \brief  In this file we define the structure for Command Packet.

        In this file we define the structure for Command Packet and the control unit
    of MGMT Memory Pool.
*/




#ifndef _CMD_BUF_H
#define _CMD_BUF_H

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

typedef enum _COMMAND_TYPE {
    COMMAND_TYPE_GENERAL_IOCTL,
    COMMAND_TYPE_NETWORK_IOCTL,
    COMMAND_TYPE_SECURITY_FRAME,
    COMMAND_TYPE_MANAGEMENT_FRAME,
    COMMAND_TYPE_NUM
} COMMAND_TYPE, *P_COMMAND_TYPE;

typedef VOID (*PFN_CMD_DONE_HANDLER)(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
);

typedef VOID (*PFN_CMD_TIMEOUT_HANDLER)(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
);

struct _CMD_INFO_T {
    QUE_ENTRY_T rQueEntry;

    COMMAND_TYPE    eCmdType;

    UINT_16     u2InfoBufLen;   /* This is actual CMD buffer length */
    PUINT_8     pucInfoBuffer;  /* May pointer to structure in prAdapter */
    P_NATIVE_PACKET prPacket;   /* only valid when it's a security frame */

    ENUM_NETWORK_TYPE_INDEX_T eNetworkType;
    UINT_8      ucStaRecIndex;  /* only valid when it's a security frame */

    PFN_CMD_DONE_HANDLER        pfCmdDoneHandler;
    PFN_CMD_TIMEOUT_HANDLER     pfCmdTimeoutHandler;

    BOOLEAN     fgIsOid; /* Used to check if we need indicate */

    UINT_8      ucCID;
    BOOLEAN     fgSetQuery;
    BOOLEAN     fgNeedResp;
    BOOLEAN     fgDriverDomainMCR; /* Access Driver Domain MCR, for CMD_ID_ACCESS_REG only */
    UINT_8      ucCmdSeqNum;
    UINT_32     u4SetInfoLen; /* Indicate how many byte we read for Set OID */

    /* information indicating by OID/ioctl */
    PVOID       pvInformationBuffer;
    UINT_32     u4InformationBufferLength;

    /* private data */
    UINT_32     u4PrivateData;
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
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID
cmdBufInitialize(
    IN P_ADAPTER_T prAdapter
);

P_CMD_INFO_T
cmdBufAllocateCmdInfo(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4Length
);

VOID
cmdBufFreeCmdInfo(
    IN P_ADAPTER_T prAdapter,
    IN P_CMD_INFO_T prCmdInfo
);

/*----------------------------------------------------------------------------*/
/* Routines for CMDs                                                          */
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSendSetQueryCmd(
    IN P_ADAPTER_T  prAdapter,
    UINT_8          ucCID,
    BOOLEAN         fgSetQuery,
    BOOLEAN         fgNeedResp,
    BOOLEAN         fgIsOid,
    PFN_CMD_DONE_HANDLER pfCmdDoneHandler,
    PFN_CMD_TIMEOUT_HANDLER pfCmdTimeoutHandler,
    UINT_32         u4SetQueryInfoLen,
    PUINT_8         pucInfoBuffer,
    OUT PVOID       pvSetQueryBuffer,
    IN UINT_32      u4SetQueryBufferLen
);


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#endif /* _CMD_BUF_H */


