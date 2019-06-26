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
 
/****************************************************************************
    Module Name:
    WiFi

    Abstract:
    WiFi processor configure / setting for STA operations

    Revision History:
    Who                     When                 What
    Pengfei Qiu      2018/04/12       Initial
    --------            ----------      ------------------------------------------
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type_def.h"
#include "host_os_utils.h"
#include "nvdm.h"
#include "ethernet_filter.h"
#include "syslog.h"
#include "hal_efuse.h"
#include "wifi_host_api.h"
#include "wifi_host_private_api.h"
#include "wfcm_stub.h"
#include "host_os_utils.h"
#include "wifi_host_event.h"
#include "wifi_host_init.h"
#include "wifi_host_os_api.h"
#include "wfcm_at_api.h"
#include "hal.h"


#define RSP_BUF_SIZE 1024
extern uint8_t rsp_buf[RSP_BUF_SIZE];
extern uint32_t rsp_size;

extern volatile bool g_wifi_host_scanning;

#define WIFI_ERR_PARA_INVALID (-1)
#define WIFI_ERR_NOT_SUPPORT  (-1)

/**
* @brief This function sets the SSID and SSID length that the Wi-Fi driver uses for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] ssid is the SSID content. If #WIFI_PORT_STA or #WIFI_PORT_APCLI, the SSID indicates the target AP's SSID.
*                       If WIFI_PORT_AP, the SSID indicates the device's own SSID.
* @param[in] ssid_length is the length of the SSID, the maximum length is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_ssid(uint8_t port, uint8_t *ssid, uint8_t ssid_length)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute SSID command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_set_ssid_cmd_para_t cmd_para;
    wh_set_ssid_resp_para_t *rsp_para;
    if(ssid_length > WIFI_MAX_LENGTH_OF_SSID) {
        return -1;
    }
    cmd_para.api_id.api = WFC_API_SSID;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    cmd_para.ssid_len = ssid_length;

    os_memcpy(cmd_para.ssid, ssid, ssid_length);
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_ssid_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;

}

/**
* @brief This function sets the authentication and encryption modes used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] auth_mode is the authentication mode. For details about this parameter, please refer to
* #wifi_auth_mode_t.
* @param[in] encrypt_type is the encryption mode. For details about this parameter, please refer to
* #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note In station mode, the device can connect to AP without setting any security mode.\n
*       If #wifi_host_config_set_security_mode() is not called, the device will change security mode dynamically according to the target AP.\n
*       If #wifi_host_config_set_security_mode() is called, the device will set a security mode to connect to the target AP.\n
*       Calling #wifi_host_config_set_security_mode() is optional in station mode.\n
* To skip the security mode setting, simply do not call #wifi_host_config_set_security_mode() before #wifi_host_config_reload_setting().
*/
int32_t wifi_host_config_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute security command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_set_security_cmd_para_t cmd_para;
    wh_set_security_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SECURITY;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    cmd_para.auth_mode = auth_mode;
    cmd_para.encrypt_type = encrypt_type;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_security_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;

}

/**
* @brief This function sets the password used in the Wi-Fi driver for a specific wireless port.
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] password is the password of AP.
*
* @param[in] password_length is the password length of AP.
*
* @param[in] wep_key_index is the wep key index of WEP AP. For other kinds of AP, set it to -1.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_password(uint8_t port, uint8_t *password, uint8_t password_length, uint8_t wep_key_index)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute password command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_set_password_cmd_para_t cmd_para;
    wh_set_password_resp_para_t *rsp_para;
    if(password_length > WIFI_LENGTH_PASSPHRASE) {
        return -1;
    }
    cmd_para.api_id.api = WFC_API_PASSWD;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    cmd_para.wep_key_index = wep_key_index;
    cmd_para.password_len = password_length;
    os_memcpy(cmd_para.password, password, password_length);
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_password_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;

}

/**
* @brief This function informs the wpa_supplicant to reload the configuration and applies the
* configuration settings of the Wi-Fi Configuration APIs. This function is applied to
* the following APIs: #wifi_host_config_set_ssid(), #wifi_host_config_set_security_mode(),
* #wifi_host_config_set_wpa_psk_key(), #wifi_host_config_set_wep_key() and  #wifi_host_config_set_pmk().
* In wireless station mode, the device will use the new configuration and start to scan
* and connect to the target AP router. In wireless AP mode, device will load the new
* SSID and encryption information.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_reload_setting(void)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute reload command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_set_reload_cmd_para_t cmd_para;
    wh_set_reload_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_RELOAD;
    cmd_para.api_id.set = true;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_reload_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;
}

/**
* @brief This function disconnects specific station's connection, and takes effect immediately. This
* API is available only in the AP mode.
*
* @param[in] address is station's MAC address.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_disconnect_ap(void)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute disconnect command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_set_connection_cmd_para_t cmd_para;
    wh_set_connection_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_CONNECTION;
    cmd_para.api_id.set = true;
    cmd_para.port = WIFI_PORT_STA ;
    cmd_para.disconnect = true;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_connection_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;
}

/**
* @brief This function gets the RSSI of the connected AP. It's only used for the STA mode and while the station is connected to the AP.
*
* @param[out]  rssi returns the RSSI of the connected AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*
*/
int32_t wifi_host_connection_get_rssi(int8_t *rssi)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute RSSI command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_get_connection_cmd_para_t cmd_para;
    wh_get_connection_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_CONNECTION;
    cmd_para.api_id.set = false;
    cmd_para.port = WIFI_PORT_STA ;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_get_connection_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    *rssi = rsp_para->rssi;
    return ret;

}

/**
* @brief This function gets the current STA port's link up or link down connection status.
*
* @param[out]  link_status indicates the current STA port's link up or link down connection status.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | #WIFI_STATUS_LINK_DISCONNECTED|
* \b 1                          | #WIFI_STATUS_LINK_CONNECTED|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note #WIFI_STATUS_LINK_DISCONNECTED indicates station may be in
*       IDLE/ SCAN/ CONNECTING state.
*/
int32_t wifi_host_connection_get_link_status(uint8_t *link_status)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute link status command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_get_connection_cmd_para_t cmd_para;
    wh_get_connection_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_CONNECTION;
    cmd_para.api_id.set = false;
    cmd_para.port = WIFI_PORT_STA;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_get_connection_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    *link_status = rsp_para->link_status;
    return ret;
}
/**
* @brief This function stops wifi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
static uint32_t g_wfcm_wifi_lock_sleep = 1;
int32_t wifi_host_config_set_wifi_sleep(uint8_t sleep)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute sleep command.\r\n");
        return -1;
    }
    int32_t ret;
    wh_set_sleep_cmd_para_t cmd_para;
    wh_set_sleep_cmd_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SLEEP_LOCK;
    cmd_para.sleep = sleep;
    cmd_para.api_id.set = true;

    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);

    g_wifi_host_scanning = false;
    rsp_para = (wh_set_sleep_cmd_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    if(ret == 0) {
        g_wfcm_wifi_lock_sleep = sleep;
    }

    return ret;
}

uint32_t wifi_host_config_get_wifi_sleep(void)
{
    return g_wfcm_wifi_lock_sleep;
}

