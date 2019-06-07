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

#ifndef _KAL_PUBLIC_API_H
#define _KAL_PUBLIC_API_H

/*******************************************************************************
 * Common Header File Include
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "ucos_ii.h"
#include "freertos_ii.h"

#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_trace.h"
#include "syslog.h"
//#include "lwipopts.h"
//#include "pbuf.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef QueueHandle_t       os_queue_t;
typedef SemaphoreHandle_t   os_semaphore_t;
typedef TimerHandle_t       os_tmr_t;
typedef TaskHandle_t        os_task_t;

#define os_queue_full       errQUEUE_FULL
#define os_queue_empty      errQUEUE_EMPTY

#define os_false            pdFALSE
#define os_true             pdTRUE
#define os_pass             os_true
#define os_fail             os_false

#ifndef __KAL_INLINE__

#if defined( __CC_ARM )
#define __KAL_INLINE__                  static __inline
#elif defined( __ICCARM__ )
#define __KAL_INLINE__                  static inline
#else
#define __KAL_INLINE__                  static __inline
#endif

#endif

typedef unsigned long mem_size_t;
//typedef unsigned short mem_size_t;
//Use FreeRTOS mem API
//#define MEM_CALLOC mem_calloc         //mask because of not used
#define MEM_MALLOC pvPortMalloc     //#define MEM_MALLOC mem_malloc
#define MEM_FREE vPortFree          //#define MEM_FREE mem_free
//extern void printc( char* szFormat, ... );    //change to syslog wifi module INFO

#if defined (MTK_DEBUG_LEVEL_NONE)

#define printc(message,...)
#define printc_switch(_log_switch)

#else

#define printc(message,...) \
do { \
    extern log_control_block_t log_control_block_wifi; \
    log_control_block_wifi.print_handle(&log_control_block_wifi, \
                                             __FUNCTION__, \
                                             __LINE__, \
                                             PRINT_LEVEL_INFO, \
                                             (message), \
                                             ##__VA_ARGS__); \
} while (0)

#define printc_switch(_log_switch) \
do { \
    extern log_control_block_t log_control_block_wifi; \
    _log_switch = _log_switch==0 ? DEBUG_LOG_ON : DEBUG_LOG_OFF; \
    log_control_block_wifi.log_switch = (_log_switch); \
} while (0)

#endif/*(MTK_DEBUG_LEVEL_NONE)*/



int sscanfl(char *szInput, char *szFormat, ...);
//extern void *mem_malloc(mem_size_t size);
//extern void *mem_calloc(mem_size_t count, mem_size_t size);
//extern void  mem_free(void *mem);
extern uint32_t random(void);
extern void wlan_rx_pkt(peer_buff_struct *peer_buff_ptr);

extern kal_uint32 wndrv_dbglevel;

#define TRACE_MSG       TRACE_STATE
#define TRACE_BUFF      TRACE_GROUP_9

#define TRACE_BUFF_DEBUG    0

#if(TRACE_BUFF_DEBUG == 1)
extern void ILMTraceSetCheckflag(kal_bool checkflag);
extern void ILMTraceRecord(ilm_struct *ilm_ptr);
extern void ILMTraceCheck(unsigned short point);
#endif

#if 0
#define KAL_PUBLIC_TRACE(tst_lev,...)                       \
    do{                                                     \
        if((1 << tst_lev) & wndrv_dbglevel){                \
            printc("[MT5931] "__VA_ARGS__);                 \
        }                                                   \
    }while(0)

