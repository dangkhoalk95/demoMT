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
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sys_init.h"

#include "wifi_api.h"
//#include "wifi_os_api.h"
#include "wfc_stub.h"
#include "wfc_sdio.h"
 
#include "lwip/pbuf.h"
#include "wfc_at_api.h"
#include "wifi_os_api.h"
#include "os_util.h"
#include "os.h"
#include "mlme_interface.h"
#include "wifi_init.h"

/*the semaphore to get wifi host setting,  then start wifi_init in device*/
extern os_semaphore_t g_sem_wfc_host_cfg;
wfc_system_config_t g_wfc_system_config = {{0}};

wifi_scan_list_item_t *scan_ap_list = NULL;
int scan_ap_size = 0;
extern uint8_t g_scan_inited;

#ifdef MTK_WIFI_STUB_CONF_ENABLE
uint8_t wfc_send_to_host(uint8_t *pbuf, uint32_t bufSize, uint8_t isEvt);
#endif

static int scan_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    int i = 0;
#ifdef MTK_WIFI_STUB_CONF_ENABLE
    uint32_t size = 0;
    wh_scan_reslt_evt_para_t *evt_para = NULL;
#endif
    switch (event_id) {

        case WIFI_EVENT_IOT_SCAN_COMPLETE:
            handled = 1;
            printf("[MTK Event Callback Sample]: Scan Done!\n");
#ifdef MTK_WIFI_STUB_CONF_ENABLE
            if(scan_ap_size <= SCAN_AP_LIST_MAX_COUNT) {
                size = sizeof(wh_scan_reslt_evt_para_t) + sizeof(wifi_scan_list_item_t)*scan_ap_size;
                evt_para = (wh_scan_reslt_evt_para_t *)os_malloc(size);
                evt_para->evt_id = WFC_EVT_SCAN_RESULTS;
                evt_para->scan_table.list_size = scan_ap_size;
                evt_para->scan_table.index = 0;
                os_memcpy(evt_para->scan_table.scan_list, scan_ap_list, sizeof(wifi_scan_list_item_t)*scan_ap_size);
                for (i = 0; i < scan_ap_size; i++) {
                    printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR");
                    printf("%-4d", evt_para->scan_table.scan_list[i].channel);
                    printf("%-33s", evt_para->scan_table.scan_list[i].ssid);
                    printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                           evt_para->scan_table.scan_list[i].bssid[0],
                           evt_para->scan_table.scan_list[i].bssid[1],
                           evt_para->scan_table.scan_list[i].bssid[2],
                           evt_para->scan_table.scan_list[i].bssid[3],
                           evt_para->scan_table.scan_list[i].bssid[4],
                           evt_para->scan_table.scan_list[i].bssid[5]);
                    printf("%-8d", evt_para->scan_table.scan_list[i].auth_mode);
                    printf("%-8d", evt_para->scan_table.scan_list[i].encrypt_type);
                    printf("%-8d", evt_para->scan_table.scan_list[i].rssi);
                    printf("%-8d", evt_para->scan_table.scan_list[i].is_wps_supported);
                    printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.configuration_methods);
                    printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.device_password_id);
                    printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.selected_registrar);
                    printf("\n");
                }
                wfc_send_to_host((uint8_t *)evt_para, size, 1);
                os_free(evt_para);
                os_free(scan_ap_list);
            }else {
                int reamin_ap_size = scan_ap_size;
                uint8_t index = 0;
                while(reamin_ap_size > 0) {
                    printf("scan index:%d, remain size: %d.\r\n",index, reamin_ap_size);
                    if(reamin_ap_size > SCAN_AP_LIST_MAX_COUNT) {
                        size = sizeof(wh_scan_reslt_evt_para_t) + sizeof(wifi_scan_list_item_t)*SCAN_AP_LIST_MAX_COUNT;
                        evt_para = (wh_scan_reslt_evt_para_t *)os_malloc(size);
                        evt_para->evt_id = WFC_EVT_SCAN_RESULTS;
                        evt_para->scan_table.list_size = SCAN_AP_LIST_MAX_COUNT;
                        evt_para->scan_table.index = index;
                        os_memcpy(evt_para->scan_table.scan_list, scan_ap_list + index, sizeof(wifi_scan_list_item_t)*SCAN_AP_LIST_MAX_COUNT);
                        for (i = 0; i < SCAN_AP_LIST_MAX_COUNT; i++) {
                            printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR");
                            printf("%-4d", evt_para->scan_table.scan_list[i].channel);
                            printf("%-33s", evt_para->scan_table.scan_list[i].ssid);
                            printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                                   evt_para->scan_table.scan_list[i].bssid[0],
                                   evt_para->scan_table.scan_list[i].bssid[1],
                                   evt_para->scan_table.scan_list[i].bssid[2],
                                   evt_para->scan_table.scan_list[i].bssid[3],
                                   evt_para->scan_table.scan_list[i].bssid[4],
                                   evt_para->scan_table.scan_list[i].bssid[5]);
                            printf("%-8d", evt_para->scan_table.scan_list[i].auth_mode);
                            printf("%-8d", evt_para->scan_table.scan_list[i].encrypt_type);
                            printf("%-8d", evt_para->scan_table.scan_list[i].rssi);
                            printf("%-8d", evt_para->scan_table.scan_list[i].is_wps_supported);
                            printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.configuration_methods);
                            printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.device_password_id);
                            printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.selected_registrar);
                            printf("\n");
                        }
                        wfc_send_to_host((uint8_t *)evt_para, size, 1);
                        index += SCAN_AP_LIST_MAX_COUNT;
                        reamin_ap_size -= SCAN_AP_LIST_MAX_COUNT;
                        os_free(evt_para);
                        
                    }else {
                        size = sizeof(wh_scan_reslt_evt_para_t) + sizeof(wifi_scan_list_item_t)*reamin_ap_size;
                        evt_para = (wh_scan_reslt_evt_para_t *)os_malloc(size);
                        evt_para->evt_id = WFC_EVT_SCAN_RESULTS;
                        evt_para->scan_table.list_size = reamin_ap_size;
                        evt_para->scan_table.index = index;
                        os_memcpy(evt_para->scan_table.scan_list, scan_ap_list + index, sizeof(wifi_scan_list_item_t)*reamin_ap_size);
                        for (i = 0; i < reamin_ap_size; i++) {
                            printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR");
                            printf("%-4d", evt_para->scan_table.scan_list[i].channel);
                            printf("%-33s", evt_para->scan_table.scan_list[i].ssid);
                            printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                                   evt_para->scan_table.scan_list[i].bssid[0],
                                   evt_para->scan_table.scan_list[i].bssid[1],
                                   evt_para->scan_table.scan_list[i].bssid[2],
                                   evt_para->scan_table.scan_list[i].bssid[3],
                                   evt_para->scan_table.scan_list[i].bssid[4],
                                   evt_para->scan_table.scan_list[i].bssid[5]);
                            printf("%-8d", evt_para->scan_table.scan_list[i].auth_mode);
                            printf("%-8d", evt_para->scan_table.scan_list[i].encrypt_type);
                            printf("%-8d", evt_para->scan_table.scan_list[i].rssi);
                            printf("%-8d", evt_para->scan_table.scan_list[i].is_wps_supported);
                            printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.configuration_methods);
                            printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.device_password_id);
                            printf("%-8d", evt_para->scan_table.scan_list[i].wps_element.selected_registrar);
                            printf("\n");
                        }
                        wfc_send_to_host((uint8_t *)evt_para, size, 1);
                        reamin_ap_size = 0;
                        os_free(evt_para);
                    }
                }
                os_free(scan_ap_list);
                printf("free scan table.\r\n");
            }
