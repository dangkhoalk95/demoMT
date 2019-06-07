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

#ifndef CONFIG_SSID_H
#define CONFIG_SSID_H

#define WPA_CIPHER_NONE BIT(0)
#define WPA_CIPHER_WEP40 BIT(1)
#define WPA_CIPHER_WEP104 BIT(2)
#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CIPHER_CCMP BIT(4)
#if 1 //#ifdef __WAPI_SUPPORT__
#define WAPI_CIPHER_SMS4    BIT(5)
#endif

#define WPA_KEY_MGMT_IEEE8021X BIT(0)
#define WPA_KEY_MGMT_PSK BIT(1)
#define WPA_KEY_MGMT_NONE BIT(2)
#define WPA_KEY_MGMT_IEEE8021X_NO_WPA BIT(3)
#define WPA_KEY_MGMT_WPA_NONE BIT(4)
#if 1 //#ifdef __WAPI_SUPPORT__
#define WAPI_KEY_MGMT_PSK BIT(5)
#define WAPI_KEY_MGMT_WAI BIT(6)
#endif

#define WPA_PROTO_WPA BIT(0)
#define WPA_PROTO_RSN BIT(1)
#if 1  //#ifdef __WAPI_SUPPORT__
#define WPA_PROTO_WAPI BIT(2)
#endif
#if 1 //#ifdef __WPS_SUPPORT__
#define WPA_PROTO_WPS_OPEN  BIT(3)
#define WPA_PROTO_WPS_WPA   BIT(4)
#define WPA_PROTO_WPS_WPA2  BIT(5)
#endif

#define WPA_AUTH_ALG_OPEN BIT(0)
#define WPA_AUTH_ALG_SHARED BIT(1)
#define WPA_AUTH_ALG_LEAP BIT(2)

#define MAX_SSID_LEN 32
#define PMK_LEN 32
#define EAP_PSK_LEN 16

/* Joseph */
//Saker : change for WiFi UE enhancement 11->31
#define SUPC_MAX_SSID_CONFIG_PRIORITY 31

/*jim */
#define DEFAULT_EAP_WORKAROUND ((unsigned int) -1)
#define DEFAULT_PROTO (WPA_PROTO_WPA | WPA_PROTO_RSN)
#define DEFAULT_KEY_MGMT (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_IEEE8021X)
#define DEFAULT_PAIRWISE (WPA_CIPHER_CCMP | WPA_CIPHER_TKIP)
#define DEFAULT_GROUP (WPA_CIPHER_CCMP | WPA_CIPHER_TKIP | \
               WPA_CIPHER_WEP104 | WPA_CIPHER_WEP40)


typedef struct wpa_ssid {
    struct wpa_ssid *next; /* next network in global list */
    struct wpa_ssid *pnext; /* next network in per-priority list */
    int id; /* unique id for ctrl_iface */
    kal_uint8 priority;
    kal_uint32 profile_id;
    u8 ssid[MAX_SSID_LEN];
    wpa_size_t ssid_len;
    u8 bssid[ETH_ALEN];
    int bssid_set;
    kal_uint32 channel_number;
    u8 psk[PMK_LEN];
    int psk_set;
#if 1 //#ifdef __WAPI_SUPPORT__
    u8 wapi_psk[PMK_LEN];
    u8 wapi_psk_type;/* 0 for HEX 1 for ASCII */
#endif
    char *passphrase;
    /* Bitfields of allowed Pairwise/Group Ciphers, WPA_CIPHER_* */
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
    int proto; /* Bitfield of allowed protocols (WPA_PROTO_*) */
    int auth_alg; /* Bitfield of allow authentication algorithms
               * (WPA_AUTH_ALG_*) */
    int scan_ssid; /* scan this SSID with Probe Requests */
    u8 *identity; /* EAP Identity */
    wpa_size_t identity_len;
    u8 *anonymous_identity; /* Anonymous EAP Identity (for unencrypted use
                 * with EAP types that support different
                 * tunnelled identity, e.g., EAP-TTLS) */
    wpa_size_t anonymous_identity_len;
    u8 *eappsk;
    wpa_size_t eappsk_len;
    u8 *nai;
    wpa_size_t nai_len;
    u8 *server_nai;
    wpa_size_t server_nai_len;
    u8 *password;
    wpa_size_t password_len;
    void *ca_cert;
    void *client_cert;
    u8 *private_key;
    u16 private_key_passwd_len;
    u8 *private_key_passwd;
#if 1//#ifdef __CERTMAN_SUPPORT__
    kal_uint32 ca_cert_id; // Kinki add for CertMan *start*
    kal_uint32 client_cert_id;
    u16 privkey_random_passwd_len;
    u8 *privkey_random_passwd; // Kinki add *end*
#endif
    u8 *dh_file;
    u8 *subject_match;
    void *ca_cert2;
    void *client_cert2;
    u8 *private_key2;
    u16 private_key2_passwd_len;
    u8 *private_key2_passwd;
#if 1//#ifdef __CERTMAN_SUPPORT__
    kal_uint32 ca_cert2_id;  // Kinki add for CertMan *start*
    kal_uint32 client_cert2_id;
    u16 privkey2_random_passwd_len;
    u8 *privkey2_random_passwd;
    kal_bool decrypted_or_not;
    kal_bool get_all_cert_or_not;
#endif
    u8 *dh_file2;
    u8 *subject_match2;
    u8 *eap_methods; /* zero (EAP_TYPE_NONE) terminated list of allowed
              * EAP methods or NULL = any */
    u32  phase1;
    u32  phase2;
    char *pcsc;
    char *pin;

#define EAPOL_FLAG_REQUIRE_KEY_UNICAST BIT(0)
#define EAPOL_FLAG_REQUIRE_KEY_BROADCAST BIT(1)
#define DEFAULT_EAPOL_FLAGS (EAPOL_FLAG_REQUIRE_KEY_UNICAST | \
                     EAPOL_FLAG_REQUIRE_KEY_BROADCAST)

    int eapol_flags; /* bit field of IEEE 802.1X/EAPOL options */

#define NUM_WEP_KEYS 4
#define MAX_WEP_KEY_LEN 16
    u8 wep_key[NUM_WEP_KEYS][MAX_WEP_KEY_LEN];
    wpa_size_t wep_key_len[NUM_WEP_KEYS];
    int wep_tx_keyidx;

    /* Per SSID variables that are not read from the configuration file */
    u8 *otp;
    wpa_size_t otp_len;
    int pending_req_identity, pending_req_password;
    char *pending_req_otp;
    wpa_size_t pending_req_otp_len;
    int leap, non_leap;

    unsigned int eap_workaround;

    char *pac_file;

    int network_type;   // Karen
    int mode;
    int disabled;
} wpa_ssid;

int wpa_config_allowed_eap_method(struct wpa_ssid *ssid, int method);
char *wpa_cipher_txt(int cipher);
const char *wpa_key_mgmt_txt(int key_mgmt, int proto);

#endif /* CONFIG_SSID_H */
