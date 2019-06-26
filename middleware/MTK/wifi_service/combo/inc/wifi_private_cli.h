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
    @file       wifi_private_cli.h
    @brief      WiFi - Configure API Examples, these CLI in the wifi_private_api.c
                is only for internal use, it will be removed or modified in future.

    @history    2016/07/27  Initial for 1st draft  (Pengfei Qiu)
 */

/**@addtogroup Wi-Fi_Private_CLI
* @{
* This section introduces the Wi-Fi private CLI APIs including terms and acronyms, supported features,
* details on how to use the WiFi-Private_CLI, function groups, enums, structures and functions. Before you config private cli,
* you should enter test mode frist(enter en) .
*
*/

/**@defgroup WIFI_COMMAND_Private_CLI_1 WiFi_Configuration_Private_CLI_Usage
* @{
*
* @section WiFiPrivateCLI_Terms_Chapter1_1 Wi-Fi Configuration Private CLI
*  <b>"WiFi Configuration Command Private" Cli </b>
*  |  Command Format                                   |   Usage Refer and Example                  |  Description |
*  |----------------------------------------------|------------------------------------     --    --|--------------------------------|
*  | <b>wifi config get rssi_threshold  </b>           |  @sa #wifi_config_get_rssi_threshold_ex    |  Get WiFi rssi threshold          |
*  | <b>wifi config set rssi_threshold <enabled> <rssi_value> </b>|   @sa #wifi_config_set_rssi_threshold_ex  |  Set WiFi rssi threshold           |
*  | <b>wifi config set frame_filter <enable> <frame_type> [ vender_ie.element_id ] [ vender_ie.OUI[0] ] [ vender_ie.OUI[1] ] [ vender_ie.OUI[2] ] </b>|   @sa #wifi_config_set_frame_filter_ex  |  Example of set frame filter for packets format wanted to be received, without rx_desc_flag set           |
*  | <b>wifi config set frame_filter_advanced <enable> <frame_type> [vender_ie.element_id] [ vender_ie.OUI[0] ] [ vender_ie.OUI[1] ] [ vender_ie.OUI[2] ] </b>|   @sa #wifi_config_set_frame_filter_advanced_ex  |  Example of set frame filter for packets format wanted to be received, with rx_desc_flag set           |
*  | <b>wifi config set n9dbg <dbg_level> </b>          | @sa #wifi_config_set_n9_dbg_level          |  Set N9 Debug Level                |
*  | <b>wifi config get n9dbg </b>                     |  @sa #wifi_config_get_n9_dbg_level          |  Get N9 Debug Level                |
*  | <b>wifi config get tx_power </b>                  | @sa #wifi_config_get_tx_power_ex            |  Example of get WiFi Tx power      |
*  | <b>wifi config set tx_power <power_value> </b>     |  @sa #wifi_config_set_tx_power_ex          |  Example of set WiFi Tx power      |
*  | <b>n9log set <off/n9/host> </b>                    | @sa #n9log_state_set_cli                   |  Example n9log state set           |
*  | <b>wifi config get wlanstat </b>                  |  @sa #wifi_config_get_statistic_ex          |  Example of Get Wlan statistic     |
*  | <b>wifi config set ip_filter <ip_address> </b>         | @sa #wifi_config_set_ip_filter_ex      |  Example of Set IP filter         |
*  | <b>wifi config set ip_mac_port_filter <onoff> </b>    | @sa #wifi_config_set_ip_mac_port_filter_ex    | Example of Set WiFi ip_mac_port_filter ON/OFF        |
*  | <b>wifi config get ip_mac_port_filter </b>        | @sa #wifi_config_get_ip_mac_port_filter_ex    | Example of get WiFi ip_mac_port_filter ON/OFF     |
*  | <b>wifi config set ip_mac_port <action> <vaild_bitmap> [src_ip] [src_mac] [src_port] [dst_ip] [dst_mac] [dst_port] </b>         | @sa #wifi_config_set_ip_mac_port_entry_ex   | Example of Set WiFi ip_mac_port_entry.       |
*  | <b>wifi config get ip_mac_port </b>               | @sa #wifi_config_get_ip_mac_port_entry_ex   | Example of Get WiFi ip_mac_port_entry List        |
*  | <b>wifi config set txraw_adv <tx_mode> <mcs> <rate_fixed> <ack_required> <retry_count> <raw_packets> </b> | @sa #wifi_tx_raw_pkt_advanced_ex      |  Example of sends a raw Wi-Fi packet over the air |
*  | <b>wifi config set ps_mode <ps_mode> </b>          | @sa #wifi_config_set_ps_mode_ex            | Example of set power save mode     |
*  | <b>wifi config get ps_mode </b>                    | @sa #wifi_config_get_ps_mode_ex            | Example of get power save mode     |
*  | <b>wifi config get calc_pmk <ssid> <passphrase> </b> |@sa #wifi_config_calculate_pmk_ex            |  Example of calculate pmk value         |
*  | <b>wifi config get conn_status </b>               | @sa #wifi_config_get_connection_status_ex   |  Example of Get Connection Status  |
*  | <b>wifi config get scan_list </b>                 | @sa #wifi_config_get_scan_list_ex           |  Example of Get Scan List          |
*  | <b>wifi config get ant_status </b>                | @sa #wifi_config_get_antenna_status_ex      |  Example of get WiFi RF antenna status            |
*  | <b>wifi config set ant_monitor </b>               | @sa #wifi_config_set_antenna_monitor_ex     |  Example of trigger WiFi to do RF antenna switch algorithm   |
*  | <b>wifi config set ant_number <number> </b>         | @sa #wifi_config_set_antenna_number_ex      | Example of set WiFi RF antenna number             |
*
*/

