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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/nic/nic_cmd_event.c#23 $
*/

/*! \file   nic_cmd_event.c
    \brief  Callback functions for Command packets.

        Various Event packet handlers which will be setup in the callback function of
    a command packet.
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
extern VOID kalFlushPendingTxPackets(IN P_GLUE_INFO_T    prGlueInfo);

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

VOID
nicCmdEventQueryMcrRead(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_MCR_RW_STRUC_T prMcrRdInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_CMD_ACCESS_REG prCmdAccessReg;


    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //4 <2> Update information of OID
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prCmdAccessReg = (P_CMD_ACCESS_REG)(pucEventBuf);

        u4QueryInfoLen = sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T);

        prMcrRdInfo = (P_PARAM_CUSTOM_MCR_RW_STRUC_T) prCmdInfo->pvInformationBuffer;
        prMcrRdInfo->u4McrOffset = prCmdAccessReg->u4Address;
        prMcrRdInfo->u4McrData = prCmdAccessReg->u4Data;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;

}


VOID
nicCmdEventQuerySwCtrlRead(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_SW_CTRL_STRUC_T prSwCtrlInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_CMD_SW_DBG_CTRL_T prCmdSwCtrl;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //4 <2> Update information of OID
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prCmdSwCtrl = (P_CMD_SW_DBG_CTRL_T)(pucEventBuf);

        u4QueryInfoLen = sizeof(PARAM_CUSTOM_SW_CTRL_STRUC_T);

        prSwCtrlInfo = (P_PARAM_CUSTOM_SW_CTRL_STRUC_T) prCmdInfo->pvInformationBuffer;
        prSwCtrlInfo->u4Id = prCmdSwCtrl->u4Id;
        prSwCtrlInfo->u4Data = prCmdSwCtrl->u4Data;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;

}



VOID
nicCmdEventSetCommon(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    if (prCmdInfo->fgIsOid) {
        /* Update Set Infomation Length */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4InformationBufferLength,
                       WLAN_STATUS_SUCCESS);
    }

    return;
}

VOID
nicCmdEventSetDisassociate(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    if (prCmdInfo->fgIsOid) {
        /* Update Set Infomation Length */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       0,
                       WLAN_STATUS_SUCCESS);
    }

    kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                 WLAN_STATUS_MEDIA_DISCONNECT,
                                 NULL,
                                 0);

#if !defined(LINUX) && !defined(WCN_MAUI)
    prAdapter->fgIsRadioOff = TRUE;
#endif

    return;
}

VOID
nicCmdEventSetIpAddress(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4Count;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    u4Count = (prCmdInfo->u4SetInfoLen - OFFSET_OF(CMD_SET_NETWORK_ADDRESS_LIST, arNetAddress))
              / sizeof(IPV4_NETWORK_ADDRESS) ;

    if (prCmdInfo->fgIsOid) {
        /* Update Set Infomation Length */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       OFFSET_OF(PARAM_NETWORK_ADDRESS_LIST, arAddress) + u4Count *
                       (OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress) + sizeof(PARAM_NETWORK_ADDRESS_IP)),
                       WLAN_STATUS_SUCCESS);
    }

    return;
}

