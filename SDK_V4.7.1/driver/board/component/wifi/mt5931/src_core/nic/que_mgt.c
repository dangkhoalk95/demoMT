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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/nic/que_mgt.c#50 $
*/

/*! \file   "que_mgt.c"
    \brief  TX/RX queues management

    The main tasks of queue management include TC-based HIF TX flow control,
    adaptive TC quota adjustment, HIF TX grant scheduling, Power-Save
    forwarding control, RX packet reordering, and RX BA agreement management.
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
extern VOID kalSetEvent(P_GLUE_INFO_T pr);
extern void dumpSTA(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec);

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
__KAL_INLINE__ VOID
qmDetermineStaRecIndex(
    IN P_ADAPTER_T   prAdapter,
    IN P_MSDU_INFO_T prMsduInfo
);

__KAL_INLINE__ VOID
qmDequeueTxPacketsFromPerStaQueues(
    IN P_ADAPTER_T  prAdapter,
    OUT P_QUE_T prQue,
    IN  UINT_8  ucTC,
    IN  UINT_8  ucCurrentAvailableQuota,
    IN  UINT_8  ucTotalQuota
);

__KAL_INLINE__ VOID
qmDequeueTxPacketsFromPerTypeQueues(
    IN P_ADAPTER_T  prAdapter,
    OUT P_QUE_T prQue,
    IN  UINT_8  ucTC,
    IN  UINT_8  ucMaxNum
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*----------------------------------------------------------------------------*/
/*!
* \brief Init Queue Managment for TX
*
* \param[in] (none)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmInit(
    IN P_ADAPTER_T  prAdapter
)
{
    UINT_32 u4QueArrayIdx;
    UINT_32 i;

    P_QUE_MGT_T prQM = &prAdapter->rQM;

    //DbgPrint("QM: Enter qmInit()\n");
#if CFG_SUPPORT_QOS
    prAdapter->rWifiVar.fgSupportQoS = TRUE;
#else
    prAdapter->rWifiVar.fgSupportQoS = FALSE;
#endif

#if CFG_SUPPORT_AMPDU_RX
    prAdapter->rWifiVar.fgSupportAmpduRx = TRUE;
#else
    prAdapter->rWifiVar.fgSupportAmpduRx = FALSE;
#endif

#if CFG_SUPPORT_AMPDU_TX
    prAdapter->rWifiVar.fgSupportAmpduTx = TRUE;
#else
    prAdapter->rWifiVar.fgSupportAmpduTx = FALSE;
#endif

#if CFG_SUPPORT_TSPEC
    prAdapter->rWifiVar.fgSupportTspec = TRUE;
#else
    prAdapter->rWifiVar.fgSupportTspec = FALSE;
#endif

#if CFG_SUPPORT_UAPSD
    prAdapter->rWifiVar.fgSupportUAPSD = TRUE;
#else
    prAdapter->rWifiVar.fgSupportUAPSD = FALSE;
#endif

#if CFG_SUPPORT_UL_PSMP
    prAdapter->rWifiVar.fgSupportULPSMP = TRUE;
#else
    prAdapter->rWifiVar.fgSupportULPSMP = FALSE;
#endif

    //4 <2> Initialize other TX queues (queues not in STA_RECs)
    for (u4QueArrayIdx = 0; u4QueArrayIdx < NUM_OF_PER_TYPE_TX_QUEUES; u4QueArrayIdx++) {
        QUEUE_INITIALIZE(&(prQM->arTxQueue[u4QueArrayIdx]));
    }

    //4 <3> Initialize the RX BA table and RX queues
    /* Initialize the RX Reordering Parameters and Queues */
    for (u4QueArrayIdx = 0; u4QueArrayIdx < CFG_NUM_OF_RX_BA_AGREEMENTS; u4QueArrayIdx++) {
        prQM->arRxBaTable[u4QueArrayIdx].fgIsValid = FALSE;
        QUEUE_INITIALIZE(&(prQM->arRxBaTable[u4QueArrayIdx].rReOrderQue));
        prQM->arRxBaTable[u4QueArrayIdx].u2WinStart = 0xFFFF;
        prQM->arRxBaTable[u4QueArrayIdx].u2WinEnd = 0xFFFF;

        prQM->arRxBaTable[u4QueArrayIdx].fgIsWaitingForPktWithSsn = FALSE;

    }
    prQM->ucRxBaCount = 0;

#if QM_ADAPTIVE_TC_RESOURCE_CTRL
    //4 <4> Initialize TC resource control variables
    for (i = 0; i < TC_NUM; i++) {
        prQM->au4AverageQueLen[i] = 0;
    }
    prQM->u4TimeToAdjustTcResource = QM_INIT_TIME_TO_ADJUST_TC_RSC;
    prQM->u4TimeToUpdateQueLen = QM_INIT_TIME_TO_UPDATE_QUE_LEN;

//    ASSERT(prQM->u4TimeToAdjust && prQM->u4TimeToUpdateQueLen);

    prQM->au4CurrentTcResource[TC0_INDEX] = NIC_TX_BUFF_COUNT_TC0;
    prQM->au4CurrentTcResource[TC1_INDEX] = NIC_TX_BUFF_COUNT_TC1;
    prQM->au4CurrentTcResource[TC2_INDEX] = NIC_TX_BUFF_COUNT_TC2;
    prQM->au4CurrentTcResource[TC3_INDEX] = NIC_TX_BUFF_COUNT_TC3;
    prQM->au4CurrentTcResource[TC4_INDEX] = NIC_TX_BUFF_COUNT_TC4; /* Not adjustable (TX port 1)*/
    prQM->au4CurrentTcResource[TC5_INDEX] = NIC_TX_BUFF_COUNT_TC5;

    prQM->au4MinReservedTcResource[TC0_INDEX] = QM_MIN_RESERVED_TC0_RESOURCE;
    prQM->au4MinReservedTcResource[TC1_INDEX] = QM_MIN_RESERVED_TC1_RESOURCE;
    prQM->au4MinReservedTcResource[TC2_INDEX] = QM_MIN_RESERVED_TC2_RESOURCE;
    prQM->au4MinReservedTcResource[TC3_INDEX] = QM_MIN_RESERVED_TC3_RESOURCE;
    prQM->au4MinReservedTcResource[TC4_INDEX] = QM_MIN_RESERVED_TC4_RESOURCE; /* Not adjustable (TX port 1)*/
    prQM->au4MinReservedTcResource[TC5_INDEX] = QM_MIN_RESERVED_TC5_RESOURCE;


    prQM->au4GuaranteedTcResource[TC0_INDEX] = QM_GUARANTEED_TC0_RESOURCE;
    prQM->au4GuaranteedTcResource[TC1_INDEX] = QM_GUARANTEED_TC1_RESOURCE;
    prQM->au4GuaranteedTcResource[TC2_INDEX] = QM_GUARANTEED_TC2_RESOURCE;
    prQM->au4GuaranteedTcResource[TC3_INDEX] = QM_GUARANTEED_TC3_RESOURCE;
    prQM->au4GuaranteedTcResource[TC4_INDEX] = QM_GUARANTEED_TC4_RESOURCE;
    prQM->au4GuaranteedTcResource[TC5_INDEX] = QM_GUARANTEED_TC5_RESOURCE;

    prQM->fgTcResourcePostAnnealing = FALSE;

    ASSERT(QM_INITIAL_RESIDUAL_TC_RESOURCE < 64);
#endif

#if QM_TEST_MODE
    prQM->u4PktCount = 0;

#if QM_TEST_FAIR_FORWARDING

    prQM->u4CurrentStaRecIndexToEnqueue = 0;
    {
        UINT_8 aucMacAddr[MAC_ADDR_LEN];
        P_STA_RECORD_T prStaRec;

        /* Irrelevant in case this STA is an AIS AP (see qmDetermineStaRecIndex()) */
        aucMacAddr[0] = 0x11;
        aucMacAddr[1] = 0x22;
        aucMacAddr[2] = 0xAA;
        aucMacAddr[3] = 0xBB;
        aucMacAddr[4] = 0xCC;
        aucMacAddr[5] = 0xDD;

        prStaRec = &prAdapter->arStaRec[1];
        ASSERT(prStaRec);

        prStaRec->fgIsValid      = TRUE;
        prStaRec->fgIsQoS        = TRUE;
        prStaRec->fgIsInPS        = FALSE;
        prStaRec->ucPsSessionID  = 0xFF;
        prStaRec->ucNetTypeIndex = NETWORK_TYPE_AIS_INDEX;
        prStaRec->fgIsAp         = TRUE;
        COPY_MAC_ADDR((prStaRec)->aucMacAddr, aucMacAddr);

    }

#endif

#endif

#if QM_FORWARDING_FAIRNESS
    {
        UINT_32 i;
        for (i = 0; i < NUM_OF_PER_STA_TX_QUEUES; i++) {
            prQM->au4ForwardCount[i] = 0;
            prQM->au4HeadStaRecIndex[i] = 0;
        }
    }
#endif

}

#if QM_TEST_MODE
VOID
qmTestCases(
    IN P_ADAPTER_T  prAdapter
)
{
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    DbgPrint("QM: ** TEST MODE **\n");

    if (QM_TEST_STA_REC_DETERMINATION) {
        if (prAdapter->arStaRec[0].fgIsValid) {
            prAdapter->arStaRec[0].fgIsValid = FALSE;
            DbgPrint("QM: (Test) Deactivate STA_REC[0]\n");
        } else {
            prAdapter->arStaRec[0].fgIsValid = TRUE;
            DbgPrint("QM: (Test) Activate STA_REC[0]\n");
        }
    }

    if (QM_TEST_STA_REC_DEACTIVATION) {
        /* Note that QM_STA_REC_HARD_CODING shall be set to 1 for this test */

        if (prAdapter->arStaRec[0].fgIsValid) {

            DbgPrint("QM: (Test) Deactivate STA_REC[0]\n");
            qmDeactivateStaRec(prAdapter, 0);
        } else {

            UINT_8 aucMacAddr[MAC_ADDR_LEN];

            /* Irrelevant in case this STA is an AIS AP (see qmDetermineStaRecIndex()) */
            aucMacAddr[0] = 0x11;
            aucMacAddr[1] = 0x22;
            aucMacAddr[2] = 0xAA;
            aucMacAddr[3] = 0xBB;
            aucMacAddr[4] = 0xCC;
            aucMacAddr[5] = 0xDD;

            DbgPrint("QM: (Test) Activate STA_REC[0]\n");
            qmActivateStaRec(
                prAdapter,    /* Adapter pointer */
                0,                      /* STA_REC index from FW */
                TRUE,                   /* fgIsQoS */
                NETWORK_TYPE_AIS_INDEX, /* Network type */
                TRUE,                   /* fgIsAp */
                aucMacAddr              /* MAC address */
            );
        }
    }

    if (QM_TEST_FAIR_FORWARDING) {
        if (prAdapter->arStaRec[1].fgIsValid) {
            prQM->u4CurrentStaRecIndexToEnqueue ++;
            prQM->u4CurrentStaRecIndexToEnqueue %= 2;
            DbgPrint("QM: (Test) Switch to STA_REC[%ld]\n", prQM->u4CurrentStaRecIndexToEnqueue);
        }
    }

}
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Activate a STA_REC
*
* \param[in] prAdapter  Pointer to the Adapter instance
* \param[in] u4StaRecIdx The index of the STA_REC
* \param[in] fgIsQoS    Set to TRUE if this is a QoS STA
* \param[in] pucMacAddr The MAC address of the STA
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmActivateStaRec(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T  prStaRec
)
{
    DEBUGFUNC("qmActivateStaRec");
    //4 <1> Deactivate first
    ASSERT(prStaRec);

    if (prStaRec->fgIsValid) { /* The STA_REC has been activated */
        DbgPrint("QM: (WARNING) Activating a STA_REC which has been activated \n");
        DbgPrint("QM: (WARNING) Deactivating a STA_REC before re-activating \n");
        qmDeactivateStaRec(prAdapter, prStaRec->ucIndex); // To flush TX/RX queues and del RX BA agreements
    }

    //4 <2> Activate the STA_REC
    /* Init the STA_REC */
    prStaRec->fgIsValid      = TRUE;
    prStaRec->fgIsInPS       = FALSE;
    prStaRec->ucPsSessionID  = 0xFF;
    prStaRec->fgIsAp         = (IS_AP_STA(prStaRec)) ? TRUE : FALSE;;

    /* Done in qmInit() or qmDeactivateStaRec() */
#if 0
    /* At the beginning, no RX BA agreements have been established */
    for (i = 0; i < CFG_RX_MAX_BA_TID_NUM; i++) {
        (prStaRec->aprRxReorderParamRefTbl)[i] = NULL;
    }
#endif

    DbgPrint("QM: +STA[%ld]\n", prStaRec->ucIndex);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_99, "QM: +STA[%d]\n", prStaRec->ucIndex);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Deactivate a STA_REC
