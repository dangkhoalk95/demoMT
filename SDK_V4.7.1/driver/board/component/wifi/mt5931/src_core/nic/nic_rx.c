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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/nic/nic_rx.c#62 $
*/

/*! \file   nic_rx.c
    \brief  Functions that provide many rx-related functions

    This file includes the functions used to process RFB and dispatch RFBs to
    the appropriate related rx functions for protocols.
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
#ifdef WCN_MAUI
extern kal_bool    WNDRVPauseFlag;
extern kal_bool    WNDRVNoFLC2Flag;
#endif

#ifndef LINUX
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#ifdef __WIFI_SNIFFER_SUPPORT__
extern int parser_fsm_go(char *p, int len);
#endif

extern kal_bool Wifi_Query_Dynamic_Switch_Cachability(void);
extern VOID kalHandleAssocInfo(IN P_GLUE_INFO_T prGlueInfo, IN P_EVENT_ASSOC_INFO prAssocInfo);

#if __WIFI_HIF_SDIO__

#ifdef __ICCARM__
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN extern unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#else
extern ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#endif

#endif
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

#if CFG_MGMT_FRAME_HANDLING
static PROCESS_RX_MGT_FUNCTION apfnProcessRxMgtFrame[MAX_NUM_OF_FC_SUBTYPES] = {
#if CFG_SUPPORT_AAA
    aaaFsmRunEventRxAssoc,              /* subtype 0000: Association request */
#else
    NULL,                               /* subtype 0000: Association request */
#endif /* CFG_SUPPORT_AAA */
    saaFsmRunEventRxAssoc,              /* subtype 0001: Association response */
#if CFG_SUPPORT_AAA
    aaaFsmRunEventRxAssoc,              /* subtype 0010: Reassociation request */
#else
    NULL,                               /* subtype 0010: Reassociation request */
#endif /* CFG_SUPPORT_AAA */
    saaFsmRunEventRxAssoc,              /* subtype 0011: Reassociation response */
#if CFG_SUPPORT_ADHOC
    bssProcessProbeRequest,             /* subtype 0100: Probe request */
#else
    NULL,                               /* subtype 0100: Probe request */
#endif /* CFG_SUPPORT_ADHOC */
    scanProcessBeaconAndProbeResp,      /* subtype 0101: Probe response */
    NULL,                               /* subtype 0110: reserved */
    NULL,                               /* subtype 0111: reserved */
    scanProcessBeaconAndProbeResp,      /* subtype 1000: Beacon */
    NULL,                               /* subtype 1001: ATIM */
    saaFsmRunEventRxDisassoc,           /* subtype 1010: Disassociation */
    authCheckRxAuthFrameTransSeq,       /* subtype 1011: Authentication */
    saaFsmRunEventRxDeauth,             /* subtype 1100: Deauthentication */
    nicRxProcessActionFrame,            /* subtype 1101: Action */
    NULL,                               /* subtype 1110: reserved */
    NULL                                /* subtype 1111: reserved */
};
#endif


/* NOTE(Nelson): for MT5931 E3 eHPI short RX issue workaround */
#define E3_WORKAROND_RX_MIN_SIZE    (20)

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
* @brief Initialize the RFBs
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxInitialize(
    IN P_ADAPTER_T prAdapter
)
{
    P_RX_CTRL_T prRxCtrl;
    PUINT_8 pucMemHandle;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
    UINT_32 i;

    DEBUGFUNC("nicRxInitialize");

    ASSERT(prAdapter);
    prRxCtrl = &prAdapter->rRxCtrl;

    //4 <0> Clear allocated memory.
    kalMemZero((PVOID) prRxCtrl->pucRxCached, prRxCtrl->u4RxCachedSize);

    //4 <1> Initialize the RFB lists
    QUEUE_INITIALIZE(&prRxCtrl->rFreeSwRfbList);
    QUEUE_INITIALIZE(&prRxCtrl->rReceivedRfbList);
    QUEUE_INITIALIZE(&prRxCtrl->rIndicatedRfbList);

    pucMemHandle = prRxCtrl->pucRxCached;
    for (i = CFG_RX_MAX_PKT_NUM; i != 0; i--) {
        prSwRfb = (P_SW_RFB_T)pucMemHandle;

#ifndef WCN_MAUI
        /* NOTE(Nelson): Do not init the buffer at first otherwise the flow control mechanism may
                        take place.
        */
        nicRxSetupRFB(prAdapter, prSwRfb);
#endif
        nicRxReturnRFB(prAdapter, prSwRfb);

        pucMemHandle += ALIGN_4(sizeof(SW_RFB_T));
    }

#ifndef WCN_MAUI
    /* NOTE(Nelson): the rFreeRFBWithBuffList is not init on MAUI so skip this check */
    ASSERT(prRxCtrl->rFreeSwRfbList.u4NumElem == CFG_RX_MAX_PKT_NUM);
#endif
    /* Check if the memory allocation consist with this initialization function */
    ASSERT((UINT_32)(pucMemHandle - prRxCtrl->pucRxCached) == prRxCtrl->u4RxCachedSize);

    //4 <2> Clear all RX counters
    RX_RESET_ALL_CNTS(prRxCtrl);

#if CFG_SDIO_RX_AGG
    prRxCtrl->pucRxCoalescingBufPtr = prAdapter->pucCoalescingBufCached;
#if !defined(MT5931)
    HAL_CFG_MAX_HIF_RX_LEN_NUM(prAdapter, CFG_SDIO_MAX_RX_AGG_NUM);
#endif
#else
#if !defined(MT5931)
    HAL_CFG_MAX_HIF_RX_LEN_NUM(prAdapter, 1);
#endif
#endif

#if CFG_HIF_STATISTICS
    prRxCtrl->u4TotalRxAccessNum = 0;
    prRxCtrl->u4TotalRxPacketNum = 0;
#endif

#if CFG_HIF_RX_STARVATION_WARNING
    prRxCtrl->u4QueuedCnt = 0;
    prRxCtrl->u4DequeuedCnt = 0;
#endif

    return;
} /* end of nicRxInitialize() */


#if defined(MT5931)
/*----------------------------------------------------------------------------*/
/*!
* @brief Initialize HIF RX control registers explicitly
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxPostInitialize(
    IN P_ADAPTER_T prAdapter
)
{
    //P_RX_CTRL_T prRxCtrl;
    //DEBUGFUNC("nicRxPostInitialize");

    ASSERT(prAdapter);
    //prRxCtrl = &prAdapter->rRxCtrl;

#if CFG_SDIO_RX_AGG
    HAL_CFG_MAX_HIF_RX_LEN_NUM(prAdapter, CFG_SDIO_MAX_RX_AGG_NUM);
#else
    HAL_CFG_MAX_HIF_RX_LEN_NUM(prAdapter, 1);
#endif

} /* end of nicRxPostInitialize() */
#endif


/*----------------------------------------------------------------------------*/
/*!
* @brief Uninitialize the RFBs
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxUninitialize(
    IN P_ADAPTER_T prAdapter
)
{
    P_RX_CTRL_T prRxCtrl;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    nicRxFlush(prAdapter);

    do {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        QUEUE_REMOVE_HEAD(&prRxCtrl->rReceivedRfbList, prSwRfb, P_SW_RFB_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        if (prSwRfb) {
            if (prSwRfb->pvPacket) {
                kalPacketFree(prAdapter->prGlueInfo, prSwRfb->pvPacket);
            }
            prSwRfb->pvPacket = NULL;
        } else {
            break;
        }
    } while (TRUE);

    do {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        QUEUE_REMOVE_HEAD(&prRxCtrl->rFreeSwRfbList, prSwRfb, P_SW_RFB_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        if (prSwRfb) {
            if (prSwRfb->pvPacket) {
                kalPacketFree(prAdapter->prGlueInfo, prSwRfb->pvPacket);
            }
            prSwRfb->pvPacket = NULL;
        } else {
            break;
        }
    } while (TRUE);

    return;
} /* end of nicRxUninitialize() */

/*----------------------------------------------------------------------------*/
/*!
* @brief release the RFBs for specific network
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @param eNetworkTypeIdx
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicReleaseRxRFBByNetwork(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
)
{
    P_RX_CTRL_T prRxCtrl;
    P_QUE_T prRxQue;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
    peer_buff_struct    *peer_p = (peer_buff_struct *)NULL;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry;


    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_DEINIT, "nicReleaseRxRFBByNetwork, eNetworkTypeIdx=%d", eNetworkTypeIdx);

    //1. alread release in nicDeactivateNetwork() -> qmDelRxBaEntry() -> qmFlushStaRxQueue()
    //nicRxFlush(prAdapter);//zhilin_20131018

    //2. Release packets of rReceivedRfbList
    prRxQue = &prRxCtrl->rReceivedRfbList;
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prRxQue);
    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);

    while (prQueueEntry) {
        prSwRfb = (P_SW_RFB_T)prQueueEntry;
        if (prSwRfb->pvPacket) {
            peer_p = (peer_buff_struct *)prSwRfb->pvPacket;

            if ((GLUE_GET_PKT_IS_P2P(peer_p) && (eNetworkTypeIdx == NETWORK_TYPE_P2P_INDEX)) //P2P case
                    || ((!GLUE_GET_PKT_IS_P2P(peer_p)) && (eNetworkTypeIdx == NETWORK_TYPE_AIS_INDEX))) { //AIS CASE
                kalPacketFree(prAdapter->prGlueInfo, prSwRfb->pvPacket);
                prSwRfb->pvPacket = NULL;
            } else {

                QUEUE_INSERT_TAIL(prRxQue, prQueueEntry);
            }

        } else {
            QUEUE_INSERT_TAIL(prRxQue, prQueueEntry);

        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    };
    QUEUE_CONCATENATE_QUEUES(prRxQue, prTempCmdQue);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

    //3. Release packets of rFreeSwRfbList
    prRxQue = &prRxCtrl->rFreeSwRfbList;
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prRxQue);
    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    while (prQueueEntry) {
        prSwRfb = (P_SW_RFB_T)prQueueEntry;

        if (prSwRfb->pvPacket) {
            if ((GLUE_GET_PKT_IS_P2P(peer_p) && (eNetworkTypeIdx == NETWORK_TYPE_P2P_INDEX)) //P2P case
                    || ((!GLUE_GET_PKT_IS_P2P(peer_p)) && (eNetworkTypeIdx == NETWORK_TYPE_AIS_INDEX))) { //AIS CASE
                kalPacketFree(prAdapter->prGlueInfo, prSwRfb->pvPacket);
                prSwRfb->pvPacket = NULL;

            } else {
                QUEUE_INSERT_TAIL(prRxQue, prQueueEntry);
            }

        } else {
            QUEUE_INSERT_TAIL(prRxQue, prQueueEntry);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    };

    QUEUE_CONCATENATE_QUEUES(prRxQue, prTempCmdQue);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

    return;
} /* end of nicRxUninitialize() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Fill RFB
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb   specify the RFB to receive rx data
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxFillRFB(
    IN P_ADAPTER_T    prAdapter,
    IN OUT P_SW_RFB_T prSwRfb
)
{
    P_HIF_RX_HEADER_T prHifRxHdr;

    UINT_32 u4PktLen = 0;
    UINT_32 u4MacHeaderLen;
    UINT_32 u4HeaderOffset;


    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prHifRxHdr = prSwRfb->prHifRxHdr;
    ASSERT(prHifRxHdr);

    u4PktLen = prHifRxHdr->u2PacketLen;

    u4HeaderOffset = (UINT_32)(prHifRxHdr->ucHerderLenOffset & HIF_RX_HDR_HEADER_OFFSET_MASK);
    u4MacHeaderLen = (UINT_32)(prHifRxHdr->ucHerderLenOffset & HIF_RX_HDR_HEADER_LEN)
                     >> HIF_RX_HDR_HEADER_LEN_OFFSET;

    //DBGLOG(RX, TRACE, ("u4HeaderOffset = %d, u4MacHeaderLen = %d\n",
    //    u4HeaderOffset, u4MacHeaderLen));

    prSwRfb->u2HeaderLen = (UINT_16)u4MacHeaderLen;
    prSwRfb->pvHeader = (PUINT_8)prHifRxHdr + HIF_RX_HDR_SIZE + u4HeaderOffset;
    prSwRfb->u2PacketLen = (UINT_16)(u4PktLen - (HIF_RX_HDR_SIZE + u4HeaderOffset));

    //DBGLOG(RX, TRACE, ("Dump Rx packet, u2PacketLen = %d\n", prSwRfb->u2PacketLen));
    //DBGLOG_MEM8(RX, TRACE, prSwRfb->pvHeader, prSwRfb->u2PacketLen);

#if 0
    if (prHifRxHdr->ucReorder & HIF_RX_HDR_80211_HEADER_FORMAT) {
        prSwRfb->u4HifRxHdrFlag |= HIF_RX_HDR_FLAG_802_11_FORMAT;
        DBGLOG(RX, TRACE, ("HIF_RX_HDR_FLAG_802_11_FORMAT\n"));
    }

    if (prHifRxHdr->ucReorder & HIF_RX_HDR_DO_REORDER) {
        prSwRfb->u4HifRxHdrFlag |= HIF_RX_HDR_FLAG_DO_REORDERING;
        DBGLOG(RX, TRACE, ("HIF_RX_HDR_FLAG_DO_REORDERING\n"));

        /* Get Seq. No and TID, Wlan Index info */
        if (prHifRxHdr->u2SeqNoTid & HIF_RX_HDR_BAR_FRAME) {
            prSwRfb->u4HifRxHdrFlag |= HIF_RX_HDR_FLAG_BAR_FRAME;
            DBGLOG(RX, TRACE, ("HIF_RX_HDR_FLAG_BAR_FRAME\n"));
        }

        prSwRfb->u2SSN = prHifRxHdr->u2SeqNoTid & HIF_RX_HDR_SEQ_NO_MASK;
        prSwRfb->ucTid = (UINT_8)((prHifRxHdr->u2SeqNoTid & HIF_RX_HDR_TID_MASK)
                                  >> HIF_RX_HDR_TID_OFFSET);
        DBGLOG(RX, TRACE, ("u2SSN = %d, ucTid = %d\n",
                           prSwRfb->u2SSN, prSwRfb->ucTid));
    }

    if (prHifRxHdr->ucReorder & HIF_RX_HDR_WDS) {
        prSwRfb->u4HifRxHdrFlag |= HIF_RX_HDR_FLAG_AMP_WDS;
        DBGLOG(RX, TRACE, ("HIF_RX_HDR_FLAG_AMP_WDS\n"));
    }
