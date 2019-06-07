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

/* include ubin symbol definition header */
#if defined(FUSION_PEDOMETER_USE) && (FUSION_PEDOMETER_USE==M_INHOUSE_PEDOMETER)
#include "pedometer.h"
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE) && (FUSION_HEART_RATE_MONITOR_USE==M_INHOUSE_HEART_RATE_MONITOR)
#include "ohrm.h"
#include "ppg_control.h"
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE) && (FUSION_HEART_RATE_VARIABILITY_USE==M_INHOUSE_HEART_RATE_VARIABILITY)
#include "heart_rate_variability.h"
#endif

#if defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER)
//#include "hr_fet.h"
//#include "motion_fet.h"
//#include "motion_st.h"
#include "sleep.h"
#include "af_api.h"
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_BLOOD_PRESSURE)
#include "ppg_control.h"
#include "bp_alg.h"
#endif

/* ubin region init function */
extern void ustartup(void);

/* jump table definition */
const void* ubin_table[] __attribute__ ((section (".ubin_table"))) =
{
    &ustartup,                              /* 00 */
#if defined(FUSION_PEDOMETER_USE) && (FUSION_PEDOMETER_USE==M_INHOUSE_PEDOMETER)
    &pedometer_algorithm_init,
    &get_pedometer_final_step_count,
    &get_pedomter_step_type,
    &set_pedometer_t_s,
    &pedometer_detector,
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
#if defined(FUSION_HEART_RATE_MONITOR_USE) && (FUSION_HEART_RATE_MONITOR_USE==M_INHOUSE_HEART_RATE_MONITOR) 
    &ohrm_get_version,                      /* 06 */
    &ohrm_process,
    &ohrm_init,
    &ohrm_get_bpm,
    &ohrm_get_time,
    &ohrm_get_confidence_level,
    &ohrm_get_version_string,
#else
    NULL,                                   /* 06 */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
    //&ohrm_set_mode,    // not defined in sensor fusion library
#if defined(FUSION_HEART_RATE_VARIABILITY_USE) && (FUSION_HEART_RATE_VARIABILITY_USE==M_INHOUSE_HEART_RATE_VARIABILITY)
    &HRV_initialization,                    /* 13 */
    &HRV_process_data,
    &HRV_estimate,
#else
    NULL,                                   /* 13 */
    NULL,
    NULL,
#endif
#if defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER)
    &hr_fet_init,                           /* 16 */
    &hr_fet_ext,
    &check_sleep_hr_fet,
    &get_one_hr_fet,
    &rearrange_queue_for_hr_fet,
    &resp_fet_ext,
    &check_sleep_motion_fet,
    &get_one_motion_fet,
    &rearrange_queue_for_motion_fet,
    &motion_st_init,
    &motion_st,
    &check_sleep_results,
    &get_one_sleep_result,
    &get_se,
    &get_bedtime,
    &rearrange_queue_for_motion_fet,
    &sleep_fet_init,
    &sleep_fet_run,
    &check_fet_ready,
    &send_fet_packet,
#else
    NULL,                                   /* 16 */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
#if (defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER)) \
    || (defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_BLOOD_PRESSURE))
    &ppg_control_get_version,               /* 36 */
    &ppg_control_init,
    &ppg_control_process,
    &ppg_control_set_app,
    &ppg_control_get_status,
    &ppg_control_get_version_string,
#else
    NULL,                                   /* 36 */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
#if defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_BLOOD_PRESSURE)
    &bp_alg_init,                           /* 42 */
    &bp_alg_set_user_info,
    &bp_alg_process_data,
    &bp_alg_get_bp,
#if BP_ALG_CALIBRATION
    &bp_alg_set_calibration_data,
    &bp_alg_get_calibration_data,
#else
    NULL,
    NULL,
#endif /* BP_ALG_CALIBRATION */
#else
    NULL,                                   /* 42 */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
#if defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER)
    &af_init,                               /* 48 */
    &af_get_feature,
    &af_fet_run,
#else
    NULL,                                   /* 48 */
    NULL,
    NULL,
#endif
};

