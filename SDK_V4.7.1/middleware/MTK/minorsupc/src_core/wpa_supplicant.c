
/**
 * Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  - Neither the name of the the name(s) of the above-listed copyright holder(s) nor the names of
 *    its contributors may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif



#if 0
#include <stdio.h>
#include <stdlib.h>             /* rand() */
#include "kal_release.h"        /* Basic data type */
#include "kal_non_specific_general_types.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "stack_ltlcom.h"
#include "app_buff_alloc.h"
#endif
//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "supc_common.h"
//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */

//#include "supc_trc.h"
#include "eapol_sm.h"
#include "eap.h"
#include "wpa.h"
#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "driver.h"
#include "eloop.h"
#include "wpa_supplicant.h"
#include "config_ssid.h"
#include "supc_config.h"
#include "l2_packet.h"
#include "supc_abm_msgs.h"
#include "wpa_supplicant_i.h"
#include "supc_version.h"

#include "wndrv_supc_msg.h"
#include "supc_wpa_common.h"

#include "driver_mtk_i.h"
#include "driver_mtk.h"

#include "stdarg.h"

#ifdef __WAPI_SUPPORT__
#include "interface_inout.h"
#include <stdlib.h>//add for rand func
#endif

/*mtk80707 FOR WPS*/
#ifdef __WPS_SUPPORT__
#include "supc_wps.h"
#include "wps_i.h"
#include "wpabuf.h"
#include "wps_supplicant.h"

#endif

static const char *wpa_supplicant_version =
    "wpa_supplicant v" VERSION_STR "\n"
    "Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi> and contributors";

static const char *wpa_supplicant_license =
    "This program is SUPC_DEALLOC_BUFF software. You can distribute it and/or modify it\n"
    "under the terms of the GNU General Public License version 2.\n"
    "\n"
    "Alternatively, this software may be distributed under the terms of the\n"
    "BSD license. See README and COPYING for more details.\n"
#ifdef EAP_TLS_FUNCS
    "\nThis product includes software developed by the OpenSSL Project\n"
    "for use in the OpenSSL Toolkit (http://www.openssl.org/)\n"
#endif /* EAP_TLS_FUNCS */
    ;

static const char *wpa_supplicant_full_license =
    "This program is SUPC_DEALLOC_BUFF software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License version 2 as\n"
    "published by the Free Software Foundation.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program; if not, write to the Free Software\n"
    "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
    "\n"
    "Alternatively, this software may be distributed under the terms of the\n"
    "BSD license.\n"
    "\n"
    "Redistribution and use in source and binary forms, with or without\n"
    "modification, are permitted provided that the following conditions are\n"
    "met:\n"
    "\n"
    "1. Redistributions of source code must retain the above copyright\n"
    "   notice, this list of conditions and the following disclaimer.\n"
    "\n"
    "2. Redistributions in binary form must reproduce the above copyright\n"
    "   notice, this list of conditions and the following disclaimer in the\n"
    "   documentation and/or other materials provided with the distribution.\n"
    "\n"
    "3. Neither the name(s) of the above-listed copyright holder(s) nor the\n"
    "   names of its contributors may be used to endorse or promote products\n"
    "   derived from this software without specific prior written permission.\n"
    "\n"
    "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
    "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
    "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n"
    "A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n"
    "OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
    "SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n"
    "LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
    "DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
    "THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
    "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
    "OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
    "\n";

extern struct wpa_driver_ops *wpa_supplicant_drivers[];
extern char *ssid_txt(kal_uint8 *input_ssid, kal_int16 ssid_len);

extern void supc_dbg_show_ssid_ex(wpa_ssid *ssid);

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


extern int wpa_debug_level;
extern int wpa_debug_show_keys;
extern int wpa_debug_timestamp;
extern kal_uint32     Protocol_Status4WLAN;

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
extern kal_uint32 supc_temporal_blacklist_timeout_sec(void);
extern kal_uint32 supc_permanent_blacklist_timeout_sec(void);
#endif
#ifdef __WAPI_SUPPORT__
extern void supc_stop_wapi_auth_timer(void);
extern eventid supc_start_wapi_auth_timer(kal_uint32 delay);
extern int supc_read_file(void *file, void *outbuf, int bufSz);
void wpa_supplicant_req_wapi_auth_timeout(struct wpa_supplicant *wpa_s,
        int sec, int usec);

extern void wpa_supplicant_wapi_key_neg_complete(struct wpa_supplicant *wpa_s,
        u8 *addr, int secure);
extern int prepare_cap_check(kal_uint32 *challenge, kal_uint32 *output);
#endif

void wpa_msg(struct wpa_supplicant *wpa_s, int level, char *fmt, ...)
{
#if 0
    va_list ap;
    char *buf;
    const kal_uint16 buflen = 512;

    buf = SUPC_ALLOC_BUFF(buflen);
    va_start(ap, fmt);
    //len = vsnprintf(buf, buflen, fmt, ap);
    // temp_len = kal_sprintf(buf, fmt, ap);
    vsprintf(buf, fmt, ap);
    //SUPC_ASSERT(buflen >= temp_len);
    va_end(ap);
    wpa_printf(MSG_INFO, "%s", buf);
    SUPC_DEALLOC_BUFF(buf);
#endif
}


int wpa_eapol_send(void *ctx, int type, u8 *buf, wpa_size_t len)
{
    struct wpa_supplicant *wpa_s = ctx;
    u8 *dst;
    struct ieee802_1x_hdr *hdr;
    wndrv_supc_data_req_struct *local_para_ptr = NULL;
    peer_buff_struct *peer_buff_ptr = NULL;
    kal_uint8 *pdu_ptr = NULL;
    kal_uint16 pdu_len = 0;

    /* TODO: could add l2_packet_sendmsg that allows fragments to avoid
    * extra copy here */

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_NONE) {
        /* Current SSID is not using IEEE 802.1X/EAP, so drop possible
        * EAPOL frames (mainly, EAPOL-Start) from EAPOL state
        * machines. */
        wpa_printf(TRACE_GROUP_1, "WPA: drop TX EAPOL in non-IEEE 802.1X "
                   "mode (type=%d len=%u)\n", type, (unsigned long) len);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_DROP_TX_EAPOL, type, (unsigned long) len);
        return -1;
    }
#if 0
    /* since in wpas_wps_eapol_cb, it modify the wpa_s' key_mgmt,
    it need not check whether to drop EAPOL_Start.
    at the same time, this code bring up a side-effect: it cause to drop
    WSC_DONE after recive M8
    */
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPS && wpa_s->current_ssid) {
        if (wpa_s->current_ssid->key_mgmt == WPA_KEY_MGMT_NONE) {
            wpa_printf(MSG_DEBUG, "WPA: drop TX EAPOL, curssid->key_megmt is open mode");
            return;
        }
    }
#endif
#ifdef WIFI_PORT_TIMER
    if (wpa_s->cur_pmksa && type == IEEE802_1X_TYPE_EAPOL_START && wpa_s->eapol_received > 0) { //Kinki change for the port timer
        /* Trying to use PMKSA caching - do not send EAPOL-Start frames
        * since they will trigger full EAPOL authentication. */
        wpa_printf(TRACE_GROUP_1, "RSN: PMKSA caching and have recieved frames- do not send "
                   "EAPOL-Start");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_DROP_TX_EAPOL_START_REC);
        return -1;
    }
#else
    if (wpa_s->cur_pmksa && type == IEEE802_1X_TYPE_EAPOL_START) {
        /* Trying to use PMKSA caching - do not send EAPOL-Start frames
        * since they will trigger full EAPOL authentication. */
        //wpa_printf(MSG_DBG_AUTH, "RSN: PMKSA caching - do not send "
        //"EAPOL-Start");
        return -1;
    }
#endif

    if (kal_mem_cmp(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) == 0) {
        /* The bssid information will be the same between network driver and Supc */
        wpa_printf(TRACE_GROUP_1, "BSSID not set when trying to send an "
                   "EAPOL frame\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_DROP_TX_EAPOL_BSSID_NOT_SET);
        return -1;
        /*
        wpa_printf(MSG_DEBUG, "BSSID not set when trying to send an "
        "EAPOL frame");
        if (wpa_drv_get_bssid(wpa_s, bssid) == 0 &&
            memcmp(bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) != 0)
        {
            dst = bssid;
            wpa_printf(MSG_DEBUG, "Using current BSSID " MACSTR
            " from the driver as the EAPOL destination",
            MAC2STR(dst));
        }
        else
        {
            dst = wpa_s->last_eapol_src;
            wpa_printf(MSG_DEBUG, "Using the source address of the"
                " last received EAPOL frame " MACSTR " as "
                "the EAPOL destination",
            MAC2STR(dst));
        }
        */
    } else {
        /* BSSID was already set (from (Re)Assoc event, so use it as
         * the EAPOL destination. */
        dst = wpa_s->bssid;
    }

    /* J: Fill ethernet information */
    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    kal_mem_cpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    kal_mem_cpy(local_para_ptr->dest_mac_addr, dst, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_EAPOL);

    /* Construct 802.1x payload including 802.1x header */
    /*peer_buff_ptr = construct_peer_buff((kal_uint16) (len + IEEE8021X_HEADER_LEN),
        WNDRV_FREE_HEADER_SPACE,
        0, TD_UL);*/
    peer_buff_ptr = construct_peer_buff((kal_uint16)(len + IEEE8021X_HEADER_LEN),
                                        WNDRV_FREE_HEADER_SPACE,
                                        0, TD_UL);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    hdr = (struct ieee802_1x_hdr *)(pdu_ptr);

    hdr->version = wpa_s->conf->eapol_version;
    hdr->type = type;
    hdr->length = htons(len);

    kal_mem_cpy((u8 *)(hdr + 1), buf, len);

    //wpa_hexdump(MSG_MSGDUMP, "TX EAPOL", (char *)peer_buff_ptr, (len + IEEE8021X_HEADER_LEN));
    if (Protocol_Status4WLAN == 0) {
        kal_sleep_task(1);
    }
    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);

    return KAL_TRUE;
}


int wpa_eapol_send_preauth(void *ctx, int type, u8 *buf, wpa_size_t len)
{
    struct wpa_supplicant *wpa_s = ctx;
    wpa_size_t msglen;
    struct ieee802_1x_hdr *hdr;
    wndrv_supc_data_req_struct *local_para_ptr = NULL;
    peer_buff_struct                *peer_buff_ptr = NULL;
    kal_uint8 *pdu_ptr = NULL;
    kal_uint16 pdu_len = 0;

    /* TODO: could add l2_packet_sendmsg that allows fragments to avoid
     * extra copy here */

    if (wpa_s->l2_preauth == NULL) {
        return -1;
    }

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    memcpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    memcpy(local_para_ptr->dest_mac_addr, wpa_s->preauth_bssid, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_RSN_PREAUTH);


    msglen = sizeof(*hdr) + len;
    //peer_buff_ptr = construct_peer_buff(msglen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    peer_buff_ptr = construct_peer_buff(msglen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    // hdr = (struct ieee802_1x_hdr *) (peer_buff_ptr);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    hdr = (struct ieee802_1x_hdr *)(pdu_ptr);

    hdr->version = wpa_s->conf->eapol_version;
    hdr->type = type;
    hdr->length = htons(len);

    memcpy((u8 *)(hdr + 1), buf, len);

    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);
    //res = l2_packet_send(wpa_s->l2_preauth, msg, msglen);
    //free(msg);
    return KAL_TRUE;
}


/**
 * wpa_eapol_set_wep_key - set WEP key for the driver
 * @ctx: pointer to wpa_supplicant data
 * @unicast: 1 = individual unicast key, 0 = broadcast key
 * @keyidx: WEP key index (0..3)
 * @key: pointer to key data
 * @keylen: key length in bytes
 *
 * Returns 0 on success or < 0 on error.
 */
int wpa_eapol_set_wep_key(void *ctx, int unicast, int keyidx,
                          u8 *key, wpa_size_t keylen)
{
    struct wpa_supplicant *wpa_s = ctx;
    wpa_s->keys_cleared = 0;
    return wpa_driver_mtk_set_key(wpa_s->drv_priv, WPA_ALG_WEP,
                                  0, keyidx, unicast, (u8 *) "", 0, key, keylen);
}




int wpa_supplicant_set_wpa_none_key(struct wpa_supplicant *wpa_s,
                                    struct wpa_ssid *ssid)
{
    u8 key[32];
    wpa_size_t keylen;
    wpa_alg alg;
    u8 seq[6] = { 0 };

    /* IBSS/WPA-None uses only one key (Group) for both receiving and
     * sending unicast and multicast packets. */

    if (ssid->mode != WNDRV_SUPC_NETWORK_IBSS) {
        wpa_printf(TRACE_GROUP_1, "WPA: Invalid mode %d (not IBSS/ad-hoc) "
                   "for WPA-None\n", ssid->mode);
//      kal_trace(DBG_CONN,INFO_SUPC_INVALID_MODE_WPA_NONE, ssid->mode);
        return -1;
    }

    if (!ssid->psk_set) {
        wpa_printf(TRACE_GROUP_1, "WPA: No PSK configured for WPA-None\n");
//      kal_trace(DBG_CONN,INFO_SUPC_NO_PSK_FOR_WPA_NONE);
        return -1;
    }

    switch (wpa_s->group_cipher) {
        case WPA_CIPHER_CCMP:
            kal_mem_cpy(key, ssid->psk, 16);
            keylen = 16;
            alg = WPA_ALG_CCMP;
            break;
        case WPA_CIPHER_TKIP:
            /* WPA-None uses the same Michael MIC key for both TX and RX */
            kal_mem_cpy(key, ssid->psk, 16 + 8);
            kal_mem_cpy(key + 16 + 8, ssid->psk + 16, 8);
            keylen = 32;
            alg = WPA_ALG_TKIP;
            break;
        default:
            wpa_printf(TRACE_GROUP_1, "WPA: Invalid group cipher %d for "
                       "WPA-None\n", wpa_s->group_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_INVALID_GROUP_CIPHER_WPA_NONE, wpa_s->group_cipher);
            return -1;
    }

    /* TODO: should actually remember the previously used seq#, both for TX
     * and RX from each STA.. */

    return wpa_driver_mtk_set_key(wpa_s->drv_priv, alg, (u8 *) "\xff\xff\xff\xff\xff\xff",
                                  0, 1, seq, 6, key, keylen);
}


void wpa_supplicant_notify_eapol_done(void *ctx)
{
    struct wpa_supplicant *wpa_s = ctx;
    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;
    supc_abm_wifi_connect_ind_struct *ind_local_para_ptr = NULL;
    wndrv_supc_auth_state_update_req_struct *req_local_para_ptr = NULL;

    /* 1. Send debug message EAPOL is complete
     * 2. Cancel supplicant scan timer
     * 3. Cancel auth timer
     */
    //wpa_msg(wpa_s, MSG_DBG_AUTH, "WPA: EAPOL processing complete");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_COMPLETE);
    wpa_s->wpa_state = WPA_EAPOL_DONE;
    //eloop_cancel_timeout(wpa_supplicant_scan, wpa_s, NULL);
    wpa_supplicant_cancel_auth_timeout(wpa_s);
    supc_stop_handover_timer();
#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif

    //req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
    req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_RESET);
    req_local_para_ptr->in_progress = KAL_FALSE;
    supc_send_wndrv_auth_state_update_req((local_para_struct *) req_local_para_ptr);

    if (KAL_FALSE == wpa_s->is_wpa_connect_req_pending) {
        if (kal_mem_cmp(wpa_s->bssid, wpa_s->prev_bssid, ETH_ALEN) != 0) {
            //ind_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_ind_struct), TD_UL);
            ind_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_ind_struct), TD_RESET);
            kal_mem_cpy(&(ind_local_para_ptr->bssid[0]), &(wpa_s->bssid[0]), WNDRV_MAC_ADDRESS_LEN);
            kal_mem_cpy(&(ind_local_para_ptr->ssid[0]), &(wpa_s->ssid[0]), wpa_s->ssid_len);
            ind_local_para_ptr->ssid_len = wpa_s->ssid_len;
            ind_local_para_ptr->channel_number = wpa_s->channel_number;
            ind_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;
            supc_send_abm_wifi_connect_ind((local_para_struct *) ind_local_para_ptr);
        }
    } else {
        wpa_s->is_wpa_connect_req_pending = KAL_FALSE;
        //cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
        cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
        cnf_local_para_ptr->result = KAL_TRUE;
        cnf_local_para_ptr->cause = SUPC_ABM_INVALID_CAUSE;
        kal_mem_cpy(&(cnf_local_para_ptr->bssid[0]), &(wpa_s->bssid[0]), WNDRV_MAC_ADDRESS_LEN);
        kal_mem_cpy(&(cnf_local_para_ptr->ssid[0]), &(wpa_s->ssid[0]), wpa_s->ssid_len);
        cnf_local_para_ptr->ssid_len = wpa_s->ssid_len;
        cnf_local_para_ptr->channel_number = wpa_s->channel_number;
        cnf_local_para_ptr->profile_id = wpa_s->current_ssid->profile_id;

        //add for WiFi UE enhancement
        cnf_local_para_ptr->auth_type          = (supc_auth_mode_enum) 0;
        cnf_local_para_ptr->eap_peap_auth_type = 0;
        cnf_local_para_ptr->eap_ttls_auth_type = 0;

        /*clear wps data*/
        cnf_local_para_ptr->passphase_len = 0;
        cnf_local_para_ptr->conn_type = wpa_s->connection_type;
        cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

        wpa_s->wpa_scan_retry_counter = 0xff;
        supc_stop_report_mmi_timer();
        supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);
    }

    kal_mem_cpy(wpa_s->prev_bssid, wpa_s->bssid, ETH_ALEN);
    wpa_s->num_of_disconnect = 0;
    wpa_printf(TRACE_GROUP_1, "set supplicant prev_ssid " MACSTR "\n",
               MAC2STR(wpa_s->preauth_bssid));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_SET_PREV_BSSID,MAC2STR(wpa_s->preauth_bssid));

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    wpa_listed_add_success(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id); //Kinki add for MAUI_00401655, MAUI_00416432
#endif
}



#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
kal_uint32 wpa_auth_timeout_num_get(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_auth_timeout_list *e;

    e = wpa_s->auth_timeout_list;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                //wpa_printf(MSG_DEBUG, "BSSID " MACSTR " is in auth timeout list of profile %d , Num=%d",
                //MAC2STR(bssid), profile_id, e->auth_timeout_num);
                kal_trace(DBG_TIMER, INFO_SUPC_BSSID_IN_AUTH_TIMEOUT_LIST, MAC2STR(bssid), profile_id, e->auth_timeout_num);
                return e->auth_timeout_num;
            }
        }
        e = e->next;
    }
    return 0;
}

int wpa_auth_timeout_list_add(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_auth_timeout_list *e, *f;

    f = wpa_s->auth_timeout_list;
    while (f) {
        if (memcmp(f->bssid, bssid, ETH_ALEN) == 0) {
            if (f->profile_id == profile_id) {
                f->auth_timeout_num++;
                //wpa_printf(MSG_DEBUG, "BSSID " MACSTR " is already in auth timeout list of profile %d, Num of timeout=%d",
                //MAC2STR(bssid), profile_id,  f->auth_timeout_num);
                kal_trace(DBG_TIMER, INFO_SUPC_BSSID_AL_IN_AUTH_TIMEOUT_LIST, MAC2STR(bssid), profile_id,  f->auth_timeout_num);
                return 0;
            }
        }
        f = f->next;
    }


    if (wpa_s->auth_timeout_bssid_num == WNDRV_BSSID_MAX_NUM) {
        //wpa_printf(MSG_DEBUG, "Auth timeout list is full !Num = %d",wpa_s->auth_timeout_bssid_num);
        kal_trace(DBG_TIMER, INFO_SUPC_AUTH_TIMEOUT_LIST_FULL, wpa_s->auth_timeout_bssid_num);
        return 0;
    }

    e = SUPC_ALLOC_BUFF(sizeof(*e));
    if (e == NULL) {
        return -1;
    }
    memset(e, 0, sizeof(*e));
    memcpy(e->bssid, bssid, ETH_ALEN);
    e->next = wpa_s->auth_timeout_list;
    e->auth_timeout_num = 1;
    e->profile_id = profile_id;

    wpa_s->auth_timeout_bssid_num++; // new bssid is added into auth_timeout_list
    wpa_s->auth_timeout_list = e;

    //wpa_printf(MSG_DEBUG, "Added BSSID " MACSTR " into auth timeout list of profile %d, Num of timeout = %d",
    // MAC2STR(bssid), profile_id, wpa_s->auth_timeout_list->auth_timeout_num);
    kal_trace(DBG_TIMER, INFO_SUPC_ADD_IN_AUTH_TIMEOUT_LIST, MAC2STR(bssid), profile_id, wpa_s->auth_timeout_list->auth_timeout_num);

    return 0;

}

