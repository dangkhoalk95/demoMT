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

#include "che_api.h"
#include "supc_aes.h"
#include "supc_dhm.h"
#define S_SWAP(sa,sb) do { kal_uint8 t = sa; sa = sb; sb = t; } while(0)


#undef BYTE_ORDER   /* 1 = big-endian, -1 = little-endian, 0 = unknown */
#ifdef ARCH_IS_BIG_ENDIAN
#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
#else
#  define BYTE_ORDER 0
#endif

#define T_MASK ((md5_word_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)


static void CHE_md5_process(md5_context *pms, const md5_byte_t *data /*[64]*/)
{
    md5_word_t
    a = pms->abcd[0], b = pms->abcd[1],
    c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;
    /* Define storage for little-endian or both types of CPUs. */
    md5_word_t xbuf[16];
    const md5_word_t *X;

    {
#if BYTE_ORDER == 0
        /*
         * Determine dynamically whether this is a big-endian or
         * little-endian machine, since we can use a more efficient
         * algorithm on the latter.
         */
        static const int w = 1;

        if (*((const md5_byte_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0     /* little-endian */
        {
            /*
             * On little-endian machines, we can process properly aligned
             * data without copying it.
             */
            if (!((data - (const md5_byte_t *)0) & 3)) {
                /* data are properly aligned */
                X = (const md5_word_t *)data;
            } else {
                /* not aligned */
                memcpy(xbuf, data, 64);
                X = xbuf;
            }
        }
#endif
#if BYTE_ORDER == 0
        else            /* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0     /* big-endian */
        {
            /*
             * On big-endian machines, we must arrange the bytes in the
             * right order.
             */
            const md5_byte_t *xp = data;
            int i;

#  if BYTE_ORDER == 0
            X = xbuf;       /* (dynamic only) */
#  else
#    define xbuf X      /* (static only) */
#  endif
            for (i = 0; i < 16; ++i, xp += 4) {
                xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
            }
        }
#endif
    }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s i] denote the operation
       a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F_MD5(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + F_MD5(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  7,  T1);
    SET(d, a, b, c,  1, 12,  T2);
    SET(c, d, a, b,  2, 17,  T3);
    SET(b, c, d, a,  3, 22,  T4);
    SET(a, b, c, d,  4,  7,  T5);
    SET(d, a, b, c,  5, 12,  T6);
    SET(c, d, a, b,  6, 17,  T7);
    SET(b, c, d, a,  7, 22,  T8);
    SET(a, b, c, d,  8,  7,  T9);
    SET(d, a, b, c,  9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12,  7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET

    /* Round 2. */
    /* Let [abcd k s i] denote the operation
         a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + G(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  1,  5, T17);
    SET(d, a, b, c,  6,  9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a,  0, 20, T20);
    SET(a, b, c, d,  5,  5, T21);
    SET(d, a, b, c, 10,  9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a,  4, 20, T24);
    SET(a, b, c, d,  9,  5, T25);
    SET(d, a, b, c, 14,  9, T26);
    SET(c, d, a, b,  3, 14, T27);
    SET(b, c, d, a,  8, 20, T28);
    SET(a, b, c, d, 13,  5, T29);
    SET(d, a, b, c,  2,  9, T30);
    SET(c, d, a, b,  7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET

    /* Round 3. */
    /* Let [abcd k s t] denote the operation
         a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + H(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  5,  4, T33);
    SET(d, a, b, c,  8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d,  1,  4, T37);
    SET(d, a, b, c,  4, 11, T38);
    SET(c, d, a, b,  7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13,  4, T41);
    SET(d, a, b, c,  0, 11, T42);
    SET(c, d, a, b,  3, 16, T43);
    SET(b, c, d, a,  6, 23, T44);
    SET(a, b, c, d,  9,  4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a,  2, 23, T48);
#undef SET

    /* Round 4. */
    /* Let [abcd k s t] denote the operation
         a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + I(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  6, T49);
    SET(d, a, b, c,  7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a,  5, 21, T52);
    SET(a, b, c, d, 12,  6, T53);
    SET(d, a, b, c,  3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a,  1, 21, T56);
    SET(a, b, c, d,  8,  6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b,  6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d,  4,  6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b,  2, 15, T63);
    SET(b, c, d, a,  9, 21, T64);
#undef SET

    /* Then perform the following additions. (That is increment each
       of the four registers by the value it had before this block
       was started.) */
    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}



void CHE_md5_init(md5_context *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
    pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
    pms->abcd[3] = 0x10325476;
}



void CHE_md5_append(md5_context *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0) {
        return;
    }

    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits) {
        pms->count[1]++;
    }

    /* Process an initial partial block. */
    if (offset) {
        int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

        ASSERT(offset <= sizeof(pms->buf));

        memcpy(pms->buf + offset, p, copy);
        if (offset + copy < 64) {
            return;
        }
        p += copy;
        left -= copy;
        CHE_md5_process(pms, pms->buf);
    }

    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64) {
        CHE_md5_process(pms, p);
    }

    /* Process a final partial block. */
    if (left) {
        memcpy(pms->buf, p, left);
    }
}



void CHE_md5_finish(md5_context *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    md5_byte_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i) {
        data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    }
    /* Pad to 56 bytes mod 64. */
    CHE_md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    /* Append the length. */
    CHE_md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i) {
        digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
    }
}



static sha1_context canned_context;

/*****************************************************************************
 * FUNCTION
 *  type2sz
 * DESCRIPTION
 *
 * PARAMETERS
 *  type        [IN]
 * RETURNS
 *
 *****************************************************************************/
kal_uint32 type2sz(kal_uint32 type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (type) {
        case CHE_DES:
        case CHE_3DES:
            return sizeof(DES_CNXT);

        case CHE_AES:
            return sizeof(AES_CNXT);

        case CHE_MD5:
        case CHE_SHA1:
        case CHE_SHA256:
        case CHE_SHA224:
            return sizeof(MD5_CNXT);

        case CHE_MAC_MD5:
        case CHE_MAC_SHA1:
        case CHE_MAC_SHA224:
        case CHE_MAC_SHA256:
        case CHE_MD4:
            return sizeof(SW_MISC_CNXT);
        case CHE_RC4:
            return sizeof(RC4_CNXT);

        case CHE_RSA:
            return sizeof(RSA_CNXT);
        default:
            break;
    }
    return 0;
}

void CHE_sha1_Init(sha1_context_ptr context)
{
    int loopindex;

    if (!context) {
        context = &canned_context;
    }

    /* Set initial values */
    context->digest[0] = 0x67452301L;
    context->digest[1] = 0xEFCDAB89L;
    context->digest[2] = 0x98BADCFEL;
    context->digest[3] = 0x10325476L;
    context->digest[4] = 0xC3D2E1F0L;

    /* Initialize bit count */
    context->countLo = 0L;
    context->countHi = 0L;

    /* Initialize buffer to empty. */
    context->thisword = 0;
    context->thisbyte = 0;

    /* Zero out data */
    for (loopindex = 0; loopindex < SHF_BLOCKWORDSIZE; loopindex++) {
        context->data[loopindex] = 0;
    }

    /* What sort of SHA are we doing? */
    context->kind = 1;
}



