/*
 * Wi-Fi Protected Setup - attribute processing
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

#include "supc_common.h"
#include "wndrv_supc_types.h"

#include "wpa.h"
#include "wpabuf.h"
#include "wpa_supplicant_i.h"
#include "defs.h"

#include "supc_wpa_common.h"
#include "sha256.h"
#include "wps_i.h"


int wps_process_authenticator(struct wps_data *wps, const u8 *authenticator,
                              const struct wpabuf *msg)
{
    u8 hash[SHA256_MAC_LEN];
    const u8 *addr[2];
    size_t len[2];

    if (authenticator == NULL) {
        //wpa_printf(MSG_DEBUG, "WPS: No Authenticator attribute "
        //"included");
//      kal_trace(DBG_WPS,INFO_WPS_NO_AUTHENTICATOR_ATTR_INCLUDED);
        return -1;
    }

    if (wps->last_msg == NULL) {
        //wpa_printf(MSG_DEBUG, "WPS: Last message not available for "
        //"validating authenticator");
//      kal_trace(DBG_WPS,INFO_WPS_NO_LAST_MSG_TO_VALIDATE_AC);
        return -1;
    }

    /* Authenticator = HMAC-SHA256_AuthKey(M_prev || M_curr*)
     * (M_curr* is M_curr without the Authenticator attribute)
     */
    addr[0] = wpabuf_head(wps->last_msg);
    len[0] = wpabuf_len(wps->last_msg);
    addr[1] = wpabuf_head(msg);
    len[1] = wpabuf_len(msg) - 4 - WPS_AUTHENTICATOR_LEN;
    hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 2, addr, len, hash);

    if (os_memcmp(hash, authenticator, WPS_AUTHENTICATOR_LEN) != 0) {
        //wpa_printf(MSG_DEBUG, "WPS: Incorrect Authenticator");
//      kal_trace(DBG_WPS,INFO_WPS_INCORRECT_AC);
        return -1;
    }

    return 0;
}


int wps_process_key_wrap_auth(struct wps_data *wps, struct wpabuf *msg,
                              const u8 *key_wrap_auth)
{
    u8 hash[SHA256_MAC_LEN];
    const u8 *head;
    size_t len;

    if (key_wrap_auth == NULL) {
        //wpa_printf(MSG_DEBUG, "WPS: No KWA in decrypted attribute");
//      kal_trace(DBG_WPS,INFO_WPS_NO_KWA_IN_DECRYPTED_ATTR);
        return -1;
    }

    head = wpabuf_head(msg);
    len = wpabuf_len(msg) - 4 - WPS_KWA_LEN;
    if (head + len != key_wrap_auth - 4) {
        //wpa_printf(MSG_DEBUG, "WPS: KWA not in the end of the "
        //"decrypted attribute");
//      kal_trace(DBG_WPS,INFO_WPS_KWA_NOT_THE_END_OF_DECRYPTED_ATTR);
        return -1;
    }

    wps_hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN, head, len, hash);
    if (os_memcmp(hash, key_wrap_auth, WPS_KWA_LEN) != 0) {
        //wpa_printf(MSG_DEBUG, "WPS: Invalid KWA");
//      kal_trace(DBG_WPS,INFO_WPS_INVALID_WKA);
        return -1;
    }

    return 0;
}


static int wps_process_cred_network_idx(struct wps_credential *cred,
                                        const u8 *idx)
{
    if (idx == NULL) {
        wpa_printf(TRACE_GROUP_1, "WPS: Credential did not include "
                   "Network Index\n");
//      kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NOT_INCLUDE_NETWORK_INDEX);
        return -1;
    }

    wpa_printf(TRACE_GROUP_1, "WPS: Network Index: %d\n", *idx);
//  kal_trace(DBG_WPS,INFO_WPS_NETWORK_INDEX, *idx);

    return 0;
}