int wpa_auth_timeout_list_del(struct wpa_supplicant *wpa_s, const u8 *bssid , kal_uint32 profile_id)
{
    struct wpa_auth_timeout_list *e, *prev = NULL;

    e = wpa_s->auth_timeout_list;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                wpa_s->auth_timeout_bssid_num--;

                if (prev == NULL) {
                    wpa_s->auth_timeout_list = e->next;
                } else {
                    prev->next = e->next;
                }
                //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
                //"auth timeout list of profile %d", MAC2STR(bssid), profile_id);
                kal_trace(DBG_TIMER, INFO_SUPC_REMOVE_AUTH_TIMEOUT_LIST, MAC2STR(bssid), profile_id);
                SUPC_DEALLOC_BUFF(e);
                return 0;
            }
        }
        prev = e;
        e = e->next;
    }

    return -1;
}

void wpa_auth_timeout_list_clear(struct wpa_supplicant *wpa_s)
{
    struct wpa_auth_timeout_list *e, *prev;

    e = wpa_s->auth_timeout_list;
    wpa_s->auth_timeout_list = NULL;
    while (e) {
        prev = e;
        e = e->next;
        //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
        //"auth timeout list of profile %d (clear)", MAC2STR(prev->bssid), prev->profile_id);

        kal_trace(DBG_CONN, INFO_SUPC_REMOVE_BSS_FROM_AUTH_TIMEOUT_LIST, MAC2STR(prev->bssid), prev->profile_id);
        SUPC_DEALLOC_BUFF(prev);
    }
    wpa_s->auth_timeout_bssid_num = 0;

}

int wpa_blacklisted_temporal(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_blacklist *e;

    e = wpa_s->temporal_blacklist;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                //wpa_printf(MSG_DEBUG, "BSSID " MACSTR " is already in temporal blacklist of profile %d",
                //MAC2STR(bssid), profile_id);
                kal_trace(DBG_CONN, INFO_SUPC_BSSIDIN_TBLACKLIST, MAC2STR(bssid), profile_id);

                return 1;
            }
        }
        e = e->next;
    }
    return 0;
}

int wpa_blacklist_add_temporal(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_blacklist *e;

    if (wpa_blacklisted_temporal(wpa_s, bssid, profile_id)) {
        return 0;
    }

    if (wpa_s->temporal_num == WNDRV_BSSID_MAX_NUM) {
        //wpa_printf(MSG_DEBUG, "Temporal blacklist is full!Num=%d",wpa_s->temporal_num);
        kal_trace(DBG_CONN, INFO_SUPC_TBLACKLIST_FULL, wpa_s->temporal_num);
        return 0;
    }

    e = SUPC_ALLOC_BUFF(sizeof(*e));
    if (e == NULL) {
        return -1;
    }
    memset(e, 0, sizeof(*e));
    memcpy(e->bssid, bssid, ETH_ALEN);
    e->profile_id = profile_id;
    e->next = wpa_s->temporal_blacklist;

    if (wpa_s->temporal_blacklist == NULL) {
        wpa_s->temporal_num = 1;
        wpa_s->temporal_blacklist_timer_event_id = supc_start_temporal_blacklist_timer(supc_temporal_blacklist_timeout_sec()); //Start a timer for 5 mins
    } else {
        wpa_s->temporal_num++;
    }

    wpa_s->temporal_blacklist = e;
    //wpa_printf(MSG_DEBUG, "Added BSSID " MACSTR " into temporal blacklist of profile %d",
    //MAC2STR(bssid), profile_id);
    kal_trace(DBG_CONN, INFO_SUPC_ADD_BSSID_TO_TBLACKLIST, MAC2STR(bssid), profile_id);

    return 0;
}

int wpa_blacklist_del_temporal(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_blacklist *e, *prev = NULL;

    e = wpa_s->temporal_blacklist;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                wpa_s->temporal_num--;

                if (prev == NULL) {
                    wpa_s->temporal_blacklist = e->next;
                } else {
                    prev->next = e->next;
                }
                //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
                //"temporal blacklist of profile %d", MAC2STR(bssid), profile_id);
                kal_trace(DBG_CONN, INFO_SUPC_REMOVE_BSSID_FROM_TBLACKLIST, MAC2STR(bssid), profile_id);
                SUPC_DEALLOC_BUFF(e);
                return 0;
            }
        }
        prev = e;
        e = e->next;
    }

    return -1;
}


void wpa_blacklist_clear_temporal(struct wpa_supplicant *wpa_s)
{
    struct wpa_blacklist *e, *prev;

    e = wpa_s->temporal_blacklist;
    wpa_s->temporal_blacklist = NULL;
    while (e) {
        prev = e;
        e = e->next;
        //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
        // "temporal blacklist of profile %d (clear)", MAC2STR(prev->bssid), prev->profile_id);
        kal_trace(DBG_CONN, INFO_SUPC_REMOVE_BSSID_FROM_TBLACKLIST, MAC2STR(prev->bssid), prev->profile_id);
        SUPC_DEALLOC_BUFF(prev);
    }
    wpa_s->temporal_num = 0;
    supc_stop_temporal_blacklist_timer();

}


int wpa_blacklisted_permanent(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_blacklist *e;

    e = wpa_s->permanent_blacklist;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                //wpa_printf(MSG_DEBUG, "BSSID " MACSTR " is already in permanent blacklist of profile %d",
                //MAC2STR(bssid), profile_id);
                kal_trace(DBG_CONN, INFO_SUPC_BSSID_AL_IN_P_BLACKLIST, MAC2STR(bssid), profile_id);
                return 1;
            }
        }
        e = e->next;
    }

    return 0;
}



int wpa_blacklist_add_permanent(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_blacklist *e;

    if (wpa_blacklisted_permanent(wpa_s, bssid, profile_id)) {
        return 0;
    }

    if (wpa_s->permanent_num == WNDRV_BSSID_MAX_NUM) {
        //wpa_printf(MSG_DEBUG, "Permanent blacklist is full!Num=%d",wpa_s->permanent_num);
        kal_trace(DBG_CONN, INFO_SUPC_P_BLACKLIST_FULL, wpa_s->permanent_num);
        return 0;
    }

    e = SUPC_ALLOC_BUFF(sizeof(*e));
    if (e == NULL) {
        return -1;
    }

    memset(e, 0, sizeof(*e));
    memcpy(e->bssid, bssid, ETH_ALEN);
    e->profile_id = profile_id;
    e->next = wpa_s->permanent_blacklist;

    if (wpa_s->permanent_blacklist == NULL) {
        wpa_s->permanent_num = 1;
        wpa_s->permanent_blacklist_timer_event_id = supc_start_permanent_blacklist_timer(supc_permanent_blacklist_timeout_sec()); //Start a timer for 5 mins
    } else {
        wpa_s->permanent_num++;
    }

    wpa_s->permanent_blacklist = e; // the num in the head represents how many BSSID in the permanent blacklist
    //wpa_printf(MSG_DEBUG, "Added BSSID " MACSTR " into permanent blacklist of profile %d",
    //MAC2STR(bssid), profile_id);
    kal_trace(DBG_CONN, INFO_SUPC_ADD_P_BLACKLIST, MAC2STR(bssid), profile_id);

    return 0;
}

int wpa_blacklist_del_permanent(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_blacklist *e, *prev = NULL;

    e = wpa_s->permanent_blacklist;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                wpa_s->permanent_num--;

                if (prev == NULL) {
                    wpa_s->permanent_blacklist = e->next;
                } else {
                    prev->next = e->next;
                }
                //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
                //"permanent blacklist of profile %d", MAC2STR(bssid), profile_id);
                kal_trace(DBG_CONN, INFO_SUPC_REMOVE_P_BLACKLIST, MAC2STR(bssid), profile_id);
                SUPC_DEALLOC_BUFF(e);
                return 0;
            }
        }
        prev = e;
        e = e->next;
    }
    return -1;
}


void wpa_blacklist_clear_permanent(struct wpa_supplicant *wpa_s)
{
    struct wpa_blacklist *e, *prev;

    e = wpa_s->permanent_blacklist;
    wpa_s->permanent_blacklist = NULL;
    while (e) {
        prev = e;
        e = e->next;
        //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
        //"permanent blacklist of profile %d (clear)", MAC2STR(prev->bssid), prev->profile_id);
        kal_trace(DBG_CONN, INFO_SUPC_CLR_P_BLACKLIST, MAC2STR(prev->bssid), prev->profile_id);
        SUPC_DEALLOC_BUFF(prev);
    }
    wpa_s->permanent_num = 0;
    supc_stop_permanent_blacklist_timer();
}

int wpa_listed_success(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_success_list *e;

    e = wpa_s->success_list;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (e->profile_id == profile_id) {
                //wpa_printf(MSG_DEBUG, "BSSID " MACSTR " is already in success list of profile %d",
                //MAC2STR(bssid), profile_id);
                kal_trace(DBG_CONN, INFO_SUPC_AL_IN_SUCC_LIST, MAC2STR(bssid), profile_id);

                return 1;
            }
        }
        e = e->next;
    }

    return 0;
}

int wpa_listed_add_success(struct wpa_supplicant *wpa_s, const u8 *bssid, kal_uint32 profile_id)
{
    struct wpa_success_list *e;

    if (wpa_listed_success(wpa_s, bssid, profile_id)) {
        return 0;
    }

    if (wpa_s->success_num == WNDRV_BSSID_MAX_NUM) {
        //wpa_printf(MSG_DEBUG, "Success list is full!Num=%d",wpa_s->success_num);
        kal_trace(DBG_CONN, INFO_SUPC_SUCC_LIST_FULL, wpa_s->success_num);
        return 0;
    }

    e = SUPC_ALLOC_BUFF(sizeof(*e));
    if (e == NULL) {
        return -1;
    }

    memset(e, 0, sizeof(*e));
    memcpy(e->bssid, bssid, ETH_ALEN);
    e->profile_id = profile_id;
    e->next = wpa_s->success_list;

    if (wpa_s->success_list == NULL) {
        wpa_s->success_num = 1;
    } else {
        wpa_s->success_num++;
    }

    wpa_s->success_list = e;
    //wpa_printf(MSG_DEBUG, "Added BSSID " MACSTR " into success list of profile %d",
    //MAC2STR(bssid), profile_id);
    kal_trace(DBG_CONN, INFO_SUPC_ADD_IN_SUCC_LIST, MAC2STR(bssid), profile_id);

    return 0;
}

void wpa_listed_clear_success(struct wpa_supplicant *wpa_s)
{
    struct wpa_success_list *e, *prev;

    e = wpa_s->success_list;
    wpa_s->success_list = NULL;
    while (e) {
        prev = e;
        e = e->next;
        //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
        //"success list of profile %d (clear)", MAC2STR(prev->bssid), prev->profile_id);
        kal_trace(DBG_CONN, INFO_SUPC_CLR_SUCC_LIST, MAC2STR(prev->bssid), prev->profile_id);
        SUPC_DEALLOC_BUFF(prev);
    }
    wpa_s->success_num = 0;
}
#else

int wpa_blacklisted(struct wpa_supplicant *wpa_s, const u8 *bssid)
{
    struct wpa_blacklist *e;

    e = wpa_s->blacklist;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            return 1;
        }
        e = e->next;
    }

    return 0;
}


int wpa_blacklist_add(struct wpa_supplicant *wpa_s, const u8 *bssid)
{
    struct wpa_blacklist *e;

    if (wpa_blacklisted(wpa_s, bssid)) {
        return 0;
    }

    e = SUPC_ALLOC_BUFF(sizeof(*e));
    if (e == NULL) {
        return -1;
    }
    memset(e, 0, sizeof(*e));
    memcpy(e->bssid, bssid, ETH_ALEN);
    e->next = wpa_s->blacklist;
    wpa_s->blacklist = e;
    //wpa_printf(MSG_DEBUG, "Added BSSID " MACSTR " into blacklist",
    //MAC2STR(bssid));

    return 0;
}


int wpa_blacklist_del(struct wpa_supplicant *wpa_s, const u8 *bssid)
{
    struct wpa_blacklist *e, *prev = NULL;

    e = wpa_s->blacklist;
    while (e) {
        if (memcmp(e->bssid, bssid, ETH_ALEN) == 0) {
            if (prev == NULL) {
                wpa_s->blacklist = e->next;
            } else {
                prev->next = e->next;
            }
            //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
            //"blacklist", MAC2STR(bssid));
            SUPC_DEALLOC_BUFF(e);
            return 0;
        }
        prev = e;
        e = e->next;
    }
    return -1;
}


void wpa_blacklist_clear(struct wpa_supplicant *wpa_s)
{
    struct wpa_blacklist *e, *prev;

    e = wpa_s->blacklist;
    wpa_s->blacklist = NULL;
    while (e) {
        prev = e;
        e = e->next;
        //wpa_printf(MSG_DEBUG, "Removed BSSID " MACSTR " from "
        //"blacklist (clear)", MAC2STR(prev->bssid));
        SUPC_DEALLOC_BUFF(prev);
    }
}
#endif


const char *wpa_ssid_txt(u8 *ssid, wpa_size_t ssid_len)
{
    static char ssid_txt[MAX_SSID_LEN + 1];
    char *pos;

// for compile warning removal
    (void) wpa_supplicant_version;
    (void) wpa_supplicant_license;
    (void) wpa_supplicant_full_license;

    if (ssid_len > MAX_SSID_LEN) {
        ssid_len = MAX_SSID_LEN;
    }
    memcpy(ssid_txt, ssid, ssid_len);
    ssid_txt[ssid_len] = '\0';
    for (pos = ssid_txt; *pos != '\0'; pos++) {
        if ((u8) *pos < 32 || (u8) *pos >= 127) {
            *pos = '_';
        }
    }
    return ssid_txt;
}

kal_uint8 *evs_ptr;

void wpa_supplicant_req_scan(struct wpa_supplicant *wpa_s, int sec, int usec)
{
    wpa_printf(TRACE_GROUP_1, "Setting scan request: %d sec %d usec\n",
               sec, usec);
//    kal_trace(DBG_CONN,INFO_SUPC_REQ_SCAN_SETTING,sec, usec);
    supc_stop_scan_timer();
    supc_stop_auth_timer();

    if (wpa_s->wpa_scan_retry_counter != 0xff) {
        wpa_s->wpa_scan_retry_counter++;
    }

    wpa_printf(TRACE_GROUP_1, "now scan retry %d times\n", wpa_s->wpa_scan_retry_counter);
//  kal_trace(DBG_CONN,INFO_SUPC_REQ_SCAN_TIMES, wpa_s->wpa_scan_retry_counter);
    wpa_s->scan_timer_event_id = supc_start_wpa_scan_timer(sec);
    /*
        temp mark to build pass
        evs_ptr = (kal_uint8 *)(&(((event *)(((kal_uint8*)(wpa_s->scan_timer_event_id)) + 12))->owneres));
    */
    //eloop_cancel_timeout(wpa_supplicant_scan, wpa_s, NULL);
    //eloop_register_timeout(sec, usec, wpa_supplicant_scan, wpa_s, NULL);
}


void wpa_supplicant_cancel_scan()
{
    wpa_printf(TRACE_GROUP_1, "Cancelling scan request\n");

//  kal_trace(DBG_SCAN, INFO_SUPC_CANCELL_SCAN);

    supc_stop_scan_timer();
    //eloop_cancel_timeout(wpa_supplicant_scan, wpa_s, NULL);
}



void wpa_supplicant_cancel_countermeasure()
{
    supc_stop_countermeasure_timer();
    wpa_s->countermeasures = 0;
}

void wpa_supplicant_cancel_handover_timer()
{
    supc_stop_handover_timer();
}

void wpa_supplicant_req_auth_timeout(struct wpa_supplicant *wpa_s,
                                     int sec, int usec)
{
    wpa_printf(TRACE_GROUP_1, "Setting authentication timeout: %d sec "
               "%d usec\n", sec, usec);
//  kal_trace(DBG_TIMER,INFO_SUPC_REQ_AUTH_TIMEOUT, sec, usec);
    /* Joseph:
     * Use event schedule to replace eloop
     */
    supc_stop_auth_timer();

    wpa_s->auth_timer_event_id  = supc_start_wpa_auth_timer(sec);
}


void wpa_supplicant_cancel_auth_timeout(struct wpa_supplicant *wpa_s)
{
    //wpa_msg(wpa_s, MSG_DEBUG, "Cancelling authentication timeout");
    wpa_printf(TRACE_GROUP_1, "Cancelling authentication timeout\n");
//  kal_trace(DBG_TIMER,INFO_SUPC_CANCEL_AUTH_TIMER);
    //eloop_cancel_timeout(wpa_supplicant_timeout, wpa_s, NULL);
    supc_stop_auth_timer();
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    //wpa_blacklist_del_temporal(wpa_s, wpa_s->bssid);
    //wpa_blacklist_del_permanent(wpa_s, wpa_s->bssid);
    //supc_blacklist_update(wpa_s);
#else
    wpa_blacklist_del(wpa_s, wpa_s->bssid);
#endif
}

#ifdef __WAPI_SUPPORT__
/* start a timer for WAPI. invoked by WAPI IP interface.c */
void wapi_start_timer(int sec, int usec, int period)
{
    //wpa_printf(MSG_DEBUG, "wapi_start_timert: %d sec %d usec %d period\n",
    //sec, usec, period);
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_START_TIMER, sec, usec, period);

    /* use 1 sec */
    wpa_supplicant_req_wapi_auth_timeout(wpa_s, 1, 0);
}

/* start a timer for WAPI. invoked by WAPI IP interface.c */
void wapi_stop_timer(void)
{
    //wpa_printf(MSG_DEBUG, "wapi_stop_timer\n");
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_STOP_TIMER);
    wpa_supplicant_cancel_wapi_auth_timeout(wpa_s);
}

void wpa_supplicant_req_wapi_auth_timeout(struct wpa_supplicant *wpa_s,
        int sec, int usec)
{
    //wpa_printf(MSG_DEBUG, "Setting WAPI authentication timeout: %d sec "
    //"%d usec", sec, usec);
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_START_AUTH_TIMER, sec, usec);
    /* Joseph:
     * Use event schedule to replace eloop
     */
    supc_stop_wapi_auth_timer();

    wpa_s->wapi_auth_timer_event_id  = supc_start_wapi_auth_timer(sec);
}


void wpa_supplicant_cancel_wapi_auth_timeout(struct wpa_supplicant *wpa_s)
{
    //wpa_msg(wpa_s, MSG_DEBUG, "Cancelling authentication timeout");
    //wpa_printf(MSG_DEBUG, "Cancelling authentication timeout");
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_CANCEL_AUTH_TIMER);
    //eloop_cancel_timeout(wpa_supplicant_timeout, wpa_s, NULL);
    supc_stop_wapi_auth_timer();
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    //wpa_blacklist_del_temporal(wpa_s, wpa_s->bssid);
    //wpa_blacklist_del_permanent(wpa_s, wpa_s->bssid);
    //supc_blacklist_update(wpa_s);
#else
    wpa_blacklist_del(wpa_s, wpa_s->bssid);
#endif
}
#endif