#endif
}


#if CFG_TCP_IP_CHKSUM_OFFLOAD || CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60
/*----------------------------------------------------------------------------*/
/*!
* @brief Fill checksum status in RFB
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
* @param u4TcpUdpIpCksStatus specify the Checksum status
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxFillChksumStatus(
    IN  P_ADAPTER_T   prAdapter,
    IN OUT P_SW_RFB_T prSwRfb,
    IN  UINT_32 u4TcpUdpIpCksStatus
)
{

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    if (prAdapter->u4CSUMFlags != CSUM_NOT_SUPPORTED) {
        if (u4TcpUdpIpCksStatus & RX_CS_TYPE_IPv4) { // IPv4 packet
            prSwRfb->aeCSUM[CSUM_TYPE_IPV6] = CSUM_RES_NONE;
            if (u4TcpUdpIpCksStatus & RX_CS_STATUS_IP) { //IP packet csum failed
                prSwRfb->aeCSUM[CSUM_TYPE_IPV4] = CSUM_RES_FAILED;
            } else {
                prSwRfb->aeCSUM[CSUM_TYPE_IPV4] = CSUM_RES_SUCCESS;
            }

            if (u4TcpUdpIpCksStatus & RX_CS_TYPE_TCP) { //TCP packet
                prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_NONE;
                if (u4TcpUdpIpCksStatus & RX_CS_STATUS_TCP) { //TCP packet csum failed
                    prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_FAILED;
                } else {
                    prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_SUCCESS;
                }
            } else if (u4TcpUdpIpCksStatus & RX_CS_TYPE_UDP) { //UDP packet
                prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_NONE;
                if (u4TcpUdpIpCksStatus & RX_CS_STATUS_UDP) { //UDP packet csum failed
                    prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_FAILED;
                } else {
                    prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_SUCCESS;
                }
            } else {
                prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_NONE;
                prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_NONE;
            }
        } else if (u4TcpUdpIpCksStatus & RX_CS_TYPE_IPv6) { //IPv6 packet
            prSwRfb->aeCSUM[CSUM_TYPE_IPV4] = CSUM_RES_NONE;
            prSwRfb->aeCSUM[CSUM_TYPE_IPV6] = CSUM_RES_SUCCESS;

            if (u4TcpUdpIpCksStatus & RX_CS_TYPE_TCP) { //TCP packet
                prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_NONE;
                if (u4TcpUdpIpCksStatus & RX_CS_STATUS_TCP) { //TCP packet csum failed
                    prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_FAILED;
                } else {
                    prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_SUCCESS;
                }
            } else if (u4TcpUdpIpCksStatus & RX_CS_TYPE_UDP) { //UDP packet
                prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_NONE;
                if (u4TcpUdpIpCksStatus & RX_CS_STATUS_UDP) { //UDP packet csum failed
                    prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_FAILED;
                } else {
                    prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_SUCCESS;
                }
            } else {
                prSwRfb->aeCSUM[CSUM_TYPE_UDP] = CSUM_RES_NONE;
                prSwRfb->aeCSUM[CSUM_TYPE_TCP] = CSUM_RES_NONE;
            }
        } else {
            prSwRfb->aeCSUM[CSUM_TYPE_IPV4] = CSUM_RES_NONE;
            prSwRfb->aeCSUM[CSUM_TYPE_IPV6] = CSUM_RES_NONE;
        }
    }

}
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */


/*----------------------------------------------------------------------------*/
/*!
* @brief Process packet doesn't need to do buffer reordering
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessPktWithoutReorder(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb
)
{
    P_RX_CTRL_T prRxCtrl;
    P_TX_CTRL_T prTxCtrl;
    BOOL fgIsRetained = FALSE;
    //UINT_32 u4CurrentRxBufferCount;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;

    //DBGLOG(RX, TRACE, ("\n"));
    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    prTxCtrl = &prAdapter->rTxCtrl;
    ASSERT(prTxCtrl);

    //u4CurrentRxBufferCount = prRxCtrl->rFreeSwRfbList.u4NumElem;
    /* QM USED = $A, AVAILABLE COUNT = $B, INDICATED TO OS = $C
     * TOTAL = $A + $B + $C
     *
     * Case #1 (Retain)
     * -------------------------------------------------------
     * $A + $B < THRESHOLD := $A + $B + $C < THRESHOLD + $C := $TOTAL - THRESHOLD < $C
     * => $C used too much, retain
     *
     * Case #2 (Non-Retain)
     * -------------------------------------------------------
     * $A + $B > THRESHOLD := $A + $B + $C > THRESHOLD + $C := $TOTAL - THRESHOLD > $C
     * => still availble for $C to use
     *
     */
    /*fgIsRetained = (((u4CurrentRxBufferCount +
                    qmGetRxReorderQueuedBufferCount(prAdapter) +
                    prTxCtrl->i4PendingFwdFrameCount) < CFG_RX_RETAINED_PKT_THRESHOLD) ?
            TRUE : FALSE);*/
    fgIsRetained = FALSE; //set fgIsRetained always false in MAUI load
    //DBGLOG(RX, INFO, ("fgIsRetained = %d\n", fgIsRetained));

    if (kalProcessRxPacket(prAdapter->prGlueInfo,
                           prSwRfb->pvPacket,
                           prSwRfb->pvHeader,
                           (UINT_32)prSwRfb->u2PacketLen,
                           fgIsRetained,
                           prSwRfb->aeCSUM) != WLAN_STATUS_SUCCESS) {
        //DBGLOG(RX, ERROR, ("kalProcessRxPacket return value != WLAN_STATUS_SUCCESS\n"));
        MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_65, "kalProcessRxPacket return value != WLAN_STATUS_SUCCESS\n");
        ASSERT(0);

        nicRxReturnRFB(prAdapter, prSwRfb);
        return;
    } else {
        prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
        if (prStaRec) {
#if CFG_ENABLE_WIFI_DIRECT
            if (prStaRec->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX &&
                    prAdapter->fgIsP2PRegistered == TRUE) {
                GLUE_SET_PKT_FLAG_P2P(prSwRfb->pvPacket);
            }
#endif
#if CFG_ENABLE_BT_OVER_WIFI
            if (prStaRec->ucNetTypeIndex == NETWORK_TYPE_BOW_INDEX) {
                GLUE_SET_PKT_FLAG_PAL(prSwRfb->pvPacket);
            }
#endif
#if CFG_ENABLE_WIFI_TETHERING
            if (prStaRec->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX &&
                    prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable == TRUE) {
                //4TODO: flag set
                GLUE_SET_PKT_FLAG_P2P(prSwRfb->pvPacket);
            }
#endif /* CFG_ENABLE_WIFI_TETHERING */
        }
        prRxCtrl->apvIndPacket[prRxCtrl->ucNumIndPacket] = prSwRfb->pvPacket;
        prRxCtrl->ucNumIndPacket++;
    }

    if (fgIsRetained) {
        prRxCtrl->apvRetainedPacket[prRxCtrl->ucNumRetainedPacket] = prSwRfb->pvPacket;
        prRxCtrl->ucNumRetainedPacket++;
        /* TODO : error handling of nicRxSetupRFB */
        nicRxSetupRFB(prAdapter, prSwRfb);
        nicRxReturnRFB(prAdapter, prSwRfb);
    } else {
        prSwRfb->pvPacket = NULL;
        nicRxReturnRFB(prAdapter, prSwRfb);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Process forwarding data packet
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessForwardPkt(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb
)
{
    P_MSDU_INFO_T prMsduInfo, prRetMsduInfoList;
    P_TX_CTRL_T prTxCtrl;
    P_RX_CTRL_T prRxCtrl;
    KAL_SPIN_LOCK_DECLARATION();

    // DEBUGFUNC("nicRxProcessForwardPkt");
    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prTxCtrl = &prAdapter->rTxCtrl;
    prRxCtrl = &prAdapter->rRxCtrl;

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
    QUEUE_REMOVE_HEAD(&prTxCtrl->rFreeMsduInfoList, prMsduInfo, P_MSDU_INFO_T);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);

    if (prMsduInfo && kalProcessRxPacket(prAdapter->prGlueInfo,
                                         prSwRfb->pvPacket,
                                         prSwRfb->pvHeader,
                                         (UINT_32)prSwRfb->u2PacketLen,
                                         prRxCtrl->rFreeSwRfbList.u4NumElem < CFG_RX_RETAINED_PKT_THRESHOLD ? TRUE : FALSE,
                                         prSwRfb->aeCSUM) == WLAN_STATUS_SUCCESS) {

        prMsduInfo->eSrc = TX_PACKET_FORWARDING;
        // pack into MSDU_INFO_T
        nicTxFillMsduInfo(prAdapter, prMsduInfo, (P_NATIVE_PACKET)(prSwRfb->pvPacket));
        // Overwrite the ucNetworkType
        prMsduInfo->ucNetworkType = HIF_RX_HDR_GET_NETWORK_IDX(prSwRfb->prHifRxHdr);

        // release RX buffer (to rIndicatedRfbList)
        prSwRfb->pvPacket = NULL;
        nicRxReturnRFB(prAdapter, prSwRfb);

        // increase forward frame counter
        GLUE_INC_REF_CNT(prTxCtrl->i4PendingFwdFrameCount);

        // send into TX queue
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);
        prRetMsduInfoList = qmEnqueueTxPackets(prAdapter, prMsduInfo);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);

        if (prRetMsduInfoList != NULL) { // TX queue refuses queuing the packet
            nicTxFreeMsduInfoPacket(prAdapter, prRetMsduInfoList);
            nicTxReturnMsduInfo(prAdapter, prRetMsduInfoList);
        }
        /* indicate service thread for sending */
        if (prTxCtrl->i4PendingFwdFrameCount > 0) {
            kalSetEvent(prAdapter->prGlueInfo);
        }
    } else { // no TX resource
        nicRxReturnRFB(prAdapter, prSwRfb);
    }

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Process broadcast data packet for both host and forwarding
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessGOBroadcastPkt(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb
)
{
    P_SW_RFB_T prSwRfbDuplicated;
    //P_TX_CTRL_T prTxCtrl;
    P_RX_CTRL_T prRxCtrl;
    P_HIF_RX_HEADER_T prHifRxHdr;

    KAL_SPIN_LOCK_DECLARATION();

    // DEBUGFUNC("nicRxProcessGOBroadcastPkt");
    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    //prTxCtrl = &prAdapter->rTxCtrl;
    prRxCtrl = &prAdapter->rRxCtrl;

    prHifRxHdr = prSwRfb->prHifRxHdr;
    ASSERT(prHifRxHdr);

    ASSERT(CFG_NUM_OF_QM_RX_PKT_NUM >= 16);

    if (prRxCtrl->rFreeSwRfbList.u4NumElem
            >= (CFG_RX_MAX_PKT_NUM - (CFG_NUM_OF_QM_RX_PKT_NUM - 16 /* Reserved for others */))) {

        /* 1. Duplicate SW_RFB_T */
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        QUEUE_REMOVE_HEAD(&prRxCtrl->rFreeSwRfbList, prSwRfbDuplicated, P_SW_RFB_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

        if (prSwRfbDuplicated) {
#ifdef WCN_MAUI
            nicRxSetupRFB(prAdapter, prSwRfbDuplicated);
#endif /* WCN_MAUI */
            kalMemCopy(prSwRfbDuplicated->pucRecvBuff,
                       prSwRfb->pucRecvBuff,
                       ALIGN_4(prHifRxHdr->u2PacketLen + HIF_RX_HW_APPENDED_LEN));

            prSwRfbDuplicated->ucPacketType = HIF_RX_PKT_TYPE_DATA;
            prSwRfbDuplicated->ucStaRecIdx = (UINT_8)(prHifRxHdr->ucStaRecIdx);
            nicRxFillRFB(prAdapter, prSwRfbDuplicated);

            /* 2. Modify eDst */
            prSwRfbDuplicated->eDst = RX_PKT_DESTINATION_FORWARD;

            /* 4. Forward */
            nicRxProcessForwardPkt(prAdapter, prSwRfbDuplicated);
        }
    } else {
        // DBGLOG(RX, WARN, ("Stop to forward BMC packet due to less free Sw Rfb %u\n", prRxCtrl->rFreeSwRfbList.u4NumElem));
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_68, "Stop to forward BMC packet due to less free Sw Rfb %u\n", prRxCtrl->rFreeSwRfbList.u4NumElem);
    }

    /* 3. Indicate to host */
    prSwRfb->eDst = RX_PKT_DESTINATION_HOST;
    nicRxProcessPktWithoutReorder(prAdapter, prSwRfb);

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Process HIF data packet
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessDataPacket(
    IN P_ADAPTER_T    prAdapter,
    IN OUT P_SW_RFB_T prSwRfb
)
{
#if CFG_HIF_RX_STARVATION_WARNING
    P_RX_CTRL_T prRxCtrl;
#endif
    P_SW_RFB_T prRetSwRfb, prNextSwRfb;
    P_HIF_RX_HEADER_T prHifRxHdr;
    P_STA_RECORD_T prStaRec;

    //DEBUGFUNC("nicRxProcessDataPacket");
    //DBGLOG(INIT, TRACE, ("\n"));

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prHifRxHdr = prSwRfb->prHifRxHdr;
#if CFG_HIF_RX_STARVATION_WARNING
    prRxCtrl = &prAdapter->rRxCtrl;
#endif
    nicRxFillRFB(prAdapter, prSwRfb);

#if CFG_TCP_IP_CHKSUM_OFFLOAD || CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60
    {
        UINT_32 u4TcpUdpIpCksStatus;

        u4TcpUdpIpCksStatus = *((PUINT_32)((UINT_32)prHifRxHdr +
                                           (UINT_32)(ALIGN_4(prHifRxHdr->u2PacketLen))));
        nicRxFillChksumStatus(prAdapter, prSwRfb, u4TcpUdpIpCksStatus);

    }
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */

    prStaRec = cnmGetStaRecByIndex(prAdapter, prHifRxHdr->ucStaRecIdx);
    if (secCheckClassError(prAdapter, prSwRfb, prStaRec) == TRUE &&
            prAdapter->fgTestMode == FALSE) {
#if CFG_HIF_RX_STARVATION_WARNING
        prRxCtrl->u4QueuedCnt++;
#endif

        if ((prRetSwRfb = qmHandleRxPackets(prAdapter, prSwRfb)) != NULL) {
            do {
                // save next first
                prNextSwRfb = (P_SW_RFB_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prRetSwRfb);
                switch (prRetSwRfb->eDst) {
                    case RX_PKT_DESTINATION_HOST:
                        nicRxProcessPktWithoutReorder(prAdapter, prRetSwRfb);
                        break;

                    case RX_PKT_DESTINATION_FORWARD:
                        nicRxProcessForwardPkt(prAdapter, prRetSwRfb);
                        break;

                    case RX_PKT_DESTINATION_HOST_WITH_FORWARD:
                        nicRxProcessGOBroadcastPkt(prAdapter, prRetSwRfb);
                        break;

                    case RX_PKT_DESTINATION_NULL:
                        MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_69, "RX_PKT_DESTINATION_NULL");
                        nicRxReturnRFB(prAdapter, prRetSwRfb);
                        break;

                    default:
                        break;
                }
#if CFG_HIF_RX_STARVATION_WARNING
                prRxCtrl->u4DequeuedCnt++;
#endif
                prRetSwRfb = prNextSwRfb;
            } while (prRetSwRfb);
        }
    } else {
        MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_70, "secCheckClassError() == FALSE");
        nicRxReturnRFB(prAdapter, prSwRfb);
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Process HIF event packet
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
*
* @return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessEventPacket(
    IN P_ADAPTER_T    prAdapter,
    IN OUT P_SW_RFB_T prSwRfb
)
{
    P_CMD_INFO_T prCmdInfo;
    P_MSDU_INFO_T prMsduInfo;
    P_WIFI_EVENT_T prEvent;
    P_GLUE_INFO_T prGlueInfo;

    // DEBUGFUNC("nicRxProcessEventPacket");
    //DBGLOG(INIT, TRACE, ("\n"));

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prEvent = (P_WIFI_EVENT_T) prSwRfb->pucRecvBuff;
    prGlueInfo = prAdapter->prGlueInfo;

    // Event Handling
    //MT5931_WAP_TRACE(MOD_WNDRV,TRACE_INFO, "RX EVENT ID: 0x%X", prEvent->ucEID);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "RX EVENT ID: 0x%X", prEvent->ucEID);

#if CFG_ENABLE_TXRX_PATH_LOG
    dumpMemory8(prSwRfb->pucRecvBuff, prEvent->u2PacketLen);
#endif
    switch (prEvent->ucEID) {
        case EVENT_ID_CMD_RESULT:
            prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

            if (prCmdInfo != NULL) {
                P_EVENT_CMD_RESULT prCmdResult;
                prCmdResult = (P_EVENT_CMD_RESULT)((PUINT_8)prEvent + EVENT_HDR_SIZE);

                /* CMD_RESULT should be only in response to Set commands */
                ASSERT(prCmdInfo->fgSetQuery == FALSE || prCmdInfo->fgNeedResp == TRUE);

                if (prCmdResult->ucStatus == 0) { // success
                    if (prCmdInfo->pfCmdDoneHandler) {
                        prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
                    } else if (prCmdInfo->fgIsOid == TRUE) {
                        kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
                    }
                } else if (prCmdResult->ucStatus == 1) { // reject
                    if (prCmdInfo->fgIsOid == TRUE) {
                        kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_FAILURE);
                    }
                } else if (prCmdResult->ucStatus == 2) { // unknown CMD
                    if (prCmdInfo->fgIsOid == TRUE) {
                        kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_NOT_SUPPORTED);
                    }
                }

                // return prCmdInfo
                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
            }

            break;

#if 0
        case EVENT_ID_CONNECTION_STATUS:
            /* OBSELETE */
        {
            P_EVENT_CONNECTION_STATUS prConnectionStatus;
            prConnectionStatus = (P_EVENT_CONNECTION_STATUS)(prEvent->aucBuffer);

            DbgPrint("RX EVENT: EVENT_ID_CONNECTION_STATUS = %d\n", prConnectionStatus->ucMediaStatus);
            if (prConnectionStatus->ucMediaStatus == PARAM_MEDIA_STATE_DISCONNECTED) { // disconnected
                if (kalGetMediaStateIndicated(prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED) {

                    kalIndicateStatusAndComplete(prGlueInfo,
                                                 WLAN_STATUS_MEDIA_DISCONNECT,
                                                 NULL,
                                                 0);

                    prAdapter->rWlanInfo.u4SysTime = kalGetTimeTick();
                }
            } else if (prConnectionStatus->ucMediaStatus == PARAM_MEDIA_STATE_CONNECTED) { // connected
                prAdapter->rWlanInfo.u4SysTime = kalGetTimeTick();

                // fill information for association result
                prAdapter->rWlanInfo.rCurrBssId.rSsid.u4SsidLen
                    = prConnectionStatus->ucSsidLen;
                kalMemCopy(prAdapter->rWlanInfo.rCurrBssId.rSsid.aucSsid,
                           prConnectionStatus->aucSsid,
                           prConnectionStatus->ucSsidLen);

                kalMemCopy(prAdapter->rWlanInfo.rCurrBssId.arMacAddress,
                           prConnectionStatus->aucBssid,
                           MAC_ADDR_LEN);

                prAdapter->rWlanInfo.rCurrBssId.u4Privacy
                    = prConnectionStatus->ucEncryptStatus; // @FIXME
                prAdapter->rWlanInfo.rCurrBssId.rRssi
                    = 0; //@FIXME
                prAdapter->rWlanInfo.rCurrBssId.eNetworkTypeInUse
                    = PARAM_NETWORK_TYPE_AUTOMODE; //@FIXME
                prAdapter->rWlanInfo.rCurrBssId.rConfiguration.u4BeaconPeriod
                    = prConnectionStatus->u2BeaconPeriod;
                prAdapter->rWlanInfo.rCurrBssId.rConfiguration.u4ATIMWindow
                    = prConnectionStatus->u2ATIMWindow;
                prAdapter->rWlanInfo.rCurrBssId.rConfiguration.u4DSConfig
                    = prConnectionStatus->u4FreqInKHz;
                prAdapter->rWlanInfo.ucNetworkType
                    = prConnectionStatus->ucNetworkType;

                switch (prConnectionStatus->ucInfraMode) {
                    case 0:
                        prAdapter->rWlanInfo.rCurrBssId.eOpMode = NET_TYPE_IBSS;
                        break;
                    case 1:
                        prAdapter->rWlanInfo.rCurrBssId.eOpMode = NET_TYPE_INFRA;
                        break;
                    case 2:
                    default:
                        prAdapter->rWlanInfo.rCurrBssId.eOpMode = NET_TYPE_AUTO_SWITCH;
                        break;
                }
                // always indicate to OS according to MSDN (re-association/roaming)
                kalIndicateStatusAndComplete(prGlueInfo,
                                             WLAN_STATUS_MEDIA_CONNECT,
                                             NULL,
                                             0);
            }
        }
        break;

        case EVENT_ID_SCAN_RESULT:
            /* OBSELETE */
            break;
#endif

        case EVENT_ID_RX_ADDBA:
            /* The FW indicates that an RX BA agreement will be established */
            qmHandleEventRxAddBa(prAdapter, prEvent);
            break;

        case EVENT_ID_RX_DELBA:
            /* The FW indicates that an RX BA agreement has been deleted */
            qmHandleEventRxDelBa(prAdapter, prEvent);
            break;

        case EVENT_ID_LINK_QUALITY:
            nicUpdateLinkQuality(prAdapter, NETWORK_TYPE_AIS_INDEX, (P_EVENT_LINK_QUALITY)(prEvent->aucBuffer));

            /* command response handling */
            prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

            if (prCmdInfo != NULL) {
                if (prCmdInfo->pfCmdDoneHandler) {
                    prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
                } else if (prCmdInfo->fgIsOid) {
                    kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
                }

                // return prCmdInfo
                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
            }

#ifndef LINUX
            if (prAdapter->rWlanInfo.eRssiTriggerType == ENUM_RSSI_TRIGGER_GREATER &&
                    prAdapter->rWlanInfo.rRssiTriggerValue >= (PARAM_RSSI)(prAdapter->rLinkQuality.cRssi)) {
                prAdapter->rWlanInfo.eRssiTriggerType = ENUM_RSSI_TRIGGER_TRIGGERED;

                kalIndicateStatusAndComplete(prGlueInfo,
                                             WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
                                             (PVOID) & (prAdapter->rWlanInfo.rRssiTriggerValue), sizeof(PARAM_RSSI));
            } else if (prAdapter->rWlanInfo.eRssiTriggerType == ENUM_RSSI_TRIGGER_LESS &&
                       prAdapter->rWlanInfo.rRssiTriggerValue <= (PARAM_RSSI)(prAdapter->rLinkQuality.cRssi)) {
                prAdapter->rWlanInfo.eRssiTriggerType = ENUM_RSSI_TRIGGER_TRIGGERED;

                kalIndicateStatusAndComplete(prGlueInfo,
                                             WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
                                             (PVOID) & (prAdapter->rWlanInfo.rRssiTriggerValue), sizeof(PARAM_RSSI));
            }
#endif

            break;

        case EVENT_ID_MIC_ERR_INFO: {
            P_EVENT_MIC_ERR_INFO prMicError;
            //P_PARAM_AUTH_EVENT_T prAuthEvent;
            P_STA_RECORD_T prStaRec;

            //DBGLOG(RSN, EVENT, ("EVENT_ID_MIC_ERR_INFO\n"));
            kal_trace(TRACE_GROUP_10 , EVENT_RSN_ID_MIC_ERR_INFO);

            prMicError = (P_EVENT_MIC_ERR_INFO)(prEvent->aucBuffer);
            prStaRec = cnmGetStaRecByAddress(prAdapter,
                                             (UINT_8) NETWORK_TYPE_AIS_INDEX,
                                             prAdapter->rWlanInfo.rCurrBssId.arMacAddress);
            //ASSERT(prStaRec);     /* NOTE(Nelson): StaRec would be updated in AIS_JOIN state, but IBSS mode would not to AIS_JOIN state */

            if (prStaRec) {
                rsnTkipHandleMICFailure(prAdapter, prStaRec, (BOOLEAN)prMicError->u4Flags);
            } else {
                // DBGLOG(RSN, INFO, ("No STA rec!!\n"));
                kal_trace(TRACE_WARNING , INFO_RSN_NO_STA_REC);
            }
#if 0
            prAuthEvent = (P_PARAM_AUTH_EVENT_T)prAdapter->aucIndicationEventBuffer;

            /* Status type: Authentication Event */
            prAuthEvent->rStatus.eStatusType = ENUM_STATUS_TYPE_AUTHENTICATION;

            /* Authentication request */
            prAuthEvent->arRequest[0].u4Length = sizeof(PARAM_AUTH_REQUEST_T);
            kalMemCopy((PVOID)prAuthEvent->arRequest[0].arBssid,
                       (PVOID)prAdapter->rWlanInfo.rCurrBssId.arMacAddress, /* whsu:Todo? */
                       PARAM_MAC_ADDR_LEN);

            if (prMicError->u4Flags != 0) {
                prAuthEvent->arRequest[0].u4Flags = PARAM_AUTH_REQUEST_GROUP_ERROR;
            } else {
                prAuthEvent->arRequest[0].u4Flags = PARAM_AUTH_REQUEST_PAIRWISE_ERROR;
            }

            kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                         WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
                                         (PVOID)prAuthEvent,
                                         sizeof(PARAM_STATUS_INDICATION_T) + sizeof(PARAM_AUTH_REQUEST_T));
#endif
        }
        break;

        case EVENT_ID_ASSOC_INFO: {
            P_EVENT_ASSOC_INFO prAssocInfo;
            prAssocInfo = (P_EVENT_ASSOC_INFO)(prEvent->aucBuffer);

            kalHandleAssocInfo(prAdapter->prGlueInfo, prAssocInfo);
        }
        break;

        case EVENT_ID_802_11_PMKID: {
            P_PARAM_AUTH_EVENT_T           prAuthEvent;
            PUINT_8                        cp;
            UINT_32                        u4LenOfUsedBuffer;

            prAuthEvent = (P_PARAM_AUTH_EVENT_T)ALIGN_4((UINT_32)&prAdapter->aucIndicationEventBuffer[0]);

            prAuthEvent->rStatus.eStatusType = ENUM_STATUS_TYPE_CANDIDATE_LIST;

            u4LenOfUsedBuffer = (UINT_32)(prEvent->u2PacketLen - 8);

            prAuthEvent->arRequest[0].u4Length = u4LenOfUsedBuffer;

            cp = (PUINT_8)&prAuthEvent->arRequest[0];

            /* Status type: PMKID Candidatelist Event */
            kalMemCopy(cp, (P_EVENT_PMKID_CANDIDATE_LIST_T)(prEvent->aucBuffer), prEvent->u2PacketLen - 8);

            kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                         WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
                                         (PVOID)prAuthEvent,
                                         sizeof(PARAM_STATUS_INDICATION_T) + u4LenOfUsedBuffer);
        }
        break;

