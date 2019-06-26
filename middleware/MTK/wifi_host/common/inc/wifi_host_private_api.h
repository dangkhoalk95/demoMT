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
 * @file wifi_host_private_api.h
 *
 * Wi-Fi processor configuration / settings for the STA operations.
 *
 */

/**@addtogroup WIFI
* @{
* This section introduces the Wi-Fi driver APIs including terms and acronyms, supported features,
* software architecture, details on how to use this driver, Wi-Fi function groups, enumerations,
* structures and functions.
* The host APIs are used to configure slave Wi-Fi behaviour.
*
* Terms                         |Details                                                                 |
* ------------------------------|------------------------------------------------------------------------|
* \b STA                        | In station (STA) mode the device operates as a client that connects to a Wi-Fi access point.|
* \b AP                         | In Access Point (AP) mode other devices can connect to the Wi-Fi access point.|
* \b APCLI                      | In AP Client (APCLI) mode the Access Point becomes a wireless client for another AP.|
* \b DTIM                       | Delivery Traffic Indication Message (DTIM) is how the AP (wireless router) warns its clients that it is about to transmit the multicast (and broadcast*) frames it queued up since the previous DTIM. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Delivery_traffic_indication_message">introduction to DTIM in Wikipedia</a>. |
* \b MCS                        | Modulation and Coding Set (MCS) implies the packet data rate. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to MCS in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard to create a secure wireless home network. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b PIN                        | Personal identification number (PIN) is a method of WPS, has to be read from either a sticker or display on the new wireless device. This PIN must then be entered at the "representant" of the network, usually the network's AP. Alternately, a PIN provided by the AP may be entered into the new device. This method is the mandatory baseline mode and everything must support it. The Wi-Fi direct specification supersedes this requirement by stating that all devices with a keypad or display must support the PIN method.|
* \b PBC                        | Push button (PBC) is a method of WPS. For more information, please refer to <a herf="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to PBC in Wikipedia</a>.|
* \b STBC                       | Space-time Block Code (STBC) used in wireless telecommunications. Space-time block coding is a technique used in wireless communications to transmit multiple copies of a data stream across a number of antennas and to exploit the various received versions of the data to improve the reliability of data-transfer. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Space%E2%80%93time_block_code">introduction to STBC in Wikipedia</a>.|
* \b FCS                        | Frame Check Sequence (FCS) refers to the extra error-detecting code added to a frame in a communications protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Frame_check_sequence">introduction to FCS in Wikipedia</a>.|
* \b Broadcast                  | A communication where a piece of information is sent from one point to all other points.|
* \b Multicast                  | A communication where a piece of information is sent from one or more points to a set of other points.|
* \b RTS                        | Request to Send (RTS) is IEEE 802.11 control signal is an optional mechanism used by the 802.11 wireless networking protocol to reduce frame collisions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RTS/CTS">introduction to RTS in Wikipedia</a>.|
* \b CTS                        | Clear to Send (CTS) is IEEE 802.11 control signal is an optional mechanism used by the 802.11 wireless networking protocols to reduce frame collisions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RTS/CTS">introduction to CTS in Wikipedia</a>.|
* \b RSSI                       | Received Signal Strength Indication (RSSI). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Received_signal_strength_indication">introduction to RSSI in Wikipedia</a>. |
* \b Beacon                     | Beacon frame is one of the management frames in IEEE 802.11 based WLANs.|
* \b WPA                        | Wi-Fi Protected Access (WPA) is a wireless encryption standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Access">introduction to WPA in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b OPMODE                     | Operate Mode (OPMODE) is STA/AP/APCLI.|
* \b PSK                        | Pre-shared key (PSK) is a method to set encryption keys. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Pre-shared_key">introduction to PSK in Wikipedia</a>.|
* \b PMK                        | Pair-wise Master Key (PMK) is calculated from PSK.|
* \b WEP                        | Wired Equivalent Privacy (WEP) is a wireless network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wired_Equivalent_Privacy">introduction to WEP in Wikipedia</a>.|
* \b WOW                        | Wake on Wireless (WOW) is a technology that allows remote wake-up of workstations from a standby power state to facilitate device management. |
* \b TKIP                       | Temporal Key Integrity Protocol (TKIP) is an algorithm used to secure wireless computer networks. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Temporal_Key_Integrity_Protocol">introduction to TKIP in Wikipedia</a>.|
* \b AES                        | Advanced Encryption Standard process (AES) is a 'symmetric block cipher' for encrypting texts which can be decrypted with the original encryption key. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard">introduction to AES in Wikipedia</a>.|
* \b PHY                        | PHY of the Open system interconnection (OSI) model and refers to the circuitry required to implement physical layer functions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/PHY_(chip)">introduction to PHY in Wikipedia</a>.|
* \b BSS                        | The basic service set (BSS) provides the basic building-block of an 802.11 wireless LAN. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b IBSS                       | With 802.11, one can set up an ad hoc network of client devices without a controlling access point; the result is called an IBSS (independent BSS). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b MCS                        | Modulation and Coding Scheme (MCS) is related to the maximum data rate and robustness of data transmission. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to Link adaptation in Wikipedia</a>.|
* \b LDPC                       | In information theory, a low-density parity-check (LDPC) code is a linear error correcting code, a method of transmitting a message over a noisy transmission channel. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Low-density_parity-check_code">introduction to Low-density parity-check code in Wikipedia</a>.|
* \b GI                         | In telecommunications, guard intervals (GI) are used to ensure that distinct transmissions do not interfere with one another. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Guard_interval">introduction to Guard interval in Wikipedia</a>.|
* \b MPDU                       | MPDU stands for MAC protocol data unit.|
* \b TSC                        | TKIP uses a per-MPDU TKIP sequence counter (TSC) to sequence the MPDUs it sends. The receiver drops MPDUs received out of order, i.e., not received with increasing sequence numbers.|
*
*/

