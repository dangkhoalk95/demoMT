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

/* ****************************************************************************
* Alternatively, this software may be distributed under the terms of BSD
* license.
*****************************************************************************/


extern void supc_timer_expiry_hdlr(ilm_struct *ilm_ptr);

/* Joseph:
 * Message handler of SUPC_MMI_WIFI_AP_LIST_REQ
 */
extern void supc_abm_wifi_ap_list_req_handler(ilm_struct *ilm_ptr);

/* :
 * Message handler of MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_REQ
 */
extern void supc_abm_wifi_get_connection_info_req_handler(ilm_struct *ilm_ptr);
/* Joseph:
 * Message handler of WNDRV_SUPC_DATA_IND
 */
extern void supc_data_ind_handler(ilm_struct *ilm_ptr);

void supc_abm_wifi_get_wps_credential_req_handler(ilm_struct *ilm_ptr);


extern void supc_deinit_req_handler(ilm_struct *ilm_ptr);


/* Joseph:
 * Message handler of WNDRV_SUPC_INIT_IND
 */
extern void supc_init_req_handler(ilm_struct *ilm_ptr);

extern void supc_init_cnf_handler(ilm_struct *ilm_ptr);

extern void supc_network_statistics_ind_handler(ilm_struct *ilm_ptr);

/* Joseph:
 * Message handler of WNDRV_SUPC_NETWORK_CONNECT_IND
 */
extern void supc_network_connect_ind_handler(ilm_struct *ilm_ptr);

extern void supc_network_connect_fail_ind_handler(ilm_struct *ilm_ptr);


/* Joseph:
 * Message handler of WNDRV_SUPC_NETWORK_DISCONNECT_IND
 */
extern void supc_network_disconnect_ind_handler(ilm_struct *ilm_ptr);


/* Joseph:
 * Message handler of WNDRV_SUPC_SITE_SURVEY_IND
 */
extern void supc_site_survey_ind_handler(ilm_struct *ilm_ptr);

extern void supc_pmkid_candidate_ind_handler(ilm_struct *ilm_ptr);


extern void supc_sim_read_cnf_handler(ilm_struct *ilm_ptr);


/* Joseph:
 * Todos:
 * 1. Save the AP selection mode into context
 * 2. Get the pointer of the profiles and store the information of the profiles
 * 3. Change the WPA state to WPA_SCANNING and starts to scan
 */
extern void supc_abm_wifi_activate_req_handler(ilm_struct *ilm_ptr);


/* Joseph:
 * 1. Change wap state
 * 2. Remove unnecessary information in the context
 *   a. bssid
 *   b. Profiles from ABM
 *   c. Keys
 * 3. Reinit eapol state
 */
extern void supc_abm_wifi_deactivate_req_handler(ilm_struct *ilm_ptr);
extern void supc_sim_ready_ind_handler(ilm_struct *ilm_ptr);
extern void supc_status_ind_handler(ilm_struct *ilm_ptr);
extern void supc_capbility_ind_handler(ilm_struct *ilm_ptr);
extern void supc_abm_wifi_connect_req_handler(ilm_struct *ilm_ptr);
extern void supc_abm_wifi_disconnect_req_handler(ilm_struct *ilm_ptr);
extern void supc_assoc_current_profile(void); //Kinki change for the blacklist
#if 1//#ifdef __CERTMAN_SUPPORT__
extern void supc_get_privkey_cnf_handler(ilm_struct *ilm_ptr);  //Kinki add for CertMan
extern void supc_get_cert_by_id_cnf_handler(ilm_struct *ilm_ptr);  //Kinki add for CertMan
extern void supc_get_cert_chain_cnf_handler(ilm_struct *ilm_ptr); //Kinki add for CertMan
#endif
