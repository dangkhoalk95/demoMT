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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/cnm.c#14 $
*/

/*! \file   "cnm.c"
    \brief  Module of Concurrent Network Management

    Module of Concurrent Network Management
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
* @brief This function is used to initialize variables in CNM_INFO_T.
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmInit(
    P_ADAPTER_T     prAdapter
)
{
    return;
} /* end of cnmInit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used to initialize variables in CNM_INFO_T.
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmUninit(
    P_ADAPTER_T     prAdapter
)
{
    return;
} /* end of cnmUninit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Before handle the message from other module, it need to obtain
*        the Channel privilege from Channel Manager
*
* @param[in] prMsgHdr   The message need to be handled.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmChMngrRequestPrivilege(
    P_ADAPTER_T     prAdapter,
    P_MSG_HDR_T     prMsgHdr
)
{
    P_MSG_CH_REQ_T          prMsgChReq;
    P_CMD_CH_PRIVILEGE_T    prCmdBody;
    WLAN_STATUS             rStatus;

    ASSERT(prAdapter);
    ASSERT(prMsgHdr);
    prCmdBody = (P_CMD_CH_PRIVILEGE_T)
                cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_CH_PRIVILEGE_T));
//    ASSERT(prCmdBody);

    /* To do: exception handle */
    //if ((prAdapter->rWifiVar.rAisFsmInfo.fgIsChannelRequested == TRUE)||(!prCmdBody)) {
    if (!prCmdBody) {
        MT5931_TRACE(TRACE_ERROR, MT5931_INFO_41, "fgIsChannelRequested %d", prAdapter->rWifiVar.rAisFsmInfo.fgIsChannelRequested);
        cnmMemFree(prAdapter, prMsgHdr);
        return;
    }

    prMsgChReq = (P_MSG_CH_REQ_T) prMsgHdr;

    prCmdBody->ucNetTypeIndex = prMsgChReq->ucNetTypeIndex;
    prCmdBody->ucTokenID = prMsgChReq->ucTokenID;
    prCmdBody->ucAction = CMD_CH_ACTION_REQ;    /* Request */
    prCmdBody->ucPrimaryChannel = prMsgChReq->ucPrimaryChannel;
    prCmdBody->ucRfSco = (UINT_8) prMsgChReq->eRfSco;
    prCmdBody->ucRfBand = (UINT_8) prMsgChReq->eRfBand;
    prCmdBody->ucReqType = (UINT_8) prMsgChReq->eReqType;
    prCmdBody->ucReserved = 0;
    prCmdBody->u4MaxInterval = prMsgChReq->u4MaxInterval;
    COPY_MAC_ADDR(prCmdBody->aucBSSID, prMsgChReq->aucBSSID);

    rStatus = wlanSendSetQueryCmd(
                  prAdapter,                  /* prAdapter */
                  CMD_ID_CH_PRIVILEGE,        /* ucCID */
                  TRUE,                       /* fgSetQuery */
                  FALSE,                      /* fgNeedResp */
                  FALSE,                      /* fgIsOid */
                  NULL,                       /* pfCmdDoneHandler */
                  NULL,                       /* pfCmdTimeoutHandler */
                  sizeof(CMD_CH_PRIVILEGE_T), /* u4SetQueryInfoLen */
                  (PUINT_8) prCmdBody,        /* pucInfoBuffer */
                  NULL,                       /* pvSetQueryBuffer */
                  0                           /* u4SetQueryBufferLen */
              );

    MT5931_TRACE(TRACE_CONN, MT5931_INFO_42, "[CMD_ID_CH_PRIVILEGE]ucNetTypeIndex %d ucTokenID %d ucAction %d ucPrimaryChannel %d",
                 prCmdBody->ucNetTypeIndex, prCmdBody->ucTokenID, prCmdBody->ucAction, prCmdBody->ucPrimaryChannel);
    MT5931_TRACE(TRACE_CONN, MT5931_INFO_43, "[CMD_ID_CH_PRIVILEGE]ucRfSco %d ucRfBand %d ucReqType %d u4MaxInterval %d",
                 prCmdBody->ucRfSco, prCmdBody->ucRfBand, prCmdBody->ucReqType, prCmdBody->u4MaxInterval);
    MT5931_TRACE(TRACE_CONN, MT5931_INFO_44, "[CMD_ID_CH_PRIVILEGE]BSSID: "MACSTR, MAC2STR(prCmdBody->aucBSSID));

    ASSERT(rStatus == WLAN_STATUS_PENDING);

    cnmMemFree(prAdapter, prCmdBody);
    cnmMemFree(prAdapter, prMsgHdr);

    return;
} /* end of cnmChMngrRequestPrivilege() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Before deliver the message to other module, it need to release
*        the Channel privilege to Channel Manager.
*
* @param[in] prMsgHdr   The message need to be delivered
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmChMngrAbortPrivilege(
    P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
)
{
    P_MSG_CH_ABORT_T        prMsgChAbort;
    P_CMD_CH_PRIVILEGE_T    prCmdBody;
    WLAN_STATUS             rStatus;

    ASSERT(prAdapter);
    ASSERT(prMsgHdr);
    DEBUGFUNC("cnmChMngrAbortPrivilege()");
    prCmdBody = (P_CMD_CH_PRIVILEGE_T)
                cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_CH_PRIVILEGE_T));
    //ASSERT(prCmdBody);

    /* To do: exception handle */
    if (!prCmdBody) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "cnmChMngrAbortPrivilege(): cnmMemAlloc() fail");
        cnmMemFree(prAdapter, prMsgHdr);
        return;
    }

    prMsgChAbort = (P_MSG_CH_ABORT_T) prMsgHdr;

    prCmdBody->ucNetTypeIndex = prMsgChAbort->ucNetTypeIndex;
    prCmdBody->ucTokenID = prMsgChAbort->ucTokenID;
    prCmdBody->ucAction = CMD_CH_ACTION_ABORT;  /* Abort */

    rStatus = wlanSendSetQueryCmd(
                  prAdapter,                  /* prAdapter */
                  CMD_ID_CH_PRIVILEGE,        /* ucCID */
                  TRUE,                       /* fgSetQuery */
                  FALSE,                      /* fgNeedResp */
                  FALSE,                      /* fgIsOid */
                  NULL,                       /* pfCmdDoneHandler */
                  NULL,                       /* pfCmdTimeoutHandler */
                  sizeof(CMD_CH_PRIVILEGE_T), /* u4SetQueryInfoLen */
                  (PUINT_8) prCmdBody,        /* pucInfoBuffer */
                  NULL,                       /* pvSetQueryBuffer */
                  0                           /* u4SetQueryBufferLen */
              );
    MT5931_TRACE(TRACE_CONN, MT5931_INFO_45, "[CMD_ID_CH_PRIVILEGE]ucNetTypeIndex %d ucTokenID %d ucAction %d",
                 prCmdBody->ucNetTypeIndex, prCmdBody->ucTokenID, prCmdBody->ucAction);
    ASSERT(rStatus == WLAN_STATUS_PENDING);

    cnmMemFree(prAdapter, prCmdBody);
    cnmMemFree(prAdapter, prMsgHdr);

    return;
} /* end of cnmChMngrAbortPrivilege() */

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmChMngrHandleChEvent(
    P_ADAPTER_T     prAdapter,
    P_WIFI_EVENT_T  prEvent
)
{
    P_EVENT_CH_PRIVILEGE_T  prEventBody;
    P_MSG_CH_GRANT_T        prChResp;

    ASSERT(prAdapter);
    ASSERT(prEvent);
    prEventBody = (P_EVENT_CH_PRIVILEGE_T)(prEvent->aucBuffer);
    prChResp = (P_MSG_CH_GRANT_T)
               cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_CH_GRANT_T));
    ASSERT(prChResp);

    /* To do: exception handle */
    if (!prChResp) {
        return;
    }

    ASSERT(prEventBody->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);
    ASSERT(prEventBody->ucStatus == EVENT_CH_STATUS_GRANT);

    /* Decide message ID based on network and response status */
    if (prEventBody->ucNetTypeIndex == NETWORK_TYPE_AIS_INDEX) {
        prChResp->rMsgHdr.eMsgId = MID_CNM_AIS_CH_GRANT;
    }
