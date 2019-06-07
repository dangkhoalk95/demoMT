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

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif


/* ****************************************************************************
* Alternatively, this software may be distributed under the terms of BSD
* license.
*****************************************************************************/

//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

//#include "kal_non_specific_general_types.h"
//#include "stack_common.h"
#include "stack_msgs.h"
//#include "app_ltlcom.h"           /* Task message communiction */
//#include "syscomp_config.h"
//#include "task_config.h"          /* Task creation */
//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
//#include "app_buff_alloc.h"

//#include "fs_type.h" // Kinki add
//#include "fs_func.h"
//#include "fs_errcode.h"

#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"
#include "supc_abm_msgs.h"

#include "supc_common.h"
#include "eapol_sm.h"
#include "eap.h"
#include "wpa.h"
#include "driver_mtk.h"
#include "driver.h"
#include "eloop.h"
#include "wpa_supplicant.h"
#include "config_ssid.h"
#include "supc_config.h"
#include "l2_packet.h"
#include "wpa_supplicant_i.h"

#include "supc_wpa_functions.h"
#include "supc_wpa_common.h"
//#include "supc_mmi_msgs.h"
#include "wndrv_supc_msg.h"
#include "supc_common.h"
/* For SIM */
//#include "ps2sim_struct.h"
//#include "sim_common_enums.h"

#include "stack_ltlcom.h"
//#include "app_ltlcom.h"

//#include "supc_trc.h" /* message database */

/*mtk80707 FOR WPS*/
#ifdef __WPS_SUPPORT__
#include "supc_wps.h"
#include "wps_i.h"
#include "wpabuf.h"
#include "wps_supplicant.h"
#endif

#include "eap_i.h"

/*MAUI_03055418*/
#ifdef __CERTMAN_SUPPORT__
#include "ssl_api.h"
#include "supc_ssl.h"
#endif

#define EVENT_PARAM_WPA_SCAN            0x01
#define EVENT_PARAM_REPORT_MMI          0x02
#define EVENT_PARAM_WPA_AUTH            0x03
//
//#define EVENT_PARAM_WAPI_AUTH           0x04
#define EVENT_PARAM_WPS_WALK_TIME       0x04


#define EVENT_PARAM_PREAUTH             0x05
#define EVENT_PARAM_WPA_HANDOVER        0x06
#define EVENT_PARAM_WPA_COUNTERMEASURE  0x07
#define EVENT_PARAM_PMK_CACHE           0x08
//#define EVENT_PARAM_TMP_BLACK_LIST      0x09
#define EVENT_PARAM_PBC_REGISTTER_TIME  0x09
//#define EVENT_PARAM_PERM_BLACK_LIST     0x0a
#define EVENT_PARAM_WPS_REG_SET_SELECTED 0x0a
#define EVENT_PARAM_SCAN_MONITOR        0x0b
#define EVENT_PARAM_EAPOL_TIKER         0x0c

extern void supc_cb_disconnected(void);
extern void supc_cb_connected(void);
#ifdef WIFI_BLACKLIST
extern kal_uint32 supc_auth_timeout_num(void);
#endif

kal_bool supc_is_logoff_needed(void)
{
    return KAL_FALSE;
}
#ifdef __WAPI_SUPPORT__
extern void WAPI_timer_run(void);
extern kal_bool WAPI_callback_check(void);
void wapi_deinit(struct wpa_supplicant *wpa_s);
void wpa_supplicant_req_wapi_auth_timeout(struct wpa_supplicant *wpa_s,
        int sec, int usec);
#endif

/*
 SUPC supports auto-connect mechanism. in this case,
 SUPC will retry profiles passed by ABM after failure,
 whether send CONN_CNF or not.
*/
kal_bool supc_allow_autoconn(void)
{
    return KAL_FALSE;
}

kal_bool fgSiteSurveyReq = KAL_FALSE;/* MAUI_02639824 */
/*Arima EAP issue*/
const kal_uint8 eap_sim_realm[] = "@wlan.mnc000.mcc000.3gppnetwork.org";


/*********************************************************************
   Function Name    : supc_stop_scan_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_scan_timer(void)
{
    wpa_printf(TRACE_GROUP_1, "supc_stop_scan_timer\n");
//  kal_trace(DBG_TIMER,INFO_SUPC_STOP_SCAN_TIMER);
    if (NULL != wpa_s->scan_timer_event_id) {
        //kal_prompt_trace(MAP_MOD_SUPC, "Stop scan event id: %x", wpa_s->scan_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_SCAN_EVENT_ID,wpa_s->scan_timer_event_id);

        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->scan_timer_event_id));

        wpa_s->scan_timer_event_id = NULL;
    }
}

/*********************************************************************
   Function Name    : supc_stop_report_mmi_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/

void supc_stop_report_mmi_timer(void)
{
    INNER_TRACE_STEP(1);
    if (NULL != wpa_s->report_mmi_timer_event_id) {
        //wpa_printf(MSG_DBG_TIMER, "Stop report mmi event id: 0x%x",
        //wpa_s->report_mmi_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_REPROT_MMI_TIMER, wpa_s->report_mmi_timer_event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->report_mmi_timer_event_id));

        wpa_s->report_mmi_timer_event_id = NULL;
    }
}

/*********************************************************************
   Function Name    : supc_stop_countermeasure_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_countermeasure_timer(void)
{
    if (NULL != wpa_s->countermeasure_timer_event_id) {
        //kal_prompt_trace(MAP_MOD_SUPC, "Stop coutnermeasure event id: %x", wpa_s->countermeasure_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_CONNTERMEASURE_TIMER, wpa_s->countermeasure_timer_event_id);

        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->countermeasure_timer_event_id));

        wpa_s->countermeasure_timer_event_id = NULL;
    }
}

/*********************************************************************
   Function Name    : supc_stop_auth_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_auth_timer(void)
{

    if (NULL != wpa_s->auth_timer_event_id) {
        wpa_printf(TRACE_GROUP_1, "Stop auth event id: %x\n", wpa_s->auth_timer_event_id);
//        kal_trace(DBG_TIMER,INFO_SUPC_STOP_AUTH_EVENT_ID,wpa_s->auth_timer_event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->auth_timer_event_id));

        wpa_s->auth_timer_event_id = NULL;
    }
}
#ifdef __WAPI_SUPPORT__
/*********************************************************************
   Function Name    : supc_stop_auth_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_wapi_auth_timer(void)
{

    if (NULL != wpa_s->wapi_auth_timer_event_id) {
        //kal_prompt_trace(MAP_MOD_SUPC, "Stop WAPI auth event id: %x", wpa_s->wapi_auth_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_WAPI_AUTH_TIMER,wpa_s->wapi_auth_timer_event_id);

        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->wapi_auth_timer_event_id));

        wpa_s->wapi_auth_timer_event_id = NULL;
    }
}
#endif

/*********************************************************************
   Function Name    : supc_stop_handover_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_handover_timer(void)
{
    if (NULL != wpa_s->handover_timer_event_id) {
        wpa_printf(TRACE_GROUP_1, "stop handover timer id=0x%x\n",
                   wpa_s->handover_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_HANDOVER_TIMER,wpa_s->handover_timer_event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->handover_timer_event_id));

        wpa_s->handover_timer_event_id = NULL;
    }
}

/*********************************************************************
   Function Name    : supc_stop_preauth_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_preauth_timer(void)
{
    if (NULL != wpa_s->preauth_timer_event_id) {

        //kal_prompt_trace(MAP_MOD_SUPC, "Stop preauth event id: %x", wpa_s->preauth_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_PRE_AUTH_TIMER,wpa_s->preauth_timer_event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->preauth_timer_event_id));

        wpa_s->preauth_timer_event_id = NULL;
    }
}

/*********************************************************************
   Function Name    : supc_stop_pmksa_cache_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : rsn_pmksa_cache *pmksa_cache_ptr
   Description      :
*********************************************************************/
void supc_stop_pmksa_cache_timer(rsn_pmksa_cache *pmksa_cache_ptr)
{
    if (NULL != pmksa_cache_ptr->event_id) {
        wpa_printf(TRACE_GROUP_1, "Stop PMKSA cache timer, event id : %x\n", pmksa_cache_ptr->event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_PMKSA_CACHE_TIMER,pmksa_cache_ptr->event_id);

        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(pmksa_cache_ptr->event_id));

        pmksa_cache_ptr->event_id = NULL;
    }
}

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
/*********************************************************************
   Function Name    : supc_stop_temporal_blacklist_timer_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_temporal_blacklist_timer(void)
{
    if (NULL != wpa_s->temporal_blacklist_timer_event_id) {

        //kal_prompt_trace(MAP_MOD_SUPC, "Stop temporal blacklist event id: %x", wpa_s->temporal_blacklist_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_T_BLACKLIST_TIMER,wpa_s->temporal_blacklist_timer_event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->temporal_blacklist_timer_event_id));

        wpa_s->temporal_blacklist_timer_event_id = NULL;
    }
}

/*********************************************************************
   Function Name    : supc_stop_permanent_blacklist_timer_timer
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_stop_permanent_blacklist_timer(void)
{
    if (NULL != wpa_s->permanent_blacklist_timer_event_id) {

        //kal_prompt_trace(MAP_MOD_SUPC, "Stop permanent blacklist event id: %x", wpa_s->permanent_blacklist_timer_event_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_P_BLACKLIST_TIMER,wpa_s->permanent_blacklist_timer_event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->permanent_blacklist_timer_event_id));

        wpa_s->permanent_blacklist_timer_event_id = NULL;
    }
}
#endif

/*********************************************************************
   Function Name    : supc_start_wpa_scan_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_wpa_scan_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = 0;
    evid = (evshed_set_event((wpa_timer_context_g.wpa_event_scheduler_ptr),
                             supc_wpa_scan_timer_expiry_handler,
                             (void *)EVENT_PARAM_WPA_SCAN,
                             (delay * KAL_TICKS_1_SEC)));
    //kal_prompt_trace(MAP_MOD_SUPC, "Start scan timer event id: %x", evid);
//  kal_trace(DBG_TIMER,INFO_SUPC_START_SCAN_TIMER,evid);
    return evid;
}

/*********************************************************************
   Function Name    : supc_start_report_mmi_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_report_mmi_timer(kal_uint32 delay)
{

    return NULL;
    // The unit of delay is in seconds
    eventid evid = 0;
    evid = (evshed_set_event((wpa_timer_context_g.wpa_event_scheduler_ptr),
                             supc_report_mmi_timer_expiry_handler,
                             (void *)EVENT_PARAM_REPORT_MMI,
                             (delay * KAL_TICKS_1_SEC)));
    wpa_printf(TRACE_GROUP_1, "Start report mmi timer event id: %x after %d sec\n", evid, delay);
//  kal_trace(DBG_TIMER,INFO_SUPC_START_REPROT_MMI_TIMER,evid, delay);

    return evid;
}

void supc_prepare_disconnect()
{
    u8 *addr = NULL;

    wpa_s->num_of_profile = 0;
    wpa_s->wpa_state = WPA_DISCONNECTED;

    if (supc_is_logoff_needed()) {
        if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X ||
                wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
            eapol_sm_notify_logoff(wpa_s->eapol, TRUE);
            kal_sleep_task(5);  // need to delay for a short period of time so that driver can send out the logoff frame.
        }
    }

    addr = wpa_s->bssid;
    wpa_clear_keys(wpa_s, addr);
    kal_mem_cpy(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN);
}

void supc_after_mmi_conn_cnf_fail()
{
    wpa_ssid *pnext;
    if (wpa_s->conf != NULL) {
        while (wpa_s->conf->ssid != NULL) {
            supc_wpa_free_ssid(wpa_s->conf->ssid);
            pnext = wpa_s->conf->ssid->next;
            SUPC_DEALLOC_BUFF(wpa_s->conf->ssid);
            wpa_s->conf->ssid = pnext;
        }
        SUPC_DEALLOC_BUFF(wpa_s->conf);
        wpa_s->conf = NULL;
    }

    SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
    wpa_s->ap_wpa_ie = NULL;
    SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
    wpa_s->ap_rsn_ie = NULL;
#ifdef __WAPI_SUPPORT__
    if (wpa_s->ap_wapi_ie) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wapi_ie);
        wpa_s->ap_wapi_ie = NULL;
    }
#endif
    if (wpa_s->assoc_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
        wpa_s->assoc_wpa_ie = NULL;
        wpa_s->assoc_wpa_ie_len = 0;
    }

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_listed_clear_success(wpa_s); //Kinki add for MAUI_00401655
    wpa_auth_timeout_list_clear(wpa_s);
    wpa_blacklist_clear_temporal(wpa_s);
    wpa_blacklist_clear_permanent(wpa_s);
    supc_blacklist_update(wpa_s);
#else
    wpa_blacklist_clear(wpa_s);
#endif

#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif
    supc_stop_scan_timer();
    supc_stop_auth_timer();
    supc_stop_handover_timer();
#ifdef __WAPI_SUPPORT__
    supc_stop_wapi_auth_timer();
#endif

    wpa_supplicant_mark_disassoc(wpa_s);
    wpa_s->current_ssid = NULL;
    eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);

    eapol_sm_deinit(wpa_s->eapol);
    wpa_s->eapol = NULL;
    eapol_sm_deinit(wpa_s->preauth_eapol);
    wpa_s->preauth_eapol = NULL;
#ifndef  WIFI_AUTH_PSK_ONLY
#ifdef __CERTMAN_SUPPORT__
    /*MAUI_03055418 preauth eapol & eapol use the same ssl context*/
    if (wpa_s->support_pre_auth) {
        if (wpa_s->g_ssl_ctx) {
            sec_ssl_ctx_free((ssl_ctx *)wpa_s->g_ssl_ctx);
            wpa_s->g_ssl_ctx = NULL;
        }
    }
#endif
#endif
    wpa_s->num_scan_results = 0;

}
/*********************************************************************
   Function Name    : supc_report_mmi_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void * para_ptr
   Description      :
*********************************************************************/
void supc_report_mmi_timer_expiry_handler(void *para_ptr)
{
    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;

    wpa_printf(MSG_DEBUG, "supc_report_mmi_timer_expiry_handler\n");
//  kal_trace(DBG_TIMER,INFO_SUPC_REPROT_MMI_TIMER_OUT);
    SUPC_ASSERT(EVENT_PARAM_REPORT_MMI == (int)para_ptr);
    wpa_s->report_mmi_timer_event_id = NULL;

    if (supc_allow_autoconn() == KAL_FALSE) {
        supc_prepare_disconnect();
    }

    if (KAL_FALSE == wpa_s->is_reported_mmi_fail) {
        //cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
        cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_RESET);

        cnf_local_para_ptr->result = KAL_FALSE;

        //add for WiFi UE enhancement
        wpa_printf(MSG_DEBUG, "eap fail cause : %d\n", wpa_s->eap_fail_cause);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_FAIL_CAUSE,wpa_s->eap_fail_cause);
        cnf_local_para_ptr->cause = wpa_s->eap_fail_cause;

        cnf_local_para_ptr->channel_number = 0;
        if (wpa_s->current_ssid != NULL) {
            cnf_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;    //Kinki change for MoDIS
        } else {
            cnf_local_para_ptr->profile_id = wpa_s->prev_scan_ssid->profile_id;
        }

        //add for WiFi UE enhancement
        /*fix build warning, MAUI_03120978, set auth_type to invalid value*/
        cnf_local_para_ptr->auth_type = SUPC_AUTH_MODE_TOTAL;
        cnf_local_para_ptr->eap_peap_auth_type = 0;
        cnf_local_para_ptr->eap_ttls_auth_type = 0;

        /*clear wps data*/
        cnf_local_para_ptr->passphase_len = 0;
        cnf_local_para_ptr->conn_type = wpa_s->connection_type;
        cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

        wpa_s->wpa_scan_retry_counter = 0xff;
        wpa_s->is_reported_mmi_fail = KAL_TRUE;
        wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
        supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);
        //MAUI_02140735
        //20100127 SAKER : sync ABM and WNDRV state after supc_send_abm_wifi_connect_cnf
        supc_reinit_wpa();

        /*mtk80707, if supc report mmi connect fail, it means all profiles
        are be tried and fail to connect. so profiles need to be update at
        next connect req*/
        wpa_s->num_of_profile = 0;
        //wpa_printf(MSG_DEBUG, "zero profile number", wpa_s->eap_fail_cause);

#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->chip_state == CHIP_POWER_ON) { //Kinki add for power-saving
            supc_send_wndrv_disjoin_network_req(); //Kinki add for power-saving

            wpa_s->chip_state = CHIP_POWER_OFF;
            supc_send_wndrv_chip_poweroff_req();
        }
#else
        supc_send_wndrv_disjoin_network_req();
#endif

    }
    if (supc_allow_autoconn() == KAL_FALSE) {
        /*if supc decide to report connect_cnf to mmi,
        supc must stop scan and all timer, and reset wpa sm state.
        */
        supc_after_mmi_conn_cnf_fail();
    }
}

#ifdef __WPS_SUPPORT__
/*
return wpa_type: WPS_TYPE_PIN or PBC
*/
int wpas_wps_in_use(struct wpa_config *conf,
                    enum wps_request_type *req_type) {
    struct wpa_ssid *ssid;
    int wps = 0;
    if (NULL == conf)
    {
        wpa_printf(TRACE_GROUP_1, "%s wps->conf is NULL\n", __func__);
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_CONF_NULL);
        return 0;
    }

    for (ssid = conf->ssid; NULL != ssid; ssid = ssid->next)
    {
        INNER_TRACE_STEP(1);
        if (ssid->ssid_len) {
            printc("ssid addr = %p, ssid=%c%c%c%c, ssid->key_mgmt=%d\n", ssid, ssid->ssid[0], ssid->ssid[1], ssid->ssid[2], ssid->ssid[3], ssid->key_mgmt);
        }
        if (!(ssid->key_mgmt & WPA_KEY_MGMT_WPS)) {
            continue;
        }
        INNER_TRACE_STEP(2);

        wps = WPS_TYPE_PIN;
        if (req_type) {
            *req_type = wpas_wps_get_req_type(ssid);
        }
        if (!ssid->phase1) {
            continue;
        }

        if (strstr((char *)ssid->phase1, "pbc=1")) {
            return WPS_TYPE_PBC;
        }
    }

    return wps;
}
#endif /* __WPS_SUPPORT__ */


/*********************************************************************
   Function Name    : supc_wpa_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void * para_ptr
   Description      :
*********************************************************************/
extern kal_uint32 max_scan_interval_index;
void supc_wpa_scan_timer_expiry_handler(void *para_ptr)
{
    int prio;
    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;
    kal_uint32 max_try_time = 0;
#ifdef __WPS_SUPPORT__
    kal_uint32 wps_method = 0;
    enum wps_request_type req_type = WPS_REQ_ENROLLEE_INFO;
#endif

    wpa_printf(TRACE_GROUP_1, "supc_wpa_scan_timer_expiry_handler\n");
//  kal_trace(DBG_TIMER, INFO_SUPC_SCAN_TIMER_OUT);

    SUPC_ASSERT(EVENT_PARAM_WPA_SCAN == (int)para_ptr);
    /* Joseph, if it's now AP_AUTOMATIC_SELECT mode, the searching for APs will keep
     * working till user break event
     */
    if (AP_MANUAL_SELECT == wpa_s->ap_selection_mode && wpa_s->scan_interval_index < max_scan_interval_index) {
        supc_send_wndrv_site_survey_req(wpa_s->prev_scan_ssid->ssid, wpa_s->prev_scan_ssid->ssid_len);
        return;
    }
    if (AP_MANUAL_SELECT == wpa_s->ap_selection_mode) {
        /* Joseph: Things shall be done here
         * 1. Stop sending scan req and report ABM ACT_REJ
         * 2. Clean every the WPA resource
         * 3. Send the necessary message to network driver to help clean the resource
         */
        //cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
        cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_RESET);
        cnf_local_para_ptr->result = KAL_FALSE;
        cnf_local_para_ptr->cause = SUPC_ABM_TIMEOUT;
        cnf_local_para_ptr->channel_number = 0;
        if (wpa_s->current_ssid != NULL) {
            cnf_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;    //Kinki change for MoDIS
        } else {
            cnf_local_para_ptr->profile_id = wpa_s->prev_scan_ssid->profile_id;
        }

        //add for WiFi UE enhancement
        cnf_local_para_ptr->auth_type = SUPC_AUTH_MODE_TOTAL;
        cnf_local_para_ptr->eap_peap_auth_type = 0;
        cnf_local_para_ptr->eap_ttls_auth_type = 0;

        /*clear wps data*/
        cnf_local_para_ptr->passphase_len = 0;
        cnf_local_para_ptr->conn_type = wpa_s->connection_type;
        cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

        INNER_TRACE_STEP(1);
        wpa_s->is_reported_mmi_fail = KAL_TRUE;
        supc_stop_report_mmi_timer();

        wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
        supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);
        supc_reinit_wpa();

#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->chip_state == CHIP_POWER_ON) { //Kinki add for power-saving
            supc_send_wndrv_disjoin_network_req(); //Kinki add for power-saving

            wpa_s->chip_state = CHIP_POWER_OFF;
            supc_send_wndrv_chip_poweroff_req();
        }
#else
        supc_send_wndrv_disjoin_network_req();
#endif

        return;
    }

    /* Joseph,
     * It shall be AP_AUTOMATIC_SELECT to come here.
     * We have to return connect_cnf with failed cause if we
     * scan more than WPA_MAX_SCAN_RETRY times and keeping
     * doing scan
     */
    max_try_time = WPA_MAX_SCAN_RETRY;
