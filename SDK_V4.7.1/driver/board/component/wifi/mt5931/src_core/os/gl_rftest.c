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
** $Id: //Project/WLAN/MT5921_BRANCHES/MT5921_11AW1112MP/os/gl_rftest.c#1 $
*/
/*******************************************************************************
* Copyright (c) 2003-2004 Inprocomm, Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* Inprocomm, Inc.
********************************************************************************
*/
/*******************************************************************************
GENERAL DESCRIPTION
********************
This file contains the RF test mode routines of Windows driver for
INPROCOMM 802.11 Wireless LAN adapters.
*/
#ifdef __MTK_TARGET__
/*******************************************************************************
*                E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"
#include "wndrv_context.h"
/*WCP modification*/#include "wndrv_ft_types.h"
#include "wndrv_param.h"

extern wndrv_context_struct wndrv_context;

#if 0
/* WCP Modification */
extern EEPROM_CHANNEL_CFG_ENTRY arNicEepromChannelTable[];
#endif
/*******************************************************************************
*                         D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                        P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                       P R I V A T E   D A T A
********************************************************************************
*/
/*******************************************************************************
*                             M A C R O S
********************************************************************************
*/

/*******************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID
rftestDisablePacketTxTest(IN P_ADAPTER_T adapter_p);

WLAN_STATUS
rftestEnablePacketTxTest(IN P_ADAPTER_T      adapter_p,
                         IN PTX_PACKET_STRUC txPacketBuffer_p,
                         IN UINT_32          structLen);

int rfcalAllProcedure(int nCount);
int rfcalRfDefineForRx(void);
int rfcalRfRxPhase(int nCount);
int rfcalRfRxGain(int nCount);
int rfcalRfTxPhase(int nCount);
int rfcalRfTxGain(int nCount);
int rfcalTxIDc(int nCount);
int rfcalTxQDc(int nCount);
int rfcalRfDefineForTx(void);
void rfCalRegBackup(void);
void rfCalRegRestore(void);

/*******************************************************************************
*                          F U N C T I O N S
********************************************************************************
*/
/*******************************************************************************
**  rftestEnterTestMode
**
**  descriptions: Set the driver to test mode
**  parameters:
**      adapter_p - Pointer to the adapter object data area.
**  return:       (none)
**  note:
*******************************************************************************/
VOID
rftestEnterTestMode(IN P_ADAPTER_T adapter_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;


    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetTestMode,
                       (PVOID)&data,
                       0,
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    kal_event_stop_timer(PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER);

#if defined (MSTAR_CARDV)
    kal_event_stop_timer(PARAM_WNDRV_ES_STAT_IND_TIMER);
#endif
}   /* rftestEnterTestMode */

/*******************************************************************************
**  rftestAbortTestMode
**
**  descriptions: Abort the test mode and set the driver to the idle state.
**  parameters:
**      adapter_p - Pointer to the adapter object data area.
**  return:       (none)
**  note:
*******************************************************************************/
VOID
rftestAbortTestMode(IN P_ADAPTER_T adapter_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;


    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAbortTestMode,
                       (PVOID)&data,
                       0,
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    kal_event_stop_timer(PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER);
}   /* rftestAbortTestMode */

