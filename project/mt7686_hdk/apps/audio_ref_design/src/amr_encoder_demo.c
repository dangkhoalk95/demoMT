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
#include "amr_encoder.h"
#include "amr_encoder_demo.h"
#include "hal_log.h"
#include "ff.h"
#include "hal_gpio.h"

static FATFS fatfs;
static FIL fdst;
static FIL fout;

static FRESULT res;
static UINT length_read, length_written;
static uint8_t first_time_f_mount = 1;
static uint8_t share_buffer[2048];  //share buffer
static uint8_t demo_done = 0;

void do_amr_encoder_demo(void);

static int16_t test_open_file_from_sd(FIL *fp, const TCHAR *path)
{
    FRESULT res;

    if (first_time_f_mount) {
        res = f_mount(&fatfs, _T("0:"), 1);
        first_time_f_mount = 0;
    } else {
        log_hal_info("already fmount\n");
        res = FR_OK;
    }

    if (!res) {
        log_hal_info("fmount ok \n");
        res = f_open(fp, path, FA_OPEN_EXISTING | FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error , res=%d\n", res);
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

static void test_stop_read_from_sd(amr_encoder_handle_t *hdl)
{
    res = f_close(&fdst);
    if (!res) {
        log_hal_info("fclose fdst success \n");
    } else {
        log_hal_error("fclose fdst error \n");
    }
    hdl->encoder_flush(hdl, 1);
}


static void test_sd_event_callback(amr_encoder_handle_t *hdl, amr_encoder_media_event_t event)
{
    uint8_t *share_buf;
    uint32_t share_buf_len;

    uint8_t *base;
    uint32_t len;

    switch (event) {
        case AMR_ENCODER_MEDIA_EVENT_REQUEST: {
            if (f_eof(&fdst)) {
                log_hal_info("[EOF]End of this file. Stop encoder\n");
                test_stop_read_from_sd(hdl);
                break;
            }

            uint32_t loop_idx;
            uint32_t loop_cnt = 2;
            for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
                hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
                f_read(&fdst, share_buf, share_buf_len, &length_read);
                log_hal_info("SD_EVENT:share_buf_len=%d", share_buf_len);
                //log_hal_info("SD_EVENT:length_read from SD=%d", length_read);
                hdl->write_data_done(hdl, length_read);
            }
            hdl->finish_write_data(hdl);
        }
        break;
        case AMR_ENCODER_MEDIA_EVENT_DATA_NOTIFICATION: {
            hdl->get_encoded_data_info(&base, &len);
            //log_hal_info("base=%x len=%d\r\n", base, len);
            res = f_write(&fout, base, len, &length_written);
            if (length_written != len) {
                log_hal_error("get_encoded_data_info error, length_written=%d  len=%d\r\n", length_written, len);
            }
            hdl->get_encoded_data_done(length_written);
        }
        break;
        case AMR_ENCODER_MEDIA_EVENT_FLUSH_DONE: {
            hdl->encoder_stop(hdl);
            res = f_close(&fout);
            if (!res) {
                log_hal_info("fclose fout success \n");
            } else {
                log_hal_error("fclose fout error \n");
            }
            demo_done = 1;
        }
        break;
    }
}

void do_amr_encoder_demo(void)
{

    amr_encoder_handle_t *hdl = NULL;
    amr_encoder_config_t encoder_config;
    uint8_t *share_buf;
    uint32_t share_buf_len;
    FRESULT res;

    encoder_config.bitrate = AMR_ENCODER_12_20KBPS;
    encoder_config.dtx_mode = AMR_ENCODER_DTX_DISABLE;

    log_hal_info("do_amr_encoder_demo start\n");
    hdl = amr_encoder_open(test_sd_event_callback, encoder_config);
    if (hdl == NULL) {
        log_hal_error("amr_encoder_open fail.\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    if (test_open_file_from_sd(&fdst, _T("SD:/AMR/in.pcm")) < 0) {
        log_hal_error("Fail to SD:/AMR/in.pcm\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    hdl->set_share_buffer(hdl, share_buffer, 2048);

    /* Step1: Create a new file for writing . */
    res = f_open(&fout, _T("SD:/AMR/in.amr"), FA_CREATE_ALWAYS);
    if (FR_OK != res) {
        log_hal_error("Failed to create the fout!\n\r");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }

    }

    res = f_open(&fout, _T("SD:/AMR/in.amr"), FA_OPEN_EXISTING | FA_WRITE);
    if (FR_OK != res) {
        log_hal_error("Failed to open the fout!\n\r");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }

    }

    /* write magic number to indicate single channel AMR file storage format */
    char *str = "#!AMR\n";
    res = f_write(&fout, str, 6, &length_written);

    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    //log_hal_info("share_buf_len=%d", share_buf_len);
    //log_hal_info("length_read from SD=%d", length_read);
    hdl->write_data_done(hdl, length_read);
    hdl->finish_write_data(hdl);

    log_hal_info("start to encode\n");
    hdl->encoder_start(hdl);

    while (1) {
        if (demo_done == 1) {
            amr_encoder_close(hdl);
            log_hal_info("do_amr_encoder_demo done\n");
            break;
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }


}

/**Notice
**  This DEMO will read pcm stream from SD card to encode it to file
**/
void amr_encoder_demo(void *pvParameters)
{

    do_amr_encoder_demo();

    while (1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