#endif
            scan_ap_size = 0;
            break;

        default:
            handled = 0;
            printf("[MTK Event Callback Sample]: Unknown event(%d)\n", event_id);
            break;
    }
    return handled;

}



int32_t wfc_api_process(uint8_t *pcmd,  uint32_t cmdSize, uint8_t *txptr, uint32_t *txsize)
{
    int32_t ret = 0;
    wh_api_id_t *api_id = (wh_api_id_t *)pcmd;
    
    LOG_I(wfc, "<WFC>wfc_api_process(%u)(set=%d)",api_id->api,api_id->set);
    
    switch(api_id->api)
    {                 
        case WFC_API_OPMODE:
        {
            if(api_id->set == true) {
                wh_set_opmode_cmd_para_t  *set_opmode_cmd = (wh_set_opmode_cmd_para_t *)pcmd;
                wh_set_opmode_resp_para_t set_opmode_resp;
                set_opmode_resp.error_code = wifi_config_set_opmode(set_opmode_cmd->opmode);
                ret = set_opmode_resp.error_code;
                os_memcpy(txptr, &set_opmode_resp, sizeof(set_opmode_resp));
                *txsize = sizeof(set_opmode_resp);
            } else {
                wh_get_opmode_resp_para_t get_opmode_resp;
                get_opmode_resp.error_code = wifi_config_get_opmode(&get_opmode_resp.opmode);
                ret = get_opmode_resp.error_code;
                os_memcpy(txptr, &get_opmode_resp, sizeof(get_opmode_resp));
                *txsize = sizeof(get_opmode_resp);
            }
        
            break;
        }
        case WFC_API_SSID:  
        {
            if(api_id->set == true) {
                wh_set_ssid_cmd_para_t  *set_ssid_cmd = (wh_set_ssid_cmd_para_t *)pcmd;
                wh_set_ssid_resp_para_t set_ssid_resp;
                set_ssid_resp.error_code = wifi_config_set_ssid(set_ssid_cmd->port, set_ssid_cmd->ssid, set_ssid_cmd->ssid_len);
                ret = set_ssid_resp.error_code;
                os_memcpy(txptr, &set_ssid_resp, sizeof(set_ssid_resp));
                *txsize = sizeof(set_ssid_resp);
            } else {
                wh_get_ssid_cmd_para_t  *get_ssid_cmd = (wh_get_ssid_cmd_para_t *)pcmd;
                wh_get_ssid_resp_para_t get_ssid_resp;
                get_ssid_resp.error_code = wifi_config_get_ssid(get_ssid_cmd->port, get_ssid_resp.ssid, &get_ssid_resp.ssid_len);
                ret = get_ssid_resp.error_code;
                os_memcpy(txptr, &get_ssid_resp, sizeof(get_ssid_resp));
                *txsize = sizeof(get_ssid_resp);
            }
            break;
        }
        case WFC_API_SECURITY:  
        {
            if(api_id->set == true) {
                wh_set_security_cmd_para_t  *set_security_cmd = (wh_set_security_cmd_para_t *)pcmd;
                wh_set_security_resp_para_t set_security_resp;
                set_security_resp.error_code = wifi_config_set_security_mode(set_security_cmd->port, set_security_cmd->auth_mode, set_security_cmd->encrypt_type);
                ret = set_security_resp.error_code;
                os_memcpy(txptr, &set_security_resp, sizeof(set_security_resp));
                *txsize = sizeof(set_security_resp);
            } else {
                wh_get_security_cmd_para_t  *get_security_cmd = (wh_get_security_cmd_para_t *)pcmd;
                wh_get_security_resp_para_t get_security_resp;
                get_security_resp.error_code = wifi_config_get_security_mode(get_security_cmd->port, &get_security_resp.auth_mode, &get_security_resp.encrypt_type);
                ret = get_security_resp.error_code;
                os_memcpy(txptr, &get_security_resp, sizeof(get_security_resp));
                *txsize = sizeof(get_security_resp);
            }
            break;
        }
        case WFC_API_PASSWD:
        {
            if(api_id->set == true) {
                wh_set_password_cmd_para_t  *set_password_cmd = (wh_set_password_cmd_para_t *)pcmd;
                wh_set_password_resp_para_t set_password_resp;
                if(set_password_cmd->wep_key_index >= 0 && set_password_cmd->wep_key_index < WIFI_NUMBER_WEP_KEYS) {
                    wifi_wep_key_t wep_keys= {{{0}}};
                    if(set_password_cmd->password_len > WIFI_MAX_WEP_KEY_LENGTH) {
                        return -1;
                    }
                    wep_keys.wep_tx_key_index = set_password_cmd->wep_key_index;
                    wep_keys.wep_key_length[set_password_cmd->wep_key_index] = set_password_cmd->password_len;
                    os_memcpy(wep_keys.wep_key[set_password_cmd->wep_key_index], set_password_cmd->password, set_password_cmd->password_len);
                    set_password_resp.error_code = wifi_config_set_wep_key(set_password_cmd->port, &wep_keys);    
                } else {
                    set_password_resp.error_code = wifi_config_set_wpa_psk_key(set_password_cmd->port, set_password_cmd->password, set_password_cmd->password_len);
                }
                ret = set_password_resp.error_code;
                os_memcpy(txptr, &set_password_resp, sizeof(set_password_resp));
                *txsize = sizeof(set_password_resp);
            } else {
                wh_get_password_cmd_para_t  *get_password_cmd = (wh_get_password_cmd_para_t *)pcmd;
                wh_get_password_resp_para_t get_password_resp;
                if(get_password_cmd->wep_key_index >= 0 && get_password_cmd->wep_key_index < WIFI_NUMBER_WEP_KEYS) {
                    wifi_wep_key_t wep_keys;
                    get_password_resp.error_code = wifi_config_get_wep_key(get_password_cmd->port, &wep_keys);
                    get_password_resp.password_len = wep_keys.wep_key_length[get_password_cmd->wep_key_index];
                    if(get_password_resp.password_len > WIFI_MAX_WEP_KEY_LENGTH) {
                        return -1;
                    }
                    os_memcpy(get_password_resp.password, wep_keys.wep_key[get_password_cmd->wep_key_index], wep_keys.wep_key_length[get_password_cmd->wep_key_index]);
                } else {
                    get_password_resp.error_code = wifi_config_get_wpa_psk_key(get_password_cmd->port, get_password_resp.password, &get_password_resp.password_len);
                }
                ret = get_password_resp.error_code;
                os_memcpy(txptr, &get_password_resp, sizeof(get_password_resp));
                *txsize = sizeof(get_password_resp);
            }
            break;
        }
        case WFC_API_RELOAD:
        {
            if(api_id->set == true) {
                wh_set_reload_resp_para_t set_reload_resp;
                set_reload_resp.error_code = wifi_config_reload_setting();
                ret = set_reload_resp.error_code;
                os_memcpy(txptr, &set_reload_resp, sizeof(set_reload_resp));
                *txsize = sizeof(set_reload_resp);
            }
            break;
        }
        case WFC_API_CONNECTION:    
        {
            if(api_id->set == true) {
                wh_set_connection_cmd_para_t  *set_connection_cmd = (wh_set_connection_cmd_para_t *)pcmd;
                wh_set_connection_resp_para_t set_connection_resp;
                if(set_connection_cmd->disconnect == true) {
                    if(set_connection_cmd->port == WIFI_PORT_STA) {
                        set_connection_resp.error_code = wifi_connection_disconnect_ap();
                    } else {
                        set_connection_resp.error_code = wifi_connection_disconnect_sta(set_connection_cmd->client_mac);

                    }
                } else {
                    set_connection_resp.error_code = wifi_config_reload_setting();
                }
                ret = set_connection_resp.error_code;
                os_memcpy(txptr, &set_connection_resp, sizeof(set_connection_resp));
                *txsize = sizeof(set_connection_resp);
            } else {
                wh_get_connection_cmd_para_t  *get_connection_cmd = (wh_get_connection_cmd_para_t *)pcmd;
                wh_get_connection_resp_para_t get_connection_resp;
                os_memset(&get_connection_resp, 0 ,sizeof(wh_get_connection_resp_para_t));
                if(get_connection_cmd->port == WIFI_PORT_STA) {
                    get_connection_resp.error_code = wifi_connection_get_rssi(&get_connection_resp.rssi);
                    get_connection_resp.error_code = wifi_connection_get_link_status(&get_connection_resp.link_status);
                }
                else {
                    return -1;
                }
                ret = get_connection_resp.error_code;
                os_memcpy(txptr, &get_connection_resp, sizeof(get_connection_resp));
                *txsize = sizeof(get_connection_resp);
            }
            break;
        }
        case WFC_API_SCAN_START:    
        {
            if(api_id->set == true) {
                wh_set_scan_start_cmd_para_t  *set_scan_start_cmd = (wh_set_scan_start_cmd_para_t *)pcmd;
                wh_set_scan_start_resp_para_t set_scan_start_resp;
                uint8_t *ssid;
                uint8_t ssid_length;
                uint8_t *bssid;
                if(set_scan_start_cmd->ssid_valid == false) {
                    ssid = NULL;
                    ssid_length = 0;
                } else {
                    ssid = set_scan_start_cmd->ssid;
                    ssid_length = set_scan_start_cmd->ssid_length;
                }
                if(set_scan_start_cmd->bssid_valid == false) {
                    bssid = NULL;
                } else {
                    bssid = set_scan_start_cmd->bssid;
                }
                uint32_t number = set_scan_start_cmd->support_number;

                if (1 == g_scan_inited) {
                    wifi_connection_scan_deinit();
                    wifi_connection_unregister_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                }
                scan_ap_size = number;
                wifi_connection_register_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                scan_ap_list = (wifi_scan_list_item_t *)os_malloc(sizeof(wifi_scan_list_item_t)*scan_ap_size);
                if(scan_ap_list == NULL){
                    printf("buffer malloc fail.\r\n");
                    return -1;
                }
                wifi_connection_scan_init(scan_ap_list, number);

                set_scan_start_resp.error_code = wifi_connection_start_scan(ssid, ssid_length, bssid, set_scan_start_cmd->scan_mode, set_scan_start_cmd->scan_option);

                ret = set_scan_start_resp.error_code;
                os_memcpy(txptr, &set_scan_start_resp, sizeof(set_scan_start_resp));
                *txsize = sizeof(set_scan_start_resp);
            } else {

                return -1;
            }
            break;
        }
        case WFC_API_SCAN_STOP:     
        {
            if(api_id->set == true) {
                wh_set_scan_stop_resp_para_t set_scan_stop_resp;
                set_scan_stop_resp.error_code = wifi_connection_stop_scan();
                ret = set_scan_stop_resp.error_code;
                os_memcpy(txptr, &set_scan_stop_resp, sizeof(set_scan_stop_resp));
                *txsize = sizeof(set_scan_stop_resp);
            } else {

                return -1;
            }
            break;
        }
        case WFC_API_MAC_ADDR:
        {
            if(api_id->set == false) {
                wh_get_mac_addr_cmd_para_t  *get_mac_addr_cmd = (wh_get_mac_addr_cmd_para_t *)pcmd;
                wh_get_mac_addr_resp_para_t get_mac_addr_resp;
                get_mac_addr_resp.error_code = wifi_config_get_mac_address(get_mac_addr_cmd->port, get_mac_addr_resp.mac_addr);
                ret = get_mac_addr_resp.error_code;
                os_memcpy(txptr, &get_mac_addr_resp, sizeof(get_mac_addr_resp));
                *txsize = sizeof(get_mac_addr_resp);
            }
            break;
        }
        case WFC_API_INIT_PARA:     
        {
            if(api_id->set == true) {
                wh_set_init_para_cmd_para_t *set_init_para_cmd = (wh_set_init_para_cmd_para_t *)pcmd;
                wh_set_init_para_resp_para_t set_init_para_resp;
                wifi_config_t  *pconfig;
                wifi_config_ext_t *pconfig_ext;
                wifi_sys_cfg_t *psys_cfg;
                if(set_init_para_cmd->config_valid == false) {
                    pconfig = NULL;
                } else {
                    pconfig = &set_init_para_cmd->config;
                }
                if(set_init_para_cmd->config_ext_valid == false) {
                    pconfig_ext = NULL;
                } else {
                    pconfig_ext = &set_init_para_cmd->config_ext;
                }
                if(set_init_para_cmd->sys_cfg_valid == false) {
                    psys_cfg = NULL;
                } else {
                    psys_cfg = &set_init_para_cmd->sys_cfg;
                }

                wfc_set_wifi_host_setting(pconfig, pconfig_ext, psys_cfg);

                set_init_para_resp.error_code = 0;
                os_memcpy(txptr, &set_init_para_resp, sizeof(set_init_para_resp));
                *txsize = sizeof(set_init_para_resp);
            } else {

                return -1;
            }
            break;
        }
        case WFC_API_SLEEP_LOCK:     
        {
            if(api_id->set == true) {
                wh_set_sleep_cmd_para_t *set_sleep_para_cmd = (wh_set_sleep_cmd_para_t *)pcmd;
                wh_set_sleep_cmd_resp_para_t set_sleep_para_resp;
                ret = wfc_set_wifi_sleep(set_sleep_para_cmd->sleep);
                if(set_sleep_para_cmd->sleep == 1){
                    printf("rcv sleep cmd = %d.\r\n", set_sleep_para_cmd->sleep);
                }else if(set_sleep_para_cmd->sleep == 0) {
                    printf("rcv sleep cmd = %d.\r\n", set_sleep_para_cmd->sleep);
                }
                
                set_sleep_para_resp.error_code = ret;
                os_memcpy(txptr, &set_sleep_para_resp, sizeof(set_sleep_para_resp));
                *txsize = sizeof(set_sleep_para_resp);
            } else {

                return -1;
            }
            break;
        }
        case WFC_API_INIT_TX_POWER:     
        {
            if(api_id->set == true) {
                wh_set_tx_power_cmd_para_t *set_tx_power_cmd = (wh_set_tx_power_cmd_para_t *)pcmd;
                wh_set_tx_power_resp_para_t set_tx_power_resp;
                fw_mlme_set_tx_power_bin(set_tx_power_cmd->tx_power_bin,sizeof(set_tx_power_cmd->tx_power_bin));
                set_tx_power_resp.error_code = 0;               
                os_memcpy(txptr, &set_tx_power_resp, sizeof(set_tx_power_resp));
                *txsize = sizeof(set_tx_power_resp);
            } else {

                return -1;
            }
            break;
        }

        default:
            break;
    }

    return ret;

}