static void wpa_supplicant_initiate_eapol(struct wpa_supplicant *wpa_s)
{
    struct eapol_config eapol_conf;
    struct wpa_ssid *ssid = wpa_s->current_ssid;

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
        eapol_sm_notify_eap_success(wpa_s->eapol, FALSE);
        eapol_sm_notify_eap_fail(wpa_s->eapol, FALSE);
    }

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        eapol_sm_notify_portControl(wpa_s->eapol, ForceAuthorized);
    } else {
        eapol_sm_notify_portControl(wpa_s->eapol, Auto);
    }

    memset(&eapol_conf, 0, sizeof(eapol_conf));

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
        eapol_conf.accept_802_1x_keys = 1;
        eapol_conf.required_keys = 0;
        if (ssid->eapol_flags & EAPOL_FLAG_REQUIRE_KEY_UNICAST) {
            eapol_conf.required_keys |= EAPOL_REQUIRE_KEY_UNICAST;
        }
        if (ssid->eapol_flags & EAPOL_FLAG_REQUIRE_KEY_BROADCAST) {
            eapol_conf.required_keys |=
                EAPOL_REQUIRE_KEY_BROADCAST;
        }
    }
    eapol_conf.fast_reauth = wpa_s->conf->fast_reauth;
    eapol_conf.workaround = ssid->eap_workaround;
    eapol_sm_notify_config(wpa_s->eapol, ssid, &eapol_conf);
}


static void wpa_supplicant_set_non_wpa_policy(struct wpa_supplicant *wpa_s,
        struct wpa_ssid *ssid)
{
    wpa_printf(TRACE_GROUP_1, "ssid: key_mgmt=%d pair_cipher=%d gp_cipher=%d\n",
               ssid->key_mgmt,
               ssid->pairwise_cipher,
               ssid->group_cipher);

//  kal_trace(DBG_CONN,INFO_SUPC_SSID_KEY_MGMT,ssid->key_mgmt,ssid->pairwise_cipher,ssid->group_cipher);
    if (ssid->key_mgmt & WPA_KEY_MGMT_WPS) {
        wpa_s->key_mgmt = WPA_KEY_MGMT_WPS;
    }

    else if (ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
        wpa_s->key_mgmt = WPA_KEY_MGMT_IEEE8021X_NO_WPA;
    } else {
        wpa_s->key_mgmt = WPA_KEY_MGMT_NONE;
    }
    if (wpa_s->ap_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
    }
    wpa_s->ap_wpa_ie = NULL;
    wpa_s->ap_wpa_ie_len = 0;
    if (wpa_s->ap_rsn_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
    }
    wpa_s->ap_rsn_ie = NULL;
    wpa_s->ap_rsn_ie_len = 0;
    if (wpa_s->assoc_wpa_ie) {
        SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
    }
    wpa_s->assoc_wpa_ie = NULL;
    wpa_s->assoc_wpa_ie_len = 0;
    //wpa_s->pairwise_cipher = WPA_CIPHER_NONE;
    //wpa_s->group_cipher = WPA_CIPHER_NONE;
    /*in wpa_supplicant_wps_cred, it does not change ssid's cipher in
    case of open mode.
    so the wpa_s's cipher is not update. */
    wpa_s->pairwise_cipher = ssid->pairwise_cipher;;
    wpa_s->group_cipher = ssid->group_cipher;

    wpa_s->cur_pmksa = NULL;
}

#if 0
static void wpa_supplicant_cleanup(struct wpa_supplicant *wpa_s)
{
    scard_deinit(wpa_s->scard);
    wpa_s->scard = NULL;
    eapol_sm_register_scard_ctx(wpa_s->eapol, NULL);
    //l2_packet_deinit(wpa_s->l2);
    SUPC_DEALLOC_BUFF(wpa_s->l2);
    wpa_s->l2 = NULL;

#ifdef CONFIG_XSUPPLICANT_IFACE
    if (wpa_s->dot1x_s > -1) {
        close(wpa_s->dot1x_s);
        wpa_s->dot1x_s = -1;
    }
#endif /* CONFIG_XSUPPLICANT_IFACE */

    wpa_supplicant_ctrl_iface_deinit(wpa_s);
    if (wpa_s->conf != NULL) {
        wpa_config_free(wpa_s->conf);
        wpa_s->conf = NULL;
    }

    SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
    wpa_s->assoc_wpa_ie = NULL;

    SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
    wpa_s->ap_wpa_ie = NULL;
    SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
    wpa_s->ap_rsn_ie = NULL;

    SUPC_DEALLOC_BUFF(wpa_s->confname);
    wpa_s->confname = NULL;

    eapol_sm_deinit(wpa_s->eapol);
    wpa_s->eapol = NULL;

    rsn_preauth_deinit(wpa_s);

    pmksa_candidate_free(wpa_s);
    pmksa_cache_free(wpa_s);
#ifdef WIFI_BLACKLIST
    wpa_auth_timeout_list_clear(wpa_s);
    wpa_blacklist_clear_temporal(wpa_s);
    wpa_blacklist_clear_permanent(wpa_s);
    supc_blacklist_update(wpa_s);
#else
    wpa_blacklist_clear(wpa_s);
#endif
    wpa_s->num_scan_results = 0;
}
#endif

void wpa_clear_keys(struct wpa_supplicant *wpa_s, u8 *addr)
{
    //u8 *bcast = (u8 *) "\xff\xff\xff\xff\xff\xff";

    return; // Karen, since driver already removed key by itself
#if 0
    if (wpa_s->keys_cleared) {
        /* Some drivers (e.g., ndiswrapper & NDIS drivers) seem to have
         * timing issues with keys being cleared just before new keys
         * are set or just after association or something similar. This
         * shows up in group key handshake failing often because of the
         * client not receiving the first encrypted packets correctly.
         * Skipping some of the extra key clearing steps seems to help
         * in completing group key handshake more reliably. */
        wpa_printf(MSG_DEBUG, "No keys have been configured - "
                   "skip key clearing");
        return;
    }
    wpa_driver_mtk_remove_key(wpa_s->drv_priv, 0, bcast, bcast, KAL_FALSE);
    wpa_driver_mtk_remove_key(wpa_s->drv_priv, 1, bcast, bcast, KAL_FALSE);
    wpa_driver_mtk_remove_key(wpa_s->drv_priv, 2, bcast, bcast, KAL_FALSE);
    wpa_driver_mtk_remove_key(wpa_s->drv_priv, 3, bcast, bcast, KAL_FALSE);
    if (addr) {
        wpa_driver_mtk_remove_key(wpa_s->drv_priv, 0, addr, addr, KAL_TRUE);
    }

    wpa_s->keys_cleared = 1;
#endif
}

#if 0
static void wpa_supplicant_stop_countermeasures(void *eloop_ctx,
        void *sock_ctx)
{
    struct wpa_supplicant *wpa_s = eloop_ctx;

    //whsu
    wpa_printf(MSG_DEBUG, "wpa_supplicant_stop_countermeasures\n");
    if (wpa_s->countermeasures) {
        wpa_s->countermeasures = 0;
        //wpa_drv_set_countermeasures(wpa_s, 0);
        wpa_msg(wpa_s, MSG_INFO, "WPA: TKIP countermeasures stopped");
        wpa_supplicant_req_scan(wpa_s, 0, 0);
    }
}
#endif

void wpa_supplicant_mark_disassoc(struct wpa_supplicant *wpa_s)
{
    wpa_s->wpa_state = WPA_DISCONNECTED;
    kal_mem_set(wpa_s->bssid, 0, ETH_ALEN);
    kal_mem_set(wpa_s->prev_bssid, 0, ETH_ALEN);
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
        eapol_sm_notify_eap_success(wpa_s->eapol, FALSE);
    }
}


void wpa_find_assoc_pmkid(struct wpa_supplicant *wpa_s)
{
    struct wpa_ie_data ie;
    int i;

    if (wpa_parse_wpa_ie(wpa_s, wpa_s->assoc_wpa_ie,
                         wpa_s->assoc_wpa_ie_len, &ie) < 0 ||
            ie.pmkid == NULL) {
        return;
    }

    for (i = 0; i < ie.num_pmkid; i++) {
        wpa_s->cur_pmksa = pmksa_cache_get(wpa_s, NULL,
                                           ie.pmkid + i * PMKID_LEN);
        if (wpa_s->cur_pmksa) {
            eapol_sm_notify_pmkid_attempt(wpa_s->eapol, 1);
            break;
        }
    }

    wpa_printf(TRACE_GROUP_1, "RSN: PMKID from assoc IE %sfound from PMKSA "
               "cache", wpa_s->cur_pmksa ? "\n" : "not \n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_PMKID_FOUND_RESULT, wpa_s->cur_pmksa);

}

#if 0
static void wpa_supplicant_add_pmkid_candidate(struct wpa_supplicant *wpa_s,
        union wpa_event_data *data)
{
    if (data == NULL) {
        wpa_printf(MSG_DEBUG, "RSN: No data in PMKID candidate event");
        return;
    }
    wpa_printf(MSG_DEBUG, "RSN: PMKID candidate event - bssid=" MACSTR
               " index=%d preauth=%d",
               MAC2STR(data->pmkid_candidate.bssid),
               data->pmkid_candidate.index,
               data->pmkid_candidate.preauth);

    if (!data->pmkid_candidate.preauth) {
        wpa_printf(MSG_DEBUG, "RSN: Ignored PMKID candidate without "
                   "preauth flag");
        return;
    }

    pmksa_candidate_add(wpa_s, data->pmkid_candidate.bssid,
                        data->pmkid_candidate.index);
}
#endif

int wpa_supplicant_dynamic_keys(struct wpa_supplicant *wpa_s)
{
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        return 0;
    }

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA &&
            wpa_s->current_ssid &&
            !(wpa_s->current_ssid->eapol_flags &
              (EAPOL_FLAG_REQUIRE_KEY_UNICAST |
               EAPOL_FLAG_REQUIRE_KEY_BROADCAST))) {
        /* IEEE 802.1X, but not using dynamic WEP keys (i.e., either
         * plaintext or static WEP keys). */
        return 0;
    }

    return 1;
}

#if 0
static void wpa_supplicant_associnfo(struct wpa_supplicant *wpa_s,
                                     union wpa_event_data *data)
{
    int l, len;
    u8 *p;

    wpa_printf(MSG_DEBUG, "Association info event");
    wpa_hexdump(MSG_DEBUG, "req_ies", (char *)data->assoc_info.req_ies,
                data->assoc_info.req_ies_len);
    wpa_hexdump(MSG_DEBUG, "resp_ies", (char *)data->assoc_info.resp_ies,
                data->assoc_info.resp_ies_len);
    if (wpa_s->assoc_wpa_ie) {
        SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
        wpa_s->assoc_wpa_ie = NULL;
        wpa_s->assoc_wpa_ie_len = 0;
    }

    p = data->assoc_info.req_ies;
    l = data->assoc_info.req_ies_len;

    /* Go through the IEs and make a copy of the WPA/RSN IE, if present. */
    while (l >= 2) {
        len = p[1] + 2;
        if (len > l) {
            wpa_hexdump(MSG_DEBUG, "Truncated IE in assoc_info",
                        (char *)p, (wpa_size_t)l);
            break;
        }
        if ((p[0] == GENERIC_INFO_ELEM && p[1] >= 6 &&
                (memcmp(&p[2], "\x00\x50\xF2\x01\x01\x00", 6) == 0)) ||
                (p[0] == RSN_INFO_ELEM && p[1] >= 2)) {
            wpa_s->assoc_wpa_ie = SUPC_ALLOC_BUFF((kal_uint16)len);
            if (wpa_s->assoc_wpa_ie == NULL) {
                break;
            }
            wpa_s->assoc_wpa_ie_len = len;
            memcpy(wpa_s->assoc_wpa_ie, p, len);
            wpa_hexdump(MSG_DEBUG, "assoc_wpa_ie",
                        (char *)wpa_s->assoc_wpa_ie,
                        wpa_s->assoc_wpa_ie_len);
            wpa_find_assoc_pmkid(wpa_s);
            break;
        }
        l -= len;
        p += len;
    }

    /* WPA/RSN IE from Beacon/ProbeResp */
    SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
    wpa_s->ap_wpa_ie = NULL;
    wpa_s->ap_wpa_ie_len = 0;
    SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
    wpa_s->ap_rsn_ie = NULL;
    wpa_s->ap_rsn_ie_len = 0;

    p = data->assoc_info.beacon_ies;
    l = data->assoc_info.beacon_ies_len;

    /* Go through the IEs and make a copy of the WPA/RSN IEs, if present.
     */
    while (l >= 2) {
        len = p[1] + 2;
        if (len > l) {
            wpa_hexdump(MSG_DEBUG, "Truncated IE in beacon_ies",
                        (char *)p, (wpa_size_t)l);
            break;
        }
        if (wpa_s->ap_wpa_ie == NULL &&
                p[0] == GENERIC_INFO_ELEM && p[1] >= 6 &&
                memcmp(&p[2], "\x00\x50\xF2\x01\x01\x00", 6) == 0) {
            wpa_s->ap_wpa_ie = SUPC_ALLOC_BUFF((kal_uint16)len);
            if (wpa_s->ap_wpa_ie) {
                memcpy(wpa_s->ap_wpa_ie, p, len);
                wpa_s->ap_wpa_ie_len = len;
            }
        }

        if (wpa_s->ap_rsn_ie == NULL &&
                p[0] == RSN_INFO_ELEM && p[1] >= 2) {
            wpa_s->ap_rsn_ie = SUPC_ALLOC_BUFF((kal_uint16)len);
            if (wpa_s->ap_rsn_ie) {
                memcpy(wpa_s->ap_rsn_ie, p, len);
                wpa_s->ap_rsn_ie_len = len;
            }

        }

        l -= len;
        p += len;
    }

}
#endif
#if 0
void wpa_supplicant_event(struct wpa_supplicant *wpa_s, wpa_event_type event,
                          union wpa_event_data *data)
{
    int pairwise;
    u32 now;
    u8 bssid[ETH_ALEN];

    switch (event) {
        case EVENT_ASSOC:
            wpa_s->wpa_state = WPA_ASSOCIATED;
            wpa_printf(MSG_DEBUG, "Association event - clear replay "
                       "counter");
            memset(wpa_s->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
            wpa_s->rx_replay_counter_set = 0;
            wpa_s->renew_snonce = 1;
            if (wpa_driver_mtk_get_bssid(wpa_s, bssid) >= 0 &&
                    memcmp(bssid, wpa_s->bssid, ETH_ALEN) != 0) {
                wpa_msg(wpa_s, MSG_DEBUG, "Associated to a new BSS: "
                        "BSSID=" MACSTR, MAC2STR(bssid));
                memcpy(wpa_s->bssid, bssid, ETH_ALEN);
                if (wpa_supplicant_dynamic_keys(wpa_s)) {
                    wpa_clear_keys(wpa_s, bssid);
                }
                //wpa_supplicant_select_config(wpa_s);
            }
            wpa_msg(wpa_s, MSG_INFO, "Associated with " MACSTR,
                    MAC2STR(bssid));
            /* Set portEnabled first to FALSE in order to get EAP state
             * machine out of the SUCCESS state and eapSuccess cleared.
             * Without this, EAPOL PAE state machine may transit to
             * AUTHENTICATING state based on obsolete eapSuccess and then
             * trigger BE_AUTH to SUCCESS and PAE to AUTHENTICATED without
             * ever giving chance to EAP state machine to reset the state.
             */
            eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
            eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
            if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
                eapol_sm_notify_eap_success(wpa_s->eapol, FALSE);
            }
            /* 802.1X::portControl = Auto */
            eapol_sm_notify_portEnabled(wpa_s->eapol, TRUE);
            wpa_s->eapol_received = 0;
            if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
                    wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
                wpa_supplicant_cancel_auth_timeout(wpa_s);
            } else {
                /* Timeout for receiving the first EAPOL packet */
                wpa_supplicant_req_auth_timeout(wpa_s, TIMEOUT_EAP_IDREQ, 0);
            }
            break;
        case EVENT_DISASSOC:
            if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
                /* At least Host AP driver and a Prism3 card seemed to
                 * be generating streams of disconnected events when
                 * configuring IBSS for WPA-None. Ignore them for now.
                 */
                wpa_printf(MSG_DEBUG, "Disconnect event - ignore in "
                           "IBSS/WPA-None mode");
                break;
            }
            if (wpa_s->wpa_state >= WPA_ASSOCIATED) {
                wpa_supplicant_req_scan(wpa_s, 0, 100000);
            }
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
            if (!wpa_listed_success(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id)) { // Kinki add for MAUI_00401655
                wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
                wpa_blacklist_add_temporal(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
                wpa_blacklist_add_permanent(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
                supc_blacklist_update(wpa_s);
            }
#else
            wpa_blacklist_add(wpa_s, wpa_s->bssid);
#endif

            wpa_supplicant_mark_disassoc(wpa_s);
            wpa_msg(wpa_s, MSG_INFO, "Disconnect event - remove keys");
            if (wpa_supplicant_dynamic_keys(wpa_s)) {
                wpa_s->keys_cleared = 0;
                wpa_clear_keys(wpa_s, wpa_s->bssid);
            }
            break;
        case EVENT_MICHAEL_MIC_FAILURE:
            wpa_msg(wpa_s, MSG_WARNING, "Michael MIC failure detected");
            pairwise = (data && data->michael_mic_failure.unicast);
            wpa_supplicant_key_request(wpa_s, 1, pairwise);
            kal_get_time(&now);
            if (wpa_s->last_michael_mic_error &&
                    kal_ticks_to_secs(now - wpa_s->last_michael_mic_error) <= 60) {
                /* initialize countermeasures */
                wpa_s->countermeasures = 1;
                wpa_msg(wpa_s, MSG_WARNING, "TKIP countermeasures "
                        "started");

                /* Need to wait for completion of request frame. We do
                 * not get any callback for the message completion, so
                 * just wait a short while and hope for the best. */
                //usleep(10000);

                wpa_supplicant_deauthenticate(
                    wpa_s, REASON_MICHAEL_MIC_FAILURE);
                //wpa_drv_set_countermeasures(wpa_s, 1);

#if CONFIG_DRIVER_MTK
                wpa_s->reassociate = 1;
                wpa_supplicant_cancel_auth_timeout(wpa_s);
#endif
//          eloop_cancel_timeout(
//              wpa_supplicant_stop_countermeasures, wpa_s,
//              NULL);
//          eloop_register_timeout(
//              60, 0, wpa_supplicant_stop_countermeasures,
//              wpa_s, NULL);
                /* TODO: mark the AP rejected for 60 second. STA is
                 * allowed to associate with another AP.. */
            }
            kal_get_time(&(wpa_s->last_michael_mic_error));
            //wpa_s->last_michael_mic_error = now;
            break;
        case EVENT_SCAN_RESULTS:
            wpa_supplicant_scan_results(wpa_s);
            break;
        case EVENT_ASSOCINFO:
            wpa_supplicant_associnfo(wpa_s, data);
            break;
        case EVENT_INTERFACE_STATUS:
            if (strcmp(wpa_s->ifname, data->interface_status.ifname) != 0) {
                break;
            }
            switch (data->interface_status.ievent) {
                case EVENT_INTERFACE_ADDED:
                    if (!wpa_s->interface_removed) {
                        break;
                    }
                    wpa_s->interface_removed = 0;
                    wpa_printf(MSG_DEBUG, "Configured interface was "
                               "added.");
                    if (wpa_supplicant_driver_init(wpa_s, 1) < 0) {
                        wpa_printf(MSG_INFO, "Failed to initialize "
                                   "the driver after interface was "
                                   "added.");
                    }
                    break;
                case EVENT_INTERFACE_REMOVED:
                    wpa_printf(MSG_DEBUG, "Configured interface was "
                               "removed.");
                    wpa_s->interface_removed = 1;
                    wpa_supplicant_mark_disassoc(wpa_s);
                    //l2_packet_deinit(wpa_s->l2);
                    break;
            }
            break;
        case EVENT_PMKID_CANDIDATE:
            wpa_supplicant_add_pmkid_candidate(wpa_s, data);
            break;
        default:
            wpa_printf(MSG_INFO, "Unknown event %d", event);
            break;
    }
}
#endif
#if 0
static void wpa_supplicant_terminate(int sig, void *eloop_ctx,
                                     void *signal_ctx)
{
    struct wpa_supplicant *wpa_s = eloop_ctx;
    for (wpa_s = wpa_s->head; wpa_s; wpa_s = wpa_s->next) {
        wpa_msg(wpa_s, MSG_INFO, "Signal %d received - terminating",
                sig);
    }
    eloop_terminate();
}


