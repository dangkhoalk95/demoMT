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

#ifndef _KAL_PUBLIC_DEFS_H
#define _KAL_PUBLIC_DEFS_H

#include "kal_general_types.h"
//#include "stack_config.h"               /* module_type & task_indx_type */
#include "stack_msgs.h"                 /* sap_type & msg_type */
//#include "ucos_ii.h"
#include "freertos_ii.h"

/*******************************************************************************
 * Type Definitions
 *******************************************************************************/

/* opaque type for task identity */
typedef struct {
    kal_uint8 unused;
} *kal_taskid;

/* opaque type for hisr identity */
typedef struct {
    kal_uint8 unused;
} *kal_hisrid;

/* opaque type for semaphore identity */
typedef struct {
    kal_uint8 unused;
} *kal_semid;

/* opaque type for message queue identity */
typedef void *kal_msgqid;

/* contain message queue information */
typedef struct {
    /* current number of pending messages in message queue */
    kal_uint32 pending_msgs;
    /* maximum number of pending messages in message queue */
    kal_uint32 max_msgs;
} kal_msgq_info;

/* opaque type for mutex identity */
typedef void *kal_mutexid;

/* opaque type for event group identity */
typedef struct {
    kal_uint8 unused;
} *kal_eventgrpid;

/* opaque type for partition memory pool identity */
typedef struct {
    kal_uint8 unused;
} *kal_poolid;

/* opaque type for Application Dynamic Memory identity */
typedef struct {
    kal_uint8 unused;
} *KAL_ADM_ID;

/* opaque type for Application Fix Memory identity */
typedef struct {
    kal_uint8 unused;
} *KAL_AFM_ID;

/* opaque type for First Level Memory Manager identity */
typedef struct {
    kal_uint8 unused;
} *KAL_FLMM_ID;

/* opaque type for Second Level Memory Manager identity */
typedef struct {
    kal_uint8 unused;
} *KAL_SLMM_ID;

/* opaque type for enhance mutex identity */
typedef struct {
    kal_uint8 unused;
} *kal_enhmutexid;

/* opaque type for kal timer identity */
typedef struct {
    kal_uint8 unused;
} *kal_timerid;

/* opaque type for event scheduler identity */
typedef struct event_scheduler {
    void *tmr;
} event_scheduler;

/* opaque type for event identity */
typedef void *eventid;

typedef void *kal_sem; // type of semiphores

typedef enum {
    RPS_TOTAL_STACK_TASKS
} task_indx_type;
/* task entry function parameter */
typedef struct task_entry_struct {
    /* this task index */
    task_indx_type task_indx;
} task_entry_struct;

/* general function pointer prototype */
typedef void (*kal_func_ptr)(void);
/* task entry function prototype */
typedef void (*kal_task_func_ptr)(task_entry_struct *task_entry_ptr);
/* hisr entry function prototype */
typedef void (*kal_hisr_func_ptr)(void);
/* timer (kal timer, stack timer, event scheduler) callback function prototype */
typedef void (*kal_timer_func_ptr)(void *param_ptr);

/*************************************************************************
 * Type Definitions for ILM communication
 *************************************************************************/

/* transfer direction flags for local_para & peer_buff, only TD_RESET is meaningful now */
typedef enum {
    TD_UL = 0x01 << 0,   /* Uplink Direction, obsolete */
    TD_DL = 0x01 << 1,   /* Downlink Direction, obsolete */
    TD_CTRL = 0x01 << 2, /* Control Plane. Both directions, obsolete */
    TD_RESET = 0x01 << 3 /* Reset buffer content to 0 */
} transfer_direction;

#define LOCAL_PARA_HDR \
   kal_uint8    ref_count; \
   kal_uint8    lp_reserved; \
   kal_uint16   msg_len;
/*  common local_para header */
typedef struct local_para_struct {
    /* ref_count: reference count;
     * lp_reserved : reserved for future;
     * msg_len  : total length including this header.
     */
    LOCAL_PARA_HDR
#ifdef __BUILD_DOM__
    ;
#endif
} local_para_struct;

#define PEER_BUFF_HDR \
   kal_uint16   pdu_len; \
   kal_uint8    ref_count; \
   kal_uint8    pb_resvered; \
   kal_uint16   free_header_space; \
   kal_uint16   free_tail_space;
/* peer buffer header, user should treat it as opaque type */
typedef struct peer_buff_struct {
    PEER_BUFF_HDR
#ifdef __BUILD_DOM__
    ;
#endif
} peer_buff_struct;

/* The Interlayer Message structure, which is exchaged between modules. */
typedef struct ilm_struct {
    module_type       src_mod_id;      /* Source module ID of the message. */
    module_type       dest_mod_id;     /* Destination module ID of the message. */
    sap_type          sap_id;          /* Service Access Pointer Identifier. */
    msg_type          msg_id;          /* Message identifier */
    local_para_struct *local_para_ptr; /* local_para pointer */
    peer_buff_struct  *peer_buff_ptr;  /* peer_buff pointer */
} ilm_struct;

