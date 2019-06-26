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

#include "hal_sleep_manager_platform.h"
#include "hal_sleep_driver.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "string.h"
#include "hal_lp.h"
#include "top.h"
#include "connsys_driver.h"
#include "spi_flash.h"
#include "timer.h"
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */

uint8_t sleep_manager_handle;
hal_gpt_port_t wakeup_gpt_port = HAL_GPT_0;
void (*ptr_tickless_cb)(void *) = NULL;
extern void hal_module_sleep_register_callback(void);
extern uint32_t SystemCoreClock;
static uint32_t g_sys_clock = 0;

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
    if (hal_sleep_manager_get_lock_status() & (1 << handle)) {
        log_hal_error("handle %d is currently holding a lock, cannot release\n", handle);
        return HAL_SLEEP_MANAGER_ERROR;
    }
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
    return sleepdrv_is_handle_valid(handle_index);
}

static void sleep_GPT_CB(void *data)
{
    if (ptr_tickless_cb != NULL) {
        ptr_tickless_cb(data);
    }
}

hal_sleep_manager_status_t hal_sleep_manager_init()
{
    hal_lp_handle_intr();

    hal_gpt_init(wakeup_gpt_port);
    hal_gpt_register_callback(wakeup_gpt_port, sleep_GPT_CB, NULL);

    sleep_driver_init();

    hal_module_sleep_register_callback();

    g_sys_clock = SystemCoreClock;

    return HAL_SLEEP_MANAGER_OK;
}

void hal_sleep_manager_enter_sleep_mode(hal_sleep_mode_t mode)
{
    /* Switch flash clock to XTAL */
    cmnSerialFlashClkConfToXtal();
    if (g_sys_clock == 192000000) {
        /* Switch MCU clock to XTAL */
        cmnCpuClkConfigureToXtal();
    }
    /* Unclaim PLL usage */
    cmnPLL1OFF_PLL2OFF();

    if (mode == HAL_SLEEP_MODE_SLEEP) {
        HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) &= ~(BIT(2));
        HAL_REG_32(TOP_CFG_CM4_PWR_CTL_CR)    &= ~(BIT(30));
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("wfi",SA_START);
#endif /* MTK_SWLA_ENABLE */
        __asm volatile("dsb");

        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");

        __asm volatile("wfi");

        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");

        __asm volatile("isb");

        #ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("wfi",SA_STOP);
        #endif /* MTK_SWLA_ENABLE */
    } else if (mode == HAL_SLEEP_MODE_LEGACY_SLEEP) {
        sleepdrv_run_suspend_cbs(HAL_SLEEP_MODE_LEGACY_SLEEP);
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("lsm",SA_START);
#endif /* MTK_SWLA_ENABLE */
        hal_lp_legacy_sleep();
#ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("lsm",SA_STOP);
#endif /* MTK_SWLA_ENABLE */
        sleepdrv_run_resume_cbs(HAL_SLEEP_MODE_LEGACY_SLEEP);
    }

    /* After wakeup from sleep */
    if (g_sys_clock == 192000000) {
        /* Enable MCU clock to 192M */
        cmnCpuClkConfigureTo192M();
    }
    /* Enable flash clock to 64MHz */
    cmnSerialFlashClkConfTo64M();
    flash_switch_mode(SPIQ);
}


hal_sleep_manager_status_t hal_sleep_manager_set_sleep_time(uint32_t sleep_time_ms)
{
    hal_gpt_status_t ret;

    if (sleep_time_ms > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        sleep_time_ms = HAL_GPT_MAXIMUM_MS_TIMER_TIME;
    }

    if (sleep_time_ms == 0) {
        printf("hal_sleep_manager_set_sleep_time set 0 wakeup time error\n");
        return HAL_SLEEP_MANAGER_ERROR;
    }

    ret = hal_gpt_start_timer_ms(wakeup_gpt_port, sleep_time_ms, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    if (ret != HAL_GPT_STATUS_OK) {
        printf("hal_gpt_start_timer_ms %u error, err code: %d\n", (unsigned int)sleep_time_ms, ret);
        return HAL_SLEEP_MANAGER_ERROR;
    }

    return HAL_SLEEP_MANAGER_OK;
}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
