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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/scan_fsm.c#27 $
*/

/*! \file   "scan_fsm.c"
    \brief  This file defines the state transition function for SCAN FSM.

    The SCAN FSM is part of SCAN MODULE and responsible for performing basic SCAN
    behavior as metioned in IEEE 802.11 2007 11.1.3.1 & 11.1.3.2 .
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
#if 0
/*lint -save -e64 Type mismatch */
static PUINT_8 apucDebugScanState[SCAN_STATE_NUM] = {
    (PUINT_8)DISP_STRING("SCAN_STATE_IDLE"),
    (PUINT_8)DISP_STRING("SCAN_STATE_SCANNING"),
};
/*lint -restore */
#endif /* DBG */

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
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
scnFsmSteps(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_SCAN_STATE_T eNextState
)
{
    P_SCAN_INFO_T prScanInfo;
    P_SCAN_PARAM_T prScanParam;
    CMD_SCAN_REQ rCmdScanReq;
    P_MSG_SCN_SCAN_REQ prScanReqMsg;

    UINT_32 i;

    BOOLEAN fgIsTransition = (BOOLEAN)FALSE;

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prScanParam = &prScanInfo->rScanParam;

    do {
#if 0
        DBGLOG(SCN, STATE, ("TRANSITION: [%s] -> [%s]\n",
                            apucDebugScanState[prScanInfo->eCurrentState],
                            apucDebugScanState[eNextState]));
#endif
        MT5931_TRACE(TRACE_STATE, MT5931_INFO_147, "SCAN TRANSITION: %d -> %d",
                     prScanInfo->eCurrentState, eNextState);

        /* NOTE(Kevin): This is the only place to change the eCurrentState(except initial) */
        prScanInfo->eCurrentState = eNextState;

        fgIsTransition = (BOOLEAN)FALSE;

        switch (prScanInfo->eCurrentState) {
            case SCAN_STATE_IDLE:
                /* check for pending scanning requests */
                if (!LINK_IS_EMPTY(&(prScanInfo->rPendingMsgList))) {
                    // load next message from pending list as scan parameters
                    LINK_REMOVE_HEAD(&(prScanInfo->rPendingMsgList), prScanReqMsg, P_MSG_SCN_SCAN_REQ);

                    prScanParam->eScanType      = prScanReqMsg->eScanType;
                    prScanParam->eNetTypeIndex  = (ENUM_NETWORK_TYPE_INDEX_T)prScanReqMsg->ucNetTypeIndex;
                    prScanParam->ucSSIDType     = prScanReqMsg->ucSSIDType;
                    if (prScanParam->ucSSIDType & (SCAN_REQ_SSID_SPECIFIED | SCAN_REQ_SSID_P2P_WILDCARD)) {
                        COPY_SSID(prScanParam->aucSpecifiedSSID,
                                  prScanParam->ucSpecifiedSSIDLen,
                                  prScanReqMsg->aucSSID,
                                  prScanReqMsg->ucSSIDLength);
                    }

                    prScanParam->eScanChannel   = prScanReqMsg->eScanChannel;
                    if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
                        if (prScanReqMsg->ucChannelListNum <= MAXIMUM_OPERATION_CHANNEL_LIST) {
                            prScanParam->ucChannelListNum   = prScanReqMsg->ucChannelListNum;
                        } else {
                            prScanParam->ucChannelListNum   = MAXIMUM_OPERATION_CHANNEL_LIST;
                        }

                        kalMemCopy(prScanParam->arChnlInfoList,
                                   prScanReqMsg->arChnlInfoList,
                                   sizeof(RF_CHANNEL_INFO_T) * prScanParam->ucChannelListNum);
                    }

                    if (prScanReqMsg->u2IELen <= MAX_IE_LENGTH) {
                        prScanParam->u2IELen    = prScanReqMsg->u2IELen;
                    } else {
                        prScanParam->u2IELen    = MAX_IE_LENGTH;
                    }

                    if (prScanParam->u2IELen) {
                        kalMemCopy(prScanParam->aucIE, prScanReqMsg->aucIE, prScanParam->u2IELen);
                    }

#if CFG_ENABLE_WIFI_DIRECT
                    if (prAdapter->fgIsP2PRegistered) {
                        prScanParam->u2PassiveListenInterval = prScanReqMsg->u2ChannelDwellTime;
                    }
#endif
                    prScanParam->ucSeqNum       = prScanReqMsg->ucSeqNum;

                    if (prScanReqMsg->rMsgHdr.eMsgId == MID_RLM_SCN_SCAN_REQ) {
                        prScanParam->fgIsObssScan   = TRUE;
                    } else {
                        prScanParam->fgIsObssScan   = FALSE;
                    }

                    /* switch to next state */
                    eNextState = SCAN_STATE_SCANNING;
                    fgIsTransition = TRUE;

                    cnmMemFree(prAdapter, prScanReqMsg);
                }
                break;

            case SCAN_STATE_SCANNING:
                // send command packet for scan
                kalMemZero(&rCmdScanReq, sizeof(CMD_SCAN_REQ));

                rCmdScanReq.ucSeqNum        = prScanParam->ucSeqNum;
                rCmdScanReq.ucNetworkType   = (UINT_8)prScanParam->eNetTypeIndex;
                rCmdScanReq.ucScanType      = (UINT_8)prScanParam->eScanType;
                rCmdScanReq.ucSSIDType      = prScanParam->ucSSIDType;
                if (prScanParam->ucSSIDType & (SCAN_REQ_SSID_SPECIFIED | SCAN_REQ_SSID_P2P_WILDCARD)) {
                    COPY_SSID(rCmdScanReq.aucSSID,
                              rCmdScanReq.ucSSIDLength,
                              prScanParam->aucSpecifiedSSID,
                              prScanParam->ucSpecifiedSSIDLen);
                }

                rCmdScanReq.ucChannelType   = (UINT_8)prScanParam->eScanChannel;

                if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
                    /* P2P would use:
                      * 1. Specified Listen Channel of passive scan for LISTEN state.
                      * 2. Specified Listen Channel of Target Device of active scan for SEARCH state. (Target != NULL)
                      */
                    rCmdScanReq.ucChannelListNum    = prScanParam->ucChannelListNum;

                    for (i = 0 ; i < rCmdScanReq.ucChannelListNum ; i++) {
                        rCmdScanReq.arChannelList[i].ucBand =
                            (UINT_8) prScanParam->arChnlInfoList[i].eBand;

                        rCmdScanReq.arChannelList[i].ucChannelNum =
                            (UINT_8)prScanParam->arChnlInfoList[i].ucChannelNum;
                    }
                }

#if CFG_ENABLE_WIFI_DIRECT
                if (prAdapter->fgIsP2PRegistered) {
                    rCmdScanReq.u2ChannelDwellTime = prScanParam->u2PassiveListenInterval;
                }
#endif
                if (prScanParam->eNetTypeIndex == NETWORK_TYPE_AIS_INDEX) { //set dwell time
                    if (prScanInfo->u2ChannelDwellTime >= 255) { //max is 255 in FW
                        prScanInfo->u2ChannelDwellTime = 255;
                    }

                    rCmdScanReq.u2ChannelDwellTime = prScanInfo->u2ChannelDwellTime;

                    if (rCmdScanReq.ucScanType) { //active scan, set  same as u2ChannelDwellTime
                        rCmdScanReq.u2ChannelMinDwellTime = prScanInfo->u2ChannelDwellTime;
                    }
                }

                if (prScanParam->u2IELen <= MAX_IE_LENGTH) {
                    rCmdScanReq.u2IELen = prScanParam->u2IELen;
                } else {
                    rCmdScanReq.u2IELen = MAX_IE_LENGTH;
                }

                if (prScanParam->u2IELen) {
                    kalMemCopy(rCmdScanReq.aucIE,
                               prScanParam->aucIE,
                               sizeof(UINT_8) * rCmdScanReq.u2IELen);
                }
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_SCAN, "u2ChannelMinDwellTime%d", rCmdScanReq.u2ChannelMinDwellTime);
                MT5931_TRACE(TRACE_SCAN, MT5931_INFO_148, "[CMD_ID_SCAN_REQ]ucSeqNum %d ucNetworkType %d ucScanType %d ucSSIDType %d ucSSIDLength %d ",
                             rCmdScanReq.ucSeqNum, rCmdScanReq.ucNetworkType, rCmdScanReq.ucScanType, rCmdScanReq.ucSSIDType, rCmdScanReq.ucSSIDLength);
                MT5931_TRACE(TRACE_SCAN, MT5931_INFO_149, "[CMD_ID_SCAN_REQ]u2ChannelDwellTime %d ucChannelType %d ucChannelListNum %d u2IELen %d",
                             rCmdScanReq.u2ChannelDwellTime, rCmdScanReq.ucChannelType, rCmdScanReq.ucChannelListNum, rCmdScanReq.u2IELen);
                wlanSendSetQueryCmd(prAdapter,
                                    CMD_ID_SCAN_REQ,
                                    TRUE,
                                    FALSE,
                                    FALSE,
                                    NULL,
                                    NULL,
                                    OFFSET_OF(CMD_SCAN_REQ, aucIE) + rCmdScanReq.u2IELen,
                                    (PUINT_8)&rCmdScanReq,
                                    NULL,
                                    0);
                break;

            default:
                ASSERT(0);
                break;

        }
    } while (fgIsTransition);

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
scnFsmMsgStart(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_SCN_SCAN_REQ prScanReqMsg;
    P_SCAN_INFO_T prScanInfo;
    P_SCAN_PARAM_T prScanParam;

    ASSERT(prMsgHdr);

    prScanReqMsg = (P_MSG_SCN_SCAN_REQ)prMsgHdr;
    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prScanParam = &prScanInfo->rScanParam;

    ASSERT(prScanReqMsg->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

    if (prScanInfo->eCurrentState == SCAN_STATE_IDLE) {
        /* Load scan parameters into SCAN_PARAM_T */
        prScanParam->eScanType      = prScanReqMsg->eScanType;
        prScanParam->eNetTypeIndex  = (ENUM_NETWORK_TYPE_INDEX_T)prScanReqMsg->ucNetTypeIndex;
        prScanParam->ucSSIDType     = prScanReqMsg->ucSSIDType;
        if (prScanParam->ucSSIDType & (SCAN_REQ_SSID_SPECIFIED | SCAN_REQ_SSID_P2P_WILDCARD)) {
            COPY_SSID(prScanParam->aucSpecifiedSSID,
                      prScanParam->ucSpecifiedSSIDLen,
                      prScanReqMsg->aucSSID,
                      prScanReqMsg->ucSSIDLength);
        }

        prScanParam->eScanChannel   = prScanReqMsg->eScanChannel;
        if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
            if (prScanReqMsg->ucChannelListNum <= MAXIMUM_OPERATION_CHANNEL_LIST) {
                prScanParam->ucChannelListNum   = prScanReqMsg->ucChannelListNum;
            } else {
                prScanParam->ucChannelListNum   = MAXIMUM_OPERATION_CHANNEL_LIST;
            }

            kalMemCopy(prScanParam->arChnlInfoList,
                       prScanReqMsg->arChnlInfoList,
                       sizeof(RF_CHANNEL_INFO_T) * prScanParam->ucChannelListNum);
        }

        prScanParam->u2IELen        = prScanReqMsg->u2IELen;
        if (prScanParam->u2IELen) {
            kalMemCopy(prScanParam->aucIE, prScanReqMsg->aucIE, prScanParam->u2IELen);
        }

#if CFG_ENABLE_WIFI_DIRECT
        if (prAdapter->fgIsP2PRegistered) {
            prScanParam->u2PassiveListenInterval = prScanReqMsg->u2ChannelDwellTime;
        }
#endif

        prScanParam->ucSeqNum       = prScanReqMsg->ucSeqNum;

        if (prScanReqMsg->rMsgHdr.eMsgId == MID_RLM_SCN_SCAN_REQ) {
            prScanParam->fgIsObssScan   = TRUE;
        } else {
            prScanParam->fgIsObssScan   = FALSE;
        }

        scnFsmSteps(prAdapter, SCAN_STATE_SCANNING);

        cnmMemFree(prAdapter, prMsgHdr);
    } else {
        LINK_INSERT_TAIL(&prScanInfo->rPendingMsgList, &prMsgHdr->rLinkEntry);
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_150, "scnFsmMsgStart: scan pending list %d",
                     prScanInfo->rPendingMsgList.u4NumElem);
    }

    return;
}



