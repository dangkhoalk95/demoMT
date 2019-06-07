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


#include <stdlib.h>

//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
//#include "tst_def.h"
#include "stack_ltlcom.h"
//#include "app_ltlcom.h"

#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "stack_ltlcom.h"
#include "app_buff_alloc.h" /* get_ctrl_buff */
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
#include "config_ssid.h"
#include "wpa_supplicant.h"
#include "supc_config.h"
#include "l2_packet.h"
#include "wpa_supplicant_i.h"
#include "driver_mtk_i.h"

#include "supc_wpa_common.h"
#include "supc_wpa_functions.h"
//#include "supc_mmi_msgs.h"

#include "wndrv_supc_msg.h"

//#include "ps2sim_struct.h"
//#include "sim_common_enums.h"

//#include "fs_func.h"

//#include "Certman_struct.h"
//#include "Certman_api.h"


/* 04-20 2011 jim*/
//#include "ssl_api.h"
//#include "eap_tls_common.h"
//#include "supc_ssl.h"
#include "eap_i.h"
#include "wps_supplicant.h"

#if 0
/* For unittest, profiles */
#ifdef _SUPC_UNITTEST_
wifi_data_account_profile_struct default_profile = {"mtk01",  // ssid
                                                    6,    // ssid_len
                                                    1,    // scan_ssid
                                                    KAL_TRUE, // bssid_p
{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}, // bssid(mac)
1, // priority
WNDRV_SUPC_NETWORK_INFRASTRUCTURE, //network_type(wndrv_supc_network_type_enum
1, //max_rate
1, // proto ??TBD??
1, // key_mgmt ??TBD??
1, //pairwise ??TBD??
1, //group ??TBD??
KAL_FALSE, // psk_p
{},
1 //eap ??TBD??
                                                   }
#endif
#endif

#ifdef __WAPI_SUPPORT__
#include "interface_inout.h"
#endif
kal_uint32 scan_interval_table[] = {2, 2, 2, 2, 4, 4, 8, 16, 32, 64};   // secs
kal_uint32 max_scan_interval_index = (sizeof(scan_interval_table) / sizeof(kal_uint32) - 1);

void supc_disconnect_network(void);
extern kal_uint32 L1T_GetFN(void);
extern void supc_cb_wps_status(WPS_STATE stat);
#ifdef __WAPI_SUPPORT__
extern int wapi_init(struct wpa_supplicant *wpa_s,
                     const kal_int8 *ifname, const kal_uint8 *own_addr
                    );
#endif

typedef char            S8;
typedef unsigned short  U16;


//extern U16 mmi_asc_to_ucs2(S8 *pOutBuffer, S8 *pInBuffer );
//extern void supc_send_sim_read_imsi_mnc_req(void);

#if 0
kal_char *wndrv_get_manufacture_name(void);
kal_char *wndrv_get_dev_name(void);
kal_char *wndrv_get_model_name(void);
kal_char *wndrv_get_model_num(void);
kal_char *wndrv_get_serial_num(void);
#endif

/* global value
*/
extern kal_bool fgSiteSurveyReq; /* MAUI_02639824 */
kal_bool force_wps_pin = KAL_FALSE;
#if 0
void supc_tst_start_wps_connect_handler(ilm_struct *ilm_ptr)
{

    tst_inject_string_struct *tst = (tst_inject_string_struct *)(ilm_ptr->local_para_ptr);

    //wpa_printf(MSG_DEBUG, "supc_tst_start_wps_connect_handler");
    kal_trace(DBG_WPS, INFO_SUPC_WPS_TST_CONN_REQ);

    if (strcmp((char *)tst->string, "wps_pin") == 0) {
        force_wps_pin = KAL_TRUE;
        //wpa_printf(MSG_DEBUG, "force tu use wps_pin connection");
        kal_trace(DBG_WPS, INFO_SUPC_WPS_FORCE_USE_PIN);
    } else if (strcmp((char *)tst->string, "normal") == 0) {
        force_wps_pin = KAL_FALSE;
        //wpa_printf(MSG_DEBUG, "use profile's connection type");
        kal_trace(DBG_WPS, INFO_SUPC_WPS_TST_NORMAL);
    } else {
        supc_abm_wifi_connect_req_handler(ilm_ptr);
    }
    return;
}

void supc_tst_start_wps_disconn_handler(ilm_struct *ilm_ptr)
{
    //wpa_printf(MSG_DEBUG, "inject disconnect !");
    kal_trace(DBG_WPS, INFO_SUPC_WPS_TST_DISCONN);
    supc_abm_wifi_disconnect_req_handler(NULL);
    return;
}

void supc_tst_dbg(ilm_struct *ilm_ptr)
{
    tst_inject_string_struct *tst = (tst_inject_string_struct *)(ilm_ptr->local_para_ptr);
    INNER_TRACE_STEP(1);
    if (strcmp((char *)tst->string, "current_ssid") == 0) {
        supc_dbg_show_current_ssid();
    } else if (strcmp((char *)tst->string, "dump_conf")) {
        supc_dbg_dump_config();
    } else {
        INNER_TRACE_STEP(2);
        return;
    }
}

void supc_tst_switch_conn_type(ilm_struct *ilm_ptr)
{
    tst_inject_string_struct *tst = (tst_inject_string_struct *)(ilm_ptr->local_para_ptr);
    //wpa_printf(MSG_DEBUG, "supc_tst_switch_conn_type");
    kal_trace(DBG_WPS, INFO_SUPC_WPS_TST_SWITCH_TYPE);

    if (strcmp((char *)tst->string, "wps_pin") == 0) {
        force_wps_pin = KAL_TRUE;
        //wpa_printf(MSG_DEBUG, "force tu use wps_pin connection");
        kal_trace(DBG_WPS, INFO_SUPC_WPS_FORCE_USE_PIN);
    } else if (strcmp((char *)tst->string, "normal") == 0) {
        force_wps_pin = KAL_FALSE;
        //wpa_printf(MSG_DEBUG, "use profile's connection type");
        kal_trace(DBG_WPS, INFO_SUPC_WPS_TST_NORMAL);
    } else {
        //wpa_printf(MSG_DEBUG, "bad param");
        kal_trace(DBG_WPS, INFO_SUPC_WPS_TST_BAD_PARAM);
    }
}

void supc_tst_open_ssl_file_handler(ilm_struct *ilm_ptr)
{
    FS_HANDLE  cFile;
    UINT  fileSize = 0;
    WCHAR fileName[128];
    tst_inject_string_struct *tst = (tst_inject_string_struct *)ilm_ptr->local_para_ptr;

    mmi_asc_to_ucs2((S8 *) fileName, (S8 *)tst->string);


    cFile = FS_Open(fileName, FS_READ_ONLY);

    if (cFile == NULL) {
        supc_ssl_error("FS_Open", 0, 0);
    }

    FS_GetFileSize(cFile, &fileSize);

    if (fileSize == 0) {
        supc_ssl_error("FS_GetFileSize", 0, 0);
    }

}
#endif


/*********************************************************************
   Function Name    : supc_timer_expiry_hdlr
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct * ilm_ptr
   Description      :
*********************************************************************/
void supc_timer_expiry_hdlr(ilm_struct *ilm_ptr)
{
    //if (stack_is_time_out_valid(&(wpa_timer_context_g.wpa_base_timer)))
    //{
    /* Execute event's timeout handler */
    evshed_timer_handler(wpa_timer_context_g.wpa_event_scheduler_ptr);
    // }
    /* Shoulbe be paired with stack_is_time_out_valid() */
    //   stack_process_time_out(&(wpa_timer_context_g.wpa_base_timer));
}


/* Joseph:
 * Message handler of SUPC_MMI_WIFI_AP_LIST_REQ
 */

/*********************************************************************
   Function Name    : supc_abm_wifi_ap_list_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_abm_wifi_ap_list_req_handler(ilm_struct *ilm_ptr)
{
    /*MAUI_02964683, if supc is scanning, all scan req must fail immediately*/
    if (KAL_TRUE ==  wpa_s->is_mmi_req_ap_list) {
        wpa_printf(TRACE_GROUP_1, "SUPC is busy now, ap list request must be ignored\n");
//      kal_trace(DBG_SCAN,INFO_SUPC_SCAN_BUSY);
        supc_send_empty_ap_list_cnf();
        return;
    }

    wpa_s->is_mmi_req_ap_list = KAL_TRUE;
    wpa_s->num_scan_results = 0;
    wpa_s->scan_monitor_timer_id = supc_start_scan_monitor_timer(30);

#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
        wpa_s->chip_state = CHIP_POWER_ON;
        supc_send_wndrv_chip_poweron_req();
    }
#endif

    /*
     MAUI_03106036.  after disconnect with AP passively,
     the wpa_s->current_ssid is not NULL. if AP stated above is
     belonged to a hidden SSID, current_ssid point a hidden SSID.
     since AP_LIST_REQ require to scan non-hidden SSID, supc shouldn't
     to scan with a given SSID.
    */
    /* MAUI_03106036
     if (wpa_s->current_ssid != NULL)
         supc_send_wndrv_site_survey_req(wpa_s->current_ssid->ssid,
             wpa_s->current_ssid->ssid_len);

     else
     */
    supc_send_wndrv_site_survey_req(NULL, 0);
}

/*
 * Message handler of MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_REQ
 */

/*********************************************************************
   Function Name    : supc_abm_wifi_get_connection_info_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_abm_wifi_get_connection_info_req_handler(ilm_struct *ilm_ptr)
{
    supc_send_abm_connection_info_cnf();
}
/*
 * Message handler of MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_REQ
 */

/*********************************************************************
   Function Name    : supc_abm_wifi_get_wps_credential_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_abm_wifi_get_wps_credential_req_handler(ilm_struct *ilm_ptr)
{
    supc_send_abm_wps_get_credential_cnf();
}

/* Joseph:
 * Message handler of WNDRV_SUPC_DATA_IND
 */

/*********************************************************************
   Function Name    : supc_data_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_data_ind_handler(ilm_struct *ilm_ptr)
{
    struct l2_packet_data *l2 = wpa_s->l2;
    struct l2_packet_data *l2_preauth = wpa_s->l2_preauth;
    wndrv_supc_data_ind_struct *local_para_ptr = (wndrv_supc_data_ind_struct *)(ilm_ptr->local_para_ptr);
    kal_uint8 *pdu_ptr = NULL;
    wpa_size_t pdu_len = 0;

    INNER_TRACE_STEP(1);

    pdu_ptr = get_pdu_ptr((peer_buff_struct *)ilm_ptr->peer_buff_ptr, &pdu_len);


    if ((ilm_ptr->peer_buff_ptr == NULL) || (local_para_ptr == NULL)) {
        return;
    }

    if (local_para_ptr->proto_type == ETH_P_EAPOL) {
        if (l2 == NULL) {
            wpa_printf(TRACE_GROUP_1, "Ignore this eapol frame because the eapol state machine is deinit\n");
//           kal_trace(DBG_EAPOL_SM,INFO_SUPC_IGNORE_EAPOL_FRAME_WRONG_SM);
            return;
        }
        l2->rx_callback(l2->rx_callback_ctx, local_para_ptr->src_mac_addr, pdu_ptr, pdu_len);
    } else if (local_para_ptr->proto_type == ETH_P_RSN_PREAUTH) {
        if (l2_preauth == NULL) {
            wpa_printf(TRACE_GROUP_1, "Ignore this pre-auth frame because the pre-auth state machine is deinit\n");
//         kal_trace(DBG_EAPOL_SM,INFO_SUPC_IGNORE_PRE_AUTH_FRAME_WRONG_SM);
            return;
        }
        l2_preauth->rx_callback(l2_preauth->rx_callback_ctx, local_para_ptr->src_mac_addr, pdu_ptr, pdu_len);
    }
#ifdef __WAPI_SUPPORT__
    else if (local_para_ptr->proto_type == ETH_P_WAI) {
        if (wpa_s->l2_wapi == NULL) {
            //wpa_printf(MSG_DEBUG, "Ignore this WAPI frame because the pre-auth state machine is deinit");
            kal_trace(DBG_WAPI, INFO_SUPC_IGNORE_WAI_AUTH_FRAME_WRONG_SM);
            return;
        }

        //wpa_printf(MSG_DEBUG, "receive ETH_P_WAI len %d\n", pdu_len);
        kal_trace(DBG_WAPI, INFO_SUPC_RX_WAI_FRAME_LEN, pdu_len);
        wpa_s->l2_wapi->rx_callback(wpa_s->l2_wapi->rx_callback_ctx,
                                    local_para_ptr->src_mac_addr, pdu_ptr, pdu_len);
    }
#endif
    else {
        SUPC_ASSERT(0);
    }
}



/*********************************************************************
   Function Name    : supc_deinit_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_deinit_req_handler(ilm_struct *ilm_ptr)
{

    INNER_TRACE_STEP(1);
#if 1//#ifdef __CERTMAN_SUPPORT__
    wpa_s->rx_deinit_req = KAL_TRUE;

    wpa_printf(MSG_DEBUG, "pending_cert_req = %d, tx_deinit_cnf = %d\n", wpa_s->pending_cert_req, wpa_s->tx_deinit_cnf);
//  kal_trace(DBG_INIT,INFO_SUPC_PENDING_CERT_IN_DEINIT,wpa_s->pending_cert_req, wpa_s->tx_deinit_cnf);

    if ((wpa_s->pending_cert_req == 0) && (!(wpa_s->tx_deinit_cnf))) {
        supc_do_deinit_req();
        supc_send_abm_wifi_deinit_cnf();
        wpa_s->tx_deinit_cnf = KAL_TRUE;
    }
#else
    supc_do_deinit_req();
    supc_send_abm_wifi_deinit_cnf();
#endif

#ifdef _SUPC_CHECK_MEM_LEAK
    supc_check_mem_leak();
#endif

    return;
}

/* Joseph:
 * Message handler of WNDRV_SUPC_INIT_REQ
 */

/*********************************************************************
   Function Name    : supc_init_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_init_req_handler(ilm_struct *ilm_ptr)
{
    //struct eapol_ctx *ctx;


    //ctx = SUPC_ALLOC_BUFF(sizeof(*ctx));
    //kal_mem_set(ctx, 0, sizeof(*ctx));
    //ctx->ctx = wpa_s;
    //ctx->msg_ctx = wpa_s;
    //ctx->preauth = 0;
    //ctx->eapol_done_cb = wpa_supplicant_notify_eapol_done;
    //ctx->eapol_send = wpa_eapol_send;
    //ctx->set_wep_key = wpa_eapol_set_wep_key;


    //wpa_s->eapol = eapol_sm_init(ctx);

    /* RSNA Supplicant Key Management - INITIALIZE */
    //eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    //eapol_sm_notify_portValid(wpa_s->eapol, FALSE);

    //INNER_TRACE_STEP(1);
    wpa_s->need_to_send_init_cnf = KAL_FALSE;

#if 1//#ifdef __CERTMAN_SUPPORT__
    wpa_s->pending_cert_req = 0; //Kinki: init in the first time due to deinit_req handler
    wpa_s->rx_deinit_req = KAL_FALSE;
    wpa_s->tx_deinit_cnf = KAL_FALSE;
#endif
    supc_send_wndrv_init_req();

    return;

}

