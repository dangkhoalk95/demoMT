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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/assoc.c#30 $
*/

/*! \file   "assoc.c"
    \brief  This file includes the association-related functions.

    This file includes the association-related functions.
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
APPEND_VAR_IE_ENTRY_T txAssocReqIETable[] = {
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_HT_CAP),    NULL,     rlmReqGenerateHtCapIE },/* 45 */
#if CFG_SUPPORT_WPS2
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_WSC),   NULL,      rsnGenerateWSCIE },     /* 221 */
#endif
#ifdef __WAPI_SUPPORT__
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_WAPI),   NULL,    wapiGenerateWAPIIE },   /* 68 */
#endif
#if CFG_RSN_MIGRATION
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_WPA),   NULL,        rsnGenerateWPAIE },     /* 72 */
#endif
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_EXT_CAP),   NULL,    rlmReqGenerateExtCapIE },  /* 127 */
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_WMM_INFO),   NULL,   mqmGenerateWmmInfoIE },  /* 221 */
#if CFG_RSN_MIGRATION
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_RSN),   NULL,        rsnGenerateRSNIE },    /* 221 */
#endif
#if CFG_ENABLE_WIFI_DIRECT
    { (0),   linkToP2pCalculateP2P_IELenForAssocReq,  linkToP2pGenerateP2P_IEForAssocReq },   /* 221 */
#endif
};

#if CFG_SUPPORT_AAA
VERIFY_IE_ENTRY_T rxAssocReqIETable[] = {
    { ELEM_ID_RESERVED,                         NULL                    } /* 255 */
};

APPEND_IE_ENTRY_T txAssocRespIETable[] = {
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_ERP),        rlmRspGenerateErpIE   },        /* 42 */
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_HT_CAP),     rlmRspGenerateHtCapIE   },      /* 45 */
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_HT_OP),      rlmRspGenerateHtOpIE   },       /* 61 */
#if CFG_ENABLE_WIFI_DIRECT
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_OBSS_SCAN),  linkToRlmRspGenerateObssScanIE   },   /* 74 */
#endif
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_EXT_CAP),    rlmRspGenerateExtCapIE   },      /* 127 */
    { (ELEM_HDR_LEN + ELEM_MAX_LEN_WMM_PARAM),  mqmGenerateWmmParamIE }         /* 221 */
};
#endif


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
* @brief This function is used to compose the Capability Info Field.
*
* @param[in] prStaRec               Pointer to the STA_RECORD_T
*
* @retval Capability Info Field
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ UINT_16
assocBuildCapabilityInfo(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
)
{
    UINT_32 u4NonHTPhyType;
    UINT_16 u2CapInfo;


    ASSERT(prStaRec);


    /* Set up our requested capabilities. */
    u2CapInfo = CAP_INFO_ESS;
    u2CapInfo |= CAP_CF_STA_NOT_POLLABLE;

    if (prStaRec->u2CapInfo & CAP_INFO_PRIVACY) {
        u2CapInfo |= CAP_INFO_PRIVACY;
    }


    /* 7.3.1.4 */
    if (prStaRec->fgHasBasicPhyType) {
        u4NonHTPhyType = prStaRec->ucNonHTBasicPhyType;

        if ((rNonHTPhyAttributes[u4NonHTPhyType].fgIsShortPreambleOptionImplemented) &&
                ((prAdapter->rWifiVar.ePreambleType == PREAMBLE_TYPE_SHORT) ||  /* Short Preamble Option Enable is TRUE */
                 ((prAdapter->rWifiVar.ePreambleType == PREAMBLE_TYPE_AUTO) &&
                  (prStaRec->u2CapInfo & CAP_INFO_SHORT_PREAMBLE)))) {

            /* Case I: Implemented == TRUE and Short Preamble Option Enable == TRUE.
             * Case II: Implemented == TRUE and Short Preamble == AUTO (depends on
             *          BSS_DESC_T's capability)
             */
            u2CapInfo |= CAP_INFO_SHORT_PREAMBLE;
        }

        if (rNonHTPhyAttributes[u4NonHTPhyType].fgIsShortSlotTimeOptionImplemented &&
                prAdapter->rWifiVar.fgIsShortSlotTimeOptionEnable) {
            u2CapInfo |= CAP_INFO_SHORT_SLOT_TIME;
        }
    }

    DBGLOG(SAA, LOUD, ("ASSOC REQ: Compose Capability = 0x%04x for Target BSS ["MACSTR"].\n",
                       u2CapInfo, MAC2STR(prStaRec->aucMacAddr)));


    return u2CapInfo;

} /* end of assocBuildCapabilityInfo() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used to compose Common Information Elements for Association
*        Request Frame.
*
* @param[in] prMsduInfo     Pointer to the composed MSDU_INFO_T.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
assocBuildReAssocReqFrameCommonIEs(
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfo
)
{
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_STA_RECORD_T prStaRec;
    PUINT_8 pucBuffer;
    UINT_16 u2SupportedRateSet;
    UINT_8 aucAllSupportedRates[RATE_NUM] = {0};
    UINT_8 ucAllSupportedRatesLen;
    UINT_8 ucSupRatesLen;
    UINT_8 ucExtSupRatesLen;

    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    ASSERT(prMsduInfo);
    ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
    ASSERT(prStaRec);

    if (!prStaRec) {
        return;
    }

    pucBuffer = (PUINT_8)((UINT_32)prMsduInfo->prPacket +
                          (UINT_32)prMsduInfo->u2FrameLength);
    ASSERT(pucBuffer);

    if (IS_STA_IN_AIS(prStaRec)) {

        /* Fill the SSID element. */
        SSID_IE(pucBuffer)->ucId = ELEM_ID_SSID;

        /* NOTE(Kevin): We copy the SSID from CONNECTION_SETTINGS for the case of
         * Passive Scan and the target BSS didn't broadcast SSID on its Beacon Frame.
         */

        COPY_SSID(SSID_IE(pucBuffer)->aucSSID,
                  SSID_IE(pucBuffer)->ucLength,
                  prConnSettings->aucSSID,
                  prConnSettings->ucSSIDLen);

        prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
        pucBuffer += IE_SIZE(pucBuffer);

    }
#if CFG_ENABLE_WIFI_DIRECT
    else if ((prAdapter->fgIsP2PRegistered) &&
             (IS_STA_IN_P2P(prStaRec))) {
        ASSERT(prAdapter->rP2pFuncLkr.prP2pBuildReassocReqFrameCommIEs);
        pucBuffer = prAdapter->rP2pFuncLkr.prP2pBuildReassocReqFrameCommIEs(prAdapter, prMsduInfo, pucBuffer);
    }
#endif
#if CFG_ENABLE_BT_OVER_WIFI
    else if (IS_STA_IN_BOW(prStaRec)) {

        SSID_IE(pucBuffer)->ucId = ELEM_ID_SSID;

        /* NOTE(Kevin): We copy the SSID from CONNECTION_SETTINGS for the case of
         * Passive Scan and the target BSS didn't broadcast SSID on its Beacon Frame.
         */

        COPY_SSID(SSID_IE(pucBuffer)->aucSSID,
                  SSID_IE(pucBuffer)->ucLength,
                  prConnSettings->aucSSID,
                  prConnSettings->ucSSIDLen);

        prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
        pucBuffer += IE_SIZE(pucBuffer);
    }
