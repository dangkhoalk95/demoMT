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

#ifndef _WLAN_WT_H
#define _WLAN_WT_H
/******************************************************************************
*                                 M A C R O S
*******************************************************************************
*/
#define wtChangeMediaState(_prAdapter, _eNewMediaState) \
            (_prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].eConnectionState = (_eNewMediaState));


/******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************
*/
UINT_16
wtGetStaRecNumByNetType(
    IN P_ADAPTER_T                 prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex
);

VOID
wtRunEventAAATxFail(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

WLAN_STATUS
wtRunEventAAAComplete(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

WLAN_STATUS
wtRunEventAAASuccess(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

WLAN_STATUS
wtRunEventRxDeAuth(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN P_SW_RFB_T prSwRfb
);

BOOLEAN
wtValidateAuth(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    IN PP_STA_RECORD_T pprStaRec,
    OUT PUINT_16 pu2StatusCode
);

BOOLEAN
wtValidateAssocReq(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT PUINT_16 pu2StatusCode
);

VOID
wtRequestCh(
    IN P_ADAPTER_T prAdapter
);

VOID
wtReleaseCh(
    IN P_ADAPTER_T prAdapter
);

VOID
wtRunEventChGrant(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
);

VOID
wtInit(
    IN P_ADAPTER_T prAdapter
);

VOID
wtFuncDisconnect(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN BOOLEAN fgSendDeauth,
    IN UINT_16 u2ReasonCode
);

WLAN_STATUS
wlanSendSetQueryWTCmd(
    IN P_ADAPTER_T  prAdapter,
    UINT_8          ucCID,
    BOOLEAN         fgSetQuery,
    BOOLEAN         fgNeedResp,
    PFN_CMD_DONE_HANDLER pfCmdDoneHandler,
    PFN_CMD_TIMEOUT_HANDLER pfCmdTimeoutHandler,
    UINT_32         u4SetQueryInfoLen,
    PUINT_8         pucInfoBuffer,
    OUT PVOID       pvSetQueryBuffer,
    IN UINT_32      u4SetQueryBufferLen
);

WLAN_STATUS
wtAddPairwiseKey(
    IN  P_ADAPTER_T prAdapter,
    IN  PVOID       pvSetBuffer,
    IN  UINT_32     u4SetBufferLen,
    OUT PUINT_32    pu4SetInfoLen
);

WLAN_STATUS
wtUpdateTxPower(
    IN P_ADAPTER_T  prAdapter,
    IN INT_8        cTxPower
);

VOID
wtResetStaRecStatus(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
);

WLAN_STATUS
wtRunEventDeauthTxDone(
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T rTxDoneStatus
);
BOOLEAN
wtValidateProbeReq(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT PUINT_32 pu4ControlFlags
);

VOID
rlmHandleObssStatusEventPkt(
    P_ADAPTER_T                 prAdapter,
    P_EVENT_AP_OBSS_STATUS_T    prObssStatus
);

VOID
wtRunEventRxDisassociation(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN P_SW_RFB_T prSwRfb
);

#endif /* _WLAN_WT_H */