#if 0//#ifdef __CERTMAN_SUPPORT__
// Kinki:
/* Message handler of CERTMAN_GET_PRIVKEY_CNF
*/
/*********************************************************************
   Function Name    :  supc_get_privkey_cnf_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_get_privkey_cnf_handler(ilm_struct *ilm_ptr)
{
    /*Local Variable*/
    kal_bool match = KAL_FALSE;
    certman_get_privkey_cnf_struct *local_para_ptr = NULL;
    wpa_ssid *ssid = NULL;

    local_para_ptr = (certman_get_privkey_cnf_struct *)ilm_ptr->local_para_ptr;
    ssid = wpa_s->selected_ssid;

    /*Code Body*/

    if (ssid == NULL) {
        FS_Delete((const WCHAR *)local_para_ptr->filename);
        return;
    }

    if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
            (ssid->client_cert_id == local_para_ptr->cert_id)) { //phase 1
        wpa_s->pending_cert_req --;
    } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
               (ssid->client_cert2_id == local_para_ptr->cert_id)) {
        wpa_s->pending_cert_req --;
    }

    //Kinki: process disconnect_cnf or deinit_cnf until all certificate cnf in this session have been received
    if ((wpa_s->pending_cert_req == 0) && (wpa_s->rx_disconnect_req) && (!(wpa_s->tx_disconnect_cnf))) {
        supc_do_disconnect_req();
        supc_send_abm_wifi_disconnect_cnf();
        wpa_s->tx_disconnect_cnf = KAL_TRUE;
        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    } else if ((wpa_s->pending_cert_req == 0) && (wpa_s->rx_deinit_req) && (!(wpa_s->tx_deinit_cnf))) {
        supc_do_deinit_req();
        supc_send_abm_wifi_deinit_cnf();
        wpa_s->tx_deinit_cnf = KAL_TRUE;
        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    }


    if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
            (ssid->client_cert_id == local_para_ptr->cert_id)) { //phase 1

        //wpa_printf(MSG_DEBUG,"Get privkey(before dealloc): private_key :%x",ssid->private_key);
        kal_trace(DBG_CONN, INFO_SUPC_GET_PRIVATE_KEY_BEFORE, ssid->private_key);
        if (ssid->private_key != NULL) {
            FS_Delete((const WCHAR *)ssid->private_key);
            SUPC_DEALLOC_BUFF(ssid->private_key);
            ssid->private_key = NULL;
        }

        if (local_para_ptr->status == CERTMAN_ERR_NONE) {
            ssid->private_key = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
            if (ssid->private_key == NULL) {
                SUPC_ASSERT(0);
            }

            kal_mem_cpy(ssid->private_key, local_para_ptr->filename, sizeof(local_para_ptr->filename));
            //wpa_printf(MSG_DEBUG,"Get privkey(before dealloc): private_key :%x", ssid->private_key);
            kal_trace(DBG_CONN, INFO_SUPC_GET_PRIVATE_KEY_AFTER, ssid->private_key);
            match = KAL_TRUE;
        }
    } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
               (ssid->client_cert2_id == local_para_ptr->cert_id)) { //phase 2

        //wpa_printf(MSG_DEBUG,"Get privkey2(before dealloc): private_key2 :%x",ssid->private_key2);
        kal_trace(DBG_CONN, INFO_SUPC_GET_PRIVATE2_KEY_BEFORE, ssid->private_key2);
        if (ssid->private_key2 != NULL) {
            FS_Delete((const WCHAR *)ssid->private_key2);
            SUPC_DEALLOC_BUFF(ssid->private_key2);
            ssid->private_key2 = NULL;
        }

        if (local_para_ptr->status == CERTMAN_ERR_NONE) {
            ssid->private_key2 = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
            if (ssid->private_key2 == NULL) {
                SUPC_ASSERT(0);
            }

            kal_mem_cpy(ssid->private_key2, local_para_ptr->filename, sizeof(local_para_ptr->filename));
            //wpa_printf(MSG_DEBUG,"Get privkey2(before dealloc): private_key2 :%x", ssid->private_key2);
            kal_trace(DBG_CONN, INFO_SUPC_GET_PRIVATE2_KEY_BEFORE, ssid->private_key2);
            match = KAL_TRUE;
        }
    }

    if (match != KAL_TRUE) {
        /*Kinki: to avoid deallocating the correct random_passwd.
                     It may occur when 2 cnf are arrival at the same time,
                     and the privkey_random_passwd or privkey2_random_passwd have been over-written in a correct value.
        */
        if (local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) {
            //wpa_printf(MSG_DEBUG,"ssid->privkey_random_passwd : %x", ssid->privkey_random_passwd);
            kal_trace(DBG_CONN, INFO_SUPC_PRIVKEY_RAND_PD, ssid->privkey_random_passwd);
            SUPC_DEALLOC_BUFF(ssid->privkey_random_passwd);
            ssid->privkey_random_passwd = NULL;
        } else if (local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) {
            //wpa_printf(MSG_DEBUG,"ssid->privkey2_random_passwd : %x", ssid->privkey2_random_passwd);
            kal_trace(DBG_CONN, INFO_SUPC_PRIVKEY2_RAND_PD, ssid->privkey2_random_passwd);
            SUPC_DEALLOC_BUFF(ssid->privkey2_random_passwd);
            ssid->privkey2_random_passwd = NULL;
        }

        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    }

    if (wpa_s->pending_cert_req == 0) {
        ssid->get_all_cert_or_not = KAL_TRUE;
        wpa_supplicant_associate(wpa_s, wpa_s->selected_bssid, wpa_s->selected_ssid);
    } else if (wpa_s->pending_cert_req < 0) {
        SUPC_ASSERT(0);
    }

    return;
}

// Kinki:
/* Message handler of CERTMAN_SUPC_GET_CERT_BY_ID_CNF
*/
/*********************************************************************
   Function Name    :  supc_get_cert_by_id_cnf_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_get_cert_by_id_cnf_handler(ilm_struct *ilm_ptr)
{
    /*Local Variable*/
    kal_bool match = KAL_FALSE;
    certman_get_cert_by_id_cnf_struct *local_para_ptr = NULL;
    wpa_ssid *ssid = NULL;

    local_para_ptr = (certman_get_cert_by_id_cnf_struct *)ilm_ptr->local_para_ptr;
    ssid = wpa_s->selected_ssid;

    /*Code Body*/
    if (ssid == NULL) {
        FS_Delete((const WCHAR *)local_para_ptr->filename);
        return;
    }

#ifdef __WAPI_SUPPORT__
    //wpa_printf(MSG_DEBUG, "supc_get_cert_by_id_cnf_handler return %d\n", local_para_ptr->cert_id);
    kal_trace(DBG_WAPI, INFO_SUPC_GET_WAPI_CERT_ID_CNF, local_para_ptr->cert_id);


    /* if WAPI is used, certificate id is for client(user) and client2(AS) */
    if (ssid->proto == WPA_PROTO_WAPI) {
        if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
                (ssid->client_cert_id == local_para_ptr->cert_id)) {
            //wpa_printf(MSG_DEBUG, "supc_get_cert_by_id_cnf_handler found client_cert_id %d\n", ssid->client_cert_id);
            kal_trace(DBG_WAPI, INFO_SUPC_GET_WAPI_CLIENT_CERT_ID_CNF, ssid->client_cert_id);
            wpa_s->pending_cert_req --;
        } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
                   (ssid->client_cert2_id == local_para_ptr->cert_id)) {
            //wpa_printf(MSG_DEBUG, "supc_get_cert_by_id_cnf_handler found client_cert2_id %d\n", ssid->client_cert2_id);
            kal_trace(DBG_WAPI, INFO_SUPC_GET_WAPI_CLIENT_CERT2_ID_CNF, ssid->client_cert2_id);
            wpa_s->pending_cert_req --;
        }
    } else {
        /* For EAP methods, get_cert_by_id_cnf is only used by CA */
        if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
                (ssid->ca_cert_id == local_para_ptr->cert_id)) {
            wpa_s->pending_cert_req --;
        } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
                   (ssid->ca_cert2_id == local_para_ptr->cert_id)) {
            wpa_s->pending_cert_req --;
        }
    }
#else
    if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
            (ssid->ca_cert_id == local_para_ptr->cert_id)) {
        wpa_s->pending_cert_req --;
    } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
               (ssid->ca_cert2_id == local_para_ptr->cert_id)) {
        wpa_s->pending_cert_req --;
    }
#endif

    if ((wpa_s->pending_cert_req == 0) && (wpa_s->rx_disconnect_req) && (!(wpa_s->tx_disconnect_cnf))) {
        supc_do_disconnect_req();
        supc_send_abm_wifi_disconnect_cnf();
        wpa_s->tx_disconnect_cnf = KAL_TRUE;
        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    } else if ((wpa_s->pending_cert_req == 0) && (wpa_s->rx_deinit_req) && (!(wpa_s->tx_deinit_cnf))) {
        supc_do_deinit_req();
        supc_send_abm_wifi_deinit_cnf();
        wpa_s->tx_deinit_cnf = KAL_TRUE;
        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    }

    //map cert path to cert_id into ssid
#ifdef __WAPI_SUPPORT__
    //wpa_printf(MSG_DEBUG, "supc_get_cert_by_id_cnf_handler return %d\n", local_para_ptr->cert_id);

    /* if WAPI is used, certificate id is for client(user) and client2(AS) */
    if (ssid->proto == WPA_PROTO_WAPI) {
        /* WAPI only use get by ID to get certificate */
        if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
                (ssid->client_cert_id == local_para_ptr->cert_id)) {
            if (ssid->client_cert != NULL) {
                FS_Delete((const WCHAR *)ssid->client_cert);
                SUPC_DEALLOC_BUFF(ssid->client_cert);
                ssid->client_cert = NULL;
            }

            if (local_para_ptr->status == CERTMAN_ERR_NONE) {
                ssid->client_cert = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
                if (ssid->client_cert == NULL) {
                    SUPC_ASSERT(0);
                }
                kal_mem_cpy(ssid->client_cert, local_para_ptr->filename, sizeof(local_para_ptr->filename));
                match = KAL_TRUE;
                //wpa_printf(MSG_DEBUG, "supc_get_cert_by_id_cnf_handler client_cert file name %s\n",
                //ssid->client_cert);
                kal_trace(DBG_WAPI, INFO_SUPC_GET_WAPI_CLIENT_CERT_FILE_NAME);
                kal_wap_trace(MAP_MOD_SUPC, DBG_WAPI, "%s", ssid->client_cert);
            }
        } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
                   (ssid->client_cert2_id == local_para_ptr->cert_id)) {

            if (ssid->client_cert2 != NULL) {
                FS_Delete((const WCHAR *)ssid->client_cert2);
                SUPC_DEALLOC_BUFF(ssid->client_cert2);
                ssid->client_cert2 = NULL;
            }

            if (local_para_ptr->status == CERTMAN_ERR_NONE) {
                ssid->client_cert2 = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
                if (ssid->client_cert2 == NULL) {
                    SUPC_ASSERT(0);
                }
                kal_mem_cpy(ssid->client_cert2, local_para_ptr->filename, sizeof(local_para_ptr->filename));
                match = KAL_TRUE;
                //wpa_printf(MSG_DEBUG, "supc_get_cert_by_id_cnf_handler client_cert file name %s\n",
                //ssid->client_cert2);
                kal_trace(DBG_WAPI, INFO_SUPC_GET_WAPI_CLIENT_CERT2_FILE_NAME);
                kal_wap_trace(MAP_MOD_SUPC, DBG_WAPI, "%s", ssid->client_cert2);
            }
        }
    } else {
        if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
                (ssid->ca_cert_id == local_para_ptr->cert_id)) {

            if (ssid->ca_cert != NULL) {
                FS_Delete((const WCHAR *)ssid->ca_cert);
                SUPC_DEALLOC_BUFF(ssid->ca_cert);
                ssid->ca_cert = NULL;
            }

            if (local_para_ptr->status == CERTMAN_ERR_NONE) {
                ssid->ca_cert = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
                if (ssid->ca_cert == NULL) {
                    SUPC_ASSERT(0);
                }
                kal_mem_cpy(ssid->ca_cert, local_para_ptr->filename, sizeof(local_para_ptr->filename));
                match = KAL_TRUE;
            }
        } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
                   (ssid->ca_cert2_id == local_para_ptr->cert_id)) {

            if (ssid->ca_cert2 != NULL) {
                FS_Delete((const WCHAR *)ssid->ca_cert2);
                SUPC_DEALLOC_BUFF(ssid->ca_cert2);
                ssid->ca_cert2 = NULL;
            }

            if (local_para_ptr->status == CERTMAN_ERR_NONE) {
                ssid->ca_cert2 = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
                if (ssid->ca_cert2 == NULL) {
                    SUPC_ASSERT(0);
                }
                kal_mem_cpy(ssid->ca_cert2, local_para_ptr->filename, sizeof(local_para_ptr->filename));
                match = KAL_TRUE;
            }
        }
    }
#else
    if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
            (ssid->ca_cert_id == local_para_ptr->cert_id)) {

        if (ssid->ca_cert != NULL) {
            FS_Delete((const WCHAR *)ssid->ca_cert);
            SUPC_DEALLOC_BUFF(ssid->ca_cert);
            ssid->ca_cert = NULL;
        }

        if (local_para_ptr->status == CERTMAN_ERR_NONE) {
            ssid->ca_cert = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
            if (ssid->ca_cert == NULL) {
                SUPC_ASSERT(0);
            }
            kal_mem_cpy(ssid->ca_cert, local_para_ptr->filename, sizeof(local_para_ptr->filename));
            match = KAL_TRUE;
        }
    } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
               (ssid->ca_cert2_id == local_para_ptr->cert_id)) {

        if (ssid->ca_cert2 != NULL) {
            FS_Delete((const WCHAR *)ssid->ca_cert2);
            SUPC_DEALLOC_BUFF(ssid->ca_cert2);
            ssid->ca_cert2 = NULL;
        }

        if (local_para_ptr->status == CERTMAN_ERR_NONE) {
            ssid->ca_cert2 = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
            if (ssid->ca_cert2 == NULL) {
                SUPC_ASSERT(0);
            }
            kal_mem_cpy(ssid->ca_cert2, local_para_ptr->filename, sizeof(local_para_ptr->filename));
            match = KAL_TRUE;
        }
    }
#endif

    if (match != KAL_TRUE) {
        FS_Delete((const WCHAR *)local_para_ptr->filename);
        return;
    }

    if (wpa_s->pending_cert_req == 0) { // Kinki: eap-peap/eap-ttls may have CA cert only
        ssid->get_all_cert_or_not = KAL_TRUE;
        wpa_supplicant_associate(wpa_s, wpa_s->selected_bssid, wpa_s->selected_ssid);
    } else if (wpa_s->pending_cert_req < 0) {
        SUPC_ASSERT(0);
    }

    return;
}

/* Message handler of CERTMAN_SUPC_GET_CERT_CHAIN_CNF
*/
/*********************************************************************
   Function Name    :  supc_get_cert_chain_cnf_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_get_cert_chain_cnf_handler(ilm_struct *ilm_ptr)
{
    /*Local Variable*/
    kal_bool match = KAL_FALSE;
    certman_get_cert_chain_cnf_struct *local_para_ptr = NULL;
    wpa_ssid *ssid = NULL;

    local_para_ptr = (certman_get_cert_chain_cnf_struct *)ilm_ptr->local_para_ptr;
    ssid = wpa_s->selected_ssid;

    /*Code Body*/
    if (ssid == NULL) {
        FS_Delete((const WCHAR *)local_para_ptr->filename);
        return;
    }

#ifdef __WAPI_SUPPORT__
    SUPC_ASSERT((ssid->proto & WPA_PROTO_WAPI) == 0);
