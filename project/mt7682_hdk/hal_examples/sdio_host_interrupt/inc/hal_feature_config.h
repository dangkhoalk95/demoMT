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
 
#ifndef __HAL_FEATURE_CONFIG_H__
#define __HAL_FEATURE_CONFIG_H__


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* module ON or OFF feature option,only option in this temple
*****************************************************************************/

#define HAL_ADC_MODULE_ENABLED
#define HAL_AES_MODULE_ENABLED
#define HAL_CACHE_MODULE_ENABLED
#define HAL_CLOCK_MODULE_ENABLED
#define HAL_DES_MODULE_ENABLED
#define HAL_EINT_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GDMA_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_GPT_MODULE_ENABLED
#define HAL_I2C_MASTER_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_MD5_MODULE_ENABLED
#define HAL_MPU_MODULE_ENABLED
#define HAL_NVIC_MODULE_ENABLED
#define HAL_PWM_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SHA_MODULE_ENABLED
#define HAL_SPI_MASTER_MODULE_ENABLED
#define HAL_SPI_SLAVE_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_WDT_MODULE_ENABLED
#define HAL_DWT_MODULE_ENABLED
//#define HAL_SD_MODULE_ENABLED
#define HAL_SDIO_MODULE_ENABLED
//#define HAL_SDIO_SLAVE_MODULE_ENABLED
#define HAL_TRNG_MODULE_ENABLED
#define HAL_PMU_MODULE_ENABLED
#define HAL_DCXO_MODULE_ENABLED
#define HAL_SLEEP_MANAGER_ENABLED

#define MTK_HAL_PLAIN_LOG_ENABLE

/*This definition determines whether we can use the sub_feature of HAL_SD_CARD_DETECTION, which supports SD card detection if this module is enabled.*/
#ifdef HAL_SD_MODULE_ENABLED

/*Support SD card detection. If this feature is used, the corresponding EINT pin should be cofingured in the EPT tool.*/
/*#define HAL_SD_CARD_DETECTION*/

#endif/*#ifdef HAL_SD_MODULE_ENABLED*/

/*****************************************************************************
 * * customization module feature option
 * *****************************************************************************/
 
 
#ifdef __cplusplus
}
#endif

#endif /* __HAL_FEATURE_CONFIG_H__ */