void CHE_shaTransform(sha1_context_ptr context)
{
    /* Rotate Left n bits (32 bit) */
#define S_SHA(n,X) (((X)<<(n)) | ((X)>>(32-(n))))

    BITS32 W[80], temp;
    int i;

    /* A buffer of 5 32-bit words */
    BITS32 A, B, C, D, E;

#define DG   context->digest

    if (!context) {
        context = &canned_context;
    }

    /* Get digest */
    A = DG[0];
    B = DG[1];
    C = DG[2];
    D = DG[3];
    E = DG[4];

    /* Copy the data buffer into the work buffer */
    for (i = 0; i < SHF_BLOCKWORDSIZE; i++) {
        W[i] = context->data[i];
        context->data[i] = 0;
    }

    /* Expand the 16 words into the SHF_BLOCKSIZE temporary data words */
    for (i = 16; i < 80; i++) {
        W[i] = W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16];
        if (context->kind) {
            W[i] = S_SHA(1, W[i]);
        }
    }

    /* Guts (four sub-rounds) */
#define PRE  temp = W[i] + S_SHA(5,A) + E +
#define POST ; E = D; D = C; C = S_SHA(30,B); B = A; A = temp;
    for (i = 0; i < 20; i++) {
        PRE 0x5A827999L + ((B & C) | (~B & D))  POST
    }
    for (i = 20; i < 40; i++) {
        PRE 0x6ED9EBA1L + (B ^ C ^ D)                 POST
    }
    for (i = 40; i < 60; i++) {
        PRE 0x8F1BBCDCL + ((B & C) | (B & D) | (C & D)) POST
    }
    for (i = 60; i < 80; i++) {
        PRE 0xCA62C1D6L + (B ^ C ^ D)                 POST
    }

    /* Update digest */
    DG[0] += A;
    DG[1] += B;
    DG[2] += C;
    DG[3] += D;
    DG[4] += E;



    /* Block is now empty */
    context->thisword = 0;
    context->thisbyte = 0;
}


/* ******************************* */


void CHE_sha1_append(sha1_context_ptr context, BITS8 *buffer, int count)
{

    BITS32 thebits; /* current bit pattern being processed */
    int theword; /* Which word in the buffer we are dealing with. */

    if (!context) {
        context = &canned_context;
    }

    /* Add a potentially 32 bit count to the two word count */
    context->countHi += count >> 29;           /* handle count > 2**29 */
    context->countLo += count & 0x1FFFFFFF;    /* Handle count <=2**29 */
    context->countHi += context->countLo >> 29;/* Handle carry */
    context->countLo &= 0x1FFFFFFF;            /* Clear carry  */

    theword = context->thisword;
    thebits = context->data[theword];

    while (count--) {
        thebits = (thebits << 8) | *buffer++;
        if (++context->thisbyte >= 4) {
            context->data[theword++] = thebits;
            thebits = 0;
            if (theword >= SHF_BLOCKWORDSIZE) {
                CHE_shaTransform(context);
                theword = 0;
            }
            context->thisbyte = 0;
        }
    }
    context->data[theword] = thebits;
    context->thisword = theword;

}
/* ************************************ */
void CHE_shaStrUpdate(sha1_context_ptr context, char *input)
{
    CHE_sha1_append(context, (BITS8 *)input, strlen(input));
}

/* ************************************ */
/* Pad out a block. */

void CHE_shapad(sha1_context_ptr context)
{
    int loopindex;

    //Julie 1214
    //if ((context->length) % 64 == 55)

    if (!context) {
        context = &canned_context;
    }

    context->data[context->thisword] <<= 8;
    context->data[context->thisword] |= 0x80;
    /* pad out the rest of this word */
    switch (context->thisbyte) {
        case 3:
            ;
            break;
        case 2:
            context->data[context->thisword] <<= 8;
            break;
        case 1:
            context->data[context->thisword] <<= 16;
            break;
        case 0:
            context->data[context->thisword] <<= 24;
            break;
    }

    /* and then the rest of the words in the block */
    if ((context->thisword != 13) && (context->thisbyte != 3)) //Julie1213
        for (loopindex = context->thisword + 1; loopindex < SHF_BLOCKWORDSIZE; loopindex++) {
            context->data[loopindex] = 0;
        }

    /* And note it is now empty */
    context->thisword = 0;
    context->thisbyte = 0;
}
/* ************************************ */
/* Convert a word digest to bytes, in a byte order independent manner */

void CHE_shaBytes(sha1_context_ptr context, sha1_digest adigest)
{
    int loopindex;

    if (!context) {
        context = &canned_context;
    }

    for (loopindex = 0; loopindex < SHF_DIGESTWORDSIZE; loopindex++) {
        *adigest++ = (BITS8)(context->digest[loopindex] >> 24  & (BITS32) 0xFF);
        *adigest++ = (BITS8)(context->digest[loopindex] >> 16  & (BITS32) 0xFF);
        *adigest++ = (BITS8)(context->digest[loopindex] >>  8  & (BITS32) 0xFF);
        *adigest++ = (BITS8)(context->digest[loopindex]        & (BITS32) 0xFF);
    }

}

void CHE_sha1_finish(sha1_context_ptr context, sha1_digest adigest)
{
    int loopindex;
    int bytesused;

    if (!context) {
        context = &canned_context;
    }

    /* bytes used in the buffer */
//bytesused = context->thisword * 4 + context->thisbyte + 2;
    bytesused = context->thisword * 4 + context->thisbyte + 1; //Julie 1213
    /* +1 for un zerobasing, +1 for the pad character we're about to do */

    /* Pad, but with the convention that the 0 pad starts with a single */
    /* one bit. */
    CHE_shapad(context);

    /* if we don't have room for the message size, then start a new block */
    if (bytesused > (SHF_BLOCKSIZE - SHF_LENGTH_PAD)) {
        CHE_shaTransform(context);
        for (loopindex = 0; loopindex < (SHF_BLOCKWORDSIZE - SHF_PAD_WORDS); loopindex++) {
            context->data[loopindex] = 0;
        }
        context->thisword = SHF_BLOCKWORDSIZE;
        context->thisbyte = 0;
    }

    /* Append length in bits, and transform */
    context->data[14] = context->countHi;
    context->data[15] = context->countLo << 3; /* We keep it as a byte count */
    CHE_shaTransform(context);

    /* get the digest out to the user. */
    CHE_shaBytes(context, adigest);

} /* end routine sha1_finish */


/* ************************************************************ */

void CHE_shaHash(int version, BITS8 *buffer, int count, sha1_digest adigest)
{
    sha1_context example;

    CHE_sha1_Init(&example);
    CHE_sha1_append(&example, buffer, count);
    CHE_sha1_finish(&example, adigest);

}


