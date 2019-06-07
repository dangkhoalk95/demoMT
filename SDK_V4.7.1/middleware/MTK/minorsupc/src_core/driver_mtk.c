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

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif


/* ****************************************************************************
* Alternatively, this software may be distributed under the terms of BSD
* license.
*****************************************************************************/


#include <stdlib.h>
//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

//#include "stack_common.h"
#include "stack_msgs.h"
//#include "app_ltlcom.h"           /* Task message communiction */
//#include "syscomp_config.h"
//#include "task_config.h"          /* Task creation */
//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
//#include "app_buff_alloc.h"


#include "supc_common.h"

#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"
#include "wpa_supplicant.h"

#include "wpa.h"
#include "supc_abm_msgs.h"
#include "config_ssid.h"
#include "wpa_supplicant_i.h"
#include "supc_wpa_common.h"

#include "driver_mtk.h"

#include "stack_ltlcom.h"
//#include "app_ltlcom.h"
#include "defs.h"
/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                        P U B L I C   D A T A
*************************************************************************
*/


/************************************************************************
*                       P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                             M A C R O S
*************************************************************************
*/

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
//static int  iGetOID(struct wpa_driver_mtk_data *drv, unsigned int oid,
//          char *data, int len);
#if 0
static void wpa_driver_mtk_event_media_specific(struct wpa_driver_mtk_data *drv,
        const u8 *data, wpa_size_t data_len);
//static void wpa_driver_wext_scan_timeout(void *eloop_ctx, void *timeout_ctx);
static void wpa_driver_mtk_event_pmkid(struct wpa_driver_mtk_data *drv,
                                       const u8 *data, wpa_size_t data_len);
#endif
static void wpa_driver_mtk_get_capability(struct wpa_driver_mtk_data *drv, wndrv_supc_param_capability_struct *capability);
int wpa_driver_mtk_set_pmkid(struct wpa_driver_mtk_data *drv);

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/



int wpa_driver_mtk_get_bssid(void *priv, u8 *bssid)
{
    /* J: Assume it intends to get the current ap's bssid */
    // bssid = wpa_s->bssid;
    kal_mem_cpy(bssid, wpa_s->bssid, ETH_ALEN); // Karen

    return KAL_TRUE;
    //return iGetOID(drv, OID_802_11_BSSID, bssid, ETH_ALEN) < 0 ? -1 : 0;
}

int wpa_driver_mtk_get_ssid(void *priv, u8 *ssid)
{
    if (wpa_s->ssid_len == 0) {
        return 0;
    } else {
        kal_mem_cpy(ssid, wpa_s->ssid, wpa_s->ssid_len);
    }

    return wpa_s->ssid_len;
}


static int wpa_driver_mtk_set_ssid(struct wpa_driver_mtk_data *drv,
                                   const u8 *ssid, wpa_size_t ssid_len)
{
    param_ssid_t rssid;

    memset(&rssid, 0, sizeof(rssid));
    rssid.len = ssid_len;
    memcpy(rssid.ssid, ssid, ssid_len);

    //return driver_set_ssid(drv, &rssid);
    return 0;
}

#if 0
/* Disconnect using OID_802_11_DISASSOCIATE. This will also turn the radio off.
 */
static int wpa_driver_mtk_radio_off(struct wpa_driver_mtk_data *drv)
{
    drv->radio_enabled = 0;
    return iSetOID(drv, OID_802_11_DISASSOCIATE, "    ", 4);
}
#endif

/* Disconnect by setting SSID to random (i.e., likely not used). */
static int wpa_driver_mtk_disconnect(struct wpa_driver_mtk_data *drv)
{
    char ssid[32];
    int i;
    for (i = 0; i < 32; i++) {
        ssid[i] = rand() & 0xff;
    }
    return wpa_driver_mtk_set_ssid(drv, (u8 *)ssid, 32);
}

#if 0
static void wpa_driver_mtk_get_ies(struct wpa_scan_result *res, u8 *ie,
                                   wpa_size_t ie_len)
{
    u8 *pos = ie;
    u8 *end = ie + ie_len;

    if (ie_len < sizeof(NDIS_802_11_FIXED_IEs)) {
        return;
    }

    pos += sizeof(NDIS_802_11_FIXED_IEs);
    /* wpa_hexdump(MSG_MSGDUMP, "IEs", pos, end - pos); */
    while (pos + 1 < end && pos + 2 + pos[1] <= end) {
        u8 ielen = 2 + pos[1];
        if (ielen > SSID_MAX_WPA_IE_LEN) {
            pos += ielen;
            continue;
        }
        if (pos[0] == GENERIC_INFO_ELEM && pos[1] >= 4 &&
                memcmp(pos + 2, "\x00\x50\xf2\x01", 4) == 0) {
            memcpy(res->wpa_ie, pos, ielen);
            res->wpa_ie_len = ielen;
        } else if (pos[0] == RSN_INFO_ELEM) {
            memcpy(res->rsn_ie, pos, ielen);
            res->rsn_ie_len = ielen;
        }
        pos += ielen;
    }
}
#endif

/*----------------------------------------------------------------*/
/*!
* \brief
*
* \param
* \return
*/
/*----------------------------------------------------------------*/
int wpa_driver_mtk_set_wpa(void *priv, int enabled)
{
    int ret = 0;

//        wpa_printf(MSG_DEBUG, "%s: enabled=%d", __FUNCTION__, enabled);

    return ret;
}

