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

#ifndef __ZIMO_H__
#define __ZIMO_H__

#include "bw_gdi_api.h"


//Symbol
extern const UINT8 eChar_20[16];
extern const UINT8 eChar_21[16];
extern const UINT8 eChar_22[16];
extern const UINT8 eChar_23[16];
extern const UINT8 eChar_24[16];
extern const UINT8 eChar_25[16];
extern const UINT8 eChar_26[16];
extern const UINT8 eChar_27[16];
extern const UINT8 eChar_28[16];
extern const UINT8 eChar_29[16];
extern const UINT8 eChar_2A[16];
extern const UINT8 eChar_2B[16];
extern const UINT8 eChar_2C[16];
extern const UINT8 eChar_2D[16];
extern const UINT8 eChar_2E[16];
extern const UINT8 eChar_2F[16];
//0~9
extern const UINT8 eChar_30[16];
extern const UINT8 eChar_31[16];
extern const UINT8 eChar_32[16];
extern const UINT8 eChar_33[16];
extern const UINT8 eChar_34[16];
extern const UINT8 eChar_35[16];
extern const UINT8 eChar_36[16];
extern const UINT8 eChar_37[16];
extern const UINT8 eChar_38[16];
extern const UINT8 eChar_39[16];
//Symbol
extern const UINT8 eChar_3A[16];
extern const UINT8 eChar_3B[16];
extern const UINT8 eChar_3C[16];
extern const UINT8 eChar_3D[16];
extern const UINT8 eChar_3E[16];
extern const UINT8 eChar_3F[16];
extern const UINT8 eChar_40[16];
//A~Z
extern const UINT8 eChar_41[16];
extern const UINT8 eChar_42[16];
extern const UINT8 eChar_43[16];
extern const UINT8 eChar_44[16];
extern const UINT8 eChar_45[16];
extern const UINT8 eChar_46[16];
extern const UINT8 eChar_47[16];
extern const UINT8 eChar_48[16];
extern const UINT8 eChar_49[16];
extern const UINT8 eChar_4A[16];
extern const UINT8 eChar_4B[16];
extern const UINT8 eChar_4C[16];
extern const UINT8 eChar_4D[16];
extern const UINT8 eChar_4E[16];
extern const UINT8 eChar_4F[16];
extern const UINT8 eChar_50[16];
extern const UINT8 eChar_51[16];
extern const UINT8 eChar_52[16];
extern const UINT8 eChar_53[16];
extern const UINT8 eChar_54[16];
extern const UINT8 eChar_55[16];
extern const UINT8 eChar_56[16];
extern const UINT8 eChar_57[16];
extern const UINT8 eChar_58[16];
extern const UINT8 eChar_59[16];
extern const UINT8 eChar_5A[16];

//Symbol
extern const UINT8 eChar_5B[16];
extern const UINT8 eChar_5C[16];
extern const UINT8 eChar_5D[16];
extern const UINT8 eChar_5E[16];
extern const UINT8 eChar_5F[16];
extern const UINT8 eChar_60[16];

//a~z
extern const UINT8 eChar_61[16];
extern const UINT8 eChar_62[16];
extern const UINT8 eChar_63[16];
extern const UINT8 eChar_64[16];
extern const UINT8 eChar_65[16];
extern const UINT8 eChar_66[16];
extern const UINT8 eChar_67[16];
extern const UINT8 eChar_68[16];
extern const UINT8 eChar_69[16];
extern const UINT8 eChar_6A[16];
extern const UINT8 eChar_6B[16];
extern const UINT8 eChar_6C[16];
extern const UINT8 eChar_6D[16];
extern const UINT8 eChar_6E[16];
extern const UINT8 eChar_6F[16];
extern const UINT8 eChar_70[16];
extern const UINT8 eChar_71[16];
extern const UINT8 eChar_72[16];
extern const UINT8 eChar_73[16];
extern const UINT8 eChar_74[16];
extern const UINT8 eChar_75[16];
extern const UINT8 eChar_76[16];
extern const UINT8 eChar_77[16];
extern const UINT8 eChar_78[16];
extern const UINT8 eChar_79[16];
extern const UINT8 eChar_7A[16];
const font_db_index_t font_db_index_list[] = {
    {' ', eChar_20},
    {'!', eChar_21},
    {'"', eChar_22},
    {'#', eChar_23},
    {'$', eChar_24},
    {'%', eChar_25},
    {'&', eChar_26},
    {0x27, eChar_27},
    {'(', eChar_28},
    {')', eChar_29},
    {'*', eChar_2A},
    {'+', eChar_2B},
    {',', eChar_2C},
    {'-', eChar_2D},
    {'.', eChar_2E},
    {'/', eChar_2F},

    {'0', eChar_30},
    {'1', eChar_31},
    {'2', eChar_32},
    {'3', eChar_33},
    {'4', eChar_34},
    {'5', eChar_35},
    {'6', eChar_36},
    {'7', eChar_37},
    {'8', eChar_38},
    {'9', eChar_39},

    {':', eChar_3A},
    {';', eChar_3B},
    {'<', eChar_3C},
    {'=', eChar_3D},
    {'>', eChar_3E},
    {'?', eChar_3F},
    {'@', eChar_40},

    {'A', eChar_41},
    {'B', eChar_42},
    {'C', eChar_43},
    {'D', eChar_44},
    {'E', eChar_45},
    {'F', eChar_46},
    {'G', eChar_47},
    {'H', eChar_48},
    {'I', eChar_49},
    {'J', eChar_4A},
    {'K', eChar_4B},
    {'L', eChar_4C},
    {'M', eChar_4D},
    {'N', eChar_4E},
    {'O', eChar_4F},
    {'P', eChar_50},
    {'Q', eChar_51},
    {'R', eChar_52},
    {'S', eChar_53},
    {'T', eChar_54},
    {'U', eChar_55},
    {'V', eChar_56},
    {'W', eChar_57},
    {'X', eChar_58},
    {'Y', eChar_59},
    {'Z', eChar_5A},

    {'[', eChar_5B},
    {0x5C, eChar_5C},
    {']', eChar_5D},
    {'^', eChar_5E},
    {'_', eChar_5F},
    {0x60, eChar_60},

    {'a', eChar_61},
    {'b', eChar_62},
    {'c', eChar_63},
    {'d', eChar_64},
    {'e', eChar_65},
    {'f', eChar_66},
    {'g', eChar_67},
    {'h', eChar_68},
    {'i', eChar_69},
    {'j', eChar_6A},
    {'k', eChar_6B},
    {'l', eChar_6C},
    {'m', eChar_6D},
    {'n', eChar_6E},
    {'o', eChar_6F},
    {'p', eChar_70},
    {'q', eChar_71},
    {'r', eChar_72},
    {'s', eChar_73},
    {'t', eChar_74},
    {'u', eChar_75},
    {'v', eChar_76},
    {'w', eChar_77},
    {'x', eChar_78},
    {'y', eChar_79},
    {'z', eChar_7A},
};

#define DATA_BASE_LIST_MAX		sizeof(font_db_index_list)/sizeof(font_db_index_list[0])


#endif

