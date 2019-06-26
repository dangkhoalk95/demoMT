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
 
#include "mt7686.h"
#include "memory_map.h"


#if defined(__GNUC__)
extern unsigned int Image$$VECTOR$$TABLE$$Base[];
#elif defined(__CC_ARM)
extern unsigned int isr_vector$$Base[];
#elif defined(__ICCARM__)
#pragma section = ".intvec"
#endif


/* ----------------------------------------------------------------------------
   -- Core clock macros
   ---------------------------------------------------------------------------- */
#define CLK_CM4_FREQ_26M      ((uint32_t) 26000000)
#define CLK_CM4_FREQ_96M      ((uint32_t) 96000000)
#define CLK_CM4_FREQ_148M     ((uint32_t)148571428)
#define CLK_CM4_FREQ_192M     ((uint32_t)192000000)

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock;

static volatile uint32_t System_Initialize_Done = 0;



/**
   * @brief systick reload value reloaded via this function.
  *         This function can be called in init stage and system runtime.
  * @param  ticks value to be set
  * @retval 0 means successful
  */
uint32_t SysTick_Set(uint32_t ticks)
{
    uint32_t val;

    /* reload value impossible */
    if ((ticks - 1) > SysTick_LOAD_RELOAD_Msk) {
        return (1);
    }

    /* backup CTRL register */
    val = SysTick->CTRL;

    /* disable sys_tick */
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk |
                       SysTick_CTRL_ENABLE_Msk);

    /* set reload register */
    SysTick->LOAD  = ticks - 1;
    SysTick->VAL   = 0;

    /* restore CTRL register */
    SysTick->CTRL = val;

    return (0);
}


/**
  * @brief System init status set function.
  *         This function can be called in init stage when system initialization is finished.
  * @param  None
  * @retval None
  */
void SysInitStatus_Set(void)
{
    System_Initialize_Done = 1;
}

/**
  * @brief System init status query function.
  *         This function is used to query system init status.
  * @param  None
  * @retval 0 means system is still under initialization.
  * @retval 1 means system initialize done.
  */
uint32_t SysInitStatus_Query(void)
{
    return System_Initialize_Done;
}

/**
   * @brief Update SystemCoreClock variable according to PLL config.
  *         The SystemCoreClock variable stands for core clock (HCLK), which can
  *         be used to setup the SysTick timer or other use.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
    SystemCoreClock = CLK_CM4_FREQ_192M;
}

/**
  * @brief  Setup system
  *         Initialize the FPU setting, vector table location and faults enabling.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
    /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif

    /* Configure the Vector Table location add offset address ------------------*/
#if defined(__GNUC__)
    SCB->VTOR  = (uint32_t)(Image$$VECTOR$$TABLE$$Base);
#elif defined (__CC_ARM)
    SCB->VTOR  = (uint32_t)(isr_vector$$Base);
#elif defined(__ICCARM__)
    SCB->VTOR  = (uint32_t)__section_begin(".intvec");
#endif

    /* enable common faults */
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk |
                  SCB_SHCSR_USGFAULTENA_Msk |
                  SCB_SHCSR_BUSFAULTENA_Msk;
}

/**
  * @brief  CACHE preinit
  *         Init CACHE to accelerate region init progress.
  * @param  None
  * @retval None
  */
void CachePreInit(void)
{
    /* CACHE disable */
    CACHE->CACHE_CON = 0x00;

    /* Flush all cache lines */
    CACHE->CACHE_OP = 0x13;

    /* Invalidate all cache lines */
    CACHE->CACHE_OP = 0x03;

    /* Set cacheable region */
    CACHE->CACHE_ENTRY_N[0] = CM4_BASE | 0x100;
    CACHE->CACHE_END_ENTRY_N[0] = CM4_BASE + CM4_LENGTH;

    CACHE->CACHE_REGION_EN = 1;

    switch (TCM_LENGTH) {
        /* 64K TCM/32K CACHE */
        case 0x00010000:
            CACHE->CACHE_CON = 0x30D;
            break;
        /* 80K TCM/16K CACHE */
        case 0x00014000:
            CACHE->CACHE_CON = 0x20D;
            break;
        /* 88K TCM/8K CACHE */
        case 0x00016000:
            CACHE->CACHE_CON = 0x10D;
            break;
        /* 96K TCM/NO CACHE */
        default:
            break;
    }
}
