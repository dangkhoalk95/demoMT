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

#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

#define WIN32

#define __LCD_ROTATE_90__
#define MONO_DISPLAY
#if defined(__LCD_ROTATE_90__)
#define LCD_WIDTH (128)
#define LCD_HEIGHT (64)
#else
#define LCD_WIDTH (64)
#define LCD_HEIGHT (128)
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif
#ifndef UINT16
#define UINT16  unsigned short
#endif
#ifndef UINT32
#define UINT32  unsigned long
#endif
#ifndef INT8
#define INT8   signed char
#endif
#ifndef INT16
#define INT16  signed short
#endif
#ifndef INT32
#define INT32  signed long
#endif
#ifndef BOOL
#define BOOL unsigned char
#endif
#ifndef BOOLEAN
#define BOOLEAN unsigned char
#endif
#ifndef CHAR
#define CHAR char
#endif
#ifndef WCHAR
#define WCHAR unsigned short
#endif
#ifndef NULL
#define NULL 0
#endif
//typedef unsigned short  WCHAR;

#define FALSE	 	0
#define TRUE		1

#define hmemset memset
#define hmemcpy memcpy
#define MMI_MALLOC(size)    malloc(size)
#define MMI_FREE(type)     free(type)
#define 	DBG_PRINTF		printf
#define ASSERT_DEBUG
#define BLACK_BIT_STR	"."
#define WHITE_BIT_STR 		"*"

#endif //LCD_CONFIG_H


