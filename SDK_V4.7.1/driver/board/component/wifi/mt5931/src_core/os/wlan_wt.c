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

/******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
*******************************************************************************
*/
#include "precomp.h"
#include "wndrv_context.h"
#include "wlan_typedef.h"

#if CFG_ENABLE_WIFI_TETHERING

extern void wndrv_send_hostap_set_channel_cnf_handler(kal_bool status);
extern void wndrv_send_hostap_set_privacy_cnf_handler(void);
extern void wndrv_send_hostap_add_sta_cnf_handler(kal_bool status, PUINT_8 pucStaAddr);

VOID dumpBss(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

extern wndrv_context_struct wndrv_context;


/******************************************************************************
*                              C O N S T A N T S
*******************************************************************************
*/

/******************************************************************************
*                             D A T A   T Y P E S
*******************************************************************************
*/

/******************************************************************************
*                            P U B L I C   D A T A
*******************************************************************************
*/

/******************************************************************************
*                           P R I V A T E   D A T A
*******************************************************************************
*/

/******************************************************************************
*                                 M A C R O S
*******************************************************************************
*/

/******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************
*/

/******************************************************************************
*                              F U N C T I O N S
*******************************************************************************
*/

UINT_16
wtGetStaRecNumByNetType(
    IN P_ADAPTER_T                 prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex
)
{
    P_STA_RECORD_T  prStaRec;
    UINT_16         i;
    UINT_16         u2RecNum = 0;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT, "[TETHERING]wtGetStaRecNumByNetType");

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = (P_STA_RECORD_T) &prAdapter->arStaRec[i];

        if (prStaRec->fgIsInUse &&
                prStaRec->ucNetTypeIndex == (UINT_8) eNetTypeIndex) {
            u2RecNum++;
        }
    } /* end of for loop */

    return u2RecNum;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indiate the Event of Tx Fail of AAA Module.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prStaRec           Pointer to the STA_RECORD_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wtRunEventAAATxFail(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
)
{
    P_BSS_INFO_T prBssInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT, "[TETHERING]wtRunEventAAATxFail");

    ASSERT(prAdapter);
    ASSERT(prStaRec);

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

    wtFuncDisconnect(prAdapter, prStaRec, FALSE, REASON_CODE_UNSPECIFIED);
    bssRemoveStaRecFromClientList(prAdapter, prBssInfo, prStaRec);

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indiate the Event of Successful Completion of AAA Module.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prStaRec           Pointer to the STA_RECORD_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wtRunEventAAAComplete(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
)
{
    P_BSS_INFO_T prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT, "[TETHERING]wtRunEventAAAComplete");
    ASSERT(prStaRec);

    bssRemoveStaRecFromClientList(prAdapter, prBssInfo, prStaRec);

    bssAddStaRecToClientList(prAdapter, prBssInfo, prStaRec);

    prStaRec->u2AssocId = bssAssignAssocID(prStaRec);

    /*Update StaRec to State3*/
    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);

    /*Update BssInfo to connected*/
    wtChangeMediaState(prAdapter, PARAM_MEDIA_STATE_CONNECTED);
    nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);

    dumpSTA(prAdapter, prStaRec);

    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_240, "[TETHERING]AAA complete ["MACSTR"]\n", MAC2STR(prStaRec->aucMacAddr));
#if 0
    if (prStaRec->fgIsAddStaReq == KAL_TRUE) {
        wndrv_send_hostap_add_sta_cnf_handler(KAL_TRUE, prStaRec->aucMacAddr);
    }
