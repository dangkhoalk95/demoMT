/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
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

static volatile lcm_config_para_t ST7301_para =
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
    .buffer_address = NULL,
};

#define LCM_SPI_PORT HAL_SPI_MASTER_1
#define LCD_A0 HAL_GPIO_11
#define LCD_CS HAL_GPIO_19
#define LCD_RESET HAL_GPIO_35
#define LCD_CLOCK HAL_GPIO_22

hal_spi_master_config_t spi_config_st7301;

void LCD_CtrlWrite_ST7301(uint8_t cmd)
{
    hal_spi_master_send_polling(LCM_SPI_PORT, &cmd, 1);
}

void LCD_DataWrite_ST7301(uint8_t data)
{
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_HIGH);
    hal_spi_master_send_polling(LCM_SPI_PORT, &data, 1);
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_LOW);
}

void LCD_Init_Interface_ST7301(void)
{
     // SPI init
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    uint8_t status_receive[2];
    uint8_t data[2] = {0x7E, 0x55};
    uint32_t size = 2;
    spi_config_st7301.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config_st7301.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config_st7301.clock_frequency = 13000000;
    spi_config_st7301.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config_st7301.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    status_receive[1] = 0;

    hal_gpio_init(HAL_GPIO_19);
    hal_gpio_init(HAL_GPIO_20);
    hal_gpio_init(HAL_GPIO_21);
    hal_gpio_init(HAL_GPIO_22);

    hal_pinmux_set_function(HAL_GPIO_19, HAL_GPIO_19_SPI_MST1_CS);// Set the pin to be used as CS signal of SPI.
    hal_pinmux_set_function(HAL_GPIO_22, HAL_GPIO_22_SPI_MST1_SCK);// Set the pin to be used as SCK signal of SPI.
    hal_pinmux_set_function(HAL_GPIO_20, HAL_GPIO_20_SPI_MST1_MISO);// Set the pin to be used as MOSI signal of SPI.
    hal_pinmux_set_function(HAL_GPIO_21, HAL_GPIO_21_SPI_MST1_MOSI);// Set the pin to be used as MISO signal of SPI.

    // Initialize the SPI master.
    if (HAL_SPI_MASTER_STATUS_OK == hal_spi_master_init(LCM_SPI_PORT,&spi_config_st7301)) {
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

/***********************ST7301********************************/

void LCD_EnterSleep_ST7301(void)
{
    LCD_CtrlWrite_ST7301(0x10);
    hal_gpt_delay_ms(120); 
}

void LCD_ExitSleep_ST7301(void)
{
    LCD_CtrlWrite_ST7301(0x11);
    hal_gpt_delay_ms(10); // delay time
    LCD_CtrlWrite_ST7301(0x29);
    hal_gpt_delay_ms(120); 
}

void LCD_blockClear_ST7301(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t data)
{
    uint16_t LCD_x;
    uint16_t LCD_y;
    uint8_t transfer_data;
/*
    LCD_CtrlWrite_ST7301(0x2A);
    LCD_DataWrite_ST7301((x1&0xFF00)>>8);
    LCD_DataWrite_ST7301(x1&0xFF);
    LCD_DataWrite_ST7301((x2&0xFF00)>>8);
    LCD_DataWrite_ST7301(x2&0xFF);

    LCD_CtrlWrite_ST7301(0x2B);
    LCD_DataWrite_ST7301((y1&0xFF00)>>8);
    LCD_DataWrite_ST7301(y1&0xFF);
    LCD_DataWrite_ST7301((y2&0xFF00)>>8);
    LCD_DataWrite_ST7301(y2&0xFF);
*/

    //WRITE RAM 240x240
    LCD_CtrlWrite_ST7301(0x2A);   //Column Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0X3B);

    LCD_CtrlWrite_ST7301(0x2B);   //Row Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0XEF);

    hal_pinmux_set_function(LCD_CS, 0);// Set the pin to be used as CS signal of SPI.
    hal_gpio_set_direction(LCD_CS, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_HIGH);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_LOW);

    LCD_CtrlWrite_ST7301(0x2C);

    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_HIGH);

    transfer_data = 0xC0;


    for(LCD_y=0;LCD_y<=59;LCD_y++)
    {
        for(LCD_x=0;LCD_x<=239;LCD_x++)
        {
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
        }
    }
/*
    for(LCD_y=y1;LCD_y<=y2;LCD_y++) {
        for(LCD_x=x1;LCD_x<=x2;LCD_x++) {
            transfer_data = (uint8_t)((data&0xFF00)>>8);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
            transfer_data = (uint8_t)(data&0xFF);
            hal_spi_master_send_polling(LCM_SPI_PORT, &transfer_data, 1);
         }
     }
*/
    hal_gpio_set_output(LCD_A0, HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(LCD_CS, HAL_GPIO_DATA_HIGH);
    hal_pinmux_set_function(LCD_CS, HAL_GPIO_19_SPI_MST1_CS);// Set the pin to be used as CS signal of SPI.    
}

void LCD_ClearAll_ST7301(uint16_t data)
{
    LCD_blockClear_ST7301(0,0,HEIGHT-1,WIDTH-1,data);
}

void LCD_Init_ST7301(uint16_t bkground)
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

    LCD_CtrlWrite_ST7301(0xEB);
    LCD_DataWrite_ST7301(0x02);

    LCD_CtrlWrite_ST7301(0xD7);
    LCD_DataWrite_ST7301(0x01);

    LCD_CtrlWrite_ST7301(0xD1);   //Auto Power Control
    LCD_DataWrite_ST7301(0X01);  //

    LCD_CtrlWrite_ST7301(0xE8);   //Test Setting for Memory
    LCD_DataWrite_ST7301(0XFF);  //Timing Adjust
    LCD_DataWrite_ST7301(0X0F);  //

    LCD_CtrlWrite_ST7301(0xC0);   //Gate Voltage Setting
    LCD_DataWrite_ST7301(000);  //VGH=10V ; VGL=-8V

    LCD_CtrlWrite_ST7301(0xC1);   //Source Voltage Setting
    LCD_DataWrite_ST7301(0X28);  //VSH=5V
    LCD_DataWrite_ST7301(0X10);  //VSL=2.3V
    LCD_DataWrite_ST7301(0X14);  //VSI=2.5V
    LCD_DataWrite_ST7301(0X00);  //VSK=1.5V

    LCD_CtrlWrite_ST7301(0xC2);   //VCOMH Setting
    LCD_DataWrite_ST7301(0X14);  //VCOMH1=4V
    LCD_DataWrite_ST7301(0X14);  //VCOMH2=4V
    LCD_DataWrite_ST7301(0X14);  //VCOMH3=4V
    LCD_DataWrite_ST7301(0X14);  //VCOMH4=4V

    LCD_CtrlWrite_ST7301(0xB4);   //Gate EQ
    LCD_DataWrite_ST7301(0XE5);  //
    LCD_DataWrite_ST7301(0X00);  //
    LCD_DataWrite_ST7301(0XFF);  //HPM EQ
    LCD_DataWrite_ST7301(0XFF);  //
    LCD_DataWrite_ST7301(0XF5);  //
    LCD_DataWrite_ST7301(0X55);  //
    LCD_DataWrite_ST7301(0XFF);  //LPM EQ
    LCD_DataWrite_ST7301(0XFF);  //
    LCD_DataWrite_ST7301(0X2B);  //
    LCD_DataWrite_ST7301(0X54);  //

    LCD_CtrlWrite_ST7301(0x11);   //Sleep out 
    hal_gpt_delay_ms(100);

    LCD_CtrlWrite_ST7301(0xC7);   //OSC Setting
    LCD_DataWrite_ST7301(0X14);
    LCD_DataWrite_ST7301(0X14);

    LCD_CtrlWrite_ST7301(0xC7);   //OSC Setting
    LCD_DataWrite_ST7301(0XA0);
    LCD_DataWrite_ST7301(0XA5);

    LCD_CtrlWrite_ST7301(0xCA);   //OSC Trim
    LCD_DataWrite_ST7301(0XB9);

    LCD_CtrlWrite_ST7301(0x36);   //MY=0, MX=0, MV=0, ML=0 
    LCD_DataWrite_ST7301(0X48);   // MX=1 ; DO=1

    LCD_CtrlWrite_ST7301(0x3A);   //64Color Mode 
    LCD_DataWrite_ST7301(0X00);  //10:4write for 24bit ; 11: 3write for 24bit

    LCD_CtrlWrite_ST7301(0xBD);   //De-stress Mode
    LCD_DataWrite_ST7301(0X00);  //ST Waveform

    LCD_CtrlWrite_ST7301(0xB9);   //Source Setting
    LCD_DataWrite_ST7301(0X23);  //Pixel Cutting

    LCD_CtrlWrite_ST7301(0xB8);   //Source Setting
    LCD_DataWrite_ST7301(0X08);  //Pixel Cutting

    //WRITE RAM 240x240
    LCD_CtrlWrite_ST7301(0x2A);   //Column Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0X3B);

    LCD_CtrlWrite_ST7301(0x2B);   //Row Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0XEF);

    LCD_CtrlWrite_ST7301(0x29);   //DISPLAY ON
    LCD_CtrlWrite_ST7301(0x39);   //LPM

    while(1){
    LCD_ClearAll_ST7301(0);};
}