VOID
nicCmdEventQueryRfTestATInfo(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_TEST_STATUS prTestStatus, prQueryBuffer;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prTestStatus = (P_EVENT_TEST_STATUS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prQueryBuffer = (P_EVENT_TEST_STATUS) prCmdInfo->pvInformationBuffer;

        kalMemCopy(prQueryBuffer, prTestStatus, sizeof(EVENT_TEST_STATUS));

        u4QueryInfoLen = sizeof(EVENT_TEST_STATUS);

        /* Update Query Infomation Length */
        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

    return;
}

VOID
nicCmdEventQueryLinkQuality(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    PARAM_RSSI rRssi, *prRssi;
    P_EVENT_LINK_QUALITY prLinkQuality;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prLinkQuality = (P_EVENT_LINK_QUALITY)pucEventBuf;

    rRssi = (PARAM_RSSI)prLinkQuality->cRssi; // ranged from (-128 ~ 30) in unit of dBm

    if (rRssi > PARAM_WHQL_RSSI_MAX_DBM) {
        rRssi = PARAM_WHQL_RSSI_MAX_DBM;
    } else if (rRssi < PARAM_WHQL_RSSI_MIN_DBM) {
        rRssi = PARAM_WHQL_RSSI_MIN_DBM;
    }

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prRssi = (PARAM_RSSI *) prCmdInfo->pvInformationBuffer;

        kalMemCopy(prRssi, &rRssi, sizeof(PARAM_RSSI));
        u4QueryInfoLen = sizeof(PARAM_RSSI);

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This routine is in response of OID_GEN_LINK_SPEED query request
*
* @param prAdapter      Pointer to the Adapter structure.
* @param prCmdInfo      Pointer to the pending command info
* @param pucEventBuf
*
* @retval none
*/
/*----------------------------------------------------------------------------*/
VOID
nicCmdEventQueryLinkSpeed(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_LINK_QUALITY prLinkQuality;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4LinkSpeed;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prLinkQuality = (P_EVENT_LINK_QUALITY)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        pu4LinkSpeed = (PUINT_32)(prCmdInfo->pvInformationBuffer);

        if (prLinkQuality->u2LinkSpeed == 0) {
            *pu4LinkSpeed = 10000; /* 10K * 100bps = 1Mbps */
        } else {
            *pu4LinkSpeed = prLinkQuality->u2LinkSpeed * 5000;
        }

        u4QueryInfoLen = sizeof(UINT_32);

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryStatistics(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_PARAM_802_11_STATISTICS_STRUCT_T prStatistics;
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        u4QueryInfoLen = sizeof(PARAM_802_11_STATISTICS_STRUCT_T);
        prStatistics = (P_PARAM_802_11_STATISTICS_STRUCT_T) prCmdInfo->pvInformationBuffer;

        prStatistics->u4Length = sizeof(PARAM_802_11_STATISTICS_STRUCT_T);
        prStatistics->rTransmittedFragmentCount
            = prEventStatistics->rTransmittedFragmentCount;
        prStatistics->rMulticastTransmittedFrameCount
            = prEventStatistics->rMulticastTransmittedFrameCount;
        prStatistics->rFailedCount
            = prEventStatistics->rFailedCount;
        prStatistics->rRetryCount
            = prEventStatistics->rRetryCount;
        prStatistics->rMultipleRetryCount
            = prEventStatistics->rMultipleRetryCount;
        prStatistics->rRTSSuccessCount
            = prEventStatistics->rRTSSuccessCount;
        prStatistics->rRTSFailureCount
            = prEventStatistics->rRTSFailureCount;
        prStatistics->rACKFailureCount
            = prEventStatistics->rACKFailureCount;
        prStatistics->rFrameDuplicateCount
            = prEventStatistics->rFrameDuplicateCount;
        prStatistics->rReceivedFragmentCount
            = prEventStatistics->rReceivedFragmentCount;
        prStatistics->rMulticastReceivedFrameCount
            = prEventStatistics->rMulticastReceivedFrameCount;
        prStatistics->rFCSErrorCount
            = prEventStatistics->rFCSErrorCount;
        prStatistics->rTKIPLocalMICFailures.QuadPart
            = 0;
        prStatistics->rTKIPICVErrors.QuadPart
            = 0;
        prStatistics->rTKIPCounterMeasuresInvoked.QuadPart
            = 0;
        prStatistics->rTKIPReplays.QuadPart
            = 0;
        prStatistics->rCCMPFormatErrors.QuadPart
            = 0;
        prStatistics->rCCMPReplays.QuadPart
            = 0;
        prStatistics->rCCMPDecryptErrors.QuadPart
            = 0;
        prStatistics->rFourWayHandshakeFailures.QuadPart
            = 0;
        prStatistics->rWEPUndecryptableCount.QuadPart
            = 0;
        prStatistics->rWEPICVErrorCount.QuadPart
            = 0;
        prStatistics->rDecryptSuccessCount.QuadPart
            = 0;
        prStatistics->rDecryptFailureCount.QuadPart
            = 0;

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}

VOID
nicCmdEventEnterRfTest(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4WHISR = 0, u4Value = 0;
    UINT_8 aucTxCount[8];

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    // [driver-land]
    prAdapter->fgTestMode = TRUE;

    // 0. always indicate disconnection
    if (kalGetMediaStateIndicated(prAdapter->prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED) {
        kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                     WLAN_STATUS_MEDIA_DISCONNECT,
                                     NULL,
                                     0);
    }

    // 1. Remove pending TX
    nicTxRelease(prAdapter);

    // 1.1 clear pending Security / Management Frames
    kalClearSecurityFrames(prAdapter->prGlueInfo);
    kalClearMgmtFrames(prAdapter->prGlueInfo);

    // 1.2 clear pending TX packet queued in glue layer
    kalFlushPendingTxPackets(prAdapter->prGlueInfo);

    // 2. Reset driver-domain FSMs
    nicUninitMGMT(prAdapter);

    nicResetSystemService(prAdapter);
    nicInitMGMT(prAdapter, NULL);

    // 3. Disable Interrupt
    HAL_INTR_DISABLE(prAdapter);

    // 4. Block til firmware completed entering into RF test mode
    kalMsleep(500);
    while (1) {
        HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

        if (u4Value & WCIR_WLAN_READY) {
            break;
        } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                   || fgIsBusAccessFailed == TRUE) {
            if (prCmdInfo->fgIsOid) {
                /* Update Set Infomation Length */
                kalOidComplete(prAdapter->prGlueInfo,
                               prCmdInfo->fgSetQuery,
                               prCmdInfo->u4SetInfoLen,
                               WLAN_STATUS_NOT_SUPPORTED);

            }
            return;
        } else {
            kalMsleep(10);
        }
    }

    // 5. Clear Interrupt Status
    HAL_READ_INTR_STATUS(prAdapter, 4, &u4WHISR);
    if (HAL_IS_TX_DONE_INTR(u4WHISR)) {
        HAL_READ_TX_RELEASED_COUNT(prAdapter, aucTxCount);
    }

    // 6. Reset TX Counter
    nicTxResetResource(prAdapter);

#if 0   /* NOTE(Nelson): Because it's polling mode in META mode, so shouldn't enable interrupt */
    // 7. Re-enable Interrupt
    HAL_INTR_ENABLE(prAdapter);
#endif

    // 8. completion indication
    if (prCmdInfo->fgIsOid) {
        /* Update Set Infomation Length */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4SetInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

#if CFG_SUPPORT_NVRAM
    // 9. load manufacture data
    wlanLoadManufactureData(prAdapter, kalGetConfiguration(prAdapter->prGlueInfo));
#endif

    return;
}

VOID
nicCmdEventLeaveRfTest(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4WHISR = 0, u4Value = 0;
    UINT_8 aucTxCount[8];

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    // 1. Disable Interrupt
    HAL_INTR_DISABLE(prAdapter);

    // 2. Block til firmware completed leaving from RF test mode
    kalMsleep(500);
    while (1) {
        HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

        if (u4Value & WCIR_WLAN_READY) {
            break;
        } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                   || fgIsBusAccessFailed == TRUE) {
            if (prCmdInfo->fgIsOid) {
                /* Update Set Infomation Length */
                kalOidComplete(prAdapter->prGlueInfo,
                               prCmdInfo->fgSetQuery,
                               prCmdInfo->u4SetInfoLen,
                               WLAN_STATUS_NOT_SUPPORTED);

            }
            return;
        } else {
            kalMsleep(10);
        }
    }

    // 3. Clear Interrupt Status
    HAL_READ_INTR_STATUS(prAdapter, 4, &u4WHISR);
    if (HAL_IS_TX_DONE_INTR(u4WHISR)) {
        HAL_READ_TX_RELEASED_COUNT(prAdapter, aucTxCount);
    }

    // 4. Reset TX Counter
    nicTxResetResource(prAdapter);

    // 5. Re-enable Interrupt
    HAL_INTR_ENABLE(prAdapter);

    // 6. set driver-land variable
    prAdapter->fgTestMode = FALSE;

    // 7. completion indication
    if (prCmdInfo->fgIsOid) {
        /* Update Set Infomation Length */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4SetInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

    /* 8. Indicate as disconnected */
    if (kalGetMediaStateIndicated(prAdapter->prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED) {

        kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                     WLAN_STATUS_MEDIA_DISCONNECT,
                                     NULL,
                                     0);

        prAdapter->rWlanInfo.u4SysTime = kalGetTimeTick();
    }

#if CFG_SUPPORT_NVRAM
    /* 9. load manufacture data */
    wlanLoadManufactureData(prAdapter, kalGetConfiguration(prAdapter->prGlueInfo));
#endif

    /* 10. Override network address */
    wlanUpdateNetworkAddress(prAdapter);

    return;
}

VOID
nicCmdEventQueryAddress(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4QueryInfoLen;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_BASIC_CONFIG prEventBasicConfig;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    prEventBasicConfig = (P_EVENT_BASIC_CONFIG)(pucEventBuf);

    // copy to adapter
    kalMemCopy(&(prAdapter->rMyMacAddr), &(prEventBasicConfig->rMyMacAddr), MAC_ADDR_LEN);

    //4 <2> Update information of OID
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        kalMemCopy(prCmdInfo->pvInformationBuffer, &(prEventBasicConfig->rMyMacAddr), MAC_ADDR_LEN);
        u4QueryInfoLen = MAC_ADDR_LEN;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    //4 <3> Update new MAC address and all 3 networks
    COPY_MAC_ADDR(prAdapter->rWifiVar.aucMacAddress, prAdapter->rMyMacAddr);
    COPY_MAC_ADDR(prAdapter->rWifiVar.aucDeviceAddress, prAdapter->rMyMacAddr);
    prAdapter->rWifiVar.aucDeviceAddress[0] ^= MAC_ADDR_LOCAL_ADMIN;

    COPY_MAC_ADDR(prAdapter->rWifiVar.aucInterfaceAddress, prAdapter->rMyMacAddr);

    COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].aucOwnMacAddr,
                  prAdapter->rMyMacAddr);

#if CFG_ENABLE_WIFI_TETHERING
    if (prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable) {
        COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr,
                      prAdapter->rWifiVar.aucDeviceAddress);
        MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_59, "prAdapter->rMyMacAddr: "MACSTR,
                     MAC2STR(prAdapter->rMyMacAddr));
        MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_60, "prAdapter->rWifiVar.aucInterfaceAddress: "MACSTR,
                     MAC2STR(prAdapter->rWifiVar.aucDeviceAddress));
        MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_61, "prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr: "MACSTR,
                     MAC2STR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr));
    }
