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
 
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "memory_attribute.h"
#include "hal_gpio.h"


/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware(void);

int __io_putchar(int ch)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

static void SystemClock_Config(void)
{
    hal_clock_init();
}


/**
* @brief       This function is to do cache initialization.
* @param[in]   cache_size: the size of cache user wants to config.
*              #HAL_CACHE_SIZE_32KB, config cache size as 32KB.\n
*              #HAL_CACHE_SIZE_16KB, config cache size as 16KB.\n
*              #HAL_CACHE_SIZE_8KB, config cache size as 8KB.\n
* @return      #0, if OK.\n
*              #-1, if user's parameter is invalid.\n
*/

#if defined(HAL_CACHE_MODULE_ENABLED)
static int32_t cache_enable(hal_cache_size_t cache_size)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */
        /* UBIN length */
#ifdef CM4_UBIN_LENGTH
        {CM4_BASE, CM4_LENGTH + CM4_UBIN_LENGTH},
#else
        {CM4_BASE, CM4_LENGTH},
#endif
        /* virtual memory */
        {VRAM_BASE, VRAM_LENGTH},
        /* virtual sysram */
        {VSYSRAM_BASE, VSYSRAM_LENGTH}
    };

    region_number = (hal_cache_region_t)(sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));
    if (region_number > HAL_CACHE_REGION_MAX) {
        return -1;
    }

    hal_cache_init();

    /* Set the cache size to 32KB. It will ocuppy the TCM memory size */
    hal_cache_set_size(cache_size);
    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }
    for (; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }
    hal_cache_enable();
    return 0;
}
#endif

static void prvSetupHardware(void)
{
    hal_uart_config_t uart_config;

    /* COM port settings */
    hal_gpio_init(HAL_GPIO_21);
    hal_gpio_init(HAL_GPIO_22);
    hal_pinmux_set_function(HAL_GPIO_21, HAL_GPIO_21_URXD0);
    hal_pinmux_set_function(HAL_GPIO_22, HAL_GPIO_22_UTXD0);
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);

    //io_def_uart_init();
#if defined(HAL_CACHE_MODULE_ENABLED)
    if (cache_enable(HAL_CACHE_SIZE_32KB) < 0) {
        printf("cache enable failed \r\n");
    }
#endif
    hal_flash_init();
    hal_nvic_init();
#ifdef HAL_PMU_MODULE_ENABLED
    hal_pmu_init();
#endif
}

extern unsigned int sdio_slave_test(void);
int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    printf("\r\n\r\n");/* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
    printf("pls add your own code from here\r\n");
    printf("\r\n\r\n");

    /* Add your application code here. */
    if (sdio_slave_test() == 0) {
        /* This is for hal_examples auto-test */
        printf("example project test success.\n");
    }else {
        printf("example project test fail.\n");
    }

    while (1);
}