#endif
    return WLAN_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will indiate the Event of Successful Completion of AAA Module.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prStaRec           Pointer to the STA_RECORD_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wtRunEventAAASuccess (
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
    )
{

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtRunEventAAASuccess");

    if (prStaRec->fgIsAddStaReq == KAL_TRUE) {
        wndrv_send_hostap_add_sta_cnf_handler(KAL_TRUE, prStaRec->aucMacAddr);
    }

    return WLAN_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will handle RxDeauth
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prStaRec           Pointer to the STA_RECORD_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wtRunEventRxDeAuth (
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN P_SW_RFB_T prSwRfb
    )
{
    P_BSS_INFO_T prBssInfo;
    UINT_16 u2ReasonCode = 0;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtRunEventRxDeAuth");
    ASSERT(prAdapter);

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

/* Delete client from client list. */
    if (authProcessRxDeauthFrame(prSwRfb,
            prBssInfo->aucBSSID,
            &u2ReasonCode) == WLAN_STATUS_SUCCESS) {
        P_LINK_T prStaRecOfClientList = (P_LINK_T)NULL;
        P_LINK_ENTRY_T prLinkEntry = (P_LINK_ENTRY_T)NULL;
        P_STA_RECORD_T prCurrStaRec = (P_STA_RECORD_T)NULL;

        prStaRecOfClientList = &prBssInfo->rStaRecOfClientList;

        LINK_FOR_EACH(prLinkEntry, prStaRecOfClientList) {
            prCurrStaRec = LINK_ENTRY(prLinkEntry, STA_RECORD_T, rLinkEntry);

            ASSERT(prCurrStaRec);

            if (EQUAL_MAC_ADDR(prCurrStaRec->aucMacAddr, prStaRec->aucMacAddr)) {

                /* Remove STA from client list. */
                LINK_REMOVE_KNOWN_ENTRY(prStaRecOfClientList, &prCurrStaRec->rLinkEntry);

                /* Indicate disconnect to Host. */
                wtFuncDisconnect(prAdapter, prStaRec, FALSE, u2ReasonCode);

                MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_241, "Disconnect STA:"MACSTR, MAC2STR(prStaRec->aucMacAddr));

                break;
            }
        }
    }

#if 0
    if(!IS_STA_IN_P2P(prStaRec)) {
       return WLAN_STATUS_NOT_ACCEPTED;
    }

    if (prStaRec->ucStaState > STA_STATE_1) {

        if (STA_STATE_3 == prStaRec->ucStaState) {

                /* NOTE(Kevin): Change state immediately to avoid starvation of
                            * MSG buffer because of too many deauth frames before changing
                            * the STA state.
                            */
                cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
        }

        return WLAN_STATUS_SUCCESS;
    }
#endif
    return WLAN_STATUS_NOT_ACCEPTED;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is call when RX deauthentication frame from the AIR.
*             If we are under STA mode, we would go back to P2P Device.
*             If we are under AP mode, we would stay in AP mode until disconnect event from HOST.
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
wtRunEventRxDisassociation (
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN P_SW_RFB_T prSwRfb
    )
{
    P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;
    UINT_16 u2ReasonCode = 0;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtRunEventRxDisassociation");

    do {
        ASSERT((prAdapter != NULL) && (prSwRfb != NULL));

        if (prStaRec == NULL) {
            prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
        }


        prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

        if (prStaRec->ucStaState == STA_STATE_1) {

            break;
        }
        /* Delete client from client list. */
        if (assocProcessRxDisassocFrame(prAdapter,
                prSwRfb,
                prBssInfo->aucBSSID,
                &u2ReasonCode) == WLAN_STATUS_SUCCESS) {
            P_LINK_T prStaRecOfClientList = (P_LINK_T)NULL;
            P_LINK_ENTRY_T prLinkEntry = (P_LINK_ENTRY_T)NULL;
            P_STA_RECORD_T prCurrStaRec = (P_STA_RECORD_T)NULL;

            prStaRecOfClientList = &prBssInfo->rStaRecOfClientList;

            LINK_FOR_EACH(prLinkEntry, prStaRecOfClientList) {
                prCurrStaRec = LINK_ENTRY(prLinkEntry, STA_RECORD_T, rLinkEntry);

                ASSERT(prCurrStaRec);

                if (EQUAL_MAC_ADDR(prCurrStaRec->aucMacAddr, prStaRec->aucMacAddr)) {

                    /* Remove STA from client list. */
                    LINK_REMOVE_KNOWN_ENTRY(prStaRecOfClientList, &prCurrStaRec->rLinkEntry);

                    /* Indicate disconnect to Host. */
                    wtFuncDisconnect(prAdapter, prStaRec, FALSE, u2ReasonCode);

                    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_242, "Disconnect STA:"MACSTR, MAC2STR(prStaRec->aucMacAddr));

                    break;
                }
            }
        }

    } while (FALSE);

    return;

}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will validate the Rx Auth Frame and then return
*        the status code to AAA to indicate if need to perform following actions
*        when the specified conditions were matched.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prSwRfb            Pointer to SW RFB data structure.
* @param[in] pprStaRec          Pointer to pointer of STA_RECORD_T structure.
* @param[out] pu2StatusCode     The Status Code of Validation Result
*
* @retval TRUE      Reply the Auth
* @retval FALSE     Don't reply the Auth
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wtValidateAuth (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    IN PP_STA_RECORD_T pprStaRec,
    OUT PUINT_16 pu2StatusCode
    )
{
    BOOLEAN fgReplyAuth = TRUE;
    P_BSS_INFO_T prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    P_WLAN_AUTH_FRAME_T prAuthFrame = (P_WLAN_AUTH_FRAME_T)NULL;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING][Rx_Auth]wtValidateAuth");

    do {
        *pu2StatusCode = STATUS_CODE_REQ_DECLINED;

        prAuthFrame = (P_WLAN_AUTH_FRAME_T) prSwRfb->pvHeader;

        prStaRec = cnmGetStaRecByAddress(prAdapter,
                            (UINT_8) NETWORK_TYPE_P2P_INDEX,
                            (PUINT_8)prAuthFrame->aucSrcAddr);

        if (!prStaRec) {
            /* add new STA_RECORD_T for client */
            prStaRec = cnmStaRecAlloc(prAdapter,
                            (UINT_8) NETWORK_TYPE_P2P_INDEX);

            MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_243, "[TETHERING][Rx_Auth]wtValidateAuth: cnmStaRecAlloc ucIndex %d", prStaRec->ucIndex);

            if(prStaRec->ucIndex >= CFG_NUM_OF_STA_RECORD) {
                 prStaRec->fgIsInUse = FALSE;
                 fgReplyAuth = FALSE;
                 
				 MT5931_WAP_TRACE(MOD_WNDRV,TRACE_WARNING, "[TETHERING][Rx_Auth]wtValidateAuth: cnmStaRecAlloc ucIndex(%d) reach MaxMum", prStaRec->ucIndex);
                 return fgReplyAuth;
            }
            /* TODO(Kevin): Error handling of allocation of STA_RECORD_T for
             * exhausted case and do removal of unused STA_RECORD_T.
             */
            ASSERT(prStaRec);

            COPY_MAC_ADDR(prStaRec->aucMacAddr, prAuthFrame->aucSrcAddr);
            prSwRfb->ucStaRecIdx = prStaRec->ucIndex;
            prBssInfo->prStaRecOfAP = prStaRec;
            prStaRec->u2DesiredNonHTRateSet = prBssInfo->u2BSSBasicRateSet;
            prStaRec->u2OperationalRateSet = prBssInfo->u2OperationalRateSet;
            prStaRec->ucPhyTypeSet = prBssInfo->ucPhyTypeSet;
            prStaRec->eStaType = STA_TYPE_LEGACY_CLIENT;
            prStaRec->ucNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
            /* Update Station Record - Status/Reason Code */
            prStaRec->u2StatusCode = STATUS_CODE_SUCCESSFUL;
            prStaRec->ucJoinFailureCount = 0;

            /* NOTE(Kevin): Better to change state here, not at TX Done */
            cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

            MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_244, "[TETHERING][Rx_Auth]wtValidateAuth: cnmStaRecChangeState STA_STATE_1");
        }
        else
        {
            MT5931_TRACE(TRACE_WARNING, MT5931_INFO_245, "[TETHERING][Rx_Auth]wtValidateAuth: prStaRec->ucIndex = %x", prStaRec->ucIndex);
            prSwRfb->ucStaRecIdx = prStaRec->ucIndex;

            if ((prStaRec->ucStaState > STA_STATE_1) && (IS_STA_IN_P2P(prStaRec))) {
                 /* Change station state. */
                 cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
                 wtResetStaRecStatus(prAdapter, prStaRec);
                 bssRemoveStaRecFromClientList(prAdapter, prBssInfo, prStaRec);
            }

        }

        if (prBssInfo->rStaRecOfClientList.u4NumElem > WT_MAXIMUM_CLIENT_COUNT)  {
            cnmStaRecFree(prAdapter, prStaRec, FALSE);
            MT5931_TRACE(TRACE_WARNING, MT5931_INFO_246, "[TETHERING][Rx_Auth]wtValidateAuth: STA REC Num %d reach LIMIT(%d)",
                prBssInfo->rStaRecOfClientList.u4NumElem, WT_MAXIMUM_CLIENT_COUNT);
            break;
        }
        /* for hostap lookback modification */
        prStaRec->fgIsAuthLoopback = FALSE;
        prStaRec->fgIsAssocLoopback = FALSE;

        *pprStaRec = prStaRec;
        *pu2StatusCode = STATUS_CODE_SUCCESSFUL;

        MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_247, "[TETHERING][Rx_Auth]wtValidateAuth: fgReplyAuth, %x", fgReplyAuth);
    }while(FALSE);
    return fgReplyAuth;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will validate the Rx Assoc Req Frame and then return
