/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#ifndef __WIFI_HOST_DEFAULT_CONFIG_H__
#define __WIFI_HOST_DEFAULT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#pragma pack(push, 1)

#define WIFI_DEFAULT_OPMODE                 ("1")
#define WIFI_DEFAULT_COUNTRY_CODE           ("TW")
#define WIFI_DEFAULT_COUNTRY_REGION         ("0")
#define WIFI_DEFAULT_COUNTRY_REGION_A_BAND  ("8")
#define WIFI_DEFAULT_RADIO_ONOFF            ("0")
#define WIFI_DEFAULT_N9_DEBUG_LEVEL         ("3")
#define WIFI_DEFAULT_RTS_THRESHOLD          ("2347")
#define WIFI_DEFAULT_FRAGMENT_THRESHOLD     ("2346")
#define WIFI_DEFAULT_BG_CHANNEL_TABLE       ("1,14,0|")    //first_channel=1, num_of_ch=14,channel_prop=0(active)
#define WIFI_DEFAULT_A_CHANNEL_TABLE        ("36,8,0|100,11,0|149,4,0|")
#define WIFI_DEFAULT_SYSLOG_FILTERS         ("")
#ifdef MTK_WIFI_PRIVILEGE_ENABLE
#define WIFI_DEFAULT_WIFI_PRIVILEGE_ENABLE  ("1")
#else
#define WIFI_DEFAULT_WIFI_PRIVILEGE_ENABLE  ("0")
#endif

#define WIFI_DEFAULT_STA_FAST_LINK          ("0")
#define WIFI_DEFAULT_STA_LOCAL_ADMIN_MAC    ("1")
#define WIFI_DEFAULT_STA_MAC_ADDR           ("00:0c:59:32:01:02")
#define WIFI_DEFAULT_STA_SSID               ("MTK_SOFT_AP")
#define WIFI_DEFAULT_STA_SSID_LEN           ("11")
#define WIFI_DEFAULT_STA_BSS_TYPE           ("1")
#define WIFI_DEFAULT_STA_CHANNEL            ("1")
#define WIFI_DEFAULT_STA_BANDWIDTH          ("0")
#define WIFI_DEFAULT_STA_WIRELESS_MODE      ("9")
#define WIFI_DEFAULT_STA_BA_DECLINE         ("0")
#define WIFI_DEFAULT_STA_AUTO_BA            ("1")
#define WIFI_DEFAULT_STA_HT_MCS             ("33")
#define WIFI_DEFAULT_STA_HT_BA_WINDOW_SIZE  ("4")
#define WIFI_DEFAULT_STA_HT_GI              ("1")
#define WIFI_DEFAULT_STA_HT_PROTECT         ("1")
#define WIFI_DEFAULT_STA_HT_EXT_CHANNEL     ("1")
#define WIFI_DEFAULT_STA_WMM_CAPABLE        ("1")
#define WIFI_DEFAULT_STA_LISTEN_INTERVAL    ("1")
#define WIFI_DEFAULT_STA_AUTH_MODE          ("0")
#define WIFI_DEFAULT_STA_ENCRYPT_TYPE       ("1")
#define WIFI_DEFAULT_STA_WPA_PSK            ("12345678")
#define WIFI_DEFAULT_STA_WPA_PSK_LEN        ("8")
#define WIFI_DEFAULT_STA_PAIR_CIPHER        ("0")
#define WIFI_DEFAULT_STA_GROUP_CIPHER       ("0")
#define WIFI_DEFAULT_STA_DEFAULT_KEY_ID     ("0")
#define WIFI_DEFAULT_STA_SHARE_KEY          ("12345,12345,12345,12345")
#define WIFI_DEFAULT_STA_SHARE_KEY_LEN      ("5,5,5,5")
#define WIFI_DEFAULT_STA_POWER_SAVE_MODE    ("0")
#define WIFI_DEFAULT_STA_KEEP_ALIVE_PERIOD  ("55")
#define WIFI_DEFAULT_STA_BEACON_LOST_TIME   ("2")
#define WIFI_DEFAULT_APCLI_BW_AUTO_UP_BELOW ("1")
#define WIFI_DEFAULT_STA_KEEP_ALIVE_PACKET  ("1")


