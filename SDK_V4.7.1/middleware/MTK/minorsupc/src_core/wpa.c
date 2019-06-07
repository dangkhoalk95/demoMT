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

/*
 * WPA Supplicant
 * Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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


//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"
#endif
#include "supc_common.h"

#include "supc_md5.h"
#include "supc_sha1.h"
#include "rc4.h"
#include "aes_wrap.h"
#include "wpa.h"
#include "eloop.h"
#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "driver_mtk.h"
#include "driver.h"
#include "wpa_supplicant.h"
#include "config_ssid.h"
#include "supc_config.h"
#include "l2_packet.h"
#include "eapol_sm.h"

#include "supc_abm_msgs.h"
#include "wpa_supplicant_i.h"

#include "supc_wpa_common.h"
#include "wndrv_supc_msg.h"

void rsn_preauth_candidate_process(struct wpa_supplicant *wpa_s);

#define PMKID_CANDIDATE_PRIO_SCAN 1000

/* TODO: make these configurable */
/* Joseph:
 * The unit of these numbers are seconds
 */
static const int dot11RSNAConfigPMKLifetime = (KAL_TICKS_1_MIN * 720); // 12Hrs
//static const int dot11RSNAConfigPMKReauthThreshold = 70;
//static const int dot11RSNAConfigSATimeout = 60;
static const int pmksa_cache_max_entries = 32;

static const int WPA_SELECTOR_LEN = 4;
static const u8 WPA_OUI_TYPE[] = { 0x00, 0x50, 0xf2, 1 };
static const u16 WPA_VERSION = 1;
static const u8 WPA_AUTH_KEY_MGMT_NONE[] = { 0x00, 0x50, 0xf2, 0 };
static const u8 WPA_AUTH_KEY_MGMT_UNSPEC_802_1X[] = { 0x00, 0x50, 0xf2, 1 };
static const u8 WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X[] = { 0x00, 0x50, 0xf2, 2 };
static const u8 WPA_CIPHER_SUITE_NONE[] = { 0x00, 0x50, 0xf2, 0 };
static const u8 WPA_CIPHER_SUITE_WEP40[] = { 0x00, 0x50, 0xf2, 1 };
static const u8 WPA_CIPHER_SUITE_TKIP[] = { 0x00, 0x50, 0xf2, 2 };
//static const u8 WPA_CIPHER_SUITE_WRAP[] = { 0x00, 0x50, 0xf2, 3 };
static const u8 WPA_CIPHER_SUITE_CCMP[] = { 0x00, 0x50, 0xf2, 4 };
static const u8 WPA_CIPHER_SUITE_WEP104[] = { 0x00, 0x50, 0xf2, 5 };

/* WPA IE version 1
 * 00-50-f2:1 (OUI:OUI type)
 * 0x01 0x00 (version; little endian)
 * (all following fields are optional:)
 * Group Suite Selector (4 octets) (default: TKIP)
 * Pairwise Suite Count (2 octets, little endian) (default: 1)
 * Pairwise Suite List (4 * n octets) (default: TKIP)
 * Authenticated Key Management Suite Count (2 octets, little endian)
 *    (default: 1)
 * Authenticated Key Management Suite List (4 * n octets)
 *    (default: unspec 802.1x)
 * WPA Capabilities (2 octets, little endian) (default: 0)
 */

struct wpa_ie_hdr {
    u8 elem_id;
    u8 len;
    u8 oui[3];
    u8 oui_type;
    u16 version;
} ;


static const int RSN_SELECTOR_LEN = 4;
#ifdef __WAPI_SUPPORT__
static const int WAPI_SELECTOR_LEN = 4;
#endif
static const u16 RSN_VERSION = 1;
#ifdef __WAPI_SUPPORT__
static const u16 WAPI_VERSION = 1;
#endif
static const u8 RSN_AUTH_KEY_MGMT_UNSPEC_802_1X[] = { 0x00, 0x0f, 0xac, 1 };
static const u8 RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X[] = { 0x00, 0x0f, 0xac, 2 };
static const u8 RSN_CIPHER_SUITE_NONE[] = { 0x00, 0x0f, 0xac, 0 };
static const u8 RSN_CIPHER_SUITE_WEP40[] = { 0x00, 0x0f, 0xac, 1 };
static const u8 RSN_CIPHER_SUITE_TKIP[] = { 0x00, 0x0f, 0xac, 2 };
//static const u8 RSN_CIPHER_SUITE_WRAP[] = { 0x00, 0x0f, 0xac, 3 };
static const u8 RSN_CIPHER_SUITE_CCMP[] = { 0x00, 0x0f, 0xac, 4 };
static const u8 RSN_CIPHER_SUITE_WEP104[] = { 0x00, 0x0f, 0xac, 5 };

#ifdef __WAPI_SUPPORT__
static const u8 WAPI_AUTH_KEY_MGMT_WAI[] = { 0x00, 0x14, 0x72, 1 };
static const u8 WAPI_AUTH_KEY_MGMT_PSK[] = { 0x00, 0x14, 0x72, 2 };
static const u8 WAPI_CIPHER_SUITE_WPI[] = { 0x00, 0x14, 0x72, 1 };
#endif

/* EAPOL-Key Key Data Encapsulation
 * GroupKey and STAKey require encryption, otherwise, encryption is optional.
 */
static const u8 RSN_KEY_DATA_GROUPKEY[] = { 0x00, 0x0f, 0xac, 1 };
//static const u8 RSN_KEY_DATA_STAKEY[] = { 0x00, 0x0f, 0xac, 2 };
//static const u8 RSN_KEY_DATA_MAC_ADDR[] = { 0x00, 0x0f, 0xac, 3 };
static const u8 RSN_KEY_DATA_PMKID[] = { 0x00, 0x0f, 0xac, 4 };

/* 1/4: PMKID
 * 2/4: RSN IE
 * 3/4: one or two RSN IEs + GTK IE (encrypted)
 * 4/4: empty
 * 1/2: GTK IE (encrypted)
 * 2/2: empty
 */

/* RSN IE version 1
 * 0x01 0x00 (version; little endian)
 * (all following fields are optional:)
 * Group Suite Selector (4 octets) (default: CCMP)
 * Pairwise Suite Count (2 octets, little endian) (default: 1)
 * Pairwise Suite List (4 * n octets) (default: CCMP)
 * Authenticated Key Management Suite Count (2 octets, little endian)
 *    (default: 1)
 * Authenticated Key Management Suite List (4 * n octets)
 *    (default: unspec 802.1x)
 * RSN Capabilities (2 octets, little endian) (default: 0)
 * PMKID Count (2 octets) (default: 0)
 * PMKID List (16 * n octets)
 */

struct rsn_ie_hdr {
    u8 elem_id; /* WLAN_EID_RSN */
    u8 len;
    u16 version;
} ;