#ifdef __WPS_SUPPORT__
    /*WPS need more retry time, because it's particular scenario*/
    if (0 != wpas_wps_in_use(wpa_s->conf, &req_type)) {
        max_try_time = (WPS_PBC_WALK_TIME / WPS_SCAN_INTERVAL);
    }
#endif
    wpa_printf(TRACE_GROUP_1, "supc_scan_timer_expire %d of %d,\n",
               wpa_s->wpa_scan_retry_counter, max_try_time);
//  kal_trace(DBG_TIMER, INFO_SUPC_SCAN_TIMER_RETRY,wpa_s->wpa_scan_retry_counter, max_try_time);

    wpa_s->scan_timer_event_id = NULL;
#ifdef __WPS_SUPPORT__
    wps_method = wpas_wps_in_use(wpa_s->conf, &req_type);
    /*wps_method=1: pin_method, wps_method=2: pbc_method*/
    if (wps_method) {
        wpa_s->prev_scan_ssid = NULL;
        wpa_s->num_scan_results = 0;
        wpa_s->is_site_survey_req_pending = KAL_TRUE;
#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
            wpa_s->chip_state = CHIP_POWER_ON;
            supc_send_wndrv_chip_poweron_req();
        }
#endif
        supc_send_wndrv_wps_sit_survey_req(wpa_s->wps,
                                           (WPS_TYPE_PBC == wps_method ? KAL_TRUE : KAL_FALSE), req_type);
        wpa_printf(TRACE_GROUP_1, "WPS-mode scanning\n");
//      kal_trace(DBG_WPS, INFO_SUPC_WPS_MODE_SCANNING);
        return;
    } else {
        wpa_printf(TRACE_GROUP_1, "no WPS-mode in used, refresh all APs\n");
//      kal_trace(DBG_WPS, INFO_SUPC_NO_WPS_MODE_REFRESH);
    }
#endif

    // supc_send_wndrv_site_survey_req(wpa_s->prev_scan_ssid->ssid, wpa_s->prev_scan_ssid->ssid_len);
    for (prio = 0; prio < SUPC_MAX_SSID_CONFIG_PRIORITY && wpa_s->conf; prio++) {
        if (wpa_s->conf->pssid[prio] != NULL) {
            if (wpa_s->conf->pssid[prio]->network_type == WNDRV_SUPC_NETWORK_IBSS) {
                //Kinki add for IBSS power-saving: do not scan IBSS network
                break;
            }
            wpa_s->prev_scan_ssid = wpa_s->conf->pssid[prio];
            wpa_s->num_scan_results = 0;
            wpa_s->is_site_survey_req_pending = KAL_TRUE; /*Kinki add*/
#ifdef __WIFI_CHIP_DYM_POWERON__
            //Kinki add for power-saving
            if (wpa_s->chip_state != CHIP_POWER_ON) {
                wpa_s->chip_state = CHIP_POWER_ON;
                supc_send_wndrv_chip_poweron_req();
            }
#endif
            printc("wpa_s->pre_scan_ssid=%c%c%c%c, wpa_s->prev_scan_ssid->ssid_len=%d\n", wpa_s->prev_scan_ssid->ssid[0],
                   wpa_s->prev_scan_ssid->ssid[1], wpa_s->prev_scan_ssid->ssid[2], wpa_s->prev_scan_ssid->ssid[3], wpa_s->prev_scan_ssid->ssid_len);
            supc_send_wndrv_site_survey_req(wpa_s->prev_scan_ssid->ssid,
                                            wpa_s->prev_scan_ssid->ssid_len);
            break;
        }
    }
}



/*********************************************************************
   Function Name    : supc_wpa_auth_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_wpa_auth_timer_expiry_handler(void *para_ptr)
{
#ifdef WIFI_BLACKLIST
    kal_uint32 num = 0; // Kinki change for the blacklist
#endif
    wpa_printf(TRACE_GROUP_1, "supc_wpa_auth_timer_expiry_handler\n");
//  kal_trace(DBG_TIMER, INFO_SUPC_AUTH_TIMER_OUT);

    SUPC_ASSERT(EVENT_PARAM_WPA_AUTH == (int)para_ptr);

    supc_dbg_dump_supplicant();
#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif
    /*mtk80707. in WPS, connect confirm must be sent
    after real wifi connection done*/
#ifdef __WPS_SUPPORT__
    if (1 == wpa_s->wps_success) {
        wpa_printf(TRACE_GROUP_1, "WPS DONE, skip blacklist procedure\n");
//      kal_trace(DBG_WPS, INFO_SUPC_WPS_DONE_SKIP_BLACKLIST);
        goto skip;
    }
#endif

#ifdef WIFI_BLACKLIST
    // Kinki add for MAUI_00401655
    if (!wpa_listed_success(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id)) {
        wpa_auth_timeout_list_add(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);

        num = wpa_auth_timeout_num_get(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);

        SUPC_ASSERT(num > 0);

        if (num >= supc_auth_timeout_num()) {
            wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);

            if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
                wpa_blacklist_add_permanent(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
            } else {
                wpa_blacklist_add_temporal(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
            }

            supc_blacklist_update(wpa_s);
        }
    }
#else
    wpa_printf(TRACE_GROUP_1, "Authentication with " MACSTR " timed out.\n",
               MAC2STR(wpa_s->bssid));
//  kal_trace(DBG_TIMER, INFO_SUPC_AUTH_BSS_TIMEOUT, MAC2STR(wpa_s->bssid));

    wpa_blacklist_add(wpa_s, wpa_s->bssid);
#endif

#ifdef __WPS_SUPPORT__

skip:

#endif
    // We always tried to use PMK caching when connecting with
    // an AP we ever connected with. If the AP does not support
    // PMK caching, the authentication always fails.
    // Delete the PMK caching for that AP so that we could
    // restart 802.1x authentication next time.
    // TODO: in that case, we should not add AP to blacklist.
    pmksa_cache_del(wpa_s->bssid);
    wpa_supplicant_disassociate(wpa_s, REASON_DEAUTH_LEAVING);
    wpa_s->reassociate = 1;

    wpa_s->auth_timer_event_id = NULL;
    wpa_s->wpa_state = WPA_SCANNING;
    wpa_supplicant_req_scan(wpa_s, 0, 0);
    /* MAUI_02928725 */
    if (wpa_s->handover_timer_event_id == NULL) {
        wpa_s->handover_timer_event_id = supc_start_wpa_handover_timer(wpa_s->handover_timer_value);
    }

}

#ifdef __WAPI_SUPPORT__
/*********************************************************************
   Function Name    : supc_wpa_auth_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_wapi_auth_timer_expiry_handler(void *para_ptr)
{

    SUPC_ASSERT(EVENT_PARAM_WAPI_AUTH == (int)para_ptr);
//    kal_prompt_trace(MAP_MOD_SUPC, "WAPI auth timer expiry");
//    kal_trace( DBG_WAPI, INFO_SUPC_WAPI_TIME_OUT);

    /* Invoke WAPI timer call back */
    WAPI_timer_run();

    /* Schedule next time. 1 sec */
    wpa_supplicant_req_wapi_auth_timeout(wpa_s, 1, 0);
}
#endif
/*********************************************************************
   Function Name    : supc_wpa_handover_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_wpa_handover_timer_expiry_handler(void *para_ptr)
{
    SUPC_ASSERT(EVENT_PARAM_WPA_HANDOVER == (int)para_ptr);

    INNER_TRACE_STEP(1);
    supc_dbg_dump_supplicant();

    /*
    handover timer expiry means supc fail to assoc & authen with current network.
    supc must add current bssid into blacklist.
    */

#ifdef WIFI_BLACKLIST
    // Kinki add for MAUI_00401655
    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
        wpa_blacklist_add_permanent(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    } else {
        wpa_blacklist_add_temporal(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    }

    supc_blacklist_update(wpa_s);
#else
    wpa_printf(MSG_INFO, "Authentication with " MACSTR " timed out.\n",
               MAC2STR(wpa_s->bssid));

//  kal_trace(DBG_TIMER, INFO_SUPC_AUTH_BSS_TIMEOUT, MAC2STR(wpa_s->bssid));

    wpa_blacklist_add(wpa_s, wpa_s->bssid);
#endif

    /* Joseph, todos
     * 1. set handover timer id to NULL
     * 2. send disconnect_ind to abm
     */

    wpa_s->handover_timer_event_id = NULL;

    /* CMCCTD to MAUI
    */
    supc_send_abm_wifi_disconnect_ind(wpa_s->eap_fail_cause);

    /*
       [MAUI_02162995] [WIFI] The behavior is strange
       Saker : disjoin req let wndrv leave IBSS mode
    */
#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state == CHIP_POWER_ON) { //Kinki add for power-saving
        supc_send_wndrv_disjoin_network_req(); //Kinki add for power-saving

        wpa_s->chip_state = CHIP_POWER_OFF;
        supc_send_wndrv_chip_poweroff_req();
    }
#else
    supc_send_wndrv_disjoin_network_req();
#endif

}

/*********************************************************************
   Function Name    : supc_wpa_countermeasure_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_wpa_countermeasure_timer_expiry_handler(void *para_ptr)
{
    SUPC_ASSERT(EVENT_PARAM_WPA_COUNTERMEASURE == (int)para_ptr);
    //kal_prompt_trace(MAP_MOD_SUPC, "WPA countermeasure timer expiry");
//  kal_trace(DBG_TIMER,INFO_SUPC_CONNTERMEASURE_TIMER_OUT);
    wpa_s->countermeasure_timer_event_id = NULL;

    if (wpa_s->countermeasures) {
        wpa_s->countermeasures = 0;
        //wpa_msg(wpa_s, MSG_INFO, "TKIP countermeasures stopped");
//      kal_trace(DBG_TIMER,INFO_SUPC_CONNTERMEASURE_STOPED);
        wpa_s->wpa_state = WPA_SCANNING;
        wpa_supplicant_req_scan(wpa_s, 0, 0);
    }

}
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
//Kinki
/*********************************************************************
   Function Name    : supc_wpa_temporal_blacklist_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_wpa_temporal_blacklist_timer_expiry_handler(void *para_ptr)
{
    SUPC_ASSERT(EVENT_PARAM_TMP_BLACK_LIST == (int)para_ptr);
    wpa_printf(TRACE_GROUP_1, "Temporal blacklist is expired \n");
//  kal_trace(DBG_TIMER,INFO_SUPC_T_BLACKLIST_TIMER_OUT);
    wpa_s->temporal_blacklist_timer_event_id = NULL;
    wpa_auth_timeout_list_clear(wpa_s);
    wpa_blacklist_clear_temporal(wpa_s);
    supc_blacklist_update(wpa_s);
}

/*********************************************************************
   Function Name    : supc_wpa_permanent_blacklist_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_wpa_permanent_blacklist_timer_expiry_handler(void *para_ptr)
{
    SUPC_ASSERT(EVENT_PARAM_PERM_BLACK_LIST == (int)para_ptr);
    wpa_printf(MSG_INFO, "Permanent blacklist is expired \n");
//  kal_trace(DBG_TIMER,INFO_SUPC_P_BLACKLIST_TIMER_OUT);
    wpa_s->permanent_blacklist_timer_event_id = NULL;
    wpa_blacklist_clear_permanent(wpa_s);
    supc_blacklist_update(wpa_s);
}
#endif

/*********************************************************************
   Function Name    : supc_start_wpa_auth_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_wpa_auth_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = 0;
    INNER_TRACE_STEP(1);
    if (delay > 0) {
        supc_stop_auth_timer();

        evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                 supc_wpa_auth_timer_expiry_handler,
                                 (void *)EVENT_PARAM_WPA_AUTH,
                                 (delay * KAL_TICKS_1_SEC)));

        //kal_prompt_trace(MAP_MOD_SUPC, "Start auth timer event id: %x, delay: %d sec", evid, delay);
//  kal_trace(DBG_TIMER,INFO_SUPC_START_AUTH_EVENT_ID, evid, delay);
    }
    return evid;
}

#ifdef __WAPI_SUPPORT__
/*********************************************************************
   Function Name    : supc_start_wpa_auth_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_wapi_auth_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = 0;
    if (delay > 0) {
        supc_stop_wapi_auth_timer();
        /* check WAPI callback if exist */
        if (WAPI_callback_check()) {
            evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                     supc_wapi_auth_timer_expiry_handler,
                                     (void *)EVENT_PARAM_WAPI_AUTH,
                                     (delay * KAL_TICKS_1_SEC)));

            //kal_prompt_trace(MAP_MOD_SUPC, "Start auth timer event id: %x, delay: %d sec", evid, delay);
//          kal_trace(DBG_TIMER,INFO_SUPC_START_WAPI_AUTH_EVENT_ID, evid, delay);

        } else {
            //kal_wap_trace(MAP_MOD_SUPC, TRACE_WARNING, "pass start auth timer due to WAPI already deinit");
//          kal_trace(DBG_TIMER,INFO_SUPC_PASS_WAPI_AUTH_TIMER);
        }
    }
    return evid;
}
#endif
/*********************************************************************
   Function Name    : supc_start_preauth_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_preauth_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = NULL;

    if (delay > 0) {
        supc_stop_preauth_timer();

        evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                 supc_preauth_timer_expiry_handler,
                                 (void *)EVENT_PARAM_PREAUTH,
                                 (delay * KAL_TICKS_1_SEC)));

        //kal_prompt_trace(MAP_MOD_SUPC, "Start preauth timer event id: %x, delay: %d sec", evid, delay);
//   kal_trace(DBG_TIMER,INFO_SUPC_START_PRE_AUTH_EVENT_ID, evid, delay);
    }

    return evid;


    // The unit of delay is in seconds
    /*return (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
        supc_preauth_timer_expiry_handler,
        NULL,
        (delay * KAL_TICKS_1_SEC)));
    */
}

/*********************************************************************
   Function Name    : supc_start_wpa_handover_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_wpa_handover_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = NULL;

    if (delay > 0) {
        //supc_stop_handover_timer();

        evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                 supc_wpa_handover_timer_expiry_handler,
                                 (void *)EVENT_PARAM_WPA_HANDOVER,
                                 (delay * KAL_TICKS_1_SEC)));

        //kal_prompt_trace(MAP_MOD_SUPC, "Start handover timer event id: %x, delay: %d sec", evid, delay);
//   kal_trace(DBG_TIMER,INFO_SUPC_START_HANDOVER_EVENT_ID, evid, delay);
    }

    return evid;
}

/*********************************************************************
   Function Name    : supc_start_wpa_countermeasure_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_wpa_countermeasure_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = 0;

    evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                             supc_wpa_countermeasure_timer_expiry_handler,
                             (void *)EVENT_PARAM_WPA_COUNTERMEASURE,
                             (delay * KAL_TICKS_1_SEC)));

    //kal_prompt_trace(MAP_MOD_SUPC, "Start wpa countermeasure timer event id: %x, delay: %d sec", evid, delay);
//  kal_trace(DBG_TIMER,INFO_SUPC_START_COUNTERMEASURE_EVENT_ID, evid, delay);

    return evid;
}

/*********************************************************************
   Function Name    : supc_start_pmksa_cache_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : rsn_pmksa_cache *pmksa_cache_ptr
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_pmksa_cache_timer(rsn_pmksa_cache *pmksa_cache_ptr, kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = NULL;
    if (delay > 0) {
        supc_stop_pmksa_cache_timer(pmksa_cache_ptr);

        evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                 supc_pmksa_timer_expiry_handler,
                                 pmksa_cache_ptr,
                                 (delay * KAL_TICKS_1_SEC)));

        //kal_prompt_trace(MAP_MOD_SUPC, "Start pmksa cache timer event id: %x, delay: %d sec", evid, delay);
//         kal_trace(DBG_TIMER,INFO_SUPC_START_PMKSA_CACHE_EVENT_ID, evid, delay);
    }

    return evid;
}
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
//Kinki
/*********************************************************************
   Function Name    : supc_start_temporal_blacklist_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_temporal_blacklist_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = NULL;

    if (delay > 0) {
        supc_stop_temporal_blacklist_timer();
        evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                 supc_wpa_temporal_blacklist_timer_expiry_handler,
                                 (void *)EVENT_PARAM_TMP_BLACK_LIST,
                                 (delay * KAL_TICKS_1_SEC)));
        //kal_prompt_trace(MAP_MOD_SUPC, "Start temporal blacklist timer event id: %x, delay: %d sec", evid, delay);
//      kal_trace(DBG_TIMER,INFO_SUPC_START_T_BLACKLIST_EVENT_ID, evid, delay);
    }

    return evid;
}

/*********************************************************************
   Function Name    : supc_start_permanent_blacklist_timer
   Return Value     : eventid
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint32 delay
   Description      :
*********************************************************************/
eventid supc_start_permanent_blacklist_timer(kal_uint32 delay)
{
    // The unit of delay is in seconds
    eventid evid = NULL;

    if (delay > 0) {
        supc_stop_permanent_blacklist_timer();
        evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                 supc_wpa_permanent_blacklist_timer_expiry_handler,
                                 (void *)EVENT_PARAM_PERM_BLACK_LIST,
                                 (delay * KAL_TICKS_1_SEC)));
        //kal_prompt_trace(MAP_MOD_SUPC, "Start permanent blacklist timer event id: %x, delay: %d sec", evid, delay);
//      kal_trace(DBG_TIMER,INFO_SUPC_START_P_BLACKLIST_EVENT_ID, evid, delay);
    }

    return evid;
}
#endif


/*********************************************************************
   Function Name    : supc_preauth_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void * temp_ptr
   Description      :
*********************************************************************/
void supc_preauth_timer_expiry_handler(void *temp_ptr)
{
    SUPC_ASSERT(EVENT_PARAM_PREAUTH == (int)temp_ptr);

    //wpa_printf(MSG_INFO, "RSN: pre-authentication with " MACSTR
    //" timed out", MAC2STR(wpa_s->preauth_bssid));
//  kal_trace(DBG_TIMER,INFO_SUPC_PRE_AUTH_TIMER_OUT_WITH_BSS, MAC2STR(wpa_s->preauth_bssid));

    wpa_s->preauth_timer_event_id = NULL;

    rsn_preauth_deinit(wpa_s);
    rsn_preauth_candidate_process(wpa_s);
}

/*********************************************************************
   Function Name    : supc_pmksa_timer_expiry_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* para_ptr
   Description      :
*********************************************************************/
void supc_pmksa_timer_expiry_handler(void *para_ptr)
{
    rsn_pmksa_cache *pmksa_cache_ptr = (rsn_pmksa_cache *)para_ptr;
    struct rsn_pmksa_cache *entry, *prev;
    /* Joseph:
     * I'll assume that the expired pmksa_cache is the header of pmksa caches. i believe they
     * originally assume so.
     */

    /* Assert when expiry message losts */
    // SUPC_ASSERT(wpa_s->pmksa == pmksa_cache_ptr);    // Karen: no need

    //wpa_printf(MSG_DEBUG, "RSN: expired PMKSA cache entry for "
    //MACSTR, MAC2STR(pmksa_cache_ptr->aa));
//  kal_trace(DBG_TIMER,INFO_SUPC_PMKSA_CACHE_TIMER_OUT, MAC2STR(pmksa_cache_ptr->aa));

    entry = wpa_s->pmksa;
    prev = NULL;

    while (entry) {
        if (entry == pmksa_cache_ptr) {
            break;
        }
        prev = entry;
        entry = entry->next;
    }

    if (entry != NULL) {
        if (prev != NULL) {
            prev->next = entry->next;
        } else {
            wpa_s->pmksa = entry->next;
        }

        wpa_driver_mtk_remove_pmkid(wpa_s->drv_priv, pmksa_cache_ptr->aa, pmksa_cache_ptr->pmkid);   // Karen
        pmksa_cache_free_entry(pmksa_cache_ptr);
    }

    return;
}



/*********************************************************************
   Function Name    : supc_send_abm_wifi_disconnect_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_abm_wifi_disconnect_cnf()
{
    ilm_struct *ilm_ptr = NULL;

    supc_cb_disconnected();
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_DISCONNECT_CNF;//MSG_ID_SUPC_ABM_WIFI_DISCONNECT_CNF;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_abm_wifi_disconnect_ind
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : supc_abm_cause_enum cause
   Description      :
*********************************************************************/
void supc_send_abm_wifi_disconnect_ind(supc_abm_cause_enum cause)
{
    ilm_struct *ilm_ptr = NULL;
    supc_abm_wifi_disconnect_ind_struct *local_para_ptr = NULL;

    //local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_disconnect_ind_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_disconnect_ind_struct), TD_RESET);
    local_para_ptr->cause = cause;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_DISCONNECT_IND;//MSG_ID_SUPC_ABM_WIFI_DISCONNECT_IND;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

    return;
}



/*********************************************************************
   Function Name    : supc_send_abm_wifi_connect_ind
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_abm_wifi_connect_ind(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;
    supc_abm_wifi_connect_ind_struct *ind = (supc_abm_wifi_connect_ind_struct *)local_para_ptr;
    kal_uint32 i;

    for (i = 0; i < wpa_s->num_scan_results; i++) {
        if (kal_mem_cmp(wpa_s->scan_results[i].bssid, ind->bssid, ETH_ALEN) == 0) {
            break;
        }
    }
    if (i < wpa_s->num_scan_results) {
        ind->rssi = wpa_s->scan_results[i].rssi;
        //kal_prompt_trace(MOD_SUPC,"FIND RSSI in ind:%d",cnf->rssi);
    }
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_CONNECT_IND;//MSG_ID_SUPC_ABM_WIFI_CONNECT_IND;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_abm_wifi_connect_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_abm_wifi_connect_cnf(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;
#if 1
    supc_abm_wifi_connect_cnf_struct *cnf = (supc_abm_wifi_connect_cnf_struct *)local_para_ptr;
#endif
    /*if user cancel WPS connection, while WPS running.
    a connect_cnf will be sent in
    supc_abm_wifi_disconnect_req_handler, so other callers don't
    need to send connect_cnf*/
    INNER_TRACE_STEP(1);
