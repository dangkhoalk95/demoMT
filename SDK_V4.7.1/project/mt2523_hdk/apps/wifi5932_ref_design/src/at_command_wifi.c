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

/* Includes ------------------------------------------------------------------*/
/*--- For Register AT command handler ---*/
#include "atci.h"
// System header files
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"
#include "hal_gpt.h"
#include "type_def.h"
#include "mt5932_netif.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#include "iperf_task.h"
#include "ping.h"
#include "dhcp.h"
#ifdef  MTK_SMTCN_ENABLE
#include "smt_conn.h"
#endif

#if defined(MTK_WIFI_STUB_CONF_ENABLE) && (MTK_WIFI_STUB_CONF_SPIM_ENABLE || MTK_WIFI_STUB_CONF_SDIO_MSDC_ENABLE)
#include "wfcm_stub.h"
#endif

#include "host_os_utils.h"
#include "wifi_host_os_api.h"
#include "wifi_host_api.h"
#include "wifi_host_private_api.h"
#include "wfcm_at_api.h"

log_create_module(WIFI_ATCMD, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(WIFI_ATCMD, "ATCI wfcm_over_sdio project: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(WIFI_ATCMD, "ATCI wfcm_over_sdio project: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(WIFI_ATCMD, "ATCI wfcm_over_sdio project: "fmt,##arg)


atci_status_t  atci_cmd_hdlr_mac_address(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_smart_connection(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_wifi_sys(atci_parse_cmd_param_t *parse_cmd);

atci_status_t  atci_cmd_hdlr_wifi_sta_cmd(atci_parse_cmd_param_t *parse_cmd);

atci_status_t  atci_cmd_hdlr_wifi_ap_cmd(atci_parse_cmd_param_t *parse_cmd);

atci_status_t  atci_cmd_hdlr_iperf(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_ping(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_ip(atci_parse_cmd_param_t *parse_cmd);
static uint8_t atci_cmd_iperf_server(char *param);
static uint8_t atci_cmd_iperf_client(char *param);
#ifdef MODEM_ENABLE
static void atci_cmd_iperf_cb(iperf_result_t *iperf_result);
#endif
static void ping_request_result(ping_result_t *result);
static void atci_cmd_show_ip(netif_type_t type);
static void atci_cmd_set_ip(netif_type_t type, char *ipaddr, char *netmask, char *gw);
atci_status_t atci_cmd_hdlr_rmtos(atci_parse_cmd_param_t *parse_cmd);

uint16_t wfcm_to_slave(uint16_t op, uint8_t *ptr, uint32_t ptrSize, uint8_t *rsp, uint32_t *rspSize, uint8_t fromISR);

static uint8_t BtoH(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return (ch - '0');    /* Handle numerals*/
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 0xA);    /* Handle capitol hex digits*/
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 0xA);    /* Handle small hex digits*/
    }
    return (255);
}

static void AtoH(char *src, char *dest, int destlen)
{
    char *srcptr;
    char *destTemp;

    srcptr = src;
    destTemp = (char *) dest;

    while (destlen--) {
        *destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
        *destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
        destTemp++;
    }
}

/*---  Variant ---*/
atci_cmd_hdlr_item_t item_table[] = {

    {"AT+WIFI",      atci_cmd_hdlr_wifi_sys,      0, 0},
    {"AT+WIFIMAC",   atci_cmd_hdlr_mac_address,      0, 0},
/*
    {"AT+WIFIIP",    atci_cmd_hdlr_ip_address,    0, 0},
*/
#if defined(MTK_SMTCN_ENABLE)
    {"AT+SMNT",      atci_cmd_hdlr_smart_connection,      0, 0},
#endif
/*
    {"AT+WIFICH",    atci_cmd_hdlr_ch_bw,      0, 0},
    {"AT+WIFIBASupport",    atci_cmd_hdlr_block_ack,      0, 0},
    {"AT+WIFIWirelessMode",    atci_cmd_hdlr_wireless_mode,    0, 0},
    {"AT+WIFITxRawPkt",    atci_cmd_hdlr_wifi_tx_raw_cmd,    0, 0},
*/
    {"AT+WIFISTA",    atci_cmd_hdlr_wifi_sta_cmd,    0, 0},
    {"AT+WIFIAP",     atci_cmd_hdlr_wifi_ap_cmd,    0, 0},
    {"AT+IPERF",        atci_cmd_hdlr_iperf,         0, 0},
    {"AT+PING",         atci_cmd_hdlr_ping,         0, 0},
    {"AT+IP",           atci_cmd_hdlr_ip,         0, 0},
    {"AT+RMTOS",        atci_cmd_hdlr_rmtos,         0, 0},
};

wifi_scan_list_item_t scan_ap_list[SCAN_AP_LIST_MAX_COUNT];
static int scan_ap_size = 0;

extern uint8_t g_scan_inited;
extern uint8_t g_scan_list_size;
extern volatile bool g_wifi_host_scanning;

static int scan_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    uint32_t count;
    int i = 0;

    switch (event_id) {

        case WIFI_EVENT_IOT_SCAN_COMPLETE:
            handled = 1;
            g_wifi_host_scanning = false;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&count);
            for (i = 0; i < scan_ap_size; i++) {
                printf("\nCh\tSSID\t\t\t\t\tBSSID\t\t\tAuth\tCipher\tRSSI\tWPS_EN\tCM\tDPID\tSR\r\n");
                printf("%d\t", scan_ap_list[i].channel);
                printf("%-33s\t", scan_ap_list[i].ssid);
                printf("%02x:%02x:%02x:%02x:%02x:%02x\t",
                       scan_ap_list[i].bssid[0],
                       scan_ap_list[i].bssid[1],
                       scan_ap_list[i].bssid[2],
                       scan_ap_list[i].bssid[3],
                       scan_ap_list[i].bssid[4],
                       scan_ap_list[i].bssid[5]);
                printf("%d\t", scan_ap_list[i].auth_mode);
                printf("%d\t", scan_ap_list[i].encrypt_type);
                printf("%d\t", scan_ap_list[i].rssi);
                printf("%d\t", scan_ap_list[i].is_wps_supported);
                printf("%d\t", scan_ap_list[i].wps_element.configuration_methods);
                printf("%d\t", scan_ap_list[i].wps_element.device_password_id);
                printf("%d\t", scan_ap_list[i].wps_element.selected_registrar);
                printf("\r\n");
            }

            printf("[MTK Event Callback Sample]: Scan Done! Time = %d.\n",(int)count);
            break;

        default:
            handled = 0;
            printf("[MTK Event Callback Sample]: Unknown event(%d)\n", event_id);
            break;
    }
    return handled;
}



void wifi_atci_example_init()
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;

    LOGW("atci_example_init\r\n");

    ret = atci_register_handler(item_table, sizeof(item_table) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        LOGW("wifi_atci_example_init register success\r\n");
    } else {
        LOGW("wifi_atci_example_init register fail\r\n");
    }

}



#if defined(MTK_SMTCN_ENABLE)
atci_status_t atci_cmd_hdlr_smart_connection(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t smnt_flag = 0;     //1 is start, 0 is stop
    int32_t ret = 0;
    LOGI("atci_cmd_hdlr_smart_connection");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+SMNT=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+SMNT?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+SMNT:<smnt_flag>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+SMNT
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+SMNT=<smnt_flag>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            printf("AT+SMNT=<smnt_flag> (smnt_flag: 1 is start, 0 is stop)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+SMNT=") != NULL) {
                    LOGI("AT CMD received smnt_flag:%s", param);
                    param = strtok((char *)parse_cmd->string_ptr, "AT+SMNT=");
                    smnt_flag = (uint8_t)atoi(param);
                    if (0 == smnt_flag) {
                        //ret = mtk_smart_stop();
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Stop smart connect failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Stop smart connect success\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == smnt_flag) {
                        //ret = mtk_smart_connect();
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Start smart connect failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Start smart connect success\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}
#endif

atci_status_t atci_cmd_hdlr_wifi_sys(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;      //0 is get, 1 is set
    int32_t ret = 0;
    LOGI("atci_cmd_hdlr_opmode");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFI=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFI?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFI:\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFI
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFI=xxx,<op>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=sleep,") != NULL) {
                    LOGI("AT+WIFI=sleep,<lock>  (lock: 0 means unlock sleep; 1 means lock sleep)\n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received lock command:%s", param);
                    operation = (uint8_t)atoi(param);
                    if (0 == operation) {
                        ret = wifi_host_config_set_wifi_sleep(operation);
                        printf("rcv unlock sleep.\r\n");
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Set Sleep failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Set Sleep:%d\n\r", operation);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        ret = wifi_host_config_set_wifi_sleep(operation);
                        printf("rcv lock sleep.\r\n");
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Set Sleep failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Set Sleep:%d\n\r", operation);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                        }
                    }
                }else if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=0") != NULL) {
                    LOGI("AT+WIFI=<op>  (op: 0 means stop; 1 means start)\n");

                    ret = wifi_host_config_set_wifi_stop();
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Stop wifi failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Stop wifi\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                     }
                } else if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=1") != NULL) {
                    LOGI("AT+WIFI=<op>  (op: 0 means stop; 1 means start)\n");
                    ret = wifi_host_config_set_wifi_start();
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Start wifi failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Start wifi\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    }
                }

            }else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
             break;
        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_mac_address(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;      //0 is get, 1 is set
    int32_t ret = 0;
    uint8_t addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char addr2[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char mac_addr_buf[17] = {0};
    LOGI("atci_cmd_hdlr_mac_address");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIMAC=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIMAC?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFIMAC\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIMAC
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIMAC=<set|get>,<sta|ap>,mac[,<value>] the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFIMAC=<set|get>, <sta|ap>, mac[<value>](op: 0 means get; 1 means set)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIMAC=") != NULL) {
                    param = strtok((char *)parse_cmd->string_ptr, "=");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    param = strtok(NULL, ",");
                    uint8_t port = (uint8_t)atoi(param);
                    if ( atoi(param) < 0 ) {
                        printf("Invalid port\n");
                        return -1;
                    }
                    if (0 == operation) {
                        ret = wifi_host_config_get_mac_address(port, addr);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Get MacAddress failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            for (int i = 0; i < WIFI_MAC_ADDRESS_LENGTH; i++) {
                                addr2[i] = addr[i];
                            }

                            LOGI("wifi_config_get_mac_address(port%ld): (%02x:%02x:%02x:%02x:%02x:%02x)\n", port,
                                 addr2[0], addr2[1], addr2[2],
                                 addr2[3], addr2[4], addr2[5]);
                            sprintf((char *)resonse.response_buf, "SUCCEED:Get MacAddress \n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        param = strtok(NULL, "m");
                        param = param + 2;
                        sscanf(param, "%s", mac_addr_buf);
                        char *group_name = (WIFI_PORT_AP == port) ? "AP" : "STA";
#ifdef MTK_NVDM_ENABLE
                        ret = nvdm_write_data_item(group_name, "MacAddr", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)mac_addr_buf, strlen((char *)mac_addr_buf));
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Write NVDM MAC Status=%ld,Port=%d,Mac=%s\n\r", ret, port, mac_addr_buf);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Write Port=%d, Mac=%s, Please Reboot.\n\r", port, mac_addr_buf);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                        LOGI("nvdm_write_data_item Status=%d,Port=%d,Mac=%s\n", ret, port, mac_addr_buf);
#else
                        LOGI("NVDM not enabled\n");
                        sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
#endif
                    }
                }
                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}



