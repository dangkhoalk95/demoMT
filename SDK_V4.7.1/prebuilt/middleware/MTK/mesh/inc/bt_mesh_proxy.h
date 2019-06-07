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

#ifndef __BT_MESH_PROXY_H__
#define __BT_MESH_PROXY_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshFeatures Features 
 *   @{
*/

/**
 * @defgroup Bluetooth_mesh_features_enum Enum 
 * @{
 */

/**
 * @brief The values for the FilterType.
 */
typedef enum {
    BT_MESH_PROXY_FILTER_TYPE_WHITE = 0,   /**< White list filter. */
    BT_MESH_PROXY_FILTER_TYPE_BLACK = 1,   /**< Black list filter. */
} bt_mesh_proxy_filter_type_t;

/**
 * @}
 */

/**
 * @brief Send a message to change the proxy filter type and clear the proxy filter list.
 * @param[in] type is the filter type. This parameter can only be the value of type #bt_mesh_proxy_filter_type_t.
 * @param[in] netkeyidx specifies the target network.
 * @return
 * #BT_MESH_SUCCESS, means setting the proxy filter type and clearing the proxy filter list was successful. \n
 * #BT_MESH_ERROR_FAIL, means setting the proxy filter type and clearing the proxy filter list failed.\n
 * @note This API is used by a Proxy Client.
 */
bt_mesh_status_t bt_mesh_proxy_set_filter_type(bt_mesh_proxy_filter_type_t type, uint16_t netkeyidx);

/**
 * @brief Sending a message to add the destination addresses to the proxy filter list.
 * @param[in] address_list is a list of addresses where N is the number of addresses in this message.
 * @param[in] list_size is the number of addresses.
 * @param[in] netkeyidx specifies the target network.
 * @return
 * #BT_MESH_SUCCESS, means the proxy addresses were successfully added. \n
 * #BT_MESH_ERROR_FAIL, means adding the proxy addresses failed.\n
 * @note This API is used by a Proxy Client.
 */
bt_mesh_status_t bt_mesh_proxy_add_addresses(uint16_t *address_list, uint16_t list_size, uint16_t netkeyidx);

/**
 * @brief Sending a message to remove the destination addresses from the proxy filter list.
 * @param[in] address_list is a list of addresses where N is the number of addresses in this message.
 * @param[in] list_size is the number of addresses.
 * @param[in] netkeyidx specifies the target network.
 * @return
 * #BT_MESH_SUCCESS, means the proxy addresses were successfully removed. \n
 * #BT_MESH_ERROR_FAIL, means removing the proxy addresses failed.\n
 * @note This API is used by a Proxy Client.
 */
bt_mesh_status_t bt_mesh_proxy_remove_addresses(uint16_t *address_list, uint16_t list_size, uint16_t netkeyidx);

/**
 * @brief Setting the time interval of the network ID and node identity. The time interval is in milliseconds.
 * @param network_id_tout interval of network ID.
 * @param node_identity_tout interval of the node identity.
 * @return
 * #BT_MESH_SUCCESS, means setting the proxy time interval of the network ID and node identity was successful. \n
 * #BT_MESH_ERROR_FAIL, means setting the proxy time interval of the network ID and node identity failed.\n
*/
bt_mesh_status_t bt_mesh_proxy_set_timeout(uint16_t network_id_tout, uint16_t node_identity_tout);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_PROXY_H__

