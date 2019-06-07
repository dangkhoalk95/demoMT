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

#include "mesh_app_util.h"
#include "mesh_app_switch_def.h"

#include "bt_mesh.h"
#include "bt_mesh_model_configuration_client.h"

#define MESH_APP_GET_NODE_INFO_TIMER 5000
typedef enum {
    MESH_APP_QUERY_INFO_STATE_IDLE,
    MESH_APP_QUERY_INFO_STATE_START,
    MESH_APP_QUERY_INFO_STATE_GET_COMPOSITION_DATA,
    MESH_APP_QUERY_INFO_STATE_GET_MODEL_PUB_INFO,
    MESH_APP_QUERY_INFO_STATE_GET_MODEL_SUB_INFO,
    MESH_APP_QUERY_INFO_STATE_GET_MODEL_APP_INFO,
    MESH_APP_QUERY_INFO_STATE_GET_NETKEY,
    MESH_APP_QUERY_INFO_STATE_GET_DEFAULT_TTL,
    MESH_APP_QUERY_INFO_STATE_GET_NETWORK_TRANSMIT,
    MESH_APP_QUERY_INFO_STATE_GET_RELAY,
    MESH_APP_QUERY_INFO_STATE_GET_BEACON,
    MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_PUB,
    MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_SUB,
    MESH_APP_QUERY_INFO_STATE_GET_NODE_IDENTITY,
    MESH_APP_QUERY_INFO_STATE_GET_APPKEY,
    MESH_APP_QUERY_INFO_STATE_GET_LPN_POLL_TIMEOUT,
    MESH_APP_QUERY_INFO_STATE_DONE,
} mesh_app_query_info_state_t;

static void *remote_model_list = NULL; // AB_queue of switch_mesh_binding_model_t
static mesh_app_query_info_state_t g_query_info_state = MESH_APP_QUERY_INFO_STATE_IDLE;
static uint16_t g_dst_addr;
static uint16_t g_lpn_addr;
static uint8_t gConfigureRetry = MESH_CONFIGURE_RETRY_MAX;

extern uint8_t gConfigureRetry;
extern uint16_t g_config_client_element_index;
extern TimerHandle_t mesh_get_remote_info_timer;
extern uint16_t g_prov_current_addr;

void mesh_app_get_composition_data();
void mesh_app_get_model_publication();
void mesh_app_get_model_subscription();
void mesh_app_get_model_app();
void mesh_app_get_net_key();
void mesh_app_get_default_ttl();
void mesh_app_get_network_transmit();
void mesh_app_get_relay();
void mesh_app_get_beacon();
void mesh_app_get_heartbeat_publication();
void mesh_app_get_heartbeat_subscription();
void mesh_app_get_node_identity();
void mesh_app_get_app_key();
void mesh_app_get_lpn_poll_timeout();


static void _parse_composition_data(bt_mesh_composition_data_t *data)
{
    LOG_I(mesh_app, "CID: 0x%04x\n", data->cid);
    LOG_I(mesh_app, "PID: 0x%04x\n", data->pid);
    LOG_I(mesh_app, "VID: 0x%04x\n", data->vid);
    LOG_I(mesh_app, "CRPL: 0x%04x\n", data->crpl);
    LOG_I(mesh_app, "Features: 0x%04x\n", data->features);
    if (remote_model_list != NULL) {
        bt_mesh_os_layer_ds_queue_free(remote_model_list);
    }
    remote_model_list = bt_mesh_os_layer_ds_queue_alloc();

    if (data->elements != NULL && bt_mesh_os_layer_ds_dlist_count(data->elements) > 0) {
        switch_mesh_binding_model_t *model_entry = NULL;
        uint32_t i, j = 0;
        bt_mesh_composition_element_t *entry = (bt_mesh_composition_element_t *)bt_mesh_os_layer_ds_dlist_first(data->elements);
        while (entry != NULL) {
            LOG_I(mesh_app, "    Element#%d, LOC: 0x%04x\n", j, entry->loc);
            LOG_I(mesh_app, "\tNumS: %d {", entry->num_s);
            if (entry->num_s > 0) {
                for (i = 0; i < entry->num_s ; i++) {
                    if (entry->sig_models[i] != BT_MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER) {
                        model_entry = bt_mesh_os_layer_ds_queue_entry_alloc(sizeof(switch_mesh_binding_model_t));
                        model_entry->element_index = j;
                        model_entry->model_id = entry->sig_models[i];
                        bt_mesh_os_layer_ds_queue_push(remote_model_list, model_entry);
                        LOG_I(mesh_app, "%04x, ", entry->sig_models[i]);
                    }
                }
            }
            LOG_I(mesh_app, "}\n");
            LOG_I(mesh_app, "\tNumV: %d {", entry->num_v);
            if (entry->num_v > 0) {
                for (i = 0; i < entry->num_v ; i++) {
                    model_entry = bt_mesh_os_layer_ds_queue_entry_alloc(sizeof(switch_mesh_binding_model_t));
                    model_entry->element_index = j;
                    model_entry->model_id = entry->vendor_models[i];
                    bt_mesh_os_layer_ds_queue_push(remote_model_list, model_entry);
                    LOG_I(mesh_app, "%08lx, ", entry->vendor_models[i]);
                }
            }
            LOG_I(mesh_app, "}\n");
            entry = bt_mesh_os_layer_ds_dlist_next(data->elements, (void *)entry);
            j++;
        }
    }
}

