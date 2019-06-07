/*
 * wpa_supplicant / WPS integration
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */


#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif

//#include "kal_release.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
//#include "supc_trc.h"
#include "wndrv_supc_types.h"
#include "supc_common.h"
#include "wpa.h"

#include "wpa_supplicant_i.h"

#include "supc_wpa_common.h"
#include "config_ssid.h"
#include "supc_config.h"
#include "eap.h"
#include "wpa_supplicant_i.h"

#include "uuid.h"
//#include "wpa_ctrl.h"
#include "eap_wsc_common.h"
//#include "blacklist.h"
#include "wpa.h"
#include "wps_supplicant.h"
#include "wpabuf.h"
#include "wpa_supplicant.h"
#define WPS_PIN_SCAN_IGNORE_SEL_REG 3

static void wpas_wps_timeout(void *eloop_ctx, void *timeout_ctx);
static void wpas_clear_wps(struct wpa_supplicant *wpa_s);
int eap_is_wps_pin_enrollee(struct wpa_ssid *ssid);
extern int wpa_driver_mtk_get_capa(void *priv, struct wpa_driver_capa *capa);
extern void pbkdf2_sha1(const char *passphrase, const char *ssid, kal_uint16 ssid_len,
                        int iterations, kal_uint8 *buf, kal_uint16 buflen);
extern void supc_cb_wps_status(WPS_STATE stat);
#if 0
extern kal_uint16 wndrv_get_dev_category(void);
extern kal_uint16 wndrv_get_dev_sub_cate(void);
extern kal_uint32 wndrv_get_cus_oui(void);
extern kal_uint8  wndrv_get_rf_bands(void);
#endif
void supc_dbg_show_ssid_ex(wpa_ssid *ssid);

int wpas_wps_eapol_cb(struct wpa_supplicant *wpa_s)
{
    INNER_TRACE_STEP(1);
    supc_stop_wps_walk_timer();

    //wpa_printf(MSG_DEBUG, "wps_success=%d ", wpa_s->wps_success);
//  kal_trace(DBG_WPS,INFO_WPS_SUCCESS_CODE, wpa_s->wps_success);
    supc_dbg_show_ssid_ex(wpa_s->current_ssid);
    /*under PIN-MODE, after process M2D, it goes here. */
    if (0 == wpa_s->wps_success &&
            wpa_s->current_ssid &&
            eap_is_wps_pin_enrollee(wpa_s->current_ssid)) {
        u8 *bssid = wpa_s->bssid;
        wpa_hexdump(MSG_DEBUG, "EAP_WSC fail with SSID", (const char *)wpa_s->ssid, wpa_s->ssid_len);
        //wpa_printf(MSG_DEBUG, "EAP_WSC fail with BSSID " MACSTR "",MAC2STR(bssid));
//      kal_trace(DBG_WPS,INFO_WPS_FAIL_WITH_BSSID,MAC2STR(bssid));
        wpa_supplicant_disassociate(wpa_s, REASON_DEAUTH_LEAVING);
        wpa_s->reassociate = 1;
        wpa_s->wpa_state = WPA_SCANNING;
        /*extend interval of next scan. it prevent supc from connecting AP frequently.
        let user have enough time to input pin-code*/
        wpa_supplicant_req_scan(wpa_s, 8, 0);
        wpa_s->blacklist_cleared = 0;
        return 1;
    }
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPS && wpa_s->current_ssid &&
            !(wpa_s->current_ssid->key_mgmt & WPA_KEY_MGMT_WPS)) {
        //wpa_printf(MSG_DEBUG, "WPS: Network configuration replaced - "
        //"try to associate with the received credential");
//      kal_trace(DBG_WPS,INFO_WPS_ASSOC_WITH_NEW_CRED);
        /*update wps_s's attr*/
        //wpa_printf(MSG_DEBUG, "update wpa_s with cur_ssid");
//      kal_trace(DBG_WPS,INFO_WPS_UPDATE_CURRENT_SSID);
        wpa_s->key_mgmt = wpa_s->current_ssid->key_mgmt;
        wpa_s->proto = wpa_s->current_ssid->proto;
        wpa_s->pairwise_cipher =  wpa_s->current_ssid->pairwise_cipher;
        wpa_s->group_cipher =  wpa_s->current_ssid->group_cipher;
        //wpa_printf(MSG_DEBUG, "lastest wpa_s key_mgmt=%d,"
        //"proto=%d, pair_cipher=%d, gp_cipher=%d",
        //wpa_s->key_mgmt, wpa_s->proto,
        //wpa_s->pairwise_cipher, wpa_s->group_cipher);
//      kal_trace(DBG_WPS,INFO_WPS_LASTEST_KEY_MGMT,wpa_s->key_mgmt, wpa_s->proto,
//            wpa_s->pairwise_cipher, wpa_s->group_cipher);

        wpa_supplicant_cancel_auth_timeout(wpa_s);

        pmksa_cache_del(wpa_s->bssid);
        wpa_supplicant_disassociate(wpa_s, REASON_DEAUTH_LEAVING);
        wpa_s->reassociate = 1;

        wpa_s->auth_timer_event_id = NULL;
        wpa_s->wpa_state = WPA_SCANNING;
        wpa_supplicant_req_scan(wpa_s, 0, 0);
        wpa_s->blacklist_cleared = 0;
        return 1;
    }

    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPS && wpa_s->current_ssid) {
        //wpa_printf(MSG_DEBUG, "WPS: Registration completed - waiting "
        //"for external credential processing");
//      kal_trace(DBG_WPS,INFO_WPS_REGISTRATION_DONE_WAIT_CRED_PROCESS);
        wpa_supplicant_disassociate(wpa_s, REASON_DEAUTH_LEAVING);
        wpa_s->reassociate = 1;
        wpa_s->wpa_state = WPA_SCANNING;
        //wpa_supplicant_req_scan(wpa_s, 0, 0);
        wpa_s->blacklist_cleared = 0;
        return 1;
    }

    return 0;
}

