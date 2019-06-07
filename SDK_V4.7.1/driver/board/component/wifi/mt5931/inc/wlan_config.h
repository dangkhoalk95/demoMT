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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/config.h#81 $
*/

/*! \file   "config.h"
    \brief  This file includes the various configurable parameters for customers

    This file ncludes the configurable parameters except the paramters indicate the turning-on/off of some features
*/




#ifndef _WLAN_CONFIG_H
#define _WLAN_CONFIG_H
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
//2 Flags for OS capability

#ifdef LINUX
#ifdef CONFIG_X86
#define MTK_WCN_HIF_SDIO        0
#else
#define MTK_WCN_HIF_SDIO        1
#endif
#else
#define MTK_WCN_HIF_SDIO            0
#endif

#define RESEVED_BUFFER_FOR_TX_PAD       8

//2 Flags for Driver Features
#define CFG_TX_FRAGMENT                         0 /*!< 1: Enable TX fragmentation
                                                       0: Disable */
#define CFG_SUPPORT_PERFORMANCE_TEST          0  /*Only for performance Test*/

#define CFG_COUNTRY_CODE                        NULL //"US"

#ifndef LINUX
#define CFG_FW_FILENAME                         L"WIFI_RAM_CODE"
#else
#define CFG_FW_FILENAME                         "WIFI_RAM_CODE"
#endif

#define CFG_ENABLE_THROUGHPUT_DBG 0

#define CFG_SUPPORT_802_11D                     0 /*!< 1(default): Enable 802.11d
                                                     0: Disable */

#define CFG_SUPPORT_SPEC_MGMT       0   /* Spectrum Management (802.11h): TPC and DFS */
#define CFG_SUPPORT_RRM             0   /* Radio Reasource Measurement (802.11k) */
#define CFG_SUPPORT_QUIET           0   /* Quiet (802.11h) */


#define CFG_SUPPORT_RX_RDG          0   /* 11n feature. RX RDG capability */
#define CFG_SUPPORT_MFB             0   /* 802.11n MCS Feedback responder */
#define CFG_SUPPORT_RX_STBC         1   /* 802.11n RX STBC (1SS) */
#define CFG_SUPPORT_RX_SGI          1   /* 802.11n RX short GI for both 20M and 40M BW */
#define CFG_SUPPORT_RX_HT_GF        1   /* 802.11n RX HT green-field capability */
/*------------------------------------------------------------------------------
 * MT5931 TCP TX throughput enhance
 *------------------------------------------------------------------------------
 */
#define CFG_TCP_IP_FLOW_CTRL        0   /* Pending frame control for TCP TX */
#define CFG_DEQUEUE_OPT         1   /* Reduce Dequeue time for TCP TX */

#define CFG_ENABLE_RX_INT_IMPROVE   1
#define CFG_ENABLE_TXRX_PATH_LOG    0
#define CFG_REFINE_REGISTER_ACCESS              1
#define CFG_ENABLE_HIF_ULTRA_CONTRL              0
#define CFG_ENABLE_REG_ACCESS_DBG              0

/*------------------------------------------------------------------------------
 * SLT Option
 *------------------------------------------------------------------------------
 */
#define CFG_SLT_SUPPORT                         0


#ifdef NDIS60_MINIPORT
#define CFG_NATIVE_802_11                       1

#define CFG_TX_MAX_PKT_SIZE                     2304
#define CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60       0 /* !< 1: Enable TCP/IP header checksum offload
                                                            0: Disable */
#define CFG_TCP_IP_CHKSUM_OFFLOAD               0
#define CFG_WHQL_DOT11_STATISTICS               1
#define CFG_WHQL_ADD_REMOVE_KEY                 1
#define CFG_WHQL_CUSTOM_IE                      1
#define CFG_WHQL_SAFE_MODE_ENABLED              1

#else
#define CFG_TCP_IP_CHKSUM_OFFLOAD               0 /* !< 1: Enable TCP/IP header checksum offload
                                                            0: Disable */
#define CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60       0
#define CFG_TX_MAX_PKT_SIZE                     1600
#define CFG_NATIVE_802_11                       0
#endif


//2 Flags for Driver Parameters
/*------------------------------------------------------------------------------
 * Flags for EHPI Interface in Colibri Platform
 *------------------------------------------------------------------------------
 */
#define CFG_EHPI_FASTER_BUS_TIMING                  0 /*!< 1: Do workaround for faster bus timing
                                                           0(default): Disable */

/*------------------------------------------------------------------------------
 * Flags for HIFSYS Interface
 *------------------------------------------------------------------------------
 */
#ifdef _lint
#define _HIF_SDIO   1
#endif

/*------------------------------------------------------------------------------
 * Flags for SDIO Interface
 *------------------------------------------------------------------------------
 */
#if defined(__WIFI_HIF_SDIO__)
#define CFG_SDIO_INTR_ENHANCE                        0 /*!< 1(default): Enable SDIO ISR & TX/RX status enhance mode
                                                           0: Disable */
#define CFG_SDIO_RX_ENHANCE                          0 /*!< 1(default): Enable SDIO ISR & TX/RX status enhance mode
                                                           0: Disable */
#define CFG_SDIO_TX_AGG                              0 /*!< 1: Enable SDIO TX enhance mode(Multiple frames in single BLOCK CMD)
                                                           0(default): Disable */

#define CFG_SDIO_RX_AGG                              0 /*!< 1: Enable SDIO RX enhance mode(Multiple frames in single BLOCK CMD)
                                                               0(default): Disable */
#if (CFG_SDIO_RX_AGG == 1) && (CFG_SDIO_INTR_ENHANCE == 0)
#error "CFG_SDIO_INTR_ENHANCE should be 1 once CFG_SDIO_RX_AGG equals to 1"
#elif (CFG_SDIO_INTR_ENHANCE == 1 || CFG_SDIO_RX_ENHANCE == 1) && (CFG_SDIO_RX_AGG == 0)
#error "CFG_SDIO_RX_AGG should be 1 once CFG_SDIO_INTR_ENHANCE and/or CFG_SDIO_RX_ENHANCE equals to 1"
#endif

#define CFG_SDIO_MAX_RX_AGG_NUM                 0 /*!< 1: Setting the maximum RX aggregation number
                                                               0(default): no limited */

#ifdef WINDOWS_CE
#define CFG_SDIO_PATHRU_MODE                    1 /*!< 1: Suport pass through (PATHRU) mode
                                                         0: Disable */
#else
#define CFG_SDIO_PATHRU_MODE                    0 /*!< 0: Always disable if WINDOWS_CE is not defined */
#endif
#else /* NOT SDIO */

#define CFG_SDIO_INTR_ENHANCE                   0 /*!< 0: Always disable if !_HIF_SDIO */
#define CFG_SDIO_RX_ENHANCE                     0 /*!< 0: Always disable if !_HIF_SDIO */
#define CFG_SDIO_TX_AGG                         0 /*!< 0: Always disable if !_HIF_SDIO */
#define CFG_SDIO_RX_AGG                         0 /*!< 0: Always disable if !_HIF_SDIO */
#define CFG_SDIO_PATHRU_MODE                    0 /*!< 0: Always disable if !_HIF_SDIO */

#endif /* _HIF_SDIO */

#define CFG_MAX_RX_ENHANCE_LOOP_COUNT               3


/*------------------------------------------------------------------------------
 * Flags and Parameters for Integration
 *------------------------------------------------------------------------------
 */
#if defined(MT6620)
#define MT6620_FPGA_BWCS    0
#define MT6620_FPGA_V5      0

#if (MT6620_FPGA_BWCS == 1) && (MT6620_FPGA_V5 == 1)
#error
#endif
#elif defined(MT5931)

#endif

#define CFG_CHIP_RESET_SUPPORT                      0


/*------------------------------------------------------------------------------
 * Flags for workaround
 *------------------------------------------------------------------------------
 */
#if defined(MT6620) && (MT6620_FPGA_BWCS == 0) && (MT6620_FPGA_V5 == 0)
#define MT6620_E1_ASIC_HIFSYS_WORKAROUND            0
#else
#define MT6620_E1_ASIC_HIFSYS_WORKAROUND            0
#endif

/*------------------------------------------------------------------------------
 * Flags for driver version
 *------------------------------------------------------------------------------
 */
#define CFG_DRV_OWN_VERSION                     ((UINT_16)((NIC_DRIVER_MAJOR_VERSION << 8) | (NIC_DRIVER_MINOR_VERSION)))
#define CFG_DRV_PEER_VERSION                    ((UINT_16)0x0000)


/*------------------------------------------------------------------------------
 * Flags for TX path which are OS dependent
 *------------------------------------------------------------------------------
 */
/*! NOTE(Kevin): If the Network buffer is non-scatter-gather like structure(without
 * NETIF_F_FRAGLIST in LINUX), then we can set CFG_TX_BUFFER_IS_SCATTER_LIST to "0"
 * for zero copy TX packets.
 * For scatter-gather like structure, we set "1", driver will do copy frame to
 * internal coalescing buffer before write it to FIFO.
 */
#if defined(LINUX)
#define CFG_TX_BUFFER_IS_SCATTER_LIST       1 /*!< 1: Do frame copy before write to TX FIFO.
                                                        Used when Network buffer is scatter-gather.
                                                     0(default): Do not copy frame */
#else /* WINDOWS/WINCE */
#define CFG_TX_BUFFER_IS_SCATTER_LIST       1
#endif /* LINUX */


#if CFG_SDIO_TX_AGG || CFG_TX_BUFFER_IS_SCATTER_LIST
#define CFG_COALESCING_BUFFER_SIZE          (CFG_TX_MAX_PKT_SIZE * NIC_TX_BUFF_SUM)
#else
#define CFG_COALESCING_BUFFER_SIZE          (CFG_TX_MAX_PKT_SIZE)
#endif /* CFG_SDIO_TX_AGG || CFG_TX_BUFFER_IS_SCATTER_LIST */

/*------------------------------------------------------------------------------
 * Flags and Parameters for TX path
 *------------------------------------------------------------------------------
 */

/*! Maximum number of SW TX packet queue */
#if defined (__WIFI_SLIM__)
#define CFG_TX_MAX_PKT_NUM                      20
#else
#define CFG_TX_MAX_PKT_NUM                      256
#endif /* __WIFI_SLIM__ */

/*! Maximum number of SW TX CMD packet buffer */
#define CFG_TX_MAX_CMD_PKT_NUM                  32

/*! Maximum number of associated STAs */
#define CFG_NUM_OF_STA_RECORD                   8

/*------------------------------------------------------------------------------
 * Flags and Parameters for RX path
 *------------------------------------------------------------------------------
 */

/*! Max. descriptor number - sync. with firmware */
#if CFG_SLT_SUPPORT
#define CFG_NUM_OF_RX0_HIF_DESC                 42
#else
#define CFG_NUM_OF_RX0_HIF_DESC                 16
#endif
#define CFG_NUM_OF_RX1_HIF_DESC                 2

/*! Max. buffer hold by QM */
#if defined (__WIFI_SLIM__)
#define CFG_NUM_OF_QM_RX_PKT_NUM                20
#else
#define CFG_NUM_OF_QM_RX_PKT_NUM                120
#endif /* __WIFI_SLIM__ */

/*! Maximum number of SW RX packet buffer */
#define CFG_RX_MAX_PKT_NUM                      ((CFG_NUM_OF_RX0_HIF_DESC + CFG_NUM_OF_RX1_HIF_DESC) * 2 \
                                                + CFG_NUM_OF_QM_RX_PKT_NUM)

#define CFG_RX_REORDER_Q_THRESHOLD              8

#ifndef LINUX
#define CFG_RX_RETAINED_PKT_THRESHOLD           (CFG_NUM_OF_RX0_HIF_DESC + CFG_NUM_OF_RX1_HIF_DESC + CFG_NUM_OF_QM_RX_PKT_NUM)
#else
#define CFG_RX_RETAINED_PKT_THRESHOLD           0
#endif

/*! Maximum RX packet size, if exceed this value, drop incoming packet */
#define CFG_RX_MAX_PKT_SIZE                     1600 // TODO: it should be 4096 under emulation mode

/*! Minimum RX packet size, if lower than this value, drop incoming packet */
#define CFG_RX_MIN_PKT_SIZE                     10 /*!< 802.11 Control Frame is 10 bytes */

#if defined(__WIFI_HIF_SDIO__) && CFG_SDIO_RX_AGG
/* extra size for CS_STATUS and enhanced response */
#define CFG_RX_COALESCING_BUFFER_SIZE       ((CFG_NUM_OF_RX0_HIF_DESC  + 1) \
                                                * CFG_RX_MAX_PKT_SIZE)
#else
#define CFG_RX_COALESCING_BUFFER_SIZE       (CFG_RX_MAX_PKT_SIZE)
#endif

/*! RX BA capability */
#define CFG_NUM_OF_RX_BA_AGREEMENTS             8
#define CFG_RX_BA_MAX_WINSIZE                   4//16 real size is win size = CFG_RX_BA_MAX_WINSIZE * 2
#define CFG_RX_MAX_BA_TID_NUM                   8
#if defined (__WIFI_SLIM__)
#define CFG_RX_REORDERING_ENABLED               0
#else
#define CFG_RX_REORDERING_ENABLED               1
#endif

/*------------------------------------------------------------------------------
 * Flags and Parameters for CMD/RESPONSE
 *------------------------------------------------------------------------------
 */
#define CFG_RESPONSE_POLLING_TIMEOUT            512


/*------------------------------------------------------------------------------
 * Flags and Parameters for Protocol Stack
 *------------------------------------------------------------------------------
 */
/*! Maximum number of BSS in the SCAN list */
#if defined(WCN_MAUI)
#if defined(__IOT__)
#define CFG_MAX_NUM_BSS_LIST                    16
#else
#if defined (__WIFI_SLIM__)
#define CFG_MAX_NUM_BSS_LIST                    9
#else
#define CFG_MAX_NUM_BSS_LIST                    16
#endif /* __WIFI_SLIM__ */
#endif
#else
#define CFG_MAX_NUM_BSS_LIST                    32
#endif

#define CFG_MAX_COMMON_IE_BUF_LEN         (1500 * CFG_MAX_NUM_BSS_LIST) / 3

/*! Maximum size of IE buffer of each SCAN record */
#define CFG_IE_BUFFER_SIZE                      512

/*! Maximum number of STA records */
#define CFG_MAX_NUM_STA_RECORD                  32



/*------------------------------------------------------------------------------
 * Flags and Parameters for Power management
 *------------------------------------------------------------------------------
 */
#define CFG_ENABLE_FULL_PM                      1
#define CFG_ENABLE_WAKEUP_ON_LAN                0

#define CFG_INIT_POWER_SAVE_PROF                    ENUM_PSP_FAST_SWITCH

#define CFG_INIT_ENABLE_PATTERN_FILTER_ARP                    0

#define CFG_INIT_UAPSD_AC_BMP                    0//(BIT(3) | BIT(2) | BIT(1) | BIT(0))

//#define CFG_SUPPORT_WAPI                        0
#ifdef __WPS_SUPPORT__
#define CFG_SUPPORT_WPS                          1
#define CFG_SUPPORT_WPS2                         1
#else
#define CFG_SUPPORT_WPS                          0
#define CFG_SUPPORT_WPS2                         0
#endif

