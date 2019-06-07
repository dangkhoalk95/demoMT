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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/swcr.h#2 $
*/

/*! \file   "swcr.h"
    \brief
*/



/*
 *
 */

#ifndef _SWCR_H
#define _SWCR_H

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

#define SWCR_VAR(x) ((VOID *)&x)
#define SWCR_FUNC(x)  ((VOID *)x)

#define SWCR_T_FUNC BIT(7)

#define SWCR_L_32 3
#define SWCR_L_16 2
#define SWCR_L_8  1

#define SWCR_READ 0
#define SWCR_WRITE 1

#define SWCR_MAP_NUM(x)  (sizeof(x)/sizeof(x[0]))

#define SWCR_CR_NUM 7

#define SWCR_GET_RW_INDEX(action,rw,index) \
    index = action & 0x7F; \
    rw = action >> 7;


extern UINT_32          g_au4SwCr[]; /*: 0: command other: data */

typedef VOID (*PFN_SWCR_RW_T)(P_ADAPTER_T prAdapter, UINT_8 ucRead, UINT_16 u2Addr, UINT_32 *pu4Data);
typedef VOID (*PFN_CMD_RW_T)(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);

typedef struct _SWCR_MAP_ENTRY_T {
    UINT_16             u2Type;
    PVOID               u4Addr;
} SWCR_MAP_ENTRY_T, *P_SWCR_MAP_ENTRY_T;


typedef struct _SWCR_MOD_MAP_ENTRY_T {
    UINT_8 ucMapNum;
    P_SWCR_MAP_ENTRY_T prSwCrMap;
} SWCR_MOD_MAP_ENTRY_T, *P_SWCR_MOD_MAP_ENTRY_T;


/*******************************************************************************
*                             D A T A   T Y P E S
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


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

VOID swCrReadWriteCmd(
    P_ADAPTER_T prAdapter,
    UINT_8 ucRead,
    UINT_16 u2Addr,
    UINT_32 *pu4Data
);


void dumpSTA(
    P_ADAPTER_T prAdapter,
    P_STA_RECORD_T prStaRec
);
#endif

