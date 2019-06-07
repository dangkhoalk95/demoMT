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
 
#include "ble_dtp.h"
#include "bt_spp.h"
#include "syslog.h"
#include "bioband_bt_common.h"
#include "bt_callback_manager.h"
#include "fota_bt_common.h"


#ifdef SENSOR_BTSPP
#include "sensor_bt_spp_server.h"
extern const bt_sdps_record_t sensor_bt_spp_sdp_record;
extern sensor_bt_spp_cntx_t* sensor_bt_spp_context_p;
extern bt_status_t sensor_bt_spp_event_handler(bt_msg_type_t msg, bt_status_t status, void *parameter);
#endif /*SENSOR_BTSPP*/
#ifdef ATCMD_VIA_SPPS  
extern const bt_sdps_record_t bt_spps_atci_sdp_record;
extern uint32_t bt_spps_atci_handle;
extern bt_status_t bt_spps_atci_event_callback(bt_msg_type_t msg, bt_status_t status, void *param);
#endif /*ATCMD_VIA_SPPS*/
extern bt_status_t bio_bt_gap_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff);
//extern bt_status_t ble_dtp_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);

extern const bt_sdps_record_t fota_spp_sdp_record;
 

log_create_module(BIOBAND_COMMON, PRINT_LEVEL_INFO);
#if 0
/*implement SDP Record weak functions bt_sdps_get_customized_record*/
static const bt_sdps_record_t *bio_bt_sdps_spp_record[] = {
#ifdef SENSOR_BTSPP
    &sensor_bt_spp_sdp_record,
#endif /*SENSOR_BTSPP*/
#ifdef ATCMD_VIA_SPPS    
    &bt_spps_atci_sdp_record,
#endif /*SENSOR_BTSPP*/
    &fota_spp_sdp_record
};

uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
{
    *record_list = bio_bt_sdps_spp_record;
    return sizeof(bio_bt_sdps_spp_record) / sizeof(bt_sdps_record_t*);
}
#endif
/*Dispatch spp event to the right app*/
bt_status_t bioband_spp_event_dispatch_callback(bt_msg_type_t msg, bt_status_t status, void *buff) 
{
    uint8_t app_type = 0; /*1: sensor app; 2: atci app*/
    //LOG_I(BIOBAND_COMMON, ":msg:%x, status:%x", msg, status);
    switch (msg) {
        case BT_SPP_CONNECT_IND:
            {
                bt_spp_connect_ind_t * con_ind = (bt_spp_connect_ind_t *)buff;
                if (con_ind->local_server_id == BT_SPP_SERVER_ID_START) {
                    app_type = 1;
                } else if (con_ind->local_server_id == (BT_SPP_SERVER_ID_START + 1)) {
                    app_type = 2;
                } else {
                    LOG_I(BIOBAND_COMMON, "Not support server id: %d", con_ind->local_server_id);
                }
            }
        break;
            
        case BT_SPP_CONNECT_CNF:
            {
                bt_spp_connect_cnf_t * con_cnf = (bt_spp_connect_cnf_t *)buff;
                if (con_cnf->handle == BT_SPP_INVALID_HANDLE) {
                    break; 
                }
            #ifdef SENSOR_BTSPP
                if (con_cnf->handle == sensor_bt_spp_context_p->spp_handle) {
                    app_type = 1;
                } else
            #endif /*SENSOR_BTSPP*/
            #ifdef ATCMD_VIA_SPPS
                 if (con_cnf->handle == bt_spps_atci_handle) {
                    app_type = 2;
                } else
             #endif /*ATCMD_VIA_SPPS*/   
                {
                    LOG_I(BIOBAND_COMMON, "Not support handle: 0x%x", con_cnf->handle);
                }
            }
        break;

        case BT_SPP_DISCONNECT_IND:
            {
                bt_spp_disconnect_ind_t * dis_ind = (bt_spp_disconnect_ind_t *)buff;
                if (dis_ind->handle == BT_SPP_INVALID_HANDLE) {
                    break; 
                }
            #ifdef SENSOR_BTSPP
                if (dis_ind->handle == sensor_bt_spp_context_p->spp_handle) {
                    app_type = 1;
                } else
            #endif /*SENSOR_BTSPP*/
            #ifdef ATCMD_VIA_SPPS
                if(dis_ind->handle == bt_spps_atci_handle) {
                    app_type = 2;
                } else
            #endif /*ATCMD_VIA_SPPS*/ 
                {
                    LOG_I(BIOBAND_COMMON, "Not support handle: 0x%x", dis_ind->handle);
                }
            } 
        break;

        case BT_SPP_READY_TO_SEND_IND:
            {
                bt_spp_ready_to_send_ind_t * send_ind = (bt_spp_ready_to_send_ind_t *)buff;
                if (send_ind->handle == BT_SPP_INVALID_HANDLE) {
                    break; 
                }
            #ifdef SENSOR_BTSPP
                if (send_ind->handle == sensor_bt_spp_context_p->spp_handle) {
                    app_type = 1;
                } else
            #endif /*SENSOR_BTSPP*/
            #ifdef ATCMD_VIA_SPPS
                if(send_ind->handle == bt_spps_atci_handle) {
                    app_type = 2;
                } else
            #endif /*ATCMD_VIA_SPPS*/
                {
                    LOG_I(BIOBAND_COMMON, "Not support handle: 0x%x", send_ind->handle);
                } 
            }
        break;
            
        case BT_SPP_DATA_RECEIVED_IND:
            {
                bt_spp_data_received_ind_t * data_ind = (bt_spp_data_received_ind_t *)buff;
                if (data_ind->handle == BT_SPP_INVALID_HANDLE) {
                    break; 
                }
            #ifdef SENSOR_BTSPP
                if (data_ind->handle == sensor_bt_spp_context_p->spp_handle) {
                    app_type = 1;
                } else
            #endif /*SENSOR_BTSPP*/
            #ifdef ATCMD_VIA_SPPS
                if(data_ind->handle == bt_spps_atci_handle) {
                    app_type = 2;
                } else 
            #endif /*ATCMD_VIA_SPPS*/
                {
                    LOG_I(BIOBAND_COMMON, "Not support handle: 0x%x", data_ind->handle);
                }                         
            }
        break;
            
        default:
        break;
    }
    
    //LOG_I(BIOBAND_COMMON, "app_type:%d",app_type);

    if (app_type == 1) {
        #ifdef SENSOR_BTSPP
        return sensor_bt_spp_event_handler(msg,status,buff);
        #endif /*SENSOR_BTSPP*/
    } else if (app_type == 2) {
        #ifdef ATCMD_VIA_SPPS
        return bt_spps_atci_event_callback(msg,status,buff);
        #endif /*ATCMD_VIA_SPPS*/
    } else {
        LOG_I(BIOBAND_COMMON, "Not spp app.");
    }
    return BT_STATUS_SUCCESS;
}


bt_status_t bioband_bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    if (msg >= BT_MODULE_SPP && msg < BT_MODULE_AVRCP) {
        return bioband_spp_event_dispatch_callback(msg,status,buff);
    } else {
        ble_dtp_event_callback(msg, status, buff);
        bio_bt_gap_event_handler(msg, status, buff);
    }
    return BT_STATUS_SUCCESS;
}


bt_gap_config_t bioband_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
    .cod           = 0x240404, /* It defines the class of the local device. */
    .device_name   = {"BIOBAND_DEMO"}, /* It defines the name of the local device with '\0' ending. */
};


const bt_gap_config_t* bioband_bt_gap_get_local_configuration(void)
{
    return &bioband_gap_config;
}



void bioband_bt_callback_init()
{
    bt_callback_manager_register_callback(bt_callback_type_app_event, 0xFFFFFFFF, bioband_bt_app_event_callback);
    bt_callback_manager_register_callback(bt_callback_type_gap_get_link_key,0, bioband_bt_gap_get_link_key);    
    bt_callback_manager_register_callback(bt_callback_type_gap_get_local_configuration, 0, bioband_bt_gap_get_local_configuration);

    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig, 0, dtp_bt_gap_le_get_local_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_bonding_info, 0, dtp_bt_gap_le_get_bonding_info);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_pairing_config, 0, dtp_bt_gap_le_get_pairing_config);

#ifdef SENSOR_BTSPP
    bt_callback_manager_add_sdp_customized_record(&sensor_bt_spp_sdp_record);
#endif

#ifdef ATCMD_VIA_SPPS
    bt_callback_manager_add_sdp_customized_record(&bt_spps_atci_sdp_record);
#endif

    bt_callback_manager_add_sdp_customized_record(&fota_spp_sdp_record);
}

