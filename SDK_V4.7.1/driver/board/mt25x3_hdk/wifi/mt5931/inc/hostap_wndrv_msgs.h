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

#ifndef HOSTAP_WNDRV_MSG_H
#define HOSTAP_WNDRV_MSG_H

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define HOSTAP_MAX_ELEM_DATA_SIZE 50 /* need confirm */
#define HOSTAP_ESSID_MAX_LEN 32

#define MAX_IE_LEN  1024 /* need confirm, modify by suchao from 2312 to 1024 for buffer defination*/
#define HOSTAP_CRYPT_ALG_NAME_LEN   16
#define MAX_KEY_LEN 32


/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_init_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
} hostap_wndrv_init_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_deinit_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
} hostap_wndrv_deinit_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_flush_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
} hostap_wndrv_flush_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_add_sta_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    sta_addr[ETH_ALEN];
    kal_uint16   aid; /* association id */
    kal_uint16   capability;
    kal_uint16   tx_supp_rates;
} hostap_wndrv_add_sta_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_remove_sta_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_remove_sta_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_get_sta_info_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_get_sta_info_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_sta_flags_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    sta_addr[ETH_ALEN];
    kal_uint32   flags_and;
    kal_uint32   flags_or;
} hostap_wndrv_set_sta_flags_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_generic_elem_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    len;
    kal_uint8    data[ MAX_IE_LEN ];
} hostap_wndrv_set_generic_elem_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_essid_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_uint16                  essid_flag;
    kal_uint16                  essid_len;
    kal_uint8                   essid[ HOSTAP_ESSID_MAX_LEN ];
} hostap_wndrv_set_essid_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_8021x_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_bool                    fgIs1x;
} hostap_wndrv_set_8021x_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_host_encrypt_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_bool                    fgIsEncrypt;
} hostap_wndrv_set_host_encrypt_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_host_decrypt_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_bool                    fgIsDecrypt;
} hostap_wndrv_set_host_decrypt_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_privacy_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_bool                    fgIsPrivacy;
} hostap_wndrv_set_privacy_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_encryption_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;

    kal_uint8                   sta_addr[ETH_ALEN];
    kal_uint16                  alg_len;
    kal_uint8                   alg[ HOSTAP_CRYPT_ALG_NAME_LEN ];
    kal_uint32                  flags;
    kal_uint8                   idx;
    kal_uint16                  key_len;
    kal_uint8                   key[ MAX_KEY_LEN ];
} hostap_wndrv_set_encryption_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_get_encryption_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    sta_addr[ETH_ALEN];
    kal_uint8    idx;
} hostap_wndrv_get_encryption_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_channel_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    channel_num;
    kal_uint8    ap_phytype;
} hostap_wndrv_set_channel_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_frame_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    /* 802.11 hdr */
    kal_uint16   frame_control;
    kal_uint16   duration_id;
    kal_uint8    addr1[6];
    kal_uint8    addr2[6];
    kal_uint8    addr3[6];
    kal_uint16   seq_ctrl;
} hostap_wndrv_frame_req_struct;


/***************************************************************************
*  WNDRV -> HOSTAP Indication Message
***************************************************************************/

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_init_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint8    ap_addr[ETH_ALEN];
} hostap_wndrv_init_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_deinit_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
} hostap_wndrv_deinit_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_add_sta_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_add_sta_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_remove_sta_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_remove_sta_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_get_sta_info_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint32   inactive_sec;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_get_sta_info_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_sta_flags_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_set_sta_flags_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_flush_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_flush_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_generic_elem_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_set_generic_elem_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_essid_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_set_essid_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_8021x_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_set_8021x_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_host_encrypt_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_set_host_encrypt_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_host_decrypt_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_set_host_decrypt_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_privacy_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
} hostap_wndrv_set_privacy_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_encryption_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint32   err;
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_set_encryption_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_get_encryption_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint8    seq[8]; /* sequence counter (set: RX, get: TX) */
    kal_uint8    sta_addr[ETH_ALEN];
} hostap_wndrv_get_encryption_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_set_channel_cnf_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool     status;
    kal_uint8    channel_num;
} hostap_wndrv_set_channel_cnf_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     hostap_wndrv_frame_ind_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    /* 802.11 hdr */
    kal_uint16   frame_control;
    kal_uint16   duration_id;
    kal_uint8    addr1[6];
    kal_uint8    addr2[6];
    kal_uint8    addr3[6];
    kal_uint16   seq_ctrl;
} hostap_wndrv_frame_ind_struct;

#endif /* HOSTAP_WNDRV_MSG_H */
