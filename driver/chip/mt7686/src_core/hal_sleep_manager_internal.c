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
#include "hal_sleep_manager_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include <stdio.h>
#include <string.h>
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"

#ifndef __UBL__
#include "assert.h"
#else
#define assert(expr) log_hal_error("assert\r\n")
#endif

#include "hal_emi_internal.h"
#include "hal_cache_internal.h"
#include "hal_flash_sf.h"
#include "hal_clock_internal.h"
#include "hal_gpt.h"
#include "hal_mpu_internal.h"
#include "hal_clock.h"

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif

static sleep_management_handle_t sleep_management_handle = {
    .lock_sleep_request = 0,
    .user_handle_resoure = 0,
    .user_handle_count = 0
};

static sleep_management_suspend_callback_func_t    suspend_callback_func_table      [SLEEP_BACKUP_RESTORE_MODULE_MAX];
static sleep_management_resume_callback_func_t     resume_callback_func_table       [SLEEP_BACKUP_RESTORE_MODULE_MAX];
static sleep_management_suspend_callback_func_t    suspend_user_callback_func_table [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
static sleep_management_resume_callback_func_t     resume_user_callback_func_table  [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
static uint32_t suspend_user_register_count = 0, resume_user_register_count = 0;

ATTR_ZIDATA_IN_TCM volatile uint32_t wakeup_source_status;
ATTR_ZIDATA_IN_TCM volatile uint32_t Vector0_backup, temp_reg;
ATTR_ZIDATA_IN_TCM volatile uint32_t origin_msp_bak_reg, origin_psp_bak_reg, backup_return_address;
ATTR_ZIDATA_IN_TCM volatile CPU_CORE_BAKEUP_REG_T  cpu_core_reg ;
ATTR_ZIDATA_IN_TCM volatile nvic_sleep_backup_register_t nvic_backup_register;
ATTR_ZIDATA_IN_TCM volatile FPU_BAKEUP_REG_T  fpu_reg;
ATTR_ZIDATA_IN_TCM volatile CMSYS_CFG_BAKEUP_REG_T  cmsys_cfg_reg;
ATTR_ZIDATA_IN_TCM volatile CMSYS_CFG_EXT_BAKEUP_REG_T cmsys_cfg_ext_reg;
ATTR_ZIDATA_IN_TCM volatile CM4_SYS_CTRL_BAKEUP_REG_T  cm4_sys_ctrl_reg;
ATTR_RWDATA_IN_TCM volatile uint8_t sleep_abort_flag = 0;
uint32_t cli_dtim_sleep_mode = 0;

/* for N9 API */
ATTR_RWDATA_IN_TCM int8_t (*ptr_lp_connsys_get_own_enable_int)(void)  = NULL;
ATTR_RWDATA_IN_TCM int8_t (*ptr_lp_connsys_give_n9_own)(void)         = NULL;
ATTR_RWDATA_IN_TCM uint8_t (*ptr_connsys_get_ownership)(void)         = NULL;

#if     defined (__GNUC__)      //GCC disable compiler optimize
__attribute__((optimize("O0")))
#elif   defined (__ICCARM__)    //IAR disable compiler optimize
#pragma optimize=none
#elif   defined (__CC_ARM)      //MDK disable compiler optimize
#pragma push
#pragma diag_suppress 1267
#pragma O0
#endif
ATTR_TEXT_IN_TCM void sleep_management_enter_deep_sleep(hal_sleep_mode_t mode)
{
#if defined (__CC_ARM)
    /* Backup function return address(R14) */
    __asm volatile("mov backup_return_address,__return_address() \n");
#endif

#ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE
#ifdef SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE
    if ((*SPM_PWR_STATUS & 0x4) == 0) {
        log_debug("[Sleep]MTCMOS CONN:Off\r\n");
    } else {
        log_debug("[Sleep]MTCMOS CONN:On\r\n");
    }
    if ((*SPM_PWR_STATUS & 0x8) == 0) {
        log_debug("[Sleep]MTCMOS SDIO:Off\r\n");
    } else {
        log_debug("[Sleep]MTCMOS SDIO:On\r\n");
    }
    log_debug("[Sleep]Enter Deep Sleep\r\n");
#endif
#endif
    clock_suspend();

    /* Set Boot Slave */
    *CMCFG_BOOT_FROM_SLV = 0x1;

    /* peripheral driver backup callback function */
    sleep_management_suspend_callback();

    /* spm Kick start*/
    spm_kick_start();

    /* enable spm irq and clear pending bits */
    hal_nvic_enable_irq(SPM_IRQn);
    hal_nvic_clear_pending_irq(SPM_IRQn);

    /* backup cmsys register */
    deep_sleep_cmsys_backup();

    /* general register backup */
    __CPU_STACK_POINT_BACKUP(origin_psp_bak_reg, origin_msp_bak_reg);

    /* Set CM4 SLEEPDEEP bits */
    *CM4_SYSTEM_CONTROL = *CM4_SYSTEM_CONTROL | 0x4;

    /* Flash Enter Powerdown Mode */
#ifdef HAL_FLASH_MODULE_ENABLED
    SF_DAL_DEV_Enter_DPD();
#endif
    /* PSRAM Enter Sleep Mode */
    mtk_psram_half_sleep_enter();

    /* backup BootVector0 Stack Address */
    Vector0_backup = *CMCFG_BOOT_VECTOR0;   //boot vector 0(boot slave stack point)

    /* backup MSP Address */
#if (defined (__GNUC__) || defined (__ICCARM__))
    __asm volatile("push {r0-r12, lr}");
    __asm volatile("mov %0, sp" : "=r"(temp_reg));
#elif defined (__CC_ARM)
    __PUSH_CPU_REG();
    __BACKUP_SP(temp_reg);
#endif
    *CMCFG_BOOT_VECTOR0 = temp_reg;  //VECTOR0 write MSP Address

    if (*SPM_SLEEP_ISR_STATUS == 1) {
        *SPM_PCM_CON0 = *SPM_PCM_CON0 & 0x1E; // [0]: kick PCM process
        *SPM_SLEEP_ISR_STATUS = 1;
        *SPM_SLEEP_ISR_STATUS = 0;
        sleep_abort_flag++;
    } else {
        /* Enter Deep Sleep */
        temp_reg = CMCFG_BOOT_VECTOR1;  //CMCFG_BOOT_VECTOR1 Address
        __ENTER_DEEP_SLEEP(temp_reg);

        /* CMSYS Peripheral : make virtual space available */
        *((volatile uint32_t *)0xE0181000) = 0x10000023;
        *((volatile uint32_t *)0xE0181004) = 0x0;
        /*sysram remap */
        *((volatile uint32_t *)0xE0181008) = 0x14200015;
        *((volatile uint32_t *)0xE018100C) = 0x04200000;

        /* pop CPU Reg R0-R12 */
        __POP_CPU_REG();
    }

    /* wait protect_en release */
    while (((*(volatile uint32_t *)(0xA21F0010)) & 0x1000100) != 0);
    /* add 2us delay for protect_en ready (40Mhz Xtal) */
    hal_gpt_delay_us(2);

    /* PSRAM Leave Sleep Mode */
    mtk_psram_half_sleep_exit();

    /* Flash Leave Powerdown Mode */
#ifdef HAL_FLASH_MODULE_ENABLED
    SF_DAL_DEV_Leave_DPD();
#endif
    /* get wakeup source */
    wakeup_source_status = *SPM_WAKEUP_SOURCE_STA;

    /* restore MSP */
    temp_reg = (unsigned int)&origin_msp_bak_reg;
    __MSP_RESTORE(temp_reg);

    /* swtich stack point to psp */
    __SWITCH_TO_PSP_STACK_POINT();

    /* restore PSP */
    temp_reg = (unsigned int)&origin_psp_bak_reg;
    __PSP_RESTORE(temp_reg);

    /* restore Core register - CONTROL */
    temp_reg = (unsigned int)&cpu_core_reg.CONTROL;
    __CPU_CORE_CONTROL_REG_RESTORE(temp_reg);

    /* restore boot Vector */
    *CMCFG_BOOT_FROM_SLV = 0x0;
    *CMCFG_BOOT_VECTOR0 = Vector0_backup ;

    /* Clear CM4 Deep Sleep bits */
    *CM4_SYSTEM_CONTROL = *CM4_SYSTEM_CONTROL & (~0x4);

    /* restore cmsys register */
    deep_sleep_cmsys_restore();

    /* disable spm irq and clear pending bits */
    hal_nvic_disable_irq(SPM_IRQn);
    hal_nvic_clear_pending_irq(SPM_IRQn);

    clock_resume();

    /* peripheral driver restore callback function */
    sleep_management_resume_callback();

    /* disable spm clock*/
    hal_clock_disable(HAL_CLOCK_CG_SW_SPM);

#if defined (__CC_ARM)
    __RESTORE_LR(backup_return_address);
#endif
}
#if     defined (__GNUC__)
#elif   defined (__CC_ARM)
#pragma pop
#endif

ATTR_TEXT_IN_TCM inline void deep_sleep_cmsys_backup(void)
{
    uint32_t i;
    /* backup CPU core registers */
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_BACKUP(temp_reg);

    /* NVIC backup */
    nvic_backup_register.nvic_iser = NVIC->ISER[0];
    nvic_backup_register.nvic_iser1 = NVIC->ISER[1];
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        nvic_backup_register.nvic_ip[i] = NVIC->IP[i];
    }

    /* cache backcp */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_save();
#endif

    /* mpu backcp */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_save();
#endif

    /* cmsys config backup */
    cmsys_cfg_reg.STCALIB = CMSYS_CFG->STCALIB;
    cmsys_cfg_reg.AHB_BUFFERALBE = CMSYS_CFG->AHB_BUFFERALBE;
    cmsys_cfg_reg.AHB_FIFO_TH = CMSYS_CFG->AHB_FIFO_TH;
    cmsys_cfg_reg.INT_ACTIVE_HL0 = CMSYS_CFG->INT_ACTIVE_HL0;
    cmsys_cfg_reg.INT_ACTIVE_HL1 = CMSYS_CFG->INT_ACTIVE_HL1;
    cmsys_cfg_reg.DCM_CTRL_REG = CMSYS_CFG->DCM_CTRL_REG;

    cmsys_cfg_ext_reg.CG_EN = CMSYS_CFG_EXT->CG_EN;
    cmsys_cfg_ext_reg.DCM_EN = CMSYS_CFG_EXT->DCM_EN;

    /* fpu backup */
    fpu_reg.FPCCR = FPU->FPCCR;
    fpu_reg.FPCAR = FPU->FPCAR;

    /* CM4 system control registers backup */
    cm4_sys_ctrl_reg.ACTLR = SCnSCB->ACTLR;
    cm4_sys_ctrl_reg.VTOR = SCB->VTOR;
    cm4_sys_ctrl_reg.SCR = SCB->SCR;
    cm4_sys_ctrl_reg.CCR = SCB->CCR;

    cm4_sys_ctrl_reg.SHP[0] = SCB->SHP[0]; /* MemMange */
    cm4_sys_ctrl_reg.SHP[1] = SCB->SHP[1]; /* BusFault */
    cm4_sys_ctrl_reg.SHP[2] = SCB->SHP[2]; /* UsageFault */
    cm4_sys_ctrl_reg.SHP[7] = SCB->SHP[7]; /* SVCall */
    cm4_sys_ctrl_reg.SHP[8] = SCB->SHP[8]; /* DebugMonitor */
    cm4_sys_ctrl_reg.SHP[10] = SCB->SHP[10]; /* PendSV */
    cm4_sys_ctrl_reg.SHP[11] = SCB->SHP[11]; /* SysTick */

    cm4_sys_ctrl_reg.SHCSR = SCB->SHCSR;
    cm4_sys_ctrl_reg.CPACR = SCB->CPACR;
}

ATTR_TEXT_IN_TCM inline void deep_sleep_cmsys_restore(void)
{
    uint32_t i;

    /* CM4 system control registers restore */
    SCnSCB->ACTLR = cm4_sys_ctrl_reg.ACTLR;
    SCB->VTOR = cm4_sys_ctrl_reg.VTOR;
    SCB->SCR = cm4_sys_ctrl_reg.SCR;
    SCB->CCR = cm4_sys_ctrl_reg.CCR;
    SCB->SHP[0] = cm4_sys_ctrl_reg.SHP[0]; /* MemMange */
    SCB->SHP[1] = cm4_sys_ctrl_reg.SHP[1]; /* BusFault */
    SCB->SHP[2] = cm4_sys_ctrl_reg.SHP[2]; /* UsageFault */
    SCB->SHP[7] = cm4_sys_ctrl_reg.SHP[7]; /* SVCall */
    SCB->SHP[8] = cm4_sys_ctrl_reg.SHP[8]; /* DebugMonitor */
    SCB->SHP[10] = cm4_sys_ctrl_reg.SHP[10]; /* PendSV */
    SCB->SHP[11] = cm4_sys_ctrl_reg.SHP[11]; /* SysTick */
    SCB->SHCSR = cm4_sys_ctrl_reg.SHCSR;
    SCB->CPACR = cm4_sys_ctrl_reg.CPACR;

    /* fpu restore */
    FPU->FPCCR = fpu_reg.FPCCR;
    FPU->FPCAR = fpu_reg.FPCAR;

    /* cmsys config restore */
    CMSYS_CFG->STCALIB = cmsys_cfg_reg.STCALIB;
    CMSYS_CFG->AHB_BUFFERALBE = cmsys_cfg_reg.AHB_BUFFERALBE;
    CMSYS_CFG->AHB_FIFO_TH = cmsys_cfg_reg.AHB_FIFO_TH;
    CMSYS_CFG->INT_ACTIVE_HL0 = cmsys_cfg_reg.INT_ACTIVE_HL0;
    CMSYS_CFG->INT_ACTIVE_HL1 = cmsys_cfg_reg.INT_ACTIVE_HL1;
    CMSYS_CFG->DCM_CTRL_REG = cmsys_cfg_reg.DCM_CTRL_REG;

    CMSYS_CFG_EXT->CG_EN = cmsys_cfg_ext_reg.CG_EN;
    CMSYS_CFG_EXT->DCM_EN = cmsys_cfg_ext_reg.DCM_EN;

    /* mpu restore */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_restore();
#endif

    /* cache restore */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_restore();
#endif

    /* restore CPU core registers */
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_RESTORE(temp_reg);

    /* NVIC restore */
    NVIC->ISER[0] = nvic_backup_register.nvic_iser;
    NVIC->ISER[1] = nvic_backup_register.nvic_iser1;
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        NVIC->IP[i] = nvic_backup_register.nvic_ip[i];
    }
}