int wpa_supplicant_reload_configuration(struct wpa_supplicant *wpa_s)
{
    struct wpa_config *conf;
    int reconf_ctrl;
    if (wpa_s->confname == NULL) {
        return -1;
    }
    conf = wpa_config_read(wpa_s->confname);
    if (conf == NULL) {
        wpa_msg(wpa_s, MSG_ERROR, "Failed to parse the configuration "
                "file '%s' - exiting", wpa_s->confname);
        return -1;
    }

    reconf_ctrl = !!conf->ctrl_interface != !!wpa_s->conf->ctrl_interface
                  || (conf->ctrl_interface && wpa_s->conf->ctrl_interface &&
                      strcmp(conf->ctrl_interface, wpa_s->conf->ctrl_interface)
                      != 0);

    if (reconf_ctrl) {
        wpa_supplicant_ctrl_iface_deinit(wpa_s);
    }

    wpa_s->current_ssid = NULL;
    eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
    rsn_preauth_deinit(wpa_s);
    wpa_config_free(wpa_s->conf);
    wpa_s->conf = conf;
    if (reconf_ctrl) {
        wpa_supplicant_ctrl_iface_init(wpa_s);
    }
    wpa_s->reassociate = 1;
    wpa_supplicant_req_scan(wpa_s, 0, 0);
    wpa_msg(wpa_s, MSG_DEBUG, "Reconfiguration completed");
    return 0;
}

#ifndef CONFIG_NATIVE_WINDOWS
static void wpa_supplicant_reconfig(int sig, void *eloop_ctx,
                                    void *signal_ctx)
{
    struct wpa_supplicant *wpa_s = eloop_ctx;
    wpa_printf(MSG_DEBUG, "Signal %d received - reconfiguring", sig);
    for (wpa_s = wpa_s->head; wpa_s; wpa_s = wpa_s->next) {
        if (wpa_supplicant_reload_configuration(wpa_s) < 0) {
            eloop_terminate();
        }
    }
}
#endif /* CONFIG_NATIVE_WINDOWS */


static void wpa_supplicant_gen_assoc_event(struct wpa_supplicant *wpa_s)
{
    struct wpa_ssid *ssid;
    union wpa_event_data data;

    ssid = wpa_supplicant_get_ssid(wpa_s);
    if (ssid == NULL) {
        return;
    }

    wpa_printf(MSG_DEBUG, "Already associated with a configured network - "
               "generating associated event");
    memset(&data, 0, sizeof(data));
    wpa_supplicant_event(wpa_s, EVENT_ASSOC, &data);
}
#endif



static wpa_cipher cipher_suite2driver(int cipher)
{
    INNER_TRACE_STEP(1);
    //wpa_printf(MSG_DEBUG, "input %d", cipher);
    switch (cipher) {
        case WPA_CIPHER_NONE:
            //wpa_printf(MSG_DEBUG, "RET 1 %d", CIPHER_NONE);
            return CIPHER_NONE;
        case WPA_CIPHER_WEP40:
            //wpa_printf(MSG_DEBUG, "RET 2 %d", CIPHER_WEP40);
            return CIPHER_WEP40;
        case WPA_CIPHER_WEP104:
            //wpa_printf(MSG_DEBUG, "RET 3 %d", CIPHER_WEP104);
            return CIPHER_WEP104;
        case WPA_CIPHER_CCMP:
            //wpa_printf(MSG_DEBUG, "RET 4 %d", CIPHER_CCMP);
            return CIPHER_CCMP;
        case WPA_CIPHER_TKIP:
            //wpa_printf(MSG_DEBUG, "RET 5 %d", CIPHER_TKIP);
            return CIPHER_TKIP;
#ifdef __WAPI_SUPPORT__
        case WAPI_CIPHER_SMS4:
            //wpa_printf(MSG_DEBUG, "RET 6 %d", CIPHER_SMS4);
            return CIPHER_SMS4;
#endif
        default:
            //wpa_printf(MSG_DEBUG, "RET 7 %d", CIPHER_TKIP);
            return CIPHER_TKIP;
    }
}


static wpa_key_mgmt key_mgmt2driver(int key_mgmt)
{
    switch (key_mgmt) {
        case WPA_KEY_MGMT_NONE:
            return KEY_MGMT_NONE;
        case WPA_KEY_MGMT_IEEE8021X_NO_WPA:
            return KEY_MGMT_802_1X_NO_WPA;
        case WPA_KEY_MGMT_IEEE8021X:
            return KEY_MGMT_802_1X;
        case WPA_KEY_MGMT_WPA_NONE:
            return KEY_MGMT_WPA_NONE;
        case WPA_KEY_MGMT_PSK:
            return KEY_MGMT_PSK;
#ifdef __WAPI_SUPPORT__
        case WAPI_KEY_MGMT_PSK:
            return KEY_MGMT_WAPI_PSK;
        case WAPI_KEY_MGMT_WAI:
            return KEY_MGMT_WAPI_WAI;
#endif
#ifdef __WPS_SUPPORT__
        case WPA_KEY_MGMT_WPS:
            return KEY_MGMT_802_1X;
#endif
        default:
            return KEY_MGMT_PSK;
    }
}


static int wpa_supplicant_suites_from_ai(struct wpa_supplicant *wpa_s,
        struct wpa_ssid *ssid,
        struct wpa_ie_data *ie)
{
    if (wpa_s->assoc_wpa_ie == NULL) {
        return -1;
    }

#ifdef __WAPI_SUPPORT__
    if (ssid->proto & WPA_PROTO_WAPI) {
        if (wpa_parse_wapi_ie(wpa_s, wpa_s->assoc_wpa_ie,
                              wpa_s->assoc_wpa_ie_len, ie)) {
            //wpa_msg(wpa_s, MSG_INFO, "WPA: Failed to parse WPA IE from "
            //"association info");
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_FAIL_PARSE_WPA_IE_FROM_AI);
            return -1;
        }
    } else if (ssid->proto & (WPA_PROTO_RSN | WPA_PROTO_WPA)) {
        if (wpa_parse_wpa_ie(wpa_s, wpa_s->assoc_wpa_ie,
                             wpa_s->assoc_wpa_ie_len, ie)) {
            //wpa_msg(wpa_s, MSG_INFO, "WPA: Failed to parse WPA IE from "
            //"association info");
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_FAIL_PARSE_WPA_IE_FROM_AI);
            return -1;
        }
    } else {
        //wpa_msg(wpa_s, MSG_INFO, "WPA: Failed to parse IE from "
        //"association info");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_FAIL_PARSE_WPA_IE_FROM_AI);
        return -1;
    }
#else
    if (wpa_parse_wpa_ie(wpa_s, wpa_s->assoc_wpa_ie,
                         wpa_s->assoc_wpa_ie_len, ie)) {
        wpa_printf(TRACE_GROUP_1, "WPA: Failed to parse WPA IE from "
                   "association info\n");
//      kal_trace(DBG_CONN,INFO_SUPC_WAPI_FAIL_PARSE_WPA_IE_FROM_AI);
        return -1;
    }
#endif
    wpa_printf(TRACE_GROUP_1, "WPA: Using WPA IE from AssocReq to set cipher "
               "suites\n");
//  kal_trace(DBG_CONN,INFO_SUPC_USE_WPA_IE_FROM_ASSOC);

    if (!(ie->group_cipher & ssid->group_cipher)) {
        //wpa_msg(wpa_s, MSG_INFO, "WPA: Driver used disabled group "
        //"cipher 0x%x (mask 0x%x) - reject",
        //ie->group_cipher, ssid->group_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_DRV_USE_DISABLED_GROUP_CIPHER,ie->group_cipher, ssid->group_cipher);
        return -1;
    }
    if (!(ie->pairwise_cipher & ssid->pairwise_cipher)) {
        //wpa_msg(wpa_s, MSG_INFO, "WPA: Driver used disabled pairwise "
        //"cipher 0x%x (mask 0x%x) - reject",
        //ie->pairwise_cipher, ssid->pairwise_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_DRV_USE_DISABLED_PAIR_CIPHER,ie->pairwise_cipher, ssid->pairwise_cipher);
        return -1;
    }
    if (!(ie->key_mgmt & ssid->key_mgmt)) {
        //wpa_msg(wpa_s, MSG_INFO, "WPA: Driver used disabled key "
        //"management 0x%x (mask 0x%x) - reject",
        //ie->key_mgmt, ssid->key_mgmt);
//      kal_trace(DBG_CONN, INFO_SUPC_DRV_USE_DISABLED_KEY_MGMT, ie->key_mgmt, ssid->key_mgmt);
        return -1;
    }

    return 0;
}

//Kinki add for MAUI_00526025
int wpa_supplicant_suites_from_assoc_ie(struct wpa_supplicant *wpa_s,
                                        u8 *assoc_ie, wpa_size_t assoc_ie_len)
{
    struct wpa_ie_data ie;
    if (assoc_ie == NULL) {
        //wpa_msg(wpa_s, MSG_WARNING, "WPA: No assoc IE for the connected BSS.");
//      kal_trace(DBG_CONN, INFO_SUPC_NO_ASSOC_IE_FROM_BSS);
        return -1;
    }
    if (assoc_ie && wpa_parse_wpa_ie(wpa_s, assoc_ie, assoc_ie_len, &ie)) {
        //wpa_msg(wpa_s, MSG_WARNING, "WPA: Failed to parse WPA IE for "
        //"the connected BSS.");
//       kal_trace(DBG_CONN, INFO_SUPC_PARSE_ASSOC_IE_FROM_BSS_FAIL);
        return -1;
    }

    wpa_s->pairwise_cipher = ie.pairwise_cipher;
    wpa_s->group_cipher = ie.group_cipher;
    wpa_s->key_mgmt = ie.key_mgmt;
    wpa_s->proto = ie.proto;

    return 0;
}

#ifdef __WAPI_SUPPORT__
int wapi_supplicant_suites_from_assoc_ie(struct wpa_supplicant *wpa_s, u8 *assoc_ie, wpa_size_t assoc_ie_len)
{
    struct wpa_ie_data ie;
    if (assoc_ie == NULL) {
        //wpa_msg(wpa_s, MSG_WARNING, "WPA: No assoc IE for the connected BSS.");
        kal_trace(DBG_WAPI, INFO_SUPC_NO_ASSOC_IE_FROM_BSS);
        return -1;
    }
    if (assoc_ie && wpa_parse_wapi_ie(wpa_s, assoc_ie, assoc_ie_len, &ie)) {
        //wpa_msg(wpa_s, MSG_WARNING, "WPA: Failed to parse WAPI IE for "
        //"the connected BSS.");
        kal_trace(DBG_WAPI, INFO_SUPC_PARSE_ASSOC_IE_FROM_BSS_FAIL);
        return -1;
    }

    wpa_s->pairwise_cipher = ie.pairwise_cipher;
    wpa_s->group_cipher = ie.group_cipher;
    wpa_s->key_mgmt = ie.key_mgmt;
    wpa_s->proto = ie.proto;

    return 0;
}
#endif

int wpa_supplicant_set_suites(struct wpa_supplicant *wpa_s,
                              struct wndrv_supc_bss_info_struct *bss,
                              struct wpa_ssid *ssid,
                              u8 *wpa_ie, int *wpa_ie_len)
{
    struct wpa_ie_data ie;
    int sel, proto;
    u8 *ap_ie;
    wpa_size_t ap_ie_len;

    /* Joseph:
     * The purpose of thses codes are to get the ie_data
     */
    if (bss && bss->rsn_ie_len && (ssid->proto & WPA_PROTO_RSN)) {
        //wpa_msg(wpa_s, MSG_INFO, "RSN: using IEEE 802.11i/D9.0");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITE_11I_D90);
        proto = WPA_PROTO_RSN;
        ap_ie = bss->rsn_ie;
        ap_ie_len = bss->rsn_ie_len;
    }
#ifdef __WAPI_SUPPORT__
    else if (bss && bss->wapi_ie_len && (ssid->proto & WPA_PROTO_WAPI)) {
        //wpa_msg(wpa_s, MSG_INFO, "WAPI: using WAPI");
        kal_trace(DBG_WAPI, INFO_SUPC_SET_SUITE_USE_WAPI);
        proto = WPA_PROTO_WAPI;
        ap_ie = bss->wapi_ie;
        ap_ie_len = bss->wapi_ie_len;
    }
#endif
    else if (bss) {
        //wpa_msg(wpa_s, MSG_INFO, "WPA: using IEEE 802.11i/D3.0");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITE_11I_D30);
        proto = WPA_PROTO_WPA;
        ap_ie = bss->wpa_ie;
        ap_ie_len = bss->wpa_ie_len;
    } else {
        if (ssid->proto & WPA_PROTO_RSN) {
            proto = WPA_PROTO_RSN;
        }
#ifdef __WAPI_SUPPORT__
        else if (ssid->proto & WPA_PROTO_WAPI) {
            proto = WPA_PROTO_WAPI;
        }
#endif
        else {
            proto = WPA_PROTO_WPA;
        }

        ap_ie = NULL;
        ap_ie_len = 0;

        if (wpa_supplicant_suites_from_ai(wpa_s, ssid, &ie) < 0) {
            kal_mem_set(&ie, 0, sizeof(ie));
            ie.group_cipher = ssid->group_cipher;
            ie.pairwise_cipher = ssid->pairwise_cipher;
            ie.key_mgmt = ssid->key_mgmt;
            wpa_printf(TRACE_GROUP_1, "WPA: Set cipher suites based "
                       "on configuration\n");
//          kal_trace(DBG_CONN,INFO_SUPC_SET_CIPHER_BASED_CONFIG);
        }
    }

#ifdef __WAPI_SUPPORT__
    if (proto == WPA_PROTO_WAPI) {
        if (ap_ie && wpa_parse_wapi_ie(wpa_s, ap_ie, ap_ie_len, &ie)) {
            //wpa_msg(wpa_s, MSG_WARNING, "WPA: Failed to parse WAPI IE for "
            //"the selected BSS.");
            kal_trace(DBG_WAPI, INFO_SUPC_FAIL_PARSE_WAPI_IE_FOR_BSS);
            return -1;
        }
    } else {
        if (ap_ie && wpa_parse_wpa_ie(wpa_s, ap_ie, ap_ie_len, &ie)) {
            //wpa_msg(wpa_s, MSG_WARNING, "WPA: Failed to parse WPA/RSN IE for "
            //"the selected BSS.");
            kal_trace(DBG_CONN, INFO_SUPC_FAIL_PARSE_RSN_WPA_IE_FOR_BSS);
            return -1;
        }
    }
#else
    if (ap_ie && wpa_parse_wpa_ie(wpa_s, ap_ie, ap_ie_len, &ie)) {
        //wpa_msg(wpa_s, MSG_WARNING, "WPA: Failed to parse WPA IE for "
        //"the selected BSS.");
//      kal_trace(DBG_CONN,INFO_SUPC_FAIL_PARSE_WPA_IE_SELECTED_BSS);
        return -1;
    }
#endif
    wpa_printf(TRACE_GROUP_1, "WPA: Selected cipher suites: group %d "
               "pairwise %d key_mgmt %d\n",
               ie.group_cipher, ie.pairwise_cipher, ie.key_mgmt);
//  kal_trace(DBG_CONN,INFO_SUPC_SELECTED_CIHPER_SUITES,ie.group_cipher, ie.pairwise_cipher, ie.key_mgmt);


    wpa_s->proto = proto;

    /*J: Clear the previously stored wpa ie data */
    if (wpa_s->ap_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
    }
    wpa_s->ap_wpa_ie = NULL;
    wpa_s->ap_wpa_ie_len = 0;

    /* Store the ie data of the selected ap */
    if (bss && bss->wpa_ie_len) {
        wpa_s->ap_wpa_ie = SUPC_ALLOC_BUFF(bss->wpa_ie_len);
        if (wpa_s->ap_wpa_ie == NULL) {
            wpa_printf(TRACE_GROUP_1, "WPA: SUPC_ALLOC_BUFF failed\n");
//          kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_ALLOC_BUFFER_FAIL);
            return -1;
        }

        kal_mem_cpy(wpa_s->ap_wpa_ie, bss->wpa_ie, bss->wpa_ie_len);
        wpa_s->ap_wpa_ie_len = bss->wpa_ie_len;
    }
    if (wpa_s->ap_rsn_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
    }
    wpa_s->ap_rsn_ie = NULL;
    wpa_s->ap_rsn_ie_len = 0;
    if (bss && bss->rsn_ie_len) {
        wpa_s->ap_rsn_ie = SUPC_ALLOC_BUFF(bss->rsn_ie_len);
        if (wpa_s->ap_rsn_ie == NULL) {
            wpa_printf(TRACE_GROUP_1, "WPA: SUPC_ALLOC_BUFF failed\n");
//          kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_ALLOC_BUFFER_FAIL);
            return -1;
        }

        kal_mem_cpy(wpa_s->ap_rsn_ie, bss->rsn_ie, bss->rsn_ie_len);
        wpa_s->ap_rsn_ie_len = bss->rsn_ie_len;
    }
#ifdef __WAPI_SUPPORT__
    if (wpa_s->ap_wapi_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wapi_ie);
    }
    wpa_s->ap_wapi_ie = NULL;
    wpa_s->ap_wapi_ie_len = 0;
    if (bss && bss->wapi_ie_len) {
        wpa_s->ap_wapi_ie = SUPC_ALLOC_BUFF(bss->wapi_ie_len);
        if (wpa_s->ap_wapi_ie == NULL) {
            wpa_printf(TRACE_GROUP_1, "WPA: SUPC_ALLOC_BUFF failed\n");
            kal_trace(DBG_CONN, INFO_SUPC_SET_SUITES_ALLOC_BUFFER_FAIL);
            return -1;
        }

        kal_mem_cpy(wpa_s->ap_wapi_ie, bss->wapi_ie, bss->wapi_ie_len);
        wpa_s->ap_wapi_ie_len = bss->wapi_ie_len;
    }
#endif

    /* J: Decide which group cipher algorith to use */
    // sel = ie.group_cipher & ssid->group_cipher;
    sel = ie.group_cipher;  // Karen, use AP's group cipher
    if (sel & WPA_CIPHER_CCMP) {
        wpa_s->group_cipher = WPA_CIPHER_CCMP;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using GTK CCMP");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_GTK_CCMP);
    } else if (sel & WPA_CIPHER_TKIP) {
        wpa_s->group_cipher = WPA_CIPHER_TKIP;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using GTK TKIP");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_GTK_TKIP);
    } else if (sel & WPA_CIPHER_WEP104) {
        wpa_s->group_cipher = WPA_CIPHER_WEP104;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using GTK WEP104");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_GTK_WEP104);
    } else if (sel & WPA_CIPHER_WEP40) {
        wpa_s->group_cipher = WPA_CIPHER_WEP40;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using GTK WEP40");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_GTK_WEP40);
    }
#ifdef __WAPI_SUPPORT__
    else if (sel & WAPI_CIPHER_SMS4) {
        wpa_s->group_cipher = WAPI_CIPHER_SMS4;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using SMS4");
        kal_trace(DBG_CONN, INFO_SUPC_SET_SUITES_GTK_SMS4);
    }
#endif
    else {
        wpa_printf(TRACE_ERROR, "WPA: Failed to select group cipher.\n");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_GTK_FAIL);
        return -1;
    }

    /* J: Decide which pairwise cipher algorithm to use */
    sel = ie.pairwise_cipher & ssid->pairwise_cipher;
    if (sel & WPA_CIPHER_CCMP) {
        wpa_s->pairwise_cipher = WPA_CIPHER_CCMP;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using PTK CCMP");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_PTK_CCMP);
    } else if (sel & WPA_CIPHER_TKIP) {
        wpa_s->pairwise_cipher = WPA_CIPHER_TKIP;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using PTK TKIP");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_PTK_TKIP);
    } else if (sel & WPA_CIPHER_NONE) {
        wpa_s->pairwise_cipher = WPA_CIPHER_NONE;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using PTK NONE");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_PTK_NONE);
    }
