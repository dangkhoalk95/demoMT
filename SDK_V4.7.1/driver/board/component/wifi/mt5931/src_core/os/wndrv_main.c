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

#define __WNDRV_OS_FILES_C__

//#include "kal_release.h"
//#include "app_ltlcom.h"
//#include "eint.h"
#include "wndrv_cnst.h"
#include "wndrv_cal.h"
//#include "wndrv_private.h"
#include "wndrv_kal.h"
#include "wndrv_os_private.h"
#include "wndrv_context.h"
#include "precomp.h"

/* ------------------------------------------------------------------------- */
extern wndrv_context_struct   wndrv_context;
extern void wndrv_abm_set_ps_mode_req_handler(ilm_struct *ilm_ptr);
extern void wndrv_abm_set_scan_desired_ssid_list_req_handler(ilm_struct *ilm_ptr);
extern void wndrv_abm_set_each_channel_scan_time_req_handler(ilm_struct *ilm_ptr);
extern void wndrv_abm_set_roaming_para_req_handler(ilm_struct *ilm_ptr);
#ifdef __WIFI_SNIFFER_SUPPORT__
extern void wndrv_abm_set_sniffer_mode_req_handler(ilm_struct *ilm_ptr);
#endif

//extern UINT32 control;

typedef void (* wndrv_func_ptr)(ilm_struct *);

const wndrv_func_ptr
wndrv_function_table[] = {
    /* MSG_ID_WNDRV_TEST_SET_REQ */
    (wndrv_func_ptr) wndrv_test_set_handler,
    /* MSG_ID_WNDRV_TEST_QUERY_REQ */
    (wndrv_func_ptr) wndrv_test_query_handler,
    /* MSG_ID_WNDRV_SUPC_DATA_REQ */
    (wndrv_func_ptr) wndrv_supc_data_req_handler,
    /* MSG_ID_WNDRV_SUPC_INIT_REQ */
    (wndrv_func_ptr) wndrv_supc_init_req_handler,
    /* MSG_ID_WNDRV_SUPC_DEINIT_REQ */
    (wndrv_func_ptr) wndrv_supc_deinit_req_handler,
    /* MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ */
    (wndrv_func_ptr) wndrv_supc_site_survey_req_handler,
    /* MSG_ID_WNDRV_SUPC_JOIN_NETWORK_REQ */
    (wndrv_func_ptr) wndrv_supc_join_network_req_hanlder,
    /* MSG_ID_WNDRV_SUPC_DISJOIN_NETWORK_REQ */
    (wndrv_func_ptr) wndrv_supc_disjoin_network_req_handler,
    /* MSG_ID_WNDRV_SUPC_COUNTRY_SETTING_REQ */
    (wndrv_func_ptr) wndrv_supc_country_setting_req_handler,
    /* MSG_ID_WNDRV_SUPC_REMOVE_KEY_REQ */
    (wndrv_func_ptr) wndrv_supc_remove_key_req_handler,
    /* MSG_ID_WNDRV_SUPC_ADD_KEY_REQ */
    (wndrv_func_ptr) wndrv_supc_add_key_req_handler,
    /* MSG_ID_WNDRV_SUPC_PMKID_UPDATE_REQ */
    (wndrv_func_ptr) wndrv_supc_pmkid_update_req_handler,
    /* MSG_ID_WNDRV_SUPC_AUTH_STATE_UPDATE_REQ */
    (wndrv_func_ptr) wndrv_supc_auth_state_update_req_handler,
    /* MSG_ID_WNDRV_SUPC_BLACKLIST_UPDATE_REQ */
    (wndrv_func_ptr) wndrv_supc_blacklist_update_req_handler,
    /* MSG_ID_WNDRV_SUPC_CHIP_POWERON_REQ */
    (wndrv_func_ptr) wndrv_supc_chip_poweron_req_handler,
    /* MSG_ID_WNDRV_SUPC_CHIP_POWEROFF_REQ */
    (wndrv_func_ptr) wndrv_supc_chip_poweroff_req_handler,
    /* MSG_ID_WNDRV_SUPC_QUERY_CAPABILITY_REQ */
    (wndrv_func_ptr) wndrv_supc_query_capability_req_handler,
    /* MSG_ID_TCPIP_WNDRV_DATA_REQ*/
    (wndrv_func_ptr) tcpip_wndrv_data_req_handler,
    /* MSG_ID_TCPIP_WNDRV_SETBEARER_REQ */
    (wndrv_func_ptr) tcpip_wndrv_setbearer_req_handler
};

extern kal_bool  WNDRVPauseFlag;

