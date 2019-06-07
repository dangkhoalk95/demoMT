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

#define __WNDRV_OS_FILES_C__

#include <stdlib.h>

//#include "kal_release.h"
#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"
#include "syscomp_config.h"
#include "task_config.h"
#include "task_main_func.h"
#include "stack_ltlcom.h"

/*----------------------------*/
#include "app_buff_alloc.h"

/*----------------------------*/
#include "stack_timer.h"  /* Stack timer */

/*----------------------------*/
#include "kal_trace.h"
#endif

//#include "tst_sap.h"
/*----------------------------*/
#if 0
#include "wndrv_cnst.h"
#include "wndrv_cid.h"
#include "wndrv_cal.h"
#endif

#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"

#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"

//#include "wndrv_private.h"
#include "wndrv_os_private.h"

/*----------------------------*/
#include "precomp.h"
#include "wndrv_context.h"
#include "wndrv_kal.h"
/*----------------------------*/
/* include after precomp.h due to MP_ADAPTER */
/* include after stack_timer.h due to stack_timer_struct */
//#include "wndrv_context.h"
/*---------------------------*/
#if defined(__HOTSPOT_SUPPORT__)
#include "string.h"
#endif /*__HOTSPOT_SUPPORT__*/

#include "wndrv_abm_msg.h"

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
UINT_8  g_hif_rx_event_port_num = HIF_RX_EVENT_PORT_NUM;

unsigned char LB_WIFI_Chip_PowerOn(void)
{
    wifi_chip_hw_power_on();
    return 1;
}

void LB_WIFI_Chip_PowerOff(void)
{
    wifi_chip_hw_power_off();
}

#if CFG_SUPPORT_INJECT_MSG

volatile kal_bool Stop_Gen = (kal_bool) FALSE;


typedef struct _HIF_TX_HEADER_T_ {
    kal_uint16    u2TxByteCount;
    kal_uint8     ucEtherTypeOffset;
    kal_uint8     ucCSflags;
    kal_uint8     aucPayload[1];
} HIF_TX_HEADER_T_, *P_HIF_TX_HEADER_T_;
extern PINT_8 wndrv_firmware_p;
extern UINT_32 wndrv_firmware_size;
extern UINT_32 u4DbgLogSitch;  /* DBG log switch controlled by inject message */

extern void dumpQueue(P_ADAPTER_T prAdapter);
extern VOID swCtrlCmdCategory0(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);
extern VOID swCtrlCmdCategory1(P_ADAPTER_T prAdapter, UINT_8 ucCate, UINT_8 ucAction, UINT_8 ucOpt0, UINT_8 ucOpt1);
#ifdef firmware_loopback
extern PINT_8 wndrv_firmware_loopback_p;
extern UINT_32 wndrv_firmware_loopback_size;
#endif
//extern void WiFi_InitGPIO(char direction, unsigned char pin);
//extern void WiFi_WriteGPIO(char val, unsigned char pin);
extern UINT_32     g_pmu_en_delay;
extern UINT_32     g_sys_rst_delay;
extern BOOLEAN kalIsCardRemoved(IN P_GLUE_INFO_T    prGlueInfo);

static void wndrv_test_set_test_mode_req(kal_char *inject_string);

/* META mode tests */
static void wndrv_test_meta_rx_req(kal_char *inject_string);
static void wndrv_test_meta_tx_req(kal_char *inject_string);
static void wndrv_test_chip_cr_rw(kal_char *inject_string);
static void wndrv_test_chip_swcr_dump(kal_char *inject_string);
static void wndrv_test_get_conn_status(kal_char *inject_string);
static void wndrv_test_set_ibss_channel(kal_char *inject_string);
extern void wndrv_test_ehpi_loopback_test(char *inject_string);
static void wndrv_set_uAPSD(kal_char *inject_string);    /* can be used by IPERF */
static void wndrv_test_setquery_ant(kal_char *inject_string);
static void wndrv_test_chip_efuse(kal_char *inject_string);
static void wndrv_test_set_rx_event_port(kal_char *inject_string);
static void wndrv_test_on_off(kal_char *inject_string);
static void wndrv_test_fw_dl_stress(kal_char *inject_string);
static void wndrv_test_power_on(kal_char *inject_string);
static void wndrv_test_power_on_off(kal_char *inject_string);
static void wndrv_test_set_scan_desired_ssid_list(kal_char *inject_string);
static void wndrv_test_sniffer_mode(kal_char *inject_string);


void wndrv_set_uAPSD(kal_char *inject_string);                     /* can be used by IPERF */
#if 0//defined(__HOTSPOT_SUPPORT__)
static void wndrv_hostap_ut(kal_char *inject_string);
#endif /*__HOTSPOT_SUPPORT__*/

extern void EINT_RestoreMask(kal_uint8 eintno, kal_uint32 val);
#define TEST_PACKET_MIN_LENGTH      (200)
#define TEST_PACKET_MAX_LENGTH      (1500)
#define MAX_TX_QUEUE_NUM        16
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section rwdata="DYNAMICCACHEABLERW_C", zidata="DYNAMICCACHEABLEZI_C"
#endif
#if !defined (__WIFI_SLIM__)
//static unsigned char ucDmaBuf[1500]; /*__HOTSPOT_SUPPORT__*/
#endif /* __WIFI_SLIM__ */
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section
#endif
extern kal_bool WNDRVONOFFTEST;

#if 0
extern WLAN_PS_CTL    wndrv_ps_ctl;
#endif
//extern int rand(void);

typedef void (* wndrv_tst_inject_string_func_ptr)(kal_char *);

/* Packet info to the host driver. The first DW is consistent with the first DW of the HIF RX header. */
// total 16 bytes anyway, this header is pasted by HIF-SW
typedef struct _NIC_HIF_RX_HEADER_T {
    kal_uint16   u2PacketLen;
    kal_uint16   u2PacketType;
    kal_uint8    ucHerderLenOffset;
    kal_uint8    ucReorder;
    kal_uint16   u2SeqNoTid;
    kal_uint8    ucStaRecIdx;
    kal_uint8    aucReserved[3];
} NIC_HIF_RX_HEADER_T, *P_NIC_HIF_RX_HEADER_T;

/* Packet info from the host driver. The first DW is consistent with the first DW of the HIF RX header. */
// change to 4 bytes long which is used currently by F/W
// this structure is for HIF-HW
typedef struct _NIC_HIF_TX_HEADER_T {
    kal_uint16  u2TxByteCount;
    kal_uint8       ucEtherTypeOffset;
    kal_uint8       ucCSUMFlags;
    kal_uint32  dummy[3];
} NIC_HIF_TX_HEADER_T, *P_NIC_HIF_TX_HEADER_T;

typedef enum {
    RANDOM = 0,
    DATA_FF,
    DATA_00,
    DATA_F0,
    DATA_0F,
    DATA_55,
    DATA_99,
    DATA_AA,
    DATA_66
} Data_Pattern;
const wndrv_tst_inject_string_func_ptr
wndrv_tst_inject_string_function_table[] = {
    /* 0: enter test mode */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_test_mode_req,
    /* 1: META Rx test */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_meta_rx_req,
    /* 2: META Tx test */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_meta_tx_req,
    /* 3: MT5931 CR read/write */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_chip_cr_rw,
    /* 4: MT5931 SWCR info dump */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_chip_swcr_dump,
    /* 5: Get connection status */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_get_conn_status,
    /* 6: Set IBSS channel */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_ibss_channel,
#if CFG_SUPPORT_LOOPBACK
    /* 7: eHPI loopback test */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_ehpi_loopback_test,
#endif /* CFG_SUPPORT_LOOPBACK */
    /* 8: Set UAPSD mode */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_uAPSD,
    /* 9: Set antenna path */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_setquery_ant,
    /* 10: MT5931 EFUSE read/write */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_chip_efuse,
    /* 11: Set HIF RX event port */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_rx_event_port,
    /* 12: MT5931 on/off test */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_on_off,
    /* 13: MT5931 FW DL stress test */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_fw_dl_stress,
    /* 14: MT5931 FW DL stress test */
    wndrv_test_power_on,
    /* 15: MT5931 FW DL stress test */
    wndrv_test_power_on_off,
    /* 16: wndrv_test_set_scan_desired_ssid_list */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_scan_desired_ssid_list,
    /* 17: wndrv_test_sniffer_mode */
    (wndrv_tst_inject_string_func_ptr)wndrv_test_sniffer_mode
#if 0
    /* 1: cont tx */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_cont_tx_req,
    /* 2: local freqence */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_local_freq_req,
    /* 3: carrier suppression */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_carrier_supp_req,
    /* 4: packet tx */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_pkt_tx_req,
    /* 5: packet rx */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_pkt_rx_req,
    /* 6: stop */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_stop_req,
    /* 7: reg domain*/
    (wndrv_tst_inject_string_func_ptr) wndrv_test_set_reg_domain_req,
    /* 8: set mcr32*/
    (wndrv_tst_inject_string_func_ptr) wndrv_test_mcr32_rw_req,
    /* 9: set mcr16*/
    (wndrv_tst_inject_string_func_ptr) wndrv_test_mcr16_rw_req,
    /* 10: set bbcr */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_bbcr_rw_req,
    /* 11: set eeprom */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_eeprom_rw_req,
    /* 12: query packet tx */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_query_pkt_tx_req,
    /* 13: query packet rx */
    (wndrv_tst_inject_string_func_ptr) wndrv_test_query_pkt_rx_req,
    /* 14: set fixed rate */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_fixed_rate_req,
    /* 15: set delay */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_delay_req,
    /* 16: set Power Mode */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_PowerMode,
    /* 17: Close Msg */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_CloseMsg,
    /* 18: Set_Misc */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_Misc,
    /* 19: Set_Join */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_Join,
    /* 20: Set Roaming Policy */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_roaming,
    /* 21: Set Scan Policy */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_linkQuality,
    /* 22: Set UAPSD mode */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_uAPSD,
    /* 23: Set BT co-existence */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_btcer,
    /* 24: Set IP address */
    (wndrv_tst_inject_string_func_ptr) wndrv_set_ip
#if defined(__HOTSPOT_SUPPORT__)
    /* 25 : HOST AP */
    , (wndrv_tst_inject_string_func_ptr) wndrv_hostap_ut
#endif /*__HOTSPOT_SUPPORT__*/
#endif
};

static const kal_uint32 freqTable24[] = {
    0,
    2412000,
    2417000,
    2422000,
    2427000,
    2432000,
    2437000,
    2442000,
    2447000,
    2452000,
    2457000,
    2462000,
    2467000,
    2472000,
    2484000
};


extern wndrv_context_struct wndrv_context;
extern kal_uint32 wndrv_UAPSD_isEnable;
kal_uint32   gMsgCtl = 1;