kal_bool che_sw_mac(CHE_TYPE type, STCHE *che_context, kal_uint8 *data_src, kal_uint8 *data_dst, kal_int32 length, kal_bool last_block)
{
    kal_uint8 k_ipad[64], k_opad[64], tmpbuf[32];
    kal_uint32 i;
    SW_MISC_CNXT *mac_cnxt = NULL;
    md5_context *mac_md5_str = NULL;
    sha1_context *mac_sha1_str = NULL;
    SHA256_CTX *mac_sha_str = NULL;

    //ASSERT(che_context && data_src && data_dst);
    mac_cnxt = (SW_MISC_CNXT *) che_context->cheWorkBuf;

    if (NULL == mac_cnxt->working_buffer) {
#if 0
        if (type == CHE_MAC_SHA256 || type == CHE_MAC_SHA224) {
            mac_cnxt->working_buffer = MEM_MALLOC(sizeof(SHA256_CTX));

        } else
#endif
            if (type == CHE_MAC_MD5) {
                mac_cnxt->working_buffer = MEM_MALLOC(sizeof(md5_context));

            } else if (type == CHE_MAC_SHA1) {
                mac_cnxt->working_buffer = MEM_MALLOC(sizeof(sha1_context));

            }
    }

#if 0
    if (type == CHE_MAC_SHA256 || type == CHE_MAC_SHA224) {
        mac_sha_str = (SHA256_CTX *) mac_cnxt->working_buffer;
    } else
#endif
        if (type == CHE_MAC_MD5) {
            mac_md5_str = (md5_context *) mac_cnxt->working_buffer;
        } else

            if (type == CHE_MAC_SHA1) {
                mac_sha1_str = (sha1_context *) mac_cnxt->working_buffer;
            }

    if ((mac_sha_str == NULL && mac_md5_str == NULL &&  mac_sha1_str == NULL) ||
            che_context->cheKey == NULL) {
        return KAL_FALSE;
    }

    if (che_context->cheKeyLength > 64) {
        switch (type) {
#if 0
            case CHE_MAC_SHA224: {
                CHE_SHA224_Init(mac_sha_str);
                CHE_SHA224_Update(mac_sha_str, (const void *)che_context->cheKey,
                                  (size_t)che_context->cheKeyLength);
                CHE_SHA224_Final((unsigned char *)tmpbuf, mac_sha_str);
                che_context->cheKey = tmpbuf;
                che_context->cheKeyLength = 28;
            }
            break;

            case CHE_MAC_SHA256: {
                CHE_SHA256_Init(mac_sha_str);
                CHE_SHA256_Update(mac_sha_str, (const void *)che_context->cheKey,
                                  (size_t)che_context->cheKeyLength);
                CHE_SHA256_Final((unsigned char *)tmpbuf, mac_sha_str);
                che_context->cheKey = tmpbuf;
                che_context->cheKeyLength = 32;
            }
            break;
#endif
            case CHE_MAC_SHA1: {
                CHE_sha1_Init(mac_sha1_str);
                CHE_sha1_append(mac_sha1_str, che_context->cheKey, che_context->cheKeyLength);
                CHE_sha1_finish(mac_sha1_str, tmpbuf);

                che_context->cheKey = tmpbuf;
                che_context->cheKeyLength = 20;
            }
            break;
            case CHE_MAC_MD5: {
                CHE_md5_init(mac_md5_str);
                CHE_md5_append(mac_md5_str, che_context->cheKey, che_context->cheKeyLength);
                CHE_md5_finish(mac_md5_str, tmpbuf);

                che_context->cheKey = tmpbuf;
                che_context->cheKeyLength = 16;
            }
            break;
            default:
                ASSERT(0);
        }
    }

    if (che_context->cheFirstFlag) {
        memset(k_ipad, 0x36, 64);
        for (i = 0; i < che_context->cheKeyLength; i++) {
            k_ipad[i] ^= che_context->cheKey[i];
        }
        switch (type) {
            case CHE_MAC_MD5: {
                CHE_md5_init(mac_md5_str);
                CHE_md5_append(mac_md5_str, k_ipad, 64);
            }
            break;
            case CHE_MAC_SHA1: {
                CHE_sha1_Init(mac_sha1_str);
                CHE_sha1_append(mac_sha1_str, k_ipad, 64);
            }
            break;
#if 0
            case CHE_MAC_SHA256: {
                CHE_SHA256_Init(mac_sha_str);
                CHE_SHA256_Update(mac_sha_str, k_ipad, 64);
            }
            break;

            case CHE_MAC_SHA224: {
                CHE_SHA224_Init(mac_sha_str);
                CHE_SHA224_Update(mac_sha_str, k_ipad, 64);
            }
            break;
#endif
            default:
                break;
        }
        che_context->cheFirstFlag = KAL_FALSE;
    }

    switch (type) {
        case CHE_MAC_MD5:
            CHE_md5_append(mac_md5_str, data_src, length);
            break;
        case CHE_MAC_SHA1:
            CHE_sha1_append(mac_sha1_str, data_src, length);
            break;
#if 0
        case CHE_MAC_SHA224:
            CHE_SHA224_Update(mac_sha_str, data_src, length);
            break;

        case CHE_MAC_SHA256:
            CHE_SHA256_Update(mac_sha_str, data_src, length);
            break;
#endif
        default:
            break;
    }

    if (last_block) {
        memset(k_opad, 0x5C, 64);
        for (i = 0; i < che_context->cheKeyLength; i++) {
            k_opad[i] ^= che_context->cheKey[i];
        }

        switch (type) {
            case CHE_MAC_MD5: {
                CHE_md5_finish(mac_md5_str, tmpbuf);
                CHE_md5_init(mac_md5_str);
                CHE_md5_append(mac_md5_str, k_opad, 64);
                CHE_md5_append(mac_md5_str, tmpbuf, 16);
                CHE_md5_finish(mac_md5_str, data_dst);
            }
            break;
            case CHE_MAC_SHA1: {
                CHE_sha1_finish(mac_sha1_str, tmpbuf);
                CHE_sha1_Init(mac_sha1_str);
                CHE_sha1_append(mac_sha1_str, k_opad, 64);
                CHE_sha1_append(mac_sha1_str, tmpbuf, 20);
                CHE_sha1_finish(mac_sha1_str, data_dst);
            }
            break;
#if 0
            case CHE_MAC_SHA224: {
                CHE_SHA224_Final((unsigned char *)tmpbuf, mac_sha_str);
                CHE_SHA224_Init(mac_sha_str);
                CHE_SHA224_Update(mac_sha_str, k_opad, 64);
                CHE_SHA224_Update(mac_sha_str, tmpbuf, 28);
                CHE_SHA224_Final((unsigned char *)data_dst, mac_sha_str);
            }
            break;

            case CHE_MAC_SHA256: {
                CHE_SHA256_Final((unsigned char *)tmpbuf, mac_sha_str);
                CHE_SHA256_Init(mac_sha_str);
                CHE_SHA256_Update(mac_sha_str, k_opad, 64);
                CHE_SHA256_Update(mac_sha_str, tmpbuf, 32);
                CHE_SHA256_Final((unsigned char *)data_dst, mac_sha_str);
            }
            break;
#endif
            default:
                break;
        }
    }
    return KAL_TRUE;
}

/** DESCRIPTION :
 *     Initialize the CHE_CONTEXT data structure created from AP \n
 *  @param[in] STCHE* che_context one context created from CHE AP
 *  @return None
 */
