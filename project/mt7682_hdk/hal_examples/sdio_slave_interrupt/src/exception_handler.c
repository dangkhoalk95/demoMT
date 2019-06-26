/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#include "exception_mt7686.h"
#include "mt7686.h"
#include <stdint.h>

void fault_dump(unsigned int sp[])
{
    printf("register dump:\r\n");
    printf("R0=0x%08x, R1=0x%08x, R2=0x%08x, R3=0x%08x\r\n", sp[0], sp[1], sp[2], sp[3]);
    printf("R12=0x%08x, LR=0x%08x, PC=0x%08x, xPSR=0x%08x\r\n", sp[4], sp[5], sp[6], sp[7]);
}

void cfsr_status_report(unsigned int cfsr)
{
    if ((cfsr >> MMFSR_SHIFT) & MMFSR_MASK) {
        printf("Memory fault status\r\n");
    } else if ((cfsr >> BFSR_SHIFT) & BFSR_MASK) {
        printf("Bus fault status\r\n");
    } else if ((cfsr >> UFSR_SHIFT) & UFSR_MASK) {
        printf("Usage fault status\r\n");
    }

}

#if defined(__GNUC__)

#if (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533)
#define __EXHDLR_ATTR__   __attribute__((naked, section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932)
#define __EXHDLR_ATTR__   __attribute__((naked))
#endif

__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "TST lr, #4                    \n"
        "ITE EQ                        \n"
        "MRSEQ r0, MSP                 \n"
        "MRSNE r0, PSP                 \n"
        "B _HardFault_Handler          \n"
    );
}

__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "TST lr, #4                    \n"
        "ITE EQ                        \n"
        "MRSEQ r0, MSP                 \n"
        "MRSNE r0, PSP                 \n"
        "B _MemManage_Handler          \n"
    );
}

__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "TST lr, #4                    \n"
        "ITE EQ                        \n"
        "MRSEQ r0, MSP                 \n"
        "MRSNE r0, PSP                 \n"
        "B _BusFault_Handler           \n"
    );
}

__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "TST lr, #4                    \n"
        "ITE EQ                        \n"
        "MRSEQ r0, MSP                 \n"
        "MRSNE r0, PSP                 \n"
        "B _UsageFault_Handler         \n"
    );
}


/*************************************************************************/
/*************************************************************************/

void _HardFault_Handler(unsigned int sp[])
{
    printf("SCB->HFSR=0x%08x\r\n", (unsigned int)SCB->HFSR);
    printf("Hard fault status\r\n");

    if (((SCB->HFSR) & (1 << 30)) != 0) {
        printf("SCB->CFSR=0x%08x\r\n", (unsigned int)SCB->CFSR);
        cfsr_status_report((unsigned int)SCB->CFSR);
    }

    fault_dump(sp);

    while (1);
}

void _MemManage_Handler(unsigned int sp[])
{
    printf("SCB->CFSR=0x%08x\r\n", (unsigned int)SCB->CFSR);
    cfsr_status_report((unsigned int)SCB->CFSR);
    fault_dump(sp);

    while (1);
}

void _BusFault_Handler(unsigned int sp[])
{
    printf("SCB->CFSR=0x%08x\r\n", (unsigned int)SCB->CFSR);
    cfsr_status_report((unsigned int)SCB->CFSR);
    fault_dump(sp);

    while (1);
}

void _UsageFault_Handler(unsigned int sp[])
{
    printf("SCB->CFSR=0x%08x\r\n", (unsigned int)SCB->CFSR);
    cfsr_status_report((unsigned int)SCB->CFSR);
    fault_dump(sp);

    while (1);
}


#endif /* __GNUC__ */