#if 0
static UINT_8 g_fw_dl_buffer[CMD_PKT_SIZE_FOR_IMAGE];
/*****************************************************************************
* FUNCTION
*   wndrv_firmware_download
* DESCRIPTION
*   Firmware download function
* PARAMETERS
*   none
* RETURNS
*   none
* GLOBALS AFFECTED
*   none
*****************************************************************************/
void wndrv_firmware_download(void)
{
    kal_uint32 i, ret;
    kal_uint8 *data;
    kal_uint32 len;
    kal_uint8 ucSeqNum = 0;
    kal_uint32 u4DestAddr = 0x00000000;
    kal_uint32 transSize;
    P_HIF_TX_HEADER_T_ prHifTxHeader;
    kal_uint8 key_idx = 0;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32 reg_value = 0;

    /* 0. Read Chip ID */
    HAL_MCR_RD(prAdapter, WCIR, &ret);
    ret &= 0xfffff;
    MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]Chip ID = %x", ret);

    /* 1. wait for INIT_RDY */
    for (i = 0 ; i < CFG_RESPONSE_POLLING_TIMEOUT ; i++) {
        HAL_MCR_RD(prAdapter, FWDLCMR0, &ret);
        if ((ret & WMCSR_INI_RDY) == WMCSR_INI_RDY) {
            MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]INIT_RDY = 1");
            break;
        } else {
            kalMsleep(10);
        }
    }

    if (i == CFG_RESPONSE_POLLING_TIMEOUT) {
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]INIT_RDY is not 1 (FW Download fail!)");
        ASSERT(0);
        goto exit;
    }

    /* 2. request firmware for upload */
    len = wndrv_firmware_loopback_size;

    data = (kal_uint8 *)wndrv_firmware_loopback_p;

    /* 3. set KSEL/FLEN */
    MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]sing #%d key with length = %d bytes\n", key_idx, (int)len);
    HAL_MCR_WR(prAdapter, FWDLCMR1, (key_idx << 14) | (len >> 6));
    HAL_MCR_RD(prAdapter, FWDLCMR1, &reg_value);
    MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test][RD] key index = %d / length = %d bytes\n",
                     (reg_value & 0xc000) >> 14,
                     (reg_value & 0x3fff) * 64);

    /* 4. enable FWDL_EN */
    //mt5931_ehpi_write_reg32(MCR_WMCSR, WMCSR_FWDLEN|WMCSR_FWDLRST);
    HAL_MCR_WR(prAdapter, FWDLCMR0, WMCSR_FWDLEN | WMCSR_FWDLRST);
    //mt5931_ehpi_write_reg32(MCR_WMCSR, WMCSR_FWDLEN);
    HAL_MCR_WR(prAdapter, FWDLCMR0, WMCSR_FWDLEN);

    /* 5. wait for PLL_RDY */
    for (i = 0 ; i < CFG_RESPONSE_POLLING_TIMEOUT ; i++) {
        HAL_MCR_RD(prAdapter, FWDLCMR0, &ret);
        if ((ret & WMCSR_PLLRDY) == WMCSR_PLLRDY) {
            MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]PLL_RDY = 1\n");
            break;
        } else {
            kalMsleep(10);
        }
    }

    if (i == CFG_RESPONSE_POLLING_TIMEOUT) {
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]PLL_RDY is not 1 (FW Download fail!)");
        ASSERT(0);
        goto exit;
    }

    /* 6. turn on HIFSYS firmware download mode */
    HAL_MCR_WR(prAdapter, FWDLSR, FWDLSR_FWDL_MODE);

    /* 7. set starting address from 0xff900000 */
    HAL_MCR_WR(prAdapter, FWDLDSAR, 0xFF900000);

    /* 8. upload firmware */
    prHifTxHeader = (P_HIF_TX_HEADER_T_)&g_fw_dl_buffer[0];

    while (len > 0) {
        kal_uint32 translen = (len > 2048) ? 2048 : len;

        /* memory zero */
        memset(prHifTxHeader, 0, HIF_HW_TX_HDR_SIZE);

        /* prepare init command .. */
        prHifTxHeader->u2TxByteCount = ALIGN_4(HIF_HW_TX_HDR_SIZE + translen);
        prHifTxHeader->ucEtherTypeOffset = 0;
        prHifTxHeader->ucCSflags = 0;

        memcpy(prHifTxHeader->aucPayload, data, translen);

        /* add 4-bytes zero tail */
        kalMemZero(&(prHifTxHeader->aucPayload[ALIGN_4(translen)]), HIF_HW_TX_HDR_SIZE);

        /* wait til FWDL_RDY = 1 */
        for (i = 0 ; i < CFG_RESPONSE_POLLING_TIMEOUT ; i++) {
            HAL_MCR_RD(prAdapter, FWDLSR, &ret);
            if ((ret & FWDLSR_FWDL_RDY) == FWDLSR_FWDL_RDY) {
                break;
            } else {
                kalMsleep(10);
            }
        }

        if (i == CFG_RESPONSE_POLLING_TIMEOUT) {
            MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test](1)FWDL_RDY is not 1\n");
            ASSERT(0);
            goto exit;
        }

        HAL_MCR_RD(prAdapter, FWDLDSAR, &ret);
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]FWDL_RDY = 1 (0x%x / %d bytes)\n",
                         ret,
                         translen);

#if 0
        /* send */
        transSize = prHifTxHeader->u2TxByteCount;
        if (transSize % 512) { /* block alignment */
            transSize += (512 - (transSize % 512));
        }
#endif

        HAL_PORT_WR(prAdapter, FWDLDR, transSize, (kal_uint8 *)prHifTxHeader, transSize);

        /* wait til FWDL_RDY = 1 */
        for (i = 0 ; i < CFG_RESPONSE_POLLING_TIMEOUT ; i++) {
            HAL_MCR_RD(prAdapter, FWDLSR, &ret);
            if ((ret & FWDLSR_FWDL_RDY) == FWDLSR_FWDL_RDY) {
                break;
            } else {
                kalMsleep(10);
            }
        }

        if (i == CFG_RESPONSE_POLLING_TIMEOUT) {
            MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test](2)FWDL_RDY is not 1\n");
            ASSERT(0);
            goto exit;
        }

        /* increasing */
        u4DestAddr += translen;
        ucSeqNum++;

        len -= translen;
        data += translen;
    }

    /* 9. poll FWDL_OK & FWDL_FAIL bits */
    for (i = 0 ; i < CFG_RESPONSE_POLLING_TIMEOUT ; i++) {
        HAL_MCR_RD(prAdapter, FWDLCMR0, &ret);
        if ((ret & (WMCSR_DL_OK | WMCSR_DL_FAIL)) != 0) {
            if (ret & WMCSR_DL_OK) {
                MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]WDL_OK = 1\n");
            } else if (ret & WMCSR_DL_FAIL) {
                MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]FWDL_FAIL = 1\n");
                ASSERT(0);
            } else {
                MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]WMCSR = 0x%08x\n", ret);
            }

            break;
        } else {
            kalMsleep(10);
        }
    }

    if (i == CFG_RESPONSE_POLLING_TIMEOUT) {
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]FWDL_OK/FWDL_FAIL is not ready!\n");
        HAL_MCR_RD(prAdapter, FWDLCMR0, &ret);
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]WMCSR = 0x%08x\n", ret);
        HAL_MCR_RD(prAdapter, FWDLCMR1, &ret);
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]FWCFG = 0x%08x\n", ret) ;
        ASSERT(0);
        goto exit;
    }

    /* turn off download mode */
    HAL_MCR_WR(prAdapter, FWDLSR, 0);

    /* disable interrupt */
    nicDisableInterrupt(prAdapter);

    /* wait for ready bit */
    for (i = 0 ; i < CFG_RESPONSE_POLLING_TIMEOUT ; i++) {
        HAL_MCR_RD(prAdapter, WCIR, &ret);

        if ((ret & WCIR_WLAN_READY) == WCIR_WLAN_READY) {
            break;
        } else {
            kalMsleep(10);
        }
    }
    if (i == CFG_RESPONSE_POLLING_TIMEOUT) {
        MT5931_NEW_TRACE(MT5931_INFO_4,  TRACE_GROUP_6, "[eHPI test]wait for ready bit failed!\n");
        ASSERT(0);
    }

exit:
    return;
}
#endif
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_test_mode_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_test_mode_req(kal_char *inject_string)
{
    kal_uint32                  test_mode;
    kal_uint32                  uInterval;
    kal_uint32                  test_item;

    sscanf(inject_string, "%d,%d", &test_item, &test_mode);

    MT5931_NEW_TRACE(MT5931_INFO_506, TRACE_GROUP_10, "wndrv_test_set_test_mode_req(): %d ms", test_mode);

    uInterval = test_mode * 13 / 60 + 1;    /* ~= x 1/4.615 => convert ms to ticks */

    if (test_item == 0) {
        kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_TEST_TIMER], uInterval);
    }
    if (test_item == 1) {
        kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_CNM_TIMER], uInterval);
    }

#if 0
    if (test_mode == 1) {
        RFTool_WiFi_EnterTestMode();
    } else {
        RFTool_WiFi_EnterNormalMode();
    }
#endif

}


