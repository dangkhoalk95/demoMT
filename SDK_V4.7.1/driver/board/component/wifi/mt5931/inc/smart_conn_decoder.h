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

#ifndef __PARSER_FSM_H__
#define __PARSER_FSM_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned long long BITMAP_T;
#define bits_mask(from, span) ((((BITMAP_T)(-1)) >> ((sizeof(BITMAP_T)*8)-(span))) << (from))
#define offset_of(type, member) ((unsigned long) &((type *)0)->member)

#define BIT_MASK_TA (bits_mask(sizeof(BITMAP_T)*8-offset_of(struct smart_conn_body, ta)/2-ETH_MAC_LEN/2, ETH_MAC_LEN/2))
#define BIT_MASK_MODE (bits_mask(sizeof(BITMAP_T)*8-offset_of(struct smart_conn_body, mode)/2-1, 1))
#define BIT_MASK_LEN (bits_mask(sizeof(BITMAP_T)*8-offset_of(struct smart_conn_body, len)/2-1, 1))

#define SSID_PWD_LEN_MAX 48 /*Should be 32+64, we use half to save memory*/
#define PFSM_RST_CNTER 128
#define M80211_HEADER_LEN_MIN 24
#define ETH_MAC_LEN 6
#define PFSM_IDX_MIN 0x15   /*the start value of index.*/
#define PFSM_IDX_MAX 0x54   /*the max value of index.*/
typedef void (*status_cb)(char, unsigned char, unsigned char, char *, unsigned char);

enum parser_fsm_state {
    PARSER_STATE_IDLE = 0,
    PARSER_STATE_SYNC0,
    PARSER_STATE_SYNC1,
    PARSER_STATE_SYNC2,
    PARSER_STATE_DATA,
    PARSER_STATE_MAX
};

enum parser_status_code {
    S_SYNFAIL = 0x0,
    S_SYNCSUC,
    S_INFOGET,
    S_MAX
};

enum parser_err_code {
    ESTATE = 0x3000,
    ELOCK,
    EMACLEN,
    EPARAM,
    EMAX
};

__packed struct smart_conn_body {
    unsigned char ta[ETH_MAC_LEN];
    unsigned char mode[2];
    unsigned char len[2];
    char s_p[96];
};

struct parser_fsm {
    /* Local MAC addr. */
    char la[ETH_MAC_LEN];

    /* Func pointer used to indicate SSID/PWD/MODE. */
    status_cb sts_cb;

    enum parser_fsm_state state;

    /* Source MAC Addr of SP, used to filter package. */
    char sa[ETH_MAC_LEN];

    /* Rst-Counter used in each phase to determine whether go to IDLE state. */
    unsigned char rc;

    unsigned char reported;

    /* the data len of the input buffer, used to filter package*/
    unsigned short data_len;

    /* buffer user to store Smart Connection config data. */
    struct smart_conn_body data;

    /*bit map used to record received data. */
    BITMAP_T bitmap;
};

int parser_fsm_init(char *la, status_cb sts_cb);
int parser_fsm_go(char *p, int len);

#endif

