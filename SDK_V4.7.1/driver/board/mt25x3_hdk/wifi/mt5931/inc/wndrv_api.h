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

#ifndef _WNDRV_API_H
#define _WNDRV_API_H

//#include "kal_non_specific_general_types.h"
//#include "kal_release.h"
//#include "stack_config.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
//#include "MMIDataType.h"


/*******************************************************************************
*
*  802.11 power saving mode Related Functions
*
*******************************************************************************/
typedef struct {
    kal_uint8    HandleCount;
    kal_uint32   Enable;      /* Default disable WLAN power saving mode */
    kal_int8     name[32][9];
#if defined (MT5931) || defined (WIFI_BB_MT5921)
    kal_uint32   StautHandle; /* Store the STAUT handle in the WMM PS test when using 5921 driver*/
#endif
} WLAN_PS_CTL;

#define BT_STATE_IDLE         0x00000001
#define BT_STATE_INQUIRY      0x00000002
#define BT_STATE_INQUIRY_SCAN 0x00000004
#define BT_STATE_PAGE         0x00000008
#define BT_STATE_PAGE_SCAN    0x00000010
#define BT_STATE_CONNECT      0x00000020
#define BT_STATE_SNIFF        0x00000040
#define BT_STATE_HOLD         0x00000080
#define BT_STATE_PARK         0x00000100
#define BT_STATE_MASTER       0x00000200
#define BT_STATE_SCATTERNET   0x00000400
#define BT_STATE_EDR          0x00000800
#define BT_STATE_1SCO         0x00001000
#define BT_STATE_2SCO         0x00002000
#define BT_STATE_3SCO         0x00004000
#define BT_STATE_1eSCO        0x00010000
#define BT_STATE_2eSCO        0x00020000
#define BT_STATE_3eSCO        0x00040000

#define PROTOCOL_VOIP_SIPLOGIN  0x00000001
#define PROTOCOL_VOIP_SIPINCALL 0x00000002
#define PROTOCOL_SOCKETOPEN     0x00000004
#define PROTOCOL_POC_SIPLOGIN   0x00000010
#define PROTOCOL_POC_SIPONCALL  0x00000020

#define WLAN_STATE_SIP_LOGIN    0x00000001
#define WLAN_STATE_TALKING_MODE 0x00000002

kal_uint8 wndrv_PS_GetHandle(kal_int8 *psName);
void wndrv_PS_Enable(kal_uint8 handle);
void wndrv_PS_Disable(kal_uint8 handle);
void wndrv_isTalkingMode(void);
void wndrv_isIdleMode(void);

void wndrv_PowerSave_Ctrl(kal_uint8 bCtl);

void wndrv_set_Protocol_Status(kal_uint32 value, kal_uint32 mask);

/*******************************************************************************
*
*  WMM U-APSD power saving mode Related Functions
*
*******************************************************************************/
extern kal_uint32 wndrv_UAPSD_isEnable;

/*******************************************************************************
*
*  WLAN BT Co-existence Related Functions
*
*******************************************************************************/
void wndrv_PTA_isOFF(void);
void wndrv_PTA_isON(void);
kal_uint8 BT_exist(void);
void BT_State_Report2WLAN(kal_uint32 value, kal_uint32 mask);
kal_uint32 WLAN_State_Report2BT(void);
/*******************************************************************************
*
*  Chip status Related Functions
*
*******************************************************************************/
kal_bool wndrv_chip_isOK(void);

void disable_PS_mode(module_type module_id);

/*******************************************************************************
*
*  MMI WIFI callback
*
*******************************************************************************/
kal_uint32 wndrv_query_chip_capability(void);

#endif /* _WNDRV_API_H */
