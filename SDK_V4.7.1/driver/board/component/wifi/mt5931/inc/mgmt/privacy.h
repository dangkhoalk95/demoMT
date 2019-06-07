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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/privacy.h#2 $
*/

/*! \file   privacy.h
    \brief This file contains the function declaration for privacy.c.
*/




#ifndef _PRIVACY_H
#define _PRIVACY_H

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
#define MAX_KEY_NUM                             4
#define WEP_40_LEN                              5
#define WEP_104_LEN                             13
#define LEGACY_KEY_MAX_LEN                      16
#define CCMP_KEY_LEN                            16
#define TKIP_KEY_LEN                            32
#define MAX_KEY_LEN                             32
#define MIC_RX_KEY_OFFSET                       16
#define MIC_TX_KEY_OFFSET                       24
#define MIC_KEY_LEN                             8

#define WEP_KEY_ID_FIELD      BITS(0,29)
#define KEY_ID_FIELD          BITS(0,7)

#define IS_TRANSMIT_KEY       BIT(31)
#define IS_UNICAST_KEY        BIT(30)
#define IS_AUTHENTICATOR      BIT(28)

#define CIPHER_SUITE_NONE               0
#define CIPHER_SUITE_WEP40              1
#define CIPHER_SUITE_TKIP               2
#define CIPHER_SUITE_TKIP_WO_MIC        3
#define CIPHER_SUITE_CCMP               4
#define CIPHER_SUITE_WEP104             5
#define CIPHER_SUITE_BIP                6
#define CIPHER_SUITE_WEP128             7
#define CIPHER_SUITE_WPI                8

#define WPA_KEY_INFO_KEY_TYPE BIT(3) /* 1 = Pairwise, 0 = Group key */
#define WPA_KEY_INFO_MIC      BIT(8)
#define WPA_KEY_INFO_SECURE   BIT(9)

#define MASK_2ND_EAPOL       (WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC)


/*******************************************************************************
*                         D A T A   T Y P E S
********************************************************************************
*/

typedef struct _IEEE_802_1X_HDR {
    UINT_8      ucVersion;
    UINT_8      ucType;
    UINT_16     u2Length;
    /* followed by length octets of data */
} IEEE_802_1X_HDR, *P_IEEE_802_1X_HDR;

typedef struct _EAPOL_KEY {
    UINT_8 ucType;
    /* Note: key_info, key_length, and key_data_length are unaligned */
    UINT_8 aucKeyInfo[2]; /* big endian */
    UINT_8 aucKeyLength[2]; /* big endian */
    UINT_8 aucReplayCounter[8];
    UINT_8 aucKeyNonce[16];
    UINT_8 aucKeyIv[16];
    UINT_8 aucKeyRsc[8];
    UINT_8 aucKeyId[8]; /* Reserved in IEEE 802.11i/RSN */
    UINT_8 aucKeyMic[16];
    UINT_8 aucKeyDataLength[2]; /* big endian */
    /* followed by key_data_length bytes of key_data */
} EAPOL_KEY, *P_EAPOL_KEY;

/* WPA2 PMKID candicate structure */
typedef struct _PMKID_CANDICATE_T {
    UINT_8              aucBssid[MAC_ADDR_LEN];
    UINT_32             u4PreAuthFlags;
} PMKID_CANDICATE_T, *P_PMKID_CANDICATE_T;

#if 0
/* WPA2 PMKID cache structure */
typedef struct _PMKID_ENTRY_T {
    PARAM_BSSID_INFO_T  rBssidInfo;
    BOOLEAN             fgPmkidExist;
} PMKID_ENTRY_T, *P_PMKID_ENTRY_T;
#endif

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
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

VOID
secInit(
    IN P_ADAPTER_T          prAdapter,
    IN UINT_8               ucNetTypeIdx
);

VOID
secSetPortBlocked(
    IN P_ADAPTER_T          prAdapter,
    IN P_STA_RECORD_T       prSta,
    IN BOOLEAN              fgPort
);

BOOL
secCheckClassError(
    IN P_ADAPTER_T          prAdapter,
    IN P_SW_RFB_T           prSwRfb,
    IN P_STA_RECORD_T       prStaRec
);

BOOL
secTxPortControlCheck(
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsduInfo,
    IN P_STA_RECORD_T       prStaRec
);

BOOLEAN
secRxPortControlCheck(
    IN P_ADAPTER_T          prAdapter,
    IN P_SW_RFB_T           prSWRfb
);

VOID
secSetCipherSuite(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32     u4CipherSuitesFlags
);

BOOL
secProcessEAPOL(
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsduInfo,
    IN P_STA_RECORD_T       prStaRec,
    IN PUINT_8              pucPayload,
    IN UINT_16              u2PayloadLen
);

VOID
secHandleTxDoneCallback(
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        pMsduInfo,
    IN P_STA_RECORD_T       prStaRec,
    IN WLAN_STATUS          rStatus
);

BOOLEAN
secIsProtectedFrame(
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsdu,
    IN P_STA_RECORD_T       prStaRec
);

VOID
secClearPmkid(
    IN P_ADAPTER_T          prAdapter
);

BOOLEAN
secRsnKeyHandshakeEnabled(
    IN P_ADAPTER_T          prAdapter
);

BOOLEAN
secTransmitKeyExist(
    IN P_ADAPTER_T          prAdapter,
    IN P_STA_RECORD_T       prSta
);

BOOLEAN
secEnabledInAis(
    IN P_ADAPTER_T          prAdapter
);


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _PRIVACY_H */

