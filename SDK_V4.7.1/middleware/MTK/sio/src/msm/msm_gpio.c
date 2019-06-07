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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "syslog.h"

#include "hal_platform.h"
#include "hal_gpio.h"
#include "msm.h"
#include "hal_gpt.h"

#ifdef MSM_PRINTF
#define LOGE(fmt,arg...)   printf(("[MSM GPIO]: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("[MSM GPIO]: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("[MSM GPIO]: "fmt), ##arg)
#else
log_create_module(msm_gpio, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(msm_gpio, "[MSM GPIO]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(msm_gpio, "[MSM GPIO]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(msm_gpio ,"[MSM GPIO]: "fmt,##arg)
#endif

extern uint32_t g_msm_pin_config;
extern uint32_t g_msm_exception_pin_parameter;

msm_status_t g_msm_status;

static msm_ret_t msm_gpio_trigger_reset_to_modem(void);
static msm_ret_t msm_gpio_trigger_wakeup_to_modem(void);
static msm_ret_t msm_gpio_trigger_sleep_to_modem(void);

msm_ret_t msm_notify_host_status_to_modem(msm_status_t status)
{
    hal_gpio_status_t ret;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif

    if (!(g_msm_pin_config & MSM_UPDATE_HOST_STATUS_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    ret = hal_gpio_init(UPDATE_HOST_STATUS_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(UPDATE_HOST_STATUS_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_RET_ERROR;
    }
#endif
    ret = hal_gpio_set_direction(UPDATE_HOST_STATUS_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    if (MSM_STATUS_ACTIVE == status) {
        ret = hal_gpio_set_output(UPDATE_HOST_STATUS_PIN, HAL_GPIO_DATA_HIGH);
    } else {
        ret = hal_gpio_set_output(UPDATE_HOST_STATUS_PIN, HAL_GPIO_DATA_LOW);
    }
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_deinit(UPDATE_HOST_STATUS_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    LOGI("msm_notify_host_status_to_modem, success. status:%d\r\n", status);

    return MSM_RET_OK;
}

static msm_ret_t msm_gpio_trigger_reset_to_modem(void)
{
    hal_gpio_status_t ret;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif

    if (!(g_msm_pin_config & MSM_TRIGGER_MODEM_RESET_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    ret = hal_gpio_init(TRIGGER_MODEM_RESET_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(TRIGGER_MODEM_RESET_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_RET_ERROR;
    }
#endif
    ret = hal_gpio_set_direction(TRIGGER_MODEM_RESET_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_set_output(TRIGGER_MODEM_RESET_PIN, HAL_GPIO_DATA_LOW);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    hal_gpt_delay_ms(1000);

    ret = hal_gpio_set_output(TRIGGER_MODEM_RESET_PIN, HAL_GPIO_DATA_HIGH);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_deinit(TRIGGER_MODEM_RESET_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    LOGI("msm_gpio_trigger_reset_to_modem, success\r\n");

    return MSM_RET_OK;
}

static msm_ret_t msm_gpio_trigger_power_on_modem(void)
{
#if 0
    hal_gpio_status_t ret;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif

    if (!(g_msm_pin_config & MSM_TRIGGER_MODEM_PWERON_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    ret = hal_gpio_init(TRIGGER_MODEM_POWERON_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(TRIGGER_MODEM_POWERON_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_RET_ERROR;
    }
#endif

    ret = hal_gpio_set_direction(TRIGGER_MODEM_POWERON_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
   
    ret = hal_gpio_set_output(TRIGGER_MODEM_POWERON_PIN, HAL_GPIO_DATA_HIGH);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_deinit(TRIGGER_MODEM_POWERON_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    LOGI("msm_gpio_trigger_power_on_modem, success\r\n");
#endif

    return MSM_RET_OK;
}

static msm_ret_t msm_gpio_trigger_power_off_modem(void)
{
#if 0
    hal_gpio_status_t ret;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif

    if (!(g_msm_pin_config & MSM_TRIGGER_MODEM_PWERON_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    ret = hal_gpio_init(TRIGGER_MODEM_POWERON_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(TRIGGER_MODEM_POWERON_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_RET_ERROR;
    }
#endif

    ret = hal_gpio_set_direction(TRIGGER_MODEM_POWERON_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
    
    ret = hal_gpio_set_output(TRIGGER_MODEM_POWERON_PIN, HAL_GPIO_DATA_LOW);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_deinit(TRIGGER_MODEM_POWERON_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    LOGI("msm_gpio_trigger_power_on_modem, success\r\n");
    
#endif

    return MSM_RET_OK;
}

msm_ret_t msm_trigger_to_modem(msm_trigger_modem_t type)
{
    LOGI("msm_trigger_to_modem, type = %d. \r\n", type);
    
    if (type == MSM_TRIGGER_TYPE_RESET)
        return msm_gpio_trigger_reset_to_modem();
    else if (type == MSM_TRIGGER_TYPE_WAKEUP)
        return msm_gpio_trigger_wakeup_to_modem(); 
    else if (type == MSM_TRIGGER_TYPE_SLEEP)
        return msm_gpio_trigger_sleep_to_modem();  
    else if (type == MSM_TRIGGER_TYPE_POWERON)
        return msm_gpio_trigger_power_on_modem();
    else if (type == MSM_TRIGGER_TYPE_POWEROFF)
        return msm_gpio_trigger_power_off_modem();
    return MSM_RET_OK;
}


static msm_ret_t msm_gpio_trigger_wakeup_to_modem(void)
{
    hal_gpio_status_t ret;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif

    if (!(g_msm_pin_config & MSM_TRIGGER_MODEM_WAKEUP_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    ret = hal_gpio_init(TRIGGER_MODEM_WAKEUP_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(TRIGGER_MODEM_WAKEUP_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_RET_ERROR;
    }
#endif
    ret = hal_gpio_set_direction(TRIGGER_MODEM_WAKEUP_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_set_output(TRIGGER_MODEM_WAKEUP_PIN, HAL_GPIO_DATA_LOW);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_deinit(TRIGGER_MODEM_WAKEUP_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    g_msm_status = MSM_STATUS_ACTIVE;
 
    LOGI("msm_gpio_trigger_wakeup_to_modem, success\r\n");

    return MSM_RET_OK;
}

static msm_ret_t msm_gpio_trigger_sleep_to_modem(void)
{
    hal_gpio_status_t ret;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif

    if (!(g_msm_pin_config & MSM_TRIGGER_MODEM_WAKEUP_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    ret = hal_gpio_init(TRIGGER_MODEM_WAKEUP_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(TRIGGER_MODEM_WAKEUP_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_RET_ERROR;
    }
#endif
    ret = hal_gpio_set_direction(TRIGGER_MODEM_WAKEUP_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_set_output(TRIGGER_MODEM_WAKEUP_PIN, HAL_GPIO_DATA_HIGH);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }

    ret = hal_gpio_deinit(TRIGGER_MODEM_WAKEUP_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_RET_ERROR;
    }
 
    g_msm_status = MSM_STATUS_INACTIVE;
    
    LOGI("msm_gpio_trigger_sleep_to_modem, success\r\n");

    return MSM_RET_OK;
}

void msm_gpio_set_modem_status(msm_status_t status)
{
    g_msm_status = status;
}

msm_status_t msm_query_status_from_modem(void)
{
    return g_msm_status;
#if 0
    hal_gpio_status_t ret;
    msm_status_t status = MSM_STATUS_ACTIVE;
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    hal_pinmux_status_t ret_pinmux_status;
#endif
    hal_gpio_data_t gpio_data;

    if (!(g_msm_pin_config & MSM_QUERY_MODEM_STATUS_PIN_SUPPORTED)){
        return MSM_STATUS_ACTIVE;
    }
    
    ret = hal_gpio_init(QUERY_MODEM_STATUS_PIN);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_STATUS_ERR;
    }
#ifdef SKIP_PINMUX // don't need to set GPIO mode again
    ret_pinmux_status = hal_pinmux_set_function(QUERY_MODEM_STATUS_PIN, 0);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        return MSM_STATUS_ERR;
    }
#endif
    ret = hal_gpio_set_direction(QUERY_MODEM_STATUS_PIN, HAL_GPIO_DIRECTION_INPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_STATUS_ERR;
    }

    ret = hal_gpio_get_input(QUERY_MODEM_STATUS_PIN, &gpio_data);
    if (HAL_GPIO_STATUS_OK != ret) {
        return MSM_STATUS_ERR;
    }

    if (HAL_GPIO_DATA_HIGH == gpio_data) {
        status = MSM_STATUS_ACTIVE;
    } else {
        status = MSM_STATUS_INACTIVE;
    }
    LOGI("msm_query_status_from_modem, ret = %d\r\n", status);

    return status;
#endif
}
