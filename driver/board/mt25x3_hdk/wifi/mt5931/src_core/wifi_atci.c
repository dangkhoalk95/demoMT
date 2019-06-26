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

#include "wifi_atci.h"
// System head file
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"
#include "mtk_wifi_adapter.h"
#include "kal_public_api.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#include "wifi_common_config.h"
#include "wifi_api.h"

#define WIFI_SCAN_RSP_MAX_AP        (10)     //memory need(bytes) = 12 + WIFI_SCAN_RSP_MAX_AP * 140
#define WLAN_STATUS_PENDING         ((uint32_t) 0x00000103L)


extern wl_drv_hdl __g_virtual_hdl;


atci_print_level_t wifi_atci_print_level = ATCI_PRINT_COMMON;
atci_response_t scan_response = {{0}};
uint8_t scan_max_ap = WIFI_SCAN_RSP_MAX_AP;

static void wifi_pkt_rx_callback(wndrv_test_rx_status_struct *result);
static int32_t wifi_scan_callback(wifi_event_t event, uint8_t *payload, uint32_t length);
void atci_cmd_wifi_ready_announce(void);


void atci_cmd_wifi_ready_announce(void)
{
    atci_response_t resonse = {{0}};

    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;

    strcpy((char *)resonse.response_buf, "WIFI INIT DONE\r\n");
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}

int32_t wifi_scan_callback(wifi_event_t event, uint8_t *payload, uint32_t length)
{
    WLAN_BSS_INFO *ap_info = NULL;
    uint32_t buff_pos = 0;
    WLAN_SCAN_RESULT *scan_result;     //scan complete result point

    if (WIFI_EVENT_IOT_SCAN_COMPLETE == event) {
        scan_result = (WLAN_SCAN_RESULT *) payload;
        if (scan_result->count > 0) {
            ap_info = scan_result->bss_info;
            for (uint8_t i = 0; i < scan_result->count; i++, ap_info = (WLAN_BSS_INFO *)((uint8_t *)ap_info + ap_info->length)) {
                buff_pos = sprintf((char *)scan_response.response_buf, "\r\n\r\n%-4s%-15s%-20s%-8s%-12s%-12s\r\n", "Ch", "SSID", "BSSID", "RSSI", "Encrypt", "Auth");
                buff_pos += sprintf((char *)scan_response.response_buf + buff_pos, "%-4d",  ap_info->chanspec);
                buff_pos += sprintf((char *)scan_response.response_buf + buff_pos, "%-15s", ap_info->SSID);
                buff_pos += sprintf((char *)scan_response.response_buf + buff_pos, "%02x:%02x:%02x:%02x:%02x:%02x   ",
                                    ap_info->BSSID[0], ap_info->BSSID[1], ap_info->BSSID[2], ap_info->BSSID[3], ap_info->BSSID[4], ap_info->BSSID[5]);
                buff_pos += sprintf((char *)scan_response.response_buf + buff_pos, "%ddBm  ", (int)ap_info->RSSI);
                buff_pos += sprintf((char *)scan_response.response_buf + buff_pos, "%d=%-10s",  ap_info->encryption, look_for_encrytion_type(ap_info->encryption));
                buff_pos += sprintf((char *)scan_response.response_buf + buff_pos, "%d=%s",  ap_info->authMode, look_for_auth_mode(ap_info->authMode));
                scan_response.response_len = strlen((char *)scan_response.response_buf);
                atci_send_response(&scan_response);
            }
        } else {
            //printc("WARNING: Scan Result is Empty!");
            return 0;
        }
    } else {
        //printc("ERROR:Wrong WIFI EVENT!");
        return -1;
    }
    return 0;
}

static void wifi_pkt_rx_callback(wndrv_test_rx_status_struct *result)
{
    uint32_t per  = 0;

    per = (result->int_crc_err_num * 10000) / (result->int_crc_err_num + result->int_rx_ok_num);
    printc("RX_ERR:%d,RX_OK:%d,Err_Rate:%d.%d%\r\n",
           result->int_crc_err_num, result->int_rx_ok_num, per / 100, per % 100);
    ATCI_PRINT(ATCI_PRINT_COMMON, "RX_ERR:%d,RX_OK:%d\r\n",
               result->int_crc_err_num, result->int_rx_ok_num);
}

