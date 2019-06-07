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
#include "wfcm_stub.h"
#include "host_os_utils.h"
#include "wifi_host_event.h"
#include "wifi_host_os_api.h"
#include "wifi_host_init.h"
#include "wfcm_at_api.h"
#include "hal.h"
#if (PRODUCT_VERSION == 2523)
#include "hif_sdio.h"
#elif (PRODUCT_VERSION == 2625)
#include "hif_spi.h"
#endif

#define RSP_BUF_SIZE 1024
uint8_t rsp_buf[RSP_BUF_SIZE];
uint32_t rsp_size;

wifi_scan_list_item_t *g_scan_list = NULL;
uint8_t g_scan_inited = 0;
uint8_t g_scan_list_size = 0;
volatile bool g_wifi_host_scanning = false;

#define WIFI_ERR_PARA_INVALID (-1)
#define WIFI_ERR_NOT_SUPPORT  (-1)

/**
* @brief This function initializes the scan table to the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list will be stored.
* @param[in] max_count is the maximum number of ap_list can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note #wifi_host_connection_scan_init() should be called before calling #wifi_host_connection_start_scan(), and it should be called only once to initialize one scan.\n
*       When the scan is done, the scanned AP list is already stored in parameter ap_list with descending order of the RSSI values.
*/

int32_t wifi_host_connection_scan_init(wifi_scan_list_item_t *ap_list, uint32_t max_count)
{
    if(g_wifi_host_scanning == true) {
        return -1;
    }

    if (NULL == ap_list) {
        return -1;
    }
    wifi_os_task_enter_critical();
    if ((1 != g_scan_inited) && (NULL == g_scan_list)) {
        g_scan_list = ap_list;
        g_scan_list_size = max_count;
        os_memset(g_scan_list, 0, g_scan_list_size * sizeof(wifi_scan_list_item_t));
        g_scan_inited = 1;
        wifi_os_task_exit_critical();
    } else {
        wifi_os_task_exit_critical();
        return -1;
    }
    return 0;

}

/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note When the scan is finished, #wifi_host_connection_scan_deinit() should be called to unload the buffer from the driver. After that, the data in the parameter ap_list can be safely processed by user applications, and then another scan can be initialized by calling #wifi_host_connection_scan_init().
*/
int32_t wifi_host_connection_scan_deinit(void)
{
    if(g_wifi_host_scanning == true) {
        return -1;
    }

    if (g_scan_inited == 0) {
        return -1;
    }
    g_scan_inited = 0;
    g_scan_list = NULL;
    g_scan_list_size = 0;
    return 0;

}

/**
* @brief This function starts Wi-Fi Scanning.
*
* @param[in] ssid specifies the SSID to be included in the probe request packet for scanning the hidden AP. If the SSID is NULL, the SSID field in probe request packet will be NULL.
* @param[in] ssid_length specifies the length of the SSID.
* @param[in] bssid specifies the BSSID of the AP to be scanned. If the BSSID is specified, the unicast probe request is sent. If the BSSID is NULL, then the broadcast probe request is sent.
* @param[in] scan_mode is the scan mode that can be either a full scan or a partial scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Full|
* \b 1                          | Partial, it's expected to be used in the SoftAP mode and keep stations online.|
*
* In the partial scan mode, the scanning is performed as follows.
* If the AP is currently on channel 5, it jumps to channels (1, 2),
* and then back to channel 5. After that it scans the channels (3, 4).
* The number of channels it scans each time is specified in the implementation.
* @param[in] scan_option selects scan options based on one of the following: active scan, passive scan or force active scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Active scan, (Passive in regulatory channels). The channels that will be scanned are decided by the product's country and region. A probe request will be sent to each of these channels.|
* \b 1                          | Passive in all channels. Receives and processes the beacon. No probe request is sent.|
* \b 2                          | Force active (active in all channels). Forced to active scan in all channels. A probe request will be sent to each of these channels.|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note 1. No matter what the SSID and BSSID parameters are, the device will receive all beacon and probe response packets over the air, \n
* and record the result into ap_list buffer registered by #wifi_host_connection_scan_init().
* @note 2. The station mode supports only the full scan mode.
* @note 3. The AP/repeater/p2p GO mode supports only the partial scan mode.
*/
int32_t wifi_host_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute scan command.\r\n");
        return -1;
    }

    if(g_wifi_host_scanning == true) {
        return -1;
    }

    if ((NULL != ssid) && (ssid_length > WIFI_MAX_LENGTH_OF_SSID)) {
        //LOG_E(wifi, "ssid_length is invalid: %d", ssid_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (scan_mode > 1) {
        //LOG_E(wifi, "scan_mode is invalid: %d", scan_mode);
        return WIFI_ERR_PARA_INVALID;
    }
    if (scan_option > 2) {
        //LOG_E(wifi, "scan_option is invalid: %d", scan_option);
        return WIFI_ERR_PARA_INVALID;
    }


    int32_t ret;
    wh_set_scan_start_cmd_para_t cmd_para;
    wh_set_scan_start_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SCAN_START;
    cmd_para.api_id.set = true;
    if(ssid != NULL) {
        os_memcpy(cmd_para.ssid, ssid, ssid_length);
        cmd_para.bssid_valid = true;
    } else {
        cmd_para.bssid_valid = false;
    }
    if(bssid != NULL) {
        os_memcpy(cmd_para.bssid, bssid, WIFI_MAC_ADDRESS_LENGTH);
        cmd_para.bssid_valid = true;
    } else {
        cmd_para.bssid_valid = false;
    }

    cmd_para.scan_mode = scan_mode;
    cmd_para.scan_option = scan_option;

    cmd_para.support_number = g_scan_list_size;

    ret = wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);

    rsp_para = (wh_set_scan_start_resp_para_t *)rsp_buf;

    if ((ret == WFC_RSP_OK) && rsp_para->error_code >= 0){
        g_wifi_host_scanning = true;
        ret = rsp_para->error_code;
    }

    return ret; 

    //return wifi_inband_start_scan(ssid, ssid_length, bssid, scan_mode, scan_option);

}