static void wpas_wps_security_workaround(struct wpa_supplicant *wpa_s,
        struct wpa_ssid *ssid,
        const struct wps_credential *cred)
{
    struct wpa_driver_capa capa;
    size_t i;
    struct wndrv_supc_bss_info_struct bss;
    //const u8 *ie;
    struct wpa_ie_data adv;
    int wpa2 = 0, ccmp = 0;
    /*
     * Many existing WPS APs do not know how to negotiate WPA2 or CCMP in
     * case they are configured for mixed mode operation (WPA+WPA2 and
     * TKIP+CCMP). Try to use scan results to figure out whether the AP
     * actually supports stronger security and select that if the client
     * has support for it, too.
     */
    //change wpa_drv_get_capa to wpa_driver_mtk_get_capa
    if (wpa_driver_mtk_get_capa(wpa_s, &capa)) {
        return;    /* Unknown what driver supports */
    }

    if (wpa_supplicant_get_scan_results(wpa_s)
            || wpa_s->num_scan_results == 0) {
        return; /* Could not get scan results for checking advertised
             * parameters */
    }

    for (i = 0; i < wpa_s->num_scan_results; i++) {
        bss = wpa_s->scan_results[i];
        //wpa_printf(MSG_DEBUG, "fetch a ssid form wps->scan_result");
//      kal_trace(DBG_WPS,INFO_WPS_FETCH_SSID_FROM_SCAN_RESULT);
        wpa_hexdump(MSG_DEBUG, "SSID", (char *)bss.ssid, bss.ssid_len);
        /*
        wpa_printf(MSG_DEBUG, "Bssid=" MACSTR, MAC2STR(bss.bssid));
        wpa_hexdump(MSG_DEBUG, "wpa_IE", bss.wpa_ie, WNDRV_MAX_WPA_IE_LEN);
        wpa_hexdump(MSG_DEBUG, "rsn_IE", bss.rsn_ie, WNDRV_MAX_RSN_IE_LEN);
        if (bss.has_wps_ie)
        {
            wpa_hexdump(MSG_DEBUG, "wps_IE", bss.wps_ie, ELEM_WPS_PROBE_RESP_IE_LEN);
        }
        */
        /* MTK80707, must skip this step, cred->mac_addr is self MAC, and
        bss.bssid is AP's MAC. this compare is odd*/
        /*
        if (os_memcmp(bss.bssid, cred->mac_addr, ETH_ALEN) != 0)
            continue;
        */

        /*wndrv_supc_bss_info_struct has IE buffer as memeber*/
        //ie = wpa_scan_get_ie(bss, WLAN_EID_SSID);
        //if (ie == NULL)
        //  continue;
        if (bss.ssid_len != ssid->ssid_len || ssid->ssid == NULL ||
                os_memcmp(bss.ssid, ssid->ssid, ssid->ssid_len) != 0) {
            continue;
        }

        //wpa_printf(MSG_DEBUG, "WPS: AP found from scan results");
//      kal_trace(DBG_WPS,INFO_WPS_AP_FOUND_FROM_SCAN_RESULT);
        break;
    }

    if (i == wpa_s->num_scan_results) {
        //wpa_printf(MSG_DEBUG, "WPS: The AP was not found from scan "
        //"results - use credential as-is");
//      kal_trace(DBG_WPS,INFO_WPS_NO_AP_IN_SCAN_RESULT);
        return;
    }
    /*bss.rsn_ie contains WPA2 or RSN data
      bss.wpa_ie contains WPA data*/
    //ie = wpa_scan_get_ie(bss, WLAN_EID_RSN);
    /*here, modify the AP's properties. in the next workaround,
    supc can use this config to connect AP*/
    if (bss.rsn_ie_len
            && wpa_parse_wpa_ie(wpa_s, bss.rsn_ie, bss.rsn_ie_len, &adv) == 0) {
        wpa2 = 1;
        if (adv.pairwise_cipher & WPA_CIPHER_CCMP) {
            ccmp = 1;
        }
    } else {
        //ie = wpa_scan_get_vendor_ie(bss, WPA_IE_VENDOR_TYPE);
        if (bss.wpa_ie_len
                && (wpa_parse_wpa_ie(wpa_s, bss.wpa_ie, bss.wpa_ie_len, &adv) == 0)) {
            if (adv.pairwise_cipher & WPA_CIPHER_CCMP) {
                ccmp = 1;
            }
        }
    }

    //if (ie == NULL && (ssid->proto & WPA_PROTO_WPA) &&
    //   (ssid->pairwise_cipher & WPA_CIPHER_TKIP))
    if ((!bss.wpa_ie_len || !bss.rsn_ie_len)
            && (ssid->pairwise_cipher & WPA_CIPHER_TKIP)
            && (ssid->proto & WPA_PROTO_WPA)) {
        /*
         * TODO: This could be the initial AP configuration and the
         * Beacon contents could change shortly. Should request a new
         * scan and delay addition of the network until the updated
         * scan results are available.
         */
        //wpa_printf(MSG_DEBUG, "WPS: The AP did not yet advertise WPA "
        //"support - use credential as-is");
//      kal_trace(DBG_WPS,INFO_WPS_NOT_ADVERTISE_WPA_SUPPORT_YET);
        return;
    }

    if (ccmp && !(ssid->pairwise_cipher & WPA_CIPHER_CCMP) &&
            (ssid->pairwise_cipher & WPA_CIPHER_TKIP) &&
            (capa.key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK)) {
        //wpa_printf(MSG_DEBUG, "WPS: Add CCMP into the credential "
        //"based on scan results");
//      kal_trace(DBG_WPS,INFO_WPS_ADD_CCMP_INTO_CRED);
        /*
        if (wpa_s->conf->ap_scan == 1)
            ssid->pairwise_cipher |= WPA_CIPHER_CCMP;
        else
            ssid->pairwise_cipher = WPA_CIPHER_CCMP;
        */
        // supc process the scan result
        ssid->proto |= WPA_PROTO_RSN;
    }

    if (wpa2 && !(ssid->proto & WPA_PROTO_RSN) &&
            (ssid->proto & WPA_PROTO_WPA) &&
            (capa.enc & WPA_DRIVER_CAPA_ENC_CCMP)) {
        //wpa_printf(MSG_DEBUG, "WPS: Add WPA2 into the credential "
        //"based on scan results");
//      kal_trace(DBG_WPS,INFO_WPS_ADD_WPA2_INTO_CRED);
        /*
        if (wpa_s->conf->ap_scan == 1)
            ssid->proto |= WPA_PROTO_RSN;

        else
            ssid->proto = WPA_PROTO_RSN;
        */
        // supc process the scan result
        ssid->proto |= WPA_PROTO_RSN;
    }
}

