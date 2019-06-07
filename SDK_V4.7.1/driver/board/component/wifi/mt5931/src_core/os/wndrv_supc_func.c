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

#define __WNDRV_OS_FILES_C__
//#include "kal_release.h"
//#include "app_ltlcom.h"

#include "wndrv_cnst.h"
#include "wndrv_cal.h"
#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"
#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"
#include "wndrv_tcpip_msg.h"
#include "wndrv_tool_types.h"
#include "wndrv_tool_msg.h"
#include "wndrv_abm_msg.h"
//#include "uart_sw.h"
#include "bluetooth_struct.h"
#include "wmt_struct.h"
#include "wndrv_os_private.h"
#include "precomp.h"
#include "wndrv_context.h"
//#include "lcd_if.h"

#include "wndrv_param.h"
#ifdef __WIFI_SNIFFER_SUPPORT__
#include "wndrv_crypt_aes.h"
#endif
/* -------------------------------------------------------------------------- */
kal_bool WNDRVBearerEnable = KAL_TRUE;//KAL_FALSE
int wifi_under_cam_mode = 0;
//kal_bool SUPCAuthProcessing = KAL_FALSE;
extern wndrv_context_struct wndrv_context;
extern const UINT_8 aucDataRate[];
extern kal_uint8 BT_exist(void);

static kal_bool     bScanExplicitType;     /* determinate explicit scan or implicit scan */
static kal_uint32   iSessionID = 0 ;            /* for session id */
static kal_uint32   freq_record = 0xFFFFFFFF; /* for WiFi channel indicate handler */
static kal_uint8    PTA_record = 0xFF;        /* for WiFi channel indicate handler */
kal_bool            fgStaModeActive = KAL_FALSE;

extern PINT_8 wndrv_firmware_p;
extern UINT_32 wndrv_firmware_size;

//#if CFG_SINGLE_ANTENNA_ENABLED
static kal_bool    isCCK = KAL_FALSE;
static wndrv_wmt_conn_status_update_ind_struct last_update_rec;
//#endif
typedef struct _CTS_STRUCT {
    UINT16 frame_ctrl;
    UINT16 duration;
    UINT8 addr[6];
} __KAL_ATTRIB_PACKED__ WLAN_CTS_FRAME_T, *P_WLAN_CTS_FRAME_T;

#if 0
static const kal_uint32 freqTable24[] = {
    0,
    2412000,
    2417000,
    2422000,
    2427000,
    2432000,
    2437000,
    2442000,
    2447000,
    2452000,
    2457000,
    2462000,
    2467000,
    2472000,
    2484000
};

//#if defined(__VOIP__)
#include "evinf.h"
extern void network_event_callback(EVINF_INFO_ID id, kal_uint32 parm);
#endif
extern kal_uint32     Protocol_Status4WLAN;
extern kal_uint32     wndrv_UAPSD_isEnable;

//#if CFG_SINGLE_ANTENNA_ENABLED
void wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFIEVENT wifi_evt);
#if CFG_DISABLE_PS_MODE
void wndrv_abm_pseudo_set_ps_mode(void);
#endif
//#endif /*CFG_SINGLE_ANTENNA_ENABLED*/
/* For setting IBSS mode channel in WPA2 test */
#define INIT_IBSS_CH 15
UINT_32 u4IbssCh = INIT_IBSS_CH;

kal_bool WNDRVONOFFTEST = KAL_FALSE;

/* 20100625 WNDRV_LIB*/
extern kal_bool WiFi_Query_Bluetooth_Capability(void);
extern void wndrv_one(void);
extern void wndrv_two(void);
extern void wndrv_three(kal_bool in_progress);
extern void wndrv_four(void);
/* [MAUI_03125163][1]Assert fail: l1sm.c 4881 0x0 0x0 0x0 - (LISR)CTIRQ1 */
extern void L1SM_EnterFPM(kal_bool isFPM, kal_uint8 module);
/*******************************************************************************
*
*  Local Functions
*
*******************************************************************************/

/*******************************************************************************
* FUNCTION
*   status_ind_handler
*
* DESCRIPTION
*   Send MSG_ID_WNDRV_SUPC_STATUS_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void status_ind_handler(void *in_p)
{
    ilm_struct                    *ilm_p;
    wndrv_supc_status_ind_struct  *ind_p;
    P_PARAM_AUTH_EVENT_T pAuth = (P_PARAM_AUTH_EVENT_T)in_p;

    /*ind_p = (wndrv_supc_status_ind_struct *)
    construct_local_para( sizeof( wndrv_supc_status_ind_struct ), TD_CTRL );*/
    ind_p = (wndrv_supc_status_ind_struct *)
            construct_local_para(sizeof(wndrv_supc_status_ind_struct), TD_CTRL);

    WNDRV_EXT_ASSERT(ind_p, "status_ind_handler: ind_p can't get local parameter", 0, 0, 0);

    kal_mem_cpy(ind_p->bssid, &pAuth->arRequest[0].arBssid, WNDRV_MAC_ADDRESS_LEN);
    ind_p->flags = pAuth->arRequest[0].u4Flags;

    ilm_p                 = allocate_ilm(MOD_WNDRV);
    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_STATUS_IND; //MSG_ID_WNDRV_SUPC_STATUS_IND
    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_SUPC(ilm_p);
}


/*******************************************************************************
* FUNCTION
*   pmkid_candidate_ind_handler
*
* DESCRIPTION
*   Send MSG_ID_WNDRV_SUPC_PMKID_CANDIDATE_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void pmkid_candidate_ind_handler(void *in_p)
{
    ilm_struct                            *ilm_p;
    wndrv_supc_pmkid_candidate_ind_struct *ind_p;
    kal_int32                              i;
    P_PARAM_STATUS_INDICATION_T pStatus = (P_PARAM_STATUS_INDICATION_T)in_p;
    P_PARAM_PMKID_CANDIDATE_LIST_T pPmkid =
        (P_PARAM_PMKID_CANDIDATE_LIST_T)(pStatus + ALIGN_4(sizeof(ENUM_STATUS_TYPE_T)));

    /*ind_p = (wndrv_supc_pmkid_candidate_ind_struct *)
            construct_local_para( sizeof( wndrv_supc_pmkid_candidate_ind_struct ), TD_CTRL );*/
    ind_p = (wndrv_supc_pmkid_candidate_ind_struct *)
            construct_local_para(sizeof(wndrv_supc_pmkid_candidate_ind_struct), TD_CTRL);

    WNDRV_EXT_ASSERT(ind_p, "pmkid_candidate_ind_handler: ind_p can't get local parameter", 0, 0, 0);
    ind_p->num = pPmkid->u4NumCandidates;

    WNDRV_EXT_ASSERT(ind_p->num <= WNDRV_PMKID_CACHE_NUM,
                     "pmkid_candidate_ind_handler: ind_p->num should smaller than WNDRV_PMKID_CACHE_NUM",
                     ind_p->num, 0, 0);

    for (i = 0; i < ind_p->num; i++) {
        wndrv_supc_pmkid_candidate_struct        *out_pmkid_p;
        out_pmkid_p = &ind_p->list[i];
        kal_mem_cpy(out_pmkid_p->bssid, &(pPmkid->arCandidateList[i].arBSSID), WNDRV_MAC_ADDRESS_LEN);
        out_pmkid_p->preauth_enabled = (kal_bool)(pPmkid->arCandidateList[i].u4Flags);
    }

    ilm_p                 = allocate_ilm(MOD_WNDRV);
    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_PMKID_CANDIDATE_IND;//MSG_ID_WNDRV_SUPC_PMKID_CANDIDATE_IND
    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_SUPC(ilm_p);
}


/*******************************************************************************
*
*  Global Request Functions
*
*******************************************************************************/
/*******************************************************************************
* FUNCTION
*   wndrv_supc_init_req_handler
*
* DESCRIPTION
*   Corresponding to OID_802_11_CAPABILITY.
*   Send MSG_ID_WNDRV_SUPC_INIT_CNF.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_init_req_handler(ilm_struct *ilm_ptr)
{
    ilm_struct                         *ilm_p;
    wndrv_supc_init_cnf_struct         *cnf_p;
    wndrv_supc_param_capability_struct *out_capability_p;
    P_PARAM_CAPABILITY_T                in_capability_p;
    kal_uint32                         setBufLen;
    kal_uint32                         setInfoLen;
    kal_int32                          status, i;
    P_REG_INFO_T                        prRegInfo = &wndrv_context.adpt_p->prGlueInfo->rRegInfo;

    WNDRV_TRACE0(TRACE_INIT, INFO_WNDRV_SUPC_INIT_REQ,
                 "SUPC_INIT_REQ: *****************");

    //lcd__hpi_module_enable(LCD__HPI_MODULE_WIFI_IS_ENABLED);
    L1SM_EnterFPM(KAL_TRUE, 2);
    //wndrv_bus_hpi_module_control(1);

    fgStaModeActive = KAL_TRUE;

    if (TRUE == wndrv_context.glue_p->fgIsAdaptStopped) {
        WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
#ifdef __WAPI_SUPORT__
        /* reset to default mode */
        wndrv_context.adpt_p->fgUseWapi = FALSE;
#endif /*__WAPI_SUPORT__*/
        u4Status = wlanAdapterStart(wndrv_context.adpt_p, prRegInfo, (PVOID)wndrv_firmware_p, wndrv_firmware_size);
        if (u4Status != WLAN_STATUS_SUCCESS) {
            u4Status = wlanWholeChipReset(wndrv_context.adpt_p);
        }
        WNDRV_ASSERT(u4Status == WLAN_STATUS_SUCCESS);

        wndrv_context.glue_p->fgIsAdaptStopped = FALSE;
    }

    /********************************************
        *  1. Prepare confirm primitive
        ********************************************/
    // statistics time interval is one sec
    cnf_p = (wndrv_supc_init_cnf_struct *)
            construct_local_para(sizeof(wndrv_supc_init_cnf_struct), TD_CTRL);

    WNDRV_EXT_ASSERT(cnf_p, "wndrv_supc_init_req_handler: cnf_p can't get local parameter", 0, 0, 0);

    setBufLen = OFFSET_OF(PARAM_CAPABILITY_T, arAuthenticationEncryptionSupported) + WNDRV_PMKID_CACHE_NUM * sizeof(PARAM_AUTH_ENCRYPTION);
    in_capability_p = (P_PARAM_CAPABILITY_T) WNDRV_ALLOC_BUF(setBufLen);
    WNDRV_EXT_ASSERT(in_capability_p, "wndrv_supc_init_req_handler: in_capability_p can't get system buffer", 0, 0, 0);

    out_capability_p = &cnf_p->capability;

    /********************************************
        *  2. Query Capability();
        ********************************************/
    status =
        wlanoidQueryCapability(wndrv_context.adpt_p,
                               in_capability_p,
                               setBufLen,
                               (PUINT_32)&setInfoLen);
    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_init_req_handler: wlanoidQueryCapability fail", status, 0, 0);

    out_capability_p->version = in_capability_p->u4Version;
    out_capability_p->num_pmkids = in_capability_p->u4NoOfPMKIDs;
    out_capability_p->num_auth_encry_pairs_supported = in_capability_p->u4NoOfAuthEncryptPairsSupported;

    for (i = 0; i < out_capability_p->num_auth_encry_pairs_supported; i++) {
        out_capability_p->auth_encry_supported[i].auth_mode = (wndrv_supc_auth_mode_enum)(in_capability_p->arAuthenticationEncryptionSupported[i].eAuthModeSupported);
        out_capability_p->auth_encry_supported[i].encryption = (wndrv_supc_encryption_status_enum)(in_capability_p->arAuthenticationEncryptionSupported[i].eEncryptStatusSupported);
    }

    WNDRV_FREE_BUF(in_capability_p);

    /* For WNDRV_L1STATE_ADDRESS */
    MT5931_TRACE0(TRACE_INIT, MT5931_INFO_326, "[L1]wndrv_one()");
    wndrv_one();

#if 0
    kal_mem_cpy(cnf_p->mac_addr, wndrv_context.adpt_p->aucMacAddress, WNDRV_MAC_ADDRESS_LEN);
#else
    kal_mem_cpy(cnf_p->mac_addr, wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].aucOwnMacAddr, WNDRV_MAC_ADDRESS_LEN);
#endif

    if (WNDRVONOFFTEST) {
        //free_ilm(ilm_ptr);
        //cancel_ilm(MOD_SUPC);
        free_local_para((local_para_struct *)cnf_p);

        kal_sleep_task(100);
        ilm_p                 = allocate_ilm(MOD_SUPC);
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_DEINIT_REQ;
        ilm_p->local_para_ptr = NULL;
        ilm_p->peer_buff_ptr  = NULL;

        SEND_ILM(MOD_SUPC, MOD_WNDRV, WNDRV_SUPC_SAP, ilm_p);
        //wndrv_main( ilm_p );
    } else {
        ilm_p                 = allocate_ilm(MOD_WNDRV);
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_INIT_CNF;//MSG_ID_WNDRV_SUPC_INIT_CNF
        ilm_p->local_para_ptr = (local_para_struct *) cnf_p;
        ilm_p->peer_buff_ptr  = NULL;

        WNDRV_SEND_MSG_TO_SUPC(ilm_p);

#if CFG_SINGLE_ANTENNA_ENABLED
        // notify wifi power on
        if (wndrv_context.adpt_p->rPtaInfo.fgSingleAntenna) {
            last_update_rec.wifi_eventid = WMT_WIFI_NONE;
            wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFI_UP);
        }
#endif  /* CFG_SINGLE_ANTENNA_ENABLED */

#if CFG_DISABLE_PS_MODE
        wndrv_abm_pseudo_set_ps_mode();
#endif

    }  /* WNDRVONOFFTEST */
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_deinit_req_handler
*
* DESCRIPTION
*
*   Send MSG_ID_WNDRV_SUPC_DEINIT_CNF.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_deinit_req_handler(ilm_struct *ilm_ptr)
{
    ilm_struct                  *ilm_p;
    PTA_IPC_T                   PtaIpc;
    UINT_32                     fg1WireMode;
    UINT_32                     u4Len;
    //PARAM_CUSTOM_MCR_RW_STRUC_T rMcrWrInfo;
    //UINT_32                     u4InfoLen;
    //WLAN_STATUS                 status;
    UINT_32 i;

    /* NOTE(Nelson): Set antenna path to BT for E2 workaround */
//#if !CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    if (wndrv_context.adpt_p->ucRevID == MT5931_E2_REVERSION_ID)
//#endif
    {
        /* Set PIN mux for ANT_SEL */
        //rMcrWrInfo.u4McrOffset = 0x80050190;
        //rMcrWrInfo.u4McrData   = 0x00100000;
        //status = wlanoidSetMcrWrite(wndrv_context.adpt_p, &rMcrWrInfo, sizeof(rMcrWrInfo), &u4InfoLen);
        //kalUdelay(500);
        /* Switch ANT path */
#if 1
        fg1WireMode = (VAR_WMT_CONFIG_ANT_SEL | BIT(4));    //ansel1 Bit0 (BT)
#else
        fg1WireMode = (VAR_WMT_CONFIG_WIFI_1WIRE_MODE | BT_EXT_1_WIRE_MODE << 8 | BIT(4));  //ansel1 Bit0 (BT)
#endif
        wndrv_context.adpt_p->fgIsAntBT = TRUE;

        PtaIpc.u.aucBTPParams[3] = (UINT_8) fg1WireMode;
        PtaIpc.u.aucBTPParams[2] = (UINT_8)(fg1WireMode >> 8);
        PtaIpc.u.aucBTPParams[1] = (UINT_8)(fg1WireMode >> 16);
        PtaIpc.u.aucBTPParams[0] = (UINT_8)(fg1WireMode >> 24);

        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);

        kalMainThread(wndrv_context.glue_p);

    }

#if CFG_ENABLE_WIFI_TETHERING
    if (wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable == FALSE) //deinit ap&sta mode driver
#else
    if (1)
#endif
    {
        GLUE_SPIN_LOCK_DECLARATION();

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wlanAdapterStop(wndrv_context.adpt_p);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wndrv_context.glue_p->fgIsAdaptStopped = TRUE;

        /* Add stop all event timers when de-init */
        for (i = 0; i < PARAM_WNDRV_ES_NUM; i ++) {
            kal_event_stop_timer((wndrv_context.glue_p->prParamWndrvEs[i]));
        }
    } else { //deinit sta mode driver
        wlanAdapterStopForAPorSTA(wndrv_context.adpt_p, KAL_FALSE);
    }
    fgStaModeActive = KAL_FALSE;

    if (WNDRVONOFFTEST) {
        //free_ilm(ilm_ptr);
        //cancel_ilm(MOD_SUPC);

        kal_sleep_task(100);
        ilm_p                 = allocate_ilm(MOD_SUPC);
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_INIT_REQ;
        ilm_p->local_para_ptr = NULL;
        ilm_p->peer_buff_ptr  = NULL;

        SEND_ILM(MOD_SUPC, MOD_WNDRV, WNDRV_SUPC_SAP, ilm_p);
        //wndrv_main( ilm_p );

    } else {
        ilm_p                 = allocate_ilm(MOD_WNDRV);
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_DEINIT_CNF;
        ilm_p->local_para_ptr = NULL;
        ilm_p->peer_buff_ptr  = NULL;

        WNDRV_SEND_MSG_TO_SUPC(ilm_p);

#if CFG_SINGLE_ANTENNA_ENABLED
        if (wndrv_context.adpt_p->rPtaInfo.fgSingleAntenna) {
            // notify wifi power down
            wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFI_DOWN);
        }
#endif  /* CFG_SINGLE_ANTENNA_ENABLED */

    }   /* WNDRVONOFFTEST */

    //lcd__hpi_module_disable(LCD__HPI_MODULE_WIFI_IS_ENABLED);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_hpi_module_control(0);
#endif
    L1SM_EnterFPM(KAL_FALSE, 2);

}

void send_cts_frame(IN P_ADAPTER_T prAdapter, IN kal_uint8 *paddr, IN kal_uint32 len)
{
    P_MSDU_INFO_T prMsduInfo;
    P_WLAN_CTS_FRAME_T pctsstr = NULL;

    if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, sizeof(WLAN_CTS_FRAME_T))) == NULL) {
        ASSERT(0);
    }

    pctsstr = (P_WLAN_CTS_FRAME_T)(prMsduInfo->prPacket);
    pctsstr->frame_ctrl = 0x00C4;
    pctsstr->duration = 0x7FFF;
    kal_mem_cpy((void *) & (pctsstr->addr[0]), (void *)paddr, len);

    prMsduInfo->eSrc = TX_PACKET_MGMT;
    prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
    prMsduInfo->ucStaRecIndex = 0;
    prMsduInfo->ucNetworkType = NETWORK_TYPE_AIS_INDEX;
    prMsduInfo->ucMacHeaderLength = 10;
    prMsduInfo->fgIs802_1x = FALSE;
    prMsduInfo->fgIs802_11 = TRUE;
    prMsduInfo->u2FrameLength = 10;
    prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
    prMsduInfo->pfTxDoneHandler = NULL;
    prMsduInfo->fgIsBasicRate = TRUE;

    nicTxEnqueueMsdu(prAdapter, prMsduInfo);
    return;
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_data_req_handler
*
* DESCRIPTION
*   None
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_data_req_handler(ilm_struct *ilm_ptr)
{
    peer_buff_struct           *peer_p = ilm_ptr->peer_buff_ptr;
    wndrv_supc_data_req_struct *local_p = (wndrv_supc_data_req_struct *) ilm_ptr->local_para_ptr;
    wndrv_pkt_descriptor_struct        *wndrv_pkt_p;
    kal_uint8                   temp[ WNDRV_ETHERNET_MAC_SPACE ];

    kal_uint8                    *data_p;
    kal_uint16                    pdu_len;
    kal_uint8                    isKey4 = 0;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlueInfo = prAdapter->prGlueInfo;
    P_PACKET_INFO_T prPacketInfo;
    //WLAN_STATUS rStatus;

    WNDRV_TRACE0(TRACE_TXRX, INFO_WNDRV_SUPC_DATA_REQ,
                 "SUPC_DATA_REQ: *****************");

    WNDRV_EXT_ASSERT(MOD_SUPC == ilm_ptr->src_mod_id, "Data Source is not MOD_SPUC", ilm_ptr->src_mod_id, 0, 0);
    /****************************************************************************
    *  1. Fill Ethernet MAC header
    ****************************************************************************/
    isKey4 = *((kal_uint8 *) peer_p + sizeof(*peer_p) + peer_p->free_header_space - 1);

    kal_mem_cpy(&temp[ 0 ], local_p->dest_mac_addr,
                WNDRV_MAC_ADDRESS_LEN);

    kal_mem_cpy(&temp[ WNDRV_MAC_ADDRESS_LEN ], local_p->src_mac_addr,
                WNDRV_MAC_ADDRESS_LEN);

    temp[ 2 * WNDRV_MAC_ADDRESS_LEN ] = (kal_uint8)(local_p->proto_type >> 8);
    temp[ 2 * WNDRV_MAC_ADDRESS_LEN + 1 ] = (kal_uint8)(local_p->proto_type & 0xff);
    prepend_to_peer_buff(peer_p, temp, WNDRV_ETHERNET_MAC_SPACE);


    /****************************************************************************
    *  2. Hold peer buffer
    *     After it's sent, free peer buffer then.
    ****************************************************************************/
    wndrv_context.glue_p->tx_sys_peer_num++;
    /****************************************************************************
    *  3. Fill FLC related parameters in free header of peer buffer
    ****************************************************************************/

    wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);
    data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);

    wndrv_pkt_p->tx_peer_p        = peer_p;
    wndrv_pkt_p->src_mod_id    = ilm_ptr->src_mod_id;
    wndrv_pkt_p->flc_pool_type = 0;
    wndrv_pkt_p->pool_id       = FLC2_POOL_ID_APPPDU_UL;
    wndrv_pkt_p->flc_mode      = KAL_FALSE;

    //prPacketInfo = kalMemAlloc( sizeof(PACKET_INFO_T) );
    prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);

    wndrv_pkt_p->tid = 0;
    wndrv_pkt_p->queue_num     = mpMapTidToQueue(wndrv_pkt_p->tid);
    wndrv_pkt_p->check_mark    = WNDRV_BUFFER_HDLR_CHECK_MARK;
    wndrv_pkt_p->flags         = 0;

    wndrv_pkt_p->sys_time = kalGetTimeTick();
    wndrv_pkt_p->prPacketInfo = prPacketInfo;
    prPacketInfo->prPacket = (P_NATIVE_PACKET) peer_p;
    prPacketInfo->wndrv_pkt_p = wndrv_pkt_p;
    prPacketInfo->fgIsP2P = FALSE;

    pdu_len -= WNDRV_ETHERNET_MAC_SPACE;

    PACKET_INFO_INIT(prPacketInfo, \
                     FALSE, \
                     TRUE, \
                     peer_p, \
                     wndrv_pkt_p->tid, \
                     WNDRV_ETHERNET_MAC_SPACE, \
                     pdu_len, \
                     (((PUINT_8)data_p) + WNDRV_MAC_ADDRESS_LEN)\
                    );