int wpa_driver_mtk_set_countermeasures(void *priv,
                                       int enabled)
{
    wpa_printf(TRACE_GROUP_1, "wpa_driver_mtk_set_countermeasures - handle by sta driver \n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DRV_SET_COUNTMEASURE);
    return 0;
}


int wpa_driver_mtk_set_drop_unencrypted(void *priv,
                                        int enabled)
{
    /* FIX */
    wpa_printf(TRACE_GROUP_1, "wpa_driver_set_drop_unencrypted - not yet "
               "implemented\n");
//      kal_trace(DBG_KEY_HANDSHAKE,INFO_SUPC_DRV_SET_DROP_UNENCRYPTED);
    return 0;
}

int wpa_driver_mtk_deauthenticate(void *priv, const u8 *addr,
                                  int reason_code)
{
#if 1
    struct wpa_driver_mtk_data *drv = priv;
    return wpa_driver_mtk_disconnect(drv);
#else
    wpa_printf(MSG_DEBUG, "%s", __FUNCTION__);

    if (wpa_driver_mlme(priv, addr, MLME_STA_DEAUTH, reason_code)) {
        printf("wpa_driver_mtk_deauthenticate fail !!!\n");
        return -1;
    }
    return 0;
#endif
}


int wpa_driver_mtk_disassociate(void *priv, const u8 *addr,
                                int reason_code)
{
#if 1
    struct wpa_driver_mtk_data *drv = priv;
    return wpa_driver_mtk_disconnect(drv);
#else
    wpa_printf(MSG_DEBUG, "%s", __FUNCTION__);
    if (wpa_driver_mlme(priv, addr, MLME_STA_DISASSOC, reason_code)) {
        printf("wpa_driver_mtk_disassociate fail !!!\n");
        return -1;
    }
    return 0;
#endif
}


int wpa_driver_mtk_associate(void *priv,
                             struct wpa_driver_associate_params *params)
{
    //u32 priv_mode;
    kal_uint8 i = 0;

    wndrv_supc_join_network_req_struct *local_para_ptr = NULL;
    static kal_uint8 session_id = 0;

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_join_network_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_join_network_req_struct), TD_RESET);
    local_para_ptr->has_wps_ie = KAL_FALSE;
    // local_para_ptr->ref_count = ref_cnt++;
    wpa_s->join_session_id = session_id++;
    local_para_ptr->session_id = wpa_s->join_session_id;

    /* Note: Setting OID_802_11_INFRASTRUCTURE_MODE clears current keys,
    * so static WEP keys needs to be set again after this. */
    /*
    if (params->mode == IEEE80211_MODE_IBSS)
        local_para_ptr->network_type = WNDRV_SUPC_NETWORK_IBSS;
    else
        local_para_ptr->network_type = WNDRV_SUPC_NETWORK_INFRASTRUCTURE;
    */
    local_para_ptr->network_type = (wndrv_supc_network_type_enum) params->mode;

    /* J: The action set_infra_mode will be together with set ssid. Remark it */
    //driver_set_infra_mode(drv, mode);
    INNER_TRACE_STEP(1);

    wpa_printf(MSG_DEBUG,
               "input para: chnl:%d wpa_len=%d gp_suite=%d, "
               "key_mgmt=%d, mode=%d, paire_suite=%d, auth_alg=%d, wep_enable=%d\n",
               params->channel_number, params->wpa_ie_len,
               params->group_suite, params->key_mgmt_suite,
               params->mode, params->pairwise_suite, params->auth_alg,
               params->wep_enabled);
//    kal_trace(DBG_CONN,INFO_SUPC_ASSOC_PARA_DUMP,
//        params->channel_number, params->wpa_ie_len,
//        params->group_suite, params->key_mgmt_suite,
//        params->mode, params->pairwise_suite, params->auth_alg,
//        params->wep_enabled);
    if (params->wpa_ie == NULL || params->wpa_ie_len == 0) {
        if (params->auth_alg & AUTH_ALG_SHARED_KEY) {
            if (params->auth_alg & AUTH_ALG_OPEN_SYSTEM) {
                local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_AUTO_SWITCH;
            } else {
                local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_SHARED;
            }
        } else {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_OPEN;
        }

        // priv_mode = Ndis802_11PrivFilterAcceptAll;
    } else if (params->wpa_ie[0] == RSN_INFO_ELEM) {
        // priv_mode = Ndis802_11PrivFilter8021xWEP;

        if (params->key_mgmt_suite == KEY_MGMT_PSK) {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA2_PSK;
        } else {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA2;
        }
    }
#ifdef __WAPI_SUPPORT__
    else if (params->wpa_ie[0] == WAPI_INFO_ELEM) {
        // priv_mode = Ndis802_11PrivFilter8021xWEP;

        if (params->key_mgmt_suite == KEY_MGMT_WAPI_PSK) {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WAPI_PSK;
        } else {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WAPI;
        }
    }
#endif