void wfcm_dump_buf(uint8_t *pbuf, uint32_t bufSize);
atci_status_t atci_cmd_hdlr_wifi_sta_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;

    LOGI("atci_cmd_hdlr_wifi_sta_cmd");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFISTA=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFISTA?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFIWSTA\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFISTA
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFISTA=xxx,xxx,xxx,xxx
            LOGI("AT Executing...\r\n");
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
            	#if 0//defined(MTK_WIFI_STUB_CONF_ENABLE) && (MTK_WIFI_STUB_CONF_SPIM_ENABLE || MTK_WIFI_STUB_CONF_SDIO_MSDC_ENABLE)
            	uint16_t      rsp;
                uint8_t       rspbuf[256];
                uint32_t      rspSize;

                //wfcm_dump_buf(parse_cmd->string_ptr, parse_cmd->string_len);
                rsp = wfcm_to_slave(WFC_CMD_AT, (uint8_t *)parse_cmd->string_ptr, parse_cmd->string_len, rspbuf, &rspSize, 0);
                if (rsp == WFC_RSP_OK) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }
            	#else
				char *key = NULL;
				uint8_t port = 0;    // STA port
				uint8_t link = 0;   // 0 is disconnected, 1 is connected
				uint8_t auth = 0;
				uint8_t encrypt = 0;
				wifi_wep_key_t wep_key = {{{0}}};
				int32_t ret = 0;
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=disconnect") != NULL) {
                    ret = wifi_host_connection_disconnect_ap();
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Disconnect failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Disconnect Success\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=connectinfo") != NULL) {
                    ret = wifi_host_connection_get_link_status(&link);
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Get Link Status failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Current link status in station mode:%d\n\r", link);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                    sprintf((char *)parse_cmd->string_ptr, "0");
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=connect") != NULL) {
                    LOGI("AT+WIFISTA=connect,s<ssid>,a<authMode>,e<encryption>,p<key>,<wep_key_index>\n");
                    param = strtok((char *)parse_cmd->string_ptr, "s");
                    param = strtok(NULL, ",");
                    LOGI("ssid=%s", param);
                    ret = wifi_host_config_set_ssid(port, (uint8_t *)param, os_strlen(param));
                    if (ret < 0) {
                        printf("Set ssid failed\n");
                        ret = 1;
                    }
                    param = strtok(NULL, ",");
                    param = param + 1;
                    auth = atoi(param);
                    LOGI("auth=%d", auth);
                    param = strtok(NULL, ",");
                    param = param + 1;
                    encrypt = atoi(param);
                    LOGI("encrypt=%d", encrypt);
                    ret = wifi_host_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
                    if (ret < 0) {
                        printf("Set auth mode failed\n");
                        ret = 1;
                    }
                    if ((auth == 0) && (encrypt == 0)) {
                        LOGI("WEP encrypt\n\n");
                        param = strtok(NULL, ",");
                        param = param + 1;
                        key = param;
                        LOGI("WEP key=%s", key);
                        param = strtok(NULL, ",");
                        param = param + 1;
                        wep_key.wep_tx_key_index = atoi(param);
                        LOGI("WEP key index=%d", wep_key.wep_tx_key_index);
                        if (os_strlen(key) == 10 || os_strlen(key) == 26) {
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key) / 2;
                            AtoH((char *)key, (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
                        } else if (os_strlen(key) == 5 || os_strlen(key) == 13) {
                            os_memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], key, os_strlen(key));
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key);
                        } else {
                            printf("invalid length of value.\n");
                            return 1;
                        }

                        ret = wifi_host_config_set_password(port, wep_key.wep_key[wep_key.wep_tx_key_index], wep_key.wep_key_length[wep_key.wep_tx_key_index], wep_key.wep_tx_key_index);

                    } else if ((auth != 0) && (encrypt != 1)) {
                        param = strtok(NULL, ",");
                        param = param + 1;
                        LOGI("password=%s", param);
                        ret = wifi_host_config_set_password(port, (uint8_t *)param, os_strlen(param), -1);
                    }
                    ret = wifi_host_config_reload_setting();
                    if (ret < 0) {
                        printf("Connect to network failed\n");
                        ret = 1;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=scanmode") != NULL) {
                    LOGI("AT+WIFISTA=scanmode,<start/stop>,<active|passive>,<partial/full> (0:stop 1:start;0:full 1:partial; 0:active 1:passive)\n");
                    param = strtok((char *)parse_cmd->string_ptr, ",");
                    param = strtok(NULL, ",");
                    uint8_t start = atoi(param);
                    if (!start) {
                        ret = wifi_host_connection_stop_scan();
                    } else {
                        param = strtok(NULL, ",");
                        uint8_t scan_mode = atoi(param);
                        param = strtok(NULL, ",");
                        uint8_t scan_option = atoi(param);
                        wifi_host_connection_scan_init(scan_ap_list, 8);
                        ret = wifi_host_connection_start_scan(NULL, 0, NULL, scan_mode, scan_option);
                    }
                    if (ret < 0) {
                        ret = 1;
                    }
                    sprintf((char *)parse_cmd->string_ptr, "0");
                }
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=scan") != NULL) {
                    LOGI("AT+WIFISTA=scan,<max_ap_num> \n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");

                    uint32_t number = atoi(param);
                    uint32_t count;
                    if(number > SCAN_AP_LIST_MAX_COUNT) {
                        number = SCAN_AP_LIST_MAX_COUNT;
                    }

                    if (1 == g_scan_inited) {
                        wifi_host_connection_scan_deinit();
                        wifi_host_connection_unregister_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                    }

                    scan_ap_size = number;

                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&count);
                    printf("Start scan time = %d.\r\n", (int)count);
                    wifi_host_connection_register_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                    wifi_host_connection_scan_init(scan_ap_list, number);
                    ret = wifi_host_connection_start_scan(NULL, 0, NULL, 0, 0);
                    if (ret < 0) {
                        ret = 1;
                    }
                }
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=network") != NULL) {
                    LOGI("AT+WIFISTA=network, <dhcp|static> \n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    char *group_name = "STA";
#ifdef MTK_NVDM_ENABLE
                    ret = nvdm_write_data_item(group_name, "IpMode", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)param, strlen(param));
                    if (ret != 0) {
                        sprintf((char *)resonse.response_buf, "FAILED: Write NVDM IpMode Status=%ld\n\r", ret);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED: Write IpMode =%s, Please Reboot.\n\r", param);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
#else
                    LOGI("NVDM not enabled\n");
                    sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 1;
#endif
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=psmode") != NULL) {
                    LOGI("AT+WIFISTA=psmode,<pm>\n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    char *group_name = "STA";
#ifdef MTK_NVDM_ENABLE
                    ret = nvdm_write_data_item(group_name, "PSMode", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)param, strlen(param));
                    if (ret != 0) {
                        sprintf((char *)resonse.response_buf, "FAILED: Write NVDM PSMode Status=%ld\n\r", ret);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED: Write PSMode =%c, Please Reboot.\n\r", *param);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
#else
                    LOGI("NVDM not enabled\n");
                    sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 1;
#endif
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=rssi") != NULL) {
                    LOGI("AT+WIFISTA=rssi(Get rssi of the connected AP)");
                    int8_t rssi = 0;
                    ret = wifi_host_connection_get_rssi(&rssi);
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Get rssi of the connected AP failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Get rssi of the connected AP is :%d\n\r", rssi);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                    sprintf((char *)parse_cmd->string_ptr, "0");
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }
                #endif

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}


