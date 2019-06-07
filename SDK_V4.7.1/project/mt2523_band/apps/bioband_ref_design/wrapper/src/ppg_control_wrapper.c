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
#include "ppg_control_wrapper.h"


typedef UINT32 (*func0_ptr)(void);
typedef ppg_control_status_t (*func1_ptr)(void);
typedef INT32 (*func2_ptr)(ppg_control_t*, INT32, INT32*);
typedef ppg_control_status_t (*func3_ptr)(ppg_control_app_t);
typedef INT32 (*func4_ptr)(INT32);
typedef const char* (*func5_ptr)(void);


UINT32 ppg_control_get_version_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[ppg_control_get_version_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


ppg_control_status_t ppg_control_init_fota_wrapper(void)
{
    func1_ptr temp;
    temp = (func1_ptr)func_entry_tbl[ppg_control_init_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


INT32 ppg_control_process_fota_wrapper(
    ppg_control_t *ppg_control_input,    // The input signal data.
    INT32 ppg_control_mode,              // The PPG control mode.
    INT32 *ppg_control_output            // The output signal data.
)
{
    func2_ptr temp;
    temp = (func2_ptr)func_entry_tbl[ppg_control_process_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(ppg_control_input, ppg_control_mode, ppg_control_output);
}


ppg_control_status_t ppg_control_set_app_fota_wrapper(ppg_control_app_t ppg_control_app)
{
    func3_ptr temp;
    temp = (func3_ptr)func_entry_tbl[ppg_control_set_app_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(ppg_control_app);
}


INT32 ppg_control_get_status_fota_wrapper(INT32 ppg_control_internal_config)
{
    func4_ptr temp;
    temp = (func4_ptr)func_entry_tbl[ppg_control_get_status_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(ppg_control_internal_config);
}


const char* ppg_control_get_version_string_fota_wrapper(void)
{
    func5_ptr temp;
    temp = (func5_ptr)func_entry_tbl[ppg_control_get_version_string_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