/*******************************************************************************
* FUNCTION
*   wndrv_test_meta_rx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_meta_rx_req(kal_char *inject_string)
{
    kal_uint32                  test_times;
    kal_uint32                  test_item;
    ilm_struct                   *ilm_p;
    wndrv_test_query_req_struct   *query_p;
    wndrv_test_set_req_struct     *set_p;
    kal_uint32 i = 0;

    kal_uint16                  pdu_len;
    wndrv_test_rx_bw_struct    *test_band_p;

    sscanf(inject_string, "%d,%d", &test_item, &test_times);

    MT5931_NEW_TRACE(MT5931_INFO_507, TRACE_GROUP_10, "wndrv_test_meta_rx_req(): rx query times: %d", test_times);

    /* Set to enter meta inject msg mode */
    wndrv_context.glue_p->meta_inject_mode = (kal_bool) TRUE;

    /* allocate ilm */
    query_p = (wndrv_test_query_req_struct *)
              construct_local_para(sizeof(wndrv_test_query_req_struct), TD_CTRL);
    set_p = (wndrv_test_set_req_struct *)
            construct_local_para(sizeof(wndrv_test_set_req_struct), TD_CTRL);
    query_p->token = 0;
    set_p->token = 0;

    ilm_p                 = allocate_ilm(MOD_NIL);
    ilm_p->peer_buff_ptr  = NULL;

    /* 1. set_test_mode */
    if ((test_item == 0) || (test_item == 1)) {
        set_p->type = WNDRV_TEST_SET_TEST_MODE;    /* A list of set commands */
        set_p->data.test_mode = 1;  /* test mode */
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* 2. set_rx_bw */
    if ((test_item == 0) || (test_item == 2)) {
        set_p->type = WNDRV_TEST_SET_RX_BW;    /* A list of set commands */
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        ilm_p->peer_buff_ptr = construct_peer_buff(sizeof(wndrv_test_rx_bw_struct), 0, 0, TD_UL);
        test_band_p = (wndrv_test_rx_bw_struct *)get_pdu_ptr(ilm_p->peer_buff_ptr, &pdu_len);
        test_band_p->rxBw = WNDRV_TEST_BW_20MHZ;
        wndrv_test_set_handler(ilm_p);
        free_peer_buff(ilm_p->peer_buff_ptr);
        ilm_p->peer_buff_ptr  = NULL;
    }

    /* 3. set_pkt_rx */
    if ((test_item == 0) || (test_item == 3)) {
        set_p->type = WNDRV_TEST_SET_PKT_RX;    /* A list of set commands */
        set_p->data.pkt_rx.ch_freq = 2412000;   /* unit:khz, channel 1 */
        set_p->data.pkt_rx.rxAnt = WNDRV_RX_ANT_DIVERSITY_AGC;
        set_p->data.pkt_rx.mode = WNDRV_RF_RX_TEST_MODE;
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* 4. query_pkt_rx */
    if ((test_item == 0) || (test_item == 4)) {
        for (i = 0; i < test_times; i++) {
            query_p->type = WNDRV_TEST_QUERY_PKT_RX;    /* A list of set commands */
            ilm_p->msg_id         = MSG_ID_WNDRV_TEST_QUERY_REQ; //MSG_ID_WNDRV_TEST_QUERY_REQ
            ilm_p->local_para_ptr = (local_para_struct *) query_p;
            wndrv_test_query_handler(ilm_p);
            kalUdelay(5000);    // delay 5 ms
        }
    }

    /* 5. set_stop */
    if ((test_item == 0) || (test_item == 5)) {
        set_p->type = WNDRV_TEST_SET_STOP;    /* A list of set commands */
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* free ilm */
    free_local_para((local_para_struct *)query_p);
    free_local_para((local_para_struct *)set_p);
    ilm_p->local_para_ptr = NULL;
    free_ilm(ilm_p);
    //cancel_ilm(MOD_NIL);

    /* Set to leave meta inject msg mode */
    wndrv_context.glue_p->meta_inject_mode = (kal_bool) FALSE;
}


/*******************************************************************************
* FUNCTION
*   wndrv_test_meta_tx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_meta_tx_req(kal_char *inject_string)
{
    kal_uint32                  test_times;
    kal_uint32                  test_item;
    ilm_struct                   *ilm_p;
    wndrv_test_query_req_struct   *query_p;
    wndrv_test_set_req_struct     *set_p;

    kal_uint32 i = 0;

    kal_uint16                  pdu_len;
    wndrv_test_tx_bw_struct    *test_band_p;
    wndrv_test_tx_gi_struct    *test_gi_p;
    wndrv_test_tx_mode_sel_struct *test_mode_sel_p;

    sscanf(inject_string, "%d,%d", &test_item, &test_times);

    MT5931_NEW_TRACE(MT5931_INFO_508, TRACE_GROUP_10, "wndrv_test_meta_tx_req(): tx query times: %d", test_times);

    /* Set to enter meta inject msg mode */
    wndrv_context.glue_p->meta_inject_mode = (kal_bool) TRUE;

    /* allocate ilm */
    query_p = (wndrv_test_query_req_struct *)
              construct_local_para(sizeof(wndrv_test_query_req_struct), TD_CTRL);
    set_p = (wndrv_test_set_req_struct *)
            construct_local_para(sizeof(wndrv_test_set_req_struct), TD_CTRL);
    query_p->token = 0;
    set_p->token = 0;

    ilm_p                 = allocate_ilm(MOD_NIL);
    ilm_p->peer_buff_ptr  = NULL;

    /* 1. set_test_mode */
    if ((test_item == 0) || (test_item == 1)) {
        set_p->type = WNDRV_TEST_SET_TEST_MODE;
        set_p->data.test_mode = 1;  /* test mode */
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* 2. set_tx_bw */
    if ((test_item == 0) || (test_item == 2)) {
        set_p->type = WNDRV_TEST_SET_TX_BW;
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        ilm_p->peer_buff_ptr = construct_peer_buff(sizeof(wndrv_test_tx_bw_struct), 0, 0, TD_UL);
        test_band_p = (wndrv_test_tx_bw_struct *)get_pdu_ptr(ilm_p->peer_buff_ptr, &pdu_len);
        test_band_p->txBw = WNDRV_TEST_BW_20MHZ;
        wndrv_test_set_handler(ilm_p);
        free_peer_buff(ilm_p->peer_buff_ptr);
        ilm_p->peer_buff_ptr  = NULL;
    }

    /* 3. set_tx_gi */
    if ((test_item == 0) || (test_item == 3)) {
        set_p->type = WNDRV_TEST_SET_TX_GI;
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        ilm_p->peer_buff_ptr = construct_peer_buff(sizeof(wndrv_test_tx_gi_struct), 0, 0, TD_UL);
        test_gi_p = (wndrv_test_tx_gi_struct *)get_pdu_ptr(ilm_p->peer_buff_ptr, &pdu_len);
        test_gi_p->txGi = WNDRV_TEST_TX_GI_800NS;
        wndrv_test_set_handler(ilm_p);
        free_peer_buff(ilm_p->peer_buff_ptr);
        ilm_p->peer_buff_ptr  = NULL;
    }

    /* 4. set_tx_mode_sel */
    if ((test_item == 0) || (test_item == 4)) {
        set_p->type = WNDRV_TEST_SET_TX_MODE_SEL;
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        ilm_p->peer_buff_ptr = construct_peer_buff(sizeof(wndrv_test_tx_mode_sel_struct), 0, 0, TD_UL);
        test_mode_sel_p = (wndrv_test_tx_mode_sel_struct *)get_pdu_ptr(ilm_p->peer_buff_ptr, &pdu_len);
        test_mode_sel_p->txMode = WNDRV_TEST_TX_MIX_MODE;
        wndrv_test_set_handler(ilm_p);
        free_peer_buff(ilm_p->peer_buff_ptr);
        ilm_p->peer_buff_ptr  = NULL;
    }

    /* 5. set_cont_tx */
    if ((test_item == 0) || (test_item == 5)) {
        set_p->type = WNDRV_TEST_SET_CONT_TX;
        set_p->data.cont_tx.ch_freq = 2412000;   /* unit:khz, channel 1 */
        set_p->data.cont_tx.tx_gain_dac = 7;
        set_p->data.cont_tx.txAnt = 0;
        set_p->data.cont_tx.tx_rate = WNDRV_TEST_RATE_54M;
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* 6. set_pkt_tx */
    if ((test_item == 0) || (test_item == 6)) {
        set_p->type = WNDRV_TEST_SET_PKT_TX;
        set_p->data.pkt_tx.ch_freq = 2412000;   /* unit:khz, channel 1 */
        set_p->data.pkt_tx.tx_rate = WNDRV_TEST_RATE_54M;
        set_p->data.pkt_tx.tx_gain_dac = 7;
        set_p->data.pkt_tx.pktCount = 20;   /* tx pkt count */
        set_p->data.pkt_tx.pktInterval = 0;
        set_p->data.pkt_tx.pktLength = 512;
        set_p->data.pkt_tx.pattern = WNDRV_TX_ALTERNATE_BITS;
        set_p->data.pkt_tx.txAnt = 0;
        set_p->data.pkt_tx.txFlags = 0;
        set_p->data.pkt_tx.targetAlc = 7;
        set_p->data.pkt_tx.is_short_preamble = (kal_bool) FALSE;   /* long preamble */
        kal_mem_set(set_p->data.pkt_tx.mac_header, 0xff, 24);
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* 7. query_pkt_tx */
    if ((test_item == 0) || (test_item == 7)) {
        for (i = 0; i < test_times; i++) {
            query_p->type = WNDRV_TEST_QUERY_PKT_TX;
            ilm_p->msg_id         = MSG_ID_WNDRV_TEST_QUERY_REQ; //MSG_ID_WNDRV_TEST_QUERY_REQ
            ilm_p->local_para_ptr = (local_para_struct *) query_p;
            wndrv_test_query_handler(ilm_p);
            kalUdelay(5000);    // delay 5 ms
        }
    }

    /* 8. set_stop */
    if ((test_item == 0) || (test_item == 8)) {
        set_p->type = WNDRV_TEST_SET_STOP;
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
        ilm_p->local_para_ptr = (local_para_struct *) set_p;
        wndrv_test_set_handler(ilm_p);
    }

    /* free ilm */
    free_local_para((local_para_struct *)query_p);
    free_local_para((local_para_struct *)set_p);
    ilm_p->local_para_ptr = NULL;
    free_ilm(ilm_p);
    //cancel_ilm(MOD_NIL);

    /* Set to leave meta inject msg mode */
    wndrv_context.glue_p->meta_inject_mode = (kal_bool) FALSE;
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_chip_cr_rw
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_chip_cr_rw(kal_char *inject_string)
{
    kal_uint32                  read, cr_offset, cr_value;
    UINT_32 u4BufLen;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;

    P_WIFI_EVENT_T prEvent;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_ACCESS_REG prRegStatus;
    UINT_32 u4RxPktLength;

    sscanf(inject_string, " %d , %x , %x ", &read, &cr_offset, &cr_value);

    MT5931_NEW_TRACE(MT5931_INFO_509, TRACE_GROUP_10, "wndrv_test_chip_cr_rw(): read: %d, cr_offset: %x, cr_value: %x", read, cr_offset, cr_value);

#if 0
    if (read == 1) {
        HAL_MCR_RD(wndrv_context.adpt_p, cr_offset, &cr_value);
        MT5931_NEW_TRACE(MT5931_INFO_5, TRACE_GROUP_10, "[READ]cr_offset: %x, cr_value: %x", cr_offset, cr_value);
    } else {
        HAL_MCR_WR(wndrv_context.adpt_p, cr_offset, cr_value);
        MT5931_NEW_TRACE(MT5931_INFO_5, TRACE_GROUP_10, "[WRITE]cr_offset: %x, cr_value: %x", cr_offset, cr_value);
    }
#endif

    if (read == 1) {
        /* Be sure MCR address is 4 byte boundary. */
        WNDRV_ASSERT((cr_offset & 0x03) == 0x00);

        rMcrInfo.u4McrOffset = cr_offset;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        wlanQueryInformation(prAdapter,
                             wlanoidQueryMcrRead,
                             (PVOID)&rMcrInfo,
                             sizeof(rMcrInfo),
                             &u4BufLen);

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        /* Check if access F/W Domain MCR (due to WiFiSYS is placed from 0x6000-0000*/
        if (rMcrInfo.u4McrOffset & 0xFFFF0000) {
            /* Process mailbox, CMD, packet tx here */
            kalMainThread(prGlueInfo);

            /* Polling Query Transmitted count Event */
            memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
            if (nicRxWaitResponse(prAdapter,
                                  g_hif_rx_event_port_num,
                                  aucBuffer,
                                  EVENT_HDR_SIZE + sizeof(EVENT_ACCESS_REG),
                                  &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
                MT5931_NEW_TRACE(MT5931_INFO_510, TRACE_GROUP_10, "[READ]cr_offset: %x Fail!", cr_offset);
            } else {
                prEvent = (P_WIFI_EVENT_T)aucBuffer;
                prRegStatus = (P_EVENT_ACCESS_REG)(prEvent->aucBuffer);
                MT5931_NEW_TRACE(MT5931_INFO_511, TRACE_GROUP_10, "[READ]cr_offset: %x, cr_value: %x", cr_offset, prRegStatus->u4Data);
            }
        } else {
            MT5931_NEW_TRACE(MT5931_INFO_512, TRACE_GROUP_10, "[READ]cr_offset: %x, cr_value: %x", cr_offset, rMcrInfo.u4McrData);
        }
    } else {
        rMcrInfo.u4McrOffset = cr_offset;
        rMcrInfo.u4McrData = cr_value;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        wlanSetInformation(prAdapter,
                           wlanoidSetMcrWrite,
                           (PVOID)&rMcrInfo,
                           sizeof(rMcrInfo),
                           &u4BufLen);

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        MT5931_NEW_TRACE(MT5931_INFO_513, TRACE_GROUP_10, "[WRITE]cr_offset: %x, cr_value: %x", cr_offset, cr_value);
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_chip_swcr_dump
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_chip_swcr_dump(kal_char *inject_string)
{
    kal_uint32                  category = 2;   /* 0:swCtrlCmdCategory0, 1:swCtrlCmdCategory1, 2: dumpQueue*/
    kal_uint32                  ucCate, ucAction, ucOpt0, ucOpt1;
    kal_uint32                  u4UpperBound0 = 0, u4UpperBound1 = 0;
    //UINT_32 u4BufLen;

    sscanf(inject_string, "%d, %d, %d, %d, %d ", &category, &ucCate, &ucAction, &ucOpt0, &ucOpt1);

    MT5931_NEW_TRACE(MT5931_INFO_565, TRACE_GROUP_10, "wndrv_test_chip_swcr_dump(): category:%d, ucCate:%d, ucAction:%d, ucOpt0:%d, ucOpt1:%d", \
                     category, ucCate, ucAction, ucOpt0, ucOpt1);

    u4UpperBound0 = QM_DBG_CNT_NUM;


    if (ucOpt0 == 0) {
        u4UpperBound1 = TC_NUM;
    } else if (ucOpt0 == 1) {
        u4UpperBound1 = NUM_OF_PER_STA_TX_QUEUES;

    } else if (ucOpt0 == 2) {
        u4UpperBound1 = NUM_OF_PER_TYPE_TX_QUEUES;
    }


    if (ucOpt0 >=  u4UpperBound0 || ucOpt1 >= u4UpperBound1) {


        MT5931_NEW_TRACE0(MT5931_INFO_601, TRACE_ERROR, "ucOpt0 or ucOpt1 is too large");
        return;

    }

    switch (category) {
        case 0:
            swCtrlCmdCategory0(wndrv_context.adpt_p, (UINT_8)ucCate, (UINT_8)ucAction, (UINT_8)ucOpt0, (UINT_8)ucOpt1);
            break;
        case 1:
            swCtrlCmdCategory1(wndrv_context.adpt_p, (UINT_8)ucCate, (UINT_8)ucAction, (UINT_8)ucOpt0, (UINT_8)ucOpt1);
            break;
        case 2:
            dumpQueue(wndrv_context.adpt_p) ;
            break;
        case 3: /* power test */
            wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR] = ucCate;
            wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_PWR_CTRL_TIMEOUT] = ucAction;
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_285, "set dec power = %d, power ctrl TO = %d * 100ms", ucCate, ucAction);
            break;
        default:
            MT5931_NEW_TRACE0(MT5931_INFO_514, TRACE_GROUP_10, "Unexpected category!");
            break;
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_get_conn_status
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_get_conn_status(kal_char *inject_string)
{
    WLAN_STATUS             rStatus;

    MT5931_NEW_TRACE0(MT5931_INFO_515, TRACE_GROUP_10, "wndrv_test_get_conn_status()");

    rStatus = wlanSendSetQueryCmd(
                  wndrv_context.adpt_p,           /* prAdapter */
                  CMD_ID_GET_CONNECTION_STATUS,   /* ucCID */
                  FALSE,                          /* fgSetQuery */
                  TRUE,                           /* fgNeedResp */
                  FALSE,                          /* fgIsOid */
                  NULL,                           /* pfCmdDoneHandler */
                  NULL,                           /* pfCmdTimeoutHandler */
                  0,                              /* u4SetQueryInfoLen */
                  NULL,                           /* pucInfoBuffer */
                  NULL,                           /* pvSetQueryBuffer */
                  0                               /* u4SetQueryBufferLen */
              );

    MT5931_NEW_TRACE(MT5931_INFO_516, TRACE_GROUP_10, "wlanSendSetQueryCmd(CMD_ID_GET_CONNECTION_STATUS): status=%d", rStatus);
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_set_ibss_channel
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_ibss_channel(kal_char *inject_string)
{
    kal_uint32  channel_num = 1;

    sscanf(inject_string, "%d", &channel_num);

    MT5931_NEW_TRACE(MT5931_INFO_517, TRACE_GROUP_10, "wndrv_test_set_ibss_channel(): channel num=%d", channel_num);
    if (channel_num > 14) {

        MT5931_NEW_TRACE(MT5931_INFO_600, TRACE_ERROR, "error! channel num=%d", channel_num);
        return;

    }


    wndrv_context.glue_p->rRegInfo.u4StartFreq = freqTable24[channel_num];

    wndrv_context.adpt_p->rWifiVar.rConnSettings.ucAdHocChannelNum =
        (UINT_8) nicFreq2ChannelNum(wndrv_context.glue_p->rRegInfo.u4StartFreq);
    wndrv_context.adpt_p->rWifiVar.rConnSettings.eAdHocBand =
        wndrv_context.glue_p->rRegInfo.u4StartFreq < 5000000 ? BAND_2G4 : BAND_5G;
}


/*******************************************************************************
* FUNCTION
*   wndrv_prepare_test
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#if !defined (__WIFI_SLIM__)
#if 0

static int  wndrv_prepare_test(int test_number)
{
    kal_uint32 response = 0;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32 reg_value = 0;
    //kal_uint32 i;
    kal_uint32 loop_count = 0;

    MT5931_NEW_TRACE(MT5931_INFO_518, TRACE_GROUP_10, "wndrv_prepare_test():#%d", test_number);

    /* write test number */
    HAL_MCR_WR(prAdapter, H2DSM0R, (unsigned int)test_number);

    /* interrupt firmware: H2D[31] */
//    HAL_MCR_WR( prAdapter, WSICR, 1 << 31 );
    HAL_MCR_WR(prAdapter, WSICR, 0x80000000);

    /* wait for response */
    while (1) {
        HAL_MCR_RD(prAdapter, WHISR, &reg_value);

        if (reg_value & (0x80000000)) { /* check bit_31 of WHISR */
            HAL_MCR_RD(prAdapter, D2HRM0R, &response);
            MT5931_NEW_TRACE(MT5931_INFO_519, TRACE_GROUP_10, "WHISR = [%x]", reg_value);
            MT5931_NEW_TRACE(MT5931_INFO_520, TRACE_GROUP_10, "response = [%x]", response);
            break;
        } else {
            HAL_MCR_WR(prAdapter, WHIER, 0xffffff17);   /* enable interrupt */
            kalUdelay(10);
            //if ( loop_count++ > 500000 )
            if (loop_count++ > 5120) {
                MT5931_NEW_TRACE0(MT5931_INFO_521, TRACE_GROUP_10, "Pollin WHISR fail");
                return -1;
            }
        }
    }
    if (response == test_number + 1) {
        return 0;
    } else if (response == test_number - 1) {
        return -1;
    } else { /* unexpected response */
        return -1;
    }
}

static int wndrv_generate_random_payload(unsigned char *buf, int len)
{
    int i;

    //randomize();

    for (i = 0 ; i < len ; i++) {
        buf[i] = (unsigned char)(rand() & 0xff);
    }

    return 0;
}

/*******************************************************************************
* FUNCTION
*   wndrv_generate_test_frame
*
* DESCRIPTION
*   generate packet data to TX
*
* CALLS
*   None
*
* PARAMETERS
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/


static int wndrv_generate_test_frame(unsigned char *buf,    int len, Data_Pattern type)
{

    // controls pattern (initialization included)
    //unsigned char test_cnt = 5;
    //unsigned char i;
    unsigned char *payload = NULL;
    unsigned short payload_s = 0;
    unsigned char *dst = buf;
    int offset = 0;
    NIC_HIF_TX_HEADER_T txHeader;

    payload = kalMemAlloc(1500, PHY_MEM_TYPE);
    payload_s = len - sizeof(NIC_HIF_TX_HEADER_T);


    // attach NIC_HIF_TX_HEADER_T in the head

    txHeader.u2TxByteCount      = len;
    txHeader.ucEtherTypeOffset  = 0x0;//(sizeof(NIC_HIF_TX_HEADER_T) + 12) / 2;
    txHeader.ucCSUMFlags        = 0x0; // [UT = 0 / I = 0]
    txHeader.dummy[0]       = 0x0;
    txHeader.dummy[1]       = 0x0;
    txHeader.dummy[2]       = 0x0;

    kal_mem_cpy(&dst[offset], &txHeader, sizeof(NIC_HIF_TX_HEADER_T));
    offset += sizeof(NIC_HIF_TX_HEADER_T);

    // payload generation
    switch (type) {
        case RANDOM:
            if (payload_s > 0) {
                wndrv_generate_random_payload(&payload[0], payload_s);
            }
            break;
        case DATA_FF:
            memset(payload, 0xFF , payload_s);
            break;
        case DATA_00:
            memset(payload, 0x00 , payload_s);
            break;
        case DATA_F0:
            memset(payload, 0xF0 , payload_s);
            break;
        case DATA_0F:
            memset(payload, 0x0F , payload_s);
            break;
        case DATA_55:
            memset(payload, 0x55 , payload_s);
            break;
        case DATA_99:
            memset(payload, 0x99 , payload_s);
            break;
        case DATA_AA:
            memset(payload, 0xAA , payload_s);
            break;
        case DATA_66:
            memset(payload, 0x66 , payload_s);
            break;
        default:
            memset(payload, 0x11 , payload_s);
            break;
    }
    // memcpy for generated frame
    kal_mem_cpy(&dst[offset], &payload[0], payload_s);
    kalMemFree(payload, PHY_MEM_TYPE, 1500);
    return 0;
}
/*******************************************************************************
* FUNCTION
*   wndrv_wifi_hifsys_init
*
* DESCRIPTION
*   Request FW-Own back
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/

static void  wndrv_wifi_hifsys_init(void)
{
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32     u4Value;
    UINT_32  counter = 0;

    do {
        HAL_MCR_WR(prAdapter, WHLPCR, 0x00000200);
        HAL_MCR_RD(prAdapter, WHLPCR, &u4Value);
        kalMsleep(10);
        if (counter++ > 512) {
            MT5931_NEW_TRACE(MT5931_INFO_522, TRACE_GROUP_10, "[eHPI test]Request FW-Own back failed! (times=%d)", counter);
            ASSERT(0);
        }
    } while (!(u4Value & 0x00000100));
    HAL_MCR_WR(prAdapter, WHIER, 0xffffff17);
}
#endif
#endif

/*******************************************************************************
* FUNCTION
*   wndrv_set_uAPSD
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_set_uAPSD(kal_char *inject_string)
{
    kal_int32   ucIsEnterPsAtOnce, ucIsDisableUcTrigger, bAC0, bAC1, bAC2, bAC3;
    P_PARAM_CUSTOM_WMM_PS_TEST_STRUC_T set = &wndrv_context.glue_p->UAPSDSetting;
    //kal_uint32 setInfoLen;

    PARAM_CUSTOM_SW_CTRL_STRUC_T rSwCtrlInfo;
    UINT_32 u4SetInfoLen;

    WiFi_EintMask();
    //wndrv_bus_power_control(1);

    sscanf(inject_string, " %d, %d, %d, %d, %d, %d", &ucIsEnterPsAtOnce, &ucIsDisableUcTrigger, &bAC0, &bAC1, &bAC2, &bAC3);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", ucIsEnterPsAtOnce);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", ucIsDisableUcTrigger);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC0);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC1);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC2);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC3);

    set->bmfgApsdEnAc = 0;

    if (bAC0) {
        set->bmfgApsdEnAc |= UAPSD_AC0;
    }

    if (bAC1) {
        set->bmfgApsdEnAc |= UAPSD_AC1;
    }

    if (bAC2) {
        set->bmfgApsdEnAc |= UAPSD_AC2;
    }

    if (bAC3) {
        set->bmfgApsdEnAc |= UAPSD_AC3;
    }

    if (ucIsEnterPsAtOnce) {
        set->ucIsEnterPsAtOnce = 1;
        wndrv_UAPSD_isEnable = TRUE;
    } else {
        set->ucIsEnterPsAtOnce = 0;
        wndrv_UAPSD_isEnable = FALSE;
    }

    /* Enable/disable UPSD support */
    rSwCtrlInfo.u4Id = 0x1001 << 16;
    rSwCtrlInfo.u4Id |= 0x2;
    rSwCtrlInfo.u4Data = set->ucIsEnterPsAtOnce;
    wlanoidSetSwCtrlWrite(wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen);

    if (ucIsDisableUcTrigger) {
        set->ucIsDisableUcTrigger = 1;
    } else {
        set->ucIsDisableUcTrigger = 0;
    }

#if 0
    /* lcko(NOTE): move to wndrv_ps_ctl_handler */

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetWiFiWmmPsTest,
                       &set,
                       sizeof(PARAM_CUSTOM_WMM_PS_TEST_STRUC_T),
                       (PUINT_32)&setInfoLen);

#endif

    WiFi_EintUnMask();

}

/*******************************************************************************
* FUNCTION
*   wndrv_test_setquery_ant
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_test_setquery_ant(kal_char *inject_string)
{
    kal_int32   u4IsQuery, u4AntPath;
    PTA_IPC_T   PtaIpc;
    UINT_32     fg1WireMode;
    UINT_32     u4Len;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrWrInfo;
    UINT_32                     u4InfoLen;
    //WLAN_STATUS                 status;

    sscanf(inject_string, " %d, %d", &u4IsQuery, &u4AntPath);

    MT5931_NEW_TRACE(MT5931_INFO_555, TRACE_GROUP_10, "wndrv_test_setquery_ant(): query=%d, ant=%d", u4IsQuery, u4AntPath);

    if (u4IsQuery == 1) {   /* query ANT path */
        if (wndrv_context.adpt_p->fgIsAntBT) {
            MT5931_NEW_TRACE0(MT5931_INFO_556, TRACE_GROUP_10, "Current Antenna path is BT");
        } else {
            MT5931_NEW_TRACE0(MT5931_INFO_557, TRACE_GROUP_10, "Current Antenna path is WIFI");
        }
        return;
    }

    PtaIpc.ucCmd = BT_CMD_PROFILE;
    PtaIpc.ucLen = sizeof(PTA_IPC_T);

    switch (u4AntPath) {
        case 0:/* WiFi */
            kal_trace(TRACE_GROUP_10, INFO_WNDRV_OID_ANT_TO_WIFI);
            /* Set PIN mux for ANT_SEL */
            if (wndrv_context.adpt_p->ucRevID >= MT5931_E3_REVERSION_ID) {
                rMcrWrInfo.u4McrOffset = 0x80050180;
            } else {
                rMcrWrInfo.u4McrOffset = 0x80050190;
            }
            rMcrWrInfo.u4McrData   = 0x00100000;
            wlanoidSetMcrWrite(wndrv_context.adpt_p, &rMcrWrInfo, sizeof(rMcrWrInfo), &u4InfoLen);
            kalUdelay(500);
            /* Switch ANT path */
#if 1
            fg1WireMode = (VAR_WMT_CONFIG_ANT_SEL | BIT(0));  //ansel0 Bit0 (WIFI)
#else
            fg1WireMode = (VAR_WMT_CONFIG_WIFI_1WIRE_MODE | BT_EXT_1_WIRE_MODE << 8 | BIT(0));  //ansel0 Bit0 (WIFI)
#endif
            wndrv_context.adpt_p->fgIsAntBT = FALSE;
            break;

        case 1:/* BT */
            kal_trace(TRACE_GROUP_10, INFO_WNDRV_OID_ANT_TO_BT);
            /* Set PIN mux for ANT_SEL */
            if (wndrv_context.adpt_p->ucRevID >= MT5931_E3_REVERSION_ID) {
                rMcrWrInfo.u4McrOffset = 0x80050180;
            } else {
                rMcrWrInfo.u4McrOffset = 0x80050190;
            }
            rMcrWrInfo.u4McrData   = 0x00100000;
            wlanoidSetMcrWrite(wndrv_context.adpt_p, &rMcrWrInfo, sizeof(rMcrWrInfo), &u4InfoLen);
            kalUdelay(500);
            /* Switch ANT path */
#if 1
            fg1WireMode = (VAR_WMT_CONFIG_ANT_SEL | BIT(4));  //ansel1 Bit0 (BT)
#else
            fg1WireMode = (VAR_WMT_CONFIG_WIFI_1WIRE_MODE | BT_EXT_1_WIRE_MODE << 8 | BIT(4));  //ansel1 Bit0 (BT)
#endif
            wndrv_context.adpt_p->fgIsAntBT = TRUE;
            break;

        default:
            return;
    }

    PtaIpc.u.aucBTPParams[3] = (UINT_8) fg1WireMode;
    PtaIpc.u.aucBTPParams[2] = (UINT_8)(fg1WireMode >> 8);
    PtaIpc.u.aucBTPParams[1] = (UINT_8)(fg1WireMode >> 16);
    PtaIpc.u.aucBTPParams[0] = (UINT_8)(fg1WireMode >> 24);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                       (PVOID)&PtaIpc,
                       sizeof(PtaIpc),
                       (PUINT_32)&u4Len);
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_chip_efuse
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_chip_efuse(kal_char *inject_string)
{
    kal_int32   u4IsQuery, u4Addr;
    kal_uint32 u4Value;
    //PTA_IPC_T   PtaIpc;
    //UINT_32     fg1WireMode;
    //UINT_32     u4Len;
    //PARAM_CUSTOM_MCR_RW_STRUC_T rMcrWrInfo;
    //UINT_32                     u4InfoLen;
    //WLAN_STATUS                 status;

    sscanf(inject_string, " %d, %d, %d", &u4IsQuery, &u4Addr, &u4Value);

    if (u4IsQuery == 1) {   /* read efuse */
        RFTool_WiFi_EFUSE_Read(u4Addr, &u4Value);
    } else {                /* write efuse */
        RFTool_WiFi_EFUSE_Write(u4Addr, u4Value);
    }

    MT5931_NEW_TRACE(MT5931_INFO_558, TRACE_GROUP_10, "wndrv_test_chip_efuse(): query=%d, offset=%d, value=%d",
                     u4IsQuery, u4Addr, u4Value);
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_on_off
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_test_on_off(kal_char *inject_string)
{
    kal_int32   u4IsTestMode = 0;
    ilm_struct  *ilm_p;

    sscanf(inject_string, "%d", &u4IsTestMode);

    MT5931_NEW_TRACE(MT5931_INFO_559, TRACE_GROUP_10, "wndrv_test_on_off(): u4IsTestMode=%d", u4IsTestMode);

    if (u4IsTestMode == 1) {
        WNDRVONOFFTEST = KAL_TRUE;

        ilm_p                 = allocate_ilm(MOD_SUPC);
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_INIT_REQ;
        ilm_p->local_para_ptr = NULL;
        ilm_p->peer_buff_ptr  = NULL;

        SEND_ILM(MOD_SUPC, MOD_WNDRV, WNDRV_SUPC_SAP, ilm_p);
        //wndrv_main( ilm_p );
    } else {
        WNDRVONOFFTEST = KAL_FALSE;
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_fw_dl_stress
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_test_fw_dl_stress(kal_char *inject_string)
{
    kal_int32   u4IsTestMode = 0;
    //ilm_struct  *ilm_p;

    P_ADAPTER_T  prAdapter = wndrv_context.adpt_p;
    PVOID        pvFwImageMapFile = wndrv_firmware_p;
    P_REG_INFO_T prRegInfo = &wndrv_context.adpt_p->prGlueInfo->rRegInfo;
    UINT_32      u4FwImageFileLength = wndrv_firmware_size;
    UINT_32      i;
    UINT_32      u4FwLoadAddr, u4ImgSecSize;

    //P_HIF_HW_TX_HEADER_T prHifTxHeader;

    //PUINT_8 pucDmaBuf;
    //UINT_32 u4TotalLen;
    //UINT_32 u4DmaLen;
    //UINT_32 u4DmaOffset;

    MT5931_NEW_TRACE(MT5931_INFO_560, TRACE_GROUP_10, "wndrv_test_fw_dl_stress(): u4IsTestMode=%d", u4IsTestMode);

    sscanf(inject_string, "%d", &u4IsTestMode);

    if (u4IsTestMode == 1) {
        u4FwLoadAddr = prRegInfo->u4LoadAddress;

        while (1) {
            do {
                for (i = 0; i < u4FwImageFileLength ; i += CMD_PKT_SIZE_FOR_IMAGE) {
                    if (i + CMD_PKT_SIZE_FOR_IMAGE < u4FwImageFileLength) {
                        u4ImgSecSize = CMD_PKT_SIZE_FOR_IMAGE;
                    } else {
                        u4ImgSecSize = u4FwImageFileLength - i;
                    }

                    wlanImageSectionDownload_Temp(prAdapter,
                                                  u4FwLoadAddr + i,
                                                  u4ImgSecSize,
                                                  (PUINT_8)pvFwImageMapFile + i);
                }
            } while (0);

            kalMsleep(10);
        }
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_power_on
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_test_power_on(kal_char *inject_string)
{
    kal_int32   u4IsTestMode = 0;

    P_ADAPTER_T  prAdapter = wndrv_context.adpt_p;
    kal_uint32   reg_value;

    MT5931_NEW_TRACE(MT5931_INFO_561, TRACE_GROUP_10, "wndrv_test_power_on(): u4IsTestMode=%d, prAdapter=%d", u4IsTestMode, prAdapter);

    sscanf(inject_string, "%d", &u4IsTestMode);

    if (u4IsTestMode == 1) {
        /* Init power on sequence timing */
        g_pmu_en_delay = 3;    // ms
        g_sys_rst_delay = 12;   // ms

        /* Power on MT5931 */
        LB_WIFI_Chip_PowerOn();

        /****************************************************************************
        *  0. Verify chip ID
        ****************************************************************************/
        MT5931_NEW_TRACE0(MT5931_INFO_438,  TRACE_GROUP_10, "[eHPI test]1. Chip ID/Revision ID");
        HAL_MCR_RD(prAdapter, WCIR, &reg_value);
        MT5931_NEW_TRACE(MT5931_INFO_439,  TRACE_GROUP_10, "[eHPI test]Chip ID/Revision ID = %x", reg_value);
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_power_on_off
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_test_power_on_off(kal_char *inject_string)
{
    kal_int32   u4IsTestMode = 0;
    P_ADAPTER_T  prAdapter = wndrv_context.adpt_p;
    kal_uint32   reg_value;

    MT5931_NEW_TRACE(MT5931_INFO_562, TRACE_GROUP_10, "wndrv_test_power_on_off(): u4IsTestMode=%d,prAdapter=%d", u4IsTestMode, prAdapter);

    sscanf(inject_string, "%d", &u4IsTestMode);

    if (u4IsTestMode == 1) {
        /* Init power on sequence timing */
        g_pmu_en_delay = 3;    // ms
        g_sys_rst_delay = 12;   // ms

        while (1) {
            /* Power on MT5931 */
            LB_WIFI_Chip_PowerOn();

            /****************************************************************************
            *  0. Verify chip ID
            ****************************************************************************/
            MT5931_NEW_TRACE0(MT5931_INFO_440,  TRACE_GROUP_10, "[eHPI test]1. Chip ID/Revision ID");
            HAL_MCR_RD(prAdapter, WCIR, &reg_value);
            MT5931_NEW_TRACE(MT5931_INFO_441,  TRACE_GROUP_10, "[eHPI test]Chip ID/Revision ID = %x", reg_value);

            kalMsleep(20);

            /* Power off MT5931 */
            LB_WIFI_Chip_PowerOff();
        }
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_set_rx_event_port
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_rx_event_port(kal_char *inject_string)
{
    kal_int32   u4RxEventPortNum;

    sscanf(inject_string, " %d", &u4RxEventPortNum);

    MT5931_NEW_TRACE0(MT5931_INFO_563, TRACE_GROUP_10, "wndrv_test_set_rx_event_port()");

    g_hif_rx_event_port_num = u4RxEventPortNum;
}

static void wndrv_test_set_scan_desired_ssid_list(kal_char *inject_string)
{
#if CFG_ENABLE_SET_SCAN_DESIRED_SSID_LIST
    kal_uint32  fgIsAddorDel = 0;

    wndrv_abm_set_scan_desired_ssid_list_req_struct *req_ptr;
    ilm_struct  *ilm_p;

    sscanf(inject_string, "%d", &fgIsAddorDel);

    req_ptr = (wndrv_abm_set_scan_desired_ssid_list_req_struct *)
              construct_local_para(sizeof(wndrv_abm_set_scan_desired_ssid_list_req_struct), TD_CTRL);

    DEBUGFUNC("wndrv_test_set_scan_desired_ssid_list");

    memset(&req_ptr->ssid_list[0], 0, sizeof(abm_wndrv_ssid_struct)*WNDRV_MAX_SSID_LIST_NUM);

    if (fgIsAddorDel) { //add

        req_ptr->ssid_list[0].ssid_len = strlen("TEST");
        strncpy((char *)req_ptr->ssid_list[0].ssid, "TEST", req_ptr->ssid_list[0].ssid_len);

        req_ptr->ssid_list[1].ssid_len = strlen("mtkadb");
        strncpy((char *)req_ptr->ssid_list[1].ssid, "WZL", req_ptr->ssid_list[1].ssid_len);
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_5, " add ssid len:%d, ssid:%s", req_ptr->ssid_list[1].ssid_len, req_ptr->ssid_list[1].ssid);

        req_ptr->ssid_list[2].ssid_len = strlen("mtkguest");
        strncpy((char *)req_ptr->ssid_list[2].ssid, "mtkguest", req_ptr->ssid_list[2].ssid_len);

        req_ptr->ssid_list[3].ssid_len = strlen("mtklab");
        strncpy((char *)req_ptr->ssid_list[3].ssid, "mtklab", req_ptr->ssid_list[3].ssid_len);


        req_ptr->ssid_list[4].ssid_len = strlen("LG");
        strncpy((char *)req_ptr->ssid_list[4].ssid, "LG", req_ptr->ssid_list[4].ssid_len);

        req_ptr->ssid_list_num = 5;

    } else { //delete
        req_ptr->ssid_list_num = 0;
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_GROUP_5, "delete all ssid list");
    }


    ilm_p                 = allocate_ilm(MOD_ABM);
    ilm_p->msg_id         = MSG_ID_WNDRV_ABM_SET_SCAN_DESIRED_SSID_LIST_REQ;
    ilm_p->local_para_ptr = (local_para_struct *)req_ptr;
    ilm_p->peer_buff_ptr  = NULL;

    SEND_ILM(MOD_ABM, MOD_WNDRV, SUPC_ABM_SAP, ilm_p);
#endif
}


#if 0
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_cont_tx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_cont_tx_req(kal_char *inject_string)
{
    wndrv_test_tx_struct cont_tx;
    kal_uint8            channel_num, tx_rate;
    kal_uint16           domain = WNDRV_COUNTRY_CODE_US;

    sscanf(inject_string, "%d , %d", &channel_num, &tx_rate);

    cont_tx.ch_freq = freqTable24[channel_num];
    cont_tx.tx_gain_dac = 0x26;
    cont_tx.txAnt = WNDRV_RX_ANT_DIVERSITY_AGC;
    cont_tx.tx_rate = tx_rate;

    domain = (((kal_uint16)(domain & 0x00ff) << 8) | ((kal_uint16)(domain & 0xff00) >> 8));
    RFTool_WiFi_ContTx((kal_uint8 *)&domain, cont_tx);

}
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_local_freq_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_local_freq_req(kal_char *inject_string)
{
    wndrv_test_tx_struct local_freq;
    kal_uint8            channel_num;
    kal_uint16           domain = WNDRV_COUNTRY_CODE_US;

    sscanf(inject_string, "%d", &channel_num);

    local_freq.ch_freq = freqTable24[channel_num];
    local_freq.tx_gain_dac = 0x26;
    local_freq.txAnt = WNDRV_RX_ANT_DIVERSITY_AGC;

    domain = (((kal_uint16)(domain & 0x00ff) << 8) | ((kal_uint16)(domain & 0xff00) >> 8));
    RFTool_WiFi_LocalFreq((kal_uint8 *)&domain, local_freq);
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_carrier_supp_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_carrier_supp_req(kal_char *inject_string)
{
    wndrv_test_tx_struct carrier_supp;
    kal_uint8            channel_num, tx_rate;
    kal_uint16           domain = WNDRV_COUNTRY_CODE_US;

    sscanf(inject_string, "%d , %d", &channel_num, &tx_rate);

    carrier_supp.ch_freq = freqTable24[channel_num];
    carrier_supp.tx_gain_dac = 0x26;
    carrier_supp.txAnt = WNDRV_RX_ANT_DIVERSITY_AGC;
    carrier_supp.tx_rate = tx_rate;

    domain = (((kal_uint16)(domain & 0x00ff) << 8) | ((kal_uint16)(domain & 0xff00) >> 8));
    RFTool_WiFi_CarrierSupp((kal_uint8 *)&domain, carrier_supp);
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_pkt_tx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_pkt_tx_req(kal_char *inject_string)
{
    wndrv_test_pkt_tx_structEX pkt_tx;
    kal_uint8                  channel_num, tx_rate, pattern;
    kal_uint32                 pktCount, pktLength;
    kal_uint32                 txFlags, targetAlc;
    kal_uint16                 domain = WNDRV_COUNTRY_CODE_US;
    kal_uint8  mac_header[24] = {0x08, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
                                 0xFF, 0xFF, 0x00, 0x08, 0x22, 0x00, 0x00, 0x01,
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0x00
                                };
    sscanf(inject_string, "%d , %d , %d , %d , %d, %d, %d",
           &channel_num, &tx_rate, &pktCount, &pktLength, &pattern, &txFlags, &targetAlc);

    pkt_tx.ch_freq = freqTable24[channel_num];
    pkt_tx.tx_rate = tx_rate;
    pkt_tx.tx_gain_dac = 0x26;
    pkt_tx.pktCount = pktCount;
    pkt_tx.pktInterval = 0;
    pkt_tx.pktLength = pktLength;
    pkt_tx.pattern = WNDRV_TX_ALL_ZEROS;
    pkt_tx.is_short_preamble = 1;
    pkt_tx.txFlags = txFlags;
    pkt_tx.targetAlc = targetAlc;
    pkt_tx.txAnt = WNDRV_RX_ANT_DIVERSITY_AGC;
    kal_mem_cpy(pkt_tx.mac_header, mac_header, 24);

    domain = (((kal_uint16)(domain & 0x00ff) << 8) | ((kal_uint16)(domain & 0xff00) >> 8));
    RFTool_WiFi_PktTxEX((kal_uint8 *)&domain, pkt_tx);
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_pkt_rx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_pkt_rx_req(kal_char *inject_string)
{
    kal_uint8 channel_num;
    sscanf(inject_string, "%d", &channel_num);

    //RFTool_WiFi_PktRx(channel_num, void (* callback) (wndrv_test_rx_status_struct* result))

}
/*******************************************************************************
* FUNCTION
*   wndrv_test_set_stop_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_stop_req(kal_char *inject_string)
{
    RFTool_WiFi_Stop();
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_set_reg_domain_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_set_reg_domain_req(kal_char *inject_string)
{
    ilm_struct                                *ilm_p;
    wndrv_test_set_req_struct     *req_p;


    /*  req_p = (wndrv_test_set_req_struct *)
            construct_local_para( sizeof( wndrv_test_set_req_struct ), TD_CTRL );*/
    req_p = (wndrv_test_set_req_struct *)
            construct_local_para(sizeof(wndrv_test_set_req_struct), TD_CTRL);
    req_p->token = 0;
    req_p->type = WNDRV_TEST_SET_REG_DOMAIN;    /* A list of set commands */
    kal_mem_cpy(req_p->data.country_code, inject_string, 2);

    ilm_p                 = allocate_ilm(MOD_NIL);
    ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
    ilm_p->local_para_ptr = (local_para_struct *) req_p;
    ilm_p->peer_buff_ptr  = NULL;

    wndrv_test_set_handler(ilm_p);
    //free_ilm(ilm_p);
    free_ilm(ilm_p);
    //cancel_ilm(MOD_NIL);
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_mcr32_rw_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_mcr32_rw_req(kal_char *inject_string)
{
    kal_uint32                    read, mcr32_index, mcr32_data;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;
    UINT_32 u4BufLen;

    sscanf(inject_string, " %d , %x , %x ", &read, &mcr32_index, &mcr32_data);
    if (read == 0) {
        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_SET_MCR32,
                     "SET_MCR32: idx=0x%x, data=0x%x",
                     mcr32_index,
                     mcr32_data);
        rMcrInfo.u4McrOffset = mcr32_index;
        rMcrInfo.u4McrData = mcr32_data;

        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wlanSetInformation(wndrv_context.adpt_p,
                           wlanoidSetMcrWrite,
                           (PVOID)&rMcrInfo,
                           sizeof(rMcrInfo),
                           &u4BufLen);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
    } else {
        rMcrInfo.u4McrOffset = mcr32_index;
        GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        wlanQueryInformation(wndrv_context.adpt_p,
                             wlanoidQueryMcrRead,
                             (PVOID)&rMcrInfo,
                             sizeof(rMcrInfo),
                             &u4BufLen);

        GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

        mcr32_data = rMcrInfo.u4McrData;

        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_QUERY_MCR32,
                     "QUERY_MCR32: idx=0x%x, data=0x%x",
                     mcr32_index,
                     mcr32_data);
    }
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_mcr16_rw_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_mcr16_rw_req(kal_char *inject_string)
{
#if 0
    kal_uint32                     read, mcr16_index;
    kal_uint16                    mcr16_data;
    sscanf(inject_string, " %d , %x , %x ", &read, &mcr16_index, &mcr16_data);
    if (read == 0) {
        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_SET_MCR16,
                     "SET_MCR16: idx=0x%x, data=0x%x",
                     mcr16_index,
                     mcr16_data);

        RFTool_WiFi_MCR16_Write(mcr16_index, mcr16_data);
    } else {
        RFTool_WiFi_MCR16_Read(mcr16_index, &mcr16_data);

        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_QUERY_MCR16,
                     "QUERY_MCR16: idx=0x%x, data=0x%x",
                     mcr16_index,
                     mcr16_data);
    }
#endif
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_bbcr_rw_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_bbcr_rw_req(kal_char *inject_string)
{
#if 0
    kal_uint32                 read, bbcr_index;
    kal_uint8                  bbcr_data;

    sscanf(inject_string, " %d , %x , %x ", &read, &bbcr_index, &bbcr_data);

    if (read == 0) {
        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_SET_BBCR,
                     "SET_BBCR: idx=%d, data=0x%x",
                     bbcr_index,
                     bbcr_data);

        RFTool_WiFi_BBCR_Write(bbcr_index, bbcr_data);
    } else {
        RFTool_WiFi_BBCR_Read(bbcr_index, &bbcr_data);

        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_QUERY_BBCR,
                     "QUERY_BBCR: idx=%d, data=0x%x",
                     bbcr_index,
                     bbcr_data);
    }
#endif
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_eeprom_rw_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_eeprom_rw_req(kal_char *inject_string)
{
    kal_uint32                     read, eeprom_index;
    kal_uint16                    eeprom_data;

    sscanf(inject_string, " %d , %x , %x ", &read, &eeprom_index, &eeprom_data);
    if (read == 0) {
        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_SET_EEPROM,
                     "SET_EEPROM: idx=0x%x, data=0x%x",
                     eeprom_index,
                     eeprom_data);

        RFTool_WiFi_EEPROM_Write(eeprom_index, eeprom_data);
    } else {
        RFTool_WiFi_EEPROM_Read(eeprom_index, &eeprom_data);

        WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_QUERY_EEPROM,
                     "QUERY_EEPROM: idx=0x%x, data=0x%x",
                     eeprom_index,
                     eeprom_data);
    }
}
/*******************************************************************************
* FUNCTION
*   wndrv_test_query_pkt_tx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_query_pkt_tx_req(kal_char *inject_string)
{
    ilm_struct                                *ilm_p;
    wndrv_test_query_req_struct   *req_p;

    /*  req_p = (wndrv_test_query_req_struct *)
            construct_local_para( sizeof( wndrv_test_query_req_struct ), TD_CTRL );*/
    req_p = (wndrv_test_query_req_struct *)
            construct_local_para(sizeof(wndrv_test_query_req_struct), TD_CTRL);
    req_p->token = 0;
    req_p->type = WNDRV_TEST_QUERY_PKT_TX;    /* A list of set commands */


    ilm_p                 = allocate_ilm(MOD_NIL);   //MOD_NIL
    ilm_p->msg_id         = MSG_ID_WNDRV_TEST_QUERY_REQ;
    ilm_p->local_para_ptr = (local_para_struct *) req_p;
    ilm_p->peer_buff_ptr  = NULL;

    wndrv_test_set_handler(ilm_p);
    //free_ilm(ilm_p);
    free_ilm(ilm_p);
    //cancel_ilm(MOD_NIL);
}

/*******************************************************************************
* FUNCTION
*   wndrv_test_query_pkt_rx_req
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_test_query_pkt_rx_req(kal_char *inject_string)
{
    ilm_struct                                *ilm_p;
    wndrv_test_query_req_struct   *req_p;

    /*  req_p = (wndrv_test_query_req_struct *)
            construct_local_para( sizeof( wndrv_test_query_req_struct ), TD_CTRL );*/
    req_p = (wndrv_test_query_req_struct *)
            construct_local_para(sizeof(wndrv_test_query_req_struct), TD_CTRL);
    req_p->token = 0;
    req_p->type = WNDRV_TEST_QUERY_PKT_RX;    /* A list of set commands */

    ilm_p                 = allocate_ilm(MOD_NIL);
    ilm_p->msg_id         = MSG_ID_WNDRV_TEST_QUERY_REQ; //MSG_ID_WNDRV_TEST_QUERY_REQ
    ilm_p->local_para_ptr = (local_para_struct *) req_p;
    ilm_p->peer_buff_ptr  = NULL;

    wndrv_test_set_handler(ilm_p);
    //free_ilm(ilm_p);
    free_ilm(ilm_p);
    //cancel_ilm(MOD_NIL);
}
/*******************************************************************************
* FUNCTION
*   wndrv_set_fixed_rate_req
*
* DESCRIPTION
*   Set fixed transmission rate
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_set_fixed_rate_req(kal_char *inject_string)
{

    kal_int32   status;
    kal_uint8   set[2], rate_idx;

    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    sscanf(inject_string, " %d ", &rate_idx);

    set[0] = rate_idx2value[ rate_idx ];
    set[1] = 0x00;

    data.u4FuncIndex = RF_AT_FUNCID_RATE;
    data.u4FuncData = (kal_uint32)rate_idx;

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_RATE, "SET_RATE: %d", set[0]);

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    status = wlanSetInformation(prAdapter,
                                (PFN_OID_HANDLER_FUNC)arbFsmRunEventRftestSetAutoTest,
                                (PVOID)&data,
                                sizeof(data),
                                (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    WNDRV_EXT_ASSERT(status == 0, "set_rate: txrateSetDesiredRates fail", status, 0, 0);

}
/*******************************************************************************
* FUNCTION
*   wndrv_set_delay_req
*
* DESCRIPTION
*   delay function (time unit: marco seconds)
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_set_delay_req(kal_char *inject_string)
{
    kal_uint32   udelay;
    sscanf(inject_string, " %d ", &udelay);
    kalUdelay(udelay);
}


/*******************************************************************************
* FUNCTION
*   wndrv_set_PowerMode
*
* DESCRIPTION
*   Set Power Mode function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_set_PowerMode(kal_char *inject_string)
{
    PARAM_POWER_MODE pm = Param_PowerModeMAX_PSP;
    kal_uint32              uPowerMode, setInfoLen, uSavedMask;
    uSavedMask = WiFi_EintMask();

    sscanf(inject_string, " %d ", &uPowerMode);
    wndrv_bus_power_control(1);
    wndrv_context.glue_p->fgIsForceActive = FALSE;

    switch (uPowerMode) {
        case 0:
            pm = Param_PowerModeCAM;
            wndrv_context.glue_p->ePSMode = ENUM_PSP_CONTINUOUS_ACTIVE;
            wndrv_context.glue_p->fgIsForceActive = TRUE;
            break;
        case 1:
            pm = Param_PowerModeMAX_PSP;
            wndrv_context.glue_p->ePSMode = ENUM_PSP_CONTINUOUS_POWER_SAVE;
            break;
        case 2:
            pm = Param_PowerModeFast_PSP;
            wndrv_context.glue_p->ePSMode = ENUM_PSP_FAST_SWITCH;
            wndrv_context.glue_p->eSWMode = PARAM_PSP_FAST_SWITCH_TIME;
            break;
        case 3:
            pm = Param_PowerModeFast_PSP;
            wndrv_context.glue_p->ePSMode = ENUM_PSP_FAST_SWITCH;
            wndrv_context.glue_p->eSWMode = PARAM_PSP_FAST_SWITCH_DIRECT;
            break;
        case 4: /* for CTIA test (Nelson) */
            RFTool_WiFi_PwrMgt(WNDRV_POWER_MODE_CONTINUOUS_ACTIVE);
            break;
        case 5: /* for Normal mode test (Nelson) */
            RFTool_WiFi_PwrMgt(WNDRV_POWER_MODE_NORMAL);
            break;
    }

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", uPowerMode);
    if ((uPowerMode != 4) || (uPowerMode != 5)) {
        wlanSetInformation(wndrv_context.adpt_p,
                           (PFN_OID_HANDLER_FUNC)wlanoidSet802dot11PowerSaveProfile,
                           &pm,
                           sizeof(PARAM_POWER_MODE),
                           (PUINT_32)&setInfoLen);
    }

    WiFi_EintUnMask();
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_CloseMsg
*
* DESCRIPTION
*   Close Msg function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_set_CloseMsg(kal_char *inject_string)
{
    kal_uint32   uTmp;
    sscanf(inject_string, " %d ", &uTmp);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "", uTmp);
    gMsgCtl = uTmp;
    //MT5931_NEW_TRACE( MT5931_INFO_5, 0x02, "macState:%d\n", wndrv_context.adpt_p->rArbInfo.eCurrentState);
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_Misc
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
extern UINT_32 u4IbssCh;
volatile kal_bool Stop_Gen = FALSE;
extern kal_uint8 force_reset;
extern void wlanDevMonitorFun(void);
static void wndrv_set_Misc(kal_char *inject_string)
{
    kal_int32   uTmp, uTmp1;
    UINT_32     u4Value, u4Len;
    sscanf(inject_string, " %d , %d ", &uTmp, &uTmp1);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", uTmp);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", uTmp1);
    if (uTmp == 1) {
        if (uTmp1 > 14) {
            u4IbssCh = 15;
        } else {
            u4IbssCh = uTmp1;
        }
    } else if (uTmp == 2) {
        if (uTmp1 == 1) {
            Stop_Gen = TRUE;
        } else {
            Stop_Gen = FALSE;
        }
    }
#if CFG_SINGLE_ANTENNA_ENABLED
    else if (uTmp == 3) { // set Ant path
        if (uTmp1 == 1) {
            u4Value = 1;
            //4 <2> set ant
            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSetPreferredAnt,
                               (PVOID)&u4Value,
                               sizeof(u4Value),
                               (PUINT_32)&u4Len);
        } else {
            u4Value = 0;
            //4 <2> set ant
            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSetPreferredAnt,
                               (PVOID)&u4Value,
                               sizeof(u4Value),
                               (PUINT_32)&u4Len);
        }
    } else if (uTmp == 4) {
        /*update MAC */
        wndrv_context.glue_p->permanentMacAddress[MAC_ADDR_LEN - 1] = uTmp1;
        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "INJECT: MAC: "MACSTR,
                     MAC2STR(wndrv_context.glue_p->permanentMacAddress));
    }
#endif /*#if CFG_SINGLE_ANTENNA_ENABLED*/
    else if (uTmp == 5) {
        /* debug */
        force_reset = 1;
        wlanDevMonitorFun();
        force_reset = 0;
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_Join
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_set_Join(kal_char *inject_string)
{
    kal_uint32   NetworkType, ssid_len;
    kal_uint8    ssid[32];
    ilm_struct   *ilm_p;
    wndrv_supc_join_network_req_struct  *req_p;

    sscanf(inject_string, " %d,%d,%s ", &NetworkType, &ssid_len, &ssid);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", NetworkType);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", ssid_len);

    /*req_p = (wndrv_supc_join_network_req_struct *)
            construct_local_para( sizeof( wndrv_supc_join_network_req_struct ), TD_CTRL );*/
    req_p = (wndrv_supc_join_network_req_struct *)
            construct_local_para(sizeof(wndrv_supc_join_network_req_struct), TD_CTRL);
    req_p->network_type     = NetworkType;
    req_p->channel_number   = 1;
    req_p->auth_mode        = WNDRV_SUPC_AUTH_MODE_OPEN;
    req_p->encryption       = WNDRV_SUPC_ENCRYPTION_DISABLED;
    req_p->key_enable       = 0; //0x0F;

    req_p->ssid_len = ssid_len;
    kal_mem_cpy(req_p->ssid , ssid, ssid_len);

    ilm_p                 = allocate_ilm(MOD_NIL);   //MOD_NIL
    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_JOIN_NETWORK_REQ; //MSG_ID_WNDRV_SUPC_JOIN_NETWORK_REQ
    ilm_p->local_para_ptr = (local_para_struct *) req_p;
    ilm_p->peer_buff_ptr  = NULL;

    wndrv_supc_join_network_req_hanlder(ilm_p);
    //free_ilm(ilm_p);
    free_ilm(ilm_p);
    //cancel_ilm(MOD_NIL);
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_roaming
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_set_roaming(kal_char *inject_string)
{
#if 0
    PARAM_ROAM_CONFIG   config;
    UINT_32 len;
    kal_int32 roamType, roamOutNumOfLostBeacon, minScanIntervalLostConnection;
    kal_int32 roamOutTxRetryLimit;
    kal_int32 minRssiCalDuration, minRoamOutStayDuration, roamOutRssiTrigger;


    sscanf(inject_string, "%d , %d , %d , %d , %d , %d, %d",
           &roamType,
           &roamOutNumOfLostBeacon,
           &minScanIntervalLostConnection,
           &roamOutTxRetryLimit,
           &minRssiCalDuration,
           &minRoamOutStayDuration,
           &roamOutRssiTrigger);

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", roamType);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", roamOutNumOfLostBeacon);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", minScanIntervalLostConnection);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", minRssiCalDuration);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", minRoamOutStayDuration);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", roamOutRssiTrigger);

    config.Length = sizeof(PARAM_ROAM_CONFIG);
    if (1 == roamType) {
        config.roamFlag = ROAM_OUT_FLAG_LOST_BEACON;
        config.roamOutNumOfLostBeacon = roamOutNumOfLostBeacon;
        config.minScanIntervalLostConnection = minScanIntervalLostConnection;
        wndrv_context.adpt_p->bgScanConfigEnabled = FALSE;
    } else if (2 == roamType) {
        config.roamFlag = ROAM_OUT_FLAG_RETRY_FAIL;
        config.roamOutNumOfLostBeacon = roamOutNumOfLostBeacon;
        config.minScanIntervalLostConnection = minScanIntervalLostConnection;
        config.roamOutTxRetryLimit = roamOutTxRetryLimit;
        wndrv_context.adpt_p->bgScanConfigEnabled = TRUE;

    } else if (3 == roamType) {
        config.roamFlag = ROAM_OUT_FLAG_RSSI_THRESHOLD;
        config.roamOutNumOfLostBeacon = roamOutNumOfLostBeacon;
        config.minScanIntervalLostConnection = minScanIntervalLostConnection;
        config.roamOutTxRetryLimit = roamOutTxRetryLimit;
        config.minRssiCalDuration = minRssiCalDuration;
        config.minRoamOutStayDuration = minRoamOutStayDuration;
        config.roamOutRssiTrigger = roamOutRssiTrigger;
        wndrv_context.adpt_p->bgScanConfigEnabled = TRUE;
    } else if (4 == roamType) {
        config.roamFlag = ROAM_OUT_FLAG_LOST_BEACON | ROAM_OUT_FLAG_RETRY_FAIL | ROAM_OUT_FLAG_RSSI_THRESHOLD;
        config.roamOutNumOfLostBeacon = roamOutNumOfLostBeacon;
        config.minScanIntervalLostConnection = minScanIntervalLostConnection;
        config.roamOutTxRetryLimit = roamOutTxRetryLimit;
        config.minRssiCalDuration = minRssiCalDuration;
        config.minRoamOutStayDuration = minRoamOutStayDuration;
        config.roamOutRssiTrigger = roamOutRssiTrigger;
    }
    wlanoidSetRoamPolicy(wndrv_context.adpt_p, &config, sizeof(PARAM_ROAM_CONFIG), &len);
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_linkQuality
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_set_linkQuality(kal_char *inject_string)
{
#if 0
    kal_int32   uTmp, uTmp1;

    sscanf(inject_string, " %d , %d ", &uTmp, &uTmp1);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", uTmp);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", uTmp1);
    if (uTmp == 1) {
        wndrv_context.adpt_p->roamingRssiHighThreshold = uTmp1;
    } else if (uTmp == 2) {
        wndrv_context.adpt_p->roamingRssiLowThreshold = uTmp1;
    } else if (uTmp == 3) {
        wndrv_context.adpt_p->scanRssiHighThreshold = uTmp1;
    } else if (uTmp == 4) {
        wndrv_context.adpt_p->scanRssiLowThreshold = uTmp1;
    } else if (uTmp == 5) {
        wndrv_context.adpt_p->highThresholdSup = uTmp1;
    } else if (uTmp == 6) {
        wndrv_context.adpt_p->lowThresholdSup = uTmp1;
    } else if (uTmp == 7) {
        wndrv_context.adpt_p->initScanInterval = uTmp1;
    } else if (uTmp == 8) {
        wndrv_context.adpt_p->highThresholdScanInterval = uTmp1;
    } else if (uTmp == 9) {
        wndrv_context.adpt_p->lowThresholdScanInterval = uTmp1;
    } else if (uTmp == 10) {
        wndrv_context.adpt_p->highThresholdScanCount = uTmp1;
    } else if (uTmp == 11) {
        wndrv_context.adpt_p->lowThresholdScanCount = uTmp1;
    } else if (uTmp == 12) {
        wndrv_context.adpt_p->idleRoamingEnable = uTmp1;
    }
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_uAPSD
*
* DESCRIPTION
*   Misc function
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_set_uAPSD(kal_char *inject_string)
{

    kal_int32   ucIsEnterPsAtOnce, ucIsDisableUcTrigger, bAC0, bAC1, bAC2, bAC3; //lcko: default value is  1,0,1,1,1,1
    P_PARAM_CUSTOM_WMM_PS_TEST_STRUC_T set = &wndrv_context.glue_p->UAPSDSetting;
    //PARAM_POWER_MODE pm = Param_PowerModeMAX_PSP;
    kal_uint32             uSavedMask;

    uSavedMask = WiFi_EintMask();
    wndrv_bus_power_control(1);

    sscanf(inject_string, " %d, %d, %d, %d, %d, %d", &ucIsEnterPsAtOnce, &ucIsDisableUcTrigger, &bAC0, &bAC1, &bAC2, &bAC3);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", ucIsEnterPsAtOnce);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", ucIsDisableUcTrigger);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC0);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC1);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC2);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%d ******", bAC3);

    set->bmfgApsdEnAc = 0;

    if (bAC0) {
        set->bmfgApsdEnAc |= UAPSD_AC0;
    }

    if (bAC1) {
        set->bmfgApsdEnAc |= UAPSD_AC1;
    }

    if (bAC2) {
        set->bmfgApsdEnAc |= UAPSD_AC2;
    }

    if (bAC3) {
        set->bmfgApsdEnAc |= UAPSD_AC3;
    }

    if (ucIsEnterPsAtOnce) {
        set->ucIsEnterPsAtOnce = 1;
        wndrv_UAPSD_isEnable = TRUE;
    } else {
        set->ucIsEnterPsAtOnce = 0;
        wndrv_UAPSD_isEnable = FALSE;
    }

    if (ucIsDisableUcTrigger) {
        set->ucIsDisableUcTrigger = 1;
    } else {
        set->ucIsDisableUcTrigger = 0;
    }
