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


#ifndef WPA_SUPPLICANT_I_H
#define WPA_SUPPLICANT_I_H

#include "driver.h"
//#include "Fs_type.h" // Kinki add
//add for WiFi UE enhancement
#include "eap_defs.h"
#include "supc_abm_msgs.h"

#include "config_ssid.h"
#include "supc_config.h"

#ifdef EAPOL_TEST
#include <netinet/in.h>
#include "wpa.h"


struct hostapd_radius_server {
    struct in_addr addr;
    int port;
    u8 *shared_secret;
    wpa_size_t shared_secret_len;
};
#endif /* EAPOL_TEST */
// Kinki add
/*mark for not used yet
typedef struct file_info{
    FS_DOSDateTime dateTime;
    UINT          size;
} file_info;
*/
#define PMKID_LEN 16
#define WPA_MAX_SCAN_RETRY (3)


// timeout values
#define AUTH_TIMEOUT_IEEE80211 10 /*mtk80707 change from 5 to 10*/
#define TIMEOUT_EAP_IDREQ 5/*Kinki changed from 1 to 5*/
#define AUTH_TIMEOUT_IEEE8021X 15
#define AUTH_TIMEOUT_NOIEEE8021X 10 /*mtk80707 change from 5 to 10*/
#define REPORT_MMI_TIMEOUT 30

typedef struct rsn_pmksa_cache {
    struct rsn_pmksa_cache *next;
    u8 pmkid[PMKID_LEN];
    u8 pmk[PMK_LEN];
    wpa_size_t pmk_len;
    eventid event_id;
    //time_t expiration;
    int akmp; /* WPA_KEY_MGMT_* */
    u8 aa[ETH_ALEN];
} rsn_pmksa_cache;


typedef struct rsn_pmksa_candidate {
    struct rsn_pmksa_candidate *next;
    u8 bssid[ETH_ALEN];
    int priority;
} rsn_pmksa_candidate;


struct wpa_ptk {
    u8 mic_key[16]; /* EAPOL-Key MIC Key (MK) */
    u8 encr_key[16]; /* EAPOL-Key Encryption Key (EK) */
    u8 tk1[16]; /* Temporal Key 1 (TK1) */
    union {
        u8 tk2[16]; /* Temporal Key 2 (TK2) */
        struct {
            u8 tx_mic_key[8];
            u8 rx_mic_key[8];
        } auth;
    } u;
} ;


#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
struct wpa_blacklist {
    struct wpa_blacklist *next;
    u8 bssid[ETH_ALEN];
    kal_uint32 profile_id;
};

struct wpa_auth_timeout_list {
    struct wpa_auth_timeout_list *next;
    u8 bssid[ETH_ALEN];
    kal_uint32 auth_timeout_num;
    kal_uint32 profile_id;
};

struct wpa_success_list { // Kinki add for MAUI_00401655
    struct wpa_success_list *next;
    u8 bssid[ETH_ALEN];
    kal_uint32 profile_id;
};

#else

struct wpa_blacklist {
    struct wpa_blacklist *next;
    u8 bssid[ETH_ALEN];
};

#endif
typedef struct {
    //stack_timer_struct       wpa_base_timer;
    event_scheduler          *wpa_event_scheduler_ptr;
} wpa_timer_context_struct;