*        the status code to AAA to indicate if need to perform following actions
*        when the specified conditions were matched.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prSwRfb            Pointer to SW RFB data structure.
* @param[out] pu2StatusCode     The Status Code of Validation Result
*
* @retval TRUE      Reply the Assoc Resp
* @retval FALSE     Don't reply the Assoc Resp
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wtValidateAssocReq (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT PUINT_16 pu2StatusCode
    )
{
    BOOLEAN fgReplyAssocResp = FALSE;
    P_BSS_INFO_T prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    P_WLAN_ASSOC_REQ_FRAME_T prAssocReqFrame = (P_WLAN_ASSOC_REQ_FRAME_T)NULL;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING][Rx_Assoc]wtValidateAssocReq");

    ASSERT(prAdapter);

    prAssocReqFrame = (P_WLAN_ASSOC_REQ_FRAME_T) prSwRfb->pvHeader;

    /*Update StaRec*/
    prStaRec = cnmGetStaRecByAddress(prAdapter,
                        (UINT_8) NETWORK_TYPE_P2P_INDEX,
                        (PUINT_8)prAssocReqFrame->aucSrcAddr);


    /*Assoc Accept*/
    if (prStaRec) {

        prStaRec->eStaType = STA_TYPE_LEGACY_CLIENT;
        prStaRec->u2DesiredNonHTRateSet &= prBssInfo->u2OperationalRateSet;
        if (!(prStaRec->u2DesiredNonHTRateSet &~ BASIC_RATE_SET_HR_DSSS_ERP) && /* CCK 1/2/5.5/11M */
            !(prAssocReqFrame->u2FrameCtrl & CAP_INFO_SHORT_PREAMBLE)) /* Long preamble */
        {
            prStaRec->ucPhyTypeSet = PHY_TYPE_SET_802_11B;
        }
        else {
        prStaRec->ucDesiredPhyTypeSet = prStaRec->ucPhyTypeSet & prBssInfo->ucPhyTypeSet;
        }
        prStaRec->ucNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
        prStaRec->ucRCPI = prSwRfb->prHifRxHdr->ucRcpi;

        /*Undpate BssInfo to FW*/
        //wtChangeMediaState(prAdapter, PARAM_MEDIA_STATE_CONNECTED);
        //nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);

        /*reply successful*/
        *pu2StatusCode = STATUS_CODE_SUCCESSFUL;
        fgReplyAssocResp = TRUE;
    }
    /*Reject Assoc*/
    else {
        /* cannot find StaRec */
        /* Station record should be ready while RX AUTH frame. */
        *pu2StatusCode = STATUS_CODE_REQ_DECLINED;
        fgReplyAssocResp = FALSE;
        MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_248, "RX_ASSOC: prStaRec = NULL");
    }
    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_249, "[TETHERING][Rx_Assoc]wtValidateAssocReq: fgReplyAssocResp, %x", fgReplyAssocResp);
    return fgReplyAssocResp;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will validate the Rx Probe Request Frame and then return
*        result to BSS to indicate if need to send the corresponding Probe Response
*        Frame if the specified conditions were matched.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] prSwRfb            Pointer to SW RFB data structure.
* @param[out] pu4ControlFlags   Control flags for replying the Probe Response
*
* @retval TRUE      Reply the Probe Response
* @retval FALSE     Don't reply the Probe Response
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wtValidateProbeReq (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT PUINT_32 pu4ControlFlags
    )
{
    BOOLEAN fgIsReplyProbeRsp = FALSE;
    PUINT_8 pucIE = NULL;
    UINT_16 u2IELength = 0, u2Offset = 0;
    P_IE_SSID_T prIeSsid = (P_IE_SSID_T)NULL;
    P_IE_SUPPORTED_RATE_T prIeSupportedRate = (P_IE_SUPPORTED_RATE_T)NULL;
    P_IE_EXT_SUPPORTED_RATE_T prIeExtSupportedRate = (P_IE_EXT_SUPPORTED_RATE_T)NULL;
    P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtValidateProbeReq");

    do {

        ASSERT((prAdapter != NULL) && (prSwRfb != NULL));

        /*------------------Check Probe Request Frame------------------*/
        pucIE = (PUINT_8)((UINT_32)prSwRfb->pvHeader + prSwRfb->u2HeaderLen);
        u2IELength = prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen;

        /* Check RSP Probe Request*/
        IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
            switch (IE_ID(pucIE)) {
            case ELEM_ID_SSID:
                if ((!prIeSsid) &&
                    (IE_LEN(pucIE) <= ELEM_MAX_LEN_SSID)) {
                    prIeSsid = (P_IE_SSID_T)pucIE;
                }
                break;
            case ELEM_ID_SUP_RATES:
                /* NOTE(Kevin): Buffalo WHR-G54S's supported rate set IE exceed 8.
                 * IE_LEN(pucIE) == 12, "1(B), 2(B), 5.5(B), 6(B), 9(B), 11(B),
                 * 12(B), 18(B), 24(B), 36(B), 48(B), 54(B)"
                 */
                // if (IE_LEN(pucIE) <= ELEM_MAX_LEN_SUP_RATES) {
                if ((!prIeSupportedRate) && (IE_LEN(pucIE) <= RATE_NUM)) {
                    prIeSupportedRate = SUP_RATES_IE(pucIE);
                }
                break;
            case ELEM_ID_EXTENDED_SUP_RATES:
                if (!prIeExtSupportedRate)
                    prIeExtSupportedRate = EXT_SUP_RATES_IE(pucIE);
                break;
            default:
                break;
            }

        } /* end of IE_FOR_EACH */

        /* P2P 2.4.1 - P2P Devices shall not respond to Probe Request frames which
          * only contain 11b rates only.
          */
        if (prIeSupportedRate || prIeExtSupportedRate) {
            UINT_16 u2OperationalRateSet, u2BSSBasicRateSet;
            BOOLEAN fgIsUnknownBssBasicRate;

            rateGetRateSetFromIEs(prIeSupportedRate,
                                  prIeExtSupportedRate,
                                  &u2OperationalRateSet,
                                  &u2BSSBasicRateSet, /* Ignore any Basic Bit */
                                  &fgIsUnknownBssBasicRate);

            if ((u2OperationalRateSet & ~RATE_SET_HR_DSSS) == 0) {
                MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_250, "[TETHERING]Probe Request Rate Set only contains 11B rate");
                break; /* Break While. */
            }
        }

        if (!prIeSsid) {
            MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_251, "[TETHERING]prIeSsid = NULL");
            break;
        }

        prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX];

        /* P2P 3.1.2.1.1 !VShall only reply to the Probe Request which contains,
          * P2P Wildcard SSID, Wild card BSSID.
          */
        if (!(((prIeSsid->ucLength == BC_SSID_LEN) || /* WILDCARD SSID */
                EQUAL_SSID((void *)prBssInfo->aucSSID, prBssInfo->ucSSIDLen, /* CURRENT SSID */
                          (void *)prIeSsid->aucSSID, prIeSsid->ucLength)))) {
            MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_252, "[TETHERING]P2P SSID not Wildcard SSID or not P2P Wildcard SSID");
            break; /* Break While. */
        }
        /*------------------End Check Probe Request Frame------------------*/

        switch (prBssInfo->eCurrentOPMode) {
        case OP_MODE_ACCESS_POINT:
            fgIsReplyProbeRsp = TRUE;
            break;
        default:
            //ASSERT(FALSE);
            break;
        }

    } while (FALSE);

    return fgIsReplyProbeRsp;

} /* end of p2pFuncValidateProbeReq() */


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is to inform CNM for channel privilege requesting
*           has been released
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
wtRequestCh (
    IN P_ADAPTER_T prAdapter
    )
{
    P_MSG_CH_REQ_T prMsgChReq;
    P_GL_WT_INFO   prWtInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtRequestCh");

    ASSERT(prAdapter);

    prWtInfo = &(prAdapter->prGlueInfo->rWtInfo);

    /* send message to CNM for acquiring channel */
    prMsgChReq = (P_MSG_CH_REQ_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_CH_REQ_T));

    if (!prMsgChReq) {
        ASSERT(0); // Can't indicate CNM for channel acquiring
        return;
    }

    prMsgChReq->rMsgHdr.eMsgId = MID_MNY_CNM_CH_REQ; /* call cnmChMngrRequestPrivilege() */
    prMsgChReq->ucNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
    prMsgChReq->ucTokenID = ++prWtInfo->ucSeqNumOfChReq;
    prMsgChReq->eReqType = CH_REQ_TYPE_JOIN;
    prMsgChReq->u4MaxInterval = 1;
    prMsgChReq->ucPrimaryChannel = prWtInfo->ucPrimaryChannel;
    prMsgChReq->eRfSco = CHNL_EXT_SCN;
    prMsgChReq->eRfBand = BAND_2G4; /* 5931 only 2.4G */

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prMsgChReq,
                MSG_SEND_METHOD_BUF);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is to inform CNM that channel privilege is granted
