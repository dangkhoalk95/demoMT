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

/*! \file   "cnm_scan.h"
    \brief

*/




#ifndef _CNM_SCAN_H
#define _CNM_SCAN_H

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
#define SCN_CHANNEL_DWELL_TIME_MIN_MSEC         12
#define SCN_CHANNEL_DWELL_TIME_EXT_MSEC         98

#define SCN_TOTAL_PROBEREQ_NUM_FOR_FULL         3
#define SCN_SPECIFIC_PROBEREQ_NUM_FOR_FULL      1

#define SCN_TOTAL_PROBEREQ_NUM_FOR_PARTIAL      2
#define SCN_SPECIFIC_PROBEREQ_NUM_FOR_PARTIAL   1


#define SCN_INTERLACED_CHANNEL_GROUPS_NUM       3   /* Used by partial scan */

#define SCN_PARTIAL_SCAN_NUM                    3

#define SCN_PARTIAL_SCAN_IDLE_MSEC              100

#define MAXIMUM_OPERATION_CHANNEL_LIST          32

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* The type of Scan Source */
typedef enum _ENUM_SCN_REQ_SOURCE_T {
    SCN_REQ_SOURCE_HEM = 0,
    SCN_REQ_SOURCE_NET_FSM,
    SCN_REQ_SOURCE_ROAMING, /* ROAMING Module is independent of AIS FSM */
    SCN_REQ_SOURCE_OBSS,    /* 2.4G OBSS scan */
    SCN_REQ_SOURCE_NUM
} ENUM_SCN_REQ_SOURCE_T, *P_ENUM_SCN_REQ_SOURCE_T;

typedef enum _ENUM_SCAN_PROFILE_T {
    SCAN_PROFILE_FULL = 0,
    SCAN_PROFILE_PARTIAL,
    SCAN_PROFILE_VOIP,
    SCAN_PROFILE_FULL_2G4,
    SCAN_PROFILE_NUM
} ENUM_SCAN_PROFILE_T, *P_ENUM_SCAN_PROFILE_T;

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

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#if 0
VOID
cnmScanInit(
    VOID
);

VOID
cnmScanRunEventScanRequest(
    IN P_MSG_HDR_T prMsgHdr
);

BOOLEAN
cnmScanRunEventScanAbort(
    IN P_MSG_HDR_T prMsgHdr
);

VOID
cnmScanProfileSelection(
    VOID
);

VOID
cnmScanProcessStart(
    VOID
);

VOID
cnmScanProcessStop(
    VOID
);

VOID
cnmScanRunEventReqAISAbsDone(
    IN P_MSG_HDR_T prMsgHdr
);

VOID
cnmScanRunEventCancelAISAbsDone(
    IN P_MSG_HDR_T prMsgHdr
);

VOID
cnmScanPartialScanTimeout(
    UINT_32 u4Param
);

VOID
cnmScanRunEventScnFsmComplete(
    IN P_MSG_HDR_T prMsgHdr
);
#endif



#endif /* _CNM_SCAN_H */


