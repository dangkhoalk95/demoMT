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



//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#endif
#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "supc_abm_msgs.h"

#include "supc_common.h"
#include "eapol_sm.h"
#include "eap.h"
#include "wpa.h"
#include "driver.h"
#include "eloop.h"
#include "wpa_supplicant.h"
#include "config_ssid.h"
#include "supc_config.h"
#include "l2_packet.h"
#include "wpa_supplicant_i.h"

#include "supc_init.h"



wpa_supplicant supc_context_g;
wpa_supplicant *wpa_s;
wpa_timer_context_struct wpa_timer_context_g;
//extern kal_bool supc_support_pre_authentication(void);

#ifdef _SIM_BLOCK_FUNC_SUPPORT_
kal_eventgrpid supc_sim_block_ev_id;
#endif //_SIM_BLOCK_FUNC_SUPPORT_

#ifdef _SUPC_MEM_ADM

// #define SUPC_MEM_ADM_SIZE (150 * 1024) // Karen, 90K
#ifdef __WIFI_SLIM__
#define SUPC_MEM_ADM_SIZE (15 * 1024) /*wifi_slim only used in mt6255 and psk-only*/
#else
#define SUPC_MEM_ADM_SIZE (60 * 1024)   // Karen, SSL memory is managed by SSL wrapper
#endif

kal_uint32 supc_mem_adm_pool_g[SUPC_MEM_ADM_SIZE / 4];
KAL_ADM_ID supc_mem_adm_id;

#endif //_SUPC_MEM_ADM