#if CFG_ENABLE_WIFI_DIRECT
    else if ((prAdapter->fgIsP2PRegistered) &&
             (prEventBody->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX)) {
        prChResp->rMsgHdr.eMsgId = MID_CNM_P2P_CH_GRANT;
    }
#endif
#if CFG_ENABLE_BT_OVER_WIFI
    else if (prEventBody->ucNetTypeIndex == NETWORK_TYPE_BOW_INDEX) {
        prChResp->rMsgHdr.eMsgId = MID_CNM_BOW_CH_GRANT;
    }
#endif
#if CFG_ENABLE_WIFI_TETHERING
    else if ((prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable) &&
             (prEventBody->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX)) {
        prChResp->rMsgHdr.eMsgId = MID_CNM_P2P_CH_GRANT;
    }
#endif /* CFG_ENABLE_WIFI_TETHERING */

    prChResp->ucNetTypeIndex = prEventBody->ucNetTypeIndex;
    prChResp->ucTokenID = prEventBody->ucTokenID;
    prChResp->ucPrimaryChannel = prEventBody->ucPrimaryChannel;
    prChResp->eRfSco = (ENUM_CHNL_EXT_T) prEventBody->ucRfSco;
    prChResp->eRfBand = (ENUM_BAND_T) prEventBody->ucRfBand;
    prChResp->eReqType = (ENUM_CH_REQ_TYPE_T) prEventBody->ucReqType;
    prChResp->u4GrantInterval = prEventBody->u4GrantInterval;

    MT5931_TRACE(TRACE_CONN, MT5931_INFO_46, "[EVENT_ID_CH_PRIVILEGE]ucNetTypeIndex %d ucTokenID %d ucPrimaryChannel %d eRfSco %d eRfBand %d eReqType %d u4GrantInterval %d",
                 prChResp->ucNetTypeIndex, prChResp->ucTokenID, prChResp->ucPrimaryChannel, prChResp->eRfSco,
                 prChResp->eRfBand, prChResp->eReqType, prChResp->u4GrantInterval);

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prChResp,
                MSG_SEND_METHOD_BUF);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return TRUE: suggest to adopt the returned preferred channel
