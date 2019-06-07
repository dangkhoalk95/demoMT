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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/swcr.c#7 $
*/

/*! \file   "swcr.c"
    \brief

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

#if CFG_SUPPORT_SWCR

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wformat"
#endif
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
#if 0
extern SWCR_MAP_ENTRY_T g_arRlmArSwCrMap[];
SWCR_MOD_MAP_ENTRY_T g_arSwCrAllMaps[] = {
    { SWCR_MAP_NUM(g_arRlmArSwCrMap), g_arRlmArSwCrMap},  /* 0x00nn */
    {0, NULL}
};
#endif

VOID swCtrlCmdCategory0(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);
VOID swCtrlCmdCategory1(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);
VOID testPsCmdCategory0(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);
VOID testPsCmdCategory1(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);
VOID swCtrlSwCr(P_ADAPTER_T prAdapter, UINT_8 ucRead, UINT_16 u2Addr, UINT_32 *pu4Data);




UINT_32 g_au4SwCr[SWCR_CR_NUM]; /*: 0: command other: data */

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
#define TEST_PS 1

const static PFN_CMD_RW_T g_arSwCtrlCmd[] = {
    swCtrlCmdCategory0,
    swCtrlCmdCategory1
#if TEST_PS
    , testPsCmdCategory0
    , testPsCmdCategory1
#endif

};


const PFN_SWCR_RW_T g_arSwCrModHandle[] = {
    swCtrlSwCr,
    NULL
};


/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

enum {
    SWCTRL_MAGIC,
    SWCTRL_DEBUG,
    SWCTRL_WIFI_VAR,
    SWCTRL_ENABLE_INT,
    SWCTRL_DISABLE_INT,
    SWCTRL_TXM_INFO,
    SWCTRL_RXM_INFO,
    SWCTRL_DUMP_BSS,
    SWCTRL_QM_INFO,
    SWCTRL_DUMP_ALL_QUEUE_LEN,
    SWCTRL_DUMP_MEM,
    SWCTRL_TX_CTRL_INFO,
    SWCTRL_DUMP_QUEUE,
    SWCTRL_DUMP_QM_DBG_CNT,
    SWCTRL_QM_DBG_CNT,
    SWCTRL_RX_PKTS_DUMP,
    SWCTRL_CATA0_INDEX_NUM
};

enum {
    SWCTRL_STA_INFO,
    SWCTRL_DUMP_STA,
    SWCTRL_STA_QUE_INFO,
    SWCTRL_CATA1_INDEX_NUM
};



#if TEST_PS
enum {
    TEST_PS_MAGIC,
    TEST_PS_SETUP_BSS,
    TEST_PS_ENABLE_BEACON,
    TEST_PS_TRIGGER_BMC,
    TEST_PS_SEND_NULL,
    TEST_PS_BUFFER_BMC,
    TEST_PS_UPDATE_BEACON,
    TEST_PS_CATA0_INDEX_NUM
};

enum {
    TEST_PS_STA_PS,
    TEST_PS_STA_ENTER_PS,
    TEST_PS_STA_EXIT_PS,
    TEST_PS_STA_TRIGGER_PSPOLL,
    TEST_PS_STA_TRIGGER_FRAME,
    TEST_PS_CATA1_INDEX_NUM
};
#endif





#define _SWCTRL_MAGIC 0x66201642

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

