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

#ifndef __BT_MESH_MODEL_GENERIC_CLIENT_H__
#define __BT_MESH_MODEL_GENERIC_CLIENT_H__

#include "bt_mesh_device_property.h"
#include "bt_mesh_model_generic_opcodes.h"

/*!
     @defgroup PAGE_MESH_MODEL_GENERIC_CLIENT bt_mesh_model_generic_client.h
     @{
        @page PAGE_MESH_MODEL_GENERIC_CLIENT bt_mesh_model_generic_client.h
        bt_mesh_model_generic_client.h defines the BLE Mesh Generic Client Model APIs.
*/

/*!
     @brief Generic client event id
     @{
 */
typedef enum {
    BT_MESH_GENERIC_CLIENT_EVT_ONOFF_STATUS,                    /**< Event for generic onoff status. */
    BT_MESH_GENERIC_CLIENT_EVT_LEVEL_STATUS,                    /**< Event for generic level status. */
    BT_MESH_GENERIC_CLIENT_EVT_DEFAULT_TRANSITION_TIME_STATUS,  /**< Event for generic default transition time status. */
    BT_MESH_GENERIC_CLIENT_EVT_ONPOWERUP_STATUS,                /**< Event for generic onpowerup status. */
    BT_MESH_GENERIC_CLIENT_EVT_POWER_LEVEL_STATUS,              /**< Event for generic power level status. */
    BT_MESH_GENERIC_CLIENT_EVT_POWER_LAST_STATUS,               /**< Event for generic power last status. */
    BT_MESH_GENERIC_CLIENT_EVT_POWER_DEFAULT_STATUS,            /**< Event for generic power default status. */
    BT_MESH_GENERIC_CLIENT_EVT_POWER_RANGE_STATUS,              /**< Event for generic power range status. */
    BT_MESH_GENERIC_CLIENT_EVT_BATTERY_STATUS,                  /**< Event for generic battery status. */
    BT_MESH_GENERIC_CLIENT_EVT_LOCATION_GLOBAL_STATUS,          /**< Event for generic location global status. */
    BT_MESH_GENERIC_CLIENT_EVT_LOCATION_LOCAL_STATUS,           /**< Event for generic location local status. */
    BT_MESH_GENERIC_CLIENT_EVT_MANUFACTURER_PROPERTIES_STATUS,  /**< Event for generic manufacturer properties status. */
    BT_MESH_GENERIC_CLIENT_EVT_MANUFACTURER_PROPERTY_STATUS,    /**< Event for generic manufacturer property status. */
    BT_MESH_GENERIC_CLIENT_EVT_ADMIN_PROPERTIES_STATUS,         /**< Event for generic admin properties status. */
    BT_MESH_GENERIC_CLIENT_EVT_ADMIN_PROPERTY_STATUS,           /**< Event for generic admin property status. */
    BT_MESH_GENERIC_CLIENT_EVT_USER_PROPERTIES_STATUS,          /**< Event for generic user properties status. */
    BT_MESH_GENERIC_CLIENT_EVT_USER_PROPERTY_STATUS,            /**< Event for generic user property status. */
    BT_MESH_GENERIC_CLIENT_EVT_CLIENT_PROPERTIES_STATUS,        /**< Event for generic client properties status. */
} bt_mesh_generic_client_event_id_t;
/*!  @} */

/*!
     @brief Message parameter of generic onoff status
 */
typedef struct {
    uint8_t present_onoff;   /**< Indicate the present value of the Generic OnOff state. */
    uint8_t target_onoff;   /**< Indicate the target value of the Generic OnOff state (optional). */
    uint8_t remaining_time;   /**< Indicate the remaining time of transition. */
} __attribute__((packed)) bt_mesh_generic_client_evt_onoff_status_t;

/*!
     @brief Message parameter of generic level status
 */
typedef struct {
    int16_t present_level;   /**< Indicate the present value of the Generic Level state. */
    int16_t target_level;   /**< Indicate the target value of the Generic Level state (Optional). */
    uint8_t remaining_time;   /**< Indicate the remaining time of transition. */
} __attribute__((packed)) bt_mesh_generic_client_evt_level_status_t;

/*!
     @brief Message parameter of default transition time status
 */
typedef struct {
    uint8_t transition_time;   /**< The value of the Generic Default Transition Time state. */
} __attribute__((packed)) bt_mesh_generic_client_evt_default_transition_time_status_t;

