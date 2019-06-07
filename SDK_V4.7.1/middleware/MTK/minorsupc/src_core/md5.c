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

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif


//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "che_api.h"

void hmac_md5_vector(kal_uint8 *key, kal_uint16 key_len, kal_uint16 num_elem,
                     kal_uint8 *msg[], kal_uint16 *msg_len, kal_uint8 *digest)
{
    STCHE che_context;
    int i;

    che_init(&che_context, CHE_MAC_MD5);
    che_key_action(&che_context, CHE_SET_KEY, (kal_uint8 *) key, key_len);

    for (i = 0; i < num_elem; i++) {
        if (i == num_elem - 1) {
            che_process(&che_context, CHE_MAC_MD5, CHE_MODE_NULL, CHE_HASH, (kal_uint8 *) msg[i], digest, msg_len[i], KAL_TRUE);
        } else {
            che_process(&che_context, CHE_MAC_MD5, CHE_MODE_NULL, CHE_HASH, (kal_uint8 *) msg[i], digest, msg_len[i], KAL_FALSE);
        }
    }
    che_deinit(&che_context);
}

void hmac_md5(kal_uint8 *key, kal_uint16 key_len, kal_uint8 *data, kal_uint16 data_len,
              kal_uint8 *digest)
{
    hmac_md5_vector(key, key_len, 1, &data, &data_len, digest);
}


void md5_vector(kal_uint8 *digest, kal_uint16 num_elem, kal_uint8 *msg[], kal_uint16 *msg_len)
{
    STCHE che_context;
    int i;

    che_init(&che_context, CHE_MD5);

    for (i = 0; i < num_elem; i++) {
        if (i == num_elem - 1) {
            che_process(&che_context, CHE_MD5, CHE_MODE_NULL, CHE_HASH, (kal_uint8 *) msg[i], (kal_uint8 *) digest, msg_len[i], KAL_TRUE);
        } else {
            che_process(&che_context, CHE_MD5, CHE_MODE_NULL, CHE_HASH, (kal_uint8 *) msg[i], (kal_uint8 *) digest, msg_len[i], KAL_FALSE);
        }
    }
    che_deinit(&che_context);
}

void md5(kal_uint8 *digest, kal_uint8 *msg, kal_uint16 msg_len)
{
    md5_vector(digest, 1, &msg, &msg_len);
}