void dumpQueue(P_ADAPTER_T prAdapter)
{

    P_TX_CTRL_T prTxCtrl;
    P_QUE_MGT_T prQM;
    P_GLUE_INFO_T           prGlueInfo;
    UINT_32 i;
    //UINT_32 j;


    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "dumpQueue");

    prTxCtrl = &prAdapter->rTxCtrl;
    prQM = &prAdapter->rQM;
    prGlueInfo = prAdapter->prGlueInfo;

    for (i = TC0_INDEX; i <= TC5_INDEX; i++) {
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_152, "TC %u\n", i);
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_153, "Max %u Free %u\n",
                     prTxCtrl->rTc.aucMaxNumOfBuffer[i], prTxCtrl->rTc.aucFreeBufferCount[i]);

        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_154, "Average %u minReserved %u CurrentTcResource %u GuaranteedTcResource %u\n",
                     QM_GET_TX_QUEUE_LEN(prAdapter, i),
                     prQM->au4MinReservedTcResource[i],
                     prQM->au4CurrentTcResource[i],
                     prQM->au4GuaranteedTcResource[i]);

    }


    for (i = 0; i < NUM_OF_PER_STA_TX_QUEUES; i++) {
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_155, "TC %u HeadStaIdx %u ForwardCount %u\n", i, prQM->au4HeadStaRecIndex[i], prQM->au4ForwardCount[i]);
    }

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_156, "BMC or unknown TxQueue Len %u\n", prQM->arTxQueue[0].u4NumElem);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_157, "Pending %d\n", prGlueInfo->i4TxPendingFrameNum);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_158, "Pending Security %d\n", prGlueInfo->i4TxPendingSecurityFrameNum);
#if defined(LINUX)
    for (i = 0; i < 4; i++) {
        for (j = 0; j < CFG_MAX_TXQ_NUM; j++) {
            LOG_FUNC("Pending Q[%u][%u] %d\n", i, j, prGlueInfo->ai4TxPendingFrameNumPerQueue[i][j]);
        }
    }
#endif

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_159, " rFreeSwRfbList %u\n", prAdapter->rRxCtrl.rFreeSwRfbList.u4NumElem);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_160, " rReceivedRfbList %u\n", prAdapter->rRxCtrl.rReceivedRfbList.u4NumElem);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_161, " rIndicatedRfbList %u\n", prAdapter->rRxCtrl.rIndicatedRfbList.u4NumElem);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_162, " ucNumIndPacket %u\n", prAdapter->rRxCtrl.ucNumIndPacket);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_163, " ucNumRetainedPacket %u\n", prAdapter->rRxCtrl.ucNumRetainedPacket);


}


void dumpSTA(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec)
{
    UINT_8 ucWTEntry;
    UINT_32 i;
    P_BSS_INFO_T            prBssInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "dumpSTA");

    ASSERT(prStaRec);
    ucWTEntry = prStaRec->ucWTEntry;

    prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];
    ASSERT(prBssInfo);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_164, "Mac address: " MACSTR " Rcpi %u" "\n", MAC2STR(prStaRec->aucMacAddr), prStaRec->ucRCPI);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_165, "Idx %u Wtbl %u Used %u State %u Bss Phy %x Sta DesiredPhy %x\n",
                 prStaRec->ucIndex, ucWTEntry,
                 prStaRec->fgIsInUse, prStaRec->ucStaState,
                 prBssInfo->ucPhyTypeSet,
                 prStaRec->ucDesiredPhyTypeSet);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_166, "Sta Operation %x  DesiredNontHtRateSet  %x Mcs %x u2HtCapInfo %x u2CapInfo %x\n",
                 prStaRec->u2OperationalRateSet,
                 prStaRec->u2DesiredNonHTRateSet,
                 prStaRec->ucMcsSet,
                 prStaRec->u2HtCapInfo,
                 prStaRec->u2CapInfo);


    for (i = 0; i < NUM_OF_PER_STA_TX_QUEUES; i++) {
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_167, "TC %u Queue Len %u\n", i, prStaRec->arTxQueue[i].u4NumElem);
    }

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_168, "BmpDeliveryAC %x\n", prStaRec->ucBmpDeliveryAC);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_169, "BmpTriggerAC  %x\n", prStaRec->ucBmpTriggerAC);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_170, "UapsdSpSupproted  %u\n", prStaRec->fgIsUapsdSupported);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_171, "IsQoS  %u\n", prStaRec->fgIsQoS);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_172, "AssocId %u\n", prStaRec->u2AssocId);

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_173, "fgIsInPS %u\n", prStaRec->fgIsInPS);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_174, "ucFreeQuota %u\n", prStaRec->ucFreeQuota);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_175, "ucFreeQuotaForDelivery %u\n", prStaRec->ucFreeQuotaForDelivery);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_176, "ucFreeQuotaForNonDelivery %u\n", prStaRec->ucFreeQuotaForNonDelivery);


#if 0
    LOG_FUNC("IsQmmSup  %u\n", prStaRec->fgIsWmmSupported));
    LOG_FUNC("IsUapsdSup  %u\n", prStaRec->fgIsUapsdSupported));
    LOG_FUNC("AvailabaleDeliverPkts  %u\n", prStaRec->ucAvailableDeliverPkts));
    LOG_FUNC("BmpDeliverPktsAC  %u\n", prStaRec->u4BmpDeliverPktsAC));
    LOG_FUNC("BmpBufferAC  %u\n", prStaRec->u4BmpBufferAC));
    LOG_FUNC("BmpNonDeliverPktsAC  %u\n", prStaRec->u4BmpNonDeliverPktsAC));
