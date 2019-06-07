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
 
#include "bt_gap_le.h"
#include "bt_gatts.h"
#include "bt_callback_manager.h"
#include "bt_connection_info.h"

static bt_gap_le_local_config_req_ind_t local_config;

/* Start of flash. */
bt_gap_le_smp_pairing_config_t pairing_config_req = {
    .io_capability = BT_GAP_LE_SMP_NO_INPUT_NO_OUTPUT,
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_BONDING,
    .maximum_encryption_key_size = 16,
};

bt_gap_le_local_key_t local_key_req = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};
/* End of flash. */

/* Start of default configuration, don't edit here. */
bool sc_only = false;
/* End of default configuration. */

static bool g_bt_hci_log_enable = false;

bool bt_hci_log_enabled(void)
{
    return g_bt_hci_log_enable;
}

void bt_hci_log_enable(bool enable)
{
    g_bt_hci_log_enable = enable;
}

bt_gap_le_bonding_info_t *bt_app_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    app_bt_bonded_info_t *bonded_info = get_bonded_info(&remote_addr, 1);
    if (bonded_info) {
        return &(bonded_info->info);
    }

    return NULL;
}

bt_gap_le_local_config_req_ind_t *bt_app_gap_le_get_local_config(void)
{
    local_config.local_key_req = &local_key_req;
    local_config.sc_only_mode_req = sc_only;

    return &local_config;
}

bt_status_t bt_app_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    ind->pairing_config_req = pairing_config_req;

    return BT_STATUS_SUCCESS;
}

bt_status_t bt_app_gatts_get_authorization_check_result(bt_gatts_authorization_check_req_t *req)
{
    bt_gap_le_bonding_info_t *bonded_info = &(find_bonded_info_by_handle(req->connection_handle)->info);
    BT_LOGI("APP", "Peer ask to access attribute with authorization requirement.");
    BT_LOGI("APP", "connection[0x%04x] attribute handle[0x%04x] [%s]", req->connection_handle, req->attribute_handle,
            req->read_write == BT_GATTS_CALLBACK_READ ? "Read" : "Write");
    BT_LOGI("APP", "Security mode[0x%02x]", bonded_info->key_security_mode);
    if ((bonded_info->key_security_mode & BT_GAP_LE_SECURITY_AUTHENTICATION_MASK) > 0) {
        /* If you agree peer device can access all characteristic with
           authorization permission, you can set #BT_GAP_LE_SECURITY_AUTHORIZATION_MASK
           flag, and GATTS will not call for authorization check again. */
        bonded_info->key_security_mode = bonded_info->key_security_mode | BT_GAP_LE_SECURITY_AUTHORIZATION_MASK;
        /* If application accept peer access this attribute. */
        return BT_STATUS_SUCCESS;
    } else {
        /* If application reject peer access this attribute. */
        return BT_STATUS_UNSUPPORTED;
    }

}

void bt_app_callback_handler_init()
{
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig,
                                          0,
                                          (void *)bt_app_gap_le_get_local_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_pairing_config,
                                          0,
                                          (void *)bt_app_gap_le_get_pairing_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_bonding_info,
                                          0,
                                          (void *)bt_app_gap_le_get_bonding_info);
    bt_callback_manager_register_callback(bt_callback_type_gatts_get_authorization_check_result,
                                          0,
                                          (void *)bt_app_gatts_get_authorization_check_result);

}

