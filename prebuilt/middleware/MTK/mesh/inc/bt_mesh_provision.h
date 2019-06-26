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

#ifndef __BT_MESH_PROVISION_H__
#define __BT_MESH_PROVISION_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "bt_mesh_config.h"

/**
 *   @addtogroup BluetoothService Bluetooth Services
 *   @{
 *   @addtogroup BluetoothMesh Mesh
 *   @{
 *   @addtogroup BluetoothMeshFeatures Features 
 *   @{
 *   This section introduces the APIs for mesh features including provisioning, proxy, and friend feature, with descriptions of the terms and acronyms. It shows how to use
 *   the enums and structures for each feature and the API prototypes for: sending unprovisioned beacons; scanning unprovisioned devices; starting the provisioning process; 
 *   providing OOB info for the provisioning process; adding or removing and address; and setting the filter type for a proxy node.
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b OOB                        | Out-of-band. An association mode, primarily designed for scenarios where an Out-of-Band mechanism is used to discover devices and to exchange or transfer cryptographic numbers used during the pairing process. |
 * |\b ADV                        | Advertising. A device sends data in either non-connectable undirected or scannable undirected advertising events. |
 * |\b GATT                       | Generic Attribute Profile. A service framework using the Attribute Protocol for discovering services, and for reading and writing characteristic values on a peer device. |

 * @section bt_mesh_prov_api_usage How to do provisioning
 *
 * - The application calls the #bt_mesh_provision_start_provisioning function.
 * - It shows how a provisioner starts the provisioning process. The provisioning parameters must be assigned in #bt_mesh_prov_provisioner_params_t.
 * - This example does not use an OOB public key or an OOB authentication value.
 * - Sample code:
 *      @code
    void start_provisioning(uint8_t *target_uuid, uint8_t *network_key)
    {
        bt_mesh_prov_provisioner_params_t params;
        // invite
        params.invite.attention_duration = 0;
        // start
        params.start.algorithm = BT_MESH_PROV_START_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE;
        params.start.public_key = BT_MESH_PROV_START_PUBLIC_KEY_NO_OOB;
        params.start.authentication_method = BT_MESH_PROV_START_AUTHEN_METHOD_NO_OOB;
        params.start.authentication_action = 0;
        params.start.authentication_size = 0;
        // data
        memcpy(params.data.netkey, network_key, BT_MESH_NETKEY_SIZE);
        params.data.netkey_index = 0;
        params.data.iv_index = 0;
        params.data.address = 0x1234;
        params.data.flags = 0;

        bt_mesh_provision_start_provisioning(target_uuid, &params);
    }
 *      @endcode
 *
*/

/**
 * @defgroup Bluetooth_mesh_features_define Define 
 * @{
 */

/*!
     @brief This bit field is used in #bt_mesh_prov_unprovisioned_device_t.
 */
#define BT_MESH_PROV_OOB_INFO_FIELD_OTHER                          (1 << 0x00)   /**< Other location. */
#define BT_MESH_PROV_OOB_INFO_FIELD_ELECTRONIC_URI                 (1 << 0x01)   /**< Electronic/URI. */
#define BT_MESH_PROV_OOB_INFO_FIELD_2D_MACHINE_READABLE_CODE       (1 << 0x02)   /**< 2D machine-readable code. */
#define BT_MESH_PROV_OOB_INFO_FIELD_BAR_CODE                       (1 << 0x03)   /**< Bar code. */
#define BT_MESH_PROV_OOB_INFO_FIELD_NFC                            (1 << 0x04)   /**< Near Field Communication (NFC). */
#define BT_MESH_PROV_OOB_INFO_FIELD_NUMBER                         (1 << 0x05)   /**< Number. */
#define BT_MESH_PROV_OOB_INFO_FIELD_STRING                         (1 << 0x06)   /**< String. */
#define BT_MESH_PROV_OOB_INFO_FIELD_ON_BOX                         (1 << 0x0B)   /**< On box. */
#define BT_MESH_PROV_OOB_INFO_FIELD_INSIDE_BOX                     (1 << 0x0C)   /**< Inside box. */
#define BT_MESH_PROV_OOB_INFO_FIELD_ON_PIECE_OF_PAPER              (1 << 0x0D)   /**< On a piece of paper. */
#define BT_MESH_PROV_OOB_INFO_FIELD_INSIDE_MANUAL                  (1 << 0x0E)   /**< In the manual. */
#define BT_MESH_PROV_OOB_INFO_FIELD_ON_DEVICE                      (1 << 0x0F)   /**< On the device. */