/*****************************************************************************
 * FUNCTION
 *  che_init
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [?]
 *  type            [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void che_init(STCHE *che_context, kal_uint32 type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //ASSERT(che_context);

    memset(che_context, 0, sizeof(STCHE));

    che_context->cipher_type = type;
    che_context->cheBufLen = type2sz(type);
    if (che_context->cheBufLen) {
        che_context->cheWorkBuf = MEM_MALLOC(che_context->cheBufLen);
        //ASSERT(NULL != che_context->cheWorkBuf);
        memset(che_context->cheWorkBuf, 0, che_context->cheBufLen);
    }
    che_context->cheFirstFlag = KAL_TRUE;
}

/** DESCRIPTION :
 *     Generate, set, get, and destroy key of the specified length \n
 *  @param[in] che_context one context created from CHE AP
 *  @param[in] key_act generate, set, get, and destroy. For RSA, user also could use CHE_SET_RSA_ to set the eight factors to generate the public and private key.
 *  @param[in] key input for generate, set and destroy key. The input has to be 4-byte alignment on ARM platform. Output for get key.
 *  @param[in] key_len original key length
 *  @return None
 */
/*****************************************************************************
 * FUNCTION
 *  che_set_iv
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  iv              [IN]
 *  iv_len          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void che_set_iv(STCHE *che_context, kal_uint8 *iv, kal_uint32 iv_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //ASSERT(che_context && iv);

    che_context->cheIV = iv;
    che_context->cheIVLength = iv_len;

#if defined(__HW_CHE__)
    che_context->cheResetIVFlag = KAL_TRUE;
#endif
}

/* check it after execute cipher/hash functions */
/*****************************************************************************
 * FUNCTION
 *  checkCHEProcessPostfix
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [?]
 *  last_block      [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void checkCHEProcessPostfix(STCHE *che_context, kal_bool last_block)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (last_block) {
#if defined(__HW_CHE__)
        che_context->cheResetKeyFlag = KAL_FALSE;
        che_context->cheResetIVFlag = KAL_FALSE;
        che_context->cheFirstFlag = KAL_TRUE;
#else /* defined(__HW_CHE__) */
        che_context->cheLastLength = 0;
        che_context->cheFirstFlag = KAL_TRUE;
        che_context->cheLastDstAddr = NULL;
#endif /* defined(__HW_CHE__) */
    }
}
/*****************************************************************************
 * FUNCTION
 *   che_sw_sha_1
 *
 * DESCRIPTION
 *   Execute SHA_1 hash function.
 *
 * PARAMETERS
 *   che_context              [IN] : one context created from CHE AP
 *   data_src            [IN] : input data source
 *   data_dst            [OUT] : output dest data
 *   length              [IN] : input data source length
 *   last_block          [IN] : KAL_TRUE is last block. KAL_FLASE is none.
 *
 * RETURNS
 *   KAL_TRUE : if DES process successful
 *   KAL_FLASE : if DES process fail.
 *****************************************************************************/
kal_bool che_sw_sha_1(STCHE *che_context, kal_uint8 *data_src, kal_uint8 *data_dst, kal_int32 length, kal_bool last_block)
{
    SW_MISC_CNXT *sha1_cnxt;
    sha1_context *sha1_str;

    //ASSERT(che_context && data_dst);

    if (length > 0 && data_src == NULL) {
        ASSERT(0);
    }

    sha1_cnxt = (SW_MISC_CNXT *) che_context->cheWorkBuf;

    if (NULL == sha1_cnxt->working_buffer) {
        sha1_cnxt->working_buffer = MEM_MALLOC(sizeof(sha1_context));
    }

    sha1_str = (sha1_context *) sha1_cnxt->working_buffer;

    if (che_context->cheFirstFlag) {
        CHE_sha1_Init(sha1_str);
        che_context->cheFirstFlag = KAL_FALSE;
    }

    CHE_sha1_append(sha1_str, data_src, length);

    if (last_block) {
        CHE_sha1_finish(sha1_str, data_dst);
    }
    return KAL_TRUE;
}

/*
che_rc4_skip: the iteration of RC4 algorithm
*/
static void che_rc4_process(RC4_CNXT *keySet, kal_uint32 data_len, kal_uint8 *data, kal_uint8 *output)
{
    kal_uint32 x, y, k;
    kal_uint8 *s = keySet->keyStream;

    x = keySet->x;
    y = keySet->y;

    for (k = 0; k < data_len; k++) {
        x = (x + 1) & 0xff;
        y = (y + s[x]) & 0xff;
        S_SWAP(s[x], s[y]);
        *output = (*data) ^ (s[(s[x] + s[y]) & 0xff]);
        output++;
        data++;
    }

    keySet->x = x;
    keySet->y = y;
}



/*
  RC4_set_key: reset the RC4 keystream
  data: RC4 key
*/
void che_rc4_set_key(RC4_CNXT *keySet, kal_uint32 len, kal_uint8 *data)
{

    kal_uint32 i, j, k;
    kal_uint8 *s = keySet->keyStream;

    /* Setup RC4 state */
    for (k = 0; k < 256; k++) {
        s[k] = k;
    }

    j = 0;
    k = 0;
    for (i = 0; i < 256; i++) {
        j = (j + s[i] + data[k]) & 0xff;
        if (++k == len) {
            k = 0;
        }
        S_SWAP(s[i], s[j]);
    }
    keySet->x = 0;
    keySet->y = 0;

}



/*
Important!! For che_rc4_skip:
In IEEE802.11i
Section 8.5.2 EAPOL-Key frames

The key wrap algorithm selected depends on the key descriptor version:
¡X Key Descriptor Version 1: RC4 is used to encrypt the Key Data field using the KEK field from the
derived PTK. No padding shall be used. The encryption key is generated by concatenating the
EAPOL-Key IV field and the KEK. The first 256 octets of the RC4 key stream shall be discarded
following RC4 stream cipher initialization with the KEK, and encryption begins using the 257th key
stream octet.
*/
void che_rc4_skip(RC4_CNXT *keySet, kal_uint8 *input, kal_uint32 len, kal_uint8 *key, kal_uint32 key_len, kal_uint32 skip_len, kal_uint8 *output)
{
    kal_uint32 i, j, k;
    kal_uint8 *s = keySet->keyStream;

    che_rc4_set_key(keySet, key_len, key);

    /* Skip the start of the stream */
    i = keySet->x;
    j = keySet->y;

    for (k = 0; k < skip_len; k++) {
        i = (i + 1) & 0xff;
        j = (j + s[i]) & 0xff;
        S_SWAP(s[i], s[j]);
    }

    keySet->x = i;
    keySet->y = j;
    che_rc4_process(keySet, len, input, output);
}


//void che_rc4(RC4_CNXT *keySet, kal_int32 len, kal_uint8 *indata, kal_uint8 *outdata)
void che_rc4(RC4_CNXT *keySet, kal_uint8 *input, kal_uint32 len, kal_uint8 *key, kal_uint32 key_len, CHE_OPERATION_MODE mode, kal_uint8 *output)
{
    if (mode == CHE_SKIP) {
        che_rc4_skip(keySet, input, len, key, key_len, 256, output);
    } else {
        che_rc4_process(keySet, len, input, output);
    }
}