#if 0
    /* lcko(NOTE): move to wndrv_ps_ctl_handler */

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetWiFiWmmPsTest,
                       &set,
                       sizeof(PARAM_CUSTOM_WMM_PS_TEST_STRUC_T),
                       (PUINT_32)&setInfoLen);

#endif

    WiFi_EintUnMask();

}


static void wndrv_set_btcer(kal_char *inject_string)
{

    PARAM_CUSTOM_BT_COEXIST_T set;
    kal_uint32             setInfoLen;

    sscanf(inject_string, " %u, %u, %u, %u, %u, %u",
           &set.rPtaParam.u4BtCR0,
           &set.rPtaParam.u4BtCR1,
           &set.rPtaParam.u4BtCR2,
           &set.rPtaParam.u4BtCR3,
           &set.u4IsEnableTxAutoFragmentForBT,
           &set.eBTCoexistWindowType);

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", set.rPtaParam.u4BtCR0);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", set.rPtaParam.u4BtCR1);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", set.rPtaParam.u4BtCR2);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", set.rPtaParam.u4BtCR3);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", set.u4IsEnableTxAutoFragmentForBT);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", set.eBTCoexistWindowType);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetBtCoexistCtrl,
                       &set,
                       sizeof(PARAM_CUSTOM_BT_COEXIST_T),
                       (PUINT_32)&setInfoLen);

}

