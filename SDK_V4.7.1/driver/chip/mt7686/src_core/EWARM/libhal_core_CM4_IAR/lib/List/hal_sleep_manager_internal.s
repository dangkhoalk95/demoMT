///////////////////////////////////////////////////////////////////////////////
//
// IAR ANSI C/C++ Compiler V7.50.1.10123/W32 for ARM      07/Apr/2017  14:17:21
// Copyright 1999-2015 IAR Systems AB.
//
//    Cpu mode     =  thumb
//    Endian       =  little
//    Source file  =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_sleep_manager_internal.c
//    Command line =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_sleep_manager_internal.c
//        -D MTK_DEBUG_LEVEL_INFO -D PRODUCT_VERSION=7686 -lcN
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\List
//        -lA
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\List
//        --diag_suppress Pa050,Ta022,Ta023 --diag_error Pe606 -o
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\Obj
//        --no_unroll --no_inline --no_tbaa --no_scheduling --debug
//        --endian=little --cpu=Cortex-M4 -e --fpu=VFPv4_sp --dlib_config
//        "C:\Program Files (x86)\IAR Systems\Embedded Workbench
//        7.3\arm\INC\c\DLib_Config_Full.h" -I
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\inc\
//        -I
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\..\..\..\..\inc\
//        -I
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\..\..\..\inc\
//        -I
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\..\..\..\..\..\CMSIS\Include\
//        -I
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\..\..\..\..\..\CMSIS\Device\MTK\mt7686\Include\
//        -I
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\..\..\..\..\..\..\kernel\service\inc\
//        -I W:\Documents\2523_dev\gva\middleware\MTK\minicli\inc\ -Om -I
//        "C:\Program Files (x86)\IAR Systems\Embedded Workbench
//        7.3\arm\CMSIS\Include\"
//    List file    =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\List\hal_sleep_manager_internal.s
//
///////////////////////////////////////////////////////////////////////////////

        RTMODEL "__SystemLibrary", "DLib"
        RTMODEL "__dlib_file_descriptor", "1"
        RTMODEL "__dlib_full_locale_support", "1"
        RTMODEL "__iar_require _Printf", ""
        AAPCS BASE,INTERWORK,VFP
        PRESERVE8
        REQUIRE8

        #define SHT_PROGBITS 0x1

        EXTERN SF_DAL_DEV_Enter_DPD
        EXTERN SF_DAL_DEV_Leave_DPD
        EXTERN __aeabi_assert
        EXTERN __aeabi_memcpy
        EXTERN __aeabi_memset
        EXTWEAK __iar_EmptyStepPoint
        EXTERN atoi
        EXTERN cache_status_restore
        EXTERN cache_status_save
        EXTERN clock_resume
        EXTERN clock_suspend
        EXTERN hal_clock_disable
        EXTERN hal_gpt_delay_ms
        EXTERN hal_gpt_delay_us
        EXTERN hal_sleep_manager_enter_sleep_mode
        EXTERN hal_sleep_manager_is_sleep_locked
        EXTERN hal_sleep_manager_set_sleep_time
        EXTERN log_hal_error_internal
        EXTERN log_hal_warning_internal
        EXTERN mpu_status_restore
        EXTERN mpu_status_save
        EXTERN mtk_psram_half_sleep_enter
        EXTERN mtk_psram_half_sleep_exit
        EXTERN printf
        EXTERN restore_interrupt_mask
        EXTERN save_and_set_interrupt_mask
        EXTERN sleep_management_enter_retention
        EXTERN spm_kick_start
        EXTERN strlen

        PUBLIC Vector0_backup
        PUBLIC backup_return_address
        PUBLIC cli_dtim_sleep_mode
        PUBLIC cm4_sys_ctrl_reg
        PUBLIC cmsys_cfg_ext_reg
        PUBLIC cmsys_cfg_reg
        PUBLIC cpu_core_reg
        PUBLIC deep_sleep_cmsys_backup
        PUBLIC deep_sleep_cmsys_restore
        PUBLIC fpu_reg
        PUBLIC hal_lp_connsys_get_own_enable_int
        PUBLIC hal_lp_connsys_give_n9_own
        PUBLIC lp_cli
        PUBLIC nvic_backup_register
        PUBLIC origin_msp_bak_reg
        PUBLIC origin_psp_bak_reg
        PUBLIC ptr_cli_dtim_enter_sleep
        PUBLIC ptr_connsys_get_ownership
        PUBLIC ptr_lp_connsys_get_own_enable_int
        PUBLIC ptr_lp_connsys_give_n9_own
        PUBLIC sleep_management_check_handle_status
        PUBLIC sleep_management_check_sleep_locks
        PUBLIC sleep_management_enter_deep_sleep
        PUBLIC sleep_management_get_lock_handle
        PUBLIC sleep_management_get_lock_sleep_handle_list
        PUBLIC sleep_management_get_lock_sleep_request_info
        PUBLIC sleep_management_lock_sleep
        PUBLIC sleep_management_low_power_init_setting
        PUBLIC sleep_management_register_resume_callback
        PUBLIC sleep_management_register_suspend_callback
        PUBLIC sleep_management_release_lock_handle
        PUBLIC sleep_management_resume_callback
        PUBLIC sleep_management_suspend_callback
        PUBLIC temp_reg
        PUBLIC wakeup_source_status
        
          CFI Names cfiNames0
          CFI StackFrame CFA R13 DATA
          CFI Resource R0:32, R1:32, R2:32, R3:32, R4:32, R5:32, R6:32, R7:32
          CFI Resource R8:32, R9:32, R10:32, R11:32, R12:32, R13:32, R14:32
          CFI Resource D0:64, D1:64, D2:64, D3:64, D4:64, D5:64, D6:64, D7:64
          CFI Resource D8:64, D9:64, D10:64, D11:64, D12:64, D13:64, D14:64
          CFI Resource D15:64
          CFI EndNames cfiNames0
        
          CFI Common cfiCommon0 Using cfiNames0
          CFI CodeAlign 2
          CFI DataAlign 4
          CFI ReturnAddress R14 CODE
          CFI CFA R13+0
          CFI R0 Undefined
          CFI R1 Undefined
          CFI R2 Undefined
          CFI R3 Undefined
          CFI R4 SameValue
          CFI R5 SameValue
          CFI R6 SameValue
          CFI R7 SameValue
          CFI R8 SameValue
          CFI R9 SameValue
          CFI R10 SameValue
          CFI R11 SameValue
          CFI R12 Undefined
          CFI R14 SameValue
          CFI D0 Undefined
          CFI D1 Undefined
          CFI D2 Undefined
          CFI D3 Undefined
          CFI D4 Undefined
          CFI D5 Undefined
          CFI D6 Undefined
          CFI D7 Undefined
          CFI D8 SameValue
          CFI D9 SameValue
          CFI D10 SameValue
          CFI D11 SameValue
          CFI D12 SameValue
          CFI D13 SameValue
          CFI D14 SameValue
          CFI D15 SameValue
          CFI EndCommon cfiCommon0
        
// W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_sleep_manager_internal.c
//    1 /* Copyright Statement:
//    2  *
//    3  * (C) 2005-2016  MediaTek Inc. All rights reserved.
//    4  *
//    5  * This software/firmware and related documentation ("MediaTek Software") are
//    6  * protected under relevant copyright laws. The information contained herein
//    7  * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
//    8  * Without the prior written permission of MediaTek and/or its licensors,
//    9  * any reproduction, modification, use or disclosure of MediaTek Software,
//   10  * and information contained herein, in whole or in part, shall be strictly prohibited.
//   11  * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
//   12  * if you have agreed to and been bound by the applicable license agreement with
//   13  * MediaTek ("License Agreement") and been granted explicit permission to do so within
//   14  * the License Agreement ("Permitted User").  If you are not a Permitted User,
//   15  * please cease any access or use of MediaTek Software immediately.
//   16  * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
//   17  * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
//   18  * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
//   19  * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//   20  * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
//   21  * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
//   22  * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
//   23  * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
//   24  * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
//   25  * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
//   26  * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
//   27  * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
//   28  * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
//   29  * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
//   30  * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
//   31  * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
//   32  * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
//   33  */
//   34 
//   35 #include "hal_sleep_manager.h"

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock0 Using cfiCommon0
          CFI Function NVIC_EnableIRQ
          CFI NoCalls
        THUMB
