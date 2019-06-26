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
#include "stdio.h"
#include "host_os_utils.h"
#include "syslog.h"
#include "nvdm.h"
#include "hal_efuse.h"
#include "hal_gpt.h"
#include "wifi_host_api.h"
#include "wifi_host_os_api.h"
#include "wifi_host_private_api.h"
#include "wfcm_stub.h"
#include "wfcm_at_api.h"
#include "hal_sleep_manager.h"
#include "FreeRTOS.h"
#include "task.h"

/* WiFi Host task definition */
#define WIFI_HOST_TASK_NAME              "wifi_host"
#define WIFI_HOST_TASK_STACKSIZE         (1024*4) /*unit byte!*/
#define WIFI_HOST_TASK_PRIO              4 //TASK_PRIORITY_NORMAL

#define RSP_BUF_SIZE 1024
extern uint8_t rsp_buf[RSP_BUF_SIZE];
extern uint32_t rsp_size;
uint8_t g_tx_power_bin[TX_POWER_BIN_SIZE] = {0x76,0x82,0x76,0x86,0xC5,0xC5,0xC4,0xC3,0xC1,0xC0,0xC0,0xC2,0xC2,0xC0,0xC0,0x82,0x82,0x00,0x00,0x00,0x00,0x83,0x83,0x00};

int32_t wifi_host_config_init(wifi_host_system_config_t *pwifi_host_cfg)
{
    int32_t ret = -1;
    wh_set_init_para_cmd_para_t cmd_para = {{0}};
    wh_set_init_para_resp_para_t *rsp_para;

    uint8_t  wfc_is_ready = 0;
    uint8_t  rspbuf[32]={0};  /*WFC_CMD_WFC_READY, */
    uint32_t rspSize  = 0;

    cmd_para.api_id.api = WFC_API_INIT_PARA;
    cmd_para.api_id.set = true;

    if(pwifi_host_cfg == NULL) {
        ret = -1;
        return ret;
    }

    os_memcpy(&cmd_para.config, &(pwifi_host_cfg->wifi_config), sizeof(wifi_config_t));
    cmd_para.config_valid = true;

    os_memcpy(&cmd_para.config_ext, &(pwifi_host_cfg->wifi_config_ext), sizeof(wifi_config_ext_t));
    cmd_para.config_ext_valid = true;

    //build wifi default config
    wifi_host_get_default_config(&(cmd_para.sys_cfg));
    cmd_para.sys_cfg_valid = true;

    // Get Device WFC module Status
    wfc_is_ready = 0;
    while (!wfc_is_ready) {
        wfcm_to_slave(WFC_CMD_WFC_READY, NULL, 0, rspbuf, &rspSize, 0);
        wfc_is_ready = rspbuf[0];
        printf("-");
        hal_gpt_delay_ms(100);
    }
    printf("\r\nDevice WFC Ready(%d)\r\n", wfc_is_ready);

    //set TX power init
    wh_set_tx_power_cmd_para_t power_para;
    wh_set_tx_power_resp_para_t *power_resp;
    power_para.api_id.api = WFC_API_INIT_TX_POWER;
    power_para.api_id.set = true;
    os_memcpy(power_para.tx_power_bin,g_tx_power_bin, sizeof(g_tx_power_bin));
    if (WFC_RSP_OK != wfcm_to_slave(WFC_CMD_API, (uint8_t *)&power_para, sizeof(power_para), (uint8_t *)&rsp_buf, &rsp_size, 0)){
        printf("\r\n wfcm_to_slave fail\r\n");
    } else {
        power_resp = (wh_set_tx_power_resp_para_t *)rsp_buf;
        ret = power_resp->error_code;
    }

    // Set Device WiFi Init settings
    if (WFC_RSP_OK != wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0)){
        printf("\r\n wfcm_to_slave fail\r\n");
    } else {
        rsp_para = (wh_set_init_para_resp_para_t *)rsp_buf;
        ret = rsp_para->error_code;
    }
    
    return ret;
}

