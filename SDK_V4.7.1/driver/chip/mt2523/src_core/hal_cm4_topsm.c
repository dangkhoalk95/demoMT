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

#include "hal_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_cm4_topsm.h"
#include "hal_dsp_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_flash_sf.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_sleep_driver.h"
#include "hal_cm4_topsm_internal.h"
#include "hal_cache.h"
#include "hal_cache_internal.h"
#include "hal_mpu.h"
#include "hal_mpu_internal.h"
#include "hal_emi_internal.h"
#include "hal_pmu.h"
#include "hal_dvfs.h"
#include "hal_nvic.h"

#define MAX_CB_NUM 32

bool SLEEP_MANAGER_DEBUG = false;
bool BT_workaround = false;

int32_t suspend_cb_idx = 0;
int32_t resume_cb_idx = 0;
hal_cm4_topsm_cb_t suspend_cbs[MAX_CB_NUM];
hal_cm4_topsm_cb_t resume_cbs[MAX_CB_NUM];
uint32_t sleep_backup_fun_time[MAX_CB_NUM];
uint32_t sleep_restore_fun_time[MAX_CB_NUM];

cm4_topsm_mtcmos_control mtcmos_control;
ATTR_RWDATA_IN_TCM bool FAST_WAKEUP_VERIFICATION = false;
ATTR_RWDATA_IN_TCM CPU_CORE_BAKEUP_REG_T  cpu_core_reg ;
ATTR_RWDATA_IN_TCM uint32_t sleep_time_ms = 0;
ATTR_RWDATA_IN_TCM uint32_t Target_Sleep_Frames = 0;
ATTR_RWDATA_IN_TCM uint32_t frm_dur = 0;
ATTR_RWDATA_IN_TCM volatile uint32_t Vector0_backup, temp_reg;
ATTR_RWDATA_IN_TCM volatile uint32_t origin_msp_bak_reg, origin_psp_bak_reg, backup_return_address, g_spi_clock_cg_status;
ATTR_RWDATA_IN_TCM uint32_t activity_cpu_fre;
ATTR_RWDATA_IN_TCM uint32_t CM4_TOPSM_SETTING_WAKEUP_MASK=0x40;
void cm4_sleep_mode_no_init(unsigned short type);
void clock_gated_observe(void);
void clock_gated(void);
void cm4_sleep_bootup_check(const uint32_t wakeup_source_index);
void hal_cm4_topsm_run_suspend_cbs(void);
void hal_cm4_topsm_run_resume_cbs(void);
void hal_cm4_topsm_run_suspend_cbs(void);
void hal_cm4_topsm_run_resume_cbs(void);

extern void hal_module_sleep_register_callback(void);
extern void uart_backup_all_registers(void);
extern void uart_restore_all_registers(void);

void cm4_topsm_debug_option(bool sta)
{
    SLEEP_MANAGER_DEBUG = sta;
}
void cm4_tdma_enter_sleep()
{
    *CM4_TOPSM_MODEM_CG_CLR2 = 0x1;
}

ATTR_TEXT_IN_TCM void cm4_topsm_set_sleep_timeframe(uint32_t sleep_time, uint32_t frame, uint32_t duration)
{
    Target_Sleep_Frames = frame;
    frm_dur = duration;
    sleep_time_ms = sleep_time;
}

int cm4_topsm_lock_BT_MTCMOS()
{
    mtcmos_control.BT_Count++;
    if (mtcmos_control.BT_Count != 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 1);
    }
    return mtcmos_control.BT_Count;
}
int cm4_topsm_lock_MM_MTCMOS()
{
    mtcmos_control.MM_Count++;
    if (mtcmos_control.MM_Count != 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 1);
    }
    return mtcmos_control.MM_Count;
}
int cm4_topsm_lock_MD2G_MTCMOS()
{
    mtcmos_control.MD2G_Count++;
    if (mtcmos_control.MD2G_Count != 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 1);
    }
    return mtcmos_control.MD2G_Count;
}
int cm4_topsm_unlock_BT_MTCMOS()
{
    if (mtcmos_control.BT_Count == 0) {
        return 0;
    }
    mtcmos_control.BT_Count--;
    if (mtcmos_control.BT_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 0);
    }
    return mtcmos_control.BT_Count;
}
int cm4_topsm_unlock_MM_MTCMOS()
{
    if (mtcmos_control.MM_Count == 0) {
        return 0;
    }
    mtcmos_control.MM_Count--;
    if (mtcmos_control.MM_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 0);
    }
    return mtcmos_control.MM_Count;
}
int cm4_topsm_unlock_MD2G_MTCMOS()
{
    if (mtcmos_control.MD2G_Count == 0) {
        return 0;
    }
    mtcmos_control.MD2G_Count--;
    if (mtcmos_control.MD2G_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 0);
    }
    return mtcmos_control.MD2G_Count;
}
void mtcmos_bt()
{
    mtcmos_control.BT_Count = 0;
}
void mtcmos_mm()
{
    mtcmos_control.MM_Count = 0;
}
void mtcmos_md2g()
{
    mtcmos_control.MD2G_Count = 0;
}
void cm4_topsm_rm_init_interrupt(void)
{
    uint32_t timer_status = 0;
    do {
        timer_status = *CM4_TOPSM_RM_TMR_SSTA;
    } while (!(timer_status == TIMER_NORMAL_STATE));
    *CM4_TOPSM_ISR = 0x001F;
}

