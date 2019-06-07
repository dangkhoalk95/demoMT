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

//#include "kal_release.h"          /* Basic data type */
//replace below
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif
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

#include "supc_wpa_functions.h"
#include "supc_wpa_common.h"

//#include "kal_release.h"
//#include "stack_common.h"
#include "stack_msgs.h"
//#include "app_ltlcom.h"
//#include "app_buff_alloc.h"
//#include "stack_utils.h"
//#include "divider_public.h"       /* H/W divider */
typedef char            S8;
typedef unsigned short  U16;
//#include "gv.h"
//#include "tst_def.h"

//#include "fs_type.h"
//#include "fs_func.h"
//#include "fs_errcode.h"
#include "supc_init.h"
//#include "ssl_api.h"
//#include "supc_ssl.h"
#if 0
extern U16 mmi_asc_to_ucs2(S8 *pOutBuffer, S8 *pInBuffer);
extern msg_type supc_msg_mapping_table[];
void supc_tst_start_wps_connect_handler(ilm_struct *ilm_ptr);
void supc_tst_start_wps_disconn_handler(ilm_struct *ilm_ptr);
void supc_tst_open_ssl_file_handler(ilm_struct *ilm_ptr);
void supc_tst_switch_conn_type(ilm_struct *ilm_ptr);
void supc_tst_dbg(ilm_struct *ilm_ptr);


const PFN_TST_INJECT_HANDLER apfn_tst_handlers[] = {
    supc_tst_open_ssl_file_handler,
    supc_tst_start_wps_connect_handler,
    supc_tst_start_wps_disconn_handler,
    supc_tst_switch_conn_type,
    supc_tst_dbg,
};
#endif
/*********************************************************************
   Function Name    : supc_main
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct      *ilm_ptr
   Description      :
*********************************************************************/
void supc_main(ilm_struct      *ilm_ptr)
{
    //switch( ilm_ptr->msg_id )
    //{
#if 0   //temp disable for not used
    if (ilm_ptr->msg_id == MSG_ID_TST_INJECT_STRING) { //MSG_ID_TST_INJECT_STRING

        tst_inject_string_struct *inject_str = (tst_inject_string_struct *)(ilm_ptr->local_para_ptr);
        //wpa_printf(MSG_DEBUG, "num of apfn_tst_handlers=%d", DIM(apfn_tst_handlers));
        kal_trace(DBG_TST, INFO_SUPC_TST_APFN_NUM, DIM(apfn_tst_handlers));
        wpa_hexdump(MSG_DEBUG, "ilm content", (const char *)ilm_ptr, sizeof(ilm_struct));
        wpa_hexdump(MSG_DEBUG, "inject_string content", (const char *)inject_str, sizeof(tst_inject_string_struct));

        if (inject_str->index < DIM(apfn_tst_handlers)) {
            apfn_tst_handlers[inject_str->index](ilm_ptr);
        } else {
            //kal_prompt_trace(MAP_MOD_SUPC, "bad inject msg index");
            kal_trace(DBG_TST, INFO_SUPC_TST_BAD_INJECT_MSG);
        }
    } else
#endif
#if 0
        // MSGs from SIM
        else if (ilm_ptr->msg_id == MAP_MSG_ID_SIM_READY_IND) { //MSG_ID_SIM_READY_IND
            supc_sim_ready_ind_handler(ilm_ptr);
            //break;
        }

        else if (ilm_ptr->msg_id == MSG_ID_SIM_READ_CNF) {

            supc_sim_read_cnf_handler(ilm_ptr);

        }
#endif

    // Timeout MSGs
    if (ilm_ptr->msg_id == MSG_ID_TIMER_EXPIRY) { //case MSG_ID_TIMER_EXPIRY:
        supc_timer_expiry_hdlr(ilm_ptr);
        //break;
    }
    // MSGs from WNDRV
    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_DATA_IND) { //case MSG_ID_WNDRV_SUPC_DATA_IND:
        supc_data_ind_handler(ilm_ptr);
        //break;
    } else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_INIT_REQ) { //ase MSG_ID_SUPC_ABM_WIFI_INIT_REQ:
#if 0
        ilm_struct *ilm_ptr = NULL;

        supc_abm_wifi_connect_req_struct *local_para_ptr = NULL;
        kal_uint8 key[16] = {236, 175, 68, 11, 60, 189, 6, 139, 172, 168, 186, 81, 16, 222, 231, 170};


        if (KAL_FALSE == gprs_flc_is_flc_context_activated(SPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_DL, 0)) {
            gprs_flc_activate_context(SPDU_DATA,
                                      TCP_OVER_WIFI,
                                      GPRS_FLC_DL,
                                      0);
        }


        local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_req_struct), TD_UL);


        /* TEST CODE TEST CODE */
        {
            //kal_uint32 allocated_ul_size, allocated_dl_size;
            peer_buff_struct *peer_buff_ptr1 = NULL;
            peer_buff_struct *peer_buff_ptr2 = NULL;
            peer_buff_struct *peer_buff_ptr3 = NULL;
            peer_buff_struct *peer_buff_ptr4 = NULL;
            peer_buff_struct *peer_buff_ptr5 = NULL;
            peer_buff_struct *peer_buff_ptr6 = NULL;
            peer_buff_struct *peer_buff_ptr7 = NULL;
            peer_buff_struct *peer_buff_ptr8 = NULL;
            peer_buff_struct *peer_buff_ptr9 = NULL;
            gprs_flc_activate_context(TPDU_DATA,
                                      TCP_OVER_WIFI,
                                      GPRS_FLC_UL,
                                      0);

            peer_buff_ptr1 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr2 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr3 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr4 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr5 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr6 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr7 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr8 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);
            peer_buff_ptr9 = gprs_flc_get_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, 0, 0, 1500, NULL);

            //gprs_flc_grow_context_memory(GPRS_UNACK_DATA, GPRS_INVALID_SAPI, GPRS_FLC_UL, 5, (22*1024),0,
            //    &allocated_ul_size, &allocated_dl_size);

            gprs_flc_free_peer_buff(TPDU_DATA, TCP_OVER_WIFI, GPRS_FLC_UL, 0, peer_buff_ptr1);
        }