/*!
     @brief This bit field is used in #bt_mesh_prov_capabilities_t.
 */
#define BT_MESH_PROV_CAPABILITY_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE                (1<<0)    /**< Capabilities bit indicating that the FIPS P256 Elliptic Curve algorithm is supported. */

/*!
     @brief This bit field is used in #bt_mesh_prov_capabilities_t.
 */
#define BT_MESH_PROV_CAPABILITY_OOB_PUBLIC_KEY_TYPE_INBAND                        (0)       /**< Capabilities bit indicating that the public key is available in-band. This is the default value if no public key type is set. */
#define BT_MESH_PROV_CAPABILITY_OOB_PUBLIC_KEY_TYPE_OOB                           (1<<0)    /**< Capabilities bit indicating that the public key is available OOB. */

/*!
     @brief This bit field is used in #bt_mesh_prov_capabilities_t
 */
#define BT_MESH_PROV_CAPABILITY_OOB_STATIC_TYPE_SUPPORTED                         (1<<0)    /**< Capabilities bit indicating that static OOB authentication is supported. */

/*!
     @brief This bit field is used in #bt_mesh_prov_capabilities_t.
 */
#define BT_MESH_PROV_CAPABILITY_OUTPUT_OOB_ACTION_BLINK                           (1<<0)    /**< Capabilities bit indicating that the device supports blinking as an output OOB action. */
#define BT_MESH_PROV_CAPABILITY_OUTPUT_OOB_ACTION_BEEP                            (1<<1)    /**< Capabilities bit indicating that the device supports beeping as an output OOB action. */
#define BT_MESH_PROV_CAPABILITY_OUTPUT_OOB_ACTION_VIBRATE                         (1<<2)    /**< Capabilities bit indicating that the device supports vibrating as an output OOB action. */
#define BT_MESH_PROV_CAPABILITY_OUTPUT_OOB_ACTION_OUTPUT_NUMERIC                  (1<<3)    /**< Capabilities bit indicating that the device supports displaying numberic data as an output OOB action. */
#define BT_MESH_PROV_CAPABILITY_OUTPUT_OOB_ACTION_OUTPUT_ALPHANUMERIC             (1<<4)    /**< Capabilities bit indicating that the device supports displaying alphanumeric data as an output OOB action. */

/*!
     @brief This bit field is used in #bt_mesh_prov_capabilities_t.
 */
#define BT_MESH_PROV_CAPABILITY_INPUT_OOB_ACTION_PUSH                             (1<<0)    /**< Capabilities bit indicating that the device supports pushing something as an input OOB action. */
#define BT_MESH_PROV_CAPABILITY_INPUT_OOB_ACTION_TWIST                            (1<<1)    /**< Capabilities bit indicating that the device supports twisting something as an input OOB action. */
#define BT_MESH_PROV_CAPABILITY_INPUT_OOB_ACTION_INPUT_NUMBER                     (1<<2)    /**< Capabilities bit indicating that the device supports entering a number as an input OOB action. */
#define BT_MESH_PROV_CAPABILITY_INPUT_OOB_ACTION_INPUT_ALPHANUMERIC               (1<<3)    /**< Capabilities bit indicating that the device supports entering a string as an input OOB action. */

/*!
     @brief This value is used in #bt_mesh_prov_start_t.
 */
#define BT_MESH_PROV_START_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE      (0x00)    /**< FIPS P256 Elliptic Curve. */

/*!
     @brief This value is used in #bt_mesh_prov_start_t.
 */
#define BT_MESH_PROV_START_PUBLIC_KEY_NO_OOB                       (0x00)    /**< No OOB Public Key is used. */
#define BT_MESH_PROV_START_PUBLIC_KEY_OOB                          (0x01)    /**< OOB Public Key is used. */

/*!
     @brief This value is used in #bt_mesh_prov_start_t
 */
