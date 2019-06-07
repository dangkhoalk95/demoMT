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

#ifndef __BT_MESH_MODEL_HEALTH_CLIENT_H__
#define __BT_MESH_MODEL_HEALTH_CLIENT_H__

#include "bt_mesh_access.h"

/*!

     @defgroup PAGE_MESH_MODEL_HEALTH_CLIENT bt_mesh_model_health_client.h
     @{
        @page PAGE_MESH_MODEL_HEALTH_CLIENT bt_mesh_model_health_client.h
        bt_mesh_model_health_client.h defines the Health Client Model APIs.
*/

/** @brief Event parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_CURRENT_STATUS. */
typedef struct {
    uint8_t test_id;            /**< Identifier of a most recently performed test. */
    uint16_t company_id;        /**< 16-bit Bluetooth assigned Company Identifier. */
    uint8_t *fault_array;       /**< An array contains a sequence of 1-octet fault values. */
    uint8_t fault_array_length; /**< Length of the fault array. */
} bt_mesh_health_client_evt_current_status_t;

/** @brief Event parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_FAULT_STATUS. */
typedef struct {
    uint8_t test_id;            /**< Identifier of a most recently performed test. */
    uint16_t company_id;        /**< 16-bit Bluetooth assigned Company Identifier. */
    uint8_t *fault_array;       /**< An array contains a sequence of 1-octet fault values. */
    uint8_t fault_array_length; /**< Length of the fault array. */
} bt_mesh_health_client_evt_fault_status_t;

/** @brief Event parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_PERIOD_STATUS. */
typedef struct {
    uint8_t fast_period_divisor;    /**< Divider for the Publish Period. Modified Publish Period is used for sending Current Health Status messages when there are active faults to communicate.*/
} bt_mesh_health_client_evt_period_status_t;

/** @brief Event parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_ATTENTION_STATUS. */
typedef struct {
    uint8_t attention; /**< Value of the Attention Timer state, which represents the remaining duration of the attention state of a server in seconds. */
} bt_mesh_health_client_evt_attention_status_t;

/*!
     @brief Health client model event id
     @{
 */
typedef enum {
    BT_MESH_HEALTH_CLIENT_EVT_CURRENT_STATUS,      /**< Event for health current status. */
    BT_MESH_HEALTH_CLIENT_EVT_FAULT_STATUS,        /**< Event for health fault status. */
    BT_MESH_HEALTH_CLIENT_EVT_PERIOD_STATUS,       /**< Event for health period status. */
    BT_MESH_HEALTH_CLIENT_EVT_ATTENTION_STATUS     /**< Event for health attention status. */
} bt_mesh_health_client_event_id_t;
/*!  @} */

/*!
     @brief Health client event structure.
 */
typedef struct {
    bt_mesh_health_client_event_id_t evt_id;        /**<  Health client event ID. */
    union {
        bt_mesh_health_client_evt_current_status_t current_status;      /**<  parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_CURRENT_STATUS */
        bt_mesh_health_client_evt_fault_status_t fault_status;          /**<  parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_FAULT_STATUS */
        bt_mesh_health_client_evt_period_status_t period_status;        /**<  parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_PERIOD_STATUS */
        bt_mesh_health_client_evt_attention_status_t attention_status;  /**<  parameter of health client model event @ref BT_MESH_HEALTH_CLIENT_EVT_ATTENTION_STATUS */
    } data;
} bt_mesh_health_client_evt_t;

/** @brief  This defines the health client event handler prototype.
 *  @param[in] model_handle is the model handle which needs to handle these messages.
 *  @param[in] msg is the original received message.
 *  @param[in] event is the parsed health client event.
 *  @return NONE
 */
typedef void (*bt_mesh_health_client_evt_handler)(uint16_t model_handle, 
    const bt_mesh_access_message_rx_t *msg, const bt_mesh_health_client_evt_t *event);

/*!
 * @brief Add a health client model.
 * @param[out] model_handle is the handle of this added model.
 * @param[in] element_index is the specified element for adding health client model.
 * @param[in] callback is the message handler for health client model
 * @return
 * @c true means the model was added successfully.\n
 * @c false means adding the model failed.
*/
bool bt_mesh_model_add_health_client(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_health_client_evt_handler callback);

/*!
 * @brief Sends a request to clear current Registered Fault.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] company_id is the Bluetooth assigned Company Identifier.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_FAULT_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_clear_fault(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t company_id, bool reliable);

/*!
 * @brief Sends a request to get current Registered Fault.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] company_id is the Bluetooth assigned Company Identifier.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_FAULT_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_get_fault(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint16_t company_id);

/*!
 * @brief Sends a request to invoke self-test procedure of an element.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] test_id is the identifier of a specific test to be performed.
 * @param[in] company_id is the Bluetooth assigned Company Identifier.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_FAULT_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_test_fault(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t test_id, uint16_t company_id, bool reliable);

/*!
 * @brief Sends a request to get current Health Period state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_PERIOD_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_get_period(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set current Health Period state.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] fast_period_divisor is the divider for the publish period.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_PERIOD_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_set_period(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t fast_period_divisor, bool reliable);

/*!
 * @brief Sends a request to get current Attention Timer.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_ATTENTION_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_get_attention(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta);

/*!
 * @brief Sends a request to set current Attention Timer.
 * @param[in] model_handle is the model handle which the message belongs to.
 * @param[in] tx_meta is the metadata of the message.
 * @param[in] attention is the value of the attention timer.
 * @param[in] reliable is to send the request as a reliable message or not.
 * @note Response: #BT_MESH_HEALTH_CLIENT_EVT_ATTENTION_STATUS
 * @return
 * #BT_MESH_SUCCESS, packets successfully sent. \n
 * #BT_MESH_ERROR_OOM, not enough memory for sending the packet. \n
 * #BT_MESH_ERROR_NULL, tx_params or data is NULL. \n
 * #BT_MESH_ERROR_INVALID_ADDR, invalid source or destination address. \n
 * #BT_MESH_ERROR_INVALID_TTL, invalid TTL value. \n
 * #BT_MESH_ERROR_INVALID_KEY, invalid key index or no bound network key. \n
*/
bt_mesh_status_t bt_mesh_model_health_client_set_attention(
    uint16_t model_handle, const bt_mesh_access_message_tx_meta_t *tx_meta,
    uint8_t attention, bool reliable);

/*!
@}
*/

#endif // __BT_MESH_MODEL_HEALTH_CLIENT_H__