#endif

    if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
            (ssid->client_cert_id == local_para_ptr->cert_id)) { //phase 1
        wpa_s->pending_cert_req --;
    } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
               (ssid->client_cert2_id == local_para_ptr->cert_id)) { //phase 2
        wpa_s->pending_cert_req --;
    }

    if ((wpa_s->pending_cert_req == 0) && (wpa_s->rx_disconnect_req) && (!(wpa_s->tx_disconnect_cnf))) {
        supc_do_disconnect_req();
        supc_send_abm_wifi_disconnect_cnf();
        wpa_s->tx_disconnect_cnf = KAL_TRUE;
        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    } else if ((wpa_s->pending_cert_req == 0) && (wpa_s->rx_deinit_req) && (!(wpa_s->tx_deinit_cnf))) {
        supc_do_deinit_req();
        supc_send_abm_wifi_deinit_cnf();
        wpa_s->tx_deinit_cnf = KAL_TRUE;
        FS_Delete((const WCHAR *)local_para_ptr->filename);

        return;
    }

    if ((local_para_ptr->trans_id == wpa_s->current_phase1_trans_id) &&
            (ssid->client_cert_id == local_para_ptr->cert_id)) { //phase 1

        if (ssid->client_cert != NULL) {
            //wpa_printf(MSG_DEBUG,"ssid->client_cert (before DEALLOC): %x", ssid->client_cert);
            kal_trace(DBG_CONN, INFO_SUPC_CLIENT_CERT_BEFORE_DEALLOC, ssid->client_cert);
            FS_Delete((const WCHAR *)ssid->client_cert);
            SUPC_DEALLOC_BUFF(ssid->client_cert);
            ssid->client_cert = NULL;
        }

        if (local_para_ptr->status == CERTMAN_ERR_NONE) {
            //wpa_printf(MSG_DEBUG,"ssid->client_cert (before ALLOC): %x", ssid->client_cert);
            kal_trace(DBG_CONN, INFO_SUPC_CLIENT_CERT_BEFORE_ALLOC, ssid->client_cert);
            ssid->client_cert = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
            if (ssid->client_cert == NULL) {
                SUPC_ASSERT(0);
            }

            kal_mem_cpy(ssid->client_cert, local_para_ptr->filename, sizeof(local_para_ptr->filename));
            //wpa_printf(MSG_DEBUG,"ssid->client_cert (after ALLOC): %x", ssid->client_cert);
            kal_trace(DBG_CONN, INFO_SUPC_CLIENT_CERT_AFTER_ALLOC, ssid->client_cert);
            match = KAL_TRUE;
        }
    } else if ((local_para_ptr->trans_id == wpa_s->current_phase2_trans_id) &&
               (ssid->client_cert2_id == local_para_ptr->cert_id)) { //phase 2

        if (ssid->client_cert2 != NULL) {
            //wpa_printf(MSG_DEBUG,"ssid->client_cert2 (before DEALLOC): %x", ssid->client_cert2);
            kal_trace(DBG_CONN, INFO_SUPC_CLIENT_CERT2_BEFORE_DEALLOC, ssid->client_cert2);
            FS_Delete((const WCHAR *)ssid->client_cert2);
            SUPC_DEALLOC_BUFF(ssid->client_cert2);
            ssid->client_cert2 = NULL;
        }

        if (local_para_ptr->status == CERTMAN_ERR_NONE) {
            //wpa_printf(MSG_DEBUG,"ssid->client_cert2 (before ALLOC): %x", ssid->client_cert2);
            kal_trace(DBG_CONN, INFO_SUPC_CLIENT_CERT2_BEFORE_ALLOC, ssid->client_cert2);
            ssid->client_cert2 = SUPC_ALLOC_BUFF(sizeof(local_para_ptr->filename));
            if (ssid->client_cert2 == NULL) {
                SUPC_ASSERT(0);
            }

            kal_mem_cpy(ssid->client_cert2, local_para_ptr->filename, sizeof(local_para_ptr->filename));
            //wpa_printf(MSG_DEBUG,"ssid->client_cert2 (after ALLOC): %x", ssid->client_cert2);
            kal_trace(DBG_CONN, INFO_SUPC_CLIENT_CERT2_AFTER_ALLOC, ssid->client_cert2);
            match = KAL_TRUE;
        }
    }

    if (match != KAL_TRUE) {
        FS_Delete((const WCHAR *)local_para_ptr->filename);
        return;
    }

    if (wpa_s->pending_cert_req == 0) { // Kinki: eap-peap/eap-ttls may have CA cert only
        ssid->get_all_cert_or_not = KAL_TRUE;
        wpa_supplicant_associate(wpa_s, wpa_s->selected_bssid, wpa_s->selected_ssid);
    } else if (wpa_s->pending_cert_req < 0) {
        SUPC_ASSERT(0);
    }

    return;
}
#endif

/* Joseph:
 * Message handler of WNDRV_SUPC_INIT_CNF
 */

/*********************************************************************
   Function Name    : supc_init_cnf_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_init_cnf_handler(ilm_struct *ilm_ptr)
{
    kal_uint32 srand_seed = 0;
    wndrv_supc_init_cnf_struct  *local_para_ptr = NULL;
    int p, temp = 0;
    static int count = 0;
    kal_bool sim_status = KAL_FALSE;

    // INNER_TRACE_STEP(1);

    local_para_ptr = (wndrv_supc_init_cnf_struct *)ilm_ptr->local_para_ptr;

    wpa_s->wpa_state = WPA_DISCONNECTED; // Kink add

    wpa_s->pmksa = NULL;
    /* Initialize driver interface and register driver event handler before
     * L2 receive handler so that association events are processed before
     * EAPOL-Key packets if both become available for the same select()
     * call. */
    wpa_s->drv_priv = wpa_driver_mtk_init(wpa_s, wpa_s->ifname, &local_para_ptr->capability);

    wpa_s->renew_snonce = 1;

    //from wpa_supplicant_driver_init
    wpa_s->l2 = l2_packet_init(wpa_s->ifname,
                               local_para_ptr->mac_addr,
                               ETH_P_EAPOL,
                               wpa_supplicant_rx_eapol, wpa_s);

    if (l2_packet_get_own_addr(wpa_s->l2, wpa_s->own_addr)) {
        //fprintf(stderr, "Failed to get own L2 address\n");
        SUPC_ASSERT(0);
    }

    wpa_printf(MSG_DEBUG, "Own MAC address: " MACSTR,
               MAC2STR(wpa_s->own_addr));
//  kal_buffer_trace(DBG_INIT,DUMP_SUPC_MAC_AADR,6,wpa_s->own_addr);

    for (p = 0; p < ETH_ALEN; p++) {
        temp += (unsigned int)wpa_s->own_addr[p];
    }

    if (count == 0) {
        kal_get_time(&srand_seed);
//      srand(L1T_GetFN() + temp);
        srand(srand_seed + temp);
//      wpa_printf(TRACE_GROUP_1, "temp: %d, L1T_GETFN(): %d, L1T_GetFN()+temp:%d \n", temp, L1T_GetFN(), L1T_GetFN()+temp);
//      kal_trace(DBG_INIT, DUMP_SUPC_SRAND, temp, L1T_GetFN(), L1T_GetFN()+temp);
        count++;
    }
#ifdef __WAPI_SUPPORT__
    if (wapi_init(wpa_s, (const kal_int8 *)wpa_s->ifname, (const kal_uint8 *)local_para_ptr->mac_addr) < 0) {
        SUPC_ASSERT(0);
    }
#endif


    /* Joseph:
     * Send the local mac address within the message SUPC_MMI_WIFI_INIT_IND to mmi
     */
    /* Joseph:
     * Move the sending SUPC_MMI_WIFI_INIT_IND to mmi after the recipt of SIM_READ_CNF
     * for IMSI information
     */
    //supc_send_mmi_wifi_init_ind();

    /* J: wpa_driver_mtk_set_wpa is not implemented. Remart it */
    /*
    if (wpa_drv_set_wpa(wpa_s, 1) < 0) {
        fprintf(stderr, "Failed to enable WPA in the driver.\n");
        SUPC_ASSERT(0);
    }
    */

    //wpa_clear_keys(wpa_s, NULL);

    /* Make sure that TKIP countermeasures are not left enabled (could
     * happen if wpa_supplicant is killed during countermeasures. */
    /* Joseph
     * Both of these two function are not implemented for MTK solution yet, remark it
     */
    //wpa_driver_mtk_set_countermeasures(wpa_s, 0);

    //wpa_driver_mtk_set_drop_unencrypted(wpa_s, 1);

    wpa_s->prev_scan_ssid = BROADCAST_SSID_SCAN;

    // supc_send_sim_read_imsi_req(); // Removed by Karen

//    sim_status = supc_check_sim_status();
    sim_status = KAL_TRUE;
#ifdef __MTK_TARGET__
    if ((wpa_s->imsi_ready) ||  // If SIM is inserted
            sim_status
#if 0
#ifdef __GEMINI__ //[MAUI_01629798] mtk01612: kinki_gemini_wifi
            ((check_sim_card_status(SIM1) != NORMAL_SIM_PRESENCE) && (check_sim_card_status(SIM1) != TEST_SIM_PRESENCE))
#else
            ((check_sim_card_status() != NORMAL_SIM_PRESENCE) && (check_sim_card_status() != TEST_SIM_PRESENCE))
#endif
#endif
       ) { // If SIM is not inserted(SIM_CARD_NOT_INSERTED) or has problem(SIM_NOT_READY)
        supc_send_abm_wifi_init_cnf();
        wpa_s->need_to_send_init_cnf = KAL_FALSE;
    } else {
        wpa_s->need_to_send_init_cnf = KAL_TRUE;
    }
#else
    supc_send_abm_wifi_init_cnf();
    wpa_s->need_to_send_init_cnf = KAL_FALSE;
#endif
    return;

}
#if 0
void supc_network_statistics_ind_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_network_statistics_ind_struct     *rcvd_local_para_ptr = NULL;
    supc_mmi_wifi_curr_ap_info_ind_struct        *send_local_para_ptr = NULL;

    INNER_TRACE_STEP(1);
    /* Not to report statistics if the WiFi service is not initialized */
    if (wpa_s->wpa_state == WPA_DEINITED) {
        return;
    }

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        if (wpa_s->wpa_state != WPA_ASSOCIATED) {
            return;
        }
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X ||
               wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
        if (wpa_s->wpa_state != WPA_COMPLETED) {
            return;
        }
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
        if (wpa_s->wpa_state != WPA_EAPOL_DONE) {
            return;
        }
    }
#ifdef __WAPI_SUPPORT__
    else if (wpa_s->key_mgmt == WAPI_KEY_MGMT_PSK ||
             wpa_s->key_mgmt == WAPI_KEY_MGMT_WAI) {
        if (wpa_s->wpa_state != WPA_COMPLETED) {
            return;
        }
    }
#endif

    rcvd_local_para_ptr = (wndrv_supc_network_statistics_ind_struct *)(ilm_ptr->local_para_ptr);

    //send_local_para_ptr = (supc_mmi_wifi_curr_ap_info_ind_struct *)construct_local_para(sizeof(supc_mmi_wifi_curr_ap_info_ind_struct), TD_UL);
    send_local_para_ptr = (supc_mmi_wifi_curr_ap_info_ind_struct *)construct_local_para(sizeof(supc_mmi_wifi_curr_ap_info_ind_struct), TD_UL);

    send_local_para_ptr->rssi = rcvd_local_para_ptr->statistics.rssi;
    send_local_para_ptr->current_data_rate = rcvd_local_para_ptr->statistics.current_data_rate;
    send_local_para_ptr->ht_current_data_rate = rcvd_local_para_ptr->statistics.ht_current_data_rate;
    supc_send_mmi_wifi_curr_ap_info_ind((local_para_struct *)send_local_para_ptr);

    return;
}
#endif
/* Joseph:
 * Message handler of WNDRV_SUPC_NETWORK_CONNECT_IND
 */

/*********************************************************************
   Function Name    : supc_network_connect_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_network_connect_ind_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_network_connect_ind_struct *revd_local_para_ptr = NULL;
    wndrv_supc_bss_info_struct *bss_info_ptr = NULL;
    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;
    supc_abm_wifi_connect_ind_struct *ind_local_para_ptr = NULL;
    wndrv_supc_auth_state_update_req_struct *req_local_para_ptr = NULL;
#ifdef WIFI_BLACKLIST
    int p;
#endif
    kal_bool fgChswitch = KAL_FALSE; /* MAUI_02928793 */

    INNER_TRACE_STEP(1);
    revd_local_para_ptr = (wndrv_supc_network_connect_ind_struct *)(ilm_ptr->local_para_ptr);
    bss_info_ptr = &(revd_local_para_ptr->bss_info);

    INNER_TRACE_STEP(1);

    supc_dbg_dump_supplicant();
    // Ignore this event if we receive it after user sends a disconnect request
    // This could happen since there is a 5ms time difference between disconnect
    // req from ABM and disjoin req to driver (for sending out the EAP-LOGOFF packet)
    // We may receive connect ind during this period of time.
    if (wpa_s->wpa_state < WPA_ASSOCIATING) {
        wpa_printf(TRACE_GROUP_1, "ignore it wpa_s state=%d\n", wpa_s->wpa_state);
//      kal_trace(DBG_CONN,INFO_SUPC_IGNORE_CONNECT_IND, wpa_s->wpa_state);
        return;
    }

    wpa_s->wpa_state = WPA_ASSOCIATED;
    wpa_printf(MSG_DEBUG, "Association event - clear replay counter\n");
//kal_trace(DBG_CONN,INFO_SUPC_ASSOC_EVENT_CLR_REPLAY_CNT);



    memset(wpa_s->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
    wpa_s->rx_replay_counter_set = 0;
    wpa_s->renew_snonce = 1;

    if (bss_info_ptr->ssid_len != 0) {
        kal_mem_cpy(wpa_s->ssid, bss_info_ptr->ssid, bss_info_ptr->ssid_len);
        wpa_s->ssid_len = bss_info_ptr->ssid_len;
        wpa_printf(TRACE_GROUP_1, "set wpa_s->ssid\n");
        // wpa_hexdump(MSG_DEBUG, "wpa_s->ssid", (char*)wpa_s->ssid, wpa_s->ssid_len);
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SSID_AFTER_CONN);
//      kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s,len:%d",wpa_s->ssid,wpa_s->ssid_len);
    } else {
        wpa_s->ssid_len = 0;
        wpa_printf(TRACE_GROUP_1, "connect to hiden wlan!\n");
//      kal_trace(DBG_CONN,INFO_SUPC_CONNECT_HIDEN_AP);
    }

    if (kal_mem_cmp(wpa_s->bssid, bss_info_ptr->bssid, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "Associated to a new BSS: "
                   "BSSID=" MACSTR "\n", MAC2STR(bss_info_ptr->bssid));
//      kal_trace(DBG_CONN,INFO_SUPC_CONNECT_TO_NEW_BSSID,MAC2STR(bss_info_ptr->bssid));
        kal_mem_cpy(wpa_s->bssid, bss_info_ptr->bssid, ETH_ALEN);

        wpa_s->last_michael_mic_error = 0;  // Karen: reset last michael error time if roam to a new AP
        /* Joseph:
         * if dynamic keys, associate keys with the bssid
         */
        //if (wpa_supplicant_dynamic_keys(wpa_s)) {
        //   wpa_clear_keys(wpa_s, bss_info_ptr->bssid);
        //}

        /*Joseph:
         * This function is not functioned inf the ap_scan is set 1. Remark it
         */
        //wpa_supplicant_select_config(wpa_s);
    }

    /* [MAUI_02928793] check if channel switch
    */
    if (wpa_s->channel_number != bss_info_ptr->channel_number) {
        fgChswitch = KAL_TRUE;
    }
    wpa_s->channel_number = bss_info_ptr->channel_number;

    wpa_printf(MSG_INFO, "Associated with " MACSTR ", ch=%d\n",
               MAC2STR(bss_info_ptr->bssid), bss_info_ptr->channel_number);
