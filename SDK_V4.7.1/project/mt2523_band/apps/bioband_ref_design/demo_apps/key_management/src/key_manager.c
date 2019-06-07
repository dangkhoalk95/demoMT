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

// For Register AT command handler
// System head file

#include "hal_keypad.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include "keypad_custom.h"
#include "key_manager.h"
#include "hal_sleep_manager.h"
#include "hal_gpio.h"
#include "hal_gpt.h"
#include "task_def.h"
#include "timers.h"

xQueueHandle queKeypad;

QueueHandle_t keypad_queue_handle;

TaskHandle_t  keypad_task_handle;

typedef enum {
    KEYPAD_GET_KEY_DATA = 1,
    POWERKEY_GET_KEY_DATA = 2
} keypad_msg_type_t;

typedef struct {
    char *src_mod;
    keypad_msg_type_t msg_id;
} keypad_msg_t;


void keypad_task_main(void *pvParameters);
void keypad_powerkey_handler(void *arg);
void keypad_handler(void *arg);
void bl_rtc_mode_control(int32_t set);

TimerHandle_t xTimerofdownload;
unsigned int pwr_count = 0,key_count = 0,count = 0;
void download_timeout_callback(TimerHandle_t pxTimer)
{    
    if((pwr_count == 8)&&(key_count == 8))
    {
        printf("Start Download\r\n");
        bl_rtc_mode_control(0); // clear RTC mode
        pmu_set_register_value(PMU_RG_PWRHOLD_ADDR, PMU_RG_PWRHOLD_MASK, PMU_RG_PWRHOLD_SHIFT, 0);
    }
    
    pwr_count = 0;
    key_count = 0;
}

void keypad_task_init(void)
{

#if 0

    bool                            ret_bool;
    hal_keypad_status_t ret_state;
    /*init keypad and powerkey*/

    keypad_queue_handle   = xQueueCreate(10, sizeof(keypad_msg_t));
    queKeypad   = xQueueCreate(10, sizeof(tKeypadEvent));
    xTaskCreate(keypad_task_main, KEYPAD_TASK_NAME, KEYPAD_TASK_STACK_SIZE / sizeof(StackType_t), NULL, KEYPAD_TASK_PRIO, &keypad_task_handle);


    ret_bool = keypad_custom_powerkey_init();
    if (ret_bool == false) {
        printf("[keypad][test]keypad_custom_init init fail\r\n");
    }
    ret_state = hal_keypad_powerkey_register_callback((hal_keypad_callback_t)keypad_powerkey_handler, NULL);
    if (ret_state != HAL_KEYPAD_STATUS_OK) {

        hal_pmu_set_register_value(HAL_PMU_RG_PWRHOLD_ADDR, HAL_PMU_RG_PWRHOLD_MASK, HAL_PMU_RG_PWRHOLD_SHIFT, 0);

    }

    ret_bool = keypad_custom_normal_init();

    if (ret_bool == false) {
        printf("[keypad][test]keypad_custom_init init fail\r\n");
    }

    ret_state = hal_keypad_register_callback((hal_keypad_callback_t)keypad_handler, NULL);
    if (ret_state != HAL_KEYPAD_STATUS_OK) {
        printf("[keypad][test]hal_keypad_key_register_callback fail, state = %d\r\n", ret_state);
    }

    ret_state = hal_keypad_enable();
    if (ret_state != HAL_KEYPAD_STATUS_OK) {
        printf("[keypad][test]hal_keypad_enable fail, state = %d\r\n", ret_state);
    }


    printf("keypad and powerkey initilized OK...\r\n");

#else

    // Todo: add keypad case, turn on keypad module and return keypad_state and key_data
    if (keypad_custom_init() == true) {
        keypad_queue_handle   = xQueueCreate(10, sizeof(keypad_msg_t));
        queKeypad   = xQueueCreate(10, sizeof(tKeypadEvent));
        xTaskCreate(keypad_task_main, KEYPAD_TASK_NAME, KEYPAD_TASK_STACK_SIZE / sizeof(StackType_t), NULL, KEYPAD_TASK_PRIO, &keypad_task_handle);

        hal_keypad_register_callback(keypad_handler, NULL);
        hal_keypad_enable();
        hal_keypad_powerkey_register_callback(keypad_powerkey_handler, NULL);

        //printf("keypad and powerkey initilized OK...\r\n");
    } else {


        //printf("keypad and powerkey initilized NOT OK...\r\n");
        hal_sleep_manager_enter_power_off_mode();
    }

#endif
}

void keypad_task_deinit(void)
{
    if (keypad_custom_deinit() == true) {
        vQueueDelete(keypad_queue_handle);
        vQueueDelete(queKeypad);
        vTaskDelete(keypad_task_handle);
        hal_keypad_disable();

        //printf("keypad and powerkey deinitilized OK...\r\n");
    }
}

