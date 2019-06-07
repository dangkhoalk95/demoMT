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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/os/linux/gl_kal.c#78 $
*/

/*! \file   gl_kal.c
    \brief  GLUE Layer will export the required procedures here for internal driver stack.

    This file contains all routines which are exported from GLUE Layer to internal
    driver stack.
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
#include "gl_kal.h"

#if 0//#if defined(__VOIP__) /* WCP Modification */
#include "evinf.h"
extern void network_event_callback(EVINF_INFO_ID id, kal_uint32 parm);
#endif

//extern const UINT_8 aucPriority2TC[];

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
UINT_8 wndrv_io_buf_pool[(CFG_COALESCING_BUFFER_SIZE >= CFG_RX_COALESCING_BUFFER_SIZE) ? \
                         ALIGN_32(CFG_COALESCING_BUFFER_SIZE) + ALIGN_32(sizeof(ENHANCE_MODE_DATA_STRUCT_T)) + 32 : \
                         ALIGN_32(CFG_RX_COALESCING_BUFFER_SIZE) + ALIGN_32(sizeof(ENHANCE_MODE_DATA_STRUCT_T)) + 32];


/* NOTE(lcko): These two variable are used to record allocated packet address */
UINT_8 u4PktAllocIndex = 0;
PVOID v4PktAllocRecord [255];


/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
static PVOID pvIoBuffer = NULL;
static UINT_32 pvIoBufferSize = 0;
static UINT_32 pvIoBufferUsage = 0;

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


/*******************************************************************************
*                  W C P   M O D I F I C A T I O N  S T A R T
********************************************************************************
*/


#include "soc2tcpip_struct.h"
#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"
#include "wndrv_tcpip_msg.h"
#include "wndrv_os_private.h"
#include "wndrv_context.h"
#include "wndrv_abm_msg.h"
//#define __WNDRV_OS_FILES_C__
/*
#include "app_ltlcom.h"
#include "wndrv_os_private.h"
#include "wndrv_cnst.h"

#include "precomp.h"
#include "gl_typedef.h"

#include "wndrv_context.h"
*/
/*----------------------------*/
extern kal_uint32 L1I_GetTimeStamp(void);

/* ------------------------------------------------------------------------- */
extern wndrv_context_struct wndrv_context;
extern kal_bool             WNDRVTimerEnable;
extern kal_bool             WNDRVBearerEnable;
extern kal_bool             WNDRVPauseFlag;
extern kal_bool             WNDRVNoFLC2Flag;
extern int wifi_under_cam_mode;

#if defined(MT6250)
int rx_flow_ctrl_no_flc = 0;
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

PVOID
kalMemAlloc(IN UINT_32 u4Size, IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType)
{
    return wifi_get_ctrl_buffer(u4Size, (void *)__func__);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

VOID
kalMemFree(IN PVOID pvAddr,
           IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType,
           IN UINT_32 u4Size)
{
    wifi_free_ctrl_buffer(pvAddr, (void *)__func__);
}


void kalUdelay(UINT_32 u4MicroSec)
{
    if (u4MicroSec == 0) {
        return;
    }

    if (u4MicroSec < 1000) {
        u4MicroSec = 1;
    } else {
        u4MicroSec = u4MicroSec / 1000;
    }
    kalMsleep(u4MicroSec);  //should be replaced with loop wait
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code = "SECONDARY_ROCODE"
#endif

VOID
kalMdelay(IN UINT_32 u4MilliSec)
{
    UINT_32    i;

    for (i = 0; i < u4MilliSec; i++) {
        kalUdelay(1000);
    }

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_KAL_M_DELAY,
                 "WKAL: Mdelay=%d", u4MilliSec);

}
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code
#endif
/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

VOID
kalMsleep(IN UINT_32 u4MilliSec)
{
    kal_sleep_task(u4MilliSec);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

INT_32
kalMemCmp(IN PVOID pvAddr1,
          IN PVOID pvAddr2,
          IN UINT_32 u4Size)
{
    return kal_mem_cmp(pvAddr1, pvAddr2, u4Size);
}

/* DEBUG FUNCTION */
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

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

VOID
kalSendComplete(IN P_GLUE_INFO_T prGlueInfo,
                IN PVOID pvPacket,
                IN INT_32 i4Status)
{
    peer_buff_struct             *peer_p = (peer_buff_struct *)pvPacket;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;

    WNDRV_ASSERT(pvPacket);
    {
        /****************************************************************************
        *  1. Get packet descriptor in free header
        ****************************************************************************/
        wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

        WNDRV_EXT_ASSERT((wndrv_pkt_p->src_mod_id == MOD_SUPC) || (wndrv_pkt_p->src_mod_id == MOD_TCPIP),
                         "kalSendComplete: src_mod_id must be SUPC or TCPIP",
                         wndrv_pkt_p->src_mod_id, 0, 0);
        /****************************************************************************
        *  2. Free transmitted peer buffer and GLUE info
        ****************************************************************************/

        kalSendCompleteAndAwakeQueue(prGlueInfo, pvPacket);

        // kalMemFree( wndrv_pkt_p->prPacketInfo, sizeof(PACKET_INFO_T) );

        if (wndrv_pkt_p->flc_mode == 0) {
            /* SUPC packet TX */
            //free_peer_buff( peer_p );
            free_peer_buff(peer_p);
            wndrv_context.glue_p->tx_sys_peer_num--;
        }
#if defined (WLAN_ZERO_COPY)
        else if (wndrv_pkt_p->flc_mode == 1) { //MOD_TCPIP
            /* TCPIP packet */
#if CFG_ENABLE_THROUGHPUT_DBG
            SLA_CustomLogging("FPB", SA_start);
#endif
            flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);

            wndrv_context.glue_p->tx_flc_peer_num--;
            // James modify for optimization
            //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_SEND_COMPLETE_FLC,
            //              "WKAL: SendComplete: tx_flc_peer_num=%d", wndrv_context.tx_flc_peer_num );
#if CFG_ENABLE_THROUGHPUT_DBG
            SLA_CustomLogging("FPB", SA_stop);
#endif
        }
#endif
        else {
            WNDRV_ASSERT(0);
        }

        //wndrv_context.glue_p->u4TxPendingFrameNum--;

        /****************************************************************************
        *  3. Handle pending tx packets.
        ****************************************************************************/
    }
}

#if 0
/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

PUINT_8
kalQueryBufferPointer(IN P_GLUE_INFO_T prGlueInfo,
                      IN PVOID pvPacket)
{

    peer_buff_struct  *peer_buff_ptr = (peer_buff_struct *) pvPacket;
    kal_uint16        pdu_len;

    return ((PUINT_8)(get_pdu_ptr(peer_buff_ptr, &pdu_len)));

}
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

VOID
kalCopyFrame(IN P_GLUE_INFO_T prGlueInfo,
             IN PVOID pvPacket, IN OUT PUINT_8  pucDestBuffer)
{

    peer_buff_struct  *peer_buff_ptr = (peer_buff_struct *) pvPacket;
    kal_uint16        pdu_len;
    kal_uint8         *data_p;

    data_p = (PUINT_8)(get_pdu_ptr(peer_buff_ptr, &pdu_len));

    kalMemCopy(pucDestBuffer, data_p, pdu_len);

}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

UINT_32
kalGetTimeTick(VOID)
{
    UINT_32 tick;

#if 0   /* Nelson(Note): query current time ticks */
    kal_get_time(&tick);
#else
    tick = L1I_GetTimeStamp(); //timer unit is gsm frame tick
#endif
    //WNDRV_TRACE1( TRACE_GROUP_2, INFO_WNDRV_KAL_GET_TICK,
    //              "WKAL: GetTick=%d", tick );
    return tick;
}

#if 0
/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

/* WCP Modification */ BOOL
kalCheckDevRegWR(
    IN P_GLUE_INFO_T  prGlueInfo,
    IN UINT_32        u4Register,
    IN UINT_32        u4Value,
    IN UINT_32        u4Count
)
{
    UINT_32 i;
    UINT_32 rValue = 0;

    for (i = 0; i < u4Count ; i++) {

        //kalDevRegWrite(prGlueInfo, u4Register, u4Value);
        //kalDevRegRead(prGlueInfo, u4Register, &rValue);
        kalDevRegWrite(u4Register, u4Value);
        kalDevRegRead(u4Register, &rValue);

        WNDRV_ASSERT(u4Value == rValue);

    }

    return TRUE;
}
#endif

/*******************************************************************************
* FUNCTION
*   wndrv_supc_get_pkt_descriptor
*
* DESCRIPTION
*   Get packet descriptor in peer buffer
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
wndrv_pkt_descriptor_struct *wndrv_get_pkt_descriptor(peer_buff_struct *peer_p)
{
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;
    kal_uint8                    *data_p;
    kal_uint16                    pdu_len;

    data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);

    wndrv_pkt_p = (wndrv_pkt_descriptor_struct *)(data_p - peer_p->free_header_space);

    /* NOTE(Nelson): peer_p->free_header_space should be large enough to put wndrv_pkt_descriptor_struct info */
    WNDRV_EXT_ASSERT(peer_p->free_header_space >= sizeof(wndrv_pkt_descriptor_struct),
                     "wndrv_get_pkt_descriptor: peer_p->free_header_space must greater than wndrv_pkt_descriptor_struct",
                     peer_p->free_header_space, sizeof(wndrv_pkt_descriptor_struct), (UINT_32)peer_p);

    WNDRV_EXT_ASSERT(((kal_uint32)wndrv_pkt_p & 0x03) == 0x00,
                     "wndrv_get_pkt_descriptor: wndrv_pkt_p must be 4-byte alignment",
                     (kal_uint32)wndrv_pkt_p, 0, 0);

    return wndrv_pkt_p;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
*
* \param[in]
* \param[out]
*
* \return
*/
/*----------------------------------------------------------------------------*/

VOID
kalIndicatePackets(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID *apvPkts
)
{
    ilm_struct                  *ilm_p;
    peer_buff_struct            *peer_p = (peer_buff_struct *) apvPkts;
    wndrv_pkt_descriptor_struct *wndrv_pkt_p;
    kal_uint8                   *data_p;
    kal_uint16                   pdu_len, proto_type;
    kal_uint8                    src_mac_addr[WNDRV_MAC_ADDRESS_LEN], dest_mac_addr[WNDRV_MAC_ADDRESS_LEN];
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    BOOLEAN                      fgIsP2P = FALSE;
#endif

#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("di1", SA_start);
#endif
    /****************************************************************************
    *  1. Construct local param and fill proto type
    ****************************************************************************/
    //ILMTraceCheck(110);
    data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);

    kal_mem_cpy(dest_mac_addr, data_p, WNDRV_MAC_ADDRESS_LEN);
    data_p += WNDRV_MAC_ADDRESS_LEN;

    kal_mem_cpy(src_mac_addr, data_p, WNDRV_MAC_ADDRESS_LEN);
    data_p += WNDRV_MAC_ADDRESS_LEN;

    proto_type = ((*data_p << 8) & 0xff00) | (*(data_p + 1) & 0xff);


    /****************************************************************************
    *  2. Remove Ethernet MAC header
    ****************************************************************************/
    if ((0x888E == proto_type) ||
#ifdef __WAPI_SUPPORT__
            (0x88B4 == proto_type) || //WAPI
#endif /*__WAPI_SUPPORT__*/
            (0x88C7 == proto_type)) {        /* 802.1x packet */
        remove_hdr_of_peer_buff(peer_p, WNDRV_ETHERNET_MAC_SPACE);
    }


    /****************************************************************************
    *  3. Get Q node in free header
    ****************************************************************************/
    wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    fgIsP2P = GLUE_GET_PKT_IS_P2P(peer_p);
#endif

    WNDRV_EXT_ASSERT(wndrv_pkt_p->check_mark == WNDRV_BUFFER_HDLR_CHECK_MARK,
                     "kalIndicatePackets: wndrv_pkt_p check mark error",
                     (kal_uint32)wndrv_pkt_p, wndrv_pkt_p->check_mark, 0);

    {
        WNDRV_EXT_ASSERT(wndrv_pkt_p->src_mod_id == MOD_WNDRV, //MOD_WNDRV
                         "kalIndicatePackets: src_mod_id should be MOD_WNDRV",
                         wndrv_pkt_p->src_mod_id, 0, 0);
    }
#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("di1", SA_stop);
#endif

    /****************************************************************************
    *  4. Send packet to upper layer
    ****************************************************************************/
    if ((0x888E == proto_type) ||
#ifdef __WAPI_SUPPORT__
            (0x88B4 == proto_type) || //WAPI
#endif /*__WAPI_SUPPORT__*/
            (0x88C7 == proto_type)) {        /* 802.1x packet */
#if CFG_ENABLE_WIFI_TETHERING
        if ((wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable) && (fgIsP2P)) {
            /* send 4way handshake to hostap
            */
            WLAN_MAC_HEADER_T WlanHeader;
            UCHAR rfc1042_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 }; /* Ethernet-II snap header (RFC1042 for most EtherTypes) */
            UCHAR eth_p_pae[2] = { 0x88, 0x8e};
            PUINT_8 pdu_ptr;
            UINT_16 pdu_len;


            /* fill WlanHeader TO AP */
            kalMemZero((void *)&WlanHeader, sizeof(WLAN_MAC_HEADER_T));
            COPY_MAC_ADDR(WlanHeader.aucAddr1, dest_mac_addr); /* BSSID , BSSID = DA is MAUI */
            COPY_MAC_ADDR(WlanHeader.aucAddr2, src_mac_addr);  /* SA */
            COPY_MAC_ADDR(WlanHeader.aucAddr3, dest_mac_addr); /* DA */

            WlanHeader.u2FrameCtrl |= (MAC_FRAME_TYPE_DATA | MASK_FC_TO_DS);

            /* compose 802.11 packet : ieee80211_hdr + rfc1042_header + ETH_P_PAE + data(peer_p) */

            /* 1. ETH_P_PAE + data */
            prepend_to_peer_buff(peer_p, (PVOID)eth_p_pae, sizeof(eth_p_pae));

            /* 2. rfc1042_header + ETH_P_PAE + data */
            prepend_to_peer_buff(peer_p, (PVOID)rfc1042_header, sizeof(rfc1042_header));

            /* 3. ieee80211_hdr + rfc1042_header + ETH_P_PAE + data */
            prepend_to_peer_buff(peer_p, (PVOID)&WlanHeader, sizeof(WlanHeader));

            pdu_ptr = (PVOID)get_pdu_ptr(peer_p, &pdu_len);

            //_wpa_hexdump("[TETHERING] RX EAPOL:", (char *)pdu_ptr, pdu_len, 1);
            //ILMTraceCheck(111);
            /* send to hostap */
            hostap_wndrv_frame_ind_handler(pdu_ptr, pdu_len);

            flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);
        } else