/*******************************************************************************
**  rftestEnablePacketRxTest
**
**  descriptions: This routine is called to enable the continuous packet
**                receive test in the RF test mode. The RDMA will be
**                started to receive packets from the wireless medium.
**  parameters:
**      adapter_p - Pointer to the adapter object data area.
**  return:       (none)
**  note:
*******************************************************************************/
VOID
rftestEnablePacketRxTest(IN P_ADAPTER_T adapter_p)
{
#if 0
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlue = wndrv_context.glue_p;
    RFTEST_RX_STAT *prRxStat = &(wndrv_context.glue_p->rRfRxStat);
    UINT_32 u4RegisterBackup = 0;
    UINT_32 u4Value = 0;
    UINT_32 u4SetBuffer = 0;
    P_RX_CTRL_T                 prRxCtrl;
    prRxCtrl = &prAdapter->rRxCtrl;


    // rfcalAllProcedure(1);

    // record the initial value for RFB G0/ 1/ 2 setting
    HAL_MCR_RD(prAdapter, 0xf4, &u4RegisterBackup);

    // OR BIT(0) | BIT(1) | BIT(2) | BIT(5)
    HAL_MCR_WR(prAdapter, 0xf4,
               u4RegisterBackup |
               BIT(0) | BIT(1) | BIT(2) | BIT(5));

    // reset BB RX counters
    HAL_MCR_RD(prAdapter, 0x348/*CR 82*/, &u4Value);
    u4Value |= BIT(7);
    HAL_MCR_WR(prAdapter, 0x348/*CR 82*/, u4Value);
    u4Value &= ~(BIT(7));
    HAL_MCR_WR(prAdapter, 0x348/*CR 82*/, u4Value);

    // enable BB RX counters
    u4Value |= BIT(6);
    HAL_MCR_WR(prAdapter, 0x348/*CR 82*/, u4Value);

    // Clear initial RX counters
    //ED pass BBCR_RD(m_prAdapter, 91, &m_u4InitEdPass);
    HAL_MCR_RD(prAdapter, 0x36c/*CR 91*/, &prRxStat->u4InitEd);

    //OFDM OSD pass BBCR_RD(m_prAdapter, 92, &m_u4InitOfdmOsdPass);
    HAL_MCR_RD(prAdapter, 0x370/*CR 92*/, &prRxStat->u4InitOsd);

    //CCK_SQ BBCR_RD(m_prAdapter, 94, &m_u4InitCckSq);
    HAL_MCR_RD(prAdapter, 0x378/*CR 94*/, &prRxStat->u4InitCckSq);

    //CCK_SFD Pass BBCR_RD(m_prAdapter, 96, &m_u4InitCckSfdPass);
    HAL_MCR_RD(prAdapter, 0x380/*CR 96*/, &prRxStat->u4InitCckSfd);

    //BB_OFDM_SIG BBCR_RD(m_prAdapter, 93, &m_u4InitOfdmSig);
    HAL_MCR_RD(prAdapter, 0x374/*CR 93*/, &prRxStat->u4InitOfdmSig);

    //BB_CCK_SIG BBCR_RD(m_prAdapter, 95, &m_u4InitCckSig);
    HAL_MCR_RD(prAdapter, 0x37c/*CR 95*/, &prRxStat->u4InitCckSig);


    /* WCP Modification: init all statistics to 0 */

    prRxCtrl->au8Statistics[RX_MPDU_TOTAL_COUNT] = 0;
    prRxCtrl->au8Statistics[RX_FCS_ERR_DROP_COUNT] = 0 ;
    prRxCtrl->au8Statistics[RX_MPDU_CCK_SHORT_PREAMBLE_COUNT] = 0;
    prRxCtrl->au8Statistics[RX_MPDU_CCK_LONG_PREAMBLE_COUNT] = 0;
    prRxCtrl->au8Statistics[RX_BB_FCS_ERROR_COUNT] = 0;
    prRxCtrl->au8Statistics[RX_BB_FIFO_FULL_COUNT] = 0;
    prRxCtrl->au8Statistics[RX_BB_MPDU_COUNT] = 0;
    prRxCtrl->au8Statistics[RX_BB_CCA_TIME_COUNT] = 0;

    /* WCP Modification: init all statistics to 0 */


    //MAC_FCS_ERR, MAC_RX_OK
    //m_prAdapter->getPacketRxStatus(&m_i4InitRxOKCnt, &m_i4InitRxFcsErrCnt);
    prRxStat->u4InitRxOKCnt = (UINT_32)prRxCtrl->au8Statistics[RX_MPDU_TOTAL_COUNT];
    prRxStat->u4InitRxFcsErrCnt = (UINT_32)prRxCtrl->au8Statistics[RX_FCS_ERR_DROP_COUNT];

    // ALC
    //m_prAdapter->SetATParam(9, m_fgAlcEn);

    // Enable RX m_prAdapter->SetATParam(1, 2);

    prRxStat->u4TxGain = 0;
    prRxStat->u4CntCckSq = 0;
    prRxStat->u4CntCckSig = 0;
    prRxStat->u4CntCckSfd = 0;
    prRxStat->u4CntOsd = 0;
    prRxStat->u4CntOfdmSig = 0;
    prRxStat->u4CntEd = 0;
    prRxStat->u4RxCntFcsErr = 0;
    prRxStat->u4RxPer = 0;
    prRxStat->u4RxCntOK = 0;

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_STARTRX;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    HAL_MCR_RD(prAdapter, 0x150/*RFCR*/, &u4SetBuffer);

    /* Turn on promiscuous mode */

    u4SetBuffer = PARAM_PACKET_FILTER_PROMISCUOUS + PARAM_PACKET_FILTER_BROADCAST;

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetCurrentPacketFilter,
                       (PVOID)&u4SetBuffer,
                       sizeof(u4SetBuffer),
                       (PUINT_32)&pdu_len);


    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_STARTRX;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