#endif

    else {
        /* TODO(Kevin): For other network */
    }

    /* NOTE(Kevin 2008/12/19): 16.3.6.3 MLME-ASSOCIATE.indication -
     * SupportedRates - The set of data rates that are supported by the STA
     * that is requesting association.
     * Original(Portable Driver): Only send the Rates that we'll support.
     * New: Send the Phy Rates if the result of following & operation == NULL.
     */
    //rateGetDataRatesFromRateSet((prBssDesc->u2OperationalRateSet &
    //                             rPhyAttributes[prBssDesc->ePhyType].u2SupportedRateSet),

    if (prStaRec->fgHasBasicPhyType) {
        UINT_32 u4NonHTPhyType;


        u4NonHTPhyType = prStaRec->ucNonHTBasicPhyType;

        u2SupportedRateSet = (prStaRec->u2OperationalRateSet &
                              rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet);

        ASSERT(u2SupportedRateSet);

        if (!u2SupportedRateSet) {
            u2SupportedRateSet = rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet;
        }

        /* TODO(Kevin): For P2P, we shouldn't send support rate set which contains 11b rate */

        rateGetDataRatesFromRateSet(u2SupportedRateSet,
                                    0,
                                    aucAllSupportedRates,
                                    &ucAllSupportedRatesLen);

        ucSupRatesLen = ((ucAllSupportedRatesLen > ELEM_MAX_LEN_SUP_RATES) ?
                         ELEM_MAX_LEN_SUP_RATES : ucAllSupportedRatesLen);

        ucExtSupRatesLen = ucAllSupportedRatesLen - ucSupRatesLen;


        /* Fill the Supported Rates element. */
        if (ucSupRatesLen) {
            SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_SUP_RATES;
            SUP_RATES_IE(pucBuffer)->ucLength = ucSupRatesLen;
            kalMemCopy(SUP_RATES_IE(pucBuffer)->aucSupportedRates,
                       aucAllSupportedRates,
                       ucSupRatesLen);

            prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
            pucBuffer += IE_SIZE(pucBuffer);
        }


        /* Fill the Extended Supported Rates element. */
        if (ucExtSupRatesLen) {

            EXT_SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_EXTENDED_SUP_RATES;
            EXT_SUP_RATES_IE(pucBuffer)->ucLength = ucExtSupRatesLen;

            kalMemCopy(EXT_SUP_RATES_IE(pucBuffer)->aucExtSupportedRates,
                       &aucAllSupportedRates[ucSupRatesLen],
                       ucExtSupRatesLen);

            prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
            pucBuffer += IE_SIZE(pucBuffer);
        }
    }

    return;
} /* end of assocBuildReAssocReqFrameCommonIEs() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will compose the (Re)Association Request frame header and
*        its fixed fields
*
* @param[in] prStaRec               Pointer to the STA_RECORD_T
* @param[in] pucBuffer              Pointer to the frame buffer.
* @param[in] aucMACAddress          Given Our MAC Address.
* @param[in out] pu2PayloadLen      Return the length of the composed fixed fields
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
assocComposeReAssocReqFrameHeaderAndFF(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN PUINT_8 pucBuffer,
    IN UINT_8 aucMACAddress[],
    IN OUT PUINT_16 pu2PayloadLen
)
{
    P_WLAN_ASSOC_REQ_FRAME_T prAssocFrame;
    BOOLEAN fgIsReAssoc;

    UINT_16 u2FrameCtrl;
    UINT_16 u2CapInfo;
    UINT_16 u2ListenInterval;


    ASSERT(prStaRec);
    ASSERT(pucBuffer);
    ASSERT(aucMACAddress);
    ASSERT(pu2PayloadLen);

    prAssocFrame = (P_WLAN_ASSOC_REQ_FRAME_T)pucBuffer;
    fgIsReAssoc = prStaRec->fgIsReAssoc;

    //4 <1> Compose the frame header of the (Re)Association Request  frame.
    /* Fill the Frame Control field. */
    if (fgIsReAssoc) {
        u2FrameCtrl = MAC_FRAME_REASSOC_REQ;
    } else {
        u2FrameCtrl = MAC_FRAME_ASSOC_REQ;
    }
    WLAN_SET_FIELD_16(&prAssocFrame->u2FrameCtrl, u2FrameCtrl);

    /* Fill the DA field with Target BSSID. */
    COPY_MAC_ADDR(prAssocFrame->aucDestAddr, prStaRec->aucMacAddr);

    /* Fill the SA field with our MAC Address. */
    COPY_MAC_ADDR(prAssocFrame->aucSrcAddr, aucMACAddress);

    /* Fill the BSSID field with Target BSSID. */
    COPY_MAC_ADDR(prAssocFrame->aucBSSID, prStaRec->aucMacAddr);

    /* Clear the SEQ/FRAG_NO field(HW won't overide the FRAG_NO, so we need to clear it). */
    prAssocFrame->u2SeqCtrl = 0;


    //4 <2> Compose the frame body's common fixed field part of the (Re)Association Request  frame.
    u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);

    /* Fill the Capability Information field. */
    WLAN_SET_FIELD_16(&prAssocFrame->u2CapInfo, u2CapInfo);


    /* Calculate the listen interval for the maximum power mode. Currently, we
       set it to the value 2 times DTIM period. */
    if (prStaRec->ucDTIMPeriod) {
        u2ListenInterval = prStaRec->ucDTIMPeriod * DEFAULT_LISTEN_INTERVAL_BY_DTIM_PERIOD;
    } else {
        DBGLOG(SAA, TRACE, ("Use default listen interval\n"));
        u2ListenInterval = DEFAULT_LISTEN_INTERVAL;
    }
    prStaRec->u2ListenInterval = u2ListenInterval;

    /* Fill the Listen Interval field. */
    WLAN_SET_FIELD_16(&prAssocFrame->u2ListenInterval, u2ListenInterval);


    //4 <3> Compose the Current AP Address field for ReAssociation Request  frame.
    /* Fill the Current AP Address field. */
    if (prStaRec->fgIsReAssoc) {
        if (IS_STA_IN_AIS(prStaRec)) {

            P_AIS_BSS_INFO_T prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
            P_WLAN_REASSOC_REQ_FRAME_T prReAssocFrame =
                (P_WLAN_REASSOC_REQ_FRAME_T)prAssocFrame;

            COPY_MAC_ADDR(prReAssocFrame->aucCurrentAPAddr, prAisBssInfo->aucBSSID);
        } else {
            ASSERT(0); /* We don't support ReAssociation for other network */
        }

        *pu2PayloadLen = (CAP_INFO_FIELD_LEN +
                          LISTEN_INTERVAL_FIELD_LEN +
                          CURR_AP_ADDR_FIELD_LEN);
    } else {
        *pu2PayloadLen = (CAP_INFO_FIELD_LEN +
                          LISTEN_INTERVAL_FIELD_LEN);
    }

    return;
} /* end of assocComposeReAssocReqFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will send the (Re)Association Request frame
*
* @param[in] prStaRec           Pointer to the STA_RECORD_T
*
* @retval WLAN_STATUS_RESOURCES No available resource for frame composing.
* @retval WLAN_STATUS_SUCCESS   Successfully send frame to TX Module
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocSendReAssocReqFrame(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
)
{
    P_MSDU_INFO_T prMsduInfo;
    P_BSS_INFO_T prBssInfo;

    UINT_16 u2PayloadLen;
    UINT_16 u2EstimatedFrameLen;
    UINT_16 u2EstimatedExtraIELen;
    BOOLEAN fgIsReAssoc;
    UINT_32 i;


    ASSERT(prStaRec);
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "Send (Re)Assoc Frame\n");
    //4 <1> Allocate a PKT_INFO_T for Authentication Frame
    fgIsReAssoc = prStaRec->fgIsReAssoc;

    /* Init with MGMT Header Length + Length of Fixed Fields + Common IE Length */
    if (fgIsReAssoc) {
        u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD + \
                              WLAN_MAC_MGMT_HEADER_LEN + \
                              CAP_INFO_FIELD_LEN + \
                              LISTEN_INTERVAL_FIELD_LEN + \
                              CURR_AP_ADDR_FIELD_LEN + \
                              (ELEM_HDR_LEN + ELEM_MAX_LEN_SSID) + \
                              (ELEM_HDR_LEN + ELEM_MAX_LEN_SUP_RATES) + \
                              (ELEM_HDR_LEN + (RATE_NUM - ELEM_MAX_LEN_SUP_RATES));
    } else {
        u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD + \
                              WLAN_MAC_MGMT_HEADER_LEN + \
                              CAP_INFO_FIELD_LEN + \
                              LISTEN_INTERVAL_FIELD_LEN + \
                              (ELEM_HDR_LEN + ELEM_MAX_LEN_SSID) + \
                              (ELEM_HDR_LEN + ELEM_MAX_LEN_SUP_RATES) + \
                              (ELEM_HDR_LEN + (RATE_NUM - ELEM_MAX_LEN_SUP_RATES));
    }

    /* + Extra IE Length */
    u2EstimatedExtraIELen = 0;


    for (i = 0; i < sizeof(txAssocReqIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
        if (txAssocReqIETable[i].u2EstimatedFixedIELen != 0) {
            u2EstimatedExtraIELen += txAssocReqIETable[i].u2EstimatedFixedIELen;
        } else {
            u2EstimatedExtraIELen += (UINT_16)txAssocReqIETable[i].pfnCalculateVariableIELen(prAdapter,
                                     (ENUM_NETWORK_TYPE_INDEX_T)prStaRec->ucNetTypeIndex,
                                     prStaRec);
        }

    }

#ifdef __WPS_SUPPORT__
    if (!prAdapter->WpsAssoc) {
        u2EstimatedExtraIELen -= (ELEM_HDR_LEN + ELEM_MAX_LEN_WSC);
    }
#endif

    u2EstimatedFrameLen += u2EstimatedExtraIELen;

    /* Allocate a MSDU_INFO_T */
    if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "No PKT_INFO_T for sending (Re)Assoc Request.\n");
        return WLAN_STATUS_RESOURCES;
    }

    //4 <2> Compose (Re)Association Request frame header and fixed fields in MSDU_INfO_T.
    ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);
    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);


    /* Compose Header and Fixed Field */
    assocComposeReAssocReqFrameHeaderAndFF(prAdapter,
                                           prStaRec,
                                           (PUINT_8)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD),
                                           prBssInfo->aucOwnMacAddr,
                                           &u2PayloadLen);

    //4 <3> Update information of MSDU_INFO_T
    prMsduInfo->eSrc = TX_PACKET_MGMT;
    prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
    prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
    prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
    prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
    prMsduInfo->fgIs802_1x = FALSE;
    prMsduInfo->fgIs802_11 = TRUE;
    prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
    prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
    prMsduInfo->pfTxDoneHandler = saaFsmRunEventTxDone;
    prMsduInfo->fgIsBasicRate = TRUE;

    //4 <4> Compose the frame body's IEs of the (Re)Association Request  frame.
    assocBuildReAssocReqFrameCommonIEs(prAdapter, prMsduInfo);


    //4 <5> Compose IEs in MSDU_INFO_T

    /* Append IE */
    for (i = 0; i < sizeof(txAssocReqIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
        if (txAssocReqIETable[i].pfnAppendIE) {
            txAssocReqIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
        }
    }

    //4 <6> Update the (Re)association request information
    if (IS_STA_IN_AIS(prStaRec)) {
        P_WLAN_ASSOC_REQ_FRAME_T prAssocFrame;

        prAssocFrame = (P_WLAN_ASSOC_REQ_FRAME_T)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD);

#if CFG_RSN_MIGRATION
        kalUpdateReAssocReqInfo(prAdapter->prGlueInfo,
                                (PUINT_8)&prAssocFrame->u2CapInfo,
                                prMsduInfo->u2FrameLength,
                                fgIsReAssoc);
#endif
    }

