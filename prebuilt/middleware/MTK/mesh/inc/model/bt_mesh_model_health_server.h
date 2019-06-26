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

#ifndef __BT_MESH_MODEL_HEALTH_SERVER_H__
#define __BT_MESH_MODEL_HEALTH_SERVER_H__

#include "bt_mesh_access.h"

/*!

     @defgroup PAGE_MESH_MODEL_HEALTH_SERVER bt_mesh_model_health_server.h
     @{
        @page PAGE_MESH_MODEL_HEALTH_SERVER bt_mesh_model_health_server.h
        bt_mesh_model_health_server.h defines the Health Server Model APIs.
*/

/** @brief  This defines the health server message handler prototype.
 *  @param[in] model_handle is the model handle which needs to handle these messages.
 *  @param[in] msg is the received message.
 *  @param[in] arg is user data.
 *  @return NONE
 */
typedef void (*bt_mesh_health_server_evt_handler) (uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg);

/*!
    @brief Add a health server model.
    @param[out] model_handle is the handle of this added model.
    @param[in] element_index is the specified element for adding health server model.
    @param[in] callback is the message handler for health server model.
    @return
    @c true means the health server model added successfully. \n
    @c false means adding the health server model failed.
    @note The health server model should be added in every element.
*/
bool bt_mesh_model_add_health_server(
    uint16_t *model_handle, uint16_t element_index, bt_mesh_health_server_evt_handler callback);

/*!
    @brief Sends a current health status.
    @param[in] model_handle is the model handle which the message belongs to.
    @param[in] msg is the received message which this API replies to.
    @param[in] test_id is the identifier of a specific test to be performed.
    @param[in] company_id is the Bluetooth assigned Company Identifier.
    @param[in] fault_array is the fault array.
    @param[in] fault_array_length is the fault array length.
    @return
    #BT_MESH_SUCCESS, the request was sent successfully.\n
    #BT_MESH_ERROR_OOM, not enough memory for sending request. \n
    #BT_MESH_ERROR_INVALID_ADDR, the address to add is invalid. \n
*/
bt_mesh_status_t bt_mesh_model_health_server_fault_status(
    uint16_t model_handle, const bt_mesh_access_message_rx_t *msg,
    uint8_t test_id, uint16_t company_id, uint8_t *fault_array, uint8_t fault_array_length);


/*!
@}
*/

#endif // __BT_MESH_MODEL_HEALTH_SERVER_H__

