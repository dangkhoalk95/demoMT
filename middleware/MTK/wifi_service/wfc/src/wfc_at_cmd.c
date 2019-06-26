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
#include "os.h"
#include "wfc_stub.h"
#include "wfc_sdio.h"
 
#include "lwip/pbuf.h"

void AtoH(char *src, char *dest, int destlen);

int32_t wfc_at_os_cmd(uint8_t *pcmd,  uint32_t cmdSize)
{
    printf( "heap:\n");
    printf( "\ttotal: %u\n", configTOTAL_HEAP_SIZE);
    printf( "\tfree:  %u\n", xPortGetFreeHeapSize());
    printf( "\tlow:   %u\n", xPortGetMinimumEverFreeHeapSize());
    printf( "\r\n\r\n");    
    
    return 0;
}

int32_t wfc_at_wifi_cmd(uint8_t *pcmd,  uint32_t cmdSize)
{
    char *param = NULL;
    char *key = NULL;    
    uint8_t port = 0;    // STA port
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    wifi_wep_key_t wep_key = {{{0}}};    
    int32_t ret = 0;
                
    //printf( "<AT>: Cmd(%s), Size(%lu)\r\n",pcmd, cmdSize);
 
    if (strstr((char *)pcmd, "AT+WIFISTA=connect") != NULL) {
        //LOGI("AT+WIFISTA=connect,s<ssid>,a<authMode>,e<encryption>,p<key>,<wep_key_index>\n");
        param = strtok((char *)pcmd, "s");
        param = strtok(NULL, ",");
        //LOGI("ssid=%s", param);
        ret = wifi_config_set_ssid(port, (uint8_t *)param, strlen(param));
        if (ret < 0) {
            printf("Set ssid failed\n");
            ret = 1;
        }
        param = strtok(NULL, ",");
        param = param + 1;
        auth = atoi(param);
        //LOGI("auth=%d", auth);
        param = strtok(NULL, ",");
        param = param + 1;
        encrypt = atoi(param);
        //LOGI("encrypt=%d", encrypt);
        ret = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
        if (ret < 0) {
            printf("Set auth mode failed\n");
            ret = 1;
        }
        if ((auth == 0) && (encrypt == 0)) {
            //LOGI("WEP encrypt\n\n");
            param = strtok(NULL, ",");
            param = param + 1;
            key = param;
            //LOGI("WEP key=%s", key);
            param = strtok(NULL, ",");
            param = param + 1;
            wep_key.wep_tx_key_index = atoi(param);
            //LOGI("WEP key index=%d", wep_key.wep_tx_key_index);
            if (strlen(key) == 10 || strlen(key) == 26) {
                wep_key.wep_key_length[wep_key.wep_tx_key_index] = strlen(key) / 2;
                AtoH((char *)key, (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
            } else if (strlen(key) == 5 || strlen(key) == 13) {
                os_memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], key, strlen(key));
                wep_key.wep_key_length[wep_key.wep_tx_key_index] = strlen(key);
            } else {
                printf("invalid length of value.\n");
                return 1;
            }

            ret = wifi_config_set_wep_key(port, &wep_key);

        } else if ((auth != 0) && (encrypt != 1)) {
            param = strtok(NULL, ",");
            param = param + 1;
            //LOGI("password=%s", param);
            ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param, strlen(param));
        }
        ret = wifi_config_reload_setting();
        if (ret < 0) {
            printf("Connect to network failed\n");
            ret = 1;
        }
    }          
    
    return ret;         
}

int32_t wfc_at_process(uint8_t *pcmd,  uint32_t cmdSize)
{
    int32_t ret = 0;
                
    printf( "<AT>: Cmd(%s), Size(%lu)\r\n",pcmd, cmdSize);
 
    if (strstr((char *)pcmd, "AT+WIFISTA") != NULL) 
    {
        ret = wfc_at_wifi_cmd(pcmd, cmdSize);
    }

    if (strstr((char *)pcmd, "AT+RMTOS") != NULL) {
        ret = wfc_at_os_cmd(pcmd, cmdSize);
    }

    return ret;         
}