static int32_t wfc_is_mac_address_valid(uint8_t *mac_addr)
{
    uint32_t byte_sum = 0;
    for (uint32_t index = 0; index < WIFI_MAC_ADDRESS_LENGTH; index++) {
        byte_sum += mac_addr[index];
    }
    return (byte_sum != 0);
}

/**
  * @brief  wfc_get_wifi_host_mac  function to get the wifi MAC in wfc module which be set from host
  * @param  None
  * @retval None
  */
int32_t wfc_get_wifi_host_mac(uint8_t port, uint8_t *pmac_addr)
{
    if (port == WIFI_PORT_STA){
        if(wfc_is_mac_address_valid(g_wfc_system_config.sys_cfg.sta_mac_addr)){
            os_memcpy(pmac_addr, g_wfc_system_config.sys_cfg.sta_mac_addr, WIFI_MAC_ADDRESS_LENGTH);
            return 0;
        }
    }else if(port == WIFI_PORT_AP){
        if(wfc_is_mac_address_valid(g_wfc_system_config.sys_cfg.ap_mac_addr)){
            os_memcpy(pmac_addr, g_wfc_system_config.sys_cfg.ap_mac_addr, WIFI_MAC_ADDRESS_LENGTH);
            return 0;
        }
    }
    return -1;
}


