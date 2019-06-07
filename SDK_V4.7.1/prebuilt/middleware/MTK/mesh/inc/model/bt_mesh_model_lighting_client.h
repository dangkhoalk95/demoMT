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


#ifndef __BT_MESH_MODEL_LIGHTING_CLIENT_H__
#define __BT_MESH_MODEL_LIGHTING_CLIENT_H__

/*!
     @defgroup PAGE_MESH_MODEL_LIGHTING_CLIENT bt_mesh_model_lighting_client.h
     @{
        @page PAGE_MESH_MODEL_LIGHTING_CLIENT bt_mesh_model_lighting_client.h
        bt_mesh_model_lighting_client.h defines the SIG Mesh Lighting Client Model APIs.
*/

#include "bt_mesh_access.h"

/*!
     @brief Lighting lightness client event id
     @{
 */
typedef enum {
    BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LIGHTNESS_STATUS,    /**< Event for lightness status. */
    BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LINEAR_STATUS,       /**< Event for lightness linear status. */
    BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LAST_STATUS,         /**< Event for lightness last status. */
    BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_DEFAULT_STATUS,      /**< Event for lightness default status. */
    BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_RANGE_STATUS         /**< Event for lightness range status. */
} bt_mesh_lightness_client_event_id_t;
/*!  @} */

/*!
     @brief Lighting CTL client event id
     @{
 */
typedef enum {
    BT_MESH_LIGHT_CTL_CLIENT_EVT_STATUS,                    /**< Event for CTL status. */
    BT_MESH_LIGHT_CTL_CLIENT_EVT_TEMPERATURE_RANGE_STATUS,  /**< Event for CTL temperature range status. */
    BT_MESH_LIGHT_CTL_CLIENT_EVT_TEMPERATURE_STATUS,        /**< Event for CTL temperature status. */
    BT_MESH_LIGHT_CTL_CLIENT_EVT_DEFAULT_STATUS             /**< Event for CTL default status. */
} bt_mesh_ctl_client_event_id_t;
/*!  @} */

/*!
     @brief Lighting HSL client event id
     @{
 */
typedef enum {
    BT_MESH_LIGHT_HSL_CLIENT_EVT_HUE_STATUS,        /**< Event for HSL hue status. */
    BT_MESH_LIGHT_HSL_CLIENT_EVT_SATURATION_STATUS, /**< Event for HSL saturation status. */
    BT_MESH_LIGHT_HSL_CLIENT_EVT_STATUS,            /**< Event for HSL status. */
    BT_MESH_LIGHT_HSL_CLIENT_EVT_TARGET_STATUS,     /**< Event for HSL target status. */
    BT_MESH_LIGHT_HSL_CLIENT_EVT_DEFAULT_STATUS,    /**< Event for HSL default status. */
    BT_MESH_LIGHT_HSL_CLIENT_EVT_RANGE_STATUS       /**< Event for HSL range status. */
} bt_mesh_hsl_client_event_id_t;
/*!  @} */

/** @brief Event parameter of lightness client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LIGHTNESS_STATUS. */
typedef struct {
    uint16_t present_lightness; /**< The present value of the Light Lightness Actual state. */
    uint16_t target_lightness;  /**< The present value of the Light Lightness Actual state. */
    uint8_t remaining_time;     /**< The remaining time. */
} __attribute__((packed)) bt_mesh_lightness_client_evt_status_t;

/** @brief Event parameter of lightness client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LINEAR_STATUS. */
typedef struct {
    uint16_t present_lightness; /**< The present value of the Light Lightness Linear state. */
    uint16_t target_lightness;  /**< The present value of the Light Lightness Linear state. */
    uint8_t remaining_time;     /**< The remaining time. */
} __attribute__((packed)) bt_mesh_lightness_client_evt_linear_status_t;

/** @brief Event parameter of lightness client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LAST_STATUS. */
typedef struct {
    uint16_t lightness;     /**< The value of the Light Lightness Last state. */
} __attribute__((packed)) bt_mesh_lightness_client_evt_last_status_t;

/** @brief Event parameter of lightness client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_DEFAULT_STATUS. */
typedef struct {
    uint16_t lightness;     /**< The value of the Light Lightness Default state. */
} __attribute__((packed)) bt_mesh_lightness_client_evt_default_status_t;