#if 0
        case EVENT_ID_ACTIVATE_STA_REC_T: {
            P_EVENT_ACTIVATE_STA_REC_T prActivateStaRec;
            prActivateStaRec = (P_EVENT_ACTIVATE_STA_REC_T)(prEvent->aucBuffer);

            DbgPrint("RX EVENT: EVENT_ID_ACTIVATE_STA_REC_T Index:%d, MAC:["MACSTR"]\n",
                     prActivateStaRec->ucStaRecIdx,
                     MAC2STR(prActivateStaRec->aucMacAddr));

            qmActivateStaRec(prAdapter,
                             (UINT_32)prActivateStaRec->ucStaRecIdx,
                             ((prActivateStaRec->fgIsQoS) ? TRUE : FALSE),
                             prActivateStaRec->ucNetworkTypeIndex,
                             ((prActivateStaRec->fgIsAP) ? TRUE : FALSE),
                             prActivateStaRec->aucMacAddr);

        }
        break;

        case EVENT_ID_DEACTIVATE_STA_REC_T: {
            P_EVENT_DEACTIVATE_STA_REC_T prDeactivateStaRec;
            prDeactivateStaRec = (P_EVENT_DEACTIVATE_STA_REC_T)(prEvent->aucBuffer);

            DbgPrint("RX EVENT: EVENT_ID_DEACTIVATE_STA_REC_T Index:%d, MAC:["MACSTR"]\n",
                     prDeactivateStaRec->ucStaRecIdx);

            qmDeactivateStaRec(prAdapter,
                               prDeactivateStaRec->ucStaRecIdx);
        }
        break;
#endif

        case EVENT_ID_SCAN_DONE: {
            P_EVENT_SCAN_DONE prScanDone;

            prScanDone = (P_EVENT_SCAN_DONE)(prEvent->aucBuffer);
            MT5931_TRACE(TRACE_GROUP_6, MT5931_INFO_72, "[EVENT_ID_SCAN_DONE]ucSeqNum %d", prScanDone->ucSeqNum);
            scnEventScanDone(prAdapter, prScanDone->ucSeqNum);
        }
        break;

        case EVENT_ID_TX_DONE: {
            P_EVENT_TX_DONE_T prTxDone;
            prTxDone = (P_EVENT_TX_DONE_T)(prEvent->aucBuffer);

            MT5931_TRACE(TRACE_GROUP_6, MT5931_INFO_73, "RX EVENT: EVENT_ID_TX_DONE PacketSeq:%d, ucStatus: %d\n",
                         prTxDone->ucPacketSeq, prTxDone->ucStatus);

            /* call related TX Done Handler */
            prMsduInfo = nicGetPendingTxMsduInfo(prAdapter, prTxDone->ucPacketSeq);

            if (prMsduInfo) {
                prMsduInfo->pfTxDoneHandler(prAdapter, prMsduInfo, (ENUM_TX_RESULT_CODE_T)(prTxDone->ucStatus));

                cnmMgtPktFree(prAdapter, prMsduInfo);
            }
        }
        break;

        case EVENT_ID_SLEEPY_NOTIFY: {
            P_EVENT_SLEEPY_NOTIFY prEventSleepyNotify;
            prEventSleepyNotify = (P_EVENT_SLEEPY_NOTIFY)(prEvent->aucBuffer);

            //DBGLOG(RX, INFO, ("ucSleepyState = %d\n", prEventSleepyNotify->ucSleepyState));

            prAdapter->fgWiFiInSleepyState = (BOOLEAN)(prEventSleepyNotify->ucSleepyState);
        }
        break;
        case EVENT_ID_BT_OVER_WIFI:
#if CFG_ENABLE_BT_OVER_WIFI
        {
            UINT_8 aucTmp[sizeof(AMPC_EVENT) + sizeof(BOW_LINK_DISCONNECTED)];
            P_EVENT_BT_OVER_WIFI prEventBtOverWifi;
            P_AMPC_EVENT prBowEvent;
            P_BOW_LINK_CONNECTED prBowLinkConnected;
            P_BOW_LINK_DISCONNECTED prBowLinkDisconnected;

            prEventBtOverWifi = (P_EVENT_BT_OVER_WIFI)(prEvent->aucBuffer);

            // construct event header
            prBowEvent = (P_AMPC_EVENT)aucTmp;

            if (prEventBtOverWifi->ucLinkStatus == 0) {
                // Connection
                prBowEvent->rHeader.ucEventId = BOW_EVENT_ID_LINK_CONNECTED;
                prBowEvent->rHeader.ucSeqNumber = 0;
                prBowEvent->rHeader.u2PayloadLength = sizeof(BOW_LINK_CONNECTED);

                // fill event body
                prBowLinkConnected = (P_BOW_LINK_CONNECTED)(prBowEvent->aucPayload);
                prBowLinkConnected->rChannel.ucChannelNum = prEventBtOverWifi->ucSelectedChannel;
                kalMemZero(prBowLinkConnected->aucPeerAddress, MAC_ADDR_LEN); //@FIXME

                kalIndicateBOWEvent(prAdapter->prGlueInfo, prBowEvent);
            } else {
                // Disconnection
                prBowEvent->rHeader.ucEventId = BOW_EVENT_ID_LINK_DISCONNECTED;
                prBowEvent->rHeader.ucSeqNumber = 0;
                prBowEvent->rHeader.u2PayloadLength = sizeof(BOW_LINK_DISCONNECTED);

                // fill event body
                prBowLinkDisconnected = (P_BOW_LINK_DISCONNECTED)(prBowEvent->aucPayload);
                prBowLinkDisconnected->ucReason = 0; //@FIXME
                kalMemZero(prBowLinkDisconnected->aucPeerAddress, MAC_ADDR_LEN); //@FIXME

                kalIndicateBOWEvent(prAdapter->prGlueInfo, prBowEvent);
            }
        }
        break;
#endif
        case EVENT_ID_STATISTICS:
            /* buffer statistics for further query */
            prAdapter->fgIsStatValid = TRUE;
            prAdapter->rStatUpdateTime = kalGetTimeTick();
            kalMemCopy(&prAdapter->rStatStruct, prEvent->aucBuffer, sizeof(EVENT_STATISTICS));

            /* command response handling */
            prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

            if (prCmdInfo != NULL) {
                if (prCmdInfo->pfCmdDoneHandler) {
                    prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
                } else if (prCmdInfo->fgIsOid) {
                    kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
                }

                // return prCmdInfo
                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
            }

            break;

        case EVENT_ID_CH_PRIVILEGE:
            cnmChMngrHandleChEvent(prAdapter, prEvent);
            break;

        case EVENT_ID_BSS_ABSENCE_PRESENCE:
            qmHandleEventBssAbsencePresence(prAdapter, prEvent);
            break;

        case EVENT_ID_STA_CHANGE_PS_MODE:
            qmHandleEventStaChangePsMode(prAdapter, prEvent);
            break;
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
        case EVENT_ID_STA_UPDATE_FREE_QUOTA:
            qmHandleEventStaUpdateFreeQuota(prAdapter, prEvent);
            break;
#endif
        case EVENT_ID_BSS_BEACON_TIMEOUT:
            if (prAdapter->fgDisBcnLostDetection == FALSE) {
                P_EVENT_BSS_BEACON_TIMEOUT_T prEventBssBeaconTimeout;
                prEventBssBeaconTimeout = (P_EVENT_BSS_BEACON_TIMEOUT_T)(prEvent->aucBuffer);

                if (prEventBssBeaconTimeout->ucNetTypeIndex == NETWORK_TYPE_AIS_INDEX) {
                    aisBssBeaconTimeout(prAdapter);
                }
#if CFG_ENABLE_WIFI_DIRECT
                else if ((prAdapter->fgIsP2PRegistered) &&
                         (prEventBssBeaconTimeout->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX)) {

                    prAdapter->rP2pFuncLkr.prP2pFsmRunEventBeaconTimeout(prAdapter);
                }
#endif
#if CFG_ENABLE_BT_OVER_WIFI
                else if (prEventBssBeaconTimeout->ucNetTypeIndex == NETWORK_TYPE_BOW_INDEX) {
                }
#endif
                else {
                    // DBGLOG(RX, ERROR, ("EVENT_ID_BSS_BEACON_TIMEOUT: (ucNetTypeIdx = %d)\n",
                    //             prEventBssBeaconTimeout->ucNetTypeIndex));

                    kal_trace(TRACE_ERROR , ERROR_RX_EVENT_ID_BSS_BEACON_TIMEOUT, prEventBssBeaconTimeout->ucNetTypeIndex);
                }
            }

            break;
        case EVENT_ID_UPDATE_NOA_PARAMS:
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                P_EVENT_UPDATE_NOA_PARAMS_T prEventUpdateNoaParam;
                prEventUpdateNoaParam = (P_EVENT_UPDATE_NOA_PARAMS_T)(prEvent->aucBuffer);

                if (prEventUpdateNoaParam->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX) {
                    prAdapter->rP2pFuncLkr.prP2pProcessEvent_UpdateNOAParam(prAdapter,
                            prEventUpdateNoaParam->ucNetTypeIndex,
                            prEventUpdateNoaParam);
                } else {
                    ASSERT(0);
                }
            }
#else
            //ASSERT(0);
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_74, "bypass EVENT_ID_UPDATE_NOA_PARAMS");
#endif
            break;

        case EVENT_ID_STA_AGING_TIMEOUT:
#if CFG_ENABLE_WIFI_DIRECT
        {
            P_EVENT_STA_AGING_TIMEOUT_T prEventStaAgingTimeout;
            P_STA_RECORD_T prStaRec;
            P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;

            prEventStaAgingTimeout = (P_EVENT_STA_AGING_TIMEOUT_T)(prEvent->aucBuffer);
            prStaRec = cnmGetStaRecByIndex(prAdapter, prEventStaAgingTimeout->ucStaRecIdx);
            if (prStaRec == NULL) {
                break;
            }


            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

            /* Call False Auth */
            if (prAdapter->fgIsP2PRegistered) {
                if (prAdapter->rP2pFuncLkr.prP2pFuncDisconnect) {
                    prAdapter->rP2pFuncLkr.prP2pFuncDisconnect(prAdapter, prStaRec, TRUE, REASON_CODE_DISASSOC_INACTIVITY);
                }
            }

            bssRemoveStaRecFromClientList(prAdapter, prBssInfo, prStaRec);
        }
#endif

#if CFG_ENABLE_WIFI_TETHERING
        {
            P_EVENT_STA_AGING_TIMEOUT_T prEventStaAgingTimeout;
            P_STA_RECORD_T prStaRec;
            P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;

            prEventStaAgingTimeout = (P_EVENT_STA_AGING_TIMEOUT_T)(prEvent->aucBuffer);
            prStaRec = cnmGetStaRecByIndex(prAdapter, prEventStaAgingTimeout->ucStaRecIdx);
            if (prStaRec == NULL) {
                break;
            }

            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

            //4
            if ((prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable) && (IS_STA_IN_P2P(prStaRec))) {
                wtFuncDisconnect(prAdapter, prStaRec, TRUE, REASON_CODE_DISASSOC_INACTIVITY);
                /*set inactive time */
                prStaRec->u4InactTime = 0xFFFF;
            }

            bssRemoveStaRecFromClientList(prAdapter, prBssInfo, prStaRec);
        }
#endif

        break;

        case EVENT_ID_AP_OBSS_STATUS:
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prRlmHandleObssStatusEventPkt);
                prAdapter->rP2pFuncLkr.prRlmHandleObssStatusEventPkt(
                    prAdapter,
                    (P_EVENT_AP_OBSS_STATUS_T) prEvent->aucBuffer);
            }
#endif
#if CFG_ENABLE_WIFI_TETHERING
            {
                P_EVENT_AP_OBSS_STATUS_T prAPObssStatus;
                prAPObssStatus = (P_EVENT_AP_OBSS_STATUS_T)(prEvent->aucBuffer);
                if ((prAdapter->prGlueInfo->rWtInfo.fgIsTetheringEnable) &&
                        (prAPObssStatus->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX)) {
                    rlmHandleObssStatusEventPkt(
                        prAdapter,
                        (P_EVENT_AP_OBSS_STATUS_T) prEvent->aucBuffer);
                }

            }
#endif /* CFG_ENABLE_WIFI_TETHERING */
            break;

        case EVENT_ID_ROAMING_STATUS:
#if CFG_SUPPORT_ROAMING
        {
            P_ROAMING_PARAM_T prParam;

            if (prAdapter->rWifiVar.rRoamingInfo.fgBriefEnableRoamingDynamically) {
                prParam = (P_ROAMING_PARAM_T)(prEvent->aucBuffer);
                roamingFsmProcessEvent(prAdapter, prParam);
            } else {
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "Roaming is briefly disabled, ignore EVENT:EVENT_ID_ROAMING_STATUS[0x%x]", prEvent->ucEID);
            }
        }
#endif /* CFG_SUPPORT_ROAMING */
        break;
#if CFG_ENABLE_WIFI_DIRECT && CFG_SUPPORT_ANTI_PIRACY
        case EVENT_ID_SEC_CHECK_RSP:
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prP2pHandleSecCheckRsp);

                prAdapter->rP2pFuncLkr.prP2pHandleSecCheckRsp(
                    prAdapter,
                    (PUINT_8) prEvent->aucBuffer,
                    prEvent->u2PacketLen - EVENT_HDR_SIZE);

                /* command response handling */
                prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

                if (prCmdInfo != NULL) {
                    if (prCmdInfo->pfCmdDoneHandler) {
                        prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
                    } else if (prCmdInfo->fgIsOid) {
                        kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
                    }

                    // return prCmdInfo
                    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
                }
            }
            break;
#endif
        case EVENT_ID_SEND_DEAUTH:
#if DBG
        {
            P_WLAN_MAC_HEADER_T prWlanMacHeader;

            prWlanMacHeader = (P_WLAN_MAC_HEADER_T)&prEvent->aucBuffer[0];
            // DBGLOG(RX, INFO, ("nicRx: aucAddr1: "MACSTR"\n", MAC2STR(prWlanMacHeader->aucAddr1)));
            //  DBGLOG(RX, INFO, ("nicRx: aucAddr2: "MACSTR"\n", MAC2STR(prWlanMacHeader->aucAddr2)));


            kal_trace(TRACE_DISCONN, INFO_RX_aucAddr1, MAC2STR(prWlanMacHeader->aucAddr1));
            kal_trace(TRACE_DISCONN , INFO_RX_aucAddr2, MAC2STR(prWlanMacHeader->aucAddr2));
        }
#endif
            /* receive packets without StaRec */
        prSwRfb->pvHeader = (void *)(P_WLAN_MAC_HEADER_T)&prEvent->aucBuffer[0];
        if (WLAN_STATUS_SUCCESS == authSendDeauthFrame(prAdapter,
                NULL,
                prSwRfb,
                REASON_CODE_CLASS_3_ERR,
                (PFN_TX_DONE_HANDLER)NULL)) {
            kal_trace(TRACE_DISCONN , INFO_RX_Deauth_Error);
        }
        break;

#if CFG_SUPPORT_BCM && CFG_SUPPORT_BCM_BWCS
        case EVENT_ID_UPDATE_BWCS_STATUS: {
            P_PTA_IPC_T prEventBwcsStatus;

            prEventBwcsStatus = (P_PTA_IPC_T)(prEvent->aucBuffer);

            kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                         WLAN_STATUS_BWCS_UPDATE,
                                         (PVOID) prEventBwcsStatus,
                                         sizeof(PTA_IPC_T));
        }

        break;
#endif

        case EVENT_ID_ACCESS_REG:
        case EVENT_ID_NIC_CAPABILITY:
        case EVENT_ID_BASIC_CONFIG:
        case EVENT_ID_MAC_MCAST_ADDR:
        case EVENT_ID_ACCESS_EEPROM:
        case EVENT_ID_TEST_STATUS:
        default:
            prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

            if (prCmdInfo != NULL) {
                if (prCmdInfo->pfCmdDoneHandler) {
                    prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
                } else if (prCmdInfo->fgIsOid) {
                    kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
                }

                // return prCmdInfo
                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
            }

            break;
    }

    nicRxReturnRFB(prAdapter, prSwRfb);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief nicRxProcessMgmtPacket is used to dispatch management frames
*        to corresponding modules
*
* @param prAdapter Pointer to the Adapter structure.
* @param prSWRfb the RFB to receive rx data
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessMgmtPacket(
    IN P_ADAPTER_T    prAdapter,
    IN OUT P_SW_RFB_T prSwRfb
)
{
    UINT_8 ucSubtype;

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    nicRxFillRFB(prAdapter, prSwRfb);

    ucSubtype = (*(PUINT_8)(prSwRfb->pvHeader) & MASK_FC_SUBTYPE) >> OFFSET_OF_FC_SUBTYPE;

#if DBG && 0
    {
        P_HIF_RX_HEADER_T   prHifRxHdr;

        prHifRxHdr = prSwRfb->prHifRxHdr;
        //if (prAdapter->rRxCtrl.u4RxPktsDumpTypeMask & BIT(HIF_RX_PKT_TYPE_MANAGEMENT)) {
        LOG_FUNC("QM RX MGT: net %u sta idx %u wlan idx %u ssn %u ptype %u subtype %u 11 %u\n",
                 HIF_RX_HDR_GET_NETWORK_IDX(prHifRxHdr),
                 prHifRxHdr->ucStaRecIdx,
                 prSwRfb->ucWlanIdx,
                 HIF_RX_HDR_GET_SN(prHifRxHdr),  /* The new SN of the frame */
                 prSwRfb->ucPacketType,
                 ucSubtype,
                 HIF_RX_HDR_GET_80211_FLAG(prHifRxHdr));

        dumpMemory8((PUINT_8)prSwRfb->pvHeader, prSwRfb->u2PacketLen);
        //}
    }
#endif

    if (prAdapter->fgTestMode == FALSE) {
#if CFG_MGMT_FRAME_HANDLING
        if (apfnProcessRxMgtFrame[ucSubtype]) {
            switch (apfnProcessRxMgtFrame[ucSubtype](prAdapter, prSwRfb)) {
                case WLAN_STATUS_PENDING:
                    return;
                case WLAN_STATUS_SUCCESS:
                case WLAN_STATUS_FAILURE:
                    break;

                default:
                    ASSERT(0);
                    break;
            }
        }
#endif
    }

    nicRxReturnRFB(prAdapter, prSwRfb);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief nicProcessRFBs is used to process RFBs in the rReceivedRFBList queue.
*
* @param prAdapter Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxProcessRFBs(
    IN  P_ADAPTER_T prAdapter
)
{
    P_RX_CTRL_T prRxCtrl;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
#if defined (WLAN_ZERO_COPY)
    wndrv_pkt_descriptor_struct *wndrv_pkt_p;
#endif
    KAL_SPIN_LOCK_DECLARATION();

    //DEBUGFUNC("nicRxProcessRFBs");

    ASSERT(prAdapter);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    prRxCtrl->ucNumIndPacket = 0;
    prRxCtrl->ucNumRetainedPacket = 0;

    do {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        QUEUE_REMOVE_HEAD(&prRxCtrl->rReceivedRfbList, prSwRfb, P_SW_RFB_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

        if (prSwRfb) {
#if defined (WLAN_ZERO_COPY)
            wndrv_pkt_p = wndrv_get_pkt_descriptor((peer_buff_struct *)(prSwRfb->pvPacket));
#endif
#ifdef __WIFI_SNIFFER_SUPPORT__
            if (prAdapter->u1SnifferOnOff) { //sniffer mode
                if ((prSwRfb->ucPacketType == HIF_RX_PKT_TYPE_DATA) || (prSwRfb->ucPacketType == HIF_RX_PKT_TYPE_MANAGEMENT)) {
                    nicRxProcessSnifferedPacket(prAdapter, prSwRfb);// indicate to upper
                    return;
                }
            }
#endif
            switch (prSwRfb->ucPacketType) {
                case HIF_RX_PKT_TYPE_DATA:
#if defined (WLAN_ZERO_COPY)
                    if (!wndrv_pkt_p->flc_mode) {
                        nicRxReturnRFB(prAdapter, prSwRfb);
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "No flc buffer ,Drop packet\n");
                    } else
#endif
                    {
#if CFG_ENABLE_THROUGHPUT_DBG
                        SLA_CustomLogging("Rpd", SA_start);
#endif
                        //ILMTraceCheck(30);
                        nicRxProcessDataPacket(prAdapter, prSwRfb);
                        //ILMTraceCheck(39);
                        prAdapter->u4RxDataResvTime = kalGetTimeTick();
                    }
                    break;

                case HIF_RX_PKT_TYPE_EVENT:
#if CFG_ENABLE_THROUGHPUT_DBG
                    SLA_CustomLogging("Rpe", SA_start);
#endif
                    //ILMTraceCheck(40);
                    nicRxProcessEventPacket(prAdapter, prSwRfb);
                    //ILMTraceCheck(49);
#if CFG_ENABLE_THROUGHPUT_DBG
                    SLA_CustomLogging("Rpe", SA_stop);
#endif
                    break;

                case HIF_RX_PKT_TYPE_TX_LOOPBACK:
                    // DBGLOG(RX, ERROR, ("ucPacketType = %d\n", prSwRfb->ucPacketType));
                    MT5931_TRACE(TRACE_TXRX, MT5931_INFO_76, "HIF_RX_PKT_TYPE_TX_LOOP: BACKucPacketType = %d\n", prSwRfb->ucPacketType);
                    break;

                case HIF_RX_PKT_TYPE_MANAGEMENT:
#if CFG_ENABLE_THROUGHPUT_DBG
                    SLA_CustomLogging("Rpm", SA_start);
#endif
                    //ILMTraceCheck(50);
                    nicRxProcessMgmtPacket(prAdapter, prSwRfb);
                    //ILMTraceCheck(59);
                    prAdapter->u4RxDataResvTime = kalGetTimeTick();
#if CFG_ENABLE_THROUGHPUT_DBG
                    SLA_CustomLogging("Rpm", SA_stop);
#endif
                    break;

                default:
                    // DBGLOG(RX, ERROR, ("ucPacketType = %d\n", prSwRfb->ucPacketType));
                    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_77, "ucPacketType = %d\n", prSwRfb->ucPacketType);
                    break;
            }
        } else {
            break;
        }
    } while (TRUE);

    if (prRxCtrl->ucNumIndPacket > 0) {
        RX_ADD_CNT(prRxCtrl, RX_DATA_INDICATION_COUNT, prRxCtrl->ucNumIndPacket);
        RX_ADD_CNT(prRxCtrl, RX_DATA_RETAINED_COUNT, prRxCtrl->ucNumRetainedPacket);

        //DBGLOG(RX, INFO, ("%d packets indicated, Retained cnt = %d\n",
        //    prRxCtrl->ucNumIndPacket, prRxCtrl->ucNumRetainedPacket));
#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("pdi", SA_start);
#endif
#if CFG_NATIVE_802_11
        kalRxIndicatePkts(prAdapter->prGlueInfo, (UINT_32)prRxCtrl->ucNumIndPacket, (UINT_32)prRxCtrl->ucNumRetainedPacket);
#else
        kalRxIndicatePkts(prAdapter->prGlueInfo, prRxCtrl->apvIndPacket, (UINT_32)prRxCtrl->ucNumIndPacket);
#endif
#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("pdi", SA_stop);
        SLA_CustomLogging("Rpd", SA_stop);
#endif
    }

} /* end of nicRxProcessRFBs() */


#if !CFG_SDIO_INTR_ENHANCE
/*----------------------------------------------------------------------------*/
/*!
* @brief Read the rx data from data port and setup RFB
*
* @param prAdapter pointer to the Adapter handler
* @param prSWRfb the RFB to receive rx data
*
* @retval WLAN_STATUS_SUCCESS: SUCCESS
* @retval WLAN_STATUS_FAILURE: FAILURE
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRxReadBuffer(
    IN P_ADAPTER_T prAdapter,
    IN OUT P_SW_RFB_T prSwRfb
)
{
    P_RX_CTRL_T prRxCtrl;
    PUINT_8 pucBuf;
    P_HIF_RX_HEADER_T prHifRxHdr;
    UINT_32 u4PktLen = 0, u4ReadBytes;
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
    BOOL fgResult = TRUE;
    UINT_32 u4RegValue;
    UINT_32 rxNum;
    //UINT_32 u4TotalLen;
    //PUINT_8 pucDmaBuf;
    //UINT_32 u4DmaLen;
    //UINT_32 u4DmaOffset;


    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);


#if __WIFI_HIF_SDIO__
    pucBuf = &wndrv_cmd_temp_buf[0];
#else
    pucBuf = prSwRfb->pucRecvBuff;
#endif
    prHifRxHdr = prSwRfb->prHifRxHdr;
    ASSERT(pucBuf);
    //DBGLOG(RX, TRACE, ("pucBuf= 0x%x, prHifRxHdr= 0x%x\n", pucBuf, prHifRxHdr));
    //kal_trace(TRACE_PEER ,TRACE_RX_pucBuf, pucBuf, prHifRxHdr);

    do {
        /* Read the RFB DW length and packet length */
        HAL_MCR_RD(prAdapter, MCR_WRPLR, &u4RegValue);
#if CFG_ENABLE_TXRX_PATH_LOG
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_TXRX, "Read MCR_WRPLR:0x%x", u4RegValue);
#endif
        if (!fgResult) {
            // DBGLOG(RX, ERROR, ("Read RX Packet Lentgh Error\n"));
            kal_trace(TRACE_ERROR , ERROR_RX_Lentgh_Error);
            return WLAN_STATUS_FAILURE;
        }

        //20091021 move the line to get the HIF RX header (for RX0/1)
        if (u4RegValue == 0) {
            //DBGLOG(RX, ERROR, ("No RX packet\n"));
            kal_trace(TRACE_ERROR , ERROR_RX_No_RX);
            return WLAN_STATUS_FAILURE;
        }

        u4PktLen = u4RegValue & BITS(0, 15);
        if (u4PktLen != 0) {
            rxNum = 0;
        } else {
            rxNum = 1;
            u4PktLen = (u4RegValue & BITS(16, 31)) >> 16;
        }

        //kal_trace(TRACE_GROUP_4 ,TRACE_RX_u4PktLen, rxNum, u4PktLen);

        //4 <4> Read Entire RFB and packet, include HW appended DW (Checksum Status)
        u4ReadBytes = ALIGN_4(u4PktLen) + 4;
