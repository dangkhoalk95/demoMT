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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "bt_type.h"
#include "bt_system.h"

#include "mesh_app_util.h"
#include "bt_mesh.h"
#include "bt_mesh_debug.h"
#include "bt_mesh_flash.h"

#include "bt_mesh_model_configuration_server.h"
#include "bt_mesh_model_health_server.h"
#include "bt_mesh_model_generic_server.h"
#include "bt_mesh_model_generic_opcodes.h"
#include "bt_mesh_model_lighting_server.h"
#include "bt_mesh_model_configuration_server.h"
#include "bt_mesh_model_health_server.h"

#include "mesh_app_vendor_device_def.h"
#include "mesh_app_vendor_device_msg_handler.h"
#include "mesh_app_vendor_device_pwm.h"

#include "bsp_led.h"
#include "hal_sys.h"
#include "hal_cache.h"

#define RELAY_ENABLE    (1)

#define VENDOR_COMPANY_ID 0x000a
#define VENDOR_MODEL_ID1 0x002A
#define VENDOR_MODEL_ID2 0x002B
#define VENDOR_OPCODE_1 0xC1
#define VENDOR_OPCODE_2 0xC2
#define VENDOR_OPCODE_3 0xC3
#define VENDOR_OPCODE_4 0xC4

#define BSP_LED0_PIN 0
/******************************************************************************/
/* global variables                                                           */
/******************************************************************************/
uint16_t g_appkey_index = 0x123;
static mesh_onoff_server_model_t gOnOffServer;
static uint16_t g_model_handle_onoff_server = 0;
static uint16_t g_model_handle_config_server = 0;
static uint16_t g_model_handle_health_server = 0;
static uint16_t g_model_handle_vendor = 0;
static uint16_t g_model_handle_vendor1 = 0;
static uint16_t g_model_handle_ctl_server = 0;
static uint16_t g_model_handle_hsl_server = 0;

static uint8_t deviceUuid[BT_MESH_UUID_SIZE] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};
/*****************************************************************************/
/* main                                                                      */
/*****************************************************************************/

/*Application messages start*/
static void msg_generic_on_off_status(uint16_t model_handle, uint8_t presentOnOff, uint8_t targetOnOff,
                                      uint8_t remainingTime, const bt_mesh_access_message_rx_t *msg)
{
    uint8_t buffer[3];
    buffer[0] = presentOnOff;
    buffer[1] = targetOnOff;
    buffer[2] = remainingTime;

    bt_mesh_model_generic_on_off_status(model_handle, buffer, 3, msg);
}
/*Application messages end*/

/* Callback */
void _generic_onoff_publish_callback(uint16_t model_handle, void *arg)
{
    if (model_handle == g_model_handle_onoff_server) {
        VD_DEBUG_COLOR_PRINTF("GenericOnOffStatus(Publish)", 1);

        bt_mesh_access_message_tx_t msg;
        uint8_t buffer[1];
        buffer[0] = gOnOffServer.onOff;
        msg.opcode.opcode = BT_MESH_MODEL_GENERIC_ONOFF_STATUS;
        msg.opcode.company_id = MESH_MODEL_COMPANY_ID_NONE;
        msg.buffer = buffer;
        msg.length = 1;

        bt_mesh_status_t ret = bt_mesh_access_model_publish(model_handle, &msg);
        LOG_I(mesh_app, "ret = %d\n", ret);
    }
}

static void _generic_onoff_set_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool reliable)
{
    LOG_I(mesh_app, "[R]Generic_OnOffSet={src=0x%04x, onoff=0x%02x, tid=0x%02x, transTime=0x%02x, delay=0x%02x}, reliable=%s, ",
          msg->meta_data.src_addr, msg->buffer[0], msg->buffer[1], msg->buffer[2], msg->buffer[3],
          reliable ? "true" : "false");

    if (gOnOffServer.TID != msg->buffer[1]) { //new message

        LOG_I(mesh_app, "done.\n");

        if (gOnOffServer.onOff != msg->buffer[0]) {
            gOnOffServer.onOff = msg->buffer[0];
            _generic_onoff_publish_callback(model_handle, NULL);
        }

        if (gOnOffServer.onOff == 1) {
            LOG_I(mesh_app, "[Mesh]led is on\n");
            bsp_led_on(BSP_LED0_PIN);
        } else {
            LOG_I(mesh_app, "[Mesh]led is off\n");
            bsp_led_off(BSP_LED0_PIN);
        }

        gOnOffServer.TID = msg->buffer[1];
        gOnOffServer.transTime = msg->buffer[2];
        gOnOffServer.delay = msg->buffer[3];
    } else {
        LOG_I(mesh_app, "ignored. (not new message)\n");
    }

    if (reliable) {
        msg_generic_on_off_status(model_handle, gOnOffServer.onOff, msg->buffer[0], 0, msg);
    }
}