#endif

    for (i = 0; i < CFG_RX_MAX_BA_TID_NUM; i++) {
    if (prStaRec->aprRxReorderParamRefTbl[i]) {
            LOG_FUNC("RxReorder fgIsValid: %u\n", prStaRec->aprRxReorderParamRefTbl[i]->fgIsValid);
            LOG_FUNC("RxReorder Tid: %u\n", prStaRec->aprRxReorderParamRefTbl[i]->ucTid);
            LOG_FUNC("RxReorder rReOrderQue Len: %u\n", prStaRec->aprRxReorderParamRefTbl[i]->rReOrderQue.u4NumElem);
            LOG_FUNC("RxReorder WinStart: %u\n", prStaRec->aprRxReorderParamRefTbl[i]->u2WinStart);
            LOG_FUNC("RxReorder WinEnd: %u\n", prStaRec->aprRxReorderParamRefTbl[i]->u2WinEnd);
            LOG_FUNC("RxReorder WinSize: %u\n", prStaRec->aprRxReorderParamRefTbl[i]->u2WinSize);
        }
    }

}


VOID dumpBss(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_10, "SSID %s\n", prBssInfo->aucSSID);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_177, "OWN " MACSTR"\n",MAC2STR(prBssInfo->aucOwnMacAddr));
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_178, "BSSID " MACSTR"\n",MAC2STR(prBssInfo->aucBSSID));
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_179, "ucNetTypeIndex %u\n",prBssInfo->ucNetTypeIndex);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_180, "eConnectionState %u\n",prBssInfo->eConnectionState);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_181, "eCurrentOPMode %u\n",prBssInfo->eCurrentOPMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_182, "fgIsQBSS %u\n",prBssInfo->fgIsQBSS);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_183, "fgIsShortPreambleAllowed %u\n",prBssInfo->fgIsShortPreambleAllowed);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_184, "fgUseShortPreamble %u\n",prBssInfo->fgUseShortPreamble);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_185, "fgUseShortSlotTime %u\n",prBssInfo->fgUseShortSlotTime);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_186, "ucNonHTBasicPhyType %x\n",prBssInfo->ucNonHTBasicPhyType);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_187, "u2OperationalRateSet %x\n",prBssInfo->u2OperationalRateSet);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_188, "u2BSSBasicRateSet %x\n",prBssInfo->u2BSSBasicRateSet);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_189, "ucPhyTypeSet %x\n",prBssInfo->ucPhyTypeSet);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_190, "rStaRecOfClientList %d\n",prBssInfo->rStaRecOfClientList.u4NumElem);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_191, "u2CapInfo %x\n",prBssInfo->u2CapInfo);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_192, "u2ATIMWindow %x\n",prBssInfo->u2ATIMWindow);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_193, "u2AssocId %x\n",prBssInfo->u2AssocId);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_194, "ucDTIMPeriod %x\n",prBssInfo->ucDTIMPeriod);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_195, "ucDTIMCount %x\n",prBssInfo->ucDTIMCount);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_196, "fgIsNetAbsent %x\n", prBssInfo->fgIsNetAbsent);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_197, "eBand %d\n", prBssInfo->eBand);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_198, "ucPrimaryChannel %d\n", prBssInfo->ucPrimaryChannel);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_199, "ucHtOpInfo1 %d\n", prBssInfo->ucHtOpInfo1);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_200, "ucHtOpInfo2 %d\n", prBssInfo->u2HtOpInfo2);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_201, "ucHtOpInfo3 %d\n", prBssInfo->u2HtOpInfo3);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_202, "fgErpProtectMode %d\n", prBssInfo->fgErpProtectMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_203, "eHtProtectMode %d\n", prBssInfo->eHtProtectMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_204, "eGfOperationMode %d\n", prBssInfo->eGfOperationMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_205, "eRifsOperationMode %d\n", prBssInfo->eRifsOperationMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_206, "fgObssErpProtectMode %d\n", prBssInfo->fgObssErpProtectMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_207, "eObssHtProtectMode %d\n", prBssInfo->eObssHtProtectMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_208, "eObssGfProtectMode %d\n", prBssInfo->eObssGfOperationMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_209, "fgObssRifsOperationMode %d\n", prBssInfo->fgObssRifsOperationMode);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_210, "fgAssoc40mBwAllowed %d\n", prBssInfo->fgAssoc40mBwAllowed);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_211, "fg40mBwAllowed %d\n", prBssInfo->fg40mBwAllowed);
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_212, "eBssSCO %d\n", prBssInfo->eBssSCO);


}