/*******************************************************************************
*
*  Global Functions
*
*******************************************************************************/
/*****************************************************************************
* FUNCTION
*   wndrv_main
* DESCRIPTION
*   WiFi main function
* PARAMETERS
*   none
* RETURNS
*   none
* GLOBALS AFFECTED
*   none
*****************************************************************************/
void wndrv_main(ilm_struct *ilm_ptr)
{

    WiFi_EintMask();

    if (wndrv_context.glue_p->fgIsAdaptStopped == TRUE) {
        /* 5931 is turn off.
                  only process
                  STA MODE: MSG_ID_WNDRV_SUPC_INIT_REQ
                  AP  MODE: MAP_MSG_ID_HOSTAP_WNDRV_INIT_REQ
            */
        if (MSG_ID_WNDRV_SUPC_INIT_REQ  ==  ilm_ptr->msg_id) { //MSG_ID_WNDRV_SUPC_INIT_REQ
            wndrv_supc_init_req_handler(ilm_ptr);
#if CFG_SUPPORT_INJECT_MSG
        } else if (MSG_ID_TST_INJECT_STRING  == ilm_ptr->msg_id) { //MSG_ID_TST_INJECT_STRING
            wndrv_tst_inject_string_handler(ilm_ptr);
#endif
        } else if (MSG_ID_WNDRV_TEST_SET_REQ  == ilm_ptr->msg_id) { //MSG_ID_WNDRV_TEST_SET_REQ
            wndrv_test_set_handler(ilm_ptr);
        } else if (MSG_ID_WNDRV_TEST_QUERY_REQ  == ilm_ptr->msg_id) { //MSG_ID_WNDRV_TEST_QUERY_REQ
            wndrv_test_query_handler(ilm_ptr);
        } else if (MSG_ID_TCPIP_WNDRV_DATA_REQ == ilm_ptr->msg_id) {
            tcpip_wndrv_data_req_handler(ilm_ptr);
        }
#if CFG_ENABLE_WIFI_TETHERING
        else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_INIT_REQ) {
            hostap_wndrv_init_req_handler(ilm_ptr);
        }
#endif /* CFG_ENABLE_WIFI_TETHERING */
        else {
            MT5931_TRACE(TRACE_ERROR, MT5931_INFO_313, "wndrv_main: Unexpected ilm_ptr->msg_id:%d; fgIsAdaptStopped:%d", ilm_ptr->msg_id, wndrv_context.glue_p->fgIsAdaptStopped);
        }
    } else {
        if ((ilm_ptr->msg_id >= MSG_ID_WNDRV_TEST_SET_REQ) &&   //MSG_ID_WNDRV_TEST_SET_REQ
                (ilm_ptr->msg_id <= MSG_ID_TCPIP_WNDRV_SETBEARER_REQ)) { //MSG_ID_TCPIP_WNDRV_SETBEARER_REQ
            (*(wndrv_function_table[ ilm_ptr->msg_id - MSG_ID_WNDRV_TEST_SET_REQ ]))(ilm_ptr);     //MSG_ID_WNDRV_TEST_SET_REQ
        } else if (MSG_ID_WNDRV_INTERRUPT_IND  ==  ilm_ptr->msg_id) { //MSG_ID_WNDRV_INTERRUPT_IND
            wndrv_interrupt_handler(ilm_ptr);
        } else if (MSG_ID_TIMER_EXPIRY  == ilm_ptr->msg_id) { //MSG_ID_TIMER_EXPIRY
            wndrv_timer_expiry_handler(ilm_ptr);
#if CFG_SUPPORT_INJECT_MSG
        } else if (MSG_ID_TST_INJECT_STRING  == ilm_ptr->msg_id) { //MSG_ID_TST_INJECT_STRING
            wndrv_tst_inject_string_handler(ilm_ptr);
#endif
        } else if ((MSG_ID_WNDRV_SCAN_INDICATION  == ilm_ptr->msg_id) || //MSG_ID_WNDRV_SCAN_INDICATION
                   (MSG_ID_WNDRV_STATISTIC_INDICATION  == ilm_ptr->msg_id)) { //MSG_ID_WNDRV_STATISTIC_INDICATION
        } else if (MSG_ID_SOC_WNDRV_DL_SRPDU_RESUME  == ilm_ptr->msg_id) { //MSG_ID_SOC_WNDRV_DL_SRPDU_RESUME
            WNDRVPauseFlag = KAL_FALSE;
        } else if (MSG_ID_WNDRV_MMI_POWEROFF_REQ  == ilm_ptr->msg_id) { //MSG_ID_WNDRV_MMI_POWEROFF_REQ
            wndrv_mmi_poweroff_req_handler();
        } else if (MSG_ID_WNDRV_ABM_SET_PS_MODE_REQ == ilm_ptr->msg_id) { // Service API
            wndrv_abm_set_ps_mode_req_handler(ilm_ptr);
        } else if (MSG_ID_WNDRV_ABM_SET_SCAN_DESIRED_SSID_LIST_REQ == ilm_ptr->msg_id) {
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
            wndrv_abm_set_scan_desired_ssid_list_req_handler(ilm_ptr);
#endif
        } else if (MSG_ID_WNDRV_ABM_SET_SCAN_DWELL_TIME_REQ == ilm_ptr->msg_id) {
            wndrv_abm_set_each_channel_scan_time_req_handler(ilm_ptr);
        } else if (MSG_ID_WNDRV_ABM_SET_ROAMING_PARA_REQ == ilm_ptr->msg_id) {
            wndrv_abm_set_roaming_para_req_handler(ilm_ptr);
        } else if (MSG_ID_WNDRV_ABM_SET_SNIFFER_MODE_REQ == ilm_ptr->msg_id) {
#ifdef __WIFI_SNIFFER_SUPPORT__
            wndrv_abm_set_sniffer_mode_req_handler(ilm_ptr);
#endif
        }
#if 1//#if CFG_SINGLE_ANTENNA_ENABLED
        else if (MSG_ID_WMT_WNDRV_SET_BWCS_REQ  == ilm_ptr->msg_id) { //MSG_ID_WMT_WNDRV_SET_BWCS_REQ
            wndrv_wmt_set_bwcs_req_handler(ilm_ptr);
        } else if (MSG_ID_WMT_WNDRV_QUERY_RSSI_REQ == ilm_ptr->msg_id) { //MSG_ID_WMT_WNDRV_QUERY_RSSI_REQ
            wndrv_wmt_query_rssi_req_handler(ilm_ptr);
        } else if (MSG_ID_WMT_WNDRV_SET_ANT_REQ  == ilm_ptr->msg_id) { //MSG_ID_WMT_WNDRV_SET_ANT_REQ
            wndrv_wmt_set_ant_req_handler(ilm_ptr);
        } else if (MSG_ID_WMT_WNDRV_SET_FLOW_CTRL_REQ  == ilm_ptr->msg_id) { //MSG_ID_WMT_WNDRV_SET_FLOW_CTRL_REQ
#if 0   //NOTE(Nelson): MT5931 HW is not supported
            wndrv_wmt_set_flow_ctrl_req_handler(ilm_ptr);
#endif
        } else if (MSG_ID_WMT_WNDRV_SET_FIXED_RX_GAIN_REQ  == ilm_ptr->msg_id) { //MSG_ID_WMT_WNDRV_SET_FIXED_RX_GAIN_REQ
#if 0   //NOTE(Nelson): Beta5 FW is not supported
            wndrv_wmt_set_fixed_rx_gain_req_handler(ilm_ptr);
#endif
        } else if (MSG_ID_WMT_WNDRV_SET_OMIT_LOW_RATE_REQ  == ilm_ptr->msg_id) { //MSG_ID_WMT_WNDRV_SET_OMIT_LOW_RATE_REQ
            wndrv_wmt_set_omit_low_rate_req_handler(ilm_ptr);
        }
#endif /* CFG_SINGLE_ANTENNA_ENABLED */
#if CFG_ENABLE_WIFI_TETHERING
        else if ((ilm_ptr->msg_id >= MSG_ID_HOSTAP_WNDRV_INIT_REQ) &&
                 (ilm_ptr->msg_id <= MSG_ID_HOSTAP_WNDRV_FRAME_REQ) ||
                 (ilm_ptr->msg_id == MSG_ID_WNDRV_ABM_SET_TX_PWR_REQ) || (ilm_ptr->msg_id == MSG_ID_FLC_WNDRV_UL_SSPDU_RESUME)) {
            hostap_wndrv_msg_handler(ilm_ptr);
        }
#endif /* CFG_ENABLE_WIFI_TETHERING */
        else if (MSG_ID_ABM_WNDRV_IPNETWORK_STATUS_IND  == ilm_ptr->msg_id) {
            wndrv_abm_dhcp_complete_handler(ilm_ptr);
        } else {
            MT5931_TRACE(TRACE_ERROR, MT5931_INFO_313, "wndrv_main: Unexpected ilm_ptr->msg_id:%d; fgIsAdaptStopped:%d", ilm_ptr->msg_id, wndrv_context.glue_p->fgIsAdaptStopped);
        }
    }

    /* Main thread to process mailbox, CMD, packet tx */
    kalMainThread(wndrv_context.glue_p);

    if (wndrv_context.glue_p->fgIsAdaptStopped == FALSE) {
        WiFi_EintUnMask();
    }
}