atci_status_t atci_cmd_hdlr_wifi_ap_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    char *key = NULL;
    uint8_t port = 1;    // AP port
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    wifi_wep_key_t wep_key = {{{0}}};
    int32_t ret = 0;

    LOGI("atci_cmd_hdlr_wifi_sta_cmd");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIAP=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIAP?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+AT+WIFIAP\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIAP
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIAP=xxx,xxx,xxx,xxx
            LOGI("AT Executing...\r\n");
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIAP=connect") != NULL) {
                    LOGI("AT+WIFIAP=connect,s<ssid>,a<authMode>,e<encryption>,p<key>,<wep_key_index>\n");
                    param = strtok((char *)parse_cmd->string_ptr, "s");
                    param = strtok(NULL, ",");
                    LOGI("ssid=%s", param);
                    ret = wifi_host_config_set_ssid(port, (uint8_t *)param, os_strlen(param));
                    if (ret < 0) {
                        LOGE("Set ssid failed\n");
                        ret = 1;
                    }
                    param = strtok(NULL, ",");
                    param = param + 1;
                    auth = atoi(param);
                    LOGI("auth=%d", auth);
                    param = strtok(NULL, ",");
                    param = param + 1;
                    encrypt = atoi(param);
                    LOGI("encrypt=%d", encrypt);
                    ret = wifi_host_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
                    if (ret < 0) {
                        printf("Set auth mode failed\n");
                        ret = 1;
                    }
                    if ((auth == 0) && (encrypt == 0)) {
                        LOGI("WEP encrypt\n\n");
                        param = strtok(NULL, ",");
                        param = param + 1;
                        key = param;
                        LOGI("WEP key=%s", key);
                        param = strtok(NULL, ",");
                        param = param + 1;
                        wep_key.wep_tx_key_index = atoi(param);
                        LOGI("WEP key index=%d", wep_key.wep_tx_key_index);
                        if (os_strlen(key) == 10 || os_strlen(key) == 26) {
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key) / 2;
                            AtoH((char *)key, (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
                        } else if (os_strlen(key) == 5 || os_strlen(key) == 13) {
                            os_memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], key, os_strlen(key));
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key);
                        } else {
                            printf("invalid length of value.\n");
                            return 1;
                        }

                        ret = wifi_host_config_set_password(port, wep_key.wep_key[wep_key.wep_tx_key_index], wep_key.wep_key_length[wep_key.wep_tx_key_index], wep_key.wep_tx_key_index);

                    } else if ((auth != 0) && (encrypt != 1)) {
                        param = strtok(NULL, ",");
                        param = param + 1;
                        LOGI("password=%s", param);
                        ret = wifi_host_config_set_password(port, (uint8_t *)param, os_strlen(param), -1);
                    }

                    ret = wifi_host_config_reload_setting();
                    if (ret < 0) {
                        LOGE("Connect to network failed\n");
                        ret = 1;
                    }
                }
                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}