#if CFG_ENABLE_WIFI_DIRECT
    if ((prAdapter->fgIsP2PRegistered) && (IS_STA_IN_P2P(prStaRec))) {
        P_WLAN_ASSOC_REQ_FRAME_T prAssocFrame;

        prAssocFrame = (P_WLAN_ASSOC_REQ_FRAME_T)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD);

        if (prAdapter->rP2pFuncLkr.prKalP2pUpdateAssocInfo) {
            prAdapter->rP2pFuncLkr.prKalP2pUpdateAssocInfo(
                prAdapter->prGlueInfo,
                (PUINT_8)&prAssocFrame->u2CapInfo,
                prMsduInfo->u2FrameLength,
                fgIsReAssoc);
        } else {
            ASSERT(0);
        }
    }
#endif

    /* TODO(Kevin): Also release the unused tail room of the composed MMPDU */

    //4 <6> Enqueue the frame to send this (Re)Association request frame.
    nicTxEnqueueMsdu(prAdapter, prMsduInfo);

    return WLAN_STATUS_SUCCESS;
} /* end of assocSendReAssocReqFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will strictly check the TX (Re)Association Request frame for
*        SAA event handling.
*
* @param[in] prMsduInfo         Pointer of MSDU_INFO_T
*
* @retval WLAN_STATUS_FAILURE   This is not the frame we should handle at current state.
* @retval WLAN_STATUS_SUCCESS   This is the frame we should handle.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocCheckTxReAssocReqFrame(
    IN P_ADAPTER_T      prAdapter,
    IN P_MSDU_INFO_T    prMsduInfo
)
{
    P_WLAN_ASSOC_REQ_FRAME_T prAssocReqFrame;
    P_STA_RECORD_T prStaRec;
    UINT_16 u2TxFrameCtrl;


    ASSERT(prMsduInfo);
    ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

    prAssocReqFrame = (P_WLAN_ASSOC_REQ_FRAME_T)(prMsduInfo->prPacket);
    ASSERT(prAssocReqFrame);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
    ASSERT(prStaRec);

    if (!prStaRec) {
        return WLAN_STATUS_INVALID_PACKET;
    }

    //WLAN_GET_FIELD_16(&prAssocReqFrame->u2FrameCtrl, &u2TxFrameCtrl)
    u2TxFrameCtrl = prAssocReqFrame->u2FrameCtrl; // NOTE(Kevin): Optimized for ARM
    u2TxFrameCtrl &= MASK_FRAME_TYPE;
    if (prStaRec->fgIsReAssoc) {
        if (u2TxFrameCtrl != MAC_FRAME_REASSOC_REQ) {
            return WLAN_STATUS_FAILURE;
        }
    } else {
        if (u2TxFrameCtrl != MAC_FRAME_ASSOC_REQ) {
            return WLAN_STATUS_FAILURE;
        }
    }

    return WLAN_STATUS_SUCCESS;

} /* end of assocCheckTxReAssocReqFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will strictly check the TX (Re)Association Response frame for
*        AAA event handling.
*
* @param[in] prMsduInfo         Pointer of MSDU_INFO_T
*
* @retval WLAN_STATUS_FAILURE   This is not the frame we should handle at current state.
* @retval WLAN_STATUS_SUCCESS   This is the frame we should handle.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocCheckTxReAssocRespFrame(
    IN P_ADAPTER_T   prAdapter,
    IN P_MSDU_INFO_T prMsduInfo
)
{
    P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
    P_STA_RECORD_T prStaRec;
    UINT_16 u2TxFrameCtrl;


    ASSERT(prMsduInfo);
    ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

    prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T)(prMsduInfo->prPacket);
    ASSERT(prAssocRspFrame);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
    ASSERT(prStaRec);

    if (!prStaRec) {
        return WLAN_STATUS_INVALID_PACKET;
    }

    //WLAN_GET_FIELD_16(&prAssocFrame->u2FrameCtrl, &u2TxFrameCtrl)
    u2TxFrameCtrl = prAssocRspFrame->u2FrameCtrl; // NOTE(Kevin): Optimized for ARM
    u2TxFrameCtrl &= MASK_FRAME_TYPE;
    if (prStaRec->fgIsReAssoc) {
        if (u2TxFrameCtrl != MAC_FRAME_REASSOC_RSP) {
            return WLAN_STATUS_FAILURE;
        }
    } else {
        if (u2TxFrameCtrl != MAC_FRAME_ASSOC_RSP) {
            return WLAN_STATUS_FAILURE;
        }
    }

    return WLAN_STATUS_SUCCESS;

} /* end of assocCheckTxReAssocRespFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will validate the incoming (Re)Association Frame and take out
*        the status code.
*
* @param[in] prSwRfb                Pointer to SW RFB data structure.
* @param[out] pu2StatusCode         Pointer to store the Status Code from Authentication.
*
* @retval WLAN_STATUS_FAILURE   This is not the frame we should handle at current state.
* @retval WLAN_STATUS_SUCCESS   This is the frame we should handle.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocCheckRxReAssocRspFrameStatus(
    IN P_ADAPTER_T  prAdapter,
    IN P_SW_RFB_T   prSwRfb,
    OUT PUINT_16    pu2StatusCode
)
{
    P_STA_RECORD_T prStaRec;
    P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
    UINT_16 u2RxFrameCtrl;
    UINT_16 u2RxCapInfo;
    UINT_16 u2RxStatusCode;
    UINT_16 u2RxAssocId;


    ASSERT(prSwRfb);
    ASSERT(pu2StatusCode);

    if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) < (CAP_INFO_FIELD_LEN +
            STATUS_CODE_FIELD_LEN +
            AID_FIELD_LEN)) {
        ASSERT(0);
        return WLAN_STATUS_FAILURE;
    }

    DBGLOG(SAA, LOUD, ("prSwRfb->u2PayloadLength = %d\n", prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen));

    prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
    ASSERT(prStaRec);

    if (!prStaRec) {
        return WLAN_STATUS_INVALID_PACKET;
    }

    //4 <1> locate the (Re)Assocation Resp Frame.
    prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T) prSwRfb->pvHeader;

    //4 <2> Parse the Header of (Re)Assocation Resp Frame.
    //WLAN_GET_FIELD_16(&prAssocRspFrame->u2FrameCtrl, &u2RxFrameCtrl);
    u2RxFrameCtrl = prAssocRspFrame->u2FrameCtrl; // NOTE(Kevin): Optimized for ARM
    u2RxFrameCtrl &= MASK_FRAME_TYPE;
    if (prStaRec->fgIsReAssoc) {
        if (u2RxFrameCtrl != MAC_FRAME_REASSOC_RSP) {
            return WLAN_STATUS_FAILURE;
        }
    } else {
        if (u2RxFrameCtrl != MAC_FRAME_ASSOC_RSP) {
            return WLAN_STATUS_FAILURE;
        }
    }

    //4 <3> Parse the Fixed Fields of (Re)Assocation Resp Frame Body.
    //WLAN_GET_FIELD_16(&prAssocRspFrame->u2CapInfo, &u2RxCapInfo);
    u2RxCapInfo = prAssocRspFrame->u2CapInfo; // NOTE(Kevin): Optimized for ARM

    //WLAN_GET_FIELD_16(&prAssocRspFrame->u2StatusCode, &u2RxStatusCode);
    u2RxStatusCode = prAssocRspFrame->u2StatusCode; // NOTE(Kevin): Optimized for ARM

    //4 <4> Check CAP_INFO
    /* NOTE(Kevin): CM suggest to add MGMT workaround for those APs didn't check
     * the CAP Privacy Bit to overcome a corner case that the Privacy Bit
     * of our SCAN result didn't consist with AP's Association Resp.
     */
    if (u2RxStatusCode == STATUS_CODE_SUCCESSFUL) {
#ifdef __WAPI_SUPPORT__
        if (prAdapter->rWifiVar.rConnSettings.fgWapiMode) {
            /* WAPI AP allow the customer use WZC to join mode, the privacy bit is 0 */
            /* even at WAI & WAPI_PSK mode, but the assoc respose set the privacy bit set 1 */
            DBGLOG(SEC, TRACE, ("Workaround the WAPI AP allow the customer to use WZC to join\n"));
        } else
#endif
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered && 1) {
                /* Todo:: Fixed this */
            } else
