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

#ifndef __BT_MESH_DEBUG_H__
#define __BT_MESH_DEBUG_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if (!defined(MESH_NO_DEBUG) && !defined(MESH_DEBUG))
#define MESH_DEBUG
#endif


/*!

     @defgroup PAGE_MESH_DEBUG bt_mesh_debug.h
     @{
        @page PAGE_MESH_DEBUG bt_mesh_debug.h
        bt_mesh_debug.h defines the debug related APIs.
*/

/*!
     @name Debug log group bit filter
     @{
 */
typedef enum {
    BT_MESH_DEBUG_NONE                 = 0x0000,  /**< no debug log */
    BT_MESH_DEBUG_MUST                 = 0x0001,  /**< always show */
    BT_MESH_DEBUG_BEARER               = 0x0002,  /**< debug log for mesh bearer layer module */
    BT_MESH_DEBUG_BEARER_GATT          = 0x0004,  /**< debug log for mesh bearer GATT module */
    BT_MESH_DEBUG_NETWORK              = 0x0008,  /**< debug log for mesh network layer module */
    BT_MESH_DEBUG_TRANSPORT            = 0x0010,  /**< debug log for mesh transport layer module */
    BT_MESH_DEBUG_ACCESS               = 0x0040,  /**< debug log for mesh access layer rx module */
    BT_MESH_DEBUG_MODEL                = 0x0080,  /**< debug log for mesh foundation model module */
    BT_MESH_DEBUG_PROVISION            = 0x0100,  /**< debug log for mesh provision module */
    BT_MESH_DEBUG_BEACON               = 0x0200,  /**< debug log for mesh beacon module */
    BT_MESH_DEBUG_PROXY                = 0x0400,  /**< debug log for mesh proxy module */
    BT_MESH_DEBUG_CONFIG               = 0x0800,  /**< debug log for mesh configuration module */
    BT_MESH_DEBUG_MIDDLEWARE           = 0x1000,  /**< debug log for mesh middleware */
    BT_MESH_DEBUG_APPLICATION          = 0x0020,  /**< debug log for application */
    BT_MESH_DEBUG_FRIEND               = 0x2000,  /**< debug log for friend*/
    BT_MESH_DEBUG_UTILS                 = 0x4000,  /**< debug log for friend*/
    BT_MESH_DEBUG_ALL                  = 0x3FFF
} bt_mesh_debug_group_t;

/*!  @} */

/*!
     @name Debug log level
     @{
 */
typedef enum {
    BT_MESH_DEBUG_LOG_LEVEL_ERROR,     /**< error level*/
    BT_MESH_DEBUG_LOG_LEVEL_WARNING,   /**< warning level*/
    BT_MESH_DEBUG_LOG_LEVEL_INFO,      /**< information level*/
    BT_MESH_DEBUG_LOG_LEVEL_DEBUG,   /**< debug level*/
} bt_mesh_debug_log_level_t;
/*!  @} */


#ifdef MESH_DEBUG
void bt_mesh_debug_log(bt_mesh_debug_log_level_t level, bt_mesh_debug_group_t group,const char *format, ...);
#define MESH_LOGI(module_name, format, ...)   \
    bt_mesh_debug_log(BT_MESH_DEBUG_LOG_LEVEL_INFO, module_name, "[**]"format, ## __VA_ARGS__)

#define MESH_LOGD(module_name, format, ...)   \
    bt_mesh_debug_log(BT_MESH_DEBUG_LOG_LEVEL_DEBUG, module_name, "[**]"format, ## __VA_ARGS__)

#define MESH_LOGW(module_name, format, ...)   \
    bt_mesh_debug_log(BT_MESH_DEBUG_LOG_LEVEL_WARNING, module_name, "[**]"format, ## __VA_ARGS__)

#define MESH_LOGE(module_name, format, ...)   \
    bt_mesh_debug_log(BT_MESH_DEBUG_LOG_LEVEL_ERROR, module_name, "[**]"format, ## __VA_ARGS__)

#define MESH_DUMP_HEX_LOGI(module_name, format, data, len, ...)   \
    bt_mesh_dump_hex_log(BT_MESH_DEBUG_LOG_LEVEL_INFO, module_name, data, len, "[**]"format, ## __VA_ARGS__)

#else
#define MESH_LOGI(...)
#define MESH_LOGD(...)
#define MESH_LOGW(...)
#define MESH_LOGE(...)
#define MESH_DUMP_HEX_LOGI(...)
#endif

/*!
@}
*/

#endif // __BT_MESH_DEBUG_H__

