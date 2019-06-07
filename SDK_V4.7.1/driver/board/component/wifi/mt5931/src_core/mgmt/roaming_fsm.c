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
** $Id:
*/

/*! \file   "roaming_fsm.c"
    \brief  This file defines the FSM for Roaming MODULE.

    This file defines the FSM for Roaming MODULE.
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

#if CFG_SUPPORT_ROAMING
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

#if 0
/*lint -save -e64 Type mismatch */
static PUINT_8 apucDebugRoamingState[ROAMING_STATE_NUM] = {
    (PUINT_8)DISP_STRING("ROAMING_STATE_IDLE"),
    (PUINT_8)DISP_STRING("ROAMING_STATE_DECISION"),
    (PUINT_8)DISP_STRING("ROAMING_STATE_DISCOVERY"),
    (PUINT_8)DISP_STRING("ROAMING_STATE_ROAM")
};
/*lint -restore */
#endif /* DBG */

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

#define ROAMING_ENABLE_CHECK(_roam) \
        { \
            if (!(_roam->fgIsEnableRoaming)) {return;} \
        }

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
* @brief Initialize the value in ROAMING_FSM_INFO_T for ROAMING FSM operation
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmInit(
    IN P_ADAPTER_T prAdapter
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "->roamingFsmInit()");

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

    //4 <1> Initiate FSM
    prRoamingFsmInfo->fgIsEnableRoaming = prConnSettings->fgIsEnableRoaming;
    prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;
    prRoamingFsmInfo->rRoamingDiscoveryUpdateTime = 0;
    prRoamingFsmInfo->fgBriefEnableRoamingDynamically = KAL_TRUE;

    return;
} /* end of roamingFsmInit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Uninitialize the value in AIS_FSM_INFO_T for AIS FSM operation
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmUninit(
    IN P_ADAPTER_T prAdapter
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DEINIT, "->roamingFsmUninit()");

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;

    prRoamingFsmInfo->fgBriefEnableRoamingDynamically = KAL_TRUE;

    return;
} /* end of roamingFsmUninit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Send commands to firmware
*
* @param [IN P_ADAPTER_T]       prAdapter
*        [IN P_ROAMING_PARAM_T] prParam
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmSendCmd(
    IN P_ADAPTER_T prAdapter,
    IN P_ROAMING_PARAM_T prParam
)
{
    //P_ROAMING_INFO_T prRoamingFsmInfo;
    WLAN_STATUS rStatus;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "->roamingFsmSendCmd(): Current Time = %ld\n", kalGetTimeTick());

    //prRoamingFsmInfo = (P_ROAMING_INFO_T)&(prAdapter->rWifiVar.rRoamingInfo);

    rStatus = wlanSendSetQueryCmd(
                  prAdapter,                  /* prAdapter */
                  CMD_ID_ROAMING_TRANSIT,     /* ucCID */
                  TRUE,                       /* fgSetQuery */
                  FALSE,                      /* fgNeedResp */
                  FALSE,                      /* fgIsOid */
                  NULL,                       /* pfCmdDoneHandler */
                  NULL,                       /* pfCmdTimeoutHandler */
                  sizeof(ROAMING_PARAM_T),    /* u4SetQueryInfoLen */
                  (PUINT_8) prParam,          /* pucInfoBuffer */
                  NULL,                       /* pvSetQueryBuffer */
                  0                           /* u4SetQueryBufferLen */
              );

    ASSERT(rStatus == WLAN_STATUS_PENDING);

    return;
} /* end of roamingFsmSendCmd() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Update the recent time when ScanDone occurred
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmScanResultsUpdate(
    IN P_ADAPTER_T prAdapter
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    /* Check Roaming Conditions */
    ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "->roamingFsmScanResultsUpdate(): Current Time = %ld\n", kalGetTimeTick());

    GET_CURRENT_SYSTIME(&prRoamingFsmInfo->rRoamingDiscoveryUpdateTime);

    return;
} /* end of roamingFsmScanResultsUpdate() */

