/*
 * Wi-Fi Protected Setup - attribute building
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
#include "wpabuf.h"
#include "defs.h"

#include "wpa_supplicant_i.h"
#include "supc_wpa_common.h"
#include "dh_groups.h"
#include "sha256.h"
#include "aes_wrap.h"
#include "wps_i.h"


int wps_build_public_key(struct wps_data *wps, struct wpabuf *msg)
{
    struct wpabuf *pubkey;

    wpa_printf(TRACE_GROUP_1, "WPS:  * Public Key\n");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_WPS_KEY);
    pubkey = dh_init(dh_groups_get(WPS_DH_GROUP), &wps->dh_privkey);
    //pubkey = wpabuf_zeropad(pubkey, 192);
    /*since dh_init doesn't support long prime,
    we choose dh_group_1 to generator public key*/
    pubkey = wpabuf_zeropad(pubkey, GROUP_5_PRIME_LEN);
    if (pubkey == NULL) {
        //wpa_printf(MSG_DEBUG, "WPS: Failed to initialize "
        //"Diffie-Hellman handshake");
//      kal_trace(DBG_WPS,INFO_WPS_INIT_DH_HANDSHAKE_FAIL);
        return -1;
    }
    wpabuf_put_be16(msg, ATTR_PUBLIC_KEY);
    wpabuf_put_be16(msg, wpabuf_len(pubkey));
    wpabuf_put_buf(msg, pubkey);

    if (wps->registrar) {
        wpabuf_free(wps->dh_pubkey_r);
        wps->dh_pubkey_r = pubkey;
    } else {
        wpabuf_free(wps->dh_pubkey_e);
        wps->dh_pubkey_e = pubkey;
    }

    return 0;
}


int wps_build_req_type(struct wpabuf *msg, enum wps_request_type type) {
    //wpa_printf(MSG_DEBUG, "WPS:  * Request Type");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_REQ_TPYE);
    wpabuf_put_be16(msg, ATTR_REQUEST_TYPE);
    wpabuf_put_be16(msg, 1);
    wpabuf_put_u8(msg, type);
    return 0;
}


int wps_build_config_methods(struct wpabuf *msg, u16 methods)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Config Methods (%x)", methods);
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_CONFIG_METHOD, methods);
    wpabuf_put_be16(msg, ATTR_CONFIG_METHODS);
    wpabuf_put_be16(msg, 2);
    wpabuf_put_be16(msg, methods);
    return 0;
}


int wps_build_uuid_e(struct wpabuf *msg, const u8 *uuid)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * UUID-E");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_UUID_E);
    wpabuf_put_be16(msg, ATTR_UUID_E);
    wpabuf_put_be16(msg, WPS_UUID_LEN);
    wpabuf_put_data(msg, uuid, WPS_UUID_LEN);
    return 0;
}


int wps_build_dev_password_id(struct wpabuf *msg, u16 id)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Device Password ID (%d)", id);
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_DEV_PWID, id);
    wpabuf_put_be16(msg, ATTR_DEV_PASSWORD_ID);
    wpabuf_put_be16(msg, 2);
    wpabuf_put_be16(msg, id);
    return 0;
}


int wps_build_config_error(struct wpabuf *msg, u16 err)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Configuration Error (%d)", err);
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_CONFIG_ERROR, err);
    wpabuf_put_be16(msg, ATTR_CONFIG_ERROR);
    wpabuf_put_be16(msg, 2);
    wpabuf_put_be16(msg, err);
    return 0;
}