static void _generic_onoff_get_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg)
{
    LOG_I(mesh_app, "[R]Generic_OnOffGet={src=0x%04x}", msg->meta_data.src_addr);

    msg_generic_on_off_status(model_handle, gOnOffServer.onOff, gOnOffServer.onOff, gOnOffServer.transTime, msg);
}


static void _generic_server_msg_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg)
{
    switch (msg->opcode.opcode) {
        case BT_MESH_MODEL_GENERIC_ONOFF_GET: {
            VD_DEBUG_EVT_SIG(msg->opcode.opcode, 2);
            bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);
            _generic_onoff_get_handler(model_handle, msg, arg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONOFF_SET: {
            VD_DEBUG_EVT_SIG(msg->opcode.opcode, 2);
            bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);
            _generic_onoff_set_handler(model_handle, msg, arg, true);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONOFF_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_SIG(msg->opcode.opcode, 2);
            bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);
            _generic_onoff_set_handler(model_handle, msg, arg, false);
            break;
        }
    }
}

void msg_vendor_latency_status(uint16_t model_handle, bt_mesh_access_message_rx_t *msg, uint8_t recv_hops)
{
    VD_DEBUG_COLOR_PRINTF("VendorLatencyStatus", 1);
    bt_mesh_access_message_tx_t *reply;

    reply = (bt_mesh_access_message_tx_t *)bt_mesh_app_util_alloc(sizeof(bt_mesh_access_message_tx_t));
    reply->opcode.company_id = VENDOR_COMPANY_ID;
    reply->opcode.opcode = VENDOR_OPCODE_2;
    reply->length = 2;
    reply->buffer = bt_mesh_app_util_alloc(2);
    reply->buffer[0] = bt_mesh_config_get_default_ttl();
    reply->buffer[1] = recv_hops;

    LOG_I(mesh_app, "[T]Vendor_LatencyStatus={dst=0x%04x, initTTL=0x%02x, recvHops=0x%02x}\n",
          msg->meta_data.src_addr, reply->buffer[0], reply->buffer[1]);
    bt_mesh_access_model_reply(model_handle, msg, reply);

    bt_mesh_app_util_free(reply->buffer);
    bt_mesh_app_util_free((uint8_t *)reply);

}

static void msg_op2_send(uint16_t dst_addr, uint8_t recv_hops, uint8_t ttl)
{
    VD_DEBUG_COLOR_PRINTF("VendorLatencyStatus", 1);

    bt_mesh_tx_params_t param;
    param.dst.value = dst_addr;
    param.dst.type = bt_mesh_utils_get_addr_type(param.dst.value);
    param.src = bt_mesh_model_get_element_address(0);
    param.ttl = ttl;
    param.data_len = 5;

    uint8_t *payload = bt_mesh_app_util_alloc(param.data_len);

    payload[0] = VENDOR_OPCODE_2;
    payload[1] = VENDOR_COMPANY_ID & 0x00FF;
    payload[2] = (VENDOR_COMPANY_ID & 0xFF00) >> 8;
    payload[3] = ttl;
    payload[4] = recv_hops;

    param.data = payload;
    param.security.appidx = 0x123;
    param.security.netidx = BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX;
    bt_mesh_send_packet(&param);

    LOG_I(mesh_app, "[T]Vendor_LatencyStatus={dst=0x%04x, initTTL=0x%02x, recvHops=0x%02x}\n",
          dst_addr, ttl, recv_hops);
    bt_mesh_app_util_free(payload);
}