/**
  * @brief  wifi_wfc_set_host_setting  function to set the received host wifi settings to wfc module
  * @param  None
  * @retval None
  */
void wfc_set_wifi_host_setting(wifi_config_t *pconfig, wifi_config_ext_t*pconfig_ext,wifi_sys_cfg_t *sys_cfg)
{   
    if (pconfig == NULL) {
        g_wfc_system_config.wfc_wifi_config_valid = FALSE;
    } else {
        os_memcpy(&g_wfc_system_config.wfc_wifi_config, pconfig, sizeof(wifi_config_t) );
        g_wfc_system_config.wfc_wifi_config_valid = TRUE;
    }

    if (pconfig_ext == NULL) {
        g_wfc_system_config.wfc_wifi_config_ext_valid = FALSE;
    } else {
        os_memcpy(&g_wfc_system_config.wfc_wifi_config_ext, pconfig_ext, sizeof(wifi_config_ext_t));
        g_wfc_system_config.wfc_wifi_config_ext_valid = TRUE;
    }

    if (sys_cfg == NULL) {
        g_wfc_system_config.sys_cfg_valid = FALSE;
    } else {
        os_memcpy(&g_wfc_system_config.sys_cfg, sys_cfg, sizeof(wifi_sys_cfg_t));
        g_wfc_system_config.sys_cfg_valid = TRUE;
    }

    wifi_os_semphr_give(g_sem_wfc_host_cfg);
}

