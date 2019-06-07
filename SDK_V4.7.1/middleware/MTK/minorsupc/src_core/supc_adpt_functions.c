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

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif



//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"
#endif
#include "supc_common.h"



void *supc_int_get_buffer(kal_uint16 buff_size)
{
#ifdef _SUPC_MEM_ADM
    return kal_adm_alloc(supc_mem_adm_id, buff_size);
#else
    return wifi_get_ctrl_buffer(buff_size, (void *)__func__);
#endif
}

void supc_int_free_buffer(void *buff_ptr)
{
#ifdef _SUPC_MEM_ADM
    kal_adm_free(supc_mem_adm_id, buff_ptr);
    return;
#else
    wifi_free_ctrl_buffer(buff_ptr, (void *)__func__);
    return;
#endif
}


#ifdef _SUPC_CHECK_MEM_LEAK

typedef struct {
    kal_uint16 size;
    kal_uint16 curr_num;
    kal_uint16 watermark_num;
    kal_uint32 curr_size;
    kal_uint32 watermark_size;
} supc_mem_pool_rec_struct;

supc_mem_check_strcut supc_mem_info[SUPC_MEM_INFO_MAX_REC];
kal_uint16            supc_mem_info_index;
kal_uint16            supc_mem_info_seq_no = 0;

kal_uint32            supc_mem_curr_size = 0;
kal_uint32            supc_mem_high_watermark = 0;
kal_uint32            supc_mem_index_curr_usage = 0; // Kinki add for assert fail
kal_uint32            supc_mem_index_high_watermark = 0; // Kinki add for assert fail

supc_mem_pool_rec_struct    supc_mem_pool_usage[] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0},
    {16, 0, 0, 0,},
    {32, 0, 0, 0,},
    {64, 0, 0, 0,},
    {128, 0, 0, 0,},
    {256, 0, 0, 0,},
    {512, 0, 0, 0,},
    {1024, 0, 0, 0,},
    {2048, 0, 0, 0,},
    {4096, 0, 0, 0,},
    {8192, 0, 0, 0,}
};

kal_uint32                  ssl_mem_curr_size = 0;
kal_uint32                  ssl_mem_high_watermark = 0;
supc_mem_pool_rec_struct    ssl_mem_pool_usage[] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {8, 0, 0, 0, 0},
    {16, 0, 0, 0,},
    {32, 0, 0, 0,},
    {64, 0, 0, 0,},
    {128, 0, 0, 0,},
    {256, 0, 0, 0,},
    {512, 0, 0, 0,},
    {1024, 0, 0, 0,},
    {2048, 0, 0, 0,},
    {4096, 0, 0, 0,},
    {8192, 0, 0, 0,}
};

kal_uint32                  supc_total_curr_size = 0;
kal_uint32                  supc_total_high_watermark = 0;


