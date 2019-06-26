/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
/**
* @file bwcs_wifi.h
*
*  BWCS to Wi-Fi configuration/settings in BWCS.
*
*/

/**@addtogroup BWCS
* @{
*
* This section introduces BWCS to Wi-Fi configuration/settings in BWCS.
*
*/

#ifndef __BWCS_WIFI_HOST_H__
#define __BWCS_WIFI_HOST_H__

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WIFI_PTA_ENABLE = 0,
    WIFI_PTA_DISABLE,
    WIFI_PTA_NOCHANGE,
    WIFI_PTA_NONE, //for init
    WIFI_PTA_NUM
} BWCS_WIFI_PTA;

typedef enum {
    BWCS_PROFILE_CUSTOM = 0,
    BWCS_PROFILE_SCO,
    BWCS_PROFILE_ACL,
    BWCS_PROFILE_A2DP,
    BWCS_PROFILE_MIXED,
    BWCS_PROFILE_NOCONN,
    BWCS_PROFILE_NONE, //for init
    BWCS_PROFILE_NUM
} BWCS_PROFILE;

typedef enum {
    WIFI_PSP_CONTINUOUS_ACTIVE = 0,
    WIFI_PSP_CONTINUOUS_POWER_SAVE,
    WIFI_PSP_FAST_SWITCH_DIRECT,
    WIFI_PSP_FAST_SWITCH_TIME,
    WIFI_PSP_DEFAULT,
    WIFI_PSP_NUM
} WIFI_PS_PROFILE;

typedef enum {
    BWCS_1_WIRE_MODE = 0,
    BWCS_2_WIRE_MODE,
    BWCS_3_WIRE_MODE,
    BWCS_4_WIRE_MODE,
    BWCS_1_WIRE_EXTEND_MODE,
    BWCS_2_WIRE_ESI_MODE,
    BWCS_1_WIRE_ESI_MODE,
    BWCS_WIRE_NUM
} BWCS_PTA_mode;

typedef enum {
    BWCS_UCM_mode = 0,
    BWCS_SCM_mode,
    BWCS_CM_mode_NUM
} BWCS_CM_mode;

typedef struct
{
    uint8_t                           ref_count;
    uint16_t                          msg_len;
    BWCS_WIFI_PTA                     eWiFiPta;
    BWCS_PROFILE                      eBwcsProfile;
    WIFI_PS_PROFILE                   eWiFiPsProfile;
    uint32_t                          wifi_poll_Int; // interval to send poll pkt in ps mode
    bool                              fgUseSingleAntSet;
/* for single antenna TDD */
    bool                              fgUseRW;     /* True: use remaining window */
    BWCS_PTA_mode                     ePtaMode;
    BWCS_CM_mode                      eCmMode;     /* UCM/SCM */
    uint8_t                           ucBt_tsco;   /* Tsco=6: CTS NAV=1.25ms, Tsco>6: CTS NAV=2.5ms */
} bwcs_req_struct;

int32_t bwcs_to_wifi_host_setting_param(bwcs_req_struct *bwcs_req);


#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif  /* __BWCS_WIFI_H__ */
