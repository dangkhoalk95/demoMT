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


#ifndef _CHE_API_H_
#define _CHE_API_H_
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

#define TRICK_CHE_MD5                     1
#define TRICK_CHE_SHA224                 1
#define TRICK_CHE_SHA256                 1
#define TRICK_CHE_SHA1                    1
#define TRICK_CHE_DES                     1
#define TRICK_CHE_3DES                    1
#define TRICK_CHE_AES                     1
#define TRICK_CHE_MD4                     1
#define TRICK_CHE_MAC_MD5                 1
#define TRICK_CHE_MAC_SHA1                1
#define TRICK_CHE_MAC_SHA224                1
#define TRICK_CHE_MAC_SHA256                1
#define TRICK_CHE_RC4                     1
#define TRICK_CHE_RSA                     1
#define TRICK_CHE_DH                      1
#define TRICK_CHE_GEN_KEY                  1
#define TRICK_CHE_SET_KEY                  1
#define TRICK_CHE_SET_RC4_KEY                 1
#define TRICK_CHE_GET_KEY                  1
#define TRICK_CHE_DEL_KEY                  1
#define TRICK_CHE_GEN_RSA_KEY              1
#define TRICK_CHE_DEL_RSA_KEY              1
#define TRICK_CHE_SET_RSA_N                1
#define TRICK_CHE_SET_RSA_E                1
#define TRICK_CHE_SET_RSA_D                1
#define TRICK_CHE_SET_RSA_P                1
#define TRICK_CHE_SET_RSA_Q                1
#define TRICK_CHE_SET_RSA_dModPm1          1
#define TRICK_CHE_SET_RSA_dModQm1          1
#define TRICK_CHE_SET_RSA_qInvModP         1
#define TRICK_CHE_SET_DH_P                 1
#define TRICK_CHE_SET_DH_G                 1
#define TRICK_CHE_SET_DH_SELF_PUBLIC       1
#define TRICK_CHE_SET_DH_PEER_PUBLIC       1
#define TRICK_CHE_SET_DH_PRIVATE           1
#define TRICK_CHE_GET_DH_SELF_PUBLIC_LEN   1
#define TRICK_CHE_GET_DH_PRIVATE_LEN       1
#define TRICK_CHE_GET_DH_SECRET_LEN        1
#define TRICK_CHE_GET_DH_SELF_PUBLIC       1
#define TRICK_CHE_GET_DH_PRIVATE           1
#define TRICK_CHE_GET_DH_SECRET            1
#define TRICK_CHE_GET_RSA_N                     1
#define TRICK_CHE_GET_RSA_N_LEN             1
#define TRICK_CHE_GET_RSA_E                     1
#define TRICK_CHE_GET_RSA_E_LEN             1
#define TRICK_CHE_GET_RSA_D                     1
#define TRICK_CHE_GET_RSA_D_LEN             1
#define TRICK_CHE_GET_RSA_P                     1
#define TRICK_CHE_GET_RSA_P_LEN             1
#define TRICK_CHE_GET_RSA_Q                     1
#define TRICK_CHE_GET_RSA_Q_LEN             1
#define TRICK_CHE_GET_RSA_dModPm1        1
#define TRICK_CHE_GET_RSA_dModPm1_LEN   1
#define TRICK_CHE_GET_RSA_dModQm1           1
#define TRICK_CHE_GET_RSA_dModQm1_LEN   1
#define TRICK_CHE_GET_RSA_qInvModP            1
#define TRICK_CHE_GET_RSA_qInvModP_LEN     1


