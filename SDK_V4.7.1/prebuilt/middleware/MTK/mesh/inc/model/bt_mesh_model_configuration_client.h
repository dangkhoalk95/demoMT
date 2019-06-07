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

#ifndef __BT_MESH_MODEL_CONFIGURATION_CLIENT_H__
#define __BT_MESH_MODEL_CONFIGURATION_CLIENT_H__

#include "bt_mesh_access.h"

/*!

     @defgroup PAGE_MESH_MODEL_CONFIG_CLIENT bt_mesh_model_configuration_client.h
     @{
        @page PAGE_MESH_MODEL_CONFIG_CLIENT bt_mesh_model_configuration_client.h
        bt_mesh_model_configuration_client.h defines the Configuration Client Model APIs.
*/

/** @brief The heartbeat publication parameters structure */
typedef struct {
    uint16_t destination;                   /**< Destination address for Heartbeat messages. */
    uint8_t count_log;                      /**< Destination address for Heartbeat messages. */
    uint8_t period_log;                     /**< Period for sending Heartbeat messages. */
    uint8_t ttl;                            /**< TTL to be used when sending Heartbeat messages. */
    uint16_t features;                      /**< Bit field indicating features that trigger Heartbeat messages when changed. */
    uint16_t netkey_index;                  /**< Network key index. */
} bt_mesh_heartbeat_publication_param_t;

/** @brief The heartbeat subscription parameters structure */
typedef struct {
    uint16_t source;                        /**< Source address for Heartbeat messages. */
    uint16_t destination;                   /**< Destination address for Heartbeat messages. */
    uint8_t period_log;                     /**< Period for receiving Heartbeat messages. */
} bt_mesh_heartbeat_subscription_param_t;

