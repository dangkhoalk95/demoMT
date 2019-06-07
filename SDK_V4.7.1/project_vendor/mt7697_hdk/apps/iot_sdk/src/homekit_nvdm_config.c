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
#ifdef MTK_HOMEKIT_ENABLE
#include "hal_sys.h"

#define MAX_KEY_PAIRS   16
#define CFG_KEY_HAP_CTRL_LTPK             "HAP_CTRL_LTPK"
#define NVDM_GROUP_HOMEKIT                "homekit"
#endif
#include "type_def.h"
#include "nvdm_config.h"

#ifdef MTK_HOMEKIT_ENABLE
/* homekit config */
static const group_data_item_t g_homekit_data_item_array[] = {
    NVDM_DATA_ITEM("WACDONE",                     "0"),
    NVDM_DATA_ITEM("WAC_ACS_NAME",                "New Wi-Fi Device"),
    NVDM_DATA_ITEM("HOMEKIT_AUTO_START",          "1"),
    NVDM_DATA_ITEM("HOMEKIT_DELAY",               "3"),
    NVDM_DATA_ITEM("HAP_MODEL_NAME",              "MT7687E2"),
    NVDM_DATA_ITEM("HAP_ACS_NAME",                "MTK_Aces"),
    NVDM_DATA_ITEM("HAP_SERIAL_NUM",              "123456789"),
    NVDM_DATA_ITEM("HAP_SETUP_CODE",              "482-11-763"),
    NVDM_DATA_ITEM("HAP_CONFIG_NUM",              "1"),
    NVDM_DATA_ITEM("HAP_MFR",                     "MTK"),
    NVDM_DATA_ITEM("HAP_ACS_LTPK",                ""),
    NVDM_DATA_ITEM("HAP_ACS_LTSK",                ""),
    NVDM_DATA_ITEM("HAP_CTRL_LTPK",               ""),
    NVDM_DATA_ITEM("HAP_MFI_ENABLE",              "1"),
    NVDM_DATA_ITEM("HAP_PAIR_SETUP_DISABLE",      "0"),
    NVDM_DATA_ITEM("HAP_BIND_INF",                "apcli0"),
    NVDM_DATA_ITEM("HAP_DEBUG_LEVEL",             "4"),
    NVDM_DATA_ITEM("HAP_ACS_CONF",                "3E,4A"),
    NVDM_DATA_ITEM("HAP_ACS_CI",                  "9"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_HC_CURRENT",   "1"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_HC_TARGET",    "1"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_TP_CURRENT",   "50"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_TP_TARGET",    "25"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_TP_UNITS",     "1"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_NAME",         "Thermostat_Service"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_RH_CURRENT",   "0"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_RH_TARGET",    "0"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_TC_THRESHOLD", "10"),
    NVDM_DATA_ITEM("HAP_APP_THERMO_TH_THRESHOLD", "0"),
};

static void homekit_check_default_value(void)
{
    check_default_value("homekit",
                        (group_data_item_t *)g_homekit_data_item_array,
                        sizeof(g_homekit_data_item_array) / sizeof(g_homekit_data_item_array[0]));
    LOG_I(common, "homekit_check start %d", sizeof(g_homekit_data_item_array) / sizeof(group_data_item_t));

    uint8_t *buffer = NULL;
    buffer = (uint8_t *)pvPortMalloc(1024 * sizeof(uint8_t));
    if (buffer == NULL) {
        LOG_E(common, "homekit_check_value, failed to malloc");
        return;
    }
    memset(buffer, 0, 1024 * sizeof(uint8_t));

    for (uint32_t idx = 0 ; idx < MAX_KEY_PAIRS; idx++) {
        char keyName[64] = {0};
        uint32_t buffer_size = 1024;
        snprintf(keyName, sizeof(keyName), "%s_%d", CFG_KEY_HAP_CTRL_LTPK, (int16_t)idx);
        nvdm_status_t status = nvdm_read_data_item(NVDM_GROUP_HOMEKIT, keyName, buffer, &buffer_size);
        LOG_I(common, "HAP cli: homekit_check start CTRL_LTPK %d", status);
        if (status != NVDM_STATUS_OK) {
            nvdm_status_t status = nvdm_write_data_item(NVDM_GROUP_HOMEKIT,
                                   keyName,
                                   NVDM_DATA_ITEM_TYPE_STRING,
                                   (const uint8_t *)(""),
                                   sizeof(""));
            LOG_I(common, "HAP cli: homekit_check start CTRL_LTPK %d", status);
            if (status != NVDM_STATUS_OK) {
                LOG_I(common, "homekit_check CTRL_LTPK error: %s", g_homekit_data_item_array[idx].item_name);
            }
        }
    }
    vPortFree(buffer);
    LOG_I(common, "homekit_check end");
}

static void homekit_reset_to_default(void)
{
    reset_to_default("homekit",
                     (group_data_item_t *)g_homekit_data_item_array,
                     sizeof(g_homekit_data_item_array) / sizeof(g_homekit_data_item_array[0]));

    for (uint32_t idx = 0 ; idx < MAX_KEY_PAIRS; idx++) {
        char keyName[64] = {0};
        snprintf(keyName, sizeof(keyName), "%s_%d", CFG_KEY_HAP_CTRL_LTPK, (int16_t)idx);
        nvdm_status_t status = nvdm_write_data_item(NVDM_GROUP_HOMEKIT,
                               keyName,
                               NVDM_DATA_ITEM_TYPE_STRING,
                               (const uint8_t *)(""),
                               sizeof(""));
        if (status != NVDM_STATUS_OK) {
            LOG_I(common, "          error: %s", keyName);
        }
    }
    LOG_I(common, "homekit_reset end.         System will reboot now... ...");
    hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
}

static void homekit_show_value(void)
{
    show_group_value("homekit",
                     (group_data_item_t *)g_homekit_data_item_array,
                     sizeof(g_homekit_data_item_array) / sizeof(g_homekit_data_item_array[0]));

    LOG_I(common, "homekit_show_value start");

    uint8_t *buffer = NULL;
    buffer = (uint8_t *)pvPortMalloc(1024 * sizeof(uint8_t));
    if (buffer == NULL) {
        LOG_E(common, "homekit_show_value, failed to malloc");
        return;
    }
    memset(buffer, 0, 1024 * sizeof(uint8_t));

    for (uint32_t idx = 0 ; idx < MAX_KEY_PAIRS; idx++) {
        char keyName[64] = {0};
        uint32_t buffer_size = 1024;
        snprintf(keyName, sizeof(keyName), "%s_%d", CFG_KEY_HAP_CTRL_LTPK, (int16_t)idx);
        nvdm_status_t status = nvdm_read_data_item(NVDM_GROUP_HOMEKIT, keyName, buffer, &buffer_size);
        if (status == NVDM_STATUS_OK) {
            LOG_I(common, "          %s: %s", keyName, buffer);
        } else {
            LOG_I(common, "          error: %s %d", keyName, status);
        }
    }

    vPortFree(buffer);
    LOG_I(common, "homekit_show_value end");
}

const user_data_item_operate_t homekit_data_item_operate_array[] = {
    {
        "homekit",
        homekit_check_default_value,
        homekit_reset_to_default,
        homekit_show_value,
    },
};

#endif

