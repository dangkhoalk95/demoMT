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
#include "lcd_config.h"
#include "bw_gdi_api.h"
#include "zimo.h"
#include "mt25x3_hdk_lcd.h"
#include <string.h>
#include "hal_log.h"
#if defined(__LCD_ROTATE_90__)
#define 	lcd_real_width  LCD_HEIGHT
#else
#define 	lcd_real_width  LCD_WIDTH
#endif
static UINT8 lcd_frame_buf[LCD_WIDTH * ((LCD_HEIGHT + 7) / 8)];
static LCD_FRAME_PTR lcd_frame_ptr = lcd_frame_buf;

/* LCD_WIDTH and LCD_HEIGHT is provided at compiling time. */
/* lcd_clip_width and lcd_clip_height are used to specify the right and bottom position for font
 * and bitmap display clip. By default it is LCD_WIDTH and LCD_HEIGHT but it can be adjusted to other values.
 * This data applies ONLY for bitmap and font. The clip width and height is related to (left,top)
 * i.e. (0,0) of LCD. The clip area is (0,0) - (lcd_clip_width-1, lcd_clip_height-1) */
#if defined(__LCD_ROTATE_90__)
static uint16_t lcd_page   = (LCD_WIDTH + 7) / 8;
static uint16_t lcd_width  = LCD_HEIGHT;
static uint16_t lcd_height = LCD_WIDTH;
static uint16_t lcd_clip_width = LCD_HEIGHT;
static uint16_t lcd_clip_height = LCD_WIDTH;
#else
static uint16_t lcd_page   = (LCD_HEIGHT + 7) / 8;
static uint16_t lcd_width  = LCD_WIDTH;
static uint16_t lcd_height = LCD_HEIGHT;
static uint16_t lcd_clip_width = LCD_WIDTH;
static uint16_t lcd_clip_height = LCD_HEIGHT;
#endif

/* Default foreground color. */
static color_type_t lcd_foreground_color = COLOR_BLACK;
/* Default background color. */
static color_type_t lcd_background_color = COLOR_WHITE;
/* Transparent mode for text display. */
static bool lcd_transparent_mode = false;
static disp_mode_t disp_mode = DISP_NORMAL;
/* Indicate if LCD initialized. */
//static bool lcd_initialized = false;
static const uint8_t start_mask0[] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80};
static const uint8_t stop_mask0[] = {0xff, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f};
static font_type_t lcd_font = FONT_16x16;

#if defined(__LCD_ROTATE_90__)
void lcd_exchage(uint16_t *x1, uint16_t *x2)
{
    uint16_t temp;
    temp = *x1;
    *x1 = *x2;
    *x2 = temp;
}
#endif

uint8_t lcd_get_page_count(uint8_t char_height, uint8_t y0)
{
    uint8_t base_page = (char_height + 7) >> 3;
    uint8_t mod = y0 % 8;
    uint8_t height_limit = base_page << 3;

    if ((mod + char_height) <= height_limit) {
        return base_page;
    } else {
        return base_page + 1;
    }
}

void disp_set_color(color_type_t color)
{
    lcd_foreground_color = color;
}

void disp_get_color(color_type_t *p_color)
{
    *p_color = lcd_foreground_color;
}

void disp_set_back_color(color_type_t color)
{
    lcd_background_color = color;
}

void disp_get_back_color(color_type_t *p_color)
{
    *p_color = lcd_background_color;
}

/* Draw mono bitmap on LCD with transparent mode:
 * the bit  with '1' will be filled with foreground color; the bit with '0' will be negelected.
 * And raster operaion mode will affect the display as well:
 * DISP_NORMAL: fill with foreground color for those bits = 1;
 * DISP_INVERSE: NOT operation on the color in frame buffer
 * DISP_XOR:
 * DISP_AND:
 * DISP_OR :
 *
 * Caveats: it is assumed that the bitmap data is BYTE alligned. for
 *          instance if each line contain 33 pixels then 5 bytes are
 *          needed (7 bits for dummy) */
