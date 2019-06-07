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

/**
 * @file wifi_api.h
 *
 * Wi-Fi processor configuration / settings for the STA operations.
 *
 */

/**@addtogroup WIFI
* @{
* This section introduces the Wi-Fi driver APIs including terms and acronyms, supported features,
* software architecture, details on how to use this driver, Wi-Fi function groups, enumerations,
* structures and functions.
*
* @image html freertos_mt5931_arch.png
*
*
* Terms                         |Details                                                                 |
* ------------------------------|------------------------------------------------------------------------|
* \b STA                        | In station (STA) mode the device operates as a client that connects to a Wi-Fi access point.|
* \b AP                         | In Access Point (AP) mode other devices can connect to the Wi-Fi access point.|
* \b APCLI                      | In AP Client (APCLI) mode the Access Point becomes a wireless client for another AP.|
* \b DTIM                       | Delivery Traffic Indication Message (DTIM) is how the AP (wireless router) warns its clients that it is about to transmit the multicast (and broadcast*) frames it queued up since the previous DTIM. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Delivery_traffic_indication_message">introduction to DTIM in Wikipedia</a>. |
* \b MCS                        | Modulation and Coding Set (MCS) implies the packet data rate. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to MCS in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup(WPS) is a network security standard to create a secure wireless home network. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b PIN                        | Personal indentification number(PIN) is a method of WPS, has to be read from either a sticker or display on the new wireless device. This PIN must then be entered at the "representant" of the network, usually the network's AP. Alternately, a PIN provided by the AP may be entered into the new device. This method is the mandatory baseline mode and everything must support it. The Wi-Fi direct specification supersedes this requirement by stating that all devices with a keypad or display must support the PIN method.|
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
* \b PHY                        | PHY of the Open system Interconnection(OSI) model and refers to the circuitry required to implement physical layer functions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/PHY_(chip)">introduction to PHY in Wikipedia</a>.|
* \b BSS                        | The basic service set (BSS) provides the basic building-block of an 802.11 wireless LAN. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b IBSS                       | With 802.11, one can set up an ad hoc network of client devices without a controlling access point; the result is called an IBSS (independent BSS). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b MCS                        | Modulation and Coding Scheme (MCS) is related to the maximum data rate and robustness of data transmission. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to Link adaptation in Wikipedia</a>.|
* \b LDPC                       | In information theory, a low-density parity-check (LDPC) code is a linear error correcting code, a method of transmitting a message over a noisy transmission channel. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Low-density_parity-check_code">introduction to Low-density parity-check code in Wikipedia</a>.|
* \b GI                         | In telecommunications, guard intervals (GI) are used to ensure that distinct transmissions do not interfere with one another. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Guard_interval">introduction to Guard interval in Wikipedia</a>.|
* \b MPDU                       | MPDU stands for MAC protocol data unit.|
* \b TSC                        | TKIP uses a per-MPDU TKIP sequence counter (TSC) to sequence the MPDUs it sends. The receiver drops MPDUs received out of order, i.e., not received with increasing sequence numbers.|
*
*/

#ifndef __WIFI_API_H__
#define __WIFI_API_H__

#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/

/**@brief The event ID to indicate the link is disconnected.
*/
#define WIFI_STATUS_LINK_DISCONNECTED       (0)

/**@brief The event ID to indicate the link is connected.
*/
#define WIFI_STATUS_LINK_CONNECTED          (1)

/**@brief The input parameter is not correctly.
*/
#define WIFI_ERR_PARA_INVALID (-1)

/** @brief Specifies the STA operation.
*/
#define WIFI_PORT_STA           (0)

/** @brief Specifies the result is successful.
*/
#define WIFI_SUCC       (0)

/** @brief Specifies the result is failed.
*/
#define WIFI_FAIL       (-1)

/**
*@}
*/


/**@defgroup WIFI_ENUM Enumeration
* @{
*/