#endif /* CFG_ENABLE_WIFI_TETHERING */

#if CFG_ENABLE_WIFI_DIRECT
    if (prAdapter->fgIsP2PRegistered) {
        COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr,
                      prAdapter->rWifiVar.aucDeviceAddress);
    }
#endif

#if CFG_ENABLE_BT_OVER_WIFI
    COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX].aucOwnMacAddr,
                  prAdapter->rMyMacAddr);
#endif

#if CFG_TEST_WIFI_DIRECT_GO
    if (prAdapter->rWifiVar.prP2pFsmInfo->eCurrentState == P2P_STATE_IDLE) {
        wlanEnableP2pFunction(prAdapter);

        wlanEnableATGO(prAdapter);
    }
#endif

    kalUpdateMACAddress(prAdapter->prGlueInfo, prAdapter->rWifiVar.aucMacAddress);

    return;
}

VOID
nicCmdEventQueryMcastAddr(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4QueryInfoLen;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_MAC_MCAST_ADDR prEventMacMcastAddr;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //4 <2> Update information of OID
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEventMacMcastAddr = (P_EVENT_MAC_MCAST_ADDR)(pucEventBuf);

        u4QueryInfoLen = prEventMacMcastAddr->u4NumOfGroupAddr * MAC_ADDR_LEN;

        // buffer length check
        if (prCmdInfo->u4InformationBufferLength < u4QueryInfoLen) {
            kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_BUFFER_TOO_SHORT);
        } else {
            kalMemCopy(prCmdInfo->pvInformationBuffer,
                       prEventMacMcastAddr->arAddress,
                       prEventMacMcastAddr->u4NumOfGroupAddr * MAC_ADDR_LEN);

            kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
        }
    }
}

