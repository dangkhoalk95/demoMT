///////////////////////////////////////////////////////////////////////////////
//
// IAR ANSI C/C++ Compiler V7.50.1.10123/W32 for ARM      07/Apr/2017  14:04:55
// Copyright 1999-2015 IAR Systems AB.
//
//    Cpu mode     =  thumb
//    Endian       =  little
//    Source file  =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_spm.c
//    Command line =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_spm.c -D
//        MTK_DEBUG_LEVEL_INFO -D PRODUCT_VERSION=7686 -lcN
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
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\List\hal_spm.s
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

        EXTERN hal_clock_disable
        EXTERN hal_clock_enable
        EXTERN hal_gpt_delay_us
        EXTERN log_hal_error_internal
        EXTERN log_hal_warning_internal
        EXTERN memcmp
        EXTERN pmu_set_register_value
        EXTERN printf

        PUBLIC manual_spm_read_im
        PUBLIC manual_spm_write_im
        PUBLIC pcm_event_vector_parameter
        PUBLIC spm_control_mtcmos
        PUBLIC spm_control_mtcmos_internal
        PUBLIC spm_init
        PUBLIC spm_kick_start
        PUBLIC spm_mask_wakeup_source
        PUBLIC spm_unmask_wakeup_source
        
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
        
// W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_spm.c
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
//   35 #include "hal_spm.h"
//   36 
//   37 #ifdef HAL_SLEEP_MANAGER_ENABLED
//   38 
//   39 #include <stdio.h>
//   40 #include <string.h>
//   41 #include "hal_log.h"
//   42 #include "memory_attribute.h"
//   43 #include "hal_nvic.h"
//   44 #include "hal_nvic_internal.h"
//   45 #include "assert.h"
//   46 #include "hal_gpt.h"
//   47 #include "hal_pmu.h"
//   48 #include "hal_clock.h"
//   49 #include "hal_platform.h"
//   50 
//   51 /* SPM Code Rev : 20170306 - pcm_suspend.spm */

        SECTION `.rodata`:CONST:NOROOT(2)
        DATA
//   52 const unsigned int pcm_event_vector_parameter[8] = {0x31, 0x140032, 0x460033, 0x5a0034, 0x680035, 0x0, 0x0, 0x0};
pcm_event_vector_parameter:
        DC32 49, 1310770, 4587571, 5898292, 6815797, 0, 0, 0
//   53 #define PCM_IM_MAX_LENGTH 612
//   54 static const unsigned int spm_im[PCM_IM_MAX_LENGTH] = {
//   55     0x1910001f, 0xa20f0810, 0x81000404, 0x1950001f, 0xa20f0810, 0x81421401, 0xa1401005, 0xd82001a5, 0x17c07c1f,
//   56     0x1b00001f, 0x40000001, 0xf0000000, 0x17c07c1f, 0x18c0001f, 0xa20f0810, 0xe0e00010, 0x1b00001f, 0x80000001,
//   57     0xf0000000, 0x17c07c1f, 0x18c0001f, 0xa20f03a8, 0x1910001f, 0xa20f03a8, 0x89000004, 0xfffffeff, 0xe0c00004,
//   58     0x18c0001f, 0xa21f0010, 0x1900001f, 0x00010001, 0xe0c00004, 0x18c0001f, 0xa20f02c0, 0xe0e00001, 0x18d0001f,
//   59     0xa21f0010, 0x814c0c01, 0x1990001f, 0xa20f02c0, 0x81841801, 0x1910001f, 0xa20f03ac, 0x81081001, 0xa1a01006,
//   60     0x81001805, 0xd8200464, 0x17c07c1f, 0x88d00001, 0xa0050000, 0xd8200603, 0x17c07c1f, 0x18c0001f, 0xa20f0810,
//   61     0xe0e00004, 0x18c0001f, 0xa20f0204, 0x1940001f, 0x00000001, 0x1910001f, 0xa20f03ac, 0x81041001, 0xc8e01c24,
//   62     0x17c07c1f, 0xa0118400, 0xa0110400, 0x1b00001f, 0x042a2001, 0xf0000000, 0x17c07c1f, 0x18c0001f, 0xa20f0810,
//   63     0xe0e00008, 0x18c0001f, 0xa20f03a8, 0x1910001f, 0xa20f03a8, 0x89000004, 0xfffffefe, 0xe0c00004, 0x1b00001f,
//   64     0x04122001, 0xe8208000, 0xa20f0300, 0x0b16ff0c, 0xe8208000, 0xa20f0300, 0x0b16ff08, 0xf0000000, 0x17c07c1f,
//   65     0x18c0001f, 0xa20f0810, 0xe0e00004, 0x18c0001f, 0xa20f03a8, 0x1910001f, 0xa20f03a8, 0xa9000004, 0x00000001,
//   66     0xe0c00004, 0x1b00001f, 0x042a2001, 0xf0000000, 0x17c07c1f, 0x18c0001f, 0xa20f0810, 0xe0e00002, 0x18c0001f,
//   67     0xa20f0204, 0x1940001f, 0x00000001, 0x1910001f, 0xa20f0808, 0x81009001, 0xc8e011c4, 0x17c07c1f, 0x18c0001f,
//   68     0xa21f0010, 0x1900001f, 0x00000001, 0xe0c00004, 0x18c0001f, 0xa20f02c0, 0xe0e00000, 0x80328400, 0x1b80001f,
//   69     0x2000259f, 0x80310400, 0x1b80001f, 0x20000cda, 0x80318400, 0x18c0001f, 0xa20f03a8, 0x1910001f, 0xa20f03a8,
//   70     0xa9000004, 0x00000101, 0xe0c00004, 0x1b00001f, 0x04062001, 0xf0000000, 0x17c07c1f, 0xe0e00016, 0x1b80001f,
//   71     0x20000001, 0xe0e0001e, 0x1b80001f, 0x20000001, 0x81801401, 0xd80013e6, 0x17c07c1f, 0x81809401, 0xd8001686,
//   72     0x17c07c1f, 0x81811401, 0xd8001a46, 0x17c07c1f, 0xd0001b00, 0x17c07c1f, 0x1900001f, 0xa20f0254, 0x1980001f,
//   73     0x00010101, 0xe1000006, 0x1b80001f, 0x2000000e, 0x1980001f, 0x00000101, 0xe1000006, 0x1b80001f, 0x2000000e,
//   74     0xe1200100, 0x1900001f, 0xa20f02b0, 0x1980001f, 0x00010100, 0xe1000006, 0xe1200100, 0xd0001b00, 0x17c07c1f,
//   75     0x1900001f, 0xa20f0230, 0xe120003f, 0xe120001f, 0xe120000f, 0xe1200007, 0xe1200003, 0xe1200001, 0xe1200000,
//   76     0x1b80001f, 0x20000122, 0x1900001f, 0xa20f0224, 0x1980001f, 0x00010101, 0xe1000006, 0x1b80001f, 0x2000000e,
//   77     0x1980001f, 0x00000101, 0xe1000006, 0x1b80001f, 0x2000000e, 0xe1200100, 0x1900001f, 0xa20f0234, 0xe1200100,
//   78     0xe1200000, 0xd0001b00, 0x17c07c1f, 0x1900001f, 0xa20f0260, 0x1980001f, 0x00010100, 0xe1000006, 0xe1200100,
//   79     0xe0e0000e, 0xe0e0000c, 0xe0e0001c, 0x1b80001f, 0x20000004, 0xe0e0001d, 0xe0e0000d, 0xf0000000, 0x17c07c1f,
//   80     0xe0e0001d, 0xe0e0001f, 0x81801401, 0xd8001dc6, 0x17c07c1f, 0x81809401, 0xd8002086, 0x17c07c1f, 0x81811401,
//   81     0xd8002326, 0x17c07c1f, 0xd0002420, 0x17c07c1f, 0x1900001f, 0xa20f0254, 0xe1200101, 0x1980001f, 0x00010101,
//   82     0xe1000006, 0x1b80001f, 0x2000000e, 0x1980001f, 0x00010001, 0xe1000006, 0x1b80001f, 0x2000000e, 0x1900001f,
//   83     0xa20f02b0, 0x1980001f, 0x00010100, 0xe1000006, 0x1980001f, 0x00010101, 0xe1000006, 0xd0002420, 0x1900001f,
//   84     0xa20f0230, 0xe120007f, 0x1900001f, 0xa20f0224, 0xe1200101, 0x1980001f, 0x00010101, 0xe1000006, 0x1b80001f,
//   85     0x2000000e, 0x1980001f, 0x00010001, 0xe1000006, 0x1b80001f, 0x2000000e, 0x1900001f, 0xa20f0234, 0xe1200100,
//   86     0xe120010f, 0xd0002420, 0x1900001f, 0xa20f0260, 0x1980001f, 0x00010100, 0xe1000006, 0x1980001f, 0x00010101,
//   87     0xe1000006, 0xe0e0001e, 0xe0e0001a, 0xe0e00012, 0xf0000000, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   88     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   89     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   90     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   91     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   92     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   93     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   94     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   95     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   96     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   97     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   98     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//   99     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  100     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  101     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  102     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  103     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  104     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  105     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  106     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  107     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  108     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  109     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  110     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f,
//  111     0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x17c07c1f, 0x1840001f,
//  112     0x00000001, 0x18c0001f, 0xa20f0810, 0xe0e00001, 0xa0100400, 0xa1d00407, 0x18c0001f, 0xa20f0380, 0xe0f07fff,
//  113     0x1b00001f, 0x04032001, 0x81f10407, 0x1b80001f, 0xd0100001, 0x810f3001, 0xd8004bc4, 0x17c07c1f, 0xe8208000,
//  114     0xa20f0300, 0x0b16ff08, 0x18c0001f, 0xa20f0810, 0xe0e00002, 0x18c0001f, 0xa21f0010, 0xe0e00001, 0x18d0001f,
//  115     0xa21f0010, 0x81040c01, 0xd8204364, 0x17c07c1f, 0x18c0001f, 0xa20f0200, 0x1940001f, 0x00000002, 0xc0c01c20,
//  116     0x17c07c1f, 0x18c0001f, 0xc0070140, 0x1900001f, 0x90904bed, 0xe0c00004, 0xa0120400, 0x1b00001f, 0x04062001,
//  117     0x1b80001f, 0x90100001, 0x1b80001f, 0x2000259f, 0x80310400, 0x1b80001f, 0x20000cda, 0x80318400, 0x80320400,
//  118     0x80328400, 0x1910001f, 0xa20f0808, 0x81009001, 0x18c0001f, 0xa20f0204, 0x1940001f, 0x00000001, 0xc8e011c4,
//  119     0x17c07c1f, 0x18c0001f, 0xa20f03a8, 0x1910001f, 0xa20f03a8, 0xa9000004, 0x00000101, 0xe0c00004, 0x18c0001f,
//  120     0xa20f0200, 0x1940001f, 0x00000002, 0xc0c011c0, 0x17c07c1f, 0x18c0001f, 0xa21f0010, 0x1900001f, 0x00000000,
//  121     0xe0c00004, 0x18c0001f, 0xa20f02c0, 0xe0e00000, 0x18c0001f, 0xc0070140, 0x1900001f, 0xfee1900d, 0xe0c00004,
//  122     0xe8208000, 0xa20f0300, 0x0b16ff08, 0x18c0001f, 0xa20f0810, 0xe0e00000, 0x81f00407, 0xa1d10407, 0xf0000000,
//  123 };
//  124 
//  125 
//  126 uint32_t manual_spm_read_im(uint32_t addr);
//  127 void manual_spm_write_im(uint32_t addr, uint32_t data);
//  128 void spm_control_mtcmos_internal(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl);
//  129 

        SECTION `.tcm_code`:CODE:NOROOT(2)
          CFI Block cfiBlock0 Using cfiCommon0
          CFI Function spm_kick_start
        THUMB