#ifndef __WIFI_HOST_PRIVATE_API_H__
#define __WIFI_HOST_PRIVATE_API_H__

#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/


/**@brief The maximum length of passphrase used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PASSPHRASE              (64)

/**
* @brief Station operation mode. In this mode the device works as a Wi-Fi client.
*/
#define WIFI_MODE_STA_ONLY      (1)

/**
* @brief SoftAP operation mode. In Access Point (AP) mode, other client devices can connect to the Wi-Fi AP.
*/
#define WIFI_MODE_AP_ONLY       (2)

/**
* @brief Specifies the STA operation.
*/
#define WIFI_PORT_STA           (0)

/**
* @brief Specifies the AP operation.
*/
#define WIFI_PORT_AP            (1)

#define WIFI_NUMBER_WEP_KEYS     (4)    /**<  The group number of WEP keys. */
#define WIFI_MAX_WEP_KEY_LENGTH  (26)   /**<  The maximum length of each WEP key. */

/**
* @}
*/


/**@defgroup WIFI_ENUM Enumeration
* @{
*/

/** @brief This enumeration defines the wireless physical mode.
*/
typedef enum {
    WIFI_PHY_11BG_MIXED = 0,     /**<  0, 2.4GHz band. */
    WIFI_PHY_11B,                /**<  1, 2.4GHz band. */
    WIFI_PHY_11A,                /**<  2, 5GHz band. */
    WIFI_PHY_11ABG_MIXED,        /**<  3, both 2.4G and 5G band. */
    WIFI_PHY_11G,                /**<  4, 2.4GHz band. */
    WIFI_PHY_11ABGN_MIXED,       /**<  5, both 2.4G and 5G band. */
    WIFI_PHY_11N_2_4G,           /**<  6, 11n-only with 2.4GHz band. */
    WIFI_PHY_11GN_MIXED,         /**<  7, 2.4GHz band. */
    WIFI_PHY_11AN_MIXED,         /**<  8, 5GHz band. */
    WIFI_PHY_11BGN_MIXED,        /**<  9, 2.4GHz band. */
    WIFI_PHY_11AGN_MIXED,        /**< 10, both 2.4G and 5G band. */
    WIFI_PHY_11N_5G,             /**< 11, 11n-only with 5GHz band. */
} wifi_phy_mode_t;