/**
* @brief This function stops the Wi-Fi Scanning triggered by #wifi_host_connection_start_scan(). If the device in station mode cannot connect to an AP, it keeps scanning till it connects to the target AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_stop_scan(void)
{
    if(wifi_get_init_status() == false){
        printf("Wi-Fi is not ready, can't execute scan command.\r\n");
        return -1;
    }

    int32_t ret;
    wh_set_scan_stop_cmd_para_t cmd_para;
    wh_set_scan_stop_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SCAN_STOP;
    cmd_para.api_id.set = true;

    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);

    g_wifi_host_scanning = false;
    rsp_para = (wh_set_scan_stop_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;

    return ret;
    //return wifi_inband_stop_scan();

}

/**
* @brief This function registers the Wi-Fi event handler. Each event can register multiple event handlers with maximum number of 16.
* In the AP/STA mode, the scan stops automatically when the scan cycle is finished
* and SCAN COMPLETE event is triggered;\n
* In the AP mode, once the device is disconnected from the station,  a DISCONNECT event with station's
* MAC address is triggered;\n
* In the AP mode, once the station connects to the device, CONNECT event with station's
* MAC address is triggered;\n
* In the STA mode, once the device disconnects from the AP, DISCONNECT event with BSSID is triggered;\n
* In the STA mode, once the device connect to the AP, CONNECT event with BSSID is triggered.\n
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  It is not recommended to call any other Wi-Fi APIs or sleep in any form in the event handler.
*/
int32_t wifi_host_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler)
{
    return wifi_api_set_event_handler(1, event, handler);
}

/**
* @brief This function unregisters Wi-Fi event handler. The function #wifi_host_connection_register_event_handler() registers an event and matches it with the corresponding event handler. For the event behavior, please refer to #wifi_host_connection_register_event_handler().
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t for more details.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler)
{
    return wifi_api_set_event_handler(0, event, handler);
}

/**
* @brief This function starts wifi module. \n
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
static uint8_t g_wifi_start_flag = 0;
int32_t wifi_host_config_set_wifi_start(void)
{
    int32_t ret = 0;
    if(g_wifi_start_flag == 0) {
#if (PRODUCT_VERSION == 2523)
        hal_gpio_set_output(MT5932_RESET_PIN, HAL_GPIO_DATA_HIGH); 
#elif (PRODUCT_VERSION == 2625)
        hal_gpio_set_output(SPIM_PIN_NUMBER_MT5932_RESET, HAL_GPIO_DATA_HIGH); //pull down chip_en
#endif        
        wifi_host_wfcm_init();
#if (PRODUCT_VERSION == 2523)
        wifi_host_main_init();
#endif        
        g_wifi_start_flag = 1;
    }else {
        printf("WiFi already start,no need to start again.\r\n");
    }
    return ret;
}


/**
* @brief This function stops wifi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_stop(void)
{
    int32_t ret = 0;
    if(g_wifi_start_flag == 1) {
#if (PRODUCT_VERSION == 2523)
        hal_gpio_set_output(MT5932_RESET_PIN, HAL_GPIO_DATA_LOW); //pull down chip_en
#elif (PRODUCT_VERSION == 2625)
        hal_gpio_set_output(SPIM_PIN_NUMBER_MT5932_RESET, HAL_GPIO_DATA_LOW); //pull down chip_en
#endif
        wifi_set_init_status(false);
        wfcm_stub_deinit();
        g_wifi_start_flag = 0;
    }else {
        printf("WiFi already stop,no need to stop again.\r\n");
    }
    return ret;
}