static void _vendorMsgHandler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg)
{
    VD_DEBUG_EVT_VENDOR(VENDOR_COMPANY_ID, msg->opcode.opcode, 2);
    bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);

    switch (msg->opcode.opcode) {
        case VENDOR_OPCODE_1: {

            LOG_I(mesh_app, "[R]Vendor_LatencyMeasure={src=0x%04x, initTTL=0x%02x}, metaTTL=0x%02x\n", msg->meta_data.src_addr, msg->buffer[0], msg->meta_data.ttl);
            //msg_op2_reply(model_handle, msg, msg->buf[0] - msg->meta_data.ttl + 1, msg->meta_data.ttl);
            msg_op2_send(msg->meta_data.src_addr, msg->buffer[0] - msg->meta_data.ttl + 1, msg->meta_data.ttl);

#if 0
            if (msg->meta_data.dst_addr == bt_mesh_model_get_element_address(g_vendor_element_index)) {
                msg_op1_send(bt_mesh_model_get_element_address(g_vendor_element_index) + 1, bt_mesh_model_get_element_address(g_vendor_element_index),  msg->buf[0], msg->meta_data.ttl - 1);
            }
#endif
            break;
        }
        case VENDOR_OPCODE_2: {
            LOG_I(mesh_app, "[R]Vendor_LatencyStatus={src=0x%04x, initTTL=0x%02x, recvTTL=0x%02x}, metaTTL=0x%02x\n",
                  msg->meta_data.src_addr, msg->buffer[0], msg->buffer[1], msg->meta_data.ttl);

#if 0
            if (msg->meta_data.dst_addr == bt_mesh_model_get_element_address(g_vendor_element_index)) {
                msg_op2_send(bt_mesh_model_get_element_address(g_vendor_element_index) - 1, bt_mesh_model_get_element_address(g_vendor_element_index),  msg->buf[1], msg->meta_data.ttl - 1);
            }
#endif

            break;
        }

        default:
            return;
    }

    return;
}

static void _vendorMsgHandler2(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg)
{
    VD_DEBUG_EVT_VENDOR(VENDOR_COMPANY_ID, msg->opcode.opcode, BROWN);
    bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);

    switch (msg->opcode.opcode) {
        case VENDOR_OPCODE_3: {
            // TODO: handle vendor opcode 3
            break;
        }
        case VENDOR_OPCODE_4: {
            // TODO: handle vendor opcode 4
            break;
        }
        default:
            return;
    }

    return;
}

const bt_mesh_access_opcode_handler_t vendor_message_handler[] = {
    {{VENDOR_OPCODE_1, VENDOR_COMPANY_ID}, _vendorMsgHandler},
    {{VENDOR_OPCODE_2, VENDOR_COMPANY_ID}, _vendorMsgHandler},
};

const bt_mesh_access_opcode_handler_t vendor_message_handler2[] = {
    {{VENDOR_OPCODE_3, VENDOR_COMPANY_ID}, _vendorMsgHandler2},
    {{VENDOR_OPCODE_4, VENDOR_COMPANY_ID}, _vendorMsgHandler2},
};