#ifdef __WAPI_SUPPORT__
    else if (sel & WAPI_CIPHER_SMS4) {
        wpa_s->pairwise_cipher = WAPI_CIPHER_SMS4;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using SMS4");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_PTK_SMS4);
    }
#endif
    else {
        wpa_printf(MSG_WARNING, "WPA: Failed to select pairwise "
                   "cipher.\n");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_PTK_FAIL);
        return -1;
    }

    /* J: Decide which key management algorithm to use */
    sel = ie.key_mgmt & ssid->key_mgmt;
    if (sel & WPA_KEY_MGMT_IEEE8021X) {
        wpa_s->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using KEY_MGMT 802.1X");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_8021X);
    } else if (sel & WPA_KEY_MGMT_PSK) {
        wpa_s->key_mgmt = WPA_KEY_MGMT_PSK;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using KEY_MGMT WPA-PSK");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_PSK);
    } else if (sel & WPA_KEY_MGMT_WPA_NONE) {
        wpa_s->key_mgmt = WPA_KEY_MGMT_WPA_NONE;
        wpa_s->proto = WPA_PROTO_WPA;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using KEY_MGMT WPA-NONE");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_WPA_NONE);
    }
#ifdef __WAPI_SUPPORT__
    else if (sel & WAPI_KEY_MGMT_WAI) {
        wpa_s->key_mgmt = WAPI_KEY_MGMT_WAI;
        wpa_s->proto = WPA_PROTO_WAPI;
        // wpa_msg(wpa_s, MSG_DEBUG, "WPA: using KEY_MGMT WAPI WAI");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_WAI);
    }  else if (sel & WAPI_KEY_MGMT_PSK) {
        wpa_s->key_mgmt = WAPI_KEY_MGMT_PSK;
        wpa_s->proto = WPA_PROTO_WAPI;
        //wpa_msg(wpa_s, MSG_DEBUG, "WPA: using KEY_MGMT WAPI PSK");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_WAPI_PSK);
    }
#endif
    else {
        //wpa_printf(MSG_WARNING, "WPA: Failed to select authenticated "
        //"key management type.");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_AKM_FAIL);
        return -1;
    }

#ifdef __WAPI_SUPPORT__
    if (proto == WPA_PROTO_WAPI) {
        *wpa_ie_len = wpa_gen_wapi_ie(wpa_s, wpa_ie);
    } else {
        *wpa_ie_len = wpa_gen_wpa_ie(wpa_s, wpa_ie);
    }
#else
    *wpa_ie_len = wpa_gen_wpa_ie(wpa_s, wpa_ie);
#endif
    if (*wpa_ie_len < 0) {
        wpa_printf(MSG_WARNING, "WPA: Failed to generate WPA IE.\n");
//      kal_trace(DBG_CONN,INFO_SUPC_SET_SUITES_GEN_IE_FAIL);
        return -1;
    }
    //wpa_hexdump(MSG_DEBUG, "WPA: Own WPA IE", (char *)wpa_ie, (wpa_size_t)*wpa_ie_len);
//  kal_buffer_trace(DBG_CONN, INFO_SUPC_SET_SUITES_DUMP_IE,(wpa_size_t)*wpa_ie_len,wpa_ie);

    if (ssid->key_mgmt & WPA_KEY_MGMT_PSK) {
        wpa_s->pmk_len = PMK_LEN;
        memcpy(wpa_s->pmk, ssid->psk, PMK_LEN);
    }
#ifdef __WAPI_SUPPORT__
    else if (ssid->key_mgmt & WAPI_KEY_MGMT_PSK) {
        wpa_s->pmk_len = PMK_LEN;//TODO how to calculate BK
        //Convert PSK stores a string based presharekey.
        memcpy(wpa_s->pmk, ssid->psk, PMK_LEN);
    }
#endif
    else if (wpa_s->cur_pmksa) {
        wpa_s->pmk_len = wpa_s->cur_pmksa->pmk_len;
        memcpy(wpa_s->pmk, wpa_s->cur_pmksa->pmk, wpa_s->pmk_len);
    } else {
        wpa_s->pmk_len = PMK_LEN;
        kal_mem_set(wpa_s->pmk, 0, PMK_LEN);
#ifdef CONFIG_XSUPPLICANT_IFACE
        wpa_s->ext_pmk_received = 0;
#endif /* CONFIG_XSUPPLICANT_IFACE */
    }

    return 0;
}


void wpa_supplicant_associate(struct wpa_supplicant *wpa_s,
                              struct wndrv_supc_bss_info_struct *bss,
                              struct wpa_ssid *ssid)
{
    int i = 0;
    u8 wpa_ie[80];
    int wpa_ie_len;
    // int use_crypt;
//    int algs = AUTH_ALG_OPEN_SYSTEM;
    wpa_cipher cipher_pairwise, cipher_group;
    struct wpa_driver_associate_params params;
    int wep_keys_set = 0;
//    struct wpa_driver_capa capa;
#ifdef __WAPI_SUPPORT__
    CNTAP_PARA  wapi_param;
    int wapi_set_param_result = 0;
#endif

    wpa_s->reassociate = 0;
    INNER_TRACE_STEP(1);
    kal_mem_set(&params, 0, sizeof(struct wpa_driver_associate_params));

    if (bss != NULL) {
        wpa_printf(TRACE_GROUP_1, "Trying to associate with SSID\n");
        //wpa_hexdump(MSG_INFO, "SSID is", (char*)ssid->ssid, ssid->ssid_len);
        //wpa_hexdump(MSG_INFO, "bssid is", (char*)bss->bssid, WNDRV_MAC_ADDRESS_LEN);
//      kal_trace(DBG_CONN,INFO_SUPC_TRY_ASSOC);
//      kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s,%d",ssid->ssid,ssid->ssid_len);
//      kal_buffer_trace(DBG_CONN,INFO_SUPC_TRY_ASSOC_BSSID,6,bss->bssid);
        kal_mem_set(wpa_s->bssid, 0, ETH_ALEN);
    } else {
        wpa_printf(TRACE_GROUP_1, "Trying to associate with AP with empty BSSID\n");
        //wpa_hexdump(MSG_INFO, "SSID is", (char*)ssid->ssid, ssid->ssid_len);
//      kal_trace(DBG_CONN,INFO_SUPC_TRY_ASSOC_NULL_BSS);
//      kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s,%d",ssid->ssid,ssid->ssid_len);
    }

    /* Joseph:
     * The original design of supplicant scan req is to periodically request network driver to
     * scan neiborhood aps, so it has to cancel scan at this time (going to associate). In our
     * solution, we have no need to do the scan all the time, so we need not to stop the
     * scan either. Remart it.
     */
    //wpa_supplicant_cancel_scan(wpa_s);

    /* Starting new association, so clear the possibly used WPA IE from the
    * previous association. */
    if (wpa_s->assoc_wpa_ie != NULL) {
        SUPC_DEALLOC_BUFF(wpa_s->assoc_wpa_ie);
    }
    wpa_s->assoc_wpa_ie = NULL;
    wpa_s->assoc_wpa_ie_len = 0;

    /* Joseph:
     * Set authentication algorithm to driver
     * It says driver will handle this. I hope so. Remark these codes
     */
    /*
    if (ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA)
    {
        if (ssid->leap)
        {
            if (ssid->non_leap == 0)
            {
                algs = AUTH_ALG_LEAP;
            }
            else
            {
                algs |= AUTH_ALG_LEAP;
            }
        }
    }

    wpa_printf(MSG_DEBUG, "Automatic auth_alg selection: 0x%x", algs);

    if (ssid->auth_alg)
    {
        algs = 0;

        if (ssid->auth_alg & WPA_AUTH_ALG_OPEN)
        {
            algs |= AUTH_ALG_OPEN_SYSTEM;
        }

        if (ssid->auth_alg & WPA_AUTH_ALG_SHARED)
        {
            algs |= AUTH_ALG_SHARED_KEY;
        }

        if (ssid->auth_alg & WPA_AUTH_ALG_LEAP)
        {
            algs |= AUTH_ALG_LEAP;
        }

        wpa_printf(MSG_DEBUG, "Overriding auth_alg selection: 0x%x", algs);
    }

    wpa_driver_mtk_set_auth_alg(wpa_s, algs);

    */


    /* Joseph:
     * If selected ap (information from the air) is not NULL && there are wpa/rsn ies supported && the profile
     * of the ap (information for the config file) supports (802.1x or PSK) key mgmt
     */
    if (bss && (bss->wpa_ie_len || bss->rsn_ie_len) &&
            (ssid->key_mgmt & (WPA_KEY_MGMT_IEEE8021X | WPA_KEY_MGMT_PSK))) {
        wpa_s->cur_pmksa = pmksa_cache_get(wpa_s, bss->bssid, NULL);

        if (wpa_s->cur_pmksa) {
            //wpa_hexdump(MSG_DBG_CONN, "RSN: PMKID",
            //(char *)wpa_s->cur_pmksa->pmkid, PMKID_LEN);
//          kal_buffer_trace(DBG_CONN, DUMP_SUPC_PMKID,PMKID_LEN,wpa_s->cur_pmksa->pmkid);
            eapol_sm_notify_pmkid_attempt(wpa_s->eapol, 1);
        }

        if (wpa_supplicant_set_suites(wpa_s, bss, ssid,
                                      wpa_ie, &wpa_ie_len)) {
            wpa_printf(TRACE_GROUP_1, "WPA: Failed to set WPA key "
                       "management and encryption suites\n");
//          kal_trace(DBG_CONN,INFO_SUPC_FAIL_SET_WPA_KEY_MGMT);
            return;
        }
    } else if (ssid->key_mgmt &
               (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_IEEE8021X |
                WPA_KEY_MGMT_WPA_NONE)) {
        if (wpa_supplicant_set_suites(wpa_s, NULL, ssid,
                                      wpa_ie, &wpa_ie_len)) {
            wpa_printf(TRACE_WARNING, "WPA: Failed to set WPA key "
                       "management and encryption suites (no scan "
                       "results)\n");
//          kal_trace(DBG_CONN,INFO_SUPC_FAIL_SET_WPA_KEY_MGMT_NO_SCAN);
            return;
        }
    }
#ifdef __WAPI_SUPPORT__
    else if (ssid->key_mgmt & (WAPI_KEY_MGMT_PSK | WAPI_KEY_MGMT_WAI)) {
        if (wpa_supplicant_set_suites(wpa_s, NULL, ssid,
                                      wpa_ie, &wpa_ie_len)) {
            wpa_printf(TRACE_WARNING, "WPA: Failed to set WPA key "
                       "management and encryption suites (no scan "
                       "results)\n");
            kal_trace(DBG_CONN, INFO_SUPC_FAIL_SET_WPA_KEY_MGMT_NO_SCAN);
            return;
        }
    }

#endif

#ifdef __WPS_SUPPORT__
    else if (ssid->key_mgmt & WPA_KEY_MGMT_WPS) {
        struct wpabuf *wps_ie;
        wps_ie = wps_build_assoc_req_ie(wpas_wps_get_req_type(ssid));
        if (wps_ie && wpabuf_len(wps_ie) <= sizeof(wpa_ie)) {
            wpa_ie_len = wpabuf_len(wps_ie);
            kal_mem_cpy(wpa_ie, wpabuf_head(wps_ie), wpa_ie_len);
        } else {
            wpa_ie_len = 0;
        }
        wpabuf_free(wps_ie);
        wpa_supplicant_set_non_wpa_policy(wpa_s, ssid);
    }
#endif /* __WPS_SUPPORT__ */

    else {
        wpa_printf(TRACE_GROUP_1, "set non wpa policy for open-mode\n");
//      kal_trace(DBG_CONN,INFO_SUPC_FAIL_SET_NON_WPA_OPEN_MODE);
        wpa_supplicant_set_non_wpa_policy(wpa_s, ssid);
        wpa_ie_len = 0;
    }

    wpa_clear_keys(wpa_s, bss ? bss->bssid : NULL);

    // use_crypt = 1;

    wpa_printf(TRACE_GROUP_1, "wpa_s->key_mgmt=%d pair_cipher=%d, gp_cipher=%d\n",
               wpa_s->key_mgmt, wpa_s->pairwise_cipher, wpa_s->group_cipher);

//   kal_trace(DBG_CONN,INFO_SUPC_WPA_KEY_MGMT, wpa_s->key_mgmt, wpa_s->pairwise_cipher, wpa_s->group_cipher);

    cipher_pairwise = cipher_suite2driver(wpa_s->pairwise_cipher);
    cipher_group = cipher_suite2driver(wpa_s->group_cipher);

    wpa_printf(TRACE_GROUP_1, "cipher_pair=%d, cipher_gp=%d\n",
               cipher_pairwise, cipher_group);
//  kal_trace(DBG_CONN,INFO_SUPC_CIPHER_TO_DRVIVER,cipher_pairwise, cipher_group);
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
        int i;
        INNER_TRACE_STEP(1);
        // if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE)
        //  {
        //      use_crypt = 0;
        //  }
        // wpa_printf(MSG_DEBUG, "cipher_pair=%d, cipher_gp=%d",
        //cipher_pairwise, cipher_group);

        for (i = 0; i < NUM_WEP_KEYS; i++) {
            if (ssid->wep_key_len[i]) {
                //         use_crypt = 1;
                wep_keys_set = 1;
                params.wep_info[i].key_index = i;
                params.wep_info[i].key_length = ssid->wep_key_len[i];
                //wpa_printf(MSG_DEBUG, "cipher_pair=%d, cipher_gp=%d",
                //cipher_pairwise, cipher_group);

                kal_mem_cpy(&(params.wep_info[i].key_material[0]),
                            &(ssid->wep_key[i][0]), params.wep_info[i].key_length);

                if (i == ssid->wep_tx_keyidx) {
                    params.wep_info[i].key_index |= (kal_uint32)1 << 31;
                    /* J, 0x01 means keyID 1 is enabled, 0x08 means keyID 4 is enabled */
                    params.wep_enabled |= (0x01 << i);
                }
                //params.wep_enabled |= (0x01 << i); //moved for fix invalid WEP key also connect
                //wpa_set_wep_key(wpa_s, i == ssid->wep_tx_keyidx, i, ssid->wep_key[i], ssid->wep_key_len[i]);
            }
        }
    }
    INNER_TRACE_STEP(2);
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) {
        if ((ssid->eapol_flags &
                (EAPOL_FLAG_REQUIRE_KEY_UNICAST |
                 EAPOL_FLAG_REQUIRE_KEY_BROADCAST)) == 0 &&
                !wep_keys_set) {
            // use_crypt = 0;
        } else {
            /* Assume that dynamic WEP-104 keys will be used and
            * set cipher suites in order for drivers to expect
            * encryption. */
            cipher_pairwise = cipher_group = CIPHER_WEP104;
        }
    }
    wpa_printf(TRACE_GROUP_1,
               "before assoc, ssid->proto=%d pair_cipher=%d gp_cipher=%d\n",
               ssid->proto, ssid->pairwise_cipher, ssid->group_cipher);
//  kal_trace(DBG_CONN,INFO_SUPC_KEY_MGMT_BEFORE_ASSOC,ssid->proto, ssid->pairwise_cipher, ssid->group_cipher);
#ifdef __WPS_SUPPORT__
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPS) {
        wpa_printf(TRACE_GROUP_1, "set cipher in wps mode, ssid->proto=%d\n", ssid->proto);
//      kal_trace(DBG_WPS,INFO_SUPC_CIPHER_IN_WPS,ssid->proto);
        if (ssid->proto & WPA_PROTO_WPS_OPEN) {
            cipher_pairwise = cipher_group = CIPHER_NONE;
        } else if (ssid->proto & WPA_PROTO_WPS_WPA) {
            cipher_pairwise = cipher_group = CIPHER_TKIP;
        } else {
            cipher_pairwise = cipher_group = CIPHER_CCMP;
        }
    }
#endif
#if 0 /* [MAUI_02209450] [1] Assert Fail: wndrv_supc_func.c 1963 - MMI */
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        /* Set the key before (and later after) association */
        wpa_supplicant_set_wpa_none_key(wpa_s, ssid);
    }
#endif
    //wpa_drv_set_drop_unencrypted(wpa_s, use_crypt);
    wpa_s->wpa_state = WPA_ASSOCIATING;

    //kal_mem_set(&params, 0, sizeof(params));
    if (bss) {
        params.bssid = bss->bssid;
        params.ssid = bss->ssid;
        params.ssid_len = bss->ssid_len;
        //params.freq = bss->freq;
    } else {
        params.ssid = ssid->ssid;
        params.ssid_len = ssid->ssid_len;
    }
    printc("\nssid_dump start\n");
    for (i = 0; i < ssid->ssid_len; i++) {
        printc("%x-", ssid->ssid[i]);
    }
    printc("\nssid_dump end\n");
    params.wpa_ie = wpa_ie;
    params.wpa_ie_len = wpa_ie_len;
    params.pairwise_suite = cipher_pairwise;
    params.group_suite = cipher_group;
    INNER_TRACE_STEP(3);
    /* wps AP may be set to open or wpa2 mode,so supc will tell driver
    which auth_mode is chosen*/
#ifdef __WPS_SUPPORT__
    if (ssid->key_mgmt & WPA_KEY_MGMT_WPS) {
        //wpa_printf(MSG_DEBUG. "ssid->proto=%d", ssid->proto);
        if (ssid->proto & WPA_PROTO_WPS_OPEN) {
            params.key_mgmt_suite = KEY_MGMT_WPS_OPEN;
        } else if (ssid->proto & WPA_PROTO_WPS_WPA) {
            params.key_mgmt_suite = KEY_MGMT_WPS_WPA;
        } else {
            params.key_mgmt_suite = KEY_MGMT_WPS_WPA2;
        }
    } else {
        params.key_mgmt_suite = key_mgmt2driver(wpa_s->key_mgmt);
    }
#else
    params.key_mgmt_suite = key_mgmt2driver(wpa_s->key_mgmt);
#endif
    params.auth_alg = ssid->auth_alg;
    params.mode = ssid->mode;
    /* only meaningful to driver in IBSS mode */
    params.channel_number = ssid->channel_number;

    wpa_s->current_ssid = ssid; // Kinki remove from below

    /*don't remove debug code!*/
    //wpa_printf(MSG_DEBUG, "---- network block -----");
//  kal_trace(DBG_CONN,INFO_SUPC_NETWORK_BLOCK_DEBUG);
    supc_dbg_show_ssid_ex(ssid);
    //wpa_printf(MSG_DEBUG, "------------------------");
//  kal_trace(DBG_CONN,INFO_SUPC_NETWORK_BLOCK_DEBUG_END);

#ifdef WIFI_BLACKLIST
    supc_blacklist_update(wpa_s);
#endif

