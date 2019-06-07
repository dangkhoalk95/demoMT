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

/****************************************************************************
    Module Name:
    WiFi

    Abstract:
    WiFi processor configure / setting for STA operations

    Revision History:
    Who                     When                 What
    Jerry Liu           2016/09/20       Initial
    --------            ----------      ------------------------------------------
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wifi_api.h"
#include "kal_public_api.h"
#include "mtk_wifi_adapter.h"


WLAN_SCAN_RESULT *g_scan_list = NULL;
uint32_t g_scan_list_size = 0;
uint8_t  g_scan_inited = 0; //To protect the buffer in function wifi_connection_scan_init(), if other task start scan just before reset the buffer.


/**
* @brief Get WiFi Interface MAC Address.
* port: 0 STA, 1 AP
*/
int32_t wifi_config_get_mac_address(uint8_t port, uint8_t *address)
{
    if (NULL == address) {
        printc("address is null.\n");
        return -1;
    }

    if (0 == wifi_get_mac_addr_from_efuse(port, address)) {
        return 0;
    }
    printc("wifi_get_mac_addr_from_efuse fail.\n");

    if (0 == wifi_get_mac_addr_from_nvdm(port, address)) {
        return 0;
    }
    printc("wifi_get_mac_addr_from_nvdm fail.\n");

    if (0 == wifi_get_mac_addr_from_config(port, address)) {
        return 0;
    }
    printc("wifi_get_mac_addr_from_config fail.\n");

    return -1;
}

/**
 * @brief Disconnect the current connection
 *
 * @return  >=0 means success, <0 means fail
 *
 * @note    STA will back to IDLE state once disconnect from AP.
 */
int32_t wifi_connection_disconnect_ap(void)
{
    return wlan_disconnect_network();
}

/**
 * @brief Get the current STA port link up / link down status of the connection
 *
 * @param [OUT]link_status
 * @param 0 WIFI_STATUS_LINK_DISCONNECTED
 * @param 1 WIFI_STATUS_LINK_CONNECTED
 *
 * @return  >=0 means success, <0 means fail
 *
 * @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may be in
 *       IDLE/ SCAN/ CONNECTING state.
 */
int32_t wifi_connection_get_link_status(uint8_t *link_status)
{
    if (NULL == link_status) {
        printc("link_status is null\n");
        return -1;
    }

    wl_connection_info_t conn_info = {0};

    if (0 == wlan_get_connection_info(&conn_info)) {
        if (conn_info.Ssid_len == 0) {
            *link_status = WIFI_STATUS_LINK_DISCONNECTED;
        } else {
            *link_status = WIFI_STATUS_LINK_CONNECTED;
        }
    } else {
        printc("get link status fail\n");
        return -1;
    }

    return 0;
}

/**
* @brief This function initializes the scan table to the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list will be stored.
* @param[in] max_count is the maximum number of ap_list can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_scan_init(WLAN_SCAN_RESULT *ap_list, uint32_t max_count)
{
    if (NULL == ap_list) {
        return -1;
    }
    kal_task_enter_critical();
    if ((1 != g_scan_inited) && (NULL == g_scan_list)) {
        g_scan_list = ap_list;
        g_scan_list_size = sizeof(WLAN_SCAN_RESULT) - sizeof(WLAN_BSS_INFO) + max_count * sizeof(WLAN_BSS_INFO);
        kal_mem_set(g_scan_list, 0, g_scan_list_size);
        g_scan_inited = 1;
        kal_task_exit_critical();
    } else {
        kal_task_exit_critical();
        return -1;
    }
    return 0;
}

/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_scan_deinit(void)
{
    if (g_scan_inited == 0) {
        return -1;
    }
    g_scan_inited = 0;
    g_scan_list = NULL;
    g_scan_list_size = 0;
    return 0;
}


/**
* @brief Star WiFi Scanning
*
* @param [IN]ssid SSID. Not support now.
* @param [IN]ssid_len Length of SSID. Not support now.
* @param [IN]bssid BSSID. Not support now.
* @param [IN]scan_mode. Only support Active scan now.
* @param 0 Active Scan
* @param 1 Passive Scan
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option)
{
    return wlan_scan_network(ssid, ssid_length, bssid, scan_mode, scan_option);
}

/**
* @brief Get the rssi of the connected AP.
*
* @param [OUT]rssi The rssi of the connected AP will be returned
*
* @return  >=0 means success, <0 means fail
*
* @note  Only used for STA mode and the station has connected to the AP.
*
*/
int32_t wifi_connection_get_rssi(int32_t *rssi)
{
    if (NULL == rssi) {
        printc("rssi is null\n");
        return -1;
    }

    wl_connection_info_t conn_info = {0};

    if (0 == wlan_get_connection_info(&conn_info)) {
        *rssi = conn_info.Rssi;
        return 0;
    } else {
        *rssi = 0;
        return -1;
    }
}

