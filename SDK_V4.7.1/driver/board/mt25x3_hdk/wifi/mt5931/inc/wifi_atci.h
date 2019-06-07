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

#ifndef __WIFI_ATCI_H__
#define __WIFI_ATCI_H__
#include "atci.h"

typedef enum {
    WIFI_EM_ENTER  = 0x00,
    WIFI_EM_STOP   = 0x01,
    WIFI_EM_TX     = 0x02,
    WIFI_EM_RX     = 0x03,
    WIFI_EM_CAL    = 0x04,
    WIFI_EM_BW     = 0x05, //Set WiFi Bandwidth
    WIFI_EM_MAX
} wifi_test_item_t;

typedef enum {
    WIFI_TX_CT  = 0x00,
    WIFI_TX_CS  = 0x01,
    WIFI_TX_LF  = 0x02,
    WIFI_TX_PT  = 0x03,
    WIFI_TX_MAX
} wifi_tx_mode_t;

typedef enum {
    ATCI_PRINT_NONE    = 0x00,
    ATCI_PRINT_COMMON  = 0x01,
    ATCI_PRINT_ERROR   = 0x02,
    ATCI_PRINT_WARNING = 0x03,
    ATCI_PRINT_INFO    = 0x04,
    ATCI_PRINT_MAX
} atci_print_level_t;

#if 0
#define ATCI_PRINT(level, message, ...) \
do { \
    if (level <= wifi_atci_print_level) { \
        atci_response_t print_response = {0}; \
        sprintf(print_response.response_buf, message, ##__VA_ARGS__); \
        print_response.response_len = strlen((char *)print_response.response_buf); \
        if(level == ATCI_PRINT_ERROR) \
            print_response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; \
        else \
            print_response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; \
        atci_send_response(&print_response); \
    } \
} while(0)
#endif
#define ATCI_PRINT(level, message, ...) \
do { \
    if (level <= wifi_atci_print_level) { \
        atci_response_t print_response = {{0}}; \
        sprintf((char *)print_response.response_buf, message, ##__VA_ARGS__); \
        print_response.response_len = strlen((char *)print_response.response_buf); \
        atci_send_response(&print_response); \
    } \
} while(0)

atci_status_t atci_cmd_hdlr_wifi_em_cmd(atci_parse_cmd_param_t *parse_cmd);
atci_status_t atci_cmd_hdlr_wifi_cmd(atci_parse_cmd_param_t *parse_cmd);

#endif/*__WIFI_ATCI_H__*/
