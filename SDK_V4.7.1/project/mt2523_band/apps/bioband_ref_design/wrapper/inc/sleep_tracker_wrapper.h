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

#ifndef __SLEEP_TRACKER_WRAPPER_H__
#define __SLEEP_TRACKER_WRAPPER_H__
// #include "hr_fet.h"
// #include "motion_fet.h"
// #include "motion_st.h"
#include "sleep.h"
#include "af_api.h"

#define hr_fet_init                             hr_fet_init_fota_wrapper
#define hr_fet_ext                              hr_fet_ext_fota_wrapper
#define check_sleep_hr_fet                      check_sleep_hr_fet_fota_wrapper
#define get_one_hr_fet                          get_one_hr_fet_fota_wrapper
#define rearrange_queue_for_hr_fet              rearrange_queue_for_hr_fet_fota_wrapper

#define check_sleep_motion_fet                  check_sleep_motion_fet_fota_wrapper
#define get_one_motion_fet                      get_one_motion_fet_fota_wrapper
#define rearrange_queue_for_motion_fet          rearrange_queue_for_motion_fet_fota_wrapper

#define motion_st_init                          motion_st_init_fota_wrapper
#define motion_st                               motion_st_fota_wrapper
#define check_sleep_results                     check_sleep_results_fota_wrapper
#define get_one_sleep_result                    get_one_sleep_result_fota_wrapper
#define get_se                                  get_se_fota_wrapper
#define get_bedtime                             get_bedtime_fota_wrapper
#define rearrange_queue_for_sleep               rearrange_queue_for_sleep_fota_wrapper
#define resp_fet_ext                            resp_fet_ext_fota_wrapper

#define sleep_fet_init                          sleep_fet_init_fota_wrapper
#define sleep_fet_run                           sleep_fet_run_fota_wrapper
#define check_fet_ready                         check_fet_ready_fota_wrapper
#define send_fet_packet                         send_fet_packet_fota_wrapper

#define af_init                                 af_init_fota_wrapper
#define af_get_feature                          af_get_feature_fota_wrapper
#define af_fet_run                              af_fet_run_fota_wrapper

extern int  hr_fet_init_fota_wrapper(void);
extern int  hr_fet_ext_fota_wrapper(int ppg_bisi, int timestamp, uint8_t sensor_type, uint8_t fs);
extern int  check_sleep_hr_fet_fota_wrapper(void);
extern int  get_one_hr_fet_fota_wrapper(int *results, unsigned int *results_timestamp);
extern int  rearrange_queue_for_hr_fet_fota_wrapper(void);

extern int  check_sleep_motion_fet_fota_wrapper(void);
extern int  get_one_motion_fet_fota_wrapper(int *results, unsigned int *results_timestamp);
extern int  rearrange_queue_for_motion_fet_fota_wrapper(void);

extern int  motion_st_init_fota_wrapper(void);
extern int  motion_st_fota_wrapper(int ax, int ay, int az, int timestamp);
extern int  check_sleep_results_fota_wrapper();
extern int   get_one_sleep_result_fota_wrapper(int *results, unsigned int *results_timestamp);
extern int   get_se_fota_wrapper(void);
extern unsigned int get_bedtime_fota_wrapper(void);
extern int  rearrange_queue_for_sleep_fota_wrapper(void);
extern int resp_fet_ext_fota_wrapper(int ppg, int timestamp);

extern int  sleep_fet_init_fota_wrapper(void);
extern int  sleep_fet_run_fota_wrapper(sleep_fet_sp_t in, int pid, int algo_state);
extern int  check_fet_ready_fota_wrapper(void);
extern void send_fet_packet_fota_wrapper(int32_t *fet, uint32_t *fet_time, int32_t *fet_num, int fet_type);


extern void af_init_fota_wrapper(void);
extern int32_t af_get_feature_fota_wrapper(void);
extern int32_t af_fet_run_fota_wrapper(sleep_fet_sp_t in);


#endif /* __SLEEP_TRACKER_WRAPPER_H__ */
