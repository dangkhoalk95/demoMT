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

#ifndef __MSM_H__
#define __MSM_H__

#include "hal_platform.h"

#define SKIP_PINMUX
#define MODEM_GPIO_NAME_BY_DWS

extern const char TRIGGER_MODEM_RESET_PIN;
extern const char UPDATE_HOST_STATUS_PIN;
extern const char QUERY_MODEM_STATUS_PIN;
extern const char TRIGGER_MODEM_WAKEUP_PIN;
 
extern const char MODEM_WAKEUP_EINT;
extern const char MODEM_EXCEPTION_EINT;

#define TRIGGER_MODEM_POWERON_PIN           HAL_GPIO_25

typedef enum {
    MSM_RET_ERROR = -1,
    MSM_RET_OK = 0,
} msm_ret_t;

typedef enum {
    MSM_STATUS_ACTIVE = 0,
    MSM_STATUS_INACTIVE,
    MSM_STATUS_ERR
} msm_status_t;

typedef enum {
    MSM_TRIGGER_TYPE_RESET = 0,
    MSM_TRIGGER_TYPE_WAKEUP,
    MSM_TRIGGER_TYPE_SLEEP,
    MSM_TRIGGER_TYPE_POWERON,
    MSM_TRIGGER_TYPE_POWEROFF
} msm_trigger_modem_t;

typedef enum {
    MSM_EVENT_EXCETION = 0,
    MSM_EVENT_WAKEUP,
    MSM_EVENT_SLEEP,
    MSM_EVENT_MAX_NUMBER
} msm_event_t;

typedef enum {
    MSM_NOTITF_MODEM_WAKEUP_PIN_SUPPORTED =    0X0002,
    MSM_NOTITF_MODEM_EXCEPTION_PIN_SUPPORTED = 0X0004,
    MSM_TRIGGER_MODEM_RESET_PIN_SUPPORTED =    0X0008,
    MSM_UPDATE_HOST_STATUS_PIN_SUPPORTED =     0X0010,
    MSM_QUERY_MODEM_STATUS_PIN_SUPPORTED =     0X0020,
    MSM_TRIGGER_MODEM_WAKEUP_PIN_SUPPORTED =   0X0040, 
    MSM_TRIGGER_MODEM_PWERON_PIN_SUPPORTED =   0X0080
} msm_pin_config_t;

typedef enum {    
    MSM_EXCEPTION_PIN_EDGE_TRIGGER = 0X0001,
    MSM_EXCEPTION_PIN_LEVEL_TRIGGER = 0X0002,
} msm_eint_mode_config_t;

typedef int32_t (* msm_event_handler)(msm_event_t event, void *data);

extern msm_ret_t msm_init(uint32_t pin_config, uint32_t exception_eint_mode_parameter);
extern msm_ret_t msm_register_callback(msm_event_handler event_handler);
extern msm_ret_t msm_trigger_to_modem(msm_trigger_modem_t type);
extern msm_status_t msm_query_status_from_modem(void);
extern msm_ret_t msm_notify_host_status_to_modem(msm_status_t status);
#endif /* __MSM_H__ */

