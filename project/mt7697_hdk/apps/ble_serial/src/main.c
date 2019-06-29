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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt7687.h"
#include "system_mt7687.h"

/* wifi related header */
#include "wifi_api.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"

/* ble related header */
#include "bt_init.h"
#include "bt_gatts.h"
#include "bt_gap_le.h"
#include "bt_uuid.h"
#include "bt_debug.h"
#include "bt_type.h"
#include "bt_notify.h"

/* app-wise config */
#include "project_config.h"
#include "sys_init.h"
#include "task_def.h"
#include "flash_map.h"

/* hal */
#include "nvdm.h"

/* ---------------------------------------------------------------------------- */

/* Create the log control block as user wishes. Here we use 'app' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(app, PRINT_LEVEL_INFO);

/* ---------------------------------------------------------------------------- */
static void app_btnotify_init(void);

/* ---------------------------------------------------------------------------- */

#define APP_BLE_MAX_INTERVAL          0x00C0    /*The range is from 0x0020 to 0x4000.*/
#define APP_BLE_MIN_INTERVAL          0x00C0    /*The range is from 0x0020 to 0x4000.*/
#define APP_BLE_CHANNEL_NUM           7
#define APP_BLE_FILTER_POLICY         0
#define APP_BLE_AD_FLAG_LEN           2
#define APP_BLE_AD_UUID_LEN           3


static void app_start_advertising(void)
{
    bt_hci_cmd_le_set_advertising_enable_t enable;
    bt_hci_cmd_le_set_advertising_parameters_t adv_param = {
            .advertising_interval_min = APP_BLE_MIN_INTERVAL,
            .advertising_interval_max = APP_BLE_MAX_INTERVAL,
            .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
            .own_address_type = BT_ADDR_RANDOM,
            .advertising_channel_map = APP_BLE_CHANNEL_NUM,
            .advertising_filter_policy = APP_BLE_FILTER_POLICY
        };
    bt_hci_cmd_le_set_advertising_data_t adv_data;

    adv_data.advertising_data[0] = APP_BLE_AD_FLAG_LEN;
    adv_data.advertising_data[1] = BT_GAP_LE_AD_TYPE_FLAG;
    adv_data.advertising_data[2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;

    adv_data.advertising_data[3] = APP_BLE_AD_UUID_LEN;
    adv_data.advertising_data[4] = BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE;
    adv_data.advertising_data[5] = APP_BLE_SERVICE_UUID & 0x00FF;
    adv_data.advertising_data[6] = (APP_BLE_SERVICE_UUID & 0xFF00)>>8;

    adv_data.advertising_data[7] = 1+strlen(APP_BLE_DEVICE_NAME);
    adv_data.advertising_data[8] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    memcpy(adv_data.advertising_data+9, APP_BLE_DEVICE_NAME, strlen(APP_BLE_DEVICE_NAME));

    adv_data.advertising_data_length = 9 + strlen(APP_BLE_DEVICE_NAME);

    enable.advertising_enable = BT_HCI_ENABLE;
    bt_gap_le_set_advertising(&enable, &adv_param, &adv_data, NULL);
}

extern bt_bd_addr_t local_public_addr;

// called by bt_app_event_callback@bt_common.c
bt_status_t app_bt_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_I(app, "---> bt_event_callback(0x%08X,%d)", msg, status);

    switch(msg)
    {
    case BT_POWER_ON_CNF:
        LOG_I(app, "[BT_POWER_ON_CNF](%d)", status);

        // set random address before advertising
        LOG_I(app, "bt_gap_le_set_random_address()");    
        bt_gap_le_set_random_address((bt_bd_addr_ptr_t)local_public_addr);

        app_btnotify_init();
        break;

    case BT_GAP_LE_SET_RANDOM_ADDRESS_CNF: 
        LOG_I(app, "[BT_GAP_LE_SET_RANDOM_ADDRESS_CNF](%d)", status);

        // start advertising
        app_start_advertising();
        break;

    case BT_GAP_LE_SET_ADVERTISING_CNF:
        LOG_I(app, "[BT_GAP_LE_SET_ADVERTISING_CNF](%d)", status);
        break;

    case BT_GAP_LE_DISCONNECT_IND:
        LOG_I(app, "[BT_GAP_LE_DISCONNECT_IND](%d)", status);

        // start advertising
        app_start_advertising();
        break;

    case BT_GAP_LE_CONNECT_IND:
        LOG_I(app, "[BT_GAP_LE_CONNECT_IND](%d)", status);

        bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buff;
        LOG_I(app, "-> connection handle = 0x%04x, role = %s", connection_ind->connection_handle, (connection_ind->role == BT_ROLE_MASTER)? "master" : "slave");

        LOG_I(app, "************************");
        LOG_I(app, "BLE connected!!");
        LOG_I(app, "************************");
        break;
    }

    LOG_I(app, "<--- bt_event_callback(0x%08X,%d)", msg, status);
    return BT_STATUS_SUCCESS;
}

/* ---------------------------------------------------------------------------- */

/* command, hardcoded at mobile-side application */
#define SERIAL_EXTCMD_UPDATE_BIN_SENDER           "myserial"
#define SERIAL_EXTCMD_UPDATE_BIN_RECEIVER         "myserial"

static bt_bd_addr_t g_remote_bt_addr;

