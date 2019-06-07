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

#if CFG_ENABLE_WIFI_TETHERING

//#include "kal_release.h"
#include "precomp.h"

#include "wndrv_context.h" /* wndrv_context_struct */
#include "hostap_wndrv_msgs.h"

//#include "tcpip2abm_struct.h"
//#include "task_config.h"
#include "wndrv_flc_msg.h"
#include "wndrv_abm_msg.h"


extern wndrv_context_struct wndrv_context;
extern PINT_8               wndrv_firmware_p;
extern UINT_32              wndrv_firmware_size;
extern kal_bool             WNDRVPauseFlag;
extern kal_bool             fgStaModeActive;


extern void wtInit(IN P_ADAPTER_T prAdapter);

/* [MAUI_03125163][1]Assert fail: l1sm.c 4881 0x0 0x0 0x0 - (LISR)CTIRQ1 */
extern void L1SM_EnterFPM(kal_bool isFPM, kal_uint8 module);
/******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************
*/
void hostap_wndrv_msg_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_init_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_deinit_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_add_sta_req_handler(ilm_struct *ilm_ptr);
void wndrv_send_hostap_add_sta_cnf_handler(kal_bool status, PUINT_8 pucStaAddr);
void hostap_wndrv_remove_sta_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_get_sta_info_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_sta_flags_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_flush_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_generic_elem_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_essid_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_8021x_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_host_encrypt_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_host_decrypt_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_privacy_req_handler(ilm_struct *ilm_ptr);
void wndrv_send_hostap_set_privacy_cnf_handler(void);
void hostap_wndrv_set_channel_req_handler(ilm_struct *ilm_ptr);
//void wndrv_send_hostap_set_channel_cnf_handler( kal_bool status);
void hostap_wndrv_set_encryption_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_get_encryption_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_frame_ind_handler(PVOID pvPacket, UINT_16 u2PacketLen);
void hostap_wndrv_frame_req_handler(ilm_struct *ilm_ptr);
void flc_wndrv_ul_sspdu_resume_handler(ilm_struct *ilm_ptr);
void wndrv_abm_set_tx_pwr_req_handler(ilm_struct *ilm_ptr);

/******************************************************************************
*                            P U B L I C   D A T A
*******************************************************************************
*/
#define WNDRV_HOSTAP_DEFAULT_POWER (10)

/******************************************************************************
*                           P R I V A T E   D A T A
*******************************************************************************
*/