/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
scnFsmMsgAbort(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_SCN_SCAN_CANCEL prScanCancel;
    P_SCAN_INFO_T prScanInfo;
    P_SCAN_PARAM_T prScanParam;
    CMD_SCAN_CANCEL rCmdScanCancel;

    ASSERT(prMsgHdr);

    prScanCancel = (P_MSG_SCN_SCAN_CANCEL)prMsgHdr;
    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prScanParam = &prScanInfo->rScanParam;

    if (prScanInfo->eCurrentState != SCAN_STATE_IDLE) {
        if (prScanCancel->ucSeqNum == prScanParam->ucSeqNum &&
                prScanCancel->ucNetTypeIndex == (UINT_8)prScanParam->eNetTypeIndex) {
            /* send cancel message to firmware domain */
            rCmdScanCancel.ucSeqNum = prScanParam->ucSeqNum;

#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                rCmdScanCancel.ucIsExtChannel = (UINT_8) prScanCancel->fgIsChannelExt;
            } else {
                rCmdScanCancel.ucIsExtChannel = (UINT_8) FALSE;
            }

#endif
            MT5931_TRACE(TRACE_SCAN, MT5931_INFO_151, "[CMD_ID_SCAN_CANCEL]ucSeqNum %d ucIsExtChannel %d",
                         rCmdScanCancel.ucSeqNum, rCmdScanCancel.ucIsExtChannel);

            wlanSendSetQueryCmd(prAdapter,
                                CMD_ID_SCAN_CANCEL,
                                TRUE,
                                FALSE,
                                FALSE,
                                NULL,
                                NULL,
                                sizeof(CMD_SCAN_CANCEL),
                                (PUINT_8)&rCmdScanCancel,
                                NULL,
                                0);

            /* generate scan-done event for caller */
            scnFsmGenerateScanDoneMsg(prAdapter,
                                      prScanParam->ucSeqNum,
                                      (UINT_8)prScanParam->eNetTypeIndex,
                                      SCAN_STATUS_CANCELLED);

            /* switch to next pending scan */
            scnFsmSteps(prAdapter, SCAN_STATE_IDLE);
        } else {
            P_MSG_HDR_T prPendingMsgHdr, prPendingMsgHdrNext;

            /* traverse through rPendingMsgList for removal */
            LINK_FOR_EACH_ENTRY_SAFE(prPendingMsgHdr,
                                     prPendingMsgHdrNext,
                                     &(prScanInfo->rPendingMsgList),
                                     rLinkEntry,
                                     MSG_HDR_T) {
                P_MSG_SCN_SCAN_REQ prScanReqMsg = (P_MSG_SCN_SCAN_REQ)prPendingMsgHdr;

                if (prScanCancel->ucSeqNum == prScanReqMsg->ucSeqNum &&
                        prScanCancel->ucNetTypeIndex == prScanReqMsg->ucNetTypeIndex) {
                    /* generate scan-done event for caller */
                    scnFsmGenerateScanDoneMsg(prAdapter,
                                              prScanReqMsg->ucSeqNum,
                                              prScanReqMsg->ucNetTypeIndex,
                                              SCAN_STATUS_CANCELLED);

                    /* remove from pending list */
                    LINK_REMOVE_KNOWN_ENTRY(&(prScanInfo->rPendingMsgList), &(prScanReqMsg->rMsgHdr.rLinkEntry));

                    cnmMemFree(prAdapter, prScanReqMsg);

                    break;
                }
            }
        }
    }

    cnmMemFree(prAdapter, prMsgHdr);

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
scnEventScanDone(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucSeqNum
)
{
    P_SCAN_INFO_T prScanInfo;
    P_SCAN_PARAM_T prScanParam;

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prScanParam = &prScanInfo->rScanParam;

    if (prScanInfo->eCurrentState == SCAN_STATE_SCANNING &&
            ucSeqNum == prScanParam->ucSeqNum) {
        /* generate scan-done event for caller */
        scnFsmGenerateScanDoneMsg(prAdapter,
                                  prScanParam->ucSeqNum,
                                  (UINT_8)prScanParam->eNetTypeIndex,
                                  SCAN_STATUS_DONE);

        /* switch to next pending scan */
        scnFsmSteps(prAdapter, SCAN_STATE_IDLE);
    } else {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "Unexpected SCAN-DONE event: SeqNum = %d, Current State = %d\n",
                         ucSeqNum,
                         prScanInfo->eCurrentState);
    }

    return;
} /* end of scnEventScanDone */


