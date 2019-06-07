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

#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include <dma_sw.h>
#include <hal_uart.h>

#include "io_def.h"
#include "syslog.h"
#include "memory_attribute.h"
#include "hal_eint.h"
#include "connsys_driver.h"

#if configUSE_TICKLESS_IDLE == 2
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif


#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */


#define VFIFO_TX_SIZE           512
#define VFIFO_RX_SIZE           128
#define VFIFO_RX_THRESHOLD      1
#define VFIFO_ALERT_LENGTH      0
#define UART_RX_TIMEOUT         3000

/* Block UART definition ----------------------------------------------------*/

static bool is_io_ready = false;

static SemaphoreHandle_t    _g_semaphore_tx = NULL;
static SemaphoreHandle_t    _g_semaphore_rx = NULL;

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t              g_tx_vfifo[VFIFO_TX_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t              g_rx_vfifo[VFIFO_RX_SIZE];


/****************************************************************************
 * Forward Declaration
 ****************************************************************************/


static void _uart_event(hal_uart_callback_event_t event, void *user_data);


/****************************************************************************
 * Sleep Functions
 ****************************************************************************/


#ifdef HAL_SLEEP_MANAGER_ENABLED
#if configUSE_TICKLESS_IDLE == 2

#include "hal_eint.h"

extern uint32_t eint_get_status(void);
extern void eint_ack_interrupt(uint32_t eint_number);

static uint8_t uart_rx_eint_sleep_handle = 0xFF;

void console_uart_rx_eint_handler(void *parameter)
{
    _uart_event(HAL_UART_EVENT_READY_TO_READ, NULL);
}

static void io_def_uart_sleep_management_suspend_callback(void *data)
{
    eint_ack_interrupt(CONSOLE_UART_RX_EINT);
    hal_eint_unmask(CONSOLE_UART_RX_EINT);
}

static void io_def_uart_sleep_management_resume_callback(void *data)
{
    if ((eint_get_status() >> CONSOLE_UART_RX_EINT) & 0x01)
    {
        hal_sleep_manager_lock_sleep(uart_rx_eint_sleep_handle);
    }
}
#endif
#endif


void _io_def_uart_sleep_init(void)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
#if configUSE_TICKLESS_IDLE == 2
    hal_eint_config_t config1;
    config1.trigger_mode = HAL_EINT_EDGE_FALLING;
    config1.debounce_time = 0;

    hal_eint_init(CONSOLE_UART_RX_EINT, &config1);    //set EINT trigger mode and debounce time.
    hal_eint_register_callback(CONSOLE_UART_RX_EINT, console_uart_rx_eint_handler, NULL); // register a user callback.
    hal_eint_unmask(CONSOLE_UART_RX_EINT);
            
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_UART_RX_EINT,
                                               io_def_uart_sleep_management_suspend_callback,
                                               (void *)CONSOLE_UART);

    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_UART_RX_EINT,
                                              io_def_uart_sleep_management_resume_callback,
                                              (void *)CONSOLE_UART);
                                              
    uart_rx_eint_sleep_handle = hal_sleep_manager_set_sleep_handle("rx_eint");
#endif
#endif
}


/****************************************************************************
 * Private Functions
 ****************************************************************************/


static void _uart_event(hal_uart_callback_event_t event, void *user_data)
{
    BaseType_t  x_higher_priority_task_woken = pdFALSE;

    switch (event) {
        case HAL_UART_EVENT_READY_TO_READ:
            xSemaphoreGiveFromISR(_g_semaphore_rx, &x_higher_priority_task_woken);
            break;
        case HAL_UART_EVENT_READY_TO_WRITE:
            xSemaphoreGiveFromISR(_g_semaphore_tx, &x_higher_priority_task_woken);
            break;
    }

    /*
     * xSemaphoreGiveFromISR() will set *pxHigherPriorityTaskWoken to pdTRUE
     * if giving _g_semaphore_rx or _g_semaphore_tx caused a task to unblock,
     * and the unblocked task has a priority higher than the currently running
     * task. If xSemaphoreGiveFromISR() sets this value to pdTRUE then a
     * context switch should be requested before the interrupt exits.
     */

    portYIELD_FROM_ISR(x_higher_priority_task_woken);
}

/**
 * Check for conditions that blocking APIs can not be used.
 *
 * 1. Before OS starts.
 * 2. When interrupt is disabled.
 * 3. Currently in an ISR.
 */
static int8_t _uart_dma_blocking_is_safe(void)
{
#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING &&
            __get_PRIMASK() == 0                              &&
            ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos) == 0) {
        return 1;
    }
