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

#ifdef ATCMD_VIA_SPPS
#include <stdio.h>
#include "bt_spp.h"
#include "bt_type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "atci.h"
#include "task_def.h"
#include "sync_time.h"

typedef struct {
    bt_msg_type_t event_id;
    bt_status_t status;
    uint8_t param[50];
} bt_spps_atci_msg_t;
                   
/****************************************************************************
 * ROMable data
 *bt spps atci sdp record -start
 ****************************************************************************/
#define BT_SPP_ATCI_UUID    0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,   \
                                    0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0x5B
static const uint8_t bt_spps_atci_service_class_id[] =
{
    BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
    BT_SPP_SDP_ATTRIBUTE_UUID(BT_SPP_ATCI_UUID)
};

static const uint8_t bt_spps_atci_protocol_descriptor_list[] =
{
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_SPP_SERVER_ID_START + 1)
};

static const uint8_t bt_spps_atci_browse_group[] =
{
    BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
};
    
static const uint8_t bt_spps_atci_language[] =
{
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE
};

static const uint8_t bt_spps_atci_service_name[] =
{
    BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(8),
    'A', 'T', 'C', 'I', '-', 'S', 'P', 'P'
};

const bt_sdps_attribute_t bt_spps_atci_sdp_attributes[] =
{
    /* Service Class ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_spps_atci_service_class_id),
    /* Protocol Descriptor List attribute */
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_spps_atci_protocol_descriptor_list),
    /* Public Browse Group Service */
    BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(bt_spps_atci_browse_group),
    /* Language Base ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_spps_atci_language),
    /* Serial Port Profile Service Name */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(bt_spps_atci_service_name)
};

const bt_sdps_record_t bt_spps_atci_sdp_record = 
{
    .attribute_list_length = sizeof(bt_spps_atci_sdp_attributes),
    .attribute_list = bt_spps_atci_sdp_attributes,
};
/*********************************************************************
 *bt spps atci sdp record -end
 *********************************************************************/

#define BT_SPPS_ATCI_RX_MAX_LEN  1000
uint32_t bt_spps_atci_handle = BT_SPP_INVALID_HANDLE;
QueueHandle_t bt_spps_atci_queue = NULL;
uint16_t bt_spps_atci_packet_max_length = 0;

log_create_module(SPPS_ATCI, PRINT_LEVEL_INFO);

void atci_def_task(void *param)
{
    LOG_W(atci_biogui, "enter atci_def_task");
    while (1) {
        atci_processing();
    }
}

int8_t bt_spps_atci_send_data(uint32_t len, uint8_t *data)
{
    uint32_t remain_data_len = len;
    LOG_I(SPPS_ATCI, "total len = %d, data =0x%x", len, data);

    if  (bt_spps_atci_handle == BT_SPP_INVALID_HANDLE) {
        LOG_I(SPPS_ATCI, "spp is disconnected.");
        return -1;
    }

    do {
        uint16_t real_sent_len = 0;
        if (remain_data_len >= bt_spps_atci_packet_max_length) {
            real_sent_len = bt_spps_atci_packet_max_length;
        } else {
            real_sent_len = remain_data_len;
        }
        bt_status_t result = bt_spp_send(bt_spps_atci_handle, data + len - remain_data_len, real_sent_len);
        if(result == BT_STATUS_SUCCESS) {
            remain_data_len -= real_sent_len;
        } else if (result == BT_STATUS_SPP_TX_NOT_AVAILABLE) {
            // sleep to wait bt task to free BT TX buffer
            vTaskDelay(500);
        } else {
            LOG_I(SPPS_ATCI, "error result = 0x%x, need check reason.", result);
            return - 2;
        }
    } while (remain_data_len > 0);
   
    return 0;
}

void bt_spps_atci_connect_ind(bt_status_t status, bt_spp_connect_ind_t *param)
{
    bt_spps_atci_handle = param->handle;
    bt_status_t result =  bt_spp_connect_response(bt_spps_atci_handle, true);
    LOG_I(SPPS_ATCI, "connect response, handle: 0x%x, result: 0x%x", bt_spps_atci_handle, result);
    if (result != BT_STATUS_SUCCESS) {
        bt_spps_atci_handle = BT_SPP_INVALID_HANDLE;
        bt_spps_atci_packet_max_length = 0;
    }
    return;
}

void bt_spps_atci_connect_cnf(bt_status_t status, bt_spp_connect_cnf_t *param)
{
    LOG_I(SPPS_ATCI, "connect cnf, handle: 0x%x, status: 0x%x", param->handle, status);
    if (status != BT_STATUS_SUCCESS) {
        bt_spps_atci_handle = BT_SPP_INVALID_HANDLE;
        bt_spps_atci_packet_max_length = 0;
        return;
    }
    bt_spps_atci_packet_max_length = param->max_packet_length;
    LOG_I(SPPS_ATCI, "max_packet_length = %d", bt_spps_atci_packet_max_length);
    atci_init_by_spp(bt_spps_atci_send_data); // TODO: need to confirm if right.
    xTaskCreate(atci_def_task, ATCI_TASK_NAME, ATCI_TASK_STACKSIZE /((uint32_t)sizeof(StackType_t)), NULL, ATCI_TASK_PRIO, NULL);
    return;
}

void bt_spps_atci_disconnect_ind(bt_status_t status, bt_spp_disconnect_ind_t *param)
{
    LOG_I(SPPS_ATCI, "disconnect ind, handle: 0x%x, status: 0x%x", param->handle, status);
    atci_deinit_by_spp();
    bt_spps_atci_handle = BT_SPP_INVALID_HANDLE;
    bt_spps_atci_packet_max_length = 0;
    return;
}

