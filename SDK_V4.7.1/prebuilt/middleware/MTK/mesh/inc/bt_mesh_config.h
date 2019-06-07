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

#ifndef __BT_MESH_CONFIG_H__
#define __BT_MESH_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "bt_mesh_access.h"
#include "bt_mesh_model.h"
#include "bt_mesh_common.h"
#include "bt_mesh_flash.h"
#include "bt_mesh_access.h"



/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshConfig Configuration 
 *   @{
 *   This section introduces the mesh configuration APIs. It shows how to: use enums and structures, and API prototypes to enable and disable features;
 *   set and get the default TTL; add and delete the network key, application key, or device key; update the network key and application key; 
 *   set the replay protection size or sequence number or iv index; and how to get other mesh network parameters.
 *
*/

/**
 * @defgroup Bluetooth_mesh_config Define 
 * @{
 */

/*!
    @brief This value is used in the composition header, #bt_mesh_config_init_params_t,
           #bt_mesh_config_enable_features, #bt_mesh_config_disable_features and #bt_mesh_config_is_feature_enabled.
 */
#define BT_MESH_FEATURE_NONE   0x00    /**< A bit field indicating no feature. */
#define BT_MESH_FEATURE_RELAY  0x01    /**< A bit field indicating the feature relay. */
#define BT_MESH_FEATURE_PROXY  0x02    /**< A bit field indicating the feature proxy. */
#define BT_MESH_FEATURE_FRIEND 0x04    /**< A bit field indicating the feature friend. */
#define BT_MESH_FEATURE_LPN    0x08    /**< A bit field indicating the feature low-power node. */

/*!
     @brief These values are the maximum number of each parameter that can be stored.
 */
#define BT_MESH_CONFIG_NET_KEY_MAXIMUM_NUMBER                     10  /**< The maximum number of network keys. */
#define BT_MESH_CONFIG_APP_KEY_MAXIMUM_NUMBER                     10  /**< The maximum number of application keys. */
#define BT_MESH_CONFIG_MODEL_RECORD_MAXIMUM_NUMBER                50  /**< The maximum number of model records. */
#define BT_MESH_CONFIG_MODEL_PUBLICATION_RECORD_MAXIMUM_NUMBER    15  /**< The maximum number of model publication records. */
#define BT_MESH_CONFIG_MODEL_SUBSCRIPTION_RECORD_MAXIMUM_NUMBER   50  /**< The maximum number of model subscription records. */
#define BT_MESH_CONFIG_HEALTH_SERVER_RECORD_MAXIMUM_NUMBER        50  /**< The maximum number of health server records. */

#define BT_MESH_REPLAY_PROTECTION_LIST_DEFAULT      (100)    /**< Default number of records for replay protection; Cannot be modified. */

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_config_enum Enum 
 * @{
 */

/** @brief  IV update state.
 */
typedef enum {
    BT_MESH_IV_UPDATE_STATE_NORMAL = 0,        /**< Indicates the IV update is in normal operation. */
    BT_MESH_IV_UPDATE_STATE_IN_PROGRESS = 1,   /**< Indicates the IV update is in progress. */
} bt_mesh_iv_update_state_t;

/** @brief  Key refresh state.
 */
typedef enum {
    BT_MESH_KEY_REFRESH_STATE_NONE = 0,    /**< Key refresh phase 0. Indicates normal device operation. */
    BT_MESH_KEY_REFRESH_STATE_1 = 1,       /**< Key refresh phase 1. Old keys are used for packet transmission, but new keys can be used to receive messages. */
    BT_MESH_KEY_REFRESH_STATE_2 = 2,       /**< Key refresh phase 2. New keys are used for packet transmission, but old keys can be used to receive messages. */
    BT_MESH_KEY_REFRESH_STATE_3 = 3,       /**< Key refresh phase 3. Used to complete a key refresh procedure and transition back to phase 0. */
} bt_mesh_key_refresh_state_t;

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_config_struct Struct 
 * @{
 */

/** @brief The capability of mesh storage. The user can configure when the mesh is initialized. 
*/
typedef struct {
    uint16_t network_key_record;             /**< The maximum number of network keys that can be stored. */
    uint16_t application_key_record;         /**< The maximum number of application keys that can be stored. */
    uint16_t model_record;                   /**< The maximum number of model records that can be stored. */
    uint16_t publication_address_record;     /**< The maximum number of model publication records that can be stored. */
    uint16_t subscription_address_record;    /**< The maximum number of model subscription records that can be stored. */
    uint16_t health_server_record;	         /**< The maximum number of health server records that can be stored. */
} mesh_record_config_t;

/** @brief  The initialization configuration parameters for mesh. This parameter is used in #bt_mesh_init.
 */
typedef struct {
    uint8_t device_uuid[BT_MESH_UUID_SIZE];    /**< The device UUID. */
    uint16_t oob_info;                      /**< The OOB information of this device. Please refer to the Provision OOB information.*/
    uint8_t default_ttl;                    /**< The default TTL value. */
    const char *uri;                        /**< The uri infomation of this device; Can be NULL. */
    mesh_record_config_t *mesh_record;      /**< The configuration mesh record. */
} bt_mesh_config_init_params_t;

/** @brief  The device key structure.
 */
typedef struct {
    uint16_t address;                           /**< The unicast address associated with this device key. */
    uint8_t device_key[BT_MESH_DEVKEY_SIZE];   /**< The device key value. */
} bt_mesh_config_devkey_entry_t;

/**
 * @}
 */

/*!
    @brief Enabling features.
    @param[in] feature is a bit field indicating the device features.
    @return
    #BT_MESH_SUCCESS,  means the feature was successfully enabled. \n
    #BT_MESH_ERROR_FAIL, means enabling the feature failed. \n
*/
bt_mesh_status_t bt_mesh_config_enable_features(uint16_t feature);

/*!
    @brief Disabling features.
    @param[in] feature is a bit field indicating the device features.
    @return
    #BT_MESH_SUCCESS, means the feature was successfully disabled. \n
    #BT_MESH_ERROR_FAIL, means disabling the feature failed. \n
*/
bt_mesh_status_t bt_mesh_config_disable_features(uint16_t feature);

/*!
    @brief Checking whether the specified feature is enabled.
    @param[in] feature is a bit field indicating the device features.
    @return
    true, means the feature is enabled.\n
    false, means the feature is disabled.
*/
bool bt_mesh_config_is_feature_enabled(uint16_t feature);

/*!
    @brief Getting the currently enabled features.
    @return Mesh Feature Bit Field of the currently active features of the node.
*/
uint16_t bt_mesh_config_get_feature_enabled(void);

/*!
    @brief Setting the default TTL value.
    @param[in] ttl is the new default TTL value.
    @return
    #BT_MESH_SUCCESS, means the default TTL was successfully set. \n
    #BT_MESH_ERROR_FAIL, means setting the default TTL failed. \n
*/
bt_mesh_status_t bt_mesh_config_set_default_ttl(uint8_t ttl);

/*!
    @brief Getting the default TTL value.
    @return The default TTL value.
*/
uint8_t bt_mesh_config_get_default_ttl(void);

/*!
    @brief Getting the device UUID.
    @return The 16-byte device UUID.
*/
uint8_t *bt_mesh_config_get_uuid( void );

/*!
    @brief Adding the network key.
    @param[in] network_key is the 16-byte network key.
    @param[in] key_index is the key index that is assigned for this key.
    @note This process automatically generates the NID, encryption key, and privacy key.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the network key was successfully added . \n
    #BT_MESH_ACCESS_MSG_STATUS_INSUFFICIENT_RESOURCES, there are no resources for the new network key. \n
    #BT_MESH_ACCESS_MSG_STATUS_KEY_INDEX_ALREADY_STORED, the specified key index is already used. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_add_network_key(const uint8_t *network_key, uint16_t key_index);

/*!
    @brief Updating the network key.
    @param[in] network_key is the 16-byte network key.
    @param[in] key_index is the key index that is assigned for this key.
    @note This process automatically generates the NID, encryption key, and privacy key.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the network key was successfully updated . \n
    #BT_MESH_ACCESS_MSG_STATUS_CANNOT_UPDATE, cannot update the network key during the key refresh process. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_NETKEY, the network key index is invalid or does not exist. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_update_network_key(const uint8_t *network_key, uint16_t key_index );

/*!
    @brief Deleting the network key.
    @param[in] key_index is the assigned key index that will be deleted.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the network key was successfully deleted . \n
    #BT_MESH_ACCESS_MSG_STATUS_CANNOT_REMOVE, cannot delete the network key. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_delete_network_key(uint16_t key_index);

/*!
    @brief Getting network key index list.
    @param[out] key_count (output)key count
    @return Pointer to the array for storing the application key indexes. The array must be freed.
*/
uint16_t *bt_mesh_config_get_network_key_index_list(uint32_t *key_count);

/*!
    @brief Getting the network key.
    @param[in] key_index is the network key index.
    @return A 16-byte network key array.
*/
uint8_t *bt_mesh_config_get_network_key(uint16_t key_index);

/*!
    @brief Adding the application key.
    @param[in] application_key contains a 16-byte application key.
    @param[in] application_index is the application key index.
    @param[in] network_index is the network key index bound with this application key.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the application key was successfully added . \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_NETKEY, the network key index is invalid or does not exist. \n
    #BT_MESH_ACCESS_MSG_STATUS_KEY_INDEX_ALREADY_STORED, the application key index already exists. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_add_application_key(const uint8_t *application_key, uint16_t application_index, uint16_t network_index);

/*!
    @brief Updating the application key.
    @param[in] application_key contains a 16-byte application key.
    @param[in] application_index is the application key index.
    @param[in] network_index is the network key index bound with this application key.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the application key was successfully updated . \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_APPKEY, the application key index does not exist. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_BINDING, the network key is not bound with this application key. \n
    #BT_MESH_ACCESS_MSG_STATUS_CANNOT_UPDATE, cannot update the application key during the key refresh process. \n
    #BT_MESH_ACCESS_MSG_STATUS_INSUFFICIENT_RESOURCES, there are no resource for updating the application key. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_NETKEY, the network key index is inavlid or does not exist. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_update_application_key(const uint8_t *application_key, uint16_t application_index, uint16_t network_index);

/*!
    @brief Getting the appkey index list that binds to the specific netkey.
    @param[in] netkey_index the request key index.
    @param[out] key_count (output) key count of the appkey.
    @return Pointer to the array for storing the application key indexes. The array must be freed.
*/
uint16_t *bt_mesh_config_get_application_key_index_list(uint16_t netkey_index, uint32_t *key_count);

/*!
    @brief Getting the appkey index list that binds to the specific model.
    @param[in] model_handle the request model handle.
    @param[out] key_count (output)key count of appkey.
    @return Pointer to the array for storing the application key indexes. The array must be freed.
*/
uint16_t *bt_mesh_config_get_bound_application_key_index_list(
    uint16_t model_handle, uint32_t *key_count);

/*!
    @brief Getting the application key.
    @param[in] key_index is the application key index.
    @return A 16-byte application key array.
*/
uint8_t *bt_mesh_config_get_application_key(uint16_t key_index);

/*!
    @brief Setting the device key for this device.
    @param[in] device_key is the 16-byte device key.
    @return
    #BT_MESH_SUCCESS, the device key was successfully added. \n
    #BT_MESH_ERROR_INVALID_PARAM, the device key is invalid.
*/
bt_mesh_status_t bt_mesh_config_set_device_key(const uint8_t *device_key);

/*!
    @brief Adding the device key.
    @param[in] device_key is the 16-byte device key.
    @param[in] address is the unicast address associated with this device key.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the device key was successfully added. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_ADDRESS, the address is not a unicast address or it already exists.
    #BT_MESH_ACCESS_MSG_STATUS_INSUFFICIENT_RESOURCES, there are no resources for the new device key. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_add_device_key(
    const uint8_t *device_key, uint16_t address);

/*!
    @brief Deleting the device key.
    @param[in] address is the unicast address associated with this device key.
    @return
    #BT_MESH_ACCESS_MSG_STATUS_SUCCESS, the device key was successfully deleted. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_ADDRESS, the address is not a unicast address or does not exist. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_config_delete_device_key(uint16_t address);

/*!
    @brief Getting the device key by node address.
    @param[in] address indicates the target node address.
    @return the device key of the target node address.
*/
bt_mesh_config_devkey_entry_t *bt_mesh_config_get_device_key(uint16_t address);

/*!
    @brief Setting the size of the replay protection list.
    @param[in] size is the size of the replay protection list.
    @return
    #BT_MESH_SUCCESS, means the replay protection size was successfully set. \n
    #BT_MESH_ERROR_FAIL, means setting the replay protection failed. \n
*/
bt_mesh_status_t bt_mesh_config_set_replay_protection_size(uint16_t size);

/*!
    @brief Clearing the replay protection list.
    @return
    #BT_MESH_SUCCESS, means the replay protection successfully cleared. \n
    #BT_MESH_ERROR_FAIL, means clearing the replay protection failed. \n
*/
bt_mesh_status_t bt_mesh_config_clear_replay_protection(void);

/*!
    @brief Setting the IV index and IV update state.
    @param[in] iv_index is the current IV index.
    @param[in] state is the current state of the IV update.
    @return
    #BT_MESH_SUCCESS, the IV index and state were successully set.\n
    #BT_MESH_ERROR_INVALID_PARAM, invalid IV udpate state.
*/
bt_mesh_status_t bt_mesh_config_set_iv_index(uint32_t iv_index, bt_mesh_iv_update_state_t state);

/*!
    @brief Setting the sequence number.
    @param[in] sequence_number is the new sequence number.
    @return
    #BT_MESH_SUCCESS, the sequence number was successully set.\n
    #BT_MESH_ERROR_INVALID_PARAM, invalid sequence number.
*/
bt_mesh_status_t bt_mesh_config_set_sequence_number(uint32_t sequence_number);

/*!
    @brief Getting the network transmit state.
    @return The network transmit state.
*/
bt_mesh_network_transmit_t *bt_mesh_config_get_network_transmit(void);

/*!
    @brief Getting the relay retransmit transmit state.
    @return The relay retransmit transmit state.
*/
bt_mesh_relay_retransmit_t *bt_mesh_config_get_relay_retransmit(void);

/*!
    @brief Getting the current network beacon state.
    @return The network beacon state.
*/
uint8_t bt_mesh_config_get_secure_network_beacon_state(void);

/*!
    @brief Getting the current node identity state.
    @return The node identity state.
*/
bt_mesh_node_identity_t bt_mesh_config_get_node_identity_state(void);

/*!
    @brief Getting the heartbeat publication state.
    @return The heartbeat publication state.
*/
bt_mesh_heartbeat_publication_t *bt_mesh_config_get_heartbeat_publication_state(void);

/*!
    @brief Getting the heartbeat subscription state.
    @return The heartbeat subscription state.
*/
bt_mesh_heartbeat_subscription_t* bt_mesh_config_get_heartbeat_subscription_state(void);

/*!
    @brief Getting the lpn poll timeout list.
    @param[out] count is the total count of the lpn poll timeout list.
    @return The the lpn poll timeout list.
*/
bt_mesh_lpn_poll_timeout_t *bt_mesh_config_get_lpn_poll_timeout(uint32_t *count);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_CONFIG_H__
