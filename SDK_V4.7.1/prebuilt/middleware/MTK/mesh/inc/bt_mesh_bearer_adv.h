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

#ifndef __BT_MESH_BEARER_ADV_H__
#define __BT_MESH_BEARER_ADV_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bt_mesh_provision.h"



/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshBearer Bearer
 *   @{
 *   This section introduces the advertising Bearer layer and GATT Bearer layer APIs. It shows on how to use the enums and structures and the API prototypes 
 *   to set the advertising parameters and scan parameters for the advertising Bearer layer, and how to connect and disconnect with the GATT Bearer layer.
 *
 */

/**
 * @defgroup Bluetooth_mesh_bearer_struct Struct 
 * @{
 */


/** @brief This struct defines the scanner parameters. */
typedef struct {
    uint32_t scan_period;           /**< Scan period in milliseconds. */
    uint16_t scan_interval;         /**< Scan interval in 625 us units. */
    uint16_t scan_window;           /**< Scan window in 625 us units. */
} bt_mesh_bearer_scan_params_t;

/** @brief This struct defines the advertising parameters. */
typedef struct {
    uint32_t adv_period;            /**< Advertising period in milliseconds. */
    uint16_t min_interval;          /**< Advertising minimum interval in 625 us units. */
    uint16_t max_interval;          /**< Advertising maximum interval in 625 us units. */
    uint8_t resend;                 /**< Resend number of times; 0xFF means to continue resending until the user stops the process. */
} bt_mesh_bearer_adv_params_t;

/**
 * @}
 */

/**
 * @brief Setting up Advertising bearer parameters.
 * @param[in] adv_params is the advertising parameters as defined in #bt_mesh_bearer_adv_params_t.
 * @param[in] scan_params is the scanner parameters as defined in #bt_mesh_bearer_scan_params_t.
 * @return
 * #BT_MESH_SUCCESS, means the advertising parameters were successfully set. \n
 * #BT_MESH_ERROR_FAIL, means setting the advertising parameters failed.\n
 */
bt_mesh_status_t bt_mesh_bearer_adv_set_params(const bt_mesh_bearer_adv_params_t *adv_params, const bt_mesh_bearer_scan_params_t *scan_params);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_BEARER_ADV_H__