VOID swCtrlCmdCategory0(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0,UINT_8 ucOpt1)
{
    UINT_8 ucIndex,ucRead;
    //UINT_32 i;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"swCtrlCmdCategory0");

    SWCR_GET_RW_INDEX(ucAction,ucRead,ucIndex);

    //i=0;

    if(ucIndex>=SWCTRL_CATA0_INDEX_NUM) return;

    if(ucRead == SWCR_WRITE) {
        switch(ucIndex) {
            case SWCTRL_DEBUG:
#if DBG
                aucDebugModule[ucOpt0] = (UINT_8)g_au4SwCr[1];
#endif
                break;
            case SWCTRL_WIFI_VAR:
               break;

#if QM_DEBUG_COUNTER
           case SWCTRL_DUMP_QM_DBG_CNT:
                for(i=0;i<QM_DBG_CNT_NUM;i++) {
                    prAdapter->rQM.au4QmDebugCounters[i] = 0;
                }
                break;
           case SWCTRL_QM_DBG_CNT:
                prAdapter->rQM.au4QmDebugCounters[ucOpt0] = g_au4SwCr[1];

               break;
#endif
#ifdef CFG_RX_PKTS_DUMP
           case SWCTRL_RX_PKTS_DUMP:
           	   //LOG_FUNC("SWCTRL_RX_PKTS_DUMP: mask %x\n", g_au4SwCr[1]);
           	   prAdapter->rRxCtrl.u4RxPktsDumpTypeMask = g_au4SwCr[1];
           	   break;
#endif
            default:
                break;
        }
    }
    else {
        switch(ucIndex) {
            case SWCTRL_DEBUG:
#if DBG
                g_au4SwCr[1] = aucDebugModule[ucOpt0] ;
#endif
                break;
            case SWCTRL_MAGIC:
                g_au4SwCr[1] = _SWCTRL_MAGIC ;
                LOG_FUNC("BUILD TIME: %s %s\n", __DATE__, __TIME__);
                break;
            case SWCTRL_QM_INFO:
                {
                    P_QUE_MGT_T prQM = &prAdapter->rQM;
                    switch(ucOpt0) {
                        case 0:
                           g_au4SwCr[1] = (QM_GET_TX_QUEUE_LEN(prAdapter, ucOpt1)) ;
                           g_au4SwCr[2] = prQM->au4MinReservedTcResource[ucOpt1] ;
                           g_au4SwCr[3] = prQM->au4CurrentTcResource[ucOpt1];
                           g_au4SwCr[4] = prQM->au4GuaranteedTcResource[ucOpt1];
                            break;
                    
                        case 1:
                            g_au4SwCr[1] = prQM->au4ForwardCount[ucOpt1];
                            g_au4SwCr[2] = prQM->au4HeadStaRecIndex[ucOpt1];
                            break;

                        case 2:
                            g_au4SwCr[1] = prQM->arTxQueue[ucOpt1].u4NumElem; /* only one */
                            

                            break;
                    }

                }
            case SWCTRL_TX_CTRL_INFO:
                {
                    //P_TX_CTRL_T prTxCtrl;
                    //prTxCtrl = &prAdapter->rTxCtrl;
                    switch(ucOpt0) {
                           case 0:
                                g_au4SwCr[1] =  prAdapter->rTxCtrl.rTc.aucFreeBufferCount[ucOpt1];
                                g_au4SwCr[2] =  prAdapter->rTxCtrl.rTc.aucMaxNumOfBuffer[ucOpt1];
                                break;
                    }

                }
                break;
           case SWCTRL_DUMP_QUEUE:
                    dumpQueue(prAdapter);

                    break;
#if QM_DEBUG_COUNTER
           case SWCTRL_DUMP_QM_DBG_CNT:
                    for(i=0;i<QM_DBG_CNT_NUM;i++) {
                        LOG_FUNC("QM:DBG %u %u\n",i , prAdapter->rQM.au4QmDebugCounters[i]);
                    }
                    break;

           case SWCTRL_QM_DBG_CNT:
                    g_au4SwCr[1] = prAdapter->rQM.au4QmDebugCounters[ucOpt0];
                    break;
#endif
            case SWCTRL_DUMP_BSS:
                {
                    dumpBss(prAdapter, &(prAdapter->rWifiVar.arBssInfo[ucOpt0])) ;
                }
                break;

            default:
                    break;
        }

    }
}