static void app_btnotify_msg_hdlr(void *data)
{
    bt_notify_callback_data_t *p_data = (bt_notify_callback_data_t *)data;

    LOG_I(app, "> app_btnotify_msg_hdlr(evt_id=%d)", p_data->evt_id);

    switch (p_data->evt_id) 
    {
    case BT_NOTIFY_EVENT_CONNECTION:
        LOG_I(app, "  >BT_NOTIFY_EVENT_CONNECTION");
        memcpy(g_remote_bt_addr, p_data->bt_addr, 6);
        break;

    case BT_NOTIFY_EVENT_DISCONNECTION:
        LOG_I(app, "  >BT_NOTIFY_EVENT_DISCONNECTION");
        memset(g_remote_bt_addr, 0, 6);
        break;

    case BT_NOTIFY_EVENT_SEND_IND:
        /*send  new/the rest data flow start*/
        LOG_I(app, "  >BT_NOTIFY_EVENT_SEND_IND");
        break;

    case BT_NOTIFY_EVENT_DATA_RECEIVED:
        /*receive data*/
        LOG_I(app, "  >BT_NOTIFY_EVENT_DATA_RECEIVED(code=%d,len=%d)", p_data->event_data.error_code, p_data->event_data.length);
        if (strcmp(p_data->event_data.sender_id, SERIAL_EXTCMD_UPDATE_BIN_SENDER) ||
            strcmp(p_data->event_data.receiver_id, SERIAL_EXTCMD_UPDATE_BIN_RECEIVER))
        {
            LOG_E(app, "sender (%s) or receiver (%s) error", p_data->event_data.sender_id, p_data->event_data.receiver_id);
            break;
        }

        char buf[65];
        int32_t len = p_data->event_data.length;
        if(len > 64)
            len = 64;
        memcpy(buf, p_data->event_data.data, len);
        buf[len] = 0;

        LOG_I(app, "Receive data(%s)", buf);

        // reverse content
        int32_t i,j;
        for(i=0,j=len-1;i<j;i++,j--)
        {
            char t;
            t = buf[i];
            buf[i] = buf[j];
            buf[j] = t;
        }

        // send it back
        static char fullcmd[128];
        sprintf(fullcmd, "%s %s %d %d %s",
                SERIAL_EXTCMD_UPDATE_BIN_SENDER,
                SERIAL_EXTCMD_UPDATE_BIN_RECEIVER,
                0,
                len,
                buf);

        int32_t written = bt_notify_send_data(&g_remote_bt_addr, fullcmd, strlen(fullcmd), true);
        if (written != strlen(fullcmd)) {
            LOG_E(app, "Send data unfinished, plan to write(%d), actual written(%d)", strlen(fullcmd), written);
        }
        break;
    default:
        break;
    }

    LOG_I(app, "< app_btnotify_msg_hdlr(evt_id=%d)", p_data->evt_id);
}

static void app_btnotify_init(void)
{
    bt_notify_init(0);
    bt_notify_register_callback(NULL, SERIAL_EXTCMD_UPDATE_BIN_SENDER, app_btnotify_msg_hdlr);    

    log_config_print_switch(NOTIFY, DEBUG_LOG_OFF);
    log_config_print_switch(NOTIFY_SRV, DEBUG_LOG_OFF);
    log_config_print_switch(DOGP, DEBUG_LOG_OFF);
    log_config_print_switch(DOGP_ADP, DEBUG_LOG_OFF);
    log_config_print_switch(DOGP_CM, DEBUG_LOG_OFF);
}

/* ---------------------------------------------------------------------------- */

extern void bt_common_init(void);

static int32_t _wifi_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;

    LOG_I(app, "wifi event: %d", event);

    switch(event)
    {
    case WIFI_EVENT_IOT_INIT_COMPLETE:
        LOG_I(app, "wifi inited complete");
        break;
    }

    return 1;
}

/**
* @brief       Main function
* @param[in]   None.
* @return      None.
*/
int main(void)
{
    /* Do system initialization, eg: hardware, nvdm and random seed. */
    system_init();

    /* system log initialization.
     * This is the simplest way to initialize system log, that just inputs three NULLs
     * as input arguments. User can use advanved feature of system log along with NVDM.
     * For more details, please refer to the log dev guide under /doc folder or projects
     * under project/mtxxxx_hdk/apps/.
     */
    log_init(NULL, NULL, NULL);

    LOG_I(app, "main()");

    /* Wi-Fi must be initialized for BLE start-up */
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE , _wifi_event_handler);

    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
    wifi_init(&config, NULL);

    lwip_tcpip_config_t tcpip_config = {{0}, {0}, {0}, {0}, {0}, {0}};
    lwip_tcpip_init(&tcpip_config, WIFI_MODE_STA_ONLY);

    bt_create_task();
    bt_common_init();
	
	/* As for generic HAL init APIs like: hal_uart_init(), hal_gpio_init() and hal_spi_master_init() etc,
     * user can call them when they need, which means user can call them here or in user task at runtime.
     */

    /* Create a user task for demo when and how to use wifi config API to change WiFI settings,
    Most WiFi APIs must be called in task scheduler, the system will work wrong if called in main(),
    For which API must be called in task, please refer to wifi_api.h or WiFi API reference.
    xTaskCreate(user_wifi_app_entry,
                UNIFY_USR_DEMO_TASK_NAME,
                UNIFY_USR_DEMO_TASK_STACKSIZE / 4,
                NULL, UNIFY_USR_DEMO_TASK_PRIO, NULL);
    user_wifi_app_entry is user's task entry function, which may be defined in another C file to do application job.
    UNIFY_USR_DEMO_TASK_NAME, UNIFY_USR_DEMO_TASK_STACKSIZE and UNIFY_USR_DEMO_TASK_PRIO should be defined
    in task_def.h. User needs to refer to example in task_def.h, then makes own task MACROs defined.
    */

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}