/** @brief This enumeration defines 40MHz bandwidth extension.
*/
typedef enum {
    WIFI_BANDWIDTH_EXT_40MHZ_UP,    /**< 40MHz up. */
    WIFI_BANDWIDTH_EXT_40MHZ_BELOW  /**< 40MHz below. */
} wifi_bandwidth_ext_t;

/**
* @brief This enumeration defines three power saving modes.
*/
typedef enum {
    WIFI_POWER_SAVING_MODE_OFF = 0,          /**<  WIFI_POWER_SAVING_MODE_OFF is a power saving mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power saving setting consumes the most power but offers the highest throughput. */
    WIFI_POWER_SAVING_MODE_LEGACY,           /**<  WIFI_POWER_SAVING_MODE_LEGACY: The access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it has responded to each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operated devices). */
    WIFI_POWER_SAVING_MODE_FAST              /**<  WIFI_POWER_SAVING_MODE_FAST is a power saving mode that switches between power saving and WIFI_POWER_SAVING_MODE_OFF modes, depending on the network traffic. For example, it switches to WIFI_POWER_SAVING_MODE_OFF mode after receiving a large number of packets and switches back to power saving mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.  */
} wifi_power_saving_mode_t;

/**
* @}
*/

/**@defgroup WIFI_STRUCT Structure
* @{
*/

/** @brief This structure is the Wi-Fi configuration for initialization in STA mode.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< The SSID of the target AP. */
    uint8_t ssid_length;                      /**< The length of the SSID. */
    uint8_t bssid_present;                    /**< The BSSID is present if it is set to 1. Otherwise, it is set to 0. */
    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< The MAC address of the target AP. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the target AP. */
    uint8_t password_length;                  /**< The length of the password. If the length is 64, the password is regarded as PMK. */
} wifi_sta_config_t;


/** @brief This structure is the Wi-Fi configuration for initialization in AP mode.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< The SSID of the AP. */
    uint8_t ssid_length;                      /**< The length of the SSID. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the AP. */
    uint8_t password_length;                  /**< The length of the password. */
    wifi_auth_mode_t auth_mode;               /**< The authentication mode. */
    wifi_encrypt_type_t encrypt_type;         /**< The encryption mode. */
    uint8_t channel;                          /**< The channel. */
    uint8_t bandwidth;                        /**< The bandwidth that is either #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ or #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ. */
    wifi_bandwidth_ext_t bandwidth_ext;       /**< The bandwidth extension. It is only applicable when the bandwidth is set to #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ. */
} wifi_ap_config_t;


/** @brief Wi-Fi configuration for initialization.
*/
typedef struct {
    uint8_t opmode;                          /**< The operation mode. The value should be #WIFI_MODE_STA_ONLY, #WIFI_MODE_AP_ONLY, #WIFI_MODE_REPEATER or #WIFI_MODE_MONITOR*/
    wifi_sta_config_t sta_config;            /**< The configurations for the STA. It should be set when the OPMODE is #WIFI_MODE_STA_ONLY or #WIFI_MODE_REPEATER. */
    wifi_ap_config_t ap_config;              /**< The configurations for the AP. It should be set when the OPMODE is #WIFI_MODE_AP_ONLY or #WIFI_MODE_REPEATER. */
} wifi_config_t;


