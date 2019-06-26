/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#ifndef __SECUTIRY_H__
#define __SECURITY_H__
#include "hal_platform.h"

#ifdef HAL_SECURITY_MODULE_ENABLED

#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief     This function to know whether secure boot is enable or not
 * @param[out]  pointer to store enable bit
 * @return  void
 * @sa     #security_sbc_enable
 *
 */
void security_sbc_enable(uint32_t *enable);

/**
 * @brief     This function to read RSA public key hash from eFuse
 * @param[in] pointer to buffer used to store hash value 
 * @param[in]  size of hash value
 * @param[in]  system may has more than one key hash. to indicate which set of hash to read.
 * @return  void
 *
 */
void security_key_hash(uint32_t *phash, uint32_t size, uint32_t set);

/**
 * @brief     the function to read the selected algorithm for secure boot in device
 * @return  #HAL_SBC_NO, secure boot is not enabled and not seclect any algorithm
 *          #HAL_SBC_MAC, mediatek proprietary MAC
 *          #HAL_SBC_AES_CMAC, AES-CMAC(RFC 4493)
 *          #HAL_SBC_RSA, RSASSA-PSS(RFC 3447)
 *
 */
hal_sbc_algo_t  security_sbc_algo(void);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_SECURITY_MODULE_ENABLED*/

#endif /* __SECURITY_H__ */

