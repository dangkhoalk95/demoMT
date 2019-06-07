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


#ifndef __SPEEX_CODEC_INTERNAL_H__
#define __SPEEX_CODEC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <speex/speex.h>

#include "FreeRTOS.h"
#include "hal_gpt.h"
#include "task.h"
#include "queue.h"
#include "syslog.h"
#include <string.h>
#include "memory_attribute.h"
#include "task_def.h"
#include "hal_platform.h"
#include "hal_gpt.h"
#include "hal_log.h"
#include "speex_codec.h"


typedef enum {
    SPEEX_CODEC_QUEUE_EVENT_NONE = 0,
    SPEEX_CODEC_QUEUE_EVENT_DECODE,
    SPEEX_CODEC_QUEUE_EVENT_ENCODE,
    SPEEX_CODEC_QUEUE_EVENT_FILL_DATA,
    SPEEX_CODEC_QUEUE_EVENT_TOTAL
} speex_codec_queue_event_id_t;


typedef struct {
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t buffer_byte_count;
    uint8_t *buffer_base_pointer;
} ring_buffer_information_t;


typedef struct {
    speex_codec_media_handle_t    handle;                       /** SPEEX codec handle */
    ring_buffer_information_t     stream_out_pcm_buff;          /** This buffer contains the pcm data that are ready to play */

    SpeexBits                     enc_bits;                     /** SPEEX codec IP used. Holds bits so they can be read and written to by the Speex routines */
    SpeexBits                     dec_bits;                     /** SPEEX codec IP used. Holds bits so they can be read and written to by the Speex routines */
    void                          *enc_state;                   /** SPEEX codec IP used. Holds the state of the encoder */
    void                          *dec_state;                   /** SPEEX codec IP used. Holds the state of the decoder */
    int16_t                       *in;                          /** SPEEX codec IP used. Holds the audio that will be read  from  file (16 bits per sample)  */
    float                         *input;                       /** SPEEX codec IP used. Speex handle samples as float, so we need an array of floats to hold encoded data */
    int16_t                       *out;                         /** SPEEX codec IP used. Holds the audio that will be written to file (16 bits per sample) */
    float                         *output;                      /** SPEEX codec IP used. Speex handle samples as float, so we need an array of floats  to deocded data*/
    int8_t                        *enc_cbits;
    int8_t                        *dec_cbits;
    uint16_t                      frame_size;                   /** SPEEX codec IP used. The frame size*/
    uint16_t                      stream_out_pcm_buff_size;     /**  stream out pcm buffer size */
} speex_codec_internal_handle_t;


typedef struct {
    speex_codec_queue_event_id_t id;
    void *parameter;
} speex_codec_queue_event_t;


typedef void (*speex_codec_internal_callback_t)(void *parameter);


#ifdef __cplusplus
}
#endif

#endif  /*__SPEEX_CODEC_INTERNAL_H__*/

