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

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

#include "mt25x3_hdk_lcd.h"
#include "hal_gpt.h"
#include "hal_gpio.h"
#include "bsp_lcd.h"
#include "memory_attribute.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HEIGHT 240
#define WIDTH 240

extern volatile LCD_REGISTER_T	*lcd_register_ptr;
uint32_t ranesis_delay_ST7301;

#define MAIN_LCD_CMD_ADDR		LCD_SERIAL0_A0_LOW_ADDR
#define MAIN_LCD_DATA_ADDR		LCD_SERIAL0_A0_HIGH_ADDR
#define LCD_DataRead_ST7301    (*(volatile uint8_t *)MAIN_LCD_DATA_ADDR);

static lcm_config_para_t ST7301_para =
{
    .type = LCM_INTERFACE_TYPE_DBI,
    .backlight_type = BACKLIGHT_TYPE_ISINK,
    .main_command_address = LCD_SERIAL0_A0_LOW_ADDR,
	.main_data_address = LCD_SERIAL0_A0_HIGH_ADDR,
	.main_lcd_output = LCM_8BIT_24_BPP_RGB888_2,
    .output_pixel_width = 8,
#ifdef MTK_TE_ENABLE
    .te_enable = false,
#endif
};

#if 1
#define LCD_CtrlWrite_ST7301(_data) \
{\
	*((volatile uint16_t *)MAIN_LCD_CMD_ADDR) = (_data&0x00FF);\
for(ranesis_delay_ST7301 = 20; ranesis_delay_ST7301 > 0; ranesis_delay_ST7301--);\
}

#define LCD_DataWrite_ST7301(_data) \
{\
	*((volatile uint16_t *)MAIN_LCD_DATA_ADDR) = (_data&0x00FF);\
for(ranesis_delay_ST7301 = 20; ranesis_delay_ST7301 > 0; ranesis_delay_ST7301--);\
}
#endif


void LCD_Init_Interface_ST7301(void)
{
    hal_display_lcd_interface_mode_t mode_settings;
    hal_display_lcd_interface_timing_t timing_settings;

    mode_settings.port_number = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
    mode_settings.three_wire_mode = 0;
    mode_settings.cs_stay_low_mode = 0;
    mode_settings.driving_current = HAL_DISPLAY_LCD_DRIVING_CURRENT_16MA;
    mode_settings.single_a0_mode = 0;
    mode_settings.read_from_SDI = 0;
    mode_settings.width = HAL_DISPLAY_LCD_INTERFACE_WIDTH_8;
    mode_settings.hw_cs = 1;
    mode_settings.power_domain = HAL_DISPLAY_LCD_POWER_DOMAIN_1V8;
    mode_settings.start_byte_mode = 0;

    hal_display_lcd_set_interface_mode(mode_settings);

    timing_settings.port_number = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
    timing_settings.csh = 1;
    timing_settings.css = 1;
    timing_settings.wr_low= 0xF;
    timing_settings.wr_high= 0xF;
    timing_settings.rd_low= 7;
    timing_settings.rd_high= 7;
    timing_settings.clock_freq= HAL_DISPLAY_LCD_INTERFACE_CLOCK_124MHZ;

    hal_display_lcd_set_interface_timing(timing_settings);
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

    //WRITE RAM 240x240
    LCD_CtrlWrite_ST7301(0x2A);   //Column Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0X3B);

    LCD_CtrlWrite_ST7301(0x2B);   //Row Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0XEF);

    LCD_CtrlWrite_ST7301(0x2C);

    for(LCD_y=0;LCD_y<=59;LCD_y++)
    {
        for(LCD_x=0;LCD_x<=239;LCD_x++)
        {
            *((volatile unsigned char *) MAIN_LCD_DATA_ADDR)=(uint8_t)(0xC0);
            *((volatile unsigned char *) MAIN_LCD_DATA_ADDR)=(uint8_t)(0xC0);	  
            *((volatile unsigned char *) MAIN_LCD_DATA_ADDR)=(uint8_t)(0xC0);
            *((volatile unsigned char *) MAIN_LCD_DATA_ADDR)=(uint8_t)(0xC0);	  
        }
    }
}

