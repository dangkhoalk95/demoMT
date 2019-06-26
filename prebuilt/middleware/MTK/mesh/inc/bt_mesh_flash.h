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

#ifndef __BT_MESH_FLASH_H__
#define __BT_MESH_FLASH_H__

#include <stdlib.h>
#include <stdint.h>



/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshStorage Storage
 *   @{
 *   This section introduces the mesh storage APIs and shows how to use enums and API prototypes to reset the specification storage.
 *   
*/

/**
 * @defgroup Bluetooth_mesh_storage_enum Enum 
 * @{
 */

/** @brief Mesh flash sector.
 */
typedef enum {
    BT_MESH_FLASH_SECTOR_DATA = 0,          /**< Sector to store all mesh non-versatile data except for the sequence number. */
    BT_MESH_FLASH_SECTOR_SEQUENCE_NUMBER,   /**< Sector to store only the sequence number. */
    BT_MESH_FLASH_SECTOR_FOUNDATION_MODEL,  /**< Sector to store the foundation model. */
    BT_MESH_FLASH_SECTOR_PUB_SUB,           /**< Sector to store publish and subscription. */
    BT_MESH_FLASH_SECTOR_ALL,               /**< All flash sector of mesh. */
} bt_mesh_flash_sector_t;
/**
 * @}
 */

/**
 * @brief 	This function is used to reset the mesh data stored in flash.
   @param[in] sector indicates which flash sector of the mesh is to be reset.
 * @return
 * true, means the mesh was successfully reset.\n
 * false, means the mesh reset failed.
 * @note Please use #bt_mesh_init instead of using this API individually.
 */
bool bt_mesh_flash_reset(bt_mesh_flash_sector_t sector);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_FLASH_H__

