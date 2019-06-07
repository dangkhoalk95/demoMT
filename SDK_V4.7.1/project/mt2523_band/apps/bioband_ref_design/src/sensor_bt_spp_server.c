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


/*bt spp include */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sensor_demo.h"
#include "syslog.h"
#include "sensor_bt_spp_server.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "timer.h"
#include "semphr.h"
#include "portmacro.h"

#ifdef SENSOR_BTSPP
log_create_module(SENSOR_SPP, PRINT_LEVEL_INFO);

/*bt spp include */
#include "bt_spp.h"
sensor_bt_spp_cntx_t sensor_bt_spp_context = {0};
sensor_bt_spp_cntx_t* sensor_bt_spp_context_p = &sensor_bt_spp_context;
uint32_t bt_sensor_mutex_id = 0xFFFFFFFF;
/****************************************************************************
 * ROMable data
 *Sensor spp sdp record - start
 ****************************************************************************/
#define SENSOR_BT_SPP_UUID    0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,   \
                                0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB

static const uint8_t sensor_bt_spp_service_class_id[] =
{
    BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
    BT_SPP_SDP_ATTRIBUTE_UUID(SENSOR_BT_SPP_UUID)
};

static const uint8_t sensor_bt_spp_protocol_descriptor_list[] =
{
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_SPP_SERVER_ID_START)
};

static const uint8_t sensor_bt_spp_browse_group[] =
{
    BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
};
    
static const uint8_t sensor_bt_spp_language[] =
{
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE
};

static const uint8_t sensor_bt_spp_service_name[] =
{
    BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(10),
    'S', 'E', 'N', 'S', 'O', 'R', '-', 'S', 'P', 'P'
};

static const bt_sdps_attribute_t sensor_bt_spp_sdp_attributes[] =
{
    /* Service Class ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(sensor_bt_spp_service_class_id),
    /* Protocol Descriptor List attribute */
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(sensor_bt_spp_protocol_descriptor_list),
    /* Public Browse Group Service */
    BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(sensor_bt_spp_browse_group),
    /* Language Base ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(sensor_bt_spp_language),
    /* Serial Port Profile Service Name */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(sensor_bt_spp_service_name)
};

const bt_sdps_record_t sensor_bt_spp_sdp_record = 
{
    .attribute_list_length = sizeof(sensor_bt_spp_sdp_attributes),
    .attribute_list = sensor_bt_spp_sdp_attributes,
};

/*********************************************************************
 *Sensor spp sdp record -end
 *********************************************************************/


#ifdef RECEIVE_DATA_ENABLE
spps_receive_data_cb_t recev_data_cb;
#define SPPS_READ_DATA_MAX_LEN  200
log_create_module(SPPS, PRINT_LEVEL_INFO);
void sensor_bt_spp_register(spps_receive_data_cb_t *callback)
{
    recev_data_cb = callback;
    return;
}
void sensor_bt_spp_receive_data(bt_status_t status, bt_spp_data_received_ind_t *param)
{
    uint16_t read_len = 0;
    uint8_t read_buff[SPPS_READ_DATA_MAX_LEN] = {0};

    //read_len = bt_spp_read(param->port, read_buff, SPPS_READ_DATA_MAX_LEN);

    //LOG_I(SENSOR_SPP, "read_len = %d", read_len);

    if (recev_data_cb) {
        recev_data_cb(read_len, read_buff);
    }
}
#endif /*RECEIVE_DATA_ENABLE*/

uint32_t bt_spp_sensor_create_mutex(void)
{
    return (uint32_t)xSemaphoreCreateRecursiveMutex();
}

void bt_spp_sensor_take_mutex(uint32_t mutex_id)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex_id, portMAX_DELAY);
}

void bt_spp_sensor_give_mutex(uint32_t mutex_id)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex_id);
}

void bt_spp_sensor_delete_mutex(uint32_t mutex_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)mutex_id);
}

void sensor_bt_spp_context_reset(void)
{
    LOG_I(SENSOR_SPP, "reset context");
    sensor_bt_spp_context_p->is_connected = false;
    sensor_bt_spp_context_p->spp_handle = BT_SPP_INVALID_HANDLE;
    sensor_bt_spp_context_p->max_packet_size = 0;
}
void sensor_bt_spp_connect_ind(bt_status_t status, bt_spp_connect_ind_t *parameter)
{
    sensor_bt_spp_context_p->spp_handle = parameter->handle;
    bt_status_t result =  bt_spp_connect_response(sensor_bt_spp_context_p->spp_handle, true);
    if(result != BT_STATUS_SUCCESS) {
        LOG_I(SENSOR_SPP, "response error result: 0x%x", result);
    }
    return;
}
void sensor_bt_spp_connect_cnf(bt_status_t status, bt_spp_connect_cnf_t *parameter)
{
    LOG_I(SENSOR_SPP, "connect cnf result: 0x%x", status);
    if (status== BT_STATUS_SUCCESS) { /**< connect sucessfully*/
        sensor_bt_spp_context_p->is_connected = true;
        sensor_bt_spp_context_p->max_packet_size = (parameter->max_packet_length & 0xFFC0);
        if (sensor_bt_spp_context_p->max_packet_size > 1024) {
            sensor_bt_spp_context_p->max_packet_size = 1024;
        }
        LOG_I(SENSOR_SPP, "max cache size: %d", sensor_bt_spp_context_p->max_packet_size);
    }  else {

    }
    return;
}
void sensor_bt_spp_disconnect_ind(bt_status_t status, bt_spp_disconnect_ind_t *parameter)
{
    LOG_I(SENSOR_SPP, "disconnect result: 0x%x", status);
    sensor_bt_spp_context_reset();
    return;
}

bt_status_t sensor_bt_spp_event_handler(bt_msg_type_t msg, bt_status_t status, void *parameter)
{
    //LOG_I(SENSOR_SPP, ":msg:%x, status:%x", msg, status);
    switch (msg) {
        case BT_SPP_CONNECT_IND:
            sensor_bt_spp_connect_ind(status, (bt_spp_connect_ind_t *)parameter);
            break;
        case BT_SPP_CONNECT_CNF:
            sensor_bt_spp_connect_cnf(status, (bt_spp_connect_cnf_t *) parameter);
            break;
        case BT_SPP_DISCONNECT_IND:
            sensor_bt_spp_disconnect_ind(status, (bt_spp_disconnect_ind_t *) parameter);
            break;
        case BT_SPP_READY_TO_SEND_IND:
            //sensor_bt_spp_ready_to_write_ind(status, (bt_spp_ready_to_write_ind_t*) parameter);
        break;
#ifdef RECEIVE_DATA_ENABLE
        case BT_SPP_DATA_RECEIVED_IND:
            sensor_bt_spp_receive_data(status, (bt_spp_data_received_ind_t *)parameter);
            break;
#endif
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t sensor_bt_spp_send_data(int32_t sensor_type, uint32_t handle, void *packet, uint16_t packet_size)
{
    static uint8_t send_buff[1024];
    static uint16_t curr_size = 0;
    bt_status_t status = BT_STATUS_SUCCESS;
    if (bt_sensor_mutex_id == 0xFFFFFFFF) {
        bt_sensor_mutex_id = bt_spp_sensor_create_mutex();
    }
    bt_spp_sensor_take_mutex(bt_sensor_mutex_id);
    memcpy(send_buff+curr_size, packet, packet_size);
    curr_size += packet_size;
    if  ((curr_size >= sensor_bt_spp_context_p->max_packet_size) ||
        (sensor_type == SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR)  ||
        (sensor_type == SENSOR_TYPE_BLOOD_PRESSURE_MONITOR)  || 
        (sensor_type == SENSOR_TYPE_INTERNAL_BEATPOS)  || 
        (sensor_type == SENSOR_TYPE_INTERNAL_PWTT)) {
        status = bt_spp_send(handle, send_buff, curr_size);
        if (status != BT_STATUS_SUCCESS) {
            LOG_I(SENSOR_SPP, "send data, sensor:%d,  size: %d, result:0x%08x", sensor_type, curr_size, status);
        }
        curr_size = 0;
    }
    bt_spp_sensor_give_mutex(bt_sensor_mutex_id);
    return status;
}

int send_sensor_data_via_btspp(int32_t magic, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp)
{
#if 0
    int real_write_len = 0;
    int spps_data_length = 28;
    int ret = 0;
    if (bt_spp_is_connected) {
        int32_t DATA[7];
        DATA[0] = magic;
        DATA[1] = sensor_type;
        DATA[2] = x;
        DATA[3] = y;
        DATA[4] = z;
        DATA[5] = status;
        DATA[6] = time_stamp;
        real_write_len = bt_spp_write(port_id, (void *)DATA, spps_data_length);
        if (real_write_len != 28) {
//            bt_spp_is_connected = false;
//            printf("buffer full\r\n");
            ret = -1;
        }
        memset(DATA, 0, 28);
    }
#endif
    int spps_data_length = 64;
    int ret = 0;
    int i = 0;
    if (sensor_bt_spp_context_p->is_connected) {
        int32_t DATA[16];
        DATA[0] = magic;
        DATA[1] = sensor_type;
        DATA[2] = 0;//seq
        DATA[3] = x;
        DATA[4] = y;
        DATA[5] = z;
        DATA[6] = status;
        DATA[7] = time_stamp;
        for (i = 8; i < 16; i++) {
            DATA[i] = 12345;//reserved
        }
        bt_status_t status = sensor_bt_spp_send_data(sensor_type,sensor_bt_spp_context_p->spp_handle, (void *)DATA, spps_data_length);
        if (status == BT_STATUS_SUCCESS) {
            //send data succussfully.
        } else if (status == BT_STATUS_SPP_TX_NOT_AVAILABLE) {
            LOG_I(SENSOR_SPP, "tx not avalibale.");
            ret = -1;
        } else {
            LOG_I(SENSOR_SPP, "send data failed:0x%x", status);
            ret = -2;
        }
        memset(DATA, 0, 64);
    }
    return ret;
}

int send_sensor_16_datas_via_btspp(int32_t magic, int32_t sensor_type, int32_t seq, int32_t data_buf[], int32_t data_size, int32_t reserve)
{
    int spps_data_length = 64;
    int ret = 0;
    int i = 0;
    if (sensor_bt_spp_context_p->is_connected) {
        int32_t DATA[16];
        DATA[0] = magic;
        DATA[1] = sensor_type;
        DATA[2] = seq;

        if (data_size > 12) {
            data_size = 12;
        }
        for (i = 3; i < (data_size+3); i++) {
            DATA[i] = data_buf[i - 3];
        }
        while (i<=15) {
            DATA[i] = reserve;
            i++;
        }

        bt_status_t status = sensor_bt_spp_send_data(sensor_type, sensor_bt_spp_context_p->spp_handle, (void *)DATA, spps_data_length);
        if (status == BT_STATUS_SUCCESS) {
            //send data succussfully.
        } else if (status == BT_STATUS_SPP_TX_NOT_AVAILABLE) {
            LOG_I(SENSOR_SPP, "tx not avalibale.");
            ret = -1;
        } else {
            LOG_I(SENSOR_SPP, "send data failed:0x%x", status);
            ret = -2;
        }
        memset(DATA, 0, 64);
    }
    return ret;
}
#else /*SENSOR_BTSPP*/
int send_sensor_data_via_btspp(int32_t number, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp)
{
    return 0;
}

int send_sensor_16_datas_via_btspp(int32_t magic, int32_t sensor_type, int32_t seq, int32_t data_buf[], int32_t data_size, int32_t reserve)
{
    return 0;
}

#endif /*SENSOR_BTSPP*/