void cm4_topsm_rm_init_misc(void)
{
    *CM4_TOPSM_SM_SLV_REQ_IRQ = 0xF;
    *CM4_TOPSM_SM_MAS_RDY_IRQ = 0xF;
    //*CM4_TOPSM_FRC_CON = 0x11530000 + 0x00000001;
}

uint32_t cm4_topsm_get_free_run_counter(void)
{
    return *CM4_TOPSM_FRC_VAL_R;
}
uint32_t cm4_topsm_get_32k_free_run_counter(void)
{
    return *CM4_TOPSM_F32K_CNT;
}
//wakeup setting function

void cm4_topsm_wakeup_unmask_all_irq(void)
{
    CM4_TOPSM_SETTING_WAKEUP_MASK = 0x40;
}
void cm4_topsm_wakeup_mask_all_irq(void)
{
    CM4_TOPSM_SETTING_WAKEUP_MASK = 0xFFFFFFFF;
}
bool cm4_topsm_wakeup_set_one_irq_enable(hal_sleep_manager_wakeup_source_t index)
{
    cm4_topsm_wakeup_mask_all_irq();
    CM4_TOPSM_SETTING_WAKEUP_MASK = CM4_TOPSM_SETTING_WAKEUP_MASK & ~(1 << index);
    return true;
}

bool cm4_topsm_wakeup_set_one_irq_disable(hal_sleep_manager_wakeup_source_t index)
{
    CM4_TOPSM_SETTING_WAKEUP_MASK = CM4_TOPSM_SETTING_WAKEUP_MASK | (1 << index);
    return true;
}

void cm4_topsm_wakeup_irq_callback(void *user_data)
{
    if (*CM4_TOPSM_ISR != 0) {
        *CM4_TOPSM_ISR = 0x1F;
        hal_nvic_disable_irq(OSTimer_IRQn);
    }
}
void cm4_topsm_wakeup_init()
{
    *CM4_TOPSM_INT_MASK = (~(0x001E)) & 0x1F; /*unmask AFE timeout ,UFN timeout , pasuse abort, and pause interrupt*/
    *CM4_TOPSM_DBG_WAKEUP = 0x80000000; /*Cirq mask enable*/
    hal_nvic_register_isr_handler(OSTimer_IRQn, (hal_nvic_isr_t)cm4_topsm_wakeup_irq_callback);
    cm4_topsm_set_sleep_timeframe(4277145, 255, 4095); //SET_F32K_4096_MAX_DELAY_MS = 4277145
}