static void mesh_create_ctl_server(uint16_t element_index)
{
    uint8_t element_count;

    gCTL_server = (lighting_ctl_server_t *)bt_mesh_app_util_alloc(sizeof(lighting_ctl_server_t));
    gCTL_temperature_server = (lighting_ctl_temperature_server_t *)bt_mesh_app_util_alloc(sizeof(lighting_ctl_temperature_server_t));
    memset(gCTL_server, 0, sizeof(lighting_ctl_server_t));
    memset(gCTL_temperature_server, 0, sizeof(lighting_ctl_temperature_server_t));

    gCTL_server->range_min = 0x320;
    gCTL_server->range_max = 0x4E20;
    gCTL_server->default_temperature = 0x320;   // default value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->default_delta_uv = 0;  // default value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->target_ctl_temperature = 0x2000; // last known value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->target_ctl_delta_uv = -100;    // last known value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->lightness_server.target_lightness = 0xabcd;    // last known value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->lightness_server.last_lightness = 0x1234;
    gCTL_server->lightness_server.default_lightness = 0x0000;   // default value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->lightness_server.range_min = 0x0001;
    gCTL_server->lightness_server.range_max = 0xffff;
    gCTL_server->lightness_server.onoff_server.target_on_off = 1; // last known value for GENERIC_ON_POWERUP_RESTORE
    gCTL_server->lightness_server.onpowerup_server.on_power_up = GENERIC_ON_POWERUP_RESTORE;
    gCTL_server->element_index = 0xFFFF;

    gCTL_temperature_server->element_index = 0xFFFF;

    /* CTL server for DemoKit LED, containing 2 elements for Light CTL Server, Light CTL Temperature Server */
    bt_mesh_model_lighting_add_ctl_setup_server(&g_model_handle_ctl_server, &element_count, element_index, light_ctl_server_msg_handler, NULL);
    LOG_I(mesh_app, "ctl element_index = %d element_count = %d", element_index, element_count);
    gCTL_server->element_index = element_index;
    gCTL_temperature_server->element_index = gCTL_server->element_index + 1;

    /* Binding temperature with gCTL_server */
    bind_ctl_temperature(gCTL_server, gCTL_temperature_server, MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE, MESH_MODEL_BINDING_BOTH_VALUE);
}

static void mesh_create_hsl_server(uint16_t element_index)
{
    uint8_t element_count;

    gHSL_server = (lighting_hsl_server_t *)bt_mesh_app_util_alloc(sizeof(lighting_hsl_server_t));
    gHSL_hue_server = (lighting_hsl_hue_server_t *)bt_mesh_app_util_alloc(sizeof(lighting_hsl_hue_server_t));
    gHSL_saturation_server = (lighting_hsl_saturation_server_t *)bt_mesh_app_util_alloc(sizeof(lighting_hsl_saturation_server_t));
    memset(gHSL_server, 0, sizeof(lighting_hsl_server_t));
    memset(gHSL_hue_server, 0, sizeof(lighting_hsl_hue_server_t));
    memset(gHSL_saturation_server, 0, sizeof(lighting_hsl_saturation_server_t));

    /* Default blue */
    gHSL_server->default_hue = 0xaaaa;
    gHSL_server->default_saturation = 0xffff;
    gHSL_server->hue_range_min = 0;
    gHSL_server->hue_range_max = 0xFFFF;
    gHSL_server->saturation_range_min = 0;
    gHSL_server->saturation_range_max = 0xFFFF;
    gHSL_server->lightness_server.default_lightness = 0x8000;
    gHSL_server->lightness_server.range_min = 0x0001;
    gHSL_server->lightness_server.range_max = 0xffff;
    gHSL_server->lightness_server.onpowerup_server.on_power_up = GENERIC_ON_POWERUP_DEFAULT;

    gHSL_server->element_index = 0xFFFF;
    gHSL_hue_server->element_index = 0xFFFF;
    gHSL_saturation_server->element_index = 0xFFFF;

    /* HSL server for DemoKit LED, containing 3 elements for Light HSL Server, Light HSL Hue Server, Light HSL Saturation Server */
    bt_mesh_model_lighting_add_hsl_setup_server(&g_model_handle_hsl_server, &element_count, element_index, light_hsl_server_msg_handler, NULL);
    LOG_I(mesh_app, "hsl element_index = %d element_count = %d\n", element_index, element_count);
    gHSL_server->element_index = element_index;
    gHSL_hue_server->element_index = gHSL_server->element_index + 1;
    gHSL_saturation_server->element_index = gHSL_hue_server->element_index + 1;

    /* Binding hue and saturation with gHSL_server */
    bind_hsl_hue(gHSL_hue_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_HUE, MESH_MODEL_BINDING_BOTH_VALUE);
    bind_hsl_saturation(gHSL_saturation_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_SATURATION, MESH_MODEL_BINDING_BOTH_VALUE);
}

