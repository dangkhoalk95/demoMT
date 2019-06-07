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
#include "mp3_codec_demo.h"
#include "mp3_codec.h"
#include "hal_audio.h"
#include "hal_log.h"
#include "ff.h"
#include "hal_gpt.h"

#define MP3_CODEC_UT_PLAY_PAUSE_RESUME_10TIMES (0) // for QA test only SDK v4.5.0

static FATFS fatfs;
static FIL fdst;
static FRESULT res;
static UINT length_read;
static uint8_t first_time_f_mount = 1;

static int16_t test_open_file_from_sd(FIL *fp, const TCHAR *path)
{
    FRESULT res;
    printf("test_open_file_from_sd() +\r\n");
    if (first_time_f_mount){
        printf("f_mount() +\r\n");
        res = f_mount(&fatfs, _T("0:"), 1);
        printf("f_mount() -\r\n");
        first_time_f_mount = 0;
    } else {
        printf("already fmount\r\n");
        res = FR_OK;
    }
    if (!res) {
        printf("fmount ok \r\n");
        res = f_open(fp, path, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
        if (!res) {
            printf("fopen ok \r\n");
        } else {
            printf("fopen error \r\n");
            return -1;
        }
    } else {
        printf("fmount error \r\n");
    }
    if (res != FR_OK) {
        return -1;
    } else {
        return 0;
    }
    printf("test_open_file_from_sd() -\r\n");
}

static void test_stop_read_from_sd(mp3_codec_media_handle_t *hdl)
{
    hdl->stop(hdl);
    res = f_close(&fdst);
    if (!res) {
        printf("[MP3 Codec Demo] fclose success \r\n");
    } else {
        printf("[MP3 Codec Demo] fclose error \r\n");
    }
}

static void test_sd_event_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event)
{
    uint8_t *share_buf;
    uint32_t share_buf_len;
    switch (event) {
        case MP3_CODEC_MEDIA_JUMP_FILE_TO:
            res = f_lseek(&fdst, (DWORD)hdl->jump_file_to_specified_position);
            // printf("[MP3 Codec Demo] MP3_CODEC_MEDIA_JUMP_FILE_TO: fdst read/write pointer=%x\r\n", f_tell(&fdst));
            break;
        case MP3_CODEC_MEDIA_REQUEST:
        case MP3_CODEC_MEDIA_UNDERFLOW:
            if (f_eof(&fdst)) {
                printf("[MP3 Codec Demo] [EOF]End of this mp3 file. Stop mp3\r\n");
                test_stop_read_from_sd(hdl);
                break;
            }
            uint32_t loop_idx;
            uint32_t loop_cnt = 2;
            for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
                hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
                f_read(&fdst, share_buf, share_buf_len, &length_read);
                hdl->write_data_done(hdl, share_buf_len);
            }
            hdl->finish_write_data(hdl);
            break;
    }
}

void mp3_codec_demo_play_stop_StereoMono_SR_8K_to_48K()
{
    mp3_codec_media_handle_t *hdl = NULL;
    uint8_t *share_buf;
    uint32_t share_buf_len;

    // Open the mp3 codec.
    printf("[MP3 Codec Demo] open mp3 codec\r\n");
    hdl = mp3_codec_open(test_sd_event_callback);
    if (hdl == NULL) {
        printf("[MP3 Codec Demo] Fail to open the codec.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    printf("[MP3 Codec Demo] mp3_codec_set_memory2() +\r\n");
    mp3_codec_set_memory2();
    printf("[MP3 Codec Demo] mp3_codec_set_memory2() -\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/08mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/08mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 08mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 08mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/08st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/08st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 08st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 08st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/11mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/11mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 11mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 11mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/11st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/11st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 11st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 11st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/12mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/12mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 12mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 12mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/12st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/12st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 12st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 12st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/16mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/16mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 16mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 16mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/16st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/16st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 16st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 16st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/22mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/22mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 22mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 22mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/22st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/22st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 22st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 22st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/24mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/24mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 24mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 24mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/24st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/24st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 24st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 24st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/32mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/32mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 32mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 32mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/32st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/32st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 32st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 32st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/44mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/44mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 44mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 44mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/44st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/44st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 44st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 44st_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/48mo_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/48mo_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 48mo_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 48mo_002.mp3\r\n");

    if (test_open_file_from_sd(&fdst, _T("SD:/temp/48st_002.mp3")) < 0) {
        printf("[MP3 Codec Demo] Fail to open SD:/temp/48st_002.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);
    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
    printf("[MP3 Codec Demo] play 48st_002.mp3\r\n");
    hdl->play(hdl);
    vTaskDelay(30000 / portTICK_RATE_MS);
    test_stop_read_from_sd(hdl);
    printf("[MP3 Codec Demo] Stop 48st_002.mp3\r\n");


    hdl->stop(hdl);

}