int wps_build_authenticator(struct wps_data *wps, struct wpabuf *msg)
{
    u8 hash[SHA256_MAC_LEN];
    const u8 *addr[2];
    size_t len[2];

    if (wps->last_msg == NULL) {
        //wpa_printf(MSG_DEBUG, "WPS: Last message not available for "
        //"building authenticator");
//      kal_trace(DBG_WPS,INFO_WPS_NO_LAST_MSG_BUILD_AUTHENTICATOR);
        return -1;
    }

    /* Authenticator = HMAC-SHA256_AuthKey(M_prev || M_curr*)
     * (M_curr* is M_curr without the Authenticator attribute)
     */
    addr[0] = wpabuf_head(wps->last_msg);
    len[0] = wpabuf_len(wps->last_msg);
    addr[1] = wpabuf_head(msg);
    len[1] = wpabuf_len(msg);
    hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 2, addr, len, hash);

    //wpa_printf(MSG_DEBUG, "WPS:  * Authenticator");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_AUTHENTICATOR);
    wpabuf_put_be16(msg, ATTR_AUTHENTICATOR);
    wpabuf_put_be16(msg, WPS_AUTHENTICATOR_LEN);
    wpabuf_put_data(msg, hash, WPS_AUTHENTICATOR_LEN);

    return 0;
}


int wps_build_version(struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Version");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_WPS_VERSION);
    wpabuf_put_be16(msg, ATTR_VERSION);
    wpabuf_put_be16(msg, 1);
    wpabuf_put_u8(msg, WPS_VERSION_V1);
    return 0;
}


int wps_build_msg_type(struct wpabuf *msg, enum wps_msg_type msg_type) {
    //wpa_printf(MSG_DEBUG, "WPS:  * Message Type (%d)", msg_type);
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_MSG_TYPE, msg_type);
    wpabuf_put_be16(msg, ATTR_MSG_TYPE);
    wpabuf_put_be16(msg, 1);
    wpabuf_put_u8(msg, msg_type);
    return 0;
}


int wps_build_enrollee_nonce(struct wps_data *wps, struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Enrollee Nonce");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_ENROLLEE_NONCE);
    wpabuf_put_be16(msg, ATTR_ENROLLEE_NONCE);
    wpabuf_put_be16(msg, WPS_NONCE_LEN);
    wpabuf_put_data(msg, wps->nonce_e, WPS_NONCE_LEN);
    return 0;
}


int wps_build_registrar_nonce(struct wps_data *wps, struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Registrar Nonce");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_REGISTRAR_NONCE);
    wpabuf_put_be16(msg, ATTR_REGISTRAR_NONCE);
    wpabuf_put_be16(msg, WPS_NONCE_LEN);
    wpabuf_put_data(msg, wps->nonce_r, WPS_NONCE_LEN);
    return 0;
}


int wps_build_auth_type_flags(struct wps_data *wps, struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Authentication Type Flags");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_AUTH_TYPE_FLAGS);
    wpabuf_put_be16(msg, ATTR_AUTH_TYPE_FLAGS);
    wpabuf_put_be16(msg, 2);
    wpabuf_put_be16(msg, WPS_AUTH_TYPES);
    return 0;
}


int wps_build_encr_type_flags(struct wps_data *wps, struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Encryption Type Flags");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_ENCRYPTION_TYPE_FLAGS);
    wpabuf_put_be16(msg, ATTR_ENCR_TYPE_FLAGS);
    wpabuf_put_be16(msg, 2);
    wpabuf_put_be16(msg, WPS_ENCR_TYPES);
    return 0;
}


int wps_build_conn_type_flags(struct wps_data *wps, struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Connection Type Flags");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_CONNECTION_TYPE_FLAGS);
    wpabuf_put_be16(msg, ATTR_CONN_TYPE_FLAGS);
    wpabuf_put_be16(msg, 1);
    wpabuf_put_u8(msg, WPS_CONN_ESS);
    return 0;
}


int wps_build_assoc_state(struct wps_data *wps, struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "WPS:  * Association State");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_ASSOCIATION_STATE);
    wpabuf_put_be16(msg, ATTR_ASSOC_STATE);
    wpabuf_put_be16(msg, 2);
    wpabuf_put_be16(msg, WPS_ASSOC_NOT_ASSOC);
    return 0;
}


