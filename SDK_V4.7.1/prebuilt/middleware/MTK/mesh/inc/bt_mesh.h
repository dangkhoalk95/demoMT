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

#ifndef __BT_MESH_H__
#define __BT_MESH_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "bt_mesh_common.h"

#include "bt_mesh_access.h"
#include "bt_mesh_bearer_adv.h"
#include "bt_mesh_bearer_gatt.h"
#include "bt_mesh_config.h"
#include "bt_mesh_debug.h"
#include "bt_mesh_flash.h"
#include "bt_mesh_friend.h"
#include "bt_mesh_model.h"
#include "bt_mesh_provision.h"
#include "bt_mesh_proxy.h"
#include "bt_mesh_utils.h"

/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   This section introduces the implementation of Bluetooth Mesh profiles and APIs for an application to join a mesh network, then communicate with other mesh nodes when they are on the same mesh network.
 *   @addtogroup BluetoothMesCommon Common
 *   @{
 *   This section introduces the common APIs for the mesh core and mesh utilities, including a description of the terms and acronyms. It shows how to use enums and structures and 
 *   API prototypes to: initialize all mesh parameters; enable or disable mesh features; send mesh messages for a vendor model; and the event callback for an application to receive mesh messages. 
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b BLE                        | Bluetooth Low Energy. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth_low_energy">Wikipedia</a>. |
 * |\b OOB                        | Out-of-band. An association mode primarily designed for scenarios where an Out-of-Band mechanism is used to discover devices and to exchange or transfer cryptographic numbers used during the pairing process. |
 *
 * @section bt_mesh_api_usage How to start Mesh module
 *
 * - The application calls functions #bt_mesh_init() , #bt_mesh_enable() and implement #bt_mesh_app_event_callback().
 * - Before starting the mesh module, please register bt event callback with bt_callback_manager_register_callback(). 
 * - When Bluetooth successfully powers on, refer to \ref bt_mesh_model_api_usage to create a mesh device and use meshinit() in the sample code to do mesh initialization. Event #BT_MESH_EVT_INIT_DONE is received when the mesh module is initialized.
 * - When the mesh module is initialized, call #bt_mesh_enable to start mesh.
 * - Sample code:
 *      @code
    static void meshinit()
    {
        printf("Mesh initialising...\n");
        bt_mesh_init_params_t *initparams = (bt_mesh_init_params_t *)malloc(sizeof(bt_mesh_init_params_t));
        initparams->role = BT_MESH_ROLE_PROVISIONEE;

        // init provision parameter
        initparams->provisionee = (bt_mesh_prov_provisionee_params_t *)malloc(sizeof(bt_mesh_prov_provisionee_params_t));
        initparams->provisionee->cap.number_of_elements = bt_mesh_model_get_element_count();
        initparams->provisionee->cap.algorithms = BT_MESH_PROV_CAPABILITY_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE; // bit 0: P-256, bit 1~15: RFU
        initparams->provisionee->cap.public_key_type = BT_MESH_PROV_CAPABILITY_OOB_PUBLIC_KEY_TYPE_INBAND;
        initparams->provisionee->cap.static_oob_type = 0x00;
        initparams->provisionee->cap.output_oob_size = 0x00;
        initparams->provisionee->cap.output_oob_action = 0x0000;
        initparams->provisionee->cap.input_oob_size = 0x00;
        initparams->provisionee->cap.input_oob_action = 0x0000;

        // init config parameter
        initparams->config = (bt_mesh_config_init_params_t *)malloc(sizeof(bt_mesh_config_init_params_t));
        memset(initparams->config, 0, sizeof(bt_mesh_config_init_params_t));
        memcpy(initparams->config->device_uuid, deviceUuid1, BT_MESH_UUID_SIZE);
        initparams->config->oob_info = BT_MESH_PROV_OOB_INFO_FIELD_NUMBER | BT_MESH_PROV_OOB_INFO_FIELD_STRING;
        initparams->config->uri = NULL;
        initparams->config->default_ttl = 4;

        // init friend parameter
        initparams->friend = NULL;

        bt_mesh_init(initparams);
        free(initparams->provisionee);
        free(initparams->config);
        free(initparams);
    }
    
    bt_mesh_status_t bt_mesh_app_event_callback(bt_mesh_event_id evt_id, bool status, bt_mesh_evt_t *evt_data)
    {
        switch (evt_id) {
            case BT_MESH_EVT_INIT_DONE: {
                ......
                bt_mesh_enable();
                ......
                break;
            }
            
        return BT_MESH_SUCCESS;
    }

    bt_status_t bt_app_event_callback_demo(bt_msg_type_t msg, bt_status_t status, void *buff)
    {
        switch (msg) {
            case BT_POWER_ON_CNF: {    
                mesh_create_device(); //
                meshinit();
                break;
            }
            default:
                break;

        return BT_STATUS_SUCCESS;
    }


    int main(void)    
    {
        ...         
        bt_callback_manager_register_callback(bt_callback_type_app_event,
                                              (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_GATT | MODULE_MASK_SYSTEM | MODULE_MASK_MM),
                                              (void *)bt_app_event_callback_demo);
        ...
    }

 *      @endcode
 *
*/

/******************************************************************************
 * Macros
 ******************************************************************************/
/**
 * @defgroup Bluetooth_mesh_common_enum Enum 
 * @{
 */
/** @brief Mesh roles. */
typedef enum {
    BT_MESH_ROLE_PROVISIONEE,      /**< Act as a provisionee. */
    BT_MESH_ROLE_PROVISIONER,      /**< Act as a provisioner. */
} bt_mesh_role_t;

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_common_struct Struct 
 * @{
 */

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_PROV_CAPABILITIES.
 */
typedef struct {
    bt_mesh_prov_capabilities_t cap;     /**< The capabilities detail value. */
} bt_mesh_evt_prov_capabilities_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY.
 */
typedef struct {
    uint8_t method;   /**< Authentication Method used. */
    uint8_t action;   /**< Selected Output OOB Action or Input OOB Action or 0x00. */
    uint8_t size;     /**< Size of the Output OOB used or size of the Input OOB used or 0x00. */
} bt_mesh_evt_prov_request_auth_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY.
 */
typedef struct {
    uint8_t *pk;    /**< The public key is received. */
} bt_mesh_evt_prov_show_pk_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE.
 */
typedef struct {
    uint8_t auth[BT_MESH_AUTHENTICATION_SIZE];  /**< The authentication value is received. */
} bt_mesh_evt_prov_show_auth_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_PROV_DONE.
 */
typedef struct {
    uint16_t address;      /**< Indicates the target unicast address. */
    uint8_t device_key[BT_MESH_DEVKEY_SIZE]; /**< Indicates the device key. */
} bt_mesh_evt_prov_done_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_PROV_SCAN_UD_RESULT.
 */
typedef struct {
    bt_mesh_prov_unprovisioned_device_t device;      /**< The scanned unprovisioned device. */
} bt_mesh_evt_prov_scan_ud_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_BEARER_GATT_STATUS.
 */
typedef struct {
    uint32_t handle;  /**< The handle of this connection. */
    bt_mesh_bearer_gatt_status_t status;      /**< The status of bearer GATT. */
} bt_mesh_evt_bearer_gatt_status_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_FRIENDSHIP_STATUS.
 */
typedef struct {
    uint16_t address;                                   /**< Indicates the friend or the low-power node unicast address. */
    bt_mesh_friendship_status_t status;                /**< Indicates the friendship status between the nodes. */
} bt_mesh_evt_friendship_status_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_LPN_FRIEND_OFFER.
 */
typedef struct {
    uint8_t receive_window;             /**< Indicates the receive window value supported by the Friend node. */
    uint8_t queue_size;                 /**< Indicates the queue size available on the Friend node. */
    uint8_t subscription_list_size;     /**< Indicates the subscription list size supported by the Friend node for a Low Power node. */
    int8_t rssi;                        /**< Indicates the RSSI measured by the Friend node. */
    uint16_t friend_counter;            /**< Indicates the number of friend offer messages that the Friend node has sent. */
    uint16_t address;                   /**< Indicates the friend node unicast address. */
} bt_mesh_evt_lpn_friend_offer_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM.
 */
typedef struct {
    uint8_t transaction_number;         /**< Indicates the number for identifying a transaction. */
} bt_mesh_evt_lpn_subscription_list_confirm_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_HEARTBEAT.
 */
typedef struct {
    uint8_t init_ttl;   /**< The initial TTL used when sending the heartbeat. */
    uint8_t hops;       /**< The number of times this heartbeat was retransmitted. */
    uint16_t features;   /**< Bit field of the currently active features of the node. */
    uint8_t src;        /**< The source address of this heartbeat. */
} bt_mesh_evt_heartbeat_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_IV_UPDATE.
 */
typedef struct {
    uint32_t iv_index;  /**< The IV index currently used for sending messages. */
    bt_mesh_iv_update_state_t state;      /**< The current IV update state. */
} bt_mesh_evt_iv_update_t;

/*!
     @brief Event parameter of mesh event @ref BT_MESH_EVT_KEY_REFRESH.
 */
typedef struct {
    uint16_t netkey_index;  /**< The network key index. */
    bt_mesh_key_refresh_state_t phase;          /**< The current key refresh phase. */
} bt_mesh_evt_key_refresh_t;

/*!
     @brief Mesh event structure.
 */
typedef struct {
    /**
     * @brief Mesh union structure. 
     */
    union {
        bt_mesh_evt_prov_capabilities_t      prov_cap;           /**<  Parameter of mesh event @ref BT_MESH_EVT_PROV_CAPABILITIES. */
        bt_mesh_evt_prov_request_auth_t      prov_request_auth;  /**<  Parameter of mesh event @ref BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE. */
        bt_mesh_evt_prov_show_pk_t           prov_show_pk;       /**<  Parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY. */
        bt_mesh_evt_prov_show_auth_t         prov_show_auth;     /**<  Parameter of mesh event @ref BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE. */
        bt_mesh_evt_prov_done_t              prov_done;          /**<  Parameter of mesh event @ref BT_MESH_EVT_PROV_DONE. */
        bt_mesh_evt_prov_scan_ud_t           prov_scan_ud;       /**<  Parameter of mesh event @ref BT_MESH_EVT_PROV_SCAN_UD_RESULT. */
        bt_mesh_evt_friendship_status_t      friendship_status;  /**<  Parameter of mesh event @ref BT_MESH_EVT_FRIENDSHIP_STATUS. */
        bt_mesh_evt_lpn_friend_offer_t       lpn_friend_offer;   /**<  Parameter of mesh event @ref BT_MESH_EVT_LPN_FRIEND_OFFER. */
        bt_mesh_evt_lpn_subscription_list_confirm_t lpn_subscription_list_confirm; /**<  Parameter of mesh event @ref BT_MESH_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM. */
        bt_mesh_evt_heartbeat_t              heartbeat;          /**<  Parameter of mesh event @ref BT_MESH_EVT_HEARTBEAT. */
        bt_mesh_evt_iv_update_t              iv_update;          /**<  Parameter of mesh event @ref BT_MESH_EVT_IV_UPDATE. */
        bt_mesh_evt_key_refresh_t            key_refresh;        /**<  Parameter of mesh event @ref BT_MESH_EVT_KEY_REFRESH. */
        bt_mesh_evt_bearer_gatt_status_t     bearer_gatt_status; /**<  Parameter of mesh event @ref BT_MESH_EVT_BEARER_GATT_STATUS. */
    } mesh;
} bt_mesh_evt_t;

/** @brief Mesh security information. */
typedef struct bt_mesh_security_t {
    uint16_t netidx;                /**< The index of the network key. */
    uint16_t appidx;                /**< The index of the application key. If it is 0xFFFF, it means it is using device key. */
    uint8_t *device_key;            /**< The device key value. Cannot be NULL if appidx is 0xFFFF. */
} bt_mesh_security_t;

/** @brief The parameters used for sending the mesh message. */
typedef struct {
    bt_mesh_address_t dst;         /**< Destination address information. */
    uint16_t src;                   /**< Source unicast address. */
    uint8_t ttl;                    /**< TTL value. */
    const uint8_t *data;            /**< The data buffer to be sent. */
    uint16_t data_len;              /**< The data buffer length. */
    bt_mesh_security_t security;   /**< Security information. */
} bt_mesh_tx_params_t;


/** @brief The initialization parameters for mesh. */
typedef struct {
    bt_mesh_role_t role;           /**< Mesh role. */
    bt_mesh_prov_provisionee_params_t *provisionee;       /**< Initialization parameters of the provisionee. Cannot be NULL when the role is #BT_MESH_ROLE_PROVISIONEE. */
    bt_mesh_config_init_params_t *config;   /**< Initialization parameters of config. */
    bt_mesh_friend_init_params_t *friend;               /**< Initialization parameters of friend. The parameters are available only when the friend feature is supported and enabled. The default value is used when init_params is NULL. */
} bt_mesh_init_params_t;

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_common_enum Enum 
 * @{
 */

/*!
    @brief Mesh event ID.
*/
typedef enum {
    BT_MESH_EVT_INIT_DONE = 0,                         /**< Event for mesh initialization complete. */
    BT_MESH_EVT_PROV_CAPABILITIES,                     /**< Event for receiving the capabilites message sent from a provisionee. */
    BT_MESH_EVT_PROV_REQUEST_OOB_PUBLIC_KEY,           /**< Event for requesting the public key. Use #bt_mesh_provision_provide_oob_public_key to set the public key. */
    BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE,           /**< Event for requesting the authentication value. Use #bt_mesh_provision_provide_oob_auth_value to set the authentication value. */
    BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY,              /**< Event for showing the public key. After showing, use #bt_mesh_provision_provide_oob_public_key to set the public key. */
    BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE,              /**< Event for showing the authentication value. After showing, use #bt_mesh_provision_provide_oob_auth_value to set the authentication value. */
    BT_MESH_EVT_PROV_DONE,                             /**< Event for mesh provisioning is complete. */
    BT_MESH_EVT_PROV_SCAN_UD_RESULT,                   /**< Event for scanning the mesh for an unprovidioned device. */
    BT_MESH_EVT_CONFIG_RESET,                          /**< Event for resetting the mesh configuration. */
    BT_MESH_EVT_FRIENDSHIP_STATUS,                     /**< Event for changing the mesh friendship status. */
    BT_MESH_EVT_LPN_FRIEND_OFFER,                      /**< Event for the mesh LPN receiving a friend offer. */
    BT_MESH_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM,  /**< Event for the mesh LPN receiving a friend subscription list confirm. */
    BT_MESH_EVT_HEARTBEAT,                             /**< Event for the mesh heartbeat. */
    BT_MESH_EVT_IV_UPDATE,                             /**< Event for the mesh IV index update. */
    BT_MESH_EVT_KEY_REFRESH,                           /**< Event for the mesh key refresh. */
    BT_MESH_EVT_BEARER_GATT_STATUS,                    /**< Event for the mesh bearer GATT status. */
} bt_mesh_event_id;

/**
 * @}
 */

/**
 * @brief This function is used for mesh module initialization.
 * @param[in] init_params is the parameter for initialization.
 * @return
 * #BT_MESH_SUCCESS, mesh is successfully initialized. \n
 * #BT_MESH_ERROR_NULL, init_params or any parameters in init_params is NULL. \n
 * #BT_MESH_ERROR_OOM, not enough memory for mesh initialization. \n
 * @note when mesh initialization is complete, an event #BT_MESH_EVT_INIT_DONE is received.
 */
bt_mesh_status_t bt_mesh_init ( const bt_mesh_init_params_t *init_params );

/**
 * @brief Enabling the mesh network.
 * @return
 * #BT_MESH_SUCCESS, mesh successfully enabled. \n
 * #BT_MESH_ERROR_NOT_INIT, mesh core is not yet initialized. \n
 */
bt_mesh_status_t bt_mesh_enable( void );

/**
 * @brief Disabling the mesh network.
 * @return
 * #BT_MESH_SUCCESS, mesh successfully disabled. \n
 * #BT_MESH_ERROR_FAIL, disabling the mesh failed. \n
 */
bt_mesh_status_t bt_mesh_disable( void );

/**
 * @brief This function is used to send packets via the mesh network.
 * @param[in] tx_params is the parameters of the packet.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
 * @par       Example
 * @code
 * void send_packet_demo(void)
 * {
 *     bt_mesh_tx_params_t param;
 *     param.dst.value = BT_MESH_ADDR_GROUP_NODES_VALUE;
 *     param.dst.type = bt_mesh_utils_get_addr_type(param.dst.value);
 *     param.src = bt_mesh_model_get_element_address(0);
 *     param.ttl = bt_mesh_config_get_default_ttl;  // default TTL value
 *     param.data_len = 6; // total length of this packet
 *
 *     // set packet buffer, please refer to Mesh Profile Specification 3.7.3
 *     uint8_t *payload = malloc(param.data_len);
 *     payload[0] = (BT_MESH_MODEL_GENERIC_ONOFF_SET_UNACKNOWLEDGED & 0xFF00) >> 8;
 *     payload[1] = BT_MESH_MODEL_GENERIC_ONOFF_SET_UNACKNOWLEDGED & 0x00FF;
 *     payload[2] = val != 0 ? 1 : 0;
 *     payload[3] = gTid++;
 *     payload[4] = 0;
 *     payload[5] = 0;
 *     param.data = payload;
 *     param.security.appidx = 0x123; // which application key used to send
 *     param.security.device_key = NULL;
 *     bt_mesh_send_packet(&param);
 *     free(payload);
 * }
 * @endcode
 */
bt_mesh_status_t bt_mesh_send_packet(	const bt_mesh_tx_params_t *tx_params );

/**
 * @brief   This function is a static callback for the application to listen to the mesh event. Provide a user-defined callback.
 * @param[in] evt_id         is the mesh event ID.
 * @param[in] status         is the status of the callback event.
 * @param[in] evt_data     is the payload of the callback event.
 * @return            The status of this operation returned from the callback.
 */
bt_mesh_status_t bt_mesh_app_event_callback(bt_mesh_event_id evt_id, bool status, bt_mesh_evt_t *evt_data);

/*!
@}
@}
@}
*/


#endif // __BT_MESH_H__