*
* \param[in] prAdapter  Pointer to the Adapter instance
* \param[in] u4StaRecIdx The index of the STA_REC
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmDeactivateStaRec(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4StaRecIdx
)
{
    P_STA_RECORD_T prStaRec;
    UINT_32 i;
    P_MSDU_INFO_T prFlushedTxPacketList = NULL;

    //do {
    ASSERT(u4StaRecIdx < CFG_NUM_OF_STA_RECORD);

    prStaRec = &prAdapter->arStaRec[u4StaRecIdx];
    ASSERT(prStaRec);

#if 0   /* NOTE(Nelson): Do not remove enqueud MMPDU while connection state from calss3 -> class1 */
    /* If not activated, do nothing */
    if (!prStaRec->fgIsValid) {
        DbgPrint("QM: (WARNING) STA[%ld] shall not be deactivated again\n", u4StaRecIdx);
        prFlushedTxPacketList = nicGetPendingStaMMPDU(prAdapter, (UINT_8)u4StaRecIdx);
        break;
    }
#endif

    //4<1> Flush TX queues
    prFlushedTxPacketList = qmFlushStaTxQueues(prAdapter, u4StaRecIdx);

    if (prFlushedTxPacketList) {
        wlanProcessQueuedMsduInfo(prAdapter, prFlushedTxPacketList);
    }

    //4 <2> Flush RX queues and delete RX BA agreements
    for (i = 0; i < CFG_RX_MAX_BA_TID_NUM; i++) {
        /* Delete the RX BA entry with TID = i */
        qmDelRxBaEntry(prAdapter, (UINT_8)u4StaRecIdx, (UINT_8)i, FALSE);
    }

    //4 <3> Deactivate the STA_REC
    prStaRec->fgIsValid = FALSE;
    prStaRec->fgIsInPS = FALSE;

    DbgPrint("QM: -STA[%ld]\n", u4StaRecIdx);
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Deactivate a STA_REC
*
* \param[in] prAdapter  Pointer to the Adapter instance
* \param[in] u4StaRecIdx The index of the network
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/

VOID
qmFreeAllByNetType(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
)
{

    P_QUE_MGT_T prQM;
    P_QUE_T     prQue;
    QUE_T       rNeedToFreeQue;
    QUE_T       rTempQue;
    P_QUE_T     prNeedToFreeQue;
    P_QUE_T     prTempQue;
    P_MSDU_INFO_T prMsduInfo;


    prQM = &prAdapter->rQM;
    prQue = &prQM->arTxQueue[TX_QUEUE_INDEX_BMCAST];

    QUEUE_INITIALIZE(&rNeedToFreeQue);
    QUEUE_INITIALIZE(&rTempQue);

    prNeedToFreeQue = &rNeedToFreeQue;
    prTempQue = &rTempQue;

    QUEUE_MOVE_ALL(prTempQue, prQue);

    QUEUE_REMOVE_HEAD(prTempQue, prMsduInfo, P_MSDU_INFO_T);
    while (prMsduInfo) {

        if (prMsduInfo->ucNetworkType == eNetworkTypeIdx) {
            QUEUE_INSERT_TAIL(prNeedToFreeQue, (P_QUE_ENTRY_T)prMsduInfo);
        } else {
            QUEUE_INSERT_TAIL(prQue, (P_QUE_ENTRY_T)prMsduInfo);
        }

        QUEUE_REMOVE_HEAD(prTempQue, prMsduInfo, P_MSDU_INFO_T);
    }
    if (QUEUE_IS_NOT_EMPTY(prNeedToFreeQue)) {
        wlanProcessQueuedMsduInfo(prAdapter, (P_MSDU_INFO_T)QUEUE_GET_HEAD(prNeedToFreeQue));
    }

}

/*----------------------------------------------------------------------------*/
/*!
* \brief Flush all TX queues
*
* \param[in] (none)
*
* \return The flushed packets (in a list of MSDU_INFOs)
*/
/*----------------------------------------------------------------------------*/
P_MSDU_INFO_T
qmFlushTxQueues(
    IN P_ADAPTER_T prAdapter
)
{
    UINT_8 ucStaArrayIdx;
    UINT_8 ucQueArrayIdx;

    P_MSDU_INFO_T prMsduInfoListHead;
    P_MSDU_INFO_T prMsduInfoListTail;

    P_QUE_MGT_T prQM = &prAdapter->rQM;

    DbgPrint("QM: Enter qmFlushTxQueues()\n");

    prMsduInfoListHead = NULL;
    prMsduInfoListTail = NULL;

    /* Concatenate all MSDU_INFOs in per-STA queues */
    for (ucStaArrayIdx = 0; ucStaArrayIdx < CFG_NUM_OF_STA_RECORD; ucStaArrayIdx++) {

        /* Always check each STA_REC when flushing packets no matter it is inactive or active */
#if 0
        if (!prAdapter->arStaRec[ucStaArrayIdx].fgIsValid) {
            continue; /* Continue to check the next STA_REC */
        }
#endif

        for (ucQueArrayIdx = 0; ucQueArrayIdx < NUM_OF_PER_STA_TX_QUEUES; ucQueArrayIdx++) {
            if (QUEUE_IS_EMPTY(&(prAdapter->arStaRec[ucStaArrayIdx].arTxQueue[ucQueArrayIdx]))) {
                continue; /* Continue to check the next TX queue of the same STA */
            }

            if (!prMsduInfoListHead) {

                /* The first MSDU_INFO is found */
                prMsduInfoListHead = (P_MSDU_INFO_T)
                                     QUEUE_GET_HEAD(&prAdapter->arStaRec[ucStaArrayIdx].arTxQueue[ucQueArrayIdx]);
                prMsduInfoListTail = (P_MSDU_INFO_T)
                                     QUEUE_GET_TAIL(&prAdapter->arStaRec[ucStaArrayIdx].arTxQueue[ucQueArrayIdx]);
            } else {
                /* Concatenate the MSDU_INFO list with the existing list */
                QM_TX_SET_NEXT_MSDU_INFO(prMsduInfoListTail,
                                         QUEUE_GET_HEAD(&prAdapter->arStaRec[ucStaArrayIdx].arTxQueue[ucQueArrayIdx]));

                prMsduInfoListTail = (P_MSDU_INFO_T)
                                     QUEUE_GET_TAIL(&prAdapter->arStaRec[ucStaArrayIdx].arTxQueue[ucQueArrayIdx]);
            }

            QUEUE_INITIALIZE(&prAdapter->arStaRec[ucStaArrayIdx].arTxQueue[ucQueArrayIdx]);
        }
    }

    /* Flush per-Type queues */
    for (ucQueArrayIdx = 0; ucQueArrayIdx < NUM_OF_PER_TYPE_TX_QUEUES; ucQueArrayIdx++) {

        if (QUEUE_IS_EMPTY(&(prQM->arTxQueue[ucQueArrayIdx]))) {
            continue; /* Continue to check the next TX queue of the same STA */
        }

        if (!prMsduInfoListHead) {

            /* The first MSDU_INFO is found */
            prMsduInfoListHead = (P_MSDU_INFO_T)
                                 QUEUE_GET_HEAD(&prQM->arTxQueue[ucQueArrayIdx]);
            prMsduInfoListTail = (P_MSDU_INFO_T)
                                 QUEUE_GET_TAIL(&prQM->arTxQueue[ucQueArrayIdx]);
        } else {
            /* Concatenate the MSDU_INFO list with the existing list */
            QM_TX_SET_NEXT_MSDU_INFO(prMsduInfoListTail,
                                     QUEUE_GET_HEAD(&prQM->arTxQueue[ucQueArrayIdx]));

            prMsduInfoListTail = (P_MSDU_INFO_T)
                                 QUEUE_GET_TAIL(&prQM->arTxQueue[ucQueArrayIdx]);
        }

        QUEUE_INITIALIZE(&prQM->arTxQueue[ucQueArrayIdx]);

    }

    if (prMsduInfoListTail) {
        /* Terminate the MSDU_INFO list with a NULL pointer */
        QM_TX_SET_NEXT_MSDU_INFO(prMsduInfoListTail, NULL);
    }

    return prMsduInfoListHead;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Flush TX packets for a particular STA
*
* \param[in] u4StaRecIdx STA_REC index
*
* \return The flushed packets (in a list of MSDU_INFOs)
*/
/*----------------------------------------------------------------------------*/
P_MSDU_INFO_T
qmFlushStaTxQueues(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4StaRecIdx
)
{
    UINT_8 ucQueArrayIdx;
    P_MSDU_INFO_T prMsduInfoListHead;
    P_MSDU_INFO_T prMsduInfoListTail;
    P_STA_RECORD_T prStaRec;

    DbgPrint("QM: Enter qmFlushStaTxQueues(%ld)\n", u4StaRecIdx);

    ASSERT(u4StaRecIdx < CFG_NUM_OF_STA_RECORD);

    prMsduInfoListHead = NULL;
    prMsduInfoListTail = NULL;

    prStaRec = &prAdapter->arStaRec[u4StaRecIdx];
    ASSERT(prStaRec);

    /* No matter whether this is an activated STA_REC, do flush */
#if 0
    if (!prStaRec->fgIsValid) {
        return NULL;
    }
#endif

    /* Concatenate all MSDU_INFOs in TX queues of this STA_REC */
    for (ucQueArrayIdx = 0; ucQueArrayIdx < NUM_OF_PER_STA_TX_QUEUES; ucQueArrayIdx++) {
        if (QUEUE_IS_EMPTY(&(prStaRec->arTxQueue[ucQueArrayIdx]))) {
            continue;
        }

        if (!prMsduInfoListHead) {
            /* The first MSDU_INFO is found */
            prMsduInfoListHead = (P_MSDU_INFO_T)
                                 QUEUE_GET_HEAD(&prStaRec->arTxQueue[ucQueArrayIdx]);
            prMsduInfoListTail = (P_MSDU_INFO_T)
                                 QUEUE_GET_TAIL(&prStaRec->arTxQueue[ucQueArrayIdx]);
        } else {
            /* Concatenate the MSDU_INFO list with the existing list */
            QM_TX_SET_NEXT_MSDU_INFO(prMsduInfoListTail,
                                     QUEUE_GET_HEAD(&prStaRec->arTxQueue[ucQueArrayIdx]));

            prMsduInfoListTail =
                (P_MSDU_INFO_T)QUEUE_GET_TAIL(&prStaRec->arTxQueue[ucQueArrayIdx]);
        }

        QUEUE_INITIALIZE(&prStaRec->arTxQueue[ucQueArrayIdx]);

    }

#if 0   /* NOTE(Nelson): Do not remove enqueud MMPDU while connection state from calss3 -> class1 */
    if (prMsduInfoListTail) {
        /* Terminate the MSDU_INFO list with a NULL pointer */
        QM_TX_SET_NEXT_MSDU_INFO(prMsduInfoListTail, nicGetPendingStaMMPDU(prAdapter, (UINT_8)u4StaRecIdx));
    } else {
        prMsduInfoListHead = nicGetPendingStaMMPDU(prAdapter, (UINT_8)u4StaRecIdx);
    }
#endif

    return prMsduInfoListHead;

}

/*----------------------------------------------------------------------------*/
/*!
* \brief Flush RX packets
*
* \param[in] (none)
*
* \return The flushed packets (in a list of SW_RFBs)
*/
/*----------------------------------------------------------------------------*/
P_SW_RFB_T
qmFlushRxQueues(
    IN P_ADAPTER_T  prAdapter
)
{
    UINT_32 i;
    P_SW_RFB_T prSwRfbListHead;
    P_SW_RFB_T prSwRfbListTail;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    prSwRfbListHead = prSwRfbListTail = NULL;

    DbgPrint("QM: Enter qmFlushRxQueues()\n");

    for (i = 0; i < CFG_NUM_OF_RX_BA_AGREEMENTS; i++) {
        if (QUEUE_IS_NOT_EMPTY(&(prQM->arRxBaTable[i].rReOrderQue))) {
            if (!prSwRfbListHead) {

                /* The first MSDU_INFO is found */
                prSwRfbListHead = (P_SW_RFB_T)
                                  QUEUE_GET_HEAD(&(prQM->arRxBaTable[i].rReOrderQue));
                prSwRfbListTail = (P_SW_RFB_T)
                                  QUEUE_GET_TAIL(&(prQM->arRxBaTable[i].rReOrderQue));
            } else {
                /* Concatenate the MSDU_INFO list with the existing list */
                QM_TX_SET_NEXT_MSDU_INFO(prSwRfbListTail,
                                         QUEUE_GET_HEAD(&(prQM->arRxBaTable[i].rReOrderQue)));

                prSwRfbListTail = (P_SW_RFB_T)
                                  QUEUE_GET_TAIL(&(prQM->arRxBaTable[i].rReOrderQue));
            }

            QUEUE_INITIALIZE(&(prQM->arRxBaTable[i].rReOrderQue));

        } else {
            continue;
        }
    }

    if (prSwRfbListTail) {
        /* Terminate the MSDU_INFO list with a NULL pointer */
        QM_TX_SET_NEXT_SW_RFB(prSwRfbListTail, NULL);
    }
    return prSwRfbListHead;

}


/*----------------------------------------------------------------------------*/
/*!
* \brief Flush RX packets with respect to a particular STA
*
* \param[in] u4StaRecIdx STA_REC index
* \param[in] u4Tid TID
*
* \return The flushed packets (in a list of SW_RFBs)
*/
/*----------------------------------------------------------------------------*/
P_SW_RFB_T
qmFlushStaRxQueue(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4StaRecIdx,
    IN UINT_32 u4Tid
)
{
    //UINT_32 i;
    P_SW_RFB_T prSwRfbListHead;
    P_SW_RFB_T prSwRfbListTail;
    P_RX_BA_ENTRY_T prReorderQueParm;
    P_STA_RECORD_T prStaRec;


    prSwRfbListHead = prSwRfbListTail = NULL;

    prStaRec = &prAdapter->arStaRec[u4StaRecIdx];
    ASSERT(prStaRec);

    /* No matter whether this is an activated STA_REC, do flush */
#if 0
    if (!prStaRec->fgIsValid) {
        return NULL;
    }
#endif

    /* Obtain the RX BA Entry pointer */
    prReorderQueParm = ((prStaRec->aprRxReorderParamRefTbl)[u4Tid]);

    /* Note: For each queued packet, prCurrSwRfb->eDst equals RX_PKT_DESTINATION_HOST */
    if (prReorderQueParm) {

        if (QUEUE_IS_NOT_EMPTY(&(prReorderQueParm->rReOrderQue))) {

            prSwRfbListHead = (P_SW_RFB_T)
                              QUEUE_GET_HEAD(&(prReorderQueParm->rReOrderQue));
            prSwRfbListTail = (P_SW_RFB_T)
                              QUEUE_GET_TAIL(&(prReorderQueParm->rReOrderQue));


            QUEUE_INITIALIZE(&(prReorderQueParm->rReOrderQue));

        }
    }

    if (prSwRfbListTail) {
        /* Terminate the MSDU_INFO list with a NULL pointer */
        QM_TX_SET_NEXT_SW_RFB(prSwRfbListTail, NULL);
    }
    return prSwRfbListHead;


}


/*----------------------------------------------------------------------------*/
/*!
* \brief Enqueue TX packets
*
* \param[in] prMsduInfoListHead Pointer to the list of TX packets
*
* \return The freed packets, which are not enqueued
*/
/*----------------------------------------------------------------------------*/
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code = "WIFISpeedUp", rwdata = "WIFISpeedUp",  zidata = "WIFISpeedUp"
#endif

P_MSDU_INFO_T
qmEnqueueTxPackets(
    IN P_ADAPTER_T   prAdapter,
    IN P_MSDU_INFO_T prMsduInfoListHead
)
{
    P_MSDU_INFO_T   prMsduInfoReleaseList;
    P_MSDU_INFO_T   prCurrentMsduInfo;
    P_MSDU_INFO_T   prNextMsduInfo;

    P_STA_RECORD_T  prStaRec;
    P_QUE_T         prTxQue;
    QUE_T           rNotEnqueuedQue;


    UINT_8          ucPacketType;
    UINT_8          ucTC;
    P_QUE_MGT_T prQM = &prAdapter->rQM;
    UINT_8 aucNextUP[WMM_AC_INDEX_NUM] = { 1 /* BEtoBK*/, 1 /*na*/, 0/*VItoBE*/ , 4 /*VOtoVI*/};


    ASSERT(prMsduInfoListHead);

#if QM_ADAPTIVE_TC_RESOURCE_CTRL
    {
        //UINT_32         i;
        //4 <0> Update TC resource control related variables
        /* Keep track of the queue length */
        if (--prQM->u4TimeToUpdateQueLen == 0) {
            prQM->u4TimeToUpdateQueLen = QM_INIT_TIME_TO_UPDATE_QUE_LEN;
            qmUpdateAverageTxQueLen(prAdapter);
        }
    }
#endif

    /* Push TX packets into STA_REC (for UNICAST) or prAdapter->rQM (for BMCAST) */
    prStaRec = NULL;
    prMsduInfoReleaseList = NULL;
    prCurrentMsduInfo = NULL;
    QUEUE_INITIALIZE(&rNotEnqueuedQue);
    prNextMsduInfo = prMsduInfoListHead;

    do {
        P_BSS_INFO_T prBssInfo;
        BOOLEAN      fgCheckACMAgain;
        ENUM_WMM_ACI_T eAci = WMM_AC_BE_INDEX;
        prCurrentMsduInfo = prNextMsduInfo;
        prNextMsduInfo = QM_TX_GET_NEXT_MSDU_INFO(prCurrentMsduInfo);
        ucTC = TC1_INDEX;

        //4 <1> Lookup the STA_REC index
        /* The ucStaRecIndex will be set in this function */
        qmDetermineStaRecIndex(prAdapter, prCurrentMsduInfo);
        ucPacketType = HIF_TX_PACKET_TYPE_DATA;

        //DBGLOG(TX, INFO, ("***** ucStaRecIndex = %d *****\n",
        //       prCurrentMsduInfo->ucStaRecIndex));

        prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prCurrentMsduInfo->ucNetworkType]);

        if (IS_NET_ACTIVE(prAdapter, prCurrentMsduInfo->ucNetworkType)) {

            switch (prCurrentMsduInfo->ucStaRecIndex) {
                case STA_REC_INDEX_BMCAST:
                    prTxQue = &prQM->arTxQueue[TX_QUEUE_INDEX_BMCAST];
                    ucTC = TC5_INDEX;

                    QM_DBG_CNT_INC(prQM, QM_DBG_CNT_23);
                    break;

                case STA_REC_INDEX_NOT_FOUND:
                    ucTC = TC5_INDEX;

                    if (prCurrentMsduInfo->eSrc == TX_PACKET_FORWARDING) {

                        /* if the packet is the forward type. the packet should be freed */
                        //DBGLOG(TX, WARN, ("Forwarding packet but Sta is STA_REC_INDEX_NOT_FOUND\n"));
                        //prTxQue = &rNotEnqueuedQue;
                        MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_101, "Forwarding packet but Sta is STA_REC_INDEX_NOT_FOUND\n");
                    }
                    prTxQue = &prQM->arTxQueue[TX_QUEUE_INDEX_NO_STA_REC];
                    QM_DBG_CNT_INC(prQM, QM_DBG_CNT_24);

                    break;

                default:
                    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, prCurrentMsduInfo->ucStaRecIndex);

                    ASSERT(prStaRec);
                    ASSERT(prStaRec->fgIsValid);

                    if (prCurrentMsduInfo->ucUserPriority < 8) {
                        QM_DBG_CNT_INC(prQM, prCurrentMsduInfo->ucUserPriority + 15);
                        /* QM_DBG_CNT_15 */ /* QM_DBG_CNT_16 */ /* QM_DBG_CNT_17 */ /* QM_DBG_CNT_18 */
                        /* QM_DBG_CNT_19 */ /* QM_DBG_CNT_20 */ /* QM_DBG_CNT_21 */ /* QM_DBG_CNT_22 */
                    }

                    eAci = WMM_AC_BE_INDEX;
                    do {
                        fgCheckACMAgain = FALSE;
                        if (prStaRec->fgIsQoS) {
                            switch (prCurrentMsduInfo->ucUserPriority) {
                                case 1:
                                case 2:
                                    prTxQue = &prStaRec->arTxQueue[TX_QUEUE_INDEX_AC0];
                                    ucTC = TC0_INDEX;
#if CFG_DEQUEUE_OPT
                                    prAdapter->u4TC_Count++;
#endif
                                    eAci = WMM_AC_BK_INDEX;
                                    break;
                                case 0:
                                case 3:
                                    prTxQue = &prStaRec->arTxQueue[TX_QUEUE_INDEX_AC1];
                                    ucTC = TC1_INDEX;
                                    eAci = WMM_AC_BE_INDEX;
                                    break;
                                case 4:
                                case 5:
                                    prTxQue = &prStaRec->arTxQueue[TX_QUEUE_INDEX_AC2];
                                    ucTC = TC2_INDEX;
#if CFG_DEQUEUE_OPT
                                    prAdapter->u4TC_Count++;
#endif
                                    eAci = WMM_AC_VI_INDEX;
                                    break;
                                case 6:
                                case 7:
                                    prTxQue = &prStaRec->arTxQueue[TX_QUEUE_INDEX_AC3];
                                    ucTC = TC3_INDEX;
#if CFG_DEQUEUE_OPT
                                    prAdapter->u4TC_Count++;
#endif
                                    eAci = WMM_AC_VO_INDEX;
                                    break;
                                default:
                                    prTxQue = &prStaRec->arTxQueue[TX_QUEUE_INDEX_AC1];
                                    ucTC = TC1_INDEX;
                                    eAci = WMM_AC_BE_INDEX;
                                    ASSERT(0);
                                    break;
                            }
                            if (prBssInfo->arACQueParms[eAci].fgIsACMSet && eAci != WMM_AC_BK_INDEX) {
                                prCurrentMsduInfo->ucUserPriority = aucNextUP[eAci];
                                fgCheckACMAgain = TRUE;
                            }
                        } else {
                            prTxQue = &prStaRec->arTxQueue[TX_QUEUE_INDEX_AC1];
                            ucTC = TC1_INDEX;
                        }
                    } while (fgCheckACMAgain);

                    //LOG_FUNC ("QoS %u UP %u TC %u",prStaRec->fgIsQoS,prCurrentMsduInfo->ucUserPriority, ucTC);

                    break;
            } /* switch (prCurrentMsduInfo->ucStaRecIndex) */

            if (prCurrentMsduInfo->eSrc == TX_PACKET_FORWARDING) {
                if (prTxQue->u4NumElem > 32) {
                    //DBGLOG(TX, WARN, ("Drop the Packet for full Tx queue (forwarding) Bss %u\n", prCurrentMsduInfo->ucNetworkType));
                    MT5931_TRACE(TRACE_WARNING, MT5931_INFO_102, "Drop the Packet for full Tx queue (forwarding) Bss %u\n", prCurrentMsduInfo->ucNetworkType);
                    prTxQue = &rNotEnqueuedQue;
                }
            }

        } else {

            //DBGLOG(TX, WARN, ("Drop the Packet for inactive Bss %u\n", prCurrentMsduInfo->ucNetworkType));
            MT5931_TRACE(TRACE_WARNING, MT5931_INFO_103, "Drop the Packet for inactive Bss %u\n", prCurrentMsduInfo->ucNetworkType);
            QM_DBG_CNT_INC(prQM, QM_DBG_CNT_31);
            prTxQue = &rNotEnqueuedQue;
        }

        //4 <3> Fill the MSDU_INFO for constructing HIF TX header

        /* TODO: Fill MSDU_INFO according to the network type,
        *  EtherType, and STA status (for PS forwarding control).
        */

        /* Note that the Network Type Index and STA_REC index are determined in
        *  qmDetermineStaRecIndex(prCurrentMsduInfo).
        */
        QM_TX_SET_MSDU_INFO_FOR_DATA_PACKET(
            prCurrentMsduInfo,                          /* MSDU_INFO ptr */
            ucTC,                                       /* TC tag */
            ucPacketType,                               /* Packet Type */
            0,                                          /* Format ID */
            prCurrentMsduInfo->fgIs802_1x,              /* Flag 802.1x */
            prCurrentMsduInfo->fgIs802_11,              /* Flag 802.11 */
            0,                                          /* PAL LLH */
            0,                                          /* ACL SN */
            PS_FORWARDING_TYPE_NON_PS,                  /* PS Forwarding Type */
            0                                           /* PS Session ID */
        );

        //4 <4> Enqueue the packet
        QUEUE_INSERT_TAIL(prTxQue, (P_QUE_ENTRY_T)prCurrentMsduInfo);


#if QM_TEST_MODE
        if (++prQM->u4PktCount == QM_TEST_TRIGGER_TX_COUNT) {
            prQM->u4PktCount = 0;
            qmTestCases(prAdapter);
        }

#endif

        //DBGLOG(QM, INFO, ("Current queue length = %u\n", prTxQue->u4NumElem));

#if CFG_ENABLE_TXRX_PATH_LOG
        kal_trace(TRACE_GROUP_10 , INFO_QM_queue_length, prTxQue->u4NumElem);
#endif
    } while (prNextMsduInfo);

    if (QUEUE_IS_NOT_EMPTY(&rNotEnqueuedQue)) {
        QM_TX_SET_NEXT_MSDU_INFO((P_MSDU_INFO_T)QUEUE_GET_TAIL(&rNotEnqueuedQue), NULL);
        prMsduInfoReleaseList = (P_MSDU_INFO_T)QUEUE_GET_HEAD(&rNotEnqueuedQue);
#if CFG_ENABLE_TXRX_PATH_LOG
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_104, "rNotEnqueuedQue is not empty %d", rNotEnqueuedQue.u4NumElem);
#endif
    }


    return prMsduInfoReleaseList;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Determine the STA_REC index for a packet
*
* \param[in] prMsduInfo Pointer to the packet
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
qmDetermineStaRecIndex(
    IN P_ADAPTER_T   prAdapter,
    IN P_MSDU_INFO_T prMsduInfo
)
{
    UINT_32 i;

    P_STA_RECORD_T prTempStaRec;
    //P_QUE_MGT_T prQM = &prAdapter->rQM;

    prTempStaRec = NULL;

    ASSERT(prMsduInfo);

    //4 <1> DA = BMCAST
    if (IS_BMCAST_MAC_ADDR(prMsduInfo->aucEthDestAddr)) {
        /* For intrastructure mode and P2P (playing as a GC), BMCAST frames shall be sent to the AP.
        *  FW shall take care of this. The host driver is not able to distinguish these cases. */
        prMsduInfo->ucStaRecIndex = STA_REC_INDEX_BMCAST;
        //DBGLOG(QM, INFO, ("TX with DA = BMCAST\n"));
        //kal_trace(TRACE_GROUP_10 ,INFO_QM_TX_DA);
        return;
    }


    //4 <2> Check if an AP STA is present
    for (i = 0; i < CFG_NUM_OF_STA_RECORD; i++) {
        prTempStaRec = &(prAdapter->arStaRec[i]);

        if ((prTempStaRec->ucNetTypeIndex == prMsduInfo->ucNetworkType)
                && (prTempStaRec->fgIsAp)
                && (prTempStaRec->fgIsValid)) {
            prMsduInfo->ucStaRecIndex = prTempStaRec->ucIndex;
            return;
        }
    }




    //4 <3> Not BMCAST, No AP --> Compare DA (i.e., to see whether this is a unicast frame to a client)
    for (i = 0; i < CFG_NUM_OF_STA_RECORD; i++) {
        prTempStaRec = &(prAdapter->arStaRec[i]);
        if (prTempStaRec->fgIsValid) {
            if (EQUAL_MAC_ADDR(prTempStaRec->aucMacAddr, prMsduInfo->aucEthDestAddr)) {
                prMsduInfo->ucStaRecIndex = prTempStaRec->ucIndex;
                return;
            }
        }
    }


    //4 <4> No STA found, Not BMCAST --> Indicate NOT_FOUND to FW
    prMsduInfo->ucStaRecIndex = STA_REC_INDEX_NOT_FOUND;
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "QM: TX with STA_REC_INDEX_NOT_FOUND\n");


#if (QM_TEST_MODE && QM_TEST_FAIR_FORWARDING)
    prMsduInfo->ucStaRecIndex = (UINT_8)prQM->u4CurrentStaRecIndexToEnqueue;