#if 0
    if (pdu_len > 1000) {

        WNDRV_ASSERT(0); // abnormal packet len

    }
#endif
    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

#if 0 //TODO(Nelson): Handling SUPC data TX
    if ((rStatus = wlanSendPacket(prAdapter, prPacketInfo)) == WLAN_STATUS_PENDING) {
        prGlueInfo->u4TxPendingFrameNum++;
    }

    else if (rStatus == WLAN_STATUS_SUCCESS) {
        /* NOTE: When rStatus == WLAN_STATUS_SUCCESS, packet will be freed
         * by WLAN driver internal.
         */
    } else  if (rStatus == WLAN_STATUS_FAILURE) {
        //free_peer_buff( peer_p );
        free_peer_buff(peer_p);
        wndrv_context.glue_p->tx_sys_peer_num--;
    } else {
        WNDRV_ASSERT(0);
    }
#else
    //we send a CTS-to-self here to occupy the channel to avoid some AP send GTK before we install the PTK.
    if (isKey4 == 0x34) {
        kal_wap_trace(MOD_WNDRV, TRACE_INFO, "KEY 4, send CTS");
        send_cts_frame(prAdapter, local_p->src_mac_addr, WNDRV_MAC_ADDRESS_LEN);
    }
    wlanSendPacket(prGlueInfo, wndrv_pkt_p);
#endif

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    ilm_ptr->peer_buff_ptr = NULL;
}

/*******************************************************************************
* FUNCTION
*   tcpip_wndrv_data_req_handler
*
* DESCRIPTION
*   None
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void tcpip_wndrv_data_req_handler(ilm_struct *ilm_ptr)
{
    peer_buff_struct             *peer_p = ilm_ptr->peer_buff_ptr;
    tcpip_wndrv_data_req_struct  *local_p = (tcpip_wndrv_data_req_struct *) ilm_ptr->local_para_ptr;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;
    //kal_uint8                     temp[ WNDRV_ETHERNET_MAC_SPACE ];

    kal_uint8                    *data_p;
    kal_uint16                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlueInfo = prAdapter->prGlueInfo;
    P_PACKET_INFO_T prPacketInfo;

    //debug
    WNDRV_EXT_ASSERT(MOD_TCPIP == ilm_ptr->src_mod_id, "Data Source is not MOD_TCPIP", ilm_ptr->src_mod_id, 0, 0);

#if 0
    /****************************************************************************
    *  1. Fill Ethernet MAC header
    ****************************************************************************/
    kal_mem_cpy(&temp[ 0 ], local_p->dest_mac_addr,
                WNDRV_MAC_ADDRESS_LEN);

    kal_mem_cpy(&temp[ WNDRV_MAC_ADDRESS_LEN ], local_p->src_mac_addr,
                WNDRV_MAC_ADDRESS_LEN);

    temp[ 2 * WNDRV_MAC_ADDRESS_LEN ] = (kal_uint8)(local_p->proto_type >> 8);
    temp[ 2 * WNDRV_MAC_ADDRESS_LEN + 1 ] = (kal_uint8)(local_p->proto_type & 0xff);
    prepend_to_peer_buff(peer_p, temp, WNDRV_ETHERNET_MAC_SPACE);
#endif
    /****************************************************************************
    *  2. Hold peer buffer
    *     After it's sent, free peer buffer then.
    ****************************************************************************/
    wndrv_context.glue_p->tx_flc_peer_num++;

    //ILMTraceCheck(101);
    /****************************************************************************
    *  3. Fill FLC related parameters in free header of peer buffer
    ****************************************************************************/
    wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

    if (peer_p->free_header_space < (WNDRV_FREE_HEADER_SPACE - WNDRV_ETHERNET_MAC_SPACE)) {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_327, "tcpip_wndrv_data_req_handler: peer_p->free_header_space = %d < %d",
                     peer_p->free_header_space, (WNDRV_FREE_HEADER_SPACE - WNDRV_ETHERNET_MAC_SPACE));
    }

    data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);
    wndrv_pkt_p->tx_peer_p     = peer_p;
    wndrv_pkt_p->src_mod_id    = ilm_ptr->src_mod_id;
#if defined (WLAN_ZERO_COPY)
    wndrv_pkt_p->flc_pool_type = local_p->flc_pool_type;
    wndrv_pkt_p->pool_id       = (flc2_pool_id_enum)(local_p->pool_id);
    wndrv_pkt_p->flc_mode      = KAL_TRUE;
#else
    wndrv_pkt_p->flc_pool_type = 0;
    wndrv_pkt_p->pool_id       = 0;
    wndrv_pkt_p->flc_mode      = KAL_FALSE;
#endif

    prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);

    //zhilin_20131022: Set AP or STA flag according to SRC MAC address
    if (kal_mem_cmp(local_p->src_mac_addr, \
                    wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr, \
                    WNDRV_MAC_ADDRESS_LEN) == 0) { //AP mode
        prPacketInfo->fgIsP2P = TRUE;
    } else if (kal_mem_cmp(local_p->src_mac_addr, \
                           wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].aucOwnMacAddr, \
                           WNDRV_MAC_ADDRESS_LEN) == 0) { //STA mode
        prPacketInfo->fgIsP2P = FALSE;
    } else {
        ASSERT(0);
    }

    if (wndrv_context.adpt_p->fgIsWmmAssoc == TRUE) {
        wndrv_pkt_p->tid = local_p->tid;
    } else {
        wndrv_pkt_p->tid = 0;
    }

    wndrv_pkt_p->queue_num     = mpMapTidToQueue(wndrv_pkt_p->tid);
    wndrv_pkt_p->check_mark    = WNDRV_BUFFER_HDLR_CHECK_MARK;
    wndrv_pkt_p->flags         = local_p->flags;

    wndrv_pkt_p->sys_time = kalGetTimeTick();
    wndrv_pkt_p->prPacketInfo = prPacketInfo;
    prPacketInfo->prPacket = (P_NATIVE_PACKET) peer_p;
    prPacketInfo->wndrv_pkt_p = wndrv_pkt_p;

    pdu_len -= WNDRV_ETHERNET_MAC_SPACE;

    PACKET_INFO_INIT(prPacketInfo, \
                     FALSE, \
                     FALSE, \
                     peer_p, \
                     wndrv_pkt_p->tid, \
                     WNDRV_ETHERNET_MAC_SPACE, \
                     pdu_len, \
                     (((PUINT_8)data_p))\
                    );

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_6, "data_p+6: "MACSTR, MAC2STR(prPacketInfo->pucDestAddr));
    // MT5931_TRACE(TRACE_GROUP_6, MT5931_INFO_329, "prPacketInfo->pDestAddr: "MACSTR" peer_p %x", MAC2STR(prPacketInfo->pucDestAddr), peer_p);
    //ILMTraceCheck(102);
    if (prGlueInfo->fgIsAdaptStopped == FALSE) {
        wlanSendPacket(prGlueInfo, wndrv_pkt_p);
    } else {
#if defined (WLAN_ZERO_COPY)
        flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);
#else
        free_peer_buff(peer_p);
#endif
    }
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
    //ILMTraceCheck(109);
    ilm_ptr->peer_buff_ptr = NULL;
}
/*******************************************************************************
* FUNCTION
*   tcpip_wndrv_setbearer_req_handler
*
* DESCRIPTION
*   None
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void tcpip_wndrv_setbearer_req_handler(ilm_struct *ilm_ptr)
{
    tcpip_wndrv_setbearer_req_struct  *local_p = (tcpip_wndrv_setbearer_req_struct *) ilm_ptr->local_para_ptr;

    if (KAL_TRUE == local_p->status) {
        WNDRVBearerEnable = KAL_TRUE;
    } else {
        WNDRVBearerEnable = KAL_FALSE;
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_site_survey_req_handler
*
* DESCRIPTION
*   Corresponding to:
*   OID_802_11_BSSID_LIST_SCAN
*
* CALLS
*   wlanoidSetBssidListScanByWindows
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_site_survey_req_handler(ilm_struct *ilm_ptr)
{
    kal_int32                          status;
    wndrv_supc_site_survey_req_struct  *local_p = (wndrv_supc_site_survey_req_struct *) ilm_ptr->local_para_ptr;

    PARAM_SSID_T rNewSsid;
    UINT_32 u4BufLen;
    P_SCAN_INFO_T prScanInfo;
    prScanInfo = &(wndrv_context.adpt_p->rWifiVar.rScanInfo);

    /* NOTE (lcko): Site survey can be issued with or without SSID. When no SSID is specified,
        it implies we are not connected to any network now, which also means we will use wlanoidSetSSID
        to do scan. If we are already connected to any network, the SSID of this network will be used by
        SUPC task to issue site survey.
    */

    if (local_p->ssid_len) {
        rNewSsid.u4SsidLen = local_p->ssid_len;
        kal_mem_cpy(&rNewSsid.aucSsid, &local_p->ssid, local_p->ssid_len);

        prScanInfo->rSSIDOfScanReq.u4SsidLen = local_p->ssid_len;
        kal_mem_cpy(prScanInfo->rSSIDOfScanReq.aucSsid, &local_p->ssid, local_p->ssid_len);
#if 0 //TODO(Nelson): store the AP SSID to glue layer
        wndrv_context.glue_p->rScanSetting.ucSSIDLen =  local_p->ssid_len;
        kal_mem_cpy(&wndrv_context.glue_p->rScanSetting.aucSSID, &local_p->ssid, local_p->ssid_len);
#endif

    } else {
        rNewSsid.u4SsidLen = 0;
        prScanInfo->rSSIDOfScanReq.u4SsidLen = 0;
        kalMemZero(prScanInfo->rSSIDOfScanReq.aucSsid, PARAM_MAX_LEN_SSID);
#if 0 //TODO(Nelson): store the AP SSID to glue layer
        wndrv_context.glue_p->rScanSetting.ucSSIDLen = 0; //TODO(Nelson): store the AP SSID to glue layer
#endif

    }

    bScanExplicitType = KAL_TRUE; /* indicate the scan was issued by upper layer */
#if 0   //TODO(Nelson): set re-join, scan done int, resport scan result to glue layer
    wndrv_context.glue_p->fgIsSupcReqJoin = FALSE;  /* We are not going to do join */
    wndrv_context.glue_p->fgIsGenScanInt = TRUE;    /* Generate scan done INT at the end */
#endif
    wndrv_context.glue_p->fgIsReportScanResult = TRUE;  /* Generate scan result to avoid MMI hang */
    wndrv_context.adpt_p->WpsProbe = FALSE;

#ifdef __WPS_SUPPORT__
    /*check if KAL_TRUE, but not non-zero value. strictly.
       prevent from rubbish data interfering*/
    if (KAL_TRUE == local_p->has_wps_ie) {
        ADAPTER_T *p = wndrv_context.adpt_p;
        UINT_8  min = 0;
        wndrv_supc_wps_probe_ie *pie = &local_p->vendor_ie;
        p->WpsProbe = TRUE;
        //kalMemCopy(&wndrv_context.adpt_p->WpsIE, &local_p->vendor_ie, sizeof(WPS_PROBE_IE))
        p->WpsVer = pie->wps_ver;
        p->ReqType = pie->req_type;
        p->CfgMethod = pie->cfg_method;
        kal_mem_cpy(p->Uuid, pie->uuid, 16);
        p->Cate = pie->categ;
        p->SubCate = pie->sub_cate;
        p->Oui = pie->oui;
        p->RfBands = pie->rf_bands;
        p->AssocStat = pie->assoc_stat;
        p->CfgErr = pie->cfg_err;
        p->DevPswdId = pie->dev_pswd_id;
        /*for wps 2.0*/
        min = (pie->dev_name_len <= WPS_DEV_NAME_LEN) ? pie->dev_name_len : WPS_DEV_NAME_LEN;
        kal_mem_cpy(p->DevName, pie->dev_name, min);
        p->DevNameLen = min;

        min = (pie->model_name_len <= WPS_MODEL_NAME_LEN) ? pie->model_name_len : WPS_MODEL_NAME_LEN;
        kal_mem_cpy(p->ModelName, pie->model_name, min);
        p->ModelNameLen = min;

        min = (pie->model_num_len <= WPS_MODEL_NUM_LEN) ? pie->model_num_len : WPS_MODEL_NUM_LEN;
        kal_mem_cpy(p->ModelNum, pie->model_num, min);
        p->ModelNumLen = min;

        min = (pie->manuf_len <= WPS_MANUFACTURE_LEN) ? pie->manuf_len : WPS_MANUFACTURE_LEN;
        kal_mem_cpy(p->Manufact, pie->manufact, min);
        p->ManufLen = min;

        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_330, "store a wps probe req in adapter");
    }
#endif

    if ((wndrv_context.adpt_p->fgIsRadioOff) /*||(0 != local_p->ssid_len)*/) {
        status =   wlanSetInformation(wndrv_context.adpt_p,
                                      wlanoidSetSsid,
                                      &rNewSsid,
                                      sizeof(rNewSsid),
                                      &u4BufLen);
    } else { /* Use wlanoidSetBssidListScan when Raido is on */
        /* NOTE(lcko): Although in current version, we have implement radion turn on function inside wlanoidSetBssidListScan
                by transfer ARB state from IDLE to SCAN, we will be block by the fgIsRadioOff check in side this funcation. As a result,
                So we only use wlanoidSetBssidListScan when radio is already on.
            */
#if 0   //TODO(Nelson): set re-join to glue layer
        wndrv_context.glue_p->fgIsSupcReqJoin = TRUE;  /* Set to true bcs we are not in SEARCH state and thus will not perform join */
#endif
        /* NOTE(Nelson): For hidden SSID AP connection, the SSID of scan should be set. */
        status =   wlanSetInformation(wndrv_context.adpt_p,
                                      wlanoidSetBssidListScan,
                                      rNewSsid.aucSsid,
                                      rNewSsid.u4SsidLen,
                                      &u4BufLen);
    }

    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_site_survey_req_handler: wlanoidSetBssidListScanByWindows fail", status, 0, 0);
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_join_network_req_hanlder
*
* DESCRIPTION
*   Corresponding to:
*   OID_802_11_INFRASTRUCTURE_MODE
*   OID_802_11_AUTHENTICATION_MODE
*   OID_802_11_ENCRYPTION_STATUS
*   OID_802_11_SSID
*
* CALLS
*   wlanoidSetInfrastructureMode
*   wlanoidSetAuthMode
*   wlanoidSetEncryptionStatus
*   wlanoidSetSsid
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_join_network_req_hanlder(ilm_struct *ilm_ptr)
{
    wndrv_supc_join_network_req_struct   *join_p;
    PARAM_SSID_T                         *ssid_p;
    UINT_32                             setInfoLen;
    kal_uint32                           setBufLen;
    kal_int32                            status;
    kal_uint8                            key_index;
    wndrv_wep_info_struct                *in_p;
    PARAM_KEY                            *out_p;

    ENUM_PARAM_OP_MODE_T                eOpMode;
    ENUM_PARAM_AUTH_MODE_T              eAuthMode;
    ENUM_PARAM_ENCRYPTION_STATUS_T      eEncStatus;
#ifdef __WIFI_SNIFFER_SUPPORT__
    wndrv_supc_network_connect_fail_ind_struct *ind_p;
    ilm_struct                          *ilm_p;
#endif

#ifdef __WAPI_SUPPORT__
    UINT_32    u4SetWapiMode = FALSE;
    //BOOL       fgModeSwitch = FALSE;
    UCHAR      aucWapiAssocIE[64] = {0x44,
                                     22,
                                     1, 0,
                                     1, 0,
                                     0, 0x14, 0x72, 1,
                                     1, 0,
                                     0, 0x14, 0x72, 1,
                                     0, 0x14, 0x72, 1,
                                     0, 0,
                                     0, 0
                                    };
#endif /*__WAPI_SUPPORT__*/


#ifdef __WPS_SUPPORT__
    PUINT_8     pucBuffer;
    PUINT_8     pWpsAttr = NULL;
#endif

    WNDRV_TRACE0(TRACE_CONN, INFO_WNDRV_SUPC_JOIN_NETWORK_REQ,
                 "SUPC_JOIN_NETWORK_REQ: *********");


    join_p = (wndrv_supc_join_network_req_struct *) ilm_ptr->local_para_ptr;

#ifdef __WIFI_SNIFFER_SUPPORT__
    if (wndrv_context.adpt_p->u1SnifferOnOff) { //sniffer mode on
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "SUPC_JOIN_NETWORK_REQ: sniffer mode is on, respond join fail msg");
        iSessionID = join_p->session_id;

        ind_p = (wndrv_supc_network_connect_fail_ind_struct *)
                construct_local_para(sizeof(wndrv_supc_network_connect_fail_ind_struct), TD_CTRL);
        ind_p->session_id = iSessionID;

        ind_p->bssid_list_num = 0;

        ilm_p                 = allocate_ilm(MOD_WNDRV);
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND;//MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND
        ilm_p->local_para_ptr = (local_para_struct *) ind_p;
        ilm_p->peer_buff_ptr  = NULL;

        WNDRV_SEND_MSG_TO_SUPC(ilm_p);
        return;
    }
#endif

#ifdef __WAPI_SUPPORT__
    /* Saker : Check Auth Mode first for WAPI/WIFI Mode Dynamic switch */

    switch (join_p->auth_mode) {
        case WNDRV_SUPC_AUTH_MODE_OPEN:
            eAuthMode = AUTH_MODE_OPEN;

            break;

        case WNDRV_SUPC_AUTH_MODE_SHARED:
            eAuthMode = AUTH_MODE_SHARED;

            break;

        case WNDRV_SUPC_AUTH_MODE_AUTO_SWITCH:
            eAuthMode = AUTH_MODE_AUTO_SWITCH;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA:
            eAuthMode = AUTH_MODE_WPA;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA_PSK:
            eAuthMode = AUTH_MODE_WPA_PSK;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA_NONE:
            eAuthMode = AUTH_MODE_WPA_NONE;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA2:
            eAuthMode = AUTH_MODE_WPA2;

            break;

        case  WNDRV_SUPC_AUTH_MODE_WPA2_PSK:
            eAuthMode = AUTH_MODE_WPA2_PSK;

            break;
        case WNDRV_SUPC_AUTH_MODE_WAPI:
        case WNDRV_SUPC_AUTH_MODE_WAPI_PSK:
            u4SetWapiMode = TRUE;
            break;
        default:

            WNDRV_ASSERT(0);

    }

    // Set wapi flag for turn on/off wapi support
    if (u4SetWapiMode == TRUE && wndrv_context.adpt_p->fgUseWapi == FALSE) {
        wndrv_context.adpt_p->fgUseWapi = TRUE;
        //fgModeSwitch = TRUE;
        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_331, "WIFI Mode Switch to WAPI Mode");
    } else if (u4SetWapiMode == FALSE && wndrv_context.adpt_p->fgUseWapi == TRUE) {
        wndrv_context.adpt_p->fgUseWapi = FALSE;
        //fgModeSwitch = TRUE;
        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_332, "WAPI Mode Switch to WIFI Mode");
    } else {
        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_333, "Mode No Change");
    }

#endif /*__WAPI_SUPPORT__*/


    switch (join_p->network_type) {
        case WNDRV_SUPC_NETWORK_IBSS:
            eOpMode = NET_TYPE_IBSS;
            break;

        case WNDRV_SUPC_NETWORK_INFRASTRUCTURE:
            eOpMode = NET_TYPE_INFRA;
            break;

        case  WNDRV_SUPC_NETWORK_AUTO_UNKNOWN:
            eOpMode = NET_TYPE_AUTO_SWITCH;
            break;

        default:
            WNDRV_ASSERT(0);
    }

    /********************************************
     *  0. For WMM UAPSD test, configure related parameter first
     ********************************************/
    if (wndrv_UAPSD_isEnable) {
        wndrv_context.glue_p->UAPSDSetting.ucIsDisableUcTrigger = 0;
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_CONN, "SetWiFiWmmPsTest");
        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetWiFiWmmPsTest,
                           &wndrv_context.glue_p->UAPSDSetting,
                           sizeof(PARAM_CUSTOM_WMM_PS_TEST_STRUC_T),
                           (PUINT_32)&setInfoLen);
    }

    /********************************************
     *  1. Set Infrastructure or Ad-Hoc Mode
     ********************************************/
    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   wlanoidSetInfrastructureMode,
                                   &eOpMode,
                                   sizeof(eOpMode),
                                   (PUINT_32)&setInfoLen);

    /********************************
     set hasWpsIe flag, it will be used
     in assocComposeReAssocReqFrame
     ********************************/
    wndrv_context.adpt_p->WpsAssoc = FALSE;