void bt_spps_atci_read_data(bt_status_t status, bt_spp_data_received_ind_t *param)
{
    uint8_t read_buff[BT_SPPS_ATCI_RX_MAX_LEN] = { 0 };
    //LOG_I(SPPS_ATCI, "handle = 0x%x, data length = %d",param->handle ,param->packet_length);
    memcpy(read_buff, param->packet, param->packet_length);
    //relase data to free BT RX buffer.
    bt_spp_release_data(param->packet);
    
    if (!strncmp((const char*)read_buff, SYNC_TIME_CMD_HEADER, strlen(SYNC_TIME_CMD_HEADER))) {
        sync_time_status result;
        uint16_t rsp_len;
        uint8_t *rsp_buf = NULL;
        
        //set sp time to the watch
        result = sync_time_set_time(read_buff, param->packet_length);

        //send response to sp
        rsp_buf = sync_time_create_response(result, &rsp_len);

        if (rsp_buf) {
            bt_spps_atci_send_data(rsp_len, rsp_buf);
        }
    } else {
        atci_receive_data_by_spp(read_buff, (uint32_t)param->packet_length);
    }
    return;
}

void bt_spps_atci_ready_to_send_ind(bt_status_t status, bt_spp_ready_to_send_ind_t *param)
{
    LOG_I(SPPS_ATCI, "handle = 0x%x, status = 0x%x", param->handle, status);
    //no need to handle this event.
    return;
}

void bt_spps_atci_task_event_handler(bt_msg_type_t msg, bt_status_t status, void *param)
{
    //LOG_I(SPPS_ATCI, "msg = 0x%x, status = 0x%x", msg, status);
    switch (msg) {
        case BT_SPP_CONNECT_IND: {
            bt_spps_atci_connect_ind(status, (bt_spp_connect_ind_t *)param);
        }
            break;
        case BT_SPP_CONNECT_CNF: {
            bt_spps_atci_connect_cnf(status, (bt_spp_connect_cnf_t *) param);
        }
            break;
        case BT_SPP_DISCONNECT_IND: {
            bt_spps_atci_disconnect_ind(status, (bt_spp_disconnect_ind_t *) param);
        }
            break;
        case BT_SPP_DATA_RECEIVED_IND: {
            bt_spps_atci_read_data(status, (bt_spp_data_received_ind_t *)param);
        }
            break;
        case BT_SPP_READY_TO_SEND_IND: {
            bt_spps_atci_ready_to_send_ind(status, (bt_spp_ready_to_send_ind_t *) param);
        }
            break;
        default:
            break;
    }
}

bt_status_t bt_spps_atci_event_callback(bt_msg_type_t msg, bt_status_t status, void *param)
{
    bt_spps_atci_msg_t message;

    //LOG_I(SPPS_ATCI, "msg = %x", msg);
    switch (msg) {
        case BT_SPP_CONNECT_IND:
            memcpy((void *) message.param, param, sizeof(bt_spp_connect_ind_t));
            break;

        case BT_SPP_CONNECT_CNF:
            memcpy((void *) message.param, param, sizeof(bt_spp_connect_cnf_t));
            break;

        case BT_SPP_DISCONNECT_IND:
            memcpy((void *) message.param, param, sizeof(bt_spp_disconnect_ind_t));
            break;

        case BT_SPP_READY_TO_SEND_IND:
            memcpy((void *) message.param, param, sizeof(bt_spp_ready_to_send_ind_t));
            break;

        case BT_SPP_DATA_RECEIVED_IND:
        {
            memcpy((void *) message.param, param, sizeof(bt_spp_data_received_ind_t));
            bt_spp_data_received_ind_t *data_ind = (bt_spp_data_received_ind_t *)param;
            bt_spp_hold_data(data_ind->packet);
        }
            break;

        default:
            break;
    }
    message.event_id = msg;
    message.status = status;
    xQueueSend(bt_spps_atci_queue, (void *)&message, 0);
    return BT_STATUS_SUCCESS;
}

void bt_spps_atci_task(void *arg)
{
    bt_spps_atci_msg_t spps_message;

    // Create a queue capable of containing 10 uint32_t values.
    bt_spps_atci_queue = xQueueCreate(100, sizeof(bt_spps_atci_msg_t));
    if (bt_spps_atci_queue == NULL) {
        LOG_I(SPPS_ATCI, "create server queue failed");
        return;
    }

    while (1) {
        if (xQueueReceive(bt_spps_atci_queue, (void *)&spps_message, portMAX_DELAY)) {
            //LOG_I(SPPS_ATCI, "event_id = %x, param = %x", spps_message.event_id, spps_message.param);
            bt_spps_atci_task_event_handler(spps_message.event_id, spps_message.status, (void *)(spps_message.param));
            memset((void *) &spps_message, 0, sizeof(bt_spps_atci_msg_t));
        }
    }
}

void bt_spps_atci_main(void)
{
    TaskHandle_t xCreatedServerTask;
    LOG_I(SPPS_ATCI, "create spps atci task\n");
    xTaskCreate(bt_spps_atci_task, SPPS_ATCI_TASK_NAME, SPPS_ATCI_TASK_STACK_SIZE /((uint32_t)sizeof(StackType_t)), NULL, SPPS_ATCI_TASK_PRIO, &xCreatedServerTask);
}
#endif /*ATCMD_VIA_SPPS*/