bool disp_put_bitmap_1bit(disp_bitmap_t *p_bitmap, uint16_t x, uint16_t y)
{
    uint16_t width, height;
    uint16_t i, j;
    uint8_t *p_bmp;
    LCD_FRAME_PTR p_frame;
    uint8_t	base_page_count, page_count, start_page;
    uint8_t start_bit_index, stop_bit_index;
    uint16_t temp16_1, temp16_2;
    uint8_t old_data8, data8;
    uint8_t mask1, mask0;

#if defined(__LCD_ROTATE_90__)
    lcd_exchage(&x, &y);
    lcd_exchage(&(p_bitmap->width), &(p_bitmap->height));
#endif

    /* Sanity check: ensure the left-top position is within LCD screen or within
     clip area. By default the clip area is whole display but it can also be adjusted... */
    if ((x >= lcd_clip_width) || (y >= lcd_clip_height)) {
        return false;
    }

    /* Adjust width and height, if some part of the bitmap fall out of LCD screen */
    p_bmp  = (uint8_t *)p_bitmap->p_bitmap;
    width  = p_bitmap->width;
    height = p_bitmap->height;

    if ((x + width) > lcd_clip_width) {
        width = lcd_clip_width - x;
    }
    if ((y + height) > lcd_clip_height) {
        height = lcd_clip_height - y;
    }

    /* Calculate aligned page count according to the height, the actual page count and the start page index */
    base_page_count	= (height + 7) >> 3;
    page_count		= lcd_get_page_count((uint8_t)height, (uint8_t)y);
    start_page		= y >> 3;

    /* Calculate the start and stop bit index */
    start_bit_index = y & 0x07;
    stop_bit_index  = (height + start_bit_index) & 0x07;

    /* Now would perform the display, according to current LCD display mode */
    for (i = 0; i < width; i++) {
        if ((x + i) >= lcd_width) {
            continue;
        }

        temp16_1 = temp16_2 = 0;
        for (j = 0; j < page_count; j++) {
            if ((start_page + j) >= lcd_page) {
                break;
            }

            p_frame = lcd_frame_ptr + (start_page + j) * lcd_real_width + x + i;
            if (j == base_page_count) {
                data8 = (uint8_t)(temp16_2 & 0xff);
            } else {
                temp16_1 = *(p_bmp + p_bitmap->width * j + i);
                temp16_1 <<= start_bit_index;
                temp16_1 |=  temp16_2;
                data8    =   temp16_1 & 0xff;
                temp16_2 =   temp16_1 >> 8;
            }
            /* Fetch the old data from frame buffer */
            old_data8 = *p_frame;

            if (j == 0) {
                mask0 = start_mask0[start_bit_index];

                if (page_count == 1) {
                    mask0 &= stop_mask0[stop_bit_index];
                }
                mask1 = ~mask0;

                switch (disp_mode) {
                    case DISP_NORMAL:
                        if ((lcd_transparent_mode) && (lcd_foreground_color == COLOR_BLACK)) {
                            *p_frame = old_data8 | (data8 & mask0);
                        } else if ((lcd_transparent_mode) && (lcd_foreground_color == COLOR_WHITE)) {
                            uint8_t part_data = (~data8);
                            part_data &= mask0;
                            *p_frame = old_data8 & (~data8);
                        } else if ((!lcd_transparent_mode) && (lcd_foreground_color == COLOR_WHITE)) {
                            uint8_t part_data = (~data8);
                            part_data &= mask0;
                            *p_frame = (old_data8 & mask1) | (part_data);
                        } else {
                            *p_frame = (old_data8 & mask1) | (data8 & mask0);
                        }

                        break;
                    case DISP_XOR:
                        *p_frame = old_data8 ^ data8;
                        break;
                    default:
                        break;
                }
            } else if (j == (page_count - 1)) {
                mask0 = stop_mask0[stop_bit_index];
                mask1 = ~mask0;
                data8 &= mask0;

                switch (disp_mode) {
                    case DISP_NORMAL:
                        if ((lcd_transparent_mode) && (lcd_foreground_color == COLOR_BLACK)) {
                            *p_frame = old_data8 | data8;
                        } else if ((lcd_transparent_mode) && (lcd_foreground_color == COLOR_WHITE)) {
                            *p_frame = old_data8 & (~data8);
                        } else if ((!lcd_transparent_mode) && (lcd_foreground_color == COLOR_WHITE)) {
                            uint8_t part_data = ~data8;
                            part_data &= mask0;
                            *p_frame = (old_data8 & mask1) | (part_data);
                        } else {
                            *p_frame = (old_data8 & mask1) | data8;
                        }

                        break;
                    case DISP_XOR:
                        *p_frame = old_data8 ^ data8;
                        break;
                    default:
                        break;
                }
            } else {
                switch (disp_mode) {
                    case DISP_NORMAL:
                        if ((lcd_transparent_mode) && (lcd_foreground_color == COLOR_BLACK)) {
                            *p_frame = old_data8 | data8;
                        } else if ((lcd_transparent_mode) && (lcd_foreground_color == COLOR_WHITE)) {
                            *p_frame = old_data8 & (~data8);
                        } else if ((!lcd_transparent_mode) && (lcd_foreground_color == COLOR_WHITE)) {
                            *p_frame = ~data8;
                        } else {
                            *p_frame = data8;
                        }
                        break;
                    case DISP_XOR:
                        *p_frame = old_data8 ^ data8;
                        break;
                }
            }
        }
    }
    return true;
}