/** @brief Wi-Fi extention configuration for initialization.
*/
typedef struct {
    uint32_t sta_wep_key_index_present: 1;        /**< Set to 1 to mark the presence of the sta_wep_key_index, set to 0, otherwise. */
    uint32_t sta_auto_connect_present: 1;         /**< Set to 1 to mark the presence of the sta_auto_connect, set to 0, otherwise. */
    uint32_t ap_wep_key_index_present: 1;         /**< Set to 1 to mark the presence of the ap_wep_key_index, set to 0, otherwise. */
    uint32_t ap_hidden_ssid_enable_present: 1;    /**< Set to 1 to mark the presence of the ap_hidden_ssid_enable, set to 0, otherwise. */
    uint32_t country_code_present: 1;             /**< Set to 1 to mark the presence of the country_code[4], set to 0, otherwise. */
    uint32_t sta_bandwidth_present: 1;            /**< Set to 1 to mark the presence of the sta_bandwidth, set to 0, otherwise. */
    uint32_t sta_wireless_mode_present: 1;        /**< Set to 1 to mark the presence of the sta_wireless_mode, set to 0, otherwise. */
    uint32_t sta_listen_interval_present: 1;      /**< Set to 1 to mark the presence of the sta_listen_interval, set to 0, otherwise. */
    uint32_t sta_power_save_mode_present: 1;      /**< Set to 1 to mark the presence of the sta_power_save_mode, set to 0, otherwise. */
    uint32_t ap_wireless_mode_present: 1;         /**< Set to 1 to mark the presence of the ap_wireless_mode, set to 0, otherwise. */
    uint32_t ap_dtim_interval_present: 1;         /**< Set to 1 to mark the presence of the ap_dtim_interval, set to 0, otherwise. */
    uint32_t reserved_bit: 21;                    /**< Reserved. */
    uint32_t reserved_word[3];                    /**< Reserved. */

    uint8_t sta_wep_key_index;                    /**< The WEP key index for STA. It should be set when the STA uses the WEP encryption. */
    uint8_t sta_auto_connect;                     /**< Set to 1 to enable the STA to automatically connect to the target AP after the initialization. Set to 0 to force the STA to stay idle after the initialization and to call #wifi_config_reload_setting() to trigger connection. The default is set to 1. */
    uint8_t ap_wep_key_index;                     /**< The WEP key index for AP. It should be set when the AP uses WEP encryption. */
    uint8_t ap_hidden_ssid_enable;                /**< Set to 1 to enable the hidden SSID in the beacon and probe response packets. The default is set to 0. */
    uint8_t country_code[4];                      /**< The country code setting. */
    uint8_t sta_bandwidth;                        /**< The bandwidth setting for STA. The value is either #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ or #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ, or WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ.*/
    wifi_phy_mode_t sta_wireless_mode;            /**< The wireless mode setting for STA. Please refer to the definition of #wifi_phy_mode_t.*/
    uint8_t sta_listen_interval;                  /**< The listening interval setting for STA. The interval range is from 1 to 255 beacon intervals.*/
    wifi_power_saving_mode_t sta_power_save_mode; /**< The power saving mode setting for STA. Please refer to the definition of #wifi_power_saving_mode_t.*/
    wifi_phy_mode_t ap_wireless_mode;             /**< The wireless mode setting for AP. Please refer to the definition of #wifi_phy_mode_t.*/
    uint8_t ap_dtim_interval;                     /**< The DTIM interval setting for AP. The interval range is from 1 to 255 beacon intervals. */

} wifi_config_ext_t;

/** @brief Wi-Fi WEP keys.
*/
typedef struct {
    uint8_t wep_key[WIFI_NUMBER_WEP_KEYS][WIFI_MAX_WEP_KEY_LENGTH];     /**< There are four WEP keys. */
    uint8_t wep_key_length[WIFI_NUMBER_WEP_KEYS];                       /**< WEP key length. */
    uint8_t wep_tx_key_index;                                           /**< Default key index for TX frames using WEP. */
} wifi_wep_key_t;