#if 0
    UINT_32 temp32;

    DEBUGFUNC("rftestEnablePacketRxTest");

    DBGLOG(TEST, TRACE, ("\n"));

    /* Start the receive unit -- we can now receive packets off the wire */
    DBGLOG(TEST, TRACE, ("Start Receive Unit\n"));

    /* Clear the status of the receive test. */
    kalMemZero((PVOID) &adapter_p->rxTestStatus,
               sizeof(TEST_RX_STATUS_STRUC));

    /* Read the receiving counters to clear them. */
    NIC_MCR_RD(adapter_p, MCR_RPCR, &temp32);
    NIC_MCR_RD(adapter_p, MCR_RFECR, &temp32);
    NIC_MCR_RD(adapter_p, MCR_RFFCR, &temp32);
    NIC_MCR_RD(adapter_p, MCR_CACR, &temp32);

    /* Clear the current own MAC address to avoid sending ACK frames during the
       test. */
    pauSetOwnMacAddress(adapter_p, NULL);

    /* Set up the PAU to enable the promiscuous mode and discard received
       frames with FCS errors. */
    /* WCP Modification */    //pauEnablePromiscuousMode(adapter_p, TRUE);
    /* WCP Modification */    pauEnablePromiscuousMode(adapter_p, FALSE);

    rxStartRecvUnit(adapter_p, FALSE);

    pauEnable(adapter_p);
#endif
#endif
}   /* rftestEnablePacketRxTest */

/*******************************************************************************
**  rftestDisablePacketRxTest
**
**  descriptions: This routine is called to disable the continuous packet
**                receive test in the RF test mode. The RDMA will be
**                stopped.
**  parameters:
**      adapter_p - Pointer to the adapter object data area.
**  return:       (none)
**  note:
*******************************************************************************/
VOID
rftestDisablePacketRxTest(IN P_ADAPTER_T adapter_p)
{
#if 0
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    UINT_32 u4Value = 0;

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_STOPTEST;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    HAL_MCR_RD(prAdapter, 0x348/*CR 82*/, &u4Value);
    u4Value &= ~(BIT(6));
    HAL_MCR_WR(prAdapter, 0x348/*CR 82*/, u4Value);

#if 0 /* Need to check if the MAC address is restored in 5921 */
    /* Restore the current own MAC address. */
    pauSetOwnMacAddress(adapter_p, adapter_p->mib.dot11MACAddress);
#endif
#endif
}   /* rftestDisablePacketRxTest */


