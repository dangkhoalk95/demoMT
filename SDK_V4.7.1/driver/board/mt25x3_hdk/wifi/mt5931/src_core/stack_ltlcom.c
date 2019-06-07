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

/*********************************************************************
   (C) _____ (year of first publication) Sasken communication
   Technologies Limited, All rights reserved.
*   This file provides a template for .c files. This space
*   should be used to describe the file contents
*   Component-specific prefix : xxxx
*********************************************************************/

/*******************************************************************************
 * Inculded header files
 *******************************************************************************/
#include "stdio.h"   /* for kal_sprintf */
#include "stack_ltlcom.h"



/*******************************************************************************
 * Constant definition
 *******************************************************************************/

char *MODULE_TYPE_TO_STR[] = {
    "WNDRV",
    "SUPC",
    "HOSTAP",
    "ABM",
    "WNDRV_HISR",
    "TCPIP",
    "BWCS",
    "TIMER",
    "NIL",
    "FT",
    "MMI",
    "BT",
    "NVRAM",
};

char *MSG_TYPE_TO_STR[] = {
    "WNDRV_TEST_SET_REQ",
    "WNDRV_TEST_QUERY_REQ",
    "WNDRV_SUPC_DATA_REQ",
    "WNDRV_SUPC_INIT_REQ",
    "WNDRV_SUPC_DEINIT_REQ",
    "WNDRV_SUPC_SITE_SURVEY_REQ",
    "WNDRV_SUPC_JOIN_NETWORK_REQ",
    "WNDRV_SUPC_DISJOIN_NETWORK_REQ",
    "WNDRV_SUPC_COUNTRY_SETTING_REQ",
    "WNDRV_SUPC_REMOVE_KEY_REQ",
    "WNDRV_SUPC_ADD_KEY_REQ",
    "WNDRV_SUPC_PMKID_UPDATE_REQ",
    "WNDRV_SUPC_AUTH_STATE_UPDATE_REQ",
    "WNDRV_SUPC_BLACKLIST_UPDATE_REQ",
    "WNDRV_SUPC_CHIP_POWERON_REQ",
    "WNDRV_SUPC_CHIP_POWEROFF_REQ",
    "WNDRV_SUPC_QUERY_CAPABILITY_REQ",
    "TCPIP_WNDRV_DATA_REQ",
    "TCPIP_WNDRV_SETBEARER_REQ",

    "WNDRV_TEST_SET_CNF",
    "WNDRV_TEST_QUERY_CNF",
    "WNDRV_SUPC_STATUS_IND",
    "WNDRV_SUPC_PMKID_CANDIDATE_IND",
    "WNDRV_SUPC_INIT_CNF",
    "WNDRV_SUPC_DEINIT_CNF",
    "WNDRV_SUPC_NETWORK_CONNECT_IND",
    "WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND",
    "WNDRV_SUPC_NETWORK_DISCONNECT_IND",
    "WNDRV_SUPC_SITE_SURVEY_IND",
    "WNDRV_SUPC_NETWORK_STATISTICS_IND",
    "WNDRV_SUPC_QUERY_CAPABILITY_IND",
    "WNDRV_SUPC_DATA_IND",
    "TCPIP_WNDRV_DATA_IND",
    "FLC_WNDRV_UL_SSPDU_RESUME",
    "NVRAM_READ_REQ",

    "WNDRV_INTERRUPT_IND",
    "TIMER_EXPIRY",
    "TST_INJECT_STRING",
    "WNDRV_SCAN_INDICATION",
    "WNDRV_STATISTIC_INDICATION",
    "SOC_WNDRV_DL_SRPDU_RESUME",
    "WNDRV_MMI_POWEROFF_REQ",
    "NVRAM_READ_CNF",
    "WNDRV_MMI_POWEROFF_RSP",
    "BT_WIFI_SET_CHNL_REQ",

    "WMT_WNDRV_SET_BWCS_REQ",
    "WMT_WNDRV_QUERY_RSSI_REQ",
    "WMT_WNDRV_SET_ANT_REQ",
    "WMT_WNDRV_SET_FLOW_CTRL_REQ",
    "WMT_WNDRV_SET_FIXED_RX_GAIN_REQ",
    "WMT_WNDRV_SET_OMIT_LOW_RATE_REQ",
    "WNDRV_WMT_QUERY_RSSI_IND",
    "WNDRV_WMT_CONN_STATUS_UPDATE_IND",

    "ABM_WNDRV_IPNETWORK_STATUS_IND",
    "WNDRV_ABM_SET_PS_MODE_REQ",
    "WNDRV_ABM_SET_PS_MODE_CNF",
    "WNDRV_ABM_NET_STATUS_IND",
    "WNDRV_ABM_SET_TX_PWR_REQ",
    "WNDRV_ABM_SET_TX_PWR_CNF",
    "WNDRV_ABM_SET_SCAN_DESIRED_SSID_LIST_REQ",
    "WNDRV_ABM_SET_SCAN_DESIRED_SSID_LIST_CNF",
    "WNDRV_ABM_SET_ROAMING_PARA_REQ",
    "WNDRV_ABM_SET_ROAMING_PARA_CNF",
    "WNDRV_ABM_SET_SCAN_DWELL_TIME_REQ",
    "WNDRV_ABM_SET_SCAN_DWELL_TIME_CNF",
    "WNDRV_ABM_SET_SNIFFER_MODE_REQ",
    "WNDRV_ABM_SET_SNIFFER_MODE_CNF",
    "WNDRV_ABM_IOT_LISTENED_AP_INFO_IND",

    /****************************************************************************
     *  Request Messages: HOSTAP -> WNDRV
     ****************************************************************************/
    /* Control message */
    "HOSTAP_WNDRV_INIT_REQ",
    "HOSTAP_WNDRV_DEINIT_REQ",
    "HOSTAP_WNDRV_ADD_STA_REQ",
    "HOSTAP_WNDRV_REMOVE_STA_REQ",
    "HOSTAP_WNDRV_GET_STA_INFO_REQ",
    "HOSTAP_WNDRV_SET_STA_FLAGS_REQ",
    "HOSTAP_WNDRV_FLUSH_REQ",
    "HOSTAP_WNDRV_SET_GENERIC_ELEM_REQ",
    "HOSTAP_WNDRV_SET_ESSID_REQ",
    "HOSTAP_WNDRV_SET_8021X_REQ",
    "HOSTAP_WNDRV_SET_HOST_ENCRYPT_REQ",
    "HOSTAP_WNDRV_SET_HOST_DECRYPT_REQ",
    "HOSTAP_WNDRV_SET_ENCRYPTION_REQ",
    "HOSTAP_WNDRV_GET_ENCRYPTION_REQ",
    "HOSTAP_WNDRV_SET_PRIVACY_REQ",
    "HOSTAP_WNDRV_SET_CHANNEL_REQ",
    /* Management frame/DATA */
    "HOSTAP_WNDRV_FRAME_REQ",

    /****************************************************************************
     *  Indication Messages: WNDRV -> HOSTAP
     ****************************************************************************/
    /* Control message */
    "HOSTAP_WNDRV_INIT_CNF",
    "HOSTAP_WNDRV_DEINIT_CNF",
    "HOSTAP_WNDRV_ADD_STA_CNF",
    "HOSTAP_WNDRV_REMOVE_STA_CNF",
    "HOSTAP_WNDRV_GET_STA_INFO_CNF",
    "HOSTAP_WNDRV_SET_STA_FLAGS_CNF",
    "HOSTAP_WNDRV_FLUSH_CNF",
    "HOSTAP_WNDRV_SET_GENERIC_ELEM_CNF",
    "HOSTAP_WNDRV_SET_ESSID_CNF",
    "HOSTAP_WNDRV_SET_8021X_CNF",
    "HOSTAP_WNDRV_SET_HOST_ENCRYPT_CNF",
    "HOSTAP_WNDRV_SET_HOST_DECRYPT_CNF",
    "HOSTAP_WNDRV_SET_ENCRYPTION_CNF",
    "HOSTAP_WNDRV_GET_ENCRYPTION_CNF",
    "HOSTAP_WNDRV_SET_PRIVACY_CNF",
    "HOSTAP_WNDRV_SET_CHANNEL_CNF",
    /* Management frame */
    "HOSTAP_WNDRV_FRAME_IND",

    /****************************************************************************
    *   Indication Messages: HOSTAP -> ABM
    ****************************************************************************/
    "HOSTAP_ABM_INIT_REQ",
    "HOSTAP_ABM_DEINIT_REQ",
    "HOSTAP_ABM_STA_LIST_REQ",
    "HOSTAP_ABM_PRE_INIT_REQ",
    "HOSTAP_ABM_TIMER_CREATE_TEST",
    "HOSTAP_ABM_TIMER_CANCEL_TEST",
    "HOSTAP_ABM_GET_ADDRESS_REQ",
    "HOSTAP_ABM_INIT_CNF",
    "HOSTAP_ABM_DEINIT_CNF",            // 110
    "HOSTAP_ABM_STA_LIST_CNF",
    "HOSTAP_ABM_PREINIT_CNF",
    "HOSTAP_ABM_GETMAC_CNF",
    "HOSTAP_ABM_UPDATE_STA_LIST_IND",

    /****************************************************************************   *   Indication Messages: SUPC <-> ABM
    ****************************************************************************/
    "MSG_ID_SUPC_ABM_WIFI_INIT_REQ",
    "MSG_ID_SUPC_ABM_WIFI_DEINIT_REQ",
    "MSG_ID_SUPC_ABM_WIFI_CONNECT_REQ",
    "MSG_ID_SUPC_ABM_WIFI_DISCONNECT_REQ",
    "MSG_ID_SUPC_ABM_WIFI_AP_LIST_REQ",
    "MSG_ID_SUPC_ABM_WIFI_DISCONNECT_CNF",
    "MSG_ID_SUPC_ABM_WIFI_DISCONNECT_IND",
    "MSG_ID_SUPC_ABM_WIFI_CONNECT_IND",
    "MSG_ID_SUPC_ABM_WIFI_CONNECT_CNF",
    "MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF",
    "MSG_ID_SUPC_ABM_WIFI_INIT_CNF",
    "MSG_ID_SUPC_ABM_WIFI_DEINIT_CNF",
    "MSG_ID_SUPC_ABM_WIFI_WPS_INFO_IND",
};


