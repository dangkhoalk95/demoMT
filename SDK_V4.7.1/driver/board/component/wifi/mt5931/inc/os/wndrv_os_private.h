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

#ifndef _WNDRV_OS_PRIVATE_H
#define _WNDRV_OS_PRIVATE_H

#include <stdio.h>  //for the following sprintf
#include "precomp.h"
//#include "kal_release.h"

#include "stack_ltlcom.h"

//#include "app_ltlcom.h"

/*******************************************************************************
*
*  Enums
*
*******************************************************************************/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*
*  Function Prototypes of Other Modules
*
*******************************************************************************/
/*******************************************************************************
*
*  Function Prototypes
*
*******************************************************************************/
/* --- wndrv_main.c --------------------------------------------------------- */
void wndrv_main(ilm_struct *ilm_ptr);

/* --- wndrv_rftest_func.c -------------------------------------------------- */
void wndrv_test_set_handler(ilm_struct *ilm_ptr);
void wndrv_test_query_handler(ilm_struct *ilm_ptr);
void wndrv_test_set_test_mode(void);
void wndrv_test_set_normal_mode(void);
WLAN_STATUS rftestSetTestMode(PVOID pvAdapter, PVOID setBuffer_p, UINT_32 setBufferLen, PUINT_32 setInfoLen_p);
WLAN_STATUS rftestSetAbortTestMode(PVOID pvAdapter, PVOID setBuffer_p, UINT_32 setBufferLen, PUINT_32 setInfoLen_p);

/* --- wndrv_dcoffset_cal.c ----------------------------------------------------------- */
void getDACOffset(kal_uint8 *i_ch, kal_uint8 *q_ch);

/* --- wndrv_supc_func.c ----------------------------------------------------- */

void wndrv_supc_data_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_init_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_deinit_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_site_survey_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_join_network_req_hanlder(ilm_struct *ilm_ptr);
void wndrv_supc_disjoin_network_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_country_setting_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_remove_key_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_add_key_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_pmkid_update_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_auth_state_update_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_blacklist_update_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_chip_poweron_req_handler(ilm_struct *ilm_ptr);
void wndrv_supc_chip_poweroff_req_handler(ilm_struct *ilm_ptr);
extern void wndrv_supc_query_capability_req_handler(ilm_struct *ilm_ptr);
void tcpip_wndrv_data_req_handler(ilm_struct *ilm_ptr);
void tcpip_wndrv_setbearer_req_handler(ilm_struct *ilm_ptr);
void wndrv_wmt_set_bwcs_req_handler(ilm_struct *ilm_ptr);
void wndrv_wmt_query_rssi_req_handler(ilm_struct *ilm_ptr);
void wndrv_wmt_set_ant_req_handler(ilm_struct *ilm_ptr);
void wndrv_wmt_set_flow_ctrl_req_handler(ilm_struct *ilm_ptr);
void wndrv_wmt_set_fixed_rx_gain_req_handler(ilm_struct *ilm_ptr);
void wndrv_wmt_set_omit_low_rate_req_handler(ilm_struct *ilm_ptr);
void wndrv_bwcs_fw_event_handler(void *pvBuf);
void wndrv_bwcs_set_sco_pta_periodicity_handler(UINT_32 timer_switch, UINT_32 wifi_ms, UINT_32 bt_ms);

void wndrv_supc_network_connect_ind_handler(void);
void wndrv_supc_network_disconnect_ind_handler(void);
void wndrv_supc_specific_ind_handler(void *pvBuf);
void wndrv_supc_site_survey_ind_handler(void);
void wndrv_supc_network_statistics_ind_handler(void);
void wndrv_mmi_poweroff_req_handler(void);
void wndrv_WiFi_channel_ind_handler(kal_bool afh, kal_uint32 freq, kal_uint8 hb, kal_bool pta, kal_uint8 pta_action);
void wndrv_abm_dhcp_complete_handler(ilm_struct *ilm_ptr);
void wndrv_supc_network_connect_fail_ind_handler(void);

/* --- wndrv_inject_msg.c----------------------------------------------------*/
void wndrv_tst_inject_string_handler(ilm_struct *ilm_ptr);

/* --- wlan_mis.c ----------------------------------------------------------- */
void wndrv_evshed_timer_handler(void *msg_ptr);
void wndrv_get_poweron_mode(void);
void wndrv_init(void);
void wndrv_interrupt_handler(ilm_struct *ilm_ptr);
void wndrv_timer_expiry_handler(ilm_struct *ilm_ptr);
void wndrv_cal_init_dac_dc_offset(wndrv_cal_dac_dc_offset_struct *dcOffset);
void wndrv_cal_ALC_Slope_2400M(wndrv_cal_ALC_Slope_2400M_struct *pALCSlope);
void wndrv_cal_Trim_Thermo_value(wndrv_cal_setting_trim_thermo_struct *set);
void wndrv_cal_txpwr_cal_free_flow(wndrv_cal_txpwr_cal_free_flow_struct *TxPwrCal);
//void wndrv_read_cal_data       ( P_EEPROM_CTRL_T prEEPROMCtrl );

/* --- MT5931_firmware.c ----------------------------------------------------------- */
void wndrv_firmware_init(void);

/* --- wndrv_hostap_func.c ----------------------------------------------------------- */
void hostap_wndrv_msg_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_init_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_deinit_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_add_sta_req_handler(ilm_struct *ilm_ptr);
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
void hostap_wndrv_set_channel_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_set_encryption_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_get_encryption_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_frame_req_handler(ilm_struct *ilm_ptr);
void hostap_wndrv_frame_ind_handler(PVOID pvPacket, UINT_16 u2PacketLen);
void wndrv_send_hostap_add_sta_cnf_handler(kal_bool status, PUINT_8 pucStaAddr);
void wndrv_send_hostap_set_privacy_cnf_handler(void);


#endif /* _WNDRV_OS_PRIVATE_H */