static int wps_process_cred_ssid(struct wps_credential *cred, const u8 *ssid,
                                 size_t ssid_len)
{
    if (ssid == NULL) {
        wpa_printf(TRACE_GROUP_1, "WPS: Credential did not include SSID\n");
//      kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NOT_INCLUDE_SSID);
        return -1;
    }

    /* Remove zero-padding since some Registrar implementations seem to use
     * hardcoded 32-octet length for this attribute */
    while (ssid_len > 0 && ssid[ssid_len - 1] == 0) {
        ssid_len--;
    }

    wpa_hexdump_ascii(MSG_DEBUG, "WPS: SSID", (char *)ssid, ssid_len);
    if (ssid_len <= sizeof(cred->ssid)) {
        os_memcpy(cred->ssid, ssid, ssid_len);
        cred->ssid_len = ssid_len;
    }

    return 0;
}


static int wps_process_cred_auth_type(struct wps_credential *cred,
                                      const u8 *auth_type)
{
    if (auth_type == NULL) {
        wpa_printf(TRACE_GROUP_1, "WPS: Credential did not include "
                   "Authentication Type\n");
//      kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NOT_INCLUDE_AUTH_TYPE);
        return -1;
    }

    cred->auth_type = WPA_GET_BE16(auth_type);
    wpa_printf(TRACE_GROUP_1, "WPS: Authentication Type: 0x%x\n",
               cred->auth_type);
//  kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_AUTH_TYPE,cred->auth_type);

    return 0;
}


static int wps_process_cred_encr_type(struct wps_credential *cred,
                                      const u8 *encr_type)
{
    if (encr_type == NULL) {
        wpa_printf(TRACE_GROUP_1, "WPS: Credential did not include "
                   "Encryption Type\n");
//kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NOT_INCLUDE_ENCR_TYPE);
        return -1;
    }

    cred->encr_type = WPA_GET_BE16(encr_type);
    wpa_printf(TRACE_GROUP_1, "WPS: Encryption Type: 0x%x\n",
               cred->encr_type);
//  kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_ENCR_TYPE,cred->encr_type);

    return 0;
}


static int wps_process_cred_network_key_idx(struct wps_credential *cred,
        const u8 *key_idx)
{
    if (key_idx == NULL) {
        return 0;    /* optional attribute */
    }

    wpa_printf(TRACE_GROUP_1, "WPS: Network Key Index: %d\n", *key_idx);
//  kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NETWORK_KEY_INDEX, *key_idx);
    cred->key_idx = *key_idx;

    return 0;
}


static int wps_process_cred_network_key(struct wps_credential *cred,
                                        const u8 *key, size_t key_len)
{
    if (key == NULL) {
        wpa_printf(MSG_DEBUG, "WPS: Credential did not include "
                   "Network Key\n");
//      kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NOT_INCLUDE_NETWORK_KEY);
        return -1;
    }

    wpa_hexdump_key(MSG_DEBUG, "WPS: Network Key\n", (char *)key, key_len);
    if (key_len <= sizeof(cred->key)) {
        os_memcpy(cred->key, key, key_len);
        cred->key_len = key_len;
    }

    return 0;
}


static int wps_process_cred_mac_addr(struct wps_credential *cred,
                                     const u8 *mac_addr)
{
    if (mac_addr == NULL) {
        wpa_printf(TRACE_GROUP_1, "WPS: Credential did not include "
                   "MAC Address\n");
//      kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_NOT_INLCUDE_MAC_ADDR);
        return -1;
    }

    wpa_printf(TRACE_GROUP_1, "WPS: MAC Address " MACSTR "\n", MAC2STR(mac_addr));
//  kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_MAC_ADDR, MAC2STR(mac_addr));
    os_memcpy(cred->mac_addr, mac_addr, ETH_ALEN);

    return 0;
}


static int wps_process_cred_eap_type(struct wps_credential *cred,
                                     const u8 *eap_type, size_t eap_type_len)
{
    if (eap_type == NULL) {
        return 0;    /* optional attribute */
    }

    wpa_hexdump(TRACE_GROUP_1, "WPS: EAP Type", (char *)eap_type, eap_type_len);

    return 0;
}


static int wps_process_cred_eap_identity(struct wps_credential *cred,
        const u8 *identity,
        size_t identity_len)
{
    if (identity == NULL) {
        return 0;    /* optional attribute */
    }

    wpa_hexdump_ascii(TRACE_GROUP_1, "WPS: EAP Identity",
                      (char *)identity, identity_len);

    return 0;
}


