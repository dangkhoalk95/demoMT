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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/scan.c#60 $
*/

/*! \file   "scan.c"
    \brief  This file defines the scan profile and the processing function of
            scan result for SCAN Module.

    The SCAN Profile selection is part of SCAN MODULE and responsible for defining
    SCAN Parameters - e.g. MIN_CHANNEL_TIME, number of scan channels.
    In this file we also define the process of SCAN Result including adding, searching
    and removing SCAN record from the list.
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
extern UINT_8 g_ignore_un_exist_ap[MAC_ADDR_LEN];
/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define REPLICATED_BEACON_TIME_THRESHOLD        (3000)
#define REPLICATED_BEACON_FRESH_PERIOD          (10000)
#define REPLICATED_BEACON_STRENGTH_THRESHOLD    (32)

#define ROAMING_NO_SWING_RCPI_STEP              (10)

/* CMCCC TD */
#define CFG_SCAN_AVAILABLE_RSSI_THRESHOLD       (-85)

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
* @brief This function is used by SCN to initialize its variables
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
scnInit(
    IN P_ADAPTER_T prAdapter
)
{
    P_SCAN_INFO_T prScanInfo;
    P_BSS_DESC_T prBSSDesc;
    PUINT_8 pucBSSBuff;
    UINT_32 i;


    ASSERT(prAdapter);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    pucBSSBuff = &prScanInfo->aucScanBuffer[0];


    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "->scnInit()\n");

    //4 <1> Reset STATE and Message List
    prScanInfo->eCurrentState = SCAN_STATE_IDLE;

    prScanInfo->rLastScanCompletedTime = (OS_SYSTIME)0;

    prScanInfo->u2ChannelDwellTime = 0;

    LINK_INITIALIZE(&prScanInfo->rPendingMsgList);


    //4 <2> Reset link list of BSS_DESC_T
    kalMemZero((PVOID) pucBSSBuff, SCN_MAX_BUFFER_SIZE);

    LINK_INITIALIZE(&prScanInfo->rFreeBSSDescList);
    LINK_INITIALIZE(&prScanInfo->rBSSDescList);

    for (i = 0; i < CFG_MAX_NUM_BSS_LIST; i++) {

        prBSSDesc = (P_BSS_DESC_T)pucBSSBuff;

        LINK_INSERT_TAIL(&prScanInfo->rFreeBSSDescList, &prBSSDesc->rLinkEntry);

        pucBSSBuff += ALIGN_4(sizeof(BSS_DESC_T));
    }
    /* Check if the memory allocation consist with this initialization function */
    ASSERT(((UINT_32)pucBSSBuff - (UINT_32)&prScanInfo->aucScanBuffer[0]) == SCN_MAX_BUFFER_SIZE);

#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    //4 <3> Reset link list of scanDesiredSSIDList
    LINK_INITIALIZE(&prScanInfo->rscanDesiredSSIDList);
#endif

    /* reset freest channel information */
    prScanInfo->fgIsSparseChannelValid = FALSE;
    prScanInfo->rSSIDOfScanReq.u4SsidLen = 0;
    kalMemZero(prScanInfo->rSSIDOfScanReq.aucSsid, PARAM_MAX_LEN_SSID);
    return;
} /* end of scnInit() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used by SCN to uninitialize its variables
*
* @param (none)
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
scnUninit(
    IN P_ADAPTER_T prAdapter
)
{
    P_SCAN_INFO_T prScanInfo;
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    SSID_LIST_T *ssid_list_ptr_next;
    SSID_LIST_T *ssid_list_ptr;
    P_LINK_T    prscanDesiredSSIDList;
#endif

    ASSERT(prAdapter);
    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    prscanDesiredSSIDList = &prScanInfo->rscanDesiredSSIDList;
#endif

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DEINIT, "->scnUninit()\n");

    //4 <1> Reset STATE and Message List
    prScanInfo->eCurrentState = SCAN_STATE_IDLE;

    prScanInfo->rLastScanCompletedTime = (OS_SYSTIME)0;

    prScanInfo->u2ChannelDwellTime = 0;

    /* NOTE(Kevin): Check rPendingMsgList ? */

    //4 <2> Reset link list of BSS_DESC_T
    LINK_INITIALIZE(&prScanInfo->rFreeBSSDescList);
    LINK_INITIALIZE(&prScanInfo->rBSSDescList);

#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    //4 <3> Reset link list of scanDesiredSSIDList, dealloc mem
    if (prscanDesiredSSIDList->u4NumElem) {
        LINK_FOR_EACH_ENTRY_SAFE(ssid_list_ptr, ssid_list_ptr_next, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) {

            kal_wap_trace(MOD_WNDRV, TRACE_DEINIT, " free ssid is:[%s] ", ssid_list_ptr->ssid_list->ssid);

            LINK_REMOVE_KNOWN_ENTRY(prscanDesiredSSIDList, ssid_list_ptr);
            kalMemFree(ssid_list_ptr->ssid_list, VIR_MEM_TYPE, sizeof(abm_wndrv_ssid_struct));
            kalMemFree(ssid_list_ptr, VIR_MEM_TYPE, sizeof(SSID_LIST_T));

        }
        LINK_INITIALIZE(&prScanInfo->rscanDesiredSSIDList);
    }
#endif
    prScanInfo->rSSIDOfScanReq.u4SsidLen = 0;
    kalMemZero(prScanInfo->rSSIDOfScanReq.aucSsid, PARAM_MAX_LEN_SSID);
    return;
} /* end of scnUninit() */



/*----------------------------------------------------------------------------*/
/*!
* @brief Find the corresponding BSS Descriptor according to given BSSID
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] aucBSSID           Given BSSID.
*
* @return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchBssDescByBssid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
)
{
    return scanSearchBssDescByBssidAndSsid(prAdapter,
                                           aucBSSID,
                                           FALSE,
                                           NULL);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Find the corresponding BSS Descriptor according to given BSSID
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] aucBSSID           Given BSSID.
* @param[in] fgCheckSsid        Need to check SSID or not. (for multiple SSID with single BSSID cases)
* @param[in] prSsid             Specified SSID
*
* @return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchBssDescByBssidAndSsid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
)
{
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prBSSDescList;
    P_BSS_DESC_T prBssDesc;
    P_BSS_DESC_T prDstBssDesc = (P_BSS_DESC_T)NULL;


    ASSERT(prAdapter);
    ASSERT(aucBSSID);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);

    prBSSDescList = &prScanInfo->rBSSDescList;

    /* Search BSS Desc from current SCAN result list. */
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {

        if (EQUAL_MAC_ADDR(prBssDesc->aucBSSID, aucBSSID)) {
            if (fgCheckSsid == FALSE || prSsid == NULL) {
                return prBssDesc;
            } else {
                if (EQUAL_SSID(prBssDesc->aucSSID,
                               prBssDesc->ucSSIDLen,
                               prSsid->aucSsid,
                               prSsid->u4SsidLen)) {
                    return prBssDesc;
                } else if (prDstBssDesc == NULL && prBssDesc->fgIsHiddenSSID == TRUE) {
                    prDstBssDesc = prBssDesc;
                }
            }
        }
    }

    return prDstBssDesc;

} /* end of scanSearchBssDescByBssid() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Find the corresponding BSS Descriptor according to given Transmitter Address.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] aucSrcAddr         Given Source Address(TA).
*
* @return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchBssDescByTA(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucSrcAddr[]
)
{
    return scanSearchBssDescByTAAndSsid(prAdapter,
                                        aucSrcAddr,
                                        FALSE,
                                        NULL);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Find the corresponding BSS Descriptor according to given Transmitter Address.
*
* @param[in] prAdapter          Pointer to the Adapter structure.
* @param[in] aucSrcAddr         Given Source Address(TA).
* @param[in] fgCheckSsid        Need to check SSID or not. (for multiple SSID with single BSSID cases)
* @param[in] prSsid             Specified SSID
*
* @return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchBssDescByTAAndSsid(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucSrcAddr[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
)
{
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prBSSDescList;
    P_BSS_DESC_T prBssDesc;
    P_BSS_DESC_T prDstBssDesc = (P_BSS_DESC_T)NULL;


    ASSERT(prAdapter);
    ASSERT(aucSrcAddr);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);

    prBSSDescList = &prScanInfo->rBSSDescList;

    /* Search BSS Desc from current SCAN result list. */
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {

        if (EQUAL_MAC_ADDR(prBssDesc->aucSrcAddr, aucSrcAddr)) {
            if (fgCheckSsid == FALSE || prSsid == NULL) {
                return prBssDesc;
            } else {
                if (EQUAL_SSID(prBssDesc->aucSSID,
                               prBssDesc->ucSSIDLen,
                               prSsid->aucSsid,
                               prSsid->u4SsidLen)) {
                    return prBssDesc;
                } else if (prDstBssDesc == NULL && prBssDesc->fgIsHiddenSSID == TRUE) {
                    prDstBssDesc = prBssDesc;
                }
            }
        }
    }

    return prDstBssDesc;

} /* end of scanSearchBssDescByTA() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Find the corresponding BSS Descriptor according to
*        given eBSSType, BSSID and Transmitter Address
*
* @param[in] prAdapter  Pointer to the Adapter structure.
* @param[in] eBSSType   BSS Type of incoming Beacon/ProbeResp frame.
* @param[in] aucBSSID   Given BSSID of Beacon/ProbeResp frame.
* @param[in] aucSrcAddr Given source address (TA) of Beacon/ProbeResp frame.
*
* @return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchExistingBssDesc(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T eBSSType,
    IN UINT_8 aucBSSID[],
    IN UINT_8 aucSrcAddr[]
)
{
    return scanSearchExistingBssDescWithSsid(prAdapter,
            eBSSType,
            aucBSSID,
            aucSrcAddr,
            FALSE,
            NULL);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Find the corresponding BSS Descriptor according to
*        given eBSSType, BSSID and Transmitter Address
*
* @param[in] prAdapter  Pointer to the Adapter structure.
* @param[in] eBSSType   BSS Type of incoming Beacon/ProbeResp frame.
* @param[in] aucBSSID   Given BSSID of Beacon/ProbeResp frame.
* @param[in] aucSrcAddr Given source address (TA) of Beacon/ProbeResp frame.
* @param[in] fgCheckSsid Need to check SSID or not. (for multiple SSID with single BSSID cases)
* @param[in] prSsid     Specified SSID
*
* @return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchExistingBssDescWithSsid(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T eBSSType,
    IN UINT_8 aucBSSID[],
    IN UINT_8 aucSrcAddr[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
)
{
    P_SCAN_INFO_T prScanInfo;
    P_BSS_DESC_T prBssDesc, prIBSSBssDesc;

    ASSERT(prAdapter);
    ASSERT(aucSrcAddr);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);


    switch (eBSSType) {
        case BSS_TYPE_P2P_DEVICE:
            fgCheckSsid = FALSE;
        case BSS_TYPE_INFRASTRUCTURE:
        case BSS_TYPE_BOW_DEVICE: {
            prBssDesc = scanSearchBssDescByBssidAndSsid(prAdapter, aucBSSID, fgCheckSsid, prSsid);

            /* if (eBSSType == prBssDesc->eBSSType) */

            return prBssDesc;
        }

        case BSS_TYPE_IBSS: {
            prIBSSBssDesc = scanSearchBssDescByBssidAndSsid(prAdapter, aucBSSID, fgCheckSsid, prSsid);
            prBssDesc = scanSearchBssDescByTAAndSsid(prAdapter, aucSrcAddr, fgCheckSsid, prSsid);

            /* NOTE(Kevin):
             * Rules to maintain the SCAN Result:
             * For AdHoc -
             *    CASE I    We have TA1(BSSID1), but it change its BSSID to BSSID2
             *              -> Update TA1 entry's BSSID.
             *    CASE II   We have TA1(BSSID1), and get TA1(BSSID1) again
             *              -> Update TA1 entry's contain.
             *    CASE III  We have a SCAN result TA1(BSSID1), and TA2(BSSID2). Sooner or
             *               later, TA2 merge into TA1, we get TA2(BSSID1)
             *              -> Remove TA2 first and then replace TA1 entry's TA with TA2, Still have only one entry of BSSID.
             *    CASE IV   We have a SCAN result TA1(BSSID1), and another TA2 also merge into BSSID1.
             *              -> Replace TA1 entry's TA with TA2, Still have only one entry.
             *    CASE V    New IBSS
             *              -> Add this one to SCAN result.
             */
            if (prBssDesc) {
                if ((!prIBSSBssDesc) || // CASE I
                        (prBssDesc == prIBSSBssDesc)) { // CASE II

                    return prBssDesc;
                } else { // CASE III
                    P_LINK_T prBSSDescList;
                    P_LINK_T prFreeBSSDescList;


                    prBSSDescList = &prScanInfo->rBSSDescList;
                    prFreeBSSDescList = &prScanInfo->rFreeBSSDescList;

                    /* Remove this BSS Desc from the BSS Desc list */
                    LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDesc);

                    /* Return this BSS Desc to the free BSS Desc list. */
                    LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDesc->rLinkEntry);

                    return prIBSSBssDesc;
                }
            }

            if (prIBSSBssDesc) { // CASE IV

                return prIBSSBssDesc;
            }

            // CASE V
            break; // Return NULL;
        }

        default:
            break;
    }


    return (P_BSS_DESC_T)NULL;

} /* end of scanSearchExistingBssDesc() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Delete BSS Descriptors from current list according to given Remove Policy.
*
* @param[in] u4RemovePolicy     Remove Policy.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
scanRemoveBssDescsByPolicy(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4RemovePolicy
)
{
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prBSSDescList;
    P_LINK_T prFreeBSSDescList;
    P_BSS_DESC_T prBssDesc;

#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    P_LINK_T    prscanDesiredSSIDList;
    prscanDesiredSSIDList = &prAdapter->rWifiVar.rScanInfo.rscanDesiredSSIDList;
#endif

    ASSERT(prAdapter);

    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prBSSDescList = &prScanInfo->rBSSDescList;
    prFreeBSSDescList = &prScanInfo->rFreeBSSDescList;

    //DBGLOG(SCN, TRACE, ("Before Remove - Number Of SCAN Result = %ld\n",
    //prBSSDescList->u4NumElem));

    if (u4RemovePolicy & SCN_RM_POLICY_TIMEOUT) {
        P_BSS_DESC_T prBSSDescNext;
        OS_SYSTIME rCurrentTime;


        GET_CURRENT_SYSTIME(&rCurrentTime);

        /* Search BSS Desc from current SCAN result list. */
        LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList, rLinkEntry, BSS_DESC_T) {

            if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) &&
                    (prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) {
                /* Don't remove the one currently we are connected. */
                continue;
            }

            if (CHECK_FOR_TIMEOUT(rCurrentTime, prBssDesc->rUpdateTime,
                                  SEC_TO_SYSTIME(SCN_BSS_DESC_REMOVE_TIMEOUT_SEC))) {

                //DBGLOG(SCN, TRACE, ("Remove TIMEOUT BSS DESC(%#x): MAC: "MACSTR", Current Time = %08lx, Update Time = %08lx\n",
                //prBssDesc, MAC2STR(prBssDesc->aucBSSID), rCurrentTime, prBssDesc->rUpdateTime));

                /* Remove this BSS Desc from the BSS Desc list */
                LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDesc);
