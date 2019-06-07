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
#include "hal_gpio.h"
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
#include "hal_sleep_manager.h"
/* battery management includes */
#include "battery_management.h"

#include "app_task.h"
#include "watch.h"
#include "app_manager.h"
#include "app_status_bar.h"
#include "gdi.h"
#include "custom_resource_def.h"

#define APP_SBAR_LOG(fmt,arg...)   LOG_I(common ,"[SBAR]"fmt,##arg)

#if 1
#define BAT_MAX_LEVEL		4
#define BAT_MAX_VOLT		4200
#define BAT_MIN_VOLT		3000
#define BAT_EMPTY_PERCENTAGE        3

xQueueHandle queStatusBarTask;
TimerHandle_t vStatusBarTimer;

const uint8_t bat_pic[16] =
{0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0x3F, 0x20, 0xE0, 0xE0, 0xE0, 0xE0, 0x20, 0x3F};

typedef struct {
    uint16_t start_x;
    uint16_t start_y;
    uint16_t height;
    uint16_t width;
    uint16_t col;
    const uint8_t *pic;
} bat_image_msg_t;

const bat_image_msg_t bat_image_msg = {
    108,
    1,
    8,
    16,
    12,
    bat_pic
};

typedef struct {
    uint8_t charge;
    uint8_t pre_lv;
    uint8_t cur_lv;
} battery_manager_t;

battery_manager_t bat_msg;
volatile bool bl_en;

TimerHandle_t vBlTimer;
void vBlTimerCallback(TimerHandle_t xTimer)
{
    bl_TimeStop();
}

void bl_TimeStop(void)
{
    lcd_DisplayOff();
    xTimerStop(vBlTimer, 0);
    bl_en = false;
}

void bl_TimeReset(void)
{
    if (bl_en) {
        xTimerReset(vBlTimer, 0);
    } else {
        lcd_DisplayOn();
        xTimerStart(vBlTimer, 0);
        bl_en = true;
    }
}


bool bl_isOn(void)
{
    return bl_en;
}

void bl_init(uint32_t time)
{
    bl_en = true;
    vBlTimer = xTimerCreate("vBlTimer",            // Just a text name, not used by the kernel.
                            (time * 1000 / portTICK_PERIOD_MS), // The timer period in ticks.
                            pdTRUE,                    // The timer is a one-shot timer.
                            0,                          // The id is not used by the callback so can take any value.
                            vBlTimerCallback     // The callback function that switches the LCD back-light off.
                           );

    xTimerStart(vBlTimer, 0);
}

static void check_battery_status(void)
{
    int32_t capacity, battery_temperature,charger_exist;

    capacity = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    battery_temperature = battery_management_get_battery_property(BATTERY_PROPERTY_TEMPERATURE);
    charger_exist = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);

    if((capacity == 0 ) && (capacity != BATTERY_INVALID_VALUE)) {
        /* Low battery shutdown */
        //printf("Low battery shutdown");
        vTaskDelay(3 * 1000 / portTICK_RATE_MS);
        hal_sleep_manager_enter_power_off_mode();
    }

    /* High temperature protection. It depends on your application and the battery specifications */
    if((battery_temperature >= 60 ) && (battery_temperature != BATTERY_INVALID_VALUE)) {
        /* High temperature  shutdown */
        //printf("High temperature shutdown");
        vTaskDelay(3 * 1000 / portTICK_RATE_MS);
        hal_sleep_manager_enter_power_off_mode();
    }

    if((charger_exist == 1 ) && (charger_exist != BATTERY_INVALID_VALUE)
       && (capacity == 100 ) && (capacity != BATTERY_INVALID_VALUE)) {
        /* Battery full */
        //printf("Battery full");
    }

}

void vStatusBarCallback(TimerHandle_t xTimer);
static void get_battery_notification(void)
{
	bool cable_in;
    cable_in = (bool)battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
	//printf("cable_in = %d, bat_msg.charge = %d \n\r", cable_in, bat_msg.charge);

    check_battery_status();

	if (bat_msg.charge != cable_in)
	{
		bl_TimeReset();
		bat_msg.charge = cable_in;

		if (bat_msg.charge == 0) {


			//printf("charger remove \n\r");
	   		xTimerStop(vStatusBarTimer, 0);
            vStatusBarCallback(0); // should update as the time stops.

		} else {

			//printf("charger start \n\r");
	   		xTimerStart(vStatusBarTimer, 0);

		}
	}

}


