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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/rftest.h#2 $
*/

/*! \file   "rftest.h"
    \brief  definitions for RF Productino test

*/



#ifndef _RFTEST_H
#define _RFTEST_H

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
// Table Version
#define RF_AUTO_TEST_FUNCTION_TABLE_VERSION 0x01000001

// Power
#define RF_AT_PARAM_POWER_MASK      BITS(0,7)
#define RF_AT_PARAM_POWER_MAX       RF_AT_PARAM_POWER_MASK

// Rate
#define RF_AT_PARAM_RATE_MCS_MASK   BIT(31)
#define RF_AT_PARAM_RATE_MASK       BITS(0,7)
#define RF_AT_PARAM_RATE_CCK_MAX    3
#define RF_AT_PARAM_RATE_1M         0
#define RF_AT_PARAM_RATE_2M         1
#define RF_AT_PARAM_RATE_5_5M       2
#define RF_AT_PARAM_RATE_11M        3
#define RF_AT_PARAM_RATE_6M         4
#define RF_AT_PARAM_RATE_9M         5
#define RF_AT_PARAM_RATE_12M        6
#define RF_AT_PARAM_RATE_18M        7
#define RF_AT_PARAM_RATE_24M        8
#define RF_AT_PARAM_RATE_36M        9
#define RF_AT_PARAM_RATE_48M        10
#define RF_AT_PARAM_RATE_54M        11

// Antenna
#define RF_AT_PARAM_ANTENNA_ID_MASK BITS(0,7)
#define RF_AT_PARAM_ANTENNA_ID_MAX  1

// Packet Length
#define RF_AT_PARAM_TX_80211HDR_BYTE_MAX     (32)
#define RF_AT_PARAM_TX_80211PAYLOAD_BYTE_MAX (2048)

#define RF_AT_PARAM_TX_PKTLEN_BYTE_DEFAULT  1024
#define RF_AT_PARAM_TX_PKTLEN_BYTE_MAX  \
    ((UINT_16)(RF_AT_PARAM_TX_80211HDR_BYTE_MAX + RF_AT_PARAM_TX_80211PAYLOAD_BYTE_MAX ))

// Packet Count
#define RF_AT_PARAM_TX_PKTCNT_DEFAULT    1000
#define RF_AT_PARAM_TX_PKTCNT_UNLIMITED  0

// Packet Interval
#define RF_AT_PARAM_TX_PKT_INTERVAL_US_DEFAULT  50

// ALC
#define RF_AT_PARAM_ALC_DISABLE     0
#define RF_AT_PARAM_ALC_ENABLE      1

// TXOP
#define RF_AT_PARAM_TXOP_DEFAULT    0
#define RF_AT_PARAM_TXOPQUE_QMASK   BITS(16,31)
#define RF_AT_PARAM_TXOPQUE_TMASK   BITS(0,15)
#define RF_AT_PARAM_TXOPQUE_AC0     (0<<16)
#define RF_AT_PARAM_TXOPQUE_AC1     (1<<16)
#define RF_AT_PARAM_TXOPQUE_AC2     (2<<16)
#define RF_AT_PARAM_TXOPQUE_AC3     (3<<16)
#define RF_AT_PARAM_TXOPQUE_AC4     (4<<16)
#define RF_AT_PARAM_TXOPQUE_QOFFSET 16

// Retry Limit
#define RF_AT_PARAM_TX_RETRY_DEFAULT    0
#define RF_AT_PARAM_TX_RETRY_MAX        6

// QoS Queue
#define RF_AT_PARAM_QOSQUE_AC0      0
#define RF_AT_PARAM_QOSQUE_AC1      1
#define RF_AT_PARAM_QOSQUE_AC2      2
#define RF_AT_PARAM_QOSQUE_AC3      3
#define RF_AT_PARAM_QOSQUE_AC4      4
#define RF_AT_PARAM_QOSQUE_DEFAULT  RF_AT_PARAM_QOSQUE_AC0

// Bandwidth
#define RF_AT_PARAM_BANDWIDTH_20MHZ             0
#define RF_AT_PARAM_BANDWIDTH_40MHZ             1
#define RF_AT_PARAM_BANDWIDTH_U20_IN_40MHZ      2
#define RF_AT_PARAM_BANDWIDTH_D20_IN_40MHZ      3
#define RF_AT_PARAM_BANDWIDTH_DEFAULT   RF_AT_PARAM_BANDWIDTH_20MHZ

// GI (Guard Interval)
#define RF_AT_PARAM_GI_800NS    0
#define RF_AT_PARAM_GI_400NS    1
#define RF_AT_PARAM_GI_DEFAULT  RF_AT_PARAM_GI_800NS

// STBC
#define RF_AT_PARAM_STBC_DISABLE    0
#define RF_AT_PARAM_STBC_ENABLE     1