//  kal_trace(DBG_CONN,INFO_SUPC_CONNECT_WITH_BSSID_CHANNLE,MAC2STR(bss_info_ptr->bssid), bss_info_ptr->channel_number);


#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    //wpa_printf(MSG_DEBUG, "WPA: Add Blacklist from connect_ind");
    kal_trace(DBG_CONN, INFO_SUPC_ADD_BLACK_LIST_FROM_CONN_IND);

    //wpa_printf(MSG_DEBUG, "WPA: Blacklist BSSID Num = %d",revd_local_para_ptr->bssid_list_num);
    kal_trace(DBG_CONN, INFO_SUPC_BLACK_BSSID_NUM, revd_local_para_ptr->bssid_list_num);

    for (p = 0; p < revd_local_para_ptr->bssid_list_num && p < WNDRV_BSSID_MAX_NUM; p++) {
        //wpa_printf(MSG_DEBUG, "WPA: p = %d",p);
        kal_trace(DBG_CONN, INFO_SUPC_BLACK_LIST_NUM, p);

        if (wpa_listed_success(wpa_s, revd_local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id)) { // Kinki add for MAUI_00401655
            continue;
        }

        wpa_auth_timeout_list_del(wpa_s, revd_local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);

        if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE && revd_local_para_ptr->bssid_list[p].is_status_code_present && revd_local_para_ptr->bssid_list[p].status_code == 15) {
            wpa_blacklist_add_permanent(wpa_s, revd_local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
        } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK && revd_local_para_ptr->bssid_list[p].is_reason_code_present && revd_local_para_ptr->bssid_list[p].reason_code == 15) {
            wpa_blacklist_add_permanent(wpa_s, revd_local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
        } else {
            wpa_blacklist_add_temporal(wpa_s, revd_local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
        }

    }
    supc_blacklist_update(wpa_s);

#endif
    /* Set portEnabled first to FALSE in order to get EAP state
     * machine out of the SUCCESS state and eapSuccess cleared.
     * Without this, EAPOL PAE state machine may transit to
     * AUTHENTICATING state based on obsolete eapSuccess and then
     * trigger BE_AUTH to SUCCESS and PAE to AUTHENTICATED without
     * ever giving chance to EAP state machine to reset the state.
     */
    // copy WPA/RSN IE used in association request
    if (wpa_s->assoc_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
        wpa_s->assoc_wpa_ie = NULL;
        wpa_s->assoc_wpa_ie_len = 0;
    }

    if (wpa_s->ap_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
        wpa_s->ap_wpa_ie = NULL;
        wpa_s->ap_wpa_ie_len = 0;
    }

    if (wpa_s->ap_rsn_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
        wpa_s->ap_rsn_ie = NULL;
        wpa_s->ap_rsn_ie_len = 0;
    }

#ifdef __WAPI_SUPPORT__
    if (wpa_s->ap_wapi_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wapi_ie);
        wpa_s->ap_wapi_ie = NULL;
        wpa_s->ap_wapi_ie_len = 0;
    }
#endif

    wpa_s->assoc_wpa_ie_len = revd_local_para_ptr->assoc_ie_len;
    if (wpa_s->assoc_wpa_ie_len > 0) {
        wpa_s->assoc_wpa_ie = SUPC_ALLOC_BUFF(revd_local_para_ptr->assoc_ie_len);
        kal_mem_cpy(wpa_s->assoc_wpa_ie, revd_local_para_ptr->assoc_ie, revd_local_para_ptr->assoc_ie_len);
        wpa_find_assoc_pmkid(wpa_s);
        wpa_supplicant_suites_from_assoc_ie(wpa_s, wpa_s->assoc_wpa_ie, wpa_s->assoc_wpa_ie_len);
    }

    wpa_s->ap_wpa_ie_len = bss_info_ptr->wpa_ie_len;
    if (wpa_s->ap_wpa_ie_len > 0) {
        wpa_s->ap_wpa_ie = SUPC_ALLOC_BUFF(wpa_s->ap_wpa_ie_len);
        kal_mem_cpy(wpa_s->ap_wpa_ie, bss_info_ptr->wpa_ie, bss_info_ptr->wpa_ie_len);
    }

    wpa_s->ap_rsn_ie_len = bss_info_ptr->rsn_ie_len;
    if (wpa_s->ap_rsn_ie_len > 0) {
        wpa_s->ap_rsn_ie = SUPC_ALLOC_BUFF(wpa_s->ap_rsn_ie_len);
        kal_mem_cpy(wpa_s->ap_rsn_ie, bss_info_ptr->rsn_ie, bss_info_ptr->rsn_ie_len);
    }

#ifdef __WAPI_SUPPORT__

    wpa_s->ap_wapi_ie_len = bss_info_ptr->wapi_ie_len;

    if (wpa_s->ap_wapi_ie_len > 0) {
        wpa_s->ap_wapi_ie = SUPC_ALLOC_BUFF(wpa_s->ap_wapi_ie_len);
        kal_mem_cpy(wpa_s->ap_wapi_ie, bss_info_ptr->wapi_ie, bss_info_ptr->wapi_ie_len);
    }
#endif
    /*
    while roaming, supc may receive connect_ind anytime.
    as long as wndrv associate with AP, SUPC should receive connect_ind, and need to try authen again.
    since total time used to connect with wlan SSID is limited, nomatter how many times connection occurs,
    handover timer keeps the total consuming time is constant. if handover timer expires, it means
    this SSID is assumed invalid.
    */
    if (wpa_s->handover_timer_event_id == NULL) {
        wpa_s->handover_timer_event_id = supc_start_wpa_handover_timer(wpa_s->handover_timer_value);
    }
#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);

    supc_dbg_dump_supplicant();

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK ||
            (wpa_s->key_mgmt == WPA_KEY_MGMT_WPS
             && wpa_s->current_ssid->key_mgmt == WPA_KEY_MGMT_PSK)) {
        wpa_printf(TRACE_GROUP_1, "notify eapol EAP-FAIL\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_EAPOL_FAIL_WHEN_CONN);
        eapol_sm_notify_eap_success(wpa_s->eapol, FALSE);
    }
    /* 802.1X::portControl = Auto */
    eapol_sm_notify_portEnabled(wpa_s->eapol, TRUE);
    wpa_s->eapol_received = 0;

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {

        wpa_printf(TRACE_GROUP_1, "cancel handover timer if key mgmt none or wpa_none!!\n");
//      kal_trace(DBG_TIMER,INFO_SUPC_CANCEL_HANDOVER_TIMER_WHEN_CONN_IND);
        wpa_supplicant_cancel_auth_timeout(wpa_s);
        supc_stop_handover_timer();
#ifdef WIFI_PORT_TIMER
        supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif

        //req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
        req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_RESET);
        req_local_para_ptr->in_progress = KAL_FALSE;
        supc_send_wndrv_auth_state_update_req((local_para_struct *) req_local_para_ptr);

#ifdef __WPS_SUPPORT__
        /*MTK80707 wps open mode*/
        if (1 ==  wpa_s->wps_success ||
                KAL_TRUE == wpa_s->is_wpa_connect_req_pending)
#else

        if (KAL_TRUE == wpa_s->is_wpa_connect_req_pending)
#endif
        {
            //cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
            cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
            cnf_local_para_ptr->result = KAL_TRUE;
            cnf_local_para_ptr->cause = SUPC_ABM_INVALID_CAUSE;
            kal_mem_cpy(&(cnf_local_para_ptr->bssid[0]), &(bss_info_ptr->bssid[0]), WNDRV_MAC_ADDRESS_LEN);
            kal_mem_cpy(&(cnf_local_para_ptr->ssid[0]), &(bss_info_ptr->ssid[0]), bss_info_ptr->ssid_len);
            cnf_local_para_ptr->ssid_len = bss_info_ptr->ssid_len;
            cnf_local_para_ptr->channel_number = bss_info_ptr->channel_number;
            cnf_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;
#ifdef __WPS_SUPPORT__
            /*supc_get_auth_mode(wpa_s->key_mgmt,
                                    wpa_s->proto,
                                    wpa_s->current_ssid->auth_alg);
            */
            cnf_local_para_ptr->auth_type = SUPC_AUTH_MODE_OPEN;
            cnf_local_para_ptr->eap_peap_auth_type = 0;
            cnf_local_para_ptr->eap_ttls_auth_type = 0;
            /* if wps cred choose open mode, wpa_supplicant_wps_cred will ignore and
            does not to update ssid's cipher. so it is still DEFAULT_PAIRWISE.*/
            //supc_get_encrypt_mode(wpa_s->current_ssid->pairwise_cipher);
            cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;
            cnf_local_para_ptr->conn_type = wpa_s->connection_type;
            cnf_local_para_ptr->passphase_len = 0;

            if (wpa_s->current_ssid->passphrase) {
                kal_uint32 passlen = strlen(wpa_s->current_ssid->passphrase);
                wpa_printf(TRACE_GROUP_1, "WPS: store passphase len=%d\n", passlen);
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_STORE_PASSPHASE_LEN,passlen);
                if (passlen <= 29) {
                    kal_mem_cpy(cnf_local_para_ptr->passphase,
                                wpa_s->current_ssid->passphrase, passlen);
                    cnf_local_para_ptr->passphase_len = passlen;
                } else {
                    wpa_printf(MSG_DEBUG, "WPS: passphase exceed 30bytes\n");
//                   kal_trace(DBG_WPS,INFO_SUPC_WPS_PASSPHASE_EXCEED_30BYTES);
                }
            }
#else
            //add for WiFi UE enhancement
            cnf_local_para_ptr->auth_type          = (supc_auth_mode_enum)0;
            cnf_local_para_ptr->eap_peap_auth_type = 0;
            cnf_local_para_ptr->eap_ttls_auth_type = 0;

            /*clear wps data*/
            cnf_local_para_ptr->passphase_len = 0;
            cnf_local_para_ptr->conn_type = wpa_s->connection_type;
            cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;
#endif
            wpa_s->wpa_scan_retry_counter = 0xff;
            supc_stop_report_mmi_timer();
            wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
            supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);
        } else { //if (KAL_FALSE == wpa_s->is_wpa_connect_req_pending)
            if (kal_mem_cmp(wpa_s->bssid, wpa_s->prev_bssid, ETH_ALEN) != 0 ||
                    fgChswitch == KAL_TRUE) {
                //ind_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_ind_struct), TD_UL);
                ind_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_ind_struct), TD_RESET);
                kal_mem_cpy(&(ind_local_para_ptr->bssid[0]), &(bss_info_ptr->bssid[0]), WNDRV_MAC_ADDRESS_LEN);
                kal_mem_cpy(&(ind_local_para_ptr->ssid[0]), &(bss_info_ptr->ssid[0]), bss_info_ptr->ssid_len);
                ind_local_para_ptr->ssid_len = bss_info_ptr->ssid_len;
                ind_local_para_ptr->channel_number = bss_info_ptr->channel_number;
                ind_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;

                supc_send_abm_wifi_connect_ind((local_para_struct *) ind_local_para_ptr);
            }
        }


        kal_mem_cpy(wpa_s->prev_bssid, wpa_s->bssid, ETH_ALEN);
        wpa_s->num_of_disconnect = 0;
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
        wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
        wpa_listed_add_success(wpa_s, bss_info_ptr->bssid, wpa_s->current_ssid->profile_id); //Kinki add for MAUI_00401655
#endif

    }
#ifdef __WAPI_SUPPORT__
    else if (wpa_s->key_mgmt == WAPI_KEY_MGMT_WAI ||
             wpa_s->key_mgmt == WAPI_KEY_MGMT_PSK) {
        MAC_ADDRESS bssid_s;
        MAC_ADDRESS own_s;

        /* stop WPA and other time out use WAPI time only */
        wpa_supplicant_cancel_auth_timeout(wpa_s);
//      supc_stop_handover_timer();//need this for roaming failure handshake
#ifdef WIFI_PORT_TIMER
        supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif

        /* Notify WNDRV there is autentication underway */
        //req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
        req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
        req_local_para_ptr->in_progress = KAL_TRUE;
        supc_send_wndrv_auth_state_update_req((local_para_struct *) req_local_para_ptr);

        wpa_printf(TRACE_GROUP_1, "Own MAC address "MACSTR" "MACSTR"!!\n",
                   MAC2STR(wpa_s->bssid), MAC2STR(wpa_s->own_addr));
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_CONN_BSSID, MAC2STR(wpa_s->bssid), MAC2STR(wpa_s->own_addr));

        kal_mem_cpy(bssid_s.v, wpa_s->bssid, sizeof(bssid_s.v));
        kal_mem_cpy(own_s.v, wpa_s->own_addr, sizeof(own_s.v));
        SUPC_ASSERT(wpa_s->ap_wapi_ie_len);
        WAI_Msg_Input(CONN_ASSOC, &bssid_s, &own_s, wpa_s->ap_wapi_ie, wpa_s->ap_wapi_ie_len);

        /* leave deallocation here, cause WAI_Msg_Input need it */
        SUPC_DEALLOC_BUFF(wpa_s->ap_wapi_ie);
        wpa_s->ap_wapi_ie = NULL;
        wpa_s->ap_wapi_ie_len = 0;

        /* Start WAPI timer. Timer is invoked in WAI_Msg_Input */
//      wpa_supplicant_req_wapi_auth_timeout(wpa_s, 1, 0);
    }
#endif
    else {
        wpa_printf(TRACE_GROUP_1, "after assoc, restart auth timer\n");
//      kal_trace(DBG_TIMER,INFO_SUPC_RESTART_AUTH_TIMER_WHEN_ASSOC);
        //req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
        req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
        req_local_para_ptr->in_progress = KAL_TRUE;
        supc_send_wndrv_auth_state_update_req((local_para_struct *) req_local_para_ptr);

        wpa_supplicant_req_auth_timeout(wpa_s, AUTH_TIMEOUT_NOIEEE8021X, 0);
    }
}


/* Joseph:
 * Message handler of WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND
 */

/*********************************************************************
   Function Name    : supc_network_connect_fail_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_network_connect_fail_ind_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_network_connect_fail_ind_struct *local_para_ptr = NULL;
    //kal_uint32              session_id = 0;
    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;
#ifdef WIFI_BLACKLIST
    int p;
#endif
    INNER_TRACE_STEP(1);
    local_para_ptr = (wndrv_supc_network_connect_fail_ind_struct *)(ilm_ptr->local_para_ptr);
    //session_id = local_para_ptr->session_id;

    /* J, on the recipt of this message, do the following things
     * 1. report mmi about the connect fail. Two condition,
     *      a. manual mode for the first failure
     *      b. auto mode the for third failure
     * 2. start another scan
    */
    supc_stop_auth_timer(); // Karen

    /*mtk80707 prevent handover timeout handler send disjoin msg again.
    it will trigger supc disconnect handler*/
    supc_stop_handover_timer();