/** @brief This enumeration defines the wireless authentication mode to indicate the Wi-Fi device authentication attribute.
*/
enum {
    WLM_WPA_AUTH_DISABLED        = 0x0000,    /**<  Legacy (i.e., non-WPA).  */
    WLM_WPA_AUTH_NONE            = 0x0001,    /**<  none (IBSS)                    */
    WLM_WPA_AUTH_PSK             = 0x0002,    /**<  WPA PAK                         */
    WLM_WPA2_AUTH_PSK            = 0x0003,    /**<  WPA2 PSK                       */
    WLM_WPA_WPA2_AUTH_PSK        = 0x0004,    /**<  WPA/WPA2 Mixturemode */
    WLM_WPA_AUTH_ENTERPRISE      = 0x0005,    /**<  Not supported.       */
    WLM_WPA2_AUTH_ENTERPRISE     = 0x0006,    /**<  Not supported.        */
    WLM_WPA_WPA2_AUTH_ENTERPRISE = 0x0007,    /**<  Not supported.        */
    WLM_WAPI_AUTH_PSK            = 0x0008,    /**<  Not supported.        */
    WLM_WAPI_AUTH_WAI            = 0x0009,    /**<  Not supported.        */
    WLM_AUTH_MAX
};

/** @brief This enumeration defines the wireless encryption mode to indicate the Wi-Fi device encryption attribute.
*/
enum {
    WLM_ENCRYPT_NONE     = 0x0000,     /**<  No encryption. */
    WLM_ENCRYPT_WEP      = 0x0001,     /**<  WEP encryption. */
    WLM_ENCRYPT_TKIP     = 0x0002,     /**<  TKIP encryption. */
    WLM_ENCRYPT_AES      = 0x0003,     /**<  AES encryption. */
    WLM_ENCRYPT_TKIP_AES = 0x0004,     /**<  TKIP AES encryption. */
    WLM_ENCRYPT_WSEC     = 0x0005,     /**<  Software WSEC encryption. */
    WLM_ENCRYPT_FIPS     = 0x0006,     /**<  FIPS encryption. */
    WLM_ENCRYPT_MAX
};

/**@brief This enumeration defines the supported events generated by the Wi-Fi driver. The event will be sent to the upper layer handler registered in #wifi_connection_register_event_handler().
*/
typedef enum {
    WIFI_EVENT_IOT_DISCONNECTED = 0,     /**< Disconnected event.*/
    WIFI_EVENT_IOT_PORT_SECURE,          /**< Secure event, mainly processed in supplicant. It can be used at the DHCP start.*/
    WIFI_EVENT_IOT_INIT_DONE,            /**< WIFI Init event.*/
    WIFI_EVENT_IOT_SCAN_COMPLETE,        /**< WIFI Scan complete event.*/
    WIFI_EVENT_MAX_NUMBER
} wifi_event_t;

/**@brief This enumeration defines the supported power saving mode of the Wi-Fi chip.
*/
enum {
    Param_PowerModeCAM      = 0x0000,     /**< Continuous active mode.*/
    Param_PowerModeMAX_PSP  = 0x0001,     /**< Max power saving mode.*/
    Param_PowerModeFast_PSP = 0x0002,     /**< Fast power saving mode.*/
    Param_PowerModeMax      = 0x0003,
};


/**
*@}
*/


/**@defgroup WIFI_TYPEDEF Typedef
* @{
*/

/**@brief This defines the wireless authentication mode type.
*/
typedef int WLM_AUTH_MODE;

/**@brief This defines the wireless encryption mode type.
*/
typedef int WLM_ENCRYPTION;

/**@brief This defines the power saving mode type.
*/
typedef int PARAM_POWER_MODE;

/**@brief This defines the pointer for power saving mode type.
*/
typedef int *PPARAM_POWER_MODE;

/**@brief This defines the OS time type.
*/
typedef long os_time_t;