static int wpa_selector_to_bitfield(u8 *s)
{
    if (kal_mem_cmp(s, WPA_CIPHER_SUITE_NONE, WPA_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_NONE;
    }
    if (kal_mem_cmp(s, WPA_CIPHER_SUITE_WEP40, WPA_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_WEP40;
    }
    if (kal_mem_cmp(s, WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_TKIP;
    }
    if (kal_mem_cmp(s, WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_CCMP;
    }
    if (kal_mem_cmp(s, WPA_CIPHER_SUITE_WEP104, WPA_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_WEP104;
    }
    return 0;
}


int wpa_key_mgmt_to_bitfield(u8 *s)
{
    if (memcmp(s, WPA_AUTH_KEY_MGMT_UNSPEC_802_1X, WPA_SELECTOR_LEN) == 0) {
        return WPA_KEY_MGMT_IEEE8021X;
    }
    if (memcmp(s, WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X, WPA_SELECTOR_LEN) ==
            0) {
        return WPA_KEY_MGMT_PSK;
    }
    if (memcmp(s, WPA_AUTH_KEY_MGMT_NONE, WPA_SELECTOR_LEN) == 0) {
        return WPA_KEY_MGMT_WPA_NONE;
    }
    return 0;
}


static int rsn_selector_to_bitfield(u8 *s)
{
    if (memcmp(s, RSN_CIPHER_SUITE_NONE, RSN_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_NONE;
    }
    if (memcmp(s, RSN_CIPHER_SUITE_WEP40, RSN_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_WEP40;
    }
    if (memcmp(s, RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_TKIP;
    }
    if (memcmp(s, RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_CCMP;
    }
    if (memcmp(s, RSN_CIPHER_SUITE_WEP104, RSN_SELECTOR_LEN) == 0) {
        return WPA_CIPHER_WEP104;
    }
    return 0;
}


int rsn_key_mgmt_to_bitfield(u8 *s)
{
    if (memcmp(s, RSN_AUTH_KEY_MGMT_UNSPEC_802_1X, RSN_SELECTOR_LEN) == 0) {
        return WPA_KEY_MGMT_IEEE8021X;
    }
    if (memcmp(s, RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X, RSN_SELECTOR_LEN) ==
            0) {
        return WPA_KEY_MGMT_PSK;
    }
    return 0;
}

#ifdef __WAPI_SUPPORT__
static int wapi_selector_to_bitfield(u8 *s)
{
    if (kal_mem_cmp(s, WAPI_CIPHER_SUITE_WPI, WAPI_SELECTOR_LEN) == 0) {
        return WAPI_CIPHER_SMS4;
    }
    return 0;
}


int wapi_key_mgmt_to_bitfield(u8 *s)
{
    if (memcmp(s, WAPI_AUTH_KEY_MGMT_WAI, WAPI_SELECTOR_LEN) == 0) {
        return WAPI_KEY_MGMT_WAI;
    }
    if (memcmp(s, WAPI_AUTH_KEY_MGMT_PSK, WAPI_SELECTOR_LEN) ==
            0) {
        return WAPI_KEY_MGMT_PSK;
    }
    return 0;
}
#endif

void rsn_pmkid(u8 *pmk, u8 *aa, u8 *spa, u8 *pmkid)
{
    char *title = "PMK Name";
    const unsigned char *addr[3];
    const wpa_size_t len[3] = { 8, ETH_ALEN, ETH_ALEN };
    unsigned char hash[SHA1_MAC_LEN];

    addr[0] = (unsigned char *) title;
    addr[1] = aa;
    addr[2] = spa;

    hmac_sha1_vector(pmk, PMK_LEN, 3, (kal_uint8 **) addr, (kal_uint16 *) len, hash);
    memcpy(pmkid, hash, PMKID_LEN);
}


//static void pmksa_cache_set_expiration(struct wpa_supplicant *wpa_s);


void pmksa_cache_free_entry(struct rsn_pmksa_cache *entry)
{
    SUPC_DEALLOC_BUFF(entry);
    wpa_s->pmksa_count--;
    if (wpa_s->cur_pmksa == entry) {
        wpa_printf(TRACE_GROUP_1, "RSN: removed current PMKSA entry\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RSN_REMOVE_PMKSA_ENTRY);
        /* TODO: should drop PMK and PTK and trigger new key
         * negotiation */
        wpa_s->cur_pmksa = NULL;
    }
}

static void pmksa_cache_add(u8 *pmk,
                            wpa_size_t pmk_len, u8 *aa, u8 *spa)
{
    struct rsn_pmksa_cache *entry, *pos, *prev;

    if (wpa_s->proto != WPA_PROTO_RSN || pmk_len > PMK_LEN) {
        return;
    }

    entry = SUPC_ALLOC_BUFF(sizeof(*entry));
    if (entry == NULL) {
        return;
    }
    memset(entry, 0, sizeof(*entry));
    memcpy(entry->pmk, pmk, pmk_len);
    entry->pmk_len = pmk_len;
    rsn_pmkid(pmk, aa, spa, entry->pmkid);

    entry->event_id = supc_start_pmksa_cache_timer(entry, dot11RSNAConfigPMKLifetime);

    //entry->expiration = time(NULL) + dot11RSNAConfigPMKLifetime;
    entry->akmp = WPA_KEY_MGMT_IEEE8021X;
    memcpy(entry->aa, aa, ETH_ALEN);

    /* Replace an old entry for the same Authenticator (if found) with the
        * new entry */
    pos = wpa_s->pmksa;
    prev = NULL;
    while (pos) {
        if (memcmp(aa, pos->aa, ETH_ALEN) == 0) {
            if (prev == NULL) {
                wpa_s->pmksa = pos->next;
            } else {
                prev->next = pos->next;
            }

            wpa_driver_mtk_remove_pmkid(wpa_s->drv_priv, pos->aa, pos->pmkid);  // Karen
            supc_stop_pmksa_cache_timer(pos);
            pmksa_cache_free_entry(pos);

            break;
        }

        prev = pos;
        pos = pos->next;
    }

#if 0
    if (wpa_s->pmksa_count >= pmksa_cache_max_entries && wpa_s->pmksa) {
        /* Remove the oldest entry to make room for the new entry */
        pos = wpa_s->pmksa;
        wpa_s->pmksa = pos->next;
        wpa_printf(MSG_DEBUG, "RSN: removed the oldest PMKSA cache "
                   "entry (for " MACSTR ") to make room for new one",
                   MAC2STR(pos->aa));
        wpa_driver_mtk_remove_pmkid(wpa_s->drv_priv, pos->aa, pos->pmkid);
        pmksa_cache_free_entry(pos);
    }
#endif
    // Karen
    if (wpa_s->pmksa_count >= pmksa_cache_max_entries && wpa_s->pmksa) {
        pos = wpa_s->pmksa;
        prev = NULL;
        while ((pos != NULL) && (pos->next != NULL)) {
            prev = pos;
            pos = pos->next;
        }
        if (pos != NULL) {
            wpa_printf(TRACE_GROUP_1, "RSN: removed the oldest PMKSA cache "
                       "entry (for " MACSTR ") to make room for new one\n",
                       MAC2STR(pos->aa));
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RSN_REMOVE_OLDEST_PMKSA,MAC2STR(pos->aa));

            if (prev != NULL) {
                prev->next = NULL;
            }

            wpa_driver_mtk_remove_pmkid(wpa_s->drv_priv, pos->aa, pos->pmkid);
            supc_stop_pmksa_cache_timer(pos);
            pmksa_cache_free_entry(pos);

            if (pos == wpa_s->pmksa) {
                wpa_s->pmksa = NULL;
            }
        }
    }
    /* Joseph:
     * The order of ther pmksas are order in nature
     */
    if (wpa_s->pmksa == NULL) {
        wpa_s->pmksa = entry;
    } else {
        entry->next = wpa_s->pmksa;
        wpa_s->pmksa = entry;
    }

    /* Add the new entry; order by expiration time */
    /*
    pos = wpa_s->pmksa;
    prev = NULL;
    while (pos)
    {
        if (pos->expiration > entry->expiration)
        break;
        prev = pos;
        pos = pos->next;
    }

    if (prev == NULL)
    {
        entry->next = wpa_s->pmksa;
        wpa_s->pmksa = entry;
    }
    else
    {
        entry->next = prev->next;
        prev->next = entry;
    }
    */
    wpa_s->pmksa_count++;
    wpa_printf(TRACE_GROUP_1, "RSN: added PMKSA cache entry for " MACSTR "\n",
               MAC2STR(entry->aa));
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RSN_ADD_PMKSA_FOR_AA,MAC2STR(entry->aa));
    //wpa_hexdump(MSG_DEBUG, "RSN: PMKID",
    //(char *)entry->pmkid, PMKID_LEN);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RSN_PMKID_DUMP,PMKID_LEN,entry->pmkid);
    wpa_driver_mtk_add_pmkid(wpa_s->drv_priv, entry->aa, entry->pmkid);
}

void pmksa_cache_del(u8 *aa)
{
    struct rsn_pmksa_cache *pos, *prev;

    wpa_printf(TRACE_GROUP_1, "pmksa_cache_del\n");
//   kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RSN_PMKSA_DEL);

    pos = wpa_s->pmksa;
    prev = NULL;
    while (pos) {
        if (memcmp(aa, pos->aa, ETH_ALEN) == 0) {
            if (prev == NULL) {
                wpa_s->pmksa = pos->next;
            } else {
                prev->next = pos->next;
            }

            wpa_driver_mtk_remove_pmkid(wpa_s->drv_priv, pos->aa, pos->pmkid); // Karen
            supc_stop_pmksa_cache_timer(pos);
            pmksa_cache_free_entry(pos);

            break;
        }

        prev = pos;
        pos = pos->next;
    }
}

void pmksa_cache_free(struct wpa_supplicant *wpa_s)
{
    struct rsn_pmksa_cache *entry, *prev;

    entry = wpa_s->pmksa;
    wpa_s->pmksa = NULL;
    while (entry) {
        prev = entry;
        entry = entry->next;
        // SUPC_DEALLOC_BUFF(prev);
        // evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr,
        // &(prev->event_id));
        supc_stop_pmksa_cache_timer(prev);
        pmksa_cache_free_entry(prev);   // Karen
    }

    wpa_driver_mtk_flush_pmkid(wpa_s->drv_priv);

    //pmksa_cache_set_expiration(wpa_s);
    wpa_s->cur_pmksa = NULL;
}


struct rsn_pmksa_cache *pmksa_cache_get(struct wpa_supplicant *wpa_s,
                                        u8 *aa, u8 *pmkid)
{
    struct rsn_pmksa_cache *entry = wpa_s->pmksa;
    while (entry) {
        if ((aa == NULL || kal_mem_cmp(entry->aa, aa, ETH_ALEN) == 0) &&
                (pmkid == NULL || kal_mem_cmp(entry->pmkid, pmkid, PMKID_LEN) == 0)) {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

void pmksa_candidate_free(struct wpa_supplicant *wpa_s)
{
    struct rsn_pmksa_candidate *entry, *prev;

    entry = wpa_s->pmksa_candidates;
    wpa_s->pmksa_candidates = NULL;
    while (entry) {
        prev = entry;
        entry = entry->next;
        SUPC_DEALLOC_BUFF(prev);
    }
}


static int wpa_parse_wpa_ie_wpa(struct wpa_supplicant *wpa_s, u8 *wpa_ie,
                                wpa_size_t wpa_ie_len, struct wpa_ie_data *data)
{
    struct wpa_ie_hdr *hdr;
    u8 *pos;
    int left;
    int i, count;

    data->proto = WPA_PROTO_WPA;
    data->pairwise_cipher = WPA_CIPHER_TKIP;
    data->group_cipher = WPA_CIPHER_TKIP;
    data->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
    data->capabilities = 0;
    data->pmkid = NULL;
    data->num_pmkid = 0;

    if (wpa_ie_len == 0) {
        /* No WPA IE - fail silently */
        wpa_printf(TRACE_ERROR, "WPA IE len is 0\n");
//kal_trace(DBG_CONN,INFO_SUPC_WPA_LEN_0);
        return -1;
    }

    if (wpa_ie_len < sizeof(struct wpa_ie_hdr)) {
        wpa_printf(TRACE_ERROR, "WPA IE len too short %lu\n",
                   (unsigned long) wpa_ie_len);
//      kal_trace(DBG_CONN,INFO_SUPC_WPA_IE_LEN_SHORT,wpa_ie_len);
        return -1;
    }

    hdr = (struct wpa_ie_hdr *) wpa_ie;

    if (hdr->elem_id != GENERIC_INFO_ELEM ||
            hdr->len != wpa_ie_len - 2 ||
            kal_mem_cmp(&hdr->oui, WPA_OUI_TYPE, WPA_SELECTOR_LEN) != 0 ||
            le_to_host16(hdr->version) != WPA_VERSION) {
        wpa_printf(TRACE_ERROR, "malformed WPA IE or unknown version\n");
//      kal_trace(DBG_CONN,INFO_SUPC_MALFORMED_WPA_IE);
        return -1;
    }

    pos = (u8 *)(hdr + 1);
    left = wpa_ie_len - sizeof(*hdr);

    if (left >= WPA_SELECTOR_LEN) {
        data->group_cipher = wpa_selector_to_bitfield(pos);
        pos += WPA_SELECTOR_LEN;
        left -= WPA_SELECTOR_LEN;
    } else if (left > 0) {
        wpa_printf(TRACE_ERROR, "WPA IE length mismatch, %u too much\n", left);
//      kal_trace(DBG_CONN,INFO_SUPC_WPA_LEN_MISMATCH_TOO_MUCH,left);
        return -1;
    }

    if (left >= 2) {
        data->pairwise_cipher = 0;
        count = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WPA_SELECTOR_LEN) {
            wpa_printf(TRACE_ERROR, "WPA IE count botch (pairwise), "
                       "count %u left %u\n", count, left);
//          kal_trace(DBG_CONN,INFO_SUPC_WPA_IE_COUNT_BOTCH_ERROR, count, left);

            return -1;
        }
        for (i = 0; i < count; i++) {
            data->pairwise_cipher |= wpa_selector_to_bitfield(pos);
            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    } else if (left == 1) {
        wpa_printf(TRACE_ERROR, "WPA IE too short (for key mgmt)\n");
//      kal_trace(DBG_CONN,INFO_SUPC_WPA_IE_SHORT_KEY_MGMT);
        return -1;
    }

    if (left >= 2) {
        data->key_mgmt = 0;
        count = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WPA_SELECTOR_LEN) {
            wpa_printf(TRACE_ERROR, "WPA IE count botch (key mgmt), "
                       "count %u left %u\n", count, left);
//kal_trace(DBG_CONN,INFO_SUPC_WPA_IE_COUNT_BOTCH_ERROR_FOR_MGMT, count, left);
            return -1;
        }
        for (i = 0; i < count; i++) {
            data->key_mgmt |= wpa_key_mgmt_to_bitfield(pos);
            pos += WPA_SELECTOR_LEN;
            left -= WPA_SELECTOR_LEN;
        }
    } else if (left == 1) {
        wpa_printf(TRACE_ERROR, "WPA IE too short (for capabilities)\n");
//      kal_trace(DBG_CONN,INFO_SUPC_WPA_IE_SHORT_CAPA);
        return -1;
    }

    if (left >= 2) {
        data->capabilities = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
#if CONFIG_DRIVER_MTK // whsu
        wpa_s->app_wpa_cap = 1;
#endif
    }
#if CONFIG_DRIVER_MTK // whsu
    else {
        wpa_s->app_wpa_cap = 0;
    }
#endif

    if (left > 0) {
        wpa_printf(TRACE_ERROR, "WPA IE has %u trailing bytes\n", left);
//      kal_trace(DBG_CONN,INFO_SUPC_WPA_IE_HAVE_TRAILING_BYTES, left);
        return -1;
    }

    return 0;
}


static int wpa_parse_wpa_ie_rsn(struct wpa_supplicant *wpa_s, u8 *rsn_ie,
                                wpa_size_t rsn_ie_len, struct wpa_ie_data *data)
{
    struct rsn_ie_hdr *hdr;
    u8 *pos;
    int left;
    int i, count;

    data->proto = WPA_PROTO_RSN;
    data->pairwise_cipher = WPA_CIPHER_CCMP;
    data->group_cipher = WPA_CIPHER_CCMP;
    data->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
    data->capabilities = 0;
    data->pmkid = NULL;
    data->num_pmkid = 0;
    if (rsn_ie_len == 0) {
        /* No RSN IE - fail silently */
        wpa_printf(TRACE_ERROR, "RSN IE len is 0\n");
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_IE_0);
        return -1;
    }
    if (rsn_ie_len < sizeof(struct rsn_ie_hdr)) {
        wpa_printf(TRACE_ERROR, "RSN IE len too short %lu\n", (unsigned long) rsn_ie_len);
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_IE_TOO_SHORT, rsn_ie_len);
        return -1;
    }

    hdr = (struct rsn_ie_hdr *) rsn_ie;

    if (hdr->elem_id != RSN_INFO_ELEM ||
            hdr->len != rsn_ie_len - 2 ||
            le_to_host16(hdr->version) != RSN_VERSION) {
        wpa_printf(TRACE_ERROR, "malformed RSN IE or unknown version\n");
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_IE_MALFORMED);
        return -1;
    }
    pos = (u8 *)(hdr + 1);
    left = rsn_ie_len - sizeof(*hdr);

    if (left >= RSN_SELECTOR_LEN) {

        data->group_cipher = rsn_selector_to_bitfield(pos);
        pos += RSN_SELECTOR_LEN;
        left -= RSN_SELECTOR_LEN;
    } else if (left > 0) {
        wpa_printf(TRACE_ERROR, "RSN IE length mismatch, %u too much\n", left);
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_LEN_MISMATCH_TOO_MUCH, left);
        return -1;
    }

    if (left >= 2) {
        data->pairwise_cipher = 0;
        count = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * RSN_SELECTOR_LEN) {
            wpa_printf(TRACE_ERROR, "RSN IE count botch (pairwise), "
                       "count %u left %u\n", count, left);
//          kal_trace(DBG_CONN,INFO_SUPC_RSN_LEN_COUNT_BOTCH_ERROR, count, left);
            return -1;
        }
        for (i = 0; i < count; i++) {
            data->pairwise_cipher |= rsn_selector_to_bitfield(pos);
            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }
    } else if (left == 1) {
        wpa_printf(TRACE_ERROR, "RSN IE too short (for key mgmt)\n");
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_LEN_SHORT_KEY_MGMT);
        return -1;
    }

    if (left >= 2) {
        data->key_mgmt = 0;
        count = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * RSN_SELECTOR_LEN) {
            wpa_printf(TRACE_ERROR, "RSN IE count botch (key mgmt), "
                       "count %u left %u\n", count, left);
//          kal_trace(DBG_CONN,INFO_SUPC_RSN_LEN_COUNT_BOTCH_ERROR_FOR_MGMT, count, left);
            return -1;
        }
        for (i = 0; i < count; i++) {
            data->key_mgmt |= rsn_key_mgmt_to_bitfield(pos);
            pos += RSN_SELECTOR_LEN;
            left -= RSN_SELECTOR_LEN;
        }
    } else if (left == 1) {
        wpa_printf(TRACE_ERROR, "RSN IE too short (for capabilities)\n");
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_IE_SHORT_CAPA);
        return -1;
    }

    if (left >= 2) {
        data->capabilities = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
    }

    if (left >= 2) {
        data->num_pmkid = pos[0] | (pos[1] << 8);
        pos += 2;
        left -= 2;
        if (left < data->num_pmkid * PMKID_LEN) {
            wpa_printf(TRACE_ERROR, "PMKID underflow "
                       "(num_pmkid=%d left=%d)\n", data->num_pmkid, left);
//          kal_trace(DBG_CONN,INFO_SUPC_PMKID_UNDERFLOW, data->num_pmkid, left);
            data->num_pmkid = 0;
        } else {
            data->pmkid = pos;
            pos += data->num_pmkid * PMKID_LEN;
            left -= data->num_pmkid * PMKID_LEN;
        }
    }

    if (left > 0) {
        wpa_printf(TRACE_ERROR, "RSN IE has %u trailing bytes - ignored\n", left);
//      kal_trace(DBG_CONN,INFO_SUPC_RSN_IE_HAVE_TRAILING_BYTES, left);
    }
    return 0;
}


int wpa_parse_wpa_ie(struct wpa_supplicant *wpa_s, u8 *wpa_ie,
                     wpa_size_t wpa_ie_len, struct wpa_ie_data *data)
{
    if (wpa_ie_len >= 1 && wpa_ie[0] == RSN_INFO_ELEM) {
        return wpa_parse_wpa_ie_rsn(wpa_s, wpa_ie, wpa_ie_len, data);
    } else {
        return wpa_parse_wpa_ie_wpa(wpa_s, wpa_ie, wpa_ie_len, data);
    }
}

#ifdef __WAPI_SUPPORT__
int wpa_parse_wapi_ie(struct wpa_supplicant *wpa_s, u8 *wapi_ie,
                      wpa_size_t wapi_ie_len, struct wpa_ie_data *data)
{
    struct rsn_ie_hdr *hdr;
    u8 *pos;
    int left;
    int i, count;

    data->proto = WPA_PROTO_WAPI;
    data->pairwise_cipher = WAPI_CIPHER_SMS4;
    data->group_cipher = WAPI_CIPHER_SMS4;
    data->key_mgmt = WAPI_KEY_MGMT_PSK;
    data->capabilities = 0;
    data->pmkid = NULL;
    data->num_pmkid = 0;
    if (wapi_ie_len == 0) {
        /* No WAPI IE - fail silently */
        //wpa_printf(MSG_ERROR, "WAPI IE len is 0");
        kal_trace(DBG_WAPI, INFO_SUPC_NO_WAPI_IE);
        return -1;
    }
    if (wapi_ie_len < sizeof(struct rsn_ie_hdr)) {
        //wpa_printf(MSG_ERROR, "WAPI IE len too short %lu", (unsigned long) wapi_ie_len);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_TOO_SHORT,  wapi_ie_len);
        return -1;
    }

    hdr = (struct rsn_ie_hdr *) wapi_ie;

    if (hdr->elem_id != WAPI_INFO_ELEM ||
            hdr->len != wapi_ie_len - 2 ||
            le_to_host16(hdr->version) != WAPI_VERSION) {
        //wpa_printf(MSG_ERROR, "malformed WAPI IE or unknown version");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_MALFORMED);
        return -1;
    }
    pos = (u8 *)(hdr + 1);
    left = wapi_ie_len - sizeof(*hdr);

    if (left >= 2) {
        count = pos[0] | (((u16)pos[1]) << 8);

        if (count != 1) {
            //wpa_printf(MSG_ERROR, "WAPI IE invalid AKM count %u", count);
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_INVALID_AKM_COUNT, count);
            return -1;
        }

        pos += 2;
        left -= 2;
    } else {
        //wpa_printf(MSG_ERROR, "WAPI IE length mismatch, %u too much", left);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_LEN_MISMATCH, left);
        return -1;
    }

    if (left >= WAPI_SELECTOR_LEN) {
        data->key_mgmt = wapi_key_mgmt_to_bitfield(pos);
        pos += WAPI_SELECTOR_LEN;
        left -= WAPI_SELECTOR_LEN;
    } else {
        //wpa_printf(MSG_ERROR, "WAPI IE length mismatch, %u too much", left);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_LEN_MISMATCH, left);
        return -1;
    }


    if (left >= 2) {
        data->pairwise_cipher = 0;
        count = pos[0] | (((int)pos[1]) << 8);
        pos += 2;
        left -= 2;
        if (count == 0 || left < count * WAPI_SELECTOR_LEN) {
            //wpa_printf(MSG_ERROR, "WAPI IE count botch (pairwise), "
            //"count %u left %u", count, left);
            kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_COUNT_BOTCH, count, left);
            return -1;
        }

        for (i = 0; i < count; i++) {
            data->pairwise_cipher |= wapi_selector_to_bitfield(pos);
            pos += WAPI_SELECTOR_LEN;
            left -= WAPI_SELECTOR_LEN;
        }
    } else if (left == 1) {
        //wpa_printf(MSG_ERROR, "WAPI IE too short (for key mgmt)");
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_TOO_SHORT_FOR_KEY_MGMT);
        return -1;
    }

    if (left >= WAPI_SELECTOR_LEN) {

        data->group_cipher = wapi_selector_to_bitfield(pos);
        pos += WAPI_SELECTOR_LEN;
        left -= WAPI_SELECTOR_LEN;
    } else if (left > 0) {
        //wpa_printf(MSG_ERROR, "WAPI IE length mismatch, %u too much", left);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_LEN_MISMATCH, left);
        return -1;
    }

    if (left >= 2) {
        data->capabilities = pos[0] | (((int)pos[1]) << 8);
        pos += 2;
        left -= 2;
    }

    /* ignore WAPI BKID we do not support anyway. */

    if (left > 0) {
        //wpa_printf(MSG_ERROR, "WAPI IE has %u trailing bytes - ignored", left);
        kal_trace(DBG_WAPI, INFO_SUPC_WAPI_IE_HAVE_TRAILING_BYTES, left);
    }
    return 0;
}
#endif


static int wpa_gen_wpa_ie_wpa(struct wpa_supplicant *wpa_s, u8 *wpa_ie)
{
    u8 *pos;
    struct wpa_ie_hdr *hdr;

    hdr = (struct wpa_ie_hdr *) wpa_ie;
    hdr->elem_id = GENERIC_INFO_ELEM;
    kal_mem_cpy(&hdr->oui, WPA_OUI_TYPE, WPA_SELECTOR_LEN);
    hdr->version = host_to_le16(WPA_VERSION);
    pos = (u8 *)(hdr + 1);

    if (wpa_s->group_cipher == WPA_CIPHER_CCMP) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN);
    } else if (wpa_s->group_cipher == WPA_CIPHER_TKIP) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN);
    } else if (wpa_s->group_cipher == WPA_CIPHER_WEP104) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_WEP104, WPA_SELECTOR_LEN);
    } else if (wpa_s->group_cipher == WPA_CIPHER_WEP40) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_WEP40, WPA_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid group cipher (%d).\n",
                   wpa_s->group_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_INVALID_GROUP_CIPHER,wpa_s->group_cipher);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (wpa_s->pairwise_cipher == WPA_CIPHER_CCMP) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN);
    } else if (wpa_s->pairwise_cipher == WPA_CIPHER_TKIP) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN);
    } else if (wpa_s->pairwise_cipher == WPA_CIPHER_NONE) {
        kal_mem_cpy(pos, WPA_CIPHER_SUITE_NONE, WPA_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid pairwise cipher (%d).\n",
                   wpa_s->pairwise_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_INVALID_PAIRWISE_CIPHER,wpa_s->pairwise_cipher);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        kal_mem_cpy(pos, WPA_AUTH_KEY_MGMT_UNSPEC_802_1X, WPA_SELECTOR_LEN);
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
        kal_mem_cpy(pos, WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X,
                    WPA_SELECTOR_LEN);
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        kal_mem_cpy(pos, WPA_AUTH_KEY_MGMT_NONE, WPA_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid key management type (%d).\n",
                   wpa_s->key_mgmt);
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_INVALID_KEY_MGMT_TPYE,wpa_s->key_mgmt);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    /* WPA Capabilities; use defaults, so no need to include it */

#if 1 /* whsu : test */
    if (wpa_s->app_wpa_cap) {
        *pos++ = 0x00;
        *pos++ = 0x00;
    }
#endif
    hdr->len = (pos - wpa_ie) - 2;
    return pos - wpa_ie;
}


static int wpa_gen_wpa_ie_rsn(struct wpa_supplicant *wpa_s, u8 *rsn_ie)
{
    u8 *pos;
    struct rsn_ie_hdr *hdr;

    hdr = (struct rsn_ie_hdr *) rsn_ie;
    hdr->elem_id = RSN_INFO_ELEM;
    hdr->version = host_to_le16(RSN_VERSION);
    pos = (u8 *)(hdr + 1);

    if (wpa_s->group_cipher == WPA_CIPHER_CCMP) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN);
    } else if (wpa_s->group_cipher == WPA_CIPHER_TKIP) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN);
    } else if (wpa_s->group_cipher == WPA_CIPHER_WEP104) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_WEP104, RSN_SELECTOR_LEN);
    } else if (wpa_s->group_cipher == WPA_CIPHER_WEP40) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_WEP40, RSN_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid group cipher (%d).\n",
                   wpa_s->group_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_INVALID_GROUP_CIPHER,wpa_s->group_cipher);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (wpa_s->pairwise_cipher == WPA_CIPHER_CCMP) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN);
    } else if (wpa_s->pairwise_cipher == WPA_CIPHER_TKIP) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN);
    } else if (wpa_s->pairwise_cipher == WPA_CIPHER_NONE) {
        kal_mem_cpy(pos, RSN_CIPHER_SUITE_NONE, RSN_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid pairwise cipher (%d).\n",
                   wpa_s->pairwise_cipher);
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_INVALID_PAIRWISE_CIPHER,wpa_s->pairwise_cipher);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        kal_mem_cpy(pos, RSN_AUTH_KEY_MGMT_UNSPEC_802_1X, RSN_SELECTOR_LEN);
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
        kal_mem_cpy(pos, RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X,
                    RSN_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid key management type (%d).\n",
                   wpa_s->key_mgmt);