typedef struct wpa_supplicant {
    struct wpa_supplicant *head;
    struct wpa_supplicant *next;
    struct l2_packet_data *l2;
    unsigned char own_addr[ETH_ALEN];
    char *ifname;
    supc_ap_selection_mode_enum ap_selection_mode;
#ifdef CONFIG_XSUPPLICANT_IFACE
    int dot1x_s; /* socket for connection to Xsupplicant */
    int ext_pmk_received; /* 1 = PMK was received from Xsupplicant */
#endif /* CONFIG_XSUPPLICANT_IFACE */

    kal_uint32 connection_rssi;     // used to record current connection rssi
    kal_uint8 connection_max_rate;  //used to record current connection max data rate
    /* Joseph add */
    kal_bool  is_mmi_req_ap_list;
    kal_bool is_reported_mmi_fail;
    eventid scan_timer_event_id;
    eventid auth_timer_event_id;
    eventid preauth_timer_event_id;
    eventid handover_timer_event_id;
    eventid countermeasure_timer_event_id;
    eventid report_mmi_timer_event_id;
#if 1  //#ifdef __WAPI_SUPPORT__
    eventid wapi_auth_timer_event_id;
#endif
    u8   wpa_scan_retry_counter;
    kal_uint8 handover_timer_value;
    kal_uint8 join_session_id;
    kal_uint32 scan_interval_index;
    kal_uint32 num_of_disconnect;

    /*Kinki add*/
    kal_bool is_site_survey_req_pending;
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    eventid temporal_blacklist_timer_event_id;
    eventid permanent_blacklist_timer_event_id;
    struct wpa_blacklist *temporal_blacklist;
    struct wpa_blacklist *permanent_blacklist;
    struct wpa_auth_timeout_list *auth_timeout_list;
    kal_uint32 temporal_num;
    kal_uint32 permanent_num;
    kal_uint32 auth_timeout_bssid_num;
    struct wpa_success_list *success_list; // Kinki add for MAUI_00401655 *start*
    kal_uint32 success_num; // Kinki add for MAUI_00401655 *end*
#endif

    /* mark for not used yet
        file_info caCertInfo[MAX_NUM_PROFILE];
        file_info clientCertInfo[MAX_NUM_PROFILE];
        file_info privateKeyCertInfo[MAX_NUM_PROFILE];
        file_info caCertInfo2[MAX_NUM_PROFILE];
        file_info clientCertInfo2[MAX_NUM_PROFILE];
        file_info privateKeyCertInfo2[MAX_NUM_PROFILE];
    */
    supc_connection_type_enum connection_type; //Kinki add for IBSS power-saving
#if 1//#ifdef __CERTMAN_SUPPORT__
    kal_int32 pending_cert_req; // Kinki add for certman //Kinki change : kal_uint32 -> kal_int32 to capture the missing error handling
    kal_uint32 last_trans_id;
    kal_uint32 current_phase1_trans_id;
    kal_uint32 current_phase2_trans_id;
    kal_bool rx_disconnect_req;
    kal_bool tx_disconnect_cnf;
    kal_bool rx_deinit_req;
    kal_bool tx_deinit_cnf;
#endif
    kal_bool check_cert; //Kinki
    u8 pmk[PMK_LEN];
    wpa_size_t pmk_len;
    u8 snonce[WPA_NONCE_LEN];
    u8 anonce[WPA_NONCE_LEN]; /* ANonce from the last 1/4 msg */
    struct wpa_ptk ptk, tptk;
    int ptk_set, tptk_set;
    int renew_snonce;
    char *confname;
    struct wpa_config *conf;
    u8 request_counter[WPA_REPLAY_COUNTER_LEN];
    int countermeasures;
    u32 last_michael_mic_error;
    u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
    int rx_replay_counter_set;
    u8 bssid[ETH_ALEN];
    u8 prev_bssid[ETH_ALEN];
    u8 ssid[WNDRV_SSID_MAX_LEN];
    wpa_size_t ssid_len;
    kal_uint32   channel_number;
    int reassociate; /* reassociation requested */
    struct wpa_ssid *current_ssid;
#if 1//#ifdef __CERTMAN_SUPPORT__
    struct wpa_ssid *selected_ssid; // Kinki : add for certman
    wndrv_supc_bss_info_struct *selected_bssid; // Kinki : add for certman
#endif
    u8 *ap_wpa_ie, *ap_rsn_ie;
#if 1 //#ifdef __WAPI_SUPPORT__
    u8 *ap_wapi_ie;
#endif
    wpa_size_t ap_wpa_ie_len, ap_rsn_ie_len;
#if 1 //#ifdef __WAPI_SUPPORT__
    wpa_size_t ap_wapi_ie_len;
#endif
    u8 *assoc_wpa_ie;
    wpa_size_t assoc_wpa_ie_len;
    kal_bool need_to_send_init_cnf;
    kal_bool imsi_ready;

    /* Selected configuration (based on Beacon/ProbeResp WPA IE) */
    int proto;
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
#if CONFIG_DRIVER_MTK
    int app_wpa_cap;
#endif

    void *drv_priv; /* private data used by driver_ops */

    struct wpa_ssid *prev_scan_ssid; /* previously scanned SSID;
                      * NULL = not yet initialized (start
                      * with broadcast SSID)
                      * BROADCAST_SSID_SCAN = broadcast
                      * SSID was used in the previous scan
                      */
#define BROADCAST_SSID_SCAN ((struct wpa_ssid *) 1)

    //struct wpa_scan_result *scan_results;
    //int num_scan_results;
    wndrv_supc_bss_info_struct scan_results[WNDRV_MAX_SCAN_RESULTS_NUM];
    kal_uint8     num_scan_results;
    wifi_data_account_profile_struct profile[MAX_NUM_PROFILE];
    kal_uint8     num_of_profile;
    struct wpa_driver_ops *driver;

    int interface_removed; /* whether the network interface has been
                * removed */
    struct eapol_sm *eapol;

    int ctrl_sock; /* UNIX domain socket for control interface or -1 if
            * not used */
    struct wpa_ctrl_dst *ctrl_dst;

    enum {
        WPA_DEINITED, WPA_DISCONNECTED, WPA_SCANNING, WPA_ASSOCIATING,
        WPA_ASSOCIATED, WPA_4WAY_HANDSHAKE, WPA_GROUP_HANDSHAKE,
        WPA_COMPLETED, WPA_EAPOL_DONE
    } wpa_state;

#ifdef __WIFI_CHIP_DYM_POWERON__
    enum {
        CHIP_POWER_OFF, CHIP_POWER_ON
    } chip_state;
#endif

    kal_bool is_wpa_connect_req_pending;

    struct rsn_pmksa_cache *pmksa; /* PMKSA cache */
    int pmksa_count; /* number of entries in PMKSA cache */
    struct rsn_pmksa_cache *cur_pmksa; /* current PMKSA entry */
    struct rsn_pmksa_candidate *pmksa_candidates;

    struct l2_packet_data *l2_preauth;
    u8 preauth_bssid[ETH_ALEN]; /* current RSN pre-auth peer or
                     * 00:00:00:00:00:00 if no pre-auth is
                     * in progress */
    struct eapol_sm *preauth_eapol;

    int eapol_received; /* number of EAPOL packets received after the
                 * previous association event */

#if 1 //#ifdef __WAPI_SUPPORT__
    struct l2_packet_data *l2_wapi;
    kal_uint32  cap_check[4];
#endif

    u8 imsi[9];
    wpa_size_t imsi_len;
    u8 imsi_mnc_digits;//mnc digits for eap sim/aka
    u8 eap_sim_id[16];
#ifdef _SIM_BLOCK_FUNC_SUPPORT_
    kal_eventgrpid supc_sim_block_ev_id;
#endif //_SIM_BLOCK_FUNC_SUPPORT_
    struct scard_data *scard;

    unsigned char last_eapol_src[ETH_ALEN];

    int keys_cleared;

    struct wpa_blacklist *blacklist;

    /* add for WiFi UE enhancement */
    supc_abm_cause_enum eap_fail_cause;
    EapType last_eap_method;
    u8 phase2_nak;
    u8 rx_identity;
#ifdef EAPOL_TEST
    u8 radius_identifier;
    struct radius_msg *last_recv_radius;
    struct in_addr own_ip_addr;
    struct radius_client_data *radius;

    /* RADIUS Authentication and Accounting servers in priority order */
    struct hostapd_radius_server *auth_servers, *auth_server;
    int num_auth_servers;
    struct hostapd_radius_server *acct_servers, *acct_server;
    int num_acct_servers;

    int radius_retry_primary_interval;
    int radius_acct_interim_interval;

    u8 *last_eap_radius; /* last received EAP Response from Authentication
                  * Server */
    wpa_size_t last_eap_radius_len;

    u8 authenticator_pmk[PMK_LEN];
    wpa_size_t authenticator_pmk_len;
    int radius_access_accept_received;
    int radius_access_reject_received;
    int auth_timed_out;

    u8 *eap_identity;
    wpa_size_t eap_identity_len;
#endif /* EAPOL_TEST */
#if 1 //#ifdef __WPS_SUPPORT__
    struct wps_context *wps;
    int wps_success; /* WPS success event received. it is important flag,
                       while decide to whether add an AP into blacklist*/
    int blacklist_cleared;
    eventid wps_walk_timer_id;
    eventid wps_registrar_pbc_timer_id;
    eventid wps_reg_set_selected_timer_id;
    kal_bool need_abm_conn_cnf;
    kal_bool wps_eapol_cb_ret; /*return value of wpas_wps_eapol_cb*/
#endif
    /*mtk80707, MAUI_02964683
    to prevent site_survey_ind from being discarded by wndrv.
    in roaming case, this indication may be discarded in wndrv's SM.
    it a known issue in internal test*/
    eventid scan_monitor_timer_id;
    /*MAUI_03055418. ssl context is created independly on
    EAPOL and preauth EAPOL SM. both eapol sm use the same ssl context
    (eap.globalCtx)*/
    void *g_ssl_ctx;
    void *g_ssl_db;
    kal_bool support_pre_auth;// support pre-authentication or not(get it from custom file)

} wpa_supplicant ;


