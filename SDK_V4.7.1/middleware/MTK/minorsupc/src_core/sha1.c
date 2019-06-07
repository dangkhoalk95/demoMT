/**
 * Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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
 * SHA1 hash implementation and interface functions
 * Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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
#include "supc_sha1.h"
#include "supc_md5.h"
#include "che_api.h"
//#include "ssl_api.h"
//#include "supc_ssl.h"
void hmac_sha1_vector(kal_uint8 *key, kal_uint16 key_len, kal_uint16 num_elem,
                      kal_uint8 *msg[], kal_uint16 *msg_len, kal_uint8 *digest)
{
    STCHE che_context;
    int i;

    che_init(&che_context, CHE_MAC_SHA1);
    che_key_action(&che_context, CHE_SET_KEY,  key, key_len);

    for (i = 0; i < num_elem; i++) {
        if (i == num_elem - 1) {
            che_process(&che_context, CHE_MAC_SHA1, CHE_MODE_NULL, CHE_HASH,  msg[i], digest, msg_len[i], KAL_TRUE);
        } else {
            che_process(&che_context, CHE_MAC_SHA1, CHE_MODE_NULL, CHE_HASH,  msg[i], digest, msg_len[i], KAL_FALSE);
        }
    }
    che_deinit(&che_context);
}

void hmac_sha1(kal_uint8 *key, kal_uint16 key_len, kal_uint8 *data, kal_uint16 data_len,
               kal_uint8 *mac)
{
    hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}


void sha1_prf(kal_uint8 *key, kal_uint16 key_len, const char *label,
              kal_uint8 *data, kal_uint16 data_len, kal_uint8 *buf, kal_uint16 buf_len)
{
    kal_uint8 zero = 0, counter = 0;
    kal_uint16 pos, plen;
    kal_uint8 hash[SHA1_MAC_LEN];
    kal_uint16 label_len = strlen(label);
    unsigned char *addr[4];
    kal_uint16 len[4];

    addr[0] = (u8 *) label;
    len[0] = label_len;
    addr[1] = &zero;
    len[1] = 1;
    addr[2] = data;
    len[2] = data_len;
    addr[3] = &counter;
    len[3] = 1;

    pos = 0;
    while (pos < buf_len) {
        plen = buf_len - pos;
        if (plen >= SHA1_MAC_LEN) {
            hmac_sha1_vector(key, key_len, 4, addr, len,
                             &buf[pos]);
            pos += SHA1_MAC_LEN;
        } else {
            hmac_sha1_vector(key, key_len, 4, addr, len,
                             hash);
            kal_mem_cpy(&buf[pos], hash, plen);
            break;
        }
        counter++;
    }
}


/* draft-cam-winget-eap-fast-00.txt */
void sha1_t_prf(kal_uint8 *key, kal_uint16 key_len, const char *label,
                kal_uint8 *seed, kal_uint16 seed_len, kal_uint8 *buf, kal_uint16 buf_len)
{
    unsigned char counter = 0;
    kal_uint16 pos, plen;
    kal_uint8 hash[SHA1_MAC_LEN];
    kal_uint16 label_len = strlen(label);
    kal_uint8 output_len[2];
    unsigned char *addr[5];
    kal_uint16 len[5];

    addr[0] = hash;
    len[0] = 0;
    addr[1] = (unsigned char *) label;
    len[1] = label_len + 1;
    addr[2] = seed;
    len[2] = seed_len;
    addr[3] = output_len;
    len[3] = 2;
    addr[4] = &counter;
    len[4] = 1;

    output_len[0] = (buf_len >> 8) & 0xff;
    output_len[1] = buf_len & 0xff;
    pos = 0;
    while (pos < buf_len) {
        counter++;
        plen = buf_len - pos;
        hmac_sha1_vector(key, key_len, 5, addr, len, hash);
        if (plen >= SHA1_MAC_LEN) {
            memcpy(&buf[pos], hash, SHA1_MAC_LEN);
            pos += SHA1_MAC_LEN;
        } else {
            memcpy(&buf[pos], hash, plen);
            break;
        }
        len[0] = SHA1_MAC_LEN;
    }
}