void *supc_get_ctrl_buffer(kal_uint16 buff_size, kal_char *file_name, kal_uint32 line, kal_bool is_supc)
{
    void *buff_ptr = NULL;
    kal_uint8 i = 0;
    kal_uint16 local_mem_info_index = 0;

    buff_ptr = supc_int_get_buffer(buff_size);

    SUPC_ASSERT(buff_ptr != NULL);


    if (supc_mem_info_index == SUPC_MEM_INFO_MAX_REC) {
        supc_mem_info_index = 0;
    }

    if (supc_mem_info_index == (SUPC_MEM_INFO_MAX_REC - 1)) {
        local_mem_info_index = 0;
    } else {
        local_mem_info_index = supc_mem_info_index;
    }

    while (supc_mem_info[local_mem_info_index].mem_addr != 0) {
        local_mem_info_index++;
        if (local_mem_info_index >= SUPC_MEM_INFO_MAX_REC) { // Karen
            local_mem_info_index = 0;
        }
        //J, local_mem_info_index == supc_mem_info_index implies all the
        // buffer is used.
        SUPC_ASSERT(local_mem_info_index != supc_mem_info_index);
    }

    supc_mem_info_index = local_mem_info_index;

    supc_mem_info[supc_mem_info_index].alloc_seqno = supc_mem_info_seq_no;
    supc_mem_info[supc_mem_info_index].line = line;
    supc_mem_info[supc_mem_info_index].file_name = file_name;
    supc_mem_info[supc_mem_info_index].mem_addr = (kal_uint32)buff_ptr;
    supc_mem_info[supc_mem_info_index].buff_size = buff_size;

    supc_mem_index_curr_usage++;
    if (supc_mem_index_curr_usage > supc_mem_index_high_watermark) {
        supc_mem_index_high_watermark = supc_mem_index_curr_usage;
    }

    if (KAL_TRUE == is_supc) {
        for (i = 3; i < 14; i++) {
            if ((buff_size >> (i + 1)) > 0) {
                continue;
            }
            supc_mem_pool_usage[i].curr_num ++;
            supc_mem_pool_usage[i].curr_size += buff_size;
            if (supc_mem_pool_usage[i].curr_num > supc_mem_pool_usage[i].watermark_num) {
                supc_mem_pool_usage[i].watermark_num = supc_mem_pool_usage[i].curr_num;
                supc_mem_pool_usage[i].watermark_size = supc_mem_pool_usage[i].curr_size;
            }
            supc_mem_curr_size += buff_size;
            if (supc_mem_curr_size > supc_mem_high_watermark) {
                supc_mem_high_watermark = supc_mem_curr_size;
            }

            break;

        }
    } else {
        for (i = 3; i < 14; i++) {           //for fix klocwork issue, arrary possiable overflow
            if ((buff_size >> (i + 1)) > 0) {
                continue;
            }

            ssl_mem_pool_usage[i].curr_num ++;
            ssl_mem_pool_usage[i].curr_size += buff_size;
            if (ssl_mem_pool_usage[i].curr_num > ssl_mem_pool_usage[i].watermark_num) {
                ssl_mem_pool_usage[i].watermark_num = ssl_mem_pool_usage[i].curr_num;
                ssl_mem_pool_usage[i].watermark_size = ssl_mem_pool_usage[i].curr_size;
            }

            ssl_mem_curr_size += buff_size;
            if (ssl_mem_curr_size > ssl_mem_high_watermark) {
                ssl_mem_high_watermark = ssl_mem_curr_size;
            }

            break;

        }
    }

    supc_total_curr_size += buff_size;
    if (supc_total_curr_size > supc_total_high_watermark) {
        supc_total_high_watermark = supc_total_curr_size;
    }

    supc_mem_info_index ++;
    supc_mem_info_seq_no ++;


    //kal_prompt_trace(MOD_SUPC, file_name);
    //kal_prompt_trace(MOD_SUPC, "[SUPC] Alloc: %x, Line: %d, SeqNo: %d", (kal_uint32)buff_ptr, line, supc_mem_info_seq_no);

    return buff_ptr;
}

void supc_free_ctrl_buffer(void *buff_ptr, kal_char *file_name, kal_uint32 line, kal_bool is_supc)
{
    kal_uint16 i = 0, j = 0;

    for (i = 0; i < SUPC_MEM_INFO_MAX_REC; i ++) {
        if ((kal_uint32)(supc_mem_info[i].mem_addr) == (kal_uint32)buff_ptr) {
            wpa_printf(TRACE_WARNING, "[SUPC] last allocater %s, %d\n",
                       supc_mem_info[i].file_name, supc_mem_info[i].line);
//          kal_trace(DBG_MEM_MGMT,INFO_SUPC_ALLOC_MEM_LOCATION,supc_mem_info[i].line);
//          kal_wap_trace(MAP_MOD_SUPC, DBG_MEM_MGMT,"%s", supc_mem_info[i].file_name);
            wpa_printf(TRACE_WARNING, "[SUPC] %s to Free: %x, Line: %d\n",
                       file_name, (kal_uint32)buff_ptr, line);
//          kal_trace(DBG_MEM_MGMT,INFO_SUPC_FREE_MEM_LOCATION,(kal_uint32)buff_ptr, line);
//          kal_wap_trace(MAP_MOD_SUPC, DBG_MEM_MGMT,"%s", file_name);

            supc_int_free_buffer(buff_ptr);
            supc_mem_info[i].free_seqno = supc_mem_info_seq_no;
            supc_mem_info[i].mem_addr = 0;
            supc_mem_info[i].line = line;
            supc_mem_info[i].file_name = file_name;

            supc_total_curr_size -= supc_mem_info[i].buff_size;
            supc_mem_index_curr_usage--;

            if (KAL_TRUE == is_supc) {
                for (j = 3; j < 14; j++) {
                    if (((supc_mem_info[i].buff_size) >> (j + 1)) > 0) {
                        continue;
                    }
                    supc_mem_pool_usage[j].curr_num --;
                    supc_mem_pool_usage[j].curr_size -= supc_mem_info[i].buff_size;
                    supc_mem_curr_size -= supc_mem_info[i].buff_size;
                    break;
                }
            } else {
                for (j = 3; j < 14; j++) {
                    if (((supc_mem_info[i].buff_size) >> (j + 1)) > 0) {
                        continue;
                    }
                    ssl_mem_pool_usage[j].curr_num --;
                    ssl_mem_pool_usage[j].curr_size -= supc_mem_info[i].buff_size;
                    ssl_mem_curr_size -= supc_mem_info[i].buff_size;
                    break;
                }
            }

            supc_mem_info_seq_no ++;

            return;
        }
    }
    //wpa_printf(MSG_WARNING, "in %s %d to free %p", file_name, line, buff_ptr);
    SUPC_ASSERT(0);

}

