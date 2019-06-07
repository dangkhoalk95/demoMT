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

//#include "kal_release.h"
#include "kal_trace.h"
//#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h"         /* enum for message IDs */
//#include "stack_ltlcom.h"         /* Task message communiction */
//#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"         /* Event scheduler */
//#include "stack_timer.h"        /* Stack timer */
//#include "syscomp_config.h"
//#include "task_config.h"
//#include "task_main_func.h"
#include "stack_ltlcom.h"
#include "kal_public_api.h"
#ifdef __HOTSPOT_SUPPORT__
#include "hostap_hdlr.h"
#endif
#include "hostap_abm_msgs.h"
#include "supc_abm_msgs.h"
//#include "ilm_api.h"
#if (!defined(__CC_ARM)) && (!defined(__ICCARM__))
#include <unistd.h>
#endif
#include "stdlib.h"
//#include "ucos_ii.h"
#include "freertos_ii.h"
#include "wndrv_tcpip_msg.h"
#include "lwipopts.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip_network.h"
#include "mtk_hw_adapter.h"
#include "mtk_wifi_adapter.h"
#include "wifi_common_config.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif



#ifndef _wlioctl_h_
struct ether_addr {
    unsigned char mac[6];
};
#endif

#ifndef _wlioctl_h_
/* For ioctls that take a list of MAC addresses */
struct maclist {
    unsigned int count;         /* number of MAC addresses */
    struct ether_addr ea[1];    /* variable length array of MAC addresses */
};
#endif

typedef struct _WLAN_CONNECTION_INFO {
    kal_uint8   Ssid_len;
    kal_uint8   Ssid[32];
    kal_int32   Rssi;
    kal_int32   Phy_rate;
    kal_int32   channel;
} WLAN_CONNECTION_INFO;

#define WNDRV_VER_STR       "2.0"
#define HOSTAPD_VER_STR     "2.0"
#define SUPC_VER_STR        "1.0"

#define WPS_WALK_TIME   120
#define WLAN_E_SET_SSID         0   /* indicates status of set SSID */
#define WLAN_E_ASSOC_IND        8   /* 802.11 ASSOC indication */
#define WLAN_E_DISASSOC_IND     12  /* 802.11 DISASSOC indication */

typedef struct _WLAN_SSID {
    /* SSID Length */
    kal_uint32   SsidLength;

    /* SSID information field */
    kal_uint8   Ssid[32];
} WLAN_SSID;
typedef struct _WLAN_WEP_KEY {
    kal_uint32 KeyLength;       //length in bytes
    kal_uint8 KeyMaterial[16];  //hexadecimal

} WLAN_WEP_KEY;

typedef struct _WLAN_CONFIG_INFO {
    int  InfrastructureMode;
    WLAN_SSID       Ssid;
    kal_uint8           InfraBssid[6];

    // Security settings
    int  Auth_Mode;
    int Enc_Mode;
    WLAN_WEP_KEY    Wep_Key[4];
    kal_uint8       WepKeyToUse;        //Specifies the WEP key index(0:key 1, 1:key 2, 2:key 3, 3:key 4).
    kal_uint8       PresharedKey[64];   //Preshared key for WPA-PSK or WPA2-PSK
    kal_uint32      Psk_length;

} WLAN_CONFIG_INFO;


#define WPA_CIPHER_NONE BIT(0)
#define WPA_CIPHER_WEP40 BIT(1)
#define WPA_CIPHER_WEP104 BIT(2)
#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CIPHER_CCMP BIT(4)
#if 1 //#ifdef __WAPI_SUPPORT__
#define WAPI_CIPHER_SMS4    BIT(5)
#endif

#define WPA_KEY_MGMT_IEEE8021X BIT(0)
#define WPA_KEY_MGMT_PSK BIT(1)
#define WPA_KEY_MGMT_NONE BIT(2)
#define WPA_KEY_MGMT_IEEE8021X_NO_WPA BIT(3)
#define WPA_KEY_MGMT_WPA_NONE BIT(4)
#if 1 //#ifdef __WAPI_SUPPORT__
#define WAPI_KEY_MGMT_PSK BIT(5)
#define WAPI_KEY_MGMT_WAI BIT(6)
#endif

#define WPA_PROTO_WPA BIT(0)
#define WPA_PROTO_RSN BIT(1)
#if 1  //#ifdef __WAPI_SUPPORT__
#define WPA_PROTO_WAPI BIT(2)
#endif
#if 1 //#ifdef __WPS_SUPPORT__
#define WPA_PROTO_WPS_OPEN  BIT(3)
#define WPA_PROTO_WPS_WPA   BIT(4)
#define WPA_PROTO_WPS_WPA2  BIT(5)
#endif

#define WPA_AUTH_ALG_OPEN BIT(0)
#define WPA_AUTH_ALG_SHARED BIT(1)
#define WPA_AUTH_ALG_LEAP BIT(2)
enum {
    ADAPTER_MOD_NONE = 0,
    ADAPTER_MOD_STA = 1,
    ADAPTER_MOD_AP = 2
};

enum {
    ADAPTER_MOD_STA_CONNECTED = 0,
    ADAPTER_MOD_STA_DISCONNECTED = 1
};


kal_uint8 gladapter_mode = ADAPTER_MOD_NONE;
WPS_STATE gladapter_wps_state;
kal_uint8 gladapter_mode_sta_status = ADAPTER_MOD_STA_DISCONNECTED;


extern void wlan_sys_rx_pkt(struct pbuf *p);
#ifdef __HOTSPOT_SUPPORT__
int wlan_create_sw_ap(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption,
                      const char *key, WLM_CHANNEL channel, int ssidBcst);
#endif

extern kal_sem hostapd_adapter_sem;           //semaphore used between hostapd and adapter

extern kal_bool hostap_get_wndrv_test_mode(void);
extern void hostap_flush_local_buffer(void *buf);
extern void *hostap_allocate_local_buffer(kal_uint32 size);
extern void pbkdf2_sha1(const char *passphrase, const char *ssid, kal_uint16 ssid_len,
                        int iterations, kal_uint8 *buf, kal_uint16 buflen);
extern void atci_cmd_wifi_ready_announce(void);
extern kal_int32 osl_ext_read_mac(kal_uint8 *pMac);

#ifdef __HOTSPOT_SUPPORT__
int wlan_start_ap(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption,
                  const char *key, WLM_CHANNEL channel, int ssidBcst);
KAL_BOOL wlan_stop_ap(void);
#endif
KAL_BOOL wlan_start_sta(void);
KAL_BOOL wlan_stop_sta(void);


kal_sem hostapd_adapter_sem;            //semaphore between hostapd and adapter layer
unsigned int HostapdDebugLevel = (1 << TRACE_ERROR) | (1 << TRACE_WARNING) | (1 << TRACE_STATE) |
                                 (1 << TRACE_INFO) | (1 << TRACE_FUNC) | (1 << TRACE_GROUP_1);             //hostapd debug level, default = 0(DEBUG)
#define WNDRV_CREATED   0x01
#define HOSTAPD_CREATED 0x02
#define SUPC_CREATED    0x04
#define MSGQ_CREATED    0x08
#define LWIP_CREATED    0x10
static kal_uint8 wlan_status = 0;

wlan_drv_event_callback cb_from_wlan_c = NULL;
wlan_wps_event_callback wps_cb_from_wlan_c = NULL;



#define MESSAGE_BOX_QUEUE_SIZE 32
wl_drv_hdl __g_virtual_hdl;                         //virtual hdl structure, used in upper layer
extern kal_uint8 hostapd_mt5931_mac_address[6];     //mt5931 mac address stored in hostapd, used for upper layer
kal_uint8 mt5931_mac_address_sta[6];            //sta mac address
kal_uint8 mt5931_mac_address_ap[6];             //sw ap mac address
kal_uint8 main_dev_mac_addr[6];
kal_uint8 *pmain_dev_mac_addr;                   //pointer to main device mac address
#define SUPC_MAX_EVENT_HD_NUM 16                        //max number of event handler
wifi_event_handler_struct *callback_list_head = NULL;   //event handler head

const wifi_event_handler_t wifi_rx_event_handler[] = {  //event handler proxy handler
    wifi_event_proxy_handler,  /* Disconnect Event */
    wifi_event_proxy_handler,  /* Port Secure Event */
    wifi_event_proxy_handler,  /* WIFI Init Done Event */
    wifi_event_proxy_handler,  /* WIFI Scan Complete event */
};

extern void hexdump(char *header, unsigned char *src, unsigned int len);
char *ssid_txt(kal_uint8 *input_ssid, kal_int16 ssid_len)
{
    static char ssid[32];
    memset(ssid, 0, sizeof(ssid));
    memcpy(ssid, input_ssid, ssid_len);
    return ssid;
}
char *key_txt(kal_uint8 *input_key, kal_int16 key_len)
{
    static char key[64];
    if (key_len >= 64) {
        key_len = 63;
        ADAPTER_DEBUG(0, ("input key len > 64\n"));
    }
    memset(key, 0, sizeof(key));
    memcpy(key, input_key, key_len);
    return key;
}

#ifdef __HOTSPOT_SUPPORT__
void adapter_send_hostapd_req(void *msg, msg_type type)
{
    ilm_struct *ilm_ptr;
    /* Return message */
    ilm_ptr                 = allocate_ilm(MOD_ABM);
    ilm_ptr->msg_id         = type;
    ilm_ptr->local_para_ptr = (local_para_struct *) msg;
    ilm_ptr->peer_buff_ptr  = NULL;
    /* Send message */
    SEND_ILM(MOD_ABM, MOD_HOSTAP, HOSTAP_ABM_SAP, ilm_ptr)
}
#endif