#endif /* CFG_ENABLE_WIFI_TETHERING */
        {
            wndrv_supc_data_ind_struct  *ind_p;

            /*   ind_p = (wndrv_supc_data_ind_struct *)
               construct_local_para( sizeof( wndrv_supc_data_ind_struct ), TD_CTRL ); */
            ind_p = (wndrv_supc_data_ind_struct *)
                    construct_local_para(sizeof(wndrv_supc_data_ind_struct), TD_CTRL);
            kal_mem_cpy(ind_p->dest_mac_addr, dest_mac_addr, WNDRV_MAC_ADDRESS_LEN);
            kal_mem_cpy(ind_p->src_mac_addr, src_mac_addr, WNDRV_MAC_ADDRESS_LEN);
            ind_p->proto_type = proto_type;
#if defined (WLAN_ZERO_COPY)
            if (wndrv_pkt_p->flc_mode == 1) {
                peer_buff_struct  *peer_copy_p;
                kal_uint8         *data_copy_p;

                data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);
                /*peer_copy_p = construct_peer_buff ( pdu_len, 0, 0, TD_DL );*/
                peer_copy_p = construct_peer_buff(pdu_len, 0, 0, TD_DL);

                WNDRV_EXT_ASSERT(peer_copy_p != NULL,
                                 "kalIndicatePackets: system buffer not available",
                                 pdu_len, 0, 0);

                data_copy_p = (kal_uint8 *)get_pdu_ptr(peer_copy_p, &pdu_len);
                kalMemCopy(data_copy_p, data_p, pdu_len);
#if 1//#ifdef  __FLC2__
                flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p); /* STA MODE no need change to HS POOL */
#else
                gprs_flc_free_peer_buff(SPDU_DATA,         /* data type */
                                        TCP_OVER_WIFI,    /* sapi type */
                                        GPRS_FLC_DL,      /* dir */
                                        0,                /* context id */
                                        peer_p);          /* buff ptr */
#endif

                peer_p = peer_copy_p;

                wndrv_context.glue_p->rx_flc_peer_num--;
                // James modify for optimization
                //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_IND_PACKETS_FLC,
                //              "WKAL: IndPackets: rx_flc_peer_num=%d", wndrv_context.rx_flc_peer_num );
            } else
#endif
            {
                wndrv_context.glue_p->rx_sys_peer_num--;
                // James modify for optimization
                //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_IND_PACKETS_SYS,
                //              "WKAL: IndPackets: rx_sys_peer_num=%d", wndrv_context.rx_sys_peer_num );
            }

            ilm_p                 = allocate_ilm(MOD_WNDRV);   //MOD_WNDRV
            ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_DATA_IND; //MSG_ID_WNDRV_SUPC_DATA_IND
            ilm_p->local_para_ptr = (local_para_struct *) ind_p;
            ilm_p->peer_buff_ptr  = peer_p;
            WNDRV_SEND_MSG_TO_SUPC(ilm_p);
        }
    } else { /* other packets */
        if (KAL_FALSE == WNDRVBearerEnable) {
            WNDRV_TRACE0(TRACE_WARNING, WARNING_NO_WIFI_BEARAR,
                         "No WiFi bearer");
            kalPacketFree(wndrv_context.glue_p, apvPkts);
            return;
        }

        if (wndrv_pkt_p->flc_mode == 0) {
            peer_buff_struct  *peer_copy_p;
#if defined (WLAN_ZERO_COPY)
            kal_uint8         *data_copy_p;
#endif
            data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);

#if defined (WLAN_ZERO_COPY)
            // James modify for new flow control
#if CFG_ENABLE_WIFI_TETHERING
            if ((wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable) && (fgIsP2P)) {
                peer_copy_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_HS, pdu_len, WNDRV_PKT_DESCRIPTOR_SIZE + WNDRV_WLAN_MAC_HEADER_LEN + MBUF_LENGTH , 0);
            } else
#endif /* CFG_ENABLE_WIFI_TETHERING */
            {

#ifdef  __WIFI_AP_STA_CONCURRENCY__
                peer_copy_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_HS, pdu_len, MBUF_LENGTH, 0);  /* STA MODE no need change to HS POOL */
#else
                peer_copy_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_DL, pdu_len, MBUF_LENGTH, 0);  /* STA MODE no need change to HS POOL */
#endif

            }
#else
            peer_copy_p = peer_p;

#endif

            if (peer_copy_p == NULL) {
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_WARNING, "No FLC2 buffer,Drop packet");

                /* NOTE(lcko): call kalPacketFree is equlivent to free_peer_buff plus
                other cleanup procedure.
                */
                kalPacketFree(wndrv_context.glue_p, apvPkts);
                return;
            }
#if defined (WLAN_ZERO_COPY)
            data_copy_p = (kal_uint8 *)get_pdu_ptr(peer_copy_p, &pdu_len);
            kalMemCopy(data_copy_p, data_p, pdu_len);

            //free_peer_buff( peer_p );
            free_peer_buff(peer_p);
            WNDRVNoFLC2Flag = KAL_FALSE;
            peer_p = peer_copy_p;
#endif
            wndrv_context.glue_p->rx_sys_peer_num--;
            // James modify for optimization
            //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_IND_PACKETS_SYS,
            //             "WKAL: IndPackets: rx_sys_peer_num=%d", wndrv_context.rx_sys_peer_num );
        } else { /* flc control buffer */
            wndrv_context.glue_p->rx_flc_peer_num--;
            // James modify for optimization
            //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_IND_PACKETS_FLC,
            //             "WKAL: IndPackets: rx_flc_peer_num=%d", wndrv_context.rx_flc_peer_num );
        }

        wlan_rx_pkt(peer_p);

    }
    //ILMTraceCheck(119);
    apvPkts = NULL;
}

VOID
kalOidComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN BOOLEAN fgSetQuery,
    IN UINT_32 u4SetQueryInfoLen,
    IN WLAN_STATUS rOidStatus
)
{

}

VOID
kalOidClearance(
    IN P_GLUE_INFO_T prGlueInfo
)
{

}


/*******************************************************************************
*                  W C P   M O D I F I C A T I O N  E N D
********************************************************************************
*/


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is provided by GLUE Layer for internal driver stack to acquire
*        OS SPIN_LOCK.
*
* \param[in] prGlueInfo     Pointer of GLUE Data Structure
* \param[in] rLockCategory  Specify which SPIN_LOCK
* \param[out] pu4Flags      Pointer of a variable for saving IRQ flags
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
kalAcquireSpinLock(
    IN P_GLUE_INFO_T                prGlueInfo,
    IN ENUM_SPIN_LOCK_CATEGORY_E    rLockCategory
)
{

} /* end of kalAcquireSpinLock() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is provided by GLUE Layer for internal driver stack to release
*        OS SPIN_LOCK.
*
* \param[in] prGlueInfo     Pointer of GLUE Data Structure
* \param[in] rLockCategory  Specify which SPIN_LOCK
* \param[in] u4Flags        Saved IRQ flags
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
kalReleaseSpinLock(
    IN P_GLUE_INFO_T                prGlueInfo,
    IN ENUM_SPIN_LOCK_CATEGORY_E    rLockCategory
)
{

} /* end of kalReleaseSpinLock() */


VOID
kalPacketFree(
    IN  P_GLUE_INFO_T       prGlueInfo,
    IN PVOID pvPacket
)
{
    /* WCP modification: use 5911's kalPacketFree here */

    peer_buff_struct             *peer_p = (peer_buff_struct *)pvPacket;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;
    wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

    /* All RX and management TX */
    {
        WNDRV_EXT_ASSERT(wndrv_pkt_p->src_mod_id == MOD_WNDRV,
                         "kalPacketFree: src_mod_id must be MOD_WNDRV ",
                         wndrv_pkt_p->src_mod_id, 0, 0);
    }

    if (wndrv_pkt_p->flc_mode == 0) {
        /* Management TX or RX without flc */
        //free_peer_buff( peer_p );
        free_peer_buff(peer_p);

        if (wndrv_pkt_p->pool_id == 0) {
            wndrv_context.glue_p->tx_sys_peer_num--;
            // James modify for optimization
            //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_FREE_SYS,
            //          "WKAL: PacketFree: tx_sys_peer_num=%d", wndrv_context.tx_sys_peer_num );

        } else if (wndrv_pkt_p->pool_id == 1) {
            wndrv_context.glue_p->rx_sys_peer_num--;
            // James modify for optimization
            //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_FREE_SYS,
            //          "WKAL: PacketFree: rx_sys_peer_num=%d", wndrv_context.rx_sys_peer_num );

        }
    }
// James modify for new flow control
#if defined (WLAN_ZERO_COPY)
    else if (wndrv_pkt_p->pool_id == FLC2_POOL_ID_SPDU_DL) {
        /* All RX */
        flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);
        wndrv_context.glue_p->rx_flc_peer_num--;

        // James modify for optimization
        //WNDRV_TRACE1( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_FREE_FLC,
        //              "WKAL: PacketFree: rx_flc_peer_num=%d", wndrv_context.rx_flc_peer_num );
    }
#if 1
    else if (wndrv_pkt_p->pool_id == FLC2_POOL_ID_SPDU_HS) {
        /*AP MODE */
        flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);
        wndrv_context.glue_p->rx_flc_peer_num--;
    }
#endif /* CFG_ENABLE_WIFI_TETHERING */
    else {
#if defined(__HOTSPOT_SUPPORT__)
        if (wndrv_pkt_p->src_mod_id == MOD_TCPIP) {
            flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);
            wndrv_context.glue_p->rx_flc_peer_num--;
        } else
#endif /* __HOTSPOT_SUPPORT__ */
        {
            WNDRV_ASSERT(0);
        }
    }
#endif
    pvPacket = NULL;
}

#if 0
/*******************************************************************************
**  kalPacketAlloc
**
**  descriptions:
**      Only handles driver own creating packet (coalescing buffer).
**  parameters:
**      pvAdapter - pointer to adapter descriptor
**      u4Size - needed buffer size
**  result:
**      *ppucData - pointer to packet starting address
**  return:
**      pointer to allocated packet descriptor. NULL if no buffers.
**  note:
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*!
* \brief Only handles driver own creating packet (coalescing buffer).
*
* \param prGlueInfo     Pointer of GLUE Data Structure
* \param u4Size       Pointer of Packet Handle
* \param ppucData         Status Code for OS upper layer
*
* \return NULL: Failed to allocate skb, Not NULL get skb
*/
/*----------------------------------------------------------------------------*/

/* NOTE(lcko): These two variable are used to record allocated packet address */
UINT_8 u4PktAllocIndex = 0;
PVOID v4PktAllocRecord [255];