#endif
                if ((prStaRec->u2CapInfo & CAP_INFO_PRIVACY) ^ (u2RxCapInfo & CAP_INFO_PRIVACY)) {
                    u2RxStatusCode = STATUS_CODE_CAP_NOT_SUPPORTED;
                }
    }

    if (u2RxStatusCode == STATUS_CODE_SUCCESSFUL) {
#if CFG_RSN_MIGRATION
        /* Update the information in the structure used to query and set
           OID_802_11_ASSOCIATION_INFORMATION. */
        kalUpdateReAssocRspInfo(prAdapter->prGlueInfo,
                                (PUINT_8)&prAssocRspFrame->u2CapInfo,
                                (UINT_32)(prSwRfb->u2PacketLen));
#endif
    }

    //4 <5> Update CAP_INFO and ASSOC_ID
    if (u2RxStatusCode == STATUS_CODE_SUCCESSFUL) {
        prStaRec->u2CapInfo = u2RxCapInfo;

        //WLAN_GET_FIELD_16(&prAssocRspFrame->u2AssocId, &u2RxAssocId);
        u2RxAssocId = prAssocRspFrame->u2AssocId; // NOTE(Kevin): Optimized for ARM

        /*  20110715 Workaround for Kingnet 710 AP (Realtek 8186)
                *   This AP raises the bit 6&7 not bit 14&15 in AID field.
                *   It cause wrong AID assignment.
                *   For AID = 2
                *     Normal case: 0xC002(1100 0000 0000 0010) => 2
                *     Kingnet 710:  0x00C2(0000 0000 1100 0010) => 194
                *     workaround: mask bit 6&7 for this AP
                */
        if ((u2RxAssocId & BIT(6)) &&
                (u2RxAssocId & BIT(7)) &&
                !(u2RxAssocId & BITS(8, 15))) {
            prStaRec->u2AssocId = u2RxAssocId & ~BITS(6, 7);
        } else {
            prStaRec->u2AssocId = u2RxAssocId & ~AID_MSB;
        }
    }

    *pu2StatusCode = u2RxStatusCode;

    return WLAN_STATUS_SUCCESS;

} /* end of assocCheckRxReAssocRspFrameStatus() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will compose the Disassociation frame
*
* @param[in] prStaRec               Pointer to the STA_RECORD_T
* @param[in] pucBuffer              Pointer to the frame buffer.
* @param[in] aucMACAddress     Given Our MAC Address.
* @param[in] u2ReasonCode      The reason code of disassociation
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
assocComposeDisassocFrame(
    IN P_STA_RECORD_T   prStaRec,
    IN PUINT_8          pucBuffer,
    IN UINT_8           aucMACAddress[],
    IN UINT_16          u2ReasonCode
)
{
    P_WLAN_DISASSOC_FRAME_T prDisAssocFrame;
    UINT_16 u2FrameCtrl;

    ASSERT(pucBuffer);
    ASSERT(pucBuffer);
    ASSERT(aucMACAddress);

    prDisAssocFrame = (P_WLAN_DISASSOC_FRAME_T)pucBuffer;

    //4 <1> Compose the frame header of the DisAssociation  frame.
    /* Fill the Frame Control field. */
    u2FrameCtrl = MAC_FRAME_DISASSOC;

    WLAN_SET_FIELD_16(&prDisAssocFrame->u2FrameCtrl, u2FrameCtrl);

    /* Fill the DA field with Target BSSID. */
    COPY_MAC_ADDR(prDisAssocFrame->aucDestAddr, prStaRec->aucMacAddr);

    /* Fill the SA field with our MAC Address. */
    COPY_MAC_ADDR(prDisAssocFrame->aucSrcAddr, aucMACAddress);

    /* Fill the BSSID field with Target BSSID. */
    COPY_MAC_ADDR(prDisAssocFrame->aucBSSID, prStaRec->aucMacAddr);

    /* Clear the SEQ/FRAG_NO field(HW won't overide the FRAG_NO, so we need to clear it). */
    prDisAssocFrame->u2SeqCtrl = 0;

    //4 <2> Compose the frame body's fixed field part of the Disassociation frame.
    /* Fill the Reason Code field. */
    WLAN_SET_FIELD_16(&prDisAssocFrame->u2ReasonCode, u2ReasonCode);

    return;
} /* end of assocComposeDisassocFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will send the Disassociation frame
*
* @param[in] prStaRec           Pointer to the STA_RECORD_T
* @param[in] u2ReasonCode  The reason code of disassociation
*
* @retval WLAN_STATUS_RESOURCES No available resource for frame composing.
* @retval WLAN_STATUS_SUCCESS   Successfully send frame to TX Module
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocSendDisAssocFrame(
    IN P_ADAPTER_T    prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN UINT_16        u2ReasonCode
)
{
    PUINT_8 pucMacAddress;
    P_MSDU_INFO_T prMsduInfo;
    UINT_16 u2PayloadLen;
    UINT_16 u2EstimatedFrameLen;
    //UINT_32 u4Status = WLAN_STATUS_SUCCESS;


    ASSERT(prStaRec);

    //4 <1> Allocate a PKT_INFO_T for Disassociation Frame
    /* Init with MGMT Header Length + Length of Fixed Fields + IE Length */
    u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD + \
                          WLAN_MAC_MGMT_HEADER_LEN + \
                          REASON_CODE_FIELD_LEN;

    /* Allocate a MSDU_INFO_T */
    if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
        DBGLOG(SAA, WARN, ("No PKT_INFO_T for sending DisAssoc.\n"));
        return WLAN_STATUS_RESOURCES;
    }

    //4 <2> Compose Disassociation  frame header and fixed fields in MSDU_INfO_T.
    ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

    pucMacAddress = prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex].aucOwnMacAddr;

    /* Compose Header and Fixed Field */
    assocComposeDisassocFrame(prStaRec,
                              (PUINT_8)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD),
                              pucMacAddress,
                              u2ReasonCode);

    u2PayloadLen = REASON_CODE_FIELD_LEN;

    //4 <3> Update information of MSDU_INFO_T
    ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

    prMsduInfo->eSrc = TX_PACKET_MGMT;
    prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
    prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
    prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
    prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
    prMsduInfo->fgIs802_1x = FALSE;
    prMsduInfo->fgIs802_11 = TRUE;
    prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
    prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
    prMsduInfo->pfTxDoneHandler = NULL;
    prMsduInfo->fgIsBasicRate = TRUE;

    //4 <4> Enqueue the frame to send this (Re)Association request frame.
    nicTxEnqueueMsdu(prAdapter, prMsduInfo);

    return WLAN_STATUS_SUCCESS;
} /* end of assocSendDisAssocFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will parse and process the incoming Disassociation frame
*        if the given BSSID is matched.
*
* @param[in] prSwRfb            Pointer to SW RFB data structure.
* @param[in] aucBSSID           Given BSSID
* @param[out] pu2ReasonCode     Pointer to store the Reason Code from Deauthentication.
*
* @retval WLAN_STATUS_FAILURE   This is not the frame we should handle at current state.
* @retval WLAN_STATUS_SUCCESS   This is the frame we should handle.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocProcessRxDisassocFrame(
    IN P_ADAPTER_T  prAdapter,
    IN P_SW_RFB_T   prSwRfb,
    IN UINT_8       aucBSSID[],
    OUT PUINT_16    pu2ReasonCode
)
{
    P_WLAN_DISASSOC_FRAME_T prDisassocFrame;
    UINT_16 u2RxReasonCode;


    ASSERT(prSwRfb);
    ASSERT(aucBSSID);
    ASSERT(pu2ReasonCode);

    //4 <1> locate the Disassociation Frame.
    prDisassocFrame = (P_WLAN_DISASSOC_FRAME_T) prSwRfb->pvHeader;

    //4 <2> Parse the Header of Disassociation Frame.
    if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) < REASON_CODE_FIELD_LEN) {
        ASSERT(0);
        return WLAN_STATUS_FAILURE;
    }

    /* Check if this Disassoc Frame is coming from Target BSSID */
    if (UNEQUAL_MAC_ADDR((void *)prDisassocFrame->aucBSSID, (void *)aucBSSID)) {

        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_31, "[TETHERING]Ignore Disassoc Frame from other BSS ["MACSTR"]\n",
                     MAC2STR(prDisassocFrame->aucSrcAddr));
        return WLAN_STATUS_FAILURE;
    }

    //4 <3> Parse the Fixed Fields of Deauthentication Frame Body.
    WLAN_GET_FIELD_16(&prDisassocFrame->u2ReasonCode, &u2RxReasonCode);
    *pu2ReasonCode = u2RxReasonCode;

    return WLAN_STATUS_SUCCESS;

} /* end of assocProcessRxDisassocFrame() */