#endif
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Dequeue TX packets from a STA_REC for a particular TC
*
* \param[out] prQue The queue to put the dequeued packets
* \param[in] ucTC The TC index (TC0_INDEX to TC5_INDEX)
* \param[in] ucMaxNum The maximum amount of dequeued packets
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
qmDequeueTxPacketsFromPerStaQueues(
    IN P_ADAPTER_T   prAdapter,
    OUT P_QUE_T prQue,
    IN  UINT_8  ucTC,
    IN  UINT_8  ucCurrentQuota,
    IN  UINT_8  ucTotalQuota
)
{

#if QM_FORWARDING_FAIRNESS
    UINT_32         i;  /* Loop for */

    PUINT_32        pu4HeadStaRecIndex;         /* The Head STA index */
    PUINT_32        pu4HeadStaRecForwardCount;  /* The total forwarded packets for the head STA */

    P_STA_RECORD_T  prStaRec = NULL;           /* The current focused STA */
    P_BSS_INFO_T    prBssInfo = NULL;          /* The Bss for current focused STA */
    P_QUE_T         prCurrQueue;        /* The current TX queue to dequeue */
    P_MSDU_INFO_T   prDequeuedPkt;      /* The dequeued packet */

    UINT_32         u4ForwardCount;     /* To remember the total forwarded packets for a STA */
    UINT_32         u4MaxForwardCount = 0;  /* The maximum number of packets a STA can forward */
    UINT_32         u4Resource;         /* The TX resource amount */

    BOOLEAN         fgChangeHeadSta;    /* Whether a new head STA shall be determined at the end of the function */
    P_QUE_MGT_T prQM = &prAdapter->rQM;
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    PUINT_8         pucFreeQuota = NULL;
#endif
#if CFG_DEQUEUE_OPT
    UINT_32         u4StaNum;
    BOOLEAN        fgHotSpotMode;
    P_CONNECTION_SETTINGS_T prConnSettings;
#endif


    ASSERT(ucTC == TC0_INDEX || ucTC == TC1_INDEX ||
           ucTC == TC2_INDEX || ucTC == TC3_INDEX ||
           ucTC == TC4_INDEX
          );

    if (!ucCurrentQuota) {
#if CFG_ENABLE_TXRX_PATH_LOG
        kal_trace(TRACE_WARNING , EVENT_TX_TC , ucTC, ucCurrentQuota);
#endif
        return;
    }

    u4Resource = ucCurrentQuota;

    //4 <1> Determine the head STA
    /* The head STA shall be an active STA */

    pu4HeadStaRecIndex = &(prQM->au4HeadStaRecIndex[ucTC]);
    pu4HeadStaRecForwardCount = &(prQM->au4ForwardCount[ucTC]);

    // DBGLOG(QM, LOUD, ("(Fairness) TID = %u Init Head STA = %u Resource = %u\n",
    //     ucTC, *pu4HeadStaRecIndex, u4Resource));


    /* From STA[x] to STA[x+1] to STA[x+2] to ... to STA[x] */
#if CFG_DEQUEUE_OPT
    fgHotSpotMode = prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].fgIsNetActive;
    prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
    u4StaNum = (fgHotSpotMode || (prConnSettings->eOPMode == NET_TYPE_IBSS)) ? CFG_NUM_OF_STA_RECORD : 0;
    for (i = 0; i < u4StaNum + 1; i++)
#else
    for (i = 0; i < CFG_NUM_OF_STA_RECORD + 1; i++)
#endif
    {
        prStaRec = &prAdapter->arStaRec[(*pu4HeadStaRecIndex)];
        ASSERT(prStaRec);

        /* Only Data frame (1x was not included) will be queued in */
        if (prStaRec->fgIsValid) {

            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

            ASSERT(prBssInfo->ucNetTypeIndex == prStaRec->ucNetTypeIndex);

            /* Determine how many packets the head STA is allowed to send in a round */

            QM_DBG_CNT_INC(prQM, QM_DBG_CNT_25);
            u4MaxForwardCount = ucTotalQuota;
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING

            pucFreeQuota = NULL;
            if (prStaRec->fgIsInPS && (ucTC != TC4_INDEX)) {
                // TODO: Change the threshold in coorperation with the PS forwarding mechanism
                // u4MaxForwardCount = ucTotalQuota;
                /* Per STA flow control when STA in PS mode */
                /* The PHASE 1: only update from ucFreeQuota (now) */
                /* XXX The PHASE 2: Decide by ucFreeQuota and ucBmpDeliveryAC (per queue ) aucFreeQuotaPerQueue[] */
                /* NOTE: other method to set u4Resource */

                if (prStaRec->fgIsQoS && prStaRec->fgIsUapsdSupported
                        /*  && prAdapter->rWifiVar.fgSupportQoS
                          && prAdapter->rWifiVar.fgSupportUAPSD*/) {

                    if (prStaRec->ucBmpTriggerAC & BIT(ucTC)) {
                        u4MaxForwardCount = prStaRec->ucFreeQuotaForDelivery;
                        pucFreeQuota = &prStaRec->ucFreeQuotaForDelivery;
                    } else {
                        u4MaxForwardCount = prStaRec->ucFreeQuotaForNonDelivery;
                        pucFreeQuota = &prStaRec->ucFreeQuotaForNonDelivery;
                    }

                } else {
                    ASSERT(prStaRec->ucFreeQuotaForDelivery == 0);
                    u4MaxForwardCount = prStaRec->ucFreeQuotaForNonDelivery;
                    pucFreeQuota = &prStaRec->ucFreeQuotaForNonDelivery;
                }

            } /* fgIsInPS */
#endif /* CFG_ENABLE_WIFI_DIRECT */

#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
            if (prBssInfo->fgIsNetAbsent && (ucTC != TC4_INDEX)) {
                if (u4MaxForwardCount > prBssInfo->ucBssFreeQuota) {
                    u4MaxForwardCount = prBssInfo->ucBssFreeQuota;
                }
            }

#endif /* CFG_ENABLE_WIFI_DIRECT */

            /* Determine whether the head STA can continue to forward packets in this round */
            if ((*pu4HeadStaRecForwardCount) < u4MaxForwardCount) {
                break;
            }

        } /* prStaRec->fgIsValid */
        else {
            /* The current Head STA has been deactivated, so search for a new head STA */
            prStaRec = NULL;
            prBssInfo = NULL;
            (*pu4HeadStaRecIndex) ++;
            (*pu4HeadStaRecIndex) %= CFG_NUM_OF_STA_RECORD;

            /* Reset the forwarding count before searching (since this is for a new selected STA) */
            (*pu4HeadStaRecForwardCount) = 0;
        }
    } /* i < CFG_NUM_OF_STA_RECORD + 1 */

    /* All STA_RECs are inactive, so exit */
    if (!prStaRec) {
        /* Under concurrent, it is possible that there is no candidcated STA.*/
        //DBGLOG(TX, EVENT, ("All STA_RECs are inactive\n"));
        return;
    }

    //kal_trace(TRACE_GROUP_10, INFO_QM_TID_ROUND_HEAD_STA ,ucTC, *pu4HeadStaRecIndex);

    //4 <2> Dequeue packets from the head STA

    prCurrQueue = &prStaRec->arTxQueue[ucTC];
    prDequeuedPkt = NULL;
    fgChangeHeadSta = FALSE;

    while (prCurrQueue) {


#if QM_DEBUG_COUNTER

        if (ucTC <= TC4_INDEX) {
            if (QUEUE_IS_EMPTY(prCurrQueue)) {
                QM_DBG_CNT_INC(prQM, ucTC);
                /* QM_DBG_CNT_00 */ /* QM_DBG_CNT_01 */ /* QM_DBG_CNT_02 */ /* QM_DBG_CNT_03 */ /* QM_DBG_CNT_04 */
            }
            if (u4Resource == 0) {
                QM_DBG_CNT_INC(prQM, ucTC + 5);
                /* QM_DBG_CNT_05 */ /* QM_DBG_CNT_06 */ /* QM_DBG_CNT_07 */ /* QM_DBG_CNT_08 */ /* QM_DBG_CNT_09 */
            }
            if (((*pu4HeadStaRecForwardCount) >= u4MaxForwardCount)) {
                QM_DBG_CNT_INC(prQM, ucTC + 10);
                /* QM_DBG_CNT_10 */ /* QM_DBG_CNT_11 */ /* QM_DBG_CNT_12 */ /* QM_DBG_CNT_13 */ /* QM_DBG_CNT_14 */
            }
        }
#endif


        /* Three cases to break: (1) No resource (2) No packets (3) Fairness */
        if (QUEUE_IS_EMPTY(prCurrQueue) || ((*pu4HeadStaRecForwardCount) >= u4MaxForwardCount)) {
            fgChangeHeadSta = TRUE;
            break;
        } else if (u4Resource == 0) {
            break;
        } else {

            QUEUE_REMOVE_HEAD(prCurrQueue, prDequeuedPkt, P_MSDU_INFO_T);
#if CFG_DEQUEUE_OPT
            if ((prDequeuedPkt->ucTC) != TC1_INDEX) {
                prAdapter->u4TC_Count--;
            }
#endif
#if CFG_ENABLE_TXRX_PATH_LOG

            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_107, "Deq0 TC %d queued %u net %u mac len %u len %u Type %u 1x %u 11 %u\n",
                         prDequeuedPkt->ucTC,
                         prCurrQueue->u4NumElem,
                         prDequeuedPkt->ucNetworkType,
                         prDequeuedPkt->ucMacHeaderLength,
                         prDequeuedPkt->u2FrameLength,
                         prDequeuedPkt->ucPacketType,
                         prDequeuedPkt->fgIs802_1x,
                         prDequeuedPkt->fgIs802_11);

            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_108, "Dest Mac: " MACSTR "\n",
                         MAC2STR(prDequeuedPkt->aucEthDestAddr));

#if LINUX
            {
                struct sk_buff *prSkb = (struct sk_buff *) prDequeuedPkt->prPacket;
                dumpMemory8((PUINT_8)prSkb->data, prSkb->len);
            }
#endif

#endif

            ASSERT(prDequeuedPkt->ucTC == ucTC);

            if (!QUEUE_IS_EMPTY(prCurrQueue)) {
                /* XXX: check all queues for STA */
                prDequeuedPkt->ucPsForwardingType = PS_FORWARDING_MORE_DATA_ENABLED;
            }

            QUEUE_INSERT_TAIL(prQue, (P_QUE_ENTRY_T)prDequeuedPkt);
            u4Resource--;
            (*pu4HeadStaRecForwardCount) ++;


#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
            /* XXX The PHASE 2: decrease from  aucFreeQuotaPerQueue[] */
            if (prStaRec->fgIsInPS && (ucTC != TC4_INDEX)) {
                ASSERT(pucFreeQuota);
                ASSERT(*pucFreeQuota > 0);
                if (*pucFreeQuota > 0) {
                    *pucFreeQuota = *pucFreeQuota - 1;
                }
            }
#endif  /* CFG_ENABLE_WIFI_DIRECT */

#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
            if (prBssInfo->fgIsNetAbsent && (ucTC != TC4_INDEX)) {
                if (prBssInfo->ucBssFreeQuota > 0) {
                    prBssInfo->ucBssFreeQuota--;
                }
            }
#endif  /* CFG_ENABLE_WIFI_DIRECT */

        }
    }

#if CFG_ENABLE_TXRX_PATH_LOG
    if (*pu4HeadStaRecForwardCount) {
        // DBGLOG(TX, INFO, ("TC = %u Round Head STA = %lu, u4HeadStaRecForwardCount = %lu\n", ucTC, *pu4HeadStaRecIndex, (*pu4HeadStaRecForwardCount)));
        kal_trace(TRACE_GROUP_10 , INFO_TX_Round_Head_STA, ucTC, *pu4HeadStaRecIndex, (*pu4HeadStaRecForwardCount));
    }
#endif

#if QM_BURST_END_INFO_ENABLED
    /* Let FW know which packet is the last one dequeued from the STA */
    if (prDequeuedPkt) {
        prDequeuedPkt->fgIsBurstEnd = TRUE;
    }
#endif


    //4 <3> Dequeue from the other STAs if there is residual TX resource

    /* Check all of the STAs to continue forwarding packets (including the head STA) */
    for (i = 0; i < CFG_NUM_OF_STA_RECORD; i++) {
        /* Break in case no reasource is available */
        if (u4Resource == 0) {
            break;
        }
#if CFG_DEQUEUE_OPT
        if (u4StaNum == 0) {
            break;
        }
#endif
        /* The current head STA will be examined when i = CFG_NUM_OF_STA_RECORD-1 */
        prStaRec = &prAdapter->arStaRec[((*pu4HeadStaRecIndex) + i + 1) % CFG_NUM_OF_STA_RECORD];
        ASSERT(prStaRec);

        if (prStaRec->fgIsValid) {

            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);
            ASSERT(prBssInfo->ucNetTypeIndex == prStaRec->ucNetTypeIndex);

            kal_trace(TRACE_GROUP_10 , INFO_QM_TID_SHARING_STA,  ucTC, prStaRec->ucIndex, u4Resource);

            prCurrQueue = &prStaRec->arTxQueue[ucTC];
            u4ForwardCount = 0;
            u4MaxForwardCount = ucTotalQuota;

#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
            pucFreeQuota = NULL;
            if (prStaRec->fgIsInPS && (ucTC != TC4_INDEX)) {
                // TODO: Change the threshold in coorperation with the PS forwarding mechanism
                // u4MaxForwardCount = ucTotalQuota;
                /* Per STA flow control when STA in PS mode */
                /* The PHASE 1: only update from ucFreeQuota (now) */
                /* XXX The PHASE 2: Decide by ucFreeQuota and ucBmpDeliveryAC (per queue ) aucFreeQuotaPerQueue[] */
                /* NOTE: other method to set u4Resource */
                if (prStaRec->fgIsQoS && prStaRec->fgIsUapsdSupported
                        /*  && prAdapter->rWifiVar.fgSupportQoS
                          && prAdapter->rWifiVar.fgSupportUAPSD*/) {

                    if (prStaRec->ucBmpTriggerAC & BIT(ucTC)) {
                        u4MaxForwardCount = prStaRec->ucFreeQuotaForDelivery;
                        pucFreeQuota = &prStaRec->ucFreeQuotaForDelivery;
                    } else {
                        u4MaxForwardCount = prStaRec->ucFreeQuotaForNonDelivery;
                        pucFreeQuota = &prStaRec->ucFreeQuotaForNonDelivery;
                    }

                } else {
                    ASSERT(prStaRec->ucFreeQuotaForDelivery == 0);
                    u4MaxForwardCount = prStaRec->ucFreeQuotaForNonDelivery;
                    pucFreeQuota = &prStaRec->ucFreeQuotaForNonDelivery;
                }

            }
#endif /* CFG_ENABLE_WIFI_DIRECT */
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
            if (prBssInfo->fgIsNetAbsent && (ucTC != TC4_INDEX)) {
                if (u4MaxForwardCount > prBssInfo->ucBssFreeQuota) {
                    u4MaxForwardCount = prBssInfo->ucBssFreeQuota;
                }
            }

#endif /* CFG_ENABLE_WIFI_DIRECT */
        } /* prStaRec->fgIsValid */
        else {
            prBssInfo = NULL;
            /* Invalid STA, so check the next STA */
            continue;
        }

        while (prCurrQueue) {
            /* Three cases to break: (1) No resource (2) No packets (3) Fairness */
            if ((u4Resource == 0) || QUEUE_IS_EMPTY(prCurrQueue) || (u4ForwardCount >= u4MaxForwardCount)) {
                break;
            } else {

                QUEUE_REMOVE_HEAD(prCurrQueue, prDequeuedPkt, P_MSDU_INFO_T);

#if CFG_ENABLE_TXRX_PATH_LOG
                LOG_FUNC("Deq0 TC %d queued %u net %u mac len %u len %u Type %u 1x %u 11 %u\n",
                         prDequeuedPkt->ucTC,
                         prCurrQueue->u4NumElem,
                         prDequeuedPkt->ucNetworkType,
                         prDequeuedPkt->ucMacHeaderLength,
                         prDequeuedPkt->u2FrameLength,
                         prDequeuedPkt->ucPacketType,
                         prDequeuedPkt->fgIs802_1x,
                         prDequeuedPkt->fgIs802_11);

                LOG_FUNC("Dest Mac: " MACSTR "\n",
                         MAC2STR(prDequeuedPkt->aucEthDestAddr));

#if LINUX
                {
                    struct sk_buff *prSkb = (struct sk_buff *) prDequeuedPkt->prPacket;
                    dumpMemory8((PUINT_8)prSkb->data, prSkb->len);
                }
#endif

#endif


                ASSERT(prDequeuedPkt->ucTC == ucTC);

                if (!QUEUE_IS_EMPTY(prCurrQueue)) {
                    prDequeuedPkt->ucPsForwardingType = PS_FORWARDING_MORE_DATA_ENABLED;
                }

                QUEUE_INSERT_TAIL(prQue, (P_QUE_ENTRY_T)prDequeuedPkt);
                u4Resource--;
                u4ForwardCount ++;

#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
                /* XXX The PHASE 2: decrease from  aucFreeQuotaPerQueue[] */
                if (prStaRec->fgIsInPS && (ucTC != TC4_INDEX)) {
                    ASSERT(pucFreeQuota);
                    ASSERT(*pucFreeQuota > 0);
                    if (*pucFreeQuota > 0) {
                        *pucFreeQuota = *pucFreeQuota - 1;
                    }
                }
#endif  /* CFG_ENABLE_WIFI_DIRECT */


#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
                ASSERT(prBssInfo->ucNetTypeIndex == prStaRec->ucNetTypeIndex);
                if (prBssInfo->fgIsNetAbsent && (ucTC != TC4_INDEX)) {
                    if (prBssInfo->ucBssFreeQuota > 0) {
                        prBssInfo->ucBssFreeQuota--;
                    }
                }
#endif  /* CFG_ENABLE_WIFI_DIRECT */

            }
        }

#if QM_BURST_END_INFO_ENABLED
        /* Let FW know which packet is the last one dequeued from the STA */
        if (u4ForwardCount) {
            prDequeuedPkt->fgIsBurstEnd = TRUE;
        }
#endif
    }

#if CFG_DEQUEUE_OPT
    if (u4StaNum == 0) {
        (*pu4HeadStaRecForwardCount) = 0;
    } else
#endif
        if (fgChangeHeadSta) {
            (*pu4HeadStaRecIndex) ++;
            (*pu4HeadStaRecIndex) %= CFG_NUM_OF_STA_RECORD;
            (*pu4HeadStaRecForwardCount) = 0;

            kal_trace(TRACE_GROUP_10 , INFO_QM_TID_SCHEDULED_STA, ucTC, (*pu4HeadStaRecIndex),  u4Resource);
        }


    /***************************************************************************************/
#else
    UINT_8          ucStaRecIndex;
    P_STA_RECORD_T  prStaRec;
    P_QUE_T         prCurrQueue;
    UINT_8          ucPktCount;
    P_MSDU_INFO_T   prDequeuedPkt;

    //DBGLOG(QM, LOUD, ("Enter qmDequeueTxPacketsFromPerStaQueues (TC = %u)\n", ucTC));
    kal_trace(TRACE_GROUP_10 , LOUD_QM_qmDequeueTxPacketsFromPerStaQueues, ucTC);

    if (ucCurrentQuota == 0) {
        return;
    }

    //4 <1> Determine the queue index and the head STA

    /* The head STA */
    ucStaRecIndex = 0;  /* TODO: Get the current head STA */
    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, ucStaRecIndex);
    ASSERT(prStaRec);

    if (prStaRec == NULL) {
        return;
    }

    /* The queue to pull out packets */
    ASSERT(ucTC == TC0_INDEX || ucTC == TC1_INDEX ||
           ucTC == TC2_INDEX || ucTC == TC3_INDEX ||
           ucTC == TC4_INDEX
          );
    prCurrQueue = &prStaRec->arTxQueue[ucTC];

    ucPktCount = ucCurrentQuota;
    prDequeuedPkt = NULL;

    //4 <2> Dequeue packets for the head STA
    while (TRUE) {
        if (!(prStaRec->fgIsValid) || ucPktCount == 0 || QUEUE_IS_EMPTY(prCurrQueue)) {
            break;

        } else {

            QUEUE_REMOVE_HEAD(prCurrQueue, prDequeuedPkt, P_MSDU_INFO_T);
            //DbgPrint("QM: Remove Queue Head, TC= %d\n", prDequeuedPkt->ucTC);
            ASSERT(prDequeuedPkt->ucTC == ucTC);

            QUEUE_INSERT_TAIL(prQue, (P_QUE_ENTRY_T)prDequeuedPkt);
            ucPktCount--;
        }
    }

    //DbgPrint("QM: Remaining number of queued packets = %d\n", prCurrQueue->u4NumElem);

#if QM_BURST_END_INFO_ENABLED
    if (prDequeuedPkt) {
        prDequeuedPkt->fgIsBurstEnd = TRUE;
    }

#endif

    //4 <3> Update scheduling info
    /* TODO */

    //4 <4> Utilize the remainaing TX opportunities for non-head STAs
    /* TODO */