void cm4_topsm_set_frame_duration(uint32_t frame_dur)
{
    uint32_t tmp_frm_f32k = (frame_dur * 8) / (245);/*30.5176*8*/
    uint32_t topsm_frm_num = 0;
    uint32_t target_frm_f32k = 0; /*bit8~0 in OST_FRM_F32K*/
    uint32_t register_frm_f32k = 0;
    uint32_t settle_time;
    settle_time = CM4_TOPSM_SYSCLK_SETTLE + CM4_TOPSM_PLL_SETTLE;
    topsm_frm_num = ((((settle_time + (tmp_frm_f32k - 1)) / tmp_frm_f32k) + 1) > 2 ? (((settle_time + (tmp_frm_f32k - 1)) / tmp_frm_f32k) + 1) : 2);
    target_frm_f32k = (topsm_frm_num * tmp_frm_f32k) - 5;
#ifndef duration_1ms
    register_frm_f32k = (topsm_frm_num << 12) + (target_frm_f32k);
    *CM4_TOPSM_RM_CLK_SETTLE = 0x050f0013;
#else
    register_frm_f32k = (0x6 << 12) + 0x24 * CM4_TOPSM_RM_CLK_SETTLE = 0x050f0010;
#endif
    *CM4_TOPSM_FRM_F32K = register_frm_f32k;
    *CM4_TOPSM_FRM = frame_dur;
}
void cm4_topsm_enable_setting(uint32_t enable)
{
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    uint16_t tmp = 0;
    tmp = *CM4_TOPSM_CON;
    /*set 1:Always enable UFN down-count feature in ARM os TImer Driver*/
    tmp |= 0x2;
    tmp &= ~(0x4);
    /*Enable /Disable OS Timer */
    if (enable == 0x1) {
        tmp |= 0x1;
    } else {
        tmp &= ~(0x1);
    }

    *CM4_TOPSM_CON = tmp;
    *CM4_TOPSM_CMD = (0x11530000 + 0x0000E004);
    do {
        reg_val = *CM4_TOPSM_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));
}

CM4_TOPSM_RESULT_Enum cm4_topsm_set_afn(uint32_t afn)
{
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    uint32_t curr_afn = 0;
    /*Trigger HW Read command*/
    *CM4_TOPSM_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val = *CM4_TOPSM_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));
    curr_afn = *CM4_TOPSM_AFN_R;
    (void)curr_afn;
    {
        /*Set AFN value to HW*/
        *CM4_TOPSM_AFN = afn;
        do {
            reg_val = *CM4_TOPSM_STA;
            wait_time++;
        } while (!(reg_val & 0x0002));
        return CM4_TOPSM_SUCCESS_TRUE;
    }

}

CM4_TOPSM_RESULT_Enum cm4_topsm_set_ufn(uint32_t ufn)
{
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    uint32_t curr_ufn = 0;

    *CM4_TOPSM_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val = *CM4_TOPSM_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));
    curr_ufn = *CM4_TOPSM_UFN_R;
    (void)curr_ufn;
    if ((*CM4_TOPSM_CON & 0x2) == 0) {
        return CM4_TOPSM_FALSE;
    } else {
        *CM4_TOPSM_UFN = ufn;
        wait_time = 0;
        *CM4_TOPSM_CMD = (0x11530000 + 0x00002004);
        do {
            reg_val = *CM4_TOPSM_STA;
            wait_time++;
        } while (!(reg_val & 0x0002));
        return CM4_TOPSM_SUCCESS_TRUE;
    }
}

ATTR_TEXT_IN_TCM void cm4_topsm_enter_sleep_cmsys_memory_sleep(uint32_t Target_Sleep_Frames)
{
    uint16_t reg_val1 = 0, reg_val2 = 0, reg_val3 = 0;
    uint16_t wait_time = 0;
    uint32_t Set_AFN = 0, Set_UFN = 0;

//	/*Set AFN UFN before sleep*/
    Set_AFN = Set_UFN = Target_Sleep_Frames;
    cm4_topsm_set_afn(Set_AFN);
    cm4_topsm_set_ufn(Set_UFN);

    *CM4_TOPSM_INT_MASK = (~(0x001F)) & 0x1F; //turn on AFN ,UFN,PauseAbort and PauseINT ,and also Frames Tick
    /*Trigger HW Read commnad*/
    *CM4_TOPSM_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val3 = *CM4_TOPSM_STA;
        wait_time++;
    } while (!(reg_val3 & 0x0002));
    /*Trigger Pause Request*/
    wait_time = 0;
    *CM4_TOPSM_CMD = (0x11530000 + 0x00000001);
    do {
        reg_val1 = *CM4_TOPSM_STA;
        wait_time++;
    } while (!(reg_val1 & 0x0002));
    /*Then , check if OST_STA.PAUSE_REQ "11" or "10" */
    reg_val2 = *CM4_TOPSM_STA;
    if ((reg_val2 & 0x0018) == 0x0018) {
        //log_hal_info("Failed to trigger ost pause mode due to FUN <2");
    } else if ((reg_val2 & 0x0010) == 0x0010) {
        //log_hal_info("Failed to trigger ost pause mode due to wake up event happend before activate cp15 ex: sleep abort");
    } else { /*OST_STA.PAUSE_REQ = "00" */
        //TINF0 = "Successfully trigger OST HW ready to sleep"
    }
}

