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


/* Common files Inclusion */
//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"

//#include "stack_common.h"
//#include "stack_msgs.h"
//#include "app_ltlcom.h"           /* Task message communiction */
//#include "syscomp_config.h"
//#include "task_config.h"          /* Task creation */
//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */

#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "supc_abm_msgs.h"

#include "supc_common.h"

#include "kal_trace.h"
#include "stack_ltlcom.h"
void supc_task_main(void *idx);
extern kal_bool supc_init(task_indx_type task_indx);
extern void supc_main(ilm_struct *ilm_ptr);
extern kal_bool supc_reset(task_indx_type task_index);
extern kal_bool supc_deinit(task_indx_type task_index);


#if 0
/*********************************************************************
   Function Name    : supc_create
   Return Value     : kal_bool
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : comptask_handler_struct **handle
   Description      :
*********************************************************************/
kal_bool supc_create(comptask_handler_struct **handle)
{
    static comptask_handler_struct wpa_handler_info = {
        supc_task_main,       /* task entry function */
        supc_init,         /* task initialization function */
        NULL,     /* task configuration function */
        supc_reset,           /* task reset handler */
        supc_deinit          /* task termination handler */
    };

    *handle = &wpa_handler_info;
    return KAL_TRUE;
}
#endif
/*********************************************************************
   Function Name    : supc_task_main
   Return Value     : void
   Usage Notes      :
   Limitations      :
   Change history   :
   Parameters       : task_entry_struct *task_entry_ptr
   Description      :
*********************************************************************/
void supc_task_main(void *idx)
{
    ilm_struct *pcurrent_ilm;
//   kal_uint32 my_index = 0;

//   kal_get_my_task_index(&my_index);
    /*   no use now
    #ifdef __WAPI_SUPPORT__
    wpa_printf(MSG_INFO, "SUPC LIB : supc_11BW1136MP.P17 WAPI %s %s", __DATE__, __TIME__);
    #else
    wpa_printf(MSG_INFO, "SUPC LIB : supc_11BW1136MP.P17  %s %s", __DATE__, __TIME__);
    #endif
    */
    while (1) {
#if 0
        receive_msg_ext_q_for_stack(task_info_g[task_entry_ptr->task_indx].
                                    task_ext_qid, &current_ilm);
        stack_set_active_module_id(my_index, current_ilm.dest_mod_id);
        //wpa_printf(MSG_FUNC, "[----------------SUPC msg handler-----------]");
        kal_trace(TRACE_FUNC, FUNC_SUPC_MSG_HANDLER);
#endif
        receive_msg_ext_q_for_stack(mt5931_msgqid[MOD_SUPC], &pcurrent_ilm);
//       stack_set_active_module_id(my_index, pcurrent_ilm.dest_mod_id);
#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceRecord(pcurrent_ilm);
#endif
        //wpa_printf(MSG_INFO, "recv msg(%d) from module(%d)\n", pcurrent_ilm->msg_id, pcurrent_ilm->src_mod_id);
        supc_main(pcurrent_ilm);

        //pcurrent_ilm->peer_buff_ptr = NULL;
        //free_int_ilm(pcurrent_ilm, NULL, 0);

#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceCheck(3);
#endif
        free_ilm(pcurrent_ilm);
#if 0
        if (pcurrent_ilm.src_mod_id != MAP_MOD_TIMER) {
            //free_ilm( &current_ilm);
            free_ilm(&current_ilm);
        }
#endif
    }
}

void supc_create(void *argv)
{
    supc_init(0);
    supc_task_main(NULL);
}

