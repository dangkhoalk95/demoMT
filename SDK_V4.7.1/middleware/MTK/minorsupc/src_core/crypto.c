/**
 * Copyright (c) 2004-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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

/*
 * WPA Supplicant / wrapper functions for libcrypto
 * Copyright (c) 2004-2005, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif

//#include "kal_release.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

#include "che_api.h"
#if 0


void md4(kal_uint8 *msg, kal_uint16 msg_len, kal_uint8 *digest)
{

    STCHE che_context;

    che_init(&che_context, CHE_MD4);
    che_process(&che_context, CHE_MD4, CHE_MODE_NULL, CHE_HASH,  msg, digest, msg_len, KAL_TRUE);
    che_deinit(&che_context);

}


/**
 * @clear: 8 octets (in)
 * @key: 7 octets (in) (no parity bits included)
 * @cypher: 8 octets (out)
 */

void des_encrypt(kal_uint8 *plaintext, kal_uint16 plaintext_len, kal_uint8 *key, kal_uint16 key_len, kal_uint8 *ciphertext)
{
    kal_uint8 pkey[8], next, tmp;
    STCHE che_context;
    int i;

    /* Add parity bits to the key */
    next = 0;
    for (i = 0; i < 7; i++) {
        tmp = key[i];
        pkey[i] = (tmp >> i) | next | 1;
        next = tmp << (7 - i);
    }
    pkey[i] = next | 1;
#if 0
    mark for not used yet
    che_init(&che_context, CHE_DES);
    che_key_action(&che_context, CHE_SET_KEY,  pkey, key_len);
    che_process(&che_context, CHE_DES, CHE_ECB, CHE_ENC,  plaintext, ciphertext, plaintext_len, KAL_TRUE);
    che_deinit(&che_context);
#endif
}
#endif

