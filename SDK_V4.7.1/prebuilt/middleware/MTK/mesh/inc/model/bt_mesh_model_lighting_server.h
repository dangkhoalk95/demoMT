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


#ifndef __BT_MESH_MODEL_LIGHTING_H__
#define __BT_MESH_MODEL_LIGHTING_H__

/*!
     @defgroup PAGE_MESH_MODEL_LIGHTING_SERVER bt_mesh_model_lighting_server.h
     @{
        @page PAGE_MESH_MODEL_LIGHTING_SERVER bt_mesh_model_lighting_server.h
        bt_mesh_model_lighting_server.h defines the SIG Mesh Lighting Model APIs.
*/

#include "bt_mesh_access.h"

/*!
    @brief Add a lighting lightness server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting lightness server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting lightness server model successfully. \n
    @c false means adding lighting lightness server model failed.
*/
bool bt_mesh_model_lighting_add_lightness_server(
    uint16_t *model_handle, uint16_t element_index,
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting lightness setup server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting lightness setup server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting lightness setup server model successfully. \n
    @c false means adding lighting lightness setup server model failed.
*/
bool bt_mesh_model_lighting_add_lightness_setup_server(
    uint16_t *model_handle, uint16_t element_index,
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting CTL server model.
    @param[out] model_handle is the handle of this added model.
    @param[out] element_count is the element count that are created this model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting CTL server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting CTL server model successfully. \n
    @c false means adding lighting CTL server model failed.
    @note Element indexes are consecutive for all newly created elements.
*/
bool bt_mesh_model_lighting_add_ctl_server(
    uint16_t *model_handle, uint8_t *element_count, uint16_t element_index, 
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting CTL temperature server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting CTL temperature server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting CTL temperature server model successfully. \n
    @c false means adding lighting CTL temperature server model failed.
*/
bool bt_mesh_model_lighting_add_ctl_temperature_server(
    uint16_t *model_handle, uint16_t element_index,
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting CTL setup server model.
    @param[out] model_handle is the handle of this added model.
    @param[out] element_count is the element count that are created this model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting CTL setup server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting CTL setup server model successfully. \n
    @c false means adding lighting CTL setup server model failed.
    @note Element indexes are consecutive for all newly created elements.
*/
bool bt_mesh_model_lighting_add_ctl_setup_server(
    uint16_t *model_handle, uint8_t *element_count, uint16_t element_index, 
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting HSL server model.
    @param[out] model_handle is the handle of this added model.
    @param[out] element_count is the element count that are created this model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting HSL server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting HSL server model successfully. \n
    @c false means adding lighting HSL server model failed.
    @note Element indexes are consecutive for all newly created elements.
*/
bool bt_mesh_model_lighting_add_hsl_server(
    uint16_t *model_handle, uint8_t *element_count, uint16_t element_index, 
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting HSL hue server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting HSL hue server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting HSL hue server model successfully. \n
    @c false means adding lighting HSL hue server model failed.
*/
bool bt_mesh_model_lighting_add_hsl_hue_server(
    uint16_t *model_handle, uint16_t element_index,
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting HSL saturation server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting HSL saturation server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting HSL saturation server model successfully. \n
    @c false means adding lighting HSL saturation server model failed.
*/
bool bt_mesh_model_lighting_add_hsl_saturation_server(
    uint16_t *model_handle, uint16_t element_index,
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Add a lighting HSL setup server model.
    @param[out] model_handle is the handle of this added model.
    @param[out] element_count is the element count that are created this model.
    @param[in] element_index is the index of element that this model to be added in.
    @param[in] callback is the message handler for lighting HSL setup server model.
    @param[in] publish_timeout_cb is the periodic publishing timeout callback.
    @return
    @c true means adding lighting HSL setup server model successfully. \n
    @c false means adding lighting HSL setup server model failed.
    @note Element indexes are consecutive for all newly created elements.
*/
bool bt_mesh_model_lighting_add_hsl_setup_server(
    uint16_t *model_handle, uint8_t *element_count, uint16_t element_index, 
    bt_mesh_access_msg_handler callback, bt_mesh_access_publish_timeout_cb_t publish_timeout_cb);

/*!
    @brief Sends a lighting lightness status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_lightness_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting lightness linear status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_lightness_linear_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting lightness last status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] lightness is the message payload to be sent.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_lightness_last_status(uint16_t model_handle, uint16_t lightness,
        const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting lightness default status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] lightness is the message payload to be sent.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_lightness_default_status(uint16_t model_handle, uint16_t lightness,
        const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting lightness range status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] status is the status code for the requesting message.
    @param[in] range_min is the value of Range Min for light lightness range state.
    @param[in] range_max is the value of Range Max for light lightness range state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_lightness_range_status(uint16_t model_handle, uint8_t status,
        uint16_t range_min, uint16_t range_max, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting CTL status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_ctl_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting CTL temperature range status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] status is the status code for the requesting message.
    @param[in] range_min is the value of Range Min for CTL temperature range state.
    @param[in] range_max is the value of Range Max for CTL temperature range state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_ctl_temperature_range_status(uint16_t model_handle, uint8_t status,
        uint16_t range_min, uint16_t range_max, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting CTL temperature status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_ctl_temperature_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting CTL default status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] lightness is the value of lightness default state.
    @param[in] temperature is the value of CTL temperature default state.
    @param[in] delta_uv is the value of CTL delta UV default state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_ctl_default_status(uint16_t model_handle, uint16_t lightness,
        uint16_t temperature, uint16_t delta_uv, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting HSL status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_hsl_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting HSL target status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_hsl_target_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting HSL hue status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_hsl_hue_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting HSL saturation status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] buffer is the message payload to be sent.
    @param[in] buffer_length is the message payload length.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_hsl_saturation_status(uint16_t model_handle, uint8_t *buffer,
        uint8_t buffer_length, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting HSL default status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] lightness is the the value of lightness default state.
    @param[in] hue is the value of Light HSL Hue default state.
    @param[in] saturation is the value of Light HSL saturation default state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_hsl_default_status(uint16_t model_handle, uint16_t lightness,
        uint16_t hue, uint16_t saturation, const bt_mesh_access_message_rx_t *msg);

/*!
    @brief Sends a lighting HSL range status message.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] status is the status code for the requesting message.
    @param[in] hue_range_min is the value of Hue Range Min field of Light HSL Hue Range state.
    @param[in] hue_range_max is the value of Hue Range Max field of Light HSL Hue Range state.
    @param[in] saturation_range_min is value of Saturation Range Min field of Light HSL Saturation Range state.
    @param[in] saturation_range_max is value of Saturation Range Max field of Light HSL Saturation Range state.
    @param[in] msg is the received message which this API replies to.
    @return
    #BT_MESH_SUCCESS, requesting message is performed successfully. \n
    #BT_MESH_ERROR_INVALID_ADDR, cannot find corresponding address. \n
    #BT_MESH_ERROR_OOM, not enough memory for sending reply
*/
bt_mesh_status_t bt_mesh_model_lighting_hsl_range_status(uint16_t model_handle, uint8_t status,
        uint16_t hue_range_min, uint16_t hue_range_max, uint16_t saturation_range_min,
        uint16_t saturation_range_max, const bt_mesh_access_message_rx_t *msg);

/*!
@}
*/

#endif // __BT_MESH_MODEL_LIGHTING_SERVER_H__