kal_bool che_sw_rc4(STCHE *che_context, CHE_OPERATION_MODE mode, kal_uint8 *data_src, kal_uint8 *data_dst, kal_uint32 length, kal_bool last_block)
{
    RC4_CNXT *cnxt = (RC4_CNXT *) che_context->cheWorkBuf;

    che_rc4(cnxt, data_src, length, che_context->cheKey, che_context->cheKeyLength, mode, data_dst);
    return KAL_TRUE;
}
/*****************************************************************************
 * FUNCTION
 *   che_sw_md5
 *
 * DESCRIPTION
 *   Execute MD5 hash function.
 *
 * PARAMETERS
 *   che_context              [IN] : one context created from CHE AP
 *   data_src            [IN] : input data source
 *   data_dst            [OUT] : output dest data
 *   length              [IN] : input data source length
 *   last_block          [IN] : KAL_TRUE is last block. KAL_FLASE is none.
 *
 * RETURNS
 *   KAL_TRUE : if DES process successful
 *   KAL_FLASE : if DES process fail.
 *****************************************************************************/
kal_bool che_sw_md5(STCHE *che_context, kal_uint8 *data_src, kal_uint8 *data_dst, kal_int32 length, kal_bool last_block)
{
    SW_MISC_CNXT *md5_cnxt;
    md5_context *md5_str;

    //ASSERT(che_context && data_dst);

    if (length > 0 && data_src == NULL) {
        ASSERT(0);
    }

    md5_cnxt = (SW_MISC_CNXT *) che_context->cheWorkBuf;

    if (NULL == md5_cnxt->working_buffer) {
        md5_cnxt->working_buffer = MEM_MALLOC(sizeof(md5_context));
    }

    md5_str = (md5_context *) md5_cnxt->working_buffer;

    if (che_context->cheFirstFlag) {
        CHE_md5_init(md5_str);
        che_context->cheFirstFlag = KAL_FALSE;
    }

    CHE_md5_append(md5_str, data_src, length);

    if (last_block) {
        CHE_md5_finish(md5_str, data_dst);
    }
    return KAL_TRUE;
}


static void GET_UINT32_LE(unsigned long *n, unsigned char *b, kal_int32 i);
static void PUT_UINT32_LE(unsigned long *n, unsigned char *b, kal_int32 i);


static void GET_UINT32_LE(unsigned long *n, unsigned char *b, kal_int32 i)
{
    (*n) = ((unsigned long)(b)[(i)    ])
           | ((unsigned long)(b)[(i) + 1] <<  8)
           | ((unsigned long)(b)[(i) + 2] << 16)
           | ((unsigned long)(b)[(i) + 3] << 24);
}

static void PUT_UINT32_LE(unsigned long *n, unsigned char *b, kal_int32 i)
{
    (b)[(i)    ] = (unsigned char)((*n));
    (b)[(i) + 1] = (unsigned char)((*n) >>  8);
    (b)[(i) + 2] = (unsigned char)((*n) >> 16);
    (b)[(i) + 3] = (unsigned char)((*n) >> 24);
}

/*
 * MD4 context setup
 */
void CHE_md4_init(md4_context *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}



static void md4_process(md4_context *ctx, unsigned char data[64])
{
    unsigned long X[16], A, B, C, D;

    GET_UINT32_LE(&X[0],  data,  0);
    GET_UINT32_LE(&X[1],  data,  4);
    GET_UINT32_LE(&X[2],  data,  8);
    GET_UINT32_LE(&X[3],  data, 12);
    GET_UINT32_LE(&X[4],  data, 16);
    GET_UINT32_LE(&X[5],  data, 20);
    GET_UINT32_LE(&X[6],  data, 24);
    GET_UINT32_LE(&X[7],  data, 28);
    GET_UINT32_LE(&X[8],  data, 32);
    GET_UINT32_LE(&X[9],  data, 36);
    GET_UINT32_LE(&X[10], data, 40);
    GET_UINT32_LE(&X[11], data, 44);
    GET_UINT32_LE(&X[12], data, 48);
    GET_UINT32_LE(&X[13], data, 52);
    GET_UINT32_LE(&X[14], data, 56);
    GET_UINT32_LE(&X[15], data, 60);

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x, y, z) ((x & y) | ((~x) & z))
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x; a = S(a,s); }

    P(A, B, C, D, X[ 0],  3);
    P(D, A, B, C, X[ 1],  7);
    P(C, D, A, B, X[ 2], 11);
    P(B, C, D, A, X[ 3], 19);
    P(A, B, C, D, X[ 4],  3);
    P(D, A, B, C, X[ 5],  7);
    P(C, D, A, B, X[ 6], 11);
    P(B, C, D, A, X[ 7], 19);
    P(A, B, C, D, X[ 8],  3);
    P(D, A, B, C, X[ 9],  7);
    P(C, D, A, B, X[10], 11);
    P(B, C, D, A, X[11], 19);
    P(A, B, C, D, X[12],  3);
    P(D, A, B, C, X[13],  7);
    P(C, D, A, B, X[14], 11);
    P(B, C, D, A, X[15], 19);

#undef P
#undef F



#define F(x,y,z) ((x & y) | (x & z) | (y & z))
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x + 0x5A827999; a = S(a,s); }

    P(A, B, C, D, X[ 0],  3);
    P(D, A, B, C, X[ 4],  5);
    P(C, D, A, B, X[ 8],  9);
    P(B, C, D, A, X[12], 13);
    P(A, B, C, D, X[ 1],  3);
    P(D, A, B, C, X[ 5],  5);
    P(C, D, A, B, X[ 9],  9);
    P(B, C, D, A, X[13], 13);
    P(A, B, C, D, X[ 2],  3);
    P(D, A, B, C, X[ 6],  5);
    P(C, D, A, B, X[10],  9);
    P(B, C, D, A, X[14], 13);
    P(A, B, C, D, X[ 3],  3);
    P(D, A, B, C, X[ 7],  5);
    P(C, D, A, B, X[11],  9);
    P(B, C, D, A, X[15], 13);

#undef P
#undef F


#define F(x,y,z) (x ^ y ^ z)
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x + 0x6ED9EBA1; a = S(a,s); }

    P(A, B, C, D, X[ 0],  3);
    P(D, A, B, C, X[ 8],  9);
    P(C, D, A, B, X[ 4], 11);
    P(B, C, D, A, X[12], 15);
    P(A, B, C, D, X[ 2],  3);
    P(D, A, B, C, X[10],  9);
    P(C, D, A, B, X[ 6], 11);
    P(B, C, D, A, X[14], 15);
    P(A, B, C, D, X[ 1],  3);
    P(D, A, B, C, X[ 9],  9);
    P(C, D, A, B, X[ 5], 11);
    P(B, C, D, A, X[13], 15);
    P(A, B, C, D, X[ 3],  3);
    P(D, A, B, C, X[11],  9);
    P(C, D, A, B, X[ 7], 11);
    P(B, C, D, A, X[15], 15);

#undef F
#undef P

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}



/*
 * MD4 process buffer
 */
void CHE_md4_append(md4_context *ctx, unsigned char *input, int ilen)
{
    int fill;
    unsigned long left;

    if (ilen <= 0) {
        return;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (unsigned long) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left),
               (void *) input, fill);
        md4_process(ctx, ctx->buffer);
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= 64) {
        md4_process(ctx, input);
        input += 64;
        ilen  -= 64;
    }

    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left),
               (void *) input, ilen);
    }
}