//wakeup end
#if defined (__GNUC__) //GCC disable compiler optimize
__attribute__((optimize("O0")))
#elif defined (__ICCARM__) //IAR disable compiler optimize
#pragma optimize=none
#elif defined (__CC_ARM) //MDK disable compiler optimize
#pragma push
#pragma diag_suppress 1267
#pragma O0
#endif
ATTR_TEXT_IN_TCM void cm4_topsm_enter_suspend(hal_sleep_mode_t mode)
{
    if ((mode != HAL_SLEEP_MODE_SLEEP) && (mode != HAL_SLEEP_MODE_DEEP_SLEEP)) {
        return;
    }
    if (SLEEP_MANAGER_DEBUG) {
        printf("===============[Start Enter Sleep]===============\r\n");
    }
    pmu_set_vcore_s1_buck();    //Set Vcore S1 Buck Voltage
    hal_cm4_topsm_run_suspend_cbs();
    cm4_topsm_init();
    cm4_topsm_wakeup_init();
    cm4_topsm_set_frame_duration(frm_dur);
#ifndef CM4_TCM_pd
    *CMCFG_SYSRAM_CFG = 0xFFFFFFFF;
#endif

    (mode == HAL_SLEEP_MODE_SLEEP) ? clock_suspend(true) : clock_suspend(false);
    if (mtcmos_control.BT_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_BTSYS, 0);
    }
    if (mtcmos_control.MM_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MMSYS, 0);
    }
    if (mtcmos_control.MD2G_Count == 0) {
        sys_topsm_mtcmos_control(HAL_SYS_TOPSM_MTCMOS_MD2G, 0);
        *((volatile uint32_t *)0xA21C0028) &= ~(1 << 15); //ABBA_TOP_CON0[15] RG_ABB_LVSH_EN
        *ABBA_AUDIODL_CON4_W = 0x0000;//(it's default value) DMIC global bias on     /* DMIC off */
//#ifdef MTK_AUDIO_MP3_ENABLED
//        mp3_codec_enter_suspend();
//#endif
    }
    cm4_topsm_enable_setting(1);
    cm4_tdma_enter_sleep();
    while ((*CM4_TOPSM_SM_SLV_REQ_STA & 0x1) == 0x1); /*turn off dsp_topsm clk req*/
    *DSP_TOPSM_DBG_RM_SM_MASK = 0x4;
    *SYS_TOPSM_RM_PWR_CON0 = *SYS_TOPSM_RM_PWR_CON0 & 0xFFFFFFBD; // [6]:PWR_REQ_EN=0 [2]:PWR_ON_0 = 0

    *CM4_TOPSM_DBG_RM_SM_MASK = 0x7;
    *CM4_TOPSM_RM_CLK_SETTLE = 0x050F0068; //cm4 overflow
    *CM4_TOPSM_SM_TRIG_SETTLE0 = 0x6;
    activity_cpu_fre = hal_dvfs_get_cpu_frequency();
    if (mode == HAL_SLEEP_MODE_SLEEP) {
        clock_suspend(true);
        if (pmu_get_type()) {
            PSI_SLEEP_AT_S0(SLEEP_PSI_MASTER_CM4);
        } else {
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_L);
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_H);
        }
        *CM4_TOPSM_MCF_CNT_BASE = 0x2;
        *CM4_TOPSM_RM_PWR_PER1 = *CM4_TOPSM_RM_PWR_PER1 | 0xB00;
        *CM4_TOPSM_RM_PWR_PER0 = *CM4_TOPSM_RM_PWR_PER0 | 0x6;
        *CM4_TOPSM_RM_CLK_SETTLE = 0x050f0013;
        *CM4_TOPSM_RM_CLK_SETTLE = *CM4_TOPSM_RM_CLK_SETTLE & 0xFF00FFFF;
        *CM4_TOPSM_RM_PWR_CON0 = *CM4_TOPSM_RM_PWR_CON0 & 0xFFFFE0FF;
        *CM4_TOPSM_EVENT_MASK = *CM4_TOPSM_EVENT_MASK | 0x3FFD;
    } else {
        if (pmu_get_type()) {
            PSI_SLEEP_AT_S1(SLEEP_PSI_MASTER_CM4); //Normal
        } else {
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_L);
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_H);
        }
        *CM4_TOPSM_RM_PWR_PER1 = *CM4_TOPSM_RM_PWR_PER1 | 0x300; // [11:8] reg_memup_settle
        if (mtcmos_control.BT_Count) {
            PSI_SLEEP_AT_S1(SLEEP_PSI_MASTER_BT); //Normal
        }
        if (FAST_WAKEUP_VERIFICATION == 1) {
            *PSI_MST_CON2 = 0x01;
        }
        *PSI_MST_CON2 = 0x70007;    //PSI settle time
    }
    cm4_topsm_enter_sleep_cmsys_memory_sleep(Target_Sleep_Frames);
    uint16_t reg_val = 0;
    uint16_t wait_time = 0;
    /*Trigger HW Read command*/
    *CM4_TOPSM_CMD = (0x11530000 + 0x00000002);
    do {
        reg_val = *CM4_TOPSM_STA;
        wait_time++;
    } while (!(reg_val & 0x0002));

    if (SLEEP_MANAGER_DEBUG) {
        printf("[BT_C:%d][MM_C:%d][MD2G_C:%d]\r\n", mtcmos_control.BT_Count, mtcmos_control.MM_Count, mtcmos_control.MD2G_Count);
        printf("[EINT IRQ(EINT_STA):%lx]\r\n", *((volatile uint32_t *)(0xA2030300)));
        sleep_driver_debug_dump_lock_sleep_time();
        if (pmu_get_type()) {
                    printf("Sleep in MTK_INTERNAL_PMIC\n");
        }
    }
    cm4_topsm_enter_deep_sleep(mode);
}
#if defined (__GNUC__)
#elif defined (__CC_ARM)
#pragma pop
#endif

