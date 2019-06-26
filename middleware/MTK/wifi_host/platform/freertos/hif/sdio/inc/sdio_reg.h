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

#ifndef __SDIO_REG_H__
#define __SDIO_REG_H__

/*----------------------- DMA Driver Domain Control Register ----------------------*/
#define SDIO_IP_WCIR			(0x0000)
#define SDIO_IP_WHLPCR			(0x0004)
#define SDIO_IP_WSDIOCSR		(0x0008)
#define SDIO_IP_WHCR			(0x000C)
#define SDIO_IP_WHISR			(0x0010)
#define SDIO_IP_WHIER			(0x0014)
#define SDIO_IP_WASR			(0x0020)
#define SDIO_IP_WSICR			(0x0024)
#define SDIO_IP_WTSR0			(0x0028)
#define SDIO_IP_WTSR1			(0x002C)
#define SDIO_IP_WTDR0			(0x0030)
#define SDIO_IP_WTDR1			(0x0034)
#define SDIO_IP_WRDR0			(0x0050)
#define SDIO_IP_WRDR1			(0x0054)
#define SDIO_IP_WRDR2			(0x0058)
#define SDIO_IP_WRDR3			(0x005C)
#define SDIO_IP_H2DSM0R			(0x0070)
#define SDIO_IP_H2DSM1R			(0x0074)
#define SDIO_IP_D2HRM0R			(0x0078)
#define SDIO_IP_D2HRM1R			(0x007C)
#define SDIO_IP_D2HRM2R			(0x0080)
#define SDIO_IP_WRPLR			(0x0090)
#define SDIO_IP_WRPLR1			(0x0094)
#define SDIO_IP_SWPCDBGR        (0x0154)  // add for debug use to read cpu pc vaule
#define SDIO_IP_WTMDR       	(0x00B0)
#define SDIO_IP_WTMCR       	(0x00B4)
#define SDIO_IP_WTMDPCR0    	(0x00B8)	//Read pattern
#define SDIO_IP_WTMDPCR1    	(0x00BC)	//Write pattern

//SDIO_IP_WHLPCR
#define W_INT_EN_SET		(0x1)
#define W_INT_EN_CLR		(0x2)
#define W_FW_OWN_REQ_SET	(0x100)
#define W_DRV_OWN_STATUS	(0x100)
#define W_FW_OWN_REQ_CLR	(0x200)


//WSDIOCSR
#define DB_WR_BUSY_EN		(0x8)
#define DB_RD_BUSY_EN		(0x4)
#define SDIO_INT_CTL		(0x2)


//WHCR
#define RX_ENHANC_MODE		(0x10000)
#define MAX_HIF_RX_LEN_NUM	(0xF0)
#define RECV_MAIL_BOX_RD_CLR_EN	(0x4)
#define W_INT_CLR_CTRL		(0x2)


//WCIR
#define POR_INDICATOR		(0x100000)
#define W_FUNC_RDY			(0x200000)
#define REVISION_ID			(0xF0000)
#define CHIP_ID				(0xFFFF)

#endif

