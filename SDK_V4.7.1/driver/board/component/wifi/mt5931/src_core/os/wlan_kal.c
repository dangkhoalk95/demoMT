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
** $Id: //Project/WLAN/MT5921_BRANCHES/MT5921_11AW1112MP/os/wlan_kal.c#1 $
*/

/*******************************************************************************
GENERAL DESCRIPTION

This file contains the main routines of Windows driver for MediaTek Inc.
802.11 Wireless LAN Adapters.
*******************************************************************************/

/*******************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*******************************************************************************/
#define __WNDRV_OS_FILES_C__
//#include "kal_release.h"
//#include "stack_timer.h"  /* Stack timer */
//#include "wndrv_cal.h"
#include "wndrv_os_private.h"
#include "precomp.h"
#include "wndrv_context.h"

/*----------------------------*/
extern kal_uint32 L1I_GetTimeStamp(void);

/* ------------------------------------------------------------------------- */
extern wndrv_context_struct wndrv_context;
#if 0
extern kal_bool             WNDRVTimerEnable;
extern kal_bool             WNDRVBearerEnable;
extern kal_bool             WNDRVPauseFlag;
#endif

/*******************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/

/*******************************************************************************
*                          F U N C T I O N S
*******************************************************************************/

/*******************************************************************************
**  mpMapTidToQueue
**
**  descriptions:
**      Map a given TID with Queue number.
**  parameters:
**      tid:        TID value for mapping
**  return:
**      0: WMM_AC_BK
**      1: WMM_AC_BE
**      2: WMM_AC_VI
**      3: WMM_AC_VO
**  note:
*******************************************************************************/
UINT_8
mpMapTidToQueue(IN UINT_8 tid)
{
    UINT_8 queue;

    /* Support WMM only */
    switch (tid) {
        case 0:
        case 3:
            queue = WNDRV_QUEUE_AC_BE;
            break;
        case 1:
        case 2:
            queue = WNDRV_QUEUE_AC_BK;
            break;
        case 4:
        case 5:
            queue = WNDRV_QUEUE_AC_VI;
            break;
        case 6:
        case 7:
            queue = WNDRV_QUEUE_AC_VO;
            break;
        default:
            queue = WNDRV_QUEUE_AC_BE;
            break;
    }
    return queue;
} /* mpMapTidToQueue */

#if 0 /* Move to gl_kal.c */
/*******************************************************************************
**  kalIndicateStatusAndComplete
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/
#endif

#if 0 /*Move to gl_kal.c */
/*******************************************************************************
**  kalSendComplete
**
**  @brief  Notify OS with SendComplete event of the specific packet.
**          Linux should free packets here.
**  @param  pvAdapter - pointer to adapter descriptor
**  @param  pvPacket - pointer to packet descriptor
**  @param  i4Status - the indicated status
**  @result
**  @return (none)
**  @note
*******************************************************************************/
#endif

