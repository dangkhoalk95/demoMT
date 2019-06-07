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
#ifndef __BW_GDI_API_H__
#define __BW_GDI_API_H__
#include <stdint.h>
#include <stdbool.h>
#include "lcd_config.h"

typedef unsigned char   color_type_t;
typedef UINT8          *LCD_FRAME_PTR;

#define  COLOR_BLACK    0xFF
#define  COLOR_WHITE    0x00

typedef enum {
    FONT_16x16,
    FONT_MAX
} font_type_t;

typedef uint16_t font_code_t;
typedef struct {
    font_code_t code;
    const unsigned char *map;
} font_db_index_t;

typedef enum {
    DISP_NORMAL,    /* overwrite LCD buffer */
    DISP_INVERSE,   /* overwrite with inverse */
    DISP_XOR,       /* XOR with buffer */
    DISP_OR,        /* OR with buffer */
    DISP_AND        /* AND with buffer */
} disp_mode_t;

typedef struct {
    const uint8_t    *p_bitmap;  /* pointer to bitmap data */
    uint16_t   height;
    uint16_t   width;
    uint8_t	 bits;		   /* bits per pixel, 8 for 256 color, 16 for 65k and 1 for black-white */
} disp_bitmap_t;

extern UINT8 *lcd_get_frame_buffer_address(void);
extern bool disp_update_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
extern void disp_clear_screen(void);
extern void disp_draw_rect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, color_type_t fill_color, bool b_fill);
extern void disp_set_font(font_type_t font);
extern void disp_draw_pic(uint16_t x, uint16_t y, uint16_t height, uint16_t width, const uint8_t *map);
extern void disp_draw_string(uint16_t x, uint16_t y, char *string);

#endif