/* DNS context structure */
typedef struct DES_CNXT_T {
#if defined(__HW_CHE__)
    kal_uint8 cheCKey[36];          /* des; 3des;convert_aes_128_key;convert_aes_192_key;convert_aes_256_key */
    kal_uint8 cheDataAddr[120];     /* che_hw_des;che_hw_aes;che_hw_hash */
#else /* defined(__HW_CHE__) */
    kal_uint8 cheLastBlock[16];     /* Last block used in counter mode (store the previous cipher text) */
    kal_uint8 cheLastReminder[16];  /* Last remainder block */
    kal_uint8 cheKeySchedule[128];  /* key schedule */
    kal_uint8 iv[64];               /* initial vector */
#endif /* defined(__HW_CHE__) */
} DES_CNXT;
/* AES context structure */
typedef struct AES_CNXT_T {
#if defined(__HW_CHE__)
    kal_uint8 cheCKey[36];          /* des; 3des;convert_aes_128_key;convert_aes_192_key;convert_aes_256_key */
    kal_uint8 cheDataAddr[120];     /* che_hw_des;che_hw_aes;che_hw_hash */
#else /* defined(__HW_CHE__) */
    kal_uint8 cheLastReminder[16];  /* Last remaining segment */
    kal_uint8 orgIV[64];            /* Initial vector */
#endif /* defined(__HW_CHE__) */
} AES_CNXT;
/* MD5 and SH1 structure */
typedef struct MD5_CNXT_T {
#if defined(__HW_CHE__)
    kal_uint8 cheDataAddr[120]; /* che_hw_des;che_hw_aes;che_hw_hash */
#else
    kal_uint8 *working_buffer; /* working buffer */
#endif
} MD5_CNXT, SHA1_CNXT;
/* structure used for MD5,SHA1, MAC_MD5, MAC_SHA1, MD4, DHM */
typedef struct CNXT_T {
    kal_uint8 *working_buffer; /* working buffer */
} SW_MISC_CNXT;
/* RC4 structure */
typedef struct RC4_CNXT_T {
    kal_uint32 x, y; /* the swap index */
    kal_uint8 keyStream[256]; /* key stream */
} RC4_CNXT;
/* RSA structure */
typedef struct RSA_CNXT_T {
    /* RSA */
    kal_uint8 *modulusN; /* modulus N. N=P*Q. It is a prime number */
    kal_uint32 modulusNLen; /* the length of modulus N */
    kal_uint8 *pubExp; /* public exponent: E */
    kal_uint32 pubExpLen; /* the length of public exponent */
    kal_uint8 *privExpD; /* private exponent: D */
    kal_uint32 privExpDLen; /* the length of private exponent D */
    kal_uint8 *primeP; /* modulus P. It is a prime number */
    kal_uint32 primePLen; /* the length of P */
    kal_uint8 *primeQ; /* modulus Q. It is a prime number */
    kal_uint32 primeQLen; /* the length of Q */
    kal_uint8 *dModPm1; /* private exponent: DP */
    kal_uint32 dModPm1Len; /* the length of private exponent DP */
    kal_uint8 *dModQm1; /* private exponent: DQ */
    kal_uint32 dModQm1Len; /* the length of private exponent DQ */
    kal_uint8 *qInvModP; /* coefficient Integer: inverse Q */
    kal_uint32 qInvModPLen; /* the length of coefficient integer */
    kal_uint8 *hash;    /* Hash value used to verify the signature */
    kal_uint32 hashLen; /* Hash length */
    kal_bool rsaKeyFlag; /* RSA key flag */
} RSA_CNXT;
typedef struct {
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[4];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
md4_context;


typedef struct STCHE_T {
    kal_uint32 cheLastLength; /* last block length */
    kal_uint8 *cheLastDstAddr; /* last destination address */

    kal_uint8 *cheWorkBuf;  /* pointe to a space used by each algorithm */
    kal_uint32 cheBufLen;   /* the length of cheWorkBuf. It depends on the encrypt algorithm */
    kal_uint32 cipher_type; /* cipher type */

    kal_uint8 *cheKey; /* key stream */
    kal_uint8 *cheIV; /* initial vector */
    kal_uint32 cheKeyLength; /* key stream length */
    kal_uint32 cheIVLength; /* initial vector length */

#if defined(__HW_CHE__)
    kal_bool cheResetKeyFlag; /* Reset key flag */
    kal_bool cheResetIVFlag; /* Reset initial vector flag */
#endif /* defined(__HW_CHE__) */

    kal_bool cheFirstFlag; /* first block flag */
} STCHE;
typedef enum {
    CHE_MD5 = 1, /* MD5 hash algorithm. The output length is fixed 16 bytes */
    CHE_SHA1, /* SHA1 hash algorithm. The output length is fixed 20 bytes */
    CHE_DES, /* DES block cipher algorithm. The CHE will do auto padding in DES type.
                The maximum padding length is 8 bytes */
    CHE_3DES, /* 3DES block cipher algorithm. The CHE will do auto padding in 3DES type.
                 The maximum padding length is 8 bytes */
    CHE_AES, /* AES block cipher algorithm. The CHE will do auto padding in AES(ECB, CBC) type.
                The maximum padding length is 16 bytes */

    CHE_MD4, /* MD4 hash algorithm. The output length is fixed 16 bytes */
    CHE_MAC_MD5, /* HMAC_MD5 hash algorithm. The output length is fixed 16 bytes */
    CHE_MAC_SHA1, /* HMAC_SHA1 hash algorithm. The output length is fixed 20 bytes */
    CHE_RC4, /* RC4 stream cipher algorithm. No padding */

    CHE_RSA, /* RSA asym cipher algorithm. */
    CHE_DH, /* Diffie Hellman cipher algorithm used to generate the key. No padding */
    CHE_MAC_SHA224, /* HMAC_SHA224 hash algorithm. The output length is fixed 28 bytes */
    CHE_MAC_SHA256, /* HMAC_SHA256 hash algorithm. The output length is fixed 32 bytes */
    CHE_SHA224, /* SHA 224 algorithm */
    CHE_SHA256 /* SHA 256 algorithm */
} CHE_TYPE;
typedef enum {
    CHE_GEN_KEY, /* generate the key exclude asynmetry and RC4 algirhtm */
    CHE_SET_KEY, /* set the key exclude asynmetry and RC4 algirhtm */
    CHE_GET_KEY, /* get the key exclude asynmetry and RC4 algirhtm*/
    CHE_DEL_KEY, /* delete the key exclude asynmetry and RC4 algirhtm */

    CHE_SET_RC4_KEY, /* set the RC4 key */

#if defined(__CHE_RSA_GEN_KEY__)
    CHE_GEN_RSA_KEY, /* generate RSA key for hardware */
    CHE_DEL_RSA_KEY, /* delete the RSA key for hardward */
#endif /* defined(__CHE_RSA_GEN_KEY__) */
    CHE_SET_RSA_N, /* set the RSA modulus N. N=P*Q. N is prime number.
                      The input N is a binary data */
    CHE_SET_RSA_E, /* set the RSA public exponent: E.
                      The input E is a binary data */
    CHE_SET_RSA_D, /* set the RSA private exponent: D.
                      The input D is a binary data */
    CHE_SET_RSA_P, /* set the RSA modulus P(CRT format).
                      The input P is a binary data */
    CHE_SET_RSA_Q, /* set the RSA modulus Q(CRT format).
                      The input Q is the binary data */
    CHE_SET_RSA_dModPm1, /* set the RSA private exponent(CRT format): DP.
                            The input DP is a binary data */
    CHE_SET_RSA_dModQm1, /* set the RSA private exponent(CRT format): DQ.
                            The input DQ is a binary data */
    CHE_SET_RSA_qInvModP, /* set the coefficient Integer(CRT format): inverse Q.
                             The input InvQ is a binary data */

    CHE_SET_DH_P, /* set the DH modulus: P. The input P is a binary data. */
    CHE_SET_DH_G, /* set the DH prime root: G. The input G is a binary data */
    CHE_SET_DH_SELF_PUBLIC, /* set the self public value. G^X mod P. The input is a binary data. */
    CHE_SET_DH_PEER_PUBLIC, /* set the peer public value. G^Y mod P. The input is a binary data. */
    CHE_SET_DH_PRIVATE, /* set the discrete logarithms(private key): X.
                           The input X is a binary data. */

    CHE_GET_DH_SELF_PUBLIC_LEN, /* get the self public length */
    CHE_GET_DH_PRIVATE_LEN, /* get the discrete logarithm length */
    CHE_GET_DH_SECRET_LEN, /* get the secret length */

    CHE_GET_DH_SELF_PUBLIC, /* get self public value */
    CHE_GET_DH_PRIVATE, /* get the discrete logarithm */
    CHE_GET_DH_SECRET, /* get the shared key,i.e. key. key = GY^X mod P */

    CHE_GET_RSA_N, /* get the RSA modulus N. N=P*Q. N is prime number. The output N is binary data */
    CHE_GET_RSA_E, /* get the RSA public exponent: E. The output E is a binary data */
    CHE_GET_RSA_D, /* get the RSA private exponent: D. The output D is a binary data */
    CHE_GET_RSA_P, /* get the RSA modulus P(CRT format). The output P is a binary data */
    CHE_GET_RSA_Q, /* get the RSA modulus Q(CRT format). The output Q is the binary data */
    CHE_GET_RSA_dModPm1, /* get the RSA private exponent(CRT format): DP. The output DP is a binary data */
    CHE_GET_RSA_dModQm1, /* get the RSA private exponent(CRT format): DQ. The output DQ is a binary data */
    CHE_GET_RSA_qInvModP, /* get the coefficient Integer(CRT format): inverse Q. The output InvQ is a binary data */

    CHE_GET_RSA_N_LEN, /* get the RSA modulus N. N=P*Q. N is prime number. The output N is binary data */
    CHE_GET_RSA_E_LEN, /* get the RSA public exponent: E. The output E is a binary data */
    CHE_GET_RSA_D_LEN, /* get the RSA private exponent: D. The output D is a binary data */
    CHE_GET_RSA_P_LEN, /* get the RSA modulus P(CRT format). The output P is a binary data */
    CHE_GET_RSA_Q_LEN, /* get the RSA modulus Q(CRT format). The output Q is the binary data */
    CHE_GET_RSA_dModPm1_LEN, /* get the RSA private exponent(CRT format): DP. The output DP is a binary data */
    CHE_GET_RSA_dModQm1_LEN, /* get the RSA private exponent(CRT format): DQ. The output DQ is a binary data */
    CHE_GET_RSA_qInvModP_LEN /* get the coefficient Integer(CRT format): inverse Q. The output InvQ is a binary data */

} CHE_KEY_ACTION;
typedef enum {
    CHE_MODE_NULL = 0, /* null mode. This mode is used when there is no mode operation */
    CHE_ECB, /* AES/DES ECB mode */
    CHE_CBC, /* AES/DES CBC mode */
    CHE_CTR, /* AES CTR(counter) mode */
    CHE_F8, /* AES F8 mode */

    CHE_SKIP, /* RC4 Skip mode */
    CHE_KEY_WRAP, /* AES key wrap mode */
    CHE_KEY_UNWRAP /* AES key unwrap mode */

} CHE_OPERATION_MODE;
typedef enum {
    CHE_HASH, /* generate hash value */
    CHE_ENC, /* encryption data */
    CHE_DEC, /* decryption data */
    CHE_MAC_HASH, /* generate HMAC value */

    DH_INIT, /* initialize the Diffie-Hellman */
    DH_MAKE_PARAMS, /* Suppose A=G^a mod p. generate discrete logarithms(a) and A */
    DH_MAKE_PUBLIC, /* Suppose A=G^a mod p. generate discrete logarithms(a) and A */
    DH_CALC_SECRET, /* Suppose B=G^b mod p. Calculate the shared secret (G^b)^a mod P */
    DH_FREE /* finialize the Diffie-Hellman */
} CHE_ACTION;
typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2];    /* message length in bits, lsw first */
    md5_word_t abcd[4];     /* digest buffer */
    md5_byte_t buf[64];     /* accumulate block */
} md5_context;
#define BITS8  unsigned char
#define BITS16 unsigned short
#define BITS32 unsigned long
#define ARGCARGV
#define PROTOTYPES



