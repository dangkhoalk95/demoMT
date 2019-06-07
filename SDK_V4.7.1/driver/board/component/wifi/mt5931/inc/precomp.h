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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/precomp.h#12 $
*/

/*! \file   precomp.h
    \brief  Collection of most compiler flags are described here.

    In this file we collect all compiler flags and detail the driver behavior if
    enable/disable such switch or adjust numeric parameters.
*/




#ifndef _PRECOMP_H
#define _PRECOMP_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
/*------------------------------------------------------------------------------
 * MAUI includes
 *------------------------------------------------------------------------------
 */
#include "wlan_config.h"

/* WCP Modification Begin */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
//#include "stack_config.h"
/*WCP modification*///#include "kal_release.h"
/*WCP modification*/#include "wndrv_cnst.h"
/*WCP modification*/#include "wndrv_cid.h"
/*WCP modification*/#include "wndrv_cal.h"
/*WCP modification*/#include "kal_trace.h"
//#include "stack_common.h"
#include "stack_msgs.h"
//#include "app_ltlcom.h"
//#include "syscomp_config.h"
//#include "task_config.h"
//#include "task_main_func.h"
#include "stack_ltlcom.h"
//#include "stack_timer.h"

/*
#if !defined(_STACKLIB_H)
#include "stacklib.h"
#endif
#if !defined(_EVENT_SHED_H)
#include "event_shed.h"
#endif
#if !defined(_EVENT_SHED_DEFS_H)
#include "event_shed_defs.h"
#endif
*/
//#include "flc2_mem_mngr_config.h"
//#include "flc2_config.h"

//#include "soc_api.h"    /* for ntohl, etc */
#include "gl_typedef.h"
//#include "wndrv_typedef.h"
/* WCP Modification */ #include "wndrv_private.h"
/* WCP Modification End */

#include "gl_os.h"

#include "debug.h"

#include "link.h"
#include "wlan_queue.h"


/*------------------------------------------------------------------------------
 * .\include\mgmt
 *------------------------------------------------------------------------------
 */
#include "wlan_typedef.h"

#include "mac.h"

/* Dependency:  mac.h (MAC_ADDR_LEN) */
#include "wlan_def.h"

#if CFG_SUPPORT_SWCR
#include "swcr.h"
#endif

/*------------------------------------------------------------------------------
 * .\include\nic
 *------------------------------------------------------------------------------
 */
/* Dependency:  wlan_def.h (ENUM_NETWORK_TYPE_T) */
#include "cmd_buf.h"

/* Dependency:  mac.h (MAC_ADDR_LEN) */
#include "nic_cmd_event.h"

/* Dependency:  nic_cmd_event.h (P_EVENT_CONNECTION_STATUS) */
#include "nic.h"

#include "nic_init_cmd_event.h"

#include "hif_rx.h"
#include "hif_tx.h"

#include "nic_tx.h"

#include "wlan_config.h"

/* Dependency:  hif_rx.h (P_HIF_RX_HEADER_T) */
#include "nic_rx.h"

#include "que_mgt.h"

#if CFG_ENABLE_WIFI_DIRECT
#include "p2p_typedef.h"
#endif


/*------------------------------------------------------------------------------
 * .\include\mgmt
 *------------------------------------------------------------------------------
 */

#include "hem_mbox.h"

#include "scan.h"
#include "bss.h"

#include "wlan_lib.h"
#include "wlan_oid.h"
#include "wlan_bow.h"

#include "gl_kal.h"

#include "hal_wifi.h"


#if defined(MT6620)
#include "mt6620_reg.h"
#elif defined(MT5931)
#include "mt5931_reg.h"
#include "mt593x_reg.h"
#endif

#include "rlm.h"
#include "rlm_domain.h"
#include "rlm_protection.h"
#include "rlm_obss.h"
#include "rate.h"


#include "aa_fsm.h"

#include "cnm_timer.h"


#if CFG_ENABLE_BT_OVER_WIFI
#include "bow.h"
#include "bow_fsm.h"
#endif

#include "pwr_mgt.h"


#include "cnm.h"
/* Dependency:  aa_fsm.h (ENUM_AA_STATE_T), p2p_fsm.h (WPS_ATTRI_MAX_LEN_DEVICE_NAME) */
#include "cnm_mem.h"
#include "cnm_scan.h"


#include "privacy.h"

#include "mib.h"

#include "auth.h"
#include "assoc.h"

#if CFG_SUPPORT_ROAMING
#include "roaming_fsm.h"
#endif /* CFG_SUPPORT_ROAMING */

#include "ais_fsm.h"


#include "adapter.h"


#include "que_mgt.h"
#include "rftest.h"


#if CFG_RSN_MIGRATION
#include "rsn.h"
#include "sec_fsm.h"
#endif


#ifdef __WAPI_SUPPORT__
#include "wapi.h"
#endif

/*------------------------------------------------------------------------------
 * NVRAM structure
 *------------------------------------------------------------------------------
 */
#include "CFG_Wifi_File.h"

//#include "wndrv_map.h"
#include "wlan_kal.h"

/* WCP Modification */ #include "wndrv_queue.h"
#if 0
/* WCP Modification */ #include "wndrv_wlan_queue.h"
/* WCP Modification */ #include "wlan_kal.h"
/* WCP Modification */ #include "wndrv_api.h"
#if CFG_SUPPORT_802_11D
#include "domain.h"
#endif
#endif

#if CFG_ENABLE_WIFI_TETHERING
#include "wlan_wt.h"
#endif
#include "wndrv_trc.h"
/* WPS definitions */
#include "wps_defs.h"

//#include "flc2_ent_functions.h"



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
#if 0
extern msg_type wndrv_msg_mapping_table[];
extern module_type wndrv_module_mapping_table[];
extern sap_type wndrv_sap_mapping_table[];
extern trc_msg_type wndrv_trc_msg_mapping_table[];
extern flc2_pool_id_enum wndrv_flc2_mapping_table[];
#if defined(__HOTSPOT_SUPPORT__)
extern msg_type wndrv_hostap_msg_mapping_table[];
#endif /*__HOTSPOT_SUPPORT__*/
#endif
/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
extern kal_uint32 wndrv_get_pta_setting(int mode, int ant, int idx);

//extern kal_uint32 wifi_check_enum(kal_uint8 op);
#if !defined(__WIFI_HIF_SDIO__)
extern void wndrv_bus_power_control(kal_uint8 sel); /* LCD Control */
extern void wndrv_bus_hpi_module_control(kal_uint8 sel); /* LCD Control */
#endif
extern kal_uint16 wndrv_get_NVRAM_EF_WNDRV_TPCFF_LID(void);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _PRECOMP_H */