/*******************************************************************************
**  rftestEnablePacketTxTest
**
**  descriptions: This routine is called to disable the continuous packet
**                transmit test in the RF test mode.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      txPacketBuffer_p - Pointer to the tx packet structure that stores the
**                   transmitted OID Tx Packet frame.
**  return:
**      WLAN_STATUS_SUCCESS - Tx test successfully.
**      WLAN_STATUS_RESOURCES - Out of resources for Tx test.
**  note:
*******************************************************************************/
WLAN_STATUS
rftestEnablePacketTxTest(IN P_ADAPTER_T      adapter_p,
                         IN PTX_PACKET_STRUC txPacketBuffer_p,
                         IN UINT_32          structLen)
{
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlueInfo = wndrv_context.adpt_p->prGlueInfo;
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    WLAN_STATUS status = WLAN_STATUS_FAILURE;
    /* WCP Modification */
    //UINT_32                        u4flags = 0;
    //UINT_16                        u2Ch, u2Rate;
    //UINT_16                        u2PktLen;
    //UINT_32                        u4InterPktDly;
    //UINT_32                        u4Val;
    //UINT_32                        u4SlotTime;

    /* Use the same method in 5911 to stop TX */
    if ((txPacketBuffer_p->pktLength     == 0xF0F0F0F0) &&
            (txPacketBuffer_p->pktCount      == 0xF0F0F0F0) &&
            (txPacketBuffer_p->pktInterval   == 0xF0F0F0F0)) {
        rftestDisablePacketTxTest(adapter_p);
        return WLAN_STATUS_SUCCESS;
    }
#if 0
    /* WCP Modification */
    /* set TX Flags */
    if (structLen  == sizeof(TX_PACKET_V0_STRUC)) {
        u4flags = 0;
    } else {
        u4flags = txPacketBuffer_p->txFlags;
    }

    /* NOTE(Nelson): Default temperature compensation in FW is on, and always do not turn off it in META/EM mode */
    /* set ALC Tracking Flag to decide Thermo-Sensor Usage */
    if ((u4flags & TX_PACKET_FLAG_ENABLE_ALC_TRACK)) {
        data.u4FuncIndex = RF_AT_FUNCID_ALC;
        data.u4FuncData = 1;
        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                             (PFN_OID_HANDLER_FUNC) wlanoidRftestSetAutoTest,
                                             (PVOID)&data,
                                             sizeof(data),
                                             (PUINT_32)&pdu_len))) {

            return status;

        }

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    } else {
        data.u4FuncIndex = RF_AT_FUNCID_ALC;
        data.u4FuncData = 0;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                             (PFN_OID_HANDLER_FUNC) wlanoidRftestSetAutoTest,
                                             (PVOID)&data,
                                             sizeof(data),
                                             (PUINT_32)&pdu_len))) {

            return status;

        }

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    }
#endif

    /* set 11b/g packet preamble */
    if (txPacketBuffer_p->txRate < WNDRV_TEST_MOD_MCS0) {
        data.u4FuncIndex = RF_AT_FUNCID_PREAMBLE;
        data.u4FuncData = txPacketBuffer_p->is_short_preamble;

        GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
        if (WLAN_STATUS_SUCCESS != (wlanSetInformation(prAdapter,
                                    (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                    (PVOID)&data,
                                    sizeof(data),
                                    (PUINT_32)&pdu_len))) {

            return status;

        }
        GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    }

    /* Set TX packet count*/
    if (txPacketBuffer_p->pktCount) {
        data.u4FuncIndex = RF_AT_FUNCID_PKTCNT;
        data.u4FuncData = txPacketBuffer_p->pktCount;

        GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
        if (WLAN_STATUS_SUCCESS != (wlanSetInformation(prAdapter,
                                    (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                    (PVOID)&data,
                                    sizeof(data),
                                    (PUINT_32)&pdu_len))) {

            return status;

        }
        GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    } else {

        data.u4FuncIndex = RF_AT_FUNCID_PKTCNT;
        data.u4FuncData = RF_AT_PARAM_PACKET_UNLIMITED;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
        if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                             (PFN_OID_HANDLER_FUNC) wlanoidRftestSetAutoTest,
                                             (PVOID)&data,
                                             sizeof(data),
                                             (PUINT_32)&pdu_len))) {

            return status;

        }
        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    }

    /* Set Packet Content */
    // only the first two bytes are replaced here
    /* WCP Modification */
#if 0    //Nelson: reference to SP META and no use this CMD
    for (u2PktLen = 0; u2PktLen < txPacketBuffer_p->pktLength; u2PktLen++) {
        data.u4FuncIndex = RF_AT_FUNCID_PKTCONTENT;
        data.u4FuncData = (txPacketBuffer_p->pktContent[u2PktLen] << 24) |
                          (txPacketBuffer_p->pktContent[u2PktLen + 1] << 16) |
                          u2PktLen;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
        if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                             (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                             (PVOID)&data,
                                             sizeof(data),
                                             (PUINT_32)&pdu_len))) {

            return status;
        }
        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    }
#endif

    /* Set TX packet length */
    data.u4FuncIndex = RF_AT_FUNCID_PKTLEN;
    data.u4FuncData = txPacketBuffer_p->pktLength;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                         (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                         (PVOID)&data,
                                         sizeof(data),
                                         (PUINT_32)&pdu_len))) {

        return status;
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);


    /* Set TX packet interval */
    data.u4FuncIndex = RF_AT_FUNCID_PKTINTERVAL;
    data.u4FuncData = txPacketBuffer_p->pktInterval;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                         (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                         (PVOID)&data,
                                         sizeof(data),
                                         (PUINT_32)&pdu_len))) {

        return status;
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);


    /* Set Retry Limit */
    data.u4FuncIndex = RF_AT_FUNCID_RETRYLIMIT;
    data.u4FuncData =  1;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                         (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                         (PVOID)&data,
                                         sizeof(data),
                                         (PUINT_32)&pdu_len))) {

        return status;
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);


    /* Enable TX queue */
    data.u4FuncIndex = RF_AT_FUNCID_QUEUE;
    data.u4FuncData =  0;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                         (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                         (PVOID)&data,
                                         sizeof(data),
                                         (PUINT_32)&pdu_len))) {

        return status;
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);


    //HAL_MCR_RD(prAdapter, MCR_CIR, &u4MCR);   // lcko: check for LP status


    /* Set TX power */
    data.u4FuncIndex = RF_AT_FUNCID_POWER;
    //data.u4FuncData = txPacketBuffer_p->txGain * 2; /* data.u4FuncData is unit of 0.5 dbm */
    data.u4FuncData = txPacketBuffer_p->txGain;  /* data.u4FuncData is unit of 0.5 dbm, and MMI will multiply it by 2 */

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                         (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                         (PVOID)&data,
                                         sizeof(data),
                                         (PUINT_32)&pdu_len))) {

        return status;
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);


    /* Start TX packet */
    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_STARTTX;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    if (WLAN_STATUS_SUCCESS != (status = wlanSetInformation(prAdapter,
                                         (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                                         (PVOID)&data,
                                         sizeof(data),
                                         (PUINT_32)&pdu_len))) {

        return status;
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    return WLAN_STATUS_SUCCESS;

}   /* rftestEnablePacketTxTest */


/*******************************************************************************
**  rftestDisablePacketTxTest
**
**  descriptions: This routine is called to disable the continuous packet
**                transmit test in the RF test mode. The RDMA will be
**                stopped.
**  parameters:
**      adapter_p - Pointer to the adapter object data area.
**  return:       (none)
**  note:
*******************************************************************************/
VOID
rftestDisablePacketTxTest(IN P_ADAPTER_T adapter_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_STOPTEST;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}   /* rftestDisablePacketTxTest */

/*******************************************************************************
**  rftestSetTestMode
**
**  descriptions: This routine is called to request the driver to enter the
**                RF test mode.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      setBuffer_p -  A pointer to the buffer that holds the data to be
**                     set
**      setBufferLen - The length of the set buffer
**  result:
**      setInfoLen_p - If the call is successful, returns the number of
**                     bytes read from the set buffer. If the call failed
**                     due to invalid length of the set buffer, returns the
**                     amount of storage needed.
**  return:
**      WLAN_STATUS_SUCCESS
**  note:
*******************************************************************************/
WLAN_STATUS
rftestSetTestMode(IN  PVOID    pvAdapter,
                  IN  PVOID    setBuffer_p,
                  IN  UINT_32  setBufferLen,
                  OUT PUINT_32 setInfoLen_p)
{
    P_ADAPTER_T adapter_p = (P_ADAPTER_T) pvAdapter;

    DEBUGFUNC("rftestSetTestMode");

    *setInfoLen_p = 0;

    rftestEnterTestMode(adapter_p);

    return WLAN_STATUS_SUCCESS;

}   /* rftestSetTestMode */

/*******************************************************************************
**  rftestSetAbortTestMode
**
**  descriptions: This routine is called to request the driver to abort the
**                RF test mode.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      setBuffer_p -  A pointer to the buffer that holds the data to be
**                     set
**      setBufferLen - The length of the set buffer
**  result:
**      setInfoLen_p - If the call is successful, returns the number of
**                     bytes read from the set buffer. If the call failed
**                     due to invalid length of the set buffer, returns the
**                     amount of storage needed.
**  return:
**      WLAN_STATUS_SUCCESS
**  note:
*******************************************************************************/
WLAN_STATUS
rftestSetAbortTestMode(IN  PVOID    pvAdapter,
                       IN  PVOID    setBuffer_p,
                       IN  UINT_32  setBufferLen,
                       OUT PUINT_32 setInfoLen_p)
{

    rftestAbortTestMode(pvAdapter);

    *setInfoLen_p = 0;

    return WLAN_STATUS_SUCCESS;

}   /* rftestSetAbortTestMode */

/*******************************************************************************
**  rftestSetTestPacketRx
**
**  descriptions: This routine is called to request the driver to enable or
**                disable the continuous packet receive test.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      setBuffer_p -  A pointer to the buffer that holds the data to be
**                     set
**      setBufferLen - The length of the set buffer
**  result:
**      setInfoLen_p - If the call is successful, returns the number of
**                     bytes read from the set buffer. If the call failed
**                     due to invalid length of the set buffer, returns the
**                     amount of storage needed.
**  return:
**      WLAN_STATUS_SUCCESS
**  note:
*******************************************************************************/
WLAN_STATUS
rftestSetTestPacketRx(IN  PVOID    pvAdapter,
                      IN  PVOID    setBuffer_p,
                      IN  UINT_32  setBufferLen,
                      OUT PUINT_32 setInfoLen_p)
{
    P_ADAPTER_T adapter_p = (P_ADAPTER_T) pvAdapter;

    DEBUGFUNC("rftestSetTestPacketRx");

    *setInfoLen_p = 4;

    /* Enable or disable the continuous packet receive test. */
    if (*((PUINT_32) setBuffer_p)) {
        rftestEnablePacketRxTest(adapter_p);
    } else {
        rftestDisablePacketRxTest(adapter_p);
    }

    return WLAN_STATUS_SUCCESS;
}   /* rftestSetTestPacketRx */

/*******************************************************************************
**  rftestSetTestPacketTx
**
**  descriptions: This routine is called to request the driver to enable or
**                disable the continuous packet transmit test.
**  parameters:
**      adapter_p - Pointer to the Adapter structure
**      setBuffer_p -  A pointer to the buffer that holds the data to be
**                     set
**      setBufferLen - The length of the set buffer
**  result:
**      setInfoLen_p - If the call is successful, returns the number of
**                     bytes read from the set buffer. If the call failed
**                     due to invalid length of the set buffer, returns the
**                     amount of storage needed.
**  return:
**      WLAN_STATUS_SUCCESS
**      WLAN_STATUS_RESOURCES
**  note:
*******************************************************************************/
WLAN_STATUS
rftestSetTestPacketTx(IN  PVOID    pvAdapter,
                      IN  PVOID    setBuffer_p,
                      IN  UINT_32  setBufferLen,
                      OUT PUINT_32 setInfoLen_p)
{

    P_ADAPTER_T adapter_p = (P_ADAPTER_T) pvAdapter;
    WLAN_STATUS status;

    DEBUGFUNC("rftestSetTestPacketTx");

    /* Compatible w. older testdll.dll */
    if (setBufferLen == sizeof(TX_PACKET_V0_STRUC)) {
        *setInfoLen_p = sizeof(TX_PACKET_V0_STRUC);
    } else {
        *setInfoLen_p = sizeof(TX_PACKET_STRUC);
    }

    if (setBufferLen < sizeof(TX_PACKET_V0_STRUC)) {
        return WLAN_STATUS_BUFFER_TOO_SHORT;
    }

    /* Enable the continuous packet receive test. */
    status = rftestEnablePacketTxTest(adapter_p,
                                      (PTX_PACKET_STRUC) setBuffer_p,
                                      setBufferLen);

    return status;
}   /* rftestSetTestPacketTx */


void getDACOffset(kal_uint8 *i_ch, kal_uint8 *q_ch)
{
#if 0
    UINT_32 u4Value1 = 0, u4Value2 = 0, nCount = 1;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    int i;

    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();
        rfcalRfDefineForTx();
        HAL_MCR_WR(prAdapter, 0x0334, 0xc180);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x0338, &u4Value1);
        *i_ch = (UINT_8) u4Value1;
        HAL_MCR_RD(prAdapter, 0x033C, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x033C, (u4Value2 & ~BITS(0, 6)) | (u4Value1 & BITS(0, 6)));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);
        rfCalRegRestore();
    }

    u4Value1 = 0;
    u4Value2 = 0;

    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();
        rfcalRfDefineForTx();

        HAL_MCR_WR(prAdapter, 0x0334, 0xa180);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x033c,  &u4Value1);
        *q_ch = (UINT_8) u4Value1;
        HAL_MCR_RD(prAdapter, 0x033c, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x033c,
                   (u4Value2 & ~BITS(8, 14)) |
                   ((u4Value1 & BITS(0, 6)) << 8));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);
        rfCalRegRestore();
    }
#endif
}


#if 1

int rfcalAllProcedure(int nCount)
{
#if 0
//  DBGPRINT(("rfcalAllProcedure"));

    int i;
    for (i = 0; i < nCount; i++) {
        rfcalTxIDc(1);
        rfcalTxQDc(1);
        rfcalRfTxGain(1);
        rfcalRfTxPhase(1);
        rfcalRfRxGain(1);
        rfcalRfRxPhase(1);
    }
#endif
    return 0;
}

int rfcalRfDefineForRx(void)
{
#if 0
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    // RFCR0
    HAL_MCR_WR(prAdapter, 0x0400, 0xDB6D1357);
    // RFCR6
    HAL_MCR_WR(prAdapter, 0x0418, 0x05249E3B);
    // RFCR12
    HAL_MCR_WR(prAdapter, 0x0430, 0xF6404087);

    //RXHP=1
    // BBCR97
    HAL_MCR_WR(prAdapter, 0x0384, 0x0008);
    // BBCR98
    HAL_MCR_WR(prAdapter, 0x98, 0x0080);

    //No I / Q SWAP
    // BBCR1
    HAL_MCR_WR(prAdapter, 0x0204, 0x0601);

    // MCR98
    HAL_MCR_WR(prAdapter, 0x98, 0x00180100);
    // MCR60
    HAL_MCR_WR(prAdapter, 0x60, 0xE0200100);
#endif
    return 0;
}

int rfcalRfRxPhase(int nCount)
{
#if 0
    UINT_32 u4Value1, u4Value2;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

//  DBGPRINT(("rfcalRfRxPhase"));

    int i;

    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();

        rfcalRfDefineForRx();

        HAL_MCR_WR(prAdapter, 0x0334, 0x83c0);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x0338, &u4Value1);
//      DBGPRINT(("CR78 BIT(7): complete state = %d",
//          (u4Value1 & BIT(7)) ? 1 : 0));
//      DBGPRINT(("RFRX phase: 0x%x", u4Value1 & BITS(0, 6)));
        HAL_MCR_RD(prAdapter, 0x0344, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x0344,
                   (u4Value2 & ~BITS(8, 14)) |
                   ((u4Value1 & BITS(0, 6)) << 8));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);

        rfCalRegRestore();
    }
#endif
    return 0;
}


int rfcalRfRxGain(int nCount)
{
#if 0
    UINT_32 u4Value1, u4Value2;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

//  DBGPRINT(("rfcalRfRxGain"));
    int i;
    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();

        rfcalRfDefineForRx();

        HAL_MCR_WR(prAdapter, 0x0334, 0x85c0);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x0338, &u4Value1);
//      DBGPRINT(("CR78 BIT(7): complete state = %d",
//          (u4Value1 & BIT(7)) ? 1 : 0));
//      DBGPRINT(("RFRX gain: 0x%x", u4Value1 & BITS(0, 6)));
        HAL_MCR_RD(prAdapter, 0x0344, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x0344,
                   (u4Value2 & ~BITS(0, 6)) |
                   (u4Value1 & BITS(0, 6)));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);

        rfCalRegRestore();
    }
#endif
    return 0;
}

int rfcalRfTxPhase(int nCount)
{
#if 0
    UINT_32 u4Value1, u4Value2;
//  UINT_32 u4Value3, u4Value4;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

//  DBGPRINT(("rfcalRfTxPhase"));
    int i;
    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();

        rfcalRfDefineForTx();

        HAL_MCR_WR(prAdapter, 0x0334, 0x89c0);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x0338, &u4Value1);
//      DBGPRINT(("CR78 BIT(7): complete state = %d",
//          (u4Value1 & BIT(7)) ? 1 : 0));
//      DBGPRINT(("RFTX phase: 0x%x", u4Value1 & BITS(0, 6)));
        HAL_MCR_RD(prAdapter, 0x0340,  &u4Value2);
        HAL_MCR_WR(prAdapter, 0x0340,
                   (u4Value2 & ~BITS(8, 14)) |
                   ((u4Value1 & BITS(0, 6)) << 8));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);

        rfCalRegRestore();
    }
#endif
    return 0;
}

int rfcalRfTxGain(int nCount)
{
#if 0
    UINT_32 u4Value1, u4Value2;
//  UINT_32 u4Value3, u4Value4;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

//  DBGPRINT(("rfcalRfTxGain"));
    int i;
    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();

        rfcalRfDefineForTx();

        HAL_MCR_WR(prAdapter, 0x0334, 0x91c0);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x0338, &u4Value1);
//      DBGPRINT(("RFTX gain: 0x%x", u4Value1 & BITS(0, 6)));
        HAL_MCR_RD(prAdapter, 0x0340, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x0340,
                   (u4Value2 & ~BITS(0, 6)) |
                   (u4Value1 & BITS(0, 6)));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);

        rfCalRegRestore();
    }
#endif
    return 0;
}


int rfcalTxIDc(int nCount)
{
#if 0
    UINT_32 u4Value1, u4Value2;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    int i;
    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();
        rfcalRfDefineForTx();
        HAL_MCR_WR(prAdapter, 0x0334, 0xc180);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x0338, &u4Value1);
        HAL_MCR_RD(prAdapter, 0x033C, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x033C, (u4Value2 & ~BITS(0, 6)) | (u4Value1 & BITS(0, 6)));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);
        rfCalRegRestore();
    }
#endif
    return 0;
}



int rfcalTxQDc(int nCount)
{
#if 0
    UINT_32 u4Value1, u4Value2;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    //DBGPRINT(("rfcalTxQDc"));
    int i;
    for (i = 0; i < nCount; i++) {
        rfCalRegBackup();
        rfcalRfDefineForTx();

        HAL_MCR_WR(prAdapter, 0x0334, 0xa180);
        kalUdelay(1000);
        HAL_MCR_RD(prAdapter, 0x033c,  &u4Value1);
//      DBGPRINT(("TXQDC: 0x%x", u4Value1 & BITS(0, 6)));
        HAL_MCR_RD(prAdapter, 0x033c, &u4Value2);
        HAL_MCR_WR(prAdapter, 0x033c,
                   (u4Value2 & ~BITS(8, 14)) |
                   ((u4Value1 & BITS(0, 6)) << 8));
        HAL_MCR_WR(prAdapter, 0x0334, 0x0000);

        rfCalRegRestore();
    }
#endif
    return 0;
}