#ifdef __WPS_SUPPORT__
    if (KAL_FALSE == wpa_s->need_abm_conn_cnf) {
        wpa_printf(TRACE_GROUP_1, "discard this conn-confirm\n");
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_DISCARD_CONN_CONFIRM);
        //INNER_TRACE_STEP(2);
        return;
    }

    if ((START_WPS_PBC == wpa_s->connection_type ||
            START_WPS_PIN == wpa_s->connection_type)
            && KAL_FALSE == cnf->result) {
        INNER_TRACE_STEP(4);
        wpas_wps_deinit(wpa_s);
        wpa_printf(TRACE_GROUP_1, "clean wpa context in eapol and eap\n");
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_CLEAN_WPA_CONTEX);
        /*to fix CR 02745873*/
        if (wpa_s->eapol) {
            struct eapol_ctx *ctx = wpa_s->eapol->ctx;
            struct eap_sm *sm = wpa_s->eapol->eap;
            if (ctx) {
                ctx->wps = NULL;
            }
            if (sm) {
                sm->wps = NULL;
            }
        }
    }
#endif
    if (cnf->result) {
        kal_uint32 i;
        for (i = 0; i < wpa_s->num_scan_results; i++) {
            if (kal_mem_cmp(wpa_s->scan_results[i].bssid, cnf->bssid, ETH_ALEN) == 0) {
                break;
            }
        }
        if (i < wpa_s->num_scan_results) {
            cnf->rssi = wpa_s->scan_results[i].rssi;
            wpa_s->connection_rssi = cnf->rssi;
            wpa_s->connection_max_rate = wpa_s->scan_results[i].max_rate;
            wpa_printf(TRACE_GROUP_1, "update connection_rssi = %d, connection_max_rate = %d\n", cnf->rssi,
                       wpa_s->connection_max_rate);
            //kal_prompt_trace(MOD_SUPC,"FIND RSSI:%d",cnf->rssi);
        }
    }

    if (cnf->result) {
        supc_cb_connected();
    } else {
        supc_cb_disconnected();
    }
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_CONNECT_CNF;//MSG_ID_SUPC_ABM_WIFI_CONNECT_CNF;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

    return;
}



/*********************************************************************
   Function Name    : supc_send_wndrv_update_pmkid_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_wndrv_update_pmkid_req(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_PMKID_UPDATE_REQ;//MSG_ID_WNDRV_SUPC_PMKID_UPDATE_REQ;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}



/*********************************************************************
   Function Name    : supc_send_wndrv_data_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Parameters       : peer_buff_struct *peer_buff_ptr
   Description      :
*********************************************************************/
void supc_send_wndrv_data_req(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_DATA_REQ;//MSG_ID_WNDRV_SUPC_DATA_REQ;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = peer_buff_ptr;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}



/*********************************************************************
   Function Name    : supc_send_wndrv_init_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_send_wndrv_init_req(void)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_INIT_REQ;//MSG_ID_WNDRV_SUPC_INIT_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}

/*********************************************************************
   Function Name    : supc_send_wndrv_auth_state_update_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_wndrv_auth_state_update_req(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_AUTH_STATE_UPDATE_REQ;//MSG_ID_WNDRV_SUPC_AUTH_STATE_UPDATE_REQ;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
/*********************************************************************
   Function Name    : supc_send_wndrv_blacklist_update_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_wndrv_blacklist_update_req(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_BLACKLIST_UPDATE_REQ;//MSG_ID_WNDRV_SUPC_BLACKLIST_UPDATE_REQ;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}
#endif

/*********************************************************************
   Function Name    : supc_send_wndrv_deinit_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_send_wndrv_deinit_req(void)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_DEINIT_REQ;//MSG_ID_WNDRV_SUPC_DEINIT_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}




/*********************************************************************
   Function Name    : supc_send_wndrv_add_key_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_wndrv_add_key_req(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_ADD_KEY_REQ;//MSG_ID_WNDRV_SUPC_ADD_KEY_REQ;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_mmi_wifi_curr_ap_info_ind
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
#if 0
void supc_send_mmi_wifi_curr_ap_info_ind(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;
    kal_int32 pending_msgs = 0;
    /*
        if (kal_get_queue_info(task_info_g[mod_task_g[MAP_MOD_MMI]].task_ext_qid, &msgq_info) == KAL_TRUE)
        {
            pending_msgs = msgq_info.pending_msgs;
        }
    */
    pending_msgs = msg_get_task_extq_messages(kal_map_module_to_task_index(MAP_MOD_MMI));
    if (pending_msgs != 0 || pending_msgs == -1) {
        //free_local_para(local_para_ptr);
        free_local_para(local_para_ptr);
        kal_wap_trace(MAP_MOD_SUPC, TRACE_GROUP_5, "Skip MSG_ID_SUPC_MMI_WIFI_CURR_AP_INFO_IND msg, MMI pending_msgs: %d", pending_msgs);
    } else {
        ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

        ilm_ptr->msg_id = MAP_MSG_ID_SUPC_MMI_WIFI_CURR_AP_INFO_IND;//MSG_ID_SUPC_MMI_WIFI_CURR_AP_INFO_IND;
        ilm_ptr->local_para_ptr = local_para_ptr;
        ilm_ptr->peer_buff_ptr = NULL;

        SEND_ILM(MAP_MOD_SUPC, MAP_MOD_MMI, MAP_SUPC_MMI_SAP, ilm_ptr);
    }
    return;
}
#endif
#ifdef __WAPI_SUPPORT__
/*********************************************************************
   Function Name    : supc_send_wndrv_query_cap
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
void supc_send_wndrv_query_cap(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_QUERY_CAPABILITY_REQ;//MSG_ID_WNDRV_SUPC_QUERY_CAPABILITY_REQ;

    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}
#endif

/*********************************************************************
   Function Name    : supc_filter_duplicated_ap
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint8 total_num_of_bss
   Parameters       : wndrv_supc_bss_info_struct *bss_info
   Parameters       : kal_uint8 *num_of_diff_ap
   Parameters       : wndrv_supc_bss_info_struct *filter_result
   Parameters       : kal_uint8 *ap_num
   Description
*********************************************************************/
void supc_filter_duplicated_ap(kal_uint8 total_num_of_bss, wndrv_supc_bss_info_struct *bss_info,
                               kal_uint8 *num_of_diff_ap, wndrv_supc_bss_info_struct *filter_result, kal_uint8 *ap_num)
{
    kal_uint32 i, j;
    kal_bool is_duplicated;
    struct wpa_ie_data ie1, ie2;

    *num_of_diff_ap = 0;
    for (i = 0; i < total_num_of_bss; i++) {
        is_duplicated = KAL_FALSE;
        //wpa_hexdump(MSG_DEBUG, "fileter_duplicate row AP",(char*)bss_info[i].ssid, bss_info[i].ssid_len);
//      kal_trace(DBG_SCAN,INFO_SUPC_SCAN_FITLER_AP);
//      kal_wap_trace(MAP_MOD_SUPC,DBG_SCAN,"%s,%d",bss_info[i].ssid,bss_info[i].ssid_len);

        for (j = 0; j < *num_of_diff_ap; j++) {
            if (bss_info[i].ssid_len != filter_result[j].ssid_len) {
                continue;
            }

            if (kal_mem_cmp(bss_info[i].ssid, filter_result[j].ssid, bss_info[i].ssid_len) != 0) {
                continue;
            }

            if (bss_info[i].network_type != filter_result[j].network_type) {
                continue;
            }

            /*[MAUI_01698468] mtk01612*/
#if 0
            if (bss_info[i].wpa_ie_len != filter_result[j].wpa_ie_len) {
                continue;
            }

            if (bss_info[i].rsn_ie_len != filter_result[j].rsn_ie_len) {
                continue;
            }
#endif
            if (bss_info[i].privacy != filter_result[j].privacy) {
                continue;
            }

            if (bss_info[i].privacy == 1) {
                if ((bss_info[i].wpa_ie_len == 0) &&
#ifdef __WAPI_SUPPORT__
                        (bss_info[i].wapi_ie_len == 0) &&
#endif
                        (bss_info[i].rsn_ie_len == 0)) {
                    if ((filter_result[j].wpa_ie_len != 0) ||
#ifdef __WAPI_SUPPORT__
                            (filter_result[j].wapi_ie_len != 0) ||
#endif
                            (filter_result[j].rsn_ie_len != 0)) {
                        continue;
                    }
                }
            }

#ifdef __WAPI_SUPPORT__
            if ((bss_info[i].wapi_ie_len != 0) &&
                    (wpa_parse_wapi_ie(wpa_s, bss_info[i].wapi_ie, bss_info[i].wapi_ie_len, &ie1) == 0) &&
                    (wpa_parse_wapi_ie(wpa_s, filter_result[j].wapi_ie, filter_result[j].wapi_ie_len, &ie2) == 0)) {
                if (ie1.pairwise_cipher != ie2.pairwise_cipher) {
                    continue;
                }
                if (ie1.key_mgmt != ie2.key_mgmt) {
                    continue;
                }
            }

#endif

            if ((bss_info[i].wpa_ie_len != 0) &&
//              (bss_info[i].wpa_ie_len == filter_result[j].wpa_ie_len) && /*[MAUI_01698468] mtk01612*/
                    (wpa_parse_wpa_ie(wpa_s, bss_info[i].wpa_ie, bss_info[i].wpa_ie_len, &ie1) == 0) &&
                    (wpa_parse_wpa_ie(wpa_s, filter_result[j].wpa_ie, filter_result[j].wpa_ie_len, &ie2) == 0)) {
                if (ie1.pairwise_cipher != ie2.pairwise_cipher) {
                    continue;
                }
                if (ie1.key_mgmt != ie2.key_mgmt) {
                    continue;
                }
            }

            if ((bss_info[i].rsn_ie_len != 0) &&
//              (bss_info[i].rsn_ie_len == filter_result[j].rsn_ie_len) && /*[MAUI_01698468] mtk01612*/
                    (wpa_parse_wpa_ie(wpa_s, bss_info[i].rsn_ie, bss_info[i].rsn_ie_len, &ie1) == 0) &&
                    (wpa_parse_wpa_ie(wpa_s, filter_result[j].rsn_ie, filter_result[j].rsn_ie_len, &ie2) == 0)) {
                if (ie1.pairwise_cipher != ie2.pairwise_cipher) {
                    continue;
                }
                if (ie1.key_mgmt != ie2.key_mgmt) {
                    continue;
                }
            }

            is_duplicated = KAL_TRUE;
            if (filter_result[j].rssi < bss_info[i].rssi) {
                filter_result[j].rssi = bss_info[i].rssi;
            }

            ap_num[j]++; /*Kinki: number of APs in a SSID*/

            break;
        }

        if (is_duplicated == KAL_FALSE) {
            kal_mem_cpy(&filter_result[*num_of_diff_ap], &bss_info[i], sizeof(wndrv_supc_bss_info_struct));
            ap_num[*num_of_diff_ap] = 1; /*Kinki: number of APs in a SSID*/
            *num_of_diff_ap += 1;
        }
    }
}
/*********************************************************************
   Function Name    : supc_send_abm_wifi_ap_list_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_abm_connection_info_cnf()
{
    ilm_struct *ilm_ptr = NULL;
    supc_abm_wifi_get_connection_info_cnf_struct *local_para_ptr = NULL;
    local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_get_connection_info_cnf_struct), TD_RESET);
    local_para_ptr->ssid_len = wpa_s->ssid_len;
    kal_mem_cpy(local_para_ptr->ssid, wpa_s->ssid, wpa_s->ssid_len);
    local_para_ptr->phy_rate = wpa_s->connection_max_rate;
    local_para_ptr->channel = wpa_s->channel_number;
    local_para_ptr->rssi = wpa_s->connection_rssi;
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_CNF;//MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

}
/*********************************************************************
   Function Name    : supc_send_abm_wifi_ap_list_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_abm_wps_get_credential_cnf()
{
    ilm_struct *ilm_ptr = NULL;
    struct wpa_ssid *ssid;
    supc_abm_wifi_wps_credential_struct *local_para_ptr = NULL;
    local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_wps_credential_struct), TD_RESET);
    ssid = wpa_s->conf->ssid;
    while (ssid) {
        if (ssid->key_mgmt | WPA_KEY_MGMT_WPS) {
            break;
        }
    }
    if (ssid) {
        local_para_ptr->ssid_len = ssid->ssid_len;
        kal_mem_cpy(local_para_ptr->ssid, ssid->ssid, ssid->ssid_len);
        kal_mem_cpy(local_para_ptr->mac_addr, ssid->bssid, 6);
        /*
        ulong AuthType;         // mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa2-psk
        ulong EncrType;         // mandatory, 1: none, 2: wep, 4: tkip, 8: aes
        */
        if (ssid->auth_alg == WPA_AUTH_ALG_OPEN && ssid->key_mgmt == WPA_KEY_MGMT_NONE) {   //open
            local_para_ptr->auth_type = 1;  //open
            local_para_ptr->encr_type = 1;  //none
        } else if (ssid->auth_alg == WPA_AUTH_ALG_OPEN && ssid->key_mgmt == WPA_KEY_MGMT_PSK) { //psk
            if (ssid->proto == WPA_PROTO_RSN) {    //wpa2-psk
                local_para_ptr->auth_type = 0x20;
            } else if (ssid->proto == WPA_PROTO_WPA) { //wpa-psk
                local_para_ptr->auth_type = 2;
            }

            if (ssid->pairwise_cipher == WPA_CIPHER_TKIP) {
                local_para_ptr->encr_type = 4;
            } else if (ssid->pairwise_cipher == WPA_CIPHER_CCMP) {
                local_para_ptr->encr_type = 8;
            }

            local_para_ptr->key_len = strlen(ssid->passphrase);
            kal_mem_cpy(local_para_ptr->key, ssid->passphrase, strlen(ssid->passphrase));
        }

        if (ssid->wep_key_len[ssid->wep_tx_keyidx] != 0) { //wep
            local_para_ptr->auth_type = 1;    //open or shared????
            local_para_ptr->encr_type = 2;
            local_para_ptr->key_idx = ssid->wep_tx_keyidx;
            local_para_ptr->key_len = ssid->wep_key_len[ssid->wep_tx_keyidx];
            kal_mem_cpy(local_para_ptr->key, ssid->wep_key[ssid->wep_tx_keyidx]
                        , ssid->wep_key_len[ssid->wep_tx_keyidx]);
        }

    }
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MSG_ID_SUPC_ABM_WIFI_WPS_GET_CREDENTIAL_CNF;//MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

}

/*********************************************************************
   Function Name    : supc_send_abm_wifi_ap_list_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : kal_uint8 num_of_bss
   Parameters       : wndrv_supc_bss_info_struct *bss_info
   Parameters       : kal_uint8 *ap_num
   Description      :
*********************************************************************/
void supc_send_abm_wifi_ap_list_cnf(kal_uint8 total_num_of_bss,
                                    wndrv_supc_bss_info_struct *bss_info, kal_uint8 *ap_num)
{
    /* Joseph:
     * Copy the information from wndrv to mmi
     */
    ilm_struct *ilm_ptr = NULL;
    kal_uint32 i, index;
    supc_abm_wifi_ap_list_cnf_struct *local_para_ptr = NULL;
    struct wpa_ie_data ie;
    kal_uint8   num_of_bss = total_num_of_bss;

    index = 0;

    while (num_of_bss > WNDRV_SCAN_RESULTS_NUM) {
        //local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_ap_list_cnf_struct), TD_UL);
        local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_ap_list_cnf_struct), TD_RESET);
        local_para_ptr->more_flag = TRUE;
        local_para_ptr->num_of_bss = WNDRV_SCAN_RESULTS_NUM;
        for (i = 0; i < WNDRV_SCAN_RESULTS_NUM; i++, index++) {
            kal_mem_cpy(local_para_ptr->bss_info[i].bssid, bss_info[index].bssid, WNDRV_MAC_ADDRESS_LEN);
            local_para_ptr->bss_info[i].privacy = bss_info[index].privacy;
            local_para_ptr->bss_info[i].ssid_len = bss_info[index].ssid_len;
            kal_mem_cpy(local_para_ptr->bss_info[i].ssid, bss_info[index].ssid, WNDRV_SSID_MAX_LEN);
            local_para_ptr->bss_info[i].rssi = bss_info[index].rssi;
            local_para_ptr->bss_info[i].channel_number = bss_info[index].channel_number;
            local_para_ptr->bss_info[i].network_type = bss_info[index].network_type;
            local_para_ptr->bss_info[i].max_rate = bss_info[index].max_rate;
            local_para_ptr->bss_info[i].num_of_ap = ap_num[index]; /*Kinki: number of APs in a SSID*/



            local_para_ptr->bss_info[i].wpa_ie_info_p = bss_info[index].wpa_ie_len != 0 ? KAL_TRUE : KAL_FALSE;
            if ((local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) &&
                    (wpa_parse_wpa_ie(wpa_s, bss_info[index].wpa_ie, bss_info[index].wpa_ie_len, &ie) == 0)) {
                local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher = ie.pairwise_cipher;
                local_para_ptr->bss_info[i].wpa_ie_info.group_cipher = ie.group_cipher;
                local_para_ptr->bss_info[i].wpa_ie_info.key_mgmt = ie.key_mgmt;
            }

            local_para_ptr->bss_info[i].rsn_ie_info_p = bss_info[index].rsn_ie_len != 0 ? KAL_TRUE : KAL_FALSE;
            if ((local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE) &&
                    (wpa_parse_wpa_ie(wpa_s, bss_info[index].rsn_ie, bss_info[index].rsn_ie_len, &ie) == 0)) {
                local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher = ie.pairwise_cipher;
                local_para_ptr->bss_info[i].rsn_ie_info.group_cipher = ie.group_cipher;
                local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt = ie.key_mgmt;
            }

#ifdef __WAPI_SUPPORT__
            local_para_ptr->bss_info[i].wapi_ie_info_p = bss_info[index].wapi_ie_len != 0 ? KAL_TRUE : KAL_FALSE;
            if ((local_para_ptr->bss_info[i].wapi_ie_info_p == KAL_TRUE) &&
                    (wpa_parse_wapi_ie(wpa_s, bss_info[index].wapi_ie, bss_info[index].wapi_ie_len, &ie) == 0)) {
                local_para_ptr->bss_info[i].wapi_ie_info.pairwise_cipher = ie.pairwise_cipher;
                local_para_ptr->bss_info[i].wapi_ie_info.group_cipher = ie.group_cipher;
                local_para_ptr->bss_info[i].wapi_ie_info.key_mgmt = ie.key_mgmt;
//              kal_trace(DBG_WAPI,INFO_SUPC_SCAN_WAPI_AP_IND);
                /*
                wpa_printf(MSG_STATE,"WAPI ssid pairwise %d group %d key %d\n",
                    (int)local_para_ptr->bss_info[i].wapi_ie_info.pairwise_cipher,
                    (int)local_para_ptr->bss_info[i].wapi_ie_info.group_cipher,
                    (int)local_para_ptr->bss_info[i].wapi_ie_info.key_mgmt);
                */
            }
#endif

        }
        num_of_bss -= WNDRV_SCAN_RESULTS_NUM;

        // send the ILM
        ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

        ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;//MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;
        ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
        ilm_ptr->peer_buff_ptr = NULL;
        /*      wpa_printf(TRACE_GROUP_1, "MAP_MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF ssid=%c%c%c%c, more_flag=%d\n", local_para_ptr->bss_info[0].ssid[0]
                    , local_para_ptr->bss_info[0].ssid[1], local_para_ptr->bss_info[0].ssid[2], local_para_ptr->bss_info[0].ssid[3]
                    , local_para_ptr->more_flag);
                    */
        SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);
    }
    //local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_ap_list_cnf_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_ap_list_cnf_struct), TD_UL);
    local_para_ptr->num_of_bss = num_of_bss;
    local_para_ptr->more_flag = FALSE;
    // kal_mem_cpy(local_para_ptr->bss_info, bss_info, sizeof(wndrv_supc_bss_info_struct)* num_of_bss);
    for (i = 0; i < num_of_bss; i++, index++) {
        kal_mem_cpy(local_para_ptr->bss_info[i].bssid, bss_info[index].bssid, WNDRV_MAC_ADDRESS_LEN);
        local_para_ptr->bss_info[i].privacy = bss_info[index].privacy;
        local_para_ptr->bss_info[i].ssid_len = bss_info[index].ssid_len;
        kal_mem_cpy(local_para_ptr->bss_info[i].ssid, bss_info[index].ssid, WNDRV_SSID_MAX_LEN);
        local_para_ptr->bss_info[i].rssi = bss_info[index].rssi;
        local_para_ptr->bss_info[i].channel_number = bss_info[index].channel_number;
        local_para_ptr->bss_info[i].network_type = bss_info[index].network_type;
        local_para_ptr->bss_info[i].max_rate = bss_info[index].max_rate;
        local_para_ptr->bss_info[i].num_of_ap = ap_num[index]; /*Kinki: number of APs in a SSID*/

        local_para_ptr->bss_info[i].wpa_ie_info_p = bss_info[index].wpa_ie_len != 0 ? KAL_TRUE : KAL_FALSE;
        if ((local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) &&
                (wpa_parse_wpa_ie(wpa_s, bss_info[index].wpa_ie, bss_info[index].wpa_ie_len, &ie) == 0)) {
            local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher = ie.pairwise_cipher;
            local_para_ptr->bss_info[i].wpa_ie_info.group_cipher = ie.group_cipher;
            local_para_ptr->bss_info[i].wpa_ie_info.key_mgmt = ie.key_mgmt;
        }

        local_para_ptr->bss_info[i].rsn_ie_info_p = bss_info[index].rsn_ie_len != 0 ? KAL_TRUE : KAL_FALSE;
        if ((local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE) &&
                (wpa_parse_wpa_ie(wpa_s, bss_info[index].rsn_ie, bss_info[index].rsn_ie_len, &ie) == 0)) {
            local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher = ie.pairwise_cipher;
            local_para_ptr->bss_info[i].rsn_ie_info.group_cipher = ie.group_cipher;
            local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt = ie.key_mgmt;
        }

#ifdef __WAPI_SUPPORT__
        local_para_ptr->bss_info[i].wapi_ie_info_p = bss_info[index].wapi_ie_len != 0 ? KAL_TRUE : KAL_FALSE;

        if (local_para_ptr->bss_info[i].wapi_ie_info_p == KAL_TRUE) {
            wpa_printf(TRACE_GROUP_1, "supc_send_abm_wifi_ap_list_cnf WAPI ssid %s\n", local_para_ptr->bss_info[i].ssid);
//          kal_trace(DBG_WAPI,INFO_SUPC_SCAN_WAPI_AP_IND);
        }

        if ((local_para_ptr->bss_info[i].wapi_ie_info_p == KAL_TRUE) &&
                (wpa_parse_wapi_ie(wpa_s, bss_info[index].wapi_ie, bss_info[index].wapi_ie_len, &ie) == 0)) {
            local_para_ptr->bss_info[i].wapi_ie_info.pairwise_cipher = ie.pairwise_cipher;
            local_para_ptr->bss_info[i].wapi_ie_info.group_cipher = ie.group_cipher;
            local_para_ptr->bss_info[i].wapi_ie_info.key_mgmt = ie.key_mgmt;
            /*    no need because msg to ABM will include it
                wpa_printf(MSG_DEBUG,"WAPI ssid pairwise %d group %d key %d\n",
                    (int)local_para_ptr->bss_info[i].wapi_ie_info.pairwise_cipher,
                    (int)local_para_ptr->bss_info[i].wapi_ie_info.group_cipher,
                    (int)local_para_ptr->bss_info[i].wapi_ie_info.key_mgmt);
                    */
        }
#endif
    }
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;//MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;


    /*  wpa_printf(TRACE_GROUP_1, "MAP_MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF ssid=%c%c%c%c, more_flag=%d\n", local_para_ptr->bss_info[0].ssid[0]
            , local_para_ptr->bss_info[0].ssid[1], local_para_ptr->bss_info[0].ssid[2], local_para_ptr->bss_info[0].ssid[3]
            , local_para_ptr->more_flag);
            */
    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_abm_wifi_init_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_abm_wifi_init_cnf()
{
    ilm_struct *ilm_ptr = NULL;

    supc_abm_wifi_init_cnf_struct *local_para_ptr = NULL;

    //local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_init_cnf_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_init_cnf_struct), TD_RESET);

    kal_mem_cpy(local_para_ptr->mac_addr, wpa_s->own_addr, ETH_ALEN);

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_INIT_CNF;//MSG_ID_SUPC_ABM_WIFI_INIT_CNF;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_MMI_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_abm_wifi_deinit_cnf
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_abm_wifi_deinit_cnf()
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_DEINIT_CNF;//MSG_ID_SUPC_ABM_WIFI_DEINIT_CNF;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_MMI_SAP, ilm_ptr);

    return;
}