/*******************************************************************************
* FUNCTION
*   hostap_wndrv_msg_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_INIT_REQ
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
void hostap_wndrv_msg_handler(ilm_struct *ilm_ptr)
{
    if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_INIT_REQ) {
        hostap_wndrv_init_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_DEINIT_REQ) {
        hostap_wndrv_deinit_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_ADD_STA_REQ) {
        hostap_wndrv_add_sta_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id ==  MSG_ID_HOSTAP_WNDRV_REMOVE_STA_REQ) {
        hostap_wndrv_remove_sta_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_GET_STA_INFO_REQ) {
        hostap_wndrv_get_sta_info_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_STA_FLAGS_REQ) {
        hostap_wndrv_set_sta_flags_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_FLUSH_REQ) {
        hostap_wndrv_flush_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_GENERIC_ELEM_REQ) {
        hostap_wndrv_set_generic_elem_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_ESSID_REQ) {
        hostap_wndrv_set_essid_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_8021X_REQ) {
        hostap_wndrv_set_8021x_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_HOST_ENCRYPT_REQ) {
        hostap_wndrv_set_host_encrypt_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_HOST_DECRYPT_REQ) {
        hostap_wndrv_set_host_decrypt_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_ENCRYPTION_REQ) {
        hostap_wndrv_set_encryption_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_GET_ENCRYPTION_REQ) {
        hostap_wndrv_get_encryption_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_PRIVACY_REQ) {
        hostap_wndrv_set_privacy_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_SET_CHANNEL_REQ) {
        hostap_wndrv_set_channel_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_HOSTAP_WNDRV_FRAME_REQ) {
        hostap_wndrv_frame_req_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_FLC_WNDRV_UL_SSPDU_RESUME) {
        flc_wndrv_ul_sspdu_resume_handler(ilm_ptr);
    } else if (ilm_ptr->msg_id == MSG_ID_WNDRV_ABM_SET_TX_PWR_REQ) {
        wndrv_abm_set_tx_pwr_req_handler(ilm_ptr);
    }

    return;
}

/*******************************************************************************
* FUNCTION
*   wndrv_send_msg_to_hostap
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_INIT_REQ
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
void wndrv_send_msg_to_hostap(msg_type msg_name, void *param_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    /* First allocate a memory for an interlayer message structure */
    ilm_ptr = allocate_ilm(MOD_WNDRV);

    ilm_ptr->msg_id = (msg_type) msg_name;    /* Set the message id */
    ilm_ptr->peer_buff_ptr = NULL;      /* there are no peer message to be
                                           sent to cmux. Hence this field is null. */
    ilm_ptr->local_para_ptr = (local_para_struct *) param_ptr;

    ilm_ptr->src_mod_id = MOD_WNDRV;
    ilm_ptr->dest_mod_id = MOD_HOSTAP;

    msg_send_ext_queue(ilm_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_init_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_INIT_REQ
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
void hostap_wndrv_init_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_init_req_struct *req_ptr;
    hostap_wndrv_init_cnf_struct *cnf_ptr;
    P_REG_INFO_T                 prRegInfo = &wndrv_context.adpt_p->prGlueInfo->rRegInfo;

    cnf_ptr = (hostap_wndrv_init_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_init_cnf_struct), TD_CTRL);

    L1SM_EnterFPM(KAL_TRUE, 2);
    //wndrv_bus_hpi_module_control(1);

    /* init rWtInfo
    */
    kalMemZero(&(wndrv_context.glue_p->rWtInfo), sizeof(GL_WT_INFO));

    /* notify WiFi Tethering Enable
    */
    wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable = TRUE;
    wndrv_context.glue_p->rWtInfo.fgIsPrivacy = FALSE;
    wndrv_context.glue_p->rWtInfo.fgIsGTKExist = FALSE;
    wndrv_context.glue_p->rWtInfo.fgIsInit = FALSE;
    kalMemZero(wndrv_context.glue_p->rWtInfo.GTKkeyBuffer, (sizeof(PARAM_KEY_T) + CCMP_KEY_LEN));

    cnf_ptr->status = KAL_TRUE;
    if (wndrv_context.glue_p->fgIsAdaptStopped == TRUE) { //init ap&sta mode driver
        if (wlanAdapterStart(wndrv_context.adpt_p, prRegInfo,
                             (PVOID)wndrv_firmware_p, wndrv_firmware_size) == WLAN_STATUS_SUCCESS) {
            wndrv_context.glue_p->fgIsAdaptStopped = FALSE;
            cnf_ptr->status = KAL_TRUE;
        } else {
            if (wlanWholeChipReset(wndrv_context.adpt_p) == WLAN_STATUS_SUCCESS) {
                wndrv_context.glue_p->fgIsAdaptStopped = FALSE;
                cnf_ptr->status = KAL_TRUE;
            } else {
                cnf_ptr->status = KAL_FALSE;
            }
        }
    }


    /*sta&ap Addr config in wlanAdapterStart, send ap addr to hostap*/

    COPY_MAC_ADDR(cnf_ptr->ap_addr, wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr);

    wtUpdateTxPower(wndrv_context.adpt_p, WNDRV_HOSTAP_DEFAULT_POWER);
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_INIT_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_deinit_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_DEINIT_REQ
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
void hostap_wndrv_deinit_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_deinit_req_struct *req_ptr;
    hostap_wndrv_deinit_cnf_struct *cnf_ptr;

    cnf_ptr = (hostap_wndrv_deinit_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_deinit_cnf_struct), TD_CTRL);

    if (fgStaModeActive == FALSE) { //deinit ap&sta mode driver

        wlanAdapterStop(wndrv_context.adpt_p);
        wndrv_context.glue_p->fgIsAdaptStopped = TRUE;

    } else { //deinit ap mode driver
        wlanAdapterStopForAPorSTA(wndrv_context.adpt_p, KAL_TRUE);
    }

    //wndrv_bus_hpi_module_control(0);
    L1SM_EnterFPM(KAL_FALSE, 2);

    wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable = FALSE;
    wndrv_context.glue_p->rWtInfo.fgIsInit = FALSE;
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_DEINIT_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_essid_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_ESSID_REQ
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
void hostap_wndrv_set_essid_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_set_essid_req_struct *req_ptr;
    hostap_wndrv_set_essid_cnf_struct *cnf_ptr;
    P_BSS_INFO_T prBssInfo = &(wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

    req_ptr = (hostap_wndrv_set_essid_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_essid_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_essid_cnf_struct), TD_CTRL);

    //4 <1> Setup SSID
    if (req_ptr->essid_flag == 1) {
        prBssInfo->ucSSIDLen = req_ptr->essid_len;
        kalMemCopy(prBssInfo->aucSSID, req_ptr->essid, req_ptr->essid_len);
        cnf_ptr->status = KAL_TRUE;
    } else {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_267, "[APNIC] essid_flag %d", req_ptr->essid_flag);
        cnf_ptr->status = KAL_FALSE;
    }

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_ESSID_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_channel_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_CHANNEL_REQ
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
void hostap_wndrv_set_channel_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_set_channel_req_struct *req_ptr;
    hostap_wndrv_set_channel_cnf_struct *cnf_ptr;
    P_GL_WT_INFO prWtInfo = &(wndrv_context.adpt_p->prGlueInfo->rWtInfo);
    P_BSS_INFO_T prBssInfo = &(wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

    req_ptr = (hostap_wndrv_set_channel_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_channel_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_channel_cnf_struct), TD_CTRL);


    prWtInfo->ucPrimaryChannel = req_ptr->channel_num;
    prBssInfo->ucPrimaryChannel = prWtInfo->ucPrimaryChannel;

    if (req_ptr->ap_phytype == 0) {
        prBssInfo->ucConfigAdHocAPMode = AP_MODE_11B;
        prBssInfo->ucPhyTypeSet = wndrv_context.adpt_p->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11B;
    } else if (req_ptr->ap_phytype == 1) {
        prBssInfo->ucConfigAdHocAPMode = AP_MODE_MIXED_11BG;
        prBssInfo->ucPhyTypeSet = wndrv_context.adpt_p->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11BG;
    } else if (req_ptr->ap_phytype == 2) {
        prBssInfo->ucConfigAdHocAPMode = AP_MODE_11G;
        prBssInfo->ucPhyTypeSet = wndrv_context.adpt_p->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11G;
    } else if (req_ptr->ap_phytype == 3) {
        prBssInfo->ucConfigAdHocAPMode = AP_MODE_MIXED_11BG;
        prBssInfo->ucPhyTypeSet = wndrv_context.adpt_p->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11BGN;
    }


    cnf_ptr->status = KAL_TRUE;
    cnf_ptr->channel_num = req_ptr->channel_num;