#if __WIFI_HIF_SDIO__
        if (Wifi_Query_Dynamic_Switch_Cachability()) {  //NOTE(Nelson): dest buffer address & size align for dma

#if CFG_ENABLE_THROUGHPUT_DBG
            SLA_CustomLogging("rb2", SA_start);
#endif
            HAL_READ_RX_PORT(prAdapter, rxNum, u4ReadBytes, pucBuf, CFG_RX_MAX_PKT_SIZE);
            kal_mem_cpy(prSwRfb->pucRecvBuff, pucBuf, u4ReadBytes);
#if CFG_ENABLE_THROUGHPUT_DBG
            SLA_CustomLogging("rb2", SA_stop);
#endif
        }
#else

#endif

        //20091021 move the line to get the HIF RX header
        //u4PktLen = (UINT_32)prHifRxHdr->u2PacketLen;
        if (u4PktLen != (UINT_32)prHifRxHdr->u2PacketLen) {
            //DBGLOG(RX, ERROR, ("Read u4PktLen = %d, prHifRxHdr->u2PacketLen: %d\n",
            //                     u4PktLen, prHifRxHdr->u2PacketLen));

            kal_trace(TRACE_ERROR , ERROR_RX_u4PktLen, u4PktLen, prHifRxHdr->u2PacketLen);
#if 0
            dumpMemory8((PUINT_8)prHifRxHdr, (prHifRxHdr->u2PacketLen > 4096) ? 4096 : prHifRxHdr->u2PacketLen);
#endif
            ASSERT(0);
        }
        /* u4PktLen is byte unit, not inlude HW appended DW */

        prSwRfb->ucPacketType = (UINT_8)(prHifRxHdr->u2PacketType & HIF_RX_HDR_PACKET_TYPE_MASK);
        // DBGLOG(RX, TRACE, ("ucPacketType = %d\n", prSwRfb->ucPacketType));
        //kal_trace(TRACE_GROUP_3 ,TRACE_RX_ucPacketType, prSwRfb->ucPacketType);
        prSwRfb->ucStaRecIdx = (UINT_8)(prHifRxHdr->ucStaRecIdx);

        /* fgResult will be updated in MACRO */
        if (!fgResult) {
            return WLAN_STATUS_FAILURE;
        }

        // DBGLOG(RX, TRACE, ("Dump RX buffer, length = 0x%x\n", u4ReadBytes));
        //kal_trace(TRACE_GROUP_3 ,TRACE_RX_Dump_RX_buffer, u4ReadBytes);
        //DBGLOG_MEM8(RX, TRACE, pucBuf, u4ReadBytes);
        //dumpMemory8((PUINT_8)pucBuf,u4ReadBytes);
    } while (FALSE);

    return u4Status;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Read frames from the data port, fill RFB
*        and put each frame into the rReceivedRFBList queue.
*
* @param prAdapter   Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxReceiveRFBs(
    IN P_ADAPTER_T  prAdapter
)
{
    P_RX_CTRL_T prRxCtrl;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
    //P_HIF_RX_HEADER_T prHifRxHdr;


    KAL_SPIN_LOCK_DECLARATION();


    ASSERT(prAdapter);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    do {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        QUEUE_REMOVE_HEAD(&prRxCtrl->rFreeSwRfbList, prSwRfb, P_SW_RFB_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

        if (!prSwRfb) {
            // DBGLOG(RX, TRACE, ("No More RFB\n"));
            MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_79, "nicRxReceiveRFBs: No More RFB");
            break;
        }

#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("Rsb", SA_start);
#endif
#ifdef WCN_MAUI
        nicRxSetupRFB(prAdapter, prSwRfb);
#endif
#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("Rsb", SA_stop);
#endif

#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("Rrb", SA_start);
#endif
        // need to consider
        if (nicRxReadBuffer(prAdapter, prSwRfb) == WLAN_STATUS_FAILURE) {
            // DBGLOG(RX, TRACE, ("halRxFillRFB failed\n"));
            MT5931_TRACE0(TRACE_WARNING, MT5931_INFO_80, "nicRxReceiveRFBs: halRxFillRFB failed");
            nicRxReturnRFB(prAdapter, prSwRfb);
            break;
        }
#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("Rrb", SA_stop);
#endif

        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        QUEUE_INSERT_TAIL(&prRxCtrl->rReceivedRfbList, &prSwRfb->rQueEntry);
        RX_INC_CNT(prRxCtrl, RX_MPDU_TOTAL_COUNT);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

        //prHifRxHdr = prSwRfb->prHifRxHdr;
#if 0
        u4HwAppendDW = *((PUINT_32)((UINT_32)prHifRxHdr +
                                    (UINT_32)(ALIGN_4(prHifRxHdr->u2PacketLen))));
#endif
    }
//    while (RX_STATUS_TEST_MORE_FLAG(u4HwAppendDW));
    while (FALSE);

    return;

} /* end of nicReceiveRFBs() */

#else
/*----------------------------------------------------------------------------*/
/*!
* @brief Read frames from the data port, fill RFB
*        and put each frame into the rReceivedRFBList queue.
*
* @param prAdapter      Pointer to the Adapter structure.
* @param u4DataPort     Specify which port to read
* @param u2RxLength     Specify to the the rx packet length in Byte.
* @param prSwRfb        the RFB to receive rx data.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/

WLAN_STATUS
nicRxEnhanceReadBuffer(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32      u4DataPort,
    IN UINT_16      u2RxLength,
    IN OUT P_SW_RFB_T prSwRfb
)
{
    P_RX_CTRL_T prRxCtrl;
    PUINT_8 pucBuf;
    P_HIF_RX_HEADER_T prHifRxHdr;
    UINT_32 u4PktLen = 0;
    WLAN_STATUS u4Status = WLAN_STATUS_FAILURE;
    BOOL fgResult = TRUE;

    DEBUGFUNC("nicRxEnhanceReadBuffer");

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    pucBuf = prSwRfb->pucRecvBuff;
    ASSERT(pucBuf);

    prHifRxHdr = prSwRfb->prHifRxHdr;
    ASSERT(prHifRxHdr);

    //DBGLOG(RX, TRACE, ("u2RxLength = %d\n", u2RxLength));

    do {
        //4 <1> Read RFB frame from MCR_WRDR0, include HW appended DW
        /* NOTE(Nelson): for MT5931 E3 eHPI short RX issue workaround start */
        if ((prAdapter->ucRevID >= MT5931_E3_REVERSION_ID) &&
                (ALIGN_4(u2RxLength + HIF_RX_HW_APPENDED_LEN) < E3_WORKAROND_RX_MIN_SIZE)) {
            HAL_MCR_WR(prAdapter, WEHTCR, E3_WORKAROND_RX_MIN_SIZE);    /* Set EHPI transaction count */
            HAL_READ_RX_PORT(prAdapter,
                             u4DataPort,
                             E3_WORKAROND_RX_MIN_SIZE,
                             pucBuf,
                             CFG_RX_MAX_PKT_SIZE);
        } else
            /* NOTE(Nelson): for MT5931 E3 eHPI short RX issue workaround end */
        {
            HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(u2RxLength + HIF_RX_HW_APPENDED_LEN)); /* Set EHPI transaction count */
            HAL_READ_RX_PORT(prAdapter,
                             u4DataPort,
                             ALIGN_4(u2RxLength + HIF_RX_HW_APPENDED_LEN),
                             pucBuf,
                             CFG_RX_MAX_PKT_SIZE);
        }
        HAL_MCR_WR(prAdapter, WEHTCR, 4); /* Reset EHPI transaction count */

        if (!fgResult) {
            //DBGLOG(RX, ERROR, ("Read RX Packet Lentgh Error\n"));
            kal_trace(TRACE_ERROR , ERROR_RX_Lentgh_Error);
            break;
        }

        u4PktLen = (UINT_32)(prHifRxHdr->u2PacketLen);
        //DBGLOG(RX, TRACE, ("u4PktLen = %d\n", u4PktLen));

        prSwRfb->ucPacketType = (UINT_8)(prHifRxHdr->u2PacketType & HIF_RX_HDR_PACKET_TYPE_MASK);
        //DBGLOG(RX, TRACE, ("ucPacketType = %d\n", prSwRfb->ucPacketType));

        prSwRfb->ucStaRecIdx = (UINT_8)(prHifRxHdr->ucStaRecIdx);

        //4 <2> if the RFB dw size or packet size is zero
        if (u4PktLen == 0) {
            //DBGLOG(RX, ERROR, ("Packet Length = %d\n", u4PktLen));
            kal_trace(TRACE_ERROR, ERROR_RX_Packet_Length, u4PktLen);
            ASSERT(0);
            break;
        }

        //4 <3> if the packet is too large or too small
        if (u4PktLen > CFG_RX_MAX_PKT_SIZE) {
            //DBGLOG(RX, TRACE, ("Read RX Packet Lentgh Error (%d)\n", u4PktLen));
            kal_trace(TRACE_GROUP_3 , TRACE_RX_READ_ERROR, u4PktLen);
            ASSERT(0);
            break;
        }

        u4Status = WLAN_STATUS_SUCCESS;
    } while (FALSE);

    //DBGLOG_MEM8(RX, TRACE, pucBuf, ALIGN_4(u2RxLength + HIF_RX_HW_APPENDED_LEN));
    return u4Status;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief Read frames from the data port for SDIO
*        I/F, fill RFB and put each frame into the rReceivedRFBList queue.
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxSDIOReceiveRFBs(
    IN  P_ADAPTER_T prAdapter
)
{
    P_SDIO_CTRL_T prSDIOCtrl;
    P_RX_CTRL_T prRxCtrl;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
    UINT_32 i, rxNum;
    UINT_16 u2RxPktNum, u2RxLength = 0, u2Tmp = 0;
    KAL_SPIN_LOCK_DECLARATION();

    DEBUGFUNC("nicRxSDIOReceiveRFBs");

    ASSERT(prAdapter);

    prSDIOCtrl = prAdapter->prSDIOCtrl;
    ASSERT(prSDIOCtrl);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    for (rxNum = 0 ; rxNum < 2 ; rxNum++) {
        u2RxPktNum = (rxNum == 0 ? prSDIOCtrl->rRxInfo.u.u2NumValidRx0Len : prSDIOCtrl->rRxInfo.u.u2NumValidRx1Len);

        if (u2RxPktNum == 0) {
            continue;
        }

        for (i = 0; i < u2RxPktNum; i++) {
            if (rxNum == 0) {
                HAL_READ_RX_LENGTH(prAdapter, &u2RxLength, &u2Tmp);
            } else if (rxNum == 1) {
                HAL_READ_RX_LENGTH(prAdapter, &u2Tmp, &u2RxLength);
            }

            if (!u2RxLength) {
                break;
            }


            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
            QUEUE_REMOVE_HEAD(&prRxCtrl->rFreeSwRfbList, prSwRfb, P_SW_RFB_T);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

            if (!prSwRfb) {
                //DBGLOG(RX, TRACE, ("No More RFB\n"));
                kal_trace(TRACE_GROUP_3 , TRACE_RX_No_RFB);
                break;
            }
            ASSERT(prSwRfb);

            if (nicRxEnhanceReadBuffer(prAdapter, rxNum, u2RxLength, prSwRfb) == WLAN_STATUS_FAILURE) {
                // DBGLOG(RX, TRACE, ("nicRxEnhanceRxReadBuffer failed\n"));
                kal_trace(TRACE_GROUP_3 , TRACE_RX_nicRxEnhanceRxReadBuffer);
                nicRxReturnRFB(prAdapter, prSwRfb);
                break;
            }

            //prSDIOCtrl->au4RxLength[i] = 0;

            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
            QUEUE_INSERT_TAIL(&prRxCtrl->rReceivedRfbList, &prSwRfb->rQueEntry);
            RX_INC_CNT(prRxCtrl, RX_MPDU_TOTAL_COUNT);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
        }
    }

    prSDIOCtrl->rRxInfo.u.u2NumValidRx0Len = 0;
    prSDIOCtrl->rRxInfo.u.u2NumValidRx1Len = 0;

    return;
}/* end of nicRxSDIOReceiveRFBs() */

#endif /* CFG_SDIO_INTR_ENHANCE */



#if CFG_SDIO_RX_AGG
/*----------------------------------------------------------------------------*/
/*!
* @brief Read frames from the data port for SDIO with Rx aggregation enabled
*        I/F, fill RFB and put each frame into the rReceivedRFBList queue.
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxSDIOAggReceiveRFBs(
    IN  P_ADAPTER_T prAdapter
)
{
    P_ENHANCE_MODE_DATA_STRUCT_T prEnhDataStr;
    P_RX_CTRL_T prRxCtrl;
    P_SDIO_CTRL_T prSDIOCtrl;
    P_SW_RFB_T prSwRfb = (P_SW_RFB_T)NULL;
    UINT_32 u4RxLength;
    UINT_32 i, rxNum;
    UINT_32 u4RxAggCount = 0, u4RxAggLength = 0;
    UINT_32 u4RxAvailAggLen, u4CurrAvailFreeRfbCnt;
    PUINT_8 pucSrcAddr;
    P_HIF_RX_HEADER_T prHifRxHdr;
    BOOL fgResult = TRUE;
    BOOLEAN fgIsRxEnhanceMode;
    UINT_16 u2RxPktNum;
#if CFG_SDIO_RX_ENHANCE
    UINT_32 u4MaxLoopCount = CFG_MAX_RX_ENHANCE_LOOP_COUNT;
#endif

    KAL_SPIN_LOCK_DECLARATION();

    DEBUGFUNC("nicRxSDIOAggReceiveRFBs");

    ASSERT(prAdapter);
    prEnhDataStr = prAdapter->prSDIOCtrl;
    prRxCtrl = &prAdapter->rRxCtrl;
    prSDIOCtrl = prAdapter->prSDIOCtrl;

#if CFG_SDIO_RX_ENHANCE
    fgIsRxEnhanceMode = TRUE;
#else
    fgIsRxEnhanceMode = FALSE;
#endif

    do {
#if CFG_SDIO_RX_ENHANCE
        /* to limit maximum loop for RX */
        u4MaxLoopCount--;
        if (u4MaxLoopCount == 0) {
            break;
        }