#if defined (__GNUC__) //GCC disable compiler optimize
__attribute__((optimize("O0")))
#elif defined (__ICCARM__) //IAR disable compiler optimize
#pragma optimize=none
#elif defined (__CC_ARM) //MDK disable compiler optimize
#pragma push
#pragma diag_suppress 1267
#pragma O0
#endif
ATTR_TEXT_IN_TCM void cm4_topsm_enter_deep_sleep(hal_sleep_mode_t mode)
{
#if defined (__CC_ARM)
    /* Backup function return address(R14)*/
    __asm volatile("mov backup_return_address,__return_address() \n");
#endif
    /* SW Workaround for low power: control power down bit = 1 by using testmode before entering sleep mode*/
    *PMIC_WR_PATH_CON0 = 0x0001;

    /* Because GPIO design,must be set, otherwise there will be an exception IO */
    GPIO_TDSEL_SET;

    if (FAST_WAKEUP_VERIFICATION) {
        *((volatile uint32_t *)(0xA2020304)) = 0x00008000;
    }

    *CM4_TOPSM_RM_TMR_PWR0 = 1;
    __asm volatile("dsb");
    *CM4_TOPSM_RM_PWR_CON0 = 0x6901; // CM4 MTCMOS OFF while sleep

    /* UART backup */
    uart_backup_all_registers();

    /*backup CPU core registers*/
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_BACKUP(temp_reg);

    /* Set Boot Slave */
    *CMCFG_BOOT_FROM_SLV = 0x1;

    /* CM4_TOPSM_SYSTEM_CONTROL ? */
    *CM4_TOPSM_SYSTEM_CONTROL = *CM4_TOPSM_SYSTEM_CONTROL | 0x4;

    /* Start back up Cmsys */
    /* NVIC backup */
    nvic_sleep_backup_register();

    /* cache backcp */
#ifdef HAL_CACHE_MODULE_ENABLED
    cache_status_save();
#endif

    /* mpu backcp */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_save();
#endif

    /* cmsys config backup */
    cmsys_cfg_status_save();

    /* fpu backup */
    fpu_status_save();

    /* CM4 system control registers backup*/
    deepsleep_backup_CM4_sys_ctrl_reg();

    /* general register backup */
    __CPU_STACK_POINT_BACKUP(origin_psp_bak_reg, origin_msp_bak_reg);

    /* Check LDO Codition,switch PMU MTCMOS Power ON/OFF in S0,S1 Mode */
    pmu_control_mtcmos();

    /* Flash Enter Powerdown Mode */
    SF_DAL_DEV_Enter_DPD();

    /* PSRAM Enter Sleep Mode */
    mtk_psram_half_sleep_enter();

    /* infra power down */
    INFRA_POWER_OFF;

    /* clean SYS_TOPSM_RM_PERI_CON bits*/
    *((volatile uint32_t *)0xA20F0030) = *((volatile uint32_t *)0xA20F0030) & 0xFFFEFFFF;

    /* backup BootVector0 Stack Address */
    Vector0_backup = *((volatile uint32_t *)0xA2110000); //0xA2110000 : boot vector 0(boot slave stack point)

    /* Disable I2C wakeup source*/
    *CM4_TOPSM_EVENT_MASK = CM4_TOPSM_SETTING_WAKEUP_MASK;

    /* backup MSP Address */
#if (defined (__GNUC__) || defined (__ICCARM__))
    __asm volatile("push {r0-r12, lr}");
    __asm volatile("mov %0, sp" : "=r"(temp_reg));
#elif defined (__CC_ARM)
    __PUSH_CPU_REG();
    __BACKUP_SP(temp_reg);
#endif
    *((volatile uint32_t *)0xA2110000) = temp_reg;  //VECTOR0 write MSP Address

    /* Enable OST ISR */
    *CM4_TOPSM_INT_MASK = 0x17;
    *((volatile uint32_t *)0xe000E100) |= 1;        //NVIC_EnableIRQ(OSTimer_IRQn);
    *((volatile uint32_t *)0xe000e280) = 0x1;       //Clear CM4 OST IRQ Pending bits
    *CM4_TOPSM_ISR = 0x1F;

    /* Enter Deep Sleep */
    temp_reg = 0xA2110004;                          //CMCFG_BOOT_VECTOR1 Address
    __ENTER_DEEP_SLEEP(temp_reg);

    //HW : Wait PMIC ready
    if (mode == HAL_SLEEP_MODE_DEEP_SLEEP) {
        hal_gpt_delay_us(400);
    } else {
        hal_gpt_delay_us(600);
    }

    /* pop CPU Reg R0-R12 */
    __POP_CPU_REG();

    /* Clear OST Interrupt Flag */
    *CM4_TOPSM_ISR = 0x1F;

    /* Set SYS_TOPSM_RM_PERI_CON bits */
    *((volatile uint32_t *)0xA20F0030) |= 0x10000;

    if (FAST_WAKEUP_VERIFICATION == 1) {
        *((volatile uint32_t *)(0xA2020308)) = 0x00008000;  //low
    }


    /* save current SPI CG status */
    g_spi_clock_cg_status = *((volatile uint32_t *)(0xA2010304));
    /* enable all SPI CG */
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI0 % 32))) {
        *((volatile uint32_t *)(0xA2010324)) = (0x1 << (HAL_CLOCK_CG_SPI0 % 32));
    }
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI1 % 32))) {
        *((volatile uint32_t *)(0xA2010324)) = (0x1 << (HAL_CLOCK_CG_SPI1 % 32));
    }
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI2 % 32))) {
        *((volatile uint32_t *)(0xA2010324)) = (0x1 << (HAL_CLOCK_CG_SPI2 % 32));
    }
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI3 % 32))) {
        *((volatile uint32_t *)(0xA2010324)) = (0x1 << (HAL_CLOCK_CG_SPI3 % 32));
    }

    /* infra power on */
    INFRA_POWER_ON;

    /* PSRAM Leave Sleep Mode */
    mtk_psram_half_sleep_exit();

    /* Flash Leave Powerdown Mode */
    SF_DAL_DEV_Leave_DPD();

    /* CMSYS Peripheral : make virtual space available */
    *((volatile uint32_t *)(0xA0881000)) = 0x10000023;
    *((volatile uint32_t *)(0xA0881004)) = 0x0;

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
    *((volatile uint32_t *)0xA2110000) = Vector0_backup ;

    /* Start restore Cmsys */
    /* CM4 system control registers restore*/
    deepsleep_restore_CM4_sys_ctrl_reg();

    /* fpu restore */
    fpu_status_restore();

    /* cmsys config restore */
    cmsys_cfg_status_restore();

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

    /* Clear CM4 Deep Sleep bits */
    *CM4_TOPSM_SYSTEM_CONTROL = *CM4_TOPSM_SYSTEM_CONTROL & (~0x4);

    /* NVIC restore */
    nvic_sleep_restore_register();
    /* End restore Cmsys */

    /* UART restore */
    uart_restore_all_registers();

    clock_resume();
    if (pmu_get_type() == 0) {
        hal_dvfs_target_cpu_frequency(activity_cpu_fre, HAL_DVFS_FREQ_RELATION_L);
        hal_dvfs_target_cpu_frequency(activity_cpu_fre, HAL_DVFS_FREQ_RELATION_H);
    }

    hal_cm4_topsm_run_resume_cbs();

    /* SW Workaround for low power: retore the normal mode setting after exiting sleep mode */
    *PMIC_WR_PATH_CON0 = 0x0004;

    /* Because GPIO design,must be set, otherwise there will be an exception IO */
    GPIO_TDSEL_CLR;

    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI0 % 32))) {
        *((volatile uint32_t *)(0xA2010314)) = (0x1 << (HAL_CLOCK_CG_SPI0 % 32));
    }
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI1 % 32))) {
        *((volatile uint32_t *)(0xA2010314)) = (0x1 << (HAL_CLOCK_CG_SPI1 % 32));
    }
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI2 % 32))) {
        *((volatile uint32_t *)(0xA2010314)) = (0x1 << (HAL_CLOCK_CG_SPI2 % 32));
    }
    if (g_spi_clock_cg_status & (0x1 << (HAL_CLOCK_CG_SPI3 % 32))) {
        *((volatile uint32_t *)(0xA2010314)) = (0x1 << (HAL_CLOCK_CG_SPI3 % 32));
    }

    if (SLEEP_MANAGER_DEBUG) {
        cm4_topsm_debug_dump_wakeup_source();
        cm4_topsm_debug_dump_backup_restore_time();
        sleep_driver_debug_dump_lock_sleep_time();
    }

}
#if defined (__GNUC__)
#elif defined (__CC_ARM)
#pragma pop
#endif