#ifdef __WPS_SUPPORT__
    /*check if KAL_TRUE, but not non-zero value. strictly.
    prevent from rubbish data interfering*/
    if (KAL_TRUE == join_p->has_wps_ie) {
        wndrv_context.adpt_p->WpsAssoc = TRUE;
        wndrv_context.adpt_p->WpsVer = join_p->wps_version;
        wndrv_context.adpt_p->ReqType = WPS_REQ_ENROLLEE;

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_PEER, "%s build WPS IE", __func__);
        pucBuffer = (PUINT_8) & (wndrv_context.adpt_p->prGlueInfo->aucWSCAssocInfoIE);

        WPS_ASSOC_IE(pucBuffer)->ucId = ELEM_ID_VENDOR;
        WPS_ASSOC_IE(pucBuffer)->ucLength = VENDOR_OUI_LEN + WPS_ASSOC_REQ_ATTRS_LEN;

        WPA_PUT_BE32((PUINT_8) & (WPS_ASSOC_IE(pucBuffer)->Oui), WPS_DEV_OUI_WFA);
        /*compose version attr*/
        pWpsAttr = (PUINT_8) & (WPS_ASSOC_IE(pucBuffer)->Ver);
        WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_VER_ST(pWpsAttr)->Hdr.Id), ATTR_VERSION);
        WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_VER_ST(pWpsAttr)->Hdr.Len), WPS_VER_LEN);
        WPS_ATTR_VER_ST(pWpsAttr)->Ver = wndrv_context.adpt_p->WpsVer;
        /*Reqtype*/
        pWpsAttr = (PUINT_8) & (WPS_ASSOC_IE(pucBuffer)->ReqType);
        WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_REQTYPE_ST(pWpsAttr)->Hdr.Id), ATTR_REQUEST_TYPE);
        WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_REQTYPE_ST(pWpsAttr)->Hdr.Len), WPS_REQ_TYPE_LEN);
        WPS_ATTR_REQTYPE_ST(pWpsAttr)->ReqType = wndrv_context.adpt_p->ReqType;

        /*WFA VENDOR EXT*/
        /*add WFA VENDOR EXT VER2.0*/
        pWpsAttr = (PUINT_8) & (WPS_ASSOC_IE(pucBuffer)->WfaExt);
        WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_WFA_EXT_ST(pWpsAttr)->Hdr.Id), ATTR_VENDOR_EXT);
        WPA_PUT_BE16((PUINT_8) & (WPS_ATTR_WFA_EXT_ST(pWpsAttr)->Hdr.Len),
                     (WPS_WFA_ATTR_VID_LEN + WPS_WFA_ATTR_SUB_HDR_LEN + WPS_WFA_ATTR_SUB_VER_LEN));
        WPA_PUT_BE24((PUINT_8) & (WPS_ATTR_WFA_EXT_ST(pWpsAttr)->VID), ATTR_WFA_VENDOR_ID);

        /*put WFA payload*/
        /* TODO put ver2.0 */
        pWpsAttr = (PUINT_8) & (WPS_ASSOC_IE(pucBuffer)->WfaVer);
        WFA_SUB_ATTR_VER_ST(pWpsAttr)->Id = ATTR_WFA_SUB_VER_ID;
        WFA_SUB_ATTR_VER_ST(pWpsAttr)->Len = WPS_WFA_ATTR_SUB_VER_LEN;
        WFA_SUB_ATTR_VER_ST(pWpsAttr)->Ver = WPS_VERSION_V2;

        wndrv_context.adpt_p->prGlueInfo->u2WSCAssocInfoIELen = (UINT_16)ELEM_WPS_ASSOC_REQ_IE_LEN;
    }
#endif

#if 1
    if ((status != 0) && (status != WLAN_STATUS_PENDING)) {
        MT5931_TRACE(TRACE_ERROR, MT5931_INFO_334,
                     "wndrv_supc_join_network_req_hanlder: wlanoidSetInfrastructureMode failNo Change:%d", status);
        return;
    }
#else
    /* NOTE(Nelson): After driver enqueue cmd for FW, status is returned WLAN_STATUS_PENDING */
    WNDRV_EXT_ASSERT((status == 0) || (status == WLAN_STATUS_PENDING),
                     "wndrv_supc_join_network_req_hanlder: wlanoidSetInfrastructureMode fail", status, 0, 0);
#endif

    /* To set channel number used in IBSS mode.That means this field is
       useful when network_type is WNDRV_SUPC_NETWORK_IBSS. */

    if (join_p->network_type == WNDRV_SUPC_NETWORK_IBSS || join_p->network_type == WNDRV_SUPC_NETWORK_AUTO_UNKNOWN) {

        UINT_32 u4data;

#if 0   //TODO(Nelson): check if set channel is needed for IBSS on MT5931
        /* Select a random channel to create the IBSS */

        if (u4IbssCh == INIT_IBSS_CH) {
            u4data = ((UINT_32)rand()) % wndrv_context.adpt_p->u2NicOpChnlNum;
        } else {
            u4data = u4IbssCh;
        }

        u4data = wndrv_context.adpt_p->arNicOpChnList[u4data].ucChannelNum;

        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetChannel,
                                       &u4data,
                                       sizeof(u4data),
                                       (PUINT_32)&setInfoLen);

        WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_join_network_req_hanlder: wlanoidSetChannel fail", status, 0, 0);
#endif

        /* Set BI to 100 TU */
        u4data = 100;
        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetBeaconInterval,
                                       &u4data,
                                       sizeof(u4data),
                                       (PUINT_32)&setInfoLen);

        WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_join_network_req_hanlder: wlanoidSetBeaconInterval fail", status, 0, 0);
    }

    /********************************************
     *  2. Set Authentication Mode
     ********************************************/
#ifndef __WAPI_SUPPORT__
    switch (join_p->auth_mode) {
        case WNDRV_SUPC_AUTH_MODE_OPEN:
            eAuthMode = AUTH_MODE_OPEN;

            break;

        case WNDRV_SUPC_AUTH_MODE_SHARED:
            eAuthMode = AUTH_MODE_SHARED;

            break;

        case WNDRV_SUPC_AUTH_MODE_AUTO_SWITCH:
            eAuthMode = AUTH_MODE_AUTO_SWITCH;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA:
            eAuthMode = AUTH_MODE_WPA;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA_PSK:
            eAuthMode = AUTH_MODE_WPA_PSK;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA_NONE:
            eAuthMode = AUTH_MODE_WPA_NONE;

            break;

        case WNDRV_SUPC_AUTH_MODE_WPA2:
            eAuthMode = AUTH_MODE_WPA2;

            break;

        case  WNDRV_SUPC_AUTH_MODE_WPA2_PSK:
            eAuthMode = AUTH_MODE_WPA2_PSK;

            break;

        default:

            WNDRV_ASSERT(0);

    }
#endif /*not define __WAPI_SUPPORT__*/

#ifdef __WAPI_SUPPORT__
    if (join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WAPI || join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WAPI_PSK) {
        eAuthMode = AUTH_MODE_OPEN;
        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetAuthMode,
                                       &eAuthMode,
                                       sizeof(eAuthMode),
                                       (PUINT_32)&setInfoLen);

        MT5931_TRACE(TRACE_PEER, MT5931_INFO_335, "Set to auth mode status %d\n", status);

        eEncStatus = ENUM_WEP_DISABLED;
        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetEncryptionStatus,
                                       &eEncStatus,
                                       sizeof(eEncStatus),
                                       (PUINT_32)&setInfoLen);

        MT5931_TRACE(TRACE_PEER, MT5931_INFO_336, "Set to encrypt mode status %d\n", status);

        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetWapiMode,
                                       &u4SetWapiMode,
                                       sizeof(u4SetWapiMode),
                                       (PUINT_32)&setInfoLen);
        MT5931_TRACE(TRACE_PEER, MT5931_INFO_337, "Set to WAPI Mode status %d", status);

        if (join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WAPI_PSK) {
            aucWapiAssocIE[9] = 2;
        }
        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetWapiAssocInfo,
                                       &aucWapiAssocIE[0],
                                       24,
                                       (PUINT_32)&setInfoLen);
        MT5931_TRACE(TRACE_PEER, MT5931_INFO_338, "Set to assoc info status %d", status);
    } else
#endif /*__WAPI_SUPPORT__*/
    {
        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetAuthMode,
                                       &eAuthMode,
                                       sizeof(eAuthMode),
                                       (PUINT_32)&setInfoLen);
    }
    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_join_network_req_hanlder: wlanoidSetAuthMode fail", status, 0, 0);

    /********************************************
     *  3. Set Encryption mode
     ********************************************/

    switch (join_p->encryption) {
        case WNDRV_SUPC_WEP_ENABLED:
            eEncStatus = ENUM_WEP_ENABLED;

            break;

        case WNDRV_SUPC_WEP_DISABLED:
            eEncStatus = ENUM_WEP_DISABLED;
            break;


        case WNDRV_SUPC_WEP_KEY_ABSENT:
            eEncStatus = ENUM_WEP_KEY_ABSENT;
            break;

        case WNDRV_SUPC_WEP_NOT_SUPPORTED:
            eEncStatus = ENUM_WEP_NOT_SUPPORTED;
            break;

        case WNDRV_SUPC_ENCRYPTION2_ENABLED:
            eEncStatus = ENUM_ENCRYPTION2_ENABLED;
            break;

        case WNDRV_SUPC_ENCRYPTION2_KEY_ABSENT:
            eEncStatus = ENUM_ENCRYPTION2_KEY_ABSENT;
            break;

        case  WNDRV_SUPC_ENCRYPTION3_ENABLED:
            eEncStatus = ENUM_ENCRYPTION3_ENABLED;
            break;

        case WNDRV_SUPC_ENCRYPTION3_KEY_ABSENT:
            eEncStatus = ENUM_ENCRYPTION3_KEY_ABSENT;
            break;
#ifdef __WAPI_SUPPORT__
        case WNDRV_SUPC_WPI_ENABLED:
            break;
        case WNDRV_SUPC_WPI_KEY_ABSENT:
            break;
#endif /*__WAPI_SUPPORT__*/
        default:

            WNDRV_ASSERT(0);

    }
#ifdef __WAPI_SUPPORT__
    if ((join_p->encryption != WNDRV_SUPC_WPI_ENABLED) && (join_p->encryption != WNDRV_SUPC_WPI_KEY_ABSENT)) {
#endif /*__WAPI_SUPPORT__ */
        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetEncryptionStatus,
                                       &eEncStatus,
                                       sizeof(eEncStatus),
                                       (PUINT_32)&setInfoLen);
#ifdef __WAPI_SUPPORT__
    }
#endif /*__WAPI_SUPPORT__ */

    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_join_network_req_hanlder: wlanoidSetEncryptionStatus fail", status, 0, 0);

    /********************************************
     *  4. Set Pre-Share Key
     ********************************************/
    if (join_p->key_enable & 0x0F) {
        for (key_index = 0; key_index < 4; key_index++) {
            if ((join_p->key_enable >> key_index) & 0x01) {
                in_p = (wndrv_wep_info_struct *) &join_p->wep_info[key_index];
                setBufLen = OFFSET_OF(PARAM_KEY , KeyMaterial) + in_p->key_length;
                out_p = (PARAM_KEY *) WNDRV_ALLOC_BUF(setBufLen);
                WNDRV_EXT_ASSERT(out_p, "wndrv_supc_join_network_req_hanlder: out_p can't get system buffer", 0, 0, 0);
                out_p->Length      = setBufLen;
                out_p->KeyIndex    = in_p->key_index;
                out_p->KeyLength   = in_p->key_length;
                kal_mem_set(out_p->BSSID, 0xff, WNDRV_MAC_ADDRESS_LEN);
                kal_mem_cpy((kal_uint8 *)out_p->KeyMaterial, in_p->key_material, in_p->key_length);

                status =    wlanSetInformation(wndrv_context.adpt_p,
                                               wlanoidSetAddKey,
                                               (kal_uint8 *)out_p,
                                               setBufLen,
                                               (PUINT_32)&setInfoLen);

                //WNDRV_EXT_ASSERT( status == 0, "wndrv_supc_join_network_req_hanlder: wlanoidSetAddKey fail", status, 0, 0 );
                WNDRV_EXT_ASSERT(status == WLAN_STATUS_PENDING, "wndrv_supc_join_network_req_hanlder: wlanoidSetAddKey fail", status, 0, 0);
                WNDRV_FREE_BUF((kal_uint8 *)out_p);
            }
        }
    }


    /********************************************
     *  5. Set SSID
     ********************************************/
    ssid_p = (PARAM_SSID_T *) WNDRV_ALLOC_BUF(sizeof(PARAM_SSID_T));

    WNDRV_EXT_ASSERT(ssid_p, "ssid_p can't get system buffer", 0, 0, 0);

    ssid_p->u4SsidLen = join_p->ssid_len;

    kal_mem_cpy(ssid_p->aucSsid, join_p->ssid, ssid_p->u4SsidLen);

    iSessionID = join_p->session_id;
#if 0
    wndrv_context.glue_p->fgIsSupcReqJoin = TRUE;  /* We are really going to do join */
    wndrv_context.glue_p->fgIsGenScanInt = TRUE;    /* Generate scan done INT at the end */
    wndrv_context.glue_p->fgIsReportConnFail = TRUE;
#endif

    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   wlanoidSetSsid,
                                   ssid_p,
                                   sizeof(PARAM_SSID_T),
                                   (PUINT_32)&setInfoLen);

    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_join_network_req_hanlder: wlanoidSetSsid fail", status, 0, 0);

#if 0   //TODO(Nelson): check single antenna to set coex parameters for tx fragmentation on MT5931
    {
        PARAM_CUSTOM_BT_COEXIST_T set;
        kal_uint32             setInfoLen;
#if 1//#if CFG_SINGLE_ANTENNA_ENABLED
        set.rPtaParam.u4BtCR0 = wndrv_get_pta_setting(0, 0, 0);//DEFAULT_BTCR0 // enable PTA
        set.rPtaParam.u4BtCR1 = wndrv_get_pta_setting(0, 0, 1);//DEFAULT_BTCR1;
        set.rPtaParam.u4BtCR2 = DEFAULT_BTCR2;
        set.rPtaParam.u4BtCR3 = DEFAULT_BTCR3;
#else
        set.rPtaParam.u4BtCR0 = DEFAULT_BTCR0_ON_VALUE;  // enable PTA
        set.rPtaParam.u4BtCR1 = DEFAULT_BTCR1_VALUE;
        set.rPtaParam.u4BtCR2 = DEFAULT_BTCR2_VALUE;
        set.rPtaParam.u4BtCR3 = DEFAULT_BTCR3_VALUE;
#endif /*CFG_SINGLE_ANTENNA_ENABLED*/
        set.u4IsEnableTxAutoFragmentForBT = 0;//DEFAULT_TX_AUTO_FRAGMENT_FOR_BT;
        set.eBTCoexistWindowType = 0;//DEFAULT_BT_COEXIST_WINDOW_TYPE;

        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBtCoexistCtrl,
                           &set,
                           sizeof(PARAM_CUSTOM_BT_COEXIST_T),
                           (PUINT_32)&setInfoLen);
    }

    wndrv_context.glue_p->bSelfPTAOn = TRUE;
    wndrv_context.glue_p->SelfPTAOnScore = WNDRV_SELF_PTA_ON_THRESHOLD;
#endif

    WNDRV_FREE_BUF(ssid_p);

#if 0
    if ((join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WPA) || (join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WPA_PSK) ||
            (join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WPA2) || (join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WPA2_PSK)) {
        /* Create timer to remain CAM mode before add_key or timeout */
        kal_event_stop_timer(PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER);
        kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER],
                              WNDRV_ADD_KEY_FAIL_TIMEOUT);
        nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX, Param_PowerModeCAM, FALSE);
    }
#ifdef __WAPI_SUPPORT__
    else if ((join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WAPI) || (join_p->auth_mode == WNDRV_SUPC_AUTH_MODE_WAPI_PSK)) {
        /* Create timer to remain CAM mode before add_key or timeout */
        kal_event_stop_timer(PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER);
        kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER],
                              WNDRV_ADD_KEY_FAIL_TIMEOUT);
        nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX, Param_PowerModeCAM, FALSE);
    }
#endif
#endif
    /* 5921 driver will set a  connection timeout timer to check connection fail event; so we do nothing here */
#if 0
    kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_CONNETION_FAIL_TIMER],
                          KAL_TICKS_1_SEC * 5);
#endif
}


/*******************************************************************************
* FUNCTION
*   wndrv_supc_disjoin_network_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_disjoin_network_req_handler(ilm_struct *ilm_ptr)
{
    kal_uint32                  setInfoLen;
    kal_int32                   status;
    //ENUM_PARAM_OP_MODE_T        eOpMode;

    WNDRV_TRACE0(TRACE_DISCONN, INFO_WNDRV_SUPC_DISJOIN_NETWORK_REQ,
                 "SUPC_DISJOIN_NETWORK_REQ: ******");

    //kal_event_stop_timer(PARAM_WNDRV_ES_STAT_IND_TIMER);

    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   wlanoidSetDisassociate,
                                   NULL,
                                   0,
                                   (PUINT_32)&setInfoLen);

#if 0   //TODO(Nelson): IBSS mode handling
    //[MAUI_02148110] reset OPMODE to default for avoid IBSS abnormal
    if (wndrv_context.adpt_p->rConnSettings.eOPMode == NET_TYPE_IBSS) {
        eOpMode = NET_TYPE_INFRA;
        status =   wlanSetInformation(wndrv_context.adpt_p,
                                      wlanoidSetInfrastructureMode,
                                      &eOpMode,
                                      sizeof(eOpMode),
                                      (PUINT_32)&setInfoLen);
    }
#endif

    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_disjoin_network_req_handler: wlanoidSetDisassociate fail", status, 0, 0);
}


/*******************************************************************************
* FUNCTION
*   wndrv_supc_country_setting_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_country_setting_req_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_country_setting_req_struct   *in_p;
    COUNTRY_STRING_ENTRY                        out;
    UINT_32                             setInfoLen;
    kal_int32                               status;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_SUPC_COUNTRY_SETTING_REQ,
                 "COUNTRY_SETTING_REQ: ***********");

    in_p = (wndrv_supc_country_setting_req_struct *) ilm_ptr->local_para_ptr;

    out.aucCountryCode[0] = (kal_uint8)(in_p->country_code >> 8);
    out.aucCountryCode[1] = (kal_uint8)(in_p->country_code & 0xff);

#if 1
    status =    wlanoidSetCountryCode(wndrv_context.adpt_p,
                                      (PVOID)&out.aucCountryCode[0],
                                      2,
                                      &setInfoLen,
                                      TRUE);
#else
    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   wlanoidSetCountryCode,
                                   &out,
                                   2,
                                   (PUINT_32)&setInfoLen,
                                   TRUE);
#endif

    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_country_setting_req_handler: domainSetCurrentCountry fail", status, 0, 0);
}


/*******************************************************************************
* FUNCTION
*   wndrv_supc_remove_key_req_handler
*
* DESCRIPTION
*   Corresponding to:
*   OID_802_11_REMOVE_KEY
*
* CALLS
*   wlanoidSetRemoveKey
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_remove_key_req_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_remove_key_req_struct     *in_p;
    P_PARAM_REMOVE_KEY_T            out_p;
    kal_uint32                  setInfoLen;
    kal_int32                   status;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_SUPC_REMOVE_KEY_REQ,
                 "SUPC_REMOVE_KEY_REQ: ***********");

    in_p = (wndrv_supc_remove_key_req_struct *) ilm_ptr->local_para_ptr;
    out_p = (P_PARAM_REMOVE_KEY_T) WNDRV_ALLOC_BUF(sizeof(PARAM_REMOVE_KEY_T));
    WNDRV_EXT_ASSERT(out_p, "wndrv_supc_remove_key_req_handler: out_p can't get system buffer", 0, 0, 0);

    out_p->u4KeyIndex = in_p->key_index;
    kal_mem_cpy(&out_p->arBSSID, &in_p->bssid[0], WNDRV_MAC_ADDRESS_LEN);

    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   wlanoidSetRemoveKey,
                                   out_p,
                                   sizeof(PARAM_REMOVE_KEY),
                                   (PUINT_32)&setInfoLen);

    //WNDRV_EXT_ASSERT( status == 0, "wndrv_supc_remove_key_req_handler wlanoidSetRemoveKey fail", status, 0, 0 );
    WNDRV_EXT_ASSERT(status == WLAN_STATUS_PENDING, "wndrv_supc_remove_key_req_handler wlanoidSetRemoveKey fail", status, 0, 0);

    WNDRV_FREE_BUF(out_p);
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_add_key_req_handler
*
* DESCRIPTION
*   Corresponding to:
*   OID_802_11_ADD_KEY
*
* CALLS
*   wlanoidSetAddKey
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_add_key_req_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_add_key_req_struct   *in_p;
    P_PARAM_KEY_T              out_p;
    kal_uint32             setBufLen;
    kal_uint32             setInfoLen;
    kal_int32              status;
#ifdef __WAPI_SUPPORT__
    static PARAM_WPI_KEY_T       WpiKey;
#endif /*__WAPI_SUPPORT__*/

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_SUPC_ADD_KEY_REQ,
                 "SUPC_ADD_KEY_REQ: **************");

    in_p = (wndrv_supc_add_key_req_struct *) ilm_ptr->local_para_ptr;

#ifdef __WAPI_SUPPORT__
    if (wndrv_context.adpt_p->fgUseWapi == FALSE) //WPA/WPA2
#endif /*__WAPI_SUPPORT__*/
    {
        setBufLen = OFFSET_OF(PARAM_KEY, KeyMaterial) + in_p->key_length;

        out_p = (P_PARAM_KEY_T) WNDRV_ALLOC_BUF(setBufLen);
        WNDRV_EXT_ASSERT(out_p, "wndrv_supc_add_key_req_handler: out_p can't get systtem buffer", 0, 0, 0);

        out_p->u4Length = setBufLen;
        out_p->u4KeyIndex = in_p->key_index;
        out_p->u4KeyLength = in_p->key_length;
        kal_mem_cpy(&out_p->arBSSID, &(in_p->bssid[0]), WNDRV_MAC_ADDRESS_LEN);
        kal_mem_cpy((PUINT_8) & (out_p->rKeyRSC), (PUINT_8) & (in_p->key_rsc[0]), 4);
        kal_mem_cpy(((PUINT_8) & (out_p->rKeyRSC) + 4), (PUINT_8) & (in_p->key_rsc[1]), 4);
        kal_mem_cpy(&out_p->aucKeyMaterial[0], &(in_p->key_material[0]), in_p->key_length);

        status =    wlanSetInformation(wndrv_context.adpt_p,
                                       wlanoidSetAddKey,
                                       out_p,
                                       setBufLen,
                                       (PUINT_32)&setInfoLen);

        WNDRV_FREE_BUF(out_p);
    }
#ifdef __WAPI_SUPPORT__
    else { // WAPI
        WNDRV_ASSERT(wndrv_context.adpt_p->fgUseWapi);
        WNDRV_ASSERT(wndrv_context.adpt_p->rWifiVar.rConnSettings.fgWapiMode);

        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_339, "WAPI MODE ADD KEY");
        WNDRV_EXT_ASSERT(in_p->u4LenWPIEK == 16, "WPIEK Len %d", in_p->u4LenWPIEK, 0, 0);
        WNDRV_EXT_ASSERT(in_p->u4LenWPICK == 16, "WPICK Len %d", in_p->u4LenWPICK, 0, 0);

        //prWpiKey = (P_PARAM_WPI_KEY_T) WNDRV_ALLOC_BUF( sizeof(PARAM_WPI_KEY_T) );
        WpiKey.eKeyType   = (ENUM_KEY_TYPE)(in_p->eKeyType);
        WpiKey.eDirection = (ENUM_WPI_PROTECT_TYPE)(in_p->eDirection);
        WpiKey.ucKeyID    = in_p->ucKeyID;
        /*PN*/
        kalMemCopy(&(WpiKey.aucPN[0]), &(in_p->aucPN[0]), 16);
        /*BSSID*/
        kalMemCopy(&(WpiKey.aucAddrIndex[0]), &(in_p->aucAddrIndex[0]), 12);

        WpiKey.u4LenWPIEK = in_p->u4LenWPIEK;
        kalMemSet(&(WpiKey.aucWPIEK[0]), 0x00, sizeof(WpiKey.aucWPIEK));
        kalMemCopy(&(WpiKey.aucWPIEK[0]), &(in_p->aucWPIEK[0]), WpiKey.u4LenWPIEK);

        WpiKey.u4LenWPICK = in_p->u4LenWPICK;
        kalMemSet(&(WpiKey.aucWPICK[0]), 0x00, sizeof(WpiKey.aucWPICK));
        kalMemCopy(&(WpiKey.aucWPICK[0]), &(in_p->aucWPICK[0]), WpiKey.u4LenWPICK);

        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_340, "Set Wapi Key");
        status = wlanSetInformation(wndrv_context.adpt_p,
                                    wlanoidSetWapiKey,
                                    (PVOID)&WpiKey,
                                    sizeof(PARAM_WPI_KEY_T),
                                    (PUINT_32)&setInfoLen);
        MT5931_TRACE0(TRACE_PEER, MT5931_INFO_341, "Set wlanoidSetWapiKey Done");
    }
#endif /*__WAPI_SUPPORT__*/

    /* Set PS mode to config in adapter */

    //kal_event_stop_timer( PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER );
    //nicConfigPowerSaveProfile( wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX,
    //                            wndrv_context.adpt_p->u4PowerMode, FALSE );

    //WNDRV_EXT_ASSERT( status == 0, "wndrv_supc_add_key_req_handler: wlanoidSetAddKey fail", status, 0, 0 );
    WNDRV_EXT_ASSERT(status == WLAN_STATUS_PENDING, "wndrv_supc_add_key_req_handler: wlanoidSetAddKey fail", status, 0, 0);
}


/*******************************************************************************
* FUNCTION
*   wndrv_supc_pmkid_update_req_handler
*
* DESCRIPTION
*   Corresponding to:
*   OID_802_11_PMKID
*
* CALLS
*   wlanoidSetPmkid
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_pmkid_update_req_handler(ilm_struct *ilm_ptr)
{
    wndrv_supc_pmkid_update_req_struct  *in_p;
    P_PARAM_PMKID_T                           out_p;
    kal_uint32                          setBufLen;
    kal_uint32                          setInfoLen;
    kal_int32                           status, i;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_SUPC_REMOVE_KEY_REQ,
                 "SUPC_REMOVE_KEY_REQ: ***********");

    in_p = (wndrv_supc_pmkid_update_req_struct *) ilm_ptr->local_para_ptr;

    WNDRV_EXT_ASSERT(in_p->num <= WNDRV_PMKID_CACHE_NUM,
                     "wndrv_supc_pmkid_update_req_handler: in_p->num must smaller than WNDRV_PMKID_CACHE_NUM",
                     in_p->num, 0, 0);

    setBufLen = OFFSET_OF(PARAM_PMKID, BSSIDInfo) + in_p->num * sizeof(PARAM_BSSID_INFO);

    out_p = (P_PARAM_PMKID_T) WNDRV_ALLOC_BUF(setBufLen);

    kalMemSet(out_p, '0', setBufLen);

    WNDRV_EXT_ASSERT(out_p, "wndrv_supc_pmkid_update_req_handler: out_p can't get system buffer", 0, 0, 0);

    out_p->u4Length = setBufLen;
    out_p->u4BSSIDInfoCount = in_p->num;

    for (i = 0; i < in_p->num; i++) {
        kal_mem_cpy(&(out_p->arBSSIDInfo[i].arBSSID[0]), &(in_p->pmkid_param[i].bssid[0]), WNDRV_MAC_ADDRESS_LEN);
        kal_mem_cpy(&(out_p->arBSSIDInfo[i].arPMKID[0]), &(in_p->pmkid_param[i].pmkid[0]), 16);
    }

    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   //wlanoidQueryPmkid,
                                   wlanoidSetPmkid,
                                   out_p,
                                   setBufLen,
                                   (PUINT_32)&setInfoLen);


    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_pmkid_update_req_handler: wlanoidSetPmkid fail", status, 0, 0);

    WNDRV_FREE_BUF(out_p);
}

/*******************************************************************************
*
*  Global Indication Functions
*
*******************************************************************************/
/*******************************************************************************
* FUNCTION
*   wndrv_supc_network_connect_ind_handler
*
* DESCRIPTION
*   Send MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_network_connect_ind_handler(void)
{
    ilm_struct                            *ilm_p;
    wndrv_supc_network_connect_ind_struct *ind_p;
    wndrv_supc_bss_info_struct            *bss_info_p;
    BSS_INFO_T                            *currentBss_p;
    kal_int32                             i;
    P_BSS_DESC_T  prBssDesc;
    kal_uint32                            u4LinkSpeed;
    kal_uint32                            u4BufLen;

//#ifdef WIFI_BLACKLIST /*20080925 modify by saker*/
#if 0
    kal_int32     j;
    P_SCAN_INFO_T prScanInfo = &(wndrv_context.adpt_p->rScanInfo);
    P_LINK_T      prBSSDescList = &prScanInfo->rBSSDescList;
#endif

    kal_event_stop_timer(PARAM_WNDRV_ES_KEEP_CAM_TIMER);
#if (CFG_DISABLE_PS_MODE == 0)
    kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_KEEP_CAM_TIMER],
                          WNDRV_KEEP_CAM_TIMEOUT);
#endif
    nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX, Param_PowerModeCAM, FALSE);


    ind_p = (wndrv_supc_network_connect_ind_struct *)
            construct_local_para(sizeof(wndrv_supc_network_connect_ind_struct), TD_CTRL);

    WNDRV_EXT_ASSERT(ind_p, "wndrv_supc_network_connect_ind_handler: ind_p can't get local parameter", 0, 0, 0);

    /* record the session id */
    ind_p->session_id = iSessionID;

    bss_info_p   = &ind_p->bss_info;
#if 0
    if (bss_info_p->network_type == WNDRV_SUPC_NETWORK_INFRASTRUCTURE)
#endif
    {
        currentBss_p = &wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX];
        /*ad-hoc need check*/
    }
#if 0
    else if (bss_info_p->network_type == WNDRV_SUPC_NETWORK_IBSS) {
        //TODO(Nelson): Check if P2P network ?
        currentBss_p = &wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX];
    }
#endif

    kal_mem_cpy(bss_info_p->bssid, currentBss_p->aucBSSID, WNDRV_MAC_ADDRESS_LEN);

    kal_mem_set(bss_info_p->ssid, 0, WNDRV_SSID_MAX_LEN);
    bss_info_p->ssid_len = currentBss_p->ucSSIDLen;
    kal_mem_cpy(bss_info_p->ssid, currentBss_p->aucSSID,  bss_info_p->ssid_len);

#if 1   //NOTE(Nelson): instead it
    bss_info_p->rssi = wndrv_context.adpt_p->rWlanInfo.rCurrBssId.rRssi;
#else
    bss_info_p->rssi = currentBss_p->rRssi;
    bss_info_p->linkQuality = currentBss_p->rRcpi;
#endif

#if SUPPORT_802_11G
    {
        if (currentBss_p->u2OperationalRateSet & (RATE_SET_BIT_6M | RATE_SET_BIT_54M)) {
            bss_info_p->modulation_type = WNDRV_SUPC_OFDM_24;
        } else {
            bss_info_p->modulation_type = WNDRV_SUPC_DS;
        }
    }
#else
    bss_info_p->modulation_type = WNDRV_SUPC_DS;
#endif

#if 1   //NOTE(Nelson): instead it
    bss_info_p->channel_number = (kal_uint32) currentBss_p->ucPrimaryChannel;

    if (currentBss_p->eCurrentOPMode == OP_MODE_INFRASTRUCTURE) {
        bss_info_p->network_type = WNDRV_SUPC_NETWORK_INFRASTRUCTURE;
    } else {
        bss_info_p->network_type = WNDRV_SUPC_NETWORK_IBSS; //TODO(Nelson): if need to check P2P/BOW networks ?
    }
#else
    bss_info_p->channel_number = (kal_uint32) currentBss_p->ucChnl;

    if (currentBss_p->eBSSType == BSS_TYPE_INFRASTRUCTURE) {
        bss_info_p->network_type = WNDRV_SUPC_NETWORK_INFRASTRUCTURE;
    } else {
        bss_info_p->network_type = WNDRV_SUPC_NETWORK_IBSS;
    }
#endif

    for (i = RATE_1M ; i < RATE_NUM ; i ++) {
        if (currentBss_p->u2OperationalRateSet & BIT(i)) {
            bss_info_p->max_rate = aucDataRate[i];
        }
    }

    // Get the BBSSID's privacy information
    prBssDesc = scanSearchBssDescByBssid(wndrv_context.adpt_p, currentBss_p->aucBSSID);

    ASSERT(prBssDesc);

    if (prBssDesc->u2CapInfo & CAP_INFO_PRIVACY) {
        bss_info_p->privacy = 1;
    } else {
        bss_info_p->privacy = 0;
    }

    bss_info_p->wpa_ie_len = 0;
    bss_info_p->rsn_ie_len = 0;

    // The total size of Timestamp, Beacon interval, and Capability information is 12 bytes
#if 0
    if (prBssDesc->prIEWPA) {
        WNDRV_ASSERT(WNDRV_MAX_WPA_IE_LEN >= prBssDesc->prIEWPA->ucLength + 2);
        kal_mem_cpy(bss_info_p->wpa_ie, prBssDesc->prIEWPA, prBssDesc->prIEWPA->ucLength + 2);
        bss_info_p->wpa_ie_len = prBssDesc->prIEWPA->ucLength + 2;
    }
#else
    if (prBssDesc->fgIEWPA) {
        WNDRV_ASSERT(WNDRV_MAX_WPA_IE_LEN >= IE_LEN(prBssDesc->aucIEWPA) + 2);
        kal_mem_cpy(bss_info_p->wpa_ie, prBssDesc->aucIEWPA, IE_LEN(prBssDesc->aucIEWPA) + 2);
        bss_info_p->wpa_ie_len = IE_LEN(prBssDesc->aucIEWPA) + 2;
    }
#endif

    if (prBssDesc->fgIERSN) {
        WNDRV_ASSERT(WNDRV_MAX_WPA_IE_LEN >= IE_LEN(prBssDesc->aucIERSN) + 2);
        kal_mem_cpy(bss_info_p->rsn_ie, prBssDesc->aucIERSN, IE_LEN(prBssDesc->aucIERSN) + 2);
        bss_info_p->rsn_ie_len = IE_LEN(prBssDesc->aucIERSN) + 2;
    }

#ifdef __WAPI_SUPPORT__
    bss_info_p->wapi_ie_len = 0;
    if (prBssDesc->fgIEWAPI) {
        WNDRV_ASSERT(WNDRV_MAX_WAPI_IE_LEN >= prBssDesc->rIEWAPI.ucLength + 2);
        kal_mem_cpy(bss_info_p->wapi_ie, (PUINT_8)&prBssDesc->rIEWAPI, prBssDesc->rIEWAPI.ucLength + 2);
        bss_info_p->wapi_ie_len = prBssDesc->rIEWAPI.ucLength + 2;
    }
#endif /*__WAPI_SUPPORT__*/
    ind_p->assoc_ie_len = 0;

    /* Set WPA IE to SUPC for security data gen */
    if (wndrv_context.glue_p->assoc_ie_len > 0) {
        ind_p->assoc_ie_len = wndrv_context.glue_p->assoc_ie_len;
        kal_mem_cpy(ind_p->assoc_ie, wndrv_context.glue_p->assoc_ie, ind_p->assoc_ie_len);
        wndrv_context.glue_p->assoc_ie_len = 0;
        kal_mem_set(wndrv_context.glue_p->assoc_ie, 0, WNDRV_MAX_ASSOC_IE_LEN);
    }

#if 1   //TODO(Nelson): check if blacklist need ?
#ifdef WIFI_BLACKLIST // zheng.zhou add to avoid build error

    ind_p->bssid_list_num = 0;
#endif
#else
//#ifdef WIFI_BLACKLIST /*20080925 modify by saker*/
    j = 0;
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {
        if ((prBssDesc->ucSSIDLen == wndrv_context.adpt_p->rConnSettings.ucSSIDLen) &&
                !kalMemCmp(prBssDesc->aucSSID, wndrv_context.adpt_p->rConnSettings.aucSSID, prBssDesc->ucSSIDLen) &&
                prBssDesc->fgIsStatusCodePresent) {
            kalMemCopy(ind_p->bssid_list[j].bssid, prBssDesc->aucBSSID, WNDRV_MAC_ADDRESS_LEN);
            ind_p->bssid_list[j].is_reason_code_present = KAL_FALSE;
            ind_p->bssid_list[j].is_status_code_present = KAL_TRUE;
            ind_p->bssid_list[j].status_code = prBssDesc->u2StatusCode;
            j++;
        }
    }
    if (TRUE == wndrv_context.adpt_p->rBssInfo.fgIsReasonCodePresent) {
        kalMemCopy(ind_p->bssid_list[j].bssid, wndrv_context.adpt_p->rBssInfo.aucBSSID, WNDRV_MAC_ADDRESS_LEN);
        ind_p->bssid_list[j].is_reason_code_present = KAL_TRUE;
        ind_p->bssid_list[j].is_status_code_present = KAL_FALSE;
        ind_p->bssid_list[j].reason_code = wndrv_context.adpt_p->rBssInfo.u2ReasonCode;
    }
    ind_p->bssid_list_num = j;
#endif

    ilm_p                 = allocate_ilm(MOD_WNDRV);    //MOD_WNDRV
    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_IND; //MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_IND
    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_SUPC(ilm_p);

    MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_342, "[L1]wndrv_four()");
    wndrv_four();

    kalMsleep(20); //release MCU to ensure SUPC get time to deal with the MSG.

    /* Query link speed information */
    wlanQueryInformation(wndrv_context.adpt_p,
                         wlanoidQueryLinkSpeed,
                         &u4LinkSpeed,
                         sizeof(u4LinkSpeed),
                         (PUINT_32)&u4BufLen);

#if 0   //TODO(Nelson): check
    wndrv_context.glue_p->fgIsReportConnFail = FALSE;
#endif

#if 0   //TODO(Nelson): wndrv_supc_network_statistics_ind_handler timer
    kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_STAT_IND_TIMER],
                          KAL_TICKS_1_SEC * 5);
#endif

#if 0   //TODO(Nelson): power save timer handling
    // start power save contorl timer
    kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_PS_CTL_TIMER],
                          KAL_TICKS_500_MSEC);
#endif

#if 0
    for (i = 0; i < MAX_NUM_RSSI_LQ_RECORDS ; i++) {
        wndrv_context.glue_p->rssiArray[i] = -101;
    }

    wndrv_context.glue_p->rssiArray[0] = -102;
#endif

#if CFG_SINGLE_ANTENNA_ENABLED
    // BT-Coexistence
    if (wndrv_context.adpt_p->rPtaInfo.fgSingleAntenna) {
        // notify wifi connected AP
        wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFI_CONNECTION);
    } else
