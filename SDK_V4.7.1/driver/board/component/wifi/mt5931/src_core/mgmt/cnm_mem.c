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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/cnm_mem.c#24 $
*/

/*! \file   "cnm_mem.c"
    \brief  This file contain the management function of packet buffers and
            generic memory alloc/free functioin for mailbox message.

            A data packet has a fixed size of buffer, but a management
            packet can be equipped with a variable size of buffer.
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
static VOID
cnmStaRecHandleEventPkt(
    P_ADAPTER_T     prAdapter,
    P_CMD_INFO_T    prCmdInfo,
    PUINT_8         pucEventBuf
);

static VOID
cnmStaSendUpdateCmd(
    P_ADAPTER_T     prAdapter,
    P_STA_RECORD_T  prStaRec,
    BOOLEAN         fgNeedResp
);

static VOID
cnmStaSendRemoveCmd(
    P_ADAPTER_T     prAdapter,
    P_STA_RECORD_T  prStaRec
);

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
P_MSDU_INFO_T
cnmMgtPktAlloc(
    P_ADAPTER_T     prAdapter,
    UINT_32         u4Length
)
{
    P_MSDU_INFO_T   prMsduInfo;
    P_QUE_T         prQueList;

    //MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"cnmMgtPktAlloc");
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    prQueList = &prAdapter->rTxCtrl.rFreeMsduInfoList;

    /* Get a free MSDU_INFO_T */
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
    QUEUE_REMOVE_HEAD(prQueList, prMsduInfo, P_MSDU_INFO_T);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);

    if (prMsduInfo) {
        prMsduInfo->prPacket = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, u4Length);
        prMsduInfo->eSrc = TX_PACKET_MGMT;

        if (prMsduInfo->prPacket == NULL) {
            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
            QUEUE_INSERT_TAIL(prQueList, &prMsduInfo->rQueEntry);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
            prMsduInfo = NULL;
        }
    }

#if 1
    if (prMsduInfo == NULL) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "cnmMgtPktAlloc():FreeMsduInfoList.Num%d\n", prQueList->u4NumElem);

#if CFG_DBG_MGT_BUF
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "rMgtBufInfo: alloc#=%ld, free#=%ld, null#=%ld\n",
                         prAdapter->rMgtBufInfo.u4AllocCount,
                         prAdapter->rMgtBufInfo.u4FreeCount,
                         prAdapter->rMgtBufInfo.u4AllocNullCount);
#endif

    }
