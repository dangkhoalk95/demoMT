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

/**************************************************************************//**
 *
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "bsp_lcd.h"
#include "lcd_manager.h"
//#include "bl_manager.h"

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
#include "task_def.h"
#include "app_task.h"
#include "watch.h"
#include "app_manager.h"
#include "key_manager.h"
#include "hal_keypad_table.h"
#include "hal_keypad.h"
#include "app_status_bar.h"





#define APP_MGR_LOG(fmt,arg...)   LOG_I(common ,"[MGR]"fmt,##arg)

#if 1

tAppTask appModeTable[MODE_LAST] = {
    {MODE_HR,			MODE_WATCH,		 appWatch_Task},
    {MODE_HRV,     	    MODE_WATCH,		 appHR_Task},
    {MODE_BP, 			MODE_WATCH,		 appHRV_Task},
#ifdef __INTERNAL_UI_SUPPORT__
    {MODE_SLEEP, 		MODE_WATCH,		 appBP_Task},
    {MODE_PEDO, 		MODE_WATCH,		 appSLEEP_Task},
    {MODE_BTINFO, 		MODE_WATCH,		 appPedo_Task},
    {MODE_WATCH, 		MODE_WATCH,		 appBT_Task},
#else
    {MODE_BTINFO, 		MODE_WATCH,		 appBP_Task},
    {MODE_WATCH, 		MODE_WATCH,		 appBT_Task},
#endif
};


xSemaphoreHandle semModeChange;
xTaskHandle modeTaskHandle;
xQueueHandle queAppMgr;
xQueueHandle queAppTask;
TimerHandle_t vTestTimer;
volatile int isPreviousLongPress = 0;
volatile int isReleaseFromLcdOff = 0;
extern uint32_t hr_init_val;
extern int32_t status_init_val;
extern uint32_t steps_init_val;
extern uint32_t bp1_init_val;
extern uint32_t bp2_init_val;
extern uint32_t sdnn_init_val;
extern int32_t lf_hf_init_val;


extern volatile int isBPFnTrigger;
extern volatile int isHRFnTrigger;
extern volatile int isHRVFnTrigger;
extern volatile int isPEDOFnTrigger;
extern volatile int isSLEEPFnTrigger;

bool isFnTrigger(void) {

    if (1 == isBPFnTrigger ||
        1 == isHRFnTrigger ||
        1 == isHRVFnTrigger ||
        1 == isPEDOFnTrigger ||
        1 == isSLEEPFnTrigger) return true;

    return false;

}


void appKeyProc_Task(void *pParameters)
{
    pParameters = pParameters;   /* to quiet warnings */
    tKeypadEvent eventGet;
    tEvent outEvent;

    for (;;) {

        if (queKeypad == NULL) {
            vTaskDelay(10);
            continue;
        }

        if (pdTRUE == xQueueReceive(queKeypad, &eventGet, portMAX_DELAY)) {
            //APP_MGR_LOG(common,"==== key_state=%d,key_data=%d ====\r\n",eventGet.key_state,eventGet.key_data);
            switch (eventGet.key_data) {
                case DEVICE_KEY_UP:
                    if (eventGet.key_state == HAL_KEYPAD_KEY_RELEASE ) {
                        if (isPreviousLongPress == 1) {
                            isPreviousLongPress = 0;
                        } else {

                            APP_MGR_LOG("==========DEVICE_KEY_RELEASE ==============\r\n");
                            if (isReleaseFromLcdOff == 0) {
                                outEvent.event = EVENT_BUTTON_1;
                            } else {
                                outEvent.event = EVENT_BUTTON_5;
                            }
                            xQueueSend(queAppMgr, &outEvent, 0);
                        }

                    } else if (eventGet.key_state == HAL_KEYPAD_KEY_LONG_PRESS ) {

                        APP_MGR_LOG("=========== DEVICE_KEY_LONG_PRESS ========== \r\n");
                        isPreviousLongPress = 1;
                        outEvent.event = EVENT_BUTTON_LEFT_LONG_PRESS;
                        xQueueSend(queAppMgr, &outEvent, 0);
                    } else if (eventGet.key_state == HAL_KEYPAD_KEY_PRESS) {
                        if (bl_isOn()) {
                            isReleaseFromLcdOff = 0;
                        } else {
                            isReleaseFromLcdOff = 1;
                        }
                    }

                    break;

                case DEVICE_KEY_POWER:
                    if (eventGet.key_state == HAL_KEYPAD_KEY_RELEASE) {
                        APP_MGR_LOG("DEVICE_KEY_POWER press\r\n");
                        if (isReleaseFromLcdOff == 0) {
                            outEvent.event = EVENT_BUTTON_2;
                        } else {
                            outEvent.event = EVENT_BUTTON_5;
                        }
                        xQueueSend(queAppMgr, &outEvent, 0);
                    } else if (eventGet.key_state == HAL_KEYPAD_KEY_LONG_PRESS) {
                        APP_MGR_LOG("DEVICE_KEY_POWER long press \r\n");
                        outEvent.event = EVENT_BUTTON_PWR_LONG_PRESS;
                        xQueueSend(queAppMgr, &outEvent, 0);
                    } else if (eventGet.key_state == HAL_KEYPAD_KEY_PRESS) {
                        if (bl_isOn()) {
                            isReleaseFromLcdOff = 0;
                        } else {
                            isReleaseFromLcdOff = 1;
                        }
                    }

                    break;
                default:

                    break;

            }

            bl_TimeReset();
        }
    }
}