/* RFC 2246 */
int tls_prf(kal_uint8 *secret, kal_uint16 secret_len, const char *label,
            kal_uint8 *seed, kal_uint16 seed_len, kal_uint8 *out, kal_uint16 outlen)
{
    kal_uint16 L_S1, L_S2;
    kal_uint8 *S1, *S2;
    kal_uint8 A_MD5[MD5_MAC_LEN], A_SHA1[SHA1_MAC_LEN];
    kal_uint8 P_MD5[MD5_MAC_LEN], P_SHA1[SHA1_MAC_LEN];
    int i, MD5_pos, SHA1_pos;
    kal_uint8 *MD5_addr[3];
    kal_uint16 MD5_len[3];
    unsigned char *SHA1_addr[3];
    kal_uint16 SHA1_len[3];

    if (secret_len & 1) {
        return -1;
    }

    MD5_addr[0] = A_MD5;
    MD5_len[0] = MD5_MAC_LEN;
    MD5_addr[1] = (unsigned char *) label;
    MD5_len[1] = strlen(label);
    MD5_addr[2] = seed;
    MD5_len[2] = seed_len;

    SHA1_addr[0] = A_SHA1;
    SHA1_len[0] = SHA1_MAC_LEN;
    SHA1_addr[1] = (unsigned char *) label;
    SHA1_len[1] = strlen(label);
    SHA1_addr[2] = seed;
    SHA1_len[2] = seed_len;

    /* RFC 2246, Chapter 5
     * A(0) = seed, A(i) = HMAC(secret, A(i-1))
     * P_hash = HMAC(secret, A(1) + seed) + HMAC(secret, A(2) + seed) + ..
     * PRF = P_MD5(S1, label + seed) XOR P_SHA-1(S2, label + seed)
     */

    L_S1 = L_S2 = (secret_len + 1) / 2;
    S1 = secret;
    S2 = secret + L_S1;

    hmac_md5_vector(S1, L_S1, 2, &MD5_addr[1], &MD5_len[1], A_MD5);
    hmac_sha1_vector(S2, L_S2, 2, &SHA1_addr[1], &SHA1_len[1], A_SHA1);

    MD5_pos = MD5_MAC_LEN;
    SHA1_pos = SHA1_MAC_LEN;
    for (i = 0; i < outlen; i++) {
        if (MD5_pos == MD5_MAC_LEN) {
            hmac_md5_vector(S1, L_S1, 3, MD5_addr, MD5_len, P_MD5);
            MD5_pos = 0;
            hmac_md5(S1, L_S1, A_MD5, MD5_MAC_LEN, A_MD5);
        }
        if (SHA1_pos == SHA1_MAC_LEN) {
            hmac_sha1_vector(S2, L_S2, 3, SHA1_addr, SHA1_len,
                             P_SHA1);
            SHA1_pos = 0;
            hmac_sha1(S2, L_S2, A_SHA1, SHA1_MAC_LEN, A_SHA1);
        }

        out[i] = P_MD5[MD5_pos] ^ P_SHA1[SHA1_pos];

        MD5_pos++;
        SHA1_pos++;
    }

    return 0;
}

static void pbkdf2_sha1_f(const char *passphrase, const char *ssid,
                          kal_uint16 ssid_len, int iterations, int count,
                          kal_uint8 *digest)
{
    unsigned char tmp[SHA1_MAC_LEN], tmp2[SHA1_MAC_LEN];
    int i, j;
    unsigned char count_buf[4];
    kal_uint8 *addr[2];
    kal_uint16 len[2];
    kal_uint16 passphrase_len = strlen(passphrase);

    addr[0] = (u8 *) ssid;
    len[0] = ssid_len;
    addr[1] = count_buf;
    len[1] = 4;

    /* F(P, S, c, i) = U1 xor U2 xor ... Uc
     * U1 = PRF(P, S || i)
     * U2 = PRF(P, U1)
     * Uc = PRF(P, Uc-1)
     */

    count_buf[0] = (count >> 24) & 0xff;
    count_buf[1] = (count >> 16) & 0xff;
    count_buf[2] = (count >> 8) & 0xff;
    count_buf[3] = count & 0xff;
    hmac_sha1_vector((u8 *) passphrase, passphrase_len, 2, addr, len, tmp);
    memcpy(digest, tmp, SHA1_MAC_LEN);

    for (i = 1; i < iterations; i++) {
        hmac_sha1((u8 *) passphrase, passphrase_len, tmp, SHA1_MAC_LEN,
                  tmp2);
        memcpy(tmp, tmp2, SHA1_MAC_LEN);
        for (j = 0; j < SHA1_MAC_LEN; j++) {
            digest[j] ^= tmp2[j];
        }
    }
}