//  130 ATTR_TEXT_IN_TCM void spm_kick_start(void)
//  131 {
spm_kick_start:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  132     hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
          CFI FunCall hal_clock_enable
        BL       hal_clock_enable
//  133 
//  134     //Need reset SPM ,after wakeup from sleep
//  135     /* [4]: Resets PCM */
//  136     *SPM_PCM_CON0 = 0x0B160000 | 0x10;
        LDR.N    R4,??spm_kick_start_0  ;; 0xa20f0300
        LDR.N    R0,??spm_kick_start_0+0x4  ;; 0xb160010
        STR      R0,[R4, #+0]
//  137     hal_gpt_delay_us(10);
        MOVS     R0,#+10
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  138     *SPM_PCM_CON0 = 0x0B160000;
        LDR.N    R0,??spm_kick_start_0+0x8  ;; 0xb160000
        STR      R0,[R4, #+0]
//  139     hal_gpt_delay_us(10);
        MOVS     R0,#+10
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  140 
//  141     //*SPM_SIDEBAND_CONTROL_0 |= (0x0F << 16); // wait UART_SLEEP_ACK = 0xF
//  142 
//  143     *SPM_POWER_ON_VAL0 = *SPM_PCM_REG0_DATA;
        LDR.N    R0,??spm_kick_start_0+0xC  ;; 0xa20f0010
        LDR.N    R1,??spm_kick_start_0+0x10  ;; 0xa20f0900
        LDR      R2,[R1, #+0]
        STR      R2,[R0, #+0]
//  144     *SPM_POWER_ON_VAL1 = *SPM_PCM_REG7_DATA;
        LDR      R1,[R1, #+28]
        STR      R1,[R0, #+4]
//  145     *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL0 | 0x1;   // [0]: sc_xo_cg_en default =1
        LDR      R1,[R0, #+0]
        ORR      R1,R1,#0x1
        STR      R1,[R4, #+56]
//  146 
//  147     *SPM_PCM_PWR_IO_EN = 0x00010000;                    // for R0
        MOV      R1,#+65536
        STR      R1,[R4, #+96]
//  148     *SPM_PCM_PWR_IO_EN = 0x00000000;
        MOVS     R1,#+0
        STR      R1,[R4, #+96]
//  149     *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL1;
        LDR      R0,[R0, #+4]
        STR      R0,[R4, #+56]
//  150     *SPM_PCM_PWR_IO_EN = 0x00800000;                    // for R7
        MOV      R0,#+8388608
        STR      R0,[R4, #+96]
//  151     *SPM_PCM_PWR_IO_EN = 0x00000000;
        MOV      R0,R1
        STR      R0,[R4, #+96]
//  152 
//  153     *SPM_PCM_PWR_IO_EN = 0x81;
        MOVS     R0,#+129
        STR      R0,[R4, #+96]
//  154 
//  155     *SPM_SLEEP_WAKEUP_EVENT_MASK = 0xFFFFFFFF;          //mask all AP wakeup event
        MOV      R0,#-1
        LDR.N    R1,??spm_kick_start_0+0x14  ;; 0xa20f0380
        STR      R0,[R1, #+0]
//  156 
//  157     *SPM_PCM_CON0 = 0x0B160103;
        LDR.N    R0,??spm_kick_start_0+0x18  ;; 0xb160103
        STR      R0,[R4, #+0]
//  158 }
        POP      {R4,PC}          ;; return
        DATA
??spm_kick_start_0:
        DC32     0xa20f0300
        DC32     0xb160010
        DC32     0xb160000
        DC32     0xa20f0010
        DC32     0xa20f0900
        DC32     0xa20f0380
        DC32     0xb160103
          CFI EndBlock cfiBlock0
//  159 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock1 Using cfiCommon0
          CFI Function spm_init
        THUMB
//  160 void spm_init(uint32_t spm_auto_load)
//  161 {
spm_init:
        PUSH     {R4-R8,LR}
          CFI R14 Frame(CFA, -4)
          CFI R8 Frame(CFA, -8)
          CFI R7 Frame(CFA, -12)
          CFI R6 Frame(CFA, -16)
          CFI R5 Frame(CFA, -20)
          CFI R4 Frame(CFA, -24)
          CFI CFA R13+24
        SUB      SP,SP,#+2448
          CFI CFA R13+2472
        MOV      R8,R0
//  162     uint32_t i, status;
//  163 
//  164     if (HAL_CLOCK_STATUS_OK != hal_clock_enable(HAL_CLOCK_CG_SW_SPM)) {
        MOVS     R0,#+48
          CFI FunCall hal_clock_enable
        BL       hal_clock_enable
        CMP      R0,#+0
        BEQ.N    ??spm_init_0
//  165         log_hal_error("\r\n [SPM] Clock enable failed!");
        ADR.W    R2,?_0
        MOVS     R1,#+165
        ADR.W    R0,??__FUNCTION__
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  166     }
//  167 
//  168     /* [4]: Resets PCM */
//  169     *SPM_PCM_CON0 = 0x0B160000 | 0x10;
??spm_init_0:
        LDR.W    R6,??DataTable15  ;; 0xa20f0300
        LDR.W    R0,??DataTable15_1  ;; 0xb160010
        STR      R0,[R6, #+0]
//  170     hal_gpt_delay_us(10);
        MOVS     R0,#+10
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  171     *SPM_PCM_CON0 = 0x0B160000;
        LDR.W    R0,??DataTable15_2  ;; 0xb160000
        STR      R0,[R6, #+0]
//  172     hal_gpt_delay_us(10);
        MOVS     R0,#+10
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  173 
//  174     //*SPM_SIDEBAND_CONTROL_0 |= (0x0F << 16); // wait UART_SLEEP_ACK = 0xF
//  175 
//  176     /* Load PCM_EVENT_VECTOR[0-7] */
//  177     *SPM_PCM_EVENT_VECTOR0 = pcm_event_vector_parameter[0];
        MOVS     R0,#+49
        STR      R0,[R6, #+64]
//  178     *SPM_PCM_EVENT_VECTOR1 = pcm_event_vector_parameter[1];
        LDR.W    R0,??DataTable15_3  ;; 0x140032
        STR      R0,[R6, #+68]
//  179     *SPM_PCM_EVENT_VECTOR2 = pcm_event_vector_parameter[2];
        LDR.W    R0,??DataTable15_4  ;; 0x460033
        STR      R0,[R6, #+72]
//  180     *SPM_PCM_EVENT_VECTOR3 = pcm_event_vector_parameter[3];
        LDR.W    R0,??DataTable15_5  ;; 0x5a0034
        STR      R0,[R6, #+76]
//  181     *SPM_PCM_EVENT_VECTOR4 = pcm_event_vector_parameter[4];
        LDR.W    R0,??DataTable15_6  ;; 0x680035
        STR      R0,[R6, #+80]
//  182     *SPM_PCM_EVENT_VECTOR5 = pcm_event_vector_parameter[5];
        MOVS     R0,#+0
        STR      R0,[R6, #+84]
//  183     *SPM_PCM_EVENT_VECTOR6 = pcm_event_vector_parameter[6];
        STR      R0,[R6, #+88]
//  184     *SPM_PCM_EVENT_VECTOR7 = pcm_event_vector_parameter[7];
        STR      R0,[R6, #+92]
//  185 
//  186     *SPM_POWER_ON_VAL0 = 0;
        LDR.W    R4,??DataTable15_7  ;; 0xa20f0000
        STR      R0,[R4, #+16]
//  187     *SPM_POWER_ON_VAL1 = 0x44;
        MOVS     R0,#+68
        STR      R0,[R4, #+20]
//  188 
//  189     *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL0 | 0x1; // [0]: sc_xo_cg_en default =1
        LDR      R0,[R4, #+16]
        ORR      R0,R0,#0x1
        STR      R0,[R6, #+56]
//  190 
//  191     *SPM_PCM_PWR_IO_EN = 0x00010000; // for R0
        MOV      R0,#+65536
        STR      R0,[R6, #+96]
//  192     *SPM_PCM_PWR_IO_EN = 0x00000000;
        MOVS     R0,#+0
        STR      R0,[R6, #+96]
//  193     *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL1;
        LDR      R0,[R4, #+20]
        STR      R0,[R6, #+56]
//  194     *SPM_PCM_PWR_IO_EN = 0x00800000; // for R7
        MOV      R0,#+8388608
        STR      R0,[R6, #+96]
//  195     *SPM_PCM_PWR_IO_EN = 0x00000000;
        MOVS     R0,#+0
        STR      R0,[R6, #+96]
//  196     *SPM_CLK_SETTLE = 0x3;
        LDR.W    R5,??DataTable15_8  ;; 0xa20f03ac
        MOVS     R0,#+3
        STR      R0,[R5, #+20]
//  197 
//  198     /* Init IM Length and pointer */
//  199     *SPM_PCM_IM_LEN = PCM_IM_MAX_LENGTH;
        MOV      R0,#+612
        STR      R0,[R6, #+36]
//  200     *SPM_PCM_IM_PTR = (volatile uint32_t)spm_im;
        ADR.W    R7,spm_im
        STR      R7,[R6, #+32]
//  201     if (spm_auto_load != 0) {
        CMP      R8,#+0
        BEQ.N    ??spm_init_1
//  202         *SPM_PCM_CON1 = 0x0B160001; // Enables IM slave mode
        LDR.W    R0,??DataTable15_9  ;; 0xb160001
        STR      R0,[R6, #+4]
//  203         /* Kick IM process */
//  204         *SPM_PCM_CON0 = 0x0B160002;
        LDR.W    R0,??DataTable15_10  ;; 0xb160002
        STR      R0,[R6, #+0]
//  205         printf("spm_auto_load\r\n");
        ADR.W    R0,?_1
          CFI FunCall printf
        BL       printf
        B.N      ??spm_init_2
//  206     } else {
//  207         printf("manual load spm code\r\n");
??spm_init_1:
        ADR.W    R0,?_2
          CFI FunCall printf
        BL       printf
//  208         /* manual load spm code */
//  209         for (i = 0; i < PCM_IM_MAX_LENGTH; i++) {
        MOV      R8,#+0
        B.N      ??spm_init_3
//  210             manual_spm_write_im(i, spm_im[i]);
??spm_init_4:
        LDR      R1,[R7, R8, LSL #+2]
        MOV      R0,R8
          CFI FunCall manual_spm_write_im
        BL       manual_spm_write_im
//  211         }
        ADD      R8,R8,#+1
??spm_init_3:
        CMP      R8,#+612
        BCC.N    ??spm_init_4
//  212 
//  213         *SPM_PCM_CON1 = 0x0B163C49;
        LDR.W    R0,??DataTable15_11  ;; 0xb163c49
        STR      R0,[R6, #+4]
//  214         /* Kick IM process */
//  215         *SPM_PCM_CON0 = 0x0B160002;
        LDR.W    R0,??DataTable15_10  ;; 0xb160002
        STR      R0,[R6, #+0]
//  216     }
//  217 
//  218     /* Wait ready state */
//  219     do {
//  220         status = (*SPM_PCM_FSM_STA >> 9) & 0x1;
??spm_init_2:
        LDR.W    R0,??DataTable15_12  ;; 0xa20f09e4
        LDR      R0,[R0, #+0]
        UBFX     R0,R0,#+9,#+1
//  221     } while (status != 0x01);
        CMP      R0,#+1
        BNE.N    ??spm_init_2
//  222 
//  223     /* Read back spm code */
//  224     uint32_t im_check_buf[PCM_IM_MAX_LENGTH];
//  225     for (i = 0; i < PCM_IM_MAX_LENGTH; i++) {
        MOV      R8,#+0
        B.N      ??spm_init_5
//  226         im_check_buf[i] = manual_spm_read_im(i);
??spm_init_6:
        MOV      R0,R8
          CFI FunCall manual_spm_read_im
        BL       manual_spm_read_im
        MOV      R1,SP
        STR      R0,[R1, R8, LSL #+2]
//  227     }
        ADD      R8,R8,#+1
??spm_init_5:
        CMP      R8,#+612
        BCC.N    ??spm_init_6
//  228 
//  229     /* Check SPM Code */
//  230     if (memcmp(im_check_buf, spm_im, PCM_IM_MAX_LENGTH * 4) == 0) {
        ADR.W    R8,??__FUNCTION__
        MOV      R2,#+2448
        MOV      R1,R7
        MOV      R0,SP
          CFI FunCall memcmp
        BL       memcmp
        CMP      R0,#+0
        BNE.N    ??spm_init_7
//  231         log_hal_warning("SPM Code loading Success\r\n");
        ADR.W    R2,?_3
        MOVS     R1,#+231
        MOV      R0,R8
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
        B.N      ??spm_init_8
//  232     } else {
//  233         log_hal_warning("SPM Code loading Fail\r\n");
??spm_init_7:
        ADR.W    R2,?_4
        MOVS     R1,#+233
        MOV      R0,R8
          CFI FunCall log_hal_warning_internal
        BL       log_hal_warning_internal
//  234     }
//  235 
//  236     *SPM_PCM_PWR_IO_EN = 0x0081;                // enable R0 & R7 output
??spm_init_8:
        MOVS     R0,#+129
        STR      R0,[R6, #+96]
//  237     *SPM_SEQUENCER_26M_REG_1 = 0x1B;
        LDR.N    R0,??DataTable15_13  ;; 0xa20f0104
        MOVS     R1,#+27
        STR      R1,[R0, #+64]
//  238     *SPM_SEQUENCER_32K_REG_3 = (0x18);          //pmu guard time need check!!!
        MOVS     R1,#+24
        STR      R1,[R0, #+8]
//  239 
//  240     /* SYSTEM_INFOD */
//  241     if (((*(volatile uint32_t *)(0xA2010040)) & 0x20) == 0) {
        LDR.N    R1,??DataTable15_14  ;; 0xa2010040
        LDR      R1,[R1, #+0]
        LSLS     R1,R1,#+26
        LDR      R1,[R0, #+0]
        BMI.N    ??spm_init_9
//  242         /* use external 32K */
//  243         *SPM_SEQUENCER_32K_REG_1 &= 0xFF00;     //XO_OFF_RISE_SETTLE_TIME = 0
        AND      R1,R1,#0xFF00
        STR      R1,[R0, #+0]
//  244         *SPM_SEQUENCER_32K_REG_2 &= 0xFF00;     //XO_OFF_FALL_SETTLE_TIME = 0
        LDR      R1,[R0, #+4]
        AND      R1,R1,#0xFF00
        STR      R1,[R0, #+4]
        B.N      ??spm_init_10
//  245     } else {
//  246         /* use internal 32K */
//  247         *SPM_SEQUENCER_32K_REG_1 |= 0x000C;     //XO_OFF_RISE_SETTLE_TIME = 0x0C
??spm_init_9:
        ORR      R1,R1,#0xC
        STR      R1,[R0, #+0]
//  248         *SPM_SEQUENCER_32K_REG_2 |= 0x0029;     //XO_OFF_FALL_SETTLE_TIME = 0x29
        LDR      R1,[R0, #+4]
        ORR      R1,R1,#0x29
        STR      R1,[R0, #+4]
//  249     }
//  250 
//  251     *SPM_RESOURCE_CONTROL_0 |= 0x00010000;      //Force on CONN MTCMOS
??spm_init_10:
        LDR      R0,[R5, #+0]
        ORR      R0,R0,#0x10000
        STR      R0,[R5, #+0]
//  252     *SPM_RESOURCE_CONTROL_0 |= 0x01000000;      //Force on SDIO MTCMOS
        LDR      R0,[R5, #+0]
        ORR      R0,R0,#0x1000000
        STR      R0,[R5, #+0]
//  253 
//  254     *SPM_WAKEUP_SOURCE_MASK = 0x80;             //enable all wakeup source,without debugsys
        MOVS     R0,#+128
        STR      R0,[R4, #+0]
//  255 
//  256     if (HAL_CLOCK_STATUS_OK != hal_clock_disable(HAL_CLOCK_CG_SW_SPM)) {
        MOVS     R0,#+48
          CFI FunCall hal_clock_disable
        BL       hal_clock_disable
        CMP      R0,#+0
        BEQ.N    ??spm_init_11
//  257         log_hal_error("\r\n [SPM] Clock disable failed!");
        ADR.W    R2,?_5
        MOVW     R1,#+257
        MOV      R0,R8
          CFI FunCall log_hal_error_internal
        BL       log_hal_error_internal
//  258     }
//  259 }
??spm_init_11:
        ADD      SP,SP,#+2448
          CFI CFA R13+24
        POP      {R4-R8,PC}       ;; return
          CFI EndBlock cfiBlock1
//  260 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock2 Using cfiCommon0
          CFI Function spm_control_mtcmos
        THUMB
//  261 uint32_t spm_control_mtcmos(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl)
//  262 {
spm_control_mtcmos:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  263     static uint32_t mtcmos_conn_resource_cnt = 0, mtcmos_sdio_resource_cnt = 0;
//  264     if (mtcmos == SPM_MTCMOS_CONN) {
        CMP      R0,#+0
        BNE.N    ??spm_control_mtcmos_0
//  265         if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
        LDR.N    R4,??DataTable15_15
        LDR      R0,[R4, #+0]
        CMP      R1,#+0
        BNE.N    ??spm_control_mtcmos_1
//  266             if (mtcmos_conn_resource_cnt == 0) {
        CMP      R0,#+0
        BNE.N    ??spm_control_mtcmos_2
//  267                 spm_control_mtcmos_internal(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_DISABLE);
        MOVS     R1,#+0
        MOV      R0,R1
          CFI FunCall spm_control_mtcmos_internal
        BL       spm_control_mtcmos_internal
//  268                 return 0;
        MOVS     R0,#+0
        POP      {R4,PC}
//  269             }
//  270             mtcmos_conn_resource_cnt--;
??spm_control_mtcmos_2:
        SUBS     R0,R0,#+1
        STR      R0,[R4, #+0]
//  271             if (mtcmos_conn_resource_cnt == 0) {
        BNE.N    ??spm_control_mtcmos_3
//  272                 spm_control_mtcmos_internal(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_DISABLE);
        MOVS     R1,#+0
        MOV      R0,R1
          CFI FunCall spm_control_mtcmos_internal
        BL       spm_control_mtcmos_internal
        B.N      ??spm_control_mtcmos_3
//  273             }
//  274         } else {
//  275             mtcmos_conn_resource_cnt++;
??spm_control_mtcmos_1:
        ADDS     R0,R0,#+1
        STR      R0,[R4, #+0]
//  276             if (mtcmos_conn_resource_cnt != 0) {
        BEQ.N    ??spm_control_mtcmos_3
//  277                 spm_control_mtcmos_internal(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+0
          CFI FunCall spm_control_mtcmos_internal
        BL       spm_control_mtcmos_internal
//  278             }
//  279         }
//  280         return (mtcmos_conn_resource_cnt);
??spm_control_mtcmos_3:
        LDR      R0,[R4, #+0]
        POP      {R4,PC}
//  281     }
//  282 
//  283     if (mtcmos == SPM_MTCMOS_SDIO_SLV) {
??spm_control_mtcmos_0:
        CMP      R0,#+1
        BNE.N    ??spm_control_mtcmos_4
//  284         if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
        LDR.N    R4,??DataTable15_15
        LDR      R0,[R4, #+4]
        CMP      R1,#+0
        BNE.N    ??spm_control_mtcmos_5
//  285             if (mtcmos_sdio_resource_cnt == 0) {
        CMP      R0,#+0
        BNE.N    ??spm_control_mtcmos_6
//  286                 spm_control_mtcmos_internal(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+1
          CFI FunCall spm_control_mtcmos_internal
        BL       spm_control_mtcmos_internal
//  287                 return 0;
        MOVS     R0,#+0
        POP      {R4,PC}
//  288             }
//  289             mtcmos_sdio_resource_cnt--;
??spm_control_mtcmos_6:
        SUBS     R0,R0,#+1
        STR      R0,[R4, #+4]
//  290             if (mtcmos_sdio_resource_cnt == 0) {
        BNE.N    ??spm_control_mtcmos_7
//  291                 spm_control_mtcmos_internal(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+1
          CFI FunCall spm_control_mtcmos_internal
        BL       spm_control_mtcmos_internal
        B.N      ??spm_control_mtcmos_7
//  292             }
//  293         } else {
//  294             mtcmos_sdio_resource_cnt++;
??spm_control_mtcmos_5:
        ADDS     R0,R0,#+1
        STR      R0,[R4, #+4]
//  295             if (mtcmos_sdio_resource_cnt != 0) {
        BEQ.N    ??spm_control_mtcmos_7
//  296                 spm_control_mtcmos_internal(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_ENABLE);
        MOVS     R1,#+1
        MOV      R0,R1
          CFI FunCall spm_control_mtcmos_internal
        BL       spm_control_mtcmos_internal
//  297             }
//  298         }
//  299         return (mtcmos_sdio_resource_cnt);
??spm_control_mtcmos_7:
        LDR      R0,[R4, #+4]
        POP      {R4,PC}
//  300     }
//  301     return 0;
??spm_control_mtcmos_4:
        MOVS     R0,#+0
        POP      {R4,PC}          ;; return
//  302 }
          CFI EndBlock cfiBlock2

        SECTION `.bss`:DATA:REORDER:NOROOT(2)
        DATA
??mtcmos_conn_resource_cnt:
        DS8 4
        DS8 4
//  303 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock3 Using cfiCommon0
          CFI Function spm_control_mtcmos_internal
        THUMB
//  304 void spm_control_mtcmos_internal(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl)
//  305 {
spm_control_mtcmos_internal:
        PUSH     {R4,LR}
          CFI R14 Frame(CFA, -4)
          CFI R4 Frame(CFA, -8)
          CFI CFA R13+8
//  306     if (mtcmos == SPM_MTCMOS_CONN) {
        CMP      R0,#+0
        BNE.N    ??spm_control_mtcmos_internal_0
//  307         if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
        LDR.N    R0,??DataTable15_16  ;; 0xa20f0808
        CMP      R1,#+0
        BNE.N    ??spm_control_mtcmos_internal_1
//  308             if ((*SPM_PWR_STATUS & 0x4) == 0) {
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+29
        BPL.W    ??spm_control_mtcmos_internal_2
//  309                 return;
//  310             }
//  311             /* E1 sw WK,E2 need remove */
//  312             pmu_set_register_value(PMU_DIG_VCORE_ANA_CON3, PMU_RG_BUCK_ZXPDN_MASK, PMU_RG_BUCK_ZXPDN_SHIFT, 0); //enable PMU FPWM
        MOVS     R3,#+0
        MOVS     R2,#+4
        MOVS     R1,#+1
        LDR.N    R0,??DataTable15_17  ;; 0xa207010c
          CFI FunCall pmu_set_register_value
        BL       pmu_set_register_value
//  313 
//  314             hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
          CFI FunCall hal_clock_enable
        BL       hal_clock_enable
//  315             *SPM_RESOURCE_CONTROL_0 &= 0xFFFEFFFF;   //Force off CONN MTCMOS
        LDR.N    R0,??DataTable15_8  ;; 0xa20f03ac
        LDR      R1,[R0, #+0]
        BIC      R1,R1,#0x10000
        STR      R1,[R0, #+0]
//  316             //CONN MTCMOS OFF
//  317             *SPM_CONN_PWR_CON = 0x1D;
        LDR.N    R4,??DataTable15_18  ;; 0xa20f0208
        MOVS     R0,#+29
        STR      R0,[R4, #+0]
//  318             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  319             *SPM_CONN_PWR_CON = 0x1F;
        MOVS     R0,#+31
        STR      R0,[R4, #+0]
//  320             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  321             *SPM_CONN_PWR_CON = 0x1E;
        MOVS     R0,#+30
        STR      R0,[R4, #+0]
//  322             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  323             *SPM_CONN_PWR_CON = 0x1A;
        MOVS     R0,#+26
        STR      R0,[R4, #+0]
//  324             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  325             *SPM_CONN_PWR_CON = 0x12;
        MOVS     R0,#+18
        STR      R0,[R4, #+0]
//  326             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  327             hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
        POP      {R4,LR}
          CFI R4 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall hal_clock_disable
        B.W      hal_clock_disable
          CFI R4 Frame(CFA, -8)
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  328         } else {
//  329             if ((*SPM_PWR_STATUS & 0x4) == 0) {
??spm_control_mtcmos_internal_1:
        LDR      R1,[R0, #+0]
        LSLS     R1,R1,#+29
        BMI.W    ??spm_control_mtcmos_internal_2
//  330                 if ((*SPM_PWR_STATUS & 0x4) != 0) {
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+29
        BMI.W    ??spm_control_mtcmos_internal_2
//  331                     return;
//  332                 }
//  333                 /* E1 sw WK,E2 need remove */
//  334                 pmu_set_register_value(PMU_DIG_VCORE_ANA_CON3, PMU_RG_BUCK_ZXPDN_MASK, PMU_RG_BUCK_ZXPDN_SHIFT, 1); //disable PMU FPWM
        MOVS     R3,#+1
        MOVS     R2,#+4
        MOV      R1,R3
        LDR.N    R0,??DataTable15_17  ;; 0xa207010c
          CFI FunCall pmu_set_register_value
        BL       pmu_set_register_value
//  335 
//  336                 hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
          CFI FunCall hal_clock_enable
        BL       hal_clock_enable
//  337                 *SPM_RESOURCE_CONTROL_0 |= 0x00010000;   //Force on CONN MTCMOS
        LDR.N    R0,??DataTable15_8  ;; 0xa20f03ac
        LDR      R1,[R0, #+0]
        ORR      R1,R1,#0x10000
        STR      R1,[R0, #+0]
//  338                 //CONN MTCMOS ON
//  339                 *SPM_CONN_PWR_CON = 0x16;
        LDR.N    R4,??DataTable15_18  ;; 0xa20f0208
        MOVS     R0,#+22
        STR      R0,[R4, #+0]
//  340                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  341                 *SPM_CONN_PWR_CON = 0x1E;
        MOVS     R0,#+30
        STR      R0,[R4, #+0]
//  342                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  343                 *SPM_CONN_PWR_CON = 0x0E;
        MOVS     R0,#+14
        STR      R0,[R4, #+0]
//  344                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  345                 *SPM_CONN_PWR_CON = 0x0C;
        MOVS     R0,#+12
        STR      R0,[R4, #+0]
//  346                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  347                 *SPM_CONN_PWR_CON = 0x1C;
        MOVS     R0,#+28
        STR      R0,[R4, #+0]
//  348                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  349                 *SPM_CONN_PWR_CON = 0x1D;
        MOVS     R0,#+29
        STR      R0,[R4, #+0]
//  350                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  351                 *SPM_CONN_PWR_CON = 0x0D;
        MOVS     R0,#+13
        STR      R0,[R4, #+0]
//  352                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  353                 hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
        POP      {R4,LR}
          CFI R4 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall hal_clock_disable
        B.W      hal_clock_disable
          CFI R4 Frame(CFA, -8)
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  354             }
//  355         }
//  356         return;
//  357     }
//  358 
//  359     if (mtcmos == SPM_MTCMOS_SDIO_SLV) {
??spm_control_mtcmos_internal_0:
        CMP      R0,#+1
        BNE.W    ??spm_control_mtcmos_internal_2
//  360         if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
        LDR.N    R0,??DataTable15_16  ;; 0xa20f0808
        CMP      R1,#+0
        BNE.N    ??spm_control_mtcmos_internal_3
//  361             if ((*SPM_PWR_STATUS & 0x8) == 0) {
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+28
        BPL.W    ??spm_control_mtcmos_internal_2
//  362                 return;
//  363             }
//  364             hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
          CFI FunCall hal_clock_enable
        BL       hal_clock_enable
//  365             *SPM_RESOURCE_CONTROL_0 &= 0xFEFFFFFF;   //Force off SDIO MTCMOS
        LDR.N    R0,??DataTable15_8  ;; 0xa20f03ac
        LDR      R1,[R0, #+0]
        BIC      R1,R1,#0x1000000
        STR      R1,[R0, #+0]
//  366             //SDIO_SLV MTCMOS OFF
//  367             *SPM_SDIO_SLV_PWR_CON = 0x1D;
        LDR.N    R4,??DataTable15_18  ;; 0xa20f0208
        MOVS     R0,#+29
        STR      R0,[R4, #+4]
//  368             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  369             *SPM_SDIO_SLV_PWR_CON = 0x1F;
        MOVS     R0,#+31
        STR      R0,[R4, #+4]
//  370             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  371 
//  372             *SPM_SDIO_SLV_SRAM_PD |= (1 << 16); /* SDIO_SLV_SRAM_MEM_ISO_EN bit : 16 */
        LDR      R0,[R4, #+88]
        ORR      R0,R0,#0x10000
        STR      R0,[R4, #+88]
//  373             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  374             *SPM_SDIO_SLV_SRAM_PD |= (1 << 0);  /* SDIO_SLV_SRAM_PD bit : 0 */
        LDR      R0,[R4, #+88]
        ORR      R0,R0,#0x1
        STR      R0,[R4, #+88]
//  375             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  376 
//  377             *SPM_SDIO_SLV_PWR_CON = 0x1E;
        MOVS     R0,#+30
        STR      R0,[R4, #+4]
//  378             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  379             *SPM_SDIO_SLV_PWR_CON = 0x1A;
        MOVS     R0,#+26
        STR      R0,[R4, #+4]
//  380             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  381             *SPM_SDIO_SLV_PWR_CON = 0x12;
        MOVS     R0,#+18
        STR      R0,[R4, #+4]
//  382             hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  383             hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
        POP      {R4,LR}
          CFI R4 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall hal_clock_disable
        B.W      hal_clock_disable
          CFI R4 Frame(CFA, -8)
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  384         } else {
//  385             if ((*SPM_PWR_STATUS & 0x8) == 0) {
??spm_control_mtcmos_internal_3:
        LDR      R1,[R0, #+0]
        LSLS     R1,R1,#+28
        BMI.N    ??spm_control_mtcmos_internal_2
//  386                 if ((*SPM_PWR_STATUS & 0x8) != 0) {
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+28
        BMI.N    ??spm_control_mtcmos_internal_2
//  387                     return;
//  388                 }
//  389                 hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
          CFI FunCall hal_clock_enable
        BL       hal_clock_enable
//  390                 *SPM_RESOURCE_CONTROL_0 |= 0x01000000;   //Force on SDIO MTCMOS
        LDR.N    R0,??DataTable15_8  ;; 0xa20f03ac
        LDR      R1,[R0, #+0]
        ORR      R1,R1,#0x1000000
        STR      R1,[R0, #+0]
//  391                 //SDIO_SLV MTCMOS ON
//  392                 *SPM_SDIO_SLV_PWR_CON = 0x12;
        LDR.N    R4,??DataTable15_18  ;; 0xa20f0208
        MOVS     R0,#+18
        STR      R0,[R4, #+4]
//  393                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  394                 *SPM_SDIO_SLV_PWR_CON = 0x16;
        MOVS     R0,#+22
        STR      R0,[R4, #+4]
//  395                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  396                 *SPM_SDIO_SLV_PWR_CON = 0x1E;
        MOVS     R0,#+30
        STR      R0,[R4, #+4]
//  397                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  398 
//  399                 *SPM_SDIO_SLV_SRAM_PD &= (0xFFFEFFFF);   /* SDIO_SLV_SRAM_MEM_ISO_EN bit : 16 */
        LDR      R0,[R4, #+88]
        BIC      R0,R0,#0x10000
        STR      R0,[R4, #+88]
//  400                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  401                 *SPM_SDIO_SLV_SRAM_PD &= (0xFFFFFFFE);    /* SDIO_SLV_SRAM_PD bit : 0 */
        LDR      R0,[R4, #+88]
        LSRS     R0,R0,#+1
        LSLS     R0,R0,#+1
        STR      R0,[R4, #+88]
//  402                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  403 
//  404                 *SPM_SDIO_SLV_PWR_CON = 0x0E;
        MOVS     R0,#+14
        STR      R0,[R4, #+4]
//  405                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  406                 *SPM_SDIO_SLV_PWR_CON = 0x0C;
        MOVS     R0,#+12
        STR      R0,[R4, #+4]
//  407                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  408                 *SPM_SDIO_SLV_PWR_CON = 0x1C;
        MOVS     R0,#+28
        STR      R0,[R4, #+4]
//  409                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  410                 *SPM_SDIO_SLV_PWR_CON = 0x1D;
        MOVS     R0,#+29
        STR      R0,[R4, #+4]
//  411                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  412                 *SPM_SDIO_SLV_PWR_CON = 0x0D;
        MOVS     R0,#+13
        STR      R0,[R4, #+4]
//  413                 hal_gpt_delay_us(1);
        MOVS     R0,#+1
          CFI FunCall hal_gpt_delay_us
        BL       hal_gpt_delay_us
//  414                 hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        MOVS     R0,#+48
        POP      {R4,LR}
          CFI R4 SameValue
          CFI R14 SameValue
          CFI CFA R13+0
          CFI FunCall hal_clock_disable
        B.W      hal_clock_disable
          CFI R4 Frame(CFA, -8)
          CFI R14 Frame(CFA, -4)
          CFI CFA R13+8
//  415             }
//  416         }
//  417         return;
//  418     }
//  419 }
??spm_control_mtcmos_internal_2:
        POP      {R4,PC}          ;; return
          CFI EndBlock cfiBlock3
//  420 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock4 Using cfiCommon0
          CFI Function manual_spm_write_im
          CFI NoCalls
        THUMB
//  421 void manual_spm_write_im(uint32_t addr, uint32_t data)
//  422 {
//  423     *SPM_PCM_IM_HOST_RW_PTR = 0x01010000 | addr;
manual_spm_write_im:
        LDR.N    R2,??DataTable15_19  ;; 0xa20f0330
        LDR.N    R3,??DataTable15_20  ;; 0x1010000
        ORRS     R0,R3,R0
        STR      R0,[R2, #+0]
//  424     *SPM_PCM_IM_HOST_RW_DAT = data;
        STR      R1,[R2, #+4]
//  425     return;
        BX       LR               ;; return
//  426 }
          CFI EndBlock cfiBlock4
//  427 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock5 Using cfiCommon0
          CFI Function manual_spm_read_im
          CFI NoCalls
        THUMB
//  428 uint32_t manual_spm_read_im(uint32_t addr)
//  429 {
//  430     uint32_t data;
//  431     *SPM_PCM_IM_HOST_RW_PTR = 0x01000000 | addr;
manual_spm_read_im:
        LDR.N    R1,??DataTable15_19  ;; 0xa20f0330
        ORR      R0,R0,#0x1000000
        STR      R0,[R1, #+0]
//  432     data = *SPM_PCM_IM_HOST_RW_DAT;
        LDR      R0,[R1, #+4]
//  433     *SPM_PCM_IM_HOST_RW_PTR = 0;
        MOVS     R2,#+0
        STR      R2,[R1, #+0]
//  434     return (data);
        BX       LR               ;; return
//  435 }
          CFI EndBlock cfiBlock5
//  436 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock6 Using cfiCommon0
          CFI Function spm_mask_wakeup_source
          CFI NoCalls
        THUMB
//  437 void spm_mask_wakeup_source(uint32_t wakeup_source)
//  438 {
//  439     if(wakeup_source == HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL) {
spm_mask_wakeup_source:
        LDR.N    R1,??DataTable15_7  ;; 0xa20f0000
        CMP      R0,#+8
        BNE.N    ??spm_mask_wakeup_source_0
//  440         *SPM_WAKEUP_SOURCE_MASK = 0xFF;
        MOVS     R0,#+255
        STR      R0,[R1, #+0]
        BX       LR
//  441     }else {
//  442         *SPM_WAKEUP_SOURCE_MASK |= (1<<wakeup_source);
??spm_mask_wakeup_source_0:
        LDR      R2,[R1, #+0]
        MOVS     R3,#+1
        LSL      R0,R3,R0
        ORRS     R0,R0,R2
        STR      R0,[R1, #+0]
//  443     }
//  444 }
        BX       LR               ;; return
          CFI EndBlock cfiBlock6
//  445 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock7 Using cfiCommon0
          CFI Function spm_unmask_wakeup_source
          CFI NoCalls
        THUMB
//  446 void spm_unmask_wakeup_source(uint32_t wakeup_source)
//  447 {
//  448     if(wakeup_source == HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL) {
spm_unmask_wakeup_source:
        LDR.N    R1,??DataTable15_7  ;; 0xa20f0000
        CMP      R0,#+8
        BNE.N    ??spm_unmask_wakeup_source_0
//  449         *SPM_WAKEUP_SOURCE_MASK = 0x00;
        MOVS     R0,#+0
        STR      R0,[R1, #+0]
        BX       LR
//  450     }else {
//  451         *SPM_WAKEUP_SOURCE_MASK &= ~(1<<wakeup_source);
??spm_unmask_wakeup_source_0:
        LDR      R2,[R1, #+0]
        MOVS     R3,#+1
        LSL      R0,R3,R0
        BIC      R0,R2,R0
        STR      R0,[R1, #+0]
//  452     }       
//  453 }
        BX       LR               ;; return
          CFI EndBlock cfiBlock7

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15:
        DC32     0xa20f0300

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_1:
        DC32     0xb160010

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_2:
        DC32     0xb160000

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_3:
        DC32     0x140032

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_4:
        DC32     0x460033

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_5:
        DC32     0x5a0034

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_6:
        DC32     0x680035

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_7:
        DC32     0xa20f0000

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_8:
        DC32     0xa20f03ac

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_9:
        DC32     0xb160001

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_10:
        DC32     0xb160002

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_11:
        DC32     0xb163c49

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_12:
        DC32     0xa20f09e4

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_13:
        DC32     0xa20f0104

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_14:
        DC32     0xa2010040

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_15:
        DC32     ??mtcmos_conn_resource_cnt

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_16:
        DC32     0xa20f0808

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_17:
        DC32     0xa207010c

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_18:
        DC32     0xa20f0208

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_19:
        DC32     0xa20f0330

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable15_20:
        DC32     0x1010000

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
// __absolute char const spm_init::__FUNCTION__[9]
??__FUNCTION__:
        DC8 "spm_init"
        DC8 0, 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_0:
        DC8 "\015\012 [SPM] Clock enable failed!"
        DC8 0, 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_1:
        DC8 "spm_auto_load\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_2:
        DC8 "manual load spm code\015\012"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_3:
        DC8 "SPM Code loading Success\015\012"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_4:
        DC8 "SPM Code loading Fail\015\012"

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
?_5:
        DC8 "\015\012 [SPM] Clock disable failed!"
        DC8 0

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
spm_im:
        DC32 420479007, 2718894096, 2164261892, 424673311, 2718894096
        DC32 2168591361, 2705330181, 3625976229, 398490655, 452984863
        DC32 1073741825, 4026531840, 398490655, 415236127, 2718894096
        DC32 3772776464, 452984863, 2147483649, 4026531840, 398490655
        DC32 415236127, 2718892968, 420479007, 2718892968, 2298478596
        DC32 4294967039, 3770679300, 415236127, 2719940624, 419430431, 65537
        DC32 3770679300, 415236127, 2718892736, 3772776449, 416284703
        DC32 2719940624, 2169244673, 428867615, 2718892736, 2172917761
        DC32 420479007, 2718892972, 2164789249, 2711621638, 2164267013
        DC32 3625976932, 398490655, 2295332865, 2684682240, 3625977347
        DC32 398490655, 415236127, 2718894096, 3772776452, 415236127
        DC32 2718892548, 423624735, 1, 420479007, 2718892972, 2164527105
        DC32 3370130468, 398490655, 2685502464, 2685469696, 452984863, 69869569
        DC32 4026531840, 398490655, 415236127, 2718894096, 3772776456
        DC32 415236127, 2718892968, 420479007, 2718892968, 2298478596
        DC32 4294967038, 3770679300, 452984863, 68296705, 3894444032
        DC32 2718892800, 186056460, 3894444032, 2718892800, 186056456
        DC32 4026531840, 398490655, 415236127, 2718894096, 3772776452
        DC32 415236127, 2718892968, 420479007, 2718892968, 2835349508, 1
        DC32 3770679300, 452984863, 69869569, 4026531840, 398490655, 415236127
        DC32 2718894096, 3772776450, 415236127, 2718892548, 423624735, 1
        DC32 420479007, 2718894088, 2164297729, 3370127812, 398490655
        DC32 415236127, 2719940624, 419430431, 1, 3770679300, 415236127
        DC32 2718892736, 3772776448, 2150794240, 461373471, 536880543
        DC32 2150695936, 461373471, 536874202, 2150728704, 415236127
        DC32 2718892968, 420479007, 2718892968, 2835349508, 257, 3770679300
        DC32 452984863, 67510273, 4026531840, 398490655, 3772776470, 461373471
        DC32 536870913, 3772776478, 461373471, 536870913, 2172654593
        DC32 3623883750, 398490655, 2172687361, 3623884422, 398490655
        DC32 2172720129, 3623885382, 398490655, 3489667840, 398490655
        DC32 419430431, 2718892628, 427819039, 65793, 3774873606, 461373471
        DC32 536870926, 427819039, 257, 3774873606, 461373471, 536870926
        DC32 3776971008, 419430431, 2718892720, 427819039, 65792, 3774873606
        DC32 3776971008, 3489667840, 398490655, 419430431, 2718892592
        DC32 3776970815, 3776970783, 3776970767, 3776970759, 3776970755
        DC32 3776970753, 3776970752, 461373471, 536871202, 419430431
        DC32 2718892580, 427819039, 65793, 3774873606, 461373471, 536870926
        DC32 427819039, 257, 3774873606, 461373471, 536870926, 3776971008
        DC32 419430431, 2718892596, 3776971008, 3776970752, 3489667840
        DC32 398490655, 419430431, 2718892640, 427819039, 65792, 3774873606
        DC32 3776971008, 3772776462, 3772776460, 3772776476, 461373471
        DC32 536870916, 3772776477, 3772776461, 4026531840, 398490655
        DC32 3772776477, 3772776479, 2172654593, 3623886278, 398490655
        DC32 2172687361, 3623886982, 398490655, 2172720129, 3623887654
        DC32 398490655, 3489670176, 398490655, 419430431, 2718892628
        DC32 3776971009, 427819039, 65793, 3774873606, 461373471, 536870926
        DC32 427819039, 65537, 3774873606, 461373471, 536870926, 419430431
        DC32 2718892720, 427819039, 65792, 3774873606, 427819039, 65793
        DC32 3774873606, 3489670176, 419430431, 2718892592, 3776970879
        DC32 419430431, 2718892580, 3776971009, 427819039, 65793, 3774873606
        DC32 461373471, 536870926, 427819039, 65537, 3774873606, 461373471
        DC32 536870926, 419430431, 2718892596, 3776971008, 3776971023
        DC32 3489670176, 419430431, 2718892640, 427819039, 65792, 3774873606
        DC32 427819039, 65793, 3774873606, 3772776478, 3772776474, 3772776466
        DC32 4026531840, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 398490655, 398490655
        DC32 398490655, 398490655, 398490655, 398490655, 406847519, 1
        DC32 415236127, 2718894096, 3772776449, 2685404160, 2714764295
        DC32 415236127, 2718892928, 3773857791, 452984863, 67313665, 2180056071
        DC32 461373471, 3490709505, 2165256193, 3623898052, 398490655
        DC32 3894444032, 2718892800, 186056456, 415236127, 2718894096
        DC32 3772776450, 415236127, 2719940624, 3772776449, 416284703
        DC32 2719940624, 2164526081, 3625993060, 398490655, 415236127
        DC32 2718892544, 423624735, 2, 3233815584, 398490655, 415236127
        DC32 3221684544, 419430431, 2425375725, 3770679300, 2685535232
        DC32 452984863, 67510273, 461373471, 2416967681, 461373471, 536880543
        DC32 2150695936, 461373471, 536874202, 2150728704, 2150761472
        DC32 2150794240, 420479007, 2718894088, 2164297729, 415236127
        DC32 2718892548, 423624735, 1, 3370127812, 398490655, 415236127
        DC32 2718892968, 420479007, 2718892968, 2835349508, 257, 3770679300
        DC32 415236127, 2718892544, 423624735, 2, 3233812928, 398490655
        DC32 415236127, 2719940624, 419430431, 0, 3770679300, 415236127
        DC32 2718892736, 3772776448, 415236127, 3221684544, 419430431
        DC32 4276195341, 3770679300, 3894444032, 2718892800, 186056456
        DC32 415236127, 2718894096, 3772776448, 2179990535, 2714829831
        DC32 4026531840

        SECTION `.iar_vfe_header`:DATA:NOALLOC:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
        DC32 0

        SECTION __DLIB_PERTHREAD:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        SECTION __DLIB_PERTHREAD_init:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        END
//  454 
//  455 #endif
// 
//     8 bytes in section .bss
//    32 bytes in section .rodata
//   120 bytes in section .tcm_code
// 3 852 bytes in section .text
// 
// 3 972 bytes of CODE  memory
//    32 bytes of CONST memory
//     8 bytes of DATA  memory
//
//Errors: none
//Warnings: 1