void adapter_send_supc_req(void *msg, msg_type type)
{
    ilm_struct *ilm_ptr;
    /* Return message */
    ilm_ptr                 = allocate_ilm(MOD_ABM);
    ilm_ptr->msg_id         = type;
    ilm_ptr->local_para_ptr = (local_para_struct *) msg;
    ilm_ptr->peer_buff_ptr  = NULL;
    /* Send message */
    SEND_ILM(MOD_ABM, MOD_SUPC, SUPC_ABM_SAP, ilm_ptr)
}

ilm_struct *adapter_wait_for_msg(msg_type wait_msg, kal_uint32 timeout)
{
    ilm_struct *pcurrent_ilm = NULL;
    kal_uint32 time_start_wait = 0, time_now = 0;
    while (1) {
        kal_get_time(&time_start_wait);
        //printf("[D]BR:%d",time_start_wait);
        receive_msg_ext_q(mt5931_msgqid[MOD_ABM], &pcurrent_ilm, timeout);
        kal_get_time(&time_now);
        //printf("[D]BR:%d",time_now);
        if (pcurrent_ilm) {
            if (pcurrent_ilm->msg_id == wait_msg) {
                ADAPTER_DEBUG(0, ("expected msg_id(%d), waited for %d ms.\n", pcurrent_ilm->msg_id, time_now - time_start_wait));
                return pcurrent_ilm;
            } else {
                if (time_now > time_start_wait) {
                    timeout = timeout - (time_now - time_start_wait);
                } else {
                    timeout = timeout - (0xFFFFFFFF - time_start_wait + time_now);
                }
                ADAPTER_DEBUG(0, ("not the expected msg_id(%d), so skip it, left wait time %d ms \n", pcurrent_ilm->msg_id, timeout));
                free_ilm(pcurrent_ilm);
            }
        } else {
            ADAPTER_DEBUG(0, ("wait for msg(%d) timeout %d\n", wait_msg, timeout));
            return NULL;
        }
    }

}


void wndrv_create_task(void)
{
    KAL_TASK_CFG task_cfg;
    unsigned char ret = 0;

    task_cfg.pTaskName = WNDRV_TASK_NAME;
    task_cfg.ubPriority = WNDRV_TASK_PRIO;
    task_cfg.usStackDepth = WNDRV_TASK_STACKSIZE;
    ret = kal_create_task(wndrv_create, &task_cfg, (void *) NULL);
    ASSERT(ret == WNDRV_TASK_PRIO);
    KAL_PUBLIC_TRACE(TRACE_STATE, "wndrv task: %s, %d, %d\n", task_cfg.pTaskName, task_cfg.ubPriority, task_cfg.usStackDepth);
}

#ifdef __HOTSPOT_SUPPORT__
void hostap_create_task(void)
{
    KAL_TASK_CFG task_cfg;
    unsigned char ret = 0;

    task_cfg.pTaskName = HOSTAPD_TASK_NAME;
    task_cfg.ubPriority = HOSTAPD_TASK_PRIO;
    task_cfg.usStackDepth = HOSTAPD_TASK_STACKSIZE;
    ret = kal_create_task(hostapd_create, &task_cfg, (void *) NULL);
    ASSERT(ret == HOSTAPD_TASK_PRIO);
    KAL_PUBLIC_TRACE(TRACE_STATE, "hostap task: %s, %d, %d\n", task_cfg.pTaskName, task_cfg.ubPriority, task_cfg.usStackDepth);
}
#endif

void supc_create_task(void)
{
    KAL_TASK_CFG task_cfg;
    unsigned char ret = 0;

    task_cfg.pTaskName = WPA_SUPPLICANT_TASK_NAME;
    task_cfg.ubPriority = WPA_SUPPLICANT_TASK_PRIO;
    task_cfg.usStackDepth = WPA_SUPPLICANT_TASK_STACKSIZE;
    ret = kal_create_task(supc_create, &task_cfg, (void *) NULL);
    ASSERT(ret == WPA_SUPPLICANT_TASK_PRIO);
    KAL_PUBLIC_TRACE(TRACE_STATE, "supc task: %s, %d, %d\n", task_cfg.pTaskName, task_cfg.ubPriority, task_cfg.usStackDepth);
}