static void mesh_create_device(void)
{
    uint8_t composition_data_header[10] = {
        0x94, 0x00, // cid
        0x1A, 0x00, // pid
        0x01, 0x00, // vid
        0x64, 0x00, // crpl, BT_MESH_REPLAY_PROTECTION_LIST_DEFAULT=10
#if RELAY_ENABLE
        BT_MESH_FEATURE_RELAY | BT_MESH_FEATURE_PROXY | BT_MESH_FEATURE_FRIEND, 0x00, // features
#else
        BT_MESH_FEATURE_PROXY | BT_MESH_FEATURE_FRIEND, 0x00, // features
#endif
    };
    uint16_t element_index;
    bt_mesh_model_add_params_t model_params;

    memset(&gOnOffServer, 0, sizeof(mesh_onoff_server_model_t));

    bt_mesh_model_set_composition_data_header(composition_data_header, 10);
    bt_mesh_model_add_element(&element_index, BT_MESH_MODEL_ELEMENT_LOCATION_MAIN);

    bt_mesh_model_add_configuration_server(&g_model_handle_config_server, NULL);
    bt_mesh_model_add_health_server(&g_model_handle_health_server, element_index, NULL);
    /* onoff server for EVB LED1 */
    bt_mesh_model_add_generic_onoff_server(&g_model_handle_onoff_server, element_index,
                                           _generic_server_msg_handler,
                                           _generic_onoff_publish_callback);

    /* vendor model 1 */
    model_params.model_id = MESH_VENDOR_MODEL_ID(VENDOR_COMPANY_ID, VENDOR_MODEL_ID1);
    model_params.element_index = element_index;
    model_params.opcode_handlers = vendor_message_handler;
    model_params.opcode_count = sizeof(vendor_message_handler) / sizeof(bt_mesh_access_opcode_handler_t);
    model_params.publish_timeout_cb = NULL;
    bt_mesh_model_add_model(&g_model_handle_vendor, &model_params);

    /* vendor model 2 */
    model_params.model_id = MESH_VENDOR_MODEL_ID(VENDOR_COMPANY_ID, VENDOR_MODEL_ID2);
    model_params.element_index = element_index;
    model_params.opcode_handlers = vendor_message_handler2;
    model_params.opcode_count = sizeof(vendor_message_handler2) / sizeof(bt_mesh_access_opcode_handler_t);
    model_params.publish_timeout_cb = NULL;

    bt_mesh_model_add_model(&g_model_handle_vendor1, &model_params);

    bt_mesh_model_add_element(&element_index, BT_MESH_MODEL_ELEMENT_LOCATION_FIRST);
    mesh_create_ctl_server(element_index);

    bt_mesh_model_add_element(&element_index, BT_MESH_MODEL_ELEMENT_LOCATION_SECOND);
    mesh_create_hsl_server(element_index);

    bind_generic_power_up(MESH_MODEL_STATE_GENERIC_ON_POWER_UP, MESH_MODEL_BINDING_BOTH_VALUE);
}

void mesh_dump_model_handle(void)
{
    LOG_I(mesh_app, "mesh_dump_model_handle\n");
    LOG_I(mesh_app, "g_model_handle_config_server %x\n", g_model_handle_config_server);
    LOG_I(mesh_app, "g_model_handle_health_server %x\n", g_model_handle_health_server);
    LOG_I(mesh_app, "g_model_handle_vendor %x\n", g_model_handle_vendor);
    LOG_I(mesh_app, "g_model_handle_vendor1 %x\n", g_model_handle_vendor1);
    LOG_I(mesh_app, "g_model_handle_onoff_server %x\n", g_model_handle_onoff_server);
}