atci_status_t atci_cmd_hdlr_iperf(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;

    LOGI("atci_cmd_hdlr_iperf\n");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+IPERF=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\n+IPERF:(0,1)\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+IPERF?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+IPERF:<ip>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;    // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:    // rec: AT+IPERF
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+IPERF=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received: %s", parse_cmd->string_ptr);

            param = strtok((char *)parse_cmd->string_ptr, "AT+IPERF=");
            if (param != NULL) {
                int len = strlen((char *)(param + 3));
                if (param[0] == '-' && param[1] == 'c') {
                    char *str = (char *)pvPortMalloc(len + 1);
                    if(str != NULL) {
                        str[len] = '\0';
                        os_memcpy(str, param + 3, len);
                        atci_cmd_iperf_client(str);
                        vPortFree(str);
                    }
                } else if (param[0] == '-' && param[1] == 's') {
                    char *str = (char *)pvPortMalloc(len + 1);
                    if(str != NULL) {
                        str[len] = '\0';
                        os_memcpy(str, param + 3, len);
                        atci_cmd_iperf_server(str);
                        vPortFree(str);
                    }
                } else {
                    strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;

}


static uint8_t atci_cmd_iperf_server(char *param)
{
    int i;
    char **g_iperf_param = NULL;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    char *p = NULL;
    int is_create_task = 0;
    LOGI("atci_cmd_iperf_server.\n");

    if (param == NULL) {
        return 0;
    }
    g_iperf_param = pvPortMalloc(IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        return 0;
    }

#ifdef MODEM_ENABLE
    sio_set_mode(gprs_get_app_id(), SIO_DATA_TYPE_DATA);
    iperf_register_callback(atci_cmd_iperf_cb);
#endif

    i = 0;
    p = strtok(param, " ");
    while (p != NULL && i < 13) {
        strcpy((char *)&g_iperf_param[i * offset], p);
        i++;
        p = strtok(NULL, " ");
    }

    for (i = 0; i < 13; i++) {
        if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) == 0) {
            //printf("Iperf UDP Server: Start!\n");
            //printf("Iperf UDP Server Receive Timeout = 20 (secs)\n");
            //xTaskCreate((TaskFunction_t)iperf_udp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / ((uint32_t)sizeof(StackType_t)), g_iperf_param, IPERF_TASK_PRIO, NULL);
            xTaskCreate((TaskFunction_t)iperf_udp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / ((uint32_t)sizeof(StackType_t)), g_iperf_param, 6, NULL);
            is_create_task = 1;
            break;
        }
    }

    if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) != 0) {
        printf("Iperf TCP Server: Start!");
        printf("Iperf TCP Server Receive Timeout = 20 (secs)");
        //xTaskCreate((TaskFunction_t)iperf_tcp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , NULL);
        xTaskCreate((TaskFunction_t)iperf_tcp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, 6 , NULL);
        is_create_task = 1;
    }

    if (is_create_task == 0) {
        vPortFree(g_iperf_param);
    }
    return 0;
}

