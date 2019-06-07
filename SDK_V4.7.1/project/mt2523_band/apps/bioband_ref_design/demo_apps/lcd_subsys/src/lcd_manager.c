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
#include "bsp_lcd.h"
#include "mt25x3_hdk_lcd.h"
#include "bw_gdi_api.h"
#include "lcd_manager.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "task_def.h"
#include "gdi.h"
#include "gdi_font_engine.h"
#include "hal_log.h"

#define LCD_INFO(fmt, args...)    printf("[LCDManager] INFO: "fmt, ##args)

xQueueHandle queLcdUpdate;
TaskHandle_t lcdTaskHandle;
static bool lcd_enable = false;
extern bool bl_en;

void lcd_mgr_task(void *pvParameters);
static uint8_t __attribute__((__section__(".noncached_zidata"),__aligned__(4))) gdi_buf[128 * 64 / 8] = {0};
void lcd_manager_init(void)
{
    //LCD_INFO("lcd manager init......\r\n");
    bsp_lcd_init(0);
    lcd_enable = true;
    queLcdUpdate = xQueueCreate(5, sizeof(tLcdLimit));
    gdi_init(128, 64, GDI_COLOR_FORMAT_1, gdi_buf);
    xTaskCreate(lcd_mgr_task, LCDMGR_TASK_NAME, LCDMGR_TASK_STACK_SIZE / sizeof(StackType_t), (void *)0, LCDMGR_TASK_PRIO, &lcdTaskHandle);;
}

void lcd_manager_deinit(void)
{
    vQueueDelete(queLcdUpdate);
    vTaskDelete(lcdTaskHandle);
    bsp_lcd_display_off();
    lcd_enable = false;
}


extern uint8_t *lcd_get_frame_buffer_address(void);
void lcd_UpdateRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    #if 0
#if 1
    hal_display_lcd_layer_input_t lcm_para;
    uint8_t *frame_buff;

    frame_buff = lcd_get_frame_buffer_address();

    lcm_para.source_key_flag = 0;
    lcm_para.alpha_flag = 0;
    lcm_para.color_format = HAL_DISPLAY_LCD_LAYER_COLOR_RGB565;
    lcm_para.alpha = 0;
    lcm_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
    lcm_para.row_size = 240;
    lcm_para.column_size = 240;
    lcm_para.window_x_offset = 0;
    lcm_para.window_y_offset = 0;
    lcm_para.layer_enable = HAL_DISPLAY_LCD_LAYER0;
    lcm_para.buffer_address = (uint32_t)frame_buff;
    lcm_para.pitch = 240 * 2;

    bsp_lcd_config_layer(&lcm_para);
#endif

    tLcdLimit evtSet;
    evtSet.event = LCD_EVENT_UPDATE;
    evtSet.start_x = sx;
    evtSet.start_y = sy;
    evtSet.end_x = ex;
    evtSet.end_y = ey;
    xQueueSend(queLcdUpdate, &evtSet, 0);
    #endif
}

void lcd_DisplayOn(void)
{
    //LCD_INFO("lcd_DisplayOn %d \n\r",lcd_enable);
    tLcdLimit evtSet;
    if (!bl_en) {
        evtSet.event = LCD_EVENT_ON;
        xQueueSend(queLcdUpdate, &evtSet, 0);
    }
}

void lcd_DisplayOff(void)
{
    tLcdLimit evtSet;
    //LCD_INFO("lcd_DisplayOff %d \n\r",lcd_enable);
    if (bl_en) {
        evtSet.event = LCD_EVENT_OFF;
        xQueueSend(queLcdUpdate, &evtSet, 0);
    }
}

void lcd_mgr_task(void *pvParameters)
{
    tLcdLimit evtGet;
    //LCD_INFO("lcd manager task......\r\n");
    while (1) {
        //LCD_INFO("lcd manager task wait to receive queue......\r\n");
        if (pdTRUE == xQueueReceive(queLcdUpdate, &evtGet, portMAX_DELAY)) {
            //LCD_INFO("event=%d,start_x=%d,start_y=%d,end_x=%d,end_y=%d\r\n",evtGet.event,evtGet.start_x, evtGet.start_y, evtGet.end_x, evtGet.end_y);
            switch (evtGet.event) {
                case LCD_EVENT_ON:
                    bsp_lcd_display_on();
                    lcd_enable = true;
					//disp_update_rect(0, 0, LCD_WIDTH, LCD_HEIGHT);
					lcd_UpdateRect(0, 0, LCD_WIDTH, LCD_HEIGHT);
                    break;
                case LCD_EVENT_OFF:
                    lcd_enable = false;
                    bsp_lcd_display_off();
                    break;
                case LCD_EVENT_UPDATE:
                    if (lcd_enable) {
                        disp_update_rect(evtGet.start_x, evtGet.start_y, evtGet.end_x, evtGet.end_y);
                    }
                    break;
            }
        }
    }
}