PVOID
kalPacketAlloc(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32 u4Size,
    OUT PUINT_8 *ppucData
)
{

    /* WCP modification: use 5911's kalPacketFree here */

    peer_buff_struct             *peer_p;
    static volatile PVOID record = NULL;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;
    kal_uint16                    pdu_len;
#if defined(__HOTSPOT_SUPPORT__)
    P_PACKET_INFO_T               prPacketInfo;
#endif /* __HOTSPOT_SUPPORT__ */
    DEBUGFUNC("kalPacketAlloc");
    if (u4Size != CFG_RX_ALLOC_PKT_SIZE) {
#ifdef WCP_MAUI /* WCP Modification */
        WNDRV_EXT_ASSERT(0, "Packet Alloc Size Err: %d", u4Size, 0, 0);
#else
        WNDRV_ASSERT(0);
#endif
    }

    { /* management packet RX, TCPIP packet RX and SUPC packet RX */

#if defined(__HOTSPOT_SUPPORT__)
        if (wndrv_context.glue_p->wlanDriveMode == WLAN_DRIVER_AP_MODE) {
            peer_p = flc2_get_peer_buff(FLC2_POOL_ID_SPDU_HS, u4Size, WNDRV_PKT_DESCRIPTOR_SIZE + WNDRV_WLAN_MAC_HEADER_LEN + MBUF_LENGTH , 0);
        } else
#endif /*__HOTSPOT_SUPPORT__*/
        {
            peer_p = flc2_get_peer_buff(FLC2_POOL_ID_SPDU_DL, u4Size, WNDRV_PKT_DESCRIPTOR_SIZE + MBUF_LENGTH, 0);
        }

        if (peer_p == NULL) {
            /* DEBUG */
            P_TX_CTRL_T prTxCtrl;
            P_RX_CTRL_T prRxCtrl;
            int idx;

            prTxCtrl = &wndrv_context.adpt_p->rTxCtrl;
            prRxCtrl = &wndrv_context.adpt_p->rRxCtrl;

            for (idx = TXQ_AC0; idx < TXQ_NUM; idx++) {
                MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "TXQ %d [NUM]: %d %d %d %d %d %d\n\r", idx,
                             (int) prTxCtrl->arTcQPara[idx].ucMaxNumOfMsduInfo,
                             (int) prTxCtrl->arTcQPara[idx].rFreeMsduInfoList.u4NumElem,
                             (int) prTxCtrl->arTcQPara[idx].rOsSendQueue.u4NumElem,
                             (int) prTxCtrl->arTxACQPara[idx].rSendWaitQueue.u4NumElem,
                             (int) prTxCtrl->arTxACQPara[idx].rActiveChainList.u4NumElem,
                             (int) prTxCtrl->arTxACQPara[idx].rFreeTFCBList.u4NumElem);
                MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "TXQ %d [size]: %d %d %d", idx,
                             DWORD_TO_BYTE(prTxCtrl->arTxACQPara[idx].u2MaxBufferSizeDW),
                             DWORD_TO_BYTE(prTxCtrl->arTxACQPara[idx].u2FreeBufferSizeDW),
                             prTxCtrl->arTxACQPara[idx].ucMaxNumOfSwTfcb);
            }
            MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "RFB: %d %d %d",
                         prRxCtrl->rFreeRFBWithBuffList.u4NumElem,
                         prRxCtrl->rFreeRFBWOBuffList.u4NumElem,
                         prRxCtrl->rReceivedRFBList.u4NumElem);
            /* DEBUG */
            WNDRV_TRACE0(TRACE_WARNING, WARNING_NO_FLC_BUFFER,
                         "Cannot allocate flow control buffer");
            //kal_sleep_task(1);
            WNDRVPauseFlag = KAL_TRUE;
            WNDRVNoFLC2Flag = KAL_TRUE;
            /*peer_p = construct_peer_buff ( u4Size, WNDRV_PKT_DESCRIPTOR_SIZE, 0, TD_UL );*/
#if defined(__HOTSPOT_SUPPORT__)
            if (wndrv_context.glue_p->wlanDriveMode == WLAN_DRIVER_AP_MODE) {
                kal_uint32 u4RegVal;
                peer_p = construct_peer_buff(u4Size, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
                /* disable RX interrupt */
                HAL_MCR_RD(wndrv_context.adpt_p, MCR_HIER, &u4RegVal);
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "kalPacketAlloc construct_peer_buff: %x %d MCR_HIER %x", peer_p, u4Size, u4RegVal);
                u4RegVal &= ~HIER_RX_DONE;
                HAL_MCR_WR(wndrv_context.adpt_p, MCR_HIER, u4RegVal);
            } else
#endif /*__HOTSPOT_SUPPORT__*/
            {
                peer_p = construct_peer_buff(u4Size, WNDRV_PKT_DESCRIPTOR_SIZE, 0, TD_UL);
            }
            WNDRV_EXT_ASSERT(peer_p != NULL,
                             "kalPacketAlloc: system buffer not available",
                             u4Size, 0, 0);

            wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

            wndrv_pkt_p->src_mod_id = MOD_WNDRV;
            wndrv_pkt_p->flc_mode   = 0;
            wndrv_pkt_p->pool_id    = 1;
            wndrv_pkt_p->check_mark = WNDRV_BUFFER_HDLR_CHECK_MARK;
            wndrv_pkt_p->reserved0  = 0; /* ADD FOR HOSTAP */
            wndrv_pkt_p->tx_peer_p  = peer_p;


            prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);
            prPacketInfo->wndrv_pkt_p = wndrv_pkt_p;
            prPacketInfo->prPacket = (P_NATIVE_PACKET)peer_p;
            prPacketInfo->ucMacHeaderLength = WLAN_MAC_HEADER_LEN;
            prPacketInfo->u2PayloadLength = pdu_len - WLAN_MAC_HEADER_LEN;
            wndrv_pkt_p->prPacketInfo = prPacketInfo;

            wndrv_context.glue_p->rx_sys_peer_num++;

            // James modify for optimization
            //WNDRV_TRACE2( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_ALLOC_SYS,
            //              "WKAL: PacketAlloc: rx_sys_peer_num=%d, pdu_len=%d",
            //              wndrv_context.rx_sys_peer_num, u4Size );
        } else {
            wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

            wndrv_pkt_p->src_mod_id    = MOD_WNDRV;
// James modify for new flow control
#if defined(__HOTSPOT_SUPPORT__)
            if (wndrv_context.glue_p->wlanDriveMode == WLAN_DRIVER_AP_MODE) {
                wndrv_pkt_p->pool_id = FLC2_POOL_ID_SPDU_HS;
            } else
#endif /* __HOTSPOT_SUPPORT__ */
            {
                wndrv_pkt_p->pool_id = FLC2_POOL_ID_SPDU_DL;
            }
            wndrv_pkt_p->flc_mode      = 1;
            wndrv_pkt_p->check_mark    = WNDRV_BUFFER_HDLR_CHECK_MARK;
            wndrv_pkt_p->reserved0  = 0; /* ADD FOR HOSTAP */
            wndrv_pkt_p->tx_peer_p  = peer_p;

            prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);

            prPacketInfo->wndrv_pkt_p = wndrv_pkt_p;
            prPacketInfo->prPacket = (P_NATIVE_PACKET)peer_p;
            prPacketInfo->ucMacHeaderLength = WLAN_MAC_HEADER_LEN;
            prPacketInfo->u2PayloadLength = pdu_len - WLAN_MAC_HEADER_LEN;
            wndrv_pkt_p->prPacketInfo = prPacketInfo;


            wndrv_context.glue_p->rx_flc_peer_num++;

            // James modify for optimization
            //WNDRV_TRACE2( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_ALLOC_FLC,
            //              "WKAL: PacketAlloc: rx_flc_peer_num=%d, pdu_len=%d",
            //              wndrv_context.rx_flc_peer_num, u4Size );
        }
    }
    // James modify for catcher, end
    WNDRV_EXT_ASSERT(peer_p != NULL, "kalPacketAlloc: buffer can't be NULL", 0, 0, 0);

    u4PktAllocIndex ++;
    u4PktAllocIndex %= 255;
    v4PktAllocRecord [u4PktAllocIndex] = peer_p;

    if (peer_p) {
        record = peer_p;
        *ppucData = (kal_uint8 *) get_pdu_ptr(peer_p, &pdu_len);

        if (wndrv_pkt_p->flc_mode == 1) {
            MT5931_TRACE(TRACE_GROUP_8, MT5931_INFO_, "kalPacketAlloc : peer_p %x addr %x: %x",
                         peer_p, (kal_uint32 *)(*ppucData + CFG_RX_ALLOC_PKT_SIZE), *((kal_uint32 *)(*ppucData + CFG_RX_ALLOC_PKT_SIZE)) & 0xFFFF);
            WNDRV_ASSERT((*((kal_uint32 *)(*ppucData + CFG_RX_ALLOC_PKT_SIZE)) & 0xFFFF) == 0xF3F4);
        }

        kalMemSet(*ppucData, u4PktAllocIndex, CFG_RX_ALLOC_PKT_SIZE);
    }

    return (PVOID)peer_p;

}
#endif