#endif
        supc_init_req_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_DEINIT_REQ) { //case MSG_ID_SUPC_ABM_WIFI_DEINIT_REQ:
        supc_deinit_req_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_INIT_CNF) { //case MSG_ID_WNDRV_SUPC_INIT_CNF:
        supc_init_cnf_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_DEINIT_CNF) { //case MSG_ID_WNDRV_SUPC_DEINIT_CNF:
        /* Do nothing now */
        wpa_printf(TRACE_GROUP_1, "no handler for this msg\n");
//          kal_trace(DBG_INIT,INFO_SUPC_NO_HANDLER_IN_DEINIT);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_IND) { //case MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_IND :
        supc_network_connect_ind_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND) { //case MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND :
        supc_network_connect_fail_ind_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND) { //case MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND :
        supc_network_disconnect_ind_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_NETWORK_STATISTICS_IND) { //case MSG_ID_WNDRV_SUPC_NETWORK_STATISTICS_IND:
//            supc_network_statistics_ind_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND) { //case MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND :
        supc_site_survey_ind_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_PMKID_CANDIDATE_IND) { //case MSG_ID_WNDRV_SUPC_PMKID_CANDIDATE_IND :
        /*MAUI_03055418 create a common ssl context used by eap.globalCtx */
        if (wpa_s->support_pre_auth) {
            supc_pmkid_candidate_ind_handler(ilm_ptr);
        }
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_STATUS_IND) { //case MSG_ID_WNDRV_SUPC_STATUS_IND:
        supc_status_ind_handler(ilm_ptr);
        //break;
    } else if (ilm_ptr->msg_id == MSG_ID_WNDRV_SUPC_QUERY_CAPABILITY_IND) { //case MSG_ID_WNDRV_SUPC_QUERY_CAPABILITY_IND:
        supc_capbility_ind_handler(ilm_ptr);
        //break;
    }
    //  MSGs from ABM
    //case MSG_ID_SUPC_ABM_WIFI_CONNECT_REQ:

    else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_CONNECT_REQ) { //case MSG_ID_SUPC_ABM_WIFI_CONNECT_REQ:
        supc_abm_wifi_connect_req_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_DISCONNECT_REQ) { //case MSG_ID_SUPC_ABM_WIFI_DISCONNECT_REQ:
        supc_abm_wifi_disconnect_req_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_AP_LIST_REQ) { //case MSG_ID_SUPC_ABM_WIFI_AP_LIST_REQ:
        supc_abm_wifi_ap_list_req_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_REQ) {
        supc_abm_wifi_get_connection_info_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_SUPC_ABM_WIFI_WPS_GET_CREDENTIAL_REQ) {
        supc_abm_wifi_get_wps_credential_req_handler(ilm_ptr);
    }
#if 0//#ifdef __CERTMAN_SUPPORT__
    else if (ilm_ptr->msg_id == MSG_ID_CERTMAN_GET_CERT_BY_ID_CNF) { //case MSG_ID_CERTMAN_GET_CERT_BY_ID_CNF:
        supc_get_cert_by_id_cnf_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_CERTMAN_GET_PRIVKEY_CNF) { //case MSG_ID_CERTMAN_GET_PRIVKEY_CNF:
        supc_get_privkey_cnf_handler(ilm_ptr);
        //break;
    }

    else if (ilm_ptr->msg_id == MSG_ID_CERTMAN_GET_CERT_CHAIN_CNF) { //case MSG_ID_CERTMAN_GET_CERT_CHAIN_CNF:
        supc_get_cert_chain_cnf_handler(ilm_ptr);
        //break;
    }
#endif
    // MSGs from SIM


    // MSGs from MMI

    //default:
    //{
    //}
    //}

    return;
}