/** @brief The model publication parameter structure. */
typedef struct {
    uint16_t element_address;               /**< Address of the element. */
    bt_mesh_address_t publish_address;     /**< Value of the publish address. */
    uint16_t appkey_index;                  /**< Index of the application key. */
    bool friendship_credential_flag;         /**< Value of the Friendship Credential Flag. */
    uint8_t publish_ttl;                    /**< Default TTL value for the outgoing messages. */
    uint8_t publish_period;                 /**< Period for periodic status publishing. */
    uint8_t retransmit_count;               /**< Number of retransmissions for each published message. */
    uint8_t retransmit_interval_steps;      /**< Number of 50-millisecond steps between retransmissions. */
    uint32_t model_id;                      /**< SIG Model ID or Vendor Model ID. */
} bt_mesh_model_publication_param_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_BEACON_STATUS. */
typedef struct {
    uint8_t beacon;            /**< Secure Network Beacon state. */
} config_client_evt_beacon_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_COMPOSITION_DATA_STATUS. */
typedef struct {
    uint8_t page;                                   /**< Page number of the Composition Data. */
    bt_mesh_composition_data_t *composition_data;  /**< Composition Data for the identified page. */
} config_client_evt_composition_data_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_DEFAULT_TTL_STATUS. */
typedef struct {
    uint8_t ttl;    /**< Default TTL value. */
} config_client_evt_default_ttl_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_GATT_PROXY_STATUS. */
typedef struct {
    uint8_t gatt_proxy;     /**< GATT proxy state. */
} config_client_evt_gatt_proxy_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_FRIEND_STATUS. */
typedef struct {
    uint8_t friend;         /**< Friend state. */
} config_client_evt_friend_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_MODEL_PUBLICATION_STATUS. */
typedef struct {
    uint8_t status;             /**< Status Code for the requesting message. */
    uint16_t element_address;   /**< The unicast address of the element, all other address types are prohibited. */
    uint16_t publish_address;   /**< The new Publish Address state for the model. */
    uint16_t appkey_index : 12; /**< The index of application key used for this publication. */
    uint16_t credential_flag : 1;   /**< The value of the friendship credential flag. */
    uint16_t rfu : 3;           /**< Reserved for future use. */
    uint8_t publish_ttl;        /**< The default TTL value for the outgoing messages. */
    uint8_t publish_period;     /**< The period for periodic status publishing. */
    uint8_t publish_retransmit_count : 3;   /**< The number of retransmissions for each published message. */
    uint8_t publish_retransmit_interval_steps : 5;  /**< The number of 50-millisecond steps between retransmissions. */
    uint32_t model_id;          /**< SIG Model ID or Vendor Model ID. */
} __attribute__((packed)) config_client_evt_model_publication_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS. */
typedef struct {
    uint8_t status;             /**< Status Code for the requesting message. */
    uint16_t element_address;   /**< The address of the element. */
    uint16_t subscribed_address;   /**< The address that was used to modify the Subscription List or the unassigned address. */
    uint32_t model_id;          /**< SIG Model ID or Vendor Model ID. */
} __attribute__((packed)) config_client_evt_model_subscription_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_NETWORK_TRANSMIT_STATUS. */
typedef struct {
    uint8_t retransmit_count : 3;           /**< Number of transmissions for each Network PDU originating from the node. */
    uint8_t retransmit_interval_steps : 5;  /**< Number of 10-millisecond steps between retransmissions. */
} __attribute__((packed)) config_client_evt_network_transmit_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_RELAY_STATUS. */
typedef struct {
    uint8_t relay;                          /**< Relay state. */
    uint8_t retransmit_count : 3;           /**< Number of retransmissions on advertising bearer for each Network PDU relayed by the node. */
    uint8_t retransmit_interval_steps : 5;  /**< Number of 10-millisecond steps between retransmissions. */
} __attribute__((packed)) config_client_evt_relay_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_LIST. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t element_address;   /**< Address of the element. */
    uint32_t model_id;          /**< Model id. */
    uint16_t *address;          /**< All addresses from the Subscription List of an element. */
    uint16_t address_count;     /**< Total count of address. */
} __attribute__((packed)) config_client_evt_model_subscription_list_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_LPN_POLL_TIMEOUT_STATUS. */
typedef struct {
    uint16_t lpn_address;       /**< The unicast address of the Low Power node. */
    uint32_t poll_timeout : 24; /**< The current value of the PollTimeout timer of the Low Power node. */
    uint32_t rfu : 8;           /**< Reserved for future use. */
} __attribute__((packed)) config_client_evt_lpn_poll_timeout_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_NETKEY_LIST. */
typedef struct {
    uint16_t *netkey_indexes;   /**< A list of NetKey Indexes known to the node. */
    uint32_t netkey_count;      /**< The count of netkey_indexes. */
} __attribute__((packed)) config_client_evt_netkey_list_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_NETKEY_STATUS. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t netkey_index;      /**< Index of the NetKey. */
} __attribute__((packed)) config_client_evt_netkey_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_APPKEY_LIST. */
typedef struct {
    uint16_t *appkey_indexes;   /**< A list of AppKey Indexes known to the node. */
    uint32_t appkey_count;      /**< The count of appkey_indexes. */
} __attribute__((packed)) config_client_evt_appkey_list_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_APPKEY_STATUS. */
typedef struct {
    uint32_t status : 8;        /**< Status code for the requesting message. */
    uint32_t netkey_index : 12; /**< Index of the NetKey. */
    uint32_t appkey_index : 12; /**< Index of the AppKey. */
} __attribute__((packed)) config_client_evt_appkey_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_MODEL_APP_STATUS. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t element_address;   /**< Address of the element. */
    uint16_t appkey_index;      /**< Index of the AppKey. */
    uint32_t model_id;          /**< SIG Model ID or Vendor Model ID. */
} __attribute__((packed)) config_client_evt_model_app_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_MODEL_APP_LIST. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t element_address;   /**< Address of the element. */
    uint32_t model_id;          /**< Model id. */
    uint16_t *appkey_indexes;   /**< All addresses from the Subscription List of an element. */
    uint32_t appkey_count;      /**< Total count of appkey_indexes. */
} __attribute__((packed)) config_client_evt_model_app_list_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_NODE_IDENTITY_STATUS. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t netkey_index;      /**< Index of the NetKey. */
    uint8_t identity;           /**< Node Identity state. */
} __attribute__((packed)) config_client_evt_node_identity_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_KEY_REFRESH_PHASE_STATUS. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t netkey_index;      /**< Index of the NetKey. */
    uint8_t phase;              /**< Key Refresh Phase State. */
} __attribute__((packed)) config_client_evt_key_refresh_phase_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_HEARTBEAT_PUBLICATION_STATUS. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t destination;       /**< Destination address for Heartbeat messages. */
    uint8_t count_log;          /**< Number of Heartbeat messages remaining to be sent. */
    uint8_t period_log;         /**< Period for sending Heartbeat messages. */
    uint8_t ttl;                /**< TTL to be used when sending Heartbeat messages. */
    uint16_t features;          /**< Bit field indicating features that trigger Heartbeat messages when changed. */
    uint16_t netkey_index;      /**< Index of the NetKey. */
} __attribute__((packed)) config_client_evt_heartbeat_publication_status_t;

