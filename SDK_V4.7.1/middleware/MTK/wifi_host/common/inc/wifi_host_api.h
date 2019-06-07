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
 * @file wifi_host_api.h
 *
 * Wi-Fi processor configuration Settings for STA operations.
 *
 */

/**@addtogroup WIFI_MT5932
* @{
* This section introduces the Wi-Fi host APIs, including terms and acronyms, Wi-Fi function groups, enumerations, structures, and functions. 
* It shows how to perform some of the Wi-Fi operations with the platforms that utilize the host chip (MT2523) and the Wi-Fi chip (MT5932).
*
* Terms                         |Details                                                                 |
* ------------------------------|------------------------------------------------------------------------|
* \b STA                        | In station (STA) mode, the device operates as a client that connects to a Wi-Fi access point.|
* \b AP                         | In Access Point (AP) mode, other devices can connect to the Wi-Fi access point.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard for creating a secure wireless home network. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b PIN                        | Personal identification number (PIN) is a WPS method in which the password for the device is shown on the actually device, usually as a sticker. This PIN must then be keyed in when the device joins the network, usually the network's AP. Alternately, a PIN provided by the AP can be entered into the new device. This method is the mandatory baseline mode and everything must support it. The Wi-Fi direct specification supersedes this requirement by stating that all devices with a keypad or display must support the PIN method.|
* \b PBC                        | Push button (PBC) is a method of WPS. For more information, please refer to <a herf="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to PBC in Wikipedia</a>.|
* \b RSSI                       | Received Signal Strength Indication (RSSI). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Received_signal_strength_indication">introduction to RSSI in Wikipedia</a>. |
* \b Beacon                     | Beacon frame is one of the management frames in IEEE 802.11 based WLANs.|
* \b WPA                        | Wi-Fi Protected Access (WPA) is a wireless encryption standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Access">introduction to WPA in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b WEP                        | Wired Equivalent Privacy (WEP) is a wireless network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wired_Equivalent_Privacy">introduction to WEP in Wikipedia</a>.|
* \b WOW                        | Wake on Wireless (WOW) is a technology that allows remote wake-up of workstations from a standby power state to facilitate device management. |
* \b TKIP                       | Temporal Key Integrity Protocol (TKIP) is an algorithm used to secure wireless computer networks. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Temporal_Key_Integrity_Protocol">introduction to TKIP in Wikipedia</a>.|
* \b AES                        | Advanced Encryption Standard process (AES) is a 'symmetric block cipher' for encrypting texts which can be decrypted with the original encryption key. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard">introduction to AES in Wikipedia</a>.|
* \b BSS                        | The basic service set (BSS) provides the basic building-block of an 802.11 wireless LAN. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b IBSS                       | With 802.11, you can set up an ad hoc network of client devices without a controlling access point. The result is called an Independent BSS (IBSS). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
*
*/

#ifndef __WIFI_HOST_API_H__
#define __WIFI_HOST_API_H__

#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/

/**@brief The maximum length of a SSID.
*/
#define WIFI_MAX_LENGTH_OF_SSID             (32)

/**@brief MAC address length.
*/
#define WIFI_MAC_ADDRESS_LENGTH             (6)
/**
* @}
*/


/**@defgroup WIFI_ENUM Enumeration
* @{
*/
/**@brief This enumeration defines the supported events generated by the Wi-Fi driver. The event is sent to the upper layer handler that is registered in #wifi_host_connection_register_event_handler().
*/
typedef enum {
    WIFI_EVENT_IOT_CONNECTED = 0,               /**< Connected event. This is not currently supported.*/
    WIFI_EVENT_IOT_SCAN_COMPLETE,               /**< Scan completed event.*/
    WIFI_EVENT_IOT_DISCONNECTED,                /**< Disconnected event. This is not currently supported.*/
    WIFI_EVENT_IOT_PORT_SECURE,                 /**< Secure event(it is usually processed as a request in supplicant). It can be used when DHCP start. This is not currently supported.*/
    WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,/**< Sends notification to show whether the beacon or probe request is received. This is not currently supported.*/
    WIFI_EVENT_IOT_WPS_COMPLETE,                /**< Credential event sends a notification to show when the WPS process is complete. This is not currently supported.*/
    WIFI_EVENT_IOT_INIT_COMPLETE,               /**< Initialization event is complete. */
    WIFI_EVENT_IOT_REPORT_FILTERED_FRAME,       /**< Report the expected packet frame. This is not currently supported.*/
    WIFI_EVENT_IOT_CONNECTION_FAILED,           /**< Connection has failed. This option is not yet available. This is not currently supported.*/
    WIFI_EVENT_MAX_NUMBER
} wifi_event_t;