int wps_build_key_wrap_auth(struct wps_data *wps, struct wpabuf *msg)
{
    u8 hash[SHA256_MAC_LEN];

    //wpa_printf(MSG_DEBUG, "WPS:  * Key Wrap Authenticator");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_KEY_WRAP_AHTHENTICATOR);
    wps_hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN, wpabuf_head(msg),
                    wpabuf_len(msg), hash);

    wpabuf_put_be16(msg, ATTR_KEY_WRAP_AUTH);
    wpabuf_put_be16(msg, WPS_KWA_LEN);
    wpabuf_put_data(msg, hash, WPS_KWA_LEN);
    return 0;
}


int wps_build_encr_settings(struct wps_data *wps, struct wpabuf *msg,
                            struct wpabuf *plain)
{
    size_t pad_len;
    const size_t block_size = 16;
    u8 *iv;
    kal_uint32 offset = 0;
    kal_uint8 *temp = NULL;
    kal_uint32 temp_len = 0;

    //wpa_printf(MSG_DEBUG, "WPS:  * Encrypted Settings");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_ENCR_SETTING);

    /* PKCS#5 v2.0 pad */
    pad_len = block_size - wpabuf_len(plain) % block_size;
    os_memset(wpabuf_put(plain, pad_len), pad_len, pad_len);
    wpa_hexdump(MSG_DEBUG, "plain test", (char *)(wpabuf_head_u8(plain)), wpabuf_len(plain));

    wpabuf_put_be16(msg, ATTR_ENCR_SETTINGS);
    wpabuf_put_be16(msg, block_size + wpabuf_len(plain));

    iv = wpabuf_put(msg, block_size);
    if (hostapd_get_rand(iv, block_size) < 0) {
        return -1;
    }
#if 0
    data = wpabuf_put(msg, 0);
    wpabuf_put_buf(msg, plain);
    if (aes_128_cbc_encrypt(wps->keywrapkey, iv, data, wpabuf_len(plain))) {
        return -1;
    }
#endif
    /*che require input and output buffer must 4-byte alignment/
    and  encrypt will pad input buffer*/
    temp_len = wpabuf_len(plain) + 32;
    temp = SUPC_ALLOC_BUFF(temp_len);
    kal_mem_set(temp, 0xEC, temp_len);
    offset = 0;
    if (NULL == temp) {
        return -1;
    }
    if (((kal_uint32)temp) & 0x3) {
        offset = 0x4 - (((kal_uint32)temp) & 0x3);
        //wpa_printf(MSG_DEBUG, "change address for AES handler");
//      kal_trace(DBG_WPS,INFO_WPS_CHAGE_ADDR_FOR_AES);
    }
    temp += offset;
    /*copy from plain text*/
    kal_mem_cpy(temp, wpabuf_head_u8(plain), wpabuf_len(plain));
    wpa_hexdump(MSG_DEBUG, "before aes_128_cbc_encrypt temp test", (char *)temp, temp_len);

    if (aes_128_cbc_encrypt(wps->keywrapkey, iv, temp, wpabuf_len(plain))) {
        SUPC_DEALLOC_BUFF((temp - offset));
        return -1;
    }
    wpa_hexdump(MSG_DEBUG, "after aes_cbc_128_encrypt temp test", (char *)temp, temp_len);
    /*copy encrypted data into msg*/
    wpabuf_put_data(msg, temp, wpabuf_len(plain));
    SUPC_DEALLOC_BUFF(temp - offset);
    return 0;
}

int wps_build_wfa_sub_ver(struct wpabuf *msg)
{
    //wpa_printf(MSG_DEBUG, "build WFA EXT attr");
//  kal_trace(DBG_WPS,INFO_WPS_BUILD_WFA_EXT_ATTR);
    /*make vendor attr first
    fix head length: 2+2+3
    sub ver length: 2 + 1 */
    wpabuf_put_be16(msg, ATTR_VENDOR_EXT);
    wpabuf_put_be16(msg, 6);
    wpabuf_put_be24(msg, 0x00372A);
    /*make ver*/
    wpabuf_put_u8(msg, 0);
    wpabuf_put_u8(msg, 1);
    wpabuf_put_u8(msg, WPS_VERSION_V2);
    return 0;
}