VOID
nicCmdEventQueryEepromRead(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_EEPROM_RW_STRUC_T prEepromRdInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_ACCESS_EEPROM prEventAccessEeprom;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //4 <2> Update information of OID
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEventAccessEeprom = (P_EVENT_ACCESS_EEPROM)(pucEventBuf);

        u4QueryInfoLen = sizeof(PARAM_CUSTOM_EEPROM_RW_STRUC_T);

        prEepromRdInfo = (P_PARAM_CUSTOM_EEPROM_RW_STRUC_T) prCmdInfo->pvInformationBuffer;
        prEepromRdInfo->ucEepromIndex = (UINT_8)(prEventAccessEeprom->u2Offset);
        prEepromRdInfo->u2EepromData = prEventAccessEeprom->u2Data;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;

}


VOID
nicCmdEventSetMediaStreamMode(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    PARAM_MEDIA_STREAMING_INDICATION rParamMediaStreamIndication;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    if (prCmdInfo->fgIsOid) {
        /* Update Set Infomation Length */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4SetInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

    rParamMediaStreamIndication.rStatus.eStatusType =
        ENUM_STATUS_TYPE_MEDIA_STREAM_MODE;
    rParamMediaStreamIndication.eMediaStreamMode =
        prAdapter->rWlanInfo.eLinkAttr.ucMediaStreamMode == 0 ?
        ENUM_MEDIA_STREAM_OFF : ENUM_MEDIA_STREAM_ON;

    kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                 WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
                                 (PVOID)&rParamMediaStreamIndication,
                                 sizeof(PARAM_MEDIA_STREAMING_INDICATION));
}


