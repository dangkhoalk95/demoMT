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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/nic/nic_tx.c#28 $
*/

/*! \file   nic_tx.c
    \brief  Functions that provide TX operation in NIC Layer.

    This file provides TX functions which are responsible for both Hardware and
    Software Resource Management and keep their Synchronization.
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
#include "wndrv_context.h"

extern kal_bool Wifi_Query_Dynamic_Switch_Cachability(void);
extern UINT_32 kalGetTxPendingCmdCount(IN P_GLUE_INFO_T    prGlueInfo);
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
#if __WIFI_HIF_SDIO__

#ifdef __ICCARM__
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN extern unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#else
extern ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#endif

#endif
/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
#if defined (__WIFI_EHPI_16BIT__) && defined (MT6255)
kal_int32  tcpip_data_counter_tx = 0;
kal_int32  tcpip_data_limit_tx = 6;
#elif defined(MT6250)||defined(MT6260)
kal_int32  tcpip_data_counter_tx = 0;
kal_int32  tcpip_data_limit_tx = 4;
extern wndrv_context_struct wndrv_context;
#endif

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/*!
* @brief This function will initial all variables in regard to SW TX Queues and
*        all free lists of MSDU_INFO_T and SW_TFCB_T.
*
* @param prAdapter  Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicTxInitialize(
    IN P_ADAPTER_T  prAdapter
)
{
    P_TX_CTRL_T prTxCtrl;
    PUINT_8 pucMemHandle;
    P_MSDU_INFO_T prMsduInfo;
    UINT_32 i;
    KAL_SPIN_LOCK_DECLARATION();

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "nicTxInitialize");

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    //4 <1> Initialization of Traffic Class Queue Parameters
    nicTxResetResource(prAdapter);

//#if CFG_SDIO_TX_AGG
#if 1   //TODO(Nelson): check if this assignment is correct?
    prTxCtrl->pucTxCoalescingBufPtr = prAdapter->pucCoalescingBufCached;
#endif /* CFG_SDIO_TX_AGG */

    // allocate MSDU_INFO_T and link it into rFreeMsduInfoList
    QUEUE_INITIALIZE(&prTxCtrl->rFreeMsduInfoList);

    pucMemHandle = prTxCtrl->pucTxCached;
    for (i = 0 ; i < CFG_TX_MAX_PKT_NUM ; i++) {
        prMsduInfo = (P_MSDU_INFO_T)pucMemHandle;
        kalMemZero(prMsduInfo, sizeof(MSDU_INFO_T));

        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
        QUEUE_INSERT_TAIL(&prTxCtrl->rFreeMsduInfoList, (P_QUE_ENTRY_T)prMsduInfo);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);

        pucMemHandle += ALIGN_4(sizeof(MSDU_INFO_T));
    }

    ASSERT(prTxCtrl->rFreeMsduInfoList.u4NumElem == CFG_TX_MAX_PKT_NUM);
    /* Check if the memory allocation consist with this initialization function */
    ASSERT((UINT_32)(pucMemHandle - prTxCtrl->pucTxCached) == prTxCtrl->u4TxCachedSize);

    QUEUE_INITIALIZE(&prTxCtrl->rTxMgmtTxingQueue);
    prTxCtrl->i4TxMgmtPendingNum = 0;

#if CFG_HIF_STATISTICS
    prTxCtrl->u4TotalTxAccessNum = 0;
    prTxCtrl->u4TotalTxPacketNum = 0;
#endif

    prTxCtrl->i4PendingFwdFrameCount = 0;

    qmInit(prAdapter);

    return;
} /* end of nicTxInitialize() */


/*----------------------------------------------------------------------------*/
/*!
* \brief Driver maintain a variable that is synchronous with the usage of individual
*        TC Buffer Count. This function will check if has enough TC Buffer for incoming
*        packet and then update the value after promise to provide the resources.
*
* \param[in] prAdapter              Pointer to the Adapter structure.
* \param[in] ucTC                   Specify the resource of TC
*
* \retval WLAN_STATUS_SUCCESS   Resource is available and been assigned.
* \retval WLAN_STATUS_RESOURCES Resource is not available.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxAcquireResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucTC
)
{
    P_TX_CTRL_T prTxCtrl;
    WLAN_STATUS u4Status = WLAN_STATUS_RESOURCES;
    KAL_SPIN_LOCK_DECLARATION();

    //DEBUGFUNC("nicTxAcquireResource");

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;


    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

//    DbgPrint("nicTxAcquireResource prTxCtrl->rTc.aucFreeBufferCount[%d]=%d\n", ucTC, prTxCtrl->rTc.aucFreeBufferCount[ucTC]);

    if (prTxCtrl->rTc.aucFreeBufferCount[ucTC]) {

        prTxCtrl->rTc.aucFreeBufferCount[ucTC]--;

        // DBGLOG(TX, EVENT, ("Acquire: TC = %d aucFreeBufferCount = %d\n",
        //  ucTC, prTxCtrl->rTc.aucFreeBufferCount[ucTC]));
#if CFG_ENABLE_TXRX_PATH_LOG
        MT5931_TRACE(TRACE_TXRX, MT5931_INFO_84, "Acquire: TC = %d aucFreeBufferCount = %d\n",
                     ucTC, prTxCtrl->rTc.aucFreeBufferCount[ucTC]);
#endif
        u4Status = WLAN_STATUS_SUCCESS;
    } else {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_84, "Acquire: TC = %d aucFreeBufferCount = %d\n",
                     ucTC, prTxCtrl->rTc.aucFreeBufferCount[ucTC]);
    }
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

    return u4Status;

}/* end of nicTxAcquireResourceAndTFCBs() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Driver maintain a variable that is synchronous with the usage of individual
*        TC Buffer Count. This function will do polling if FW has return the resource.
*        Used when driver start up before enable interrupt.
*
* @param prAdapter      Pointer to the Adapter structure.
* @param ucTC           Specify the resource of TC
*
* @retval WLAN_STATUS_SUCCESS   Resource is available.
* @retval WLAN_STATUS_FAILURE   Resource is not available.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxPollingResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucTC
)
{
    P_TX_CTRL_T prTxCtrl;
    WLAN_STATUS u4Status = WLAN_STATUS_FAILURE;
    INT_32 i = NIC_TX_RESOURCE_POLLING_TIMEOUT;
    UINT_32 au4WTSR[2];

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    if (ucTC >= TC_NUM) {
        return WLAN_STATUS_FAILURE;
    }

    if (prTxCtrl->rTc.aucFreeBufferCount[ucTC] > 0) {
        return WLAN_STATUS_SUCCESS;
    }

    while (i-- > 0) {
        HAL_READ_TX_RELEASED_COUNT(prAdapter, au4WTSR);

        if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            u4Status = WLAN_STATUS_FAILURE;
            break;
        } else if (nicTxReleaseResource(prAdapter, (PUINT_8)au4WTSR)) {
            if (prTxCtrl->rTc.aucFreeBufferCount[ucTC] > 0) {
                u4Status = WLAN_STATUS_SUCCESS;
                break;
            } else {
                kalMsleep(NIC_TX_RESOURCE_POLLING_DELAY_MSEC);
            }
        } else {
            kalMsleep(NIC_TX_RESOURCE_POLLING_DELAY_MSEC);
        }
    }
    if ((i < 1) && (prAdapter->prGlueInfo->fgIsAdaptStopped == TRUE)) {
        kal_wap_trace(MOD_WNDRV, TRACE_ERROR, "NO resource, whole chip reset.");
        wlanWholeChipReset(prAdapter);
    }
#if DBG
    {
        INT_32 i4Times = NIC_TX_RESOURCE_POLLING_TIMEOUT - (i + 1);

        if (i4Times) {
            //  DBGLOG(TX, TRACE, ("Polling MCR_WTSR delay %d times, %d msec\n",
            //      i4Times, (i4Times * NIC_TX_RESOURCE_POLLING_DELAY_MSEC)));
            kal_trace(TRACE_GROUP_10 , TRACE_TX_Polling, i4Times, (i4Times * NIC_TX_RESOURCE_POLLING_DELAY_MSEC));
        }
    }
#endif /* DBG */

    return u4Status;

} /* end of nicTxPollingResource() */


