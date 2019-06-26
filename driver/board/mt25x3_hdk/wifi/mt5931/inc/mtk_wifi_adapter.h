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

#ifndef _MTK_WIFI_ADAPTER_H
#define _MTK_WIFI_ADAPTER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lwipopts.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "wifi_api.h"
#include "wlan_config.h"


/**
* @brief Station operation mode. In this mode the device works as a Wi-Fi client.
*/
#define WIFI_MODE_STA_ONLY      (1)

/**
* @brief SoftAP operation mode. In Access Point (AP) mode, other client devices can connect to the Wi-Fi AP.
*/
#define WIFI_MODE_AP_ONLY       (2)


/**
* @brief Specifies the AP operation.
*/
#define WIFI_PORT_AP            (1)

/**
* @brief Repeater mode. This mode isn't supported yet.
*/
#define WIFI_MODE_REPEATER          (3)

#define WLAN_NRATE_MCS_INUSE        0x00000080  /* MSC in use,indicates b0-6 holds an mcs */

#define WLAN_NRATE_RATE_MASK        0x0000007F  /* rate/mcs value */

#define PRINT_MAC(a) a[0],a[1],a[2],a[3],a[4],a[5]

#define ADAPTER_DEBUG(log_level, fmt)   \
{                           \
    printc("[ADAPT ] ");    \
    printc fmt;             \
}

#define BIT(n) (0x0001 << (n))

#define WLAN_CHANSPEC_CHAN_MASK     (0x00ff)

#define WLAN_CHSPEC_CHANNEL(chspec) ((uint8_t)((chspec) & WLAN_CHANSPEC_CHAN_MASK))

#define WIFI_MAC_ADDRESS_LENGTH             (6)


typedef void (*wlan_netif_input_fn)(struct netif *inp, struct pbuf *p);

void wlan_get_mac_addr(unsigned char *mac_addr);

char *wlan_get_version(void);

int wlan_get_mode(void);

void wlan_tx_pkt(struct pbuf *p);

int wlan_get_assoc_list(char *buf, unsigned int buf_size);

extern unsigned int kalGetTxAvaliableFrameCount(void);

void wlan_config_func_pin(unsigned char function, unsigned char pin);

void wlan_config_cal_data(unsigned char calibrateType, void *data);

void wlan_rx_register_callback(wlan_netif_input_fn input, struct netif *sta_netif, struct netif *ap_netif);

/**************** MTK_MFG ********************/
#ifndef kal_bool
typedef unsigned char           kal_bool;
#endif
#ifndef kal_uint8
typedef unsigned char           kal_uint8;
#endif
#ifndef kal_int8
typedef signed char             kal_int8;
#endif
#ifndef kal_uint16
typedef unsigned short int      kal_uint16;
#endif
#ifndef kal_int16
typedef signed short int        kal_int16;
#endif
#ifndef kal_uint32
typedef unsigned int            kal_uint32;
#endif
#ifndef kal_int32
typedef signed int              kal_int32;
#endif

typedef enum {
    WNDRV_TEST_RATE_1M = 0,     /* 1M          */
    WNDRV_TEST_RATE_2M,         /* 2M          */
    WNDRV_TEST_RATE_5_5M,       /* 5.5M        */
    WNDRV_TEST_RATE_11M,        /* 11M         */
    WNDRV_TEST_RATE_6M,         /* 6M          */
    WNDRV_TEST_RATE_9M,         /* 9M          */
    WNDRV_TEST_RATE_12M,        /* 12M         */
    WNDRV_TEST_RATE_18M,        /* 18M         */
    WNDRV_TEST_RATE_24M,        /* 24M         */
    WNDRV_TEST_RATE_36M,        /* 36M         */
    WNDRV_TEST_RATE_48M,        /* 48M         */
    WNDRV_TEST_RATE_54M,        /* 54M         */
    WNDRV_TEST_MOD_MCS0,        /* 802.11n MSC 0 */
    WNDRV_TEST_MOD_MCS1,        /* 802.11n MSC 1 */
    WNDRV_TEST_MOD_MCS2,        /* 802.11n MSC 2 */
    WNDRV_TEST_MOD_MCS3,        /* 802.11n MSC 3 */
    WNDRV_TEST_MOD_MCS4,        /* 802.11n MSC 4 */
    WNDRV_TEST_MOD_MCS5,        /* 802.11n MSC 5 */
    WNDRV_TEST_MOD_MCS6,        /* 802.11n MSC 6 */
    WNDRV_TEST_MOD_MCS7,        /* 802.11n MSC 7 */
    WNDRV_TEST_MOD_MCS32,       /* 802.11n MSC 32 */
    WNDRV_TEST_RATE_COUNT       /* Total count */
} wndrv_test_rate_enum;