/*******************************************************************************
 * External Function Declaration
 *******************************************************************************/


#define WNDRV_MSG_QUEUE_SIZE        64
#define SUPC_MSG_QUEUE_SIZE         50
#define HOSTAP_MSG_QUEUE_SIZE       20
#define ABM_MSG_QUEUE_SIZE          40

kal_msgqid mt5931_msgqid[MOD_TOTAL_NUM];

kal_msgqid create_msg_ext_q(void **start, kal_uint16 size)
{
#if 0//Need implement
    return (OSQCreate(start, size));
#endif
    os_queue_t q_id = NULL;
    q_id = kal_queue_create(size, sizeof(ilm_struct *));
    return (kal_msgqid)q_id;
}

/*************************************************************************
* FUNCTION
*  receive_msg_ext_q
*
* DESCRIPTION
*     This function reveive ilm from task's external queue
*
* PARAMETERS
*  Task Queue Id for which ILM needs to be received
*
* RETURNS
*  KAL_SUCCESS if succeed, else error
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint8
receive_msg_ext_q(kal_msgqid task_ext_qid, ilm_struct **ilm_ptr, kal_uint32 timeout)
{
#if 0//Need implement
    kal_uint8 ret;

    ASSERT(task_ext_qid);

    *ilm_ptr = (ilm_struct *)OSQPend(task_ext_qid, timeout, &ret);   //infinite wait

    if (ret != OS_NO_ERR) {
        KAL_PUBLIC_TRACE(TRACE_WARNING, "Warning:%s,ret:%d,qid:0x%X\n", __func__, ret, task_ext_qid);
    }
    return ret;
#endif
    BaseType_t ret = -1;

    ASSERT(task_ext_qid);

    //printf("[D]BQ:%d",delay);
    ret = kal_queue_receive((os_queue_t)task_ext_qid, ilm_ptr, timeout);
    //printf("[D]AQ:%d",delay);

    if (ret != 0) {
        KAL_PUBLIC_TRACE(TRACE_WARNING, "Warning:%s,ret:%d,qid:0x%X\n", __func__, ret, task_ext_qid);
        ret = KAL_ERROR;
    } else {
        ret = KAL_SUCCESS;
    }
    return ret;

}

/*************************************************************************
* FUNCTION
*  receive_msg_ext_q_main
*
* DESCRIPTION
*     This function reveive ilm from task's external queue
*   only used in task main function.
*
* PARAMETERS
*  Task Queue Id for which ILM needs to be received
*
* RETURNS
*  KAL_SUCCESS if succeed, else error
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint8
receive_msg_ext_q_for_stack(kal_msgqid task_ext_qid, ilm_struct **ilm_ptr)
{
#if 0//Need implement
    ASSERT(task_ext_qid);
    return receive_msg_ext_q(task_ext_qid, ilm_ptr, 0);
#endif

    ASSERT(task_ext_qid);
    return receive_msg_ext_q(task_ext_qid, ilm_ptr, 0);

}


/*************************************************************************
* FUNCTION
*  msg_get_ext_queue_info
*
* DESCRIPTION
*  This routine implements to get external queue's information
*
* PARAMETERS
*  task_ext_qid   -  Task's external queue id
*  messages       -  Pointer to number of queue's messages
*
* RETURNS
*  KAL_TRUE if success, else fail
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint8 msg_get_ext_queue_info(kal_msgqid task_ext_qid, kal_uint32 *messages)
{
#if 0//Need implement
    kal_uint8 retval;
    OS_Q_DATA msgq_info;

    ASSERT(task_ext_qid);

    retval = OSQQuery(task_ext_qid, &msgq_info);
    if (retval == OS_NO_ERR) {
        *messages = msgq_info.OSNMsgs;    /* Number of messages in message queue*/
    }

    return retval;