/*[JCB]extern for test*/
extern void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);

/**
  * @brief  wifi_wfc_get_host_setting  function to get the wifi settings in wfc module which be set from host
  * @param  None
  * @retval None
  */
void wfc_get_wifi_host_setting(wifi_config_t *pconfig, uint8_t *pconfig_valid, wifi_config_ext_t *pconfig_ext,uint8_t *pconfig_ext_valid)
{
    LOG_I(common, "wfc_get_wifi_host_setting");

    /*1: need a semaphone to wait wfc get host setting done*/
    if( g_sem_wfc_host_cfg != NULL ) {
        if (wifi_os_semphr_take(g_sem_wfc_host_cfg, NULL) == 0){
            /*2: copy the config ,config_ext setting from wfc module*/
        #if 0
            pconfig->opmode = g_wfc_system_config.wfc_wifi_config.opmode;
            pconfig->sta_config.ssid_length = g_wfc_system_config.wfc_wifi_config.sta_config.ssid_length;
            memcpy(pconfig->sta_config.ssid,
                g_wfc_system_config.wfc_wifi_config.sta_config.ssid, 
                WIFI_MAC_ADDRESS_LENGTH);

            pconfig->sta_config.password_length = g_wfc_system_config.wfc_wifi_config.sta_config.password_length;
            memcpy(pconfig->sta_config.password, 
                g_wfc_system_config.wfc_wifi_config.sta_config.password, 
                pconfig->sta_config.password_length);

            pconfig->sta_config.bssid_present = g_wfc_system_config.wfc_wifi_config.sta_config.bssid_present;
            memcpy(pconfig->sta_config.bssid, 
                g_wfc_system_config.wfc_wifi_config.sta_config.bssid, 
                WIFI_MAC_ADDRESS_LENGTH);
            
            pconfig->ap_config.auth_mode    = g_wfc_system_config.wfc_wifi_config.ap_config.auth_mode;
            pconfig->ap_config.bandwidth    = g_wfc_system_config.wfc_wifi_config.ap_config.bandwidth;
            pconfig->ap_config.bandwidth_ext= g_wfc_system_config.wfc_wifi_config.ap_config.bandwidth_ext;
            pconfig->ap_config.channel      = g_wfc_system_config.wfc_wifi_config.ap_config.channel;
            pconfig->ap_config.encrypt_type = g_wfc_system_config.wfc_wifi_config.ap_config.encrypt_type;
            pconfig->ap_config.ssid_length  = g_wfc_system_config.wfc_wifi_config.ap_config.ssid_length;
            memcpy(pconfig->ap_config.ssid,
                g_wfc_system_config.wfc_wifi_config.ap_config.ssid,
                WIFI_MAC_ADDRESS_LENGTH);
            
            pconfig->ap_config.password_length= g_wfc_system_config.wfc_wifi_config.ap_config.password_length;
            memcpy(pconfig->ap_config.password,
                g_wfc_system_config.wfc_wifi_config.ap_config.password, 
                pconfig->ap_config.password_length);
        #else
            if(g_wfc_system_config.wfc_wifi_config_valid == TRUE){
                os_memcpy(pconfig,     &g_wfc_system_config.wfc_wifi_config,     sizeof(wifi_config_t));
                *pconfig_valid = TRUE;
            } else {
                pconfig = NULL;
                *pconfig_valid = FALSE;
            }

            if(g_wfc_system_config.wfc_wifi_config_ext_valid == TRUE){
                os_memcpy(pconfig_ext, &g_wfc_system_config.wfc_wifi_config_ext, sizeof(wifi_config_ext_t));
                *pconfig_ext_valid = TRUE;
            } else {
                pconfig_ext = NULL;
                *pconfig_ext_valid = FALSE;
            }
        #endif

            printf("pconfig->opmode=%d\r\n",                    pconfig->opmode);
            printf("pconfig->sta_config.ssid_length=%d\r\n",    pconfig->sta_config.ssid_length);
            printf("pconfig->sta_config.password_length=%d\r\n",pconfig->sta_config.password_length);
            printf("pconfig->sta_config.bssid_present=%d\r\n",  pconfig->sta_config.bssid_present);
            
            printf("pconfig->sta_config.ssid=%s\r\n",           pconfig->sta_config.ssid);
            hex_dump("pconfig->sta_config.password: ",          pconfig->sta_config.password, pconfig->sta_config.password_length);
            hex_dump("pconfig->sta_config.bssid: ",             pconfig->sta_config.bssid, 6);

            wifi_os_semphr_give(g_sem_wfc_host_cfg);
        } else {
            goto Exit;
        }
    } else {
        goto Exit;
    }

    return;
    
Exit:
    LOG_I(common, "wfc_get_wifi_host_setting fail");
    pconfig = NULL;
    pconfig_ext = NULL;
    *pconfig_valid = FALSE;
    *pconfig_ext_valid = FALSE;
}