#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state != CHIP_POWER_OFF) { //Kinki add for power-saving
        wpa_s->chip_state = CHIP_POWER_OFF;
        supc_send_wndrv_chip_poweroff_req();
    }
#endif

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    //wpa_printf(MSG_DEBUG, "WPA: Add Blacklist from connect_fail_ind");
    //wpa_printf(MSG_DEBUG, "WPA: Blacklist BSSID Num = %d",local_para_ptr->bssid_list_num);

    kal_trace(DBG_CONN, INFO_SUPC_ADD_BLACK_LIST_FROM_CONN_FAIL);
    kal_trace(DBG_CONN, INFO_SUPC_BLACK_BSSID_NUM, local_para_ptr->bssid_list_num);

    for (p = 0; p < local_para_ptr->bssid_list_num && p < WNDRV_BSSID_MAX_NUM; p++) {
        //wpa_printf(MSG_DEBUG, "WPA: p = %d",p);
        kal_trace(DBG_CONN, INFO_SUPC_BLACK_LIST_NUM, p);

        if (wpa_listed_success(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id)) { // Kinki add for MAUI_00401655
            continue;
        }

        wpa_auth_timeout_list_del(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);

        if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE && local_para_ptr->bssid_list[p].is_status_code_present && local_para_ptr->bssid_list[p].status_code == 15) {
            wpa_blacklist_add_permanent(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
        } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK && local_para_ptr->bssid_list[p].is_reason_code_present && local_para_ptr->bssid_list[p].reason_code == 15) {
            wpa_blacklist_add_permanent(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
        } else {
            wpa_blacklist_add_temporal(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
        }

    }
    supc_blacklist_update(wpa_s);

#endif

    if (AP_MANUAL_SELECT == wpa_s->ap_selection_mode) {
        /* J, for the manual mode, this is the must-be condition */
        SUPC_ASSERT(KAL_FALSE == wpa_s->is_reported_mmi_fail);

        //cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
        cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_RESET);
        cnf_local_para_ptr->result = KAL_FALSE;
        cnf_local_para_ptr->cause = wpa_s->eap_fail_cause; //SUPC_ABM_CONNECT_FAIL;
        cnf_local_para_ptr->channel_number = 0;
        cnf_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;

        //add for WiFi UE enhancement
        cnf_local_para_ptr->auth_type          = (supc_auth_mode_enum)0;
        cnf_local_para_ptr->eap_peap_auth_type = 0;
        cnf_local_para_ptr->eap_ttls_auth_type = 0;

        /*clear wps data*/
        cnf_local_para_ptr->passphase_len = 0;
        cnf_local_para_ptr->conn_type = wpa_s->connection_type;
        cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

        wpa_s->is_reported_mmi_fail = KAL_TRUE;
        wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
        supc_stop_report_mmi_timer();
        supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);

        return;
    } else { /* AP_AUTOMATIC_SELECT mode */
        /*only if one profile is contained in wpa's conf list,
        supc can terminal MMI connection request after retry times exceed
        maxium value, in advance mmi_report_timer expire.*/
        if ((wpa_s->wpa_scan_retry_counter >= WPA_MAX_SCAN_RETRY) &&
                (KAL_FALSE == wpa_s->is_reported_mmi_fail) &&
                wpa_s->num_of_profile == 1) {
            //cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
            cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
            cnf_local_para_ptr->result = KAL_FALSE;
            cnf_local_para_ptr->cause = wpa_s->eap_fail_cause; //SUPC_ABM_CONNECT_FAIL;
            cnf_local_para_ptr->channel_number = 0;
            cnf_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;

            //add for WiFi UE enhancement
            cnf_local_para_ptr->auth_type          = (supc_auth_mode_enum)0;
            cnf_local_para_ptr->eap_peap_auth_type = 0;
            cnf_local_para_ptr->eap_ttls_auth_type = 0;

            /*clear wps data*/
            cnf_local_para_ptr->passphase_len = 0;
            cnf_local_para_ptr->conn_type = wpa_s->connection_type;
            cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

            wpa_s->wpa_scan_retry_counter = 0xff;
            wpa_s->is_reported_mmi_fail = KAL_TRUE;
            wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
            supc_stop_report_mmi_timer();
            supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);

            if (KAL_FALSE == supc_allow_autoconn()) {
                return;
            }
        }
    }

    /*if supc allow autoconn, wpa sm enter into scaning*/
    if (wpa_s->wpa_state == WPA_ASSOCIATING) {
        if (local_para_ptr->session_id == wpa_s->join_session_id) {
            wpa_s->num_scan_results = 0;
            // supc_send_wndrv_site_survey_req();
            wpa_s->wpa_state = WPA_SCANNING;    // Karen
            // do not do sitesurvey immediately since during countermeasure, we will keep receiving connect_fail_ind
            wpa_supplicant_req_scan(wpa_s, 2, 0);
        }
    }

    return;

}

/*********************************************************************
   Function Name    : supc_assoc_current_profile
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : none
   Description      : Associate with the current profile
*********************************************************************/
void supc_assoc_current_profile(void)
{
    wndrv_supc_bss_info_struct *selected = NULL;

    selected = SUPC_ALLOC_BUFF(sizeof(wndrv_supc_bss_info_struct));

    kal_mem_set(selected, 0, sizeof(wndrv_supc_bss_info_struct));
    kal_mem_cpy(selected->bssid, wpa_s->bssid, WNDRV_MAC_ADDRESS_LEN);
    selected->ssid_len = wpa_s->current_ssid->ssid_len;
    kal_mem_cpy(selected->ssid, wpa_s->current_ssid->ssid, selected->ssid_len);
    selected->channel_number = wpa_s->channel_number;
    selected->wpa_ie_len = wpa_s->ap_wpa_ie_len;
    kal_mem_cpy(selected->wpa_ie, wpa_s->ap_wpa_ie, wpa_s->ap_wpa_ie_len);
    selected->rsn_ie_len = wpa_s->ap_rsn_ie_len;
    kal_mem_cpy(selected->rsn_ie, wpa_s->ap_rsn_ie, wpa_s->ap_rsn_ie_len);

#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
        wpa_s->chip_state = CHIP_POWER_ON;
        supc_send_wndrv_chip_poweron_req();
    }
#endif
    wpa_supplicant_associate(wpa_s, selected, wpa_s->current_ssid);

    SUPC_DEALLOC_BUFF(selected);
}


/* Joseph:
 * Message handler of WNDRV_SUPC_NETWORK_DISCONNECT_IND
 */

/*********************************************************************
   Function Name    : supc_network_disconnect_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_network_disconnect_ind_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_network_disconnect_ind_struct *local_para_ptr = NULL;
#ifdef WIFI_BLACKLIST
    int p;
#endif
    local_para_ptr = (wndrv_supc_network_disconnect_ind_struct *)(ilm_ptr->local_para_ptr);
    INNER_TRACE_STEP(1);

    supc_dbg_dump_supplicant();

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        /* At least Host AP driver and a Prism3 card seemed to
         * be generating streams of disconnected events when
         * configuring IBSS for WPA-None. Ignore them for now.
         */
        wpa_printf(TRACE_GROUP_1, "Disconnect event - ignore in "
                   "IBSS/WPA-None mode\n");
//        kal_trace(DBG_CONN,INFO_SUPC_DISCONN_EVENT_IN_IBSS);
    }

    /* Joseph:
     * We'll add auto-select/manual-select to decide whether to start scaning at this time
     */
    wpa_printf(TRACE_GROUP_1, "wpa state = %d\n", wpa_s->wpa_state);
//  kal_trace(DBG_CONN,INFO_SUPC_WPA_STA_IN_DISCONN_EVENT,wpa_s->wpa_state);
    /*wpa_state implicate this disconn_ind is caused by request from supc or AP disconnection.*/
    if (wpa_s->wpa_state >= WPA_ASSOCIATED) {
        wpa_s->num_of_disconnect++;

#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->chip_state != CHIP_POWER_OFF) { //Kinki add for power-saving
            wpa_s->chip_state = CHIP_POWER_OFF;
            supc_send_wndrv_chip_poweroff_req();
        }
#endif


#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
        //wpa_printf(MSG_DEBUG, "WPA: Add Blacklist from disconnect_ind");
        //wpa_printf(MSG_DEBUG, "WPA: Blacklist BSSID Num = %d",local_para_ptr->bssid_list_num);

        kal_trace(DBG_CONN, INFO_SUPC_ADD_BLACK_LIST_FROM_DIS_CONN);
        kal_trace(DBG_CONN, INFO_SUPC_BLACK_BSSID_NUM, local_para_ptr->bssid_list_num);

        for (p = 0; p < local_para_ptr->bssid_list_num && p < WNDRV_BSSID_MAX_NUM; p++) {
            //wpa_printf(MSG_DEBUG, "WPA: p = %d",p);
            kal_trace(DBG_CONN, INFO_SUPC_BLACK_LIST_NUM, p);

            if (wpa_listed_success(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id)) { // Kinki add for MAUI_00401655
                continue;
            }

            wpa_auth_timeout_list_del(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);

            if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE && local_para_ptr->bssid_list[p].is_status_code_present && local_para_ptr->bssid_list[p].status_code == 15) {
                wpa_blacklist_add_permanent(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
            } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK && local_para_ptr->bssid_list[p].is_reason_code_present && local_para_ptr->bssid_list[p].reason_code == 15) {
                wpa_blacklist_add_permanent(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
            } else {
                wpa_blacklist_add_temporal(wpa_s, local_para_ptr->bssid_list[p].bssid, wpa_s->current_ssid->profile_id);
            }

        }
        supc_blacklist_update(wpa_s);

#endif

#ifdef __WAPI_SUPPORT__
        if (wpa_s->key_mgmt == WAPI_KEY_MGMT_WAI ||
                wpa_s->key_mgmt == WAPI_KEY_MGMT_PSK) {
            MAC_ADDRESS bssid_s;
            MAC_ADDRESS own_s;

            memcpy(bssid_s.v, wpa_s->bssid, sizeof(bssid_s.v));
            memcpy(own_s.v, wpa_s->own_addr, sizeof(own_s.v));

            /* stop WAPI timer in WAI_Msg_Input */
//            wpa_supplicant_cancel_wapi_auth_timeout(wpa_s);
            WAI_Msg_Input(CONN_DISASSOC, &bssid_s, &own_s, NULL, 0);
        }
#endif
        /* CMCC 7.6.2 When RSSI < 85dBM. disconnect directly
        */
        if (local_para_ptr->session_id & ((kal_uint32) 1 << 31)) {
            wpa_s->eap_fail_cause = SUPC_ABM_LOW_RSSI;
        } else {
            wpa_s->eap_fail_cause = SUPC_ABM_LOST_COVERAGE;
        }


        /* J, maybe we can not just add this ap into blacklist for it might be the case
         * user just walks away
         */
        //wpa_blacklist_add(wpa_s, wpa_s->bssid);
        wpa_supplicant_mark_disassoc(wpa_s);    // Karen, clear current bssid here
        //wpa_msg(wpa_s, MSG_INFO, "Disconnect event - remove keys");
//      kal_trace(DBG_CONN, INFO_SUPC_REMOVE_KEY_IN_DISCONN);

        if (wpa_s->ap_selection_mode == AP_AUTOMATIC_SELECT) {
            /* Joseph:
             * if AP_AUTOMATIC_SELECT mode is specified, supplicant will be in charge of finding
             * another workable AP for user
             */
            //wpa_supplicant_req_scan(wpa_s, 0, 100000);

            //if (wpa_supplicant_dynamic_keys(wpa_s))
            //{
            //  wpa_s->keys_cleared = 0;
            //  wpa_clear_keys(wpa_s, wpa_s->bssid);
            //}

            // wpa_s->is_wpa_connect_req_pending = KAL_FALSE;

            // J, start handover timer

            /*
            after wndrv reassociate fail,  it send disconnect_ind.
            supc may try to autoconn with profiles. here handover timer prevent
            supc auto-conn from fall into "scan, assoc, fail, scan again" dead loop.
            */
            if (wpa_s->handover_timer_event_id == NULL
                    && supc_allow_autoconn()) {
                wpa_s->handover_timer_event_id = supc_start_wpa_handover_timer(wpa_s->handover_timer_value);
            }

            supc_stop_auth_timer();
#ifdef WIFI_PORT_TIMER
            supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif
            /*
                if (wpa_s->num_of_disconnect <= 2)
                {
                wpa_printf(MSG_DEBUG, "WPA: inside disconnect_ind, associate with current profile. Num= %d",wpa_s->num_of_disconnect);
                supc_assoc_current_profile(); // retry current SSID immediately
                }
            else
                {
                    wpa_s->wpa_state = WPA_SCANNING;
                    wpa_supplicant_req_scan(wpa_s, 2, 0);
                }
                */
            if (supc_allow_autoconn()) {
                wpa_s->wpa_state = WPA_SCANNING;
                wpa_supplicant_req_scan(wpa_s, 2, 0);
            } else {
                supc_do_disconnect_req();
                supc_send_abm_wifi_disconnect_ind(wpa_s->eap_fail_cause);
                wpa_s->num_of_profile = 0;
                /*if disc_ind received before send conn_cnf, supc must retry profiles*/
                if (wpa_s->is_wpa_connect_req_pending == KAL_TRUE) {
                    wpa_s->wpa_state = WPA_SCANNING;
                    wpa_supplicant_req_scan(wpa_s, 2, 0);
                }
            }
        } else if (wpa_s->ap_selection_mode == AP_MANUAL_SELECT) {
            /* CMCCTD to MAUI
            */
            supc_do_disconnect_req();
            supc_send_abm_wifi_disconnect_ind(wpa_s->eap_fail_cause);
        }

    }
    return;
}

/*********************************************************************
   Function Name    : supc_status_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_status_ind_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_status_ind_struct *local_para_ptr = NULL;

    INNER_TRACE_STEP(1);
    local_para_ptr = (wndrv_supc_status_ind_struct *)(ilm_ptr->local_para_ptr);

    switch (local_para_ptr->flags) {
        case WNDRV_AUTH_REQUEST_REAUTH:
            break;
        case WNDRV_AUTH_REQUEST_KEY_UPDATE:
            break;
        case WNDRV_AUTH_REQUEST_PAIRWISE_ERROR:
            supc_mic_failure_handler(1);
            break;
        case WNDRV_AUTH_REQUEST_GROUP_ERROR:
            supc_mic_failure_handler(0);
            break;
    }
}
/*********************************************************************
   Function Name    : supc_capbility_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_capbility_ind_handler(ilm_struct *ilm_ptr)
{

#ifdef __WAPI_SUPPORT__

    wndrv_supc_query_capability_ind_struct *local_para_ptr = NULL;

    local_para_ptr = (wndrv_supc_query_capability_ind_struct *)(ilm_ptr->local_para_ptr);

    wpa_s->cap_check[0] = local_para_ptr->response[0];
    wpa_s->cap_check[1] = local_para_ptr->response[1];
    wpa_s->cap_check[2] = local_para_ptr->response[2];
    wpa_s->cap_check[3] = local_para_ptr->response[3];
#endif
}

/* Joseph:
 * Message handler of WNDRV_SUPC_SITE_SURVEY_IND
 */