void pbkdf2_sha1(const char *passphrase, const char *ssid, kal_uint16 ssid_len,
                 int iterations, kal_uint8 *buf, kal_uint16 buflen)
{
    int count = 0;
    unsigned char *pos = buf;
    kal_uint16 left = buflen, plen;
    unsigned char digest[SHA1_MAC_LEN];

    while (left > 0) {
        count++;
        pbkdf2_sha1_f(passphrase, ssid, ssid_len, iterations, count,
                      digest);
        plen = left > SHA1_MAC_LEN ? SHA1_MAC_LEN : left;
        memcpy(pos, digest, plen);
        pos += plen;
        left -= plen;
    }
}

void sha1_transform(kal_uint8 *state, kal_uint8 data[64])
{
#ifdef EAP_TLS_FUNCS
    SHA_CTX context;
    memset(&context, 0, sizeof(context));
    memcpy(&context.h0, state, 5 * 4);
    SHA1_Transform(&context, data);
    memcpy(state, &context.h0, 5 * 4);
#else /* EAP_TLS_FUNCS */
    SHA1Transform((u32 *) state, data);
#endif /* EAP_TLS_FUNCS */
}

void sha1_vector(kal_uint16 num_elem, kal_uint8 *msg[], kal_uint16 *msg_len,
                 kal_uint8 *digest)
{
    STCHE che_context;
    int i;

    che_init(&che_context, CHE_SHA1);

    for (i = 0; i < num_elem; i++) {
        if (i == num_elem - 1) {
            che_process(&che_context, CHE_SHA1, CHE_MODE_NULL, CHE_HASH, (kal_uint8 *) msg[i], digest, msg_len[i], KAL_TRUE);
        } else {
            che_process(&che_context, CHE_SHA1, CHE_MODE_NULL, CHE_HASH, (kal_uint8 *) msg[i], digest, msg_len[i], KAL_FALSE);
        }
    }
    che_deinit(&che_context);
}

void sha1(kal_uint8 *msg, kal_uint16 msg_len, kal_uint8 *messageDigest)
{
    kal_uint8 *data[1];
    kal_uint16 len[1];

    data[0] = msg;
    len[0] = msg_len;
    sha1_vector(1, data, len, messageDigest);
}

#ifndef EAP_TLS_FUNCS

/* ===== start - public domain SHA1 implementation ===== */

