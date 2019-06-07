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



#ifndef DRIVER_MTK_H
#define DRIVER_MTK_H

#include "driver.h"

//#ifndef __MTK_TARGET__
typedef signed char        CHAR;
typedef signed short       INT16;
typedef signed long        INT32;
typedef unsigned char      UCHAR;
typedef unsigned short     UINT16;
typedef unsigned long      UINT32;
// typedef unsigned long long UINT64;
//#endif
typedef unsigned char      UCHAR;
typedef kal_uint64 UINT64;

#define IFNAMSIZ    32

struct pmkid_entry {
    struct pmkid_entry *next;
    u8 bssid[ETH_ALEN];
    u8 pmkid[16];
};

struct wpa_driver_mtk_data {
    void *ctx;
    char ifname[IFNAMSIZ + 1];
    int has_capability;
    int radio_enabled;
    int no_of_pmkid;
    struct wpa_driver_capa capa;
    struct pmkid_entry *pmkid;
    int event_sock;
    int ioctl_sock;
};

#define PARAM_MAX_SSID_LEN              32
typedef struct param_ssid_s {
    unsigned long             len;
    unsigned char             ssid[PARAM_MAX_SSID_LEN];
} param_ssid_t;

//typedef struct PACKET_OID_DATA {
//    ULONG Oid;
//    ULONG Length;
//    unsigned char Data[1];
//}PACKET_OID_DATA;

#ifndef OID_802_11_BSSID
#define OID_802_11_BSSID            0x0d010101
#define OID_802_11_SSID             0x0d010102
#define OID_802_11_INFRASTRUCTURE_MODE      0x0d010108
#define OID_802_11_ADD_WEP          0x0D010113
#define OID_802_11_REMOVE_WEP           0x0D010114
#define OID_802_11_DISASSOCIATE         0x0D010115
#define OID_802_11_BSSID_LIST           0x0d010217
#define OID_802_11_AUTHENTICATION_MODE      0x0d010118
#define OID_802_11_PRIVACY_FILTER       0x0d010119
#define OID_802_11_BSSID_LIST_SCAN      0x0d01011A
#define OID_802_11_WEP_STATUS           0x0d01011B
#define OID_802_11_ENCRYPTION_STATUS OID_802_11_WEP_STATUS
#define OID_802_11_ADD_KEY          0x0d01011D
#define OID_802_11_REMOVE_KEY           0x0d01011E
#define OID_802_11_ASSOCIATION_INFORMATION  0x0d01011F
#define OID_802_11_TEST             0x0d010120
#define OID_802_11_CAPABILITY           0x0d010122
#define OID_802_11_PMKID            0x0d010123

#define NDIS_802_11_LENGTH_SSID 32
#define NDIS_802_11_LENGTH_RATES 8
#define NDIS_802_11_LENGTH_RATES_EX 16

// #pragma pack(1)

typedef UCHAR NDIS_802_11_MAC_ADDRESS[6];

typedef struct NDIS_802_11_SSID {
    UINT32 SsidLength;
    UCHAR Ssid[NDIS_802_11_LENGTH_SSID];
} NDIS_802_11_SSID;

typedef INT32 NDIS_802_11_RSSI;

typedef enum NDIS_802_11_NETWORK_TYPE {
    Ndis802_11FH,
    Ndis802_11DS,
    Ndis802_11OFDM5,
    Ndis802_11OFDM24,
    Ndis802_11NetworkTypeMax
} NDIS_802_11_NETWORK_TYPE;

typedef struct NDIS_802_11_CONFIGURATION_FH {
    UINT32 Length;
    UINT32 HopPattern;
    UINT32 HopSet;
    UINT32 DwellTime;
} NDIS_802_11_CONFIGURATION_FH;

typedef struct NDIS_802_11_CONFIGURATION {
    UINT32 Length;
    UINT32 BeaconPeriod;
    UINT32 ATIMWindow;
    UINT32 DSConfig;
    NDIS_802_11_CONFIGURATION_FH FHConfig;
} NDIS_802_11_CONFIGURATION;

typedef enum NDIS_802_11_NETWORK_INFRASTRUCTURE {
    Ndis802_11IBSS,
    Ndis802_11Infrastructure,
    Ndis802_11AutoUnknown,
    Ndis802_11InfrastructureMax
} NDIS_802_11_NETWORK_INFRASTRUCTURE;