#endif

    return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/


__weak PUTCHAR_PROTOTYPE
{
    return bsp_io_def_uart_putchar(ch);
}


__weak GETCHAR_PROTOTYPE
{
    return bsp_io_def_uart_getchar();
}


/**
  * @brief  Retargets the C library printf function to the USART.
  *
  * This API correspond to __io_putchar() for many open source C library
  * implementations. Most commonly, it is called by printf series APIs.
  * It could be used before or after OS starts running (scheduler starts).
  *
  * The difference in phases before or after OS starts is the supporting of
  * blocking APIs.
  *
  * The implementation sends to UART using polling API if scheduler is not
  * running. If scheduler is running, it sends to TX VFIFO when there is space
  * in TX VFIFO. When there is no space in VFIFO, it waits for TX ready event
  * (which means there is now some space in TX VFIFO) from DMA engine before
  * continuing the transmission.
  *
  * @param  ch  the character to be sent.
  *
  * @retval None
  */
int bsp_io_def_uart_putchar(int ch)
{
    uint32_t n;

    if (!is_io_ready)
    {
        return 0;
    }

    if (_uart_dma_blocking_is_safe())
    {
        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)&ch, 1);
        } while (!n && xSemaphoreTake(_g_semaphore_tx, 1000/*portMAX_DELAY*/) != pdTRUE);

        if (ch != '\n') {
            return ch;
        }

        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", 1);
        } while (!n && xSemaphoreTake(_g_semaphore_tx, 1000/*portMAX_DELAY*/) != pdTRUE);
    } else {
        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)&ch, 1);
        } while (!n);

        if (ch != '\n')
        {
            return ch;
        }

        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", 1);
        } while (!n);
    }

    return ch;
}


/**
  * @brief  Retargets the C library getchar function to the USART.
  * @param  None
  * @retval None
  */

int bsp_io_def_uart_getchar(void)
{
    /* Blocked UART Getchar */
    while (1)
    {
        uint32_t    len;

        len = hal_uart_get_available_receive_bytes(CONSOLE_UART);

        if (len > 0) {
            uint8_t ret;

            hal_uart_receive_dma(CONSOLE_UART, &ret, 1);

            return ret;
        } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
#if configUSE_TICKLESS_IDLE == 2
            if (xSemaphoreTake(_g_semaphore_rx, UART_RX_TIMEOUT / portTICK_PERIOD_MS /*portMAX_DELAY*/) == pdTRUE)
                continue;
            
            if(hal_sleep_manager_is_sleep_handle_alive(uart_rx_eint_sleep_handle) == true) {
                hal_sleep_manager_unlock_sleep(uart_rx_eint_sleep_handle);
            }            
#endif
#endif
            xSemaphoreTake(_g_semaphore_rx, portMAX_DELAY);
        }
    }
}

int log_write(char *buf, int len)
{
    int left;
    char last_char;

    if (!is_io_ready || len < 1) {
        return 0;
    }

    last_char = *(buf + len - 1);

    if (_uart_dma_blocking_is_safe()) {
        left = len;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)(buf + len - left), left);
        } while (left && xSemaphoreTake(_g_semaphore_tx, 1000));

        if (last_char != '\n') {
            return len;
        }

        left = 1;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", left);
        } while (left && xSemaphoreTake(_g_semaphore_tx, 1000));
    } else {
        left = len;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)(buf + len - left), left);
        } while (left);

        if (last_char != '\n') {
            return len;
        }

        left = 1;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", left);
        } while (left);
    }

    return len;
}


void bsp_io_def_uart_init(void)
{
    hal_uart_dma_config_t   dma_config = {
        .send_vfifo_buffer              = g_tx_vfifo,
        .send_vfifo_buffer_size         = VFIFO_TX_SIZE,
        .send_vfifo_threshold_size      = VFIFO_TX_SIZE / 8,
        .receive_vfifo_buffer           = g_rx_vfifo,
        .receive_vfifo_buffer_size      = VFIFO_RX_SIZE,
        .receive_vfifo_threshold_size   = VFIFO_RX_THRESHOLD,
        .receive_vfifo_alert_size       = VFIFO_ALERT_LENGTH
    };

    log_uart_init(CONSOLE_UART);
    is_io_ready = true;

    /* initialize Semephore */
    _g_semaphore_tx = xSemaphoreCreateBinary();
    _g_semaphore_rx = xSemaphoreCreateBinary();

    hal_uart_set_dma(CONSOLE_UART, &dma_config);

    hal_uart_register_callback(CONSOLE_UART, _uart_event, NULL);
    
    _io_def_uart_sleep_init();
}