#ifdef __WPS_SUPPORT__
    else if (WLAN_EID_VENDOR_SPECIFIC == params->wpa_ie[0] &&
             WPS_DEV_OUI_WFA == WPA_GET_BE32(&(params->wpa_ie[2]))) {
        const kal_uint8 *pattr = (params->wpa_ie + 6);  // elmID + len + OUI
        /*
        elmID(1) + len(1) + OUI(4) + ATTR_VER(5) + ATTR_REQ_TYPE(5)
        */
        SUPC_ASSERT(16 == params->wpa_ie_len);
        wpa_printf(TRACE_GROUP_1, "assoc via WPS IE\n");
//      kal_trace(DBG_WPS,INFO_SUPC_ASSOC_VIA_WPS_IE);
        local_para_ptr->has_wps_ie = KAL_TRUE;
        local_para_ptr->wps_version = *(pattr + 4);//WPS_VERSION_V1;

        /*in WPS, set auth_mode to WNDRV_SUPC_AUTH_MODE_WPA_PSK, to
        avoid WNDRV to filter bssid according to AP's auth_mod*/
        if (KEY_MGMT_WPS_OPEN == params->key_mgmt_suite) {
            wpa_printf(TRACE_GROUP_1, "associate open-mode\n");
//          kal_trace(DBG_CONN,INFO_SUPC_ASSOC_OPEN_MODE);
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_OPEN;
        } else if (KEY_MGMT_WPS_WPA == params->key_mgmt_suite) {
            wpa_printf(MSG_DEBUG, "associate WPA\n");
//          kal_trace(DBG_CONN,INFO_SUPC_ASSOC_WPA_MODE);
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA_PSK;
        } else {
            wpa_printf(MSG_DEBUG, "associate WPA2\n");
//          kal_trace(DBG_CONN,INFO_SUPC_ASSOC_WPA2_MODE);
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA2_PSK;
        }
        //priv_mode = Ndis802_11PrivFilterAcceptAll;
    }
#endif
    else {
        // priv_mode = Ndis802_11PrivFilter8021xWEP;
        if (params->key_mgmt_suite == KEY_MGMT_WPA_NONE) {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA_NONE;
        } else if (params->key_mgmt_suite == KEY_MGMT_PSK) {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA_PSK;
        } else {
            local_para_ptr->auth_mode = WNDRV_SUPC_AUTH_MODE_WPA;
        }
    }

    switch (params->pairwise_suite) {
        case CIPHER_CCMP:
            local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION3_ENABLED;
            break;
        case CIPHER_TKIP:
            local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION2_ENABLED;
            break;
        case CIPHER_WEP40:
        case CIPHER_WEP104:
            local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION1_ENABLED;
            break;
#ifdef __WAPI_SUPPORT__
        case CIPHER_SMS4:
            local_para_ptr->encryption = WNDRV_SUPC_WPI_ENABLED;
            break;
#endif
        case CIPHER_NONE:
            if (params->group_suite == CIPHER_CCMP) {
                local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION3_ENABLED;
            } else if (params->group_suite == CIPHER_TKIP) {
                local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION2_ENABLED;
            } else if (params->group_suite == CIPHER_WEP104 ||
                       params->group_suite == CIPHER_WEP40) {
                local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION1_ENABLED;
            } else {
                local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION_DISABLED;
            }
            break;
        default:
            local_para_ptr->encryption = WNDRV_SUPC_ENCRYPTION_DISABLED;
    };

    /* J: JC's comment: priv_mode is not used. Just ignore it */
    /*
    if (iSetOID(drv, OID_802_11_PRIVACY_FILTER,
        (char *) &priv_mode, sizeof(priv_mode)) < 0)
    {
        wpa_printf(MSG_DEBUG, "NDIS: Failed to set "
            "OID_802_11_PRIVACY_FILTER (%d)",
            (int) priv_mode);
    }
    */

    /* set auth and encr will be together with set ssid. Remark them */
    //driver_set_auth_mode(drv, auth_mode);
    //driver_set_encr_status(drv, encr);

    /* J: ssid information */
    kal_mem_cpy(&local_para_ptr->ssid[0], params->ssid, params->ssid_len);
    local_para_ptr->ssid_len = params->ssid_len;

    /* J: channel number TBD */
    local_para_ptr->channel_number = params->channel_number;

    /* J: WEP keys */
    if (params->wep_enabled > 0) {
        for (i = 0; i < NUM_WEP_KEYS; i++) {
            local_para_ptr->wep_info[i].key_index = params->wep_info[i].key_index;
            local_para_ptr->wep_info[i].key_length = params->wep_info[i].key_length;
            kal_mem_cpy(&(local_para_ptr->wep_info[i].key_material[0]), &(params->wep_info[i].key_material[0]), params->wep_info[i].key_length);
        }
        local_para_ptr->key_enable = params->wep_enabled;
    } else {
        local_para_ptr->key_enable = 0;
    }
//  wpa_printf(TRACE_GROUP_1, "session_id=%d, network_type=%d, channel_number=%d, auth_mode=%d, encryption=%d, key_enable=%d, ssid_len=%d\n"
//      , local_para_ptr->session_id, local_para_ptr->network_type, local_para_ptr->channel_number, local_para_ptr->auth_mode, local_para_ptr->encryption,
//      local_para_ptr->key_enable, local_para_ptr->ssid_len);
    supc_send_wndrv_join_network_req((local_para_struct *)local_para_ptr);

    return KAL_TRUE;
    //return wpa_driver_mtk_set_ssid(drv, params->ssid, params->ssid_len);
}


