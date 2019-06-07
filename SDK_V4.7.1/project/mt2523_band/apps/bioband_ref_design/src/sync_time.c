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

#include <string.h>
#include "hal_rtc.h"
#include "syslog.h"
#include "sync_time.h"

uint8_t sync_rsp[SYNT_TIME_RESPONSE_LEN];

log_create_module(SYNC_TIME, PRINT_LEVEL_INFO);

static sync_time_status sync_time_parse_time(uint8_t *data_buf, uint32_t data_len, hal_rtc_time_t *parsed_time)
{
    uint8_t parsed_len = 0;
    //uint16_t tlv_size = 0;

    if (!strncmp((const char*)data_buf, SYNC_TIME_CMD_HEADER, strlen(SYNC_TIME_CMD_HEADER))) {
        parsed_len = strlen(SYNC_TIME_CMD_HEADER);
        //tlv_size = (*(data_buf + parsed_len + 1) << 8) | *(data_buf + parsed_len); // 2 bytes for tlv size

        parsed_len += 2;

        LOG_I(SYNC_TIME, "parsed_len = %d, tag_id = %d, data_len = %d\r\n", parsed_len, *(data_buf + parsed_len), data_len);
        while (parsed_len < data_len) {
            switch (*(data_buf + parsed_len)) {
                case SYNC_TIME_TAG_ID_TIMESTAMP: 
                {
                    //parse timestamp
                    uint8_t tag_len;
                    uint16_t year;
                    
                    LOG_I(SYNC_TIME, "Parse timestamp\r\n");
                    parsed_len++;
                    tag_len = *(data_buf + parsed_len);
                    parsed_len++;
                    LOG_I(SYNC_TIME, "tag_len = %d, parsed_len = %d\r\n", tag_len, parsed_len);
                    year = *(data_buf + parsed_len + 1) << 8 | *(data_buf + parsed_len);
                    if (year > 1999) {
                        parsed_time->rtc_year = year - 2000;
                        parsed_len += 2;
                        LOG_I(SYNC_TIME, "year = %d, parsed_len = %d\r\n", parsed_time->rtc_year, parsed_len);
                        parsed_time->rtc_week = *(data_buf + parsed_len);
                        parsed_len++;
                        
                        parsed_time->rtc_mon = *(data_buf + parsed_len);
                        parsed_len++;
                        
                        parsed_time->rtc_day = *(data_buf + parsed_len);
                        parsed_len++;
                        
                        parsed_time->rtc_hour = *(data_buf + parsed_len);
                        parsed_len++;
                        
                        parsed_time->rtc_min = *(data_buf + parsed_len);
                        parsed_len++;
                        
                        parsed_time->rtc_sec = *(data_buf + parsed_len);
                        parsed_len++;
                    
                    } else {
                        //do not set this time to watch
                        LOG_I(SYNC_TIME, "The year is before 2000, time will not sync to watch\r\n");
                        parsed_len += tag_len;
                    }
                    break;
                }

                case SYNC_TIME_TAG_ID_TIMEZONE: 
                {
                    uint8_t tag_len;
                    
                    LOG_I(SYNC_TIME, "Parse time zone\r\n");
                    parsed_len++;
                    tag_len = *(data_buf + parsed_len);
                    parsed_len++;

                    //time zone is not used now, get time zone if you need
                    parsed_len += tag_len;
                    break;
                }

                default:
                    break;
            }
        }

        if (parsed_len == data_len)
            return SYNC_TIME_STATUS_SUCCESS;
    }

    return SYNC_TIME_STATUS_FAIL;
}

sync_time_status sync_time_set_time(uint8_t *data_buf, uint32_t data_len)
{
    hal_rtc_time_t sp_time = {0};
    sync_time_status status;
    
    status = sync_time_parse_time(data_buf, data_len, &sp_time);
    LOG_I(SYNC_TIME, "sp time: Y%d, M%d, D%d, H%d, M%d, S%d, W%d\r\n", sp_time.rtc_year, sp_time.rtc_mon, sp_time.rtc_day,
        sp_time.rtc_hour, sp_time.rtc_min, sp_time.rtc_sec, sp_time.rtc_week);

    if (status == SYNC_TIME_STATUS_SUCCESS) {
        hal_rtc_status_t ret;
        
        //set sp time to rtc
        ret = hal_rtc_init();
        if (HAL_RTC_STATUS_OK != ret) {
            LOG_I(SYNC_TIME, "Init rtc fail\r\n");
            status = SYNC_TIME_STATUS_FAIL;
        } else {

            //Set the RTC current time
            ret = hal_rtc_set_time(&sp_time);
            if (HAL_RTC_STATUS_OK != ret) {
                LOG_I(SYNC_TIME, "Set rtc time fail\r\n");
                status = SYNC_TIME_STATUS_FAIL;
            }
        }
    }

    return status;
}

uint8_t* sync_time_create_response(sync_time_status rsp, uint16_t *rsp_len)
{

    //the format of sync time response is: [mtkcare][size(2 byte)][rsp (e.g ok)]
    memset(sync_rsp, 0, SYNT_TIME_RESPONSE_LEN);
    memcpy(sync_rsp, SYNC_TIME_CMD_HEADER, strlen(SYNC_TIME_CMD_HEADER));
    *rsp_len = strlen(SYNC_TIME_CMD_HEADER);
    *rsp_len += 2; // 2 BYTE for size
  
    if (rsp == SYNC_TIME_STATUS_SUCCESS) {
        //response ok
        memcpy(sync_rsp + *rsp_len, "ok", strlen("ok"));
        *rsp_len += strlen("ok");
    } else {
        //response fail, define the fail information as you need. 
        //Be careful: modify SYNT_TIME_RESPONSE_LEN if response length is more than 30!!!
        sync_rsp[*rsp_len] = 0xFF;
        (*rsp_len)++;
    }

    sync_rsp[strlen(SYNC_TIME_CMD_HEADER)] = (uint8_t)((*rsp_len - strlen(SYNC_TIME_CMD_HEADER) - 2) & 0x00FF);
    sync_rsp[strlen(SYNC_TIME_CMD_HEADER) + 1] = (uint8_t)((*rsp_len - strlen(SYNC_TIME_CMD_HEADER) - 2) & 0xFF00) >> 8;
    if (*rsp_len > SYNT_TIME_RESPONSE_LEN) {

        LOG_I(SYNC_TIME, "response buffer is not enough!!!\r\n");
        *rsp_len = 0;
        memset(sync_rsp, 0, SYNT_TIME_RESPONSE_LEN);
        return NULL;
    }

    LOG_I(SYNC_TIME, "rsp_len = %d, sync_rsp: %x %x %x %x %x %x %x %x %x %x %x\r\n", *rsp_len, sync_rsp[0], sync_rsp[1],sync_rsp[2],
        sync_rsp[3], sync_rsp[4], sync_rsp[5], sync_rsp[6], sync_rsp[7], sync_rsp[8], sync_rsp[9], sync_rsp[10]);
    return sync_rsp;
}
