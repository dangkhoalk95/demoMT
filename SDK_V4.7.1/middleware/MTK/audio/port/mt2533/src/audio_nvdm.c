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

#include "audio_nvdm.h"
#include "nvdm.h"
#include "syslog.h"
#include <stdint.h>
#include <string.h>

#include "audio_middleware_api.h"
#include "audio_default_coefficient.h"
#include "hal_audio_internal_afe.h"

#ifdef MTK_AUDIO_TUNING_ENABLED
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#endif

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#endif

//#define NOT_PRINT_MESSAGE
#ifdef NOT_PRINT_MESSAGE
#define LOGE(fmt,arg...)   {}
#define LOGW(fmt,arg...)   {}
#define LOGI(fmt,arg...)   {}
#else
log_create_module(audio_nvdm, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(audio_nvdm, "[audio_nvdm]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(audio_nvdm, "[audio_nvdm]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(audio_nvdm, "[audio_nvdm]"fmt,##arg)
#endif

#define MAX_CALLBACK_NUMBER 32


typedef struct {
    audio_nvdm_callback_id *callback_id;
    audio_nvdm_callback_funtion_t callback_function;
    void *data;
} audio_nvdm_eaps_is_changed_callback_t;

#if defined(__AFE_HS_DC_CALIBRATION__) //HP can be added in the future
static afe_dc_calibration_t audio_nvdm_dc_calibration;
#endif

//static audio_eaps_t audio_eaps = {{{0}}};
static audio_eaps_t audio_eaps = {
    .speech_common_parameter.speech_common_parameter = SPEECH_COMMON_PARAMETER,

    .audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain = AUDIO_HANDSFREE_GAIN_STREAM_OUT_GAIN,
    .audio_parameter.handsfree_headset_stream_out_gain.headset_gain = AUDIO_HEADSET_GAIN_STREAM_OUT_GAIN,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[0].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER1,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[1].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER2,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[2].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER3,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[3].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER4,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[4].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER5,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[5].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER6,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[6].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER7,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[7].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER8,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[8].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER9,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[9].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER10,
    .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[10].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER11,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[0].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER1,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[1].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER2,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[2].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER3,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[3].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER4,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[4].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER5,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[5].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER6,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[6].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER7,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[7].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER8,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[8].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER9,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[9].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER10,
    .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[10].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER11,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[0].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER1,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[1].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER2,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[2].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER3,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[3].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER4,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[4].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER5,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[5].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER6,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[6].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER7,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[7].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER8,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[8].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER9,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[9].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER10,
    .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[10].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER11,

    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.stream_out_gain = VOICE_NB_MODE1_GAIN_STREAM_OUT_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.stream_in_gain = VOICE_NB_MODE1_GAIN_STREAM_IN_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.sidetone_gain = VOICE_NB_MODE1_GAIN_SIDETONE_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_mode_parameter = VOICE_NB_MODE1_NB_MODE_PARAMETER,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient = VOICE_NB_MODE1_NB_STREAM_IN_FIR_COEFFICIENT,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_frequency = VOICE_NB_MODE1_NB_STREAM_IN_FIR_FREQUENCY,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_response = VOICE_NB_MODE1_NB_STREAM_IN_FIR_RESPONSE,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient = VOICE_NB_MODE1_NB_STREAM_OUT_FIR_COEFFICIENT,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency = VOICE_NB_MODE1_NB_STREAM_OUT_FIR_FREQUENCY,
    .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_response = VOICE_NB_MODE1_NB_STREAM_OUT_FIR_RESPONSE,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_out_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_OUT_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_in_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_IN_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.sidetone_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_GAIN_SIDETONE_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_mode_parameter = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_MODE_PARAMETER,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_STREAM_OUT_FIR_COEFFICIENT,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_STREAM_OUT_FIR_FREQUENCY,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_response = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_STREAM_OUT_FIR_RESPONSE,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_gain.stream_out_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_OUT_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_gain.stream_in_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_IN_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_gain.sidetone_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_GAIN_SIDETONE_GAIN,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_mode_parameter = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_MODE_PARAMETER,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_STREAM_OUT_FIR_COEFFICIENT,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_STREAM_OUT_FIR_FREQUENCY,
    .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_stream_out_fir_response = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_STREAM_OUT_FIR_RESPONSE,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.stream_out_gain = VOICE_WB_MODE1_GAIN_STREAM_OUT_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.stream_in_gain = VOICE_WB_MODE1_GAIN_STREAM_IN_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.sidetone_gain = VOICE_WB_MODE1_GAIN_SIDETONE_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_mode_parameter = VOICE_WB_MODE1_WB_MODE_PARAMETER,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient = VOICE_WB_MODE1_WB_STREAM_IN_FIR_COEFFICIENT,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_frequency = VOICE_WB_MODE1_WB_STREAM_IN_FIR_FREQUENCY,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_response = VOICE_WB_MODE1_WB_STREAM_IN_FIR_RESPONSE,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient = VOICE_WB_MODE1_WB_STREAM_OUT_FIR_COEFFICIENT,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency = VOICE_WB_MODE1_WB_STREAM_OUT_FIR_FREQUENCY,
    .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_response = VOICE_WB_MODE1_WB_STREAM_OUT_FIR_RESPONSE,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_out_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_OUT_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_in_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_IN_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.sidetone_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_SIDETONE_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_mode_parameter = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_MODE_PARAMETER,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_STREAM_OUT_FIR_COEFFICIENT,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_STREAM_OUT_FIR_FREQUENCY,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_response = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_STREAM_OUT_FIR_RESPONSE,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_gain.stream_out_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_OUT_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_gain.stream_in_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_IN_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_gain.sidetone_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_SIDETONE_GAIN,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_mode_parameter = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_MODE_PARAMETER,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_STREAM_OUT_FIR_COEFFICIENT,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_STREAM_OUT_FIR_FREQUENCY,
    .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_stream_out_fir_response = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_STREAM_OUT_FIR_RESPONSE,

    .record_parameter.record_wb_parameter[0].stream_in_gain = RECORD_WB_MODE1_GAIN_STREAM_IN_GAIN,
    .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_mode_parameter = RECORD_WB_MODE1_WB_MODE_PARAMETER,
    .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient = RECORD_WB_MODE1_WB_CHANNEL1_FIR_COEFFICIENT,
    .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_frequency = RECORD_WB_MODE1_WB_CHANNEL1_FIR_FREQUENCY,
    .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_response = RECORD_WB_MODE1_WB_CHANNEL1_FIR_RESPONSE,
    .record_parameter.record_wb_parameter_with_external_dsp[0].stream_in_gain = RECORD_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_IN_GAIN,
    .record_parameter.record_wb_parameter_with_external_dsp[0].record_wb_enhancement_parameter.external_dsp_sending_path_register_info = RECORD_WB_WITH_EXTERNAL_DSP_MODE1_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
    .record_parameter.record_wb_parameter_with_external_dsp[1].stream_in_gain = RECORD_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_IN_GAIN,
    .record_parameter.record_wb_parameter_with_external_dsp[1].record_wb_enhancement_parameter.external_dsp_sending_path_register_info = RECORD_WB_WITH_EXTERNAL_DSP_MODE2_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO
};


audio_nvdm_eaps_is_changed_callback_t audio_nvdm_eaps_data_is_changed_callback[MAX_CALLBACK_NUMBER] = {{0}};



#ifdef MTK_AUDIO_TUNING_ENABLED

#define INDEX_UNDEFINE_VALUE 0xffff

#define EXTERNAL_DSP_QUERY_INDEX \
{ \
    {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0}, {4, 0, 0}, {5, 0, 0}, {6, 0, 0}, {8, 0, 0}, {9, 0, 0}, {10, 0, 0}, \
    {11, 0, 0}, {12, 0, 0}, {18, 0, 0}, {19, 0, 0}, {20, 0, 0}, {21, 0, 0}, {24, 0, 0}, {25, 0, 0}, {26, 0, 0}, {27, 0, 0}, \
    {28, 0, 0}, {31, 0, 0}, {35, 0, 0}, {36, 0, 0}, {114, 0, 0}, {115, 0, 0}, {116, 0, 0}, {117, 0, 0}, {118, 0, 0}, {133, 0, 0}, \
    {134, 0, 0}, {135, 0, 0}, {136, 0, 0}, {137, 0, 0}, {138, 0, 0}, {139, 0, 0}, {140, 0, 0}, {141, 0, 0}, {142, 0, 0}, {143, 0, 0}, \
    {144, 0, 0}, {145, 0, 0}, {146, 0, 0}, {147, 0, 0}, {148, 0, 0}, {149, 0, 0}, {150, 0, 0}, {151, 0, 0}, {152, 0, 0}, {153, 0, 0}, \
    {154, 0, 0}, {155, 0, 0}, {168, 0, 0}, {182, 0, 0}, {183, 0, 0}, {184, 0, 0}, {185, 0, 0}, {186, 0, 0}, {187, 0, 0}, {188, 0, 0}, \
    {189, 0, 0}, {190, 0, 0}, {191, 0, 0}, {192, 0, 0}, {193, 0, 0}, {194, 0, 0}, {195, 0, 0}, {196, 0, 0}, {197, 0, 0}, {198, 0, 0}, \
    {199, 0, 0}, {200, 0, 0}, {214, 0, 0}, {215, 0, 0}, {216, 0, 0}, {217, 0, 0}, {218, 0, 0}, {219, 0, 0}, {220, 0, 0}, {271, 0, 0}, \
    {272, 0, 0}, {273, 0, 0}, {274, 0, 0}, {276, 0, 0}, {376, 0, 0}, {378, 0, 0}, {380, 0, 0}, {381, 0, 0}, {382, 0, 0}, {404, 0, 0}, \
    {494, 0, 0}, {495, 0, 0}, {496, 0, 0}, {497, 0, 0}, {498, 0, 0}, {499, 0, 0}, {624, 0, 0}, {625, 0, 0}, {626, 0, 0}, {627, 0, 0}, \
    {628, 0, 0}, {629, 0, 0}, {630, 0, 0}, {637, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, \
    {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, {INDEX_UNDEFINE_VALUE, 0, 0}, \
}

external_dsp_power_on_default_value_t audio_external_dsp_power_on_default_value = {.register_info = EXTERNAL_DSP_QUERY_INDEX};

#endif



//#if PRODUCT_VERSION == 2533
#if 0


#include "bt_mode_dsp_profile.h"
#include "car_mode_dsp_profile.h"

static external_dsp_sending_path_register_value_t audio_external_dsp_sending_path_register_value = {0};

static external_dsp_sending_path_register_value_t audio_external_dsp_sending_path_register_value_bt = {
    .external_dsp_register_value = BT_MODE_DSP_PROFILE_EXTERNAL_DSP_REGISTER_VALUE,
    .need_to_download_index = BT_MODE_DSP_PROFILE_NEED_TO_DOWNLOAD_INDEX
};

static external_dsp_sending_path_register_value_t audio_external_dsp_sending_path_register_value_car = {
    .external_dsp_register_value = CAR_MODE_DSP_PROFILE_EXTERNAL_DSP_REGISTER_VALUE,
    .need_to_download_index = CAR_MODE_DSP_PROFILE_NEED_TO_DOWNLOAD_INDEX
};

#endif




#ifdef MTK_AUDIO_TUNING_ENABLED
void audio_nvdm_get_external_dsp_power_on_default_value_by_memcpy(external_dsp_power_on_default_value_t *external_dsp_power_on_default_value)
{
    memcpy(external_dsp_power_on_default_value, &audio_external_dsp_power_on_default_value, sizeof(audio_external_dsp_power_on_default_value));
}

void audio_tuning_task_main(void *arg)
{
    printf("audio_tuning_task_main: get dsp power on default value\r\n");

    if (external_dsp_is_power_on()) {
        printf("audio_tuning_task_main: note dsp already power on, so the value we get may not power on default value\r\n");
        int i = 0;
        uint32_t register_index;
        uint16_t register_value;        

        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
            register_index = (uint32_t)audio_external_dsp_power_on_default_value.register_info[i].index_value;
            if(register_index != INDEX_UNDEFINE_VALUE) {
                if (external_dsp_read_parameter(register_index, &register_value) != EXTERNAL_DSP_STATUS_ERROR) {
                    audio_external_dsp_power_on_default_value.register_info[i].register_value= (uint16_t)register_value;
                    printf("audio_nvdm_update_external_dsp_power_on_default_value: index=%3ld, value=%x\r\n", register_index, register_value);
                } else {
                    printf("audio_nvdm_update_external_dsp_power_on_default_value read dsp fail: index=%3ld, value=%x\r\n", register_index, register_value);
                }
            }
        }
    } else {
        printf("audio_nvdm_update_external_dsp_power_on_default_value: dsp didn't active. Active it\r\n");
        external_dsp_activate(true);
        external_dsp_switch_pdm_clock(true);

        int i = 0;
        uint32_t register_index;
        uint16_t register_value;

        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
            register_index = (uint32_t)audio_external_dsp_power_on_default_value.register_info[i].index_value;
            if(register_index != INDEX_UNDEFINE_VALUE) {
                if (external_dsp_read_parameter(register_index, &register_value) != EXTERNAL_DSP_STATUS_ERROR) {
                    audio_external_dsp_power_on_default_value.register_info[i].register_value= (uint16_t)register_value;
                    printf("audio_nvdm_update_external_dsp_power_on_default_value: index=%3ld, value=%x\r\n", register_index, register_value);
                } else {
                    printf("audio_nvdm_update_external_dsp_power_on_default_value read dsp fail: index=%3ld, value=%x\r\n", register_index, register_value);
                }
            }
        }
        external_dsp_switch_pdm_clock(false);
        external_dsp_activate(false);
    }

    vTaskDelete(NULL);
}

void audio_tuning_task_create(void)
{
    xTaskCreate(audio_tuning_task_main, "audio_tuning_task", (512*4) / sizeof(portSTACK_TYPE), NULL, TASK_PRIORITY_SOFT_REALTIME, NULL);
    return;
}

#endif

audio_eaps_t audio_get_audio_default_coefficient()
{
    audio_eaps_t audio_eaps = {
        .speech_common_parameter.speech_common_parameter = SPEECH_COMMON_PARAMETER,

        .audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain = AUDIO_HANDSFREE_GAIN_STREAM_OUT_GAIN,
        .audio_parameter.handsfree_headset_stream_out_gain.headset_gain = AUDIO_HEADSET_GAIN_STREAM_OUT_GAIN,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[0].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER1,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[1].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER2,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[2].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER3,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[3].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER4,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[4].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER5,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[5].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER6,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[6].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER7,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[7].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER8,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[8].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER9,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[9].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER10,
        .audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[10].audio_iir_design_parameter = AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER11,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[0].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER1,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[1].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER2,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[2].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER3,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[3].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER4,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[4].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER5,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[5].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER6,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[6].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER7,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[7].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER8,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[8].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER9,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[9].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER10,
        .audio_parameter.audio_post_processing_parameter[1].audio_post_processing_compensation_filter[10].audio_iir_design_parameter = AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER11,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[0].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER1,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[1].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER2,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[2].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER3,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[3].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER4,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[4].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER5,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[5].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER6,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[6].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER7,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[7].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER8,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[8].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER9,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[9].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER10,
        .audio_parameter.audio_post_processing_parameter[2].audio_post_processing_compensation_filter[10].audio_iir_design_parameter = AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER11,

        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.stream_out_gain = VOICE_NB_MODE1_GAIN_STREAM_OUT_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.stream_in_gain = VOICE_NB_MODE1_GAIN_STREAM_IN_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.sidetone_gain = VOICE_NB_MODE1_GAIN_SIDETONE_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_mode_parameter = VOICE_NB_MODE1_NB_MODE_PARAMETER,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient = VOICE_NB_MODE1_NB_STREAM_IN_FIR_COEFFICIENT,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_frequency = VOICE_NB_MODE1_NB_STREAM_IN_FIR_FREQUENCY,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_response = VOICE_NB_MODE1_NB_STREAM_IN_FIR_RESPONSE,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient = VOICE_NB_MODE1_NB_STREAM_OUT_FIR_COEFFICIENT,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency = VOICE_NB_MODE1_NB_STREAM_OUT_FIR_FREQUENCY,
        .voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_response = VOICE_NB_MODE1_NB_STREAM_OUT_FIR_RESPONSE,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_out_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_OUT_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_in_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_IN_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.sidetone_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_GAIN_SIDETONE_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_mode_parameter = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_MODE_PARAMETER,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_STREAM_OUT_FIR_COEFFICIENT,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_STREAM_OUT_FIR_FREQUENCY,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_response = VOICE_NB_WITH_EXTERNAL_DSP_MODE1_NB_STREAM_OUT_FIR_RESPONSE,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_gain.stream_out_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_OUT_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_gain.stream_in_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_IN_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_gain.sidetone_gain = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_GAIN_SIDETONE_GAIN,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_mode_parameter = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_MODE_PARAMETER,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_STREAM_OUT_FIR_COEFFICIENT,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_STREAM_OUT_FIR_FREQUENCY,
        .voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1].voice_nb_enhancement_parameter.nb_stream_out_fir_response = VOICE_NB_WITH_EXTERNAL_DSP_MODE2_NB_STREAM_OUT_FIR_RESPONSE,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.stream_out_gain = VOICE_WB_MODE1_GAIN_STREAM_OUT_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.stream_in_gain = VOICE_WB_MODE1_GAIN_STREAM_IN_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.sidetone_gain = VOICE_WB_MODE1_GAIN_SIDETONE_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_mode_parameter = VOICE_WB_MODE1_WB_MODE_PARAMETER,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient = VOICE_WB_MODE1_WB_STREAM_IN_FIR_COEFFICIENT,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_frequency = VOICE_WB_MODE1_WB_STREAM_IN_FIR_FREQUENCY,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_response = VOICE_WB_MODE1_WB_STREAM_IN_FIR_RESPONSE,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient = VOICE_WB_MODE1_WB_STREAM_OUT_FIR_COEFFICIENT,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency = VOICE_WB_MODE1_WB_STREAM_OUT_FIR_FREQUENCY,
        .voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_response = VOICE_WB_MODE1_WB_STREAM_OUT_FIR_RESPONSE,      
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_out_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_OUT_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_in_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_IN_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.sidetone_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_SIDETONE_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_mode_parameter = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_MODE_PARAMETER,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_STREAM_OUT_FIR_COEFFICIENT,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_STREAM_OUT_FIR_FREQUENCY,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_response = VOICE_WB_WITH_EXTERNAL_DSP_MODE1_WB_STREAM_OUT_FIR_RESPONSE,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_gain.stream_out_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_OUT_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_gain.stream_in_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_IN_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_gain.sidetone_gain = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_SIDETONE_GAIN,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_mode_parameter = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_MODE_PARAMETER,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_STREAM_OUT_FIR_COEFFICIENT,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_STREAM_OUT_FIR_FREQUENCY,
        .voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1].voice_wb_enhancement_parameter.wb_stream_out_fir_response = VOICE_WB_WITH_EXTERNAL_DSP_MODE2_WB_STREAM_OUT_FIR_RESPONSE,
        
        .record_parameter.record_wb_parameter[0].stream_in_gain = RECORD_WB_MODE1_GAIN_STREAM_IN_GAIN,
        .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_mode_parameter = RECORD_WB_MODE1_WB_MODE_PARAMETER,
        .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient = RECORD_WB_MODE1_WB_CHANNEL1_FIR_COEFFICIENT,
        .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_frequency = RECORD_WB_MODE1_WB_CHANNEL1_FIR_FREQUENCY,
        .record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_response = RECORD_WB_MODE1_WB_CHANNEL1_FIR_RESPONSE,
        .record_parameter.record_wb_parameter_with_external_dsp[0].stream_in_gain = RECORD_WB_WITH_EXTERNAL_DSP_MODE1_GAIN_STREAM_IN_GAIN,
        .record_parameter.record_wb_parameter_with_external_dsp[0].record_wb_enhancement_parameter.external_dsp_sending_path_register_info = RECORD_WB_WITH_EXTERNAL_DSP_MODE1_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO,
        .record_parameter.record_wb_parameter_with_external_dsp[1].stream_in_gain = RECORD_WB_WITH_EXTERNAL_DSP_MODE2_GAIN_STREAM_IN_GAIN,
        .record_parameter.record_wb_parameter_with_external_dsp[1].record_wb_enhancement_parameter.external_dsp_sending_path_register_info = RECORD_WB_WITH_EXTERNAL_DSP_MODE2_EXTERNAL_DSP_SENDING_PATH_REGISTER_INFO
    };

    return audio_eaps;
}

void audio_nvdm_init()
{
    uint32_t size;

    LOGI("audio nvdm init ++\n");

    size = sizeof(audio_eaps.speech_common_parameter);
    if (nvdm_read_data_item("EAPS", "COMMON", (uint8_t *)&audio_eaps.speech_common_parameter, &size) == NVDM_STATUS_OK) {
        LOGI("audio nvdm has data, get parameters from nvdm\n");

        size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[0]);
        if (nvdm_read_data_item("EAPS", "AUDIO_1", (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:AUDIO_1 fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[1]);
        if (nvdm_read_data_item("EAPS", "AUDIO_2", (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[1], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:AUDIO_2 fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[2]);
        if (nvdm_read_data_item("EAPS", "AUDIO_3", (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[2], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:AUDIO_3 fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.handsfree_headset_stream_out_gain);
        if (nvdm_read_data_item("EAPS", "AUDIO_GAIN", (uint8_t *)&audio_eaps.audio_parameter.handsfree_headset_stream_out_gain, &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:AUDIO_GAIN fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter[0]);
        if (nvdm_read_data_item("EAPS", "VOICE_NB_1", (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:VOICE_NB_1 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0]);
        if (nvdm_read_data_item("EAPS", "VOICE_NB_4", (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:VOICE_NB_4 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1]);
        if (nvdm_read_data_item("EAPS", "VOICE_NB_5", (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:VOICE_NB_5 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter[0]);
        if (nvdm_read_data_item("EAPS", "VOICE_WB_1", (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:VOICE_WB_1 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0]);
        if (nvdm_read_data_item("EAPS", "VOICE_WB_4", (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:VOICE_WB_4 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1]);
        if (nvdm_read_data_item("EAPS", "VOICE_WB_5", (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:VOICE_WB_5 fail\n");
        }

        size = sizeof(audio_eaps.record_parameter.record_wb_parameter[0]);
        if (nvdm_read_data_item("EAPS", "RECORD_WB_1", (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:AUDIO_1 fail\n");
        }

        size = sizeof(audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[0]);
        if (nvdm_read_data_item("EAPS", "RECORD_WB_2", (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[0], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:RECORD_WB_2 fail\n");
        }

        size = sizeof(audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[1]);
        if (nvdm_read_data_item("EAPS", "RECORD_WB_3", (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[1], &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EAPS:RECORD_WB_3 fail\n");
        }
    } else {
        LOGI("audio nvdm has no data, write default value to nvdm\n");

        //audio_eaps = audio_get_audio_default_coefficient();   // <-- In some project, their system statck size is small. In audio_get_audio_default_coefficient() API, the local parameter will exceed statck size. In oder to solve this problem, we create golbal audio_eaps with default value as audio_default_coefficien.h 

        size = sizeof(audio_eaps.speech_common_parameter);
        if (nvdm_write_data_item("EAPS", "COMMON", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.speech_common_parameter, size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:COMMON fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[0]);
        if (nvdm_write_data_item("EAPS", "AUDIO_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:AUDIO_1 fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[1]);
        if (nvdm_write_data_item("EAPS", "AUDIO_2", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[1], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:AUDIO_2 fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[2]);
        if (nvdm_write_data_item("EAPS", "AUDIO_3", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[2], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:AUDIO_3 fail\n");
        }

        size = sizeof(audio_eaps.audio_parameter.handsfree_headset_stream_out_gain);
        if (nvdm_write_data_item("EAPS", "AUDIO_GAIN", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.handsfree_headset_stream_out_gain, size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:AUDIO_GAIN fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter[0]);
        if (nvdm_write_data_item("EAPS", "VOICE_NB_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:VOICE_NB_1 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0]);
        if (nvdm_write_data_item("EAPS", "VOICE_NB_4", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:VOICE_NB_4 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1]);
        if (nvdm_write_data_item("EAPS", "VOICE_NB_5", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:VOICE_NB_5 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter[0]);
        if (nvdm_write_data_item("EAPS", "VOICE_WB_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:VOICE_WB_1 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0]);
        if (nvdm_write_data_item("EAPS", "VOICE_WB_4", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:VOICE_WB_4 fail\n");
        }

        size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1]);
        if (nvdm_write_data_item("EAPS", "VOICE_WB_5", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:VOICE_WB_5 fail\n");
        }

        size = sizeof(audio_eaps.record_parameter.record_wb_parameter[0]);
        if (nvdm_write_data_item("EAPS", "RECORD_WB_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:RECORD_WB_1 fail\n");
        }

        size = sizeof(audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[0]);
        if (nvdm_write_data_item("EAPS", "RECORD_WB_2", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[0], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:RECORD_WB_2 fail\n");
        }

        size = sizeof(audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[1]);
        if (nvdm_write_data_item("EAPS", "RECORD_WB_3", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[1], size) != NVDM_STATUS_OK) {
            LOGW("write NVDM EAPS:RECORD_WB_3 fail\n");
        }
    }


//#if PRODUCT_VERSION == 2533
#if 0

    printf("piter\r\n");
    size = sizeof(audio_external_dsp_sending_path_register_value.external_dsp_register_value);
    if (nvdm_read_data_item("EXTERNAL_DSP_RG", "RG_VALUE", (uint8_t *)&audio_external_dsp_sending_path_register_value.external_dsp_register_value, &size) == NVDM_STATUS_OK) {
        LOGI("audio nvdm has external dsp RG data, get data from nvdm\n");

        size = sizeof(audio_external_dsp_sending_path_register_value.need_to_download_index);
        if (nvdm_read_data_item("EXTERNAL_DSP_RG", "DOWNLOAD_INDEX", (uint8_t *)&audio_external_dsp_sending_path_register_value.need_to_download_index, &size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EXTERNAL_DSP_RG:DOWNLOAD_INDEX fail\n");
        }
    } else {
        LOGI("audio nvdm has no external dsp RG data, write default value to nvdm\n");

        memset(&audio_external_dsp_sending_path_register_value, 0, sizeof(audio_external_dsp_sending_path_register_value));

        size = sizeof(audio_external_dsp_sending_path_register_value.external_dsp_register_value);
        if (nvdm_write_data_item("EXTERNAL_DSP_RG", "RG_VALUE", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_external_dsp_sending_path_register_value.external_dsp_register_value, size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EXTERNAL_DSP_RG:RG_VALUE\n");
        }


        size = sizeof(audio_external_dsp_sending_path_register_value.need_to_download_index);
        if (nvdm_write_data_item("EXTERNAL_DSP_RG", "DOWNLOAD_INDEX", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_external_dsp_sending_path_register_value.need_to_download_index, size) != NVDM_STATUS_OK) {
            LOGW("read NVDM EXTERNAL_DSP_RG:DOWNLOAD_INDEX\n");
        }

    }

#endif

#if defined(__AFE_HS_DC_CALIBRATION__) //HP can be added in the future
/*DC compensation value*/
{
    int ret = 0;
    size = sizeof(audio_nvdm_dc_calibration.calibration_sta);
    if (nvdm_read_data_item("DCCAL", "STA", (uint8_t *)&audio_nvdm_dc_calibration.calibration_sta, &size) != NVDM_STATUS_OK) {
        ret = -1;
        LOGW("Read DC calibration status fail.\n");
    }
    //HS compensation value
    size = sizeof(audio_nvdm_dc_calibration.hs_dc_compensate_value);
    if (nvdm_read_data_item("DCCAL", "DC_HS", (uint8_t *)&audio_nvdm_dc_calibration.hs_dc_compensate_value, &size) != NVDM_STATUS_OK) {
        ret = -1;
        LOGW("Read DC calibration value fail.\n");
    }
    if (ret == 0) {
        LOGI("Read HS DC compensation value =%x.\n", audio_nvdm_dc_calibration.hs_dc_compensate_value);
        afe_nvdm_set_data(audio_nvdm_dc_calibration);
    }
}
#endif

#ifdef MTK_AUDIO_TUNING_ENABLED
    audio_tuning_task_create();
#endif


    LOGI("audio nvdm init --\n");

}

audio_eaps_t audio_nvdm_get_eaps_data()
{
    return audio_eaps;
}

void audio_nvdm_get_eaps_data_by_memcpy(audio_eaps_t *eaps)
{
    memcpy(eaps, &audio_eaps, sizeof(audio_eaps));
}

const audio_eaps_t* audio_nvdm_get_global_eaps_address(void)
{
    return &audio_eaps;
}

static void audio_nvdm_callback_eaps_data_is_changed_callback_function(void)
{
    int32_t i = 0;

    for (i = 0; i < MAX_CALLBACK_NUMBER; i++) {
        if (audio_nvdm_eaps_data_is_changed_callback[i].callback_id != NULL) {
            LOGI("eaps data is changed. callback the registered callback funtion\n");

            audio_nvdm_eaps_data_is_changed_callback[i].callback_function(audio_nvdm_eaps_data_is_changed_callback[i].data);
        }
    }
}


int audio_nvdm_unregister_eaps_is_changed_callback(audio_nvdm_callback_id *callback_id)
{
    int32_t i = 0;

    for (i = 0; i < MAX_CALLBACK_NUMBER; i++) {
        if (audio_nvdm_eaps_data_is_changed_callback[i].callback_id == callback_id) {
            audio_nvdm_eaps_data_is_changed_callback[i].callback_id = NULL;
            audio_nvdm_eaps_data_is_changed_callback[i].callback_function = NULL;
            audio_nvdm_eaps_data_is_changed_callback[i].data = NULL;
            LOGI("unregister success\n");
            return 0;
        }
    }

    LOGI("didn't register before\n");

    return -1;
}


int audio_nvdm_register_eaps_is_changed_callback(audio_nvdm_callback_id *callback_id, audio_nvdm_callback_funtion_t func, void *data)
{
    int32_t i = 0;


    for (i = 0; i < MAX_CALLBACK_NUMBER; i++) {
        if (audio_nvdm_eaps_data_is_changed_callback[i].callback_id == callback_id) {
            LOGI("already register. please unregister first\n");
            return -1;
        }
    }


    for (i = 0; i < MAX_CALLBACK_NUMBER; i++) {
        if (audio_nvdm_eaps_data_is_changed_callback[i].callback_id == NULL) {
            audio_nvdm_eaps_data_is_changed_callback[i].callback_id = callback_id;
            audio_nvdm_eaps_data_is_changed_callback[i].callback_function = func;
            audio_nvdm_eaps_data_is_changed_callback[i].data = data;

            LOGI("register success i=%d\n", i);
            return 0;
        }
    }


    LOGI("over max eaps data is changed callback function can be registered\n");

    return -1;
}



int audio_nvdm_save_eaps_data_to_nvdm(audio_eaps_t eaps)
{
    uint32_t size;
    int ret = 0;

    size = sizeof(audio_eaps.speech_common_parameter);
    if (nvdm_write_data_item("EAPS", "COMMON", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.speech_common_parameter, size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:COMMON fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[0]);
    if (nvdm_write_data_item("EAPS", "AUDIO_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:AUDIO_1 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[1]);
    if (nvdm_write_data_item("EAPS", "AUDIO_2", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[1], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:AUDIO_2 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.audio_parameter.audio_post_processing_parameter[2]);
    if (nvdm_write_data_item("EAPS", "AUDIO_3", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.audio_post_processing_parameter[2], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:AUDIO_3 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.audio_parameter.handsfree_headset_stream_out_gain);
    if (nvdm_write_data_item("EAPS", "AUDIO_GAIN", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.audio_parameter.handsfree_headset_stream_out_gain, size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:AUDIO_GAIN fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter[0]);
    if (nvdm_write_data_item("EAPS", "VOICE_NB_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:VOICE_NB_1 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0]);
    if (nvdm_write_data_item("EAPS", "VOICE_NB_4", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:VOICE_NB_4 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1]);
    if (nvdm_write_data_item("EAPS", "VOICE_NB_5", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[1], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:VOICE_NB_5 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter[0]);
    if (nvdm_write_data_item("EAPS", "VOICE_WB_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:VOICE_WB_1 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0]);
    if (nvdm_write_data_item("EAPS", "VOICE_WB_4", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:VOICE_WB_4 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1]);
    if (nvdm_write_data_item("EAPS", "VOICE_WB_5", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[1], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:VOICE_WB_5 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.record_parameter.record_wb_parameter[0]);
    if (nvdm_write_data_item("EAPS", "RECORD_WB_1", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:RECORD_WB_1 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[0]);
    if (nvdm_write_data_item("EAPS", "RECORD_WB_2", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[0], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:RECORD_WB_2 fail\n");
        ret = -1;
    }

    size = sizeof(audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[1]);
    if (nvdm_write_data_item("EAPS", "RECORD_WB_3", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_eaps.record_parameter.record_wb_parameter_with_external_dsp[1], size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EAPS:RECORD_WB_3 fail\n");
        ret = -1;
    }


    audio_nvdm_callback_eaps_data_is_changed_callback_function();

    return ret;
}

int audio_nvdm_set_eaps_data(audio_eaps_t eaps)
{
    audio_eaps = eaps;

    return 1;
}



//#if PRODUCT_VERSION == 2533
#if 0



external_dsp_sending_path_register_value_t audio_nvdm_get_external_dsp_register_profile(dsp_register_profile_t dsp_register_profile)
{
    if (dsp_register_profile == DSP_REGISTER_PROFILE_BT) {
        return audio_external_dsp_sending_path_register_value_bt;
    } else if (dsp_register_profile == DSP_REGISTER_PROFILE_CAR) {
        return audio_external_dsp_sending_path_register_value_car;
    } else {
        return audio_external_dsp_sending_path_register_value;
    }
}


void audio_nvdm_get_external_dsp_register_value_by_memcpy(dsp_register_profile_t dsp_register_profile, external_dsp_sending_path_register_value_t *external_dsp_sending_path_register_value)
{
#ifndef MTK_AUDIO_TUNING_ENABLED
    if (dsp_register_profile == DSP_REGISTER_PROFILE_BT) {
        audio_external_dsp_sending_path_register_value = audio_external_dsp_sending_path_register_value_bt;
    } else if (dsp_register_profile == DSP_REGISTER_PROFILE_CAR) {
        audio_external_dsp_sending_path_register_value = audio_external_dsp_sending_path_register_value_car;
    } else { // (dsp_register_profile == DSP_REGISTER_PROFILE_TUNING)
        // do nothing
    }
#endif


    memcpy(external_dsp_sending_path_register_value, &audio_external_dsp_sending_path_register_value, sizeof(audio_external_dsp_sending_path_register_value));
}

void audio_nvdm_set_external_dsp_register_value(external_dsp_sending_path_register_value_t external_dsp_sending_path_register_value)
{
    audio_external_dsp_sending_path_register_value = external_dsp_sending_path_register_value;
}


uint16_t audio_nvdm_read_external_dsp_register_value_from_nvdm()
{
    uint32_t size;
    int ret = 0;
    int16_t i = 0;

    // note: the maximum size of each item in nvdm is 2k. The Maximum size of name is 16 characters.


    size = sizeof(audio_external_dsp_sending_path_register_value.external_dsp_register_value);
    if (nvdm_read_data_item("EXTERNAL_DSP_RG", "RG_VALUE", (uint8_t *)&audio_external_dsp_sending_path_register_value.external_dsp_register_value, &size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EXTERNAL_DSP_RG:RG_VALUE fail\n");
    }


    size = sizeof(audio_external_dsp_sending_path_register_value.need_to_download_index);
    if (nvdm_read_data_item("EXTERNAL_DSP_RG", "DOWNLOAD_INDEX", (uint8_t *)&audio_external_dsp_sending_path_register_value.need_to_download_index, &size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EXTERNAL_DSP_RG:DOWNLOAD_INDEX fail\n");
    }

    return ret;

}


uint16_t audio_nvdm_save_external_dsp_register_value_to_nvdm()
{
    uint32_t size;
    int ret = 1;

    // note: the maximum size of each item in nvdm is 2k. The Maximum size of name is 16 characters.

    size = sizeof(audio_external_dsp_sending_path_register_value.external_dsp_register_value);
    if (nvdm_write_data_item("EXTERNAL_DSP_RG", "RG_VALUE", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_external_dsp_sending_path_register_value.external_dsp_register_value, size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EXTERNAL_DSP_RG:RG_VALUE\n");
        ret = -1;
    }


    size = sizeof(audio_external_dsp_sending_path_register_value.need_to_download_index);
    if (nvdm_write_data_item("EXTERNAL_DSP_RG", "DOWNLOAD_INDEX", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_external_dsp_sending_path_register_value.need_to_download_index, size) != NVDM_STATUS_OK) {
        LOGW("read NVDM EXTERNAL_DSP_RG:DOWNLOAD_INDEX\n");
        ret = -1;
    }

    if (ret > 0) {
        audio_nvdm_callback_eaps_data_is_changed_callback_function();
    }

    return ret;
}



#endif

#if defined(__AFE_HS_DC_CALIBRATION__) //HP can be added in the future
int16_t audio_nvdm_save_dc_data_to_nvdm(void)
{
    uint32_t size;
    afe_dc_calibration_t afe_dc_calibration;
    int ret = 1;

    if (audio_nvdm_dc_calibration.calibration_sta == 1) { // Not the first boot up, skip.
        return ret;
    }

    afe_dc_calibration = afe_get_dc_calibration_data();
    audio_nvdm_dc_calibration.calibration_sta = afe_dc_calibration.calibration_sta;
    audio_nvdm_dc_calibration.hs_dc_compensate_value = afe_dc_calibration.hs_dc_compensate_value;

    size = sizeof(audio_nvdm_dc_calibration.calibration_sta);
    if (nvdm_write_data_item("DCCAL", "STA", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_nvdm_dc_calibration.calibration_sta, size) != NVDM_STATUS_OK) {
        LOGW("Write DC calibration status fail.\n");
        ret = -1;
    }
    //HS compensation value
    size = sizeof(audio_nvdm_dc_calibration.hs_dc_compensate_value);
    if (nvdm_write_data_item("DCCAL", "DC_HS", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&audio_nvdm_dc_calibration.hs_dc_compensate_value, size) != NVDM_STATUS_OK) {
        LOGW("Write DC calibration value fail.\n");
        ret = -1;
    }
    LOGI("Save HS DC compensation sta=%d, value =%x.\n", audio_nvdm_dc_calibration.calibration_sta, audio_nvdm_dc_calibration.hs_dc_compensate_value);
    return ret;
}
#endif
