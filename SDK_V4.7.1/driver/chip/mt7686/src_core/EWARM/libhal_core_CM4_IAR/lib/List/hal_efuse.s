///////////////////////////////////////////////////////////////////////////////
//
// IAR ANSI C/C++ Compiler V7.50.1.10123/W32 for ARM      07/Apr/2017  14:04:53
// Copyright 1999-2015 IAR Systems AB.
//
//    Cpu mode     =  thumb
//    Endian       =  little
//    Source file  =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_efuse.c
//    Command line =  
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_efuse.c -D
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
//        W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\EWARM\libhal_core_CM4_IAR\lib\List\hal_efuse.s
//
///////////////////////////////////////////////////////////////////////////////

        RTMODEL "__SystemLibrary", "DLib"
        AAPCS BASE,INTERWORK,VFP
        PRESERVE8
        REQUIRE8

        #define SHT_PROGBITS 0x1

        EXTERN __aeabi_memcpy

        PUBLIC EFUSE_REGISTER
        PUBLIC hal_efuse_read
        PUBLIC hal_efuse_write
        
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
        
// W:\Documents\2523_dev\gva\driver\chip\mt7686\src_core\hal_efuse.c
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
//   35 #include <string.h>
//   36 
//   37 #include "hal_platform.h"
//   38 #include "hal_efuse.h"
//   39 #include "type_def.h"
//   40 
//   41 #define PMU_ELDO (PMU_BASE+0x0118)
//   42 
//   43 #define HAL_EFUSE_WRITE_MAGIC (0xA07923B6)
//   44 
//   45 typedef union {
//   46     struct {
//   47         uint32_t VLD:  1;
//   48         uint32_t rsv0: 7;
//   49         uint32_t BUSY: 1;
//   50         uint32_t rsv1: 7;
//   51         uint32_t RD:   1;
//   52         uint32_t rsv2: 7;
//   53         uint32_t RST:  1;
//   54         uint32_t rsv3: 7;
//   55     } b;
//   56     uint32_t w;
//   57 } EFUSE_CON1_REGISTER_T;
//   58 
//   59 typedef struct {
//   60     __IO EFUSE_CON1_REGISTER_T EFUSEC_CON1;       /* EFUSEC Control1 */
//   61     __IO uint32_t EFUSEC_CON2;       /* EFUSEC Control2 */
//   62     __IO uint32_t EFUSEC_BPKEY;      /* EFUSE Blowing Protection Key */
//   63     __IO uint32_t EFUSEC_PGMT;       /* EFUSE Blowing Time */
//   64 } EFUSE_REGISTER_T;
//   65 

        SECTION `.data`:DATA:REORDER:NOROOT(2)
        DATA
//   66 EFUSE_REGISTER_T *EFUSE_REGISTER = (EFUSE_REGISTER_T *)EFUSE_BASE;
EFUSE_REGISTER:
        DC32 0A20A0000H
//   67 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock0 Using cfiCommon0
          CFI Function hal_efuse_write
        THUMB
//   68 hal_efuse_status_t hal_efuse_write(uint32_t magic, uint16_t addr, const uint8_t *buf, uint8_t len)
//   69 {
hal_efuse_write:
        PUSH     {R3-R9,LR}
          CFI R14 Frame(CFA, -4)
          CFI R9 Frame(CFA, -8)
          CFI R8 Frame(CFA, -12)
          CFI R7 Frame(CFA, -16)
          CFI R6 Frame(CFA, -20)
          CFI R5 Frame(CFA, -24)
          CFI R4 Frame(CFA, -28)
          CFI CFA R13+32
        MOV      R4,R2
        MOV      R5,R3
//   70     uint32_t eldo_en, real_addr, value, i;
//   71 
//   72     if (buf == NULL || len % 4 || addr & 0x3) {
        CMP      R4,#+0
        BEQ.N    ??hal_efuse_write_0
        MOVS     R2,#+3
        TST      R5,R2
        BNE.N    ??hal_efuse_write_0
        TST      R1,R2
        BEQ.N    ??hal_efuse_write_1
//   73         return HAL_EFUSE_INVALID_PARAMETER;
??hal_efuse_write_0:
        MOVS     R0,#+1
        B.N      ??hal_efuse_write_2
//   74     }
//   75 
//   76     if (magic != HAL_EFUSE_WRITE_MAGIC) {
??hal_efuse_write_1:
        LDR.N    R2,??DataTable1  ;; 0xa07923b6
        CMP      R0,R2
        BEQ.N    ??hal_efuse_write_3