#endif
}

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code, rwdata,  zidata
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Dequeue TX packets from a per-Type-based Queue for a particular TC
*
* \param[out] prQue The queue to put the dequeued packets
* \param[in] ucTC The TC index (Shall always be TC5_INDEX)
* \param[in] ucMaxNum The maximum amount of dequeued packets
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
qmDequeueTxPacketsFromPerTypeQueues(
    IN P_ADAPTER_T   prAdapter,
    OUT P_QUE_T prQue,
    IN  UINT_8  ucTC,
    IN  UINT_8  ucMaxNum
)
{
    //UINT_8          ucQueIndex;
    //UINT_8          ucStaRecIndex;
    P_BSS_INFO_T prBssInfo;
    P_BSS_INFO_T parBssInfo;
    P_QUE_T         prCurrQueue;
    UINT_8          ucPktCount;
    P_MSDU_INFO_T   prDequeuedPkt;
    P_MSDU_INFO_T   prBurstEndPkt;
    QUE_T           rMergeQue;
    P_QUE_T         prMergeQue;
    P_QUE_MGT_T     prQM;

    //DBGLOG(QM, INFO, ("Enter qmDequeueTxPacketsFromPerTypeQueues (TC = %d, Max = %d)\n", ucTC, ucMaxNum));

#if CFG_ENABLE_TXRX_PATH_LOG
    kal_trace(TRACE_GROUP_10 , INFO_QM_qmDequeueTxPacketsFromPerTypeQueues, ucTC, ucMaxNum);
#endif
    /* TC5: Broadcast/Multicast data packets */
    ASSERT(ucTC == TC5_INDEX);

    if (ucMaxNum == 0) {
        return;
    }

    prQM = &prAdapter->rQM;
    //4 <1> Determine the queue

    prCurrQueue = &prQM->arTxQueue[TX_QUEUE_INDEX_BMCAST];
    ucPktCount = ucMaxNum;
    prDequeuedPkt = NULL;
    prBurstEndPkt = NULL;

    parBssInfo = prAdapter->rWifiVar.arBssInfo;

    QUEUE_INITIALIZE(&rMergeQue);
    prMergeQue = &rMergeQue;

    //4 <2> Dequeue packets
    while (TRUE) {
        if (ucPktCount == 0 || QUEUE_IS_EMPTY(prCurrQueue)) {
            break;
        } else {
            QUEUE_REMOVE_HEAD(prCurrQueue, prDequeuedPkt, P_MSDU_INFO_T);
            ASSERT(prDequeuedPkt->ucTC == ucTC);

            ASSERT(prDequeuedPkt->ucNetworkType < NETWORK_TYPE_INDEX_NUM);

            prBssInfo = &parBssInfo[prDequeuedPkt->ucNetworkType];

            if (IS_BSS_ACTIVE(prBssInfo)) {
                if (!prBssInfo->fgIsNetAbsent) {
                    QUEUE_INSERT_TAIL(prQue, (P_QUE_ENTRY_T)prDequeuedPkt);
                    prBurstEndPkt = prDequeuedPkt;
                    ucPktCount--;
                    QM_DBG_CNT_INC(prQM, QM_DBG_CNT_26);
#if DBG && 0
                    LOG_FUNC("DeqType TC %d queued %u net %u mac len %u len %u Type %u 1x %u 11 %u\n",
                             prDequeuedPkt->ucTC,
                             prCurrQueue->u4NumElem,
                             prDequeuedPkt->ucNetworkType,
                             prDequeuedPkt->ucMacHeaderLength,
                             prDequeuedPkt->u2FrameLength,
                             prDequeuedPkt->ucPacketType,
                             prDequeuedPkt->fgIs802_1x,
                             prDequeuedPkt->fgIs802_11);

                    LOG_FUNC("Dest Mac: " MACSTR "\n",
                             MAC2STR(prDequeuedPkt->aucEthDestAddr));

#if LINUX
                    {
                        struct sk_buff *prSkb = (struct sk_buff *) prDequeuedPkt->prPacket;
                        dumpMemory8((PUINT_8)prSkb->data, prSkb->len);
                    }
#endif

#endif
                } else {
                    QUEUE_INSERT_TAIL(prMergeQue, (P_QUE_ENTRY_T)prDequeuedPkt);
                }
            } else {
                QM_TX_SET_NEXT_MSDU_INFO(prDequeuedPkt, NULL);
                wlanProcessQueuedMsduInfo(prAdapter, prDequeuedPkt);
            }
        }
    }

    if (QUEUE_IS_NOT_EMPTY(prMergeQue)) {
        QUEUE_CONCATENATE_QUEUES(prMergeQue, prCurrQueue);
        QUEUE_MOVE_ALL(prCurrQueue, prMergeQue);
        QM_TX_SET_NEXT_MSDU_INFO((P_MSDU_INFO_T)QUEUE_GET_TAIL(prCurrQueue), NULL);
    }

#if QM_BURST_END_INFO_ENABLED
    if (prBurstEndPkt) {
        prBurstEndPkt->fgIsBurstEnd = TRUE;
    }
#endif
} /* qmDequeueTxPacketsFromPerTypeQueues */




/*----------------------------------------------------------------------------*/
/*!
* \brief Dequeue TX packets to send to HIF TX
*
* \param[in] prTcqStatus Info about the maximum amount of dequeued packets
*
* \return The list of dequeued TX packets
*/
/*----------------------------------------------------------------------------*/
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code = "WIFISpeedUp", rwdata = "WIFISpeedUp",  zidata = "WIFISpeedUp"
#endif

P_MSDU_INFO_T
qmDequeueTxPackets(
    IN P_ADAPTER_T   prAdapter,
    IN P_TX_TCQ_STATUS_T prTcqStatus
)
{

    INT_32 i;
    P_MSDU_INFO_T prReturnedPacketListHead;
    QUE_T rReturnedQue;
#if CFG_DEQUEUE_OPT
    BOOLEAN        fgHotSpotMode;
#endif



    QUEUE_INITIALIZE(&rReturnedQue);

    prReturnedPacketListHead = NULL;

    /* TC0 to TC4: AC0~AC3, 802.1x (commands packets are not handled by QM) */
#if CFG_DEQUEUE_OPT
    fgHotSpotMode = prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].fgIsNetActive;
    if ((!prAdapter->u4TC_Count) && (!fgHotSpotMode)) {
        i = TC1_INDEX;
        //DBGLOG(QM, LOUD, ("Dequeue packets from Per-STA queue[%u]\n", i));
        qmDequeueTxPacketsFromPerStaQueues(
            prAdapter,
            &rReturnedQue,
            (UINT_8)i,
            prTcqStatus->aucFreeBufferCount[i],
            prTcqStatus->aucMaxNumOfBuffer[i]
        );

        /* The aggregate number of dequeued packets */
#if CFG_ENABLE_TXRX_PATH_LOG
        kal_trace(TRACE_GROUP_10 , LOUD_QM_DQA,  i, rReturnedQue.u4NumElem);
#endif
    } else
#endif
    {
        for (i = TC4_INDEX; i >= TC0_INDEX; i--) {
            //DBGLOG(QM, LOUD, ("Dequeue packets from Per-STA queue[%u]\n", i));
            kal_trace(TRACE_GROUP_10 , LOUD_QM_Dequeue_packetss, i);

            qmDequeueTxPacketsFromPerStaQueues(
                prAdapter,
                &rReturnedQue,
                (UINT_8)i,
                prTcqStatus->aucFreeBufferCount[i],
                prTcqStatus->aucMaxNumOfBuffer[i]
            );

            /* The aggregate number of dequeued packets */

#if CFG_ENABLE_TXRX_PATH_LOG
            kal_trace(TRACE_GROUP_10 , LOUD_QM_DQA,  i, rReturnedQue.u4NumElem);
#endif
        }
    }

    /* TC5 (BMCAST or STA-NOT-FOUND packets) */
    qmDequeueTxPacketsFromPerTypeQueues(
        prAdapter,
        &rReturnedQue,
        TC5_INDEX,
        prTcqStatus->aucFreeBufferCount[TC5_INDEX]
    );

    //DBGLOG(QM, LOUD, ("Current total number of dequeued packets = %u\n", rReturnedQue.u4NumElem));

#if CFG_ENABLE_TXRX_PATH_LOG
    kal_trace(TRACE_GROUP_10 , LOUD_QM_num_dequeued_packets,  rReturnedQue.u4NumElem);
#endif
    if (QUEUE_IS_NOT_EMPTY(&rReturnedQue)) {
        prReturnedPacketListHead = (P_MSDU_INFO_T)QUEUE_GET_HEAD(&rReturnedQue);
        QM_TX_SET_NEXT_MSDU_INFO((P_MSDU_INFO_T)QUEUE_GET_TAIL(&rReturnedQue), NULL);
    }

#if CFG_ENABLE_TXRX_PATH_LOG
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_109, "DequeueTx: MSDU %x", prReturnedPacketListHead);
#endif
    return prReturnedPacketListHead;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Adjust the TC quotas according to traffic demands
*
* \param[out] prTcqAdjust The resulting adjustment
* \param[in] prTcqStatus Info about the current TC quotas and counters
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmAdjustTcQuotas(
    IN P_ADAPTER_T  prAdapter,
    OUT P_TX_TCQ_ADJUST_T prTcqAdjust,
    IN P_TX_TCQ_STATUS_T prTcqStatus
)
{
#if QM_ADAPTIVE_TC_RESOURCE_CTRL
    UINT_32 i;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    /* Must initialize */
    for (i = 0; i < TC_NUM; i++) {
        prTcqAdjust->acVariation[i] = 0;
    }

    //4 <1> If TC resource is not just adjusted, exit directly
    if (!prQM->fgTcResourcePostAnnealing) {
        return;
    }

    //4 <2> Adjust TcqStatus according to the updated prQM->au4CurrentTcResource
    else {
        INT_32 i4TotalExtraQuota = 0;
        INT_32 ai4ExtraQuota[TC_NUM];
        BOOLEAN fgResourceRedistributed = TRUE;

        /* Obtain the free-to-distribute resource */
        for (i = 0; i < TC_NUM; i++) {
            ai4ExtraQuota[i] = (INT_32)prTcqStatus->aucMaxNumOfBuffer[i] - (INT_32)prQM->au4CurrentTcResource[i];

            if (ai4ExtraQuota[i] > 0) { /* The resource shall be reallocated to other TCs */
                if (ai4ExtraQuota[i] > prTcqStatus->aucFreeBufferCount[i]) {
                    ai4ExtraQuota[i] = prTcqStatus->aucFreeBufferCount[i];
                    fgResourceRedistributed = FALSE;
                }

                i4TotalExtraQuota += ai4ExtraQuota[i];
                prTcqAdjust->acVariation[i] = (INT_8)(-ai4ExtraQuota[i]);
            }
        }

        /* Distribute quotas to TCs which need extra resource according to prQM->au4CurrentTcResource */
        for (i = 0; i < TC_NUM; i++) {
            if (ai4ExtraQuota[i] < 0) {
                if ((-ai4ExtraQuota[i]) > i4TotalExtraQuota) {
                    ai4ExtraQuota[i] = (-i4TotalExtraQuota);
                    fgResourceRedistributed = FALSE;
                }

                i4TotalExtraQuota += ai4ExtraQuota[i];
                prTcqAdjust->acVariation[i] = (INT_8)(-ai4ExtraQuota[i]);
            }
        }

        /* In case some TC is waiting for TX Done, continue to adjust TC quotas upon TX Done */
        prQM->fgTcResourcePostAnnealing = (!fgResourceRedistributed);

#if QM_PRINT_TC_RESOURCE_CTRL
        DbgPrint("QM: Curr Quota [0]=%u [1]=%u [2]=%u [3]=%u [4]=%u [5]=%u\n",
                 prTcqStatus->aucFreeBufferCount[0],
                 prTcqStatus->aucFreeBufferCount[1],
                 prTcqStatus->aucFreeBufferCount[2],
                 prTcqStatus->aucFreeBufferCount[3],
                 prTcqStatus->aucFreeBufferCount[4],
                 prTcqStatus->aucFreeBufferCount[5]
                );
#endif
    }

#else
    UINT_32 i;
    for (i = 0; i < TC_NUM; i++) {
        prTcqAdjust->acVariation[i] = 0;
    }

#endif
}

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code, rwdata,  zidata
#endif

#if QM_ADAPTIVE_TC_RESOURCE_CTRL
/*----------------------------------------------------------------------------*/
/*!
* \brief Update the average TX queue length for the TC resource control mechanism
*
* \param (none)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmUpdateAverageTxQueLen(
    IN P_ADAPTER_T   prAdapter
)
{
    INT_32 u4CurrQueLen, i, k;
    P_STA_RECORD_T prStaRec;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    //4 <1> Update the queue lengths for TC0 to TC3 (skip TC4) and TC5 */
    for (i = 0; i < NUM_OF_PER_STA_TX_QUEUES - 1; i++) {
        u4CurrQueLen = 0;

        for (k = 0; k < CFG_NUM_OF_STA_RECORD; k++) {
            prStaRec = &prAdapter->arStaRec[k];
            ASSERT(prStaRec);

            /* If the STA is activated, get the queue length */
            if (prStaRec->fgIsValid &&
                    (!prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex].fgIsNetAbsent)
               ) {

                u4CurrQueLen += (prStaRec->arTxQueue[i].u4NumElem);
            }
        }

        if (prQM->au4AverageQueLen[i] == 0) {
            prQM->au4AverageQueLen[i] = (u4CurrQueLen << QM_QUE_LEN_MOVING_AVE_FACTOR);
        } else {
            prQM->au4AverageQueLen[i] -= (prQM->au4AverageQueLen[i] >> QM_QUE_LEN_MOVING_AVE_FACTOR);
            prQM->au4AverageQueLen[i] += (u4CurrQueLen);
        }

    }

    /* Update the queue length for TC5 (BMCAST) */
    u4CurrQueLen = prQM->arTxQueue[TX_QUEUE_INDEX_BMCAST].u4NumElem;

    if (prQM->au4AverageQueLen[TC_NUM - 1] == 0) {
        prQM->au4AverageQueLen[TC_NUM - 1] = (u4CurrQueLen << QM_QUE_LEN_MOVING_AVE_FACTOR);
    } else {
        prQM->au4AverageQueLen[TC_NUM - 1] -= (prQM->au4AverageQueLen[TC_NUM - 1] >> QM_QUE_LEN_MOVING_AVE_FACTOR);
        prQM->au4AverageQueLen[TC_NUM - 1] += (u4CurrQueLen);
    }


    //4 <2> Adjust TC resource assignment
    /* Check whether it is time to adjust the TC resource assignment */
    if (--prQM->u4TimeToAdjustTcResource == 0) {
        /* The last assignment has not been completely applied */
        if (prQM->fgTcResourcePostAnnealing) {
            /* Upon the next qmUpdateAverageTxQueLen function call, do this check again */
            prQM->u4TimeToAdjustTcResource = 1;
        }

        /* The last assignment has been applied */
        else {
            prQM->u4TimeToAdjustTcResource = QM_INIT_TIME_TO_ADJUST_TC_RSC;
            qmReassignTcResource(prAdapter);
        }
    }

    /* Debug */
#if QM_PRINT_TC_RESOURCE_CTRL
    for (i = 0; i < TC_NUM; i++) {
        if (QM_GET_TX_QUEUE_LEN(prAdapter, i) >= 100) {
            DbgPrint("QM: QueLen [%ld %ld %ld %ld %ld %ld]\n",
                     QM_GET_TX_QUEUE_LEN(prAdapter, 0),
                     QM_GET_TX_QUEUE_LEN(prAdapter, 1),
                     QM_GET_TX_QUEUE_LEN(prAdapter, 2),
                     QM_GET_TX_QUEUE_LEN(prAdapter, 3),
                     QM_GET_TX_QUEUE_LEN(prAdapter, 4),
                     QM_GET_TX_QUEUE_LEN(prAdapter, 5)
                    );
            break;
        }
    }
#endif

}



/*----------------------------------------------------------------------------*/
/*!
* \brief Assign TX resource for each TC according to TX queue length and current assignment
*
* \param (none)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmReassignTcResource(
    IN P_ADAPTER_T   prAdapter
)
{
    INT_32 i4TotalResourceDemand = 0;
    UINT_32 u4ResidualResource = 0;
    UINT_32 i;
    INT_32 ai4PerTcResourceDemand[TC_NUM];
    UINT_32 u4ShareCount = 0;
    UINT_32 u4Share = 0 ;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    /* Note: After the new assignment is obtained, set prQM->fgTcResourcePostAnnealing to TRUE to
    *  start the TC-quota adjusting procedure, which will be invoked upon every TX Done
    */

    //4 <1> Determine the demands
    /* Determine the amount of extra resource to fulfill all of the demands */
    for (i = 0; i < TC_NUM; i++) {
        /* Skip TC4, which is not adjustable */
        if (i == TC4_INDEX) {
            continue;
        }

        /* Define: extra_demand = que_length + min_reserved_quota - current_quota */
        ai4PerTcResourceDemand[i] =
            ((UINT_32)(QM_GET_TX_QUEUE_LEN(prAdapter, i)) + prQM->au4MinReservedTcResource[i] - prQM->au4CurrentTcResource[i]);

        /* If there are queued packets, allocate extra resource for the TC (for TCP consideration) */
        if (QM_GET_TX_QUEUE_LEN(prAdapter, i)) {
            ai4PerTcResourceDemand[i] += QM_EXTRA_RESERVED_RESOURCE_WHEN_BUSY;
        }

        i4TotalResourceDemand += ai4PerTcResourceDemand[i];
    }

    //4 <2> Case 1: Demand <= Total Resource
    if (i4TotalResourceDemand <= 0) {
        //4 <2.1> Satisfy every TC
        for (i = 0; i < TC_NUM; i++) {
            /* Skip TC4 (not adjustable) */
            if (i == TC4_INDEX) {
                continue;
            }

            prQM->au4CurrentTcResource[i] += ai4PerTcResourceDemand[i];
        }

        //4 <2.2> Share the residual resource evenly
        u4ShareCount = (TC_NUM - 1); /* excluding TC4 */
        u4ResidualResource = (UINT_32)(-i4TotalResourceDemand);
        u4Share = (u4ResidualResource / u4ShareCount);

        for (i = 0; i < TC_NUM; i++) {
            /* Skip TC4 (not adjustable) */
            if (i == TC4_INDEX) {
                continue;
            }

            prQM->au4CurrentTcResource[i] += u4Share;

            /* Every TC is fully satisfied */
            ai4PerTcResourceDemand[i] = 0;

            /* The left resource will be allocated to TC3 */
            u4ResidualResource -= u4Share;
        }

        //4 <2.3> Allocate the left resource to TC3 (VO)
        prQM->au4CurrentTcResource[TC3_INDEX] += (u4ResidualResource);

    }

    //4 <3> Case 2: Demand > Total Resource --> Guarantee a minimum amount of resource for each TC
    else {
        u4ResidualResource = QM_INITIAL_RESIDUAL_TC_RESOURCE;

        //4 <3.1> Allocated resouce amount  = minimum of (guaranteed, total demand)
        for (i = 0; i < TC_NUM; i++) {
            /* Skip TC4 (not adjustable) */
            if (i == TC4_INDEX) {
                continue;
            }

            /* The demand can be fulfilled with the guaranteed resource amount */
            if (prQM->au4CurrentTcResource[i] + ai4PerTcResourceDemand[i] < prQM->au4GuaranteedTcResource[i]) {
                prQM->au4CurrentTcResource[i] += ai4PerTcResourceDemand[i];
                u4ResidualResource += (prQM->au4GuaranteedTcResource[i] - prQM->au4CurrentTcResource[i]);
                ai4PerTcResourceDemand[i] = 0;
            }

            /* The demand can not be fulfilled with the guaranteed resource amount */
            else {
                ai4PerTcResourceDemand[i] -= (prQM->au4GuaranteedTcResource[i] - prQM->au4CurrentTcResource[i]);
                prQM->au4CurrentTcResource[i] = prQM->au4GuaranteedTcResource[i];
                u4ShareCount++;
            }
        }

        //4 <3.2> Allocate the residual resource
        do {
            /* If there is no resource left, exit directly */
            if (u4ResidualResource == 0) {
                break;
            }

            /* This shall not happen */
            if (u4ShareCount == 0) {
                prQM->au4CurrentTcResource[TC1_INDEX] += u4ResidualResource;
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "QM: (Error) u4ShareCount = 0\n");
                break;
            }

            /* Share the residual resource evenly */
            u4Share = (u4ResidualResource / u4ShareCount);
            if (u4Share) {
                for (i = 0; i < TC_NUM; i++) {
                    /* Skip TC4 (not adjustable) */
                    if (i == TC4_INDEX) {
                        continue;
                    }

                    if (ai4PerTcResourceDemand[i]) {
                        if (ai4PerTcResourceDemand[i] - u4Share) {
                            prQM->au4CurrentTcResource[i] += u4Share;
                            u4ResidualResource -= u4Share;
                            ai4PerTcResourceDemand[i] -= u4Share;
                        } else {
                            prQM->au4CurrentTcResource[i] += ai4PerTcResourceDemand[i];
                            u4ResidualResource -= ai4PerTcResourceDemand[i];
                            ai4PerTcResourceDemand[i] = 0;
                        }
                    }
                }
            }

            /* By priority, allocate the left resource that is not divisible by u4Share */
            if (u4ResidualResource == 0) {
                break;
            }

            if (ai4PerTcResourceDemand[TC3_INDEX]) {     /* VO */
                prQM->au4CurrentTcResource[TC3_INDEX]++;
                if (--u4ResidualResource == 0) {
                    break;
                }
            }

            if (ai4PerTcResourceDemand[TC2_INDEX]) {     /* VI */
                prQM->au4CurrentTcResource[TC2_INDEX]++;
                if (--u4ResidualResource == 0) {
                    break;
                }
            }

            if (ai4PerTcResourceDemand[TC5_INDEX]) {     /* BMCAST */
                prQM->au4CurrentTcResource[TC5_INDEX]++;
                if (--u4ResidualResource == 0) {
                    break;
                }
            }

            if (ai4PerTcResourceDemand[TC1_INDEX]) {     /* BE */
                prQM->au4CurrentTcResource[TC1_INDEX]++;
                if (--u4ResidualResource == 0) {
                    break;
                }
            }

            if (ai4PerTcResourceDemand[TC0_INDEX]) {     /* BK */
                prQM->au4CurrentTcResource[TC0_INDEX]++;
                if (--u4ResidualResource == 0) {
                    break;
                }
            }

            /* Allocate the left resource */
            prQM->au4CurrentTcResource[TC3_INDEX] += u4ResidualResource;

        } while (FALSE);
    }

    prQM->fgTcResourcePostAnnealing = TRUE;

