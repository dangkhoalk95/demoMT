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
#include "sleep_tracker_wrapper.h"


typedef int (*func0_ptr)(void);
typedef int (*func1_ptr)(int, int, uint8_t, uint8_t);
typedef int (*func2_ptr)(int*, unsigned int*);
typedef int (*func3_ptr)(int, int, int, int);
typedef unsigned int (*func4_ptr)(void);
typedef int (*func5_ptr)(int, int);
typedef int (*func6_ptr)(sleep_fet_sp_t, int, int);
typedef void (*func7_ptr)(int32_t*, uint32_t*, int32_t*, int);
typedef void (*func8_ptr)(void);
typedef int32_t (*func9_ptr)(sleep_fet_sp_t);

/*****************************************************************************
 *  wrapper for function defined in hr_fet.h
 *****************************************************************************/
int  hr_fet_init_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[hr_fet_init_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

int  hr_fet_ext_fota_wrapper(int ppg_bisi, int timestamp, uint8_t sensor_type, uint8_t fs)
{
    func1_ptr temp;
    temp = (func1_ptr)func_entry_tbl[hr_fet_ext_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(ppg_bisi, timestamp, sensor_type, fs);
}


int  check_sleep_hr_fet_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[check_sleep_hr_fet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


int  get_one_hr_fet_fota_wrapper(int *results, unsigned int *results_timestamp)
{
    func2_ptr temp;
    temp = (func2_ptr)func_entry_tbl[get_one_hr_fet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(results, results_timestamp);
}


int  rearrange_queue_for_hr_fet_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[rearrange_queue_for_hr_fet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

int resp_fet_ext_fota_wrapper(int ppg, int timestamp)
{
    func5_ptr temp;
    temp = (func5_ptr)func_entry_tbl[resp_fet_ext_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(ppg, timestamp);
}


/*****************************************************************************
 *  wrapper for function defined in motion_fet.h
 *****************************************************************************/
int  check_sleep_motion_fet_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[check_sleep_motion_fet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

 
int  get_one_motion_fet_fota_wrapper(int *results, unsigned int *results_timestamp)
{
    func2_ptr temp;
    temp = (func2_ptr)func_entry_tbl[get_one_motion_fet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(results, results_timestamp);
}


int  rearrange_queue_for_motion_fet_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[rearrange_queue_for_motion_fet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


/*****************************************************************************
 *  wrapper for function defined in motion_st.h
 *****************************************************************************/
int  motion_st_init_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[motion_st_init_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

int  motion_st_fota_wrapper(int ax, int ay, int az, int timestamp)
{
    func3_ptr temp;
    temp = (func3_ptr)func_entry_tbl[motion_st_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(ax, ay, az, timestamp);
}

   
int  check_sleep_results_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[check_sleep_results_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

int   get_one_sleep_result_fota_wrapper(int *results, unsigned int *results_timestamp)
{
    func2_ptr temp;
    temp = (func2_ptr)func_entry_tbl[get_one_sleep_result_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(results, results_timestamp);
}

int   get_se_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[get_se_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

unsigned int get_bedtime_fota_wrapper(void)
{
    func4_ptr temp;
    temp = (func4_ptr)func_entry_tbl[get_bedtime_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}

int  rearrange_queue_for_sleep_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[rearrange_queue_for_sleep_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}



int  sleep_fet_init_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[sleep_fet_init_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


int  sleep_fet_run_fota_wrapper(sleep_fet_sp_t in, int pid, int algo_state)
{
    func6_ptr temp;
    temp = (func6_ptr)func_entry_tbl[sleep_fet_run_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(in, pid, algo_state);
}


int  check_fet_ready_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[check_fet_ready_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


void send_fet_packet_fota_wrapper(int32_t *fet, uint32_t *fet_time, int32_t *fet_num, int fet_type)
{
    func7_ptr temp;
    temp = (func7_ptr)func_entry_tbl[send_fet_packet_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp(fet, fet_time, fet_num, fet_type);
}


/*****************************************************************************
 *  wrapper for function defined in af_api.h
 *****************************************************************************/

void af_init_fota_wrapper(void)
{
    func8_ptr temp;
    temp = (func8_ptr)func_entry_tbl[af_init_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    temp();
}


int32_t af_get_feature_fota_wrapper(void)
{
    func0_ptr temp;
    temp = (func0_ptr)func_entry_tbl[af_get_feature_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp();
}


int32_t af_fet_run_fota_wrapper(sleep_fet_sp_t in)
{
    func9_ptr temp;
    temp = (func9_ptr)func_entry_tbl[af_fet_run_fota_wrapper_idx];
    PRINT_FUNC_ADDR(temp);
    return temp(in);
}