void keypad_task_main(void *pvParameters)
{
    keypad_msg_t msgs;

    int  key_data  = 0;
    int  key_state = 0;
    //uint32_t key_position = 0;

    hal_keypad_event_t 			keypad_event;
    hal_keypad_powerkey_event_t powekey_event;
    hal_keypad_status_t		 	ret;

    tKeypadEvent outEvent;

    xTimerofdownload = xTimerCreate("xTimerofdownload",       /* Just a text name, not used by the kernel. */
                                   (1 * 5000 / portTICK_PERIOD_MS),    /* The timer period in ticks. */
                                   pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                   NULL,   /* Assign each timer a unique id equal to its array index. */
                                   download_timeout_callback /* Each timer calls the same callback when it expires. */
                                  );    
    
    while (1) {
        if (xQueueReceive(keypad_queue_handle, &msgs, portMAX_DELAY)) {
            switch (msgs.msg_id) {
                case KEYPAD_GET_KEY_DATA: {
                    //printf("[user]**enter keypad hanlder get event in at command\r\n");
                    while (1) {
                        ret = hal_keypad_get_key(&keypad_event);

                        if (ret == HAL_KEYPAD_STATUS_ERROR) {
                            //printf("[keypad][DATA]normal no key in buffer\r\n");
                            break;
                        }

                        //key_position  = keypad_event.key_data; //key postion in register
                        key_state 	  = keypad_event.state;
                        key_data      = keypad_custom_translate_keydata(keypad_event.key_data);
                        key_count++;

                        //printf("[keypad][DATA]keypad state = %d, key_position = %d, key_data = %d\r\n",keypad_event.state,key_position,keypad_event.key_data);
                        outEvent.key_state = key_state;
                        outEvent.key_data = key_data;
                        if (queKeypad != NULL) {
                            xQueueSend(queKeypad, &outEvent, 0);
                        }
                    }


                }
                break;


                case POWERKEY_GET_KEY_DATA: {
                    //printf("[user]**enter powerkey hanlder get event in at command\r\n");
                    while (1) {
                        ret = hal_keypad_powerkey_get_key(&powekey_event);

                        if (ret == HAL_KEYPAD_STATUS_ERROR) {
                            //printf("[keypad][DATA]powerkey no key in buffer\r\n");
                            break;
                        }

                        key_state 	  = powekey_event.state;
                        key_data      = powekey_event.key_data;

                        //printf("[keypad][DATA]powerkey state = %d, key_data = %d\r\n",powekey_event.state,powekey_event.key_data);


#if 1

                        if (key_state == 2) {

                            hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
                            hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW);
                            hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_HIGH);

                            hal_gpt_delay_ms(3 * 1000);
                            printf("[keypad][test]start to power down\r\n");
                            hal_sleep_manager_enter_power_off_mode();
                        }

#endif
                        pwr_count++;
                        xTimerStart(xTimerofdownload, 0);
                        outEvent.key_state = key_state;
                        outEvent.key_data = key_data;
                        if (queKeypad != NULL) {
                            xQueueSend(queKeypad, &outEvent, 0);
                        }
                    }


                }
                break;

                default: {


                }
                break;
            }
        }
        //vTaskDelay(xTicksToWait);
    }

}
void keypad_powerkey_handler(void *arg)
{
    keypad_msg_t msgs;
    BaseType_t xHigherPriorityTaskWoken;

    // We have not woken a task at the start of the ISR.
    xHigherPriorityTaskWoken = pdFALSE;

    msgs.msg_id =  POWERKEY_GET_KEY_DATA;
    //printf("enter atci_keypad_handler \r\n");
    while (xQueueSendFromISR(keypad_queue_handle, &msgs, &xHigherPriorityTaskWoken) != pdTRUE);

    // Now the buffer is empty we can switch context if necessary.
    if (xHigherPriorityTaskWoken) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }

}

void keypad_handler(void *arg)
{
    keypad_msg_t msgs;
    BaseType_t xHigherPriorityTaskWoken;

    // We have not woken a task at the start of the ISR.
    xHigherPriorityTaskWoken = pdFALSE;

    msgs.msg_id = KEYPAD_GET_KEY_DATA ;
    //printf("enter atci_keypad_handler \r\n");
    while (xQueueSendFromISR(keypad_queue_handle, &msgs, &xHigherPriorityTaskWoken) != pdTRUE);

    // Now the buffer is empty we can switch context if necessary.
    if (xHigherPriorityTaskWoken) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }

}

#define BL_RTC_CON      ((volatile uint16_t *)(0xA21E0078))
#define BL_RTC_BBPU     ((volatile uint16_t *)(0xA21E0000)) /* [6] CBUSY */
#define BL_RTC_WRTGR    ((volatile uint16_t *)(0xA21E0074)) /* [0] WRTGR */

static void bl_rtc_write_trigger(void)
{
    uint32_t count = 0;

    // RTC write trigger
    *BL_RTC_WRTGR = 1;
    // RTC wait busy
    while (count < 0x6EEEEE) {
        if ((0x40 & (*BL_RTC_BBPU)) == 0) {
            break;
        }
        count++;
    }
     if (count >= 0x6EEEEE) {
         printf("RTC write error\r\n");
    }
}

void bl_rtc_mode_control(int32_t set)
{
    uint16_t rtc_con = 0;

    rtc_con = *BL_RTC_CON;
    printf("1 RTC_CON yo %x\n\r", rtc_con);

    if (set != 0) {
        /* set RTC mode */
        if ((rtc_con & 0x100) == 0) {
            rtc_con |= 0x100; // set bit 8
            *BL_RTC_CON = rtc_con;
            bl_rtc_write_trigger();
            printf("2 Set RTC mode (%x)\n\r", *BL_RTC_CON);
        }
    } else {
        /* clear RTC mode */
        if (rtc_con & 0x100) {
            rtc_con &= 0xFEFF; // clear bit 8
            *BL_RTC_CON = rtc_con;
            bl_rtc_write_trigger();
            printf("3 Clear RTC mode (%x)\n\r", *BL_RTC_CON);
        }
    }
    //hal_rtc_disable_alarm();
}
