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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_host_default_config.h"
#include "host_os_utils.h"

static uint8_t BtoH(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return (ch - '0');    /* Handle numerals*/
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 0xA);    /* Handle capitol hex digits*/
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 0xA);    /* Handle small hex digits*/
    }
    return (255);
}

static void AtoH(char *src, char *dest, int destlen)
{
    char *srcptr;
    char *destTemp;

    srcptr = src;
    destTemp = (char *) dest;

    while (destlen--) {
        *destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
        *destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
        destTemp++;
    }
}
void wifi_host_conf_get_mac_from_str(char *mac_dst, const char *mac_src)
{
    unsigned int len = os_strlen(mac_src);

    if (len == 17) {
        //Mac address acceptable format 01:02:03:04:05:06 length 17
        int     j;
        char    *p = (char *)mac_src;
        for (j = 0; j < 6; j++) {
            AtoH(p, &mac_dst[j], 1);
            p += 3;
        }
    } else {
        printf("string format is wrong.. %s.\r\n", mac_src);
    }
}

static char *find_next_char(char *src, unsigned int src_len, char want, char replace)
{
    char            *ret = NULL;
    unsigned int    i;

    for (i = 0; i < src_len; i++) {
        if (want == (*(src + i))) {
            if (want != replace) {
                (*(src + i)) = replace;
            }

            return (src + i);
        }
    }

    return ret;
}

static int parse_ch_triplet(char *first_ch, char *ch_num, char *ch_prop, const char *str, unsigned int str_len)
{
    unsigned int    len = str_len;
    char            *ptr;
    char            *strr = (char *)str;

    ptr = find_next_char(strr, len, ',', ',');
    if (NULL == ptr) {
        return -1;
    }

    (*first_ch) = atoi(strr);
    len = len - ((ptr - strr) + 1);
    strr = ptr + 1;

    ptr = find_next_char(strr, len, ',', ',');
    if (NULL == ptr) {
        return -2;
    }

    (*ch_num) = atoi(strr);
    len = len - ((ptr - strr) + 1);
    strr = ptr + 1;

    (*ch_prop) = atoi(strr);

    return 0;
}

int wifi_conf_get_ch_table_from_str(ch_desc_t *ch_list, char ch_list_num, const char *ch_str, unsigned int ch_str_len)
{
    unsigned int    len = ch_str_len;
    char            *ptr;
    unsigned char   index;
    int             ch_num;

    if ((!ch_list) || (!ch_str)) {
        return -1;
    }

    index  = 0;
    ch_num = 0;

    do {
        ptr = find_next_char((char *)ch_str, len, '|', '|');
        if (NULL == ptr) {
            //return -1;
            break;
        }

        os_memset(&(ch_list[index]), 0x0, sizeof(*ch_list));

        parse_ch_triplet((char *) & (ch_list[index].first_channel),
                         (char *) & (ch_list[index].num_of_ch),
                         (char *) & (ch_list[index].channel_prop),
                         ch_str,
                         (ptr - ch_str));

        len    = len - ((ptr - ch_str) + 1);
        ch_str = ptr + 1;

        ch_num++;
        index++;
    } while (ch_num < ch_list_num);

    return ch_num;

}

#ifdef MTK_SINGLE_SKU_SUPPORT
void wifi_conf_get_pwr_from_str(uint16_t ret_len, void * single, const char *pwr_src)
{
    int     i   = 0;
    int     len = os_strlen(pwr_src);
    char    *dot_ptr = NULL;
    char    pwr_dst[714] = {0};

    //printf("[DEBUG]wifi_conf_get_pwr_from_str ret_len = %d, ptr = %d, in_len = %d\n", ret_len, single, len);

    /* 14ch * 19 power=266 */
    for (i = 0; i < (ret_len-1); i++) {
        char tmp_str[2] = {0};

        dot_ptr = find_next_char((char *)pwr_src, len, ',', ',');
        if ((dot_ptr > (pwr_src + os_strlen(pwr_src))) || (dot_ptr == NULL)) {
            LOG_E(wifi, "Single SKU Pwr string format is wrong.. %s", pwr_src);
            return;
        }
        //os_memset(tmp_str, 0x0, sizeof(tmp_str));
        os_memcpy(tmp_str, pwr_src, (dot_ptr - pwr_src));

        pwr_dst[i] = atoi(tmp_str);

        len = len - ((dot_ptr - pwr_src) + 1);
        pwr_src = (dot_ptr + 1);
    }

    pwr_dst[i] = atoi(pwr_src);

    //copy to 14 channel, each channel 19 power value.
    //for(uint8_t i = 0; i<14; i++) {
    //    os_memcpy(single->singleSKU2G[i], pwr_dst+19*i, 19);
    //}
    os_memcpy(single, pwr_dst, ret_len);
}
#endif