/*
SHA-1 in C
By Steve Reid <sreid@sea-to-sky.net>
100% Public Domain

-----------------
Modified 7/98
By James H. Brown <jbrown@burgoyne.com>
Still 100% Public Domain

Corrected a problem which generated improper hash values on 16 bit machines
Routine SHA1Update changed from
    void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int
len)
to
    void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned
long len)

The 'len' parameter was declared an int which works fine on 32 bit machines.
However, on 16 bit machines an int is too small for the shifts being done
against
it.  This caused the hash function to generate incorrect values if len was
greater than 8191 (8K - 1) due to the 'len << 3' on line 3 of SHA1Update().

Since the file IO in main() reads 16K at a time, any file 8K or larger would
be guaranteed to generate the wrong hash (e.g. Test Vector #3, a million
"a"s).

I also changed the declaration of variables i & j in SHA1Update to
unsigned long from unsigned int for the same reason.

These changes should make no difference to any 32 bit implementations since
an
int and a long are the same size in those environments.

--
I also corrected a few compiler warnings generated by Borland C.
1. Added #include <process.h> for exit() prototype
2. Removed unused variable 'j' in SHA1Final
3. Changed exit(0) to return(0) at end of main.

ALL changes I made can be located by searching for comments containing 'JHB'
-----------------
Modified 8/98
By Steve Reid <sreid@sea-to-sky.net>
Still 100% public domain

1- Removed #include <process.h> and used return() instead of exit()
2- Fixed overwriting of finalcount in SHA1Final() (discovered by Chris Hall)
3- Changed email address from steve@edmweb.com to sreid@sea-to-sky.net

-----------------
Modified 4/01
By Saul Kravitz <Saul.Kravitz@celera.com>
Still 100% PD
Modified to run on Compaq Alpha hardware.

-----------------
Modified 4/01
By Jouni Malinen <jkmaline@cc.hut.fi>
Minor changes to match the coding style used in Dynamics.

Modified September 24, 2004
By Jouni Malinen <jkmaline@cc.hut.fi>
Fixed alignment issue in SHA1Transform when SHA1HANDSOFF is defined.

*/

/*
Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#define SHA1HANDSOFF

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#ifndef WORDS_BIGENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) | \
    (rol(block->l[i], 8) & 0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i & 15] = rol(block->l[(i + 13) & 15] ^ \
    block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15], 1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#ifdef R0
#undef R0
#endif
#define R0(v,w,x,y,z,i) \
    z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5A827999 + rol(v, 5); \
    w = rol(w, 30);
#ifdef R1
#undef R1
#endif
#define R1(v,w,x,y,z,i) \
    z += ((w & (x ^ y)) ^ y) + blk(i) + 0x5A827999 + rol(v, 5); \
    w = rol(w, 30);

#ifdef R2
#undef R2
#endif
#define R2(v,w,x,y,z,i) \
    z += (w ^ x ^ y) + blk(i) + 0x6ED9EBA1 + rol(v, 5); w = rol(w, 30);

#ifdef R3
#undef R3
#endif
#define R3(v,w,x,y,z,i) \
    z += (((w | x) & y) | (w & x)) + blk(i) + 0x8F1BBCDC + rol(v, 5); \
    w = rol(w, 30);

#ifdef R4
#undef R4
#endif
#define R4(v,w,x,y,z,i) \
    z += (w ^ x ^ y) + blk(i) + 0xCA62C1D6 + rol(v, 5); \
    w=rol(w, 30);

/* Hash a single 512-bit block. This is the core of the algorithm. */

void SHA1Transform(u32 state[5], const unsigned char buffer[64])
{
    u32 a, b, c, d, e;
    typedef union {
        unsigned char c[64];
        u32 l[16];
    } CHAR64LONG16;
    CHAR64LONG16 *block;
#ifdef SHA1HANDSOFF
    u32 workspace[16];
    block = (CHAR64LONG16 *) workspace;
    memcpy(block, buffer, 64);
#else
    block = (CHAR64LONG16 *) buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);
    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
    memset(block, 0, 64);
#endif
}

#endif /* EAP_TLS_FUNCS */


#ifdef TEST_MAIN

#include "md5.c"