void mesh_app_get_composition_data()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_composition_data(
                                        0xff,
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX
                                    );
        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_COMPOSITION_DATA;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get composition data\n");
        mesh_app_get_model_publication();
    }
}

/* start binding model and application key */

void mesh_app_get_model_publication()
{
    if (gConfigureRetry > 0) {
        switch_mesh_binding_model_t *entry = bt_mesh_os_layer_ds_queue_first(remote_model_list);
        if (entry != NULL) {
            bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_model_publication(
                                            g_prov_current_addr + entry->element_index,
                                            entry->model_id,
                                            bt_mesh_model_get_element_address(g_config_client_element_index),
                                            g_dst_addr,
                                            bt_mesh_config_get_default_ttl(),
                                            BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

            LOG_I(mesh_app, "ret: %d\n", ret);
            if (ret == 0) {
                gConfigureRetry--;
                g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_MODEL_PUB_INFO;
                bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
            } else {
                LOG_I(mesh_app, "error!\n");
            }
        } else {
            LOG_I(mesh_app, "no more model, all model pub list query is done\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get model pub\n");
        mesh_app_get_model_subscription();
    }
}

void mesh_app_get_model_subscription()
{
    if (gConfigureRetry > 0) {
        switch_mesh_binding_model_t *entry = bt_mesh_os_layer_ds_queue_first(remote_model_list);
        if (entry != NULL) {
            bt_mesh_status_t ret;
            if (entry->model_id & 0xff00) {
                ret = bt_mesh_model_configuration_client_get_vendor_model_subscription(
                          g_prov_current_addr + entry->element_index,
                          entry->model_id,
                          bt_mesh_model_get_element_address(g_config_client_element_index),
                          g_dst_addr,
                          bt_mesh_config_get_default_ttl(),
                          BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
            } else {
                ret = bt_mesh_model_configuration_client_get_sig_model_subscription(
                          g_prov_current_addr + entry->element_index,
                          entry->model_id,
                          bt_mesh_model_get_element_address(g_config_client_element_index),
                          g_dst_addr,
                          bt_mesh_config_get_default_ttl(),
                          BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
            }

            LOG_I(mesh_app, "ret: %d\n", ret);
            if (ret == 0) {
                gConfigureRetry--;
                g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_MODEL_SUB_INFO;
                bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
            } else {
                LOG_I(mesh_app, "error!\n");
            }
        } else {
            LOG_I(mesh_app, "no more model, all model sub list query is done\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get model sub\n");
        mesh_app_get_model_app();
    }
}

void mesh_app_get_model_app()
{
    if (gConfigureRetry > 0) {
        switch_mesh_binding_model_t *entry = bt_mesh_os_layer_ds_queue_first(remote_model_list);
        if (entry != NULL) {
            bt_mesh_status_t ret;
            if (entry->model_id & 0xff00) {
                ret = bt_mesh_model_configuration_client_get_vendor_model_app(
                          g_prov_current_addr + entry->element_index,
                          entry->model_id,
                          bt_mesh_model_get_element_address(g_config_client_element_index),
                          g_dst_addr,
                          bt_mesh_config_get_default_ttl(),
                          BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
            } else {
                ret = bt_mesh_model_configuration_client_get_sig_model_app(
                          g_prov_current_addr + entry->element_index,
                          entry->model_id,
                          bt_mesh_model_get_element_address(g_config_client_element_index),
                          g_dst_addr,
                          bt_mesh_config_get_default_ttl(),
                          BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
            }
            LOG_I(mesh_app, "ret: %d\n", ret);
            if (ret == 0) {
                gConfigureRetry--;
                g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_MODEL_APP_INFO;
                bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
            } else {
                LOG_I(mesh_app, "error!\n");
            }
        } else {
            LOG_I(mesh_app, "no more model, all model app query is done\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get model app\n");
        mesh_app_get_net_key();
    }
}

void mesh_app_get_net_key()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_net_key(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_NETKEY;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get net key\n");
        mesh_app_get_default_ttl();
    }

}

void mesh_app_get_default_ttl()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_default_ttl(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_DEFAULT_TTL;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get default ttl\n");
        mesh_app_get_network_transmit();
    }

}

void mesh_app_get_network_transmit()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_network_transmit(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_NETWORK_TRANSMIT;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get network transmit\n");
        mesh_app_get_relay();
    }

}

void mesh_app_get_relay()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_relay(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);
        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_RELAY;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get relay\n");
        mesh_app_get_beacon();
    }
}

void mesh_app_get_beacon()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_beacon(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_BEACON;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get beacon\n");
        mesh_app_get_heartbeat_publication();
    }
}

void mesh_app_get_heartbeat_publication()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_heartbeat_publication(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_PUB;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get heartbeat pub\n");
        mesh_app_get_heartbeat_subscription();
    }
}

void mesh_app_get_heartbeat_subscription()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_heartbeat_subscription(
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_SUB;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get heartbeat sub\n");
        mesh_app_get_node_identity();
    }
}

