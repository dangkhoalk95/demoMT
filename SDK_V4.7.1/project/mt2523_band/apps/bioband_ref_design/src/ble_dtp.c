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
#include <stdbool.h>
#include <stdio.h>
#include "syslog.h"
#include "bt_hci.h"
#include "bt_gap_le.h"
#include "ble_dtp.h"
#include "bt_gattc.h"
#include "bt_gatts.h"
#include "FreeRTOS.h"
#include "portable.h"


#define DTP_SERVICE_UUID        (0x18AA)
#define DTP_CHAR_VALUE_HANDLE   (0x0003)
#define DTP_CHAR_UUID           (0x2AAA)


static bt_gap_le_smp_pairing_config_t pairing_config = {//mitm, bond, oob
    .maximum_encryption_key_size = 16,
    .io_capability = BT_GAP_LE_SMP_NO_INPUT_NO_OUTPUT,
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_BONDING,
    .oob_data_flag = BT_GAP_LE_SMP_OOB_DATA_NOT_PRESENTED,
    .initiator_key_distribution = BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN,
    .responder_key_distribution = BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN,
};

static bt_gap_le_local_key_t local_key = {
    .encryption_info.ltk = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc8, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf },
    .master_id.ediv = 0x1005,
    .master_id.rand = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 },
    .identity_info.irk = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf },
    .signing_info.csrk = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf }
};

static bt_gap_le_bonding_info_t  bonding_info;
static bool sc_only = false;
static bt_gap_le_local_config_req_ind_t local_config;


static ble_dtp_t dtp_cntx;
static ble_dtp_t *p_dtp = &dtp_cntx;

const bt_uuid_t DTP_CHAR_UUID128 = BT_UUID_INIT_WITH_UUID16(DTP_CHAR_UUID);
log_create_module(DTP, PRINT_LEVEL_INFO);


void ble_dtp_register(dtp_event_cb_t callback)
{
    memset(p_dtp, 0, sizeof(ble_dtp_t));
    
    if (callback) {
        p_dtp->event_cb = callback;
    }
}

static bt_status_t ble_dtp_send_indication(uint16_t buff_len, uint8_t *buff)
{
    bt_status_t status;
    uint16_t mtu_size = bt_gattc_get_mtu(p_dtp->conn_handle);
    uint16_t send_len = (buff_len < (mtu_size - 3))? buff_len : (mtu_size - 3);;
    bt_gattc_charc_value_notification_indication_t *req = 
                    (bt_gattc_charc_value_notification_indication_t *)pvPortMalloc(send_len + sizeof(bt_gattc_charc_value_notification_indication_t));
    
    req->attribute_value_length = 3 + send_len;
    req->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_INDICATION;
    req->att_req.handle = DTP_CHAR_VALUE_HANDLE;
    memcpy(req->att_req.attribute_value, buff, send_len);
    status = bt_gatts_send_charc_value_notification_indication(p_dtp->conn_handle, req);

    vPortFree(req);
    
    if (status == BT_STATUS_SUCCESS) {
        p_dtp->left_len = buff_len - send_len;
    } 
    
    LOG_I(DTP, "send_len = %d, left_len = %d, p_buff = 0x%x, status = %d\n",
        send_len, p_dtp->left_len, buff, status);
    return status;
}


ble_dtp_err_code_t ble_dtp_send_data(uint16_t buff_len, uint8_t *buff)
{    
    ble_dtp_err_code_t status;
    
    LOG_I(DTP, "[DTP]ble_dtp_send_data, len = %d\r\n", buff_len);

    if(p_dtp->indicate_enable == 0x0002) {
        if (!p_dtp->p_buff && !p_dtp->len) {
            //dtp is free
            if (buff && buff_len > 0) {  
                //send data

                p_dtp->p_buff = (uint8_t *)pvPortMalloc(buff_len);
                memcpy(p_dtp->p_buff, buff, buff_len);
                p_dtp->len = buff_len;
                p_dtp->left_len = buff_len;

                if (BT_STATUS_SUCCESS == ble_dtp_send_indication(buff_len, buff)) {                   
                    status = DTP_SUCCESS;
                } else {
                    status = DTP_FAIL;
                }
            } else {
                status = DTP_FAIL_INVALID_DATA;
            }
        } else {
            status = DTP_FAIL_BUSY;
        }
    } else {
        status = DTP_FAIL_DIABLED;
    }

    return status;
}