static int test_eap_fast(void)
{
    /* draft-cam-winget-eap-fast-01.txt */
    const u8 pac_key[] = {
        0x0B, 0x97, 0x39, 0x0F, 0x37, 0x51, 0x78, 0x09,
        0x81, 0x1E, 0xFD, 0x9C, 0x6E, 0x65, 0x94, 0x2B,
        0x63, 0x2C, 0xE9, 0x53, 0x89, 0x38, 0x08, 0xBA,
        0x36, 0x0B, 0x03, 0x7C, 0xD1, 0x85, 0xE4, 0x14
    };
    const u8 seed[] = {
        0x3F, 0xFB, 0x11, 0xC4, 0x6C, 0xBF, 0xA5, 0x7A,
        0x54, 0x40, 0xDA, 0xE8, 0x22, 0xD3, 0x11, 0xD3,
        0xF7, 0x6D, 0xE4, 0x1D, 0xD9, 0x33, 0xE5, 0x93,
        0x70, 0x97, 0xEB, 0xA9, 0xB3, 0x66, 0xF4, 0x2A,
        0x00, 0x00, 0x00, 0x02, 0x6A, 0x66, 0x43, 0x2A,
        0x8D, 0x14, 0x43, 0x2C, 0xEC, 0x58, 0x2D, 0x2F,
        0xC7, 0x9C, 0x33, 0x64, 0xBA, 0x04, 0xAD, 0x3A,
        0x52, 0x54, 0xD6, 0xA5, 0x79, 0xAD, 0x1E, 0x00
    };
    const u8 master_secret[] = {
        0x4A, 0x1A, 0x51, 0x2C, 0x01, 0x60, 0xBC, 0x02,
        0x3C, 0xCF, 0xBC, 0x83, 0x3F, 0x03, 0xBC, 0x64,
        0x88, 0xC1, 0x31, 0x2F, 0x0B, 0xA9, 0xA2, 0x77,
        0x16, 0xA8, 0xD8, 0xE8, 0xBD, 0xC9, 0xD2, 0x29,
        0x38, 0x4B, 0x7A, 0x85, 0xBE, 0x16, 0x4D, 0x27,
        0x33, 0xD5, 0x24, 0x79, 0x87, 0xB1, 0xC5, 0xA2
    };
    const u8 key_block[] = {
        0x59, 0x59, 0xBE, 0x8E, 0x41, 0x3A, 0x77, 0x74,
        0x8B, 0xB2, 0xE5, 0xD3, 0x60, 0xAC, 0x4D, 0x35,
        0xDF, 0xFB, 0xC8, 0x1E, 0x9C, 0x24, 0x9C, 0x8B,
        0x0E, 0xC3, 0x1D, 0x72, 0xC8, 0x84, 0x9D, 0x57,
        0x48, 0x51, 0x2E, 0x45, 0x97, 0x6C, 0x88, 0x70,
        0xBE, 0x5F, 0x01, 0xD3, 0x64, 0xE7, 0x4C, 0xBB,
        0x11, 0x24, 0xE3, 0x49, 0xE2, 0x3B, 0xCD, 0xEF,
        0x7A, 0xB3, 0x05, 0x39, 0x5D, 0x64, 0x8A, 0x44,
        0x11, 0xB6, 0x69, 0x88, 0x34, 0x2E, 0x8E, 0x29,
        0xD6, 0x4B, 0x7D, 0x72, 0x17, 0x59, 0x28, 0x05,
        0xAF, 0xF9, 0xB7, 0xFF, 0x66, 0x6D, 0xA1, 0x96,
        0x8F, 0x0B, 0x5E, 0x06, 0x46, 0x7A, 0x44, 0x84,
        0x64, 0xC1, 0xC8, 0x0C, 0x96, 0x44, 0x09, 0x98,
        0xFF, 0x92, 0xA8, 0xB4, 0xC6, 0x42, 0x28, 0x71
    };
    const u8 sks[] = {
        0xD6, 0x4B, 0x7D, 0x72, 0x17, 0x59, 0x28, 0x05,
        0xAF, 0xF9, 0xB7, 0xFF, 0x66, 0x6D, 0xA1, 0x96,
        0x8F, 0x0B, 0x5E, 0x06, 0x46, 0x7A, 0x44, 0x84,
        0x64, 0xC1, 0xC8, 0x0C, 0x96, 0x44, 0x09, 0x98,
        0xFF, 0x92, 0xA8, 0xB4, 0xC6, 0x42, 0x28, 0x71
    };
    const u8 isk[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    const u8 imck[] = {
        0x16, 0x15, 0x3C, 0x3F, 0x21, 0x55, 0xEF, 0xD9,
        0x7F, 0x34, 0xAE, 0xC8, 0x1A, 0x4E, 0x66, 0x80,
        0x4C, 0xC3, 0x76, 0xF2, 0x8A, 0xA9, 0x6F, 0x96,
        0xC2, 0x54, 0x5F, 0x8C, 0xAB, 0x65, 0x02, 0xE1,
        0x18, 0x40, 0x7B, 0x56, 0xBE, 0xEA, 0xA7, 0xC5,
        0x76, 0x5D, 0x8F, 0x0B, 0xC5, 0x07, 0xC6, 0xB9,
        0x04, 0xD0, 0x69, 0x56, 0x72, 0x8B, 0x6B, 0xB8,
        0x15, 0xEC, 0x57, 0x7B
    };
    const u8 msk[] = {
        0x4D, 0x83, 0xA9, 0xBE, 0x6F, 0x8A, 0x74, 0xED,
        0x6A, 0x02, 0x66, 0x0A, 0x63, 0x4D, 0x2C, 0x33,
        0xC2, 0xDA, 0x60, 0x15, 0xC6, 0x37, 0x04, 0x51,
        0x90, 0x38, 0x63, 0xDA, 0x54, 0x3E, 0x14, 0xB9,
        0x27, 0x99, 0x18, 0x1E, 0x07, 0xBF, 0x0F, 0x5A,
        0x5E, 0x3C, 0x32, 0x93, 0x80, 0x8C, 0x6C, 0x49,
        0x67, 0xED, 0x24, 0xFE, 0x45, 0x40, 0xA0, 0x59,
        0x5E, 0x37, 0xC2, 0xE9, 0xD0, 0x5D, 0x0A, 0xE3
    };
    u8 tlv[] = {
        0x80, 0x0C, 0x00, 0x38, 0x00, 0x01, 0x01, 0x00,
        0xD8, 0x6A, 0x8C, 0x68, 0x3C, 0x32, 0x31, 0xA8,
        0x56, 0x63, 0xB6, 0x40, 0x21, 0xFE, 0x21, 0x14,
        0x4E, 0xE7, 0x54, 0x20, 0x79, 0x2D, 0x42, 0x62,
        0xC9, 0xBF, 0x53, 0x7F, 0x54, 0xFD, 0xAC, 0x58,
        0x43, 0x24, 0x6E, 0x30, 0x92, 0x17, 0x6D, 0xCF,
        0xE6, 0xE0, 0x69, 0xEB, 0x33, 0x61, 0x6A, 0xCC,
        0x05, 0xC5, 0x5B, 0xB7
    };
    const u8 compound_mac[] = {
        0x43, 0x24, 0x6E, 0x30, 0x92, 0x17, 0x6D, 0xCF,
        0xE6, 0xE0, 0x69, 0xEB, 0x33, 0x61, 0x6A, 0xCC,
        0x05, 0xC5, 0x5B, 0xB7
    };
    u8 buf[512];
    const u8 *simck, *cmk;
    int errors = 0;

    wpa_printf(MSG_DEBUG, "EAP-FAST test cases\n");

    wpa_printf(MSG_DEBUG, "- T-PRF (SHA1) test case / master_secret\n");
    sha1_t_prf(pac_key, sizeof(pac_key), "PAC to master secret label hash",
               seed, sizeof(seed), buf, sizeof(master_secret));
    if (memcmp(master_secret, buf, sizeof(master_secret)) != 0) {
        wpa_printf(MSG_DEBUG, "T-PRF test - FAILED!\n");
        errors++;
    }

    wpa_printf(MSG_DEBUG, "- PRF (TLS, SHA1/MD5) test case / key_block\n");
    tls_prf(master_secret, sizeof(master_secret), "key expansion",
            seed, sizeof(seed), buf, sizeof(key_block));
    if (memcmp(key_block, buf, sizeof(key_block)) != 0) {
        wpa_printf(MSG_DEBUG, "PRF test - FAILED!\n");
        errors++;
    }

    wpa_printf(MSG_DEBUG, "- T-PRF (SHA1) test case / IMCK\n");
    sha1_t_prf(sks, sizeof(sks), "Inner Methods Compound Keys",
               isk, sizeof(isk), buf, sizeof(imck));
    if (memcmp(imck, buf, sizeof(imck)) != 0) {
        wpa_printf(MSG_DEBUG, "T-PRF test - FAILED!\n");
        errors++;
    }

    simck = imck;
    cmk = imck + 40;

    wpa_printf(MSG_DEBUG, "- T-PRF (SHA1) test case / MSK\n");
    sha1_t_prf(simck, 40, "Session Key Generating Function",
               "", 0, buf, sizeof(msk));
    if (memcmp(msk, buf, sizeof(msk)) != 0) {
        wpa_printf(MSG_DEBUG, "T-PRF test - FAILED!\n");
        errors++;
    }

    wpa_printf(MSG_DEBUG, "- Compound MAC test case\n");
    memset(tlv + sizeof(tlv) - 20, 0, 20);
    hmac_sha1(cmk, 20, tlv, sizeof(tlv), tlv + sizeof(tlv) - 20);
    if (memcmp(tlv + sizeof(tlv) - 20, compound_mac, sizeof(compound_mac))
            != 0) {
        wpa_printf(MSG_DEBUG, "Compound MAC test - FAILED!\n");
        errors++;
    }

    return errors;
}


static u8 key0[] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b
};
static u8 data0[] = "Hi There";
static u8 prf0[] = {
    0xbc, 0xd4, 0xc6, 0x50, 0xb3, 0x0b, 0x96, 0x84,
    0x95, 0x18, 0x29, 0xe0, 0xd7, 0x5f, 0x9d, 0x54,
    0xb8, 0x62, 0x17, 0x5e, 0xd9, 0xf0, 0x06, 0x06,
    0xe1, 0x7d, 0x8d, 0xa3, 0x54, 0x02, 0xff, 0xee,
    0x75, 0xdf, 0x78, 0xc3, 0xd3, 0x1e, 0x0f, 0x88,
    0x9f, 0x01, 0x21, 0x20, 0xc0, 0x86, 0x2b, 0xeb,
    0x67, 0x75, 0x3e, 0x74, 0x39, 0xae, 0x24, 0x2e,
    0xdb, 0x83, 0x73, 0x69, 0x83, 0x56, 0xcf, 0x5a
};

