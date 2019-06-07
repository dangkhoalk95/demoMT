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


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "hal_rtc.h"
#include "hal_eint.h"
#include "hal_i2c_master.h"
#include "hal_spi_master.h"
#include "hal_gpio.h"
#include "hal_sys_topsm.h"
#include "hal_dsp_topsm.h"

#include "hal_pdma_internal.h"

#include "lcd_manager.h"
#include "task_def.h"
#include "hal_charger.h"
#include "watch.h"
#include "app_manager.h"
#include "app_task.h"
#include "key_manager.h"
#include "app_status_bar.h"

TimerHandle_t vMainTimer;
void vMainTimerCallback(TimerHandle_t xTimer)
{
    tEvent outEvent;

    outEvent.event = EVEINT_1MIN;
    xQueueSend(queAppMgr, &outEvent, 0);
}

void peripheral_init(void)
{
    /* 1. Config SPI3 For SPI Nand */
    hal_pinmux_set_function(HAL_GPIO_32, 9);
    hal_pinmux_set_function(HAL_GPIO_33, 9);
    hal_pinmux_set_function(HAL_GPIO_34, 9);
    hal_pinmux_set_function(HAL_GPIO_35, 9);
    /*
        hal_spi_master_config_t spi_config;
        spi_config.clock_frequency = 0x400000;
        spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
        spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
        spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
        spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;

        mems_spi_init(SPI_SELECT_NAND, HAL_SPI_MASTER_3, &spi_config, HAL_SPI_MASTER_MACRO_GROUP_B);
    */
    /* 2. Config I2C0 */
    hal_pinmux_set_function(HAL_GPIO_36, 1);
    hal_pinmux_set_function(HAL_GPIO_37, 1);
#if 0
    DMA_Init();
    DMA_Vfifo_init();
#endif

    //mems_i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_400K);

    /* 3. Config SPI For LCD */
    *((volatile uint32_t *)(0xA2020824)) |= 0xFFF000;

    hal_pinmux_set_function(HAL_GPIO_38, 1);		// LSRSTB
    hal_pinmux_set_function(HAL_GPIO_39, 1);		// LSCE_B0
    hal_pinmux_set_function(HAL_GPIO_40, 1);		// LSCK0
    hal_pinmux_set_function(HAL_GPIO_41, 1);		// LSA0
    hal_pinmux_set_function(HAL_GPIO_42, 1);		// LSA0DA0
    hal_pinmux_set_function(HAL_GPIO_43, 1);		// LPTE
}

void vGTMain_Task(void *pvParameters)
{
    //peripheral_init();

    lcd_manager_init();
    queAppMgr = xQueueCreate(5, sizeof(tEvent));

    xTaskCreate(appMgr_Task, APPMGR_TASK_NAME, APPMGR_TASK_STACK_SIZE / sizeof(StackType_t), (void *)0, APPMGR_TASK_PRIO, NULL);
    xTaskCreate(appStatusBar_Task, APP_STATUSBAR_TASK_NAME, APP_STATUSBAR_TASK_STACK_SIZE / sizeof(StackType_t), (void *)0, APP_STATUSBAR_TASK_PRIO, NULL);

    keypad_task_init();

    vMainTimer = xTimerCreate("vMainTimer",            // Just a text name, not used by the kernel.
                              (603000 / portTICK_PERIOD_MS),  // The timer period in ticks.
                              pdTRUE,                    // The timer is a one-shot timer.
                              0,                          // The id is not used by the callback so can take any value.
                              vMainTimerCallback     // The callback function that switches the LCD back-light off.
                             );
    xTimerStart(vMainTimer, 0);

    vTaskSuspend(NULL);
    vTaskDelete(NULL);
}