#if CFG_SUPPORT_AAA
/*----------------------------------------------------------------------------*/
/*!
* @brief This function will parse and process the incoming Association Req frame
*        and return a Status Code.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prSwRfb            Pointer to SW RFB data structure.
* @param[out] pu2StatusCode     Pointer to store the Status Code for carried in Association Response.
*
* @retval WLAN_STATUS_FAILURE   This is not the frame we should handle at current state.
* @retval WLAN_STATUS_SUCCESS   This is the frame we should handle.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocProcessRxAssocReqFrame(
    IN P_ADAPTER_T  prAdapter,
    IN P_SW_RFB_T   prSwRfb,
    OUT PUINT_16    pu2StatusCode
)
{
    P_WLAN_ASSOC_REQ_FRAME_T prAssocReqFrame;
    P_STA_RECORD_T prStaRec;
    P_BSS_INFO_T prBssInfo;
    P_IE_SSID_T prIeSsid = (P_IE_SSID_T)NULL;
#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK || CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    P_RSN_INFO_ELEM_T prIeRsn = (P_RSN_INFO_ELEM_T)NULL;
#endif
    P_IE_SUPPORTED_RATE_T prIeSupportedRate = (P_IE_SUPPORTED_RATE_T)NULL;
    P_IE_EXT_SUPPORTED_RATE_T prIeExtSupportedRate = (P_IE_EXT_SUPPORTED_RATE_T)NULL;
    PUINT_8 pucIE, pucIEStart;
    UINT_16 u2IELength;
    UINT_16 u2Offset = 0;
    UINT_16 u2StatusCode = STATUS_CODE_SUCCESSFUL;
    UINT_16 u2RxFrameCtrl;
    UINT_16 u2BSSBasicRateSet;
    BOOLEAN fgIsUnknownBssBasicRate;
    UINT_32 i;

    DEBUGFUNC("assocProcessRxAssocReqFrame");

    ASSERT(prAdapter);
    ASSERT(prSwRfb);
    ASSERT(pu2StatusCode);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

    if (prStaRec == NULL) {
        return WLAN_STATUS_FAILURE;
    }

    //4 <1> locate the Association Req Frame.
    prAssocReqFrame = (P_WLAN_ASSOC_REQ_FRAME_T) prSwRfb->pvHeader;

    //4 <2> Parse the Header of Association Req Frame.
    if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) <
            (CAP_INFO_FIELD_LEN + LISTEN_INTERVAL_FIELD_LEN)) {
        ASSERT(0);
        return WLAN_STATUS_FAILURE;
    }

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

    /* Check if this Disassoc Frame is coming from Target BSSID */
    if (UNEQUAL_MAC_ADDR((void *)prAssocReqFrame->aucBSSID, (void *)prBssInfo->aucBSSID)) {
        return WLAN_STATUS_FAILURE; /* Just Ignore this MMPDU */
    }

    //WLAN_GET_FIELD_16(&prAssocReqFrame->u2FrameCtrl, &u2RxFrameCtrl);
    u2RxFrameCtrl = prAssocReqFrame->u2FrameCtrl; // NOTE(Kevin): Optimized for ARM
    u2RxFrameCtrl &= MASK_FRAME_TYPE;
    if (MAC_FRAME_REASSOC_REQ == u2RxFrameCtrl) {
        prStaRec->fgIsReAssoc = TRUE;

        u2IELength = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
                     (UINT_16)(OFFSET_OF(WLAN_REASSOC_REQ_FRAME_T, aucInfoElem[0]) - WLAN_MAC_MGMT_HEADER_LEN);

        pucIEStart = pucIE = ((P_WLAN_REASSOC_REQ_FRAME_T)(prSwRfb->pvHeader))->aucInfoElem;
    } else {
        prStaRec->fgIsReAssoc = FALSE;

        u2IELength = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
                     (UINT_16)(OFFSET_OF(WLAN_ASSOC_REQ_FRAME_T, aucInfoElem[0]) - WLAN_MAC_MGMT_HEADER_LEN);

        pucIEStart = pucIE = prAssocReqFrame->aucInfoElem;
    }


    //4 <3> Parse the Fixed Fields of Assoc Req Frame Body.
    prStaRec->u2CapInfo = prAssocReqFrame->u2CapInfo;