typedef enum NDIS_802_11_AUTHENTICATION_MODE {
    Ndis802_11AuthModeOpen,
    Ndis802_11AuthModeShared,
    Ndis802_11AuthModeAutoSwitch,
    Ndis802_11AuthModeWPA,
    Ndis802_11AuthModeWPAPSK,
    Ndis802_11AuthModeWPANone,
    Ndis802_11AuthModeWPA2,
    Ndis802_11AuthModeWPA2PSK,
    Ndis802_11AuthModeMax
} NDIS_802_11_AUTHENTICATION_MODE;

typedef enum NDIS_802_11_WEP_STATUS {
    Ndis802_11WEPEnabled,
    Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
    Ndis802_11WEPDisabled,
    Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
    Ndis802_11WEPKeyAbsent,
    Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
    Ndis802_11WEPNotSupported,
    Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
    Ndis802_11Encryption2Enabled,
    Ndis802_11Encryption2KeyAbsent,
    Ndis802_11Encryption3Enabled,
    Ndis802_11Encryption3KeyAbsent
} NDIS_802_11_WEP_STATUS, NDIS_802_11_ENCRYPTION_STATUS;

typedef enum NDIS_802_11_PRIVACY_FILTER {
    Ndis802_11PrivFilterAcceptAll,
    Ndis802_11PrivFilter8021xWEP
} NDIS_802_11_PRIVACY_FILTER;

typedef UCHAR NDIS_802_11_RATES[NDIS_802_11_LENGTH_RATES];
typedef UCHAR NDIS_802_11_RATES_EX[NDIS_802_11_LENGTH_RATES_EX];

typedef struct NDIS_WLAN_BSSID_EX {
    UINT32 Length;
    NDIS_802_11_MAC_ADDRESS MacAddress; /* BSSID */
    UCHAR Reserved[2];
    NDIS_802_11_SSID Ssid;
    UINT32 Privacy;
    NDIS_802_11_RSSI Rssi;
    NDIS_802_11_NETWORK_TYPE NetworkTypeInUse;
    NDIS_802_11_CONFIGURATION Configuration;
    NDIS_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
    NDIS_802_11_RATES_EX SupportedRates;
    UINT32 IELength;
    UCHAR IEs[1];
} NDIS_WLAN_BSSID_EX;

typedef struct NDIS_802_11_BSSID_LIST_EX {
    UINT32 NumberOfItems;
    NDIS_WLAN_BSSID_EX Bssid[1];
} NDIS_802_11_BSSID_LIST_EX;

typedef struct NDIS_802_11_FIXED_IEs {
    UCHAR Timestamp[8];
    UINT16 BeaconInterval;
    UINT16 Capabilities;
} NDIS_802_11_FIXED_IEs;

typedef struct NDIS_802_11_WEP {
    UINT32 Length;
    UINT32 KeyIndex;
    UINT32 KeyLength;
    UCHAR KeyMaterial[1];
} NDIS_802_11_WEP;

typedef UINT32 NDIS_802_11_KEY_INDEX;
typedef UINT64 NDIS_802_11_KEY_RSC;

typedef struct NDIS_802_11_KEY {
    UINT32 Length;
    UINT32 KeyIndex;
    UINT32 KeyLength;
    NDIS_802_11_MAC_ADDRESS BSSID;
    NDIS_802_11_KEY_RSC KeyRSC;
    UCHAR KeyMaterial[1];
} NDIS_802_11_KEY;

typedef struct NDIS_802_11_REMOVE_KEY {
    UINT32 Length;
    UINT32 KeyIndex;
    NDIS_802_11_MAC_ADDRESS BSSID;
} NDIS_802_11_REMOVE_KEY;

typedef struct NDIS_802_11_AI_REQFI {
    UINT16 Capabilities;
    UINT16 ListenInterval;
    NDIS_802_11_MAC_ADDRESS CurrentAPAddress;
} NDIS_802_11_AI_REQFI;

typedef struct NDIS_802_11_AI_RESFI {
    UINT16 Capabilities;
    UINT16 StatusCode;
    UINT16 AssociationId;
} NDIS_802_11_AI_RESFI;

typedef struct NDIS_802_11_ASSOCIATION_INFORMATION {
    UINT32 Length;
    UINT16 AvailableRequestFixedIEs;
    NDIS_802_11_AI_REQFI RequestFixedIEs;
    UINT32 RequestIELength;
    UINT32 OffsetRequestIEs;
    UINT16 AvailableResponseFixedIEs;
    NDIS_802_11_AI_RESFI ResponseFixedIEs;
    UINT32 ResponseIELength;
    UINT32 OffsetResponseIEs;
} NDIS_802_11_ASSOCIATION_INFORMATION;

typedef struct NDIS_802_11_AUTHENTICATION_ENCRYPTION {
    NDIS_802_11_AUTHENTICATION_MODE AuthModeSupported;
    NDIS_802_11_ENCRYPTION_STATUS EncryptStatusSupported;
} NDIS_802_11_AUTHENTICATION_ENCRYPTION;