/* wpa_supplicant.c */

void wpa_supplicant_req_scan(struct wpa_supplicant *wpa_s, int sec, int usec);

void wpa_supplicant_cancel_scan(void);
void wpa_supplicant_cancel_countermeasure(void);

void wpa_supplicant_disassociate(struct wpa_supplicant *wpa_s,
                                 int reason_code);
void wpa_supplicant_deauthenticate(struct wpa_supplicant *wpa_s,
                                   int reason_code);

void wpa_supplicant_req_auth_timeout(struct wpa_supplicant *wpa_s,
                                     int sec, int usec);

void wpa_supplicant_cancel_auth_timeout(struct wpa_supplicant *wpa_s);
#ifdef __WAPI_SUPPORT__
void wpa_supplicant_cancel_wapi_auth_timeout(struct wpa_supplicant *wpa_s);
#endif
int wpa_supplicant_reload_configuration(struct wpa_supplicant *wpa_s);

int wpa_supplicant_get_beacon_ie(struct wpa_supplicant *wpa_s);
void wpa_supplicant_eapol_cb(struct eapol_sm *eapol, int success, void *ctx);


/* wpa.c */
void wpa_supplicant_key_request(struct wpa_supplicant *wpa_s,
                                int error, int pairwise);

struct wpa_ie_data {
    int proto;
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
    int capabilities;
    int num_pmkid;
    u8 *pmkid;
};