/*----------------------------------------------------------------------------*/
/*!
* \brief Driver maintain a variable that is synchronous with the usage of individual
*        TC Buffer Count. This function will release TC Buffer count according to
*        the given TX_STATUS COUNTER after TX Done.
*
* \param[in] prAdapter              Pointer to the Adapter structure.
* \param[in] u4TxStatusCnt          Value of TX STATUS
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
nicTxReleaseResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8      *aucTxRlsCnt
)
{
    PUINT_32 pu4Tmp = (PUINT_32)aucTxRlsCnt;
    P_TX_CTRL_T prTxCtrl;
    BOOLEAN bStatus = FALSE;
    UINT_32 i;

    KAL_SPIN_LOCK_DECLARATION();

    //DEBUGFUNC("nicTxReleaseResource");

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    if (pu4Tmp[0] | pu4Tmp[1]) {

        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);
        for (i = 0; i < TC_NUM; i++) {
            prTxCtrl->rTc.aucFreeBufferCount[i] += aucTxRlsCnt[i];

            if ((i == 1) || (i == 5)) {
                // DBGLOG(TX, EVENT, ("Release: i = %d aucFreeBufferCount = %d\n",
                //  i, prTxCtrl->rTc.aucFreeBufferCount[i]));
                //MT5931_TRACE(TRACE_GROUP_3, MT5931_INFO_85, "Release: i = %d aucFreeBufferCount = %d\n",
                //    i, prTxCtrl->rTc.aucFreeBufferCount[i]);
            }
        }
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);
#if 0
        for (i = 0; i < TC_NUM; i++) {
            DBGLOG(INIT, TRACE, ("aucFreeBufferCount[%d]: %d, aucMaxNumOfBuffer[%d]: %d\n",
                                 i, prTxCtrl->rTc.aucFreeBufferCount[i], i, prTxCtrl->rTc.aucMaxNumOfBuffer[i]));
        }
        DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[0]=%d\n", prTxCtrl->rTc.aucFreeBufferCount[0]);
        DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[1]=%d\n", prTxCtrl->rTc.aucFreeBufferCount[1]);
        DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[2]=%d\n", prTxCtrl->rTc.aucFreeBufferCount[2]);
        DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[3]=%d\n", prTxCtrl->rTc.aucFreeBufferCount[3]);
        DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[4]=%d\n", prTxCtrl->rTc.aucFreeBufferCount[4]);
        DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[5]=%d\n", prTxCtrl->rTc.aucFreeBufferCount[5]);
#endif

        /*work around EHPI problem, to avoid assert fail directly in mass production*/
        if (!(prTxCtrl->rTc.aucFreeBufferCount[TC0_INDEX] <= prTxCtrl->rTc.aucMaxNumOfBuffer[TC0_INDEX])) {
#if defined (MT6250)
            EHPI_BUS_ERROR(TC0_INDEX, 0xA0700FE8, 0xA01B001C, 0xA0020810)
#else
            ASSERT(0);
#endif
        }

        if (!(prTxCtrl->rTc.aucFreeBufferCount[TC1_INDEX] <= prTxCtrl->rTc.aucMaxNumOfBuffer[TC1_INDEX])) {
#if defined (MT6250)
            EHPI_BUS_ERROR(TC1_INDEX, 0xA0700FE8, 0xA01B001C, 0xA0020810)
#else
            ASSERT(0);
#endif
        }

        if (!(prTxCtrl->rTc.aucFreeBufferCount[TC2_INDEX] <= prTxCtrl->rTc.aucMaxNumOfBuffer[TC2_INDEX])) {
#if defined (MT6250)
            EHPI_BUS_ERROR(TC2_INDEX, 0xA0700FE8, 0xA01B001C, 0xA0020810)
#else
            ASSERT(0);
#endif
        }

        if (!(prTxCtrl->rTc.aucFreeBufferCount[TC3_INDEX] <= prTxCtrl->rTc.aucMaxNumOfBuffer[TC3_INDEX])) {
#if defined (MT6250)
            EHPI_BUS_ERROR(TC3_INDEX, 0xA0700FE8, 0xA01B001C, 0xA0020810)
#else
            ASSERT(0);
#endif
        }

        if (!(prTxCtrl->rTc.aucFreeBufferCount[TC4_INDEX] <= prTxCtrl->rTc.aucMaxNumOfBuffer[TC4_INDEX])) {
#if defined (MT6250)
            EHPI_BUS_ERROR(TC4_INDEX, 0xA0700FE8, 0xA01B001C, 0xA0020810)
#else
            ASSERT(0);
#endif
        }

        if (!(prTxCtrl->rTc.aucFreeBufferCount[TC5_INDEX] <= prTxCtrl->rTc.aucMaxNumOfBuffer[TC5_INDEX])) {
#if defined (MT6250)
            EHPI_BUS_ERROR(TC5_INDEX, 0xA0700FE8, 0xA01B001C, 0xA0020810)
#else
            ASSERT(0);
#endif
        }

        bStatus = TRUE;
    }

    return bStatus;
} /* end of nicTxReleaseResource() */


/*----------------------------------------------------------------------------*/
/*!
* \brief Reset TC Buffer Count to initialized value
*
* \param[in] prAdapter              Pointer to the Adapter structure.
*
* @return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxResetResource(
    IN P_ADAPTER_T  prAdapter
)
{
    P_TX_CTRL_T prTxCtrl;

    KAL_SPIN_LOCK_DECLARATION();

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "nicTxResetResource");

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC0_INDEX] = NIC_TX_BUFF_COUNT_TC0;
    prTxCtrl->rTc.aucFreeBufferCount[TC0_INDEX] = NIC_TX_BUFF_COUNT_TC0;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC1_INDEX] = NIC_TX_BUFF_COUNT_TC1;
    prTxCtrl->rTc.aucFreeBufferCount[TC1_INDEX] = NIC_TX_BUFF_COUNT_TC1;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC2_INDEX] = NIC_TX_BUFF_COUNT_TC2;
    prTxCtrl->rTc.aucFreeBufferCount[TC2_INDEX] = NIC_TX_BUFF_COUNT_TC2;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC3_INDEX] = NIC_TX_BUFF_COUNT_TC3;
    prTxCtrl->rTc.aucFreeBufferCount[TC3_INDEX] = NIC_TX_BUFF_COUNT_TC3;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC4_INDEX] = NIC_TX_BUFF_COUNT_TC4;
    prTxCtrl->rTc.aucFreeBufferCount[TC4_INDEX] = NIC_TX_BUFF_COUNT_TC4;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC5_INDEX] = NIC_TX_BUFF_COUNT_TC5;
    prTxCtrl->rTc.aucFreeBufferCount[TC5_INDEX] = NIC_TX_BUFF_COUNT_TC5;

    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Driver maintain a variable that is synchronous with the usage of individual
*        TC Buffer Count. This function will return the value for other component
*        which needs this information for making decisions
*
* @param prAdapter      Pointer to the Adapter structure.
* @param ucTC           Specify the resource of TC
*
* @retval UINT_8        The number of corresponding TC number
*/
/*----------------------------------------------------------------------------*/
UINT_8
nicTxGetResource(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucTC
)
{
    P_TX_CTRL_T prTxCtrl;

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    ASSERT(prTxCtrl);

    if (ucTC >= TC_NUM) {
        return 0;
    } else {
        return prTxCtrl->rTc.aucFreeBufferCount[ucTC];
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief In this function, we'll aggregate frame(PACKET_INFO_T)
* corresponding to HIF TX port
*
* @param prAdapter              Pointer to the Adapter structure.
* @param prMsduInfoListHead     a link list of P_MSDU_INFO_T
*
* @retval WLAN_STATUS_SUCCESS   Bus access ok.
* @retval WLAN_STATUS_FAILURE   Bus access fail.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxMsduInfoList(
    IN P_ADAPTER_T      prAdapter,
    IN P_MSDU_INFO_T    prMsduInfoListHead
)
{
    P_MSDU_INFO_T prMsduInfo, prNextMsduInfo;
    QUE_T qDataPort0, qDataPort1;
    WLAN_STATUS status;
#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("TMI", SA_start);
#endif

    //DEBUGFUNC("nicTxMsduInfoList");

    ASSERT(prAdapter);
    ASSERT(prMsduInfoListHead);

    prMsduInfo = prMsduInfoListHead;

    QUEUE_INITIALIZE(&qDataPort0);
    QUEUE_INITIALIZE(&qDataPort1);

    // Separate MSDU_INFO_T lists into 2 categories: for Port#0 & Port#1
    while (prMsduInfo) {
        prNextMsduInfo = (P_MSDU_INFO_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo);
//#if DBG && 0
#if 0
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_PEER, "nicTxMsduInfoList Acquire TC %d net %u mac len %u len %u Type %u 1x %u 11 %u\n",
                         prMsduInfo->ucTC,
                         prMsduInfo->ucNetworkType,
                         prMsduInfo->ucMacHeaderLength,
                         prMsduInfo->u2FrameLength,
                         prMsduInfo->ucPacketType,
                         prMsduInfo->fgIs802_1x,
                         prMsduInfo->fgIs802_11);

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_PEER, "Dest Mac: " MACSTR "\n",
                         MAC2STR(prMsduInfo->aucEthDestAddr));
#endif

        switch (prMsduInfo->ucTC) {
            case TC0_INDEX:
            case TC1_INDEX:
            case TC2_INDEX:
            case TC3_INDEX:
            case TC5_INDEX: // Broadcast/multicast data packets
                QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo) = NULL;
                QUEUE_INSERT_TAIL(&qDataPort0, (P_QUE_ENTRY_T)prMsduInfo);
                //status = nicTxAcquireResource(prAdapter, prMsduInfo->ucTC);
                //ASSERT(status == WLAN_STATUS_SUCCESS)
                while (1) {
                    if ((status = nicTxAcquireResource(prAdapter, prMsduInfo->ucTC)) == WLAN_STATUS_RESOURCES) {
                        if (nicTxPollingResource(prAdapter, prMsduInfo->ucTC) != WLAN_STATUS_SUCCESS) {
                            MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_86, "Fail to get TX resource return within timeout");
                            status = WLAN_STATUS_FAILURE;
                            break;
                        } else {
                            continue;
                        }
                    }
                    break;
                }
                ASSERT(status == WLAN_STATUS_SUCCESS);

                break;

            case TC4_INDEX: // Command or 802.1x packets
                QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo) = NULL;
                QUEUE_INSERT_TAIL(&qDataPort1, (P_QUE_ENTRY_T)prMsduInfo);

                //status = nicTxAcquireResource(prAdapter, prMsduInfo->ucTC);
                //ASSERT(status == WLAN_STATUS_SUCCESS)
                while (1) {
                    if ((status = nicTxAcquireResource(prAdapter, prMsduInfo->ucTC)) == WLAN_STATUS_RESOURCES) {
                        if (nicTxPollingResource(prAdapter, prMsduInfo->ucTC) != WLAN_STATUS_SUCCESS) {
                            MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_87, "Fail to get TX resource return within timeout");
                            status = WLAN_STATUS_FAILURE;
                            break;
                        } else {
                            continue;
                        }
                    }
                    break;
                }
                ASSERT(status == WLAN_STATUS_SUCCESS);

                break;

            default:
                ASSERT(0);
                break;
        }

        prMsduInfo = prNextMsduInfo;
    }

    if (qDataPort0.u4NumElem > 0) {
        nicTxMsduQueue(prAdapter, 0, &qDataPort0);
    }

    if (qDataPort1.u4NumElem > 0) {
        nicTxMsduQueue(prAdapter, 1, &qDataPort1);
    }