*         FALSE: No suggestion. Caller should adopt its preference
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
cnmPreferredChannel(
    P_ADAPTER_T         prAdapter,
    P_ENUM_BAND_T       prBand,
    PUINT_8             pucPrimaryChannel,
    P_ENUM_CHNL_EXT_T   prBssSCO
)
{
    P_BSS_INFO_T    prBssInfo;
    UINT_8          ucNetIdx;
    BOOLEAN         fgPrefer;

    ASSERT(prAdapter);
    ASSERT(prBand);
    ASSERT(pucPrimaryChannel);
    ASSERT(prBssSCO);

    fgPrefer = FALSE;

    RLM_NET_FOR_EACH(ucNetIdx) {
        prBssInfo = &prAdapter->rWifiVar.arBssInfo[ucNetIdx];

        if (RLM_NET_PARAM_VALID(prBssInfo)) {
            *prBand = prBssInfo->eBand;
            *pucPrimaryChannel = prBssInfo->ucPrimaryChannel;
            *prBssSCO = prBssInfo->eBssSCO;

            fgPrefer = TRUE;
            break;
        }
    }

    return fgPrefer;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return TRUE: available channel is limited to return value
*         FALSE: no limited
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
cnmAisInfraChannelFixed(
    P_ADAPTER_T         prAdapter,
    P_ENUM_BAND_T       prBand,
    PUINT_8             pucPrimaryChannel
)
{
#if CFG_ENABLE_WIFI_DIRECT ||(CFG_ENABLE_BT_OVER_WIFI && CFG_BOW_LIMIT_AIS_CHNL)
    P_BSS_INFO_T    prBssInfo;
#endif

#if CFG_ENABLE_WIFI_DIRECT
    if (IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_P2P_INDEX) &&
            prAdapter->rP2pFuncLkr.prP2pFuncIsApMode &&
            prAdapter->rP2pFuncLkr.prP2pFuncIsApMode(
                prAdapter->rWifiVar.prP2pFsmInfo)) {

        ASSERT(prAdapter->fgIsP2PRegistered);

        prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX];

        *prBand = prBssInfo->eBand;
        *pucPrimaryChannel = prBssInfo->ucPrimaryChannel;

        return TRUE;
    }
