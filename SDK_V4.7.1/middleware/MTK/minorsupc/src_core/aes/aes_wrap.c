/**
 * Copyright (c) 2003-2004, Jouni Malinen <jkmaline@cc.hut.fi>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice, this list of 
 *    conditions and the following disclaimer. 
 *  - Redistributions in binary form must reproduce the above copyright notice, this list of 
 *    conditions and the following disclaimer in the documentation and/or other materials provided 
 *    with the distribution.
 *  - Neither the name of the the name(s) of the above-listed copyright holder(s) nor the names of
 *    its contributors may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif

/*
 * AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 * One-Key CBC MAC (OMAC1) hash with AES-128
 * AES-128 CTR mode encryption
 * AES-128 EAX mode encryption/decryption
 * AES-128 CBC
 */
//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

#include "che_api.h"
#include "supc_common.h"

/*
 * @kek: key encryption key (KEK)
 * @n: length of the wrapped key in 64-bit units; e.g., 2 = 128-bit = 16 bytes
 * @plain: plaintext key to be wrapped, n * 64 bit
 * @cipher: wrapped key, (n + 1) * 64 bit
 */
void aes_wrap(kal_uint8 *kek, kal_uint32 n, kal_uint8 *plaintext, kal_uint8 *ciphertext)
{
    STCHE che_context;
    kal_uint8 *source = NULL;

    che_init(&che_context, CHE_AES);
    che_key_action(&che_context, CHE_SET_KEY,  kek, 16);
    source = SUPC_ALLOC_BUFF(n);
    if (source == NULL) {
        SUPC_ASSERT(0);
    }

    SUPC_ASSERT(((kal_uint32) source & 0x03) == 0);

    kal_mem_cpy(source, plaintext, n);
    che_process(&che_context, CHE_AES, CHE_KEY_WRAP, CHE_ENC, source, ciphertext, n, KAL_TRUE);
    SUPC_DEALLOC_BUFF(source);
    che_deinit(&che_context);
}


/*
 * @kek: key encryption key (KEK)
 * @n: length of the wrapped key in 64-bit units; e.g., 2 = 128-bit = 16 bytes
 * @cipher: wrapped key to be unwrapped, (n + 1) * 64 bit
 * @plain: plaintext key, n * 64 bit
 */
int aes_unwrap(kal_uint8 *kek, kal_uint32 n, kal_uint8 *ciphertext, kal_uint8 *plaintext)
{
    STCHE che_context;
    kal_bool ret = KAL_FALSE;
    kal_uint8 *source = NULL;

    che_init(&che_context, CHE_AES);
    che_key_action(&che_context, CHE_SET_KEY, kek, 16);
    source = SUPC_ALLOC_BUFF(n + 8);
    if (source == NULL) {
        SUPC_ASSERT(0);
    }

    SUPC_ASSERT(((kal_uint32) source & 0x03) == 0);

    kal_mem_cpy(source, ciphertext, n + 8);
    ret = che_process(&che_context, CHE_AES, CHE_KEY_UNWRAP, CHE_DEC, source, plaintext, n + 8, KAL_TRUE);
    SUPC_DEALLOC_BUFF(source);
    che_deinit(&che_context);
    if (ret == KAL_TRUE) {
        return 0;
    } else {
        return -1;
    }
}


#define BLOCK_SIZE 16
int aes_128_cbc_encrypt(kal_uint8 *key, kal_uint8 *iv, kal_uint8 *data,
                        kal_uint16 data_len)
{
    STCHE che_context;
    kal_uint8 *source = NULL;

    che_init(&che_context, CHE_AES);
    che_key_action(&che_context, CHE_SET_KEY,  key, 16);
    che_set_iv(&che_context, iv, BLOCK_SIZE);
    source = SUPC_ALLOC_BUFF(data_len);
    if (source == NULL) {
        SUPC_ASSERT(0);
    }
    SUPC_ASSERT(((kal_uint32) source & 0x03) == 0);

    kal_mem_cpy(source, data, data_len);
    che_process(&che_context, CHE_AES, CHE_CBC, CHE_ENC,  source, data, data_len, KAL_TRUE);
    SUPC_DEALLOC_BUFF(source);
    che_deinit(&che_context);
    return 0;
}


int aes_128_cbc_decrypt(kal_uint8 *key, kal_uint8 *iv, kal_uint8 *data,
                        kal_uint16 data_len)
{
    STCHE che_context;
    kal_uint8 *source = NULL;

    che_init(&che_context, CHE_AES);
    che_key_action(&che_context, CHE_SET_KEY,  key, 16);
    che_set_iv(&che_context, iv, BLOCK_SIZE);
    source = SUPC_ALLOC_BUFF(data_len);
    if (source == NULL) {
        SUPC_ASSERT(0);
    }
    SUPC_ASSERT(((kal_uint32) source & 0x03) == 0);

    kal_mem_cpy(source, data, data_len);
    che_process(&che_context, CHE_AES, CHE_CBC, CHE_DEC,  source, data, data_len, KAL_TRUE);
    SUPC_DEALLOC_BUFF(source);
    che_deinit(&che_context);
    return 0;
}