/**
* @}
*/


#ifndef __WIFI_PRIVATE_CLI_H__
#define __WIFI_PRIVATE_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "cli.h"


/**
* @brief Get WiFi rssi threshold
* @parameter
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config get rssi_threshold 
* \n <b>cli_output</b>:   Get rssi threshold
*
* \n <b>cli_example</b>: wifi config get rssi_threshold
* \n <b>cli_note</b>:     Get rssi threshold, eg:rssi threthold: [enabled: 0], [value: 0] 
*
*/
uint8_t wifi_config_get_rssi_threshold_ex(uint8_t len, char *param[]);

/**
* @brief Set wifi rssi enable and threshold, which is used to filter packets
* @param [IN]enable
*    0 enable rssi threshold.
*    1 disable rssi threshold.
* @param [IN]rssi value only useful when enable=1
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set rssi_threshold <enabled> <rssi_value>
* \n <b>cli_param</b>:    enabled: 0 disable rssi threshold; 1 enable rssi threshold
* \n                      rssi_value: It has a value of 0 to RSSI Max, only useful when enable=1.
*
* \n <b>cli_example</b>: wifi config set rssi_threshold 1 5
* \n <b>cli_note</b>:     Set the rssi threshold ernable and use filter packets have rssi value bigger than 5. 
*
*/
uint8_t wifi_config_set_rssi_threshold_ex(uint8_t len, char *param[]);

int32_t wifi_frame_type_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

/**
* @brief Example of set frame filter for packets format wanted to be received, without rx_desc_flag set
* @parameter
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set frame_filter <enable> <frame_type> [ vender_ie.element_id ] [ vender_ie.OUI[0] ] [ vender_ie.OUI[1] ] [ vender_ie.OUI[2] ]
* \n <b>cli_param</b>:    enable: 
* \n                      frame_type: 
* \n                      vender_ie.element_id:
* \n                      vender_ie.OUI[0]:
* \n                      vender_ie.OUI[1]:
* \n                      vender_ie.OUI[2]:
*
* \n <b>cli_example 1</b>: wifi config set frame_filter 1 8 221 0 15 172
* \n <b>cli_note</b>:     Set the frame_filter ernable,to revice frame with frame_type is 8, vender_ie info matched
*
* \n <b>cli_example 2</b>: wifi config set frame_filter 1 8
* \n <b>cli_note</b>:     Set the frame_filter ernable,to revice frame with frame_type is 8.
*
*/
uint8_t wifi_config_set_frame_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of set frame filter for packets format wanted to be received, with rx_desc_flag set
* @parameter
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set frame_filter_advanced <enable> <frame_type> [vender_ie.element_id] [ vender_ie.OUI[0] ] [ vender_ie.OUI[1] ] [ vender_ie.OUI[2] ]
* \n <b>cli_param</b>:    enable: 
* \n                      frame_type: 
* \n                      vender_ie.element_id:
* \n                      vender_ie.OUI[0]:
* \n                      vender_ie.OUI[1]:
* \n                      vender_ie.OUI[2]:
*
* \n <b>cli_example 1</b>: wifi config set frame_filter_advanced 1 8 221 0 15 172
* \n <b>cli_note</b>:     Set the frame_filter ernable,to revice frame with frame_type is 8, vender_ie info matched
*
* \n <b>cli_example 2</b>: wifi config set frame_filter_advanced 1 8
* \n <b>cli_note</b>:     Set the frame_filter ernable,to revice frame with frame_type is 8.
*
*/
uint8_t wifi_config_set_frame_filter_advanced_ex(uint8_t len, char *param[]);