/* Clear screen area with given color */
bool disp_clear_rect_with_color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, color_type_t color)
{
    uint16_t i, j;
    uint16_t start_page_num, page_num;
    uint16_t start_bit_num, stop_bit_num;
    uint16_t height, width;
    LCD_FRAME_PTR p_frame;
    uint8_t mask2;

#if defined(__LCD_ROTATE_90__)
    lcd_exchage(&x1, &y1);
    lcd_exchage(&x2, &y2);
#endif

    if ((x1 > x2) || (y1 > y2)) {
        return false;
    }

    if (x1 >= lcd_width) {
        x1 = lcd_width - 1;
    }

    if (x2 >= lcd_width) {
        x2 = lcd_width - 1;
    }

    if (y1 >= lcd_height) {
        y1 = lcd_height - 1;
    }

    if (y2 >= lcd_height) {
        y2 = lcd_height - 1;
    }

    /* get RECT height and width */
    height = y2 - y1 + 1;
    width  = x2 - x1 + 1;

    page_num = lcd_get_page_count((uint8_t) height, (uint8_t)y1);

    start_page_num = y1 >> 3;
    start_bit_num  = y1 & 0x07;
    stop_bit_num   = (height + start_bit_num) & 0x07;

    p_frame = lcd_frame_ptr + start_page_num * lcd_real_width + x1;

    /* Copy first page data, begin. */
    mask2 = start_mask0[start_bit_num];
    if (page_num == 1) {
        mask2 &= stop_mask0[stop_bit_num];
    }
    for (i = 0; i < width; i++) {
        if (color == COLOR_WHITE) {
            *(p_frame + i) &= (~mask2);
        } else {
            *(p_frame + i) |= mask2;
        }
    }
    p_frame += lcd_real_width;
    /* Copy first page data, end. Move to next page. */

    /* Copy datas from the second page to the last second page, begin. */
    if (page_num > 2) {
        for (j = 1; j < page_num - 1; j++) {
            if (color == COLOR_WHITE) {
                memset(p_frame, 0x00, width);
            } else {
                memset(p_frame, 0xFF, width);
            }
            p_frame += lcd_real_width;
        }
    }
    /* Copy the datas from the second page to the last second page, end. */

    /* Copy the datas of last page, begin. */
    if (page_num > 1) {
        mask2 = stop_mask0[stop_bit_num];
        for (i = 0; i < width; i++) {
            if (color == COLOR_WHITE) {
                *(p_frame + i) &= (~mask2);
            } else {
                *(p_frame + i) |= mask2;
            }
        }
    }
    /* Copy the datas of last page, end. */
    return true;
}