void LCD_EnterIdle_ST7301(void)
{
	LCD_CtrlWrite_ST7301(0x39);
}

void LCD_ExitIdle_ST7301(void)
{
	LCD_CtrlWrite_ST7301(0x38);
}

#define LCD_WIDTH (240)
#define LCD_HEIGHT (240)
#define D_LCD_Layer_Size    (LCD_WIDTH*LCD_HEIGHT)

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t R_LCD_Buffer[D_LCD_Layer_Size];

void LCD_transfer_framebuffer(uint16_t * pBuf, uint16_t wX1, uint16_t wY1, uint16_t wX2, uint16_t wY2)
{
    uint16_t wX, wY;
    uint32_t i;
    uint8_t tempR, tempG, tempB;

    memset(R_LCD_Buffer, 0, sizeof(R_LCD_Buffer));

    for (wY = wY1; wY <= wY2; wY++) {
        for (wX = wX1; wX <= wX2; wX++) {
            i = wY*LCD_WIDTH+wX;
            tempR = ((pBuf[i]&0xC000)>>8);
            tempG = ((pBuf[i]&0x600)>>5);
            tempB = ((pBuf[i]&0x18)>>1);
            R_LCD_Buffer[i] = (tempR | tempG | tempB);
        }
    }

/*
    for (wY = 0; wY <= 79; wY++) {
        for (wX = wX1; wX <= wX2; wX++) {
            i = wY*LCD_WIDTH+wX;
            tempR = ((0xFFFF&0xC000)>>8);
            tempG = ((0&0x600)>>5);
            tempB = ((0&0x18)>>1);
            R_LCD_Buffer[i] = (tempR | tempG | tempB);
        }
    }

for (wY = 80; wY <= 159; wY++) {
    for (wX = wX1; wX <= wX2; wX++) {
        i = wY*LCD_WIDTH+wX;
        tempR = ((0&0xC000)>>8);
        tempG = ((0xFFFF&0x600)>>5);
        tempB = ((0&0x18)>>1);
        R_LCD_Buffer[i] = (tempR | tempG | tempB);
    }
}

for (wY = 160; wY <= 239; wY++) {
    for (wX = wX1; wX <= wX2; wX++) {
        i = wY*LCD_WIDTH+wX;
        tempR = ((0&0xC000)>>8);
        tempG = ((0&0x600)>>5);
        tempB = ((0xFFFF&0x18)>>1);
        R_LCD_Buffer[i] = (tempR | tempG | tempB);
    }
}
*/
}
  