extern bt_status_t bt_hci_send_vendor_cmd(uint16_t cmd_code, uint8_t *param, uint8_t param_length);
#define MESH_WIFI_SERVICE_TIME (0x20)    //0x32, 0x22, 0x18 
static void meshinit()
{
    LOG_I(mesh_app, "Mesh initialising...\n");

    bt_mesh_init_params_t *initparams = (bt_mesh_init_params_t *)bt_mesh_app_util_alloc(sizeof(bt_mesh_init_params_t));
    initparams->role = BT_MESH_ROLE_PROVISIONEE;

    // init provision parameter
    initparams->provisionee = (bt_mesh_prov_provisionee_params_t *)bt_mesh_app_util_alloc(sizeof(bt_mesh_prov_provisionee_params_t));
    initparams->provisionee->cap.number_of_elements = bt_mesh_model_get_element_count();
    initparams->provisionee->cap.algorithms = BT_MESH_PROV_CAPABILITY_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE; // bit 0: P-256, bit 1~15: RFU
    initparams->provisionee->cap.public_key_type = BT_MESH_PROV_CAPABILITY_OOB_PUBLIC_KEY_TYPE_INBAND;
    initparams->provisionee->cap.static_oob_type = BT_MESH_PROV_CAPABILITY_OOB_STATIC_TYPE_SUPPORTED;
    initparams->provisionee->cap.output_oob_size = 0x00;
    initparams->provisionee->cap.output_oob_action = 0x0000;
    initparams->provisionee->cap.input_oob_size = 0x00;
    initparams->provisionee->cap.input_oob_action = 0x0000;

    // init config parameter
    initparams->config = (bt_mesh_config_init_params_t *)bt_mesh_app_util_alloc(sizeof(bt_mesh_config_init_params_t));
    memset(initparams->config, 0, sizeof(bt_mesh_config_init_params_t));
    memcpy(initparams->config->device_uuid, deviceUuid, BT_MESH_UUID_SIZE);
    initparams->config->oob_info = 0;
    initparams->config->uri = NULL;
    initparams->config->default_ttl = 4;

    initparams->friend = NULL;

    bt_mesh_init(initparams);
    bt_mesh_app_util_free((uint8_t *)(initparams->provisionee));
    bt_mesh_app_util_free((uint8_t *)(initparams->config));
    bt_mesh_app_util_free((uint8_t *)initparams);
}

bt_status_t bt_app_event_callback_demo(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    switch (msg) {
        case BT_POWER_ON_CNF: {
            LOG_I(mesh_app, "BT_POWER_ON_CNF\n");
            bt_mesh_app_util_check_device_uuid(deviceUuid);
            bsp_led_init(BSP_LED0_PIN);
            // init wifi service time, default value 0x22
            {
                // now it's not a SDk API, so extern it to here.
                uint8_t wifi_time = MESH_WIFI_SERVICE_TIME;
                uint16_t cmd_code = 0xFD71;
                LOG_I(mesh_app, "config wifi_time_set %x\n", wifi_time);
                bt_hci_send_vendor_cmd(cmd_code, &wifi_time, 1);
            }
            // create mesh device
            mesh_create_device();
            meshinit();
            break;
        }
    }
    return BT_STATUS_SUCCESS;
}