*           has been released
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
wtReleaseCh (
    IN P_ADAPTER_T prAdapter
    )
{
    P_MSG_CH_ABORT_T prMsgChAbort;
    P_GL_WT_INFO   prWtInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtReleaseCh");

    ASSERT(prAdapter);

    prWtInfo = &(prAdapter->prGlueInfo->rWtInfo);

    /* 1. return channel privilege to CNM immediately */
    prMsgChAbort = (P_MSG_CH_ABORT_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_CH_ABORT_T));
    if (!prMsgChAbort) {
        ASSERT(0); // Can't release Channel to CNM
        return;
    }

    prMsgChAbort->rMsgHdr.eMsgId = MID_MNY_CNM_CH_ABORT;
    prMsgChAbort->ucNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
    prMsgChAbort->ucTokenID = prWtInfo->ucSeqNumOfChReq;

    mboxSendMsg(prAdapter,
                MBOX_ID_0,
                (P_MSG_HDR_T) prMsgChAbort,
                MSG_SEND_METHOD_BUF);

    return;
} /* end of aisFsmReleaseCh() */

/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is invoked when CNM granted channel privilege
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
wtRunEventChGrant (
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
    )
{
    P_MSG_CH_GRANT_T prMsgChGrant;
    P_GL_WT_INFO     prWtInfo;
    UINT_8 ucTokenID;
    UINT_32 u4GrantInterval;
	P_BSS_INFO_T prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

    //DEBUGFUNC("wtRunEventChGrant");

    ASSERT(prAdapter);
    ASSERT(prMsgHdr);

    /* invoke by Rx EVENT_ID_CH_PRIVILEGE event packet */
    prWtInfo = &(prAdapter->prGlueInfo->rWtInfo);
    prMsgChGrant = (P_MSG_CH_GRANT_T)prMsgHdr;
    ucTokenID = prMsgChGrant->ucTokenID;
    u4GrantInterval = prMsgChGrant->u4GrantInterval;

    prAdapter->prGlueInfo->rWtInfo.ucPrimaryChannel = prMsgChGrant->ucPrimaryChannel;

    /* 1. free message */
    cnmMemFree(prAdapter, prMsgHdr);

    if(prWtInfo->ucSeqNumOfChReq == ucTokenID) {
        /* channel privilege has been approved */
        MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_253, "wtRunEventChGrant: [u4GrantInterval %d]", u4GrantInterval);

		if( wndrv_context.glue_p->rWtInfo.fgIsInit == TRUE ) {

				prBssInfo->ucNonHTBasicPhyType = (UINT_8)  rNonHTApModeAttributes[prBssInfo->ucConfigAdHocAPMode].ePhyTypeIndex;
    		prBssInfo->u2BSSBasicRateSet =  rNonHTApModeAttributes[prBssInfo->ucConfigAdHocAPMode].u2BSSBasicRateSet;
    		prBssInfo->u2OperationalRateSet = rNonHTPhyAttributes[prBssInfo->ucNonHTBasicPhyType].u2SupportedRateSet;


    		rateGetDataRatesFromRateSet(prBssInfo->u2OperationalRateSet,
                                    prBssInfo->u2BSSBasicRateSet,
                                    prBssInfo->aucAllSupportedRates,
                                    &prBssInfo->ucAllSupportedRatesLen);
	
	    	nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);

	    	nicPmIndicateBssCreated(prAdapter, NETWORK_TYPE_P2P_INDEX);

	    	/* Update FW BCN content */
	    	bssUpdateBeaconContent(prAdapter, NETWORK_TYPE_P2P_INDEX);

	    	/* Indicate CNM a new network is created */
	    	wtReleaseCh(prAdapter);

				wndrv_send_hostap_set_privacy_cnf_handler();//by Jerry

				return;
		}
		/* Init AP mode */
        wtInit(prAdapter);

    }
    else { /* mismatched grant */
        MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_254, "wtRunEventChGrant: [ucSeqNumOfChReq:%d][ucTokenID:%d]",
            prWtInfo->ucSeqNumOfChReq, ucTokenID);

        /* return channel privilege to CNM immediately */
        wtReleaseCh(prAdapter);

        /* request channel fail */
        //wndrv_send_hostap_set_channel_cnf_handler(FALSE);
    }

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief    This function is to init WiFi Tethering AP Mode
*
* \param[in] prAdapter  Pointer of ADAPTER_T
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
wtInit(
    IN P_ADAPTER_T prAdapter
    )
{
    P_BSS_INFO_T prBssInfo;
    P_PARAM_KEY_T prKey = (P_PARAM_KEY_T) &prAdapter->prGlueInfo->rWtInfo.GTKkeyBuffer[0];
    UINT_32       setInfoLen;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtInit");

    /* Update BSS_INFO_T */
    /* Setup OP Mode */
    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);
    prBssInfo->ucNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
    prBssInfo->eCurrentOPMode = OP_MODE_ACCESS_POINT;

    /* Setup MAC/BSSID */
    COPY_MAC_ADDR(prBssInfo->aucOwnMacAddr, prAdapter->rWifiVar.aucDeviceAddress);
    COPY_MAC_ADDR(prBssInfo->aucBSSID, prAdapter->rWifiVar.aucDeviceAddress);

    /* SSID setup in hostap_wndrv_set_essid_req_handler */
    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_255, "prBssInfo->aucOwnMacAddr: "MACSTR, MAC2STR(prBssInfo->aucOwnMacAddr));
    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_256, "prBssInfo->aucBSSID: "MACSTR, MAC2STR(prBssInfo->aucBSSID));
    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_257, "prAdapter->rWifiVar.aucDeviceAddress: "MACSTR, MAC2STR(prAdapter->rWifiVar.aucDeviceAddress));

    /* clear current AP's STA_RECORD_T and current AID */
    prBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;
    prBssInfo->u2AssocId = 0;

    /* Setup Channel/Band/Phy Attri
       Only 802.11bg support now!
    */
    //4TODO: 802.11n support ???
    prBssInfo->ucPrimaryChannel = prAdapter->prGlueInfo->rWtInfo.ucPrimaryChannel;
    prBssInfo->eBand = BAND_2G4;
	#if 0
    prBssInfo->ucConfigAdHocAPMode = AP_MODE_MIXED_11BG;

    prBssInfo->ucPhyTypeSet = prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11BG; /* Depend on eBand */
	#endif
    prBssInfo->eBssSCO = CHNL_EXT_SCN;
    prBssInfo->ucNonHTBasicPhyType = (UINT_8)
            rNonHTApModeAttributes[prBssInfo->ucConfigAdHocAPMode].ePhyTypeIndex;
    prBssInfo->u2BSSBasicRateSet =
            rNonHTApModeAttributes[prBssInfo->ucConfigAdHocAPMode].u2BSSBasicRateSet;
    prBssInfo->u2OperationalRateSet =
            rNonHTPhyAttributes[prBssInfo->ucNonHTBasicPhyType].u2SupportedRateSet;

    prBssInfo->fgErpProtectMode = FALSE;

    /* Allocate MSDU_INFO_T for Beacon */
    prBssInfo->prBeacon = cnmMgtPktAlloc(prAdapter, OFFSET_OF(WLAN_BEACON_FRAME_T, aucInfoElem[0]) + MAX_IE_LENGTH);
    if (prBssInfo->prBeacon) {
        prBssInfo->prBeacon->eSrc = TX_PACKET_MGMT;
        prBssInfo->prBeacon->ucNetworkType = NETWORK_TYPE_P2P_INDEX;
    }

    rateGetDataRatesFromRateSet(prBssInfo->u2OperationalRateSet,
                                    prBssInfo->u2BSSBasicRateSet,
                                    prBssInfo->aucAllSupportedRates,
                                    &prBssInfo->ucAllSupportedRatesLen);

    /* setup MID for current BSS */
    prBssInfo->u2BeaconInterval = DOT11_BEACON_PERIOD_DEFAULT;
    prBssInfo->ucDTIMPeriod = DOT11_DTIM_PERIOD_DEFAULT;
    prBssInfo->u2ATIMWindow = 0;
    prBssInfo->ucBeaconTimeoutCount = 0;
    /* no QoS */
    prBssInfo->fgIsQBSS = TRUE;//with Qos

    prBssInfo->fgIsProtection = prAdapter->prGlueInfo->rWtInfo.fgIsPrivacy;

    /* Update BSS_INFO_T common part */