#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("TMI", SA_stop);
#endif

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief In this function, we'll write frame(PACKET_INFO_T) into HIF.
*
* @param prAdapter              Pointer to the Adapter structure.
* @param ucPortIdx              Port Number
* @param prQue                  a link list of P_MSDU_INFO_T
*
* @retval WLAN_STATUS_SUCCESS   Bus access ok.
* @retval WLAN_STATUS_FAILURE   Bus access fail.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxMsduQueue(
    IN P_ADAPTER_T  prAdapter,
    UINT_8          ucPortIdx,
    P_QUE_T         prQue
)
{
    P_MSDU_INFO_T prMsduInfo, prNextMsduInfo;
    HIF_TX_HEADER_T rHwTxHeader;
    P_NATIVE_PACKET prNativePacket;
    UINT_16 u2OverallBufferLength;
    UINT_8 ucEtherTypeOffsetInWord;
    PUINT_8 pucOutputBuf = (PUINT_8)NULL; /* Pointer to Transmit Data Structure Frame */
    UINT_32 u4TxHdrSize;
    UINT_32 u4ValidBufSize;
    //UINT_32 u4TotalLength;
    P_TX_CTRL_T prTxCtrl;
    QUE_T rFreeQueue;
#if CFG_TCP_IP_CHKSUM_OFFLOAD
    UINT_8 ucChksumFlag;
#endif
    //UINT_32 u4TotalLen;
    //PUINT_8 pucDmaBuf;
    //UINT_32 u4DmaLen;
    //UINT_32 u4DmaOffset;
#if defined(WCN_MAUI)
    UINT_16 pdu_len;
#endif

    ASSERT(prAdapter);
    ASSERT(ucPortIdx < 2);
    ASSERT(prQue);

    prTxCtrl = &prAdapter->rTxCtrl;
    u4ValidBufSize = prAdapter->u4CoalescingBufCachedSize;

#if CFG_HIF_STATISTICS
    prTxCtrl->u4TotalTxAccessNum++;
    prTxCtrl->u4TotalTxPacketNum += prQue->u4NumElem;
#endif

    QUEUE_INITIALIZE(&rFreeQueue);

    if (prQue->u4NumElem > 0) {
        prMsduInfo = (P_MSDU_INFO_T)QUEUE_GET_HEAD(prQue);
#if __WIFI_HIF_SDIO__
        pucOutputBuf = &wndrv_cmd_temp_buf[0];
#else
        pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
#endif
        //u4TotalLength = 0;

        while (prMsduInfo) {
            kalMemZero(&rHwTxHeader, sizeof(rHwTxHeader));

            prNativePacket = prMsduInfo->prPacket;

            ASSERT(prNativePacket);

            u4TxHdrSize = TX_HDR_SIZE;

            u2OverallBufferLength = ((prMsduInfo->u2FrameLength + TX_HDR_SIZE) &
                                     (UINT_16)HIF_TX_HDR_TX_BYTE_COUNT_MASK);

            rHwTxHeader.u2TxByteCount_UserPriority = u2OverallBufferLength;
            rHwTxHeader.u2TxByteCount_UserPriority |=
                ((UINT_16)prMsduInfo->ucUserPriority << HIF_TX_HDR_USER_PRIORITY_OFFSET);

            if (prMsduInfo->fgIs802_11) {
                ucEtherTypeOffsetInWord =
                    (TX_HDR_SIZE + prMsduInfo->ucMacHeaderLength + prMsduInfo->ucLlcLength) >> 1;
            } else {
                ucEtherTypeOffsetInWord =
                    ((ETHER_HEADER_LEN - ETHER_TYPE_LEN) + TX_HDR_SIZE) >> 1;
            }

            rHwTxHeader.ucEtherTypeOffset =
                ucEtherTypeOffsetInWord & HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK;

            rHwTxHeader.ucResource_PktType_CSflags = (prMsduInfo->ucTC) << HIF_TX_HDR_RESOURCE_OFFSET;
            rHwTxHeader.ucResource_PktType_CSflags |=
                (UINT_8)(((prMsduInfo->ucPacketType) << HIF_TX_HDR_PACKET_TYPE_OFFSET) &
                         (HIF_TX_HDR_PACKET_TYPE_MASK));

#if CFG_TCP_IP_CHKSUM_OFFLOAD
            if (prMsduInfo->eSrc == TX_PACKET_OS
                    || prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
                if (prAdapter->u4CSUMFlags &
                        (CSUM_OFFLOAD_EN_TX_TCP |
                         CSUM_OFFLOAD_EN_TX_UDP |
                         CSUM_OFFLOAD_EN_TX_IP)) {
                    kalQueryTxChksumOffloadParam(prNativePacket, &ucChksumFlag);

                    if (ucChksumFlag & TX_CS_IP_GEN) {
                        rHwTxHeader.ucResource_PktType_CSflags |= (UINT_8)HIF_TX_HDR_IP_CSUM;
                    }

                    if (ucChksumFlag & TX_CS_TCP_UDP_GEN) {
                        rHwTxHeader.ucResource_PktType_CSflags |= (UINT_8)HIF_TX_HDR_TCP_CSUM;
                    }
                }
            }
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */

            rHwTxHeader.u2LLH = prMsduInfo->u2PalLLH;
            rHwTxHeader.ucStaRecIdx = prMsduInfo->ucStaRecIndex;
            rHwTxHeader.ucForwardingType_SessionID_Reserved =
                (prMsduInfo->ucPsForwardingType) | ((prMsduInfo->ucPsSessionID) << HIF_TX_HDR_PS_SESSION_ID_OFFSET)
                | ((prMsduInfo->fgIsBurstEnd) ? HIF_TX_HDR_BURST_END_MASK : 0);

            rHwTxHeader.ucWlanHeaderLength = (prMsduInfo->ucMacHeaderLength & HIF_TX_HDR_WLAN_HEADER_LEN_MASK);
            rHwTxHeader.ucPktFormtId_Flags =
                (prMsduInfo->ucFormatID & HIF_TX_HDR_FORMAT_ID_MASK)
                | ((prMsduInfo->ucNetworkType << HIF_TX_HDR_NETWORK_TYPE_OFFSET) & HIF_TX_HDR_NETWORK_TYPE_MASK)
                | ((prMsduInfo->fgIs802_1x << HIF_TX_HDR_FLAG_1X_FRAME_OFFSET) & HIF_TX_HDR_FLAG_1X_FRAME_MASK)
                | ((prMsduInfo->fgIs802_11 << HIF_TX_HDR_FLAG_802_11_FORMAT_OFFSET) & HIF_TX_HDR_FLAG_802_11_FORMAT_MASK);

            rHwTxHeader.u2SeqNo = prMsduInfo->u2AclSN;

            if (prMsduInfo->pfTxDoneHandler) {
                rHwTxHeader.ucPacketSeqNo = prMsduInfo->ucTxSeqNum;
                rHwTxHeader.ucAck_BIP_BasicRate = HIF_TX_HDR_NEED_ACK;
            } else {
                rHwTxHeader.ucPacketSeqNo = 0;
                rHwTxHeader.ucAck_BIP_BasicRate = 0;
            }

            if (prMsduInfo->fgIsBIP) {
                rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BIP;
            }

            if (prMsduInfo->fgIsBasicRate) {
                rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BASIC_RATE;
            }

#if CFG_SDIO_TX_AGG
            // attach to coalescing buffer
            kalMemCopy(pucOutputBuf + u4TotalLength, &rHwTxHeader, u4TxHdrSize);
            u4TotalLength += u4TxHdrSize;

            if (prMsduInfo->eSrc == TX_PACKET_OS
                    || prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
                kalCopyFrame(prAdapter->prGlueInfo,
                             prNativePacket,
                             pucOutputBuf + u4TotalLength);
            } else if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
                kalMemCopy(pucOutputBuf + u4TotalLength,
                           prNativePacket,
                           prMsduInfo->u2FrameLength);
            } else {
                ASSERT(0);
            }

            u4TotalLength += ALIGN_4(prMsduInfo->u2FrameLength);

#else
#if defined(WCN_MAUI)
            if (prMsduInfo->eSrc == TX_PACKET_OS
                    || prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
#if 0//__WIFI_HIF_SDIO__
                kalMemCopy(pucOutputBuf, &rHwTxHeader, u4TxHdrSize);
                kalCopyFrame(prAdapter->prGlueInfo,
                             prNativePacket,
                             pucOutputBuf + u4TxHdrSize);
#else
                prepend_to_peer_buff((peer_buff_struct *)prNativePacket, &rHwTxHeader, u4TxHdrSize);
                PUINT_8 buff = (PUINT_8)get_pdu_ptr((peer_buff_struct *)prNativePacket, &pdu_len);
                kalMemCopy(pucOutputBuf, buff, pdu_len);
#endif

            } else if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
                kalMemCopy(pucOutputBuf, &rHwTxHeader, u4TxHdrSize);
                kalMemCopy(pucOutputBuf + u4TxHdrSize,
                           prNativePacket,
                           prMsduInfo->u2FrameLength);
            } else {
                ASSERT(0);
            }
#else
#endif

            ASSERT(u2OverallBufferLength <= u4ValidBufSize);

            if (Wifi_Query_Dynamic_Switch_Cachability()) {  //NOTE(Nelson): dest buffer address & size align for dma
#if __WIFI_HIF_SDIO__
                HAL_WRITE_TX_PORT(prAdapter,
                                  ucPortIdx,
                                  (UINT_32)u2OverallBufferLength,
                                  (PUINT_8)pucOutputBuf,
                                  u4ValidBufSize);
#else
#endif
            } else {
                HAL_WRITE_TX_PORT(prAdapter,
                                  ucPortIdx,
                                  (UINT_32)u2OverallBufferLength,
                                  (PUINT_8)pucOutputBuf,
                                  u4ValidBufSize);
            }
#endif

            prNextMsduInfo = (P_MSDU_INFO_T)
                             QUEUE_GET_NEXT_ENTRY(&prMsduInfo->rQueEntry);

            if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
                GLUE_DEC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);

                if (prMsduInfo->pfTxDoneHandler == NULL) {
                    cnmMgtPktFree(prAdapter, prMsduInfo);
                } else {
                    KAL_SPIN_LOCK_DECLARATION();
                    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
                    QUEUE_INSERT_TAIL(&(prTxCtrl->rTxMgmtTxingQueue), (P_QUE_ENTRY_T)prMsduInfo);
                    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
                }
            } else {
                /* only free MSDU when it is not a MGMT frame */
                QUEUE_INSERT_TAIL(&rFreeQueue, (P_QUE_ENTRY_T)prMsduInfo);

                if (prMsduInfo->eSrc == TX_PACKET_OS) {
                    kalSendComplete(prAdapter->prGlueInfo,
                                    prNativePacket,
                                    WLAN_STATUS_SUCCESS);
                } else if (prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
                    GLUE_DEC_REF_CNT(prTxCtrl->i4PendingFwdFrameCount);
                }
            }
            prMsduInfo = prNextMsduInfo;
        }