typedef enum {
    WNDRV_TX_ALL_ZEROS = 0,
    WNDRV_TX_ALL_ONES,
    WNDRV_TX_ALTERNATE_BITS,
    WNDRV_TX_PSEUDO_RANDOM
} wndrv_test_pkt_tx_pattern_enum;

typedef enum {
    WNDRV_TEST_BW_20MHZ = 0,
    WNDRV_TEST_BW_40MHZ,
    WNDRV_TEST_BW_U20MHZ,
    WNDRV_TEST_BW_L20MHZ,
    WNDRV_TEST_BW_COUNT
} wndrv_test_bandwidth_enum;
typedef enum {
    WNDRV_TEST_TX_GI_400NS = 0,
    WNDRV_TEST_TX_GI_800NS,
    WNDRV_TEST_TX_GI_COUNT
} wndrv_test_guard_intval_enum;
typedef enum {
    WNDRV_TEST_TX_GREEN_FIELD = 0,
    WNDRV_TEST_TX_MIX_MODE,
    WNDRV_TEST_TX_MODE_COUNT
} wndrv_test_mode_sel_enum;


typedef enum _WscSecurityMode {
    WPA2PSKAES,
    WPA2PSKTKIP,
    WPAPSKAES,
    WPAPSKTKIP
} WSC_SECURITY_MODE;



typedef enum _WPSState {
    WPS_STATE_OFF,
    WPS_STATE_INIT,
    WPS_STATE_START,
    WPS_STATE_CONN_FAIL,
    WPS_STATE_FAIL,
    WPS_STATE_LINK_UP,
    WPS_STATE_WAIT_START,
    WPS_STATE_WAIT_M2,
    WPS_STATE_RX_M2D,
    WPS_STATE_WAIT_M4,
    WPS_STATE_WAIT_M6,
    WPS_STATE_WAIT_M8,
    WPS_STATE_WAIT_EAPFAIL,
    WPS_STATE_WAIT_DISCONN,
    WPS_STATE_CONFIGURED
}   WPS_STATE;

typedef void *wl_drv_hdl;

extern wl_drv_hdl __g_virtual_hdl;

typedef struct _WLAN_802_11_SSID {
    /* SSID Length */
    kal_uint32 SsidLength;

    /* SSID information field */
    kal_uint8 Ssid[32];
} WLAN_802_11_SSID;

typedef struct _WPS_CONFIG_INFO {
    unsigned long   WscMode;        // predefined WSC mode, 1: PIN, 2: PBC
    unsigned char   PIN[8];         // 8 bytes PIN nunmber
    unsigned char   Bssid[6];
    unsigned char   JoinAP;         //1: to join AP after WPS success, 0: Not to join AP after WPS success
    unsigned int    ScanBufSize;    //WPS scan buffer size, max 32KB
    unsigned int    AssocAPTime;    //Time to associate target AP in mili-seconds, default is 20000 (20s)
#if 1
    WLAN_802_11_SSID    Ssid;
    WSC_SECURITY_MODE   WscSecurityMode;
    unsigned char       WpaPsk[64];
    int                 WpaPskLen;
    int                 channel;
    int                 ssidBcst;
#endif
} WPS_CONFIG_INFO, *PWPS_CONFIG_INFO;

#ifndef WNDRV_PACKET_SIZE_COUNT
#define WNDRV_PACKET_SIZE_COUNT   (6)
#endif
#ifndef WNDRV_PACKET_TYPE_COUNT
#define WNDRV_PACKET_TYPE_COUNT   (3)
#endif