/** @brief This enumeration defines the wireless authentication mode to indicate the Wi-Fi device’s authentication attribute.
*/
typedef enum {
    WIFI_AUTH_MODE_OPEN = 0,                        /**< Open mode.     */
    WIFI_AUTH_MODE_SHARED,                          /**< Not supported. */
    WIFI_AUTH_MODE_AUTO_WEP,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA,                             /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK,                         /**< WPA_PSK.       */
    WIFI_AUTH_MODE_WPA_None,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA2,                            /**< Not supported. */
    WIFI_AUTH_MODE_WPA2_PSK,                        /**< WPA2_PSK.      */
    WIFI_AUTH_MODE_WPA_WPA2,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK,                /**< Mixture mode.  */
} wifi_auth_mode_t;

/** @brief This enumeration defines the wireless encryption type to indicate the Wi-Fi device’s encryption attribute.
*/
typedef enum {
    WIFI_ENCRYPT_TYPE_WEP_ENABLED = 0,                                              /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_ENCRYPT1_ENABLED = WIFI_ENCRYPT_TYPE_WEP_ENABLED,             /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_WEP_DISABLED = 1,                                             /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_DISABLED = WIFI_ENCRYPT_TYPE_WEP_DISABLED,            /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT = 2,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_KEY_ABSENT = WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT,        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED = 3,                                        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_NOT_SUPPORTED = WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED,  /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_ENABLED = 4,                                             /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT2_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_ENABLED,            /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_AES_ENABLED = 6,                                              /**< AES encryption.    */
    WIFI_ENCRYPT_TYPE_ENCRYPT3_ENABLED = WIFI_ENCRYPT_TYPE_AES_ENABLED,             /**< AES encryption.     */
    WIFI_ENCRYPT_TYPE_AES_KEY_ABSENT = 7,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_AES_MIX = 8,                                             /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT4_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX,            /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_TKIP_AES_KEY_ABSENT = 9,                                      /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP40_ENABLED = 10,                                     /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP104_ENABLED = 11,                                    /**< Not supported.     */
} wifi_encrypt_type_t;

/** @brief This enumeration of the device password ID is used for WPS.
* This enumeration is only applicable when the boolean variable selected_registrar inside #wifi_wps_element_t is true.
* For more information, please refer to Table 37, "Device Password ID" in chapter 12, "Data Element Definitions"
* of the Wi-Fi Simple Configuration Technical Specification V2.0.5.
*/
typedef enum {
    WIFI_WPS_DPID_DEFAULT             = 0x0000,     /**<  Default value or use PIN if selected_registrar is true. */
    WIFI_WPS_DPID_USER_SPECIFIED      = 0x0001,     /**<  User specified. */
    WIFI_WPS_DPID_MACHINE_SPECIFIED   = 0x0002,     /**<  Machine specified. */
    WIFI_WPS_DPID_REKEY               = 0x0003,     /**<  Re-key. */
    WIFI_WPS_DPID_PUSHBUTTON          = 0x0004,     /**<  Push-button. */
    WIFI_WPS_DPID_REGISTRAR_SPECIFIED = 0x0005,     /**<  Registrar specified. */
    WIFI_WPS_DPID_MAX_NUMBER          = 0xFFFF
} wps_device_password_id_t;