static uint8_t atci_cmd_iperf_client(char *param)
{
    int i;
    char **g_iperf_param = NULL;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    int is_create_task = 0;
    char *p = NULL;

    if (param == NULL) {
        return 0;
    }

    g_iperf_param = pvPortCalloc(1, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        //printf("Warning: No enough memory to running iperf.\n");
        return 0;
    }

#ifdef MODEM_ENABLE
    sio_set_mode(gprs_get_app_id(), SIO_DATA_TYPE_DATA);
    iperf_register_callback(atci_cmd_iperf_cb);
#endif

    i = 0;
    p = strtok(param, " ");
    while (p != NULL && i < 18) {
        strcpy((char *)&g_iperf_param[i * offset], p);
        i++;
        p = strtok(NULL, " ");
    }

    for (i = 0; i < 18; i++) {
        if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) == 0) {
            printf("iperf run udp client\n");
            xTaskCreate((TaskFunction_t)iperf_udp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , NULL);
            //xTaskCreate((TaskFunction_t)iperf_udp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, 6 , NULL);
            is_create_task = 1;
            break;
        }
    }

    if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) != 0) {
        printf("Iperf TCP Client: Start!");
#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
        xTaskCreate((TaskFunction_t)iperf_tcp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , &g_balance_ctr.tx_handle);
#else
        xTaskCreate((TaskFunction_t)iperf_tcp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , NULL); 
#endif
        is_create_task = 1;
    }

    if (is_create_task == 0) {
        vPortFree(g_iperf_param);
    }

    return 0;
}


