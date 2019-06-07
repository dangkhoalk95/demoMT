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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/cnm.h#8 $
*/

/*! \file   "cnm.h"
    \brief
*/




#ifndef _CNM_H
#define _CNM_H

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

typedef enum _ENUM_CH_REQ_TYPE_T {
    CH_REQ_TYPE_JOIN,
    CH_REQ_TYPE_P2P_LISTEN,

    CH_REQ_TYPE_NUM
} ENUM_CH_REQ_TYPE_T, *P_ENUM_CH_REQ_TYPE_T;

typedef struct _MSG_CH_REQ_T {
    MSG_HDR_T           rMsgHdr;    /* Must be the first member */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
    UINT_8              ucPrimaryChannel;
    ENUM_CHNL_EXT_T     eRfSco;
    ENUM_BAND_T         eRfBand;
    ENUM_CH_REQ_TYPE_T  eReqType;
    UINT_32             u4MaxInterval;  /* In unit of ms */
    UINT_8              aucBSSID[6];
    UINT_8              aucReserved[2];
} MSG_CH_REQ_T, *P_MSG_CH_REQ_T;

typedef struct _MSG_CH_ABORT_T {
    MSG_HDR_T           rMsgHdr;    /* Must be the first member */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
} MSG_CH_ABORT_T, *P_MSG_CH_ABORT_T;

typedef struct _MSG_CH_GRANT_T {
    MSG_HDR_T           rMsgHdr;    /* Must be the first member */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
    UINT_8              ucPrimaryChannel;
    ENUM_CHNL_EXT_T     eRfSco;
    ENUM_BAND_T         eRfBand;
    ENUM_CH_REQ_TYPE_T  eReqType;
    UINT_32             u4GrantInterval;    /* In unit of ms */
} MSG_CH_GRANT_T, *P_MSG_CH_GRANT_T;

typedef struct _MSG_CH_REOCVER_T {
    MSG_HDR_T           rMsgHdr;    /* Must be the first member */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
    UINT_8              ucPrimaryChannel;
    ENUM_CHNL_EXT_T     eRfSco;
    ENUM_BAND_T         eRfBand;
    ENUM_CH_REQ_TYPE_T  eReqType;
} MSG_CH_RECOVER_T, *P_MSG_CH_RECOVER_T;


typedef struct _CNM_INFO_T {
    UINT_32     u4Reserved;
} CNM_INFO_T, *P_CNM_INFO_T;

#if CFG_ENABLE_WIFI_DIRECT
/* Moved from p2p_fsm.h */
typedef __KAL_ATTRIB_PACKED__ struct _DEVICE_TYPE_T {
    UINT_16     u2CategoryId;           /* Category ID */
    UINT_8      aucOui[4];              /* OUI */
    UINT_16     u2SubCategoryId;        /* Sub Category ID */
}  DEVICE_TYPE_T, *P_DEVICE_TYPE_T;
#endif


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
cnmInit(
    P_ADAPTER_T prAdapter
);

VOID
cnmUninit(
    P_ADAPTER_T     prAdapter
);

VOID
cnmChMngrRequestPrivilege(
    P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
);

VOID
cnmChMngrAbortPrivilege(
    P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
);

VOID
cnmChMngrHandleChEvent(
    P_ADAPTER_T     prAdapter,
    P_WIFI_EVENT_T  prEvent
);

BOOLEAN
cnmPreferredChannel(
    P_ADAPTER_T         prAdapter,
    P_ENUM_BAND_T       prBand,
    PUINT_8             pucPrimaryChannel,
    P_ENUM_CHNL_EXT_T   prBssSCO
);

BOOLEAN
cnmAisInfraChannelFixed(
    P_ADAPTER_T         prAdapter,
    P_ENUM_BAND_T       prBand,
    PUINT_8             pucPrimaryChannel
);

VOID
cnmAisInfraConnectNotify(
    P_ADAPTER_T         prAdapter
);

BOOLEAN
cnmAisIbssIsPermitted(
    P_ADAPTER_T     prAdapter
);

BOOLEAN
cnmP2PIsPermitted(
    P_ADAPTER_T     prAdapter
);

BOOLEAN
cnmBowIsPermitted(
    P_ADAPTER_T     prAdapter
);

BOOLEAN
cnmBss40mBwPermitted(
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIdx
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#ifndef _lint
/* We don't have to call following function to inspect the data structure.
 * It will check automatically while at compile time.
 * We'll need this to guarantee the same member order in different structures
 * to simply handling effort in some functions.
 */
__KAL_INLINE__ VOID
cnmMsgDataTypeCheck(
    VOID
)
{
    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, rMsgHdr) == 0);

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, rMsgHdr) ==
        OFFSET_OF(MSG_CH_RECOVER_T, rMsgHdr));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, ucNetTypeIndex) ==
        OFFSET_OF(MSG_CH_RECOVER_T, ucNetTypeIndex));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, ucTokenID) ==
        OFFSET_OF(MSG_CH_RECOVER_T, ucTokenID));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, ucPrimaryChannel) ==
        OFFSET_OF(MSG_CH_RECOVER_T, ucPrimaryChannel));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, eRfSco) ==
        OFFSET_OF(MSG_CH_RECOVER_T, eRfSco));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, eRfBand) ==
        OFFSET_OF(MSG_CH_RECOVER_T, eRfBand));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T, eReqType) ==
        OFFSET_OF(MSG_CH_RECOVER_T, eReqType));

    return;
}
#endif /* _lint */

#endif /* _CNM_H */


