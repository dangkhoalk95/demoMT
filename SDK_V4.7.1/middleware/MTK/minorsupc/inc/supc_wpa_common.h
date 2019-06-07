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

/* ****************************************************************************
* Alternatively, this software may be distributed under the terms of BSD
* license.
*****************************************************************************/



//#include "Certman_struct.h"
#include "supc_wps.h"
#include "supc_wps_defs.h"


#define CERTMAN_MAX_TRANS_ID 1000

typedef enum {
    CA_CERT, CLIENT_CERT, PRIVATE_KEY_CERT, CA_CERT_2, CLIENT_CERT_2, PRIVATE_KEY_CERT_2
} cert_type_enum;


extern void supc_preauth_timer_expiry_handler(void *);
extern void supc_stop_preauth_timer(void); // Kinki change for stop preauth timer
extern eventid supc_start_preauth_timer(kal_uint32 delay);


extern void supc_stop_wpa_timer(void);
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
extern void supc_stop_temporal_blacklist_timer(void);
extern void supc_stop_permanent_blacklist_timer(void);
#endif
extern eventid supc_start_wpa_scan_timer(kal_uint32 delay);

extern eventid supc_start_report_mmi_timer(kal_uint32 delay);

extern void supc_wpa_scan_timer_expiry_handler(void *para_ptr);

extern void supc_report_mmi_timer_expiry_handler(void *para_ptr);

extern void supc_wpa_auth_timer_expiry_handler(void *para_ptr);
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
extern void supc_wpa_temporal_blacklist_timer_expiry_handler(void *para_ptr);
extern void supc_wpa_permanent_blacklist_timer_expiry_handler(void *para_ptr);
#endif
extern eventid supc_start_wpa_auth_timer(kal_uint32 delay);

extern eventid supc_start_pmksa_cache_timer(rsn_pmksa_cache *pmksa_cache_ptr, kal_uint32 delay);

extern eventid supc_start_wpa_handover_timer(kal_uint32 delay);

extern eventid supc_start_wpa_countermeasure_timer(kal_uint32 delay);

extern void supc_pmksa_timer_expiry_handler(void *para_ptr);

extern void supc_send_abm_wifi_disconnect_ind(supc_abm_cause_enum cause);

extern void supc_send_abm_wifi_disconnect_cnf(void);

extern void supc_send_abm_wifi_connect_cnf(local_para_struct *local_para_ptr);

extern void supc_send_abm_wifi_connect_ind(local_para_struct *local_para_ptr);

//extern void supc_send_abm_wifi_activate_rej(supc_abm_cause_enum cause);


extern void supc_send_wndrv_update_pmkid_req(local_para_struct *local_para_ptr);

extern void supc_send_abm_wifi_deinit_cnf(void);

extern void supc_mic_failure_handler(int pairwise);

extern void supc_send_wndrv_data_req(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr);

extern void supc_send_wndrv_init_req(void);

extern void supc_send_wndrv_auth_state_update_req(local_para_struct *local_para_ptr);
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
extern void supc_send_wndrv_blacklist_update_req(local_para_struct *local_para_ptr);
#endif
extern void supc_send_wndrv_deinit_req(void);

extern void supc_send_wndrv_add_key_req(local_para_struct *local_para_ptr);


extern void supc_send_mmi_wifi_curr_ap_info_ind(local_para_struct *local_para_ptr);


extern void supc_send_abm_wifi_ap_list_cnf(kal_uint8 num_of_bss, wndrv_supc_bss_info_struct *bss_info, kal_uint8 *ap_num);

extern void supc_send_abm_connection_info_cnf();

extern void supc_send_abm_wps_get_credential_cnf();

extern void supc_send_abm_wifi_init_cnf(void);

extern void supc_send_sim_read_imsi_req(void);


extern void supc_send_wndrv_join_network_req(local_para_struct *local_para_ptr);


extern void supc_send_wndrv_disjoin_network_req(void);

/*mtk80707 WPS probe*/
#ifdef __WPS_SUPPORT__
extern void supc_send_wndrv_wps_sit_survey_req(
    struct wps_context *wps_ctx, kal_bool ispbc,
    enum wps_request_type req_type);
struct wpabuf *wpa_scan_get_vendor_ie_multi(const kal_uint8 *ie_buf,
        kal_uint16 len, u32 vendor_type);
const kal_uint8 *wpa_scan_get_vendor_ie(const kal_uint8 *ie_buf,
                                        kal_uint16 len, u32 vendor_type);
void supc_stop_wps_walk_timer(void);
eventid supc_start_wps_walk_timer(kal_uint32 delay);
int wpas_wps_in_use(struct wpa_config *conf,
                    enum wps_request_type *req_type);