void wlan_start_create(void *argv)
{
    //LOG_I(hal, "wifi loopback start!!\n\r");
    //wndrv_test_ehpi_loopback_test("1000 1 1020");
    //LOG_I(hal, "wifi loopback end!!\n\r");
    //goto end;

    KAL_BOOL result = FALSE;
    ADAPTER_DEBUG(0, ("wlan_start_sta begin\n"));
    result = wlan_start_sta();
    ADAPTER_DEBUG(0, ("wlan_start_sta end\n"));
    if (result == KAL_FALSE) {
        ADAPTER_DEBUG(0, ("%s wlan start sta fail\n", __func__));
        MEM_FREE(__g_virtual_hdl);
        __g_virtual_hdl = NULL;
        goto error;
    } else {

        //set as station mode
        ADAPTER_DEBUG(0, ("%s wlan start sta successfully\n", __func__));
        gladapter_mode = ADAPTER_MOD_STA;
        ADAPTER_DEBUG(0, ("origin main device address=%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pmain_dev_mac_addr)));
        pmain_dev_mac_addr = main_dev_mac_addr;
        memcpy(pmain_dev_mac_addr, mt5931_mac_address_sta, sizeof(mt5931_mac_address_sta));

        wifi_cb_init_done();

        //wlan_real_join_network("77", 0, 0, NULL, NULL);
        //wlan_real_join_network("hello_TP", WLM_WPA_AUTH_DISABLED, WLM_ENCRYPT_WEP, "1234512345", NULL);
        goto end;
    }

error:
    ADAPTER_DEBUG(0, ("wlan_start_create fail\n"));
    kal_delete_task(NULL);
end:
    kal_delete_task(NULL);

}


void wlan_start_create_task(void)
{
    KAL_TASK_CFG task_cfg;
    unsigned char ret = 0;

    task_cfg.pTaskName = WLAN_START_TASK_NAME;
    task_cfg.ubPriority = WLAN_START_TASK_PRIO;
    task_cfg.usStackDepth = WLAN_START_TASK_STACKSIZE;
    kal_uint8 *p = (kal_uint8 *)MEM_MALLOC(sizeof(kal_uint8));
    if (!p) {
        ADAPTER_DEBUG(0, ("wlan_start alloc mem fail\n"));
        return;
    }
    __g_virtual_hdl = (wl_drv_hdl)p;
    ret = kal_create_task(wlan_start_create, &task_cfg, (void *) NULL);
    ASSERT(ret == WLAN_START_TASK_PRIO);
    KAL_PUBLIC_TRACE(TRACE_STATE, "wlan start task: %s, %d, %d\n", task_cfg.pTaskName, task_cfg.ubPriority, task_cfg.usStackDepth);
}



wl_drv_hdl wlan_start(wlan_drv_event_callback cb, unsigned int tx_buf_cnt, unsigned int rx_buf_cnt, unsigned int scan_buf_size)
{

    ADAPTER_DEBUG(0, ("wlan_start\n"));
    //pmain_dev_mac_addr = (kal_uint8*) tx_buf_cnt;

    if (!(wlan_status & MSGQ_CREATED))
        /*step1-init all mail boxes and semaphores that will be used later*/
    {
        ADAPTER_DEBUG(0, ("wlan first start, init msg queue\n"));
        if (mt5931_create_msg_ext_q() != KAL_SUCCESS) {
            goto error;
        }
        wlan_status |= MSGQ_CREATED;
    }
    /*step2-create 5931 related tasks, i.e. hostapd/wndrv/supc*/
    if (!(wlan_status & WNDRV_CREATED)) {
        ADAPTER_DEBUG(0, ("wlan first start, create wndrv task\n"));
        wndrv_create_task();
        wlan_status |= WNDRV_CREATED;
    }
#ifdef __HOTSPOT_SUPPORT__
    if (!(wlan_status & HOSTAPD_CREATED)) {
        ADAPTER_DEBUG(0, ("wlan first start, create hostapd task\n"));
        hostap_create_task();
        wlan_status |= HOSTAPD_CREATED;
    }
#endif
    if (!(wlan_status & SUPC_CREATED)) {
        ADAPTER_DEBUG(0, ("wlan first start, create supc task\n"));
        supc_create_task();
        wlan_status |= SUPC_CREATED;
    }
    //wifi_cb_init_done
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_DONE, wifi_init_done_announce);

    wlan_start_create_task();

    cb_from_wlan_c = cb;
#if 0
    ADAPTER_DEBUG(0, ("wlan_start_sta begin\n"));
    result = wlan_start_sta();
    ADAPTER_DEBUG(0, ("wlan_start_sta end\n"));
    if (result == KAL_FALSE) {
        ADAPTER_DEBUG(0, ("%s wlan start sta fail\n", __func__));
        goto error;
    } else {
        p = (kal_uint8 *)MEM_MALLOC(sizeof(kal_uint8));
        if (!p) {
            ADAPTER_DEBUG(0, ("wlan_start alloc mem fail\n"));
            goto error;
        }
        //set as station mode
        ADAPTER_DEBUG(0, ("%s wlan start sta successfully\n", __func__));
        gladapter_mode = ADAPTER_MOD_STA;
        ADAPTER_DEBUG(0, ("origin main device address=%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pmain_dev_mac_addr)));
        memcpy(pmain_dev_mac_addr, mt5931_mac_address_sta, sizeof(mt5931_mac_address_sta));
        __g_virtual_hdl = (wl_drv_hdl)p;
        return __g_virtual_hdl;
    }
    //default start station.
    //avoid multiple call wlan_start_sta
    if (__g_virtual_hdl) {
        return __g_virtual_hdl;
    }
#endif
    return __g_virtual_hdl;
error:
    ADAPTER_DEBUG(0, ("wlan_start fail\n"));
    return NULL;
}

void wlan_stop(wl_drv_hdl hdl)
{
    kal_uint8 *p = NULL;
    KAL_BOOL ret = KAL_FALSE;
#ifdef __HOTSPOT_SUPPORT__
    if (gladapter_mode == ADAPTER_MOD_AP) {
        //stop sw ap
        ret = wlan_stop_ap();
    }
#endif
    if (gladapter_mode == ADAPTER_MOD_STA) {
        //stop station
        ret = wlan_stop_sta();
        wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_INIT_DONE, wifi_init_done_announce);
    } else {
        ADAPTER_DEBUG(0, ("not in ap or sta mode\n"));
    }

    if (ret == KAL_TRUE) {
        gladapter_mode = ADAPTER_MOD_NONE;
        ADAPTER_DEBUG(0, ("wlan have stoped\n"));
    } else {
        ADAPTER_DEBUG(0, ("wlan stop fail\n"));
    }

    p = (kal_uint8 *)hdl;
    MEM_FREE(p);
    __g_virtual_hdl = NULL;

}


void wlan_get_mac_addr(unsigned char *mac_addr)
{

    if (gladapter_mode == ADAPTER_MOD_AP) {
        memcpy(mac_addr, mt5931_mac_address_ap, sizeof(mt5931_mac_address_ap));
    } else if (gladapter_mode == ADAPTER_MOD_STA) {
        memcpy(mac_addr, mt5931_mac_address_sta, sizeof(mt5931_mac_address_sta));
    } else {
        ADAPTER_DEBUG(0, ("wlan_get_mac_addr Error!!\n"));
    }
}

#ifdef __HOTSPOT_SUPPORT__
int wlan_create_sw_ap(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption,
                      const char *key, WLM_CHANNEL channel, int ssidBcst)
{
    KAL_BOOL ret = KAL_FALSE;
    if (gladapter_mode == ADAPTER_MOD_STA) {
        //first stop station
        ret = wlan_stop_sta();
        if (ret == KAL_TRUE) {
            gladapter_mode = ADAPTER_MOD_NONE;
        } else {
            ADAPTER_DEBUG(0, ("%s wlan stop sta fail\n", __func__));
        }
    } else if (gladapter_mode == ADAPTER_MOD_AP) {
        ADAPTER_DEBUG(0, ("%s we are now in SW AP mode, so do not call wlan_start_ap again\n", __func__));
        return 1;
    }

    kal_sleep_task(500); //delay 500 miniseconds to start ap

    ret = wlan_start_ap(ssid, authMode, encryption, key, channel, ssidBcst);
    if (ret == KAL_TRUE) {
        ADAPTER_DEBUG(0, ("%s wlan start ap successfully\n", __func__));
        gladapter_mode = ADAPTER_MOD_AP;
        ADAPTER_DEBUG(0, ("origin main device address=%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pmain_dev_mac_addr)));
        memcpy(pmain_dev_mac_addr, mt5931_mac_address_ap, sizeof(mt5931_mac_address_ap));
        return 1;
    } else {
        ADAPTER_DEBUG(0, ("%s wlan start ap fail\n", __func__));
        return -1;
    }
}
#endif

char *wlan_get_version(void)
{
    return WNDRV_VER_STR"-"HOSTAPD_VER_STR"-"SUPC_VER_STR;
}


int wlan_get_mode(void)
{
    if (gladapter_mode == ADAPTER_MOD_STA) {
        return 0;
    } else if (gladapter_mode == ADAPTER_MOD_AP) {
        return 1;    // 1 AP   0 STA
    } else {
        return -1;
    }
}


#define ETHER_ADDR_LEN  6

#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

void wlan_tx_pkt(struct pbuf *p)
{
    ilm_struct *ilm_ptr;
    peer_buff_struct *peer_buff_ptr = NULL;
    tcpip_wndrv_data_req_struct *data_req = NULL;
    unsigned char *pload = p->payload;
    //kal_uint16 len = 0;
    struct pbuf *q;
    kal_uint16 total_len = 0;
    kal_uint16 offset = 0;

    ilm_ptr = allocate_ilm(MOD_TCPIP);
    if (ilm_ptr == NULL) {
        return;
    }
    for (q = p; q != NULL; q = q->next) {
        total_len = total_len + (q->len);
    }
#if defined (WLAN_ZERO_COPY)
    peer_buff_ptr = (peer_buff_struct *)pbuf_to_flc2(p);
    if (peer_buff_ptr == NULL) {
        return;
    }

#else
    peer_buff_ptr = construct_peer_buff(total_len, WNDRV_FREE_HEADER_SPACE, 0, TD_UL);
    if (peer_buff_ptr == NULL) {
        free_ilm(ilm_ptr);
        ASSERT(peer_buff_ptr);
        return;
    }
    kal_uint8 *data_copy_p = (kal_uint8 *)get_pdu_ptr(peer_buff_ptr, &total_len);
    //kal_mem_cpy(data_copy_p, p->payload, p->len);
    for (q = p; q != NULL; q = q->next) {
        kal_mem_cpy(data_copy_p + offset, q->payload, q->len);
        offset += q->len;
    }
#endif
    ilm_ptr->peer_buff_ptr = peer_buff_ptr;

    data_req = (tcpip_wndrv_data_req_struct *) construct_local_para(sizeof(tcpip_wndrv_data_req_struct), TD_UL);

    memcpy(data_req->dest_mac_addr, pload, ETHER_ADDR_LEN);
    memcpy(data_req->src_mac_addr, pload + ETHER_ADDR_LEN, ETHER_ADDR_LEN);
    //TODO review here
    data_req->proto_type = (*(pload + 2 * ETHER_ADDR_LEN) << 8) + *(pload + 2 * ETHER_ADDR_LEN + 1);

    data_req->tid = 0x00;

    if (data_req->proto_type == ETHERTYPE_IP) {
#ifdef __HOTSPOT_SUPPORT__
        data_req->pool_id = FLC2_POOL_ID_SPDU_DL;
#else
        data_req->pool_id = FLC2_POOL_ID_TPDU;//FLC2_POOL_ID_SPDU_UL_0
#endif
    } else if (data_req->proto_type == ETHERTYPE_ARP) {
        data_req->pool_id = FLC2_POOL_ID_TPDU;
    } else {
        KAL_PUBLIC_TRACE(TRACE_INFO, "Waring:data_req->proto_type 0x%X\n", data_req->proto_type);
    }

    ilm_ptr->local_para_ptr = (local_para_struct *) data_req;
    ilm_ptr->msg_id = MSG_ID_TCPIP_WNDRV_DATA_REQ;
    SEND_ILM(MOD_TCPIP, MOD_WNDRV, WNDRV_TCPIP_SAP, ilm_ptr);

}

wlan_netif_input_fn wlan_input = NULL;
struct netif *wlan_sta_netif = NULL;
struct netif *wlan_ap_netif = NULL;

void wlan_rx_register_callback(wlan_netif_input_fn input, struct netif *sta_netif, struct netif *ap_netif)
{
    wlan_input = input;
    wlan_sta_netif = sta_netif;
    wlan_ap_netif = ap_netif;
    return;
}

void wlan_rx_pkt(peer_buff_struct *peer_buff_ptr)
{
    struct pbuf *pbuf_ptr;
#if defined (WLAN_ZERO_COPY)
    pbuf_ptr = flc2_to_pbuf(peer_buff_ptr);
    KAL_PUBLIC_TRACE(TRACE_MSG, "[%d->%d]%d\n", MOD_WNDRV, MOD_TCPIP, MSG_ID_TCPIP_WNDRV_DATA_IND);
    wlan_sys_rx_pkt(pbuf_ptr);
#else
    if (wlan_input == NULL) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "wlan_rx_register_callback abnormal\n");
        return;
    }
    unsigned char *data_p = NULL;
    kal_uint16 pdu_len = 0;
    data_p = (kal_uint8 *)(get_pdu_ptr(peer_buff_ptr, &pdu_len));
    pbuf_ptr = pbuf_alloc(PBUF_RAW, pdu_len, PBUF_RAM);
    if (pbuf_ptr == NULL) {
        free_peer_buff(peer_buff_ptr);
        ASSERT(pbuf_ptr);
        return;
    }
    kal_mem_cpy(pbuf_ptr->payload, data_p, peer_buff_ptr->pdu_len);
    pbuf_ptr->len = peer_buff_ptr->pdu_len;
    pbuf_ptr->tot_len = peer_buff_ptr->pdu_len;
    wlan_input(wlan_sta_netif, pbuf_ptr);//need judge station/ap netif according to mac address in station/ap concurrent mode
    free_peer_buff(peer_buff_ptr);
#endif

}

#ifdef __HOTSPOT_SUPPORT__
int wlan_get_assoc_list(char *buf, unsigned int buf_size)
{
    ilm_struct *pcurrent_ilm;
    hostap_abm_sta_list_cnf_struct *plist_cnf;
    struct hostap_abm_sta_list_req_struct *req;
    struct maclist *mac_list = (struct maclist *) buf;
    unsigned int max = (buf_size - sizeof(int)) / ETHER_ADDR_LEN;
    mac_list->count = max;
    req = (struct hostap_abm_sta_list_req_struct *)hostap_allocate_local_buffer(sizeof(hostap_abm_sta_list_req_struct));
    adapter_send_hostapd_req((void *)req, MSG_ID_HOSTAP_ABM_STA_LIST_REQ);
    ADAPTER_DEBUG(0, ("waiting for assoc list msg from wndrv\n"));
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_HOSTAP_ABM_STA_LIST_CNF, 0);
    if (pcurrent_ilm) {
        int i = 0;
        struct ether_addr *pcurrent_mac = &mac_list->ea[0];
        plist_cnf = (hostap_abm_sta_list_cnf_struct *)pcurrent_ilm->local_para_ptr;
        if (plist_cnf) {
            if (mac_list->count > plist_cnf->sta_num) {
                mac_list->count = plist_cnf->sta_num;
            }
            for (i = 0; i < mac_list->count; i++) {
                memcpy(pcurrent_mac, plist_cnf->sta_info[i].addr, ETHER_ADDR_LEN);
                pcurrent_mac++;
            }
        }
        ADAPTER_DEBUG(0, ("current station count = %d\n", i));
        free_ilm(pcurrent_ilm);
        return 1;
    } else {
        return -1;
    }

}
int wlan_start_ap(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption,
                  const char *key, WLM_CHANNEL channel, int ssidBcst)
{

    ilm_struct *pcurrent_ilm = NULL;
    //hostap_abm_pre_init_req_struct* pre_req;
    hostap_abm_init_req_struct *req;
    hostap_abm_init_cnf_struct *init_cnf;

    ADAPTER_DEBUG(0, ("wlan_start_ap\n"));
    //step 1. pre-init

#if 0
    /*step3-send message to hostapd to do pre-init, because later system will get mac address before start SW AP*/
    pre_req = (hostap_abm_pre_init_req_struct *)hostap_allocate_local_buffer(sizeof(hostap_abm_pre_init_req_struct));
//  kal_mem_set(pre_req, 0, sizeof(hostap_abm_pre_init_req_struct));
    pre_req->cb = (void *)cb_from_wlan_c;
    adapter_send_hostapd_req((void *)pre_req, MSG_ID_HOSTAP_ABM_PRE_INIT_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_HOSTAP_ABM_PREINIT_CNF, 15 * 1000);
    if (!pcurrent_ilm) {
        ADAPTER_DEBUG(0, ("wlan_start_ap PRE INIT fail\n"));
        return -1;
    }
    free_ilm(pcurrent_ilm);
#endif
//  OSTimeDly(5000); //delay 5 seconds to next action

    //step2. normal init
    req = (hostap_abm_init_req_struct *)hostap_allocate_local_buffer(sizeof(hostap_abm_init_req_struct));
//  kal_mem_set(req, 0, sizeof(hostap_abm_init_req_struct));
    req->hostap_config_profile.ssid_len = strlen(ssid);
    memcpy(req->hostap_config_profile.ssid, ssid, strlen(ssid));
    req->hostap_config_profile.psk_len = strlen(key);
    memcpy(req->hostap_config_profile.psk, key, strlen(key));
    req->hostap_config_profile.channel = channel;
    req->hostap_config_profile.max_sta_num = 8;
    if (authMode != WLM_WPA_AUTH_DISABLED) {
        req->hostap_config_profile.fgIsPsk = 1;
        if (authMode == WLM_WPA_AUTH_PSK) {
            req->hostap_config_profile.auth_alg = HOSTAP_AUTH_ALG_WPA_PSK;
        } else if (authMode == WLM_WPA2_AUTH_PSK) {
            req->hostap_config_profile.auth_alg = HOSTAP_AUTH_ALG_WPA2_PSK;
        } else if (authMode == WLM_WPA_AUTH_NONE) {
            req->hostap_config_profile.auth_alg = HOSTAP_AUTH_ALG_NONE;
        }
    } else {
        req->hostap_config_profile.auth_alg = HOSTAP_AUTH_ALG_NONE;
    }
    ADAPTER_DEBUG(0, ("wlan_start_ap ap: ssid = %s, authMode = %d, encryption = %d, key = %s, channel = %d, ssidBcst= %d\n",
                      ssid, authMode, encryption, key, channel, ssidBcst));
    adapter_send_hostapd_req((void *)req, MSG_ID_HOSTAP_ABM_INIT_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_HOSTAP_ABM_INIT_CNF, 10 * 1000);
    if (pcurrent_ilm) {
        init_cnf = (hostap_abm_init_cnf_struct *)pcurrent_ilm->local_para_ptr;
        if (init_cnf->status == 1) {
            memcpy(mt5931_mac_address_ap, init_cnf->ap, sizeof(mt5931_mac_address_ap));
            ADAPTER_DEBUG(0, ("wlan_start_ap successfully, ap address=%02x:%02x:%02x:%02x:%02x:%02x!\n", PRINT_MAC(mt5931_mac_address_ap)));
            free_ilm(pcurrent_ilm);
            return 1;
        }
        free_ilm(pcurrent_ilm);
    }
    ADAPTER_DEBUG(0, ("wlan_start_ap fail!\n"));
    return -1;
}

KAL_BOOL wlan_stop_ap(void)
{
    ilm_struct *pcurrent_ilm = NULL;
    hostap_abm_deinit_req_struct *req;

    req = (hostap_abm_deinit_req_struct *)hostap_allocate_local_buffer(sizeof(hostap_abm_deinit_req_struct));
    adapter_send_hostapd_req((void *)req, MSG_ID_HOSTAP_ABM_DEINIT_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_HOSTAP_ABM_DEINIT_CNF, 0);
    if (pcurrent_ilm) {
        free_ilm(pcurrent_ilm);
        return KAL_TRUE;
    } else {
        return KAL_FALSE;
    }
}

#endif

KAL_BOOL wlan_start_sta(void)
{
    ilm_struct *pcurrent_ilm;
    supc_abm_wifi_init_cnf_struct *local_para_ptr = NULL;
    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_INIT_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_INIT_CNF, 0);
    if (pcurrent_ilm) {
        local_para_ptr = (supc_abm_wifi_init_cnf_struct *)pcurrent_ilm->local_para_ptr;
        ADAPTER_DEBUG(0, ("start supc successfully, my own mac=%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(local_para_ptr->mac_addr)));
        memcpy(mt5931_mac_address_sta, local_para_ptr->mac_addr, sizeof(mt5931_mac_address_sta));
        memcpy(mt5931_mac_address_ap, mt5931_mac_address_sta, sizeof(mt5931_mac_address_sta));
        mt5931_mac_address_ap[0] |= 0x02;
        free_ilm(pcurrent_ilm);
        return KAL_TRUE;
    } else {
        return KAL_FALSE;
    }
}
KAL_BOOL wlan_stop_sta(void)
{
    ilm_struct *pcurrent_ilm;
    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_DEINIT_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_DEINIT_CNF, 0);
    if (pcurrent_ilm) {
        ADAPTER_DEBUG(0, ("supc stop successfully\n"));
        free_ilm(pcurrent_ilm);
        return KAL_TRUE;
    } else {
        return KAL_FALSE;
    }
}

#if 0
int wlan_scan_network(char *scan_buf, kal_uint32 buf_size)
{
    ilm_struct *pcurrent_ilm;
    WLAN_SCAN_RESULT *scan_result = (WLAN_SCAN_RESULT *)scan_buf;
    WLAN_BSS_INFO *pbss = scan_result->bss_info;
    int count = 0;
    int i = 0;
    char wait_flag = 0;
    supc_abm_wifi_ap_list_cnf_struct *local_para_ptr = NULL;
    if (gladapter_wps_state == WPS_STATE_INIT) {
        ADAPTER_DEBUG(0, ("In WPS process, ignoe scan!!!\n"));
        return -1;
    }
    if (scan_buf) {
        memset(scan_buf, 0, buf_size);
    }
    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_AP_LIST_REQ);
    while (!wait_flag) {
        pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF, 20000);

        if (pcurrent_ilm) {
            local_para_ptr = (supc_abm_wifi_ap_list_cnf_struct *)pcurrent_ilm->local_para_ptr;
            for (i = 0; i < local_para_ptr->num_of_bss &&
                    ((3 * sizeof(kal_uint32) + (count + 1) * sizeof(WLAN_BSS_INFO)) <= buf_size); i++) {
                memcpy(pbss->BSSID, local_para_ptr->bss_info[i].bssid, 6);
                memcpy(pbss->SSID, local_para_ptr->bss_info[i].ssid, local_para_ptr->bss_info[i].ssid_len);
                pbss->SSID_len = local_para_ptr->bss_info[i].ssid_len;
                pbss->beacon_period = 100;  //default
                pbss->chanspec = local_para_ptr->bss_info[i].channel_number;
                pbss->RSSI = local_para_ptr->bss_info[i].rssi;
                pbss->length = sizeof(WLAN_BSS_INFO);
                pbss->authMode = 0;
                pbss->encryption = 0;
                if (local_para_ptr->bss_info[i].privacy)  {
                    //Support WPA2
                    if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode |= WLM_WPA2_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode |= WLM_WPA2_AUTH_PSK;
                        }

                        if (local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher & WPA_CIPHER_TKIP) {
                            pbss->encryption |= WLM_ENCRYPT_TKIP;
                        }
                        if (local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher & WPA_CIPHER_CCMP) {
                            pbss->encryption |= WLM_ENCRYPT_AES;
                        }
                    }
                    //Support WPA
                    if (local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].wpa_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode |= WLM_WPA_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode |= WLM_WPA_AUTH_PSK;
                        }

                        if (local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher & WPA_CIPHER_TKIP) {
                            pbss->encryption |= WLM_ENCRYPT_TKIP;
                        }
                        if (local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher & WPA_CIPHER_CCMP) {
                            pbss->encryption |= WLM_ENCRYPT_AES;
                        }
                    }
                    //Support WEP or WAPI
                    if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_FALSE && local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_FALSE) {
                        pbss->authMode |= WLM_WPA_AUTH_DISABLED;//may regard WAPI AP as WEP AP
                        pbss->encryption |= WLM_ENCRYPT_WEP;
                    }
                }/*end if(..privacy)*/

                pbss++;
                count++;
                ADAPTER_DEBUG(0, ("BSS:%02x:%02x:%02x:%02x:%02x:%02x, ssid[%s], ch=%d\n", PRINT_MAC(local_para_ptr->bss_info[i].bssid),
                                  ssid_txt(local_para_ptr->bss_info[i].ssid, local_para_ptr->bss_info[i].ssid_len), local_para_ptr->bss_info[i].channel_number));
            }


            if (local_para_ptr->more_flag == FALSE) {
                wait_flag = 1;
            }
            free_ilm(pcurrent_ilm);
        } else {
            ADAPTER_DEBUG(0, ("no more received bss/scan request no response!\n"));
            break;
        }
    }

    scan_result->buflen = 3 * sizeof(kal_uint32) + count * sizeof(WLAN_BSS_INFO);
    scan_result->count = count;
    ADAPTER_DEBUG(0, ("wlan_scan_network scan_result->count=%d, buflen=%d\n", scan_result->count, scan_result->buflen));

    return 0;

}
#endif