/* Defines common to everything */

#define SHF_DIGESTSIZE 20
/* Put out a digest of this size in bytes */
#define SHF_DIGESTWORDSIZE 5
/* and words */

#define SHF_BLOCKSIZE  64
#define SHF_BLOCKWORDSIZE 16
/* Process messages in this block size */

/* Structure for storing SHF info */

typedef BITS32 shadigest[SHF_DIGESTWORDSIZE];
typedef BITS8  sha1_digest[SHF_DIGESTSIZE];

typedef struct {
    BITS32   data [SHF_BLOCKWORDSIZE];     /* SHS data buffer */
    BITS32    countLo;       /* Count (64 bits in              */
    BITS32   countHi;       /* 2 32 bit words)                */
    shadigest digest;        /* Message digest                 */
    int       kind;          /* Which type of SHA?             */
    int       thisword;      /* Which word are we processing?  */
    int       thisbyte;      /* Which byte in that word?       */
    int       length; //Julie1213
} sha1_context, *sha1_context_ptr;

#define SHA_LONG unsigned int
#define SHA_LBLOCK  16
#define SHF_LENGTH_PAD  8
#define SHF_PAD_WORDS 2

typedef struct SHA256state_st {
    SHA_LONG h[8];
    SHA_LONG Nl, Nh;
    SHA_LONG data[SHA_LBLOCK];
    unsigned int num, md_len;
} SHA256_CTX;


