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
#include <stdio.h>
#include <string.h>
#include <wchar.h>
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
#include "nvdm.h"


#if 1

#define APP_BT_LOG(fmt,arg...)   //LOG_I(common ,"[WATCH]"fmt,##arg)

/*******************************************************************************************************************
 *				Application Task
 *******************************************************************************************************************/
extern xQueueHandle queAppTask;
extern xSemaphoreHandle semModeChange;


void appBT_Display(void)
{



}


static void appBT_Init(void)
{

    uint8_t buf[20] = {0};
    uint16_t buffer[20] = {0};
    uint32_t buf_size = sizeof(buf);
    uint8_t addr[20] = {0};
    nvdm_status_t nvdm_status;

    APP_BT_LOG("appBT_Init\r\n");

    gdi_font_engine_size_t font = GDI_FONT_ENGINE_FONT_MEDIUM;
    gdi_font_engine_display_string_info_t string_info = {0};
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color

    gdi_draw_filled_rectangle(0, 10, 128, 128, 0);
    gdi_font_engine_set_font_size(font);
    gdi_font_engine_set_text_color(text_color);

    string_info.x = 0;
    string_info.y = 10;
    string_info.baseline_height = -1;
    string_info.string = (uint8_t *)L"BT Addr";
    string_info.length = wcslen(L"BT Addr") * 2;
    gdi_font_engine_display_string(&string_info);


    string_info.y = 30;
    string_info.baseline_height = -1;

    nvdm_status = nvdm_read_data_item("BT", "address", (uint8_t *)buf, &buf_size);

    if (nvdm_status != NVDM_STATUS_OK) {


        string_info.x = 6 * 4;
        string_info.string = (uint8_t *)L"unknown";
        string_info.length = wcslen(L"unknown") * 2;

    } else {


        for (int i = 0; i < 6; i++) {
            addr[10 - 2 * i] = buf[2 * i];
            addr[10 - 2 * i + 1] = buf[2 * i + 1];
        }

        printf("addr = %s, original = %s \n\r", addr , buf);


        string_info.x = 6 * 1;
        swprintf((wchar_t *)buffer, 20, L"%s", addr);
        string_info.string = (uint8_t *)buffer;
        string_info.length = wcslen((const wchar_t *)buffer) * 2;

    }

    gdi_font_engine_display_string(&string_info);
    gdi_lcd_update_screen(0, 10, 128, 64);
    appBT_Display();
}

void appBT_Task(void *pParameters)
{
    tEvent inEvent = {EVENT_NO, NULL} ;

    APP_BT_LOG("appBT_Task\r\n");

    appBT_Init();


    for (;;) {
        if (pdTRUE == xQueueReceive(queAppTask, &inEvent, portMAX_DELAY /*60000/portTICK_RATE_MS*/)) {
            switch (inEvent.event) {
                case EVEINT_1MIN :
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