//      kal_trace(DBG_CONN,INFO_SUPC_GEN_INVALID_KEY_MGMT_TPYE,wpa_s->key_mgmt);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    /* RSN Capabilities */
    *pos++ = 0;
    *pos++ = 0;

    if (wpa_s->cur_pmksa) {
        /* PMKID Count (2 octets, little endian) */
        *pos++ = 1;
        *pos++ = 0;
        /* PMKID */
        kal_mem_cpy(pos, wpa_s->cur_pmksa->pmkid, PMKID_LEN);
        pos += PMKID_LEN;
    }

    hdr->len = (pos - rsn_ie) - 2;

    return pos - rsn_ie;
}

#ifdef __WAPI_SUPPORT__
int wpa_gen_wapi_ie(struct wpa_supplicant *wpa_s, u8 *wapi_ie)
{
    u8 *pos;
    struct rsn_ie_hdr *hdr;

    hdr = (struct rsn_ie_hdr *) wapi_ie;
    hdr->elem_id = WAPI_INFO_ELEM;
    hdr->version = host_to_le16(WAPI_VERSION);
    pos = (u8 *)(hdr + 1);

    /* akm count = 1 */
    *pos++ = 1;
    *pos++ = 0;

    if (wpa_s->key_mgmt == WAPI_KEY_MGMT_WAI) {
        kal_mem_cpy(pos, WAPI_AUTH_KEY_MGMT_WAI, WAPI_SELECTOR_LEN);
    } else if (wpa_s->key_mgmt == WAPI_KEY_MGMT_PSK) {
        kal_mem_cpy(pos, WAPI_AUTH_KEY_MGMT_PSK, WAPI_SELECTOR_LEN);
    } else {
        wpa_printf(TRACE_ERROR, "Invalid key management type (%d).\n",
                   wpa_s->key_mgmt);
        kal_trace(DBG_CONN, INFO_SUPC_GEN_INVALID_KEY_MGMT_TPYE, wpa_s->key_mgmt);
        return -1;
    }
    pos += WAPI_SELECTOR_LEN;

    /* unicase count = 1 */
    *pos++ = 1;
    *pos++ = 0;

    if (wpa_s->pairwise_cipher == WAPI_CIPHER_SMS4) {
        kal_mem_cpy(pos, WAPI_CIPHER_SUITE_WPI, WAPI_SELECTOR_LEN);
    } else {
        //wpa_printf(MSG_ERROR, "Invalid pairwise cipher (%d).",
        // wpa_s->pairwise_cipher);
        kal_trace(DBG_CONN, INFO_SUPC_GEN_INVALID_PAIRWISE_CIPHER, wpa_s->pairwise_cipher);
        return -1;
    }
    pos += WAPI_SELECTOR_LEN;

    /* group cast key count */
    if (wpa_s->group_cipher == WAPI_CIPHER_SMS4) {
        kal_mem_cpy(pos, WAPI_CIPHER_SUITE_WPI, WAPI_SELECTOR_LEN);
    } else {
        //wpa_printf(MSG_ERROR, "Invalid group cipher (%d).",
        // wpa_s->group_cipher);
        kal_trace(DBG_CONN, INFO_SUPC_GEN_INVALID_GROUP_CIPHER, wpa_s->group_cipher);
        return -1;
    }
    pos += WAPI_SELECTOR_LEN;

    /* WAPI Capabilities */
    *pos++ = 0;
    *pos++ = 0;

    /* BKID Count (2 octets, little endian) */
    *pos++ = 0;
    *pos++ = 0;


    hdr->len = (pos - wapi_ie) - 2;

    return pos - wapi_ie;
}
#endif

int wpa_gen_wpa_ie(struct wpa_supplicant *wpa_s, u8 *wpa_ie)
{
    if (wpa_s->proto == WPA_PROTO_RSN) {
        return wpa_gen_wpa_ie_rsn(wpa_s, wpa_ie);
    } else {
        return wpa_gen_wpa_ie_wpa(wpa_s, wpa_ie);
    }
}


static void wpa_pmk_to_ptk(u8 *pmk, wpa_size_t pmk_len, u8 *addr1, u8 *addr2,
                           u8 *nonce1, u8 *nonce2, u8 *ptk, wpa_size_t ptk_len)
{
    u8 data[2 * ETH_ALEN + 2 * 32];

    /* PTK = PRF-X(PMK, "Pairwise key expansion",
     *             Min(AA, SA) || Max(AA, SA) ||
     *             Min(ANonce, SNonce) || Max(ANonce, SNonce)) */
    if (kal_mem_cmp(addr1, addr2, ETH_ALEN) < 0) {
        kal_mem_cpy(data, addr1, ETH_ALEN);
        kal_mem_cpy(data + ETH_ALEN, addr2, ETH_ALEN);
    } else {
        kal_mem_cpy(data, addr2, ETH_ALEN);
        kal_mem_cpy(data + ETH_ALEN, addr1, ETH_ALEN);
    }

    if (kal_mem_cmp(nonce1, nonce2, 32) < 0) {
        kal_mem_cpy(data + 2 * ETH_ALEN, nonce1, 32);
        kal_mem_cpy(data + 2 * ETH_ALEN + 32, nonce2, 32);
    } else {
        kal_mem_cpy(data + 2 * ETH_ALEN, nonce2, 32);
        kal_mem_cpy(data + 2 * ETH_ALEN + 32, nonce1, 32);
    }

    sha1_prf(pmk, pmk_len, "Pairwise key expansion", data, sizeof(data),
             ptk, ptk_len);
    //sha1_prf(pmk, pmk_len, "Pairwise key expansion", data2, sizeof(data2),
    //   ptk, ptk_len);

    //wpa_hexdump_key(MSG_DEBUG, "WPA: PMK", (char *)pmk, pmk_len);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_WPA_DUMP_PMK,pmk_len,pmk);
    //wpa_hexdump_key(MSG_DEBUG, "WPA: PTK", (char *)ptk, ptk_len);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_WPA_DUMP_PTK,ptk_len,ptk);
}


struct wpa_ssid *wpa_supplicant_get_ssid(struct wpa_supplicant *wpa_s)
{
    struct wpa_ssid *entry;
    u8 ssid[MAX_SSID_LEN];
    int ssid_len;
    u8 bssid[ETH_ALEN];

    ssid_len = wpa_driver_mtk_get_ssid(wpa_s, ssid);
    if (ssid_len < 0) {
        wpa_printf(TRACE_ERROR, "Could not read SSID from driver.\n");
//      kal_trace(DBG_CONN,INFO_SUPC_COULD_NOT_READ_SSID_FROM_DRV);
        return NULL;
    }

    if (wpa_driver_mtk_get_bssid(wpa_s, bssid) < 0) {
        wpa_printf(TRACE_ERROR, "Could not read BSSID from driver.\n");
//      kal_trace(DBG_CONN,INFO_SUPC_COULD_NOT_READ_BSSID_FROM_DRV);
        return NULL;
    }
    if (NULL == wpa_s->conf) {
        wpa_printf(TRACE_ERROR, "Error!! wpa_s->conf is NULL\n");
//      kal_trace(DBG_CONN,INFO_SUPC_NULL_WPA_CONF);
        return NULL;
    }
    entry = wpa_s->conf->ssid;
    while (entry) {
        if (ssid_len == entry->ssid_len &&
                kal_mem_cmp(ssid, entry->ssid, ssid_len) == 0 &&
                (!entry->bssid_set ||
                 kal_mem_cmp(bssid, entry->bssid, ETH_ALEN) == 0)) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}


static void wpa_eapol_key_mic(u8 *key, int ver, u8 *buf, wpa_size_t len, u8 *mic)
{
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4) {
        hmac_md5(key, 16, buf, len, mic);

    } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        u8 hash[SHA1_MAC_LEN];
        hmac_sha1(key, 16, buf, len, hash);
        kal_mem_cpy(mic, hash, MD5_MAC_LEN);
    }
}


void wpa_supplicant_key_request(struct wpa_supplicant *wpa_s,
                                int error, int pairwise)
{
    kal_uint16 rlen;
    struct ieee802_1x_hdr *hdr;
    int key_info, ver;
    u8 bssid[ETH_ALEN];
    kal_uint8 *pdu_ptr = NULL;
    kal_uint16 pdu_len = 0;
    kal_uint8 *encode_ptr = NULL, *mic_ptr = NULL;
    wndrv_supc_data_req_struct *local_para_ptr = NULL;
    peer_buff_struct *peer_buff_ptr = NULL;

    if (wpa_s->pairwise_cipher == WPA_CIPHER_CCMP) {
        ver = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
    } else {
        ver = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;
    }

    if (wpa_driver_mtk_get_bssid(wpa_s, bssid) < 0) {
        wpa_printf(TRACE_ERROR, "Failed to read BSSID for EAPOL-Key "
                   "request\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_FAIL_TO_READ_BSSID_FROM_EAPOL_KEY);
        return;
    }

    /* J: local_para_ptr */
    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    kal_mem_cpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    kal_mem_cpy(local_para_ptr->dest_mac_addr, bssid, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_EAPOL);

    /* J: peer_buff_ptr */
    // rlen = sizeof(*hdr) + sizeof(*reply);
    rlen = sizeof(*hdr) + WPA_EAPOL_KEY_SIZE;
//    peer_buff_ptr = construct_peer_buff(rlen, IEEE8021X_HEADER_LEN, 0, TD_UL);
//    hdr = (struct ieee802_1x_hdr *) (peer_buff_ptr);
//    hdr->version = wpa_s->conf->eapol_version;
//    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
//    hdr->length = htons(sizeof(*reply));

    //peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    kal_mem_set(pdu_ptr, 0, pdu_len);

    hdr = (struct ieee802_1x_hdr *)(pdu_ptr);
    hdr->version = wpa_s->conf->eapol_version;
    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
    // hdr->length = htons(sizeof(*reply));
    hdr->length = htons(WPA_EAPOL_KEY_SIZE);

    /* Key negotiation part */
    encode_ptr = (kal_uint8 *)(hdr + 1);
    // Descriptor Type
    *encode_ptr = wpa_s->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    encode_ptr++;

    // Key information
    key_info = WPA_KEY_INFO_REQUEST | ver;
    if (wpa_s->ptk_set) {
        key_info |= WPA_KEY_INFO_MIC;
    }
    if (error) {
        key_info |= WPA_KEY_INFO_ERROR;
    }
    if (pairwise) {
        key_info |= WPA_KEY_INFO_KEY_TYPE;
    }

    *encode_ptr = (key_info & 0xFF00) >> 8;
    encode_ptr++;
    *encode_ptr = key_info & 0x00FF;
    encode_ptr++;

    // Key length
    *encode_ptr = 0;
    encode_ptr ++;
    *encode_ptr = 0;
    encode_ptr ++;

    // Key replay counter
    kal_mem_cpy(encode_ptr, wpa_s->request_counter,
                WPA_REPLAY_COUNTER_LEN);
    encode_ptr += WPA_REPLAY_COUNTER_LEN;
    inc_byte_array(wpa_s->request_counter, WPA_REPLAY_COUNTER_LEN);

    // Key nounce
    // kal_mem_set(encode_ptr, 0, WPA_NONCE_LEN);
    encode_ptr += WPA_NONCE_LEN;

    // Key IV (skiped)
    encode_ptr += 16;

    // Key RSC (skiped)
    encode_ptr += 8;

    // Reserved (skiped)
    encode_ptr += 8;

    // Key MIC as 0
    mic_ptr = encode_ptr;
    encode_ptr += 16;



    // Key data length
    *encode_ptr = 0;
    encode_ptr ++;
    *encode_ptr = 0;
    encode_ptr ++;

    // Key MIC
    if (key_info & WPA_KEY_INFO_MIC) {
        wpa_eapol_key_mic(wpa_s->ptk.mic_key, ver, (u8 *) hdr,
                          rlen, mic_ptr);
    }

    wpa_printf(TRACE_GROUP_1, "WPA: Sending EAPOL-Key Request (error=%d "
               "pairwise=%d ptk_set=%d len=%d)",
               error, pairwise, wpa_s->ptk_set, rlen);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_SEND_EAPOL_KEY_REQ,error, pairwise, wpa_s->ptk_set, rlen);

    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);
    eapol_sm_notify_tx_eapol_key(wpa_s->eapol);

    return;
}


#ifdef _SUPC_UNITTEST_

u8 sample_snonce_wpa_psk[]      = {0x8D, 0x2F, 0x88, 0x56, 0x05, 0xE8, 0x90, 0x65, 0x76\
                                   , 0xA7, 0x14, 0x01, 0x5A, 0xEC, 0x6B, 0x5B, 0x88, 0x3B, 0x4B, 0x35, 0x0C, 0x9D, 0x9F, 0x89, 0x26, 0xC7\
                                   , 0xDB, 0xE8, 0x62, 0x87, 0xE3, 0x68
                                  };

u8 sample_snonce_wpa2_psk[]      = {0x67, 0x3A, 0x94, 0x63, 0x68, 0x1A, 0x86, 0x8B, 0x7B\
                                    , 0x04, 0x27, 0x6E, 0x24, 0x0C, 0x11, 0xFC, 0x4B, 0x85, 0xA6, 0x62, 0xDD, 0x9C, 0xD3, 0xFE, 0x9D, 0xB6\
                                    , 0xEC, 0xDE, 0x1F, 0x49, 0xDF, 0xC5
                                   };

u8 sample_snonce_wpa3_psk[]      = {0xca, 0x25, 0x55, 0x1b, 0x27, 0xe7, 0xe0, 0x1f, 0x06, \
                                    0x01, 0xbb, 0x28, 0xcd, 0x1e, 0x88, 0xc5, 0x4e, 0x7e, 0x4e, 0x06, 0x5e, 0xef, 0x54, 0xb4, 0x54, 0x03, \
                                    0x5e, 0x13, 0x7f, 0xe3, 0x18, 0x4f
                                   };
#endif /* _SUPC_UNITTEST_ */


static void wpa_supplicant_process_1_of_4(struct wpa_supplicant *wpa_s,
        unsigned char *src_addr,
        struct wpa_eapol_key *key, int ver, u8 *key_data_ptr)
{
    kal_uint16 rlen;
    struct ieee802_1x_hdr *hdr;
    // struct wpa_eapol_key *reply;
    //unsigned char *rbuf;
    // struct l2_ethhdr *ethhdr;
    struct wpa_ssid *ssid;
    struct wpa_ptk *ptk;
    u8 buf[8], wpa_ie_buf[80], *wpa_ie, *pmkid = NULL;
    int wpa_ie_len;
    int abort_cached = 0;
    wndrv_supc_data_req_struct *local_para_ptr = NULL;
    peer_buff_struct *peer_buff_ptr = NULL;
    kal_uint8 *pdu_ptr = NULL;
    kal_uint8 *encode_ptr = NULL;
    kal_uint8 *mic_ptr = NULL;
    kal_uint16 pdu_len = 0;

