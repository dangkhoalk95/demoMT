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


#ifndef DEFS_H
#define DEFS_H
#include "kal_general_types.h"

#ifdef CONFIG_NATIVE_WINDOWS
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
#endif /* CONFIG_NATIVE_WINDOWS */
//typedef enum { FALSE = 0, TRUE = 1 } Boolean;
typedef unsigned char Boolean;

#define WPA_CIPHER_NONE BIT(0)
#define WPA_CIPHER_WEP40 BIT(1)
#define WPA_CIPHER_WEP104 BIT(2)
#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CIPHER_CCMP BIT(4)

#define WPA_KEY_MGMT_IEEE8021X BIT(0)
#define WPA_KEY_MGMT_PSK BIT(1)
#define WPA_KEY_MGMT_NONE BIT(2)
#define WPA_KEY_MGMT_IEEE8021X_NO_WPA BIT(3)
#define WPA_KEY_MGMT_WPA_NONE BIT(4)
#define WPA_KEY_MGMT_FT_IEEE8021X BIT(5)
#define WPA_KEY_MGMT_FT_PSK BIT(6)
#define WPA_KEY_MGMT_IEEE8021X_SHA256 BIT(7)
#define WPA_KEY_MGMT_PSK_SHA256 BIT(8)
#define WPA_KEY_MGMT_WPS BIT(9)

__KAL_INLINE__ int wpa_key_mgmt_wpa_ieee8021x(int akm)
{
    return akm == WPA_KEY_MGMT_IEEE8021X ||
           akm == WPA_KEY_MGMT_FT_IEEE8021X ||
           akm == WPA_KEY_MGMT_IEEE8021X_SHA256;
}

__KAL_INLINE__ int wpa_key_mgmt_wpa_psk(int akm)
{
    return akm == WPA_KEY_MGMT_PSK ||
           akm == WPA_KEY_MGMT_FT_PSK ||
           akm == WPA_KEY_MGMT_PSK_SHA256;
}

__KAL_INLINE__ int wpa_key_mgmt_ft(int akm)
{
    return akm == WPA_KEY_MGMT_FT_PSK ||
           akm == WPA_KEY_MGMT_FT_IEEE8021X;
}

__KAL_INLINE__ int wpa_key_mgmt_sha256(int akm)
{
    return akm == WPA_KEY_MGMT_PSK_SHA256 ||
           akm == WPA_KEY_MGMT_IEEE8021X_SHA256;
}


#define WPA_PROTO_WPA BIT(0)
#define WPA_PROTO_RSN BIT(1)
#if 1 //#ifdef __WAPI_SUPPORT__
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

#if 0  /*defined in driver.h already*/
typedef enum { WPA_ALG_NONE, WPA_ALG_WEP, WPA_ALG_TKIP, WPA_ALG_CCMP,
               WPA_ALG_IGTK, WPA_ALG_PMK
             } wpa_alg;


typedef enum { CIPHER_NONE, CIPHER_WEP40, CIPHER_TKIP, CIPHER_CCMP,
               CIPHER_WEP104
             } wpa_cipher;

typedef enum { KEY_MGMT_802_1X, KEY_MGMT_PSK, KEY_MGMT_NONE,
               KEY_MGMT_802_1X_NO_WPA, KEY_MGMT_WPA_NONE,
               KEY_MGMT_FT_802_1X, KEY_MGMT_FT_PSK,
               KEY_MGMT_802_1X_SHA256, KEY_MGMT_PSK_SHA256,
               KEY_MGMT_WPS
             } wpa_key_mgmt;
#endif

#if 0 /*defined in wpa_supplicant_i.h*/
/**
 * enum wpa_states - wpa_supplicant state
 *
 * These enumeration values are used to indicate the current wpa_supplicant
 * state (wpa_s->wpa_state). The current state can be retrieved with
 * wpa_supplicant_get_state() function and the state can be changed by calling
 * wpa_supplicant_set_state(). In WPA state machine (wpa.c and preauth.c), the
 * wrapper functions wpa_sm_get_state() and wpa_sm_set_state() should be used
 * to access the state variable.
 */
