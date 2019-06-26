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

#ifndef __BT_MESH_MODEL_H__
#define __BT_MESH_MODEL_H__

#include <stdlib.h>
#include <stdint.h>

#include "bt_mesh_access.h"
/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMesModel Model
 *   @{
 *   This section introduces the Mesh Model APIs for creating a mesh device. It shows how to: use enums and structures  
 *   and API prototypes for adding elements and client or server models; bind a model to an application key; set and  
 *   get composition data; get the publication state and subscription list.
 *
 * @section bt_mesh_model_api_usage How to create a mesh device
 *
 * - The application calls functions #bt_mesh_model_set_composition_data_header(), #bt_mesh_model_add_element(), bt_mesh_model_add_configuration_server(), bt_mesh_model_add_health_server and #bt_mesh_model_add_model.
 * - It then adds elements and models to create a mesh device. This example shows a device with one element and one model.
 * - Please refer to Mesh Profile Specification v1.0 Section 4.2.1.1 for information about the composition data header.
 * - First, set the composition data header. Then, add the element. Finally, add the model to the added element. All registered messages are sent back to _sig_msg_handler.
 * - The configuration server model and health server model can be added by bt_mesh_model_add_configuration_server and bt_mesh_model_add_health_server. If you want to implement them yourself, use #bt_mesh_model_add_model to add these two server models.
 * - Sample code:
 *      @code
 * const bt_mesh_access_opcode_handler_t generic_onoff_server_handler[] =
 * {
 *      {Generic_OnOff_Set, MESH_MODEL_COMPANY_ID_NONE, _sig_msg_handler},
 *      {Generic_OnOff_Set_Unreliable, MESH_MODEL_COMPANY_ID_NONE, _sig_msg_handler},
 * };
 *
 * void create_mesh_device(void)
 * {
 *      uint8_t composition_data_header[10] =
 *      {
 *          0x94, 0x00, // cid
 *          0x1A, 0x00, // pid
 *          0x01, 0x00, // vid
 *          0x08, 0x00, // crpl
 *          BT_MESH_FEATURE_PROXY, 0x00, // features
 *      };
 *      uint16_t element_index;
 *      uint16_t model_handle;
 *      bt_mesh_model_add_params_t model_params;
 *
 *      memset(&gOnOffServer, 0, sizeof(mesh_onoff_server_model_t));
 *
 *      bt_mesh_model_set_composition_data_header(10, composition_data_header);
 *      bt_mesh_model_add_element(BT_MESH_MODEL_ELEMENT_LOCATION_FRONT, &element_index);
 *
 *      bt_mesh_model_add_configuration_server(&model_handle, config_server_handler);
 *      bt_mesh_model_add_health_server(element_index, &model_handle);
 *
 *      model_params.model_id = BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_SERVER;
 *      model_params.element_index = element_index;
 *      model_params.opcode_handlers = generic_onoff_server_handler;
 *      model_params.opcode_count = sizeof(generic_onoff_server_handler)/sizeof(bt_mesh_access_opcode_handler_t);
 *      bt_mesh_model_add_model(&model_handle, &model_params);
 * }
 *      @endcode
 *
*/

/**
 * @defgroup Bluetooth_mesh_model_define Define 
 * @{
 */

#define MESH_MODEL_INVALID_ELEMENT_INDEX    0xFFFF    /**< Invalid element index. */

#define MESH_MODEL_COMPANY_ID_NONE    0xFFFF    /**< Company ID for SIG Models. */

/**
 * @brief Foundation model ID.
 */
#define BT_MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER    0x0000    /**< The model ID of the configuration server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT    0x0001    /**< The model ID of the configuration client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_HEALTH_SERVER    0x0002    /**< The model ID of the health  server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_HEALTH_CLIENT    0x0003    /**< The model ID of the health client. */

/**
 * @brief Generic model ID.
 */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_SERVER    0x1000    /**< The model ID of the generic onoff server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT    0x1001    /**< The model ID of the generic onoff client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_LEVEL_SERVER    0x1002    /**< The model ID of the generic level server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT    0x1003    /**< The model ID of the generic level clinet. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_DEFAULT_TRANSITION_TIME_SERVER    0x1004    /**< The model ID of the generic default transition time server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT    0x1005    /**< The model ID of the generic default transition time client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_SERVER    0x1006    /**< The model ID of the generic power onoff server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SERVER    0x1007    /**< The model ID of the generic power onoff setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_ONOFF_CLIENT    0x1008    /**< The model ID of the generic power onoff client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_SERVER    0x1009    /**< The model ID of the generic power level server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_SETUP_SERVER    0x100a    /**< The model id of the generic power level setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_POWER_LEVEL_CLIENT    0x100b    /**< The model ID of the generic power level client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_BATTERY_SERVER    0x100c    /**< The model ID of the generic battery server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_BATTERY_CLIENT    0x100d    /**< The model ID of the generic battery client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_LOCATION_SERVER    0x100e    /**< The model ID of the generic location server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_LOCATION_SETUP_SERVER    0x100f    /**< The model ID of the generic location server server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_LOCATION_CLIENT    0x1010    /**< The model ID of the generic location client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_ADMIN_PROPERTY_SERVER    0x1011    /**< The model ID of the generic admin property server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_MANUFACTURER_PROPERTY_SERVER    0x1012    /**< The model ID of the generic manufacturer property server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_USER_PROPERTY_SERVER    0x1013    /**< The model ID of the generic user property server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_CLIENT_PROPERTY_SERVER    0x1014    /**< The model ID of the generic client property server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_GENERIC_PROPERTY_CLIENT    0x1015    /**< The model ID of the generic property client. */

/**
 * @brief Sensors model ID.
 */
#define BT_MESH_MODEL_SIG_MODEL_ID_SENSOR_SERVER    0x1101    /**< The model ID of the sensor server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SENSOR_SETUP_SERVER    0x1102    /**< The model ID of the sensor setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SENSOR_CLIENT    0x1103    /**< The model ID of the sensor client. */

/**
 * @brief Time and Scenes model ID. */

#define BT_MESH_MODEL_SIG_MODEL_ID_TIME_SERVER    0x1200    /**< The model ID of the time server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_TIME_SETUP_SERVER    0x1201    /**< The model ID of the time setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_TIME_CLIENT    0x1202    /**< The model ID of the time client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SCENE_SERVER    0x1203    /**< The model ID of the scene server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SCENE_SETUP_SERVER    0x1204    /**< The model ID of the scene setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SCENE_CLIENT    0x1205    /**< The model ID of the scene client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SCHEDULER_SERVER    0x1206    /**< The model ID of the scheduler server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SCHEDULER_SETUP_SERVER    0x1207    /**< The model ID of the scheduler setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_SCHEDULER_CLIENT    0x1208    /**< The model ID of the scheduler client. */
/**
 * @brief Lighting model ID.
 */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_SERVER    0x1300    /**< The model ID of the light lightness server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SERVER    0x1301    /**< The model ID of the light lightness setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_CLIENT    0x1302    /**< The model ID of the light lightness client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_SERVER    0x1303    /**< The model ID of the light CTL server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_SETUP_SERVER    0x1304    /**< The model ID of the light CTL setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_CLIENT    0x1305    /**< The model ID of the light CTL client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_TEMPERATURE_SERVER    0x1306    /**< The model ID of the light CTL temperature server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SERVER    0x1307    /**< The model ID of the light HSL server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SETUP_SERVER    0x1308    /**< The model ID of the light HSL setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_CLIENT    0x1309    /**< The model ID of the light HSL client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_HUE_SERVER    0x130a    /**< The model ID of the light HSL HUE server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SATURATION_SERVER    0x130b    /**< The model ID of the light XYL server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_XYL_SERVER    0x130c    /**< The model ID of the light XYL setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_XYL_SETUP_SERVER    0x130d    /**< The model ID of the light XYL setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_XYL_CLIENT    0x130e    /**< The model ID of the light XYL client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_SERVER    0x130f    /**< The model ID of the light LC server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_LIGHT_ONOFF_SERVER    0x1310    /**< The model ID of the light LC light onoff server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_SETUP_SERVER    0x1311    /**< The model ID of the light LC setup server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_LIGHT_LC_CLIENT    0x1312    /**< The model ID of the light LC client. */

/**
 * @brief Firmware update model ID.
 */
#define BT_MESH_MODEL_SIG_MODEL_ID_FIRMWARE_UPDATE_SERVER    0xFE00    /**< The model ID of the firmware update server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_FIRMWARE_UPDATE_CLIENT    0xFE01    /**< The model ID of the firmware update client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_FIRMWARE_DISTRIBUTION_SERVER    0xFE02    /**< The model ID of the firmware distribution server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_FIRMWARE_DISTRIBUTION_CLIENT    0xFE03    /**< The model ID of the firmware distribution client. */
#define BT_MESH_MODEL_SIG_MODEL_ID_OBJECT_TRANSFER_SERVER    0xFF00    /**< The model ID of the object transfer server. */
#define BT_MESH_MODEL_SIG_MODEL_ID_OBJECT_TRANSFER_CLIENT    0xFF01    /**< The model ID of the object transfer client. */


/**
 *  @brief Only a few defintions are derived from the GATT Bluetooth Namespace Descriptors section of the Bluetooth SIG Assigned Numbers.
 *  @note Please refer to https://www.bluetooth.com/specifications/assigned-numbers/gatt-namespace-descriptors for more definitions.
 */
#define BT_MESH_MODEL_ELEMENT_LOCATION_FIRST     0x0001    /**< Indicates that the location is first. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_SECOND    0x0002    /**< Indicates that the location is second. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_FRONT     0x0100    /**< Indicates that the location is front. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_BACK      0x0101    /**< Indicates that the location is back. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_TOP       0x0102    /**< Indicates that the location is top. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_BOTTOM    0x0103    /**< Indicates that the location is bottom. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_UPPER     0x0104    /**< Indicates that the location is upper. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_LOWER     0x0105    /**< Indicates that the location is lower. */
#define BT_MESH_MODEL_ELEMENT_LOCATION_MAIN      0x0106    /**< Indicates that the location is main. */

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_model_enum Enum 
 * @{
 */

/** @brief Fault values
 *  @note
 * 0x33-0x7F is reserved for future use. \n
 * 0x80-0xFF is a vendor specific warning/error.
 */
typedef enum {
    BT_MESH_MODEL_HEALTH_FAULT_NO_FAULT = 0,                    /**< No Fault */
    BT_MESH_MODEL_HEALTH_FAULT_BATTERY_LOW_WARNING,             /**< Battery Low Warning */
    BT_MESH_MODEL_HEALTH_FAULT_BATTERY_LOW_ERROR,               /**< Battery Low Error */
    BT_MESH_MODEL_HEALTH_FAULT_SUPPLY_VOLTAGE_TOO_LOW_WARNING,  /**< Supply Voltage Too Low Warning */
    BT_MESH_MODEL_HEALTH_FAULT_SUPPLY_VOLTAGE_TOO_LOW_ERROR,    /**< Supply Voltage Too Low Error */
    BT_MESH_MODEL_HEALTH_FAULT_SUPPLY_VOLTAGE_TOO_HIGH_WARNING, /**< Supply Voltage Too High Warning */
    BT_MESH_MODEL_HEALTH_FAULT_SUPPLY_VOLTAGE_TOO_HIGH_ERROR,   /**< Supply Voltage Too High Error */
    BT_MESH_MODEL_HEALTH_FAULT_POWER_SUPPLY_INTERRUPTED_WARNING,    /**< Power Supply Interrupted Warning */
    BT_MESH_MODEL_HEALTH_FAULT_POWER_SUPPLY_INTERRUPTED_ERROR,  /**< Power Supply Interrupted Error */
    BT_MESH_MODEL_HEALTH_FAULT_NO_LOAD_WARNING,                 /**< No Load Warning */
    BT_MESH_MODEL_HEALTH_FAULT_NO_LOAD_ERROR,                   /**< No Load Error */
    BT_MESH_MODEL_HEALTH_FAULT_OVERLOAD_WARNING,                /**< Overload Warning */
    BT_MESH_MODEL_HEALTH_FAULT_OVERLOAD_ERROR,                  /**< Overload Error */
    BT_MESH_MODEL_HEALTH_FAULT_OVERHEAT_WARNING,                /**< Overheat Warning */
    BT_MESH_MODEL_HEALTH_FAULT_OVERHEAT_ERROR,                  /**< Overheat Error */
    BT_MESH_MODEL_HEALTH_FAULT_CONDENSATION_WARNING,            /**< Condensation Warning */
    BT_MESH_MODEL_HEALTH_FAULT_CONDENSATION_ERROR,              /**< Condensation Error */
    BT_MESH_MODEL_HEALTH_FAULT_VIBRATION_WARNING,               /**< Vibration Warning */
    BT_MESH_MODEL_HEALTH_FAULT_VIBRATION_ERROR,                 /**< Vibration Error */
    BT_MESH_MODEL_HEALTH_FAULT_CONFIGURATION_WARNING,           /**< Configuration Warning */
    BT_MESH_MODEL_HEALTH_FAULT_CONFIGURATION_ERROR,             /**< Configuration Error */
    BT_MESH_MODEL_HEALTH_FAULT_ELEMENT_NOT_CALIBRATED_WARNING,  /**< Element Not Calibrated Warning */
    BT_MESH_MODEL_HEALTH_FAULT_ELEMENT_NOT_CALIBRATED_ERROR,    /**< Element Not Calibrated Error */
    BT_MESH_MODEL_HEALTH_FAULT_MEMORY_WARNING,                  /**< Memory Warning */
    BT_MESH_MODEL_HEALTH_FAULT_MEMORY_ERROR,                    /**< Memory Error */
    BT_MESH_MODEL_HEALTH_FAULT_SELF_TEST_WARNING,               /**< Self-Test Warning */
    BT_MESH_MODEL_HEALTH_FAULT_SELF_TEST_ERROR,                 /**< Self-Test Error */
    BT_MESH_MODEL_HEALTH_FAULT_INPUT_TOO_LOW_WARNING,           /**< Input Too Low Warning */
    BT_MESH_MODEL_HEALTH_FAULT_INPUT_TOO_LOW_ERROR,             /**< Input Too Low Error */
    BT_MESH_MODEL_HEALTH_FAULT_INPUT_TOO_HIGH_WARNING,          /**< Input Too High Warning */
    BT_MESH_MODEL_HEALTH_FAULT_INPUT_TOO_HIGH_ERROR,            /**< Input Too High Error */
    BT_MESH_MODEL_HEALTH_FAULT_INPUT_NO_CHANGE_WARNING,         /**< Input No Change Warning */
    BT_MESH_MODEL_HEALTH_FAULT_INPUT_NO_CHANGE_ERROR,           /**< Input No Change Error */
    BT_MESH_MODEL_HEALTH_FAULT_ACTUATOR_BLOCKED_WARNING,        /**< Actuator Blocked Warning */
    BT_MESH_MODEL_HEALTH_FAULT_ACTUATOR_BLOCKED_ERROR,          /**< Actuator Blocked Error */
    BT_MESH_MODEL_HEALTH_FAULT_HOUSING_OPENED_WARNING,          /**< Housing Opened Warning */
    BT_MESH_MODEL_HEALTH_FAULT_HOUSING_OPENED_ERROR,            /**< Housing Opened Error */
    BT_MESH_MODEL_HEALTH_FAULT_TAMPER_WARNING,                  /**< Tamper Warning */
    BT_MESH_MODEL_HEALTH_FAULT_TAMPER_ERROR,                    /**< Tamper Error */
    BT_MESH_MODEL_HEALTH_FAULT_DEVICED_MOVED_WARNING,           /**< Device Moved Warning */
    BT_MESH_MODEL_HEALTH_FAULT_DEVICED_MOVED_ERROR,             /**< Device Moved Error */
    BT_MESH_MODEL_HEALTH_FAULT_DEVICE_DROPPED_WARNING,          /**< Device Dropped Warning */
    BT_MESH_MODEL_HEALTH_FAULT_DEVICE_DROPPED_ERROR,            /**< Device Dropped Error */
    BT_MESH_MODEL_HEALTH_FAULT_OVERFLOW_WARNING,                /**< Overflow Warning */
    BT_MESH_MODEL_HEALTH_FAULT_OVERFLOW_ERROR,                  /**< Overflow Error */
    BT_MESH_MODEL_HEALTH_FAULT_EMPTY_WARNING,                   /**< Empty Warning */
    BT_MESH_MODEL_HEALTH_FAULT_EMPTY_ERROR,                     /**< Empty Error */
    BT_MESH_MODEL_HEALTH_FAULT_INTERNAL_BUS_WARNING,            /**< Internal Bus Warning */
    BT_MESH_MODEL_HEALTH_FAULT_INTERNAL_BUS_ERROR,              /**< Internal Bus Error */
    BT_MESH_MODEL_HEALTH_FAULT_MECHANISM_JAMMED_WARNING,        /**< Mechanism Jammed Warning */
    BT_MESH_MODEL_HEALTH_FAULT_MECHANISM_JAMMED_ERROR,          /**< Mechanism Jammed Error */
} bt_mesh_model_health_fault_value;

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_model_struct Struct 
 * @{
 */

/** @brief The composition element structure. */
typedef struct {
    uint16_t loc;               /**< The location of this element. */
    uint8_t num_s;              /**< The number of SIG models in this element. */
    uint8_t num_v;              /**< The number of vendor models in this element. */
    uint16_t *sig_models;       /**< SIG model IDs. */
    uint32_t *vendor_models;    /**< Vendor model IDs. */
} bt_mesh_composition_element_t;

/** @brief The composition data structure. */
typedef struct {
    uint16_t cid;                           /**< Contains a 16-bit company identifier assigned by the Bluetooth SIG. */
    uint16_t pid;                           /**< Contains a 16-bit vendor-assigned product identifier. */
    uint16_t vid;                           /**< Contains a 16-bit vendor-assigned product version identifier. */
    uint16_t crpl;                          /**< Contains a 16-bit value representing the minimum number of replay protection list entries in a device. */
    uint16_t features;                      /**< Contains a bit field indicating the device features. */
    void *elements;                         /**< Contains a double-linked list of #bt_mesh_composition_element_t. */
    uint8_t element_length;                 /**< The number of elements. */
} bt_mesh_composition_data_t;

/** @brief The model adding parameter structure. */
typedef struct {
    uint32_t model_id;                      /**< The model ID. It can be a SIG defined value or a vendor defined value. */
    uint16_t element_index;                 /**< The target element index to add model. */
    const bt_mesh_access_opcode_handler_t *opcode_handlers;    /**< The access message handler for this model. */
    uint8_t opcode_count;                   /**< Indicates the number of opcodes that must be managed in this model. */
    /**
     * The timeout callback is called when the publication timer expires. Set to @c NULL for models that
     * do not support periodic publishing.
     */
    bt_mesh_access_publish_timeout_cb_t    publish_timeout_cb;
} bt_mesh_model_add_params_t;

/** @brief The node identity structure. */
typedef struct {
    uint16_t keyIndex;            /**< The netkey index. */
    uint8_t nodeIdentiy;          /**< The node identity value. */
} bt_mesh_node_identity_t;

/** @brief The heartbeat publication structure. */
typedef struct {
    uint16_t dest;        /**< The heartbeat publication destination. */
    uint16_t count;       /**< The heartbeat publication count. */
    uint8_t period_log;   /**< The heartbeat publication period log. */
    uint8_t ttl;          /**< The heartbeat publication TTL. */
    uint16_t features;    /**< The heartbeat publication features. */
    uint16_t netkey_index; /**< The heartbeat publication NetKey index. */
} bt_mesh_heartbeat_publication_t;

/** @brief The heartbeat subscription structure. */
typedef struct {
    uint16_t source;     /**< The source address for HB messages. */
    uint16_t dest;       /**< The destination address for HB messages. */
    uint8_t period_log;   /**< The remaining period log for processing HB messages. */
    uint16_t count;      /**< The number of HB messages received. */
    uint8_t min_hops;     /**< The minimum number of hops when receiving HB messages. */
    uint8_t max_hops;     /**< The maximum number of hops when receiving HB messages. */
} bt_mesh_heartbeat_subscription_t;

/** @brief The lpn poll timeout structure. */
typedef struct {
    uint16_t lpn_address;   /**< The low-power node address. */
    uint32_t poll_timeout;  /**< The poll timeout. */
} bt_mesh_lpn_poll_timeout_t;

/** @brief The model publication structure. */
typedef struct {
    uint16_t addr;                           /**< The publish address. */
    uint16_t appkey_index : 12;              /**< The AppKey index. */
    uint16_t flag : 1;                       /**< The friendship credentials flag. */
    uint16_t rfu : 3;                        /**< Reserved for future use. */
    uint8_t ttl;                             /**< The publish TTL. */
    uint8_t period;                          /**< The publish period. */
    uint8_t retransmit_count : 3;            /**< The publish retransmit count. */
    uint8_t retransmit_interval_steps : 5;   /**< The number of publish retransmit interval steps. */
} __attribute__((packed)) bt_mesh_model_publication_t;

/** @brief The network transmit structure. */
typedef struct {
    uint8_t network_transmit_count : 3;             /**< The network transmit count. */
    uint8_t network_transmit_interval_steps : 5;    /**< The number of network transmit interval steps. */
} __attribute__((packed)) bt_mesh_network_transmit_t;

/** @brief The relay retransmit structure. */
typedef struct {
    uint8_t relay_retransmit_count : 3;             /**< The relay retransmit count. */
    uint8_t relay_retransmit_interval_steps : 5;    /**< The number of relay retransmit interval steps. */
} __attribute__((packed)) bt_mesh_relay_retransmit_t;

/**
 * @}
 */

/*!
    @brief Getting the the count of elements in this node.
    @return the number of elements.
*/
uint8_t bt_mesh_model_get_element_count( void );

/*!
    @brief Set the compostition data header for this device.
    @param[in] buffer is the buffer containing the composition data header.
    @param[in] length is the length of the composition data header.
    @return
    #BT_MESH_SUCCESS, means the composition data header was successfully set. \n
    #BT_MESH_ERROR_FAIL, means setting the composition header failed.\n
*/
bt_mesh_status_t bt_mesh_model_set_composition_data_header(uint8_t *buffer, uint8_t length);

/*!
    @brief Adding an element to this device.
    @param[out] element_index is the element index in this node.
    @param[in] location is the location of this element.
    @return
    @c true, means the element was successfully added.\n
    @c false, means adding the element failed.
*/
bool bt_mesh_model_add_element(uint16_t *element_index, uint16_t location);

/*!
    @brief Adding a model to this device.
    @param[out] model_handle is the handle of this added model.
    @param[in] model_params specifies where this model should be added, and defines the message handler of this model.
    @return
    @c true, means the model was successfully added.\n
    @c false, means adding the model failed.
*/
bool bt_mesh_model_add_model(uint16_t *model_handle, const bt_mesh_model_add_params_t *model_params);

/*!
    @brief Getting the address of the specified element.
    @param[in] element_index is the specified element.
    @return The 16-byte unicast address.
*/
uint16_t bt_mesh_model_get_element_address(uint16_t element_index);

/*!
    @brief Setting the unicast address for the primary element of this node.
    @param[in] unicast_addr the address assigned to this node.
    @return
    #BT_MESH_SUCCESS, means the element address was successfully set. \n
    #BT_MESH_ERROR_FAIL, means setting the element address failed. \n
*/
bt_mesh_status_t bt_mesh_model_set_element_addr(uint16_t unicast_addr);

/*!
    @brief Binding the model to the assigned application key.
    @param[in] model_handle is the handle of the model to be bound with the application key.
    @param[in] appkey_index is the application key index to be bound with the model.
    @return
    #BT_MESH_SUCCESS, requesting the message was successfully performed. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_MODEL, there is not enough memory for sending the reply. \n
    #BT_MESH_ACCESS_MSG_STATUS_INVALID_APPKEY, cannot find the corresponding address. \n
*/
bt_mesh_access_msg_status_code_t bt_mesh_model_bind_model_app(uint16_t model_handle, uint16_t appkey_index);

/*!
    @brief Getting the composition data of this device.
    @return The composition data. It must be freed by #bt_mesh_model_free_composition_data after it is used.
*/
bt_mesh_composition_data_t *bt_mesh_model_get_composition_data(void);

/*!
    @brief Parsing a byte array to composition data.
    @param[in] buffer is the byte array of the composition data.
    @param[in] length is the length of the buffer.
    @return The parsed composition data. It must be freed by #bt_mesh_model_free_composition_data after it is used.
*/
bt_mesh_composition_data_t *bt_mesh_model_parse_composition_data(const uint8_t *buffer, uint16_t length);

/*!
    @brief Releasing the composition data.
    @param[in] composition_data is the composition data to be freed.
    @return
    #BT_MESH_SUCCESS, means the composition data header was successfully freed. \n
    #BT_MESH_ERROR_FAIL, means the composition data header failed to be freed.\n
*/
bt_mesh_status_t bt_mesh_model_free_composition_data(bt_mesh_composition_data_t *composition_data);

/*!
    @brief Getting the publication state of the specified model.
    @param[in] model_handle is the handle of the model.
    @return The publication state of the model.
*/
bt_mesh_model_publication_t *bt_mesh_model_get_publication_state(uint16_t model_handle);

/*!
    @brief Getting the subscription list of the specified model.
    @param[in] model_handle is the handle of model.
    @param[out] addr_count
    @return The subscription list array. It must be freed after it is used.
*/
uint16_t *bt_mesh_model_get_subscription_list(uint16_t model_handle, uint32_t *addr_count);

/*!
    @brief Getting the element index of the specified model.
    @param[in] model_handle is the handle of the model.
    @return The element index of this model.
*/
uint16_t bt_mesh_model_get_element_index_by_model_handle(uint16_t model_handle);
/*!
@}
@}
@}
*/

#endif // __BT_MESH_MODEL_H__