#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK
    if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
        if (((prStaRec->u2CapInfo & CAP_INFO_PRIVACY) &&
                prAdapter->rP2pFuncLkr.prKalP2pGetCipher &&
                !prAdapter->rP2pFuncLkr.prKalP2pGetCipher(prAdapter->prGlueInfo))/* ||
                (((prStaRec->u2CapInfo & CAP_INFO_PRIVACY) == 0) &&
                prAdapter->rP2pFuncLkr.prKalP2pGetCipher &&
                prAdapter->rP2pFuncLkr.prKalP2pGetCipher(prAdapter->prGlueInfo))*/) {
            u2StatusCode = STATUS_CODE_CAP_NOT_SUPPORTED;
            DBGLOG(RSN, TRACE, ("STA Assoc req privacy bit check fail\n"));
            return WLAN_STATUS_SUCCESS;
        }
    }
#endif

    prStaRec->u2ListenInterval = prAssocReqFrame->u2ListenInterval;


    //4 <4> Parse the IE of Assoc Req Frame Body.
    IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
        switch (IE_ID(pucIE)) {
            case ELEM_ID_SSID:
                if ((!prIeSsid) && /* NOTE(Kevin): Get SSID once */
                        (IE_LEN(pucIE) <= ELEM_MAX_LEN_SSID)) {
                    prIeSsid = (P_IE_SSID_T)pucIE;
                }
                break;

            case ELEM_ID_SUP_RATES:
                if ((!prIeSupportedRate) && (IE_LEN(pucIE) <= RATE_NUM)) {
                    prIeSupportedRate = SUP_RATES_IE(pucIE);
                }
                break;

            case ELEM_ID_EXTENDED_SUP_RATES:
                if (!prIeExtSupportedRate) {
                    prIeExtSupportedRate = EXT_SUP_RATES_IE(pucIE);
                }
                break;
            case ELEM_ID_HT_CAP:
                prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_HT;
                break;
            case ELEM_ID_RSN:
#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK
                if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
                    prIeRsn = RSN_IE(pucIE);
                    rsnParserCheckForRSNCCMPPSK(prAdapter, prIeRsn, &u2StatusCode);
                    if (u2StatusCode != STATUS_CODE_SUCCESSFUL) {
                        *pu2StatusCode = u2StatusCode;
                        return WLAN_STATUS_SUCCESS;
                    }
                }
#endif
#if CFG_ENABLE_WIFI_TETHERING
                if (prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable && IS_STA_IN_P2P(prStaRec)) {
                    prIeRsn = RSN_IE(pucIE);
                    rsnParserCheckForRSNCCMPPSK(prAdapter, prIeRsn, &u2StatusCode);
                    if (u2StatusCode != STATUS_CODE_SUCCESSFUL) {
                        *pu2StatusCode = u2StatusCode;
                        return WLAN_STATUS_SUCCESS;
                    }
                }