#define WIFI_DEFAULT_AP_LOCAL_ADMIN_MAC     ("1")
#define WIFI_DEFAULT_AP_MAC_ADDR            ("00:0c:59:32:01:03")
#define WIFI_DEFAULT_AP_SSID                ("MTK_SOFT_AP")
#define WIFI_DEFAULT_AP_SSID_LEN            ("11")
#define WIFI_DEFAULT_AP_CHANNEL             ("1")
#define WIFI_DEFAULT_AP_BANDWIDTH           ("0")
#define WIFI_DEFAULT_AP_WIRELESS_MODE       ("9")
#define WIFI_DEFAULT_AP_AUTO_BA             ("1")
#define WIFI_DEFAULT_AP_HT_MCS              ("33")
#define WIFI_DEFAULT_AP_HT_BA_WINDOW_SIZE   ("4")
#define WIFI_DEFAULT_AP_HT_GI               ("1")
#define WIFI_DEFAULT_AP_HT_PROTECT          ("1")
#define WIFI_DEFAULT_AP_HT_EXT_CHANNEL      ("1")
#define WIFI_DEFAULT_AP_WMM_CAPABLE         ("1")
#define WIFI_DEFAULT_AP_DTIM_PERIOD         ("1")
#define WIFI_DEFAULT_AP_AUTH_MODE           ("0")
#define WIFI_DEFAULT_AP_ENCRYPT_TYPE        ("1")
#define WIFI_DEFAULT_AP_WPA_PSK             ("12345678")
#define WIFI_DEFAULT_AP_WPA_PSK_LEN         ("8")
#define WIFI_DEFAULT_AP_PAIR_CIPHER         ("0")
#define WIFI_DEFAULT_AP_GROUP_CIPHER        ("0")
#define WIFI_DEFAULT_AP_DEFAULT_KEY_ID      ("0")
#define WIFI_DEFAULT_AP_SHARE_KEY           ("11111,22222,33333,44444")
#define WIFI_DEFAULT_AP_SHARE_KEY_LEN       ("5,5,5,5")
#define WIFI_DEFAULT_AP_HIDDEN_SSID         ("0")
#define WIFI_DEFAULT_AP_REKEY_INTERVAL      ("3600")
#define WIFI_DEFAULT_AP_AUTO_CHANNEL_SELECT ("0")
#define WIFI_DEFAULT_AP_BEACON_DISABLE      ("0")

#define WIFI_DEFAULT_MBSS_ENABLE            ("0")
#define WIFI_DEFAULT_MBSS_SSID1             ("MTK_MBSS1")
#define WIFI_DEFAULT_MBSS_SSID2             ("MTK_MBSS2")

#ifdef MTK_WIFI_WPS_ENABLE
#define WIFI_DEFAULT_MANUFACTURER           ("MTK")
#define WIFI_DEFAULT_MODEL_NAME             ("MTK Wireless Device")
#define WIFI_DEFAULT_MODEL_NUMBER           ("MT7687")
#define WIFI_DEFAULT_SERIAL_NUMBER          ("12345678")
#define WIFI_DEFAULT_DEVICE_NAME            ("MTK IoT")
#endif

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
#define WIFI_DEFAULT_CONFIG_FREE_READY      ("0")
#define WIFI_DEFAULT_CONFIG_FREE_ENABLE     ("0")
#endif

#ifdef MTK_SINGLE_SKU_SUPPORT
#define WIFI_DEFAULT_SUPPORT_SINGLE_SKU ("0")    //0:not support single SKU, 1:support 2G SKU, 2:support 5G SKU, 3:support 2G and 5G SKU

