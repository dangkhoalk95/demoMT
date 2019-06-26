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

#ifndef __BT_MESH_COMMON_H__
#define __BT_MESH_COMMON_H__

/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMesCommon Common
 *   @{
 *
*/


/**
 * @defgroup Bluetooth_mesh_common_define Define 
 * @{
 */

/*!
    @brief These values indicate the size of each key in the mesh module.
 */
#define BT_MESH_KEY_SIZE  (16)                             /**< General key size. */
#define BT_MESH_NETKEY_SIZE (BT_MESH_KEY_SIZE)            /**< The network key size. */
#define BT_MESH_APPKEY_SIZE (BT_MESH_KEY_SIZE)            /**< The application key size. */
#define BT_MESH_DEVKEY_SIZE (BT_MESH_KEY_SIZE)            /**< The device key size. */
#define BT_MESH_SESSIONKEY_SIZE (BT_MESH_KEY_SIZE)        /**< The session key size. */
#define BT_MESH_CONFIRMATION_KEY_SIZE (BT_MESH_KEY_SIZE)  /**< The confirmation key size. */
#define BT_MESH_PUBLIC_KEY_SIZE (64)                       /**< The public key size. */
#define BT_MESH_BEACONKEY_SIZE (BT_MESH_KEY_SIZE)         /**< The beacon key size. */

#define BT_MESH_AUTHENTICATION_SIZE (16)                   /**< Authentication value size in mesh provisioning; Cannot be modified. */

#define BT_MESH_UUID_SIZE (16)                             /**< UUID size in mesh; Cannot be modified. */

#define BT_MESH_TTL_MAX (0x7F)                             /**< Maximum TTL value in mesh; Cannot be modified. */
#define BT_MESH_SEQUENCE_NUMBER_MAX  (0xFFFFFF)            /**< Maximum sequence number value in mesh; Cannot be modified. */
#define BT_MESH_GLOBAL_KEY_INDEX_MAX  (0xFFF)              /**< Maximum key index in mesh; Cannot be modified. */
#define BT_MESH_GLOBAL_PRIMARY_NETWORK_KEY_INDEX  (0x0)    /**< Primary network key index in mesh; Cannot be modified. */

/*!
    @brief This is a special key index for the device key in the Airoha Mesh SDK
    @note Should be used in the sending message only.
 */
#define BT_MESH_DEVICE_KEY_INDEX  (0xFFFF)                 /**< Device key index; Cannot be modified. */


/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_common_enum Enum 
 * @{
 */

/** @brief Mesh error codes. */
typedef enum {
    BT_MESH_SUCCESS = 0,           /**< Success. */
    BT_MESH_ERROR_OOM,             /**< No memory. */
    BT_MESH_ERROR_NULL,            /**< Parameter is null pointer. */
    BT_MESH_ERROR_INVALID_ADDR,    /**< Invalid address. */
    BT_MESH_ERROR_INVALID_TTL,     /**< Invalid TTL value. */
    BT_MESH_ERROR_INVALID_KEY,     /**< Invalid key index. */
    BT_MESH_ERROR_NOT_INIT,        /**< The mesh core is not initialized. */
    BT_MESH_ERROR_INVALID_STATE,   /**< Invalid state. */
    BT_MESH_ERROR_INVALID_ROLE,    /**< Invalid role. */
    BT_MESH_ERROR_FAIL,            /**< Operation failed. */
    BT_MESH_ERROR_INVALID_PARAM    /**< Invalid parameter. */
} bt_mesh_status_t;

/** @brief Mesh address type. */
typedef enum {
    BT_MESH_ADDRESS_TYPE_UNASSIGNED = 0,   /**< Unassigned address. */
    BT_MESH_ADDRESS_TYPE_UNICAST,          /**< Unicast address. */
    BT_MESH_ADDRESS_TYPE_VIRTUAL,          /**< Virtual address. */
    BT_MESH_ADDRESS_TYPE_GROUP,            /**< Group address. */
} bt_mesh_address_type_t;

/** @brief  The configuration state of features.
 */
typedef enum {
    BT_MESH_FEATURE_STATE_DISABLED = 0,       /**< The feature is supported, but disabled. */
    BT_MESH_FEATURE_STATE_ENABLED = 1,        /**< The feature is supported and enabled. */
    BT_MESH_FEATURE_STATE_NOT_SUPPORTED = 2   /**< The feature is not supported. */
} bt_mesh_feature_state_t;

/** @brief The friendship status.
 */
typedef enum {
    BT_MESH_FRIENDSHIP_TERMINATED = 0,         /**< The friendship is terminated. */
    BT_MESH_FRIENDSHIP_ESTABLISHED = 1,        /**< The friendship is successfully established. */
    BT_MESH_FRIENDSHIP_ESTABLISH_FAIL = 2,     /**< The friendship is not established. */
} bt_mesh_friendship_status_t;

/** @brief The Bearer GATT status.
 */
typedef enum {
    BT_MESH_BEARER_GATT_STATUS_CONNECTED = 0,      /**< Bearer GATT is connected. */
    BT_MESH_BEARER_GATT_STATUS_DISCONNECTED = 1,   /**< Bearer GATT is disconnected. */
    BT_MESH_BEARER_GATT_STATUS_NO_SERVICE = 2,     /**< Bearer GATT failed to be established because the specified service was not found. */
    BT_MESH_BEARER_GATT_STATUS_NO_CHARACTERISTIC = 3,  /**< Bearer GATT failed to be established because the specified characteristics were not found. */
    BT_MESH_BEARER_GATT_STATUS_WRITE_CCCD_FAILED = 4,  /**< Bearer GATT failed to be established because writing the CCCD failed. */
} bt_mesh_bearer_gatt_status_t;

/**
 * @}
 */


/**
 * @defgroup Bluetooth_mesh_common_struct Struct 
 * @{
 */

/** @brief Mesh address structure. */
typedef struct {
    bt_mesh_address_type_t type;    /**< The address type of this address. */
    uint16_t value;                 /**< Address value. */
    const uint8_t *virtual_uuid;    /**< The virtual UUID value. It must be NULL unless the address type is #BT_MESH_ADDRESS_TYPE_VIRTUAL. */
} bt_mesh_address_t;


/**
 * @}
 */

/*!
@}
@}
@}
*/

#endif // __BT_MESH_COMMON_H__