scan_item_callback scan_item_cb = NULL;

void wifi_scan_item_register_callback(scan_item_callback cb)
{
    scan_item_cb = cb;
}
#if 0
int wlan_scan_network(char *scan_buf, kal_uint32 buf_size)
{
    ilm_struct *pcurrent_ilm;
    WLAN_SCAN_RESULT *single_scan_buf = (WLAN_SCAN_RESULT *)MEM_MALLOC(sizeof(WLAN_SCAN_RESULT));
    WLAN_SCAN_RESULT *scan_result = (WLAN_SCAN_RESULT *)single_scan_buf;
    WLAN_BSS_INFO *pbss = scan_result->bss_info;
    int count = 0;
    int i = 0;
    char wait_flag = 0;
    supc_abm_wifi_ap_list_cnf_struct *local_para_ptr = NULL;

    if (gladapter_wps_state == WPS_STATE_INIT) {
        ADAPTER_DEBUG(0, ("In WPS process, ignoe scan!!!\n"));
        MEM_FREE(single_scan_buf);
        return -1;
    }

    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_AP_LIST_REQ);
    while (!wait_flag) {
        pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF, 20000);

        if (pcurrent_ilm) {
            local_para_ptr = (supc_abm_wifi_ap_list_cnf_struct *)pcurrent_ilm->local_para_ptr;
            for (i = 0; i < local_para_ptr->num_of_bss; i++) {
                memset(single_scan_buf, 0, sizeof(WLAN_SCAN_RESULT));
                memcpy(pbss->BSSID, local_para_ptr->bss_info[i].bssid, 6);
                memcpy(pbss->SSID, local_para_ptr->bss_info[i].ssid, local_para_ptr->bss_info[i].ssid_len);
                pbss->SSID_len = local_para_ptr->bss_info[i].ssid_len;
                pbss->beacon_period = 100;  //default
                pbss->chanspec = local_para_ptr->bss_info[i].channel_number;
                pbss->RSSI = local_para_ptr->bss_info[i].rssi;
                pbss->length = sizeof(WLAN_BSS_INFO);
                pbss->authMode = 0;
                pbss->encryption = 0;
                if (local_para_ptr->bss_info[i].privacy)  {
                    if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE && local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode = WLM_WPA_WPA2_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode = WLM_WPA_WPA2_AUTH_PSK;
                        }

                        if (local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher & WPA_CIPHER_TKIP) {
                            pbss->encryption  = WLM_ENCRYPT_TKIP;
                        }
                        if (local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher & WPA_CIPHER_CCMP) {
                            pbss->encryption  = WLM_ENCRYPT_AES;
                        }
                        if ((local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher & WPA_CIPHER_CCMP) &&
                                (local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher & WPA_CIPHER_TKIP)) {
                            pbss->encryption  = WLM_ENCRYPT_TKIP_AES;
                        }

                    }
                    //Support WPA2
                    else if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode = WLM_WPA2_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode = WLM_WPA2_AUTH_PSK;
                        }

                        pbss->encryption  = WLM_ENCRYPT_AES;
                    }
                    //Support WPA
                    else if (local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].wpa_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode = WLM_WPA_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode = WLM_WPA_AUTH_PSK;
                        }

                        pbss->encryption = WLM_ENCRYPT_TKIP;
                    }
                    //Support WEP or WAPI
                    else if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_FALSE && local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_FALSE) {
                        pbss->authMode  = WLM_WPA_AUTH_DISABLED;//may regard WAPI AP as WEP AP
                        pbss->encryption = WLM_ENCRYPT_WEP;
                    }
                } else {
                    pbss->authMode  = WLM_WPA_AUTH_DISABLED;//may regard WAPI AP as WEP AP
                    pbss->encryption = WLM_ENCRYPT_NONE;
                }
                if (scan_item_cb != NULL) {
                    scan_item_cb(pbss);
                }
                ADAPTER_DEBUG(0, ("BSS:%02x:%02x:%02x:%02x:%02x:%02x, ssid[%s], ch=%d\n", PRINT_MAC(local_para_ptr->bss_info[i].bssid),
                                  ssid_txt(local_para_ptr->bss_info[i].ssid, local_para_ptr->bss_info[i].ssid_len), local_para_ptr->bss_info[i].channel_number));
            }

            if (local_para_ptr->more_flag == FALSE) {
                wait_flag = 1;
                if (scan_item_cb != NULL) {
                    scan_item_cb(NULL);
                }
            }
            free_ilm(pcurrent_ilm);
        } else {
            ADAPTER_DEBUG(0, ("no more received bss/scan request no response!\n"));
            break;
        }
    }

    //scan_result->buflen = 3 * sizeof(kal_uint32) + count * sizeof(WLAN_BSS_INFO);
    //scan_result->count = count;
    //ADAPTER_DEBUG(0, ("wlan_scan_network scan_result->count=%d, buflen=%d\n", scan_result->count, scan_result->buflen));
    MEM_FREE(single_scan_buf);
    return 0;

}
#endif
extern WLAN_SCAN_RESULT *g_scan_list;
extern uint32_t g_scan_list_size;
extern uint8_t  g_scan_inited;