/**
* @brief This defines the Wi-Fi event handler. Call #wifi_connection_register_event_handler() to register a handler, when the Wi-Fi driver generates an event and sends it to the handler.
* @param[in] event  is an optional event to register. For more details, please refer to #wifi_event_t.
* @param[in] payload  is the payload for the event.
* @param[in] length is the length of a packet.
*
* @return The return value is reserved and it is ignored.
*/
typedef int32_t (* wifi_event_handler_t)(wifi_event_t event, uint8_t *payload, uint32_t length);


/**
*@}
*/


/**@defgroup WIFI_STRUCT Structure
* @{
*/

/** @brief This structure is the STA Wi-Fi connection information.
*/
typedef struct wl_connection_info {
    uint8_t    Ssid_len;    /**< SSID length of the connected AP's SSID */
    uint8_t    Ssid[32];    /**< SSID of the connected AP */
    int32_t    Rssi;        /**< Rx RSSI of the connected AP */
    int32_t    Phy_rate;    /**< physical rate with the connected AP */
    int32_t    channel;     /**< channel of the connected AP */
} wl_connection_info_t;

/** @brief This structure is the BSS  information.
*/
typedef struct _WLAN_BSS_INFO {
    uint32_t      version;        /**< version field */
    uint32_t      length;         /**< byte length of data in this record,
                                                        * starting at version and including IEs   */
    uint8_t       BSSID[6];       /**< BSSID of BSS */
    uint16_t      beacon_period;  /**< units are Kusec */
    uint16_t      capability;     /**< Capability information */
    uint8_t       SSID_len;       /**< ssid length */
    uint8_t       SSID[32];       /**< ssid of bss */

    struct {
        uint32_t  count;           /**< rates in this set */
        uint8_t   rates[16];       /**< rates in 500kbps units*/
    } rateset;                     /**< struct name of rate set */

    uint16_t      chanspec;       /**< chanspec for bss */
    uint16_t      atim_window;    /**< units are Kusec */
    uint8_t       dtim_period;    /**< DTIM period */
    int32_t       RSSI;           /**< receive signal strength (in dBm) */
    int8_t        phy_noise;      /**< noise (in dBm) */
    uint8_t       n_cap;          /**< BSS is 802.11N Capable */
    uint32_t      nbss_cap;       /**< 802.11N BSS Capabilities (based on HT_CAP_*) */
    uint8_t       ctl_ch;         /**< 802.11N BSS control channel number */
    uint32_t      reserved32[1];  /**< Reserved for expansion of BSS properties */
    uint8_t       flags;          /**< flags */
    uint8_t       reserved[3];    /**< Reserved for expansion of BSS properties */
    uint8_t       basic_mcs[16];  /**< 802.11N BSS required MCS set */
    uint16_t      ie_offset;      /**< offset at which IEs start, from beginning */
    uint32_t      ie_length;      /**< byte length of Information Elements */
    uint16_t      SNR;            /**< average SNR of during frame reception */
    WLM_AUTH_MODE     authMode;   /**< wireless authentication mode */
    WLM_ENCRYPTION    encryption; /**< wireless encryption type */
} WLAN_BSS_INFO;

/** @brief This structure is the scan result information.
*/
typedef struct _WLAN_SCAN_RESULT {
    uint32_t buflen;            /**< Scan result buffer length */
    uint32_t version;           /**< Reserved */
    uint32_t count;             /**< Scan result bss number  */
    WLAN_BSS_INFO bss_info[1];  /**< Scan result bss content  */
} WLAN_SCAN_RESULT;


/** @brief This structure is the OS time structure type.
*/

struct os_time {
    os_time_t sec;
    os_time_t usec;
};


/**
* @}
*/