#if CFG_SUPPORT_AAA
    bssInitForAP(prAdapter, prBssInfo);
    nicQmUpdateWmmParms(prAdapter, NETWORK_TYPE_P2P_INDEX);
#endif

    /* set MAC HW */
    WT_BSS_INFO_INIT(prAdapter, NETWORK_TYPE_P2P_INDEX);

    /* Setup channel and bandwidth */
    rlmBssInitForAPandIbss(prAdapter, prBssInfo);

    /* use command packet to inform firmware
       setup to AP mode and disconnect state
    */

    nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);

    /*  NETWORK_TYPE_P2P_INDEX: FW generate and send beacon
        MUST Update BCN content after created.
        can update BCN content anytime if BSS created
    */
    nicPmIndicateBssCreated(prAdapter, NETWORK_TYPE_P2P_INDEX);

    /* Update FW BCN content */
    bssUpdateBeaconContent(prAdapter, NETWORK_TYPE_P2P_INDEX);

    /* Indicate CNM a new network is created */
    wtReleaseCh(prAdapter);

    /* Update GTK if exist */
     if (prAdapter->prGlueInfo->rWtInfo.fgIsGTKExist) {
        /* add GTK */
        MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_258, "Add GTK");
        wlanSetInformation(prAdapter,
                wtAddPairwiseKey,
                (PVOID)prKey,
                (UINT_32)prKey->u4Length,
                (PUINT_32)&setInfoLen );
     }
    /* debug */
    dumpBss(prAdapter, prBssInfo);

    /* indicated to hostap */
    wndrv_send_hostap_set_privacy_cnf_handler();

    return;
}