#endif

    return prMsduInfo;
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
cnmMgtPktFree(
    P_ADAPTER_T     prAdapter,
    P_MSDU_INFO_T   prMsduInfo
)
{
    P_QUE_T         prQueList;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    ASSERT(prMsduInfo);

    prQueList = &prAdapter->rTxCtrl.rFreeMsduInfoList;

    ASSERT(prMsduInfo->prPacket);
    if (prMsduInfo->prPacket) {
        cnmMemFree(prAdapter, prMsduInfo->prPacket);
        prMsduInfo->prPacket = NULL;
    }

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
    QUEUE_INSERT_TAIL(prQueList, &prMsduInfo->rQueEntry)
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is used to initial the MGMT/MSG memory pool.
*
* \param (none)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmMemInit(
    P_ADAPTER_T     prAdapter
)
{
    P_BUF_INFO_T    prBufInfo;

    /* Initialize Management buffer pool */
    prBufInfo = &prAdapter->rMgtBufInfo;
    kalMemZero(prBufInfo, sizeof(prAdapter->rMgtBufInfo));
    prBufInfo->pucBuf = prAdapter->pucMgtBufCached;

    /* Setup available memory blocks. 1 indicates FREE */
    prBufInfo->rFreeBlocksBitmap =
        (BUF_BITMAP) BITS(0, MAX_NUM_OF_BUF_BLOCKS - 1);


    /* Initialize Message buffer pool */
    prBufInfo = &prAdapter->rMsgBufInfo;
    kalMemZero(prBufInfo, sizeof(prAdapter->rMsgBufInfo));
    prBufInfo->pucBuf = &prAdapter->aucMsgBuf[0];

    /* Setup available memory blocks. 1 indicates FREE */
    prBufInfo->rFreeBlocksBitmap =
        (BUF_BITMAP) BITS(0, MAX_NUM_OF_BUF_BLOCKS - 1);

    return;

} /* end of cnmMemInit() */


/*----------------------------------------------------------------------------*/
/*!
* \brief Allocate MGMT/MSG memory pool.
*
* \param[in] eRamType       Target RAM type.
*                           TCM blk_sz= 16bytes, BUF blk_sz= 256bytes
* \param[in] u4Length       Length of the buffer to allocate.
*
* \retval !NULL    Pointer to the start address of allocated memory.
* \retval NULL     Fail to allocat memory
*/
/*----------------------------------------------------------------------------*/
PVOID
cnmMemAlloc(
    IN P_ADAPTER_T      prAdapter,
    IN ENUM_RAM_TYPE_T  eRamType,
    IN UINT_32          u4Length
)
{
    P_BUF_INFO_T        prBufInfo;
    BUF_BITMAP          rRequiredBitmap;
    UINT_32             u4BlockNum;
    UINT_32             i, u4BlkSzInPower;
    PVOID               pvMemory;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    ASSERT(u4Length);

    u4Length += RESEVED_BUFFER_FOR_TX_PAD;

    if (eRamType == RAM_TYPE_MSG && u4Length <= 256) {
        prBufInfo = &prAdapter->rMsgBufInfo;
        u4BlkSzInPower = MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2;

        u4Length += (MSG_BUF_BLOCK_SIZE - 1);
        u4BlockNum = u4Length >> MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2;

        ASSERT(u4BlockNum <= MAX_NUM_OF_BUF_BLOCKS);
    } else {
        eRamType = RAM_TYPE_BUF;

        prBufInfo = &prAdapter->rMgtBufInfo;
        u4BlkSzInPower = MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2;

        u4Length += (MGT_BUF_BLOCK_SIZE - 1);
        u4BlockNum = u4Length >> MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2;

        ASSERT(u4BlockNum <= MAX_NUM_OF_BUF_BLOCKS);
    }

#if CFG_DBG_MGT_BUF
    prBufInfo->u4AllocCount++;
#endif

    KAL_ACQUIRE_SPIN_LOCK(prAdapter,
                          eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

    if ((u4BlockNum > 0) && (u4BlockNum <= MAX_NUM_OF_BUF_BLOCKS)) {

        /* Convert number of block into bit cluster */
        rRequiredBitmap = BITS(0, u4BlockNum - 1);

        for (i = 0; i <= (MAX_NUM_OF_BUF_BLOCKS - u4BlockNum); i++) {

            /* Have available memory blocks */
            if ((prBufInfo->rFreeBlocksBitmap & rRequiredBitmap)
                    == rRequiredBitmap) {

                /* Clear corresponding bits of allocated memory blocks */
                prBufInfo->rFreeBlocksBitmap &= ~rRequiredBitmap;

                /* Store how many blocks be allocated */
                prBufInfo->aucAllocatedBlockNum[i] = (UINT_8) u4BlockNum;

                KAL_RELEASE_SPIN_LOCK(prAdapter,
                                      eRamType == RAM_TYPE_MSG ?
                                      SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

                /* Return the start address of allocated memory */
                return (PVOID)(prBufInfo->pucBuf + (i << u4BlkSzInPower));

            }

            rRequiredBitmap <<= 1;
        }
    }

#if defined(LINUX) //|| defined(WCN_MAUI)   /* NOTE(Nelson): Do not alloc memory from control buffer */
    pvMemory = (PVOID)kalMemAlloc(u4Length, VIR_MEM_TYPE);
#else
    pvMemory = (PVOID)NULL;
#endif

#if CFG_DBG_MGT_BUF
    prBufInfo->u4AllocNullCount++;

    if (pvMemory) {
        prAdapter->u4MemAllocDynamicCount++;
    }
#endif

    KAL_RELEASE_SPIN_LOCK(prAdapter,
                          eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

    return pvMemory;

} /* end of cnmMemAlloc() */


/*----------------------------------------------------------------------------*/
/*!
* \brief Release memory to MGT/MSG memory pool.
*
* \param pucMemory  Start address of previous allocated memory
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmMemFree(
    IN P_ADAPTER_T      prAdapter,
    IN PVOID            pvMemory
)
{
    P_BUF_INFO_T        prBufInfo;
    UINT_32             u4BlockIndex;
    BUF_BITMAP          rAllocatedBlocksBitmap;
    ENUM_RAM_TYPE_T     eRamType;
    KAL_SPIN_LOCK_DECLARATION();


    ASSERT(prAdapter);
    ASSERT(pvMemory);
    if (!pvMemory) {
        return;
    }

    /* Judge it belongs to which RAM type */
    if (((UINT_32)pvMemory >= (UINT_32)&prAdapter->aucMsgBuf[0]) &&
            ((UINT_32)pvMemory <= (UINT_32)&prAdapter->aucMsgBuf[MSG_BUFFER_SIZE - 1])) {

        prBufInfo = &prAdapter->rMsgBufInfo;
        u4BlockIndex = ((UINT_32)pvMemory - (UINT_32)prBufInfo->pucBuf)
                       >> MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2;
        ASSERT(u4BlockIndex < MAX_NUM_OF_BUF_BLOCKS);
        eRamType = RAM_TYPE_MSG;
    } else if (((UINT_32)pvMemory >= (UINT_32)prAdapter->pucMgtBufCached) &&
               ((UINT_32)pvMemory <= ((UINT_32)prAdapter->pucMgtBufCached + MGT_BUFFER_SIZE - 1))) {
        prBufInfo = &prAdapter->rMgtBufInfo;
        u4BlockIndex = ((UINT_32)pvMemory - (UINT_32)prBufInfo->pucBuf)
                       >> MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2;
        ASSERT(u4BlockIndex < MAX_NUM_OF_BUF_BLOCKS);
        eRamType = RAM_TYPE_BUF;
    } else {
#if defined(LINUX) //|| defined(WCN_MAUI)   /* NOTE(Nelson): Do not alloc memory from control buffer */
        /* For Linux/MAUI, it is supported because size is not needed */
        kalMemFree(pvMemory, VIR_MEM_TYPE, 0);
#else
        /* For Windows, it is not supported because of no size argument */
        ASSERT(0);
#endif

#if CFG_DBG_MGT_BUF
        prAdapter->u4MemFreeDynamicCount++;
#endif
        return;
    }

    KAL_ACQUIRE_SPIN_LOCK(prAdapter,
                          eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

#if CFG_DBG_MGT_BUF
    prBufInfo->u4FreeCount++;
#endif

    /* Convert number of block into bit cluster */
    ASSERT(prBufInfo->aucAllocatedBlockNum[u4BlockIndex] > 0);

    rAllocatedBlocksBitmap =
        BITS(0, prBufInfo->aucAllocatedBlockNum[u4BlockIndex] - 1);
    rAllocatedBlocksBitmap <<= u4BlockIndex;

    /* Clear saved block count for this memory segment */
    prBufInfo->aucAllocatedBlockNum[u4BlockIndex] = 0;

    /* Set corresponding bit of released memory block */
    prBufInfo->rFreeBlocksBitmap |= rAllocatedBlocksBitmap;

    KAL_RELEASE_SPIN_LOCK(prAdapter,
                          eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

    return;

} /* end of cnmMemFree() */


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
cnmStaRecInit(
    P_ADAPTER_T     prAdapter
)
{
    P_STA_RECORD_T  prStaRec;
    UINT_16         i;

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = &prAdapter->arStaRec[i];

        prStaRec->ucIndex = (UINT_8) i;
        prStaRec->fgIsInUse = FALSE;
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
cnmStaRecUninit(
    IN P_ADAPTER_T  prAdapter
)
{
    P_STA_RECORD_T  prStaRec;
    UINT_16         i;

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = &prAdapter->arStaRec[i];

        if (prStaRec->fgIsInUse) {
            cnmStaRecFree(prAdapter, prStaRec, FALSE);
        }
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
P_STA_RECORD_T
cnmStaRecAlloc(
    P_ADAPTER_T     prAdapter,
    UINT_8          ucNetTypeIndex
)
{
    P_STA_RECORD_T  prStaRec;
    UINT_16         i, k;
    //  DEBUGFUNC("cnmStaRecAlloc");
    ASSERT(prAdapter);

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = &prAdapter->arStaRec[i];

        if (!prStaRec->fgIsInUse) {
            /*---- Initialize STA_REC_T here ----*/
            kalMemZero(prStaRec, sizeof(STA_RECORD_T));
            prStaRec->ucIndex = (UINT_8) i;
            prStaRec->ucNetTypeIndex = ucNetTypeIndex;
            prStaRec->fgIsInUse = TRUE;

            if (prStaRec->pucAssocReqIe) {
                kalMemFree(prStaRec->pucAssocReqIe, VIR_MEM_TYPE, prStaRec->u2AssocReqIeLen);
                prStaRec->pucAssocReqIe = NULL;
                prStaRec->u2AssocReqIeLen = 0;
            }

            /* Initialize the SN caches for duplicate detection */
            for (k = 0; k < TID_NUM + 1; k++) {
                prStaRec->au2CachedSeqCtrl[k] = 0xFFFF;
            }

            /* Initialize SW TX queues in STA_REC */
            for (k = 0; k < STA_WAIT_QUEUE_NUM; k++) {
                LINK_INITIALIZE(&prStaRec->arStaWaitQueue[k]);
            }

            /* Default enable TX/RX AMPDU */
            prStaRec->fgTxAmpduEn = TRUE;
            prStaRec->fgRxAmpduEn = TRUE;

            for (k = 0; k < NUM_OF_PER_STA_TX_QUEUES; k++) {
                QUEUE_INITIALIZE(&prStaRec->arTxQueue[k]);
            }

            break;
        }
    }

    return (i < CFG_STA_REC_NUM) ? prStaRec : NULL;
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
cnmStaRecFree(
    P_ADAPTER_T     prAdapter,
    P_STA_RECORD_T  prStaRec,
    BOOLEAN         fgSyncToChip
)
{
    ASSERT(prAdapter);
    ASSERT(prStaRec);

    /* To do: free related resources, e.g. timers, buffers, etc */
    cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);
    prStaRec->fgTransmitKeyExist = FALSE;
    prStaRec->fgSetPwrMgtBit = FALSE;

    if (prStaRec->pucAssocReqIe) {
        kalMemFree(prStaRec->pucAssocReqIe, VIR_MEM_TYPE, prStaRec->u2AssocReqIeLen);
        prStaRec->pucAssocReqIe = NULL;
        prStaRec->u2AssocReqIeLen = 0;
    }

    qmDeactivateStaRec(prAdapter, prStaRec->ucIndex);

    if (fgSyncToChip) {
        cnmStaSendRemoveCmd(prAdapter, prStaRec);
    }
    if (prStaRec->prChallengeText != NULL) {
        cnmMemFree(prAdapter, (void *)prStaRec->prChallengeText);
        prStaRec->prChallengeText = NULL;
    }

    prStaRec->fgIsInUse = FALSE;

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
cnmStaFreeAllStaByNetType(
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex,
    BOOLEAN                     fgSyncToChip
)
{
    P_STA_RECORD_T  prStaRec;
    UINT_16         i;

    MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_47, "cnmStaFreeAllStaByNetType");

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = (P_STA_RECORD_T) &prAdapter->arStaRec[i];

        if (prStaRec->fgIsInUse &&
                prStaRec->ucNetTypeIndex == (UINT_8) eNetTypeIndex) {

            cnmStaRecFree(prAdapter, prStaRec, fgSyncToChip);
        }
    } /* end of for loop */
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
P_STA_RECORD_T
cnmGetStaRecByIndex(
    P_ADAPTER_T     prAdapter,
    UINT_8          ucIndex
)
{
    P_STA_RECORD_T  prStaRec;

    ASSERT(prAdapter);

    prStaRec = (ucIndex < CFG_STA_REC_NUM) ?
               &prAdapter->arStaRec[ucIndex] : NULL;

    if (prStaRec && prStaRec->fgIsInUse == FALSE) {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_48, "cnmGetStaRecByIndex:[%d] fgIsInUse=NULL", ucIndex);
        prStaRec = NULL;
    }

    return prStaRec;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Get STA_RECORD_T by Peer MAC Address(Usually TA).
*
* @param[in] pucPeerMacAddr      Given Peer MAC Address.
*
* @retval   Pointer to STA_RECORD_T, if found. NULL, if not found
*/
/*----------------------------------------------------------------------------*/
P_STA_RECORD_T
cnmGetStaRecByAddress(
    P_ADAPTER_T     prAdapter,
    UINT_8          ucNetTypeIndex,
    PUINT_8         pucPeerMacAddr
)
{
    P_STA_RECORD_T  prStaRec;
    UINT_16         i;

    //MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"cnmGetStaRecByAddress");

    ASSERT(prAdapter);
    ASSERT(pucPeerMacAddr);

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = &prAdapter->arStaRec[i];

        if (prStaRec->fgIsInUse &&
                prStaRec->ucNetTypeIndex == ucNetTypeIndex &&
                EQUAL_MAC_ADDR(prStaRec->aucMacAddr, pucPeerMacAddr)) {
            //dumpSTA(prAdapter, prStaRec);
            break;
        }
    }

    return (i < CFG_STA_REC_NUM) ? prStaRec : NULL;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Reset the Status and Reason Code Field to 0 of all Station Records for
*        the specified Network Type
*
* @param[in] eNetType       Specify Network Type
*
* @return   (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmStaRecResetStatus(
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex
)
{
    cnmStaFreeAllStaByNetType(prAdapter, eNetTypeIndex, FALSE);

#if 0
    P_STA_RECORD_T  prStaRec;
    UINT_16         i;

    ASSERT(prAdapter);

    for (i = 0; i < CFG_STA_REC_NUM; i++) {
        prStaRec = &prAdapter->arStaRec[i];

        if (prStaRec->fgIsInUse) {
            if ((NETWORK_TYPE_AIS_INDEX == eNetTypeIndex) &&
                    IS_STA_IN_AIS(prStaRec->eStaType)) {

                prStaRec->u2StatusCode = STATUS_CODE_SUCCESSFUL;
                prStaRec->u2ReasonCode = REASON_CODE_RESERVED;
                prStaRec->ucJoinFailureCount = 0;
                prStaRec->fgTransmitKeyExist = FALSE;

                prStaRec->fgSetPwrMgtBit = FALSE;
            }

            /* TODO(Kevin): For P2P and BOW */
        }
    }

    return;
#endif
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function will change the ucStaState of STA_RECORD_T and also do
*        event indication to HOST to sync the STA_RECORD_T in driver.
*
* @param[in] prStaRec       Pointer to the STA_RECORD_T
* @param[in] u4NewState     New STATE to change.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
cnmStaRecChangeState(
    P_ADAPTER_T     prAdapter,
    P_STA_RECORD_T  prStaRec,
    UINT_8          ucNewState
)
{
    BOOLEAN         fgNeedResp;

    ASSERT(prAdapter);
    ASSERT(prStaRec);
    ASSERT(prStaRec->fgIsInUse);

    //DEBUGFUNC("cnmStaRecChangeState");
    MT5931_TRACE(TRACE_INFO, MT5931_INFO_49, "ucNewState=%d prStaRec->ucStaState=%d",
                 ucNewState, prStaRec->ucStaState);
    /* Do nothing when following state transitions happen,
     * other 6 conditions should be sync to FW, including 1-->1, 3-->3
     */
    if ((ucNewState == STA_STATE_2 && prStaRec->ucStaState != STA_STATE_3) ||
            (ucNewState == STA_STATE_1 && prStaRec->ucStaState == STA_STATE_2)) {
        prStaRec->ucStaState = ucNewState;
        return;
    }

    fgNeedResp = FALSE;
    if (ucNewState == STA_STATE_3) {
        secFsmEventStart(prAdapter, prStaRec);
        if (ucNewState != prStaRec->ucStaState) {
            fgNeedResp = TRUE;
        }
    } else {
        if (ucNewState != prStaRec->ucStaState &&
                prStaRec->ucStaState == STA_STATE_3) {
            qmDeactivateStaRec(prAdapter, prStaRec->ucIndex);
        }
        fgNeedResp = FALSE;
    }
    prStaRec->ucStaState = ucNewState;

    cnmStaSendUpdateCmd(prAdapter, prStaRec, fgNeedResp);

#if CFG_ENABLE_WIFI_DIRECT
    /* To do: Confirm if it is invoked here or other location, but it should
     *        be invoked after state sync of STA_REC
     * Update system operation parameters for AP mode
     */
    if (prAdapter->fgIsP2PRegistered && (IS_STA_IN_P2P(prStaRec))) {
        P_BSS_INFO_T    prBssInfo;

        prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];

        if (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT) {
            if (prAdapter->rP2pFuncLkr.prRlmUpdateParamsForAp) {
                prAdapter->rP2pFuncLkr.prRlmUpdateParamsForAp(prAdapter, prBssInfo, FALSE);
            } else {
                ASSERT(0);
            }
        }
    }
#endif

#if CFG_ENABLE_WIFI_TETHERING
    if ((prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable) && (IS_STA_IN_P2P(prStaRec))) {
        P_BSS_INFO_T prBssInfo;

        prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];

        if (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT) {
            //ASSERT(0);
            rlmUpdateParamsForAP(prAdapter, prBssInfo, FALSE);
        }
    }
#endif /* CFG_ENABLE_WIFI_TETHERING */
    return;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param[in]
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
cnmStaRecHandleEventPkt(
    P_ADAPTER_T     prAdapter,
    P_CMD_INFO_T    prCmdInfo,
    PUINT_8         pucEventBuf
)
{
    P_EVENT_ACTIVATE_STA_REC_T  prEventContent;
    P_STA_RECORD_T              prStaRec;

    DEBUGFUNC("cnmStaRecHandleEventPkt");

    prEventContent = (P_EVENT_ACTIVATE_STA_REC_T) pucEventBuf;
    prStaRec = cnmGetStaRecByIndex(prAdapter, prEventContent->ucStaRecIdx);

    if (prStaRec && prStaRec->ucStaState == STA_STATE_3 &&
            !kalMemCmp(&prStaRec->aucMacAddr[0], &prEventContent->aucMacAddr[0],
                       MAC_ADDR_LEN)) {

        qmActivateStaRec(prAdapter, prStaRec);
    }

}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param[in]
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
cnmStaSendUpdateCmd(
    P_ADAPTER_T     prAdapter,
    P_STA_RECORD_T  prStaRec,
    BOOLEAN         fgNeedResp
)
{
    P_CMD_UPDATE_STA_RECORD_T   prCmdContent;
    WLAN_STATUS                 rStatus;

    DEBUGFUNC("cnmStaSendUpdateCmd");

    ASSERT(prAdapter);
    ASSERT(prStaRec);
    ASSERT(prStaRec->fgIsInUse);

    /* To do: come out a mechanism to limit one STA_REC sync once for AP mode
     *        to avoid buffer empty case when many STAs are associated
     *        simultaneously.
     */

    /* To do: how to avoid 2 times of allocated memory. Use Stack?
     *        One is here, the other is in wlanSendQueryCmd()
     */
    prCmdContent = cnmMemAlloc(prAdapter,
                               RAM_TYPE_BUF, sizeof(CMD_UPDATE_STA_RECORD_T));
    //ASSERT(prCmdContent);

    /* To do: exception handle */
    if (!prCmdContent) {
        DBGLOG(MEM, ERROR, ("cnmStaSendUpdateCmd(): cnmMemAlloc() fail"));
        return;
    }

    prCmdContent->ucIndex = prStaRec->ucIndex;
    prCmdContent->ucStaType = (UINT_8) prStaRec->eStaType;
    kalMemCopy(&prCmdContent->aucMacAddr[0], &prStaRec->aucMacAddr[0],
               MAC_ADDR_LEN);
    prCmdContent->u2AssocId = prStaRec->u2AssocId;
    prCmdContent->u2ListenInterval = prStaRec->u2ListenInterval;
    prCmdContent->ucNetTypeIndex = prStaRec->ucNetTypeIndex;

    prCmdContent->ucDesiredPhyTypeSet = prStaRec->ucDesiredPhyTypeSet;
    prCmdContent->u2DesiredNonHTRateSet = prStaRec->u2DesiredNonHTRateSet;
    prCmdContent->u2BSSBasicRateSet = prStaRec->u2BSSBasicRateSet;
    prCmdContent->ucMcsSet = prStaRec->ucMcsSet;
    prCmdContent->ucSupMcs32 = (UINT_8) prStaRec->fgSupMcs32;
    prCmdContent->u2HtCapInfo = prStaRec->u2HtCapInfo;
    prCmdContent->ucNeedResp = (UINT_8) fgNeedResp;

#if !CFG_SLT_SUPPORT
    if (prAdapter->rWifiVar.eRateSetting != FIXED_RATE_NONE) {
        /* override rate configuration */
        nicUpdateRateParams(prAdapter,
                            prAdapter->rWifiVar.eRateSetting,
                            &(prCmdContent->ucDesiredPhyTypeSet),
                            &(prCmdContent->u2DesiredNonHTRateSet),
                            &(prCmdContent->u2BSSBasicRateSet),
                            &(prCmdContent->ucMcsSet),
                            &(prCmdContent->ucSupMcs32),
                            &(prCmdContent->u2HtCapInfo));
    }
#endif

    prCmdContent->ucIsQoS = prStaRec->fgIsQoS;
    prCmdContent->ucIsUapsdSupported = prStaRec->fgIsUapsdSupported;
    prCmdContent->ucStaState = prStaRec->ucStaState;

    prCmdContent->ucAmpduParam = prStaRec->ucAmpduParam;
    prCmdContent->u2HtExtendedCap = prStaRec->u2HtExtendedCap;
    prCmdContent->u4TxBeamformingCap = prStaRec->u4TxBeamformingCap;
    prCmdContent->ucAselCap = prStaRec->ucAselCap;
    prCmdContent->ucRCPI = prStaRec->ucRCPI;

    prCmdContent->ucUapsdAc = prStaRec->ucBmpTriggerAC | (prStaRec->ucBmpDeliveryAC << 4);
    prCmdContent->ucUapsdSp = prStaRec->ucUapsdSp;

    rStatus = wlanSendSetQueryCmd(
                  prAdapter,                  /* prAdapter */
                  CMD_ID_UPDATE_STA_RECORD,   /* ucCID */
                  TRUE,                       /* fgSetQuery */
                  fgNeedResp,                /* fgNeedResp */
                  FALSE,                      /* fgIsOid */
                  fgNeedResp ? cnmStaRecHandleEventPkt : NULL,
                  NULL,                       /* pfCmdTimeoutHandler */
                  sizeof(CMD_UPDATE_STA_RECORD_T),    /* u4SetQueryInfoLen */
                  (PUINT_8) prCmdContent,     /* pucInfoBuffer */
                  NULL,                       /* pvSetQueryBuffer */
                  0                           /* u4SetQueryBufferLen */
              );

    ASSERT(rStatus == WLAN_STATUS_PENDING);

    cnmMemFree(prAdapter, prCmdContent);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param[in]
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID
cnmStaSendRemoveCmd(
    P_ADAPTER_T     prAdapter,
    P_STA_RECORD_T  prStaRec
)
{
    CMD_REMOVE_STA_RECORD_T     rCmdContent;
    WLAN_STATUS                 rStatus;

    ASSERT(prAdapter);
    ASSERT(prStaRec);

    rCmdContent.ucIndex = prStaRec->ucIndex;
    kalMemCopy(&rCmdContent.aucMacAddr[0], &prStaRec->aucMacAddr[0],
               MAC_ADDR_LEN);

    rStatus = wlanSendSetQueryCmd(
                  prAdapter,                  /* prAdapter */
                  CMD_ID_REMOVE_STA_RECORD,   /* ucCID */
                  TRUE,                       /* fgSetQuery */
                  FALSE,                      /* fgNeedResp */
                  FALSE,                      /* fgIsOid */
                  NULL,                       /* pfCmdDoneHandler */
                  NULL,                       /* pfCmdTimeoutHandler */
                  sizeof(CMD_REMOVE_STA_RECORD_T),    /* u4SetQueryInfoLen */
                  (PUINT_8) &rCmdContent,     /* pucInfoBuffer */
                  NULL,                       /* pvSetQueryBuffer */
                  0                           /* u4SetQueryBufferLen */
              );

    ASSERT(rStatus == WLAN_STATUS_PENDING);
}