    wpa_s->wpa_state = WPA_4WAY_HANDSHAKE;
    wpa_printf(TRACE_GROUP_1, "WPA: RX message 1 of 4-Way Handshake from "
               MACSTR " (ver=%d)\n", MAC2STR(src_addr), ver);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_FROM, MAC2STR(src_addr), ver);

    ssid = wpa_supplicant_get_ssid(wpa_s);
    if (ssid == NULL) {
        wpa_printf(TRACE_WARNING, "WPA: No SSID info found (msg 1 of "
                   "4).");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_NOSSID_FOUND);
        return;
    }

    if (wpa_s->proto == WPA_PROTO_RSN) {
        /* RSN: msg 1/4 should contain PMKID for the selected PMK */
        u8 *pos = (u8 *)(key_data_ptr);
        u8 *end = pos + (key->key_data_length);
        //wpa_hexdump(MSG_DEBUG, "RSN: msg 1/4 key data",
        //(char *)pos, (key->key_data_length));
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_DUMP,key->key_data_length,pos);

        while (pos + 1 < end) {
            if (pos + 2 + pos[1] > end) {
                wpa_printf(TRACE_GROUP_1, "RSN: key data "
                           "underflow (ie=%d len=%d)",
                           pos[0], pos[1]);
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_UNDER_FLOW,pos[0], pos[1]);
                break;
            }

            if (pos[0] == GENERIC_INFO_ELEM &&
                    pos + 1 + RSN_SELECTOR_LEN < end &&
                    pos[1] >= RSN_SELECTOR_LEN + PMKID_LEN &&
                    memcmp(pos + 2, RSN_KEY_DATA_PMKID,
                           RSN_SELECTOR_LEN) == 0) {
                pmkid = pos + 2 + RSN_SELECTOR_LEN;
                //wpa_hexdump(MSG_DEBUG, "RSN: PMKID from "
                //"Authenticator", (char *)pmkid, PMKID_LEN);
//              kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_DUMP_PMK,PMKID_LEN,pmkid);
                break;
            } else if (pos[0] == GENERIC_INFO_ELEM &&  pos[1] == 0) {
                break;
            }

            pos += 2 + pos[1];

        }
    }

    if (wpa_s->assoc_wpa_ie) {
        /* The driver reported a WPA IE that may be different from the
        * one that the Supplicant would use. Message 2/4 has to use
        * the exact copy of the WPA IE from the Association Request,
        * so use the value reported by the driver. */
        wpa_ie = wpa_s->assoc_wpa_ie;
        wpa_ie_len = wpa_s->assoc_wpa_ie_len;
    } else {
        wpa_ie = wpa_ie_buf;
        wpa_ie_len = wpa_gen_wpa_ie(wpa_s, wpa_ie);
        if (wpa_ie_len < 0) {
            wpa_printf(TRACE_ERROR, "WPA: Failed to generate "
                       "WPA IE (for msg 2 of 4).\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_FAIL_GEN_KEY2);
            return;
        }

        //wpa_hexdump(MSG_DEBUG, "WPA: WPA IE for msg 2/4",
        //(char *)wpa_ie, (wpa_size_t) wpa_ie_len);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_DUMP_KEY_MSG2,wpa_ie_len,wpa_ie);
    }

    /* Joseph:
     * Start to compose the l2 packet
     */
    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    kal_mem_cpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    kal_mem_cpy(local_para_ptr->dest_mac_addr, src_addr, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_EAPOL);

    rlen = sizeof(*hdr) + WPA_EAPOL_KEY_SIZE + wpa_ie_len;
    //peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    kal_mem_set(pdu_ptr, 0, pdu_len);

    /* 802.1X Header */
    hdr = (struct ieee802_1x_hdr *)(pdu_ptr);
    hdr->version = wpa_s->conf->eapol_version;
    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
    hdr->length = htons(WPA_EAPOL_KEY_SIZE + wpa_ie_len);

    /* Key negotiation part */
    encode_ptr = (kal_uint8 *)(hdr + 1);
    // Descriptor Type
    *encode_ptr = wpa_s->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    encode_ptr ++;

    // Key information
    *encode_ptr = ((ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC) & 0xFF00) >> 8;
    encode_ptr ++;
    *encode_ptr = (ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC) & 0x00FF;
    encode_ptr ++;
    //*(kal_uint16 *)encode_ptr = htons(ver | WPA_KEY_INFO_KEY_TYPE|WPA_KEY_INFO_MIC);
    //encode_ptr += 2;

    // Key length
    //*(kal_uint16 *)encode_ptr = htons(key->key_length);
    //encode_ptr += 2;
    //*encode_ptr = ((key->key_length) & 0xFF00) >> 8;
    //encode_ptr ++;
    //*encode_ptr = (key->key_length) & 0x00FF;
    //encode_ptr ++;
    *encode_ptr = 0;
    encode_ptr ++;
    *encode_ptr = 0;
    encode_ptr ++;

    // Key replay counter
    kal_mem_cpy(encode_ptr, key->replay_counter,
                WPA_REPLAY_COUNTER_LEN);
    encode_ptr += WPA_REPLAY_COUNTER_LEN;

    // Key nonce
    if (wpa_s->renew_snonce) {
        if (hostapd_get_rand(wpa_s->snonce, WPA_NONCE_LEN)) {
            //wpa_msg(wpa_s, MSG_WARNING, "WPA: Failed to get "
            //"random data for SNonce");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_FAIL_GEN_SNONCE);
            //free_peer_buff(peer_buff_ptr);
            free_peer_buff(peer_buff_ptr);
            //free_local_para((local_para_struct*) local_para_ptr);
            free_local_para((local_para_struct *) local_para_ptr);
            return;
        }

        wpa_s->renew_snonce = 0;
        // wpa_hexdump(MSG_DEBUG, "WPA: Renewed SNonce",
        //(char *)wpa_s->snonce, WPA_NONCE_LEN);
//        kal_buffer_trace(DBG_KEY_HANDSHAKE,   INFO_SUPC_RX_1_4KEY_DUMP_SNONCE,WPA_NONCE_LEN,wpa_s->snonce);
    }

//#ifdef _SUPC_UNITTEST_

    // kal_mem_cpy(&(wpa_s->snonce[0]), &(sample_snonce_wpa3_psk[0]), WPA_NONCE_LEN);

//#endif /* _SUPC_UNITTEST_ */


    kal_mem_cpy(encode_ptr, wpa_s->snonce, WPA_NONCE_LEN);
    encode_ptr += WPA_NONCE_LEN;
    ptk = &wpa_s->tptk;
    kal_mem_cpy(wpa_s->anonce, key->key_nonce, WPA_NONCE_LEN);

    // Key IV (skiped)
    encode_ptr += 16;

    // Key RSC (skiped)
    encode_ptr += 8;

    // Reserved (skiped)
    encode_ptr += 8;

    if (pmkid && !wpa_s->cur_pmksa) {
        /* When using drivers that generate RSN IE, wpa_supplicant may
        * not have enough time to get the association information
        * event before receiving this 1/4 message, so try to find a
        * matching PMKSA cache entry here. */
        wpa_s->cur_pmksa = pmksa_cache_get(wpa_s, src_addr, pmkid);
        if (wpa_s->cur_pmksa) {
            wpa_printf(TRACE_GROUP_1, "RSN: found matching PMKID from "
                       "PMKSA cache\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_FIND_PMKID);
        } else {
            wpa_printf(TRACE_GROUP_1, "RSN: no matching PMKID found\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_NO_PMKID);
            abort_cached = 1;
        }
    }

    if (pmkid && wpa_s->cur_pmksa &&
            kal_mem_cmp(pmkid, wpa_s->cur_pmksa->pmkid, PMKID_LEN) == 0) {
        //wpa_hexdump(MSG_DEBUG, "RSN: matched PMKID", (char *)pmkid, PMKID_LEN);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_RX_1_4KEY_DUMP_MATCHED_PMKID, PMKID_LEN,pmkid);
        kal_mem_cpy(wpa_s->pmk, wpa_s->cur_pmksa->pmk, PMK_LEN);
        //wpa_hexdump_key(MSG_DEBUG, "RSN: PMK from PMKSA cache",
        //(char *)wpa_s->pmk, PMK_LEN);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_RX_1_4KEY_DUMP_PMK_FROM_CACHE,PMK_LEN,wpa_s->pmk);
        eapol_sm_notify_cached(wpa_s->eapol);
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X && wpa_s->eapol) {
        int res, pmk_len;
        u8 sta_pmkid[PMKID_LEN];

        pmk_len = PMK_LEN;
        res = eapol_sm_get_key(wpa_s->eapol, wpa_s->pmk, PMK_LEN);
#ifdef EAP_LEAP
        if (res) {
            res = eapol_sm_get_key(wpa_s->eapol, wpa_s->pmk, 16);
            pmk_len = 16;
        }
#endif /* EAP_LEAP */
        if (res == 0) {
            //wpa_hexdump_key(MSG_MSGDUMP, "WPA: PMK from EAPOL state "
            //"machines", (char *)wpa_s->pmk, (wpa_size_t)pmk_len);
//          kal_buffer_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_RX_1_4KEY_DUMP_PMK_FROM_EAPOL,pmk_len,wpa_s->pmk);
            wpa_s->pmk_len = pmk_len;

#if 0
            pmksa_cache_add(wpa_s->pmk, (wpa_size_t)pmk_len, src_addr,
                            wpa_s->own_addr);
            if (!wpa_s->cur_pmksa && pmkid &&
                    pmksa_cache_get(wpa_s, src_addr, pmkid)) {
                wpa_printf(MSG_DEBUG, "RSN: the new PMK "
                           "matches with the PMKID");
                abort_cached = 0;
            }
#endif
            rsn_pmkid(wpa_s->pmk, src_addr, wpa_s->own_addr, sta_pmkid);
            if (!wpa_s->cur_pmksa && pmkid &&
                    kal_mem_cmp(pmkid, sta_pmkid, PMKID_LEN) == 0) {
                wpa_printf(TRACE_GROUP_1, "RSN: the new PMK "
                           "matches with the PMKID\n");
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_NEW_PMK_MATCH);
                abort_cached = 0;
            }

        } else {
            // wpa_msg(wpa_s, MSG_DBG_AUTH,
            //"WPA: Failed to get master session key from "
            // "EAPOL state machines");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_FAIL_GET_MSK);
            //wpa_msg(wpa_s, MSG_DBG_AUTH,
            // "WPA: Key handshake aborted");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_HANDSHAKE_ABORT);

            if (wpa_s->cur_pmksa) {
                wpa_printf(TRACE_GROUP_1, "RSN: Cancelled PMKSA "
                           "caching attempt\n");
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_CANCELL_PMKSA_ATTEMPT);
                wpa_s->cur_pmksa = NULL;
                abort_cached = 1;
            } else { // Karen: can't we just abort the cache and redo 802.1x authentication?
                //free_peer_buff(peer_buff_ptr);
                free_peer_buff(peer_buff_ptr);
                //free_local_para((local_para_struct*) local_para_ptr);
                free_local_para((local_para_struct *) local_para_ptr);
                return;
            }
        }
#ifdef CONFIG_XSUPPLICANT_IFACE
    } else if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X &&
               !wpa_s->ext_pmk_received) {
        //wpa_printf(MSG_INFO, "WPA: Master session has not yet "
        //"been received from the external IEEE "
        //"802.1X Supplicant - ignoring WPA "
        //"EAPOL-Key frame");
        //free_peer_buff(peer_buff_ptr);
        free_peer_buff(peer_buff_ptr);
        //free_local_para((local_para_struct*) local_para_ptr);
        free_local_para((local_para_struct *) local_para_ptr);
        return;
#endif /* CONFIG_XSUPPLICANT_IFACE */
    }


    /* We tried to get a catched information in the previous codes. Now we are going
     * to try full EAP authentication procedure
     */
    if (abort_cached && wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        /* Send EAPOL-Start to trigger full EAP authentication. */
        wpa_printf(TRACE_GROUP_1, "RSN: no PMKSA entry found - trigger "
                   "full EAP authenication\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_NOPMKSA_TRIGGER_EAP);
        wpa_eapol_send(wpa_s, IEEE802_1X_TYPE_EAPOL_START,
                       (u8 *) "", 0);
        //free_peer_buff(peer_buff_ptr);
        free_peer_buff(peer_buff_ptr);
        //free_local_para((local_para_struct*) local_para_ptr);
        free_local_para((local_para_struct *) local_para_ptr);
        return;
    }

    wpa_pmk_to_ptk(wpa_s->pmk, wpa_s->pmk_len, wpa_s->own_addr, src_addr,
                   wpa_s->snonce, key->key_nonce,
                   (u8 *) ptk, sizeof(*ptk));

    /* Supplicant: swap tx/rx Mic keys */
    kal_mem_cpy(buf, ptk->u.auth.tx_mic_key, 8);
    kal_mem_cpy(ptk->u.auth.tx_mic_key, ptk->u.auth.rx_mic_key, 8);
    kal_mem_cpy(ptk->u.auth.rx_mic_key, buf, 8);
    wpa_s->tptk_set = 1;

    // Key MIC as 0
    mic_ptr = encode_ptr;
    encode_ptr += 16;



    // Key data
    //*(kal_uint16 *)encode_ptr = htons(wpa_ie_len);
    //encode_ptr += 2;
    *encode_ptr = (wpa_ie_len & 0xFF00) >> 8;
    encode_ptr ++;
    *encode_ptr = wpa_ie_len & 0x00FF;
    encode_ptr ++;

    // Key data
    memcpy(encode_ptr, wpa_ie, wpa_ie_len);

    // J: MIC shall include key data
    // Key MIC
    wpa_eapol_key_mic(wpa_s->tptk.mic_key, ver, (u8 *) hdr,
                      rlen , mic_ptr);
    //wpa_hexdump(MSG_DEBUG, "WPA: EAPOL-Key MIC", (char *)mic_ptr, 16);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_DUMP_KEY_MIC,16,mic_ptr);
    //encode_ptr += 16;


    wpa_printf(TRACE_GROUP_1, "WPA: Sending EAPOL-Key 2/4\n");
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_4KEY_SEND_KEY2_MSG);
    //wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key 2/4", (char *)peer_buff_ptr, rlen);

    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);
    //l2_packet_send(wpa_s->l2, rbuf, rlen);
    eapol_sm_notify_tx_eapol_key(wpa_s->eapol);

    wpa_supplicant_req_auth_timeout(wpa_s, 3, 0);

    //free(rbuf);
}


static void wpa_supplicant_key_neg_complete(struct wpa_supplicant *wpa_s,
        u8 *addr, int secure)
{

    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;
    supc_abm_wifi_connect_ind_struct *ind_local_para_ptr = NULL;
    wndrv_supc_auth_state_update_req_struct *req_local_para_ptr = NULL;
    int res, pmk_len;

    //wpa_printf(MSG_INFO, "WPA: Key negotiation completed with "
    //MACSTR " [PTK=%s GTK=%s]", MAC2STR(addr),
    //wpa_cipher_txt(wpa_s->pairwise_cipher),
    //wpa_cipher_txt(wpa_s->group_cipher));
//  eloop_cancel_timeout(wpa_supplicant_scan, wpa_s, NULL);
    wpa_printf(TRACE_GROUP_1, "Cancel handover timer after 4-way and 2-way complete!!\n");
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_KEY_NEG_DONE_CANCEL_HANDOVER_TIMER);

    wpa_supplicant_cancel_auth_timeout(wpa_s);
    supc_stop_handover_timer();
#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer
#endif
    wpa_s->wpa_state = WPA_COMPLETED;

    // J
    /*
    cnf_local_para_ptr = construct_local_para(sizeof(supc_abm_wifi_connect_cnf_struct), TD_UL);
    cnf_local_para_ptr->result = KAL_TRUE;
    cnf_local_para_ptr->cause = SUPC_ABM_INVALID_CAUSE;
    kal_mem_cpy(&(cnf_local_para_ptr->bssid[0]), &(wpa_s->bssid[0]), WNDRV_MAC_ADDRESS_LEN);
    kal_mem_cpy(&(cnf_local_para_ptr->ssid[0]), &(wpa_s->ssid[0]), wpa_s->ssid_len);
    cnf_local_para_ptr->ssid_len = wpa_s->ssid_len;

    wpa_s->wpa_scan_retry_counter = 0xff;
    supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);
    */
    //supc_send_abm_wifi_connect_cnf(KAL_TRUE, SUPC_ABM_INVALID_CAUSE);

    pmk_len = PMK_LEN;
    res = eapol_sm_get_key(wpa_s->eapol, wpa_s->pmk, PMK_LEN);
#ifdef EAP_LEAP
    if (res) {
        res = eapol_sm_get_key(wpa_s->eapol, wpa_s->pmk, 16);
        pmk_len = 16;
    }
#endif /* EAP_LEAP */
    if (res == 0) {
        wpa_s->pmk_len = pmk_len;
        pmksa_cache_add(wpa_s->pmk, (wpa_size_t)pmk_len, addr,
                        wpa_s->own_addr);
    }

    //req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
    req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_RESET);
    req_local_para_ptr->in_progress = KAL_FALSE;
    supc_send_wndrv_auth_state_update_req((local_para_struct *) req_local_para_ptr);

    /*in WSC exchange phase, this flag is set to false, but
    confirm is discarded in supc_send_abm_wifi_connect*/
#ifdef __WPS_SUPPORT__
    if (1 ==  wpa_s->wps_success ||
            KAL_TRUE == wpa_s->is_wpa_connect_req_pending)
#else
    if (KAL_TRUE == wpa_s->is_wpa_connect_req_pending)
#endif
    {
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

#ifdef __WPS_SUPPORT__
        cnf_local_para_ptr->auth_type = supc_get_auth_mode(wpa_s->key_mgmt,
                                        wpa_s->proto, wpa_s->current_ssid->auth_alg);
        cnf_local_para_ptr->eap_peap_auth_type = 0;
        cnf_local_para_ptr->eap_ttls_auth_type = 0;
        cnf_local_para_ptr->encry_mode = supc_get_encrypt_mode(wpa_s->current_ssid->pairwise_cipher);
        cnf_local_para_ptr->passphase_len = 0;
#if 0
        if (wpa_s->current_ssid->passphrase) {
            kal_uint32 passlen = 0;
            passlen =  strlen(wpa_s->current_ssid->passphrase);
            wpa_printf(MSG_DEBUG, "WPS: store passphase len=%d", passlen);
            if (passlen < 30) {
                kal_mem_cpy(cnf_local_para_ptr->passphase,
                            wpa_s->current_ssid->passphrase, passlen);
                cnf_local_para_ptr->passphase_len = passlen;
            } else {
                wpa_printf(MSG_DEBUG, "WPS: passphase exceed 64 bytes");
            }
        }
#else
        if (KAL_TRUE == wpa_s->current_ssid->psk_set) {
            cnf_local_para_ptr->passphase_len = 32;
            kal_mem_cpy(cnf_local_para_ptr->passphase,
                        wpa_s->current_ssid->psk, 32);
        }
#endif
#else
        //add for WiFi UE enhancement
        cnf_local_para_ptr->auth_type  = (supc_auth_mode_enum)0;
        cnf_local_para_ptr->eap_peap_auth_type = 0;
        cnf_local_para_ptr->eap_ttls_auth_type = 0;

        /*clear wps data*/

        cnf_local_para_ptr->passphase_len = 0;
        cnf_local_para_ptr->conn_type = wpa_s->connection_type;
        cnf_local_para_ptr->encry_mode = SUPC_ENCRYPT_MODE_NONE;
#endif
        wpa_s->wpa_scan_retry_counter = 0xff;
        supc_stop_report_mmi_timer();
        supc_send_abm_wifi_connect_cnf((local_para_struct *) cnf_local_para_ptr);
    } else { // if (KAL_FALSE == wpa_s->is_wpa_connect_req_pending)
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
    }

    kal_mem_cpy(wpa_s->prev_bssid, wpa_s->bssid, ETH_ALEN);
    wpa_s->num_of_disconnect = 0;
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    wpa_listed_add_success(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id); //Kinki add for MAUI_00401655
#endif
    if (secure) {
        /* MLME.SETPROTECTION.request(TA, Tx_Rx) */
        eapol_sm_notify_portValid(wpa_s->eapol, TRUE);
        if (wpa_s->key_mgmt == WPA_KEY_MGMT_PSK) {
            eapol_sm_notify_eap_success(wpa_s->eapol, TRUE);
        }
        rsn_preauth_candidate_process(wpa_s);
    }
}