#endif /* CFG_ENABLE_WIFI_TETHERING */
                break;

            default:
                for (i = 0; i < (sizeof(rxAssocReqIETable) / sizeof(VERIFY_IE_ENTRY_T)); i++) {

                    if ((IE_ID(pucIE)) == rxAssocReqIETable[i].ucElemID) {
                        rxAssocReqIETable[i].pfnVarifyIE(prAdapter, prSwRfb, (P_IE_HDR_T)pucIE, &u2StatusCode);

                        if (u2StatusCode != STATUS_CODE_SUCCESSFUL) {
                            *pu2StatusCode = u2StatusCode;
                            return WLAN_STATUS_SUCCESS;
                        }
                    }
                }

                break;
        }
    } /* end of IE_FOR_EACH */

    // parsing for WMM related information (2010/12/21)
    mqmProcessAssocReq(
        prAdapter,
        prSwRfb,
        pucIEStart,
        u2IELength);

    do {
        if (prIeSsid) {
            if (UNEQUAL_SSID((void *)prBssInfo->aucSSID, prBssInfo->ucSSIDLen,
                             (void *)prIeSsid->aucSSID, prIeSsid->ucLength)) {

                u2StatusCode = STATUS_CODE_UNSPECIFIED_FAILURE;
                break;
            }
        } else {
            u2StatusCode = STATUS_CODE_UNSPECIFIED_FAILURE;
            break;
        }

        prStaRec->u2OperationalRateSet = 0;
        prStaRec->u2BSSBasicRateSet = 0;

        if (prIeSupportedRate || prIeExtSupportedRate) {
            rateGetRateSetFromIEs(prIeSupportedRate,
                                  prIeExtSupportedRate,
                                  &prStaRec->u2OperationalRateSet,
                                  &u2BSSBasicRateSet, /* Ignore any Basic Bit */
                                  &fgIsUnknownBssBasicRate);

            if ((prBssInfo->u2BSSBasicRateSet & prStaRec->u2OperationalRateSet) !=
                    prBssInfo->u2BSSBasicRateSet) {

                u2StatusCode = STATUS_CODE_ASSOC_DENIED_RATE_NOT_SUPPORTED;
                break;
            }

            /* Accpet the Sta, update BSSBasicRateSet from Bss */

            prStaRec->u2BSSBasicRateSet = prBssInfo->u2BSSBasicRateSet;

            prStaRec->u2DesiredNonHTRateSet = (prStaRec->u2OperationalRateSet & RATE_SET_ALL_ABG);

            if (BAND_2G4 == HIF_RX_HDR_GET_RF_BAND(prSwRfb->prHifRxHdr)) {
                /* check if support 11n */
                if (!(u2BSSBasicRateSet & RATE_SET_BIT_HT_PHY)) {

                    if (prStaRec->u2OperationalRateSet & RATE_SET_OFDM) {
                        prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_ERP;
                    }


                    if (!(u2BSSBasicRateSet & RATE_SET_OFDM)) {
                        if (prStaRec->u2OperationalRateSet & RATE_SET_HR_DSSS) {
                            prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_HR_DSSS;
                        }

                    }

                }
            } else { /* (BAND_5G == prBssDesc->eBande) */
                if (!(u2BSSBasicRateSet & RATE_SET_BIT_HT_PHY)) {

                    prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_OFDM;
                }
                ASSERT((prStaRec->u2OperationalRateSet & RATE_SET_HR_DSSS) == 0);
            }

        } else {
            ASSERT(0);
            u2StatusCode = STATUS_CODE_ASSOC_DENIED_RATE_NOT_SUPPORTED;
            break;
        }

#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK
        if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
            if (prIeRsn) {
                if (prAdapter->rP2pFuncLkr.prKalP2pGetCipher &&
                        !prAdapter->rP2pFuncLkr.prKalP2pGetCipher(prAdapter->prGlueInfo)) {
                    u2StatusCode = STATUS_CODE_CIPHER_SUITE_REJECTED;
                    break;
                }
            } else {
                prStaRec->rSecInfo.fgAllowOnly1x = FALSE;
                if (prAdapter->rP2pFuncLkr.prKalP2pGetCipher &&
                        prAdapter->rP2pFuncLkr.prKalP2pGetCipher(prAdapter->prGlueInfo)) {
                    //Only Allow 1x
                    prStaRec->rSecInfo.fgAllowOnly1x = TRUE;
                    break;
                }
            }
        }
#endif

    } while (FALSE);

#if CFG_ENABLE_WIFI_DIRECT
    if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
        if (prAdapter->rP2pFuncLkr.prKalP2pUpdateAssocInfo) {
            prAdapter->rP2pFuncLkr.prKalP2pUpdateAssocInfo(prAdapter->prGlueInfo, (PUINT_8)&prAssocReqFrame->u2CapInfo, u2IELength + (prStaRec->fgIsReAssoc ? 10 : 4), prStaRec->fgIsReAssoc);
        } else {
            ASSERT(0);
        }
    }