int32_t wifi_host_get_default_config(wifi_sys_cfg_t *syscfg)
{
    if (NULL == syscfg) {
        printf("syscfg is null\r\n");
        return -1;
    }

    syscfg->opmode =                  (uint8_t)atoi(WIFI_DEFAULT_OPMODE);
    syscfg->country_region =          (uint8_t)atoi(WIFI_DEFAULT_COUNTRY_REGION);
    syscfg->country_region_a_band =   (uint8_t)atoi(WIFI_DEFAULT_COUNTRY_REGION_A_BAND);
    os_strncpy((char *)syscfg->country_code, WIFI_DEFAULT_COUNTRY_CODE, sizeof(syscfg->country_code));
    syscfg->radio_off =               (uint8_t)atoi(WIFI_DEFAULT_RADIO_ONOFF);
    syscfg->dbg_level =               (uint8_t)atoi(WIFI_DEFAULT_N9_DEBUG_LEVEL);
    syscfg->rts_threshold =           (uint16_t)atoi(WIFI_DEFAULT_RTS_THRESHOLD);
    syscfg->frag_threshold =          (uint16_t)atoi(WIFI_DEFAULT_FRAGMENT_THRESHOLD);

    syscfg->sta_local_admin_mac =     (uint8_t)atoi(WIFI_DEFAULT_STA_LOCAL_ADMIN_MAC);
    //wifi_conf_get_ip_from_str(syscfg->sta_ip_addr, "192.168.1.1");
    wifi_host_conf_get_mac_from_str((char *)syscfg->sta_mac_addr,WIFI_DEFAULT_STA_MAC_ADDR);
    os_strncpy((char *)syscfg->sta_ssid, WIFI_DEFAULT_STA_SSID, sizeof(syscfg->sta_ssid));
    syscfg->sta_ssid_len =            (uint8_t)atoi(WIFI_DEFAULT_STA_SSID_LEN);
    syscfg->sta_bss_type =            (uint8_t)atoi(WIFI_DEFAULT_STA_BSS_TYPE);
    syscfg->sta_channel =             (uint8_t)atoi(WIFI_DEFAULT_STA_CHANNEL);
    syscfg->sta_bw =                  (uint8_t)atoi(WIFI_DEFAULT_STA_BANDWIDTH);
    syscfg->sta_wireless_mode =       (uint8_t)atoi(WIFI_DEFAULT_STA_WIRELESS_MODE);
    syscfg->sta_ba_decline =          (uint8_t)atoi(WIFI_DEFAULT_STA_BA_DECLINE);
    syscfg->sta_auto_ba =             (uint8_t)atoi(WIFI_DEFAULT_STA_AUTO_BA);
    syscfg->sta_ht_mcs =              (uint8_t)atoi(WIFI_DEFAULT_STA_HT_MCS);
    syscfg->sta_ht_ba_win_size =      (uint8_t)atoi(WIFI_DEFAULT_STA_HT_BA_WINDOW_SIZE);
    syscfg->sta_ht_gi =               (uint8_t)atoi(WIFI_DEFAULT_STA_HT_GI);
    syscfg->sta_ht_protect =          (uint8_t)atoi(WIFI_DEFAULT_STA_HT_PROTECT);
    syscfg->sta_ht_ext_ch =           (uint8_t)atoi(WIFI_DEFAULT_STA_HT_EXT_CHANNEL);
    syscfg->sta_wmm_capable =         (uint8_t)atoi(WIFI_DEFAULT_STA_WMM_CAPABLE);
    syscfg->sta_listen_interval =     (uint8_t)atoi(WIFI_DEFAULT_STA_LISTEN_INTERVAL);
    syscfg->sta_auth_mode =           (uint8_t)atoi(WIFI_DEFAULT_STA_AUTH_MODE);
    syscfg->sta_encryp_type =         (uint8_t)atoi(WIFI_DEFAULT_STA_ENCRYPT_TYPE);
    os_strncpy((char *)syscfg->sta_wpa_psk, WIFI_DEFAULT_STA_WPA_PSK, sizeof(syscfg->sta_wpa_psk));
    syscfg->sta_wpa_psk_len =         (uint8_t)atoi(WIFI_DEFAULT_STA_WPA_PSK_LEN);
    //os_strncpy((char *)syscfg->sta_pmk, "", sizeof(syscfg->sta_pmk));
    syscfg->sta_pair_cipher =         (uint8_t)atoi(WIFI_DEFAULT_STA_PAIR_CIPHER);;
    syscfg->sta_group_cipher =        (uint8_t)atoi(WIFI_DEFAULT_STA_GROUP_CIPHER);;
    syscfg->sta_default_key_id =      (uint8_t)atoi(WIFI_DEFAULT_STA_DEFAULT_KEY_ID);;
    syscfg->sta_ps_mode = (uint8_t)atoi(WIFI_DEFAULT_STA_POWER_SAVE_MODE);
    syscfg->sta_keep_alive_period =   (uint8_t)atoi(WIFI_DEFAULT_STA_KEEP_ALIVE_PERIOD);

    syscfg->ap_local_admin_mac =      (uint8_t)atoi(WIFI_DEFAULT_AP_LOCAL_ADMIN_MAC);
    //wifi_conf_get_ip_from_str(syscfg->ap_ip_addr, "192.168.1.1");
    wifi_host_conf_get_mac_from_str((char *)syscfg->ap_mac_addr,WIFI_DEFAULT_AP_MAC_ADDR);
    os_strncpy((char *)syscfg->ap_ssid, WIFI_DEFAULT_AP_SSID, sizeof(syscfg->ap_ssid));
    syscfg->ap_ssid_len =             (uint8_t)atoi(WIFI_DEFAULT_AP_SSID_LEN);
    syscfg->ap_channel =              (uint8_t)atoi(WIFI_DEFAULT_AP_CHANNEL);
    syscfg->ap_bw =                   (uint8_t)atoi(WIFI_DEFAULT_AP_BANDWIDTH);
    syscfg->ap_wireless_mode =        (uint8_t)atoi(WIFI_DEFAULT_AP_WIRELESS_MODE);
    syscfg->ap_auto_ba =              (uint8_t)atoi(WIFI_DEFAULT_AP_AUTO_BA);
    syscfg->ap_ht_mcs =               (uint8_t)atoi(WIFI_DEFAULT_AP_HT_MCS);
    syscfg->ap_ht_ba_win_size =       (uint8_t)atoi(WIFI_DEFAULT_AP_HT_BA_WINDOW_SIZE);
    syscfg->ap_ht_gi =                (uint8_t)atoi(WIFI_DEFAULT_AP_HT_GI);
    syscfg->ap_ht_protect =           (uint8_t)atoi(WIFI_DEFAULT_AP_HT_PROTECT);
    syscfg->ap_ht_ext_ch =            (uint8_t)atoi(WIFI_DEFAULT_AP_HT_EXT_CHANNEL);
    syscfg->ap_wmm_capable =          (uint8_t)atoi(WIFI_DEFAULT_AP_WMM_CAPABLE);
    syscfg->ap_dtim_period =          (uint8_t)atoi(WIFI_DEFAULT_AP_DTIM_PERIOD);
    syscfg->ap_hide_ssid =            (uint8_t)atoi(WIFI_DEFAULT_AP_HIDDEN_SSID);
    syscfg->ap_auto_channel_select =  (uint8_t)atoi(WIFI_DEFAULT_AP_AUTO_CHANNEL_SELECT);
    syscfg->ap_auth_mode =            (uint8_t)atoi(WIFI_DEFAULT_AP_AUTH_MODE);
    syscfg->ap_encryp_type =          (uint8_t)atoi(WIFI_DEFAULT_AP_ENCRYPT_TYPE);
    os_strncpy((char *)syscfg->ap_wpa_psk, WIFI_DEFAULT_AP_WPA_PSK, sizeof(syscfg->ap_wpa_psk));
    syscfg->ap_wpa_psk_len =          (uint8_t)atoi(WIFI_DEFAULT_AP_WPA_PSK_LEN);
    //os_strncpy((char *)syscfg->ap_pmk, "", sizeof(syscfg->ap_pmk));
    syscfg->ap_pair_cipher =          (uint8_t)atoi(WIFI_DEFAULT_AP_PAIR_CIPHER);
    syscfg->ap_group_cipher =         (uint8_t)atoi(WIFI_DEFAULT_AP_GROUP_CIPHER);
    syscfg->ap_default_key_id =       (uint8_t)atoi(WIFI_DEFAULT_AP_DEFAULT_KEY_ID);
    syscfg->ap_beacon_disable =       (uint8_t)atoi(WIFI_DEFAULT_AP_BEACON_DISABLE);
    syscfg->forwarding_zero_copy = 1;

    syscfg->bg_band_entry_num = wifi_conf_get_ch_table_from_str(syscfg->bg_band_triple,
                                                                10, WIFI_DEFAULT_BG_CHANNEL_TABLE,
                                                                os_strlen(WIFI_DEFAULT_BG_CHANNEL_TABLE));

    syscfg->a_band_entry_num = wifi_conf_get_ch_table_from_str(syscfg->a_band_triple,
                                                               10, WIFI_DEFAULT_A_CHANNEL_TABLE,
                                                               os_strlen(WIFI_DEFAULT_A_CHANNEL_TABLE));
    syscfg->mbss_enable =             (uint8_t)atoi(WIFI_DEFAULT_MBSS_ENABLE);
    os_strncpy((char *)syscfg->mbss_ssid1, WIFI_DEFAULT_MBSS_SSID1, sizeof(syscfg->mbss_ssid1));
    syscfg->mbss_ssid_len1 = os_strlen(WIFI_DEFAULT_MBSS_SSID1);
    os_strncpy((char *)syscfg->mbss_ssid2, WIFI_DEFAULT_MBSS_SSID2, sizeof(syscfg->mbss_ssid2));
    syscfg->mbss_ssid_len2 = os_strlen(WIFI_DEFAULT_MBSS_SSID2);

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    syscfg->config_free_ready =       (uint8_t)atoi(WIFI_DEFAULT_CONFIG_FREE_READY);
    syscfg->config_free_enable =      (uint8_t)atoi(WIFI_DEFAULT_CONFIG_FREE_ENABLE);
#endif
    syscfg->beacon_lost_time =        (uint8_t)atoi(WIFI_DEFAULT_STA_BEACON_LOST_TIME);
    syscfg->apcli_40mhz_auto_upbelow =(uint8_t)atoi(WIFI_DEFAULT_APCLI_BW_AUTO_UP_BELOW);
    syscfg->wifi_privilege_enable =   (uint8_t)atoi(WIFI_DEFAULT_WIFI_PRIVILEGE_ENABLE);
    syscfg->sta_keep_alive_packet =   (uint8_t)atoi(WIFI_DEFAULT_STA_KEEP_ALIVE_PACKET);

#ifdef MTK_SINGLE_SKU_SUPPORT
    syscfg->support_single_sku  =     (uint8_t)atoi(WIFI_DEFAULT_SUPPORT_SINGLE_SKU);
    wifi_conf_get_pwr_from_str(sizeof(singleSKU2G_t), &syscfg->single_sku_2g, WIFI_DEFAULT_SKU_2G_GROUP0);
    wifi_conf_get_pwr_from_str(sizeof(singleSKU5G_t), &syscfg->single_sku_5g, WIFI_DEFAULT_SKU_5G_GROUP0);
#endif

#ifdef MTK_ANT_DIV_ENABLE
    syscfg->antenna_number = (uint8_t)atoi(WIFI_DEFAULT_ANT_NUMBER);
    syscfg->antenna_diversity_enable = (uint8_t)atoi(WIFI_DEFAULT_ANT_DIVERSITY_ENABLE);
    syscfg->antenna_probe_req_count = (uint8_t)atoi(WIFI_DEFAULT_ANT_PROB_REQ_COUNT);
    syscfg->antenna_threshold_level = (uint8_t)atoi(WIFI_DEFAULT_ANT_THRESHOLD_LEVEL);
    syscfg->antenna_delta_rssi = (uint8_t)atoi(WIFI_DEFAULT_ANT_DELTA_RSSI);
    syscfg->antenna_rssi_switch = (uint8_t)atoi(WIFI_DEFAULT_ANT_RSSI_SWITCH);
    syscfg->antenna_pre_selected_rssi = (uint8_t)atoi(WIFI_DEFAULT_ANT_PRE_SELECTED_RSSI);
#endif

    return 0;
}