static int wpa_supplicant_wps_cred(void *ctx,
                                   const struct wps_credential *cred)
{
    struct wpa_supplicant *wpa_s = ctx;
    struct wpa_ssid *ssid = wpa_s->current_ssid;
    u8 key_idx = 0;
    u16 auth_type;

    if ((wpa_s->conf->wps_cred_processing == 1 ||
            wpa_s->conf->wps_cred_processing == 2) && cred->cred_attr) {
        size_t blen = cred->cred_attr_len * 2 + 1;
        char *buf = os_malloc(blen);
        if (buf) {
            wpa_snprintf_hex(buf, blen,
                             cred->cred_attr, cred->cred_attr_len);
            wpa_printf(TRACE_GROUP_1, "%s%s\n",
                       WPS_EVENT_CRED_RECEIVED, buf);
            os_free(buf);
        }
        //wpa_supplicant_dbus_notify_wps_cred(wpa_s, cred);
    } else {
        wpa_printf(TRACE_GROUP_1, "%s\n", WPS_EVENT_CRED_RECEIVED);
    }
//      kal_trace(DBG_WPS,INFO_SUPC_WPS_EVENT_CRED_RECEIVED);

    wpa_hexdump_key(MSG_DEBUG, "WPS: Received Credential attribute",
                    (char *)cred->cred_attr, cred->cred_attr_len);

    if (wpa_s->conf->wps_cred_processing == 1) {
        return 0;
    }

    wpa_hexdump_ascii(MSG_DEBUG, "WPS: SSID", (char *)cred->ssid, cred->ssid_len);
    wpa_printf(TRACE_GROUP_1, "WPS: Authentication Type %x\n",
               cred->auth_type);
//  kal_trace(DBG_WPS,INFO_WPS_AUTHENTICATION_TYPE_CODE,cred->auth_type);
    wpa_printf(TRACE_GROUP_1, "WPS: Encryption Type %x\n", cred->encr_type);
//  kal_trace(DBG_WPS,INFO_WPS_ENCR_TYPE_CODE, cred->encr_type);
    wpa_printf(TRACE_GROUP_1, "WPS: Network Key Index %d\n", cred->key_idx);
//  kal_trace(DBG_WPS,INFO_WPS_NETWORK_KEY_INDEX_CODE, cred->key_idx);
    wpa_hexdump_key(MSG_DEBUG, "WPS: Network Key",
                    (char *)cred->key, cred->key_len);
    wpa_printf(TRACE_GROUP_1, "WPS: MAC Address " MACSTR "\n",
               MAC2STR(cred->mac_addr));
//  kal_trace(DBG_WPS,INFO_WPS_MAC_ADDR_IN_CRED,MAC2STR(cred->mac_addr));

    auth_type = cred->auth_type;
    if (auth_type == (WPS_AUTH_WPAPSK | WPS_AUTH_WPA2PSK)) {
        wpa_printf(TRACE_GROUP_1, "WPS: Workaround - convert mixed-mode "
                   "auth_type into WPA2PSK\n");
//      kal_trace(DBG_WPS,INFO_WPS_WA_CONVERT_TO_WPA2PSK);
        auth_type = WPS_AUTH_WPA2PSK;
    }

    if (auth_type != WPS_AUTH_OPEN &&
            auth_type != WPS_AUTH_SHARED &&
            auth_type != WPS_AUTH_WPAPSK &&
            auth_type != WPS_AUTH_WPA2PSK) {
        wpa_printf(TRACE_GROUP_1, "WPS: Ignored credentials for "
                   "unsupported authentication type 0x%x\n",
                   auth_type);
//      kal_trace(DBG_WPS,INFO_WPS_IGNOR_CRED_UNSUPPORTED_AHTH_TYPE,auth_type);
        return 0;
    }

    if (ssid && (ssid->key_mgmt & WPA_KEY_MGMT_WPS)) {
        wpa_printf(TRACE_GROUP_1, "WPS: Replace WPS network block based "
                   "on the received credential\n");
//      kal_trace(DBG_WPS,INFO_WPS_REPLACE_WPS_NETWORK_BLOCK);
        os_free(ssid->identity);
        ssid->identity = NULL;
        ssid->identity_len = 0;
        /*it set a constant data*/
        //os_free(ssid->phase1);
        ssid->phase1 = NULL;
        os_free(ssid->eap_methods);
        ssid->eap_methods = NULL;
    } else {
        wpa_printf(TRACE_GROUP_1, "WPS: Create a new network based on the "
                   "received credential\n");
//      kal_trace(DBG_WPS,INFO_WPS_CREATE_NEW_CREDENTIAL);
        ssid = wpa_config_add_network(wpa_s->conf);
        if (ssid == NULL) {
            return -1;
        }
    }

    wpa_config_set_network_defaults(ssid);

    //os_free(ssid->ssid);
    //ssid->ssid = os_malloc(cred->ssid_len);
    kal_mem_set(ssid->ssid, 0, MAX_SSID_LEN);
    if (ssid->ssid) {
        os_memcpy(ssid->ssid, cred->ssid, cred->ssid_len);
        ssid->ssid_len = cred->ssid_len;
    }

    //2016.04.19 add for use when get_credential_info command
    kal_mem_set(ssid->bssid, 0, ETH_ALEN);
    os_memcpy(ssid->bssid, cred->mac_addr, ETH_ALEN);

    switch (cred->encr_type) {
        case WPS_ENCR_NONE:
            break;
        case WPS_ENCR_WEP:
            if (cred->key_len <= 0) {
                break;
            }
            if (cred->key_len != 5 && cred->key_len != 13 &&
                    cred->key_len != 10 && cred->key_len != 26) {
                wpa_printf(TRACE_GROUP_1, "WPS: Invalid WEP Key length "
                           "%lu\n", (unsigned long) cred->key_len);
//          kal_trace(DBG_WPS,INFO_WPS_INVALID_WEP_KEY_LEN, (unsigned long) cred->key_len);
                return -1;
            }
            if (cred->key_idx > NUM_WEP_KEYS) {
                wpa_printf(MSG_ERROR, "WPS: Invalid WEP Key index %d\n",
                           cred->key_idx);
//          kal_trace(DBG_WPS,INFO_WPS_INVALID_WEP_KEY_INDEX,cred->key_idx);
                return -1;
            }
            if (cred->key_idx) {
                key_idx = cred->key_idx - 1;
            }
            if (cred->key_len == 10 || cred->key_len == 26) {
                if (hexstr2bin((char *) cred->key,
                               ssid->wep_key[key_idx],
                               cred->key_len / 2) < 0) {
                    wpa_printf(TRACE_ERROR, "WPS: Invalid WEP Key "
                               "%d\n", key_idx);
//              kal_trace(DBG_WPS,INFO_WPS_INVALID_WEP_KEY, key_idx);
                    return -1;
                }
                ssid->wep_key_len[key_idx] = cred->key_len / 2;
            } else {
                os_memcpy(ssid->wep_key[key_idx], cred->key,
                          cred->key_len);
                ssid->wep_key_len[key_idx] = cred->key_len;
            }
            ssid->wep_tx_keyidx = key_idx;
            break;
        case WPS_ENCR_TKIP:
            ssid->pairwise_cipher = WPA_CIPHER_TKIP;
            break;
        case WPS_ENCR_AES:
            ssid->pairwise_cipher = WPA_CIPHER_CCMP;
            break;
    }

    switch (auth_type) {
        case WPS_AUTH_OPEN:
            ssid->auth_alg = WPA_AUTH_ALG_OPEN;
            ssid->key_mgmt = WPA_KEY_MGMT_NONE;
            ssid->proto = 0;
            break;
        case WPS_AUTH_SHARED:
            ssid->auth_alg = WPA_AUTH_ALG_SHARED;
            ssid->key_mgmt = WPA_KEY_MGMT_NONE;
            ssid->proto = 0;
            break;
        case WPS_AUTH_WPAPSK:
            ssid->auth_alg = WPA_AUTH_ALG_OPEN;
            ssid->key_mgmt = WPA_KEY_MGMT_PSK;
            ssid->proto = WPA_PROTO_WPA;
            break;
        case WPS_AUTH_WPA:
            ssid->auth_alg = WPA_AUTH_ALG_OPEN;
            ssid->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
            ssid->proto = WPA_PROTO_WPA;
            break;
        case WPS_AUTH_WPA2:
            ssid->auth_alg = WPA_AUTH_ALG_OPEN;
            ssid->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
            ssid->proto = WPA_PROTO_RSN;
            break;
        case WPS_AUTH_WPA2PSK:
            ssid->auth_alg = WPA_AUTH_ALG_OPEN;
            ssid->key_mgmt = WPA_KEY_MGMT_PSK;
            ssid->proto = WPA_PROTO_RSN;
            break;
    }

    if (ssid->key_mgmt == WPA_KEY_MGMT_PSK) {
        if (cred->key_len == 2 * PMK_LEN) {
            if (hexstr2bin((const char *) cred->key, ssid->psk,
                           PMK_LEN)) {
                wpa_printf(TRACE_ERROR, "WPS: Invalid Network "
                           "Key\n");
//              kal_trace(DBG_WPS,INFO_WPS_INVALID_NETWORK_KEY);
                return -1;
            }
            wpa_hexdump_key(MSG_MSGDUMP, "PSK (native)",
                            (char *)ssid->psk, PMK_LEN);
            ssid->psk_set = KAL_TRUE;
        } else if (cred->key_len >= 8 && cred->key_len < 2 * PMK_LEN) {
            os_free(ssid->passphrase);
            ssid->passphrase = os_malloc(cred->key_len + 1);
            if (ssid->passphrase == NULL) {
                return -1;
            }
            os_memcpy(ssid->passphrase, cred->key, cred->key_len);
            ssid->passphrase[cred->key_len] = '\0';

            pbkdf2_sha1(ssid->passphrase,
                        (char *) ssid->ssid, ssid->ssid_len, 4096,
                        ssid->psk, PMK_LEN);
            wpa_hexdump_key(MSG_MSGDUMP, "PSK (from passphrase)",
                            (char *)ssid->psk, PMK_LEN);
            ssid->psk_set = KAL_TRUE;
        } else {
            wpa_printf(TRACE_ERROR, "WPS: Invalid Network Key "
                       "length %lu\n",
                       (unsigned long) cred->key_len);
//          kal_trace(DBG_WPS,INFO_WPS_INVALID_NETWORK_KEY_LEN,(unsigned long) cred->key_len);
            return -1;
        }
    }

    wpas_wps_security_workaround(wpa_s, ssid, cred);
    /*
    #ifndef CONFIG_NO_CONFIG_WRITE
        if (wpa_s->conf->update_config &&
            wpa_config_write(wpa_s->confname, wpa_s->conf)) {
            wpa_printf(MSG_DEBUG, "WPS: Failed to update configuration");
            return -1;
        }
    #endif // CONFIG_NO_CONFIG_WRITE
    */
    /*
        send msg to MMI to update profile's psk
    */
    supc_cb_wps_status(WPS_ADAPTER_STATE_CONFIGURED);
    return 0;
}

