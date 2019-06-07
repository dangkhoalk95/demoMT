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

#ifndef _WNDRV_CONTEXT_H
#define _WNDRV_CONTEXT_H

//todo
/*
#if !defined(_STACKLIB_H)
#include "stacklib.h"
#endif
#if !defined(_EVENT_SHED_H)
#include "event_shed.h"
#endif
#if !defined(_EVENT_SHED_DEFS_H)
#include "event_shed_defs.h"
#endif
*/
typedef struct {
    ADAPTER_T             *adpt_p;
    GLUE_INFO_T           *glue_p;

#if 0 /*All move to GLUE_INFO_T */
    stack_timer_struct      timer;
    stack_timer_struct      evsched_timer;
    eventid                 event_id;
    event_scheduler         *event_scheduler_ptr;
    kal_int32               tx_sys_peer_num;
    kal_int32               tx_flc_peer_num;
    kal_int32               rx_sys_peer_num;
    kal_int32               rx_flc_peer_num;
    kal_int32               mem_allc_size;
    kal_bool                meta_mode;
#endif
} wndrv_context_struct;

#if 0
typedef struct {
    P_QUE_ENTRY_T prQueueEntry;       /* Be careful: q_entry should be put in first order. */
    peer_buff_struct  *tx_peer_p;
    module_type       src_mod_id;
    kal_uint8         flc_pool_type;
    kal_uint8         pool_id;
    kal_bool          flc_mode;      /* 0: don't use flc. 1: use flc */
    kal_uint8         tid;
    kal_uint8         queue_num;
    kal_uint8         check_mark;    /* For debug purpose */
    kal_uint8         flags;         /* For indicate info from TCPIP */
    kal_uint8         reserved0;
    kal_uint8         reserved1;
    kal_uint8         reserved2;
    kal_uint32       sys_time;
    P_PACKET_INFO_T prPacketInfo;
} wndrv_pkt_descriptor_struct;
#else
struct wndrv_pkt_descriptor_struct {
    P_QUE_ENTRY_T prQueueEntry;       /* Be careful: q_entry should be put in first order. */
    peer_buff_struct  *tx_peer_p;
    module_type       src_mod_id;
    kal_uint8         flc_pool_type;
    flc2_pool_id_enum         pool_id;
    kal_bool          flc_mode;      /* 0: don't use flc. 1: use flc */
    kal_uint8         tid;
    kal_uint8         queue_num;
    kal_uint8         check_mark;    /* For debug purpose */
    kal_uint8         flags;         /* For indicate info from TCPIP */
    kal_uint8         reserved0;
    kal_uint8         reserved1;
    kal_uint8         reserved2;
    kal_uint32       sys_time;
    P_PACKET_INFO_T prPacketInfo;
    PACKET_INFO_T pkt_info;
};
#endif

typedef struct wndrv_pkt_descriptor_struct wndrv_pkt_descriptor_struct;


typedef struct {
    QUE_ENTRY    q_entry;
    ilm_struct   ilm;
    ilm_struct   *src_ilm_ptr;
} wndrv_int_queue_info_struct;

wndrv_pkt_descriptor_struct *wndrv_get_pkt_descriptor(peer_buff_struct *peer_p);

#endif /* _WNDRV_CONTEXT_H */

