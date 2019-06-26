;/* Copyright Statement:
; *
; * (C) 2005-2016  MediaTek Inc. All rights reserved.
; *
; * This software/firmware and related documentation ("MediaTek Software") are
; * protected under relevant copyright laws. The information contained herein
; * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
; * Without the prior written permission of MediaTek and/or its licensors,
; * any reproduction, modification, use or disclosure of MediaTek Software,
; * and information contained herein, in whole or in part, shall be strictly prohibited.
; * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
; * if you have agreed to and been bound by the applicable license agreement with
; * MediaTek ("License Agreement") and been granted explicit permission to do so within
; * the License Agreement ("Permitted User").  If you are not a Permitted User,
; * please cease any access or use of MediaTek Software immediately.
; * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
; * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
; * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
; * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
; * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
; * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
; * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
; * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
; * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
; * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
; * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
; * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
; * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
; * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
; * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
; * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
; * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
; */

.file "startup_bootloader.s"
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.equ   CACHE_CON,         0xE0180000
.equ   CACHE_OP,          0xE0180004
.equ   CACHE_REGION_EN,   0xE018002C
.equ   CACHE_ENTRY_0,     0xE0190000
.equ   CACHE_END_ENTRY_0, 0xE0190040

.equ   ZERO,          0x0
.equ   CACHE_FLUSH_ALL_LINES,      0x13
.equ   CACHE_INVALIDATE_ALL_LINES, 0x3

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss


.globl _start
_start:
  b Reset_Handler


/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

.section  .reset_handler
.weak  Reset_Handler
.type  Reset_Handler, %function
Reset_Handler:
  ldr  sp, =_estack    		 /* set stack pointer */

/* Enable cache sequence */
  /* hal_cache_invalidate_all_cache_lines */
  ldr  r0, =CACHE_OP
  ldr  r1, =ZERO
  str  r1, [r0, #0]
  ldr  r0, =CACHE_OP
  ldr  r1, =CACHE_FLUSH_ALL_LINES
  str  r1, [r0, #0]
  ldr  r0, =CACHE_OP
  ldr  r1, =ZERO
  str  r1, [r0, #0]
  ldr  r0, =CACHE_OP
  ldr  r1, =CACHE_INVALIDATE_ALL_LINES
  str  r1, [r0, #0]
  ISB

  /* CACHE->CACHE_CON = 0; */
  ldr  r0, =CACHE_CON
  ldr  r1, =ZERO
  str  r1, [r0, #0]

  /* CACHE->CACHE_REGION_EN = 0;*/
  ldr  r0, =CACHE_REGION_EN
  ldr  r1, =ZERO
  str  r1, [r0, #0]

  /* hal_cache_set_size */
  ldr  r0, =CACHE_CON
  ldr  r1, =0x200
  str  r1, [r0, #0]

  /* hal_cache_region_config */
  ldr  r0, =CACHE_END_ENTRY_0
  ldr  r1, =0x800E000
  str  r1, [r0, #0]
  ldr  r0, =CACHE_ENTRY_0
  ldr  r1, =0x8002000
  str  r1, [r0, #0]
  ldr  r0, =CACHE_ENTRY_0
  ldr  r1, =0x8002100
  str  r1, [r0, #0]

  /* hal_cache_region_enable */
  ldr  r0, =CACHE_REGION_EN
  ldr  r1, =0x1
  str  r1, [r0, #0]

  /* hal_cache_enable */
  ldr  r0, =CACHE_CON
  ldr  r1, [r0, #0]
  ORR  r1, #0x0D
  str  r1, [r0, #0]

/* Copy the data segment initializers from flash to TCM */
  ldr  r1, =_sidata
  ldr  r2, =_sdata
  ldr  r3, =_edata

CopyTCMDataLoop:
  cmp     r2, r3
  ittt    lo
  ldrlo   r0, [r1], #4
  strlo   r0, [r2], #4
  blo     CopyTCMDataLoop

/* Zero initialize in SYSRAM */
  ldr  r2, =_sbss
  ldr  r3, =_ebss

ZeroSYSRAMLoop:
  cmp     r2, r3
  ittt    lo
  movlo   r0, #0
  strlo   r0, [r2], #4
  blo     ZeroSYSRAMLoop

/* Zero initialize in TCM */
  ldr  r2, =_tcm_zi_start
  ldr  r3, =_tcm_zi_end

ZeroTCMLoop:
  cmp     r2, r3
  ittt    lo
  movlo   r0, #0
  strlo   r0, [r2], #4
  blo     ZeroTCMLoop

/* disable interrupt */
//TODO how to check interrupt disabled , check PRIMASK = 1
//TODO reference : http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/CHDBIBGJ.html
  CPSID  I

/* Call the bootloader main function.*/
  bl  main


end_bootloader:
  //bl bootloader_error_hanbler
  bl end_bootloader

/*utility function*/
.globl JumpCmd
JumpCmd:
  ORR  r0, #0x01
  BX  r0

/* vector table */
.section  .isr_vector,"a",%progbits
.type  g_pfnVectors, %object
.size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