/**
* @brief Register WiFi Event Notifier
*
* @param [IN]event
* @param event Event ID
* @param Disconnect Event (0)
* @param Port Secure Event (1)
* @param WIFI Init Done Event (2)
* @param WIFI Scan Complete event (3)
* @param [IN]notifier
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler)
{
    return wifi_set_notification_callback(1, (int)(0x1 << event), handler);
}


/**
* @brief Register WiFi Event Notifier
*
* @param [IN]event
* @param [IN]notifier
* @param event Event ID
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler)
{
    return wifi_set_notification_callback(0, (int)(0x1 << event), handler);
}


/**
* @brief This function sets power saving mode in station mode. \n
*
* @param[in] mode indicates the power saving mode that the Wi-Fi chip will be configured to.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #Param_PowerModeCAM        | Continuous active mode|
* \b #Param_PowerModeMAX_PSP    | Max power saving mode|
* \b #Param_PowerModeFast_PSP   | Fast power saving mode|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_sta_power_saving_mode(PARAM_POWER_MODE mode)
{

    return wlan_sta_set_power_saving(mode);

}

/**
* @brief This function gets power saving mode in station mode. \n
*
* @param[out] mode indicates the current power saving mode of the Wi-Fi chip.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | #Param_PowerModeCAM, continuous active mode|
* \b 1                          | #Param_PowerModeMAX_PSP, max power saving mode|
* \b 2                          | #Param_PowerModeFast_PSP, fast power saving mode|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_sta_power_saving_mode(PARAM_POWER_MODE *mode)
{

    return wlan_sta_get_power_saving(mode);

}

/**
* @brief This function starts wifi module. \n
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wifi_start(void)
{
    int32_t status = 0;

    __g_virtual_hdl = wlan_start(NULL, 0, 0, 0);

    if (__g_virtual_hdl == NULL) {
        status = WIFI_FAIL;
    } else {
        status = WIFI_SUCC;
    }

    return status;
}

/**
* @brief This function stops wifi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wifi_stop(void)
{

    int32_t status = 0;

    if (__g_virtual_hdl == NULL) {
        status = WIFI_FAIL;
    } else {
        wlan_stop(__g_virtual_hdl);
        status = WIFI_SUCC;
    }

    return status;
}

/**
* @brief This function connects to a specific AP in station mode. \n
*
* @param[in] ssid is the SSID of the target AP.
*
* @param[in] authMode is the authentication mode of the target AP.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WLM_WPA_AUTH_DISABLED     | Legacy (i.e., non-WPA)|
* \b #WLM_WPA_AUTH_NONE         | none (IBSS)|
* \b #WLM_WPA_AUTH_PSK          | WPA PAK|
* \b #WLM_WPA2_AUTH_PSK         | WPA2 PSK|
* \b #WLM_WPA_WPA2_AUTH_PSK     | WPA/WPA2 PSK mixture mode|
*
* @param[in] encryption is the encryption type of the target AP.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WLM_ENCRYPT_NONE          | No encryption|
* \b #WLM_ENCRYPT_WEP           | WEP encryption|
* \b #WLM_ENCRYPT_TKIP          | TKIP encryption|
* \b #WLM_ENCRYPT_AES           | AES encryption|
* \b #WLM_ENCRYPT_TKIP_AES      | TKIP AES encryption|
*
* @param[in] key is password of the target AP.\n
*            For AP with security mode of WEP open/WEP shared key, it indicates WEP key.\n
*            For AP with security mode of WPA PSK/WPA2 PSK/WPA/WPA2 PSK mixture mode, it indicates passphrase.\n
*
* @param[in] wep_key_index is WEP key index of the target AP if the security mode is WEP open/WEP shared key.
*
* @param[in] timeout is of the maximum connecting time. if it is NULL, the default time is 10000ms
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  1.For AP with security mode of open mode, the authMode is WLM_WPA_AUTH_DISABLED, and the encryption is WLM_ENCRYPT_NONE.\n
*        2.For AP with security mode of WEP open/WEP shared key, the authMode is WLM_WPA_AUTH_DISABLED, and the encryption is WLM_ENCRYPT_WEP.\n
*        3.For AP with security mode of WPA PSK, the authMode is WLM_WPA_AUTH_PSK, and the encryption is WLM_ENCRYPT_TKIP.\n
*        4.For AP with security mode of WPA2 PSK, the authMode is WLM_WPA2_AUTH_PSK, and the encryption is WLM_ENCRYPT_AES.\n
*        5.For AP with security mode of WPA/WPA2 PSK mixture mode, use either WPA PSK or WPA2 PSK method to connect to it.\n
*
*/
int32_t wifi_config_set_sta_connection(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption, const char *key, uint8_t wep_key_index, struct os_time *timeout)
{
    return wlan_join_network(ssid, authMode, encryption, key, wep_key_index, timeout);
}

