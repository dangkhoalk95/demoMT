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

#ifndef WPA_H
#define WPA_H

#define BIT(n) (0x0001 << (n))

typedef struct ieee802_1x_hdr {
    u8 version;
    u8 type;
    u16 length;
    /* followed by length octets of data */
} ieee802_1x_hdr;

#define EAPOL_VERSION 2

enum { IEEE802_1X_TYPE_EAP_PACKET = 0,
       IEEE802_1X_TYPE_EAPOL_START = 1,
       IEEE802_1X_TYPE_EAPOL_LOGOFF = 2,
       IEEE802_1X_TYPE_EAPOL_KEY = 3,
       IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT = 4
     };

enum { EAPOL_KEY_TYPE_RC4 = 1, EAPOL_KEY_TYPE_RSN = 2,
       EAPOL_KEY_TYPE_WPA = 254
     };


#define IEEE8021X_REPLAY_COUNTER_LEN 8
#define IEEE8021X_KEY_SIGN_LEN 16
#define IEEE8021X_KEY_IV_LEN 16
#define IEEE8021X_HEADER_LEN sizeof(struct ieee802_1x_hdr)
#define IEEE8021X_EAPOL_KEY_LEN 44

#define IEEE8021X_KEY_INDEX_FLAG 0x80
#define IEEE8021X_KEY_INDEX_MASK 0x03

typedef struct ieee802_1x_eapol_key {
    u8 type;
    u16 key_length;
    /* does not repeat within the life of the keying material used to
     * encrypt the Key field; 64-bit NTP timestamp MAY be used here */
    u8 replay_counter[IEEE8021X_REPLAY_COUNTER_LEN];
    u8 key_iv[IEEE8021X_KEY_IV_LEN]; /* cryptographically random number */
    u8 key_index; /* key flag in the most significant bit:
               * 0 = broadcast (default key),
               * 1 = unicast (key mapping key); key index is in the
               * 7 least significant bits */
    /* HMAC-MD5 message integrity check computed with MS-MPPE-Send-Key as
     * the key */
    u8 key_signature[IEEE8021X_KEY_SIGN_LEN];

    /* followed by key: if packet body length = 44 + key length, then the
     * key field (of key_length bytes) contains the key in encrypted form;
     * if packet body length = 44, key field is absent and key_length
     * represents the number of least significant octets from
     * MS-MPPE-Send-Key attribute to be used as the keying material;
     * RC4 key used in encryption = Key-IV + MS-MPPE-Recv-Key */
} ieee802_1x_eapol_key;


#define WPA_NONCE_LEN 32
#define WPA_REPLAY_COUNTER_LEN 8
#define WPA_EAPOL_KEY_SIZE 95

typedef struct wpa_eapol_key {
    u8 type;
    u16 key_info;
    u16 key_length;
    u8 replay_counter[WPA_REPLAY_COUNTER_LEN];
    u8 key_nonce[WPA_NONCE_LEN];
    u8 key_iv[16];
    u8 key_rsc[8];
    u8 key_id[8]; /* Reserved in IEEE 802.11i/RSN */
    u8 key_mic[16];
    u16 key_data_length;
    /* followed by key_data_length bytes of key_data */
} wpa_eapol_key;

#define WPA_KEY_INFO_TYPE_MASK (BIT(0) | BIT(1) | BIT(2))
#define WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 BIT(0)
#define WPA_KEY_INFO_TYPE_HMAC_SHA1_AES BIT(1)
#define WPA_KEY_INFO_KEY_TYPE BIT(3) /* 1 = Pairwise, 0 = Group key */
/* bit4..5 is used in WPA, but is reserved in IEEE 802.11i/RSN */
#define WPA_KEY_INFO_KEY_INDEX_MASK (BIT(4) | BIT(5))
#define WPA_KEY_INFO_KEY_INDEX_SHIFT 4
#define WPA_KEY_INFO_INSTALL BIT(6) /* pairwise */
#define WPA_KEY_INFO_TXRX BIT(6) /* group */
#define WPA_KEY_INFO_ACK BIT(7)
#define WPA_KEY_INFO_MIC BIT(8)
#define WPA_KEY_INFO_SECURE BIT(9)
#define WPA_KEY_INFO_ERROR BIT(10)
#define WPA_KEY_INFO_REQUEST BIT(11)
#define WPA_KEY_INFO_ENCR_KEY_DATA BIT(12) /* IEEE 802.11i/RSN only */

#define WPA_CAPABILITY_PREAUTH BIT(0)

#define GENERIC_INFO_ELEM 0xdd
#define RSN_INFO_ELEM 0x30
#if 1 //#ifdef __WAPI_SUPPORT__
#define WAPI_INFO_ELEM (68)
#endif

enum {
    REASON_UNSPECIFIED = 1,
    REASON_DEAUTH_LEAVING = 3,
    REASON_INVALID_IE = 13,
    REASON_MICHAEL_MIC_FAILURE = 14,
    REASON_4WAY_HANDSHAKE_TIMEOUT = 15,
    REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
    REASON_IE_IN_4WAY_DIFFERS = 17,
    REASON_GROUP_CIPHER_NOT_VALID = 18,
    REASON_PAIRWISE_CIPHER_NOT_VALID = 19,
    REASON_AKMP_NOT_VALID = 20,
    REASON_UNSUPPORTED_RSN_IE_VERSION = 21,
    REASON_INVALID_RSN_IE_CAPAB = 22,
    REASON_IEEE_802_1X_AUTH_FAILED = 23,
    REASON_CIPHER_SUITE_REJECTED = 24
#if 1 //#ifdef __WAPI_SUPPORT__
    ,
    REASON_UNSUPPORT_WAPI_VERSION = 30,
    REASON_UNSUPPORT_WAPI_CAP = 31,
    REASON_WAPI_CERT_AUTH_FAILED = 32
#endif
};

#endif /* WPA_H */