int wpa_parse_wpa_ie(struct wpa_supplicant *wpa_s, u8 *wpa_ie,
                     wpa_size_t wpa_ie_len, struct wpa_ie_data *data);

int wpa_gen_wpa_ie(struct wpa_supplicant *wpa_s, u8 *wpa_ie);
#ifdef __WAPI_SUPPORT__
int wpa_gen_wapi_ie(struct wpa_supplicant *wpa_s, u8 *wapi_ie);
int wpa_parse_wapi_ie(struct wpa_supplicant *wpa_s, u8 *wapi_ie,
                      wpa_size_t wapi_ie_len, struct wpa_ie_data *data);
#endif

void wpa_supplicant_rx_eapol(void *ctx, unsigned char *src_addr,
                             unsigned char *buf, wpa_size_t len);

struct wpa_ssid *wpa_supplicant_get_ssid(struct wpa_supplicant *wpa_s);

void pmksa_cache_free(struct wpa_supplicant *wpa_s);
struct rsn_pmksa_cache *pmksa_cache_get(struct wpa_supplicant *wpa_s,
                                        u8 *aa, u8 *pmkid);
int pmksa_cache_list(struct wpa_supplicant *wpa_s, char *buf, wpa_size_t len);
void pmksa_candidate_free(struct wpa_supplicant *wpa_s);

int wpa_get_mib(struct wpa_supplicant *wpa_s, char *buf, wpa_size_t buflen);

struct wpa_scan_result;
#ifdef IEEE8021X_EAPOL
int rsn_preauth_init(struct wpa_supplicant *wpa_s, u8 *dst);
void rsn_preauth_deinit(struct wpa_supplicant *wpa_s);
void rsn_preauth_scan_results(struct wpa_supplicant *wpa_s,
                              wndrv_supc_bss_info_struct *results, int count);