VOID
wtResetStaRecStatus (
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec
    )
{
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtResetStaRecStatus");
    do {
        WNDRV_ASSERT((prAdapter != NULL) && (prStaRec != NULL));

        prStaRec->u2StatusCode = STATUS_CODE_SUCCESSFUL;
        prStaRec->u2ReasonCode = REASON_CODE_RESERVED;
        prStaRec->ucJoinFailureCount = 0;
        prStaRec->fgTransmitKeyExist = FALSE;
        prStaRec->fgSetPwrMgtBit = FALSE;
        //prStaRec->fgIsInUse = FALSE;

    } while (FALSE);

    return;
} /* p2pFuncResetStaRecStatus */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to dissolve from group or one group. (Would not change P2P FSM.)
*              1. GC: Disconnect from AP. (Send Deauth)
*              2. GO: Disconnect all STA
*
* @param[in] prAdapter   Pointer to the adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wtFuncDisconnect (
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN BOOLEAN fgSendDeauth,
    IN UINT_16 u2ReasonCode
    )
{
    P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;
    ENUM_PARAM_MEDIA_STATE_T eOriMediaStatus;

    //DEBUGFUNC("wtFuncDisconnect");

    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_259, "[TETHERING]Disconnect STA: "MACSTR, MAC2STR(prStaRec->aucMacAddr));
    do {
        ASSERT((prAdapter != NULL) && (prStaRec != NULL));

        prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);
        eOriMediaStatus = prBssInfo->eConnectionState;

        if (fgSendDeauth) {
            /* Send deauth. */
            authSendDeauthFrame(prAdapter,
                        prStaRec,
                        (P_SW_RFB_T)NULL,
                        u2ReasonCode,
                        (PFN_TX_DONE_HANDLER)wtRunEventDeauthTxDone);
        }
        else {
        /* Change station state. */
        cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

        /* Reset Station Record Status. */
        wtResetStaRecStatus(prAdapter, prStaRec);

        if (prBssInfo->rStaRecOfClientList.u4NumElem == 0) {
            /* MAUI Note:
               to avoid hostap unsync. only one sta is free
               send deauth in here
            */
            wtChangeMediaState(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED);
        }

        /* Indicate disconnect to hostap. */
        //4TODO: compose dummy deauth frame to hostap
        //wtIndicationOfMediaStateToHost(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED, prStaRec->aucMacAddr);

        if (eOriMediaStatus != prBssInfo->eConnectionState) {
            /* Update Disconnected state to FW. */
            nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);
        }
        }
        /* Release channel if disconnected. ???? */

    } while (FALSE);

    return;

} /* p2pFuncDisconnect */
#if 0
static void _wpa_hexdump(const char *title, const char *buf,
			 kal_uint16 len, int show)
{
	kal_uint16 i,j = 0;
	kal_char str_buff[129];
	kal_uint16 local_len = 0;

	//wpa_debug_print_timestamp();
	MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "%s - hexdump(len=%d):", title, len);
    kal_mem_set(str_buff, 0, 129);
  while (len >0)
  {
	    if(len <= 64)
	    {
	    	local_len = len;
	    }
	    else
	    {
	    	local_len = 64;
	    }

	    if (show) {
	    	for (i = 0; i < local_len; i++)
	    	{
	    		kal_sprintf(&(str_buff[i*2]), "%02x-", (*(buf+i+j*64))&0x000000FF);
	    	}
            MT5931_TRACE(  TRACE_GROUP_10, str_buff);
	    } else {
	    	MT5931_TRACE(  TRACE_GROUP_10, " [REMOVED]");
	    }
	    len -= local_len;
      j++;
  }
}
#endif
/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to set a key to Wi-Fi Tethering
*
* \param[in] prAdapter Pointer to the Adapter structure.
* \param[in] pvSetBuffer A pointer to the buffer that holds the data to be set.
* \param[in] u4SetBufferLen The length of the set buffer.
* \param[out] pu4SetInfoLen If the call is successful, returns the number of
*                          bytes read from the set buffer. If the call failed
*                          due to invalid length of the set buffer, returns
*                          the amount of storage needed.
*
* \retval WLAN_STATUS_SUCCESS
* \retval WLAN_STATUS_ADAPTER_NOT_READY
* \retval WLAN_STATUS_INVALID_LENGTH
* \retval WLAN_STATUS_INVALID_DATA
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wtAddPairwiseKey(
    IN  P_ADAPTER_T prAdapter,
    IN  PVOID       pvSetBuffer,
    IN  UINT_32     u4SetBufferLen,
    OUT PUINT_32    pu4SetInfoLen
    )
{
    CMD_802_11_KEY rCmdKey;
    P_PARAM_KEY_T prNewKey;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wtAddPairwiseKey");

    ASSERT(prAdapter);
    ASSERT(pvSetBuffer);
    ASSERT(pu4SetInfoLen);

    prNewKey = (P_PARAM_KEY_T) pvSetBuffer;

    /* Verify the key structure length. */
    if (prNewKey->u4Length > u4SetBufferLen) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Invalid key structure length (%d) greater than total buffer length (%d)\n",
                    (UINT_8)prNewKey->u4Length,
                    (UINT_8)u4SetBufferLen);

        *pu4SetInfoLen = u4SetBufferLen;
        return WLAN_STATUS_INVALID_LENGTH;
    }
    /* Verify the key material length for key material buffer */
    else if (prNewKey->u4KeyLength > prNewKey->u4Length - OFFSET_OF(PARAM_KEY_T, aucKeyMaterial)) {
         MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR,"Invalid key material length (%d)\n", (UINT_8)prNewKey->u4KeyLength);
        *pu4SetInfoLen = u4SetBufferLen;
        return WLAN_STATUS_INVALID_DATA;
    }
    /* Exception check */
    else if (prNewKey->u4KeyIndex & 0x0fffff00) {
        return WLAN_STATUS_INVALID_DATA;
    }
    /* Exception check, pairwise key must with transmit bit enabled */
    else if ((prNewKey->u4KeyIndex & BITS(30,31)) == IS_UNICAST_KEY) {
        return WLAN_STATUS_INVALID_DATA;
    }
    else if (!(prNewKey->u4KeyLength == CCMP_KEY_LEN)) {
        return WLAN_STATUS_INVALID_DATA;
    }
    /* Exception check, pairwise key must with transmit bit enabled */
    else if ((prNewKey->u4KeyIndex & BITS(30,31)) == BITS(30,31)) {
        if (((prNewKey->u4KeyIndex & 0xff) != 0) ||
                ((prNewKey->arBSSID[0] == 0xff) && (prNewKey->arBSSID[1] == 0xff) && (prNewKey->arBSSID[2] == 0xff) &&
                 (prNewKey->arBSSID[3] == 0xff) && (prNewKey->arBSSID[4] == 0xff) && (prNewKey->arBSSID[5] == 0xff))) {
            return WLAN_STATUS_INVALID_DATA;
        }
    }

    *pu4SetInfoLen = u4SetBufferLen;

    // fill CMD_802_11_KEY
    kalMemZero(&rCmdKey, sizeof(CMD_802_11_KEY));
    rCmdKey.ucAddRemove = 1; /* add */
    rCmdKey.ucTxKey = ((prNewKey->u4KeyIndex & IS_TRANSMIT_KEY) == IS_TRANSMIT_KEY) ? 1 : 0;
    rCmdKey.ucKeyType = ((prNewKey->u4KeyIndex & IS_UNICAST_KEY) == IS_UNICAST_KEY) ? 1 : 0;
    rCmdKey.ucIsAuthenticator = 1;
    COPY_MAC_ADDR(rCmdKey.aucPeerAddr, prNewKey->arBSSID);
    rCmdKey.ucNetType = NETWORK_TYPE_P2P_INDEX;
    rCmdKey.ucAlgorithmId = CIPHER_SUITE_CCMP; // AES
    rCmdKey.ucKeyId = (UINT_8)(prNewKey->u4KeyIndex & 0xff);
    rCmdKey.ucKeyLen = (UINT_8)prNewKey->u4KeyLength;
    kalMemCopy(rCmdKey.aucKeyMaterial, (PUINT_8)prNewKey->aucKeyMaterial, rCmdKey.ucKeyLen);

    /*
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucAddRemove : %d",rCmdKey.ucAddRemove);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucTxKey : %d", rCmdKey.ucTxKey);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucKeyType : %d",rCmdKey.ucKeyType);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucIsAuthenticator : %d",rCmdKey.ucIsAuthenticator);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.aucPeerAddr: "MACSTR, MAC2STR(rCmdKey.aucPeerAddr));
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucNetType : %d",rCmdKey.ucNetType);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucAlgorithmId: %d",rCmdKey.ucAlgorithmId);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucKeyId : %d",rCmdKey.ucKeyId);
    kal_wap_trace(MAP_MOD_WNDRV, TRACE_HOTSPOT, "rCmdKey.ucKeyLen : %d",rCmdKey.ucKeyLen);
    _wpa_hexdump("rCmdKey.aucKeyMaterial:", (char *)rCmdKey.aucKeyMaterial, rCmdKey.ucKeyLen, 1);
    */

    return wlanSendSetQueryWTCmd(prAdapter,
            CMD_ID_ADD_REMOVE_KEY,
            TRUE,
            FALSE,
            nicCmdEventSetCommon,
            NULL,
            sizeof(CMD_802_11_KEY),
            (PUINT_8)&rCmdKey,
            pvSetBuffer,
            u4SetBufferLen
            );
}


