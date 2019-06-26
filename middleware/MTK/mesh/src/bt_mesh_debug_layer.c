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
 
#include "syslog.h"
#include "bt_mesh_debug.h"

#ifdef MESH_DEBUG
log_create_module(mesh_must, PRINT_LEVEL_INFO);
log_create_module(mesh_bearer, PRINT_LEVEL_INFO);
log_create_module(mesh_bearer_gatt, PRINT_LEVEL_INFO);
log_create_module(mesh_network, PRINT_LEVEL_INFO);
log_create_module(mesh_transport, PRINT_LEVEL_INFO);
log_create_module(mesh_access, PRINT_LEVEL_INFO);
log_create_module(mesh_model, PRINT_LEVEL_INFO);
log_create_module(mesh_provision, PRINT_LEVEL_INFO);
log_create_module(mesh_beacon, PRINT_LEVEL_INFO);
log_create_module(mesh_proxy, PRINT_LEVEL_INFO);
log_create_module(mesh_config, PRINT_LEVEL_INFO);
log_create_module(mesh_utils, PRINT_LEVEL_INFO);
log_create_module(mesh_middleware, PRINT_LEVEL_INFO);
log_create_module(mesh_friend, PRINT_LEVEL_INFO);

#if 0
#define BT_MESH_DEBUG_MUST          "mesh_must"  /**<  must debug log for mesh other modules */
#define BT_MESH_DEBUG_BEARER        "mesh_bearer"  /**< debug log for mesh bearer layer module */
#define BT_MESH_DEBUG_BEARER        "mesh_bearer_gatt"  /**< debug log for mesh gatt bearer layer module */
#define BT_MESH_DEBUG_NETWORK       "mesh_network"  /**< debug log for mesh network layer module */
#define BT_MESH_DEBUG_TRANSPORT     "mesh_transport"  /**< debug log for mesh transport layer module */
#define BT_MESH_DEBUG_ACCESS        "mesh_access"  /**< debug log for mesh access layer module */
#define BT_MESH_DEBUG_MODEL         "mesh_model"  /**< debug log for mesh foundation model module */
#define BT_MESH_DEBUG_PROVISION     "mesh_provision"  /**< debug log for mesh provision module */
#define BT_MESH_DEBUG_BEACON        "mesh_beacon"  /**< debug log for mesh beacon module */
#define BT_MESH_DEBUG_PROXY         "mesh_proxy"  /**< debug log for mesh proxy module */
#define BT_MESH_DEBUG_CONFIG        "mesh_config"  /**<  debug log for mesh configuration module */
#define BT_MESH_DEBUG_UTILS         "mesh_utils"  /**<  debug log for mesh other modules */
#define BT_MESH_DEBUG_MIDDLEWARE    "mesh_mid"  /**<  debug log for mesh middleware modules */
#define BT_MESH_DEBUG_FRIEND        "mesh_friend"  /**<  debug log for mesh friend modules */
    
#endif

//#define MESH_DEBUG_NO_BTIF
#define MESH_DEBUG_BUFF_SIZE  150
static char mesh_debug_buff[MESH_DEBUG_BUFF_SIZE];