eventid supc_start_wps_registrar_pbc_timer(kal_uint32 delay);
void supc_stop_wps_registrar_pbc_timer(void);

eventid supc_start_wps_reg_set_selected_timer(kal_uint32 delay);
void supc_stop_wps_reg_set_selected_timer(void);
#endif

void supc_stop_scan_monitor_timer(void);
void supc_send_empty_ap_list_cnf(void);
eventid supc_start_scan_monitor_timer(kal_uint32 delay);

int wpa_config_add_prio_network(struct wpa_config *config,
                                struct wpa_ssid *ssid);
int wpa_config_update_prio_list(struct wpa_config *config);

struct wpa_ssid *wpa_config_add_network(struct wpa_config *config);
void wpa_config_free_ssid(struct wpa_ssid *ssid);
int wpa_config_remove_network(struct wpa_config *config, int id);
void supc_send_abm_wifi_wps_info_ind(kal_uint8 type, kal_uint8 sec,
                                     kal_uint32 pincode, kal_bool show);

void supc_send_abm_wifi_wps_pbc_status(kal_bool overlapped);

/*end mtk80707*/

extern void supc_send_wndrv_site_survey_req(kal_uint8 *ssid, kal_uint16 ssid_len);

extern void supc_wpa_free_ssid(wpa_ssid *ssid);

extern void supc_do_disconnect_req(void);// Kinki add for disconnect req

extern int supc_cmp_fileInfo(wifi_data_account_profile_struct *profile_ptr, kal_uint8 id);  //Kinki add
extern wpa_ssid *supc_read_profiles(wifi_data_account_profile_struct *profile_ptr, kal_uint8 id);
extern void supc_reinit_wpa(void);


extern u8 *supc_get_eap_mothod_from_profile(kal_uint32 eap, kal_uint16 *num_methods);
extern u8 *supc_get_phase2_eap_mothods(kal_uint32 eap, kal_uint16 *num_methods);
extern void supc_stop_handover_timer(void);
extern void supc_stop_report_mmi_timer(void);
extern void supc_stop_auth_timer(void);
extern void supc_filter_duplicated_ap(kal_uint8 total_num_of_bss, wndrv_supc_bss_info_struct *bss_info,
                                      kal_uint8 *num_of_diff_ap, wndrv_supc_bss_info_struct *filter_result, kal_uint8 *ap_num); /*Kinki: number of APs in a SSID*/
extern void supc_stop_pmksa_cache_timer(rsn_pmksa_cache *pmksa_cache_ptr);
extern void supc_stop_scan_timer(void);
extern void supc_stop_countermeasure_timer(void);

extern wpa_supplicant supc_context_g;
extern wpa_supplicant *wpa_s;
extern wpa_timer_context_struct wpa_timer_context_g;
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
extern eventid supc_start_permanent_blacklist_timer(kal_uint32 delay);
extern eventid supc_start_temporal_blacklist_timer(kal_uint32 delay);
extern void supc_blacklist_update(struct wpa_supplicant *wpa_s);
#endif
#ifdef __WIFI_CHIP_DYM_POWERON__
extern void supc_send_wndrv_chip_poweron_req(void);
extern void supc_send_wndrv_chip_poweroff_req(void);
#endif
extern void supc_do_deinit_req(void);
#if 0//#ifdef __CERTMAN_SUPPORT__
extern kal_int32 supc_get_cert_and_privkey(wpa_ssid *ssid); // Kinki add for CertMan
extern void supc_set_private_key_cb(kal_uint32 trans_id, kal_uint32 cert_id,
                                    const kal_uint8 *pwd, kal_uint32 size);
extern void certman_send_get_cert_by_id_req(module_type src_mod, kal_uint32 trans_id,
        kal_uint32 cert_id, certman_encoding_enum encoding, kal_uint32 key_purpose, const kal_wchar *output_dir);
extern void certman_send_get_privkey_req_auto_accept(module_type src_mod, kal_uint32 trans_id,
        kal_uint32 cert_id, certman_fp_pwd_cb pwd_cb, certman_encoding_enum encoding,
        const kal_uint8 *user_specified_pwd, const kal_wchar *output_dir);
extern void certman_send_get_cert_chain_req(module_type src_mod,
        kal_uint32 trans_id, kal_uint32 cert_id, kal_uint32 key_purpose, const kal_wchar *output_dir);
#endif
supc_auth_mode_enum supc_get_auth_mode(int key_mgmt, int proto, int auth_alg);
supc_encrypt_mode_enum supc_get_encrypt_mode(int cipher);

void supc_dbg_show_ssid_ex(wpa_ssid *ssid);
void supc_dbg_dump_supplicant(void);
void supc_dbg_show_current_ssid(void);
void supc_dbg_dump_config(void);
kal_bool supc_allow_autoconn(void);