/*********************************************************************
   Function Name    : supc_site_survey_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_site_survey_ind_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_site_survey_ind_struct *local_para_ptr = NULL;
    kal_uint8 num   = 0;
    wndrv_supc_bss_info_struct *results = NULL;
    wndrv_supc_bss_info_struct *selected = NULL;
    int  prio;
    struct wpa_ssid *ssid;
    kal_bool is_ibss_mode = KAL_FALSE;
    kal_uint8 num_of_diff_ap;
    wndrv_supc_bss_info_struct *filter_result = NULL;
    kal_uint8 *ap_num = NULL; /*Kinki: number of APs in a SSID [MAUI_00398552]*/
    struct wpa_ssid **prio_ssid;
    //kal_uint16 k = 0;
    //wndrv_supc_bss_info_struct * pdbg = NULL;
    //kal_uint8 loop = 0;
#ifdef __WPS_SUPPORT__
    enum wps_request_type req_type = WPS_REQ_ENROLLEE;
#endif

    //INNER_TRACE_STEP(1);

    /* MAUI_02639824 note: avoid twice site survey req with ind
    */
    fgSiteSurveyReq = KAL_FALSE;

    local_para_ptr = (wndrv_supc_site_survey_ind_struct *)(ilm_ptr->local_para_ptr);

#if 0 // for debug
    pdbg = local_para_ptr->bss_info;
    for (loop = 0; loop < local_para_ptr->num; loop ++) {
        wpa_printf(MSG_DEBUG, "report %d st AP", loop);
        wpa_hexdump(MSG_DEBUG, "dump row AP" , (const char *)pdbg, 128);
        pdbg ++;
    }
#endif

    /* Joseph:
     * The scan results is passed within the primitive. Remark these code
     */
    /*
    if (wpa_supplicant_get_scan_results(wpa_s) < 0) {
        wpa_printf(MSG_DEBUG, "Failed to get scan results - try "
               "scanning again");
        wpa_supplicant_req_scan(wpa_s, 1, 0);
        return;
    }
    */

    /* Joseph:
     * Update the site survey result into wpa context
     */

    /*Kinki add*/
    //SUPC_ASSERT(wpa_s->num_scan_results <= WNDRV_MAX_SCAN_RESULTS_NUM);
    //kal_mem_cpy(&(wpa_s->scan_results[wpa_s->num_scan_results]), local_para_ptr->bss_info, local_para_ptr->num * sizeof(wndrv_supc_bss_info_struct));
    //wpa_s->num_scan_results += local_para_ptr->num;

    if ((wpa_s->num_scan_results + local_para_ptr->num) > WNDRV_MAX_SCAN_RESULTS_NUM) {
//      kal_trace(DBG_SCAN,INFO_SUPC_SCAN_FULL_IND, wpa_s->num_scan_results + local_para_ptr->num, WNDRV_MAX_SCAN_RESULTS_NUM);
        wpa_printf(TRACE_GROUP_1, "Scan num %d exceeds max number = %d", wpa_s->num_scan_results + local_para_ptr->num, WNDRV_MAX_SCAN_RESULTS_NUM);
        kal_mem_cpy(&(wpa_s->scan_results[wpa_s->num_scan_results]),
                    local_para_ptr->bss_info,
                    (WNDRV_MAX_SCAN_RESULTS_NUM - wpa_s->num_scan_results) *
                    sizeof(wndrv_supc_bss_info_struct));
        wpa_s->num_scan_results = WNDRV_MAX_SCAN_RESULTS_NUM;
    } else {
        kal_mem_cpy(&(wpa_s->scan_results[wpa_s->num_scan_results]),
                    local_para_ptr->bss_info,
                    local_para_ptr->num * sizeof(wndrv_supc_bss_info_struct));
        wpa_s->num_scan_results += local_para_ptr->num;
//        kal_trace(DBG_SCAN, INFO_SUPC_SCAN_NUM, local_para_ptr->num, wpa_s->num_scan_results);
        wpa_printf(TRACE_ERROR, "bssid=%02x:%02x:%02x:%02x:%02x:%02x, wps_ie_len=%d\n", local_para_ptr->bss_info[0].bssid[0], local_para_ptr->bss_info[0].bssid[1],
                   local_para_ptr->bss_info[0].bssid[2], local_para_ptr->bss_info[0].bssid[3], local_para_ptr->bss_info[0].bssid[4],
                   local_para_ptr->bss_info[0].bssid[5], local_para_ptr->bss_info[0].wps_ie_len);
        wpa_printf(TRACE_GROUP_1, "get scan result=%d total=%d\n", local_para_ptr->num, wpa_s->num_scan_results);
    }

    if (local_para_ptr->more_flag != 0) {
//        kal_trace(DBG_SCAN, INFO_SUPC_SCAN_MORE_IND);
        //wpa_printf(TRACE_GROUP_1, "there more scan result in wndrv to be reported\n");
        return;
    }

    /* Joseph:
     * MMI send supc_mmi_wifi_ap_list_req before. Send the cnf message back
     */
    if (wpa_s->is_mmi_req_ap_list == KAL_TRUE) {
        wpa_s->is_mmi_req_ap_list = KAL_FALSE;
//        kal_trace(DBG_SCAN, INFO_SUPC_SCAN_DONE_IND);
        wpa_printf(TRACE_GROUP_1, "scan done\n");
        supc_stop_scan_monitor_timer();
        if (wpa_s->num_scan_results > 0) {
            filter_result = SUPC_ALLOC_BUFF(wpa_s->num_scan_results * sizeof(wndrv_supc_bss_info_struct));
            ap_num = SUPC_ALLOC_BUFF(wpa_s->num_scan_results * sizeof(kal_uint8)); /*Kinki: number of APs in a SSID*/
            supc_filter_duplicated_ap(wpa_s->num_scan_results, wpa_s->scan_results, &num_of_diff_ap, filter_result, ap_num);
            supc_send_abm_wifi_ap_list_cnf(num_of_diff_ap, filter_result, ap_num);
            SUPC_DEALLOC_BUFF(filter_result);
            SUPC_DEALLOC_BUFF(ap_num);
        } else {
            supc_send_abm_wifi_ap_list_cnf(0, filter_result, ap_num); //no ap found
        }
#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->wpa_state == WPA_DISCONNECTED) {
            if (wpa_s->chip_state != CHIP_POWER_OFF) { //Kinki add for power-saving
                wpa_s->chip_state = CHIP_POWER_OFF;
                supc_send_wndrv_chip_poweroff_req();
            }
        }
#endif
        if (wpa_s->is_site_survey_req_pending == KAL_FALSE) { //no need to react the scan req
            wpa_printf(TRACE_GROUP_1, "doesn't pend in scaning, msg handler over");
//          kal_trace(DBG_SCAN, INFO_SUPC_SCAN_NOT_PENDING_IND);
            return;
        }
    }

    /* Joseph:
     * send the join_network req if and only if SUPC in the state of SCANING
     */
    if (wpa_s->wpa_state != WPA_SCANNING) {
        wpa_s->is_site_survey_req_pending = KAL_FALSE; /*Kinki add*/
        wpa_printf(TRACE_ERROR,
                   "Ignore site survey indication, wrong state = %d, msg handler over",
                   wpa_s->wpa_state);
//      kal_trace(DBG_SCAN, INFO_SUPC_SCAN_WRONG_STA_IND,wpa_s->wpa_state);
        return;
    }

    /* Joseph:
     * send the join_network req if and only if SUPC in the state of SCANING
     */

    num = wpa_s->num_scan_results;
    results = wpa_s->scan_results;
    wpa_s->is_site_survey_req_pending = KAL_FALSE; /*Kinki add*/
    if (NULL == wpa_s->conf) {
        wpa_printf(TRACE_ERROR, "Error!! wpa_s->conf is NULL");
//      kal_trace(DBG_CONN,INFO_SUPC_CONF_NULL_IND);
        return;
    }
    prio_ssid = (wpa_s->conf->pssid);

    supc_dbg_dump_supplicant();

    while (selected == NULL) {
        for (prio = 0; prio < SUPC_MAX_SSID_CONFIG_PRIORITY; prio++) {
            if (prio_ssid[prio] != NULL) {
                if ((wpa_s->connection_type == ALL_PROFILE_ENABLE) &&
                        (prio_ssid[prio]->network_type == WNDRV_SUPC_NETWORK_IBSS)) {
                    wpa_printf(MSG_DBG_SCAN, "IBSS and INFRA net coexist, skip select bss");
//                  kal_trace(DBG_CONN,INFO_SUPC_IBSS_INF_COEXISTD);
                    break;
                }
                selected = wpa_supplicant_select_bss(wpa_s, prio_ssid[prio], &ssid);

                // We support only one profile in IBSS mode
                if (prio_ssid[prio]->network_type == WNDRV_SUPC_NETWORK_IBSS) {
                    is_ibss_mode = KAL_TRUE;
                    break;
                }
            }

            if (selected) {
                break;
            }
        }

        /* Joseph:
         * Here, we'll retry the tried but still failed APs. Maybe we can find out
         * the methods to avoid it
         */
        if ((selected == NULL) && (is_ibss_mode == FALSE)) {
            for (prio = (wpa_s->prev_scan_ssid->priority + 1);
                    prio < SUPC_MAX_SSID_CONFIG_PRIORITY; prio++) {
                wpa_printf(TRACE_GROUP_1, "restart scan at prio=%d\n", prio);
//              kal_trace(DBG_CONN,INFO_SUPC_RESTART_SCAN_PRIO,prio);
                if (prio_ssid[prio] != NULL) {
                    if (prio_ssid[prio]->network_type == WNDRV_SUPC_NETWORK_IBSS) {
                        //Kinki add for IBSS power-saving: do not scan IBSS network
                        break;
                    }
                    wpa_hexdump(MSG_DBG_SCAN, "SSID:",
                                (const char *)prio_ssid[prio]->ssid,
                                prio_ssid[prio]->ssid_len);
                    wpa_printf(TRACE_GROUP_1, "BSSID= " MACSTR, MAC2STR(prio_ssid[prio]->bssid));
//                  kal_buffer_trace(DBG_CONN,INFO_SUPC_PROFILE_BSSID,6,prio_ssid[prio]->bssid);

                    wpa_s->prev_scan_ssid = prio_ssid[prio];
                    wpa_s->num_scan_results = 0;
                    wpa_s->is_site_survey_req_pending = KAL_TRUE; /*Kinki add*/
#ifdef __WIFI_CHIP_DYM_POWERON__
                    if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
                        wpa_s->chip_state = CHIP_POWER_ON;
                        supc_send_wndrv_chip_poweron_req();
                    }
#endif
                    supc_send_wndrv_site_survey_req(wpa_s->prev_scan_ssid->ssid,
                                                    wpa_s->prev_scan_ssid->ssid_len);
                    return;
                }
            }
        }

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
        if (selected == NULL && wpa_s->temporal_blacklist) {
            wpa_printf(TRACE_GROUP_1, "No APs found - clear temporal blacklist "
                       "and try again");
            kal_trace(DBG_CONN, INFO_SUPC_NO_AP_CLR_TMP_LIST);
            wpa_auth_timeout_list_clear(wpa_s);
            wpa_blacklist_clear_temporal(wpa_s);
            supc_blacklist_update(wpa_s);
        } else if (selected == NULL) {
            break;
        }
#else
        if (selected == NULL && wpa_s->blacklist) {
            wpa_printf(TRACE_GROUP_1, "No APs found - clear blacklist "
                       "and try again");

//          kal_trace(DBG_CONN,INFO_SUPC_NO_AP_CLR_LIST);
            wpa_blacklist_clear(wpa_s);
        } else if (selected == NULL) {
            break;
        }
#endif

    }

    if (selected) {
        /* Joseph:
         * Strange if statement
         * ===================================================
         * if (wpa_s->reassociate  ||
         *   kal_mem_cmp(selected->bssid, wpa_s->bssid, ETH_ALEN) != 0)
         * ===================================================
         * rewrite
         */
        /* Joseph:
         * The original state is correct. If and only if it's a reassociate case we can init
         * associate procedure directly
         * ===================================================
         * if ((wpa_s->reassociate == 0) ||
         *   kal_mem_cmp(selected->bssid, wpa_s->bssid, ETH_ALEN) != 0)
         * ===================================================
         * Change it back
         */
#if 0
        if ((wpa_s->reassociate) ||
                kal_mem_cmp(selected->bssid, wpa_s->bssid, ETH_ALEN) != 0) {
            /* Joseph
             * Init sim card if needed. We should request for the IMSI in the initial state NOT HERE!
             */
            //wpa_supplicant_scard_init(wpa_s, ssid);

            /* Joseph
             * Start to issue associate req
             */
            wpa_supplicant_associate(wpa_s, selected, ssid);

        } else {
            wpa_printf(MSG_DEBUG, "Already associated with the "
                       "selected AP. reassoc flag %d", wpa_s->reassociate);
        }
#endif
#ifdef __WPS_SUPPORT__
        if (wpa_s->connection_type == START_WPS_PBC) {
            if (wpas_wps_scan_pbc_overlap(wpa_s, selected, ssid)) {
                wpa_printf(TRACE_GROUP_1, "PBC session overlap\n"); //later use wps trace group
//                kal_trace(DBG_WPS,INFO_SUPC_WPS_PBC_OVERLAP);
                supc_send_abm_wifi_wps_pbc_status(KAL_TRUE);
                goto req_scan;
            }
            supc_send_abm_wifi_wps_pbc_status(KAL_FALSE);
        }

        /*since wps add a ssid into wpa_s->conf without setting ssid name,
        if select bss done, it must set ssid name, or this ssid is invalid*/
        if (ssid->ssid_len == 0) {
            ssid->ssid_len = selected->ssid_len;
            kal_mem_cpy(ssid->ssid, selected->ssid, selected->ssid_len);
            wpa_printf(TRACE_GROUP_1, "copy SSID form selected ssid\n");
            //wpa_hexdump(MSG_DEBUG, "SSID: ",(char*)ssid->ssid, ssid->ssid_len);
//          kal_trace(DBG_WPS,INFO_SUPC_WPS_COPY_SSID_FROM_SCAN);
//          kal_wap_trace(MAP_MOD_SUPC,DBG_WPS,"%s,len:%d",ssid->ssid,ssid->ssid_len);
        } else {
            if ((kal_mem_cmp(ssid->ssid, selected->ssid, ssid->ssid_len) != 0) ||
                    ssid->ssid_len != selected->ssid_len) {
                wpa_printf(TRACE_GROUP_1, "fatal error! AP selected does not match with specified one\n");
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_SELECTED_AP_NOT_MATCH);
                goto req_scan;
            }
        }


#endif

#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
            wpa_s->chip_state = CHIP_POWER_ON;
            supc_send_wndrv_chip_poweron_req();
        }
#endif
#if 0//#ifdef __CERTMAN_SUPPORT__
        wpa_s->selected_ssid = ssid; //Kinki add for CertMan
        wpa_s->selected_bssid = selected;

        if (ssid->get_all_cert_or_not == KAL_FALSE) {
            if (supc_get_cert_and_privkey(wpa_s->selected_ssid) == 0) {
                wpa_supplicant_associate(wpa_s, wpa_s->selected_bssid, wpa_s->selected_ssid);
            }
        } else {
            wpa_supplicant_associate(wpa_s, wpa_s->selected_bssid, wpa_s->selected_ssid);
        }
#else
        wpa_supplicant_associate(wpa_s, selected, ssid);
#endif
        rsn_preauth_scan_results(wpa_s, results, num);
        wpa_s->scan_interval_index = 0;
        /*after associate, handler over and return*/
        return;
    }
#ifdef __WPS_SUPPORT__

req_scan:

