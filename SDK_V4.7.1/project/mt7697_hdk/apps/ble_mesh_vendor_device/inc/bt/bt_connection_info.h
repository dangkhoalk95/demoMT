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
 
#ifndef __BT_CONNECTION_INFO_H__
#define __BT_CONNECTION_INFO_H__

#include "bt_system.h"
#include "bt_gap_le.h"
#include "project_config.h"

BT_EXTERN_C_BEGIN
#define BT_ADDR_TYPE_UNKNOW 0xFF
BT_PACKED (
typedef struct {
    bt_handle_t       connection_handle;
    bt_role_t         role;
    bt_addr_t         peer_addr;
    uint8_t           gatts_wait_att_rx_opcode;//use to wait handle value confirmation.
}) app_bt_connection_cb_t;

BT_PACKED (
typedef struct {
   bt_addr_t bt_addr;
   bt_gap_le_bonding_info_t info;
}) app_bt_bonded_info_t;

extern app_bt_connection_cb_t connection_cb[BT_CONNECTION_MAX];

void add_connection_info(void *buff);
void delete_connection_info(void *buff);
app_bt_connection_cb_t* find_conneciton_info_by_handle(bt_handle_t target_handle);
app_bt_bonded_info_t* get_bonded_info(const bt_addr_t *target_bt, uint8_t create);
app_bt_bonded_info_t* find_bonded_info_by_index(uint8_t idx);
app_bt_bonded_info_t* find_bonded_info_by_handle(bt_handle_t target_handle);
void cancel_bonded_info(const bt_addr_t *target_bt);
void clear_bonded_info(void);
void dump_bonded_info_list(void);
void dump_connection_info_list(void);
BT_EXTERN_C_END

#endif /* __BT_CONNECTION_INFO_H__ */