static void wndrv_set_ip(kal_char *inject_string)
{

    kal_uint32             set;
    INT_32                 a, b, c, d;
    kal_uint32             setInfoLen;

    sscanf(inject_string, " %d, %d, %d, %d ",
           &a,
           &b,
           &c,
           &d
          );

    set = (((0xff & d) << 24) | ((0xff & c) << 16) | ((0xff & b) << 8) | (0xff & a));

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", a);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", b);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", c);
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END, "SET_END: successful=%u ******", d);

    wlanSetInformation(wndrv_context.adpt_p,
                       (PFN_OID_HANDLER_FUNC)wlanoidSetIpAddress,
                       &set,
                       sizeof(kal_uint32),
                       (PUINT_32)&setInfoLen);

}

#if defined(__HOTSPOT_SUPPORT__)
/*******************************************************************************
* FUNCTION
*   wndrv_send_msg_to_hostap
*
* DESCRIPTION
*   MSG_ID_HOSTAP_WNDRV_INIT_REQ
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_hostap_send_msg_ut(msg_type msg_name, void *param_ptr, void *peer_buff_ptr)
{
    ilm_struct *ilm_ptr = NULL;

    /* First allocate a memory for an interlayer message structure */
    ilm_ptr = allocate_ilm(MOD_WNDRV);

    ilm_ptr->msg_id = (msg_type) msg_name;    /* Set the message id */
    if (peer_buff_ptr != NULL) {
        ilm_ptr->peer_buff_ptr = NULL;
    } else {
        ilm_ptr->peer_buff_ptr = (peer_buff_struct *)peer_buff_ptr;
    }
    ilm_ptr->local_para_ptr = (local_para_struct *) param_ptr;
    ilm_ptr->src_mod_id = MOD_WNDRV;
    ilm_ptr->dest_mod_id = MOD_WNDRV;

    msg_send_ext_queue(ilm_ptr);
    return;
}