#define che_key_action(che_context, key_act, key, key_len)                 \
                                                                               \
       ((void)TRICK_##key_act, run_##key_act##_process(che_context, key, key_len))
#define che_process(che_context, type, mode, act, source, dest, source_length, final)                       \
                                                                                                                     \
       ((void)TRICK_##type, run_##type##_process(che_context, type, mode, act, source, dest, source_length, final, 0))

#define TRICK_CHE_SET_KEY                  1

void che_init(STCHE *che_context, kal_uint32 type);

void che_deinit(STCHE *che_context);

void run_CHE_SET_KEY_process(STCHE *che_context, kal_uint8 *key, kal_int32 key_len);

kal_bool run_CHE_SHA1_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL);

kal_bool run_CHE_RC4_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL);

kal_bool run_CHE_AES_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL);

kal_bool run_CHE_MAC_MD5_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL);

kal_bool run_CHE_MAC_SHA1_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL);

void che_set_iv(STCHE *che_context, kal_uint8 *iv, kal_uint32 iv_len);

kal_bool run_CHE_MD5_process(
    STCHE *che_context,
    CHE_TYPE type,
    CHE_OPERATION_MODE mode,
    CHE_ACTION act,
    kal_uint8 *source,
    kal_uint8 *dest,
    kal_uint32 source_length,
    kal_bool last_block,
    void *pstFSAL);

#endif