/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
scnFsmGenerateScanDoneMsg(
    IN P_ADAPTER_T          prAdapter,
    IN UINT_8               ucSeqNum,
    IN UINT_8               ucNetTypeIndex,
    IN ENUM_SCAN_STATUS     eScanStatus
)
{
    P_SCAN_INFO_T prScanInfo;
    P_SCAN_PARAM_T prScanParam;
    P_MSG_SCN_SCAN_DONE prScanDoneMsg;

    ASSERT(prAdapter);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prScanParam = &prScanInfo->rScanParam;

    prScanDoneMsg = (P_MSG_SCN_SCAN_DONE)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SCN_SCAN_DONE));
    if (!prScanDoneMsg) {
        ASSERT(0); // Can't indicate SCAN FSM Complete
        return;
    }

    if (prScanParam->fgIsObssScan == TRUE) {
        prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_RLM_SCAN_DONE;
    } else {
        switch ((ENUM_NETWORK_TYPE_INDEX_T)ucNetTypeIndex) {
            case NETWORK_TYPE_AIS_INDEX:
                prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_AIS_SCAN_DONE;
                break;

#if CFG_ENABLE_WIFI_DIRECT
            case NETWORK_TYPE_P2P_INDEX:
                if (prAdapter->fgIsP2PRegistered) {
                    prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_P2P_SCAN_DONE;
                } else {
                    DBGLOG(SCN, LOUD, ("Unexpected Network Type: %d\n", ucNetTypeIndex));
                    ASSERT(0);
                }
                break;
#endif

#if CFG_ENABLE_BT_OVER_WIFI
            case NETWORK_TYPE_BOW_INDEX:
                prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_BOW_SCAN_DONE;
                break;
#endif

            default:
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Unexpected Network Type: %d\n", ucNetTypeIndex);
                ASSERT(0);
                break;
        }
    }

    prScanDoneMsg->ucSeqNum         = ucSeqNum;
    prScanDoneMsg->ucNetTypeIndex   = ucNetTypeIndex;
    prScanDoneMsg->eScanStatus      = eScanStatus;

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prScanDoneMsg,
                MSG_SEND_METHOD_BUF);

} /* end of scnFsmGenerateScanDoneMsg() */