#endif
    ASSERT(task_ext_qid);
    uint16_t queue_item_num = 0;
    queue_item_num = (uint16_t)kal_queue_message_waiting((os_queue_t) task_ext_qid);
    *messages = queue_item_num;
    return KAL_TRUE;


}

/*************************************************************************
* FUNCTION
*  msg_get_ext_queue_length
*
* DESCRIPTION
*  This routine implements to get external queue's length(maximum number of messages)
*
* PARAMETERS
*  task_ext_qid   -  Task's external queue id
*  length         -  Pointer to maximum messages number in the queue
*
* RETURNS
*  KAL_TRUE if success, else fail
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint8 msg_get_ext_queue_length(kal_msgqid task_ext_qid, kal_uint32 *length)
{
#if 0//Need implement
    kal_uint8 retval;
    OS_Q_DATA msgq_info;



    retval = OSQQuery(task_ext_qid, &msgq_info);
    if (retval == OS_NO_ERR) {
        *length = msgq_info.OSQSize;
    }

    return retval;
#endif

    ASSERT(task_ext_qid);
    uint16_t queue_item_num = 0;
    uint16_t free_space_num = 0;
    queue_item_num = (uint16_t)kal_queue_message_waiting((os_queue_t)task_ext_qid);
    free_space_num = (uint16_t)kal_queue_get_space((os_queue_t)task_ext_qid);
    *length = queue_item_num + free_space_num;
    return KAL_TRUE;


}


/*************************************************************************
* FUNCTION
*  msg_send_ext_queue
*
* DESCRIPTION
*     This routine constructs the message and sends it to the task
*  identified by 'destTaskId'. Also call SmeTrace.
*
* PARAMETERS
*  The inter layer message to be sent to the external queue
*
* RETURNS
*  KAL_TRUE if successful, KAL_FALSE otherwise
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_bool
msg_send_ext_queue(ilm_struct *ilm_ptr)
{
#if 0//Need implement
    ASSERT(mt5931_msgqid[ilm_ptr->dest_mod_id]);

    if (!mt5931_msgqid[ilm_ptr->dest_mod_id]) {
        KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-D]I:0x%X,L:0x%X,P:0x%X\n", ilm_ptr, ilm_ptr->local_para_ptr, ilm_ptr->peer_buff_ptr);
        //KAL_PUBLIC_TRACE(TRACE_WARNING,"destmod:%d,srcmod:%d\n",ilm_ptr->dest_mod_id,ilm_ptr->src_mod_id);
#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceRecord(ilm_ptr);
#endif
        free_ilm(ilm_ptr);
        return KAL_FALSE;
    }

    if (ilm_ptr->src_mod_id != MOD_WNDRV_HISR) {
        KAL_PUBLIC_TRACE(TRACE_MSG, "[%d->%d]%d\n", ilm_ptr->src_mod_id, ilm_ptr->dest_mod_id, ilm_ptr->msg_id);
    }
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-S]I:0x%X,L:0x%X,P:0x%X,%d,%d\n", ilm_ptr, ilm_ptr->local_para_ptr, ilm_ptr->peer_buff_ptr, ilm_ptr->dest_mod_id, ilm_ptr->msg_id);
    ASSERT(ilm_ptr->dest_mod_id <= MOD_ABM);
    if ((OSQPost(mt5931_msgqid[ilm_ptr->dest_mod_id], ilm_ptr)) == OS_NO_ERR) {
        return KAL_TRUE;
    } else {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "MSG send fail, Maybe Queue is Full.[%d->%d]%d, 0x%X\n", ilm_ptr->src_mod_id, ilm_ptr->dest_mod_id, ilm_ptr->msg_id, mt5931_msgqid[ilm_ptr->dest_mod_id]);
        return KAL_FALSE;
    }
#endif
    ASSERT(mt5931_msgqid[ilm_ptr->dest_mod_id]);

    if (!mt5931_msgqid[ilm_ptr->dest_mod_id]) {
        KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-D]I:0x%X,L:0x%X,P:0x%X\n", ilm_ptr, ilm_ptr->local_para_ptr, ilm_ptr->peer_buff_ptr);
        //KAL_PUBLIC_TRACE(TRACE_WARNING,"destmod:%d,srcmod:%d\n",ilm_ptr->dest_mod_id,ilm_ptr->src_mod_id);
#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceRecord(ilm_ptr);
#endif
        free_ilm(ilm_ptr);
        return KAL_FALSE;
    }

    if (ilm_ptr->src_mod_id != MOD_WNDRV_HISR) {
        KAL_PUBLIC_TRACE(TRACE_MSG, "[%d->%d]%d\n", ilm_ptr->src_mod_id, ilm_ptr->dest_mod_id, ilm_ptr->msg_id);
    }
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-S]I:0x%X,L:0x%X,P:0x%X,%d,%d\n", ilm_ptr, ilm_ptr->local_para_ptr, ilm_ptr->peer_buff_ptr, ilm_ptr->dest_mod_id, ilm_ptr->msg_id);
    ASSERT(ilm_ptr->dest_mod_id <= MOD_ABM);

    BaseType_t ret = 0;
    //BaseType_t xHigherPriorityTaskWoken;

    if (0 == hal_nvic_query_exception_number()) {
        ret = kal_queue_send((os_queue_t)mt5931_msgqid[ilm_ptr->dest_mod_id], &ilm_ptr, 0) ;
    } else {
        ret = kal_queue_send_from_isr((os_queue_t)mt5931_msgqid[ilm_ptr->dest_mod_id], &ilm_ptr);
    }

    if (ret == 0) {
        return KAL_TRUE;
    } else {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "MSG send fail, Maybe Queue is Full.[%d->%d]%d, 0x%X\n", ilm_ptr->src_mod_id, ilm_ptr->dest_mod_id, ilm_ptr->msg_id, mt5931_msgqid[ilm_ptr->dest_mod_id]);
        return KAL_FALSE;
    }

}


/*************************************************************************
* FUNCTION
*  msg_send_ext_queue_to_head
*
* DESCRIPTION
*     This routine constructs the message and sends it to the head of
*  queue identified by 'destTaskId'. Also call SmeTrace.
*
* PARAMETERS
*  The inter layer message to be sent to the external queue
*
* RETURNS
*  KAL_TRUE if successful, KAL_FALSE otherwise
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_bool
msg_send_ext_queue_to_head(ilm_struct *ilm_ptr)
{

#if 0//Need implement

    ASSERT(mt5931_msgqid[ilm_ptr->dest_mod_id]);

    if (!mt5931_msgqid[ilm_ptr->dest_mod_id]) {
        return KAL_FALSE;
    }

    if ((OSQPostFront(mt5931_msgqid[ilm_ptr->dest_mod_id], ilm_ptr)) == OS_NO_ERR) {
        return KAL_TRUE;
    } else {
        return KAL_FALSE;
    }
#endif
    ASSERT(mt5931_msgqid[ilm_ptr->dest_mod_id]);

    if (!mt5931_msgqid[ilm_ptr->dest_mod_id]) {
        return KAL_FALSE;
    }

    BaseType_t ret = 0;
    BaseType_t xHigherPriorityTaskWoken;

    if (0 == hal_nvic_query_exception_number()) {
        ret = kal_queue_send_front((os_queue_t)mt5931_msgqid[ilm_ptr->dest_mod_id], &ilm_ptr, 0);
    } else {
        ret = kal_queue_send_front_from_isr((os_queue_t)mt5931_msgqid[ilm_ptr->dest_mod_id], &ilm_ptr);
    }

    if (ret == 0) {
        return KAL_TRUE;
    } else {
        return KAL_FALSE;
    }

}



kal_status mt5931_create_msg_ext_q(void)
{

    kal_uint8 i = 0;

    for (i = 0; i < MOD_TOTAL_NUM; i++) {
        mt5931_msgqid[i] = NULL;
    }

    mt5931_msgqid[MOD_WNDRV] = create_msg_ext_q(NULL, WNDRV_MSG_QUEUE_SIZE);
    if (!mt5931_msgqid[MOD_WNDRV]) {
        return KAL_ERROR;
    }

    mt5931_msgqid[MOD_SUPC] = create_msg_ext_q(NULL, SUPC_MSG_QUEUE_SIZE);
    if (!mt5931_msgqid[MOD_SUPC]) {
        return KAL_ERROR;
    }

    mt5931_msgqid[MOD_HOSTAP] = create_msg_ext_q(NULL, HOSTAP_MSG_QUEUE_SIZE);
    if (!mt5931_msgqid[MOD_HOSTAP]) {
        return KAL_ERROR;
    }

    mt5931_msgqid[MOD_ABM] = create_msg_ext_q(NULL, ABM_MSG_QUEUE_SIZE);
    if (!mt5931_msgqid[MOD_ABM]) {
        return KAL_ERROR;
    }

    KAL_PUBLIC_TRACE(TRACE_BUFF, "[MSG_Q_ID]0x%X,0x%X,0x%X,0x%X\n", mt5931_msgqid[MOD_WNDRV],
                     mt5931_msgqid[MOD_SUPC], mt5931_msgqid[MOD_HOSTAP], mt5931_msgqid[MOD_ABM]);
    return KAL_SUCCESS;
}




