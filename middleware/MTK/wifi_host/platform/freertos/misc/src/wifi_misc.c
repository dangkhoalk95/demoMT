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
#include "hal.h"
#include "assert.h"
#include "hal_log.h"
#include "hal_eint_internal.h"
#include "wifi_misc.h"


extern const unsigned char BSP_WIFI_EINT;
extern const char BSP_WIFI_32K_PIN;
extern const char BSP_WIFI_26M_PIN;
extern const char BSP_WIFI_RESET_PIN;
extern const char BSP_WIFI_ENABLE_PIN;

void wifi_register_eint_callback(void *call_back)
{
    hal_eint_config_t config;

    if (0xff == BSP_WIFI_EINT) {
        log_hal_error("wifi eint pin not config. \r\n");
        assert(0);
    }

    config.debounce_time = 2;
    //config.debounce_time = 0;
    config.trigger_mode = HAL_EINT_LEVEL_LOW;

    hal_eint_mask((hal_eint_number_t)BSP_WIFI_EINT);
    hal_eint_init((hal_eint_number_t)BSP_WIFI_EINT, &config);

    hal_eint_register_callback((hal_eint_number_t)BSP_WIFI_EINT, (hal_eint_callback_t)call_back, NULL);

    /*Set new debounce value to 2*(1/32K) = 0.0625ms.*/
    hal_eint_set_debounce_count((hal_eint_number_t)BSP_WIFI_EINT, 2);

    hal_eint_unmask((hal_eint_number_t)BSP_WIFI_EINT);
}

void wifi_eint_mask(void)
{
    hal_eint_mask((hal_eint_number_t)BSP_WIFI_EINT);
    return;
}


void wifi_eint_unmask(void)
{
    hal_eint_unmask((hal_eint_number_t)BSP_WIFI_EINT);
    return;
}


void wifi_chip_hw_power_on(void)
{
    /*wifi control pin config.*/

    /*1, clock(32k or 26m) config in ept.*/
	
    if ((0xff == BSP_WIFI_ENABLE_PIN) ||
            (0xff == BSP_WIFI_RESET_PIN)) {
        log_hal_error("wifi pin not config. \r\n");
        assert(0);
    }

	/*2, enable wifi.*/
    hal_gpt_delay_ms(10);
    hal_gpio_set_output(BSP_WIFI_ENABLE_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW

    hal_gpio_set_output(BSP_WIFI_RESET_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW

    hal_gpio_set_output(BSP_WIFI_ENABLE_PIN, HAL_GPIO_DATA_HIGH); //OUTPUT HIGH
    hal_gpt_delay_ms(15);

    hal_gpio_set_output(BSP_WIFI_RESET_PIN, HAL_GPIO_DATA_HIGH); //OUTPUT HIGH
    hal_gpt_delay_ms(10);

	
    return;
}


void wifi_chip_hw_power_off(void)
{
    wifi_eint_mask();

	if ((0xff == BSP_WIFI_ENABLE_PIN) ||
            (0xff == BSP_WIFI_RESET_PIN)) {
        log_hal_error("wifi pin not config. \r\n");
        assert(0);
    }

    hal_gpio_set_output(BSP_WIFI_RESET_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW
    hal_gpio_set_output(BSP_WIFI_ENABLE_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW

    /*need add disable clock for power.*/

}


