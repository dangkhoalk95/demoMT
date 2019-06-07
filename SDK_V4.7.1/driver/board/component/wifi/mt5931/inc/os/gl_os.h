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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/os/linux/include/gl_os.h#43 $
*/

/*! \file   gl_os.h
    \brief  List the external reference to OS for GLUE Layer.

    In this file we define the data structure - GLUE_INFO_T to store those objects
    we acquired from OS - e.g. TIMER, SPINLOCK, NET DEVICE ... . And all the
    external reference (header file, extern func() ..) to OS for GLUE Layer should
    also list down here.
*/




#ifndef _GL_OS_H
#define _GL_OS_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/
/*------------------------------------------------------------------------------
 * Flags for LINUX(OS) dependent
 *------------------------------------------------------------------------------
 */
#define CFG_MAX_WLAN_DEVICES                1 /* number of wlan card will coexist*/

#define CFG_MAX_TXQ_NUM                     4 /* number of tx queue for support multi-queue h/w  */


#define CFG_USE_SPIN_LOCK_BOTTOM_HALF       0 /* 1: Enable use of SPIN LOCK Bottom Half for LINUX
                                                 0: Disable - use SPIN LOCK IRQ SAVE instead */

#define CFG_TX_PADDING_SMALL_ETH_PACKET     0 /* 1: Enable - Drop ethernet packet if it < 14 bytes.
                                                             And pad ethernet packet with dummy 0 if it < 60 bytes.
                                                 0: Disable */

#define CFG_TX_STOP_NETIF_QUEUE_THRESHOLD   256 /* packets */

#define CFG_TX_STOP_NETIF_PER_QUEUE_THRESHOLD   64  /* packets */
#define CFG_TX_START_NETIF_PER_QUEUE_THRESHOLD  32  /* packets */


#define ETH_P_1X                            0x888E
#define IPTOS_PREC_OFFSET                   5
#define USER_PRIORITY_DEFAULT               0

#define ETH_WPI_1X                         0x88B4

#define ETH_HLEN                                14
#define ETH_TYPE_LEN_OFFSET                     12
#define ETH_P_IP                                0x0800
#define ETH_P_1X                                0x888E
#define ETH_P_PRE_1X                            0x88C7

#define IPVERSION                               4
#define IP_HEADER_LEN                           20

#define IPVH_VERSION_OFFSET                     4 // For Little-Endian
#define IPVH_VERSION_MASK                       0xF0
#define IPTOS_PREC_OFFSET                       5
#define IPTOS_PREC_MASK                         0xE0

#define SOURCE_PORT_LEN                         2
/* NOTE(Kevin): Without IP Option Length */
#define LOOK_AHEAD_LEN                          (ETH_HLEN + IP_HEADER_LEN + SOURCE_PORT_LEN)

/* 802.2 LLC/SNAP */
#define ETH_LLC_OFFSET                          (ETH_HLEN)
#define ETH_LLC_LEN                             3
#define ETH_LLC_DSAP_SNAP                       0xAA
#define ETH_LLC_SSAP_SNAP                       0xAA
#define ETH_LLC_CONTROL_UNNUMBERED_INFORMATION  0x03

/* Bluetooth SNAP */
#define ETH_SNAP_OFFSET                         (ETH_HLEN + ETH_LLC_LEN)
#define ETH_SNAP_LEN                            5
#define ETH_SNAP_BT_SIG_OUI_0                   0x00
#define ETH_SNAP_BT_SIG_OUI_1                   0x19
#define ETH_SNAP_BT_SIG_OUI_2                   0x58

#define BOW_PROTOCOL_ID_SECURITY_FRAME          0x0003


#define DRV_NAME    "[MT6620]: "

#define WNDRV_ADD_KEY_FAIL_TIMEOUT              (KAL_TICKS_1_SEC * 120)

#define WNDRV_KEEP_CAM_TIMEOUT              (KAL_TICKS_1_SEC * 120)
/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#if 0

#include <linux/version.h>      /* constant of kernel version */

#include <linux/kernel.h>       /* bitops.h */

#include <linux/timer.h>        /* struct timer_list */
#include <linux/jiffies.h>      /* jiffies */
#include <linux/delay.h>        /* udelay and mdelay macro */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 12)
#include <linux/irq.h>          /* IRQT_FALLING */
#endif

#include <linux/netdevice.h>    /* struct net_device, struct net_device_stats */
#include <linux/etherdevice.h>  /* for eth_type_trans() function */
#include <linux/wireless.h>     /* struct iw_statistics */
#include <linux/if_arp.h>
#include <linux/inetdevice.h>   /* struct in_device */

#include <linux/ip.h>           /* struct iphdr */

#include <linux/string.h>       /* for memcpy()/memset() function */
#include <linux/stddef.h>       /* for offsetof() macro */

#include <linux/proc_fs.h>      /* The proc filesystem constants/structures */

#include <linux/rtnetlink.h>    /* for rtnl_lock() and rtnl_unlock() */
#include <linux/kthread.h>      /* kthread_should_stop(), kthread_run() */
#include <asm/uaccess.h>        /* for copy_from_user() */
#include <linux/fs.h>           /* for firmware download */
#include <linux/vmalloc.h>

#include <linux/kfifo.h>        /* for kfifo interface */
#include <linux/cdev.h>         /* for cdev interface */

#include <linux/firmware.h>     /* for firmware download */

#if defined(_HIF_SDIO)
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_func.h>
#endif

#include <linux/random.h>

#include <asm/io.h>             /* readw and writew */

#if WIRELESS_EXT > 12
#include <net/iw_handler.h>
#endif

#endif

#include "version.h"
#include "wlan_config.h"

#if CFG_ENABLE_WIFI_DIRECT_CFG_80211
#include <net/wireless.h>
#include <net/cfg80211.h>
#endif

#include "gl_typedef.h"
#include "typedef.h"
#include "wlan_queue.h"
#if CFG_CHIP_RESET_SUPPORT
#include "gl_rst.h"
#endif
//#include "hif.h"


#include "debug.h"

#include "mac.h"
#include "wlan_def.h"
#include "wlan_lib.h"
#include "wlan_oid.h"
#include "gl_kal.h"

extern bool fgIsBusAccessFailed;

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define GLUE_FLAG_HALT          BIT(0)
#define GLUE_FLAG_INT           BIT(1)
#define GLUE_FLAG_OID           BIT(2)
#define GLUE_FLAG_TIMEOUT       BIT(3)
#define GLUE_FLAG_TXREQ         BIT(4)
#define GLUE_FLAG_SUB_MOD_INIT  BIT(5)
#define GLUE_FLAG_SUB_MOD_EXIT  BIT(6)
#define GLUE_FLAG_HALT_BIT          (0)
#define GLUE_FLAG_INT_BIT           (1)
#define GLUE_FLAG_OID_BIT           (2)
#define GLUE_FLAG_TIMEOUT_BIT       (3)
#define GLUE_FLAG_TXREQ_BIT         (4)
#define GLUE_FLAG_SUB_MOD_INIT_BIT  (5)
#define GLUE_FLAG_SUB_MOD_EXIT_BIT  (6)


#define GLUE_BOW_KFIFO_DEPTH        (1024)
//#define GLUE_BOW_DEVICE_NAME        "MT6620 802.11 AMP"
#define GLUE_BOW_DEVICE_NAME        "ampc0"


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/* need to update eParamWndrvEs if any new ES is added */
typedef enum _ENUM_PARAM_WNDRV_ES {
    PARAM_WNDRV_ES_TEST_TIMER = 0,
#if 1
    PARAM_WNDRV_ES_SCAN_COMPLETE_TIMER,
    PARAM_WNDRV_ES_CONNETION_FAIL_TIMER,
    PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER,
    PARAM_WNDRV_ES_STAT_IND_TIMER,
    PARAM_WNDRV_ES_PS_CTL_TIMER,
#endif
    //PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER,
    PARAM_WNDRV_ES_KEEP_CAM_TIMER,
    /* MT5931 module timers */
    PARAM_WNDRV_ES_CNM_TIMER,
    PARAM_WNDRV_ES_PWR_CTRL_TIMR,
    PARAM_WNDRV_ES_BEACON_TIMR,
    PARAM_WNDRV_ES_SMART_CONFIG_TIMER,
    PARAM_WNDRV_ES_NUM
} ENUM_PARAM_WNDRV_ES_T, *P_ENUM_PARAM_WNDRV_ES;

typedef struct _GL_WPA_INFO_T {
    UINT_32 u4WpaVersion;
    UINT_32 u4KeyMgmt;
    UINT_32 u4CipherGroup;
    UINT_32 u4CipherPairwise;
    UINT_32 u4AuthAlg;
    BOOLEAN fgPrivacyInvoke;
} GL_WPA_INFO_T, *P_GL_WPA_INFO_T;

typedef enum _ENUM_RSSI_TRIGGER_TYPE {
    ENUM_RSSI_TRIGGER_NONE,
    ENUM_RSSI_TRIGGER_GREATER,
    ENUM_RSSI_TRIGGER_LESS,
    ENUM_RSSI_TRIGGER_TRIGGERED,
    ENUM_RSSI_TRIGGER_NUM
} ENUM_RSSI_TRIGGER_TYPE;

#if CFG_ENABLE_WIFI_DIRECT
typedef enum _ENUM_SUB_MODULE_IDX_T {
    P2P_MODULE = 0,
    SUB_MODULE_NUM
} ENUM_SUB_MODULE_IDX_T;
#endif

typedef struct _GL_IO_REQ_T {
    QUE_ENTRY_T             rQueEntry;
    //wait_queue_head_t       cmdwait_q;
    BOOL                    fgRead;
    BOOL                    fgWaitResp;
#if CFG_ENABLE_WIFI_DIRECT
    BOOL                    fgIsP2pOid;
#endif
    P_ADAPTER_T             prAdapter;
    PFN_OID_HANDLER_FUNC    pfnOidHandler;
    PVOID                   pvInfoBuf;
    UINT_32                 u4InfoBufLen;
    PUINT_32                pu4QryInfoLen;
    WLAN_STATUS             rStatus;
    UINT_32                 u4Flag;
} GL_IO_REQ_T, *P_GL_IO_REQ_T;

/* wifi_driver_customer_setting[] */
typedef enum _ENUM_PARAM_CUST_SET {
    PARAM_CUST_SET_REG_DOMAIN = 0,              /* 0. Country code */
    PARAM_CUST_SET_PS_MODE,                     /* 1. Power saving mode */
    PARAM_CUST_SET_OSC_STABLE_TIME,             /* 2. OSC stable time */
    PARAM_CUST_SET_WMMPS_CONFIG,                /* 3. WMM PS config */
    PARAM_CUST_SET_EINT_POLARITY,               /* 4. WIFI EINT polarity */
    PARAM_CUST_SET_LOWEST_RSSI,                 /* 5. RSSI Low threshold */
    PARAM_CUST_SET_CONNECTION_KEEP_TIME,        /* 6. Connection keep time */
    PARAM_CUST_SET_CONNECTION_RETRY_LIMIT,      /* 7. Connection retry limit */
    PARAM_CUST_SET_DTIM_PERIOD,                 /* 8. Sleep Period */
    PARAM_CUST_SET_BT_1WIRE_MODE_T6,            /* 9. 1 wire mode T6 */
    PARAM_CUST_SET_BT_1WIRE_MODE_T8,            /* 10. 1 wire mode T8 */
    PARAM_CUST_SET_BT_1WIRE_MODE_BT_DELAY,      /* 11. 1 wire mode BT delay */
    PARAM_CUST_SET_DCXO_CONFIG,                 /* 12. DCXO config */
    PARAM_CUST_SET_DEC_PWR,                     /* 13. dec power*/
    PARAM_CUST_SET_PWR_CTRL_TIMEOUT,            /* 14. polling time*/
    PARAM_CUST_SET_SDIO_SEL_CARD,               /* 15. SDIO card select */
    PARAM_CUST_SET_SDIO_CLK_FREQ,               /* 16. SDIO clock frequency */
    PARAM_CUST_SET_FUNC1_SDIO_BLK_SIZE,         /* 17. SDIO WIFI function block size */
    PARAM_CUST_SET_MAX_NUM
} ENUM_PARAM_CUST_SET_T, *P_ENUM_PARAM_CUST_SET_T;


#if CFG_ENABLE_BT_OVER_WIFI
typedef struct _GL_BOW_INFO {
    BOOLEAN                 fgIsRegistered;
    dev_t                   u4DeviceNumber; /* dynamic device number */
//    struct kfifo            *prKfifo;       /* for buffering indicated events */
    struct kfifo                      rKfifo;   /* for buffering indicated events */
    spinlock_t              rSpinLock;      /* spin lock for kfifo */
    struct cdev             cdev;
    UINT_32                 u4FreqInKHz;    /* frequency */

    UINT_8                  aucRole[CFG_BOW_PHYSICAL_LINK_NUM];  /* 0: Responder, 1: Initiator */
    ENUM_BOW_DEVICE_STATE   aeState[CFG_BOW_PHYSICAL_LINK_NUM];
    PARAM_MAC_ADDRESS       arPeerAddr[CFG_BOW_PHYSICAL_LINK_NUM];

    wait_queue_head_t       outq;

#if CFG_BOW_SEPARATE_DATA_PATH
    /* Device handle */
    struct net_device           *prDevHandler;
    BOOLEAN                     fgIsNetRegistered;
#endif

} GL_BOW_INFO, *P_GL_BOW_INFO;
#endif

#if CFG_ENABLE_WIFI_TETHERING
typedef struct _GL_WT_INFO {
    BOOLEAN                 fgIsTetheringEnable;
    BOOLEAN                 fgIsPrivacy; /* OPEN: FALSE WPA2-PSK: TRUE */
    BOOLEAN                 fgIsInit;
    UINT_8                  ucSeqNumOfChReq;
    UINT_8                  GTKkeyBuffer[sizeof(PARAM_KEY_T) + 16/*CCMP_KEY_LEN*/];
    UINT_8                  ucPrimaryChannel;
    BOOLEAN                 fgIsGTKExist;
} GL_WT_INFO, *P_GL_WT_INFO;
#endif /* CFG_ENABLE_WIFI_TETHERING */


typedef struct _GL_HIF_INFO_T {
    void            *mcr_addr_base;
    void            *mcr_data_base;
    struct semaphore        *lock;
    BOOLEAN                 fgIntReadClear;
    BOOLEAN                 fgMbxReadClear;
} GL_HIF_INFO_T, *P_GL_HIF_INFO_T;

/*
* type definition of pointer to p2p structure
*/
typedef struct _GL_P2P_INFO_T   GL_P2P_INFO_T, *P_GL_P2P_INFO_T;

struct _GLUE_INFO_T {
    /* Device handle */
    struct net_device *prDevHandler;

    /* Device Index(index of arWlanDevInfo[]) */
    INT_32 i4DevIdx;

    /* Device statistics */
//    struct net_device_stats rNetDevStats;

    /* Wireless statistics struct net_device */
//    struct iw_statistics rIwStats;

    /* spinlock to sync power save mechanism */
    UINT_32 rSpinLock[SPIN_LOCK_NUM];

    /* semaphore for ioctl */
//    struct semaphore ioctl_sem;

    UINT_32 u4Flag; /* GLUE_FLAG_XXX */
    UINT_32 u4PendFlag;
    UINT_32 u4TimeoutFlag;
    UINT_32 u4ReadyFlag;  /* check if card is ready */

    /* Number of pending frames, also used for debuging if any frame is
     * missing during the process of unloading Driver.
     *
     * NOTE(Kevin): In Linux, we also use this variable as the threshold
     * for manipulating the netif_stop(wake)_queue() func.
     */
    INT_32             ai4TxPendingFrameNumPerQueue[4][CFG_MAX_TXQ_NUM];
    INT_32             i4TxPendingFrameNum;
    INT_32             i4TxPendingSecurityFrameNum;

    /* current IO request for kalIoctl */
    GL_IO_REQ_T         OidEntry;

    /* registry info*/
    REG_INFO_T rRegInfo;

    /* firmware */
    struct firmware     *prFw;

    /* Host interface related information */
    /* defined in related hif header file */
    GL_HIF_INFO_T       rHifInfo;

    /*! \brief wext wpa related information */
    GL_WPA_INFO_T       rWpaInfo;


    /* Pointer to ADAPTER_T - main data structure of internal protocol stack */
    P_ADAPTER_T         prAdapter;

#ifdef WLAN_INCLUDE_PROC
    struct proc_dir_entry *pProcRoot;
#endif /* WLAN_INCLUDE_PROC */

    /* Indicated media state */
    ENUM_PARAM_MEDIA_STATE_T eParamMediaStateIndicated;

    /* Device power state D0~D3 */
    PARAM_DEVICE_POWER_STATE ePowerState;

//    struct completion rScanComp; /* indicate scan complete */
//    struct completion rHaltComp; /* indicate main thread halt complete */
//    struct completion rPendComp; /* indicate main thread halt complete */
#if CFG_ENABLE_WIFI_DIRECT
    struct completion rSubModComp; /*indicate sub module init or exit complete*/
#endif
    WLAN_STATUS             rPendStatus;

    QUE_T                   rTxQueue;


    /* OID related */
    QUE_T                   rCmdQueue;
    //PVOID                   pvInformationBuffer;
    //UINT_32                 u4InformationBufferLength;
    //PVOID                   pvOidEntry;
    //PUINT_8                 pucIOReqBuff;
    //QUE_T                   rIOReqQueue;
    //QUE_T                   rFreeIOReqQueue;

//    wait_queue_head_t       waitq;
    struct task_struct      *main_thread;

//    struct timer_list tickfn;


#if CFG_SUPPORT_EXT_CONFIG
    UINT_16     au2ExtCfg[256];  /* NVRAM data buffer */
    UINT_32     u4ExtCfgLength;  /* 0 means data is NOT valid */
#endif

#if 1//CFG_SUPPORT_WAPI
    /* Should be large than the PARAM_WAPI_ASSOC_INFO_T */
    UINT_8                  aucWapiAssocInfoIEs[42];
    UINT_16                 u2WapiAssocInfoIESz;
#endif

#if CFG_ENABLE_BT_OVER_WIFI
    GL_BOW_INFO             rBowInfo;
#endif

#if CFG_ENABLE_WIFI_DIRECT
    P_GL_P2P_INFO_T         prP2PInfo;
#endif
    BOOLEAN                 fgWpsActive;
    UINT_8                  aucWSCIE[500]; /*for probe req*/
    UINT_16                 u2WSCIELen;
    UINT_8                  aucWSCAssocInfoIE[200]; /*for Assoc req*/
    UINT_16                 u2WSCAssocInfoIELen;

    SET_TXPWR_CTRL_T        rTxPwr;

    /* NVRAM availability */
    BOOLEAN                 fgNvramAvailable;

    BOOLEAN                 fgMcrAccessAllowed;

    /* MAC Address Overriden by IOCTL */
    BOOLEAN                 fgIsMacAddrOverride;
    PARAM_MAC_ADDRESS       rMacAddrOverride;

    /* For 5911 wndrv_context struct */
    stack_timer_struct      timer;
    stack_timer_struct      evsched_timer;
    eventid                 event_id[PARAM_WNDRV_ES_NUM];
    event_scheduler         *event_scheduler_ptr;
    kal_int32               tx_sys_peer_num;
    kal_int32               tx_flc_peer_num;
    kal_int32               rx_sys_peer_num;
    kal_int32               rx_flc_peer_num;
    kal_int32               mem_allc_size;
    kal_bool                meta_mode;
    kal_bool                meta_inject_mode;

    /* This is to indicate if we have called wlanAdapterStop after adapter is inited*/
    BOOLEAN fgIsAdaptStopped;

    /* Select EINT polarity */
    kal_bool           fgEintPolarity;

    /* For event scheduler */
    P_ENUM_PARAM_WNDRV_ES  prParamWndrvEs;

    /* For SCAN specific SSID */
    BOOLEAN             fgIsReportScanResult;         /* If we need to ndicate site survey result */

    /* Record the securtiy parameters we set in the association request */
    UINT_32            assoc_ie_len;
    UINT_8             assoc_ie[WNDRV_MAX_ASSOC_IE_LEN];

    /* For WMM PS test setting */
    PARAM_CUSTOM_WMM_PS_TEST_STRUC_T UAPSDSetting;

    /* TX target power control (unit:dbm) */
    INT_8              c11b_tx_pwr;
    INT_8              c11g_tx_pwr;
    INT_8              c11n_bw20_tx_pwr;
    INT_8              c11n_bw40_tx_pwr;

    /* Band Edge TX target power control (unit:dbm) */
    INT_8              cCck_band_edge_tx_pwr;
    INT_8              cOfdm_band_edge_tx_pwr;
    INT_8              cOfdm_bw20_band_edge_tx_pwr;
    INT_8              cOfdm_bw40_band_edge_tx_pwr;

    /* TX target channel power offset */
    INT_8              cCh_offset_L;
    INT_8              cCh_offset_M;
    INT_8              cCh_offset_H;

    /* TX frequency offset (unit:db) */
    INT_8              cFreq_offset;
    INT_8              cCystal_trim;

    kal_bool                engineer_mode;

    /* customer definitions */
    INT_32          as4Cust_config[PARAM_CUST_SET_MAX_NUM];
#if CFG_ENABLE_WIFI_TETHERING
    GL_WT_INFO       rWtInfo;
#endif /* CFG_ENABLE_WIFI_TETHERING */
#if CFG_TCP_IP_FLOW_CTRL
    BOOLEAN            fgPending_Ctrl;
    BOOLEAN            fgPending_Frame0;
#endif

    /* Channel setting mode selection:
     * 0=By Country Code,
     * 1=By Regulatory Channel List Index,
     * 2=By Regulatory SubBand Info field
     */
    UINT_8             ucCh_set_mode;
};

//#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
/* linux 2.4 */
//    typedef void (*PFN_WLANISR)(int irq, void *dev_id, struct pt_regs *regs);
//#else
//    typedef irqreturn_t (*PFN_WLANISR)(int irq, void *dev_id, struct pt_regs *regs);
//#endif

typedef void (*PFN_LINUX_TIMER_FUNC)(unsigned long);


/* generic sub module init/exit handler
*   now, we only have one sub module, p2p
*/
#if CFG_ENABLE_WIFI_DIRECT
typedef BOOLEAN(*SUB_MODULE_INIT)(P_GLUE_INFO_T prGlueInfo);
typedef BOOLEAN(*SUB_MODULE_EXIT)(P_GLUE_INFO_T prGlueInfo);

typedef struct _SUB_MODULE_HANDLER {
    SUB_MODULE_INIT subModInit;
    SUB_MODULE_EXIT subModExit;
    BOOLEAN fgIsInited;
} SUB_MODULE_HANDLER, *P_SUB_MODULE_HANDLER;

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
/*----------------------------------------------------------------------------*/
/* Macros of SPIN LOCK operations for using in Glue Layer                     */
/*----------------------------------------------------------------------------*/
#if defined(WCN_MAUI)
#define GLUE_SPIN_LOCK_DECLARATION()
#define GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, rLockCategory)
#define GLUE_RELEASE_SPIN_LOCK(prGlueInfo, rLockCategory)
#else
#if CFG_USE_SPIN_LOCK_BOTTOM_HALF
#define GLUE_SPIN_LOCK_DECLARATION()
#define GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
                { \
                    if (rLockCategory < SPIN_LOCK_NUM) \
                    spin_lock_bh(&(prGlueInfo->rSpinLock[rLockCategory])); \
                }
#define GLUE_RELEASE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
                { \
                    if (rLockCategory < SPIN_LOCK_NUM) \
                    spin_unlock_bh(&(prGlueInfo->rSpinLock[rLockCategory])); \
                }
#else /* !CFG_USE_SPIN_LOCK_BOTTOM_HALF */
#define GLUE_SPIN_LOCK_DECLARATION()                        UINT_32 __u4Flags = 0
#define GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
                { \
                    if (rLockCategory < SPIN_LOCK_NUM) \
                    spin_lock_irqsave(&(prGlueInfo)->rSpinLock[rLockCategory], __u4Flags); \
                }
#define GLUE_RELEASE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
                { \
                    if (rLockCategory < SPIN_LOCK_NUM) \
                    spin_unlock_irqrestore(&(prGlueInfo->rSpinLock[rLockCategory]), __u4Flags); \
                }
#endif /* !CFG_USE_SPIN_LOCK_BOTTOM_HALF */
#endif

/*----------------------------------------------------------------------------*/
/* Macros for accessing Reserved Fields of native packet                      */
/*----------------------------------------------------------------------------*/
#if defined(WCN_MAUI)
#define GLUE_GET_PKT_QUEUE_ENTRY(_p)    \
            (&(((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prQueueEntry ))

#define GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)  \
            ((P_NATIVE_PACKET)((wndrv_pkt_descriptor_struct*) (_prQueueEntry))->prPacketInfo->prPacket)

#define  GLUE_SET_PKT_FLAG_802_11(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIs802_11=TRUE)
#define GLUE_SET_PKT_FLAG_1X(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIs1x=TRUE)
#define GLUE_SET_PKT_FLAG_PAL(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIsPAL=TRUE)
#define GLUE_SET_PKT_FLAG_P2P(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIsP2P=TRUE)
#define GLUE_SET_PKT_TID(_p, _tid) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->ucTID=(UINT_8)_tid)
#define GLUE_SET_PKT_FRAME_LEN(_p, _u2PayloadLen)
#define GLUE_GET_PKT_FRAME_LEN(_p) \
   (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->ucMacHeaderLength+ \
   ((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->u2PayloadLength)
#define  GLUE_GET_PKT_IS_802_11(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIs802_11)
#define  GLUE_GET_PKT_IS_1X(_p)\
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIs1x)
#define GLUE_GET_PKT_TID(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->ucTID)
#define GLUE_GET_PKT_IS_PAL(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIsPAL)
#define GLUE_GET_PKT_IS_P2P(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->fgIsP2P)
#define GLUE_SET_PKT_HEADER_LEN(_p, _ucMacHeaderLen) \
  (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->ucMacHeaderLength= \
    (UINT_8)_ucMacHeaderLen)
#define GLUE_GET_PKT_HEADER_LEN(_p) \
  (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->prPacketInfo->ucMacHeaderLength)
#define GLUE_SET_PKT_ARRIVAL_TIME(_p, _rSysTime) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->sys_time = (kal_uint32)_rSysTime)
#define GLUE_GET_PKT_ARRIVAL_TIME(_p) \
    (((wndrv_pkt_descriptor_struct*)wndrv_get_pkt_descriptor((peer_buff_struct *)_p))->sys_time)

/* Check validity of prDev, private data, and pointers */
#define GLUE_CHK_DEV(prDev)
#define GLUE_CHK_PR2(prDev, pr2)
#define GLUE_CHK_PR3(prDev, pr2, pr3)
#define GLUE_CHK_PR4(prDev, pr2, pr3, pr4)
#else
#define GLUE_GET_PKT_QUEUE_ENTRY(_p)    \
            (&( ((struct sk_buff *)(_p))->cb[0] ))

#define GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)  \
            ((P_NATIVE_PACKET) ((UINT_32)_prQueueEntry - offsetof(struct sk_buff, cb[0])) )

#define  GLUE_SET_PKT_FLAG_802_11(_p)  \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) |= BIT(7))

#define GLUE_SET_PKT_FLAG_1X(_p)  \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) |= BIT(6))

#define GLUE_SET_PKT_FLAG_PAL(_p)  \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) |= BIT(5))

#define GLUE_SET_PKT_FLAG_P2P(_p)  \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) |= BIT(4))



#define GLUE_SET_PKT_TID(_p, _tid)  \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) |= (((UINT_8)((_tid) & (BITS(0,3))))))


#define GLUE_SET_PKT_FRAME_LEN(_p, _u2PayloadLen) \
            (*((PUINT_16) &( ((struct sk_buff *)(_p))->cb[6] )) = (UINT_16)(_u2PayloadLen))

#define GLUE_GET_PKT_FRAME_LEN(_p)    \
            (*((PUINT_16) &( ((struct sk_buff *)(_p))->cb[6] )) )


#define  GLUE_GET_PKT_IS_802_11(_p)        \
            ((*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) ) & (BIT(7)))

#define  GLUE_GET_PKT_IS_1X(_p)        \
            ((*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) ) & (BIT(6)))

#define GLUE_GET_PKT_TID(_p)        \
            ((*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) ) & (BITS(0,3)))