/*----------------------------------------------------------------------------*/
/*!
* \brief It is only for AP mode in NETWORK_TYPE_P2P_INDEX.
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
rlmUpdateParamsForAP (
    P_ADAPTER_T     prAdapter,
    P_BSS_INFO_T    prBssInfo,
    BOOLEAN         fgUpdateBeacon
    )
{
    P_LINK_T                prStaList;
    P_STA_RECORD_T          prStaRec;
    BOOLEAN                 fgErpProtectMode, fgSta40mIntolerant;
    ENUM_HT_PROTECT_MODE_T  eHtProtectMode;
    ENUM_GF_MODE_T          eGfOperationMode;
    UINT_8                  ucHtOpInfo1;
    BOOLEAN                 fgUseShortPreamble;

    ASSERT(prAdapter);
    ASSERT(prBssInfo);
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]rlmUpdateParamsForAP");
    if (!IS_BSS_ACTIVE(prBssInfo) ||
        prBssInfo->eCurrentOPMode != OP_MODE_ACCESS_POINT) {
        return;
    }

    fgErpProtectMode = FALSE;
    eHtProtectMode = HT_PROTECT_MODE_NONE;
    eGfOperationMode = GF_MODE_NORMAL;
    fgSta40mIntolerant = FALSE;
    //prBssInfo->fgUseShortPreamble = prBssInfo->fgIsShortPreambleAllowed;
    fgUseShortPreamble = prBssInfo->fgIsShortPreambleAllowed;
    ucHtOpInfo1 = (UINT_8) CHNL_EXT_SCN;

    prStaList = &prBssInfo->rStaRecOfClientList;
    if (prStaList->u4NumElem > 0) {
        LINK_FOR_EACH_ENTRY(prStaRec, prStaList, rLinkEntry, STA_RECORD_T) {
            ASSERT(prStaRec);
            dumpSTA(prAdapter, prStaRec);
            if (prStaRec->fgIsInUse && prStaRec->ucStaState == STA_STATE_3) {
                if (!(prStaRec->ucPhyTypeSet &
                      (PHY_TYPE_SET_802_11GN | PHY_TYPE_SET_802_11A))) {
                    /* B-only mode, so mode 1 (ERP protection) */
                    MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_260, "B-only mode");
                    fgErpProtectMode = TRUE;
                }
                else if (!(prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N)) {
                    /* BG-only or A-only */
                    MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_261, "BG-only/A-only mode");
                    eHtProtectMode = HT_PROTECT_MODE_NON_HT;
                }
                else if (!(prStaRec->u2HtCapInfo & HT_CAP_INFO_SUP_CHNL_WIDTH)) {
                    /* 20MHz-only */
                    if (eHtProtectMode == HT_PROTECT_MODE_NONE) {
                        MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_262, "20MHZ-only");
                        eHtProtectMode = HT_PROTECT_MODE_20M;
                    }
                }

                if (!(prStaRec->u2HtCapInfo & HT_CAP_INFO_HT_GF)) {
                    eGfOperationMode = GF_MODE_PROTECT;
                }

                if (!(prStaRec->u2CapInfo & CAP_INFO_SHORT_PREAMBLE)) {
                    MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_263, "Long preamble");
                    prBssInfo->fgUseShortPreamble = FALSE;
                }

                if (prStaRec->u2HtCapInfo & HT_CAP_INFO_40M_INTOLERANT) {
                    fgSta40mIntolerant = TRUE;
                }
            }
        } /* end of LINK_FOR_EACH_ENTRY */
    }
    /* Check if HT operation IE about 20/40M bandwidth shall be updated */
    if (prBssInfo->eBssSCO != CHNL_EXT_SCN) {
        if (/*!LINK_IS_EMPTY(prStaList) && */ !fgSta40mIntolerant &&
            !prBssInfo->fgObssActionForcedTo20M &&
            !prBssInfo->fgObssBeaconForcedTo20M) {

            ucHtOpInfo1 = (UINT_8)
                (((UINT_32) prBssInfo->eBssSCO) | HT_OP_INFO1_STA_CHNL_WIDTH);
        }
    }

    /* Check if any new parameter may be updated */
    if (prBssInfo->fgErpProtectMode != fgErpProtectMode ||
        prBssInfo->eHtProtectMode != eHtProtectMode ||
        prBssInfo->eGfOperationMode != eGfOperationMode ||
        prBssInfo->ucHtOpInfo1 != ucHtOpInfo1 ||
        prBssInfo->fgIsShortPreambleAllowed != fgUseShortPreamble) {

        prBssInfo->fgErpProtectMode = fgErpProtectMode;
        prBssInfo->eHtProtectMode = eHtProtectMode;
        prBssInfo->eGfOperationMode = eGfOperationMode;
        prBssInfo->ucHtOpInfo1 = ucHtOpInfo1;

        rlmSyncOperationParams(prAdapter, prBssInfo);
        fgUpdateBeacon = TRUE;
    }

    /* Update Beacon content if related IE content is changed */
    if (fgUpdateBeacon) {
        bssUpdateBeaconContent(prAdapter, (ENUM_NETWORK_TYPE_INDEX_T)(prBssInfo->ucNetTypeIndex));
        dumpBss(prAdapter, prBssInfo);
    }
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
rlmHandleObssStatusEventPkt (
    P_ADAPTER_T                 prAdapter,
    P_EVENT_AP_OBSS_STATUS_T    prObssStatus
    )
{
    P_BSS_INFO_T            prBssInfo;

    ASSERT(prAdapter);
    ASSERT(prObssStatus);
    ASSERT(prObssStatus->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX);
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]rlmHandleObssStatusEventPkt");

    prBssInfo = &prAdapter->rWifiVar.arBssInfo[prObssStatus->ucNetTypeIndex];
    ASSERT(prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT);

    prBssInfo->fgObssErpProtectMode =
            (BOOLEAN) prObssStatus->ucObssErpProtectMode;
    prBssInfo->eObssHtProtectMode =
            (ENUM_HT_PROTECT_MODE_T) prObssStatus->ucObssHtProtectMode;
    prBssInfo->eObssGfOperationMode =
            (ENUM_GF_MODE_T) prObssStatus->ucObssGfOperationMode;
    prBssInfo->fgObssRifsOperationMode =
            (BOOLEAN) prObssStatus->ucObssRifsOperationMode;
    prBssInfo->fgObssBeaconForcedTo20M =
            (BOOLEAN) prObssStatus->ucObssBeaconForcedTo20M;

    /* Check if Beacon content need to be updated */
    rlmUpdateParamsForAP(prAdapter, prBssInfo, TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief command packet generation utility
*
* \param[in] prAdapter          Pointer to the Adapter structure.
* \param[in] ucCID              Command ID
* \param[in] fgSetQuery         Set or Query
* \param[in] fgNeedResp         Need for response
* \param[in] pfCmdDoneHandler   Function pointer when command is done
* \param[in] u4SetQueryInfoLen  The length of the set/query buffer
* \param[in] pucInfoBuffer      Pointer to set/query buffer
*
*
* \retval WLAN_STATUS_PENDING
* \retval WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSendSetQueryWTCmd (
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
    )
{
    P_GLUE_INFO_T prGlueInfo;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    UINT_8 ucCmdSeqNum;

    ASSERT(prAdapter);

    prGlueInfo = prAdapter->prGlueInfo;
    ASSERT(prGlueInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_HOTSPOT,"[TETHERING]wlanoidSendSetQueryWTCmd");

    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, (CMD_HDR_SIZE + u4SetQueryInfoLen));

    if (!prCmdInfo) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Allocate CMD_INFO_T ==> FAILED.\n");
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);
    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_264, "ucCmdSeqNum =%d / CmdID=%x\n", ucCmdSeqNum, ucCID);

    // Setup common CMD Info Packet
    prCmdInfo->eCmdType = COMMAND_TYPE_NETWORK_IOCTL;
    prCmdInfo->eNetworkType = NETWORK_TYPE_P2P_INDEX;
    prCmdInfo->u2InfoBufLen = (UINT_16)(CMD_HDR_SIZE + u4SetQueryInfoLen);
    prCmdInfo->pfCmdDoneHandler = pfCmdDoneHandler;
    prCmdInfo->pfCmdTimeoutHandler = pfCmdTimeoutHandler;
    prCmdInfo->fgIsOid = TRUE;
    prCmdInfo->ucCID = ucCID;
    prCmdInfo->fgSetQuery = fgSetQuery;
    prCmdInfo->fgNeedResp = fgNeedResp;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = u4SetQueryInfoLen;
    prCmdInfo->pvInformationBuffer = pvSetQueryBuffer;
    prCmdInfo->u4InformationBufferLength = u4SetQueryBufferLen;

    // Setup WIFI_CMD_T (no payload)
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    if(u4SetQueryInfoLen > 0 && pucInfoBuffer != NULL) {
        kalMemCopy(prWifiCmd->aucBuffer, pucInfoBuffer, u4SetQueryInfoLen);
    }

    // insert into prCmdQueue
    kalEnqueueCommand(prGlueInfo, (P_QUE_ENTRY_T)prCmdInfo);

    // wakeup txServiceThread later
    GLUE_SET_EVENT(prGlueInfo);
    return WLAN_STATUS_PENDING;
}