void sleep_management_register_suspend_callback(sleep_management_backup_restore_module_t module , sleep_management_suspend_callback_t callback , void *data)
{
    if (module == SLEEP_BACKUP_RESTORE_USER) {
        if (suspend_user_register_count < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX) {
            suspend_user_callback_func_table[suspend_user_register_count].func        = callback;
            suspend_user_callback_func_table[suspend_user_register_count].data        = data;
            suspend_user_callback_func_table[suspend_user_register_count].init_status = SLEEP_MANAGEMENT_INITIALIZED;
            suspend_user_register_count++;
        } else {
            log_hal_error("[Sleep Management]register suspend callback function overflow\r\n");
            assert(0);
        }
    } else {
        suspend_callback_func_table[module].func        = callback;
        suspend_callback_func_table[module].data        = data;
        suspend_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
    }
}

void sleep_management_register_resume_callback(sleep_management_backup_restore_module_t module , sleep_management_resume_callback_t callback , void *data)
{
    if (module == SLEEP_BACKUP_RESTORE_USER) {
        if (resume_user_register_count < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX) {
            resume_user_callback_func_table[resume_user_register_count].func        = callback;
            resume_user_callback_func_table[resume_user_register_count].data        = data;
            resume_user_callback_func_table[resume_user_register_count].init_status = SLEEP_MANAGEMENT_INITIALIZED;
            resume_user_register_count++;
        } else {
            log_hal_error("[Sleep Management]register resume callback function overflow\r\n");
            assert(0);
        }
    } else {
        resume_callback_func_table[module].func = callback;
        resume_callback_func_table[module].data = data;
        resume_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
    }
}