typedef struct {
    kal_uint32                       ch_freq;/* Frq, units are kHz */
    wndrv_test_rate_enum             tx_rate;
    kal_uint8                        txAnt;  /* 0 for Antenna 0 and 1 for Antenna 1 */
    kal_uint16                       tx_gain_dac;
    wndrv_test_bandwidth_enum        txBw;              /* tx bandwidth */
    wndrv_test_guard_intval_enum     txGI;              /* tx guard interval */
    wndrv_test_mode_sel_enum         txMode;            /* tx preamble mode */
} wndrv_test_tx_struct;

typedef struct {
    kal_uint32                       ch_freq;           /* Frq, units are kHz */
    wndrv_test_rate_enum             tx_rate;
    kal_uint16                       tx_gain_dac;
    kal_uint32                       pktCount;
    kal_uint32                       pktInterval;       /* interval between each Tx Packet */
    kal_uint32                       pktLength;         /* 24~1500 */
    wndrv_test_pkt_tx_pattern_enum   pattern;           /* content of the Tx Packet */
    kal_uint8                        txAnt;             /* 0 for Antenna 0 and 1 for Antenna 1 */
    kal_bool                         is_short_preamble; /* 0 for long preamble and 1 for short preamble */
    kal_uint8                        mac_header[ 24 ];  /* Frame Ctrl, Duration = 2bytes + 2bytes */
    /* Address 1 = 6 bytes */
    /* Address 2 = 6 bytes */
    /* Address 3 = 6 bytes */
    /* Sequence Ctrl = 2 bytes */
    wndrv_test_bandwidth_enum        txBw;              /* tx bandwidth */
    wndrv_test_guard_intval_enum     txGI;              /* tx guard interval */
    wndrv_test_mode_sel_enum         txMode;            /* tx preamble mode */
} wndrv_test_pkt_tx_struct;

typedef struct {
    kal_uint32   int_rx_ok_num;          /* number of packets that Rx ok from interrupt (Total Rx count) */
    kal_uint32   int_crc_err_num;        /* number of packets that CRC error from interrupt */
    kal_uint32   pau_rx_pkt_count;       /* number of packets that Rx ok from PAU (Total Rx count) */
    kal_uint32   pau_crc_err_count;      /* number of packets that CRC error from PAU */
    kal_uint32   pau_cca_count;          /* CCA rising edge count */
    kal_uint32   pau_rx_fifo_full_count; /* number of lost packets due to FiFo full */
    kal_uint32   int_long_preamble_num;
    kal_uint32   int_short_preamble_num;
    kal_uint32   int_rate_ok_num[ WNDRV_TEST_RATE_54M + 1 ];
    kal_uint32   int_rate_crc_err_num[ WNDRV_TEST_RATE_54M + 1 ];
    kal_int32    int_rssi_max;
    kal_int32    int_rssi_min;
    kal_int32    int_rssi_mean;
    kal_int32    int_rssi_variance;
    kal_uint32   packetSize[WNDRV_PACKET_SIZE_COUNT];  /* 0~63, 64~127, 128~255, 256~511, 512~1023, 1024~2047 */
    kal_uint32   packetType[WNDRV_PACKET_TYPE_COUNT];   /* Data, Management, Control */
    kal_uint32   int_rate_ok_num_802_11n[WNDRV_TEST_MOD_MCS7 - WNDRV_TEST_RATE_54M ];
    kal_uint32   int_rate_crc_err_num_802_11n[WNDRV_TEST_MOD_MCS7 - WNDRV_TEST_RATE_54M ];
} wndrv_test_rx_status_struct;

typedef struct _WLAN_WPS_CONFIG {
    uint32_t  WscMode;        // predefined WSC mode, 1: PIN, 2: PBC
    uint8_t   PIN[8];         // 8 bytes PIN nunmber
    uint8_t   Bssid[6];
    uint8_t   JoinAP;         //1: to join AP after WPS success, 0: Not to join AP after WPS success
    uint32_t  ScanBufSize;    //WPS scan buffer size, max 32KB
    uint32_t  AssocAPTime;    //Time to associate target AP in mili-seconds, default is 20000 (20s)
} WLAN_WPS_CONFIG;


