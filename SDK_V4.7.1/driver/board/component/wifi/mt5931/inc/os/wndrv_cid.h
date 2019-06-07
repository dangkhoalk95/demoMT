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

#ifndef _WNDRV_CID_H
#define _WNDRV_CID_H

#ifndef _WNDRV_CNST_H
#error "wndrv_cnst.h should be included before wndrv_cid.h"
#endif

/*******************************************************************************
*
*  1) BB Chip
*
*******************************************************************************/
//2008116 modify by saker for add MT5921 definition
#if defined( WIFI_BB_MT5911 )
#define WIFI_BB_CHIP    WNDRV_BB_MT5911
#elif defined ( WIFI_BB_MT5921 )
#define WIFI_BB_CHIP    WNDRV_BB_MT5921
#define WIFI_RF_CHIP    WNDRV_RF_MT5921
#elif defined ( MT5931 )
#define WIFI_BB_CHIP    WNDRV_BB_MT5931
#define WIFI_RF_CHIP    WNDRV_RF_MT5931
#else
#error "No BB Chip declared"
#endif

#define  IS_BB_CHIP_MT5911     ( WIFI_BB_CHIP  == WNDRV_BB_MT5911  )
//20081106 modify by saker for MT5921 definition
#define  IS_BB_CHIP_MT5921     ( WIFI_BB_CHIP  == WNDRV_BB_MT5921  )
#define  IS_RF_CHIP_MT5921     ( WIFI_RF_CHIP  == WNDRV_RF_MT5921  )
//20110509 modify by Nelson for MT5931 definition
#define  IS_BB_CHIP_MT5931     ( WIFI_BB_CHIP  == WNDRV_BB_MT5931  )
#define  IS_RF_CHIP_MT5931     ( WIFI_RF_CHIP  == WNDRV_RF_MT5931  )

/*******************************************************************************
*
*  2) RF Chip
*
*******************************************************************************/
#if (IS_BB_CHIP_MT5911)

#if defined( WIFI_RF_AL2236 )
#define WIFI_RF_CHIP    WNDRV_RF_AL2236
#elif defined( WIFI_RF_MAX2827 )
#define WIFI_RF_CHIP    WNDRV_RF_MAX2827
#else
#error "No RF Chip declared"
#endif

#endif
#define  IS_RF_CHIP_AL2236    ( WIFI_RF_CHIP == WNDRV_RF_AL2236 )
#define  IS_RF_CHIP_MAX2827   ( WIFI_RF_CHIP == WNDRV_RF_MAX2827 )


#endif /* _WNDRV_CID_H */