void cm4_topsm_init(void)
{
    *CM4_TOPSM_RM_CLK_SETTLE = 0x50F01FF; /*8:0]=rm_sysclk_settle max value*/
    *CM4_TOPSM_RM_TMR_TRG0 = 0x1;
    *CM4_TOPSM_RM_TMR_PWR0 = 0x00000001; // setting ostimer -> MCU power region
    *CM4_TOPSM_RM_PWR_PER0 = 0x0900121F; // For Power state setting time
    *CM4_TOPSM_RM_PWR_PER1 = 0x11111111;
    *CM4_TOPSM_PROTECT_ACK_MASK = 0x00000000; // Mask when power domain always on
    *CM4_TOPSM_RM_PWR_CON0 = 0x00042244; // CM4 MTCMOS ON while sleep
    *CM4_TOPSM_CCF_CLK_CON |= (0x01 << 12); /*Setting for TOPSM state debug output*/
    *CM4_TOPSM_SM_TRIG_SETTLE0 = 0x00000008; //SM_TRIG_SETTLE0 > 2+2+2 = RM_PLL1_SETTLE+RM_PLL2_SETTLE+(MAX_PWR_SETTLE in RM_PWR_PER)
    *CM4_TOPSM_SM_REQ_MASK = 0x00000000; // {2: to MODEM , 0:to ARM7 }
    *CM4_TOPSM_RM_SM_TRG = 0xFFFFFFFF;
    *CM4_TOPSM_RM_SM_PWR = 0x00000000;
    *CM4_TOPSM_RM_SM_PLL_MASK0 = 0xFFFFFFFF;
    *CM4_TOPSM_INDIV_CLK_PROTECT_ACK_MASK = 0x00;
    cm4_topsm_rm_init_interrupt();
    cm4_topsm_rm_init_misc();

    *CM4_TOPSM_CCF_CLK_CON = *CM4_TOPSM_CCF_CLK_CON | 0x4000;
    *SYS_TOPSM_CCF_CLK_CON = *SYS_TOPSM_CCF_CLK_CON | 0x4000;
    *DSP_TOPSM_CCF_CLK_CON = *DSP_TOPSM_CCF_CLK_CON | 0x4000;

    *CM4_TOPSM_TOPSM_DBG = *CM4_TOPSM_TOPSM_DBG & 0xFFFFFFFE;
    *SYS_TOPSM_TOPSM_DBG = *SYS_TOPSM_TOPSM_DBG & 0xFFFFF0FF;
    *DSP_TOPSM_TOPSM_DBG = *DSP_TOPSM_TOPSM_DBG & 0xFFFFFFFE;

    *((volatile uint32_t *)0xA20F0030) |= 0x10000;  //Set SYS_TOPSM_RM_PERI_CON bits
}