/*
****************MFG ATCI CMD******************
*
*/
atci_status_t atci_cmd_hdlr_wifi_em_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    int32_t ret = 1;    //default is error status
    char *param1 = NULL, *param2 = NULL;
    wndrv_test_tx_struct    tx_param = {0};
    wndrv_test_pkt_tx_struct tx_pkt_param = {0};
    uint8_t country_code[2] = {0};
    uint32_t ch_freq = 0;
    wifi_test_item_t test_item = 0;
    wifi_tx_mode_t tx_mode = 0;
    uint8_t param_num = 0;
    //char test_buf1[] = "2412000,0,0,0,0,1,0";
    //char test_buf2[] = "2412000,0,0,0,0,1,0,0,100,1024,0,0";
    uint8_t read_write = 0;
    uint8_t cal_item = 0;
    uint8_t cal_data = 0;
    uint32_t cal_data_len = 0;
    uint8_t bandwidth = 0;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIEM=?
            printc("+WIFIEM:OK\r\n");
            ret = 0;
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIEM?
            printc("+WIFIEM:%d\r\n", 0);
            ret = 0;
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIEM
            printc("ACTIVE\r\n");
            ret = 0;
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIEM=xxxx,xx,xx,xx
            //printc("RF Test, input_buf: %s\n", parse_cmd->string_ptr);
            param1 = strstr(parse_cmd->string_ptr, "AT+WIFIEM=");
#if 0
            //For Debug :param1 = "AT+EM=TX,CT,12,34,56,78\r\n"
            param1 = strtok(parse_cmd->string_ptr, ",");//AT+EM=TX
            param1 = strtok(NULL, ",");//TX
            param2 = strtok(NULL, ",");//CT
            param3 = strtok(NULL, ",");//12
            param4 = strtok(NULL, ",");//34
            param5 = strtok(NULL, ",");//56
            param6 = strtok(NULL, ",");//78
            *(param6 + 1) = 0x0d; // /r
            *(param6 + 2) = 0x0a; // /n