#define BT_MESH_PROV_START_AUTHEN_METHOD_NO_OOB                    (0x00)    /**< No OOB authentication is used. */
#define BT_MESH_PROV_START_AUTHEN_METHOD_STATIC_OOB                (0x01)    /**< Static OOB authentication is used. */
#define BT_MESH_PROV_START_AUTHEN_METHOD_OUTPUT_OOB                (0x02)    /**< Output OOB authentication is used. */
#define BT_MESH_PROV_START_AUTHEN_METHOD_INPUT_OOB                 (0x03)    /**< Input OOB authentication is used. */

/*!
     @brief This value is used for the Authentication Action field in #bt_mesh_prov_start_t.
 */
#define BT_MESH_PROV_START_AUTHEN_ACTION_OUTOOB_BLINK              (0x00)    /**< Blink. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_OUTOOB_BEEP               (0x01)    /**< Beep. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_OUTOOB_VIBRATE            (0x02)    /**< Vibrate. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_OUTOOB_NUMERIC            (0x03)    /**< Output Numeric. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_OUTOOB_ALPHANUMERIC       (0x04)    /**< Output Alphanumeric. */

/*!
     @brief This value is used for the Authentication Action field in #bt_mesh_prov_start_t.
 */
#define BT_MESH_PROV_START_AUTHEN_ACTION_INOOB_PUSH                (0x00)    /**< Push. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_INOOB_TWIST               (0x01)    /**< Twist. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_INOOB_NUMERIC             (0x02)    /**< Input Numeric. */
#define BT_MESH_PROV_START_AUTHEN_ACTION_INOOB_ALPHANUMERIC        (0x03)    /**< Input Alphanumeric. */

/**
 * @}
 */

/**
 * @defgroup Bluetooth_mesh_features_struct Struct 
 * @{
 */

/** @brief The unprovisioned device structure. */
typedef struct {
    uint8_t uuid[BT_MESH_UUID_SIZE];    /**< The unprovisioned device UUID. */
    uint16_t oob_info;                  /**< The OOB information of the unprovisioned device. */
    uint8_t *uri_hash;                  /**< The URI hash value of the unprovisioned device; Can be NULL. */
} bt_mesh_prov_unprovisioned_device_t;

/** @brief Provisioning Invite structure. */
typedef struct {
    uint8_t attention_duration;  /**< Attention Timer state. */
} bt_mesh_prov_invite_t;

/** @brief Provisioning the Cpabilities structure. */
typedef struct {
    uint8_t number_of_elements;   /**< The number of elements that are supported by the device. */
    uint16_t algorithms;          /**< Supported algorithms and other capabilities. */
    uint8_t public_key_type;      /**< Supported public key types. */
    uint8_t static_oob_type;      /**< Supported static OOB types. */
    uint8_t output_oob_size;      /**< The maximum size of the supported output OOB. */
    uint16_t output_oob_action;   /**< Supported output OOB actions. */
    uint8_t input_oob_size;       /**< The maximum size in octets of the supported input OOB. */
    uint16_t input_oob_action;    /**< Supported input OOB actions. */
} bt_mesh_prov_capabilities_t;

/** @brief Provisioning the Start structure. */
typedef struct {
    uint8_t algorithm;               /**< The algorithm used for provisioning. */
    uint8_t public_key;              /**< A Public Key is used. */
    uint8_t authentication_method;   /**< Authentication Method is used. */
    uint8_t authentication_action;   /**< Selected Output OOB Action or Input OOB Action or 0x00. */
    uint8_t authentication_size;     /**< The size of the Output OOB used or size of the Input OOB used or 0x00. */
} bt_mesh_prov_start_t;

/** @brief Provisioning the Data structure. */
typedef struct {
    uint8_t netkey[BT_MESH_KEY_SIZE];  /**< Network key value. */
    uint16_t netkey_index;             /**< Network key index. */
    uint32_t iv_index;                 /**< IV Index. */
    uint16_t address;                  /**< Unicast address. */
    uint8_t flags;                     /**< Flags indicate the current states of the IV update and key refresh. */
} bt_mesh_prov_data_t;

/** @brief Provisioning parameters for the provisioner. */
typedef struct {
    bt_mesh_prov_start_t start;     /**< The start parameters. */
    bt_mesh_prov_data_t data;       /**< The data parameters. */
} bt_mesh_prov_provisioner_params_t;

/** @brief The initialization parameters for the provisioning module. This parameter is used in #bt_mesh_init.*/
typedef struct {
    bt_mesh_prov_capabilities_t cap;   /**< The capabilities parameters. */
} bt_mesh_prov_provisionee_params_t;