WLAN_STATUS
wtUpdateTxPower (
    IN P_ADAPTER_T  prAdapter,
    IN INT_8        cTxPower
    )
{
    CMD_TX_PWR_T rTxPwrParam;
    rTxPwrParam.cTxPwr2G4Dsss =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4Cck = (cTxPower << 1);

    rTxPwrParam.cTxPwr2G4OFDM_BPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4OFDM_QPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4OFDM_16QAM = (cTxPower << 1);

    rTxPwrParam.cTxPwr2G4OFDM_48Mbps = (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4OFDM_54Mbps = (cTxPower << 1);

    rTxPwrParam.cTxPwr2G4HT20_BPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_QPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_16QAM = (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_MCS5 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_MCS6 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_MCS7 =  (cTxPower << 1);

    rTxPwrParam.cTxPwr2G4HT40_BPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_QPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_16QAM = (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_MCS5 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_MCS6 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_MCS7 =  (cTxPower << 1);

    rTxPwrParam.cTxPwr5GOFDM_BPSK =   (cTxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_QPSK =   (cTxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_16QAM =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_48Mbps = (cTxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_54Mbps = (cTxPower << 1);

    rTxPwrParam.cTxPwr5GHT20_BPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_QPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_16QAM = (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_MCS5 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_MCS6 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_MCS7 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_BPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_QPSK =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_16QAM = (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_MCS5 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_MCS6 =  (cTxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_MCS7 =  (cTxPower << 1);

    nicUpdateTxPower(prAdapter, &rTxPwrParam);
	return WLAN_STATUS_SUCCESS;
}

WLAN_STATUS
wtRunEventDeauthTxDone (
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T rTxDoneStatus
    )
{
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;
    ENUM_PARAM_MEDIA_STATE_T eOriMediaStatus;
    do {
        ASSERT((prAdapter != NULL) && (prMsduInfo != NULL));
        MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_265, "Deauth TX Done");
        prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
        if (prStaRec == NULL) {
            MT5931_TRACE(TRACE_WARNING, MT5931_INFO_266, "Station Record NULL, Index:%d\n", prMsduInfo->ucStaRecIndex);
            break;
        }
        prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);
        eOriMediaStatus = prBssInfo->eConnectionState;
        cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
        wtResetStaRecStatus(prAdapter, prStaRec);
        if (prBssInfo->rStaRecOfClientList.u4NumElem == 0) {
             wtChangeMediaState(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED);
         }
        if (eOriMediaStatus != prBssInfo->eConnectionState) {
            nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);
        }
    } while (FALSE);
    return WLAN_STATUS_SUCCESS;
} /* p2pFsmRunEventDeauthTxDone */

#endif

