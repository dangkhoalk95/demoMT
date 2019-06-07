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

#ifndef AES_WRAP_H
#define AES_WRAP_H

void aes_wrap(u8 *kek, int n, u8 *plain, u8 *cipher);
int aes_unwrap(u8 *kek, int n, u8 *cipher, u8 *plain);
void omac1_aes_128(const u8 *key, const u8 *data, wpa_size_t data_len, u8 *mac);
/* Start -- Jau modify prototype from void to int { */
//void aes_128_encrypt_block(const u8 *key, const u8 *in, u8 *out);
int aes_128_encrypt_block(const u8 *key, const u8 *in, u8 *out);
void aes_128_ctr_encrypt(const u8 *key, const u8 *nonce,
                         u8 *data, wpa_size_t data_len);
/* Start -- Jau modify prototype from void to int { */

int aes_128_eax_encrypt(const u8 *key, const u8 *nonce, wpa_size_t nonce_len,
                        const u8 *hdr, wpa_size_t hdr_len,
                        u8 *data, wpa_size_t data_len, u8 *tag);
int aes_128_eax_decrypt(const u8 *key, const u8 *nonce, wpa_size_t nonce_len,
                        const u8 *hdr, wpa_size_t hdr_len,
                        u8 *data, wpa_size_t data_len, const u8 *tag);
int aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data,
                        wpa_size_t data_len);
int aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data,
                        wpa_size_t data_len);

#endif /* AES_WRAP_H */