#ifdef CFG_USE_SW_ROOT_TIMER
/*----------------------------------------------------------------------------*/
/*!
* \brief Get the packet from swrfb and do something for retained packet.
*        There is nothing to do for Linux packet.
*
* \param[in] prGlueInfo Pointer to the Adapter structure.
* \param[in] prSWRfb The software RFB
* \param[in] pfgIsRetain Indicate that the packet of RFB is retained or not.
*
* \retval TRUE Success.
*
*/
/*----------------------------------------------------------------------------*/
VOID
kalOsTimerInitialize(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID prTimerHandler
)
{
    struct timer_list *prTimer = &prGlueInfo->rMasterTimer;

    /* Setup master timer. This master timer is the root timer for following
       management timers. */

    init_timer(prTimer);
    prTimer->function = (PFN_LINUX_TIMER_FUNC)prTimerHandler;
    prTimer->data = (unsigned long)(prGlueInfo->prAdapter);

    INITLOG(("Init SW master Timer -- OK\n"));
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to set the time to do the time out check.
*
* \param[in] pvAdapter Pointer to the Adapter structure.
* \param[in] u4Interval Time out interval from current time.
*
* \retval TRUE Success.
*
*/
/*----------------------------------------------------------------------------*/
BOOL
kalSetTimer(
    IN P_GLUE_INFO_T prGlueInfo,
    IN OS_SYSTIME rInterval
)
{
    struct timer_list *prTimer = &prGlueInfo->rMasterTimer;

    del_timer(prTimer);

    prTimer->expires = jiffies + rInterval;

    add_timer(prTimer);

    return TRUE; /* success */
} /* end of kalSetTimer() */
#endif /* CFG_USE_SW_ROOT_TIMER */


/*----------------------------------------------------------------------------*/
/*!
* \brief Process the received packet for indicating to OS.
*
* \param[in] prGlueInfo     Pointer to the Adapter structure.
* \param[in] pvPacket       Pointer of the packet descriptor
* \param[in] pucPacketStart The starting address of the buffer of Rx packet.
* \param[in] u4PacketLen    The packet length.
* \param[in] pfgIsRetain    Is the packet to be retained.
* \param[in] aerCSUM        The result of TCP/ IP checksum offload.
*
* \retval WLAN_STATUS_SUCCESS.
* \retval WLAN_STATUS_FAILURE.
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
kalProcessRxPacket(
    IN P_GLUE_INFO_T      prGlueInfo,
    IN PVOID              pvPacket,
    IN PUINT_8            pucPacketStart,
    IN UINT_32            u4PacketLen,
    //IN PBOOLEAN           pfgIsRetain,
    IN BOOLEAN            fgIsRetain,
    IN ENUM_CSUM_RESULT_T aerCSUM[]
)
{
    WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;

#if 0   /* WCP Modification */
    struct sk_buff *skb = (struct sk_buff *)pvPacket;


    skb->data = (unsigned char *)pucPacketStart;
    skb->tail = (unsigned char *)((UINT_32)pucPacketStart + u4PacketLen);
    skb->len = (unsigned int)u4PacketLen;
#else

    kal_uint8 *data_p;
    kal_uint16 pdu_len;
    peer_buff_struct *peer_p = (peer_buff_struct *) pvPacket;

    ASSERT(pvPacket);
    {
        data_p = (kal_uint8 *)get_pdu_ptr(peer_p, &pdu_len);
        peer_p->free_header_space += ((UINT_32)pucPacketStart - (UINT_32)data_p);
        peer_p->pdu_len = u4PacketLen;
    }
#endif

#if CFG_TCP_IP_CHKSUM_OFFLOAD
    kalUpdateRxCSUMOffloadParam(skb, aerCSUM);
#endif

    //*fgIsRetain = FALSE;

    return rStatus;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief To indicate an array of received packets is available for higher
*        level protocol uses.
*
* \param[in] prGlueInfo Pointer to the Adapter structure.
* \param[in] apvPkts The packet array to be indicated
* \param[in] ucPktNum The number of packets to be indicated
*
* \retval TRUE Success.
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
kalRxIndicatePkts(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PVOID apvPkts[],
    IN UINT_8 ucPktNum
)
{
    UINT_8 ucIdx = 0;
    PVOID prSkb = NULL;

    for (ucIdx = 0; ucIdx < ucPktNum; ucIdx++) {
        prSkb = apvPkts[ucIdx];
        kalIndicatePackets(prGlueInfo, prSkb);
        /* NOTE(lcko): we don't return pkt here bcs now we allocate
            buffer dynamically
        */
        //wlanReturnPacket(prGlueInfo->prAdapter, NULL);
    }

    return WLAN_STATUS_SUCCESS;
}


#if SUPPORT_WPS
/*----------------------------------------------------------------------------*/
/*!
* \brief Called by driver to indicate mgmt frame to upper layer, for example,
*        the wpa supplicant or wireless tools.
*
* \param[in] prGlueInfo Pointer to the Adapter structure.
* \param[in] ucFilterType The mgmt type
* \param[in] pvPacket The packet array to be indicated
*
* \retval TRUE Success.
*
*/
/*----------------------------------------------------------------------------*/
VOID
kalRxIndicateMgt(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          ucFilterType,
    IN PVOID            *pvPacket
)
{
    struct net_device *prNetDev = prGlueInfo->prDevHandler;

    struct sk_buff *prSkb;

    prSkb = skb_copy((struct sk_buff *)pvPacket[0], GFP_ATOMIC);

    if (prSkb == NULL) {
        printk("\n *** No skb from skb_copy()\n");
        return;
    }

    prSkb->dev = prNetDev; /* Important, crash if not correct */
    prSkb->mac.raw = prSkb->data;
    prSkb->ip_summed = CHECKSUM_NONE;
    prSkb->pkt_type = PACKET_OTHERHOST;
    prSkb->protocol = __constant_htons(0x0019);  /* ETH_P_80211_RAW */
    //dumpMemory32((PUINT_32)prSkb->data, prSkb->len);
    netif_rx(prSkb);

}
#endif


/*----------------------------------------------------------------------------*/
/*!
* \brief Called by driver to indicate event to upper layer, for example, the wpa
*        supplicant or wireless tools.
*
* \param[in] pvAdapter Pointer to the adapter descriptor.
* \param[in] eStatus Indicated status.
* \param[in] pvBuf Indicated message buffer.
* \param[in] u4BufLen Indicated message buffer size.
*
* \return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
kalIndicateStatusAndComplete(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN WLAN_STATUS      eStatus,
    IN PVOID            pvBuf,
    IN UINT_32          u4BufLen
)
{
    UINT_32 bufLen;
    //P_PARAM_STATUS_INDICATION_T pStatus = (P_PARAM_STATUS_INDICATION_T) pvBuf;
    //P_PARAM_AUTH_EVENT_T pAuth = (P_PARAM_AUTH_EVENT_T) pStatus;
    //P_PARAM_PMKID_CANDIDATE_LIST_T pPmkid =
    //   (P_PARAM_PMKID_CANDIDATE_LIST_T) (pStatus + 1);
    PARAM_MAC_ADDRESS arBssid;

    kalMemZero(arBssid, MAC_ADDR_LEN);

    ASSERT(prGlueInfo);

    switch (eStatus) {
        case WLAN_STATUS_MEDIA_CONNECT:

            prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_CONNECTED;

            /* indicate assoc event */
            wlanQueryInformation(prGlueInfo->prAdapter,
                                 wlanoidQueryBssid,
                                 &arBssid[0],
                                 sizeof(arBssid),
                                 &bufLen);

            //wext_indicate_wext_event(prGlueInfo, SIOCGIWAP, arBssid, bufLen);
            wndrv_supc_network_connect_ind_handler();

            do {
                /* print message on console */
                PARAM_SSID_T ssid;

                wlanQueryInformation(prGlueInfo->prAdapter,
                                     wlanoidQuerySsid,
                                     &ssid,
                                     sizeof(ssid),
                                     &bufLen);

                ssid.aucSsid[(ssid.u4SsidLen >= PARAM_MAX_LEN_SSID) ?
                             (PARAM_MAX_LEN_SSID - 1) : ssid.u4SsidLen ] = '\0';
#if 0
                printk(KERN_INFO "[wifi] %s netif_carrier_on [ssid:%s " MACSTR "]\n",
                       prGlueInfo->prDevHandler->name,
                       ssid.aucSsid,
                       MAC2STR(arBssid));
#endif
            } while (0);
            break;

        case WLAN_STATUS_MEDIA_DISCONNECT:
            wndrv_supc_network_disconnect_ind_handler();
            prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
            break;

        case WLAN_STATUS_MEDIA_SPECIFIC_INDICATION:
            wndrv_supc_specific_ind_handler(pvBuf);
            break;

        case WLAN_STATUS_SCAN_COMPLETE :
            wndrv_supc_site_survey_ind_handler();
            break;

#if CFG_SUPPORT_BCM && CFG_SUPPORT_BCM_BWCS
        case WLAN_STATUS_BWCS_UPDATE:
            //wext_indicate_wext_event(prGlueInfo, IWEVCUSTOM, pvBuf, sizeof(PTA_IPC_T));
            wndrv_bwcs_fw_event_handler(pvBuf);
            break;
#endif

        default:
            //WNDRV_EXT_ASSERT( 0, "kalIndicateStatusAndComplete: Unexpected eStatus", eStatus, 0, 0 );
            DBGLOG(
                RX, ERROR, ("kalIndicateStatusAndComplete: Unexpected eStatus:%d", eStatus));
            break;
    }

    if (eStatus == WLAN_STATUS_MEDIA_CONNECT) {
        prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_CONNECTED;
    } else if (eStatus == WLAN_STATUS_MEDIA_DISCONNECT) {
        prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
    }
} /* kalIndicateStatusAndComplete */


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to update the (re)association request
*        information to the structure used to query and set
*        OID_802_11_ASSOCIATION_INFORMATION.
*
* \param[in] pvAdapter Pointer to the Adapter structure.
* \param[in] pucFrameBody Pointer to the frame body of the last (Re)Association
*                         Request frame from the AP.
* \param[in] u4FrameBodyLen The length of the frame body of the last
*                           (Re)Association Request frame.
* \param[in] fgReassocRequest TRUE, if it is a Reassociation Request frame.
*
* \return (none)
*
*/
/*----------------------------------------------------------------------------*/
VOID
kalUpdateReAssocReqInfo(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PUINT_8          pucFrameBody,
    IN UINT_32          u4FrameBodyLen,
    IN BOOLEAN          fgReassocRequest
)
{

    /* WCP Modification */
#if 0
    PUINT_8             cp;

    if (fgReassocRequest) {
        if (u4FrameBodyLen < 15) {
            /*
            printk(KERN_WARNING "frameBodyLen too short:%ld\n", frameBodyLen);
            */
            return;
        }
    } else {
        if (u4FrameBodyLen < 9) {
            /*
            printk(KERN_WARNING "frameBodyLen too short:%ld\n", frameBodyLen);
            */
            return;
        }
    }

    cp = pucFrameBody;

    if (fgReassocRequest) {
        /* Capability information field 2 */
        /* Listen interval field 2*/
        /* Current AP address 6 */
        cp += 10;
        u4FrameBodyLen -= 10;
    } else {
        /* Capability information field 2 */
        /* Listen interval field 2*/
        cp += 4;
        u4FrameBodyLen -= 4;
    }

    wext_indicate_wext_event(prGlueInfo, IWEVASSOCREQIE, cp, u4FrameBodyLen);
#endif
    return;
}



/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to update the (re)association response
*        information to the structure used to query and set
*        OID_802_11_ASSOCIATION_INFORMATION.
*
* \param[in] pvAdapter Pointer to the Adapter structure.
* \param[in] pucFrameBody Pointer to the frame body of the last (Re)Association
*                         Response frame from the AP.
* \param[in] u4FrameBodyLen The length of the frame body of the last
*                          (Re)Association Response frame.
*
* \return (none)
*
* \note This information is not needed by wpa supplicant in Linux, skip
*       indication.
*/
/*----------------------------------------------------------------------------*/
VOID
kalUpdateReAssocRspInfo(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PUINT_8          pucFrameBody,
    IN UINT_32          u4FrameBodyLen
)
{

    /* WCP Modification */

    return;
}


#if CFG_TX_FRAGMENT
BOOLEAN
kalQueryTxPacketHeader(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket,
    OUT PUINT_16 pu2EtherTypeLen,
    OUT PUINT_8 pucEthDestAddr
)
{

    kal_uint16                    pdu_len;
    UINT_16 u16Value = 0;
    PUINT_8 data_p = (PUINT_8)get_pdu_ptr(pvPacket, &pdu_len);

    //20091012 fix little-endian for parse packet header for WAPI usage
    u16Value = ((UINT_16)data_p[2 * PARAM_MAC_ADDR_LEN]) << 8;
    u16Value |= ((UINT_16)data_p[2 * PARAM_MAC_ADDR_LEN + 1]);

    memcpy(pu2EtherTypeLen, &u16Value, 2);
    memcpy(pucEthDestAddr, data_p, PARAM_MAC_ADDR_LEN);

#if 0
    struct sk_buff *prSkb = (struct sk_buff *)pvPacket;

    *pu2EtherTypeLen = (UINT_16)ntohs(prSkb->protocol);
    memcpy(pucEthDestAddr, prSkb->data, PARAM_MAC_ADDR_LEN);
#endif

    return TRUE;
}
#endif /* CFG_TX_FRAGMENT */

#ifdef CFG_TX_DBG_PACKET_ORDER
UINT_32 u4PacketIDOut[16] = {0};
#endif /* CFG_TX_DBG_PACKET_ORDER */

VOID
kalSendCompleteAndAwakeQueue(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket
)
{
    peer_buff_struct    *peer_p = NULL;
    UINT_16             u2QueueIdx = 0;
    UINT_8              ucNetworkType = 0;

    ASSERT(pvPacket);
    ASSERT(prGlueInfo->i4TxPendingFrameNum);

    peer_p = (peer_buff_struct *) pvPacket;

    if (GLUE_GET_PKT_IS_PAL(peer_p)) {
        ucNetworkType = NETWORK_TYPE_BOW_INDEX;
    } else if (GLUE_GET_PKT_IS_P2P(peer_p)) {
        ucNetworkType = NETWORK_TYPE_P2P_INDEX;
    } else {
        ucNetworkType = NETWORK_TYPE_AIS_INDEX;
    }

    GLUE_DEC_REF_CNT(prGlueInfo->i4TxPendingFrameNum);
    GLUE_DEC_REF_CNT(prGlueInfo->ai4TxPendingFrameNumPerQueue[ucNetworkType][u2QueueIdx]);

#if 0
    if (prGlueInfo->i4TxPendingFrameNum < CFG_TX_STOP_NETIF_QUEUE_THRESHOLD) {
        netif_wake_queue(prGlueInfo->prDevHandler);
    }
#endif


    return;
}

#if 0

/*----------------------------------------------------------------------------*/
/*!
* \brief Setting the IP address using for pattern search function, which is to
*        matching the ARP request/ response to our NIC.
*
* \param[in] prGlueInfo     Pointer to the Adapter structure.
* \param[in] prSetBuffer    Points to a buffer containing the OID-specific data for the set.
* \param[in] u4SetBufferLen Specifies the number of bytes at prSetBuffer.
* \param[in] pu4SetInfoLen  Points to the number of bytes it read or is needed.
*
* \retval TRUE Success.
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
kalSetNetworkAddress(
    IN  P_GLUE_INFO_T   prGlueInfo,
    IN  PVOID    prSetBuffer,
    IN  UINT_32  u4SetBufferLen,
    OUT PUINT_32 pu4SetInfoLen
)
{
    return WLAN_STATUS_SUCCESS;
}

WLAN_STATUS
kalQueryPowerMode(
    IN  P_GLUE_INFO_T   prGlueInfo,
    IN  PVOID           pvQueryBuffer,
    IN  UINT_32         u4QueryBufferLen,
    OUT PUINT_32        pu4QueryInfoLen
)
{
    return WLAN_STATUS_SUCCESS;
}

WLAN_STATUS
kalSetPowerMode(
    IN  P_GLUE_INFO_T   prGlueInfo,
    IN  PVOID           prSetBuffer,
    IN  UINT_32         u4SetBufferLen,
    OUT PUINT_32        pu4SetInfoLen
)
{
    return WLAN_STATUS_SUCCESS;
}
#endif

#if CFG_TCP_IP_CHKSUM_OFFLOAD

WLAN_STATUS
kalQueryTaskOffload(
    IN  P_GLUE_INFO_T   prGlueInfo,
    IN PVOID            pvQueryBuffer,
    IN UINT_32          u4QueryBufferLen,
    OUT PUINT_32        pu4QueryInfoLen
)
{
    return WLAN_STATUS_SUCCESS;
}


WLAN_STATUS
kalSetTaskOffload(
    IN  P_GLUE_INFO_T   prGlueInfo,
    IN  PVOID           prSetBuffer,
    IN  UINT_32         u4SetBufferLen,
    OUT PUINT_32        pu4SetInfoLen
)
{
    return WLAN_STATUS_SUCCESS;
}

#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Copy Mac Address setting from registry. It's All Zeros in Linux.
*
* \param[in] prAdapter Pointer to the Adapter structure
*
* \param[out] paucMacAddr Pointer to the Mac Address buffer
*
* \retval WLAN_STATUS_SUCCESS
*
* \note
*/
/*----------------------------------------------------------------------------*/
VOID
kalQueryRegistryMacAddr(
    IN  P_GLUE_INFO_T   prGlueInfo,
    OUT PUINT_8        paucMacAddr
)
{
    UINT_8 aucZeroMac[MAC_ADDR_LEN] = {0, 0, 0, 0, 0, 0};
    DEBUGFUNC("kalQueryRegistryMacAddr");


    WNDRV_ASSERT(prGlueInfo);
    WNDRV_ASSERT(paucMacAddr);

    kalMemCopy((PVOID) paucMacAddr, (PVOID)aucZeroMac, MAC_ADDR_LEN);

    return;
} /* end of kalQueryRegistryMacAddr() */

#if 0
/*----------------------------------------------------------------------------*/
/*!
* \brief Setting the power mode (power save profile).
*
* \param[in] prGlueInfo    Pointer to the Glue info structure.
* \param[in] eInitParam    The initial value to be queried.
*
* \retval return value of the parameter.
*
*/
/*----------------------------------------------------------------------------*/
UINT_32
kalGetParamDefaultValue(
    IN P_GLUE_INFO_T   prGlueInfo,
    IN ENUM_NIC_INITIAL_PARAM_E eInitParam
)
{
    UINT_32 u4RetVal = 0;


    WNDRV_ASSERT(prGlueInfo);

    switch (eInitParam) {
        case INIT_PARAM_POWER_MODE:
            u4RetVal = CFG_INIT_POWER_SAVE_PROF;
            break;

        case INIT_PARAM_ATIM_WINDOW:
            u4RetVal = CFG_IBSS_ATIM_WINDOW;
            break;

        case INIT_PARAM_UAPSD_AC:
            u4RetVal = CFG_INIT_UAPSD_AC;
            break;

        case INIT_PARAM_VOIP_INTERVAL:
            u4RetVal = CFG_INIT_VOIP_INTERVAL;
            break;

        case INIT_PARAM_L3_FILTER_EN:
            u4RetVal = CFG_L3_PATTERN_MATCH_WAKEUP;
            break;

        case INIT_PARAM_ADHOC_MODE:
            u4RetVal = CFG_INIT_ADHOC_MODE;
            break;

        default:
            WNDRV_ASSERT(0);
            break;
    }

    return u4RetVal;

} /* end of kalGetParamDefaultValue() */