ATTR_TEXT_IN_TCM void sleep_management_suspend_callback()
{
    static uint32_t i;

    for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        if (suspend_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(0, 0, i);
#endif
            suspend_callback_func_table[i].func(suspend_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(0, 1, i);
#endif
        }
    }

    for (i = 0; i < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX; i++) {
        if (suspend_user_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif

            suspend_user_callback_func_table[i].func(suspend_user_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 1, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif
        }
    }
}


ATTR_TEXT_IN_TCM void sleep_management_resume_callback()
{
    static uint32_t i;

    for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        if (resume_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i);
#endif

            resume_callback_func_table[i].func(resume_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 1, i);
#endif
        }
    }

    for (i = 0; i < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX; i++) {
        if (resume_user_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif

            resume_user_callback_func_table[i].func(resume_user_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 1, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif
        }
    }
}

uint8_t sleep_management_get_lock_handle(const char *handle_name)
{
    uint8_t index = 0, i;
    uint32_t mask, name_len;

    mask = save_and_set_interrupt_mask();
    for (index = 0 ; index < SLEEP_LOCK_HANDLE_USER_MAX; index++) {
        if (((sleep_management_handle.user_handle_resoure >> index) & 0x01) == 0) {
            sleep_management_handle.user_handle_resoure |= (1 << index);
            sleep_management_handle.user_handle_count++;

            memset(&sleep_management_handle.user_handle_name[index][0], 0, SLEEP_HANDLE_NAME_LEN);
            name_len = strlen(handle_name);

            if (name_len >= SLEEP_HANDLE_NAME_LEN) {
                name_len = SLEEP_HANDLE_NAME_LEN - 1;
            }
            memcpy(&sleep_management_handle.user_handle_name[index][0], handle_name, name_len);

            /* Check handle name */
            if (name_len == 0) {
                log_hal_error("[Sleep Management]sleep handle name error\r\n");
                assert(0);
            }
            for (i = 0; i < name_len; i++) {
                if ((sleep_management_handle.user_handle_name[index][i] <= 0x20) || (sleep_management_handle.user_handle_name[index][i] >= 0x7E)) {
                    log_hal_error("[Sleep Management]sleep handle name error\r\n");
                    assert(0);
                }
            }
            break;
        }
    }
    restore_interrupt_mask(mask);
    //log_hal_info("[Sleep Management]sleep handle name : %s\r\n", &sleep_management_handle.user_handle_name[index][0]);
    if (index >= SLEEP_LOCK_HANDLE_USER_MAX) {
        log_hal_error("[Sleep Management]cannot get sleep handle\r\n");
        assert(0);
        return (SLEEP_LOCK_INVALID_ID);
    }

    index += SLEEP_LOCK_USER_START_ID;

    return (index);
}

