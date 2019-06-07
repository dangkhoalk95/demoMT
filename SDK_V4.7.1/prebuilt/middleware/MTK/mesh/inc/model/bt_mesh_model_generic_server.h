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

#ifndef __BT_MESH_MODEL_GENERIC_SERVER_H__
#define __BT_MESH_MODEL_GENERIC_SERVER_H__

/*!
     @defgroup PAGE_MESH_MODEL_GENERIC_SERVER bt_mesh_model_generic_server.h
     @{
        @page PAGE_MESH_MODEL_GENERIC_SERVER bt_mesh_model_generic_server.h
        bt_mesh_model_generic_server.h defines the BLE Mesh Generic Model APIs.
*/

/*!
    @brief Add a generic onOff server model
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic onOff server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic onOff server model successfully. \n
    @c false means adding generic onOff server model failed.
*/
bool bt_mesh_model_add_generic_onoff_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic level server model
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic level server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic level server model successfully. \n
    @c false means adding generic level server model failed.
*/
bool bt_mesh_model_add_generic_level_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic default transition server model
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic default transition server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic default transition server model successfully. \n
    @c false means adding generic default transition server model failed.
*/
bool bt_mesh_model_add_generic_default_transition_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic power onOff server model
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic power onOff server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic power onOff server model successfully. \n
    @c false means adding generic power onOff server model failed.
*/
bool bt_mesh_model_add_generic_power_onoff_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic power onOff setup server model
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic power onOff setup server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic power onOff setup server model successfully. \n
    @c false means adding generic power onOff setup server model failed.
*/
bool bt_mesh_model_add_generic_power_onoff_setup_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic power onOff level server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic power onOff level server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic power onOff level server model successfully. \n
    @c false means adding generic power onOff level server model failed.
*/
bool bt_mesh_model_add_generic_power_onoff_level_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic power onOff level setup server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic power onOff level setup server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic power onOff level setup server model successfully. \n
    @c false means adding generic power onOff level setup server model failed.
*/
bool bt_mesh_model_add_generic_power_onoff_level_setup_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic battery server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic battery server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic battery server model successfully. \n
    @c false means adding generic battery server model failed.
*/
bool bt_mesh_model_add_generic_battery_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic location server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic location server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic location server model successfully. \n
    @c false means adding generic location server model failed.
*/
bool bt_mesh_model_add_generic_location_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic location setup server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic location setup server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic location setup server model successfully. \n
    @c false means adding generic location setup server model failed.
*/
bool bt_mesh_model_add_generic_location_setup_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic user property server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic user property server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic user property server model successfully. \n
    @c false means adding generic user property server model failed.
*/
bool bt_mesh_model_add_generic_user_property_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic Admin property server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic Admin property server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic Admin property server model successfully. \n
    @c false means adding generic Admin property server model failed.
*/
bool bt_mesh_model_add_generic_admin_property_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic manufacturer property server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic manufacturer property server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic manufacturer property server model successfully. \n
    @c false means adding generic manufacturer property server model failed.
*/
bool bt_mesh_model_add_generic_manufacturer_property_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a generic client property server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for generic client property server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding generic client property server model successfully. \n
    @c false means adding generic client property server model failed.
*/
bool bt_mesh_model_add_generic_client_property_server(uint16_t *model_handle, uint16_t element_index,
        bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Send a generic onOff status message
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent
    @param[in] buffer_length is the message payload length
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_generic_on_off_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic level status message
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent
    @param[in] buffer_length is the message payload length
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_generic_level_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic default transition time status message
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] transition_time is the message payload to be sent
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_generic_default_transition_time_status(uint16_t model_handle,
        uint8_t transition_time, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic on power up status message
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] on_power_up is the message payload to be sent
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_generic_onpowerup_status(uint16_t model_handle,
        uint8_t on_power_up, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic power level status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_power_level_status(uint16_t model_handle,
        uint8_t *buffer, uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic power last status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] power is the value of the Generic Power Last state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_power_last_status(uint16_t model_handle,
        uint16_t power, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic power default status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] power is the value of the Generic Power Default state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_power_default_status(uint16_t model_handle,
        uint16_t power, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic power range status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] status is the status code for the requesting message.
    @param[in] range_min is the minimum value of the Generic Power Range state.
    @param[in] range_max is the maximum value of the Generic Power Range state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_power_range_status(uint16_t model_handle,
        uint8_t status, uint16_t range_min, uint16_t range_max, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic battery status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] battery_level is the value of the Generic Battery Level state.
    @param[in] time_to_discharge is a 3-byte value representing the Generic Battery Time to Discharge state.
    @param[in] time_to_charge is 3-byte value representing the Generic Battery Time to Charge state.
    @param[in] flags is the value of the Generic Battery Flags state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_battery_status(uint16_t model_handle,
        uint8_t battery_level, uint8_t *time_to_discharge, uint8_t *time_to_charge, uint8_t flags,
        const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic location global status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] global_latitude is the global coordinates(latitude).
    @param[in] global_longitude is the global coordinates(longitude).
    @param[in] global_altitude is the global altitude.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_location_global_status(uint16_t model_handle,
        uint32_t global_latitude, uint32_t global_longitude, uint16_t global_altitude,
        const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic location local status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] local_north is the local coordinates(North).
    @param[in] local_east is the local coordinates(East).
    @param[in] local_altitude is the Local Altitude.
    @param[in] floor_number is the floor number.
    @param[in] uncertainty is the value of Generic Location Uncertainty state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_location_local_status(uint16_t model_handle,
        uint16_t local_north, uint16_t local_east, uint16_t local_altitude, uint8_t floor_number,
        uint16_t uncertainty, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic user properties status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] user_property_ids is a sequence of User Property IDs within an element.
    @param[in] id_count is the number of user propery IDs.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_user_properties_status(uint16_t model_handle,
        uint16_t *user_property_ids, uint8_t id_count, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic user property status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_user_property_status(uint16_t model_handle,
        uint8_t *buffer, uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic admin properties status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] admin_property_ids is a sequence of Admin Property IDs within an element.
    @param[in] id_count is the number of Admin propery IDs.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_admin_properties_status(uint16_t model_handle,
        uint16_t *admin_property_ids, uint8_t id_count, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic admin property status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_admin_property_status(uint16_t model_handle,
        uint8_t *buffer, uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic manufacturer properties status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] manufacturer_property_ids is a sequence of Manufacturer Property IDs within an element.
    @param[in] id_count is the number of Manufacturer propery IDs.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_manufacturer_properties_status(uint16_t model_handle,
        uint16_t *manufacturer_property_ids, uint8_t id_count, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic manufacturer property status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_manufacturer_property_status(uint16_t model_handle,
        uint8_t *buffer, uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Send a generic client properties status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] client_property_ids is a sequence of client Property IDs within an element.
    @param[in] id_count is the number of client propery IDs.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding model_handle. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply.
*/
bt_mesh_status_t bt_mesh_model_generic_client_properties_status(uint16_t model_handle,
        uint16_t *client_property_ids, uint8_t id_count, const bt_mesh_access_message_rx_t *msg);

/*!
@}
*/

#endif // __BT_MESH_MODEL_GENERIC_SERVER_H__

