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

#ifndef __HEART_RATE_WRAPPER_H__
#define __HEART_RATE_WRAPPER_H__
#include "ohrm.h"

#define ohrm_get_version                        ohrm_get_version_fota_wrapper
#define ohrm_process                            ohrm_process_fota_wrapper
#define ohrm_init                               ohrm_init_fota_wrapper
#define ohrm_get_bpm                            ohrm_get_bpm_fota_wrapper
#define ohrm_get_time                           ohrm_get_time_fota_wrapper
#define ohrm_get_confidence_level               ohrm_get_confidence_level_fota_wrapper
//#define ohrm_set_mode                           ohrm_set_mode_fota_wrapper
//#define ppg_set_watch_off_threshold             ppg_set_watch_off_threshold_fota_wrapper
#define ohrm_get_version_string                 ohrm_get_version_string_fota_wrapper


extern UINT32 ohrm_get_version_fota_wrapper(void);
extern void ohrm_process_fota_wrapper(ohrm_data_t *ohrm_signal_in, INT32 fs_compare_ratio, INT32 *ohrm_out);
extern void ohrm_init_fota_wrapper(INT32 ohrm_motion_represent_1g);
extern INT32 ohrm_get_bpm_fota_wrapper(void);
extern INT32 ohrm_get_time_fota_wrapper(void);
extern UINT32 ohrm_get_confidence_level_fota_wrapper(void);
//extern void ppg_set_watch_off_threshold_fota_wrapper(INT32 ambient_threshold);
extern const char* ohrm_get_version_string_fota_wrapper(void);


#endif /* __HEART_RATE_WRAPPER_H__ */