#endif
    if (is_ibss_mode) {
#ifdef __WIFI_CHIP_DYM_POWERON__
        if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
            wpa_s->chip_state = CHIP_POWER_ON;
            supc_send_wndrv_chip_poweron_req();
        }
#endif
        wpa_supplicant_associate(wpa_s, NULL, prio_ssid[prio]);
        wpa_printf(TRACE_GROUP_1, "WPA: we are the creater in IBSS mode\n");
//      kal_trace(DBG_CONN,INFO_SUPC_CREATE_IBSS);
        return;
    }
    /* Joseph:
     * No matched AP. We have to find way to solve this. Proposal
     * 1. Retry N times and report fail
     * 2. Retry N times with timer and report fail
     * 3. Report fail directly
     */
    //wpa_printf(MSG_INFO, "No suitable AP found.");
    // exponentially increase the sitesurvey interval
    //wpa_printf(MSG_INFO, "Scan after %d secs",
    //scan_interval_table[wpa_s->scan_interval_index]);

//  kal_trace(DBG_CONN,INFO_SUPC_NO_AP_SCAN_AGAIN,scan_interval_table[wpa_s->scan_interval_index]);
#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state != CHIP_POWER_OFF) { //Kinki add for power-saving
        wpa_s->chip_state = CHIP_POWER_OFF;
        supc_send_wndrv_chip_poweroff_req();
    }
#endif
#ifdef __WPS_SUPPORT__
    if (wpas_wps_in_use(wpa_s->conf, &req_type)) {
        /*wps has particular scenario.
        user must input pincode or press PBC button.
        so it must expand scan interval*/
        wpa_printf(MSG_DEBUG, "do scan in 8s for wps\n");
//      kal_trace(DBG_WPS,INFO_SUPC_SCAN_FOR_WPS);
        wpa_supplicant_req_scan(wpa_s, 8, 0);
    } else {
        wpa_supplicant_req_scan(wpa_s,
                                scan_interval_table[wpa_s->scan_interval_index], 0);
    }
#else
    wpa_supplicant_req_scan(wpa_s,
                            scan_interval_table[wpa_s->scan_interval_index], 0);
#endif
    wpa_s->scan_interval_index ++;
    if (wpa_s->scan_interval_index > max_scan_interval_index) {
        wpa_s->scan_interval_index = max_scan_interval_index;
    }
}



/*********************************************************************
   Function Name    : supc_pmkid_candidate_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct * ilm_ptr
   Description      :
*********************************************************************/
void supc_pmkid_candidate_ind_handler(ilm_struct *ilm_ptr)
{

    wndrv_supc_pmkid_candidate_ind_struct *local_para_ptr = NULL;
    wndrv_supc_pmkid_candidate_struct *pmkid_candidate_ptr = NULL;
    kal_uint32 pmkid_candidate_num = 0;
    kal_uint32 index = 0;

    INNER_TRACE_STEP(1);
    local_para_ptr = (wndrv_supc_pmkid_candidate_ind_struct *)(ilm_ptr->local_para_ptr);
    pmkid_candidate_num = local_para_ptr->num;

    for (index = 0; index < pmkid_candidate_num; index++) {
        pmkid_candidate_ptr = &(local_para_ptr->list[index]);
        //If the preauthentication is not allowed, skip it
        if (pmkid_candidate_ptr->preauth_enabled == KAL_FALSE) {
            continue;
        }

        pmksa_candidate_add(wpa_s, &(pmkid_candidate_ptr->bssid[0]), index);
    }


}

/*********************************************************************
   Function Name    : supc_sim_ready_ind_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct * ilm_ptr
   Description      :
*********************************************************************/
#include "milenage.h" /* Jau Add for AKA simulator testing */
#if 0

void supc_sim_ready_ind_handler(ilm_struct *ilm_ptr)
{
    sim_ready_ind_struct *local_para_ptr = (sim_ready_ind_struct *)ilm_ptr->local_para_ptr;
    u8 i = 2;

#if 0//#ifdef __GEMINI__ //[MAUI_01629798] mtk01612: kinki_gemini_wifi
    if (ilm_ptr->src_mod_id == MOD_SIM_2) { //[MAUI_01629798] mtk01612: kinki_gemini_wifi : ignore the SIM2 READY IND, just care about SIM1
        return;
    }
    /*kinki_gemini_wifi: gemini(on/off) * wifi(on/off)

    gemini(on) * wifi(on): do nothing if it's from SIM_2, handle the msg from SIM1
    gemini(on) * wifi(off): pure gemini project. should not enter wifi
    gemini(off) * wifi(on): pure wifi project. do nothing.
    gemini(off) * wifi(off): no such case. should not enter wifi
    */
#endif
    if (supc_check_sim_ready(ilm_ptr->src_mod_id)) {
        return;
    }

    kal_mem_cpy(wpa_s->imsi, local_para_ptr->ef_imsi, 9);

    supc_send_sim_read_imsi_mnc_req();

    /* Start -- Jau add for AKA milenage simulation { */
#if CONFIG_SIM_USIM_SIMULATOR
    kal_mem_cpy(real_local_imsi, local_para_ptr->ef_imsi, 9);
    if (milenage_simulation) {
        /* identity = "0232 0100 0000 0000" */
        wpa_s->imsi[1] = ('2' - '0') << 4;
        wpa_s->imsi[2] = ('3' - '0') + (('2' - '0') << 4);
        wpa_s->imsi[3] = ('0' - '0') + (('1' - '0') << 4);
        wpa_s->imsi[4] = ('0' - '0') + (('0' - '0') << 4);
        wpa_s->imsi[5] = ('0' - '0') + (('0' - '0') << 4);
        wpa_s->imsi[6] = ('0' - '0') + (('0' - '0') << 4);
        wpa_s->imsi[7] = ('0' - '0') + (('0' - '0') << 4);
        wpa_s->imsi[8] = ('0' - '0') + (('0' - '0') << 4);
        //wpa_s->imsi[9] = ('0'-'0') + (('0'-'0')<<4);
        wpa_s->eap_sim_id[0] = '1';
    }
    real_local_id[0] = 0x31;
    real_local_id[1] = (real_local_imsi[1] >> 4) + 0x30;
#endif
    /* Stop  -- Jau add for AKA milenage simulation } */

    wpa_s->eap_sim_id[0] = 0x31;
    wpa_s->eap_sim_id[1] = (wpa_s->imsi[1] >> 4) + 0x30;

    /* Convert IMSI to ASCII */
    for (i = 2; i < 16; i++) {
        if ((i % 2) == 1) {
            wpa_s->eap_sim_id[i] = (wpa_s->imsi[i / 2 + 1] >> 4) + 0x30;
#if CONFIG_SIM_USIM_SIMULATOR
            real_local_id[i] = (real_local_imsi[i / 2 + 1] >> 4) + 0x30;
#endif
        } else {
            wpa_s->eap_sim_id[i] = (wpa_s->imsi[i / 2 + 1] & 0x0F) + 0x30;
#if CONFIG_SIM_USIM_SIMULATOR
            real_local_id[i] = (real_local_imsi[i / 2 + 1] & 0x0F) + 0x30;
#endif
        }
    }

    wpa_s->imsi_ready = KAL_TRUE;

    if (wpa_s->need_to_send_init_cnf) {
        supc_send_abm_wifi_init_cnf();
        wpa_s->need_to_send_init_cnf = KAL_FALSE;
    }

}


/*********************************************************************
   Function Name    : supc_sim_read_cnf_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct * ilm_ptr
   Description      :
*********************************************************************/
void supc_sim_read_cnf_handler(ilm_struct *ilm_ptr)
{
    char buf[100];
    wpa_size_t len;
    sim_read_cnf_struct *local_para_ptr = (sim_read_cnf_struct *)ilm_ptr->local_para_ptr;
    u8 i = 2;
    /* Joseph
     * Need to re-write
     */

    /* Joseph
     * 1. Store the imsi information
     * 2. Send MMI the init ind
     */
    /* Joseph, IMSI len is not more than 15 digits. We'll get the 9 bytes raw data.
     * The first byte is the length and it must be 08. And the left nibble of the second
     * byte is ignored. So we have 7.5 nibbles and represents a total 15 digits
     */
    SUPC_ASSERT(local_para_ptr->length == 9);
    /* EAP_SIM spec. We have to add ASCII "1" (0x31) as the first character */
#if 0
    wpa_s->eap_sim_id[0] = 0x31;
    wpa_s->eap_sim_id[1] = ((local_para_ptr->data[1]) >> 4) + 0x30;


    /* Convert IMSI to ASCII */
    for (i = 2; i < 16; i++) {
        if ((i % 2) == 0) {
            wpa_s->eap_sim_id[i] = (local_para_ptr->data[i / 2 + 1] >> 4) + 0x30;
        } else {
            wpa_s->eap_sim_id[i] = (local_para_ptr->data[i / 2 + 1] & 0x0F) + 0x30;
        }
    }
#endif

    kal_mem_cpy(wpa_s->imsi, local_para_ptr->data, 9);

    wpa_s->eap_sim_id[0] = 0x31;
    wpa_s->eap_sim_id[1] = (wpa_s->imsi[1] >> 4) + 0x30;

    /* Convert IMSI to ASCII */
    for (i = 2; i < 16; i++) {
        if ((i % 2) == 1) {
            wpa_s->eap_sim_id[i] = (wpa_s->imsi[i / 2 + 1] >> 4) + 0x30;
        } else {
            wpa_s->eap_sim_id[i] = (wpa_s->imsi[i / 2 + 1] & 0x0F) + 0x30;
        }
    }

    supc_send_abm_wifi_init_cnf();

#if 0

    if (wpa_s->scard != NULL) {
        wpa_supplicant_imsi_identity(wpa_s, ssid);
        return;
    }
    wpa_printf(MSG_DEBUG, "Selected network is configured to use SIM - "
               "initialize PCSC");
    wpa_s->scard = scard_init(SCARD_TRY_BOTH, ssid->pin);
    if (wpa_s->scard == NULL) {
        wpa_printf(MSG_WARNING, "Failed to initialize SIM "
                   "(pcsc-lite)");
        /* TODO: what to do here? */
        return;
    }
    eapol_sm_register_scard_ctx(wpa_s->eapol, wpa_s->scard);

    len = sizeof(buf);
    if (scard_get_imsi(wpa_s->scard, buf, &len)) {
        wpa_printf(MSG_WARNING, "Failed to get IMSI from SIM");
        /* TODO: what to do here? */
        return;
    }

    wpa_hexdump_ascii(MSG_DEBUG, "IMSI", (u8 *) buf, len);
    MEM_FREE(wpa_s->imsi);
    wpa_s->imsi = MEM_MALLOC(len);
    if (wpa_s->imsi) {
        memcpy(wpa_s->imsi, buf, len);
        wpa_s->imsi_len = len;
        wpa_supplicant_imsi_identity(wpa_s, ssid);
    }
#endif
}
#endif

/* Joseph:
 * Todos:
 * 1. Save the AP selection mode into context
 * 2. Get the pointer of the profiles and store the information of the profiles
 * 3. Change the WPA state to WPA_SCANNING and starts to scan
 */

/*********************************************************************
   Function Name    : supc_abm_wifi_connect_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct * ilm_ptr
   Description      :
*********************************************************************/
void supc_abm_wifi_connect_req_handler(ilm_struct *ilm_ptr)
{

    supc_abm_wifi_connect_req_struct *local_para_ptr = NULL;
    wpa_config *wpa_conf = NULL;
    wpa_ssid *ssid, *tail = NULL, *head = NULL, * ptail = NULL;
    wifi_data_account_profile_struct *profile_ptr = NULL;
#if 0//#ifndef __CERTMAN_SUPPORT__
    wifi_data_account_profile_struct *temp_profile_ptr = NULL;  //Kinki add
#endif
    kal_uint8 i = 0, num = 0;
    struct eapol_ctx *ctx;
    int  prio;
#if 0
    if (MAP_MSG_ID_TST_INJECT_STRING == ilm_ptr->msg_id) {
        tst_inject_string_struct *tst = (tst_inject_string_struct *)(ilm_ptr->local_para_ptr);
        local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_req_struct), TD_UL);
        if (0 == strcmp((char *)tst->string, "pbc") ||
                0 == strcmp((char *)tst->string, "PBC")) {
            local_para_ptr->connect_type = START_WPS_PBC;
        } else if (0 == strcmp((char *)tst->string, "pin") ||
                   0 == strcmp((char *)tst->string, "PIN")) {
            local_para_ptr->connect_type = START_WPS_PIN;
        } else {
            //wpa_printf(MSG_DEBUG, "Invalid inject msg");
            kal_trace(DBG_CONN, INFO_SUPC_INVALID_INJECT_MSG);
            goto done;
        }
        local_para_ptr->more_flag = 0;
        local_para_ptr->num_of_profile = 1;
        local_para_ptr->selection_mode = AP_AUTOMATIC_SELECT;
        kal_mem_set(local_para_ptr->profile_header_ptr, 0,
                    sizeof(local_para_ptr->profile_header_ptr));
        //wpa_printf(MSG_DEBUG, "do inject msg:  %s connect", tst->string);
        kal_trace(DBG_CONN, INFO_SUPC_INJECT_MSG);
        kal_wap_trace(MAP_MOD_SUPC, DBG_CONN, "%s", tst->string);


    } else {
#endif
        local_para_ptr = (supc_abm_wifi_connect_req_struct *)ilm_ptr->local_para_ptr;
#if 0
    }
#endif

    supc_dbg_dump_supplicant();
    if (wpa_s->wpa_state >= WPA_SCANNING) {
        supc_do_disconnect_req();    // Kinki add for disconnect req
    }


    if ((wpa_s->num_of_profile + local_para_ptr->num_of_profile) > MAX_NUM_PROFILE) {
        wpa_printf(TRACE_GROUP_1, "Num of profiles %d exceeds max number = %d\n",
                   wpa_s->num_of_profile + local_para_ptr->num_of_profile, MAX_NUM_PROFILE);
//      kal_trace(DBG_CONN,INFO_SUPC_PROFILE_EXCEED,wpa_s->num_of_profile + local_para_ptr->num_of_profile, MAX_NUM_PROFILE);

        num  = MAX_NUM_PROFILE - wpa_s->num_of_profile;
    } else {
        num = local_para_ptr->num_of_profile;
    }

    // If configration is changed, clear the cache
    if (kal_mem_cmp(&(wpa_s->profile[wpa_s->num_of_profile]), &local_para_ptr->profile_header_ptr[0],
                    num * sizeof(wifi_data_account_profile_struct)) != 0) {
        rsn_preauth_deinit(wpa_s);
        pmksa_candidate_free(wpa_s);
        pmksa_cache_free(wpa_s);
    }
    INNER_TRACE_STEP(1);
#if 0//#ifndef __CERTMAN_SUPPORT__
    else {
        temp_profile_ptr = &local_para_ptr->profile_header_ptr[0];

        for (i = 0; i < num; i++) {
            if (supc_cmp_fileInfo(temp_profile_ptr, i + wpa_s->num_of_profile) > 0) { //point
                rsn_preauth_deinit(wpa_s);
                pmksa_candidate_free(wpa_s);
                pmksa_cache_free(wpa_s);

                wpa_printf(MSG_DEBUG, "profile id=%d is not the same as before", i);

                break;

            }
            temp_profile_ptr++;
        }
    }
#endif
    wpa_printf(TRACE_GROUP_1, "profile num=%d\n", num);
