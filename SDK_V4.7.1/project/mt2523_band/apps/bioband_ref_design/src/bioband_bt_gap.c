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

/*****************************************************************************
 *
 * Description:
 * ------------
 *
 *
 ****************************************************************************/
#include "bt_gap.h"
#include "bt_type.h"
#include "syslog.h"
#include "nvdm.h"

#define LINK_KEY_NUM 5
#define FOTA_LINK_KEY_NVDM_GROUP     "fota_link_key"
#define FOTA_LINK_KEY_NVDM_NAME      "edrs"

static bt_gap_link_key_notification_ind_t edr_keys[LINK_KEY_NUM];

void fota_save_link_key(void)
{
    nvdm_status_t ret = nvdm_write_data_item(
                            FOTA_LINK_KEY_NVDM_GROUP,
                            FOTA_LINK_KEY_NVDM_NAME, 
                            NVDM_DATA_ITEM_TYPE_RAW_DATA,
                            (uint8_t*)&edr_keys[0],
                            sizeof(edr_keys));

    if (NVDM_STATUS_OK != ret) {
        LOG_E(fota_dl_m, "[FOTA] failed to save link key to nvdm, err: %d\r\n", ret);
    } else {
        LOG_I(fota_dl_m, "[FOTA] succeed to save link key to nvdm.\r\n");
    }
}

void fota_load_link_key(void)
{
    uint32_t len = sizeof(edr_keys);
    LOG_I(fota_dl_m, "[FOTA] nvdm data len: %u\r\n", len);
    nvdm_status_t ret = nvdm_read_data_item (
                            FOTA_LINK_KEY_NVDM_GROUP,
                            FOTA_LINK_KEY_NVDM_NAME, 
                            (uint8_t*)edr_keys,
                            &len);

    if (NVDM_STATUS_OK != ret) {
        LOG_E(fota_dl_m, "[FOTA] failed to load link key, err: %d\r\n", ret);
    } else if (len != sizeof(edr_keys)) {
        LOG_E(fota_dl_m, "[FOTA] link key loading is not complete.\r\n");
    } else {
        LOG_I(fota_dl_m, "[FOTA] succeed to load link key.\r\n");
    }
}

    
static bt_gap_link_key_notification_ind_t* bt_find_link_key_edr(bt_bd_addr_t *address)
{
    uint32_t i;
    for (i = 0; i < LINK_KEY_NUM; i++) {
        if (edr_keys[i].key_type == BT_GAP_LINK_KEY_TYPE_INVAILIDE) {
            memset(&(edr_keys[i].address), 0, sizeof(bt_bd_addr_t));
        }
        if (memcmp(address, &(edr_keys[i].address), sizeof(bt_bd_addr_t)) == 0) {
            return edr_keys + i;
        }
    }
    return NULL;
}
static bt_gap_link_key_notification_ind_t* bt_alloc_link_key_edr(bt_bd_addr_t *address)
{
    uint32_t i;
    bt_bd_addr_t empty = {0};
    bt_gap_link_key_notification_ind_t* key;
    key = bt_find_link_key_edr(address);
    if (key == NULL) {
        key = bt_find_link_key_edr(&empty);        
    }
    if (key == NULL) {
        for (i = 0; i < LINK_KEY_NUM - 1; i++) {
            memcpy(&(edr_keys[i]), &(edr_keys[i + 1]), sizeof(bt_gap_link_key_notification_ind_t));
        }
        key = edr_keys + (LINK_KEY_NUM - 1);
        key->key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
    }
    return key;
}

void bioband_bt_gap_get_link_key(bt_gap_link_key_notification_ind_t* key_information)
{
    bt_gap_link_key_notification_ind_t* key;
    key = bt_find_link_key_edr(&(key_information->address));
    if (key) {
        memcpy(key_information, key, sizeof(bt_gap_link_key_notification_ind_t));
    }
    else 
    {
        key_information->key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
    }
    return;
}

bt_status_t bio_bt_gap_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    //printf("msg_id = 0x%x\n");
    switch (msg) {
        case BT_POWER_ON_CNF: 
        {
            bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
            break;
        }
        case BT_GAP_IO_CAPABILITY_REQ_IND: /*103*/
            bt_gap_reply_io_capability_request(BT_GAP_OOB_DATA_PRESENTED_NONE, 
                                        BT_GAP_SECURITY_AUTH_REQUEST_GENERAL_BONDING_AUTO_ACCEPTED /* BT_GAP_SECURITY_AUTH_REQUEST_MITM_GENERAL_BONDING */
                                        );
            break;
            
        case BT_GAP_USER_CONFIRM_REQ_IND: /*104*/
            bt_gap_reply_user_confirm_request(true);
            break;
            
        case BT_GAP_LINK_KEY_NOTIFICATION_IND: {
            bt_gap_link_key_notification_ind_t * key_info = (bt_gap_link_key_notification_ind_t *) buff;
            bt_gap_link_key_notification_ind_t * key;
            if (key_info->key_type == BT_GAP_LINK_KEY_TYPE_INVAILIDE) {
                key = bt_find_link_key_edr(&(key_info->address));
                if(key != NULL) {
                    memset(key, 0, sizeof(bt_gap_link_key_notification_ind_t));
                    key->key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
                    }
             }
           else {
                  key = bt_alloc_link_key_edr(&(key_info->address));
                  memcpy(key, key_info, sizeof(bt_gap_link_key_notification_ind_t));
              }
            fota_save_link_key();
            break;
        }
        case BT_GAP_LINK_STATUS_UPDATED_IND:
        {
             bt_gap_link_status_updated_ind_t *ind = (bt_gap_link_status_updated_ind_t *)buff;
             if (ind->link_status >= BT_GAP_LINK_STATUS_CONNECTED_0)
             {
                 bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
             }
             else if (ind->link_status == BT_GAP_LINK_STATUS_DISCONNECTED)
             {
                 bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
             }
             break;
        }
        
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