int wlan_scan_network(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option)
{
    ilm_struct *pcurrent_ilm;
    WLAN_SCAN_RESULT *scan_result = g_scan_list;
    WLAN_BSS_INFO *pbss = scan_result->bss_info;
    int count = 0;
    int i = 0;
    char wait_flag = 0;
    supc_abm_wifi_ap_list_cnf_struct *local_para_ptr = NULL;
    wifi_event_handler_t handler;

    if (gladapter_wps_state == WPS_STATE_INIT) {
        ADAPTER_DEBUG(0, ("In WPS process, ignoe scan!!!\n"));
        return -1;
    }
    printc("scan_buf_size = %d\n\r", g_scan_list_size);

    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_AP_LIST_REQ);
    while (!wait_flag) {
        pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_AP_LIST_CNF, 3000);

        if (pcurrent_ilm) {
            local_para_ptr = (supc_abm_wifi_ap_list_cnf_struct *)pcurrent_ilm->local_para_ptr;
            for (i = 0; i < local_para_ptr->num_of_bss &&
                    ((sizeof(WLAN_SCAN_RESULT) - sizeof(WLAN_BSS_INFO) + (count + 1) * sizeof(WLAN_BSS_INFO)) <= g_scan_list_size); i++) {

                //memset(single_scan_buf, 0, sizeof(WLAN_SCAN_RESULT));
                kal_mem_cpy(pbss->BSSID, local_para_ptr->bss_info[i].bssid, 6);
                kal_mem_cpy(pbss->SSID, local_para_ptr->bss_info[i].ssid, local_para_ptr->bss_info[i].ssid_len);
                pbss->SSID_len = local_para_ptr->bss_info[i].ssid_len;
                pbss->beacon_period = 100;  //default
                pbss->chanspec = local_para_ptr->bss_info[i].channel_number;
                pbss->RSSI = local_para_ptr->bss_info[i].rssi;
                pbss->length = sizeof(WLAN_BSS_INFO);
                pbss->authMode = 0;
                pbss->encryption = 0;
                if (local_para_ptr->bss_info[i].privacy)  {
                    if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE && local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode = WLM_WPA_WPA2_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode = WLM_WPA_WPA2_AUTH_PSK;
                        }

                        if (local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher & WPA_CIPHER_TKIP) {
                            pbss->encryption  = WLM_ENCRYPT_TKIP;
                        }
                        if (local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher & WPA_CIPHER_CCMP) {
                            pbss->encryption  = WLM_ENCRYPT_AES;
                        }
                        if ((local_para_ptr->bss_info[i].rsn_ie_info.pairwise_cipher & WPA_CIPHER_CCMP) &&
                                (local_para_ptr->bss_info[i].wpa_ie_info.pairwise_cipher & WPA_CIPHER_TKIP)) {
                            pbss->encryption  = WLM_ENCRYPT_TKIP_AES;
                        }

                    }
                    //Support WPA2
                    else if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].rsn_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode = WLM_WPA2_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode = WLM_WPA2_AUTH_PSK;
                        }

                        pbss->encryption  = WLM_ENCRYPT_AES;
                    }
                    //Support WPA
                    else if (local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_TRUE) {
                        if (local_para_ptr->bss_info[i].wpa_ie_info.key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                            pbss->authMode = WLM_WPA_AUTH_ENTERPRISE;
                        } else {
                            pbss->authMode = WLM_WPA_AUTH_PSK;
                        }

                        pbss->encryption = WLM_ENCRYPT_TKIP;
                    }
                    //Support WEP or WAPI
                    else if (local_para_ptr->bss_info[i].rsn_ie_info_p == KAL_FALSE && local_para_ptr->bss_info[i].wpa_ie_info_p == KAL_FALSE) {
                        pbss->authMode  = WLM_WPA_AUTH_DISABLED;//may regard WAPI AP as WEP AP
                        pbss->encryption = WLM_ENCRYPT_WEP;
                    }
                } else {
                    pbss->authMode  = WLM_WPA_AUTH_DISABLED;//may regard WAPI AP as WEP AP
                    pbss->encryption = WLM_ENCRYPT_NONE;
                }
                //(scan_item_cb != NULL)
                //  scan_item_cb(pbss);

                pbss++;
                count++;
                ADAPTER_DEBUG(0, ("BSS:%02x:%02x:%02x:%02x:%02x:%02x, ssid[%s], ch=%d\n", PRINT_MAC(local_para_ptr->bss_info[i].bssid),
                                  ssid_txt(local_para_ptr->bss_info[i].ssid, local_para_ptr->bss_info[i].ssid_len), local_para_ptr->bss_info[i].channel_number));
            }

            if (local_para_ptr->more_flag == FALSE) {
                wait_flag = 1;
                //if(scan_item_cb != NULL)
                //    scan_item_cb(NULL);
            }
            free_ilm(pcurrent_ilm);
        } else {
            ADAPTER_DEBUG(0, ("no more received bss/scan request no response!\n"));
            break;
        }
    }

    scan_result->buflen = sizeof(WLAN_SCAN_RESULT) - sizeof(WLAN_BSS_INFO) + count * sizeof(WLAN_BSS_INFO);
    scan_result->count = count;

    handler = wifi_rx_event_handler[WIFI_EVENT_IOT_SCAN_COMPLETE];
    if (NULL != wifi_rx_event_handler[WIFI_EVENT_IOT_SCAN_COMPLETE]) {
        handler(WIFI_EVENT_IOT_SCAN_COMPLETE, (kal_uint8 *)scan_result, scan_result->buflen);
    }
    ADAPTER_DEBUG(0, ("wlan_scan_network scan_result->count=%d, buflen=%d\n", scan_result->count, scan_result->buflen));
    //MEM_FREE(single_scan_buf);
    return 0;

}