int wpa_driver_mtk_set_auth_alg(void *priv, int auth_alg)
{
    // wpa_printf(MSG_DEBUG,"wpa_driver_mtk_set_auth_alg - driver handle this \n");

    return 0;
}


int wpa_driver_mtk_remove_key(struct wpa_driver_mtk_data *drv,
                              int key_idx, const u8 *addr,
                              const u8 *bssid, int pairwise)
{
    ilm_struct *ilm_ptr = NULL;
    wndrv_supc_remove_key_req_struct *local_para_ptr = NULL;
    /* TEST TEST */
    //wndrv_supc_remove_wep_req_struct *local_para_2_ptr = NULL;

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_remove_key_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_remove_key_req_struct), TD_RESET);

    // kal_mem_set(&(local_para_ptr->key_index), 0, sizeof(kal_uint32));
    local_para_ptr->key_index = key_idx;

    if (pairwise) {
        local_para_ptr->key_index |= 1 << 30;
    }

    kal_mem_cpy(&(local_para_ptr->bssid[0]), bssid, WNDRV_MAC_ADDRESS_LEN);

    ilm_ptr = allocate_ilm(MAP_MOD_SUPC);

    ilm_ptr->msg_id = MAP_MSG_ID_WNDRV_SUPC_REMOVE_KEY_REQ;//MSG_ID_WNDRV_SUPC_REMOVE_KEY_REQ;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    SEND_ILM(MAP_MOD_SUPC, MAP_MOD_WNDRV, MAP_WNDRV_SUPC_SAP, ilm_ptr);

    /* TEST TEST */
    /*
    if(!pairwise)
    {
        ilm_ptr = allocate_ilm(MOD_SUPC);
        local_para_2_ptr = construct_local_para(sizeof(wndrv_supc_remove_wep_req_struct), TD_UL);

        ilm_ptr->msg_id = MSG_ID_WNDRV_SUPC_REMOVE_WEP_REQ;
        ilm_ptr->local_para_ptr = (local_para_struct *)local_para_2_ptr;
        local_para_2_ptr->key_index = key_idx;

        SEND_ILM(MOD_SUPC, MOD_WNDRV, WNDRV_SUPC_SAP, ilm_ptr);
    }
    */

    return KAL_TRUE;

}


int wpa_driver_mtk_set_key(void *priv, wpa_alg alg,
                           const u8 *addr, int key_idx,
                           int set_tx, const u8 *seq, wpa_size_t seq_len,
                           const u8 *key, wpa_size_t key_len)
{
    //wpa_size_t len;
    wndrv_supc_add_key_req_struct *local_para_ptr = NULL;
    int i, pairwise;
    u8 bssid[ETH_ALEN];

    /* Joseph: In our solution, we have to such "broadcase" case to remove key.
     * We have one WPA ent only so the bssid shall be specified otherwise get it
     * from the WPA context
     */
    if (addr == NULL || kal_mem_cmp(addr, "\xff\xff\xff\xff\xff\xff",
                                    ETH_ALEN) == 0) {
        /* Group Key */
        pairwise = 0;
        //driver_get_bssid(drv, bssid);
        if (wpa_s->key_mgmt != WPA_KEY_MGMT_WPA_NONE) {
            kal_mem_cpy(bssid, wpa_s->bssid, ETH_ALEN);
        } else {
            kal_mem_cpy(bssid, "\xff\xff\xff\xff\xff\xff", ETH_ALEN);
        }
    } else {
        /* Pairwise Key */
        pairwise = 1;
        kal_mem_cpy(bssid, addr, ETH_ALEN);
    }

    /* Joseph, WEP keys will be removed by WNDRV, and the the keys shall be send along with
     * the network join request
     */
    /*
        if (alg == WPA_ALG_NONE || key_len == 0)
        {
            return wpa_driver_mtk_remove_key(drv, key_idx, addr, bssid,
                          pairwise);
        }
        if (alg == WPA_ALG_WEP)
        {
            return wpa_driver_mtk_add_wep(drv, pairwise, key_idx, set_tx,
                           key, key_len);
            }
        */

    // len = 12 + 6 + 6 + 8 + key_len;

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_add_key_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_add_key_req_struct), TD_RESET);

    // kal_mem_set(local_para_ptr, 0, sizeof(wndrv_supc_add_key_req_struct));   // Karen

    local_para_ptr->key_length = key_len;
    local_para_ptr->key_index = key_idx;
    if (set_tx) {
        local_para_ptr->key_index |= (kal_uint32)1 << 31;
    }
    if (pairwise) {
        local_para_ptr->key_index |= 1 << 30;
    }
    if (seq && seq_len) {
        local_para_ptr->key_index |= 1 << 29;
    }

    kal_mem_cpy(local_para_ptr->bssid, bssid, ETH_ALEN);

    local_para_ptr->key_rsc[0] = 0;
    local_para_ptr->key_rsc[1] = 0;

    if (seq && seq_len) {
        for (i = 0; i < seq_len; i++) {
            if (i < 4) {
                local_para_ptr->key_rsc[0] |= (seq[i] << (i * 8));
            } else if (i < 8) {
                local_para_ptr->key_rsc[1] |= (seq[i] << ((i - 4) * 8));
            } else {
                SUPC_ASSERT(0);
            }
        }
    }

    if (alg == WPA_ALG_TKIP && key_len == 32) {
        kal_mem_cpy(local_para_ptr->key_material, key, 16);
        kal_mem_cpy(local_para_ptr->key_material + 16, key + 24, 8);
        kal_mem_cpy(local_para_ptr->key_material + 24, key + 16, 8);
    } else {
        kal_mem_cpy(local_para_ptr->key_material, key, key_len);
    }


    //ret = driver_set_add_key(drv, nkey, len);

    supc_send_wndrv_add_key_req((local_para_struct *)local_para_ptr);

    return KAL_TRUE;
}