bt_mesh_status_t bt_mesh_app_event_callback(bt_mesh_event_id evt_id, bool status, bt_mesh_evt_t *evt_data)
{
    switch (evt_id) {
        case BT_MESH_EVT_INIT_DONE: {
            LOG_I(mesh_app, "\n==============================\n");
            LOG_I(mesh_app, "BT_MESH_EVT_INIT_DONE\n");
            VD_DEBUG_DUMP_PDU("UUID", BT_MESH_UUID_SIZE, deviceUuid);
            LOG_I(mesh_app, "==============================\n\n");

            bt_mesh_enable();
            break;
        }
        case BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY: {
            LOG_I(mesh_app, "BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY\n");
            bt_mesh_evt_prov_show_pk_t *p = (bt_mesh_evt_prov_show_pk_t *)evt_data;
            VD_DEBUG_DUMP_PDU("PublicKey", 64, p->pk);
            bt_mesh_app_util_free(p->pk);
            break;
        }
        case BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE: {
            LOG_I(mesh_app, "BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE\n");
            bt_mesh_evt_prov_request_auth_t *p = (bt_mesh_evt_prov_request_auth_t *)evt_data;
            LOG_I(mesh_app, "\tMethod : %d\n", p->method);
            LOG_I(mesh_app, "\tAction : %d\n", p->action);
            LOG_I(mesh_app, "\tSize   : %d\n", p->size);
            switch (p->method) {
                case BT_MESH_PROV_START_AUTHEN_METHOD_STATIC_OOB:
                    LOG_I(mesh_app, "\tMethod : Static OOB\n");
                    uint8_t auth_value[16] = {
                        0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                    };
                    bt_mesh_provision_provide_oob_auth_value(auth_value, 16);
                    break;
                case BT_MESH_PROV_START_AUTHEN_METHOD_OUTPUT_OOB:
                    LOG_I(mesh_app, "\tMethod : Ouput OOB\n");
                    break;
                case BT_MESH_PROV_START_AUTHEN_METHOD_INPUT_OOB:
                    LOG_I(mesh_app, "\tMethod : Input OOB\n");
                    break;
                default:
                    LOG_I(mesh_app, "\tMethod : %d, invalid\n", p->method);
                    break;
            }
            break;
        }
        case BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE: {
            LOG_I(mesh_app, "BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE\n");
            break;
        }
        case BT_MESH_EVT_PROV_DONE: {
            bt_mesh_evt_prov_done_t *p = (bt_mesh_evt_prov_done_t *)evt_data;
            LOG_I(mesh_app_must, "BT_MESH_EVT_PROV_DONE %s\n", status ? "SUCCESS" : "FAILED");
            LOG_I(mesh_app, "\tUnicastAddr: 0x%lx", p->address);
            LOG_I(mesh_app,  "\t");
            VD_DEBUG_DUMP_PDU("DeviceKey", BT_MESH_DEVKEY_SIZE, p->device_key);
            break;
        }
        case BT_MESH_EVT_CONFIG_RESET: {
            LOG_I(mesh_app, "BT_MESH_EVT_CONFIG_RESET, ready to do SW reset...\n");
            hal_cache_disable();
            hal_cache_deinit();
            hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
            break;
        }
        case BT_MESH_EVT_HEARTBEAT: {
            LOG_I(mesh_app, "BT_MESH_EVT_HEARTBEAT\n");
            break;
        }
        case BT_MESH_EVT_IV_UPDATE: {
            LOG_I(mesh_app, "BT_MESH_EVT_IV_UPDATE\n");
            bt_mesh_evt_iv_update_t *p = (bt_mesh_evt_iv_update_t *)evt_data;
            LOG_I(mesh_app, "\tState    : 0x%x\n", p->state);
            LOG_I(mesh_app, "\tIV Index : 0x%08lx\n", p->iv_index);
            break;
        }
        case BT_MESH_EVT_KEY_REFRESH: {
            LOG_I(mesh_app, "BT_MESH_EVT_KEY_REFRESH\n");
            bt_mesh_evt_key_refresh_t *p = (bt_mesh_evt_key_refresh_t *)evt_data;
            LOG_I(mesh_app, "\tNetkeyIndex : 0x%03x\n", p->netkey_index);
            LOG_I(mesh_app, "\tPhase       : 0x%02x\n", p->phase);
        }
        case BT_MESH_EVT_BEARER_GATT_STATUS: {
            bt_mesh_evt_bearer_gatt_status_t *p = (bt_mesh_evt_bearer_gatt_status_t *)evt_data;
            LOG_I(mesh_app, "BT_MESH_EVT_BEARER_GATT_STATUS");
            LOG_I(mesh_app, "\tHandle:%ld status:%x", p->handle, p->status);
            break;
        }
        case BT_MESH_EVT_FRIENDSHIP_STATUS: {
            LOG_I(mesh_app, "BT_MESH_EVT_FRIENDSHIP_STATUS");
            bt_mesh_evt_friendship_status_t *p = (bt_mesh_evt_friendship_status_t *)evt_data;
            LOG_I(mesh_app, "\tLPN Address : 0x%03x", p->address);
            LOG_I(mesh_app, "\tStatus      : %x", p->status);
            break;
        }
    }
    
    return BT_MESH_SUCCESS;
}

