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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *  gl_hpi.c
 *
 * Project:
 * --------
 *  Maui_Software
 *
 * Description:
 * ------------
 *  WiFi glue layer function for HPI interface
 */
#define __WNDRV_OS_FILES_C__

//#include "os_wrap.h"
//#include "AHC_utility.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "gl_typedef.h"
#include "wndrv_kal.h"
#include "kal_trace.h"
#include "wndrv_trc.h"





/*-------------------------------*/

void wndrv_bus_hpi_module_control(kal_uint8 sel) {}
void wndrv_bus_power_control(kal_uint8 sel) {}

/*-------------------------------*/

//need to consider
kal_bool WiFi_Query_Bluetooth_Capability(void) //check channel info if need HB value
{
    return KAL_TRUE;
}

kal_bool bwcs_query_single_antenna_capability(void)
{
#ifdef __WIFI_BT_SINGLE_ANTENNA_SUPPORT__
    return KAL_TRUE;
#else
    return KAL_FALSE;
#endif /* __WIFI_BT_SINGLE_ANTENNA_SUPPORT__ */
}

kal_uint8 BT_exist(void)
{
#if defined(__BTMTK__)
    return 1;
#else
    return 0;
#endif
}

kal_bool Wifi_Query_Dynamic_Switch_Cachability(void)
{
    return KAL_TRUE;
}

/*-------------------------------*/

kal_uint32 L1I_GetTimeStamp(void)
{
    return (KAL_OS_TIME_GET());
}



char INFORM_WIFI_DEGRADE_POWER(void)
{
    return 0;
}

//kal_uint32 wndrv_dbglevel = ((1<<TRACE_ERROR)|(1<<TRACE_WARNING));
kal_uint32 wndrv_dbglevel = ((1 << TRACE_ERROR) | (1 << TRACE_WARNING) | (1 << TRACE_STATE) | (1 << TRACE_INFO) | (1 << TRACE_BUFF));
//kal_uint32 wndrv_dbglevel = 0xFFFF;
kal_uint8  wndrv_in_ISR = FALSE;

//kal_mutexid wifi_mutex;
BOOL wndrv_task_init()
{
    //wifi_mutex = kal_create_mutex();
    return KAL_TRUE;
}

/*-------------------------------*/
void wndrv_one(void) {}
void wndrv_two(void) {}
void wndrv_three(kal_bool in_progress) {}
void wndrv_four(void) {}
void L1SM_EnterFPM(kal_bool isFPM, kal_uint8 module) {}
void wndrv_supc_query_capability_req_handler(ilm_struct *ilm_ptr) {}