UINT8 *lcd_get_frame_buffer_address(void)
{
    return lcd_frame_ptr;
}

bool disp_update_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if ((x1 > x2) || (y1 > y2)) {
        return false;
    }

#if defined(__LCD_ROTATE_90__)
    lcd_exchage(&x1, &y1);
    lcd_exchage(&x2, &y2);
#endif

    if (x2 >= lcd_width) {
        x2 = lcd_width  - 1;
    }
    if (y2 >= lcd_height) {
        y2 = lcd_height - 1;
    }

    bsp_lcd_update_screen(x1, y1, x2, y2);

    return true;
}

/* Clear whole LCD screen. */
void disp_clear_screen(void)
{
    uint16_t i, k;
    LCD_FRAME_PTR p_frame = lcd_frame_ptr;
    for (k = 0; k < lcd_page; k++) {
        for (i = 0; i < lcd_real_width; i ++) {
            *p_frame++ = lcd_background_color;
        }
    }
}

/* Draw a pixel on LCD, only the affected pixel will be modified.
* DISP_AND:    data = old_data & 1111x111b
* DISP_OR:     data = old_data | 0000x000b
* DISP_NORMAL: data = (old_data & 11110111b) | 0000x000b
* DISP_XOR:    data = old_data ^ 0000x000b
* DISP_INVERSE:data = (old_data & 11110111b) | (~old_data & 0x00001000) */
void disp_draw_pixel(uint16_t x, uint16_t y)
{
    LCD_FRAME_PTR p_frame;
    uint8_t	cur_page_num;
    uint8_t start_bit_num;
    uint8_t old_data, mask, mask0, mask1;

#if defined(__LCD_ROTATE_90__)
    lcd_exchage(&x, &y);
#endif

    if ((x >= lcd_width) || (y >= lcd_height)) {
        return;
    }

    p_frame = lcd_frame_ptr + (y * lcd_real_width + x);

    cur_page_num  = y >> 3;
    start_bit_num = y & 0x07;
    p_frame  = lcd_frame_ptr + (cur_page_num * lcd_real_width) + x;
    old_data = *p_frame;

    mask = 0x01 << start_bit_num;

    if (lcd_foreground_color == COLOR_BLACK) {
        mask0 = 0x01 << start_bit_num;
        mask1 = 0xFF;
    } else if (lcd_foreground_color == COLOR_WHITE) {
        mask0 = 0x00;
        mask1 = 0xFF ^ mask;
    }

    switch (disp_mode) {
        case DISP_XOR:
            *p_frame = old_data ^ mask0;
            break;
        case DISP_OR:
            *p_frame = old_data | mask0;
            break;
        case DISP_AND:
            *p_frame = old_data & mask1;
            break;
        case DISP_NORMAL:
            if (lcd_foreground_color == COLOR_BLACK) {
                *p_frame = old_data | mask;
            } else {
                *p_frame = old_data ^ mask;
            }
            break;
        case DISP_INVERSE:
            *p_frame = (old_data & (0xFF ^ mask)) | (~old_data & mask);
            break;
        default:
            break;
    }
}

/* Draw pixel with given color */
void disp_draw_pixel_with_color(uint16_t x, uint16_t y, color_type_t color)
{
    color_type_t  b_color;

    disp_get_back_color(&b_color);
    disp_set_back_color(color);
    disp_draw_pixel(x, y);
    disp_set_back_color(b_color);
}

#if defined(__LCD_ROTATE_90__)
/* Draw a vertical line with given parameter, if part of the line is out of LCD
 * area, it will be clipped. We don't call disp_draw_pixel() API just for saving
 * coputing time and efficiency. */
