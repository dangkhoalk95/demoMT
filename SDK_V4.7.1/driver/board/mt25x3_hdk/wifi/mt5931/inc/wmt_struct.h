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

/*****************************************************************************
 * Include
 *****************************************************************************/
#ifndef __WMT_STRUCT_H__
//#include "kal_non_specific_general_types.h"

#define WMT_MAX_QUERY_LINK_NUM 10
#define WMT_MAX_LEN_RATES_EX   16

typedef enum {
    WMT_BT_STACK_UP = 0, /* power on */
    WMT_BT_STACK_DOWN, /* power off */
    WMT_BT_CONNECTION, /* BT create conntection */
    WMT_BT_DISCONNECTION, /* BT disconnection */
    WMT_BT_AVDTP_STATE, /* BT has AVDTP link */
    WMT_BT_NONE, // for init
    WMT_BT_NUM
} WMT_BTEVENT;

typedef enum {
    BT_AVDTP_STATE_NONE = 0,
    BT_AVDTP_STATE_SUSPENDED, /* AVDTP create, in suspend state */
    BT_AVDTP_STATE_STREAMING,
    BT_AVDTP_STATE_DISCONNECTED,
    BT_AVDTP_STATE_NUM
} WMT_BT_AVDTP_EVENT;

typedef enum {
    WMT_WIFI_UP = 0,
    WMT_WIFI_DOWN,
    WMT_WIFI_CONNECTION,
    WMT_WIFI_DISCONNECTION,
    WMT_WIFI_RATE_UPDATE,
    WMT_WIFI_NONE, //for init
    WMT_WIFI_NUM
} WMT_WIFIEVENT;

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
    WIFI_PTA_ENABLE = 0,
    WIFI_PTA_DISABLE,
    WIFI_PTA_NOCHANGE,
    WIFI_PTA_NONE, //for init
    WIFI_PTA_NUM
} BWCS_WIFI_PTA;

typedef enum {
    WIFI_PSP_CONTINUOUS_ACTIVE = 0,
    WIFI_PSP_CONTINUOUS_POWER_SAVE,
    WIFI_PSP_FAST_SWITCH_DIRECT,
    WIFI_PSP_FAST_SWITCH_TIME,
    WIFI_PSP_DEFAULT,
    WIFI_PSP_NUM
} WIFI_PS_PROFILE;

//move from bwcs.h
typedef enum {
    BWCS_EVENT_INIT = 0,
    BWCS_EVENT_BT_UP,
    BWCS_EVENT_BT_DOWN,
    BWCS_EVENT_BT_CONN,
    BWCS_EVENT_BT_DISCONN,
    BWCS_EVENT_BT_AVDTP_STREAM,
    BWCS_EVENT_BT_AVDTP_SUSPEND,
    BWCS_EVENT_BT_AVDTP_DISCONN,
    BWCS_EVENT_WIFI_UP,
    BWCS_EVENT_WIFI_DOWN,
    BWCS_EVENT_WIFI_CONN,
    BWCS_EVENT_WIFI_RATE_UPDATE,
    BWCS_EVENT_WIFI_DISCONN,
    BWCS_EVENT_TIMEOUT_DECISION,
    BWCS_EVENT_NUM
} BWCS_EVENT;

typedef enum {
    STEP1 = 0,
    STEP2,
    STEP3,
    STEP_NUM
} BWCS_DECISION_CASE;

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

