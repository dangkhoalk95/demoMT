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

#ifndef __DTP_H__
#define __DTP_H__

#include "bt_type.h"
#include "bt_gap_le.h"

typedef enum {
    DTP_SUCCESS,
    DTP_FAIL = -1,
    DTP_FAIL_INVALID_DATA = -2,
    DTP_FAIL_BUSY = -3,
    DTP_FAIL_DIABLED = -4
} ble_dtp_err_code_t;

typedef enum
{
    DTP_EVENT_ENABLED,
    DTP_EVENT_DISABLED,
    DTP_EVENT_DATA_RECEIVED,
    DTP_EVENT_DATA_SENT
} ble_dtp_event_id_t;

typedef enum {
    BLE_DTP_CONN_DEFAULT,
    BLE_DTP_CONN_HIGH_SPEED,
    BLE_DTP_CONN_LOW_POWER
} ble_dtp_conn_speed_t;

typedef  struct
{
    ble_dtp_event_id_t event_id;
    ble_dtp_err_code_t result;
    uint8_t *data;
    uint16_t length; /*the length of data*/
}dtp_event_t;

typedef void (* dtp_event_cb_t) (dtp_event_t *event);  

typedef struct 
{
    uint16_t indicate_enable;
    uint16_t conn_handle;
    
    //information of data to sent
    uint8_t *p_buff;    //the start pointer of the data buffer
    uint16_t len;       //the total length of the data
    uint16_t left_len;  //the length of left data/*if notification is enabled*/
    dtp_event_cb_t event_cb;
}ble_dtp_t;

ble_dtp_err_code_t ble_dtp_send_data(uint16_t buff_len, uint8_t *buff);
void ble_dtp_register(dtp_event_cb_t callback);
bt_status_t ble_dtp_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);


extern bt_gap_le_local_config_req_ind_t *dtp_bt_gap_le_get_local_config(void);
extern bt_gap_le_bonding_info_t *dtp_bt_gap_le_get_bonding_info(const bt_addr_t remote_addr);
extern bt_status_t dtp_bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);


#endif /*__DTP_H__*/