static u8 key1[] = "Jefe";
static u8 data1[] = "what do ya want for nothing?";
static u8 prf1[] = {
    0x51, 0xf4, 0xde, 0x5b, 0x33, 0xf2, 0x49, 0xad,
    0xf8, 0x1a, 0xeb, 0x71, 0x3a, 0x3c, 0x20, 0xf4,
    0xfe, 0x63, 0x14, 0x46, 0xfa, 0xbd, 0xfa, 0x58,
    0x24, 0x47, 0x59, 0xae, 0x58, 0xef, 0x90, 0x09,
    0xa9, 0x9a, 0xbf, 0x4e, 0xac, 0x2c, 0xa5, 0xfa,
    0x87, 0xe6, 0x92, 0xc4, 0x40, 0xeb, 0x40, 0x02,
    0x3e, 0x7b, 0xab, 0xb2, 0x06, 0xd6, 0x1d, 0xe7,
    0xb9, 0x2f, 0x41, 0x52, 0x90, 0x92, 0xb8, 0xfc
};


static u8 key2[] = {
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa
};
static u8 data2[] = {
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd
};
static u8 prf2[] = {
    0xe1, 0xac, 0x54, 0x6e, 0xc4, 0xcb, 0x63, 0x6f,
    0x99, 0x76, 0x48, 0x7b, 0xe5, 0xc8, 0x6b, 0xe1,
    0x7a, 0x02, 0x52, 0xca, 0x5d, 0x8d, 0x8d, 0xf1,
    0x2c, 0xfb, 0x04, 0x73, 0x52, 0x52, 0x49, 0xce,
    0x9d, 0xd8, 0xd1, 0x77, 0xea, 0xd7, 0x10, 0xbc,
    0x9b, 0x59, 0x05, 0x47, 0x23, 0x91, 0x07, 0xae,
    0xf7, 0xb4, 0xab, 0xd4, 0x3d, 0x87, 0xf0, 0xa6,
    0x8f, 0x1c, 0xbd, 0x9e, 0x2b, 0x6f, 0x76, 0x07
};


