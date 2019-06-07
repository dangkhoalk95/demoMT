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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/cnm_timer.c#2 $
*/

/*! \file   "cnm_timer.c"
    \brief

*/




/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to set the time to do the time out check.
*
* \param[in] rTimeout Time out interval from current time.
*
* \retval TRUE Success.
*
*/
/*----------------------------------------------------------------------------*/
static VOID
cnmTimerSetTimer(
    IN P_ADAPTER_T prAdapter,
    IN OS_SYSTIME  rTimeout
)
{
    DEBUGFUNC("cnmTimerSetTimer");

    ASSERT(prAdapter);

#if defined(WCN_MAUI)
    WNDRV_TRACE(TRACE_SCAN, "cnmTimerSetTimer:rTimeout= %d\n", rTimeout);

    /* Restart CNM timer */
    kal_event_stop_timer(PARAM_WNDRV_ES_CNM_TIMER);
    kal_event_start_timer(
        &prAdapter->prGlueInfo->prParamWndrvEs[PARAM_WNDRV_ES_CNM_TIMER],
        rTimeout);   /*  rTimeout (unit: ticks) */

#else

    kalSetTimer(prAdapter->prGlueInfo, rTimeout);

#endif

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to initialize a root timer.
*
* \param[in] prAdapter
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmTimerInitialize(
    IN P_ADAPTER_T prAdapter
)
{
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
    LINK_INITIALIZE(&(prAdapter->rRootTimer.rLinkHead));
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

    /* glue layer have configured timer */

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to destroy a root timer.
*        When WIFI is off, the token shall be returned back to system.
*
* \param[in]
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmTimerDestroy(
    IN P_ADAPTER_T prAdapter
)
{
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
    LINK_INITIALIZE(&(prAdapter->rRootTimer.rLinkHead));
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

    /* glue layer will be responsible for timer destruction */

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to initialize a timer.
*
* \param[in] prTimer Pointer to a timer structure.
* \param[in] pfnFunc Pointer to the call back function.
* \param[in] u4Data Parameter for call back function.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmTimerInitTimer(
    IN P_ADAPTER_T              prAdapter,
    IN P_TIMER_T                prTimer,
    IN PFN_MGMT_TIMEOUT_FUNC    pfFunc,
    IN UINT_32                  u4Data
)
{
    ASSERT(prAdapter);

    ASSERT(prTimer);

#if DBG
    /* Note: NULL function pointer is permitted for HEM POWER */
    if (pfFunc == NULL) {
        DBGLOG(CNM, WARN, ("Init timer with NULL callback function!\n"));
    }
#endif

#if DBG
    ASSERT(prAdapter->rRootTimer.rLinkHead.prNext);
    {
        P_LINK_T            prTimerList;
        P_LINK_ENTRY_T      prLinkEntry;
        P_TIMER_T           prPendingTimer;

        prTimerList = &(prAdapter->rRootTimer.rLinkHead);

        LINK_FOR_EACH(prLinkEntry, prTimerList) {
            prPendingTimer = LINK_ENTRY(prLinkEntry, TIMER_T, rLinkEntry);
            ASSERT(prPendingTimer);
            ASSERT(prPendingTimer != prTimer);
        }
    }
#endif

    LINK_ENTRY_INITIALIZE(&prTimer->rLinkEntry);

    prTimer->pfMgmtTimeOutFunc  = pfFunc;
    prTimer->u4Data             = u4Data;

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to stop a timer.
*
* \param[in] prTimer Pointer to a timer structure.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
cnmTimerStopTimer_impl(
    IN P_ADAPTER_T              prAdapter,
    IN P_TIMER_T                prTimer,
    IN BOOLEAN                  fgAcquireSpinlock
)
{
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    ASSERT(prTimer);

    if (fgAcquireSpinlock == TRUE) {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
    }

    if (timerPendingTimer(prTimer)) {
        LINK_REMOVE_KNOWN_ENTRY(&(prAdapter->rRootTimer.rLinkHead), &prTimer->rLinkEntry);

        /* Reduce dummy timeout for power saving, especially HIF activity.
         * If two or more timers exist and being removed timer is smallest,
         * this dummy timeout will still happen, but it is OK.
         */
        if (LINK_IS_EMPTY(&(prAdapter->rRootTimer.rLinkHead))) {

#if defined(WCN_MAUI)
            kal_event_stop_timer(PARAM_WNDRV_ES_CNM_TIMER);
#else
            kalCancelTimer(prAdapter->prGlueInfo);
#endif
        }
    }

    if (fgAcquireSpinlock == TRUE) {
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to stop a timer.
*
* \param[in] prTimer Pointer to a timer structure.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmTimerStopTimer(
    IN P_ADAPTER_T              prAdapter,
    IN P_TIMER_T                prTimer
)
{
    ASSERT(prAdapter);
    ASSERT(prTimer);

    cnmTimerStopTimer_impl(prAdapter, prTimer, TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to start a timer.
*
* \param[in] prTimer Pointer to a timer structure.
* \param[in] u4TimeoutMs Timeout to issue the timer and call back function
*                        (unit: ms).
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmTimerStartTimer(
    IN P_ADAPTER_T      prAdapter,
    IN P_TIMER_T        prTimer,
    IN UINT_32          u4TimeoutMs
)
{
    P_LINK_T        prTimerList;
    OS_SYSTIME      rExpiredSysTime, rTimeoutSystime;
    KAL_SPIN_LOCK_DECLARATION();

    DEBUGFUNC("cnmTimerStartTimer");

    ASSERT(prAdapter);

    ASSERT(prTimer);

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
    prTimerList = &(prAdapter->rRootTimer.rLinkHead);

#if defined (WCN_MAUI)
    //rTimeoutSystime = u4TimeoutMs * 13 / 60 + 1;  /* ~= x 1/4.615 => convert ms to ticks */
    rTimeoutSystime = KAL_MS_TO_TICK(u4TimeoutMs);
    rExpiredSysTime = rTimeoutSystime + kalGetTimeTick();
    WNDRV_TRACE(TRACE_SCAN, "cnmTimerStartTimer rTimeoutSystime= %d, rExpiredSysTime= %d\n", rTimeoutSystime, rExpiredSysTime);
#else
    /* If timeout interval is larger than 1 minute, the mod value is set
     * to the timeout value first, then per minutue.
     */
    if (u4TimeoutMs > MSEC_PER_MIN) {
        ASSERT(u4TimeoutMs <= ((UINT_32)0xFFFF * MSEC_PER_MIN));

        prTimer->u2Minutes = (UINT_16)(u4TimeoutMs / MSEC_PER_MIN);
        u4TimeoutMs -= (prTimer->u2Minutes * MSEC_PER_MIN);
        if (u4TimeoutMs == 0) {
            u4TimeoutMs = MSEC_PER_MIN;
            prTimer->u2Minutes--;
        }
    } else {
        prTimer->u2Minutes = 0;
    }

    /* The assertion check if MSEC_TO_SYSTIME() may be overflow. */
    ASSERT(u4TimeoutMs < (((UINT_32)0x80000000 - MSEC_PER_SEC) / KAL_HZ));
    rTimeoutSystime = MSEC_TO_SYSTIME(u4TimeoutMs);
    rExpiredSysTime = kalGetTimeTick() + rTimeoutSystime;
#endif

    /* If no timer pending or the fast time interval is used. */
    if (LINK_IS_EMPTY(prTimerList) ||
            TIME_BEFORE(rExpiredSysTime, prAdapter->rRootTimer.rNextExpiredSysTime)) {

        prAdapter->rRootTimer.rNextExpiredSysTime = rExpiredSysTime;
        cnmTimerSetTimer(prAdapter, rTimeoutSystime);
    }

    /* Add this timer to checking list */
    prTimer->rExpiredSysTime = rExpiredSysTime;

    if (!timerPendingTimer(prTimer)) {
        LINK_INSERT_TAIL(prTimerList, &prTimer->rLinkEntry);
    }

    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routines is called to check the timer list.
*
* \param[in]
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmTimerDoTimeOutCheck(
    IN P_ADAPTER_T      prAdapter
)
{
    P_LINK_T            prTimerList;
    P_LINK_ENTRY_T      prLinkEntry;
    P_TIMER_T           prTimer;
    OS_SYSTIME          rCurSysTime;
    PFN_MGMT_TIMEOUT_FUNC   pfMgmtTimeOutFunc;
    UINT_32             u4TimeoutData;
    KAL_SPIN_LOCK_DECLARATION();

    //DEBUGFUNC("cnmTimerDoTimeOutCheck");

    ASSERT(prAdapter);

    /* acquire spin lock */
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

    prTimerList = &(prAdapter->rRootTimer.rLinkHead);

    rCurSysTime = kalGetTimeTick();

#if defined (WCN_MAUI)
    /* Set the permitted max timeout value for new one */
    prAdapter->rRootTimer.rNextExpiredSysTime = MGMT_MAX_TIMEOUT_INTERVAL;

    LINK_FOR_EACH(prLinkEntry, prTimerList) {
        prTimer = LINK_ENTRY(prLinkEntry, TIMER_T, rLinkEntry);
        ASSERT(prTimer);

        //DBGLOG(CNM, INFO, ("rCurSysTime= %d, prTimer->rExpiredSysTime= %d\n", rCurSysTime, prTimer->rExpiredSysTime));

        /* Check if this entry is timeout. */
        if (!TIME_BEFORE(rCurSysTime, prTimer->rExpiredSysTime)) {
            cnmTimerStopTimer_impl(prAdapter, prTimer, FALSE);

            pfMgmtTimeOutFunc = prTimer->pfMgmtTimeOutFunc;
            u4TimeoutData = prTimer->u4Data;

            if (pfMgmtTimeOutFunc) {
                KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
                (pfMgmtTimeOutFunc)(prAdapter, u4TimeoutData);
                KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
            }

            /* Search entire list again because of nest del and add timers
                    * and current MGMT_TIMER could be volatile after stopped
                    */
            prLinkEntry = (P_LINK_ENTRY_T)prTimerList;

            prAdapter->rRootTimer.rNextExpiredSysTime = MGMT_MAX_TIMEOUT_INTERVAL;
        } else {
            /* Find the next timeout timer */
            if (TIME_BEFORE(prTimer->rExpiredSysTime,
                            prAdapter->rRootTimer.rNextExpiredSysTime)) {
                prAdapter->rRootTimer.rNextExpiredSysTime = prTimer->rExpiredSysTime;
            }
        }
    } /* end of for loop */
#else
    /* Set the permitted max timeout value for new one */
    prAdapter->rRootTimer.rNextExpiredSysTime = rCurSysTime + MGMT_MAX_TIMEOUT_INTERVAL;

    LINK_FOR_EACH(prLinkEntry, prTimerList) {
        prTimer = LINK_ENTRY(prLinkEntry, TIMER_T, rLinkEntry);
        ASSERT(prTimer);

        /* Check if this entry is timeout. */
        if (!TIME_BEFORE(rCurSysTime, prTimer->rExpiredSysTime)) {
            cnmTimerStopTimer_impl(prAdapter, prTimer, FALSE);

            pfMgmtTimeOutFunc = prTimer->pfMgmtTimeOutFunc;
            u4TimeoutData = prTimer->u4Data;

            if (prTimer->u2Minutes > 0) {
                prTimer->u2Minutes--;
                prTimer->rExpiredSysTime =
                    rCurSysTime + MSEC_TO_SYSTIME(MSEC_PER_MIN);
                LINK_INSERT_TAIL(prTimerList, &prTimer->rLinkEntry);
            } else if (pfMgmtTimeOutFunc) {
                KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
                (pfMgmtTimeOutFunc)(prAdapter, u4TimeoutData);
                KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
            }

            /* Search entire list again because of nest del and add timers
             * and current MGMT_TIMER could be volatile after stopped
             */
            prLinkEntry = (P_LINK_ENTRY_T)prTimerList;

            prAdapter->rRootTimer.rNextExpiredSysTime =
                rCurSysTime + MGMT_MAX_TIMEOUT_INTERVAL;
        } else if (TIME_BEFORE(prTimer->rExpiredSysTime,
                               prAdapter->rRootTimer.rNextExpiredSysTime)) {
            prAdapter->rRootTimer.rNextExpiredSysTime = prTimer->rExpiredSysTime;
        }
    } /* end of for loop */
#endif

    /* Setup the prNext timeout event. It is possible the timer was already
     * set in the above timeout callback function.
     */
    if (!LINK_IS_EMPTY(prTimerList)) {
        ASSERT(TIME_AFTER(prAdapter->rRootTimer.rNextExpiredSysTime, rCurSysTime));

        cnmTimerSetTimer(prAdapter,
                         (OS_SYSTIME)((INT_32)prAdapter->rRootTimer.rNextExpiredSysTime - (INT_32)rCurSysTime));
    }

    /* release spin lock */
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
}