int wlan_get_connection_info(wl_connection_info_t *wlan_connection_info)
{
    ilm_struct *pcurrent_ilm;
    supc_abm_wifi_get_connection_info_cnf_struct *local_para_ptr = NULL;

    if(gladapter_mode_sta_status == ADAPTER_MOD_STA_DISCONNECTED) {
        wlan_connection_info->channel = 0;
        wlan_connection_info->Phy_rate = 0;
        wlan_connection_info->Rssi = 0;
        wlan_connection_info->Ssid_len = 0;
        return 0;
    }
    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_GET_CONNECTION_INFO_CNF, 1000);
    if (pcurrent_ilm) {
        local_para_ptr = (supc_abm_wifi_get_connection_info_cnf_struct *)pcurrent_ilm->local_para_ptr;
        wlan_connection_info->channel = local_para_ptr->channel;
        wlan_connection_info->Phy_rate = local_para_ptr->phy_rate;
        wlan_connection_info->Rssi = local_para_ptr->rssi;
        wlan_connection_info->Ssid_len = local_para_ptr->ssid_len;
        kal_mem_cpy(wlan_connection_info->Ssid, local_para_ptr->ssid, local_para_ptr->ssid_len);
        free_ilm(pcurrent_ilm);
    } else {
        ADAPTER_DEBUG(0, ("wlan_get_connection_info no response\n"));
        return -1;
    }

    return 0;
}

void *abm_allocate_local_buffer(kal_uint32 size)
{
    return construct_local_para(size, TD_RESET);
}



int wlan_real_join_network(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption, const char *key
                           , kal_uint8 wep_key_index, WLAN_WPS_CONFIG *wps_config, struct os_time *timeout)
{
    ilm_struct *pcurrent_ilm = NULL;
    supc_abm_wifi_connect_req_struct *req = NULL;
    wifi_data_account_profile_struct *pprofile = NULL;
    supc_abm_wifi_connect_cnf_struct *pind = NULL;
    int i = 0;
    int ret = 0;
    kal_uint32 wait_time;   //for normal ap connection timeout
    kal_uint8 flg_wps_1st_round = 1;
    ADAPTER_DEBUG(0, ("wlan_join_network ssid=%s, authMode=%d, encryption=%d, key=%s\n",
                      ssid_txt((kal_uint8 *)ssid, strlen(ssid)), authMode, encryption, key_txt((kal_uint8 *)key, strlen(key))));
    if (strlen(ssid) > 32 || strlen(key) > 64) {
        ADAPTER_DEBUG(0, ("ERROR:Invalid Input Command\n"));
        return -1;
    }

    if (timeout == NULL) {
        wait_time = 10000;
    } else {
        wait_time = timeout->sec * 1000 + timeout->usec / 1000;
    }

    req = (supc_abm_wifi_connect_req_struct *)abm_allocate_local_buffer(sizeof(supc_abm_wifi_connect_req_struct));
//  kal_mem_set(req, 0, sizeof(supc_abm_wifi_connect_req_struct));
    if (!req) {
        ADAPTER_DEBUG(0, ("error!\n"));
        return -1;
    }

    pprofile = req->profile_header_ptr;

    if (!wps_config) {
        //non wps mode
        pprofile->ssid_len = strlen(ssid);
        memcpy(pprofile->ssid, ssid, strlen(ssid));
        switch (authMode) {
            case WLM_WPA_AUTH_PSK:        //wpa-psk
                pprofile->auth_alg = WPA_AUTH_ALG_OPEN;
                pprofile->key_mgmt = WPA_KEY_MGMT_PSK;
                pprofile->proto = WPA_PROTO_RSN | WPA_PROTO_WPA;
                break;
            case WLM_WPA2_AUTH_PSK:       //wpa2-psk
                pprofile->auth_alg = WPA_AUTH_ALG_OPEN;
                pprofile->key_mgmt = WPA_KEY_MGMT_PSK;
                pprofile->proto = WPA_PROTO_RSN | WPA_PROTO_WPA;
                break;
            default:
                break;
        }
        switch (encryption) {
            case WLM_ENCRYPT_TKIP:
                pprofile->pairwise = WPA_CIPHER_TKIP;
                break;
            case WLM_ENCRYPT_AES:
                pprofile->pairwise = WPA_CIPHER_CCMP;
                break;
            default:
                break;
        }
        pprofile->group = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP |
                          WPA_CIPHER_WEP104 | WPA_CIPHER_WEP40;
        //psk
        if (authMode == WLM_WPA_AUTH_PSK || authMode == WLM_WPA2_AUTH_PSK) {
            pbkdf2_sha1(key, ssid, strlen(ssid), 4096, pprofile->psk, 32);
            pprofile->psk_p = KAL_TRUE;
            ADAPTER_DEBUG(0, ("psk:\n"));
            for (i = 0; i < 32; i++) {
                printc("%x", pprofile->psk[i]);
            }
            ADAPTER_DEBUG(0, ("\n"));
        } else if ((authMode == WLM_WPA_AUTH_DISABLED) || (authMode == WLM_WPA_AUTH_NONE)) {
            //wep
            if (encryption == WLM_ENCRYPT_WEP) {
                pprofile->auth_alg = WPA_AUTH_ALG_SHARED | WPA_AUTH_ALG_OPEN;
                pprofile->key_mgmt = WPA_KEY_MGMT_NONE;
                pprofile->wep_info_p = KAL_TRUE;
                pprofile->wep_keys.wep_tx_keyidx = wep_key_index;
                if ((10 == strlen(key)) || (26 == strlen(key))) {
                    //This is HEX format len =10 or len =26
                    pprofile->wep_keys.wep_key_len[pprofile->wep_keys.wep_tx_keyidx] = strlen(key) / 2;
                    AtoH((char *)key, (char *)&pprofile->wep_keys.wep_key[pprofile->wep_keys.wep_tx_keyidx], (int)pprofile->wep_keys.wep_key_len[pprofile->wep_keys.wep_tx_keyidx]);
                } else {
                    //This is ASCII format len =5 or len =13
                    pprofile->wep_keys.wep_key_len[pprofile->wep_keys.wep_tx_keyidx] = strlen(key);
                    memcpy(pprofile->wep_keys.wep_key[pprofile->wep_keys.wep_tx_keyidx], key, strlen(key));
                }

                if (pprofile->wep_keys.wep_key_len[pprofile->wep_keys.wep_tx_keyidx] <= 5) {
                    pprofile->group = WPA_CIPHER_WEP40; //| WPA_CIPHER_WEP104;
                    pprofile->pairwise = WPA_CIPHER_WEP40; // | WPA_CIPHER_WEP104;
                } else if (pprofile->wep_keys.wep_key_len[pprofile->wep_keys.wep_tx_keyidx] > 5 && pprofile->wep_keys.wep_key_len[pprofile->wep_keys.wep_tx_keyidx] <= 13) {

                    pprofile->group = WPA_CIPHER_WEP104; //| ;
                    pprofile->pairwise = WPA_CIPHER_WEP104; // | ;
                }
            }
            //none
            else if (encryption == WLM_ENCRYPT_NONE) {
                pprofile->auth_alg = WPA_AUTH_ALG_OPEN;
                pprofile->key_mgmt = WPA_KEY_MGMT_NONE;
                pprofile->group = WPA_CIPHER_NONE;
                pprofile->pairwise = WPA_CIPHER_NONE;
            }
        }
    }
#ifdef __WPS_SUPPORT__
    else { //wps mode
        if (wps_config->WscMode == 1) { //pin
            req->connect_type = START_WPS_PIN;
            pprofile->pin = atoi((char *)wps_config->PIN);
        } else if (wps_config->WscMode == 2) { // pbc
            req->connect_type = START_WPS_PBC;
        } else {
            ADAPTER_DEBUG(0, ("not supported WscMode=%d\n", wps_config->WscMode));
        }
        gladapter_wps_state = WPS_STATE_INIT;
        if (timeout == NULL) {
            wait_time = WPS_WALK_TIME / 2 * 1000;    // for uint <= 65535, need wait 120000ms
        } else {
            wait_time = timeout->sec * 1000 + timeout->usec / 1000;
        }
        flg_wps_1st_round = 1;
    }
#endif
    pprofile->priority = 1;
    pprofile->network_type = WNDRV_SUPC_NETWORK_INFRASTRUCTURE;
    req->num_of_profile = 1;
    adapter_send_supc_req((void *)req, MSG_ID_SUPC_ABM_WIFI_CONNECT_REQ);
    if (wps_config) {
        gladapter_wps_state = WPS_STATE_INIT;
        ADAPTER_DEBUG(0, ("do not wait for connection confirm since wps connection\n"));
        return ret;
    }
second_round:
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_CONNECT_CNF, wait_time);
    if (pcurrent_ilm) {
        pind = (supc_abm_wifi_connect_cnf_struct *)pcurrent_ilm->local_para_ptr;
        if (pind->result == KAL_TRUE) {
            ADAPTER_DEBUG(0, ("Connect successfully! bssid %02x:%02x:%02x:%02x:%02x:%02x, ssid:%s, ch = %d\n", PRINT_MAC(pind->bssid)
                              , ssid_txt(pind->ssid, pind->ssid_len), pind->channel_number));
        } else {
            ADAPTER_DEBUG(0, ("Connection fail\n"));
            ret = -1;
        }
        free_ilm(pcurrent_ilm);
    } else {
        if (wps_config && flg_wps_1st_round) {   //for uint <= 65535, we need wait 120000ms
            flg_wps_1st_round = 0;
            goto second_round;
        }
        ADAPTER_DEBUG(0, ("Connection with no response\n"));
        ret = -1;
    }
    return ret;
}