int rfcalRfDefineForTx(void)
{
#if 0
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    // RFCR0
    HAL_MCR_WR(prAdapter, 0x0400, 0xDB6D1353);
    // RFCR6
    HAL_MCR_WR(prAdapter, 0x0418, 0x05249E3B);
    // RFCR12
    HAL_MCR_WR(prAdapter, 0x0430, 0xF7004087);

    //RXHP=1
    // BBCR97
    HAL_MCR_WR(prAdapter, 0x0384, 0x0008);
    // BBCR98
    HAL_MCR_WR(prAdapter, 0x98, 0x0080);

    //I / Q SWAP
    // BBCR1
    HAL_MCR_WR(prAdapter, 0x0204, 0x8601);

    // MCR98
    HAL_MCR_WR(prAdapter, 0x98, 0x00180100);
    // MCR60
    HAL_MCR_WR(prAdapter, 0x60, 0xE0200100);
#endif
    return 0;
}

void rfCalRegBackup(void)
{
#if 0
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlue = wndrv_context.glue_p;

#if 0
    RFCR_RD(m_prAdapter, 0, &u4BkRfcr0);
    RFCR_RD(m_prAdapter, 2, &u4BkRfcr2);
    RFCR_RD(m_prAdapter, 6, &u4BkRfcr6);
    RFCR_RD(m_prAdapter, 7, &u4BkRfcr7);
    RFCR_RD(m_prAdapter, 12, &u4BkRfcr12);

    BBCR_RD(m_prAdapter, 1, &u4BkBbcr1);
    BBCR_RD(m_prAdapter, 97, &u4BkBbcr97);

    MCR_RD(m_prAdapter, 0x98, &u4BkMcr98);
    MCR_RD(m_prAdapter, 0x60, &u4BkMcr60);
#endif

    HAL_MCR_RD(prAdapter, 0x0400, &prGlue->rRfRxStat.u4BkRfcr0);
    HAL_MCR_RD(prAdapter, 0x0408, &prGlue->rRfRxStat.u4BkRfcr2);
    HAL_MCR_RD(prAdapter, 0x0418, &prGlue->rRfRxStat.u4BkRfcr6);
    HAL_MCR_RD(prAdapter, 0x041c, &prGlue->rRfRxStat.u4BkRfcr7);
    HAL_MCR_RD(prAdapter, 0x0430, &prGlue->rRfRxStat.u4BkRfcr12);

    HAL_MCR_RD(prAdapter, 0x0204, &prGlue->rRfRxStat.u4BkBbcr1);
    HAL_MCR_RD(prAdapter, 0x0384, &prGlue->rRfRxStat.u4BkBbcr97);

    HAL_MCR_RD(prAdapter, 0x98, &prGlue->rRfRxStat.u4BkMcr98);
    HAL_MCR_RD(prAdapter, 0x60, &prGlue->rRfRxStat.u4BkMcr60);
#endif
}


void rfCalRegRestore(void)
{
#if 0
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlue = wndrv_context.glue_p;

    HAL_MCR_WR(prAdapter, 0x0400, prGlue->rRfRxStat.u4BkRfcr0);
    HAL_MCR_WR(prAdapter, 0x0408, prGlue->rRfRxStat.u4BkRfcr2);
    HAL_MCR_WR(prAdapter, 0x0418, prGlue->rRfRxStat.u4BkRfcr6);
    HAL_MCR_WR(prAdapter, 0x041c, prGlue->rRfRxStat.u4BkRfcr7);
    HAL_MCR_WR(prAdapter, 0x0430, prGlue->rRfRxStat.u4BkRfcr12);

    HAL_MCR_WR(prAdapter, 0x0204, prGlue->rRfRxStat.u4BkBbcr1);
    HAL_MCR_WR(prAdapter, 0x0384, prGlue->rRfRxStat.u4BkBbcr97);

    HAL_MCR_WR(prAdapter, 0x98, prGlue->rRfRxStat.u4BkMcr98);
    HAL_MCR_WR(prAdapter, 0x60, prGlue->rRfRxStat.u4BkMcr60);
#endif
}
#endif
#endif