/*********************************************************************************
* BT
**********************************************************************************/
typedef struct {
    kal_int32          bt_conn_handle; /* The connection handle. the unique to check connection */
    kal_uint8          bd_address[6];  /* BT ADDR for Query RSSI */
    kal_uint8          bt_link_type;   /* The type of link. Zero (0) indicates SCO, and 1 indicates ACL. */
    WMT_BT_AVDTP_EVENT bt_avdtp_state; /* if the connection is A2DP */
    kal_uint8          bt_tsco;        /* Tsco=6: CTS NAV=1.25ms, Tsco>6: CTS NAV=2.5ms */
} bt_conn_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    WMT_BTEVENT                         bt_eventid;
    WMT_BT_AVDTP_EVENT                  bt_avdtp_state;
    kal_uint8                           update_conn_num; //090916 ingore, always one conn. /* current msg update which bt_conn*/
    kal_uint8                           valid_conn_num;  /* how many conn num is valid */
    bt_conn_struct                      bt_conn; //090916 always one conn.
} wmt_bt_connection_status_ind_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint8                           bd_address[6];  /* BT ADDR for Query RSSI */
    //kal_uint32                          bt_conn_handle; //090916 BT conn handle for query rssi
} wmt_bt_query_rssi_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_bool                            is_valid;
    //kal_uint32                          bt_conn_handle;
    kal_uint8                           bd_address[6];  /* BT ADDR for Query RSSI */
    kal_int8                           bt_rssi;
} wmt_bt_query_rssi_ind_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_bool afh;    /* True: send AFH command */
    kal_uint32 freq;
#if (!defined(__BTMODULE_MT6601__))
    kal_uint8 hb;
#endif
    kal_bool pta;    /* True: send PTA command */
    kal_uint8 pta_action;
} wmt_bt_set_bwcs_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint8                           lower_boundary;
    kal_uint8                           higher_boundary;
} wmt_bt_set_rx_range_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint8                           ucPower;
} wmt_bt_set_default_tx_power_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint32                          conn_handle;
    kal_uint8                           ucPower;
} wmt_bt_update_conn_tx_power_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint8                           ucOcf;
    kal_uint8                           ucOgf;
    kal_uint8                           ucPara_len;
} wmt_bt_hci_cmd_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint16                          opcode;
    kal_uint8                           result; /* 1 : host send CMD to controller success, 0: fail */
    kal_uint8                           status;
} wmt_bt_hci_cmd_cnf_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint8                           ucEvent_code;
    kal_uint8                           ucPara_len;
} bt_wmt_hci_event_ind_struct;

/*********************************************************************************
* WIFI
**********************************************************************************/
typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    WMT_WIFIEVENT                       wifi_eventid;
    kal_uint8                           curr_data_rate;
    kal_uint8                           curr_chnl; /* valid only when WMT_WIFI_CONNECTION */
    kal_bool                            curr_ant_usage; /* update ant usage, KAL_TRUE : bt KAL_FALSE : wifi */
} wndrv_wmt_conn_status_update_ind_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_bool                            is_valid;
    kal_int32                           wifi_rssi;
} wmt_wifi_query_rssi_ind_struct;

/* MSG_ID_WMT_WNDRV_SET_ANT_REQ */
typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_bool                            fgAntForBT;
} wmt_wndrv_set_ant_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    BWCS_WIFI_PTA                       eWiFiPta;
    BWCS_PROFILE                        eBwcsProfile;
    WIFI_PS_PROFILE                     eWiFiPsProfile;
    kal_uint32                          wifi_poll_Int; // interval to send poll pkt in ps mode
    kal_bool                            fgUseSingleAntSet;
    /* for single antenna TDD */
    kal_bool                            fgUseRW;     /* True: use remaining window */
    BWCS_PTA_mode                       ePtaMode;
    BWCS_CM_mode                        eCmMode;     /* UCM/SCM */
    kal_uint8                           ucBt_tsco;   /* Tsco=6: CTS NAV=1.25ms, Tsco>6: CTS NAV=2.5ms */
} wmt_wndrv_set_bwcs_req_struct;

/* for single antenna TDD */
typedef struct {
    BWCS_CM_mode                        eCmMode;     /* UCM/SCM */
} wndrv_wmt_cm_mode_update_ind_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint32                          u4MediumTime;
    kal_uint32                          u4PeriodTime;
} wmt_wndrv_set_flow_ctrl_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_bool                            fgEnableFixedRxGain;
} wmt_wndrv_set_fixed_rx_gain_req_struct;

typedef struct {
    kal_uint8                           ref_count;
    kal_uint16                          msg_len;
    kal_uint8                           aucWiFiRates[WMT_MAX_LEN_RATES_EX];
} wmt_wndrv_set_omit_low_rate_req_struct;
#define __WMT_STRUCT_H__
#endif /* __WMT_STRUCT_H__ */
