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

/*
** $Id: //Project/WLAN/MT5921_BRANCHES/MT5921_11AW1112MP/include/os/wndrv_queue.h#1 $
*/
/*******************************************************************************
* Copyright (c) 2003-2004 Inprocomm, Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* Inprocomm, Inc.
********************************************************************************
*/
/*******************************************************************************
GENERAL DESCRIPTION
********************
*/

#ifndef _WNDRV_QUEUE_H
#define _WNDRV_QUEUE_H

/*******************************************************************************
*                E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
//#include "wndrv_typedef.h"
/*******************************************************************************
*                     C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                          C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                         D A T A   T Y P E S
********************************************************************************
*/
/* Queue structures */

typedef struct _QUE_ENTRY {
    struct _QUE_ENTRY   *next_p;
} QUE_ENTRY, *PQUE_ENTRY;

typedef struct _QUE_HEADER {
    PQUE_ENTRY          head_p;
    PQUE_ENTRY          tail_p;
    UINT_32             numElem;
} QUE_HEADER, *PQUE_HEADER;

/*******************************************************************************
*                             M A C R O S
********************************************************************************
*/
#define QUE_INIT(_QueueHeader) \
        { \
            (_QueueHeader)->head_p = (_QueueHeader)->tail_p = NULL; \
            (_QueueHeader)->numElem = 0; \
        }

#define QUE_IS_EMPTY(_QueueHeader)          ((_QueueHeader)->head_p == NULL)

#define QUE_GET_HEAD(_QueueHeader)          ((_QueueHeader)->head_p)

#define Q_REMOVE_HEAD(_QueueHeader) \
        QUE_GET_HEAD(_QueueHeader); \
        { \
            PQUE_ENTRY next_p; \
            WNDRV_ASSERT(_QueueHeader); \
            WNDRV_ASSERT((_QueueHeader)->head_p); \
            next_p = (_QueueHeader)->head_p->next_p; \
            (_QueueHeader)->head_p = next_p; \
            if (next_p == NULL) { \
                (_QueueHeader)->tail_p = NULL; \
            } \
            (_QueueHeader)->numElem --;\
        }

#define QUE_GET_TAIL(_QueueHeader)          ((_QueueHeader)->tail_p)

#define QUE_INS_HEAD(_QueueHeader, _QueueEntry) \
        { \
            WNDRV_ASSERT(_QueueHeader); \
            WNDRV_ASSERT(_QueueEntry); \
            ((_QueueEntry))->next_p = (_QueueHeader)->head_p; \
            (_QueueHeader)->head_p = (_QueueEntry); \
            if ((_QueueHeader)->tail_p == NULL) { \
                (_QueueHeader)->tail_p = (_QueueEntry); \
            } \
            (_QueueHeader)->numElem ++;\
        }

#define QUE_INS_TAIL(_QueueHeader, _QueueEntry) \
        { \
            WNDRV_ASSERT(_QueueHeader); \
            WNDRV_ASSERT(_QueueEntry); \
            (_QueueEntry)->next_p = NULL; \
            if ((_QueueHeader)->tail_p) { \
                (_QueueHeader)->tail_p->next_p =  (_QueueEntry); \
            } else { \
                (_QueueHeader)->head_p =  (_QueueEntry); \
            } \
            (_QueueHeader)->tail_p =  (_QueueEntry); \
            (_QueueHeader)->numElem ++;\
        }

#define QUE_GET_NEXT_ENTRY(_QueueEntry)     ((_QueueEntry)->next_p)

#define QUE_REMOVE_NEXT_ENTRY(_QueueHeader, _QueueEntry) \
        { \
            PQUE_ENTRY nextEntry_p; \
            WNDRV_ASSERT(_QueueHeader); \
            WNDRV_ASSERT(_QueueEntry); \
            nextEntry_p = QUE_GET_NEXT_ENTRY(_QueueEntry); \
            WNDRV_ASSERT(nextEntry_p); \
            (_QueueEntry)->next_p = nextEntry_p->next_p; \
            if ((_QueueEntry)->next_p == NULL) { \
                (_QueueHeader)->tail_p =  (_QueueEntry); \
            } \
            (_QueueHeader)->numElem --;\
        }

#define QUE_MOVE_ALL(_dstQueue, _srcQueue) \
        { \
            WNDRV_ASSERT(QUE_IS_EMPTY(_dstQueue)); \
            (_dstQueue)->head_p = (_srcQueue)->head_p; \
            (_dstQueue)->tail_p = (_srcQueue)->tail_p; \
            QUE_INIT(_srcQueue); \
        }

/*******************************************************************************
*                    D A T A   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                 F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

#endif  /* _WNDRV_QUEUE_H */