#ifdef __WAPI_SUPPORT__
    //wpa_printf(MSG_DEBUG, "WAPI key mgmt %d ", wpa_s->key_mgmt);
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_KEY_MGMT, wpa_s->key_mgmt);


    if (wpa_s->key_mgmt == WAPI_KEY_MGMT_PSK) {
        /* it's not PMK actually but string */
        wapi_param.authType = AUTH_TYPE_WAPI_PSK;

        /* ASSUME ASCII */
        if (ssid->wapi_psk_type == 1) {
            wapi_param.para.psk.kt = KEY_TYPE_ASCII;
            wapi_param.para.psk.kl = strlen((char *)ssid->wapi_psk);
            memcpy(wapi_param.para.psk.kv, ssid->wapi_psk, wapi_param.para.psk.kl);
            wapi_param.para.psk.kv[wapi_param.para.psk.kl] = '\0';
        } else {
            wapi_param.para.psk.kt = KEY_TYPE_HEX;
            wapi_param.para.psk.kl = strlen((char *)ssid->wapi_psk);
            memcpy(wapi_param.para.psk.kv, ssid->wapi_psk, wapi_param.para.psk.kl);
            wapi_param.para.psk.kv[wapi_param.para.psk.kl] = '\0';
        }

        //wpa_printf(MSG_DEBUG, "auth %d type %d length %d %s",
        //(int)wapi_param.authType, (int)wapi_param.para.psk.kt,
        //(int)wapi_param.para.psk.kl, wapi_param.para.psk.kv);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_PSK_AUTH_TYPE, (int)wapi_param.authType, (int)wapi_param.para.psk.kt,
                  (int)wapi_param.para.psk.kl);
        kal_buffer_trace(DBG_WAPI, INFO_SUPC_WAPI_PSK_DUMP, wapi_param.para.psk.kl, wapi_param.para.psk.kv);

        wapi_set_param_result = WAI_CNTAPPARA_SET(&wapi_param);

        //wpa_printf(MSG_WARNING, "WAPI: WAI_CNTAPPARA_SET result %d",
        // wapi_set_param_result);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_CNTAPPARA_SET, wapi_set_param_result);

    } else if (wpa_s->key_mgmt == WAPI_KEY_MGMT_WAI) {
        struct wpa_ssid    *ssid = NULL;

        wapi_param.authType = AUTH_TYPE_WAPI;

        ssid = wpa_s->selected_ssid;
        SUPC_ASSERT(ssid);

        /* Could happen if certificate is removed later */
        if (ssid->client_cert == NULL) {
            //wpa_printf(MSG_WARNING, "WPA: Failed to get WAPI user certificate");
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_GET_USER_CERT_FAIL);
            return;
        }

        if (ssid->client_cert2 == NULL) {
            //wpa_printf(MSG_WARNING, "WPA: Failed to get WAPI as certificate");
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_GET_AS_CERT_FAIL);
            return;
        }

        if (ssid->private_key == NULL) {
            //wpa_printf(MSG_WARNING, "WPA: Failed to get WAPI as certificate");
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_GET_PRI_KEY_FAIL);
            return;
        }

        wapi_set_param_result = WAI_CNTAPPARA_SET(&wapi_param);

        //wpa_printf(MSG_WARNING, "WAPI: WAI_CNTAPPARA_SET result %d", wapi_set_param_result);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_CNTAPPARA_SET, wapi_set_param_result);
    }
#endif
    INNER_TRACE_STEP(4);

    if (wpa_driver_mtk_associate(wpa_s->drv_priv, &params) < 0) {
        wpa_printf(TRACE_GROUP_1, "Association request to the driver "
                   "failed\n");
//      kal_trace(DBG_CONN,INFO_SUPC_ASSOC_REQ_FAIL);
        /* try to continue anyway; new association will be tried again
        * after timeout */
        /* Joseph: will it try again later ???. Anyway we send the primitive to wndrv
         * and return KAL_TRUE always
         */
    }

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        /* Set the key after the association just in case association
        * cleared the previously configured key. */
        wpa_supplicant_set_wpa_none_key(wpa_s, ssid);
        /* No need to timeout authentication since there is no key
        * management. */
        wpa_supplicant_cancel_auth_timeout(wpa_s);
    } else if (ssid->network_type == WNDRV_SUPC_NETWORK_IBSS) {
        wpa_supplicant_cancel_auth_timeout(wpa_s);
    } else {
        /* Timeout for IEEE 802.11 authentication and association */
        wpa_supplicant_req_auth_timeout(wpa_s, AUTH_TIMEOUT_IEEE80211, 0);
    }

#ifdef __WAPI_SUPPORT__
    /* WAPI does not need EAPoL state machine */
    if (wpa_s->proto != WPA_PROTO_WAPI) {
        wpa_supplicant_initiate_eapol(wpa_s);
    }
#else
    //wpa_s->current_ssid = ssid; //Kinki remove it up
    wpa_supplicant_initiate_eapol(wpa_s);
#endif
}


void wpa_supplicant_disassociate(struct wpa_supplicant *wpa_s,
                                 int reason_code)
{
    u8 *addr = NULL;
    INNER_TRACE_STEP(1);
    // if (memcmp(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) != 0)
    //{
    // wpa_driver_mtk_disassociate(wpa_s, wpa_s->bssid, reason_code);
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

    addr = wpa_s->bssid;
    //}
    wpa_clear_keys(wpa_s, addr);
    //wpa_s->current_ssid = NULL; //Kinki
    eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
}


void wpa_supplicant_deauthenticate(struct wpa_supplicant *wpa_s,
                                   int reason_code)
{
    u8 *addr = NULL;
    INNER_TRACE_STEP(1);
    // if (memcmp(wpa_s->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) != 0)
    //{
    // wpa_driver_mtk_deauthenticate(wpa_s, wpa_s->bssid, reason_code);
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

    addr = wpa_s->bssid;
    //}
    wpa_clear_keys(wpa_s, addr);
    wpa_s->current_ssid = NULL;
    eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
    eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
    eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
}

#if 0
static void wpa_supplicant_imsi_identity(struct wpa_supplicant *wpa_s,
        struct wpa_ssid *ssid)
{
    int aka = 0;
    u8 *pos = ssid->eap_methods;

    while (pos && *pos != EAP_TYPE_NONE) {
        if (*pos == EAP_TYPE_AKA) {
            aka = 1;
            break;
        }
        pos++;
    }

    if (ssid->identity == NULL && wpa_s->imsi) {
        ssid->identity = SUPC_ALLOC_BUFF((kal_uint16)(1 + wpa_s->imsi_len));
        if (ssid->identity) {
            ssid->identity[0] = aka ? '0' : '1';
            memcpy(ssid->identity + 1, wpa_s->imsi,
                   wpa_s->imsi_len);
            ssid->identity_len = 1 + wpa_s->imsi_len;
            wpa_hexdump_ascii(MSG_DEBUG, "permanent identity from "
                              "IMSI", (char *)ssid->identity,
                              ssid->identity_len);
        }
    }
}

void wpa_supplicant_scard_init(struct wpa_supplicant *wpa_s,
                               struct wpa_ssid *ssid)
{
    char buf[100];
    wpa_size_t len;

    if (ssid->pcsc == NULL) {
        return;
    }
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

    wpa_hexdump_ascii(MSG_DEBUG, "IMSI", (char *) buf, len);
    SUPC_DEALLOC_BUFF(wpa_s->imsi);
    wpa_s->imsi = SUPC_ALLOC_BUFF(len);
    if (wpa_s->imsi) {
        memcpy(wpa_s->imsi, buf, len);
        wpa_s->imsi_len = len;
        wpa_supplicant_imsi_identity(wpa_s, ssid);
    }
}
#endif

static int wpa_supplicant_match_privacy(
    struct wndrv_supc_bss_info_struct *bss,
    struct wpa_ssid *ssid)
{
    int i, privacy = 0;

#ifdef __WPS_SUPPORT__
    if (ssid->key_mgmt & WPA_KEY_MGMT_WPS) {
        return 1;
    }
#endif /* __WPS_SUPPORT__ */

    for (i = 0; i < NUM_WEP_KEYS; i++) {
        if (ssid->wep_key_len[i]) {
            privacy = 1;
            break;
        }
    }
#ifdef IEEE8021X_EAPOL
    if ((ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA) &&
            ssid->eapol_flags & (EAPOL_FLAG_REQUIRE_KEY_UNICAST |
                                 EAPOL_FLAG_REQUIRE_KEY_BROADCAST)) {
        privacy = 1;
    }
#endif /* IEEE8021X_EAPOL */

    if ((1 == bss->privacy && 1 == privacy)
            || (0 == bss->privacy && 0 == privacy)) {
        return 1;
    }
    return 0;

}


struct wndrv_supc_bss_info_struct *
wpa_supplicant_select_bss_wpa(
    struct wpa_supplicant *wpa_s,
    struct wpa_ssid *group,
    struct wpa_ssid **selected_ssid)
{
    int i = 0;
    int num = wpa_s->num_scan_results;
    struct wpa_ssid *ssid = NULL;
    //struct wndrv_supc_bss_info_struct* selected = NULL;
    struct wndrv_supc_bss_info_struct *bss = NULL;

//  kal_trace(DBG_CONN,INFO_SUPC_SEARCH_WPA_AP_START);
    for (i = 0; i < num; i++) {
        //char msg[48] = {0};
        bss = &(wpa_s->scan_results[i]);
        //sprintf(msg, "AP(%d)==> check AP SSID:", i);
        //wpa_hexdump(MSG_DBG_SCAN, (const char*)msg, (char*)bss->ssid, bss->ssid_len);

//      kal_trace(DBG_CONN,INFO_SUPC_SEARCH_WPA_AP,i);
//      kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s,%d",bss->ssid,bss->ssid_len);

#ifdef __WAPI_SUPPORT__
        //wpa_printf(MSG_DBG_SCAN, "BSSID(=" MACSTR
        //") wpa_ie_len(%d) rsn_ie_len(%d) wapi_ie_len(%d) )",
        //MAC2STR(bss->bssid),
        //bss->wpa_ie_len,
        //bss->rsn_ie_len,
        //bss->wapi_ie_len);
        kal_trace(DBG_CONN, INFO_SUPC_WPA_AP_WAPI, MAC2STR(bss->bssid), bss->wpa_ie_len,
                  bss->rsn_ie_len,
                  bss->wapi_ie_len);

#else
        wpa_printf(TRACE_GROUP_1, "AP(%d) => BSSID(=" MACSTR
                   ") \n wpa_ie_len(%d) rsn_ie_len(%d)\n nettype(%d)\n",
                   i, MAC2STR(bss->bssid),
                   bss->wpa_ie_len,
                   bss->rsn_ie_len,
                   bss->network_type);

//      kal_trace(DBG_CONN,INFO_SUPC_WPA_AP,i, MAC2STR(bss->bssid),
//               bss->wpa_ie_len,
//               bss->rsn_ie_len,
//               bss->network_type);


        if (bss->has_wps_ie) {
            wpa_printf(TRACE_GROUP_1, "get a WPS AP wps_len=%d\n", bss->wps_ie_len);
//          kal_trace(DBG_WPS,INFO_SUPC_FIND_WPS_AP,bss->wps_ie_len);
        }
#endif
        if (0 == bss->ssid_len &&
                wpa_s->connection_type != START_WPS_PBC &&
                wpa_s->connection_type != START_WPS_PIN) {
            wpa_printf(TRACE_GROUP_1, "skip this AP's ssid len = 0");
//          kal_trace(DBG_CONN,INFO_SUPC_SKIP_NULL_SSID);
            continue;
        }
#ifndef WIFI_BLACKLIST
        if (wpa_blacklisted(wpa_s, bss->bssid)) {
            wpa_printf(TRACE_GROUP_1, "skip - blacklisted");
//          kal_trace(DBG_CONN,INFO_SUPC_SKIP_BLACKLIST);
            continue;
        }
#endif
        INNER_TRACE_STEP(0);
#ifdef __WAPI_SUPPORT__
        if (bss->wpa_ie_len == 0 && bss->rsn_ie_len == 0
                && bss->wapi_ie_len == 0) {
            //wpa_printf(MSG_DBG_SCAN, "   skip - no WPA/RSN/WAPI IE");
            kal_trace(DBG_CONN, INFO_SUPC_SKIP_NO_WPA_WAPI);
            continue;
        }
#else
        if (bss->wpa_ie_len == 0 && bss->rsn_ie_len == 0) {
            wpa_printf(TRACE_GROUP_1, "   skip - no WPA/RSN IE\n");
//          kal_trace(DBG_CONN,INFO_SUPC_SKIP_NO_WPA);
            continue;
        }
#endif
        INNER_TRACE_STEP(1);
        for (ssid = group; ssid; ssid = ssid->pnext) {
            struct wpa_ie_data ie, ie_rsn, ie_wpa;
            kal_bool check_ssid = KAL_TRUE;
#if 0
            wpa_printf(MSG_DEBUG, "---- network block -----");
            supc_dbg_show_ssid_ex(ssid);
#endif
#ifdef WIFI_BLACKLIST
            if (wpa_blacklisted_permanent(wpa_s, bss->bssid, ssid->profile_id)) {
                kal_trace(DBG_CONN, INFO_SUPC_SKIP_PBLACKLIST);
                continue;
            }

            if (wpa_blacklisted_temporal(wpa_s, bss->bssid, ssid->profile_id)) {
                kal_trace(DBG_CONN, INFO_SUPC_SKIP_TBLACKLIST);
                continue;
            }
#endif

#ifdef __WPS_SUPPORT__
            if (ssid->ssid_len == 0 &&  wpas_wps_ssid_wildcard_ok(wpa_s, ssid, bss)) {
                check_ssid = KAL_FALSE;
            }
#endif
            INNER_TRACE_STEP(2);
            /* Joseph:
             * Compare SSID first.
             */
            if ((bss->ssid_len != ssid->ssid_len ||
                    kal_mem_cmp(bss->ssid, ssid->ssid, bss->ssid_len) != 0)
                    && check_ssid) {
                wpa_printf(TRACE_GROUP_1, "   skip - SSID not match, bss->ssid = %s, ssid->ssid = %s\n"
                           , ssid_txt(bss->ssid, bss->ssid_len), ssid_txt(ssid->ssid, ssid->ssid_len));
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_SSID_MISMATCH);
//              kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s",ssid->ssid);
                continue;
            }

            /* Joseph:
             * Compare BSSID.
             */
            INNER_TRACE_STEP(3);
            if (ssid->bssid_set &&
                    kal_mem_cmp(bss->bssid, ssid->bssid, ETH_ALEN) != 0) {
                wpa_printf(TRACE_GROUP_1, "   skip - BSSID not match\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_BSSID_MISMATCH);
                continue;
            }

            /* Karen:
             * Compare network type
             */
            INNER_TRACE_STEP(4);
            if ((ssid->network_type == WNDRV_SUPC_NETWORK_IBSS)
                    && (bss->network_type != WNDRV_SUPC_NETWORK_IBSS)) {
                wpa_printf(TRACE_GROUP_1, "	skip - network type mismatch1\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_NETWORK_TYPE_MISMATCH);

                continue;
            } else if ((ssid->network_type == WNDRV_SUPC_NETWORK_INFRASTRUCTURE) &&
                       (bss->network_type != WNDRV_SUPC_NETWORK_INFRASTRUCTURE)) {
                wpa_printf(TRACE_GROUP_1, "	skip - network type mismatch2\n");
//                kal_trace(DBG_CONN,INFO_SUPC_SKIP_NETWORK_TYPE_MISMATCH);
                continue;
            }
            INNER_TRACE_STEP(5);
#ifdef  __WPS_SUPPORT__
            /*note: if get a wps AP, function return*/
            if (wpas_wps_ssid_bss_match(wpa_s, ssid, bss) > 0) {
                ssid->mode = bss->network_type;
                //   selected = bss;
                *selected_ssid = ssid;
                wpa_printf(TRACE_GROUP_1, " wps AP is selected\n");
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_AP_SELECTED);
                /*mtk80707. since, wps's ssid can not read from profile,
                it's RSN or WPA attributes must be added in here*/
                wpa_printf(TRACE_GROUP_1, "check ssid->proto=%d \n", ssid->proto);
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_AP_SECURITY, ssid->proto);
                ssid->proto &= ~(WPA_PROTO_WPS_WPA2 | WPA_PROTO_WPS_WPA | WPA_PROTO_WPS_WPA2);
                if (wpa_parse_wpa_ie(wpa_s, bss->rsn_ie, bss->rsn_ie_len, &ie) == 0) {
                    if (ie.proto & WPA_PROTO_RSN) {
                        ssid->proto |= WPA_PROTO_WPS_WPA2;
                    }
                } else if (wpa_parse_wpa_ie(wpa_s, bss->wpa_ie, bss->wpa_ie_len, &ie) == 0) {
                    if (ie.proto & WPA_PROTO_WPA) {
                        ssid->proto |= WPA_PROTO_WPS_WPA;
                    }
                } else {
                    wpa_printf(TRACE_GROUP_1, "malform RSN or WPA IE\n");
//                   kal_trace(DBG_WPS,INFO_SUPC_WPS_AP_SECURITY_MALFORMED);
                }
                //wpa_printf(MSG_DBG_SCAN, "recheck ssid->proto=%d ", ssid->proto);
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_AP_SECURITY_RECHECK, ssid->proto);
                ssid->pairwise_cipher = ie.pairwise_cipher;
                ssid->group_cipher = ie.group_cipher;
                wpa_printf(TRACE_GROUP_1, "recheck pair_cipher=%d gp_cipher=%d\n",
                           ssid->pairwise_cipher, ssid->group_cipher);
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_AP_SECURITY_PAIRE_RECHECK, ssid->pairwise_cipher, ssid->group_cipher);
                return bss;
            }
#endif
            /* Joseph:
             * The selected ap is using the specified protocol and
             * the ie is not correctly doceode
             */
            INNER_TRACE_STEP(6);
#ifdef __WAPI_SUPPORT__
            if (!(((ssid->proto & WPA_PROTO_RSN) &&
                    wpa_parse_wpa_ie(wpa_s, bss->rsn_ie,
                                     bss->rsn_ie_len, &ie) == 0) ||
                    ((ssid->proto & WPA_PROTO_WPA) &&
                     wpa_parse_wpa_ie(wpa_s, bss->wpa_ie,
                                      bss->wpa_ie_len, &ie) == 0)  ||
                    ((ssid->proto & WPA_PROTO_WAPI) &&
                     wpa_parse_wapi_ie(wpa_s, bss->wapi_ie,
                                       bss->wapi_ie_len, &ie) == 0))) {

                kal_trace(DBG_CONN, INFO_SUPC_SKIP_CAN_NOT_PARSE_WPA_WAPI_IE);
                continue;
            }
#else
            if (!(((ssid->proto & WPA_PROTO_RSN) &&
                    wpa_parse_wpa_ie(wpa_s, bss->rsn_ie,
                                     bss->rsn_ie_len, &ie) == 0) ||
                    ((ssid->proto & WPA_PROTO_WPA) &&
                     wpa_parse_wpa_ie(wpa_s, bss->wpa_ie,
                                      bss->wpa_ie_len, &ie) == 0))) {
                wpa_printf(TRACE_GROUP_1, "   skip - could not parse WPA/RSN IE, ssid->proto = %d\n", ssid->proto);
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_CAN_NOT_PARSE_WPA_IE);
                continue;
            }
#endif
            INNER_TRACE_STEP(7);
            // Karen: need to handle the case when both RSN and WPA are enabled
            if (bss->rsn_ie_len != 0 && bss->wpa_ie_len != 0) {
                kal_mem_set(&ie_rsn, 0, sizeof(ie_rsn));
                kal_mem_set(&ie_wpa, 0, sizeof(ie_wpa));

                // If both RSN IE and WPA IE are correct,
                // choose one and clear the other one
                if ((wpa_parse_wpa_ie(wpa_s, bss->rsn_ie,
                                      bss->rsn_ie_len, &ie_rsn) == 0) &&
                        (wpa_parse_wpa_ie(wpa_s, bss->wpa_ie,
                                          bss->wpa_ie_len, &ie_wpa) == 0)) {
                    if ((ie_rsn.proto & ssid->proto) &&
                            (ie_rsn.pairwise_cipher & ssid->pairwise_cipher) &&
                            (ie_rsn.key_mgmt & ssid->key_mgmt)) {
                        kal_mem_cpy(&ie, &ie_rsn, sizeof(ie));
                        bss->wpa_ie_len = 0;
                        kal_mem_set(&bss->wpa_ie, 0, WNDRV_MAX_WPA_IE_LEN);
                    } else {
                        kal_mem_cpy(&ie, &ie_wpa, sizeof(ie));
                        bss->rsn_ie_len = 0;
                        kal_mem_set(&bss->rsn_ie, 0, WNDRV_MAX_WPA_IE_LEN);
                    }
                }
            }
            INNER_TRACE_STEP(8);
            if (!(ie.proto & ssid->proto)) {
                wpa_printf(TRACE_GROUP_1, "   skip - proto mismatch\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_PROTP_MISMATCH);
                continue;
            }
            if (!(ie.pairwise_cipher & ssid->pairwise_cipher)) {
                wpa_printf(TRACE_GROUP_1, "      skip - PTK cipher mismatch\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_PTK_MISMATCH);
                continue;
            }
            INNER_TRACE_STEP(9);
#if 0
            if (!(ie.group_cipher & ssid->group_cipher)) {
                wpa_printf(MSG_DEBUG, "   skip - "
                           "GTK cipher mismatch");
                continue;
            }
#endif
            if (!(ie.key_mgmt & ssid->key_mgmt)) {
                wpa_printf(TRACE_GROUP_1, "   skip - "
                           "key mgmt mismatch, ssid mgmt=%d, bss mgmt=%d\n", ssid->key_mgmt, ie.key_mgmt);
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_EKY_MGMT_MISMATCH, ssid->key_mgmt, ie.key_mgmt);
                continue;
            }

            ssid->mode = bss->network_type;
            // selected = bss;
            *selected_ssid = ssid;
#if __WPS_SUPPORT__
// in wps, the ssid is null when it start. if supc find the WPS AP,
// it should fill this field
            if (0 == ssid->ssid_len) {
                ssid->ssid_len = bss->ssid_len;
                kal_mem_cpy(ssid->ssid, bss->ssid, bss->ssid_len);
            }
#endif
            // wpa_hexdump(MSG_INFO, "select SSID:", (kal_char *)bss->ssid, bss->ssid_len);
            wpa_printf(MSG_INFO, "select WPA AP: "MACSTR "\n", MAC2STR(bss->bssid));
//          kal_trace(DBG_CONN,INFO_SUPC_SELECTED_SSID);
//          kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s",bss->ssid);
//          kal_trace(DBG_CONN,INFO_SUPC_SELECTED_BSSID,MAC2STR(bss->bssid));
            return bss;
        }
    }
    return NULL;
}