#if 0
void supc_sim_read_cnf_handler(ilm_struct *ilm_ptr)
{
    sim_read_cnf_struct *cnf;

    cnf = (sim_read_cnf_struct *)ilm_ptr->local_para_ptr;
    wpa_s->imsi_mnc_digits = (cnf->data[0]) & 0xF;
    if ((wpa_s->imsi_mnc_digits != 2) && (wpa_s->imsi_mnc_digits != 3)) {
        wpa_s->imsi_mnc_digits = 2;
    }

    //kal_prompt_trace(MOD_SUPC,"eap_sim-mnc=%d",wpa_s->imsi_mnc_digits);

    return;


}

/*********************************************************************
   Function Name    : supc_send_sim_read_imsi_mnc_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_sim_read_imsi_mnc_req(void)
{
    ilm_struct *ilm_ptr = NULL;
    sim_read_req_struct *local_para_ptr = NULL;


    /* Joseph:
     * Send sim the read request to read imsi
     * TBD
     */

    /*
    local_para_ptr = (sim_read_req_struct *)
          construct_local_para(
                 (kal_uint16)sizeof(sim_read_req_struct),
                     TD_CTRL);
    */
    local_para_ptr = (sim_read_req_struct *)
                     construct_local_para(
                         (kal_uint16)sizeof(sim_read_req_struct),
                         TD_CTRL);
    local_para_ptr->file_idx = FILE_AD_IDX;
    local_para_ptr->para = 3;
    local_para_ptr->length = 1;
    local_para_ptr->access_id = 0;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);
    ilm_ptr->msg_id = MAP_MSG_ID_SIM_READ_REQ;//MSG_ID_SIM_READ_REQ;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_SIM, MAP_PS_SIM_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_sim_read_imsi_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_sim_read_imsi_req()
{
    ilm_struct *ilm_ptr = NULL;
    sim_read_req_struct *local_para_ptr = NULL;


    /* Joseph:
     * Send sim the read request to read imsi
     * TBD
     */

    /*
    local_para_ptr = (sim_read_req_struct *)
          construct_local_para(
                 (kal_uint16)sizeof(sim_read_req_struct),
                     TD_CTRL);
    */
    local_para_ptr = (sim_read_req_struct *)
                     construct_local_para(
                         (kal_uint16)sizeof(sim_read_req_struct),
                         TD_CTRL);
    local_para_ptr->file_idx = FILE_IMSI_IDX;
    local_para_ptr->para = 0;
    local_para_ptr->length = 9;
    local_para_ptr->access_id = 0;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);
    ilm_ptr->msg_id = MAP_MSG_ID_SIM_READ_REQ;//MSG_ID_SIM_READ_REQ;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_SIM, MAP_PS_SIM_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_wndrv_join_network_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : local_para_struct *local_para_ptr
   Description      :
*********************************************************************/
#endif
void supc_send_wndrv_join_network_req(local_para_struct *local_para_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);
    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_JOIN_NETWORK_REQ;//MSG_ID_WNDRV_SUPC_JOIN_NETWORK_REQ;
    ilm_ptr->local_para_ptr = local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}


/*********************************************************************
   Function Name    : supc_send_wndrv_disjoin_network_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_wndrv_disjoin_network_req()
{
    ilm_struct *ilm_ptr = NULL;

    supc_cb_disconnected();
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_DISJOIN_NETWORK_REQ;//MSG_ID_WNDRV_SUPC_DISJOIN_NETWORK_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}

#ifdef __WPS_SUPPORT__
void supc_send_wndrv_wps_sit_survey_req(struct wps_context *wps_ctx, kal_bool ispbc,
                                        enum wps_request_type req_type) {
    ilm_struct *ilm_ptr = NULL;
    wndrv_supc_site_survey_req_struct *req_local_para_ptr = NULL;
    wndrv_supc_wps_probe_ie *pie = NULL;

    /* MAUI_02639824 note: avoid twice site survey req with ind
    */
    if (fgSiteSurveyReq == KAL_FALSE)
    {
        fgSiteSurveyReq = KAL_TRUE;

        req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_site_survey_req_struct), TD_RESET);

        kal_mem_set(req_local_para_ptr->ssid, 0,  WNDRV_SSID_MAX_LEN);
        req_local_para_ptr->ssid_len = 0;
        req_local_para_ptr->has_wps_ie = KAL_TRUE;

        /*build vendor IE*/
        pie = &(req_local_para_ptr->vendor_ie);
        pie->wps_ver = WPS_VERSION_V1;
        pie->req_type = req_type;
        /*
        if (ispbc)
            pie->cfg_method = WPS_CONFIG_PUSHBUTTON;
        else
            pie->cfg_method = WPS_CONFIG_LABEL | WPS_CONFIG_DISPLAY | WPS_CONFIG_KEYPAD;
        */
        pie->cfg_method = WPS_CONFIG_V_PUSHBUTTON | WPS_CONFIG_V_DISP_PIN;

        kal_mem_cpy(pie->uuid, wps_ctx->uuid, 16);

        pie->categ = wps_ctx->dev.categ;
        //WPA_PUT_BE32((kal_uint8*)&pie->oui, wps_ctx->dev.oui);
        pie->oui = wps_ctx->dev.oui;
        pie->sub_cate = wps_ctx->dev.sub_categ;

        pie->cfg_err = WPS_CFG_NO_ERROR;
        pie->rf_bands = wps_ctx->dev.rf_bands;
        pie->assoc_stat = WPS_ASSOC_NOT_ASSOC;
        pie->dev_pswd_id = (ispbc ? DEV_PW_PUSHBUTTON : DEV_PW_DEFAULT);

        /*add for wps 2.0*/
        kal_mem_cpy(pie->manufact, WPS_MANUFACTURER, sizeof(WPS_MANUFACTURER));
        pie->manuf_len = sizeof(WPS_MANUFACTURER) - 1;
        kal_mem_cpy(pie->model_name, WPS_MODEL_NAME, sizeof(WPS_MODEL_NAME));
        pie->model_name_len = sizeof(WPS_MODEL_NAME) - 1;
        kal_mem_cpy(pie->model_num, WPS_MODEL_NUM, sizeof(WPS_MODEL_NUM));
        pie->model_num_len = sizeof(WPS_MODEL_NUM) - 1;
        kal_mem_cpy(pie->dev_name, WPS_DEVICE_NAME, sizeof(WPS_DEVICE_NAME));
        pie->dev_name_len = sizeof(WPS_DEVICE_NAME) - 1;

        pie->version2 = WPS_VERSION_V2;

        ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

        ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ;//MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ;
        ilm_ptr->local_para_ptr = (local_para_struct *)req_local_para_ptr;
        ilm_ptr->peer_buff_ptr = NULL;

        SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);
    } else
    {
//        kal_wap_trace(MAP_MOD_SUPC, TRACE_WARNING,
//            "previous site survey req not indicated already");
    }

    return;
}
#endif

/*********************************************************************
   Function Name    : supc_send_wndrv_site_survey_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_wndrv_site_survey_req(kal_uint8 *ssid, kal_uint16 ssid_len)
{
    ilm_struct *ilm_ptr = NULL;
    wndrv_supc_site_survey_req_struct *local_para_ptr = NULL;

    /* MAUI_02639824 note: avoid twice site survey req with ind
    */
    if (fgSiteSurveyReq == KAL_FALSE) {
        fgSiteSurveyReq = KAL_TRUE;
        wpa_s->num_scan_results = 0;
        //local_para_ptr = construct_local_para(sizeof(wndrv_supc_site_survey_req_struct), TD_UL);
        local_para_ptr = construct_local_para(sizeof(wndrv_supc_site_survey_req_struct), TD_RESET);
        /*has_wps_ie is always contained in site_survey_struct,
        so must set wps relative members*/
        local_para_ptr->ssid_len = 0;
        local_para_ptr->has_wps_ie = KAL_FALSE;
        kal_mem_set(&(local_para_ptr->vendor_ie), 0, sizeof(wndrv_supc_wps_probe_ie));

        kal_mem_set(local_para_ptr->ssid, 0,  WNDRV_SSID_MAX_LEN);
        kal_mem_cpy(local_para_ptr->ssid, ssid, ssid_len);
        local_para_ptr->ssid_len = ssid_len;

        ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

        ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ;//MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ;
        ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
        ilm_ptr->peer_buff_ptr = NULL;
        //wpa_hexdump(MSG_INFO, "to scan ssid:", (char*)ssid, ssid_len);
        if (ssid_len != 0) {
//          kal_wap_trace(MAP_MOD_SUPC,DBG_SCAN,"to scan ssid:%s,len:%d",ssid, ssid_len);
        }
        SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);
    } else {
        wpa_printf(MSG_INFO, "previous site survey req not indicated already");
//      kal_trace(DBG_SCAN,INFO_SUPC_SITE_SURVEY_NOT_IND);
    }

    return;
}

#ifdef __WIFI_CHIP_DYM_POWERON__
//Kinki add *start*
/*********************************************************************
   Function Name    : supc_send_wndrv_chip_poweron_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_wndrv_chip_poweron_req(void)
{
    ilm_struct *ilm_ptr = NULL;
    /* MAUI_02642354 [WIFI UE]It can not enter WLAN from main mune in this case. */
    fgSiteSurveyReq = KAL_FALSE;
    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_CHIP_POWERON_REQ;//MSG_ID_WNDRV_SUPC_CHIP_POWERON_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}

/*********************************************************************
   Function Name    : supc_send_wndrv_chip_poweroff_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Description      :
*********************************************************************/
void supc_send_wndrv_chip_poweroff_req(void)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_CHIP_POWEROFF_REQ;//MSG_ID_WNDRV_SUPC_CHIP_POWEROFF_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    return;
}
//Kinki add *end*
#endif

void supc_do_deinit_req(void)
{
    u8 *addr = NULL;
    wpa_ssid *pnext;

    if (wpa_s->wpa_state >= WPA_SCANNING) {
        supc_do_disconnect_req();    // Kinki add for disconnect req
    }

#ifdef __WIFI_CHIP_DYM_POWERON__
    SUPC_ASSERT(wpa_s->chip_state == CHIP_POWER_OFF);
#endif

    //if(WPA_DISCONNECTED != wpa_s->wpa_state)
    //{
    wpa_s->wpa_state = WPA_DEINITED; //Kinki add
    if (kal_mem_cmp(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) != 0) {
        kal_mem_cpy(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN);
        //supc_send_wndrv_disjoin_network_req(); // Kinki add for power-saving
        //wpa_driver_mtk_disassociate(wpa_s, wpa_s->bssid, REASON_DEAUTH_LEAVING);
        addr = wpa_s->bssid;
    }
    wpa_clear_keys(wpa_s, addr);

    //eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
    //eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    //eapol_sm_notify_portValid(wpa_s->eapol, FALSE);

    //eapol_sm_deinit(wpa_s->eapol);
    //wpa_s->eapol = NULL;

    //eapol_sm_deinit(wpa_s->preauth_eapol);
    // wpa_s->preauth_eapol = NULL;

    rsn_preauth_deinit(wpa_s);
    pmksa_candidate_free(wpa_s);
    pmksa_cache_free(wpa_s);
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_auth_timeout_list_clear(wpa_s);
    wpa_blacklist_clear_temporal(wpa_s);
    wpa_blacklist_clear_permanent(wpa_s);
    supc_blacklist_update(wpa_s);
#else
    wpa_blacklist_clear(wpa_s);
#endif

#ifdef __WAPI_SUPPORT__
    wapi_deinit(wpa_s);
#endif

    SUPC_DEALLOC_BUFF(wpa_s->l2);
    wpa_s->l2 = NULL;

    SUPC_DEALLOC_BUFF(wpa_s->l2_preauth);
    wpa_s->l2_preauth = NULL;

#ifdef __WAPI_SUPPORT__
    SUPC_DEALLOC_BUFF(wpa_s->l2_wapi);
    wpa_s->l2_wapi = NULL;
#endif

    SUPC_DEALLOC_BUFF(wpa_s->drv_priv);
    wpa_s->drv_priv = NULL;

    if (wpa_s->conf != NULL) {
        /* J: free up memory inside wpa_conf */
        /* J: Remember to set the last ssid->next to NULL */
        while (wpa_s->conf->ssid != NULL) {
            supc_wpa_free_ssid(wpa_s->conf->ssid);
            pnext = wpa_s->conf->ssid->next;
            SUPC_DEALLOC_BUFF(wpa_s->conf->ssid);
            wpa_s->conf->ssid = pnext;
        }
        SUPC_DEALLOC_BUFF(wpa_s->conf);
        wpa_s->conf = NULL;
    }

    wpa_s->current_ssid = NULL;

    supc_send_wndrv_deinit_req();



}


/*********************************************************************
   Function Name    : supc_do_disconnect_req
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_do_disconnect_req(void)
{
    u8 *addr = NULL; //Kinki add for doing things in disconnect_req *start*
    wpa_ssid *pnext;
    wpa_printf(TRACE_GROUP_1, "%s\n", __func__);
    wpa_s->num_of_profile = 0;
    wpa_s->wpa_state = WPA_DISCONNECTED;

    if (supc_is_logoff_needed()) {
        if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X ||
                wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
            eapol_sm_notify_logoff(wpa_s->eapol, TRUE);
            kal_sleep_task(5);  // need to delay for a short period of time so that driver can send out the logoff frame.
        }
    }

    addr = wpa_s->bssid;
    wpa_clear_keys(wpa_s, addr);
    kal_mem_cpy(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN);
#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state == CHIP_POWER_ON) { //Kinki add for power-saving
        supc_send_wndrv_disjoin_network_req();

        wpa_s->chip_state = CHIP_POWER_OFF;
        supc_send_wndrv_chip_poweroff_req();
    }
#else
    supc_send_wndrv_disjoin_network_req();
#endif

    if (wpa_s->conf != NULL) {
        while (wpa_s->conf->ssid != NULL) {
            supc_wpa_free_ssid(wpa_s->conf->ssid);
            pnext = wpa_s->conf->ssid->next;
            SUPC_DEALLOC_BUFF(wpa_s->conf->ssid);
            wpa_s->conf->ssid = pnext;
        }
        SUPC_DEALLOC_BUFF(wpa_s->conf);
        wpa_s->conf = NULL;
    }

    SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
    wpa_s->ap_wpa_ie = NULL;
    SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
    wpa_s->ap_rsn_ie = NULL;
#ifdef __WAPI_SUPPORT__
    if (wpa_s->ap_wapi_ie) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wapi_ie);
        wpa_s->ap_wapi_ie = NULL;
    }
#endif
    if (wpa_s->assoc_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
        wpa_s->assoc_wpa_ie = NULL;
        wpa_s->assoc_wpa_ie_len = 0;
    }
    supc_stop_auth_timer();
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_listed_clear_success(wpa_s); //Kinki add for MAUI_00401655
    wpa_auth_timeout_list_clear(wpa_s);
    wpa_blacklist_clear_temporal(wpa_s);
    wpa_blacklist_clear_permanent(wpa_s);
    supc_blacklist_update(wpa_s);
#else
    wpa_blacklist_clear(wpa_s);
#endif
    wpa_supplicant_cancel_scan();
    //wpa_supplicant_cancel_auth_timeout(wpa_s);
    supc_stop_report_mmi_timer();
    supc_stop_handover_timer();
#ifdef __WAPI_SUPPORT__
    supc_stop_wapi_auth_timer();
#endif
    wpa_s->current_ssid = NULL;

    eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);

    eapol_sm_deinit(wpa_s->eapol);
    wpa_s->eapol = NULL;
    eapol_sm_deinit(wpa_s->preauth_eapol);
    wpa_s->preauth_eapol = NULL;
#ifndef  WIFI_AUTH_PSK_ONLY
#ifdef __CERTMAN_SUPPORT__
    /*MAUI_03055418 preauth eapol & eapol use the same ssl context*/
    if (wpa_s->support_pre_auth) {
        if (wpa_s->g_ssl_ctx) {
            sec_ssl_ctx_free((ssl_ctx *)wpa_s->g_ssl_ctx);
            wpa_s->g_ssl_ctx = NULL;
        }
    }
#endif
#endif
    wpa_s->num_scan_results = 0;

}//Kinki add for doing things in disconnect_req *end*

#if 0//#ifdef __CERTMAN_SUPPORT__
//Kinki add
/*********************************************************************
   Function Name    : supc_set_private_key_cb
   Return Value     :
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* file
   Parameters       : file_info* temp
   Description      :
*********************************************************************/
void supc_set_private_key_cb(kal_uint32 trans_id, kal_uint32 cert_id,
                             const kal_uint8 *pwd, kal_uint32 size)
{
    /*Local Variable*/
    struct wpa_ssid *ssid = wpa_s->selected_ssid;

    /*Code Body*/
    if (ssid == NULL) {
        return;
    }
    //wpa_printf(MSG_DEBUG, "trans_id = %d, cert_id = %d (in private_key_cb)", trans_id, cert_id);
//  kal_trace(DBG_CONN,INFO_SUPC_CERTID_IN_PRIKEY_CB, trans_id, cert_id);

    if ((trans_id == wpa_s->current_phase1_trans_id) && (ssid->client_cert_id == cert_id)) {
        if (ssid->privkey_random_passwd != NULL) {
            SUPC_DEALLOC_BUFF(ssid->privkey_random_passwd);
            ssid->privkey_random_passwd = NULL;
        }

        ssid->privkey_random_passwd = SUPC_ALLOC_BUFF(size);
        if (ssid->privkey_random_passwd == NULL) {
            SUPC_ASSERT(0);
        }

        kal_mem_cpy(ssid->privkey_random_passwd, pwd, size);
        ssid->privkey_random_passwd_len = size;

        //wpa_printf(MSG_DEBUG, "client_cert_id in ssid : %d", ssid->client_cert_id);
//      kal_trace(DBG_CONN,INFO_SUPC_CLIENT_CERTID_IN_SSID, ssid->client_cert_id);
    } else if ((trans_id == wpa_s->current_phase2_trans_id) && (ssid->client_cert2_id == cert_id)) {
        if (ssid->privkey2_random_passwd != NULL) {
            SUPC_DEALLOC_BUFF(ssid->privkey2_random_passwd);
            ssid->privkey2_random_passwd = NULL;
        }

        ssid->privkey2_random_passwd = SUPC_ALLOC_BUFF(size);
        if (ssid->privkey2_random_passwd == NULL) {
            SUPC_ASSERT(0);
        }

        kal_mem_cpy(ssid->privkey2_random_passwd, pwd, size);
        ssid->privkey2_random_passwd_len = size;

        //wpa_printf(MSG_DEBUG, "client_cert2_id in ssid : %d", ssid->client_cert2_id);
//      kal_trace(DBG_CONN,INFO_SUPC_CLIENT_CERTID2_IN_SSID, ssid->client_cert2_id);
    } else
        // wpa_printf(MSG_DEBUG, "trans_id(%d,%d) or cert_id(%d,%d) in context mismatch with private_key_cb",
        //wpa_s->current_phase1_trans_id, wpa_s->current_phase2_trans_id, ssid->client_cert_id, ssid->client_cert2_id);
//  kal_trace(DBG_CONN,INFO_SUPC_CERT_MISMATCH_IN_PRIKEY_CB, wpa_s->current_phase1_trans_id,
//      wpa_s->current_phase2_trans_id, ssid->client_cert_id, ssid->client_cert2_id);

    {
        return;
    }
}