int wlan_join_network(char *ssid, WLM_AUTH_MODE authMode, WLM_ENCRYPTION encryption, const char *key, kal_uint8 wep_key_index, struct os_time *timeout)
{
    return wlan_real_join_network(ssid, authMode, encryption, key, wep_key_index, NULL, timeout);
}

int wlan_wps_start(WPS_CONFIG_INFO wps_config_info, wlan_wps_event_callback cb)
{
    WLAN_WPS_CONFIG wps_config;
    wps_config.WscMode = wps_config_info.WscMode; //pbc
    memcpy(wps_config.PIN, wps_config_info.PIN, 8);
    wps_cb_from_wlan_c = cb;
    return  wlan_real_join_network(NULL, 0, 0, NULL, 0, &wps_config, NULL);

}
int wlan_wps_get_credentials(WLAN_CONFIG_INFO *wlan_config_info)
{
    ilm_struct *pcurrent_ilm = NULL;
    supc_abm_wifi_wps_credential_struct *pcnf = NULL;
    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_WPS_GET_CREDENTIAL_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_WPS_GET_CREDENTIAL_CNF, 1000);
    if (pcurrent_ilm) {
        /*
        ulong AuthType;         // mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa2-psk
        ulong EncrType;         // mandatory, 1: none, 2: wep, 4: tkip, 8: aes
        */
        pcnf = (supc_abm_wifi_wps_credential_struct *)pcurrent_ilm->local_para_ptr;
        wlan_config_info->Auth_Mode = pcnf->auth_type;   //???equeal??
        wlan_config_info->Enc_Mode = pcnf->encr_type;
        if (pcnf->auth_type == 2 || pcnf->auth_type == 0x20) { //wpa2-psk or wpa-psk
            wlan_config_info->Psk_length = pcnf->key_len;
            kal_mem_cpy(wlan_config_info->PresharedKey, pcnf->key, pcnf->key_len);
            hexdump("credential dump psk:", pcnf->key, pcnf->key_len);
        } else if (pcnf->auth_type == 1 && pcnf->encr_type == 2) { //wep
            wlan_config_info->WepKeyToUse = pcnf->key_idx;
            kal_mem_cpy(wlan_config_info->Wep_Key[pcnf->key_idx].KeyMaterial, pcnf->key, pcnf->key_len);
            wlan_config_info->Wep_Key[pcnf->key_idx].KeyLength = pcnf->key_len;
            hexdump("credential dump wep key:", pcnf->key, pcnf->key_len);
        }
        wlan_config_info->Ssid.SsidLength = pcnf->ssid_len;
        kal_mem_cpy(wlan_config_info->Ssid.Ssid, pcnf->ssid, pcnf->ssid_len);
        kal_mem_cpy(wlan_config_info->InfraBssid, pcnf->mac_addr, 6);

        wlan_config_info->InfrastructureMode = 1;
        ADAPTER_DEBUG(0, ("%s: credential ssid=%s bssid=%02x:%02x:%02x:%02x:%02x:%02x auth_mode=%d enc_mode=%d\n",
                          __func__, ssid_txt(pcnf->ssid, pcnf->ssid_len), PRINT_MAC(pcnf->mac_addr), pcnf->auth_type, pcnf->encr_type));

        free_ilm(pcurrent_ilm);
    } else {
        ADAPTER_DEBUG(0, ("wps get credential with no response\n"));
        return -1;
    }

    return 0;
}

int wlan_disconnect_network(void)
{
    ilm_struct *pcurrent_ilm = NULL;
    adapter_send_supc_req(NULL, MSG_ID_SUPC_ABM_WIFI_DISCONNECT_REQ);
    pcurrent_ilm = adapter_wait_for_msg(MSG_ID_SUPC_ABM_WIFI_DISCONNECT_CNF, 10000);
    if (pcurrent_ilm) {
        ADAPTER_DEBUG(0, ("wlan disconnect network successfully\n"));
        free_ilm(pcurrent_ilm);
        return 0;
    }
    return -1;
}
int wlan_wps_stop(void)
{
    wlan_disconnect_network();
    if (gladapter_wps_state != WPS_STATE_OFF) {
        ADAPTER_DEBUG(0, ("ERROR: %s gladapter_wps_state=%d\n", __func__, gladapter_wps_state));
        gladapter_wps_state = WPS_STATE_OFF;
    }
    return 0;
}
WPS_STATE wlan_wps_get_state(void)
{
    return gladapter_wps_state;
}
int wlan_wps_gen_pin(kal_uint32 *pin)
{
    kal_uint32 val;
    kal_uint32 check_sum = 0;
    kal_uint8 *buf = (kal_uint8 *)&val;
    /* Generate seven random digits for the PIN */
    kal_uint16 i = 0;
    unsigned int accum = 0;
    for (i = 0; i < sizeof(val); i++) {
        buf[i] = rand();
    }
    val %= 10000000;
    check_sum = val;

    while (check_sum) {
        accum += 3 * (check_sum % 10);
        check_sum /= 10;
        accum += check_sum % 10;
        check_sum /= 10;
    }
    check_sum = (10 - accum % 10) % 10;
    /* Append checksum digit */
    *pin = val * 10 + check_sum;
    return 1;
}

void supc_cb_disconnected(void)
{
#if 0
    //use 7687 handler
    if (cb_from_wlan_c) {
        cb_from_wlan_c(WLAN_E_DISASSOC_IND);
    }
#endif
    wifi_event_handler_t handler;

    handler = wifi_rx_event_handler[WIFI_EVENT_IOT_DISCONNECTED];

    handler(WIFI_EVENT_IOT_DISCONNECTED, NULL, 0);

    gladapter_mode_sta_status = ADAPTER_MOD_STA_DISCONNECTED;
}

void supc_cb_connected(void)
{
#if 0
    //use 7687 handler
    if (cb_from_wlan_c) {
        cb_from_wlan_c(WLAN_E_SET_SSID);
    }
#endif
    wifi_event_handler_t handler;

    handler = wifi_rx_event_handler[WIFI_EVENT_IOT_PORT_SECURE];

    handler(WIFI_EVENT_IOT_PORT_SECURE, NULL, 0);

    gladapter_mode_sta_status = ADAPTER_MOD_STA_CONNECTED;
}

void wifi_cb_init_done(void)
{
    wifi_event_handler_t handler;

    handler = wifi_rx_event_handler[WIFI_EVENT_IOT_INIT_DONE];

    handler(WIFI_EVENT_IOT_INIT_DONE, NULL, 0);
}

void hostap_cb_sta_connected(void)
{
    if (cb_from_wlan_c) {
        cb_from_wlan_c(WLAN_E_ASSOC_IND);
    }
}
void hostap_cb_sta_disconnected(void)
{
    if (cb_from_wlan_c) {
        cb_from_wlan_c(WLAN_E_DISASSOC_IND);
    }
}

#ifdef __WPS_SUPPORT__
void supc_cb_wps_status(WPS_STATE stat)
{
    if (wps_cb_from_wlan_c) {
        wps_cb_from_wlan_c(stat);
    }
    switch (stat) {
        case WPS_STATE_OFF:
            gladapter_wps_state = WPS_STATE_OFF;
            break;
        case WPS_STATE_CONFIGURED:
            gladapter_wps_state = WPS_STATE_CONFIGURED;
            break;
        default:
            gladapter_wps_state = WPS_STATE_OFF;
            break;
    }
}

void wlan_config_func_pin(unsigned char function, unsigned char pin)
{
    WiFi_ConfigPin((WIFI_PIN_FUNC)function, pin);
}
#endif

void wlan_config_cal_data(unsigned char calibrateType, void *data)
{
    WiFi_ConfigCalData((WIFI_CAL_TYPE)calibrateType, data);
}

#if 0
void test_start_supc()
{
    ADAPTER_DEBUG(0, ("test start supc!"));
    wlan_start_sta();
}

void test_stop_supc()
{
    ADAPTER_DEBUG(0, ("test_stop_supc!"));
    wlan_stop_sta();
}

void test_supc_req_scan()
{
    char *pscan_buf;
    pscan_buf = (char *)MEM_MALLOC(4096);
    if (!pscan_buf) {
        ADAPTER_DEBUG(0, ("pscan buf allocate fail!\n"));
        return;
    }
    wlan_scan_network(pscan_buf, 4096);
    MEM_FREE(pscan_buf);

}

void test_supc_req_connect(void)
{
    wlan_join_network("HUAWEI", WLM_WPA2_AUTH_PSK, WLM_ENCRYPT_AES, "123456789");
}
void test_supc_req_connect_wep(void)
{
    wlan_join_network("tplinkwr886", WLM_WPA_AUTH_DISABLED, WLM_ENCRYPT_WEP, "12345");
}
void test_supc_req_connect_none(void)
{
    wlan_join_network("tplinkwr886", WLM_WPA_AUTH_DISABLED, WLM_ENCRYPT_NONE, "123456789");
}
void test_supc_req_wps_connect_pbc(void)
{
    WLAN_WPS_CONFIG wps_config;
    wps_config.WscMode = 2; //pbc
    wlan_real_join_network(NULL, 0, 0, NULL, &wps_config);
}
void test_supc_req_wps_connect_pin(void)
{
    WLAN_WPS_CONFIG wps_config;
    wps_config.WscMode = 1; //pin
    wlan_real_join_network(NULL, 0, 0, NULL, &wps_config);
}
void test_supc_stop_wps(void)
{
    wlan_wps_stop();
}
void test_supc_get_wps_credential(void)
{
    WLAN_CONFIG_INFO info;
    wlan_wps_get_credentials(&info);
}