#if QM_PRINT_TC_RESOURCE_CTRL
    /* Debug print */
    DbgPrint("QM: TC Rsc %ld %ld %ld %ld %ld %ld\n",
             prQM->au4CurrentTcResource[0],
             prQM->au4CurrentTcResource[1],
             prQM->au4CurrentTcResource[2],
             prQM->au4CurrentTcResource[3],
             prQM->au4CurrentTcResource[4],
             prQM->au4CurrentTcResource[5]
            );
#endif

}

#endif


/*----------------------------------------------------------------------------*/
/* RX-Related Queue Management                                                */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*!
* \brief Init Queue Managment for RX
*
* \param[in] (none)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmInitRxQueues(
    IN P_ADAPTER_T   prAdapter
)
{
    //DbgPrint("QM: Enter qmInitRxQueues()\n");
    /* TODO */
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Handle RX packets (buffer reordering)
*
* \param[in] prSwRfbListHead The list of RX packets
*
* \return The list of packets which are not buffered for reordering
*/
/*----------------------------------------------------------------------------*/
P_SW_RFB_T
qmHandleRxPackets(
    IN P_ADAPTER_T   prAdapter,
    IN P_SW_RFB_T prSwRfbListHead
)
{

#if CFG_RX_REORDERING_ENABLED
    //UINT_32 i;
    P_SW_RFB_T          prCurrSwRfb;
    P_SW_RFB_T          prNextSwRfb;
    P_HIF_RX_HEADER_T   prHifRxHdr;
    QUE_T               rReturnedQue;
    PUINT_8             pucEthDestAddr;

    //DbgPrint("QM: Enter qmHandleRxPackets()\n");


    ASSERT(prSwRfbListHead);

    QUEUE_INITIALIZE(&rReturnedQue);
    prNextSwRfb = prSwRfbListHead;

    do {
        prCurrSwRfb = prNextSwRfb;
        prNextSwRfb = QM_RX_GET_NEXT_SW_RFB(prCurrSwRfb);

        prHifRxHdr = prCurrSwRfb->prHifRxHdr; // TODO: (Tehuang) Use macro to obtain the pointer

        /* TODO: (Tehuang) Check if relaying */
        prCurrSwRfb->eDst = RX_PKT_DESTINATION_HOST;

        /* Decide the Destination */
#if DBG && 0
        if (prAdapter->rRxCtrl.u4RxPktsDumpTypeMask & BIT(HIF_RX_PKT_TYPE_DATA)) {
            LOG_FUNC("QM RX DATA: net %u sta idx %u wlan idx %u ssn %u tid %u ptype %u 11 %u\n",
                     HIF_RX_HDR_GET_NETWORK_IDX(prHifRxHdr),
                     prHifRxHdr->ucStaRecIdx,
                     prCurrSwRfb->ucWlanIdx,
                     HIF_RX_HDR_GET_SN(prHifRxHdr),  /* The new SN of the frame */
                     HIF_RX_HDR_GET_TID(prHifRxHdr),
                     prCurrSwRfb->ucPacketType,
                     HIF_RX_HDR_GET_80211_FLAG(prHifRxHdr));

            dumpMemory8((PUINT_8)prCurrSwRfb->pvHeader, prCurrSwRfb->u2PacketLen);
        }
#endif

        if (!HIF_RX_HDR_GET_80211_FLAG(prHifRxHdr)) {

            UINT_8 ucNetTypeIdx;
            P_BSS_INFO_T prBssInfo;

            pucEthDestAddr = prCurrSwRfb->pvHeader;
            ucNetTypeIdx = HIF_RX_HDR_GET_NETWORK_IDX(prHifRxHdr);

            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[ucNetTypeIdx]);
            //DBGLOG_MEM8(QM, TRACE,prCurrSwRfb->pvHeader, 16);
            //

            if (prAdapter->rRxCtrl.rFreeSwRfbList.u4NumElem
                    > (CFG_RX_MAX_PKT_NUM - CFG_NUM_OF_QM_RX_PKT_NUM)) {

                if (IS_BSS_ACTIVE(prBssInfo)) {
                    if (OP_MODE_ACCESS_POINT == prBssInfo->eCurrentOPMode) {
                        if (IS_BMCAST_MAC_ADDR(pucEthDestAddr)) {
                            prCurrSwRfb->eDst = RX_PKT_DESTINATION_HOST_WITH_FORWARD;
                        } else if (UNEQUAL_MAC_ADDR(prBssInfo->aucOwnMacAddr, pucEthDestAddr)) {
                            prCurrSwRfb->eDst = RX_PKT_DESTINATION_FORWARD;
                            /* TODO : need to check the dst mac is valid */
                            /* If src mac is invalid, the packet will be freed in fw */
                        }
                    } /* OP_MODE_ACCESS_POINT */
                } else {
                    // DBGLOG(QM, WARN, ("Mark NULL the Packet for inactive Bss %u\n",ucNetTypeIdx));
                    kal_trace(TRACE_WARNING , WARN_QM_MARK_NULL_BSS,  ucNetTypeIdx);
                    prCurrSwRfb->eDst = RX_PKT_DESTINATION_NULL;
                    QUEUE_INSERT_TAIL(&rReturnedQue, (P_QUE_ENTRY_T)prCurrSwRfb);
                    continue;
                }

            } else {
                /* Dont not occupy other SW RFB */
                // DBGLOG(QM, WARN, ("Mark NULL the Packet for less Free Sw Rfb\n"));
                kal_trace(TRACE_WARNING , WARN_QM_MARK_NULL_RFB);
                prCurrSwRfb->eDst = RX_PKT_DESTINATION_NULL;
                QUEUE_INSERT_TAIL(&rReturnedQue, (P_QUE_ENTRY_T)prCurrSwRfb);
                continue;
            }

        }

        /* BAR frame */
        if (HIF_RX_HDR_GET_BAR_FLAG(prHifRxHdr)) {
            prCurrSwRfb->eDst = RX_PKT_DESTINATION_NULL;
            qmProcessBarFrame(prAdapter, prCurrSwRfb, &rReturnedQue);
        }
        /* Reordering is not required for this packet, return it without buffering */
        else if (!HIF_RX_HDR_GET_REORDER_FLAG(prHifRxHdr)) {
#if 0
            if (!HIF_RX_HDR_GET_80211_FLAG(prHifRxHdr)) {
                UINT_8 ucNetTypeIdx;
                P_BSS_INFO_T prBssInfo;

                pucEthDestAddr = prCurrSwRfb->pvHeader;
                ucNetTypeIdx = HIF_RX_HDR_GET_NETWORK_IDX(prHifRxHdr);

                prBssInfo = &(prAdapter->rWifiVar.arBssInfo[ucNetTypeIdx]);

                if (IS_BMCAST_MAC_ADDR(pucEthDestAddr) && (OP_MODE_ACCESS_POINT == prBssInfo->eCurrentOPMode)) {
                    prCurrSwRfb->eDst = RX_PKT_DESTINATION_HOST_WITH_FORWARD;
                }
            }
#endif
            QUEUE_INSERT_TAIL(&rReturnedQue, (P_QUE_ENTRY_T)prCurrSwRfb);
        }
        /* Reordering is required for this packet */
        else {
            /* If this packet should dropped or indicated to the host immediately,
            *  it should be enqueued into the rReturnedQue with specific flags. If
            *  this packet should be buffered for reordering, it should be enqueued
            *  into the reordering queue in the STA_REC rather than into the
            *  rReturnedQue.
            */
            qmProcessPktWithReordering(prAdapter, prCurrSwRfb, &rReturnedQue);

        }
    } while (prNextSwRfb);


    /* The returned list of SW_RFBs must end with a NULL pointer */
    if (QUEUE_IS_NOT_EMPTY(&rReturnedQue)) {
        QM_TX_SET_NEXT_MSDU_INFO((P_SW_RFB_T)QUEUE_GET_TAIL(&rReturnedQue), NULL);
    }

    return (P_SW_RFB_T)QUEUE_GET_HEAD(&rReturnedQue);

#else

    //DbgPrint("QM: Enter qmHandleRxPackets()\n");
    return prSwRfbListHead;

#endif

}

/*----------------------------------------------------------------------------*/
/*!
* \brief Reorder the received packet
*
* \param[in] prSwRfb The RX packet to process
* \param[out] prReturnedQue The queue for indicating packets
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmProcessPktWithReordering(
    IN P_ADAPTER_T   prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT P_QUE_T prReturnedQue
)
{


    P_STA_RECORD_T prStaRec;
    P_HIF_RX_HEADER_T   prHifRxHdr;
    P_RX_BA_ENTRY_T prReorderQueParm;

    UINT_32 u4SeqNo;
    UINT_32 u4WinStart;
    UINT_32 u4WinEnd;
    //P_QUE_T prReorderQue;
    //P_SW_RFB_T prReorderedSwRfb;


    ASSERT(prSwRfb);
    ASSERT(prReturnedQue);
    ASSERT(prSwRfb->prHifRxHdr);

    prHifRxHdr = prSwRfb->prHifRxHdr;
    prSwRfb->ucStaRecIdx = prHifRxHdr->ucStaRecIdx;
    prSwRfb->u2SSN = HIF_RX_HDR_GET_SN(prHifRxHdr);  /* The new SN of the frame */
    prSwRfb->ucTid = (UINT_8)(HIF_RX_HDR_GET_TID(prHifRxHdr));
    //prSwRfb->eDst = RX_PKT_DESTINATION_HOST;

    /* Incorrect STA_REC index */
    if (prSwRfb->ucStaRecIdx >= CFG_NUM_OF_STA_RECORD) {
        prSwRfb->eDst = RX_PKT_DESTINATION_NULL;
        QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);
        //DBGLOG(QM, WARN,("Reordering for a NULL STA_REC, ucStaRecIdx = %d\n",
        //    prSwRfb->ucStaRecIdx));
        kal_trace(TRACE_WARNING , WARN_QM_REORDERING_NULL_STA_REC , prSwRfb->ucStaRecIdx);
        //ASSERT(0);
        return;
    }

    /* Check whether the STA_REC is activated */
    prStaRec = &(prAdapter->arStaRec[prSwRfb->ucStaRecIdx]);
    ASSERT(prStaRec);

#if 0
    if (!(prStaRec->fgIsValid)) {
        /* TODO: (Tehuang) Handle the Host-FW sync issue. */
        prSwRfb->eDst = RX_PKT_DESTINATION_NULL;
        QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);
        // DBGLOG(QM, WARN, ("Reordering for an invalid STA_REC \n"));
        kal_trace(TRACE_WARNING , WARN_QM_REORDERING_INVALID_STA_REC);
        //ASSERT(0);
        return;
    }
#endif

    /* Check whether the BA agreement exists */
    prReorderQueParm = ((prStaRec->aprRxReorderParamRefTbl)[prSwRfb->ucTid]);
    if (!prReorderQueParm) {
        /* TODO: (Tehuang) Handle the Host-FW sync issue.*/
        prSwRfb->eDst = RX_PKT_DESTINATION_NULL;
        QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);
        //DBGLOG(QM, WARN,("Reordering for a NULL ReorderQueParm \n"));
        kal_trace(TRACE_WARNING , WARN_QM_REORDERING_QUEPARM);
        //ASSERT(0);
        return;
    }



    /* Start to reorder packets */
    u4SeqNo = (UINT_32)(prSwRfb->u2SSN);
    //prReorderQue = &(prReorderQueParm->rReOrderQue);
    u4WinStart = (UINT_32)(prReorderQueParm->u2WinStart);
    u4WinEnd = (UINT_32)(prReorderQueParm->u2WinEnd);

    /* Debug */
    //DbgPrint("QM:(R)[%d](%ld){%ld,%ld}\n", prSwRfb->ucTid, u4SeqNo, u4WinStart, u4WinEnd);

    /* Case 1: Fall within */
    if  /* 0 - start - sn - end - 4095 */
    (((u4WinStart <= u4SeqNo) && (u4SeqNo <= u4WinEnd))
            /* 0 - end - start - sn - 4095 */
            || ((u4WinEnd < u4WinStart) && (u4WinStart <= u4SeqNo))
            /* 0 - sn - end - start - 4095 */
            || ((u4SeqNo <= u4WinEnd) && (u4WinEnd < u4WinStart))) {

        qmInsertFallWithinReorderPkt(prSwRfb, prReorderQueParm, prReturnedQue);

#if QM_RX_WIN_SSN_AUTO_ADVANCING
        if (prReorderQueParm->fgIsWaitingForPktWithSsn) {
            /* Let the first received packet pass the reorder check */

            prReorderQueParm->u2WinStart = (UINT_16)u4SeqNo;
            prReorderQueParm->u2WinEnd =
                ((prReorderQueParm->u2WinStart) + (prReorderQueParm->u2WinSize) - 1) % MAX_SEQ_NO_COUNT;
            prReorderQueParm->fgIsWaitingForPktWithSsn = FALSE;
        }
#endif


        qmPopOutDueToFallWithin(prReorderQueParm, prReturnedQue);
    }
    /* Case 2: Fall ahead */
    else if
    /* 0 - start - end - sn - (start+2048) - 4095 */
    (((u4WinStart < u4WinEnd)
            && (u4WinEnd < u4SeqNo)
            && (u4SeqNo < (u4WinStart + HALF_SEQ_NO_COUNT)))
            /* 0 - sn - (start+2048) - start - end - 4095 */
            || ((u4SeqNo < u4WinStart)
                && (u4WinStart < u4WinEnd)
                && ((u4SeqNo + MAX_SEQ_NO_COUNT) < (u4WinStart + HALF_SEQ_NO_COUNT)))
            /* 0 - end - sn - (start+2048) - start - 4095 */
            || ((u4WinEnd < u4SeqNo)
                && (u4SeqNo < u4WinStart)
                && ((u4SeqNo + MAX_SEQ_NO_COUNT) < (u4WinStart + HALF_SEQ_NO_COUNT)))) {


#if QM_RX_WIN_SSN_AUTO_ADVANCING
        if (prReorderQueParm->fgIsWaitingForPktWithSsn) {
            prReorderQueParm->fgIsWaitingForPktWithSsn = FALSE;
        }
#endif

        qmInsertFallAheadReorderPkt(prSwRfb, prReorderQueParm, prReturnedQue);

        /* Advance the window after inserting a new tail */
        prReorderQueParm->u2WinEnd = (UINT_16)u4SeqNo;
        prReorderQueParm->u2WinStart =
            (((prReorderQueParm->u2WinEnd) - (prReorderQueParm->u2WinSize) + MAX_SEQ_NO_COUNT + 1)
             % MAX_SEQ_NO_COUNT);

        qmPopOutDueToFallAhead(prReorderQueParm, prReturnedQue);

    }
    /* Case 3: Fall behind */
    else {

#if QM_RX_WIN_SSN_AUTO_ADVANCING
#if QM_RX_INIT_FALL_BEHIND_PASS
        if (prReorderQueParm->fgIsWaitingForPktWithSsn) {
            //?? prSwRfb->eDst = RX_PKT_DESTINATION_HOST;
            QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);
            //DbgPrint("QM:(P)[%d](%ld){%ld,%ld}\n", prSwRfb->ucTid, u4SeqNo, u4WinStart, u4WinEnd);
            return;
        }
#endif
#endif

        /* An erroneous packet */
        prSwRfb->eDst = RX_PKT_DESTINATION_NULL;
        QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);
        //DbgPrint("QM:(D)[%d](%ld){%ld,%ld}\n", prSwRfb->ucTid, u4SeqNo, u4WinStart, u4WinEnd);
        return;
    }

    return;

}

VOID
qmProcessBarFrame(
    IN P_ADAPTER_T   prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT P_QUE_T prReturnedQue
)
{

    P_STA_RECORD_T prStaRec;
    P_HIF_RX_HEADER_T   prHifRxHdr;
    P_RX_BA_ENTRY_T prReorderQueParm;

    UINT_32 u4SSN;
    UINT_32 u4WinStart;
    //UINT_32 u4WinEnd;
    //P_QUE_T prReorderQue;
    //P_SW_RFB_T prReorderedSwRfb;

    ASSERT(prSwRfb);
    ASSERT(prReturnedQue);
    ASSERT(prSwRfb->prHifRxHdr);

    prHifRxHdr = prSwRfb->prHifRxHdr;
    prSwRfb->ucStaRecIdx = prHifRxHdr->ucStaRecIdx;
    prSwRfb->u2SSN = HIF_RX_HDR_GET_SN(prHifRxHdr); /* The new SSN */
    prSwRfb->ucTid = (UINT_8)(HIF_RX_HDR_GET_TID(prHifRxHdr));

    prSwRfb->eDst = RX_PKT_DESTINATION_NULL;
    QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);

    /* Incorrect STA_REC index */
    if (prSwRfb->ucStaRecIdx >= CFG_NUM_OF_STA_RECORD) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "QM: (Warning) BAR for a NULL STA_REC, ucStaRecIdx = %d\n",
                         prSwRfb->ucStaRecIdx);
        //ASSERT(0);
        return;
    }

    /* Check whether the STA_REC is activated */
    prStaRec = &(prAdapter->arStaRec[prSwRfb->ucStaRecIdx]);
    ASSERT(prStaRec);

#if 0
    if (!(prStaRec->fgIsValid)) {
        /* TODO: (Tehuang) Handle the Host-FW sync issue. */
        DbgPrint("QM: (Warning) BAR for an invalid STA_REC \n");
        //ASSERT(0);
        return;
    }
#endif

    /* Check whether the BA agreement exists */
    prReorderQueParm = ((prStaRec->aprRxReorderParamRefTbl)[prSwRfb->ucTid]);
    if (!prReorderQueParm) {
        /* TODO: (Tehuang) Handle the Host-FW sync issue.*/
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "QM: (Warning) BAR for a NULL ReorderQueParm \n");
        //ASSERT(0);
        return;
    }


    u4SSN = (UINT_32)(prSwRfb->u2SSN);
    //prReorderQue = &(prReorderQueParm->rReOrderQue);
    u4WinStart = (UINT_32)(prReorderQueParm->u2WinStart);
    //u4WinEnd = (UINT_32)(prReorderQueParm->u2WinEnd);

    if (qmCompareSnIsLessThan(u4WinStart, u4SSN)) {
        prReorderQueParm->u2WinStart = (UINT_16)u4SSN;
        prReorderQueParm->u2WinEnd =
            ((prReorderQueParm->u2WinStart) + (prReorderQueParm->u2WinSize) - 1) % MAX_SEQ_NO_COUNT;
        qmPopOutDueToFallAhead(prReorderQueParm, prReturnedQue);
    } else {
        // DbgPrint("QM:(BAR)(%d)(%ld){%ld,%ld}\n", prSwRfb->ucTid, u4SSN, u4WinStart, u4WinEnd);
#if CFG_ENABLE_TXRX_PATH_LOG
        MT5931_TRACE(TRACE_GROUP_8, MT5931_INFO_110, "QM:(BAR)(%d)(%ld){%ld,%ld}\n", prSwRfb->ucTid, u4SSN, u4WinStart, u4WinEnd);
#endif
    }
}

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code = "WIFISpeedUp", rwdata = "WIFISpeedUp",  zidata = "WIFISpeedUp"
#endif

VOID
qmInsertFallWithinReorderPkt(
    IN P_SW_RFB_T prSwRfb,
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
)
{
    P_SW_RFB_T prExaminedQueuedSwRfb;
    P_QUE_T prReorderQue;
    ASSERT(prSwRfb);
    ASSERT(prReorderQueParm);
    ASSERT(prReturnedQue);

    prReorderQue = &(prReorderQueParm->rReOrderQue);
    prExaminedQueuedSwRfb = (P_SW_RFB_T)QUEUE_GET_HEAD(prReorderQue);

    /* There are no packets queued in the Reorder Queue */
    if (prExaminedQueuedSwRfb == NULL) {
        ((P_QUE_ENTRY_T)prSwRfb)->prPrev = NULL;
        ((P_QUE_ENTRY_T)prSwRfb)->prNext = NULL;
        prReorderQue->prHead = (P_QUE_ENTRY_T)prSwRfb;
        prReorderQue->prTail = (P_QUE_ENTRY_T)prSwRfb;
        prReorderQue->u4NumElem ++;
    }

    /* Determine the insert position */
    else {
        do {
            /* Case 1: Terminate. A duplicate packet */
            if (((prExaminedQueuedSwRfb->u2SSN) == (prSwRfb->u2SSN))) {
                prSwRfb->eDst = RX_PKT_DESTINATION_NULL;
                QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prSwRfb);
                return;
            }

            /* Case 2: Terminate. The insert point is found */
            else if (qmCompareSnIsLessThan(
                         (prSwRfb->u2SSN), (prExaminedQueuedSwRfb->u2SSN))) {
                break;
            }

            /* Case 3: Insert point not found. Check the next SW_RFB in the Reorder Queue */
            else {
                prExaminedQueuedSwRfb =
                    (P_SW_RFB_T)(((P_QUE_ENTRY_T)prExaminedQueuedSwRfb)->prNext);
            }
        } while (prExaminedQueuedSwRfb);

        /* Update the Reorder Queue Parameters according to the found insert position */
        if (prExaminedQueuedSwRfb == NULL) {
            /* The received packet shall be placed at the tail */
            ((P_QUE_ENTRY_T)prSwRfb)->prPrev = prReorderQue->prTail;
            ((P_QUE_ENTRY_T)prSwRfb)->prNext = NULL;
            (prReorderQue->prTail)->prNext = (P_QUE_ENTRY_T)(prSwRfb);
            prReorderQue->prTail = (P_QUE_ENTRY_T)(prSwRfb);
        } else {
            ((P_QUE_ENTRY_T)prSwRfb)->prPrev = ((P_QUE_ENTRY_T)prExaminedQueuedSwRfb)->prPrev;
            ((P_QUE_ENTRY_T)prSwRfb)->prNext = (P_QUE_ENTRY_T)prExaminedQueuedSwRfb;
            if (((P_QUE_ENTRY_T)prExaminedQueuedSwRfb) == (prReorderQue->prHead)) {
                /* The received packet will become the head */
                prReorderQue->prHead = (P_QUE_ENTRY_T)prSwRfb;
            } else {
                (((P_QUE_ENTRY_T)prExaminedQueuedSwRfb)->prPrev)->prNext = (P_QUE_ENTRY_T)prSwRfb;
            }
            ((P_QUE_ENTRY_T)prExaminedQueuedSwRfb)->prPrev = (P_QUE_ENTRY_T)prSwRfb;
        }

        prReorderQue->u4NumElem ++;

    }

}