void sleep_management_release_lock_handle(uint8_t handle_index)
{
    uint32_t mask;
    /*  check handle index range */
    if ((handle_index >= SLEEP_LOCK_HANDLE_MAX) || (handle_index < SLEEP_LOCK_USER_START_ID)) {
        log_hal_error("[Sleep Management]sleep handle index error\r\n");
        return;
    }

    handle_index -= SLEEP_LOCK_USER_START_ID;

    mask = save_and_set_interrupt_mask();
    if (((sleep_management_handle.user_handle_resoure >> handle_index) & 0x01) == 1) {
        sleep_management_handle.user_handle_count--;
        sleep_management_handle.user_handle_resoure &= ~(1 << handle_index);
        memset(&sleep_management_handle.user_handle_name[handle_index][0], 0, SLEEP_HANDLE_NAME_LEN);
    } else {
        log_hal_warning("[Sleep Management]sleep handle already release \r\n");
    }
    restore_interrupt_mask(mask);
}

void sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index)
{
    uint32_t mask;

    if (handle_index >= SLEEP_LOCK_HANDLE_MAX) {
        log_hal_error("[Sleep Management]sleep handle index error\r\n");
        return;
    }

    mask = save_and_set_interrupt_mask();
    if (lock == LOCK_SLEEP) {
        /* Lock sleep request */
        sleep_management_handle.lock_sleep_request_count[handle_index]++;
        sleep_management_handle.lock_sleep_request |= (1 << handle_index);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        sleep_management_debug_lock_sleep_timelog(lock, handle_index);
#endif
        if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0xFF) {
            if (handle_index < SLEEP_LOCK_USER_START_ID) {
                log_hal_warning("[Sleep Management]sleep handle=%d,lock sleep count full \r\n", handle_index);
            } else {
                log_hal_warning("[Sleep Management]sleep handle=%d,%s,lock sleep count full \r\n", handle_index, (char *)&sleep_management_handle.user_handle_name[(handle_index - SLEEP_LOCK_USER_START_ID)]);
            }
        }
    } else {
        /* Unlock sleep request */
        if (sleep_management_handle.lock_sleep_request_count[handle_index] > 0) {
            sleep_management_handle.lock_sleep_request_count[handle_index]--;
            if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0) {
                sleep_management_handle.lock_sleep_request &= ~(1 << handle_index);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
                sleep_management_debug_lock_sleep_timelog(lock, handle_index);
#endif
            }
        } else {
            if (handle_index < SLEEP_LOCK_USER_START_ID) {
                log_hal_warning("[Sleep Management]sleep handle=%d,lock sleep has already released \r\n", handle_index);
            } else {
                log_hal_warning("[Sleep Management]sleep handle=%d,%s,lock sleep has already released \r\n", handle_index, (char *)&sleep_management_handle.user_handle_name[(handle_index - SLEEP_LOCK_USER_START_ID)]);
            }
        }
    }
    restore_interrupt_mask(mask);
}