void disp_draw_horizaontal_line(uint16_t x0, uint16_t x1, uint16_t y)
{
    uint16_t temp;
    uint16_t j;
    uint16_t start_page_num, page_num;
    uint16_t start_bit_num, stop_bit_num;
    uint16_t height;
    LCD_FRAME_PTR p_frame;
    uint8_t old_data8;
    uint8_t mask2;
    uint16_t y0, y1, x;
    y0 = x0;
    y1 = x1;
    x = y;

    if (y0 > y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    if ((x >= lcd_width) || (y0 >= lcd_height)) {
        return;
    }
    if (y1 >= lcd_height) {
        y1 = lcd_height - 1;
    }

    height = y1 - y0 + 1;
    page_num  = lcd_get_page_count((uint8_t) height, (uint8_t)y0);
    start_page_num = y0 >> 3;
    start_bit_num = y0 & 0x07;
    stop_bit_num = (height + start_bit_num) & 0x07;
    p_frame = lcd_frame_ptr + start_page_num * lcd_real_width + x;

    for (j = 0; j < page_num; j++) {
        old_data8 = *p_frame;

        if (j == 0) {
            /* first page */
            mask2 = start_mask0[start_bit_num];
            if (page_num == 1) {
                mask2 &= stop_mask0[stop_bit_num];
            }
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame = (lcd_foreground_color == COLOR_WHITE) ? old_data8 & (~mask2) : old_data8 | mask2;
                    break;
                case DISP_XOR:
                    *p_frame = old_data8 ^ mask2;
                    break;
                case DISP_INVERSE:
                    *p_frame = old_data8 ^ mask2;
                    break;
                default:
                    break;
            }
        } else if (j == page_num - 1) {
            /* last page */
            mask2 = stop_mask0[stop_bit_num];
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame = (lcd_foreground_color == COLOR_WHITE) ? old_data8 & (~mask2) : old_data8 | mask2;
                    break;
                case DISP_XOR:
                    *p_frame = old_data8 ^ mask2;
                    break;
                case DISP_INVERSE:
                    *p_frame = old_data8 ^ mask2;
                    break;
                default:
                    break;
            }
        } else if ((j > 0) && (j < page_num - 1)) {
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame = (lcd_foreground_color == COLOR_WHITE) ? 0x00 : 0xFF;
                    break;
                case DISP_XOR:
                    *p_frame = old_data8 ^ mask2;
                    break;
                case DISP_INVERSE:
                    *p_frame = ~old_data8;
                    break;
            }
        }
        p_frame += lcd_real_width;
    }
}

/* Draw a hortiontal line with given parameter, if part of the line is out of LCD
 * area, it will be clipped. We don't call disp_draw_pixel() API just for saving
 * coputing time and efficiency. */
void disp_draw_vertical_line(uint16_t x, uint16_t y0, uint16_t y1)
{
    uint16_t temp, i;
    uint16_t start_bit_num, start_page_num;
    LCD_FRAME_PTR p_frame;
    uint8_t mask;
    uint16_t x0, x1, y;

    x0 = y0;
    x1 = y1;
    y = x;

    if (x0 > x1) {
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    if ((x0 >= lcd_width) || (y >= lcd_height)) {
        return;
    }
    if (x1 >= lcd_width) {
        x1 = lcd_width - 1;
    }

    start_bit_num  = y & 0x07;
    start_page_num = y >> 3;
    mask = 0x01 << start_bit_num;
    p_frame = lcd_frame_ptr + (start_page_num * lcd_real_width) + x0;

    if (lcd_foreground_color == COLOR_BLACK) {
        for (i = x0; i <= x1; i++)  {
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame |= mask;
                    break;
                case DISP_AND:
                    break;
                case DISP_OR:
                    *p_frame |= mask;
                    break;
                case DISP_XOR:
                    *p_frame ^= mask;
                    break;
                case DISP_INVERSE:
                    *p_frame ^= mask;
                    break;
                default:
                    break;
            }
            p_frame ++;
        }
    } else {
        for (i = x0 ; i <= x1; i ++)  {
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame &= ~mask;
                    break;
                case DISP_AND:
                    *p_frame ^= mask;
                    break;
                case DISP_OR:
                    break;
                case DISP_XOR:
                    break;
                case DISP_INVERSE:
                    *p_frame ^= mask;
                    break;
                default:
                    break;
            }
            p_frame ++;
        }
    }
}
#else
/* Draw a vertical line with given parameter, if part of the line is out of LCD
 * area, it will be clipped. We don't call disp_draw_pixel() API just for saving
 * coputing time and efficiency. */