/**
* @brief Set N9 Debug Level
* @param [IN]dbg_level 
*
* Value                         |Definition                                                 |
* -----------|---------------------------------------------------------------------|
* 0           | None     |
* 1           | ERROR    |
* 2           | WARNING|
* 3           | TRACE    |
* 4           | INFO     |
* 5           | LAUD     |
*
* @return  =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set n9dbg <dbg_level>
* \n <b>cli_param</b>:    dbg_level: see param
*
* \n <b>cli_example</b>: wifi config set n9dbg 1
* \n <b>cli_note</b>:     Set N9 debug level is ERROR, it will be print error log. 
*
*/
uint8_t wifi_config_set_n9_dbg_level(uint8_t len, char *param[]);

/**
* @brief Get N9 Debug Level
* @param [OUT]dbg_level 
*
* Value                         |Definition                                                 |
* -----------|---------------------------------------------------------------------|
* 0           | None     |
* 1           | ERROR    |
* 2           | WARNING|
* 3           | TRACE    |
* 4           | INFO     |
* 5           | LAUD     |
*
* @return  =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config get n9dbg
* \n <b>cli_output</b>:   Get dbg_level value, see param
*
* \n <b>cli_example</b>: wifi config get n9dbg
* \n <b>cli_note</b>:     Get N9 debug level ,eg is ERROR.
*
*/
uint8_t wifi_config_get_n9_dbg_level(uint8_t len, char *param[]);

/**
* @brief Example of Start/Stop WiFi Scanning for internal use
* wifi connect set scan_adv <start/stop> <scan_mode> <scan_option> [chanel_string_2g] [chanel_string_5g] [ssid] [bssid]
* @param [IN] start/stop  0: stop, 1: start
* @param [IN] scan_mode  0: full, 1: partial
* @param [IN] scan_option  0: active, 1: passive in all channel, 2:force active
* @param [IN] chanel_string_2g   2g channel list
* @param [IN] chanel_string_5g   5g channel list
* @param [IN] ssid   "NULL" means not specified
* @param [IN] bssid   "NULL" means not specified
*
* @return  =0 means success, >0 means fail
*
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Tab always keeps the APs of this specified SSID/BSSID
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi connect set scan_adv <start/stop> <scan_mode> <scan_option> [chanel_string_2g] [chanel_string_5g] [ssid] [bssid]
* \n <b>cli_param</b>:    start/stop: see param
* \n                      scan_mode: see param
* \n                      scan_option: see param
* \n                      chanel_string_2g: see param
* \n                      chanel_string_5g: see param
* \n                      ssid: see param
* \n                      bssid: see param
*
*/
uint8_t wifi_connect_scan_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi tx power
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get tx_power
* \n <b>cli_output</b>:    Output the user level WiFi tx power, this is basic power for all modulation in this channel.
* \n                       output tx_power is 64~190 instead the real tx_power is -31.5dbm ~ +31.5dbm, the real tx_power = (output - 127) / 2
*
* \n <b>cli_example</b>:   wifi config get tx_power
* \n <b>cli_note</b>:      return the current user level tx_power.
*
*/
uint8_t wifi_config_get_tx_power_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Tx Power
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set tx_power <power>
* \n <b>cli_param</b>:    power: 64~190, instead of real tx_power is -31.5dbm to +31.5dbm
* \n                      power = real tx_power * 2 + 127
*
* \n <b>cli_example</b>: wifi config set tx_power 167
* \n <b>cli_note</b>:    Set the real tx_power to 20dbm.
*
*/
uint8_t wifi_config_set_tx_power_ex(uint8_t len, char *param[]);

/**
* @brief Example n9log state set
* @param [IN] level
*
* Value        |Definition                                                              |
* ------------|----------------------------------------------------------------|
* off          | Disable n9 logs            |
* n9           | Output n9 logs to uart     |
* host         | Output n9 logs to host.    |
*
* @return  =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   n9log set <off/n9/host>
* \n <b>cli_param</b>:   Supported param include: off, n9 , host. Detail see param
*
* \n <b>cli_example</b>: n9log set n9
* \n <b>cli_note</b>:    output n9 logs to uart.
*
*/
uint8_t n9log_state_set_cli(uint8_t len, char *param[]);