/*!
     @brief Message parameter of generic on power up status
 */
typedef struct {
    uint8_t on_power_up;   /**< The value of the Generic OnPowerUp state. */
} __attribute__((packed)) bt_mesh_generic_client_evt_on_power_up_status_t;

/*!
     @brief Message parameter of generic power level status
 */
typedef struct {
    uint16_t present_power;   /**< Indicate the present value of the Generic Power Actual state. */
    uint16_t target_power;    /**< Indicate the target value of the Generic Power Actual state (Optional). */
    uint8_t remaining_time;   /**< Indicate the remaining time of transition. */
} __attribute__((packed)) bt_mesh_generic_client_evt_power_level_status_t;

/*!
     @brief Message parameter of generic power last status
 */
typedef struct {
    uint16_t power;   /**< The value of the Generic Power Last state. */
} __attribute__((packed)) bt_mesh_generic_client_evt_power_last_status_t;

/*!
     @brief Message parameter of generic power default status
 */
typedef struct {
    uint16_t power;   /**< The value of the Generic Power Default state. */
} __attribute__((packed)) bt_mesh_generic_client_evt_power_default_status_t;

/*!
     @brief Message parameter of generic power range status
 */
typedef struct {
    uint8_t status_code;  /**< Status Code for the requesting message. */
    uint16_t range_min;   /**< The value of the Generic Power Min field of the Generic Power Range state. */
    uint16_t range_max;   /**< The value of the Generic Power Max field of the Generic Power Range state. */
} __attribute__((packed)) bt_mesh_generic_client_evt_power_range_status_t;

/*!
     @brief Message parameter of generic battery status
 */
typedef struct {
    uint32_t battery_level: 8;          /**< The value of the Generic Battery Level state. */
    uint32_t time_to_discharge: 24;     /**< The value of the Generic Battery Time to Discharge state. */
    uint32_t time_to_charge: 24;        /**< The value of the Generic Battery Time to Charge state. */
    uint32_t flags_presence: 2;         /**< The value of the Generic Battery presence. */
    uint32_t flags_indicator: 2;        /**< The value of the Generic Battery indicator. */
    uint32_t flags_charging: 2;         /**< The value of the Generic Battery charging. */
    uint32_t flags_serviceability: 2;   /**< The value of the Generic Battery serviceablity. */
} __attribute__((packed)) bt_mesh_generic_client_evt_battery_status_t;

/*!
     @brief Message parameter of generic global location status
 */
typedef struct {
    uint32_t latitude;      /**< The value of the Generic Location Latitude state. */
    uint32_t longitude;     /**< The value of the Generic Location Longitude state. */
    uint16_t altitude;      /**< The value of the Generic Location Altitude state. */
} __attribute__((packed)) bt_mesh_generic_client_evt_global_location_status_t;

/*!
     @brief Message parameter of generic local location status
 */
typedef struct {
    uint16_t north;         /**< The value describes the North coordinate of the device using a local coordinate system. */
    uint16_t east;          /**< The value describes the East coordinate of the device using a local coordinate system. */
    uint16_t altitude;      /**< The value determines the altitude of the device relative to the generic location global altitude. */
    uint8_t floor_number;   /**< The value describes the floor number where the element is installed. */
    uint16_t uncertainty;   /**< The value describes the uncertianty of the location information the element exposes. */
} __attribute__((packed)) bt_mesh_generic_client_evt_local_location_status_t;

/*!
     @brief Message parameter of generic properties status
 */
typedef struct {
    uint16_t *property_ids; /**< The value of the Generic Properties state. Need to be freed. */
    uint16_t property_ids_length;   /**< The total number of property ids. */
} __attribute__((packed)) bt_mesh_generic_client_evt_properties_status_t;

/*!
     @brief Message parameter of generic property status
 */
typedef struct {
    uint16_t property_id;   /**< The value of property ID. */
    uint8_t access;         /**< The value of access type. */
    uint8_t *property_value;   /**< The value of the specified property id. Need to be freed. */
} __attribute__((packed)) bt_mesh_generic_client_evt_property_status_t;

/*!
     @brief The parameter of generic property setting
 */