int32_t wifi_host_config_get_mac_address(uint8_t port, uint8_t *address)
{
    int32_t ret = 0;
    wh_get_mac_addr_cmd_para_t cmd_para;
    wh_get_mac_addr_resp_para_t *rsp_para;

    cmd_para.api_id.api = WFC_API_MAC_ADDR;
    cmd_para.api_id.set = false;
    cmd_para.port = port;

    if(WFC_RSP_OK != wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0)){
        printf("wfcm_to_slave fail in mac opt\r\n");
        ret = -1;
    } else {
        rsp_para = (wh_get_mac_addr_resp_para_t *)rsp_buf;
        ret = rsp_para->error_code;
        os_memcpy(address, rsp_para->mac_addr, WIFI_MAC_ADDRESS_LENGTH);

        printf("GET MAC: 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,\n\r",
            address[0],address[1],address[2],
            address[3],address[4],address[5]);
    }
    return ret;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
#define WIFI_MAX_SLEEP_HANDLE  32   // This define value should equal to MAX_SLEEP_HANDLE in hal_sleep_driver.h
uint8_t locks[WIFI_MAX_SLEEP_HANDLE];
#endif
uint8_t wifi_set_sleep_handle(const char *handle_name)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_set_sleep_handle(handle_name);
#else
    return 0xff;
#endif
}

int32_t wifi_lock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    locks[handle_index] = 1;
    return hal_sleep_manager_lock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_unlock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    locks[handle_index] = 0;
    return hal_sleep_manager_unlock_sleep(handle_index);
#else
    return -1;
#endif
}

static uint8_t wifi_init_status = false;
void wifi_set_init_status(uint8_t status)
{
    wifi_init_status = status;
    return;
}

uint8_t wifi_get_init_status(void)
{
    return wifi_init_status;
}
void wifi_init_done_handler(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    wifi_set_init_status(true);
    printf("Wi-Fi init done.\r\n");
    return;
}

static void wifi_host_main_task(void *param)
{
    LOG_I(common, "enter wifi_host_main_task!!\n\r");

    /*The host will send wifi configuration to wifi chip for wifi initialization*/
    wifi_host_system_config_t wifi_host_sys_cfg;   

    os_memset(&wifi_host_sys_cfg, 0 , sizeof(wifi_host_system_config_t));

    wifi_host_sys_cfg.wifi_config.opmode = WIFI_MODE_STA_ONLY;
    os_strcpy((char *)wifi_host_sys_cfg.wifi_config.sta_config.ssid,     (const char *)"MTK_STA");
    os_strcpy((char *)wifi_host_sys_cfg.wifi_config.sta_config.password, (const char *)"12345678");
    wifi_host_sys_cfg.wifi_config.sta_config.ssid_length     =  os_strlen((const char *)wifi_host_sys_cfg.wifi_config.sta_config.ssid);
    wifi_host_sys_cfg.wifi_config.sta_config.password_length =  os_strlen((const char *)wifi_host_sys_cfg.wifi_config.sta_config.password);
    wifi_host_sys_cfg.wifi_config_ext.sta_auto_connect_present = 1;
    wifi_host_sys_cfg.wifi_config_ext.sta_auto_connect = 0;
    wifi_host_sys_cfg.wifi_config_ext.sta_power_save_mode_present = 1;
    wifi_host_sys_cfg.wifi_config_ext.sta_power_save_mode = 0;

    /*Wait wifi chip's WFC module init done, and set wifi initial setting to wifi chip from Host*/
    wifi_host_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE, (wifi_event_handler_t) wifi_init_done_handler);
    wifi_host_config_init(&wifi_host_sys_cfg);

    vTaskDelete(NULL);
}

void wifi_host_main_init(void)
{
    /* create task for WiFi Host Demo*/
    wifi_os_task_create(wifi_host_main_task, 
                        WIFI_HOST_TASK_NAME, 
                        WIFI_HOST_TASK_STACKSIZE,
                        NULL, WIFI_HOST_TASK_PRIO, NULL);
}