//   77         return HAL_EFUSE_INVALID_MAGIC;
        MOVS     R0,#+2
        B.N      ??hal_efuse_write_2
//   78     }
//   79 
//   80     real_addr = EFUSE_BASE + addr;
??hal_efuse_write_3:
        LDR.N    R0,??DataTable1_1  ;; 0xa20a0000
        ADDS     R6,R0,R1
//   81 
//   82     /* ELDO_EN = 1 */
//   83     eldo_en = DRV_Reg32(PMU_ELDO) | 0x1;
        LDR.N    R7,??DataTable1_2  ;; 0xa2070118
        LDR      R0,[R7, #+0]
        ORR      R0,R0,#0x1
//   84     DRV_WriteReg32(PMU_ELDO, eldo_en);
        STR      R0,[R7, #+0]
        LDR.W    R8,??DataTable1_3
        LDR      R0,[R8, #+0]
//   85 
//   86     /* Polling until EFUSE data is valid */
//   87     while(EFUSE_REGISTER->EFUSEC_CON1.b.VLD != 1);
??hal_efuse_write_4:
        LDR      R1,[R0, #+0]
        LSLS     R1,R1,#+31
        BPL.N    ??hal_efuse_write_4
//   88 
//   89     /* Unlock */
//   90     EFUSE_REGISTER->EFUSEC_BPKEY = HAL_EFUSE_WRITE_MAGIC;
        STR      R2,[R0, #+8]
//   91 
//   92     for (i = 0;i < len;i += 4) {
        MOV      R9,#+0
        B.N      ??hal_efuse_write_5
//   93         /* Write EFUSE data */
//   94         memcpy(&value, buf + i, sizeof(uint32_t));
??hal_efuse_write_6:
        MOVS     R2,#+4
        ADD      R1,R9,R4
        MOV      R0,SP
          CFI FunCall __aeabi_memcpy
        BL       __aeabi_memcpy
//   95         DRV_WriteReg32(real_addr + i, value);
        LDR      R0,[SP, #+0]
        STR      R0,[R9, R6]
//   96     }
        ADD      R9,R9,#+4
??hal_efuse_write_5:
        CMP      R9,R5
        LDR      R0,[R8, #+0]
        BCC.N    ??hal_efuse_write_6
//   97 
//   98     /* Wait until not busy */
//   99     while(EFUSE_REGISTER->EFUSEC_CON1.b.BUSY != 0);
??hal_efuse_write_7:
        LDR      R1,[R0, #+0]
        UBFX     R1,R1,#+8,#+1
        CMP      R1,#+0
        BNE.N    ??hal_efuse_write_7
//  100 
//  101     /* Lock */
//  102     EFUSE_REGISTER->EFUSEC_BPKEY = 0;
        MOVS     R1,#+0
        STR      R1,[R0, #+8]
//  103 
//  104     /* Re-initialize */
//  105     EFUSE_REGISTER->EFUSEC_CON1.b.RD = 1;
        LDR      R0,[R8, #+0]
        LDR      R1,[R0, #+0]
        ORR      R1,R1,#0x10000
        STR      R1,[R0, #+0]
//  106 
//  107     /* Polling until EFUSE data is valid */
//  108     while(EFUSE_REGISTER->EFUSEC_CON1.b.VLD != 1);
??hal_efuse_write_8:
        LDR      R0,[R8, #+0]
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+31
        BPL.N    ??hal_efuse_write_8
//  109 
//  110     /* ELDO_EN = 0 */
//  111     eldo_en = DRV_Reg32(PMU_ELDO) & (~0x1);
        LDR      R0,[R7, #+0]
        LSRS     R0,R0,#+1
        LSLS     R0,R0,#+1
//  112     DRV_WriteReg32(PMU_ELDO, eldo_en);
        STR      R0,[R7, #+0]
//  113 
//  114     return HAL_EFUSE_OK;
        MOVS     R0,#+0
??hal_efuse_write_2:
        POP      {R1,R4-R9,PC}    ;; return
//  115 }
          CFI EndBlock cfiBlock0
//  116 

        SECTION `.text`:CODE:NOROOT(1)
          CFI Block cfiBlock1 Using cfiCommon0
          CFI Function hal_efuse_read
        THUMB
//  117 hal_efuse_status_t hal_efuse_read(uint16_t addr, uint8_t *buf, uint8_t len)
//  118 {
hal_efuse_read:
        PUSH     {R4-R8,LR}
          CFI R14 Frame(CFA, -4)
          CFI R8 Frame(CFA, -8)
          CFI R7 Frame(CFA, -12)
          CFI R6 Frame(CFA, -16)
          CFI R5 Frame(CFA, -20)
          CFI R4 Frame(CFA, -24)
          CFI CFA R13+24
        SUB      SP,SP,#+8
          CFI CFA R13+32
        MOV      R4,R1
        MOV      R5,R2
//  119     uint32_t eldo_en, real_addr, value, i;
//  120 
//  121     if (buf == NULL || len % 4 || addr & 0x3) {
        CMP      R4,#+0
        BEQ.N    ??hal_efuse_read_0
        MOVS     R1,#+3
        TST      R5,R1
        BNE.N    ??hal_efuse_read_0
        TST      R0,R1
        BEQ.N    ??hal_efuse_read_1
//  122         return HAL_EFUSE_INVALID_PARAMETER;
??hal_efuse_read_0:
        MOVS     R0,#+1
        B.N      ??hal_efuse_read_2
//  123     }
//  124 
//  125     real_addr = EFUSE_BASE + addr;
??hal_efuse_read_1:
        LDR.N    R1,??DataTable1_1  ;; 0xa20a0000
        ADDS     R7,R1,R0
//  126 
//  127     /* ELDO_EN = 1 */
//  128     eldo_en = DRV_Reg32(PMU_ELDO) | 0x1;
        LDR.N    R6,??DataTable1_2  ;; 0xa2070118
        LDR      R0,[R6, #+0]
        ORR      R0,R0,#0x1
//  129     DRV_WriteReg32(PMU_ELDO, eldo_en);
        STR      R0,[R6, #+0]
//  130 
//  131     /* Polling until EFUSE data is valid */
//  132     while(EFUSE_REGISTER->EFUSEC_CON1.b.VLD != 1);
??hal_efuse_read_3:
        LDR.N    R0,??DataTable1_3
        LDR      R0,[R0, #+0]
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+31
        BPL.N    ??hal_efuse_read_3
//  133 
//  134     for (i = 0;i < len;i += 4) {
        MOV      R8,#+0
        B.N      ??hal_efuse_read_4
//  135         /* Read EFUSE data */
//  136         value = DRV_Reg32(real_addr + i);
??hal_efuse_read_5:
        LDR      R0,[R8, R7]
        STR      R0,[SP, #+0]
//  137         memcpy(buf + i, &value, sizeof(uint32_t));
        MOVS     R2,#+4
        MOV      R1,SP
        ADD      R0,R8,R4
          CFI FunCall __aeabi_memcpy
        BL       __aeabi_memcpy
//  138     }
        ADD      R8,R8,#+4
??hal_efuse_read_4:
        CMP      R8,R5
        BCC.N    ??hal_efuse_read_5
//  139 
//  140     /* ELDO_EN = 0 */
//  141     eldo_en = DRV_Reg32(PMU_ELDO) & (~0x1);
        LDR      R0,[R6, #+0]
        LSRS     R0,R0,#+1
        LSLS     R0,R0,#+1
//  142     DRV_WriteReg32(PMU_ELDO, eldo_en);
        STR      R0,[R6, #+0]
//  143 
//  144     return HAL_EFUSE_OK;
        MOVS     R0,#+0
??hal_efuse_read_2:
        POP      {R1,R2,R4-R8,PC}  ;; return
//  145 }
          CFI EndBlock cfiBlock1

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable1:
        DC32     0xa07923b6

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable1_1:
        DC32     0xa20a0000

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable1_2:
        DC32     0xa2070118

        SECTION `.text`:CODE:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
??DataTable1_3:
        DC32     EFUSE_REGISTER

        SECTION `.iar_vfe_header`:DATA:NOALLOC:NOROOT(2)
        SECTION_TYPE SHT_PROGBITS, 0
        DATA
        DC32 0

        SECTION __DLIB_PERTHREAD:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        SECTION __DLIB_PERTHREAD_init:DATA:REORDER:NOROOT(0)
        SECTION_TYPE SHT_PROGBITS, 0

        END
// 
//   4 bytes in section .data
// 266 bytes in section .text
// 
// 266 bytes of CODE memory
//   4 bytes of DATA memory
//
//Errors: none
//Warnings: none