void cm4_topsm_debug_backup_restore_fun_timelog(uint32_t type, uint32_t order, uint32_t callback)
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
void cm4_topsm_debug_dump_backup_restore_time(void)
{
    uint32_t i;
    float time;
    printf("\r\n[==dump backup restore function execute time==]\r\n");
    for (i = 0; i < MAX_CB_NUM; i++) {
        time = (((float)sleep_backup_fun_time[i]*1000) / (float)CLOCK_SOURCE_32K_FREQ);
        if(time!=0){
            printf("backup fun[%d]  ;Spend %d us\r\n", (int)i,(int)time);
            time = (((float)sleep_restore_fun_time[i]*1000) / (float)CLOCK_SOURCE_32K_FREQ);
            printf("restore fun[%d] ;Spend %d us\r\n", (int)i,(int)time);
        }

    }
}
bool hal_cm4_topsm_register_suspend_cb(cm4_topsm_cb func, void *data)
{
    if (suspend_cb_idx >= MAX_CB_NUM) {
        printf("over max suspend callback function can be registered\n");
        return false;
    }
    suspend_cbs[suspend_cb_idx].func = func;
    suspend_cbs[suspend_cb_idx].para = data;
    suspend_cb_idx++;
    return true;
}

void hal_cm4_topsm_run_suspend_cbs(void)
{
    for (int32_t i = 0; i < suspend_cb_idx; i++) {
        //cm4_topsm_debug_backup_restore_fun_timelog(0,0,i);
        suspend_cbs[i].func(suspend_cbs[i].para);
        //cm4_topsm_debug_backup_restore_fun_timelog(0,1,i);
    }
}

