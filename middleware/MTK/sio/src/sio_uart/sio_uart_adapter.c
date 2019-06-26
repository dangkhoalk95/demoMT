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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "syslog.h"
#include "nvdm.h"
#include "timers.h"
#include "sio_uart_adapter.h"

//#define SIO_UART_ADAPTER_PRINTF

#ifdef SIO_UART_ADAPTER_PRINTF
#define LOGE(fmt,arg...)   printf(("sio uart adapter: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("sio uart adapter: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("sio uart adapter: "fmt), ##arg)
#else
#define LOGE(fmt,arg...)   //LOG_E(sio, "sio uart adapter: "fmt,##arg)
#define LOGW(fmt,arg...)   //LOG_W(sio, "sio uart adapter: "fmt,##arg)
#define LOGI(fmt,arg...)   //LOG_I(sio ,"sio uart adapter: "fmt,##arg)
#endif

uint32_t sio_uart_queue_create(uint32_t queue_length, uint32_t item_size)
{
    QueueHandle_t q_id = NULL;

    q_id = xQueueCreate(queue_length, item_size);
    return (uint32_t)q_id;
}

sio_uart_ret_t sio_uart_queue_send(uint32_t q_id, void *data)
{
    BaseType_t ret = 0;

    ret = xQueueSend((QueueHandle_t)q_id, data, 0);
    if (pdFAIL != ret) {
        return SIO_UART_RET_OK;
    } else {
        LOGW("sio_uart_queue_send, error \r\n");
        return SIO_UART_RET_OK;
    }
}

sio_uart_ret_t sio_uart_queue_send_from_isr(uint32_t q_id, void *data)
{
    BaseType_t ret = 0;
    BaseType_t xHigherPriorityTaskWoken;

    ret = xQueueSendFromISR((QueueHandle_t)q_id, data, &xHigherPriorityTaskWoken);
    if (pdPASS == ret) {
        return SIO_UART_RET_OK;
    } else {
        LOGW("sio_uart_queue_send_from_isr, error \r\n");
        return SIO_UART_RET_OK;
    }
}

sio_uart_ret_t sio_uart_queue_send_to_front_from_isr(uint32_t q_id, void *data)
{
    BaseType_t ret = 0;
    BaseType_t xHigherPriorityTaskWoken;

    ret = xQueueSendToFrontFromISR((QueueHandle_t)q_id, data, &xHigherPriorityTaskWoken);
    if (pdPASS == ret) {
        return SIO_UART_RET_OK;
    } else {
        LOGW("sio_uart_queue_send_to_front_from_isr, error \r\n");
        return SIO_UART_RET_OK;
    }
}

int32_t sio_uart_queue_receive_no_wait(uint32_t q_id, void *data)
{
    BaseType_t ret = -1;
    ret = xQueueReceive((QueueHandle_t)q_id, data, portMAX_DELAY);
    return (int32_t)ret;
}

int32_t sio_uart_queue_receive_wait(uint32_t q_id, void *data, uint32_t delay_time)
{
    BaseType_t ret = -1;
    ret = xQueueReceive((QueueHandle_t)q_id, data, delay_time / portTICK_PERIOD_MS);
    return (int32_t)ret;
}

uint16_t sio_uart_queue_get_item_num(uint32_t q_id)
{
    uint16_t queue_item_num;
    queue_item_num = (uint16_t)uxQueueMessagesWaiting((QueueHandle_t) q_id);
    return queue_item_num;
}

uint32_t sio_uart_mutex_create(void)
{
    return (uint32_t)xSemaphoreCreateMutex();
}

uint32_t sio_uart_mutex_lock(uint32_t mutex_id)
{
    LOGI("***sio_uart_mutex_lock***\r\n");
    return (uint32_t)xSemaphoreTake((SemaphoreHandle_t)mutex_id, portMAX_DELAY);
}

uint32_t sio_uart_mutex_unlock(uint32_t mutex_id)
{
    LOGI("***sio_uart_mutex_unlock***\r\n");
    return (uint32_t)xSemaphoreGive((SemaphoreHandle_t)mutex_id);
}

void *sio_uart_mem_alloc(uint32_t size)
{
    void *pvReturn = NULL;
    uint32_t  free_size;
    free_size = xPortGetFreeHeapSize();
    if (free_size > size) {
        pvReturn = pvPortCalloc(1, size);
    } else {
        LOGW("sio_uart_mem_alloc, error \r\n");
    }

    return pvReturn;
}

void sio_uart_mem_free(void *buf)
{
    if (buf != NULL) {
        vPortFree(buf);
    } else {
        LOGW("sio_uart_mem_free, error \r\n");
    }
}

uint32_t sio_uart_semaphore_create( uint32_t uxMaxCount, uint32_t uxInitialCount)
{
    return (uint32_t)xSemaphoreCreateCounting((UBaseType_t)uxMaxCount, (UBaseType_t)uxInitialCount);
}

uint32_t sio_uart_semaphore_take(uint32_t semaphore_id)
{
    LOGI("sio_uart_semaphore_take\r\n");
    return (uint32_t)xSemaphoreTake((SemaphoreHandle_t)semaphore_id, portMAX_DELAY);
}

sio_uart_ret_t sio_uart_semaphore_give(uint32_t semaphore_id)
{
    BaseType_t ret = pdFALSE;
    ret = xSemaphoreGive(semaphore_id);

    if (ret == pdTRUE) {
        LOGI("sio_uart_semaphore_give\r\n");
        return SIO_UART_RET_OK;
    } else {
        LOGW("sio_uart_semaphore_give, error \r\n");
        return SIO_UART_RET_OK;
    }
}

sio_uart_ret_t sio_uart_semaphore_give_from_isr(uint32_t semaphore_id)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(semaphore_id, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_timer_create_start(sio_uart_timer_t timer)
{
    TimerHandle_t tmr = NULL;
    sio_uart_timer_status_t status = sio_uart_timer_get_status(timer);

    LOGI("sio_uart_timer_create_start, timer:%d status:%d \r\n", timer, status);
    switch (status) {
        case SIO_UART_TIMER_STATUS_INVALID:
#ifdef SIO_DEBUG
            configASSERT(0);
#endif /* SIO_DEBUG */
            return SIO_UART_RET_ERROR;
        case SIO_UART_TIMER_STATUS_RUNNING:
            LOGW("sio_uart_timer_create_start, timer:%d has been create\r\n", timer);
            return SIO_UART_RET_OK;
    }

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION: {
        #ifdef MODEM_SUPPORT_DTE 	
            tmr = xTimerCreate("modem_exception",
                               sio_uart_timer_get_tick(SIO_UART_TIMER_MODEM_EXCEPTION),
                               pdFALSE,
                               NULL,
                               sio_uart_timer_timeout_callback);

            if (tmr == NULL) {
                LOGE("sio_uart_timer_create_start, modem_exception_timer create fail \r\n");
#ifdef SIO_DEBUG
                configASSERT(0);
#else
                return SIO_UART_RET_ERROR;
#endif
            } else {
                xTimerStart(tmr, 0);
                g_sio_uart_timer_modem_exception_handle = tmr;
            }
#endif
            break;
        }
        case SIO_UART_TIMER_WAIT_RESPONSE: {
            tmr = xTimerCreate("wait_response",
                               15 * 1000 / portTICK_PERIOD_MS, // test value 15 seconds
                               pdFALSE,
                               NULL,
                               sio_uart_timer_timeout_callback);

            if (tmr == NULL) {
                LOGE("sio_uart_timer_create_start, host_sleep_lock_timer create fail \r\n");
#ifdef SIO_DEBUG
                configASSERT(0);
#else
                return SIO_UART_RET_ERROR;
#endif
            } else {
                xTimerStart(tmr, 0);
                g_sio_uart_timer_wait_response_handle = tmr;
            }
            break;
            }
        case SIO_UART_TIMER_WAIT_SLEEP: {
            tmr = xTimerCreate("wait_sleep",
                               5 * 1000 / portTICK_PERIOD_MS, // test value 15 seconds
                               pdFALSE,
                               NULL,
                               sio_uart_timer_timeout_callback);

            if (tmr == NULL) {
                LOGE("sio_uart_timer_create_start, host_sleep_lock_timer create fail \r\n");
#ifdef SIO_DEBUG
                configASSERT(0);
#else
                return SIO_UART_RET_ERROR;
#endif
            } else {
                xTimerStart(tmr, 0);
                g_sio_uart_timer_wait_sleep_handle = tmr;
            }
            break;
            }
        case SIO_UART_TIMER_WAIT_READY: {
            tmr = xTimerCreate("wait_ready",
                               5 * 1000 / portTICK_PERIOD_MS, // test value 15 seconds
                               pdFALSE,
                               NULL,
                               sio_uart_timer_timeout_callback);

            if (tmr == NULL) {
                LOGE("sio_uart_timer_create_start, host_sleep_lock_timer create fail \r\n");
#ifdef SIO_DEBUG
                configASSERT(0);
#else
                return SIO_UART_RET_ERROR;
#endif
            } else {
                xTimerStart(tmr, 0);
                g_sio_uart_timer_wait_ready_handle = tmr;
            }
            break;
            }
        case SIO_UART_TIMER_UT: {
            tmr = xTimerCreate("modem_ut",
                               15 * 1000 / portTICK_PERIOD_MS, // test value 15 seconds
                               pdFALSE,
                               NULL,
                               sio_uart_timer_timeout_callback);

            if (tmr == NULL) {
                LOGE("sio_uart_timer_create_start, ut_timer create fail \r\n");
#ifdef SIO_DEBUG
                configASSERT(0);
#else
                return SIO_UART_RET_ERROR;
#endif
            } else {
                xTimerStart(tmr, 0);
                g_sio_uart_timer_ut_handle = tmr;
            }

            break;
        }
        default:
            return SIO_UART_RET_ERROR;
    }

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_timer_reset(sio_uart_timer_t timer)
{
    sio_uart_timer_status_t status = sio_uart_timer_get_status(timer);

    switch (status) {
        case SIO_UART_TIMER_STATUS_INVALID:
#ifdef SIO_DEBUG
            configASSERT(0);
#endif /* MSM_DEBUG */
            return SIO_UART_RET_ERROR;
        case SIO_UART_TIMER_STATUS_DELETED:
            LOGE("sio_uart_timer_reset, timer:%d has been deleted\r\n", timer);
            return SIO_UART_RET_ERROR;
    }

    LOGI("sio_uart_timer_reset, timer:%d \r\n", timer);

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION:
            xTimerReset(g_sio_uart_timer_modem_exception_handle, 0);
            break;
        case SIO_UART_TIMER_UT:
            xTimerReset(g_sio_uart_timer_ut_handle, 0);
            break;
        case SIO_UART_TIMER_WAIT_RESPONSE:
            xTimerReset(g_sio_uart_timer_wait_response_handle, 0);
            break;    
        case SIO_UART_TIMER_WAIT_SLEEP:
            xTimerReset(g_sio_uart_timer_wait_sleep_handle, 0);
            break;      
        case SIO_UART_TIMER_WAIT_READY:
            xTimerReset(g_sio_uart_timer_wait_ready_handle, 0);
            break;            
        default:
            return SIO_UART_RET_ERROR;
    }

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_timer_reset_from_isr(sio_uart_timer_t timer)
{
    sio_uart_timer_status_t status = sio_uart_timer_get_status(timer);

    switch (status) {
        case SIO_UART_TIMER_STATUS_INVALID:
#ifdef SIO_DEBUG
            configASSERT(0);
#endif /* MSM_DEBUG */
            return SIO_UART_RET_ERROR;
        case SIO_UART_TIMER_STATUS_DELETED:
            LOGE("sio_uart_timer_reset_from_isr, timer:%d has been deleted\r\n", timer);
            return SIO_UART_RET_ERROR;
    }

    LOGI("sio_uart_timer_reset_from_isr, timer:%d \r\n", timer);

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION:
            xTimerResetFromISR(g_sio_uart_timer_modem_exception_handle, 0);
            break;
        case SIO_UART_TIMER_WAIT_RESPONSE:
            xTimerResetFromISR(g_sio_uart_timer_wait_response_handle, 0);
            break;
        case SIO_UART_TIMER_WAIT_SLEEP:
            xTimerResetFromISR(g_sio_uart_timer_wait_sleep_handle, 0);
            break;
        case SIO_UART_TIMER_UT:
            xTimerResetFromISR(g_sio_uart_timer_ut_handle, 0);
            break;    
        case SIO_UART_TIMER_WAIT_READY:
            xTimerResetFromISR(g_sio_uart_timer_wait_ready_handle, 0);
            break;   
        default:
            return SIO_UART_RET_ERROR;
    }

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_timer_stop(sio_uart_timer_t timer)
{
    sio_uart_timer_status_t status = sio_uart_timer_get_status(timer);

    LOGI("sio_uart_timer_delete, timer:%d \r\n", timer);

    switch (status) {
        case SIO_UART_TIMER_STATUS_INVALID:
#ifdef SIO_DEBUG
            configASSERT(0);
#endif /* SIO_DEBUG */
            return SIO_UART_RET_ERROR;
        case SIO_UART_TIMER_STATUS_DELETED:
            LOGW("sio_uart_timer_delete, timer:%d has been deleted\r\n", timer);
            return SIO_UART_RET_OK;
    }

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION: {
            xTimerStop(g_sio_uart_timer_modem_exception_handle, 0);
            g_sio_uart_timer_modem_exception_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_UT: {
            xTimerStop(g_sio_uart_timer_ut_handle, 0);
            g_sio_uart_timer_ut_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_WAIT_SLEEP: {
            xTimerStop(g_sio_uart_timer_wait_sleep_handle, 0);
            g_sio_uart_timer_wait_sleep_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_WAIT_RESPONSE: {
            xTimerStop(g_sio_uart_timer_wait_response_handle, 0);
            g_sio_uart_timer_wait_response_handle = NULL;
            break;
        }   
        case SIO_UART_TIMER_WAIT_READY:{
            xTimerStop(g_sio_uart_timer_wait_ready_handle, 0);
            g_sio_uart_timer_wait_ready_handle = NULL;
            break;
        }    
        default:
            return SIO_UART_RET_ERROR;
    }

    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_timer_delete(sio_uart_timer_t timer)
{
    sio_uart_timer_status_t status = sio_uart_timer_get_status(timer);

    LOGI("sio_uart_timer_delete, timer:%d \r\n", timer);

    switch (status) {
        case SIO_UART_TIMER_STATUS_INVALID:
#ifdef SIO_DEBUG
            configASSERT(0);
#endif /* SIO_DEBUG */
            return SIO_UART_RET_ERROR;
        case SIO_UART_TIMER_STATUS_DELETED:
            LOGW("sio_uart_timer_delete, timer:%d has been deleted\r\n", timer);
            return SIO_UART_RET_OK;
    }

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION: {
            xTimerDelete(g_sio_uart_timer_modem_exception_handle, 0);
            g_sio_uart_timer_modem_exception_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_UT: {
            xTimerDelete(g_sio_uart_timer_ut_handle, 0);
            g_sio_uart_timer_ut_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_WAIT_SLEEP: {
            xTimerDelete(g_sio_uart_timer_wait_sleep_handle, 0);
            g_sio_uart_timer_wait_sleep_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_WAIT_RESPONSE: {
            xTimerDelete(g_sio_uart_timer_wait_response_handle, 0);
            g_sio_uart_timer_wait_response_handle = NULL;
            break;
        }
        case SIO_UART_TIMER_WAIT_READY:{
            xTimerDelete(g_sio_uart_timer_wait_ready_handle, 0);
            g_sio_uart_timer_wait_ready_handle = NULL;
            break;
        }    
        default:
            return SIO_UART_RET_ERROR;
    }

    return SIO_UART_RET_OK;
}

sio_uart_timer_status_t sio_uart_timer_get_status(sio_uart_timer_t timer)
{
    sio_uart_timer_status_t status = SIO_UART_TIMER_STATUS_INVALID;

    switch (timer) {
        case SIO_UART_TIMER_MODEM_EXCEPTION: {
            if (g_sio_uart_timer_modem_exception_handle) {
                status = SIO_UART_TIMER_STATUS_RUNNING;
            } else {
                status = SIO_UART_TIMER_STATUS_DELETED;
            }

            break;
        }
        case SIO_UART_TIMER_WAIT_RESPONSE: {
            if (g_sio_uart_timer_wait_response_handle) {
                status = SIO_UART_TIMER_STATUS_RUNNING;
            } else {
                status = SIO_UART_TIMER_STATUS_DELETED;
            }

            break;
        }
        case SIO_UART_TIMER_WAIT_SLEEP: {
            if (g_sio_uart_timer_wait_sleep_handle) {
                status = SIO_UART_TIMER_STATUS_RUNNING;
            } else {
                status = SIO_UART_TIMER_STATUS_DELETED;
            }

            break;
        }
        case SIO_UART_TIMER_WAIT_READY: {
            if (g_sio_uart_timer_wait_ready_handle) {
                status = SIO_UART_TIMER_STATUS_RUNNING;
            } else {
                status = SIO_UART_TIMER_STATUS_DELETED;
            }

            break;
        }
        case SIO_UART_TIMER_UT: {
            if (g_sio_uart_timer_ut_handle) {
                status = SIO_UART_TIMER_STATUS_RUNNING;
            } else {
                status = SIO_UART_TIMER_STATUS_DELETED;
            }

            break;
        }
        default:
            break;
    }

    return status;
}