/*------------------------------------------------------------------------------
 * 802.11i RSN Pre-authentication PMKID cahce maximun number
 *------------------------------------------------------------------------------
 */
#if defined (__WIFI_SLIM__)
#define CFG_MAX_PMKID_CACHE                     4       /*!< max number of PMKID cache
                                                           16(default) : The Max PMKID cache */
#else
#define CFG_MAX_PMKID_CACHE                     16      /*!< max number of PMKID cache
                                                           16(default) : The Max PMKID cache */
#endif /* __WIFI_SLIM__ */

/*------------------------------------------------------------------------------
 * Flags and Parameters for Ad-Hoc
 *------------------------------------------------------------------------------
 */
#define CFG_INIT_ADHOC_FREQ                     (2462000)
#define CFG_INIT_ADHOC_MODE                     AD_HOC_MODE_MIXED_11BG
#define CFG_INIT_ADHOC_BEACON_INTERVAL          (100)
#define CFG_INIT_ADHOC_ATIM_WINDOW              (0)


/*------------------------------------------------------------------------------
 * Flags and Parameters for Load Setup Default
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Flags for enable 802.11A Band setting
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Flags and Parameters for Interrupt Process
 *------------------------------------------------------------------------------
 */
#if CFG_ENABLE_RX_INT_IMPROVE
#define CFG_IST_LOOP_COUNT                  7
#else
#if defined(__WIFI_HIF_SDIO__) && defined(WINDOWS_CE)
#define CFG_IST_LOOP_COUNT                  1
#else
#define CFG_IST_LOOP_COUNT                  3 /* NOTE(Nelson): change from 1 to 3 to enhance performance */
#endif /* _HIF_SDIO */
#endif

#define CFG_INT_WRITE_CLEAR                     0

#if defined(LINUX)
#define CFG_DBG_GPIO_PINS                       0 /* if 1, use MT6516 GPIO pin to log TX behavior */
#endif

//2 Flags for Driver Debug Options
/*------------------------------------------------------------------------------
 * Flags of TX Debug Option. NOTE(Kevin): Confirm with SA before modifying following flags.
 *------------------------------------------------------------------------------
 */
#define CFG_DBG_MGT_BUF                         1 /*!< 1: Debug statistics usage of MGMT Buffer
                                                       0: Disable */

#define CFG_HIF_STATISTICS                      0

#define CFG_HIF_RX_STARVATION_WARNING           0

#define CFG_STARTUP_DEBUG                       0

/*------------------------------------------------------------------------------
 * Flags of Firmware Download Option.
 *------------------------------------------------------------------------------
 */
#define CFG_ENABLE_FW_DOWNLOAD                  1

#define CFG_ENABLE_FW_DOWNLOAD_ACK              1
#define CFG_ENABLE_FW_ENCRYPTION                0

#if defined(MT6620)
#if MT6620_FPGA_BWCS
#define CFG_FW_LOAD_ADDRESS                     0x10014000
#define CFG_OVERRIDE_FW_START_ADDRESS           0
#define CFG_FW_START_ADDRESS                    0x10014001
#elif MT6620_FPGA_V5
#define CFG_FW_LOAD_ADDRESS                     0x10008000
#define CFG_OVERRIDE_FW_START_ADDRESS           0
#define CFG_FW_START_ADDRESS                    0x10008001
#else
#define CFG_FW_LOAD_ADDRESS                     0x10008000
#define CFG_OVERRIDE_FW_START_ADDRESS           0
#define CFG_FW_START_ADDRESS                    0x10008001
#endif
#elif defined(MT5931)
#define CFG_FW_LOAD_ADDRESS                     0xFF900000
#define CFG_FW_START_ADDRESS                    0x00000000
#endif


/*------------------------------------------------------------------------------
 * Flags of Bluetooth-over-WiFi (BT 3.0 + HS) support
 *------------------------------------------------------------------------------
 */