bool sleep_management_check_sleep_locks(void)
{
    uint32_t mask;
    bool lock;
    mask = save_and_set_interrupt_mask();

    if (sleep_management_handle.lock_sleep_request == 0) {
        lock = false;
    } else {
        lock = true;
    }
    restore_interrupt_mask(mask);
    return lock;
}

bool sleep_management_check_handle_status(uint8_t handle_index)
{
    /*  check handle index range */
    if ((handle_index >= SLEEP_LOCK_HANDLE_MAX)) {
        log_hal_error("[Sleep Management]sleep handle index error\r\n");
        return false;
    }

    if (((sleep_management_handle.lock_sleep_request >> (handle_index)) & 0x01) == 1) {
        return true;
    } else {
        return false;
    }
}

uint32_t sleep_management_get_lock_sleep_request_info(void)
{
    return sleep_management_handle.lock_sleep_request;
}

uint32_t sleep_management_get_lock_sleep_handle_list(void)
{
    uint8_t i;
    uint32_t lock_sleep_request = sleep_management_handle.lock_sleep_request;

    printf("\r\n[Sleep Management]lock sleep handle list : \r\n");
    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        if (lock_sleep_request & (1 << i)) {
            if (i < SLEEP_LOCK_USER_START_ID) {
                sleep_management_dump_sleep_handel_name(i);
            } else {
                printf("sleep handle=%d,%s\r\n", i, (char *)&sleep_management_handle.user_handle_name[i - SLEEP_LOCK_USER_START_ID][0]);
            }
        }
    }
    return lock_sleep_request;
}