VOID
qmInsertFallAheadReorderPkt(
    IN P_SW_RFB_T prSwRfb,
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
)
{
    P_QUE_T prReorderQue;
    ASSERT(prSwRfb);
    ASSERT(prReorderQueParm);
    ASSERT(prReturnedQue);

    prReorderQue = &(prReorderQueParm->rReOrderQue);

    /* There are no packets queued in the Reorder Queue */
    if (QUEUE_IS_EMPTY(prReorderQue)) {
        ((P_QUE_ENTRY_T)prSwRfb)->prPrev = NULL;
        ((P_QUE_ENTRY_T)prSwRfb)->prNext = NULL;
        prReorderQue->prHead = (P_QUE_ENTRY_T)prSwRfb;
    } else {
        ((P_QUE_ENTRY_T)prSwRfb)->prPrev = prReorderQue->prTail;
        ((P_QUE_ENTRY_T)prSwRfb)->prNext = NULL;
        (prReorderQue->prTail)->prNext = (P_QUE_ENTRY_T)(prSwRfb);
    }
    prReorderQue->prTail = (P_QUE_ENTRY_T)prSwRfb;
    prReorderQue->u4NumElem ++;

}


VOID
qmPopOutDueToFallWithin(
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
)
{
    P_SW_RFB_T prReorderedSwRfb;
    P_QUE_T prReorderQue;
    BOOLEAN fgDequeuHead;

    prReorderQue = &(prReorderQueParm->rReOrderQue);

    /* Check whether any packet can be indicated to the higher layer */
    while (TRUE) {
        if (QUEUE_IS_EMPTY(prReorderQue)) {
            break;
        }

        /* Always examine the head packet */
        prReorderedSwRfb = (P_SW_RFB_T)QUEUE_GET_HEAD(prReorderQue);
        fgDequeuHead = FALSE;

        /* SN == WinStart, so the head packet shall be indicated (advance the window) */
        if ((prReorderedSwRfb->u2SSN) == (prReorderQueParm->u2WinStart)) {

            fgDequeuHead = TRUE;
            prReorderQueParm->u2WinStart =
                (((prReorderedSwRfb->u2SSN) + 1) % MAX_SEQ_NO_COUNT);
        }
        /* SN > WinStart, break to update WinEnd */
        else {
            break;
        }


        /* Dequeue the head packet */
        if (fgDequeuHead) {

            if (((P_QUE_ENTRY_T)prReorderedSwRfb)->prNext == NULL) {
                prReorderQue->prHead = NULL;
                prReorderQue->prTail = NULL;
            } else {
                prReorderQue->prHead = ((P_QUE_ENTRY_T)prReorderedSwRfb)->prNext;
                (((P_QUE_ENTRY_T)prReorderedSwRfb)->prNext)->prPrev = NULL;
            }
            prReorderQue->u4NumElem --;
            //DbgPrint("QM: [%d] %d (%d)\n", prReorderQueParm->ucTid, prReorderedSwRfb->u2PacketLen, prReorderedSwRfb->u2SSN);
            QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prReorderedSwRfb);
        }
    }

    /* After WinStart has been determined, update the WinEnd */
    prReorderQueParm->u2WinEnd =
        (((prReorderQueParm->u2WinStart) + (prReorderQueParm->u2WinSize) - 1) % MAX_SEQ_NO_COUNT);

}

VOID
qmPopOutDueToFallAhead(
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
)
{
    P_SW_RFB_T prReorderedSwRfb;
    P_QUE_T prReorderQue;
    BOOLEAN fgDequeuHead;

    prReorderQue = &(prReorderQueParm->rReOrderQue);

    /* Check whether any packet can be indicated to the higher layer */
    while (TRUE) {
        if (QUEUE_IS_EMPTY(prReorderQue)) {
            break;
        }

        /* Always examine the head packet */
        prReorderedSwRfb = (P_SW_RFB_T)QUEUE_GET_HEAD(prReorderQue);
        fgDequeuHead = FALSE;

        /* SN == WinStart, so the head packet shall be indicated (advance the window) */
        if ((prReorderedSwRfb->u2SSN) == (prReorderQueParm->u2WinStart)) {

            fgDequeuHead = TRUE;
            prReorderQueParm->u2WinStart =
                (((prReorderedSwRfb->u2SSN) + 1) % MAX_SEQ_NO_COUNT);
        }

        /* SN < WinStart, so the head packet shall be indicated (do not advance the window) */
        else if (qmCompareSnIsLessThan(
                     (UINT_32)(prReorderedSwRfb->u2SSN),
                     (UINT_32)(prReorderQueParm->u2WinStart))) {

            fgDequeuHead = TRUE;

        }

        /* SN > WinStart, break to update WinEnd */
        else {
            break;
        }


        /* Dequeue the head packet */
        if (fgDequeuHead) {

            if (((P_QUE_ENTRY_T)prReorderedSwRfb)->prNext == NULL) {
                prReorderQue->prHead = NULL;
                prReorderQue->prTail = NULL;
            } else {
                prReorderQue->prHead = ((P_QUE_ENTRY_T)prReorderedSwRfb)->prNext;
                (((P_QUE_ENTRY_T)prReorderedSwRfb)->prNext)->prPrev = NULL;
            }
            prReorderQue->u4NumElem --;
            //DbgPrint("QM: [%d] %d (%d)\n", prReorderQueParm->ucTid, prReorderedSwRfb->u2PacketLen, prReorderedSwRfb->u2SSN);
            QUEUE_INSERT_TAIL(prReturnedQue, (P_QUE_ENTRY_T)prReorderedSwRfb);
        }
    }

    /* After WinStart has been determined, update the WinEnd */
    prReorderQueParm->u2WinEnd =
        (((prReorderQueParm->u2WinStart) + (prReorderQueParm->u2WinSize) - 1) % MAX_SEQ_NO_COUNT);

}
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code, rwdata,  zidata
#endif

BOOLEAN
qmCompareSnIsLessThan(
    IN UINT_32 u4SnLess,
    IN UINT_32 u4SnGreater
)
{
    /* 0 <--->  SnLess   <--(gap>2048)--> SnGreater : SnLess > SnGreater */
    if ((u4SnLess + HALF_SEQ_NO_COUNT) <= u4SnGreater) { /* Shall be <= */
        return FALSE;
    }

    /* 0 <---> SnGreater <--(gap>2048)--> SnLess    : SnLess < SnGreater */
    else if ((u4SnGreater + HALF_SEQ_NO_COUNT) < u4SnLess) {
        return TRUE;
    }

    /* 0 <---> SnGreater <--(gap<2048)--> SnLess    : SnLess > SnGreater */
    /* 0 <--->  SnLess   <--(gap<2048)--> SnGreater : SnLess < SnGreater */
    else {
        return (u4SnLess < u4SnGreater);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Handle Mailbox RX messages
*
* \param[in] prMailboxRxMsg The received Mailbox message from the FW
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmHandleMailboxRxMessage(
    IN MAILBOX_MSG_T prMailboxRxMsg
)
{
    //DbgPrint("QM: Enter qmHandleMailboxRxMessage()\n");
    /* TODO */
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Handle ADD RX BA Event from the FW
*
* \param[in] prAdapter Adapter pointer
* \param[in] prEvent The event packet from the FW
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmHandleEventRxAddBa(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
)
{
    P_EVENT_RX_ADDBA_T prEventRxAddBa;
    P_STA_RECORD_T prStaRec;
    UINT_32 u4Tid;
    UINT_32 u4WinSize;


    prEventRxAddBa = (P_EVENT_RX_ADDBA_T)prEvent;
    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, prEventRxAddBa->ucStaRecIdx);

    if (!prStaRec) {
        /* Invalid STA_REC index, discard the event packet */
        //ASSERT(0);
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "QM: (ERROR) RX ADDBA Event for a NULL STA_REC\n");
        return;
    }

#if 0
    if (!(prStaRec->fgIsValid)) {
        /* TODO: (Tehuang) Handle the Host-FW synchronization issue */
        DbgPrint("QM: (Warning) RX ADDBA Event for an invalid STA_REC\n");
        //ASSERT(0);
        return;
    }
#endif

    u4Tid = (((prEventRxAddBa->u2BAParameterSet)& BA_PARAM_SET_TID_MASK)
             >> BA_PARAM_SET_TID_MASK_OFFSET);

    u4WinSize = (((prEventRxAddBa->u2BAParameterSet)& BA_PARAM_SET_BUFFER_SIZE_MASK)
                 >> BA_PARAM_SET_BUFFER_SIZE_MASK_OFFSET);

    if (!qmAddRxBaEntry(
                prAdapter,
                prStaRec->ucIndex,
                (UINT_8)u4Tid,
                (prEventRxAddBa->u2BAStartSeqCtrl >> OFFSET_BAR_SSC_SN),
                (UINT_16)u4WinSize)) {

        /* FW shall ensure the availabiilty of the free-to-use BA entry */
        DbgPrint("QM: (Error) qmAddRxBaEntry() failure\n");
        ASSERT(0);
    }

}

/*----------------------------------------------------------------------------*/
/*!
* \brief Handle DEL RX BA Event from the FW
*
* \param[in] prAdapter Adapter pointer
* \param[in] prEvent The event packet from the FW
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmHandleEventRxDelBa(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
)
{
    P_EVENT_RX_DELBA_T prEventRxDelBa;
    P_STA_RECORD_T prStaRec;

    //DbgPrint("QM:Event -RxBa\n");

    prEventRxDelBa = (P_EVENT_RX_DELBA_T)prEvent;
    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, prEventRxDelBa->ucStaRecIdx);

    if (!prStaRec) {
        /* Invalid STA_REC index, discard the event packet */
        //ASSERT(0);
        return;
    }

#if 0
    if (!(prStaRec->fgIsValid)) {
        /* TODO: (Tehuang) Handle the Host-FW synchronization issue */
        //ASSERT(0);
        return;
    }
#endif

    qmDelRxBaEntry(prAdapter, prStaRec->ucIndex, prEventRxDelBa->ucTid, TRUE);

}

P_RX_BA_ENTRY_T
qmLookupRxBaEntry(
    IN P_ADAPTER_T prAdapter,
    UINT_8 ucStaRecIdx,
    UINT_8 ucTid
)
{
    int i;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    //DbgPrint("QM: Enter qmLookupRxBaEntry()\n");

    for (i = 0; i < CFG_NUM_OF_RX_BA_AGREEMENTS; i++) {
        if (prQM->arRxBaTable[i].fgIsValid) {
            if ((prQM->arRxBaTable[i].ucStaRecIdx == ucStaRecIdx) &&
                    (prQM->arRxBaTable[i].ucTid == ucTid)) {
                return &prQM->arRxBaTable[i];
            }
        }
    }
    return NULL;
}

BOOL
qmAddRxBaEntry(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8  ucStaRecIdx,
    IN UINT_8  ucTid,
    IN UINT_16 u2WinStart,
    IN UINT_16 u2WinSize
)
{
    int i;
    P_RX_BA_ENTRY_T prRxBaEntry = NULL;
    P_STA_RECORD_T prStaRec;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    ASSERT(ucStaRecIdx < CFG_NUM_OF_STA_RECORD);

    if (ucStaRecIdx >= CFG_NUM_OF_STA_RECORD) {
        /* Invalid STA_REC index, discard the event packet */
        DbgPrint("QM: (WARNING) RX ADDBA Event for a invalid ucStaRecIdx = %d\n", ucStaRecIdx);
        return FALSE;
    }

    prStaRec = &prAdapter->arStaRec[ucStaRecIdx];
    ASSERT(prStaRec);

    //if(!(prStaRec->fgIsValid)){
    //    DbgPrint("QM: (WARNING) Invalid STA when adding an RX BA \n");
    //    return FALSE;
    //}

    //4 <1> Delete before adding
    /* Remove the BA entry for the same (STA, TID) tuple if it exists */
    if (qmLookupRxBaEntry(prAdapter, ucStaRecIdx, ucTid)) {
        qmDelRxBaEntry(prAdapter, ucStaRecIdx, ucTid, TRUE); /* prQM->ucRxBaCount-- */
    }

    //4 <2> Add a new BA entry
    /* No available entry to store the BA agreement info. Retrun FALSE. */
    if (prQM->ucRxBaCount >= CFG_NUM_OF_RX_BA_AGREEMENTS) {
        DbgPrint("QM: **failure** (limited resource, ucRxBaCount=%d)\n", prQM->ucRxBaCount);
        return FALSE;
    } else {
        /* Find the free-to-use BA entry */
        for (i = 0; i < CFG_NUM_OF_RX_BA_AGREEMENTS; i++) {
            if (!prQM->arRxBaTable[i].fgIsValid) {
                prRxBaEntry = &(prQM->arRxBaTable[i]);
                prQM->ucRxBaCount++;
                DbgPrint("QM: ucRxBaCount=%d\n", prQM->ucRxBaCount);
                break;
            }
        }

        /* If a free-to-use entry is found, configure it and associate it with the STA_REC */
        u2WinSize = ((u2WinSize > (CFG_RX_BA_MAX_WINSIZE << 1)) ? u2WinSize : (CFG_RX_BA_MAX_WINSIZE << 1));
        u2WinSize = ((u2WinSize > 64) ? 64 : u2WinSize);
        if (prRxBaEntry) {
            prRxBaEntry->ucStaRecIdx = ucStaRecIdx;
            prRxBaEntry->ucTid = ucTid;
            prRxBaEntry->u2WinStart = u2WinStart;
            prRxBaEntry->u2WinSize = u2WinSize;
            prRxBaEntry->u2WinEnd = ((u2WinStart + u2WinSize - 1) % MAX_SEQ_NO_COUNT);
            prRxBaEntry->fgIsValid = TRUE;
            prRxBaEntry->fgIsWaitingForPktWithSsn = TRUE;

            DbgPrint("QM: +RxBA(STA=%d TID=%d WinStart=%d WinEnd=%d WinSize=%d)\n",
                     ucStaRecIdx, ucTid,
                     prRxBaEntry->u2WinStart, prRxBaEntry->u2WinEnd, prRxBaEntry->u2WinSize);

            /* Update the BA entry reference table for per-packet lookup */
            prStaRec->aprRxReorderParamRefTbl[ucTid] = prRxBaEntry;
        } else {
            /* This shall not happen because FW should keep track of the usage of RX BA entries */
            DbgPrint("QM: **AddBA Error** (ucRxBaCount=%d)\n", prQM->ucRxBaCount);
            return FALSE;
        }
    }

    return TRUE;
}
VOID
qmDelRxBaEntry(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucStaRecIdx,
    IN UINT_8 ucTid,
    IN BOOLEAN fgFlushToHost
)
{
    P_RX_BA_ENTRY_T prRxBaEntry;
    P_STA_RECORD_T prStaRec;
    P_SW_RFB_T prFlushedPacketList = NULL;
    P_QUE_MGT_T prQM = &prAdapter->rQM;

    ASSERT(ucStaRecIdx < CFG_NUM_OF_STA_RECORD);

    prStaRec = &prAdapter->arStaRec[ucStaRecIdx];
    ASSERT(prStaRec);

#if 0
    if (!(prStaRec->fgIsValid)) {
        DbgPrint("QM: (WARNING) Invalid STA when deleting an RX BA \n");
        return;
    }
#endif

    /* Remove the BA entry for the same (STA, TID) tuple if it exists */
    prRxBaEntry = prStaRec->aprRxReorderParamRefTbl[ucTid];

    if (prRxBaEntry) {

        prFlushedPacketList = qmFlushStaRxQueue(prAdapter, ucStaRecIdx, ucTid);

        if (prFlushedPacketList) {

            if (fgFlushToHost) {
                wlanProcessQueuedSwRfb(prAdapter, prFlushedPacketList);
            } else {

                P_SW_RFB_T prSwRfb;
                P_SW_RFB_T prNextSwRfb;
                prSwRfb =  prFlushedPacketList;

                do {
                    prNextSwRfb = (P_SW_RFB_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prSwRfb);
                    nicRxReturnRFB(prAdapter, prSwRfb);
                    prSwRfb = prNextSwRfb;
                } while (prSwRfb);

            }


        }
#if ((QM_TEST_MODE == 0) && (QM_TEST_STA_REC_DEACTIVATION == 0))
        /* Update RX BA entry state. Note that RX queue flush is not done here */
        prRxBaEntry->fgIsValid = FALSE;
        prQM->ucRxBaCount--;

        /* Debug */
#if 0
        DbgPrint("QM: ucRxBaCount=%d\n", prQM->ucRxBaCount);
#endif

        /* Update STA RX BA table */
        prStaRec->aprRxReorderParamRefTbl[ucTid] = NULL;
#endif
        DbgPrint("QM: -RxBA(STA=%d,TID=%d)\n", ucStaRecIdx, ucTid);
    }


    /* Debug */
#if CFG_HIF_RX_STARVATION_WARNING
    {
        P_RX_CTRL_T prRxCtrl;
        prRxCtrl = &prAdapter->rRxCtrl;
        DbgPrint("QM: (RX DEBUG) Enqueued: %d / Dequeued: %d\n", prRxCtrl->u4QueuedCnt, prRxCtrl->u4DequeuedCnt);
    }
#endif
}


/*----------------------------------------------------------------------------*/
/*!
* \brief To process WMM related IEs in ASSOC_RSP
*
* \param[in] prAdapter Adapter pointer
* \param[in] prSwRfb            The received frame
* \param[in] pucIE              The pointer to the first IE in the frame
* \param[in] u2IELength         The total length of IEs in the frame
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mqmProcessAssocReq(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb,
    IN PUINT_8     pucIE,
    IN UINT_16     u2IELength
)
{
    P_STA_RECORD_T      prStaRec;
    UINT_16             u2Offset;
    //PUINT_8             pucIEStart;
    UINT_8 aucWfaOui[] = VENDOR_OUI_WFA;
    P_IE_WMM_INFO_T prIeWmmInfo;

    DEBUGFUNC("mqmProcessAssocReq");

    ASSERT(prSwRfb);
    ASSERT(pucIE);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
    ASSERT(prStaRec);

    if (prStaRec == NULL) {
        return;
    }

    prStaRec->fgIsQoS = FALSE;
    prStaRec->fgIsWmmSupported = prStaRec->fgIsUapsdSupported = FALSE;

    //pucIEStart = pucIE;

    /* If the device does not support QoS or if WMM is not supported by the peer, exit.*/
    if (!prAdapter->rWifiVar.fgSupportQoS) {
        return;
    }


    /* Determine whether QoS is enabled with the association */
    else {
        IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
            switch (IE_ID(pucIE)) {
                case ELEM_ID_WMM:

                    if ((WMM_IE_OUI_TYPE(pucIE) == VENDOR_OUI_TYPE_WMM) &&
                            (!kalMemCmp(WMM_IE_OUI(pucIE), aucWfaOui, 3))) {

                        switch (WMM_IE_OUI_SUBTYPE(pucIE)) {
                            case VENDOR_OUI_SUBTYPE_WMM_INFO: {

                                UINT_8 ucQosInfo;
                                UINT_8 ucQosInfoAC;
                                UINT_8 ucBmpAC;
                                if (IE_LEN(pucIE) != 7) {
                                    break; /* WMM Info IE with a wrong length */
                                }
                                prStaRec->fgIsQoS = TRUE;
                                prStaRec->fgIsWmmSupported = TRUE;

                                prIeWmmInfo = (P_IE_WMM_INFO_T)pucIE;
                                ucQosInfo = prIeWmmInfo->ucQosInfo;
                                ucQosInfoAC = ucQosInfo & BITS(0, 3);

                                prStaRec->fgIsUapsdSupported = ((ucQosInfoAC) ? TRUE : FALSE) &
                                                               prAdapter->rWifiVar.fgSupportUAPSD;

                                ucBmpAC = 0;

                                if (ucQosInfoAC & WMM_QOS_INFO_VO_UAPSD) {
                                    ucBmpAC |= BIT(ACI_VO);
                                }
                                if (ucQosInfoAC & WMM_QOS_INFO_VI_UAPSD) {
                                    ucBmpAC |= BIT(ACI_VI);
                                }
                                if (ucQosInfoAC & WMM_QOS_INFO_BE_UAPSD) {
                                    ucBmpAC |= BIT(ACI_BE);
                                }
                                if (ucQosInfoAC & WMM_QOS_INFO_BK_UAPSD) {
                                    ucBmpAC |= BIT(ACI_BK);
                                }

                                prStaRec->ucBmpTriggerAC = prStaRec->ucBmpDeliveryAC = ucBmpAC;

                                prStaRec->ucUapsdSp = (ucQosInfo & WMM_QOS_INFO_MAX_SP_LEN_MASK) >> 5;

                            }
                            break;

                            default:
                                /* Other WMM QoS IEs. Ignore any */
                                break;
                        }
                    }
                    /* else: VENDOR_OUI_TYPE_WPA, VENDOR_OUI_TYPE_WPS */

                    break;
                default:
                    break;
            }
        }

        // DBGLOG(QM, EVENT, ("MQM: Assoc_Req Parsing (QoS Enabled=%d)\n", prStaRec->fgIsQoS));
        kal_trace(TRACE_GROUP_10 , EVENT_QM_MQM_QOS , prStaRec->fgIsQoS);

    }
}


