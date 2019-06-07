/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

 
#include "bt_hci.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_system.h"
#include "bt_debug.h"
#include "syslog.h"
#include "fota_bt_common.h"

LOG_CONTROL_BLOCK_DECLARE(fota_dl_m);

 
/****************************************************************************
* ROMable data
* SPP sdp record for FOTA
****************************************************************************/
  
#define FOTA_BT_SPP_STANDARD_UUID    0x00,0x00,0xFF,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFF

static const uint8_t fota_bt_spp_app_service_class_id[] =
{
    BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
    BT_SPP_SDP_ATTRIBUTE_UUID(FOTA_BT_SPP_STANDARD_UUID)
};

static const uint8_t fota_bt_spp_app_protocol_descriptor_list[] =
{
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(FOTA_BT_SPP_SERVER_ID)
};

static const uint8_t fota_bt_spp_app_browse_group[] =
{
    BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
};

static const uint8_t fota_bt_spp_app_language[] =
{
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE
};

static const uint8_t fota_bt_spp_app_service_name[] =
{
    BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(9),
    'B', 'T', 'N', 'O', 'T', 'I', 'F', 'Y', 'R'
};

static const bt_sdps_attribute_t fota_bt_spp_app_sdp_attributes[] =
{
    /* Service Class ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(fota_bt_spp_app_service_class_id),
    /* Protocol Descriptor List attribute */
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(fota_bt_spp_app_protocol_descriptor_list),
    /* Public Browse Group Service */
    BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(fota_bt_spp_app_browse_group),
    /* Language Base ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(fota_bt_spp_app_language),
    /* Serial Port Profile Service Name */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(fota_bt_spp_app_service_name)
};

const bt_sdps_record_t fota_spp_sdp_record = 
{
    .attribute_list_length = sizeof(fota_bt_spp_app_sdp_attributes),
    .attribute_list = fota_bt_spp_app_sdp_attributes,
};



static bt_gap_le_smp_pairing_config_t pairing_config = {//mitm, bond, oob
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_BONDING,        
    .maximum_encryption_key_size = 16,
};


static bt_gap_le_local_key_t local_key = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};

static bt_gap_le_bonding_info_t  bonding_info;
static bt_gap_le_local_config_req_ind_t local_config;

/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
#if 0
bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{

     /*Listen all BT event*/
     return BT_STATUS_SUCCESS;
}
#endif

bt_status_t fota_bt_app_event_callback(
    bt_msg_type_t msg, 
    bt_status_t status, 
    void *buff)

{
    LOG_I(fota_dl_m, "[FOTA] event:0x%x, %x\n", msg, status);
    switch (msg) {
        case BT_GAP_SET_SCAN_MODE_CNF:/*109*/
            break;
            

        case BT_GAP_IO_CAPABILITY_REQ_IND:/*103*/
           bt_gap_reply_io_capability_request(BT_GAP_OOB_DATA_PRESENTED_NONE,
                BT_GAP_SECURITY_AUTH_REQUEST_GENERAL_BONDING_AUTO_ACCEPTED);
            break;

        case BT_GAP_USER_CONFIRM_REQ_IND:/*104*/
            bt_gap_reply_user_confirm_request(true);
            break;
      
        break;  
        case BT_POWER_ON_CNF:  /*24000001*/
            LOG_I(fota_dl_m, "[FOTA] bt power on confirm.\n");
            bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
            break;      

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

/**
 * @brief     This API invoked by the SDK process should be implemented by the application. The application should return the Local Configuration field.
 * @return    The loacl configuration pointer, please set the local key and secure connection mode flag. The pointer should not be NULL and it must be a global variable.
 */
bt_gap_le_local_config_req_ind_t *fota_bt_gap_le_get_local_config(void)
{
    local_config.local_key_req = &local_key;
    local_config.sc_only_mode_req = false;
 
    return &local_config;
}

/**
 * @brief   This API invoked by the SDK process should be implemented by the application. The application should return the Bonding Information field.
 * @param[in] remote_addr The address of the remote device to be bonded.
 * @return                The Bonding Information pointer, please set a pointer to the connection bonding information. The pointer should not be NULL and it must be a global variable.
 */
bt_gap_le_bonding_info_t *fota_bt_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    return &bonding_info;
}

/**
 * @brief   This API invoked by the SDK process should be implemented by the application. The application should return the Pairing Configuration field.
 * @param[in] ind         Bonding start indication structure. Application should set the pairing_config_req variable to a global variable.
 * @return    #BT_STATUS_SUCCESS, the application set the pairing configuration successfully.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t fota_bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    ind->pairing_config_req = pairing_config;

    return BT_STATUS_SUCCESS;
}



#define DEVICE_NAME "fota_download"
bt_gap_config_t bt_custom_config;

const bt_gap_config_t* fota_bt_gap_get_local_configuration(void)
{

    LOG_I(fota_dl_m, "bt_get_local_configuration1\n");
    bt_custom_config.inquiry_mode = 3;
    bt_custom_config.io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
    bt_custom_config.cod = 0x240404;
    memcpy(&bt_custom_config.device_name, DEVICE_NAME, sizeof(DEVICE_NAME));

    
    LOG_I(fota_dl_m,"1:%08x\r\n", (unsigned int)DEVICE_NAME);
    return  &bt_custom_config;
}