#ifdef MODEM_ENABLE
static void atci_cmd_iperf_cb(iperf_result_t *iperf_result)
{
    atci_response_t resonse;

    vTaskDelay(3000);

    sio_set_mode(gprs_get_app_id(), SIO_DATA_TYPE_COMMAND);

    // TODO: handle the result
    resonse.response_flag = 0; // Command Execute Finish.
    if (iperf_result)
        sprintf((char *)resonse.response_buf, "\r\n iperf finish, %s, data_size = %d, total = %s, result = %s \r\n",
                iperf_result->report_title, (int)iperf_result->data_size, iperf_result->total_len, iperf_result->result);
    else {
        strcpy((char *)resonse.response_buf, "\r\n iperf finish, no result!\r\n");
    }
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}
#endif

atci_status_t atci_cmd_hdlr_ping(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int count = 3;
    int pktsz = 64;
    char ip[16] = {0};
    char *p = NULL;

    LOGI("atci_cmd_hdlr_ping\n");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+PING=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+PING?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+PING:<ip>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+PING
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+PING=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "AT+PING=");
            if (param != NULL) {
                p = strtok(param, ",");
                strcpy(ip, p);
                p = strtok(NULL, ",");
                if (p != NULL) {
                    count = atoi(p);
                    p = strtok(NULL, ",");
                    if (p != NULL) {
                        pktsz = atoi(p);
                    }
                }
            }
            LOGI("AT CMD received IP:%s, count:%d, ptksz:%d", ip, count, pktsz);
            ping_request(count, ip, PING_IP_ADDR_V4, pktsz, ping_request_result);

            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