/*----------------------------------------------------------------------------*/
/*!
* \brief To process WMM related IEs in ASSOC_RSP
*
* \param[in] prAdapter Adapter pointer
* \param[in] prSwRfb            The received frame
* \param[in] pucIE              The pointer to the first IE in the frame
* \param[in] u2IELength         The total length of IEs in the frame
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mqmProcessAssocRsp(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb,
    IN PUINT_8     pucIE,
    IN UINT_16     u2IELength
)
{
    P_STA_RECORD_T      prStaRec;
    UINT_16             u2Offset;
    PUINT_8             pucIEStart;
    UINT_8 aucWfaOui[] = VENDOR_OUI_WFA;

    DEBUGFUNC("mqmProcessAssocRsp");

    ASSERT(prSwRfb);
    ASSERT(pucIE);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
    ASSERT(prStaRec);

    if (prStaRec == NULL) {
        return;
    }

    prStaRec->fgIsQoS = FALSE;

    pucIEStart = pucIE;

    DbgPrint("QM: (fgIsWmmSupported=%d, fgSupportQoS=%d)\n",
             prStaRec->fgIsWmmSupported, prAdapter->rWifiVar.fgSupportQoS);

    /* If the device does not support QoS or if WMM is not supported by the peer, exit.*/
    if ((!prAdapter->rWifiVar.fgSupportQoS) || (!prStaRec->fgIsWmmSupported)) {
        prAdapter->fgIsWmmAssoc = FALSE;
        return;
    }

    /* Determine whether QoS is enabled with the association */
    else {
        prAdapter->fgIsWmmAssoc = TRUE;
        IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
            switch (IE_ID(pucIE)) {
                case ELEM_ID_WMM:

                    if ((WMM_IE_OUI_TYPE(pucIE) == VENDOR_OUI_TYPE_WMM) &&
                            (!kalMemCmp(WMM_IE_OUI(pucIE), aucWfaOui, 3))) {

                        switch (WMM_IE_OUI_SUBTYPE(pucIE)) {
                            case VENDOR_OUI_SUBTYPE_WMM_PARAM:
                                if (IE_LEN(pucIE) != 24) {
                                    break; /* WMM Info IE with a wrong length */
                                }
                                prStaRec->fgIsQoS = TRUE;
                                break;

                            case VENDOR_OUI_SUBTYPE_WMM_INFO:
                                if (IE_LEN(pucIE) != 7) {
                                    break; /* WMM Info IE with a wrong length */
                                }
                                prStaRec->fgIsQoS = TRUE;
                                break;

                            default:
                                /* Other WMM QoS IEs. Ignore any */
                                break;
                        }
                    }
                    /* else: VENDOR_OUI_TYPE_WPA, VENDOR_OUI_TYPE_WPS */

                    break;
                default:
                    break;
            }
        }

        /* Parse AC parameters and write to HW CRs */
        if ((prStaRec->fgIsQoS) && (prStaRec->eStaType == STA_TYPE_LEGACY_AP)) {
            mqmParseEdcaParameters(prAdapter, prSwRfb, pucIEStart, u2IELength, TRUE);
        }

        //DBGLOG(QM, EVENT, ("MQM: Assoc_Rsp Parsing (QoS Enabled=%d)\n", prStaRec->fgIsQoS));
        kal_trace(TRACE_GROUP_10 , EVENT_QM_MQM_QOS , prStaRec->fgIsQoS);
        nicQmUpdateWmmParms(prAdapter, (ENUM_NETWORK_TYPE_INDEX_T)prStaRec->ucNetTypeIndex);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* \brief To parse WMM Parameter IE (in BCN or Assoc_Rsp)
*
* \param[in] prAdapter          Adapter pointer
* \param[in] prSwRfb            The received frame
* \param[in] pucIE              The pointer to the first IE in the frame
* \param[in] u2IELength         The total length of IEs in the frame
* \param[in] fgForceOverride    TRUE: If EDCA parameters are found, always set to HW CRs.
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mqmParseEdcaParameters(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb,
    IN PUINT_8     pucIE,
    IN UINT_16     u2IELength,
    IN BOOLEAN     fgForceOverride
)
{
    P_STA_RECORD_T      prStaRec;
    UINT_16             u2Offset;
    UINT_8 aucWfaOui[] = VENDOR_OUI_WFA;
    P_BSS_INFO_T prBssInfo;

    DEBUGFUNC("mqmParseEdcaParameters");

    ASSERT(prSwRfb);
    ASSERT(pucIE);

    prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
    ASSERT(prStaRec);

    if (prStaRec == NULL) {
        return;
    }

    DbgPrint("QM: (fgIsWmmSupported=%d, fgIsQoS=%d)\n",
             prStaRec->fgIsWmmSupported, prStaRec->fgIsQoS);

    if ((!prAdapter->rWifiVar.fgSupportQoS) || (!prStaRec->fgIsWmmSupported) || (!prStaRec->fgIsQoS)) {
        return;
    }

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

    /* Goal: Obtain the EDCA parameters */
    IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
        switch (IE_ID(pucIE)) {
            case ELEM_ID_WMM:

                if ((WMM_IE_OUI_TYPE(pucIE) == VENDOR_OUI_TYPE_WMM) &&
                        (!kalMemCmp(WMM_IE_OUI(pucIE), aucWfaOui, 3))) {

                    switch (WMM_IE_OUI_SUBTYPE(pucIE)) {
                        case VENDOR_OUI_SUBTYPE_WMM_PARAM:
                            if (IE_LEN(pucIE) != 24) {
                                break; /* WMM Param IE with a wrong length */
                            } else {
                                P_AC_QUE_PARMS_T prAcQueParams;
                                P_IE_WMM_PARAM_T prIeWmmParam;
                                UINT_8 eAci;
                                PUINT_8 pucWmmParamSetCount;
                                //int i;

                                pucWmmParamSetCount = &(prBssInfo->ucWmmParamSetCount);

                                prIeWmmParam = (P_IE_WMM_PARAM_T)pucIE;

                                /* Check the Parameter Set Count to determine whether EDCA parameters have been changed */
                                if (!fgForceOverride) {
                                    if (*pucWmmParamSetCount == (prIeWmmParam->ucQosInfo & WMM_QOS_INFO_PARAM_SET_CNT)) {
                                        break; /* Ignore the IE without updating HW CRs */
                                    }
                                }

                                /* Update Parameter Set Count */
                                *pucWmmParamSetCount = (prIeWmmParam->ucQosInfo & WMM_QOS_INFO_PARAM_SET_CNT);

                                /* Update EDCA parameters */
                                for (eAci = 0; eAci < WMM_AC_INDEX_NUM; eAci++) {

                                    prAcQueParams = &prBssInfo->arACQueParms[eAci];
                                    mqmFillAcQueParam(prIeWmmParam, eAci, prAcQueParams);

                                    prAcQueParams->fgIsACMSet =
                                        (prAcQueParams->u2Aifsn & WMM_ACIAIFSN_ACM) ? TRUE : FALSE;
                                    prAcQueParams->u2Aifsn &= WMM_ACIAIFSN_AIFSN;

                                    // DBGLOG(QM, EVENT, ("MQM: eAci = %d, ACM = %d, Aifsn = %d, CWmin = %d, CWmax = %d, TxopLimit = %d\n",
                                    kal_trace(TRACE_GROUP_10 , EVENT_QM_MQM_EACI
                                              ,  eAci, prAcQueParams->fgIsACMSet,
                                              prAcQueParams->u2Aifsn, prAcQueParams->u2CWmin,
                                              prAcQueParams->u2CWmax, prAcQueParams->u2TxopLimit);
                                }
                            }
                            break;

                        default:
                            /* Other WMM QoS IEs. Ignore */
                            break;
                    }

                }
                /* else: VENDOR_OUI_TYPE_WPA, VENDOR_OUI_TYPE_WPS, ... (not cared) */
                break;
            default:
                break;
        }
    }
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is used for parsing EDCA parameters specified in the WMM Parameter IE
*
* \param[in] prAdapter           Adapter pointer
* \param[in] prIeWmmParam        The pointer to the WMM Parameter IE
* \param[in] u4AcOffset          The offset specifying the AC queue for parsing
* \param[in] prHwAcParams        The parameter structure used to configure the HW CRs
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mqmFillAcQueParam(
    IN  P_IE_WMM_PARAM_T prIeWmmParam,
    IN  UINT_32 u4AcOffset,
    OUT P_AC_QUE_PARMS_T prAcQueParams
)
{
    prAcQueParams->u2Aifsn = *((PUINT_8)(&(prIeWmmParam->ucAciAifsn_BE)) + (u4AcOffset * 4));

    prAcQueParams->u2CWmax =
        BIT(((*((PUINT_8)(&(prIeWmmParam->ucEcw_BE)) + (u4AcOffset * 4))) & WMM_ECW_WMAX_MASK)
            >> WMM_ECW_WMAX_OFFSET) - 1;

    prAcQueParams->u2CWmin =
        BIT((*((PUINT_8)(&(prIeWmmParam->ucEcw_BE)) + (u4AcOffset * 4))) & WMM_ECW_WMIN_MASK) - 1;

    WLAN_GET_FIELD_16(((PUINT_8)(&(prIeWmmParam->aucTxopLimit_BE)) + (u4AcOffset * 4)), &(prAcQueParams->u2TxopLimit));

    prAcQueParams->ucGuradTime = TXM_DEFAULT_FLUSH_QUEUE_GUARD_TIME;


}


/*----------------------------------------------------------------------------*/
/*!
* \brief To parse WMM/11n related IEs in scan results (only for AP peers)
*
* \param[in] prAdapter       Adapter pointer
* \param[in]  prScanResult   The scan result which shall be parsed to obtain needed info
* \param[out] prStaRec       The obtained info is stored in the STA_REC
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mqmProcessScanResult(
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prScanResult,
    OUT P_STA_RECORD_T prStaRec
)
{
    PUINT_8     pucIE;
    UINT_16     u2IELength;
    UINT_16     u2Offset;
    UINT_8 aucWfaOui[] = VENDOR_OUI_WFA;

    DEBUGFUNC("mqmProcessScanResult");

    ASSERT(prScanResult);
    ASSERT(prStaRec);

    /* Reset the flag before parsing */
    prStaRec->fgIsWmmSupported = prStaRec->fgIsUapsdSupported = FALSE;

    if (!prAdapter->rWifiVar.fgSupportQoS) {
        return;
    }

    u2IELength = prScanResult->u2IELength;
    pucIE = prScanResult->aucIEBuf;

    /* Goal: Determine whether the peer supports WMM/QoS and UAPSDU */
    IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
        switch (IE_ID(pucIE)) {
            case ELEM_ID_WMM:
                if ((WMM_IE_OUI_TYPE(pucIE) == VENDOR_OUI_TYPE_WMM) &&
                        (!kalMemCmp(WMM_IE_OUI(pucIE), aucWfaOui, 3))) {

                    switch (WMM_IE_OUI_SUBTYPE(pucIE)) {
                        case VENDOR_OUI_SUBTYPE_WMM_PARAM:
                            if (IE_LEN(pucIE) != 24) {
                                break; /* WMM Param IE with a wrong length */
                            } else {
                                prStaRec->fgIsWmmSupported = TRUE;
                                prStaRec->fgIsUapsdSupported = (((((P_IE_WMM_PARAM_T)pucIE)->ucQosInfo) & WMM_QOS_INFO_UAPSD) ? TRUE : FALSE);
                            }
                            break;

                        case VENDOR_OUI_SUBTYPE_WMM_INFO:
                            if (IE_LEN(pucIE) != 7) {
                                break; /* WMM Info IE with a wrong length */
                            } else {
                                prStaRec->fgIsWmmSupported = TRUE;
                                prStaRec->fgIsUapsdSupported = (((((P_IE_WMM_INFO_T)pucIE)->ucQosInfo) & WMM_QOS_INFO_UAPSD) ? TRUE : FALSE);
                            }
                            break;

                        default:
                            /* A WMM QoS IE that doesn't matter. Ignore it. */
                            break;
                    }
                }
                /* else: VENDOR_OUI_TYPE_WPA, VENDOR_OUI_TYPE_WPS, ... (not cared) */

                break;

            default:
                /* A WMM IE that doesn't matter. Ignore it. */
                break;
        }
    }
    //DBGLOG(QM, EVENT, ("MQM: Scan Result Parsing (WMM=%d, UAPSD=%d)\n",
    //    prStaRec->fgIsWmmSupported, prStaRec->fgIsUapsdSupported));
    kal_trace(TRACE_GROUP_10 , EVENT_QM_MQM_SCAN_RESULT, prStaRec->fgIsWmmSupported, prStaRec->fgIsUapsdSupported);

}

UINT_8
qmGetStaRecIdx(
    IN P_ADAPTER_T                  prAdapter,
    IN PUINT_8                      pucEthDestAddr,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetworkType
)
{
    UINT_32 i;
    P_STA_RECORD_T prTempStaRec;

    prTempStaRec = NULL;

    ASSERT(prAdapter);

    //4 <1> DA = BMCAST
    if (IS_BMCAST_MAC_ADDR(pucEthDestAddr)) {
        return STA_REC_INDEX_BMCAST;
    }


    //4 <2> Check if an AP STA is present
    for (i = 0; i < CFG_NUM_OF_STA_RECORD; i++) {
        prTempStaRec = &(prAdapter->arStaRec[i]);
        if ((prTempStaRec->ucNetTypeIndex == eNetworkType)
                && (prTempStaRec->fgIsAp)
                && (prTempStaRec->fgIsValid)) {
            return prTempStaRec->ucIndex;
        }
    }

    //4 <3> Not BMCAST, No AP --> Compare DA (i.e., to see whether this is a unicast frame to a client)
    for (i = 0; i < CFG_NUM_OF_STA_RECORD; i++) {
        prTempStaRec = &(prAdapter->arStaRec[i]);
        if (prTempStaRec->fgIsValid) {
            if (EQUAL_MAC_ADDR(prTempStaRec->aucMacAddr, pucEthDestAddr)) {
                return prTempStaRec->ucIndex;
            }
        }
    }


    //4 <4> No STA found, Not BMCAST --> Indicate NOT_FOUND to FW
    return STA_REC_INDEX_NOT_FOUND;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Generate the WMM Info IE
*
* \param[in] prAdapter  Adapter pointer
* @param prMsduInfo The TX MMPDU
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
mqmGenerateWmmInfoIE(
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsduInfo
)
{
    P_IE_WMM_INFO_T prIeWmmInfo;
    UINT_32 ucUapsd[] = {
        WMM_QOS_INFO_BE_UAPSD,
        WMM_QOS_INFO_BK_UAPSD,
        WMM_QOS_INFO_VI_UAPSD,
        WMM_QOS_INFO_VO_UAPSD
    };
    UINT_8 aucWfaOui[] = VENDOR_OUI_WFA;

    P_PM_PROFILE_SETUP_INFO_T prPmProfSetupInfo;
    P_BSS_INFO_T prBssInfo;
    P_STA_RECORD_T prStaRec;

    DEBUGFUNC("mqmGenerateWmmInfoIE");

    ASSERT(prMsduInfo);

    /* In case QoS is not turned off, exit directly */
    if (!prAdapter->rWifiVar.fgSupportQoS) {
        return;
    }

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
    ASSERT(prStaRec);

    if (prStaRec == NULL) {
        return;
    }

    if (!prStaRec->fgIsWmmSupported) {
        return;
    }

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;

    prIeWmmInfo = (P_IE_WMM_INFO_T)
                  ((PUINT_8) prMsduInfo->prPacket + prMsduInfo->u2FrameLength);

    prIeWmmInfo->ucId = ELEM_ID_WMM;
    prIeWmmInfo->ucLength = ELEM_MAX_LEN_WMM_INFO;

    /* WMM-2.2.1 WMM Information Element Field Values */
    prIeWmmInfo->aucOui[0] = aucWfaOui[0];
    prIeWmmInfo->aucOui[1] = aucWfaOui[1];
    prIeWmmInfo->aucOui[2] = aucWfaOui[2];
    prIeWmmInfo->ucOuiType = VENDOR_OUI_TYPE_WMM;
    prIeWmmInfo->ucOuiSubtype = VENDOR_OUI_SUBTYPE_WMM_INFO;

    prIeWmmInfo->ucVersion = VERSION_WMM;
    prIeWmmInfo->ucQosInfo = 0;

    /* UAPSD intial queue configurations (delivery and trigger enabled)*/
//    if(prAdapter->rWifiVar.fgSupportUAPSD){
    if (prAdapter->rWifiVar.fgSupportUAPSD && prStaRec->fgIsUapsdSupported) {

        UINT_8 ucQosInfo = 0;
        UINT_8 i;


        /* Static U-APSD setting */
        for (i = ACI_BE; i <= ACI_VO; i++) {
            if (prPmProfSetupInfo->ucBmpDeliveryAC &  prPmProfSetupInfo->ucBmpTriggerAC & BIT(i)) {
                ucQosInfo |= (UINT_8)ucUapsd[i];
            }
        }


        if (prPmProfSetupInfo->ucBmpDeliveryAC &  prPmProfSetupInfo->ucBmpTriggerAC) {
            switch (prPmProfSetupInfo->ucUapsdSp) {
                case WMM_MAX_SP_LENGTH_ALL:
                    ucQosInfo |= WMM_QOS_INFO_MAX_SP_ALL;
                    break;

                case WMM_MAX_SP_LENGTH_2:
                    ucQosInfo |= WMM_QOS_INFO_MAX_SP_2;
                    break;

                case WMM_MAX_SP_LENGTH_4:
                    ucQosInfo |= WMM_QOS_INFO_MAX_SP_4;
                    break;

                case WMM_MAX_SP_LENGTH_6:
                    ucQosInfo |= WMM_QOS_INFO_MAX_SP_6;
                    break;

                default:
                    //DBGLOG(QM, EVENT, ("MQM: Incorrect SP length \n"));
                    kal_trace(TRACE_GROUP_10 , EVENT_QM_MQM_INCORRECT_SP_LENGTH);
                    ucQosInfo |= WMM_QOS_INFO_MAX_SP_2;
                    break;
            }
        }
        prIeWmmInfo->ucQosInfo = ucQosInfo;

    }

    /* Increment the total IE length for the Element ID and Length fields. */
    prMsduInfo->u2FrameLength += IE_SIZE(prIeWmmInfo);

}


#if 0
/*----------------------------------------------------------------------------*/
/*!
* @brief log2 calculation for CW
*
* @param[in] val value
*
* @return log2(val)
*/
/*----------------------------------------------------------------------------*/

UINT_32 cwlog2(UINT_32 val)
{

    UINT_32 n;
    n = 0;

    while (val >= 512) {
        n += 9;
        val = val >> 9;
    }
    while (val >= 16) {
        n += 4;
        val >>= 4;
    }
    while (val >= 2) {
        n += 1;
        val >>= 1;
    }
    return n;
}
#endif


/*----------------------------------------------------------------------------*/
/*!
* @brief Generate the WMM Param IE
*
* \param[in] prAdapter  Adapter pointer
* @param prMsduInfo The TX MMPDU
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
mqmGenerateWmmParamIE(
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsduInfo
)
{
    P_IE_WMM_PARAM_T prIeWmmParam;

    UINT_8 aucWfaOui[] = VENDOR_OUI_WFA;

    UINT_8 aucACI[] = {
        WMM_ACI_AC_BE,
        WMM_ACI_AC_BK,
        WMM_ACI_AC_VI,
        WMM_ACI_AC_VO
    };

    P_BSS_INFO_T prBssInfo;
    P_STA_RECORD_T prStaRec;
    UINT_8 eAci;//ENUM_WMM_ACI_T eAci;

    DEBUGFUNC("mqmGenerateWmmParamIE");
    //DBGLOG(QM, LOUD,("\n"));

    ASSERT(prMsduInfo);

    /* In case QoS is not turned off, exit directly */
    if (!prAdapter->rWifiVar.fgSupportQoS) {
        return;
    }

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

    if (prStaRec) {
        if (!prStaRec->fgIsQoS) {
            return;
        }
    }

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType]);

    if (!prBssInfo->fgIsQBSS) {
        return;
    }

    if (prBssInfo->eCurrentOPMode != OP_MODE_ACCESS_POINT &&
            prBssInfo->eCurrentOPMode != OP_MODE_BOW) {
        return;
    }

    prIeWmmParam = (P_IE_WMM_PARAM_T)
                   ((PUINT_8) prMsduInfo->prPacket + prMsduInfo->u2FrameLength);

    prIeWmmParam->ucId = ELEM_ID_WMM;
    prIeWmmParam->ucLength = ELEM_MAX_LEN_WMM_PARAM;

    /* WMM-2.2.1 WMM Information Element Field Values */
    prIeWmmParam->aucOui[0] = aucWfaOui[0];
    prIeWmmParam->aucOui[1] = aucWfaOui[1];
    prIeWmmParam->aucOui[2] = aucWfaOui[2];
    prIeWmmParam->ucOuiType = VENDOR_OUI_TYPE_WMM;
    prIeWmmParam->ucOuiSubtype = VENDOR_OUI_SUBTYPE_WMM_PARAM;

    prIeWmmParam->ucVersion = VERSION_WMM;
    prIeWmmParam->ucQosInfo = (prBssInfo->ucWmmParamSetCount & WMM_QOS_INFO_PARAM_SET_CNT);

    /* UAPSD intial queue configurations (delivery and trigger enabled)*/
    if (prAdapter->rWifiVar.fgSupportUAPSD) {

        prIeWmmParam->ucQosInfo |=  WMM_QOS_INFO_UAPSD;

    }

    /* EDCA parameter */

    for (eAci = 0; eAci < WMM_AC_INDEX_NUM; eAci++) {

        //DBGLOG(QM, LOUD, ("MQM: eAci = %d, ACM = %d, Aifsn = %d, CWmin = %d, CWmax = %d, TxopLimit = %d\n",
        //           eAci,prBssInfo->arACQueParmsForBcast[eAci].fgIsACMSet ,
        //           prBssInfo->arACQueParmsForBcast[eAci].u2Aifsn,
        //           prBssInfo->arACQueParmsForBcast[eAci].u2CWmin,
        //           prBssInfo->arACQueParmsForBcast[eAci].u2CWmax,
        //           prBssInfo->arACQueParmsForBcast[eAci].u2TxopLimit));

        *(((PUINT_8)(&prIeWmmParam->ucAciAifsn_BE)) + (eAci << 2)) = (UINT_8)(aucACI[eAci]
                | (prBssInfo->arACQueParmsForBcast[eAci].fgIsACMSet ? WMM_ACIAIFSN_ACM : 0)
                | (prBssInfo->arACQueParmsForBcast[eAci].u2Aifsn & (WMM_ACIAIFSN_AIFSN)));
#if 1
        *(((PUINT_8)(&prIeWmmParam->ucEcw_BE)) + (eAci << 2)) = (UINT_8)(0
                | (((prBssInfo->aucCWminLog2ForBcast[eAci])) & WMM_ECW_WMIN_MASK)
                | ((((prBssInfo->aucCWmaxLog2ForBcast[eAci])) << WMM_ECW_WMAX_OFFSET) & WMM_ECW_WMAX_MASK)
                                                                        );
#else
        *(((PUINT_8)(&prIeWmmParam->ucEcw_BE)) + (eAci << 2)) = (UINT_8)(0
                | (cwlog2((prBssInfo->arACQueParmsForBcast[eAci].u2CWmin + 1)) & WMM_ECW_WMIN_MASK)
                | ((cwlog2((prBssInfo->arACQueParmsForBcast[eAci].u2CWmax + 1)) << WMM_ECW_WMAX_OFFSET) & WMM_ECW_WMAX_MASK)
                                                                        );
#endif

        WLAN_SET_FIELD_16(((PUINT_8)(prIeWmmParam->aucTxopLimit_BE)) + (eAci << 2)
                          , prBssInfo->arACQueParmsForBcast[eAci].u2TxopLimit);

    }

    /* Increment the total IE length for the Element ID and Length fields. */
    prMsduInfo->u2FrameLength += IE_SIZE(prIeWmmParam);

}