/** @brief Event parameter of configuration client model event @ref CONFIG_CLIENT_EVT_HEARTBEAT_SUBSCRIPTION_STATUS. */
typedef struct {
    uint8_t status;             /**< Status code for the requesting message. */
    uint16_t source;            /**< Source address for Heartbeat message. */
    uint16_t destination;       /**< Destination address for Heartbeat messages. */
    uint8_t period_log;         /**< Remaining Period for processing Heartbeat messages. */
    uint8_t count_log;          /**< Number of Heartbeat messages remaining to be sent. */
    uint8_t min_hops;           /**< Minimum hops when receiving Heartbeat messages. */
    uint8_t max_hops;           /**< Maximum hops when receiving Heartbeat messages. */
} __attribute__((packed)) config_client_evt_heartbeat_subscription_status_t;

/*!
     @brief Configuration client model event id
     @{
 */
typedef enum {
    CONFIG_CLIENT_EVT_BEACON_STATUS,            /**< Event for beacon status. */
    CONFIG_CLIENT_EVT_COMPOSITION_DATA_STATUS,  /**< Event for composition data status. */
    CONFIG_CLIENT_EVT_DEFAULT_TTL_STATUS,       /**< Event for default TTL status. */
    CONFIG_CLIENT_EVT_GATT_PROXY_STATUS,        /**< Event for GATT proxy status. */
    CONFIG_CLIENT_EVT_FRIEND_STATUS,            /**< Event for friend status. */
    CONFIG_CLIENT_EVT_MODEL_PUBLICATION_STATUS, /**< Event for model publication status. */
    CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS,    /**< Event for model subscription status. */
    CONFIG_CLIENT_EVT_NETWORK_TRANSMIT_STATUS,      /**< Event for network transmit status. */
    CONFIG_CLIENT_EVT_RELAY_STATUS,                 /**< Event for relay status. */
    CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_LIST,      /**< Event for model subscription status. */
    CONFIG_CLIENT_EVT_LPN_POLL_TIMEOUT_STATUS,  /**< Event for low power node pull timeout status. */
    CONFIG_CLIENT_EVT_NETKEY_LIST,              /**< Event for network key list. */
    CONFIG_CLIENT_EVT_NETKEY_STATUS,            /**< Event for network key status. */
    CONFIG_CLIENT_EVT_APPKEY_LIST,              /**< Event for application key list. */
    CONFIG_CLIENT_EVT_APPKEY_STATUS,            /**< Event for application key status. */
    CONFIG_CLIENT_EVT_MODEL_APP_STATUS,         /**< Event for model app status. */
    CONFIG_CLIENT_EVT_MODEL_APP_LIST,           /**< Event for model app list. */
    CONFIG_CLIENT_EVT_NODE_IDENTITY_STATUS,     /**< Event for node identity status. */
    CONFIG_CLIENT_EVT_NODE_RESET_STATUS,        /**< Event for node reset status. */
    CONFIG_CLIENT_EVT_KEY_REFRESH_PHASE_STATUS, /**< Event for key refresh phase status. */
    CONFIG_CLIENT_EVT_HEARTBEAT_PUBLICATION_STATUS, /**< Event for heartbeat publication status. */
    CONFIG_CLIENT_EVT_HEARTBEAT_SUBSCRIPTION_STATUS /**< Event for heartbeat subscription status. */
} config_client_event_id_t;
/*!  @} */