void *wpa_driver_mtk_init(void *ctx, const char *ifname, wndrv_supc_param_capability_struct *capability)
{
    struct wpa_driver_mtk_data *drv;
    drv = SUPC_ALLOC_BUFF(sizeof(*drv));

    kal_mem_set(drv, 0, sizeof(*drv));
    drv->ctx = ctx;
    strncpy(drv->ifname, ifname, sizeof(drv->ifname));

    wpa_driver_mtk_get_capability(drv, capability);

    /* Make sure that the driver does not have any obsolete PMKID entries.
     */
    wpa_driver_mtk_flush_pmkid(drv);

    /* Set mode here in case card was configured for ad-hoc mode
     * previously. */
#if 0 // Need to discuss with JC
    mode = Ndis802_11Infrastructure;
    if (iSetOID(drv, OID_802_11_INFRASTRUCTURE_MODE,
                (char *) &mode, sizeof(mode)) < 0) {
        wpa_printf(MSG_DEBUG, "NDIS: Failed to set "
                   "OID_802_11_INFRASTRUCTURE_MODE (%d)",
                   (int) mode);
        /* Try to continue anyway */
    }
#endif

    return drv;
}


// Joseph Wifi Modification
static void wpa_driver_mtk_deinit(void *priv)
{
    struct wpa_driver_mtk_data *drv = priv;

    //close(drv->event_sock);
    //close(drv->ioctl_sock);
    SUPC_DEALLOC_BUFF(drv);
}

int wpa_driver_mtk_add_pmkid(void *priv, const u8 *bssid,
                             const u8 *pmkid)
{
    struct wpa_driver_mtk_data *drv = priv;
    struct pmkid_entry *entry, *prev;

    if (drv->no_of_pmkid == 0) {
        return 0;
    }
    prev = NULL;
    entry = drv->pmkid;
    while (entry) {
        if (memcmp(entry->bssid, bssid, ETH_ALEN) == 0) {
            break;
        }
        prev = entry;
        entry = entry->next;
    }

    if (entry) {
        /* Replace existing entry for this BSSID and move it into the
         * beginning of the list. */
        memcpy(entry->pmkid, pmkid, 16);
        if (prev) {
            prev->next = entry->next;
            entry->next = drv->pmkid;
            drv->pmkid = entry;
        }
    } else {
        entry = SUPC_ALLOC_BUFF(sizeof(*entry));
        if (entry) {
            memcpy(entry->bssid, bssid, ETH_ALEN);
            memcpy(entry->pmkid, pmkid, 16);
            entry->next = drv->pmkid;
            drv->pmkid = entry;
        }
    }
    return wpa_driver_mtk_set_pmkid(drv);
}


int wpa_driver_mtk_remove_pmkid(void *priv, const u8 *bssid,
                                const u8 *pmkid)
{
    struct wpa_driver_mtk_data *drv = priv;
    struct pmkid_entry *entry, *prev;

    if (drv->no_of_pmkid == 0) {
        return 0;
    }

    entry = drv->pmkid;
    prev = NULL;
    //drv->pmkid = NULL; //kinki add
    while (entry) {
        if (memcmp(entry->bssid, bssid, ETH_ALEN) == 0 &&
                memcmp(entry->pmkid, pmkid, 16) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                drv->pmkid = entry->next;
            }
            SUPC_DEALLOC_BUFF(entry);
            break;
        }
        prev = entry;
        entry = entry->next;
    }
    return wpa_driver_mtk_set_pmkid(drv);
}


int wpa_driver_mtk_flush_pmkid(void *priv)
{

    struct wpa_driver_mtk_data *drv = priv;
    NDIS_802_11_PMKID p;
    struct pmkid_entry *pmkid, *prev;

    if (drv->no_of_pmkid == 0) {
        return 0;
    }

    pmkid = drv->pmkid;
    drv->pmkid = NULL;
    while (pmkid) {
        prev = pmkid;
        pmkid = pmkid->next;
        SUPC_DEALLOC_BUFF(prev);
    }

    memset(&p, 0, sizeof(p));
    p.Length = 8;
    p.BSSIDInfoCount = 0;
    //wpa_hexdump(MSG_MSGDUMP, "OID_802_11_PMKID (flush)",
    //(char *) &p, 8);
//  kal_buffer_trace(DBG_INIT,DUMP_SUPC_802_11_PMKID,8,(kal_uint8*)(&p));
    return wpa_driver_mtk_set_pmkid(drv);

#if 0
    struct wpa_driver_mtk_data *drv = priv;
    NDIS_802_11_PMKID p;
    struct pmkid_entry *pmkid, *prev;

    if (drv->no_of_pmkid == 0) {
        return 0;
    }

    pmkid = drv->pmkid;
    drv->pmkid = NULL;
    while (pmkid) {
        prev = pmkid;
        pmkid = pmkid->next;
        MEM_FREE(prev);
    }

    memset(&p, 0, sizeof(p));
    p.Length = 8;
    p.BSSIDInfoCount = 0;
    wpa_hexdump(MSG_MSGDUMP, "OID_802_11_PMKID (flush)",
                (char *) &p, 8);
    return wpa_driver_mtk_set_pmkid(drv);
#endif
}