VOID swCtrlCmdCategory1(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0,UINT_8 ucOpt1)
{
    UINT_8 ucIndex,ucRead;
    //UINT_8 ucWTEntry;
    P_STA_RECORD_T prStaRec;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"swCtrlCmdCategory1");

    SWCR_GET_RW_INDEX(ucAction,ucRead,ucIndex);

    if(ucOpt0>=CFG_STA_REC_NUM) return;

    //prStaRec = cnmGetStaRecByIndex (prAdapter, ucOpt0);
    prStaRec = &prAdapter->arStaRec[ucOpt0];
    //ucWTEntry =  prStaRec->ucWTEntry;
    if(ucRead == SWCR_WRITE) {
    }
    else {
        /* Read */
        switch(ucIndex) {
            case SWCTRL_STA_QUE_INFO:
            {
	if( ucOpt1 >=  NUM_OF_PER_STA_TX_QUEUES) return;
	g_au4SwCr[1] = prStaRec->arTxQueue[ucOpt1].u4NumElem;
            }
            break;
            case SWCTRL_STA_INFO:
                switch(ucOpt1) {
                    case 0:
                        g_au4SwCr[1] = prStaRec->fgIsInPS;
                        break;
                }

                break;

             case SWCTRL_DUMP_STA:
             {
                 dumpSTA(prAdapter, prStaRec);
             }
             break;

             default:

                 break;
        }
    }


}

#if TEST_PS

VOID
testPsSendQoSNullFrame (
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN UINT_8           ucUP,
    IN UINT_8           ucNetTypeIndex,
    IN BOOLEAN          fgBMC,
    IN BOOLEAN          fgIsBurstEnd,
    IN BOOLEAN          ucPacketType,
    IN BOOLEAN          ucPsSessionID,
    IN BOOLEAN          fgSetEOSP
    )
{
    P_MSDU_INFO_T prMsduInfo;
    UINT_16 u2EstimatedFrameLen;
    P_WLAN_MAC_HEADER_QOS_T prQoSNullFrame;


    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"testPsSendQoSNullFrame");
    DBGLOG(SW4, INFO, ("\n"));

    //4 <1> Allocate a PKT_INFO_T for Null Frame
    /* Init with MGMT Header Length */
    u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD + \
                          WLAN_MAC_HEADER_QOS_LEN;

    /* Allocate a MSDU_INFO_T */
    if ( (prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
        DBGLOG(SW4, WARN, ("No PKT_INFO_T for sending Null Frame.\n"));
        return ;
    }

    //4 <2> Compose Null frame in MSDU_INfO_T.
    bssComposeQoSNullFrame(prAdapter,
            (PUINT_8)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD),
            prStaRec,
            ucUP,
            fgSetEOSP);


    prMsduInfo->eSrc = TX_PACKET_MGMT;
    //prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_DATA;
    prMsduInfo->ucPacketType = ucPacketType;
    prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
    prMsduInfo->ucNetworkType = ucNetTypeIndex;
    prMsduInfo->ucMacHeaderLength = WLAN_MAC_HEADER_QOS_LEN;
    prMsduInfo->fgIs802_1x = FALSE;
    prMsduInfo->fgIs802_11 = TRUE;
    prMsduInfo->u2FrameLength = WLAN_MAC_HEADER_QOS_LEN;
    prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
    prMsduInfo->pfTxDoneHandler = NULL;
    prMsduInfo->fgIsBasicRate = TRUE;
    prMsduInfo->fgIsBurstEnd = fgIsBurstEnd;
    prMsduInfo->ucUserPriority = ucUP;
    prMsduInfo->ucPsSessionID = ucPsSessionID  /* 0~7 Test 7 means NOACK*/;

    prQoSNullFrame = (P_WLAN_MAC_HEADER_QOS_T)(  (PUINT_8)((UINT_32)(prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD)   );

    if(fgBMC) {
        prQoSNullFrame->aucAddr1[0] = 0xfd;
    }
    else {
        prQoSNullFrame->aucAddr1[5] = 0xdd;
    }

    //4 <4> Inform TXM  to send this Null frame.
    nicTxEnqueueMsdu(prAdapter, prMsduInfo);

} 