#endif  /* CFG_SINGLE_ANTENNA_ENABLED */
    {
        wndrv_WiFi_channel_ind_handler(KAL_TRUE, (kal_uint32) currentBss_p->ucPrimaryChannel, DEFAULT_HB_VALUE, KAL_TRUE, FALSE);
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_network_connect_fail_handler
*
* DESCRIPTION
*   Send MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_network_connect_fail_ind_handler(void)
{
    ilm_struct                                 *ilm_p;
    wndrv_supc_network_connect_fail_ind_struct *ind_p;
    kal_uint32                                 setInfoLen;
    kal_int32                                  status;

#if 0
#ifdef WIFI_BLACKLIST /*20080925 modify by saker*/
    kal_int32     j;
    P_SCAN_INFO_T prScanInfo = &(wndrv_context.adpt_p->rScanInfo);
    P_LINK_T      prBSSDescList = &prScanInfo->rBSSDescList;
    P_BSS_DESC_T  prBssDesc;
#endif
#endif

    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   wlanoidSetDisassociate,
                                   NULL,
                                   0,
                                   (PUINT_32)&setInfoLen);

    WNDRV_EXT_ASSERT(status == 0, "wndrv_supc_network_connect_fail_ind_handler: wlanoidSetDisassociate fail", status, 0, 0);

    /*ind_p = (wndrv_supc_network_connect_fail_ind_struct *)
            construct_local_para( sizeof( wndrv_supc_network_connect_fail_ind_struct ), TD_CTRL );*/
    ind_p = (wndrv_supc_network_connect_fail_ind_struct *)
            construct_local_para(sizeof(wndrv_supc_network_connect_fail_ind_struct), TD_CTRL);

    WNDRV_EXT_ASSERT(ind_p, "wndrv_supc_network_connect_fail_ind_handler: ind_p can't get local parameter", 0, 0, 0);

    ind_p->session_id = iSessionID;

#if 1   //TODO(Nelson): check if blacklist need ?
#ifdef WIFI_BLACKLIST // zheng.zhou add to avoid build error
    ind_p->bssid_list_num = 0;
#endif
#else
#ifdef WIFI_BLACKLIST /*20080925 modify by saker*/
    j = 0;
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {
        if ((prBssDesc->ucSSIDLen == wndrv_context.adpt_p->rConnSettings.ucSSIDLen) &&
                !kalMemCmp(prBssDesc->aucSSID, wndrv_context.adpt_p->rConnSettings.aucSSID, prBssDesc->ucSSIDLen) &&
                prBssDesc->fgIsStatusCodePresent) {
            kalMemCopy(ind_p->bssid_list[j].bssid, prBssDesc->aucBSSID, WNDRV_MAC_ADDRESS_LEN);
            ind_p->bssid_list[j].is_reason_code_present = KAL_FALSE;
            ind_p->bssid_list[j].is_status_code_present = KAL_TRUE;
            ind_p->bssid_list[j].status_code = prBssDesc->u2StatusCode;
            j++;
        }
    }
    if (TRUE == wndrv_context.adpt_p->rBssInfo.fgIsReasonCodePresent) {
        kalMemCopy(ind_p->bssid_list[j].bssid, wndrv_context.adpt_p->rBssInfo.aucBSSID, WNDRV_MAC_ADDRESS_LEN);
        ind_p->bssid_list[j].is_reason_code_present = KAL_TRUE;
        ind_p->bssid_list[j].is_status_code_present = KAL_FALSE;
        ind_p->bssid_list[j].reason_code = wndrv_context.adpt_p->rBssInfo.u2ReasonCode;
    }
    ind_p->bssid_list_num = j;
#endif
#endif

    ilm_p                 = allocate_ilm(MOD_WNDRV);
    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND;//MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND
    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_SUPC(ilm_p);
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_network_disconnect_ind_handler
*
* DESCRIPTION
*   Send MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_network_disconnect_ind_handler(void)
{
    ilm_struct                               *ilm_p;
    wndrv_supc_network_disconnect_ind_struct *ind_p;
    kal_int32 i;

    if (wndrv_context.glue_p->meta_mode) {
        return;
    }

#if 0   //TODO(Nelson): check if blacklist need ?
#ifdef WIFI_BLACKLIST /*20080925 modify by saker*/
    kal_int32     j;
    P_SCAN_INFO_T prScanInfo = &(wndrv_context.adpt_p->rScanInfo);
    P_LINK_T      prBSSDescList = &prScanInfo->rBSSDescList;
    P_BSS_DESC_T  prBssDesc;
#endif
#endif

#if 0
    // [MAUI_02642448] Reset OPMODE in wndrv_supc_network_disconnect_ind_handler to
    // default for avoid infrastructure profile become IBSS mode.
    if (wndrv_context.adpt_p->rConnSettings.eOPMode == NET_TYPE_IBSS) {
        kal_uint32                  setInfoLen;
        kal_int32                   status;
        ENUM_PARAM_OP_MODE_T        eOpMode;

        eOpMode = NET_TYPE_INFRA;
        status =   wlanSetInformation(wndrv_context.adpt_p,
                                      wlanoidSetInfrastructureMode,
                                      &eOpMode,
                                      sizeof(eOpMode),
                                      (PUINT_32)&setInfoLen);

        WNDRV_EXT_ASSERT(status == WLAN_STATUS_PENDING, "wndrv_supc_network_disconnect_ind_handler: wlanoidSetInfrastructureMode fail", status, 0, 0);
    }
#endif

#if 0
    if (wndrv_context.glue_p->fgIsReportDisConn == FALSE) {
        return;
    }
#endif

    /*ind_p = (wndrv_supc_network_disconnect_ind_struct *)
            construct_local_para( sizeof( wndrv_supc_network_disconnect_ind_struct ), TD_CTRL );*/
    ind_p = (wndrv_supc_network_disconnect_ind_struct *)
            construct_local_para(sizeof(wndrv_supc_network_disconnect_ind_struct), TD_CTRL);

    WNDRV_EXT_ASSERT(ind_p, "wndrv_supc_network_disconnect_ind_handler: ind_p can't get local parameter", 0, 0, 0);

    ind_p->session_id = iSessionID;

#if 1   //TODO(Nelson): check if blacklist need ?
#ifdef WIFI_BLACKLIST // zheng.zhou add to avoid build error
    ind_p->bssid_list_num = 0;
#endif
#else
#ifdef WIFI_BLACKLIST /*20080925 modify by saker*/
    j = 0;
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {
        if ((prBssDesc->ucSSIDLen == wndrv_context.adpt_p->rConnSettings.ucSSIDLen) &&
                !kalMemCmp(prBssDesc->aucSSID, wndrv_context.adpt_p->rConnSettings.aucSSID, prBssDesc->ucSSIDLen) &&
                prBssDesc->fgIsStatusCodePresent) {
            kalMemCopy(ind_p->bssid_list[j].bssid, prBssDesc->aucBSSID, WNDRV_MAC_ADDRESS_LEN);
            ind_p->bssid_list[j].is_reason_code_present = KAL_FALSE;
            ind_p->bssid_list[j].is_status_code_present = KAL_TRUE;
            ind_p->bssid_list[j].status_code = prBssDesc->u2StatusCode;
            j++;
        }
    }
    if (TRUE == wndrv_context.adpt_p->rBssInfo.fgIsReasonCodePresent) {
        kalMemCopy(ind_p->bssid_list[j].bssid, wndrv_context.adpt_p->rBssInfo.aucBSSID, WNDRV_MAC_ADDRESS_LEN);
        ind_p->bssid_list[j].is_reason_code_present = KAL_TRUE;
        ind_p->bssid_list[j].is_status_code_present = KAL_FALSE;
        ind_p->bssid_list[j].reason_code = wndrv_context.adpt_p->rBssInfo.u2ReasonCode;
    }
    ind_p->bssid_list_num = j;
#endif
#endif

    ilm_p                 = allocate_ilm(MOD_WNDRV);
    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND;//MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND
    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_SUPC(ilm_p);

    nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX,
                              (PARAM_POWER_MODE)(wndrv_context.adpt_p->u4PowerMode), FALSE);
    for (i = 0; i < PARAM_WNDRV_ES_KEEP_CAM_TIMER; i ++) {
        kal_event_stop_timer((wndrv_context.glue_p->prParamWndrvEs[i]));
    }

    // BT-Coexistence
#if CFG_SINGLE_ANTENNA_ENABLED
    if (wndrv_context.adpt_p->rPtaInfo.fgSingleAntenna) {
        // notify wifi connected AP
        wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFI_DISCONNECTION);
    } else
#endif/*CFG_SINGLE_ANTENNA_ENABLED*/
    {
        wndrv_WiFi_channel_ind_handler(KAL_TRUE, 0, 0, KAL_TRUE, 0);
    }

}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_specific_ind_handler
*
* DESCRIPTION
*   Handle WLAN_STATUS_MEDIA_SPECIFIC_INDICATION.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_specific_ind_handler(void *pvBuf)
{
    P_PARAM_STATUS_INDICATION_T pStatus = (P_PARAM_STATUS_INDICATION_T)pvBuf;

    if (pStatus) {
        switch (pStatus->eStatusType) {
            case ENUM_STATUS_TYPE_AUTHENTICATION: /* ENUM_STATUS_TYPE_AUTHENTICATION */
                status_ind_handler(pvBuf);
                break;

            case ENUM_STATUS_TYPE_CANDIDATE_LIST: /* ENUM_STATUS_TYPE_CANDIDATE_LIST */
                pmkid_candidate_ind_handler(pvBuf);
                break;

            default:
                //WNDRV_EXT_ASSERT( 0, "wndrv_supc_specific_ind_handler: Unexpected status->StatusType", pStatus->eStatusType, 0, 0 );
                break;
        }
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_site_survey_ind_handler
*
* DESCRIPTION
*   Corresponding to OID_802_11_BSSID_LIST.
*   Send MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_site_survey_ind_handler(void)
{
    ilm_struct                         *ilm_p;
    wndrv_supc_site_survey_ind_struct  *ind_p;
    kal_int32                          i;
    kal_int32                          iScanIndex, iIndIndex, iScanResultCount;
    wndrv_supc_bss_info_struct         *out_p;
    P_WLAN_INFO_T                      prWlanInfo  = &wndrv_context.adpt_p->rWlanInfo;
    P_PARAM_BSSID_EX_T                 prScanResult = (P_PARAM_BSSID_EX_T) &wndrv_context.adpt_p->rWlanInfo.arScanResult;
    P_UINT_8                           prSupportedRates;
    P_LINK_T prBSSDescList = &wndrv_context.adpt_p->rWifiVar.rScanInfo.rBSSDescList;
    P_BSS_DESC_T prBssDesc;
    P_SCAN_INFO_T prScanInfo = &(wndrv_context.adpt_p->rWifiVar.rScanInfo);
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    P_LINK_T    prscanDesiredSSIDList = &wndrv_context.adpt_p->rWifiVar.rScanInfo.rscanDesiredSSIDList;
    kal_bool fgIndConnectedAP = KAL_FALSE;
    SSID_LIST_T *ssid_list_ptr;
#endif

    if (!(wndrv_context.glue_p->fgIsReportScanResult)) {
        //MT5931_TRACE(TRACE_PEER, MT5931_INFO_,"lcko: should not indicate scan result !!");
        return ;
    }

    //iScanResultCount = wndrv_context.adpt_p->rWifiVar.rScanInfo.u4NumOfBssDesc;
    iScanResultCount = prWlanInfo->u4ScanResultNum;
    MT5931_TRACE(TRACE_PEER, MT5931_INFO_343, "iScanResultCount: %d", iScanResultCount);
    iIndIndex = 0;
    iScanIndex = 0;

    ind_p = (wndrv_supc_site_survey_ind_struct *)
            construct_local_para(sizeof(wndrv_supc_site_survey_ind_struct), TD_CTRL);
    kal_mem_set(ind_p->bss_info, 0x0, sizeof(ind_p->bss_info));
    WNDRV_EXT_ASSERT(ind_p, "wndrv_supc_site_survey_ind_handler: ind_p can't get sytem buffer", 0, 0, 0);
    ind_p->more_flag = 1;

    /* Search BSS Desc from current SCAN result list. */
    LINK_FOR_EACH_ENTRY(prBssDesc, prBSSDescList, rLinkEntry, BSS_DESC_T) {
        kal_wap_trace(MOD_WNDRV, TRACE_PEER, "prBSSDescList SSID is [%s], BSSID is["MACSTR"] ",prBssDesc->aucSSID,MAC2STR(prBssDesc->aucBSSID));
        }

        for( i = 0; i < prWlanInfo->u4ScanResultNum; i++ )
        {            
        
            if((0 == iIndIndex)&&
               (0 != iScanIndex))
            {
                ind_p = (wndrv_supc_site_survey_ind_struct *)
                        construct_local_para( sizeof( wndrv_supc_site_survey_ind_struct ), TD_CTRL );

                WNDRV_EXT_ASSERT(ind_p, "wndrv_supc_site_survey_ind_handler: ind_p can't get sytem buffer",0, 0, 0);
                ind_p->more_flag = 1;
            }

            out_p = &ind_p->bss_info[iIndIndex];

            kal_mem_cpy( out_p->bssid, &prScanResult->arMacAddress, WNDRV_MAC_ADDRESS_LEN );

            if ( prScanResult->u4Privacy )
            {
                  out_p->privacy = 1;
            }
            else
            {
                  out_p->privacy = 0;
            }
            out_p->ssid_len = prScanResult->rSsid.u4SsidLen;
            kal_mem_cpy(out_p->ssid, prScanResult->rSsid.aucSsid, out_p->ssid_len );

            out_p->rssi = prScanResult->rRssi;
            out_p->linkQuality = (kal_uint32) dBm_TO_RCPI(prScanResult->rRssi);

#if SUPPORT_802_11A
            if ( prScanResult->eNetworkTypeInUse == PARAM_NETWORK_TYPE_OFDM5 )
            {
                out_p->modulation_type = WNDRV_SUPC_OFDM_5;
            }
            else
#endif
            {
                if ( prScanResult->eNetworkTypeInUse == PARAM_NETWORK_TYPE_OFDM24 )
                {
                    out_p->modulation_type = WNDRV_SUPC_OFDM_24;
                }
                else if (  prScanResult->eNetworkTypeInUse == PARAM_NETWORK_TYPE_FH )
                {
                    out_p->modulation_type = (wndrv_supc_modulation_type_enum)PARAM_NETWORK_TYPE_FH;
                }
                else if (  prScanResult->eNetworkTypeInUse == PARAM_NETWORK_TYPE_DS )
                {
                    out_p->modulation_type = WNDRV_SUPC_DS;
                }
                else
                {
                    out_p->modulation_type = WNDRV_SUPC_AUTO_MODE;
                }
            }

            out_p->channel_number = nicFreq2ChannelNum( prScanResult->rConfiguration.u4DSConfig );

            if ( NET_TYPE_INFRA == prScanResult->eOpMode )
            {
                out_p->network_type = WNDRV_SUPC_NETWORK_INFRASTRUCTURE;
            }
            else if ( NET_TYPE_IBSS == prScanResult->eOpMode )
            {
            	  out_p->network_type = WNDRV_SUPC_NETWORK_IBSS;
            }
            else
            {
            	  out_p->network_type = WNDRV_SUPC_NETWORK_AUTO_UNKNOWN;
            }

            prSupportedRates = (P_UINT_8) &prScanResult->rSupportedRates;
            while ( *prSupportedRates != 0 )
            {
                out_p->max_rate = *prSupportedRates;
                prSupportedRates++;
            }

            out_p->wpa_ie_len = 0;
            out_p->rsn_ie_len = 0;
            
            if ( prScanResult->fgIEWPA )
            {
                WNDRV_ASSERT( WNDRV_MAX_WPA_IE_LEN >= IE_LEN(prScanResult->aucIEWPA)+2 );
                kal_mem_cpy(out_p->wpa_ie, prScanResult->aucIEWPA, IE_LEN(prScanResult->aucIEWPA)+2 );
                out_p->wpa_ie_len = IE_LEN(prScanResult->aucIEWPA)+2;
		    }

            if ( prScanResult->fgIERSN )
        	{
                  WNDRV_ASSERT( WNDRV_MAX_WPA_IE_LEN >= IE_LEN(prScanResult->aucIERSN)+2 );
                  kal_mem_cpy(out_p->rsn_ie, prScanResult->aucIERSN, IE_LEN(prScanResult->aucIERSN)+2 );
                  out_p->rsn_ie_len = IE_LEN(prScanResult->aucIERSN)+2;
		    }
#ifdef __WAPI_SUPPORT__
            out_p->wapi_ie_len = 0;

            if ( prScanResult->fgIEWAPI )
            {
                WNDRV_ASSERT( WNDRV_MAX_WAPI_IE_LEN >= IE_LEN(prScanResult->aucIEWAPI)+2 );
                kal_mem_cpy(out_p->wapi_ie, prScanResult->aucIEWAPI, IE_LEN(prScanResult->aucIEWAPI)+2 );
                out_p->wapi_ie_len = IE_LEN(prScanResult->aucIEWAPI)+2;
                MT5931_TRACE(TRACE_PEER, MT5931_INFO_344, "wndrv_supc_site_survey_ind_handler : SCAN WAPI AP! WAPI_IE_LEN:%d ,BSSID %x:%x:%x:%x:%x:%x",
                    IE_LEN(prScanResult->aucIEWAPI)+2,
                    prScanResult->arMacAddress[0],prScanResult->arMacAddress[1],prScanResult->arMacAddress[2],
                    prScanResult->arMacAddress[3],prScanResult->arMacAddress[4],prScanResult->arMacAddress[5]);
            }
#endif /*__WAPI_SUPPORT__*/
            out_p->has_wps_ie = KAL_FALSE;
#ifdef __WPS_SUPPORT__
            out_p->wps_ie_len = 0;
            if (prScanResult->wps_ie_len)
            {
                out_p->has_wps_ie = KAL_TRUE;
                kal_mem_cpy(out_p->wps_ie, prScanResult->wps_ie, prScanResult->wps_ie_len);
                out_p->wps_ie_len = prScanResult->wps_ie_len;
                MT5931_TRACE(TRACE_PEER, MT5931_INFO_345,"WPS AP BSSID %x:%x:%x:%x:%x:%x\n nettype=%d",
                    out_p->bssid[0], out_p->bssid[1],
                    out_p->bssid[2], out_p->bssid[3],
                    out_p->bssid[4], out_p->bssid[5],
                    out_p->network_type);
            }
#endif

            iIndIndex++;
            iScanIndex++;

            if (iIndIndex == WNDRV_SCAN_RESULTS_NUM)
            {
                ind_p->num = iIndIndex;
                if ( iScanIndex < iScanResultCount)
                {
                    ind_p->more_flag = 1;
                }
                else
                {
                    ind_p->more_flag = 0;
                }
                iIndIndex = 0;
                if(bScanExplicitType)
                {       	
					#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
					fgIndConnectedAP = KAL_TRUE;
					if((kalGetMediaStateIndicated(wndrv_context.adpt_p->prGlueInfo)==PARAM_MEDIA_STATE_CONNECTED)\
					&&(EQUAL_MAC_ADDR(prScanResult->arMacAddress, wndrv_context.adpt_p->rWlanInfo.rCurrBssId.arMacAddress))\
					&&(prscanDesiredSSIDList->u4NumElem))
					{
						fgIndConnectedAP = KAL_FALSE;
						LINK_FOR_EACH_ENTRY(ssid_list_ptr, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
						{
							
					
							if(EQUAL_SSID(ssid_list_ptr->ssid_list->ssid,\
											ssid_list_ptr->ssid_list->ssid_len,\
											prScanResult->rSsid.aucSsid,\
											prScanResult->rSsid.u4SsidLen))
								{
									
									MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_1," Connected ssid[%s] is in desired ssid list, need to indicate",prScanResult->rSsid.aucSsid);
									fgIndConnectedAP=KAL_TRUE;
									break;
								}						
							
						}

					}

					if(!fgIndConnectedAP)//do not indicate connected ap
					{
						free_local_para( (local_para_struct *)ind_p );

					}
					else
					#endif
					{
	                    ilm_p                 = allocate_ilm ( MOD_WNDRV );
	                    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND; //MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND
	                    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
	                    ASSERT( ilm_p->local_para_ptr->ref_count != 0 );
	                    ilm_p->peer_buff_ptr  = NULL;

	                    WNDRV_SEND_MSG_TO_SUPC( ilm_p );
                    }
                }
                else
                {
                   //free_local_para(ind_p);
                   free_local_para( (local_para_struct *)ind_p );
                   bScanExplicitType = KAL_FALSE;
                   wndrv_context.glue_p->fgIsReportScanResult = FALSE;
                   return;
                }
            }

            prScanResult++;
            
        }
        
        if (ind_p->more_flag)
        {
            ind_p->num = iIndIndex;
            ind_p->more_flag = 0;

            if(bScanExplicitType)
            {
                ilm_p                 = allocate_ilm ( MOD_WNDRV );
                ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND; //MSG_ID_WNDRV_SUPC_SITE_SURVEY_IND
                ilm_p->local_para_ptr = (local_para_struct *) ind_p;
                ASSERT( ilm_p->local_para_ptr->ref_count != 0 );
                ilm_p->peer_buff_ptr  = NULL;

                WNDRV_SEND_MSG_TO_SUPC( ilm_p );       
            }
            else
            {
                //free_local_para(ind_p);
                free_local_para( (local_para_struct *)ind_p );
            }
        }
        bScanExplicitType = KAL_FALSE;
        wndrv_context.glue_p->fgIsReportScanResult = FALSE;
		prScanInfo->rSSIDOfScanReq.u4SsidLen = 0;
		kalMemZero(prScanInfo->rSSIDOfScanReq.aucSsid,PARAM_MAX_LEN_SSID);
}

/*******************************************************************************
* FUNCTION
*   GenScanIntHandler
*
* DESCRIPTION
*   Use as a callback function for
*
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
VOID genScanIntHandler(IN P_ADAPTER_T adapter, IN WLAN_STATUS status)
{
#if 0
    if ( WLAN_STATUS_SUCCESS == status )
    {
         wndrv_supc_site_survey_ind_handler();
    }

    if(adapter->prGlueInfo->pfScanDoneHandler2)
    {
        adapter->prGlueInfo->pfScanDoneHandler2(adapter, WLAN_STATUS_SUCCESS);
        adapter->rScanInfo.rScanConfig.pfScanDoneHandler = adapter->prGlueInfo->pfScanDoneHandler2;
        adapter->prGlueInfo->pfScanDoneHandler2 = (PFN_SCAN_DONE_HANDLER) NULL;
    }
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_network_statistics_ind_handler
*
* DESCRIPTION
*   Corresponding to OID_802_11_STATISTICS.
*   Send MSG_ID_WNDRV_SUPC_NETWORK_STATISTICS_IND.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_network_statistics_ind_handler( void )
{
    if (1)
    {
        ilm_struct                               *ilm_p;
        wndrv_supc_network_statistics_ind_struct *ind_p;
        wndrv_supc_statistics_struct             *out_statistics_p;
        PARAM_STATISTICS                         *in_statistics_p;
        UINT_32 u4Data, u4Len;
        //INT_8 maxRssi, minRssi, meanRssi, varRssi;
        //UINT_32 i;
        UINT_32 u4LinkSpeed;
        UINT_32 u4BufLen;
        UINT_16 u2LinkSpeed = wndrv_context.adpt_p->rLinkQuality.u2LinkSpeed;

        //P_TX_CTRL_T prTxCtrl = &(wndrv_context.adpt_p->rTxCtrl);
        //P_RX_CTRL_T prRxCtrl = &(wndrv_context.adpt_p->rRxCtrl);
        //static UINT_32 loop = 0;

        /*ind_p = (wndrv_supc_network_statistics_ind_struct *)
                construct_local_para( sizeof( wndrv_supc_network_statistics_ind_struct ), TD_CTRL );*/
        ind_p = (wndrv_supc_network_statistics_ind_struct *)
                construct_local_para( sizeof( wndrv_supc_network_statistics_ind_struct ), TD_CTRL );

        WNDRV_EXT_ASSERT(ind_p, "wndrv_supc_network_statistics_ind_handler: ind_p can't get local parameter",0, 0, 0);

        //MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_346, "[L1]wndrv_two()");
        wndrv_two();

        /********************************************
        *  1. Query Statistics;
        ********************************************/
        in_statistics_p = (PARAM_STATISTICS*) WNDRV_ALLOC_BUF( sizeof( PARAM_STATISTICS ) );

        WNDRV_EXT_ASSERT(in_statistics_p, "wndrv_supc_network_statistics_ind_handler: in_statistics_p can't get system buffer", 0, 0, 0);
        out_statistics_p = &ind_p->statistics;

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidQueryRssi,
                           (PVOID)&u4Data,
                           sizeof(u4Data),
                           (PUINT_32)&u4Len);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        /* NOTE(lcko): Some of the below fields are left to 0 because they are not really implemented since MT5911 */
        //out_statistics_p->rssi                              = u4Data;
        out_statistics_p->rssi                              = (kal_uint32)wndrv_context.adpt_p->rLinkQuality.cRssi;
        //out_statistics_p->linkQuality                       = wndrv_context.adpt_p->rBssInfo.rRcpi;   //TODO(Nelson): update link quality
        out_statistics_p->linkQuality                       = (kal_uint32)wndrv_context.adpt_p->rLinkQuality.cLinkQuality;
        //out_statistics_p->current_data_rate                 = aucDataRate[prTxCtrl->ucCurrRateIndex];
        //out_statistics_p->current_data_rate                 = (u2LinkSpeed >= 128)?(u2LinkSpeed/2):(u2LinkSpeed);
        if ( u2LinkSpeed > 255 )
		{
    		out_statistics_p->current_data_rate             = 0;
    		out_statistics_p->ht_current_data_rate          = u2LinkSpeed;
		}
		else
		{
    		out_statistics_p->current_data_rate             = (kal_uint8) u2LinkSpeed;
    		out_statistics_p->ht_current_data_rate          = 0;
		}
        out_statistics_p->transmitted_fragment_count        = 0;
        out_statistics_p->multicast_transmitted_frame_count = 0;
        out_statistics_p->failed_count                      = 0;
        out_statistics_p->retry_count                       = 0;
        out_statistics_p->multiple_retry_count              = 0;
        out_statistics_p->RTS_success_count                 = 0;
        out_statistics_p->RTS_failure_count                 = 0;
        out_statistics_p->ACK_failure_count                 = 0;
        out_statistics_p->frame_duplicate_count             = 0;
        out_statistics_p->received_fragment_count           = 0;
        out_statistics_p->multicast_received_frame_count    = 0;
        out_statistics_p->FCS_error_count                   = 0;
        out_statistics_p->TKIP_local_MIC_failures           = 0;
        out_statistics_p->TKIP_remote_MIC_errors            = 0;
        /* NOTE(lcko): MT5921 cannot tell ICV error type */
        out_statistics_p->TKIP_ICV_errors                   = 0;
        out_statistics_p->TKIP_counter_measures_invoked     = 0;
        out_statistics_p->TKIP_replays                      = 0;
        out_statistics_p->CCMP_format_errors                = 0;
        out_statistics_p->CCMP_replays                      = 0;
        out_statistics_p->CCMP_decrypt_errors               = 0;
        out_statistics_p->four_way_handshake_failures       = 0;

        WNDRV_FREE_BUF( in_statistics_p );

        ilm_p                 = allocate_ilm ( MOD_WNDRV );
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_STATISTICS_IND;//MSG_ID_WNDRV_SUPC_NETWORK_STATISTICS_IND
        ilm_p->local_para_ptr = (local_para_struct *) ind_p;
        ilm_p->peer_buff_ptr  = NULL;
        WNDRV_SEND_MSG_TO_SUPC( ilm_p );

        /* Query link speed information */
        wlanQueryInformation(wndrv_context.adpt_p,
            wlanoidQueryLinkSpeed,
            &u4LinkSpeed,
            sizeof(u4LinkSpeed),
            &u4BufLen);

#if CFG_SINGLE_ANTENNA_ENABLED
    if (wndrv_context.adpt_p->rPtaInfo.fgSingleAntenna)
    {
        if( out_statistics_p->current_data_rate <= RATE_11M && isCCK == KAL_FALSE )
        {
            wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFI_RATE_UPDATE);
            //isCCK = KAL_TRUE;
        }
        else if( out_statistics_p->current_data_rate > RATE_11M && isCCK == KAL_TRUE )
        {
            wndrv_wmt_connecton_status_update_ind_handler(WMT_WIFI_RATE_UPDATE);
            //isCCK = KAL_FALSE;
        }
    }
#endif /*CFG_SINGLE_ANTENNA_ENABLED*/
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_mmi_poweroff_req_handler
*
* DESCRIPTION
*
*   Send MSG_ID_WNDRV_MMI_POWEROFF_RSP.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_mmi_poweroff_req_handler( void )
{
#if 0
    ilm_struct            *ilm_p;

    /* Note(lcko): Do not report disconnect otherwise SUPC will try to re-connect */
    wndrv_context.glue_p->fgIsReportDisConn = FALSE;

    wlanAdapterStop(wndrv_context.adpt_p);

    ilm_p                 = allocate_ilm ( MOD_WNDRV );
    ilm_p->msg_id         = MSG_ID_WNDRV_MMI_POWEROFF_RSP;//MSG_ID_WNDRV_MMI_POWEROFF_RSP
    ilm_p->local_para_ptr = NULL;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_MMI( ilm_p );
#endif
}

// For BT-Coexistence

/*******************************************************************************
* FUNCTION
*   wndrv_WiFi_channel_ind_handler
*
* DESCRIPTION
*
*   Send MSG_ID_BT_WIFI_SET_CHNL_REQ.
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_WiFi_channel_ind_handler (kal_bool afh, kal_uint32 freq, kal_uint8 hb, kal_bool pta, kal_uint8 pta_action)
{
    ilm_struct                   *ilm_p;
    bt_wifi_set_chnl_req_struct  *ind_p;

    if (TRUE == wndrv_context.glue_p->meta_mode)
    {
        return;
    }

    if (FALSE == BT_exist())
    {
        return;
    }
    if ((TRUE == afh) && (freq_record == freq))
    {
        afh = KAL_FALSE;
    }
    if ((TRUE == pta) && (PTA_record == pta_action))
    {
        pta = KAL_FALSE;
    }
    if ((FALSE == afh) && (FALSE == pta))
    {
        return;
    }
    /*ind_p = (bt_wifi_set_chnl_req_struct *)
            construct_local_para( sizeof( bt_wifi_set_chnl_req_struct ), TD_CTRL );*/
    ind_p = (bt_wifi_set_chnl_req_struct *)
            construct_local_para( sizeof( bt_wifi_set_chnl_req_struct ), TD_CTRL );

    WNDRV_EXT_ASSERT(ind_p, "wndrv_WiFi_channel_ind_handler: ind_p can't get local parameter", 0, 0, 0);

    ind_p->afh = afh;
    ind_p->freq = freq;
    ind_p->pta = pta;
    ind_p->pta_action = pta_action;
//#if defined(__BT_CHIP_VERSION_1__)
    if (WiFi_Query_Bluetooth_Capability())
    {
        ind_p->hb = hb; //Influenced BT half bandwidth, depend on freq is valid
    }
//#endif

    freq_record = freq;
    PTA_record = pta_action;

    ilm_p                 = allocate_ilm ( MOD_WNDRV );
    ilm_p->msg_id         = MSG_ID_BT_WIFI_SET_CHNL_REQ;//MSG_ID_BT_WIFI_SET_CHNL_REQ
    ilm_p->local_para_ptr = (local_para_struct *) ind_p;
    ilm_p->peer_buff_ptr  = NULL;

    WNDRV_SEND_MSG_TO_BT( ilm_p );
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_auth_state_update_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_auth_state_update_req_handler( ilm_struct *ilm_ptr )
{
    wndrv_supc_auth_state_update_req_struct  *local_p = (wndrv_supc_auth_state_update_req_struct *) ilm_ptr->local_para_ptr;

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_347, "[L1]wndrv_three(): progress:%d", local_p->in_progress);
    wndrv_three(local_p->in_progress);
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_blacklist_update_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_blacklist_update_req_handler( ilm_struct *ilm_ptr )
{
#if 0

#ifdef WIFI_BLACKLIST /*20080924 modify by saker*/
    wndrv_supc_blacklist_update_req_struct  *local_p = (wndrv_supc_blacklist_update_req_struct *) ilm_ptr->local_para_ptr;

    WNDRV_EXT_ASSERT(local_p->blacklist_num <= WNDRV_BSSID_MAX_NUM, "wndrv_supc_join_network_req_hanlder: the size of join_p->blacklist_num is wrong", 0, 0, 0);
    wndrv_context.adpt_p->u4BlackListCount= local_p->blacklist_num;
    kalMemCopy( wndrv_context.adpt_p->auBlackList, local_p->blacklist, local_p->blacklist_num * sizeof( PARAM_MAC_ADDRESS ) );
#endif

#endif
}
/*******************************************************************************
* FUNCTION
*   wndrv_supc_chip_poweron_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_chip_poweron_req_handler( ilm_struct *ilm_ptr )
{
    /* Do nothing */
}

/*******************************************************************************
* FUNCTION
*   wndrv_supc_chip_poweroff_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_supc_chip_poweroff_req_handler( ilm_struct *ilm_ptr )
{
    /* Do nothing */
}

/*******************************************************************************
* FUNCTION
*   wndrv_ctia_mode_enable_handler
*
* DESCRIPTION
*   Corresponding to:
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_ctia_mode_enable_handler()
{
    PARAM_CUSTOM_SW_CTRL_STRUC_T rSwCtrlInfo;
    UINT_32 u4SetInfoLen;

    /* Enable ACK_WHEN_FIFO_FULL & Disable ONLINE_SCAN */
    rSwCtrlInfo.u4Id = 0xFFFF1234;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );

#if 0   /* NOTE(Nelson): 2011/09/15: has set in previous sw control, not need to set again */
    /* Disable roaming */
    rSwCtrlInfo.u4Id = 0x9000 << 16;
    rSwCtrlInfo.u4Id |= 0x0204;
    rSwCtrlInfo.u4Data = 0;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );
    rSwCtrlInfo.u4Id = 0x9000 << 16;
    rSwCtrlInfo.u4Id |= 0x0200;
    rSwCtrlInfo.u4Data = 0x00820000;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );

    /* Disable Beacon lost detection */
    rSwCtrlInfo.u4Id = 0x1001 << 16;
    rSwCtrlInfo.u4Id |= 0x0001;
    rSwCtrlInfo.u4Data = 1;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_ctia_mode_disable_handler
*
* DESCRIPTION
*   Corresponding to:
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_ctia_mode_disable_handler()
{
    PARAM_CUSTOM_SW_CTRL_STRUC_T rSwCtrlInfo;
    UINT_32 u4SetInfoLen;
    
    /* Disable ACK_WHEN_FIFO_FULL & Disable ONLINE_SCAN */
    rSwCtrlInfo.u4Id = 0xFFFF1235;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );

#if 0   /* NOTE(Nelson): 2011/09/15: has set in previous sw control, not need to set again */
    /* Enable roaming */
    rSwCtrlInfo.u4Id = 0x9000 << 16;
    rSwCtrlInfo.u4Id |= 0x0204;
    rSwCtrlInfo.u4Data = 1;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );
    rSwCtrlInfo.u4Id = 0x9000 << 16;
    rSwCtrlInfo.u4Id |= 0x0200;
    rSwCtrlInfo.u4Data = 0x00820000;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );

    /* Enable Beacon lost detection */
    rSwCtrlInfo.u4Id = 0x1001 << 16;
    rSwCtrlInfo.u4Id |= 0x0001;
    rSwCtrlInfo.u4Data = 0;
    wlanoidSetSwCtrlWrite( wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen );
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_mmi_hw_ctl_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_abm_set_ps_mode_req_handler (ilm_struct *ilm_ptr)
{
     PARAM_POWER_MODE pm = Param_PowerModeMAX_PSP;
     wndrv_ps_mode_enum PowerMode, setInfoLen;
     wndrv_abm_set_ps_mode_req_struct *req_ptr;
     wndrv_abm_set_ps_mode_cnf_struct *cnf_ptr;
     ilm_struct *ilm_p;    

     req_ptr = (wndrv_abm_set_ps_mode_req_struct *)ilm_ptr->local_para_ptr;

     cnf_ptr = (wndrv_abm_set_ps_mode_cnf_struct *)
              construct_local_para(sizeof(wndrv_abm_set_ps_mode_cnf_struct), TD_CTRL);

     PowerMode = req_ptr->ps_mode;

     switch(PowerMode)
     {
         case WNDRV_POWER_MODE_CAM:
		 	wifi_under_cam_mode = 1;
             wndrv_ctia_mode_disable_handler();
             pm = Param_PowerModeCAM;
             break;

         case WNDRV_POWER_MODE_MAX_PS:
		 	wifi_under_cam_mode = 0;
             wndrv_ctia_mode_disable_handler();
             pm = Param_PowerModeMAX_PSP;
             break;

         case WNDRV_POWER_MODE_FAST_PS:
		 	wifi_under_cam_mode = 0;
             wndrv_ctia_mode_disable_handler();
             pm = Param_PowerModeFast_PSP;
             break;

         case WNDRV_POWER_MODE_CTIA:
             //RFTool_WiFi_PwrMgt( WNDRV_POWER_MODE_CONTINUOUS_ACTIVE );
             wifi_under_cam_mode = 0;
             wndrv_ctia_mode_enable_handler();
             pm = Param_PowerModeCAM;
             break;

         default:
		 	wifi_under_cam_mode = 0;
             pm = Param_PowerModeFast_PSP;
             break;
     }

     /* Save configured PS mode in adapter */
     wndrv_context.adpt_p->u4PowerMode = pm;

     MT5931_TRACE(TRACE_INFO, MT5931_INFO_348, "[Service API] Set Power Mode : %d", PowerMode);
     wlanSetInformation(wndrv_context.adpt_p,
                    (PFN_OID_HANDLER_FUNC)wlanoidSet802dot11PowerSaveProfile,
                    &pm,
                    sizeof(PARAM_POWER_MODE),
                    (PUINT_32)&setInfoLen );

     cnf_ptr->status = KAL_TRUE;
     cnf_ptr->ps_mode = PowerMode;

     /* send message*/
     ilm_p = allocate_ilm (MOD_WNDRV);
     ilm_p->src_mod_id = MOD_WNDRV;
     ilm_p->dest_mod_id = MOD_ABM;
     ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_PS_MODE_CNF;
     ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
     ilm_p->peer_buff_ptr = NULL;

     msg_send_ext_queue(ilm_p);
}

#if CFG_DISABLE_PS_MODE
void wndrv_abm_pseudo_set_ps_mode(void)
{
	PARAM_POWER_MODE pm = Param_PowerModeMAX_PSP;
	UINT_32 setInfoLen;
	
	//refer to : wndrv_abm_set_ps_mode_req_handler 
	//case WNDRV_POWER_MODE_CAM:
	wifi_under_cam_mode = 1;
	wndrv_ctia_mode_disable_handler();
	pm = Param_PowerModeCAM;
	
	/* Save configured PS mode in adapter */
	wndrv_context.adpt_p->u4PowerMode = pm;
	
	MT5931_TRACE(TRACE_INFO, MT5931_INFO_348, "[Service API] Set Power Mode : %d", wndrv_context.adpt_p->u4PowerMode);
	wlanSetInformation(wndrv_context.adpt_p,
				   (PFN_OID_HANDLER_FUNC)wlanoidSet802dot11PowerSaveProfile,
				   &pm,
				   sizeof(PARAM_POWER_MODE),
				   (PUINT_32)&setInfoLen );

}
#endif