#if CFG_SDIO_TX_AGG
        ASSERT(u4TotalLength <= u4ValidBufSize);
        // send coalescing buffer
        HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(u4TotalLength));
        if (Wifi_Query_Dynamic_Switch_Cachability()) {  //NOTE(Nelson): dest buffer address & size align for dma
            u4TotalLen = ALIGN_4(u4TotalLength);
            if (u4TotalLen > DMA_THRESHOLD) {
                pucDmaBuf = pucOutputBuf;
                u4DmaLen = u4TotalLen;
                u4DmaOffset = CPU_CACHE_LINE_SIZE - ((UINT_32)pucDmaBuf & CPU_CACHE_LINE_SIZE_MASK);

                //1. 32byte non-ailgn block
                ASSERT((u4DmaOffset == 0) || (u4DmaOffset == CPU_CACHE_LINE_SIZE));

                //2. 32byte-align block
                u4DmaOffset = u4DmaLen & CPU_CACHE_LINE_SIZE_MASK;
                u4DmaLen -= u4DmaOffset;

                HAL_WRITE_TX_PORT(prAdapter, ucPortIdx, (UINT_32)u4DmaLen, (PUINT_8)pucDmaBuf, u4ValidBufSize);

                //3. remain 32byte non-align block
                pucDmaBuf += u4DmaLen;
                u4DmaLen = u4DmaOffset;

                if (u4DmaLen != 0) {
                    kalDevPortWrite_DMA_noneComplete((ucPortIdx == 0) ? MCR_WTDR0 : MCR_WTDR1, u4DmaLen, pucDmaBuf);
                }
            } else {
                HAL_WRITE_TX_PORT(prAdapter,
                                  ucPortIdx,
                                  u4TotalLength,
                                  (PUINT_8)pucOutputBuf,
                                  u4ValidBufSize);
            }
        } else {
            HAL_WRITE_TX_PORT(prAdapter,
                              ucPortIdx,
                              u4TotalLength,
                              (PUINT_8)pucOutputBuf,
                              u4ValidBufSize);
        }
        HAL_MCR_WR(prAdapter, WEHTCR, 4);