#define ASSERT(exp)                                             \
    do {                                                        \
        if (!(exp))                                             \
            printc("[MT5931] ASSERT FAIL:"#exp", %s, %d\n", __FILE__, __LINE__); \
    } while (0)

#define EXT_ASSERT(exp, e1, e2, e3)                             \
    do {                                                        \
        if (!(exp))                                             \
            printc("[MT5931] ASSERT FAIL:"#exp", %s, %d, 0x%X, 0x%X, 0x%X\n", __FILE__, __LINE__, e1, e2, e3)   ; \
    } while (0)
#else
#define RED_ON " "      //"\033[31m" //it's used to control color, mask
#define RED_OFF " "     //"\033[0m"  //it's used to control color, mask 


#define KAL_PUBLIC_TRACE(tst_lev,...)                           \
    do{                                                         \
        if((1 << tst_lev) & wndrv_dbglevel){                    \
            if((tst_lev == TRACE_ERROR) || (tst_lev == TRACE_WARNING)){ \
                printc(RED_ON"[MT5931] "__VA_ARGS__);printc("\n"RED_OFF);}\
            else if(tst_lev == TRACE_BUFF){printc(__VA_ARGS__);}\
            else{printc("[MT5931] "__VA_ARGS__);}               \
        }                                                       \
    }while(0)

#define ASSERT(exp)                                             \
    do {                                                        \
        if ((exp) == 0)                                             \
            printc(RED_ON"[MT5931] ASSERT FAIL:"#exp ", %s, %d\n"RED_OFF, __FILE__, __LINE__); \
    } while (0)

#define EXT_ASSERT(exp, e1, e2, e3)                             \
    do {                                                        \
        if ((exp) == 0)                                             \
            printc(RED_ON"[MT5931] ASSERT FAIL:"#exp", %s, %d, 0x%X, 0x%X, 0x%X\n"RED_OFF, __FILE__, __LINE__, e1, e2, e3)  ; \
    } while (0)
#endif

/* DOM-NOT_FOR_SDK-BEGIN */
/*******************************************************************************
 * Category 1 : StdLib-like Function
 *******************************************************************************/

__KAL_INLINE__ void *kal_mem_cpy(void *dest, const void *src, kal_uint32 size)

{
    //return memcpy ( dest, src, size );
    char *d = dest;
    const char *s = src;
    while (size--) {
        *d++ = *s++;
    }
    return dest;
}

__KAL_INLINE__ void *kal_mem_set(void *dest, kal_int32 value, kal_uint32 size)
{
    //return memset ( dest, value, size );
    char *p = dest;
    while (size--) {
        *p++ = value;
    }
    return dest;
}


__KAL_INLINE__ kal_int32 kal_mem_cmp(const void *src1, const void *src2, kal_uint32 size)
{
    //return memcmp ( src1, src2, size );
    const unsigned char *p1 = src1, *p2 = src2;
    if (size == 0) {
        return 0;
    }

    while (*p1 == *p2) {
        p1++;
        p2++;
        size--;
        if (size == 0) {
            return 0;
        }
    }
    return *p1 - *p2;
}


__KAL_INLINE__ void *kal_mem_bwcpy(void *dest, const void *src, kal_uint32 size)
{
    char *destaddr = (char *)dest + (size - 1);
    char const *srcaddr = (char *)src + (size - 1);
    while (size-- > 0) {
        *destaddr-- = *srcaddr--;
    }
    return destaddr;
}


#define KAL_MS_TO_TICK(x)       x
#define KAL_TICKS_PER_SEC       1000
#define KAL_OS_TIME_GET     xTaskGetTickCount

typedef struct {
    kal_uint32  pbos;           // lower address
    kal_uint32  ptos;           // higer address
    kal_uint8   ubPriority;
    kal_char   *pTaskName;
    kal_uint32  usStackDepth;
} KAL_TASK_CFG;
extern kal_uint8 kal_create_task(void (*taskproc)(void *param), KAL_TASK_CFG *task_cfg, void *param);

extern void kal_sleep_task(kal_uint32 time_in_ticks);
void kal_delete_task(TaskHandle_t task);

void kal_task_enter_critical();

void kal_task_exit_critical();

extern void kal_get_time(kal_uint32 *ptime);

extern void kal_task_stack_chk(unsigned char prio);

kal_int32 kal_create_sem(kal_sem *sem, unsigned char count);

kal_uint32 kal_take_sem(kal_sem *sem, unsigned int timeout);

void kal_give_sem(kal_sem *sem);

void kal_delete_sem(kal_sem *sem);

void hexdump(char *header, unsigned char *src, unsigned int len);

extern ilm_struct *allocate_ilm(module_type module_id);

extern void free_int_ilm(ilm_struct *ilm_ptr, kal_char *file_name, kal_uint32 line);

extern kal_uint32 get_ilm_avaliable_count(module_type module_id);

#define free_ilm(ilm_ptr)\
{\
   if ((ilm_ptr)->src_mod_id != MOD_TIMER) \
      free_int_ilm((ilm_ptr), NULL, 0); \
}

#define SEND_ILM( src_mod, dest_mod, sap, ilm_ptr)\
{ \
   ilm_ptr->src_mod_id  = src_mod;  \
   ilm_ptr->dest_mod_id = dest_mod; \
   ilm_ptr->sap_id = sap; \
   if (src_mod != dest_mod) { \
     msg_send_ext_queue(ilm_ptr); \
   } else { \
   } \
}

extern void *construct_int_local_para(kal_uint16 local_para_size, kal_uint32 auto_reset,
                                      kal_char *file_ptr, kal_uint32 line);
extern void free_int_local_para(local_para_struct *local_para_ptr, kal_char *file, kal_uint32 line);
extern void *construct_int_peer_buff(kal_uint16 pdu_len, kal_uint16 header_len, kal_uint16 tail_len,
                                     kal_char *file_name_ptr, kal_uint32 line);
extern void free_int_peer_buff(peer_buff_struct *pdu_ptr, kal_char *file, kal_uint32 line);

#define __construct_local_para(local_para_size, direction) \
            construct_int_local_para(local_para_size, ((kal_uint32)(direction) & (kal_uint32)TD_RESET)? 1 : 0, NULL, 0)
#define __free_local_para(local_para) free_int_local_para(local_para, NULL, 0)
#define __construct_peer_buff(pdu_len, header_len, tail_len, direction) \
            construct_int_peer_buff(pdu_len, header_len, tail_len, NULL, 0)
#define __free_peer_buff(peer_buff) free_int_peer_buff(peer_buff, NULL, 0)

/* DOM-NOT_FOR_SDK-END */
/* DOM-NOT_FOR_SDK-BEGIN */
/*******************************************************************************
* <GROUP Functions>
*
* FUNCTION
*  construct_local_para
* DESCRIPTION
*  allocate a local_para structure which has local_para_size bytes including LOCAL_PARA_HDR.
* PARAMETERS
*  local_para_size: [IN] local_para structure size, this size includes LOCAL_PARA_HDR header,
*                   so the minimum size is sizeof(LOCAL_PARA_HDR) which imply the pdu part is 0 byte.
*  direction: [IN] a bitmask, each bit is defined as following values:
*             0 - nothing.
*             TD_RESET - to zero-initialize the pdu in this local_para structure.
* RETURNS
*  a local_para structure pointer. System enter fatal error handling if no enough memory.
* NOTE
*  (1) the prototype of this API is:
*       void* construct_local_para(kal_uint16 local_para_size, transfer_direction direction);
*  (2) local_para structure is a user-defined structure has a predefined header LOCAL_PARA_HDR
*      embedded in front of it:
*          struct bt_connected_info {
*              LOCAL_PARA_HDR; // this field must be the first elements in structure
*              // user-defined fields follows, pdu of this local_para structure
*          };
*      LOCAL_PARA_HDR is defined as:
*          #define LOCAL_PARA_HDR       kal_uint8  ref_count; kal_uint8 lp_reserved; kal_uint16  msg_len;
*      where msg_len is used to record the length of total structre including LOCAL_PARA_HDR,
*      ref_count is used to record how many pointers reference to this structure, you must
*      hold it on saving a private copy and free it after clear the private copy.
*  (3) The initial reference count in local_para structure is 1. It is a common error to
*      memset the whole structure to 0, if you want to zero-initialize pdu, pass TD_RESET
*      in direction to do that.
* SEE ALSO
*  free_local_para, hold_local_para, destroy_ilm, get_local_para_ptr
******************************************************************************/
#define construct_local_para(local_para_size, direction) __construct_local_para(local_para_size, direction)
/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  free_local_para
 * DESCRIPTION
 *  decrease the local_para strucutre reference count, free the local_para if it reach 0.
 * PARAMETERS
 *  local_para        : [IN] local_para structure pointer.
 * RETURN VALUES
 *  N/A
 * NOTE
 *  (1)The prototype of this API is:
 *      void free_local_para(local_para_struct *local_para_ptr);
 *  (2)To pass user-defined local_para structure pointer to it, you need an explicit type-cast:
 *      free_local_para((local_para_struct *)ptr_to_bt_connected_info);
 * SEE ALSO
 *  construct_local_para, hold_local_para, free_local_para_r, destroy_ilm
 ******************************************************************************/
#define free_local_para(local_para) __free_local_para(local_para)

/*******************************************************************************
* <GROUP Functions>
*
* FUNCTION
*  construct_peer_buff
* DESCRIPTION
*  allocate a peer_buff structure which has specified size in each part.
* PARAMETERS
*  pdu_len: [IN] pdu size.
*  header_len: [IN] reserve size in header.
*  tail_len: [IN] reserve size in tail.
*  direction: [IN] ignored, use 0.
* RETURNS
*  a peer_buff structure pointer. System enter fatal error handling if no enough memory.
* NOTE
*  (1) the prototype of this API is:
*       void* construct_peer_buff(kal_uint16 pdu_len, kal_uint16 header_len, kal_uint16 tail_len,
*                                  transfer_direction direction);
*  (2) peer_buff is a opaque type which has reference count. Please use related API to manipulate it.
*  (3) The initial reference count in peer_buff structure is 1.
* SEE ALSO
*  free_peer_buff, hold_peer_buff, destroy_ilm, get_peer_buff_pdu, append_to_peer_buff,
*  prepend_to_peer_buff, remove_head_of_peer_buff, remove_tail_of_peer_buff, update_peer_buff_header
******************************************************************************/
#define construct_peer_buff(pdu_len, header_len, tail_len, direction) \
            __construct_peer_buff(pdu_len, header_len, tail_len, direction)
/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  free_peer_buff
 * DESCRIPTION
 *  decrease the peer_buff strucutre reference count, free the peer_buff if it reach 0.
 * PARAMETERS
 *  peer_buff        : [IN] peer_buff structure pointer.
 * RETURN VALUES
 *  N/A
 * NOTE
 *  (1)The prototype of this API is:
 *      void free_peer_buff(peer_buff_struct *peer_buff_ptr);
 * SEE ALSO
 *  construct_peer_buff, free_peer_buff_r, hold_peer_buff, destroy_ilm, get_peer_buff_pdu, append_to_peer_buff,
 *  prepend_to_peer_buff, remove_head_of_peer_buff, remove_tail_of_peer_buff, update_peer_buff_header
 ******************************************************************************/
#define free_peer_buff(peer_buff) __free_peer_buff(peer_buff)

/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  hold_local_para
 * DESCRIPTION
 *  If local_para_ptr is not NULL, increment its reference counter by 1.
 * PARAMETERS
 *  local_para_ptr        : [IN] local_para structure pointer.
 * RETURN VALUES
 *  KAL_TRUE if local_para_ptr is not NULL pointer, otherwise, KAL_FALSE.
 * NOTE
 *  (1) To pass user-defined local_para structure pointer to it, you need an explicit type-cast:
 *      hold_local_para((local_para_struct *)ptr_to_bt_connected_info);
 * SEE ALSO
 *  construct_local_para, hold_local_para_r, free_local_para, destroy_ilm
 ******************************************************************************/
extern kal_bool hold_local_para(local_para_struct *local_para_ptr);
/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  get_local_para_ptr
 * DESCRIPTION
 *  retrieving start address & length of user-defined local_para structure.
 * PARAMETERS
 *  local_para_ptr        : [IN] local_para structure pointer.
 *  local_para_len_ptr    : [OUT] to store length. If it is NULL, the length is not stored.
 * RETURN VALUES
 *  start address of user-defined local_para structure, It's equal to local_para_ptr.
 * NOTE
 *  (1)To pass user-defined local_para structure pointer to it, you need an explicit type-cast:
 *      get_local_para_ptr((local_para_struct *)ptr_to_bt_connected_info, &len);
 * SEE ALSO
 *  construct_local_para
 ******************************************************************************/
void *get_local_para_ptr(local_para_struct *local_para_ptr, kal_uint16 *local_para_len_ptr);

/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  hold_peer_buff
 * DESCRIPTION
 *  If peer_buff_ptr is not NULL, increment its reference counter by 1.
 * PARAMETERS
 *  peer_buff_ptr        : [IN] peer_buff structure pointer.
 * RETURN VALUES
 *  KAL_TRUE if peer_buff_ptr is not NULL pointer, otherwise, KAL_FALSE.
 * NOTE
 *  N/A
 * SEE ALSO
 *  construct_peer_buff, hold_peer_buff_r, free_peer_buff, destroy_ilm
 ******************************************************************************/
extern kal_bool hold_peer_buff(peer_buff_struct *peer_buff_ptr);
/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  prepend_to_peer_buff
 * DESCRIPTION
 *  prepend data to head of peer_buff. On success, pdu length increases header_len bytes,
 *  and free space at head decreases header_len bytes.
 * PARAMETERS
 *  peer_buff_ptr        : [IN] peer_buff structure pointer.
 *  header_data_ptr      : [IN] data appended.
 *  header_len           : [IN] length of data appended.
 * RETURN VALUES
 *  N/A
 * NOTE
 *  If the free head room is smaller than header_len, system enter fatal error handling.
 * SEE ALSO
 *  construct_peer_buff, free_peer_buff, hold_peer_buff, destroy_ilm, get_peer_buff_pdu,
 *  append_to_peer_buff, remove_head_of_peer_buff, remove_tail_of_peer_buff, update_peer_buff_header
 ******************************************************************************/
extern void prepend_to_peer_buff(peer_buff_struct *peer_buff_ptr,
                                 void *header_data_ptr,
                                 kal_uint16 header_len);
/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  append_to_peer_buff
 * DESCRIPTION
 *  append data to tail of peer_buff, On success, pdu length increases tail_len bytes,
 *  and free space at tail decreases tail_len bytes.
 * PARAMETERS
 *  peer_buff_ptr        : [IN] peer_buff structure pointer.
 *  tail_data_ptr        : [IN] data appended.
 *  tail_len             : [IN] length of data appended.
 * RETURN VALUES
 *  N/A
 * NOTE
 *  If the free tail room is smaller than tail_len, system enter fatal error handling.
 * SEE ALSO
 *  construct_peer_buff, free_peer_buff, hold_peer_buff, destroy_ilm, get_peer_buff_pdu,
 *  prepend_to_peer_buff, remove_head_of_peer_buff, remove_tail_of_peer_buff, update_peer_buff_header
 ******************************************************************************/
extern void append_to_peer_buff(peer_buff_struct *peer_buff_ptr,
                                void *tail_data_ptr,
                                kal_uint16 tail_len);
/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  remove_tail_of_peer_buff
 * DESCRIPTION
 *  remove data at tail of peer_buff. On success, pdu length decreases tail_len bytes,
 *  and free space at tail increases tail_len bytes.
 * PARAMETERS
 *  peer_buff_ptr        : [IN] peer_buff structure pointer.
 *  tail_len             : [IN] length of data to remove.
 * RETURN VALUES
 *  N/A
 * NOTE
 *  If the pdu size is smaller than tail_len, system enter fatal error handling.
 * SEE ALSO
 *  construct_peer_buff, free_peer_buff, hold_peer_buff, destroy_ilm, get_peer_buff_pdu,
 *  prepend_to_peer_buff, append_to_peer_buff, remove_head_of_peer_buff, update_peer_buff_header
 ******************************************************************************/
extern void remove_tail_of_peer_buff(peer_buff_struct *peer_buff_ptr,
                                     kal_uint16 tail_len);

/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  msg_send
 * DESCRIPTION
 *  send a ILM message to tail of destination task's queue.
 *  Refer to NOTE about which queue (extq or intq) the message sent to.
 * PARAMETERS
 *  ilm_ptr             : [IN] the ILM message to be sent.
 * RETURN VALUES
 *  N/A
 * NOTE
 *  (1) This API select queue based on following criteria:
 *        a. If src_mod_id and dest_mod_id in ilm_ptr mapped to same task, and the
 *           internal queue of the task is not NULL, the message is sent to internal queue.
 *        b. Otherwise, the message is sent to external queue.
 *  (2) The ILM messages is appended to tail of the queue.
 *  (3) If the destination queue is full, system enter fatal error handling.
 *  (4) This API replaces msg_send_ext_queue/msg_send_int_queue().
 *      They will be removed from next release.
 *  (5) The use of allocate_ilm()/cancel_ilm() is deprecated,
 *      They will be removed from next release.
 * SEE ALSO
 *  msg_send_to_head, msg_send6, msg_send_to_head6
 ******************************************************************************/


extern void remove_hdr_of_peer_buff(peer_buff_struct *peer_buff_ptr,
                                    kal_uint16 hdr_len);

extern void update_peer_buff_hdr(peer_buff_struct *peer_buff_ptr,
                                 kal_uint16        new_hdr_len,
                                 kal_uint16        new_pdu_len,
                                 kal_uint16        new_tail_len);

extern void *get_pdu_ptr(peer_buff_struct *peer_buff_ptr, kal_uint16 *length_ptr);


typedef enum {

    FLC2_POOL_ID_APPPDU_UL,         /* 0 */
    FLC2_POOL_ID_APPPDU_DL,
    FLC2_POOL_ID_SPDU_DL,
    FLC2_POOL_ID_HS,                        /* Pool used for hot spot feature */
    FLC2_POOL_ID_SPDU_HS = FLC2_POOL_ID_HS,
    FLC2_POOL_ID_TPDU,
    FLC2_INVALID_POOL_ID = 0xff
} flc2_pool_id_enum;
#if defined (WLAN_ZERO_COPY)

extern void *pbuf_to_flc2(struct pbuf *pbuf_ptr);

extern void *flc2_to_pbuf(peer_buff_struct *peer_buff_ptr);

extern void *flc2_ent_get_peer_buff(flc2_pool_id_enum pool_id, kal_uint16 pdu_len,
                                    kal_uint16 header_size, kal_uint16 tail_size);
extern void flc2_ent_free_peer_buff(flc2_pool_id_enum pool_id, peer_buff_struct *buff_ptr);


#define flc2_get_peer_buff(_pool_id_, _pdu_len_, _header_size_, _tail_size_) \
        flc2_ent_get_peer_buff(_pool_id_, _pdu_len_, _header_size_, _tail_size_)

#define flc2_free_peer_buff(_pool_id_, _buff_ptr_) \
        flc2_ent_free_peer_buff(_pool_id_, _buff_ptr_)

#endif

extern void *wifi_get_ctrl_buffer(unsigned long size, void *fun);
extern void wifi_free_ctrl_buffer(void *ptr, void *fun);



/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  evshed_create
 * DESCRIPTION
 *  create a new event scheduler.
 * PARAMETERS
 *  evshed_name: [IN] event scheduler name. it's used for debug only, and only the
 *                    first 8 characters retain.
 *  module_id: [IN] the module this event scheduler belongs to. When event scheduler
 *              timeout happens, system sent timeout message to task of this module.
 *  allowed_time_shift: [IN]
 *      Timer inaccuracy in ticks that the event scheduler allow, regardless of sleep mode.
 *      If set, the event may delay or prematurely timeout for maximum allowed_time_shift ticks.
 *      If don't like it, just set it to 0.
 *
 *      Advantage -
 *      Set this argument will save you a lot of CPU resource if the events are frequent and close to each other.
 *
 *  allowed_time_shift_sleep_mode: [IN]
 *      Behaviour -
 *      Time inaccuracy in ticks the event scheduler allow, due to target enters sleep mode.
 *      The event may delay for max allowed_time_shift_sleep_mode tick if the target enters sleep mode,
 *      so that the target can sleep longer. The event will suffer no delay if the target doesn't enter sleep mode.
 *
 *      Advantage -
 *      Set this argument will save the target a lot of power. You are encouraged to set this value for the sake of power.
 *
 *      Note -
 *      0<=allowed_time_error_shift_mode<=255
 *
 * RETURNS
 *  Successful: created event scheduler pointer.
 *  Fail: system enter fatal error handling.
 * NOTE
 *  (1)This API replaces new_evshed(). And new_evshed() is deprecated,
 *     it will be removed from next release.
 *  (2)It is strongly suggested to call this function only at system initialization stage.
 *     and This is no way to free the event schedule data structure once allocated.
 *  (3)Event scheduler is not thread safe, if you want to share it in multi-task/hisr,
 *     please use lock to protect it.
 *  (4)To reduce power consumption, it's strongly suggested to set allowed_time_error_sleep_mode.
 *  (5)The inaccuracy of event scheduler comes from
 *       schedule delay -  Time lisr, Time HISR, the owner task may be preempted.
 *       Task handling message delay - There may be many ILMs to handle before the event expired ILM gets handled
 *       allowed_time_shift
 *       allowed_time_shift_sleep_mode
 * SEE ALSO
 *  evshed_set_event evshed_cancel_event evshed_timer_handler
 ******************************************************************************/
extern event_scheduler *evshed_create(kal_char *evshed_name, module_type module_id,
                                      kal_uint32 allowed_time_shift, kal_uint16 allowed_time_shift_sleep_mode);

/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  evshed_set_event
 * DESCRIPTION
 *  set an event on event scheduler.
 * PARAMETERS
 *  es: [IN] event scheduler pointer.
 *  event_hf: [IN] the callback function when event timeout.
 *  event_hf_param: [IN] the user supplied argument for event_hf.
 *  elapse_time: [IN] how many system ticks elapses before this event timeout.
 * RETURNS
 *  Successful: return set eventid.
 *  Fail: return NULL.
 * NOTE
 *  System will allocate memory for event id, it will be freed automatically when
 *      a. event is cancelled
 *      b. event expired
 *   It's not allowed to use the event id after above cases.
 *   If the event id is saved, please be careful to reset it.
 * SEE ALSO
 *  evshed_create evshed_cancel_event evshed_timer_handler
 ******************************************************************************/
extern eventid evshed_set_event(event_scheduler *es,
                                kal_timer_func_ptr event_hf,
                                void *event_hf_param,
                                kal_uint32 elapse_time);

/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  evshed_cancel_event
 * DESCRIPTION
 *  cancel a event on event scheduler.
 * PARAMETERS
 *  es: [IN] event scheduler pointer.
 *  eid: [IN,OUT] pointer to event id.
 * RETURNS
 *  Successful: remain ticks before this event expires.
 *  Fail: -1.
 * NOTE
 *  (1)system would reset the event ID (eid) to NULL before returning; however,
 *     caller should pay attention to its privately saved event id
 * SEE ALSO
 *  evshed_create evshed_set_event evshed_timer_handler evshed_delete_all_events
 ******************************************************************************/
extern kal_int32 evshed_cancel_event(event_scheduler *es, eventid *eid);

/*******************************************************************************
 * <GROUP Functions>
 *
 * FUNCTION
 *  evshed_timer_handler
 * DESCRIPTION
 *  event scheduler main function. It will check all set events status, and call
 *  the callback function if they are expired.
 *
 * PARAMETERS
 *  es: [IN] event scheduler pointer.
 * RETURNS
 *  N/A
 * NOTE
 *  (1)it's not thread-safe. Don't use in parallel.
 *  (2)If an event is expired, the eventid is freed automatically and don't use it any more.
 *     In evshed_timer_handler(), system would execute event registered timeout callback function.
 *     Caller should reset saved event id in registered timeout callback function,
 *     or cause potential bug to cancel wrong timer event.
 * SEE ALSO
 *  evshed_create evshed_set_event evshed_cancel_event evshed_delete_all_events
 *****************************************************************************/
extern void evshed_timer_handler(event_scheduler *es);

/*******************************************************************************
 * <GROUP Synchronous>
 *
 * FUNCTION
 *  kal_create_mutex
 * DESCRIPTION
 *  create a mutex.
 * PARAMETERS
 * RETURNS
 *  Success:        mutex pointer.
 *  Error:          system enter fatal error handling.
 * NOTE
 *  It is strongly suggested to call this function only in system initialization
 *  time, and the related data allocated for the mutex could not be freed
 *  once it\A1\AFs created.
 * SEE ALSO
 *  kal_take_mutex kal_give_mutex
 ******************************************************************************/
extern kal_mutexid kal_create_mutex(kal_uint8 prio);

/*******************************************************************************
 * <GROUP Synchronous>
 *
 * FUNCTION
 *  kal_take_mutex
 * DESCRIPTION
 *  Obtains an instance of the specified mutex. If the mutex is taken already
 *  before this call, the function cannot be immediately satisfied, and caller
 *  task will be suspended until other task give the ownership to caller task.
 *  After the function returns, caller task is the only task held this mutex.
 * PARAMETERS
 *  mutex_ptr        : [IN] mutex pointer.
 * RETURNS
 *  N/A
 * NOTE
 *  (1)Mutex is not a recursive lock. If caller task already held this mutex,
 *     then caller task will suspend forever because any task won^t and can^t
 *     give up mutex^s ownership.
 *  (2)Mutex is a FIFO queue. If multiple tasks are waiting on a mutex, the
 *     first waiting task is selected to be the next owner.
 * SEE ALSO
 *  kal_create_mutex kal_give_mutex
 ******************************************************************************/
extern void kal_take_mutex(kal_mutexid mutex_ptr);

/*******************************************************************************
 * <GROUP Synchronous>
 *
 * FUNCTION
 *  kal_give_mutex
 * DESCRIPTION
 *  Give up mutex ownership. If any task is waiting on this mutex, the first
 *  waiting task is selected to be the next owner.
 * PARAMETERS
 *  mutex_ptr        : [IN] mutex pointer.
 * RETURNS
 *  N/A
 * NOTE
 *  Mutex ownership is private to task. That means, give can be done at the same
 *  task of take, otherwise fatal error handling takes place.
 * SEE ALSO
 *  kal_create_mutex kal_take_mutex
 ******************************************************************************/
extern void kal_give_mutex(kal_mutexid mutex_ptr);

os_queue_t kal_queue_create( uint32_t queue_len, uint32_t item_size);
uint32_t kal_queue_message_waiting(const os_queue_t queue);
uint32_t kal_queue_get_space( const os_queue_t queue);
int32_t kal_queue_receive(os_queue_t queue, void *buf, uint32_t wait_time);
int32_t kal_queue_send(os_queue_t queue, void *item, uint32_t wait_time);
int32_t kal_queue_send_from_isr(os_queue_t queue, void *item);
int32_t kal_queue_send_front(os_queue_t queue, void *item, uint32_t wait_time);
int32_t kal_queue_send_front_from_isr(os_queue_t queue, void *item);


#ifdef __cplusplus
}
#endif

#endif  /* _KAL_PUBLIC_API_H */