void appMgr_Task(void *pParameters)
{
    pParameters = pParameters;   /* to quiet warnings */
    tEvent eventGet, outEvent;
    enum tModeType modeInUse, modeForNext = MODE_WATCH;
    int modeChange = 0;

    /* Create Semaphore */
    vSemaphoreCreateBinary(semModeChange);

    queAppTask = xQueueCreate(5, sizeof(tEvent));

    xTaskCreate(appKeyProc_Task, APP_KEYPROC_TASK_NAME, APP_KEYPROC_TASK_STACK_SIZE / sizeof(StackType_t), NULL, APP_KEYPROC_TASK_PRIO, NULL);

    modeInUse =  MODE_WATCH;
    xTaskCreate(appModeTable[modeInUse].pAppTask, MODE_TASK_NAME, MODE_TASK_STACK_SIZE / sizeof(StackType_t), NULL, MODE_TASK_PRIO, &modeTaskHandle);
    for (;;) {
        if (pdTRUE == xQueueReceive(queAppMgr, &eventGet, portMAX_DELAY)) {
            //APP_MGR_LOG("==== appMgr_Task event=%d ====\r\n",eventGet.event);
            switch (eventGet.event) {
                case EVEINT_1MIN :
                    outEvent.event = EVEINT_1MIN;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_BUTTON_1:


                    if (true == isFnTrigger()) break;

                    modeForNext = appModeTable[modeInUse].btn1Mode;
                    modeChange = 1;
                    outEvent.event = EVENT_TASK_END;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_BUTTON_2:
                    
                    if (true == isFnTrigger()) break;
                    
                    modeForNext = appModeTable[modeInUse].btn2Mode;
                    modeChange = 1;
                    outEvent.event = EVENT_TASK_END;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_BUTTON_LEFT_LONG_PRESS:
                    modeForNext = appModeTable[modeInUse].btn1Mode;
                    modeChange = 0;
                    outEvent.event = EVENT_BUTTON_LEFT_LONG_PRESS;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;


                case EVENT_BUTTON_3:
                    outEvent.event = EVENT_BUTTON_3;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_BUTTON_4:			// BUTTON 4
                    outEvent.event = EVENT_BUTTON_4;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_BUTTON_5:			// BUTTON 5
                    // Turn ON LED
                    modeForNext = modeInUse;
                    modeChange = 1;
                    outEvent.event = EVENT_TASK_END;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_UPDATE_PEDO:			// BUTTON 5
                    outEvent.event = EVENT_UPDATE_PEDO;
                    outEvent.userdata = eventGet.userdata;
                    steps_init_val = ((sensor_pedometer_event_t *)eventGet.userdata)->accumulated_step_count;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_UPDATE_HR:			// BUTTON 5
                    outEvent.event = EVENT_UPDATE_HR;
                    outEvent.userdata = eventGet.userdata;

                    hr_init_val = ((sensor_heart_rate_event_t *)eventGet.userdata)->bpm;
                    status_init_val = (((sensor_heart_rate_event_t *)eventGet.userdata)->status) & 0xFF;

                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_UPDATE_HRV:			// BUTTON 5
                    outEvent.event = EVENT_UPDATE_HRV;
                    outEvent.userdata = eventGet.userdata;

                    sdnn_init_val = ((sensor_heart_rate_variability_event_t *)eventGet.userdata)->SDNN / 1000;
                    lf_hf_init_val = ((sensor_heart_rate_variability_event_t *)eventGet.userdata)->LF_HF / 1000;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;

                case EVENT_UPDATE_BP:			// BUTTON 5
                    outEvent.event = EVENT_UPDATE_BP;
                    outEvent.userdata = eventGet.userdata;
                    bp1_init_val = ((sensor_blood_pressure_event_t *)eventGet.userdata)->sbp;
                    bp2_init_val = ((sensor_blood_pressure_event_t *)eventGet.userdata)->dbp;
                    xQueueSend(queAppTask, &outEvent, 0);
                    break;


                default:
                    break;
            }

            if (modeChange == 1) {
                if (pdTRUE == xSemaphoreTake(semModeChange, portMAX_DELAY)) {
                    modeInUse = modeForNext;
                    modeChange = 0;
                    if (modeTaskHandle != NULL) {
                        vTaskDelete(modeTaskHandle);
                        while (eTaskGetState(modeTaskHandle) != eDeleted); // wait for delete task
                    }
                    //xQueueReset(qMainEvent);
                    xQueueReset(queAppTask);

                    outEvent.event = EVENT_INIT_APP;
                    xQueueSend(queAppTask, &outEvent, 0);
                    xTaskCreate(appModeTable[modeInUse].pAppTask, MODE_TASK_NAME, MODE_TASK_STACK_SIZE / sizeof(StackType_t), NULL, MODE_TASK_PRIO, &modeTaskHandle);
                }
            }
        }
    }
}
#if 0
#include "hal_spi_master.h"
uint32_t pedo;
uint32_t heart;
int32_t spi_sendAndReceive(uint8_t *txbuf, uint16_t tx_len, uint8_t *rxbuf, uint16_t rx_len);
int32_t spi_send(uint8_t *txbuf, uint16_t tx_len);
int32_t spi_receive(uint8_t *rxbuf, uint16_t rx_len);
void at_test_app_manager_handle(int para)
{
    tEvent outEvent;
    uint8_t tbuf[20], rbuf[20];
    uint8_t send = 0;
    uint8_t receive[4] = {0};
    hal_spi_master_send_and_receive_config_t cfg;
    hal_spi_master_status_t ret;
    LOG_I(common, "at_test_app_manager_handle-para:%d\r\n", para);
    switch (para % 10) {
        case 0:
            if (bl_isOn()) {
                outEvent.event = EVENT_BUTTON_1;
                xQueueSend(queAppMgr, &outEvent, 0);
            }
            bl_TimeReset();
            break;
        case 1:
            pedo = para / 10;
            outEvent.event = EVENT_UPDATE_PEDO;
            outEvent.userdata = &pedo;
            xQueueSend(queAppMgr, &outEvent, 0);
            break;
        case 2:
            heart = para / 10;
            outEvent.event = EVENT_UPDATE_HR;
            outEvent.userdata = &heart;
            xQueueSend(queAppMgr, &outEvent, 0);
            break;
        case 3:
            outEvent.event = EVENT_BUTTON_2;
            xQueueSend(queAppMgr, &outEvent, 0);
            break;
        case 4:
            tbuf[0] = 0xAA;
            tbuf[1] = 0xAA;
            spi_sendAndReceive(tbuf, 2, rbuf, 2);
            break;
        case 5:
            tbuf[0] = 0x55;
            tbuf[1] = 0xAA;
            spi_send(tbuf, 2);
            break;
        case 6:
            send = 0x9F;
            cfg.send_data = &send;
            cfg.send_length = 1;
            cfg.receive_buffer = receive;
            cfg.receive_length = 4;
            ret = hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_3, &cfg);
            LOG_I(common, "ret=%d,receive[0]=0x%x,receive[1]=0x%x,receive[2]=0x%x,receive[3]=0x%x\r\n", ret, receive[0], receive[1], receive[2], receive[3]);
            break;
        default:
            break;
    }
}
#endif
#endif