#endif

#if CFG_ENABLE_BT_OVER_WIFI && CFG_BOW_LIMIT_AIS_CHNL
    if (IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_BOW_INDEX)) {

        prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX];

        *prBand = prBssInfo->eBand;
        *pucPrimaryChannel = prBssInfo->ucPrimaryChannel;

        return TRUE;
    }
#endif

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmAisInfraConnectNotify(
    P_ADAPTER_T         prAdapter
)
{
#if CFG_ENABLE_BT_OVER_WIFI
    P_BSS_INFO_T    prAisBssInfo, prBowBssInfo;

    prAisBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX];
    prBowBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX];

    if (RLM_NET_PARAM_VALID(prAisBssInfo) && RLM_NET_PARAM_VALID(prBowBssInfo)) {
        if (prAisBssInfo->eBand != prBowBssInfo->eBand ||
                prAisBssInfo->ucPrimaryChannel != prBowBssInfo->ucPrimaryChannel) {

            /* Notify BOW to do deactivation */
            bowNotifyAllLinkDisconnected(prAdapter);
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return TRUE: permitted
*         FALSE: Not permitted
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
cnmAisIbssIsPermitted(
    P_ADAPTER_T     prAdapter
)
{
#if CFG_ENABLE_WIFI_DIRECT
    if (IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_P2P_INDEX)) {
        return FALSE;
    }
#endif

#if CFG_ENABLE_BT_OVER_WIFI
    if (IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_BOW_INDEX)) {
        return FALSE;
    }
#endif

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return TRUE: permitted
*         FALSE: Not permitted
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
cnmP2PIsPermitted(
    P_ADAPTER_T     prAdapter
)
{
    P_BSS_INFO_T    prBssInfo;

    prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX];

    if (IS_BSS_ACTIVE(prBssInfo) &&
            prBssInfo->eCurrentOPMode == OP_MODE_IBSS) {
        return FALSE;
    }

#if CFG_ENABLE_BT_OVER_WIFI
    if (IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_BOW_INDEX)) {
        /* Notify BOW to do deactivation */
        bowNotifyAllLinkDisconnected(prAdapter);
    }
#endif

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return TRUE: permitted
*         FALSE: Not permitted
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
cnmBowIsPermitted(
    P_ADAPTER_T     prAdapter
)
{
    P_BSS_INFO_T    prBssInfo;

    prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX];

    if (IS_BSS_ACTIVE(prBssInfo) &&
            prBssInfo->eCurrentOPMode == OP_MODE_IBSS) {
        return FALSE;
    }

#if CFG_ENABLE_WIFI_DIRECT
    if (IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_P2P_INDEX)) {
        return FALSE;
    }
#endif

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param (none)
*
* @return TRUE: permitted
*         FALSE: Not permitted
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
cnmBss40mBwPermitted(
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIdx
)
{
    P_BSS_INFO_T    prBssInfo;
    UINT_8          i;

    /* Note: To support real-time decision instead of current activated-time,
     *       the STA roaming case shall be considered about synchronization
     *       problem. Another variable fgAssoc40mBwAllowed is added to
     *       represent HT capability when association
     */
    for (i = 0; i < NETWORK_TYPE_INDEX_NUM; i++) {
        if (i != (UINT_8) eNetTypeIdx) {
            prBssInfo = &prAdapter->rWifiVar.arBssInfo[i];

            if (IS_BSS_ACTIVE(prBssInfo) && (prBssInfo->fg40mBwAllowed ||
                                             prBssInfo->fgAssoc40mBwAllowed)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}