#ifdef LINUX
#ifdef CONFIG_X86
#define CFG_ENABLE_BT_OVER_WIFI         0
#else
#define CFG_ENABLE_BT_OVER_WIFI         1
#endif
#else
#define CFG_ENABLE_BT_OVER_WIFI             0
#endif

#define CFG_BOW_SEPARATE_DATA_PATH              1

#define CFG_BOW_PHYSICAL_LINK_NUM               4

#define CFG_BOW_TEST                            0

#define CFG_BOW_LIMIT_AIS_CHNL                  1

#define CFG_BOW_SUPPORT_11N                     0

#define CFG_BOW_RATE_LIMITATION                 1

/*------------------------------------------------------------------------------
 * Flags of Wi-Fi Direct support
 *------------------------------------------------------------------------------
 */
#ifdef LINUX
#ifdef CONFIG_X86
#define CFG_ENABLE_WIFI_DIRECT          0
#else
#define CFG_ENABLE_WIFI_DIRECT          1
#endif
#else
#define CFG_ENABLE_WIFI_DIRECT              0
#endif

#define CFG_SUPPORT_PERSISTENT_GROUP     0

#define CFG_TEST_WIFI_DIRECT_GO                 0

#define CFG_TEST_ANDROID_DIRECT_GO              0

#define CFG_UNITEST_P2P                         0

/*------------------------------------------------------------------------------
 * Flags of Wi-Fi Tethering support
 *------------------------------------------------------------------------------
 */
#ifdef WCN_MAUI
#if defined(__HOTSPOT_SUPPORT__)
#define CFG_ENABLE_WIFI_TETHERING       1

#else
#define CFG_ENABLE_WIFI_TETHERING       0
#endif
#endif

#define WT_MAXIMUM_CLIENT_COUNT                 8

#define CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST   1

/*
 * Enable cfg80211 option after Android 2.2(Froyo) is suggested,
 * cfg80211 on linux 2.6.29 is not mature yet
 */
#define CFG_ENABLE_WIFI_DIRECT_CFG_80211        0

/*------------------------------------------------------------------------------
 * Configuration Flags (Linux Only)
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_EXT_CONFIG                  0

/*------------------------------------------------------------------------------
 * Statistics Buffering Mechanism
 *------------------------------------------------------------------------------
 */
#if CFG_SUPPORT_PERFORMANCE_TEST
#define CFG_ENABLE_STATISTICS_BUFFERING         1
#else
#define CFG_ENABLE_STATISTICS_BUFFERING         0
#endif
#define CFG_STATISTICS_VALID_CYCLE              2000
#if 0
#define CFG_LINK_QUALITY_VALID_PERIOD           5000    //ms
#else
#define CFG_LINK_QUALITY_VALID_PERIOD           1000    //ticks
#endif

/*------------------------------------------------------------------------------
 * Migration Option
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_ADHOC                       1
#define CFG_SUPPORT_AAA                         1
#define CFG_SUPPORT_LOOPBACK                    0
#define CFG_SUPPORT_INJECT_MSG                  1

#if defined(MT6620)

#elif defined(MT5931)
#define CFG_SUPPORT_BCM                         1
#define CFG_SUPPORT_BCM_BWCS                    1
#else

#endif

#define CFG_SUPPORT_RDD_TEST_MODE       0

#define CFG_SUPPORT_PWR_MGT                     1

#define CFG_RSN_MIGRATION                       1

#define CFG_PRIVACY_MIGRATION                   1

#define CFG_ENABLE_HOTSPOT_PRIVACY_CHECK        0

#define CFG_MGMT_FRAME_HANDLING                 1

#define CFG_MGMT_HW_ACCESS_REPLACEMENT          0

#if CFG_SUPPORT_PERFORMANCE_TEST

#else

#endif

#define CFG_SUPPORT_AIS_5GHZ                    0
#define CFG_SUPPORT_BEACON_CHANGE_DETECTION     1


/*------------------------------------------------------------------------------
 * Option for NVRAM and Version Checking
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_NVRAM                       1
#define CFG_NVRAM_EXISTENCE_CHECK               0
#define CFG_SW_NVRAM_VERSION_CHECK              0
#define CFG_SUPPORT_NIC_CAPABILITY              1


/*------------------------------------------------------------------------------
 * CONFIG_TITLE : Stress Test Option
 * OWNER        : Puff Wen
 * Description  : For stress test only. DO NOT enable it while normal operation
 *------------------------------------------------------------------------------
 */