#if 0 /* move to gl_kal.c */
/*******************************************************************************
**  kalPacketAlloc
**
**  @brief  Only handles driver own creating packet (coalescing buffer).
**
**  @param  pvAdapter - pointer to adapter descriptor
**  @param  u4Size - needed buffer size
**  @param  ePktPriority - what type of packet is needed
**
**  @result *ppucData pointer to packet starting address
**
**  @return pointer to allocated packet descriptor. NULL if no buffers.
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalPacketFree
**
**  @brief  Only handles driver own creating packet.
**          What about Windows TX's done ?
**  @param  pvAdapter - pointer to adapter descriptor
**  @param  pvPacket - pointer to packet descriptor
**  @result
**  @return (none)
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalPacketPut
**
**  @brief  Before indicate a recied packet to OS, we need to specify the
**          starting address and length of the packet. In Windows,
**          buffer descriptor is created and chained to packet descriptor.
**
**  @param  pvAdapter - pointer to adapter descriptor
**  @param  pvPacket - pointer to packet descriptor
**  @param  pucPacketStart - the starting address of rx packet
**  @param  u4PacketLen - total packet length
**  @result
**  @return (none)
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalPacketTid
**
**  descriptions:
**      This routine is called to return this packet's mapped TID
**  parameters:
**      pvAdapter - pointer to adapter descriptor
**      pvPacket - pointer to packet descriptor
**  result:
**  return:
**      TID
**  note:
*******************************************************************************/
#endif
#if 0 /* move to gl_kal.c */
/*******************************************************************************
**  kalIndicatePackets
**
**  @brief  This routine indicates OS with some received packets.
**          Each rx packet is composed with one buffer.
**
**  @param  pvAdapter - pointer to adapter descriptor
**  @param  apvPackets - pointer array to received packet descriptor
**  @param  u4PacketNum - packet number in apvPackets[]
**  @param  fgRetainPacket - TRUE: this packet will be used immediately
**          after function return.
**  @result
**  @return (none)
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalMemAlloc
**
**  @brief  Cache memory allocation
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalMemFree
**
**  @brief  Cache memory free
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalMemSet
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalMemZero
**
**  @brief  Zero specific memory block
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/
/*******************************************************************************
**  kalMemCopy
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/
/*******************************************************************************
**  kalMemCmp
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/
/*******************************************************************************
**  kalGetTimeTick
**
**  @brief  This function returns the number of milliseconds that have
**          elapsed since the system was booted.
**  @param
**  @result
**  @return the LSB 32-bits of the system uptime
**  @note
*******************************************************************************/
/*******************************************************************************
**  kalUdelay
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/

/*******************************************************************************
**  kalMdelay
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/
#endif
#if 0
/*******************************************************************************
**  kalSetTimer
**
**  @brief
**  @param
**  @result
**  @return
**  @note
*******************************************************************************/
BOOL
kalSetTimer(IN PVOID pvAdapter,
            IN UINT_32 u4Interval)
{
    if (stack_timer_status(&wndrv_context.timer , NULL) == STACK_TIMER_NOT_TIMED_OUT) {
        stack_stop_timer(&wndrv_context.timer);

        WNDRV_TRACE0(TRACE_GROUP_2, INFO_WNDRV_KAL_STOP_TIMER, "WKAL: StopTimer");
    }

    /* minimal time tick is in terms of TDMA timer, 4.615 ms each unit */
    stack_start_timer(&wndrv_context.timer, WNDRV_TIMER_EXPIRY, u4Interval * 13 / 60 + 1);

    WNDRV_TRACE1(TRACE_GROUP_2, INFO_WNDRV_KAL_SET_TIMER,
                 "WKAL: SetTimer=%d", u4Interval);

    return TRUE; /* success */
}
#endif
#if 0 /* move to gl_kal.c */
/*******************************************************************************
**  kalUpdateReAssocReqInfo
**
**  descriptions: This routine is called to update the (re)association
**                request information to the structure used to query and
**                set OID_802_11_ASSOCIATION_INFORMATION.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      frameBody_p - Pointer to the frame body of the last (Re)Association
**                    Response frame from the AP
**      frameBodyLen - The length of the frame body of the last
**                     (Re)Association Response frame
**  return:         (none)
**  note:
*******************************************************************************/

/*******************************************************************************
**  kalUpdateReAssocRspInfo
**
**  descriptions: This routine is called to update the (re)association
**                response information to the structure used to query and
**                set OID_802_11_ASSOCIATION_INFORMATION.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      frameBody_p - Pointer to the frame body of the last (Re)Association
**                    Response frame from the AP
**      frameBodyLen - The length of the frame body of the last
**                     (Re)Association Response frame
**  return:         (none)
**  note:
*******************************************************************************/
#endif

/*****************************************************************************
* FUNCTION
*   void kal_evevt_start_timer(void);
*
* DESCRIPTION
*   start event schedule timer in WNDRV task
*
* CALLS
*
* PARAMETERS
*
* RETURNS
*
* GLOBALS AFFECTED
*
*****************************************************************************/
VOID
kal_event_start_timer(P_ENUM_PARAM_WNDRV_ES es, kal_uint32 uInterval)
{
//   WNDRVTimerEnable = KAL_TRUE;


    if ((wndrv_context.glue_p->event_id[*es])) {
        evshed_cancel_event(wndrv_context.glue_p->event_scheduler_ptr, &wndrv_context.glue_p->event_id[*es]);
        wndrv_context.glue_p->event_id[*es] = (eventid) 0;
    }


    (wndrv_context.glue_p->event_id[*es]) = evshed_set_event(wndrv_context.glue_p->event_scheduler_ptr,
                                            (kal_timer_func_ptr)wndrv_evshed_timer_handler,
                                            (void *) es, uInterval);




}

/*****************************************************************************
* FUNCTION
*   void kal_evevt_stop_timer(void);
*
* DESCRIPTION
*   stop event schedule timer in WNDRV task
*
* CALLS
*
* PARAMETERS
*
* RETURNS
*
* GLOBALS AFFECTED
*
*****************************************************************************/
VOID
kal_event_stop_timer(ENUM_PARAM_WNDRV_ES_T es)
{

    if ((wndrv_context.glue_p->event_id[es])) {

        evshed_cancel_event(wndrv_context.glue_p->event_scheduler_ptr, &wndrv_context.glue_p->event_id[es]);
        wndrv_context.glue_p->event_id[es] = (eventid) 0;
    } else {


    }
}

#if 0 /* Move to gl_kal.c */
/*******************************************************************************
* FUNCTION
*   wndrv_supc_get_pkt_descriptor
*
* DESCRIPTION
*   Get packet descriptor in peer buffer
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#endif
