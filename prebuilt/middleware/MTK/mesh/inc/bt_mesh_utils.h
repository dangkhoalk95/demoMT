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

#ifndef __BT_MESH_UTILS_H__
#define __BT_MESH_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bt_mesh_common.h"

/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMesCommon Common
 *   @{
 *
 *      bt_mesh_utils.h defines the mesh utility APIs.
*/


/**
 * @defgroup Bluetooth_mesh_common_define Define 
 * @{
 */

#define BT_MESH_ADDR_GROUP_PROXIES_VALUE    0xFFFC  /**< All-proxies group address. */
#define BT_MESH_ADDR_GROUP_FRIENDS_VALUE    0xFFFD  /**< All-friends group address. */
#define BT_MESH_ADDR_GROUP_RELAYS_VALUE     0xFFFE  /**< All-relays group address. */
#define BT_MESH_ADDR_GROUP_NODES_VALUE      0xFFFF  /**< All-nodes group address. */

#define BT_MESH_ADDR_UNASSIGNED_VALUE       0x0000  /**< Unassigned address. */

/**
 * @}
 */

/*!
    @brief This function gets the virtual address from UUID.
    @param[in] uuid is a 16-byte UUID buffer.
    @return the virtual address of the input UUID.
*/
uint16_t bt_mesh_utils_get_virtual_address(const uint8_t *uuid) ;

/*!
    @brief Determines whether an address belongs to this device.
    @param[in] addr is the target address.
    @return
    true, means the address belongs to this device. \n
    false, means the address does not belong to this device. \n
*/
bool bt_mesh_utils_is_local_address(uint16_t addr);

/*!
    @brief Getting the address type of an address.
    @param[in] addr is the target address.
    @return The type of input address. The value is defined in #bt_mesh_address_type_t.
*/
bt_mesh_address_type_t bt_mesh_utils_get_addr_type(uint16_t addr);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_UTILS_H__