#if 0
    /* Active P2P Network */
    SET_NET_ACTIVE(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);
    SET_NET_PWR_STATE_ACTIVE(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);
    nicActivateNetwork(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);

    /* Indicate CNM a New Network is Creating */
    wtRequestCh(wndrv_context.adpt_p);
#endif
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_CHANNEL_CNF, cnf_ptr);
    //wndrv_send_hostap_set_channel_cnf_handler(TRUE);



    return;
}
#if 0
void wndrv_send_hostap_set_channel_cnf_handler(kal_bool status)
{
    hostap_wndrv_set_channel_cnf_struct *cnf_ptr;
    cnf_ptr = (hostap_wndrv_set_channel_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_channel_cnf_struct), TD_CTRL);

    cnf_ptr->status = status;
    cnf_ptr->channel_num = wndrv_context.adpt_p->prGlueInfo->rWtInfo.ucPrimaryChannel;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_CHANNEL_CNF, cnf_ptr);

    return;
}
#endif
/*******************************************************************************
* FUNCTION
*   hostap_wndrv_add_sta_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_ADD_STA_REQ
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
void hostap_wndrv_add_sta_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_add_sta_req_struct *req_ptr;
    //hostap_wndrv_add_sta_cnf_struct *cnf_ptr;
    P_STA_RECORD_T prStaRec;

    req_ptr = (hostap_wndrv_add_sta_req_struct *)ilm_ptr->local_para_ptr;

    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_268, "[AID=%d]Sta Addr: "MACSTR, req_ptr->aid, MAC2STR(req_ptr->sta_addr));



    prStaRec = cnmGetStaRecByAddress(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX, req_ptr->sta_addr);

    if (prStaRec->ucStaState == STA_STATE_3) {
        /* SEND ILM */
        wndrv_send_hostap_add_sta_cnf_handler(KAL_TRUE, req_ptr->sta_addr);
    } else {
        MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_269, "!!!SHOULD wtRunEventAAAComplete FIRST!!!");
        prStaRec->fgIsAddStaReq = KAL_TRUE;
        //wndrv_send_hostap_add_sta_cnf_handler(KAL_FALSE, req_ptr->sta_addr);
    }
    return;
}

void wndrv_send_hostap_add_sta_cnf_handler(kal_bool status, PUINT_8 pucStaAddr)
{
    hostap_wndrv_add_sta_cnf_struct *cnf_ptr;
    cnf_ptr = (hostap_wndrv_add_sta_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_add_sta_cnf_struct), TD_CTRL);

    cnf_ptr->status = status;
    COPY_MAC_ADDR(cnf_ptr->sta_addr, pucStaAddr);

    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_270, "[TETHERING]ADD_STA: %d",
                 wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].rStaRecOfClientList.u4NumElem);

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_ADD_STA_CNF, cnf_ptr);
    return;
}