#endif

        if (prEnhDataStr->rRxInfo.u.u2NumValidRx0Len == 0 &&
                prEnhDataStr->rRxInfo.u.u2NumValidRx1Len == 0) {
            break;
        }

        for (rxNum = 0 ; rxNum < 2 ; rxNum++) {
            u2RxPktNum = (rxNum == 0 ? prEnhDataStr->rRxInfo.u.u2NumValidRx0Len : prEnhDataStr->rRxInfo.u.u2NumValidRx1Len);

            // if this assertion happened, it is most likely a F/W bug
            ASSERT(u2RxPktNum <= 16);

            if (u2RxPktNum > 16) {
                continue;
            }

            if (u2RxPktNum == 0) {
                continue;
            }

#if CFG_HIF_STATISTICS
            prRxCtrl->u4TotalRxAccessNum++;
            prRxCtrl->u4TotalRxPacketNum += u2RxPktNum;
#endif

            u4CurrAvailFreeRfbCnt = prRxCtrl->rFreeSwRfbList.u4NumElem;

            // if SwRfb is not enough, abort reading this time
            if (u4CurrAvailFreeRfbCnt < u2RxPktNum) {
#if CFG_HIF_RX_STARVATION_WARNING
                DbgPrint("FreeRfb is not enough: %d available, need %d\n", u4CurrAvailFreeRfbCnt, u2RxPktNum);
                DbgPrint("Queued Count: %d / Dequeud Count: %d\n", prRxCtrl->u4QueuedCnt, prRxCtrl->u4DequeuedCnt);
#endif
                continue;
            }

#if CFG_SDIO_RX_ENHANCE
            u4RxAvailAggLen = CFG_RX_COALESCING_BUFFER_SIZE - (sizeof(ENHANCE_MODE_DATA_STRUCT_T) + 4/* extra HW padding */);
#else
            u4RxAvailAggLen = CFG_RX_COALESCING_BUFFER_SIZE;
#endif
            u4RxAggCount = 0;

            for (i = 0; i < u2RxPktNum ; i++) {
                u4RxLength = (rxNum == 0 ?
                              (UINT_32)prEnhDataStr->rRxInfo.u.au2Rx0Len[i] :
                              (UINT_32)prEnhDataStr->rRxInfo.u.au2Rx1Len[i]);

                if (!u4RxLength) {
                    ASSERT(0);
                    break;
                }

                if (ALIGN_4(u4RxLength + HIF_RX_HW_APPENDED_LEN) < u4RxAvailAggLen) {
                    if (u4RxAggCount < u4CurrAvailFreeRfbCnt) {
                        u4RxAvailAggLen -= ALIGN_4(u4RxLength + HIF_RX_HW_APPENDED_LEN);
                        u4RxAggCount++;
                    } else {
                        // no FreeSwRfb for rx packet
                        ASSERT(0);
                        break;
                    }
                } else {
                    // CFG_RX_COALESCING_BUFFER_SIZE is not large enough
                    ASSERT(0);
                    break;
                }
            }

            u4RxAggLength = (CFG_RX_COALESCING_BUFFER_SIZE - u4RxAvailAggLen);
            //DBGLOG(RX, INFO, ("u4RxAggCount = %d, u4RxAggLength = %d\n",
            //            u4RxAggCount, u4RxAggLength));

            HAL_READ_RX_PORT(prAdapter,
                             rxNum,
                             u4RxAggLength,
                             prRxCtrl->pucRxCoalescingBufPtr,
                             CFG_RX_COALESCING_BUFFER_SIZE);
            if (!fgResult) {
                // DBGLOG(RX, ERROR, ("Read RX Agg Packet Error\n"));
                kal_trace(TRACE_ERROR , ERROR_RX_Agg_Packet_Error);
                continue;
            }

            pucSrcAddr = prRxCtrl->pucRxCoalescingBufPtr;
            for (i = 0; i < u4RxAggCount; i++) {
                UINT_16 u2PktLength;

                u2PktLength = (rxNum == 0 ?
                               prEnhDataStr->rRxInfo.u.au2Rx0Len[i] :
                               prEnhDataStr->rRxInfo.u.au2Rx1Len[i]);

                KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
                QUEUE_REMOVE_HEAD(&prRxCtrl->rFreeSwRfbList, prSwRfb, P_SW_RFB_T);
                KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

                ASSERT(prSwRfb);
                kalMemCopy(prSwRfb->pucRecvBuff, pucSrcAddr,
                           ALIGN_4(u2PktLength + HIF_RX_HW_APPENDED_LEN));

                prHifRxHdr = prSwRfb->prHifRxHdr;
                ASSERT(prHifRxHdr);

                prSwRfb->ucPacketType = (UINT_8)(prHifRxHdr->u2PacketType & HIF_RX_HDR_PACKET_TYPE_MASK);
                //DBGLOG(RX, TRACE, ("ucPacketType = %d\n", prSwRfb->ucPacketType));

                prSwRfb->ucStaRecIdx = (UINT_8)(prHifRxHdr->ucStaRecIdx);

                KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
                QUEUE_INSERT_TAIL(&prRxCtrl->rReceivedRfbList, &prSwRfb->rQueEntry);
                RX_INC_CNT(prRxCtrl, RX_MPDU_TOTAL_COUNT);
                KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

                pucSrcAddr += ALIGN_4(u2PktLength + HIF_RX_HW_APPENDED_LEN);
                //prEnhDataStr->au4RxLength[i] = 0;
            }

#if CFG_SDIO_RX_ENHANCE
            kalMemCopy(prAdapter->prSDIOCtrl, (pucSrcAddr + 4), sizeof(ENHANCE_MODE_DATA_STRUCT_T));

            /* do the same thing what nicSDIOReadIntStatus() does */
            if ((prSDIOCtrl->u4WHISR & WHISR_TX_DONE_INT) == 0 &&
                    (prSDIOCtrl->rTxInfo.au4WTSR[0] | prSDIOCtrl->rTxInfo.au4WTSR[1])) {
                prSDIOCtrl->u4WHISR |= WHISR_TX_DONE_INT;
            }

            if ((prSDIOCtrl->u4WHISR & BIT(31)) == 0 &&
                    HAL_GET_MAILBOX_READ_CLEAR(prAdapter) == TRUE &&
                    (prSDIOCtrl->u4RcvMailbox0 != 0 || prSDIOCtrl->u4RcvMailbox1 != 0)) {
                prSDIOCtrl->u4WHISR |= BIT(31);
            }

            /* dispatch to interrupt handler with RX bits masked */
            nicProcessIST_impl(prAdapter, prSDIOCtrl->u4WHISR & (~(WHISR_RX0_DONE_INT | WHISR_RX1_DONE_INT)));
#endif
        }

#if !CFG_SDIO_RX_ENHANCE
        prEnhDataStr->rRxInfo.u.u2NumValidRx0Len = 0;
        prEnhDataStr->rRxInfo.u.u2NumValidRx1Len = 0;
#endif
    } while ((prEnhDataStr->rRxInfo.u.u2NumValidRx0Len
              || prEnhDataStr->rRxInfo.u.u2NumValidRx1Len)
             && fgIsRxEnhanceMode);

    return;
}
#endif /* CFG_SDIO_RX_AGG */


/*----------------------------------------------------------------------------*/
/*!
* @brief Setup a RFB and allocate the os packet to the RFB
*
* @param prAdapter      Pointer to the Adapter structure.
* @param prSwRfb        Pointer to the RFB
*
* @retval WLAN_STATUS_SUCCESS
* @retval WLAN_STATUS_RESOURCES
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRxSetupRFB(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb
)
{
    PVOID   pvPacket;
    PUINT_8 pucRecvBuff;


    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    if (!prSwRfb->pvPacket) {
        kalMemZero(prSwRfb, sizeof(SW_RFB_T));
        pvPacket = kalPacketAlloc(prAdapter->prGlueInfo, CFG_RX_MAX_PKT_SIZE,
                                  &pucRecvBuff);
        if (pvPacket == NULL) {
            return WLAN_STATUS_RESOURCES;
        }

        prSwRfb->pvPacket = pvPacket;
        prSwRfb->pucRecvBuff = (PVOID)pucRecvBuff;
    } else {
        //TODO(Nelson): Debug for prSwRfb->pvPacket not free
        ASSERT(!prSwRfb->pvPacket);

        kalMemZero(((PUINT_8)prSwRfb + OFFSET_OF(SW_RFB_T, prHifRxHdr)),
                   (sizeof(SW_RFB_T) - OFFSET_OF(SW_RFB_T, prHifRxHdr)));
    }

    prSwRfb->prHifRxHdr = (P_HIF_RX_HEADER_T)(prSwRfb->pucRecvBuff);

    return WLAN_STATUS_SUCCESS;

} /* end of nicRxSetupRFB() */


/*----------------------------------------------------------------------------*/
/*!
* @brief This routine is called to put a RFB back onto the "RFB with Buffer" list
*        or "RFB without buffer" list according to pvPacket.
*
* @param prAdapter      Pointer to the Adapter structure.
* @param prSwRfb          Pointer to the RFB
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxReturnRFB(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb
)
{
    P_RX_CTRL_T prRxCtrl;
    P_QUE_ENTRY_T prQueEntry;


    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    ASSERT(prSwRfb);
    prRxCtrl = &prAdapter->rRxCtrl;
    prQueEntry = &prSwRfb->rQueEntry;

    ASSERT(prQueEntry);

    /* The processing on this RFB is done, so put it back on the tail of
       our list */
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);

    if (prSwRfb->pvPacket) {
        QUEUE_INSERT_TAIL(&prRxCtrl->rFreeSwRfbList, prQueEntry);
#if defined(WCN_MAUI)
        kalPacketFree(prAdapter->prGlueInfo, prSwRfb->pvPacket);
        prSwRfb->pvPacket = NULL;
#endif
    } else {
#if defined(WCN_MAUI)
        QUEUE_INSERT_TAIL(&prRxCtrl->rFreeSwRfbList, prQueEntry);
#else
        QUEUE_INSERT_TAIL(&prRxCtrl->rIndicatedRfbList, prQueEntry);
#endif
    }

    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
    return;
} /* end of nicRxReturnRFB() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Process rx interrupt. When the rx
*        Interrupt is asserted, it means there are frames in queue.
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicProcessRxInterrupt(
    IN  P_ADAPTER_T prAdapter
)
{
    ASSERT(prAdapter);
#ifdef WCN_MAUI
    /* NOTE(lcko): The IST process routine is executed several times.
        When there is no FLC2 buffer, return directly */
    //if (WNDRVPauseFlag) {
    //return;
    //}
#endif
    //ILMTraceCheck(20);
#if CFG_SDIO_INTR_ENHANCE
#if CFG_SDIO_RX_AGG
    nicRxSDIOAggReceiveRFBs(prAdapter);
#else
    nicRxSDIOReceiveRFBs(prAdapter);
#endif
#else
    nicRxReceiveRFBs(prAdapter);
#endif /* CFG_SDIO_INTR_ENHANCE */

    nicRxProcessRFBs(prAdapter);

    /* Nelson: workaround for interrupt polling */
    prAdapter->fgIsRxIntPoll = TRUE;
    //ILMTraceCheck(29);
    return;

} /* end of nicProcessRxInterrupt() */