/*********************************************************************
   Function Name    : supc_get_cert_and_privkey
   Return Value     : kal_uint32
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* file
   Parameters       : file_info* temp
   Description      :
*********************************************************************/
kal_int32 supc_get_cert_and_privkey(wpa_ssid *ssid)
{
    /*Code Body*/

    if (wpa_s->last_trans_id >= CERTMAN_MAX_TRANS_ID) {
        wpa_s->last_trans_id = 0;
    }

    wpa_s->last_trans_id++;
    wpa_s->current_phase1_trans_id = wpa_s->last_trans_id;

    wpa_s->last_trans_id++;
    wpa_s->current_phase2_trans_id = wpa_s->last_trans_id;

    //phase 1 : trans_id = 0
    if (ssid->ca_cert_id != 0) {
        wpa_s->pending_cert_req ++;
        certman_send_get_cert_by_id_req(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->ca_cert_id,
                                        CERTMAN_ENC_PEM, CERTMAN_KP_SERVER_AUTH, NULL);
    }

    if (ssid->client_cert_id != 0) {
#ifdef __WAPI_SUPPORT__
        if (ssid->proto == WPA_PROTO_WAPI) {
            /* WAPI certificate uses simple signer equal parent two level only.
               No need to specify certificate purpose */
            ///wpa_printf(MSG_DEBUG, "supc_get_cert_and_privkey WAPI id %d\n",
            //(int)ssid->client_cert_id);
//          kal_trace(DBG_WAPI,INFO_SUPC_WAPI_GET_CERT1,(int)ssid->client_cert_id);

            wpa_s->pending_cert_req ++;
            certman_send_get_cert_by_id_req(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->client_cert_id,
//                CERTMAN_ENC_PEM, CERTMAN_KP_ALL, NULL);
                                            CERTMAN_ENC_DER, CERTMAN_KP_ALL, NULL);

            wpa_s->pending_cert_req ++;
            certman_send_get_privkey_req_auto_accept(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->client_cert_id,
                    NULL, CERTMAN_ENC_DER, ssid->private_key_passwd, NULL);
        } else {
            wpa_s->pending_cert_req ++;
            certman_send_get_cert_chain_req(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->client_cert_id,
                                            CERTMAN_KP_CLIENT_AUTH, NULL);
            wpa_s->pending_cert_req ++;
            certman_send_get_privkey_req_auto_accept(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->client_cert_id,
                    supc_set_private_key_cb, CERTMAN_ENC_PEM, ssid->private_key_passwd, NULL);

        }
#else
        wpa_s->pending_cert_req ++;
        certman_send_get_cert_chain_req(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->client_cert_id,
                                        CERTMAN_KP_CLIENT_AUTH, NULL);

        wpa_s->pending_cert_req ++;
        certman_send_get_privkey_req_auto_accept(MAP_MOD_SUPC, wpa_s->current_phase1_trans_id, ssid->client_cert_id,
                supc_set_private_key_cb, CERTMAN_ENC_PEM, ssid->private_key_passwd, NULL);
#endif
    }

    //phase 2 : trans_id = 1
    if (ssid->ca_cert2_id != 0) {
        wpa_s->pending_cert_req ++;
        certman_send_get_cert_by_id_req(MAP_MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->ca_cert2_id,
                                        CERTMAN_ENC_PEM, CERTMAN_KP_SERVER_AUTH, NULL);
    }

    if (ssid->client_cert2_id != 0) {
#ifdef __WAPI_SUPPORT__
        if (ssid->proto == WPA_PROTO_WAPI) {
            /* WAPI certificate uses simple signer equal parent two level only */
            //wpa_printf(MSG_DEBUG, "supc_get_cert_and_privkey WAPI id2 %d\n",
            //(int)ssid->client_cert2_id);
//          kal_trace(DBG_WAPI,INFO_SUPC_WAPI_GET_CERT2,(int)ssid->client_cert2_id);

            wpa_s->pending_cert_req ++;
            certman_send_get_cert_by_id_req(MAP_MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->client_cert2_id,
//                CERTMAN_ENC_PEM, CERTMAN_KP_ALL, NULL);
                                            CERTMAN_ENC_DER, CERTMAN_KP_ALL, NULL);
#if 0/* we don't have asu's private key */
            wpa_s->pending_cert_req ++;
            certman_send_get_privkey_req_auto_accept(MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->client_cert2_id,
                    NULL, CERTMAN_ENC_DER, ssid->private_key2_passwd, NULL);
#endif
        } else {
            wpa_s->pending_cert_req ++;
            certman_send_get_cert_chain_req(MAP_MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->client_cert2_id,
                                            CERTMAN_KP_CLIENT_AUTH, NULL);
            wpa_s->pending_cert_req ++;
            certman_send_get_privkey_req_auto_accept(MAP_MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->client_cert2_id,
                    supc_set_private_key_cb, CERTMAN_ENC_PEM, ssid->private_key2_passwd, NULL);
        }
#else
        wpa_s->pending_cert_req ++;
        certman_send_get_cert_chain_req(MAP_MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->client_cert2_id,
                                        CERTMAN_KP_CLIENT_AUTH, NULL);

        wpa_s->pending_cert_req ++;
        certman_send_get_privkey_req_auto_accept(MAP_MOD_SUPC, wpa_s->current_phase2_trans_id, ssid->client_cert2_id,
                supc_set_private_key_cb, CERTMAN_ENC_PEM, ssid->private_key2_passwd, NULL);
#endif
    }

    return (wpa_s->pending_cert_req);
}
#endif

#ifdef __WAPI_SUPPORT__
int supc_read_file(void *file, void *outbuf, int bufSz)
{
    FS_HANDLE   cFile;
    FS_FileInfo fileInfo;
    int iCount = 0;
    kal_uint32  read;
    kal_mem_set(&fileInfo, 0, sizeof(file_info));

    cFile = FS_Open(file, FS_READ_ONLY);

    if (cFile == NULL) {
        //wpa_printf(MSG_DEBUG,"Error occurs when doing FS_Open");
//      kal_trace(DBG_WAPI,INFO_SUPC_WAPI_OPEN_FILE_ERROR);
        return -1;
    }

    if (FS_GetFileInfo(cFile, &fileInfo) != FS_NO_ERROR) {
        FS_Close(cFile);
        //wpa_printf(MSG_DEBUG,"Error occurs when doing FS_GetFileInfo");
//      kal_trace(DBG_WAPI,INFO_SUPC_WAPI_GET_FILE_INFO_ERROR);
        return -2;
    }

    if (fileInfo.DirEntry->FileSize > bufSz) {
        FS_Close(cFile);
        //wpa_printf(MSG_DEBUG,"Error occurs buffer size is not large enough");
//      kal_trace(DBG_WAPI,INFO_SUPC_WAPI_BUFFER_SIZE_ERROR);
        return -3;
    }

    if (FS_Read(cFile, outbuf, fileInfo.DirEntry->FileSize, &read) != FS_NO_ERROR) {
        FS_Close(cFile);
        //wpa_printf(MSG_DEBUG,"Error FS_Read return value");
//      kal_trace(DBG_WAPI,INFO_SUPC_WAPI_FS_RETURN_ERROR);
        return -4;
    }
    if (read != fileInfo.DirEntry->FileSize) {
        FS_Close(cFile);
        //wpa_printf(MSG_DEBUG,"Error occurs read count is not expected");
//      kal_trace(DBG_WAPI,INFO_SUPC_WAPI_READ_COUNT_ERROR);
        return -5;
    }

    FS_Close(cFile);

    iCount = read;

    return iCount;
}
#endif

#if 0//#ifndef __CERTMAN_SUPPORT__
//Kinki add
/*********************************************************************
   Function Name    : supc_read_fileInfo
   Return Value     : int
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void* file
   Parameters       : file_info* temp
   Description      :
*********************************************************************/
int supc_read_fileInfo(void *file, file_info *temp)
{
    FS_HANDLE         cFile;
    FS_FileInfo fileInfo;

    kal_mem_set(temp, 0, sizeof(file_info));

    cFile = FS_Open(file, FS_READ_ONLY);

    if (cFile == NULL) {
        wpa_printf(MSG_DEBUG, "Error occurs when doing FS_Open");
        return -1;
    }

    if (FS_GetFileInfo(cFile, &fileInfo) != FS_NO_ERROR) { // Kinki add
        FS_Close(cFile);
        wpa_printf(MSG_DEBUG, "Error occurs when doing FS_GetFileInfo");
        return -2;
    } else {
        temp->dateTime.Year1980 = fileInfo.DirEntry->DateTime.Year1980 ;
        temp->dateTime.Month = fileInfo.DirEntry->DateTime.Month;
        temp->dateTime.Day = fileInfo.DirEntry->DateTime.Day;
        temp->dateTime.Hour = fileInfo.DirEntry->DateTime.Hour;
        temp->dateTime.Minute = fileInfo.DirEntry->DateTime.Minute ;
        temp->dateTime.Second2 = fileInfo.DirEntry->DateTime.Second2 ;
        temp->size = fileInfo.DirEntry->FileSize;
    }

    FS_Close(cFile);

    return 0;

}

//Kinki add
/*********************************************************************
   Function Name    : supc_set_fileInfo
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : FS_DOSDirEntry Info
   Parameters       : kal_uint8 id
   Description      :
*********************************************************************/
void supc_set_fileInfo(file_info *Info, kal_uint8 id, cert_type_enum type)
{
    switch (type) {
        case CA_CERT: //caCert
            kal_mem_set(&(wpa_s->caCertInfo[id]), 0, sizeof(file_info));
            kal_mem_cpy(&(wpa_s->caCertInfo[id]), Info, sizeof(file_info));
            //wpa_printf(MSG_DEBUG,"Setting caCert file information-DONE");
            break;
        case CLIENT_CERT: //clientCert
            kal_mem_set(&(wpa_s->clientCertInfo[id]), 0, sizeof(file_info));
            kal_mem_cpy(&(wpa_s->clientCertInfo[id]), Info, sizeof(file_info));
            //wpa_printf(MSG_DEBUG,"Setting clientCert file information-DONE");
            break;
        case PRIVATE_KEY_CERT: //privateKeyCert
            kal_mem_set(&(wpa_s->privateKeyCertInfo[id]), 0, sizeof(file_info));
            kal_mem_cpy(&(wpa_s->privateKeyCertInfo[id]), Info, sizeof(file_info));
            //wpa_printf(MSG_DEBUG,"Setting privateKeyCert file information-DONE");
            break;
        case CA_CERT_2: //caCert
            kal_mem_set(&(wpa_s->caCertInfo2[id]), 0, sizeof(file_info));
            kal_mem_cpy(&(wpa_s->caCertInfo2[id]), Info, sizeof(file_info));
            //wpa_printf(MSG_DEBUG,"Setting caCert2 file information-DONE");
            break;
        case CLIENT_CERT_2: //clientCert
            kal_mem_set(&(wpa_s->clientCertInfo2[id]), 0, sizeof(file_info));
            kal_mem_cpy(&(wpa_s->clientCertInfo2[id]), Info, sizeof(file_info));
            //wpa_printf(MSG_DEBUG,"Setting clientCert2 file information-DONE");
            break;
        case PRIVATE_KEY_CERT_2: //privateKeyCert
            kal_mem_set(&(wpa_s->privateKeyCertInfo2[id]), 0, sizeof(file_info));
            kal_mem_cpy(&(wpa_s->privateKeyCertInfo2[id]), Info, sizeof(file_info));
            //wpa_printf(MSG_DEBUG,"Setting privateKeyCert2 file information-DONE");
            break;
        default:
            wpa_printf(MSG_DEBUG, "Error occurs when setting file information");
            break;

    }
}

/*********************************************************************
   Function Name    : supc_get_fileInfo
   Return Value     : int
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : wifi_data_account_profile_struct *profile_ptr
   Parameters       :
   Description      :
*********************************************************************/
int supc_cmp_fileInfo(wifi_data_account_profile_struct *profile_ptr, kal_uint8 id)
{
    file_info tempInfo;

    if (kal_wstrlen(profile_ptr->ca_cert) != 0) {
        if (supc_read_fileInfo(profile_ptr->ca_cert, &tempInfo) < 0) {
            wpa_printf(MSG_DEBUG, "Errors occurs when reading ca_cert file information in profile %d", id);
            return -1;
        }

        if (kal_mem_cmp(&(wpa_s->caCertInfo[id]), &tempInfo, sizeof(file_info)) != 0) {
            wpa_printf(MSG_DEBUG, "ca_cert FILE SIZE = %d in profile %d", tempInfo.size, id);
            return 1;
        }
        //else
        //wpa_printf(MSG_DEBUG,"ca_cert file are the same in date and size in profile %d", id);
    }


    if (kal_wstrlen(profile_ptr->client_cert) != 0) {
        if (supc_read_fileInfo(profile_ptr->client_cert, &tempInfo) < 0) {
            wpa_printf(MSG_DEBUG, "Errors occurs when reading client_cert file information in profile %d", id);
            return -1;
        }

        if (kal_mem_cmp(&(wpa_s->clientCertInfo[id]), &tempInfo, sizeof(file_info)) != 0) {
            wpa_printf(MSG_DEBUG, "client_cert FILE SIZE = %d in profile %d", tempInfo.size, id);
            return 1;
        }
        //else
        //wpa_printf(MSG_DEBUG,"client_cert  file are the same in date and size in profile %d", id);
    }


    if (kal_wstrlen(profile_ptr->private_key) != 0) {
        if (supc_read_fileInfo(profile_ptr->private_key, &tempInfo) < 0) {
            wpa_printf(MSG_DEBUG, "Errors occurs when reading private_key file information in profile %d", id);
            return -1;
        }

        if (kal_mem_cmp(&(wpa_s->privateKeyCertInfo[id]), &tempInfo, sizeof(file_info)) != 0) {
            wpa_printf(MSG_DEBUG, "private_key FILE SIZE = %d in profile %d", tempInfo.size, id);
            return 1;
        }
        //else
        //wpa_printf(MSG_DEBUG,"private_key  file are the same in date and size in profile %d", id);
    }



    if (kal_wstrlen(profile_ptr->ca_cert2) != 0) {
        if (supc_read_fileInfo(profile_ptr->ca_cert2, &tempInfo) < 0) {
            wpa_printf(MSG_DEBUG, "Errors occurs when reading ca_cert2 file information in profile %d", id);
            return -1;
        }

        if (kal_mem_cmp(&(wpa_s->caCertInfo2[id]), &tempInfo, sizeof(file_info)) != 0) {
            wpa_printf(MSG_DEBUG, "ca_cert2 FILE SIZE = %d in profile %d", tempInfo.size, id);
            return 1;
        }
        //else
        //wpa_printf(MSG_DEBUG,"ca_cert2  file are the same in date and size in profile %d", id);
    }


    if (kal_wstrlen(profile_ptr->client_cert2) != 0) {
        if (supc_read_fileInfo(profile_ptr->client_cert2, &tempInfo) < 0) {
            wpa_printf(MSG_DEBUG, "Errors occurs when reading client_cert2 file information in profile %d", id);
            return -1;
        }

        if (kal_mem_cmp(&(wpa_s->clientCertInfo2[id]), &tempInfo, sizeof(file_info)) != 0) {
            wpa_printf(MSG_DEBUG, "client_cert2 FILE SIZE = %d in profile %d", tempInfo.size, id);
            return 1;
        }
        //else
        //wpa_printf(MSG_DEBUG,"client_cert2  file are the same in date and size in profile %d", id);
    }


    if (kal_wstrlen(profile_ptr->private_key2) != 0) {
        if (supc_read_fileInfo(profile_ptr->private_key2, &tempInfo) < 0) {
            wpa_printf(MSG_DEBUG, "Errors occurs when reading private_key2 file information in profile %d", id);
            return -1;
        }

        if (kal_mem_cmp(&(wpa_s->privateKeyCertInfo2[id]), &tempInfo, sizeof(file_info)) != 0) {
            wpa_printf(MSG_DEBUG, "private_key2 FILE SIZE = %d in profile %d", tempInfo.size, id);
            return 1;
        }
        //else
        //wpa_printf(MSG_DEBUG,"private_key2 file are the same in date and size in profile %d", id);
    }

    //wpa_printf(MSG_DEBUG,"All CA file are the same in date and size or there's no CA file in in profile %d", id);
    return 0;

}
#endif

/*********************************************************************
   Function Name    : supc_read_profiles
   Return Value     : wpa_ssid *
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : wifi_data_account_profile_struct *profile_ptr
   Parameters       : kal_uint8 id
   Description      :
*********************************************************************/
wpa_ssid *supc_read_profiles(wifi_data_account_profile_struct *profile_ptr, kal_uint8 id)
{

    wpa_ssid *ssid;
    kal_uint16 num_methods = 0;
    kal_uint8 i;
#if 0//#ifndef __CERTMAN_SUPPORT__
    file_info temp_info; // Kinki add
#endif

    ssid = (wpa_ssid *)SUPC_ALLOC_BUFF(sizeof(wpa_ssid));
    kal_mem_set(ssid, 0, sizeof(wpa_ssid));
    //wpa_printf(MSG_DEBUG, "wpa_ssid=%d", sizeof(wpa_ssid));
    /* J: Default value, it may be changed */
    ssid->proto = WPA_PROTO_WPA | WPA_PROTO_RSN;
    ssid->pairwise_cipher = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP;
    ssid->group_cipher = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP |
                         WPA_CIPHER_WEP104 | WPA_CIPHER_WEP40;
    ssid->key_mgmt = WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_IEEE8021X;
    ssid->eapol_flags = EAPOL_FLAG_REQUIRE_KEY_UNICAST |
                        EAPOL_FLAG_REQUIRE_KEY_BROADCAST;
    ssid->eap_workaround = (unsigned int) - 1;
    ssid->auth_alg = WPA_AUTH_ALG_OPEN;

    ssid->id = id;
    SUPC_ASSERT(profile_ptr->priority < SUPC_MAX_SSID_CONFIG_PRIORITY);
    ssid->priority = profile_ptr->priority;
    ssid->profile_id = profile_ptr->profile_id;

    kal_mem_cpy(ssid->ssid, profile_ptr->ssid, profile_ptr->ssid_len);
    ssid->ssid_len = profile_ptr->ssid_len;
    if (profile_ptr->bssid_p == KAL_TRUE) {
        kal_mem_cpy(ssid->bssid, profile_ptr->bssid, WNDRV_MAC_ADDRESS_LEN);
        ssid->bssid_set = KAL_TRUE;
    } else {
        ssid->bssid_set = KAL_FALSE;
    }

    if (profile_ptr->psk_p == KAL_TRUE) {
//#ifdef _SUPC_UNITTEST_
//   pbkdf2_sha1("aaa12345", ssid->ssid, ssid->ssid_len, 4096, ssid->psk, 32);
//#else
        kal_mem_cpy(ssid->psk, profile_ptr->psk, 32);
//#endif
        ssid->psk_set = KAL_TRUE;

#ifdef __WAPI_SUPPORT__
        if (profile_ptr->psk_type == 0) {
            /* HEX format */
            kal_mem_cpy(ssid->wapi_psk, profile_ptr->psk, 32);
            ssid->wapi_psk_type = 0;
        } else if (profile_ptr->psk_type == 1) {
            /* ASCII format */
            kal_mem_cpy(ssid->wapi_psk, profile_ptr->psk, 32);
            ssid->wapi_psk_type = 1;
        } else {
            // wpa_printf(MSG_INFO, "supc_read_profiles unexpected psk type %d\n", profile_ptr->psk_type);
//          kal_trace(DBG_WAPI,INFO_SUPC_WAPI_READ_PROFILE_UNEXPECTED,profile_ptr->psk_type);
            SUPC_ASSERT(0);
        }
        /* how to handle PSK here? */
#endif
    } else {
        ssid->psk_set = KAL_FALSE;
    }

    /* J: Start to read information from ABM data account */
    ssid->proto = profile_ptr->proto;
    ssid->pairwise_cipher = profile_ptr->pairwise;
    ssid->group_cipher = profile_ptr->group;
    ssid->key_mgmt = profile_ptr->key_mgmt;
    ssid->auth_alg = profile_ptr->auth_alg;
    ssid->network_type = profile_ptr->network_type;
    ssid->channel_number = profile_ptr->channel_number;

#ifdef __WAPI_SUPPORT__
    /* check invalid combination */

    //wpa_printf(MSG_INFO, "supc_read_profiles profile psk type %d pair %d group %d key %d auth %d\n",
    //(int)profile_ptr->proto, (int)profile_ptr->pairwise, (int)profile_ptr->group,
    //(int)profile_ptr->key_mgmt, (int)profile_ptr->auth_alg);
//  kal_trace(DBG_WAPI,INFO_SUPC_WAPI_READ_PROFILE_PSK_TYPE,
    (int)profile_ptr->proto, (int)profile_ptr->pairwise, (int)profile_ptr->group,
    (int)profile_ptr->key_mgmt, (int)profile_ptr->auth_alg);

    if (profile_ptr->proto & WPA_PROTO_WAPI) {
    SUPC_ASSERT(profile_ptr->pairwise == WAPI_CIPHER_SMS4);
        SUPC_ASSERT(profile_ptr->group == WAPI_CIPHER_SMS4);
        SUPC_ASSERT(profile_ptr->key_mgmt & (WAPI_KEY_MGMT_PSK | WAPI_KEY_MGMT_WAI));
        // SUPC_ASSERT(profile_ptr->auth_alg = WPA_AUTH_ALG_OPEN);
    }
