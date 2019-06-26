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

#include "hal_sleep_manager.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "hal_spm.h"
#include "hal_log.h"
#include "hal_gpt.h"
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */

void Deep_Sleep_GPT_CB()
{
    hal_gpt_stop_timer(DEEP_SLEEP_GPT);
}

uint8_t hal_sleep_manager_set_sleep_handle(const char *handle_name)
{
    uint8_t index;
    index = sleep_management_get_lock_handle(handle_name);
    return index;
}

hal_sleep_manager_status_t hal_sleep_manager_lock_sleep(uint8_t handle_index)
{
    sleep_management_lock_sleep(LOCK_SLEEP, handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_unlock_sleep(uint8_t handle_index)
{
    sleep_management_lock_sleep(UNLOCK_SLEEP, handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_release_sleep_handle(uint8_t handle_index)
{
    if (hal_sleep_manager_get_lock_status() & (1 << handle_index)) {
        log_hal_error("handle %d is currently holding a lock, cannot release\n", handle_index);
        return HAL_SLEEP_MANAGER_ERROR;
    }
    sleep_management_release_lock_handle(handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

uint32_t hal_sleep_manager_get_lock_status(void)
{
    return sleep_management_get_lock_sleep_request_info();
}

uint32_t hal_sleep_manager_sleep_driver_dump_handle_name(void)
{
    return sleep_management_get_lock_sleep_handle_list();
}

bool hal_sleep_manager_is_sleep_locked(void)
{
    return sleep_management_check_sleep_locks();
}

bool hal_sleep_manager_is_sleep_handle_alive(uint8_t handle_index)
{
    return sleep_management_check_handle_status(handle_index);
}

#ifdef HAL_SLEEP_MANAGER_SUPPORT_POWER_OFF
void hal_sleep_manager_enter_power_off_mode()
{

}
#endif

hal_sleep_manager_status_t hal_sleep_manager_set_sleep_time(uint32_t sleep_time_ms)
{
    hal_gpt_status_t         ret_status;
    hal_gpt_register_callback(DEEP_SLEEP_GPT, (hal_gpt_callback_t)Deep_Sleep_GPT_CB, NULL);
    if (sleep_time_ms > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        sleep_time_ms = HAL_GPT_MAXIMUM_MS_TIMER_TIME;
    }
    ret_status = hal_gpt_start_timer_ms(DEEP_SLEEP_GPT, sleep_time_ms, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    if (ret_status != HAL_GPT_STATUS_OK) {
        log_hal_error("ERROR : Deep Sleep GPT Start Fail");
        return HAL_SLEEP_MANAGER_ERROR;
    }

    return HAL_SLEEP_MANAGER_OK;
}

void hal_sleep_manager_enter_sleep_mode(hal_sleep_mode_t mode)
{
    if (mode == HAL_SLEEP_MODE_IDLE) {
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("wfi",SA_START);
#endif /* MTK_SWLA_ENABLE */
        __asm volatile("dsb");
        __asm volatile("wfi");
        __asm volatile("isb");
        hal_gpt_stop_timer(DEEP_SLEEP_GPT);
        #ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("wfi",SA_STOP);
        #endif /* MTK_SWLA_ENABLE */
    } else if (mode == HAL_SLEEP_MODE_SLEEP) {
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("dpm",SA_START);
#endif /* MTK_SWLA_ENABLE */
        sleep_management_enter_deep_sleep(mode);
        hal_gpt_stop_timer(DEEP_SLEEP_GPT);
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("dpm",SA_STOP);
#endif /* MTK_SWLA_ENABLE */
    } 
}

#ifdef HAL_SLEEP_MANAGER_SUPPORT_WAKEUP_SOURCE_CONFIG
hal_sleep_manager_status_t hal_sleep_manager_enable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    spm_unmask_wakeup_source(pin);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_disable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    spm_mask_wakeup_source(pin);
    return HAL_SLEEP_MANAGER_OK;
}
#endif


hal_sleep_manager_status_t hal_sleep_manager_init()
{
    //log_hal_info("hal_sleep_manager_init start\n");

    sleep_management_low_power_init_setting();

    spm_init(0);

    #if (PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 7686)
    spm_control_mtcmos(SPM_MTCMOS_CONN      ,SPM_MTCMOS_PWR_DISABLE);
    #endif
    spm_control_mtcmos(SPM_MTCMOS_SDIO_SLV  ,SPM_MTCMOS_PWR_DISABLE);
    
    if (hal_gpt_init(DEEP_SLEEP_GPT) != HAL_GPT_STATUS_OK) {
        log_hal_error("ERROR : Deep Sleep GPT Init Fail");
        return HAL_SLEEP_MANAGER_ERROR;
    }

    return HAL_SLEEP_MANAGER_OK;
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