void disp_draw_vertical_line(uint16_t x, uint16_t y0, uint16_t y1)
{
    uint16_t temp;
    uint16_t j;
    uint16_t start_page_num, page_num;
    uint16_t start_bit_num, stop_bit_num;
    uint16_t height;
    LCD_FRAME_PTR p_frame;
    uint8_t old_data8;
    uint8_t mask2;

    if (y0 > y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    if ((x >= lcd_width) || (y0 >= lcd_height)) {
        return;
    }
    if (y1 >= lcd_height) {
        y1 = lcd_height - 1;
    }

    height = y1 - y0 + 1;
    page_num = lcd_get_page_count((uint8_t) height, (uint8_t)y0);
    start_page_num = y0 >> 3;
    start_bit_num = y0 & 0x07;
    stop_bit_num = (height + start_bit_num) & 0x07;
    p_frame = lcd_frame_ptr + start_page_num * lcd_real_width + x;

    for (j = 0; j < page_num; j++) {
        old_data8 = *p_frame;

        if (j == 0) {
            /* first page */
            mask2 = start_mask0[start_bit_num];
            if (page_num == 1) {
                mask2 &= stop_mask0[stop_bit_num];
            }
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame = (lcd_foreground_color == COLOR_WHITE) ? old_data8 & (~mask2) : old_data8 | mask2;
                    break;
                case DISP_XOR:
                    *p_frame = old_data8 ^ mask2;
                    break;
                case DISP_INVERSE:
                    *p_frame = old_data8 ^ mask2;
                    break;
                default:
                    break;
            }
        } else if (j == page_num - 1) {
            /* last page */
            mask2 = stop_mask0[stop_bit_num];
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame = (lcd_foreground_color == COLOR_WHITE) ? old_data8 & (~mask2) : old_data8 | mask2;
                    break;
                case DISP_XOR:
                    *p_frame = old_data8 ^ mask2;
                    break;
                case DISP_INVERSE:
                    *p_frame = old_data8 ^ mask2;
                    break;
                default:
                    break;
            }
        } else if ((j > 0) && (j < page_num - 1)) {
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame = (lcd_foreground_color == COLOR_WHITE) ? 0x00 : 0xFF;
                    break;
                case DISP_XOR:
                    *p_frame = old_data8 ^ mask2;
                    break;
                case DISP_INVERSE:
                    *p_frame = ~old_data8;
                    break;
                default:
                    break;
            }
        }
        p_frame += lcd_real_width;
    }
}

/* Draw a hortiontal line with given parameter, if part of the line is out of LCD
 * area, it will be clipped. We don't call disp_draw_pixel() API just for saving
 * coputing time and efficiency. */
void disp_draw_horizaontal_line(uint16_t x0, uint16_t x1, uint16_t y)
{
    uint16_t temp, i;
    uint16_t start_bit_num, start_page_num;
    LCD_FRAME_PTR p_frame;
    uint8_t mask;

    if (x0 > x1) {
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    if ((x0 >= lcd_width) || (y >= lcd_height)) {
        return;
    }
    if (x1 >= lcd_width) {
        x1 = lcd_width - 1;
    }

    start_bit_num  = y & 0x07;
    start_page_num = y >> 3;
    mask = 0x01 << start_bit_num;
    p_frame = lcd_frame_ptr + (start_page_num * lcd_real_width) + x0;

    if (lcd_foreground_color == COLOR_BLACK) {
        for (i = x0; i <= x1; i++) {
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame |= mask;
                    break;
                case DISP_AND:
                    break;
                case DISP_OR:
                    *p_frame |= mask;
                    break;
                case DISP_XOR:
                    *p_frame ^= mask;
                    break;
                case DISP_INVERSE:
                    *p_frame ^= mask;
                    break;
                default:
                    break;
            }
            p_frame++;
        }
    } else {
        for (i = x0; i <= x1; i++)  {
            switch (disp_mode) {
                case DISP_NORMAL:
                    *p_frame &= ~mask;
                    break;
                case DISP_AND:
                    *p_frame ^= mask;
                    break;
                case DISP_OR:
                    break;
                case DISP_XOR:
                    break;
                case DISP_INVERSE:
                    *p_frame ^= mask;
                    break;
                default:
                    break;
            }
            p_frame ++;
        }
    }
}
#endif

