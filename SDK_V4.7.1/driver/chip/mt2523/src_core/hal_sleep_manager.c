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

#include "hal_sys_topsm.h"
#include "hal_cm4_topsm.h"
#include "hal_dsp_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "hal_dcxo.h"
#include "hal_sleep_driver.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_pmu.h"
#include "string.h"
#include "hal_rtc.h"
#include "hal_pmu_wrap_interface.h"
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */

extern cm4_topsm_mtcmos_control mtcmos_control;
extern bool BT_workaround;
uint8_t sleep_manager_handle;
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;

void Deep_Sleep_GPT_CB()
{
    hal_gpt_stop_timer(DEEP_SLEEP_GPT);
}

uint8_t hal_sleep_manager_set_sleep_handle(const char *handle_name)
{
    uint8_t index;
    index = sleepdrv_get_handle(handle_name, strlen(handle_name));
    return index;
}

hal_sleep_manager_status_t hal_sleep_manager_lock_sleep(uint8_t handle_index)
{
    sleepdrv_hold_sleep_lock(handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_unlock_sleep(uint8_t handle_index)
{
    sleepdrv_release_sleep_lock(handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_release_sleep_handle(
    uint8_t handle)
{
    sleepdrv_release_handle(handle);
    return HAL_SLEEP_MANAGER_OK;
}

uint32_t hal_sleep_manager_get_lock_status(void)
{
    return sleepdrv_get_lock_index();
}

uint32_t hal_sleep_manager_sleep_driver_dump_handle_name(void)
{
    return sleepdrv_get_lock_module();
}

bool hal_sleep_manager_is_sleep_locked(void)
{
    return sleepdrv_check_sleep_locks();
}

bool hal_sleep_manager_is_sleep_handle_alive(uint8_t handle_index)
{
    return sleepdrv_get_handle_status(handle_index);
}

#ifdef HAL_SLEEP_MANAGER_SUPPORT_POWER_OFF
void hal_sleep_manager_enter_power_off_mode()
{
    hal_rtc_deinit();
    pmu_enter_power_off();
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
        log_hal_info("ERROR : Deep Sleep GPT Start Fail");
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
    } else if ((mode == HAL_SLEEP_MODE_SLEEP) || (mode == HAL_SLEEP_MODE_DEEP_SLEEP)) {
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("dpm",SA_START);
#endif /* MTK_SWLA_ENABLE */
        if (FAST_WAKEUP_VERIFICATION) {
            mode = HAL_SLEEP_MODE_SLEEP;
        }
        cm4_topsm_enter_suspend(mode);
        hal_gpt_stop_timer(DEEP_SLEEP_GPT);
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("dpm",SA_STOP);
#endif /* MTK_SWLA_ENABLE */
    }
}

#ifdef HAL_SLEEP_MANAGER_SUPPORT_WAKEUP_SOURCE_CONFIG
hal_sleep_manager_status_t hal_sleep_manager_enable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    cm4_topsm_wakeup_set_one_irq_enable(pin);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_disable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    cm4_topsm_wakeup_set_one_irq_disable(pin);
    return HAL_SLEEP_MANAGER_OK;
}
#endif

hal_sleep_manager_status_t hal_sleep_manager_init()
{
    log_hal_info("hal_sleep_manager_init start\n");
    pmu_init();
    cm4_topsm_init();
    mtcmos_control.BT_Count = 0;
    mtcmos_control.MD2G_Count = 0;
    mtcmos_control.MM_Count = 0;
    BT_workaround = true;
    dsp_topsm_init();
    sys_topsm_init();
    sleep_driver_init();
    cm4_topsm_sleep_default_callback_function();
    sleep_manager_handle = hal_sleep_manager_set_sleep_handle("sleep_manager_handle");

    if (hal_gpt_init(DEEP_SLEEP_GPT) != HAL_GPT_STATUS_OK) {
        log_hal_info("ERROR : Deep Sleep GPT Init Fail");
        return HAL_SLEEP_MANAGER_ERROR;
    }

    return HAL_SLEEP_MANAGER_OK;
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
