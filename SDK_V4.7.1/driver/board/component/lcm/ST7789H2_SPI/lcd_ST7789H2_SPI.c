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

#include "hal_platform.h"
#include "hal_spi_master.h"
#include "hal_gpio.h"
#include "memory_attribute.h"

#include "mt2625_hdk_lcd.h"
#include "hal_gpt.h"
#include "hal_gpio.h"
#include "bsp_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HEIGHT 240
#define WIDTH 240

uint32_t ranesis_delay;

static volatile lcm_config_para_t ST7789H2_para =
{
    .type = LCM_INTERFACE_TYPE_DBI,
    .backlight_type = BACKLIGHT_TYPE_ISINK,
    .main_command_address = 0,
	.main_data_address = 0,
	.main_lcd_output = 0,
    .output_pixel_width = 16,
#ifdef MTK_TE_ENABLE
    .te_enable = true,
#endif
    .buffer_address = (uint32_t)NULL,
};

#define LCM_SPI_PORT HAL_SPI_MASTER_1
#define LCD_A0 HAL_GPIO_11
#define LCD_CS HAL_GPIO_19
#define LCD_RESET HAL_GPIO_35
#define LCD_CLOCK HAL_GPIO_22

hal_spi_master_config_t spi_config;

void LCD_CtrlWrite_ST7789H2(uint8_t cmd)
{
    hal_spi_master_send_polling(LCM_SPI_PORT, &cmd, 1);
}

void LCD_DataWrite_ST7789H2(uint8_t data)
{
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_HIGH);
    hal_spi_master_send_polling(LCM_SPI_PORT, &data, 1);
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_LOW);
}

void LCD_Init_Interface_ST7789H2(void)
{
     // SPI init
    uint8_t data[2] = {0x7E, 0x55};
    uint32_t size = 2;
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = 52000000;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;

    hal_gpio_init(HAL_GPIO_19);
    hal_gpio_init(HAL_GPIO_20);
    hal_gpio_init(HAL_GPIO_21);
    hal_gpio_init(HAL_GPIO_22);

    hal_pinmux_set_function(HAL_GPIO_19, HAL_GPIO_19_SPI_MST1_CS);// Set the pin to be used as CS signal of SPI.
    hal_pinmux_set_function(HAL_GPIO_22, HAL_GPIO_22_SPI_MST1_SCK);// Set the pin to be used as SCK signal of SPI.
    hal_pinmux_set_function(HAL_GPIO_20, HAL_GPIO_20_SPI_MST1_MISO);// Set the pin to be used as MOSI signal of SPI.
    hal_pinmux_set_function(HAL_GPIO_21, HAL_GPIO_21_SPI_MST1_MOSI);// Set the pin to be used as MISO signal of SPI.

    // Initialize the SPI master.
    if (HAL_SPI_MASTER_STATUS_OK == hal_spi_master_init(LCM_SPI_PORT,&spi_config)) {
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(LCM_SPI_PORT,data,size)) {
            // Error handler;
        }
    } else {
        // Error handler;
        while(1);
    }

    hal_gpio_init(LCD_A0);
    hal_pinmux_set_function(LCD_A0, 0);
    hal_gpio_set_direction(LCD_A0, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_LOW);
} /* init_lcd_interface() */

/***********************ST7789H2********************************/

void LCD_EnterSleep_ST7789H2(void)
{
    LCD_CtrlWrite_ST7789H2(0x10);
    hal_gpt_delay_ms(120); 
}

void LCD_ExitSleep_ST7789H2(void)
{
    LCD_CtrlWrite_ST7789H2(0x11);
    hal_gpt_delay_ms(10); // delay time
    LCD_CtrlWrite_ST7789H2(0x29);
    hal_gpt_delay_ms(120); 
}