typedef struct {
    bt_mesh_device_property_type_t type;   /**< The property type. */
    uint16_t id;            /**< Property ID identifying a property. */
    uint8_t *value;         /**< Raw value for the property */
    uint16_t length;        /**< The length of value. */
    union {
        bt_mesh_device_property_user_access_t user_access;     /**< Enumeration indicating user access. */
        bt_mesh_device_property_admin_access_t admin_user_access;      /**< Enumeration indicating user access. */
        bt_mesh_device_property_manufacturer_access_t manu_user_access;    /**< Enumeration indicating user access. */
    } access;
} __attribute__((packed)) bt_mesh_property_setting_params_t;

/*!
     @brief Generic client event structure.
 */
typedef struct {
    bt_mesh_generic_client_event_id_t evt_id;   /**<  Generic client event ID. */
    union {
        bt_mesh_generic_client_evt_onoff_status_t                   onoff_status;    /**<  Generic OnOff status */
        bt_mesh_generic_client_evt_level_status_t                   level_status;    /**<  Generic Level status */
        bt_mesh_generic_client_evt_default_transition_time_status_t default_transition_time_status;    /**<  Generic default transition time status */
        bt_mesh_generic_client_evt_on_power_up_status_t             on_powerup_status;    /**<  Generic on power up status */
        bt_mesh_generic_client_evt_power_level_status_t             power_level_status;    /**<  Generic power level status */
        bt_mesh_generic_client_evt_power_last_status_t              power_last_status;    /**<  Generic power last status */
        bt_mesh_generic_client_evt_power_default_status_t           power_default_status;    /**<  Generic power default status */
        bt_mesh_generic_client_evt_power_range_status_t             power_range_status;    /**<  Generic power range status */
        bt_mesh_generic_client_evt_battery_status_t                 battery_status;         /**<  Generic battery status */
        bt_mesh_generic_client_evt_global_location_status_t         global_location_status; /**<  Generic global location status */
        bt_mesh_generic_client_evt_local_location_status_t          local_location_status; /**<  Generic local location status */
        bt_mesh_generic_client_evt_properties_status_t              properties_status; /**<  Generic properties status */
        bt_mesh_generic_client_evt_property_status_t                property_status; /**<  Generic property status */
    } data;
} bt_mesh_generic_client_evt_t;

/** @brief  This defines the configuration client event handler prototype.
 *  @param[in] model_handle is the model handle which wants to handle these messages.
 *  @param[in] msg is the original received message.
 *  @param[in] event is the parsed generic client event.
 *  @return NONE
 * #bt_mesh_model_add_generic_level_client,\n
 * #bt_mesh_model_add_generic_default_transition_time_client,\n
 * #bt_mesh_model_add_generic_power_onoff_client,\n
 * #bt_mesh_model_add_generic_power_level_client,\n
 * #bt_mesh_model_add_generic_battery_client,\n
 * #bt_mesh_model_add_generic_location_client,\n
 * #bt_mesh_model_add_generic_property_client,
 */
typedef void (*bt_mesh_generic_client_evt_handler)(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const bt_mesh_generic_client_evt_t *event);

