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


#ifndef SHA1_H
#define SHA1_H

#ifdef EAP_TLS_FUNCS

#include <openssl/sha.h>

#define SHA1_CTX SHA_CTX
#define SHA1Init SHA1_Init
#define SHA1Update SHA1_Update
#define SHA1Final SHA1_Final
#define SHA1Transform SHA1_Transform
#define SHA1_MAC_LEN SHA_DIGEST_LENGTH

#else /* EAP_TLS_FUNCS */

#define SHA1_MAC_LEN 20

typedef struct {
    u32 state[5];
    u32 count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void SHA1Init(SHA1_CTX *context);
void SHA1Update(SHA1_CTX *context, const void *data, u32 len);
void SHA1Final(unsigned char digest[20], SHA1_CTX *context);
void SHA1Transform(u32 state[5], const unsigned char buffer[64]);

#endif /* EAP_TLS_FUNCS */

extern void hmac_sha1_vector(kal_uint8 *key, kal_uint16 key_len, kal_uint16 num_elem,
                             kal_uint8 *msg[], kal_uint16 *msg_len, kal_uint8 *digest);
extern void hmac_sha1(kal_uint8 *key, kal_uint16 key_len, kal_uint8 *data, kal_uint16 data_len,
                      kal_uint8 *mac);
extern void sha1_t_prf(kal_uint8 *key, kal_uint16 key_len, const char *label,
                       kal_uint8 *seed, kal_uint16 seed_len, kal_uint8 *buf, kal_uint16 buf_len);
extern int tls_prf(kal_uint8 *secret, kal_uint16 secret_len, const char *label,
                   kal_uint8 *seed, kal_uint16 seed_len, kal_uint8 *out, kal_uint16 outlen);
extern void pbkdf2_sha1(const char *passphrase, const char *ssid, kal_uint16 ssid_len,
                        int iterations, kal_uint8 *buf, kal_uint16 buflen);
extern void sha1_prf(kal_uint8 *key, kal_uint16 key_len, const char *label,
                     kal_uint8 *data, kal_uint16 data_len, kal_uint8 *buf, kal_uint16 buf_len);
void sha1_transform(u8 *state, u8 data[64]);
extern void sha1_vector(kal_uint16 num_elem, kal_uint8 *msg[], kal_uint16 *msg_len,
                        kal_uint8 *digest);

#endif /* SHA1_H */