/*******************************************************************************
* FUNCTION
*   hostap_wndrv_remove_sta_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_REMOVE_STA_REQ
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
void hostap_wndrv_remove_sta_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_remove_sta_req_struct *req_ptr;
    hostap_wndrv_remove_sta_cnf_struct *cnf_ptr;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    P_BSS_INFO_T prBssInfo = (P_BSS_INFO_T)NULL;

    req_ptr = (hostap_wndrv_remove_sta_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_remove_sta_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_remove_sta_cnf_struct), TD_CTRL);

    prStaRec = cnmGetStaRecByAddress(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX, req_ptr->sta_addr);

    //ASSERT(u2ValidateSta == wtGetStaRecNumByNetType(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX));
    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_271, "Sta Addr: "MACSTR, MAC2STR(req_ptr->sta_addr));

    if (prStaRec) {
        /* remove trigger by hostap */
        prBssInfo = &(wndrv_context.adpt_p->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);
        /* send Deauth */
        //wtFuncDisconnect(wndrv_context.adpt_p, prStaRec, TRUE, REASON_CODE_DISASSOC_INACTIVITY);
        bssRemoveStaRecFromClientList(wndrv_context.adpt_p, prBssInfo, prStaRec);
        cnmStaRecFree(wndrv_context.adpt_p, prStaRec, FALSE);
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_272, "!!! Remove STA:"MACSTR, MAC2STR(req_ptr->sta_addr));
        cnf_ptr->status = KAL_TRUE;
        COPY_MAC_ADDR(cnf_ptr->sta_addr, req_ptr->sta_addr);
    } else {
        MT5931_TRACE(TRACE_ERROR, MT5931_INFO_273, "[WARNING]Cannot Find STA"MACSTR, MAC2STR(req_ptr->sta_addr));
        cnf_ptr->status = KAL_FALSE;
        COPY_MAC_ADDR(cnf_ptr->sta_addr, req_ptr->sta_addr);
    }

    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_274, "[TETHERING]REMOVE_STA: %d",
                 wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].rStaRecOfClientList.u4NumElem);


    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_REMOVE_STA_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_get_sta_info_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_GET_STA_INFO_REQ
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
void hostap_wndrv_get_sta_info_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_get_sta_info_req_struct *req_ptr;
    hostap_wndrv_get_sta_info_cnf_struct *cnf_ptr;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;

    req_ptr = (hostap_wndrv_get_sta_info_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_get_sta_info_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_get_sta_info_cnf_struct), TD_CTRL);

    prStaRec = cnmGetStaRecByAddress(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX, req_ptr->sta_addr);

    cnf_ptr->status = KAL_TRUE;
    if (prStaRec) {
        cnf_ptr->inactive_sec = prStaRec->u4InactTime; /* disable hostap timeout check capability */
    } else {
        MT5931_TRACE(TRACE_ERROR, MT5931_INFO_275, "[ERROR] CANNOT FIND STA:"MACSTR, MAC2STR(req_ptr->sta_addr));
        cnf_ptr->inactive_sec = 0xFFFF;
    }
    COPY_MAC_ADDR(cnf_ptr->sta_addr, req_ptr->sta_addr);

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_GET_STA_INFO_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_sta_flags_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_STA_FLAGS_REQ
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
void hostap_wndrv_set_sta_flags_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_set_sta_flags_req_struct *req_ptr;
    hostap_wndrv_set_sta_flags_cnf_struct *cnf_ptr;

    req_ptr = (hostap_wndrv_set_sta_flags_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_sta_flags_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_sta_flags_cnf_struct), TD_CTRL);

    cnf_ptr->status = KAL_TRUE;
    COPY_MAC_ADDR(cnf_ptr->sta_addr, req_ptr->sta_addr);

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_STA_FLAGS_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_flush_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_FLUSH_REQ
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
void hostap_wndrv_flush_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_flush_req_struct *req_ptr;
    hostap_wndrv_flush_cnf_struct *cnf_ptr;

#if (TRACE_BUFF_DEBUG == 1)
    ILMTraceSetCheckflag(KAL_TRUE);
#endif
    //req_ptr = (hostap_wndrv_flush_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_flush_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_flush_cnf_struct), TD_CTRL);

    cnf_ptr->status = KAL_TRUE;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_FLUSH_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_generic_elem_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_GENERIC_ELEM_REQ
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
void hostap_wndrv_set_generic_elem_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_set_generic_elem_req_struct *req_ptr;
    hostap_wndrv_set_generic_elem_cnf_struct *cnf_ptr;

    req_ptr = (hostap_wndrv_set_generic_elem_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_generic_elem_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_generic_elem_cnf_struct), TD_CTRL);

    if (req_ptr->len != 0) {
        /* carry RSN IE for WAP2-PSK
         * update beacon content to add RSN IE
         */
        bssUpdateBeaconContent(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);
        MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_276, "[WPA2-PSK] Carry RSN IE");
    } else {
        /* open network, do nothing */
        MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_277, "[Open] Carry none");
    }
    cnf_ptr->status = KAL_TRUE;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_GENERIC_ELEM_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_privacy_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_PRIVACY_REQ
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
void hostap_wndrv_set_privacy_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_set_privacy_req_struct *req_ptr;
    // hostap_wndrv_set_privacy_cnf_struct *cnf_ptr;
    //int value;
    req_ptr = (hostap_wndrv_set_privacy_req_struct *)ilm_ptr->local_para_ptr;
    // cnf_ptr = (hostap_wndrv_set_privacy_cnf_struct *)
    //  construct_local_para( sizeof( hostap_wndrv_set_privacy_cnf_struct ), TD_CTRL );

    if (wndrv_context.glue_p->rWtInfo.fgIsInit == FALSE) {
        /* init case */
        /* open = 0; psk = 1 */
        wndrv_context.glue_p->rWtInfo.fgIsPrivacy = (req_ptr->fgIsPrivacy) ? TRUE : FALSE;

        /* Active P2P Network */
        SET_NET_ACTIVE(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);
        SET_NET_PWR_STATE_ACTIVE(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);
        nicActivateNetwork(wndrv_context.adpt_p, NETWORK_TYPE_P2P_INDEX);

        /* Indicate CNM a New Network is Creating */
        wtRequestCh(wndrv_context.adpt_p);
        // cnf_ptr->status = KAL_TRUE;
    } else {
        /* deinit case, bypass active action */

        //wndrv_send_hostap_set_privacy_cnf_handler();
        wtRequestCh(wndrv_context.adpt_p);//by Jerry
        //cnf_ptr->status = KAL_TRUE;
    }

    /* SEND ILM delay */
    //wndrv_send_msg_to_hostap(MAP_MSG_ID_HOSTAP_WNDRV_SET_PRIVACY_CNF, cnf_ptr);

    return;
}

