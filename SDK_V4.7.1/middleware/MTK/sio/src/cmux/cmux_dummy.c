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

#include "cmux_porting.h"

#define BIN_SIZE    (q->bin_size+2)
#define PREV        (q->bin_size  )
#define NEXT        (q->bin_size+1)

void
internal_create_lqueue(lcd_lqueue *q, malloc_fp_t alloc_fn_p,
                       free_fp_t free_fn_p)
{
    q->head = q->tail = 0;
    q->is_empty       = KAL_TRUE;
    q->end            = q->start = NULL;
    q->bin_count      = 0;
    q->alloc_fn_p     = alloc_fn_p;
    q->free_fn_p      = free_fn_p;
}

lcd_lqueue *
lcd_create_lqueue_with_mem(unsigned int bin_size, malloc_fp_t alloc_fn_p,
                           free_fp_t free_fn_p, void *mem_ptr)
{
    lcd_lqueue *q;
    configASSERT(alloc_fn_p != NULL && free_fn_p != NULL && mem_ptr != NULL);
    q = (lcd_lqueue *)mem_ptr;
    q->bin_size = bin_size;
    internal_create_lqueue(q, alloc_fn_p, free_fn_p);
    q->is_cntxt_mem_usr_mem = KAL_TRUE;
    return q;
}

void
internal_insert_at_rear_to_new_bin(lcd_lqueue *q, void *item,
                                   lcd_lqueue_bin  new_bin )
{
    memset (new_bin, 0, sizeof(void *) * BIN_SIZE);
    if (q->bin_count == 0) {
        q->start = new_bin;
    }
    new_bin[PREV] = q->end;
    new_bin[NEXT] = NULL;
    new_bin[0] = item;
    if (q->end != NULL) {
        q->end[NEXT] = (void *) new_bin;
    }
    q->end = new_bin;
    q->tail = 1;
    q->bin_count++;
}

kal_bool
lcd_lqueue_insert_at_rear(lcd_lqueue *q, void *item)
{
    lcd_lqueue_bin temp;
    configASSERT(q->alloc_fn_p != NULL && q->free_fn_p != NULL);
    if ((q->tail < q->bin_size) && (q->bin_count != 0)) {
        /* There is space in the last bin. */
        q->end[q->tail++] = item;
    } else {
        /* Last bin is full, allocate new bin. */
        temp = (lcd_lqueue_bin)
               q->alloc_fn_p(sizeof(void *) *BIN_SIZE);
        if (temp != NULL) {
            internal_insert_at_rear_to_new_bin(q, item, temp);
        } else {
            return KAL_FALSE;
        }
    }
    q->is_empty = KAL_FALSE;
    return KAL_TRUE;
}

void *
lcd_lqueue_remove(lcd_lqueue *q, void **mem_to_free)
{
    lcd_lqueue_bin temp = q->start;
    void *rv = temp[q->head++];
    if (q->start == q->end) {
        if (q->head == q->tail) {
            /* Queue is empty */
            q->head = q->tail = 0;
            q->start = q->end = NULL;
            q->is_empty = KAL_TRUE;
            q->bin_count--;
            if (q->alloc_fn_p != NULL && q->free_fn_p != NULL) {
                q->free_fn_p((void *)temp);
                if (mem_to_free != NULL) {
                    *mem_to_free = NULL;
                }
            } else {
                *mem_to_free = temp;
            }
        }
    } else {
        if (q->head == q->bin_size) {
            /* Starting bin is empty, next bin becomes
             * new starting bin.
             */
            q->start = q->start[NEXT];
            q->start[PREV] = NULL;
            q->head = 0;
            q->bin_count--;
            if (q->alloc_fn_p != NULL && q->free_fn_p != NULL) {
                q->free_fn_p((void *)temp);
                if (mem_to_free != NULL) {
                    *mem_to_free = NULL;
                }
            } else {
                *mem_to_free = temp;
            }
        }
    }
    return rv;
}

kal_bool
lcd_lqueue_is_empty(lcd_lqueue *q)
{
    return q->is_empty;
}

