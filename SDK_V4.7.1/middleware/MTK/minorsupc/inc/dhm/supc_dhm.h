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


#ifndef XYSSL_DHM_H
#define XYSSL_DHM_H

#include "supc_bignum.h"

#define XYSSL_ERR_DHM_BAD_INPUT_DATA                    -0x0480
#define XYSSL_ERR_DHM_READ_PARAMS_FAILED                -0x0490
#define XYSSL_ERR_DHM_MAKE_PARAMS_FAILED                -0x04A0
#define XYSSL_ERR_DHM_READ_PUBLIC_FAILED                -0x04B0
#define XYSSL_ERR_DHM_MAKE_PUBLIC_FAILED                -0x04C0
#define XYSSL_ERR_DHM_CALC_SECRET_FAILED                -0x04D0

typedef struct {
    int len;    /*!<  size(P) in chars  */
    mpi P;      /*!<  prime modulus     */
    mpi G;      /*!<  generator         */
    mpi X;      /*!<  secret value      */
    mpi GX;     /*!<  self = G^X mod P  */
    mpi GY;     /*!<  peer = G^Y mod P  */
    mpi K;      /*!<  key = GY^X mod P  */
    mpi RP;     /*!<  cached R^2 mod P  */
} dhm_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Parse the ServerKeyExchange parameters
 *
 * \param ctx      DHM context
 * \param p        &(start of input buffer)
 * \param end      end of buffer
 *
 * \return         0 if successful, or an XYSSL_ERR_DHM_XXX error code
 */
int dhm_read_params(dhm_context *ctx,
                    unsigned char **p,
                    unsigned char *end);

/**
 * \brief          Setup and write the ServerKeyExchange parameters
 *
 * \param ctx      DHM context
 * \param x_size   private value size in bits
 * \param output   destination buffer
 * \param olen     number of chars written
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 *
 * \note           This function assumes that ctx->P and ctx->G
 *                 have already been properly set (for example
 *                 using che_mpi_read_string or che_mpi_read_binary).
 *
 * \return         0 if successful, or an XYSSL_ERR_DHM_XXX error code
 */
int dhm_make_params(dhm_context *ctx, int s_size,
                    unsigned char *output, int *olen,
                    int (*f_rng)(void *), void *p_rng);

int dhm_make_params_internal(dhm_context *ctx, int x_size,
                             int (*f_rng)(void *), void *p_rng);

/**
 * \brief          Import the peer's public value G^Y
 *
 * \param ctx      DHM context
 * \param input    input buffer
 * \param ilen     size of buffer
 *
 * \return         0 if successful, or an XYSSL_ERR_DHM_XXX error code
 */
int dhm_read_public(dhm_context *ctx,
                    unsigned char *input, int ilen);

/**
 * \brief          Create own private value X and export G^X
 *
 * \param ctx      DHM context
 * \param x_size   private value size in bits
 * \param output   destination buffer
 * \param olen     must be equal to ctx->P.len
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 *
 * \return         0 if successful, or an XYSSL_ERR_DHM_XXX error code
 */
int dhm_make_public(dhm_context *ctx, int s_size,
                    unsigned char *output, int olen,
                    int (*f_rng)(void *), void *p_rng);
int dhm_make_public_internal(dhm_context *ctx, int x_size,
                             int (*f_rng)(void *), void *p_rng);

/**
 * \brief          Derive and export the shared secret (G^Y)^X mod P
 *
 * \param ctx      DHM context
 * \param output   destination buffer
 * \param olen     number of chars written
 *
 * \return         0 if successful, or an XYSSL_ERR_DHM_XXX error code
 */
int dhm_calc_secret(dhm_context *ctx,
                    unsigned char *output, int *olen);
int dhm_calc_secret_internal(dhm_context *ctx);

/*
 * \brief          Free the components of a DHM key
 */
void dhm_free(dhm_context *ctx);

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int dhm_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif
