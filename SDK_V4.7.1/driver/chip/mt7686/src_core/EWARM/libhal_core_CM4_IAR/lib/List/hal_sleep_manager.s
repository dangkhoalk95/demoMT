///////////////////////////////////////////////////////////////////////////////
//
// IAR ANSI C/C++ Compiler V7.50.1.10123/W32 for ARM      07/Apr/2017  14:04:54
// Copyright 1999-2015 IAR Systems AB.
//
//    Cpu mode     =  thumb
//    Endian       =  little
//    Source file  =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_sleep_manager.c
//    Command line =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_sleep_manager.c
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
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\List\hal_sleep_manager.s
//
///////////////////////////////////////////////////////////////////////////////

        RTMODEL "__SystemLibrary", "DLib"
        AAPCS BASE,INTERWORK,VFP
        PRESERVE8
        REQUIRE8

        #define SHT_PROGBITS 0x1

        EXTERN hal_gpt_init
        EXTERN hal_gpt_register_callback
        EXTERN hal_gpt_start_timer_ms
        EXTERN hal_gpt_stop_timer
        EXTERN log_hal_info_internal
        EXTERN sleep_management_check_handle_status
        EXTERN sleep_management_check_sleep_locks
        EXTERN sleep_management_enter_deep_sleep
        EXTERN sleep_management_enter_retention
        EXTERN sleep_management_get_lock_handle
        EXTERN sleep_management_get_lock_sleep_handle_list
        EXTERN sleep_management_get_lock_sleep_request_info
        EXTERN sleep_management_lock_sleep
        EXTERN sleep_management_low_power_init_setting
        EXTERN sleep_management_release_lock_handle
        EXTERN spm_control_mtcmos
        EXTERN spm_init

        PUBLIC Deep_Sleep_GPT_CB
        PUBLIC cli_sleep_handle
        PUBLIC hal_sleep_manager_enter_sleep_mode
        PUBLIC hal_sleep_manager_get_lock_status
        PUBLIC hal_sleep_manager_init
        PUBLIC hal_sleep_manager_is_sleep_handle_alive
        PUBLIC hal_sleep_manager_is_sleep_locked
        PUBLIC hal_sleep_manager_lock_sleep
        PUBLIC hal_sleep_manager_release_sleep_handle
        PUBLIC hal_sleep_manager_set_sleep_handle
        PUBLIC hal_sleep_manager_set_sleep_time
        PUBLIC hal_sleep_manager_sleep_driver_dump_handle_name
        PUBLIC hal_sleep_manager_unlock_sleep
        
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
        
// W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_sleep_manager.c
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
//   36 #ifdef HAL_SLEEP_MANAGER_ENABLED
//   37 
//   38 #include "hal_sleep_manager_internal.h"
//   39 #include "hal_sleep_manager_retention.h"
//   40 #include "hal_sleep_manager_platform.h"
//   41 #include "hal_spm.h"
//   42 #include "hal_log.h"
//   43 #include "hal_gpt.h"
//   44 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock0 Using cfiCommon0
          CFI Function Deep_Sleep_GPT_CB
        THUMB
//   45 void Deep_Sleep_GPT_CB()
//   46 {
//   47     hal_gpt_stop_timer(DEEP_SLEEP_GPT);
Deep_Sleep_GPT_CB:
        MOVS     R0,#+0
          CFI FunCall hal_gpt_stop_timer
        B.W      hal_gpt_stop_timer
//   48 }
          CFI EndBlock cfiBlock0
//   49 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock1 Using cfiCommon0
          CFI Function hal_sleep_manager_set_sleep_handle
          CFI FunCall sleep_management_get_lock_handle
        THUMB
//   50 uint8_t hal_sleep_manager_set_sleep_handle(const char *handle_name)
//   51 {
//   52     uint8_t index;
//   53     index = sleep_management_get_lock_handle(handle_name);
//   54     return index;
hal_sleep_manager_set_sleep_handle:
        B.W      sleep_management_get_lock_handle
//   55 }
          CFI EndBlock cfiBlock1
//   56 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock2 Using cfiCommon0
          CFI Function hal_sleep_manager_lock_sleep
        THUMB
//   57 hal_sleep_manager_status_t hal_sleep_manager_lock_sleep(uint8_t handle_index)
//   58 {
hal_sleep_manager_lock_sleep:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//   59     sleep_management_lock_sleep(LOCK_SLEEP, handle_index);
        MOV      R1,R0
        MOVS     R0,#+1
          CFI FunCall sleep_management_lock_sleep
        BL       sleep_management_lock_sleep
//   60     return HAL_SLEEP_MANAGER_OK;
        MOVS     R0,#+0
        POP      {R1,PC}          ;; return
//   61 }
          CFI EndBlock cfiBlock2
//   62 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock3 Using cfiCommon0
          CFI Function hal_sleep_manager_unlock_sleep
        THUMB
//   63 hal_sleep_manager_status_t hal_sleep_manager_unlock_sleep(uint8_t handle_index)
//   64 {
hal_sleep_manager_unlock_sleep:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//   65     sleep_management_lock_sleep(UNLOCK_SLEEP, handle_index);
        MOV      R1,R0
        MOVS     R0,#+0
          CFI FunCall sleep_management_lock_sleep
        BL       sleep_management_lock_sleep
//   66     return HAL_SLEEP_MANAGER_OK;
        MOVS     R0,#+0
        POP      {R1,PC}          ;; return
//   67 }
          CFI EndBlock cfiBlock3
//   68 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock4 Using cfiCommon0
          CFI Function hal_sleep_manager_release_sleep_handle
        THUMB
//   69 hal_sleep_manager_status_t hal_sleep_manager_release_sleep_handle(uint8_t handle_index)
//   70 {
hal_sleep_manager_release_sleep_handle:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//   71     sleep_management_release_lock_handle(handle_index);
          CFI FunCall sleep_management_release_lock_handle
        BL       sleep_management_release_lock_handle
//   72     return HAL_SLEEP_MANAGER_OK;
        MOVS     R0,#+0
        POP      {R1,PC}          ;; return
//   73 }
          CFI EndBlock cfiBlock4
//   74 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock5 Using cfiCommon0
          CFI Function hal_sleep_manager_get_lock_status
          CFI FunCall sleep_management_get_lock_sleep_request_info
        THUMB
//   75 uint32_t hal_sleep_manager_get_lock_status(void)
//   76 {
//   77     return sleep_management_get_lock_sleep_request_info();
hal_sleep_manager_get_lock_status:
        B.W      sleep_management_get_lock_sleep_request_info
//   78 }
          CFI EndBlock cfiBlock5
//   79 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock6 Using cfiCommon0
          CFI Function hal_sleep_manager_sleep_driver_dump_handle_name
          CFI FunCall sleep_management_get_lock_sleep_handle_list
        THUMB
//   80 uint32_t hal_sleep_manager_sleep_driver_dump_handle_name(void)
//   81 {
//   82     return sleep_management_get_lock_sleep_handle_list();
hal_sleep_manager_sleep_driver_dump_handle_name:
        B.W      sleep_management_get_lock_sleep_handle_list
//   83 }
          CFI EndBlock cfiBlock6
//   84 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock7 Using cfiCommon0
          CFI Function hal_sleep_manager_is_sleep_locked
          CFI FunCall sleep_management_check_sleep_locks
        THUMB
//   85 bool hal_sleep_manager_is_sleep_locked(void)
//   86 {
//   87     return sleep_management_check_sleep_locks();
hal_sleep_manager_is_sleep_locked:
        B.W      sleep_management_check_sleep_locks
//   88 }
          CFI EndBlock cfiBlock7
//   89 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock8 Using cfiCommon0
          CFI Function hal_sleep_manager_is_sleep_handle_alive
          CFI FunCall sleep_management_check_handle_status
        THUMB
//   90 bool hal_sleep_manager_is_sleep_handle_alive(uint8_t handle_index)
//   91 {
//   92     return sleep_management_check_handle_status(handle_index);
hal_sleep_manager_is_sleep_handle_alive:
        B.W      sleep_management_check_handle_status
//   93 }
          CFI EndBlock cfiBlock8
//   94 
//   95 #ifdef HAL_SLEEP_MANAGER_SUPPORT_POWER_OFF
//   96 void hal_sleep_manager_enter_power_off_mode()
//   97 {
//   98 
//   99 }
//  100 #endif
//  101 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock9 Using cfiCommon0
          CFI Function hal_sleep_manager_set_sleep_time
        THUMB
//  102 hal_sleep_manager_status_t hal_sleep_manager_set_sleep_time(uint32_t sleep_time_ms)
//  103 {
hal_sleep_manager_set_sleep_time:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
        MOV      R4,R0
//  104     hal_gpt_status_t         ret_status;
//  105     hal_gpt_register_callback(DEEP_SLEEP_GPT, (hal_gpt_callback_t)Deep_Sleep_GPT_CB, NULL);
        MOVS     R2,#+0
        LDR.N    R1,??DataTable7
        MOV      R0,R2
          CFI FunCall hal_gpt_register_callback
        BL       hal_gpt_register_callback
//  106     if (sleep_time_ms > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        LDR.N    R0,??DataTable7_1  ;; 0x7c1f07c
        CMP      R4,R0
        BCC.N    ??hal_sleep_manager_set_sleep_time_0
//  107         sleep_time_ms = HAL_GPT_MAXIMUM_MS_TIMER_TIME;
        LDR.N    R4,??DataTable7_2  ;; 0x7c1f07b
//  108     }
//  109     ret_status = hal_gpt_start_timer_ms(DEEP_SLEEP_GPT, sleep_time_ms, HAL_GPT_TIMER_TYPE_ONE_SHOT);
//  110     if (ret_status != HAL_GPT_STATUS_OK) {
??hal_sleep_manager_set_sleep_time_0:
        MOVS     R2,#+0
        MOV      R1,R4
        MOV      R0,R2
          CFI FunCall hal_gpt_start_timer_ms
        BL       hal_gpt_start_timer_ms
        CMP      R0,#+0
        BEQ.N    ??hal_sleep_manager_set_sleep_time_1
//  111         log_hal_info("ERROR : Deep Sleep GPT Start Fail");
        ADR.W    R2,?_0
        MOVS     R1,#+111
        ADR.W    R0,??__FUNCTION__
          CFI FunCall log_hal_info_internal
        BL       log_hal_info_internal
//  112         return HAL_SLEEP_MANAGER_ERROR;
        MOV      R0,#-1
        POP      {R4,PC}
//  113     }
//  114 
//  115     return HAL_SLEEP_MANAGER_OK;
??hal_sleep_manager_set_sleep_time_1:
        MOVS     R0,#+0
        POP      {R4,PC}          ;; return
//  116 }
          CFI EndBlock cfiBlock9
//  117 

        SECTION `.text`:CODE:NOROOT(2)
          CFI Block cfiBlock10 Using cfiCommon0
          CFI Function hal_sleep_manager_enter_sleep_mode
        THUMB
//  118 void hal_sleep_manager_enter_sleep_mode(hal_sleep_mode_t mode)
//  119 {
hal_sleep_manager_enter_sleep_mode:
        PUSH     {R7,LR}
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  120     if (mode == HAL_SLEEP_MODE_IDLE) {
        MOV      R1,R0
        CMP      R1,#+1
        BNE.N    ??hal_sleep_manager_enter_sleep_mode_0
//  121         __asm volatile("dsb");
        dsb
//  122         __asm volatile("wfi");
        wfi
//  123         __asm volatile("isb");
        isb
//  124         hal_gpt_stop_timer(DEEP_SLEEP_GPT);
        MOVS     R0,#+0
        POP      {R1,LR}
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall hal_gpt_stop_timer
        B.W      hal_gpt_stop_timer
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  125     } else if (mode == HAL_SLEEP_MODE_SLEEP) {
??hal_sleep_manager_enter_sleep_mode_0:
        CMP      R1,#+2
        BNE.N    ??hal_sleep_manager_enter_sleep_mode_1
//  126         sleep_management_enter_deep_sleep(mode);
          CFI FunCall sleep_management_enter_deep_sleep
        BL       sleep_management_enter_deep_sleep
//  127         hal_gpt_stop_timer(DEEP_SLEEP_GPT);
        MOVS     R0,#+0
        POP      {R1,LR}
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall hal_gpt_stop_timer
        B.W      hal_gpt_stop_timer
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  128     } else if (mode == HAL_SLEEP_MODE_RETENTION) {
??hal_sleep_manager_enter_sleep_mode_1:
        CMP      R1,#+3
        BNE.N    ??hal_sleep_manager_enter_sleep_mode_2
//  129         sleep_management_enter_retention();
        POP      {R0,LR}
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall sleep_management_enter_retention
        B.W      sleep_management_enter_retention
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  130     }
//  131 }
??hal_sleep_manager_enter_sleep_mode_2:
        POP      {R0,PC}          ;; return
          CFI EndBlock cfiBlock10
//  132 
//  133 #ifdef HAL_SLEEP_MANAGER_SUPPORT_WAKEUP_SOURCE_CONFIG
//  134 hal_sleep_manager_status_t hal_sleep_manager_enable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
//  135 {
//  136     spm_unmask_wakeup_source(pin);
//  137     return HAL_SLEEP_MANAGER_OK;
//  138 }
//  139 
//  140 hal_sleep_manager_status_t hal_sleep_manager_disable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
//  141 {
//  142     spm_mask_wakeup_source(pin);
//  143     return HAL_SLEEP_MANAGER_OK;
//  144 }
//  145 #endif
//  146 

        SECTION `.data`:DATA:REORDER:NOROOT(0)
        DATA
//  147 uint8_t cli_sleep_handle = 0xFF;
cli_sleep_handle:
        DC8 255

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock11 Using cfiCommon0
          CFI Function hal_sleep_manager_init
        THUMB
//  148 hal_sleep_manager_status_t hal_sleep_manager_init()
//  149 {
hal_sleep_manager_init:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  150     log_hal_info("hal_sleep_manager_init start\n");
        ADR.W    R4,??__FUNCTION___1
        ADR.W    R2,?_1
        MOVS     R1,#+150
        MOV      R0,R4
          CFI FunCall log_hal_info_internal
        BL       log_hal_info_internal
//  151     
//  152     sleep_management_low_power_init_setting();
          CFI FunCall sleep_management_low_power_init_setting
        BL       sleep_management_low_power_init_setting
//  153 
//  154     spm_init(0);
        MOVS     R0,#+0
          CFI FunCall spm_init
        BL       spm_init
//  155 
//  156     #if (PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 7686)
//  157     spm_control_mtcmos(SPM_MTCMOS_CONN      ,SPM_MTCMOS_PWR_DISABLE);
        MOVS     R1,#+0
        MOV      R0,R1
          CFI FunCall spm_control_mtcmos
        BL       spm_control_mtcmos
//  158     #endif
//  159     spm_control_mtcmos(SPM_MTCMOS_SDIO_SLV  ,SPM_MTCMOS_PWR_DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+1
          CFI FunCall spm_control_mtcmos
        BL       spm_control_mtcmos
//  160     
//  161     if (hal_gpt_init(DEEP_SLEEP_GPT) != HAL_GPT_STATUS_OK) {
        MOVS     R0,#+0
          CFI FunCall hal_gpt_init
        BL       hal_gpt_init
        CMP      R0,#+0
        BEQ.N    ??hal_sleep_manager_init_0
//  162         log_hal_info("ERROR : Deep Sleep GPT Init Fail");
        ADR.W    R2,?_2
        MOVS     R1,#+162
        MOV      R0,R4
          CFI FunCall log_hal_info_internal
        BL       log_hal_info_internal
//  163         return HAL_SLEEP_MANAGER_ERROR;
        MOV      R0,#-1
        POP      {R4,PC}
//  164     }
//  165 
//  166     cli_sleep_handle = hal_sleep_manager_set_sleep_handle("CLI_Sleep");
??hal_sleep_manager_init_0:
        ADR.W    R0,?_3
          CFI FunCall hal_sleep_manager_set_sleep_handle
        BL       hal_sleep_manager_set_sleep_handle
        LDR.N    R1,??DataTable7_3
        STRB     R0,[R1, #+0]
//  167     //hal_sleep_manager_lock_sleep(cli_sleep_handle);
//  168     return HAL_SLEEP_MANAGER_OK;
        MOVS     R0,#+0
        POP      {R4,PC}          ;; return
//  169 }
          CFI EndBlock cfiBlock11

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable7:
        DC32     Deep_Sleep_GPT_CB

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable7_1:
        DC32     0x7c1f07c

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable7_2:
        DC32     0x7c1f07b

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable7_3:
        DC32     cli_sleep_handle

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const hal_sleep_manager_set_sleep_time::__FUNCTION__[33]
??__FUNCTION__:
        DC8 "hal_sleep_manager_set_sleep_time"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_0:
        DC8 "ERROR : Deep Sleep GPT Start Fail"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const hal_sleep_manager_init::__FUNCTION__[23]
??__FUNCTION___1:
        DC8 "hal_sleep_manager_init"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_1:
        DC8 "hal_sleep_manager_init start\012"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_2:
        DC8 "ERROR : Deep Sleep GPT Init Fail"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_3:
        DC8 "CLI_Sleep"
        DC8 0, 0

        SECTION `.iar_vfe_header`:DATA:NOALLOC:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
        DC32 0

        SECTION __DLIB_PERTHREAD:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        SECTION __DLIB_PERTHREAD_init:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        END
//  170 
//  171 #endif /* HAL_SLEEP_MANAGER_ENABLED */
// 
//   1 byte  in section .data
// 464 bytes in section .text
// 
// 464 bytes of CODE memory
//   1 byte  of DATA memory
//
//Errors: none
//Warnings: none