typedef enum {
    /**
     * WPA_DISCONNECTED - Disconnected state
     *
     * This state indicates that client is not associated, but is likely to
     * start looking for an access point. This state is entered when a
     * connection is lost.
     */
    WPA_DISCONNECTED,

    /**
     * WPA_INACTIVE - Inactive state (wpa_supplicant disabled)
     *
     * This state is entered if there are no enabled networks in the
     * configuration. wpa_supplicant is not trying to associate with a new
     * network and external interaction (e.g., ctrl_iface call to add or
     * enable a network) is needed to start association.
     */
    WPA_INACTIVE,

    /**
     * WPA_SCANNING - Scanning for a network
     *
     * This state is entered when wpa_supplicant starts scanning for a
     * network.
     */
    WPA_SCANNING,

    /**
     * WPA_ASSOCIATING - Trying to associate with a BSS/SSID
     *
     * This state is entered when wpa_supplicant has found a suitable BSS
     * to associate with and the driver is configured to try to associate
     * with this BSS in ap_scan=1 mode. When using ap_scan=2 mode, this
     * state is entered when the driver is configured to try to associate
     * with a network using the configured SSID and security policy.
     */
    WPA_ASSOCIATING,

    /**
     * WPA_ASSOCIATED - Association completed
     *
     * This state is entered when the driver reports that association has
     * been successfully completed with an AP. If IEEE 802.1X is used
     * (with or without WPA/WPA2), wpa_supplicant remains in this state
     * until the IEEE 802.1X/EAPOL authentication has been completed.
     */
    WPA_ASSOCIATED,

    /**
     * WPA_4WAY_HANDSHAKE - WPA 4-Way Key Handshake in progress
     *
     * This state is entered when WPA/WPA2 4-Way Handshake is started. In
     * case of WPA-PSK, this happens when receiving the first EAPOL-Key
     * frame after association. In case of WPA-EAP, this state is entered
     * when the IEEE 802.1X/EAPOL authentication has been completed.
     */
    WPA_4WAY_HANDSHAKE,

    /**
     * WPA_GROUP_HANDSHAKE - WPA Group Key Handshake in progress
     *
     * This state is entered when 4-Way Key Handshake has been completed
     * (i.e., when the supplicant sends out message 4/4) and when Group
     * Key rekeying is started by the AP (i.e., when supplicant receives
     * message 1/2).
     */
    WPA_GROUP_HANDSHAKE,

    /**
     * WPA_COMPLETED - All authentication completed
     *
     * This state is entered when the full authentication process is
     * completed. In case of WPA2, this happens when the 4-Way Handshake is
     * successfully completed. With WPA, this state is entered after the
     * Group Key Handshake; with IEEE 802.1X (non-WPA) connection is
     * completed after dynamic keys are received (or if not used, after
     * the EAP authentication has been completed). With static WEP keys and
     * plaintext connections, this state is entered when an association
     * has been completed.
     *
     * This state indicates that the supplicant has completed its
     * processing for the association phase and that data connection is
     * fully configured.
     */
    WPA_COMPLETED
} wpa_states;
#endif

#define MLME_SETPROTECTION_PROTECT_TYPE_NONE 0
#define MLME_SETPROTECTION_PROTECT_TYPE_RX 1
#define MLME_SETPROTECTION_PROTECT_TYPE_TX 2
#define MLME_SETPROTECTION_PROTECT_TYPE_RX_TX 3

#define MLME_SETPROTECTION_KEY_TYPE_GROUP 0
#define MLME_SETPROTECTION_KEY_TYPE_PAIRWISE 1

/* Information Element IDs */
#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_FH_PARAMS 2
#define WLAN_EID_DS_PARAMS 3
#define WLAN_EID_CF_PARAMS 4
#define WLAN_EID_TIM 5
#define WLAN_EID_IBSS_PARAMS 6
#define WLAN_EID_COUNTRY 7
#define WLAN_EID_CHALLENGE 16
/* EIDs defined by IEEE 802.11h - START */
#define WLAN_EID_PWR_CONSTRAINT 32
#define WLAN_EID_PWR_CAPABILITY 33
#define WLAN_EID_TPC_REQUEST 34
#define WLAN_EID_TPC_REPORT 35
#define WLAN_EID_SUPPORTED_CHANNELS 36
#define WLAN_EID_CHANNEL_SWITCH 37
#define WLAN_EID_MEASURE_REQUEST 38
#define WLAN_EID_MEASURE_REPORT 39
#define WLAN_EID_QUITE 40
#define WLAN_EID_IBSS_DFS 41
/* EIDs defined by IEEE 802.11h - END */

#define WLAN_EID_ERP_INFO 42
#define WLAN_EID_HT_CAP 45
#define WLAN_EID_RSN 48
#define WLAN_EID_EXT_SUPP_RATES 50
#define WLAN_EID_MOBILITY_DOMAIN 54
#define WLAN_EID_FAST_BSS_TRANSITION 55
#define WLAN_EID_TIMEOUT_INTERVAL 56
#define WLAN_EID_RIC_DATA 57
#define WLAN_EID_HT_OPERATION 61
#define WLAN_EID_SECONDARY_CHANNEL_OFFSET 62
#define WLAN_EID_20_40_BSS_COEXISTENCE 72
#define WLAN_EID_20_40_BSS_INTOLERANT 73
#define WLAN_EID_OVERLAPPING_BSS_SCAN_PARAMS 74
#define WLAN_EID_MMIE 76
#define WLAN_EID_VENDOR_SPECIFIC 221


#endif /* DEFS_H */