#define CFG_STRESS_TEST_SUPPORT                 0

/*------------------------------------------------------------------------------
 * Flags for LINT
 *------------------------------------------------------------------------------
 */
#define LINT_SAVE_AND_DISABLE                   /*lint -save -e* */

#define LINT_RESTORE                            /*lint -restore */

#define LINT_EXT_HEADER_BEGIN                   LINT_SAVE_AND_DISABLE

#define LINT_EXT_HEADER_END                     LINT_RESTORE

/*------------------------------------------------------------------------------
 * Flags for PTA added by WCP
 *------------------------------------------------------------------------------
 */
#define DEFAULT_HB_VALUE                        (0x14)
#define PTA_ENABLED                             1

/*------------------------------------------------------------------------------
 * Flags of Features
 *------------------------------------------------------------------------------
 */

#define CFG_SUPPORT_QOS             1   /* Enable/disable QoS TX, AMPDU */
#define CFG_SUPPORT_AMPDU_TX        1
#define CFG_SUPPORT_AMPDU_RX        1
#define CFG_SUPPORT_TSPEC           0   /* Enable/disable TS-related Action frames handling */
#define CFG_SUPPORT_UAPSD           0
#define CFG_SUPPORT_UL_PSMP         0

#if 0   /* 20110607:Nelson test for auth req */
#define CFG_SUPPORT_ROAMING         0  /* Roaming System */
#else
#define CFG_SUPPORT_ROAMING         1  /* Roaming System */
#endif
#define CFG_SUPPORT_SWCR            1

#define CFG_SUPPORT_ANTI_PIRACY     0

/*------------------------------------------------------------------------------
 * BT/WIFI Single Antenna in MAUI
 *------------------------------------------------------------------------------
 */
#ifdef __WIFI_BT_SINGLE_ANTENNA_SUPPORT__
#define CFG_SINGLE_ANTENNA_ENABLED 1
#else
#define CFG_SINGLE_ANTENNA_ENABLED 0
#endif /*__WIFI_BT_SINGLE_ANTENNA_SUPPORT__*/

#define CFG_SUPPORT_OSC_SETTING    1

#if defined(MT5931)
#define CFG_SUPPORT_WHOLE_CHIP_RESET    0
#endif

/*------------------------------------------------------------------------------
 * EHPI HW issue workaround
 *------------------------------------------------------------------------------
 */
#define CFG_EHPI_SW_WORKAROUND     1   /* Enable/disable EHPI SW workaround */
#if defined (__UMTS_TDD128_MODE__)
#define CFG_ALWAYS_PWR_ON_SW_WORKAROUND 1
#else
#define CFG_ALWAYS_PWR_ON_SW_WORKAROUND 0
#endif
/*------------------------------------------------------------------------------
 * HW configuration flags
 *------------------------------------------------------------------------------
 */
#define WIFI_SUPPORT_DCXO_CO_CLOCK  BIT(0)

#define CFG_SAWLESS_SW_WORKAROUND   1

#if 1//defined (MSTAR_CARDV)
#define CFG_DISABLE_PS_MODE     1   //for Mstar CarDV platform
#else
#define CFG_DISABLE_PS_MODE     0
#endif

#if defined (MSTAR_CARDV)
#define RX_RESPONSE_POLLING_TIMEOUT             30
#else
#define RX_RESPONSE_POLLING_TIMEOUT             CFG_RESPONSE_POLLING_TIMEOUT
#endif

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
#endif /* _WLAN_CONFIG_H */