VOID
testPsSetupBss(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucNetworkTypeIndex
    )
{
    P_BSS_INFO_T prBssInfo;
    UINT_8 _aucZeroMacAddr[] = NULL_MAC_ADDR;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"testPsSetupBss()");
    DBGLOG(SW4, INFO, ("index %d\n", ucNetworkTypeIndex));

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[ucNetworkTypeIndex]);

    //4 <1.2> Initiate PWR STATE
    //SET_NET_PWR_STATE_IDLE(prAdapter, ucNetworkTypeIndex);


    //4 <2> Initiate BSS_INFO_T - common part
    BSS_INFO_INIT(prAdapter, ucNetworkTypeIndex);

    prBssInfo->eConnectionState = PARAM_MEDIA_STATE_DISCONNECTED;
    prBssInfo->eConnectionStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
    prBssInfo->eCurrentOPMode = OP_MODE_ACCESS_POINT;
    prBssInfo->fgIsNetActive = TRUE;
    prBssInfo->ucNetTypeIndex = (ucNetworkTypeIndex);
    prBssInfo->ucReasonOfDisconnect = DISCONNECT_REASON_CODE_RESERVED;

    prBssInfo->ucPhyTypeSet = PHY_TYPE_SET_802_11BG; /* Depend on eBand */
    prBssInfo->ucConfigAdHocAPMode = AP_MODE_MIXED_11BG; /* Depend on eCurrentOPMode and ucPhyTypeSet */
    prBssInfo->u2BSSBasicRateSet = RATE_SET_ERP;
    prBssInfo->u2OperationalRateSet = RATE_SET_OFDM;
    prBssInfo->fgErpProtectMode = FALSE;
    prBssInfo->fgIsQBSS = TRUE;
    
    //4 <1.5> Setup MIB for current BSS
    prBssInfo->u2BeaconInterval = 100;
    prBssInfo->ucDTIMPeriod = DOT11_DTIM_PERIOD_DEFAULT;
    prBssInfo->u2ATIMWindow = 0;

    prBssInfo->ucBeaconTimeoutCount = 0;

#if CFG_SUPPORT_AAA
    bssInitForAP (prAdapter,prBssInfo);
#endif

    COPY_MAC_ADDR(prBssInfo->aucBSSID, _aucZeroMacAddr);
    LINK_INITIALIZE(&prBssInfo->rStaRecOfClientList);
    prBssInfo->fgIsBeaconActivated = TRUE;
    prBssInfo->ucHwDefaultFixedRateCode = RATE_CCK_1M_LONG;


    COPY_MAC_ADDR(prBssInfo->aucOwnMacAddr, prAdapter->rWifiVar.aucMacAddress);

    //4 <3> Initiate BSS_INFO_T - private part
    /* TODO */
    prBssInfo->eBand = BAND_2G4;
    prBssInfo->ucPrimaryChannel = 1;
    prBssInfo->prStaRecOfAP = (P_STA_RECORD_T)NULL;


    //prBssInfo->fgErpProtectMode =  eErpProectMode;
    //prBssInfo->eHtProtectMode = eHtProtectMode;
    //prBssInfo->eGfOperationMode = eGfOperationMode;


    //4 <4> Allocate MSDU_INFO_T for Beacon
    prBssInfo->prBeacon = cnmMgtPktAlloc(prAdapter,
            OFFSET_OF(WLAN_BEACON_FRAME_T, aucInfoElem[0]) + MAX_IE_LENGTH);

    if (prBssInfo->prBeacon) {
        prBssInfo->prBeacon->eSrc = TX_PACKET_MGMT;
        prBssInfo->prBeacon->ucNetworkType = ucNetworkTypeIndex;
    }
    else {
        DBGLOG(SW4, INFO, ("prBeacon allocation fail\n"));
    }

