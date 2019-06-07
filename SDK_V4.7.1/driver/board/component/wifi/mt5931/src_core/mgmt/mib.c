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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/mib.c#2 $
*/

/*! \file   "mib.c"
    \brief  This file includes the mib default vale and functions.
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
NON_HT_PHY_ATTRIBUTE_T rNonHTPhyAttributes[] = {
    {RATE_SET_HR_DSSS,      TRUE,       FALSE   },  /* For PHY_TYPE_HR_DSSS_INDEX(0) */
    {RATE_SET_ERP,          TRUE,       TRUE    },  /* For PHY_TYPE_ERP_INDEX(1) */
    {RATE_SET_ERP_P2P,      TRUE,       TRUE    },  /* For PHY_TYPE_ERP_P2P_INDEX(2) */
    {RATE_SET_OFDM,         FALSE,      FALSE   },  /* For PHY_TYPE_OFDM_INDEX(3) */
};

NON_HT_ADHOC_MODE_ATTRIBUTE_T rNonHTAdHocModeAttributes[AD_HOC_MODE_NUM] = {
    {PHY_TYPE_HR_DSSS_INDEX,    BASIC_RATE_SET_HR_DSSS},        /* For AD_HOC_MODE_11B(0) */
    {PHY_TYPE_ERP_INDEX,        BASIC_RATE_SET_HR_DSSS_ERP},    /* For AD_HOC_MODE_MIXED_11BG(1) */
    {PHY_TYPE_ERP_INDEX,        BASIC_RATE_SET_ERP},            /* For AD_HOC_MODE_11G(2) */
    {PHY_TYPE_OFDM_INDEX,       BASIC_RATE_SET_OFDM},           /* For AD_HOC_MODE_11A(3) */
};

NON_HT_AP_MODE_ATTRIBUTE_T rNonHTApModeAttributes[AP_MODE_NUM] = {
    {PHY_TYPE_HR_DSSS_INDEX,    BASIC_RATE_SET_HR_DSSS},        /* For AP_MODE_11B(0) */
    {PHY_TYPE_ERP_INDEX,        BASIC_RATE_SET_HR_DSSS_ERP},    /* For AP_MODE_MIXED_11BG(1) */
    {PHY_TYPE_ERP_INDEX,        BASIC_RATE_SET_ERP},            /* For AP_MODE_11G(2) */
    {PHY_TYPE_ERP_P2P_INDEX,    BASIC_RATE_SET_ERP_P2P},        /* For AP_MODE_11G_P2P(3) */
    {PHY_TYPE_OFDM_INDEX,       BASIC_RATE_SET_OFDM},           /* For AP_MODE_11A(4) */
};

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

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