/*******************************************************************************
* FUNCTION
*   wndrv_tst_inject_string_handler
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void wndrv_hostap_ut(kal_char *inject_string)
{
    char str[20];
    UINT_8      val1;
    UINT_32     u4Value, u4Len;
    MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "[APNIC] wndrv_hostap_ut");
    sscanf(inject_string, " %s %d", str, &val1);

    if (strncmp(str, "INIT", 4) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_INIT_REQ */
        hostap_wndrv_init_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_init_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_init_req_struct), TD_CTRL);
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_INIT_REQ, req_ptr, NULL);
    } else if (strncmp(str, "DEINIT", 6) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_DEINIT_REQ */
        hostap_wndrv_deinit_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_deinit_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_deinit_req_struct), TD_CTRL);
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_DEINIT_REQ, req_ptr, NULL);
    } else if (strncmp(str, "SETESSID", 8) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_ESSID_REQ */
        UINT_8 auc[8] = "HOSTAPUT";

        hostap_wndrv_set_essid_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_essid_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_essid_req_struct), TD_CTRL);

        kalMemCopy(req_ptr->essid, auc, 8);
        req_ptr->essid_flag = 1; /* 0 is meanless */
        req_ptr->essid_len = 8;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_ESSID_REQ , req_ptr, NULL);

    } else if (strncmp(str, "SET1X", 5) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_8021X_REQ */
        hostap_wndrv_set_8021x_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_8021x_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_8021x_req_struct), TD_CTRL);

        req_ptr->fgIs1x = (kal_bool)val1;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_8021X_REQ , req_ptr, NULL);

    } else if (strncmp(str, "SETHOSTDEC", 10) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_HOST_DECRYPT_REQ */
        hostap_wndrv_set_host_decrypt_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_host_decrypt_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_host_decrypt_req_struct), TD_CTRL);

        req_ptr->fgIsDecrypt = (kal_bool)val1;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_HOST_DECRYPT_REQ , req_ptr, NULL);
    } else if (strncmp(str, "SETHOSTENC", 10) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_HOST_ENCRYPT_REQ */
        hostap_wndrv_set_host_encrypt_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_host_encrypt_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_host_encrypt_req_struct), TD_CTRL);

        req_ptr->fgIsEncrypt = (kal_bool)val1;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_HOST_ENCRYPT_REQ , req_ptr, NULL);

    } else if (strncmp(str, "SETPRIV", 7) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_PRIVACY_REQ */
        hostap_wndrv_set_privacy_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_privacy_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_privacy_req_struct), TD_CTRL);

        req_ptr->fgIsPrivacy = (kal_bool)val1;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_PRIVACY_REQ , req_ptr, NULL);
    } else if (strncmp(str, "SETELEM", 7) == 0) {
        UINT_8 aucElem[4] = {0x0, 0x1, 0x2, 0x3};
        /* MSG_ID_HOSTAP_WNDRV_SET_GENERIC_ELEM_REQ */
        hostap_wndrv_set_generic_elem_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_generic_elem_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_generic_elem_req_struct), TD_CTRL);

        kalMemCopy(req_ptr->data, aucElem, 4);
        req_ptr->len = 4;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_GENERIC_ELEM_REQ , req_ptr, NULL);
    } else if (strncmp(str, "SETENC", 6) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_ENCRYPTION_REQ */
        /* set security profile */
        hostap_wndrv_set_encryption_req_struct *req_ptr;
        UINT_8 aucAlg[4] = "none";
        req_ptr = (hostap_wndrv_set_encryption_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_encryption_req_struct), TD_CTRL);

        kalMemSet(req_ptr->sta_addr, 0xFF, 6);
        req_ptr->alg_len = 4;
        kalMemCopy(req_ptr->alg, aucAlg, 4);
        req_ptr->flags = 0;
        req_ptr->idx = 0;
        req_ptr->key_len = 0;
        kalMemZero(req_ptr->key, MAX_KEY_LEN);
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_ENCRYPTION_REQ , req_ptr, NULL);
    } else if (strncmp(str, "FLUSH", 5) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_FLUSH_REQ */
        hostap_wndrv_flush_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_flush_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_flush_req_struct), TD_CTRL);

        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_FLUSH_REQ , req_ptr, NULL);

    } else if (strncmp(str, "ADDSTA", 6) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_ADD_STA_REQ */
        UINT_8 aucMac[6] = {0x0, 0x0C, 0xE7, 0x12, 0x34, 0x56};
        hostap_wndrv_add_sta_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_add_sta_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_add_sta_req_struct), TD_CTRL);

        req_ptr->aid = 1;
        req_ptr->capability = CAP_INFO_ESS;
        req_ptr->tx_supp_rates = 0;
        COPY_MAC_ADDR(req_ptr->sta_addr, aucMac);
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_ADD_STA_REQ , req_ptr, NULL);
    } else if (strncmp(str, "DELSTA", 6) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_REMOVE_STA_REQ */
        UINT_8 aucMac[6] = {0x0, 0x0C, 0xE7, 0x12, 0x34, 0x56};
        hostap_wndrv_remove_sta_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_remove_sta_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_remove_sta_req_struct), TD_CTRL);

        COPY_MAC_ADDR(req_ptr->sta_addr, aucMac);
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_REMOVE_STA_REQ , req_ptr, NULL);
    } else if (strncmp(str, "GETSTAINFO", 10) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_GET_STA_INFO_REQ */
        UINT_8 aucMac[6] = {0x0, 0x0C, 0xE7, 0x12, 0x34, 0x56};
        hostap_wndrv_get_sta_info_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_get_sta_info_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_get_sta_info_req_struct), TD_CTRL);

        COPY_MAC_ADDR(req_ptr->sta_addr, aucMac);
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_GET_STA_INFO_REQ , req_ptr, NULL);
    } else if (strncmp(str, "SETSTAFLAG", 10) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_SET_STA_FLAGS_REQ */
        UINT_8 aucMac[6] = {0x0, 0x0C, 0xE7, 0x12, 0x34, 0x56};
        hostap_wndrv_set_sta_flags_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_set_sta_flags_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_set_sta_flags_req_struct), TD_CTRL);

        COPY_MAC_ADDR(req_ptr->sta_addr, aucMac);
        req_ptr->flags_or = BIT(1);
        req_ptr->flags_or = 0;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_SET_STA_FLAGS_REQ , req_ptr, NULL);
    } else if (strncmp(str, "GETENC", 6) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_GET_ENCRYPTION_REQ */
        UINT_8 aucMac[6] = {0x0, 0x0C, 0xE7, 0x12, 0x34, 0x56};
        /* set security profile */
        hostap_wndrv_get_encryption_req_struct *req_ptr;
        req_ptr = (hostap_wndrv_get_encryption_req_struct *)
                  construct_local_para(sizeof(hostap_wndrv_get_encryption_req_struct), TD_CTRL);

        COPY_MAC_ADDR(req_ptr->sta_addr, aucMac);
        req_ptr->idx = 0;
        wndrv_hostap_send_msg_ut(MSG_ID_HOSTAP_WNDRV_GET_ENCRYPTION_REQ , req_ptr, NULL);
    } else if (strncmp(str, "SENDPKT", 7) == 0) {
        /* MSG_ID_HOSTAP_WNDRV_FRAME_REQ */
        //UINT_8 aucMac[6] = {0x0, 0x0C, 0xE7, 0x12, 0x34, 0x56};
        if (val1 == 0) {
            utSendProbeRequest();    /* send probe request */
        }
    } else {
        MT5931_TRACE(TRACE_GROUP_6, MT5931_INFO_, "[APNIC] No match command");
    }

}
#endif /*__HOTSPOT_SUPPORT__*/
#endif

