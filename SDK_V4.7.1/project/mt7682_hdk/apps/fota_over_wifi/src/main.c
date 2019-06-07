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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sys_init.h"
#include "wifi_nvdm_config.h"
#include "wifi_lwip_helper.h"
#if defined(MTK_MINICLI_ENABLE)
#include "cli_def.h"
#endif

#include "bsp_gpio_ept_config.h"
#include "hal_sleep_manager.h"

#include "connsys_profile.h"
#include "wifi_api.h"

#include "hal.h"

#ifdef MTK_ATCI_ENABLE
#include "atci.h"
#include "at_command_wifi.h"


/**
 * @brief This function is a task main function for processing the data handled by ATCI module.
 * @param[in] param is the task main function paramter.
 * @return    None
 */
static void atci_def_task(void *param)
{

    LOG_I(common, "enter atci_def_task!!\n\r");
    while (1) {
        atci_processing();
    }
}

#endif

int32_t wifi_station_port_secure_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
int32_t wifi_scan_complete_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE

#ifdef HAL_WDT_MODULE_ENABLED

void wdt_timeout_handle(hal_wdt_reset_status_t wdt_reset_status)
{
    printf("%s: stattus:%u\r\n", __FUNCTION__, (unsigned int)wdt_reset_status);
    /* assert 0 to trigger exception hanling flow */
    configASSERT(0);
}

static void wdt_init(void)
{
    hal_wdt_config_t wdt_init;
    wdt_init.mode = HAL_WDT_MODE_INTERRUPT;
    wdt_init.seconds = 15;
    hal_wdt_init(&wdt_init);
    hal_wdt_register_callback(wdt_timeout_handle);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
}
#endif
#endif


/* for idle task feed wdt (DO NOT enter sleep mode)*/
void vApplicationIdleHook(void)
{

#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
#ifdef HAL_WDT_MODULE_ENABLED
 static uint32_t time1_start_count = 0;
 static uint32_t time1_end_count = 0;
 static uint32_t time1_count = 0;

 hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time1_end_count);
 hal_gpt_get_duration_count(time1_start_count, time1_end_count, &time1_count);
 if(time1_count > 32768) {
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);
 hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time1_start_count);
 time1_count = 0;
	 }


#endif
#endif
}

int32_t wifi_init_done_handler(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
    LOG_I(common, "WiFi Init Done: port = %d", payload[6]);
    return 1;
}

#ifdef MTK_USER_FAST_TX_ENABLE
#include "type_def.h"

#define DemoPktLen 64
extern UINT_8 DemoPkt[];

extern uint32_t g_FastTx_Channel;
extern PUINT_8 g_PktForSend;
extern UINT_32 g_PktLen;
static void fastTx_init(uint32_t channel, PUINT_8 pPktContent, UINT_32 PktLen)
{
    g_FastTx_Channel = channel;
    g_PktForSend = pPktContent;
    g_PktLen = PktLen;
}
#endif

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

#ifdef MTK_USER_FAST_TX_ENABLE
    /* Customize Packet Content and Length */
    fastTx_init(11, DemoPkt, DemoPktLen);
#endif

    /* Do system initialization, eg: hardware, nvdm, logging and random seed. */
    system_init();

    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
     * will be started , and adopt which settings for the specific mode while wifi initial process is running*/
    wifi_cfg_t wifi_config = {0};
    if (0 != wifi_config_init(&wifi_config)) {
        LOG_E(common, "wifi config init fail");
        return -1;
    }

    wifi_config_t config = {0};
    wifi_config_ext_t config_ext = {0};

    config.opmode = wifi_config.opmode;

    memcpy(config.sta_config.ssid, wifi_config.sta_ssid, 32);
    config.sta_config.ssid_length = wifi_config.sta_ssid_len;
    config.sta_config.bssid_present = 0;
    memcpy(config.sta_config.password, wifi_config.sta_wpa_psk, 64);
    config.sta_config.password_length = wifi_config.sta_wpa_psk_len;
    config_ext.sta_wep_key_index_present = 1;
    config_ext.sta_wep_key_index = wifi_config.sta_default_key_id;
    config_ext.sta_auto_connect_present = 1;
    config_ext.sta_auto_connect = 1;

    memcpy(config.ap_config.ssid, wifi_config.ap_ssid, 32);
    config.ap_config.ssid_length = wifi_config.ap_ssid_len;
    memcpy(config.ap_config.password, wifi_config.ap_wpa_psk, 64);
    config.ap_config.password_length = wifi_config.ap_wpa_psk_len;
    config.ap_config.auth_mode = (wifi_auth_mode_t)wifi_config.ap_auth_mode;
    config.ap_config.encrypt_type = (wifi_encrypt_type_t)wifi_config.ap_encryp_type;
    config.ap_config.channel = wifi_config.ap_channel;
    config.ap_config.bandwidth = wifi_config.ap_bw;
    config.ap_config.bandwidth_ext = WIFI_BANDWIDTH_EXT_40MHZ_UP;
    config_ext.ap_wep_key_index_present = 1;
    config_ext.ap_wep_key_index = wifi_config.ap_default_key_id;
    config_ext.ap_hidden_ssid_enable_present = 1;
    config_ext.ap_hidden_ssid_enable = wifi_config.ap_hide_ssid;
    config_ext.sta_power_save_mode = (wifi_power_saving_mode_t)wifi_config.sta_power_save_mode;

    /* Initialize wifi stack and register wifi init complete event handler,
     * notes:  the wifi initial process will be implemented and finished while system task scheduler is running,
     *            when it is done , the WIFI_EVENT_IOT_INIT_COMPLETE event will be triggered */
    wifi_init(&config, &config_ext);

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE, wifi_init_done_handler);

//Castro-
    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization*/
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);

#if defined(MTK_MINICLI_ENABLE)
    /* Initialize cli task to enable user input cli command from uart port.*/
    cli_def_create();
    cli_task_create();
#endif

#ifdef MTK_ATCI_ENABLE
    /* init ATCI module and set UART port */
    atci_init(HAL_UART_2);
    /* create task for ATCI */
    xTaskCreate(atci_def_task, ATCI_TASK_NAME, ATCI_TASK_STACKSIZE / ((uint32_t)sizeof(StackType_t)), NULL, ATCI_TASK_PRIO, NULL);
#if defined(MTK_WIFI_AT_COMMAND_ENABLE)
    wifi_atci_example_init();
#endif

#endif
    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
#ifdef HAL_WDT_MODULE_ENABLED
    wdt_init();
#endif
#endif
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