/*!
     @brief Configuration client event structure.
 */
typedef struct {
    config_client_event_id_t evt_id;        /**<  Configuration client event ID. */
    union {
        config_client_evt_beacon_status_t beacon_status;      /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_BEACON_STATUS */
        config_client_evt_composition_data_status_t composition_data_status;          /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_COMPOSITION_DATA_STATUS */
        config_client_evt_default_ttl_status_t default_ttl_status;        /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_DEFAULT_TTL_STATUS */
        config_client_evt_gatt_proxy_status_t gatt_proxy_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_GATT_PROXY_STATUS */
        config_client_evt_friend_status_t friend_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_FRIEND_STATUS */
        config_client_evt_model_publication_status_t model_publication_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_MODEL_PUBLICATION_STATUS */
        config_client_evt_model_subscription_status_t model_subscription_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS */
        config_client_evt_network_transmit_status_t network_transmit_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_NETWORK_TRANSMIT_STATUS */
        config_client_evt_relay_status_t relay_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_RELAY_STATUS */
        config_client_evt_model_subscription_list_t model_subscription_list;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_LIST */
        config_client_evt_lpn_poll_timeout_status_t lpn_poll_timeout_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_LPN_POLL_TIMEOUT_STATUS */
        config_client_evt_netkey_list_t netkey_list;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_NETKEY_LIST */
        config_client_evt_netkey_status_t netkey_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_NETKEY_STATUS */
        config_client_evt_appkey_list_t appkey_list;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_APPKEY_LIST */
        config_client_evt_appkey_status_t appkey_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_APPKEY_STATUS */
        config_client_evt_model_app_status_t model_app_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_MODEL_APP_STATUS */
        config_client_evt_model_app_list_t model_app_list;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_MODEL_APP_LIST */
        config_client_evt_node_identity_status_t node_identity_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_NODE_IDENTITY_STATUS */
        config_client_evt_key_refresh_phase_status_t key_refresh_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_NODE_RESET_STATUS */
        config_client_evt_heartbeat_publication_status_t heartbeat_publication_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_HEARTBEAT_PUBLICATION_STATUS */
        config_client_evt_heartbeat_subscription_status_t heartbeat_subscription_status;  /**<  parameter of config client model event @ref CONFIG_CLIENT_EVT_HEARTBEAT_SUBSCRIPTION_STATUS */
    } data;
} config_client_evt_t;

/** @brief  This defines the configuration client event handler prototype.
 *  @param[in] model_handle is the model handle which wants to handle these messages.
 *  @param[in] msg is the original received message.
 *  @param[in] event is the parsed configuration client event.
 *  @return NONE
 */
typedef void (*config_client_evt_handler)(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const config_client_evt_t *event);

/*!
    @brief Add a configuration client model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the specified element for adding the configuration client model.
    @param[in] callback is the message handler for the configuration client model.
    @return
    @c true means the configuration client model was added successfully. \n
    @c false means adding the configuration client model failed.
*/
bool bt_mesh_model_add_configuration_client(
    uint16_t *model_handle, uint16_t element_index, config_client_evt_handler callback);