/*----------------------------------------------------------------------------*/
/*!
* @brief The Core FSM engine of ROAMING for AIS Infra.
*
* @param [IN P_ADAPTER_T]          prAdapter
*        [IN ENUM_ROAMING_STATE_T] eNextState Enum value of next AIS STATE
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmSteps(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_ROAMING_STATE_T eNextState
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    //ENUM_ROAMING_STATE_T ePreviousState;
    BOOLEAN fgIsTransition = (BOOLEAN)FALSE;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    do {
#if 0
        /* Do entering Next State */
        DBGLOG(ROAMING, STATE, ("TRANSITION: [%s] -> [%s]\n",
                                apucDebugRoamingState[prRoamingFsmInfo->eCurrentState],
                                apucDebugRoamingState[eNextState]));
#endif
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_STATE, "ROAMING_STATE_TRANSITION: %d -> %d",
                         prRoamingFsmInfo->eCurrentState, eNextState);
        /* NOTE(Kevin): This is the only place to change the eCurrentState(except initial) */
        //ePreviousState = prRoamingFsmInfo->eCurrentState;
        prRoamingFsmInfo->eCurrentState = eNextState;

        fgIsTransition = (BOOLEAN)FALSE;

        /* Do tasks of the State that we just entered */
        switch (prRoamingFsmInfo->eCurrentState) {
            /* NOTE(Kevin): we don't have to rearrange the sequence of following
             * switch case. Instead I would like to use a common lookup table of array
             * of function pointer to speed up state search.
             */
            case ROAMING_STATE_IDLE:
            case ROAMING_STATE_DECISION:
                break;

            case ROAMING_STATE_DISCOVERY: {
                OS_SYSTIME rCurrentTime;

                GET_CURRENT_SYSTIME(&rCurrentTime);
                if (CHECK_FOR_TIMEOUT(rCurrentTime, prRoamingFsmInfo->rRoamingDiscoveryUpdateTime,
                                      SEC_TO_SYSTIME(ROAMING_DISCOVERY_TIMEOUT_SEC))) {
                    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "roamingFsmSteps: DiscoveryUpdateTime Timeout");
                    aisFsmRunEventRoamingDiscovery(prAdapter, TRUE);
                } else {
                    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "roamingFsmSteps: DiscoveryUpdateTime Updated");
                    aisFsmRunEventRoamingDiscovery(prAdapter, FALSE);
                }
            }
            break;

            case ROAMING_STATE_ROAM:
                break;

            default:
                ASSERT(0); /* Make sure we have handle all STATEs */
        }
    } while (fgIsTransition);

    return;

} /* end of roamingFsmSteps() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Decision state after join completion
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmRunEventStart(
    IN P_ADAPTER_T prAdapter
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    ENUM_ROAMING_STATE_T eNextState;
    P_BSS_INFO_T prAisBssInfo;
    ROAMING_PARAM_T rParam;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    /* Check Roaming Conditions */
    ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    if (prAisBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE) {
        return;
    }

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "EVENT-ROAMING START: Current Time = %ld\n", kalGetTimeTick());

    /* IDLE, ROAM -> DECISION */
    /* Errors as DECISION, DISCOVERY -> DECISION */
    if (!(prRoamingFsmInfo->eCurrentState == ROAMING_STATE_IDLE
            || prRoamingFsmInfo->eCurrentState == ROAMING_STATE_ROAM)) {
        return;
    }

    eNextState = ROAMING_STATE_DECISION;
    if (eNextState != prRoamingFsmInfo->eCurrentState) {
        rParam.u2Event = ROAMING_EVENT_START;
        roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) &rParam);

        /* Step to next state */
        roamingFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of roamingFsmRunEventStart() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Discovery state when deciding to find a candidate
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmRunEventDiscovery(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4Param
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    ENUM_ROAMING_STATE_T eNextState;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    /* Check Roaming Conditions */
    ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "EVENT-ROAMING DISCOVERY: Current Time = %ld\n", kalGetTimeTick());

    /* DECISION -> DISCOVERY */
    /* Errors as IDLE, DISCOVERY, ROAM -> DISCOVERY */
    if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DECISION) {
        return;
    }

    eNextState = ROAMING_STATE_DISCOVERY;
    /* DECISION -> DISCOVERY */
    if (eNextState != prRoamingFsmInfo->eCurrentState) {
        P_BSS_INFO_T prAisBssInfo;
        P_BSS_DESC_T prBssDesc;

        // sync. rcpi with firmware
        prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
        prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
        if (prBssDesc) {

            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "EVENT-ROAMING DISCOVERY: FW RCPI =%d\n", (UINT_8)(u4Param & 0xff));
            prBssDesc->ucRCPI = (UINT_8)(u4Param & 0xff);
        }

        roamingFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of roamingFsmRunEventDiscovery() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Roam state after Scan Done
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmRunEventRoam(
    IN P_ADAPTER_T prAdapter
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    ENUM_ROAMING_STATE_T eNextState;
    ROAMING_PARAM_T rParam;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    /* Check Roaming Conditions */
    ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "EVENT-ROAMING ROAM: Current Time = %ld\n", kalGetTimeTick());

    /* IDLE, ROAM -> DECISION */
    /* Errors as IDLE, DECISION, ROAM -> ROAM */
    if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DISCOVERY) {
        return;
    }

    eNextState = ROAMING_STATE_ROAM;
    /* DISCOVERY -> ROAM */
    if (eNextState != prRoamingFsmInfo->eCurrentState) {
        rParam.u2Event = ROAMING_EVENT_ROAM;
        roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) &rParam);

        /* Step to next state */
        roamingFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of roamingFsmRunEventRoam() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Decision state as being failed to find out any candidate
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmRunEventFail(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4Param
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    ENUM_ROAMING_STATE_T eNextState;
    ROAMING_PARAM_T rParam;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    /* Check Roaming Conditions */
    ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "EVENT-ROAMING FAIL: reason %x Current Time = %ld\n", u4Param, kalGetTimeTick());

    /* IDLE, ROAM -> DECISION */
    /* Errors as IDLE, DECISION, DISCOVERY -> DECISION */
    if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_ROAM) {
        return;
    }

    eNextState = ROAMING_STATE_DECISION;
    /* ROAM -> DECISION */
    if (eNextState != prRoamingFsmInfo->eCurrentState) {
        rParam.u2Event = ROAMING_EVENT_FAIL;
        rParam.u2Data = (UINT_16)(u4Param & 0xffff);
        roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) &rParam);

        /* Step to next state */
        roamingFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of roamingFsmRunEventFail() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Idle state as beging aborted by other moduels, AIS
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
roamingFsmRunEventAbort(
    IN P_ADAPTER_T prAdapter
)
{
    P_ROAMING_INFO_T prRoamingFsmInfo;
    ENUM_ROAMING_STATE_T eNextState;
    ROAMING_PARAM_T rParam;

    prRoamingFsmInfo = (P_ROAMING_INFO_T) & (prAdapter->rWifiVar.rRoamingInfo);

    ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

    DBGLOG(ROAMING, EVENT, ("EVENT-ROAMING ABORT: Current Time = %ld\n", kalGetTimeTick()));

    eNextState = ROAMING_STATE_IDLE;
    /* IDLE, DECISION, DISCOVERY, ROAM -> IDLE */
    if (eNextState != prRoamingFsmInfo->eCurrentState) {
        rParam.u2Event = ROAMING_EVENT_ABORT;
        roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) &rParam);

        /* Step to next state */
        roamingFsmSteps(prAdapter, eNextState);
    }

    return;
} /* end of roamingFsmRunEventAbort() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Process events from firmware
*
* @param [IN P_ADAPTER_T]       prAdapter
*        [IN P_ROAMING_PARAM_T] prParam
*
* @return none
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
roamingFsmProcessEvent(
    IN P_ADAPTER_T prAdapter,
    IN P_ROAMING_PARAM_T prParam
)
{
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ROAMING, "Receive FW ROAMING Events: Current Time = %ld\n", kalGetTimeTick());

    if (ROAMING_EVENT_DISCOVERY == prParam->u2Event) {
        roamingFsmRunEventDiscovery(prAdapter, prParam->u2Data);
    }

    return WLAN_STATUS_SUCCESS;
}

#endif