void disp_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint16_t temp;
    int16_t delta_x, delta_y, delta_2x, delta_2y;
    int16_t error, delta_2y_m_delta_2x, delta_2x_m_delta_2y;
    int16_t x_direction;

    if ((x0 == x1) && (y0 == y1))  {
        disp_draw_pixel(x0, y0);
        return;
    }

    if (x0 == x1) {
        disp_draw_vertical_line(x0, y0, y1);
        return;
    }

    if (y0 == y1) {
        disp_draw_horizaontal_line(x0, x1, y0);
        return;
    }

    /* Swap coordinates if necessary thus draw direction will always from top->down. */
    if (y0 > y1) {
        temp = y0;
        y0   = y1;
        y1   = temp;

        temp = x0;
        x0   = x1;
        x1   = temp;
    }

    delta_x = x1 - x0;
    delta_y = y1 - y0;

    /* Depending on delta_x, horizontal line can from left -> right or right -> left. */
    x_direction = (delta_x > 0) ? 1 : -1;

    if (delta_x < 0) {
        delta_x = -delta_x;
    }

    delta_2x = delta_x * 2;
    delta_2y = delta_y * 2;

    /* master direction in horizontal */
    if (delta_x >= delta_y) {
        delta_2y_m_delta_2x = delta_2y - delta_2x;
        error    = delta_2y - delta_x;

        disp_draw_pixel(x0, y0);
        while (delta_x--) {
            if (error >= 0) {
                y0 ++;
                error += delta_2y_m_delta_2x;
            } else {
                error += delta_2y;
            }
            x0 += x_direction;
            disp_draw_pixel(x0, y0);
        }
    } else {
        /* master direction in vertical */
        delta_2x_m_delta_2y = delta_2x - delta_2y;
        error    = delta_2x - delta_y;

        disp_draw_pixel(x0, y0);
        while (delta_y--) {
            if (error >= 0) {
                x0 += x_direction;
                error += delta_2x_m_delta_2y;
            } else {
                error += delta_2x;
            }
            y0 ++;
            disp_draw_pixel(x0, y0);
        }
    }
}

/* Draw line with given color */
void disp_draw_line_with_color(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_type_t color)
{
    color_type_t  old_color;

    disp_get_color(&old_color);
    disp_set_color(color);

    disp_draw_line(x0, y0, x1, y1);

    disp_set_color(old_color);
}

void disp_draw_rect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, color_type_t fill_color, bool b_fill)
{
    //LCD_FRAME_PTR p_frame;
    uint16_t width, height, i;
    color_type_t old_color;

    if (left > right) {
        left = right;
    }
    if (top > bottom) {
        top  = bottom;
    }

    /* Calculate the height and width */
    width = right - left + 1;
    height = bottom - top + 1;
    //p_frame = lcd_frame_ptr + (top * lcd_real_width + left);

    if (b_fill) {
        if (fill_color == lcd_foreground_color) {
            /* If the specified fill color is same as current foreground color, just draw horizontal lines from top --> bottom */
            for (i = 0; i < height; i ++) {
                disp_draw_horizaontal_line(left, right, top++);
            }
        } else {
            /* Draw boder with current foreground color first */
            disp_draw_vertical_line(left, top, bottom);
            disp_draw_horizaontal_line(left, right, bottom);
            disp_draw_vertical_line(right, top, bottom);
            disp_draw_horizaontal_line(left, right, top);

            /* Fill with given color, only if there is at least 1 pixel within the boder. */
            if ((width > 2) && (height > 2)) {
                left++;
                right--;
                top++;
                height -= 2;

                /* Save current foreground color */
                disp_get_color(&old_color);
                disp_set_color(fill_color);
                /* Draw lines */
                for (i = 0; i < height; i++) {
                    disp_draw_horizaontal_line(left, right, top++);
                }
                /* Restore color */
                disp_set_color(old_color);
            }
        }
    } else {
        /* if no fill needed, we just draw boder */
        disp_draw_vertical_line(left, top, bottom);
        disp_draw_horizaontal_line(left, right, bottom);
        disp_draw_vertical_line(right, top, bottom);
        disp_draw_horizaontal_line(left, right, top);
    }
}