int wpa_driver_mtk_set_pmkid(struct wpa_driver_mtk_data *drv)
{
    struct pmkid_entry *entry;
    wndrv_supc_pmkid_update_req_struct *local_para_ptr = NULL;

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_pmkid_update_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_pmkid_update_req_struct), TD_RESET);

    local_para_ptr->num = 0;
    entry = drv->pmkid;

    while (entry) {
        kal_mem_cpy(local_para_ptr->pmkid_param[local_para_ptr->num].bssid, entry->bssid, WNDRV_MAC_ADDRESS_LEN);
        kal_mem_cpy(local_para_ptr->pmkid_param[local_para_ptr->num].pmkid, entry->pmkid, 16);
        local_para_ptr->num++;
        if (local_para_ptr->num >= drv->no_of_pmkid) {
            break;
        }
        entry = entry->next;
    }

    //wpa_hexdump(MSG_MSGDUMP, "NDIS: OID_802_11_PMKID", (char *) entry->pmkid, len);


    supc_send_wndrv_update_pmkid_req((local_para_struct *)local_para_ptr);

    return KAL_TRUE;
}

#if 0
/* Called when driver calls NdisMIndicateStatus() with
 * NDIS_STATUS_MEDIA_SPECIFIC_INDICATION */
void wpa_driver_mtk_event_media_specific(struct wpa_driver_mtk_data *drv,
        const u8 *data, wpa_size_t data_len)
{
    NDIS_802_11_STATUS_INDICATION *status;

    if (data == NULL || data_len < sizeof(*status)) {
        return;
    }

    //wpa_hexdump(MSG_DEBUG, "NDIS: Media Specific Indication",
    //      data, data_len);

    status = (NDIS_802_11_STATUS_INDICATION *) data;
    data += sizeof(status);
    data_len -= sizeof(status);

    switch (status->StatusType) {
        case Ndis802_11StatusType_Authentication:
            //wpa_driver_mtk_event_auth(drv, data, data_len);
            break;
        case Ndis802_11StatusType_PMKID_CandidateList:
            wpa_driver_mtk_event_pmkid(drv, data, data_len);
            break;
        default:
            wpa_printf(MSG_DEBUG, "NDIS: Unknown StatusType %d",
                       (int) status->StatusType);
            break;
    }
}

static void wpa_driver_mtk_event_pmkid(struct wpa_driver_mtk_data *drv,
                                       const u8 *data, wpa_size_t data_len)
{
    NDIS_802_11_PMKID_CANDIDATE_LIST *pmkid;
    int i;
    union wpa_event_data event;

    if (data_len < 8) {
        wpa_printf(MSG_DEBUG, "NDIS: Too short PMKID Candidate List "
                   "Event (len=%d)", data_len);
        return;
    }
    pmkid = (NDIS_802_11_PMKID_CANDIDATE_LIST *) data;
    wpa_printf(MSG_DEBUG, "NDIS: PMKID Candidate List Event - Version %d "
               "NumCandidates %d",
               (int) pmkid->Version, (int) pmkid->NumCandidates);

    if (pmkid->Version != 1) {
        wpa_printf(MSG_DEBUG, "NDIS: Unsupported PMKID Candidate List "
                   "Version %d", (int) pmkid->Version);
        return;
    }

    if (data_len < 8 + pmkid->NumCandidates * sizeof(PMKID_CANDIDATE)) {
        wpa_printf(MSG_DEBUG, "NDIS: PMKID Candidate List underflow");
        return;
    }

    memset(&event, 0, sizeof(event));
    for (i = 0; i < pmkid->NumCandidates; i++) {
        PMKID_CANDIDATE *p = &pmkid->CandidateList[i];
        wpa_printf(MSG_DEBUG, "NDIS: %d: " MACSTR " Flags 0x%x",
                   i, MAC2STR(p->BSSID), (int) p->Flags);
        memcpy(event.pmkid_candidate.bssid, p->BSSID, ETH_ALEN);
        event.pmkid_candidate.index = i;
        event.pmkid_candidate.preauth =
            p->Flags & NDIS_802_11_PMKID_CANDIDATE_PREAUTH_ENABLED;
        wpa_supplicant_event(drv->ctx, EVENT_PMKID_CANDIDATE,
                             &event);
    }
}