#endif

        // return
        nicTxReturnMsduInfo(prAdapter, (P_MSDU_INFO_T)QUEUE_GET_HEAD(&rFreeQueue));
    }

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief In this function, we'll write Command(CMD_INFO_T) into HIF.
*
* @param prAdapter      Pointer to the Adapter structure.
* @param prPacketInfo   Pointer of CMD_INFO_T
* @param ucTC           Specify the resource of TC
*
* @retval WLAN_STATUS_SUCCESS   Bus access ok.
* @retval WLAN_STATUS_FAILURE   Bus access fail.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxCmd(
    IN P_ADAPTER_T      prAdapter,
    IN P_CMD_INFO_T     prCmdInfo,
    IN UINT_8           ucTC
)
{
    P_WIFI_CMD_T prWifiCmd;
    UINT_16 u2OverallBufferLength;
    PUINT_8 pucOutputBuf = (PUINT_8)NULL; /* Pointer to Transmit Data Structure Frame */
    UINT_8 ucPortIdx;
    HIF_TX_HEADER_T rHwTxHeader;
    P_NATIVE_PACKET prNativePacket;
    UINT_8 ucEtherTypeOffsetInWord;
    P_MSDU_INFO_T prMsduInfo;
    P_TX_CTRL_T prTxCtrl;

    KAL_SPIN_LOCK_DECLARATION();


    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prTxCtrl = &prAdapter->rTxCtrl;
#if __WIFI_HIF_SDIO__
    pucOutputBuf = &wndrv_cmd_temp_buf[0];
#else
    pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
#endif

    // <1> Assign Data Port
    if (ucTC != TC4_INDEX) {
        ucPortIdx = 0;
    } else {
        // Broadcast/multicast data frames, 1x frames, command packets, MMPDU
        ucPortIdx = 1;
    }

    if (prCmdInfo->eCmdType == COMMAND_TYPE_SECURITY_FRAME) {
        // <2> Compose HIF_TX_HEADER
        kalMemZero(&rHwTxHeader, sizeof(rHwTxHeader));

        prNativePacket = prCmdInfo->prPacket;

        ASSERT(prNativePacket);

        u2OverallBufferLength = TFCB_FRAME_PAD_TO_DW((prCmdInfo->u2InfoBufLen + TX_HDR_SIZE)
                                & (UINT_16)HIF_TX_HDR_TX_BYTE_COUNT_MASK);

        rHwTxHeader.u2TxByteCount_UserPriority = ((prCmdInfo->u2InfoBufLen + TX_HDR_SIZE)
                & (UINT_16)HIF_TX_HDR_TX_BYTE_COUNT_MASK);
        ucEtherTypeOffsetInWord = ((ETHER_HEADER_LEN - ETHER_TYPE_LEN) + TX_HDR_SIZE) >> 1;

        rHwTxHeader.ucEtherTypeOffset =
            ucEtherTypeOffsetInWord & HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK;

        rHwTxHeader.ucResource_PktType_CSflags = (ucTC << HIF_TX_HDR_RESOURCE_OFFSET);

        rHwTxHeader.ucStaRecIdx = prCmdInfo->ucStaRecIndex;
        rHwTxHeader.ucForwardingType_SessionID_Reserved = HIF_TX_HDR_BURST_END_MASK;

        rHwTxHeader.ucWlanHeaderLength = (ETH_HLEN & HIF_TX_HDR_WLAN_HEADER_LEN_MASK);
        rHwTxHeader.ucPktFormtId_Flags =
            (((UINT_8)(prCmdInfo->eNetworkType) << HIF_TX_HDR_NETWORK_TYPE_OFFSET) & HIF_TX_HDR_NETWORK_TYPE_MASK)
            | ((1 << HIF_TX_HDR_FLAG_1X_FRAME_OFFSET) & HIF_TX_HDR_FLAG_1X_FRAME_MASK);

        rHwTxHeader.u2SeqNo = 0;
        rHwTxHeader.ucPacketSeqNo = 0;
        rHwTxHeader.ucAck_BIP_BasicRate = 0;

        // <2.3> Copy HIF TX HEADER
        kalMemCopy((PVOID)&pucOutputBuf[0], (PVOID)&rHwTxHeader, TX_HDR_SIZE);

        // <3> Copy Frame Body Copy
        kalCopyFrame(prAdapter->prGlueInfo,
                     prNativePacket,
                     pucOutputBuf + TX_HDR_SIZE);
    } else if (prCmdInfo->eCmdType == COMMAND_TYPE_MANAGEMENT_FRAME) {
        prMsduInfo = (P_MSDU_INFO_T)prCmdInfo->prPacket;

        ASSERT(prMsduInfo->fgIs802_11 == TRUE);
        ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

        // <2> Compose HIF_TX_HEADER
        kalMemZero(&rHwTxHeader, sizeof(rHwTxHeader));

        u2OverallBufferLength = ((prMsduInfo->u2FrameLength + TX_HDR_SIZE) &
                                 (UINT_16)HIF_TX_HDR_TX_BYTE_COUNT_MASK);

        rHwTxHeader.u2TxByteCount_UserPriority = u2OverallBufferLength;
        rHwTxHeader.u2TxByteCount_UserPriority |=
            ((UINT_16)prMsduInfo->ucUserPriority << HIF_TX_HDR_USER_PRIORITY_OFFSET);

        ucEtherTypeOffsetInWord =
            (TX_HDR_SIZE + prMsduInfo->ucMacHeaderLength + prMsduInfo->ucLlcLength) >> 1;

        rHwTxHeader.ucEtherTypeOffset =
            ucEtherTypeOffsetInWord & HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK;

        rHwTxHeader.ucResource_PktType_CSflags = (prMsduInfo->ucTC) << HIF_TX_HDR_RESOURCE_OFFSET;
        rHwTxHeader.ucResource_PktType_CSflags |=
            (UINT_8)(((prMsduInfo->ucPacketType) << HIF_TX_HDR_PACKET_TYPE_OFFSET) &
                     (HIF_TX_HDR_PACKET_TYPE_MASK));

        rHwTxHeader.u2LLH = prMsduInfo->u2PalLLH;
        rHwTxHeader.ucStaRecIdx = prMsduInfo->ucStaRecIndex;
        rHwTxHeader.ucForwardingType_SessionID_Reserved =
            (prMsduInfo->ucPsForwardingType) | ((prMsduInfo->ucPsSessionID) << HIF_TX_HDR_PS_SESSION_ID_OFFSET)
            | ((prMsduInfo->fgIsBurstEnd) ? HIF_TX_HDR_BURST_END_MASK : 0);

        rHwTxHeader.ucWlanHeaderLength = (prMsduInfo->ucMacHeaderLength & HIF_TX_HDR_WLAN_HEADER_LEN_MASK);
        rHwTxHeader.ucPktFormtId_Flags =
            (prMsduInfo->ucFormatID & HIF_TX_HDR_FORMAT_ID_MASK)
            | ((prMsduInfo->ucNetworkType << HIF_TX_HDR_NETWORK_TYPE_OFFSET) & HIF_TX_HDR_NETWORK_TYPE_MASK)
            | ((prMsduInfo->fgIs802_1x << HIF_TX_HDR_FLAG_1X_FRAME_OFFSET) & HIF_TX_HDR_FLAG_1X_FRAME_MASK)
            | ((prMsduInfo->fgIs802_11 << HIF_TX_HDR_FLAG_802_11_FORMAT_OFFSET) & HIF_TX_HDR_FLAG_802_11_FORMAT_MASK);

        rHwTxHeader.u2SeqNo = prMsduInfo->u2AclSN;

        if (prMsduInfo->pfTxDoneHandler) {
            rHwTxHeader.ucPacketSeqNo = prMsduInfo->ucTxSeqNum;
            rHwTxHeader.ucAck_BIP_BasicRate = HIF_TX_HDR_NEED_ACK;
        } else {
            rHwTxHeader.ucPacketSeqNo = 0;
            rHwTxHeader.ucAck_BIP_BasicRate = 0;
        }

        if (prMsduInfo->fgIsBIP) {
            rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BIP;
        }

        if (prMsduInfo->fgIsBasicRate) {
            rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BASIC_RATE;
        }

        // <2.3> Copy HIF TX HEADER
        kalMemCopy((PVOID)&pucOutputBuf[0], (PVOID)&rHwTxHeader, TX_HDR_SIZE);

        // <3> Copy Frame Body
        kalMemCopy(pucOutputBuf + TX_HDR_SIZE,
                   prMsduInfo->prPacket,
                   prMsduInfo->u2FrameLength);

        // <4> Management Frame Post-Processing
        GLUE_DEC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);

        if (prMsduInfo->pfTxDoneHandler == NULL) {
            cnmMgtPktFree(prAdapter, prMsduInfo);
        } else {
            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
            QUEUE_INSERT_TAIL(&(prTxCtrl->rTxMgmtTxingQueue), (P_QUE_ENTRY_T)prMsduInfo);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
        }
    } else {
        prWifiCmd = (P_WIFI_CMD_T)prCmdInfo->pucInfoBuffer;
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "CMD ID =%x, SeqNum=%x", prWifiCmd->ucCID, prWifiCmd->ucSeqNum);
        // <2> Compose the Header of Transmit Data Structure for CMD Packet
        u2OverallBufferLength = TFCB_FRAME_PAD_TO_DW(
                                    (prCmdInfo->u2InfoBufLen) & (UINT_16)HIF_TX_HDR_TX_BYTE_COUNT_MASK);

        prWifiCmd->u2TxByteCount_UserPriority = u2OverallBufferLength;
        prWifiCmd->ucEtherTypeOffset = 0;
        prWifiCmd->ucResource_PktType_CSflags = (ucTC << HIF_TX_HDR_RESOURCE_OFFSET)
                                                | (UINT_8)((HIF_TX_PKT_TYPE_CMD << HIF_TX_HDR_PACKET_TYPE_OFFSET) & (HIF_TX_HDR_PACKET_TYPE_MASK));


        // <3> Copy CMD Header to command buffer (by using pucCoalescingBufCached)
        kalMemCopy((PVOID)&pucOutputBuf[0],
                   (PVOID)prCmdInfo->pucInfoBuffer,
                   prCmdInfo->u2InfoBufLen);

        ASSERT(u2OverallBufferLength <= prAdapter->u4CoalescingBufCachedSize);
    }

    // <4> Write frame to data port
    //Nelson(NOTE): add CMD debug log start
    // DBGLOG(TX, TRACE, ("Dump CMD buffer, length = 0x%x, coalescingBuf_size = 0x%x\n",
    //             u2OverallBufferLength, prAdapter->u4CoalescingBufCachedSize));