VOID
NIC_MCR_RD(IN PMP_ADAPTER pAdapter,
           IN UINT_32     offset,
           IN PUINT_32    pValue)
{
}

VOID
NIC_MCR_WR(IN PMP_ADAPTER pAdapter,
           IN UINT_32     offset,
           IN UINT_32     value)
{
}

BOOLEAN
BCIRead8(IN  PMP_ADAPTER adapter_p,
         IN  UINT_8      bciIndex,
         OUT PUINT_8     bciData_p,
         IN  BOOLEAN     requireCHK,
         IN  UINT_8      chkMask)
{
    return 0;
}

BOOLEAN
BCIWrite8(IN PMP_ADAPTER adapter_p,
          IN UINT_8      bciIndex,
          IN UINT_8      bciData,
          IN BOOLEAN     requireCHK,
          IN UINT_8      chkMask)
{
    return 0;
}

VOID
phyWriteRFInitTable(IN PMP_ADAPTER adapter_p,
                    IN PUINT_32    rfInitTable_p,
                    IN UINT        rfInitTableSize,
                    IN UINT        waitDelay)
{
}   /* phyWriteRFInitTable */
#endif


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to check if card is removed
*
* \param pvGlueInfo     Pointer of GLUE Data Structure
*
* \retval TRUE:     card is removed
*         FALSE:    card is still attached
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalIsCardRemoved(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return FALSE;
    // Linux MMC doesn't have removal notification yet
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief This routine is used to send command to firmware for overriding netweork address
 *
 * \param pvGlueInfo Pointer of GLUE Data Structure

 * \retval TRUE
 *         FALSE
 */
/*----------------------------------------------------------------------------*/
BOOLEAN
kalRetrieveNetworkAddress(
    IN P_GLUE_INFO_T prGlueInfo,
    IN OUT PARAM_MAC_ADDRESS *prMacAddr
)
{
    ASSERT(prGlueInfo);

    if (prGlueInfo->fgIsMacAddrOverride == FALSE) {
#if !defined(CONFIG_X86)
#if defined(WCN_MAUI)
        //TODO(Nelson)
        COPY_MAC_ADDR(prMacAddr, prGlueInfo->rMacAddrOverride);

        return TRUE;
#else
        UINT_32 i;
        BOOLEAN fgIsReadError = FALSE;

        for (i = 0 ; i < MAC_ADDR_LEN ; i += 2) {
            if (kalCfgDataRead16(prGlueInfo,
                                 OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucMacAddress) + i,
                                 (PUINT_16)(((PUINT_8)prMacAddr) + i)) == FALSE) {
                fgIsReadError = TRUE;
                break;
            }
        }

        if (fgIsReadError == TRUE) {
            return FALSE;
        } else {
            return TRUE;
        }
#endif
#else
        /* x86 Linux doesn't need to override network address so far */
        return FALSE;
#endif
    } else {
        COPY_MAC_ADDR(prMacAddr, prGlueInfo->rMacAddrOverride);

        return TRUE;
    }
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to flush pending TX packets in glue layer
*
* \param pvGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalFlushPendingTxPackets(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    P_QUE_T prTxQue;
    P_QUE_ENTRY_T prQueueEntry;
    PVOID prPacket;

    GLUE_SPIN_LOCK_DECLARATION();

    ASSERT(prGlueInfo);

    prTxQue = &(prGlueInfo->rTxQueue);

    if (prGlueInfo->i4TxPendingFrameNum) {
        while (TRUE) {
            GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);
            QUEUE_REMOVE_HEAD(prTxQue, prQueueEntry, P_QUE_ENTRY_T);
            GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);

            if (prQueueEntry == NULL) {
                break;
            }

            prPacket = GLUE_GET_PKT_DESCRIPTOR(prQueueEntry);

            kalSendComplete(prGlueInfo,
                            prPacket,
                            WLAN_STATUS_NOT_ACCEPTED);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to flush pending TX packets in glue layer for specific network
*
* \param pvGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalFlushPendingTxPacketsByNetwork(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
)
{
    P_QUE_T prTxQue;
    P_QUE_ENTRY_T prQueueEntry;
    PVOID prPacket;
    peer_buff_struct    *peer_p = (peer_buff_struct *)NULL;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;


    ASSERT(prGlueInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_FUNC, "kalFlushPendingTxPacketsByNetwork, eNetworkTypeIdx=%d", eNetworkTypeIdx);

    if (prGlueInfo->i4TxPendingFrameNum == 0) {
        return;
    }

    GLUE_SPIN_LOCK_DECLARATION();

    prTxQue = &(prGlueInfo->rTxQueue);

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prTxQue);
    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);

    while (prQueueEntry) {

        prPacket = GLUE_GET_PKT_DESCRIPTOR(prQueueEntry);

        peer_p = (peer_buff_struct *)prPacket;
        if (prPacket) {
            if ((GLUE_GET_PKT_IS_P2P(peer_p) && (eNetworkTypeIdx == NETWORK_TYPE_P2P_INDEX)) //P2P case
                    || ((!GLUE_GET_PKT_IS_P2P(peer_p)) && (eNetworkTypeIdx == NETWORK_TYPE_AIS_INDEX))) { //AIS CASE
                kalSendComplete(prGlueInfo,
                                prPacket,
                                WLAN_STATUS_NOT_ACCEPTED);
            } else {
                QUEUE_INSERT_TAIL(prTxQue, prQueueEntry);
            }
        } else {
            QUEUE_INSERT_TAIL(prTxQue, prQueueEntry);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    QUEUE_CONCATENATE_QUEUES(prTxQue, prTempCmdQue);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is get indicated media state
*
* \param pvGlueInfo     Pointer of GLUE Data Structure
*
* \retval
*/
/*----------------------------------------------------------------------------*/
ENUM_PARAM_MEDIA_STATE_T
kalGetMediaStateIndicated(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return prGlueInfo->eParamMediaStateIndicated;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to set indicated media state
*
* \param pvGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalSetMediaStateIndicated(
    IN P_GLUE_INFO_T            prGlueInfo,
    IN ENUM_PARAM_MEDIA_STATE_T eParamMediaStateIndicate
)
{
    ASSERT(prGlueInfo);

    prGlueInfo->eParamMediaStateIndicated = eParamMediaStateIndicate;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to clear pending OID staying in command queue
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalOidCmdClearance(
    IN P_GLUE_INFO_T prGlueInfo
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;
    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;

    GLUE_SPIN_LOCK_DECLARATION();

    ASSERT(prGlueInfo);

    prCmdQue = &prGlueInfo->rCmdQueue;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);

    while (prQueueEntry) {
#if 1
        prCmdInfo = (P_CMD_INFO_T)prQueueEntry;
        cmdBufFreeCmdInfo(prGlueInfo->prAdapter, prCmdInfo);
#else
        if (((P_CMD_INFO_T)prQueueEntry)->fgIsOid) {
            prCmdInfo = (P_CMD_INFO_T)prQueueEntry;
            break;
        } else {
            QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
        }
#endif

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    QUEUE_CONCATENATE_QUEUES(prCmdQue, prTempCmdQue);
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);

#if 0
    if (prCmdInfo) {
        if (prCmdInfo->pfCmdTimeoutHandler) {
            prCmdInfo->pfCmdTimeoutHandler(prGlueInfo->prAdapter, prCmdInfo);
        } else {
            kalOidComplete(prGlueInfo,
                           prCmdInfo->fgSetQuery,
                           0,
                           WLAN_STATUS_NOT_ACCEPTED);
        }

        cmdBufFreeCmdInfo(prGlueInfo->prAdapter, prCmdInfo);
    }
#endif
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to insert command into prCmdQueue
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*        prQueueEntry   Pointer of queue entry to be inserted
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalEnqueueCommand(
    IN P_GLUE_INFO_T prGlueInfo,
    IN P_QUE_ENTRY_T prQueueEntry
)
{
    P_QUE_T prCmdQue;

    //MT5931_WAP_TRACE(MOD_WNDRV,TRACE_PEER,"kalEnqueueCommand");

    GLUE_SPIN_LOCK_DECLARATION();

    ASSERT(prGlueInfo);
    ASSERT(prQueueEntry);

    prCmdQue = &prGlueInfo->rCmdQueue;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
    QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Handle EVENT_ID_ASSOC_INFO event packet by indicating to OS with
*        proper information
*
* @param pvGlueInfo     Pointer of GLUE Data Structure
* @param prAssocInfo    Pointer of EVENT_ID_ASSOC_INFO Packet
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
kalHandleAssocInfo(
    IN P_GLUE_INFO_T prGlueInfo,
    IN P_EVENT_ASSOC_INFO prAssocInfo
)
{
    // to do
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to get firmware load address from registry
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval
*/
/*----------------------------------------------------------------------------*/
UINT_32
kalGetFwLoadAddress(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return prGlueInfo->rRegInfo.u4LoadAddress;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to get firmware start address from registry
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval
*/
/*----------------------------------------------------------------------------*/
UINT_32
kalGetFwStartAddress(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return prGlueInfo->rRegInfo.u4StartAddress;
}

/*----------------------------------------------------------------------------*/
/*!
 * * @brief Notify OS with SendComplete event of the specific packet. Linux should
 * *        free packets here.
 * *
 * * @param pvGlueInfo     Pointer of GLUE Data Structure
 * * @param pvPacket       Pointer of Packet Handle
 * * @param status         Status Code for OS upper layer
 * *
 * * @return none
 * */
/*----------------------------------------------------------------------------*/

/// Todo
VOID
kalSecurityFrameSendComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket,
    IN WLAN_STATUS rStatus
)
{
#if defined(WCN_MAUI)
    peer_buff_struct *peer_p = (peer_buff_struct *)pvPacket;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;
#endif

    ASSERT(pvPacket);

#if defined(WCN_MAUI)
    /****************************************************************************
        *  1. Get packet descriptor in free header
        ****************************************************************************/
    wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

#if CFG_ENABLE_WIFI_TETHERING
    WNDRV_EXT_ASSERT((wndrv_pkt_p->src_mod_id == MOD_SUPC)
                     || (wndrv_pkt_p->src_mod_id == MOD_TCPIP)
                     || (wndrv_pkt_p->src_mod_id == MOD_HOSTAP)
                     , "kalSendComplete: src_mod_id must be SUPC or TCPIP",
                     wndrv_pkt_p->src_mod_id, 0, 0);
#else
    WNDRV_EXT_ASSERT((wndrv_pkt_p->src_mod_id == MOD_SUPC)
                     || (wndrv_pkt_p->src_mod_id == MOD_TCPIP)
                     , "kalSendComplete: src_mod_id must be SUPC or TCPIP",
                     wndrv_pkt_p->src_mod_id, 0, 0);
#endif
    /****************************************************************************
        *  2. Free transmitted peer buffer and GLUE info
        ****************************************************************************/
    if (wndrv_pkt_p->flc_mode == 0) {
        /* SUPC packet TX */
        free_peer_buff(peer_p);
        wndrv_context.glue_p->tx_sys_peer_num--;
    }
#if defined (WLAN_ZERO_COPY)
    else if (wndrv_pkt_p->flc_mode == 1) { //MOD_TCPIP
        /* TCPIP packet */
        flc2_free_peer_buff(wndrv_pkt_p->pool_id, peer_p);

        wndrv_context.glue_p->tx_flc_peer_num--;
    }
#endif
    else {
        WNDRV_ASSERT(0);
    }
#else
    dev_kfree_skb((struct sk_buff *) pvPacket);
#endif

    GLUE_DEC_REF_CNT(prGlueInfo->i4TxPendingSecurityFrameNum);
}

UINT_32
kalGetTxPendingFrameCount(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return (UINT_32)(prGlueInfo->i4TxPendingFrameNum);
}

UINT_32
kalGetTxAvaliableFrameCount(void)
{
    P_GLUE_INFO_T prGlueInfo = wndrv_context.adpt_p->prGlueInfo;
    UINT_32 frame_avaliable_count = 0;
    UINT_32 ilm_available_count = 0;

    ASSERT(prGlueInfo);

    if ((UINT_32)(prGlueInfo->i4TxPendingFrameNum) < CFG_TX_STOP_NETIF_QUEUE_THRESHOLD) {
        frame_avaliable_count = (CFG_TX_STOP_NETIF_QUEUE_THRESHOLD - (UINT_32)(prGlueInfo->i4TxPendingFrameNum));
    }

    ilm_available_count = get_ilm_avaliable_count(MOD_TCPIP);

    return ((ilm_available_count < frame_avaliable_count) ? ilm_available_count : frame_avaliable_count);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to retrieve the number of pending commands
*        (including MMPDU, 802.1X and command packets)
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval
*/
/*----------------------------------------------------------------------------*/
UINT_32
kalGetTxPendingCmdCount(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    P_QUE_T prCmdQue;

    ASSERT(prGlueInfo);
    prCmdQue = &prGlueInfo->rCmdQueue;

    return prCmdQue->u4NumElem;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Timer Initialization Procedure
*
* \param[in] prGlueInfo     Pointer to GLUE Data Structure
* \param[in] prTimerHandler Pointer to timer handling function, whose only
*                           argument is "prAdapter"
*
* \retval none
*
*/
/*----------------------------------------------------------------------------*/

//static struct timer_list tickfn;

VOID
kalOsTimerInitialize(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN PVOID            prTimerHandler
)
{
#if 0
    ASSERT(prGlueInfo);

    init_timer(&(prGlueInfo->tickfn));
    prGlueInfo->tickfn.function = prTimerHandler;
    prGlueInfo->tickfn.data = (unsigned long) prGlueInfo;
#endif
}

// Todo
/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to set the time to do the time out check.
*
* \param[in] prGlueInfo Pointer to GLUE Data Structure
* \param[in] rInterval  Time out interval from current time.
*
* \retval TRUE Success.
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalSetTimer(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Interval
)
{
#if 0
    ASSERT(prGlueInfo);
    del_timer_sync(&(prGlueInfo->tickfn));

    prGlueInfo->tickfn.expires = jiffies + u4Interval * HZ / MSEC_PER_SEC;
    add_timer(&(prGlueInfo->tickfn));
#endif
    return TRUE; /* success */
}
/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to cancel
*
* \param[in] prGlueInfo Pointer to GLUE Data Structure
*
* \retval TRUE  :   Timer has been canceled
*         FALAE :   Timer doens't exist
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalCancelTimer(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
#if 0
    ASSERT(prGlueInfo);

    clear_bit(GLUE_FLAG_TIMEOUT_BIT, &prGlueInfo->u4Flag);

    if (del_timer_sync(&(prGlueInfo->tickfn)) >= 0) {
        return TRUE;
    } else {
        return FALSE;
    }
#endif
    return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is a callback function for scanning done
*
* \param[in] prGlueInfo Pointer to GLUE Data Structure
*
* \retval none
*
*/
/*----------------------------------------------------------------------------*/
VOID
kalScanDone(
    IN P_GLUE_INFO_T                 prGlueInfo,
    IN ENUM_KAL_NETWORK_TYPE_INDEX_T eNetTypeIdx,
    IN WLAN_STATUS                   status
)
{
    ASSERT(prGlueInfo);
    //complete(&prGlueInfo->rScanComp);
    kalIndicateStatusAndComplete(prGlueInfo, WLAN_STATUS_SCAN_COMPLETE, NULL, 0);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to generate a random number
*
* \param none
*
* \retval UINT_32
*/
/*----------------------------------------------------------------------------*/
UINT_32
kalRandomNumber(
    VOID
)
{
    UINT_32 number = 0;

#if defined(WCN_MAUI)
    number = L1I_GetTimeStamp();
#else
    get_random_bytes(&number, 4);
#endif

    return number;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief command timeout call-back function
 *
 * \param[in] prGlueInfo Pointer to the GLUE data structure.
 *
 * \retval (none)
 */
/*----------------------------------------------------------------------------*/
VOID
kalTimeoutHandler(unsigned long arg)
{

    P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) arg;

    ASSERT(prGlueInfo);
#if 0
    /* Notify tx thread  for timeout event */
    set_bit(GLUE_FLAG_TIMEOUT_BIT, &prGlueInfo->u4Flag);
    wake_up_interruptible(&prGlueInfo->waitq);
#endif
    return;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Set mailbox, CMD, packet tx flag to be handled
 *
 * \param[in] prGlueInfo Pointer to the GLUE data structure.
 *
 * \retval (none)
 */
/*----------------------------------------------------------------------------*/
VOID
kalSetEvent(P_GLUE_INFO_T pr)
{
    set_bit(GLUE_FLAG_TXREQ_BIT, &pr->u4Flag);
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Process mailbox, CMD, packet tx
 *
 * \param[in] prGlueInfo Pointer to the GLUE data structure.
 *
 * \retval (none)
 */
/*----------------------------------------------------------------------------*/
VOID
kalMainThread(P_GLUE_INFO_T pr)
{
    P_QUE_ENTRY_T       prQueueEntry = NULL;
    P_QUE_T             prTxQueue = &pr->rTxQueue;
    P_QUE_T             prCmdQue = &pr->rCmdQueue;
    BOOLEAN             fgNeedHwAccess = FALSE;
    P_NATIVE_PACKET     prSkb = NULL;

    /*
     *
     * if TX request, clear the TXREQ flag. TXREQ set by kalSetEvent/GlueSetEvent
     * indicates the following requests occur
     *
     */
#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("KMT", SA_start);
#endif
    if (test_and_clear_bit(GLUE_FLAG_TXREQ_BIT, &pr->u4Flag)) {
        /* Process Mailbox Messages */
        wlanProcessMboxMessage(pr->prAdapter);

        /* Process CMD request */
        do {
            if (prCmdQue->u4NumElem > 0) {
                if (fgNeedHwAccess == FALSE) {
                    fgNeedHwAccess = TRUE;

                    wlanAcquirePowerControl(pr->prAdapter);
                }
                wlanProcessCommandQueue(pr->prAdapter, prCmdQue);
            }
        } while (FALSE);

        /* Handle Packet Tx */
        {
            while (QUEUE_IS_NOT_EMPTY(prTxQueue)) {
                GLUE_ACQUIRE_SPIN_LOCK(pr, SPIN_LOCK_TX_QUE);
                QUEUE_REMOVE_HEAD(prTxQueue, prQueueEntry, P_QUE_ENTRY_T);
                GLUE_RELEASE_SPIN_LOCK(pr, SPIN_LOCK_TX_QUE);

                ASSERT(prQueueEntry);
                if (NULL == prQueueEntry) {
                    break;
                }

                prSkb = (P_NATIVE_PACKET) GLUE_GET_PKT_DESCRIPTOR(prQueueEntry);
                ASSERT(prSkb);
                if (NULL == prSkb) {
                    //printk(KERN_WARNING DRV_NAME"prSkb == NULL in tx\n");
                    continue;
                }

                if (wlanEnqueueTxPacket(pr->prAdapter,
                                        (P_NATIVE_PACKET)prSkb) == WLAN_STATUS_RESOURCES) {
                    GLUE_ACQUIRE_SPIN_LOCK(pr, SPIN_LOCK_TX_QUE);
                    QUEUE_INSERT_HEAD(prTxQueue, prQueueEntry);
                    GLUE_RELEASE_SPIN_LOCK(pr, SPIN_LOCK_TX_QUE);

                    break;
                }
            }
            if (
#if CFG_TCP_IP_FLOW_CTRL
                (!pr->fgPending_Ctrl) &&
#endif
                (wlanGetTxPendingFrameCount(pr->prAdapter) > 0)) {
                wlanTxPendingPackets(pr->prAdapter, &fgNeedHwAccess);
#if CFG_TCP_IP_FLOW_CTRL
                if (pr->fgPending_Frame0) {
                    pr->fgPending_Ctrl = 1;
                }
#endif
            }
        }
    }

    /* Release ownership to enter power save mode */
    if (fgNeedHwAccess == TRUE) {
        wlanReleasePowerControl(pr->prAdapter);
    }
#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("KMT", SA_stop);
#endif
}


/*----------------------------------------------------------------------------*/
/*!
* \brief to check if configuration file (NVRAM/Registry) exists
*
* \param[in]
*           prGlueInfo
*
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalIsConfigurationExist(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
#if !defined(CONFIG_X86)
    ASSERT(prGlueInfo);

    return prGlueInfo->fgNvramAvailable;
#else
    /* there is no configuration data for x86-linux */
    return FALSE;
#endif
}


/*----------------------------------------------------------------------------*/
/*!
* \brief to retrieve Registry information
*
* \param[in]
*           prGlueInfo
*
* \return
*           Pointer of REG_INFO_T
*/
/*----------------------------------------------------------------------------*/
P_REG_INFO_T
kalGetConfiguration(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return &(prGlueInfo->rRegInfo);
}


/*----------------------------------------------------------------------------*/
/*!
* \brief to retrieve version information of corresponding configuration file
*
* \param[in]
*           prGlueInfo
*
* \param[out]
*           pu2Part1CfgOwnVersion
*           pu2Part1CfgPeerVersion
*           pu2Part2CfgOwnVersion
*           pu2Part2CfgPeerVersion
*
* \return
*           NONE
*/
/*----------------------------------------------------------------------------*/
VOID
kalGetConfigurationVersion(
    IN P_GLUE_INFO_T    prGlueInfo,
    OUT PUINT_16        pu2Part1CfgOwnVersion,
    OUT PUINT_16        pu2Part1CfgPeerVersion,
    OUT PUINT_16        pu2Part2CfgOwnVersion,
    OUT PUINT_16        pu2Part2CfgPeerVersion
)
{
    ASSERT(prGlueInfo);

    ASSERT(pu2Part1CfgOwnVersion);
    ASSERT(pu2Part1CfgPeerVersion);
    ASSERT(pu2Part2CfgOwnVersion);
    ASSERT(pu2Part2CfgPeerVersion);

    kalCfgDataRead16(prGlueInfo,
                     OFFSET_OF(WIFI_CFG_PARAM_STRUCT, u2Part1OwnVersion),
                     pu2Part1CfgOwnVersion);

    kalCfgDataRead16(prGlueInfo,
                     OFFSET_OF(WIFI_CFG_PARAM_STRUCT, u2Part1PeerVersion),
                     pu2Part1CfgPeerVersion);

    kalCfgDataRead16(prGlueInfo,
                     OFFSET_OF(WIFI_CFG_PARAM_STRUCT, u2Part2OwnVersion),
                     pu2Part2CfgOwnVersion);

    kalCfgDataRead16(prGlueInfo,
                     OFFSET_OF(WIFI_CFG_PARAM_STRUCT, u2Part2PeerVersion),
                     pu2Part2CfgPeerVersion);

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief to check if the WPS is active or not
*
* \param[in]
*           prGlueInfo
*
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalWSCGetActiveState(
    IN P_GLUE_INFO_T    prGlueInfo
)
{
    ASSERT(prGlueInfo);

    return (prGlueInfo->fgWpsActive);
}


/*----------------------------------------------------------------------------*/
/*!
* \brief update RSSI and LinkQuality to GLUE layer
*
* \param[in]
*           prGlueInfo
*           eNetTypeIdx
*           cRssi
*           cLinkQuality
*
* \return
*           None
*/
/*----------------------------------------------------------------------------*/
VOID
kalUpdateRSSI(
    IN P_GLUE_INFO_T                    prGlueInfo,
    IN ENUM_KAL_NETWORK_TYPE_INDEX_T    eNetTypeIdx,
    IN INT_8                            cRssi,
    IN INT_8                            cLinkQuality
)
{
    //struct iw_statistics *pStats;

    ASSERT(prGlueInfo);
#if 0
    switch (eNetTypeIdx) {
        case KAL_NETWORK_TYPE_AIS_INDEX:
            pStats = (struct iw_statistics *)(&(prGlueInfo->rIwStats));

            pStats->qual.qual = cLinkQuality;
            pStats->qual.noise = 0;
            pStats->qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_NOISE_UPDATED;
            pStats->qual.level = 0x100 + cRssi;
            pStats->qual.updated |= IW_QUAL_LEVEL_UPDATED;

            break;

        default:
            break;

    }
#endif
    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Pre-allocate I/O buffer
*
* \param[in]
*           none
*
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalInitIOBuffer(
    VOID
)
{
    //UINT_32 u4Size;
    UINT_32 u4Offset;
    PUINT_8 pvIoBuffer_temp;
#if 1
    /* Get 32-byte alignment address */
    u4Offset = (UINT_32)&wndrv_io_buf_pool[0];
    u4Offset = ALIGN_32(u4Offset) - (UINT_32)&wndrv_io_buf_pool[0];
    pvIoBuffer_temp = (PUINT_8)&wndrv_io_buf_pool[0] + u4Offset;
    pvIoBuffer = (PVOID)pvIoBuffer_temp;
    pvIoBufferSize = sizeof(wndrv_io_buf_pool) - u4Offset;
    pvIoBufferUsage = 0;

    return TRUE;
#else
    if (CFG_COALESCING_BUFFER_SIZE >= CFG_RX_COALESCING_BUFFER_SIZE) {
        u4Size = CFG_COALESCING_BUFFER_SIZE + sizeof(ENHANCE_MODE_DATA_STRUCT_T);
    } else {
        u4Size = CFG_RX_COALESCING_BUFFER_SIZE + sizeof(ENHANCE_MODE_DATA_STRUCT_T);
    }

    pvIoBuffer = kmalloc(u4Size, GFP_KERNEL);
    if (pvIoBuffer) {
        pvIoBufferSize = u4Size;
        pvIoBufferUsage = 0;

        return TRUE;
    }
#endif
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Free pre-allocated I/O buffer
*
* \param[in]
*           none
*
* \return
*           none
*/
/*----------------------------------------------------------------------------*/
VOID
kalUninitIOBuffer(
    VOID
)
{
#if 1
    if (pvIoBuffer) {
        pvIoBuffer = (PVOID) NULL;
        pvIoBufferSize = 0;
        pvIoBufferUsage = 0;
    }
#else
    if (pvIoBuffer) {
        kfree(pvIoBuffer);

        pvIoBuffer = (PVOID) NULL;
        pvIoBufferSize = 0;
        pvIoBufferUsage = 0;
    }
#endif
    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Dispatch pre-allocated I/O buffer
*
* \param[in]
*           u4AllocSize
*
* \return
*           PVOID for pointer of pre-allocated I/O buffer
*/
/*----------------------------------------------------------------------------*/
PVOID
kalAllocateIOBuffer(
    IN UINT_32 u4AllocSize
)
{
    PVOID ret = (PVOID)NULL;

    if (pvIoBuffer) {
        if (u4AllocSize <= (pvIoBufferSize - pvIoBufferUsage)) {
            ret = (PVOID) & (((PUINT_8)(pvIoBuffer))[pvIoBufferUsage]);
            pvIoBufferUsage += u4AllocSize;
        }
    } else {
#if 0   //TODO(Nelson): error handling
        /* fault tolerance */
        ret = (PVOID) kalMemAlloc(u4AllocSize, PHY_MEM_TYPE);
#endif
    }

    return ret;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief Release all dispatched I/O buffer
*
* \param[in]
*           none
*
* \return
*           none
*/
/*----------------------------------------------------------------------------*/
VOID
kalReleaseIOBuffer(
    IN PVOID pvAddr,
    IN UINT_32 u4Size
)
{
    if (pvIoBuffer) {
        if (pvIoBufferUsage > u4Size) {
            pvIoBufferUsage -= u4Size;
        } else {
            pvIoBufferUsage = 0;
        }
    } else {
#if 0   //TODO(Nelson): error handling
        /* fault tolerance */
        kalMemFree(pvAddr, PHY_MEM_TYPE, u4Size);
#endif
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This inline function is to extract some packet information, including
*        user priority, packet length, destination address, 802.1x and BT over Wi-Fi
*        or not.
*
* @param prGlueInfo         Pointer to the glue structure
* @param prNdisPacket       Packet descriptor
* @param pucPriorityParam   User priority
* @param pu4PacketLen       Packet length
* @param pucEthDestAddr Destination address
* @param pfgIs1X            802.1x packet or not
* @param pfgIsPAL           BT over Wi-Fi packet or not
*
* @retval TRUE      Success to extract information
* @retval FALSE     Fail to extract correct information
*/
/*----------------------------------------------------------------------------*/

BOOL
kalQoSFrameClassifierAndPacketInfo(
    IN P_GLUE_INFO_T prGlueInfo,
    IN P_NATIVE_PACKET prPacket,
    OUT PUINT_8 pucPriorityParam,
    OUT PUINT_32 pu4PacketLen,
    OUT PUINT_8 pucEthDestAddr,
    OUT PBOOLEAN pfgIs1X,
    OUT PBOOLEAN pfgIsPAL,
    OUT PUINT_8 pucNetworkType
)
{
    UINT_32 u4PacketLen;

    UINT_8 ucUserPriority = USER_PRIORITY_DEFAULT; /* Default */
    UINT_16 u2EtherTypeLen;
    UINT_16 u2PduLen;
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    BOOLEAN fgIsP2P = FALSE;
#endif

#if !defined(WCN_MAUI)
    struct sk_buff *prSkb = (struct sk_buff *) prPacket;
#else
    peer_buff_struct *peer_p = NULL;
#endif
    PUINT_8 aucLookAheadBuf = NULL;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p = NULL;


    ASSERT(prGlueInfo);
    ASSERT(prPacket);

    peer_p = (peer_buff_struct *)prPacket;
    wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);
#if CFG_ENABLE_WIFI_DIRECT || CFG_ENABLE_WIFI_TETHERING
    fgIsP2P = GLUE_GET_PKT_IS_P2P(peer_p);
#endif

#if !defined(WCN_MAUI)
    u4PacketLen = prSkb->len;

    if (u4PacketLen < ETH_HLEN) {
        printk("Invalid Ether packet length: %u\n", (unsigned int) u4PacketLen);
        return FALSE;
    }
#endif

#if !defined(WCN_MAUI)
    aucLookAheadBuf = prSkb->data;
#else
    aucLookAheadBuf = (PUINT_8)get_pdu_ptr(prPacket, &u2PduLen);
    u4PacketLen = (UINT_32)u2PduLen & 0xffff;

    if (u4PacketLen < ETH_HLEN) {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_56, "Invalid Ether packet length: %d\n", u4PacketLen);
        return FALSE;
    }
#endif

    *pfgIs1X = FALSE;
    *pfgIsPAL = FALSE;
    //4 <3> Obtain the User Priority for WMM
    u2EtherTypeLen = (aucLookAheadBuf[ETH_TYPE_LEN_OFFSET] << 8) | (aucLookAheadBuf[ETH_TYPE_LEN_OFFSET + 1]);

    if ((u2EtherTypeLen == ETH_P_IP) &&
            (u4PacketLen >= LOOK_AHEAD_LEN)) {
        PUINT_8 pucIpHdr = &aucLookAheadBuf[ETH_HLEN];
        UINT_8 ucIpVersion;


        ucIpVersion = (pucIpHdr[0] & IPVH_VERSION_MASK) >> IPVH_VERSION_OFFSET;

        //printk ("ip version %x\n", ucIpVersion);

        if (ucIpVersion == IPVERSION) {
            UINT_8 ucIpTos;


            /* Get the DSCP value from the header of IP packet. */
            ucIpTos = pucIpHdr[1];
            ucUserPriority = ((ucIpTos & IPTOS_PREC_MASK) >> IPTOS_PREC_OFFSET);
        }

        /* TODO(Kevin): Add TSPEC classifier here */
    } else if (u2EtherTypeLen == ETH_P_1X) { /* For Port Control */
        //DBGLOG(REQ, TRACE, ("Tx 1x\n"));
        *pfgIs1X = TRUE;
    } else if (u2EtherTypeLen == ETH_P_PRE_1X) { /* For Pre 1x pkt */
        //DBGLOG(REQ, TRACE, ("Tx Pre-1x\n"));
        *pfgIs1X = TRUE;
    }
#ifdef __WAPI_SUPPORT__
    else if (u2EtherTypeLen == ETH_WPI_1X) {
        *pfgIs1X = TRUE;
    }
#endif
    else if (u2EtherTypeLen <= 1500) { /* 802.3 Frame */
        UINT_8 ucDSAP, ucSSAP, ucControl;
        UINT_8 aucOUI[3];

        ucDSAP = *(PUINT_8) &aucLookAheadBuf[ETH_LLC_OFFSET];
        ucSSAP = *(PUINT_8) &aucLookAheadBuf[ETH_LLC_OFFSET + 1];
        ucControl = *(PUINT_8) &aucLookAheadBuf[ETH_LLC_OFFSET + 2];

        aucOUI[0] = *(PUINT_8) &aucLookAheadBuf[ETH_SNAP_OFFSET];
        aucOUI[1] = *(PUINT_8) &aucLookAheadBuf[ETH_SNAP_OFFSET + 1];
        aucOUI[2] = *(PUINT_8) &aucLookAheadBuf[ETH_SNAP_OFFSET + 2];

        /* PAL data Frame */
        if (ucDSAP == ETH_LLC_DSAP_SNAP &&
                ucSSAP == ETH_LLC_SSAP_SNAP &&
                ucControl == ETH_LLC_CONTROL_UNNUMBERED_INFORMATION &&
                aucOUI[0] == ETH_SNAP_BT_SIG_OUI_0 &&
                aucOUI[1] == ETH_SNAP_BT_SIG_OUI_1 &&
                aucOUI[2] == ETH_SNAP_BT_SIG_OUI_2) {

            UINT_16 tmp = ((aucLookAheadBuf[ETH_SNAP_OFFSET + 3] << 8) | aucLookAheadBuf[ETH_SNAP_OFFSET + 4]);

            *pfgIsPAL = TRUE;
#if 0
            ucUserPriority = (UINT_8)prSkb->priority;
#else
            WNDRV_TRACE(TRACE_ERROR, "ERROR:should not use wndrv_pkt_p->tid\n");
            ucUserPriority = (UINT_8)wndrv_pkt_p->tid;
#endif

            if (tmp == BOW_PROTOCOL_ID_SECURITY_FRAME) {
                *pfgIs1X = TRUE;
            }
        }
    }

    //4 <4> Return the value of Priority Parameter.
    *pucPriorityParam = ucUserPriority;

    //4 <5> Retrieve Packet Information - DA
    /* Packet Length/ Destination Address */
    *pu4PacketLen = u4PacketLen;

    kalMemCopy(pucEthDestAddr, aucLookAheadBuf, PARAM_MAC_ADDR_LEN);


    //<6> Network type
#if CFG_ENABLE_BT_OVER_WIFI
    if (*pfgIsPAL == TRUE) {
        *pucNetworkType = NETWORK_TYPE_BOW_INDEX;
    } else
#endif
    {
#if CFG_ENABLE_WIFI_DIRECT
        if (prGlueInfo->prAdapter->fgIsP2PRegistered && GLUE_GET_PKT_IS_P2P(prPacket)) {
            *pucNetworkType = NETWORK_TYPE_P2P_INDEX;
        } else
#endif
#if CFG_ENABLE_WIFI_TETHERING
            if ((prGlueInfo->rWtInfo.fgIsTetheringEnable) && (fgIsP2P)) {
                *pucNetworkType = NETWORK_TYPE_P2P_INDEX;
            } else
#endif /* CFG_ENABLE_WIFI_TETHERING */
            {
                *pucNetworkType = NETWORK_TYPE_AIS_INDEX;
            }
    }

    return TRUE;
} /* end of kalQoSFrameClassifier() */

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to clear all pending security frames
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalClearSecurityFrames(
    IN P_GLUE_INFO_T prGlueInfo
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;

    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;
    GLUE_SPIN_LOCK_DECLARATION();

    ASSERT(prGlueInfo);

    // Clear pending security frames in prGlueInfo->rCmdQueue
    prCmdQue = &prGlueInfo->rCmdQueue;



    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    while (prQueueEntry) {
        prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

        if (prCmdInfo->eCmdType == COMMAND_TYPE_SECURITY_FRAME) {
            prCmdInfo->pfCmdTimeoutHandler(prGlueInfo->prAdapter, prCmdInfo);
            cmdBufFreeCmdInfo(prGlueInfo->prAdapter, prCmdInfo);
        } else {
            QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    QUEUE_CONCATENATE_QUEUES(prCmdQue, prTempCmdQue);
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to clear all pending management frames
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalClearMgmtFrames(
    IN P_GLUE_INFO_T prGlueInfo
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;
    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;

    GLUE_SPIN_LOCK_DECLARATION();

    ASSERT(prGlueInfo);

    // Clear pending management frames in prGlueInfo->rCmdQueue
    prCmdQue = &prGlueInfo->rCmdQueue;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    while (prQueueEntry) {
        prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

        if (prCmdInfo->eCmdType == COMMAND_TYPE_MANAGEMENT_FRAME) {
            wlanReleaseCommand(prGlueInfo->prAdapter, prCmdInfo);
            cmdBufFreeCmdInfo(prGlueInfo->prAdapter, prCmdInfo);
        } else {
            QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    QUEUE_CONCATENATE_QUEUES(prCmdQue, prTempCmdQue);
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to clear all pending management frames by network
*
* \param prGlueInfo     Pointer of GLUE Data Structure
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalClearMgmtFramesByNetType(
    IN P_GLUE_INFO_T prGlueInfo,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;
    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;

    GLUE_SPIN_LOCK_DECLARATION();

    ASSERT(prGlueInfo);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_FUNC, "kalClearMgmtFramesByNetType, eNetworkTypeIdx=%d", eNetworkTypeIdx);
    // Clear pending management frames in prGlueInfo->rCmdQueue
    prCmdQue = &prGlueInfo->rCmdQueue;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    while (prQueueEntry) {
        prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

        if ((prCmdInfo->eCmdType == COMMAND_TYPE_MANAGEMENT_FRAME)
                && (prCmdInfo->eNetworkType == eNetworkTypeIdx)) {
            wlanReleaseCommand(prGlueInfo->prAdapter, prCmdInfo);
            cmdBufFreeCmdInfo(prGlueInfo->prAdapter, prCmdInfo);
        } else {
            QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    QUEUE_CONCATENATE_QUEUES(prCmdQue, prTempCmdQue);
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is provided by GLUE Layer for internal driver stack to update
*        current MAC address.
*
* \param[in] prGlueInfo     Pointer of GLUE Data Structure
* \param[in] pucMacAddr     Pointer of current MAC address
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
kalUpdateMACAddress(
    IN P_GLUE_INFO_T prGlueInfo,
    IN PUINT_8 pucMacAddr
)
{
    ASSERT(prGlueInfo);
    ASSERT(pucMacAddr);

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Only handles driver own creating packet (coalescing buffer).
*
* \param prGlueInfo   Pointer of GLUE Data Structure
* \param u4Size       Pointer of Packet Handle
* \param ppucData     Status Code for OS upper layer
*
* \return NULL: Failed to allocate skb, Not NULL get skb
*/
/*----------------------------------------------------------------------------*/
PVOID
kalPacketAlloc(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Size,
    OUT PUINT_8         *ppucData
)
{
#if defined(WCN_MAUI)
    peer_buff_struct             *peer_p = NULL;
    //static volatile PVOID record = NULL;
    wndrv_pkt_descriptor_struct  *wndrv_pkt_p;
    kal_uint16                    pdu_len;
    P_PACKET_INFO_T               prPacketInfo;
    kal_uint32                    sys_peerbuf_rx_pad;

    //DEBUGFUNC("kalPacketAlloc");
    if (u4Size != CFG_RX_MAX_PKT_SIZE) {
#ifdef WCN_MAUI /* WCP Modification */
        WNDRV_EXT_ASSERT(0, "Packet Alloc Size Err: %d", u4Size, 0, 0);
#else
        WNDRV_ASSERT(0);
#endif
    }

    { /* management packet RX, TCPIP packet RX and SUPC packet RX */
#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("sb1", SA_start);
#endif
#if defined (WLAN_ZERO_COPY)

#if CFG_ENABLE_WIFI_TETHERING
        if (wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable) {
            peer_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_HS, u4Size, WNDRV_AP_RX_PAD + WNDRV_PKT_DESCRIPTOR_SIZE + WNDRV_WLAN_MAC_HEADER_LEN + MBUF_LENGTH , 0);
        } else
#endif /*CFG_ENABLE_WIFI_TETHERING*/
        {
            if (wifi_under_cam_mode == 1) {
                int j = 0;
                do {
#ifdef  __WIFI_AP_STA_CONCURRENCY__
                    peer_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_HS, u4Size, WNDRV_RX_PAD + WNDRV_PKT_DESCRIPTOR_SIZE + MBUF_LENGTH, 0);
#else
                    peer_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_DL, u4Size, WNDRV_RX_PAD + WNDRV_PKT_DESCRIPTOR_SIZE + MBUF_LENGTH, 0);
#endif
                    if (peer_p == NULL) {
                        kalMsleep(1);
                    }
                } while (peer_p == NULL && ++j < 5);
            } else {
#ifdef  __WIFI_AP_STA_CONCURRENCY__
                peer_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_HS, u4Size, WNDRV_RX_PAD + WNDRV_PKT_DESCRIPTOR_SIZE + MBUF_LENGTH, 0);
#else
                peer_p = (peer_buff_struct *)flc2_get_peer_buff(FLC2_POOL_ID_SPDU_DL, u4Size, WNDRV_RX_PAD + WNDRV_PKT_DESCRIPTOR_SIZE + MBUF_LENGTH, 0);
#endif
            }
        }
#endif
#if CFG_ENABLE_THROUGHPUT_DBG
        SLA_CustomLogging("sb1", SA_stop);
#endif

        if (peer_p == NULL) {
#if 0
            /* DEBUG */
            P_TX_CTRL_T prTxCtrl;
            P_RX_CTRL_T prRxCtrl;
            int idx;

            prTxCtrl = &wndrv_context.adpt_p->rTxCtrl;
            prRxCtrl = &wndrv_context.adpt_p->rRxCtrl;

            for (idx = TXQ_AC0; idx < TXQ_NUM; idx++) {
                MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "TXQ %d [NUM]: %d %d %d %d %d %d\n\r", idx,
                             (int) prTxCtrl->arTcQPara[idx].ucMaxNumOfMsduInfo,
                             (int) prTxCtrl->arTcQPara[idx].rFreeMsduInfoList.u4NumElem,
                             (int) prTxCtrl->arTcQPara[idx].rOsSendQueue.u4NumElem,
                             (int) prTxCtrl->arTxACQPara[idx].rSendWaitQueue.u4NumElem,
                             (int) prTxCtrl->arTxACQPara[idx].rActiveChainList.u4NumElem,
                             (int) prTxCtrl->arTxACQPara[idx].rFreeTFCBList.u4NumElem);
                MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "TXQ %d [size]: %d %d %d", idx,
                             DWORD_TO_BYTE(prTxCtrl->arTxACQPara[idx].u2MaxBufferSizeDW),
                             DWORD_TO_BYTE(prTxCtrl->arTxACQPara[idx].u2FreeBufferSizeDW),
                             prTxCtrl->arTxACQPara[idx].ucMaxNumOfSwTfcb);
            }
            MT5931_TRACE(TRACE_WARNING, MT5931_INFO_, "RFB: %d %d %d",
                         prRxCtrl->rFreeRFBWithBuffList.u4NumElem,
                         prRxCtrl->rFreeRFBWOBuffList.u4NumElem,
                         prRxCtrl->rReceivedRFBList.u4NumElem);
            /* DEBUG */
#endif

#if CFG_ENABLE_THROUGHPUT_DBG
            SLA_CustomLogging("sb2", SA_start);
#endif

            WNDRV_TRACE0(TRACE_WARNING, WARNING_NO_FLC_BUFFER,
                         "Cannot allocate flow control buffer");
            //kal_sleep_task(1);
            WNDRVPauseFlag = KAL_TRUE;
            WNDRVNoFLC2Flag = KAL_TRUE;
#if defined(MT6250)
            rx_flow_ctrl_no_flc = 1;
#endif
            /*peer_p = construct_peer_buff ( u4Size, WNDRV_PKT_DESCRIPTOR_SIZE, 0, TD_UL );*/
            {
                peer_p = construct_peer_buff(u4Size + CPU_CACHE_LINE_SIZE, WNDRV_PKT_DESCRIPTOR_SIZE, 0, TD_UL);
                /* NOTE(Nelson): update free_header_space for 32-byte align of valid data address in peer buf */
                sys_peerbuf_rx_pad = CPU_CACHE_LINE_SIZE -
                                     (((UINT_32)peer_p + PEER_BUF_HDR_SIZE + WNDRV_PKT_DESCRIPTOR_SIZE) & CPU_CACHE_LINE_SIZE_MASK);
                remove_hdr_of_peer_buff(peer_p, sys_peerbuf_rx_pad);

            }
            WNDRV_EXT_ASSERT(peer_p != NULL,
                             "kalPacketAlloc: system buffer not available",
                             u4Size, 0, 0);

            wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

            wndrv_pkt_p->src_mod_id = MOD_WNDRV;
            wndrv_pkt_p->flc_mode   = KAL_FALSE;
            wndrv_pkt_p->pool_id    = FLC2_POOL_ID_APPPDU_DL;
            wndrv_pkt_p->check_mark = WNDRV_BUFFER_HDLR_CHECK_MARK;
            wndrv_pkt_p->reserved0  = 0; /* ADD FOR HOSTAP */
            wndrv_pkt_p->tx_peer_p  = peer_p;


            prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);
            prPacketInfo->wndrv_pkt_p = wndrv_pkt_p;
            prPacketInfo->prPacket = (P_NATIVE_PACKET)peer_p;
            prPacketInfo->ucMacHeaderLength = WLAN_MAC_HEADER_LEN;
            prPacketInfo->u2PayloadLength = pdu_len - WLAN_MAC_HEADER_LEN;
            wndrv_pkt_p->prPacketInfo = prPacketInfo;

            wndrv_context.glue_p->rx_sys_peer_num++;
#if CFG_ENABLE_THROUGHPUT_DBG
            SLA_CustomLogging("sb2", SA_stop);
#endif
            // James modify for optimization
            //WNDRV_TRACE2( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_ALLOC_SYS,
            //              "WKAL: PacketAlloc: rx_sys_peer_num=%d, pdu_len=%d",
            //              wndrv_context.rx_sys_peer_num, u4Size );
        } else {
            wndrv_pkt_p = wndrv_get_pkt_descriptor(peer_p);

            wndrv_pkt_p->src_mod_id    = MOD_WNDRV;
            // James modify for new flow control
#if CFG_ENABLE_WIFI_TETHERING
            if (wndrv_context.glue_p->rWtInfo.fgIsTetheringEnable) {
                wndrv_pkt_p->pool_id = FLC2_POOL_ID_SPDU_HS;
            } else
#endif /* CFG_ENABLE_WIFI_TETHERING */
            {
#ifdef  __WIFI_AP_STA_CONCURRENCY__
                wndrv_pkt_p->pool_id = FLC2_POOL_ID_SPDU_HS;
#else
                wndrv_pkt_p->pool_id = FLC2_POOL_ID_SPDU_DL;
#endif
            }
            wndrv_pkt_p->flc_mode      = KAL_TRUE;
            wndrv_pkt_p->check_mark    = WNDRV_BUFFER_HDLR_CHECK_MARK;
            wndrv_pkt_p->reserved0  = 0; /* ADD FOR HOSTAP */
            wndrv_pkt_p->tx_peer_p  = peer_p;

            prPacketInfo = (P_PACKET_INFO_T) & (wndrv_pkt_p->pkt_info);

            prPacketInfo->wndrv_pkt_p = wndrv_pkt_p;
            prPacketInfo->prPacket = (P_NATIVE_PACKET)peer_p;
            prPacketInfo->ucMacHeaderLength = WLAN_MAC_HEADER_LEN;
            prPacketInfo->u2PayloadLength = pdu_len - WLAN_MAC_HEADER_LEN;
            wndrv_pkt_p->prPacketInfo = prPacketInfo;


            wndrv_context.glue_p->rx_flc_peer_num++;

            // James modify for optimization
            //WNDRV_TRACE2( TRACE_GROUP_4, INFO_WNDRV_KAL_PKT_ALLOC_FLC,
            //              "WKAL: PacketAlloc: rx_flc_peer_num=%d, pdu_len=%d",
            //              wndrv_context.rx_flc_peer_num, u4Size );
        }
    }
    // James modify for catcher, end
    WNDRV_EXT_ASSERT(peer_p != NULL, "kalPacketAlloc: buffer can't be NULL", 0, 0, 0);

    u4PktAllocIndex ++;
    u4PktAllocIndex %= 255;
    v4PktAllocRecord [u4PktAllocIndex] = peer_p;

    if (peer_p) {
        //record = peer_p;
        *ppucData = (kal_uint8 *) get_pdu_ptr(peer_p, &pdu_len);

        kalMemSet(*ppucData, u4PktAllocIndex, CFG_RX_MAX_PKT_SIZE);
    }

    return (PVOID)peer_p;

#else   /* for Linux WLAN driver */
    struct sk_buff  *prSkb = dev_alloc_skb(u4Size);

    if (prSkb) {
        *ppucData = (PUINT_8)(prSkb->data);
    }
#if DBG
    {
        PUINT_32 pu4Head = (PUINT_32)&prSkb->cb[0];
        *pu4Head = (UINT_32)prSkb->head;
        DBGLOG(RX, TRACE, ("prSkb->head = %#lx, prSkb->cb = %#lx\n", (UINT_32)prSkb->head, *pu4Head));
    }
#endif
    return (PVOID) prSkb;
#endif
}


/*----------------------------------------------------------------------------*/
/*!
* \brief API for reading data on NVRAM
*
* \param[in]
*           prGlueInfo
*           u4Offset
* \param[out]
*           pu2Data
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalCfgDataRead16(
    IN P_GLUE_INFO_T    prGlueInfo,
    IN UINT_32          u4Offset,
    OUT PUINT_16        pu2Data
)
{
#if 0   //TODO(Nelson)
    if (pu2Data == NULL) {
        return FALSE;
    }

    if (nvram_read(WIFI_NVRAM_FILE_NAME,
                   (char *)pu2Data,
                   sizeof(unsigned short),
                   u4Offset) != sizeof(unsigned short)) {
        return FALSE;
    } else {
        return TRUE;
    }
#endif
    return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief API for writing data on NVRAM
*
* \param[in]
*           prGlueInfo
*           u4Offset
*           u2Data
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
kalCfgDataWrite16(
    IN P_GLUE_INFO_T    prGlueInfo,
    UINT_32             u4Offset,
    UINT_16             u2Data
)
{
#if 0   //TODO(Nelson)
    if (nvram_write(WIFI_NVRAM_FILE_NAME,
                    (char *)&u2Data,
                    sizeof(unsigned short),
                    u4Offset) != sizeof(unsigned short)) {
        return FALSE;
    } else {
        return TRUE;
    }
#endif
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is used to clear pending security frames
*        belongs to dedicated network type
*
* \param prGlueInfo         Pointer of GLUE Data Structure
* \param eNetworkTypeIdx    Network Type Index
*
* \retval none
*/
/*----------------------------------------------------------------------------*/
VOID
kalClearSecurityFramesByNetType(
    IN P_GLUE_INFO_T prGlueInfo,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;

    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;
    GLUE_SPIN_LOCK_DECLARATION();


    ASSERT(prGlueInfo);

    // Clear pending security frames in prGlueInfo->rCmdQueue
    prCmdQue = &prGlueInfo->rCmdQueue;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    while (prQueueEntry) {
        prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

        if (prCmdInfo->eCmdType == COMMAND_TYPE_SECURITY_FRAME &&
                prCmdInfo->eNetworkType == eNetworkTypeIdx) {
            prCmdInfo->pfCmdTimeoutHandler(prGlueInfo->prAdapter, prCmdInfo);
            cmdBufFreeCmdInfo(prGlueInfo->prAdapter, prCmdInfo);
        } else {
            QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    QUEUE_CONCATENATE_QUEUES(prCmdQue, prTempCmdQue);
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_CMD_QUE);
}


VOID
set_bit(
    IN  UINT_32 nr,
    OUT UINT_32 *addr
)
{
    *addr |= BIT(nr);
}


UINT_32 test_and_clear_bit(
    IN  UINT_32 nr,
    OUT UINT_32 *addr
)
{
    UINT_32 mask = BIT(nr);
    UINT_32 old;

    old = *addr;
    *addr = old & ~mask;

    return (old & mask) != 0;
}