struct passphrase_test {
    char *passphrase;
    char *ssid;
    char psk[32];
};

static struct passphrase_test passphrase_tests[] = {
    {
        "password",
        "IEEE",
        {
            0xf4, 0x2c, 0x6f, 0xc5, 0x2d, 0xf0, 0xeb, 0xef,
            0x9e, 0xbb, 0x4b, 0x90, 0xb3, 0x8a, 0x5f, 0x90,
            0x2e, 0x83, 0xfe, 0x1b, 0x13, 0x5a, 0x70, 0xe2,
            0x3a, 0xed, 0x76, 0x2e, 0x97, 0x10, 0xa1, 0x2e
        }
    },
    {
        "ThisIsAPassword",
        "ThisIsASSID",
        {
            0x0d, 0xc0, 0xd6, 0xeb, 0x90, 0x55, 0x5e, 0xd6,
            0x41, 0x97, 0x56, 0xb9, 0xa1, 0x5e, 0xc3, 0xe3,
            0x20, 0x9b, 0x63, 0xdf, 0x70, 0x7d, 0xd5, 0x08,
            0xd1, 0x45, 0x81, 0xf8, 0x98, 0x27, 0x21, 0xaf
        }
    },
    {
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
        {
            0xbe, 0xcb, 0x93, 0x86, 0x6b, 0xb8, 0xc3, 0x83,
            0x2c, 0xb7, 0x77, 0xc2, 0xf5, 0x59, 0x80, 0x7c,
            0x8c, 0x59, 0xaf, 0xcb, 0x6e, 0xae, 0x73, 0x48,
            0x85, 0x00, 0x13, 0x00, 0xa9, 0x81, 0xcc, 0x62
        }
    },
};