#endif
    if (KAL_TRUE == profile_ptr->wep_info_p) {
    kal_mem_cpy(&(ssid->wep_key[0]), &(profile_ptr->wep_keys.wep_key[0]), profile_ptr->wep_keys.wep_key_len[0]);
        kal_mem_cpy(&(ssid->wep_key[1]), &(profile_ptr->wep_keys.wep_key[1]), profile_ptr->wep_keys.wep_key_len[1]);
        kal_mem_cpy(&(ssid->wep_key[2]), &(profile_ptr->wep_keys.wep_key[2]), profile_ptr->wep_keys.wep_key_len[2]);
        kal_mem_cpy(&(ssid->wep_key[3]), &(profile_ptr->wep_keys.wep_key[3]), profile_ptr->wep_keys.wep_key_len[3]);

        ssid->wep_key_len[0] = profile_ptr->wep_keys.wep_key_len[0];
        ssid->wep_key_len[1] = profile_ptr->wep_keys.wep_key_len[1];
        ssid->wep_key_len[2] = profile_ptr->wep_keys.wep_key_len[2];
        ssid->wep_key_len[3] = profile_ptr->wep_keys.wep_key_len[3];

        ssid->wep_tx_keyidx = profile_ptr->wep_keys.wep_tx_keyidx;
    }

    /* J, IDs */
    //wpa_printf(MSG_DEBUG, "identity_len=%d", profile_ptr->identity_len);
    if (profile_ptr->identity_len != 0) {
    ssid->identity = SUPC_ALLOC_BUFF(profile_ptr->identity_len);
        kal_mem_cpy(ssid->identity, profile_ptr->identity, profile_ptr->identity_len);
        ssid->identity_len = profile_ptr->identity_len;
    }
    if (profile_ptr->password_len != 0) {
    ssid->password = SUPC_ALLOC_BUFF(profile_ptr->password_len);
        kal_mem_cpy(ssid->password, profile_ptr->password, profile_ptr->password_len);
        ssid->password_len = profile_ptr->password_len;
    } else { //Kinki add for empty pwd
        ssid->password = NULL;
        ssid->password_len = 0;
    }

    //wpa_printf(MSG_DEBUG, "anonymous_identity_len=%d", profile_ptr->anonymous_identity_len);
    /*
    if(profile_ptr->anonymous_identity_len != 0)
    {
        ssid->anonymous_identity = SUPC_ALLOC_BUFF(profile_ptr->anonymous_identity_len);
        kal_mem_cpy(ssid->anonymous_identity, profile_ptr->anonymous_identity, profile_ptr->anonymous_identity_len);
        ssid->anonymous_identity_len = profile_ptr->anonymous_identity_len;
    }
    */
    // Remove "use anonymous" option, move the custom folder?
    ssid->anonymous_identity = NULL;
    ssid->anonymous_identity_len = 0;
#ifdef __WAPI_SUPPORT__
    /* WAPI has no EAP methods */
    if ((profile_ptr->proto & WPA_PROTO_WAPI) == 0) {
        /* J, get EAP mehotds */
        ssid->eap_methods = supc_get_eap_mothod_from_profile(profile_ptr->eap, &num_methods);
    }
#else
    /* J, get EAP mehotds */
    ssid->eap_methods = supc_get_eap_mothod_from_profile(profile_ptr->eap, &num_methods);
    //supc_get_eap_mothod_from_profile(profile_ptr, ssid->eap_methods);
#endif
    /* Joseph, if EAP_SIM and EAP_AKA, we have to fill the identity field */
    /* Start -- Jau modify if condition here for AKA method { */
    // org : if(profile_ptr->eap == WLAN_EAP_SIM)
    if ((profile_ptr->eap == WLAN_EAP_SIM) || (profile_ptr->eap == WLAN_EAP_AKA) ||
                (profile_ptr->eap == (WLAN_EAP_SIM | WLAN_EAP_AKA)))
        /* Stop  -- Jau modify if condition here for AKA method } */
    {
        // SUPC_ASSERT(profile_ptr->identity_len != 0);
#if 0
        SUPC_DEALLOC_BUFF(ssid->identity); // free previously allocated buffer in this function
        ssid->identity = SUPC_ALLOC_BUFF(16);

        ssid->identity[0] = 0x31;
        ssid->identity[1] = (wpa_s->imsi[1] >> 4) + 0x30;

        for (i = 2; i < 16; i++) {
            if ((i % 2) == 1) {
                ssid->identity[i] = (wpa_s->imsi[i / 2 + 1] >> 4) + 0x30;
            } else {
                ssid->identity[i] = (wpa_s->imsi[i / 2 + 1] & 0x0F) + 0x30;
            }
        }

        ssid->identity_len = 16;
#endif
        extern unsigned char milenage_simulation;
        SUPC_DEALLOC_BUFF(ssid->identity); // free previously allocated buffer in this function

        if (wpa_s->imsi_mnc_digits == 0x2 || wpa_s->imsi_mnc_digits == 0x3) { //need add realm to identity
            ssid->identity = SUPC_ALLOC_BUFF(16 + sizeof(eap_sim_realm) - 1);
        } else {
            ssid->identity = SUPC_ALLOC_BUFF(16);
        }
        /*Arima EAP issue*/
        //kal_prompt_trace(MOD_SUPC,"mnc=%d",wpa_s->imsi_mnc_digits);

        if (profile_ptr->eap == WLAN_EAP_SIM) {
            ssid->identity[0] = 0x31;
        } else {
            ssid->identity[0] = 0x30;    // aka use 0x30
        }
        ssid->identity[1] = (wpa_s->imsi[1] >> 4) + 0x30;

        for (i = 2; i < 16; i++) {
            if ((i % 2) == 1) {
                ssid->identity[i] = (wpa_s->imsi[i / 2 + 1] >> 4) + 0x30;
            } else {
                ssid->identity[i] = (wpa_s->imsi[i / 2 + 1] & 0x0F) + 0x30;
            }
        }
        //add for test
        if (milenage_simulation) {
            ssid->identity[1] = '2';
            ssid->identity[2] = '3';
            ssid->identity[3] = '2';
            ssid->identity[4] = '0';
            ssid->identity[5] = '1';
            ssid->identity[6] = '0';
            ssid->identity[7] = '0';
            ssid->identity[8] = '0';
            ssid->identity[9] = '0';
            ssid->identity[10] = '0';
            ssid->identity[11] = '0';
            ssid->identity[12] = '0';
            ssid->identity[13] = '0';
            ssid->identity[14] = '0';
            ssid->identity[15] = '0';
        }


        if ((wpa_s->imsi_mnc_digits == 0x2) || (wpa_s->imsi_mnc_digits == 0x3)) {

            kal_mem_cpy(&(ssid->identity[16]), eap_sim_realm, sizeof(eap_sim_realm) - 1);
            if (wpa_s->imsi_mnc_digits == 0x2) { //2bit mnc
                ssid->identity[15 + 11] = ssid->identity[4];
                ssid->identity[15 + 12] = ssid->identity[5];
            } else { //3bit mnc
                ssid->identity[15 + 10] = ssid->identity[4];
                ssid->identity[15 + 11] = ssid->identity[5];
                ssid->identity[15 + 12] = ssid->identity[6];
            }

            ssid->identity[15 + 17] = ssid->identity[1];
            ssid->identity[15 + 18] = ssid->identity[2];
            ssid->identity[15 + 19] = ssid->identity[3];
            ssid->identity_len = 16 + sizeof(eap_sim_realm) - 1;


        } else {
            ssid->identity_len = 16;
        }
    }


#if 1//#ifdef __CERTMAN_SUPPORT__
//for test
    /*
         ssid->ca_cert_id = 1032;
         ssid->client_cert_id = 1033;
         ssid->ca_cert2_id = 1032;
         ssid->client_cert2_id = 1033;
         */
    ssid->ca_cert_id = profile_ptr->ca_cert;
    ssid->client_cert_id = profile_ptr->client_cert;
    ssid->ca_cert2_id = profile_ptr->ca_cert2;
    ssid->client_cert2_id = profile_ptr->client_cert2;
#ifdef __WAPI_SUPPORT__
    /* wifi_data_account_profile_struct defines wapi_as_cert to store AS certificate. I decided to
       store it in wpa_ssid's client_cert2_id. Because there is no much different to handle cert_id & cert_id2
       WAPI has no CA so far. */
    if (profile_ptr->proto & WPA_PROTO_WAPI) {
    ssid->client_cert2_id = profile_ptr->wapi_as_cert;
    //wpa_printf(MSG_DEBUG, "supc_read_profiles read certificate id %d\n", ssid->client_cert2_id);
//       kal_trace(DBG_WAPI, INFO_SUPC_WAPI_READ_PROFILE_CERTID, ssid->client_cert2_id);
}
#endif
ssid->decrypted_or_not = KAL_FALSE;
ssid->get_all_cert_or_not = KAL_FALSE;
#else

    /* CAs */    //origin
    if (kal_wstrlen(profile_ptr->ca_cert) != 0) {
        ssid->ca_cert = SUPC_ALLOC_BUFF(sizeof(profile_ptr->ca_cert));
        kal_mem_cpy(ssid->ca_cert, profile_ptr->ca_cert, sizeof(profile_ptr->ca_cert));
        supc_read_fileInfo(ssid->ca_cert, &temp_info); // Kinki add
        supc_set_fileInfo(&temp_info, id, CA_CERT); //Kinki add
    }

    if (kal_wstrlen(profile_ptr->client_cert) != 0) {
        ssid->client_cert = SUPC_ALLOC_BUFF(sizeof(profile_ptr->client_cert));
        kal_mem_cpy(ssid->client_cert, profile_ptr->client_cert, sizeof(profile_ptr->client_cert));
        supc_read_fileInfo(ssid->client_cert, &temp_info); // Kinki add
        supc_set_fileInfo(&temp_info, id, CLIENT_CERT); //Kinki add
    }

    if (kal_wstrlen(profile_ptr->private_key) != 0) {
        ssid->private_key = SUPC_ALLOC_BUFF(sizeof(profile_ptr->private_key));
        kal_mem_cpy(ssid->private_key, profile_ptr->private_key, sizeof(profile_ptr->private_key));
        supc_read_fileInfo(ssid->private_key, &temp_info); // Kinki add
        supc_set_fileInfo(&temp_info, id, PRIVATE_KEY_CERT); //Kinki add
    }

    if (kal_wstrlen(profile_ptr->ca_cert2) != 0) {
        ssid->ca_cert2 = SUPC_ALLOC_BUFF(sizeof(profile_ptr->ca_cert2));
        kal_mem_cpy(ssid->ca_cert2, profile_ptr->ca_cert2, sizeof(profile_ptr->ca_cert2));
        supc_read_fileInfo(ssid->ca_cert2, &temp_info); // Kinki add
        supc_set_fileInfo(&temp_info, id, CA_CERT_2); //Kinki add
    }

    if (kal_wstrlen(profile_ptr->private_key2) != 0) {
        ssid->private_key2 = SUPC_ALLOC_BUFF(sizeof(profile_ptr->private_key2));
        kal_mem_cpy(ssid->private_key2, profile_ptr->private_key2, sizeof(profile_ptr->private_key2));
        supc_read_fileInfo(ssid->private_key2, &temp_info); // Kinki add
        supc_set_fileInfo(&temp_info, id, PRIVATE_KEY_CERT_2); //Kinki add
    }


    if (kal_wstrlen(profile_ptr->client_cert2) != 0) {
        ssid->client_cert2 = SUPC_ALLOC_BUFF(sizeof(profile_ptr->client_cert2));
        kal_mem_cpy(ssid->client_cert2, profile_ptr->client_cert2, sizeof(profile_ptr->client_cert2));
        supc_read_fileInfo(ssid->client_cert2, &temp_info); // Kinki add
        supc_set_fileInfo(&temp_info, id, CLIENT_CERT_2); //Kinki add
    }
#endif
//wpa_printf(MSG_DEBUG, "client_cert2=%d", sizeof(profile_ptr->client_cert2));

#if 1//#ifdef __CERTMAN_SUPPORT__
if (profile_ptr->private_key_passwd_len != 0) {
    ssid->private_key_passwd_len = profile_ptr->private_key_passwd_len;
    ssid->private_key_passwd = SUPC_ALLOC_BUFF(profile_ptr->private_key_passwd_len + 1);
        if (ssid->private_key_passwd == NULL) {
            SUPC_ASSERT(0);
        }
        kal_mem_cpy(ssid->private_key_passwd, profile_ptr->private_key_passwd, profile_ptr->private_key_passwd_len);
        *(ssid->private_key_passwd + ssid->private_key_passwd_len) = '\0';
    }
//wpa_printf(MSG_DEBUG, "private_key_passwd_len=%d", profile_ptr->private_key_passwd_len);

    if (profile_ptr->private_key2_passwd_len != 0) {
    ssid->private_key2_passwd_len = profile_ptr->private_key2_passwd_len;
    ssid->private_key2_passwd = SUPC_ALLOC_BUFF(profile_ptr->private_key2_passwd_len + 1);
        if (ssid->private_key2_passwd == NULL) {
            SUPC_ASSERT(0);
        }
        kal_mem_cpy(ssid->private_key2_passwd, profile_ptr->private_key2_passwd, profile_ptr->private_key2_passwd_len);
        *(ssid->private_key2_passwd + ssid->private_key2_passwd_len) = '\0';
    }

#else
if (profile_ptr->private_key_passwd_len != 0) {
    ssid->private_key_passwd_len = profile_ptr->private_key_passwd_len;
    ssid->private_key_passwd = SUPC_ALLOC_BUFF(profile_ptr->private_key_passwd_len);
        if (ssid->private_key_passwd == NULL) {
            SUPC_ASSERT(0);
        }
        kal_mem_cpy(ssid->private_key_passwd, profile_ptr->private_key_passwd, profile_ptr->private_key_passwd_len);

    }
//wpa_printf(MSG_DEBUG, "private_key_passwd_len=%d", profile_ptr->private_key_passwd_len);

    if (profile_ptr->private_key2_passwd_len != 0) {
    ssid->private_key2_passwd_len = profile_ptr->private_key2_passwd_len;
    ssid->private_key2_passwd = SUPC_ALLOC_BUFF(profile_ptr->private_key2_passwd_len);
        if (ssid->private_key2_passwd == NULL) {
            SUPC_ASSERT(0);
        }
        kal_mem_cpy(ssid->private_key2_passwd, profile_ptr->private_key2_passwd, profile_ptr->private_key2_passwd_len);

    }
#endif

    ssid->phase1 = profile_ptr->phase1;
    ssid->phase2 = profile_ptr->phase2;
    wpa_printf(TRACE_GROUP_1, "ssid->phase1 : %d\n", ssid->phase1);
    wpa_printf(TRACE_GROUP_1, "ssid->phase2 : %d\n", ssid->phase2);
//  kal_trace(DBG_CONN,INFO_SUPC_PHASE1_EAP, ssid->phase1);
//  kal_trace(DBG_CONN,INFO_SUPC_PHASE2_EAP, ssid->phase2);
    /*
    if (ssid->phase2 & SUPC_PHASE2_EAP_SIM)
    {
    SUPC_DEALLOC_BUFF(ssid->identity); // free previously allocated buffer in this function
        ssid->identity = SUPC_ALLOC_BUFF(16);

        ssid->identity[0] = 0x31;
        ssid->identity[1] = (wpa_s->imsi[1] >> 4) + 0x30;

        for (i = 2; i < 16; i++)
        {
            if((i%2) == 1)
            {
                ssid->identity[i] = (wpa_s->imsi[i/2 + 1] >> 4) + 0x30;
            }
            else
            {
                ssid->identity[i] = (wpa_s->imsi[i/2 + 1] & 0x0F) + 0x30;
            }
        }

        ssid->identity_len = 16;
    }
    */
    //wpa_printf(MSG_DEBUG, "supc_read_profiles: details");
//  kal_trace(DBG_CONN,INFO_SUPC_READ_PROFILE_DETAILS);
    wpa_hexdump(MSG_DEBUG, "SSID:", (char *)ssid->ssid, ssid->ssid_len);
    //wpa_hexdump(MSG_DEBUG, "psk:", (char*)ssid->psk, PMK_LEN);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_PSK,PMK_LEN,ssid->psk);
    /* Joseph not finish yet ~~!!! */
#ifdef __WAPI_SUPPORT__
    //wpa_printf(MSG_DEBUG,  "wapi_psk_type %d pair %d group %d key %d "
    //"proto %d auth %d cert_id %d cert2_id %d\n",
    //(int)ssid->wapi_psk_type, (int)ssid->pairwise_cipher,
    //(int)ssid->group_cipher, (int)ssid->key_mgmt,
    //(int)ssid->proto, (int)ssid->auth_alg, (int)ssid->client_cert_id, (int)ssid->ca_cert2_id);
//  kal_trace(DBG_WAPI,INFO_SUPC_WAPI_PSK_TYPE_DUMP,
    (int)ssid->wapi_psk_type, (int)ssid->pairwise_cipher,
    (int)ssid->group_cipher, (int)ssid->key_mgmt,
    (int)ssid->proto, (int)ssid->auth_alg, (int)ssid->client_cert_id, (int)ssid->ca_cert2_id);
    //wpa_hexdump(MSG_DEBUG, "wapi_psk:", (char*)ssid->wapi_psk, PMK_LEN);
//  kal_buffer_trace(DBG_WAPI,INFO_SUPC_DUMP_WAPI_PSK,PMK_LEN,ssid->wapi_psk);

#endif
    return ssid;
}