/**
* @brief Example of Get Wlan statistic. Only for STA mode.
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get wlanstat
* \n <b>cli_output</b>:    Wlan statistic information
* 
* \n <b>cli_example</b>:   wifi connect get wlanstat
* \n <b>cli_note</b>:      Return the Wlan statistic. 
*
*/
uint8_t wifi_config_get_statistic_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set IP filter
* @param [IN]ip_address
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set ip_filter <ip_address>
* \n <b>cli_param</b>:    ip_address: 
* 
* \n <b>cli_example</b>: wifi config set ip_filter 192.168.1.101
* \n <b>cli_note</b>:     Receive packets from special ip address
*
*/
uint8_t wifi_config_set_ip_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi ip_mac_port_filter ON/OFF
* @param [IN]onoff  0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set ip_mac_port_filter <onoff>
* \n <b>cli_param</b>:    onoff: see param
* 
* \n <b>cli_example</b>: wifi config set ip_mac_port_filter on
* \n <b>cli_note</b>:     Set ip_mac_port_filter enable
*
*/
uint8_t wifi_config_set_ip_mac_port_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of get WiFi ip_mac_port_filter ON/OFF
* @param [OUT]onoff 0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config get ip_mac_port_filter
* \n <b>cli_output</b>:   Get the ip_mac_port_filter status: see param
* 
* \n <b>cli_example</b>: wifi config get ip_mac_port_filter
* \n <b>cli_note</b>:     Get the ip_mac_port_filter status
*
*/
uint8_t wifi_config_get_ip_mac_port_filter_ex(uint8_t len, char *param[]);

/** @brief This enumeration defines the ip_match_entry_filter bitmap. Each bit indicates a specific param is enable.
*/
typedef enum {
    WIFI_IP_MATCH_FILTER_SRC_IPADDR=0,     /**< bit 0   SRC_IPADDR. */
    WIFI_IP_MATCH_FILTER_SRC_MACADDR,      /**< bit 1   SRC_MACADDR. */
    WIFI_IP_MATCH_FILTER_SRC_PORT,         /**< bit 2   SRC_PORT. */
    WIFI_IP_MATCH_FILTER_DST_IPADDR,       /**< bit 3   DST_IPADDR. */
    WIFI_IP_MATCH_FILTER_DST_MACADDR,      /**< bit 4   DST_MACADDR. */
    WIFI_IP_MATCH_FILTER_DST_PORT,         /**< bit 5   DST_PORT. */
    WIFI_IP_MATCH_FILTER_MAX_NUM,          /* MAX_NUM. */
} wifi_ip_match_entry_filter_t;

/**
* @brief Example of Set WiFi ip_mac_port_entry. When you want to use set specific parameter, you should set the
*  valid bitmap fristly. It can be use fully and partial.
* @param [IN]action   0 delete, 1 create,
* @param [IN]vaild bitmap   For more details, please refer to #wifi_ip_match_entry_filter_t.
* @param [IN]src_ip
* @param [IN]src_mac
* @param [IN]src_port  0~65535,usually is between 1024 and 5000
* @param [IN]dst_ip
* @param [IN]dst_mac
* @param [IN]dst_port  0~65535,usually is between 1024 and 5000
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set ip_mac_port <action> <vaild_bitmap> [src_ip] [src_mac] [src_port] [dst_ip] [dst_mac] [dst_port]
* \n <b>cli_param</b>:   action:
* \n                     vaild_bitmap: see param
* \n                     src_ip:
* \n                     src_mac:
* \n                     src_port:
* \n                     dst_ip:
* \n                     dst_mac:
* \n                     dst_port:
* 
* \n <b>cli_example 1</b>: wifi config set ip_mac_port 1 0x1 192.168.1.1
* \n <b>cli_note</b>:     Set the ip_mac_port filter add src_ip=192.168.1.1
*
* \n <b>cli_example 2</b>: wifi config set ip_mac_port 1 0x9 192.168.1.1 192.168.1.101
* \n <b>cli_note</b>:     Set the ip_mac_port filter add src_ip=192.168.1.1, dst_ip=192.168.1.100
*
*/
uint8_t wifi_config_set_ip_mac_port_entry_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi ip_mac_port_entry List
* @param [OUT]entry_list
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config get ip_mac_port
* \n <b>cli_output</b>:   Get WiFi ip_mac_port_entry List, format see #wifi_config_set_ip_mac_port_entry_ex 
* 
* \n <b>cli_example</b>: wifi config get ip_mac_port
* \n <b>cli_note</b>:     Get the ip_mac_port filter
*
*/
uint8_t wifi_config_get_ip_mac_port_entry_ex(uint8_t len, char *param[]);