#endif
            if (NULL != param1) {
                /***************SYS ATCI CMD******************/
                if (NULL != strstr(param1, "=ENTER")) {
                    test_item = WIFI_EM_ENTER;
                } else if (NULL != strstr(param1, "=STOP")) {
                    test_item = WIFI_EM_STOP;
                } else if (NULL != strstr(param1, "=TX")) {
                    test_item = WIFI_EM_TX;
                } else if (NULL != strstr(param1, "=RX")) {
                    test_item = WIFI_EM_RX;
                } else if (NULL != strstr(param1, "=CAL")) {
                    test_item = WIFI_EM_CAL;
                } else if (NULL != strstr(param1, "=BW")) {
                    test_item = WIFI_EM_BW;
                } else {
                    printc("ERROR: WIFI EM CMD Error, %d\r\n", test_item);
                    ret = 1;
                    break;
                }
                printc("WIFI TEST ITEM CMD %d\r\n", test_item);

                switch (test_item) {
                    case WIFI_EM_ENTER :
                        printc("RFTool_WiFi_EnterTestMode\r\n");
                        RFTool_WiFi_EnterTestMode();
                        ret = 0;
                        break;/*endof WIFI_EM_ENTER*/

                    case WIFI_EM_STOP :
                        printc("RFTool_WiFi_Stop\r\n");
                        RFTool_WiFi_Stop();
                        ret = 0;
                        break;/*endof WIFI_EM_STOP*/

                    case WIFI_EM_TX   :
                        param2 = strtok(param1, ",");//AT+EM=TX
                        param2 = strtok(NULL, ",");//CT
                        if (NULL == param2) {
                            //To reminder there is a ',' after CT/CS/PT ATCI input
                            printc("ERROR: WIFI TX Mode Input Error\r\n");
                            ret = 1;
                            break;
                        }

                        if (0 == strcmp(param2, "CT")) {
                            tx_mode = WIFI_TX_CT;
                        } else if (0 == strcmp(param2, "CS")) {
                            tx_mode = WIFI_TX_CS;
                        } else if (0 == strcmp(param2, "LF")) {
                            tx_mode = WIFI_TX_LF;
                        } else if (0 == strcmp(param2, "PT")) {
                            tx_mode = WIFI_TX_PT;
                        } else {
                            printc("ERROR: WIFI TX Mode Unknown\r\n");
                            ret = 1;
                            break;
                        }

                        //Get the input tx parameter
                        param2 = param2 + 3;  //To jump over "CT,"  then the point is at first param
                        if (tx_mode < WIFI_TX_PT) {
                            param_num = sscanf(param2, "%u,%u,%u,%u,%u,%u,%u", &tx_param.ch_freq, (unsigned int *)&tx_param.tx_rate, (unsigned int *)&tx_param.txAnt,
                                               (unsigned int *)&tx_param.tx_gain_dac, (unsigned int *)&tx_param.txBw, (unsigned int *)&tx_param.txGI, (unsigned int *)&tx_param.txMode);
                            if (param_num != 7) {
                                printc("ERROR: Tx Param Input Not Match, %d\r\n", param_num);
                                ret = 1;
                                break;
                            }
                            printc("Tx Input:%u,%u,%u,%u,%u,%u,%u\r\n", tx_param.ch_freq, tx_param.tx_rate, tx_param.txAnt,
                                   tx_param.tx_gain_dac, tx_param.txBw, tx_param.txGI, tx_param.txMode);
                        } else {
                            param_num = sscanf(param2, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u", &tx_pkt_param.ch_freq, (unsigned int *)&tx_pkt_param.tx_rate, (unsigned int *)&tx_pkt_param.txAnt,
                                               (unsigned int *)&tx_pkt_param.tx_gain_dac, (unsigned int *)&tx_pkt_param.txBw, (unsigned int *)&tx_pkt_param.txGI, (unsigned int *)&tx_pkt_param.txMode,
                                               &tx_pkt_param.pktCount, &tx_pkt_param.pktInterval, &tx_pkt_param.pktLength,
                                               (unsigned int *)&tx_pkt_param.pattern, (unsigned int *)&tx_pkt_param.is_short_preamble);
                            if (param_num != 12) {
                                printc("ERROR: Tx Param Input Not Match, %d\r\n", param_num);
                                ret = 1;
                                break;
                            }
                            printc("Tx Input:%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\r\n", tx_pkt_param.ch_freq, tx_pkt_param.tx_rate, tx_pkt_param.txAnt,
                                   tx_pkt_param.tx_gain_dac, tx_pkt_param.txBw, tx_pkt_param.txGI, tx_pkt_param.txMode,
                                   tx_pkt_param.pktCount, tx_pkt_param.pktInterval, tx_pkt_param.pktLength,
                                   tx_pkt_param.pattern, tx_pkt_param.is_short_preamble);
                            for (uint8_t i = 0 ; i < 24; i++) {
                                tx_pkt_param.mac_header[i] = 0;    //For Test
                            }
                        }
                        country_code[0] = 'U';
                        country_code[1] = 'S';
                        RFTool_WiFi_Stop();
                        kal_sleep_task(100);
                        switch (tx_mode) {
                            case WIFI_TX_CT:
                                printc("RFTool_WiFi_ContTx\r\n");
                                RFTool_WiFi_ContTx(country_code, tx_param);
                                ret = 0;
                                break;
                            case WIFI_TX_CS:
                                printc("RFTool_WiFi_CarrierSupp\r\n");
                                RFTool_WiFi_CarrierSupp(country_code, tx_param);
                                ret = 0;
                                break;
                            case WIFI_TX_LF:
                                printc("RFTool_WiFi_LocalFreq\r\n");
                                RFTool_WiFi_LocalFreq(country_code, tx_param);
                                ret = 0;
                                break;
                            case WIFI_TX_PT:
                                printc("RFTool_WiFi_PktTx\r\n");
                                RFTool_WiFi_PktTx(country_code, tx_pkt_param);
                                ret = 0;
                                break;
                            default:
                                //will have no chance go here.
                                break;
                        }
                        break;/*endof WIFI_EM_TX*/

                    case WIFI_EM_RX   :
                        //AT+EM=RX,100
                        param2 = strtok(param1, ",");//AT+EM=RX
                        param2 = strtok(NULL, ",");//<channel>
                        if (NULL == param2) {
                            printc("ERROR: freq is NULL\r\n");
                            ret = 1;
                            break;
                        }
                        ch_freq = atoi(param2);
                        if (ch_freq < 2400000) {
                            //below 2.4G
                            printc("ERROR: freq is below 2400000Khz\r\n");
                            ret = 1;
                            break;
                        }
                        printc("RFTool_WiFi_PktRx,ch:%d\r\n", ch_freq);
                        RFTool_WiFi_Stop();
                        kal_sleep_task(100);
                        RFTool_WiFi_PktRx(ch_freq, wifi_pkt_rx_callback);
                        ret = 0;
                        break;/*endof WIFI_EM_RX*/

                    case WIFI_EM_CAL   :
                        //AT+WIFIEM=CAL,1,1,ab
                        param2 = strtok(param1, ",");//AT+WIFIEM=CAL
                        param2 = strtok(NULL, ",");
                        if (NULL == param2) {
                            printc("ERROR: Read/Write is Unknown\r\n");
                            ret = 1;
                            break;
                        }
                        read_write = atoi(param2);

                        param2 = strtok(NULL, ",");
                        cal_item = atoi(param2);

                        param2 = strtok(NULL, ",");
                        cal_data = atoi(param2);

                        if (read_write == 1) {
                            printc("Write CAL NVDM\r\n");
                            ret = wifi_set_cal_data(cal_item, &cal_data);
                            printc("write:%d, status=%d.\r\n", cal_data, ret);
                        } else {
                            printc("Read CAL NVDM\r\n");
                            cal_data_len = sizeof(cal_data);
                            ret = wifi_get_cal_data(cal_item, &cal_data, cal_data_len);
                            ATCI_PRINT(ATCI_PRINT_COMMON, "READ:%d\r\n", cal_data);
                        }
                        break;/*endof WIFI_EM_RX*/

                    case WIFI_EM_BW   :
                        //AT+EM=BW,0
                        param2 = strtok(param1, ",");//AT+EM=BW
                        param2 = strtok(NULL, ",");//<bandwidth>
                        if (NULL == param2) {
                            printc("ERROR: BW is NULL\r\n");
                            ret = 1;
                            break;
                        }
                        bandwidth = atoi(param2);
                        if (bandwidth > 4) {
                            printc("ERROR: BW should be 0/1/2/3\r\n");
                            ret = 1;
                            break;
                        }
                        printc("RFTool_WiFi_BW,BW:%d\r\n", bandwidth);
                        //RFTool_WiFi_Stop();
                        //kal_sleep_task(100);
                        RFTool_WiFi_BW(bandwidth);
                        ret = 0;
                        break;/*endof WIFI_EM_BW*/

                    default:
                        printc("ERROR: WIFI EM CMD Error\r\n");
                        ret = 1;
                        break;/*defaultof switch(test_item)*/
                }/*endof switch(test_item)*/

            } else { /*else  of if(NULL == param)*/
                printc("ERROR: Input is NULL\r\n");
                ret = 1;
            }

            param1 = NULL;
            param2 = NULL;
            break;/*endof ATCI_CMD_MODE_EXECUTION*/

        default :
            printc("ERROR: Invalide parse_cmd->mode\r\n");
            ret = 1;
            break;
    }

    printc("Command Execute Finish\r\n");

    if (0 == ret) {
        ATCI_PRINT(ATCI_PRINT_COMMON, "PASS\r\n");
        return ATCI_STATUS_OK;
    } else {
        ATCI_PRINT(ATCI_PRINT_COMMON, "FAIL\r\n");
        return ATCI_STATUS_ERROR;
    }
}