u8 *supc_get_eap_mothod_from_profile(kal_uint32 eap, kal_uint16 *num_methods)
{
    kal_uint8 i = 0;
    kal_uint8 *ret = NULL;

    ret = SUPC_ALLOC_BUFF(16);

    if (eap == 0) {
        *num_methods = 0;
        SUPC_DEALLOC_BUFF(ret); // Karen
        return NULL;
    }
    //modify for WiFi UE enhancement
    if (eap & WLAN_EAP_PEAP) {
        ret[i] = EAP_TYPE_PEAP;
        wpa_printf(TRACE_GROUP_1, "Get Outer : EAP_TYPE_PEAP\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_OUTER_PEAP);
        i++;
    }

    if (eap & WLAN_EAP_TTLS) {
        ret[i] = EAP_TYPE_TTLS;
        wpa_printf(TRACE_GROUP_1, "Get Outer : EAP_TYPE_TTLS\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_OUTER_TTLS);
        i++;
    }

    if (eap & WLAN_EAP_TLS) {
        ret[i] = EAP_TYPE_TLS;
        wpa_printf(TRACE_GROUP_1, "Get Outer : EAP_TYPE_TLS\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_OUTER_TLS);
        i++;
    }

    if (eap & WLAN_EAP_SIM) {
        ret[i] = EAP_TYPE_SIM;
        wpa_printf(TRACE_GROUP_1, "Get Outer : EAP_TYPE_SIM\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_OUTER_SIM);
        i++;
    }

    /* Start -- Jau add AKA type of return { */
    if (eap & WLAN_EAP_AKA) {
        ret[i] = EAP_TYPE_AKA;
        wpa_printf(MSG_DEBUG, "Get Outer : WLAN_EAP_AKA\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_OUTER_AKA);
        i++;
    }
    /* Stop -- Jau add AKA type of return } */

    if (eap & WLAN_EAP_MD5) {
        ret[i] = EAP_TYPE_MD5;
        i++;
    }

    if (eap & WLAN_EAP_FAST) {
        ret[i] = EAP_TYPE_FAST;
        i++;
    }

    if (eap & WLAN_EAP_MSCHAV2) {
        ret[i] = EAP_TYPE_MSCHAPV2;
        i++;
    }

    if (eap & WLAN_EAP_GTC) {
        ret[i] = EAP_TYPE_GTC;
        i++;
    }

    *num_methods = i;
    /* Use EAP_TYPE_NONE to end up the eap mothods */
    ret[i] = EAP_TYPE_NONE;

    return ret;

}
//20091014 Saker : change priority for check
u8 *supc_get_phase2_eap_mothods(kal_uint32 eap, kal_uint16 *num_methods)
{
    kal_uint8 i = 0;
    kal_uint8 *ret = NULL;

    ret = SUPC_ALLOC_BUFF(16);

    if (eap == 0) {
        *num_methods = 0;
        return NULL;
    }

    if (eap & SUPC_PHASE2_EAP_MSCHAV2) {
        ret[i] = EAP_TYPE_MSCHAPV2;
        wpa_printf(TRACE_GROUP_1, "Get Inner : EAP_TYPE_MSCHAPV2\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_MSCHAPV2);
        i++;
    }

    if (eap & SUPC_PHASE2_EAP_MD5) {
        ret[i] = EAP_TYPE_MD5;
        wpa_printf(TRACE_GROUP_1, "Get Inner : SUPC_PHASE2_EAP_MD5\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_MD5);
        i++;
    }

    if (eap & SUPC_PHASE2_EAP_GTC) {
        ret[i] = EAP_TYPE_GTC;
        wpa_printf(TRACE_GROUP_1, "Get Inner : SUPC_PHASE2_EAP_GTC\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_GTC);
        i++;
    }

    if (eap & SUPC_PHASE2_EAP_TTLS) {
        ret[i] = EAP_TYPE_TTLS;
        wpa_printf(TRACE_GROUP_1, "Get Inner : EAP_TYPE_TTLS\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_TTLS);
        i++;
    }

    if (eap & SUPC_PHASE2_EAP_PEAP) {
        ret[i] = EAP_TYPE_PEAP;
        wpa_printf(TRACE_GROUP_1, "Get Inner : SUPC_PHASE2_EAP_PEAP\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_PEAP);
        i++;
    }

    if (eap & SUPC_PHASE2_EAP_SIM) {
        ret[i] = EAP_TYPE_SIM;
        wpa_printf(TRACE_GROUP_1, "Get Inner : SUPC_PHASE2_EAP_SIM\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_SIM);
        i++;
    }

    /* Start -- Jau add AKA type of return { */
    if (eap & SUPC_PHASE2_EAP_AKA) {
        ret[i] = EAP_TYPE_AKA;
        wpa_printf(TRACE_GROUP_1, "Get Inner : SUPC_PHASE2_EAP_AKA\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_AKA);
        i++;
    }
    /* Stop -- Jau add AKA type of return } */

    if (eap & SUPC_PHASE2_EAP_FAST) {
        ret[i] = EAP_TYPE_FAST;
        wpa_printf(MSG_DEBUG, "Get Inner : EAP_TYPE_FAST\n");
//      kal_trace(DBG_EAP_METHOD,INFO_SUPC_EAP_INNER_FAST);
        i++;
    }

    if (eap & SUPC_PHASE2_EAP_TLS) {
        ret[i] = EAP_TYPE_TLS;
        i++;
    }

    *num_methods = i;
    /* Use EAP_TYPE_NONE to end up the eap mothods */
    ret[i] = EAP_TYPE_NONE;

    return ret;

}


void supc_wpa_free_ssid(wpa_ssid *ssid)
{

    SUPC_DEALLOC_BUFF(ssid->passphrase);
    ssid->passphrase = NULL;
    SUPC_DEALLOC_BUFF(ssid->identity);
    ssid->identity = NULL;
    SUPC_DEALLOC_BUFF(ssid->eappsk);
    ssid->eappsk = NULL;
    SUPC_DEALLOC_BUFF(ssid->eap_methods);
    ssid->eap_methods = NULL;
    SUPC_DEALLOC_BUFF(ssid->nai);
    ssid->nai = NULL;
    SUPC_DEALLOC_BUFF(ssid->server_nai);
    ssid->server_nai = NULL;
    SUPC_DEALLOC_BUFF(ssid->password);
    ssid->password = NULL;
    if (ssid->ca_cert) {
#if 0//#ifdef __CERTMAN_SUPPORT__
        FS_Delete((const WCHAR *)ssid->ca_cert);
#endif
        SUPC_DEALLOC_BUFF(ssid->ca_cert);
        ssid->ca_cert = NULL;
    }

    if (ssid->client_cert) {
#if 0//#ifdef __CERTMAN_SUPPORT__
        FS_Delete((const WCHAR *)ssid->client_cert);
#endif
        SUPC_DEALLOC_BUFF(ssid->client_cert);
        ssid->client_cert = NULL;
    }

    if (ssid->private_key) {
#if 0//#ifdef __CERTMAN_SUPPORT__
        FS_Delete((const WCHAR *)ssid->private_key);
#endif
        SUPC_DEALLOC_BUFF(ssid->private_key);
        ssid->private_key = NULL;
    }

    if (ssid->private_key_passwd) {
        SUPC_DEALLOC_BUFF(ssid->private_key_passwd);
        ssid->private_key_passwd = NULL;
    }


#if 1//#ifdef __CERTMAN_SUPPORT__
    if (ssid->privkey_random_passwd) {
        SUPC_DEALLOC_BUFF(ssid->privkey_random_passwd);
        ssid->privkey_random_passwd = NULL;
    }
#endif

    SUPC_DEALLOC_BUFF(ssid->dh_file);
    ssid->dh_file = NULL;
    SUPC_DEALLOC_BUFF(ssid->subject_match);
    ssid->subject_match = NULL;

    if (ssid->ca_cert2) {
#if 0//#ifdef __CERTMAN_SUPPORT__
        FS_Delete((const WCHAR *)ssid->ca_cert2);
#endif
        SUPC_DEALLOC_BUFF(ssid->ca_cert2);
        ssid->ca_cert2 = NULL;
    }

    if (ssid->client_cert2) {
#if 0//#ifdef __CERTMAN_SUPPORT__
        FS_Delete((const WCHAR *)ssid->client_cert2);
#endif
        SUPC_DEALLOC_BUFF(ssid->client_cert2);
        ssid->client_cert2 = NULL;
    }

    if (ssid->private_key2) {
#if 0//#ifdef __CERTMAN_SUPPORT__
        FS_Delete((const WCHAR *)ssid->private_key2);
#endif
        SUPC_DEALLOC_BUFF(ssid->private_key2);
        ssid->private_key2 = NULL;
    }

    if (ssid->private_key2_passwd) {
        SUPC_DEALLOC_BUFF(ssid->private_key2_passwd);
        ssid->private_key2_passwd = NULL;
    }

#if 1//#ifdef __CERTMAN_SUPPORT__
    if (ssid->privkey2_random_passwd) {
        SUPC_DEALLOC_BUFF(ssid->privkey2_random_passwd);
        ssid->privkey2_random_passwd = NULL;
    }
#endif

    SUPC_DEALLOC_BUFF(ssid->dh_file2);
    ssid->dh_file2 = NULL;
    SUPC_DEALLOC_BUFF(ssid->subject_match2);
    ssid->subject_match2 = NULL;
    SUPC_DEALLOC_BUFF(ssid->eap_methods);
    ssid->eap_methods = NULL;
    //SUPC_DEALLOC_BUFF(ssid->phase1);
    //ssid->phase1 = NULL;
    //SUPC_DEALLOC_BUFF(ssid->phase2);
    //ssid->phase2 = NULL;
    SUPC_DEALLOC_BUFF(ssid->pcsc);
    ssid->pcsc = NULL;
    SUPC_DEALLOC_BUFF(ssid->pin);
    ssid->pin = NULL;
    SUPC_DEALLOC_BUFF(ssid->eap_methods);
    ssid->eap_methods = NULL;
}

//}

/*********************************************************************
   Function Name    : supc_reinit_wpa
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void
   Description      :
*********************************************************************/
void supc_reinit_wpa(void)
{
    /* Joseph:
     * TBD
     */

    return;
}

/*********************************************************************
   Function Name    : supc_mic_failure_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : int pairwise
   Description      :
*********************************************************************/
void supc_mic_failure_handler(int pairwise)
{
    // u32 now;
    INNER_TRACE_STEP(1);
    wpa_printf(TRACE_ERROR, "Michael MIC failure detected, pairwise=%d\n", pairwise);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_MIC_FAIL_DETECT, pairwise);
#ifdef __WAPI_SUPPORT___
    SUPC_ASSERT(wpa_s->proto != WPA_PROTO_WAPI);
#endif
    wpa_supplicant_key_request(wpa_s, 1, pairwise);

    return;
#if 0
    kal_get_time(&now);

    if (wpa_s->last_michael_mic_error &&
            kal_ticks_to_secs(now - wpa_s->last_michael_mic_error) <= 60) {

        if (wpa_s->countermeasures == 1) {
            wpa_printf(MSG_DEBUG, "already in TKIP countermeasure");
            return;
        }
        wpa_msg(wpa_s, MSG_WARNING, "****************TKIP countermeasures "
                "started*******************");
        wpa_msg(wpa_s, MSG_WARNING, "now=%d, last=%d", kal_ticks_to_secs(now), kal_ticks_to_secs(wpa_s->last_michael_mic_error));
        /* Need to wait for completion of request frame. We do
         * not get any callback for the message completion, so
         * just wait a short while and hope for the best. */
        //usleep(10000);

//        wpa_supplicant_deauthenticate(
//            wpa_s, REASON_MICHAEL_MIC_FAILURE);

#if CONFIG_DRIVER_MTK
        wpa_s->reassociate = 1;
        // wpa_supplicant_cancel_auth_timeout(wpa_s);
#endif
        wpa_s->is_wpa_connect_req_pending = KAL_TRUE;
        supc_disconnect_network();

        /* initialize countermeasures */
        wpa_s->countermeasures = 1;

        if (wpa_s->countermeasure_timer_event_id == NULL) {
            wpa_s->countermeasure_timer_event_id = supc_start_wpa_countermeasure_timer(60);
        } else {
            wpa_printf(MSG_WARNING, "!!!Countermeasure event id is not null when trying to start");
        }
        /* TODO: mark the AP rejected for 60 second. STA is
         * allowed to associate with another AP.. ??? */
    }
    wpa_s->last_michael_mic_error = now;
#endif
}

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
void supc_blacklist_update(struct wpa_supplicant *wpa_s)
{
    //int p,q;
    int per_num = 0, temp_num = 0;
    int i = 0, j = 0; //Kinki add
    struct wpa_blacklist *e, *f;
    wndrv_supc_blacklist_update_req_struct *local_para_ptr = NULL;

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_blacklist_update_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_blacklist_update_req_struct), TD_RESET);

    e = wpa_s->permanent_blacklist;
    f = wpa_s->temporal_blacklist;

    while (e != NULL) {
        if ((wpa_s->current_ssid != NULL) && (e->profile_id == wpa_s->current_ssid->profile_id)) {
            kal_mem_cpy(local_para_ptr->blacklist[i].bssid, e->bssid, ETH_ALEN);
            wpa_printf(TRACE_GROUP_1, "Added params Permanent Blacklist BSSID " MACSTR " in blacklist_update_req\n",
                       MAC2STR(local_para_ptr->blacklist[i].bssid));
//         kal_trace(DBG_CONN,INFO_SUPC_ADD_BSSID_TO_PBLACKLIST_IN_UPDATE_REQ,MAC2STR(local_para_ptr->blacklist[i].bssid));
            i++;
        } else if (wpa_s->current_ssid == NULL) { // Kinki: if haven't associate with any profile
            kal_mem_cpy(local_para_ptr->blacklist[i].bssid, e->bssid, ETH_ALEN);
            wpa_printf(MSG_DEBUG, "Added params Permanent Blacklist BSSID " MACSTR " in blacklist_update_req\n",
                       MAC2STR(local_para_ptr->blacklist[i].bssid));
//            kal_trace(DBG_CONN,INFO_SUPC_ADD_BSSID_TO_PBLACKLIST_IN_UPDATE_REQ,MAC2STR(local_para_ptr->blacklist[i].bssid));
            i++;
        }

        //wpa_printf(MSG_DEBUG, "Added e->bssid Permanent Blacklist BSSID " MACSTR " in blacklist_update_req",
        //   MAC2STR(e->bssid));

        e = e->next;

        //}
    }
    per_num = i;
    j = per_num;

    while (f != NULL) {
        /*for(q = per_num; q < (wpa_s->temporal_num+per_num) ; q++)
        {
             if(q == per_num)
                j = q;
        */

        if (j == WNDRV_BSSID_MAX_NUM) { // Kinki :up to maximum
            break;
        }

        if ((wpa_s->current_ssid != NULL) && (f->profile_id == wpa_s->current_ssid->profile_id)) {
            kal_mem_cpy(local_para_ptr->blacklist[j].bssid, f->bssid, ETH_ALEN);
            wpa_printf(MSG_DEBUG, "Added params Temporal Blacklist BSSID " MACSTR " in blacklist_update_req\n",
                       MAC2STR(local_para_ptr->blacklist[j].bssid));
//             kal_trace(DBG_CONN,INFO_SUPC_ADD_BSSID_TO_TBLACKLIST_IN_UPDATE_REQ,MAC2STR(local_para_ptr->blacklist[j].bssid));
            j++;
        } else if (wpa_s->current_ssid == NULL) {
            kal_mem_cpy(local_para_ptr->blacklist[j].bssid, f->bssid, ETH_ALEN);
            wpa_printf(MSG_DEBUG, "Added params Temporal Blacklist BSSID " MACSTR " in blacklist_update_req\n",
                       MAC2STR(local_para_ptr->blacklist[j].bssid));
//             kal_trace(DBG_CONN,INFO_SUPC_ADD_BSSID_TO_TBLACKLIST_IN_UPDATE_REQ,MAC2STR(local_para_ptr->blacklist[j].bssid));
            j++;
        }

        //wpa_printf(MSG_DEBUG, "Added e->bssid Temporal Blacklist BSSID " MACSTR " in blacklist_update_req",
        //   MAC2STR(f->bssid));

        f = f->next;

        //}
    }
    temp_num = (j - per_num);

    if ((temp_num + per_num) <= WNDRV_BSSID_MAX_NUM) {
        local_para_ptr->blacklist_num = (temp_num + per_num);
    } else {
        SUPC_ASSERT(0);
    }

    wpa_printf(TRACE_GROUP_1, "WPA: Associating Temporal Blacklist number : %d\n", temp_num);
    wpa_printf(TRACE_GROUP_1, "WPA: Associating Permanent Blacklist number :%d\n", per_num);
    wpa_printf(TRACE_GROUP_1, "WPA: Associating Total Blacklist number :%d\n", local_para_ptr->blacklist_num);
//  kal_trace(DBG_CONN,INFO_SUPC_T_BLACKLIST_NUM, temp_num);
//  kal_trace(DBG_CONN,INFO_SUPC_P_BLACKLIST_NUM, per_num);
//  kal_trace(DBG_CONN,INFO_SUPC_ALL_BLACKLIST_NUM, local_para_ptr->blacklist_num);

    supc_send_wndrv_blacklist_update_req((local_para_struct *) local_para_ptr);

}
#endif

#ifdef __WPS_SUPPORT__
/*for WPS-enable*/
void supc_stop_wps_walk_timer(void)
{
    if (NULL != wpa_s->wps_walk_timer_id) {
        //kal_prompt_trace(MAP_MOD_SUPC,
        //"Stop wps_walk_timer event id: %x",
        //wpa_s->wps_walk_timer_id);
        wpa_printf(TRACE_GROUP_1, "supc stop wps walk time\n");
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_WPS_WALK_TIMER_EVENT,wpa_s->wps_walk_timer_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->wps_walk_timer_id));

        wpa_s->wps_walk_timer_id = NULL;
    }
}

eventid supc_start_wps_walk_timer(kal_uint32 delay)
{
    eventid evid = NULL;

    if (delay > 0) {
        supc_stop_wps_walk_timer();
        evid = evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                supc_wps_walk_timer_expiry_handler,
                                (void *)EVENT_PARAM_WPS_WALK_TIME,
                                (delay * KAL_TICKS_1_SEC));
        wpa_printf(TRACE_GROUP_1, "Start wps_pin_walk timer event id: %x, delay: %d sec\n",
                   evid, delay);
//      kal_trace(DBG_TIMER,INFO_SUPC_START_WPS_WALK_TIMER_EVENT,evid, delay);
    }
    return evid;

}

/**/
void supc_stop_wps_registrar_pbc_timer(void)
{
    if (NULL != wpa_s->wps_registrar_pbc_timer_id) {
        // kal_prompt_trace(MAP_MOD_SUPC,
        //"Stop wps registrar pbc timer event id: %x",
        // wpa_s->wps_registrar_pbc_timer_id);
//      kal_trace(DBG_TIMER,INFO_SUPC_STOP_REGISTRAR_PBC_TIMER_EVENT,wpa_s->wps_registrar_pbc_timer_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->wps_registrar_pbc_timer_id));

        wpa_s->wps_registrar_pbc_timer_id = NULL;
    }
}

eventid supc_start_wps_registrar_pbc_timer(kal_uint32 delay)
{
    eventid evid = NULL;

    if (delay > 0) {
        supc_stop_wps_registrar_pbc_timer();
        evid = evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                supc_wps_registrar_pbc_timer_expiry_handler,
                                (void *)EVENT_PARAM_PBC_REGISTTER_TIME,
                                (delay * KAL_TICKS_1_SEC));
        //kal_prompt_trace(MAP_MOD_SUPC,
        //"Start wps registrar pbc timer event id: %x, delay: %d sec",
        //evid, delay);
        wpa_printf(TRACE_GROUP_1, "Start wps registrar pbc timer event id: %x, delay: %d sec\n", evid, delay);
//      kal_trace(DBG_TIMER,INFO_SUPC_START_REGISTRAR_PBC_TIMER_EVENT,evid, delay);

    }
    return evid;
}

eventid supc_start_wps_reg_set_selected_timer(kal_uint32 delay)
{
    eventid evid = NULL;

    if (delay > 0) {
        supc_stop_wps_reg_set_selected_timer();
        evid = evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                                supc_wps_reg_selected_timer_expiry_handler,
                                (void *)EVENT_PARAM_WPS_REG_SET_SELECTED,
                                (delay * KAL_TICKS_1_SEC));
        //kal_prompt_trace(MAP_MOD_SUPC,
        //"Start wps registrar selected timer event id: %x, delay: %d sec",
        //evid, delay)£»
        wpa_printf(TRACE_GROUP_1, "Start wps registrar selected timer event id: %x, delay: %d sec",
                   evid, delay);
//      kal_trace(DBG_TIMER,INFO_SUPC_START_REGISTRAR_SELECT_TIMER_EVENT,evid, delay);
    }
    return evid;
}

void supc_stop_wps_reg_set_selected_timer(void)
{
    if (NULL != wpa_s->wps_reg_set_selected_timer_id) {
        //kal_prompt_trace(MAP_MOD_SUPC,
        //"Stop wps registrar pbc timer event id: %x",
        //wpa_s->wps_reg_set_selected_timer_id);
//         kal_trace(DBG_TIMER, INFO_SUPC_STOP_REGISTRAR_SELECT_TIMER_EVENT,wpa_s->wps_reg_set_selected_timer_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->wps_reg_set_selected_timer_id));

        wpa_s->wps_reg_set_selected_timer_id = NULL;
    }
}

#endif

int wpa_config_add_prio_network(struct wpa_config *config,
                                struct wpa_ssid *ssid)
{
    int prio = 0;
    struct wpa_ssid *last = NULL;
    struct wpa_ssid *cur = NULL;

    wpa_printf(TRACE_GROUP_1, "wpa_config_add_prio_network ++ \n");
//  kal_trace(DBG_CONN,INFO_SUPC_ADD_PRIO_NETWORK);

    for (; prio < SUPC_MAX_SSID_CONFIG_PRIORITY; prio ++) {
        if (ssid->priority == prio) {
            last = cur = config->pssid[prio];
            while (cur) {
                last = cur;
                cur = cur->pnext;
            }
            if (last) {
                last->pnext = ssid;
                ssid->pnext = NULL;
            } else {
                config->pssid[prio] = ssid;
            }
            wpa_printf(MSG_DEBUG, "insert at prio=%d \n", prio);
//          kal_trace(DBG_CONN,INFO_SUPC_ADD_PRIO_INSERT_PRIO, prio);

            return 0;
        }
    }
    wpa_printf(TRACE_GROUP_1, "match no prio\n");
//  kal_trace(DBG_CONN,INFO_SUPC_MATCH_NO_PRIO);
    return -1;
}

/**
 * wpa_config_update_prio_list - Update network priority list
 * @config: Configuration data from wpa_config_read()
 * Returns: 0 on success, -1 on failure
 *
 * This function is called to update the priority list of networks in the
 * configuration when a network is being added or removed. This is also called
 * if a priority for a network is changed.
 */
int wpa_config_update_prio_list(struct wpa_config *config)
{
    struct wpa_ssid *ssid;
    int ret = 0;

    wpa_printf(TRACE_GROUP_1, "wpa_config_update_prio_list\n");
//  kal_trace(DBG_CONN, INFO_SUPC_UPDATE_PRIO_LIST);

    kal_mem_set(config->pssid, 0, sizeof(config->pssid));
    ssid = config->ssid;
    while (ssid) {
        INNER_TRACE_STEP(1);
        ssid->pnext = NULL;
        if (wpa_config_add_prio_network(config, ssid) < 0) {
            ret = -1;
            INNER_TRACE_STEP(2);
        }
        INNER_TRACE_STEP(3);
        ssid = ssid->next;
    }
    //wpa_printf(MSG_DEBUG, "wpa_config_update_prio_list ret=%d", ret);
//  kal_trace(DBG_CONN, INFO_SUPC_UPDATE_PRIO_LIST_RET, ret);
    return ret;
}


/**
 * wpa_config_add_network - Add a new network with empty configuration
 * @config: Configuration data from wpa_config_read()
 * Returns: The new network configuration or %NULL if operation failed
 */

struct wpa_ssid *wpa_config_add_network(struct wpa_config *config)
{
    struct wpa_ssid *ssid = NULL;
    struct wpa_ssid *last = NULL;
    int id = -1;

    INNER_TRACE_STEP(1);
    ssid = config->ssid;
    while (ssid) {
        if (ssid->id > id) {
            id = ssid->id;
        }
        last = ssid;
        ssid = ssid->next;
    }
    id ++;
    wpa_printf(TRACE_GROUP_1, "new ssid's id=%d, last ssid=%x\n", id, last);
//  kal_trace(DBG_CONN, INFO_SUPC_ADD_NEW_NETWORK_SSID, id, last);
    ssid = os_zalloc(sizeof(struct wpa_ssid));
    if (ssid == NULL) {
        return NULL;
    }
    ssid->id = id;

    if (last != NULL) {
        last->next = ssid;
    } else {
        config->ssid = ssid;
    }
    wpa_config_update_prio_list(config);
    return ssid;
}

/**
 * wpa_config_free_ssid - Free network/ssid configuration data
 * @ssid: Configuration data for the network
 *
 * This function frees all resources allocated for the network configuration
 * data.
 */
void wpa_config_free_ssid(struct wpa_ssid *ssid)
{
    INNER_TRACE_STEP(1);
    supc_wpa_free_ssid(ssid);
}


/**
 * wpa_config_remove_network - Remove a configured network based on id
 * @config: Configuration data from wpa_config_read()
 * @id: Unique network id to search for
 * Returns: 0 on success, or -1 if the network was not found
 */
int wpa_config_remove_network(struct wpa_config *config, int id)
{
    struct wpa_ssid *ssid, *prev = NULL;

    ssid = config->ssid;
    while (ssid) {
        if (id == ssid->id) {
            break;
        }
        prev = ssid;
        ssid = ssid->next;
    }
    INNER_TRACE_STEP(1);
    if (ssid == NULL) {
        return -1;
    }

    if (prev) {
        prev->next = ssid->next;
    } else {
        config->ssid = ssid->next;
    }

    wpa_config_update_prio_list(config);
    wpa_config_free_ssid(ssid);
    return 0;
}

/*
@type WPS_TYPE_PBC or WPS_TYPE_PIN
@sec  countdown in second
*/
void report_supc_msg_tbl_size(void);
#ifdef __WPS_SUPPORT__
void supc_send_abm_wifi_wps_pbc_status(kal_bool overlapped)
{
    supc_abm_wifi_wps_info_ind_struct  *loc_ptr = NULL;
    ilm_struct *ilm_ptr = NULL;

    loc_ptr = construct_local_para(sizeof(supc_abm_wifi_wps_info_ind_struct), TD_RESET);
    kal_mem_set(loc_ptr->info, 0, sizeof(loc_ptr->info));
    loc_ptr->num = 1;

    loc_ptr->info[0].action = 2;
    loc_ptr->info[0].show = KAL_TRUE;
    loc_ptr->info[0].value = overlapped;

    ilm_ptr =  allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND;
    ilm_ptr->local_para_ptr = (local_para_struct *)loc_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);
}

void supc_send_abm_wifi_wps_info_ind(kal_uint8 type, kal_uint8 sec,
                                     kal_uint32 pincode, kal_bool show)
{
    supc_abm_wifi_wps_info_ind_struct  *loc_ptr = NULL;
    kal_uint8 num = 0;
    ilm_struct *ilm_ptr = NULL;

    /*All supc mantainer: following code is used to debug. don;t remove it.*/
    wpa_printf(TRACE_GROUP_1, "send wifi wps info to abm\n");
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_SEND_WPS_INFO_TO_ABM);
    //wpa_printf(MSG_DEBUG, "supc_send_abm_wifi_wps_info msgid=%d mapto %d",
    //MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND, MAP_MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND);
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_SEND_WPS_MSG_INFO, MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND, MAP_MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND);
    //wpa_printf(MSG_DEBUG, "supc_send_abm_wifi_wps_info 2  msgid=%d mapto %d",
    //MSG_ID_WNDRV_SUPC_STATUS_IND, MAP_MSG_ID_WNDRV_SUPC_STATUS_IND);
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_SEND_WPS_MSG_INFO2, MSG_ID_WNDRV_SUPC_STATUS_IND, MAP_MSG_ID_WNDRV_SUPC_STATUS_IND);
    //wpa_printf(MSG_DEBUG, "supc_send_abm_wifi_wps_info 3  mod=%d mapto %d",
    //MOD_SUPC, MAP_MOD_SUPC);
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_SEND_WPS_MSG_INFO3, MOD_SUPC, MAP_MOD_SUPC);
    /*end */
    if (WPS_TYPE_PBC == type) {
        num = 1;
    } else if (WPS_TYPE_PIN == type) {
        num = 2;
    } else {
        //kal_prompt_trace(MAP_MOD_SUPC,
        //"invalid params in %s type=%d, sec = %d",
        //__FUNCTION__, type, sec);
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_INVALID_PARA, type, sec);
        return;
    }

    loc_ptr = construct_local_para(sizeof(supc_abm_wifi_wps_info_ind_struct), TD_RESET);
    //kal_mem_set(loc_ptr->info, 0, sizeof(loc_ptr->info));
    loc_ptr->num = num;
