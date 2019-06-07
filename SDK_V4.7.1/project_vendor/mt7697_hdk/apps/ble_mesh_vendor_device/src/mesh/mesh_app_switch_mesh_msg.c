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
#include <string.h>

#include "bt_mesh_model.h"
#include "bt_mesh_model_generic_server.h"
#include "bt_mesh_model_lighting_opcodes.h"
#include "bt_mesh_model_lighting_client.h"

#include "bt_mesh.h"
#include "bt_mesh_common.h"

#include "mesh_app_util.h"
#include "mesh_app_switch_mesh_msg.h"
#include "mesh_app_switch_def.h"

static uint16_t gAppIndex = 0x0123;

void msg_Light_Lightness_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta = {{0}};
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_get_lightness(model_handle, &tx_meta);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

void msg_Light_Lightness_Linear_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta;
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_get_linear_lightness(model_handle, &tx_meta);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

void msg_Light_Lightness_Last_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta;
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_get_last_lightness(model_handle, &tx_meta);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

void msg_Light_Lightness_Default_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta;
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_get_default_lightness(model_handle, &tx_meta);

    LOG_I(mesh_app, "ret: %d\n", ret);
}


void msg_Light_Lightness_Range_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_tx_params_t param;
    param.dst.value = dst_addr;
    param.dst.type = bt_mesh_utils_get_addr_type(param.dst.value);
#ifdef PROVISIONER_ENABLE
    param.src = 0x0001;
#else
    param.src = bt_mesh_model_get_element_address(0);
#endif
    param.ttl = ttl;
    param.data_len = 2;

    uint8_t *payload = bt_mesh_app_util_alloc(param.data_len);
    payload[0] = (BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_GET  & 0xFF00) >> 8;
    payload[1] = BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_GET & 0x00FF;
    param.data = payload;
    param.security.appidx = gAppIndex;
    param.security.netidx = BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX;
    bt_mesh_status_t ret = bt_mesh_send_packet(&param);
    LOG_I(mesh_app, "[T]msg_Light_Lightness_Range_Get={dst=0x%04x}, result:%d\n", dst_addr, ret);
    bt_mesh_app_util_free(payload);
}

void msg_Light_Lightness_Set(uint16_t model_handle, uint16_t dst_addr,
                             uint16_t Lightness, uint8_t tid, uint8_t transTime,
                             uint8_t delay, bool reliable, uint8_t ttl)
{

    bt_mesh_access_message_tx_meta_t tx_meta = {{0}};
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    uint8_t payload[5] = {0};
    memcpy(payload, &Lightness, 2);
    payload[2] = tid;
    payload[3] = transTime;
    payload[4] = delay;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_set_lightness(
                                    model_handle, &tx_meta, payload, 5, reliable);

    LOG_I(mesh_app, "[T]Light_Lightness_Set={dst=0x%04x, Lightness=0x%04x, tid=0x%02x, transTime=0x%02x, delay=0x%02x}, reliable=%s, result:%d\n",
          dst_addr, Lightness, tid, transTime, delay, reliable ? "true" : "false", ret);
}

void msg_Light_Hsl_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta;
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_get_hsl(model_handle, &tx_meta);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

void msg_Light_Hsl_Set(uint16_t model_handle, uint16_t dst_addr,
                       uint16_t Lightness, uint16_t Hue, uint16_t Saturation, uint8_t tid,
                       uint8_t transTime, uint8_t delay, bool acknowledged, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta = {{0}};
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    uint8_t payload[10] = {0};
    memcpy(payload, &Lightness, 2);
    memcpy(payload + 2, &Hue, 2);
    memcpy(payload + 4, &Saturation, 2);

    payload[6] = tid;
    payload[7] = transTime;
    payload[8] = delay;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_set_hsl(model_handle, &tx_meta,
                                payload, 10, acknowledged);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

void msg_Light_Ctl_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta;
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_get_ctl(model_handle, &tx_meta);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

void msg_Light_Ctl_Set(uint16_t model_handle, uint16_t dst_addr,
                       uint16_t Lightness, uint16_t Temperature, int16_t DeltaUV,
                       uint8_t tid, uint8_t transTime, uint8_t delay, bool acknowledged, uint8_t ttl)
{
    bt_mesh_access_message_tx_meta_t tx_meta = {{0}};
    tx_meta.dst_addr.value = dst_addr;
    tx_meta.dst_addr.type = bt_mesh_utils_get_addr_type(dst_addr);
    tx_meta.appkey_index = gAppIndex;
    tx_meta.ttl = ttl;

    uint8_t payload[10] = {0};
    memcpy(payload, &Lightness, 2);
    memcpy(payload + 2, &Temperature, 2);
    memcpy(payload + 4, &DeltaUV, 2);

    payload[6] = tid;
    payload[7] = transTime;
    payload[8] = delay;

    bt_mesh_status_t ret = bt_mesh_model_lighting_client_set_ctl(model_handle, &tx_meta,
                                payload, 10, acknowledged);

    LOG_I(mesh_app, "ret: %d\n", ret);
}