/** @brief Event parameter of lightness client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_RANGE_STATUS. */
typedef struct {
    uint8_t status_code;    /**< Status Code for the requesting message. */
    uint16_t range_min;     /**< The value of the Lightness Range Min field of the Light Lightness Range state. */
    uint16_t range_max;     /**< The value of the Lightness Range Max field of the Light Lightness Range state. */
} __attribute__((packed)) bt_mesh_lightness_client_evt_range_status_t;

/** @brief Event parameter of CTL client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_STATUS. */
typedef struct {
    uint16_t present_ctl_lightness;     /**< The present value of the Light CTL Lightness state. */
    uint16_t present_ctl_temperature;   /**< The present value of the Light CTL Temperature state. */
    uint16_t target_ctl_lightness;      /**< The target value of the Light CTL Lightness state (Optional) */
    uint16_t target_ctl_temperature;    /**< The target value of the Light CTL Temperature state (Optional) */
    uint8_t remaining_time;             /**< The remaining time. */
} __attribute__((packed)) bt_mesh_ctl_client_evt_status_t;

/** @brief Event parameter of CTL client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_TEMPERATURE_STATUS. */
typedef struct {
    uint16_t present_ctl_temperature;   /**< The present value of the Light CTL Temperature state. */
    uint16_t present_ctl_delta_uv;      /**< The present value of the Light CTL Delta UV state. */
    uint16_t target_ctl_temperature;    /**< The target value of the Light CTL Temperature state (Optional) */
    uint16_t target_ctl_delta_uv;       /**< The target value of the Light CTL Delta UV state (Optional) */
    uint8_t remaining_time;             /**< The remaining time. */
} __attribute__((packed)) bt_mesh_ctl_client_evt_temperature_status_t;

/** @brief Event parameter of CTL client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_TEMPERATURE_RANGE_STATUS. */
typedef struct {
    uint8_t status_code;    /**< Status Code for the requesting message. */
    uint16_t range_min;     /**< The value of the Temperature Range Min field of the Light CTL Temperature Range state. */
    uint16_t range_max;     /**< The value of the Temperature Range Max field of the Light CTL Temperature Range state. */
} __attribute__((packed)) bt_mesh_ctl_client_evt_temperature_range_status_t;

/** @brief Event parameter of CTL client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_DEFAULT_STATUS. */
typedef struct {
    uint16_t lightness;     /**< The value of the Light Lightness Default state. */
    uint16_t temperature;   /**< The value of the Light CTL Temperature Default state. */
    uint16_t delta_uv;      /**< The value of the Light CTL Delta UV Default state. */
} __attribute__((packed)) bt_mesh_ctl_client_evt_default_status_t;

/* HSL */
/** @brief Event parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_STATUS and @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_TARGET_STATUS*/
typedef struct {
    uint16_t lightness;     /**< The value of the Light HSL Lightness state. */
    uint16_t hue;           /**< The value of the Light HSL Hue state. */
    uint16_t saturation;    /**< The value of the Light HSL Saturation state. */
    uint8_t remaining_time; /**< The remaining time. (Optional)*/
} __attribute__((packed)) bt_mesh_hsl_client_evt_status_t;

/** @brief Event parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_HUE_STATUS. */
typedef struct {
    uint16_t present_hue;   /**< The present value of the Light HSL Hue state. */
    uint16_t target_hue;    /**< The target value of the Light HSL Hue state. (Optional)*/
    uint8_t remaining_time; /**< The remaining time. Valid when target_hue exists.*/
} __attribute__((packed)) bt_mesh_hsl_client_evt_hue_status_t;

/** @brief Event parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_SATURATION_STATUS. */
typedef struct {
    uint16_t present_saturation;    /**< The present value of the Light HSL Saturation state. */
    uint16_t target_saturation;     /**< The target value of the Light HSL Saturation state. (Optional)*/
    uint8_t remaining_time;         /**< The remaining time. Valid when target_saturation exists.*/
} __attribute__((packed)) bt_mesh_hsl_client_evt_saturation_status_t;

/** @brief Event parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_DEFAULT_STATUS. */
typedef struct {
    uint16_t lightness;     /**< The value of the Light Lightness Default state. */
    uint16_t hue;           /**< The value of the Light Hue Default state. */
    uint16_t saturation;    /**< The value of the Light Saturation Default state.*/
} __attribute__((packed)) bt_mesh_hsl_client_evt_default_status_t;