//  kal_trace(DBG_CONN,INFO_SUPC_PROFILE_NUM,num);


    kal_mem_cpy(&(wpa_s->profile[wpa_s->num_of_profile]), &local_para_ptr->profile_header_ptr[0],
                num * sizeof(wifi_data_account_profile_struct));
    wpa_s->num_of_profile += num;

    if (local_para_ptr->more_flag == 1) {
        wpa_printf(TRACE_GROUP_1, "return and wait for next connect_req\n");
//      kal_trace(DBG_CONN,INFO_SUPC_WAIT_NEXT_CONN_REQ);
        goto done;
    }
    /*for WPS RD test*/
    if (force_wps_pin == KAL_TRUE) {
        local_para_ptr->selection_mode  = START_WPS_PIN;
        local_para_ptr->handover_timer_value = WPS_PBC_WALK_TIME;
    }
    wpa_s->ap_selection_mode = (supc_ap_selection_mode_enum)(local_para_ptr->selection_mode);
    wpa_s->connection_type = (supc_connection_type_enum)(local_para_ptr->connect_type);
    if (START_WPS_PBC == wpa_s->connection_type ||
            START_WPS_PIN == wpa_s->connection_type) {
        local_para_ptr->handover_timer_value = WPS_PBC_WALK_TIME;
    }
#ifdef __WPS_SUPPORT__
    wpa_s->need_abm_conn_cnf = KAL_TRUE;
    wpa_s->wps_success = 0;

    /* if enable WPS feature, it need to create a wps_context in ctx*/
    if (START_WPS_PBC == wpa_s->connection_type ||
            START_WPS_PIN == wpa_s->connection_type) {
        if (wpa_s->conf == NULL) {
            wpa_s->conf = SUPC_ALLOC_BUFF(sizeof(struct wpa_config));
        }
        kal_mem_set(wpa_s->conf, 0, sizeof(struct wpa_config));

        wpa_s->conf->device_name = WPS_DEVICE_NAME;//wndrv_get_dev_name();
        wpa_s->conf->manufacturer = WPS_MANUFACTURER;//wndrv_get_manufacture_name();
        wpa_s->conf->model_name = WPS_MODEL_NAME;//wndrv_get_model_name();
        wpa_s->conf->model_number = WPS_MODEL_NUM;//wndrv_get_model_num();
        wpa_s->conf->serial_number = WPS_SERIAL_NUM;//wndrv_get_serial_num();
        wpa_s->conf->device_type = 0;
        wpa_s->conf->eapol_version = 1;
        wpa_s->conf->fast_reauth = 1;
        wpa_s->ap_selection_mode = AP_AUTOMATIC_SELECT;
        wpa_printf(TRACE_GROUP_1, "start WPS conn_type=%d\n", local_para_ptr->connect_type);
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_START_TYPE,local_para_ptr->connect_type);
        wpas_wps_deinit(wpa_s);
        wpas_wps_init(wpa_s);
        wpa_printf(TRACE_GROUP_1, "in supc, wpa_s = %x, wpa_s->wps = %x\n",
                   wpa_s, wpa_s->wps);
        wpa_printf(TRACE_GROUP_1, "in supc, wps offset in wpa_s= %x\n", &(wpa_s->wps));
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_CONTEX_ADDR,wpa_s, wpa_s->wps);
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_CONTEX_ADDR_OFFSET,&(wpa_s->wps));
    }
#endif
    INNER_TRACE_STEP(2);

    /* RSNA Supplicant Key Management - INITIALIZE */
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);

    /* Joseph, init eap state machine */
    ctx = SUPC_ALLOC_BUFF(sizeof(*ctx));
    kal_mem_set(ctx, 0, sizeof(*ctx));
    ctx->ctx = wpa_s;
    ctx->msg_ctx = wpa_s;
    ctx->preauth = 0;
    ctx->eapol_done_cb = wpa_supplicant_notify_eapol_done;
    ctx->eapol_send = wpa_eapol_send;
    ctx->set_wep_key = wpa_eapol_set_wep_key;
    ctx->cb = wpa_supplicant_eapol_cb;
    ctx->cb_ctx = wpa_s;
#ifdef __WPS_SUPPORT__
    ctx->wps = wpa_s->wps;
    wpa_printf(TRACE_GROUP_1, "in supc, set ctx->wps=%x\n", ctx->wps);
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_SET_CTX_IN_WPS,ctx->wps);

#endif

#ifndef WIFI_AUTH_PSK_ONLY
#ifdef __CERTMAN_SUPPORT__

    /*MAUI_03055418 create a common ssl context used by eap.globalCtx */
    if (wpa_s->support_pre_auth) {
        if (0 != supc_ssl_init((ssl_ctx **) & (wpa_s->g_ssl_ctx),
                               (supc_ssl_SessionDB **) & (wpa_s->g_ssl_db))) {
            wpa_printf(TRACE_GROUP_1, "supc fail to create ssl ctx\n");
            kal_trace(DBG_EAP_SM, INFO_SUPC_EAP_CREATE_SSL_CTX_FAIL);
            wpa_s->g_ssl_ctx = NULL;
            wpa_s->g_ssl_db = NULL;
        }
    }
#endif
#endif
    wpa_s->eapol = eapol_sm_init(ctx);

    SUPC_ASSERT(wpa_s->eapol != NULL);

    /* 2. TBD */
    wpa_s->wpa_state = WPA_SCANNING;
    wpa_s->handover_timer_value = local_para_ptr->handover_timer_value;
    wpa_s->wpa_scan_retry_counter = 0;
    wpa_s->scan_interval_index = 0;
    wpa_s->is_reported_mmi_fail = KAL_FALSE;
    wpa_s->is_wpa_connect_req_pending = KAL_TRUE;  // Karen
    kal_mem_set(wpa_s->prev_bssid, 0, ETH_ALEN);
    wpa_s->num_of_disconnect = 0xFF;
#if 1//#ifdef __CERTMAN_SUPPORT__
    wpa_s->selected_ssid = NULL;
    wpa_s->selected_bssid = NULL;
    wpa_s->pending_cert_req = 0;
    wpa_s->rx_disconnect_req = KAL_FALSE;
    wpa_s->tx_disconnect_cnf = KAL_FALSE;
#endif
    wpa_s->check_cert = KAL_FALSE;
    //add for WiFi UE enhancement
    wpa_s->eap_fail_cause = SUPC_ABM_TIMEOUT;
    wpa_s->phase2_nak = 0;
    wpa_s->rx_identity = 0;

#ifdef __WIFI_CHIP_DYM_POWERON__
    SUPC_ASSERT(wpa_s->chip_state == CHIP_POWER_OFF);
#endif
    //supc_send_wndrv_disjoin_network_req();

    if (wpa_s->wpa_state == WPA_DISCONNECTED) {
        wpa_s->wpa_state = WPA_SCANNING;
    }

#ifdef __WPS_SUPPORT__

    if (START_WPS_PBC == wpa_s->connection_type) {
        wpas_wps_start_pbc(wpa_s, NULL, local_para_ptr->profile_header_ptr[0].ssid,
                           local_para_ptr->profile_header_ptr[0].ssid_len);
        goto done;
    } else if (START_WPS_PIN == wpa_s->connection_type) {
        wpas_wps_start_pin(wpa_s, NULL, local_para_ptr->profile_header_ptr[0].ssid,
                           local_para_ptr->profile_header_ptr[0].ssid_len, (char *) & (local_para_ptr->profile_header_ptr[0].pin));
        goto done;
    }
#endif
    INNER_TRACE_STEP(3);

    /* J: send site_survey_req first */
    /*
    wpa_s->num_scan_results = 0;
    supc_send_wndrv_site_survey_req();
    */

    /* J: Prepare the local parameters for the join_network_req */
    /* J: Handle the profiles */
    if (wpa_s->conf == NULL) {
        wpa_s->conf = SUPC_ALLOC_BUFF(sizeof(wpa_config));
    } else {
        wpa_ssid *pCurrent = NULL;

        /* J: free up memory inside wpa_conf */
        /* J: Remember to set the last ssid->next to NULL */
        while (wpa_s->conf->ssid != NULL) {
            supc_wpa_free_ssid(wpa_s->conf->ssid);
            pCurrent = wpa_s->conf->ssid;
            wpa_s->conf->ssid = wpa_s->conf->ssid->next;
            SUPC_DEALLOC_BUFF(pCurrent);
        }
    }

    kal_mem_set(wpa_s->conf, 0 , sizeof(wpa_config));
    wpa_conf = wpa_s->conf;

    /* J: Defautl settings */
    wpa_conf->eapol_version = 1;
    wpa_conf->fast_reauth = 1;

    profile_ptr = wpa_s->profile;
    for (i = 0; i < wpa_s->num_of_profile; i++) {
        ssid = supc_read_profiles(profile_ptr, i);

        if (head == NULL) {
            head = tail = ssid;
        } else {
            tail->next = ssid;
            tail = ssid;
        }

        if (wpa_conf->pssid[ssid->priority] == NULL) {
            wpa_conf->pssid[ssid->priority] = ssid;
        } else {
            ptail = wpa_conf->pssid[ssid->priority];
            while (ptail->pnext != NULL) {
                ptail = ptail->pnext;
            }
            // J: ptail->pnext shall be null now
            ptail->pnext = ssid;
        }
        profile_ptr++;
    }
    INNER_TRACE_STEP(4);

    wpa_conf->ssid = head;

    supc_dbg_dump_config();
    supc_dbg_dump_supplicant();

    for (prio = 0; prio < SUPC_MAX_SSID_CONFIG_PRIORITY && wpa_s->conf; prio++) {
        if (wpa_s->conf->pssid[prio] != NULL) {
            wpa_s->prev_scan_ssid = wpa_s->conf->pssid[prio];
            wpa_s->num_scan_results = 0;
            wpa_s->is_site_survey_req_pending = KAL_TRUE; /*Kinki add*/
#ifdef __WIFI_CHIP_DYM_POWERON__
            if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
                wpa_s->chip_state = CHIP_POWER_ON;
                supc_send_wndrv_chip_poweron_req();
            }
#endif
            supc_send_wndrv_site_survey_req(wpa_s->prev_scan_ssid->ssid,
                                            wpa_s->prev_scan_ssid->ssid_len);
            break;
        }
    }
    INNER_TRACE_STEP(5);

    if (wpa_s->num_of_profile) {
        if (wpa_s->num_of_profile >= 2) {
            wpa_s->report_mmi_timer_event_id = supc_start_report_mmi_timer(REPORT_MMI_TIMEOUT * 2);
        } else {
            wpa_s->report_mmi_timer_event_id = supc_start_report_mmi_timer(REPORT_MMI_TIMEOUT);
        }
    } else {
        wpa_printf(TRACE_GROUP_1, "bad input, and generate a fake  conn_cnf to MMI\n");
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_FAKE_CONF_TO_MMI);
        wpa_s->report_mmi_timer_event_id = supc_start_report_mmi_timer(5);
    }
done:
    if (MAP_MSG_ID_TST_INJECT_STRING == ilm_ptr->msg_id) {
        free_local_para((local_para_struct *)local_para_ptr);
    }
    INNER_TRACE_STEP(6);
    return;
}


/* Joseph:
 * 1. Change wap state
 * 2. Remove unnecessary information in the context
 *   a. bssid
 *   b. Profiles from ABM
 *   c. Keys
 * 3. Reinit eapol state
 */

/*********************************************************************
   Function Name    : supc_abm_wifi_disconnect_req_handler
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : ilm_struct *ilm_ptr
   Description      :
*********************************************************************/
void supc_abm_wifi_disconnect_req_handler(ilm_struct *ilm_ptr)
{
#if 1//#ifdef __CERTMAN_SUPPORT__
    wpa_s->rx_disconnect_req = KAL_TRUE;

    wpa_printf(TRACE_GROUP_1, "pending_cert_req = %d, tx_disconenct_cnf = %d\n", wpa_s->pending_cert_req, wpa_s->tx_disconnect_cnf);
//  kal_trace(DBG_CONN,INFO_SUPC_PENDING_CERT_IN_DISCONN_REQ,wpa_s->pending_cert_req, wpa_s->tx_disconnect_cnf);
    if ((wpa_s->pending_cert_req == 0) && (!(wpa_s->tx_disconnect_cnf))) {
        supc_do_disconnect_req();
#ifdef __WPS_SUPPORT__
        if (START_WPS_PBC == wpa_s->connection_type ||
                START_WPS_PIN == wpa_s->connection_type) {
            if (wpa_s->need_abm_conn_cnf) {
                supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;

                cnf_local_para_ptr = construct_local_para(
                                         sizeof(supc_abm_wifi_connect_cnf_struct), TD_RESET);

                cnf_local_para_ptr->cause = SUPC_ABM_CONNECT_FAIL;
                cnf_local_para_ptr->result = KAL_FALSE;
                /*clear wps data*/
                cnf_local_para_ptr->passphase_len = 0;
                cnf_local_para_ptr->conn_type = wpa_s->connection_type;
                cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

                wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
                supc_send_abm_wifi_connect_cnf((local_para_struct *)cnf_local_para_ptr);
                wpa_s->need_abm_conn_cnf = KAL_FALSE;
            }
            wpas_wps_deinit(wpa_s);
            supc_cb_wps_status(WPS_ADAPTER_STATE_OFF);
            wpa_printf(TRACE_GROUP_1, "clean wpa context in eapol and eap\n");
//          kal_trace(DBG_WPS,INFO_SUPC_CLEAN_EAPOL_CONTEXT);
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
        if (wpa_s->is_wpa_connect_req_pending == KAL_TRUE) {
            supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;

            cnf_local_para_ptr = construct_local_para(
                                     sizeof(supc_abm_wifi_connect_cnf_struct), TD_RESET);

            cnf_local_para_ptr->cause = SUPC_ABM_CONNECT_FAIL;
            cnf_local_para_ptr->result = KAL_FALSE;
            wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
            supc_send_abm_wifi_connect_cnf((local_para_struct *)cnf_local_para_ptr);
        }
        supc_send_abm_wifi_disconnect_cnf();
        wpa_s->tx_disconnect_cnf = KAL_TRUE;
    }
#else
    supc_do_disconnect_req();
    supc_send_abm_wifi_disconnect_cnf();
#endif
#ifdef _SUPC_CHECK_MEM_LEAK
    supc_check_mem_leak();
#endif

    return;
}
#if 0
void supc_disconnect_network(void)
{
    u8 *addr = NULL;

    wpa_printf(MSG_WARNING, "supc_disconnect_network, bssid=" MACSTR, MAC2STR(wpa_s->bssid));
    wpa_supplicant_cancel_scan();
    wpa_supplicant_cancel_auth_timeout(wpa_s);
    wpa_supplicant_cancel_handover_timer();
    wpa_supplicant_cancel_countermeasure();


    if (kal_mem_cmp(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) != 0) {
        addr = wpa_s->bssid;
    }

    wpa_clear_keys(wpa_s, addr);
    kal_mem_cpy(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN);
#ifdef __WIFI_CHIP_DYM_POWERON__
    if (wpa_s->chip_state == CHIP_POWER_ON) { //Kinki add for power-saving
        supc_send_wndrv_disjoin_network_req(); //Kinki add for power-saving

        wpa_s->chip_state = CHIP_POWER_OFF;
        supc_send_wndrv_chip_poweroff_req();
    }
#else
    supc_send_wndrv_disjoin_network_req();
#endif
    wpa_s->wpa_state = WPA_DISCONNECTED;

    wpa_s->current_ssid = NULL;

    eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);


    return;
}

#endif
