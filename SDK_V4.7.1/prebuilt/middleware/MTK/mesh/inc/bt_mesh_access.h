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

#ifndef __BT_MESH_ACCESS_H__
#define __BT_MESH_ACCESS_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "bt_mesh_common.h"

/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshAccess Access
 *   @{
 *   This section introduces the Access Layer APIs. It shows how to use the enums, structures and functions, and API prototypes to publish
 *   a message to the mesh network, and how to reply to status requests made by nodes on the mesh network.
 *
 * @section bt_mesh_access_api_usage How to manage and reply to an access message.
 *
 * - The application calls the #bt_mesh_access_model_reply() function.
 * - Please refer to \ref bt_mesh_model_api_usage for more information about the register access message.
 * - This shows how to reply to a reliable access message when it is received. A model handle, access message, and arguments are received with a registered callback.
 * @code
    void msg_generic_on_off_status(uint16_t model_handle, uint8_t presentOnOff, uint8_t targetOnOff,
        uint8_t remainingTime, bt_mesh_access_message_rx_t* msg)
    {
        bt_mesh_access_message_tx_t* reply;

        reply = malloc(sizeof(bt_mesh_access_message_tx_t));
        reply->opcode.company_id = MESH_MODEL_COMPANY_ID_NONE;
        reply->opcode.opcode = Generic_OnOff_Status;
        reply->length = 3;
        reply->buffer = malloc(3);
        reply->buffer[0] = presentOnOff;
        reply->buffer[1] = targetOnOff;
        reply->buffer[2] = remainingTime;

        bt_mesh_access_model_reply(model_handle, msg, reply);

        free(reply->buffer);
        free(reply);
    }

    void _generic_onoff_set_handler(uint16_t model_handle, bt_mesh_access_message_rx_t* msg, void* arg, bool reliable)
    {
        mesh_onoff_server_model_t* onOffServer;
        if (reliable) {
            msg_generic_on_off_status(model_handle, gOnOffServer.onOff, msg->buf[0], 0, msg);
        }
    }

    void _sig_msg_handler(uint16_t model_handle, bt_mesh_access_message_rx_t* msg, void* arg)
    {
        switch(msg->opcode.opcode) {
            case Generic_OnOff_Set: {
                _generic_onoff_set_handler(model_handle, msg, arg, true);
                break;
            }
            default:
                break;
        }
    }

 * @endcode
*/


/**
 * @defgroup Bluetooth_mesh_access_enum Enum 
 * @{
 */

/**
 * @brief Status codes for messages.
 */
typedef enum {
    BT_MESH_ACCESS_MSG_STATUS_SUCCESS = 0,                        /**< Success. */
    BT_MESH_ACCESS_MSG_STATUS_INVALID_ADDRESS = 1,                /**< Invalid Address. */
    BT_MESH_ACCESS_MSG_STATUS_INVALID_MODEL = 2,                  /**< Invalid Model. */
    BT_MESH_ACCESS_MSG_STATUS_INVALID_APPKEY = 3,                 /**< Invalid AppKey Index. */
    BT_MESH_ACCESS_MSG_STATUS_INVALID_NETKEY = 4,                 /**< Invalid NetKey Index. */
    BT_MESH_ACCESS_MSG_STATUS_INSUFFICIENT_RESOURCES = 5,         /**< Insufficient Resources. */
    BT_MESH_ACCESS_MSG_STATUS_KEY_INDEX_ALREADY_STORED = 6,       /**< Key Index Already Stored. */
    BT_MESH_ACCESS_MSG_STATUS_INVALID_PUBLISH_PARAMS = 7,         /**< Invalid Publish Parameters. */
    BT_MESH_ACCESS_MSG_STATUS_NOT_A_SUBSCRIBE_MODEL = 8,          /**< Not a Subscribe Model. */
    BT_MESH_ACCESS_MSG_STATUS_STORAGE_FAILURE = 9,                /**< Storage Failure. */
    BT_MESH_ACCESS_MSG_STATUS_FEATURE_NOT_SUPPORTED = 10,         /**< Feature Not Supported. */
    BT_MESH_ACCESS_MSG_STATUS_CANNOT_UPDATE = 11,                 /**< Cannot Update. */
    BT_MESH_ACCESS_MSG_STATUS_CANNOT_REMOVE = 12,                 /**< Cannot Remove. */
    BT_MESH_ACCESS_MSG_STATUS_CANNOT_BIND = 13,                   /**< Cannot Bind. */
    BT_MESH_ACCESS_MSG_STATUS_TEMPORARILY_UNABLE_TO_CHANGE_STATE = 14,    /**< Temporarily Unable to Change State. */
    BT_MESH_ACCESS_MSG_STATUS_CANNOT_SET = 15,                    /**< Cannot Set. */
    BT_MESH_ACCESS_MSG_STATUS_UNSPECIFIED_ERROR = 16,             /**< Unspecified Error. */
    BT_MESH_ACCESS_MSG_STATUS_INVALID_BINDING = 17,               /**< Invalid Binding. */
} bt_mesh_access_msg_status_code_t;
/**
 * @}
 */

/*!
     @name Configuration model messages
     @{
 */
#define BT_MESH_ACCESS_MSG_CONFIG_BEACON_GET	0x8009
#define BT_MESH_ACCESS_MSG_CONFIG_BEACON_SET	0x800A
#define BT_MESH_ACCESS_MSG_CONFIG_BEACON_STATUS	0x800B
#define BT_MESH_ACCESS_MSG_CONFIG_COMPOSITION_DATA_GET	0x8008
#define BT_MESH_ACCESS_MSG_CONFIG_COMPOSITION_DATA_STATUS	0x02
#define BT_MESH_ACCESS_MSG_CONFIG_DEFAULT_TTL_GET	0x800C
#define BT_MESH_ACCESS_MSG_CONFIG_DEFAULT_TTL_SET	0x800D
#define BT_MESH_ACCESS_MSG_CONFIG_DEFAULT_TTL_STATUS	0x800E
#define BT_MESH_ACCESS_MSG_CONFIG_GATT_PROXY_GET	0x8012
#define BT_MESH_ACCESS_MSG_CONFIG_GATT_PROXY_SET	0x8013
#define BT_MESH_ACCESS_MSG_CONFIG_GATT_PROXY_STATUS	0x8014
#define BT_MESH_ACCESS_MSG_CONFIG_FRIEND_GET	0x800F
#define BT_MESH_ACCESS_MSG_CONFIG_FRIEND_SET	0x8010
#define BT_MESH_ACCESS_MSG_CONFIG_FRIEND_STATUS	0x8011
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_GET	0x8018
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_SET	0x03
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_STATUS	0x8019
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET	0x801A
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD	0x801B
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE	0x801C
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL  0x801D
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE	0x801E
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_STATUS	0x801F
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD	0x8020
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE	0x8021
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE	0x8022
#define BT_MESH_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_GET	0x8023
#define BT_MESH_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_SET	0x8024
#define BT_MESH_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_STATUS	0x8025
#define BT_MESH_ACCESS_MSG_CONFIG_RELAY_GET	0x8026
#define BT_MESH_ACCESS_MSG_CONFIG_RELAY_SET	0x8027
#define BT_MESH_ACCESS_MSG_CONFIG_RELAY_STATUS	0x8028
#define BT_MESH_ACCESS_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET	0x8029
#define BT_MESH_ACCESS_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST	0x802A
#define BT_MESH_ACCESS_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET	0x802B
#define BT_MESH_ACCESS_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST	0x802C
#define BT_MESH_ACCESS_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_GET	0x802D
#define BT_MESH_ACCESS_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_STATUS	0x802E
#define BT_MESH_ACCESS_MSG_CONFIG_NETKEY_ADD	0x8040
#define BT_MESH_ACCESS_MSG_CONFIG_NETKEY_DELETE	0x8041
#define BT_MESH_ACCESS_MSG_CONFIG_NETKEY_GET	0x8042
#define BT_MESH_ACCESS_MSG_CONFIG_NETKEY_LIST	0x8043
#define BT_MESH_ACCESS_MSG_CONFIG_NETKEY_STATUS	0x8044
#define BT_MESH_ACCESS_MSG_CONFIG_NETKEY_UPDATE	0x8045
#define BT_MESH_ACCESS_MSG_CONFIG_APPKEY_ADD	0x00
#define BT_MESH_ACCESS_MSG_CONFIG_APPKEY_UPDATE	0x01
#define BT_MESH_ACCESS_MSG_CONFIG_APPKEY_DELETE	0x8000
#define BT_MESH_ACCESS_MSG_CONFIG_APPKEY_GET	0x8001
#define BT_MESH_ACCESS_MSG_CONFIG_APPKEY_LIST	0x8002
#define BT_MESH_ACCESS_MSG_CONFIG_APPKEY_STATUS	0x8003
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_APP_BIND	0x803D
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_APP_STATUS	0x803E
#define BT_MESH_ACCESS_MSG_CONFIG_MODEL_APP_UNBIND	0x803F
#define BT_MESH_ACCESS_MSG_CONFIG_SIG_MODEL_APP_GET	0x804B
#define BT_MESH_ACCESS_MSG_CONFIG_SIG_MODEL_APP_LIST	0x804C
#define BT_MESH_ACCESS_MSG_CONFIG_VENDOR_MODEL_APP_GET	0x804D
#define BT_MESH_ACCESS_MSG_CONFIG_VENDOR_MODEL_APP_LIST	0x804E
#define BT_MESH_ACCESS_MSG_CONFIG_NODE_IDENTITY_GET	0x8046
#define BT_MESH_ACCESS_MSG_CONFIG_NODE_IDENTITY_SET	0x8047
#define BT_MESH_ACCESS_MSG_CONFIG_NODE_IDENTITY_STATUS	0x8048
#define BT_MESH_ACCESS_MSG_CONFIG_NODE_RESET	0x8049
#define BT_MESH_ACCESS_MSG_CONFIG_NODE_RESET_STATUS	0x804A
#define BT_MESH_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_GET	0x8015
#define BT_MESH_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_SET	0x8016
#define BT_MESH_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_STATUS	0x8017
#define BT_MESH_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET	0x8038
#define BT_MESH_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET	0x8039
#define BT_MESH_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_STATUS	0x06
#define BT_MESH_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET	0x803A
#define BT_MESH_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET	0x803B
#define BT_MESH_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS	0x803C
/*!  @} */

/*!
     @name Health model messages
     @{
 */
#define BT_MESH_ACCESS_MSG_HEALTH_CURRENT_STATUS	0x04
#define BT_MESH_ACCESS_MSG_HEALTH_FAULT_STATUS	0x05
#define BT_MESH_ACCESS_MSG_HEALTH_FAULT_CLEAR	0x802F
#define BT_MESH_ACCESS_MSG_HEALTH_FAULT_CLEAR_UNACKNOWLEDGED	0x8030
#define BT_MESH_ACCESS_MSG_HEALTH_FAULT_GET	0x8031
#define BT_MESH_ACCESS_MSG_HEALTH_FAULT_TEST	0x8032
#define BT_MESH_ACCESS_MSG_HEALTH_FAULT_TEST_UNACKNOWLEDGED	0x8033
#define BT_MESH_ACCESS_MSG_HEALTH_PERIOD_GET	0x8034
#define BT_MESH_ACCESS_MSG_HEALTH_PERIOD_SET	0x8035
#define BT_MESH_ACCESS_MSG_HEALTH_PERIOD_SET_UNACKNOWLEDGED	0x8036
#define BT_MESH_ACCESS_MSG_HEALTH_PERIOD_STATUS    0x8037
#define BT_MESH_ACCESS_MSG_HEALTH_ATTENTION_GET	0x8004
#define BT_MESH_ACCESS_MSG_HEALTH_ATTENTION_SET	0x8005
#define BT_MESH_ACCESS_MSG_HEALTH_ATTENTION_SET_UNACKNOWLEDGED	0x8006
#define BT_MESH_ACCESS_MSG_HEALTH_ATTENTION_STATUS	0x8007
/*!  @} */

/**
 * @defgroup Bluetooth_mesh_access_typedef Typedef 
 * @{
 */

/** @brief  This defines the access layer publish timeout event callback prototype.
 *  @param[in] model_handle is the model handle which must manage this publishment.
 *  @param[in] arg is user data.
 *  @return NONE.
 */
typedef void (*bt_mesh_access_publish_timeout_cb_t) (uint16_t model_handle, void *arg);

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_access_struct Struct 
 * @{
 */

/** @brief The access message operation code information. */
typedef struct {
    uint16_t opcode;        /**< Operation code. */
    uint16_t company_id;    /**< Company id; Use #MESH_MODEL_COMPANY_ID_NONE if this is a SIG access message. */
} bt_mesh_access_opcode_t;

/** @brief This structure defines the metadata of a received access message. */
typedef struct {
    uint16_t src_addr;      /**< The source address of this message. */
    uint16_t dst_addr;      /**< The destination address of this message. */
    uint16_t appkey_index;        /**< The application key index used for this message. */
    uint16_t netkey_index;     /**< The network key index used for this message. */
    int8_t rssi;           /**< The RSSI value. */
    uint8_t ttl;            /**< The received TTL value. */
} bt_mesh_access_message_rx_meta_t;

/** @brief This structure defines the metadata of an outgoing access message. */
typedef struct {
    bt_mesh_address_t dst_addr;      /**< The destination address of this message. */
    uint16_t appkey_index;  /**< The application key index used for this message. */
    uint8_t ttl;            /**< The received TTL value. */
} bt_mesh_access_message_tx_meta_t;

/** @brief This structure defines the received access message. */
typedef struct {
    bt_mesh_access_opcode_t opcode;    /**< The operation code information. */
    uint8_t *buffer;                       /**< The received message buffer. */
    uint16_t length;                   /**< The length of the received message. */
    bt_mesh_access_message_rx_meta_t meta_data;    /**< The metadata of this message. */
} bt_mesh_access_message_rx_t;

/** @brief This structure defines an access message for sending. */
typedef struct {
    bt_mesh_access_opcode_t opcode;    /**< The operation code information. */
    uint8_t *buffer;                  /**< The message buffer for sending. */
    uint16_t length;                    /**< The length of this message. */
} bt_mesh_access_message_tx_t;

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_access_typedef Typedef 
 * @{
 */

/** @brief  This defines the access message handler prototype.
 *  @param[in] model_handle is the model handle that wants to manage these messages.
 *  @param[in] msg is the received message.
 *  @param[in] arg is user data.
 *  @return NONE.
 */
typedef void (*bt_mesh_access_msg_handler) (uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg);

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_access_struct Struct 
 * @{
 */

/** @brief This structure defines the mapping of the operation code and handler. */
typedef struct {
    bt_mesh_access_opcode_t opcode;            /**< The operation code information. */
    bt_mesh_access_msg_handler handler;    /**< The message handler for this opcode. */
} bt_mesh_access_opcode_handler_t;

/**
 * @}
 */

/*!
    @brief Reply to an access layer message.
    @param[in] model_handle is the handle of the model.
    @param[in] msg is the received message.
    @param[in] reply is the message that is sent to the peer.
    @return
    #BT_MESH_SUCCESS, means the message is successfully sent. \n
    #BT_MESH_ERROR_INVALID_ADDR, means the address of this model is invalid.
*/
bt_mesh_status_t bt_mesh_access_model_reply(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const bt_mesh_access_message_tx_t *reply);

/*!
    @brief Publish an access message to the publish address of the model.
    @param[in] model_handle is the handle of the model.
    @param[in] msg is the message to be sent to the peer.
    @return
    #BT_MESH_SUCCESS, means the message is successfully sent. \n
    #BT_MESH_ERROR_NULL, means the parameter is null.\n
    #BT_MESH_ERROR_FAIL, means the operation failed. The model handle may be invalid.\n
*/
bt_mesh_status_t bt_mesh_access_model_publish(uint16_t model_handle, const bt_mesh_access_message_tx_t *msg);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_ACCESS_H__