#ifdef __WAPI_SUPPORT__
/* invoked by WAPI lib to indicate success of handshaking */
void wpa_supplicant_wapi_key_neg_complete(struct wpa_supplicant *wpa_s,
        u8 *addr, int secure)
{

    supc_abm_wifi_connect_cnf_struct *cnf_local_para_ptr = NULL;
    supc_abm_wifi_connect_ind_struct *ind_local_para_ptr = NULL;
    wndrv_supc_auth_state_update_req_struct *req_local_para_ptr = NULL;

    //wpa_printf(MSG_INFO, "WPA: WAPI Key negotiation completed with "
    //MACSTR " [PTK=%s GTK=%s]", MAC2STR(addr),
    //wpa_cipher_txt(wpa_s->pairwise_cipher),
    //wpa_cipher_txt(wpa_s->group_cipher));
    kal_trace(DBG_WAPI, INFO_SUPC_WAPI_KEY_NEG_COMPLETE, MAC2STR(addr));

    //wpa_printf(MSG_DEBUG,"Cancel handover timer after 4-way and 2-way complete!!");
    kal_trace(DBG_WAPI, INFO_SUPC_CANCEL_HANDOVER_TIMER);
    wpa_supplicant_cancel_wapi_auth_timeout(wpa_s);
    supc_stop_handover_timer();

    wpa_s->wpa_state = WPA_COMPLETED;

    //req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_UL);
    req_local_para_ptr = construct_local_para(sizeof(wndrv_supc_auth_state_update_req_struct), TD_RESET);
    req_local_para_ptr->in_progress = KAL_FALSE;
    supc_send_wndrv_auth_state_update_req((local_para_struct *) req_local_para_ptr);

    //wpa_printf(MSG_DEBUG,"is_wpa_connect_req_pending %d "MACSTR" "MACSTR"\n",
    // wpa_s->is_wpa_connect_req_pending, MAC2STR(wpa_s->prev_bssid), MAC2STR(wpa_s->bssid));
    kal_trace(DBG_WAPI, INFO_SUPC_CONN_REQ_PENDING, wpa_s->is_wpa_connect_req_pending, MAC2STR(wpa_s->prev_bssid), MAC2STR(wpa_s->bssid));
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
#ifdef WIFI_BLACKLIST // Kinki change for the blacklist
    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
    wpa_listed_add_success(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id); //Kinki add for MAUI_00401655
#endif
}
#endif
static int wpa_supplicant_install_ptk(struct wpa_supplicant *wpa_s,
                                      unsigned char *src_addr,
                                      struct wpa_eapol_key *key)
{
    int keylen, rsclen;
    wpa_alg alg;
    u8 *key_rsc;
    u8 null_rsc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    wpa_printf(TRACE_GROUP_1, "WPA: Installing PTK to the driver.\n");
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INSTALL_PTK2DRV);

    switch (wpa_s->pairwise_cipher) {
        case WPA_CIPHER_CCMP:
            alg = WPA_ALG_CCMP;
            keylen = 16;
            rsclen = 6;
            break;

        case WPA_CIPHER_TKIP:
            alg = WPA_ALG_TKIP;
            keylen = 32;
            rsclen = 6;
            break;

        case WPA_CIPHER_NONE:
            wpa_printf(TRACE_GROUP_1, "WPA: Pairwise Cipher Suite: "
                       "NONE - do not use pairwise keys\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INSTALL_KEY_NONE);
            return 0;

        default:
            wpa_printf(TRACE_ERROR, "WPA: Unsupported pairwise cipher %d\n",
                       wpa_s->pairwise_cipher);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_P_CIPHER,wpa_s->pairwise_cipher);
            return -1;
    }

    if (wpa_s->proto == WPA_PROTO_RSN) {
        key_rsc = null_rsc;
    } else {
        key_rsc = key->key_rsc;
        //wpa_hexdump(MSG_DEBUG, "WPA: RSC", (char *)key_rsc, (wpa_size_t)rsclen);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_RSC_WHEN_INSTALL_KEY,rsclen,key_rsc);
    }

    wpa_s->keys_cleared = 0;
    if (wpa_driver_mtk_set_key(wpa_s->drv_priv, alg, src_addr, 0, 1, key_rsc, rsclen,
                               (u8 *) &wpa_s->ptk.tk1, keylen) < 0) {
        wpa_printf(TRACE_WARNING, "WPA: Failed to set PTK to the "
                   "driver.\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_SET_PTK_FAIL);
        return -1;
    }
    return 0;
}


static int wpa_supplicant_check_group_cipher(struct wpa_supplicant *wpa_s,
        int keylen, int maxkeylen,
        int *key_rsc_len, int *alg)
{
    switch (wpa_s->group_cipher) {
        case WPA_CIPHER_CCMP:
            if (keylen != 16 || maxkeylen < 16) {
                wpa_printf(TRACE_WARNING, "WPA: Unsupported CCMP Group "
                           "Cipher key length %d (%d).\n",
                           keylen, maxkeylen);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_CCMP_LEN, keylen, maxkeylen);
                return -1;
            }
            *key_rsc_len = 6;
            *alg = WPA_ALG_CCMP;
            break;

        case WPA_CIPHER_TKIP:
            if (keylen != 32 || maxkeylen < 32) {
                wpa_printf(MSG_WARNING, "WPA: Unsupported TKIP Group "
                           "Cipher key length %d (%d).\n",
                           keylen, maxkeylen);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_TKIP_LEN, keylen, maxkeylen);
                return -1;
            }
            *key_rsc_len = 6;
            *alg = WPA_ALG_TKIP;
            break;

        case WPA_CIPHER_WEP104:
            if (keylen != 13 || maxkeylen < 13) {
                wpa_printf(TRACE_WARNING, "WPA: Unsupported WEP104 Group"
                           " Cipher key length %d (%d).\n",
                           keylen, maxkeylen);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_WEP104_LEN, keylen, maxkeylen);
                return -1;
            }
            *key_rsc_len = 0;
            *alg = WPA_ALG_WEP;
            break;

        case WPA_CIPHER_WEP40:
            if (keylen != 5 || maxkeylen < 5) {
                wpa_printf(TRACE_WARNING, "WPA: Unsupported WEP40 Group "
                           "Cipher key length %d (%d).\n",
                           keylen, maxkeylen);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_WEP40_LEN, keylen, maxkeylen);
                return -1;
            }
            *key_rsc_len = 0;
            *alg = WPA_ALG_WEP;
            break;

        default:
            wpa_printf(TRACE_WARNING, "WPA: Unsupport Group Cipher %d\n",
                       wpa_s->group_cipher);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_GROUP_CIPHER, wpa_s->group_cipher);
            return -1;
    }

    return 0;
}


static int wpa_supplicant_install_gtk(struct wpa_supplicant *wpa_s,
                                      struct wpa_eapol_key *key, int alg,
                                      u8 *gtk, int gtk_len, int keyidx,
                                      int key_rsc_len, int tx)
{
    //wpa_hexdump_key(MSG_DEBUG, "WPA: Group Key", (char *)gtk, (wpa_size_t)gtk_len);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INSTALL_GTK_DUMP_GTK,gtk_len,gtk);
    wpa_printf(TRACE_GROUP_1, "WPA: Installing GTK to the driver "
               "(keyidx=%d tx=%d).\n", keyidx, tx);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INSTALL_GTK2DRV, keyidx, tx);
    //wpa_hexdump(MSG_DEBUG, "WPA: RSC", (char *)key->key_rsc, (wpa_size_t)key_rsc_len);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_RSC_WHEN_INSTALL_KEY,key_rsc_len,key->key_rsc);

    if (wpa_s->group_cipher == WPA_CIPHER_TKIP) {
        /* Swap Tx/Rx keys for Michael MIC */
        u8 tmpbuf[8];
        memcpy(tmpbuf, gtk + 16, 8);
        memcpy(gtk + 16, gtk + 24, 8);
        memcpy(gtk + 24, tmpbuf, 8);
    }
    wpa_s->keys_cleared = 0;

    if (wpa_s->pairwise_cipher == WPA_CIPHER_NONE) {
        if (wpa_driver_mtk_set_key(wpa_s->drv_priv, (wpa_alg)alg,
                                   (u8 *) "\xff\xff\xff\xff\xff\xff",
                                   keyidx, 1, key->key_rsc, key_rsc_len,
                                   gtk, gtk_len) < 0) {
            wpa_printf(TRACE_WARNING, "WPA: Failed to set "
                       "GTK to the driver (Group only).\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INSTALL_GTK2DRV_FAIL_GROUP_ONLY);
            return -1;
        }
    } else if (wpa_driver_mtk_set_key(wpa_s->drv_priv, (wpa_alg) alg,
                                      (u8 *) "\xff\xff\xff\xff\xff\xff",
                                      keyidx, tx, key->key_rsc, key_rsc_len,
                                      gtk, gtk_len) < 0) {
        wpa_printf(TRACE_WARNING, "WPA: Failed to set GTK to "
                   "the driver.\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INSTALL_GTK2DRV_FAIL);
        return -1;
    }

    return 0;
}


static int wpa_supplicant_pairwise_gtk(struct wpa_supplicant *wpa_s,
                                       unsigned char *src_addr,
                                       struct wpa_eapol_key *key,
                                       u8 *gtk, int gtk_len, int key_info)
{
    int keyidx, tx, key_rsc_len = 0, alg;

    //wpa_hexdump_key(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
    //(char *)gtk, (wpa_size_t)gtk_len);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_GTK_WHEN_PAIRWISE, gtk_len, gtk);

    keyidx = gtk[0] & 0x3;
    tx = !!(gtk[0] & BIT(2));

    if (tx && wpa_s->pairwise_cipher != WPA_CIPHER_NONE) {
        /* Ignore Tx bit in GTK IE if a pairwise key is used. One AP
         * seemed to set this bit (incorrectly, since Tx is only when
         * doing Group Key only APs) and without this workaround, the
         * data connection does not work because wpa_supplicant
         * configured non-zero keyidx to be used for unicast. */
        wpa_printf(TRACE_GROUP_1, "RSN: Tx bit set for GTK IE, but "
                   "pairwise keys are used - ignore Tx bit\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_PAIRWISE_IGNORE_TX_BIT);
        tx = 0;
    }
    gtk += 2;
    gtk_len -= 2;

    if (wpa_supplicant_check_group_cipher(wpa_s, gtk_len, gtk_len,
                                          &key_rsc_len, &alg)) {
        return -1;
    }

    if (wpa_supplicant_install_gtk(wpa_s, key, alg, gtk, gtk_len, keyidx,
                                   key_rsc_len, tx)) {
        return -1;
    }

    wpa_supplicant_key_neg_complete(wpa_s, src_addr,
                                    key_info & WPA_KEY_INFO_SECURE);
    return 0;
}


static void wpa_report_ie_mismatch(struct wpa_supplicant *wpa_s,
                                   const char *reason, const u8 *src_addr,
                                   const u8 *wpa_ie, wpa_size_t wpa_ie_len,
                                   const u8 *rsn_ie, wpa_size_t rsn_ie_len)
{
    //wpa_msg(wpa_s, MSG_WARNING, "WPA: %s (src=" MACSTR ")",
    //reason, MAC2STR(src_addr));
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_IE_MISATCH_BSSID, MAC2STR(src_addr));

//  kal_wap_trace(MAP_MOD_SUPC,DBG_CONN,"%s",reason);

    if (wpa_s->ap_wpa_ie) {
        //wpa_hexdump(MSG_INFO, "WPA: WPA IE in Beacon/ProbeResp",
        //(char *)wpa_s->ap_wpa_ie, wpa_s->ap_wpa_ie_len);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_WPA_IE_MISMATCH_IN_KEY, wpa_s->ap_wpa_ie_len, wpa_s->ap_wpa_ie);
    }
    if (wpa_ie) {
        if (!wpa_s->ap_wpa_ie) {
            wpa_printf(TRACE_GROUP_1, "WPA: No WPA IE in "
                       "Beacon/ProbeResp\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_NO_WPAIE_IN_BEACON_IN_KEY_FRAME);
        }
        //wpa_hexdump(MSG_INFO, "WPA: WPA IE in 3/4 msg",
        //(char *)wpa_ie, wpa_ie_len);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_WPA_IE_IN3_4_MSG,wpa_ie_len,wpa_ie);
    }

    if (wpa_s->ap_rsn_ie) {
        //wpa_hexdump(MSG_INFO, "WPA: RSN IE in Beacon/ProbeResp",
        //(char *)wpa_s->ap_rsn_ie, wpa_s->ap_rsn_ie_len);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_RSN_IE_BEACON_IN_KEY_FRAME,wpa_s->ap_rsn_ie_len,wpa_s->ap_rsn_ie);
    }
    if (rsn_ie) {
        if (!wpa_s->ap_rsn_ie) {
            wpa_printf(TRACE_GROUP_1, "WPA: No RSN IE in "
                       "Beacon/ProbeResp\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_NO_RSNIE_IN_BEACON_IN_KEY_FRAME);
        }
        //wpa_hexdump(MSG_INFO, "WPA: RSN IE in 3/4 msg",
        //(char *)rsn_ie, rsn_ie_len);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_RSN_IE_IN3_4_MSG,rsn_ie_len,rsn_ie);
    }

    wpa_supplicant_disassociate(wpa_s, REASON_IE_IN_4WAY_DIFFERS);
    wpa_s->wpa_state = WPA_SCANNING;    // Karen
    wpa_supplicant_req_scan(wpa_s, 0, 0);
}


static void wpa_supplicant_process_3_of_4(struct wpa_supplicant *wpa_s,
        unsigned char *src_addr,
        struct wpa_eapol_key *key,
        int extra_len, int ver, u8 *key_data_ptr)
{
    kal_uint16 rlen;
    struct ieee802_1x_hdr *hdr;
    // struct wpa_eapol_key *reply;
    // unsigned char *rbuf;
    // struct l2_ethhdr *ethhdr;
    int key_info, wpa_ie_len = 0, rsn_ie_len = 0, keylen, gtk_len = 0;
    u8 *wpa_ie = NULL, *rsn_ie = NULL, *gtk = NULL;
    u8 *pos, *end;
    u16 len;
    struct wpa_ssid *ssid = wpa_s->current_ssid;
    kal_uint8 *pdu_ptr = NULL;
    kal_uint8 *encode_ptr = NULL;
    // kal_uint8 *mic_ptr = NULL;
    kal_uint16 pdu_len = 0;
    kal_uint16 key_info_flag = 0;

    wndrv_supc_data_req_struct      *local_para_ptr = NULL;
    peer_buff_struct                    *peer_buff_ptr = NULL;

    wpa_s->wpa_state = WPA_4WAY_HANDSHAKE;
    wpa_printf(TRACE_GROUP_1, "WPA: RX message 3 of 4-Way Handshake from "
               MACSTR " (ver=%d)\n", MAC2STR(src_addr), ver);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_MSG,MAC2STR(src_addr), ver);

    key_info = (key->key_info);

    pos = (u8 *)(key_data_ptr);
    len = (key->key_data_length);
    end = pos + len;
    //wpa_hexdump(MSG_DEBUG, "WPA: IE KeyData", (char *)pos, len);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_DUMP_KEY,len,pos);

    while (pos + 1 < end) {
        if (pos + 2 + pos[1] > end) {
            wpa_printf(TRACE_GROUP_1, "WPA: key data underflow (ie=%d "
                       "len=%d)\n", pos[0], pos[1]);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_UNDERFLOW, pos[0], pos[1]);
            break;
        }

        if (*pos == RSN_INFO_ELEM) {
            rsn_ie = pos;
            rsn_ie_len = pos[1] + 2;
        } else if (*pos == GENERIC_INFO_ELEM && pos[1] >= 6 &&
                   memcmp(pos + 2, WPA_OUI_TYPE, WPA_SELECTOR_LEN) == 0
                   && pos[2 + WPA_SELECTOR_LEN] == 1 &&
                   pos[2 + WPA_SELECTOR_LEN + 1] == 0) {
            wpa_ie = pos;
            wpa_ie_len = pos[1] + 2;
        } else if (pos[0] == GENERIC_INFO_ELEM &&
                   pos[1] > RSN_SELECTOR_LEN + 2 &&
                   memcmp(pos + 2, RSN_KEY_DATA_GROUPKEY,
                          RSN_SELECTOR_LEN) == 0) {
            if (!(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
                wpa_printf(TRACE_WARNING, "WPA: GTK IE in "
                           "unencrypted key data\n");
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_GTK_UNENCRYPTED);
                return;
            }

            gtk = pos + 2 + RSN_SELECTOR_LEN;
            gtk_len = pos[1] - RSN_SELECTOR_LEN;
        } else if (pos[0] == GENERIC_INFO_ELEM && pos[1] == 0) {
            break;
        }

        pos += 2 + pos[1];

    }

    if (wpa_s->ap_wpa_ie == NULL && wpa_s->ap_rsn_ie == NULL) {
        //wpa_printf(MSG_DEBUG, "WPA: No WPA/RSN IE for this AP known. "
        //"Trying to get from scan results");
        /* Joseph:
         * We are not going to re-get beacon at this time, just let it fail
         */
        /*
        if (wpa_supplicant_get_beacon_ie(wpa_s) < 0)
        {
            wpa_printf(MSG_WARNING, "WPA: Could not find AP from "
                "the scan results");
        }
        else
        */
        {
            wpa_printf(TRACE_GROUP_1, "WPA: Found the current AP from "
                       "updated scan results\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_NULL_IE_UPDATE_SCAN);
        }
    }


    if ((wpa_ie && wpa_s->ap_wpa_ie &&
            (wpa_ie_len != wpa_s->ap_wpa_ie_len ||
             memcmp(wpa_ie, wpa_s->ap_wpa_ie, wpa_ie_len) != 0)) ||
            (rsn_ie && wpa_s->ap_rsn_ie &&
             (rsn_ie_len != wpa_s->ap_rsn_ie_len ||
              memcmp(rsn_ie, wpa_s->ap_rsn_ie, rsn_ie_len) != 0))) {
        wpa_report_ie_mismatch(wpa_s, "IE in 3/4 msg does not match "
                               "with IE in Beacon/ProbeResp",
                               src_addr, wpa_ie, (wpa_size_t)wpa_ie_len,
                               rsn_ie, (wpa_size_t)rsn_ie_len);
        return;
    }

    if (wpa_s->proto == WPA_PROTO_WPA &&
            rsn_ie && wpa_s->ap_rsn_ie == NULL &&
            ssid && (ssid->proto & WPA_PROTO_RSN)) {
        wpa_report_ie_mismatch(wpa_s, "Possible downgrade attack "
                               "detected - RSN was enabled and RSN IE "
                               "was in msg 3/4, but not in "
                               "Beacon/ProbeResp",
                               src_addr, wpa_ie, (wpa_size_t)wpa_ie_len,
                               rsn_ie, (wpa_size_t)rsn_ie_len);
        return;
    }

    if (memcmp(wpa_s->anonce, key->key_nonce, WPA_NONCE_LEN) != 0) {
        wpa_printf(TRACE_WARNING, "WPA: ANonce from message 1 of 4-Way "
                   "Handshake differs from 3 of 4-Way Handshake - drop"
                   " packet (src=" MACSTR ")\n", MAC2STR(src_addr));
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_ANONCE_ERROR, MAC2STR(src_addr));
        return;
    }

    keylen = (key->key_length);

    switch (wpa_s->pairwise_cipher) {
        case WPA_CIPHER_CCMP:
            if (keylen != 16) {
                wpa_printf(TRACE_WARNING, "WPA: Invalid CCMP key length "
                           "%d (src=" MACSTR ")\n",
                           keylen, MAC2STR(src_addr));
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_INVALID_CCMP_LEN, keylen, MAC2STR(src_addr));
                return;
            }
            break;

        case WPA_CIPHER_TKIP:
            if (keylen != 32) {
                wpa_printf(TRACE_WARNING, "WPA: Invalid TKIP key length "
                           "%d (src=" MACSTR ")\n",
                           keylen, MAC2STR(src_addr));
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_3_4KEY_INVALID_TKIP_LEN, keylen, MAC2STR(src_addr));
                return;
            }
            break;
    }

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    // memset(local_para_ptr, 0, sizeof(wndrv_supc_data_req_struct));   // Karen

    memcpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    memcpy(local_para_ptr->dest_mac_addr, src_addr, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_EAPOL);

    rlen = sizeof(*hdr) + WPA_EAPOL_KEY_SIZE;
    //peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    kal_mem_set(pdu_ptr, 0, pdu_len);
    // use below flag to nofiy driver, this is the 4th 4way-handshake,to work around
    //slow cpu which will cause wndrv lost 5th gtk notification
    *(pdu_ptr - 1) = 0x34;

    /* 802.1X Header */
    hdr = (struct ieee802_1x_hdr *)(pdu_ptr);
    hdr->version = wpa_s->conf->eapol_version;
    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
    hdr->length = htons(WPA_EAPOL_KEY_SIZE);

    /* Key negotiation part */
    encode_ptr = (kal_uint8 *)(hdr + 1);
    // Descriptor Type
    *encode_ptr = wpa_s->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    encode_ptr ++;

    // Key information
    //*(kal_uint16 *)encode_ptr = htons(ver | WPA_KEY_INFO_KEY_TYPE|WPA_KEY_INFO_MIC|WPA_KEY_INFO_SECURE);
    //encode_ptr += 2;
    if (wpa_s->proto == WPA_PROTO_WPA) {
        key_info_flag = ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
    } else {
        key_info_flag = ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    }

    *encode_ptr = (key_info_flag & 0xFF00) >> 8;
    encode_ptr ++;
    *encode_ptr = key_info_flag & 0x00FF;
    encode_ptr ++;

    // Key length
    //*(kal_uint16 *)encode_ptr = htons(key->key_length);
    //encode_ptr += 2;
    *encode_ptr = ((key->key_length) & 0xFF00) >> 8;
    encode_ptr ++;
    *encode_ptr = (key->key_length) & 0x00FF;
    encode_ptr ++;

    // Key replay counter
    kal_mem_cpy(encode_ptr, key->replay_counter,
                WPA_REPLAY_COUNTER_LEN);
    encode_ptr += WPA_REPLAY_COUNTER_LEN;

    kal_mem_cpy(encode_ptr, wpa_s->snonce, WPA_NONCE_LEN);
    encode_ptr += WPA_NONCE_LEN;

    // Key IV (skiped)
    encode_ptr += 16;

    // Key RSC (skiped)
    encode_ptr += 8;

    // Reserved (skiped)
    encode_ptr += 8;

    // Key MIC
    wpa_eapol_key_mic(wpa_s->tptk.mic_key, ver, (u8 *) hdr,
                      rlen , encode_ptr);

    wpa_printf(TRACE_GROUP_1, "WPA: Sending EAPOL-Key 4/4\n");
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_TX_4_4KEY);
    //wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key 4/4", (char *)peer_buff_ptr, rlen);

    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);
    //l2_packet_send(wpa_s->l2, rbuf, rlen);
    eapol_sm_notify_tx_eapol_key(wpa_s->eapol);
    //free(rbuf);

    /* SNonce was successfully used in msg 3/4, so mark it to be renewed
     * for the next 4-Way Handshake. If msg 3 is received again, the old
     * SNonce will still be used to avoid changing PTK. */
    wpa_s->renew_snonce = 1;

    if (key_info & WPA_KEY_INFO_INSTALL) {
        wpa_supplicant_install_ptk(wpa_s, src_addr, key);
    }

    if (key_info & WPA_KEY_INFO_SECURE) {
        /* MLME.SETPROTECTION.request(TA, Tx_Rx) */
        eapol_sm_notify_portValid(wpa_s->eapol, TRUE);
    }
    wpa_s->wpa_state = WPA_GROUP_HANDSHAKE;

    if (gtk) {
        wpa_supplicant_pairwise_gtk(wpa_s, src_addr, key,
                                    gtk, gtk_len, key_info);
    }
}