static void wpa_supplicant_wps_event_m2d(struct wpa_supplicant *wpa_s,
        struct wps_event_m2d *m2d)
{
    //wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_M2D
    //"dev_password_id=%d config_error=%d",
    //m2d->dev_password_id, m2d->config_error);
//  kal_trace(DBG_WPS,INFO_WPS_M2D_EVENT,m2d->dev_password_id, m2d->config_error);
}


static void wpa_supplicant_wps_event_fail(struct wpa_supplicant *wpa_s,
        struct wps_event_fail *fail)
{
    INNER_TRACE_STEP(1);
    //wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_FAIL "msg=%d", fail->msg);
//  kal_trace(DBG_WPS,INFO_WPS_EVENT_FAIL_MSG, fail->msg);
    wpas_clear_wps(wpa_s);
}


static void wpa_supplicant_wps_event_success(struct wpa_supplicant *wpa_s)
{
    //wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_SUCCESS);
//  kal_trace(DBG_WPS,INFO_SUPC_WPS_EVENT_WPS_SUCCESS);
    wpa_s->wps_success = 1;
}


static void wpa_supplicant_wps_event(void *ctx, enum wps_event event,
                                     union wps_event_data *data) {
    struct wpa_supplicant *wpa_s = ctx;
    switch (event)
    {
        case WPS_EV_M2D:
            wpa_supplicant_wps_event_m2d(wpa_s, &data->m2d);
            break;
        case WPS_EV_FAIL:
            wpa_supplicant_wps_event_fail(wpa_s, &data->fail);
            break;
        case WPS_EV_SUCCESS:
            wpa_supplicant_wps_event_success(wpa_s);
            break;
        case WPS_EV_PWD_AUTH_FAIL:
            break;
        case WPS_EV_PBC_OVERLAP:
            break;
        case WPS_EV_PBC_TIMEOUT:
            break;
    }
}

int eap_is_wps_pbc_enrollee(struct wpa_ssid *ssid)
{
    INNER_TRACE_STEP(1);
    if (ssid->identity_len != WSC_ID_ENROLLEE_LEN ||
            kal_mem_cmp(ssid->identity, WSC_ID_ENROLLEE, WSC_ID_ENROLLEE_LEN)) {
        INNER_TRACE_STEP(2);
        return 0; /* Not a WPS Enrollee */
    }

    if (ssid->phase1 == NULL
            || strstr((char *)ssid->phase1, "pbc=1") == NULL) {
        INNER_TRACE_STEP(3);
        return 0; /* Not using PBC */
    }

    return 1;
}

int eap_is_wps_pin_enrollee(struct wpa_ssid *ssid)
{
    INNER_TRACE_STEP(1);
    if (ssid->identity_len != WSC_ID_ENROLLEE_LEN ||
            kal_mem_cmp(ssid->identity, WSC_ID_ENROLLEE, WSC_ID_ENROLLEE_LEN)) {
        INNER_TRACE_STEP(2);
        //wpa_printf(MSG_DEBUG, "id len=%d", ssid->identity_len);
//      kal_trace(DBG_WPS,INFO_WPS_PIN_ENROLLEE_ID_LEN, ssid->identity_len);
        wpa_hexdump(MSG_DEBUG, "ssid identity", (const char *)ssid->identity, ssid->identity_len);
        return 0; /* Not a WPS Enrollee */
    }

    if (ssid->phase1 == NULL
            || strstr((char *)ssid->phase1, "pin=") == NULL) {
        INNER_TRACE_STEP(3);
        return 0; /* Not using PIN */
    }
    return 1;
}