static uint32_t ble_dtp_char_value_callback (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(DTP, "[DTP]ble_dtp_char_value_callback: rw = %d, size = %d\r\n", rw, size);

    if (rw == BT_GATTS_CALLBACK_WRITE) {
        dtp_event_t evt;

        if (p_dtp->event_cb) { 
            evt.event_id = DTP_EVENT_DATA_RECEIVED;
            evt.result = DTP_SUCCESS;
            evt.data = (uint8_t *)data;
            evt.length = size;
            p_dtp->event_cb(&evt);
        }
    } else {
        return 0;
    }

    return (uint32_t)size;
}


static uint32_t ble_dtp_client_config_callback (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(DTP, "[DTP]ble_dtp_client_config_callback: rw = %d, handle = 0x%x, size = %d\r\n", rw, handle, size);
    
    if (rw == BT_GATTS_CALLBACK_WRITE) {
        dtp_event_t evt;
        
        if (size != sizeof(p_dtp->indicate_enable)){ //Size check
            return 0;
        }
        p_dtp->indicate_enable = *(uint16_t*)data;

        if (p_dtp->indicate_enable == 0x0002) {
            //enabled
            LOG_I(DTP, "[DTP]DTP enabled\r\n");
            if (p_dtp->event_cb) { 
                evt.event_id = DTP_EVENT_ENABLED;
                evt.result = DTP_SUCCESS;
                p_dtp->event_cb(&evt);
            }
        } else {
            if (size!=0){
                memcpy(data, &p_dtp->indicate_enable, sizeof(p_dtp->indicate_enable));
            }
        }
    }
    return sizeof(p_dtp->indicate_enable);

}


static void ble_dtp_set_adv(void)
{
    bt_hci_cmd_le_set_advertising_parameters_t adv_param = {
        .advertising_interval_min = 0x0800,
        .advertising_interval_max = 0x0800,
        .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .own_address_type = BT_ADDR_RANDOM,
        .advertising_channel_map = 7,
        .advertising_filter_policy = 0
    };
 
    bt_hci_cmd_le_set_advertising_enable_t enable;
    bt_hci_cmd_le_set_advertising_data_t adv_data = {
        .advertising_data_length = 12,
        .advertising_data = "DDDDDBIOBAND",
    };
     
    adv_data.advertising_data[0] = 2; 
    adv_data.advertising_data[1] = BT_GAP_LE_AD_TYPE_FLAG;
    adv_data.advertising_data[2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
    adv_data.advertising_data[3] = 8;
    adv_data.advertising_data[4] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
     
    enable.advertising_enable = BT_HCI_ENABLE;
 
    bt_gap_le_set_advertising(&enable, &adv_param, &adv_data, NULL);
     
    bt_gatts_set_max_mtu(158);
 
}
 

static bt_status_t ble_dtp_update_connection_interval(bt_handle_t connection_handle, ble_dtp_conn_speed_t conn_speed)
{
    bt_hci_cmd_le_connection_update_t conn_params;
 
    conn_params.supervision_timeout = 0x0258;            /** TBC: 6000ms : 600 * 10 ms. */
    conn_params.connection_handle = connection_handle;
     
    switch (conn_speed) {
        case BLE_DTP_CONN_HIGH_SPEED: {
            conn_params.conn_interval_min = 0x0010;/** TBC: 20ms : 16 * 1.25 ms. */
            conn_params.conn_interval_max = 0x0020;/** TBC: 40ms : 32 * 1.25 ms. */
            conn_params.conn_latency = 0;
        }
        break;
        case BLE_DTP_CONN_LOW_POWER:
        case BLE_DTP_CONN_DEFAULT: {
            conn_params.conn_interval_min = 0x0130;/** TBC: 380ms : 304 * 1.25 ms. */
            conn_params.conn_interval_max = 0x0140;/** TBC: 400ms : 320 * 1.25 ms. interval = 303 on ios 9.2*/
            conn_params.conn_latency = 4;
        }
        break;
        default: {
            conn_params.conn_interval_min = 0x0130;/*TBC: 380ms : 304 * 1.25 ms*/
            conn_params.conn_interval_max = 0x0140;/*TBC: 400ms : 320 * 1.25 ms*/
            conn_params.conn_latency = 4;
        }
        break;
    }
 
    return bt_gap_le_update_connection_parameter(&conn_params);
}


bt_status_t ble_dtp_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    //LOG_I(DTP, "ble_dtp_event_callback: msg = 0x%x\r\n", msg);
     
    switch (msg) {
        case BT_POWER_ON_CNF:
            {
                LOG_I(DTP, "BT_POWER_ON_CNF\r\n");
                ble_dtp_set_adv();
            }
            break;
 
        case BT_GATTC_CHARC_VALUE_CONFIRMATION:
            {     
                LOG_I(DTP, "BT_GATTC_CHARC_VALUE_CONFIRMATION, indication_cnf\r\n");

                if (p_dtp->left_len > 0) {
                    ble_dtp_send_indication(p_dtp->left_len, p_dtp->p_buff + (p_dtp->len - p_dtp->left_len));
                } else {
                    //send finish
                    dtp_event_t evt;  

                    memset(&evt, 0, sizeof(dtp_event_t));

                    vPortFree(p_dtp->p_buff);
                    p_dtp->p_buff = NULL;
                    p_dtp->len = 0;
                    p_dtp->left_len = 0;

                    if (p_dtp->event_cb) {
                        evt.event_id = DTP_EVENT_DATA_SENT;
                        evt.result = DTP_SUCCESS;
                        (p_dtp->event_cb)(&evt);
                    }
                }
             }
             break;
             
        case BT_GAP_LE_CONNECT_IND:
            {
                bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buff;
 
                p_dtp->conn_handle = connection_ind->connection_handle;
 
                ble_dtp_update_connection_interval(connection_ind->connection_handle, BLE_DTP_CONN_HIGH_SPEED);
                LOG_I(DTP, "BT_GAP_LE_CONNECT_IND\r\n");
            }
            break;
 
        case BT_GAP_LE_DISCONNECT_IND:
            ble_dtp_set_adv();
            LOG_I(DTP, "BT_GAP_LE_DISCONNECT_IND\r\n");
            break;
 
        default:
            break;
    }
 
    return BT_STATUS_SUCCESS;
}


BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_dtp_primary_service, DTP_SERVICE_UUID);
 
BT_GATTS_NEW_CHARC_16(bt_if_dtp_char, BT_GATT_CHARC_PROP_WRITE | BT_GATT_CHARC_PROP_INDICATE, 
                        DTP_CHAR_VALUE_HANDLE, DTP_CHAR_UUID);
 
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_dtp_char_value, DTP_CHAR_UUID128,
                        BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, ble_dtp_char_value_callback);
 
BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_dtp_client_config,
                                  BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                  ble_dtp_client_config_callback);
 
static const bt_gatts_service_rec_t *bt_if_dtp_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_dtp_primary_service,
    (const bt_gatts_service_rec_t *) &bt_if_dtp_char,
    (const bt_gatts_service_rec_t *) &bt_if_dtp_char_value,
    (const bt_gatts_service_rec_t *) &bt_if_dtp_client_config
};


const bt_gatts_service_t bt_if_dtp_service = {
    .starting_handle = 0x0001,
    .ending_handle = 0x00004,
    .required_encryption_key_size = 0,
    .records = bt_if_dtp_service_rec
};


 //server collects all service
const bt_gatts_service_t *bt_if_gatt_server[] = {
    &bt_if_dtp_service,
    NULL
};
     

 //When GATTS get req from remote client, GATTS will call bt_get_gatt_server() to get application's gatt service DB.
 //You have to return the DB(bt_gatts_service_t pointer) to gatts stack.
#if 0
const bt_gatts_service_t **bt_get_gatt_server()
{
    //LOG_I(DTP, "[DTP]bt_get_gatt_server\r\n");
    return bt_if_gatt_server;
}
#endif

bt_gap_le_local_config_req_ind_t *dtp_bt_gap_le_get_local_config(void)
{
    LOG_I(DTP, "[DTP]bt_gap_le_get_local_config\r\n");

    local_config.local_key_req = &local_key;
    local_config.sc_only_mode_req = sc_only;
 
    return &local_config;
}

bt_gap_le_bonding_info_t *dtp_bt_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    LOG_I(DTP, "[DTP]bt_gap_le_get_bonding_info\r\n");
    return &bonding_info;
}

bt_status_t dtp_bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    LOG_I(DTP, "[DTP]bt_gap_le_get_pairing_config\r\n");
    ind->pairing_config_req = pairing_config;

    return BT_STATUS_SUCCESS;
}