/**
* @brief Example of sends a raw Wi-Fi packet over the air
* @param [IN]tx_mode 0: MODE_CCK, 1: MODE_OFDM, 2: MODE_HTMIX, 3: MODE_HTGREENFIELD, 4: MODE_VHT  
* @param [IN]mcs
* @param [IN]rate_fixed 0/1: Disable/Enable Fixed rate
* @param [IN]ack_required 0/1: Disable/Enable ACK
* @param [IN]retry_count 0: No Retry, 1-30: Retry count, 31: Unlimited Retry count
* @param [IN]raw_packets raw packets
* @return  >=0 means success, <0 means fail
* @note entry please refer to #txd_param_setting_t
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set txraw_adv <tx_mode> <mcs> <rate_fixed> <ack_required> <retry_count> <raw_packets>
* \n <b>cli_param</b>:    tx_mode: see param
* \n                      mcs:  see param
* \n                      rate_fixed: see param
* \n                      ack_required: see param
* \n                      retry_count: see param
* \n                      raw_packets: see param
*
* \n <b>cli_example</b>: wifi config set txraw_adv 0 0 1 1 5 48113C00940C6D210F22000C22768766940C6D210F227001B3015194
* \n <b>cli_note</b>:     Use MODE_CCK, Fixed rate, need ack, retry 5 times, to send raw packets
*
*/
uint8_t wifi_tx_raw_pkt_advanced_ex(uint8_t len, char *param[]);

/**
* @brief Example of set power save mode
* @param [IN]ps_mode 0(CAM ,Constantly Awake Mode) ), 1(LEGACY_POWERSAVE), 2(FAST_POWERSAVE)
*
* Value                       |Definition                                                 |
* -----|--------------------------------------------------------------------------|
*  0   | CAM: CAM (Constantly Awake Mode) is a power save mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power save setting consumes the most power but offers the highest throughput.|
*  1   | LEGACY_POWERSAVE: the access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it requests each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operating devices).|
*  2   | FAST_POWERSAVE: Fast is a power save mode that switches between power saving and CAM modes, depending on the network traffic. For example, it switches to CAM when receiving a large number of packets and switches back to PS mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.|
*
* @return  >=0 means success, <0 means fail
* @note This only support in mt7682/mt7686 currently.
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set ps_mode <ps_mode>
* \n <b>cli_param</b>:    ps_mode: see param
* 
* \n <b>cli_example</b>: wifi config set ps_mode 2
* \n <b>cli_note</b>:     Set power save mode 2(FAST_POWERSAVE) 
*
*/
uint8_t wifi_config_set_ps_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of get power save mode
* @param [IN]ps_mode 0(CAM ,Constantly Awake Mode) ), 1(LEGACY_POWERSAVE), 2(FAST_POWERSAVE), 
*
* Value                       |Definition                                                 |
* -----|--------------------------------------------------------------------------|
*  0   | CAM: CAM (Constantly Awake Mode) is a power save mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power save setting consumes the most power but offers the highest throughput.|
*  1   | LEGACY_POWERSAVE: the access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it requests each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operating devices).|
*  2   | FAST_POWERSAVE: Fast is a power save mode that switches between power saving and CAM modes, depending on the network traffic. For example, it switches to CAM when receiving a large number of packets and switches back to PS mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.|
*
* @return  >=0 means success, <0 means fail
* @note This only support in mt7682/mt7686 currently.
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config get ps_mode
* \n <b>cli_output</b>:    ps_mode: see param
* 
* \n <b>cli_example</b>: wifi config get ps_mode
* \n <b>cli_note</b>:     Get power save mode ,eg 2(FAST_POWERSAVE) 
*
*/
uint8_t wifi_config_get_ps_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of calculate pmk value
* @param [IN]ssid
* @param [IN]passphrase
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get calc_pmk <ssid> <passphrase>
* \n <b>cli_param</b>:     ssid: ssid string
* \n                       passphrase: passphrase string
* \n <b>cli_output</b>:    Output the calculated pmd, get pmk @sa #wifi_config_get_pmk
* 
* \n <b>cli_example</b>:   wifi config get calc_pmk MTK_SOFT_AP 12345678
* \n <b>cli_note</b>:      return the pmk value, 
* \n                        82 dd 92 2f b0 7f d4 97 f9 d1 79 48 17 b4 64 aa 
* \n                        d0 b3 ed 5c d8 de 73 b2 f4 d0 80 d5 97 fc cf 23
*
*/
uint8_t wifi_config_calculate_pmk_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get Connection Status
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get conn_status
* \n <b>cli_output</b>:    Output the connection status
* 
* \n <b>cli_example</b>:   wifi config get conn_status
* \n <b>cli_note</b>:      return the connection status
*
*/
uint8_t wifi_config_get_connection_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get Scan List
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get scan_list
* \n <b>cli_output</b>:    Output the scan list
* 
* \n <b>cli_example</b>:   wifi config get scan_list
* \n <b>cli_note</b>:      return the scan list
*
*/
uint8_t wifi_config_get_scan_list_ex(uint8_t len, char *param[]);