/*!
    @brief Add a Generic OnOff client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_onoff_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Level client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_level_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Default Transition client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_default_transition_time_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Power OnOff client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_power_onoff_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Power Level client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_power_level_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Battery client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_battery_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Location client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_location_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);

/*!
    @brief Add a Generic Property client model
    @param[out] model_handle is the handle of this model.
    @param[in] element_index is the specified element for this model.
    @param[in] callback is the message handler for this model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_add_generic_property_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_generic_client_evt_handler callback);


/*!
 * @brief Sends a request to get Generic OnOff state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_onoff_client_get(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set Generic OnOff state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
 * @note buffer should be composed in this format
    Field | Size (octets) | Notes
    ------|---------------|-------
    OnOff | 1 | The target value of the Generic OnOff state
    TID   | 1 | Transaction Identifier 
    Transition Time | 1 | Transition time (Optional)
    Delay | 1 | Delay time. If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
*/
bt_mesh_status_t bt_mesh_model_generic_onoff_client_set(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get Generic Level state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_level_client_get(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set Generic Level state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
 * @note buffer should be composed in this format
    Field | Size (octets) | Notes
    ------|---------------|-------
    Level | 2 | The target value of the Generic Level state
    TID   | 1 | Transaction Identifier 
    Transition Time | 1 | Transition time (Optional)
    Delay | 1 | Delay time. If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
*/
bt_mesh_status_t bt_mesh_model_generic_level_client_set(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to set the Generic Level state by a relative value.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
 * @note buffer should be composed in this format
    Field | Size (octets) | Notes
    ------|---------------|-------
    Delta Level | 4 | The Delta change of the Generic Level state 
    TID   | 1 | Transaction Identifier 
    Transition Time | 1 | Transition time (Optional)
    Delay | 1 | Delay time. If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
*/
bt_mesh_status_t bt_mesh_model_generic_level_client_set_delta(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to start a process of changing the Generic Level state of an element with a defined transition speed.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
 * @note buffer should be composed in this format
    Field | Size (octets) | Notes
    ------|---------------|-------
    Delta Level | 2 | The Delta Level step to calculate Move speed for the Generic Level state
    TID   | 1 | Transaction Identifier 
    Transition Time | 1 | Transition time (Optional)
    Delay | 1 | Delay time. If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
*/
bt_mesh_status_t bt_mesh_model_generic_level_client_set_move(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get Generic Default Transition Time state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_default_transition_time_client_get(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set Generic Default Transition Time state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] transition_time is the value of the Generic Default Transition Time state. 
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_default_transition_time_client_set(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t transition_time, bool reliable);

/*!
 * @brief Sends a request to get Generic OnPowerUp state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_onoff_client_get_on_powerup(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set Generic OnPowerUp state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] on_powerup is the value of the value of the Generic OnPowerUp state.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_onoff_setup_client_set_on_powerup(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    bt_mesh_model_generic_on_powerup_t on_powerup, bool reliable);

/*!
 * @brief Sends a request to get Generic Power Actual state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_client_get_actual(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set Generic Power Actual state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
 * @note buffer should be composed in this format
    Field | Size (octets) | Notes
    ------|---------------|-------
    Power | 2 | The target value of the Generic Power Actual state.
    TID   | 1 | Transaction Identifier 
    Transition Time | 1 | Transition time (Optional)
    Delay | 1 | Delay time. If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_client_set_actual(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get Generic Power Last state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_client_get_last(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to get Generic Power Default state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_client_get_default(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to get Generic Power Range state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_client_get_range(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set Generic Power Default state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] onpowerup is The value of the Generic Power Default state.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_setup_client_set_default(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t onpowerup, bool reliable);

/*!
 * @brief Sends a request to set Generic Power Range state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] range_min is the value of the Generic Power Min field of the Generic Power Range state.
 * @param[in] range_max is the value of the Generic Power Range Max field of the Generic Power Range state.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_setup_client_set_range(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t range_min, uint16_t range_max, bool reliable);

/*!
 * @brief Sends a request to get Generic Battery state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_battery_client_get(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to get the global related fields of the Generic Location state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_location_client_get_global(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to get the local related fields of the Generic Location state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_location_client_get_local(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the global related fields of the Generic Location state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] global_location is the global related fields of Generic Location state.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_location_client_set_global(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    bt_mesh_generic_client_evt_global_location_status_t *global_location, bool reliable);

/*!
 * @brief Sends a request to set the local related fields of the Generic Location state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] local_location is the local related fields of Generic Location state.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_location_client_set_local(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    bt_mesh_generic_client_evt_local_location_status_t *local_location, bool reliable);

/*!
 * @brief Sends a request to get the list of Generic User/Admin/Manufacturer Property states
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] type is the property type.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_property_client_get_properties(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    bt_mesh_device_property_type_t type);

/*!
 * @brief Sends a request to get the list of Generic Client Property states.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] property_id is a starting Client Property ID present within an element.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_property_client_get_client_properties(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta, uint16_t property_id);

/*!
 * @brief Sends a request to get the Generic User/Admin/Manufacturer Property state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] type is the property type.
 * @param[in] property_id is the property ID identifying a Generic User/Admin/Manufacture Property.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_property_client_get_property(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    bt_mesh_device_property_type_t type, uint16_t property_id);

/*!
 * @brief Sends a request to set Generic OnOff state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] property is the property data.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_generic_property_client_set_property(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    bt_mesh_property_setting_params_t *property, bool reliable);

/*!
@}
*/

#endif // __BT_MESH_MODEL_GENERIC_CLIENT_H__