// static __interwork __softfp void NVIC_EnableIRQ(IRQn_Type)
NVIC_EnableIRQ:
        MOVS     R1,#+1
        AND      R2,R0,#0x1F
        LSLS     R1,R1,R2
        LDR.W    R2,??DataTable53  ;; 0xe000e100
        LSRS     R0,R0,#+5
        STR      R1,[R2, R0, LSL #+2]
        BX       LR               ;; return
          CFI EndBlock cfiBlock0

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock1 Using cfiCommon0
          CFI Function NVIC_DisableIRQ
          CFI NoCalls
        THUMB
// static __interwork __softfp void NVIC_DisableIRQ(IRQn_Type)
NVIC_DisableIRQ:
        MOVS     R1,#+1
        AND      R2,R0,#0x1F
        LSLS     R1,R1,R2
        LDR.W    R2,??DataTable53_1  ;; 0xe000e180
        LSRS     R0,R0,#+5
        STR      R1,[R2, R0, LSL #+2]
        BX       LR               ;; return
          CFI EndBlock cfiBlock1

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock2 Using cfiCommon0
          CFI Function NVIC_ClearPendingIRQ
          CFI NoCalls
        THUMB
// static __interwork __softfp void NVIC_ClearPendingIRQ(IRQn_Type)
NVIC_ClearPendingIRQ:
        MOVS     R1,#+1
        AND      R2,R0,#0x1F
        LSLS     R1,R1,R2
        LDR.W    R2,??DataTable53_2  ;; 0xe000e280
        LSRS     R0,R0,#+5
        STR      R1,[R2, R0, LSL #+2]
        BX       LR               ;; return
          CFI EndBlock cfiBlock2
//   36 #include "hal_sleep_manager_internal.h"
//   37 #include <stdlib.h>
//   38 #include <stdio.h>
//   39 #include <string.h>
//   40 
//   41 #ifdef HAL_SLEEP_MANAGER_ENABLED
//   42 #include <stdio.h>
//   43 #include <string.h>
//   44 #include "hal_log.h"
//   45 #include "memory_attribute.h"
//   46 #include "hal_nvic.h"
//   47 #include "hal_nvic_internal.h"
//   48 #include "assert.h"
//   49 #include "hal_emi_internal.h"
//   50 #include "hal_cache_internal.h"
//   51 #include "hal_flash_sf.h"
//   52 #include "hal_clock_internal.h"
//   53 #include "hal_gpt.h"
//   54 #include "hal_mpu_internal.h"
//   55 #include "hal_clock.h"
//   56 #include "hal_sleep_manager_retention.h"
//   57 
//   58 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//   59 #define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
//   60 #else
//   61 #define log_debug(_message,...)
//   62 #endif
//   63 

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   64 static sleep_management_handle_t sleep_management_handle = {
sleep_management_handle:
        DS8 240
//   65     .lock_sleep_request = 0,
//   66     .user_handle_resoure = 0,
//   67     .user_handle_count = 0
//   68 };
//   69 

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   70 static sleep_management_suspend_callback_func_t    suspend_callback_func_table      [SLEEP_BACKUP_RESTORE_MODULE_MAX];
suspend_callback_func_table:
        DS8 144

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   71 static sleep_management_resume_callback_func_t     resume_callback_func_table       [SLEEP_BACKUP_RESTORE_MODULE_MAX];
resume_callback_func_table:
        DS8 144

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   72 static sleep_management_suspend_callback_func_t    suspend_user_callback_func_table [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
suspend_user_callback_func_table:
        DS8 96
        DS8 4

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   73 static sleep_management_resume_callback_func_t     resume_user_callback_func_table  [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
resume_user_callback_func_table:
        DS8 96
        DS8 4
//   74 static uint32_t suspend_user_register_count = 0, resume_user_register_count = 0;
//   75 

        SECTION `.tcm_rwdata`:DATA:REORDER:NOROOT(2)
        DATA
//   76 ATTR_RWDATA_IN_TCM volatile uint32_t wakeup_source_status;
//   77 ATTR_RWDATA_IN_TCM volatile uint32_t Vector0_backup, temp_reg;
//   78 ATTR_RWDATA_IN_TCM volatile uint32_t origin_msp_bak_reg, origin_psp_bak_reg, backup_return_address;
//   79 ATTR_RWDATA_IN_TCM volatile CPU_CORE_BAKEUP_REG_T  cpu_core_reg ;
//   80 ATTR_RWDATA_IN_TCM volatile nvic_sleep_backup_register_t nvic_backup_register;
//   81 ATTR_RWDATA_IN_TCM volatile FPU_BAKEUP_REG_T  fpu_reg;
//   82 ATTR_RWDATA_IN_TCM volatile CMSYS_CFG_BAKEUP_REG_T  cmsys_cfg_reg;
//   83 ATTR_RWDATA_IN_TCM volatile CMSYS_CFG_EXT_BAKEUP_REG_T cmsys_cfg_ext_reg;
//   84 ATTR_RWDATA_IN_TCM volatile CM4_SYS_CTRL_BAKEUP_REG_T  cm4_sys_ctrl_reg ;
cm4_sys_ctrl_reg:
        DS8 36
cmsys_cfg_reg:
        DS8 24
fpu_reg:
        DS8 8
cmsys_cfg_ext_reg:
        DS8 8
wakeup_source_status:
        DS8 4
Vector0_backup:
        DS8 4
temp_reg:
        DS8 4
origin_msp_bak_reg:
        DS8 4
origin_psp_bak_reg:
        DS8 4
cpu_core_reg:
        DS8 20

        SECTION `.tcm_rwdata`:DATA:REORDER:NOROOT(2)
        DATA
backup_return_address:
        DS8 4

        SECTION `.tcm_rwdata`:DATA:REORDER:NOROOT(2)
        DATA
nvic_backup_register:
        DS8 136

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   85 uint32_t cli_dtim_sleep_mode = 0;
cli_dtim_sleep_mode:
        DS8 4
//   86 
//   87 /* for N9 API */

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   88 int8_t (*ptr_lp_connsys_get_own_enable_int)(void)  = NULL;
ptr_lp_connsys_get_own_enable_int:
        DS8 4

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   89 int8_t (*ptr_lp_connsys_give_n9_own)(void)         = NULL;
ptr_lp_connsys_give_n9_own:
        DS8 4

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//   90 uint8_t (*ptr_connsys_get_ownership)(void)          = NULL;
ptr_connsys_get_ownership:
        DS8 4
//   91 
//   92 #if     defined (__GNUC__)      //GCC disable compiler optimize
//   93 #pragma GCC push_options
//   94 #pragma GCC optimize ("O0")
//   95 #elif   defined (__ICCARM__)    //IAR disable compiler optimize
//   96 #pragma optimize=none
//   97 #elif   defined (__CC_ARM)      //MDK disable compiler optimize
//   98 #pragma push
//   99 #pragma diag_suppress 1267
//  100 #pragma O0
//  101 #endif

        SECTION `.tcm_code`:CODE:NOROOT(2)
          CFI Block cfiBlock3 Using cfiCommon0
          CFI Function sleep_management_enter_deep_sleep
        THUMB
//  102 ATTR_TEXT_IN_TCM void sleep_management_enter_deep_sleep(hal_sleep_mode_t mode)
//  103 {
sleep_management_enter_deep_sleep:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
        MOVS     R4,R0
//  104 #if defined (__CC_ARM)
//  105     /* Backup function return address(R14) */
//  106     __asm volatile("mov backup_return_address,__return_address() \n");
//  107 #endif
//  108 
//  109 #ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE
//  110     if ((*SPM_PWR_STATUS & 0x4) == 0) {
//  111         log_debug("[Sleep]MTCMOS CONN:Off\r\n");
//  112     } else {
//  113         log_debug("[Sleep]MTCMOS CONN:On\r\n");
//  114     }
//  115     if ((*SPM_PWR_STATUS & 0x8) == 0) {
//  116         log_debug("[Sleep]MTCMOS SDIO:Off\r\n");
//  117     } else {
//  118         log_debug("[Sleep]MTCMOS SDIO:On\r\n");
//  119     }
//  120     log_debug("[Sleep]Enter Deep Sleep\r\n");
//  121 #endif
//  122 
//  123     clock_suspend();
          CFI FunCall clock_suspend
        BL       clock_suspend
//  124 
//  125     /* Set Boot Slave */
//  126     *CMCFG_BOOT_FROM_SLV = 0x1;
        MOVS     R0,#+1
        LDR.W    R1,??DataTable75  ;; 0xa21e0008
        STR      R0,[R1, #+0]
//  127 
//  128     /* peripheral driver backup callback function */
//  129     sleep_management_suspend_callback();
          CFI FunCall sleep_management_suspend_callback
        BL       sleep_management_suspend_callback
//  130 
//  131     /* spm Kick start*/
//  132     spm_kick_start();
          CFI FunCall spm_kick_start
        BL       spm_kick_start
//  133 
//  134     /* enable spm irq and clear pending bits */
//  135     NVIC_EnableIRQ(SPM_IRQn);
        MOVS     R0,#+16
          CFI FunCall NVIC_EnableIRQ
        BL       NVIC_EnableIRQ
//  136     NVIC_ClearPendingIRQ(SPM_IRQn);
        MOVS     R0,#+16
          CFI FunCall NVIC_ClearPendingIRQ
        BL       NVIC_ClearPendingIRQ
//  137 
//  138     /* backup cmsys register */
//  139     deep_sleep_cmsys_backup();
          CFI FunCall deep_sleep_cmsys_backup
        BL       deep_sleep_cmsys_backup
//  140 
//  141     /* general register backup */
//  142     __CUP_STACK_POINT_BACKUP(origin_psp_bak_reg, origin_msp_bak_reg);
        MOV R0, sp
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+92]
        MOV r0, #0
        MSR control, r0
        MSR control, r0
        MOV R0, sp
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+88]
//  143 
//  144     /* Set CM4 SLEEPDEEP bits */
//  145     *CM4_SYSTEM_CONTROL = *CM4_SYSTEM_CONTROL | 0x4;
        LDR.N    R0,??DataTable75_2  ;; 0xe000ed10
        LDR      R0,[R0, #+0]
        ORRS     R0,R0,#0x4
        LDR.N    R1,??DataTable75_2  ;; 0xe000ed10
        STR      R0,[R1, #+0]
//  146 
//  147     /* Flash Enter Powerdown Mode */
//  148 #ifdef HAL_FLASH_MODULE_ENABLED
//  149     SF_DAL_DEV_Enter_DPD();
          CFI FunCall SF_DAL_DEV_Enter_DPD
        BL       SF_DAL_DEV_Enter_DPD
//  150 #endif
//  151     /* PSRAM Enter Sleep Mode */
//  152     mtk_psram_half_sleep_enter();
          CFI FunCall mtk_psram_half_sleep_enter
        BL       mtk_psram_half_sleep_enter
//  153 
//  154     /* backup BootVector0 Stack Address */
//  155     Vector0_backup = *CMCFG_BOOT_VECTOR0;   //boot vector 0(boot slave stack point)
        LDR.N    R0,??DataTable75_3  ;; 0xa21e0000
        LDR      R0,[R0, #+0]
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+80]
//  156 
//  157     /* backup MSP Address */
//  158 #if (defined (__GNUC__) || defined (__ICCARM__))
//  159     __asm volatile("push {r0-r12, lr}");
        push {r0-r12, lr}
//  160     __asm volatile("mov %0, sp" : "=r"(temp_reg));
        mov R0, sp
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+84]
//  161 #elif defined (__CC_ARM)
//  162     __PUSH_CUP_REG();
//  163     __BACKUP_SP(temp_reg);
//  164 #endif
//  165     *CMCFG_BOOT_VECTOR0 = temp_reg;  //VECTOR0 write MSP Address
        LDR.N    R0,??DataTable75_1
        LDR      R0,[R0, #+84]
        LDR.N    R1,??DataTable75_3  ;; 0xa21e0000
        STR      R0,[R1, #+0]
//  166 
//  167     if (*SPM_SLEEP_ISR_STATUS == 1) {
        LDR.N    R0,??DataTable75_4  ;; 0xa20f0820
        LDR      R0,[R0, #+0]
        CMP      R0,#+1
        BNE.N    ??sleep_management_enter_deep_sleep_0
//  168         *SPM_PCM_CON0 = *SPM_PCM_CON0 & 0x1E; // [0]: kick PCM process
        LDR.N    R0,??DataTable75_5  ;; 0xa20f0300
        LDR      R0,[R0, #+0]
        ANDS     R0,R0,#0x1E
        LDR.N    R1,??DataTable75_5  ;; 0xa20f0300
        STR      R0,[R1, #+0]
//  169         *SPM_SLEEP_ISR_STATUS = 1;
        MOVS     R0,#+1
        LDR.N    R1,??DataTable75_4  ;; 0xa20f0820
        STR      R0,[R1, #+0]
//  170         *SPM_SLEEP_ISR_STATUS = 0;
        MOVS     R0,#+0
        LDR.N    R1,??DataTable75_4  ;; 0xa20f0820
        STR      R0,[R1, #+0]
        B.N      ??sleep_management_enter_deep_sleep_1
//  171     } else {
//  172         /* Enter Deep Sleep */
//  173         temp_reg = CMCFG_BOOT_VECTOR1;  //CMCFG_BOOT_VECTOR1 Address
??sleep_management_enter_deep_sleep_0:
        LDR.N    R0,??DataTable75_6  ;; 0xa21e0004
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+84]
//  174         __ENTER_DEEP_SLEEP(temp_reg);
        LDR.N    R0,??DataTable75_1
        LDR      R0,[R0, #+84]
        MOV    r3,         R0              
        MOV    r2,         pc              
        ADD    r2,         r2,#16          
        ORR    r2,         r2,#1           
        STR    r2,         [r3]            
        DSB                                
        WFI                                
        NOP                                
        ISB                                
        LDR    r1,         =0xE0181000     
        LDR    r2,         =0x10000023     
        STR    r2,         [r1]            
        LDR    r1,         =0xE0181004     
        LDR    r2,         =0              
        STR    r2,         [r1]            
        LDR    r1,         =0xE0181008     
        LDR    r2,         =0x14200015     
        STR    r2,         [r1]            
        LDR    r1,         =0xE018100C     
        LDR    r2,         =0x04200000     
        STR    r2,         [r1]            
        POP    {r0-r12,lr}                   
//  175     }
//  176     /* pop CPU Reg R0-R12 */
//  177 #if defined (__CC_ARM)
//  178     __POP_CUP_REG();
//  179 #endif
//  180 
//  181     /* wait protect_en release */
//  182     while (((*(volatile uint32_t *)(0xA21F0010)) & 0x1000100) != 0);
??sleep_management_enter_deep_sleep_1:
        LDR.N    R0,??DataTable75_7  ;; 0xa21f0010
        LDR      R0,[R0, #+0]
        TST      R0,#0x1000100
        BNE.N    ??sleep_management_enter_deep_sleep_1
//  183     /* add 2us delay for protect_en ready (40Mhz Xtal) */
//  184     hal_gpt_delay_us(2);
        MOVS     R0,#+2
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  185 
//  186     /* PSRAM Leave Sleep Mode */
//  187     mtk_psram_half_sleep_exit();
          CFI FunCall mtk_psram_half_sleep_exit
        BL       mtk_psram_half_sleep_exit
//  188 
//  189     /* Flash Leave Powerdown Mode */
//  190 #ifdef HAL_FLASH_MODULE_ENABLED
//  191     SF_DAL_DEV_Leave_DPD();
          CFI FunCall SF_DAL_DEV_Leave_DPD
        BL       SF_DAL_DEV_Leave_DPD
//  192 #endif
//  193     /* get wakeup source */
//  194     wakeup_source_status = *SPM_WAKEUP_SOURCE_STA;
        LDR.N    R0,??DataTable75_8  ;; 0xa20f0800
        LDR      R0,[R0, #+0]
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+76]
//  195 
//  196     /* restore MSP */
//  197     temp_reg = (unsigned int)&origin_msp_bak_reg;
        LDR.N    R0,??DataTable75_9
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+84]
//  198     __MSP_RESTORE(temp_reg);
        LDR.N    R0,??DataTable75_1
        LDR      R0,[R0, #+84]
        MOV    r2,         R0              
        MOV    r0,         r2              
        LDR    r0,         [r0]            
        MSR    msp,        r0                
//  199 
//  200     /* swtich stack point to psp */
//  201     __SWITCH_TO_PSP_STACK_POINT();
        MOV    r0,         #4              
        MSR    control,    r0              
        DSB                                
        MOV    r0,         #6              
        MSR    control,    r0              
        DSB                                  
//  202 
//  203     /* restore PSP */
//  204     temp_reg = (unsigned int)&origin_psp_bak_reg;
        LDR.N    R0,??DataTable75_10
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+84]
//  205     __PSP_RESTORE(temp_reg);
        LDR.N    R0,??DataTable75_1
        LDR      R0,[R0, #+84]
        MOV    r2,         R0              
        MOV    r0,         r2              
        LDR    r0,         [r0]            
        MSR    psp,        r0                
//  206 
//  207     /* restore Core register - CONTROL */
//  208     temp_reg = (unsigned int)&cpu_core_reg.CONTROL;
        LDR.N    R0,??DataTable75_11
        ADDS     R0,R0,#+16
        LDR.N    R1,??DataTable75_1
        STR      R0,[R1, #+84]
//  209     __CPU_CORE_CONTROL_REG_RESTORE(temp_reg);
        LDR.N    R0,??DataTable75_1
        LDR      R0,[R0, #+84]
        MOV    r2,         R0              
        MOV    r0,         r2              
        LDR    r1,         [r0],#0         
        MSR    control,    r1              
//  210 
//  211     /* restore boot Vector */
//  212     *CMCFG_BOOT_FROM_SLV = 0x0;
        MOVS     R0,#+0
        LDR.N    R1,??DataTable75  ;; 0xa21e0008
        STR      R0,[R1, #+0]
//  213     *CMCFG_BOOT_VECTOR0 = Vector0_backup ;
        LDR.N    R0,??DataTable75_1
        LDR      R0,[R0, #+80]
        LDR.N    R1,??DataTable75_3  ;; 0xa21e0000
        STR      R0,[R1, #+0]
//  214 
//  215     /* Clear CM4 Deep Sleep bits */
//  216     *CM4_SYSTEM_CONTROL = *CM4_SYSTEM_CONTROL & (~0x4);
        LDR.N    R0,??DataTable75_2  ;; 0xe000ed10
        LDR      R0,[R0, #+0]
        BICS     R0,R0,#0x4
        LDR.N    R1,??DataTable75_2  ;; 0xe000ed10
        STR      R0,[R1, #+0]
//  217 
//  218     /* restore cmsys register */
//  219     deep_sleep_cmsys_restore();
          CFI FunCall deep_sleep_cmsys_restore
        BL       deep_sleep_cmsys_restore
//  220 
//  221     /* disable spm irq and clear pending bits */
//  222     NVIC_DisableIRQ(SPM_IRQn);
        MOVS     R0,#+16
          CFI FunCall NVIC_DisableIRQ
        BL       NVIC_DisableIRQ
//  223     NVIC_ClearPendingIRQ(SPM_IRQn);
        MOVS     R0,#+16
          CFI FunCall NVIC_ClearPendingIRQ
        BL       NVIC_ClearPendingIRQ
//  224 
//  225     clock_resume();
          CFI FunCall clock_resume
        BL       clock_resume
//  226 
//  227     /* peripheral driver restore callback function */
//  228     sleep_management_resume_callback();
          CFI FunCall sleep_management_resume_callback
        BL       sleep_management_resume_callback
//  229 
//  230     /* disable spm clock*/
//  231     hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
          CFI FunCall hal_clock_disable
        BL       hal_clock_disable
//  232 
//  233 #if defined (__CC_ARM)
//  234     __RESTORE_LR(backup_return_address);
//  235 #endif
//  236 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock3
//  237 #if     defined (__GNUC__)
//  238 #pragma GCC push_options
//  239 #elif   defined (__CC_ARM)
//  240 #pragma pop
//  241 #endif
//  242 

        SECTION `.tcm_code`:CODE:NOROOT(2)
          CFI Block cfiBlock4 Using cfiCommon0
          CFI Function deep_sleep_cmsys_backup
        THUMB
//  243 ATTR_TEXT_IN_TCM inline void deep_sleep_cmsys_backup()
//  244 {
deep_sleep_cmsys_backup:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  245     uint32_t i;
//  246     /* backup CPU core registers */
//  247     temp_reg = (unsigned int)&cpu_core_reg;
        LDR.N    R4,??DataTable75_1
        ADD      R0,R4,#+96
        STR      R0,[R4, #+84]
//  248     __CPU_CORE_REG_BACKUP(temp_reg);
        LDR      R0,[R4, #+84]
        PUSH   {r0-r12, lr}                
        MOV    r2,         R0              
        MOV    r0,         r2              
        MRS    r1,         psr             
        STR    r1,         [r0],#4         
        MRS    r1,         primask         
        STR    r1,         [r0],#4         
        MRS    r1,         faultmask       
        STR    r1,         [r0],#4         
        MRS    r1,         basepri         
        STR    r1,         [r0],#4         
        MRS    r1,         control         
        STR    r1,         [r0]            
        POP    {r0-r12,lr}                   
//  249 
//  250     /* NVIC backup */
//  251     nvic_backup_register.nvic_iser = NVIC->ISER[0];
        LDR.N    R0,??DataTable75_12
        LDR.N    R1,??DataTable75_13  ;; 0xe000e100
        LDR      R2,[R1, #+0]
        STR      R2,[R0, #+0]
//  252     nvic_backup_register.nvic_iser1 = NVIC->ISER[1];
        LDR      R1,[R1, #+4]
        STR      R1,[R0, #+4]
//  253     for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        MOVS     R1,#+0
        B.N      ??deep_sleep_cmsys_backup_0
//  254         nvic_backup_register.nvic_ip[i] = NVIC->IP[i];
??deep_sleep_cmsys_backup_1:
        LDR.N    R2,??DataTable75_14  ;; 0xe000e400
        LDRB     R2,[R1, R2]
        ADD      R3,R0,R1, LSL #+2
        STR      R2,[R3, #+8]
//  255     }
        ADDS     R1,R1,#+1
??deep_sleep_cmsys_backup_0:
        CMP      R1,#+32
        BCC.N    ??deep_sleep_cmsys_backup_1
//  256 
//  257     /* cache backcp */
//  258 #ifdef HAL_CACHE_MODULE_ENABLED
//  259     cache_status_save();
          CFI FunCall cache_status_save
        BL       cache_status_save
//  260 #endif
//  261 
//  262     /* mpu backcp */
//  263 #ifdef HAL_MPU_MODULE_ENABLED
//  264     mpu_status_save();
          CFI FunCall mpu_status_save
        BL       mpu_status_save
//  265 #endif
//  266 
//  267     /* cmsys config backup */
//  268     cmsys_cfg_reg.STCALIB = CMSYS_CFG->STCALIB;
        LDR.N    R0,??DataTable75_15  ;; 0xe0100004
        LDR      R1,[R0, #+0]
        STR      R1,[R4, #+36]
//  269     cmsys_cfg_reg.AHB_BUFFERALBE = CMSYS_CFG->AHB_BUFFERALBE;
        LDR      R1,[R0, #+16]
        STR      R1,[R4, #+40]
//  270     cmsys_cfg_reg.AHB_FIFO_TH = CMSYS_CFG->AHB_FIFO_TH;
        LDR      R1,[R0, #+20]
        STR      R1,[R4, #+44]
//  271     cmsys_cfg_reg.INT_ACTIVE_HL0 = CMSYS_CFG->INT_ACTIVE_HL0;
        LDR      R1,[R0, #+48]
        STR      R1,[R4, #+48]
//  272     cmsys_cfg_reg.INT_ACTIVE_HL1 = CMSYS_CFG->INT_ACTIVE_HL1;
        LDR      R1,[R0, #+52]
        STR      R1,[R4, #+52]
//  273     cmsys_cfg_reg.DCM_CTRL_REG = CMSYS_CFG->DCM_CTRL_REG;
        LDR      R0,[R0, #+60]
        STR      R0,[R4, #+56]
//  274 
//  275     cmsys_cfg_ext_reg.CG_EN = CMSYS_CFG_EXT->CG_EN;
        LDR.N    R0,??DataTable75_16  ;; 0xe00fe000
        LDR      R1,[R0, #+0]
        STR      R1,[R4, #+68]
//  276     cmsys_cfg_ext_reg.DCM_EN = CMSYS_CFG_EXT->DCM_EN;
        LDR      R0,[R0, #+4]
        STR      R0,[R4, #+72]
//  277 
//  278     /* fpu backup */
//  279     fpu_reg.FPCCR = FPU->FPCCR;
        LDR.N    R0,??DataTable75_17  ;; 0xe000ef34
        LDR      R1,[R0, #+0]
        STR      R1,[R4, #+60]
//  280     fpu_reg.FPCAR = FPU->FPCAR;
        LDR      R0,[R0, #+4]
        STR      R0,[R4, #+64]
//  281 
//  282     /* CM4 system control registers backup */
//  283     cm4_sys_ctrl_reg.ACTLR = SCnSCB->ACTLR;
        LDR.N    R0,??DataTable75_18  ;; 0xe000e008
        LDR      R0,[R0, #+0]
        STR      R0,[R4, #+0]
//  284     cm4_sys_ctrl_reg.VTOR = SCB->VTOR;
        LDR.N    R0,??DataTable75_19  ;; 0xe000ed08
        LDR      R1,[R0, #+0]
        STR      R1,[R4, #+4]
//  285     cm4_sys_ctrl_reg.SCR = SCB->SCR;
        LDR      R1,[R0, #+8]
        STR      R1,[R4, #+8]
//  286     cm4_sys_ctrl_reg.CCR = SCB->CCR;
        LDR      R1,[R0, #+12]
        STR      R1,[R4, #+12]
//  287 
//  288     cm4_sys_ctrl_reg.SHP[0] = SCB->SHP[0]; /* MemMange */
        LDRB     R1,[R0, #+16]
        STRB     R1,[R4, #+16]
//  289     cm4_sys_ctrl_reg.SHP[1] = SCB->SHP[1]; /* BusFault */
        LDRB     R1,[R0, #+17]
        STRB     R1,[R4, #+17]
//  290     cm4_sys_ctrl_reg.SHP[2] = SCB->SHP[2]; /* UsageFault */
        LDRB     R1,[R0, #+18]
        STRB     R1,[R4, #+18]
//  291     cm4_sys_ctrl_reg.SHP[7] = SCB->SHP[7]; /* SVCall */
        LDRB     R1,[R0, #+23]
        STRB     R1,[R4, #+23]
//  292     cm4_sys_ctrl_reg.SHP[8] = SCB->SHP[8]; /* DebugMonitor */
        LDRB     R1,[R0, #+24]
        STRB     R1,[R4, #+24]
//  293     cm4_sys_ctrl_reg.SHP[10] = SCB->SHP[10]; /* PendSV */
        LDRB     R1,[R0, #+26]
        STRB     R1,[R4, #+26]
//  294     cm4_sys_ctrl_reg.SHP[11] = SCB->SHP[11]; /* SysTick */
        LDRB     R1,[R0, #+27]
        STRB     R1,[R4, #+27]
//  295 
//  296     cm4_sys_ctrl_reg.SHCSR = SCB->SHCSR;
        LDR      R0,[R0, #+28]
        STR      R0,[R4, #+28]
//  297     cm4_sys_ctrl_reg.CPACR = SCB->CPACR;
        LDR.N    R0,??DataTable75_20  ;; 0xe000ed88
        LDR      R0,[R0, #+0]
        STR      R0,[R4, #+32]
//  298 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock4
//  299 

        SECTION `.tcm_code`:CODE:NOROOT(2)
          CFI Block cfiBlock5 Using cfiCommon0
          CFI Function deep_sleep_cmsys_restore
        THUMB
//  300 ATTR_TEXT_IN_TCM inline void deep_sleep_cmsys_restore()
//  301 {
deep_sleep_cmsys_restore:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  302     uint32_t i;
//  303 
//  304     /* CM4 system control registers restore */
//  305     SCnSCB->ACTLR = cm4_sys_ctrl_reg.ACTLR;
        LDR.N    R4,??DataTable75_1
        LDR      R0,[R4, #+0]
        LDR.N    R1,??DataTable75_18  ;; 0xe000e008
        STR      R0,[R1, #+0]
//  306     SCB->VTOR = cm4_sys_ctrl_reg.VTOR;
        LDR.N    R0,??DataTable75_19  ;; 0xe000ed08
        LDR      R1,[R4, #+4]
        STR      R1,[R0, #+0]
//  307     SCB->SCR = cm4_sys_ctrl_reg.SCR;
        LDR      R1,[R4, #+8]
        STR      R1,[R0, #+8]
//  308     SCB->CCR = cm4_sys_ctrl_reg.CCR;
        LDR      R1,[R4, #+12]
        STR      R1,[R0, #+12]
//  309     SCB->SHP[0] = cm4_sys_ctrl_reg.SHP[0]; /* MemMange */
        LDRB     R1,[R4, #+16]
        STRB     R1,[R0, #+16]
//  310     SCB->SHP[1] = cm4_sys_ctrl_reg.SHP[1]; /* BusFault */
        LDRB     R1,[R4, #+17]
        STRB     R1,[R0, #+17]
//  311     SCB->SHP[2] = cm4_sys_ctrl_reg.SHP[2]; /* UsageFault */
        LDRB     R1,[R4, #+18]
        STRB     R1,[R0, #+18]
//  312     SCB->SHP[7] = cm4_sys_ctrl_reg.SHP[7]; /* SVCall */
        LDRB     R1,[R4, #+23]
        STRB     R1,[R0, #+23]
//  313     SCB->SHP[8] = cm4_sys_ctrl_reg.SHP[8]; /* DebugMonitor */
        LDRB     R1,[R4, #+24]
        STRB     R1,[R0, #+24]
//  314     SCB->SHP[10] = cm4_sys_ctrl_reg.SHP[10]; /* PendSV */
        LDRB     R1,[R4, #+26]
        STRB     R1,[R0, #+26]
//  315     SCB->SHP[11] = cm4_sys_ctrl_reg.SHP[11]; /* SysTick */
        LDRB     R1,[R4, #+27]
        STRB     R1,[R0, #+27]
//  316     SCB->SHCSR = cm4_sys_ctrl_reg.SHCSR;
        LDR      R1,[R4, #+28]
        STR      R1,[R0, #+28]
//  317     SCB->CPACR = cm4_sys_ctrl_reg.CPACR;
        LDR      R0,[R4, #+32]
        LDR.N    R1,??DataTable75_20  ;; 0xe000ed88
        STR      R0,[R1, #+0]
//  318 
//  319     /* fpu restore */
//  320     FPU->FPCCR = fpu_reg.FPCCR;
        LDR.N    R0,??DataTable75_17  ;; 0xe000ef34
        LDR      R1,[R4, #+60]
        STR      R1,[R0, #+0]
//  321     FPU->FPCAR = fpu_reg.FPCAR;
        LDR      R1,[R4, #+64]
        STR      R1,[R0, #+4]
//  322 
//  323     /* cmsys config restore */
//  324     CMSYS_CFG->STCALIB = cmsys_cfg_reg.STCALIB;
        LDR.N    R0,??DataTable75_15  ;; 0xe0100004
        LDR      R1,[R4, #+36]
        STR      R1,[R0, #+0]
//  325     CMSYS_CFG->AHB_BUFFERALBE = cmsys_cfg_reg.AHB_BUFFERALBE;
        LDR      R1,[R4, #+40]
        STR      R1,[R0, #+16]
//  326     CMSYS_CFG->AHB_FIFO_TH = cmsys_cfg_reg.AHB_FIFO_TH;
        LDR      R1,[R4, #+44]
        STR      R1,[R0, #+20]
//  327     CMSYS_CFG->INT_ACTIVE_HL0 = cmsys_cfg_reg.INT_ACTIVE_HL0;
        LDR      R1,[R4, #+48]
        STR      R1,[R0, #+48]
//  328     CMSYS_CFG->INT_ACTIVE_HL1 = cmsys_cfg_reg.INT_ACTIVE_HL1;
        LDR      R1,[R4, #+52]
        STR      R1,[R0, #+52]
//  329     CMSYS_CFG->DCM_CTRL_REG = cmsys_cfg_reg.DCM_CTRL_REG;
        LDR      R1,[R4, #+56]
        STR      R1,[R0, #+60]
//  330 
//  331     CMSYS_CFG_EXT->CG_EN = cmsys_cfg_ext_reg.CG_EN;
        LDR.N    R0,??DataTable75_16  ;; 0xe00fe000
        LDR      R1,[R4, #+68]
        STR      R1,[R0, #+0]
//  332     CMSYS_CFG_EXT->DCM_EN = cmsys_cfg_ext_reg.DCM_EN;
        LDR      R1,[R4, #+72]
        STR      R1,[R0, #+4]
//  333 
//  334     /* mpu restore */
//  335 #ifdef HAL_MPU_MODULE_ENABLED
//  336     mpu_status_restore();
          CFI FunCall mpu_status_restore
        BL       mpu_status_restore
//  337 #endif
//  338 
//  339     /* cache restore */
//  340 #ifdef HAL_CACHE_MODULE_ENABLED
//  341     cache_status_restore();
          CFI FunCall cache_status_restore
        BL       cache_status_restore
//  342 #endif
//  343 
//  344     /* restore CPU core registers */
//  345     temp_reg = (unsigned int)&cpu_core_reg;
        ADD      R0,R4,#+96
        STR      R0,[R4, #+84]
//  346     __CPU_CORE_REG_RESTORE(temp_reg);
        LDR      R0,[R4, #+84]
        PUSH   {r0-r12, lr}                
        MOV    r2,         R0              
        MOV    r0,         r2              
        LDR    r1, [r0],   #4              
        MSR    psr,        r1              
        LDR    r1, [r0],   #4              
        MSR    primask,    r1              
        LDR    r1, [r0],   #4              
        MSR    faultmask,  r1              
        LDR    r1, [r0],   #4              
        MSR    basepri,    r1              
        POP    {r0-r12,lr}                   
//  347 
//  348     /* NVIC restore */
//  349     NVIC->ISER[0] = nvic_backup_register.nvic_iser;
        LDR.N    R0,??DataTable75_13  ;; 0xe000e100
        LDR.N    R1,??DataTable75_12
        LDR      R2,[R1, #+0]
        STR      R2,[R0, #+0]
//  350     NVIC->ISER[1] = nvic_backup_register.nvic_iser1;
        LDR      R2,[R1, #+4]
        STR      R2,[R0, #+4]
//  351     for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        MOVS     R0,#+0
        LDR.N    R3,??DataTable75_14  ;; 0xe000e400
        B.N      ??deep_sleep_cmsys_restore_0
//  352         NVIC->IP[i] = nvic_backup_register.nvic_ip[i];
??deep_sleep_cmsys_restore_1:
        ADD      R2,R1,R0, LSL #+2
        LDR      R2,[R2, #+8]
        STRB     R2,[R0, R3]
//  353     }
        ADDS     R0,R0,#+1
??deep_sleep_cmsys_restore_0:
        CMP      R0,#+32
        BCC.N    ??deep_sleep_cmsys_restore_1
//  354 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock5
//  355 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock6 Using cfiCommon0
          CFI Function sleep_management_register_suspend_callback
        THUMB
//  356 void sleep_management_register_suspend_callback(sleep_management_backup_restore_module_t module , sleep_management_suspend_callback_t callback , void *data)
//  357 {
sleep_management_register_suspend_callback:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  358     if (module == SLEEP_BACKUP_RESTORE_USER) {
        CMP      R0,#+13
        BNE.N    ??sleep_management_register_suspend_callback_0
//  359         if (suspend_user_register_count < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX) {
        LDR.W    R3,??DataTable53_3
        LDR      R4,[R3, #+96]
        CMP      R4,#+8
        BCS.N    ??sleep_management_register_suspend_callback_1
//  360             suspend_user_register_count++;
        ADDS     R4,R4,#+1
        STR      R4,[R3, #+96]
//  361             suspend_user_callback_func_table[module].func        = callback;
        ADD      R4,R0,R0, LSL #+1
        ADDS     R0,R3,R4, LSL #+2
        STR      R1,[R0, #+0]
//  362             suspend_user_callback_func_table[module].data        = data;
        STR      R2,[R0, #+4]
//  363             suspend_user_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
        MOVS     R1,#+1
        STRB     R1,[R0, #+8]
        POP      {R4,PC}
//  364         } else {
//  365             log_hal_error("[Sleep Management]register suspend callback function overflow\r\n");
??sleep_management_register_suspend_callback_1:
        ADR.W    R2,?_8
        MOVW     R1,#+365
        ADR.W    R0,??__FUNCTION__
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  366             assert(0);
        MOV      R2,#+366
        ADR.W    R1,?_10
        ADR.N    R0,??DataTable47  ;; "0"
          CFI FunCall __aeabi_assert
        BL       __aeabi_assert
          CFI FunCall __iar_EmptyStepPoint
        BL       __iar_EmptyStepPoint
        POP      {R4,PC}
//  367         }
//  368     } else {
//  369         suspend_callback_func_table[module].func        = callback;
??sleep_management_register_suspend_callback_0:
        ADD      R3,R0,R0, LSL #+1
        LSLS     R0,R3,#+2
        LDR.W    R3,??DataTable53_4
        ADDS     R0,R0,R3
        STR      R1,[R0, #+0]
//  370         suspend_callback_func_table[module].data        = data;
        STR      R2,[R0, #+4]
//  371         suspend_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
        MOVS     R1,#+1
        STRB     R1,[R0, #+8]
//  372     }
//  373 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock6
//  374 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock7 Using cfiCommon0
          CFI Function sleep_management_register_resume_callback
        THUMB
//  375 void sleep_management_register_resume_callback(sleep_management_backup_restore_module_t module , sleep_management_resume_callback_t callback , void *data)
//  376 {
sleep_management_register_resume_callback:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  377     if (module == SLEEP_BACKUP_RESTORE_USER) {
        CMP      R0,#+13
        BNE.N    ??sleep_management_register_resume_callback_0
//  378         if (resume_user_register_count < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX) {
        LDR.W    R3,??DataTable53_5
        LDR      R4,[R3, #+96]
        CMP      R4,#+8
        BCS.N    ??sleep_management_register_resume_callback_1
//  379             resume_user_register_count++;
        ADDS     R4,R4,#+1
        STR      R4,[R3, #+96]
//  380             resume_user_callback_func_table[module].func        = callback;
        ADD      R4,R0,R0, LSL #+1
        ADDS     R0,R3,R4, LSL #+2
        STR      R1,[R0, #+0]
//  381             resume_user_callback_func_table[module].data        = data;
        STR      R2,[R0, #+4]
//  382             resume_user_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
        MOVS     R1,#+1
        STRB     R1,[R0, #+8]
        POP      {R4,PC}
//  383         } else {
//  384             log_hal_error("[Sleep Management]register resume callback function overflow\r\n");
??sleep_management_register_resume_callback_1:
        ADR.W    R2,?_11
        MOV      R1,#+384
        ADR.W    R0,??__FUNCTION___1
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  385             assert(0);
        MOVW     R2,#+385
        ADR.W    R1,?_10
        ADR.N    R0,??DataTable47  ;; "0"
          CFI FunCall __aeabi_assert
        BL       __aeabi_assert
          CFI FunCall __iar_EmptyStepPoint
        BL       __iar_EmptyStepPoint
        POP      {R4,PC}
//  386         }
//  387     } else {
//  388         resume_callback_func_table[module].func = callback;
??sleep_management_register_resume_callback_0:
        ADD      R3,R0,R0, LSL #+1
        LSLS     R0,R3,#+2
        LDR.W    R3,??DataTable53_6
        ADDS     R0,R0,R3
        STR      R1,[R0, #+0]
//  389         resume_callback_func_table[module].data = data;
        STR      R2,[R0, #+4]
//  390         resume_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
        MOVS     R1,#+1
        STRB     R1,[R0, #+8]
//  391     }
//  392 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock7
//  393 

        SECTION `.tcm_code`:CODE:NOROOT(1)
          CFI Block cfiBlock8 Using cfiCommon0
          CFI Function sleep_management_suspend_callback
        THUMB
//  394 ATTR_TEXT_IN_TCM void sleep_management_suspend_callback()
//  395 {
sleep_management_suspend_callback:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  396     static uint32_t i;
//  397 
//  398     for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        LDR.N    R4,??DataTable75_21
        MOVS     R0,#+0
        STR      R0,[R4, #+0]
        B.N      ??sleep_management_suspend_callback_0
//  399         if (suspend_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {
??sleep_management_suspend_callback_1:
        LDR.N    R1,??DataTable75_22
        ADD      R2,R0,R0, LSL #+1
        ADD      R1,R1,R2, LSL #+2
        LDRB     R0,[R1, #+8]
        CMP      R0,#+0
        BEQ.N    ??sleep_management_suspend_callback_2
//  400 
//  401 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  402             sleep_management_debug_backup_restore_fun_timelog(0, 0, i);
//  403 #endif
//  404             suspend_callback_func_table[i].func(suspend_callback_func_table[i].data);
        LDR      R0,[R1, #+4]
        LDR      R1,[R1, #+0]
          CFI FunCall
        BLX      R1
//  405 
//  406 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  407             sleep_management_debug_backup_restore_fun_timelog(0, 1, i);
//  408 #endif
//  409         }
//  410     }
??sleep_management_suspend_callback_2:
        LDR      R0,[R4, #+0]
        ADDS     R0,R0,#+1
        STR      R0,[R4, #+0]
??sleep_management_suspend_callback_0:
        LDR      R0,[R4, #+0]
        CMP      R0,#+12
        BCC.N    ??sleep_management_suspend_callback_1
//  411 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock8

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
??i:
        DS8 4
//  412 
//  413 

        SECTION `.tcm_code`:CODE:NOROOT(1)
          CFI Block cfiBlock9 Using cfiCommon0
          CFI Function sleep_management_resume_callback
        THUMB
//  414 ATTR_TEXT_IN_TCM void sleep_management_resume_callback()
//  415 {
sleep_management_resume_callback:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  416     static uint32_t i;
//  417 
//  418     for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        LDR.N    R4,??DataTable75_23
        MOVS     R0,#+0
        STR      R0,[R4, #+0]
        B.N      ??sleep_management_resume_callback_0
//  419         if (resume_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {
??sleep_management_resume_callback_1:
        LDR.N    R1,??DataTable75_24
        ADD      R2,R0,R0, LSL #+1
        ADD      R1,R1,R2, LSL #+2
        LDRB     R0,[R1, #+8]
        CMP      R0,#+0
        BEQ.N    ??sleep_management_resume_callback_2
//  420 
//  421 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  422             sleep_management_debug_backup_restore_fun_timelog(1, 0, i);
//  423 #endif
//  424 
//  425             resume_callback_func_table[i].func(resume_callback_func_table[i].data);
        LDR      R0,[R1, #+4]
        LDR      R1,[R1, #+0]
          CFI FunCall
        BLX      R1
//  426 
//  427 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  428             sleep_management_debug_backup_restore_fun_timelog(1, 1, i);
//  429 #endif
//  430         }
//  431     }
??sleep_management_resume_callback_2:
        LDR      R0,[R4, #+0]
        ADDS     R0,R0,#+1
        STR      R0,[R4, #+0]
??sleep_management_resume_callback_0:
        LDR      R0,[R4, #+0]
        CMP      R0,#+12
        BCC.N    ??sleep_management_resume_callback_1
//  432 }
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock9

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75:
        DC32     0xa21e0008

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_1:
        DC32     cm4_sys_ctrl_reg

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_2:
        DC32     0xe000ed10

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_3:
        DC32     0xa21e0000

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_4:
        DC32     0xa20f0820

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_5:
        DC32     0xa20f0300

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_6:
        DC32     0xa21e0004

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_7:
        DC32     0xa21f0010

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_8:
        DC32     0xa20f0800

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_9:
        DC32     cm4_sys_ctrl_reg+0x58

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_10:
        DC32     cm4_sys_ctrl_reg+0x5C

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_11:
        DC32     cm4_sys_ctrl_reg+0x60

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_12:
        DC32     nvic_backup_register

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_13:
        DC32     0xe000e100

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_14:
        DC32     0xe000e400

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_15:
        DC32     0xe0100004

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_16:
        DC32     0xe00fe000

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_17:
        DC32     0xe000ef34

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_18:
        DC32     0xe000e008

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_19:
        DC32     0xe000ed08

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_20:
        DC32     0xe000ed88

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_21:
        DC32     ??i

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_22:
        DC32     suspend_callback_func_table

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_23:
        DC32     ??i_1

        SECTION `.tcm_code`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable75_24:
        DC32     resume_callback_func_table

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
??i_1:
        DS8 4
//  433 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock10 Using cfiCommon0
          CFI Function sleep_management_get_lock_handle
        THUMB
//  434 uint8_t sleep_management_get_lock_handle(const char *handle_name)
//  435 {
sleep_management_get_lock_handle:
        PUSH     {R3-R9,LR}
          CFI R14 Frame(CFA, -4)
          CFI R9 Frame(CFA, -8)
          CFI R8 Frame(CFA, -12)
          CFI R7 Frame(CFA, -16)
          CFI R6 Frame(CFA, -20)
          CFI R5 Frame(CFA, -24)
          CFI R4 Frame(CFA, -28)
          CFI CFA R13+32
        MOV      R7,R0
//  436     uint8_t index = 0, i;
//  437     uint32_t mask, name_len;
//  438 
//  439     mask = save_and_set_interrupt_mask();
          CFI FunCall save_and_set_interrupt_mask
        BL       save_and_set_interrupt_mask
        MOV      R6,R0
//  440     for (index = 0 ; index < SLEEP_LOCK_HANDLE_USER_MAX; index++) {
        MOVS     R4,#+0
        LDR.W    R5,??DataTable53_7
        LDR      R0,[R5, #+4]
        B.N      ??sleep_management_get_lock_handle_0
??sleep_management_get_lock_handle_1:
        ADDS     R4,R4,#+1
        UXTB     R4,R4
??sleep_management_get_lock_handle_0:
        CMP      R4,#+18
        BGE.N    ??sleep_management_get_lock_handle_2
//  441         if (((sleep_management_handle.user_handle_resoure >> index) & 0x01) == 0) {
        MOV      R1,R0
        LSRS     R1,R1,R4
        LSLS     R1,R1,#+31
        BMI.N    ??sleep_management_get_lock_handle_1
//  442             sleep_management_handle.user_handle_resoure |= (1 << index);
        MOVS     R1,#+1
        LSLS     R1,R1,R4
        ORRS     R0,R1,R0
        STR      R0,[R5, #+4]
//  443             sleep_management_handle.user_handle_count++;
        LDRB     R0,[R5, #+8]
        ADDS     R0,R0,#+1
        STRB     R0,[R5, #+8]
//  444 
//  445             memset(&sleep_management_handle.user_handle_name[index][0], 0, SLEEP_HANDLE_NAME_LEN);
        MOVS     R0,#+11
        SMLABB   R8,R0,R4,R5
        MOVS     R2,#+0
        MOV      R1,R0
        ADD      R0,R8,#+41
          CFI FunCall __aeabi_memset
        BL       __aeabi_memset
//  446             name_len = strlen(handle_name);
        MOV      R0,R7
          CFI FunCall strlen
        BL       strlen
        MOV      R9,R0
//  447 
//  448             if (name_len >= SLEEP_HANDLE_NAME_LEN) {
        CMP      R9,#+11
        BCC.N    ??sleep_management_get_lock_handle_3
//  449                 name_len = SLEEP_HANDLE_NAME_LEN - 1;
        MOV      R9,#+10
//  450             }
//  451             memcpy(&sleep_management_handle.user_handle_name[index][0], handle_name, name_len);
??sleep_management_get_lock_handle_3:
        MOV      R2,R9
        MOV      R1,R7
        ADD      R0,R8,#+41
          CFI FunCall __aeabi_memcpy
        BL       __aeabi_memcpy
//  452 
//  453             /* Check handle name */
//  454             if (name_len == 0) {
        CMP      R9,#+0
        BNE.N    ??sleep_management_get_lock_handle_4
//  455                 log_hal_error("[Sleep Management]sleep handle name error\r\n");
        ADR.W    R2,?_12
        MOVW     R1,#+455
        ADR.W    R0,??__FUNCTION___2
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  456                 assert(0);
        MOV      R2,#+456
        ADR.W    R1,?_10
        ADR.N    R0,??DataTable47  ;; "0"
          CFI FunCall __aeabi_assert
        BL       __aeabi_assert
          CFI FunCall __iar_EmptyStepPoint
        BL       __iar_EmptyStepPoint
//  457             }
//  458             for (i = 0; i < name_len; i++) {
??sleep_management_get_lock_handle_4:
        MOVS     R7,#+0
        B.N      ??sleep_management_get_lock_handle_5
//  459                 if ((sleep_management_handle.user_handle_name[index][i] <= 0x20) || (sleep_management_handle.user_handle_name[index][i] >= 0x7E)) {
??sleep_management_get_lock_handle_6:
        ADD      R0,R7,R8
        LDRB     R0,[R0, #+41]
        SUBS     R0,R0,#+33
        CMP      R0,#+93
        BCC.N    ??sleep_management_get_lock_handle_7
//  460                     log_hal_error("[Sleep Management]sleep handle name error\r\n");
        ADR.W    R2,?_12
        MOV      R1,#+460
        ADR.W    R0,??__FUNCTION___2
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  461                     assert(0);
        MOVW     R2,#+461
        ADR.W    R1,?_10
        ADR.N    R0,??DataTable47  ;; "0"
          CFI FunCall __aeabi_assert
        BL       __aeabi_assert
          CFI FunCall __iar_EmptyStepPoint
        BL       __iar_EmptyStepPoint
//  462                 }
//  463             }
??sleep_management_get_lock_handle_7:
        ADDS     R7,R7,#+1
        UXTB     R7,R7
??sleep_management_get_lock_handle_5:
        CMP      R7,R9
        BCC.N    ??sleep_management_get_lock_handle_6
//  464             break;
//  465         }
//  466     }
//  467     restore_interrupt_mask(mask);
??sleep_management_get_lock_handle_2:
        MOV      R0,R6
          CFI FunCall restore_interrupt_mask
        BL       restore_interrupt_mask
//  468     log_hal_error("[Sleep Management]sleep handle name : %s\r\n", &sleep_management_handle.user_handle_name[index][0]);
        ADR.W    R6,??__FUNCTION___2
        MOVS     R0,#+11
        SMLABB   R0,R0,R4,R5
        ADD      R3,R0,#+41
        ADR.W    R2,?_13
        MOV      R1,#+468
        MOV      R0,R6
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  469     if (index >= SLEEP_LOCK_HANDLE_USER_MAX) {
        CMP      R4,#+18
        BLT.N    ??sleep_management_get_lock_handle_8
//  470         log_hal_error("[Sleep Management]cannot get sleep handle\r\n");
        ADR.W    R2,?_14
        MOV      R1,#+470
        MOV      R0,R6
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  471         assert(0);
        MOVW     R2,#+471
        ADR.W    R1,?_10
        ADR.N    R0,??DataTable47  ;; "0"
          CFI FunCall __aeabi_assert
        BL       __aeabi_assert
          CFI FunCall __iar_EmptyStepPoint
        BL       __iar_EmptyStepPoint
//  472         return (SLEEP_LOCK_INVALID_ID);
        MOVS     R0,#+255
        B.N      ??sleep_management_get_lock_handle_9
//  473     }
//  474 
//  475     index += SLEEP_LOCK_USER_START_ID;
//  476 
//  477     return (index);
??sleep_management_get_lock_handle_8:
        ADD      R0,R4,#+14
        UXTB     R0,R0
??sleep_management_get_lock_handle_9:
        POP      {R1,R4-R9,PC}    ;; return
//  478 }
          CFI EndBlock cfiBlock10
//  479 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock11 Using cfiCommon0
          CFI Function sleep_management_release_lock_handle
        THUMB
//  480 void sleep_management_release_lock_handle(uint8_t handle_index)
//  481 {
sleep_management_release_lock_handle:
        PUSH     {R3-R5,LR}
          CFI R14 Frame(CFA, -4)
          CFI R5 Frame(CFA, -8)
          CFI R4 Frame(CFA, -12)
          CFI CFA R13+16
        MOV      R4,R0
//  482     uint32_t mask;
//  483     /*  check handle index range */
//  484     if ((handle_index >= SLEEP_LOCK_HANDLE_MAX) || (handle_index < SLEEP_LOCK_USER_START_ID)) {
        SUB      R0,R4,#+14
        CMP      R0,#+18
        BCC.N    ??sleep_management_release_lock_handle_0
//  485         log_hal_error("[Sleep Management]sleep handle index error\r\n");
        ADR.W    R2,?_15
        MOVW     R1,#+485
        ADR.W    R0,??__FUNCTION___3
        POP      {R3-R5,LR}
          CFI R4 SameValue
          CFI R5 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall log_hal_error_internal
        B.W      log_hal_error_internal
          CFI R4 Frame(CFA, -12)
          CFI R5 Frame(CFA, -8)
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+16
//  486         return;
//  487     }
//  488 
//  489     handle_index -= SLEEP_LOCK_USER_START_ID;
??sleep_management_release_lock_handle_0:
        SUBS     R4,R4,#+14
//  490 
//  491     mask = save_and_set_interrupt_mask();
          CFI FunCall save_and_set_interrupt_mask
        BL       save_and_set_interrupt_mask
        MOV      R5,R0
//  492     if (((sleep_management_handle.user_handle_resoure >> handle_index) & 0x01) == 1) {
        LDR.N    R0,??DataTable53_7
        LDR      R1,[R0, #+4]
        MOV      R2,R1
        LSRS     R2,R2,R4
        LSLS     R2,R2,#+31
        BPL.N    ??sleep_management_release_lock_handle_1
//  493         sleep_management_handle.user_handle_count--;
        LDRB     R2,[R0, #+8]
        SUBS     R2,R2,#+1
        STRB     R2,[R0, #+8]
//  494         sleep_management_handle.user_handle_resoure &= ~(1 << handle_index);
        MOVS     R2,#+1
        LSLS     R2,R2,R4
        BICS     R1,R1,R2
        STR      R1,[R0, #+4]
//  495         memset(&sleep_management_handle.user_handle_name[handle_index][0], 0, SLEEP_HANDLE_NAME_LEN);
        MOVS     R2,#+0
        MOVS     R1,#+11
        UXTB     R4,R4
        MOV      R3,R1
        SMLABB   R0,R3,R4,R0
        ADDS     R0,R0,#+41
          CFI FunCall __aeabi_memset
        BL       __aeabi_memset
//  496     } else {
        B.N      ??sleep_management_release_lock_handle_2
//  497         log_hal_warning("[Sleep Management]sleep handle already release \r\n");
??sleep_management_release_lock_handle_1:
        ADR.W    R2,?_16
        MOVW     R1,#+497
        ADR.W    R0,??__FUNCTION___3
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
//  498     }
//  499     restore_interrupt_mask(mask);
??sleep_management_release_lock_handle_2:
        MOV      R0,R5
        POP      {R1,R4,R5,LR}
          CFI R4 SameValue
          CFI R5 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall restore_interrupt_mask
        B.W      restore_interrupt_mask
//  500 }
          CFI EndBlock cfiBlock11
//  501 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock12 Using cfiCommon0
          CFI Function sleep_management_lock_sleep
        THUMB
//  502 void sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index)
//  503 {
sleep_management_lock_sleep:
        PUSH     {R3-R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI R7 Frame(CFA, -8)
          CFI R6 Frame(CFA, -12)
          CFI R5 Frame(CFA, -16)
          CFI R4 Frame(CFA, -20)
          CFI CFA R13+24
        MOV      R7,R0
        MOV      R5,R1
//  504     uint32_t mask;
//  505 
//  506     if (handle_index >= SLEEP_LOCK_HANDLE_MAX) {
        MOV      R6,R5
        CMP      R6,#+32
        BLT.N    ??sleep_management_lock_sleep_0
//  507         log_hal_error("[Sleep Management]sleep handle index error\r\n");
        ADR.W    R2,?_15
        MOVW     R1,#+507
        ADR.W    R0,??__FUNCTION___4
        POP      {R3-R7,LR}
          CFI R4 SameValue
          CFI R5 SameValue
          CFI R6 SameValue
          CFI R7 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall log_hal_error_internal
        B.W      log_hal_error_internal
          CFI R4 Frame(CFA, -20)
          CFI R5 Frame(CFA, -16)
          CFI R6 Frame(CFA, -12)
          CFI R7 Frame(CFA, -8)
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+24
//  508         return;
//  509     }
//  510 
//  511     mask = save_and_set_interrupt_mask();
??sleep_management_lock_sleep_0:
          CFI FunCall save_and_set_interrupt_mask
        BL       save_and_set_interrupt_mask
        MOV      R4,R0
//  512     if (lock == LOCK_SLEEP) {
        LDR.N    R1,??DataTable53_7
        ADDS     R0,R6,R1
        LDRB     R2,[R0, #+9]
        CMP      R7,#+1
        BNE.N    ??sleep_management_lock_sleep_1
//  513         /* Lock sleep request */
//  514         sleep_management_handle.lock_sleep_request_count[handle_index]++;
        ADDS     R2,R2,#+1
        STRB     R2,[R0, #+9]
//  515         sleep_management_handle.lock_sleep_request |= (1 << handle_index);
        LDR      R2,[R1, #+0]
        MOVS     R3,#+1
        LSLS     R3,R3,R5
        ORRS     R2,R3,R2
        STR      R2,[R1, #+0]
//  516 
//  517 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  518         sleep_management_debug_lock_sleep_timelog(lock, handle_index);
//  519 #endif
//  520         if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0xFF) {
        LDRB     R0,[R0, #+9]
        CMP      R0,#+255
        BNE.N    ??sleep_management_lock_sleep_2
//  521             if (handle_index < SLEEP_LOCK_USER_START_ID) {
        ADR.W    R0,??__FUNCTION___4
        CMP      R6,#+14
        BGE.N    ??sleep_management_lock_sleep_3
//  522                 log_hal_warning("[Sleep Management]sleep handle=%d,lock sleep count full \r\n", handle_index);
        MOV      R3,R6
        ADR.W    R2,?_17
        MOVW     R1,#+522
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
        B.N      ??sleep_management_lock_sleep_2
//  523             } else {
//  524                 log_hal_warning("[Sleep Management]sleep handle=%d,%s,lock sleep count full \r\n", handle_index, (char *)&sleep_management_handle.user_handle_name[handle_index]);
??sleep_management_lock_sleep_3:
        MOVS     R2,#+11
        SMLABB   R1,R2,R6,R1
        ADDS     R1,R1,#+41
        STR      R1,[SP, #+0]
        MOV      R3,R6
        ADR.W    R2,?_18
        MOV      R1,#+524
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
        B.N      ??sleep_management_lock_sleep_2
//  525             }
//  526         }
//  527     } else {
//  528         /* Unlock sleep request */
//  529         if (sleep_management_handle.lock_sleep_request_count[handle_index] > 0) {
??sleep_management_lock_sleep_1:
        CMP      R2,#+0
        BEQ.N    ??sleep_management_lock_sleep_4
//  530             sleep_management_handle.lock_sleep_request_count[handle_index]--;
        SUBS     R2,R2,#+1
        STRB     R2,[R0, #+9]
//  531             if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0) {
        UXTB     R2,R2
        CMP      R2,#+0
        BNE.N    ??sleep_management_lock_sleep_2
//  532                 sleep_management_handle.lock_sleep_request &= ~(1 << handle_index);
        LDR      R0,[R1, #+0]
        MOVS     R2,#+1
        LSLS     R2,R2,R5
        BICS     R0,R0,R2
        STR      R0,[R1, #+0]
        B.N      ??sleep_management_lock_sleep_2
//  533 
//  534 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  535                 sleep_management_debug_lock_sleep_timelog(lock, handle_index);
//  536 #endif
//  537             }
//  538         } else {
//  539             if (handle_index < SLEEP_LOCK_USER_START_ID) {
??sleep_management_lock_sleep_4:
        ADR.W    R0,??__FUNCTION___4
        CMP      R6,#+14
        BGE.N    ??sleep_management_lock_sleep_5
//  540                 log_hal_warning("[Sleep Management]sleep handle=%d,lock sleep has already released \r\n", handle_index);
        MOV      R3,R6
        ADR.W    R2,?_19
        MOV      R1,#+540
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
        B.N      ??sleep_management_lock_sleep_2
//  541             } else {
//  542                 log_hal_warning("[Sleep Management]sleep handle=%d,%s,lock sleep has already released \r\n", handle_index, (char *)&sleep_management_handle.user_handle_name[handle_index]);
??sleep_management_lock_sleep_5:
        MOVS     R2,#+11
        SMLABB   R1,R2,R6,R1
        ADDS     R1,R1,#+41
        STR      R1,[SP, #+0]
        MOV      R3,R6
        ADR.W    R2,?_20
        MOVW     R1,#+542
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
//  543             }
//  544         }
//  545     }
//  546     restore_interrupt_mask(mask);
??sleep_management_lock_sleep_2:
        MOV      R0,R4
        POP      {R1,R4-R7,LR}
          CFI R4 SameValue
          CFI R5 SameValue
          CFI R6 SameValue
          CFI R7 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall restore_interrupt_mask
        B.W      restore_interrupt_mask
//  547 }
          CFI EndBlock cfiBlock12
//  548 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock13 Using cfiCommon0
          CFI Function sleep_management_check_sleep_locks
        THUMB
//  549 bool sleep_management_check_sleep_locks(void)
//  550 {
sleep_management_check_sleep_locks:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  551     uint32_t mask;
//  552     bool lock;
//  553     mask = save_and_set_interrupt_mask();
          CFI FunCall save_and_set_interrupt_mask
        BL       save_and_set_interrupt_mask
//  554 
//  555     if (sleep_management_handle.lock_sleep_request == 0) {
        LDR.N    R1,??DataTable53_7
        LDR      R1,[R1, #+0]
        SUBS     R4,R1,#+1
        SBCS     R4,R4,R4
        MVNS     R4,R4
        LSRS     R4,R4,#+31
//  556         lock = false;
//  557     } else {
//  558         lock = true;
//  559     }
//  560     restore_interrupt_mask(mask);
          CFI FunCall restore_interrupt_mask
        BL       restore_interrupt_mask
//  561     return lock;
        MOV      R0,R4
        POP      {R4,PC}          ;; return
//  562 }
          CFI EndBlock cfiBlock13
//  563 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock14 Using cfiCommon0
          CFI Function sleep_management_check_handle_status
        THUMB
//  564 bool sleep_management_check_handle_status(uint8_t handle_index)
//  565 {
sleep_management_check_handle_status:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  566     /*  check handle index range */
//  567     if ((handle_index >= SLEEP_LOCK_HANDLE_MAX) || (handle_index < SLEEP_LOCK_USER_START_ID)) {
        SUB      R1,R0,#+14
        CMP      R1,#+18
        BCC.N    ??sleep_management_check_handle_status_0
//  568         log_hal_error("[Sleep Management]sleep handle index error\r\n");
        ADR.W    R2,?_15
        MOV      R1,#+568
        ADR.W    R0,??__FUNCTION___5
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  569         return false;
        MOVS     R0,#+0
        POP      {R1,PC}
//  570     }
//  571 
//  572     handle_index -= SLEEP_LOCK_USER_START_ID;
//  573     if (sleep_management_handle.user_handle_resoure & (1 << handle_index)) {
??sleep_management_check_handle_status_0:
        LDR.N    R1,??DataTable53_7
        LDR      R1,[R1, #+4]
        SUBS     R0,R0,#+14
        LSRS     R1,R1,R0
        AND      R0,R1,#0x1
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
//  574         return true;
//  575     } else {
//  576         return false;
        POP      {R1,PC}          ;; return
//  577     }
//  578 }
          CFI EndBlock cfiBlock14
//  579 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock15 Using cfiCommon0
          CFI Function sleep_management_get_lock_sleep_request_info
          CFI NoCalls
        THUMB
//  580 uint32_t sleep_management_get_lock_sleep_request_info(void)
//  581 {
//  582     return sleep_management_handle.lock_sleep_request;
sleep_management_get_lock_sleep_request_info:
        LDR.N    R0,??DataTable53_7
        LDR      R0,[R0, #+0]
        BX       LR               ;; return
//  583 }
          CFI EndBlock cfiBlock15

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable47:
        DC8      "0",0x0,0x0
//  584 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock16 Using cfiCommon0
          CFI Function sleep_management_get_lock_sleep_handle_list
        THUMB
//  585 uint32_t sleep_management_get_lock_sleep_handle_list(void)
//  586 {
sleep_management_get_lock_sleep_handle_list:
        PUSH     {R3-R5,LR}
          CFI R14 Frame(CFA, -4)
          CFI R5 Frame(CFA, -8)
          CFI R4 Frame(CFA, -12)
          CFI CFA R13+16
//  587     uint8_t i;
//  588 
//  589     log_hal_warning("[Sleep Management]lock sleep handle list : \r\n");
        ADR.W    R4,??__FUNCTION___6
        ADR.W    R2,?_21
        MOVW     R1,#+589
        MOV      R0,R4
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
//  590     for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        MOVS     R5,#+0
        B.N      ??sleep_management_get_lock_sleep_handle_list_0
//  591         if (sleep_management_handle.lock_sleep_request & (1 << i)) {
//  592             if (i < SLEEP_LOCK_USER_START_ID) {
//  593                 log_hal_warning("sleep handle=%d\r\n", i);
//  594             } else {
//  595                 log_hal_warning("sleep handle=%d,%s\r\n", i, (char *)&sleep_management_handle.user_handle_name[i - SLEEP_LOCK_USER_START_ID][0]);
??sleep_management_get_lock_sleep_handle_list_1:
        MOVS     R0,#+11
        SMLABB   R0,R0,R5,R1
        SUBS     R0,R0,#+113
        STR      R0,[SP, #+0]
        MOV      R3,R5
        ADR.W    R2,?_23
        MOVW     R1,#+595
        MOV      R0,R4
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
//  596             }
??sleep_management_get_lock_sleep_handle_list_2:
        ADDS     R5,R5,#+1
??sleep_management_get_lock_sleep_handle_list_0:
        LDR.N    R1,??DataTable53_7
        LDR      R0,[R1, #+0]
        CMP      R5,#+32
        BGE.N    ??sleep_management_get_lock_sleep_handle_list_3
        LSRS     R0,R0,R5
        LSLS     R0,R0,#+31
        BPL.N    ??sleep_management_get_lock_sleep_handle_list_2
        CMP      R5,#+14
        BGE.N    ??sleep_management_get_lock_sleep_handle_list_1
        MOV      R3,R5
        ADR.W    R2,?_22
        MOVW     R1,#+593
        MOV      R0,R4
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
        B.N      ??sleep_management_get_lock_sleep_handle_list_2
//  597         }
//  598     }
//  599     return sleep_management_handle.lock_sleep_request;
??sleep_management_get_lock_sleep_handle_list_3:
        POP      {R1,R4,R5,PC}    ;; return
//  600 }
          CFI EndBlock cfiBlock16
//  601 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock17 Using cfiCommon0
          CFI Function sleep_management_low_power_init_setting
          CFI NoCalls
        THUMB
//  602 void sleep_management_low_power_init_setting(void)
//  603 {
//  604     //uint32_t hw_coded;
//  605     uint32_t system_info;
//  606 
//  607     /* cm4_dcm_en */
//  608     *((volatile uint32_t *)(0xe00fe000)) = 0x3;
sleep_management_low_power_init_setting:
        LDR.N    R0,??DataTable53_8  ;; 0xe00fe000
        MOVS     R1,#+3
        STR      R1,[R0, #+0]
//  609     *((volatile uint32_t *)(0xe00fe004)) = 0x3;
        STR      R1,[R0, #+4]
//  610     *((volatile uint32_t *)(0xe0100040)) = 0x127;
        MOVW     R0,#+295
        LDR.N    R1,??DataTable53_9  ;; 0xe0100040
        STR      R0,[R1, #+0]
//  611 
//  612     //hw_coded    = *((volatile uint32_t *)(0xA2000008)); /* HW_CODED */
//  613     system_info = *((volatile uint32_t *)(0xA2010040)); /* SYSTEM_INFOD */
        LDR.N    R0,??DataTable53_10  ;; 0xa2010040
        LDR      R0,[R0, #+0]
//  614 
//  615     /* system_info */
//  616     if (((system_info >> 8) & 0x1)) { //
        UBFX     R1,R0,#+8,#+1
        CMP      R1,#+0
        BEQ.N    ??sleep_management_low_power_init_setting_0
//  617         /* bond_psram_sip_b == 0 */
//  618         *((volatile uint32_t *)(0xA20D0058)) = 0x10; /* IO_CFG_1_PU_CFG0_CLR */
        LDR.N    R1,??DataTable53_11  ;; 0xa20d0034
        MOVS     R2,#+16
        STR      R2,[R1, #+36]
//  619         *((volatile uint32_t *)(0xA20D0034)) = 0x10; /* IO_CFG_1_PD_CFG0_SET */
        STR      R2,[R1, #+0]
        B.N      ??sleep_management_low_power_init_setting_1
//  620     } else {
//  621         /*TINFO = " -------------------- bond_psram_sip_b == 1-------------------- "*/
//  622         //EMI CG
//  623         *((volatile uint32_t *)(0xA21D0103)) = 0x4; //0xA21D0000+0x103 /* BUS_DCM_CON_0__F_RG_BUS_DCM_EN */
??sleep_management_low_power_init_setting_0:
        MOVS     R1,#+4
        LDR.N    R2,??DataTable53_12  ;; 0xa21d0103
        STR      R1,[R2, #+0]
//  624         //EMI IO
//  625     }
//  626     if (((system_info >> 9) & 0x1)) {
??sleep_management_low_power_init_setting_1:
        UBFX     R1,R0,#+9,#+1
        CMP      R1,#+0
        BEQ.N    ??sleep_management_low_power_init_setting_2
//  627         /*TINFO = " -------------------- bond_sf_sip_b == 0 -------------------- "*/
//  628         *((volatile uint32_t *)(0xA20D0058)) = 0x20; /* IO_CFG_1_PU_CFG0_CLR */
        LDR.N    R1,??DataTable53_11  ;; 0xa20d0034
        MOVS     R2,#+32
        STR      R2,[R1, #+36]
//  629         *((volatile uint32_t *)(0xA20D0034)) = 0x20; /* IO_CFG_1_PD_CFG0_SET */
        STR      R2,[R1, #+0]
//  630     }
//  631 
//  632     if (((system_info >> 10) & 0x1)) {
??sleep_management_low_power_init_setting_2:
        UBFX     R0,R0,#+10,#+1
        CMP      R0,#+0
        BEQ.N    ??sleep_management_low_power_init_setting_3
//  633         /*TINFO = " -------------------- bond_rsv_b == 0 -------------------- "*/
//  634         *((volatile uint32_t *)(0xA20D0058)) = 0x40; /* IO_CFG_1_PU_CFG0_CLR */
        LDR.N    R1,??DataTable53_11  ;; 0xa20d0034
        MOVS     R0,#+64
        STR      R0,[R1, #+36]
//  635         *((volatile uint32_t *)(0xA20D0034)) = 0x40; /* IO_CFG_1_PD_CFG0_SET */
        STR      R0,[R1, #+0]
//  636     }
//  637 
//  638     *((volatile uint32_t *)(0xA20C00A4)) = 0xF; /* IO sleep tdsel : IO_CFG_0_TDSEL_CFG_SET */
??sleep_management_low_power_init_setting_3:
        MOVS     R0,#+15
        LDR.N    R1,??DataTable53_13  ;; 0xa20c00a4
        STR      R0,[R1, #+0]
//  639 }
        BX       LR               ;; return
          CFI EndBlock cfiBlock17
//  640 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock18 Using cfiCommon0
          CFI Function hal_lp_connsys_get_own_enable_int
        THUMB
//  641 int8_t hal_lp_connsys_get_own_enable_int(void)
//  642 {
//  643     if (ptr_lp_connsys_get_own_enable_int != NULL) {
hal_lp_connsys_get_own_enable_int:
        LDR.N    R0,??DataTable53_14
        LDR      R0,[R0, #+0]
        MOVS     R1,R0
        BEQ.N    ??hal_lp_connsys_get_own_enable_int_0
//  644         return (ptr_lp_connsys_get_own_enable_int());
          CFI FunCall
        ANOTE "tailcall"
        BX       R0
//  645     }
//  646     return 0;
??hal_lp_connsys_get_own_enable_int_0:
        MOVS     R0,#+0
        BX       LR               ;; return
//  647 }
          CFI EndBlock cfiBlock18
//  648 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock19 Using cfiCommon0
          CFI Function hal_lp_connsys_give_n9_own
        THUMB
//  649 int8_t hal_lp_connsys_give_n9_own(void)
//  650 {
//  651     if (ptr_lp_connsys_give_n9_own != NULL) {
hal_lp_connsys_give_n9_own:
        LDR.N    R0,??DataTable53_15
        LDR      R0,[R0, #+0]
        MOVS     R1,R0
        BEQ.N    ??hal_lp_connsys_give_n9_own_0
//  652         return (ptr_lp_connsys_give_n9_own());
          CFI FunCall
        ANOTE "tailcall"
        BX       R0
//  653     }
//  654     return 0;
??hal_lp_connsys_give_n9_own_0:
        MOVS     R0,#+0
        BX       LR               ;; return
//  655 }
          CFI EndBlock cfiBlock19
//  656 
//  657 #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//  658 #include "hal_gpt.h"
//  659 uint32_t sleep_handle_total_lock_sleep_time[SLEEP_LOCK_HANDLE_MAX];
//  660 uint32_t sleep_handle_total_lock_sleep_count[SLEEP_LOCK_HANDLE_MAX];
//  661 uint32_t sleep_backup_fun_time[SLEEP_BACKUP_RESTORE_MODULE_MAX];
//  662 uint32_t sleep_restore_fun_time[SLEEP_BACKUP_RESTORE_MODULE_MAX];
//  663 
//  664 void sleep_management_debug_lock_sleep_timelog(sleep_management_lock_sleep_t lock, uint8_t handle_index)
//  665 {
//  666     static uint32_t lock_sleep_time[SLEEP_LOCK_HANDLE_MAX], unlock_sleep_time;
//  667 
//  668     if (lock == LOCK_SLEEP) {
//  669         if (sleep_management_handle.lock_sleep_request_count[handle_index] == 1) {
//  670             hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &lock_sleep_time[handle_index]);
//  671         }
//  672         sleep_handle_total_lock_sleep_count[handle_index]++;
//  673     } else {
//  674         hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &unlock_sleep_time);
//  675 
//  676         if (unlock_sleep_time >= lock_sleep_time[handle_index]) {
//  677             sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time - lock_sleep_time[handle_index];
//  678         } else {
//  679             sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time + (0xFFFFFFFF - lock_sleep_time[handle_index]);
//  680         }
//  681     }
//  682 }
//  683 
//  684 void sleep_management_debug_backup_restore_fun_timelog(uint32_t type, uint32_t order, uint32_t callback)
//  685 {
//  686     static  uint32_t enter, exit;
//  687 
//  688     if (order == 0) {
//  689         hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &enter);
//  690     } else {
//  691         hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &exit);
//  692         if (exit >= enter) {
//  693             exit = exit - enter;
//  694         } else {
//  695             exit = exit + (0xFFFFFFFF - enter);
//  696         }
//  697         if (type == 0) {
//  698             sleep_backup_fun_time[callback] = exit;
//  699         } else {
//  700             sleep_restore_fun_time[callback] = exit;
//  701         }
//  702     }
//  703 
//  704 }
//  705 
//  706 void sleep_management_debug_dump_lock_sleep_time()
//  707 {
//  708     uint32_t i;
//  709 
//  710     printf("dump lock sleep time : \r\n");
//  711     for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
//  712         if (sleep_handle_total_lock_sleep_count[i] > 0) {
//  713             printf("handle[%d] count : %d", (int)i, (int)sleep_handle_total_lock_sleep_count[i]);
//  714             printf("handle[%d] total lock time : %d", (int)i, (int)sleep_handle_total_lock_sleep_time[i]);
//  715         }
//  716     }
//  717 }
//  718 
//  719 void sleep_management_debug_dump_backup_restore_time()
//  720 {
//  721     uint32_t i;
//  722 
//  723     printf("dump backup restore function execute time : \r\n");
//  724     for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
//  725         printf("backup fun[%d]  : %d", (int)i, (int)sleep_backup_fun_time[i]);
//  726         printf("restore fun[%d] : %d", (int)i, (int)sleep_restore_fun_time[i]);
//  727     }
//  728 }
//  729 
//  730 void sleep_management_dump_wakeup_source(uint32_t wakeup_source,uint32_t eint_status)
//  731 {
//  732     printf("===============Wakeup from Deep Sleep===============\r\n");
//  733     printf("[Sleep Management]WAKEUP_SOURCE = 0x%x\r\n", (int)wakeup_source);
//  734     if (wakeup_source == 0x3FE) {
//  735         printf("[Sleep Management]WAKEUP_SOURCE : GPT\r\n");
//  736     }
//  737     if (wakeup_source == 0x3FD) {        
//  738         sleep_management_dump_eint_wakeup_source(eint_status);
//  739     }
//  740     if (wakeup_source == 0x3FB) {
//  741         printf("[Sleep Management]WAKEUP_SOURCE : SDIO_SLV\r\n");
//  742     }
//  743     if (wakeup_source == 0x3F7) {
//  744         printf("[Sleep Management]WAKEUP_SOURCE : SPI_SLAVE_A\r\n");
//  745     }
//  746     if (wakeup_source == 0x3EF) {
//  747         printf("[Sleep Management]WAKEUP_SOURCE : SPI_SLAVE_B\r\n");
//  748     }
//  749     if (wakeup_source == 0x3DF) {
//  750         printf("[Sleep Management]WAKEUP_SOURCE : WDT\r\n");
//  751     }
//  752     if (wakeup_source == 0x3BF) {
//  753         printf("[Sleep Management]WAKEUP_SOURCE : MSDC\r\n");
//  754     }
//  755     if (wakeup_source == 0x37F) {
//  756         printf("[Sleep Management]WAKEUP_SOURCE : DEBUGSYS\r\n");
//  757     }
//  758 }
//  759 
//  760 
//  761 void sleep_management_dump_eint_wakeup_source(uint32_t eint_num)
//  762 {
//  763     uint32_t i;
//  764 
//  765     printf("[Sleep Management]WAKEUP_SOURCE : EINT = 0x%x\r\n", (unsigned int)eint_num);
//  766 
//  767     for (i = 0; i < HAL_EINT_NUMBER_MAX; i++) {
//  768         if (((eint_num >> i) & 0x01) == 0x01) {
//  769             if (i <= 20) {
//  770                 printf("[Sleep Management]EINT%d\r\n", (unsigned int)i);
//  771             }
//  772             switch (i) {
//  773                 case 21 :
//  774                     printf("[Sleep Management]EINT_UART_0_RX\r\n");
//  775                     break;
//  776 
//  777                 case 22 :
//  778                     printf("[Sleep Management]EINT_UART_1_RX\r\n");
//  779                     break;
//  780 
//  781                 case 23 :
//  782                     printf("[Sleep Management]EINT_UART_2_RX\r\n");
//  783                     break;
//  784 
//  785                 case 24 :
//  786                     printf("[Sleep Management]EINT_RTC\r\n");
//  787                     break;
//  788 
//  789                 case 25 :
//  790                     printf("[Sleep Management]EINT_HIF\r\n");
//  791                     break;
//  792 
//  793                 case 26 :
//  794                     printf("[Sleep Management]EINT_PSE\r\n");
//  795                     break;
//  796 
//  797                 case 27 :
//  798                     printf("[Sleep Management]EINT_WDT\r\n");
//  799                     break;
//  800 
//  801                 case 28 :
//  802                     printf("[Sleep Management]EINT_MAC\r\n");
//  803                     break;
//  804 
//  805                 case 29 :
//  806                     printf("[Sleep Management]EINT_PMIC\r\n");
//  807                     break;
//  808             }
//  809         }
//  810     }
//  811 }
//  812 #endif
//  813 

        SECTION `.text`:CODE:NOROOT(2)
          CFI Block cfiBlock20 Using cfiCommon0
          CFI Function _cli_deep_sleep
        THUMB
//  814 static uint8_t _cli_deep_sleep(uint8_t len, char *param[])
//  815 {
_cli_deep_sleep:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  816     uint32_t mask_wakeup_source;
//  817 
//  818     mask_wakeup_source = atoi(param[0]);
        LDR      R0,[R1, #+0]
          CFI FunCall atoi
        BL       atoi
        MOV      R4,R0
//  819 
//  820     printf("_cli_enter_deep_sleep\r\n");
        ADR.W    R0,?_24
          CFI FunCall printf
        BL       printf
//  821 
//  822     hal_gpt_delay_ms(100);
        MOVS     R0,#+100
          CFI FunCall hal_gpt_delay_ms
        BL       hal_gpt_delay_ms
//  823 
//  824     if (mask_wakeup_source != 0) {
        CMP      R4,#+0
        BEQ.N    ??_cli_deep_sleep_0
//  825         *SPM_WAKEUP_SOURCE_MASK = 0xFFFF;
        MOVW     R0,#+65535
        LDR.N    R1,??DataTable53_16  ;; 0xa20f0000
        STR      R0,[R1, #+0]
//  826     }
//  827 
//  828     __asm volatile("cpsid i");
??_cli_deep_sleep_0:
        cpsid i
//  829     hal_sleep_manager_set_sleep_time(3000);
        MOVW     R0,#+3000
          CFI FunCall hal_sleep_manager_set_sleep_time
        BL       hal_sleep_manager_set_sleep_time
//  830     hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
        MOVS     R0,#+2
          CFI FunCall hal_sleep_manager_enter_sleep_mode
        BL       hal_sleep_manager_enter_sleep_mode
//  831     __asm volatile("cpsie i");
        cpsie i
//  832 
//  833     printf("_cli_leave_deep_sleep\r\n");
        ADR.W    R0,?_25
          CFI FunCall printf
        BL       printf
//  834     hal_gpt_delay_ms(100);
        MOVS     R0,#+100
          CFI FunCall hal_gpt_delay_ms
        BL       hal_gpt_delay_ms
//  835 
//  836     return 0;
        MOVS     R0,#+0
        POP      {R4,PC}          ;; return
//  837 }
          CFI EndBlock cfiBlock20
//  838 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock21 Using cfiCommon0
          CFI Function _cli_sleep_status
        THUMB
//  839 static uint8_t _cli_sleep_status(uint8_t len, char *param[])
//  840 {
_cli_sleep_status:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  841     printf("\r\ntickless:%s\r\n", hal_sleep_manager_is_sleep_locked() ? "locked" : "not locked");
          CFI FunCall hal_sleep_manager_is_sleep_locked
        BL       hal_sleep_manager_is_sleep_locked
        CMP      R0,#+0
        BEQ.N    ??_cli_sleep_status_0
        ADR.W    R1,?_27
        B.N      ??_cli_sleep_status_1
??_cli_sleep_status_0:
        ADR.W    R1,?_28
??_cli_sleep_status_1:
        ADR.W    R0,?_26
          CFI FunCall printf
        BL       printf
//  842 
//  843     if (hal_sleep_manager_is_sleep_locked()) {
          CFI FunCall hal_sleep_manager_is_sleep_locked
        BL       hal_sleep_manager_is_sleep_locked
        CMP      R0,#+0
        BEQ.N    ??_cli_sleep_status_2
//  844         sleep_management_get_lock_sleep_handle_list();
          CFI FunCall sleep_management_get_lock_sleep_handle_list
        BL       sleep_management_get_lock_sleep_handle_list
//  845     }
//  846 
//  847     return 0;
??_cli_sleep_status_2:
        MOVS     R0,#+0
        POP      {R1,PC}          ;; return
//  848 }
          CFI EndBlock cfiBlock21
//  849 

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
//  850 void (*ptr_cli_dtim_enter_sleep)(uint32_t mode) = NULL;
ptr_cli_dtim_enter_sleep:
        DS8 4
//  851 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock22 Using cfiCommon0
          CFI Function _cli_dtim_sleep
        THUMB
//  852 static uint8_t _cli_dtim_sleep(uint8_t len, char *param[])
//  853 {
_cli_dtim_sleep:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  854     uint32_t mode = atoi(param[0]);
        LDR      R0,[R1, #+0]
          CFI FunCall atoi
        BL       atoi
//  855 
//  856     if (ptr_cli_dtim_enter_sleep != NULL) {
        LDR.N    R1,??DataTable53_17
        LDR      R1,[R1, #+0]
        MOVS     R2,R1
        BEQ.N    ??_cli_dtim_sleep_0
//  857         ptr_cli_dtim_enter_sleep(mode);
          CFI FunCall
        BLX      R1
//  858     }
//  859 
//  860     return 0;
??_cli_dtim_sleep_0:
        MOVS     R0,#+0
        POP      {R1,PC}          ;; return
//  861 }
          CFI EndBlock cfiBlock22

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53:
        DC32     0xe000e100

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_1:
        DC32     0xe000e180

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_2:
        DC32     0xe000e280

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_3:
        DC32     suspend_user_callback_func_table

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_4:
        DC32     suspend_callback_func_table

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_5:
        DC32     resume_user_callback_func_table

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_6:
        DC32     resume_callback_func_table

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_7:
        DC32     sleep_management_handle

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_8:
        DC32     0xe00fe000

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_9:
        DC32     0xe0100040

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_10:
        DC32     0xa2010040

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_11:
        DC32     0xa20d0034

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_12:
        DC32     0xa21d0103

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_13:
        DC32     0xa20c00a4

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_14:
        DC32     ptr_lp_connsys_get_own_enable_int

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_15:
        DC32     ptr_lp_connsys_give_n9_own

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_16:
        DC32     0xa20f0000

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable53_17:
        DC32     ptr_cli_dtim_enter_sleep
//  862 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock23 Using cfiCommon0
          CFI Function _cli_enter_retention
        THUMB
//  863 static uint8_t _cli_enter_retention(uint8_t len, char *param[])
//  864 {
_cli_enter_retention:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  865     printf("_cli_enter retention\r\n");
        ADR.W    R0,?_29
          CFI FunCall printf
        BL       printf
//  866     hal_gpt_delay_ms(100);
        MOVS     R0,#+100
          CFI FunCall hal_gpt_delay_ms
        BL       hal_gpt_delay_ms
//  867 
//  868     sleep_management_enter_retention();
          CFI FunCall sleep_management_enter_retention
        BL       sleep_management_enter_retention
//  869 
//  870     return 0;
        MOVS     R0,#+0
        POP      {R1,PC}          ;; return
//  871 }
          CFI EndBlock cfiBlock23

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_register_suspend_callback::__FUNCTION__[43]
??__FUNCTION__:
        DC8 "sleep_management_register_suspend_callback"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_8:
        DC8 5BH, 53H, 6CH, 65H, 65H, 70H, 20H, 4DH
        DC8 61H, 6EH, 61H, 67H, 65H, 6DH, 65H, 6EH
        DC8 74H, 5DH, 72H, 65H, 67H, 69H, 73H, 74H
        DC8 65H, 72H, 20H, 73H, 75H, 73H, 70H, 65H
        DC8 6EH, 64H, 20H, 63H, 61H, 6CH, 6CH, 62H
        DC8 61H, 63H, 6BH, 20H, 66H, 75H, 6EH, 63H
        DC8 74H, 69H, 6FH, 6EH, 20H, 6FH, 76H, 65H
        DC8 72H, 66H, 6CH, 6FH, 77H, 0DH, 0AH, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_10:
        DC8 57H, 3AH, 5CH, 44H, 6FH, 63H, 75H, 6DH
        DC8 65H, 6EH, 74H, 73H, 5CH, 32H, 35H, 32H
        DC8 33H, 5FH, 64H, 65H, 76H, 5CH, 67H, 76H
        DC8 61H, 5CH, 64H, 72H, 69H, 76H, 65H, 72H
        DC8 5CH, 63H, 68H, 69H, 70H, 5CH, 6DH, 74H
        DC8 37H, 36H, 38H, 36H, 5CH, 73H, 72H, 63H
        DC8 5FH, 63H, 6FH, 72H, 65H, 5CH, 68H, 61H
        DC8 6CH, 5FH, 73H, 6CH, 65H, 65H, 70H, 5FH
        DC8 6DH, 61H, 6EH, 61H, 67H, 65H, 72H, 5FH
        DC8 69H, 6EH, 74H, 65H, 72H, 6EH, 61H, 6CH
        DC8 2EH, 63H, 0
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_register_resume_callback::__FUNCTION__[42]
??__FUNCTION___1:
        DC8 "sleep_management_register_resume_callback"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_11:
        DC8 5BH, 53H, 6CH, 65H, 65H, 70H, 20H, 4DH
        DC8 61H, 6EH, 61H, 67H, 65H, 6DH, 65H, 6EH
        DC8 74H, 5DH, 72H, 65H, 67H, 69H, 73H, 74H
        DC8 65H, 72H, 20H, 72H, 65H, 73H, 75H, 6DH
        DC8 65H, 20H, 63H, 61H, 6CH, 6CH, 62H, 61H
        DC8 63H, 6BH, 20H, 66H, 75H, 6EH, 63H, 74H
        DC8 69H, 6FH, 6EH, 20H, 6FH, 76H, 65H, 72H
        DC8 66H, 6CH, 6FH, 77H, 0DH, 0AH, 0
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_get_lock_handle::__FUNCTION__[33]
??__FUNCTION___2:
        DC8 "sleep_management_get_lock_handle"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_12:
        DC8 "[Sleep Management]sleep handle name error\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_13:
        DC8 "[Sleep Management]sleep handle name : %s\015\012"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_14:
        DC8 "[Sleep Management]cannot get sleep handle\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_release_lock_handle::__FUNCTION__[37]
??__FUNCTION___3:
        DC8 "sleep_management_release_lock_handle"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_15:
        DC8 "[Sleep Management]sleep handle index error\015\012"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_16:
        DC8 "[Sleep Management]sleep handle already release \015\012"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_lock_sleep::__FUNCTION__[28]
??__FUNCTION___4:
        DC8 "sleep_management_lock_sleep"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_17:
        DC8 5BH, 53H, 6CH, 65H, 65H, 70H, 20H, 4DH
        DC8 61H, 6EH, 61H, 67H, 65H, 6DH, 65H, 6EH
        DC8 74H, 5DH, 73H, 6CH, 65H, 65H, 70H, 20H
        DC8 68H, 61H, 6EH, 64H, 6CH, 65H, 3DH, 25H
        DC8 64H, 2CH, 6CH, 6FH, 63H, 6BH, 20H, 73H
        DC8 6CH, 65H, 65H, 70H, 20H, 63H, 6FH, 75H
        DC8 6EH, 74H, 20H, 66H, 75H, 6CH, 6CH, 20H
        DC8 0DH, 0AH, 0
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_18:
        DC8 5BH, 53H, 6CH, 65H, 65H, 70H, 20H, 4DH
        DC8 61H, 6EH, 61H, 67H, 65H, 6DH, 65H, 6EH
        DC8 74H, 5DH, 73H, 6CH, 65H, 65H, 70H, 20H
        DC8 68H, 61H, 6EH, 64H, 6CH, 65H, 3DH, 25H
        DC8 64H, 2CH, 25H, 73H, 2CH, 6CH, 6FH, 63H
        DC8 6BH, 20H, 73H, 6CH, 65H, 65H, 70H, 20H
        DC8 63H, 6FH, 75H, 6EH, 74H, 20H, 66H, 75H
        DC8 6CH, 6CH, 20H, 0DH, 0AH, 0
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_19:
        DC8 5BH, 53H, 6CH, 65H, 65H, 70H, 20H, 4DH
        DC8 61H, 6EH, 61H, 67H, 65H, 6DH, 65H, 6EH
        DC8 74H, 5DH, 73H, 6CH, 65H, 65H, 70H, 20H
        DC8 68H, 61H, 6EH, 64H, 6CH, 65H, 3DH, 25H
        DC8 64H, 2CH, 6CH, 6FH, 63H, 6BH, 20H, 73H
        DC8 6CH, 65H, 65H, 70H, 20H, 68H, 61H, 73H
        DC8 20H, 61H, 6CH, 72H, 65H, 61H, 64H, 79H
        DC8 20H, 72H, 65H, 6CH, 65H, 61H, 73H, 65H
        DC8 64H, 20H, 0DH, 0AH, 0
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_20:
        DC8 5BH, 53H, 6CH, 65H, 65H, 70H, 20H, 4DH
        DC8 61H, 6EH, 61H, 67H, 65H, 6DH, 65H, 6EH
        DC8 74H, 5DH, 73H, 6CH, 65H, 65H, 70H, 20H
        DC8 68H, 61H, 6EH, 64H, 6CH, 65H, 3DH, 25H
        DC8 64H, 2CH, 25H, 73H, 2CH, 6CH, 6FH, 63H
        DC8 6BH, 20H, 73H, 6CH, 65H, 65H, 70H, 20H
        DC8 68H, 61H, 73H, 20H, 61H, 6CH, 72H, 65H
        DC8 61H, 64H, 79H, 20H, 72H, 65H, 6CH, 65H
        DC8 61H, 73H, 65H, 64H, 20H, 0DH, 0AH, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_check_handle_status::__FUNCTION__[37]
??__FUNCTION___5:
        DC8 "sleep_management_check_handle_status"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const sleep_management_get_lock_sleep_handle_list::__FUNCTION__[44]
??__FUNCTION___6:
        DC8 "sleep_management_get_lock_sleep_handle_list"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_21:
        DC8 "[Sleep Management]lock sleep handle list : \015\012"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_22:
        DC8 "sleep handle=%d\015\012"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_23:
        DC8 "sleep handle=%d,%s\015\012"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_24:
        DC8 "_cli_enter_deep_sleep\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_25:
        DC8 "_cli_leave_deep_sleep\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_26:
        DC8 "\015\012tickless:%s\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_27:
        DC8 "locked"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_28:
        DC8 "not locked"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_29:
        DC8 "_cli_enter retention\015\012"
        DC8 0
//  872 

        SECTION `.data`:DATA:REORDER:NOROOT(2)
        DATA
//  873 cmd_t lp_cli[] = {
lp_cli:
        DC32 ?_0, ?_1, _cli_deep_sleep
        DC8 0, 0, 0, 0
        DC32 ?_2, ?_3, _cli_sleep_status
        DC8 0, 0, 0, 0
        DC32 ?_4, ?_5, _cli_dtim_sleep
        DC8 0, 0, 0, 0
        DC32 ?_6, ?_7, _cli_enter_retention
        DC8 0, 0, 0, 0
        DC32 0H
        DC8 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

        SECTION `.iar_vfe_header`:DATA:NOALLOC:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
        DC32 0

        SECTION __DLIB_PERTHREAD:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        SECTION __DLIB_PERTHREAD_init:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_0:
        DC8 "ds"
        DC8 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_1:
        DC8 "deep sleep"
        DC8 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_2:
        DC8 "status"
        DC8 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_3:
        DC8 "sleep status"
        DC8 0, 0, 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_4:
        DC8 "dtim"
        DC8 0, 0, 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_5:
        DC8 "DTIM sleep"
        DC8 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_6:
        DC8 "retention"
        DC8 0, 0

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
?_7:
        DC8 "enter retention"

        SECTION `.rodata`:CONST:NOROOT(1)
        DATA
?_9:
        DC8 "0"

        END
//  874     { "ds",         "deep sleep",       _cli_deep_sleep         },
//  875     { "status",     "sleep status",     _cli_sleep_status       },
//  876     { "dtim",       "DTIM sleep",       _cli_dtim_sleep         },
//  877     { "retention",  "enter retention",  _cli_enter_retention    },
//  878     { NULL }
//  879 };
//  880 
//  881 #endif
// 
//   756 bytes in section .bss
//    80 bytes in section .data
//    90 bytes in section .rodata
//   972 bytes in section .tcm_code
//   256 bytes in section .tcm_rwdata
// 2 562 bytes in section .text
// 
// 3 534 bytes of CODE  memory
//    90 bytes of CONST memory
// 1 092 bytes of DATA  memory
//
//Errors: none
//Warnings: 2