#if CFG_TCP_IP_CHKSUM_OFFLOAD
/*----------------------------------------------------------------------------*/
/*!
* @brief Used to update IP/TCP/UDP checksum statistics of RX Module.
*
* @param prAdapter  Pointer to the Adapter structure.
* @param aeCSUM     The array of checksum result.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxUpdateCSUMStatistics(
    IN P_ADAPTER_T prAdapter,
    IN const ENUM_CSUM_RESULT_T aeCSUM[]
)
{
    P_RX_CTRL_T prRxCtrl;

    ASSERT(prAdapter);
    ASSERT(aeCSUM);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    if ((aeCSUM[CSUM_TYPE_IPV4] == CSUM_RES_SUCCESS) ||
            (aeCSUM[CSUM_TYPE_IPV6] == CSUM_RES_SUCCESS)) {

        RX_INC_CNT(prRxCtrl, RX_CSUM_IP_SUCCESS_COUNT);
    } else if ((aeCSUM[CSUM_TYPE_IPV4] == CSUM_RES_FAILED) ||
               (aeCSUM[CSUM_TYPE_IPV6] == CSUM_RES_FAILED)) {

        RX_INC_CNT(prRxCtrl, RX_CSUM_IP_FAILED_COUNT);
    } else if ((aeCSUM[CSUM_TYPE_IPV4] == CSUM_RES_NONE) &&
               (aeCSUM[CSUM_TYPE_IPV6] == CSUM_RES_NONE)) {

        RX_INC_CNT(prRxCtrl, RX_CSUM_UNKNOWN_L3_PKT_COUNT);
    } else {
        ASSERT(0);
    }

    if (aeCSUM[CSUM_TYPE_TCP] == CSUM_RES_SUCCESS) {
        RX_INC_CNT(prRxCtrl, RX_CSUM_TCP_SUCCESS_COUNT);
    } else if (aeCSUM[CSUM_TYPE_TCP] == CSUM_RES_FAILED) {
        RX_INC_CNT(prRxCtrl, RX_CSUM_TCP_FAILED_COUNT);
    } else if (aeCSUM[CSUM_TYPE_UDP] == CSUM_RES_SUCCESS) {
        RX_INC_CNT(prRxCtrl, RX_CSUM_UDP_SUCCESS_COUNT);
    } else if (aeCSUM[CSUM_TYPE_UDP] == CSUM_RES_FAILED) {
        RX_INC_CNT(prRxCtrl, RX_CSUM_UDP_FAILED_COUNT);
    } else if ((aeCSUM[CSUM_TYPE_UDP] == CSUM_RES_NONE) &&
               (aeCSUM[CSUM_TYPE_TCP] == CSUM_RES_NONE)) {

        RX_INC_CNT(prRxCtrl, RX_CSUM_UNKNOWN_L4_PKT_COUNT);
    } else {
        ASSERT(0);
    }

    return;
} /* end of nicRxUpdateCSUMStatistics() */
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used to query current status of RX Module.
*
* @param prAdapter      Pointer to the Adapter structure.
* @param pucBuffer      Pointer to the message buffer.
* @param pu4Count      Pointer to the buffer of message length count.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxQueryStatus(
    IN P_ADAPTER_T prAdapter,
    IN PUINT_8 pucBuffer,
    OUT PUINT_32 pu4Count
)
{
    P_RX_CTRL_T prRxCtrl;
    PUINT_8 pucCurrBuf = pucBuffer;


    ASSERT(prAdapter);
    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    //if (pucBuffer) {} /* For Windows, we'll print directly instead of sprintf() */
    ASSERT(pu4Count);

    SPRINTF(pucCurrBuf, ("\n\nRX CTRL STATUS:"));
    SPRINTF(pucCurrBuf, ("\n==============="));
    SPRINTF(pucCurrBuf, ("\nFREE RFB w/i BUF LIST :%9d", prRxCtrl->rFreeSwRfbList.u4NumElem));
    SPRINTF(pucCurrBuf, ("\nFREE RFB w/o BUF LIST :%9d", prRxCtrl->rIndicatedRfbList.u4NumElem));
    SPRINTF(pucCurrBuf, ("\nRECEIVED RFB LIST     :%9d", prRxCtrl->rReceivedRfbList.u4NumElem));

    SPRINTF(pucCurrBuf, ("\n\n"));

    //*pu4Count = (UINT_32)((UINT_32)pucCurrBuf - (UINT_32)pucBuffer);

    return;
} /* end of nicRxQueryStatus() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Clear RX related counters
*
* @param prAdapter Pointer of Adapter Data Structure
*
* @return - (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxClearStatistics(
    IN P_ADAPTER_T prAdapter
)
{
    P_RX_CTRL_T prRxCtrl;

    ASSERT(prAdapter);
    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    RX_RESET_ALL_CNTS(prRxCtrl);
    return;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used to query current statistics of RX Module.
*
* @param prAdapter      Pointer to the Adapter structure.
* @param pucBuffer      Pointer to the message buffer.
* @param pu4Count      Pointer to the buffer of message length count.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxQueryStatistics(
    IN P_ADAPTER_T prAdapter,
    IN PUINT_8 pucBuffer,
    OUT PUINT_32 pu4Count
)
{
    P_RX_CTRL_T prRxCtrl;
    PUINT_8 pucCurrBuf = pucBuffer;

    ASSERT(prAdapter);
    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    //if (pucBuffer) {} /* For Windows, we'll print directly instead of sprintf() */
    ASSERT(pu4Count);

#define SPRINTF_RX_COUNTER(eCounter) \
    SPRINTF(pucCurrBuf, ("%-30s : %lld\n", #eCounter, prRxCtrl->au8Statistics[eCounter]))

    SPRINTF_RX_COUNTER(RX_MPDU_TOTAL_COUNT);
    SPRINTF_RX_COUNTER(RX_SIZE_ERR_DROP_COUNT);
    SPRINTF_RX_COUNTER(RX_DATA_INDICATION_COUNT);
    SPRINTF_RX_COUNTER(RX_DATA_RETURNED_COUNT);
    SPRINTF_RX_COUNTER(RX_DATA_RETAINED_COUNT);

#if CFG_TCP_IP_CHKSUM_OFFLOAD || CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60
    SPRINTF_RX_COUNTER(RX_CSUM_TCP_FAILED_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_UDP_FAILED_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_IP_FAILED_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_TCP_SUCCESS_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_UDP_SUCCESS_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_IP_SUCCESS_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_UNKNOWN_L4_PKT_COUNT);
    SPRINTF_RX_COUNTER(RX_CSUM_UNKNOWN_L3_PKT_COUNT);
    SPRINTF_RX_COUNTER(RX_IP_V6_PKT_CCOUNT);
#endif

    //*pu4Count = (UINT_32)(pucCurrBuf - pucBuffer);

    nicRxClearStatistics(prAdapter);

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Read the Response data from data port
*
* @param prAdapter pointer to the Adapter handler
* @param pucRspBuffer pointer to the Response buffer
*
* @retval WLAN_STATUS_SUCCESS: Response packet has been read
* @retval WLAN_STATUS_FAILURE: Read Response packet timeout or error occurred
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRxWaitResponse(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucPortIdx,
    OUT PUINT_8 pucRspBuffer,
    IN UINT_32 u4MaxRespBufferLen,
    OUT PUINT_32 pu4Length
)
{
    UINT_32 u4Value = 0, u4PktLen = 0, i;
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
    BOOL fgResult = TRUE;

    //DEBUGFUNC("nicRxWaitResponse");

    ASSERT(prAdapter);
    ASSERT(pucRspBuffer);
    ASSERT(ucPortIdx < 2);

    for (i = RX_RESPONSE_POLLING_TIMEOUT; i != 0; i--) {
        /* Read the packet length */
        HAL_MCR_RD(prAdapter, MCR_WRPLR, &u4Value);

        if (!fgResult) {
            // DBGLOG(RX, ERROR, ("Read Response Packet Error\n"));
            kal_trace(TRACE_ERROR , ERROR_RX_Response_Packet_Error);
            return WLAN_STATUS_FAILURE;
        }

        if (ucPortIdx == 0) {
            u4PktLen = u4Value & 0xFFFF;
        } else {
            u4PktLen = (u4Value >> 16) & 0xFFFF;
        }

        //DBGLOG(RX, TRACE, ("i = %d, u4PktLen = %d\n", i, u4PktLen));
        kal_trace(TRACE_TXRX , TRACE_RX_i, i, u4PktLen);

        if (u4PktLen == 0) {
            /* Response packet is not ready */
            kalUdelay(50);
        } else if (u4PktLen > u4MaxRespBufferLen) {
            // DBGLOG(RX, WARN, ("Not enough Event Buffer: required length = 0x%x, available buffer length = %d\n",
            //     u4PktLen, u4MaxRespBufferLen));

            kal_trace(TRACE_WARNING , WARN_RX_NO_BUFFER, u4PktLen, u4MaxRespBufferLen);
            return WLAN_STATUS_FAILURE;
        } else {
            if (u4PktLen > 4) {
                /* NOTE(Nelson): for MT5931 E3 eHPI short RX issue workaround start */
                if ((prAdapter->ucRevID >= MT5931_E3_REVERSION_ID) && (ALIGN_4(u4PktLen) < E3_WORKAROND_RX_MIN_SIZE)) {
                    HAL_MCR_WR(prAdapter, WEHTCR, E3_WORKAROND_RX_MIN_SIZE);    /* Set EHPI transaction count */
                    u4PktLen = E3_WORKAROND_RX_MIN_SIZE;
                } else
                    /* NOTE(Nelson): for MT5931 E3 eHPI short RX issue workaround end */
                {
                    HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(u4PktLen));    /* Set EHPI transaction count */
                }
            }
            HAL_PORT_RD(prAdapter,
                        ucPortIdx == 0 ? MCR_WRDR0 : MCR_WRDR1,
                        u4PktLen,
                        pucRspBuffer,
                        u4MaxRespBufferLen);
            HAL_MCR_WR(prAdapter, WEHTCR, 4);    /* Reset EHPI transaction count */

            /* fgResult will be updated in MACRO */
            if (!fgResult) {
                //DBGLOG(RX, ERROR, ("Read Response Packet Error\n"));
                kal_trace(TRACE_ERROR , ERROR_RX_Response_Packet_Error);
                return WLAN_STATUS_FAILURE;
            }

            // DBGLOG(RX, TRACE, ("Dump Response buffer, length = 0x%x\n",
            //    u4PktLen));
            kal_trace(TRACE_GROUP_10 , TRACE_Dump_Response_buffer , u4PktLen);
            DBGLOG_MEM8(RX, TRACE, pucRspBuffer, u4PktLen);

            *pu4Length = u4PktLen;
            break;
        }
    }

    if (i == 0) {
        u4Status = WLAN_STATUS_FAILURE;
        WNDRV_TRACE(TRACE_WARNING, "nicRxWaitResponse Timeout\n");
    }

    return u4Status;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Set filter to enable Promiscuous Mode
*
* @param prAdapter          Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxEnablePromiscuousMode(
    IN P_ADAPTER_T prAdapter
)
{
    ASSERT(prAdapter);

    return;
} /* end of nicRxEnablePromiscuousMode() */


/*----------------------------------------------------------------------------*/
/*!
* @brief Set filter to disable Promiscuous Mode
*
* @param prAdapter  Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
nicRxDisablePromiscuousMode(
    IN P_ADAPTER_T prAdapter
)
{
    ASSERT(prAdapter);

    return;
} /* end of nicRxDisablePromiscuousMode() */


/*----------------------------------------------------------------------------*/
/*!
* @brief this function flushes all packets queued in reordering module
*
* @param prAdapter              Pointer to the Adapter structure.
*
* @retval WLAN_STATUS_SUCCESS   Flushed successfully
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRxFlush(
    IN P_ADAPTER_T  prAdapter
)
{
    P_SW_RFB_T prSwRfb;

    ASSERT(prAdapter);

    if ((prSwRfb = qmFlushRxQueues(prAdapter)) != NULL) {
        do {
            P_SW_RFB_T prNextSwRfb;

            // save next first
            prNextSwRfb = (P_SW_RFB_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prSwRfb);

            // free
            nicRxReturnRFB(prAdapter, prSwRfb);

            prSwRfb = prNextSwRfb;
        } while (prSwRfb);
    }

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param
*
* @retval
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
nicRxProcessActionFrame(
    IN P_ADAPTER_T      prAdapter,
    IN P_SW_RFB_T       prSwRfb
)
{
    P_WLAN_ACTION_FRAME prActFrame;

    ASSERT(prAdapter);
    ASSERT(prSwRfb);

    if (prSwRfb->u2PacketLen < sizeof(WLAN_ACTION_FRAME) - 1) {
        return WLAN_STATUS_INVALID_PACKET;
    }
    prActFrame = (P_WLAN_ACTION_FRAME) prSwRfb->pvHeader;

    switch (prActFrame->ucCategory) {
        case CATEGORY_PUBLIC_ACTION:

#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prRlmProcessPublicAction);
                if (prAdapter->rP2pFuncLkr.prRlmProcessPublicAction) {
                    prAdapter->rP2pFuncLkr.prRlmProcessPublicAction(
                        prAdapter, prSwRfb);
                }

                ASSERT(prAdapter->rP2pFuncLkr.prP2pRxPublicActionFrame);
                if (prAdapter->rP2pFuncLkr.prP2pRxPublicActionFrame) {
                    prAdapter->rP2pFuncLkr.prP2pRxPublicActionFrame(
                        prAdapter, prSwRfb);
                }
            }
#endif
            break;

        case CATEGORY_HT_ACTION:
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prRlmProcessHtAction);
                if (prAdapter->rP2pFuncLkr.prRlmProcessHtAction) {
                    prAdapter->rP2pFuncLkr.prRlmProcessHtAction(
                        prAdapter, prSwRfb);
                }
            }
#endif
            break;
        case CATEGORY_VENDOR_SPECIFIC_ACTION:
#if CFG_ENABLE_WIFI_DIRECT
            if (prAdapter->fgIsP2PRegistered) {
                ASSERT(prAdapter->rP2pFuncLkr.prP2pRxActionFrame);
                if (prAdapter->rP2pFuncLkr.prP2pRxActionFrame) {
                    prAdapter->rP2pFuncLkr.prP2pRxActionFrame(prAdapter, prSwRfb);
                }
            }
#endif
            break;
        default:
            break;
    } /* end of switch case */

    return WLAN_STATUS_SUCCESS;
}

#ifdef __WIFI_SNIFFER_SUPPORT__
void nicRxProcessSnifferedPacket(
    IN P_ADAPTER_T      prAdapter,
    IN P_SW_RFB_T       prSwRfb
)
{
    nicRxFillRFB(prAdapter, prSwRfb);

#if DBG
//  kal_wap_trace(MOD_WNDRV,TRACE_ERROR,"dump prHifRxHdr for data/mgt,len=%d",sizeof(HIF_RX_HEADER_T));
//  dumpMemory8((PUINT_8)prSwRfb->prHifRxHdr,sizeof(HIF_RX_HEADER_T));
//  kal_wap_trace(MOD_WNDRV,TRACE_ERROR,"dump MAC header for data/mgt len=%d",prSwRfb->u2HeaderLen);
    dumpMemory8((PUINT_8)prSwRfb->pvHeader, prSwRfb->u2HeaderLen);
    //kal_wap_trace(MOD_WNDRV,TRACE_ERROR,"dump body for data/mgt,len=%d",prSwRfb->u2PacketLen-prSwRfb->u2HeaderLen);
    //dumpMemory8((PUINT_8)prSwRfb->pvHeader+prSwRfb->u2HeaderLen,prSwRfb->u2PacketLen-prSwRfb->u2HeaderLen);
#endif

    if (prAdapter->u1SnifferOnOff == 1) { // indicate packet  to upper derectly

    } else if ((prAdapter->u1SnifferOnOff == 2) && (prAdapter->smart_config.u1ListenState)) { //driver alanize packet to find out ap info
        parser_fsm_go((char *)prSwRfb->pvHeader, prSwRfb->u2PacketLen);
    }

    nicRxReturnRFB(prAdapter, prSwRfb);
}
#endif