/* Statistics responder */
VOID
nicCmdEventQueryXmitOk(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rTransmittedFragmentCount.QuadPart;
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rTransmittedFragmentCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvOk(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rReceivedFragmentCount.QuadPart;
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rReceivedFragmentCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}

VOID
nicCmdEventQueryXmitError(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFailedCount.QuadPart;
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) prEventStatistics->rFailedCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvError(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFCSErrorCount.QuadPart;
            // @FIXME, RX_ERROR_DROP_COUNT/RX_FIFO_FULL_DROP_COUNT is not calculated
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rFCSErrorCount.QuadPart;
            // @FIXME, RX_ERROR_DROP_COUNT/RX_FIFO_FULL_DROP_COUNT is not calculated
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvNoBuffer(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    //P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    //prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = 0; // @FIXME?
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = 0; //@FIXME?
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvCrcError(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFCSErrorCount.QuadPart;
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rFCSErrorCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvErrorAlignment(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    //P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    //prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) 0; //@FIXME
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = 0; //@FIXME
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryXmitOneCollision(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32)(prEventStatistics->rMultipleRetryCount.QuadPart - prEventStatistics->rRetryCount.QuadPart);
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64)(prEventStatistics->rMultipleRetryCount.QuadPart - prEventStatistics->rRetryCount.QuadPart);
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryXmitMoreCollisions(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rMultipleRetryCount.QuadPart;
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) prEventStatistics->rMultipleRetryCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryXmitMaxCollisions(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
)
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if (prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFailedCount.QuadPart;
        } else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) prEventStatistics->rFailedCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when command by OID/ioctl has been timeout
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
VOID
nicOidCmdTimeoutCommon(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
)
{
    ASSERT(prAdapter);

    kalOidComplete(prAdapter->prGlueInfo,
                   prCmdInfo->fgSetQuery,
                   0,
                   WLAN_STATUS_FAILURE);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is a generic command timeout handler
*
* @param pfnOidHandler      Pointer to the OID handler
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
nicCmdTimeoutCommon(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
)
{
    ASSERT(prAdapter);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when command for entering RF test has
*        failed sending due to timeout (highly possibly by firmware crash)
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicOidCmdEnterRFTestTimeout(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
)
{
    ASSERT(prAdapter);

    // 1. Remove pending TX frames
    nicTxRelease(prAdapter);

    // 1.1 clear pending Security / Management Frames
    kalClearSecurityFrames(prAdapter->prGlueInfo);
    kalClearMgmtFrames(prAdapter->prGlueInfo);

    // 1.2 clear pending TX packet queued in glue layer
    kalFlushPendingTxPackets(prAdapter->prGlueInfo);

    // 2. indiate for OID failure
    kalOidComplete(prAdapter->prGlueInfo,
                   prCmdInfo->fgSetQuery,
                   0,
                   WLAN_STATUS_FAILURE);
}