#if CFG_ENABLE_TXRX_PATH_LOG
    dumpMemory8(pucOutputBuf, u2OverallBufferLength);
#endif
    //Nelson(NOTE): add CMD debug log end

    HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(u2OverallBufferLength));
    HAL_WRITE_TX_PORT(prAdapter,
                      ucPortIdx,
                      (UINT_32)u2OverallBufferLength,
                      (PUINT_8)pucOutputBuf,
                      (UINT_32)prAdapter->u4CoalescingBufCachedSize);
    HAL_MCR_WR(prAdapter, WEHTCR, 4);

    return WLAN_STATUS_SUCCESS;
} /* end of nicTxCmd() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function will clean up all the pending frames in internal SW Queues
*        by return the pending TX packet to the system.
*
* @param prAdapter  Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicTxRelease(
    IN P_ADAPTER_T  prAdapter
)
{
    P_TX_CTRL_T prTxCtrl;
    P_MSDU_INFO_T prMsduInfo;

    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    prTxCtrl = &prAdapter->rTxCtrl;

    nicTxFlush(prAdapter);

    // free MSDU_INFO_T from rTxMgmtMsduInfoList
    do {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
        QUEUE_REMOVE_HEAD(&prTxCtrl->rTxMgmtTxingQueue, prMsduInfo, P_MSDU_INFO_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);

        if (prMsduInfo) {
            // the packet must be mgmt frame with tx done callback
            ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);
            ASSERT(prMsduInfo->pfTxDoneHandler != NULL);

            // invoke done handler
            prMsduInfo->pfTxDoneHandler(prAdapter, prMsduInfo, TX_RESULT_LIFE_TIMEOUT);

            cnmMgtPktFree(prAdapter, prMsduInfo);
        } else {
            break;
        }
    } while (TRUE);

    return;
} /* end of nicTxRelease() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Process the TX Done interrupt and pull in more pending frames in SW
*        Queues for transmission.
*
* @param prAdapter  Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicProcessTxInterrupt(
    IN P_ADAPTER_T prAdapter
)
{
    P_TX_CTRL_T prTxCtrl;
#if CFG_SDIO_INTR_ENHANCE
    P_SDIO_CTRL_T prSDIOCtrl;
#else
    UINT_32 au4TxCount[2];
#endif /* CFG_SDIO_INTR_ENHANCE */
#if CFG_TCP_IP_FLOW_CTRL
    P_GLUE_INFO_T prGlueInfo;
#endif
#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("Rtd", SA_start);
#endif

    ASSERT(prAdapter);

    prTxCtrl = &prAdapter->rTxCtrl;
    ASSERT(prTxCtrl);

    /* Get the TX STATUS */
#if CFG_SDIO_INTR_ENHANCE

    prSDIOCtrl = prAdapter->prSDIOCtrl;
#if DBG
    //dumpMemory8((PUINT_8)prSDIOCtrl, sizeof(SDIO_CTRL_T));
#endif

    nicTxReleaseResource(prAdapter, (PUINT_8)&prSDIOCtrl->rTxInfo);
    kalMemZero(&prSDIOCtrl->rTxInfo, sizeof(prSDIOCtrl->rTxInfo));

#else

    HAL_MCR_RD(prAdapter, MCR_WTSR0, &au4TxCount[0]);
    HAL_MCR_RD(prAdapter, MCR_WTSR1, &au4TxCount[1]);

#if CFG_ENABLE_TXRX_PATH_LOG
    MT5931_TRACE(TRACE_TXRX, MT5931_INFO_95, "nicProcessTxInterrupt:MCR_WTSR0:%x, MCR_WTSR1:%x\n",
                 au4TxCount[0], au4TxCount[1]);
#endif
    nicTxReleaseResource(prAdapter, (PUINT_8)au4TxCount);

#endif /* CFG_SDIO_INTR_ENHANCE */

    nicTxAdjustTcq(prAdapter);
#if CFG_TCP_IP_FLOW_CTRL
    prGlueInfo = prAdapter->prGlueInfo;
    prGlueInfo->fgPending_Ctrl = 0;
#endif
    // Indicate Service Thread
    if (kalGetTxPendingCmdCount(prAdapter->prGlueInfo) > 0
            || wlanGetTxPendingFrameCount(prAdapter) > 0) {
        kalSetEvent(prAdapter->prGlueInfo);
    }

    /* Nelson: workaround for interrupt polling */
    prAdapter->fgIsTxIntPoll = TRUE;

#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("Rtd", SA_stop);
#endif
    return;
} /* end of nicProcessTxInterrupt() */


