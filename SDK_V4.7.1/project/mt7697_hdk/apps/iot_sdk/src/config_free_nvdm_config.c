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

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "nvdm.h"
#include "syslog.h"
#include "connsys_profile.h"
#include "connsys_util.h"
#include "get_profile_string.h"
#include "wifi_nvdm_config.h"
#include "type_def.h"
#include "syslog.h"
#include "wifi_init.h"
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
#include "wifi_profile.h"
#include <inband_queue.h>
#include "wifi_scan.h"
#endif
#include "wifi_inband.h"
#include "nvdm_config.h"
#include "config_free_nvdm_config.h"
#include "wifi_default_config.h"

static const group_data_item_t g_config_free_data_item_array[] = {
    NVDM_DATA_ITEM("ConfigFree_Ready",   WIFI_DEFAULT_CONFIG_FREE_READY),
    NVDM_DATA_ITEM("ConfigFree_Enable",  WIFI_DEFAULT_CONFIG_FREE_ENABLE),
};

static void config_free_check_default_value(void)
{
    check_default_value("config_free",
                        g_config_free_data_item_array,
                        sizeof(g_config_free_data_item_array) / sizeof(g_config_free_data_item_array[0]));
}

static void config_free_reset_to_default(void)
{
    reset_to_default("config_free",
                     g_config_free_data_item_array,
                     sizeof(g_config_free_data_item_array) / sizeof(g_config_free_data_item_array[0]));
}

static void config_free_show_value(void)
{
    show_group_value("config_free",
                     g_config_free_data_item_array,
                     sizeof(g_config_free_data_item_array) / sizeof(g_config_free_data_item_array[0]));
}

const user_data_item_operate_t config_free_data_item_operate_array[] = {
    {
        "config_free",
        config_free_check_default_value,
        config_free_reset_to_default,
        config_free_show_value,
    },
};

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
extern int32_t mtk_smart_connect(void);

int32_t cf_set_ssid(uint8_t port, uint8_t *ssid , uint8_t ssid_length)
{
    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (ssid_length > WIFI_MAX_LENGTH_OF_SSID) {
        LOG_I(wifi, "incorrect length(=%d)", ssid_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid) {
        LOG_E(wifi, "ssid is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    char ssid_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char ssid_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    memcpy(ssid_buf, ssid, ssid_length);
    ssid_buf[ssid_length] = '\0';

    sprintf(ssid_len_buf, "%d", ssid_length);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t cf_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length)
{
    char pass_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char pass_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase == NULL) {
        LOG_E(wifi, "passphrase is null.");
        return WIFI_ERR_PARA_INVALID;
    }
    if ((passphrase_length < 8) || (passphrase_length > WIFI_LENGTH_PASSPHRASE)) {
        LOG_E(wifi, "incorrect length(=%d)", passphrase_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase_length == WIFI_LENGTH_PASSPHRASE) {
        for (uint8_t index = 0; index < WIFI_LENGTH_PASSPHRASE; index++) {
            if (!hex_isdigit(passphrase[index])) {
                LOG_E(wifi, "length(=%d) but the strings are not hex strings!", passphrase_length);
                return WIFI_ERR_PARA_INVALID;
            }
        }
    }

    sprintf(pass_len_buf, "%d", passphrase_length);
    memcpy(pass_buf, passphrase, passphrase_length);
    pass_buf[passphrase_length] = '\0';

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}
int32_t cf_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type)
{
    char auth_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char encrypt_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_auth_mode_valid(auth_mode)) {
        LOG_E(wifi, "auth_mode is invalid: %d", auth_mode);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_encrypt_type_valid(encrypt_type)) {
        LOG_E(wifi, "encrypt_type is invalid: %d", encrypt_type);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(auth_buf, "%d", auth_mode);
    sprintf(encrypt_buf, "%d", encrypt_type);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t cf_set_channel(uint8_t port, uint8_t channel)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", channel);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, strlen(buf))) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    return WIFI_SUCC;
}
int save_cf_credential_to_nvdm(P_IOT_CONFIG_FREE_IE cred)
{
    int status = 0;

    if (cred == NULL) {
        LOG_E(wifi, "ERROR! invalid cred pointer(NULL)");
        return -1;
    } else {
        LOG_E(wifi, "[ConfigFree] cred: Ssid = %s, SsidLen = %d, AuthMode = %d, EncrypType = %d, WpaPsk = %s, WpaPskLen = %d, Ch = %d",
              cred->Ssid,
              cred->SsidLen,
              cred->AuthMode,
              cred->EncrypType,
              cred->WpaPsk,
              cred->WpaPskLen,
              cred->Channel);
    }

    // Set NVRAM STA configuration by credential
    if (cf_set_ssid(WIFI_PORT_STA, cred->Ssid, cred->SsidLen) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][STA] wifi_profile_set_ssid failed (Ssid=%s, Len=%d)", cred->Ssid, cred->SsidLen);
        status = -1;
    }

    if (cf_set_security_mode(WIFI_PORT_STA, cred->AuthMode, cred->EncrypType) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][STA] wifi_profile_set_security_mode failed (Auth=%d, Encry=%d)", cred->AuthMode, cred->EncrypType);
        status = -1;
    }

    if (cf_set_wpa_psk_key(WIFI_PORT_STA, cred->WpaPsk, cred->WpaPskLen) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][STA] wifi_profile_set_wpa_psk_key failed (WpaPsk=%s, Len=%d)", cred->WpaPsk, cred->WpaPskLen);
        status = -1;
    }

    // Set NVRAM AP configuration by credential
    if (cf_set_ssid(WIFI_PORT_AP, cred->Ssid, cred->SsidLen) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][AP] wifi_profile_set_ssid failed (Ssid=%s, Len=%d)", cred->Ssid, cred->SsidLen);
        status = -1;
    }

    if (cf_set_security_mode(WIFI_PORT_AP, cred->AuthMode, cred->EncrypType) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][AP] wifi_profile_set_security_mode failed (Auth=%d, Encry=%d)", cred->AuthMode, cred->EncrypType);
        status = -1;
    }

    if (cf_set_channel(WIFI_PORT_AP, cred->Channel) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][AP] wifi_profile_set_channel failed (Ch=%d)", cred->Channel);
        status = -1;
    }

    return status;
}

int32_t save_cf_ready_to_nvdm(uint8_t config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 != config_ready && 1 != config_ready) {
        LOG_E(wifi, "config_ready is invalid: %d", config_ready);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", WIFI_MODE_REPEATER);
    LOG_I(wifi, "wifi_profile_set_opmode: opmode=%s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, strlen(buf))) {
        return WIFI_FAIL;
    }

    memset(buf, 0x0, WIFI_PROFILE_BUFFER_LENGTH);

    sprintf(buf, "%d", config_ready);
    LOG_I(wifi, "ConfigFree ready: %s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item("config_free", "ConfigFree_Ready",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t get_cf_ready_to_nvdm(uint8_t *config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == config_ready) {
        LOG_E(wifi, "config_ready is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item("config_free", "ConfigFree_Ready", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *config_ready = atoi(buf);
    return WIFI_SUCC;
}
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */


