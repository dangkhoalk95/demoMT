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
 
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "FreeRTOS.h"

#include "gnss_app.h"
#include "gnss_bt_bridge.h"

// This option is used to enable GNSS debug log send to smart phone.
// This feature is used to enable send debug log to smart phone via BT.
#ifdef GNSS_SUPPORT_BT_BRIDGE

#include "bt_type.h"
#include "bt_spp.h"
#include "bt_callback_manager.h"

#include "task.h"
#include "task_def.h"
#include "timers.h"
#include "queue.h"

#include "atci.h"
#include "atci_main.h"

#include "gnss_ring_buffer.h"


/****************************** GNSS Data **********************************/
#define GNSS_BT_BRIDGE_RING_BUFFER_SIZE 10240
#define GNSS_BT_BRIDGE_TRY_READ_TIME 80

typedef struct gnss_bt_data_cntx{
    bool is_valid;
    ring_buf_struct_t buff;
    uint8_t ring_buf[GNSS_BT_BRIDGE_RING_BUFFER_SIZE];
    TimerHandle_t time_handle;
}gnss_bt_data_cntx_t;

gnss_bt_data_cntx_t gnss_bt_data;

/**
* @brief       This function used to create ring buffer.
* @return     void
*/
void gnss_bt_data_init(){
    gnss_bt_data.is_valid = true;
    ring_buf_init(&gnss_bt_data.buff, (int8_t*)gnss_bt_data.ring_buf, GNSS_BT_BRIDGE_RING_BUFFER_SIZE);
}

/**
* @brief       This function used to get gnss data from ring buffer.
* @param[out]   buf  Data buffer pointer
* @param[out]   buf_len The data length
* @return      int32_t: The get data size
*/
int32_t gnss_bt_data_get_data(int8_t* buf, int32_t buf_len)
{
    return consume_data(&gnss_bt_data.buff, buf, buf_len);
}

/**
* @brief       This function used to put gnss data to ring buffer
* @param[in]   buf Data buffer pointer
* @param[in]   buf_len The data length
* @return      int32_t The put data size
*/
uint32_t gnss_bt_bridge_send_data(uint8_t *buf, uint32_t buf_len){
    uint32_t put_len = 0;
    
    if (gnss_bt_data.is_valid) {
        put_len = put_data(&gnss_bt_data.buff, (int8_t*)buf, (int32_t)buf_len); 
    } 
    return put_len;
}


/****************************** BT SPP *************************************/
/* SPP client task definition */
#define GNSS_SPP_TASK_NAME "gnss_spp_task"
#define GNSS_SPP_TASK_STACK_SIZE (512*4)
#define GNSS_SPP_TASK_PRIO TASK_PRIORITY_NORMAL

#define GNSS_SPP_MSG_TIMEOUT  (BT_MODULE_TIMER | 0x1000)

typedef struct {
    bt_msg_type_t msg;
    bt_status_t status;  
    uint8_t param[50];
} gnss_spp_message_t;

QueueHandle_t gnss_gnss_spp_queue;

#define GNSS_SPP_SEND_DATA_BUF 256
#define GNSS_SPP_RECEIVED_DATA_BUF 100

typedef struct
{
    uint32_t spp_handle;
    uint16_t max_packet_length;
    bool is_connected;
    /*the buffer for app data that will be sent*/
    uint8_t tx_data_buf[GNSS_SPP_SEND_DATA_BUF];
    uint32_t tx_data_length;
    uint32_t tx_left_length;
    /*app rx data*/
    uint8_t rx_data_buf[GNSS_SPP_RECEIVED_DATA_BUF];
    uint32_t rx_data_length;
} gnss_spp_cntx_t;

#define SPP_CLIENT_STRING "Hello, SPP Server!"
#define SPP_SERVER_STRING "Hello, SPP Client!"

#define GNSS_LOG_SPP_STANDARD_UUID    0x00,0x00,0xEE,0x01,0x00,0x00,0x10,0x00,   \
                                      0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB

#define GNSS_SPP_SERVER_ID            0x07 /**< the SPP server ID of GNSS connection.*/

static const uint8_t gnss_spp_service_class_id[] =
{
    BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
    BT_SPP_SDP_ATTRIBUTE_UUID(GNSS_LOG_SPP_STANDARD_UUID)
};

static const uint8_t gnss_spp_protocol_descriptor_list[] =
{
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(GNSS_SPP_SERVER_ID)
};

static const uint8_t gnss_spp_browse_group[] =
{
    BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
};
 
static const uint8_t gnss_spp_language[] =
{
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE
};

static const uint8_t gnss_spp_service_name[] =
{
    BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(7),
    'G', 'N', 'S', 'S', 'L', 'O', 'G'
};

static const bt_sdps_attribute_t gnss_spp_sdp_attributes[] =
{
    /* Service Class ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(gnss_spp_service_class_id),
    /* Protocol Descriptor List attribute */
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(gnss_spp_protocol_descriptor_list),
    /* Public Browse Group Service */
    BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(gnss_spp_browse_group),
    /* Language Base ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(gnss_spp_language),
    /* Serial Port Profile Service Name */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(gnss_spp_service_name)
};

static const bt_sdps_record_t gnss_spp_sdp_record = 
{
    .attribute_list_length = sizeof(gnss_spp_sdp_attributes),
    .attribute_list = gnss_spp_sdp_attributes,
};

gnss_spp_cntx_t gnss_spp_cntx;
gnss_spp_cntx_t *gnss_spp_cntx_p = &gnss_spp_cntx;

char *gnss_cmd_pmtk299 = "$PMTK299,1*2D\0d\0a";

/**
* @brief       This function used to notify bt spp connect indication.
* @param[in]   status The status of bt spp connection.
* @param[in]   conn_ind_p The data of bt connection information.
* @return      void
*/
void gnss_spp_connect_ind(bt_status_t status, bt_spp_connect_ind_t* conn_ind_p)
{
    GNSSLOGD("[BT]handle:0x%x\r\n", conn_ind_p->handle);
    gnss_spp_cntx.spp_handle = conn_ind_p->handle;
    bt_status_t result = bt_spp_connect_response(gnss_spp_cntx.spp_handle, true);
    GNSSLOGD("[BT]spp server response result: 0x%x\r\n", result);
}

/**
* @brief       This function used to notify bt spp connect status.
* @param[in]   status The status of bt spp connection.
* @param[in]   conn_cnf_p The data of bt connection information.
* @return      void
*/
void gnss_spp_connect_cnf(bt_status_t status, bt_spp_connect_cnf_t* conn_cnf_p)
{
    GNSSLOGD("[BT]handle:0x%x, max_packet_length: %d\r\n", conn_cnf_p->handle, conn_cnf_p->max_packet_length);
    if(status == BT_STATUS_SUCCESS) {
        gnss_spp_cntx_p->is_connected = true;
        gnss_spp_cntx_p->max_packet_length = conn_cnf_p->max_packet_length;
        gnss_bt_data_init();
        xTimerStart(gnss_bt_data.time_handle, 0);
        gnss_app_forward_cmd((int8_t*)gnss_cmd_pmtk299, strlen(gnss_cmd_pmtk299));
    }  else {
        memset(gnss_spp_cntx_p,  0x0, sizeof(gnss_spp_cntx_t));
    }
    return;
}

/**
* @brief       This function used to notify bt spp disconnect indication.
* @param[in]   status The status of bt spp connection.
* @param[in]   disc_ind_p The data of bt disconnection information.
* @return      void
*/
void gnss_spp_disconnect_ind(bt_status_t status, bt_spp_disconnect_ind_t *disc_ind_p)
{
    GNSSLOGD("[BT]handle:0x%x, result:0x%x\r\n", disc_ind_p->handle,status);    
    gnss_spp_cntx_p->is_connected = false;
    memset(gnss_spp_cntx_p, 0x0, sizeof(gnss_spp_cntx_t));
    xTimerStop(gnss_bt_data.time_handle, 0);
    gnss_app_forward_cmd((int8_t*)gnss_cmd_pmtk299, strlen(gnss_cmd_pmtk299));
    return;
}

/**
* @brief       This function used to disconnect bt spp.
* @return      void
*/
void gnss_spp_disconnect_request(void)
{
    bt_status_t result = bt_spp_disconnect(gnss_spp_cntx_p->spp_handle);
    GNSSLOGD("[BT]handle:0x%x, result:0x%x\r\n", gnss_spp_cntx_p->spp_handle, result);
    return;
}

/**
* @brief       This function used to print the received data.
* @return      void
*/
void gnss_spp_print_received_data(void)
{
        uint32_t i = 0;
        GNSSLOGD("[BT]print server recevied data start:%d\r\n", gnss_spp_cntx_p->rx_data_length);
        for (i = 0; i < gnss_spp_cntx_p->rx_data_length; i++) {
            GNSSLOGD( "%c", gnss_spp_cntx_p->rx_data_buf[i]);
        }
        GNSSLOGD("[BT]print server recevied end!!!\r\n");
}

/**
* @brief       This function used to send the data in ring buffer.
* @return      void
*/
void gnss_spp_send_data(void)
{
    bt_status_t result = BT_STATUS_FAIL;
    uint16_t need_send = 0;
    
    do {
        if (gnss_spp_cntx_p->tx_left_length <= 0) {
            gnss_spp_cntx_p->tx_left_length = gnss_spp_cntx_p->tx_data_length = gnss_bt_data_get_data((int8_t*)gnss_spp_cntx_p->tx_data_buf, GNSS_SPP_SEND_DATA_BUF);
            gnss_spp_cntx_p->tx_data_buf[gnss_spp_cntx_p->tx_data_length] = 0;
        }
        if (gnss_spp_cntx_p->tx_left_length == 0){
            return;
        }
        need_send = gnss_spp_cntx_p->tx_left_length;
        if (need_send > gnss_spp_cntx_p->max_packet_length){
            need_send = gnss_spp_cntx_p->max_packet_length;
        }

        result = bt_spp_send(gnss_spp_cntx_p->spp_handle, 
                gnss_spp_cntx_p->tx_data_buf + gnss_spp_cntx_p->tx_data_length - gnss_spp_cntx_p->tx_left_length,
                need_send);

        if (result == BT_STATUS_SPP_TX_NOT_AVAILABLE) {
            //means it send fail currently data, need send again when u recieve BT_SPP_READY_TO_SEND_IND.
            GNSSLOGD("[BT]wait can write message,left size:%d\r\n", gnss_spp_cntx_p->tx_left_length);
            return;
        }
        if(result != BT_STATUS_SUCCESS) {
            GNSSLOGD( "[BT]error result = 0x%x, need to check reason.\r\n", result);
            return;
        }
        gnss_spp_cntx_p->tx_left_length -= need_send;
        GNSSLOGD( "[BT]gnss_spp_send_data: %d\r\n", need_send);
    }while (1);
}

/**
* @brief       This function used to receive data.
* @param[out]   data Data buffer pointer
* @param[out]   len The data length
* @return      void
*/
void gnss_spp_receive_data(uint8_t *data, uint16_t len)
{
    memcpy((void *) (gnss_spp_cntx_p->rx_data_buf + gnss_spp_cntx_p->rx_data_length), data, len);
    gnss_spp_cntx_p->rx_data_length += len;
    return;
}

/**
* @brief       This function used to notify bt spp ready to send.
* @param[in]   send_ind_p The bt spp information of send indication.
* @return      void
*/
void gnss_spp_ready_to_send_ind(bt_spp_ready_to_send_ind_t* send_ind_p)
{
    GNSSLOGD("[BT]server ready to send data.\r\n");
    gnss_spp_send_data();
}

/**
 * @brief          This function is for spp server event handler implement in gnss task.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */
void gnss_spp_event_handler(bt_msg_type_t msg, bt_status_t status, void *param)
{
     GNSSLOGD("[BT]gnss_spp_event_handler, 0x%x\r\n", msg);
     switch (msg) {
         case BT_SPP_CONNECT_IND:/*0x34000000*/
         {
             gnss_spp_connect_ind(status, (bt_spp_connect_ind_t *) param);
         }
         break;
         
         case BT_SPP_CONNECT_CNF:/*0x34000001*/
         {
             gnss_spp_connect_cnf(status, (bt_spp_connect_cnf_t*)param);
             if(gnss_spp_cntx_p->is_connected) {
                 gnss_spp_send_data();
             }
         }
         break;
         
         case BT_SPP_DISCONNECT_IND:/*0x34000002*/
         {
             gnss_spp_disconnect_ind(status,(bt_spp_disconnect_ind_t*)param);
         }
         break;
         
         case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
         {            
             gnss_spp_ready_to_send_ind((bt_spp_ready_to_send_ind_t*)param);
         }
         break;
         
        case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
        {
            gnss_spp_print_received_data();
        }
        break;

        case GNSS_SPP_MSG_TIMEOUT: /*0x4000001000*/
        {
            gnss_spp_send_data();
        }
        break;
        default:
        break;
    }
}

/**
 * @brief          This function is for spp event handler implement in bt task.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */
bt_status_t gnss_bt_event_callback(bt_msg_type_t msg, bt_status_t status, void *param)
{
    GNSSLOGD("[BT]msg:0x%x, status:0x%x\r\n", msg, status);
    gnss_spp_message_t message;
    
    switch (msg) {
        case BT_SPP_CONNECT_IND:/*0x34000000*/
        {             
            memcpy((void *) message.param, param, sizeof(bt_spp_connect_ind_t));
        }
        break;
        
        case BT_SPP_CONNECT_CNF:/*0x34000001*/
        {
            memcpy((void *) message.param, param, sizeof(bt_spp_connect_cnf_t));
        }
        break;
        
        case BT_SPP_DISCONNECT_IND:/*0x34000002*/
        {          
            memcpy((void *) message.param, param, sizeof(bt_spp_disconnect_ind_t));
        }
        break;  
            
        case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
        {
            bt_spp_data_received_ind_t * data_ind = (bt_spp_data_received_ind_t*)param;
            memcpy((void *) message.param, param, sizeof(bt_spp_data_received_ind_t));
            //Copy out the app packet becuase the packet buffer will be release after this BT callstack is finished.
            gnss_spp_receive_data(data_ind->packet, data_ind->packet_length);
        }
        break;  
        
        case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
        {  
            memcpy((void *) message.param, param, sizeof(bt_spp_ready_to_send_ind_t));
        }
        break;   
        
        default:
        break;
    }

    message.msg = msg;
    message.status = status;
    xQueueSend(gnss_gnss_spp_queue, (void*)&message, 0);
    return BT_STATUS_SUCCESS;
}

/**
* @brief       Time out callback function
* @param[in]   timer_handle: Timer handle
* @return      None
*/
void gnss_spp_timer_callback( TimerHandle_t timer_handle )
{    
    gnss_spp_message_t message;
    message.msg = GNSS_SPP_MSG_TIMEOUT;
    xQueueSend(gnss_gnss_spp_queue, (void*)&message, 0);
}

/**
* @brief       Bridge task main function
* @param[in]   arg The task create parameter.
* @return      None
*/
void gnss_spp_task(void *arg)
{
    gnss_spp_message_t message; 
    bt_status_t result;

    gnss_gnss_spp_queue = xQueueCreate(3, sizeof(gnss_spp_message_t));
    if( gnss_gnss_spp_queue == NULL ) {
        GNSSLOGD("[BT]create server queue failed!\r\n");
        return;
    }
    
    result = bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SPP | MODULE_MASK_SYSTEM, (void*)gnss_bt_event_callback);
    if (result != BT_STATUS_SUCCESS) {
        GNSSLOGD("[BT]gnss_bt_bridge_init fail!\r\n");
        return;
    }
    
    bt_callback_manager_add_sdp_customized_record(&gnss_spp_sdp_record);

    gnss_bt_data.time_handle = xTimerCreate("GNSSBT",       // Just a text name, not used by the kernel.
        ( GNSS_BT_BRIDGE_TRY_READ_TIME ),   // The timer period in ticks.
        pdTRUE,        // The timer will auto-reload themselves when they expire.
        ( void * ) 0,  // Assign each timer a unique id equal to its array index.
        gnss_spp_timer_callback // Each timer calls the same callback when it expires.
        );
    
    while(1) {
        if(xQueueReceive(gnss_gnss_spp_queue, (void *)&message, portMAX_DELAY)) {
            gnss_spp_event_handler(message.msg, message.status, (void*) (message.param));
            memset((void*) &message, 0, sizeof(gnss_spp_message_t));
        }
    }
}

/**
* @brief       This function used to create bridge task
* @return      None
*/
void gnss_bt_bridge_init(void)
{
    TaskHandle_t xCreatedServerTask;

    GNSSLOGD("[BT]gnss_bt_bridge_init: create spp app task!\r\n");
    xTaskCreate(gnss_spp_task, GNSS_SPP_TASK_NAME, GNSS_SPP_TASK_STACK_SIZE /((uint32_t)sizeof(StackType_t)), NULL, GNSS_SPP_TASK_PRIO, &xCreatedServerTask);
}

/**
* @brief       This function used to handle AT+GNSSBT command.
* @param[in]   parse_cmd The AT command's parameter.
* @return      None
*/
atci_status_t gnss_bt_bridge_at_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    GNSSLOGD("[BT]gnss_bt_bridge_at_handler:%s\r\n", parse_cmd->string_ptr);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE:
            gnss_bt_bridge_init();
            strcpy((char*) output.response_buf, "OK");
            output.response_len = strlen((char*) output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
            atci_send_response(&output);
            break;
        default :
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
            break;
    }
    
    return ATCI_STATUS_OK;
}

/**
* @brief       This function used to register AT+GNSSBT command.
* @return      None
*/
void gnss_bt_bridge_atcmd_init()
{
    int32_t ret;
    static atci_cmd_hdlr_item_t gnss_app_bt_atcmd[] = {
        {"AT+GNSSBT",     gnss_bt_bridge_at_handler,   0, 0},
    };
    ret = atci_register_handler(gnss_app_bt_atcmd, sizeof(gnss_app_bt_atcmd) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        GNSSLOGD("[BT]gnss_bt_bridge_atcmd_init register success\r\n");
    } else {
        GNSSLOGE("[BT]gnss_bt_bridge_atcmd_init register fail\r\n");
    }
}

#endif /*GNSS_SUPPORT_BT_BRIDGE*/