void LCD_BlockWrite_ST7301(uint16_t startx,uint16_t starty,uint16_t endx,uint16_t endy)
{
    uint8_t *buffer_addr;

	buffer_addr = (uint8_t *)ST7301_para.buffer_address;
    if(buffer_addr == NULL) {
        return; }

    LCD_transfer_framebuffer(buffer_addr, 0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);

/*
    LCD_CtrlWrite_ST7301(0x2A);
    LCD_DataWrite_ST7301((startx&0xFF00)>>8);
    LCD_DataWrite_ST7301(startx&0xFF);
    LCD_DataWrite_ST7301((endx&0xFF00)>>8);
    LCD_DataWrite_ST7301(endx&0xFF);

    LCD_CtrlWrite_ST7301(0x2B);
    LCD_DataWrite_ST7301((starty&0xFF00)>>8);
    LCD_DataWrite_ST7301(starty&0xFF);
    LCD_DataWrite_ST7301((endy&0xFF00)>>8);
    LCD_DataWrite_ST7301(endy&0xFF);

    LCD_CtrlWrite_ST7301(0x2C);
*/
    LCD_CtrlWrite_ST7301(0x2A);   //Column Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0X3B);

    LCD_CtrlWrite_ST7301(0x2B);   //Row Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0XEF);

    hal_pinmux_set_function(LCD_CS, 0);// Set the pin to be used as CS signal of SPI.
    hal_gpio_set_direction(LCD_CS, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(LCD_CS , HAL_GPIO_DATA_LOW);

    LCD_CtrlWrite_ST7301(0x2C);

    hal_gpio_set_output(LCD_A0 , HAL_GPIO_DATA_HIGH);

    hal_spi_master_send_dma(LCM_SPI_PORT, ST7301_para.buffer_address, 240*240);
}


/***************************** Take Notice *******************************
*************************************************************************/ 
 
void LCD_IOCTRL_ST7301(LCM_IOCTRL_ID_ENUM ID, void* Parameters)
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
            *((lcm_config_para_t *)(Parameters)) = ST7301_para; // return LCM_para
            break;
#if 0
		//#ifdef __LCD_ESD_RECOVERY__
		/** LCM ESD feature control, Check LCD ESD is happend or not **/
		case LCM_IOCTRL_ESD_CHECK:
		{
		LCM_ESD_INFO_STRUCT* pESDInfo = (LCM_ESD_INFO_STRUCT *)Parameters;
		kal_bool lcm_esd_test = KAL_FALSE;

		lcm_esd_test = LCD_Read_ESD_ST7301();

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
		LCD_Init_ST7301(0,0);

		break;  
#endif

#ifdef MTK_TE_ENABLE
        case LCM_IOCTRL_SET_TE:
            ST7301_para.te_enable = *((bool *)(Parameters));
            break;
#endif
        case LCM_IOCTRL_SET_FB:
            ST7301_para.buffer_address = (uint32_t)Parameters;
            break;
		default:
		    break;
	}
}