#define WIFI_DEFAULT_SKU_2G_GROUP0  \
("15,15,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,16,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,17,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
19,19,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,15,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,16,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,17,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
19,19,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,15,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,16,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,17,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19")

#define WIFI_DEFAULT_SKU_5G_L0 \
"11,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
12,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
13,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
11,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
12,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
13,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
11,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
12,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
13,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
11,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
12,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
13,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
11,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
12,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19"

#define WIFI_DEFAULT_SKU_5G_M0 \
"14,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
14,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
14,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
14,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
16,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
14,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
15,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19"

#define WIFI_DEFAULT_SKU_5G_H0 \
"17,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
19,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
19,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
19,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
19,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
17,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19,\
18,18,18,18,18,17,17,17,17,17,17,19,19,19,19,19,19"


#define WIFI_DEFAULT_SKU_5G_GROUP0  (WIFI_DEFAULT_SKU_5G_L0","WIFI_DEFAULT_SKU_5G_M0","WIFI_DEFAULT_SKU_5G_H0)

#endif /*MTK_SINGLE_SKU_SUPPORT*/

#ifdef MTK_ANT_DIV_ENABLE
#define WIFI_DEFAULT_ANT_NUMBER             ("0")
#define WIFI_DEFAULT_ANT_DIVERSITY_ENABLE   ("0")
#define WIFI_DEFAULT_ANT_PROB_REQ_COUNT     ("3")
#define WIFI_DEFAULT_ANT_THRESHOLD_LEVEL    ("-50")
#define WIFI_DEFAULT_ANT_DELTA_RSSI         ("5")
#define WIFI_DEFAULT_ANT_RSSI_SWITCH        ("5")
#define WIFI_DEFAULT_ANT_PRE_SELECTED_RSSI  ("-110")
#endif

#define WIFI_2G_CHANNEL_NUMBER            (14)    //channel 1 ~14
#define WIFI_2G_SKU_POWER_PER_CHANNEL     (19)
#define WIFI_5G_SKU_POWER_PER_CHANNEL     (17)
typedef struct {
    unsigned char first_channel;
    unsigned char num_of_ch;
    unsigned char channel_prop; //0: Active, 1: Passive
    unsigned char tx_power; // 4-align and could be used to extend channel_prop
} ch_desc_t;

typedef struct {
    uint8_t singleSKU2G[WIFI_2G_CHANNEL_NUMBER][WIFI_2G_SKU_POWER_PER_CHANNEL]; // 14 2G_channel  * (2+5+6+6) power per channel
} singleSKU2G_t;

typedef struct {
#if 0
    uint8_t singleSKU5G[42][17]; // 42 5G_channel  * (5+6+6) power per channel
#else
    uint8_t singleSKU5G_ch36[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch38[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch40[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch42[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch44[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch46[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch48[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch52[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch54[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch56[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch58[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch60[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch62[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch64[WIFI_5G_SKU_POWER_PER_CHANNEL];

    uint8_t singleSKU5G_ch100[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch102[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch104[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch106[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch108[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch110[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch112[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch116[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch118[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch120[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch122[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch124[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch126[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch128[WIFI_5G_SKU_POWER_PER_CHANNEL];

    uint8_t singleSKU5G_ch132[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch134[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch136[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch140[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch149[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch151[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch153[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch155[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch157[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch159[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch161[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch165[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch169[WIFI_5G_SKU_POWER_PER_CHANNEL];
    uint8_t singleSKU5G_ch173[WIFI_5G_SKU_POWER_PER_CHANNEL];
#endif
} singleSKU5G_t;

typedef struct {
    uint8_t opmode;
    uint8_t country_region;
    uint8_t country_region_a_band;
    uint8_t country_code[4];
    uint8_t radio_off;
    uint8_t dbg_level;
    uint16_t rts_threshold;
    uint16_t frag_threshold;

    uint8_t sta_local_admin_mac;
    uint8_t sta_ip_addr[4];
    uint8_t sta_mac_addr[6];
    uint8_t sta_ssid[32];
    uint8_t sta_ssid_len;
    uint8_t sta_bss_type;
    uint8_t sta_channel;
    uint8_t sta_bw;
    uint8_t sta_wireless_mode;
    uint8_t sta_ba_decline;
    uint8_t sta_auto_ba;
    uint8_t sta_ht_mcs;
    uint8_t sta_ht_ba_win_size;
    uint8_t sta_ht_gi;
    uint8_t sta_ht_protect;
    uint8_t sta_ht_ext_ch;
    uint8_t sta_wmm_capable;
    uint8_t sta_listen_interval;
    uint8_t sta_auth_mode;
    uint8_t sta_encryp_type;
    uint8_t sta_wpa_psk[64];
    uint8_t sta_wpa_psk_len;
    uint8_t sta_pmk[32];
    uint8_t sta_pair_cipher;
    uint8_t sta_group_cipher;
    uint8_t sta_default_key_id;
    uint8_t sta_ps_mode;
    uint8_t sta_keep_alive_period;

    uint8_t ap_local_admin_mac;
    uint8_t ap_ip_addr[4];
    uint8_t ap_mac_addr[6];
    uint8_t ap_ssid[32];
    uint8_t ap_ssid_len;
    uint8_t ap_channel;
    uint8_t ap_bw;
    uint8_t ap_wireless_mode;
    uint8_t ap_auto_ba;
    uint8_t ap_ht_mcs;
    uint8_t ap_ht_ba_win_size;
    uint8_t ap_ht_gi;
    uint8_t ap_ht_protect;
    uint8_t ap_ht_ext_ch;
    uint8_t ap_wmm_capable;
    uint8_t ap_dtim_period;
    uint8_t ap_hide_ssid;
    uint8_t ap_auto_channel_select;
    uint8_t ap_auth_mode;
    uint8_t ap_encryp_type;
    uint8_t ap_wpa_psk[64];
    uint8_t ap_wpa_psk_len;
    uint8_t ap_pmk[32];
    uint8_t ap_pair_cipher;
    uint8_t ap_group_cipher;
    uint8_t ap_default_key_id;

    // "scan channel table" and "regulatory table"
    unsigned char bg_band_entry_num;
    ch_desc_t bg_band_triple[10];
    unsigned char a_band_entry_num;
    ch_desc_t a_band_triple[10];
    uint8_t ap_beacon_disable;
    uint8_t forwarding_zero_copy;

    /* These are for MBSS support, but not exist trunk (it's customer feature), however,
            we have to add them here due to N9 FW has them (only one version of N9 FW)
         */
    // TODO: How to solve it in the future...Michael
    uint8_t mbss_enable;
    uint8_t mbss_ssid1[32];
    uint8_t mbss_ssid_len1;
    uint8_t mbss_ssid2[32];
    uint8_t mbss_ssid_len2;

    uint8_t config_free_ready;
    uint8_t config_free_enable;
    uint8_t sta_fast_link;
    uint8_t beacon_lost_time;
    uint8_t apcli_40mhz_auto_upbelow;
    uint8_t wifi_privilege_enable;
    uint8_t sta_keep_alive_packet;
    uint8_t support_single_sku;//0: not support SKU, 1:support 2G,  2:support 5G, 3:support both 2G and 5G
#if MTK_ANT_DIV_ENABLE
    //TODO, need N9 update the profile together
    uint8_t antenna_number;
    uint8_t antenna_diversity_enable;
    uint8_t antenna_probe_req_count;  //nunber of probe requests to send
    int8_t  antenna_threshold_level;  //rssi threshold level
    uint8_t antenna_delta_rssi;       //delta rssi
    uint8_t antenna_rssi_switch;      //rssi switch value
    int8_t  antenna_pre_selected_rssi;
    uint8_t reserved[141];
#else
    //Fixed profile should put in the place of reserved 
    uint8_t reserved[148];
#endif

    //SKU shoud fix form offset 0x280
#if 1 //def MTK_SINGLE_SKU_SUPPORT
    singleSKU2G_t single_sku_2g;
    singleSKU5G_t single_sku_5g;
#endif
} wifi_sys_cfg_t;

int32_t wifi_host_get_default_config(wifi_sys_cfg_t *syscfg);

#pragma pack(pop)


#ifdef __cplusplus
    }
#endif

#endif //__WIFI_HOST_DEFAULT_CONFIG_H__