static const unsigned char md4_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};



/*
 * MD4 final digest
 */
void CHE_md4_finish(md4_context *ctx, unsigned char output[16])
{
    unsigned long last, padn;
    unsigned long high, low;
    unsigned char msglen[8];

    high = (ctx->total[0] >> 29)
           | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    PUT_UINT32_LE(&low,  msglen, 0);
    PUT_UINT32_LE(&high, msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    CHE_md4_append(ctx, (unsigned char *) md4_padding, padn);
    CHE_md4_append(ctx, msglen, 8);

    PUT_UINT32_LE(&ctx->state[0], output,  0);
    PUT_UINT32_LE(&ctx->state[1], output,  4);
    PUT_UINT32_LE(&ctx->state[2], output,  8);
    PUT_UINT32_LE(&ctx->state[3], output, 12);
}




/*
 * Output = HMAC-MD4( input buffer, hmac key )
 */
void md4_hmac(unsigned char *key, int keylen,
              unsigned char *input, int ilen,
              unsigned char output[16])
{
    int i;
    md4_context ctx;
    unsigned char k_ipad[64];
    unsigned char k_opad[64];
    unsigned char tmpbuf[16];

    memset(k_ipad, 0x36, 64);
    memset(k_opad, 0x5C, 64);

    for (i = 0; i < keylen; i++) {
        if (i >= 64) {
            break;
        }

        k_ipad[i] ^= key[i];
        k_opad[i] ^= key[i];
    }

    CHE_md4_init(&ctx);
    CHE_md4_append(&ctx, k_ipad, 64);
    CHE_md4_append(&ctx, input, ilen);
    CHE_md4_finish(&ctx, tmpbuf);

    CHE_md4_init(&ctx);
    CHE_md4_append(&ctx, k_opad, 64);
    CHE_md4_append(&ctx, tmpbuf, 16);
    CHE_md4_finish(&ctx, output);

    memset(k_ipad, 0, 64);
    memset(k_opad, 0, 64);
    memset(tmpbuf, 0, 16);
    memset(&ctx, 0, sizeof(md4_context));
}


kal_bool che_sw_md4(STCHE *che_context, kal_uint8 *data_src, kal_uint8 *data_dst, kal_int32 length, kal_bool last_block)
{
    SW_MISC_CNXT *md4_cnxt;
    md4_context *md4_str;

    //ASSERT(che_context && data_src && data_dst);
    md4_cnxt = (SW_MISC_CNXT *) che_context->cheWorkBuf;

    if (NULL == md4_cnxt->working_buffer) {
        md4_cnxt->working_buffer = MEM_MALLOC(sizeof(md4_context));
    }

    md4_str = (md4_context *) md4_cnxt->working_buffer;

    if (che_context->cheFirstFlag) {
        CHE_md4_init(md4_str);
        che_context->cheFirstFlag = KAL_FALSE;
    }

    CHE_md4_append(md4_str, data_src, length);

    if (last_block) {
        CHE_md4_finish(md4_str, data_dst);
    }
    return KAL_TRUE;
}

// AES kernel processes, including AES128/192/256 type,  ECB/CBC mode  //
static void AESKernel(kal_uint8 *plaintext, kal_int32 cipher_length, STCHE *che_context,
                      CHE_OPERATION_MODE mode, CHE_ACTION act, AES_KEY *aes_decryption_key, kal_uint8 **data_dst,
                      kal_uint8 **pLastAddr, AES_CNXT *cnxt)
{
    if (CHE_ECB == mode) {
        che_context->cheIV = NULL;
    }
    if (CHE_ENC == act) {
        aes_decryption_key->rounds = CHE_AES_setup_enc_key((kal_uint32 *) aes_decryption_key->rd_key,
                                     che_context->cheKey, che_context->cheKeyLength * 8);
        if (mode == CHE_CTR) {
            CHE_AES_ctr_encrypt(che_context->cheIV, aes_decryption_key, plaintext, cipher_length,
                                *data_dst, che_context->cheFirstFlag, &cnxt->orgIV[0]);
        } else if (CHE_F8 == mode) {
            CHE_AES_f8_encrypt(che_context->cheIV, aes_decryption_key, plaintext, cipher_length,
                               *data_dst, che_context->cheFirstFlag);
        } else {
            CHE_AES_block_encrypt(che_context->cheIV, aes_decryption_key, plaintext, cipher_length,
                                  *data_dst, che_context->cheFirstFlag, &cnxt->orgIV[0]);
        }
    } else if (CHE_DEC == act) {
        if (CHE_CTR == mode) {
            aes_decryption_key->rounds = CHE_AES_setup_enc_key((kal_uint32 *) aes_decryption_key->rd_key,
                                         che_context->cheKey, che_context->cheKeyLength * 8);
            CHE_AES_ctr_decrypt(che_context->cheIV, aes_decryption_key, plaintext, cipher_length,
                                *data_dst, che_context->cheFirstFlag, &cnxt->orgIV[0]);
        } else if (CHE_F8 == mode) {
            aes_decryption_key->rounds = CHE_AES_setup_enc_key((kal_uint32 *) aes_decryption_key->rd_key,
                                         che_context->cheKey, che_context->cheKeyLength * 8);
            CHE_AES_f8_decrypt(che_context->cheIV, aes_decryption_key, plaintext, cipher_length, *data_dst,
                               che_context->cheFirstFlag);
        } else {
            aes_decryption_key->rounds = CHE_AES_setup_dec_key((kal_uint32 *) aes_decryption_key->rd_key,
                                         che_context->cheKey, che_context->cheKeyLength * 8);
            CHE_AES_block_decrypt(che_context->cheIV, aes_decryption_key, plaintext, cipher_length, *data_dst,
                                  che_context->cheFirstFlag, &cnxt->orgIV[0]);
        }
    }
    if (che_context->cheFirstFlag) {
        che_context->cheFirstFlag = KAL_FALSE;
    }
    *pLastAddr += cipher_length;
    *data_dst += cipher_length;
}

/*****************************************************************************
 * FUNCTION
 *   che_sw_aes
 *
 * DESCRIPTION
 *   Execute AES128/192/256 ECB/CBC function.
 *
 * PARAMETERS
 *   che_context         [IN] : one context created from CHE AP
 *   type                [IN] : type for DES/3DES/AES/MD5/SHA1
 *   mode                [IN] : mode for ECB/CBC/CTR
 *   aes_length          [IN] : aes key length sequence for 128/192/256
 *   act                 [IN] : action for HASH/Encipher/Decipher
 *   data_src            [IN] : input data source
 *   data_dst            [OUT] : output dest data
 *   length              [IN] : input data source length
 *   last_block          [IN] : KAL_TRUE is last block. KAL_FLASE is none.
 *
 * RETURNS
 *   KAL_TRUE : if DES process successful
 *   KAL_FLASE : if DES process fail.
 *****************************************************************************/
kal_bool che_sw_aes(STCHE *che_context, CHE_OPERATION_MODE mode, CHE_ACTION act,
                    kal_uint8 *data_src, kal_uint8 *data_dst, kal_int32 length, kal_bool last_block)
{
    AES_KEY aes_decryption_key;
    AES_CNXT *cnxt;

    kal_uint8 *pLastRemider, *pLastAddr;  //*pIV, *pLastBlock,get array ptr for using in for loop //
    kal_uint32  fill_length = 0;
    //kal_uint32  cipher_length = 0;
    kal_int32 i;

    //ASSERT(che_context && data_src && data_dst);
    cnxt = (AES_CNXT *) che_context->cheWorkBuf;

    // If it's first block, initialize the last destination address as the output address //
    if (che_context->cheFirstFlag) {
        che_context->cheLastDstAddr = data_dst;
    }

    pLastAddr = che_context->cheLastDstAddr;
    //pLastBlock = che_context->cheLastBlock;

    //For CHE_F8/CHE_CTR/CHE_KEY_WRAP/CHE_KEY_UNWRAP, last_block = true
    if ((CHE_F8 == mode) || (CHE_CTR == mode)) {
        ASSERT(last_block);
        //if (KAL_FALSE == last_block) {
        //  return KAL_FALSE;
        //}

        AESKernel(data_src, length, che_context, mode, act, &aes_decryption_key, &data_dst, &pLastAddr, cnxt);
        return KAL_TRUE;
    }

    if (CHE_KEY_WRAP == mode) {
        ASSERT(last_block);
        //if (KAL_FALSE == last_block) {
        //   return KAL_FALSE;
        //}

        aes_decryption_key.rounds = CHE_AES_setup_enc_key((kal_uint32 *) aes_decryption_key.rd_key, che_context->cheKey,  che_context->cheKeyLength * 8);
        CHE_AES_WRAP(&aes_decryption_key,  length / 8, data_src, data_dst);
        return KAL_TRUE;
    }

    if (CHE_KEY_UNWRAP == mode) {
        //if (last_block == KAL_FALSE) {
        //  return KAL_FALSE;
        //}
        ASSERT(last_block);
        aes_decryption_key.rounds = CHE_AES_setup_dec_key((kal_uint32 *) aes_decryption_key.rd_key, che_context->cheKey,  che_context->cheKeyLength * 8);
        CHE_AES_UNWRAP(&aes_decryption_key, (length - 8) / 8, data_src, data_dst);
        return KAL_TRUE;
    }

    // Two conditions to fill out remider block //
    // Either there are remider in last block, or the length of this inupt source is less then AES one block //
    if ((che_context->cheLastLength > 0) || (length < AES_BLOCK_SIZE)) {
        // decide the total filled length //
        fill_length = (che_context->cheLastLength + length <= AES_BLOCK_SIZE) ? length : (AES_BLOCK_SIZE - che_context->cheLastLength);

        pLastRemider = &(cnxt->cheLastReminder[che_context->cheLastLength]);

        for (i = fill_length; i > 0; i--) {
            *pLastRemider++ = *data_src++;
        }

        che_context->cheLastLength += fill_length; // last length for reminder block add fill_length //

        // If it's last block, padding the block, then encrypt/decrypt DES processes //
        if (last_block && (che_context->cheLastLength < AES_BLOCK_SIZE)) {
            fill_length = AES_BLOCK_SIZE - che_context->cheLastLength;

            for (i = fill_length; i > 0; i--) {
                *pLastRemider++ = fill_length;
            }

            AESKernel(cnxt->cheLastReminder, AES_BLOCK_SIZE, che_context, mode, act, &aes_decryption_key, &data_dst, &pLastAddr, cnxt);

            return KAL_TRUE;
        }

        // current input data length sub fill_length //
        length -= fill_length;
    }


    // if the reminder block is not filled out, return //
    // else, encrypt this block                        //
    if (che_context->cheLastLength) {
        if (che_context->cheLastLength < AES_BLOCK_SIZE) {
            return KAL_TRUE;
        }

        AESKernel(cnxt->cheLastReminder, AES_BLOCK_SIZE, che_context, mode, act, &aes_decryption_key, &data_dst, &pLastAddr, cnxt);
    }

    // while loop, encrypt/decrypt AES processes block by block //
    // while (length >= AES_BLOCK_SIZE) {
    if (length >= AES_BLOCK_SIZE) {
        // instead of whileloop //
        fill_length = length - (length % 16);
        AESKernel(data_src, fill_length, che_context, mode, act, &aes_decryption_key, &data_dst, &pLastAddr, cnxt);
    }

    data_src += AES_BLOCK_SIZE;
    length -= fill_length;

    // after AES processes, fill out the units to reminder block for next input data AES processes //
    pLastRemider = cnxt->cheLastReminder;
    for (i = length; i > 0; i--) {
        *pLastRemider++ = *data_src++;
    }

    // If it's last block, padding the block, then encrypt/decrypt AES processes //
    if (last_block) {
        fill_length = AES_BLOCK_SIZE - length;
        for (i = fill_length; i > 0; i--) {
            *pLastRemider++ = fill_length;
        }

        AESKernel(cnxt->cheLastReminder, AES_BLOCK_SIZE, che_context, mode, act, &aes_decryption_key, &data_dst, &pLastAddr, cnxt);
    }

    // record the last output address, and the number of reminder units //
    che_context->cheLastDstAddr = pLastAddr;
    che_context->cheLastLength = length;

    return KAL_TRUE;
}

kal_bool che_sw_dh(STCHE *che_context, CHE_ACTION act,
                   kal_uint8 *data_src, kal_uint8 *data_dst,
                   kal_int32 length, kal_bool last_block)
{
    dhm_context *dhm_ptr = (dhm_context *)che_context->cheWorkBuf;

    switch (act) {
        case DH_INIT:
            //ASSERT(0 == che_context->cheWorkBuf);
            che_context->cheWorkBuf = MEM_MALLOC(sizeof(dhm_context));
            memset(che_context->cheWorkBuf, 0, sizeof(dhm_context));
            break;

        case DH_MAKE_PUBLIC:
            if (dhm_make_public_internal(dhm_ptr, 256, (int (*)(void *))rand, NULL)) {
                return KAL_FALSE;
            }
            break;

        case DH_MAKE_PARAMS:
            if (dhm_make_params_internal(dhm_ptr, 256, (int (*)(void *))rand, NULL)) {
                return KAL_FALSE;
            }
            break;

        case DH_CALC_SECRET:
            if (dhm_calc_secret_internal(dhm_ptr)) {
                return KAL_FALSE;
            }
            break;

        case DH_FREE:
            if (dhm_ptr) {
                dhm_free(dhm_ptr);
                MEM_FREE(dhm_ptr);
                che_context->cheWorkBuf = NULL;
            }
            break;

        default:
            return KAL_FALSE;
    }

    return KAL_TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  run_CHE_MAC_SHA1_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_MAC_SHA1_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (0 == pstFSAL) {
        /* process source data from AP input */
        rtnValue = che_sw_mac(CHE_MAC_SHA1, che_context, source, dest, source_length, last_block);

        checkCHEProcessPostfix(che_context, last_block);
    } else {
        /* process source data from file */
        ;
    }
    return rtnValue;
}

/*****************************************************************************
 * FUNCTION
 *  run_CHE_SHA1_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_SHA1_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (0 == pstFSAL) {
        /* process source data from AP input */

        /* SW hash processes // */
        rtnValue = che_sw_sha_1(che_context, source, dest, source_length, last_block);

        checkCHEProcessPostfix(che_context, last_block);
    } else {
        /* process source data from file */
        ;
    }
    return rtnValue;
}