#endif


int wifi_set_notification_callback(kal_uint8 enabled, kal_int32 events, wifi_event_handler_t callback)
{
    wifi_event_handler_struct *h = callback_list_head;
    wifi_event_handler_struct *p = h;
    wifi_event_handler_struct *prev = h; /* point to previous node of p, but h == p == prev at the beginning  */

    kal_task_enter_critical();

    if (enabled) { /* add */
        while (p) {
            if ((p->events == events) && (p->func == callback)) {
                LOG_W(wifi, "No need to register the same event and callback.");
                kal_task_exit_critical();
                return 0;
            }
            prev = p;
            p = p->next;
        }
        if (p == NULL) { /* Add a node to the list head */
            wifi_event_handler_struct *n = MEM_MALLOC(sizeof(wifi_event_handler_struct));
            if (n == NULL) {
                LOG_E(wifi, "No memroy for handler,event = %d.\n", events);
                return -1;
            }
            n->events = events;
            n->func = callback;
            n->next = callback_list_head;
            callback_list_head = n;
        }
    } else { /* remove callback */
        while (p) {
            if ((p->func == callback) && (p->events == events)) {
                break;
            }
            prev = p;
            p = p->next;
        }
        if (p) {
            prev->next = p->next;
            if (p == callback_list_head) {
                if (p->next == NULL) {
                    callback_list_head = NULL;
                } else {
                    callback_list_head = p->next;
                }
            }
            MEM_FREE(p);
        } else {
            LOG_E(wifi, "No match found Fail event = %d.\n", events);
            return -1;
        }

    }

    kal_task_exit_critical();
    return 0;
}


/* proxy function implementation */
int32_t wifi_event_proxy_handler(wifi_event_t event, uint8_t *payload, uint32_t length)
{
    wifi_event_handler_struct *h = callback_list_head;

    wifi_event_handler_t event_hd[SUPC_MAX_EVENT_HD_NUM];
    int event_hd_cnt = 0;

    kal_task_enter_critical();
    while (h) {
        if (h->events & (0x01 << event)) {
            //h->func(event, payload, length);
            event_hd[event_hd_cnt] = h->func;
            event_hd_cnt++;
        }
        h = h->next;
    }
#if 0
    if (h == NULL || event_hd_cnt > SUPC_MAX_EVENT_HD_NUM) {
        LOG_E(wifi, "Search event handler fail event = %d\n.", event);
        return -1;
    }
#endif
    kal_task_exit_critical();

    //assert(event_hd_cnt <= MAX_EVENT_HD_CNT);

    while (event_hd_cnt > 0) {
        event_hd_cnt--;
        event_hd[event_hd_cnt](event, payload, length);
    }

    return 1;
}



kal_int32 wlan_sta_set_power_saving(PARAM_POWER_MODE pm)
{

    kal_int32 status = wndrv_set_sta_power_saving(pm);

    if (status == 0) {
        return 0;
    } else {
        return -1;
    }
}

kal_int32 wlan_sta_get_power_saving(PARAM_POWER_MODE *pm)
{

    kal_int32 status = wndrv_get_sta_power_saving(pm);

    if (status == 0) {
        return 0;
    } else {
        return -1;
    }
}


void wlan_sta_get_scan_result(char *scan_buf)
{
    WLAN_SCAN_RESULT *list;
    WLAN_BSS_INFO *bi;
    int32_t i = 0;

    list = (WLAN_SCAN_RESULT *) scan_buf;

    if (list->count > 0) {
        bi = list->bss_info;
        for (i = 0; i < list->count; i++, bi = (WLAN_BSS_INFO *)((uint8_t *)bi + bi->length)) {
            /* SSID */
            printc("SSID = %s\n", bi->SSID);
            printc("SSID_len = %d\n", bi->SSID_len);

            /* BSSID */
            printc("BSSID: %02x:%02x:%02x:%02x:%02x:%02x, ",
                   bi->BSSID[0], bi->BSSID[1], bi->BSSID[2], bi->BSSID[3], bi->BSSID[4], bi->BSSID[5]);

            /* CHANNEL */
            //printc("bi->chanspec = %d\n", bi->chanspec);
            printc("Ch: %d, ", WLAN_CHSPEC_CHANNEL(bi->chanspec));

            /* RSSI */
            printc("RSSI: %ddBm, ", (int16_t)(bi->RSSI));
        }
    }/*end if(list->count > 0)*/
}

char *auth_mode_table[WLM_AUTH_MAX] = {"DISABLED",
                                       "IBSS NONE",
                                       "WPA_PSK",
                                       "WPA2_PSK",
                                       "WPA/WPA2_PSK",
                                       "WPA_ENTERPRISE",
                                       "WPA2_ENTERPRISE",
                                       "WPA/WPA2_ENTERPRISE",
                                       "WAPI_PSK",
                                       "WAPI_WAI"
                                      };


char *encrytion_type_table[WLM_ENCRYPT_MAX] = {"NONE",
                                               "WEP",
                                               "TKIP",
                                               "AES",
                                               "TKIP/AES",
                                               "WSEC",
                                               "FIPS"
                                              };

char *psmode_type_table[Param_PowerModeMax] = {"CAM",
                                               "MAX_PSP",
                                               "Fast_PSP"
                                              };



char *look_for_encrytion_type(WLM_ENCRYPTION encryption)
{
    configASSERT(encryption < WLM_ENCRYPT_MAX)
    return encrytion_type_table[encryption];

}

char *look_for_auth_mode(WLM_AUTH_MODE authMode)
{
    configASSERT(authMode < WLM_AUTH_MAX)
    return auth_mode_table[authMode];

}

char *look_for_ps_mode(PARAM_POWER_MODE psmode)
{
    configASSERT(psmode < Param_PowerModeMax)
    return psmode_type_table[psmode];

}



int32_t wifi_init_done_announce(wifi_event_t event, uint8_t *payload, uint32_t length)
{
    if (!(wlan_status & LWIP_CREATED)) {
        wlan_status |= LWIP_CREATED;
    }

    atci_cmd_wifi_ready_announce();
    return 0;
}

kal_uint8 BtoH(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return (ch - '0');    /* Handle numerals*/
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 0xA);    /* Handle capitol hex digits*/
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 0xA);    /* Handle small hex digits*/
    }
    return (255);
}

void AtoH(char *src, char *dest, int destlen)
{
    char *srcptr;
    char *destTemp;

    srcptr = src;
    destTemp = (char *) dest;

    while (destlen--) {
        *destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
        *destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
        destTemp++;
    }
}

int32_t wifi_is_mac_address_valid(uint8_t *mac_addr)
{
    uint32_t byte_sum = 0;
    for (uint32_t index = 0; index < WIFI_MAC_ADDRESS_LENGTH; index++) {
        byte_sum += mac_addr[index];
    }
    return (byte_sum != 0);
}

int32_t wifi_get_mac_addr_from_efuse(uint8_t port, uint8_t *mac_addr)
{
    return -1;
}

int32_t wifi_get_mac_addr_from_nvdm(uint8_t port, uint8_t *mac_addr)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t nvdm_ret = 0;
    char buf[128] = {0};
    uint32_t len = sizeof(buf);
    char *group_name = (WIFI_PORT_AP  == port) ? "AP" : "STA";
    //char test_buf[] = "11:22:33:22:11:66";
    //nvdm_ret = nvdm_write_data_item(group_name, "MacAddr", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)test_buf, strlen(test_buf));
    //printc("nvdm_write_data_item status=%d\n",nvdm_ret);

    if (NULL == mac_addr) {
        printc("input is NULL\n");
        return -1;
    }

    nvdm_ret = nvdm_read_data_item(group_name, "MacAddr", (uint8_t *)buf, &len);
    if (nvdm_ret != 0) {
        printc("nvdm read mac fail %d\n", nvdm_ret);
        return -1;
    }

    wifi_conf_get_mac_from_str((char *)mac_addr, (char *)buf);
    if (!wifi_is_mac_address_valid(mac_addr)) {
        return -1;
    }

    printc("Nvdm MAC: %02x:%02x:%02x:%02x:%02x:%02x\n\r", PRINT_MAC(mac_addr));
#else
    printc("NVDM not enable\n");
    return -1;
#endif

    return 0;
}

int32_t wifi_get_mac_addr_from_config(uint8_t port, uint8_t *mac_addr)
{
    if (NULL == mac_addr) {
        printc("input is NULL\n");
        return -1;
    }

    if (0 == osl_ext_read_mac(mac_addr)) {
        if (!wifi_is_mac_address_valid(mac_addr)) {
            printc("wlan_get_mac_addr not valid!!\n");
            return -1;
        }

        if (WIFI_PORT_AP == port) {
            mac_addr[0] |= 0x02;
            memcpy(mt5931_mac_address_ap, mac_addr, sizeof(mt5931_mac_address_ap));
        } else if (WIFI_PORT_STA == port) {
            memcpy(mt5931_mac_address_sta, mac_addr, sizeof(mt5931_mac_address_sta));
        } else {
            printc("wlan_get_mac_addr Error!!\n");
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

void wifi_conf_get_mac_from_str(char *mac_dst, const char *mac_src)
{
    unsigned int len = strlen(mac_src);

    if (len == 17) {
        //Mac address acceptable format 01:02:03:04:05:06 length 17
        int     j;
        char    *p = (char *)mac_src;
        for (j = 0; j < 6; j++) {
            AtoH(p, &mac_dst[j], 1);
            p += 3;
        }
    } else {
        printc("string format is wrong.. %s", mac_src);
    }
}