static void ping_request_result(ping_result_t *result)
{
    atci_response_t resonse = {{0}};
    int len = 0;

    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;
    sprintf((char *)resonse.response_buf, "Packets: Sent = %d, Received =%d, Lost = %d (%d%% loss)\n\r", (int)result->total_num, (int)result->recv_num, (int)result->lost_num, (int)((result->lost_num * 100) / result->total_num));
    len = strlen((char *)resonse.response_buf);
    sprintf((char *)(resonse.response_buf + len), "Packets: min = %d, max =%d, avg = %d\n", (int)result->min_time, (int)result->max_time, (int)result->avg_time);
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}

atci_status_t atci_cmd_hdlr_ip(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;   //0 is get, 1 is set
    uint8_t port = 0;
    netif_type_t type = NETIF_TYPE_STA;
    char ip[16] = {0};
    char netmask[16] = {0};
    char gw[16] = {0};
    char *p = NULL;

    LOGI("atci_cmd_hdlr_ping\n");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+IP=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+IP?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+IP:<get|set>,<sta|ap>,ip[<value>](op: 0 means get; 1 means set)\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+IP
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+IP=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "AT+IP=");
            p = strtok(param, ",");
            if (p != NULL) {
                operation = (uint8_t)atoi(p);
                p = strtok(NULL, ",");
                if (p != NULL) {
                    port = (uint8_t)atoi(p);
                }
            }
            if (port == 0) {
                type = NETIF_TYPE_STA;
            } else if (port == 1) {
                type = NETIF_TYPE_AP;
            } else {
                LOGI("invalid port");
            }

            if (operation == 0) {
                atci_cmd_show_ip(type);
            } else {
                p = strtok(NULL, ",");
                strcpy(ip, p);
                p = strtok(NULL, ",");
                strcpy(netmask, p);
                p = strtok(NULL, ",");
                strcpy(gw, p);
                atci_cmd_set_ip(type, ip, netmask, gw);
            }

            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