/*
***************SYS ATCI CMD******************
AT+WIFI=VER
AT+WIFI=START
AT+WIFI=STOP
AT+WIFI=OPMODE,<op>  (op=0 is get)
AT+WIFI=MAC,<op>         (op=0 is get)
*
****************STA ATCI CMD******************
AT+WIFI=SCAN
AT+WIFI=CONNECT,<ssid>,<authMode>,<encryption>,<key>
AT+WIFI=CONNECTIONINFO
AT+WIFI=PSMODE,<op>,<pm>    (op=1 is set)
AT+WIFI=DISCONNECT
*
****************MFG ATCI CMD******************
*
*/
atci_status_t atci_cmd_hdlr_wifi_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    static int test_param1 = 0;
    atci_response_t resonse = {{0}};
    char *param = NULL;
    int32_t ret = 1;            //default is error status
    uint8_t operation = 0;      //0 is get, 1 is set
    kal_int32 opmode = 0;         //opmode = 0 means STA
    uint8_t mac_addr[6] = {0};  //used for MAC Get
    uint32_t buff_pos = 0;      //ATCI response buffer write position mark
    uint8_t ssid[32] = {0};     //connect AP ssid
    WLM_AUTH_MODE authMode = 0; //connect AP auth mode
    WLM_ENCRYPTION encryption = 0;  //connect AP encrypt mode
    uint8_t key[64 + 1] = {0};  //connect AP password
    uint8_t wep_key_index = 0;
    PARAM_POWER_MODE ps_mode = 0;        //power saving mode
    uint8_t log_onoff = 0;
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t nvdm_ret = 0;
#endif
    kal_uint32 port = 0;
    char mac_buff[17] = {0};
    char info_buff[10] = {0};


    printc("atci_cmd_hdlr_wifi_cmd\n");

    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFI=?
            printc("AT Test OK.\n");
            strcpy((char *)resonse.response_buf, "+WIFI:OK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFI?
            printc("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "+WIFI:%d\r\n", test_param1);
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFI
            printc("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ACTIVE"
            strcpy((char *)resonse.response_buf, "ACTIVE\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFI= xxxx,xx,xx,xx
            printc("AT Executing...\r\n");
            param = strtok(parse_cmd->string_ptr, "\n\r");

            if (NULL != param) {
                /***************SYS ATCI CMD******************/
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=LOG,") != NULL) {
                    printc("Switch WIFI Log\n");
                    param = strtok(param, ",");//AT+WIFI=LOG,0/1
                    param = strtok(NULL, ",");//<0/1>
                    if (NULL == param) {
                        printc("ERROR: Input param is NULL\r\n");
                        ret = 1;
                        break;
                    }
                    log_onoff = (uint8_t)atoi(param);
                    printc_switch(log_onoff);
                    sprintf((char *)resonse.response_buf, "SUCCEED\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 0;
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=VER") != NULL) {
                    printc("Get Version\n");
                    printf("VERSION:%s\n", wlan_get_version());
                    //strcpy((char *)resonse.response_buf, wlan_get_version());
                    sprintf((char *)resonse.response_buf, "SUCCEED:Version WNDRV-HOSTAPD-SUPC:%s\n\r", wlan_get_version());
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 0;
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=START") != NULL) {
                    printc("WIFI START\n");
                    ret = wifi_config_set_wifi_start();
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED::WiFi Start\r\n");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:WiFi Start\r\n");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }

                } else {
                    if (NULL == __g_virtual_hdl) {
                        //WIFI not ready, should not input wifi ATCI
                        sprintf((char *)resonse.response_buf, "FAILD:WIFI INIT NOT READY!\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
                        atci_send_response(&resonse);
                        param = NULL;
                        break;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=STOP") != NULL) {
                    printc("WIFI STOP\n");
                    ret = wifi_config_set_wifi_stop();
                    if (ret < 0) {
                        strcpy((char *)resonse.response_buf, "FAILED:WiFi Stop\r\n");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
                    } else {
                        strcpy((char *)resonse.response_buf, "SUCCEED:WiFi Stop\r\n");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=OPMODE") != NULL) {
                    printc("AT+WIFI=OPMODE,<op>  (op=0 is get)\n");
                    param = strstr((char *)parse_cmd->string_ptr, ",");
                    param = param + 1;
                    operation = (uint8_t)atoi(param);

                    if (0 == operation) {
                        printc("Get Opmode.\n");
                        opmode = wlan_get_mode();
                        //printf("OPMODE:%d.\n",opmode);
                        sprintf((char *)resonse.response_buf, "SUCCEED:Get OPMODE:%d\n\r", opmode);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;

                    } else {
                        printc("WARNING:Set Opmode NOT SUPPORT NOW!\n");
                        sprintf((char *)resonse.response_buf, "FAILED:Set OPMODE Not Support\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=MAC") != NULL) {
                    printc("AT+WIFI=MAC,<op>  (op=0 is get)\n");
                    param = strstr((char *)parse_cmd->string_ptr, ",");
                    param = param + 1;
                    operation = (uint8_t)atoi(param);

                    if (0 == operation) {
                        printc("Get MAC.\n");
                        //wlan_get_mac_addr(mac_addr);
                        ret = wifi_config_get_mac_address(0, mac_addr);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Get MAC");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Get MAC: %02x:%02x:%02x:%02x:%02x:%02x\n\r",  PRINT_MAC(mac_addr));
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }

                        //printf("MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(mac_addr));


                    } else {
                        param = strstr(param, ",");
                        param = param + 1;
                        sscanf(param, "%u,%s", &port, mac_buff);
                        char *group_name = (WIFI_PORT_AP == port) ? "AP" : "STA";
#ifdef MTK_NVDM_ENABLE
                        nvdm_ret = nvdm_write_data_item(group_name, "MacAddr", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)mac_buff, strlen(mac_buff));
                        if (nvdm_ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Write NVDM MAC Status=%d,Port=%u,Mac=%s\n\r", nvdm_ret, port, mac_buff);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Write Port=%u, Mac=%s, Please Reboot.\n\r", port, mac_buff);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                        printc("nvdm_write_data_item Status=%d,Port=%u,Mac=%s\n", nvdm_ret, port, mac_buff);
#else
                        printc("nvdm not enable\n");
                        sprintf((char *)resonse.response_buf, "FAILED: NVDM not enable\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
#endif
                    }
                }

                /****************STA ATCI CMD******************/
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=SCAN") != NULL) {
                    printc("WIFI CAN\n");
                    param = strstr(param, ",");
                    if (NULL == param) {
                        //AT+EM=SCAN
                        printc("Use the Default Scan Max AP num = 10\r\n");
                    } else {
                        //AT+EM=SCAN,max_ap_num
                        param = param + 1;
                        scan_max_ap = atoi(param) != 0 ? atoi(param) : scan_max_ap;
                        printc("Set Scan Max AP num = %d\r\n", scan_max_ap);
                    }

                    WLAN_SCAN_RESULT *g_scan_result = NULL;
                    g_scan_result = MEM_MALLOC((sizeof(WLAN_SCAN_RESULT) - sizeof(WLAN_BSS_INFO)) + scan_max_ap * sizeof(WLAN_BSS_INFO));
                    wifi_connection_register_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, wifi_scan_callback);
                    wifi_connection_scan_init(g_scan_result, scan_max_ap);
                    wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
                    wifi_connection_scan_deinit();
                    wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, wifi_scan_callback);
                    MEM_FREE(g_scan_result);
                    g_scan_result = NULL;
                    sprintf((char *)resonse.response_buf, "\r\nScan Done\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 0;
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=CONNECT,") != NULL) {
                    printc("WIFI CONNECT\n");
                    param = strstr((char *)parse_cmd->string_ptr, ",");
                    param = param + 1;

                    sscanf(param, "%[^,],%[^,],%[^,],%[^,],%c", ssid, (char *)&authMode, (char *)&encryption, key, &wep_key_index);
                    authMode = atoi((char *)&authMode);
                    encryption = atoi((char *)&encryption);
                    wep_key_index = atoi((char *)&wep_key_index);
                    printc("ssid:%s auth:%d encry:%d key:%s keyindex:%d", ssid, authMode, encryption, key, wep_key_index);

                    if(strlen((char *)ssid) > 32 || strlen((char *)key) > 64) {
                        ret = -1;
                    } else {
                        ret = wifi_config_set_sta_connection((char *)ssid, authMode, encryption, (char *)key, wep_key_index, NULL);
                    }

                    if (ret < 0) {
                        ret = 1;
                        sprintf((char *)resonse.response_buf, "FAILED:Connected FAIL\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    } else {
                        ret = 0;
                        sprintf((char *)resonse.response_buf, "SUCCEED:Connected PASS\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=CONNECTIONINFO") != NULL) {
                    printc("WIFI CONNECTIONINFO\n");

                    param = strstr(param, ",");
                    if (NULL == param) {
                        //AT+WIFI=CONNECTIONINFO
                        printc("Display all connection information\r\n");
                        uint32_t rate = 0;
                        wl_connection_info_t conn_info = {0};
                        ret = wlan_get_connection_info(&conn_info);
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "Get Info FAIL\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            buff_pos =  sprintf((char *)resonse.response_buf, "\n\r%-4s%-15s%-8s%-8s%-8s\n\r", "Ch", "SSID", "RSSI", "MSC", "Rate");
                            buff_pos += sprintf((char *)resonse.response_buf + buff_pos, "%-4d", (int)conn_info.channel);
                            buff_pos += sprintf((char *)resonse.response_buf + buff_pos, "%-15s", conn_info.Ssid);
                            buff_pos += sprintf((char *)resonse.response_buf + buff_pos, "%ddBm  ", (int)conn_info.Rssi);
                            rate = conn_info.Phy_rate & WLAN_NRATE_RATE_MASK;
                            if (conn_info.Phy_rate & WLAN_NRATE_MCS_INUSE) {//802.11n
                                buff_pos += sprintf((char *)resonse.response_buf + buff_pos, "%-8d", (int)rate);
                            } else {
                                buff_pos += sprintf((char *)resonse.response_buf + buff_pos, "%-d%s", (int)rate / 2, (rate % 2) ? ".5" : "");
                            }
                            buff_pos += sprintf((char *)resonse.response_buf + buff_pos, "\n\rSUCCEED\r\n");
                            //strcpy((char *)resonse.response_buf, "INFO GET Done!\n");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else {
                        param = param + 1;
                        sscanf(param, "%s", info_buff);
                        if (0 == strcmp(info_buff, "RSSI")) {
                            //AT+WIFI=CONNECTIONINFO,RSSI
                            int32_t rssi;
                            ret = wifi_connection_get_rssi(&rssi);
                            if (ret < 0) {
                                sprintf((char *)resonse.response_buf, "FAIL:Get RSSI FAIL.\r\n");
                                resonse.response_len = strlen((char *)resonse.response_buf);
                                ret = 1;
                            } else {
                                sprintf((char *)resonse.response_buf, "SUCCEED:Get RSSI PASS. RSSI is %ld.\r\n", rssi);
                                resonse.response_len = strlen((char *)resonse.response_buf);
                                ret = 0;
                            }
                        } else if (0 == strcmp(info_buff, "LINK")) {
                            //AT+WIFI=CONNECTIONINFO,LINK
                            uint8_t link_status;
                            ret = wifi_connection_get_link_status(&link_status);
                            if (ret < 0) {
                                sprintf((char *)resonse.response_buf, "FAIL:Get Link Status FAIL.\r\n");
                                resonse.response_len = strlen((char *)resonse.response_buf);
                                ret = 1;
                            } else {
                                sprintf((char *)resonse.response_buf, "SUCCEED:Get Link Status PASS. Link Status is %d.\r\n", link_status);
                                resonse.response_len = strlen((char *)resonse.response_buf);
                                ret = 0;
                            }
                        } else {
                            sprintf((char *)resonse.response_buf, "FAIL:Not Supported.\r\n");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        }
                    }

                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=PSMODE") != NULL) {
                    printc("AT+WIFI=PSMODE,<op>  (op=1 is set)\n");
                    param = strstr((char *)parse_cmd->string_ptr, ",");
                    param = param + 1;
                    operation = (uint8_t)atoi(param);

                    if (0 == operation) {
                        ret = wifi_config_get_sta_power_saving_mode(&ps_mode);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Get PSMODE FAIL\r\n");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Get PSMODE PASS. PSMODE is %d(%s).\r\n", ps_mode, look_for_ps_mode(ps_mode));
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else {
                        printc("Set PSMODE.\n");
                        param = strstr((char *)param,  ",");
                        param = param + 1;
                        ps_mode = (uint8_t)atoi(param);
                        ret = wifi_config_set_sta_power_saving_mode(ps_mode);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Set PSMODE FAIL\r\n");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Set PSMODE PASS\r\n");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=DISCONNECT") != NULL) {
                    printc("WLAN DISCONNECT\n");
                    ret = wifi_connection_disconnect_ap();
                    if (ret < 0) {
                        ret = 1;
                        sprintf((char *)resonse.response_buf, "FAILED:Disconnect Wlan Connection\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    } else {
                        ret = 0;
                        sprintf((char *)resonse.response_buf, "SUCCEED:Disconnect Wlan Connection\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
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
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
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