typedef struct {
    wifi_config_t wifi_config;
    wifi_config_ext_t wifi_config_ext;
} wifi_host_system_config_t;

/**
*@}
*/



/**
* @brief This function initializes the Wi-Fi module. It is not supported in MT2625 host.
*
* @param[in] wifi_host_cfg is the Wi-Fi configuration to be set, it should not be null.
*
* @note Call this function only once at the initialization stage.
*/
int32_t wifi_host_config_init(wifi_host_system_config_t *pwifi_host_cfg);

/**
* @brief This function gets the MAC address of a specific wireless port used by the Wi-Fi driver. It is not supported in MT2625 host.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  address is the MAC address. The length of the MAC address is #WIFI_MAC_ADDRESS_LENGTH in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note To get the MAC address, follow these steps:
*       1. Read eFuse from address 0x00, take bytes 4 to 9 as the STA MAC address and generate AP address from it by increasing the last byte by 1.
*       2. If NVDM is enabled, read NVDM from group "STA" and key "MacAddr" for STA MAC address and from group "AP" and key "MacAddr" for AP MAC address.
*          If the MAC address is retrieved successfully in Step 1, skip Step 2.
*          If no information is set in eFuse and NVDM, this function will return an error and consequently the Wi-Fi driver initialization will possibly fail, too.
*
*/
int32_t wifi_host_config_get_mac_address(uint8_t port, uint8_t *address);

/**
* @brief This function sets the SSID and SSID length that the Wi-Fi driver uses for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called. It is not supported in MT2625 host.
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
int32_t wifi_host_config_set_ssid(uint8_t port, uint8_t *ssid, uint8_t ssid_length);

/**
* @brief This function sets the authentication and encryption modes used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called. It is not supported in MT2625 host.
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
int32_t wifi_host_config_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type);

/**
* @brief This function sets the password used in the Wi-Fi driver for a specific wireless port. It is not supported in MT2625 host.
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

int32_t wifi_host_config_set_password(uint8_t port, uint8_t *password, uint8_t password_length, uint8_t wep_key_index);


/**
* @brief This function informs the wpa_supplicant to reload the configuration and applies the
* configuration settings of the Wi-Fi Configuration APIs. This function is applied to
* the following APIs: #wifi_host_config_set_ssid(), #wifi_host_config_set_security_mode(),
* #wifi_host_config_set_wpa_psk_key(), #wifi_host_config_set_wep_key() and  #wifi_host_config_set_pmk().
* In wireless station mode, the device will use the new configuration and start to scan
* and connect to the target AP router. In wireless AP mode, device will load the new
* SSID and encryption information. It is not supported in MT2625 host.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_reload_setting(void);


/**
* @brief This function disconnects the current connection from AP. It takes effect immediately and the device
* switches to idle state. After calling #wifi_host_config_reload_setting(), the device switches to scan state and tries to connect to an AP router with an existing Wi-Fi
* configuration. This API should be used only in the station mode. It is not supported in MT2625 host.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_host_connection_disconnect_ap(void);



/**
* @brief This function gets the current STA port's link up or link down connection status. It is not supported in MT2625 host.
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
int32_t wifi_host_connection_get_link_status(uint8_t *link_status);

/**
* @brief This function gets the RSSI of the connected AP. It's only used for the STA mode and while the station is connected to the AP.
*
* @param[out]  rssi returns the RSSI of the connected AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*
*/
int32_t wifi_host_connection_get_rssi(int8_t *rssi);

/**
* @brief This function lock/unlock wifi sleep mode. \n
* @param[in] sleep 0: unlock sleep, 1: lock sleep .
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_sleep(uint8_t sleep);

/**
* @brief This function get lock/unlock wifi sleep mode status. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
uint32_t wifi_host_config_get_wifi_sleep(void);


#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif /* __WIFI_HOST_PRIVATE_API_H__ */