struct wndrv_supc_bss_info_struct *
wpa_supplicant_select_bss_non_wpa(
    struct wpa_supplicant *wpa_s,
    struct wpa_ssid *group,
    struct wpa_ssid **selected_ssid
)
{
    int i = 0;
    int num = wpa_s->num_scan_results;
    struct wpa_ssid *ssid = NULL;
//    struct wndrv_supc_bss_info_struct* selected = NULL;
    struct wndrv_supc_bss_info_struct *bss = NULL;

//  kal_trace(DBG_CONN,INFO_SUPC_SEARCH_NON_WPA_AP_START);
    for (i = 0; i < num; i++) {
        //char msg[48] = {0};
        bss = &(wpa_s->scan_results[i]);
        wpa_printf(TRACE_GROUP_1, "AP(%d)==> check NON_WPA AP" MACSTR "\n", i, MAC2STR(bss->bssid));
        // sprintf(msg, "AP(%d)==> check NON_WPA AP SSID:", i);
        // wpa_hexdump(MSG_DEBUG, msg,(char*)bss->ssid, bss->ssid_len);
//        kal_trace(DBG_CONN,INFO_SUPC_SEARCH_NON_WPA_AP,i);
//      kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s,%d",bss->ssid,bss->ssid_len);
        if (bss->has_wps_ie) {
            wpa_printf(TRACE_GROUP_1, "get a WPS AP wps_len=%d\n", bss->wps_ie_len);
//          kal_trace(DBG_WPS,INFO_SUPC_FIND_WPS_AP,bss->wps_ie_len);
        }

        if (0 == bss->ssid_len &&
                wpa_s->connection_type != START_WPS_PBC &&
                wpa_s->connection_type != START_WPS_PIN) {
            wpa_printf(TRACE_GROUP_1, " skip this AP's ssid len = 0\n");
//          kal_trace(DBG_CONN,INFO_SUPC_SKIP_NULL_SSID);
            continue;
        }

#ifndef WIFI_BLACKLIST
        if (wpa_blacklisted(wpa_s, bss->bssid)) {
            wpa_printf(TRACE_GROUP_1, "AP(" MACSTR
                       ") is in black list\n", MAC2STR(bss->bssid));
//          kal_trace(DBG_CONN,INFO_SUPC_SKIP_BLACKLIST);
            continue;
        }
#endif
        INNER_TRACE_STEP(0);
        for (ssid = group; ssid; ssid = ssid->pnext) {
            kal_bool check_ssid = KAL_TRUE;
#if 0
            wpa_printf(MSG_DEBUG, "---- network block -----");
            supc_dbg_show_ssid_ex(ssid);
#endif

#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
            if (wpa_blacklisted_permanent(wpa_s,
                                          bss->bssid, ssid->profile_id)) {
                wpa_printf(TRACE_GROUP_1, "   skip - permanmently blacklisted\n");
                kal_trace(DBG_CONN, INFO_SUPC_SKIP_PBLACKLIST);
                continue;
            }

            if (wpa_blacklisted_temporal(wpa_s,
                                         bss->bssid, ssid->profile_id)) {
                //wpa_printf(MSG_DEBUG, "   skip - temporarily blacklisted");
                kal_trace(DBG_CONN, INFO_SUPC_SKIP_TBLACKLIST);
                continue;
            }
#endif
            INNER_TRACE_STEP(1);
#ifdef __WPS_SUPPORT__
            if (ssid->key_mgmt & WPA_KEY_MGMT_WPS
                    && bss->has_wps_ie) {
                /* Only allow wildcard SSID match if an AP
                 * advertises active WPS operation that matches
                 * with our mode. */
                if (ssid->ssid_len == 0 &&
                        wpas_wps_ssid_wildcard_ok(wpa_s, ssid, bss)) {
                    check_ssid = KAL_FALSE;
                }
            }
#endif /* __WPS_SUPPORT__ */
            INNER_TRACE_STEP(2);

            if (check_ssid &&
                    (bss->ssid_len != ssid->ssid_len ||
                     kal_mem_cmp(bss->ssid, ssid->ssid, bss->ssid_len) != 0)) {
                wpa_printf(TRACE_GROUP_1, "   skip - SSID mismatch, ssid = %s\n", print_ssid(bss->ssid, bss->ssid_len));
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_SSID_MISMATCH);
                continue;
            }
            if (check_ssid && (ssid->ssid_len == 0) && (wpa_s->connection_type == START_WPS_PBC)) {
                INNER_TRACE_STEP(9);
                continue;
            }
            INNER_TRACE_STEP(3);

            if (ssid->bssid_set &&
                    kal_mem_cmp(bss->bssid, ssid->bssid, ETH_ALEN) != 0) {
                wpa_printf(TRACE_GROUP_1, "   skip - BSSID mismatch" MACSTR "\n",  MAC2STR(bss->bssid));
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_BSSID_MISMATCH);
                continue;
            }
            INNER_TRACE_STEP(4);

            if (!(ssid->key_mgmt & WPA_KEY_MGMT_NONE) &&
                    !(ssid->key_mgmt & WPA_KEY_MGMT_WPS) &&
                    !(ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA)) {
                wpa_printf(TRACE_GROUP_1, "   skip - non-WPA network not allowed\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_NON_WPA_NETWORK);
                continue;
            }
            INNER_TRACE_STEP(5);
            /*mtk80707
            filter out all AP WPA-ENABLED.
            if key_mgmt does not comply with OPEN mode, doesn't match*/
            if ((ssid->key_mgmt &
                    (WPA_KEY_MGMT_IEEE8021X | WPA_KEY_MGMT_PSK |
                     WPA_KEY_MGMT_FT_IEEE8021X | WPA_KEY_MGMT_FT_PSK |
                     WPA_KEY_MGMT_IEEE8021X_SHA256 |
                     WPA_KEY_MGMT_PSK_SHA256)) ||
                    (bss->wpa_ie_len != 0 || bss->rsn_ie_len != 0)) {
                wpa_printf(TRACE_GROUP_1, "   skip - WPA network\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_WPA_NETWORK);
                continue;
            }
            INNER_TRACE_STEP(6);
            if (!wpa_supplicant_match_privacy(bss, ssid)) {
                wpa_printf(TRACE_GROUP_1, "   skip - privacy mismatch\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_PRIVACY_MISMATCH);
                continue;
            }
            /*end */

            /* Karen:
             * Compare network type
             */
            INNER_TRACE_STEP(7);
            if ((ssid->network_type == WNDRV_SUPC_NETWORK_IBSS) &&
                    (bss->network_type != WNDRV_SUPC_NETWORK_IBSS)) {
                wpa_printf(TRACE_GROUP_1, "	skip - network type mismatch\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_NETWORK_TYPE_MISMATCH);
                continue;
            } else if ((ssid->network_type == WNDRV_SUPC_NETWORK_INFRASTRUCTURE)
                       && (bss->network_type != WNDRV_SUPC_NETWORK_INFRASTRUCTURE)) {
                wpa_printf(TRACE_GROUP_1, "	skip - network type mismatch\n");
//              kal_trace(DBG_CONN,INFO_SUPC_SKIP_NETWORK_TYPE_MISMATCH);
                continue;
            }

            ssid->mode = bss->network_type;
            //      selected = bss;
            *selected_ssid = ssid;
            INNER_TRACE_STEP(8);
#ifdef __WPS_SUPPORT__
            /*in wpa_supplicant_wps_cred, it set ssid's cipher and
            group cipher to default_cipher, and let it be */
            if (WPA_KEY_MGMT_WPS & ssid->key_mgmt ||
                    KAL_TRUE == wpa_s->wps_success) {
                ssid->proto &= ~(WPA_PROTO_WPS_WPA2 | WPA_PROTO_WPS_WPA | WPA_PROTO_WPS_WPA2);
                wpa_printf(TRACE_GROUP_1, "set WPA_PROTO_WPS_OPEN info into WPS SSID \n");
//              kal_trace(DBG_WPS,INFO_SUPC_WPS_SET_OPENINFO);
                ssid->proto |= WPA_PROTO_WPS_OPEN;
                //wpa_printf(MSG_DBG_SCAN, "check ssid->proto=%d ", ssid->proto);
                ssid->pairwise_cipher = WPA_CIPHER_NONE;
                ssid->group_cipher = WPA_CIPHER_NONE;
                //wpa_printf(MSG_DBG_SCAN, "check pair_cipher=%d gp_cipher=%d",
                //ssid->pairwise_cipher, ssid->group_cipher);

//              kal_trace(DBG_WPS,INFO_SUPC_WPS_CHECK_PROTO,ssid->proto, ssid->pairwise_cipher, ssid->group_cipher);
            }
#endif

            //wpa_hexdump(MSG_INFO, "select SSID:", (kal_char *)bss->ssid, bss->ssid_len);
            wpa_printf(TRACE_GROUP_1, "select  non-WPA AP: " MACSTR "\n", MAC2STR(bss->bssid));

//          kal_trace(DBG_CONN,INFO_SUPC_SELECTED_SSID);
//          kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s",bss->ssid);
//          kal_trace(DBG_CONN,INFO_SUPC_SELECTED_NON_WPA_BSSID,MAC2STR(bss->bssid));
            return bss;
        }
    }
    return NULL;
}


struct wndrv_supc_bss_info_struct *
wpa_supplicant_select_bss(
    struct wpa_supplicant *wpa_s,
    struct wpa_ssid *group,
    struct wpa_ssid **selected_ssid
)
{
    wndrv_supc_bss_info_struct  *selected = NULL;

    wpa_printf(TRACE_GROUP_1, "Selecting BSS from priority group %d\n",
               group->priority);
//  kal_trace(DBG_CONN,INFO_SUPC_SEARCH_PRIO,group->priority);
    selected = wpa_supplicant_select_bss_wpa(wpa_s, group, selected_ssid);
    if (selected) {
        return selected;
    }
    return wpa_supplicant_select_bss_non_wpa(wpa_s, group, selected_ssid);
}


int wpa_supplicant_get_scan_results(struct wpa_supplicant *wpa_s)
{
#define SCAN_AP_LIMIT 64
//  struct wndrv_supc_bss_info_struct *results, *tmp;
//  int num;

#if 0 //Joseph remark
    results = wifi_get_ctrl_buffer(SCAN_AP_LIMIT * sizeof(struct wndrv_supc_bss_info_struct));
    if (results == NULL) {
        wpa_printf(MSG_WARNING, "Failed to allocate memory for scan "
                   "results");
        return -1;
    }

    num = wpa_drv_get_scan_results(wpa_s, results, SCAN_AP_LIMIT);
    wpa_printf(MSG_DEBUG, "Scan results: %d", num);
    if (num < 0) {
        wpa_printf(MSG_DEBUG, "Failed to get scan results");
        wifi_free_ctrl_buffer(results);
        return -1;
    }
    if (num > SCAN_AP_LIMIT) {
        wpa_printf(MSG_INFO, "Not enough room for all APs (%d < %d)",
                   num, SCAN_AP_LIMIT);
        num = SCAN_AP_LIMIT;
    }

    /* Free unneeded memory for unused scan result entries */
    //tmp = realloc(results, num * sizeof(struct wndrv_supc_bss_info_struct));
    tmp = wifi_get_ctrl_buffer(num * sizeof(struct wndrv_supc_bss_info_struct));
    kal_mem_cpy(tmp, results, sizeof(results));
    wifi_free_ctrl_buffer(results);
    if (tmp || num == 0) {
        results = tmp;
    }

    //free(wpa_s->scan_results);
    //wpa_s->scan_results = results;
    kal_mem_cpy(&(wpa_s->scan_results[0]), results, sizeof(results));
    wpa_s->num_scan_results = num;
#endif //#if 0

    return 0;
}


void wpa_supplicant_scan_results(struct wpa_supplicant *wpa_s)
{
    int num, prio;
    struct wndrv_supc_bss_info_struct *selected = NULL;
    struct wpa_ssid *ssid;
    struct wndrv_supc_bss_info_struct *results;

    if (wpa_supplicant_get_scan_results(wpa_s) < 0) {
        wpa_printf(TRACE_GROUP_1, "Failed to get scan results - try "
                   "scanning again\n");
//      kal_trace(DBG_SCAN,INFO_SUPC_FAIL_GET_SCAN_RESULT);
        wpa_supplicant_req_scan(wpa_s, 1, 0);
        return;
    }
    results = &(wpa_s->scan_results[0]);
    num = wpa_s->num_scan_results;

    while (selected == NULL) {
        for (prio = 0; prio < SUPC_MAX_SSID_CONFIG_PRIORITY; prio++) {
            selected = wpa_supplicant_select_bss(wpa_s, wpa_s->conf->pssid[prio], &ssid);
            if (selected) {
                break;
            }
        }
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
        if (selected == NULL && wpa_s->temporal_blacklist) {
            //wpa_printf(MSG_DEBUG, "No APs found - clear temporal blacklist "
            //"and try again");
            kal_trace(DBG_CONN, INFO_SUPC_NO_AP_CLR_T_BLACKLIST);
            wpa_auth_timeout_list_clear(wpa_s);
            wpa_blacklist_clear_temporal(wpa_s);
            supc_blacklist_update(wpa_s);
        } else if (selected == NULL) {
            break;
        }
#else
        if (selected == NULL && wpa_s->blacklist) {
            //wpa_printf(MSG_DEBUG, "No APs found - clear blacklist and try again");
            wpa_blacklist_clear(wpa_s);
        } else if (selected == NULL) {
            break;
        }
#endif
    }

    if (selected) {
        if (wpa_s->reassociate ||
                memcmp(selected->bssid, wpa_s->bssid, ETH_ALEN) != 0) {
            //wpa_supplicant_scard_init(wpa_s, ssid);
#ifdef __WIFI_CHIP_DYM_POWERON__
            if (wpa_s->chip_state != CHIP_POWER_ON) { //Kinki add for power-saving
                wpa_s->chip_state = CHIP_POWER_ON;
                supc_send_wndrv_chip_poweron_req();
            }
#endif
            wpa_supplicant_associate(wpa_s, selected, ssid);
        } else {
            //wpa_printf(MSG_DEBUG, "Already associated with the "
            //"selected AP. reassoc flag %d", wpa_s->reassociate );
//          kal_trace(DBG_CONN,INFO_SUPC_AL_ASSOC_AP, wpa_s->reassociate );
        }
        rsn_preauth_scan_results(wpa_s, results, num);
    } else {
        //wpa_printf(MSG_DEBUG, "No suitable AP found.");
//      kal_trace(DBG_SCAN,INFO_SUPC_NO_SUITABLE_AP);
        wpa_supplicant_req_scan(wpa_s, 3, 0);
    }
}


static int wpa_get_beacon_ie(struct wpa_supplicant *wpa_s)
{
    int i, ret = 0;
    struct wndrv_supc_bss_info_struct *results, *curr = NULL;

    results = wpa_s->scan_results;
    if (results == NULL) {
        return -1;
    }

    for (i = 0; i < wpa_s->num_scan_results; i++) {
        if (memcmp(results[i].bssid, wpa_s->bssid, ETH_ALEN) == 0) {
            curr = &results[i];
            break;
        }
    }

    if (curr) {
        SUPC_DEALLOC_BUFF(wpa_s->ap_wpa_ie);
        wpa_s->ap_wpa_ie_len = curr->wpa_ie_len;
        if (curr->wpa_ie_len) {
            wpa_s->ap_wpa_ie = SUPC_ALLOC_BUFF(wpa_s->ap_wpa_ie_len);
            if (wpa_s->ap_wpa_ie) {
                memcpy(wpa_s->ap_wpa_ie, curr->wpa_ie,
                       curr->wpa_ie_len);
            } else {
                ret = -1;
            }
        } else {
            wpa_s->ap_wpa_ie = NULL;
        }

        SUPC_DEALLOC_BUFF(wpa_s->ap_rsn_ie);
        wpa_s->ap_rsn_ie_len = curr->rsn_ie_len;
        if (curr->rsn_ie_len) {
            wpa_s->ap_rsn_ie = SUPC_ALLOC_BUFF(wpa_s->ap_rsn_ie_len);
            if (wpa_s->ap_rsn_ie) {
                memcpy(wpa_s->ap_rsn_ie, curr->rsn_ie,
                       curr->rsn_ie_len);
            } else {
                ret = -1;
            }
        } else {
            wpa_s->ap_rsn_ie = NULL;
        }
    } else {
        ret = -1;
    }

    return ret;
}


int wpa_supplicant_get_beacon_ie(struct wpa_supplicant *wpa_s)
{
    if (wpa_get_beacon_ie(wpa_s) == 0) {
        return 0;
    }

    /* No WPA/RSN IE found in the cached scan results. Try to get updated
     * scan results from the driver. */
    if (wpa_supplicant_get_scan_results(wpa_s) < 0) {
        return -1;
    }

    return wpa_get_beacon_ie(wpa_s);
}


#ifdef CONFIG_XSUPPLICANT_IFACE
static void wpa_supplicant_dot1x_receive(int sock, void *eloop_ctx,
        void *sock_ctx)
{
    struct wpa_supplicant *wpa_s = eloop_ctx;
    u8 buf[128];
    int res;

    res = recv(sock, buf, sizeof(buf), 0);
    //wpa_printf(MSG_DEBUG, "WPA: Receive from dot1x (Xsupplicant) socket "
    // "==> %d", res);
    if (res < 0) {
        perror("recv");
        return;
    }

    if (res != PMK_LEN) {
        //wpa_printf(MSG_WARNING, "WPA: Invalid master key length (%d) "
        //              "from dot1x", res);
        return;
    }

    //wpa_hexdump(MSG_DEBUG, "WPA: Master key (dot1x)", buf, PMK_LEN);
    if (wpa_s->key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
        memcpy(wpa_s->pmk, buf, PMK_LEN);
        wpa_s->ext_pmk_received = 1;
    } else {
        //wpa_printf(MSG_INFO, "WPA: Not in IEEE 802.1X mode - dropping "
        // "dot1x PMK update (%d)", wpa_s->key_mgmt);
    }
}


static int wpa_supplicant_802_1x_init(struct wpa_supplicant *wpa_s)
{
    int s;
    struct sockaddr_un addr;

    s = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    addr.sun_path[0] = '\0';
    snprintf(addr.sun_path + 1, sizeof(addr.sun_path) - 1,
             "wpa_supplicant");
    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        close(s);
        return -1;
    }

    wpa_s->dot1x_s = s;
    eloop_register_read_sock(s, wpa_supplicant_dot1x_receive, wpa_s,
                             NULL);
    return 0;
}
#endif /* CONFIG_XSUPPLICANT_IFACE */

#if 0
static int wpa_supplicant_set_driver(struct wpa_supplicant *wpa_s,
                                     const char *name)
{
    int i;

    if (wpa_s == NULL) {
        return -1;
    }

    if (wpa_supplicant_drivers[0] == NULL) {
        wpa_printf(MSG_ERROR, "No driver interfaces build into "
                   "wpa_supplicant.");
        return -1;
    }

    if (name == NULL) {
        /* default to first driver in the list */
        wpa_s->driver = wpa_supplicant_drivers[0];
        return 0;
    }

    for (i = 0; wpa_supplicant_drivers[i]; i++) {
        if (strcmp(name, wpa_supplicant_drivers[i]->name) == 0) {
            wpa_s->driver = wpa_supplicant_drivers[i];
            return 0;
        }
    }

    wpa_printf(MSG_DEBUG, "Unsupported driver '%s'.\n", name);
    return -1;
}
#endif

#if !CONFIG_DRIVER_MTK
static void wpa_supplicant_fd_workaround(void)
{
    int s, i;
    /* When started from pcmcia-cs scripts, wpa_supplicant might start with
     * fd 0, 1, and 2 closed. This will cause some issues because many
     * places in wpa_supplicant are still printing out to stdout. As a
     * workaround, make sure that fd's 0, 1, and 2 are not used for other
     * sockets. */
    for (i = 0; i < 3; i++) {
        s = open("/dev/null", O_RDWR);
        if (s > 2) {
            close(s);
            break;
        }
    }
}
#endif

int wpa_supplicant_driver_init(struct wpa_supplicant *wpa_s,
                               int wait_for_interface)
{
    static int interface_count = 0;

    wpa_s->l2 = l2_packet_init(wpa_s->ifname,
                               wpa_s->own_addr,
                               ETH_P_EAPOL,
                               wpa_supplicant_rx_eapol, wpa_s);

    if (l2_packet_get_own_addr(wpa_s->l2, wpa_s->own_addr)) {
        //fprintf(stderr, "Failed to get own L2 address\n");
        return -1;
    }

    //wpa_printf(MSG_DEBUG, "Own MAC address: " MACSTR,
    //MAC2STR(wpa_s->own_addr));

#if 0 //Joseph remark
    if (wpa_drv_set_wpa(wpa_s, 1) < 0) {
        //fprintf(stderr, "Failed to enable WPA in the driver.\n");
        return -1;
    }
#endif //#if 0

    wpa_clear_keys(wpa_s, NULL);

    /* Make sure that TKIP countermeasures are not left enabled (could
     * happen if wpa_supplicant is killed during countermeasures. */
    //wpa_drv_set_countermeasures(wpa_s, 0);

    //wpa_drv_set_drop_unencrypted(wpa_s, 1);

    wpa_s->prev_scan_ssid = BROADCAST_SSID_SCAN;
    //wpa_supplicant_req_scan(wpa_s, interface_count, 100000);
    interface_count++;

    return 0;
}

#if 0
static void usage(void)
{
    int i;
    wpa_printf(MSG_DEBUG, "%s\n\n%s\n"
               "usage:\n"
               "  wpa_supplicant [-BddehLqqvw] -i<ifname> -c<config file> "
               "[-D<driver>] \\\n"
               "      [-N -i<ifname> -c<conf> [-D<driver>] ...]\n"
               "\n"
               "drivers:\n",
               wpa_supplicant_version, wpa_supplicant_license);

    for (i = 0; wpa_supplicant_drivers[i]; i++) {
        wpa_printf(MSG_DEBUG, "  %s = %s\n",
                   wpa_supplicant_drivers[i]->name,
                   wpa_supplicant_drivers[i]->desc);
    }

    wpa_printf(MSG_DEBUG, "options:\n"
               "  -B = run daemon in the background\n"
               "  -d = increase debugging verbosity (-dd even more)\n"
               "  -K = include keys (passwords, etc.) in debug output\n"
               "  -t = include timestamp in debug messages\n"
#ifdef CONFIG_XSUPPLICANT_IFACE
#ifdef IEEE8021X_EAPOL
               "  -e = use external IEEE 802.1X Supplicant (e.g., "
               "xsupplicant)\n"
               "       (this disables the internal Supplicant)\n"
#endif /* IEEE8021X_EAPOL */
#endif /* CONFIG_XSUPPLICANT_IFACE */
               "  -h = show this help text\n"
               "  -L = show license (GPL and BSD)\n"
               "  -q = decrease debugging verbosity (-qq even less)\n"
               "  -v = show version\n"
               "  -w = wait for interface to be added, if needed\n"
               "  -N = start describing new interface\n");
}


static void license(void)
{
    wpa_printf(MSG_DEBUG, "%s\n\n%s\n",
               wpa_supplicant_version, wpa_supplicant_full_license);
}


static struct wpa_supplicant *wpa_supplicant_alloc(void)
{
    struct wpa_supplicant *wpa_s;

    wpa_s = SUPC_ALLOC_BUFF(sizeof(*wpa_s));
    if (wpa_s == NULL) {
        return NULL;
    }
    memset(wpa_s, 0, sizeof(*wpa_s));
#ifdef CONFIG_XSUPPLICANT_IFACE
    wpa_s->dot1x_s = -1;
#endif /* CONFIG_XSUPPLICANT_IFACE */

    return wpa_s;
}


static int wpa_supplicant_init(struct wpa_supplicant *wpa_s,
                               const char *confname, const char *driver,
                               const char *ifname)
{
    wpa_printf(MSG_DEBUG, "Initializing interface '%s' conf '%s' driver "
               "'%s'", ifname, confname, driver ? driver : "default");

    if (wpa_supplicant_set_driver(wpa_s, driver) < 0) {
        return -1;
    }

    if (confname) {
        wpa_s->confname = rel2abs_path(confname);
        if (wpa_s->confname == NULL) {
            wpa_printf(MSG_ERROR, "Failed to get absolute path "
                       "for configuration file '%s'.", confname);
            return -1;
        }
        wpa_printf(MSG_DEBUG, "Configuration file '%s' -> '%s'",
                   confname, wpa_s->confname);
        wpa_s->conf = wpa_config_read(wpa_s->confname);
        if (wpa_s->conf == NULL) {
            wpa_printf(MSG_DEBUG, "Failed to read configuration file '%s'.\n",
                       wpa_s->confname);
            return -1;
        }
    }

    if (wpa_s->conf == NULL || wpa_s->conf->ssid == NULL) {
        usage();
        wpa_printf(MSG_DEBUG, "\nNo networks (SSID) configured.\n");
        return -1;
    }

    if (ifname == NULL) {
        usage();
        wpa_printf(MSG_DEBUG, "\nInterface name is required.\n");
        return -1;
    }
    if (strlen(ifname) >= sizeof(wpa_s->ifname)) {
        wpa_printf(MSG_DEBUG, "Too long interface name '%s'.\n", ifname);
        return -1;
    }
    strncpy(wpa_s->ifname, ifname, sizeof(wpa_s->ifname));

    return 0;
}


static void wpa_supplicant_deinit(struct wpa_supplicant *wpa_s)
{
#if 0 //Joseph remark
    if (wpa_s->drv_priv) {
        if (wpa_drv_set_wpa(wpa_s, 0) < 0) {
            //fprintf(stderr, "Failed to disable WPA in the "
            //  "driver.\n");
        }

        //wpa_drv_set_drop_unencrypted(wpa_s, 0);
        //wpa_drv_set_countermeasures(wpa_s, 0);

        //wpa_drv_deinit(wpa_s);
    }
#endif //#if 0
    wpa_supplicant_cleanup(wpa_s);
}
#endif
#ifdef __WAPI_SUPPORT__
static kal_uint32  expect_rsp[4];
extern void supc_send_wndrv_query_cap(local_para_struct *local_para_ptr);

/* Challenge underlying MT5921 */
static int wapi_query_cap(kal_uint32 *rsp)
{
    kal_uint32  challenge_rsp[4];
    wndrv_supc_query_capability_req_struct *req_local_para_ptr = NULL;

    /* already srand() */
    challenge_rsp[0] = (rand()) << 16 | (rand());
    challenge_rsp[1] = (rand()) << 16 | (rand());
    challenge_rsp[2] = (rand()) << 16 | (rand());
    challenge_rsp[3] = (rand()) << 16 | (rand());

    //wpa_printf(MSG_WARNING, "wapi_check challenge 0x%08x-0x%08x-0x%08x-0x%08x",
    //   challenge_rsp[0], challenge_rsp[1], challenge_rsp[2], challenge_rsp[3]);
    kal_trace(DBG_WAPI, INFO_SUPC_CHECK_WAPI_CHALLENGE, challenge_rsp[0], challenge_rsp[1], challenge_rsp[2], challenge_rsp[3]);
    prepare_cap_check(challenge_rsp, expect_rsp);


    //wpa_printf(MSG_WARNING, "wapi_check response 0x%08x-0x%08x-0x%08x-0x%08x",
    //expect_rsp[0], expect_rsp[1], expect_rsp[2], expect_rsp[3]);
    kal_trace(DBG_WAPI, INFO_SUPC_CHECK_WAPI_RESPONSE,
              expect_rsp[0], expect_rsp[1], expect_rsp[2], expect_rsp[3]);

    /* send to WNDRV query capability */
    /*req_local_para_ptr =
        construct_local_para(sizeof(wndrv_supc_query_capability_req_struct), TD_UL);*/
    req_local_para_ptr =
        construct_local_para(sizeof(wndrv_supc_query_capability_req_struct), TD_RESET);

    SUPC_ASSERT(req_local_para_ptr);
    req_local_para_ptr->challenge[0] = challenge_rsp[0];
    req_local_para_ptr->challenge[1] = challenge_rsp[1];
    req_local_para_ptr->challenge[2] = challenge_rsp[2];
    req_local_para_ptr->challenge[3] = challenge_rsp[3];

    supc_send_wndrv_query_cap((local_para_struct *) req_local_para_ptr);

    return TRUE;
}


int wpa_drv_set_key(
    const u8 *addr, int key_idx, int set_tx,
    const u8 *seq, int seq_len,
    const u8 *key, int key_len)
{
    wndrv_supc_add_key_req_struct *local_para_ptr = NULL;

    SUPC_ASSERT(seq_len <= 16);
    SUPC_ASSERT((key_idx == 1) || (key_idx == 0));

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_add_key_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_add_key_req_struct), TD_RESET);

    local_para_ptr->ucKeyID = key_idx;
    local_para_ptr->u4LenWPIEK = 16;
    local_para_ptr->u4LenWPICK = 16;

    if (addr == NULL ||
            memcmp(addr, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0)
        /* group key */{
        local_para_ptr->eKeyType = 1; /* ENUM_WPI_GROUP_KEY */
    } else {
        local_para_ptr->eKeyType = 0; /* ENUM_WPI_PAIRWISE_KEY */
    }

    if (set_tx) {
        local_para_ptr->eDirection = 2; /* ENUM_WPI_TX */
    }

    /* aucAddrIndex no idea */
    SUPC_ASSERT(key_len == 32);

    memcpy(local_para_ptr->aucWPIEK, key, 16);
    memcpy(local_para_ptr->aucWPICK, key + 16, 16);

    if (memcmp(wpa_s->cap_check, expect_rsp, sizeof(expect_rsp))) {
        local_para_ptr->aucWPIEK[4] = 0;
    }
    /* What's seq_len for ?? aucPN */
    SUPC_ASSERT(local_para_ptr->u4LenWPIEK == 16);
    SUPC_ASSERT(local_para_ptr->u4LenWPICK == 16);
    supc_send_wndrv_add_key_req((local_para_struct *)local_para_ptr);

    return KAL_TRUE;
}