// RIFS
#define RF_AT_PARAM_RIFS_DISABLE    0
#define RF_AT_PARAM_RIFS_ENABLE     1

#define RF_AT_PARAM_PACKET_UNLIMITED  0

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
// Function ID List
typedef enum _ENUM_RF_AT_FUNCID_T {
    RF_AT_FUNCID_VERSION = 0,
    RF_AT_FUNCID_COMMAND,
    RF_AT_FUNCID_POWER,
    RF_AT_FUNCID_RATE,
    RF_AT_FUNCID_PREAMBLE,
    RF_AT_FUNCID_ANTENNA,       // Not supported on MT6620
    RF_AT_FUNCID_PKTLEN,
    RF_AT_FUNCID_PKTCNT,
    RF_AT_FUNCID_PKTINTERVAL,
    RF_AT_FUNCID_ALC,
    RF_AT_FUNCID_TXOPLIMIT,     // Not supported on MT6620
    RF_AT_FUNCID_ACKPOLICY,     // Not supported on MT6620
    RF_AT_FUNCID_PKTCONTENT,    // Not supported on MT6620
    RF_AT_FUNCID_RETRYLIMIT,    // Not supported on MT6620
    RF_AT_FUNCID_QUEUE,         // Not supported on MT6620
    RF_AT_FUNCID_BANDWIDTH,
    RF_AT_FUNCID_GI,
    RF_AT_FUNCID_STBC,          // Not supported on MT6620
    RF_AT_FUNCID_CH_FREQ,       // Not supported on MT6620
    RF_AT_FUNCID_RIFS,
    RF_AT_FUNCID_TXPWRMODE = 31,
    RF_AT_FUNCID_TXCOUNT,
    RF_AT_FUNCID_TXOKCOUNT,
    RF_AT_FUNCID_RXOKCNT,
    RF_AT_FUNCID_RXERRORCNT,
    RF_AT_FUNCID_TSSI = 38,
    RF_AT_FUNCID_THERMO = 43,
    RF_AT_FUNCID_VOLT,
    RF_AT_FUNCID_EFUSE,
    RF_AT_FUNCID_NUM
} ENUM_RF_AT_FUNCID_T;

// Command
typedef enum _ENUM_RF_AT_COMMAND_T {
    RF_AT_COMMAND_STOPTEST = 0,
    RF_AT_COMMAND_STARTTX,
    RF_AT_COMMAND_STARTRX,
    RF_AT_COMMAND_RESET,
    RF_AT_COMMAND_OUTPUT_POWER,
    RF_AT_COMMAND_LOCAL_FREQ,
    RF_AT_COMMAND_CARRIER_SUPP,
    RF_AT_COMMAND_NUM
} ENUM_RF_AT_COMMAND_T;

// Preamble
typedef enum _ENUM_RF_AT_PREAMBLE_T {
    RF_AT_PREAMBLE_NORMAL = 0,
    RF_AT_PREAMBLE_CCK_SHORT,
    RF_AT_PREAMBLE_11N_MM,
    RF_AT_PREAMBLE_11N_GF,
    RF_AT_PREAMBLE_NUM
} ENUM_RF_AT_PREAMBLE_T;

// Ack Policy
typedef enum _ENUM_RF_AT_ACK_POLICY_T {
    RF_AT_ACK_POLICY_NORMAL = 0,
    RF_AT_ACK_POLICY_NOACK,
    RF_AT_ACK_POLICY_NOEXPLICTACK,
    RF_AT_ACK_POLICY_BLOCKACK,
    RF_AT_ACK_POLICY_NUM
} ENUM_RF_AT_ACK_POLICY_T;

typedef enum _ENUM_RF_AUTOTEST_STATE_T {
    RF_AUTOTEST_STATE_STANDBY = 0,
    RF_AUTOTEST_STATE_TX,
    RF_AUTOTEST_STATE_RX,
    RF_AUTOTEST_STATE_RESET,
    RF_AUTOTEST_STATE_OUTPUT_POWER,
    RF_AUTOTEST_STATE_LOCA_FREQUENCY,
    RF_AUTOTEST_STATE_CARRIER_SUPRRESION,
    RF_AUTOTEST_STATE_NUM
} ENUM_RF_AUTOTEST_STATE_T;


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

WLAN_STATUS
rftestSetATInfo(
    IN P_ADAPTER_T  prAdapter,
    UINT_32         u4FuncIndex,
    UINT_32         u4FuncData
);

WLAN_STATUS
rftestQueryATInfo(
    IN P_ADAPTER_T  prAdapter,
    UINT_32         u4FuncIndex,
    UINT_32         u4FuncData,
    OUT PVOID       pvQueryBuffer,
    IN UINT_32      u4QueryBufferLen
);

WLAN_STATUS
rftestSetFrequency(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4FreqInKHz,
    IN PUINT_32     pu4SetInfoLen
);

#endif /* _RFTEST_H */



