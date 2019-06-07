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
#include <string.h>
#include "bsp_lcd.h"
#include "lcd_manager.h"

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

#include "watch.h"
//#include "clock.h"
#include "lcd_config.h"
#include "app_manager.h"
#include "gdi_font_engine.h"
#include "gdi.h"
#include "custom_resource_def.h"

extern uint8_t* appUtil_convert_string_to_wstring(uint8_t* string);

#if 1

#define APP_WATCH_LOG(fmt,arg...)   //LOG_I(common ,"[WATCH]"fmt,##arg)

/*******************************************************************************************************************
 *				Application Task
 *******************************************************************************************************************/
const char day_str[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const hal_rtc_time_t base_time = {
    0,                                  /**< Seconds after the minute   - [0,59]  */
    0,                                 /**< Minutes after the hour     - [0,59]  */
    0,                                /**< Hours after the midnight   - [0,23]  */
    1,                                  /**< Day of the month           - [1,31]  */
    1,                                  /**< Months                     - [1,12]  */
    5,                                 /**< Days in a week             - [1,7]   */
    16                                /**< Years                      - [0,127] */
};

extern xQueueHandle queAppTask;
extern xSemaphoreHandle semModeChange;

hal_rtc_status_t set_current_time(hal_rtc_time_t time)
{
    hal_rtc_status_t ret;

    APP_WATCH_LOG("set_current_time\n");

    ret = hal_rtc_init();
    if (HAL_RTC_STATUS_OK != ret) {
        //error handling
        APP_WATCH_LOG("hal_rtc_init fail--ret->%d\n", ret);
        return ret;
    }

    //Set the RTC current time
    ret = hal_rtc_set_time(&time);
    if (HAL_RTC_STATUS_OK != ret) {
        //error handling
        APP_WATCH_LOG("hal_rtc_set_time fail--ret->%d\n", ret);
        return ret;
    }


    return ret;

}

hal_rtc_status_t watch_get_current_time(hal_rtc_time_t *time)
{
    hal_rtc_status_t ret;

    ret = hal_rtc_init();
    if (HAL_RTC_STATUS_OK != ret) {
        //error handling
        APP_WATCH_LOG("hal_rtc_init fail--ret->%d\n", ret);
        return ret;
    }

    //Set the RTC current time
    ret = hal_rtc_get_time(time);
    if (HAL_RTC_STATUS_OK != ret) {
        //error handling
        APP_WATCH_LOG("hal_rtc_set_time fail--ret->%d\n", ret);
        return ret;
    }

    return ret;
}

void appWatch_DisplayTime(void)
{
    uint8_t buffer[40];
    hal_rtc_time_t time;
    gdi_font_engine_size_t font = GDI_FONT_ENGINE_FONT_MEDIUM;
    gdi_font_engine_display_string_info_t string_info = {0};
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color
    gdi_draw_filled_rectangle(0, 10, 128, 128, 0);
    watch_get_current_time(&time);
    gdi_font_engine_set_font_size(font);
    gdi_font_engine_set_text_color(text_color);
    sprintf((char *)buffer, "20%02d-%02d-%02d", time.rtc_year, time.rtc_mon, time.rtc_day);
    string_info.x = 24;
    string_info.y = 11;
    string_info.baseline_height = -1;
    string_info.string = appUtil_convert_string_to_wstring(buffer);
    string_info.length = strlen((char *)buffer);
    //printf("gdi_font_engine_show_string1 %s, x %d, y %d, len %d\n", buffer, string_info.x, string_info.y, string_info.length);
    gdi_font_engine_display_string(&string_info);
    sprintf((char *)buffer, "%02d:%02d  %s", time.rtc_hour, time.rtc_min, day_str[time.rtc_week % 7]);
    string_info.x = 33;
    string_info.y = 40;
    string_info.baseline_height = -1;
    string_info.string = appUtil_convert_string_to_wstring(buffer);
    string_info.length = strlen((char *)buffer);
    //printf("gdi_font_engine_show_string2 %s, x %d, y %d, len %d\n", buffer, string_info.x, string_info.y, string_info.length);
    gdi_font_engine_display_string(&string_info);
    APP_WATCH_LOG("RTC-> (20%02d-%02d-%02d) (%02d:%02d:%02d) (%d)\n", time.rtc_year, time.rtc_mon, time.rtc_day
                  , time.rtc_hour, time.rtc_min, time.rtc_sec
                  , time.rtc_week);
    gdi_lcd_update_screen(0, 10, 128, 64);
}

static void appWatch_UpateTime(void)
{
    appWatch_DisplayTime();
}

void appWatch_rtc_time_callback(void *user_data)
{
    tEvent outEvent;
    // event = WF_EVENT_RTC;
    BaseType_t xHigherPriorityTaskWoken;
    LOG_I(common, "rtc init failed ");
    outEvent.event = EVEINT_1MIN;
    //xQueueSend(queAppMgr,&outEvent,0);
    xQueueSendFromISR(queAppMgr, (void *) &outEvent , &xHigherPriorityTaskWoken);

    // Now the buffer is empty we can switch context if necessary.
    if (xHigherPriorityTaskWoken) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

}

static void appWatch_Init(void)
{
    hal_rtc_time_t time;

    APP_WATCH_LOG("appWatch_Init\r\n");

    watch_get_current_time(&time);
    if (time.rtc_year == 0) {
        set_current_time(base_time);
    }

    appWatch_DisplayTime();
}

void appWatch_Task(void *pParameters)
{
    tEvent inEvent = {EVENT_NO, NULL} ;

    APP_WATCH_LOG("appWatch_Task\r\n");

    appWatch_Init();

    //	hal_rtc_set_time_callback(appWatch_rtc_time_callback, NULL);

    for (;;) {
        if (pdTRUE == xQueueReceive(queAppTask, &inEvent, portMAX_DELAY /*60000/portTICK_RATE_MS*/)) {
            switch (inEvent.event) {
                case EVEINT_1MIN :
                    appWatch_UpateTime();
                    break;

                case EVENT_BUTTON_3:
                    break;
                case EVENT_BUTTON_4:
                    break;
                case EVENT_BUTTON_5:
                    break;
                case EVENT_BUTTON_6:

                    break;
                case EVENT_INIT_APP:
                    break;
                case EVENT_TASK_END:
                    xSemaphoreGive(semModeChange);
                    vTaskSuspend(NULL);
                    break;
                default:
                    break;
            }
        }
    }
}
#endif