static void wndrv_test_sniffer_mode(kal_char *inject_string)
{
#ifdef __WIFI_SNIFFER_SUPPORT__
    wndrv_abm_set_sniffer_mode_req_struct *req_ptr;
    ilm_struct *ilm_p;
    kal_uint32 ucOnOffSwitch, ucChannelNum, ucSnifferFilterOptions, u2MinPacketLength, u2MaxPacketLength;
    kal_int32 cMinRssi, cMaxRssi;


    req_ptr = (wndrv_abm_set_sniffer_mode_req_struct *)\
              construct_local_para(sizeof(wndrv_abm_set_sniffer_mode_req_struct), TD_CTRL);

    sscanf(inject_string, "%d,%d,%x,%d,%d,%d,%d ", &ucOnOffSwitch, &ucChannelNum, &ucSnifferFilterOptions, &u2MinPacketLength, &u2MaxPacketLength, &cMinRssi, &cMaxRssi);

    req_ptr->ucChannelNum = (kal_uint8)ucChannelNum;
    req_ptr->ucOnOffSwitch = (sniffer_mode_onoff_enum)ucOnOffSwitch;
    req_ptr->ucSnifferFilterOptions = (kal_uint32)ucSnifferFilterOptions;
    req_ptr->u2MinPacketLength = (kal_uint16)u2MinPacketLength;
    req_ptr->u2MaxPacketLength = (kal_uint16)u2MaxPacketLength;
    req_ptr->cMinRssi = (kal_int32)cMinRssi;
    req_ptr->cMaxRssi = (kal_int32)cMaxRssi;


    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "111OnOff(%d),Channel(%d),FilterOptions(0x%x),MinRssi(%d),MaxRssi(%d),MinLen(%d),MaxLen(%d)", req_ptr->ucOnOffSwitch, \
                     req_ptr->ucChannelNum, req_ptr->ucSnifferFilterOptions, req_ptr->cMinRssi, req_ptr->cMaxRssi, req_ptr->u2MinPacketLength, req_ptr->u2MaxPacketLength);

    ilm_p                 = allocate_ilm(MOD_ABM);
    ilm_p->msg_id         = MSG_ID_WNDRV_ABM_SET_SNIFFER_MODE_REQ;
    ilm_p->local_para_ptr = (local_para_struct *)req_ptr;
    ilm_p->peer_buff_ptr  = NULL;

    SEND_ILM(MOD_ABM, MOD_WNDRV, SUPC_ABM_SAP, ilm_p);
#endif
}



/*******************************************************************************
* FUNCTION
*   wndrv_tst_inject_string_handler
*
* DESCRIPTION
*   Handle different tst inject string requests
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_tst_inject_string_handler(ilm_struct *ilm_ptr)
{

    tst_module_string_inject_struct *tst_inject = (tst_module_string_inject_struct *)ilm_ptr->local_para_ptr;

    /* needs index < array size */
    if (tst_inject->index < (sizeof(wndrv_tst_inject_string_function_table) / sizeof(wndrv_tst_inject_string_func_ptr))) {
        (*(wndrv_tst_inject_string_function_table[tst_inject->index]))((kal_char *)tst_inject->string);
    }
}

#endif /* CFG_SUPPORT_INJECT_MSG */