/* maximum payload size in an inline-ILM message */
#define MSG_INLINE_ILM_MAX_PAYLOAD (sizeof(local_para_struct *) + sizeof(peer_buff_struct  *))

/*************************************************************************
 * Type Definitions for stack timer
 *************************************************************************/

typedef enum {
    STACK_TIMER_INITIALIZED,
    STACK_TIMER_NOT_RUNNING = STACK_TIMER_INITIALIZED,
    STACK_TIMER_RUNNING,
    STACK_TIMER_NOT_TIMED_OUT = STACK_TIMER_RUNNING,        /* Backward compatiable */
    STACK_TIMER_EXPIRED,
    STACK_TIMER_TIMED_OUT = STACK_TIMER_EXPIRED,                /* Backward compatiable */
    STACK_TIMER_STOPPED
} stack_timer_status_type;

typedef enum {
    STACK_TIMER_USER_VIEW_STOPPED,
    STACK_TIMER_USER_VIEW_RUNNING
} stack_timer_user_view_status_type;

/* opaque type for stack timer, don't reference its member directly */
typedef struct stack_timer_struct_t {
    stack_timer_user_view_status_type timer_user_view_status;
    stack_timer_status_type timer_status;
    kal_uint8               invalid_time_out_count;
    kal_timerid             kal_timer_id;
    kal_uint16              timer_indx;
    module_type             dest_mod_id;
#if defined(KAL_ON_OSCAR)
    kal_uint8               ut_not_send_msg;
#endif
} stack_timer_struct;

/* define kal status */
typedef enum {
    KAL_SUCCESS,            /* the operation is succeeded */
    KAL_ERROR,              /* general error */
    KAL_Q_FULL,             /* queue is full */
    KAL_Q_EMPTY,            /* queue is empty */
    KAL_SEM_NOT_AVAILABLE,  /* semaphore is not available at the moment */
    KAL_WOULD_BLOCK,        /* the operation would block but requester don't want */
    KAL_MESSAGE_TOO_BIG,    /* message is too big */
    KAL_INVALID_ID,         /* an invalid identity */
    KAL_NOT_INITIALIZED,    /* the resource is not initialized */
    KAL_INVALID_LENGHT,     /* an invalid length */
    KAL_NULL_ADDRESS,       /* the address is NULL */
    KAL_NOT_RECEIVE,        /* no receiver want this message */
    KAL_NOT_SEND,           /* can't send this message out */
    KAL_MEMORY_NOT_VALID,   /* memory is corrupted */
    KAL_NOT_PRESENT,        /* the request resource is not present */
    KAL_MEMORY_NOT_RELEASE, /* some memory is not released */
    KAL_TIMEOUT             /* the opearation time out */
} kal_status;



#define KAL_MAX_TICKS           (2048*26*51)
#define ALIGNED_TIMER_MAX_DELAY 255

//todo
#define KAL_ON_NUCLEUS

#ifdef KAL_ON_NUCLEUS
/* define how many miliseconds per system tick represent */
#define KAL_MILLISECS_PER_TICK      4.615

/* Following defines are internal to the KAL */
#define KAL_TICKS_10_MSEC           (10)         /* 10 msec */
#define KAL_TICKS_50_MSEC           (50)        /* 50 msec */
#define KAL_TICKS_100_MSEC          (100)        /* 100 msec */
#define KAL_TICKS_500_MSEC          (500)       /* 500 msec */
#define KAL_TICKS_1024_MSEC         (1024)       /* 1024 msec */
#define KAL_TICKS_1_SEC             (1000)       /* 1 sec */
/*since rmc_context.h defined KAL_TICKS_2_SEC*/
#define KAL_TICKS_2_SEC_2           (2000)       /* 2 sec */
#define KAL_TICKS_3_SEC             (3000)       /* 3 sec */
#define KAL_TICKS_5_SEC             (5000)      /* 5 sec */
#define KAL_TICKS_30_SEC            (30000)      /* 30 sec */
#define KAL_TICKS_1_MIN             (60000)     /* 1 min */
#define KAL_MSEC_64_TICKS           (64)       /* 64 ticks */
#define KAL_MSEC_256_TICKS          (256)      /* 256 ticks */

#endif /* KAL_ON_NUCLEUS */

#ifndef OFFSET_OF
#define OFFSET_OF(_type, _field)    ((kal_uint32)&(((_type *)0)->_field))
#endif

typedef struct {
    kal_uint8   ref_count;
    kal_uint16   param_len;
    kal_uint8   index;
    kal_uint8   string[128];
}
tst_module_string_inject_struct;


#endif  /* _KAL_PUBLIC_DEFS_H */