typedef struct NDIS_802_11_CAPABILITY {
    UINT32 Length;
    UINT32 Version;
    UINT32 NoOfPMKIDs;
    UINT32 NoOfAuthEncryptPairSupported;
    NDIS_802_11_AUTHENTICATION_ENCRYPTION
    AuthenticationEncryptionSupported[1];
} NDIS_802_11_CAPABILITY;

typedef UCHAR NDIS_802_11_PMKID_VALUE[16];

typedef struct BSSID_INFO {
    NDIS_802_11_MAC_ADDRESS BSSID;
    NDIS_802_11_PMKID_VALUE PMKID;
} BSSID_INFO;

typedef struct NDIS_802_11_PMKID {
    UINT32 Length;
    UINT32 BSSIDInfoCount;
    BSSID_INFO BSSIDInfo[1];
} NDIS_802_11_PMKID;

typedef enum NDIS_802_11_STATUS_TYPE {
    Ndis802_11StatusType_Authentication,
    Ndis802_11StatusType_PMKID_CandidateList = 2,
    Ndis802_11StatusTypeMax
} NDIS_802_11_STATUS_TYPE;

typedef struct NDIS_802_11_STATUS_INDICATION {
    NDIS_802_11_STATUS_TYPE StatusType;
} NDIS_802_11_STATUS_INDICATION;

typedef struct PMKID_CANDIDATE {
    NDIS_802_11_MAC_ADDRESS BSSID;
    UINT32 Flags;
} PMKID_CANDIDATE;

#define NDIS_802_11_PMKID_CANDIDATE_PREAUTH_ENABLED 0x01

typedef struct NDIS_802_11_PMKID_CANDIDATE_LIST {
    UINT32 Version;
    UINT32 NumCandidates;
    PMKID_CANDIDATE CandidateList[1];
} NDIS_802_11_PMKID_CANDIDATE_LIST;

typedef struct NDIS_802_11_AUTHENTICATION_REQUEST {
    UINT32 Length;
    NDIS_802_11_MAC_ADDRESS Bssid;
    UINT32 Flags;
} NDIS_802_11_AUTHENTICATION_REQUEST;

#define NDIS_802_11_AUTH_REQUEST_REAUTH         0x01
#define NDIS_802_11_AUTH_REQUEST_KEYUPDATE      0x02
#define NDIS_802_11_AUTH_REQUEST_PAIRWISE_ERROR     0x06
#define NDIS_802_11_AUTH_REQUEST_GROUP_ERROR        0x0E
// #pragma pack()

#endif

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


int driver_set_auth_mode(struct wpa_driver_mtk_data *drv, int mode);
int driver_get_auth_mode(struct wpa_driver_mtk_data *drv);
int driver_set_encr_status(struct wpa_driver_mtk_data *drv, int encr);
int driver_get_encr_status(struct wpa_driver_mtk_data *drv);
//int driver_get_bssid(void *priv, u8 *bssid);
int driver_set_scan(void *priv, param_ssid_t *ssid);
int driver_get_scan_results(void *priv, unsigned char *res_buf, int len);
int driver_get_capability(void *priv, void *buf, int size);
int driver_set_add_key(struct wpa_driver_mtk_data *drv, NDIS_802_11_KEY *nkey, wpa_size_t len);
int driver_set_add_wep(struct wpa_driver_mtk_data *drv, NDIS_802_11_WEP *nkey, wpa_size_t len);
extern int wpa_driver_mtk_remove_pmkid(void *priv, const u8 *bssid,
                                       const u8 *pmkid);
extern int wpa_driver_mtk_add_pmkid(void *priv, const u8 *bssid,
                                    const u8 *pmkid);
extern int wpa_driver_mtk_flush_pmkid(void *priv);
extern int wpa_driver_mtk_get_ssid(void *priv, u8 *ssid);
extern int wpa_driver_mtk_get_bssid(void *priv, u8 *bssid);
extern int wpa_driver_mtk_set_key(void *priv, wpa_alg alg,
                                  const u8 *addr, int key_idx,
                                  int set_tx, const u8 *seq, wpa_size_t seq_len,
                                  const u8 *key, wpa_size_t key_len);
extern int wpa_driver_mtk_associate(void *priv,
                                    struct wpa_driver_associate_params *params);
extern int wpa_driver_mtk_remove_key(struct wpa_driver_mtk_data *drv,
                                     int key_idx, const u8 *addr,
                                     const u8 *bssid, int pairwise);


#endif /* DRIVER_MTK_H */