/**
* @brief This enumeration lists the configuration methods that are supported by the Enrollee or Registrar.
* For more information, please refer to Table 33, "Configuration Methods" in chapter 12, "Data Element Definitions"
* of the Wi-Fi Simple Configuration Technical Specification V2.0.5.
*/
typedef enum {
    WIFI_WPS_CONFIG_USBA                    = 0x0001,  /**<  @deprecated Ignored in WPS version 2.0. */
    WIFI_WPS_CONFIG_ETHERNET                = 0x0002,  /**<  @deprecated Ignored in WPS version 2.0. */
    WIFI_WPS_CONFIG_LABEL                   = 0x0004,  /**<  8-digit static PIN.  */
    WIFI_WPS_CONFIG_DISPLAY                 = 0x0008,  /**<  A dynamic PIN from a display. */
    WIFI_WPS_CONFIG_EXTERNAL_NFC_TOKEN      = 0x0010,  /**<  An NFC tag is used for WPS.  */
    WIFI_WPS_CONFIG_INTEGRATED_NFC_TOKEN    = 0x0020,  /**<  The NFC tag is integrated with the device.  */
    WIFI_WPS_CONFIG_NFC_INTERFACE           = 0x0040,  /**<  The device contains an NFC interface. */
    WIFI_WPS_CONFIG_PUSHBUTTON              = 0x0080,  /**<  WPS PBC supports either a physical or a virtual push-button. */
    WIFI_WPS_CONFIG_KEYPAD                  = 0x0100,  /**<  A PIN can be keyed into the device. */
    WIFI_WPS_CONFIG_VIRTUAL_PUSHBUTTON      = 0x0280,  /**<  WPS PBC supports a software user interface. */
    WIFI_WPS_CONFIG_PHY_PUSHBUTTON          = 0x0480,  /**<  WPS PBC is available as a physical button on the device. */
    WIFI_WPS_CONFIG_VIRTUAL_DISPLAY         = 0x2008,  /**<  The dynamic PIN is visible via a remote user interface, such as an HTML page. */
    WIFI_WPS_CONFIG_PHY_DISPLAY             = 0x4008   /**<  The dynamic PIN is shown on a display that is part of the device. */
} wps_config_method_t;

/**
* @}
*/

/**@defgroup WIFI_TYPEDEF Typedef
* @{
*/
/**
* @brief This defines the Wi-Fi event handler. Call #wifi_host_connection_register_event_handler() to register a handler. The Wi-Fi driver then generates an event and sends it to the handler.
* @param[in] event is an optional event to register. For more details, please refer to #wifi_event_t.
*
* @param[in] payload  is the payload for the event.
* @param[in] length is the length of a packet.
*
* @return The return value is reserved and ignored.
*/
typedef int32_t (* wifi_event_handler_t)(wifi_event_t event, uint8_t *payload, uint32_t length);


/**
* @brief This is only made available for compatibility. Applications implemented with an earlier version of the API can work with the updated API.
*/
typedef wifi_event_handler_t wifi_event_handler;


/**
* @}
*/


/**@defgroup WIFI_STRUCT Structure
* @{
*/

/** @brief This defines the WPS element.
*/
typedef struct {
    bool selected_registrar;                        /**< Indicates whether AP WPS is triggered and in progress. 1 - triggered, 0 - not triggered. */
    wps_device_password_id_t device_password_id;    /**< Indicates the AP WPS status. This parameter only takes effect when selected_registrar is 1. Please refer to the definition of #wps_device_password_id_t. */
    wps_config_method_t configuration_methods;      /**< Indicates AP's capability to support WPS methods. Please refer to the definition of #wps_config_method_t.  */
} wifi_wps_element_t;