static int wpa_driver_mtk_get_associnfo(struct wpa_driver_mtk_data *drv)
{
    char buf[512];
    NDIS_802_11_ASSOCIATION_INFORMATION *ai;
    int len;
    union wpa_event_data data;

    len = iGetOID(drv, OID_802_11_ASSOCIATION_INFORMATION, buf,
                  sizeof(buf));
    if (len < 0) {
        wpa_printf(MSG_DEBUG, "NDIS: failed to get association "
                   "information");
        return -1;
    }
    if (len > sizeof(buf)) {
        /* Some drivers seem to be producing incorrect length for this
         * data. Limit the length to the current buffer size to avoid
         * crashing in hexdump. The data seems to be otherwise valid,
         * so better try to use it. */
        wpa_printf(MSG_DEBUG, "NDIS: ignored bogus association "
                   "information length %d", len);
        len = iGetOID(drv, OID_802_11_ASSOCIATION_INFORMATION,
                      buf, sizeof(buf));
        if (len < -1) {
            wpa_printf(MSG_DEBUG, "NDIS: re-reading association "
                       "information failed");
            return -1;
        }
        if (len > sizeof(buf)) {
            wpa_printf(MSG_DEBUG, "NDIS: ignored bogus association"
                       " information length %d (re-read)", len);
            len = sizeof(buf);
        }
    }
    wpa_hexdump(MSG_MSGDUMP, "NDIS: association information", buf, len);
    if (len < sizeof(*ai)) {
        wpa_printf(MSG_DEBUG, "NDIS: too short association "
                   "information");
        return -1;
    }
    ai = (NDIS_802_11_ASSOCIATION_INFORMATION *) buf;
    wpa_printf(MSG_MSGDUMP, "NDIS: ReqFixed=0x%x RespFixed=0x%x off_req=%d "
               "off_resp=%d len_req=%d len_resp=%d",
               ai->AvailableRequestFixedIEs, ai->AvailableResponseFixedIEs,
               (int) ai->OffsetRequestIEs, (int) ai->OffsetResponseIEs,
               (int) ai->RequestIELength, (int) ai->ResponseIELength);

    if (ai->OffsetRequestIEs + ai->RequestIELength > len ||
            ai->OffsetResponseIEs + ai->ResponseIELength > len) {
        wpa_printf(MSG_DEBUG, "NDIS: association information - "
                   "IE overflow");
        return -1;
    }

    wpa_hexdump(MSG_MSGDUMP, "NDIS: Request IEs",
                buf + ai->OffsetRequestIEs, ai->RequestIELength);
    wpa_hexdump(MSG_MSGDUMP, "NDIS: Response IEs",
                buf + ai->OffsetResponseIEs, ai->ResponseIELength);

    memset(&data, 0, sizeof(data));
    data.assoc_info.req_ies = (u8 *)buf + ai->OffsetRequestIEs;
    data.assoc_info.req_ies_len = ai->RequestIELength;
    data.assoc_info.resp_ies = (u8 *)buf + ai->OffsetResponseIEs;
    data.assoc_info.resp_ies_len = ai->ResponseIELength;
    wpa_supplicant_event(drv->ctx, EVENT_ASSOCINFO, &data);

    return 0;
}

static void
wpa_driver_mtk_get_wpa_capability(struct wpa_driver_mtk_data *drv)
{
    wpa_printf(MSG_DEBUG, "NDIS: verifying driver WPA capability");

    if (driver_set_auth_mode(drv, Ndis802_11AuthModeWPA) == 0 &&
            driver_get_auth_mode(drv) == Ndis802_11AuthModeWPA) {
        wpa_printf(MSG_DEBUG, "NDIS: WPA key management supported");
        drv->capa.key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA;
    }

    if (driver_set_auth_mode(drv, Ndis802_11AuthModeWPAPSK) == 0 &&
            driver_get_auth_mode(drv) == Ndis802_11AuthModeWPAPSK) {
        wpa_printf(MSG_DEBUG, "NDIS: WPA-PSK key management "
                   "supported");
        drv->capa.key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA_PSK;
    }

    if (driver_set_encr_status(drv, Ndis802_11Encryption3Enabled) == 0 &&
            driver_get_encr_status(drv) == Ndis802_11Encryption3KeyAbsent) {
        wpa_printf(MSG_DEBUG, "NDIS: CCMP encryption supported");
        drv->capa.enc |= WPA_DRIVER_CAPA_ENC_CCMP;
    }

    if (driver_set_encr_status(drv, Ndis802_11Encryption2Enabled) == 0 &&
            driver_get_encr_status(drv) == Ndis802_11Encryption2KeyAbsent) {
        wpa_printf(MSG_DEBUG, "NDIS: TKIP encryption supported");
        drv->capa.enc |= WPA_DRIVER_CAPA_ENC_TKIP;
    }

    if (driver_set_encr_status(drv, Ndis802_11Encryption1Enabled) == 0 &&
            driver_get_encr_status(drv) == Ndis802_11Encryption1KeyAbsent) {
        wpa_printf(MSG_DEBUG, "NDIS: WEP encryption supported");
        drv->capa.enc |= WPA_DRIVER_CAPA_ENC_WEP40 |
                         WPA_DRIVER_CAPA_ENC_WEP104;
    }

    if (driver_set_auth_mode(drv, Ndis802_11AuthModeShared) == 0 &&
            driver_get_auth_mode(drv) == Ndis802_11AuthModeShared) {
        drv->capa.auth |= WPA_DRIVER_AUTH_SHARED;
    }

    if (driver_set_auth_mode(drv, Ndis802_11AuthModeOpen) == 0 &&
            driver_get_auth_mode(drv) == Ndis802_11AuthModeOpen) {
        drv->capa.auth |= WPA_DRIVER_AUTH_OPEN;
    }

    driver_set_encr_status(drv, Ndis802_11EncryptionDisabled);

    /* Could also verify OID_802_11_ADD_KEY error reporting and
     * support for OID_802_11_ASSOCIATION_INFORMATION. */

    if (drv->capa.key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_WPA &&
            drv->capa.enc & (WPA_DRIVER_CAPA_ENC_TKIP |
                             WPA_DRIVER_CAPA_ENC_CCMP)) {
        wpa_printf(MSG_DEBUG, "NDIS: driver supports WPA");
        drv->has_capability = 1;
    } else {
        wpa_printf(MSG_DEBUG, "NDIS: no WPA support found");
    }

    wpa_printf(MSG_DEBUG, "NDIS: driver capabilities: key_mgmt 0x%x "
               "enc 0x%x auth 0x%x",
               drv->capa.key_mgmt, drv->capa.enc, drv->capa.auth);
}
#endif