void pmksa_candidate_add(struct wpa_supplicant *wpa_s, const u8 *bssid,
                         int prio);
#else /* IEEE8021X_EAPOL */
__KAL_INLINE__ int rsn_preauth_init(struct wpa_supplicant *wpa_s, u8 *dst)
{
    return -1;
}

__KAL_INLINE__ void rsn_preauth_deinit(struct wpa_supplicant *wpa_s)
{
}
__KAL_INLINE__ void rsn_preauth_scan_results(struct wpa_supplicant *wpa_s,
        wndrv_supc_bss_info_struct *results,
        int count)
{
}

__KAL_INLINE__ void pmksa_candidate_add(struct wpa_supplicant *wpa_s,
        const u8 *bssid,
        int prio)
{
}
#endif /* IEEE8021X_EAPOL */

void wpa_supplicant_notify_eapol_done(void *ctx);

void wpa_clear_keys(struct wpa_supplicant *wpa_s, u8 *addr);

void wpa_supplicant_mark_disassoc(struct wpa_supplicant *wpa_s);

int wpa_blacklist_add(struct wpa_supplicant *wpa_s, const u8 *bssid);
int wpa_blacklist_del(struct wpa_supplicant *wpa_s, const u8 *bssid);
void wpa_blacklist_clear(struct wpa_supplicant *wpa_s);

void wpa_supplicant_scan_results(struct wpa_supplicant *wpa_s);
int wpa_supplicant_driver_init(struct wpa_supplicant *wpa_s,
                               int wait_for_interface);
void wpa_supplicant_associate(struct wpa_supplicant *wpa_s,
                              struct wndrv_supc_bss_info_struct *bss,
                              struct wpa_ssid *ssid);
int wpa_supplicant_set_suites(struct wpa_supplicant *wpa_s,
                              struct wndrv_supc_bss_info_struct *bss,
                              struct wpa_ssid *ssid,
                              u8 *wpa_ie, int *wpa_ie_len);

int wpa_supplicant_suites_from_assoc_ie(struct wpa_supplicant *wpa_s, u8 *assoc_ie, wpa_size_t assoc_ie_len); // Kinki add for MAUI_00526025

#ifdef __WAPI_SUPPORT__
int wapi_supplicant_suites_from_assoc_ie(struct wpa_supplicant *wpa_s, u8 *assoc_ie, wpa_size_t assoc_ie_len);
#endif
/**
 * wpa_eapol_send - send IEEE 802.1X EAPOL packet to the Authenticator
 * @ctx: pointer to wpa_supplicant data
 * @type: IEEE 802.1X packet type (IEEE802_1X_TYPE_*)
 * @buf: EAPOL payload (after IEEE 802.1X header)
 * @len: EAPOL payload length
 *
 * This function adds Ethernet and IEEE 802.1X header and sends the EAPOL frame
 * to the current Authenticator or in case of pre-authentication, to the peer
 * of the authentication.
 */
int wpa_eapol_send(void *ctx, int type, u8 *buf, wpa_size_t len);
int wpa_eapol_send_preauth(void *ctx, int type, u8 *buf, wpa_size_t len);
extern void pmksa_cache_free_entry(struct rsn_pmksa_cache *entry);

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
kal_uint32 wpa_auth_timeout_num_get(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id);
int wpa_auth_timeout_list_add(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id);
int wpa_auth_timeout_list_del(struct wpa_supplicant *wpa_s, const u8 *bssid , kal_uint32 profile_id);


void wpa_auth_timeout_list_clear(struct wpa_supplicant *wpa_s);
int wpa_blacklist_add_temporal(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id); // Kinki add for MAUI_00416432

void wpa_blacklist_clear_temporal(struct wpa_supplicant *wpa_s);
int wpa_blacklist_add_permanent(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id);


void wpa_blacklist_clear_permanent(struct wpa_supplicant *wpa_s);
int wpa_listed_success(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id); //Kinki add for MAUI_00401655
int wpa_listed_add_success(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id); //Kinki add for MAUI_00416432

void wpa_listed_clear_success(struct wpa_supplicant *wpa_s);
#endif
int wpa_supplicant_get_scan_results(struct wpa_supplicant *wpa_s);

#endif /* WPA_SUPPLICANT_I_H */