static void wpa_supplicant_process_1_of_2(struct wpa_supplicant *wpa_s,
        unsigned char *src_addr,
        struct wpa_eapol_key *key,
        int extra_len, int ver, u8 *key_data_ptr)
{
    kal_uint16 rlen;
    struct ieee802_1x_hdr *hdr;
    // struct wpa_eapol_key *reply;
    //unsigned char *rbuf;
    // struct l2_ethhdr *ethhdr;
    int key_info, keylen, keydatalen, maxkeylen, keyidx, key_rsc_len = 0;
    int alg, tx, rekey;
    u8 ek[32], gtk[32];
    u8 *gtk_ie = NULL;
    wpa_size_t gtk_ie_len = 0;
    wndrv_supc_data_req_struct    *local_para_ptr = NULL;
    peer_buff_struct *peer_buff_ptr = NULL;
    kal_uint8 *pdu_ptr = NULL;
    kal_uint8 *encode_ptr = NULL;
    // kal_uint8 *mic_ptr = NULL;
    kal_uint16 pdu_len = 0;

    rekey = (wpa_s->wpa_state == WPA_COMPLETED);
    wpa_s->wpa_state = WPA_GROUP_HANDSHAKE;
    wpa_printf(TRACE_GROUP_1, "WPA: RX message 1 of Group Key Handshake from "
               MACSTR " (ver=%d)\n", MAC2STR(src_addr), ver);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY, MAC2STR(src_addr), ver);

    key_info = (key->key_info);
    keydatalen = (key->key_data_length);

    if (wpa_s->proto == WPA_PROTO_RSN) {
        // u8 *pos = (u8 *) (key + 1);
        u8 *pos = (u8 *)(key_data_ptr);
        u8 *end = pos + keydatalen;
        while (pos + 1 < end) {
            if (pos + 2 + pos[1] > end) {
                wpa_printf(TRACE_GROUP_1, "RSN: key data "
                           "underflow (ie=%d len=%d)\n",
                           pos[0], pos[1]);
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_UNDERFLOW,pos[0], pos[1]);
                break;
            }

            if (pos[0] == GENERIC_INFO_ELEM &&
                    pos + 1 + RSN_SELECTOR_LEN < end &&
                    pos[1] > RSN_SELECTOR_LEN + 2 &&
                    memcmp(pos + 2, RSN_KEY_DATA_GROUPKEY,
                           RSN_SELECTOR_LEN) == 0) {
                if (!(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
                    wpa_printf(TRACE_WARNING, "WPA: GTK IE "
                               "in unencrypted key data\n");
//                  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_GTK_UNENCRYPTED);
                    return;
                }

                gtk_ie = pos + 2 + RSN_SELECTOR_LEN;
                gtk_ie_len = pos[1] - RSN_SELECTOR_LEN;
                break;
            } else if (pos[0] == GENERIC_INFO_ELEM && pos[1] == 0) {
                break;
            }

            pos += 2 + pos[1];
        }

        if (gtk_ie == NULL) {
            wpa_printf(TRACE_GROUP_1, "WPA: No GTK IE in Group Key "
                       "message 1/2\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_NO_GTK_IE);
            return;
        }

        maxkeylen = keylen = gtk_ie_len - 2;
    } else {
        keylen = (key->key_length);
        maxkeylen = keydatalen;
        if (keydatalen > extra_len) {
            wpa_printf(TRACE_GROUP_1, "WPA: Truncated EAPOL-Key packet:"
                       " key_data_length=%d > extra_len=%d\n",
                       keydatalen, extra_len);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_TRUNCATED_KEY, keydatalen, extra_len);
            return;
        }
        if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
            maxkeylen -= 8;
        }
    }

    if (wpa_supplicant_check_group_cipher(wpa_s, keylen, maxkeylen,
                                          &key_rsc_len, &alg)) {
        return;
    }

    if (wpa_s->proto == WPA_PROTO_RSN) {
        //wpa_hexdump(MSG_DEBUG,
        //"RSN: received GTK in group key handshake",
        //(char *)gtk_ie, gtk_ie_len);
//      kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_DUMP_GTK,gtk_ie_len,gtk_ie);
        keyidx = gtk_ie[0] & 0x3;
        tx = !!(gtk_ie[0] & BIT(2));
        if (gtk_ie_len - 2 > sizeof(gtk)) {
            wpa_printf(TRACE_GROUP_1, "RSN: Too long GTK in GTK IE "
                       "(len=%lu)\n",
                       (unsigned long) gtk_ie_len - 2);
//          kal_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_RX_1_2KEY_GTK_TOO_LONG,(unsigned long) gtk_ie_len - 2);
            return;
        }
        memcpy(gtk, gtk_ie + 2, gtk_ie_len - 2);
    } else {
        keyidx = (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
                 WPA_KEY_INFO_KEY_INDEX_SHIFT;
        if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4) {
            memcpy(ek, key->key_iv, 16);
            memcpy(ek + 16, wpa_s->ptk.encr_key, 16);
            /* Joseph: CIPHER REVISE */
            rc4_skip(ek, 32, 256, key_data_ptr, (wpa_size_t)keydatalen);
            /* CIPHER REVISE */
            memcpy(gtk, key_data_ptr, keylen);
        } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
            if (keydatalen % 8) {
                wpa_printf(TRACE_WARNING, "WPA: Unsupported "
                           "AES-WRAP len %d\n", keydatalen);
//              kal_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_RX_1_2KEY_UNSUPPORT_AES, keydatalen);
                return;
            }
            if (aes_unwrap(wpa_s->ptk.encr_key, maxkeylen,
                           key_data_ptr, gtk)) {
                wpa_printf(TRACE_WARNING, "WPA: AES unwrap "
                           "failed - could not decrypt GTK\n");
//              kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_AES_DECRYPT_FAIL);
                return;
            }
        }

        tx = !!(key_info & WPA_KEY_INFO_TXRX);
    }

    if (tx && wpa_s->pairwise_cipher != WPA_CIPHER_NONE) {
        /* Ignore Tx bit in Group Key message if a pairwise key
             * is used. Some APs seem to setting this bit
             * (incorrectly, since Tx is only when doing Group Key
             * only APs) and without this workaround, the data
             * connection does not work because wpa_supplicant
             * configured non-zero keyidx to be used for unicast.
             */
        wpa_printf(TRACE_GROUP_1, "WPA: Tx bit set for GTK, but "
                   "pairwise keys are used - ignore Tx bit\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_RX_1_2KEY_GTK_IGNORE_TX_BIT);
        tx = 0;
    }

    wpa_supplicant_install_gtk(wpa_s, key, alg, gtk, keylen, keyidx,
                               key_rsc_len, tx);

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_data_req_struct), TD_RESET);
    // memset(local_para_ptr, 0 , sizeof(wndrv_supc_data_req_struct));  // Karen
    memcpy(local_para_ptr->src_mac_addr, wpa_s->own_addr, ETH_ALEN);
    memcpy(local_para_ptr->dest_mac_addr, src_addr, ETH_ALEN);
    local_para_ptr->proto_type = (ETH_P_EAPOL);


    rlen = sizeof(*hdr) + WPA_EAPOL_KEY_SIZE;
    //peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    peer_buff_ptr = construct_peer_buff(rlen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    pdu_ptr = get_pdu_ptr(peer_buff_ptr, &pdu_len);
    kal_mem_set(pdu_ptr, 0, pdu_len);

    /* 802.1X Header */
    hdr = (struct ieee802_1x_hdr *)(pdu_ptr);
    hdr->version = wpa_s->conf->eapol_version;
    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
    hdr->length = htons(WPA_EAPOL_KEY_SIZE);

    /* Key negotiation part */
    encode_ptr = (kal_uint8 *)(hdr + 1);
    // Descriptor Type
    *encode_ptr = wpa_s->proto == WPA_PROTO_RSN ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    encode_ptr ++;

    // Key information
    //*(kal_uint16 *)encode_ptr = host_to_be16(ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE |
    //    (key_info & WPA_KEY_INFO_KEY_INDEX_MASK));
    //encode_ptr += 2;
    *encode_ptr = ((ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE |
                    (key_info & WPA_KEY_INFO_KEY_INDEX_MASK)) & 0xFF00) >> 8;
    encode_ptr ++;
    *encode_ptr = (ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE |
                   (key_info & WPA_KEY_INFO_KEY_INDEX_MASK)) & 0x00FF;
    encode_ptr ++;

    // Key length
    //*(kal_uint16 *)encode_ptr = htons(key->key_length);
    //encode_ptr += 2;
    *encode_ptr = ((key->key_length) & 0xFF00) >> 8;
    encode_ptr ++;
    *encode_ptr = (key->key_length) & 0x00FF;
    encode_ptr ++;

    // Key replay counter
    kal_mem_cpy(encode_ptr, key->replay_counter,
                WPA_REPLAY_COUNTER_LEN);
    encode_ptr += WPA_REPLAY_COUNTER_LEN;

    // Key Nonce (none skiped)
    encode_ptr += 32;

    // Key IV (skiped)
    encode_ptr += 16;

    // Key RSC (skiped)
    encode_ptr += 8;

    // Reserved (skiped)
    encode_ptr += 8;

    // Key MIC
    wpa_eapol_key_mic(wpa_s->tptk.mic_key, ver, (u8 *) hdr,
                      rlen , encode_ptr);

    wpa_printf(TRACE_GROUP_1, "WPA: Sending EAPOL-Key 2/2\n");
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_TX_2_2KEY);
    //wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key 2/2", rbuf, rlen);

    supc_send_wndrv_data_req((local_para_struct *)local_para_ptr, peer_buff_ptr);
    //l2_packet_send(wpa_s->l2, rbuf, rlen);
    eapol_sm_notify_tx_eapol_key(wpa_s->eapol);
    //free(rbuf);

    if (rekey) {
        //wpa_msg(wpa_s, MSG_INFO, "WPA: Group rekeying completed with "
        //MACSTR " [GTK=%s]", MAC2STR(src_addr),
        // wpa_cipher_txt(wpa_s->group_cipher));

        wpa_printf(TRACE_GROUP_1, "WPA: Group rekeying completed with "
                   MACSTR " [GTK=%s]\n", MAC2STR(src_addr),
                   wpa_cipher_txt(wpa_s->group_cipher));
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_GROUP_KEY_HANDSHAKE_DONE,MAC2STR(src_addr));
        wpa_s->wpa_state = WPA_COMPLETED;
    } else {
        wpa_supplicant_key_neg_complete(wpa_s, src_addr,
                                        key_info &
                                        WPA_KEY_INFO_SECURE);

    }
}