static void wpa_driver_mtk_get_capability(struct wpa_driver_mtk_data *drv, wndrv_supc_param_capability_struct *capability)
{

    kal_uint16 i = 0;

    drv->capa.flags = WPA_DRIVER_FLAGS_DRIVER_IE |
                      WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC;

    drv->has_capability = 1;
    drv->no_of_pmkid = capability->num_pmkids;
    for (i = 0; i < capability->num_auth_encry_pairs_supported; i++) {
        wndrv_supc_auth_encry_struct *ae;
        ae = &(capability->auth_encry_supported[i]);

//      kal_trace(DBG_INIT, INFO_SUPC_NDIS_AUTH_MODE,i, (int) ae->auth_mode,(int) ae->encryption);
        switch (ae->auth_mode) {
            case WNDRV_SUPC_AUTH_MODE_OPEN:
                drv->capa.auth |= WPA_DRIVER_AUTH_OPEN;
                break;
            case WNDRV_SUPC_AUTH_MODE_SHARED:
                drv->capa.auth |= WPA_DRIVER_AUTH_SHARED;
                break;
            case WNDRV_SUPC_AUTH_MODE_WPA:
                drv->capa.key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA;
                break;
            case WNDRV_SUPC_AUTH_MODE_WPA_PSK:
                drv->capa.key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA_PSK;
                break;
            case WNDRV_SUPC_AUTH_MODE_WPA2:
                drv->capa.key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA2;
                break;
            case WNDRV_SUPC_AUTH_MODE_WPA2_PSK:
                drv->capa.key_mgmt |=
                    WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK;
                break;
            case WNDRV_SUPC_AUTH_MODE_WPA_NONE:
                drv->capa.key_mgmt |=
                    WPA_DRIVER_CAPA_KEY_MGMT_WPA_NONE;
                break;
            default:
                break;
        }
        switch (ae->encryption) {
            case WNDRV_SUPC_ENCRYPTION1_ENABLED:
                drv->capa.enc |= WPA_DRIVER_CAPA_ENC_WEP40;
                drv->capa.enc |= WPA_DRIVER_CAPA_ENC_WEP104;
                break;
            case WNDRV_SUPC_ENCRYPTION2_ENABLED:
                drv->capa.enc |= WPA_DRIVER_CAPA_ENC_TKIP;
                break;
            case WNDRV_SUPC_ENCRYPTION3_ENABLED:
                drv->capa.enc |= WPA_DRIVER_CAPA_ENC_CCMP;
                break;
            default:
                break;
        }
    }

//  kal_trace(DBG_INIT,INFO_SUPC_NDIS_KEY_MNG, drv->capa.key_mgmt, drv->capa.enc, drv->capa.auth);
}

int wpa_driver_mtk_get_capa(void *priv, struct wpa_driver_capa *capa)
{
    struct wpa_driver_mtk_data *drv = priv;
    if (!drv->has_capability) {
        return -1;
    }
    kal_mem_cpy(capa, &drv->capa, sizeof(*capa));
    return 0;
}


struct wpa_driver_ops wpa_driver_mtk_ops = {
    "mtk",
    "MediaTek Wireless LAN",
    wpa_driver_mtk_get_bssid,
    wpa_driver_mtk_get_ssid,
    wpa_driver_mtk_set_wpa,
    wpa_driver_mtk_set_key,
    NULL,//wpa_driver_mtk_init,
    wpa_driver_mtk_deinit,
    wpa_driver_mtk_set_countermeasures,
    wpa_driver_mtk_set_drop_unencrypted,
    NULL,//wpa_driver_mtk_set_scan,
    NULL,//wpa_driver_mtk_get_scan_results,
    wpa_driver_mtk_deauthenticate,
    wpa_driver_mtk_disassociate,
    wpa_driver_mtk_associate,
    wpa_driver_mtk_set_auth_alg,
    wpa_driver_mtk_add_pmkid,
    wpa_driver_mtk_remove_pmkid,
    wpa_driver_mtk_flush_pmkid,
    wpa_driver_mtk_get_capa,
};

void wpa_supplicant_fd_workaround(void)
{
    return ;
}