#if 0
    prBssInfo->rPmProfSetupInfo.ucBmpDeliveryAC = PM_UAPSD_ALL;
    prBssInfo->rPmProfSetupInfo.ucBmpTriggerAC = PM_UAPSD_ALL;
    prBssInfo->rPmProfSetupInfo.ucUapsdSp = WMM_MAX_SP_LENGTH_2;
#else
    prBssInfo->rPmProfSetupInfo.ucBmpDeliveryAC = (UINT_8)prAdapter->u4UapsdAcBmp;
    prBssInfo->rPmProfSetupInfo.ucBmpTriggerAC =(UINT_8) prAdapter->u4UapsdAcBmp;
    prBssInfo->rPmProfSetupInfo.ucUapsdSp = (UINT_8)prAdapter->u4MaxSpLen;
#endif

#if 0
    for(eAci = 0; eAci < WMM_AC_INDEX_NUM; eAci++){

        prBssInfo->arACQueParms[eAci].fgIsACMSet = FALSE;
        prBssInfo->arACQueParms[eAci].u2Aifsn = (UINT_16) eAci;
        prBssInfo->arACQueParms[eAci].u2CWmin = 7;
        prBssInfo->arACQueParms[eAci].u2CWmax = 31;
        prBssInfo->arACQueParms[eAci].u2TxopLimit = eAci+1;
        DBGLOG(SW4, INFO, ("MQM: eAci = %d, ACM = %d, Aifsn = %d, CWmin = %d, CWmax = %d, TxopLimit = %d\n",
                   eAci,prBssInfo->arACQueParms[eAci].fgIsACMSet ,
                   prBssInfo->arACQueParms[eAci].u2Aifsn,
                   prBssInfo->arACQueParms[eAci].u2CWmin,
                   prBssInfo->arACQueParms[eAci].u2CWmax,
                   prBssInfo->arACQueParms[eAci].u2TxopLimit));

    }
#endif


    DBGLOG(SW4, INFO, ("[2] ucBmpDeliveryAC:0x%x, ucBmpTriggerAC:0x%x, ucUapsdSp:0x%x",
            prBssInfo->rPmProfSetupInfo.ucBmpDeliveryAC,
            prBssInfo->rPmProfSetupInfo.ucBmpTriggerAC,
            prBssInfo->rPmProfSetupInfo.ucUapsdSp));

    return;
} 




VOID testPsCmdCategory0(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0,UINT_8 ucOpt1)
{
    UINT_8 ucIndex,ucRead;
    P_STA_RECORD_T prStaRec;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"testPsCmdCategory0");
    SWCR_GET_RW_INDEX(ucAction,ucRead,ucIndex);

    DBGLOG(SW4, LOUD, ("Read %u Index %u\n",ucRead,ucIndex));

    prStaRec = cnmGetStaRecByIndex (prAdapter, 0);

    if(ucIndex>=TEST_PS_CATA0_INDEX_NUM) return;

    if(ucRead == SWCR_WRITE) {
        switch(ucIndex) {
            case TEST_PS_SETUP_BSS:
                testPsSetupBss(prAdapter, ucOpt0) ;
                break;

            case TEST_PS_ENABLE_BEACON:
               break;

            case TEST_PS_TRIGGER_BMC:
                //txmForwardQueuedBmcPkts (ucOpt0);
                break;
            case TEST_PS_SEND_NULL:
            {

                testPsSendQoSNullFrame (prAdapter,prStaRec,
                        (UINT_8)(g_au4SwCr[1] & 0xFF), /* UP */
                        ucOpt0,
                        (BOOLEAN)((g_au4SwCr[1] >>8)& 0xFF), /* BMC*/
                        (BOOLEAN)((g_au4SwCr[1] >>16)& 0xFF), /* BurstEnd*/
                        (BOOLEAN)((g_au4SwCr[1] >>24)& 0xFF), /* Packet type*/
                        (UINT_8)((g_au4SwCr[2] )& 0xFF), /* PS sesson ID 7: NOACK */
                        FALSE                                  /* EOSP */
                    );
            }
                break;
            case TEST_PS_BUFFER_BMC:
                //g_aprBssInfo[ucOpt0]->fgApToBufferBMC = (g_au4SwCr[1] & 0xFF);
                break;
            case TEST_PS_UPDATE_BEACON:
#if (CFG_SUPPORT_ADHOC) || (CFG_SUPPORT_AAA)
                bssUpdateBeaconContent(prAdapter, (ENUM_NETWORK_TYPE_INDEX_T)ucOpt0 /*networktype*/ );
#endif
                break;

           default:
                break;
        }
    }
    else {
        switch(ucIndex) {

            case TEST_PS_MAGIC:
                g_au4SwCr[1] = 0x88660011 ;
                break;

        }
    }
}