#ifdef MTK_ANT_DIV_ENABLE
/**
* @brief Example of get RF Diversity antenna status
*        This is only take effect when Antenna RF diversity feature is enabled and HW design have two WiFi antenna.
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get ant_status
* \n <b>cli_output</b>:    Output the current antenna status, include antenna_number:antenna 0/1, ant0_rssi:current rssi of antenna 0, ant1_rssi:current rssi of antenna 1, pre_selected_rssi:record last time wifi connected rssi.
*
* \n <b>cli_example</b>:   wifi config get ant_status
* \n <b>cli_note</b>:      return the current RF antenna status.
*
*/
uint8_t wifi_config_get_antenna_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of set RF Diversity antenna monitor, will trigger driver perform antenna switch algorithm,if another antenna signal is better, antenna will be switched.
*        This is only take effect when Antenna RF diversity feature is enabled and HW design have two WiFi antenna.
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set ant_monitor
* \n <b>cli_param</b>:   NA
*
* \n <b>cli_example</b>: wifi config set ant_monitor
* \n <b>cli_note</b>:    trigger antenna switch algorithm.
*
*/
uint8_t wifi_config_set_antenna_monitor_ex(uint8_t len, char *param[]);

/**
* @brief Example of set RF antenna number, will force RF switch to the special antenna.
*        This is only take effect when Antenna RF diversity feature is enabled and HW design have two WiFi antenna.
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set ant_number <ant_number>
* \n <b>cli_param</b>:   ant_number: 0 is antenna 0, 1 is antenna 1.
*
* \n <b>cli_example</b>: wifi config set ant_number 1
* \n <b>cli_note</b>:    set antenna number to 1
*
*/
uint8_t wifi_config_set_antenna_number_ex(uint8_t len, char *param[]);
#endif

/**
* @brief Example of set dwell time.
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set dwell_time <scan_option> <dwell_time>
* \n <b>cli_param</b>:   scan_option:  0:active;  1:passive
*
* \n <b>cli_example</b>: wifi config set dwell_time 0 xx
* \n <b>cli_note</b>: Set dewell time for scan
*
*/
uint8_t wifi_config_set_dwell_time_ex(uint8_t len, char *param[]);

/**
* @brief Example of get dwell time.
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:   wifi config set dwell_time <scan_option> <dwell_time>
* \n <b>cli_param</b>:   scan_option:  0:active;  1:passive
*
* \n <b>cli_example</b>: wifi config get dwell_time <scan_option>
* \n <b>cli_note</b>: Get dewell time of special scan mode from FW
*
*/
uint8_t wifi_config_get_dwell_time_ex(uint8_t len, char *param[]);

#if defined(MTK_MINICLI_ENABLE)
#define N9_LOG_CLI_ENTRY { "n9log",    "N9 consol log state", n9log_state_set_cli},
#else
#define N9_LOG_CLI_ENTRY
#endif

#if defined(MTK_MINICLI_ENABLE)
extern cmd_t   wifi_private_cli[];
#endif

#if defined(MTK_MINICLI_ENABLE)
#define WIFI_PRIV_CLI_ENTRY     { "wifi", "wifi private cli",   NULL, wifi_private_cli },
#else
#define WIFI_PRIV_CLI_ENTRY
#endif


#ifdef __cplusplus
    }
#endif
#endif /*  __WIFI_PRIVATE_CLI_H__ */

/**
*@}
*/