enum wps_request_type wpas_wps_get_req_type(struct wpa_ssid *ssid) {
    if (eap_is_wps_pbc_enrollee(ssid) ||
            eap_is_wps_pin_enrollee(ssid))
    {
        return WPS_REQ_ENROLLEE;
    } else
    { return WPS_REQ_REGISTRAR; }
}


static void wpas_clear_wps(struct wpa_supplicant *wpa_s)
{
    int id;
    struct wpa_ssid *ssid;

    supc_stop_wps_walk_timer();
    INNER_TRACE_STEP(1);
    /* Remove any existing WPS network from configuration */
    if (NULL == wpa_s->conf) {
        return;
    }
    ssid = wpa_s->conf->ssid;
    while (ssid) {
        if (ssid->key_mgmt & WPA_KEY_MGMT_WPS) {
            if (ssid == wpa_s->current_ssid) {
                wpa_printf(TRACE_GROUP_1, "ssid %x match\n", ssid->ssid);
//              kal_trace(DBG_WPS,INFO_WPS_SSID_MATCH_IN_CLEAR_WPS, ssid->ssid);
                supc_dbg_show_ssid_ex(ssid);
                wpa_s->current_ssid = NULL;
            }
            INNER_TRACE_STEP(2);
            id = ssid->id;
        } else {
            id = -1;
        }
        ssid = ssid->next;
        if (id >= 0) {
            INNER_TRACE_STEP(3);
            wpa_config_remove_network(wpa_s->conf, id);
        }
    }
}


static void wpas_wps_timeout(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_supplicant *wpa_s = eloop_ctx;
    //kal_uint8 wps_type = 0;
    /*send MMI */
    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;

    /*MMI close downcount widget when time expire,
    so supc need not to tell MMI*/
    //wps_type = wpas_wps_in_use(wpa_s->conf, NULL);
    //supc_send_abm_wifi_wps_info_ind(wps_type, 0, 0, KAL_FALSE);

    cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_RESET);
    //kal_mem_set(cnf_local_para_ptr, 0, sizeof(supc_abm_wifi_connect_cnf_struct));

    cnf_local_para_ptr->cause = SUPC_ABM_TIMEOUT;
    cnf_local_para_ptr->result = KAL_FALSE;
    /*clear wps data*/
    cnf_local_para_ptr->passphase_len = 0;
    cnf_local_para_ptr->conn_type = wpa_s->connection_type;
    cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;

    supc_send_abm_wifi_connect_cnf((local_para_struct *)cnf_local_para_ptr);
    supc_cb_wps_status(WPS_ADAPTER_STATE_OFF);
    //wpa_printf(MSG_INFO, WPS_EVENT_TIMEOUT
    //"Requested operation timed out");
//  kal_trace(DBG_WPS,INFO_WPS_REQ_OPERTATION_TIMEOUT);
#ifdef __WPS_SUPPORT__
    wpas_clear_wps(wpa_s);
#endif
}

/**
 * wpa_config_set_network_defaults - Set network default values
 * @ssid: Pointer to network configuration data
 */
void wpa_config_set_network_defaults(struct wpa_ssid *ssid)
{
    ssid->proto = DEFAULT_PROTO;
    ssid->pairwise_cipher = DEFAULT_PAIRWISE;
    ssid->group_cipher = DEFAULT_GROUP;
    ssid->key_mgmt = DEFAULT_KEY_MGMT;
#ifdef IEEE8021X_EAPOL
    ssid->eapol_flags = DEFAULT_EAPOL_FLAGS;
    ssid->eap_workaround = DEFAULT_EAP_WORKAROUND;
#endif /* IEEE8021X_EAPOL */
}

struct wpa_ssid *wpas_wps_add_network(struct wpa_supplicant *wpa_s,
                                      int registrar, const u8 *bssid, u8 *ssid_name, wpa_size_t ssid_len)
{
    struct wpa_ssid *ssid;
    kal_uint8 *eap_wsc = NULL;

    ssid = wpa_config_add_network(wpa_s->conf);

    if (ssid == NULL) {
        return NULL;
    }

    eap_wsc = SUPC_ALLOC_BUFF(2); // ZERO-terminated
    if (NULL == eap_wsc) {
        return NULL;
    }

    wpa_config_set_network_defaults(ssid);
#if 0
    if (wpa_config_set(ssid, "key_mgmt", "WPS", 0) < 0 ||
            wpa_config_set(ssid, "eap", "WSC", 0) < 0 ||
            wpa_config_set(ssid, "identity", registrar ?
                           "\"" WSC_ID_REGISTRAR "\"" :
                           "\"" WSC_ID_ENROLLEE "\"", 0) < 0) {
        wpa_config_remove_network(wpa_s->conf, ssid->id);
        return NULL;
    }
#endif
    ssid->key_mgmt = WPA_KEY_MGMT_WPS;

    eap_wsc[0] = EAP_VENDOR_TYPE_WSC;
    eap_wsc[1] = EAP_TYPE_NONE;
    ssid->eap_methods = eap_wsc;

    ssid->network_type = WNDRV_SUPC_NETWORK_INFRASTRUCTURE;
    ssid->auth_alg = WPA_AUTH_ALG_OPEN;
    ssid->get_all_cert_or_not = KAL_FALSE;

    ssid->identity = (u8 *)SUPC_ALLOC_BUFF(WSC_ID_ENROLLEE_LEN + 1);
    kal_mem_cpy(ssid->identity, WSC_ID_ENROLLEE, WSC_ID_ENROLLEE_LEN + 1);
    ssid->identity_len = WSC_ID_ENROLLEE_LEN;

    if (bssid) {
        size_t i;
        int count = 0;

        kal_mem_cpy(ssid->bssid, bssid, ETH_ALEN);
        ssid->bssid_set = KAL_FALSE;

        /* Try to get SSID from scan results */
        if (wpa_s->num_scan_results == NULL &&
                wpa_supplicant_get_scan_results(wpa_s) < 0) {
            return ssid;    /* Could not find any scan results */
        }

        for (i = 0; i < wpa_s->num_scan_results; i++) {
            //const u8 *ie;
            struct wndrv_supc_bss_info_struct *res;

            res = &wpa_s->scan_results[i];
            if (kal_mem_cmp(bssid, res->bssid, ETH_ALEN) != 0) {
                continue;
            }

            //ie = wpa_scan_get_ie(res, WLAN_EID_SSID);
            //if (ie == NULL)
            //  break;
            if (0 == res->ssid_len) {
                break;
            }
            kal_mem_set(ssid->ssid, 0, MAX_SSID_LEN);
            kal_mem_cpy(ssid->ssid, res->ssid, res->ssid_len);
            ssid->ssid_len = res->ssid_len;
            wpa_hexdump_ascii(MSG_DEBUG, "WPS: Picked SSID from "
                              "scan results",
                              (char *)ssid->ssid, ssid->ssid_len);
            count++;
        }

        if (count > 1) {
            //wpa_printf(MSG_DEBUG, "WPS: More than one SSID found "
            //"for the AP; use wildcard");
//          kal_trace(DBG_WPS,INFO_WPS_MORE_THAN_ONE_SSID_FOUND_USE_WILDCARD);
            kal_mem_set(ssid->ssid, 0, MAX_SSID_LEN);
            ssid->ssid_len = 0;
        }
    } else if (ssid_name && ssid_len > 0) {
        ssid->ssid_len = ssid_len;
        kal_mem_set(ssid->ssid, 0, MAX_SSID_LEN);
        kal_mem_cpy(ssid->ssid, ssid_name, ssid_len);

        //wpa_printf(MSG_DEBUG, "add ssid into config,");
//      kal_trace(DBG_WPS,INFO_WPS_ADD_SSID_INTO_CONFIG);
        wpa_hexdump(MSG_DEBUG, "USER's specified SSID", (const char *)ssid_name, ssid_len);
    }

    return ssid;
}