#define GLUE_GET_PKT_IS_PAL(_p)        \
            ((*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) ) & (BIT(5)))

#define GLUE_GET_PKT_IS_P2P(_p)        \
            ((*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[4] )) ) & (BIT(4)))


#define GLUE_SET_PKT_HEADER_LEN(_p, _ucMacHeaderLen)    \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[5] )) = (UINT_8)(_ucMacHeaderLen))

#define GLUE_GET_PKT_HEADER_LEN(_p) \
            (*((PUINT_8) &( ((struct sk_buff *)(_p))->cb[5] )) )

#define GLUE_SET_PKT_ARRIVAL_TIME(_p, _rSysTime) \
            (*((POS_SYSTIME) &( ((struct sk_buff *)(_p))->cb[8] )) = (OS_SYSTIME)(_rSysTime))

#define GLUE_GET_PKT_ARRIVAL_TIME(_p)    \
            (*((POS_SYSTIME) &( ((struct sk_buff *)(_p))->cb[8] )) )

/* Check validity of prDev, private data, and pointers */
#define GLUE_CHK_DEV(prDev) \
    ((prDev && netdev_priv(prDev)) ? TRUE : FALSE)

#define GLUE_CHK_PR2(prDev, pr2) \
    ((GLUE_CHK_DEV(prDev) && pr2) ? TRUE : FALSE)

#define GLUE_CHK_PR3(prDev, pr2, pr3) \
    ((GLUE_CHK_PR2(prDev, pr2) && pr3) ? TRUE : FALSE)

#define GLUE_CHK_PR4(prDev, pr2, pr3, pr4) \
    ((GLUE_CHK_PR3(prDev, pr2, pr3) && pr4) ? TRUE : FALSE)
#endif

#define GLUE_SET_EVENT(pr) \
    kalSetEvent(pr)

#if 1
#define GLUE_INC_REF_CNT(_refCount)     (_refCount++)
#define GLUE_DEC_REF_CNT(_refCount)     (_refCount--)
#else
#define GLUE_INC_REF_CNT(_refCount)     atomic_inc((atomic_t *)&(_refCount))
#define GLUE_DEC_REF_CNT(_refCount)     atomic_dec((atomic_t *)&(_refCount))
#endif

#define DbgPrint    kalPrint
/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
#ifdef WLAN_INCLUDE_PROC
INT_32
procRemoveProcfs(
    struct net_device *prDev,
    char *pucDevName
);

INT_32
procInitProcfs(
    struct net_device *prDev,
    char *pucDevName
);
#endif /* WLAN_INCLUDE_PROC */

#if CFG_ENABLE_BT_OVER_WIFI
BOOLEAN
glRegisterAmpc(
    P_GLUE_INFO_T prGlueInfo
);

BOOLEAN
glUnregisterAmpc(
    P_GLUE_INFO_T prGlueInfo
);
#endif

#if CFG_ENABLE_WIFI_DIRECT

VOID
wlanSubModRunInit(
    P_GLUE_INFO_T prGlueInfo
);

VOID
wlanSubModRunExit(
    P_GLUE_INFO_T prGlueInfo
);

BOOLEAN
wlanSubModInit(
    P_GLUE_INFO_T prGlueInfo
);

BOOLEAN
wlanSubModExit(
    P_GLUE_INFO_T prGlueInfo
);

VOID
wlanSubModRegisterInitExit(
    SUB_MODULE_INIT rSubModInit,
    SUB_MODULE_EXIT rSubModExit,
    ENUM_SUB_MODULE_IDX_T eSubModIdx
);

BOOLEAN
wlanExportGlueInfo(
    P_GLUE_INFO_T *prGlueInfoExpAddr
);

BOOLEAN
wlanIsLaunched(
    VOID
);

#endif

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _GL_OS_H */