void sleep_management_low_power_init_setting(void)
{
    //uint32_t hw_coded;
    uint32_t system_info;

    /* cm4_dcm_en */
    *((volatile uint32_t *)(0xe00fe000)) = 0x3;
    *((volatile uint32_t *)(0xe00fe004)) = 0x3;
    *((volatile uint32_t *)(0xe0100040)) = 0x127;

    //hw_coded    = *((volatile uint32_t *)(0xA2000008)); /* HW_CODED */
    system_info = *((volatile uint32_t *)(0xA2010040)); /* SYSTEM_INFOD */

    /* system_info */
    if (((system_info >> 8) & 0x1)) { //
        /* bond_psram_sip_b == 0 */
        *((volatile uint32_t *)(0xA20D0058)) = 0x10; /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x10; /* IO_CFG_1_PD_CFG0_SET */
    } else {

    }
    if (((system_info >> 9) & 0x1)) {
        /*TINFO = " -------------------- bond_sf_sip_b == 0 -------------------- "*/
        *((volatile uint32_t *)(0xA20D0058)) = 0x20; /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x20; /* IO_CFG_1_PD_CFG0_SET */
    }

    if (((system_info >> 10) & 0x1)) {
        /*TINFO = " -------------------- bond_rsv_b == 0 -------------------- "*/
        *((volatile uint32_t *)(0xA20D0058)) = 0x40; /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x40; /* IO_CFG_1_PD_CFG0_SET */
    }

    *((volatile uint32_t *)(0xA20C00A4)) = 0xF; /* IO sleep tdsel : IO_CFG_0_TDSEL_CFG_SET */
}

int8_t hal_lp_connsys_get_own_enable_int(void)
{
    if (ptr_lp_connsys_get_own_enable_int != NULL) {
        return (ptr_lp_connsys_get_own_enable_int());
    }
    return 0;
}

int8_t hal_lp_connsys_give_n9_own(void)
{
    if (ptr_lp_connsys_give_n9_own != NULL) {
        return (ptr_lp_connsys_give_n9_own());
    }
    return 0;
}