int32_t wfc_get_wifi_sys_config(wifi_sys_cfg_t *sys_cfg)
{
    if((sys_cfg != NULL) && (g_wfc_system_config.sys_cfg_valid == true)) {
        os_memcpy(sys_cfg, &g_wfc_system_config.sys_cfg, sizeof(wifi_sys_cfg_t));
        return 0;
    } else {
        return -1;
    }
}

void wfc_set_wifi_status(uint8_t wifi_event_id)
{
    g_wfc_system_config.wfc_wifi_status = wifi_event_id;
}

uint32_t wfc_api_sleep_lock = 0xff;
static uint32_t g_wfc_wifi_sleep_status = 1;
int32_t wfc_set_wifi_sleep(uint8_t sleep)
{
    int32_t ret = 0;
    if(wfc_api_sleep_lock == 0xff) {
        wfc_api_sleep_lock = wifi_set_sleep_handle("wfc_low_power");
    }

    if(wfc_api_sleep_lock == 0xff) {
        return -1;
    }
    if(sleep == 1) {
        if(wfc_get_wifi_sleep() == 0) {
            ret = wifi_lock_sleep(wfc_api_sleep_lock);
            if(ret == 0) {
                g_wfc_wifi_sleep_status = 1;
            }
        }
    }else if(sleep == 0) {
        if(wfc_get_wifi_sleep() == 1) {
            ret = wifi_unlock_sleep(wfc_api_sleep_lock);
            if(ret == 0) {
               g_wfc_wifi_sleep_status = 0;
            }
        }
    }

    return ret;
}

uint32_t wfc_get_wifi_sleep(void)
{
    return g_wfc_wifi_sleep_status;
}