/******************************************************************************/
/* command line                                                               */
/******************************************************************************/
bt_status_t cmd_config(void *no, uint16_t argc, char **argv)
{
    if (argc < 2) {
        return BT_STATUS_FAIL;
    }

    if (!strcmp(argv[1], "feature")) {
        // config feature <feature>, (relay: 1, lpn: 2, friend: 4)
        if (argc < 4) {
            LOG_I(mesh_app, "USAGE: config feature <features>, <features>: relay=1, proxy: 2, friend=4, lpn=8\n");
            return BT_STATUS_FAIL;
        }
        if (atoi(argv[3]) != 0) {
            bt_mesh_config_enable_features(atoi(argv[2]));
        } else {
            bt_mesh_config_disable_features(atoi(argv[2]));
        }
    } else if (!strcmp(argv[1], "ttl")) {
        if (argc < 3) {
            LOG_I(mesh_app, "USAGE: config ttl <ttl>\n");
            return BT_STATUS_FAIL;
        }
        uint8_t ttl = 0;
        bt_mesh_app_util_str2u8HexNum(argv[2], &ttl);
        bt_mesh_config_set_default_ttl(ttl);
    } else if (!strcmp(argv[1], "ptout")) {
        if (argc < 4) {
            LOG_I(mesh_app, "USAGE: config ptout <tout1> <tout2>\n");
            return BT_STATUS_FAIL;
        }
        uint32_t nid = 0, node_identy = 0;
        bt_mesh_app_util_str2u32HexNum(argv[2], &nid);
        bt_mesh_app_util_str2u32HexNum(argv[3], &node_identy);
        bt_mesh_proxy_set_timeout(nid, node_identy);
    } else if (!strcmp(argv[1], "padv_tout")) {
        if (argc < 4) {
            LOG_I(mesh_app, "USAGE: config padv_tout <pb_adv_tout> <beacon_tout>\n");
            return BT_STATUS_FAIL;
        }
        uint32_t pb_adv_tout = 0, beacon_tout = 0;
        bt_mesh_app_util_str2u32HexNum(argv[2], &pb_adv_tout);
        bt_mesh_app_util_str2u32HexNum(argv[3], &beacon_tout);
        bt_mesh_provision_adv_set_timeout(pb_adv_tout, beacon_tout);
    }


    return BT_STATUS_SUCCESS;
}

bt_status_t cmd_delete(void *no, uint16_t argc, char **argv)
{
    if (argc < 2) {
        LOG_I(mesh_app, "delete all mesh sectors.\n");
        bt_mesh_flash_reset(BT_MESH_FLASH_SECTOR_ALL);
        return BT_STATUS_SUCCESS;
    }

    if (!strcmp(argv[1], "seq")) {
        LOG_I(mesh_app, "delete all mesh sequence number sector.\n");
        bt_mesh_flash_reset(BT_MESH_FLASH_SECTOR_SEQUENCE_NUMBER);
    } else if (!strcmp(argv[1], "data")) {
        LOG_I(mesh_app, "delete all mesh data sector.\n");
        bt_mesh_flash_reset(BT_MESH_FLASH_SECTOR_DATA);
    }
    return BT_STATUS_SUCCESS;
}

const mesh_app_callback_table_t cmds[] = {
    {"power", cmd_power},
    {"dump", cmd_dump},
    {"delete", cmd_delete},
    {"config", cmd_config},
    {"adv", cmd_adv},
    {NULL, NULL}
};

extern void bt_hci_log_enable(bool enable);
extern void bt_driver_trigger_controller_codedump(void);

bt_status_t mesh_app_io_callback(uint8_t len, char *param[])
{
    uint16_t i;
    if (len < 1) {
        LOG_I(mesh_app, "%s, invalid param\n", __FUNCTION__);
        return BT_STATUS_FAIL;
    }
    
    LOG_I(mesh_app, "%s name is %s, param len %d\n", __FUNCTION__, param[0], len);
    for (i = 0; i < sizeof(cmds) / sizeof(mesh_app_callback_table_t); i++) {
        if (!strcmp(cmds[i].name, param[0])) {
            return cmds[i].io_callback(NULL, (uint16_t)len, param);
        }
    }

    if (!strncmp(param[0], "hci", 3)) {
        if (!strncmp(param[1], "on", 2)) {
            bt_hci_log_enable(true);
        } else if (!strncmp(param[1], "off", 3)) {
            bt_hci_log_enable(false);
        }
    } else if (!strncmp(param[0], "dump_n9", 7)) {
        LOG_I(mesh_app, "start dump n9");
        bt_driver_trigger_controller_codedump();
        assert(0);
    } else if (!strncmp(param[0], "assert", 6)) {
        assert(0);
    } else {
        LOG_I(mesh_app, "%s cmd not found\n", __FUNCTION__);
    }

    return BT_STATUS_FAIL;
}