void sleep_management_dump_sleep_handel_name(uint32_t handle_index)
{
    switch (handle_index) {
        case 0 :
            printf("sleep handle=%d,DMA\r\n", (int)handle_index);
            break;

        case 1 :
            printf("sleep handle=%d,AUDIO_TOP\r\n", (int)handle_index);
            break;

        case 2 :
            printf("sleep handle=%d,ASYS_TOP\r\n", (int)handle_index);
            break;

        case 3 :
            printf("sleep handle=%d,SPI_MASTER\r\n", (int)handle_index);
            break;

        case 4 :
            printf("sleep handle=%d,SPI_SLAVE\r\n", (int)handle_index);
            break;

        case 5 :
            printf("sleep handle=%d,UART0\r\n", (int)handle_index);
            break;

        case 6 :
            printf("sleep handle=%d,UART1\r\n", (int)handle_index);
            break;

        case 7 :
            printf("sleep handle=%d,UART2\r\n", (int)handle_index);
            break;

        case 8 :
            printf("sleep handle=%d,I2C0\r\n", (int)handle_index);
            break;

        case 9 :
            printf("sleep handle=%d,I2C1\r\n", (int)handle_index);
            break;

        case 10 :
            printf("sleep handle=%d,AUDIO_TOP_AHB\r\n", (int)handle_index);
            break;

        case 11 :
            printf("sleep handle=%d,ASYS_TOP_AHB\r\n", (int)handle_index);
            break;

        case 12 :
            printf("sleep handle=%d,SDIO_MST\r\n", (int)handle_index);
            break;

        case 13 :
            printf("sleep handle=%d,SDIO_SLV\r\n", (int)handle_index);
            break;
    }
    return;
}

void sleep_management_dump_debug_log(sleep_management_debug_log_index_t log_index)
{
    static uint32_t dump_index = 0;

    if ((log_index == SLEEP_MANAGEMENT_DEBUG_LOG_DUMP) && (dump_index != 0)) {
        if (((dump_index >> SLEEP_MANAGEMENT_DEBUG_LOG_OWNERSHIP_FAIL) & 0x01) == 0x01) {
            printf("connsys_get_ownership fail\r\n");
        }
        dump_index = 0;
    } else {
        if (log_index < SLEEP_MANAGEMENT_DEBUG_LOG_MAX) {
            dump_index |= (1 << log_index);
        }
    }
}

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#include "hal_gpt.h"
#define CLOCK_SOURCE_32K_FREQ   32.768
uint32_t sleep_handle_total_lock_sleep_time[SLEEP_LOCK_HANDLE_MAX];
uint32_t sleep_handle_total_lock_sleep_count[SLEEP_LOCK_HANDLE_MAX];
uint32_t sleep_backup_fun_time[SLEEP_BACKUP_RESTORE_MODULE_MAX + SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
uint32_t sleep_restore_fun_time[SLEEP_BACKUP_RESTORE_MODULE_MAX + SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];

void sleep_management_debug_lock_sleep_timelog(sleep_management_lock_sleep_t lock, uint8_t handle_index)
{
    static uint32_t lock_sleep_time[SLEEP_LOCK_HANDLE_MAX], unlock_sleep_time;

    if (lock == LOCK_SLEEP) {
        if (sleep_management_handle.lock_sleep_request_count[handle_index] == 1) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &lock_sleep_time[handle_index]);
        }
        sleep_handle_total_lock_sleep_count[handle_index]++;
    } else {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &unlock_sleep_time);

        if (unlock_sleep_time >= lock_sleep_time[handle_index]) {
            sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time - lock_sleep_time[handle_index];
        } else {
            sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time + (0xFFFFFFFF - lock_sleep_time[handle_index]);
        }
    }
}

void sleep_management_debug_backup_restore_fun_timelog(uint32_t type, uint32_t order, uint32_t callback)
{
    static  uint32_t enter, exit;

    if (order == 0) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &enter);
    } else {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &exit);
        if (exit >= enter) {
            exit = exit - enter;
        } else {
            exit = exit + (0xFFFFFFFF - enter);
        }
        if (type == 0) {
            sleep_backup_fun_time[callback] = exit;
        } else {
            sleep_restore_fun_time[callback] = exit;
        }
    }

}

void sleep_management_debug_dump_lock_sleep_time(void)
{
    uint32_t i;
    float lock_time;

    printf("\r\ndump lock sleep time : \r\n");
    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        if (sleep_handle_total_lock_sleep_count[i] > 0) {
            if (i < SLEEP_LOCK_USER_START_ID) {
                sleep_management_dump_sleep_handel_name(i);
            } else {
                printf("sleep handle=%d,%s\r\n", (int)i, (char *)&sleep_management_handle.user_handle_name[i - SLEEP_LOCK_USER_START_ID][0]);
            }
            printf("count : %d\r\n"             , (int)sleep_handle_total_lock_sleep_count[i]);

            lock_time = ((float)sleep_handle_total_lock_sleep_time[i]);
            lock_time = (lock_time) / CLOCK_SOURCE_32K_FREQ;
            if (lock_time < 1) {
                printf("total lock time : %d us,%d\r\n"   , (int)lock_time * 1000, (int)sleep_handle_total_lock_sleep_time[i]);
            } else if (lock_time >= 1) {
                printf("total lock time : %d ms,%d\r\n"   , (int)lock_time, (int)sleep_handle_total_lock_sleep_time[i]);
            }
            printf("\r\n");
        }
    }
}