void battery_display(const bat_image_msg_t *msg, uint8_t level, uint8_t max_level)
{
    uint8_t right;
    uint8_t x = 108, y = 1;
    uint32_t w, h;
#if __GT_OLD__
    disp_draw_pic(msg->start_x, msg->start_y, msg->height, msg->width, msg->pic);
    right = (level * msg->col / max_level);
    disp_draw_rect(msg->start_x, msg->start_y + 1, msg->start_x + right, msg->height - 1, COLOR_BLACK, 1);

    /* Initial UI */
    lcd_UpdateRect(msg->start_x, msg->start_y, msg->start_x + msg->width + 1, msg->start_y + msg->height + 1);
#endif
#ifndef __BATTERY__
    gdi_image_get_dimension_by_id(IMAGE_ID_CHARGING_BMP, &w, &h);
    gdi_draw_filled_rectangle(x, y, x + w, y + h, gdi_get_color_from_argb(0, 0, 0, 0));
    gdi_image_draw_by_id(x, y, IMAGE_ID_CHARGING_BMP);
    right = (level * msg->col / max_level);
    if (right > 0) {
        gdi_draw_filled_rectangle(x + 1, y, x + right + 1, y + h - 1, gdi_get_color_from_argb(0, 255, 255, 255));
    }
    gdi_lcd_update_screen(x, y, 128, y + h -1);
#endif
    //printf("battery_display level:%d, max:%d\n", level, max_level);
}

uint8_t battery_get_level(uint8_t max_level)
{
    int32_t percentage  = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    uint8_t capacity_level;

    if (percentage <= BAT_EMPTY_PERCENTAGE) {
        capacity_level = 0;
    } else {
        capacity_level = ((percentage * max_level) / 100) + 1;
        if (capacity_level > max_level) {
            capacity_level = max_level;
        }
    }

    return capacity_level;
}

void vStatusBarCallback(TimerHandle_t xTimer)
{
	bl_TimeReset();

	if (bat_msg.charge && (battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY)!= 100))
	{
		bat_msg.cur_lv++;
		if (bat_msg.cur_lv > BAT_MAX_LEVEL)
				bat_msg.cur_lv = 0;
	}
	else
	{
		bat_msg.cur_lv = battery_get_level(BAT_MAX_LEVEL);
	}

	if (!bl_isOn())
	{
		bat_msg.pre_lv = 0xFF;
	}
	else
	{

		//if (bat_msg.cur_lv != bat_msg.pre_lv)
		{
			battery_display(&bat_image_msg,bat_msg.cur_lv,BAT_MAX_LEVEL);
			bat_msg.pre_lv = bat_msg.cur_lv;
		}
	}


}



static void battery_message_callback_handler(battery_management_event_t event, const void *data)
{


    //printf("get battery battery_message_callback_handler:%d \n\r", event);

    switch (event) {
        case BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE: {
			get_battery_notification();
        }
        break;

        default: {

        }
        break;
    }
}


void appStatusBar_Task(void *pParameters)
{
	bool cable_in;
	uint8_t eventGet;
	battery_management_status_t status;

	queStatusBarTask = xQueueCreate(2,sizeof(uint8_t));
	bat_msg.charge = 0;

    cable_in = (bool)battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);

	bat_msg.cur_lv = battery_get_level(BAT_MAX_LEVEL);
	bat_msg.pre_lv = bat_msg.cur_lv;
	battery_display(&bat_image_msg,bat_msg.cur_lv,BAT_MAX_LEVEL);
    vStatusBarTimer = xTimerCreate( "vStatusBarCallback",           // Just a text name, not used by the kernel.
                      ( 1000 / portTICK_PERIOD_MS), // The timer period in ticks.
                                      pdTRUE,                    // The timer is a one-shot timer.
                                      0,                          // The id is not used by the callback so can take any value.
                                      vStatusBarCallback     // The callback function that switches the LCD back-light off.
                                   );
	if (cable_in)
	{
		bat_msg.charge = 1;
		xTimerStart(vStatusBarTimer, 0);

	} else {


	}

	status = battery_management_register_callback(battery_message_callback_handler);
	if (status != BATTERY_MANAGEMENT_STATUS_OK) {}


    bl_init(5);

    for (;;) {
        if (pdTRUE == xQueueReceive(queStatusBarTask, &eventGet, portMAX_DELAY)) {
            switch (eventGet) {
                case 0:
                    break;
            }
        }
    }
}
#endif