/* linked list to function structure */
typedef struct wifi_event_handler_struct {
    kal_int32 events;
    wifi_event_handler_t func;
    struct wifi_event_handler_struct *next;
} wifi_event_handler_struct;


typedef int WLM_CHANNEL;
typedef int wlan_event_msg_t;
typedef void (*wlan_drv_event_callback)(wlan_event_msg_t event);
typedef void (*scan_item_callback)(WLAN_BSS_INFO *pbss);
typedef void (*wlan_wps_event_callback)(WPS_STATE event);

int wifi_set_notification_callback(kal_uint8 enabled, kal_int32 events, wifi_event_handler_t callback);
int32_t wifi_event_proxy_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

extern void RFTool_WiFi_EnterTestMode(void);
extern void RFTool_WiFi_Stop(void);
extern void RFTool_WiFi_ContTx(kal_uint8 country_code[2], wndrv_test_tx_struct cont_tx);
extern void RFTool_WiFi_CarrierSupp(kal_uint8 country_code[2], wndrv_test_tx_struct carrier_supp);
extern void RFTool_WiFi_LocalFreq(kal_uint8 country_code[2], wndrv_test_tx_struct local_freq);
extern void RFTool_WiFi_PktTx(kal_uint8 country_code[2], wndrv_test_pkt_tx_struct pkt_tx);
extern void RFTool_WiFi_PktRx(kal_uint32 ch_freq, void (* callback)(wndrv_test_rx_status_struct *result));
extern void RFTool_WiFi_BW(kal_uint8 bandwidth);

extern void wndrv_create(void *argv);
extern void hostapd_create(void *argv);
extern void supc_create(void *argv);

kal_uint32 wndrv_set_sta_power_saving(PARAM_POWER_MODE pm);
kal_uint32 wndrv_get_sta_power_saving(PARAM_POWER_MODE *pm);


kal_int32 wlan_sta_set_power_saving(PARAM_POWER_MODE pm);
kal_int32 wlan_sta_get_power_saving(PARAM_POWER_MODE *pm);

void wlan_sta_get_scan_result(char *scan_buf);
void wifi_scan_item_register_callback(scan_item_callback cb);
char *look_for_encrytion_type(WLM_ENCRYPTION encryption);
char *look_for_auth_mode(WLM_AUTH_MODE authMode);
char *look_for_ps_mode(PARAM_POWER_MODE psmode);

int32_t wifi_init_done_announce(wifi_event_t event, uint8_t *payload, uint32_t length);
kal_uint8 BtoH(char ch);
void AtoH(char *src, char *dest, int destlen);
wl_drv_hdl wlan_start(wlan_drv_event_callback cb, unsigned int tx_buf_cnt, unsigned int rx_buf_cnt, unsigned int scan_buf_size);
void wlan_stop(wl_drv_hdl hdl);
int wlan_scan_network(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);
int wlan_join_network(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption, const char *key, kal_uint8 wep_key_index, struct os_time *timeout);
int wlan_real_join_network(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption, const char *key, kal_uint8 wep_key_index, WLAN_WPS_CONFIG *wps_config, struct os_time *timeout);
int wlan_get_connection_info(wl_connection_info_t *wlan_connection_info);
int wlan_disconnect_network(void);
void wifi_cb_init_done(void);
void wifi_conf_get_mac_from_str(char *mac_dst, const char *mac_src);
int32_t wifi_get_mac_addr_from_efuse(uint8_t port, uint8_t *mac_addr);
int32_t wifi_get_mac_addr_from_nvdm(uint8_t port, uint8_t *mac_addr);
int32_t wifi_get_mac_addr_from_config(uint8_t port, uint8_t *mac_addr);
int32_t wifi_is_mac_address_valid(uint8_t *mac_addr);

#ifdef __cplusplus
}
#endif

#endif  /* _MTK_WIFI_ADAPTER_H */
