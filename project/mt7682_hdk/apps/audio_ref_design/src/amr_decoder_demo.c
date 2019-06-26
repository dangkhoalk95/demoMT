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
#include "amr_decoder.h"
#include "amr_decoder_demo.h"
#include "hal_log.h"
#include "ff.h"
#include "hal_gpio.h"
#include "hal_gpt.h"

#define AMR_MAGIC_NUMBER "#!AMR\n"

static FATFS fatfs;
static FIL fdst;
static FRESULT res;
static UINT length_read;
static uint8_t first_time_f_mount = 1;
static uint8_t share_buffer[1024];  //share buffer

void do_amr_demo(void);

static int16_t test_open_file_from_sd(FIL *fp, const TCHAR *path)
{
    FRESULT res;
    char magic[8];

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
            log_hal_error("fopen error , res=%d\n", res);
            return -1;
        }
    } else {
        log_hal_info("fmount error \n");
    }

    memset(magic, 0, 8);
    res = f_read(fp, magic, 6 , &length_read);
    if (FR_OK != res) {
        log_hal_error("Failed to read the file!\n\r");
        f_close(fp);
        return -1;
    } else {
        log_hal_info("length_read=%d\n", length_read);
    }

    /*Find AMR file format*/
    if (memcmp((const char *)magic, AMR_MAGIC_NUMBER, 6)) {
        f_close(fp);
        log_hal_error("Invalid magic number: magic=%s!\n\r", magic);
        return -1;
    } else {
        log_hal_info("Find magic number: magic=%s!\n\r", magic);
    }

    if (res != FR_OK) {
        return -1;
    } else {
        return 0;
    }
}

static void test_stop_read_from_sd(amr_decoder_handle_t *hdl)
{
    res = f_close(&fdst);
    if (!res) {
        log_hal_info("fclose success \n");
    } else {
        log_hal_error("fclose error \n");
    }
}


static void test_sd_event_callback(amr_decoder_handle_t *hdl, amr_decoder_media_event_t event)
{
    uint8_t *share_buf;
    uint32_t share_buf_len;

    switch (event) {
        case AMR_DECODER_MEDIA_EVENT_END:
            log_hal_info("Complete data flush\r\n");
            break;
        case AMR_DECODER_MEDIA_EVENT_ERROR:
            log_hal_info("Error\n");
            test_stop_read_from_sd(hdl);
            hdl->decoder_stop(hdl);
            break;
        case AMR_DECODER_MEDIA_EVENT_REQUEST:
            if (f_eof(&fdst)) {
                log_hal_info("[EOF]End of this file.\r\n");
                test_stop_read_from_sd(hdl);
                hdl->decoder_flush(hdl, 1);
                break;
            }

            uint32_t loop_idx;
            uint32_t loop_cnt = 2;
            for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
                hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
                f_read(&fdst, share_buf, share_buf_len, &length_read);
                //log_hal_info("SD_EVENT:share_buf_len=%d", share_buf_len);
                //log_hal_info("SD=%d", length_read);
                hdl->write_data_done(hdl, length_read);
            }
            hdl->finish_write_data(hdl);
            break;
    }
}

void do_amr_demo(void)
{

    amr_decoder_handle_t *hdl = NULL;
    uint8_t *share_buf;
    uint32_t memory_size;
    uint32_t share_buf_len;

    log_hal_info("amr_decoder_demo start\n");
    hdl = amr_decoder_open(test_sd_event_callback);
    if (hdl == NULL) {
        log_hal_error("amr_decoder_open fail.\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    if (test_open_file_from_sd(&fdst, _T("SD:/AMR/in.amr")) < 0) {
        log_hal_error("Fail to SD:/AMR/in.amr\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    amr_decoder_get_memory_size(&memory_size);
    log_hal_info("memory_size needed by decoder=%d bytes", memory_size);

    hdl->set_share_buffer(hdl, share_buffer, 1024);

    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);
    hdl->write_data_done(hdl, length_read);
    hdl->finish_write_data(hdl);

    log_hal_info("start to decode\n");
    hdl->decoder_start(hdl);
    //vTaskDelay(3000 / portTICK_RATE_MS);
    //hdl->decoder_pause(hdl);
    //vTaskDelay(3000 / portTICK_RATE_MS);
    //hdl->decoder_resume(hdl);

    //log_hal_info("hal_gpt_delay_ms(24000); +\n");
    hal_gpt_delay_ms(24000);
    //log_hal_info("hal_gpt_delay_ms(24000); -\n");

    //log_hal_info("vTaskDelay(20000  / portTICK_RATE_MS); +\n");
    //vTaskDelay(60000 / portTICK_RATE_MS);
    //log_hal_info("vTaskDelay(20000  / portTICK_RATE_MS); -\n");

    hdl->decoder_stop(hdl);
    amr_decoder_close(hdl);
    log_hal_info("amr_decoder_demo done\n");

}

/**Notice
**  (1)in.amr file on SD card is encoded by amr_encoder_dem (amr_encoder_demo.c)
**  (2)This DEMO will read amr stream from SD card and output it to NAU8810
**/
void amr_decoder_demo(void *pvParameters)
{

    do_amr_demo();

    while (1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}