// countdown timer
    loc_ptr->info[0].action = 0;
    loc_ptr->info[0].show = show;
    loc_ptr->info[0].value = sec;
// pin-code
    if (WPS_TYPE_PIN == type) {
        loc_ptr->info[1].action = 1;
        loc_ptr->info[1].show = show;
        loc_ptr->info[1].value = pincode;
    }

    ilm_ptr =  allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND;
    ilm_ptr->local_para_ptr = (local_para_struct *)loc_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);
}


/*FOR WPS porting*/
const kal_uint8 *wpa_scan_get_vendor_ie(const kal_uint8 *ie_buf,
                                        kal_uint16 len, u32 vendor_type)
{
    const u8 *end, *pos;

    pos = ie_buf;
    end = pos + len;

    while (pos + 1 < end) {
        if (pos + 2 + pos[1] > end) {
            break;
        }
        if (pos[0] == WLAN_EID_VENDOR_SPECIFIC && pos[1] >= 4 &&
                vendor_type == WPA_GET_BE32(&pos[2])) {
            return pos;
        }
        pos += 2 + pos[1];
    }

    return NULL;
}

struct wpabuf *wpa_scan_get_vendor_ie_multi(const kal_uint8 *ie_buf,
        kal_uint16 len, u32 vendor_type)
{
    struct wpabuf *buf;
    const u8 *end, *pos;

    buf = wpabuf_alloc(len);
    if (buf == NULL) {
        return NULL;
    }

    pos = (const u8 *)ie_buf;
    end = pos + len;
    hexdump("ap ie dump", ie_buf, len);
    while (pos + 1 < end) {
        if (pos + 2 + pos[1] > end) {
            break;
        }
        wpa_printf(TRACE_GROUP_2, "get a IE  EID=%d, len=%d\n",
                   pos[0], pos[1]);
//       kal_trace(DBG_WPS,INFO_SUPC_WPS_GET_IE,pos[0], pos[1]);
        if (pos[0] == WLAN_EID_VENDOR_SPECIFIC
                && pos[1] >= 4
                &&  vendor_type == WPA_GET_BE32(&pos[2])) {
            wpa_printf(TRACE_GROUP_2, "vendor_type= %x\n", vendor_type);
//          kal_trace(DBG_WPS,INFO_SUPC_WPS_VENDOR_TYPE,vendor_type);
            wpabuf_put_data(buf, pos + 2 + 4, pos[1] - 4);
        }
        pos += 2 + pos[1];
    }

    if (wpabuf_len(buf) == 0) {
        wpabuf_free(buf);
        buf = NULL;
        wpa_printf(MSG_DEBUG, "get_wifi_ie_multi fail\n");
//          kal_trace(DBG_WPS,INFO_SUPC_WPS_GET_WIFI_MULTI_FAIL);
    }
    return buf;
}
#endif

supc_encrypt_mode_enum supc_get_encrypt_mode(int cipher)
{
    supc_encrypt_mode_enum encrypt = SUPC_ENCRYPT_MODE_NONE;
    if (WPA_CIPHER_CCMP & cipher) {
        encrypt = SUPC_ENCRYPT_MODE_AES; //
    } else if (WPA_CIPHER_TKIP & cipher) {
        encrypt = SUPC_ENCRYPT_MODE_TKIP;
    } else if (WPA_CIPHER_WEP40 == cipher ||
               WPA_CIPHER_WEP104 == cipher) {
        encrypt = SUPC_ENCRYPT_MODE_WEP;
    }
    wpa_printf(TRACE_GROUP_1, "%s encry_mode=%d\n", __func__, encrypt);
//  kal_trace(DBG_CONN,INFO_SUPC_GET_ENCRYPT_MODE,encrypt);
    return encrypt;
}

supc_auth_mode_enum supc_get_auth_mode(int key_mgmt, int proto, int auth_alg)
{
    supc_auth_mode_enum auth_mode = SUPC_AUTH_MODE_OPEN;
    /*
        switch (key_mgmt)
        {
        case WPA_KEY_MGMT_PSK:
        case WPA_KEY_MGMT_PSK_SHA256:
            auth_mode = SUPC_AUTH_MODE_WPAPSK;
            break;
        case WPA_KEY_MGMT_NONE:
        case WPA_KEY_MGMT_WPA_NONE:
            auth_mode = SUPC_AUTH_MODE_OPEN
            break;
        case WPA_KEY_MGMT_IEEE8021X_NO_WPA:
            break;
        case WPA_KEY_MGMT_IEEE8021X:
            auth_mode = SUPC_AUTH_MODE_IEEE8021X;
            break;
        case WPA_KEY_MGMT_IEEE8021X_SHA256:
            break;
        case WPA_KEY_MGMT_FT_PSK:
            break;
        case WPA_KEY_MGMT_FT_IEEE8021X:
            break;
        case WPA_KEY_MGMT_WPS:
            break;
        default:
            break;
        }
        */
    if (WPA_KEY_MGMT_NONE == key_mgmt) {
        auth_mode = SUPC_AUTH_MODE_OPEN;
    } else if (WPA_KEY_MGMT_NONE == key_mgmt || WPA_AUTH_ALG_SHARED == auth_alg) {
        auth_mode = SUPC_AUTH_MODE_SHARED;
    } else if (WPA_KEY_MGMT_IEEE8021X_NO_WPA == key_mgmt ||
               WPA_KEY_MGMT_IEEE8021X_SHA256 == key_mgmt ||
               WPA_KEY_MGMT_FT_IEEE8021X == key_mgmt) {
        auth_mode = SUPC_AUTH_MODE_IEEE8021X;
    } else if (WPA_KEY_MGMT_PSK == key_mgmt ||
               WPA_KEY_MGMT_PSK_SHA256 == key_mgmt ||
               WPA_KEY_MGMT_FT_PSK == key_mgmt ||
               WPA_KEY_MGMT_WPS == key_mgmt) {
        if (WPA_PROTO_RSN  == proto) {
            auth_mode = SUPC_AUTH_MODE_WPA2PSK;
        } else if (WPA_PROTO_WPA == proto) {
            auth_mode = SUPC_AUTH_MODE_WPAPSK;
        } else {
            auth_mode = SUPC_AUTH_MODE_WPA_WPA2_PSK;
        }
    } else if (WPA_KEY_MGMT_IEEE8021X == key_mgmt) {
        if (WPA_PROTO_RSN == proto) {
            auth_mode = SUPC_AUTH_MODE_WPA2;
        } else if (WPA_PROTO_WPA == proto) {
            auth_mode = SUPC_AUTH_MODE_WPA;
        } else {
            auth_mode = SUPC_AUTH_MODE_WPA_WPA2_MIX;
        }
    }
    wpa_printf(TRACE_GROUP_1, "%s auth_mode=%d", __func__, auth_mode);
//  kal_trace(DBG_CONN,INFO_SUPC_GET_AUTH_MODE, auth_mode);
    return auth_mode;

}

void supc_send_empty_ap_list_cnf(void)
{
    ilm_struct *ilm_ptr = NULL;
    supc_abm_wifi_ap_list_cnf_struct *local_para_ptr = NULL;

    local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_ap_list_cnf_struct), TD_RESET);
    local_para_ptr->num_of_bss = 0;
    local_para_ptr->more_flag = FALSE;

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);
    ilm_ptr->msg_id = MAP_MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF;;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_ABM, MAP_SUPC_ABM_SAP, ilm_ptr);
}

void supc_scan_monitor_timer_expiry_handler(void *para_ptr)
{


    if (EVENT_PARAM_SCAN_MONITOR != (int)para_ptr) {
        wpa_s->scan_monitor_timer_id = NULL;
        wpa_printf(TRACE_GROUP_1, "Fatal Error! invalid scan monitor event_param\n");
//      kal_trace(DBG_TIMER,INFO_SUPC_SCAN_TIMER_INVALID_EVENT);
        return;
    }
    wpa_printf(TRACE_GROUP_1, "scan monitor timer expiry\n");
//  kal_trace(DBG_TIMER,INFO_SUPC_SCAN_TIMER_EXPIRY);
    wpa_s->scan_monitor_timer_id = NULL;
    /*to fix MAUI_03138937. let supc enter into SCAN_DONE state*/
    fgSiteSurveyReq = KAL_FALSE;
    /*end*/
    if (KAL_TRUE == wpa_s->is_mmi_req_ap_list) {
        /* send ap_cnf*/
        wpa_s->is_mmi_req_ap_list = KAL_FALSE;
        supc_send_empty_ap_list_cnf();

#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->wpa_state == WPA_DISCONNECTED) {
            if (wpa_s->chip_state != CHIP_POWER_OFF) { //Kinki add for power-saving
                wpa_s->chip_state = CHIP_POWER_OFF;
                supc_send_wndrv_chip_poweroff_req();
            }
        }
#endif
    }

}



void supc_stop_scan_monitor_timer(void)
{
    if (NULL != wpa_s->scan_monitor_timer_id) {
//        kal_trace(DBG_TIMER, INFO_SUPC_SCAN_STOP_TIMER,
//             wpa_s->scan_monitor_timer_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                            &(wpa_s->scan_monitor_timer_id));

        wpa_s->scan_monitor_timer_id = NULL;
    }
}


eventid supc_start_scan_monitor_timer(kal_uint32 delay)
{
    eventid evid = 0;

    evid = (evshed_set_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
                             supc_scan_monitor_timer_expiry_handler,
                             (void *)EVENT_PARAM_SCAN_MONITOR,
                             (delay * KAL_TICKS_1_SEC)));

    wpa_printf(TRACE_GROUP_1, "Start scan monitor timer event id: %x, delay: %d sec\n", evid, delay);
//   kal_trace(DBG_TIMER,INFO_SUPC_SCAN_MONI_TIMER_START,evid, delay);
    return evid;
}

typedef enum {
    SHOW_TYPE_1,
    SHOW_TYPE_2,
    SHOW_TYPE_3
} ShowType;

void supc_dbg_show_ssid(wpa_ssid *ssid, ShowType type)
{
    if (ssid == NULL) {
        wpa_printf(TRACE_GROUP_1, "ssid is NULL\n");
//      kal_trace(DBG_CONN,INFO_SUPC_DBG_SHOW_SSID_NULL);
        return;
    }
    if (type > SHOW_TYPE_1) {
        wpa_hexdump(MSG_DEBUG, "identity", (const char *)ssid->identity, ssid->identity_len);

        wpa_printf(TRACE_GROUP_1, "ca_cert=%d ca_cert2=%d cli_cert=%d cli_cert2=%d\n",
                   ssid->profile_id, ssid->ca_cert_id, ssid->client_cert_id, ssid->client_cert2_id);
//      kal_trace(DBG_CONN,INFO_SUPC_DUMP_CERT,ssid->profile_id, ssid->ca_cert_id, ssid->client_cert_id, ssid->client_cert2_id);
//      kal_trace(DBG_CONN,INFO_SUPC_DUMP_PROFILE_BASIC, ssid->profile_id, ssid->priority, ssid->channel_number);
        //wpa_printf(MSG_DEBUG, "profile id = %d , prio=%d channel=%d",
        //ssid->profile_id, ssid->priority, ssid->channel_number);
        wpa_hexdump(MSG_DEBUG, "SSID", (char *)ssid->ssid, ssid->ssid_len);
        wpa_hexdump(MSG_DEBUG, "BSSID", (char *)ssid->bssid, ETH_ALEN);
    }
    if (type > SHOW_TYPE_2) {
        wpa_printf(TRACE_GROUP_1, "key_mgmt = %d proto=%d auth_alg=%d pairwise_cipher=%d gp_cipher=%d\n",
                   ssid->key_mgmt, ssid->proto, ssid->auth_alg, ssid->pairwise_cipher, ssid->group_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_DUMP_KEY_MGMT, ssid->key_mgmt, ssid->proto, ssid->auth_alg, ssid->pairwise_cipher, ssid->group_cipher);
    }
}


void supc_dbg_dump_config(void)
{
    wpa_config *conf = wpa_s->conf;
    wpa_ssid *ssid = NULL;
    int i = 0;
    if (conf == NULL) {
        wpa_printf(TRACE_GROUP_1, "wpa_s->conifg is NULL!\n");
//      kal_trace(DBG_CONN,INFO_SUPC_DUMP_NULL_CONF);
        return;
    }
    wpa_printf(MSG_DEBUG, "dump ssid in conf\n");
//kal_trace(DBG_CONN,INFO_SUPC_DUMP_CONF_START);
    ssid = conf->ssid;
    while (ssid) {
        i++;
        wpa_printf(MSG_DEBUG, "-------SSID %d--------\n", i);
//        kal_trace(DBG_CONN,INFO_SUPC_DUMP_CONF_SSID,i);
        supc_dbg_show_ssid(ssid, SHOW_TYPE_3);
        ssid = ssid->next;
    }
}

void supc_dbg_show_ssid_ex(wpa_ssid *ssid)
{
    supc_dbg_show_ssid(ssid, SHOW_TYPE_3);
}

void supc_dbg_show_current_ssid(void)
{

//  kal_trace(DBG_CONN,INFO_SUPC_SHOW_CURRENT_SSID);
    supc_dbg_show_ssid(wpa_s->current_ssid, SHOW_TYPE_3);
//  kal_trace(DBG_CONN,INFO_SUPC_SHOW_CURRENT_SSID_END);
}

void supc_dbg_dump_profiles(void)
{
    kal_uint16 i = 0;
    wifi_data_account_profile_struct *p = NULL;
    wpa_printf(TRACE_GROUP_1, "-------[%d] profiles -----\n", wpa_s->num_of_profile);
//  kal_trace(DBG_CONN,INFO_SUPC_DUMP_PROFILE,wpa_s->num_of_profile);
    for (; i < wpa_s->num_of_profile; i ++) {
        p = &wpa_s->profile[i];
        wpa_hexdump(MSG_INFO, "SSID", (char *)p->ssid, p->ssid_len);
        if (p->bssid_p) {
            wpa_printf(TRACE_GROUP_1, "BSSID: " MACSTR "\n", MAC2STR(p->bssid));
//          kal_buffer_trace(DBG_CONN,INFO_SUPC_PROFILE_BSSID,6,p->bssid);
        }
//      kal_trace(DBG_CONN,INFO_SUPC_DUMP_PROFILE_INFO,p->priority, p->profile_id,
//            p->proto, p->key_mgmt, p->eap);
        //wpa_printf(MSG_INFO, "prio=%d, profID=%d, proto=%d, key_mgmt=%d, eap=%d",
        //p->priority, p->profile_id,
        //p->proto, p->key_mgmt, p->eap);
        if (p->psk_p) {
            //wpa_hexdump(MSG_INFO, "PSK", (char*)p->psk, 32);
//          kal_buffer_trace(DBG_CONN,INFO_SUPC_PROFILE_PSK,32,p->psk);
        }
        wpa_hexdump(MSG_INFO, "identity", (char *)p->identity, p->identity_len);
    }
    wpa_printf(TRACE_GROUP_1, "------- dump profile over   -----\n");
//  kal_trace(DBG_CONN,INFO_SUPC_DUMP_PROFILE_OVER);
}

void supc_dbg_dump_supplicant(void)
{
    //wpa_printf(MSG_DEBUG, "---wpa supplicant---------++");
//   kal_trace(DBG_CONN,INFO_SUPC_DUMP_SUPC_START);
    //wpa_printf(MSG_DEBUG, "supplicant: state=%d proto= %d, key_mgmt=%d, pair_cipher=%d, gp_cipher=%d",
    //wpa_s->wpa_state, wpa_s->proto, wpa_s->key_mgmt,
    //wpa_s->pairwise_cipher, wpa_s->group_cipher);
//  kal_trace(DBG_CONN,INFO_SUPC_DUMP_SUPC_CONFIG,wpa_s->wpa_state, wpa_s->proto, wpa_s->key_mgmt,
//            wpa_s->pairwise_cipher, wpa_s->group_cipher);
    // wpa_printf(MSG_DEBUG, "supplicant: owne MAC: "MACSTR, MAC2STR(wpa_s->own_addr));
//  kal_buffer_trace(DBG_CONN,INFO_SUPC_DUMP_OWN_MAC,6,wpa_s->own_addr);
    // wpa_printf(MSG_DEBUG, "current bssid: " MACSTR, MAC2STR(wpa_s->bssid));
//  kal_buffer_trace(DBG_CONN,INFO_SUPC_DUMP_BSSID,6,wpa_s->bssid);
    //wpa_printf(MSG_DEBUG, "profile num= %d", wpa_s->num_of_profile);
//  kal_trace(DBG_CONN,INFO_SUPC_PROFILE_NUM,wpa_s->num_of_profile);
    //wpa_printf(MSG_DEBUG, "report mmi flag=%d, ap list flag=%d, "
    //"handover time=%d, scan peing flag=%d, conn pending = %d",
    //wpa_s->is_reported_mmi_fail, wpa_s->is_mmi_req_ap_list,
    //wpa_s->handover_timer_value, wpa_s->is_site_survey_req_pending,
    //wpa_s->is_wpa_connect_req_pending);
//  kal_trace(DBG_CONN,INFO_SUPC_MMI_REQ_STA,
//        wpa_s->is_reported_mmi_fail, wpa_s->is_mmi_req_ap_list,
//        wpa_s->handover_timer_value, wpa_s->is_site_survey_req_pending,
//        wpa_s->is_wpa_connect_req_pending);
    supc_dbg_show_current_ssid();
    supc_dbg_dump_profiles();

//  kal_trace(DBG_CONN,INFO_SUPC_DUMP_SUPC_OVER);

}


