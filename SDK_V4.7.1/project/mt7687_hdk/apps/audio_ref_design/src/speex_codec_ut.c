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


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
//#include "debug_interaction.h"
#include "FreeRTOS.h"
#include "task.h"
#include "speex_codec_ut.h"
#include "speex_codec.h"
#include "hal_log.h"
#include "ff.h"


static FATFS fatfs;
static FIL fdst;
static FRESULT res;
static UINT length_read;
static uint8_t first_time_f_mount = 1;
uint8_t share_buffer[4096];  //share buffer

static int16_t test_open_file_from_sd(FIL *fp, const TCHAR *path)
{
    FRESULT res;

    if (first_time_f_mount){
        res = f_mount(&fatfs, _T("0:"), 1);
        first_time_f_mount = 0;
    } else {
        log_hal_info("already fmount\n");
        res = FR_OK;
    }

    if (!res) {
        log_hal_info("fmount ok \n");
        res = f_open(fp, path, FA_OPEN_EXISTING |FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return -1;
        }
    } else {
        log_hal_info("fmount error \n");
    }

    if (res != FR_OK) {
        return -1;
    } else {
        return 0;
    }
}

static void test_stop_read_from_sd(speex_codec_media_handle_t *hdl)
{
    //hdl->stop(hdl);
    res = f_close(&fdst);
    if (!res) {
        log_hal_info("fclose success \n");
    } else {
        log_hal_info("fclose error \n");
    }
}


static void test_sd_event_callback(speex_codec_media_handle_t *hdl, speex_codec_media_event_t event)
{
    uint8_t *share_buf;
    uint32_t share_buf_len;

    switch (event) {
        case SPEEX_CODEC_MEDIA_EVENT_REQUEST:       
            if (f_eof(&fdst)) {
                log_hal_info("[EOF]End of this file. Stop decode\n");
                test_stop_read_from_sd(hdl);
                return;
            }
            uint32_t loop_idx;
            uint32_t loop_cnt = 2;
            for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
                hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
                f_read(&fdst, share_buf, share_buf_len, &length_read);
                log_hal_info("share_buf_len SD=%d", share_buf_len);
                log_hal_info("length_read from SD=%d", length_read);
                hdl->write_data_done(hdl, share_buf_len);
            }
            hdl->finish_write_data(hdl);
            break;
    }
}


/**Notice
**  (1)nb_female.en file on SD card is encoded by speex_dec_demo (speex_demo.c)
**  (2)Please visit http://www.speex.org/ for the usage about speex codec.
**  (3)This DEMO UT will read Speex stream from SD card and output it to NAU8810 
**/
void speex_codec_ut(void *pvParameters)
{

    speex_codec_media_handle_t *hdl = NULL;
    speex_codec_config_t speex_codec_config;
    uint8_t *share_buf;
    uint32_t share_buf_len;

    speex_codec_config.speex_codec_mode=SPEEX_CODEC_NB_MODE;
    speex_codec_config.speex_codec_channer_number=SPEEX_CODEC_MONO;
    speex_codec_config.speex_codec_sample_rate=SPEEX_CODEC_SAMPLE_RATE_8K;


    log_hal_info("speex_codec_ut\n");
    hdl = speex_codec_open(test_sd_event_callback, speex_codec_config);
    if (hdl == NULL) {
        log_hal_error("speex_codec_open fail.\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    if (test_open_file_from_sd(&fdst, _T("SD:/speex/nb_female.en")) < 0) {
        log_hal_error("Fail to open SD:/speex/nb_female.en\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    hdl->set_share_buffer(hdl, share_buffer, 4096);

    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    log_hal_info("length_read from SD=%d", length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);

    log_hal_info("start to decode\n");
    hdl->decode(hdl);
    //vTaskDelay(20000 / portTICK_RATE_MS);
   
    while (1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}


