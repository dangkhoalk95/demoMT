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

#ifndef __BT_MESH_BEARER_GATT_H__
#define __BT_MESH_BEARER_GATT_H__

#include "bt_gap_le.h"
#include "bt_mesh_common.h"



/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshBearer Bearer
 *   @{
 *
*/

/**
 * @defgroup Bluetooth_mesh_bearer_enum Enum 
 * @{
 */

/** @brief Mesh GATT services type. */
typedef enum {
    BT_MESH_GATT_SERVICE_PROXY,     /**< Mesh proxy service. */
    BT_MESH_GATT_SERVICE_PROVISION, /**< Mesh provisioning service. */
} bt_mesh_gatt_service_t;

/**
 * @}
 */

/**
 * @brief Creating a Mesh GATT connection and discovery service.
 * @param[in] target_addr is the target address for this connection.
 * @param[in] service_type is the service type for this connection.
 * @note When the link is established and the discovery process is complete, an event #BT_MESH_EVT_BEARER_GATT_STATUS is received.
 * @return
 * #BT_MESH_SUCCESS, the connection request is successfully sent. \n
 * #BT_MESH_ERROR_FAIL, means the connection request failed.\n
 */
bt_mesh_status_t bt_mesh_bearer_gatt_connect(bt_addr_t *target_addr, bt_mesh_gatt_service_t service_type);

/**
 * @brief Disconnecting the Mesh GATT connection.
 * @note When the link is disconncted, an event #BT_MESH_EVT_BEARER_GATT_STATUS is received.
 * @return
 * #BT_MESH_SUCCESS, the disconnection request is successfully sent. \n
 * #BT_MESH_ERROR_FAIL, means the disconnection request failed.\n
 */
bt_mesh_status_t bt_mesh_bearer_gatt_disconnect(void);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_BEARER_GATT_H__