#define NUM_PASSPHRASE_TESTS \
(sizeof(passphrase_tests) / sizeof(passphrase_tests[0]))


int main(int argc, char *argv[])
{
    u8 res[512];
    int ret = 0, i;

    wpa_printf(MSG_DEBUG, "PRF-SHA1 test cases:\n");

    sha1_prf(key0, sizeof(key0), "prefix", data0, sizeof(data0) - 1,
             res, sizeof(prf0));
    if (memcmp(res, prf0, sizeof(prf0)) == 0) {
        wpa_printf(MSG_DEBUG, "Test case 0 - OK\n");
    } else {
        wpa_printf(MSG_DEBUG, "Test case 0 - FAILED!\n");
        ret++;
    }

    sha1_prf(key1, sizeof(key1) - 1, "prefix", data1, sizeof(data1) - 1,
             res, sizeof(prf1));
    if (memcmp(res, prf1, sizeof(prf1)) == 0) {
        wpa_printf(MSG_DEBUG, "Test case 1 - OK\n");
    } else {
        wpa_printf(MSG_DEBUG, "Test case 1 - FAILED!\n");
        ret++;
    }

    sha1_prf(key2, sizeof(key2), "prefix", data2, sizeof(data2),
             res, sizeof(prf2));
    if (memcmp(res, prf2, sizeof(prf2)) == 0) {
        wpa_printf(MSG_DEBUG, "Test case 2 - OK\n");
    } else {
        wpa_printf(MSG_DEBUG, "Test case 2 - FAILED!\n");
        ret++;
    }

    ret += test_eap_fast();

    wpa_printf(MSG_DEBUG, "PBKDF2-SHA1 Passphrase test cases:\n");
    for (i = 0; i < NUM_PASSPHRASE_TESTS; i++) {
        u8 psk[32];
        struct passphrase_test *test = &passphrase_tests[i];
        pbkdf2_sha1(test->passphrase,
                    test->ssid, strlen(test->ssid),
                    4096, psk, 32);
        if (memcmp(psk, test->psk, 32) == 0) {
            wpa_printf(MSG_DEBUG, "Test case %d - OK\n", i);
        } else {
            wpa_printf(MSG_DEBUG, "Test case %d - FAILED!\n", i);
            ret++;
        }
    }

    return ret;
}
#endif /* TEST_MAIN */