void LCD_ClearScreen_ST7301(uint16_t color)
{
    LCD_ClearAll_ST7301(color);
}

void LCD_ClearScreenBW_ST7301(void)
{
    uint16_t LCD_x;
    uint16_t LCD_y;
    uint8_t x1, x2, y1, y2;
    uint32_t data;

    x1 = 0;
    x2 = WIDTH-1;
    y1 = 0;
    y2 = HEIGHT-1;
	  
	LCD_CtrlWrite_ST7301(0x2A);
	LCD_DataWrite_ST7301((x1&0xFF00)>>8);
	LCD_DataWrite_ST7301(x1&0xFF);
	LCD_DataWrite_ST7301((x2&0xFF00)>>8);
	LCD_DataWrite_ST7301(x2&0xFF);
	  
	LCD_CtrlWrite_ST7301(0x2B);
	LCD_DataWrite_ST7301((y1&0xFF00)>>8);
	LCD_DataWrite_ST7301(y1&0xFF);
	LCD_DataWrite_ST7301((y2&0xFF00)>>8);
	LCD_DataWrite_ST7301(y2&0xFF);
	  
	LCD_CtrlWrite_ST7301(0x2C);
	
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

            LCD_DataWrite_ST7301((uint8_t)((data&0xFF00)>>8));
            LCD_DataWrite_ST7301((uint8_t)(data&0xFF));
        }
    }

}

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t spi_send_data;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t spi_read_data[4];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t LCM_id[3];
extern void bsp_spi_callback (hal_spi_master_callback_event_t event,void *user_data);
bool LCD_CheckID_ST7301(void)
{
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    spi_config_st7301.clock_frequency = 1300000;
    hal_spi_master_deinit(LCM_SPI_PORT);
    hal_spi_master_init(LCM_SPI_PORT,&spi_config_st7301);

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

    spi_config_st7301.clock_frequency = 13000000;
    hal_spi_master_deinit(LCM_SPI_PORT);
    hal_spi_master_init(LCM_SPI_PORT,&spi_config_st7301);
    hal_spi_master_register_callback(HAL_SPI_MASTER_1, (hal_spi_master_callback_t)bsp_spi_callback, NULL);

    if((LCM_id[0] != 0) && (LCM_id[1] != 0) && (LCM_id[2] != 0))
        return true;

    return false;
}

LCD_Funcs LCD_func_ST7301_SPI = {
	LCD_Init_ST7301,
	LCD_Init_Interface_ST7301,
	LCD_BlockWrite_ST7301,
	LCD_EnterSleep_ST7301,
	LCD_ExitSleep_ST7301,
	LCD_EnterIdle_ST7301,
	LCD_ExitIdle_ST7301,
	LCD_ClearScreen_ST7301,
	LCD_ClearScreenBW_ST7301,
	LCD_IOCTRL_ST7301,
	LCD_CheckID_ST7301
};

#ifdef __cplusplus
}
#endif
