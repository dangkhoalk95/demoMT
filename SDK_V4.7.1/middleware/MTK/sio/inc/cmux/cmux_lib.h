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

#ifndef __CMUX_LIB_H__
#define __CMUX_LIB_H__

#include "cmux_porting.h"

typedef struct {

    kal_uint8   *pDataBuffer;

    kal_uint8   *pRead;
    kal_uint8   *pWrite;

    kal_uint32  u4BufferSize;

} CmuxRingBufferT;

extern void cmux_initBuffer ( CmuxRingBufferT *, kal_uint8 *, kal_uint32 );
extern void cmux_clearBuffer ( CmuxRingBufferT *);

extern void cmux_readBuffer ( CmuxRingBufferT *, kal_uint8 *, kal_uint32 );
extern void cmux_discardDataInBuffer ( CmuxRingBufferT *, kal_uint32 );
extern void cmux_writeBuffer ( CmuxRingBufferT *, kal_uint8 *, kal_uint32 );

extern kal_uint8 *cmux_reserveBufferSpace ( CmuxRingBufferT *, kal_uint32 *);
extern void  cmux_admitReservedBuffer ( CmuxRingBufferT *, kal_uint8 *);
extern kal_uint8 *cmux_writeReservedBuffer ( CmuxRingBufferT *, kal_uint8 *, kal_uint8 *, kal_uint32 );

extern kal_bool cmux_isBufferEmpty ( CmuxRingBufferT *);
extern kal_uint32 cmux_getBufferDataAvail ( CmuxRingBufferT *);
extern kal_uint32 cmux_getBufferSpaceAvail ( CmuxRingBufferT *);

extern void *cmux_alloc ( kal_uint32 );
extern void cmux_free ( void *);

#endif /* __CMUX_LIB_H__ */