#endif

    *pu2StatusCode = u2StatusCode;

    return WLAN_STATUS_SUCCESS;

} /* end of assocProcessRxAssocReqFrame() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used to compose Common Information Elements for Association
*        Response Frame.
*
* @param[in] prMsduInfo     Pointer to the composed MSDU_INFO_T.
* @param[in] prBssInfo      Pointer to the BSS_INFO_T.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
assocBuildReAssocRespFrameCommonIEs(
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfo,
    IN P_BSS_INFO_T prBssInfo
)
{
    PUINT_8 pucBuffer;
    P_STA_RECORD_T prStaRec;
    UINT_8 ucSupRatesLen;
    UINT_8 ucExtSupRatesLen;


    ASSERT(prMsduInfo);
    ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
    ASSERT(prStaRec);


    pucBuffer = (PUINT_8)((UINT_32)prMsduInfo->prPacket +
                          (UINT_32)prMsduInfo->u2FrameLength);
    ASSERT(pucBuffer);

    if (prBssInfo->ucAllSupportedRatesLen > ELEM_MAX_LEN_SUP_RATES) {

        ucSupRatesLen = ELEM_MAX_LEN_SUP_RATES;
        ucExtSupRatesLen = prBssInfo->ucAllSupportedRatesLen - ELEM_MAX_LEN_SUP_RATES;
    } else {
        ucSupRatesLen = prBssInfo->ucAllSupportedRatesLen;
        ucExtSupRatesLen = 0;
    }

    /* Fill the Supported Rates element. */
    if (ucSupRatesLen) {
        SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_SUP_RATES;
        SUP_RATES_IE(pucBuffer)->ucLength = ucSupRatesLen;
        kalMemCopy(SUP_RATES_IE(pucBuffer)->aucSupportedRates,
                   prBssInfo->aucAllSupportedRates,
                   ucSupRatesLen);

        prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
        pucBuffer += IE_SIZE(pucBuffer);
    }


    /* Fill the Extended Supported Rates element. */
    if (ucExtSupRatesLen) {

        EXT_SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_EXTENDED_SUP_RATES;
        EXT_SUP_RATES_IE(pucBuffer)->ucLength = ucExtSupRatesLen;

        kalMemCopy(EXT_SUP_RATES_IE(pucBuffer)->aucExtSupportedRates,
                   &prBssInfo->aucAllSupportedRates[ucSupRatesLen],
                   ucExtSupRatesLen);

        prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
    }

    return;
} /* end of assocBuildReAssocRespFrameCommonIEs() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will compose the (Re)Association Response frame
*
* @param[in] prStaRec               Pointer to the STA_RECORD_T
* @param[in] pucBuffer              Pointer to the frame buffer.
* @param[in] aucBssid               Given BSSID.
* @param[in] u2CapInfo              Capability Field of current BSS.
* @param[in out] pu2PayloadLen      Return the length of the composed fixed fields
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
assocComposeReAssocRespFrameHeaderAndFF(
    IN P_STA_RECORD_T prStaRec,
    IN PUINT_8 pucBuffer,
    IN UINT_8 aucBSSID[],
    IN UINT_16 u2CapInfo,
    IN OUT PUINT_16 pu2PayloadLen
)
{
    P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
    BOOLEAN fgIsReAssoc;

    UINT_16 u2FrameCtrl;


    ASSERT(prStaRec);
    ASSERT(pucBuffer);
    ASSERT(aucBSSID);
    ASSERT(pu2PayloadLen);

    prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T)pucBuffer;
    fgIsReAssoc = prStaRec->fgIsReAssoc;

    //4 <1> Compose the frame header of the (Re)Association Request  frame.
    /* Fill the Frame Control field. */
    if (fgIsReAssoc) {
        u2FrameCtrl = MAC_FRAME_REASSOC_RSP;
    } else {
        u2FrameCtrl = MAC_FRAME_ASSOC_RSP;
    }
    //WLAN_SET_FIELD_16(&prAssocFrame->u2FrameCtrl, u2FrameCtrl);
    prAssocRspFrame->u2FrameCtrl = u2FrameCtrl; // NOTE(Kevin): Optimized for ARM

    /* Fill the DA field with Target MAC Address. */
    COPY_MAC_ADDR(prAssocRspFrame->aucDestAddr, prStaRec->aucMacAddr);

    /* Fill the SA field with current BSSID. */
    COPY_MAC_ADDR(prAssocRspFrame->aucSrcAddr, aucBSSID);

    /* Fill the BSSID field with current BSSID. */
    COPY_MAC_ADDR(prAssocRspFrame->aucBSSID, aucBSSID);

    /* Clear the SEQ/FRAG_NO field(HW won't overide the FRAG_NO, so we need to clear it). */
    prAssocRspFrame->u2SeqCtrl = 0;


    //4 <2> Compose the frame body's common fixed field part of the (Re)Association Request  frame.
    /* Fill the Capability Information field. */
    //WLAN_SET_FIELD_16(&prAssocFrame->u2CapInfo, u2CapInfo);
    prAssocRspFrame->u2CapInfo = u2CapInfo; // NOTE(Kevin): Optimized for ARM

    //WLAN_SET_FIELD_16(&prAssocFrame->u2StatusCode, prStaRec->u2StatusCode);
    prAssocRspFrame->u2StatusCode = prStaRec->u2StatusCode; // NOTE(Kevin): Optimized for ARM

    //WLAN_SET_FIELD_16(&prAssocFrame->u2AssocId, ((prStaRec->u2AssocId & AID_MASK) | AID_MSB));
    prAssocRspFrame->u2AssocId =
        ((prStaRec->u2AssocId & AID_MASK) | AID_MSB); // NOTE(Kevin): Optimized for ARM

    *pu2PayloadLen = (CAP_INFO_FIELD_LEN +
                      STATUS_CODE_FIELD_LEN +
                      AID_FIELD_LEN);

    return;
} /* end of assocComposeReAssocRespFrameHeaderAndFF() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will send the (Re)Association Resp frame
*
* @param[in] prStaRec           Pointer to the STA_RECORD_T
*
* @retval WLAN_STATUS_RESOURCES No available resource for frame composing.
* @retval WLAN_STATUS_SUCCESS   Successfully send frame to TX Module
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
assocSendReAssocRespFrame(
    IN P_ADAPTER_T      prAdapter,
    IN P_STA_RECORD_T   prStaRec
)
{
    P_BSS_INFO_T prBssInfo;
    P_MSDU_INFO_T prMsduInfo;

    UINT_16 u2PayloadLen;
    UINT_16 u2EstimatedFrameLen;
    UINT_16 u2EstimatedExtraIELen;
    //BOOLEAN fgIsReAssoc;
    UINT_32 i;

    DEBUGFUNC("assocSendReAssocRespFrame");
    ASSERT(prStaRec);


    //4 <1> Allocate a PKT_INFO_T for Authentication Frame
    //fgIsReAssoc = prStaRec->fgIsReAssoc;

    /* Init with MGMT Header Length + Length of Fixed Fields + Common IE Length */
    u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD + \
                          WLAN_MAC_MGMT_HEADER_LEN + \
                          CAP_INFO_FIELD_LEN + \
                          STATUS_CODE_FIELD_LEN + \
                          AID_FIELD_LEN + \
                          (ELEM_HDR_LEN + ELEM_MAX_LEN_SUP_RATES) + \
                          (ELEM_HDR_LEN + (RATE_NUM - ELEM_MAX_LEN_SUP_RATES));

    /* + Extra IE Length */
    u2EstimatedExtraIELen = 0;

    for (i = 0; i < sizeof(txAssocRespIETable) / sizeof(APPEND_IE_ENTRY_T); i++) {
        u2EstimatedExtraIELen += txAssocRespIETable[i].u2EstimatedIELen;
    }

    u2EstimatedFrameLen += u2EstimatedExtraIELen;

    /* Allocate a MSDU_INFO_T */
    if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
        DBGLOG(AAA, WARN, ("No PKT_INFO_T for sending (Re)Assoc Response.\n"));
        return WLAN_STATUS_RESOURCES;
    }

    //4 <2> Compose (Re)Association Request frame header and fixed fields in MSDU_INfO_T.
    ASSERT(prStaRec->ucNetTypeIndex != NETWORK_TYPE_AIS_INDEX);
    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

    /* Compose Header and Fixed Field */
    assocComposeReAssocRespFrameHeaderAndFF(prStaRec,
                                            (PUINT_8)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD),
                                            prBssInfo->aucBSSID,
                                            prBssInfo->u2CapInfo,
                                            &u2PayloadLen);

    //4 <3> Update information of MSDU_INFO_T
    ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

    prMsduInfo->eSrc = TX_PACKET_MGMT;
    prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
    prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
    prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
    prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
    prMsduInfo->fgIs802_1x = FALSE;
    prMsduInfo->fgIs802_11 = TRUE;
    prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
    prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
    prMsduInfo->pfTxDoneHandler = aaaFsmRunEventTxDone;
    prMsduInfo->fgIsBasicRate = TRUE;

    //4 <4> Compose the frame body's IEs of the (Re)Association Request  frame.
    assocBuildReAssocRespFrameCommonIEs(prAdapter, prMsduInfo, prBssInfo);


    //4 <5> Compose IEs in MSDU_INFO_T

    /* Append IE */
    for (i = 0; i < sizeof(txAssocRespIETable) / sizeof(APPEND_IE_ENTRY_T); i++) {
        if (txAssocRespIETable[i].pfnAppendIE) {
            txAssocRespIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
        }
    }

    /* TODO(Kevin): Also release the unused tail room of the composed MMPDU */

    //4 <6> Enqueue the frame to send this (Re)Association request frame.
    nicTxEnqueueMsdu(prAdapter, prMsduInfo);

    return WLAN_STATUS_SUCCESS;

} /* end of assocSendReAssocRespFrame() */
#endif /* CFG_SUPPORT_AAA */