/* execute SET_KEY action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_SET_KEY_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_SET_KEY_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    che_context->cheKey = key;
    che_context->cheKeyLength = key_len;

}

/*****************************************************************************
 * FUNCTION
 *  run_CHE_RC4_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_RC4_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEProcessPerFix(che_context);
#endif

    rtnValue = che_sw_rc4(che_context, mode, source, dest, source_length, last_block);

    checkCHEProcessPostfix(che_context, last_block);
    return rtnValue;
}

/*****************************************************************************
 * FUNCTION
 *  run_CHE_MAC_MD5_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_MAC_MD5_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (0 == pstFSAL) {
        /* process source data from AP input */
        rtnValue = che_sw_mac(CHE_MAC_MD5, che_context, source, dest, source_length, last_block);

        checkCHEProcessPostfix(che_context, last_block);
    } else {
        /* process source data from file */
        ;
    }
    return rtnValue;

}


/*****************************************************************************
 * FUNCTION
 *  run_CHE_MD5_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_MD5_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (0 == pstFSAL) {
        /* process source data from AP input */

        /* SW hash processes */
        rtnValue = che_sw_md5(che_context, source, dest, source_length, last_block);

        checkCHEProcessPostfix(che_context, last_block);

    } else {
        /* process source data from file */
        ;
    }
    return rtnValue;
}
/*****************************************************************************
 * FUNCTION
 *  run_CHE_AES_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_AES_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;
    //AES_CNXT *aescnxt = (AES_CNXT*) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* SW AES processes */
    rtnValue = che_sw_aes(che_context, mode, act, source, dest, source_length, last_block);

    checkCHEProcessPostfix(che_context, last_block);

    return rtnValue;
}