/**
* @brief This function gets the MAC address of a specific wireless port used by the Wi-Fi driver.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
*
* @param[out]  address is the MAC address (6bytes).
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note To get the MAC address, follow these steps:
*          1. Read NVDM from group "STA" and key "MacAddr" for STA MAC address.
*          2. If no information is set in NVDM, generate a partial random MAC as 00:11:22:33:xx:xx.
*          If the MAC address is retrieved successfully in Step 1, skip Step 2.
*
*/
int32_t wifi_config_get_mac_address(uint8_t port, uint8_t *address);


/**
* @brief This function disconnects the current connection. It takes effect immediately and the device
* switches to idle state. This API is available only in the station mode.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_disconnect_ap(void);


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
int32_t wifi_connection_get_link_status(uint8_t *link_status);



/**
* @brief This function initializes the scan table to the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list will be stored.
* @param[in] max_count is the maximum number of ap_list can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note #wifi_connection_scan_init() should be called before calling #wifi_connection_start_scan(), and it should be called only once to initialize one scan.\n
*       When the scan is done, the scanned AP list is already stored in parameter ap_list.
*/
int32_t wifi_connection_scan_init(WLAN_SCAN_RESULT *ap_list, uint32_t max_count);


/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note When the scan is finished, #wifi_connection_scan_deinit() should be called to unload the buffer from the driver. After that, the data in the parameter ap_list can be safely processed by user applications, and then another scan can be initialized by calling #wifi_connection_scan_init().
*/
int32_t wifi_connection_scan_deinit(void);


/**
* @brief This function starts Wi-Fi Scanning.
*
* @param[in] ssid is the specific SSID to scan. If the SSID is NULL, scan all APs based on channel tables.Only support NULL now.
* @param[in] ssid_length is the length of the specific SSID. Not support specific SSID length now.
* @param[in] bssid is specified to scan. If the BSSID is NULL, scan all APs based on channel tables.Only support NULL now.
* @param[in] scan_mode is the scan mode that can be either a full scan or a partial scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Full|
* \b 1                          | Partial, Not support now.|
*
* @param[in] scan_option selects scan options based on one of the following: active scan, passive scan or force active scan. Only support Active scan now.|
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Active scan, (Passive in regulatory channels). The channels that will be scanned are decided by the product's country and region. A probe request will be sent to each of these channels.|
* \b 1                          | Passive in all channels. Receives and processes the beacon. No probe request is sent.|
* \b 2                          | Force active (active in all channels). Forced to active scan in all channels. A probe request will be sent to each of these channels.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);


/**
* @brief This function gets the RSSI of the connected AP. It's only used for the STA mode and while the station is connected to the AP.
*
* @param[out]  rssi returns the RSSI of the connected AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_get_rssi(int32_t *rssi);


/**
* @brief This function registers the Wi-Fi event handler. Each event can register multiple event handlers with maximum number of 16.
* In the STA mode, the scan stops automatically when the scan cycle is finished
* and SCAN COMPLETE event is triggered;\n
* In the STA mode, once the device disconnects from the AP, DISCONNECT event with BSSID is triggered;\n
* In the STA mode, once the device connect to the AP, CONNECT event with BSSID is triggered.\n
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  It is not recommended to use the Wi-Fi APIs and any form of sleep in the event handler.
*/
int32_t wifi_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler);


/**
* @brief This function unregisters Wi-Fi event handler. The function #wifi_connection_register_event_handler() registers an event and matches it with the corresponding event handler. For the event behavior, please refer to #wifi_connection_register_event_handler().
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t for more details.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler);


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
int32_t wifi_config_set_sta_power_saving_mode(PARAM_POWER_MODE mode);


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
int32_t wifi_config_get_sta_power_saving_mode(PARAM_POWER_MODE *mode);

/**
* @brief This function starts wifi module. \n
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wifi_start(void);


/**
* @brief This function stops wifi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wifi_stop(void);


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
int32_t wifi_config_set_sta_connection(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption, const char *key, uint8_t wep_key_index, struct os_time *timeout);


#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif /* __WIFI_API_H__ */