void LCD_blockClear_ST7789H2(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t data)
{
    uint16_t LCD_x;
    uint16_t LCD_y;
    uint8_t transfer_data;

    LCD_CtrlWrite_ST7789H2(0x2A);
    LCD_DataWrite_ST7789H2((x1&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(x1&0xFF);
    LCD_DataWrite_ST7789H2((x2&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(x2&0xFF);

    LCD_CtrlWrite_ST7789H2(0x2B);
    LCD_DataWrite_ST7789H2((y1&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(y1&0xFF);
    LCD_DataWrite_ST7789H2((y2&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(y2&0xFF);

    hal_pinmux_set_function(LCD_CS, 0);// Set the pin to be used as CS signal of SPI.
    hal_gpio_set_direction(LCD_CS, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_HIGH);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_LOW);

    LCD_CtrlWrite_ST7789H2(0x2C);

    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_HIGH);
    for(LCD_y=y1;LCD_y<=y2;LCD_y++) {
        for(LCD_x=x1;LCD_x<=x2;LCD_x++) {
            transfer_data = (uint8_t)((data&0xFF00)>>8);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
            transfer_data = (uint8_t)(data&0xFF);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
         }
     }

    hal_gpio_set_output(LCD_A0, HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(LCD_CS, HAL_GPIO_DATA_HIGH);
    hal_pinmux_set_function(LCD_CS, HAL_GPIO_19_SPI_MST1_CS);// Set the pin to be used as CS signal of SPI.    
}

void LCD_ClearAll_ST7789H2(uint16_t data)
{
    LCD_blockClear_ST7789H2(0,0,HEIGHT-1,WIDTH-1,data);
}

void LCD_Init_ST7789H2(uint16_t bkground)
{
    // Do HW Reset
//	hal_display_lcd_toggle_reset(10, 120);
    hal_gpio_init(LCD_RESET);
    hal_pinmux_set_function(LCD_RESET, 0);
    hal_gpio_set_direction(LCD_RESET, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_RESET , HAL_GPIO_DATA_HIGH);
    hal_gpt_delay_ms(120); 
    hal_gpio_set_output(LCD_RESET , HAL_GPIO_DATA_LOW);
    hal_gpt_delay_ms(10); 
    hal_gpio_set_output(LCD_RESET , HAL_GPIO_DATA_HIGH);

    LCD_CtrlWrite_ST7789H2(0x11);
    hal_gpt_delay_ms(120);

    LCD_CtrlWrite_ST7789H2(0x36);
    LCD_DataWrite_ST7789H2(0x00);// C0 40 60

    LCD_CtrlWrite_ST7789H2(0x35);
    LCD_DataWrite_ST7789H2(0x00); //te on

    LCD_CtrlWrite_ST7789H2(0x21);

    LCD_CtrlWrite_ST7789H2(0x2a);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0xef);

    LCD_CtrlWrite_ST7789H2(0x2b);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0xef);

    LCD_CtrlWrite_ST7789H2(0x3A);
    LCD_DataWrite_ST7789H2(0x55);

    LCD_CtrlWrite_ST7789H2(0xB2);
    LCD_DataWrite_ST7789H2(0x1C);
    LCD_DataWrite_ST7789H2(0x1C);
    LCD_DataWrite_ST7789H2(0x01);
    LCD_DataWrite_ST7789H2(0xFF);
    LCD_DataWrite_ST7789H2(0x33);


    LCD_CtrlWrite_ST7789H2(0xB3);
    LCD_DataWrite_ST7789H2(0x10);
    LCD_DataWrite_ST7789H2(0xFF);	  // 0x05
    LCD_DataWrite_ST7789H2(0x0F);

    LCD_CtrlWrite_ST7789H2(0xB4);
    LCD_DataWrite_ST7789H2(0x0B);

    LCD_CtrlWrite_ST7789H2(0xB5);
    LCD_DataWrite_ST7789H2(0x9F);

    LCD_CtrlWrite_ST7789H2(0xB7);
    LCD_DataWrite_ST7789H2(0x35);

    LCD_CtrlWrite_ST7789H2(0xBB);
    LCD_DataWrite_ST7789H2(0x28);

    LCD_CtrlWrite_ST7789H2(0xBC);
    LCD_DataWrite_ST7789H2(0xEC);

    LCD_CtrlWrite_ST7789H2(0xBD);
    LCD_DataWrite_ST7789H2(0xFE);

    LCD_CtrlWrite_ST7789H2(0xC0);
    LCD_DataWrite_ST7789H2(0x2C);

    LCD_CtrlWrite_ST7789H2(0xC2);
    LCD_DataWrite_ST7789H2(0x01);

    LCD_CtrlWrite_ST7789H2(0xC3);
    LCD_DataWrite_ST7789H2(0x1E); 

    LCD_CtrlWrite_ST7789H2(0xC4);
    LCD_DataWrite_ST7789H2(0x20);

    LCD_CtrlWrite_ST7789H2(0xC6);		  // Normal mode frame rate
    LCD_DataWrite_ST7789H2(0x0F);	// 60Hz while FPA and BPA = 0x0C

    LCD_CtrlWrite_ST7789H2(0xD0);
    LCD_DataWrite_ST7789H2(0xA4);
    LCD_DataWrite_ST7789H2(0xA1);

    LCD_CtrlWrite_ST7789H2(0xE0);
    LCD_DataWrite_ST7789H2(0xD0);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x08);
    LCD_DataWrite_ST7789H2(0x07);
    LCD_DataWrite_ST7789H2(0x05);
    LCD_DataWrite_ST7789H2(0x29);
    LCD_DataWrite_ST7789H2(0x54);
    LCD_DataWrite_ST7789H2(0x41);
    LCD_DataWrite_ST7789H2(0x3C);
    LCD_DataWrite_ST7789H2(0x17);
    LCD_DataWrite_ST7789H2(0x15);
    LCD_DataWrite_ST7789H2(0x1A);
    LCD_DataWrite_ST7789H2(0x20);

    LCD_CtrlWrite_ST7789H2(0xE1);
    LCD_DataWrite_ST7789H2(0xD0);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x00);
    LCD_DataWrite_ST7789H2(0x08);
    LCD_DataWrite_ST7789H2(0x07);
    LCD_DataWrite_ST7789H2(0x04);
    LCD_DataWrite_ST7789H2(0x29);
    LCD_DataWrite_ST7789H2(0x44);
    LCD_DataWrite_ST7789H2(0x42);
    LCD_DataWrite_ST7789H2(0x3B);
    LCD_DataWrite_ST7789H2(0x16);
    LCD_DataWrite_ST7789H2(0x15);
    LCD_DataWrite_ST7789H2(0x1B);
    LCD_DataWrite_ST7789H2(0x1F);
    	
    // clear the screen with black color
    LCD_ClearAll_ST7789H2(bkground);

    // display on
    LCD_CtrlWrite_ST7789H2(0x29);
    hal_gpt_delay_ms(120);
}

void LCD_EnterIdle_ST7789H2(void)
{
	LCD_CtrlWrite_ST7789H2(0x39);
}

void LCD_ExitIdle_ST7789H2(void)
{
	LCD_CtrlWrite_ST7789H2(0x38);
}

void LCD_BlockWrite_ST7789H2(uint16_t startx,uint16_t starty,uint16_t endx,uint16_t endy)
{
    LCD_CtrlWrite_ST7789H2(0x2A);
    LCD_DataWrite_ST7789H2((startx&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(startx&0xFF);
    LCD_DataWrite_ST7789H2((endx&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(endx&0xFF);

    LCD_CtrlWrite_ST7789H2(0x2B);
    LCD_DataWrite_ST7789H2((starty&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(starty&0xFF);
    LCD_DataWrite_ST7789H2((endy&0xFF00)>>8);
    LCD_DataWrite_ST7789H2(endy&0xFF);

    LCD_CtrlWrite_ST7789H2(0x2C);

    hal_pinmux_set_function(LCD_CS, 0);// Set the pin to be used as CS signal of SPI.
    hal_gpio_set_direction(LCD_CS, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);

    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_HIGH);

    hal_spi_master_send_dma(LCM_SPI_PORT, (uint8_t *)ST7789H2_para.buffer_address, 240*240*2);
}


/***************************** Take Notice *******************************
*************************************************************************/ 
 
void LCD_IOCTRL_ST7789H2(LCM_IOCTRL_ID_ENUM ID, void* Parameters)
{
	switch (ID)
	{
		case LCM_IOCTRL_QUERY__FRAME_RATE:
			*((uint32_t*)(Parameters)) = 60;
			break;

		case LCM_IOCTRL_QUERY__BACK_PORCH:
			*(uint32_t *)(Parameters) = 12;
			break;
		    
		case LCM_IOCTRL_QUERY__FRONT_PORCH:
			*(uint32_t *)(Parameters) = 12;
			break;

		case LCM_IOCTRL_QUERY__IF_CS_NUMBER:
			*(uint32_t *)(Parameters) = 0;//HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
			break;

		case LCM_IOCTRL_QUERY__LCM_WIDTH:
			*((uint32_t *)(Parameters)) = 240;// physical, not use app's LCD_WIDTH
			break;

		case LCM_IOCTRL_QUERY__LCM_HEIGHT:
			*((uint32_t *)(Parameters)) = 240;//320;// physical, not use app's LCD_HEIGHT
			break;

        case LCM_IOCTRL_QUERY__LCM_SETTING_PARAM:
            *((lcm_config_para_t *)(Parameters)) = ST7789H2_para; // return LCM_para
            break;
#if 0
		//#ifdef __LCD_ESD_RECOVERY__
		/** LCM ESD feature control, Check LCD ESD is happend or not **/
		case LCM_IOCTRL_ESD_CHECK:
		{
		LCM_ESD_INFO_STRUCT* pESDInfo = (LCM_ESD_INFO_STRUCT *)Parameters;
		kal_bool lcm_esd_test = KAL_FALSE;

		lcm_esd_test = LCD_Read_ESD_ST7789H2();

		if(pESDInfo)
		{
		 //TODO: add ESD real check function here
		 // If happend, must take lcm_esd_test = KAL_TURE;
		 if(lcm_esd_test)
		 {
		  lcm_esd_test = KAL_FALSE;
		  pESDInfo->bEsdHappened = KAL_TRUE;
		 }
		 else
		  pESDInfo->bEsdHappened = KAL_FALSE;
		}

		return LCM_IOCTRL_OK;
		break;
		}
			   
		/** LCM ESD feature control, If LCD ESD is happend, We should do something here like resetLCM. **/
		case LCM_IOCTRL_ESD_RESET:
		//TODO: add LCM reset function here
		//Example
		LCD_Init_ST7789H2(0,0);

		break;  
#endif

#ifdef MTK_TE_ENABLE
        case LCM_IOCTRL_SET_TE:
            ST7789H2_para.te_enable = *((bool *)(Parameters));
            break;
#endif
        case LCM_IOCTRL_SET_FB:
            ST7789H2_para.buffer_address = (uint32_t)Parameters;
            break;
		default:
		    break;
	}
}

void LCD_ClearScreen_ST7789H2(uint16_t color)
{
    LCD_ClearAll_ST7789H2(color);
}

void LCD_ClearScreenBW_ST7789H2(void)
{
    uint16_t LCD_x;
    uint16_t LCD_y;
    uint8_t x1, x2, y1, y2;
    uint32_t data;

    x1 = 0;
    x2 = WIDTH-1;
    y1 = 0;
    y2 = HEIGHT-1;
	  
	LCD_CtrlWrite_ST7789H2(0x2A);
	LCD_DataWrite_ST7789H2((x1&0xFF00)>>8);
	LCD_DataWrite_ST7789H2(x1&0xFF);
	LCD_DataWrite_ST7789H2((x2&0xFF00)>>8);
	LCD_DataWrite_ST7789H2(x2&0xFF);
	  
	LCD_CtrlWrite_ST7789H2(0x2B);
	LCD_DataWrite_ST7789H2((y1&0xFF00)>>8);
	LCD_DataWrite_ST7789H2(y1&0xFF);
	LCD_DataWrite_ST7789H2((y2&0xFF00)>>8);
	LCD_DataWrite_ST7789H2(y2&0xFF);
	  
	LCD_CtrlWrite_ST7789H2(0x2C);
	
	for(LCD_y=y1;LCD_y<=y2;LCD_y++)
    {
        for(LCD_x=x1;LCD_x<=(x2);LCD_x++)
        {
            if(LCD_y < y2/2){
                data = 0xFFFF;
            }
            else{
                data = 0x0;
            }

            LCD_DataWrite_ST7789H2((uint8_t)((data&0xFF00)>>8));
            LCD_DataWrite_ST7789H2((uint8_t)(data&0xFF));
        }
    }

}

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t spi_send_data;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t spi_read_data[4];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t LCM_id[3];
extern void bsp_spi_callback (hal_spi_master_callback_event_t event,void *user_data);
bool LCD_CheckID_ST7789H2(void)
{
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    spi_config.clock_frequency = 1300000;
    hal_spi_master_deinit(LCM_SPI_PORT);
    hal_spi_master_init(LCM_SPI_PORT,&spi_config);

    hal_pinmux_set_function(LCD_CS, 0);// Set the pin to be used as CS signal of SPI.
    hal_gpio_set_direction(LCD_CS, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_HIGH);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_LOW);

    spi_send_data = 0xDA;

    spi_send_and_receive_config.send_data = &spi_send_data;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.receive_buffer = spi_read_data;
    spi_send_and_receive_config.receive_length = 2;

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(LCM_SPI_PORT, &spi_send_and_receive_config)) {
    }

    LCM_id[0] = spi_read_data[1];

    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_HIGH);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);

    spi_send_data = 0xDB;

    spi_send_and_receive_config.send_data = &spi_send_data;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.receive_buffer = spi_read_data;
    spi_send_and_receive_config.receive_length = 2;

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(LCM_SPI_PORT, &spi_send_and_receive_config)) {
    }

    LCM_id[1] = spi_read_data[1];

    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_HIGH);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);
    spi_send_data = 0xDC;

    spi_send_and_receive_config.send_data = &spi_send_data;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.receive_buffer = spi_read_data;
    spi_send_and_receive_config.receive_length = 2;

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(LCM_SPI_PORT, &spi_send_and_receive_config)) {
    }

    LCM_id[2] = spi_read_data[1];
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_HIGH);

    hal_pinmux_set_function(LCD_CS, HAL_GPIO_19_SPI_MST1_CS);// Set the pin to be used as CS signal of SPI.

    spi_config.clock_frequency = 52000000;
    hal_spi_master_deinit(LCM_SPI_PORT);
    hal_spi_master_init(LCM_SPI_PORT,&spi_config);
    hal_spi_master_register_callback(HAL_SPI_MASTER_1, (hal_spi_master_callback_t)bsp_spi_callback, NULL);

    if((LCM_id[0] != 0) && (LCM_id[1] != 0) && (LCM_id[2] != 0))
        return true;

    return false;
}

LCD_Funcs LCD_func_ST7789H2_SPI = {
	LCD_Init_ST7789H2,
	LCD_Init_Interface_ST7789H2,
	LCD_BlockWrite_ST7789H2,
	LCD_EnterSleep_ST7789H2,
	LCD_ExitSleep_ST7789H2,
	LCD_EnterIdle_ST7789H2,
	LCD_ExitIdle_ST7789H2,
	LCD_ClearScreen_ST7789H2,
	LCD_ClearScreenBW_ST7789H2,
	LCD_IOCTRL_ST7789H2,
	LCD_CheckID_ST7789H2
};

#ifdef __cplusplus
}
#endif