#if CFG_ENABLE_WIFI_DIRECT
                if (prAdapter->fgIsP2PRegistered) {
                    ASSERT(prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc);
                    prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc(prAdapter, prBssDesc);
                }
#endif
                /* Return this BSS Desc to the free BSS Desc list. */
                LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDesc->rLinkEntry);
            }
        }
    } else if (u4RemovePolicy & SCN_RM_POLICY_OLDEST_HIDDEN) {
        P_BSS_DESC_T prBssDescOldest = (P_BSS_DESC_T)NULL;


        /* Search BSS Desc from current SCAN result list. */
        LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {

            if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) &&
                    (prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) {
                /* Don't remove the one currently we are connected. */
                continue;
            }

            if (!prBssDesc->fgIsHiddenSSID) {
                continue;
            }

            if (!prBssDescOldest) { /* 1st element */
                prBssDescOldest = prBssDesc;
                continue;
            }

            if (TIME_BEFORE(prBssDesc->rUpdateTime, prBssDescOldest->rUpdateTime)) {
                prBssDescOldest = prBssDesc;
            }
        }

        if (prBssDescOldest) {

            //DBGLOG(SCN, TRACE, ("Remove OLDEST HIDDEN BSS DESC(%#x): MAC: "MACSTR", Update Time = %08lx\n",
            //prBssDescOldest, MAC2STR(prBssDescOldest->aucBSSID), prBssDescOldest->rUpdateTime));

            /* Remove this BSS Desc from the BSS Desc list */
            LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDescOldest);
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc);
                prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc(prAdapter, prBssDescOldest);
            }
#endif
            /* Return this BSS Desc to the free BSS Desc list. */
            LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDescOldest->rLinkEntry);
        }
    } else if (u4RemovePolicy & SCN_RM_POLICY_SMART_WEAKEST) {
        P_BSS_DESC_T prBssDescWeakest = (P_BSS_DESC_T)NULL;
        P_BSS_DESC_T prBssDescWeakestSameSSID = (P_BSS_DESC_T)NULL;
        UINT_32 u4SameSSIDCount = 0;


        /* Search BSS Desc from current SCAN result list. */
        LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {

            if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) &&
                    (prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) {
                /* Don't remove the one currently we are connected. */
                continue;
            }

            if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_SCANREQ_SSID) &&
                    (prScanInfo->rSSIDOfScanReq.u4SsidLen) &&
                    (EQUAL_SSID(prBssDesc->aucSSID,
                                prBssDesc->ucSSIDLen,
                                prScanInfo->rSSIDOfScanReq.aucSsid,
                                prScanInfo->rSSIDOfScanReq.u4SsidLen))) {
                /* Don't remove the one in scan req. */
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "do not remove ssid in scanreq");
                continue;
            }
            if ((!prBssDesc->fgIsHiddenSSID) &&
                    (EQUAL_SSID(prBssDesc->aucSSID,
                                prBssDesc->ucSSIDLen,
                                prConnSettings->aucSSID,
                                prConnSettings->ucSSIDLen))) {

                u4SameSSIDCount++;

                if (!prBssDescWeakestSameSSID) {
                    prBssDescWeakestSameSSID = prBssDesc;
                } else if (prBssDesc->ucRCPI < prBssDescWeakestSameSSID->ucRCPI) {
                    prBssDescWeakestSameSSID = prBssDesc;
                }
            }

            if (!prBssDescWeakest) { /* 1st element */
                prBssDescWeakest = prBssDesc;
                continue;
            }

            if (prBssDesc->ucRCPI < prBssDescWeakest->ucRCPI) {
                prBssDescWeakest = prBssDesc;
            }

        }

        if ((u4SameSSIDCount >= SCN_BSS_DESC_SAME_SSID_THRESHOLD) &&
                (prBssDescWeakestSameSSID)) {
            prBssDescWeakest = prBssDescWeakestSameSSID;
        }

        if (prBssDescWeakest) {

            //DBGLOG(SCN, TRACE, ("Remove WEAKEST BSS DESC(%#x): MAC: "MACSTR", Update Time = %08lx\n",
            //prBssDescOldest, MAC2STR(prBssDescOldest->aucBSSID), prBssDescOldest->rUpdateTime));

            /* Remove this BSS Desc from the BSS Desc list */
            LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDescWeakest);
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc);
                prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc(prAdapter, prBssDescWeakest);
            }

#endif
            /* Return this BSS Desc to the free BSS Desc list. */
            LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDescWeakest->rLinkEntry);
        }
    } else if (u4RemovePolicy & SCN_RM_POLICY_ENTIRE) {
        P_BSS_DESC_T prBSSDescNext;

        LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList, rLinkEntry, BSS_DESC_T) {

            if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) &&
                    (prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) {
                /* Don't remove the one currently we are connected. */
                continue;
            }

            /* Remove this BSS Desc from the BSS Desc list */
            LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDesc);
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc);
                prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc(prAdapter, prBssDesc);
            }
#endif
            /* Return this BSS Desc to the free BSS Desc list. */
            LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDesc->rLinkEntry);
        }

    }
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    else if (u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_DESIRED_SSID_LIST) {
        SSID_LIST_T *ssid_list_ptr;
        kal_uint8   fgIsInSSIDList = KAL_FALSE;
        P_BSS_DESC_T prBSSDescNext;

        if (prscanDesiredSSIDList->u4NumElem == 0) {

            kal_wap_trace(MOD_WNDRV, TRACE_WARNING, "desired ssid list is empty ");
            return;
        }

        /* Search BSS Desc from current SCAN result list. */
        LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList, rLinkEntry, BSS_DESC_T) {

            if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) && \
                    (prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) {
                /* Don't remove the one currently we are connected. */
                continue;
            }

            fgIsInSSIDList = KAL_FALSE;
            LINK_FOR_EACH_ENTRY(ssid_list_ptr, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) {


                if (EQUAL_SSID(ssid_list_ptr->ssid_list->ssid, \
                               ssid_list_ptr->ssid_list->ssid_len, \
                               prBssDesc->aucSSID, \
                               prBssDesc->ucSSIDLen)) {

                    fgIsInSSIDList = KAL_TRUE;
                    break;
                }

            }

            if (fgIsInSSIDList == KAL_FALSE) {

                /* Remove this BSS Desc from the BSS Desc list */
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_SCAN, " SCN_RM_POLICY_EXCLUDE_DESIRED_SSID_LIST:remove ssid len:%d	ssid:[%s],BSSID["MACSTR"] ",prBssDesc->ucSSIDLen,prBssDesc->aucSSID,MAC2STR(prBssDesc->aucBSSID));
				LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDesc);

				/* Return this BSS Desc to the free BSS Desc list. */
				LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDesc->rLinkEntry);
			}
		}
	}
	else if(u4RemovePolicy&SCN_RM_POLICY_WEAKEST_BSSID_OF_MAXNUM_BSSID_OF_SAME_SSID_IN_DESIRED_SSID_LIST)
	{
		P_BSS_DESC_T prBssDescWeakestOfSameSSID = (P_BSS_DESC_T)NULL;
		SSID_LIST_T *ssid_list_ptr= NULL;
		SSID_LIST_T *ssid_list_ptr2= NULL;
		kal_uint8	*bssid_num_list_ptr;
		kal_uint8	size=0;
		kal_uint8	i=0;
		kal_uint8	u1maxbssid_num=0;
		kal_uint8	u1maxbssid_num_Index=0;
		
		size = (sizeof(kal_uint8))*(prscanDesiredSSIDList->u4NumElem);
		bssid_num_list_ptr =(kal_uint8*)kalMemAlloc(size,IN VIR_MEM_TYPE);
		kalMemSet(bssid_num_list_ptr,0,size);
		
		if(prscanDesiredSSIDList->u4NumElem==0)
		{
			
			kal_wap_trace(MOD_WNDRV,TRACE_WARNING,"desired ssid list is empty ");
			return;
		}

		//1.find the index of max bssid number of the same ssid in rscanDesiredSSIDList */

		LINK_FOR_EACH_ENTRY(ssid_list_ptr, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
		{

			LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) 
			{

				if(prBssDesc->fgIsHiddenSSID)
				{
					
					/* Don't remove hidden ssid. */
					continue;
				}
				
				if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) &&\
					(prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) 
				{
					/* Don't remove the one currently we are connected. */
					continue;
				}
				
				if(EQUAL_SSID(ssid_list_ptr->ssid_list->ssid,\
											ssid_list_ptr->ssid_list->ssid_len,\
											prBssDesc->aucSSID,\
											prBssDesc->ucSSIDLen)) 
					{
						bssid_num_list_ptr[i]++;
					}	
			}
			
			MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SCAN,"rscanDesiredSSIDList(%d)[%s] include %d bssid",i,ssid_list_ptr->ssid_list->ssid,bssid_num_list_ptr[i]);
			if(u1maxbssid_num<=bssid_num_list_ptr[i])
			{
				u1maxbssid_num = bssid_num_list_ptr[i];
				u1maxbssid_num_Index=i;
			}
			i++;		
		}
		
		kalMemFree(&bssid_num_list_ptr[0],VIR_MEM_TYPE,size);
		
		//2. search index to find ssid_list_ptr
		i=0;
		LINK_FOR_EACH_ENTRY(ssid_list_ptr, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
		{
			if(i==u1maxbssid_num_Index)
			{
				ssid_list_ptr2 = ssid_list_ptr;
				break;
			}
			i++;	
		}

		//3. find out the weakest BssDesc according to ssid_list_ptr
		LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) 
		{

				if ((u4RemovePolicy & SCN_RM_POLICY_EXCLUDE_CONNECTED) &&\
					(prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)) 
				{
					/* Don't remove the one currently we are connected. */
					continue;
				}
				if(prBssDesc->fgIsHiddenSSID)
				{
					
					/* Don't remove the one currently we are connected. */
					continue;
				}
				if(EQUAL_SSID(ssid_list_ptr2->ssid_list->ssid,\
											ssid_list_ptr2->ssid_list->ssid_len,\
											prBssDesc->aucSSID,\
											prBssDesc->ucSSIDLen)) 
				{


					if (!prBssDescWeakestOfSameSSID) {
						prBssDescWeakestOfSameSSID = prBssDesc;
					}
					else if (prBssDesc->ucRCPI <= prBssDescWeakestOfSameSSID->ucRCPI) {
						prBssDescWeakestOfSameSSID = prBssDesc;
					}	
				}
		}
		
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SCAN,"ssid[%s] is maxnum and bssid["MACSTR"] is weakest ",\
		prBssDescWeakestOfSameSSID->aucSSID,MAC2STR(prBssDescWeakestOfSameSSID->aucBSSID));
		
		//4. remove this BSS Desc from the BSS Desc list 
		
		kal_wap_trace(MOD_WNDRV,TRACE_SCAN,"SCN_RM_POLICY_WEAKEST_BSSID_OF_MAXNUM_BSSID_OF_SAME_SSID_IN_DESIRED_SSID_LIST:\remove ssid len:%d  ssid:[%s],BSSID["MACSTR"] ",\
		prBssDescWeakestOfSameSSID->ucSSIDLen,prBssDescWeakestOfSameSSID->aucSSID,MAC2STR(prBssDescWeakestOfSameSSID->aucBSSID));

		LINK_REMOVE_KNOWN_ENTRY(prBSSDescList,prBssDescWeakestOfSameSSID);

		/* Return this BSS Desc to the free BSS Desc list. */
		LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDescWeakestOfSameSSID->rLinkEntry);
		
	}
#endif

    return;

} /* end of scanRemoveBssDescsByPolicy() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Delete BSS Descriptors from current list according to given BSSID.
*
* @param[in] prAdapter  Pointer to the Adapter structure.
* @param[in] aucBSSID   Given BSSID.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
scanRemoveBssDescByBssid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
    )
{
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prBSSDescList;
    P_LINK_T prFreeBSSDescList;
    P_BSS_DESC_T prBssDesc = (P_BSS_DESC_T)NULL;
    P_BSS_DESC_T prBSSDescNext;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SCAN,"scanRemoveBssDescByBssid()");
    ASSERT(prAdapter);
    ASSERT(aucBSSID);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prBSSDescList = &prScanInfo->rBSSDescList;
    prFreeBSSDescList = &prScanInfo->rFreeBSSDescList;

    /* Check if such BSS Descriptor exists in a valid list */
    LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList, rLinkEntry, BSS_DESC_T) {

        if (EQUAL_MAC_ADDR(prBssDesc->aucBSSID, aucBSSID)) {

            /* Remove this BSS Desc from the BSS Desc list */
            LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDesc);
#if CFG_ENABLE_WIFI_DIRECT
            if(prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc);
                prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc(prAdapter, prBssDesc);
            }
#endif

            /* Return this BSS Desc to the free BSS Desc list. */
            LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDesc->rLinkEntry);

            /* BSSID is not unique, so need to traverse whols link-list */
        }
    }

    return;
} /* end of scanRemoveBssDescByBssid() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Delete BSS Descriptors from current list according to given band configuration
*
* @param[in] prAdapter  Pointer to the Adapter structure.
* @param[in] eBand      Given band
* @param[in] eNetTypeIndex  AIS - Remove IBSS/Infrastructure BSS
*                           BOW - Remove BOW BSS
*                           P2P - Remove P2P BSS
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
scanRemoveBssDescByBandAndNetwork (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BAND_T eBand,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
    )
{
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prBSSDescList;
    P_LINK_T prFreeBSSDescList;
    P_BSS_DESC_T prBssDesc = (P_BSS_DESC_T)NULL;
    P_BSS_DESC_T prBSSDescNext;
    BOOLEAN fgToRemove;

    ASSERT(prAdapter);
    ASSERT(eBand <= BAND_NUM);
    ASSERT(eNetTypeIndex <= NETWORK_TYPE_INDEX_NUM);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prBSSDescList = &prScanInfo->rBSSDescList;
    prFreeBSSDescList = &prScanInfo->rFreeBSSDescList;


    if(eBand == BAND_NULL) {
        return; /* no need to do anything, keep all scan result */
    }

    /* Check if such BSS Descriptor exists in a valid list */
    LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList, rLinkEntry, BSS_DESC_T) {
        fgToRemove = FALSE;

        if(prBssDesc->eBand == eBand) {
            switch (eNetTypeIndex) {
            case NETWORK_TYPE_AIS_INDEX:
                if((prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE)
                        || (prBssDesc->eBSSType == BSS_TYPE_IBSS)) {
                    fgToRemove = TRUE;
                }
                break;

            case NETWORK_TYPE_P2P_INDEX:
                if(prBssDesc->eBSSType == BSS_TYPE_P2P_DEVICE) {
                    fgToRemove = TRUE;
                }
                break;
                
            case NETWORK_TYPE_BOW_INDEX:
                if(prBssDesc->eBSSType == BSS_TYPE_BOW_DEVICE) {
                    fgToRemove = TRUE;
                }
                break;
            
            default:
                ASSERT(0);
                break;
            }
        }

        if(fgToRemove == TRUE) {
            /* Remove this BSS Desc from the BSS Desc list */
            LINK_REMOVE_KNOWN_ENTRY(prBSSDescList, prBssDesc);
#if CFG_ENABLE_WIFI_DIRECT
            if(prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc);
                prAdapter->rP2pFuncLkr.prScanRemoveP2pBssDesc(prAdapter, prBssDesc);
            }
#endif

            /* Return this BSS Desc to the free BSS Desc list. */
            LINK_INSERT_TAIL(prFreeBSSDescList, &prBssDesc->rLinkEntry);
        }
    }

    return;
} /* end of scanRemoveBssDescByBand() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Clear the CONNECTION FLAG of a specified BSS Descriptor.
*
* @param[in] aucBSSID   Given BSSID.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
scanRemoveConnFlagOfBssDescByBssid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
    )
{
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prBSSDescList;
    P_BSS_DESC_T prBssDesc = (P_BSS_DESC_T)NULL;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SCAN,"scanRemoveConnFlagOfBssDescByBssid()");

    ASSERT(prAdapter);
    ASSERT(aucBSSID);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prBSSDescList = &prScanInfo->rBSSDescList;

    /* Search BSS Desc from current SCAN result list. */
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {

        if (EQUAL_MAC_ADDR(prBssDesc->aucBSSID, aucBSSID)) {
            prBssDesc->fgIsConnected = FALSE;
            prBssDesc->fgIsConnecting = FALSE;

            /* BSSID is not unique, so need to traverse whols link-list */
        }
    }

    return;

} /* end of scanRemoveConnectionFlagOfBssDescByBssid() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Allocate new BSS_DESC_T
*
* @param[in] prAdapter          Pointer to the Adapter structure.
*
* @return   Pointer to BSS Descriptor, if has free space. NULL, if has no space.
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanAllocateBssDesc (
    IN P_ADAPTER_T prAdapter
    )
{
    P_SCAN_INFO_T prScanInfo;
    P_LINK_T prFreeBSSDescList;
    P_BSS_DESC_T prBssDesc;


    ASSERT(prAdapter);
    prScanInfo =  &(prAdapter->rWifiVar.rScanInfo);

    prFreeBSSDescList = &prScanInfo->rFreeBSSDescList;

    LINK_REMOVE_HEAD(prFreeBSSDescList, prBssDesc, P_BSS_DESC_T);

    if (prBssDesc) {
        P_LINK_T prBSSDescList;

        kalMemZero(prBssDesc, sizeof(BSS_DESC_T));

#if CFG_ENABLE_WIFI_DIRECT
        LINK_INITIALIZE(&(prBssDesc->rP2pDeviceList));
        prBssDesc->fgIsP2PPresent = FALSE;
#endif /* CFG_ENABLE_WIFI_DIRECT */

        prBSSDescList = &prScanInfo->rBSSDescList;

        /* NOTE(Kevin): In current design, this new empty BSS_DESC_T will be
         * inserted to BSSDescList immediately.
         */
        LINK_INSERT_TAIL(prBSSDescList, &prBssDesc->rLinkEntry);
    }

    return prBssDesc;

} /* end of scanAllocateBssDesc() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This API parses Beacon/ProbeResp frame and insert extracted BSS_DESC_T
*        with IEs into prAdapter->rWifiVar.rScanInfo.aucScanBuffer
*
* @param[in] prAdapter      Pointer to the Adapter structure.
* @param[in] prSwRfb        Pointer to the receiving frame buffer.
*
* @return   Pointer to BSS Descriptor
*           NULL if the Beacon/ProbeResp frame is invalid
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanAddToBssDesc (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
    )
{
    P_BSS_DESC_T prBssDesc = NULL;
    UINT_16 u2CapInfo;
    ENUM_BSS_TYPE_T eBSSType = BSS_TYPE_INFRASTRUCTURE;

    PUINT_8 pucIE;
    UINT_16 u2IELength;
    UINT_16 u2Offset = 0;
    #if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
	P_LINK_T	prscanDesiredSSIDList;
	#endif

    P_WLAN_BEACON_FRAME_T prWlanBeaconFrame = (P_WLAN_BEACON_FRAME_T)NULL;
    P_IE_SSID_T prIeSsid = (P_IE_SSID_T)NULL;
    P_IE_SUPPORTED_RATE_T prIeSupportedRate = (P_IE_SUPPORTED_RATE_T)NULL;
    P_IE_EXT_SUPPORTED_RATE_T prIeExtSupportedRate = (P_IE_EXT_SUPPORTED_RATE_T)NULL;
    P_HIF_RX_HEADER_T prHifRxHdr;
    UINT_8 ucHwChannelNum = 0;
    UINT_8 ucIeDsChannelNum = 0;
    UINT_8 ucIeHtChannelNum = 0;
    BOOLEAN fgIsValidSsid = FALSE, fgEscape = FALSE;
    PARAM_SSID_T rSsid;
    UINT_64 u8Timestamp;

    UINT_32 i;
    UINT_8 ucSSIDChar;

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prWlanBeaconFrame = (P_WLAN_BEACON_FRAME_T)prSwRfb->pvHeader;
    
	#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
	prscanDesiredSSIDList = &(prAdapter->rWifiVar.rScanInfo.rscanDesiredSSIDList);
	#endif
	
    WLAN_GET_FIELD_16(&prWlanBeaconFrame->u2CapInfo, &u2CapInfo);
    WLAN_GET_FIELD_64(&prWlanBeaconFrame->au4Timestamp[0], &u8Timestamp);

    // decide BSS type
    switch (u2CapInfo & CAP_INFO_BSS_TYPE) {
    case CAP_INFO_ESS:
        /* It can also be Group Owner of P2P Group. */
        eBSSType = BSS_TYPE_INFRASTRUCTURE;
        break;

    case CAP_INFO_IBSS:
        eBSSType = BSS_TYPE_IBSS;
        break;
    case 0:
        /* The P2P Device shall set the ESS bit of the Capabilities field in the Probe Response fame to 0 and IBSS bit to 0. (3.1.2.1.1) */
        eBSSType = BSS_TYPE_P2P_DEVICE;
        break;

#if CFG_ENABLE_BT_OVER_WIFI
        // @TODO: add rule to identify BOW beacons
#endif

    default:
        return NULL;
    }

    //4 <1.1> Pre-parse SSID IE
    pucIE = prWlanBeaconFrame->aucInfoElem;
    u2IELength = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
        (UINT_16)OFFSET_OF(WLAN_BEACON_FRAME_BODY_T, aucInfoElem[0]);

    if (u2IELength > CFG_IE_BUFFER_SIZE) {
        u2IELength = CFG_IE_BUFFER_SIZE;
    }

    IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
        switch (IE_ID(pucIE)) {
        case ELEM_ID_SSID:
            if (IE_LEN(pucIE) <= ELEM_MAX_LEN_SSID) {
                ucSSIDChar = '\0';

                /* D-Link DWL-900AP+ */
                if (IE_LEN(pucIE) == 0) {
                    fgIsValidSsid = FALSE;
                }
                /* Cisco AP1230A - (IE_LEN(pucIE) == 1) && (SSID_IE(pucIE)->aucSSID[0] == '\0') */
                /* Linksys WRK54G/ASUS WL520g - (IE_LEN(pucIE) == n) && (SSID_IE(pucIE)->aucSSID[0~(n-1)] == '\0') */
                else {
                    for (i = 0; i < IE_LEN(pucIE); i++) {
                        ucSSIDChar |= SSID_IE(pucIE)->aucSSID[i];
                    }

                    if (ucSSIDChar) {
                        fgIsValidSsid = TRUE;
                    }
                }

                /* Update SSID to BSS Descriptor only if SSID is not hidden. */
                if (fgIsValidSsid == TRUE) {
                    COPY_SSID(rSsid.aucSsid,
                              rSsid.u4SsidLen,
                              SSID_IE(pucIE)->aucSSID,
                              SSID_IE(pucIE)->ucLength);
                }
            }
            fgEscape = TRUE;
            break;
        default:
            break;
        }

        if(fgEscape == TRUE) {
            break;
        }
    }


    //4 <1.2> Replace existing BSS_DESC_T or allocate a new one
    prBssDesc = scanSearchExistingBssDescWithSsid(prAdapter,
                                          eBSSType,
                                          (PUINT_8)prWlanBeaconFrame->aucBSSID,
                                          (PUINT_8)prWlanBeaconFrame->aucSrcAddr,
                                          fgIsValidSsid,
                                          fgIsValidSsid == TRUE ? &rSsid : NULL);

    if (prBssDesc == (P_BSS_DESC_T)NULL) {
        do {

			#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
            //Check whether in desired ssid list
			if(prscanDesiredSSIDList->u4NumElem)
			{
				SSID_LIST_T *ssid_list_ptr;
				kal_uint8	fgIsInSSIDList=KAL_FALSE;
				LINK_FOR_EACH_ENTRY(ssid_list_ptr, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
				{
					

					if(EQUAL_SSID(ssid_list_ptr->ssid_list->ssid,\
		                            ssid_list_ptr->ssid_list->ssid_len,\
		                            rSsid.aucSsid,\
		                            rSsid.u4SsidLen)) 
		                {                	
		                	fgIsInSSIDList=KAL_TRUE;
		                    break;
		                }		                
					
				}

				if(fgIsInSSIDList==KAL_FALSE)//not in ssid list 
				{
					MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SCAN,"  ssid[%s] is not in desired ssid list,do not alloc BssDesc ",rSsid.aucSsid);
					return NULL;
				}
			}
			#endif
        
            //4 <1.2.1> First trial of allocation
            prBssDesc = scanAllocateBssDesc(prAdapter);
            if (prBssDesc) {
                break;
            }

            //4 <1.2.2> Hidden is useless, remove the oldest hidden ssid. (for passive scan)
            scanRemoveBssDescsByPolicy(prAdapter,
                    (SCN_RM_POLICY_EXCLUDE_CONNECTED | SCN_RM_POLICY_OLDEST_HIDDEN));

            //4 <1.2.3> Second tail of allocation
            prBssDesc = scanAllocateBssDesc(prAdapter);
            if (prBssDesc) {
                break;
            }

			#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
			 //alread in desired ssid list, remove AP which is not in desired ssid list
			 if(prscanDesiredSSIDList->u4NumElem)
			 {	 
				 scanRemoveBssDescsByPolicy(prAdapter,SCN_RM_POLICY_EXCLUDE_CONNECTED|SCN_RM_POLICY_EXCLUDE_DESIRED_SSID_LIST);
				 prBssDesc = scanAllocateBssDesc(prAdapter);
				 if (prBssDesc) 
				 {
					 break;
				 }
			 }
			 
			 //remove smart weakest AP according to desired ssid list
			 if(prscanDesiredSSIDList->u4NumElem)
			 {	 
				 scanRemoveBssDescsByPolicy(prAdapter,SCN_RM_POLICY_EXCLUDE_CONNECTED|SCN_RM_POLICY_WEAKEST_BSSID_OF_MAXNUM_BSSID_OF_SAME_SSID_IN_DESIRED_SSID_LIST);
				 prBssDesc = scanAllocateBssDesc(prAdapter);				 
				 if (prBssDesc) 
				 {
					 break;
				 }
			 }
			#endif
			
            //4 <1.2.4> Remove the weakest one
            /* If there are more than half of BSS which has the same ssid as connection
             * setting, remove the weakest one from them.
             * Else remove the weakest one.
             */
			scanRemoveBssDescsByPolicy(prAdapter,
					(SCN_RM_POLICY_EXCLUDE_CONNECTED | SCN_RM_POLICY_SMART_WEAKEST|SCN_RM_POLICY_EXCLUDE_SCANREQ_SSID));


            //4 <1.2.5> reallocation
            prBssDesc = scanAllocateBssDesc(prAdapter);
            if (prBssDesc) {
                break;
            }

            //4 <1.2.6> no space, should not happen
            //ASSERT(0); // still no space available ?
            return NULL;

        }
        while(FALSE);

    }
    else {
        OS_SYSTIME rCurrentTime;

		#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
		//Check whether in desired ssid list
		if(prscanDesiredSSIDList->u4NumElem)
		{
			SSID_LIST_T *ssid_list_ptr;
			kal_uint8	fgIsInSSIDList=KAL_FALSE;
			
			if(prBssDesc->fgIsConnected || prBssDesc->fgIsConnecting)
			{
				/* Don't remove the one currently we are connected. */
				MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_1," bssid["MACSTR"] is connected",MAC2STR(prBssDesc->aucBSSID));
			}
			else
			{
				LINK_FOR_EACH_ENTRY(ssid_list_ptr, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
				{
					
			
					if(EQUAL_SSID(ssid_list_ptr->ssid_list->ssid,\
									ssid_list_ptr->ssid_list->ssid_len,\
									prBssDesc->aucSSID,\
									prBssDesc->ucSSIDLen)) 
						{
							
							MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_5,"	ssid[%s] is in desired ssid list, use previous BssDesc ",prBssDesc->aucSSID);
							fgIsInSSIDList=KAL_TRUE;
							break;
						}						
					
				}
			
				if(fgIsInSSIDList==KAL_FALSE)//not in ssid list , remove
				{
					MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_5,"	ssid[%s] is not in desired ssid list,remove previous BssDesc ",prBssDesc->aucSSID);
					scanRemoveBssDescByBssid(prAdapter, prBssDesc->aucBSSID);
					return NULL;
				}
			}
		}
		#endif

		
        // WCXRP00000091
        // if the received strength is much weaker than the original one,
        // ignore it due to it might be received on the folding frequency

        GET_CURRENT_SYSTIME(&rCurrentTime);

        prBssDesc->eBSSType = eBSSType;

        if(HIF_RX_HDR_GET_CHNL_NUM(prSwRfb->prHifRxHdr) != prBssDesc->ucChannelNum &&
                prBssDesc->ucRCPI > prSwRfb->prHifRxHdr->ucRcpi) {

            // for signal strength is too much weaker and previous beacon is not stale
            if((prBssDesc->ucRCPI - prSwRfb->prHifRxHdr->ucRcpi) >= REPLICATED_BEACON_STRENGTH_THRESHOLD &&
                    rCurrentTime - prBssDesc->rUpdateTime <= REPLICATED_BEACON_FRESH_PERIOD) {
                return prBssDesc;
            }
            // for received beacons too close in time domain
            else if(rCurrentTime - prBssDesc->rUpdateTime <= REPLICATED_BEACON_TIME_THRESHOLD) {
                return prBssDesc;
            }
        }

        /* if Timestamp has been reset, re-generate BSS DESC 'cause AP should have reset itself */
        if(prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE && u8Timestamp < prBssDesc->u8TimeStamp.QuadPart) {
            scanRemoveBssDescByBssid(prAdapter, prBssDesc->aucBSSID);

            prBssDesc = scanAllocateBssDesc(prAdapter);
            if (!prBssDesc) {
                return NULL;
            }
        }
    }

    /* NOTE: Keep consistency of Scan Record during JOIN process */
    if (prBssDesc->fgIsConnecting) {
        return prBssDesc;
    }

    //4 <2> Get information from Fixed Fields
    prBssDesc->eBSSType = eBSSType; /* Update the latest BSS type information. */

    COPY_MAC_ADDR(prBssDesc->aucSrcAddr, prWlanBeaconFrame->aucSrcAddr);

    COPY_MAC_ADDR(prBssDesc->aucBSSID, prWlanBeaconFrame->aucBSSID);

    prBssDesc->u8TimeStamp.QuadPart = u8Timestamp;

    WLAN_GET_FIELD_16(&prWlanBeaconFrame->u2BeaconInterval, &prBssDesc->u2BeaconInterval);

    prBssDesc->u2CapInfo = u2CapInfo;


    //4 <2.1> Retrieve IEs for later parsing
    u2IELength = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
        (UINT_16)OFFSET_OF(WLAN_BEACON_FRAME_BODY_T, aucInfoElem[0]);

    if (u2IELength > CFG_IE_BUFFER_SIZE) {
        u2IELength = CFG_IE_BUFFER_SIZE;
        prBssDesc->fgIsIEOverflow = TRUE;
    }
    else {
        prBssDesc->fgIsIEOverflow = FALSE;
    }
    prBssDesc->u2IELength = u2IELength;

    kalMemCopy(prBssDesc->aucIEBuf, prWlanBeaconFrame->aucInfoElem, u2IELength);

    //4 <2.2> reset prBssDesc variables in case that AP has been reconfigured
    prBssDesc->fgIsERPPresent = FALSE;
    prBssDesc->fgIsHTPresent = FALSE;
    prBssDesc->eSco = CHNL_EXT_SCN;
#ifdef __WAPI_SUPPORT__
    prBssDesc->fgIEWAPI = FALSE;
#endif
#if CFG_RSN_MIGRATION
    prBssDesc->fgIERSN = FALSE;
#endif
#if CFG_PRIVACY_MIGRATION
    prBssDesc->fgIEWPA = FALSE;
#endif


    //4 <3.1> Full IE parsing on SW_RFB_T
    pucIE = prWlanBeaconFrame->aucInfoElem;


    IE_FOR_EACH(pucIE, u2IELength, u2Offset) {

        switch (IE_ID(pucIE)) {
        case ELEM_ID_SSID:
            if ((!prIeSsid) && /* NOTE(Kevin): for Atheros IOT #1 */
                (IE_LEN(pucIE) <= ELEM_MAX_LEN_SSID)) {
                BOOLEAN fgIsHiddenSSID = FALSE;
                UINT_32 i;
                UINT_8 ucSSIDChar = '\0';


                prIeSsid = (P_IE_SSID_T)pucIE;

                /* D-Link DWL-900AP+ */
                if (IE_LEN(pucIE) == 0) {
                    fgIsHiddenSSID = TRUE;
                }
                /* Cisco AP1230A - (IE_LEN(pucIE) == 1) && (SSID_IE(pucIE)->aucSSID[0] == '\0') */
                /* Linksys WRK54G/ASUS WL520g - (IE_LEN(pucIE) == n) && (SSID_IE(pucIE)->aucSSID[0~(n-1)] == '\0') */
                else {
                    for (i = 0; i < IE_LEN(pucIE); i++) {
                        ucSSIDChar |= SSID_IE(pucIE)->aucSSID[i];
                    }

                    if (!ucSSIDChar) {
                        fgIsHiddenSSID = TRUE;
                    }
                }

                /* Update SSID to BSS Descriptor only if SSID is not hidden. */
                //if (!fgIsHiddenSSID)  /* NOTE(Nelson): If is hidden SSID, update the BSS Descriptor, then update to SUPC */
                {
                    COPY_SSID(prBssDesc->aucSSID,
                              prBssDesc->ucSSIDLen,
                              SSID_IE(pucIE)->aucSSID,
                              SSID_IE(pucIE)->ucLength);
                }

                prBssDesc->fgIsHiddenSSID = fgIsHiddenSSID;
            }
            break;

        case ELEM_ID_SUP_RATES:
            /* NOTE(Kevin): Buffalo WHR-G54S's supported rate set IE exceed 8.
             * IE_LEN(pucIE) == 12, "1(B), 2(B), 5.5(B), 6(B), 9(B), 11(B),
             * 12(B), 18(B), 24(B), 36(B), 48(B), 54(B)"
             */
            /* TP-LINK will set extra and incorrect ie with ELEM_ID_SUP_RATES */
            if ((!prIeSupportedRate) && (IE_LEN(pucIE) <= RATE_NUM)) {
                prIeSupportedRate = SUP_RATES_IE(pucIE);
            }
            break;

        case ELEM_ID_DS_PARAM_SET:
            if (IE_LEN(pucIE) == ELEM_MAX_LEN_DS_PARAMETER_SET) {
                ucIeDsChannelNum = DS_PARAM_IE(pucIE)->ucCurrChnl;
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_135, "ELEM_ID_DS_PARAM_SET: ucIeDsChannelNum %d", ucIeDsChannelNum);
            }
            break;

        case ELEM_ID_TIM:
            if (IE_LEN(pucIE) <= ELEM_MAX_LEN_TIM) {
                prBssDesc->ucDTIMPeriod = TIM_IE(pucIE)->ucDTIMPeriod;
            }
            break;

        case ELEM_ID_IBSS_PARAM_SET:
            if (IE_LEN(pucIE) == ELEM_MAX_LEN_IBSS_PARAMETER_SET){
                prBssDesc->u2ATIMWindow = IBSS_PARAM_IE(pucIE)->u2ATIMWindow;
            }
            break;

#if 0 //CFG_SUPPORT_802_11D
        case ELEM_ID_COUNTRY_INFO:
            prBssDesc->prIECountry = (P_IE_COUNTRY_T)pucIE;
            break;
#endif

        case ELEM_ID_ERP_INFO:
            if (IE_LEN(pucIE) == ELEM_MAX_LEN_ERP) {
                prBssDesc->fgIsERPPresent = TRUE;
            }
            break;

        case ELEM_ID_EXTENDED_SUP_RATES:
            if (!prIeExtSupportedRate) {
                prIeExtSupportedRate = EXT_SUP_RATES_IE(pucIE);
            }
            break;

#if CFG_RSN_MIGRATION
        case ELEM_ID_RSN:
            if (rsnParseRsnIE(prAdapter, RSN_IE(pucIE), &prBssDesc->rRSNInfo)) {
                prBssDesc->fgIERSN = TRUE;
                prBssDesc->u2RsnCap = prBssDesc->rRSNInfo.u2RsnCap;
                kalMemCopy( prBssDesc->aucIERSN, pucIE, IE_LEN(pucIE)+2 );
                if (prAdapter->rWifiVar.rConnSettings.eAuthMode == AUTH_MODE_WPA2) {
                    rsnCheckPmkidCache(prAdapter, prBssDesc);
                }
            #if defined (__WIFI_SLIM__)
                if (prBssDesc->rRSNInfo.au4AuthKeyMgtSuite[0] == RSN_AKM_SUITE_PSK) {
                    prBssDesc->fgPSK = TRUE;
                    MT5931_TRACE0(TRACE_SCAN, MT5931_INFO_136, "CCMP PSK");
                }
            #endif /* __WIFI_SLIM__ */
            }
            break;
#endif

        case ELEM_ID_HT_CAP:
            prBssDesc->fgIsHTPresent = TRUE;
            break;

        case ELEM_ID_HT_OP:
            if (IE_LEN(pucIE) != (sizeof(IE_HT_OP_T) - 2)) {
                break;
            }

            if ((((P_IE_HT_OP_T) pucIE)->ucInfo1 & HT_OP_INFO1_SCO) != CHNL_EXT_RES) {
                prBssDesc->eSco = (ENUM_CHNL_EXT_T)
                    (((P_IE_HT_OP_T) pucIE)->ucInfo1 & HT_OP_INFO1_SCO);
            }
            ucIeHtChannelNum = ((P_IE_HT_OP_T) pucIE)->ucPrimaryChannel;
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_137, "ELEM_ID_HT_OP: ucIeHtChannelNum %d", ucIeHtChannelNum);

            break;

#ifdef __WAPI_SUPPORT__
        case ELEM_ID_WAPI:
            if (wapiParseWapiIE(WAPI_IE(pucIE), &prBssDesc->rIEWAPI)) {
                prBssDesc->fgIEWAPI = TRUE;
                kalMemCopy( prBssDesc->aucIEWAPI, pucIE, IE_LEN(pucIE)+2 );
            }
            break;
#endif

        case ELEM_ID_VENDOR:  // ELEM_ID_P2P, ELEM_ID_WMM
        {
            UINT_8 ucOuiType;
            UINT_16 u2SubTypeVersion;
#if CFG_PRIVACY_MIGRATION
            if (rsnParseCheckForWFAInfoElem(prAdapter, pucIE, &ucOuiType, &u2SubTypeVersion)) {
                if ((ucOuiType == VENDOR_OUI_TYPE_WPA) &&
                        (u2SubTypeVersion == VERSION_WPA)) {

                    if (rsnParseWpaIE(prAdapter, WPA_IE(pucIE), &prBssDesc->rWPAInfo)) {
                        prBssDesc->fgIEWPA = TRUE;
                        kalMemCopy( prBssDesc->aucIEWPA, pucIE, IE_LEN(pucIE)+2 );
                    #if defined (__WIFI_SLIM__)
                        if (prBssDesc->rWPAInfo.au4AuthKeyMgtSuite[0] == WPA_AKM_SUITE_PSK) {
                            prBssDesc->fgPSK = TRUE;
                            MT5931_TRACE0(TRACE_SCAN, MT5931_INFO_138, "WPA PSK");
                        }
                    #endif /* __WIFI_SLIM__ */
                    }
                }
            }
#endif

#if CFG_ENABLE_WIFI_DIRECT
            if(prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prP2pParseCheckForP2pInfoElem);
                if (prAdapter->rP2pFuncLkr.prP2pParseCheckForP2pInfoElem(prAdapter, pucIE, &ucOuiType)) {
                    if (ucOuiType == VENDOR_OUI_TYPE_P2P) {
                        prBssDesc->fgIsP2PPresent = TRUE;
                    }
                }
            }
#endif /* CFG_ENABLE_WIFI_DIRECT */
        }
        break;

        /* no default */
        }
    }


    //4 <3.2> Save information from IEs - SSID
    /* Update Flag of Hidden SSID for used in SEARCH STATE. */

    /* NOTE(Kevin): in current driver, the ucSSIDLen == 0 represent
     * all cases of hidden SSID.
     * If the fgIsHiddenSSID == TRUE, it means we didn't get the ProbeResp with
     * valid SSID.
     */
#if 0
    if (prBssDesc->ucSSIDLen == 0) {
        prBssDesc->fgIsHiddenSSID = TRUE;
    }
    else {
        prBssDesc->fgIsHiddenSSID = FALSE;
    }
#endif


    //4 <3.3> Check rate information in related IEs.
    if (prIeSupportedRate || prIeExtSupportedRate) {
        rateGetRateSetFromIEs(prIeSupportedRate,
                              prIeExtSupportedRate,
                              &prBssDesc->u2OperationalRateSet,
                              &prBssDesc->u2BSSBasicRateSet,
                              &prBssDesc->fgIsUnknownBssBasicRate);
    }


    //4 <4> Update information from HIF RX Header
    {
        prHifRxHdr = prSwRfb->prHifRxHdr;

        ASSERT(prHifRxHdr);

        //4 <4.1> Get TSF comparison result
        prBssDesc->fgIsLargerTSF = HIF_RX_HDR_GET_TCL_FLAG(prHifRxHdr);

        //4 <4.2> Get Band information
        prBssDesc->eBand = HIF_RX_HDR_GET_RF_BAND(prHifRxHdr);

        //4 <4.2> Get channel and RCPI information
        ucHwChannelNum = HIF_RX_HDR_GET_CHNL_NUM(prHifRxHdr);

        if (BAND_2G4 == prBssDesc->eBand) {

            /* Update RCPI if in right channel */
            if (ucIeDsChannelNum >= 1 && ucIeDsChannelNum <= 14) {

                // Receive Beacon/ProbeResp frame from adjacent channel.
                if ((ucIeDsChannelNum == ucHwChannelNum) ||
                    (prHifRxHdr->ucRcpi > prBssDesc->ucRCPI)) {
                    prBssDesc->ucRCPI = prHifRxHdr->ucRcpi;
                }

                // trust channel information brought by IE
                prBssDesc->ucChannelNum = ucIeDsChannelNum;
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_139, "scanAddToBssDesc(DS): ucChannelNum %d prBssDesc->ucRCPI %d prHifRxHdr->ucRcpi %d",
                    prBssDesc->ucChannelNum, prBssDesc->ucRCPI, prHifRxHdr->ucRcpi);
            }
            else if(ucIeHtChannelNum >= 1 && ucIeHtChannelNum <= 14) {
                // Receive Beacon/ProbeResp frame from adjacent channel.
                if ((ucIeHtChannelNum == ucHwChannelNum) ||
                    (prHifRxHdr->ucRcpi > prBssDesc->ucRCPI)) {
                    prBssDesc->ucRCPI = prHifRxHdr->ucRcpi;
                }

                // trust channel information brought by IE
                prBssDesc->ucChannelNum = ucIeHtChannelNum;
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_140, "scanAddToBssDesc(HT): ucChannelNum %d prBssDesc->ucRCPI %d prHifRxHdr->ucRcpi %d",
                    prBssDesc->ucChannelNum, prBssDesc->ucRCPI, prHifRxHdr->ucRcpi);
            }
            else {
                prBssDesc->ucRCPI = prHifRxHdr->ucRcpi;

                prBssDesc->ucChannelNum = ucHwChannelNum;
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_141, "scanAddToBssDesc(NONE): ucChannelNum %d prBssDesc->ucRCPI %d prHifRxHdr->ucRcpi %d",
                    prBssDesc->ucChannelNum, prBssDesc->ucRCPI, prHifRxHdr->ucRcpi);
            }
        }
        // 5G Band
        else {
            if(ucIeHtChannelNum >= 1 && ucIeHtChannelNum < 200) {
                // Receive Beacon/ProbeResp frame from adjacent channel.
                if ((ucIeHtChannelNum == ucHwChannelNum) ||
                    (prHifRxHdr->ucRcpi > prBssDesc->ucRCPI)) {
                    prBssDesc->ucRCPI = prHifRxHdr->ucRcpi;
                }

                // trust channel information brought by IE
                prBssDesc->ucChannelNum = ucIeHtChannelNum;
            }
            else {
                /* Always update RCPI */
                prBssDesc->ucRCPI = prHifRxHdr->ucRcpi;

                prBssDesc->ucChannelNum = ucHwChannelNum;
            }
        }
    #if 0   /* NOTE(Nelson): scan list is not maintained in BSS list, so not need to remove BSS here */
        /* CMCCTD to MAUI: CMCC 7.6.2 Only wifi signal which > -85 dbm will be found. */
        if (prBssDesc->ucRCPI < dBm_TO_RCPI(CFG_SCAN_AVAILABLE_RSSI_THRESHOLD)) {

            MT5931_TRACE(TRACE_GROUP_4, MT5931_INFO_, "scanAddToBssDesc: remove BSSID["MACSTR"] %d dBm < %d dBm",
                MAC2STR(prBssDesc->aucBSSID), RCPI_TO_dBm(prBssDesc->ucRCPI), CFG_SCAN_AVAILABLE_RSSI_THRESHOLD);
            scanRemoveBssDescByBssid(prAdapter, prBssDesc->aucBSSID);
            return NULL;
        }
    #endif
    }


    //4 <5> PHY type setting
    prBssDesc->ucPhyTypeSet = 0;

    if (BAND_2G4 == prBssDesc->eBand) {
        /* check if support 11n */
        if (prBssDesc->fgIsHTPresent) {
            prBssDesc->ucPhyTypeSet |= PHY_TYPE_BIT_HT;
        }

        /* if not 11n only */
        if (!(prBssDesc->u2BSSBasicRateSet & RATE_SET_BIT_HT_PHY)) {
            /* check if support 11g */
            if ((prBssDesc->u2OperationalRateSet & RATE_SET_OFDM) ||
                    prBssDesc->fgIsERPPresent) {
                prBssDesc->ucPhyTypeSet |= PHY_TYPE_BIT_ERP;
            }

            /* if not 11g only */
            if (!(prBssDesc->u2BSSBasicRateSet & RATE_SET_OFDM)) {
                /* check if support 11b */
                if ((prBssDesc->u2OperationalRateSet & RATE_SET_HR_DSSS)) {
                    prBssDesc->ucPhyTypeSet |= PHY_TYPE_BIT_HR_DSSS;
                }
            }
        }
    }
    else { /* (BAND_5G == prBssDesc->eBande) */
        /* check if support 11n */
        if (prBssDesc->fgIsHTPresent) {
            prBssDesc->ucPhyTypeSet |= PHY_TYPE_BIT_HT;
        }

        /* if not 11n only */
        if (!(prBssDesc->u2BSSBasicRateSet & RATE_SET_BIT_HT_PHY)) {
            /* Support 11a definitely */
            prBssDesc->ucPhyTypeSet |= PHY_TYPE_BIT_OFDM;

            ASSERT(!(prBssDesc->u2OperationalRateSet & RATE_SET_HR_DSSS));
        }
    }


    //4 <6> Update BSS_DESC_T's Last Update TimeStamp.
    GET_CURRENT_SYSTIME(&prBssDesc->rUpdateTime);

    return prBssDesc;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Convert the Beacon or ProbeResp Frame in SW_RFB_T to scan result for query
*
* @param[in] prSwRfb            Pointer to the receiving SW_RFB_T structure.
*
* @retval WLAN_STATUS_SUCCESS   It is a valid Scan Result and been sent to the host.
* @retval WLAN_STATUS_FAILURE   It is not a valid Scan Result.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
scanAddScanResult (
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prBssDesc,
    IN P_SW_RFB_T prSwRfb
    )
{
    //P_SCAN_INFO_T prScanInfo;
    UINT_8 aucRatesEx[PARAM_MAX_LEN_RATES_EX];
    P_WLAN_BEACON_FRAME_T prWlanBeaconFrame;
    PARAM_MAC_ADDRESS rMacAddr;
    PARAM_SSID_T rSsid;
    ENUM_PARAM_NETWORK_TYPE_T eNetworkType;
    PARAM_802_11_CONFIG_T rConfiguration;
    ENUM_PARAM_OP_MODE_T eOpMode;
    UINT_8 ucRateLen = 0;
    UINT_32 i;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"scanAddScanResult");

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    //prScanInfo =  &(prAdapter->rWifiVar.rScanInfo);

    if (prBssDesc->eBand == BAND_2G4) {
        if ((prBssDesc->u2OperationalRateSet & RATE_SET_OFDM)
                || prBssDesc->fgIsERPPresent) {
            eNetworkType = PARAM_NETWORK_TYPE_OFDM24;
        }
        else {
            eNetworkType = PARAM_NETWORK_TYPE_DS;
        }
    }
    else {
        ASSERT(prBssDesc->eBand == BAND_5G);
        eNetworkType = PARAM_NETWORK_TYPE_OFDM5;
    }

    if(prBssDesc->eBSSType == BSS_TYPE_P2P_DEVICE) {
        /* NOTE(Kevin): Not supported by WZC(TBD) */
        return WLAN_STATUS_FAILURE;
    }

	kal_mem_set(rSsid.aucSsid, 0x0, sizeof(rSsid.aucSsid));
    prWlanBeaconFrame = (P_WLAN_BEACON_FRAME_T)prSwRfb->pvHeader;
    COPY_MAC_ADDR(rMacAddr, prWlanBeaconFrame->aucBSSID);
    COPY_SSID(rSsid.aucSsid,
            rSsid.u4SsidLen,
            prBssDesc->aucSSID,
            prBssDesc->ucSSIDLen);

    rConfiguration.u4Length             = sizeof(PARAM_802_11_CONFIG_T);
    rConfiguration.u4BeaconPeriod       = (UINT_32) prWlanBeaconFrame->u2BeaconInterval;
    rConfiguration.u4ATIMWindow         = prBssDesc->u2ATIMWindow;
    rConfiguration.u4DSConfig           = nicChannelNum2Freq(prBssDesc->ucChannelNum);
    rConfiguration.rFHConfig.u4Length   = sizeof(PARAM_802_11_CONFIG_FH_T);

    rateGetDataRatesFromRateSet(prBssDesc->u2OperationalRateSet,
                                0,
                                aucRatesEx,
                                &ucRateLen);

    /* NOTE(Kevin): Set unused entries, if any, at the end of the array to 0.
     * from OID_802_11_BSSID_LIST
     */
    for (i = ucRateLen; i < sizeof(aucRatesEx) / sizeof(aucRatesEx[0]) ; i++) {
        aucRatesEx[i] = 0;
    }

    switch(prBssDesc->eBSSType) {
    case BSS_TYPE_IBSS:
        eOpMode = NET_TYPE_IBSS;
        break;

    case BSS_TYPE_INFRASTRUCTURE:
    case BSS_TYPE_P2P_DEVICE:
    case BSS_TYPE_BOW_DEVICE:
    default:
        eOpMode = NET_TYPE_INFRA;
        break;
    }

    nicAddScanResult(prAdapter,
            rMacAddr,
            &rSsid,
            prWlanBeaconFrame->u2CapInfo & CAP_INFO_PRIVACY ? 1 : 0,
            RCPI_TO_dBm(prBssDesc->ucRCPI),
            eNetworkType,
            &rConfiguration,
            eOpMode,
            aucRatesEx,
            prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen,
            (PUINT_8)((UINT_32)(prSwRfb->pvHeader) + WLAN_MAC_MGMT_HEADER_LEN));

    return WLAN_STATUS_SUCCESS;

} /* end of scanSendScanResultEvent() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Parse the content of given Beacon or ProbeResp Frame.
*
* @param[in] prSwRfb            Pointer to the receiving SW_RFB_T structure.
*
* @retval WLAN_STATUS_SUCCESS           if not report this SW_RFB_T to host
* @retval WLAN_STATUS_PENDING           if report this SW_RFB_T to host as scan result
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
scanProcessBeaconAndProbeResp (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
    )
{
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_DESC_T prBssDesc = (P_BSS_DESC_T)NULL;
    WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
    P_BSS_INFO_T prAisBssInfo;
    P_WLAN_BEACON_FRAME_T prWlanBeaconFrame = (P_WLAN_BEACON_FRAME_T)NULL;
#if CFG_SLT_SUPPORT
    P_SLT_INFO_T prSltInfo = (P_SLT_INFO_T)NULL;
#endif

    ASSERT(prAdapter);
    ASSERT(prSwRfb);
    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"scanProcessBeaconAndProbeResp()");
    //4 <0> Ignore invalid Beacon Frame
    if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) <
            (TIMESTAMP_FIELD_LEN + BEACON_INTERVAL_FIELD_LEN + CAP_INFO_FIELD_LEN)) {
        MT5931_NEW_TRACE(MT5931_INFO_429,  TRACE_ERROR, "Invalid Beacon Frame (u2PacketLen=%d, u2HeaderLen=%d)",
            prSwRfb->u2PacketLen, prSwRfb->u2HeaderLen );
        return rStatus;
    }

#if CFG_SLT_SUPPORT
    prSltInfo = &prAdapter->rWifiVar.rSltInfo;

    if (prSltInfo->fgIsDUT) {
        DBGLOG(P2P, INFO, ("\n\rBCN: RX\n"));
        prSltInfo->u4BeaconReceiveCnt++;
        return WLAN_STATUS_SUCCESS;
    }
    else {
        return WLAN_STATUS_SUCCESS;
    }
#endif


    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prWlanBeaconFrame = (P_WLAN_BEACON_FRAME_T)prSwRfb->pvHeader;

    //4 <1> Parse and add into BSS_DESC_T
    //MT5931_TRACE(TRACE_SCAN, MT5931_INFO_142, "prSwRfb->prHifRxHdr->ucRcpi %d", prSwRfb->prHifRxHdr->ucRcpi);
    //MT5931_TRACE(TRACE_SCAN, MT5931_INFO_143, "prSwRfb->prHifRxHdr->ucHwChannelNum %d", prSwRfb->prHifRxHdr->ucHwChannelNum);
    MT5931_TRACE(TRACE_SCAN, MT5931_INFO_144, "BSSID:"MACSTR, MAC2STR(((P_WLAN_BEACON_FRAME_T)prSwRfb->pvHeader)->aucBSSID ));

    prBssDesc = scanAddToBssDesc(prAdapter, prSwRfb);

    if (prBssDesc) {

        //4 <1.1> Beacon Change Detection for Connected BSS
        if(prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED &&
                ((prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE && prConnSettings->eOPMode != NET_TYPE_IBSS)
                 || (prBssDesc->eBSSType == BSS_TYPE_IBSS && prConnSettings->eOPMode != NET_TYPE_INFRA)) &&
                EQUAL_MAC_ADDR(prBssDesc->aucBSSID, prAisBssInfo->aucBSSID)) {
            BOOLEAN fgNeedDisconnect = FALSE;
#if CFG_SUPPORT_BEACON_CHANGE_DETECTION

            // <1.1.1> check if SSID differs
            if(UNEQUAL_SSID(prBssDesc->aucSSID, prBssDesc->ucSSIDLen, prAisBssInfo->aucSSID, prAisBssInfo->ucSSIDLen)) {
                fgNeedDisconnect = TRUE;
                MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"Passive discon1 SSID change: %d[%s]->%d[%s]",\
                prAisBssInfo->ucSSIDLen,prAisBssInfo->aucSSID,prBssDesc->ucSSIDLen,prBssDesc->aucSSID);
            }

            /* NOTE(Nelson): Do Not to indicate Beacon timeout while probe response with hidden SSID */
            if( prBssDesc->fgIsHiddenSSID == TRUE ) {
                fgNeedDisconnect = FALSE;
            }

            // <1.1.2> check if supported rate differs
            if(prAisBssInfo->u2OperationalRateSet != prBssDesc->u2OperationalRateSet)
			 {
                //fgNeedDisconnect = TRUE;
                MT5931_WAP_TRACE(MOD_WNDRV,TRACE_WARNING,"Ignore passive discon2 rate change: %d->%d",prAisBssInfo->u2OperationalRateSet,prBssDesc->u2OperationalRateSet);
            }
#endif

            // <1.1.3> beacon content change detected, disconnect immediately
            if(fgNeedDisconnect == TRUE) {
                aisBssBeaconTimeout(prAdapter);
            }
        }

        //4 <1.1> Update AIS_BSS_INFO
        if(((prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE && prConnSettings->eOPMode != NET_TYPE_IBSS)
                    || (prBssDesc->eBSSType == BSS_TYPE_IBSS && prConnSettings->eOPMode != NET_TYPE_INFRA))) {
            if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {

                /* *not* checking prBssDesc->fgIsConnected anymore,
                 * due to Linksys AP uses " " as hidden SSID, and would have different BSS descriptor */
                if ((!prAisBssInfo->ucDTIMPeriod) &&
                    EQUAL_MAC_ADDR(prBssDesc->aucBSSID, prAisBssInfo->aucBSSID) &&
                    (prAisBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE) &&
                    ((prWlanBeaconFrame->u2FrameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_BEACON)) {

                    prAisBssInfo->ucDTIMPeriod = prBssDesc->ucDTIMPeriod;

                    /* sync with firmware for beacon information */
                    nicPmIndicateBssConnected(prAdapter, NETWORK_TYPE_AIS_INDEX);
                }
            }

#if CFG_SUPPORT_ADHOC
            if (EQUAL_SSID(prBssDesc->aucSSID,
                        prBssDesc->ucSSIDLen,
                        prConnSettings->aucSSID,
                        prConnSettings->ucSSIDLen) &&
                    (prBssDesc->eBSSType == BSS_TYPE_IBSS) &&
                    (prAisBssInfo->eCurrentOPMode == OP_MODE_IBSS)) {

                ibssProcessMatchedBeacon(prAdapter, prAisBssInfo, prBssDesc, prSwRfb->prHifRxHdr->ucRcpi);
            }
#endif /* CFG_SUPPORT_ADHOC */
        }

        rlmProcessBcn(prAdapter,
                prSwRfb,
                ((P_WLAN_BEACON_FRAME_T)(prSwRfb->pvHeader))->aucInfoElem,
                (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
                (UINT_16)(OFFSET_OF(WLAN_BEACON_FRAME_BODY_T, aucInfoElem[0])));

        //4 <3> Send SW_RFB_T to HIF when we perform SCAN for HOST
        if(prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE
                || prBssDesc->eBSSType == BSS_TYPE_IBSS) {
            /* for AIS, send to host */
            if (prConnSettings->fgIsScanReqIssued &&
                    rlmDomainIsLegalChannel(prAdapter, prBssDesc->eBand, prBssDesc->ucChannelNum) == TRUE) {
                ENUM_BAND_T eBand;
                UINT_8 ucChannel;
                BOOLEAN fgAddToScanResult;

                /* check ucChannelNum/eBand for adjacement channel filtering */
                if(cnmAisInfraChannelFixed(prAdapter, &eBand, &ucChannel) == TRUE &&
                        (eBand != prBssDesc->eBand || ucChannel != prBssDesc->ucChannelNum)
                #if defined (__WIFI_SLIM__)
                   || ((prBssDesc->fgIERSN || prBssDesc->fgIEWPA) && !prBssDesc->fgPSK && prBssDesc->eBSSType != BSS_TYPE_IBSS)
                #endif /* __WIFI_SLIM__ */
                   ) {
                    fgAddToScanResult = FALSE;
                }
                else {
                    fgAddToScanResult = TRUE;
                }

                if(fgAddToScanResult == TRUE) {
                    rStatus = scanAddScanResult(prAdapter, prBssDesc, prSwRfb);
                }
            #if defined (__WIFI_SLIM__)
            if ((prBssDesc->fgIERSN || prBssDesc->fgIEWPA) && !prBssDesc->fgPSK && prBssDesc->eBSSType != BSS_TYPE_IBSS) {
                    scanRemoveBssDescByBssid(prAdapter, prBssDesc->aucBSSID);
                }
            #endif /* __WIFI_SLIM__ */
            }
        }

#if CFG_ENABLE_WIFI_DIRECT
        if(prAdapter->fgIsP2PRegistered) {
            ASSERT(prAdapter->rP2pFuncLkr.prScanP2pProcessBeaconAndProbeResp);
            prAdapter->rP2pFuncLkr.prScanP2pProcessBeaconAndProbeResp(
                prAdapter,
                prSwRfb,
                &rStatus,
                prBssDesc,
                prWlanBeaconFrame);
        }
#endif
    }

    return rStatus;

} /* end of scanProcessBeaconAndProbeResp() */


/*----------------------------------------------------------------------------*/
/*!
* \brief Search the Candidate of BSS Descriptor for JOIN(Infrastructure) or
*        MERGE(AdHoc) according to current Connection Policy.
*
* \return   Pointer to BSS Descriptor, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_BSS_DESC_T
scanSearchBssDescByPolicy (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
    )
{
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_BSS_INFO_T prBssInfo;
    P_AIS_SPECIFIC_BSS_INFO_T prAisSpecBssInfo;
    P_SCAN_INFO_T prScanInfo;

    P_LINK_T prBSSDescList;

    P_BSS_DESC_T prBssDesc = (P_BSS_DESC_T)NULL;
    P_BSS_DESC_T prPrimaryBssDesc = (P_BSS_DESC_T)NULL;
    P_BSS_DESC_T prCandidateBssDesc = (P_BSS_DESC_T)NULL;

    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    //P_STA_RECORD_T prPrimaryStaRec;
    //P_STA_RECORD_T prCandidateStaRec = (P_STA_RECORD_T)NULL;

    /* The first one reach the check point will be our candidate */
    BOOLEAN fgIsFindFirst = (BOOLEAN)FALSE;

    BOOLEAN fgIsFindBestRSSI = (BOOLEAN)FALSE;
    //BOOLEAN fgIsFindBestEncryptionLevel = (BOOLEAN)FALSE;
    //BOOLEAN fgIsFindMinChannelLoad = (BOOLEAN)FALSE;

    /* TODO(Kevin): Support Min Channel Load */
    //UINT_8 aucChannelLoad[CHANNEL_NUM] = {0};

    BOOLEAN fgIsFixedChannel;
    ENUM_BAND_T eBand;
    UINT_8 ucChannel;

    ASSERT(prAdapter);

    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[eNetTypeIndex]);

    prAisSpecBssInfo = &(prAdapter->rWifiVar.rAisSpecificBssInfo);

    prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
    prBSSDescList = &prScanInfo->rBSSDescList;

    /* check for fixed channel operation */
    if(eNetTypeIndex == NETWORK_TYPE_AIS_INDEX) {
        fgIsFixedChannel = cnmAisInfraChannelFixed(prAdapter, &eBand, &ucChannel);
    }
    else {
        fgIsFixedChannel = FALSE;
    }

#if DBG
    if (prConnSettings->ucSSIDLen < ELEM_MAX_LEN_SSID) {
        prConnSettings->aucSSID[prConnSettings->ucSSIDLen] = '\0';
    }
#endif

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Num Of BSS_DESC_T = %d, Look for SSID: %s By Policy =%d,prConnSettings->eConnectionPolicy\n",
        prBSSDescList->u4NumElem, prConnSettings->aucSSID);

    //4 <1> The outer loop to search for a candidate.
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {

        /* TODO(Kevin): Update Minimum Channel Load Information here */

        MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: ["MACSTR"], SSID:%s\n",
                            MAC2STR(prBssDesc->aucBSSID), prBssDesc->aucSSID);


        //4 <2> Check PHY Type and attributes
        //4 <2.1> Check Unsupported BSS PHY Type
        if (!(prBssDesc->ucPhyTypeSet & (prAdapter->rWifiVar.ucAvailablePhyTypeSet))) {

            MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore unsupported ucPhyTypeSet = %x\n",
                               prBssDesc->ucPhyTypeSet);
            continue;
        }

        //4 <2.2> Check if has unknown NonHT BSS Basic Rate Set.
        if (prBssDesc->fgIsUnknownBssBasicRate) {
            MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore UnknownBssBasicRate\n");
            continue;
        }

        //4 <2.3> Check if fixed operation cases should be aware
        if (fgIsFixedChannel == TRUE &&
                (prBssDesc->eBand != eBand || prBssDesc->ucChannelNum != ucChannel)) {
                
			MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore fixed operation case\n");
            continue;
        }

        //4 <2.4> Check if the channel is legal under regulatory domain
        if(rlmDomainIsLegalChannel(prAdapter, prBssDesc->eBand, prBssDesc->ucChannelNum) == FALSE) {
			MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore unlegal channel case\n");
            continue;
        }

        //4 <2.5> Check CMCCTD to MAUI: CMCC 7.6.2 Only wifi signal which > -85 dbm will be found
        if (prBssDesc->ucRCPI < dBm_TO_RCPI(prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_LOWEST_RSSI])) {
            MT5931_TRACE(TRACE_SEARCH, MT5931_INFO_145, "scanSearchBssDescByPolicy: ignore BSSID["MACSTR"] %d dBm < %d dBm",
                MAC2STR(prBssDesc->aucBSSID), RCPI_TO_dBm(prBssDesc->ucRCPI), 
                prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_LOWEST_RSSI]);
            continue;
        }

		if (EQUAL_MAC_ADDR(prBssDesc->aucSrcAddr, g_ignore_un_exist_ap)) {
			memset(g_ignore_un_exist_ap, 0, MAC_ADDR_LEN);
			kal_wap_trace(MOD_WNDRV, TRACE_SEARCH, "ignore un-exist AP.");
			continue;
		}
        //4 <3> Check if reach the excessive join retry limit
        /* NOTE(Kevin): STA_RECORD_T is recorded by TA. */
        prStaRec = cnmGetStaRecByAddress(prAdapter, (UINT_8) eNetTypeIndex,
                                            prBssDesc->aucSrcAddr);

        if (prStaRec) {
            /* NOTE(Kevin):
             * The Status Code is the result of a Previous Connection Request, we use this as SCORE for choosing a proper
             * candidate (Also used for compare see <6>)
             * The Reason Code is an indication of the reason why AP reject us, we use this Code for "Reject"
             * a SCAN result to become our candidate(Like a blacklist).
             */
#if 0 /* TODO(Kevin): */
            if (prStaRec->u2ReasonCode != REASON_CODE_RESERVED) {
                DBGLOG(SCN, INFO, ("SEARCH: Ignore BSS with previous Reason Code = %d\n",
                    prStaRec->u2ReasonCode));
                continue;
            }
            else
#endif
            if (prStaRec->u2StatusCode != STATUS_CODE_SUCCESSFUL) {
                OS_SYSTIME rCurrentTime;


                GET_CURRENT_SYSTIME(&rCurrentTime);

                /* NOTE(Kevin): greedy association - after timeout, we'll still
                 * try to associate to the AP whose STATUS of conection attempt
                 * was not success.
                 * We may also use (ucJoinFailureCount x JOIN_RETRY_INTERVAL_SEC) for
                 * time bound.
                 */
                if ((prStaRec->ucJoinFailureCount < JOIN_MAX_RETRY_FAILURE_COUNT) ||
                    (CHECK_FOR_TIMEOUT(rCurrentTime,
                                       prStaRec->rLastJoinTime,
                                       SEC_TO_SYSTIME(JOIN_RETRY_INTERVAL_SEC)))) {

                    /* NOTE(Kevin): Every JOIN_RETRY_INTERVAL_SEC interval, we can retry
                     * JOIN_MAX_RETRY_FAILURE_COUNT times.
                     */
                    if (prStaRec->ucJoinFailureCount >= JOIN_MAX_RETRY_FAILURE_COUNT) {
                        prStaRec->ucJoinFailureCount = 0;
                    }
                    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Try to join BSS again which has Status Code = %d (Curr = %ld/Last Join = %ld)\n",
                        prStaRec->u2StatusCode, rCurrentTime, prStaRec->rLastJoinTime);
                }
                else {
                    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore BSS which reach maximum Join Retry Count = %d \n",
                        JOIN_MAX_RETRY_FAILURE_COUNT);
                    continue;
                }

            }
        }


        //4 <4> Check for various NETWORK conditions
        if (eNetTypeIndex == NETWORK_TYPE_AIS_INDEX) {

            //4 <4.1> Check BSS Type for the corresponding Operation Mode in Connection Setting
            /* NOTE(Kevin): For NET_TYPE_AUTO_SWITCH, we will always pass following check. */
            if (((prConnSettings->eOPMode == NET_TYPE_INFRA) &&
                 (prBssDesc->eBSSType != BSS_TYPE_INFRASTRUCTURE)) ||
                ((prConnSettings->eOPMode == NET_TYPE_IBSS || prConnSettings->eOPMode == NET_TYPE_DEDICATED_IBSS) &&
                 (prBssDesc->eBSSType != BSS_TYPE_IBSS))) {

                MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore eBSSType = %s\n",
                    ((prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE) ?
                        "INFRASTRUCTURE" : "IBSS"));
                continue;
            }

            //4 <4.2> Check AP's BSSID if OID_802_11_BSSID has been set.
            if ((prConnSettings->fgIsConnByBssidIssued) &&
                (prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE)) {

                if (UNEQUAL_MAC_ADDR(prConnSettings->aucBSSID, prBssDesc->aucBSSID)) {

                    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore due to BSSID was not matched!\n");
                    continue;
                }
            }

#if CFG_SUPPORT_ADHOC
            //4 <4.3> Check for AdHoc Mode
            if (prBssDesc->eBSSType == BSS_TYPE_IBSS) {
                OS_SYSTIME rCurrentTime;

                //4 <4.3.1> Check if this SCAN record has been updated recently for IBSS.
                /* NOTE(Kevin): Because some STA may change its BSSID frequently after it
                 * create the IBSS - e.g. IPN2220, so we need to make sure we get the new one.
                 * For BSS, if the old record was matched, however it won't be able to pass
                 * the Join Process later.
                 */
                GET_CURRENT_SYSTIME(&rCurrentTime);
                if (CHECK_FOR_TIMEOUT(rCurrentTime, prBssDesc->rUpdateTime,
                                      SEC_TO_SYSTIME(SCN_ADHOC_BSS_DESC_TIMEOUT_SEC))) {
                    DBGLOG(SCN, LOUD, ("SEARCH: Skip old record of BSS Descriptor - BSSID:["MACSTR"]\n\n",
                              MAC2STR(prBssDesc->aucBSSID)));
                    continue;
                }

                //4 <4.3.2> Check Peer's capability
                if (ibssCheckCapabilityForAdHocMode(prAdapter, prBssDesc) == WLAN_STATUS_FAILURE) {

                    DBGLOG(SCN, INFO,
                        ("SEARCH: Ignore BSS DESC MAC: "MACSTR", Capability is not supported for current AdHoc Mode.\n",
                        MAC2STR(prPrimaryBssDesc->aucBSSID)));

                    continue;
                }


                //4 <4.3.3> Compare TSF
                if (prBssInfo->fgIsBeaconActivated &&
                    UNEQUAL_MAC_ADDR(prBssInfo->aucBSSID, prBssDesc->aucBSSID)) {

                    DBGLOG(SCN, LOUD,
                        ("SEARCH: prBssDesc->fgIsLargerTSF = %d\n",
                            prBssDesc->fgIsLargerTSF));

                    if (!prBssDesc->fgIsLargerTSF) {
                        DBGLOG(SCN, INFO,
                            ("SEARCH: Ignore BSS DESC MAC: ["MACSTR"], Smaller TSF\n", MAC2STR(prBssDesc->aucBSSID)));
                        continue;
                    }
                }
            }
#endif /* CFG_SUPPORT_ADHOC */

        }




#if 0 /* TODO(Kevin): For IBSS */
        //4 <2.c> Check if this SCAN record has been updated recently for IBSS.
        /* NOTE(Kevin): Because some STA may change its BSSID frequently after it
         * create the IBSS, so we need to make sure we get the new one.
         * For BSS, if the old record was matched, however it won't be able to pass
         * the Join Process later.
         */
        if (prBssDesc->eBSSType == BSS_TYPE_IBSS) {
            OS_SYSTIME rCurrentTime;

            GET_CURRENT_SYSTIME(&rCurrentTime);
            if (CHECK_FOR_TIMEOUT(rCurrentTime, prBssDesc->rUpdateTime,
                                  SEC_TO_SYSTIME(BSS_DESC_TIMEOUT_SEC))) {
                DBGLOG(SCAN, TRACE, ("Skip old record of BSS Descriptor - BSSID:["MACSTR"]\n\n",
                          MAC2STR(prBssDesc->aucBSSID)));
                continue;
            }
        }

        if ((prBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE) &&
            (prAdapter->eConnectionState == MEDIA_STATE_CONNECTED)) {
            OS_SYSTIME rCurrentTime;

            GET_CURRENT_SYSTIME(&rCurrentTime);
            if (CHECK_FOR_TIMEOUT(rCurrentTime, prBssDesc->rUpdateTime,
                                  SEC_TO_SYSTIME(BSS_DESC_TIMEOUT_SEC))) {
                DBGLOG(SCAN, TRACE, ("Skip old record of BSS Descriptor - BSSID:["MACSTR"]\n\n",
                          MAC2STR(prBssDesc->aucBSSID)));
                continue;
            }
        }


        //4 <4B> Check for IBSS AdHoc Mode.
        /* Skip if one or more BSS Basic Rate are not supported by current AdHocMode */
        if (prPrimaryBssDesc->eBSSType == BSS_TYPE_IBSS) {
            //4 <4B.1> Check if match the Capability of current IBSS AdHoc Mode.
            if (ibssCheckCapabilityForAdHocMode(prAdapter, prPrimaryBssDesc) == WLAN_STATUS_FAILURE) {

                DBGLOG(SCAN, TRACE,
                    ("Ignore BSS DESC MAC: "MACSTR", Capability is not supported for current AdHoc Mode.\n",
                    MAC2STR(prPrimaryBssDesc->aucBSSID)));

                continue;
            }


            //4 <4B.2> IBSS Merge Decision Flow for SEARCH STATE.
            if (prAdapter->fgIsIBSSActive &&
                UNEQUAL_MAC_ADDR(prBssInfo->aucBSSID, prPrimaryBssDesc->aucBSSID)) {

                if (!fgIsLocalTSFRead) {
                    NIC_GET_CURRENT_TSF(prAdapter, &rCurrentTsf);

                    DBGLOG(SCAN, TRACE,
                        ("\n\nCurrent TSF : %08lx-%08lx\n\n",
                            rCurrentTsf.u.HighPart, rCurrentTsf.u.LowPart));
                }

                if (rCurrentTsf.QuadPart > prPrimaryBssDesc->u8TimeStamp.QuadPart) {
                    DBGLOG(SCAN, TRACE,
                        ("Ignore BSS DESC MAC: ["MACSTR"], Current BSSID: ["MACSTR"].\n",
                            MAC2STR(prPrimaryBssDesc->aucBSSID), MAC2STR(prBssInfo->aucBSSID)));

                    DBGLOG(SCAN, TRACE,
                        ("\n\nBSS's TSF : %08lx-%08lx\n\n",
                            prPrimaryBssDesc->u8TimeStamp.u.HighPart, prPrimaryBssDesc->u8TimeStamp.u.LowPart));

                    prPrimaryBssDesc->fgIsLargerTSF = FALSE;
                    continue;
                }
                else {
                    prPrimaryBssDesc->fgIsLargerTSF = TRUE;
                }

            }
        }

        //4 <5> Check the Encryption Status.
        if (rsnPerformPolicySelection(prPrimaryBssDesc)) {

            if (prPrimaryBssDesc->ucEncLevel > 0) {
                fgIsFindBestEncryptionLevel = TRUE;

                fgIsFindFirst = FALSE;
            }
        }
        else {
            /* Can't pass the Encryption Status Check, get next one */
            continue;
        }

        /* For RSN Pre-authentication, update the PMKID canidate list for
           same SSID and encrypt status */
        /* Update PMKID candicate list. */
        if (prAdapter->rWifiVar.rConnSettings.eAuthMode == AUTH_MODE_WPA2) {
            rsnUpdatePmkidCandidateList(prPrimaryBssDesc);
            if (prAdapter->rWifiVar.rAisBssInfo.u4PmkidCandicateCount) {
                prAdapter->rWifiVar.rAisBssInfo.fgIndicatePMKID = rsnCheckPmkidCandicate();
            }
        }

#endif


        prPrimaryBssDesc = (P_BSS_DESC_T)NULL;

        //4 <6> Check current Connection Policy.
        switch (prConnSettings->eConnectionPolicy) {
        case CONNECT_BY_SSID_BEST_RSSI:
            /* Choose Hidden SSID to join only if the `fgIsEnableJoin...` is TRUE */
            if (prAdapter->rWifiVar.fgEnableJoinToHiddenSSID && prBssDesc->fgIsHiddenSSID) {
                /* NOTE(Kevin): following if () statement means that
                 * If Target is hidden, then we won't connect when user specify SSID_ANY policy.
                 */
                if (prConnSettings->ucSSIDLen) {
                    prPrimaryBssDesc = prBssDesc;

                    fgIsFindBestRSSI = TRUE;
                }

            }
            else if (EQUAL_SSID(prBssDesc->aucSSID,
                                prBssDesc->ucSSIDLen,
                                prConnSettings->aucSSID,
                                prConnSettings->ucSSIDLen)) {
                prPrimaryBssDesc = prBssDesc;

                fgIsFindBestRSSI = TRUE;
            }
            break;

        case CONNECT_BY_SSID_ANY:
            /* NOTE(Kevin): In this policy, we don't know the desired
             * SSID from user, so we should exclude the Hidden SSID from scan list.
             * And because we refuse to connect to Hidden SSID node at the beginning, so
             * when the JOIN Module deal with a BSS_DESC_T which has fgIsHiddenSSID == TRUE,
             * then the Connection Settings must be valid without doubt.
             */
            if (!prBssDesc->fgIsHiddenSSID) {
                prPrimaryBssDesc = prBssDesc;

                fgIsFindFirst = TRUE;
            }
            break;

        case CONNECT_BY_BSSID:
            if(EQUAL_MAC_ADDR(prBssDesc->aucBSSID, prConnSettings->aucBSSID)) {
                prPrimaryBssDesc = prBssDesc;
            }
            break;

        default:
            break;
        }


        /* Primary Candidate was not found */
        if (prPrimaryBssDesc == NULL) {
        
       		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH:Ignore,prPrimaryBssDesc is Null");
            continue;
        }

        //4 <7> Check the Encryption Status.
        if (prPrimaryBssDesc->eBSSType == BSS_TYPE_INFRASTRUCTURE) {
#ifdef __WAPI_SUPPORT__
            if (prAdapter->rWifiVar.rConnSettings.fgWapiMode) {
                if (wapiPerformPolicySelection(prAdapter, prPrimaryBssDesc)) {
                    fgIsFindFirst = TRUE;
                }
                else {
                    /* Can't pass the Encryption Status Check, get next one */
                    continue;
                }
            }
            else
#endif
#if CFG_RSN_MIGRATION
            if (rsnPerformPolicySelection(prAdapter, prPrimaryBssDesc)) {
                if (prAisSpecBssInfo->fgCounterMeasure) {
                    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore,while at counter measure period!!!\n");
                    continue;
                }

                if (prPrimaryBssDesc->ucEncLevel > 0) {
                    //fgIsFindBestEncryptionLevel = TRUE;

                    fgIsFindFirst = FALSE;
                }

#if 0
                /* Update PMKID candicate list. */
                if (prAdapter->rWifiVar.rConnSettings.eAuthMode == AUTH_MODE_WPA2) {
                    rsnUpdatePmkidCandidateList(prPrimaryBssDesc);
                    if (prAisSpecBssInfo->u4PmkidCandicateCount) {
                        if (rsnCheckPmkidCandicate()) {
                            DBGLOG(RSN, WARN, ("Prepare a timer to indicate candidate "MACSTR"\n",
                                MAC2STR(prAisSpecBssInfo->arPmkidCache[prAisSpecBssInfo->u4PmkidCacheCount].rBssidInfo.aucBssid)));
                            cnmTimerStopTimer(&prAisSpecBssInfo->rPreauthenticationTimer);
                            cnmTimerStartTimer(&prAisSpecBssInfo->rPreauthenticationTimer,
                                    SEC_TO_MSEC(WAIT_TIME_IND_PMKID_CANDICATE_SEC));
                        }
                    }
                }
#endif
            }
            else {
                /* Can't pass the Encryption Status Check, get next one */
				MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Ignore, Wrong Encryption Statu\n");
                continue;
            }
#endif
        }
        else {
            /* Todo:: P2P and BOW Policy Selection */
        }

        //prPrimaryStaRec = prStaRec;

        //4 <8> Compare the Candidate and the Primary Scan Record.
        if (!prCandidateBssDesc) {
            prCandidateBssDesc = prPrimaryBssDesc;
            //prCandidateStaRec = prPrimaryStaRec;

            //4 <8.1> Condition - Get the first matched one.
            if (fgIsFindFirst) {
                break;
            }
        }
        else {
#if 0 /* TODO(Kevin): For security(TBD) */
            //4 <6B> Condition - Choose the one with best Encryption Score.
            if (fgIsFindBestEncryptionLevel) {
                if (prCandidateBssDesc->ucEncLevel <
                    prPrimaryBssDesc->ucEncLevel) {

                    prCandidateBssDesc = prPrimaryBssDesc;
                    prCandidateStaRec = prPrimaryStaRec;
                    continue;
                }
            }

            /* If reach here, that means they have the same Encryption Score.
             */

            //4 <6C> Condition - Give opportunity to the one we didn't connect before.
            // For roaming, only compare the candidates other than current associated BSSID.
            if (!prCandidateBssDesc->fgIsConnected && !prPrimaryBssDesc->fgIsConnected) {
                if ((prCandidateStaRec != (P_STA_RECORD_T)NULL) &&
                    (prCandidateStaRec->u2StatusCode != STATUS_CODE_SUCCESSFUL)) {

                    DBGLOG(SCAN, TRACE, ("So far -BSS DESC MAC: "MACSTR" has nonzero Status Code = %d\n",
                        MAC2STR(prCandidateBssDesc->aucBSSID), prCandidateStaRec->u2StatusCode));

                    if (prPrimaryStaRec != (P_STA_RECORD_T)NULL) {
                        if (prPrimaryStaRec->u2StatusCode != STATUS_CODE_SUCCESSFUL) {

                            /* Give opportunity to the one with smaller rLastJoinTime */
                            if (TIME_BEFORE(prCandidateStaRec->rLastJoinTime,
                                                        prPrimaryStaRec->rLastJoinTime)) {
                                continue;
                            }
                            /* We've connect to CANDIDATE recently, let us try PRIMARY now */
                            else {
                                prCandidateBssDesc = prPrimaryBssDesc;
                                prCandidateStaRec = prPrimaryStaRec;
                                continue;
                            }
                        }
                        /* PRIMARY's u2StatusCode = 0 */
                        else {
                            prCandidateBssDesc = prPrimaryBssDesc;
                            prCandidateStaRec = prPrimaryStaRec;
                            continue;
                        }
                    }
                    /* PRIMARY has no StaRec - We didn't connet to PRIMARY before */
                    else {
                        prCandidateBssDesc = prPrimaryBssDesc;
                        prCandidateStaRec = prPrimaryStaRec;
                        continue;
                    }
                }
                else {
                    if ((prPrimaryStaRec != (P_STA_RECORD_T)NULL) &&
                        (prPrimaryStaRec->u2StatusCode != STATUS_CODE_SUCCESSFUL)) {
                        continue;
                    }
                }
            }
#endif


            //4 <6D> Condition - Visible SSID win Hidden SSID.
            if (prCandidateBssDesc->fgIsHiddenSSID) {
                if (!prPrimaryBssDesc->fgIsHiddenSSID) {
                    prCandidateBssDesc = prPrimaryBssDesc; /* The non Hidden SSID win. */
                    //prCandidateStaRec = prPrimaryStaRec;
                    continue;
                }
            }
            else {
                if (prPrimaryBssDesc->fgIsHiddenSSID) {
                    continue;
                }
            }


            //4 <6E> Condition - Choose the one with better RCPI(RSSI).
            if (fgIsFindBestRSSI) {
                /* TODO(Kevin): We shouldn't compare the actual value, we should
                 * allow some acceptable tolerance of some RSSI percentage here.
                 */
                MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"Candidate ["MACSTR"]: RCPI = %d, Primary ["MACSTR"]: RCPI = %d\n",
                    MAC2STR(prCandidateBssDesc->aucBSSID), prCandidateBssDesc->ucRCPI,
                    MAC2STR(prPrimaryBssDesc->aucBSSID), prPrimaryBssDesc->ucRCPI);
                 

                ASSERT(!(prCandidateBssDesc->fgIsConnected &&
                         prPrimaryBssDesc->fgIsConnected));

                /* NOTE: To prevent SWING, we do roaming only if target AP has at least 5dBm larger than us. */
                if (prCandidateBssDesc->fgIsConnected) {
                    if (prCandidateBssDesc->ucRCPI + ROAMING_NO_SWING_RCPI_STEP <= prPrimaryBssDesc->ucRCPI) {

                        prCandidateBssDesc = prPrimaryBssDesc;
                        //prCandidateStaRec = prPrimaryStaRec;
                        continue;
                    }
                }
                else if (prPrimaryBssDesc->fgIsConnected) {
                    if (prCandidateBssDesc->ucRCPI < prPrimaryBssDesc->ucRCPI + ROAMING_NO_SWING_RCPI_STEP) {

                        prCandidateBssDesc = prPrimaryBssDesc;
                        //prCandidateStaRec = prPrimaryStaRec;
                        continue;
                    }
                }
                else if (prCandidateBssDesc->ucRCPI < prPrimaryBssDesc->ucRCPI) {
                    prCandidateBssDesc = prPrimaryBssDesc;
                    //prCandidateStaRec = prPrimaryStaRec;
                    continue;
                }
                MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: After check RSSI,Candidate is ["MACSTR"]\n",
                MAC2STR(prCandidateBssDesc->aucBSSID));
            }

#if 0
            /* If reach here, that means they have the same Encryption Score, and
             * both RSSI value are close too.
             */
            //4 <6F> Seek the minimum Channel Load for less interference.
            if (fgIsFindMinChannelLoad) {

                /* TODO(Kevin): Check which one has minimum channel load in its channel */
            }
#endif
        }
    }
    
    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_SEARCH,"SEARCH: Done,SSID[%s],BSSID["MACSTR"]\n",
    prCandidateBssDesc->aucSSID, MAC2STR(prCandidateBssDesc->aucBSSID));
    return prCandidateBssDesc;

} /* end of scanSearchBssDescByPolicy() */