static int wpa_supplicant_verify_eapol_key_mic(struct wpa_supplicant *wpa_s,
        struct wpa_eapol_key *key,
        int ver, u8 *buf, wpa_size_t len)
{
    //u8 mic[16];
    u8 *mic_ptr = buf + (81); //81 is the offset to mic
    int ok = 0;

    //memcpy(mic, key->key_mic, 16);
    if (wpa_s->tptk_set) {
        memset(mic_ptr, 0, 16);
        wpa_eapol_key_mic(wpa_s->tptk.mic_key, ver, buf, len,
                          mic_ptr);
        if (memcmp(key->key_mic, mic_ptr, 16) != 0) {
            wpa_printf(TRACE_WARNING, "WPA: Invalid EAPOL-Key MIC "
                       "when using TPTK - ignoring TPTK\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INVALID_KEY_MIC);
        } else {
            ok = 1;
            wpa_s->tptk_set = 0;
            wpa_s->ptk_set = 1;
            memcpy(&wpa_s->ptk, &wpa_s->tptk, sizeof(wpa_s->ptk));
        }
    }

    if (!ok && wpa_s->ptk_set) {
        memset(mic_ptr, 0, 16);
        wpa_eapol_key_mic(wpa_s->ptk.mic_key, ver, buf, len,
                          mic_ptr);
        if (memcmp(key->key_mic, mic_ptr, 16) != 0) {
            wpa_printf(TRACE_WARNING, "WPA: Invalid EAPOL-Key MIC "
                       "- dropping packet");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_INVALID_KEY_MIC_DROP_PACKET);
            return -1;
        }
        ok = 1;
    }

    if (!ok) {
        wpa_printf(TRACE_WARNING, "WPA: Could not verify EAPOL-Key MIC "
                   "- dropping packet");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_CAN_NOT_VERY_KEY_MIC_DROP_PACKET);
        return -1;
    }

    memcpy(wpa_s->rx_replay_counter, key->replay_counter,
           WPA_REPLAY_COUNTER_LEN);
    wpa_s->rx_replay_counter_set = 1;
    return 0;
}


/* Decrypt RSN EAPOL-Key key data (RC4 or AES-WRAP) */
static int wpa_supplicant_decrypt_key_data(struct wpa_supplicant *wpa_s,
        struct wpa_eapol_key *key, int ver, u8 *key_data_ptr)
{
    kal_uint16 keydatalen = (key->key_data_length);

    //wpa_hexdump(MSG_DEBUG, "RSN: encrypted key data",
    //(char *) key_data_ptr, keydatalen);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_ENCRY_KEY_DATA,keydatalen,key_data_ptr);
    if (!wpa_s->ptk_set) {
        wpa_printf(TRACE_WARNING, "WPA: PTK not available, "
                   "cannot decrypt EAPOL-Key key data.\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_CAN_NOT_DECRY_KEY_DATA);
        return -1;
    }

    /* Decrypt key data here so that this operation does not need
     * to be implemented separately for each message type. */
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4) {
        u8 ek[32];
        memcpy(ek, key->key_iv, 16);
        memcpy(ek + 16, wpa_s->ptk.encr_key, 16);
        rc4_skip(ek, 32, 256, key_data_ptr, keydatalen);
    } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        u8 *buf;
        if (keydatalen % 8) {
            wpa_printf(TRACE_WARNING, "WPA: Unsupported "
                       "AES-WRAP len %d\n", keydatalen);
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_AES_WRAP_LEN, keydatalen);
            return -1;
        }
        keydatalen -= 8; /* AES-WRAP adds 8 bytes */
        buf = SUPC_ALLOC_BUFF(keydatalen);
        if (buf == NULL) {
            wpa_printf(TRACE_WARNING, "WPA: No memory for "
                       "AES-UNWRAP buffer\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_NO_MEM_FOR_AES);
            return -1;
        }
        if (aes_unwrap(wpa_s->ptk.encr_key, keydatalen,
                       key_data_ptr, buf)) {
            SUPC_DEALLOC_BUFF(buf);
            wpa_printf(TRACE_WARNING, "WPA: AES unwrap failed - "
                       "could not decrypt EAPOL-Key key data\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_AES_UNWRAP_FAIL);
            return -1;
        }
        memcpy(key_data_ptr, buf, keydatalen);
        SUPC_DEALLOC_BUFF(buf);
        key->key_data_length = (keydatalen);
    }
    //wpa_hexdump_key(MSG_DEBUG, "WPA: decrypted EAPOL-Key key data",
    //(char *)key_data_ptr, keydatalen);
//  kal_buffer_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DUMP_DECRY_KEY_DATA,keydatalen,key_data_ptr);
    return 0;
}

static struct wpa_eapol_key *wpa_decode_eapol_key_frame(u8 *buff_ptr)
{
    struct wpa_eapol_key *ret_key_ptr = NULL;

    ret_key_ptr = SUPC_ALLOC_BUFF(sizeof(wpa_eapol_key));
    if (NULL == ret_key_ptr) {
        return NULL;
    }

    ret_key_ptr->type = *buff_ptr;
    buff_ptr++;

    ret_key_ptr->key_info = *buff_ptr * 256 + *(buff_ptr + 1);
    buff_ptr += 2;

    ret_key_ptr->key_length = *buff_ptr * 256 + *(buff_ptr + 1);
    buff_ptr += 2;

    kal_mem_cpy(ret_key_ptr->replay_counter, buff_ptr, WPA_REPLAY_COUNTER_LEN);
    buff_ptr += WPA_REPLAY_COUNTER_LEN;

    kal_mem_cpy(ret_key_ptr->key_nonce, buff_ptr, WPA_NONCE_LEN);
    buff_ptr += WPA_NONCE_LEN;

    kal_mem_cpy(ret_key_ptr->key_iv, buff_ptr, 16);
    buff_ptr += 16;

    kal_mem_cpy(ret_key_ptr->key_rsc, buff_ptr, 8);
    buff_ptr += 8;

    kal_mem_cpy(ret_key_ptr->key_id, buff_ptr, 8);
    buff_ptr += 8;

    kal_mem_cpy(ret_key_ptr->key_mic, buff_ptr, 16);
    buff_ptr += 16;

    ret_key_ptr->key_data_length = (*buff_ptr) * 256 + *(buff_ptr + 1);
    buff_ptr += 2;


    return ret_key_ptr;
}


static void wpa_sm_rx_eapol(struct wpa_supplicant *wpa_s,
                            unsigned char *src_addr, unsigned char *buf,
                            wpa_size_t len)
{
    wpa_size_t plen, data_len, extra_len;
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    int key_info, ver;

    hdr = (struct ieee802_1x_hdr *) buf;

    // J: memory alignment issue, the actuall size of struct wpa_eapol_key is 95 but
    // sizeof(*key) will be 96
    if (len < sizeof(*hdr) + WPA_EAPOL_KEY_SIZE) {
        wpa_printf(TRACE_WARNING, "WPA: EAPOL frame too short, len %u, "
                   "expecting at least %u\n",
                   (unsigned long) len,
                   (unsigned long) sizeof(*hdr) + sizeof(*key));
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_TOO_SHORT,(unsigned long) len,
//             (unsigned long) sizeof(*hdr) + sizeof(*key));
        return;
    }
    key = wpa_decode_eapol_key_frame((kal_uint8 *)(hdr + 1));
    if (NULL == key) {
        return;
    }
    plen = ntohs(hdr->length);
    data_len = plen + sizeof(*hdr);
    wpa_printf(TRACE_GROUP_1, "IEEE 802.1X RX: version=%d type=%d length=%d\n",
               hdr->version, hdr->type, (unsigned long) plen);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_VER_INFO,hdr->version, hdr->type, (unsigned long) plen);

    //wpa_drv_poll(wpa_s);

    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY) {
        wpa_printf(TRACE_GROUP_1, "WPA: EAPOL frame (type %u) discarded, "
                   "not a Key frame\n", hdr->type);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_DISCARD_NONE_KEY_FRAME, hdr->type);
        if (wpa_s->cur_pmksa) {
            wpa_printf(TRACE_GROUP_1, "WPA: Cancelling PMKSA caching "
                       "attempt - attempt full EAP "
                       "authentication\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_CANCEL_PMKSA_CACHE_ATTEMPT);
            eapol_sm_notify_pmkid_attempt(wpa_s->eapol, 0);
        }

        SUPC_DEALLOC_BUFF(key);
        return;
    }
    if (plen > len - sizeof(*hdr) || plen < WPA_EAPOL_KEY_SIZE) {
        wpa_printf(TRACE_GROUP_1, "WPA: EAPOL frame payload size %lu "
                   "invalid (frame size %lu)\n",
                   (unsigned long) plen, (unsigned long) len);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_PAYLOAD_INVALID,(unsigned long) plen, (unsigned long) len);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    wpa_printf(TRACE_GROUP_1, "  EAPOL-Key type=%d\n", key->type);
//  kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_KEY_TPYE, key->type);
    if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN) {
        wpa_printf(TRACE_GROUP_1, "WPA: EAPOL-Key type (%d) unknown, "
                   "discarded\n", key->type);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_KEY_TPYE_UNKNOWN, key->type);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    //wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-Key", (char *)buf, len);
    if (data_len < len) {
        wpa_printf(TRACE_GROUP_1, "WPA: ignoring %u bytes after the IEEE "
                   "802.1X data\n", (unsigned long) len - data_len);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_EAPOL_IGNORE_BYTES, (unsigned long) len - data_len);
    }
    key_info = key->key_info;
    ver = key_info & WPA_KEY_INFO_TYPE_MASK;

    if (ver != WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 &&
            ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        wpa_printf(TRACE_GROUP_1, "WPA: Unsupported EAPOL-Key descriptor "
                   "version %d.\n", ver);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_UNSUPPORTED_KEY_DESCRIPTOR, ver);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    if (wpa_s->pairwise_cipher == WPA_CIPHER_CCMP &&
            ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        wpa_printf(TRACE_GROUP_1, "WPA: CCMP is used, but EAPOL-Key "
                   "descriptor version (%d) is not 2.\n", ver);
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_CCMP_KEY_DES_NOT_MATCH, ver);
        if (wpa_s->group_cipher != WPA_CIPHER_CCMP &&
                !(key_info & WPA_KEY_INFO_KEY_TYPE)) {
            /* Earlier versions of IEEE 802.11i did not explicitly
             * require version 2 descriptor for all EAPOL-Key
             * packets, so allow group keys to use version 1 if
             * CCMP is not used for them. */
            wpa_printf(TRACE_GROUP_1, "WPA: Backwards compatibility: "
                       "allow invalid version for non-CCMP group "
                       "keys\n");
//          kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_NONE_CCMP_ALLOW_INVALID_VERSION);
        } else {
            SUPC_DEALLOC_BUFF(key);
        }
        return;
    }

    if (wpa_s->rx_replay_counter_set &&
            memcmp(key->replay_counter, wpa_s->rx_replay_counter,
                   WPA_REPLAY_COUNTER_LEN) <= 0) {
        wpa_printf(TRACE_WARNING, "WPA: EAPOL-Key Replay Counter did not"
                   " increase - dropping packet\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_KEY_REPLAY_COUNTER_WRONG);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    if (!(key_info & WPA_KEY_INFO_ACK)) {
        wpa_printf(TRACE_GROUP_1, "WPA: No Ack bit in key_info\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_NO_ACK_BIT_IN_KEY);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        wpa_printf(TRACE_GROUP_1, "WPA: EAPOL-Key with Request bit - "
                   "dropped\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_KEY_WITH_REQ_BIT_DROP);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    if ((key_info & WPA_KEY_INFO_MIC) &&
            wpa_supplicant_verify_eapol_key_mic(wpa_s, key, ver, buf,
                    data_len)) {
        SUPC_DEALLOC_BUFF(key);
        return;
    }

    extra_len = data_len - sizeof(*hdr) - WPA_EAPOL_KEY_SIZE;

    if ((key->key_data_length) > extra_len) {
        wpa_printf(TRACE_GROUP_1, "WPA: Invalid EAPOL-Key frame - "
                   "key_data overflow (%d > %d)\n",
                   (key->key_data_length), extra_len);
//      kal_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_KEY_DATA_OVERFLOW,(key->key_data_length), extra_len);

        SUPC_DEALLOC_BUFF(key);
        return;
    }

    if (wpa_s->proto == WPA_PROTO_RSN &&
            (key_info & WPA_KEY_INFO_ENCR_KEY_DATA) &&
            wpa_supplicant_decrypt_key_data(wpa_s, key, ver, (buf + WPA_EAPOL_KEY_SIZE + sizeof(ieee802_1x_hdr)))) {
        SUPC_DEALLOC_BUFF(key);
        return;
    }

    if (key_info & WPA_KEY_INFO_KEY_TYPE) {
        if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) {
            wpa_printf(TRACE_WARNING, "WPA: Ignored EAPOL-Key "
                       "(Pairwise) with non-zero key index\n");
//          kal_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_IGNORE_KEY_WITH_NONE_ZERO_INDEX);

            SUPC_DEALLOC_BUFF(key);
            return;
        }
        if (key_info & WPA_KEY_INFO_MIC) {
            /* 3/4 4-Way Handshake */
            wpa_supplicant_process_3_of_4(wpa_s, src_addr, key,
                                          extra_len, ver, (buf + WPA_EAPOL_KEY_SIZE + sizeof(ieee802_1x_hdr)));
        } else {
            /* 1/4 4-Way Handshake */
            wpa_supplicant_process_1_of_4(wpa_s, src_addr, key,
                                          ver, (buf + WPA_EAPOL_KEY_SIZE + sizeof(ieee802_1x_hdr)));
        }
    } else {
        if (key_info & WPA_KEY_INFO_MIC) {
            /* 1/2 Group Key Handshake */
            wpa_supplicant_process_1_of_2(wpa_s, src_addr, key,
                                          extra_len, ver, (buf + WPA_EAPOL_KEY_SIZE + sizeof(ieee802_1x_hdr)));
        } else {
            wpa_printf(TRACE_ERROR, "WPA: EAPOL-Key (Group) "
                       "without Mic bit - dropped\n");
//          kal_trace(DBG_KEY_HANDSHAKE, INFO_SUPC_DROP_GROUP_WITHOUT_MIC);
        }
    }

    SUPC_DEALLOC_BUFF(key);
}



void wpa_supplicant_rx_eapol(void *ctx, unsigned char *src_addr,
                             unsigned char *buf, wpa_size_t len)
{
    struct wpa_supplicant *wpa_s = ctx;

    wpa_printf(TRACE_GROUP_1, "RX EAPOL from " MACSTR "\n", MAC2STR(src_addr));
//  kal_trace(DBG_EAPOL_SM, INFO_SUPC_RX_EAPOL_FROM, MAC2STR(src_addr));
    wpa_printf(TRACE_GROUP_1, "RX frame len=%d\n", len);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_LEN, len);

    //wpa_hexdump(MSG_MSGDUMP, "RX EAPOL snip", (char *)buf, ((len > 32) ? 32 : len));
//  kal_buffer_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_DUMP,((len > 32) ? 32 : len),buf);

    if (wpa_s->eapol == NULL) {
        wpa_printf(TRACE_GROUP_1, "Ignore this EAPOL because state machine is deinit\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_IGNORE_EAPOL_DEINIT);
        return;
    }

    if (wpa_s->wpa_state < WPA_ASSOCIATED) {
        wpa_printf(TRACE_GROUP_1, "Ignore this EAPOL because in wrong state = %d\n", wpa_s->wpa_state);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_IGNORE_EAPOL_WRONG_STA, wpa_s->wpa_state);
        return;
    }
    /*if wps_s in open mode, it must ignore eapol-frame
     in wps scenario, the wps_s->key_mgmt is still WPA_KEY_MGMT_WPS,
     so we need to check curssid->key_mgmt*/
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE) {
        wpa_printf(TRACE_GROUP_1, "ignore received EAPOL frame, wps_s->key_megmt is open mode\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_IGNORE_EAPOL_OPEN_MODE);
        return;
    }
#if 0
    /*wpas_wps_eapol_cb will change wpa_s->key_mgmt and auth_alg, proto,
    so the following workaround method is discarded*/
    if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPS && wpa_s->current_ssid) {
        if (wpa_s->current_ssid->key_mgmt == WPA_KEY_MGMT_NONE) {
            wpa_printf(MSG_DEBUG, "ignore received EAPOL frame, curssid->key_megmt is open mode");
            return;
        }
    }
#endif
    /* Joseph:
     * How can it be so sure about it implies auth fails
     */
    /*Joseph:
     * This is the guard timer and starts at the moment first time we receive the EAPOL packet.
     */
    if (wpa_s->eapol_received == 0) {
        /* Timeout for completing IEEE 802.1X and WPA authentication */
        wpa_supplicant_req_auth_timeout(
            wpa_s,
            (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X ||
             wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) ?
            AUTH_TIMEOUT_IEEE8021X : AUTH_TIMEOUT_NOIEEE8021X, 0);
    }
    wpa_s->eapol_received++;

    if (wpa_s->countermeasures) {
        wpa_printf(TRACE_GROUP_1, "WPA: Countermeasures - dropped EAPOL "
                   "packet\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_IGNORE_EAPOL_COUNTMEASURES);
        return;
    }

    /* Source address of the incoming EAPOL frame could be compared to the
     * current BSSID. However, it is possible that a centralized
     * Authenticator could be using another MAC address than the BSSID of
     * an AP, so just allow any address to be used for now. The replies are
     * still sent to the current BSSID (if available), though. */

    memcpy(wpa_s->last_eapol_src, src_addr, ETH_ALEN);

    // wpa_printf(MSG_DEBUG, "in supc lib, wpa_s=%x eapol: %x", wpa_s, wpa_s->eapol);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_DUMP_DEBUG1, wpa_s, wpa_s->eapol);
    //wpa_printf(MSG_DEBUG, "in supc lib, sizeof wpa_s: %d",
    //sizeof(struct wpa_supplicant));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_DUMP_DEBUG2,sizeof(struct wpa_supplicant));
    //wpa_printf(MSG_DEBUG, "offset drv_priv: %d",
    //((int)(&((wpa_supplicant *)0)->drv_priv)));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_DUMP_DEBUG3,((int)(&((wpa_supplicant *)0)->drv_priv)));
    //wpa_printf(MSG_DEBUG, "offset eapol: %d",
    //((int)(&((wpa_supplicant *)0)->eapol)));
//    kal_trace(DBG_EAPOL_SM,INFO_SUPC_DUMP_DEBUG4,((int)(&((wpa_supplicant *)0)->eapol)));
    eapol_sm_rx_eapol(wpa_s->eapol, src_addr, buf, len);
    wpa_sm_rx_eapol(wpa_s, src_addr, buf, len);
    // Add by Joseph
    eapol_sm_step(wpa_s->eapol);
}