void supc_check_mem_leak()
{

    kal_uint16 i = 0;

    for (i = 0; i < SUPC_MEM_INFO_MAX_REC; i ++) {
        if ((kal_uint32)(supc_mem_info[i].mem_addr) != 0) {
            //wpa_printf(MSG_WARNING, supc_mem_info[i].file_name);
            kal_wap_trace(MAP_MOD_SUPC, DBG_MEM_MGMT, "%s", supc_mem_info[i].file_name);
            //wpa_printf(MSG_WARNING, "Mem_Addr: %x, Line: %d, size: %d", supc_mem_info[i].mem_addr,
            //supc_mem_info[i].line, supc_mem_info[i].buff_size);
            kal_trace(DBG_MEM_MGMT, INFO_SUPC_CHK_MEM_LEAK_ADDR_INFO, supc_mem_info[i].mem_addr, supc_mem_info[i].line, supc_mem_info[i].buff_size);
        }
    }
    //wpa_printf(MSG_WARNING, "supc_mem_high_watermark = %d", supc_mem_high_watermark);
    kal_trace(DBG_MEM_MGMT, INFO_SUPC_CHK_MEM_LEAK_SUPC_HIGH_WATERMARK, supc_mem_high_watermark);
    //wpa_printf(MSG_WARNING, "ssl_mem_high_watermark = %d", ssl_mem_high_watermark);
    kal_trace(DBG_MEM_MGMT, INFO_SUPC_CHK_MEM_LEAK_SSL_HIGH_WATERMARK, ssl_mem_high_watermark);
    //wpa_printf(MSG_WARNING, "supc_total_high_watermark = %d", supc_total_high_watermark);
    kal_trace(DBG_MEM_MGMT, INFO_SUPC_CHK_MEM_LEAK_TOTAL_HIGH_WATERMARK, supc_total_high_watermark);
    //wpa_printf(MSG_WARNING, "supc_mem_index_high_watermark = %d", supc_mem_index_high_watermark);
    kal_trace(DBG_MEM_MGMT, INFO_SUPC_CHK_MEM_LEAK_TOTAL_INDEX_WATERMARK, supc_mem_index_high_watermark);

    return;
}
#else
void *supc_get_ctrl_buffer(kal_uint16 buff_size, kal_char *file_name, kal_uint32 line, kal_bool is_supc)
{
    void *buff_ptr = NULL;

    buff_ptr = supc_int_get_buffer(buff_size);
    SUPC_ASSERT(buff_ptr != NULL);
    return buff_ptr;
}
void supc_free_ctrl_buffer(void *buff_ptr, kal_char *file_name, kal_uint32 line, kal_bool is_supc)
{
    supc_int_free_buffer(buff_ptr);
}
void supc_check_mem_leak()
{
}

#endif /* _SUPC_CHECK_MEM_LEAK */