ENUM_FRAME_ACTION_T
qmGetFrameAction(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetworkType,
    IN UINT_8                       ucStaRecIdx,
    IN P_MSDU_INFO_T                prMsduInfo,
    IN ENUM_FRAME_TYPE_IN_CMD_Q_T   eFrameType
)
{
    P_BSS_INFO_T   prBssInfo;
    P_STA_RECORD_T prStaRec;
    P_WLAN_MAC_HEADER_T prWlanFrame;
    UINT_16        u2TxFrameCtrl;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "qmGetFrameAction");
    ASSERT(prAdapter);
#if (NIC_TX_BUFF_COUNT_TC4 > 2)
#define QM_MGMT_QUUEUD_THRESHOLD 2
#else
#define QM_MGMT_QUUEUD_THRESHOLD 1
#endif

    DATA_STRUC_INSPECTING_ASSERT(QM_MGMT_QUUEUD_THRESHOLD <= (NIC_TX_BUFF_COUNT_TC4));
    DATA_STRUC_INSPECTING_ASSERT(QM_MGMT_QUUEUD_THRESHOLD  > 0);

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[eNetworkType]);
    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, ucStaRecIdx);

    //dumpMemory8(prBssInfo,16);
    //dumpMemory8(prStaRec,16);
    /* XXX Check BOW P2P AIS time ot set active */
    if (!IS_BSS_ACTIVE(prBssInfo)) {
        if (eFrameType == FRAME_TYPE_MMPDU) {
            prWlanFrame = (P_WLAN_MAC_HEADER_T) prMsduInfo->prPacket;
            u2TxFrameCtrl = (prWlanFrame->u2FrameCtrl) & MASK_FRAME_TYPE; // Optimized for ARM
            if ((u2TxFrameCtrl == MAC_FRAME_DEAUTH) && (prMsduInfo->pfTxDoneHandler == NULL)) {
                return FRAME_ACTION_TX_PKT;
            }

        }

        // DBGLOG(QM, WARN, ("Drop packets (Inactive %u).\n",prBssInfo->ucNetTypeIndex));
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_111, "Drop packets (Inactive %u).\n", prBssInfo->ucNetTypeIndex);
        return FRAME_ACTION_DROP_PKT;
    }

    /* TODO Handle disconnect issue */

    /* P2P probe Request frame */
    do {
        if (eFrameType == FRAME_TYPE_MMPDU) {
            ASSERT(prMsduInfo != NULL);
            prWlanFrame = (P_WLAN_MAC_HEADER_T) prMsduInfo->prPacket;
            u2TxFrameCtrl = (prWlanFrame->u2FrameCtrl) & MASK_FRAME_TYPE; // Optimized for ARM

            if (u2TxFrameCtrl == MAC_FRAME_BEACON) {
                if (prBssInfo->fgIsNetAbsent) {
                    return FRAME_ACTION_DROP_PKT;
                }
            } else if (u2TxFrameCtrl == MAC_FRAME_PROBE_RSP) {
                if (prBssInfo->fgIsNetAbsent) {
                    break;
                }
            } else if (u2TxFrameCtrl == MAC_FRAME_DEAUTH) {
                DBGLOG(P2P, TRACE, ("DEAUTH Frame\n"));
                if (prBssInfo->fgIsNetAbsent) {
                    break;
                }
                DBGLOG(P2P, TRACE, ("Sending DEAUTH Frame\n"));
                return FRAME_ACTION_TX_PKT;
            }
            /* MMPDU with prStaRec && fgIsInUse not check fgIsNetActive */
            else if (u2TxFrameCtrl == MAC_FRAME_ASSOC_REQ
                     || u2TxFrameCtrl == MAC_FRAME_AUTH
                     || u2TxFrameCtrl == MAC_FRAME_REASSOC_REQ
                     || u2TxFrameCtrl == MAC_FRAME_PROBE_REQ
                     || u2TxFrameCtrl == MAC_FRAME_ACTION) {

                if (prStaRec) {
                    if (prStaRec->fgIsInPS) {
                        if (nicTxGetResource(prAdapter, TC4_INDEX) >= QM_MGMT_QUUEUD_THRESHOLD) {
                            return FRAME_ACTION_TX_PKT;
                        } else {
                            return FRAME_ACTION_QUEUE_PKT;
                        }
                    }
                }
                return FRAME_ACTION_TX_PKT;
            }

            if (!prStaRec) {
                return FRAME_ACTION_TX_PKT;
            } else {
                if (!prStaRec->fgIsInUse) {
                    return FRAME_ACTION_DROP_PKT;
                }
            }

        } /* FRAME_TYPE_MMPDU */
        else if ((eFrameType == FRAME_TYPE_802_1X)) {

            if (!prStaRec) {
                return FRAME_ACTION_TX_PKT;
            } else {
                if (!prStaRec->fgIsInUse) {
                    return FRAME_ACTION_DROP_PKT;
                }
                if (prStaRec->fgIsInPS) {
                    if (nicTxGetResource(prAdapter, TC4_INDEX) >= QM_MGMT_QUUEUD_THRESHOLD) {
                        return FRAME_ACTION_TX_PKT;
                    } else {
                        return FRAME_ACTION_QUEUE_PKT;
                    }
                }
            }

        } /* FRAME_TYPE_802_1X */
        else if ((!IS_BSS_ACTIVE(prBssInfo))
                 || (!prStaRec)
                 || (!prStaRec->fgIsInUse)) {
            return FRAME_ACTION_DROP_PKT;
        }
    } while (0);

    if (prBssInfo->fgIsNetAbsent) {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_112, "Queue packets (Absent %u).\n", prBssInfo->ucNetTypeIndex);
        return FRAME_ACTION_QUEUE_PKT;
    }

    if (prStaRec && prStaRec->fgIsInPS) {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_113, "Queue packets (PS %u).\n", prStaRec->fgIsInPS);
        return FRAME_ACTION_QUEUE_PKT;
    } else {
        switch (eFrameType) {
            case FRAME_TYPE_802_1X:
                if (!prStaRec->fgIsValid) {
                    MT5931_TRACE(TRACE_WARNING, MT5931_INFO_114, "Queue packets (IsValid %u).\n", prStaRec->fgIsValid);
                    dumpSTA(prAdapter, prStaRec);
                    return FRAME_ACTION_QUEUE_PKT;
                }
                break;

            case FRAME_TYPE_MMPDU:
                break;

            default:
                ASSERT(0);
        }
    }

    return FRAME_ACTION_TX_PKT;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Handle BSS change operation Event from the FW
*
* \param[in] prAdapter Adapter pointer
* \param[in] prEvent The event packet from the FW
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmHandleEventBssAbsencePresence(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
)
{
    P_EVENT_BSS_ABSENCE_PRESENCE_T prEventBssStatus;
    P_BSS_INFO_T prBssInfo;
    BOOLEAN fgIsNetAbsentOld;

    prEventBssStatus = (P_EVENT_BSS_ABSENCE_PRESENCE_T)prEvent;
    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prEventBssStatus->ucNetTypeIdx]);
    fgIsNetAbsentOld = prBssInfo->fgIsNetAbsent;
    prBssInfo->fgIsNetAbsent = prEventBssStatus->fgIsAbsent;
    prBssInfo->ucBssFreeQuota = prEventBssStatus->ucBssFreeQuota;

    MT5931_TRACE(TRACE_INFO, MT5931_INFO_115, "[EVENT_ID_BSS_ABSENCE_PRESENCE]ucNetTypeIdx %d fgIsAbsent %d ucBssFreeQuota %d",
                 prEventBssStatus->ucNetTypeIdx, prEventBssStatus->fgIsAbsent, prEventBssStatus->ucBssFreeQuota);

    if (!prBssInfo->fgIsNetAbsent) {
        QM_DBG_CNT_INC(&(prAdapter->rQM), QM_DBG_CNT_27);
    } else {
        QM_DBG_CNT_INC(&(prAdapter->rQM), QM_DBG_CNT_28);
    }
    /* From Absent to Present */
    if ((fgIsNetAbsentOld) && (!prBssInfo->fgIsNetAbsent)) {
        kalSetEvent(prAdapter->prGlueInfo);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Handle STA change PS mode Event from the FW
*
* \param[in] prAdapter Adapter pointer
* \param[in] prEvent The event packet from the FW
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmHandleEventStaChangePsMode(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
)
{
    P_EVENT_STA_CHANGE_PS_MODE_T prEventStaChangePsMode;
    P_STA_RECORD_T prStaRec;
    BOOLEAN fgIsInPSOld;

    //DbgPrint("QM:Event -RxBa\n");

    prEventStaChangePsMode = (P_EVENT_STA_CHANGE_PS_MODE_T)prEvent;
    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, prEventStaChangePsMode->ucStaRecIdx);
    //ASSERT(prStaRec);

    if (prStaRec) {

        fgIsInPSOld = prStaRec->fgIsInPS;
        prStaRec->fgIsInPS = prEventStaChangePsMode->fgIsInPs;

        qmUpdateFreeQuota(
            prAdapter,
            prStaRec,
            prEventStaChangePsMode->ucUpdateMode,
            prEventStaChangePsMode->ucFreeQuota);

        //DBGLOG(QM, EVENT, ("qmHandleEventStaChangePsMode (ucStaRecIdx=%d, fgIsInPs=%d)\n",
        //    prEventStaChangePsMode->ucStaRecIdx, prStaRec->fgIsInPS));
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_116, "qmHandleEventStaChangePsMode (ucStaRecIdx=%d, fgIsInPs=%d)\n",
                     prEventStaChangePsMode->ucStaRecIdx, prStaRec->fgIsInPS);

        /* From PS to Awake */
        if ((fgIsInPSOld) && (!prStaRec->fgIsInPS)) {
            kalSetEvent(prAdapter->prGlueInfo);
        }
    } else {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_117, "qmHandleEventStaChangePsMode: [%d] check if disconnected", prEventStaChangePsMode->ucStaRecIdx);
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Update STA free quota Event from FW
*
* \param[in] prAdapter Adapter pointer
* \param[in] prEvent The event packet from the FW
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmHandleEventStaUpdateFreeQuota(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
)
{
    P_EVENT_STA_UPDATE_FREE_QUOTA_T prEventStaUpdateFreeQuota;
    P_STA_RECORD_T prStaRec;

    DEBUGFUNC("qmHandleEventStaUpdateFreeQuota");
    prEventStaUpdateFreeQuota = (P_EVENT_STA_UPDATE_FREE_QUOTA_T)prEvent;
    prStaRec = QM_GET_STA_REC_PTR_FROM_INDEX(prAdapter, prEventStaUpdateFreeQuota->ucStaRecIdx);
    ASSERT(prStaRec);

    if (prStaRec) {
        if (prStaRec->fgIsInPS) {
            qmUpdateFreeQuota(
                prAdapter,
                prStaRec,
                prEventStaUpdateFreeQuota->ucUpdateMode,
                prEventStaUpdateFreeQuota->ucFreeQuota);

            kalSetEvent(prAdapter->prGlueInfo);
        }

        //DBGLOG(QM, EVENT, ("qmHandleEventStaUpdateFreeQuota (ucStaRecIdx=%d, ucUpdateMode=%d, ucFreeQuota=%d)\n",
        //     prEventStaUpdateFreeQuota->ucStaRecIdx,
        //     prEventStaUpdateFreeQuota->ucUpdateMode,
        //     prEventStaUpdateFreeQuota->ucFreeQuota));
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_118, "qmHandleEventStaUpdateFreeQuota (ucStaRecIdx=%d, ucUpdateMode=%d, ucFreeQuota=%d)\n",
                     prEventStaUpdateFreeQuota->ucStaRecIdx,
                     prEventStaUpdateFreeQuota->ucUpdateMode,
                     prEventStaUpdateFreeQuota->ucFreeQuota);

    }

}


/*----------------------------------------------------------------------------*/
/*!
* \brief Update STA free quota
*
* \param[in] prStaRec the STA
* \param[in] ucUpdateMode the method to update free quota
* \param[in] ucFreeQuota  the value for update
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
qmUpdateFreeQuota(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN UINT_8 ucUpdateMode,
    IN UINT_8 ucFreeQuota
)
{

    UINT_8 ucFreeQuotaForNonDelivery;
    UINT_8 ucFreeQuotaForDelivery;

    ASSERT(prStaRec);
    // DBGLOG(QM, LOUD, ("qmUpdateFreeQuota orig ucFreeQuota=%d Mode %u New %u\n",
    //     prStaRec->ucFreeQuota, ucUpdateMode, ucFreeQuota ));
    kal_trace(TRACE_GROUP_10 , LOUD_QM_orig_ucFreeQuota , prStaRec->ucFreeQuota, ucUpdateMode, ucFreeQuota);

    if (!prStaRec->fgIsInPS) {
        return;
    }

    switch (ucUpdateMode) {
        case FREE_QUOTA_UPDATE_MODE_INIT:
        case FREE_QUOTA_UPDATE_MODE_OVERWRITE:
            prStaRec->ucFreeQuota = ucFreeQuota;
            break;
        case FREE_QUOTA_UPDATE_MODE_INCREASE:
            prStaRec->ucFreeQuota += ucFreeQuota;
            break;
        case FREE_QUOTA_UPDATE_MODE_DECREASE:
            prStaRec->ucFreeQuota -= ucFreeQuota;
            break;
        default:
            ASSERT(0);
    }

    //DBGLOG(QM, LOUD, ("qmUpdateFreeQuota new ucFreeQuota=%d)\n",
    //    prStaRec->ucFreeQuota ));
    kal_trace(TRACE_GROUP_10 , LOUD_QM_new_ucFreeQuota , prStaRec->ucFreeQuota);

    ucFreeQuota = prStaRec->ucFreeQuota;

    ucFreeQuotaForNonDelivery = 0;
    ucFreeQuotaForDelivery = 0;

    if (ucFreeQuota > 0) {
        if (prStaRec->fgIsQoS && prStaRec->fgIsUapsdSupported
                /* && prAdapter->rWifiVar.fgSupportQoS
                 && prAdapter->rWifiVar.fgSupportUAPSD*/) {
            /* XXX We should assign quota to aucFreeQuotaPerQueue[NUM_OF_PER_STA_TX_QUEUES]  */

            if (prStaRec->ucFreeQuotaForNonDelivery > 0  && prStaRec->ucFreeQuotaForDelivery > 0) {
                ucFreeQuotaForNonDelivery = ucFreeQuota >> 1;
                ucFreeQuotaForDelivery =  ucFreeQuota - ucFreeQuotaForNonDelivery;
            } else if (prStaRec->ucFreeQuotaForNonDelivery == 0  && prStaRec->ucFreeQuotaForDelivery == 0) {
                ucFreeQuotaForNonDelivery =  ucFreeQuota >> 1;
                ucFreeQuotaForDelivery =  ucFreeQuota - ucFreeQuotaForNonDelivery;
            } else if (prStaRec->ucFreeQuotaForNonDelivery > 0) {
                /* NonDelivery is not busy */
                if (ucFreeQuota >= 3) {
                    ucFreeQuotaForNonDelivery = 2;
                    ucFreeQuotaForDelivery = ucFreeQuota - ucFreeQuotaForNonDelivery ;
                } else {
                    ucFreeQuotaForDelivery = ucFreeQuota;
                    ucFreeQuotaForNonDelivery = 0;
                }
            } else if (prStaRec->ucFreeQuotaForDelivery > 0) {
                /* Delivery is not busy */
                if (ucFreeQuota >= 3) {
                    ucFreeQuotaForDelivery = 2;
                    ucFreeQuotaForNonDelivery = ucFreeQuota - ucFreeQuotaForDelivery;
                } else {
                    ucFreeQuotaForNonDelivery = ucFreeQuota;
                    ucFreeQuotaForDelivery = 0;
                }
            }

        } else {
            /* !prStaRec->fgIsUapsdSupported */
            ucFreeQuotaForNonDelivery = ucFreeQuota;
            ucFreeQuotaForDelivery = 0;
        }
    } /* ucFreeQuota > 0 */

    prStaRec->ucFreeQuotaForDelivery =  ucFreeQuotaForDelivery;
    prStaRec->ucFreeQuotaForNonDelivery =  ucFreeQuotaForNonDelivery;

    // DBGLOG(QM, LOUD, ("new QuotaForDelivery = %d  QuotaForNonDelivery = %d\n",
    //     prStaRec->ucFreeQuotaForDelivery, prStaRec->ucFreeQuotaForNonDelivery ));
    kal_trace(TRACE_GROUP_10 , LOUD_QM_new_QuotaForDelivery , prStaRec->ucFreeQuotaForDelivery, prStaRec->ucFreeQuotaForNonDelivery);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Return the reorder queued RX packets
*
* \param[in] (none)
*
* \return The number of queued RX packets
*/
/*----------------------------------------------------------------------------*/
UINT_32
qmGetRxReorderQueuedBufferCount(
    IN P_ADAPTER_T  prAdapter
)
{
    UINT_32 i, u4Total;
    P_QUE_MGT_T prQM = &prAdapter->rQM;
    u4Total = 0;
    /* XXX The summation may impact the performance */
    for (i = 0; i < CFG_NUM_OF_RX_BA_AGREEMENTS; i++) {
        u4Total += prQM->arRxBaTable[i].rReOrderQue.u4NumElem;
#if DBG && 0
        if (QUEUE_IS_EMPTY(&(prQM->arRxBaTable[i].rReOrderQue))) {
            ASSERT(prQM->arRxBaTable[i].rReOrderQue == 0);
        }
#endif
    }
    ASSERT(u4Total <= (CFG_NUM_OF_QM_RX_PKT_NUM * 2));
    return u4Total;
}