/*******************************************************************************
* FUNCTION
*   wndrv_abm_set_scan_desired_ssid_list_req_handler
*
* DESCRIPTION
*   PTA timing setting
*
*
* CALLS
*
* PARAMETERS
*
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/

#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
void wndrv_abm_set_scan_desired_ssid_list_req_handler (ilm_struct *ilm_ptr)
{
	wndrv_abm_set_scan_desired_ssid_list_req_struct *req_ptr;
	
    SSID_LIST_T *ssid_list_ptr;
    kal_uint8	i=0;
	P_LINK_T	prscanDesiredSSIDList;
	kal_bool	fgignore=KAL_FALSE;
	SSID_LIST_T *ssid_list_ptr_next;
	wndrv_abm_set_scan_desired_ssid_list_cnf_struct *cnf_ptr; 
	ilm_struct *ilm_p;	  

	
	cnf_ptr = (wndrv_abm_set_scan_desired_ssid_list_cnf_struct *)
			   construct_local_para(sizeof(wndrv_abm_set_scan_desired_ssid_list_cnf_struct), TD_CTRL);

	
	prscanDesiredSSIDList = &(wndrv_context.adpt_p->rWifiVar.rScanInfo.rscanDesiredSSIDList);
	req_ptr = (wndrv_abm_set_scan_desired_ssid_list_req_struct *)ilm_ptr->local_para_ptr;
	
	DEBUGFUNC("wndrv_abm_set_scan_desired_ssid_list_req_handler");
	MT5931_WAP_TRACE(MOD_WNDRV,TRACE_GROUP_5,"prscanDesiredSSIDList->u4NumElem=%d",prscanDesiredSSIDList->u4NumElem);

	cnf_ptr->status = KAL_TRUE;
	
	// 1. clear driver cached ssid list
	if(prscanDesiredSSIDList->u4NumElem)
	{	
		
        LINK_FOR_EACH_ENTRY_SAFE(ssid_list_ptr, ssid_list_ptr_next, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
        {
        	
			MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR," free ssid is:[%s] ",ssid_list_ptr->ssid_list->ssid);
			
			LINK_REMOVE_KNOWN_ENTRY (prscanDesiredSSIDList, ssid_list_ptr);
			kalMemFree(ssid_list_ptr->ssid_list,VIR_MEM_TYPE,sizeof(abm_wndrv_ssid_struct));
			kalMemFree(ssid_list_ptr,VIR_MEM_TYPE,sizeof(SSID_LIST_T));
			
        }
        
		LINK_INITIALIZE(prscanDesiredSSIDList);
       
	}

	// 2. copy ssid_list to driver cached ssid list
	if(req_ptr->ssid_list_num!=0)
	{
		for(i=0 ; i < req_ptr->ssid_list_num ; i++)
		{	

			fgignore = KAL_FALSE;
			if(req_ptr->ssid_list[i].ssid_len == 0)
			{
				MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"ssid len==%d,ignore this ssid",req_ptr->ssid_list[i].ssid_len);
				continue;
			}
			// check duplicated ssid 
			if(prscanDesiredSSIDList->u4NumElem)
			{	
				
		        LINK_FOR_EACH_ENTRY_SAFE(ssid_list_ptr, ssid_list_ptr_next, prscanDesiredSSIDList, rLinkEntry, SSID_LIST_T) 
		        {
		        	if(EQUAL_SSID(ssid_list_ptr->ssid_list->ssid, \
                            ssid_list_ptr->ssid_list->ssid_len,\
                            req_ptr->ssid_list[i].ssid,\
                            req_ptr->ssid_list[i].ssid_len))
                            {
                            	fgignore = KAL_TRUE;
								MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"ssid len:%d,ssid:[%s] alread in desired list,ignore this ssid ",ssid_list_ptr->ssid_list->ssid_len,ssid_list_ptr->ssid_list->ssid);
								break;
                            }
		        }
			}
			if(fgignore)
			{
				continue;
			}
			
			//1. alloc SSID_LIST_T mem
			ssid_list_ptr = NULL;
			ssid_list_ptr=(P_SSID_LIST_T)kalMemAlloc(sizeof(SSID_LIST_T),VIR_MEM_TYPE);
			ASSERT(ssid_list_ptr);
			memset( ssid_list_ptr, 0, sizeof(SSID_LIST_T) );	

			//2. alloc ssid mem
			ssid_list_ptr->ssid_list=(abm_wndrv_ssid_struct*)kalMemAlloc(sizeof(abm_wndrv_ssid_struct),VIR_MEM_TYPE);
			ASSERT(ssid_list_ptr->ssid_list);
			memset( ssid_list_ptr->ssid_list, 0, sizeof(abm_wndrv_ssid_struct) );	
			
			//3. copy ssid info
			ssid_list_ptr->ssid_list->ssid_len = req_ptr->ssid_list[i].ssid_len;
			kalMemCopy(ssid_list_ptr->ssid_list->ssid, req_ptr->ssid_list[i].ssid, req_ptr->ssid_list[i].ssid_len);

			//4. store SSID_LIST_T
			LINK_INSERT_TAIL(prscanDesiredSSIDList,&ssid_list_ptr->rLinkEntry);
			
			MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR," add %d/%d, ssid len:%d , ssid:[%s] ",i+1,req_ptr->ssid_list_num,ssid_list_ptr->ssid_list->ssid_len,ssid_list_ptr->ssid_list->ssid);
			
		}
	}

	// 3. send cnf msg to abm
     ilm_p = allocate_ilm (MOD_WNDRV);
     ilm_p->src_mod_id = MOD_WNDRV;
     ilm_p->dest_mod_id = MOD_ABM;
     ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_SCAN_DESIRED_SSID_LIST_CNF;
     ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
     ilm_p->peer_buff_ptr = NULL;

	 msg_send_ext_queue(ilm_p);

	
}
#endif

/*******************************************************************************
* FUNCTION
*   wndrv_abm_set_each_channel_scan_time_req_handler
*
* DESCRIPTION
*  set each channel dwell time in TU
*
*
* CALLS
*
* PARAMETERS
*
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_abm_set_each_channel_scan_time_req_handler (ilm_struct *ilm_ptr)
{
	wndrv_abm_set_scan_dwell_time_req_struct *req_ptr;
	wndrv_abm_set_scan_dwell_time_cnf_struct *cnf_ptr; 
	ilm_struct *ilm_p;	  

	//1. alloc buffer
	cnf_ptr = (wndrv_abm_set_scan_dwell_time_cnf_struct *)
			   construct_local_para(sizeof(wndrv_abm_set_scan_dwell_time_cnf_struct), TD_CTRL);

	
	req_ptr = (wndrv_abm_set_scan_dwell_time_req_struct *)ilm_ptr->local_para_ptr;
	
	cnf_ptr->status = KAL_TRUE;

	// store para
	if(req_ptr->u2ChannelDwellTime)//driver use value in MSG
	{
		wndrv_context.adpt_p->rWifiVar.rScanInfo.u2ChannelDwellTime = req_ptr->u2ChannelDwellTime;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_INFO,"Set each channel scan dwell time = %d",wndrv_context.adpt_p->rWifiVar.rScanInfo.u2ChannelDwellTime);
	}
	else // driver use default value
	{
		wndrv_context.adpt_p->rWifiVar.rScanInfo.u2ChannelDwellTime = 0;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_INFO,"Set each channel scan dwell time, driver use default time");
	}

	// 3. send cnf msg to abm
     ilm_p = allocate_ilm (MOD_WNDRV);
     ilm_p->src_mod_id = MOD_WNDRV;
     ilm_p->dest_mod_id = MOD_ABM;
     ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_SCAN_DWELL_TIME_CNF;
     ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
     ilm_p->peer_buff_ptr = NULL;

	 msg_send_ext_queue(ilm_p);
}

/*******************************************************************************
* FUNCTION
*   wndrv_abm_set_roaming_para_req_handler
*
* DESCRIPTION
*  set each channel dwell time in TU
*
*
* CALLS
*
* PARAMETERS
*
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_abm_set_roaming_para_req_handler (ilm_struct *ilm_ptr)
{
	wndrv_abm_set_roaming_para_req_struct *req_ptr;
	wndrv_abm_set_roaming_para_cnf_struct *cnf_ptr; 
	ilm_struct *ilm_p;	  

	//1. alloc buffer
	cnf_ptr = (wndrv_abm_set_roaming_para_cnf_struct *)
			   construct_local_para(sizeof(wndrv_abm_set_roaming_para_cnf_struct), TD_CTRL);

	req_ptr = (wndrv_abm_set_roaming_para_req_struct *)ilm_ptr->local_para_ptr;
	
	cnf_ptr->status = KAL_TRUE;

	// store para
	if(req_ptr->fgEnableRoaming)//Enable roaming
	{
		wndrv_context.adpt_p->rWifiVar.rRoamingInfo.fgBriefEnableRoamingDynamically = KAL_TRUE;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_INFO,"Enable Roaming");
	}
	else // disable roaming
	{
		wndrv_context.adpt_p->rWifiVar.rRoamingInfo.fgBriefEnableRoamingDynamically = KAL_FALSE;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_INFO,"Disable Roaming");
	}

	// 3. send cnf msg to abm
	ilm_p = allocate_ilm (MOD_WNDRV);
	ilm_p->src_mod_id = MOD_WNDRV;
	ilm_p->dest_mod_id = MOD_ABM;
	ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_ROAMING_PARA_CNF;
	ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
	ilm_p->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_p);
}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_set_pta_timing_handler
*
* DESCRIPTION
*   PTA timing setting
*
*
* CALLS
*
* PARAMETERS
*   pta_t: PTA T0/T6/T7/T8/BT_DELAY
*   interval: PTA interval
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_wmt_set_pta_timing_handler( UINT_32 pta_t, UINT_32 interval  )
{
    UINT_32                        u4Len;
    PTA_IPC_T                      PtaIpc;
    UINT_32                        fgWiFiCoexParam;

    DEBUGFUNC("wndrv_wmt_set_pta_timing_handler");

    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);

    fgWiFiCoexParam = ( VAR_WMT_CONFIG_WIFI_1WIRE_MODE | pta_t | interval );

    PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
    PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
    PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
    PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    wlanSetInformation(wndrv_context.adpt_p,
                   (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                   (PVOID)&PtaIpc,
                   sizeof(PtaIpc),
                   (PUINT_32)&u4Len);
    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_set_bwcs_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_wmt_set_bwcs_req_handler( ilm_struct *ilm_ptr )
{
    wmt_wndrv_set_bwcs_req_struct *ind_p;
    UINT_32                        u4Len;
    //UINT_32                        u4PtaCtrl;
    PTA_IPC_T                      PtaIpc;
    PARAM_POWER_MODE               pm;
    //UINT_32                        u4PollInt;
    //UINT_32                        status;
    UINT_32                        fgWiFiCoexParam = 0;

    DEBUGFUNC("[BWCS]wndrv_wmt_set_bwcs_req_handler");

    ind_p = (wmt_wndrv_set_bwcs_req_struct *)ilm_ptr->local_para_ptr;

#ifdef __WIFI_BT_SINGLE_ANTENNA_DESIGN_TDD__
    if (ind_p->ePtaMode == BWCS_1_WIRE_MODE) //Mode 7
#endif
    {
    //4 <0> set 1-wire mode to mode7
    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);

    fgWiFiCoexParam = (VAR_WMT_CONFIG_WIFI_1WIRE_MODE | VAR_1WIRE_MODE | BT_EXT_1_WIRE_MODE);

    PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
    PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
    PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
    PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    wlanSetInformation(wndrv_context.adpt_p,
                   (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                   (PVOID)&PtaIpc,
                   sizeof(PtaIpc),
                   (PUINT_32)&u4Len);
    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    //4 <0> set disable wifi RX PE while BT TX/RX for mode7
    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);

    fgWiFiCoexParam = (VAR_WMT_CONFIG_ARB_MODE | VAR_BTRX_WFRX_OK | VAR_BTTX_WFRX_OK);

    PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
    PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
    PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
    PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    wlanSetInformation(wndrv_context.adpt_p,
                   (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                   (PVOID)&PtaIpc,
                   sizeof(PtaIpc),
                   (PUINT_32)&u4Len);
    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    //4 <0> 1-wire mode PTA timing setting
    wndrv_wmt_set_pta_timing_handler( VAR_1WIRE_MODE_T6, wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T6] );
    wndrv_wmt_set_pta_timing_handler( VAR_1WIRE_MODE_T8, wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T8] );
    wndrv_wmt_set_pta_timing_handler( VAR_1WIRE_MODE_BT_DELAY, wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_BT_DELAY] );

    //4 <1> set PTA
    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);
    
    if( ind_p->eWiFiPta == WIFI_PTA_ENABLE ||  ind_p->eWiFiPta == WIFI_PTA_DISABLE )
    {
        if( ind_p->eWiFiPta == WIFI_PTA_ENABLE )
        {
            fgWiFiCoexParam = ( VAR_WMT_CONFIG_COEX_SETTING | WMT_COEX_CONFIG_BT_ON | WMT_COEX_CONFIG_ENABLE_PTA ); // WIFI_PTA_ENABLE
        }
        else
        {
            fgWiFiCoexParam = VAR_WMT_CONFIG_COEX_SETTING;  // WIFI_PTA_DISABLE

        }
        //20090924 select PTA porfile usage is single/dual
        if( !ind_p->fgUseSingleAntSet )
        {
            fgWiFiCoexParam |= WMT_COEX_CONFIG_DUAL_ANT_MODE;
        }

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
        PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
        PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
        PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

        wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                       (PVOID)&PtaIpc,
                       sizeof(PtaIpc),
                       (PUINT_32)&u4Len);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    }

    //4 <2>  set BT profile use wlanoidSetBT
    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);
    PtaIpc.u.rProfile.eBtProfile = (ENUM_BT_PROFILE_T)(ind_p->eBwcsProfile);
    wndrv_context.adpt_p->rPtaInfo.rBtProfile.eBtProfile = (ENUM_BT_PROFILE_T)(ind_p->eBwcsProfile);
    
    if( BWCS_PROFILE_SCO == ind_p->eBwcsProfile )
    {
        fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_SCO );        
    }
    else if( BWCS_PROFILE_ACL == ind_p->eBwcsProfile )
    {
        fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_A2DP );
    }
    else if( BWCS_PROFILE_MIXED == ind_p->eBwcsProfile )    /* for SCO */
    {
        fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_SCO );
    }
    else if( BWCS_PROFILE_NOCONN == ind_p->eBwcsProfile )    /* disconnect */
    {
        fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_NONE );
    }
    else    /* other profiles */
    {
        fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED );
    }

    PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
    PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
    PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
    PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                       (PVOID)&PtaIpc,
                       sizeof(PtaIpc),
                       (PUINT_32)&u4Len);

    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    //4 <3> set WiFi Power saving profile use wlanoidSet802dot11PowerSaveProfile
    switch( ind_p->eWiFiPsProfile )
    {
        case WIFI_PSP_CONTINUOUS_ACTIVE :
            pm = Param_PowerModeCAM;
            break;

        case WIFI_PSP_CONTINUOUS_POWER_SAVE :
            pm = Param_PowerModeMAX_PSP;
            break;

        case WIFI_PSP_FAST_SWITCH_TIME :
            pm = Param_PowerModeFast_PSP;
            break;

        case WIFI_PSP_FAST_SWITCH_DIRECT :
        case WIFI_PSP_DEFAULT :
            pm = Param_PowerModeFast_PSP;
            break;
    }

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    wlanSetInformation(wndrv_context.adpt_p,
                   (PFN_OID_HANDLER_FUNC)wlanoidSet802dot11PowerSaveProfile,
                   &pm,
                   sizeof(PARAM_POWER_MODE),
                   (PUINT_32)&u4Len );

    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    }
#ifdef __WIFI_BT_SINGLE_ANTENNA_DESIGN_TDD__
    else if (ind_p->ePtaMode == BWCS_1_WIRE_EXTEND_MODE) //Mode 8
    {
        if ( WIFI_PTA_ENABLE == ind_p->eWiFiPta )
        {
            //4 <1>set PTA mode VAR_WMT_CONFIG_WIFI_1WIRE_MODE
            PtaIpc.ucCmd = BT_CMD_PROFILE;
            PtaIpc.ucLen = sizeof(PTA_IPC_T);

            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_349, 
                "[BWCS]set PTA mode 8: VAR_WMT_CONFIG_WIFI_1WIRE_MODE | VAR_1WIRE_MODE | BT_EXT_1_WIRE_EXTEND_MODE");
            fgWiFiCoexParam = (VAR_WMT_CONFIG_WIFI_1WIRE_MODE | VAR_1WIRE_MODE | BT_EXT_1_WIRE_EXTEND_MODE);

            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
            wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);
            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

            //4 <2> 1-wire mode PTA timing setting
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_350, 
                "[BWCS]set PTA timing: VAR_WMT_CONFIG_WIFI_1WIRE_MODE | PARAM_CUST_SET_BT_1WIRE_MODE_T6 | 0x%x", 
                wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T6]);
            wndrv_wmt_set_pta_timing_handler( VAR_1WIRE_MODE_T6, 
                    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T6] );
            
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_351, 
                "[BWCS]set PTA timing: VAR_WMT_CONFIG_WIFI_1WIRE_MODE | PARAM_CUST_SET_BT_1WIRE_MODE_T8 | 0x%x", 
                wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T8]);
            wndrv_wmt_set_pta_timing_handler( VAR_1WIRE_MODE_T8, 
                    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T8] );
            
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_352, 
                "[BWCS]set PTA timing: VAR_WMT_CONFIG_WIFI_1WIRE_MODE | PARAM_CUST_SET_BT_1WIRE_MODE_BT_DELAY | 0x%x", 
                wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_BT_DELAY]);
            wndrv_wmt_set_pta_timing_handler( VAR_1WIRE_MODE_BT_DELAY, 
                    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_BT_DELAY] );

            //4 <3> set UCM/SCM VAR_WIFI_COEX_MODE_REQ
            if (ind_p->eCmMode == BWCS_UCM_mode) {
                MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_353, 
                    "[BWCS]set UCM/SCM: VAR_WIFI_COEX_MODE_REQ | VAR_WIFI_COEX_MODE_UC");
                fgWiFiCoexParam = VAR_WIFI_COEX_MODE_REQ | VAR_WIFI_COEX_MODE_UC;
            }
            else if (ind_p->eCmMode == BWCS_SCM_mode) {
                MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_354, 
                    "[BWCS]set UCM/SCM: VAR_WIFI_COEX_MODE_REQ | VAR_WIFI_COEX_MODE_SC");
                fgWiFiCoexParam = VAR_WIFI_COEX_MODE_REQ | VAR_WIFI_COEX_MODE_SC;
            }

            PtaIpc.ucCmd = BT_CMD_PROFILE;
            PtaIpc.ucLen = sizeof(PTA_IPC_T);

            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
            wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);
            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
        }

        //4 <4>  set BT profile use wlanoidSetBT VAR_WMT_REPORT_BT_PROFILE
        PtaIpc.ucCmd = BT_CMD_PROFILE;
        PtaIpc.ucLen = sizeof(PTA_IPC_T);

        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_355, "(1)[BWCS]ind_p->ucBt_tsco=%d", ind_p->ucBt_tsco); /* Nelson UT */

        if( BWCS_PROFILE_SCO == ind_p->eBwcsProfile )
        {
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_356, "(2)[BWCS]ind_p->ucBt_tsco=%d", ind_p->ucBt_tsco);
            if( ind_p->ucBt_tsco > 6 )
            {
            
                MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_357, 
                    "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_ESCO | VAR_BT_PROF_SCO");
                fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_ESCO | VAR_BT_PROF_SCO );
            }
            else
            {
                MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_358, 
                    "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_SCO");
                fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_SCO );
            }
        }
        else if( BWCS_PROFILE_ACL == ind_p->eBwcsProfile )
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_359, 
                "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED");
            fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED);
        }
        else if( BWCS_PROFILE_A2DP == ind_p->eBwcsProfile )
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_360, 
                "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_A2DP");
            fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_A2DP);
        }
        else if( BWCS_PROFILE_MIXED == ind_p->eBwcsProfile )    /* for SCO */
        {
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_361, "(2)[BWCS]ind_p->ucBt_tsco=%d", ind_p->ucBt_tsco);
            if( ind_p->ucBt_tsco > 6 )
            {
            
                MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_362, 
                    "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_ESCO | VAR_BT_PROF_SCO");
                fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_ESCO | VAR_BT_PROF_SCO );
            }
            else
            {
                MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_363, 
                    "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_SCO");
                fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_LINK_CONNECTED | VAR_BT_PROF_SCO );
            }
        }
        else
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_364, 
                "[BWCS]set BT profile: VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_NONE");
            fgWiFiCoexParam = ( VAR_WMT_REPORT_BT_PROFILE | VAR_BT_PROF_NONE );
        }

        PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
        PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
        PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
        PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        //4 <5>set PTA, antenna, RW VAR_WMT_CONFIG_COEX_SETTING
        PtaIpc.ucCmd = BT_CMD_PROFILE;
        PtaIpc.ucLen = sizeof(PTA_IPC_T);

        MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_365, 
                        "[BWCS]set COEX setting: VAR_WMT_CONFIG_COEX_SETTING");
        fgWiFiCoexParam = VAR_WMT_CONFIG_COEX_SETTING;

        /*eWiFiPta*/
        if (ind_p->eWiFiPta == WIFI_PTA_ENABLE)
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_366, 
                        "[BWCS]| WMT_COEX_CONFIG_ENABLE_PTA");
            fgWiFiCoexParam |= WMT_COEX_CONFIG_ENABLE_PTA;
        }        

        /* fgUseSingleAntSet */
        if ( !ind_p->fgUseSingleAntSet )
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_367, 
                        "[BWCS]| WMT_COEX_CONFIG_DUAL_ANT_MODE");
            fgWiFiCoexParam |= WMT_COEX_CONFIG_DUAL_ANT_MODE;
        }

        /* remaining window */
        if ( ind_p->fgUseRW )
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_368, 
                        "[BWCS]| WMT_COEX_CONFIG_ENABLE_RW");
            fgWiFiCoexParam |= WMT_COEX_CONFIG_ENABLE_RW;
        }

        /*eBwcsProfile*/
        //if ( BWCS_PROFILE_NONE == ind_p->eBwcsProfile )
        if ( WIFI_PTA_DISABLE == ind_p->eWiFiPta )
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_369, 
                        "[BWCS]VAR_WMT_CONFIG_COEX_SETTING & (~WMT_COEX_CONFIG_BT_ON)");
            fgWiFiCoexParam = VAR_WMT_CONFIG_COEX_SETTING & (~WMT_COEX_CONFIG_BT_ON);
        }
        else
        {
            MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_370, 
                        "[BWCS]| WMT_COEX_CONFIG_BT_ON |  WMT_COEX_CONFIG_WIFI_ON");
            fgWiFiCoexParam |= (WMT_COEX_CONFIG_BT_ON |  WMT_COEX_CONFIG_WIFI_ON);
        }

        PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
        PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
        PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
        PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);


    }
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_query_rssi_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_wmt_query_rssi_req_handler( ilm_struct *ilm_ptr )
{
    UINT_32                        u4Data, u4Len;
    ilm_struct                     *ilm_p;
    wmt_wifi_query_rssi_ind_struct *out_p;
    PARAM_RSSI                      rRssi;

    DEBUGFUNC("wndrv_wmt_query_rssi_req_handler");

    out_p = (wmt_wifi_query_rssi_ind_struct *)
            construct_local_para( sizeof( wmt_wifi_query_rssi_ind_struct ), TD_CTRL );

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    {
        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidQueryRssi,
                           (PVOID)&u4Data,
                           sizeof(u4Data),
                           (PUINT_32)&u4Len);
    }
    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

#if 1
    rRssi = (PARAM_RSSI)wndrv_context.adpt_p->rLinkQuality.cRssi; // ranged from (-128 ~ 30) in unit of dBm
    MT5931_NEW_TRACE( MT5931_INFO_564, TRACE_GROUP_10, "WLAN RSSI = %d", rRssi );
    
    if(rRssi > PARAM_WHQL_RSSI_MAX_DBM){
        rRssi = PARAM_WHQL_RSSI_MAX_DBM;
    } else if(rRssi < PARAM_WHQL_RSSI_MIN_DBM){
        rRssi = PARAM_WHQL_RSSI_MIN_DBM;
    }
    
    out_p->wifi_rssi = rRssi;
#else
    out_p->wifi_rssi = u4Data;    
#endif

    ilm_p                 = allocate_ilm( MOD_WNDRV );
    ilm_p->src_mod_id     = MOD_WNDRV;
    ilm_p->dest_mod_id    = MOD_BWCS;
    ilm_p->msg_id         = MSG_ID_WNDRV_WMT_QUERY_RSSI_IND;//MSG_ID_WNDRV_WMT_QUERY_RSSI_IND
    ilm_p->local_para_ptr = (local_para_struct *) out_p;
    ilm_p->peer_buff_ptr  = NULL;

    msg_send_ext_queue(ilm_p);

}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_set_ant_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_wmt_set_ant_req_handler( ilm_struct *ilm_ptr )
{
    wmt_wndrv_set_ant_req_struct *ind_p;
    UINT_32                      u4Value, u4Len;
    //PTA_IPC_T                    PtaIpc;
    //UINT_32                      fg1WireMode;

    DEBUGFUNC("wndrv_wmt_set_ant_req_handler");

    ind_p = (wmt_wndrv_set_ant_req_struct *)ilm_ptr->local_para_ptr;
    
    if( ind_p->fgAntForBT == TRUE )
    {
        // ant switch to BT
        u4Value = 1;        
    }
    else
    {
        // ant switch to wifi
        u4Value = 0;
    }

    //4 <2> set ant
    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetPreferredAnt,
                       (PVOID)&u4Value,
                       sizeof(u4Value),
                       (PUINT_32)&u4Len);

    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_connecton_status_update_ind_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_wmt_connecton_status_update_ind_handler( WMT_WIFIEVENT wifi_evt )
{
    ilm_struct                            *ilm_p;
    wndrv_wmt_conn_status_update_ind_struct *out_p;
    UINT_32 u4Value, u4Len;   

    //P_TX_CTRL_T prTxCtrl = &(wndrv_context.adpt_p->rTxCtrl);

    //DEBUGFUNC("wndrv_wmt_connecton_status_update_ind_handler");

    out_p = (wndrv_wmt_conn_status_update_ind_struct *)
            construct_local_para( sizeof( wndrv_wmt_conn_status_update_ind_struct ), TD_CTRL );

    out_p->wifi_eventid = wifi_evt;

    if( wifi_evt == WMT_WIFI_CONNECTION || wifi_evt == WMT_WIFI_RATE_UPDATE )
    {
        out_p->curr_chnl = wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].ucPrimaryChannel;
        out_p->curr_data_rate = wndrv_context.adpt_p->rLinkQuality.u2LinkSpeed;

        // record current state
        if( out_p->curr_data_rate <= RATE_11M )
        {
            isCCK = KAL_TRUE;
        }
        else
        {
            isCCK = KAL_FALSE;
        }
    }
    else
    {
        out_p->curr_chnl = 0;
        out_p->curr_data_rate = 0;
    }
    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidQueryPreferredAnt,
                       (PVOID)&u4Value,
                       sizeof(u4Value),
                       (PUINT_32)&u4Len);

   GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

   if( u4Value == 1 ) // BT
   {
       out_p->curr_ant_usage = KAL_TRUE;
   }
   else if( u4Value == 0 )
   {
       out_p->curr_ant_usage = KAL_FALSE;
   }

    if ((last_update_rec.wifi_eventid == out_p->wifi_eventid)
        && (last_update_rec.curr_data_rate == out_p->curr_data_rate)
        && (last_update_rec.curr_chnl == out_p->curr_chnl)
        && (last_update_rec.curr_ant_usage == out_p->curr_ant_usage)
        )
    {
        /* bypass send message due to no change
        */
        free_local_para((local_para_struct*)out_p);
        out_p = NULL;
       // MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_371, "CONN STATUS no change, bypass update to BWCS");
    }
    else
    {
        /* record current status
        */
        last_update_rec.wifi_eventid = out_p->wifi_eventid;
        last_update_rec.curr_data_rate = out_p->curr_data_rate;
        last_update_rec.curr_chnl = out_p->curr_chnl;
        last_update_rec.curr_ant_usage = out_p->curr_ant_usage;

        /* send message
        */
        ilm_p                 = allocate_ilm( MOD_WNDRV );
        ilm_p->src_mod_id     = MOD_WNDRV;
        ilm_p->dest_mod_id    = MOD_BWCS;
        ilm_p->msg_id         = MSG_ID_WNDRV_WMT_CONN_STATUS_UPDATE_IND;//MSG_ID_WNDRV_WMT_CONN_STATUS_UPDATE_IND
        ilm_p->local_para_ptr = (local_para_struct *) out_p;
        ilm_p->peer_buff_ptr  = NULL;

        msg_send_ext_queue(ilm_p);
    }

}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_set_flow_ctrl_req_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_wmt_set_flow_ctrl_req_handler( ilm_struct *ilm_ptr )
{
#if 0   //NOTE(Nelson): MT5931 HW is not supported
    wmt_wndrv_set_flow_ctrl_req_struct *ind_p;
    PARAM_CUSTOM_MCR_RW_STRUC_T rReg;
    UINT_32 setInfoLen;
    UINT_32 u4RegVal;
    WLAN_STATUS status;

    ind_p = (wmt_wndrv_set_flow_ctrl_req_struct *)ilm_ptr->local_para_ptr;

    //4 <1> set medium time
    rReg.u4McrOffset = MCR_MTR1;
    rReg.u4McrData   = (ind_p->u4MediumTime << 16);

    //HAL_MCR_WR(wndrv_context.adpt_p, MCR_MTR1, rReg.u4McrData);
    ARB_ACQUIRE_POWER_CONTROL_FROM_PM(wndrv_context.adpt_p);

    HAL_MCR_WR(wndrv_context.adpt_p, MCR_MTR1, rReg.u4McrData);
    HAL_MCR_RD(wndrv_context.adpt_p, MCR_MTR1, &u4RegVal);

    ARB_RECLAIM_POWER_CONTROL_TO_PM(wndrv_context.adpt_p);

    //4 <2> set period time
    rReg.u4McrOffset = MCR_MTCR1;
    if( ind_p->u4PeriodTime == 0 )
    {
        rReg.u4McrData = (ind_p->u4PeriodTime << 24);
    }
    else
    {
        rReg.u4McrData = ((ind_p->u4PeriodTime << 24) | 0x02);
    }

    //HAL_MCR_WR(wndrv_context.adpt_p, MCR_MTCR1, rReg.u4McrData);

    ARB_ACQUIRE_POWER_CONTROL_FROM_PM(wndrv_context.adpt_p);

    HAL_MCR_WR(wndrv_context.adpt_p, MCR_MTCR1, rReg.u4McrData);
    HAL_MCR_RD(wndrv_context.adpt_p, MCR_MTCR1, &u4RegVal);

    ARB_RECLAIM_POWER_CONTROL_TO_PM(wndrv_context.adpt_p);
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_set_fixed_rx_gain_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#if 0
void wndrv_wmt_set_fixed_rx_gain_req_handler( ilm_struct *ilm_ptr )
{
    wmt_wndrv_set_fixed_rx_gain_req_struct *ind_p;
    UINT_32                      u4Value, u4Len;

    ind_p = (wmt_wndrv_set_fixed_rx_gain_req_struct *)ilm_ptr->local_para_ptr;

    if( ind_p->fgEnableFixedRxGain == TRUE )
    {
        // set to RX Fixed Middle Gain
        u4Value = 1;
    }
    else
    {
        // resume setting
        u4Value = 0;
    }

   	//NOTE(Nelson): Beta5 FW is not supported
    //4 <2> set Fix RX Middle Gain
    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetFixedRxGain,
                       (PVOID)&u4Value,
                       sizeof(u4Value),
                       (PUINT_32)&u4Len);

    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
}
#endif

/*******************************************************************************
* FUNCTION
*   wndrv_wmt_set_omit_low_rate_handler
*
* DESCRIPTION
*   Corresponding to:
*
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_wmt_set_omit_low_rate_req_handler( ilm_struct *ilm_ptr )
{
    wmt_wndrv_set_omit_low_rate_req_struct *ind_p;
    PARAM_RATES_EX sRateEx;
    UINT_32        u4Len;

    DEBUGFUNC("wndrv_wmt_set_omit_low_rate_req_handler");

    ind_p = (wmt_wndrv_set_omit_low_rate_req_struct *)ilm_ptr->local_para_ptr;

    kalMemCopy(sRateEx, ind_p->aucWiFiRates, WMT_MAX_LEN_RATES_EX);

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetDesiredRates,
                       (PVOID)&sRateEx,
                       sizeof(sRateEx),
                       (PUINT_32)&u4Len);

    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
}

/*******************************************************************************
* FUNCTION
*   wndrv_bwcs_dpd_cal_req_handler
*
* DESCRIPTION
*   Handle DPD Cal. Request.
*
* CALLS
*   None
*
* PARAMETERS
*   PTA_IPC_T
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_bwcs_dpd_cal_req_handler( UINT_32 value )
{
    PTA_IPC_T   PtaIpc;
    UINT_32     fgWiFiCoexParam;
    UINT_32     u4Len;
    //UINT_32     u4Value;

    if ( value == VAR_DPD_CAL_REQ )         /* Request DPD cal. */
    {
#if CFG_SINGLE_ANTENNA_ENABLED
        if ( (wndrv_context.adpt_p->rPtaInfo.rBtProfile.eBtProfile == BWCS_PROFILE_SCO) || 
             (wndrv_context.adpt_p->rPtaInfo.rBtProfile.eBtProfile == BWCS_PROFILE_MIXED) )   /* for SCO */
        {
            /* Reject WiFi DPD */
            fgWiFiCoexParam = VAR_WMT_CONFIG_PORT2_PTA_CTRL | COEX_PORT2_PTA_CTL_VAL_REJECT;

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

            wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);

            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
        }
        else
#endif  /* CFG_SINGLE_ANTENNA_ENABLED */
        {
#if CFG_SINGLE_ANTENNA_ENABLED
            /* <1> Disable PTA */
            fgWiFiCoexParam = VAR_WMT_CONFIG_COEX_SETTING;  // WIFI_PTA_DISABLE

            if( KAL_FALSE == bwcs_query_single_antenna_capability() )
            {
                fgWiFiCoexParam |= WMT_COEX_CONFIG_DUAL_ANT_MODE;
            }

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

            wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);

            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
        
            /* <2> Switch ANT to WiFi */
            if ( wndrv_context.adpt_p->fgIsAntBT )
            {
                wndrv_context.adpt_p->fgIsAntBT_beforeDPD = TRUE;
                u4Value = 0;    /* Set ANT to WiFi */
                
                GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

                wlanSetInformation(wndrv_context.adpt_p,
                                   (PFN_OID_HANDLER_FUNC)wlanoidSetPreferredAnt,
                                   (PVOID)&u4Value,
                                   sizeof(u4Value),
                                   (PUINT_32)&u4Len);

                GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
            }
            else
            {
                wndrv_context.adpt_p->fgIsAntBT_beforeDPD = FALSE;
            }
#endif  /* CFG_SINGLE_ANTENNA_ENABLED */
            /* <3> Grand WiFi DPD */
            fgWiFiCoexParam = VAR_WMT_CONFIG_PORT2_PTA_CTRL | COEX_PORT2_PTA_CTL_VAL_GRANT;

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

            wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                           (PVOID)&PtaIpc,
                           sizeof(PtaIpc),
                           (PUINT_32)&u4Len);

            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
        }
    }
    else if ( value == VAR_DPD_CAL_REL )    /* Release DPD cal. */
    {
#if CFG_SINGLE_ANTENNA_ENABLED
        /* <1> Enable PTA */
        fgWiFiCoexParam = ( VAR_WMT_CONFIG_COEX_SETTING | WMT_COEX_CONFIG_BT_ON | WMT_COEX_CONFIG_ENABLE_PTA );

        if( KAL_FALSE == bwcs_query_single_antenna_capability() )
        {
            fgWiFiCoexParam |= WMT_COEX_CONFIG_DUAL_ANT_MODE;
        }

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
        PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
        PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
        PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

        wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                       (PVOID)&PtaIpc,
                       sizeof(PtaIpc),
                       (PUINT_32)&u4Len);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    
        /* <2> Switch ANT to previous state */
        if ( wndrv_context.adpt_p->fgIsAntBT_beforeDPD )
        {
            u4Value = 1;    /* Set ANT to BT */
            
            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSetPreferredAnt,
                               (PVOID)&u4Value,
                               sizeof(u4Value),
                               (PUINT_32)&u4Len);

            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
        }
#endif  /* CFG_SINGLE_ANTENNA_ENABLED */
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_bwcs_fw_event_handler
*
* DESCRIPTION
*   Handle BWCS firmware event.
*
* CALLS
*   None
*
* PARAMETERS
*   PTA_IPC_T
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_bwcs_fw_event_handler( void *pvBuf )
{
    P_PTA_IPC_T prPtaIpc = (P_PTA_IPC_T)pvBuf;    
    UINT_32     fgCoexParam_type = 0;
    UINT_32     fgCoexParam_value = 0;

    if ( !prPtaIpc )
    {
        DBGLOG(
RX, ERROR, ("wndrv_bwcs_fw_event_handler: Null prPtaIpc !") );
        return;
    }

    /* PTA settings:
        PtaIpc.u.aucBTPParams[3] = (UINT_8) fg1WireMode;
        PtaIpc.u.aucBTPParams[2] = (UINT_8) (fg1WireMode >> 8);
        PtaIpc.u.aucBTPParams[1] = (UINT_8) (fg1WireMode >> 16);
        PtaIpc.u.aucBTPParams[0] = (UINT_8) (fg1WireMode >> 24);
    */
    fgCoexParam_type = ( (prPtaIpc->u.aucBTPParams[0] << 24) | prPtaIpc->u.aucBTPParams[1] << 16 );
    fgCoexParam_value = (prPtaIpc->u.aucBTPParams[2] << 8) | prPtaIpc->u.aucBTPParams[3];

    switch ( fgCoexParam_type )
    {
        /* Request DPD cal.  */
        case VAR_WMT_CONFIG_PORT2_PTA_CTRL:
            wndrv_bwcs_dpd_cal_req_handler( fgCoexParam_value );
            break;

        default:
            break;
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_bwcs_set_sco_pta_periodicity_handler
*
* DESCRIPTION
*   SCO PTA periodicity timing setting
*
* CALLS
*   None
*
* PARAMETERS
*   timer_switch: 0 means to disable BT : Wi-Fi SCO high priority  timer, 
*                1 means to enable BT : Wi-Fi SCO high priority  timer
*   wifi_ms: Unit  in ms, means Wi-Fi high priority interval.
*   bt_ms: Unit in ms, means BT high priority interval.
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_bwcs_set_sco_pta_periodicity_handler( UINT_32 timer_switch, UINT_32 wifi_ms, UINT_32 bt_ms  )
{
    UINT_32                        u4Len;
    PTA_IPC_T                      PtaIpc;
    UINT_32                        fgWiFiCoexParam;

    DEBUGFUNC("wndrv_bwcs_set_sco_pta_periodicity_handler");

    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_372, "timer_switch=%d, wifi_ms=%d, bt_ms=%d", timer_switch, wifi_ms, bt_ms);

    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);

    fgWiFiCoexParam = ( VAR_WMT_CONFIG_SCO_PTA_PERIODICITY | (timer_switch << 16) | (wifi_ms << 8) | bt_ms );

    PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
    PtaIpc.u.aucBTPParams[2] = (UINT_8) (fgWiFiCoexParam >> 8);
    PtaIpc.u.aucBTPParams[1] = (UINT_8) (fgWiFiCoexParam >> 16);
    PtaIpc.u.aucBTPParams[0] = (UINT_8) (fgWiFiCoexParam >> 24);

    GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    wlanSetInformation(wndrv_context.adpt_p,
                   (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                   (PVOID)&PtaIpc,
                   sizeof(PtaIpc),
                   (PUINT_32)&u4Len);
    GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
}

/*******************************************************************************
* FUNCTION
*   wndrv_abm_dhcp_complete_handler
*
* DESCRIPTION
*   When DHCP complete, should set WLAN adapter to previous mode.
*
* CALLS
*   wndrv_main()
*
* PARAMETERS
*   ilm_ptr: ILM from ABM
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_abm_dhcp_complete_handler(ilm_struct *ilm_ptr )
{
	kal_event_stop_timer( PARAM_WNDRV_ES_KEEP_CAM_TIMER );
	nicConfigPowerSaveProfile( wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX,
												(PARAM_POWER_MODE)(wndrv_context.adpt_p->u4PowerMode), FALSE );
	free_ilm(ilm_ptr);
	
	return;
}


WLAN_STATUS wndrv_set_sta_power_saving(PARAM_POWER_MODE pm)
{

    WLAN_STATUS ret;
    ret = nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX, pm, FALSE);

    if(ret == WLAN_STATUS_SUCCESS || ret == WLAN_STATUS_PENDING)
        ret = WLAN_STATUS_SUCCESS;
    return WLAN_STATUS_SUCCESS;
}

WLAN_STATUS wndrv_get_sta_power_saving(PARAM_POWER_MODE *pm)
{

    *pm = wndrv_context.adpt_p->rWlanInfo.arPowerSaveMode[NETWORK_TYPE_AIS_INDEX].ucPsProfile;
    return WLAN_STATUS_SUCCESS;

}


#ifdef __WIFI_SNIFFER_SUPPORT__
void wndrv_abm_set_sniffer_mode_req_handler (ilm_struct *ilm_ptr)
{
	 wndrv_abm_set_sniffer_mode_req_struct *req_ptr;
	 wndrv_abm_set_sniffer_mode_cnf_struct *cnf_ptr;
	 P_PARAM_CUSTOM_SET_SNIFFER_MODE_T out_p;
	 UINT32 u4NewPacketFilter,u4Len;
	 ilm_struct *ilm_p;    

	 req_ptr = (wndrv_abm_set_sniffer_mode_req_struct *)ilm_ptr->local_para_ptr;

	 cnf_ptr = (wndrv_abm_set_sniffer_mode_cnf_struct *)
			  construct_local_para(sizeof(wndrv_abm_set_sniffer_mode_cnf_struct), TD_CTRL);
	
	MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"wndrv_abm_set_sniffer_mode_req_handler");
	
	u4NewPacketFilter = (UINT_32)(req_ptr->ucSnifferFilterOptions & PARAM_PACKET_FILTER_SNIFFER_MASK); 
	
	cnf_ptr->status = SNIFFER_MODE_SUCCESS;

	//1. check para
	if(req_ptr->ucOnOffSwitch >= SNIFFER_MODE_ONOFF_END)
	{
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"wndrv_abm_set_sniffer_mode_req_handler:SNIFFER_MODE_FAIL_INVALID_ONOFFSWITCH");

	}
	if((req_ptr->ucOnOffSwitch==SNIFFER_MODE_ON_WITH_PARA)&&(!u4NewPacketFilter))//invalid filter policy
	{
		cnf_ptr->status = SNIFFER_MODE_FAIL_INVALID_SNIFFERFILTEROPTIONS;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"wndrv_abm_set_sniffer_mode_req_handler:SNIFFER_MODE_FAIL_INVALID_SNIFFERFILTEROPTIONS");
	}

	if( (req_ptr->ucOnOffSwitch==SNIFFER_MODE_ON_WITH_PARA) && ((req_ptr->ucChannelNum > 14)||(req_ptr->ucChannelNum < 1)) ) //invalid channel
	{
		cnf_ptr->status = SNIFFER_MODE_FAIL_INVALID_CHANNEL;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"wndrv_abm_set_sniffer_mode_req_handler:SNIFFER_MODE_FAIL_INVALID_CHANNEL");
	}

	if(((req_ptr->ucOnOffSwitch==SNIFFER_MODE_ON_WITH_PARA)||(req_ptr->ucOnOffSwitch==SNIFFER_MODE_ON_WITHOUT_PARA))&&\
		(wndrv_context.adpt_p->prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED)) //not allow to enable sniffer mode when network is connected
	{
		cnf_ptr->status = SNIFFER_MODE_FAIL_NETWORK_IS_CONNECTED;
		MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"wndrv_abm_set_sniffer_mode_req_handler:SNIFFER_MODE_FAIL_NETWORK_IS_CONNECTED");
	}


	//2. set sniffer mode
	if(cnf_ptr->status == SNIFFER_MODE_SUCCESS)
	{
		
		out_p = (P_PARAM_CUSTOM_SET_SNIFFER_MODE_T) WNDRV_ALLOC_BUF(sizeof(PARAM_CUSTOM_SET_SNIFFER_MODE_T));

		out_p->ucOnOffSwitch = (para_sniffer_mode_onoff_enum)req_ptr->ucOnOffSwitch;
		out_p->ucChannelNum = (UINT_8)req_ptr->ucChannelNum;
		out_p->ucSnifferFilterOptions = (UINT_32)req_ptr->ucSnifferFilterOptions;
		out_p->cMinRssi = (INT_32)req_ptr->cMinRssi;
		out_p->cMaxRssi = (INT_32)req_ptr->cMaxRssi;
		out_p->u2MinPacketLength = (UINT_16)req_ptr->u2MinPacketLength;
		out_p->u2MaxPacketLength = (UINT_16)req_ptr->u2MaxPacketLength;

	    wlanSetInformation(wndrv_context.adpt_p,
               (PFN_OID_HANDLER_FUNC)wlanoidSetSnifferMode,
               (PVOID)out_p,
               sizeof(PARAM_CUSTOM_SET_SNIFFER_MODE_T),
               (PUINT_32)&u4Len);

        WNDRV_FREE_BUF(out_p);
	}


	//3.  send cnf message
	cnf_ptr->OnOffStatus = wndrv_context.adpt_p->u1SnifferOnOff;
	
	ilm_p = allocate_ilm (MOD_WNDRV);
	ilm_p->src_mod_id = MOD_WNDRV;
	ilm_p->dest_mod_id = MOD_ABM;
	ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_SNIFFER_MODE_CNF;
	ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
	ilm_p->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_p);

}


void wndrv_abm_iot_listened_ap_info_ind_handler(kal_uint8 status, kal_uint8 ssid_len, kal_uint8 pwd_len, kal_uint8* ssid_pwd, kal_uint8 auth_mode)
{

	 wndrv_abm_iot_listened_ap_info_ind_struct *ind_ptr;
	 ilm_struct *ilm_p; 
	 kal_uint8 TotalLen=0;
	 kal_uint8 Round=0;
	 kal_uint8 i=0;
	 kal_uint8 Plain[96] = {0};
	 kal_uint8 Key[16]={"McdwCnwCdss2_18p"}; 
	 UINT32 iCipterLen = AES_BLOCK_SIZES;

	 if(!status)
	 {
		return;
	 }
	 
	//1. stop timer
	kal_event_stop_timer( PARAM_WNDRV_ES_SMART_CONFIG_TIMER);
	
	if(status == 1)
	{
		MT5931_NEW_TRACE(MT5931_INFO_612,TRACE_INFO,"Smart connection:get sync done,status=%d",status);
	}	
	else if(status == 2)
	{

		ind_ptr = (wndrv_abm_iot_listened_ap_info_ind_struct *)
			  construct_local_para(sizeof(wndrv_abm_iot_listened_ap_info_ind_struct), TD_CTRL);

		wndrv_context.adpt_p->smart_config.u1ListenState = 0 ;
		
		if((pwd_len||ssid_len)&&ssid_pwd)
		{
			TotalLen = ssid_len + pwd_len;
			if (0 == TotalLen%AES_BLOCK_SIZES)
			{
				Round = TotalLen/AES_BLOCK_SIZES;
			}
			else
			{
				Round = TotalLen/AES_BLOCK_SIZES + 1;
			}

			//Decrpty AES cipher
			for (i = 0; i < Round; i ++)
			{
				RT_AES_Decrypt(ssid_pwd + i * AES_BLOCK_SIZES, AES_BLOCK_SIZES, 
					Key, AES_BLOCK_SIZES, Plain + i * AES_BLOCK_SIZES, &iCipterLen);
			}
		}

		kalMemSet(ind_ptr->ssid,0,sizeof(ind_ptr->ssid));
		ind_ptr->ssid_len = ssid_len;
		if(ssid_len&&ssid_pwd)
		{
			kalMemCopy(ind_ptr->ssid,Plain,ssid_len);
		}
		
		kalMemSet(ind_ptr->password,0,sizeof(ind_ptr->password));
		ind_ptr->password_len = pwd_len;
		if(pwd_len&&ssid_pwd)
		{			
			kalMemCopy(ind_ptr->password,Plain+ssid_len,pwd_len);
		}

		ind_ptr->auth_mode = auth_mode;
		
		ilm_p = allocate_ilm (MOD_WNDRV);
		ilm_p->src_mod_id = MOD_WNDRV;
		ilm_p->dest_mod_id = MOD_ABM;
		ilm_p->msg_id = MSG_ID_WNDRV_ABM_IOT_LISTENED_AP_INFO_IND;
		ilm_p->local_para_ptr = (local_para_struct *)ind_ptr;
		ilm_p->peer_buff_ptr = NULL;

		msg_send_ext_queue(ilm_p);
	}
}
#endif


