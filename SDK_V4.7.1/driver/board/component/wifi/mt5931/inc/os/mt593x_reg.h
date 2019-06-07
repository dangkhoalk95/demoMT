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

#ifndef _MT593X_REG_H
#define _MT593X_REG_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

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

//1 MT5931 MCR Definition

//2 Host Interface

#define WCIR        (0x0000)
#define WHLPCR      (0x0004)
#define WSDIOCSR    (0x0008)
#define WHCR        (0x000c)
#define WHISR       (0x0010)
#define WHIER       (0x0014)
#define WASR        (0x0018)
#define WSICR       (0x001c)
#define WTSR0       (0x0020)
#define WTSR1       (0x0024)
#define WTDR0       (0x0028)
#define WTDR1       (0x002c)
#define WRDR0       (0x0030)
#define WRDR1       (0x0034)
#define H2DSM0R     (0x0038)
#define H2DSM1R     (0x003c)
#define D2HRM0R     (0x0040)
#define D2HRM1R     (0x0044)
#define D2HRM2R     (0x0048)
#define WRPLR       (0x0050)
#define WEHTCR      (0x0054)

#define FWDLDR      (0x0080)
#define FWDLDSAR    (0x0084)
#define FWDLSR      (0x0088)
#define FWDLCMR0    (0x008c)
#define FWDLCMR1    (0x0090)

/* WCIR 0x0000 */
#define WCIR_WLAN_READY                 BIT(21)
#define WCIR_POR_INDICATOR              BIT(20)
#define WCIR_REVISION_ID                BITS(16,19)
#define WCIR_CHIP_ID                    BITS(0,15)

#define MTK_CHIP_REV                     0x00005931
#define MTK_CHIP_MP_REVERSION_ID         0x0

/* FWDLSR 0x0088 */
#define FWDLSR_FWDL_RDY                 BIT(8)
#define FWDLSR_FWDL_MODE                BIT(0)

/* WMCSR 0x008c */
#define WMCSR_DL_OK                     BIT(15)
#define WMCSR_DL_FAIL                   BIT(14)
#define WMCSR_PLLRDY                    BIT(13)
#define WMCSR_WF_ON                     BIT(12)
#define WMCSR_INI_RDY                   BIT(11)
#define WMCSR_WF_EN                     BIT(6)
#define WMCSR_SW_EN                     BIT(5)
#define WMCSR_SPLLEN                    BIT(4)
#define WMCSR_SPWREN                    BIT(3)
#define WMCSR_HSTOPIL                   BIT(2)
#define WMCSR_FWDLRST                   BIT(1)
#define WMCSR_FWDLEN                    BIT(0)

#define MT5931_E1_REVERSION_ID          0x0
#define MT5931_E2_REVERSION_ID          0x1
#define MT5931_E3_REVERSION_ID          0x2

#endif /* _MT593X_REG_H */