#if 0
static int wpa_cipher_bits(int cipher)
{
    switch (cipher) {
        case WPA_CIPHER_CCMP:
            return 128;
        case WPA_CIPHER_TKIP:
            return 256;
        case WPA_CIPHER_WEP104:
            return 104;
        case WPA_CIPHER_WEP40:
            return 40;
        default:
            return 0;
    }
}


static const u8 *wpa_key_mgmt_suite(struct wpa_supplicant *wpa_s)
{
    static const u8 *dummy = (u8 *) "\x00\x00\x00\x00";
    switch (wpa_s->key_mgmt) {
        case WPA_KEY_MGMT_IEEE8021X:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_AUTH_KEY_MGMT_UNSPEC_802_1X :
                    WPA_AUTH_KEY_MGMT_UNSPEC_802_1X);
        case WPA_KEY_MGMT_PSK:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X :
                    WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X);
        case WPA_KEY_MGMT_WPA_NONE:
            return WPA_AUTH_KEY_MGMT_NONE;
        default:
            return dummy;
    }
}


static const u8 *wpa_cipher_suite(struct wpa_supplicant *wpa_s, int cipher)
{
    static const u8 *dummy = (u8 *) "\x00\x00\x00\x00";
    switch (cipher) {
        case WPA_CIPHER_CCMP:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_CCMP : WPA_CIPHER_SUITE_CCMP);
        case WPA_CIPHER_TKIP:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_TKIP : WPA_CIPHER_SUITE_TKIP);
        case WPA_CIPHER_WEP104:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_WEP104 : WPA_CIPHER_SUITE_WEP104);
        case WPA_CIPHER_WEP40:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_WEP40 : WPA_CIPHER_SUITE_WEP40);
        case WPA_CIPHER_NONE:
            return (wpa_s->proto == WPA_PROTO_RSN ?
                    RSN_CIPHER_SUITE_NONE : WPA_CIPHER_SUITE_NONE);
        default:
            return dummy;
    }
}
#endif

#define RSN_SUITE "%02x-%02x-%02x-%d"
#define RSN_SUITE_ARG(s) (s)[0], (s)[1], (s)[2], (s)[3]
#if 0 //Joseph remark

int wpa_get_mib(struct wpa_supplicant *wpa_s, char *buf, wpa_size_t buflen)
{
    int len, i;
    char pmkid_txt[PMKID_LEN * 2 + 1];

    if (wpa_s->cur_pmksa) {
        char *pos = pmkid_txt;
        for (i = 0; i < PMKID_LEN; i++) {
            pos += sprintf(pos, "%02x",
                           wpa_s->cur_pmksa->pmkid[i]);
        }
    } else {
        pmkid_txt[0] = '\0';
    }

    len = snprintf(buf, buflen,
                   "dot11RSNAConfigVersion=%d\n"
                   "dot11RSNAConfigPairwiseKeysSupported=5\n"
                   "dot11RSNAConfigGroupCipherSize=%d\n"
                   "dot11RSNAConfigPMKLifetime=%d\n"
                   "dot11RSNAConfigPMKReauthThreshold=%d\n"
                   "dot11RSNAConfigNumberOfPTKSAReplayCounters=1\n"
                   "dot11RSNAConfigSATimeout=%d\n"
                   "dot11RSNAAuthenticationSuiteSelected=" RSN_SUITE "\n"
                   "dot11RSNAPairwiseCipherSelected=" RSN_SUITE "\n"
                   "dot11RSNAGroupCipherSelected=" RSN_SUITE "\n"
                   "dot11RSNAPMKIDUsed=%s\n"
                   "dot11RSNAAuthenticationSuiteRequested=" RSN_SUITE "\n"
                   "dot11RSNAPairwiseCipherRequested=" RSN_SUITE "\n"
                   "dot11RSNAGroupCipherRequested=" RSN_SUITE "\n"
                   "dot11RSNAConfigNumberOfGTKSAReplayCounters=0\n",
                   RSN_VERSION,
                   wpa_cipher_bits(wpa_s->group_cipher),
                   dot11RSNAConfigPMKLifetime,
                   dot11RSNAConfigPMKReauthThreshold,
                   dot11RSNAConfigSATimeout,
                   RSN_SUITE_ARG(wpa_key_mgmt_suite(wpa_s)),
                   RSN_SUITE_ARG(wpa_cipher_suite(wpa_s,
                                 wpa_s->pairwise_cipher)),
                   RSN_SUITE_ARG(wpa_cipher_suite(wpa_s,
                                 wpa_s->group_cipher)),
                   pmkid_txt,
                   RSN_SUITE_ARG(wpa_key_mgmt_suite(wpa_s)),
                   RSN_SUITE_ARG(wpa_cipher_suite(wpa_s,
                                 wpa_s->pairwise_cipher)),
                   RSN_SUITE_ARG(wpa_cipher_suite(wpa_s,
                                 wpa_s->group_cipher)));
    return len;
}
#endif //#if 0


#ifdef IEEE8021X_EAPOL

static void rsn_preauth_receive(void *ctx, unsigned char *src_addr,
                                unsigned char *buf, wpa_size_t len)
{
    struct wpa_supplicant *wpa_s = ctx;

    wpa_printf(TRACE_GROUP_1, "RX pre-auth from " MACSTR "\n", MAC2STR(src_addr));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_PRE_AUTH_FROM, MAC2STR(src_addr));
    //wpa_hexdump(MSG_MSGDUMP, "RX pre-auth", (char *)buf, len);
//  kal_buffer_trace(DBG_EAPOL_SM,INFO_SUPC_DUMP_PRE_AUTH, len, buf);

    if (wpa_s->preauth_eapol == NULL ||
            memcmp(wpa_s->preauth_bssid, "\x00\x00\x00\x00\x00\x00",
                   ETH_ALEN) == 0 ||
            memcmp(wpa_s->preauth_bssid, src_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_WARNING, "RSN pre-auth frame received from "
                   "unexpected source " MACSTR " - dropped\n",
                   MAC2STR(src_addr));
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_DROP_PRE_AUTH_UNKNOWN_SOURCE,MAC2STR(src_addr));
        return;
    }

    eapol_sm_rx_eapol(wpa_s->preauth_eapol, src_addr, buf, len);
}


static void rsn_preauth_eapol_cb(struct eapol_sm *eapol, int success,
                                 void *ctx)
{
    struct wpa_supplicant *wpa_s = ctx;
    u8 pmk[PMK_LEN];

    wpa_printf(TRACE_GROUP_1, "RSN: pre-authentication with " MACSTR
               " %s", MAC2STR(wpa_s->preauth_bssid),
               success ? "completed successfully\n" : "failed\n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_PRE_AUTH_RESULT, MAC2STR(wpa_s->preauth_bssid),success);

    if (success) {
        int res, pmk_len;
        pmk_len = PMK_LEN;
        res = eapol_sm_get_key(eapol, pmk, PMK_LEN);
#ifdef EAP_LEAP
        if (res) {
            res = eapol_sm_get_key(eapol, pmk, 16);
            pmk_len = 16;
        }
#endif /* EAP_LEAP */
        if (res == 0) {
            //wpa_hexdump_key(MSG_DEBUG, "RSN: PMK from pre-auth",
            //(char *)pmk, (wpa_size_t)pmk_len);
//          kal_buffer_trace(DBG_EAPOL_SM,INFO_SUPC_DUMP_PMK_IN_PRE_AUTH,pmk_len,pmk);
            wpa_s->pmk_len = pmk_len;
            pmksa_cache_add(pmk, (wpa_size_t)pmk_len,
                            wpa_s->preauth_bssid, wpa_s->own_addr);
        } else {
            //wpa_msg(wpa_s, MSG_INFO, "RSN: failed to get master "
            //"session key from pre-auth EAPOL state "
            //"machines");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_GET_MSK_FAIL_IN_PRE_AUTH_SM);
        }
    }

    rsn_preauth_deinit(wpa_s);
    rsn_preauth_candidate_process(wpa_s);
}

#if 0
static void rsn_preauth_timeout(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_supplicant *wpa_s = eloop_ctx;
    wpa_msg(wpa_s, MSG_INFO, "RSN: pre-authentication with " MACSTR
            " timed out", MAC2STR(wpa_s->preauth_bssid));
    rsn_preauth_deinit(wpa_s);
    rsn_preauth_candidate_process(wpa_s);
}
#endif

int rsn_preauth_init(struct wpa_supplicant *wpa_s, u8 *dst)
{
    struct eapol_config eapol_conf;
    struct eapol_ctx *ctx;

    if (wpa_s->preauth_eapol) {
        return -1;
    }

    //wpa_printf(MSG_DEBUG, "RSN: starting pre-authentication with "
    //MACSTR, MAC2STR(dst));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_START_PRE_AUTH_WITH, MAC2STR(dst));

    wpa_s->l2_preauth = l2_packet_init(wpa_s->ifname,
                                       wpa_s->own_addr,
                                       ETH_P_RSN_PREAUTH,
                                       rsn_preauth_receive, wpa_s);
    if (wpa_s->l2_preauth == NULL) {
        //wpa_printf(MSG_WARNING, "RSN: Failed to initialize L2 packet "
        //"processing for pre-authentication");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_INIT_PRE_AUTH_FAIL);
        return -2;
    }

    ctx = SUPC_ALLOC_BUFF(sizeof(*ctx));
    if (ctx == NULL) {
        //wpa_printf(MSG_WARNING, "Failed to allocate EAPOL context.");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_INIT_PRE_AUTH_CONTEX_FAIL);
        return -4;
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->ctx = wpa_s;
    ctx->preauth = 1;
    ctx->cb = rsn_preauth_eapol_cb;
    ctx->cb_ctx = wpa_s;
    ctx->scard_ctx = wpa_s->scard;
    ctx->eapol_done_cb = wpa_supplicant_notify_eapol_done;
    ctx->eapol_send = wpa_eapol_send_preauth;

    wpa_s->preauth_eapol = eapol_sm_init(ctx);
    if (wpa_s->preauth_eapol == NULL) {
        SUPC_DEALLOC_BUFF(ctx);
        //wpa_printf(MSG_WARNING, "RSN: Failed to initialize EAPOL "
        //"state machines for pre-authentication");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_INIT_PRE_AUTH_SM_FAIL);
        return -3;
    }
    memset(&eapol_conf, 0, sizeof(eapol_conf));
    eapol_conf.accept_802_1x_keys = 0;
    eapol_conf.required_keys = 0;
    eapol_conf.fast_reauth = wpa_s->conf->fast_reauth;
    if (wpa_s->current_ssid) {
        eapol_conf.workaround = wpa_s->current_ssid->eap_workaround;
    }
    eapol_sm_notify_config(wpa_s->preauth_eapol, wpa_s->current_ssid,
                           &eapol_conf);
    memcpy(wpa_s->preauth_bssid, dst, ETH_ALEN);

    eapol_sm_notify_portValid(wpa_s->preauth_eapol, FALSE); // Kinki change for pre-auth port timer
    /* 802.1X::portControl = Auto */
    eapol_sm_notify_portEnabled(wpa_s->preauth_eapol, TRUE);

    //eloop_register_timeout(60, 0, rsn_preauth_timeout, wpa_s, NULL);
    wpa_s->preauth_timer_event_id = supc_start_preauth_timer(60);

    //supc_start_preauth_timer(60);

    return 0;
}


void rsn_preauth_deinit(struct wpa_supplicant *wpa_s)
{
    if (!wpa_s->preauth_eapol) {
        return;
    }
    supc_stop_preauth_timer();
//  eloop_cancel_timeout(rsn_preauth_timeout, wpa_s, NULL);
    eapol_sm_deinit(wpa_s->preauth_eapol);
    wpa_s->preauth_eapol = NULL;
    memset(wpa_s->preauth_bssid, 0, ETH_ALEN);

    //l2_packet_deinit(wpa_s->l2_preauth);
    SUPC_DEALLOC_BUFF(wpa_s->l2_preauth);
    wpa_s->l2_preauth = NULL;
}


void rsn_preauth_candidate_process(struct wpa_supplicant *wpa_s)
{
    struct rsn_pmksa_candidate *candidate;

    if (wpa_s->pmksa_candidates == NULL) {
        return;
    }

    /* TODO: drop priority for old candidate entries */

    //wpa_printf(MSG_DEBUG, "RSN: processing PMKSA candidate list");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_PRO_CAND_LIST);

    if (wpa_s->preauth_eapol ||
            wpa_s->proto != WPA_PROTO_RSN ||
            wpa_s->wpa_state != WPA_COMPLETED ||
            wpa_s->key_mgmt != WPA_KEY_MGMT_IEEE8021X) {
        //wpa_printf(MSG_DEBUG, "RSN: not in suitable state for new "
        //"pre-authentication");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_NO_SUIT_SM_FOR_PRE_AUTH);
        return; /* invalid state for new pre-auth */
    }

    while (wpa_s->pmksa_candidates) {
        struct rsn_pmksa_cache *p = NULL;
        candidate = wpa_s->pmksa_candidates;
        p = pmksa_cache_get(wpa_s, candidate->bssid, NULL);
        if (kal_mem_cmp(wpa_s->bssid, candidate->bssid, ETH_ALEN) != 0 &&
                p == NULL) {
            //wpa_printf(MSG_DEBUG, "RSN: PMKSA candidate "
            //MACSTR " selected for pre-authentication",
            //MAC2STR(candidate->bssid));
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_PMKSA_SEL_FOR_PRE_AUTH,MAC2STR(candidate->bssid));
            wpa_s->pmksa_candidates = candidate->next;
            rsn_preauth_init(wpa_s, candidate->bssid);
            SUPC_DEALLOC_BUFF(candidate);
            return;
        }
        //wpa_printf(MSG_DEBUG, "RSN: PMKSA candidate " MACSTR
        //" does not need pre-authentication anymore",
        //MAC2STR(candidate->bssid));
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_NO_NEED_PRE_AUTH,MAC2STR(candidate->bssid));
        /* Some drivers (e.g., NDIS) expect to get notified about the
         * PMKIDs again, so report the existing data now. */
        if (p) {
            wpa_driver_mtk_add_pmkid(wpa_s->drv_priv, candidate->bssid, p->pmkid);
        }

        wpa_s->pmksa_candidates = candidate->next;
        SUPC_DEALLOC_BUFF(candidate);
    }
    //wpa_printf(MSG_DEBUG, "RSN: no more pending PMKSA candidates");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_NO_PENDING_PMKSA_CAND);
}


void pmksa_candidate_add(struct wpa_supplicant *wpa_s, const u8 *bssid,
                         int prio)
{
    struct rsn_pmksa_candidate *cand, *prev, *pos;

    /* If BSSID already on candidate list, update the priority of the old
     * entry. Do not override priority based on normal scan results. */
    prev = NULL;
    cand = wpa_s->pmksa_candidates;
    while (cand) {
        if (kal_mem_cmp(cand->bssid, bssid, ETH_ALEN) == 0) {
            if (prev) {
                prev->next = cand->next;
            } else {
                wpa_s->pmksa_candidates = cand->next;
            }
            break;
        }
        prev = cand;
        cand = cand->next;
    }

    if (cand) {
        if (prio < PMKID_CANDIDATE_PRIO_SCAN) {
            cand->priority = prio;
        }
    } else {
        cand = SUPC_ALLOC_BUFF(sizeof(*cand));
        if (cand == NULL) {
            return;
        }
        kal_mem_set(cand, 0, sizeof(*cand));
        kal_mem_cpy(cand->bssid, bssid, ETH_ALEN);
        cand->priority = prio;
    }

    /* Add candidate to the list; order by increasing priority value. i.e.,
     * highest priority (smallest value) first. */
    prev = NULL;
    pos = wpa_s->pmksa_candidates;
    while (pos) {
        if (cand->priority <= pos->priority) {
            break;
        }
        prev = pos;
        pos = pos->next;
    }
    cand->next = pos;
    if (prev) {
        prev->next = cand;
    } else {
        wpa_s->pmksa_candidates = cand;
    }

    //wpa_printf(MSG_DEBUG, "RSN: added PMKSA cache "
    //"candidate " MACSTR " prio %d", MAC2STR(bssid), prio);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_ADDED_PMKSA_CACHE, MAC2STR(bssid), prio);
    rsn_preauth_candidate_process(wpa_s);
}


/* TODO: schedule periodic scans if current AP supports preauth */
void rsn_preauth_scan_results(struct wpa_supplicant *wpa_s,
                              wndrv_supc_bss_info_struct *results, int count)
{
    //struct wpa_scan_result *r;
    struct wndrv_supc_bss_info_struct *r;
    struct wpa_ie_data ie;
    int i;

    INNER_TRACE_STEP(1);
    if (wpa_s->current_ssid == NULL) {
        return;
    }

    pmksa_candidate_free(wpa_s);

    for (i = count - 1; i >= 0; i--) {
        r = &results[i];
        if (r->ssid_len == wpa_s->current_ssid->ssid_len &&
                kal_mem_cmp(r->ssid, wpa_s->current_ssid->ssid, r->ssid_len) ==
                0 &&
                kal_mem_cmp(r->bssid, wpa_s->bssid, ETH_ALEN) != 0 &&
                r->rsn_ie_len > 0 &&
                wpa_parse_wpa_ie(wpa_s, r->rsn_ie, r->rsn_ie_len, &ie) ==
                0 &&
                (ie.capabilities & WPA_CAPABILITY_PREAUTH) &&
                pmksa_cache_get(wpa_s, r->bssid, NULL) == NULL) {
            /* Give less priority to candidates found from normal
             * scan results. */
            pmksa_candidate_add(wpa_s, r->bssid,
                                PMKID_CANDIDATE_PRIO_SCAN);
        }
    }
}

#else /* IEEE8021X_EAPOL */

void rsn_preauth_candidate_process(struct wpa_supplicant *wpa_s)
{
}

#endif /* IEEE8021X_EAPOL */

