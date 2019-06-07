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

#ifndef _WNDRV_TCPIP_MSG_H
#define _WNDRV_TCPIP_MSG_H

#include "wndrv_cnst.h"
//#include "kal_non_specific_general_types.h"
#include "kal_general_types.h"
//#include "if.h"
/***************************************************************************
*  PRIMITIVE STRUCTURE
*     tcpip_wndrv_data_req_struct
*
*  DESCRIPTION
*     Request data transmission.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    src_mac_addr[ WNDRV_MAC_ADDRESS_LEN ];
    kal_uint8    dest_mac_addr[ WNDRV_MAC_ADDRESS_LEN ];
    kal_uint16   proto_type;
    kal_uint8    flc_pool_type;
    kal_uint8    pool_id;
    kal_uint8    tid;
    kal_uint8    flags;
} tcpip_wndrv_data_req_struct;

#define TCPIP_WNDRV_DATA_REQ_FORCE_PS   (1)

/***************************************************************************
*  PRIMITIVE STRUCTURE
*     tcpip_wndrv_data_ind_struct
*
*  DESCRIPTION
*     Indicate data received.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_uint8    src_mac_addr[ WNDRV_MAC_ADDRESS_LEN ];
    kal_uint8    dest_mac_addr[ WNDRV_MAC_ADDRESS_LEN ];
    kal_uint16   proto_type;
#ifdef   __WIFI_AP_STA_CONCURRENCY__
    kal_bool     fgIsAPorSTA;    //TRUE: AP mode;  FALSE: STA mode
#endif
} tcpip_wndrv_data_ind_struct;


/***************************************************************************
*  PRIMITIVE STRUCTURE
*     tcpip_wndrv_setbearer_req_struct
*
*  DESCRIPTION
*     Indicate data received.
***************************************************************************/
typedef struct {
    kal_uint8    ref_count;
    kal_uint16   msg_len;
    kal_bool status;
} tcpip_wndrv_setbearer_req_struct;

#endif /* end of _WNDRV_TCPIP_MSG_H */
