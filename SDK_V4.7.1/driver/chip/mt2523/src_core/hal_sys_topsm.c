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

#include "hal_sys_topsm.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_dsp_topsm.h"
#include "hal_cm4_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "memory_attribute.h"
#include "hal_log.h"
#include "hal_gpt.h"

extern cm4_topsm_mtcmos_control mtcmos_control;
void sys_topsm_debug_log(hal_sys_topsm_mtcmos_enum_t mtcmos)
{
    log_hal_info("[BT--SYS_TOPSM_RM_PWR_CON1 : %#x]\n", (unsigned int)(*SYS_TOPSM_RM_PWR_CON1));
    log_hal_info("[MM--SYS_TOPSM_RM_PWR_CON2 : %#x]\n", (unsigned int)(*SYS_TOPSM_RM_PWR_CON2));
    log_hal_info("[MD2G--DSP_TOPSM_RM_PWR_CON0 : %#x]\n", (unsigned int)(*DSP_TOPSM_RM_PWR_CON0));
    log_hal_info("[Counter : BT:%d; MM:%d; MD2G:%d]\n", mtcmos_control.BT_Count, mtcmos_control.MM_Count, mtcmos_control.MD2G_Count);
}
void sys_topsm_init(void)
{
    *SYS_TOPSM_RM_TMR_PWR0 = *SYS_TOPSM_RM_TMR_PWR0 | 0x1010101;
    *SYS_TOPSM_TOPSM_DBG = *SYS_TOPSM_TOPSM_DBG & 0xFFFFFFFE;
    *SYS_TOPSM_RM_PWR_CON1 = *SYS_TOPSM_RM_PWR_CON1 | 0x44;
    *SYS_TOPSM_RM_PWR_CON2 = *SYS_TOPSM_RM_PWR_CON2 | 0x00000044;
    *SYS_TOPSM_TOPSM_DBG = *SYS_TOPSM_TOPSM_DBG & 0xFFFFFBFF; //ICE power off ;

    *DSP_TOPSM_DBG_RM_SM_MASK = 0x0; // default , nbut need it for wakeup back, let indiv_clk_reg On
    *DSP_TOPSM_RM_SM_PLL_MASK0 = 0x0;
    *DSP_TOPSM_TOPSM_DBG = *DSP_TOPSM_TOPSM_DBG & 0xFFFFFFFE; //default , but need it for wakeup back
    *DSP_TOPSM_RM_PWR_CON0 = *DSP_TOPSM_RM_PWR_CON0 | 0x00000044; // no SW mode
    *DSP_TOPSM_RM_SM_PWR = 0x00000000; //default,but need it for wakeup back , cross trugger

    *DSP_TOPSM_INDIV_CLK_PROTECT_ACK_MASK = 0x1F;
    *DSP_TOPSM_TOPSM_DBG = 0x1;
    *CM4_TOPSM_DBG_RM_SM_MASK = 0x7;
    *DSP_TOPSM_RM_SM_PWR &= 0xFFFFFFF0;
}

ATTR_TEXT_IN_TCM void sys_topsm_mtcmos_control(hal_sys_topsm_mtcmos_enum_t mtcmos, bool config)
{
    switch (mtcmos) {
        case HAL_SYS_TOPSM_MTCMOS_BTSYS:
            if (config) {
                *SYS_TOPSM_RM_PWR_CON1 = *SYS_TOPSM_RM_PWR_CON1 | 0x00000044;
                while ((*SYS_TOPSM_RM_PWR_STA & 0x2) == 0)
                    ;
            } else {
                *SYS_TOPSM_RM_PWR_CON1 = (*SYS_TOPSM_RM_PWR_CON1 & 0xFFFFDFBB) | (0x40000);
                *SYS_TOPSM_RM_PWR_STA = 0x2;
            }
            break;
        case HAL_SYS_TOPSM_MTCMOS_MMSYS:
            if (config) {
                *SYS_TOPSM_RM_PWR_CON2 = *SYS_TOPSM_RM_PWR_CON2 | 0x00000044;
                hal_gpt_delay_us(500);
            } else {
                *SYS_TOPSM_RM_PWR_CON2 = *SYS_TOPSM_RM_PWR_CON2 & 0xFFFFDFBB;
                *SYS_TOPSM_RM_PWR_STA = 0x4;
            }
            break;
        case HAL_SYS_TOPSM_MTCMOS_MD2G:
            if (config) {
                *DSP_TOPSM_RM_PWR_CON0 = *DSP_TOPSM_RM_PWR_CON0 | 0x00000044;
                while ((*DSP_TOPSM_RM_PWR_STA & 0x1) == 0);
				*((volatile uint32_t *) 0xA21C0028) |= (1 << 15);
            } else {
                *DSP_TOPSM_RM_TMR_PWR0 = 0x0;
#ifndef DSP_MEM_sleep
                *DSP_TOPSM_RM_PWR_CON0 = *DSP_TOPSM_RM_PWR_CON0 & 0xFFFFDFBB;
                *CM4_TOPSM_MODEM_MEM_CON = *CM4_TOPSM_MODEM_MEM_CON | 0x80000000; // [31] : fd216sys_no_mem_sleepb , if want two sleep memory pd = 1 [2]:sw_force_fd216sys_mem_pd
#else
                *DSP_TOPSM_RM_PWR_CON0 = *DSP_TOPSM_RM_PWR_CON0 & 0xFFFBDFBB; // [18]:mem sleep
#endif
                *DSP_TOPSM_INDIV_CLK_PROTECT_ACK_MASK = 0x1F;
                *DSP_TOPSM_TOPSM_DBG = 0x1;
                *CM4_TOPSM_DBG_RM_SM_MASK = 0x7;
            }
            break;
        default:
            break;
    }
}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