static void wpas_wps_reassoc(struct wpa_supplicant *wpa_s,
                             struct wpa_ssid *selected)
{
    struct wpa_ssid *ssid;

    /* Mark all other networks disabled and trigger reassociation */
    ssid = wpa_s->conf->ssid;
    while (ssid) {
        ssid->disabled = ssid != selected;
        ssid = ssid->next;
    }
    wpa_s->wps_success = 0;
    wpa_s->blacklist_cleared = 0;
    wpa_supplicant_req_scan(wpa_s, 0, 0);
}


int wpas_wps_start_pbc(struct wpa_supplicant *wpa_s, const u8 *bssid, const u8 *ssid_name, wpa_size_t ssid_len)
{
    struct wpa_ssid *ssid;
    INNER_TRACE_STEP(1);
    wpas_clear_wps(wpa_s);
    ssid = wpas_wps_add_network(wpa_s, 0, bssid, (u8 *)ssid_name, ssid_len);
    if (ssid == NULL) {
        return -1;
    }
    ssid->phase1 = (u32)"\"pbc=1\"";
    wpa_s->wps_walk_timer_id = supc_start_wps_walk_timer(WPS_PBC_WALK_TIME);
    wpas_wps_reassoc(wpa_s, ssid);
    supc_send_abm_wifi_wps_info_ind(WPS_TYPE_PBC, WPS_PBC_WALK_TIME, 0, KAL_TRUE);

    return 0;
}

void supc_wps_walk_timer_expiry_handler(void *para_hf)
{
    INNER_TRACE_STEP(1);
    wpa_s->wps_walk_timer_id = NULL;
    wpas_wps_timeout(wpa_s, NULL);
}


int wpas_wps_start_pin(struct wpa_supplicant *wpa_s, const u8 *bssid,
                       const u8 *ssid_name, wpa_size_t ssid_len, const char *pin)
{
    struct wpa_ssid *ssid;
    static char val[30];
    unsigned int rpin = 0;

    INNER_TRACE_STEP(1);
    wpas_clear_wps(wpa_s);
    ssid = wpas_wps_add_network(wpa_s, 0, bssid, (u8 *)ssid_name, ssid_len);
    if (ssid == NULL) {
        return -1;
    }
    if ((*((unsigned int *)pin)) != 0) {
        rpin = *((unsigned int *)pin);
    } else {
        rpin = wps_generate_pin();
    }

    snprintf(val, sizeof(val), "pin=%08d", rpin);
//    kal_wap_trace(MAP_MOD_SUPC,DBG_WPS, "WPS PINCODE=%s", val);
    wpa_printf(TRACE_GROUP_1, "WPS PINCODE=%s\n", val);
    ssid->phase1 = (u32)val;

    wpa_s->wps_walk_timer_id = supc_start_wps_walk_timer(WPS_PBC_WALK_TIME);
    wpas_wps_reassoc(wpa_s, ssid);

    supc_send_abm_wifi_wps_info_ind(WPS_TYPE_PIN, WPS_PBC_WALK_TIME, rpin, KAL_TRUE);
    return rpin;
}

#if 0
int wpas_wps_start_reg(struct wpa_supplicant *wpa_s, const u8 *bssid,
                       const char *pin)
{
    struct wpa_ssid *ssid;
    char val[30];

    if (!pin) {
        return -1;
    }
    wpas_clear_wps(wpa_s);
    ssid = wpas_wps_add_network(wpa_s, 1, bssid);
    if (ssid == NULL) {
        return -1;
    }
    kal_snprintf(val, sizeof(val), "\"pin=%s\"", pin);
    wpa_config_set(ssid, "phase1", val, 0);
    eloop_register_timeout(WPS_PBC_WALK_TIME, 0, wpas_wps_timeout,
                           wpa_s, NULL);
    wpas_wps_reassoc(wpa_s, ssid);
    return 0;
}
#endif

static int wpas_wps_new_psk_cb(void *ctx, const u8 *mac_addr, const u8 *psk,
                               size_t psk_len)
{
    //wpa_printf(MSG_DEBUG, "WPS: Received new WPA/WPA2-PSK from WPS for "
    //"STA " MACSTR, MAC2STR(mac_addr));
//  kal_trace(DBG_WPS,INFO_WPS_RX_NEWPSK_FOR_STA, MAC2STR(mac_addr));
    wpa_hexdump_key(MSG_DEBUG, "Per-device PSK", (char *)psk, psk_len);

    /* TODO */

    return 0;
}


static void wpas_wps_pin_needed_cb(void *ctx, const u8 *uuid_e,
                                   const struct wps_device_data *dev)
{
    char uuid[40], txt[400];
    int len;
    if (uuid_bin2str(uuid_e, uuid, sizeof(uuid))) {
        return;
    }
//  kal_wap_trace(MAP_MOD_SUPC,DBG_WPS, "WPS: PIN needed for UUID-E %s", uuid);
    len = snprintf(txt, sizeof(txt), "WPS-EVENT-PIN-NEEDED %s " MACSTR
                   " [%s|%s|%s|%s|%s|%d-%08X-%d]",
                   uuid, MAC2STR(dev->mac_addr), dev->device_name,
                   dev->manufacturer, dev->model_name,
                   dev->model_number, dev->serial_number,
                   dev->categ, dev->oui, dev->sub_categ);
    if (len > 0 && len < (int) sizeof(txt))
        ;//     kal_wap_trace(MAP_MOD_SUPC,DBG_WPS, "%s", txt);
}