/*********************************************************************
   Function Name    : supc_init
   Return Value     : kal_bool
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : task_indx_type task_index
   Description      :
*********************************************************************/
kal_bool supc_init(task_indx_type task_index)
{
    wpa_s = &supc_context_g;
    kal_mem_set(wpa_s, 0, sizeof(wpa_supplicant));

    // init wpa_s context
    wpa_s->head = wpa_s;
    //wpa_s->driver = &wpa_driver_mtk_ops
    wpa_s->ifname = "MTK";
    wpa_s->last_michael_mic_error = 0;
    // J: create base timer
#if 0
    stack_init_timer(&(wpa_timer_context_g.wpa_base_timer), "SUPC_BASE_TIMER", MAP_MOD_SUPC);
#endif
    // J: Event scheduler
    // J: Ask CC about the max_delay_ticks
#if 0
    wpa_timer_context_g.wpa_event_scheduler_ptr =
        (event_scheduler *)
        new_evshed(&(wpa_timer_context_g.wpa_base_timer),
                   wpa_start_base_timer, wpa_stop_base_timer, 0,
                   kal_evshed_get_mem, kal_evshed_free_mem, 255);
#endif
    wpa_timer_context_g.wpa_event_scheduler_ptr =
        (event_scheduler *)evshed_create("SUPC_BASE_TIMER", MAP_MOD_SUPC, 0, 0);

    wpa_s->scan_timer_event_id = NULL;
    wpa_s->auth_timer_event_id = NULL;
    wpa_s->preauth_timer_event_id = NULL;
    wpa_s->handover_timer_event_id = NULL;
    wpa_s->countermeasure_timer_event_id = NULL;
    wpa_s->report_mmi_timer_event_id = NULL;
#ifdef __WAPI_SUPPORT__
    wpa_s->wapi_auth_timer_event_id = NULL;/* used to invoke WAPI IP's timer callback */
#endif

    wpa_s->keys_cleared = 1;
    wpa_s->wpa_state = WPA_DEINITED;
#ifdef __WIFI_CHIP_DYM_POWERON__
    wpa_s->chip_state = CHIP_POWER_OFF; //Kinki add this for power-saving mode
#endif
    wpa_s->num_of_profile = 0;
    wpa_s->imsi_ready = KAL_FALSE;
    wpa_s->need_to_send_init_cnf = KAL_FALSE;
    kal_mem_set(wpa_s->imsi, 0, sizeof(wpa_s->imsi));
    kal_mem_set(wpa_s->eap_sim_id, 0, sizeof(wpa_s->eap_sim_id));

    wpa_s->conf = NULL;
    wpa_s->countermeasures = 0;

    wpa_s->pmksa = NULL;

#ifdef _SIM_BLOCK_FUNC_SUPPORT_
    wpa_s->supc_sim_block_ev_id = kal_create_event_group("GSM_AUTH");
    supc_sim_block_ev_id = wpa_s->supc_sim_block_ev_id;
#endif //_SIM_BLOCK_FUNC_SUPPORT_

#ifdef _SUPC_CHECK_MEM_LEAK

    kal_mem_set(&(supc_mem_info[0]), 0, sizeof(supc_mem_check_strcut) * SUPC_MEM_INFO_MAX_REC); // Karen
    supc_mem_info_index = 0;

#endif /* _SUPC_CHECK_MEM_LEAK */

#ifdef _SUPC_MEM_ADM
    supc_mem_adm_id = kal_adm_create(supc_mem_adm_pool_g, SUPC_MEM_ADM_SIZE, NULL, KAL_TRUE);
#endif /*_SUPC_MEM_ADM */

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_s->permanent_blacklist = NULL;
    wpa_s->temporal_blacklist = NULL;
    wpa_s->temporal_num = 0;
    wpa_s->permanent_num = 0;
    wpa_s->temporal_blacklist_timer_event_id = NULL;
    wpa_s->permanent_blacklist_timer_event_id = NULL;

    wpa_s->auth_timeout_list = NULL;
    wpa_s->auth_timeout_bssid_num = 0;
#endif
#ifdef __WPS_SUPPORT__
    wpa_s->wps_walk_timer_id = NULL;
    wpa_s->wps_registrar_pbc_timer_id = NULL;
    wpa_s->wps_reg_set_selected_timer_id = NULL;
#endif
    wpa_s->scan_monitor_timer_id = NULL;
#if 1//#ifdef __CERTMAN_SUPPORT__
    wpa_s->last_trans_id = 0;
    wpa_s->current_phase1_trans_id = 0;
    wpa_s->current_phase2_trans_id = 0;
#endif
    //add for WiFi UE enhancement
    wpa_s->eap_fail_cause = SUPC_ABM_TIMEOUT;
    wpa_s->phase2_nak  = 0;
    wpa_s->rx_identity = 0;
    /*MAUI_03055418. ssl context is created independly on
    EAPOL and preauth EAPOL SM. both eapol sm use the same ssl context
    (eap.globalCtx)*/


//  wpa_s->support_pre_auth = supc_support_pre_authentication();
//  kal_trace(DBG_INIT,INFO_SUPC_SUPPORT_PRE_AUTH,wpa_s->support_pre_auth);



    return KAL_TRUE;
}


/*********************************************************************
   Function Name    : supc_reset
   Return Value     : kal_bool
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : task_indx_type task_index
   Description      :
*********************************************************************/
kal_bool supc_reset(task_indx_type task_index)
{
    //wpa_s->wpa_state = WPA_DISCONNECTED;

    return KAL_TRUE;
}

/*********************************************************************
   Function Name    : supc_deinit
   Return Value     : kal_bool
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : task_indx_type task_index
   Description      :
*********************************************************************/
kal_bool supc_deinit(task_indx_type task_index)
{
    return KAL_TRUE;
}

#if 0
/*********************************************************************
   Function Name    :
   Return Value     : void wpa_start_base_timer
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void *base_timer_p
   Parameters       : unsigned int time_out
   Description      :
*********************************************************************/
void wpa_start_base_timer(void *base_timer_p, unsigned int time_out)
{

    stack_start_timer((stack_timer_struct *)base_timer_p, 0, time_out);
    return;
}



/*********************************************************************
   Function Name    :
   Return Value     : void wpa_stop_base_timer
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : void *base_timer_p
   Description      :
*********************************************************************/
void wpa_stop_base_timer(void *base_timer_p)
{

    stack_stop_timer((stack_timer_struct *)base_timer_p);
    return;
}
#endif