/*----------------------------------------------------------------------------*/
/*!
* @brief this function frees packet of P_MSDU_INFO_T linked-list
*
* @param prAdapter              Pointer to the Adapter structure.
* @param prMsduInfoList         a link list of P_MSDU_INFO_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicTxFreeMsduInfoPacket(
    IN P_ADAPTER_T    prAdapter,
    IN P_MSDU_INFO_T  prMsduInfoListHead
)
{
    P_NATIVE_PACKET prNativePacket;
    P_MSDU_INFO_T prMsduInfo = prMsduInfoListHead;
    P_TX_CTRL_T prTxCtrl;


    ASSERT(prAdapter);
    //ASSERT(prMsduInfoListHead);

    prTxCtrl = &prAdapter->rTxCtrl;

    while (prMsduInfo) {
        prNativePacket = prMsduInfo->prPacket;

        if (prMsduInfo->eSrc == TX_PACKET_OS) {
            kalSendComplete(prAdapter->prGlueInfo,
                            prNativePacket,
                            WLAN_STATUS_FAILURE);
        } else if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
            if (prMsduInfo->pfTxDoneHandler) {
                prMsduInfo->pfTxDoneHandler(prAdapter, prMsduInfo, TX_RESULT_DROPPED_IN_DRIVER);
            }
            cnmMemFree(prAdapter, prNativePacket);
        } else if (prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
            GLUE_DEC_REF_CNT(prTxCtrl->i4PendingFwdFrameCount);
        }

        prMsduInfo = (P_MSDU_INFO_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo);
    }

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief this function returns P_MSDU_INFO_T of MsduInfoList to TxCtrl->rfreeMsduInfoList
*
* @param prAdapter              Pointer to the Adapter structure.
* @param prMsduInfoList         a link list of P_MSDU_INFO_T
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicTxReturnMsduInfo(
    IN P_ADAPTER_T    prAdapter,
    IN P_MSDU_INFO_T  prMsduInfoListHead
)
{
    P_TX_CTRL_T prTxCtrl;
    P_MSDU_INFO_T prMsduInfo = prMsduInfoListHead, prNextMsduInfo;

    //DEBUGFUNC("nicTxReturnMsduInfo");

    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    prTxCtrl = &prAdapter->rTxCtrl;
    ASSERT(prTxCtrl);

    while (prMsduInfo) {
        prNextMsduInfo = (P_MSDU_INFO_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo);

        switch (prMsduInfo->eSrc) {
            case TX_PACKET_FORWARDING:
                wlanReturnPacket(prAdapter, prMsduInfo->prPacket);
                prMsduInfo->prPacket = NULL;
                break;
            case TX_PACKET_OS:
            case TX_PACKET_OS_OID:
            case TX_PACKET_MGMT:
            default:
                break;
        }
        //MT5931_TRACE(TRACE_GROUP_4, MT5931_INFO_97, "nicTxReturnMsduInfo: retrun prMsduInfo %x", prMsduInfo);
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
        QUEUE_INSERT_TAIL(&prTxCtrl->rFreeMsduInfoList, (P_QUE_ENTRY_T)prMsduInfo);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
        prMsduInfo = prNextMsduInfo;
    };

    return;
}



/*----------------------------------------------------------------------------*/
/*!
* @brief this function fills packet information to P_MSDU_INFO_T
*
* @param prAdapter              Pointer to the Adapter structure.
* @param prMsduInfo             P_MSDU_INFO_T
* @param prPacket               P_NATIVE_PACKET
*
* @retval TRUE      Success to extract information
* @retval FALSE     Fail to extract correct information
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
nicTxFillMsduInfo(
    IN P_ADAPTER_T     prAdapter,
    IN P_MSDU_INFO_T   prMsduInfo,
    IN P_NATIVE_PACKET prPacket
)
{
    P_GLUE_INFO_T   prGlueInfo;
    UINT_8          ucPriorityParam;
    UINT_8          ucMacHeaderLen;
    UINT_8          aucEthDestAddr[PARAM_MAC_ADDR_LEN];
    BOOLEAN         fgIs1x = FALSE;
    BOOLEAN         fgIsPAL = FALSE;
    UINT_32         u4PacketLen;
    ULONG           u4SysTime;
    UINT_8          ucNetworkType;

    wndrv_pkt_descriptor_struct *wndrv_pkt_sec;

    //DEBUGFUNC("nicTxFillMsduInfo");
    ASSERT(prAdapter);

    prGlueInfo = prAdapter->prGlueInfo;
    ASSERT(prGlueInfo);

    if (kalQoSFrameClassifierAndPacketInfo(prGlueInfo,
                                           prPacket,
                                           &ucPriorityParam,
                                           &u4PacketLen,
                                           aucEthDestAddr,
                                           &fgIs1x,
                                           &fgIsPAL,
                                           &ucNetworkType) == FALSE) {
        return FALSE;
    }

#if CFG_ENABLE_TXRX_PATH_LOG
    MT5931_TRACE(TRACE_PEER, MT5931_INFO_98, "nicTxFillMsduInfo: Net %d Is1x %d PktLen %d DestAddr: "MACSTR,
                 ucNetworkType, fgIs1x, u4PacketLen, MAC2STR(aucEthDestAddr));
#endif
    /* Save the value of Priority Parameter */
    //GLUE_SET_PKT_TID(prPacket, ucPriorityParam);
    wndrv_pkt_sec = wndrv_get_pkt_descriptor((peer_buff_struct *)prPacket);
    wndrv_pkt_sec->prPacketInfo->ucTID = ucPriorityParam;

    if (fgIs1x) {
        GLUE_SET_PKT_FLAG_1X(prPacket);
    }

    if (fgIsPAL) {
        GLUE_SET_PKT_FLAG_PAL(prPacket);
    }

    ucMacHeaderLen = ETH_HLEN;

    /* Save the value of Header Length */
    GLUE_SET_PKT_HEADER_LEN(prPacket, ucMacHeaderLen);

    /* Save the value of Frame Length */
    GLUE_SET_PKT_FRAME_LEN(prPacket, (UINT_16)u4PacketLen);

    /* Save the value of Arrival Time*/
    u4SysTime = (OS_SYSTIME)kalGetTimeTick();
    GLUE_SET_PKT_ARRIVAL_TIME(prPacket, u4SysTime);

    prMsduInfo->prPacket = prPacket;
    prMsduInfo->fgIs802_1x = fgIs1x;
    prMsduInfo->fgIs802_11 = FALSE;
    prMsduInfo->ucNetworkType = ucNetworkType;
    prMsduInfo->ucUserPriority = ucPriorityParam;
    prMsduInfo->ucMacHeaderLength = ucMacHeaderLen;
    prMsduInfo->u2FrameLength = (UINT_16)u4PacketLen;
    COPY_MAC_ADDR(prMsduInfo->aucEthDestAddr, aucEthDestAddr);

    return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief this function update TCQ values by passing current status to txAdjustTcQuotas
*
* @param prAdapter              Pointer to the Adapter structure.
*
* @retval WLAN_STATUS_SUCCESS   Updated successfully
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxAdjustTcq(
    IN P_ADAPTER_T  prAdapter
)
{
    UINT_32 u4Num;
    TX_TCQ_ADJUST_T rTcqAdjust;
    P_TX_CTRL_T prTxCtrl;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    prTxCtrl = &prAdapter->rTxCtrl;
    ASSERT(prTxCtrl);

    qmAdjustTcQuotas(prAdapter, &rTcqAdjust, &prTxCtrl->rTc);
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

    for (u4Num = 0 ; u4Num < TC_NUM ; u4Num++) {
        prTxCtrl->rTc.aucFreeBufferCount[u4Num] += rTcqAdjust.acVariation[u4Num];
        prTxCtrl->rTc.aucMaxNumOfBuffer[u4Num] += rTcqAdjust.acVariation[u4Num];

        //ASSERT(prTxCtrl->rTc.aucFreeBufferCount[u4Num] >= 0);
        //ASSERT(prTxCtrl->rTc.aucMaxNumOfBuffer[u4Num] >= 0);
    }

    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief this function flushes all packets queued in STA/AC queue
*
* @param prAdapter              Pointer to the Adapter structure.
*
* @retval WLAN_STATUS_SUCCESS   Flushed successfully
*/
/*----------------------------------------------------------------------------*/

WLAN_STATUS
nicTxFlush(
    IN P_ADAPTER_T  prAdapter
)
{
    P_MSDU_INFO_T prMsduInfo;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    // ask Per STA/AC queue to be fllushed and return all queued packets
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);
    prMsduInfo = qmFlushTxQueues(prAdapter);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);

    if (prMsduInfo != NULL) {
        nicTxFreeMsduInfoPacket(prAdapter, prMsduInfo);
        nicTxReturnMsduInfo(prAdapter, prMsduInfo);
    }

    return WLAN_STATUS_SUCCESS;
}


#if CFG_ENABLE_FW_DOWNLOAD
/*----------------------------------------------------------------------------*/
/*!
* \brief In this function, we'll write Command(CMD_INFO_T) into HIF.
*        However this function is used for INIT_CMD.
*
*        In order to avoid further maintainance issues, these 2 functions are separated
*
* @param prAdapter      Pointer to the Adapter structure.
* @param prPacketInfo   Pointer of CMD_INFO_T
* @param ucTC           Specify the resource of TC
*
* @retval WLAN_STATUS_SUCCESS   Bus access ok.
* @retval WLAN_STATUS_FAILURE   Bus access fail.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxInitCmd(
    IN P_ADAPTER_T      prAdapter,
    IN P_CMD_INFO_T     prCmdInfo,
    IN UINT_8           ucTC
)
{
    P_INIT_HIF_TX_HEADER_T prInitTxHeader;
    UINT_16 u2OverallBufferLength;
    PUINT_8 pucOutputBuf = (PUINT_8)NULL; /* Pointer to Transmit Data Structure Frame */
    UINT_32 ucPortIdx;


    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(ucTC == TC0_INDEX);