bool hal_cm4_topsm_register_resume_cb(cm4_topsm_cb func, void *data)
{
    if (resume_cb_idx >= MAX_CB_NUM) {
        printf("over max resume callback function can be registered\r\n");
        return false;
    }
    resume_cbs[resume_cb_idx].func = func;
    resume_cbs[resume_cb_idx].para = data;
    resume_cb_idx++;
    return true;
}

void hal_cm4_topsm_run_resume_cbs(void)
{
    for (int32_t i = 0; i < resume_cb_idx; i++) {
        resume_cbs[i].func(resume_cbs[i].para);
    }
}
void cm4_topsm_sleep_default_callback_function()
{
    hal_module_sleep_register_callback();
}

void cm4_topsm_debug_dump_wakeup_source(){
    printf("\r\n[==Sleep Management Wakeup Index : 0x%lx==]\r\n", *((volatile uint32_t *)(CM4_TOPSM_WAKEUP_STA)));
    uint32_t wakeup = *((volatile uint32_t *)(CM4_TOPSM_WAKEUP_STA));
    switch(wakeup){
    case 0x1:
        printf("[Sleep Management]WAKEUP_SOURCE : GPT \r\n");
        break;
    case 0x2:
        printf("[Sleep Management]WAKEUP_SOURCE : EINT \r\n");
        break;
    case 0x4:
        printf("[Sleep Management]WAKEUP_SOURCE : RESERVED \r\n");
        break;
    case 0x8:
        printf("[Sleep Management]WAKEUP_SOURCE : KP \r\n");
        break;
    case 0x10:
        printf("[Sleep Management]WAKEUP_SOURCE : MSDC1 \r\n");
        break;
    case 0x20:
        printf("[Sleep Management]WAKEUP_SOURCE : MDIF \r\n");
        break;
    case 0x40:
        printf("[Sleep Management]WAKEUP_SOURCE : RESERVED_2 \r\n");
        break;
    case 0x80:
        printf("[Sleep Management]WAKEUP_SOURCE : MSDC2 \r\n");
        break;
    case 0x100:
        printf("[Sleep Management]WAKEUP_SOURCE : SPI_SLAVE \r\n");
        break;
    case 0x200:
        printf("[Sleep Management]WAKEUP_SOURCE : RGU \r\n");
        break;
    case 0x400:
        printf("[Sleep Management]WAKEUP_SOURCE : MM_DBI \r\n");
        break;
    case 0x800:
        printf("[Sleep Management]WAKEUP_SOURCE : MM_DSI \r\n");
        break;
    }
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