/*!
    @brief Sends a secure network beacon state get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_BEACON_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_beacon(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a secure network beacon state set request.
    @param[in] state means new secure network beacon state.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_BEACON_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_beacon(
    bt_mesh_feature_state_t state,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a composition data get request.
    @param[in] page means the page number of the composition data.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_COMPOSITION_DATA_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_composition_data(
    uint8_t page,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a default TTL get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_DEFAULT_TTL_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_default_ttl(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a default TTL set request.
    @param[in] default_ttl means the new default TTL value.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_DEFAULT_TTL_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_default_ttl(
    uint8_t default_ttl,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a GATT proxy state get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_GATT_PROXY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_gatt_proxy(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a GATT proxy state set request.
    @param[in] state means the new GATT proxy state.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_GATT_PROXY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_gatt_proxy(
    bt_mesh_feature_state_t state,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a key refresh phase get request.
    @param[in] netkey_index is the network key index.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_KEY_REFRESH_PHASE_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_key_refresh_phase(
    uint16_t netkey_index,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a key refresh phase set request.
    @param[in] netkey_index is the network key index.
    @param[in] transition is the new key refresh phase transition.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_KEY_REFRESH_PHASE_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_key_refresh_phase(
    uint16_t netkey_index, uint8_t transition,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a friend state get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_FRIEND_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_friend(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a friend state set request.
    @param[in] state means the new friend state.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_FRIEND_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_friend(
    bt_mesh_feature_state_t state,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a relay state get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_RELAY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_relay(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a relay state set request.
    @param[in] state means the new relay state.
    @param[in] retransmit_count means number of retransmissions on advertising bearer for each Network PDU relayed by the node.
    @param[in] retransmit_interval_steps means mumber of 10-millisecond steps between retransmissions.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_RELAY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_relay(
    bt_mesh_feature_state_t state, uint8_t retransmit_count, uint8_t retransmit_interval_steps,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a publication get request.
    @param[in] element_addr is the element address of the model.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_PUBLICATION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_model_publication(
    uint16_t element_addr, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a publication set request.
    @param[in] state is the publication state parameter struct pointer.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_PUBLICATION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_model_publication(
    const bt_mesh_model_publication_param_t *state,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a subscription add request.
    @param[in] element_addr is the element address of the model.
    @param[in] address is the address to add to the subscription list.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_ADDR, the address to add is invalid. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_add_model_subscription(
    uint16_t element_addr, bt_mesh_address_t address, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a subscription delete request.
    @param[in] element_addr is the element address of the model.
    @param[in] address is the address to delete from the subscription list.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_ADDR, the address to delete is invalid. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_delete_model_subscription(
    uint16_t element_addr, bt_mesh_address_t address, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a subscription delete all request.
    @param[in] element_addr is the element address of the model.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_delete_all_model_subscription(
    uint16_t element_addr, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a subscription overwrite request.
    @param[in] element_addr is the element address of the model.
    @param[in] address is the new address to add to the subscription list.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_ADDR, the address to overwirte is invalid. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_overwrite_model_subscription(
    uint16_t element_addr, bt_mesh_address_t address, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a SIG model subscription get request.
    @param[in] element_addr is the element address of the model.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_sig_model_subscription(
    uint16_t element_addr, uint16_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a vendor model subscription get request.
    @param[in] element_addr is the element address of the model.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_vendor_model_subscription(
    uint16_t element_addr, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a network key add request.
    @param[in] netkey_index is network key index.
    @param[in] netkey is the pointer to a 16-byte network key.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NETKEY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_add_net_key(
    uint16_t netkey_index, const uint8_t *netkey,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a network key delete request.
    @param[in] netkey_index is network key index.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NETKEY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_delete_net_key(
    uint16_t netkey_index,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a network key get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NETKEY_LIST
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_net_key(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a network key update request.
    @param[in] netkey_index is network key index.
    @param[in] netkey is the pointer to a 16-byte network key.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NETKEY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_update_net_key(
    uint16_t netkey_index, const uint8_t *netkey,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends an application key add request.
    @param[in] appkey_index is application key index.
    @param[in] netkey_index is network key index.
    @param[in] appkey is the pointer to a 16-byte application key.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_APPKEY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_add_app_key(
    uint16_t appkey_index, uint16_t netkey_index, const uint8_t *appkey,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends an application key update request.
    @param[in] appkey_index is application key index.
    @param[in] netkey_index is network key index.
    @param[in] appkey is the pointer to a 16-byte application key.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_APPKEY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_update_app_key(
    uint16_t appkey_index, uint16_t netkey_index, const uint8_t *appkey,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends an application key delete request.
    @param[in] appkey_index is application key index.
    @param[in] netkey_index is network key index.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_APPKEY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_delete_app_key(
    uint16_t appkey_index, uint16_t netkey_index,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends an application key(s) get request.
    @param[in] netkey_index is network key index.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_APPKEY_LIST
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_app_key(
    uint16_t netkey_index,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a model application bind request.
    @param[in] element_addr is the element address of the model.
    @param[in] appkey_index is application key index.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_APP_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_bind_model_app(
    uint16_t element_addr, uint16_t appkey_index, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends a model application unbind request.
    @param[in] element_addr is the element address of the model.
    @param[in] appkey_index is application key index.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_APP_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_unbind_model_app(
    uint16_t element_addr, uint16_t appkey_index, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends an SIG model application get request.
    @param[in] element_addr is the element address of the model.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_APP_LIST
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_sig_model_app(
    uint16_t element_addr, uint16_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Sends an vendor model application get request.
    @param[in] element_addr is the element address of the model.
    @param[in] model_id is the model identifier.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_MODEL_APP_LIST
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_vendor_model_app(
    uint16_t element_addr, uint32_t model_id,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a beacon get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NODE_RESET_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
    @warning This will make the node reset and be removed from the network.
*/
bt_mesh_status_t bt_mesh_model_configuration_client_reset_node(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a node identity state get request.
    @param[in] netkey_index is network key index.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NODE_IDENTITY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_node_identity(
    uint16_t netkey_index,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a node identity state set request.
    @param[in] netkey_index is network key index.
    @param[in] state means the new relay state.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NODE_IDENTITY_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_node_identity(
    uint16_t netkey_index, bt_mesh_feature_state_t state,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a heartbeat publication get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_HEARTBEAT_PUBLICATION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_heartbeat_publication(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a heartbeat publication set request.
    @param[in] publication is the heartbeat publication state.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_HEARTBEAT_PUBLICATION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_heartbeat_publication(
    const bt_mesh_heartbeat_publication_param_t *publication,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a heartbeat subscription get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_heartbeat_subscription(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a heartbeat subscription set request.
    @param[in] subscription is the heartbeat subscription state.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_HEARTBEAT_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_heartbeat_subscription(
    const bt_mesh_heartbeat_subscription_param_t *subscription,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a network transmit get request.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_HEARTBEAT_SUBSCRIPTION_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_network_transmit(
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a network transmit set request.
    @param[in] count means number of transmissions for each Network PDU originating from the node.
    @param[in] interval_steps means number of 10-millisecond steps between transmissions.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_NETWORK_TRANSMIT_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_set_network_transmit(
    uint8_t count, uint8_t interval_steps,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
    @brief Send a LPN poll timeout get request.
    @param[in] lpn_addr is the LPN address.
    @param[in] src_addr is the source address.
    @param[in] dst_addr is the destination address.
    @param[in] ttl is the TTL value of this request message.
    @param[in] msg_netkey_index is the network key that is used for this request message.
    @note Response: #CONFIG_CLIENT_EVT_LPN_POLL_TIMEOUT_STATUS
    @return
    #BT_MESH_SUCCESS, request sent successfully.\n
    #BT_MESH_ERROR_OOM, there is not enough memory to send this request. \n
    #BT_MESH_ERROR_INVALID_KEY, cannot find the corresponding device key according to dst_addr. \n
*/
bt_mesh_status_t bt_mesh_model_configuration_client_get_lpn_poll_timeout(
    uint16_t lpn_addr,
    uint16_t src_addr, uint16_t dst_addr, uint8_t ttl, uint16_t msg_netkey_index);

/*!
@}
*/

#endif // __BT_MESH_MODEL_CONFIGURATION_CLIENT_H__