/** @brief Event parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_RANGE_STATUS. */
typedef struct {
    uint8_t status_code;        /**< The value of the Hue Range Min field of the Light HSL Hue Range state. */
    uint16_t hue_min;           /**< The value of the Hue Range Min field of the Light HSL Hue Range state. */
    uint16_t hue_max;           /**< The value of the Hue Range Max field of the Light HSL Hue Range state. */
    uint16_t saturation_min;    /**< The value of the Saturation Range Min field of the Light HSL Saturation Range state. */
    uint16_t saturation_max;    /**< The value of the Saturation Range Max field of the Light HSL Saturation Range state. */
} __attribute__((packed)) bt_mesh_hsl_client_evt_range_status_t;

/*!
     @brief Light Lightness client event structure.
 */
typedef struct {
    bt_mesh_lightness_client_event_id_t evt_id; /**<  Light Lightness client event ID. */
    union {
        bt_mesh_lightness_client_evt_status_t status;                 /**<  parameter of health client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LIGHTNESS_STATUS */
        bt_mesh_lightness_client_evt_linear_status_t linear_status;   /**<  parameter of health client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LINEAR_STATUS */
        bt_mesh_lightness_client_evt_last_status_t last_status;       /**<  parameter of health client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LAST_STATUS */
        bt_mesh_lightness_client_evt_default_status_t default_status; /**<  parameter of health client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_DEFAULT_STATUS */
        bt_mesh_lightness_client_evt_range_status_t range_status;     /**<  parameter of health client model event @ref BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_RANGE_STATUS */
    } data;
} bt_mesh_lightness_client_evt_t;

/*!
     @brief Light CTL client event structure.
 */
typedef struct {
    bt_mesh_ctl_client_event_id_t evt_id;       /**<  Light CTL client event ID. */
    union {
        bt_mesh_ctl_client_evt_status_t status;       /**<  parameter of health client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_STATUS */
        bt_mesh_ctl_client_evt_temperature_range_status_t temperature_range_status;       /**<  parameter of health client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_TEMPERATURE_RANGE_STATUS */
        bt_mesh_ctl_client_evt_temperature_status_t temperature_status;   /**<  parameter of health client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_TEMPERATURE_STATUS */
        bt_mesh_ctl_client_evt_default_status_t default_status; /**<  parameter of health client model event @ref BT_MESH_LIGHT_CTL_CLIENT_EVT_DEFAULT_STATUS */
    } data;
} bt_mesh_ctl_client_evt_t;

/*!
     @brief Light HSL client event structure.
 */
typedef struct {
    bt_mesh_hsl_client_event_id_t evt_id;       /**<  Light HSL client event ID. */
    union {
        bt_mesh_hsl_client_evt_status_t status;       /**<  parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_STATUS and @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_TARGET_STATUS */
        bt_mesh_hsl_client_evt_hue_status_t hue_status;       /**<  parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_HUE_STATUS */
        bt_mesh_hsl_client_evt_saturation_status_t saturation_status; /**<  parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_SATURATION_STATUS */
        bt_mesh_hsl_client_evt_default_status_t default_status;       /**<  parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_DEFAULT_STATUS */
        bt_mesh_hsl_client_evt_range_status_t range_status; /**<  parameter of HSL client model event @ref BT_MESH_LIGHT_HSL_CLIENT_EVT_RANGE_STATUS */
    } data;
} bt_mesh_hsl_client_evt_t;

/** @brief  This defines the lightness client event handler prototype.
 *  @param[in] model_handle is the model handle which needs to handle these messages.
 *  @param[in] msg is the original received message.
 *  @param[in] event is the parsed lightness client event.
 *  @return NONE
 */
typedef void (*bt_mesh_lightness_client_evt_handler)(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const bt_mesh_lightness_client_evt_t *event);

/** @brief  This defines the CTL client event handler prototype.
 *  @param[in] model_handle is the model handle which needs to handle these messages.
 *  @param[in] msg is the original received message.
 *  @param[in] event is the parsed CTL client event.
 *  @return NONE
 */
typedef void (*bt_mesh_ctl_client_evt_handler)(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const bt_mesh_ctl_client_evt_t *event);

/** @brief  This defines the HSL client event handler prototype.
 *  @param[in] model_handle is the model handle which needs to handle these messages.
 *  @param[in] msg is the original received message.
 *  @param[in] event is the parsed HSL client event.
 *  @return NONE
 */