int wpas_wps_init(struct wpa_supplicant *wpa_s)
{
    struct wps_context *wps;
    struct wps_registrar_config rcfg;

    wps = SUPC_ALLOC_BUFF(sizeof(struct wps_context));
    if (wps == NULL) {
        return -1;
    }
    kal_mem_set(wps, 0, sizeof(struct wps_context));

    wps->cred_cb = wpa_supplicant_wps_cred;
    wps->event_cb = wpa_supplicant_wps_event;
    wps->cb_ctx = wpa_s;

    wps->dev.device_name = wpa_s->conf->device_name;
    wps->dev.manufacturer = wpa_s->conf->manufacturer;
    wps->dev.model_name = wpa_s->conf->model_name;
    wps->dev.model_number = wpa_s->conf->model_number;
    wps->dev.serial_number = wpa_s->conf->serial_number;

    // set fixed primary dev type
    wps->dev.sub_categ = WPS_DEV_PHONE_FEATURE_DUAL;//wndrv_get_dev_sub_cate();
    wps->dev.categ = WPS_DEV_PHONE;//wndrv_get_dev_category();
    wps->dev.oui = WPS_DEV_OUI_MTK;//wndrv_get_cus_oui();
    wps->dev.rf_bands = WPS_RF_24GHZ;// wndrv_get_rf_bands(); /* TODO: config */
    kal_mem_cpy(wps->dev.mac_addr, wpa_s->own_addr, ETH_ALEN);
    if (is_nil_uuid(wpa_s->conf->uuid)) {
        uuid_gen_mac_addr(wpa_s->own_addr, wps->uuid);
        wpa_hexdump(MSG_DEBUG, "WPS: UUID based on MAC address",
                    (char *)wps->uuid, WPS_UUID_LEN);
    } else {
        kal_mem_cpy(wps->uuid, wpa_s->conf->uuid, WPS_UUID_LEN);
    }

    wps->auth_types = WPS_AUTH_WPA2PSK | WPS_AUTH_WPAPSK;
    wps->encr_types = WPS_ENCR_AES | WPS_ENCR_TKIP;

    kal_mem_set(&rcfg, 0, sizeof(rcfg));
    rcfg.new_psk_cb = wpas_wps_new_psk_cb;
    rcfg.pin_needed_cb = wpas_wps_pin_needed_cb;
    rcfg.cb_ctx = wpa_s;

    wps->registrar = wps_registrar_init(wps, &rcfg);
    if (wps->registrar == NULL) {
        //wpa_printf(MSG_DEBUG, "Failed to initialize WPS Registrar");
        SUPC_DEALLOC_BUFF(wps);
        return -1;
    }

    wpa_s->wps = wps;
    //wpa_printf(MSG_DEBUG, "in eap, wpa_s = %x", wpa_s);
//  kal_trace(DBG_WPS,INFO_WPS_WPA_S_CONTEX_ADDR, wpa_s);
    //wpa_printf(MSG_DEBUG, "in eap, create wps = %x", wps);
//  kal_trace(DBG_WPS,INFO_WPS_CREATE_WPS_ADDR, wps);
    //wpa_printf(MSG_DEBUG, "in eap, set wpa_s->wps = %x", wpa_s->wps);
//  kal_trace(DBG_WPS,INFO_WPS_ADDR_IN_WPA_CONTEX, wpa_s->wps);
    //wpa_printf(MSG_DEBUG, "in eap, wps offset in wpa_s= %x", &(wpa_s->wps));
//  kal_trace(DBG_WPS,INFO_WPS_OFFSET_IN_WPA_S, &(wpa_s->wps));
    return 0;
}


void wpas_wps_deinit(struct wpa_supplicant *wpa_s)
{
    INNER_TRACE_STEP(1);
    supc_stop_wps_walk_timer();
    if (wpa_s->wps == NULL) {
        return;
    }

    wps_registrar_deinit(wpa_s->wps->registrar);
    os_free(wpa_s->wps->network_key);
    os_free(wpa_s->wps);
    wpa_s->wps = NULL;
}

int wpas_wps_ssid_bss_match(
    struct wpa_supplicant *wpa_s,
    struct wpa_ssid *ssid,
    struct wndrv_supc_bss_info_struct *bss)
{
    struct wpabuf *wps_ie;

    if (!(ssid->key_mgmt & WPA_KEY_MGMT_WPS)) {
        return -1;
    }

    wps_ie = wpa_scan_get_vendor_ie_multi(bss->wps_ie,
                                          bss->wps_ie_len, WPS_IE_VENDOR_TYPE);

    if (eap_is_wps_pbc_enrollee(ssid)) {
        if (!wps_ie) {
            //wpa_printf(MSG_DEBUG, "   skip - non-WPS IE");
//          kal_trace(DBG_WPS,INFO_WPS_SKIP_NON_WPS_IE);
            return 0;
        }

        if (!wps_is_selected_pbc_registrar(wps_ie)) {
            //wpa_printf(MSG_DEBUG, "   skip - WPS AP "
            //"without active PBC Registrar");
//          kal_trace(DBG_WPS,INFO_WPS_SKIP_AP_WO_ACTIVE_PBC);
            wpabuf_free(wps_ie);
            return 0;
        }

        /* TODO: overlap detection */
        //wpa_printf(MSG_DEBUG, "selected based on WPS IE "
        //"(Active PBC)");
//      kal_trace(DBG_WPS,INFO_WPS_SELECTED_BASED_ON_WPS_IE);
        wpabuf_free(wps_ie);
        return 1;
    }

    if (eap_is_wps_pin_enrollee(ssid)) {
        if (!wps_ie) {
            //wpa_printf(MSG_DEBUG, "   skip - non-WPS AP");
//          kal_trace(DBG_WPS,INFO_WPS_SKIP_NON_WPS_IE);
            return 0;
        }

        /*
         * Start with WPS APs that advertise active PIN Registrar and
         * allow any WPS AP after third scan since some APs do not set
         * Selected Registrar attribute properly when using external
         * Registrar.
         */
        if (!wps_is_selected_pin_registrar(wps_ie)) {
            /*
            if (wpa_s->scan_runs < WPS_PIN_SCAN_IGNORE_SEL_REG) {
                wpa_printf(MSG_DEBUG, "   skip - WPS AP "
                       "without active PIN Registrar");
                wpabuf_free(wps_ie);
                return 0;
            }
            */
            //wpa_printf(MSG_DEBUG, "   selected based on WPS IE");
//          kal_trace(DBG_WPS,INFO_WPS_SELECTED_BASED_WPS_IE);
        } else {
            //wpa_printf(MSG_DEBUG, "   selected based on WPS IE "
            //"(Active PIN)");
//          kal_trace(DBG_WPS,INFO_WPS_SELECTED_BASED_ON_PIN_IE);
        }
        wpabuf_free(wps_ie);
        return 1;
    }