#if __WIFI_HIF_SDIO__
    pucOutputBuf = &wndrv_cmd_temp_buf[0];
#else
    P_TX_CTRL_T prTxCtrl;
    prTxCtrl = &prAdapter->rTxCtrl;
    pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
#endif
    prInitTxHeader = (P_INIT_HIF_TX_HEADER_T)prCmdInfo->pucInfoBuffer;

    // <1> Compose the Header of Transmit Data Structure for CMD Packet
    u2OverallBufferLength = TFCB_FRAME_PAD_TO_DW(
                                (prCmdInfo->u2InfoBufLen) & (UINT_16)HIF_TX_HDR_TX_BYTE_COUNT_MASK);

    prInitTxHeader->u2TxByteCount = u2OverallBufferLength;
    prInitTxHeader->ucEtherTypeOffset = 0;
    prInitTxHeader->ucCSflags = 0;

    // <2> Assign Data Port
    if (ucTC != TC4_INDEX) {
        ucPortIdx = 0;
    } else { // Broadcast/multicast data packets
        ucPortIdx = 1;
    }

    // <3> Copy CMD Header to command buffer (by using pucCoalescingBufCached)
    kalMemCopy((PVOID)&pucOutputBuf[0],
               (PVOID)prCmdInfo->pucInfoBuffer,
               prCmdInfo->u2InfoBufLen);

    ASSERT(u2OverallBufferLength <= prAdapter->u4CoalescingBufCachedSize);

    // <4> Write frame to data port
    HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(u2OverallBufferLength));
    HAL_WRITE_TX_PORT(prAdapter,
                      ucPortIdx,
                      (UINT_32)u2OverallBufferLength,
                      (PUINT_8)pucOutputBuf,
                      (UINT_32)prAdapter->u4CoalescingBufCachedSize);
    HAL_MCR_WR(prAdapter, WEHTCR, 4);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief In this function, we'll reset TX resource counter to initial value used
*        in F/W download state
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @retval WLAN_STATUS_SUCCESS   Reset is done successfully.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxInitResetResource(
    IN P_ADAPTER_T  prAdapter
)
{
    P_TX_CTRL_T prTxCtrl;

    DEBUGFUNC("nicTxInitResetResource");

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC0_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC0;
    prTxCtrl->rTc.aucFreeBufferCount[TC0_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC0;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC1_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC1;
    prTxCtrl->rTc.aucFreeBufferCount[TC1_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC1;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC2_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC2;
    prTxCtrl->rTc.aucFreeBufferCount[TC2_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC2;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC3_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC3;
    prTxCtrl->rTc.aucFreeBufferCount[TC3_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC3;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC4_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC4;
    prTxCtrl->rTc.aucFreeBufferCount[TC4_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC4;

    prTxCtrl->rTc.aucMaxNumOfBuffer[TC5_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC5;
    prTxCtrl->rTc.aucFreeBufferCount[TC5_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC5;

    return WLAN_STATUS_SUCCESS;

}

#endif


/*----------------------------------------------------------------------------*/
/*!
* \brief this function enqueues MSDU_INFO_T into queue management,
*        or command queue
*
* @param prAdapter      Pointer to the Adapter structure.
*        prMsduInfo     Pointer to MSDU
*
* @retval WLAN_STATUS_SUCCESS   Reset is done successfully.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicTxEnqueueMsdu(
    IN P_ADAPTER_T      prAdapter,
    IN P_MSDU_INFO_T    prMsduInfo
)
{
    P_TX_CTRL_T prTxCtrl;
    P_MSDU_INFO_T prNextMsduInfo, prRetMsduInfo, prMsduInfoHead;
    QUE_T qDataPort0, qDataPort1;
    P_CMD_INFO_T prCmdInfo;
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
    KAL_SPIN_LOCK_DECLARATION();

    //DEBUGFUNC("nicTxEnqueueMsdu");

    ASSERT(prAdapter);
    ASSERT(prMsduInfo);

    prTxCtrl = &prAdapter->rTxCtrl;
    ASSERT(prTxCtrl);

    QUEUE_INITIALIZE(&qDataPort0);
    QUEUE_INITIALIZE(&qDataPort1);

    /* check how many management frame are being queued */
    while (prMsduInfo) {
        prNextMsduInfo = (P_MSDU_INFO_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo);

        QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prMsduInfo) = NULL;

        if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
            // MMPDU: force stick to TC4
            prMsduInfo->ucTC = TC4_INDEX;

            QUEUE_INSERT_TAIL(&qDataPort1, (P_QUE_ENTRY_T)prMsduInfo);
        } else {
            QUEUE_INSERT_TAIL(&qDataPort0, (P_QUE_ENTRY_T)prMsduInfo);
        }

        prMsduInfo = prNextMsduInfo;
    }

    if (qDataPort0.u4NumElem) {
        /* send to QM */
        KAL_SPIN_LOCK_DECLARATION();
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);
        prRetMsduInfo = qmEnqueueTxPackets(prAdapter, (P_MSDU_INFO_T)QUEUE_GET_HEAD(&qDataPort0));
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);

        /* post-process for dropped packets */
        if (prRetMsduInfo != NULL) { // unable to enqueue
            nicTxFreeMsduInfoPacket(prAdapter, prRetMsduInfo);
            nicTxReturnMsduInfo(prAdapter, prRetMsduInfo);
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Unable to qmEnqueueTxPackets");
        }
    }

    if (qDataPort1.u4NumElem) {
        prMsduInfoHead = (P_MSDU_INFO_T)QUEUE_GET_HEAD(&qDataPort1);

        if (qDataPort1.u4NumElem > nicTxGetFreeCmdCount(prAdapter)) {
            // not enough descriptors for sending
            u4Status = WLAN_STATUS_FAILURE;

            // free all MSDUs
            while (prMsduInfoHead) {
                prNextMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY(&prMsduInfoHead->rQueEntry);
                cnmMgtPktFree(prAdapter, prMsduInfoHead);

                prMsduInfoHead = prNextMsduInfo;
            }
        } else {
            /* send to command queue */
            while (prMsduInfoHead) {
                prNextMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY(&prMsduInfoHead->rQueEntry);

                KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
                QUEUE_REMOVE_HEAD(&prAdapter->rFreeCmdList, prCmdInfo, P_CMD_INFO_T);
                KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);

                if (prCmdInfo) {
                    GLUE_INC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);

                    kalMemZero(prCmdInfo, sizeof(CMD_INFO_T));

                    prCmdInfo->eCmdType             = COMMAND_TYPE_MANAGEMENT_FRAME;
                    prCmdInfo->u2InfoBufLen         = prMsduInfoHead->u2FrameLength;
                    prCmdInfo->pucInfoBuffer        = NULL;
                    prCmdInfo->prPacket             = (P_NATIVE_PACKET)prMsduInfoHead;
                    prCmdInfo->ucStaRecIndex        = prMsduInfoHead->ucStaRecIndex;
                    prCmdInfo->eNetworkType         = (ENUM_NETWORK_TYPE_INDEX_T)prMsduInfoHead->ucNetworkType;
                    prCmdInfo->pfCmdDoneHandler     = NULL;
                    prCmdInfo->pfCmdTimeoutHandler  = NULL;
                    prCmdInfo->fgIsOid              = FALSE;
                    prCmdInfo->fgSetQuery           = TRUE;
                    prCmdInfo->fgNeedResp           = FALSE;

                    kalEnqueueCommand(prAdapter->prGlueInfo, (P_QUE_ENTRY_T)prCmdInfo);
                } else {
                    ASSERT(0);

                    u4Status = WLAN_STATUS_FAILURE;
                    cnmMgtPktFree(prAdapter, prMsduInfoHead);
                }

                prMsduInfoHead = prNextMsduInfo;
            }
        }
    }

    /* indicate service thread for sending */
    if (prTxCtrl->i4TxMgmtPendingNum > 0
            || kalGetTxPendingFrameCount(prAdapter->prGlueInfo) > 0) {
        kalSetEvent(prAdapter->prGlueInfo);
    }

    return u4Status;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief this function returns available count in command queue
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @retval
*/
/*----------------------------------------------------------------------------*/
UINT_32
nicTxGetFreeCmdCount(
    IN P_ADAPTER_T  prAdapter
)
{
    ASSERT(prAdapter);

    return prAdapter->rFreeCmdList.u4NumElem;
}

