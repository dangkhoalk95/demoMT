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

#ifndef _WNDRV_ABM_MSG_H
#define _WNDRV_ABM_MSG_H

#include "wndrv_cnst.h"

typedef enum {
    WNDRV_HW_OTA_ENABLE = 0,
    WNDRV_HW_OTA_DISABLE,
    WNDRV_HW_OTA_END
} wndrv_hw_ota_mode_enum;

typedef enum {
    WNDRV_POWER_MODE_CAM = 0,
    WNDRV_POWER_MODE_FAST_PS,
    WNDRV_POWER_MODE_MAX_PS,
    WNDRV_POWER_MODE_CTIA, /* new add, different with WISE */
    WNDRV_POWER_MODE_END
} wndrv_ps_mode_enum;

typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    wndrv_ps_mode_enum          ps_mode;
} wndrv_abm_set_ps_mode_req_struct;

typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    wndrv_ps_mode_enum          ps_mode;
    kal_bool                    status;
} wndrv_abm_set_ps_mode_cnf_struct;

typedef struct {
    kal_uint8   ref_count;
    kal_uint16  msg_len;
    kal_uint16  status;
} wndrv_abm_net_status_ind_struct;
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_uint8                   mode;
    kal_int8                    power;
} wndrv_abm_set_tx_pwr_req_struct;

typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_uint8                   status;
} wndrv_abm_set_tx_pwr_cnf_struct;

typedef enum {
    WNDRV_IP_STAT_NEW = 0,
    WNDRV_IP_STAT_UPDATE,
    WNDRV_IP_STAT_NUM
} wndrv_ip_stat_enum;

typedef struct {
    kal_uint8           ref_count;
    kal_uint16          msg_len;
    wndrv_ip_stat_enum  status;
} abm_wndrv_ipnetwork_status_ind_struct;

typedef struct {
    kal_uint8        ssid_len;
    kal_uint8        ssid[WNDRV_SSID_MAX_LEN];
} abm_wndrv_ssid_struct;

typedef struct {
    kal_uint8      ref_count;
    kal_uint16     msg_len;
    kal_uint8      ssid_list_num; //number of ssid in ssid_list. maximum is 7;
    abm_wndrv_ssid_struct    ssid_list[WNDRV_MAX_SSID_LIST_NUM]; //WNDRV_SSID_LIST_MAX_NUM is 7
} wndrv_abm_set_scan_desired_ssid_list_req_struct;

typedef struct {
    kal_uint8      ref_count;
    kal_uint16     msg_len;
    kal_bool       status;
} wndrv_abm_set_scan_desired_ssid_list_cnf_struct;

typedef struct {
    kal_uint8      ref_count;
    kal_uint16     msg_len;
    kal_bool       fgEnableRoaming;//TRUE: enable roaming, FALSE:disable roaming
} wndrv_abm_set_roaming_para_req_struct;

typedef struct {
    kal_uint8      ref_count;
    kal_uint16     msg_len;
    kal_bool       status;
} wndrv_abm_set_roaming_para_cnf_struct;


typedef struct {
    kal_uint8      ref_count;
    kal_uint16     msg_len;
    kal_uint16     u2ChannelDwellTime;//0: driver use default dwell time; other : dwell time in TU
} wndrv_abm_set_scan_dwell_time_req_struct;


typedef struct {
    kal_uint8      ref_count;
    kal_uint16     msg_len;
    kal_bool       status;
} wndrv_abm_set_scan_dwell_time_cnf_struct;


typedef enum {
    SNIFFER_MODE_OFF = 0,
    SNIFFER_MODE_ON_WITH_PARA,
    SNIFFER_MODE_ON_WITHOUT_PARA,
    SNIFFER_MODE_ONOFF_END
} sniffer_mode_onoff_enum;

typedef struct {
    kal_uint8           ref_count;
    kal_uint16          msg_len;
    sniffer_mode_onoff_enum         ucOnOffSwitch;   // 0: off;  1:auto-config on without other para;  other: normal on with other para

    /* must filter policy, only valid when ucOnOffSwitch != 0 && ucOnOffSwitch != 1
      ** 0x00800000:receive broadcast data frame on working channel
      ** 0x00400000:receive data and mgt frame on working channel
      ** 0x00200000:receive data frame on working channel
      ** 0x00100000:receive multicast data on working channel
      ** others: invalid*/
    kal_uint32          ucSnifferFilterOptions;

    kal_uint8           ucChannelNum;    // work channel,range 1~14, only valid when ucOnOffSwitch != 0 && ucOnOffSwitch != 1

    /* optional filter policy,only valid when ucOnOffSwitch != 0 && ucOnOffSwitch != 1
      ** if -100<cMinRssi<=cMaxRssi<0,
      **       receive packet which rssi is between cMinRssi and cMaxRssi
      ** else
      **       do not filter by packet rssi*/
    kal_int32           cMinRssi;
    kal_int32           cMaxRssi;

    /* optional filter policy,only valid when ucOnOffSwitch != 0 && ucOnOffSwitch != 1
      ** if 20<u2MinPacketLength<=u2MaxPacketLength<2048,
      **      receive packet which packet length is between  u2MinPacketLength and u2MaxPacketLength
      ** else
      **      do not filter by packet length*/
    kal_uint16          u2MinPacketLength;
    kal_uint16          u2MaxPacketLength;
} wndrv_abm_set_sniffer_mode_req_struct;

typedef enum {
    SNIFFER_MODE_SUCCESS = 0,
    SNIFFER_MODE_FAIL_INVALID_CHANNEL,
    SNIFFER_MODE_FAIL_INVALID_SNIFFERFILTEROPTIONS,
    SNIFFER_MODE_FAIL_INVALID_ONOFFSWITCH,
    SNIFFER_MODE_FAIL_NETWORK_IS_CONNECTED,
    SNIFFER_MODE_FAIL_UNKNOWN
} sniffer_status_enum;

typedef struct {
    kal_uint8           ref_count;
    kal_uint16          msg_len;
    sniffer_status_enum   status;
    kal_uint8            OnOffStatus;//0:off , non-zero:on
} wndrv_abm_set_sniffer_mode_cnf_struct;

typedef struct {
    kal_uint8           ref_count;
    kal_uint16          msg_len;
    kal_uint8           ssid_len;
    kal_uint8           ssid[WNDRV_SSID_MAX_LEN];
    kal_uint8           password_len; // password length, max mun is 64
    kal_uint8           password[WNDRV_PASSWORD_MAX_LEN];
    kal_uint8           auth_mode; //autentication type byte copied from sniffer packet
} wndrv_abm_iot_listened_ap_info_ind_struct;


#endif /*_WNDRV_ABM_MSG_H*/