    if (wps_ie) {
        //wpa_printf(MSG_DEBUG, "   selected based on WPS IE");
//      kal_trace(DBG_WPS,INFO_WPS_SELECTED_BASED_WPS_IE);
        wpabuf_free(wps_ie);
        return 1;
    }

    return -1;
}

int wpas_wps_ssid_wildcard_ok(struct wpa_supplicant *wpa_s,
                              struct wpa_ssid *ssid,
                              wndrv_supc_bss_info_struct *bss)
{
    struct wpabuf *wps_ie = NULL;
    int ret = 0;

    if (ssid->identity) {
        wpa_printf(TRACE_GROUP_1, "ssid identity=%s\n", ssid->identity);
//       kal_wap_trace(MAP_MOD_SUPC,DBG_WPS, "ssid identity=%s", ssid->identity);
    }
    if (ssid->phase1) {
        wpa_printf(TRACE_GROUP_1, "ssid  phase1=%s\n", ssid->phase1);
//       kal_wap_trace(MAP_MOD_SUPC,DBG_WPS, "ssid  phase1=%s", ssid->phase1);
    }
    if (eap_is_wps_pbc_enrollee(ssid)) {
        wpa_printf(TRACE_GROUP_1, "config ssid is pbc enrollee\n");
//      kal_trace(DBG_WPS,INFO_WPS_CONFIG_SSID_IS_PBC);
        wps_ie = wpa_scan_get_vendor_ie_multi(bss->wps_ie,
                                              bss->wps_ie_len,
                                              WPS_IE_VENDOR_TYPE);
        if (wps_ie && wps_is_selected_pbc_registrar(wps_ie)) {
            /* allow wildcard SSID for WPS PBC */
            ret = 1;
        }
    } else if (eap_is_wps_pin_enrollee(ssid)) {
        wpa_printf(TRACE_GROUP_1, "config ssid is pin  enrollee\n");
//      kal_trace(DBG_WPS,INFO_WPS_CONFIG_SSID_IS_PIN);
        wps_ie = wpa_scan_get_vendor_ie_multi(bss->wps_ie,
                                              bss->wps_ie_len,
                                              WPS_IE_VENDOR_TYPE);
        if (wps_ie && wps_is_selected_pin_registrar(wps_ie)) {
            /* allow wildcard SSID for WPS PIN */
            ret = 1;
        }
    }

    if (!ret && ssid->bssid_set &&
            kal_mem_cmp(ssid->bssid, bss->bssid, ETH_ALEN) == 0) {
        /* allow wildcard SSID due to hardcoded BSSID match */
        ret = 1;
    }

    wpabuf_free(wps_ie);

    return ret;
}


int wpas_wps_scan_pbc_overlap(struct wpa_supplicant *wpa_s,
                              struct wndrv_supc_bss_info_struct *selected,
                              struct wpa_ssid *ssid)
{
    const u8 *sel_uuid, *uuid;
    kal_uint16 i;
    struct wpabuf *wps_ie;
    int ret = 0;

//    kal_wap_trace(MAP_MOD_SUPC,DBG_WPS, "%s ++", __FUNCTION__);
    if (!eap_is_wps_pbc_enrollee(ssid)) {
        return 0;
    }

    /* Make sure that only one AP is in active PBC mode */
    wps_ie = wpa_scan_get_vendor_ie_multi(selected->wps_ie,
                                          selected->wps_ie_len, WPS_IE_VENDOR_TYPE);
    if (wps_ie) {
        sel_uuid = wps_get_uuid_e(wps_ie);
    } else {
        sel_uuid = NULL;
    }

    for (i = 0; i < wpa_s->num_scan_results; i++) {
        struct wndrv_supc_bss_info_struct *bss = NULL;
        struct wpabuf *ie;

        bss = &wpa_s->scan_results[i];

        if (bss == selected) {
            continue;
        }
        if (bss->has_wps_ie) {
            ie = wpa_scan_get_vendor_ie_multi(bss->wps_ie,
                                              bss->wps_ie_len, WPS_IE_VENDOR_TYPE);
        } else {
            continue;
        }
        if (!ie) {
            continue;
        }
        if (!wps_is_selected_pbc_registrar(ie)) {
            wpabuf_free(ie);
            continue;
        }

        uuid = wps_get_uuid_e(ie);
        if (uuid != NULL && sel_uuid != NULL) {
            wpa_hexdump(MSG_DEBUG, "seld_uuid", (char *)sel_uuid, 16);
            wpa_hexdump(MSG_DEBUG, "other uuid", (char *)uuid, 16);
        }
        if (sel_uuid == NULL || uuid == NULL ||
                kal_mem_cmp(sel_uuid, uuid, 16) != 0) {
            ret = 1; /* PBC overlap */
            wpabuf_free(ie);
            break;
        }

        /* TODO: verify that this is reasonable dual-band situation */

        wpabuf_free(ie);
    }

    wpabuf_free(wps_ie);

    return ret;
}

#if 0
void wpas_wps_notify_scan_results(struct wpa_supplicant *wpa_s)
{
    size_t i;

    if (wpa_s->disconnected || wpa_s->wpa_state >= WPA_ASSOCIATED) {
        return;
    }

    for (i = 0; i < wpa_s->scan_res->num; i++) {
        struct wpa_scan_res *bss = wpa_s->scan_res->res[i];
        struct wpabuf *ie;
        ie = wpa_scan_get_vendor_ie_multi(bss, WPS_IE_VENDOR_TYPE);
        if (!ie) {
            continue;
        }
        if (wps_is_selected_pbc_registrar(ie)) {
            wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_AP_AVAILABLE_PBC);
        } else if (wps_is_selected_pin_registrar(ie)) {
            wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_AP_AVAILABLE_PIN);
        } else {
            wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_AP_AVAILABLE);
        }
        wpabuf_free(ie);
        break;
    }
}
#endif

int wpas_wps_searching(struct wpa_supplicant *wpa_s)
{
    struct wpa_ssid *ssid;

    for (ssid = wpa_s->conf->ssid; ssid; ssid = ssid->next) {
        if ((ssid->key_mgmt & WPA_KEY_MGMT_WPS) && !ssid->disabled) {
            return 1;
        }
    }

    return 0;
}