#endif //TEST_PS

#if TEST_PS

VOID testPsCmdCategory1(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0,UINT_8 ucOpt1)
{
    UINT_8 ucIndex,ucRead;
    //UINT_8 ucWTEntry;
    P_STA_RECORD_T prStaRec;

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"testPsCmdCategory1");

    SWCR_GET_RW_INDEX(ucAction,ucRead,ucIndex);

    if(ucOpt0>=CFG_STA_REC_NUM) return;

    prStaRec = cnmGetStaRecByIndex (prAdapter, ucOpt0);
    //ucWTEntry =  prStaRec->ucWTEntry;
    if(ucRead == SWCR_WRITE) {

        switch(ucIndex) {
            case TEST_PS_STA_PS:
                prStaRec->fgIsInPS = (BOOLEAN) (g_au4SwCr[1] & 0x1);
                prStaRec->fgIsQoS = (BOOLEAN) (g_au4SwCr[1] >>8 & 0xFF);
                prStaRec->fgIsUapsdSupported = (BOOLEAN) (g_au4SwCr[1] >>16 & 0xFF);
                prStaRec->ucBmpDeliveryAC = (BOOLEAN) (g_au4SwCr[1] >>24 & 0xFF);
                break;

        }

    }
    else {
        /* Read */
        switch(ucIndex) {
            default:
                break;
        }
    }


}

#endif //TEST_PS



VOID swCtrlSwCr(P_ADAPTER_T prAdapter, UINT_8 ucRead, UINT_16 u2Addr, UINT_32 *pu4Data)
{
    /* According other register STAIDX */
    UINT_8      ucOffset;
    ucOffset = (u2Addr>>2) & 0x3F;

    if(ucOffset>= SWCR_CR_NUM) return;

    if(ucRead==SWCR_WRITE) {
        g_au4SwCr[ucOffset] = *pu4Data;
        if(ucOffset==0x0) {
            /* Commmand   [31:24]: Category */
            /* Commmand   [23:23]: 1(W) 0(R) */ 
            /* Commmand   [22:16]: Index */ 
            /* Commmand   [15:08]: Option0  */  
            /* Commmand   [07:00]: Option1   */  
            UINT_8 ucCate;
            UINT_32 u4Cmd;
            u4Cmd = g_au4SwCr[0];
            ucCate = (UINT_8)(u4Cmd >> 24) ;
            if(ucCate < sizeof(g_arSwCtrlCmd)/sizeof(g_arSwCtrlCmd[0])) {
                if(g_arSwCtrlCmd[ucCate]!=NULL) {
                    g_arSwCtrlCmd[ucCate](prAdapter, ucCate, (UINT_8)(u4Cmd>>16 & 0xFF),(UINT_8)((u4Cmd>>8) & 0xFF), (UINT_8)(u4Cmd&0xFF));
                }
            }
        }
    }
    else {
        *pu4Data = g_au4SwCr[ucOffset];
    }
}

VOID swCrReadWriteCmd(P_ADAPTER_T prAdapter, UINT_8 ucRead, UINT_16 u2Addr, UINT_32 *pu4Data) 
{
    UINT_8 ucMod;

    ucMod =  u2Addr >>8;
    /* Address [15:8] MOD ID */
    /* Address [7:0] OFFSET */

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_10,"swCrReadWriteCmd");
    LOG_FUNC ("%u addr 0x%x data 0x%x\n",ucRead,u2Addr,*pu4Data);

    if(ucMod < (sizeof(g_arSwCrModHandle)/sizeof(g_arSwCrModHandle[0])) ) {

        if(g_arSwCrModHandle[ucMod]!=NULL) {
         g_arSwCrModHandle[ucMod](prAdapter, ucRead, u2Addr, pu4Data);
        }
   } /* ucMod */
}



#endif /* CFG_SUPPORT_SWCR */