/* execute DH cipher function */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_DH_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context         [?]
 *  type                [IN]
 *  mode                [IN]
 *  act                 [IN]
 *  source              [?]
 *  dest                [?]
 *  source_length       [IN]
 *  last_block          [IN]
 *  pstFSAL             [?]
 * RETURNS
 *
 *****************************************************************************/
kal_bool run_CHE_DH_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_bool rtnValue = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rtnValue = che_sw_dh(che_context, act, source, dest, source_length, last_block);

    checkCHEProcessPostfix(che_context, last_block);

    return rtnValue;
}

/* execute SET_DH_P key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_SET_DH_P_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_SET_DH_P_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    if (che_mpi_read_binary(&(dhm->P), key, key_len)) {
        ASSERT(0);
    }
}


/* execute SET_DH_G key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_SET_DH_G_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_SET_DH_G_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    if (che_mpi_read_binary(&(dhm->G), key, key_len)) {
        ASSERT(0);
    }
}


/* execute SET_DH_SELF key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_SET_DH_SELF_PUBLIC_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_SET_DH_SELF_PUBLIC_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    if (che_mpi_read_binary(&(dhm->GX), key, key_len)) {
        ASSERT(0);
    }
    dhm->len = key_len;
}


/* execute SET_DH_PEER_PUBLIC key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_SET_DH_PEER_PUBLIC_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_SET_DH_PEER_PUBLIC_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    if (che_mpi_read_binary(&(dhm->GY), key, key_len)) {
        ASSERT(0);
    }
}


/* execute SET_DH_PRIVATE key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_SET_DH_PRIVATE_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_SET_DH_PRIVATE_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    if (che_mpi_read_binary(&(dhm->X), key, key_len)) {
        ASSERT(0);
    }
}


/* execute GET_DH_SELF_PUBLIC_LEN key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_GET_DH_SELF_PUBLIC_LEN_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_GET_DH_SELF_PUBLIC_LEN_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    /* to make sure that key is 4-bytes aligned */
    ASSERT(0 == ((kal_uint32) key & 0x3));
    *(kal_uint32 *) key = che_mpi_size(&(dhm->GX));

}


/* execute GET_DH_SELF_PUBLIC key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_GET_DH_SELF_PUBLIC_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_GET_DH_SELF_PUBLIC_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;
    int n = che_mpi_size(&(dhm->GX));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    ASSERT(key_len >= n);
    che_mpi_write_binary(&(dhm->GX), key, n);

}


/* execute GET_DH_PRIVATE_LEN action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_GET_DH_PRIVATE_LEN_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_GET_DH_PRIVATE_LEN_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    /* to make sure that key is 4-bytes aligned */
    ASSERT(0 == ((kal_uint32) key & 0x3));
    *(kal_uint32 *) key = che_mpi_size(&(dhm->X));
}


/* execute CHE_GET_DH_PRIVATE key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_GET_DH_PRIVATE_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_GET_DH_PRIVATE_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;
    int n = che_mpi_size(&(dhm->X));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    ASSERT(key_len >= n);
    che_mpi_write_binary(&(dhm->X), key, n);

}


/* execute CHE_GET_DH_SECRET_LEN action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_GET_DH_SECRET_LEN_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 *  key             [IN]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_GET_DH_SECRET_LEN_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    /* to make sure that key is 4-bytes aligned */
    ASSERT(0 == ((kal_uint32) key & 0x3));
    *(kal_uint32 *) key = che_mpi_size(&(dhm->K));

}


/* execute CHE_GET_DH_SECRET key action */
/*****************************************************************************
 * FUNCTION
 *  run_CHE_GET_DH_SECRET_process
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [?]
 *  key             [?]
 *  key_len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void run_CHE_GET_DH_SECRET_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    dhm_context *dhm = (dhm_context *) che_context->cheWorkBuf;
    int n = che_mpi_size(&(dhm->K));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__HW_CHE__)
    checkCHEKeyActionPerFix(che_context, key);
#endif

    ASSERT(key_len >= n);
    che_mpi_write_binary(&(dhm->K), key, n);

}


/*****************************************************************************
 * FUNCTION
 *  che_deinit
 * DESCRIPTION
 *
 * PARAMETERS
 *  che_context     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void che_deinit(STCHE *che_context)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //ASSERT(che_context);

    if (che_context->cheWorkBuf != NULL) {
#if !defined(__HW_CHE__)

        switch (che_context->cipher_type) {
                SW_MISC_CNXT *cnxt;

            case CHE_MD5:
            case CHE_MAC_MD5:
            case CHE_SHA1:
            case CHE_MAC_SHA1:
            case CHE_MD4:
            case CHE_DH:
            case CHE_SHA256:
            case CHE_SHA224:
            case CHE_MAC_SHA224:
            case CHE_MAC_SHA256:
                cnxt = (SW_MISC_CNXT *) che_context->cheWorkBuf;
                if (cnxt->working_buffer) {
                    MEM_FREE(cnxt->working_buffer);
                }
                break;
#if 0
            case CHE_RSA:
                che_rsa_cntx_free(che_context);
                break;
#endif
            default:
                break;
        }
#endif /* !defined(__HW_CHE__) */

        MEM_FREE(che_context->cheWorkBuf);
        che_context->cheWorkBuf = NULL;
        che_context->cheBufLen = 0;
    }

}