void sleep_management_debug_reset_lock_sleep_time(void)
{
    uint32_t i;

    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        sleep_handle_total_lock_sleep_time[i] = 0;
        sleep_handle_total_lock_sleep_count[i] = 0;
    }
}

void sleep_management_debug_dump_backup_restore_time(void)
{
    uint32_t i;
    float time;

    printf("\r\ndump backup restore function execute time : \r\n");
    for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        time = (((float)sleep_backup_fun_time[i] * 1000) / CLOCK_SOURCE_32K_FREQ);
        printf("backup fun[%d]  : %d us,%d\r\n", (int)i, (int)time , (int)sleep_backup_fun_time[i]);

        time = (((float)sleep_restore_fun_time[i] * 1000) / CLOCK_SOURCE_32K_FREQ);
        printf("restore fun[%d] : %d us,%d\r\n", (int)i, (int)time , (int)sleep_restore_fun_time[i]);
    }
}

void sleep_management_dump_wakeup_source(uint32_t wakeup_source, uint32_t eint_status)
{
    if (sleep_abort_flag != 0) {
        printf("[Sleep]Abort Deep Sleep\r\n");
        sleep_abort_flag = 0;
    }

    printf("===============Wakeup from Deep Sleep===============\r\n");
    printf("[Sleep Management]WAKEUP_SOURCE = 0x%x\r\n", (int)wakeup_source);
    if (wakeup_source == 0x3FE) {
        printf("[Sleep Management]WAKEUP_SOURCE : GPT\r\n");
    }
    if (wakeup_source == 0x3FD) {
        sleep_management_dump_eint_wakeup_source(eint_status);
    }
    if (wakeup_source == 0x3FB) {
        printf("[Sleep Management]WAKEUP_SOURCE : SDIO_SLV\r\n");
    }
    if (wakeup_source == 0x3F7) {
        printf("[Sleep Management]WAKEUP_SOURCE : SPI_SLAVE_A\r\n");
    }
    if (wakeup_source == 0x3EF) {
        printf("[Sleep Management]WAKEUP_SOURCE : SPI_SLAVE_B\r\n");
    }
    if (wakeup_source == 0x3DF) {
        printf("[Sleep Management]WAKEUP_SOURCE : WDT\r\n");
    }
    if (wakeup_source == 0x3BF) {
        printf("[Sleep Management]WAKEUP_SOURCE : MSDC\r\n");
    }
    if (wakeup_source == 0x37F) {
        printf("[Sleep Management]WAKEUP_SOURCE : DEBUGSYS\r\n");
    }
}


void sleep_management_dump_eint_wakeup_source(uint32_t eint_num)
{
    uint32_t i;

    printf("[Sleep Management]WAKEUP_SOURCE : EINT = 0x%x\r\n", (unsigned int)eint_num);

    for (i = 0; i < HAL_EINT_NUMBER_MAX; i++) {
        if (((eint_num >> i) & 0x01) == 0x01) {
            if (i <= 20) {
                printf("[Sleep Management]EINT%d\r\n", (unsigned int)i);
            }
            switch (i) {
                case 21 :
                    printf("[Sleep Management]EINT_UART_0_RX\r\n");
                    break;

                case 22 :
                    printf("[Sleep Management]EINT_UART_1_RX\r\n");
                    break;

                case 23 :
                    printf("[Sleep Management]EINT_UART_2_RX\r\n");
                    break;

                case 24 :
                    printf("[Sleep Management]EINT_RTC\r\n");
                    break;

                case 25 :
                    printf("[Sleep Management]EINT_HIF\r\n");
                    break;

                case 26 :
                    printf("[Sleep Management]EINT_PSE\r\n");
                    break;

                case 27 :
                    printf("[Sleep Management]EINT_WDT\r\n");
                    break;

                case 28 :
                    printf("[Sleep Management]EINT_MAC\r\n");
                    break;

                case 29 :
                    printf("[Sleep Management]EINT_PMIC\r\n");
                    break;
            }
        }
    }
}
#endif

#endif
