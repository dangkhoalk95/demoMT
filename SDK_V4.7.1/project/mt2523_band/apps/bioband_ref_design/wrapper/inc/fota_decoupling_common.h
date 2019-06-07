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
 
#ifndef __FOTA_DECOUPLING_COMMON_H__
#define __FOTA_DECOUPLING_COMMON_H__

#include "stdint.h"
#include "syslog.h"
#include "memory_map.h"

#if !defined(MTK_DEBUG_LEVEL_NONE)
#define LOGI(fmt, arg...)       LOG_I(fota_ubin, "[FOTA_UBIN] "fmt,##arg)
#define LOGW(fmt, arg...)       LOG_W(fota_ubin, "[FOTA_UBIN] "fmt,##arg)
#define LOGE(fmt, arg...)       LOG_E(fota_ubin, "[FOTA_UBIN] "fmt,##arg)
#define PRINT_FUNC_ADDR(x)      LOGI("call address = %x\r\n", (uint32_t*)x)
#else
#define LOGI(fmt, arg...)       
#define LOGW(fmt, arg...)       
#define LOGE(fmt, arg...)       
#define PRINT_FUNC_ADDR(x)      
#endif

/* ubin entry address */
#define UBIN_TABLE_ADDR         (UBIN_BASE)
extern uint32_t* func_entry_tbl;


/* Don't  modify the order of these element in this enumulation */
typedef enum
{
    ustartup_fota_wrapper_idx,                                  /* 00 */
    pedometer_algorithm_init_fota_wrapper_idx,
    get_pedometer_final_step_count_fota_wrapper_idx,
    get_pedomter_step_type_fota_wrapper_idx,
    set_pedometer_t_s_fota_wrapper_idx,
    pedometer_detector_fota_wrapper_idx,
    ohrm_get_version_fota_wrapper_idx,                          /* 06 */
    ohrm_process_fota_wrapper_idx,
    ohrm_init_fota_wrapper_idx,
    ohrm_get_bpm_fota_wrapper_idx,
    ohrm_get_time_fota_wrapper_idx,
    ohrm_get_confidence_level_fota_wrapper_idx,
    ohrm_get_version_string_fota_wrapper_idx,
    HRV_initialization_fota_wrapper_idx,                        /* 13 */
    HRV_process_data_fota_wrapper_idx,
    HRV_estimate_fota_wrapper_idx,
    hr_fet_init_fota_wrapper_idx,                               /* 16 */
    hr_fet_ext_fota_wrapper_idx,
    check_sleep_hr_fet_fota_wrapper_idx,
    get_one_hr_fet_fota_wrapper_idx,
    rearrange_queue_for_hr_fet_fota_wrapper_idx,
    resp_fet_ext_fota_wrapper_idx,
    check_sleep_motion_fet_fota_wrapper_idx,
    get_one_motion_fet_fota_wrapper_idx,
    rearrange_queue_for_motion_fet_fota_wrapper_idx,
    motion_st_init_fota_wrapper_idx,
    motion_st_fota_wrapper_idx,
    check_sleep_results_fota_wrapper_idx,
    get_one_sleep_result_fota_wrapper_idx,
    get_se_fota_wrapper_idx,
    get_bedtime_fota_wrapper_idx,
    rearrange_queue_for_sleep_fota_wrapper_idx,
    sleep_fet_init_fota_wrapper_idx,
    sleep_fet_run_fota_wrapper_idx,
    check_fet_ready_fota_wrapper_idx,
    send_fet_packet_fota_wrapper_idx,
    ppg_control_get_version_fota_wrapper_idx,                   /* 36 */
    ppg_control_init_fota_wrapper_idx,
    ppg_control_process_fota_wrapper_idx,
    ppg_control_set_app_fota_wrapper_idx,
    ppg_control_get_status_fota_wrapper_idx,
    ppg_control_get_version_string_fota_wrapper_idx,
    bp_alg_init_fota_wrapper_idx,                               /* 42 */
    bp_alg_set_user_info_fota_wrapper_idx,
    bp_alg_process_data_fota_wrapper_idx,
    bp_alg_get_bp_fota_wrapper_idx,
    bp_alg_set_calibration_data_fota_wrapper_idx,
    bp_alg_get_calibration_data_fota_wrapper_idx,
    af_init_fota_wrapper_idx,                                   /* 48 */
    af_get_feature_fota_wrapper_idx,
    af_fet_run_fota_wrapper_idx
}fota_ubin_symbol_idx_enum;

#endif /* __FOTA_DECOUPLING_COMMON_H__ */