void wndrv_send_hostap_set_privacy_cnf_handler(void)
{
    hostap_wndrv_set_privacy_cnf_struct *cnf_ptr;
    cnf_ptr = (hostap_wndrv_set_privacy_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_privacy_cnf_struct), TD_CTRL);

    cnf_ptr->status = KAL_TRUE;
    wndrv_context.glue_p->rWtInfo.fgIsInit = TRUE;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_PRIVACY_CNF, cnf_ptr);

    return;
}


/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_encryption_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_ENCRYPTION_REQ
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
void hostap_wndrv_set_encryption_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_set_encryption_req_struct *req_ptr;
    hostap_wndrv_set_encryption_cnf_struct *cnf_ptr;
    UINT_8        keyBuffer[sizeof(PARAM_KEY_T) + CCMP_KEY_LEN];
    P_PARAM_KEY_T prKey = (P_PARAM_KEY_T) keyBuffer;
    UINT_32       setInfoLen;

    req_ptr = (hostap_wndrv_set_encryption_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_encryption_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_encryption_cnf_struct), TD_CTRL);

    if (strncmp((char *)(req_ptr->alg), "CCMP", req_ptr->alg_len) == 0) {
        /* Key ID */
        prKey->u4KeyIndex = req_ptr->idx;

        /* Tx Key Bit(31)*/
        if (req_ptr->flags == 1) {
            prKey->u4KeyIndex |= IS_TRANSMIT_KEY;
        }

        /* Pairwise Key Bit(30) */
        if (req_ptr->idx == 1) {
            /* group key */
        } else {
            /* pairwise key */
            prKey->u4KeyIndex |= IS_UNICAST_KEY;
        }
        /* BSSID */
        COPY_MAC_ADDR(prKey->arBSSID, req_ptr->sta_addr);
        /* Key Material */
        kalMemCopy(prKey->aucKeyMaterial, req_ptr->key, req_ptr->key_len);

        prKey->u4KeyLength = req_ptr->key_len;
        prKey->u4Length = OFFSET_OF(PARAM_KEY_T, aucKeyMaterial) + req_ptr->key_len;

        if (req_ptr->idx == 1) {
            /* delay add GTK */
            wndrv_context.glue_p->rWtInfo.fgIsGTKExist = TRUE;

            /* record GTK */
            kalMemCopy(wndrv_context.glue_p->rWtInfo.GTKkeyBuffer, prKey, (sizeof(PARAM_KEY_T) + CCMP_KEY_LEN));
        } else {
            /* add PTK */
            wlanSetInformation(wndrv_context.adpt_p,
                               wtAddPairwiseKey,
                               prKey,
                               prKey->u4Length,
                               (PUINT_32)&setInfoLen);
        }
    }

    cnf_ptr->status = KAL_TRUE;
    COPY_MAC_ADDR(cnf_ptr->sta_addr, req_ptr->sta_addr);
    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_ENCRYPTION_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_get_encryption_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_GET_ENCRYPTION_REQ
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
void hostap_wndrv_get_encryption_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_get_encryption_req_struct *req_ptr;
    hostap_wndrv_get_encryption_cnf_struct *cnf_ptr;

    //req_ptr = (hostap_wndrv_get_encryption_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_get_encryption_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_get_encryption_cnf_struct), TD_CTRL);

    //unused

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_GET_ENCRYPTION_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_8021x_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_8021X_REQ
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
void hostap_wndrv_set_8021x_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_set_8021x_req_struct *req_ptr;
    hostap_wndrv_set_8021x_cnf_struct *cnf_ptr;
    //int value;

    //req_ptr = (hostap_wndrv_set_8021x_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_8021x_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_8021x_cnf_struct), TD_CTRL);

    /* unused in MT5931, bypass this message */
    cnf_ptr->status = KAL_TRUE;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_8021X_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_host_encrypt_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_HOST_ENCRYPT_REQ
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
void hostap_wndrv_set_host_encrypt_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_set_host_encrypt_req_struct *req_ptr;
    hostap_wndrv_set_host_encrypt_cnf_struct *cnf_ptr;
    //int value;

    //req_ptr = (hostap_wndrv_set_host_encrypt_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_host_encrypt_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_host_encrypt_cnf_struct), TD_CTRL);

    /* unused in MT5931, bypass this message */
    cnf_ptr->status = KAL_TRUE;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_HOST_ENCRYPT_CNF, cnf_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_set_host_decrypt_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_SET_HOST_DECRYPT_REQ
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
void hostap_wndrv_set_host_decrypt_req_handler(ilm_struct *ilm_ptr)
{
    //hostap_wndrv_set_host_decrypt_req_struct *req_ptr;
    hostap_wndrv_set_host_decrypt_cnf_struct *cnf_ptr;

    //req_ptr = (hostap_wndrv_set_host_decrypt_req_struct *)ilm_ptr->local_para_ptr;
    cnf_ptr = (hostap_wndrv_set_host_decrypt_cnf_struct *)
              construct_local_para(sizeof(hostap_wndrv_set_host_decrypt_cnf_struct), TD_CTRL);

    /* unused in MT5931, bypass this message */
    cnf_ptr->status = KAL_TRUE;

    /* SEND ILM */
    wndrv_send_msg_to_hostap(MSG_ID_HOSTAP_WNDRV_SET_HOST_DECRYPT_CNF, cnf_ptr);

    return;
}
#if 0
static void _wpa_hexdump(const char *title, const char *buf,
                         kal_uint16 len, int show)
{
    kal_uint16 i, j = 0;
    kal_char str_buff[129];
    kal_uint16 local_len = 0;

    //wpa_debug_print_timestamp();
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "%s - hexdump(len=%d):", title, len);
    kal_mem_set(str_buff, 0, 129);
    while (len > 0) {
        if (len <= 64) {
            local_len = len;
        } else {
            local_len = 64;
        }

        if (show) {
            for (i = 0; i < local_len; i++) {
                kal_sprintf(&(str_buff[i * 2]), "%02x-", (*(buf + i + j * 64)) & 0x000000FF);
            }
            MT5931_TRACE(TRACE_GROUP_10, str_buff);
        } else {
            MT5931_TRACE(TRACE_GROUP_10, " [REMOVED]");
        }
        len -= local_len;
        j++;
    }
}
#endif
/*******************************************************************************
* FUNCTION
*   hostap_wndrv_frame_req_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_FRAME_REQ
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
void hostap_wndrv_frame_req_handler(ilm_struct *ilm_ptr)
{
    hostap_wndrv_frame_req_struct *req_ptr;
    PUINT_8 pdu_ptr;
    UINT_16 pdu_len;
    WLAN_MAC_HEADER_T *pWlanHeader;
    UINT_16 frameCtrl;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    wndrv_pkt_descriptor_struct *wndrv_pkt_p;
    P_PACKET_INFO_T prPacketInfo;
    UINT_8      aucDestAddr[MAC_ADDR_LEN];
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];

    DEBUGFUNC("hostap_wndrv_frame_req_handler");

    req_ptr = (hostap_wndrv_frame_req_struct *)ilm_ptr->local_para_ptr;

    /* Packets sending out from management interface, must be 802.11 packet.
     * pFrame = WLAN MAC HEADER + PAYLOAD
    */
    pdu_ptr = (PUINT_8)get_pdu_ptr(ilm_ptr->peer_buff_ptr, &pdu_len);
    pWlanHeader = (WLAN_MAC_HEADER_T *)pdu_ptr;

    /* Check version number, if the number is 2, it is special use for communication
    * between hostapd and kernel driver. TODO: Really get ACK status from low level driver.
    */
    frameCtrl = pWlanHeader->u2FrameCtrl;
    if ((frameCtrl & MASK_FC_PROTOCOL_VER) == 2) {
        if (((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_AUTH) ||
                ((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_ASSOC_RSP) ||
                ((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_REASSOC_RSP)) {
            prStaRec = cnmGetStaRecByAddress(wndrv_context.adpt_p,
                                             (UINT_8) NETWORK_TYPE_P2P_INDEX,
                                             req_ptr->addr1); /* addr is dest addr */

            if (prStaRec) {

                if ((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_AUTH) {
                    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_278, "authSendAuthFrame Dst: "MACSTR, MAC2STR(prStaRec->aucMacAddr));
                    authSendAuthFrame(wndrv_context.adpt_p,
                                      prStaRec,
                                      (ENUM_NETWORK_TYPE_INDEX_T)(prStaRec->ucNetTypeIndex),
                                      NULL, /* should not enter error status condition */
                                      AUTH_TRANSACTION_SEQ_2,
                                      prStaRec->u2StatusCode);
                } else {
                    MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_279, "assocSendReAssocRespFrame Dst: "MACSTR, MAC2STR(prStaRec->aucMacAddr));
                    /* send ASSOC_RSP & REASSOC_RSP */
                    assocSendReAssocRespFrame(wndrv_context.adpt_p, prStaRec);
                }
            } else {
                MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_280, "[ERROR!!]prStaRec = NULL, send hostap frame fail");
            }

            /* process loopback */
            //hostap_wndrv_frame_ind_handler((PVOID)pdu_ptr, pdu_len);

            free_peer_buff(ilm_ptr->peer_buff_ptr);
        } else if (((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_DATA) ||
                   ((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_QOS_DATA)) {
            /* send 4way handshake */

            COPY_MAC_ADDR(aucDestAddr, pWlanHeader->aucAddr1); /* DA */
            COPY_MAC_ADDR(aucSrcAddr, pWlanHeader->aucAddr3); /* SA */

            /* remove WLAN MAC header + Ethernet-II snap header
               remain TypeLen + payload */
            remove_hdr_of_peer_buff(ilm_ptr->peer_buff_ptr, WLAN_MAC_HEADER_LEN + 6);

            /*add SA : SA + TypeLen + payload */
            prepend_to_peer_buff(ilm_ptr->peer_buff_ptr, aucSrcAddr, MAC_ADDR_LEN);

            /* add DA : DA + SA + TypeLen + payload */
            prepend_to_peer_buff(ilm_ptr->peer_buff_ptr, aucDestAddr, MAC_ADDR_LEN);

            wndrv_pkt_p = wndrv_get_pkt_descriptor(ilm_ptr->peer_buff_ptr);
            pdu_ptr = (PUINT_8)get_pdu_ptr(ilm_ptr->peer_buff_ptr, &pdu_len);

            wndrv_pkt_p->tx_peer_p     = ilm_ptr->peer_buff_ptr;
            wndrv_pkt_p->src_mod_id    = ilm_ptr->src_mod_id;
            wndrv_pkt_p->flc_pool_type = 0;
            wndrv_pkt_p->pool_id       = FLC2_POOL_ID_APPPDU_UL;
            wndrv_pkt_p->flc_mode      = KAL_FALSE;

            prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);

            wndrv_pkt_p->tid           = 0;
            wndrv_pkt_p->queue_num     = mpMapTidToQueue(wndrv_pkt_p->tid);
            wndrv_pkt_p->check_mark    = WNDRV_BUFFER_HDLR_CHECK_MARK;
            wndrv_pkt_p->flags         = 0;
            wndrv_pkt_p->sys_time      = kalGetTimeTick();
            wndrv_pkt_p->prPacketInfo  = prPacketInfo;

            prPacketInfo->prPacket     = (P_NATIVE_PACKET) ilm_ptr->peer_buff_ptr;
            prPacketInfo->wndrv_pkt_p  = wndrv_pkt_p;

            pdu_len -= WNDRV_ETHERNET_MAC_SPACE; /* payload len */

            PACKET_INFO_INIT(prPacketInfo, \
                             FALSE, \
                             TRUE, \
                             ilm_ptr->peer_buff_ptr, \
                             wndrv_pkt_p->tid, \
                             WNDRV_ETHERNET_MAC_SPACE, \
                             pdu_len, \
                             ((PUINT_8)pdu_ptr)\
                            );
            GLUE_SET_PKT_FLAG_P2P(ilm_ptr->peer_buff_ptr);

            MT5931_TRACE(TRACE_HOTSPOT, MT5931_INFO_281, "[TETHERING] TX EAPOL: peer_buff %x", ilm_ptr->peer_buff_ptr);
            //_wpa_hexdump("TX EAPOL dump:", (char *)pdu_ptr, (pdu_len + WNDRV_ETHERNET_MAC_SPACE), 1);
            wlanSendPacket(wndrv_context.glue_p, wndrv_pkt_p);
            // free wlanReturnPacket
        }
    } else if (((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_DEAUTH) ||
               ((frameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_DISASSOC)) {
        /* hostap behavior:
           1. rx disassoc
           2. start timer with deauth
           3. timeout, send deauth.
           but driver remove StaRec in step1. so ignore deauth/disassoc from hostap if StaRec not found
        */
        prStaRec = cnmGetStaRecByAddress(wndrv_context.adpt_p,
                                         (UINT_8) NETWORK_TYPE_P2P_INDEX,
                                         req_ptr->addr1); /* addr is dest addr */
        if (prStaRec) {
            MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_282, "[THTHERING] send DEAUTH/DISASSOC from HOSTAP (TBD.)");
        } else {
            MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_283, "[THTHERING] bypass send DEAUTH/DISASSOC from HOSTAP");
        }

        free_peer_buff(ilm_ptr->peer_buff_ptr);
    }


    ilm_ptr->peer_buff_ptr = NULL;

    return;
}

/*******************************************************************************
* FUNCTION
*   hostap_wndrv_frame_ind_handler
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_FRAME_IND
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
void hostap_wndrv_frame_ind_handler(PVOID pvPacket, UINT_16 u2PacketLen)
{
    ilm_struct *ilm_ptr = NULL;
    hostap_wndrv_frame_ind_struct *ind_ptr;
    WLAN_MAC_HEADER_T *pWlanHeader;
    peer_buff_struct *peer_buff_ptr;
    PUINT_8 pdu_ptr;
    UINT_16 pdu_len;


    /* call by nicRxProcessMgmtPacket(), prSwRfb->pvHeader
    */
    //check_wifi_buff_flag = TRUE;
    //ILMTraceCheck(10);
    ind_ptr = (hostap_wndrv_frame_ind_struct *)
              construct_local_para(sizeof(hostap_wndrv_frame_ind_struct), TD_CTRL);


    pWlanHeader = (WLAN_MAC_HEADER_T *)pvPacket;

    /* fill local parameter */
    COPY_MAC_ADDR(ind_ptr->addr1, pWlanHeader->aucAddr1);
    COPY_MAC_ADDR(ind_ptr->addr2, pWlanHeader->aucAddr2);
    COPY_MAC_ADDR(ind_ptr->addr3, pWlanHeader->aucAddr3);
    ind_ptr->duration_id = pWlanHeader->u2DurationID;
    ind_ptr->frame_control = pWlanHeader->u2FrameCtrl;
    ind_ptr->seq_ctrl = pWlanHeader->u2SeqCtrl;



    /* copy mgmt frame to hostap */
    peer_buff_ptr = construct_peer_buff(u2PacketLen, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    WNDRV_EXT_ASSERT(peer_buff_ptr != NULL,
                     "hostap_wndrv_frame_ind_handler: system buffer not available",
                     u2PacketLen, 0, 0);

    pdu_ptr = (PUINT_8)get_pdu_ptr(peer_buff_ptr, &pdu_len);
    WNDRV_ASSERT((pdu_len == u2PacketLen));
    kalMemCopy(pdu_ptr, pvPacket, u2PacketLen);

    /* SEND ILM */
    ilm_ptr = allocate_ilm(MOD_WNDRV);
    ilm_ptr->msg_id = MSG_ID_HOSTAP_WNDRV_FRAME_IND;
    ilm_ptr->peer_buff_ptr = peer_buff_ptr;
    ilm_ptr->local_para_ptr = (local_para_struct *)ind_ptr;

    /* Send the message to the MOD_HOSTAP module */
    SEND_ILM(MOD_WNDRV, MOD_HOSTAP, HOSTAP_WNDRV_SAP, ilm_ptr);

    return;
}

/*******************************************************************************
* FUNCTION
*   flc_wndrv_ul_sspdu_resume_handler
*
* DESCRIPTION
*   MSG_ID_FLC_WNDRV_UL_SSPDU_RESUME
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
void flc_wndrv_ul_sspdu_resume_handler(ilm_struct *ilm_ptr)
{
    flc_wndrv_ul_sspdu_resume_struct *sspdu_resume_ptr;
    //kal_uint32 u4RegVal;

    sspdu_resume_ptr = (flc_wndrv_ul_sspdu_resume_struct *)ilm_ptr->local_para_ptr;

    if (FLC2_POOL_ID_SPDU_HS == sspdu_resume_ptr->sspdu_id) {
        MT5931_TRACE0(TRACE_HOTSPOT, MT5931_INFO_284, "resume SPDU_HS");
        WNDRVPauseFlag = KAL_FALSE;
    }

    return;
}

/*******************************************************************************
* FUNCTION
*   wndrv_abm_set_tx_pwr_req_handler
*
* DESCRIPTION
*   MSG_ID_WNDRV_ABM_SET_TX_PWR_REQ
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
void wndrv_abm_set_tx_pwr_req_handler(ilm_struct *ilm_ptr)
{
    wndrv_abm_set_tx_pwr_req_struct *req_ptr;
    wndrv_abm_set_tx_pwr_cnf_struct *cnf_ptr;
    ilm_struct *ilm_p;

    req_ptr = (wndrv_abm_set_tx_pwr_req_struct *)ilm_ptr->local_para_ptr;

    cnf_ptr = (wndrv_abm_set_tx_pwr_cnf_struct *)
              construct_local_para(sizeof(wndrv_abm_set_tx_pwr_cnf_struct), TD_CTRL);

    if (req_ptr->mode == 1) {
        wtUpdateTxPower(wndrv_context.adpt_p, req_ptr->power);

        cnf_ptr->status = KAL_TRUE;
    } else {
        /* not support in 5931 now */
        cnf_ptr->status = KAL_FALSE;
    }

    /* SEND ILM */
    ilm_p = allocate_ilm(MOD_WNDRV);
    ilm_p->src_mod_id = MOD_WNDRV;
    ilm_p->dest_mod_id = MOD_ABM;
    ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_TX_PWR_CNF;
    ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
    ilm_p->peer_buff_ptr = NULL;

    msg_send_ext_queue(ilm_p);
}

#endif

