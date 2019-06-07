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

#include "stdint.h"
#include "fota_decoupling_common.h"
#include "blood_pressure_wrapper.h"


typedef void (*func0_ptr)(void);
typedef void (*func1_ptr)(int32_t, int32_t, int32_t, int32_t, int32_t);
typedef void (*func2_ptr)(int32_t, int32_t);
typedef void (*func3_ptr)(bp_alg_out_t*);
typedef void (*func4_ptr)(int32_t*, int32_t);

void bp_alg_init_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[bp_alg_init_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp();
}

void bp_alg_set_user_info_fota_wrapper(int32_t age, int32_t gender, int32_t height, int32_t weight, int32_t arm_length)
{
    func1_ptr temp;
    temp = (func1_ptr)func_entry_tbl[bp_alg_set_user_info_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp(age, gender, height, weight, arm_length);
}

void bp_alg_process_data_fota_wrapper(int32_t data, int32_t type)
{
    func2_ptr temp;
    temp = (func2_ptr)func_entry_tbl[bp_alg_process_data_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp(data, type);
}

void bp_alg_get_bp_fota_wrapper(bp_alg_out_t* output)
{
    func3_ptr temp;
    temp = (func3_ptr)func_entry_tbl[bp_alg_get_bp_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp(output);
}

void bp_alg_set_calibration_data_fota_wrapper(int32_t* data_in,  int32_t data_in_len)
{
    func4_ptr temp;
    temp = (func4_ptr)func_entry_tbl[bp_alg_set_calibration_data_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp(data_in, data_in_len);
}

void bp_alg_get_calibration_data_fota_wrapper(int32_t* data_out, int32_t data_out_len)
{
    func4_ptr temp;
    temp = (func4_ptr)func_entry_tbl[bp_alg_get_calibration_data_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp(data_out, data_out_len);
}


