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
 
/**
* @file wfc_at_api.h
*
*  Wi-Fi API configuration/settings in WFC.
*
*/

/**@addtogroup WFC
* @{
*
* This section introduces the Wi-Fi API configuration/settings in WFC.
*
*/

#ifndef __WFCM_AT_API_H__
#define __WFCM_AT_API_H__

#include <stdint.h>
#include "wifi_host_default_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/

/**
* @brief Represents max support number of wifi scan list.
*/
#define SCAN_AP_LIST_MAX_COUNT 100

/**
* @brief Represents size of TX power bin.
*/
#define TX_POWER_BIN_SIZE 24

/**
* @brief Represents pack align method of WFC wifi host api command and response.
*/
#define WFC_PACK __attribute__((__packed__))

/**
* @}
*/

/**@defgroup WIFI_ENUM Enumeration
* @{
*/

/**
* @brief Represents the wifi api type defined in wfc.
*/
enum E_WFC_API
{
    WFC_API_OPMODE,
    WFC_API_SSID,
    WFC_API_SECURITY,
    WFC_API_PASSWD,
    WFC_API_RELOAD,
    WFC_API_CONNECTION,
    WFC_API_SCAN_START,
    WFC_API_SCAN_STOP,
    WFC_API_MAC_ADDR,
    WFC_API_INIT_PARA,
    WFC_API_SLEEP_LOCK,
    WFC_API_INIT_TX_POWER,
};

/**
* @}
*/


/**@defgroup WIFI_TYPEDEF Typedef
* @{
*/

/**
* @brief Represents the wifi api id structure defined in wfc.
*/
typedef struct{
    uint32_t api;
    bool set;
} WFC_PACK wh_api_id_t;


/**
* @brief Represents the command/response structure of #WFC_API_OPMODE command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t opmode;
} WFC_PACK wh_set_opmode_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_opmode_resp_para_t;

typedef struct {
    wh_api_id_t api_id;
} WFC_PACK wh_get_opmode_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t opmode;
} WFC_PACK wh_get_opmode_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_SSID command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t ssid_len;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
} WFC_PACK wh_set_ssid_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_ssid_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
} WFC_PACK wh_get_ssid_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t ssid_len;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
} WFC_PACK wh_get_ssid_resp_para_t;



/**
* @brief Represents the command/response structure of #WFC_API_SECURITY command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t auth_mode;
    uint8_t encrypt_type;

} WFC_PACK wh_set_security_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_security_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
} WFC_PACK wh_get_security_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t auth_mode;
    uint8_t encrypt_type;
} WFC_PACK wh_get_security_resp_para_t;



/**
* @brief Represents the command/response structure of #WFC_API_PASSWD command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    int8_t wep_key_index;
    uint8_t password_len;
    uint8_t password[WIFI_LENGTH_PASSPHRASE];
} WFC_PACK wh_set_password_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_password_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    int8_t wep_key_index;
} WFC_PACK wh_get_password_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t password_len;
    uint8_t password[WIFI_LENGTH_PASSPHRASE];
} WFC_PACK wh_get_password_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_RELOAD command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
} WFC_PACK wh_set_reload_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_reload_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_CONNECTION command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    bool disconnect;
    uint8_t client_mac[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_set_connection_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_connection_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t client_mac[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_get_connection_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t link_status;
    int8_t rssi;
    uint8_t ssid_len;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
} WFC_PACK wh_get_connection_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_SCAN_START command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    bool ssid_valid;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
    uint8_t ssid_length;
    bool bssid_valid;
    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t scan_mode;
    uint8_t scan_option;
    uint8_t support_number;
} WFC_PACK wh_set_scan_start_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_scan_start_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_SCAN_STOP command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
} WFC_PACK wh_set_scan_stop_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_scan_stop_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_MAC_ADDR command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t mac_addr[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_set_mac_addr_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_mac_addr_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
} WFC_PACK wh_get_mac_addr_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t mac_addr[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_get_mac_addr_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_INIT_PARA command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    bool config_valid;
    wifi_config_t  config;
    bool config_ext_valid;
    wifi_config_ext_t config_ext;
    bool sys_cfg_valid;
    wifi_sys_cfg_t sys_cfg;
} WFC_PACK wh_set_init_para_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_init_para_resp_para_t;

/**
* @brief Represents the command/response structure of #WFC_API_TX_POWER command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t tx_power_bin[TX_POWER_BIN_SIZE];
} WFC_PACK wh_set_tx_power_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_tx_power_resp_para_t;
/**
* @brief Represents the structure of wifi scan result defined in wfc.
*/
typedef struct {
    uint32_t list_size;
    uint32_t index;
    wifi_scan_list_item_t scan_list[];
} WFC_PACK wh_scan_list_t;


/**
* @brief Represents the structure of wifi scan result event defined in wfc.
*/
typedef struct {
    uint16_t evt_id;
    wh_scan_list_t scan_table;
} WFC_PACK wh_scan_reslt_evt_para_t;


/**
* @}
*/

/**@defgroup WIFI_STRUCT Structure
* @{
*/

/**
* @brief Represents wfc system setting.
*/
typedef struct {
    wifi_config_t wfc_wifi_config;
    wifi_config_ext_t wfc_wifi_config_ext;
    wifi_sys_cfg_t sys_cfg;
    uint8_t wfc_wifi_status;
    uint8_t wfc_wifi_config_valid;
    uint8_t wfc_wifi_config_ext_valid;
    uint8_t sys_cfg_valid;
} wfc_system_config_t;

/**
* @brief Represents the command/response structure of #WFC_API_SLEEP command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t sleep;

} WFC_PACK wh_set_sleep_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_sleep_cmd_resp_para_t;

/**
*@}
*/

#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif  /* __WFCM_AT_API_H__ */
