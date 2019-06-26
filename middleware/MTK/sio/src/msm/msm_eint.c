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
#include "hal_eint.h"
#include "hal_gpio.h"
#include "msm.h"

#ifdef MSM_PRINTF
#define LOGE(fmt,arg...)   printf(("[MSM EINT]: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("[MSM EINT]: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("[MSM EINT]: "fmt), ##arg)
#else
log_create_module(msm, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(msm, "[MSM EINT]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(msm, "[MSM EINT]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(msm ,"[MSM EINT]: "fmt,##arg)
#endif

#define MSM_EINT_SUPPORT_EVENT_HANDLE_NUMBER 10

typedef enum {
    MSM_EINT_MODEM_EXCETION = 0,
    MSM_EINT_HOST_WAKEUP,
    MSM_EINT_MAX_NUMBER
} msm_eint_t;

typedef struct msm_eint_context_struct {
    uint32_t                 mutex;
    uint32_t                 semaphore;    
    uint32_t                 cb_count;
    msm_event_handler        event_cb_list[MSM_EINT_SUPPORT_EVENT_HANDLE_NUMBER]; 
} msm_eint_context_struct;

uint32_t g_msm_pin_config;
uint32_t g_msm_exception_pin_parameter;

static msm_eint_t msm_eint[] = {MSM_EINT_MODEM_EXCETION, MSM_EINT_HOST_WAKEUP,};

static msm_eint_context_struct msm_eint_cnt = {0};

#define MD_WAKEUP_EINT_STATE       0;  // 0:LEVEL_LOW-> wakeup, 1:LEVEL_HIGHT -> sleep
static bool md_wakeup_eint_state = (bool)MD_WAKEUP_EINT_STATE;

static msm_ret_t msm_eint_init();
static msm_ret_t msm_eint_register_callback(msm_event_handler event_handler);
static void msm_eint_callback(void *user_data);
static msm_ret_t msm_eint_notify(msm_event_t event);
static msm_ret_t msm_eint_trigger_wakeup_from_modem_init(void);
static msm_ret_t msm_eint_notify_exception_from_modem_init(void);

extern void msm_gpio_set_modem_status(msm_status_t status);

msm_ret_t msm_init(uint32_t pin_config, uint32_t exception_eint_mode_parameter)
{    
    g_msm_pin_config = pin_config;
    g_msm_exception_pin_parameter = exception_eint_mode_parameter;
    
    msm_eint_init();

    return MSM_RET_OK;
}

msm_ret_t msm_register_callback(msm_event_handler event_handler)
{
    return msm_eint_register_callback(event_handler);
}
        
static msm_ret_t msm_eint_init()
{        
    if (MSM_RET_OK != msm_eint_trigger_wakeup_from_modem_init()) {
        return MSM_RET_ERROR;
    }
    if (MSM_RET_OK != msm_eint_notify_exception_from_modem_init()) {
        return MSM_RET_ERROR;
    }

    LOGI("msm_eint_init, success \r\n");
    return MSM_RET_OK;
}

msm_ret_t msm_eint_register_callback(msm_event_handler event_handler)
{
    if (msm_eint_cnt.cb_count == MSM_EINT_SUPPORT_EVENT_HANDLE_NUMBER)
        return MSM_RET_ERROR;

    msm_eint_cnt.event_cb_list[msm_eint_cnt.cb_count] = event_handler;
    msm_eint_cnt.cb_count++;
    
    return MSM_RET_OK;
}

static msm_ret_t msm_eint_notify_exception_from_modem_init(void)
{
    hal_eint_config_t eint_config;

    if (!(g_msm_pin_config & MSM_NOTITF_MODEM_EXCEPTION_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    hal_eint_mask(MODEM_EXCEPTION_EINT);
    
    if (g_msm_exception_pin_parameter & MSM_EXCEPTION_PIN_EDGE_TRIGGER)
        eint_config.trigger_mode = HAL_EINT_EDGE_RISING;
    else 
        eint_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    
    //eint_config.debounce_time = 1;
    eint_config.debounce_time = 10;
    
    if (HAL_EINT_STATUS_OK != hal_eint_init(MODEM_EXCEPTION_EINT, &eint_config)) {
        return MSM_RET_ERROR;
    }

    if (HAL_EINT_STATUS_OK != hal_eint_register_callback(MODEM_EXCEPTION_EINT, msm_eint_callback, &msm_eint[0])) {
        return MSM_RET_ERROR;
    }
    hal_eint_unmask(MODEM_EXCEPTION_EINT);
    LOGI("msm_eint_notify_exception_from_modem_init, success \r\n");

    return MSM_RET_OK;
}

static msm_ret_t msm_eint_trigger_wakeup_from_modem_init(void)
{
    hal_eint_config_t eint_config;

    if (!(g_msm_pin_config & MSM_NOTITF_MODEM_WAKEUP_PIN_SUPPORTED)){
        return MSM_RET_OK;
    }

    hal_eint_mask(MODEM_WAKEUP_EINT);
    md_wakeup_eint_state = MD_WAKEUP_EINT_STATE;
    eint_config.trigger_mode = HAL_EINT_EDGE_RISING;
    eint_config.debounce_time = 10;
    if (HAL_EINT_STATUS_OK != hal_eint_init(MODEM_WAKEUP_EINT, &eint_config)) {
        return MSM_RET_ERROR;
    }

    if (HAL_EINT_STATUS_OK != hal_eint_register_callback(MODEM_WAKEUP_EINT, msm_eint_callback, &msm_eint[1])) {
        return MSM_RET_ERROR;
    }
    hal_eint_unmask(MODEM_WAKEUP_EINT);
    LOGI("msm_eint_trigger_wakeup_from_modem_init, success \r\n");

    return MSM_RET_OK;
}

static void msm_eint_callback(void *user_data)
{
    msm_eint_t *eint = (msm_eint_t *)user_data;

    LOGI("msm_eint_callback eint: %d \r\n", *eint);

    if (*eint == msm_eint[0]) {
        hal_eint_mask(MODEM_EXCEPTION_EINT);
        msm_eint_notify(MSM_EVENT_EXCETION);
        hal_eint_unmask(MODEM_EXCEPTION_EINT);
    } else if (*eint == msm_eint[1]) {
        hal_eint_config_t eint_config;
        hal_eint_trigger_mode_t trigger_mode;
        hal_eint_mask(MODEM_WAKEUP_EINT);
        if (md_wakeup_eint_state) {
            LOGI("[MSM] modem wakeup");
        msm_eint_notify(MSM_EVENT_WAKEUP);
        } else {
            LOGI("[MSM] modem sleep");
            msm_gpio_set_modem_status(MSM_STATUS_INACTIVE);              
        }

        md_wakeup_eint_state = (bool)!md_wakeup_eint_state;

        if (md_wakeup_eint_state) {
            trigger_mode = HAL_EINT_EDGE_FALLING;
        } else {
            trigger_mode = HAL_EINT_EDGE_RISING;
        }
        
        eint_config.trigger_mode = trigger_mode;
        eint_config.debounce_time = 10;
        if (HAL_EINT_STATUS_OK != hal_eint_init(MODEM_WAKEUP_EINT, &eint_config)) {
            return;
        }
        
        if (HAL_EINT_STATUS_OK != hal_eint_register_callback(MODEM_WAKEUP_EINT, msm_eint_callback, &msm_eint[1])) {
            return;
        }
        hal_eint_unmask(MODEM_WAKEUP_EINT);
    }
}

static msm_ret_t msm_eint_notify(msm_event_t event)
{
    uint32_t i = 0;
    for (i = 0; i < msm_eint_cnt.cb_count; i++) {
        if (msm_eint_cnt.event_cb_list[i](event, NULL) == 1)
            break;
    }
    
    return MSM_RET_OK;
}