void mesh_app_get_node_identity()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_node_identity(
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX,
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_NODE_IDENTITY;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get node identity\n");
        mesh_app_get_app_key();
    }
}

void mesh_app_get_app_key()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_app_key(
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX,
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_APPKEY;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get app key\n");
        mesh_app_get_lpn_poll_timeout();
    }
}

void mesh_app_get_lpn_poll_timeout()
{
    if (gConfigureRetry > 0) {
        bt_mesh_status_t ret = bt_mesh_model_configuration_client_get_lpn_poll_timeout(
                                        g_lpn_addr,
                                        bt_mesh_model_get_element_address(g_config_client_element_index),
                                        g_dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX);

        LOG_I(mesh_app, "ret: %d\n", ret);
        if (ret == 0) {
            gConfigureRetry--;
            g_query_info_state = MESH_APP_QUERY_INFO_STATE_GET_LPN_POLL_TIMEOUT;
            bt_mesh_app_util_start_timer(mesh_get_remote_info_timer, MESH_APP_GET_NODE_INFO_TIMER);
        } else {
            LOG_I(mesh_app, "error!\n");
        }
    } else {
        LOG_I(mesh_app, "stop retry get lpn poll timeout\n");
        g_query_info_state = MESH_APP_QUERY_INFO_STATE_IDLE;
    }
}

//1.bt_mesh_model_configuration_client_get_composition_data
//2.bt_mesh_model_configuration_client_get_model_publication

//3.bt_mesh_model_configuration_client_get_vendor_model_subscription
//bt_mesh_model_configuration_client_get_sig_model_subscription

//4.bt_mesh_model_configuration_client_get_vendor_model_app
//bt_mesh_model_configuration_client_get_sig_model_app

//repeat 2 ~ 4

//5.bt_mesh_model_configuration_client_get_net_key

//6.bt_mesh_model_configuration_client_get_default_ttl

//7.bt_mesh_model_configuration_client_get_network_transmit

//8.bt_mesh_model_configuration_client_get_relay

//9.bt_mesh_model_configuration_client_get_beacon

//10.bt_mesh_model_configuration_client_get_heartbeat_publication

//11.bt_mesh_model_configuration_client_get_heartbeat_subscription

//12.bt_mesh_model_configuration_client_get_node_identity

//13.bt_mesh_model_configuration_client_get_app_key

//14.bt_mesh_model_configuration_client_get_lpn_poll_timeout
void mesh_app_get_remote_node_info(uint16_t dst_addr, uint16_t lpn_addr)
{
    g_dst_addr = dst_addr;
    g_lpn_addr = lpn_addr;
    g_query_info_state = MESH_APP_QUERY_INFO_STATE_START;
    mesh_app_get_composition_data();
}

void _get_remote_info_timeout_callback(TimerHandle_t handle)
{
    switch (g_query_info_state) {
        case MESH_APP_QUERY_INFO_STATE_GET_COMPOSITION_DATA: {
            mesh_app_get_composition_data();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_MODEL_PUB_INFO: {
            mesh_app_get_model_publication();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_MODEL_SUB_INFO: {
            mesh_app_get_model_subscription();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_MODEL_APP_INFO: {
            mesh_app_get_model_app();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_NETKEY: {
            mesh_app_get_net_key();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_DEFAULT_TTL: {
            mesh_app_get_default_ttl();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_NETWORK_TRANSMIT: {
            mesh_app_get_network_transmit();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_RELAY: {
            mesh_app_get_relay();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_BEACON: {
            mesh_app_get_beacon();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_PUB: {
            mesh_app_get_heartbeat_publication();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_SUB: {
            mesh_app_get_heartbeat_subscription();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_NODE_IDENTITY: {
            mesh_app_get_node_identity();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_APPKEY: {
            mesh_app_get_app_key();
            break;
        }
        case MESH_APP_QUERY_INFO_STATE_GET_LPN_POLL_TIMEOUT: {
            mesh_app_get_lpn_poll_timeout();
            break;
        }
        default: {
            LOG_I(mesh_app, "wrong state %d\n", g_query_info_state);
            break;
        }
    }
}

void mesh_app_get_remote_info_handler(uint16_t model_handle,
                                      const bt_mesh_access_message_rx_t *msg, const config_client_evt_t *event)
{
    bt_mesh_app_util_stop_timer(mesh_get_remote_info_timer);
    if (g_query_info_state < MESH_APP_QUERY_INFO_STATE_START) {
        return;
    }

    gConfigureRetry = MESH_CONFIGURE_RETRY_MAX;

    switch (event->evt_id) {
        case CONFIG_CLIENT_EVT_COMPOSITION_DATA_STATUS: {
            SWITCH_DEBUG_EVT_SRC("GetCompositionDataStatus", msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_COMPOSITION_DATA) {
                if (remote_model_list != NULL) {
                    bt_mesh_os_layer_ds_queue_free(remote_model_list);
                    remote_model_list = NULL;
                }

                if (event->data.composition_data_status.composition_data != NULL) {
                    _parse_composition_data(event->data.composition_data_status.composition_data);
                }

                mesh_app_get_model_publication();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_MODEL_PUBLICATION_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigModelPublicationStatus", msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_MODEL_PUB_INFO) {
                const config_client_evt_model_publication_status_t *evt = &event->data.model_publication_status;
                if (evt->status == BT_MESH_ACCESS_MSG_STATUS_SUCCESS) {
                    LOG_I(mesh_app, "\tStatus       : 0x%02x\n",  evt->status);
                    LOG_I(mesh_app, "\tElementAddr  : 0x%04x\n",  evt->element_address);
                    LOG_I(mesh_app, "\tPublishAddr  : 0x%04x\n",  evt->publish_address);
                    LOG_I(mesh_app, "\tAppkeyIndex  : 0x%03x\n",  evt->appkey_index);
                    LOG_I(mesh_app, "\tCredential   : 0x%01x\n",  evt->credential_flag);
                    LOG_I(mesh_app, "\tPublishTTL   : 0x%02x\n",  evt->publish_ttl);
                    LOG_I(mesh_app, "\tPublishPeriod: 0x%02x\n",  evt->publish_period);
                    LOG_I(mesh_app, "\tPublishCount : 0x%02x\n",  evt->publish_retransmit_count);
                    LOG_I(mesh_app, "\tIntervalSteps: 0x%02x\n",  evt->publish_retransmit_interval_steps);
                    LOG_I(mesh_app, "\tModelID      : 0x%08lx\n",  evt->model_id);
                } else {
                    LOG_I(mesh_app, "no pub list for model\n");
                }
                mesh_app_get_model_subscription();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_LIST: {//need to check vendor or SIG model
            SWITCH_DEBUG_EVT_SRC("ConfigModelSubscriptionList", msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_MODEL_SUB_INFO) {
                const config_client_evt_model_subscription_list_t *evt = &event->data.model_subscription_list;
                if (evt->status == BT_MESH_ACCESS_MSG_STATUS_SUCCESS) {
                    LOG_I(mesh_app, "\tStatus       : 0x%02x\n",  evt->status);
                    LOG_I(mesh_app, "\tElementAddr  : 0x%04x\n",  evt->element_address);
                    LOG_I(mesh_app, "\tModelID      : 0x%08lx\n",  evt->model_id);
                    LOG_I(mesh_app, "\tAddress      : ");
                    uint16_t i = 0;
                    while (i < evt->address_count) {
                        LOG_I(mesh_app, "0x%04x, ", evt->address[i]);
                        i++;
                    }
                    LOG_I(mesh_app, "\n");
                } else {
                    LOG_I(mesh_app, "no sub list for model\n");
                }
                mesh_app_get_model_app();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_MODEL_APP_LIST: {
            SWITCH_DEBUG_EVT_SRC("ConfigModelAppList", msg->meta_data.src_addr);

            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_MODEL_APP_INFO) {
                const config_client_evt_model_app_list_t *evt = &event->data.model_app_list;
                switch_mesh_binding_model_t *entry = bt_mesh_os_layer_ds_queue_pop(remote_model_list);
                bt_mesh_os_layer_ds_queue_entry_free(entry);

                if (evt->status == BT_MESH_ACCESS_MSG_STATUS_SUCCESS) {
                    LOG_I(mesh_app, "\tStatus       : 0x%02x\n",  evt->status);
                    LOG_I(mesh_app, "\tElementAddr  : 0x%04x\n",  evt->element_address);
                    LOG_I(mesh_app, "\tModelID      : 0x%08lx\n",  evt->model_id);
                    LOG_I(mesh_app, "\tAppkeyIndexes: ");
                    uint16_t i = 0;
                    while (i < evt->appkey_count) {
                        LOG_I(mesh_app, "0x%04x, ", evt->appkey_indexes[i]);
                        i++;
                    }
                    LOG_I(mesh_app, "\n");
                } else {
                    LOG_I(mesh_app, "model app query is failed, error code: %d\n", evt->status);
                }

                uint8_t count = bt_mesh_os_layer_ds_queue_count(remote_model_list);
                if (count != 0) {
                    mesh_app_get_model_publication();
                } else {
                    mesh_app_get_net_key();
                    LOG_I(mesh_app, "no more model, all model query is done\n");
                }
            }
            break;
        }
        case CONFIG_CLIENT_EVT_NETKEY_LIST: {
            SWITCH_DEBUG_EVT_SRC("ConfigNetkeyList", msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_NETKEY) {
                const config_client_evt_netkey_list_t *evt = &event->data.netkey_list;

                LOG_I(mesh_app, "\tNetkeyIndexes : ");
                uint16_t i = 0;
                while (i < evt->netkey_count) {
                    LOG_I(mesh_app, "0x%03x, ", evt->netkey_indexes[i]);
                    i++;
                }
                LOG_I(mesh_app, "\n");
                mesh_app_get_default_ttl();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_DEFAULT_TTL_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigDefaultTTLStatus", msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_DEFAULT_TTL) {
                const config_client_evt_default_ttl_status_t *evt = &event->data.default_ttl_status;
                LOG_I(mesh_app, "\tDefaultTTL  : 0x%02x\n",  evt->ttl);
                mesh_app_get_network_transmit();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_NETWORK_TRANSMIT_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigNetworkTransmitStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_NETWORK_TRANSMIT) {
                const config_client_evt_network_transmit_status_t *evt = &event->data.network_transmit_status;
                LOG_I(mesh_app, "\tCount  : 0x%02x\n",  evt->retransmit_count);
                LOG_I(mesh_app, "\tIntervalSteps  : 0x%02x\n",  evt->retransmit_interval_steps);
                mesh_app_get_relay();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_RELAY_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigRelayStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_RELAY) {
                const config_client_evt_relay_status_t *evt = &event->data.relay_status;
                LOG_I(mesh_app, "\tRelay  : 0x%02x\n",  evt->relay);
                LOG_I(mesh_app, "\tCount  : 0x%02x\n",  evt->retransmit_count);
                LOG_I(mesh_app, "\tIntervalSteps  : 0x%02x\n",  evt->retransmit_interval_steps);
                mesh_app_get_beacon();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_BEACON_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigBeaconStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_BEACON) {
                const config_client_evt_beacon_status_t *evt = &event->data.beacon_status;
                LOG_I(mesh_app, "\tBeacon    : 0x%02x\n",  evt->beacon);
                mesh_app_get_heartbeat_publication();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_HEARTBEAT_PUBLICATION_STATUS: {
            SWITCH_DEBUG_EVT_SRC("HeartbeatPubStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_PUB) {
                const config_client_evt_heartbeat_publication_status_t *evt = &event->data.heartbeat_publication_status;
                if (evt->status == BT_MESH_ACCESS_MSG_STATUS_SUCCESS) {
                    LOG_I(mesh_app, "\tDestination  : 0x%02x\n",  evt->destination);
                    LOG_I(mesh_app, "\tCount_log    : 0x%01x\n",  evt->count_log);
                    LOG_I(mesh_app, "\tPeriod_log   : 0x%01x\n",  evt->period_log);
                    LOG_I(mesh_app, "\tTTL          : 0x%01x\n",  evt->ttl);
                    LOG_I(mesh_app, "\tFeatures     : 0x%02x\n",  evt->features);
                    LOG_I(mesh_app, "\tNetkey_index : 0x%02x\n",  evt->netkey_index);
                } else {
                    LOG_I(mesh_app, "heartbeat pub query is failed, error code: %d, try next model\n", evt->status);
                }
                mesh_app_get_heartbeat_subscription();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_HEARTBEAT_SUBSCRIPTION_STATUS: {
            SWITCH_DEBUG_EVT_SRC("HeartbeatSubStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_HEARTBEAT_SUB) {
                const config_client_evt_heartbeat_subscription_status_t *evt = &event->data.heartbeat_subscription_status;
                if (evt->status == BT_MESH_ACCESS_MSG_STATUS_SUCCESS) {
                    LOG_I(mesh_app, "\tSource       : 0x%02x\n",  evt->source);
                    LOG_I(mesh_app, "\tDestination  : 0x%02x\n",  evt->destination);
                    LOG_I(mesh_app, "\tCount_log    : 0x%01x\n",  evt->count_log);
                    LOG_I(mesh_app, "\tPeriod_log   : 0x%01x\n",  evt->period_log);
                    LOG_I(mesh_app, "\tMin_hop      : 0x%01x\n",  evt->min_hops);
                    LOG_I(mesh_app, "\tMax_hop      : 0x%01x\n",  evt->max_hops);
                } else {
                    LOG_I(mesh_app, "heartbeat sub query is failed, error code: %d\n", evt->status);
                }
                mesh_app_get_node_identity();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_NODE_IDENTITY_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigNodeIdentityStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_NODE_IDENTITY) {
                const config_client_evt_node_identity_status_t *evt = &event->data.node_identity_status;
                if (evt->status == BT_MESH_ACCESS_MSG_STATUS_SUCCESS) {
                    LOG_I(mesh_app, "\tStatus    : 0x%04x\n",  evt->status);
                    LOG_I(mesh_app, "\tNetkeyIdx : 0x%06x\n",  evt->netkey_index);
                    LOG_I(mesh_app, "\tIdentity  : 0x%06x\n",  evt->identity);
                } else {
                    LOG_I(mesh_app, "node identity query is failed, error code: %d\n", evt->status);
                }
                mesh_app_get_app_key();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_APPKEY_LIST: {
            SWITCH_DEBUG_EVT_SRC("ConfigAppkeyList",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_APPKEY) {
                const config_client_evt_appkey_list_t *evt = &event->data.appkey_list;
                LOG_I(mesh_app, "\tAppkeyIndexes : ");
                uint16_t i = 0;
                while (i < evt->appkey_count) {
                    LOG_I(mesh_app, "0x%04x, ", evt->appkey_indexes[i]);
                    i++;
                }
                LOG_I(mesh_app, "\n");
                mesh_app_get_lpn_poll_timeout();
            }
            break;
        }
        case CONFIG_CLIENT_EVT_LPN_POLL_TIMEOUT_STATUS: {
            SWITCH_DEBUG_EVT_SRC("ConfigLPNPollTimeoutStatus",  msg->meta_data.src_addr);
            if (g_query_info_state == MESH_APP_QUERY_INFO_STATE_GET_LPN_POLL_TIMEOUT) {
                const config_client_evt_lpn_poll_timeout_status_t *evt = &event->data.lpn_poll_timeout_status;
                LOG_I(mesh_app, "\tLpnAddress  : 0x%04x\n",  evt->lpn_address);
                LOG_I(mesh_app, "\tPollTimeout : 0x%06x\n",  evt->poll_timeout);
                g_query_info_state = MESH_APP_QUERY_INFO_STATE_DONE;
                LOG_I(mesh_app, "all node info query is done\n");
            }
            break;
        }
        default: {
            return;
        }

    }
}


void mesh_app_set_remote_node_info(uint16_t dst_addr)
{
    uint16_t src_addr = bt_mesh_model_get_element_address(g_config_client_element_index);

    if (0 == bt_mesh_model_configuration_client_set_beacon(BT_MESH_FEATURE_STATE_ENABLED,
            src_addr,
            dst_addr,
            bt_mesh_config_get_default_ttl(),
            BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {
        if (0 == bt_mesh_model_configuration_client_set_default_ttl(3,
                src_addr,
                dst_addr,
                bt_mesh_config_get_default_ttl(),
                BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {
            if (0 == bt_mesh_model_configuration_client_set_relay(BT_MESH_FEATURE_STATE_ENABLED,
                    0x01,
                    0x05,
                    src_addr,
                    dst_addr,
                    bt_mesh_config_get_default_ttl(),
                    BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {
                if (0 == bt_mesh_model_configuration_client_set_node_identity(BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX,
                        BT_MESH_FEATURE_STATE_ENABLED,
                        src_addr,
                        dst_addr,
                        bt_mesh_config_get_default_ttl(),
                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {

                    if (0 == bt_mesh_model_configuration_client_set_network_transmit(0x01, 0x05, src_addr, dst_addr,
                            bt_mesh_config_get_default_ttl(),
                            BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {

                        bt_mesh_heartbeat_publication_param_t publication;
                        publication.destination = dst_addr;
                        publication.count_log = 2;
                        publication.period_log = 5;
                        publication.ttl = 2;
                        publication.features = 1;
                        publication.netkey_index = 0;

                        if (0 == bt_mesh_model_configuration_client_set_heartbeat_publication(&publication, src_addr, dst_addr,
                                bt_mesh_config_get_default_ttl(),
                                BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {

                            bt_mesh_heartbeat_subscription_param_t subscription;
                            subscription.source = src_addr;
                            subscription.destination = dst_addr;
                            subscription.period_log = 17;

                            if (0 == bt_mesh_model_configuration_client_set_heartbeat_subscription(&subscription, src_addr, dst_addr,
                                    bt_mesh_config_get_default_ttl(),
                                    BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {
                                uint16_t publish_addr = src_addr;

                                bt_mesh_model_publication_param_t state;
                                state.element_address = dst_addr;
                                state.publish_address.type = bt_mesh_utils_get_addr_type(publish_addr);
                                state.publish_address.value = publish_addr;
                                state.appkey_index = 0x123;
                                state.friendship_credential_flag = false;
                                state.publish_ttl = 1;
                                state.publish_period = 63;
                                state.retransmit_count = 7;
                                state.retransmit_interval_steps = 1;
                                state.model_id = 0x00001000;

                                if (0 == bt_mesh_model_configuration_client_set_model_publication(&state, src_addr, dst_addr,
                                        bt_mesh_config_get_default_ttl(),
                                        BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {
                                    bt_mesh_address_t address;
                                    uint16_t addr = dst_addr;
                                    uint16_t element_addr = dst_addr;
                                    address.type = bt_mesh_utils_get_addr_type(addr);
                                    address.value = addr;
                                    address.virtual_uuid = NULL;

                                    if (0 == bt_mesh_model_configuration_client_add_model_subscription(
                                                element_addr, address, 0x00001000, src_addr, dst_addr,
                                                bt_mesh_config_get_default_ttl(),
                                                BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX)) {
                                        LOG_I(mesh_app, "node info set is done\n");
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    LOG_I(mesh_app, "node info set error!\n");
}