/** @brief This structure defines the list of scanned APs with their corresponding information.
*/
typedef struct {
    uint8_t  is_valid;                         /**< Indicates whether the scanned item is valid. */
    int8_t   rssi;                             /**< Records the RSSI value when a probe response is received. */
    uint8_t  ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< Stores the predefined SSID. */
    uint8_t  ssid_length;                      /**< Length of the SSID. */
    uint8_t  bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< MAC address of the AP. */
    uint8_t  channel;                          /**< The channel used. */
    uint8_t  central_channel;                  /**< The center channel in 40MHz bandwidth. */
    bool     is_wps_supported;                 /**< Indicates whether WPS is supported. 1-WPS is supported, 0-WPS is not supported. */
    wifi_auth_mode_t     auth_mode;            /**< Please refer to the definition of #wifi_auth_mode_t. */
    wifi_encrypt_type_t  encrypt_type;         /**< Please refer to the definition of #wifi_encrypt_type_t. */
    bool     is_hidden;                        /**< Indicates whether the SSID is hidden. 1-The SSID is hidden, 0-The SSID is not hidden. */
    uint8_t  bss_type;                         /**< Indicates the type of BSS. 0-IBSS, 1-Infrastructure. */
    uint16_t beacon_interval;                  /**< Indicates the beacon interval. */
    uint16_t capability_info;                  /**< The Capability Information field contains a number of subfields that are used to indicate requested or advertised optional capabilities. */
    wifi_wps_element_t wps_element;            /**< Please refer to the definition of #wifi_wps_element_t. */
} wifi_scan_list_item_t;

/**
*@}
*/

/**
* @brief This function initializes the scan table with the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list is stored.
* @param[in] max_count is the maximum number of ap_list that can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note #wifi_host_connection_scan_init() must be called before calling #wifi_host_connection_start_scan(), and it must be called only one time to initialize one scan.\n
*       When the scan is done, the scanned AP list is already stored in parameter ap_list with the list in descending order according to the RSSI values.
*/
int32_t wifi_host_connection_scan_init(wifi_scan_list_item_t *ap_list, uint32_t max_count);

/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note When the scan is complete, #wifi_host_connection_scan_deinit() must be called to unload the buffer from the driver. After that, the data in the parameter ap_list can be safely processed by user applications, and another scan can be initialized by calling #wifi_host_connection_scan_init().
*/
int32_t wifi_host_connection_scan_deinit(void);

/**
* @brief This function starts Wi-Fi Scanning.
*
* @param[in] ssid specifies the SSID that is included in the probe request packet for scanning the hidden AP. If the SSID is NULL, the SSID field in the probe request packet will be NULL.
* @param[in] ssid_length specifies the length of the SSID.
* @param[in] bssid specifies the BSSID of the AP to be scanned. If the BSSID is specified, the unicast probe request is sent. If the BSSID is NULL, then the broadcast probe request is sent.
* @param[in] scan_mode is the scan mode that can be either a full scan or partial scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Full|
* \b 1                          | Partial-It is expected to be used in the SoftAP mode and keep stations online. This is not currently supported.|
*
* @param[in] scan_option selects scan options based on one of the following: active scan; passive scan; or force active scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Active scan, (Passive in regulatory channels). The country and region associated with the device determine which channels are scanned. A probe request is sent to each of these channels.|
* \b 1                          | Passive in all channels. Receives and processes the beacon. No probe request is sent.|
* \b 2                          | Force active (active in all channels). Forced to active scan in all channels. A probe request is sent to each of these channels.|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note 1. No matter what the SSID and BSSID parameters are, the device receives all beacon and probe response packets over the air, \n
* and records the results in ap_list buffer registered by #wifi_host_connection_scan_init().
* @note 2. The station mode supports only the full scan mode.
*/
int32_t wifi_host_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);

/**
* @brief This function stops the Wi-Fi Scan that is triggered by #wifi_host_connection_start_scan(). If the device in station mode cannot connect to an AP, it continues scanning until it connects to the target AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_stop_scan(void);

/**
* @brief This function registers the Wi-Fi event handler. Each event can register more than one event handler with a maximum of 16 handlers.
* In the STA mode, the scan stops automatically when the scan cycle is complete
* and the SCAN COMPLETE event is triggered;\n
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  It is strongly recommended that you do not call any other Wi-Fi APIs or sleep in any form in the event handler.
*/
int32_t wifi_host_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
* @brief This function unregisters the Wi-Fi event handler. The function #wifi_host_connection_register_event_handler() registers an event and matches it with the corresponding event handler. For the event behavior, please refer to #wifi_host_connection_register_event_handler().
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t for more details.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
* @brief This function starts the Wi-Fi module. \n
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_start(void);


/**
* @brief This function stops the Wi-Fi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_stop(void);

#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif /* __WIFI_HOST_API_H__ */