static int wps_process_cred_key_prov_auto(struct wps_credential *cred,
        const u8 *key_prov_auto)
{
    if (key_prov_auto == NULL) {
        return 0;    /* optional attribute */
    }

    wpa_printf(TRACE_GROUP_1, "WPS: Key Provided Automatically: %d\n",
               *key_prov_auto);
//  kal_trace(DBG_WPS,INFO_WPS_KEY_PROVIDED_AUTO,*key_prov_auto);

    return 0;
}


static int wps_process_cred_802_1x_enabled(struct wps_credential *cred,
        const u8 *dot1x_enabled)
{
    if (dot1x_enabled == NULL) {
        return 0;    /* optional attribute */
    }

    wpa_printf(TRACE_GROUP_1, "WPS: 802.1X Enabled: %d\n", *dot1x_enabled);
//  kal_trace(DBG_WPS,INFO_WPS_CREDENTIAL_8021XENABLE, *dot1x_enabled);

    return 0;
}


static void wps_workaround_cred_key(struct wps_credential *cred)
{
    if (cred->auth_type & (WPS_AUTH_WPAPSK | WPS_AUTH_WPA2PSK) &&
            cred->key_len > 8 && cred->key_len < 64 &&
            cred->key[cred->key_len - 1] == 0) {
        /*
         * A deployed external registrar is known to encode ASCII
         * passphrases incorrectly. Remove the extra NULL termination
         * to fix the encoding.
         */
        wpa_printf(TRACE_GROUP_1, "WPS: Workaround - remove NULL "
                   "termination from ASCII passphrase\n");
//      kal_trace(DBG_WPS,INFO_WPS_CRED_WORKAROUND_NULL_TERMINATION);
        cred->key_len--;
    }
}


int wps_process_cred(struct wps_parse_attr *attr,
                     struct wps_credential *cred)
{
    wpa_printf(TRACE_GROUP_1, "WPS: Process Credential\n");
//  kal_trace(DBG_WPS,INFO_WPS_PROCESS_CREDENTIAL);

    /* TODO: support multiple Network Keys */
    if (wps_process_cred_network_idx(cred, attr->network_idx) ||
            wps_process_cred_ssid(cred, attr->ssid, attr->ssid_len) ||
            wps_process_cred_auth_type(cred, attr->auth_type) ||
            wps_process_cred_encr_type(cred, attr->encr_type) ||
            wps_process_cred_network_key_idx(cred, attr->network_key_idx) ||
            wps_process_cred_network_key(cred, attr->network_key,
                                         attr->network_key_len) ||
            wps_process_cred_mac_addr(cred, attr->mac_addr) ||
            wps_process_cred_eap_type(cred, attr->eap_type,
                                      attr->eap_type_len) ||
            wps_process_cred_eap_identity(cred, attr->eap_identity,
                                          attr->eap_identity_len) ||
            wps_process_cred_key_prov_auto(cred, attr->key_prov_auto) ||
            wps_process_cred_802_1x_enabled(cred, attr->dot1x_enabled)) {
        return -1;
    }

    wps_workaround_cred_key(cred);

    return 0;
}


int wps_process_ap_settings(struct wps_parse_attr *attr,
                            struct wps_credential *cred)
{
    //wpa_printf(MSG_DEBUG, "WPS: Processing AP Settings");
//  kal_trace(DBG_WPS,INFO_WPS_PROCESS_AP_SETTING);
    os_memset(cred, 0, sizeof(*cred));
    /* TODO: optional attributes New Password and Device Password ID */
    if (wps_process_cred_ssid(cred, attr->ssid, attr->ssid_len) ||
            wps_process_cred_auth_type(cred, attr->auth_type) ||
            wps_process_cred_encr_type(cred, attr->encr_type) ||
            wps_process_cred_network_key_idx(cred, attr->network_key_idx) ||
            wps_process_cred_network_key(cred, attr->network_key,
                                         attr->network_key_len) ||
            wps_process_cred_mac_addr(cred, attr->mac_addr)) {
        return -1;
    }

    wps_workaround_cred_key(cred);

    return 0;
}
