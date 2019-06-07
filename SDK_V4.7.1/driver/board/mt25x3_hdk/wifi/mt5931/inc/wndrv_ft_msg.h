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

#ifndef _WNDRV_FT_MSG_H
#define _WNDRV_FT_MSG_H

#include "wndrv_ft_types.h"
#include "wndrv_cal.h"
//#include "kal_non_specific_general_types.h"
#include "kal_general_types.h"
/*******************************************************************************
*
*  1) Function Interface
*
*******************************************************************************/
void wndrv_cal_query_chip_capability(wndrv_query_chip_capability_struct *cap);
void wndrv_cal_macaddr(wndrv_cal_mac_addr_struct          *mac);
void wndrv_cal_txpwr_2400M(wndrv_cal_txpwr_2400M_struct       *txpwr);
void wndrv_cal_txpwr_5000M(wndrv_cal_txpwr_5000M_struct       *txpwr);
void wndrv_cal_dac_dc_offset(wndrv_cal_dac_dc_offset_struct     *dcOffset);

/* Single Antenna Support Usage */
kal_bool bwcs_query_single_antenna_capability(void);
kal_bool bwcs_query_antenna_path(void);
void bwcs_set_antenna_path(kal_bool fgAntForBT);


/*----------------------------MMI/WIFI------------------------------------*/
void RFTool_WiFi_Stop(void);
void RFTool_WiFi_EnterTestMode(void);
void RFTool_WiFi_EnterNormalMode(void);
void RFTool_WiFi_DacDCOffset(kal_uint8  *i_ch, kal_uint8  *q_ch);
void RFTool_WiFi_ContTx(kal_uint8 country_code[2], wndrv_test_tx_struct cont_tx);
void RFTool_WiFi_CarrierSupp(kal_uint8 country_code[2], wndrv_test_tx_struct carrier_supp);
void RFTool_WiFi_LocalFreq(kal_uint8 country_code[2], wndrv_test_tx_struct local_freq);
void RFTool_WiFi_PktTx(kal_uint8 country_code[2], wndrv_test_pkt_tx_struct pkt_tx);
void RFTool_WiFi_PktTxEX(kal_uint8 country_code[2], wndrv_test_pkt_tx_structEX pkt_tx);
void RFTool_WiFi_PktRx(kal_uint32 ch_freq, void (* callback)(wndrv_test_rx_status_struct *result));
void RFTool_WiFi_BW(kal_uint8 bandwidth);
void RFTool_WiFi_Query_PktRx(void);
void RFTool_WiFi_PwrMgt(wndrv_test_pwr_mgt_enum pwr_mgt);
void RFTool_WiFi_MCR32_Write(kal_uint32  mcr_index,  kal_uint32  mcr32);
void RFTool_WiFi_MCR32_Read(kal_uint32  mcr_index,  kal_uint32  *mcr32);
void RFTool_WiFi_EFUSE_Write(kal_uint32  mcr_index,  kal_uint32  mcr32);
void RFTool_WiFi_EFUSE_Read(kal_uint32  mcr_index,  kal_uint32  *mcr32);
void RFTool_WiFi_MCR16_Write(kal_uint32  mcr_index,  kal_uint16  mcr16);
void RFTool_WiFi_MCR16_Read(kal_uint32  mcr_index,  kal_uint16  *mcr16);
void RFTool_WiFi_BBCR_Write(kal_uint32  bbcr_index,  kal_uint8   bbcr);
void RFTool_WiFi_BBCR_Read(kal_uint32  bbcr_index,  kal_uint8   *bbcr);
void RFTool_WiFi_EEPROM_Write(kal_uint32  eeprom_index,  kal_uint16  eeprom);
void RFTool_WiFi_EEPROM_Read(kal_uint32  eeprom_index,  kal_uint16  *eeprom);

/*******************************************************************************
*
*  Message Interface
*
*******************************************************************************/
/***************************************************************************
*  PRIMITIVE STRUCTURE
*     wndrv_test_set_req_struct
*
*  DESCRIPTION
*     Configure and set parameters to network driver.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_uint32                  token;
    wndrv_test_set_data_enum    type;    /* A list of set commands */
    wndrv_test_set_data_union   data;    /* Unioin of setting data structures */
    /* When type is one of the following values,
       some setting data are put in peer buffer.
    - WNDRV_TEST_SET_MCR32
    - WNDRV_TEST_SET_MCR16
    - WNDRV_TEST_SET_BBCR
    - WNDRV_ TEST_SET_EEPROM */
} wndrv_test_set_req_struct;


/***************************************************************************
*  PRIMITIVE STRUCTURE
*     wndrv_test_set_cnf_struct
*
*  DESCRIPTION
*     Confirmation of MSG_ID_WNDRV_TEST_SET_REQ.
***************************************************************************/
typedef struct {
    kal_uint8                   ref_count;
    kal_uint16                  msg_len;
    kal_uint32                  token;
    wndrv_test_set_data_enum    type;        /* A list of set commands */
    kal_int32                   successful;  /* 0: success other values: failure */
} wndrv_test_set_cnf_struct;


/***************************************************************************
*  PRIMITIVE STRUCTURE
*     wndrv_test_query_req_struct
*
*  DESCRIPTION
*     Query responses or status of network driver.
***************************************************************************/
typedef struct {
    kal_uint8                     ref_count;
    kal_uint16                    msg_len;
    kal_uint32                    token;
    wndrv_test_query_result_enum  type;    /* A list of query commands */
    /* When type is one of the following values,
       wndrv_test_peer_buff_query_struct are put
       in peer buffer.
       - WNDRV_TEST_QUERY_MCR32
    - WNDRV_TEST_QUERY_MCR16
    - WNDRV_TEST_QUERY_BBCR
    - WNDRV_ TEST_QUERY_EEPROM */
} wndrv_test_query_req_struct;

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     wndrv_test_query_cnf_struct
*
*  DESCRIPTION
*     Confirmation of MSG_ID_WNDRV_TEST_QUERY_REQ.
***************************************************************************/
typedef struct {
    kal_uint8                     ref_count;
    kal_uint16                    msg_len;
    kal_uint32                    token;
    wndrv_test_query_result_enum  type;    /* A list of query commands */
    wndrv_test_query_result_union result;  /* Unioin of  query results structure */
    /* When type is one of the following values,
       some query results are put in peer buffer.
       - WNDRV_TEST_QUERY_MCR32
    - WNDRV_TEST_QUERY_MCR16
    - WNDRV_TEST_QUERY_BBCR
    - WNDRV_ TEST_QUERY_EEPROM */
} wndrv_test_query_cnf_struct;


#endif /* end of _WNDRV_FT_MSG_H */
