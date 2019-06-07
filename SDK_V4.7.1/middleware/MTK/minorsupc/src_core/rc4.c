/**
 * Copyright (c) 2002-2004, Jouni Malinen <jkmaline@cc.hut.fi>
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
 * Host AP (software wireless LAN access point) user space daemon for
 * Host AP kernel driver / RC4
 * Copyright (c) 2002-2004, Jouni Malinen <jkmaline@cc.hut.fi>
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

//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"
#endif
#include "supc_common.h"
#include "che_api.h"

#define S_SWAP(a,b) do { u8 t = S[a]; S[a] = S[b]; S[b] = t; } while(0)
#define che_key_action(che_context, key_act, key, key_len)                 \
                                                                               \
       ((void)TRICK_##key_act, run_##key_act##_process(che_context, key, key_len))
#define che_process(che_context, type, mode, act, source, dest, source_length, final)                       \
                                                                                                                     \
       ((void)TRICK_##type, run_##type##_process(che_context, type, mode, act, source, dest, source_length, final, 0))

void rc4_skip(kal_uint8 *key, kal_uint16 key_len, wpa_size_t skip, kal_uint8 *data, kal_uint16 data_len)
{
    STCHE che_context;

    che_init(&che_context, CHE_RC4);
    che_key_action(&che_context, CHE_SET_KEY, (kal_uint8 *) key, key_len);
    che_process(&che_context, CHE_RC4, CHE_SKIP, CHE_DEC,  data, data, data_len, KAL_TRUE);
    che_deinit(&che_context);
}

void rc4_skip_solid(const u8 *key, size_t keylen, size_t skip,
                    u8 *data, size_t data_len)
{
    u32 i, j, k;
    u8 S[256], *pos;
    size_t kpos;

    /* Setup RC4 state */
    for (i = 0; i < 256; i++) {
        S[i] = i;
    }
    j = 0;
    kpos = 0;
    for (i = 0; i < 256; i++) {
        j = (j + S[i] + key[kpos]) & 0xff;
        kpos++;
        if (kpos >= keylen) {
            kpos = 0;
        }
        S_SWAP(i, j);
    }

    /* Skip the start of the stream */
    i = j = 0;
    for (k = 0; k < skip; k++) {
        i = (i + 1) & 0xff;
        j = (j + S[i]) & 0xff;
        S_SWAP(i, j);
    }

    /* Apply RC4 to data */
    pos = data;
    for (k = 0; k < data_len; k++) {
        i = (i + 1) & 0xff;
        j = (j + S[i]) & 0xff;
        S_SWAP(i, j);
        *pos++ ^= S[(S[i] + S[j]) & 0xff];
    }
}

void rc4(kal_uint8 *data, kal_uint16 data_len, kal_uint8 *key, kal_uint16 key_len)
{
#if 0
    STCHE che_context;

    che_init(&che_context, CHE_RC4);
    che_key_action(&che_context, CHE_SET_KEY, key, key_len);
    che_process(&che_context, CHE_RC4, CHE_MODE_NULL, CHE_DEC, data, data, data_len, KAL_TRUE);
    che_deinit(&che_context);
#endif
    rc4_skip_solid(key, key_len, 0, data, data_len);
}