/**
 * @}
 */

/**
    @brief Scanning for unprovisioned devices.
    @param[in] enable means the scanning state.
    @note When an unprovisioned device is scanned, an event #BT_MESH_EVT_PROV_SCAN_UD_RESULT is received.
    @return
    #BT_MESH_SUCCESS, means the unprovisioned device successfully scanned.\n
    #BT_MESH_ERROR_FAIL, means the unprovisioned device scan failed.\n
*/
bt_mesh_status_t bt_mesh_provision_scan_unprovisioned_device(bool enable);

/*!
    @brief Inviting the provisioning process for a specific UUID.
    @param[in] target_uuid is the 128-bit UUID of the target device.
    @param[in] invite is the provisioning invite parameters.
    @return
    #BT_MESH_SUCCESS, provisioning invited successfully.\n
    #BT_MESH_ERROR_INVALID_STATE, wrong state to start inviting provisioning.\n
    #BT_MESH_ERROR_INVALID_ROLE, wrong role to invite provisioning; This should only be used by the provisioner.
    @note This API is only for the provisioner role.
*/
bt_mesh_status_t bt_mesh_provision_invite_provisioning(const uint8_t *target_uuid, const bt_mesh_prov_invite_t *invite);

/*!
    @brief Starting the provisioning process with the specified start and data parameters.
    @param[in] data is the provisioning parameters.
    @return
    #BT_MESH_SUCCESS, provisioning successfully started. \n
    #BT_MESH_ERROR_INVALID_STATE, wrong state to start provisioning.\n
    #BT_MESH_ERROR_INVALID_ROLE, wrong role to start provisioning; This should only be used by the provisioner.
    #BT_MESH_ERROR_INVALID_ADDR, wrong address type or the specified address has been already used.
    @note This API is only for the provisioner role.
*/
bt_mesh_status_t bt_mesh_provision_start_provisioning(const bt_mesh_prov_provisioner_params_t *data);

/*!
    @brief Providing OOB public key.
    @param[in] public_key is the 64-byte public key.
    @return
    #BT_MESH_SUCCESS, the public key was successfully provided. \n
    #BT_MESH_ERROR_INVALID_STATE, invalid state.
*/
bt_mesh_status_t bt_mesh_provision_provide_oob_public_key(const uint8_t *public_key);

/*!
    @brief Providing an OOB authentication value.
    @param[in] authen is the authentication value.
    @param[in] len is the length of the authen parameter.
    @return
    #BT_MESH_SUCCESS, the authentication value was successfully provided. \n
    #BT_MESH_ERROR_FAIL, invalid authentication value. \n
    #BT_MESH_ERROR_INVALID_STATE, invalid state.
*/
bt_mesh_status_t bt_mesh_provision_provide_oob_auth_value(const uint8_t *authen, uint8_t len);

/*!
    @brief Set the number of retransmission times for the provisioning protocol pdu.
    @param[in] retry is the number of times of retransmission.
    @return
    #BT_MESH_SUCCESS, means the retry number was successfully set.\n
    #BT_MESH_ERROR_FAIL, means setting the retry number failed.\n
*/
bt_mesh_status_t bt_mesh_provision_set_retry(uint8_t retry);

/*!
    @brief Enabling or disabling the broadcasting unprovisioned device beacon.
    @param[in] enable means broadcasting unprovisioned device beacon.
    @return
    #BT_MESH_SUCCESS, means the unprovisioned device beacon successfully sends.\n
    #BT_MESH_ERROR_FAIL, means the unprovisioned device beacon send failed.\n
*/
bt_mesh_status_t bt_mesh_provision_send_unprovisioned_device_beacon(bool enable);

/*!
    @brief Setting the interval of advertisement of the PB-GATT and unprovisioning beacon.
    @param[in] pb_adv_tout interval of PB-GATT advertisement.
    @param[in] beacon_tout interval of unprovisioned beacon advertisement.
    @return
    #BT_MESH_SUCCESS, means the advertising timeout was successfully set.\n
    #BT_MESH_ERROR_FAIL, means setting the advertising timeout failed.\n
*/
bt_mesh_status_t bt_mesh_provision_adv_set_timeout(uint16_t pb_adv_tout, uint16_t beacon_tout);

/*!
@}
@}
@}
*/

#endif // __BT_MESH_PROVISION_H__