void bt_mesh_debug_log(bt_mesh_debug_log_level_t level, bt_mesh_debug_group_t group, const char *format, ...)
{
    va_list arg;

    if ( 0
#ifdef MESH_DEBUG_NO_MESH_MUST
		|| strstr(format, "[mesh_must]")
#endif
#ifdef MESH_DEBUG_NO_MESH_NETWORK
        || strstr(format, "[mesh_network]")
#endif
#ifdef MESH_DEBUG_NO_MESH_TRANSPORT
        || strstr(format, "[mesh_transport]")
#endif
#ifdef MESH_DEBUG_NO_MESH_ACCESS
        || strstr(format, "[mesh_access]")
#endif
#ifdef MESH_DEBUG_NO_MESH_MODEL
        || strstr(format, "[mesh_model]")
#endif
#ifdef MESH_DEBUG_NO_MESH_CONFIG
        || strstr(format, "[mesh_config]")
#endif
#ifdef MESH_DEBUG_NO_MESH_BEARER
        || strstr(format, "[mesh_bearer]")
#endif
#ifdef MESH_DEBUG_NO_MESH_PROVISION
        || strstr(format, "[mesh_provision]")
#endif
#ifdef MESH_DEBUG_NO_MESH_BEACON
        || strstr(format, "[mesh_beacon]")
#endif
#ifdef MESH_DEBUG_NO_MESH_PROXY
        || strstr(format, "[mesh_proxy]")
#endif
#ifdef MESH_DEBUG_NO_MESH_UTILS
		|| strstr(format, "[mesh_utils]")
#endif
        ) {
		//printf("[bt_mesh_debug_log] error 1: %s\n", format);
        return;
    }

    va_start(arg, format);
    vsnprintf(mesh_debug_buff, 150, format, arg);
    va_end(arg);
    
    if (level == BT_MESH_DEBUG_LOG_LEVEL_INFO) {
        switch(group) {
            case BT_MESH_DEBUG_MUST:
                LOG_I(mesh_must, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEARER:
                LOG_I(mesh_bearer, "%s", mesh_debug_buff);
                break;        
            case BT_MESH_DEBUG_BEARER_GATT:
                LOG_I(mesh_bearer_gatt, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_NETWORK:
                LOG_I(mesh_network, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_TRANSPORT:
                LOG_I(mesh_transport, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_ACCESS:
                LOG_I(mesh_access, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MODEL:
                LOG_I(mesh_model, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROVISION:
                LOG_I(mesh_provision, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEACON:
                LOG_I(mesh_beacon, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROXY:
                LOG_I(mesh_proxy, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_CONFIG:
                LOG_I(mesh_config, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MIDDLEWARE:
                LOG_I(mesh_middleware, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_FRIEND:
                LOG_I(mesh_friend, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_UTILS:
                LOG_I(mesh_utils, "%s", mesh_debug_buff);
                break;
            default:
                break;
        }
    } else if (level == BT_MESH_DEBUG_LOG_LEVEL_DEBUG) {
        switch(group) {
            case BT_MESH_DEBUG_MUST:
                LOG_I(mesh_must, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEARER:
                LOG_I(mesh_bearer, "%s", mesh_debug_buff);
                break;        
            case BT_MESH_DEBUG_BEARER_GATT:
                LOG_I(mesh_bearer_gatt, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_NETWORK:
                LOG_I(mesh_network, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_TRANSPORT:
                LOG_I(mesh_transport, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_ACCESS:
                LOG_I(mesh_access, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MODEL:
                LOG_I(mesh_model, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROVISION:
                LOG_I(mesh_provision, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEACON:
                LOG_I(mesh_beacon, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROXY:
                LOG_I(mesh_proxy, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_CONFIG:
                LOG_I(mesh_config, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MIDDLEWARE:
                LOG_I(mesh_middleware, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_FRIEND:
                LOG_I(mesh_friend, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_UTILS:
                LOG_I(mesh_utils, "%s", mesh_debug_buff);
                break;
            default:
                break;
        }
    } else if (level == BT_MESH_DEBUG_LOG_LEVEL_WARNING) {
        switch(group) {
            case BT_MESH_DEBUG_MUST:
                LOG_W(mesh_must, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEARER:
                LOG_W(mesh_bearer, "%s", mesh_debug_buff);
                break;        
            case BT_MESH_DEBUG_BEARER_GATT:
                LOG_W(mesh_bearer_gatt, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_NETWORK:
                LOG_W(mesh_network, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_TRANSPORT:
                LOG_W(mesh_transport, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_ACCESS:
                LOG_W(mesh_access, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MODEL:
                LOG_W(mesh_model, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROVISION:
                LOG_W(mesh_provision, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEACON:
                LOG_W(mesh_beacon, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROXY:
                LOG_W(mesh_proxy, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_CONFIG:
                LOG_W(mesh_config, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MIDDLEWARE:
                LOG_W(mesh_middleware, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_FRIEND:
                LOG_W(mesh_friend, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_UTILS:
                LOG_W(mesh_utils, "%s", mesh_debug_buff);
                break;
            default:
                break;
        }
    } else if (level == BT_MESH_DEBUG_LOG_LEVEL_ERROR) {
        switch(group) {
            case BT_MESH_DEBUG_MUST:
                LOG_E(mesh_must, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEARER:
                LOG_E(mesh_bearer, "%s", mesh_debug_buff);
                break;        
            case BT_MESH_DEBUG_BEARER_GATT:
                LOG_E(mesh_bearer_gatt, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_NETWORK:
                LOG_E(mesh_network, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_TRANSPORT:
                LOG_E(mesh_transport, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_ACCESS:
                LOG_E(mesh_access, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MODEL:
                LOG_E(mesh_model, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROVISION:
                LOG_E(mesh_provision, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_BEACON:
                LOG_E(mesh_beacon, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_PROXY:
                LOG_E(mesh_proxy, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_CONFIG:
                LOG_E(mesh_config, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_MIDDLEWARE:
                LOG_E(mesh_middleware, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_FRIEND:
                LOG_E(mesh_friend, "%s", mesh_debug_buff);
                break;
            case BT_MESH_DEBUG_UTILS:
                LOG_E(mesh_utils, "%s", mesh_debug_buff);
                break;
            default:
                break;
        }
    }
	return;
}

void bt_mesh_dump_hex_log(bt_mesh_debug_log_level_t level, bt_mesh_debug_group_t group, void* data, uint16_t len, const char *format,  ...)
{

    va_list arg;

    if ( 0
#ifdef MESH_DEBUG_NO_MESH_MUST
		|| strstr(format, "[mesh_must]")
#endif
#ifdef MESH_DEBUG_NO_MESH_NETWORK
        || strstr(format, "[mesh_network]")
#endif
#ifdef MESH_DEBUG_NO_MESH_TRANSPORT
        || strstr(format, "[mesh_transport]")
#endif
#ifdef MESH_DEBUG_NO_MESH_ACCESS
        || strstr(format, "[mesh_access]")
#endif
#ifdef MESH_DEBUG_NO_MESH_MODEL
        || strstr(format, "[mesh_model]")
#endif
#ifdef MESH_DEBUG_NO_MESH_CONFIG
        || strstr(format, "[mesh_config]")
#endif
#ifdef MESH_DEBUG_NO_MESH_BEARER
        || strstr(format, "[mesh_bearer]")
#endif
#ifdef MESH_DEBUG_NO_MESH_PROVISION
        || strstr(format, "[mesh_provision]")
#endif
#ifdef MESH_DEBUG_NO_MESH_BEACON
        || strstr(format, "[mesh_beacon]")
#endif
#ifdef MESH_DEBUG_NO_MESH_PROXY
        || strstr(format, "[mesh_proxy]")
#endif
#ifdef MESH_DEBUG_NO_MESH_UTILS
		|| strstr(format, "[mesh_utils]")
#endif
        ) {
		//printf("[bt_mesh_debug_log] error 1: %s\n", format);
        return;
    }
	
    va_start(arg, format);
    vsnprintf(mesh_debug_buff, 150, format, arg);
    va_end(arg);

    if (1/*level == BT_MESH_DEBUG_LOG_LEVEL_INFO*/) {
        switch(group) {
            case BT_MESH_DEBUG_MUST:
                LOG_HEXDUMP_I(mesh_must, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_BEARER:
                LOG_HEXDUMP_I(mesh_bearer, mesh_debug_buff, data, len);
                break;        
            case BT_MESH_DEBUG_BEARER_GATT:                
                LOG_HEXDUMP_I(mesh_bearer_gatt, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_NETWORK:
                LOG_HEXDUMP_I(mesh_network, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_TRANSPORT:
                LOG_HEXDUMP_I(mesh_transport, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_ACCESS:
                LOG_HEXDUMP_I(mesh_access, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_MODEL:
                LOG_HEXDUMP_I(mesh_model, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_PROVISION:
                LOG_HEXDUMP_I(mesh_provision, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_BEACON:
                LOG_HEXDUMP_I(mesh_beacon, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_PROXY:
                LOG_HEXDUMP_I(mesh_proxy, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_CONFIG:
                LOG_HEXDUMP_I(mesh_config, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_MIDDLEWARE:                
                LOG_HEXDUMP_I(mesh_middleware, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_FRIEND:                
                LOG_HEXDUMP_I(mesh_friend, mesh_debug_buff, data, len);
                break;
            case BT_MESH_DEBUG_UTILS:
                LOG_HEXDUMP_I(mesh_utils, mesh_debug_buff, data, len);
                break;
            default:
                break;
        }
    } else {
		printf("[bt_mesh_debug_log] error 2: %s\n", mesh_debug_buff);
    }
	
	return;
}

#endif /* MESH_DEBUG */