typedef void (*bt_mesh_hsl_client_evt_handler)(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const bt_mesh_hsl_client_evt_t *event);

/*!
    @brief Add a lightness client model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the specified element for adding lightness client model.
    @param[in] callback is the message handler for health client model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_lighting_add_lightness_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_lightness_client_evt_handler callback);

/*!
    @brief Add a CTL client model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the specified element for adding CTL client model.
    @param[in] callback is the message handler for CTL client model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_lighting_add_ctl_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_ctl_client_evt_handler callback);

/*!
    @brief Add a HSL client model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the specified element for adding HSL client model.
    @param[in] callback is the message handler for HSL client model.
    @return
    @c true means the model was added successfully.\n
    @c false means adding the model failed.
*/
bool bt_mesh_model_lighting_add_hsl_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_hsl_client_evt_handler callback);

/*!
 * @brief Sends a request to get the Light Lightness Actual state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LIGHTNESS_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light Lightness Actual state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LIGHTNESS_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get the Light Lightness Linear state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LINEAR_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_linear_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light Lightness Linear state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LINEAR_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_linear_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint8_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get the Light Lightness Last state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_LAST_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_last_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to get the Light Lightness Default state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_DEFAULT_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_default_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light Lightness Default state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] lightness
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_DEFAULT_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_default_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t lightness, bool reliable);

/*!
 * @brief Sends a request to get the Light Lightness Range state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_RANGE_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_range_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light Lightness Range state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] range_min
 * @param[in] range_max
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_LIGHT_LIGHTNESS_CLIENT_EVT_RANGE_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_range_lightness(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t range_min, uint16_t range_max, bool reliable);

/*!
 * @brief Sends a request to get the Light CTL state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_ctl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light CTL Lightness state, Light CTL Temperature state, and the Light CTL Delta UV state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_ctl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint16_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get the Light CTL Temperature state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_ctl_temperature(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light CTL Temperature state and the Light CTL Delta UV state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_ctl_temperature(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint16_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get the Light CTL Temperature Range state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_ctl_temperature_range(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light CTL Temperature Range state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] range_min
 * @param[in] range_max
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_ctl_temperature_range(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t range_min, uint16_t range_max, bool reliable);

/*!
 * @brief Sends a request to get the Light CTL Temperature Default and Light CTL Delta UV Default states of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_default_ctl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light CTL Temperature Default state and the Light CTL Delta UV Default state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] lightness
 * @param[in] temperature
 * @param[in] delta_uv
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_default_ctl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t lightness, uint16_t temperature, int16_t delta_uv, bool reliable);

/*!
 * @brief Sends a request to get the Light HSL Lightness, Light HSL Hue, and Light HSL Saturation states of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light HSL Lightness state, Light HSL Hue state, and the Light HSL Saturation state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint16_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get the Light Lightness Default, the Light HSL Hue Default, and Light HSL Saturation Default states of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_default_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light Lightness Default, the Light HSL Hue Default, and Light HSL Saturation Default states of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] lightness
 * @param[in] hue
 * @param[in] saturation
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_default_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t lightness, uint16_t hue, uint16_t saturation, bool reliable);

/*!
 * @brief Sends a request to get the Light HSL Hue Range and Light HSL Saturation Range states of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_range_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light HSL Hue Range and Light HSL Saturation Range states of an element..
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] hue_min
 * @param[in] hue_max
 * @param[in] saturation_min
 * @param[in] saturation_max
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_range_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t hue_min, uint16_t hue_max, uint16_t saturation_min, uint16_t saturation_max, bool reliable);

/*!
 * @brief Sends a request to get the target Light HSL Lightness, Light HSL Hue, and Light HSL Saturation states of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_target_hsl(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to get the Light HSL Hue state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_hue(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light HSL Hue state of an element..
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_hue(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint16_t buffer_length, bool reliable);

/*!
 * @brief Sends a request to get the Light HSL Saturation state of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_get_saturation(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set the Light HSL Saturation state of an element..
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] buffer is the message payload to be sent.
 * @param[in] buffer_length is the message payload length.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is @c NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_lighting_client_set_saturation(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t *buffer, uint16_t buffer_length, bool reliable);

/*!
@}
*/

#endif // __BT_MESH_MODEL_LIGHTING_CLIENT_H__