void disp_draw_circle(uint16_t x0, uint16_t y0, uint16_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while (a <= b) {
        disp_draw_pixel(x0 - b, y0 - a);
        disp_draw_pixel(x0 + b, y0 - a);
        disp_draw_pixel(x0 - a, y0 + b);
        disp_draw_pixel(x0 - b, y0 - a);
        disp_draw_pixel(x0 - a, y0 - b);
        disp_draw_pixel(x0 + b, y0 + a);
        disp_draw_pixel(x0 + a, y0 - b);
        disp_draw_pixel(x0 + a, y0 + b);
        disp_draw_pixel(x0 - b, y0 + a);
        a++;
        if (di < 0) {
            di += 4 * a + 6;
        } else {
            di += 10 + 4 * (a - b);
            b--;
        }
        disp_draw_pixel(x0 + a, y0 + b);
    }
}

void disp_set_font(font_type_t font)
{
    lcd_font = font;
}

void disp_draw_pic(uint16_t x, uint16_t y, uint16_t height, uint16_t width, const uint8_t *map)
{
    disp_bitmap_t p_bitmap;

    p_bitmap.bits = 1;
    p_bitmap.height = height;
    p_bitmap.width = width;
    p_bitmap.p_bitmap = map;
    //printf("bitmap x:%d, y:%d, height:%d, width:%d\n", x, y, p_bitmap.height,p_bitmap.width);
    disp_put_bitmap_1bit(&p_bitmap, x, y);
}

void disp_draw_char(uint16_t x, uint16_t y, font_code_t code, const uint8_t *map)
{
    disp_bitmap_t p_bitmap;

    switch (lcd_font) {
        case FONT_16x16:
            p_bitmap.bits = 1;
            p_bitmap.height = 16;
            p_bitmap.width = (code < 0x0080) ? 8 : 16;
            break;
        default:
            break;
    }

    p_bitmap.p_bitmap = map;
    disp_put_bitmap_1bit(&p_bitmap, x, y);
}

const uint8_t *find_string_index_from_code(uint16_t w_code)
{
    uint16_t low;
    uint16_t high;
    uint16_t mid;
    const font_db_index_t *p_db;
    uint16_t w_data;

    low = 0;
    high = DATA_BASE_LIST_MAX - 1;

    while (low <= high) {
        mid = (low + high) >> 1;

        p_db = &font_db_index_list[mid];
        w_data = p_db->code;

        if (w_code == w_data) {
            return (p_db->map);
        } else if (w_code < w_data) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return 0 ;
}

void disp_draw_string(uint16_t x, uint16_t y, char *string)
{
    uint16_t temp;
    const uint8_t  *p_str;
    uint8_t  pos_x;
    uint8_t  pos_y;
    pos_x = x;
    pos_y = y;
    while (*string != '\0') {
        temp = *string++;
        if (temp < 0x0080) {
            p_str = find_string_index_from_code(temp);
            disp_draw_char(pos_x, pos_y, temp, p_str);
            switch (lcd_font) {
                case FONT_16x16:
                    pos_x += 8;
                    break;
                default:
                    break;
            }
        } else {
            temp = temp << 8;
            temp |= *string++;
            p_str = find_string_index_from_code(temp);
            disp_draw_char(pos_x, pos_y, temp, p_str);
            switch (lcd_font) {
                case FONT_16x16:
                    pos_x += 16;
                    break;
                default:
                    break;
            }
        }
    }
}

void disp_set_clip(uint16_t x2, uint16_t y2)
{
    lcd_clip_width = x2;
    lcd_clip_height = y2;
}