void LCD_ClearAll_ST7301(uint16_t data)
{
    LCD_blockClear_ST7301(0,0,HEIGHT-1,WIDTH-1,data);
}

void LCD_Init_ST7301(uint16_t bkground)
{
    // Do HW Reset
	hal_display_lcd_toggle_reset(10, 120);
    LCD_CtrlWrite_ST7301(0x11);
    hal_gpt_delay_ms(120);

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

    LCD_ClearAll_ST7301(0);
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
}
  
void LCD_BlockWrite_ST7301(uint16_t startx,uint16_t starty,uint16_t endx,uint16_t endy)
{
    uint8_t *buffer_addr;

	buffer_addr = (uint8_t *)hal_display_lcd_get_layer_address(HAL_DISPLAY_LCD_LAYER0);
    if(buffer_addr == NULL) {
        return; }

    LCD_transfer_framebuffer((uint16_t *)buffer_addr, 0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
    hal_display_lcd_set_layer_address(HAL_DISPLAY_LCD_LAYER0, (uint32_t)&R_LCD_Buffer);

    ENABLE_LCD_SERIAL_IF_HW_CS;
    DISABLE_LCD_SERIAL0_CS;

    LCD_CtrlWrite_ST7301(0x2A);   //Column Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0X3B);

    LCD_CtrlWrite_ST7301(0x2B);   //Row Address Setting
    LCD_DataWrite_ST7301(0X00);
    LCD_DataWrite_ST7301(0XEF);

    DISABLE_LCD_SERIAL_IF_HW_CS;
    ENABLE_LCD_SERIAL0_CS;

    LCD_CtrlWrite_ST7301(0x2C);

#ifdef MTK_TE_ENABLE
    if(ST7301_para.te_enable) {
        hal_display_lcd_start_dma(1);}
    else
#endif
    {
        hal_display_lcd_start_dma(0);
    }
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
			*(uint32_t *)(Parameters) = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
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
    uint16_t x1, x2, y1, y2;
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

            *((volatile unsigned char *) MAIN_LCD_DATA_ADDR)=(uint8_t)((data&0xFF00)>>8);
            *((volatile unsigned char *) MAIN_LCD_DATA_ADDR)=(uint8_t)(data&0xFF);	  
        }
    }
}

bool LCD_CheckID_ST7301(void)
{
    uint8_t data1, data2, data3;

    hal_pinmux_set_function(HAL_GPIO_39, 0); // CS
    
    hal_gpio_set_direction(HAL_GPIO_39,HAL_GPIO_DIRECTION_OUTPUT);

    // CS low
    hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_LOW);

    LCD_CtrlWrite_ST7301(0x04);   // Read ID command

    hal_pinmux_set_function(HAL_GPIO_40, 0); // LSCK
    hal_gpio_set_direction(HAL_GPIO_40,HAL_GPIO_DIRECTION_OUTPUT);

    hal_gpio_set_output(HAL_GPIO_40, HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(HAL_GPIO_40, HAL_GPIO_DATA_HIGH);
    hal_pinmux_set_function(HAL_GPIO_40, 1); // restore LSCK
    
	data1 = LCD_DataRead_ST7301;
	data2 = LCD_DataRead_ST7301;
	data3 = LCD_DataRead_ST7301;

    hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
    hal_pinmux_set_function(HAL_GPIO_39, 1); // restore CS

    hal_gpt_delay_ms(1);

    if((data1 != 0) && (data2 != 0) && (data3 != 0))
    {
        return true;
    }

    return false;
}

LCD_Funcs LCD_func_ST7301 = {
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

#endif