static void wpa_supplicant_rx_wai(void *ctx, unsigned char *src_addr, unsigned char *buf, wpa_size_t len)
{
    WAI_RX_packets_indication(buf, len);
}

void WIFI_Action_Deauth(void)
{
//  struct wapi_asue_st *wpa_s = (struct wapi_asue_st *)eloop.user_data;
    //wpa_printf(MSG_WARNING, "WIFI_Action_Deauth");
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_DEAUTH);

    wpa_supplicant_disassociate(wpa_s, 15);
    /*MAUI_03158363
    the scan req below will cause wndrv,supc enter a dead loop
    scan->join->wapi auth -> fail(error psk or certificate) -> scan
    and the retry is no need here, becuase authentication timer will do retry and control the entrie auth flow
    so just remove it

    */
    wpa_s->wpa_state = WPA_SCANNING;
    supc_stop_handover_timer();


    //wpa_supplicant_req_scan(wpa_s, 2, 0);
}

unsigned long WIFI_TX_packet(const char *pbuf, int length)
{
    wndrv_supc_data_req_struct *local_para_ptr = NULL;
    peer_buff_struct *peer_buff_ptr = NULL;
    kal_uint8 *pdu_ptr = NULL;
    kal_uint16 pdu_len = 0;
    u8 bssid[ETH_ALEN];

    if (wpa_driver_mtk_get_bssid(wpa_s, bssid) < 0) {
        //wpa_printf(MSG_WARNING, "Failed to read BSSID for WAI "
        //"request");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_FAIL_READ_BSSID_FOR_WAI);
        return 0;
    }

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    kal_mem_cpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    kal_mem_cpy(local_para_ptr->dest_mac_addr, bssid, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_WAI);

    //peer_buff_ptr = construct_peer_buff(length, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    peer_buff_ptr = construct_peer_buff(length, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    kal_mem_set(pdu_ptr, 0, pdu_len);

    kal_mem_cpy(pdu_ptr, pbuf, length);

    if (memcmp(wpa_s->cap_check, expect_rsp, sizeof(expect_rsp))) {
        if (length > 8) {
            pdu_ptr[8] = 0;
        }
    }
    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);

    return 1;
#if 0
    struct wapi_asue_st *wpa_s = (struct wapi_asue_st *)eloop.user_data;
    int ret;

    ret = l2_packet_send(wpa_s->l2, wpa_s->bssid, 0x88b4, (const u8 *)pbuf, (unsigned  int)length);

    if (ret < 0) {
        return 0;
    } else {
        return (unsigned long)ret;
    }
#endif
}

int WIFI_group_key_set(const unsigned char *pKeyValue, int keylength, int key_idx, const unsigned char *keyIV)
{
    wpa_hexdump(MSG_DEBUG, "WIFI_group_key_set: KEYSC", (const char *)pKeyValue, keylength);

    if (wpa_drv_set_key(
                (u8 *) "\xff\xff\xff\xff\xff\xff",
                key_idx, 1, keyIV, 16,
                pKeyValue, keylength) < 0) {
        //wpa_printf(MSG_WARNING, "WPA: Failed to set MSK to "
        //"the driver.");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_FAIL_SET_MSK_TO_DRV);
        return -1;
    }

    return 0;
}

int WIFI_unicast_key_set(const char *pKeyValue, int keylength, int key_idx)
{
//  struct wapi_asue_st *wpa_s = (struct wapi_asue_st *)eloop.user_data;

    int keylen, rsclen;
    u8 *key_rsc;
    u8 null_rsc[16] = { 0, 0, 0, 0, 0, 0, 0, 0 };


    //wpa_printf(MSG_DEBUG, "WIFI_unicast_key_set: Installing USK to the driver.");
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_INSTALL_USK_TO_DRV);

    keylen = 32;
    rsclen = 16;

    key_rsc = null_rsc;

    if (wpa_drv_set_key(
                wpa_s->bssid, key_idx, 1, key_rsc, rsclen,
                (u8 *) pKeyValue, keylen) < 0) {
        //wpa_printf(MSG_WARNING, "WIFI_unicast_key_set: Failed to set PTK to the driver");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_INSTALL_PTK_FAIL);
        return -1;
    }

    return 0;
}

void WIFI_update_status(kal_bool fgComplete)
{

    wpa_supplicant_wapi_key_neg_complete(wpa_s, wpa_s->bssid, KAL_TRUE);
}

int WIFI_read_user_cert(void *output_buf, int buf_size)
{
    int iFileLen = 0;

    SUPC_ASSERT(output_buf);
    SUPC_ASSERT(buf_size);

    if ((wpa_s->selected_ssid == NULL) || (wpa_s->selected_ssid->client_cert == NULL)) {
        //wpa_printf(MSG_DEBUG,"no selected ssid or user cert\n");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_NO_SSID_OR_CERT);
        return -1;
    }

    if ((iFileLen = supc_read_file(wpa_s->selected_ssid->client_cert,
                                   output_buf, buf_size)) < 0) {
        /* TODO what should done? */
        SUPC_ASSERT(0);
    } else {
        wpa_hexdump(MSG_DEBUG, "user cert content", (char *)output_buf, iFileLen);
    }

    //wpa_printf(MSG_DEBUG,"user_len %d\n", iFileLen);
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_USER_CERT_LEN, iFileLen);

    return iFileLen;
}

int WIFI_read_as_cert(void *output_buf, int buf_size)
{
    int iFileLen;

    SUPC_ASSERT(output_buf);
    SUPC_ASSERT(buf_size);

    if ((wpa_s->selected_ssid == NULL) || (wpa_s->selected_ssid->client_cert2 == NULL)) {
        //wpa_printf(MSG_DEBUG,"no selected ssid or as cert\n");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_NO_SSID_OR_AS_CERT);
        return -1;
    }

    if ((iFileLen = supc_read_file(wpa_s->selected_ssid->client_cert2,
                                   output_buf, buf_size)) < 0) {
        /* TODO what should done? */
        SUPC_ASSERT(0);
    } else {
        wpa_hexdump(MSG_DEBUG, "as cert content", (char *)output_buf, iFileLen);
    }

    //wpa_printf(MSG_DEBUG,"as_len %d\n", iFileLen);
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_AS_CERT_LEN, iFileLen);

    return iFileLen;
}

int WIFI_read_private_key(void *output_buf, int buf_size)
{
    int iFileLen;

    SUPC_ASSERT(output_buf);
    SUPC_ASSERT(buf_size);

    if ((wpa_s->selected_ssid == NULL) || (wpa_s->selected_ssid->private_key == NULL)) {
        //wpa_printf(MSG_DEBUG,"no selected ssid or private key\n");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_NO_SSID_OR_PRIV_KEY);
        return -1;
    }

    if ((iFileLen = supc_read_file(wpa_s->selected_ssid->private_key,
                                   output_buf, buf_size)) < 0) {
        /* TODO what should done? */
        SUPC_ASSERT(0);
    } else {
        wpa_hexdump(MSG_DEBUG, "private key", (char *)output_buf, iFileLen);
    }

    //wpa_printf(MSG_DEBUG,"private key %d\n", iFileLen);
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_PRIV_KEY_LEN, iFileLen);

    return iFileLen;
}

int wapi_init(struct wpa_supplicant *wpa_s,
              const char *ifname, const u8 *own_addr
             )
{
    struct l2_packet_data *l2;

    l2 = SUPC_ALLOC_BUFF(sizeof(struct l2_packet_data));
    if (l2 == NULL) {
        return NULL;
    }
    memset(l2, 0, sizeof(*l2));

    l2->rx_callback_ctx = NULL;

    if (own_addr) {
        memcpy(l2->own_addr, own_addr, ETH_ALEN);
    }

    if (WIFI_lib_init()) {
        return -1;
    }

    /* init l2 */
    wpa_s->l2_wapi = l2;
    wpa_s->l2_wapi->rx_callback = wpa_supplicant_rx_wai;

    /* Check capability */
    wapi_query_cap(wpa_s->cap_check);

    return 0;
}


void wapi_deinit(struct wpa_supplicant *wpa_s)
{

    WIFI_lib_exit();
}
#endif

void wpa_supplicant_eapol_cb(struct eapol_sm *eapol, int success, void *ctx)
{

#if 0//def __WPS_SUPPORT__
    struct wpa_supplicant *p = ctx;
#endif
    INNER_TRACE_STEP(1);
    wpa_s->wps_eapol_cb_ret = KAL_FALSE;
#ifdef __WPS_SUPPORT__
    wpa_s->wps_eapol_cb_ret = (kal_bool)wpas_wps_eapol_cb(ctx);
    wpa_printf(TRACE_GROUP_1, "wps_eapol_cb_ret=%d\n", wpa_s->wps_eapol_cb_ret);
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_EAPLO_CB, wpa_s->wps_eapol_cb_ret);
#endif
    return;
}