static void atci_cmd_show_ip(netif_type_t type)
{
    atci_response_t resonse = {{0}};
    char ip[16] = {0};
    char netmask[16] = {0};
    char gw[16] = {0};
    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;

    LOGI("show_ip:port=%d", type);
    struct netif *iface = netif_find_by_type(type);
    if (!iface) {
        sprintf((char *)resonse.response_buf, "Can't find interface by type. %d", type);
        resonse.response_len = strlen((char *)resonse.response_buf);
        atci_send_response(&resonse);
        return;
    }

#if LWIP_DHCP
    if (dhcp_supplied_address(iface)) {
        struct dhcp *d = iface->dhcp;
        strcpy(ip, ip4addr_ntoa(&d->offered_ip_addr));
        strcpy(netmask, ip4addr_ntoa(&d->offered_sn_mask));
        strcpy(gw, ip4addr_ntoa(&d->offered_gw_addr));
        sprintf((char *)resonse.response_buf, "mode:dhcp\r\nip:%s, netmask:%s, gateway:%s\n\r", ip, netmask, gw);
        resonse.response_len = strlen((char *)resonse.response_buf);
    } else
#endif
    {
        strcpy(ip, ip4addr_ntoa(&iface->ip_addr));
        strcpy(netmask, ip4addr_ntoa(&iface->netmask));
        strcpy(gw, ip4addr_ntoa(&iface->gw));
        sprintf((char *)resonse.response_buf, "mode:static\r\nip:%s, netmask:%s, gateway:%s\n\r", ip, netmask, gw);
        resonse.response_len = strlen((char *)resonse.response_buf);
    }
    atci_send_response(&resonse);
}

static void atci_cmd_set_ip(netif_type_t type, char *ipaddr, char *netmask, char *gw)
{
    atci_response_t resonse = {{0}};
    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;
    ip4_addr_t   ip;
    ip4_addr_t   nm;
    ip4_addr_t   gateway;

    LOGI("set_ip: port=%d", type);
    struct netif *iface = netif_find_by_type(type);
    if (!iface) {
        sprintf((char *)resonse.response_buf, "Can't find interface by type. %d", type);
        resonse.response_len = strlen((char *)resonse.response_buf);
        atci_send_response(&resonse);
        return;
    }
    LOGI("set_ip:ipaddr=%s", ipaddr);
    LOGI("set_ip:netmask=%s", netmask);
    LOGI("set_ip:gw=%s", gw);
    if ((ip4addr_aton(ipaddr, &ip) == 0) ||
            (ip4addr_aton(netmask, &nm) == 0) ||
            (ip4addr_aton(gw, &gateway) == 0)) {
        sprintf((char *)resonse.response_buf, "input error, can't set address\r\n");
        resonse.response_len = strlen((char *)resonse.response_buf);
        atci_send_response(&resonse);
        return;
    }
    netif_set_addr(iface, &ip, &nm, &gateway);
    sprintf((char *)resonse.response_buf, "Set address success\r\n");
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}

atci_status_t atci_cmd_hdlr_rmtos(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    //int32_t ret = 0;

    LOGI("atci_cmd_hdlr_rmtos");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFISTA=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFISTA?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+RMTOS\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFISTA
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFISTA=xxx,xxx,xxx,xxx
            LOGI("AT Executing...\r\n");
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
            	#if defined(MTK_WIFI_STUB_CONF_ENABLE) && (MTK_WIFI_STUB_CONF_SPIM_ENABLE || MTK_WIFI_STUB_CONF_SDIO_MSDC_ENABLE)
            	uint16_t      rsp;
                uint8_t       rspbuf[256];
                uint32_t      rspSize;

                rsp = wfcm_to_slave(WFC_CMD_AT, (uint8_t *)parse_cmd->string_ptr, parse_cmd->string_len, rspbuf, &rspSize, 0);
                if (rsp == WFC_RSP_OK) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }
                #endif
            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

