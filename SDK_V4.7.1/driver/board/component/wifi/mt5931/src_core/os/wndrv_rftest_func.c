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
//#include "kal_release.h"
//#include "app_ltlcom.h"

#include "wndrv_cnst.h"
#include "wndrv_cal.h"
#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"

#include "wndrv_os_private.h"
#include "precomp.h"
//#include "eint.h"
#include "wndrv_kal.h"
#include "wndrv_context.h"

#include "wndrv_param.h"

extern PINT_8 wndrv_firmware_p;
extern UINT_32 wndrv_firmware_size;
//extern int rand(void);

/* -------------------------------------------------------------------------- */

/* 6 General Functions */
static void set_channel(kal_uint32 ch_freq);
#if 1
static void set_bandwidth(wndrv_test_rx_bw_struct *band_p);
static void set_tx_gi(wndrv_test_tx_gi_struct *gi_p);
static void set_tx_preemble(wndrv_test_tx_mode_sel_struct *mode_p);
#else
static void set_bandwidth(peer_buff_struct *peer_p);
static void set_tx_gi(peer_buff_struct *peer_p);
static void set_tx_preemble(peer_buff_struct *peer_p);
#endif
static void set_gain(kal_uint16 tx_gain_dac);
static void set_rate(wndrv_test_rate_enum rate_idx);

/* 14 Set Functions */
static void      set_test_mode_handler(kal_uint8 test_mode);
static kal_int32 set_stop_handler(void);
static kal_int32 set_start_rx_handler(void);

//static void      set_cont_tx_handler     ( wndrv_test_rate_enum tx_rate );
//static void set_cont_tx_handler(UINT_32 u4Rate, UINT_16 u2TxPwrGain, UINT_8  ucTxAnt);
static void set_cont_tx_handler(wndrv_test_tx_struct *pkt_tx_p);

//static void      set_local_freq_handler  ( void );
static void set_local_freq_handler(void);

#if 0
//static void      set_carrier_supp_handler( wndrv_test_rate_enum tx_rate );
static void set_carrier_supp_handler(UINT_32 nModulationType,
                                     UINT_32 u4Rate,
                                     UINT_16 u2TxPwrGain,
                                     UINT_8  ucTxAnt
                                    );
#else
static void      set_carrier_supp_handler(wndrv_test_tx_struct *pkt_tx_p);
#endif

static kal_int32 set_pkt_tx_handlerEX(kal_bool stop, wndrv_test_pkt_tx_structEX *pkt_tx_p);
static kal_int32 set_pwr_mgt_handler(wndrv_test_pwr_mgt_enum pwr_mgt);
static void set_pkt_rx_handler(wndrv_test_rx_mode_enum mode);
static kal_int32 set_reg_domain_handler(kal_uint8 *code);
//static void      set_dbm2dac_handler     ( wndrv_test_dbm2dac_struct *dbm2dac_p );
static void      set_mcr32_handler(peer_buff_struct *peer_p);
//static void      set_mcr16_handler       ( peer_buff_struct *peer_p );
//static void      set_bbcr_handler        ( peer_buff_struct *peer_p );
static void      set_eeprom_handler(peer_buff_struct *peer_p);

/* 9 Query Functions */
static void query_pkt_tx_handlerEX(wndrv_test_tx_status_structEX *tx_status_p);
#if 0
static void query_pkt_rx_start(void);
#endif
static void query_pkt_rx_handler(wndrv_test_rx_status_struct *rx_status_p);
static void query_ch_id_list_handler(wndrv_test_ch_id_list_struct *list_p);
static void query_rx_counter_handler(wndrv_test_rx_counter_struct *counter_p);
static void query_dbm2dac_handler(kal_uint16 *tx_gain_dac_p);
static void query_mcr32_handler(peer_buff_struct *in_p, peer_buff_struct **out_p);
static void query_mcr16_handler(peer_buff_struct *in_p, peer_buff_struct **out_p);
static void query_bbcr_handler(peer_buff_struct *in_p, peer_buff_struct **out_p);
static void query_eeprom_handler(peer_buff_struct *in_p, peer_buff_struct **out_p);

/* 1 Callback Function */

void (* RFTool_WiFi_PktRx_Callback)(wndrv_test_rx_status_struct *result);

extern WLAN_STATUS
rftestSetTestPacketTx(IN  PVOID    pvAdapter,
                      IN  PVOID    setBuffer_p,
                      IN  UINT_32  setBufferLen,
                      OUT PUINT_32 setInfoLen_p);

extern WLAN_STATUS
rftestSetTestPacketRx(IN  PVOID    pvAdapter,
                      IN  PVOID    setBuffer_p,
                      IN  UINT_32  setBufferLen,
                      OUT PUINT_32 setInfoLen_p);

extern void hpi_power_ctrl(kal_bool bTurnOnClock);

/* -------------------------------------------------------------------------- */
extern wndrv_context_struct wndrv_context;
extern kal_uint8 wndrv_eeprom_table[ 512 ];

extern UINT_8 g_hif_rx_event_port_num;
#if 0
static const kal_uint8 rate_idx2value[ WNDRV_TEST_RATE_COUNT ] = {
    0x02,    /* 1M */
    0x04,    /* 2M */
    0x0B,    /* 5.5M */
    0x16,    /* 11M */
    0x0c,    /* 6M */
    0x12,    /* 9M */
    0x18,    /* 12M */
    0x24,    /* 18M */
    0x30,    /* 24M */
    0x48,    /* 36M */
    0x60,    /* 48M */
    0x6c     /* 54M */
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

/* Not used in MT5921 */
#if IS_BB_CHIP_MT5911
static const kal_uint8 rate_idx2mt5911value[ WNDRV_TEST_RATE_COUNT ] = {
    (0x00 << 2),           /* 1M */
    (0x01 << 2),           /* 2M */
    (0x02 << 2),           /* 5.5M */
    (0x03 << 2),           /* 11M */
    ((0x0b << 2) | 0x80),  /* 6M */
    ((0x0f << 2) | 0x80),  /* 9M */
    ((0x0a << 2) | 0x80),  /* 12M */
    ((0x0e << 2) | 0x80),  /* 18M */
    ((0x09 << 2) | 0x80),  /* 24M */
    ((0x0d << 2) | 0x80),  /* 36M */
    ((0x08 << 2) | 0x80),  /* 48M */
    ((0x0c << 2) | 0x80)   /* 54M */
};
#endif
#if IS_BB_CHIP_MT5911
static kal_uint8  wndrv_CCKTxFilter[6];
static kal_uint8  wndrv_OFDMTxFilter[10];
static kal_uint8  wndrv_BBCRSave[8];
static kal_uint32 wndrv_PPMCRSave;
#endif
#endif

static kal_uint16 wndrv_tx_dac_value;

/* For MT5921 */

typedef struct _DATA_RATE_ENTRY_STRUCT_T {
    char *pcRateString;
    INT_32 i4BBUiCfg; /*Script config value 2(1M), 4(2M), 11(5.5M), 22 (11M)...*/
    INT_32 i4BBHwVal; /*HW config value 0(1M), 1(2M), 2(5.5M), 3 (11M)...*/
    UINT_8 ucRateGruopEep;
    BOOLEAN fgIsCCK;  /*CCK: TRUE, OFDM: FALSE*/
} DATA_RATE_ENTRY_STRUCT_T, *P_DATA_RATE_ENTRY_STRUCT_T;

#define EEPROM_RATE_GROUP_CCK           0x0
#define EEPROM_RATE_GROUP_OFDM_6_9M     0x1
#define EEPROM_RATE_GROUP_OFDM_12_18M   0x2
#define EEPROM_RATE_GROUP_OFDM_24_36M   0x3
#define EEPROM_RATE_GROUP_OFDM_48_54M   0x4

DATA_RATE_ENTRY_STRUCT_T arDataRateTable[] = {
    {"1M",      2,      0x00,  EEPROM_RATE_GROUP_CCK,         TRUE},
    {"2M",      4,      0x01,  EEPROM_RATE_GROUP_CCK,         TRUE},
    {"5.5M",    11,     0x02,  EEPROM_RATE_GROUP_CCK,         TRUE},
    {"11M",     22,     0x03,  EEPROM_RATE_GROUP_CCK,         TRUE},
    {"6M",      12,     0x0B,  EEPROM_RATE_GROUP_OFDM_6_9M,   FALSE},
    {"9M",      18,     0x0F,  EEPROM_RATE_GROUP_OFDM_6_9M,   FALSE},
    {"12M",     24,     0x0A,  EEPROM_RATE_GROUP_OFDM_12_18M, FALSE},
    {"18M",     36,     0x0E,  EEPROM_RATE_GROUP_OFDM_12_18M, FALSE},
    {"24M",     48,     0x09,  EEPROM_RATE_GROUP_OFDM_24_36M, FALSE},
    {"36M",     72,     0x0D,  EEPROM_RATE_GROUP_OFDM_24_36M, FALSE},
    {"48M",     96,     0x08,  EEPROM_RATE_GROUP_OFDM_48_54M, FALSE},
    {"54M",     108,    0x0C,  EEPROM_RATE_GROUP_OFDM_48_54M, FALSE}
};

#define NUM_SUPPORTED_RATES (sizeof(arDataRateTable)/sizeof(DATA_RATE_ENTRY_STRUCT_T))
#define DATE_RATE_WITH_SHORT_PREAMBLE (200)
#define CCR_BB_CLOCK_ON                 BITS(30, 31)
#define CCR_BB_CLOCK_MASK               BITS(30, 31)

/* AFE Configuration Data 1 Register*/
#define ACDR1_RG_DACCLK_SEL_MASK        BITS(0,1)
#define ACDR1_RG_DACCLK_SEL_20M         0
#define ACDR1_RG_DACCLK_SEL_40M         1
#define ACDR1_RG_DACCLK_SEL_80M         2
#define ACDR1_RG_DACCLK_SEL_XTAL_CLK    3

/* AFE Configuration Data 4 Register*/
#define ACDR4_DAC_CLK_MODE              BIT(31)

#define USE_TPCFF_CODEWORD    0

#if 0
/* Link quality (RSSI) statitics */
static kal_int32 g_rssi_min = 0;
static kal_int32 g_rssi_max = 0;
static kal_int32 g_rssi_sum = 0;
static kal_int32 g_rssi_variance = 0;
static kal_uint32 g_rssi_num = 0;
#endif

/*******************************************************************************
*
*  Local Functions
*
*******************************************************************************/

/*******************************************************************************
* FUNCTION
*   set_channel
*
* DESCRIPTION
*   Corresponding to
*   Test DLL:       OID_CUSTOM_802_11_CONFIGURATION
*   Network driver: OID_CUSTOM_802_11_CONFIGURATION
*
* CALLS
*   WLAN_STATUS
*   wlanoidSetConfiguration( IN  PMP_ADAPTER adapter_p,
*                            IN  PVOID       setBuffer_p,
*                            IN  UINT_32     setBufferLen,
*                            OUT PUINT_32    setInfoLen_p );
*
* PARAMETERS
*   kal_uint32 ch_freq (KHz)
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_channel(kal_uint32 ch_freq)
{

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32                    pdu_len;
    PARAM_MTK_WIFI_TEST_STRUC_T   data;

    data.u4FuncIndex = RF_AT_FUNCID_CH_FREQ;
    data.u4FuncData = (kal_uint32)ch_freq;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}

/*******************************************************************************
* FUNCTION
*   set_bandwidth
*
* DESCRIPTION
*
* CALLS
*   WLAN_STATUS
*
* PARAMETERS
*   wndrv_test_bandwidth_enum
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#if 1
static void set_bandwidth(wndrv_test_rx_bw_struct *band_p)
#else
static void set_bandwidth(peer_buff_struct *peer_p)
#endif
{
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32                    pdu_len;
    //kal_uint16                    pdu_len_16;
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
#if 0
    wndrv_test_rx_bw_struct      *band_p;
    band_p = (wndrv_test_rx_bw_struct *)get_pdu_ptr(peer_p, &pdu_len_16);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_BANDWIDTH;
    data.u4FuncData = (kal_uint32)band_p->rxBw;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}


/*******************************************************************************
* FUNCTION
*   set_tx_gi
*
* DESCRIPTION
*
* CALLS
*   WLAN_STATUS
*
* PARAMETERS
*   wndrv_test_guard_intval_enum
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#if 1
static void set_tx_gi(wndrv_test_tx_gi_struct *gi_p)
#else
static void set_tx_gi(peer_buff_struct *peer_p)
#endif
{
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32                    pdu_len;
    //kal_uint16                    pdu_len_16;
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
#if 0
    wndrv_test_tx_gi_struct      *gi_p;
    gi_p = (wndrv_test_tx_gi_struct *)get_pdu_ptr(peer_p, &pdu_len_16);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_GI;
    if (gi_p->txGi == WNDRV_TEST_TX_GI_400NS) {
        data.u4FuncData = (kal_uint32)1;    /* short GI */
    } else if (gi_p->txGi == WNDRV_TEST_TX_GI_800NS) {
        data.u4FuncData = (kal_uint32)0;    /* normal GI */
    }

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}


/*******************************************************************************
* FUNCTION
*   set_tx_preemble
*
* DESCRIPTION
*
* CALLS
*   WLAN_STATUS
*
* PARAMETERS
*   wndrv_test_mode_sel_enum
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#if 1
static void set_tx_preemble(wndrv_test_tx_mode_sel_struct *mode_p)
#else
static void set_tx_preemble(peer_buff_struct *peer_p)
#endif
{
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32                    pdu_len;
    //kal_uint16                    pdu_len_16;
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
#if 0
    wndrv_test_tx_mode_sel_struct  *mode_p;
    mode_p = (wndrv_test_tx_mode_sel_struct *)get_pdu_ptr(peer_p, &pdu_len_16);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_PREAMBLE;
    if (mode_p->txMode == WNDRV_TEST_TX_GREEN_FIELD) {
        data.u4FuncData = (kal_uint32)3;    /* 11n GF */
    } else if (mode_p->txMode == WNDRV_TEST_TX_MIX_MODE) {
        data.u4FuncData = (kal_uint32)2;    /* 11n MM */
    }

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}


/*******************************************************************************
* FUNCTION
*   set_gain
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnSetTxPower
*   MT5911 Test DLL:  setBBTxPower
*
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   kal_uint16 tx_gain_dac
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_gain(kal_uint16 tx_gain_dbm)
{

    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_TX_GAIN_DAC,
                 "SET_GAIN: dbm=%d", (kal_uint8)tx_gain_dbm);

    data.u4FuncIndex = RF_AT_FUNCID_TXPWRMODE;
    data.u4FuncData = 0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    data.u4FuncIndex = RF_AT_FUNCID_POWER;
    //data.u4FuncData = (kal_uint32)tx_gain_dbm * 2;  /* data.u4FuncData is unit of 0.5 dbm */
    data.u4FuncData = (kal_uint32)tx_gain_dbm;  /* data.u4FuncData is unit of 0.5 dbm, and MMI will multiply it by 2 */

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

}


/*******************************************************************************
* FUNCTION
*   set_rate
*
* DESCRIPTION
*   Corresponding to
*   Test DLL:       OID_CUSTOM_802_11_DESIRED_RATES
*   Network driver: OID_CUSTOM_802_11_DESIRED_RATES
*
* CALLS
*   WLAN_STATUS
*   txrateSetDesiredRates( IN  PMP_ADAPTER adapter_p,
*                          IN  PVOID       setBuffer_p,
*                          IN  UINT_32     setBufferLen,
*                          OUT PUINT_32    setInfoLen_p );
*
* PARAMETERS
*   wndrv_test_rate_enum rate_idx
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_rate(wndrv_test_rate_enum rate_idx)
{

    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    data.u4FuncIndex = RF_AT_FUNCID_RATE;
    if (rate_idx >= WNDRV_TEST_MOD_MCS0) {
        rate_idx -= WNDRV_TEST_MOD_MCS0;
        rate_idx |= BIT(31);    /* b31 set to 1 */
    } else {
        rate_idx &= 0x7FFFFFFF;    /* b31 clear to 0 */
    }
    data.u4FuncData = (kal_uint32)rate_idx;

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_RATE, "SET_RATE: %d", rate_idx);

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

}


/*******************************************************************************
* FUNCTION
*   set_rx_antenna
*
* DESCRIPTION
*   Corresponding to
*   Test DLL:       setRxAntenna
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   wndrv_test_rx_ant_sel rx_ant
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_rx_antenna(wndrv_test_rx_ant_sel_enum rx_ant)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    data.u4FuncIndex = RF_AT_FUNCID_ANTENNA;
    data.u4FuncData = (kal_uint8) rx_ant;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}


/*******************************************************************************
* FUNCTION
*   set_tx_antenna
*
* DESCRIPTION
*   Corresponding to
*   Test DLL:       setTxAnt
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   kal_uint8 antenna
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_tx_antenna(kal_uint8 antenna)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    data.u4FuncIndex = RF_AT_FUNCID_ANTENNA;
    data.u4FuncData = (kal_uint8) antenna;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}

#if 0
/*******************************************************************************
* FUNCTION
*   set_PPMCR_SCR
*
* DESCRIPTION
*   Corresponding to
*   Test DLL:       setPPMCRAndSCR
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
#define PPMCR_RF_TX        BIT(20)
#define PPMCR_RF_TR_SW     BIT(18)
#define PPMCR_RF_SHDN      BIT(28)
#define PPMCR_CONT_TX (/*PPMCR_RF_PA2_EN |*/ PPMCR_RF_TX | PPMCR_RF_TR_SW /*| PPMCR_BB_TX_PE */| PPMCR_RF_SHDN)

#define SCR_PAUEN          BIT(7)
#define PCICR_RFBAND       BIT(5)
#define PCICR_PA5EN_POL    BIT(8)
#define PPMCR_RF_PA5_EN    BIT(21)
#define PCICR_PA2EN_POL    BIT(07)
#define PPMCR_RF_PA2_EN    BIT(22)

static void set_PPMCR_SCR(void)
{
    kal_uint32 dscrTmp;
    kal_uint32 dscrTmps;
    kal_uint32 pctx = PPMCR_CONT_TX;

    read_mcr_reg32(MCR_SCR, &dscrTmp);
    write_mcr_reg32(MCR_SCR, dscrTmp & (~SCR_PAUEN));

    read_mcr_reg32(MCR_PCICR, &dscrTmps);

    if (dscrTmps & PCICR_RFBAND) {
        /* 5 GHz band, enable PA5 */
        if (dscrTmps & PCICR_PA5EN_POL) {
            pctx |= PPMCR_RF_PA5_EN;
        }

        /* disable PA2 */
        if (!(dscrTmps & PCICR_PA2EN_POL)) {
            pctx |= PPMCR_RF_PA2_EN;
        }
    } else {
        /* 2.4 GHz band, enable PA2 */
        if (dscrTmps & PCICR_PA2EN_POL) {
            pctx |= PPMCR_RF_PA2_EN;
        }

        /* disable PA5 */
        if (!(dscrTmps & PCICR_PA5EN_POL)) {
            pctx |= PPMCR_RF_PA5_EN;
        }
    }

    /* enable TX_RX, PA_PE, TR_SW = T */
    write_mcr_reg32(MCR_PPMCR, pctx);
}

#endif


/*******************************************************************************
* FUNCTION
*   set_test_mode_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnStandby
*   MT5911 Test DLL:  OnBtnStandby
*                     - setPacketRx
*                     - setStandBy
*   Network driver: None
*
* CALLS
*
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
static void set_test_mode_handler(kal_uint8 test_mode)
{
    kal_uint32 setInfoLen;
    P_ADAPTER_T adapter_p = wndrv_context.adpt_p;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_SET_STOP, "SET_TEST_MODE");

    //20081110 add by saker for MT5921 support
#if( IS_BB_CHIP_MT5911 || IS_BB_CHIP_MT5921 || IS_BB_CHIP_MT5931 )
    {
        if (test_mode) {
#if 0   //TODO(Nelson): check the adapter stop/start procedures
            wlanAdapterStop(adapter_p); //Nelson(Note): workaround: release system resource to re-execute wlanAdapterStart()
            wndrv_test_set_test_mode();
#else
            /* Set driver to switch into RF test mode */
            rftestSetTestMode(adapter_p, NULL, 0, (PUINT_32)&setInfoLen);
#endif
        } else {
            wndrv_test_set_normal_mode();
        }
    }
#else
    //#error "error"
#endif
}


/*******************************************************************************
* FUNCTION
*   set_stop_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnStandby
*   MT5911 Test DLL:  OnBtnStandby
*                     - setPacketRx
*                     - setStandBy
*   Network driver: None
*
* CALLS
*   static kal_int32 set_pkt_rx_handler( wndrv_test_rx_mode_enum mode );
*   static kal_int32 set_pkt_tx_handler( kal_bool stop,
*                                        wndrv_test_tx_packet_struct *pkt_tx_p );
*   static kal_int32 set_pwr_mgt_handler( wndrv_test_pwr_mgt_enum pwr_mgt );
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
static kal_int32 set_stop_handler(void)
{
    kal_int32 status = WLAN_STATUS_SUCCESS;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32                    pdu_len;
    PARAM_MTK_WIFI_TEST_STRUC_T   data;

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = (kal_uint32)RF_AT_COMMAND_STOPTEST;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    kal_event_stop_timer(PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER);

    return status;
}


/*******************************************************************************
* FUNCTION
*   set_start_rx_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnStandby
*   MT5911 Test DLL:  OnBtnStandby
*                     - setPacketRx
*                     - setStandBy
*   Network driver: None
*
* CALLS
*   static kal_int32 set_pkt_rx_handler( wndrv_test_rx_mode_enum mode );
*   static kal_int32 set_pkt_tx_handler( kal_bool stop,
*                                        wndrv_test_tx_packet_struct *pkt_tx_p );
*   static kal_int32 set_pwr_mgt_handler( wndrv_test_pwr_mgt_enum pwr_mgt );
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
static kal_int32 set_start_rx_handler(void)
{
    kal_int32 status = KAL_TRUE;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    kal_uint32                    pdu_len;
    PARAM_MTK_WIFI_TEST_STRUC_T   data;

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = (kal_uint32)RF_AT_COMMAND_STARTRX;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    return status;
}


/*******************************************************************************
* FUNCTION
*   set_cont_tx_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnContTx
*                     - OnBtnSetTxPower
*                     - SetRegDomain
*                     - setChannel
*                     - OutputPower
*   MT5911 Test DLL:  OnBtnContTx
*                     - SetRegDomain
*                     - setChannel
*                     - setOutputPower
*                       - setBBTxPower
*                       - setTxAnt
*                       - setOutputPower
*   Network driver: None
*
* CALLS
*   None.
*
* PARAMETERS
*   wndrv_test_rate_enum tx_rate
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_cont_tx_handler(
    wndrv_test_tx_struct *pkt_tx_p
)
{

    PARAM_MTK_WIFI_TEST_STRUC_T     data;
    kal_uint32                      pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlue = wndrv_context.glue_p;
    UINT_32 u4Rate = pkt_tx_p->tx_rate;

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_CONT_TX,
                 "SET_CONT_TX: rate=%d", u4Rate);

    /* set 11b/g packet preamble */
    if (u4Rate < WNDRV_TEST_MOD_MCS0) {
        data.u4FuncIndex = RF_AT_FUNCID_PREAMBLE;
        data.u4FuncData = 0;  /* default set long preable */

        GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
        wlanSetInformation(prAdapter,
                           (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                           (PVOID)&data,
                           sizeof(data),
                           (PUINT_32)&pdu_len);
        GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    }

    // Rate SetATParam(3, i4AtRate);
    data.u4FuncIndex = RF_AT_FUNCID_RATE;
    if (u4Rate >= WNDRV_TEST_MOD_MCS0) {
        u4Rate -= WNDRV_TEST_MOD_MCS0;
        u4Rate |= BIT(31);    /* b31 set to 1 */
    } else {
        u4Rate &= 0x7FFFFFFF;    /* b31 clear to 0 */
    }
    data.u4FuncData = (kal_uint32)u4Rate;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    // Length SetATParam(6, 100);
    data.u4FuncIndex = RF_AT_FUNCID_PKTLEN;
    data.u4FuncData =  100;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    // Count SetATParam(7, 1);
    data.u4FuncIndex = RF_AT_FUNCID_PKTCNT;
    data.u4FuncData =  1;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    // TXOP
    // SetATParam(10, 0x00020000);
    // Retrylimit SetATParam(13, 1);
    data.u4FuncIndex = RF_AT_FUNCID_RETRYLIMIT;
    data.u4FuncData =  1;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    /* Enable TX queue */
    data.u4FuncIndex = RF_AT_FUNCID_QUEUE;
    data.u4FuncData =  0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

#if 0
    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData =  1 ;  /* start tx test */

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData =  0;   /* stop test */

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestQueryAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_OUTPUT_POWER;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}


/*******************************************************************************
* FUNCTION
*   set_local_freq_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnLocalFrequency
*                     - OnBtnSetTxPower (!IPN2128)
*                     - SetRegDomain
*                     - setChannel
*                     - LocalFrequecyMeasure
*   MT5911 Test DLL:  OnBtnLocalFrequency
*                     - SetRegDomain
*                     - setChannel
*                     - setLocalFrequecy
*                       - setBBTxPower
*                       - setTxAnt
*                       - setLocalFrequecyMeasure
*   Network driver: None
*
* CALLS
*   None.
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
static void set_local_freq_handler(void)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_LOCAL_FREQ;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}

/*******************************************************************************
* FUNCTION
*   set_carrier_supp_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnTxCarrierSuppression
*                     - OnBtnSetTxPower (!IPN2128)
*                     - SetRegDomain
*                     - setChannel
*                     - CarrierSuppressionMeasure( int nModulationType, int nTxRate )
*                       - SetPPMCRAndSCR
*                       - write BBCR31
*   MT5911 Test DLL:  OnBtnTxCarrierSuppression
*                     - SetRegDomain
*                     - setChannel
*                     - setCarrierSuppression
*                       - setBBTxPower
*                       - setTxAnt
*                       - setCarrierSuppressionMeasure
*
*   Network driver: None
*
* CALLS
*   None.
*
* PARAMETERS
*   wndrv_test_rate_enum tx_rate
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_carrier_supp_handler(wndrv_test_tx_struct *pkt_tx_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                    pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    UINT_32 u4Rate = pkt_tx_p->tx_rate;

    /* set 11b/g packet preamble */
    if (u4Rate < WNDRV_TEST_MOD_MCS0) {
        data.u4FuncIndex = RF_AT_FUNCID_PREAMBLE;
        data.u4FuncData = 0;  /* default set long preable */

        GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
        wlanSetInformation(prAdapter,
                           (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                           (PVOID)&data,
                           sizeof(data),
                           (PUINT_32)&pdu_len);
        GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    }

    // Rate SetATParam(3, i4AtRate);
    data.u4FuncIndex = RF_AT_FUNCID_RATE;
    if (u4Rate >= WNDRV_TEST_MOD_MCS0) {
        u4Rate -= WNDRV_TEST_MOD_MCS0;
        u4Rate |= BIT(31);    /* b31 set to 1 */
    } else {
        u4Rate &= 0x7FFFFFFF;    /* b31 clear to 0 */
    }
    data.u4FuncData = (kal_uint32)u4Rate;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

#if 1
    if ((u4Rate == WNDRV_TEST_RATE_1M) || (u4Rate == WNDRV_TEST_RATE_2M) || (u4Rate == WNDRV_TEST_RATE_5_5M) ||
            (u4Rate == WNDRV_TEST_RATE_11M)) {
        data.u4FuncIndex = 65;
        data.u4FuncData = 5;    /* CCK PI */
    } else {
        data.u4FuncIndex = 65;
        data.u4FuncData = 2;    /* OFDM LTF */
    }

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    data.u4FuncIndex = 1;
    data.u4FuncData = 10;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
#else
    data.u4FuncIndex = RF_AT_FUNCID_COMMAND;
    data.u4FuncData = RF_AT_COMMAND_CARRIER_SUPP;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
#endif
}


/*******************************************************************************
* FUNCTION
*   set_pkt_tx_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnContPktTx
*                     - OnBtnSetTxPower
*                     - SetRegDomain
*                     - setChannel
*                     - ContinuousPktTx
*                       - OID_CUSTOM_802_11_DESIRED_RATES
*                       - OID_CUSTOM_TEST_PACKET_TX
*   MT5911 Test DLL:  OnBtnContPktTx
*                     - setChannel
*                     - setPacketTx
*                       - OID_CUSTOM_802_11_DESIRED_RATES
*                       - OID_IPC_TEST_PACKET_TX
*
*   Network driver: OID_CUSTOM_802_11_DESIRED_RATES
*                   OID_IPC_TEST_PACKET_TX
*
* CALLS
*   static void set_rate( wndrv_test_rate_enum rate_idx );
*
*   WLAN_STATUS
*   rftestSetTestPacketTx( IN  PVOID   pvAdapter,
*                          IN  PVOID   setBuffer_p,
*                          IN  UINT32  setBufferLen,
*                          OUT PUINT32 setInfoLen_p );
*
* PARAMETERS
*   kal_bool stop
*   wndrv_test_pkt_tx_struct *pkt_tx_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static kal_int32 set_pkt_tx_handlerEX(kal_bool stop, wndrv_test_pkt_tx_structEX *pkt_tx_p)
{
    TX_PACKET_STRUC  *set_p;
    kal_uint32       setInfoLen, i;
    kal_int32        status;
    //P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlue = wndrv_context.glue_p;

    /* Make sure MAX_BUFF_SIZE in TX_PACKET_STRUC is less than 1500.*/
    set_p = WNDRV_ALLOC_BUF(sizeof(TX_PACKET_STRUC));
    WNDRV_EXT_ASSERT(set_p, "set_pkt_tx_handlerEX: set_p can't allocate buffer", 0, 0, 0);

    if (stop) {
        set_p->pktLength   = 0xF0F0F0F0;
        set_p->pktCount    = 0xF0F0F0F0;
        set_p->pktInterval = 0xF0F0F0F0;
    } else {
        /* 1) Set Desired Rate. */
        set_rate(pkt_tx_p->tx_rate);


        /* 2) Do PKT_TX Test. */
        WNDRV_EXT_ASSERT(pkt_tx_p->pktLength <= MAX_BUF_SZ ,
                         "set_pkt_tx_handlerEX: pkt_tx_p->pktLength must samller than MAX_BUF_SZ(2000)",
                         pkt_tx_p->pktLength, MAX_BUF_SZ, 0);

        // James modify for calculating packet size correctly
        // FCS is 4 byte
        pkt_tx_p->pktLength -= 4;
        set_p->pktLength   = pkt_tx_p->pktLength;   /* 24~1500 */
        set_p->pktCount    = pkt_tx_p->pktCount;
        set_p->pktInterval = pkt_tx_p->pktInterval;
        set_p->txGain      = (kal_uint8) pkt_tx_p->tx_gain_dac;
        set_p->txAnt       = pkt_tx_p->txAnt;
        set_p->txFlags     = pkt_tx_p->txFlags;
        set_p->targetAlc   = pkt_tx_p->targetAlc;
        //200081021 add by saker
        set_p->is_short_preamble = pkt_tx_p->is_short_preamble;
        //20081023 add by saker
        set_p->txRate      = (kal_uint32)pkt_tx_p->tx_rate;

        memcpy(set_p->pktContent, pkt_tx_p->mac_header, 24);

        switch (pkt_tx_p->pattern) {
            case WNDRV_TX_ALL_ZEROS:
                for (i = 24; i < set_p->pktLength; i++) {
                    set_p->pktContent[i] = 0x00;
                }
                break;

            case WNDRV_TX_ALL_ONES:
                for (i = 24; i < set_p->pktLength; i++) {
                    set_p->pktContent[i] = 0xff;
                }
                break;

            case WNDRV_TX_ALTERNATE_BITS:
                for (i = 24; i < set_p->pktLength; i++) {
                    set_p->pktContent[i] = 0xaa;
                }
                break;

            case WNDRV_TX_PSEUDO_RANDOM:
                for (i = 24; i < set_p->pktLength; i++) {
                    set_p->pktContent[i] = (kal_uint8) rand();
                }
                break;

            default:
                WNDRV_EXT_ASSERT(KAL_FALSE, "set_pkt_tx_handlerEX: Unexpected pkt_tx_p->pattern", pkt_tx_p->pattern, 0, 0);
                break;
        }
    }

    WNDRV_TRACE6(TRACE_GROUP_10, INFO_WNDRV_SET_PKT_TX,
                 "SET_PKT_TX: len=%d, cnt=%d, inter=%d, pwr=%d, ant=%d, ptern=%d",
                 set_p->pktLength,
                 set_p->pktCount,
                 set_p->pktInterval,
                 set_p->txGain,
                 set_p->txAnt,
                 pkt_tx_p->pattern);

    status = rftestSetTestPacketTx(wndrv_context.adpt_p,
                                   set_p,
                                   sizeof(TX_PACKET_STRUC),
                                   (PUINT_32)&setInfoLen);

    WNDRV_FREE_BUF(set_p);

    return status;
}


/*******************************************************************************
* FUNCTION
*   set_pwr_mgt_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnPwSaveIdle
*                     - OID_CUSTOM_TEST_PWR_MGT
*                     OnBtnPwSaveSlip
*                     - OID_CUSTOM_TEST_PWR_MGT
*   MT5911 Test DLL:  setPowerManagementState
*                     - OID_IPC_SW_RADIO_ON_OFF_STATE
                        - ipcoidSetSWRadioOnOffState
*   Network driver: OID_IPC_TEST_PWR_MGT
*
* CALLS
*   WLAN_STATUS
*   rftestSetTestPwrMgt( IN  PVOID   pvAdapter,
*                        IN  PVOID   setBuffer_p,
*                        IN  UINT32  setBufferLen,
*                        OUT PUINT32 setInfoLen_p );
*
* PARAMETERS
*   typedef enum
*   {
*     WNDRV_POWER_MODE_NORMAL,
*     WNDRV_POWER_MODE_IDLE,
*     WNDRV_POWER_MODE_SLEEP
*   } wndrv_test_pwr_mgt_enum;
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static kal_int32 set_pwr_mgt_handler(wndrv_test_pwr_mgt_enum pwr_mgt)
{

//   PWR_MGT_STRUC           out;
    //kal_uint32              setInfoLen;
    kal_int32                status = WLAN_STATUS_SUCCESS;
    //PARAM_POWER_MODE         pm     = ENUM_PSP_CONTINUOUS_ACTIVE;
    //kal_uint32               setInfoLen;
    //P_GLUE_INFO_T            prGlueInfo = wndrv_context.glue_p;

#if 0
    //20081023 modify by saker
    switch (pwr_mgt) {
        case WNDRV_POWER_MODE_NORMAL:
            nicpmPowerOn(wndrv_context.adpt_p);
            //rftestEnterTestMode(wndrv_context.adpt_p);
            arbFsmClearEventPwrMode();
            status = WLAN_STATUS_SUCCESS;
            break;

        case WNDRV_POWER_MODE_IDLE:
            nicpmPowerOff(wndrv_context.adpt_p);
            wndrv_context.glue_p->fgIsPowerSave = TRUE;
            arbFsmClearEventPwrMode();
            status = WLAN_STATUS_SUCCESS;
            break;

        case WNDRV_POWER_MODE_SLEEP:
            nicpmPowerOff(wndrv_context.adpt_p);
            wndrv_context.glue_p->fgIsPowerSave = TRUE;
            arbFsmClearEventPwrMode();
            status = WLAN_STATUS_SUCCESS;
            break;

        case WNDRV_POWER_MODE_CONTINUOUS_ACTIVE:   /* Add for CTIA test (Nelson) */
            MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "[CTIA Test]Disable PS & Roaming\r\n");
            nicpmPowerOn(wndrv_context.adpt_p);
            wndrv_context.glue_p->ePSMode = ENUM_PSP_CONTINUOUS_ACTIVE;
            wndrv_context.glue_p->fgIsForceActive = TRUE;
            pm = Param_PowerModeCAM;
            GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSet802dot11PowerSaveProfile,
                               &pm,
                               sizeof(PARAM_POWER_MODE),
                               (PUINT_32)&setInfoLen);
            /* Set Rx Filter Alwats Ack even when FIFO full */

            NIC_UNSET_RX_FILTER(wndrv_context.adpt_p, RXFILTER_RXNOACK);
            GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
            arbFsmSetEventPwrMode();
            status = WLAN_STATUS_SUCCESS;
            break;

    }
#endif

    return status;

}


/*******************************************************************************
* FUNCTION
*   set_pkt_rx_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OnBtnContPktRx
*                     - setChannel
*                     - setRxDMA
*                       - OID_CUSTOM_TEST_PACKET_RX
*   MT5911 Test DLL:  OnBtnContPktRx
*                     - setChannel
*                     - setPacketRx
*                       - setRxAntenna
*                       - OID_IPC_TEST_PACKET_RX
*   Network driver: OID_IPC_TEST_PACKET_RX
*
* CALLS
*   WLAN_STATUS
*   rftestSetTestPacketRx( IN  PVOID   pvAdapter,
*                          IN  PVOID   setBuffer_p,
*                          IN  UINT32  setBufferLen,
*                          OUT PUINT32 setInfoLen_p );
*
* PARAMETERS
*   typedef enum
*   {
*      WNDRV_STOP_RX,
*      WNDRV_RF_RX_TEST_MODE  // For RF Test use
*   } wndrv_test_rx_mode_enum;
*
* RETURNS
*   void
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_pkt_rx_handler(wndrv_test_rx_mode_enum mode)
{
//    P_ADAPTER_T adapter_p = wndrv_context.adpt_p;

#if 0
    /* Init RSSI statistic variables */
    g_rssi_min = 0;
    g_rssi_max = 0;
    g_rssi_sum = 0;
    g_rssi_num = 0;
#endif

    switch (mode) {
        case WNDRV_STOP_RX:
            //rftestAbortTestMode( adapter_p );
            set_stop_handler();
            break;

        case WNDRV_RF_RX_TEST_MODE:
            //rftestEnterTestMode( adapter_p );
            set_start_rx_handler();
            break;
    }
}


/*******************************************************************************
* FUNCTION
*   set_reg_domain_handler
*
* DESCRIPTION
*   Corresponding to
*   IPN2128 Test DLL: OID_IPC_REGULATION_DOMAIN
*   MT5911 Test DLL:  OID_IPC_CURRENT_COUNTRY
*   Network driver:   OID_IPC_CURRENT_COUNTRY
*
* CALLS
*   WLAN_STATUS
*   domainSetCurrentCountry( IN  PMP_ADAPTER adapter_p,
*                            IN  PVOID       setBuffer_p,
*                            IN  UINT_32     setBufferLen,
*                            OUT PUINT_32    setInfoLen_p );
*
* PARAMETERS
*   kal_uint8 country_code[2]
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static kal_int32 set_reg_domain_handler(kal_uint8 *code)
{
    COUNTRY_STRING_ENTRY   set;
    UINT_32             setInfoLen;
    kal_int32              status;

    set.aucCountryCode[0] = code[0];
    set.aucCountryCode[1] = code[1];

#if 1
    status =    wlanoidSetCountryCode(wndrv_context.adpt_p,
                                      (PVOID)&set.aucCountryCode[0],
                                      2,
                                      &setInfoLen,
                                      TRUE);
#else
    status =    wlanSetInformation(wndrv_context.adpt_p,
                                   (PFN_OID_HANDLER_FUNC)wlanoidSetCountryCode,
                                   &set.aucCountryCode[0],
                                   2,
                                   (PUINT_32)&setInfoLen,
                                   TRUE);
#endif

    WNDRV_TRACE2(TRACE_GROUP_10, INFO_WNDRV_SET_REG_DOMAIN, "SET_REG_DOMAIN: %x%x",
                 code[0], code[1]);

    return status;
}


#if 0
/*******************************************************************************
* FUNCTION
*   set_dbm2dac_handler
*
* DESCRIPTION
*   Corresponding to
*   Test DLL:       none
*   Network driver: none
*
* CALLS
*   None.
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
static void set_dbm2dac_handler(wndrv_test_dbm2dac_struct *dbm2dac_p)
{
    /* WNDRV_TEST_SET_DBM_TO_DAC   */

    WNDRV_TRACE3(TRACE_GROUP_10, INFO_WNDRV_SET_DBM2DAC, "SET_DBM2DAC: freq=%d, rate=%d, dbm=%d",
                 dbm2dac_p->ch_freq, dbm2dac_p->tx_rate, dbm2dac_p->dbm);

    // James modify for eeprom
    // wndrv_tx_dac_value =  wndrv_eeprom_table[ 0x20 + ( dbm2dac_p->ch_freq - 1 ) ];
}
#endif


/*******************************************************************************
* FUNCTION
*   set_mcr32_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_MAC_REG_RW
*   Network driver: OID_IPC_MCR_RW
*
* CALLS
*   WLAN_STATUS
*   ipcoidSetMcrReadWrite( IN  PMP_ADAPTER adapter_p,
*                          IN  PVOID       setBuffer_p,
*                          IN  UINT_32     setBufferLen,
*                          OUT PUINT_32    setInfoLen_p );
*
* PARAMETERS
*   peer_buff_struct *peer_p
*
* RETURNS
*   kal_int32 status
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_mcr32_handler(peer_buff_struct *peer_p)
{
    wndrv_test_peer_buff_mcr32_struct *data_p;
    kal_uint32                         number;
    kal_uint16                         pdu_len;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
//   P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;
    UINT_32 u4BufLen;

    data_p = (wndrv_test_peer_buff_mcr32_struct *) get_pdu_ptr(peer_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_mcr32_struct);

    while (number > 0) {

        rMcrInfo.u4McrOffset = data_p->mcr_index;
        rMcrInfo.u4McrData = data_p->mcr32;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        wlanSetInformation(prAdapter,
                           wlanoidSetMcrWrite,
                           (PVOID)&rMcrInfo,
                           sizeof(rMcrInfo),
                           &u4BufLen);

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);


        data_p++;
        number--;
    }
}


#if 0
/*******************************************************************************
* FUNCTION
*   set_mcr16_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_MAC_REG_RW
*   Network driver: OID_IPC_MCR_RW
*
* CALLS
*   WLAN_STATUS
*   ipcoidSetMcrReadWrite( IN  PMP_ADAPTER adapter_p,
*                          IN  PVOID       setBuffer_p,
*                          IN  UINT_32     setBufferLen,
*                          OUT PUINT_32    setInfoLen_p );
*
* PARAMETERS
*   peer_buff_struct *peer_p
*
* RETURNS
*   kal_int32 status
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_mcr16_handler(peer_buff_struct *peer_p)
{
#if 0
    wndrv_test_peer_buff_mcr16_struct *data_p;
    kal_uint32                         number;
    kal_uint16                         pdu_len;

    data_p  = (wndrv_test_peer_buff_mcr16_struct *)get_pdu_ptr(peer_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_mcr16_struct);

    while (number > 0) {
        /* Be sure MCR access is in MCR addressing space. */
        //WNDRV_ASSERT( data_p->mcr_index <= 0x512 );
        /* Be sure MCR address is 2 byte boundary. */
        WNDRV_ASSERT((data_p->mcr_index & 0x01) == 0x00);

        write_mcr_reg16(data_p->mcr_index, data_p->mcr16);

        data_p++;
        number--;
    }
#endif
}


/*******************************************************************************
* FUNCTION
*   set_bbcr_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_MMI_RW
*   Network driver: OID_IPC_BBCR_RW
*
* CALLS
*   WLAN_STATUS
*   ipcoidSetBbcrReadWrite( IN  PMP_ADAPTER adapter_p,
*                           IN  PVOID       setBuffer_p,
*                           IN  UINT_32     setBufferLen,
*                           OUT PUINT_32    setInfoLen_p );
*
* PARAMETERS
*   peer_buff_struct *peer_p
*
* RETURNS
*   kal_int32 status
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_bbcr_handler(peer_buff_struct *peer_p)
{
#if 0
    kal_uint32                        number;
    wndrv_test_peer_buff_bbcr_struct *data_p;
    kal_uint16                        pdu_len;

    data_p  = (wndrv_test_peer_buff_bbcr_struct *)get_pdu_ptr(peer_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_bbcr_struct);

    while (number > 0) {
        /* Be sure MCR access is in MCR addressing space. */
        WNDRV_ASSERT(data_p->bbcr_index <=  0x7f);

        write_bbcr_reg8(data_p->bbcr_index, data_p->bbcr);

        data_p++;
        number--;
    }
#endif
}
#endif


/*******************************************************************************
* FUNCTION
*   set_eeprom_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       None
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   peer_buff_struct *peer_p
*
* RETURNS
*   kal_int32 status
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void set_eeprom_handler(peer_buff_struct *peer_p)
{
    kal_uint32                           number;
    wndrv_test_peer_buff_eeprom_struct  *data_p;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
//   P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;
    PARAM_CUSTOM_EEPROM_RW_STRUC_T data;
    kal_uint16                    pdu_len;
    kal_uint32                    data_len;

    data_p = (wndrv_test_peer_buff_eeprom_struct *)get_pdu_ptr(peer_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_eeprom_struct);

    while (number > 0) {
        /* Be sure EEPROM access is in EEPROM addressing space. */
        WNDRV_ASSERT(data_p->eeprom_index < 256);

        data.ucEepromIndex = (UINT_8) data_p->eeprom_index;
        data.u2EepromData = data_p->eeprom;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        wlanSetInformation(prAdapter,
                           wlanoidSetEepromWrite,
                           (PVOID)&data,
                           sizeof(data),
                           (PUINT_32)&data_len);

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);



        data_p++;
        number--;
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to retrieve permanent address from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
query_pkt_tx_cnt(
    IN P_ADAPTER_T  prAdapter,
    OUT PVOID       pvQueryBuffer,
    IN UINT_32      u4QueryBufferLen
)
{
    UINT_8 ucCmdSeqNum;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    P_CMD_TEST_CTRL_T pCmdTestCtrl;
    P_PARAM_MTK_WIFI_TEST_STRUC_T  prRfATInfo;

    ASSERT(prAdapter);

    DEBUGFUNC("query_pkt_tx_cnt");

    if (u4QueryBufferLen) {
        ASSERT(pvQueryBuffer);
    }

    prRfATInfo = (P_PARAM_MTK_WIFI_TEST_STRUC_T)pvQueryBuffer;

    if (u4QueryBufferLen != sizeof(PARAM_MTK_WIFI_TEST_STRUC_T)) {
        DBGLOG(REQ, ERROR, ("Invalid data. QueryBufferLen: %ld.\n",
                            u4QueryBufferLen));
        return WLAN_STATUS_INVALID_LENGTH;
    }

    // 1. Allocate CMD Info Packet and its Buffer
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, CMD_HDR_SIZE + sizeof(P_CMD_TEST_CTRL_T));
    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // Setup common CMD Info Packet
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pfCmdDoneHandler = nicCmdEventQueryRfTestATInfo;
    prCmdInfo->pfCmdTimeoutHandler = nicOidCmdTimeoutCommon;
    prCmdInfo->fgIsOid = TRUE;
    prCmdInfo->ucCID = CMD_ID_TEST_MODE;
    prCmdInfo->fgSetQuery = FALSE;
    prCmdInfo->fgNeedResp = TRUE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pvInformationBuffer = pvQueryBuffer;
    prCmdInfo->u4InformationBufferLength = u4QueryBufferLen;

    // Setup WIFI_CMD_T (payload = CMD_TEST_CTRL_T)
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    pCmdTestCtrl = (P_CMD_TEST_CTRL_T)(prWifiCmd->aucBuffer);
    pCmdTestCtrl->ucAction = 2; // Get ATInfo
    pCmdTestCtrl->u.rRfATInfo.u4FuncIndex = prRfATInfo->u4FuncIndex;
    pCmdTestCtrl->u.rRfATInfo.u4FuncData = prRfATInfo->u4FuncData;

    wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return WLAN_STATUS_SUCCESS;
}


/*******************************************************************************
* FUNCTION
*   query_pkt_tx_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_TEST_TX_STATUS
*   Network driver: OID_IPC_TEST_TX_STATUS
*
* CALLS
*   WLAN_STATUS
*   rftestQueryTestTxStatus( IN  PVOID   pvAdapter,
*                            IN  PVOID   queryBuffer_p,
*                            IN  UINT32  queryBufferLen,
*                            OUT PUINT32 queryInfoLen_p );
*
* PARAMETERS
*   typedef struct
*   {
*     kal_uint32  pkt_sent_count;  // total num sent
*     kal_uint32  pkt_sent_acked;  // acked num
*   } wndrv_test_tx_status_struct;
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_pkt_tx_handlerEX(wndrv_test_tx_status_structEX *tx_status_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
//    kal_uint32                  pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
//    P_HIF_RX_HEADER_T prHifRxHdr;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS prTestStatus;

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    /* Query Transmitted count */
    data.u4FuncIndex = RF_AT_FUNCID_TXCOUNT;
    data.u4FuncData =  0;
    query_pkt_tx_cnt(prAdapter, (PVOID)&data, sizeof(data));

    /* Polling Query Transmitted count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset Tx num */
        tx_status_p->pkt_sent_count     = 0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Transmitted count Event info */
        tx_status_p->pkt_sent_count     = prTestStatus->rATInfo.u4FuncData;
    }

    /* Query Transmitted OK count */
    data.u4FuncIndex = RF_AT_FUNCID_TXOKCOUNT;
    data.u4FuncData =  0;
    query_pkt_tx_cnt(prAdapter, (PVOID)&data, sizeof(data));

    /* Polling Query Transmitted OK count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset Tx ok num */
        tx_status_p->pkt_sent_acked     = 0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Transmitted OK count Event info */
        tx_status_p->pkt_sent_acked     = prTestStatus->rATInfo.u4FuncData;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Set deault count values: TODO(Nelson): is need to set ?  */
    tx_status_p->avgAlc = 0;
    tx_status_p->cckGainControl = 0;
    tx_status_p->ofdmGainControl = 0;

    WNDRV_TRACE5(TRACE_GROUP_10, INFO_WNDRV_QUERY_PKT_TX,
                 "QUERY_PKT_TX: INT: count=%d, acked=%d, alc=%d, gain_control=%d, ofdm_control=%d",
                 tx_status_p->pkt_sent_count,
                 tx_status_p->pkt_sent_acked,
                 tx_status_p->avgAlc,
                 tx_status_p->cckGainControl,
                 tx_status_p->ofdmGainControl);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to retrieve permanent address from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
query_pkt_rcv_cnt(
    IN P_ADAPTER_T  prAdapter,
    OUT PVOID       pvQueryBuffer,
    IN UINT_32      u4QueryBufferLen
)
{
    UINT_8 ucCmdSeqNum;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    P_CMD_TEST_CTRL_T pCmdTestCtrl;
    P_PARAM_MTK_WIFI_TEST_STRUC_T  prRfATInfo;

    ASSERT(prAdapter);

    DEBUGFUNC("query_pkt_rcv_cnt");

    if (u4QueryBufferLen) {
        ASSERT(pvQueryBuffer);
    }

    prRfATInfo = (P_PARAM_MTK_WIFI_TEST_STRUC_T)pvQueryBuffer;

    if (u4QueryBufferLen != sizeof(PARAM_MTK_WIFI_TEST_STRUC_T)) {
        DBGLOG(REQ, ERROR, ("Invalid data. QueryBufferLen: %ld.\n",
                            u4QueryBufferLen));
        return WLAN_STATUS_INVALID_LENGTH;
    }

    // 1. Allocate CMD Info Packet and its Buffer
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, CMD_HDR_SIZE + sizeof(P_CMD_TEST_CTRL_T));
    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // Setup common CMD Info Packet
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pfCmdDoneHandler = nicCmdEventQueryRfTestATInfo;
    prCmdInfo->pfCmdTimeoutHandler = nicOidCmdTimeoutCommon;
    prCmdInfo->fgIsOid = TRUE;
    prCmdInfo->ucCID = CMD_ID_TEST_MODE;
    prCmdInfo->fgSetQuery = FALSE;
    prCmdInfo->fgNeedResp = TRUE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pvInformationBuffer = pvQueryBuffer;
    prCmdInfo->u4InformationBufferLength = u4QueryBufferLen;

    // Setup WIFI_CMD_T (payload = CMD_TEST_CTRL_T)
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    pCmdTestCtrl = (P_CMD_TEST_CTRL_T)(prWifiCmd->aucBuffer);
    pCmdTestCtrl->ucAction = 2; // Get ATInfo
    pCmdTestCtrl->u.rRfATInfo.u4FuncIndex = prRfATInfo->u4FuncIndex;
    pCmdTestCtrl->u.rRfATInfo.u4FuncData = prRfATInfo->u4FuncData;

    wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return WLAN_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to retrieve permanent address from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
query_link_quality(
    IN P_ADAPTER_T  prAdapter,
    OUT PVOID       pvQueryBuffer,
    IN UINT_32      u4QueryBufferLen
)
{
    UINT_8 ucCmdSeqNum;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;

    ASSERT(prAdapter);

    DEBUGFUNC("query_link_quality");

    if (u4QueryBufferLen) {
        ASSERT(pvQueryBuffer);
        DBGLOG(REQ, ERROR, ("Invalid data. QueryBufferLen: %ld.\n",
                            u4QueryBufferLen));
        return WLAN_STATUS_INVALID_LENGTH;
    }

    // 1. Allocate CMD Info Packet and its Buffer
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, CMD_HDR_SIZE);
    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // Setup common CMD Info Packet
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE;
    /* NOTE(Nelson): nicCmdEventQueryLinkQuality() is not needed */
    //prCmdInfo->pfCmdDoneHandler = nicCmdEventQueryLinkQuality;
    prCmdInfo->pfCmdDoneHandler = NULL;
    prCmdInfo->pfCmdTimeoutHandler = nicOidCmdTimeoutCommon;
    prCmdInfo->fgIsOid = TRUE;
    prCmdInfo->ucCID = CMD_ID_GET_LINK_QUALITY;
    prCmdInfo->fgSetQuery = FALSE;
    prCmdInfo->fgNeedResp = TRUE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pvInformationBuffer = pvQueryBuffer;
    prCmdInfo->u4InformationBufferLength = u4QueryBufferLen;

    // Setup WIFI_CMD_T (payload = CMD_TEST_CTRL_T)
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return WLAN_STATUS_SUCCESS;
}
#if 0

/*******************************************************************************
* FUNCTION
*   query_pkt_rx_start
*
* DESCRIPTION
*
*
* PARAMETERS
*   void
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_pkt_rx_start(void)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
    kal_uint32                  pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;

    /* Query Received OK count */
    data.u4FuncIndex = RF_AT_FUNCID_RXOKCNT;
    data.u4FuncData =  0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestQueryAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    /* Query Received Error count */
    data.u4FuncIndex = RF_AT_FUNCID_RXERRORCNT;
    data.u4FuncData =  0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestQueryAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
}
#endif
/*******************************************************************************
* FUNCTION
*   query_pkt_rx_handler
*
* DESCRIPTION
*   Corresponding to:
*   IPN2128 Test DLL: OID_CUSTOM_TEST_RX_STATUS
*                     FER = pauCrcErrCount / ( pauCrcErrCount + intRxOk );
*   MT5911 Test DLL:  OID_IPC_TEST_RX_STATUS
*                     FER = pauCrcErrCount / ( pauRxPktCount );
*   Network driver:   OID_IPC_TEST_RX_STATUS
*
* CALLS
*   WLAN_STATUS
*   rftestQueryTestRxStatus( IN  PVOID   pvAdapter,
*                            IN  PVOID   queryBuffer_p,
*                            IN  UINT32  queryBufferLen,
*                            OUT PUINT32 queryInfoLen_p );
*
* PARAMETERS
*   wndrv_test_rx_status_struct *rx_status_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_pkt_rx_handler(wndrv_test_rx_status_struct *rx_status_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T data;
//    kal_uint32                  pdu_len;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_HIF_RX_HEADER_T prHifRxHdr;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS prTestStatus;
    //P_EVENT_LINK_QUALITY prLinkQuality;

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    /* Query Received OK count */
    data.u4FuncIndex = RF_AT_FUNCID_RXOKCNT;
    data.u4FuncData =  0;
    query_pkt_rcv_cnt(prAdapter, (PVOID)&data, sizeof(data));

    /* Polling Query Received OK count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset ok num */
        rx_status_p->int_rx_ok_num           = 0;
        rx_status_p->pau_rx_pkt_count        = 0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Received OK count Event info */
        rx_status_p->int_rx_ok_num           = prTestStatus->rATInfo.u4FuncData;
        rx_status_p->pau_rx_pkt_count        = prTestStatus->rATInfo.u4FuncData;
    }

    /* Query Received Error count */
    data.u4FuncIndex = RF_AT_FUNCID_RXERRORCNT;
    data.u4FuncData =  0;
    query_pkt_rcv_cnt(prAdapter, (PVOID)&data, sizeof(data));

    /* Polling Query Received Error count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset ok num */
        rx_status_p->int_crc_err_num          = 0;
        rx_status_p->pau_crc_err_count        = 0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Received Error count Event info */
        rx_status_p->int_crc_err_num           = prTestStatus->rATInfo.u4FuncData;
        rx_status_p->pau_crc_err_count        = prTestStatus->rATInfo.u4FuncData;

        /* Update Query Received OK count Event info */
        rx_status_p->int_rx_ok_num           += prTestStatus->rATInfo.u4FuncData;
        rx_status_p->pau_rx_pkt_count        += prTestStatus->rATInfo.u4FuncData;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

#if 0
    /* Query RSSI */
    query_link_quality(prAdapter, 0, 0);

    /* Polling EVENT_LINK_QUALITY */
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_LINK_QUALITY),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* do nothing */
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prLinkQuality = (EVENT_LINK_QUALITY)(prEvent->aucBuffer);
        if (prLinkQuality->cLinkQuality > g_rssi_max) {
            g_rssi_max = prLinkQuality->cLinkQuality;
        }

        if (prLinkQuality->cLinkQuality < g_rssi_min) {
            g_rssi_min = prLinkQuality->cLinkQuality;
        }

        g_rssi_sum += prLinkQuality->cLinkQuality;
        g_rssi_num ++;

        /* RSSI variance calculation */
        for (u4Cont = 0; u4Cont < WNDRV_MAX_NUM_RSSI_LQ_RECORDS; u4Cont++) {
            g_rssi_variance += (prStatus->ai4RssiRecord[u4Cont] - u4RssiAvg) * (prStatus->ai4RssiRecord[u4Cont] - u4RssiAvg);
        }
        u4RssiVar /= (INT_32)(u4Cont - 1);
    }

    rx_status_p->int_rssi_max       = g_rssi_max;
    rx_status_p->int_rssi_min       = g_rssi_min;
    rx_status_p->int_rssi_mean      = g_rssi_sum / g_rssi_num;
    rx_status_p->int_rssi_variance  = g_rssi_variance;
#endif

    /* Set deault count values: TODO(Nelson): map to which register  */
    rx_status_p->pau_cca_count           = 0;
    rx_status_p->pau_rx_fifo_full_count  = 0;
    rx_status_p->int_long_preamble_num   = 0;
    rx_status_p->int_short_preamble_num  = 0;
    rx_status_p->int_rssi_max       = 0;
    rx_status_p->int_rssi_min       = 0;
    rx_status_p->int_rssi_mean      = 0;
    rx_status_p->int_rssi_variance  = 0;

    kalMemSet(&rx_status_p->int_rate_ok_num, 0, WNDRV_TEST_RATE_54M + 1);
    kalMemSet(&rx_status_p->int_rate_crc_err_num, 0, WNDRV_TEST_RATE_54M + 1);
    kalMemSet(&rx_status_p->int_rate_ok_num_802_11n, 0, WNDRV_TEST_MOD_MCS7 - WNDRV_TEST_RATE_54M);
    kalMemSet(&rx_status_p->int_rate_crc_err_num_802_11n, 0, WNDRV_TEST_MOD_MCS7 - WNDRV_TEST_RATE_54M);

    /* NOTE(lcko): The following fields were not implemented since 5911 */
    kalMemSet(&rx_status_p->packetSize, 0, WNDRV_PACKET_SIZE_COUNT);
    kalMemSet(&rx_status_p->packetType, 0, WNDRV_PACKET_TYPE_COUNT);

    WNDRV_TRACE8(TRACE_GROUP_10, INFO_WNDRV_QUERY_PKT_RX,
                 "QUERY_PKT_RX: INT: ok=%d, crcErr=%d, short=%d, long=%d. PAU: num=%d, crcErr=%d, fifoFull=%d, cca=%d",
                 rx_status_p->int_rx_ok_num,
                 rx_status_p->int_crc_err_num,
                 rx_status_p->int_short_preamble_num,
                 rx_status_p->int_long_preamble_num,
                 rx_status_p->pau_rx_pkt_count,
                 rx_status_p->pau_crc_err_count,
                 rx_status_p->pau_rx_pkt_count,
                 rx_status_p->pau_cca_count);
}

/*******************************************************************************
* FUNCTION
*   query_ch_id_list_handler
*
* DESCRIPTION
*   Corresponding to:
*   IPN2128 Test DLL: OID_IPC_TEST_CHANNEL_LIST
*   MT5911 Test DLL:  None
*   IPN2128 Network driver: OID_IPC_TEST_CHANNEL_LIST
*   MT5911 Network driver: ipcoidQueryOperatingChannelList
*
* CALLS
*   WLAN_STATUS
*   ipcoidQueryOperatingChannelList( IN  PVOID   pvAdapter,
*                                    IN  PVOID   queryBuffer_p,
*                                    IN  UINT32  queryBufferLen,
*                                    OUT PUINT32 queryInfoLen_p );
*
* PARAMETERS
*   wndrv_test_ch_id_list_struct *list_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_ch_id_list_handler(wndrv_test_ch_id_list_struct *list_p)
{

    IPC_SUPPORTED_CH_LIST  *query_p;
    UINT_32        i;
    PUINT_8     cp;
    P_ADAPTER_T adapter_p = wndrv_context.adpt_p;

    WNDRV_ASSERT(sizeof(wndrv_test_ch_id_list_struct) == sizeof(IPC_SUPPORTED_CH_LIST));

    query_p  = (IPC_SUPPORTED_CH_LIST *) list_p;

    DEBUGFUNC("ipcoidQueryOperatingChannelList");

    ((PIPC_SUPPORTED_CH_LIST) query_p)->numberOfItems = 14;

    cp = ((PIPC_SUPPORTED_CH_LIST) query_p)->channel;

    for (i = 0; i < adapter_p->u2NicOpChnlNum; i++) {
        *cp++ = i + 1;
    }

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_QUERY_CH_ID_LIST,
                 "QUERY_CH_ID_LIST: num=%d",  list_p->ch_num);

}


/*******************************************************************************
* FUNCTION
*   query_rx_counter_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       none
*   Network driver: none
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
static void query_rx_counter_handler(wndrv_test_rx_counter_struct *counter_p)
{
    UINT_32 u4Ed = 0, u4Osd = 0, u4Sq1 = 0, u4Sfd = 0, u4Crc = 0;

    //TODO(Nelson): check the correspnding registers of MT5931
#if 0
    //20081008 add by saker for META
    /* Get BB counter values */
    halGetPhyOperationCouters(wndrv_context.adpt_p, &u4Sfd, &u4Osd, &u4Sq1, &u4Ed, &u4Crc);
    /* Reset BB Counter*/
    halResetPhyOperationCounters(wndrv_context.adpt_p);
#endif

    counter_p->ED     = u4Ed;
    counter_p->OSD    = u4Osd;
    counter_p->SQ1    = u4Sq1;
    counter_p->SFD    = u4Sfd;
    counter_p->CRC16  = u4Crc;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_QUERY_RX_COUNTER, "QUERY_RX_COUNTER");
}


/*******************************************************************************
* FUNCTION
*   query_dbm2dac_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       none
*   Network driver: none
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
static void query_dbm2dac_handler(kal_uint16 *tx_gain_dac_p)
{
    *tx_gain_dac_p = wndrv_tx_dac_value;

    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_QUERY_DBM2DAC,
                 "QUERY_DBM2DAC: dac=%d", *tx_gain_dac_p);
}


/*******************************************************************************
* FUNCTION
*   query_mcr32_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_MAC_REG_RW
*   Network driver: OID_IPC_MCR_RW
*
* CALLS
*   WLAN_STATUS
*   ipcoidQueryMcrReadWrite( IN  PMP_ADAPTER adapter_p,
*                            IN  PVOID       queryBuffer_p,
*                            IN  UINT_32     queryBufferLen,
*                            OUT PUINT_32    queryInfoLen_p );
*
* PARAMETERS
*   peer_buff_struct           *in_p
*   peer_buff_struct          **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_mcr32_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
    peer_buff_struct                   *p;
    wndrv_test_peer_buff_mcr32_struct  *query_p;
    wndrv_test_peer_buff_mcr32_struct  *result_p;
    kal_uint32                          number;
    kal_uint16                          pdu_len;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
//   P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;
    UINT_32 u4BufLen;

    P_WIFI_EVENT_T prEvent;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_ACCESS_REG prRegStatus;
    UINT_32 u4RxPktLength;

    query_p = (wndrv_test_peer_buff_mcr32_struct *)get_pdu_ptr(in_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_mcr32_struct);

    /*p = construct_peer_buff ( pdu_len, 0, 0, TD_UL );*/
    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);

    result_p = (wndrv_test_peer_buff_mcr32_struct *)get_pdu_ptr(p, &pdu_len);

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    while (number > 0) {
        /* Be sure MCR access is in MCR addressing space. */
        //WNDRV_ASSERT( query_p->mcr_index <= 0x512 );
        /* Be sure MCR address is 4 byte boundary. */
        WNDRV_ASSERT((query_p->mcr_index & 0x03) == 0x00);

        result_p->mcr_index = query_p->mcr_index;
        rMcrInfo.u4McrOffset = query_p->mcr_index;

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
            kalMainThread(prAdapter->prGlueInfo);

            /* Polling Query Transmitted count Event */
            memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
            if (nicRxWaitResponse(prAdapter,
                                  g_hif_rx_event_port_num,
                                  aucBuffer,
                                  EVENT_HDR_SIZE + sizeof(EVENT_ACCESS_REG),
                                  &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
                /* Error handling: reset Tx num */
                result_p->mcr32 =  0;
            } else {
                prEvent = (P_WIFI_EVENT_T)aucBuffer;
                prRegStatus = (P_EVENT_ACCESS_REG)(prEvent->aucBuffer);

                /* Save Query Reg Event info */
                result_p->mcr32 = prRegStatus->u4Data;
            }
        } else {
            //20081006 modify by saker
            result_p->mcr32 = rMcrInfo.u4McrData;
        }

        number--;
        query_p++;
        result_p++;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Return the results in peer buffer */
    *out_p = p;
}


/*******************************************************************************
* FUNCTION
*   query_mcr16_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_MAC_REG_RW
*   Network driver: OID_IPC_MCR_RW
*
* CALLS
*   WLAN_STATUS
*   ipcoidQueryMcrReadWrite( IN  PMP_ADAPTER adapter_p,
*                            IN  PVOID       queryBuffer_p,
*                            IN  UINT_32     queryBufferLen,
*                            OUT PUINT_32    queryInfoLen_p );
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_mcr16_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
#if 0
    peer_buff_struct                   *p;
    wndrv_test_peer_buff_mcr16_struct  *query_p;
    wndrv_test_peer_buff_mcr16_struct  *result_p;
    kal_uint32                          number;
    kal_uint16                          pdu_len;

    query_p = (wndrv_test_peer_buff_mcr16_struct *)get_pdu_ptr(in_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_mcr16_struct);

    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);
    result_p = (wndrv_test_peer_buff_mcr16_struct *)get_pdu_ptr(p, &pdu_len);

    while (number > 0) {
        /* Be sure MCR access is in MCR addressing space. */
        //WNDRV_ASSERT( query_p->mcr_index <= 0x512 );
        /* Be sure MCR address is 2 byte boundary. */
        WNDRV_ASSERT((query_p->mcr_index & 0x01) == 0x00);

        result_p->mcr_index = query_p->mcr_index;

        read_mcr_reg16(result_p->mcr_index, &result_p->mcr16);

        number--;
        query_p++;
        result_p++;
    }

    /* Return the results in peer buffer */
    *out_p = p;
#endif
}


/*******************************************************************************
* FUNCTION
*   query_bbcr_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       OID_CUSTOM_MMI_RW
*   Network driver: OID_IPC_BBCR_RW
*
* CALLS
*   WLAN_STATUS
*   ipcoidQueryBbcrReadWrite( IN  PMP_ADAPTER adapter_p,
*                             IN  PVOID       queryBuffer_p,
*                             IN  UINT_32     queryBufferLen,
*                             OUT PUINT_32    queryInfoLen_p );
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_bbcr_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
#if 0
    peer_buff_struct                   *p;
    wndrv_test_peer_buff_bbcr_struct   *query_p;
    wndrv_test_peer_buff_bbcr_struct   *result_p;
    kal_uint32                          number;
    kal_uint16                          pdu_len;

    query_p = (wndrv_test_peer_buff_bbcr_struct *)get_pdu_ptr(in_p, &pdu_len);

    number = pdu_len / sizeof(wndrv_test_peer_buff_bbcr_struct);

    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);
    result_p = (wndrv_test_peer_buff_bbcr_struct *)get_pdu_ptr(p, &pdu_len);

    while (number > 0) {
        /* Be sure MCR access is in MCR addressing space. */
        WNDRV_ASSERT(query_p->bbcr_index <=  0x7f);

        result_p->bbcr_index = query_p->bbcr_index;

        read_bbcr_reg8(result_p->bbcr_index, &result_p->bbcr);

        number--;
        query_p++;
        result_p++;
    }

    /* Return the results in peer buffer */
    *out_p = p;
#endif
}


/*******************************************************************************
* FUNCTION
*   query_eeprom_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       None
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_eeprom_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
    PARAM_CUSTOM_EEPROM_RW_STRUC_T data;
    kal_uint32                    data_len;
    kal_uint16                    pdu_len;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_ACCESS_EEPROM prEepromStatus;

    peer_buff_struct                    *p;
    wndrv_test_peer_buff_eeprom_struct  *query_p;
    wndrv_test_peer_buff_eeprom_struct  *result_p;
    kal_uint32                           number;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
//    P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    query_p = (wndrv_test_peer_buff_eeprom_struct *)get_pdu_ptr(in_p, &pdu_len);
    number = pdu_len / sizeof(wndrv_test_peer_buff_eeprom_struct);
    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);

    result_p = (wndrv_test_peer_buff_eeprom_struct *)get_pdu_ptr(p, &pdu_len);

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    while (number > 0) {
        /* Be sure EEPROM access is in EEPROM addressing space. */
        WNDRV_ASSERT(query_p->eeprom_index < 256);

        result_p->eeprom_index = query_p->eeprom_index;

        data.ucEepromIndex = (UINT_8) query_p->eeprom_index;
        data.ucEepromMethod = PARAM_EEPROM_READ_METHOD_READ;

        GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        wlanSetInformation(prAdapter,
                           wlanoidQueryEepromRead,
                           (PVOID)&data,
                           sizeof(data),
                           (PUINT_32)&data_len);

        GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

        /* Polling Query Transmitted count Event */
        memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
        if (nicRxWaitResponse(prAdapter,
                              g_hif_rx_event_port_num,
                              aucBuffer,
                              EVENT_HDR_SIZE + sizeof(EVENT_ACCESS_EEPROM),
                              &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
            /* Error handling: reset Tx num */
            result_p->eeprom =  0;
        } else {
            prEvent = (P_WIFI_EVENT_T)aucBuffer;
            prEepromStatus = (P_EVENT_ACCESS_EEPROM)(prEvent->aucBuffer);

            /* Save Query EEPROM Event info */
            result_p->eeprom = prEepromStatus->u2Data;
        }

        number--;
        query_p++;
        result_p++;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Return the results in peer buffer */
    *out_p = p;
}


/*******************************************************************************
* FUNCTION
*   query_themo_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       None
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_themo_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
//    kal_uint32                    data_len;
    kal_uint16                    pdu_len;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS prTestStatus;

    peer_buff_struct                    *p;
    //wndrv_test_peer_buff_them_sens_struct  *query_p;
    wndrv_test_peer_buff_them_sens_struct  *result_p;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    get_pdu_ptr(in_p, &pdu_len);
    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);

    result_p = (wndrv_test_peer_buff_them_sens_struct *)get_pdu_ptr(p, &pdu_len);

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_THERMO;
    data.u4FuncData = 0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    /* Polling Query Transmitted count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset Tx num */
        result_p->u4ThemSensVal =  0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Thermo Event info */
        result_p->u4ThemSensVal = prTestStatus->rATInfo.u4FuncData;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Return the results in peer buffer */
    *out_p = p;
}


/*******************************************************************************
* FUNCTION
*   query_volt_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       None
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_volt_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
    //kal_uint32                    data_len;
    kal_uint16                    pdu_len;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS prTestStatus;

    peer_buff_struct                    *p;
    //wndrv_test_peer_buff_volt_sens_struct  *query_p;
    wndrv_test_peer_buff_volt_sens_struct  *result_p;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    get_pdu_ptr(in_p, &pdu_len);
    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);

    result_p = (wndrv_test_peer_buff_volt_sens_struct *)get_pdu_ptr(p, &pdu_len);

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_VOLT;
    data.u4FuncData = 0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    /* Polling Query Transmitted count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset Tx num */
        result_p->u4VoltSensVal =  0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Thermo Event info */
        result_p->u4VoltSensVal = prTestStatus->rATInfo.u4FuncData;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Return the results in peer buffer */
    *out_p = p;
}


/*******************************************************************************
* FUNCTION
*   query_tssi_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       None
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_tssi_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
    //kal_uint32                    data_len;
    kal_uint16                    pdu_len;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS prTestStatus;

    peer_buff_struct                    *p;
    //wndrv_test_peer_buff_tssi_struct  *query_p;
    wndrv_test_peer_buff_tssi_struct  *result_p;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    get_pdu_ptr(in_p, &pdu_len);
    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);

    result_p = (wndrv_test_peer_buff_tssi_struct *)get_pdu_ptr(p, &pdu_len);

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_TSSI;
    data.u4FuncData = 0;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestSetAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    /* Polling Query Transmitted count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset Tx num */
        result_p->u4Tssi =  0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Thermo Event info */
        result_p->u4Tssi = prTestStatus->rATInfo.u4FuncData;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Return the results in peer buffer */
    *out_p = p;
}


/*******************************************************************************
* FUNCTION
*   query_efuse_handler
*
* DESCRIPTION
*   Corresponding to:
*   Test DLL:       None
*   Network driver: None
*
* CALLS
*   None
*
* PARAMETERS
*   peer_buff_struct *in_p
*   peer_buff_struct **out_p
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
static void query_efuse_handler(peer_buff_struct *in_p, peer_buff_struct **out_p)
{
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
    //kal_uint32                    data_len;
    kal_uint16                    pdu_len;
    P_WIFI_EVENT_T prEvent;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS prTestStatus;

    peer_buff_struct                    *p;
    wndrv_test_peer_buff_efuse_struct  *query_p;
    wndrv_test_peer_buff_efuse_struct  *result_p;

    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    //P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;

    query_p = (wndrv_test_peer_buff_efuse_struct *)get_pdu_ptr(in_p, &pdu_len);
    p = construct_peer_buff(pdu_len, 0, 0, TD_UL);

    result_p = (wndrv_test_peer_buff_efuse_struct *)get_pdu_ptr(p, &pdu_len);

#if 0
    /* Mask interrupt output */
    nicDisableInterrupt(prAdapter);
#endif

    data.u4FuncIndex = RF_AT_FUNCID_EFUSE;
    data.u4FuncData = query_p->u2Offset;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestQueryAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);

    /* Process mailbox, CMD, packet tx here */
    kalMainThread(prAdapter->prGlueInfo);

    /* Polling Query Transmitted count Event */
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if (nicRxWaitResponse(prAdapter,
                          g_hif_rx_event_port_num,
                          aucBuffer,
                          EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                          &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        /* Error handling: reset Tx num */
        result_p->u4Value =  0;
    } else {
        prEvent = (P_WIFI_EVENT_T)aucBuffer;
        prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

        /* Save Query Efuse Event info */
        result_p->u4Value = prTestStatus->rATInfo.u4FuncData;
    }

#if 0
    /* Unmask interrupt output */
    nicEnableInterrupt(prAdapter);
#endif

    /* Return the results in peer buffer */
    *out_p = p;
}


#if 0
static kal_uint8 GetChannelNum(kal_uint32 u4Freq)
{
    kal_uint8 i;
    for (i = 1; i <= 14; i++) {
        if (freqTable24[i] == u4Freq) {
            return i;
        }
    }
}
#endif

/*******************************************************************************
*
*  Global Functions
*
*******************************************************************************/
/*****************************************************************************
* FUNCTION
*   wndrv_test_set_handler
* DESCRIPTION
*   Configure and set parameters to network driver for RF testing.
* PARAMETERS
*   none
* RETURNS
*   none
* GLOBALS AFFECTED
*   none
*****************************************************************************/
void wndrv_test_set_handler(ilm_struct *ilm_ptr)
{
    //P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    wndrv_test_set_req_struct *req_p;
    wndrv_test_set_data_union *set_p;
    kal_int32                 status = 0;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_SET_BEGIN, "SET_BEGIN: ****************");

    req_p = (wndrv_test_set_req_struct *) ilm_ptr->local_para_ptr;
    set_p = &req_p->data;

    switch (req_p->type) {
        //UINT_32  u4ModulationMode = 0;

        case WNDRV_TEST_SET_TEST_MODE:
            set_test_mode_handler(set_p->test_mode);
            break;

        case WNDRV_TEST_SET_STOP:
            status = set_stop_handler();
            break;

        case WNDRV_TEST_SET_CONT_TX:
            set_channel(set_p->cont_tx.ch_freq);
            set_gain(set_p->cont_tx.tx_gain_dac);
            set_tx_antenna(set_p->cont_tx.txAnt);
            set_cont_tx_handler(&set_p->cont_tx);
            break;

        case WNDRV_TEST_SET_LOCAL_FREQ:
            set_channel(set_p->local_freq.ch_freq);
            set_gain(set_p->local_freq.tx_gain_dac);
            set_tx_antenna(set_p->local_freq.txAnt);
            set_local_freq_handler();
            break;

        case WNDRV_TEST_SET_CARRIER_SUPP:
            set_channel(set_p->carrier_supp.ch_freq);
            set_gain(set_p->carrier_supp.tx_gain_dac);
            set_tx_antenna(set_p->carrier_supp.txAnt);
            set_carrier_supp_handler(&set_p->carrier_supp);
            break;

        case WNDRV_TEST_SET_PKT_TX:
            set_channel(set_p->local_freq.ch_freq);
            status = set_pkt_tx_handlerEX(KAL_FALSE, &set_p->pkt_tx);
            break;

        case WNDRV_TEST_SET_PWR_MGT:
            status = set_pwr_mgt_handler(set_p->pwr_mgt);
            break;

        case WNDRV_TEST_SET_PKT_RX:    /* META tool: Rx test Start/Stop */
            set_channel(set_p->pkt_rx.ch_freq);    /* META tool: Channel freq (KHz) */
            set_rx_antenna(set_p->pkt_rx.rxAnt);      /* META tool: RX Antenna */
            set_pkt_rx_handler(set_p->pkt_rx.mode);    /* META tool: Start/Stop */
            break;

        case WNDRV_TEST_SET_REG_DOMAIN:
            status = set_reg_domain_handler(set_p->country_code);
            break;

        case WNDRV_TEST_SET_DBM_TO_DAC:
            //TODO(Nelson)
            break;

        case WNDRV_TEST_SET_MCR32:
            set_mcr32_handler(ilm_ptr->peer_buff_ptr);
            break;

        case WNDRV_TEST_SET_MCR16:
            //TODO(Nelson)
            break;

        case WNDRV_TEST_SET_BBCR:
            //TODO(Nelson)
            break;

        case WNDRV_TEST_SET_EEPROM:
            set_eeprom_handler(ilm_ptr->peer_buff_ptr);
            break;

        case WNDRV_TEST_SET_TX_BW:
#if 1
            set_bandwidth((wndrv_test_rx_bw_struct *)set_p);
#else
            set_bandwidth(ilm_ptr->peer_buff_ptr);
#endif
            break;

        case WNDRV_TEST_SET_TX_GI:
#if 1
            set_tx_gi((wndrv_test_tx_gi_struct *)set_p);
#else
            set_tx_gi(ilm_ptr->peer_buff_ptr);
#endif
            break;

        case WNDRV_TEST_SET_TX_MODE_SEL:
#if 1
            set_tx_preemble((wndrv_test_tx_mode_sel_struct *)set_p);
#else
            set_tx_preemble(ilm_ptr->peer_buff_ptr);
#endif
            break;

        case WNDRV_TEST_SET_RX_BW:
#if 1
            set_bandwidth((wndrv_test_rx_bw_struct *)set_p);
#else
            set_bandwidth(ilm_ptr->peer_buff_ptr);
#endif
            break;

        default:
            WNDRV_EXT_ASSERT(KAL_FALSE, "wndrv_test_set_handler: Unexpected req_p->type", req_p->type, 0, 0);
            break;
    }

    if (wndrv_context.glue_p->meta_mode) {
        wndrv_test_set_cnf_struct *cnf_p;
        ilm_struct                *ilm_p;

        /*cnf_p = (wndrv_test_set_cnf_struct *)
               construct_local_para( sizeof( wndrv_test_set_cnf_struct ), TD_CTRL );*/
        cnf_p = (wndrv_test_set_cnf_struct *)
                construct_local_para(sizeof(wndrv_test_set_cnf_struct), TD_CTRL);
        cnf_p->token      = req_p->token;
        cnf_p->type       = req_p->type;
        cnf_p->successful = status;

        ilm_p                 = allocate_ilm(MOD_WNDRV);
        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_CNF;//MSG_ID_WNDRV_TEST_SET_CNF
        ilm_p->local_para_ptr = (local_para_struct *) cnf_p;
        ilm_p->peer_buff_ptr  = NULL;

        WNDRV_SEND_MSG_TO_FT(ilm_p);
    }
    WNDRV_TRACE1(TRACE_GROUP_10, INFO_WNDRV_SET_END,
                 "SET_END: successful=%d ******", status);
}


/*****************************************************************************
* FUNCTION
*   wndrv_test_query_handler
* DESCRIPTION
*   Query responses or status of network driver for RF testing.
* PARAMETERS
*   none
* RETURNS
*   none
* GLOBALS AFFECTED
*   none
*****************************************************************************/
void wndrv_test_query_handler(ilm_struct *ilm_ptr)
{
    wndrv_test_query_req_struct   *query_p;
    wndrv_test_query_cnf_struct   *cnf_p;
    wndrv_test_query_result_union *result_p;
    peer_buff_struct              *cnf_peer_p;

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_QUERY_BEGIN, "QUERY_BEGIN: ****************");

    query_p = (wndrv_test_query_req_struct *) ilm_ptr->local_para_ptr;

    /*cnf_p = (wndrv_test_query_cnf_struct *)
            construct_local_para( sizeof( wndrv_test_query_cnf_struct ), TD_CTRL );*/
    cnf_p = (wndrv_test_query_cnf_struct *)
            construct_local_para(sizeof(wndrv_test_query_cnf_struct), TD_CTRL);
    cnf_p->token = query_p->token;
    cnf_p->type  = query_p->type;
    result_p     = &cnf_p->result;

    cnf_peer_p   = NULL;

    switch (query_p->type) {
        case WNDRV_TEST_QUERY_PKT_TX:
            query_pkt_tx_handlerEX(&result_p->pkt_tx);
            break;

        case WNDRV_TEST_QUERY_PKT_RX:  /* META tool: Rx packet counts info */
            query_pkt_rx_handler(&result_p->pkt_rx);
            break;

        case WNDRV_TEST_QUERY_CH_ID_LIST:  /* META tool: Channel ID list */
            query_ch_id_list_handler(&result_p->ch_id_list);
            break;

        case WNDRV_TEST_QUERY_RX_COUNTER:  /* META tool: ED, OSD, SQ1, SFD, CRC16 resiters info */
            query_rx_counter_handler(&result_p->rx_counter);
            break;

        case WNDRV_TEST_QUERY_DBM_TO_DAC:
            query_dbm2dac_handler(&result_p->tx_gain_dac);
            break;

        case WNDRV_TEST_QUERY_MCR32:
            query_mcr32_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        case WNDRV_TEST_QUERY_MCR16:
            query_mcr16_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;


        case WNDRV_TEST_QUERY_BBCR:
            query_bbcr_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        case WNDRV_TEST_QUERY_EEPROM:
            query_eeprom_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        case WNDRV_TEST_QUERY_DAC_IQ_OFFSET_CAL:
            break;

        case WNDRV_TEST_QUERY_THEM_SENS:
            query_themo_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        case WNDRV_TEST_QUERY_VOLT_SENS:
            query_volt_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        case WNDRV_TEST_QUERY_TSSI:
            query_tssi_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        case WNDRV_TEST_QUERY_EFUSE:
            query_efuse_handler(ilm_ptr->peer_buff_ptr, &cnf_peer_p);
            break;

        default:
            WNDRV_EXT_ASSERT(KAL_FALSE, "wndrv_test_query_handler: Unexpected query_p->type", query_p->type, 0, 0);
            break;
    }

    if (wndrv_context.glue_p->meta_mode) {
        ilm_struct *ilm_p = allocate_ilm(MOD_WNDRV);   //MOD_WNDRV

        ilm_p->msg_id         = MSG_ID_WNDRV_TEST_QUERY_CNF;//MSG_ID_WNDRV_TEST_QUERY_CNF
        ilm_p->local_para_ptr = (local_para_struct *) cnf_p;
        ilm_p->peer_buff_ptr  = cnf_peer_p;

        WNDRV_SEND_MSG_TO_FT(ilm_p);
    } else {
        //free_local_para( (local_para_struct*)cnf_p );
        free_local_para((local_para_struct *)cnf_p);
        if (cnf_peer_p != NULL) {
            //free_peer_buff( cnf_peer_p );
            free_peer_buff(cnf_peer_p);
        }
    }

    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_QUERY_END, "QUERY_END: ****************");
}


/*****************************************************************************
* FUNCTION
*   wndrv_set_test_mode
* DESCRIPTION
*   Set Test mode
* PARAMETERS
*   none
* RETURNS
*   none
* GLOBALS AFFECTED
*   none
*****************************************************************************/
void wndrv_test_set_test_mode(void)
{
    kal_uint32 setInfoLen;
    P_REG_INFO_T prRegInfo = &wndrv_context.adpt_p->prGlueInfo->rRegInfo;
    WLAN_STATUS status = WLAN_STATUS_SUCCESS;

    //4 <1> Initialize the adapter
    if (wndrv_context.glue_p->fgIsAdaptStopped == TRUE) {
        status = wlanAdapterStart(wndrv_context.adpt_p, prRegInfo, (PVOID)wndrv_firmware_p, wndrv_firmware_size);
        wndrv_context.glue_p->fgIsAdaptStopped = FALSE;
    }

    //4 <2> Set driver to switch into RF test mode
    if (status == WLAN_STATUS_SUCCESS) {
        rftestSetTestMode(wndrv_context.adpt_p, NULL, 0, (PUINT_32)&setInfoLen);
    }
}

/*****************************************************************************
* FUNCTION
*   wndrv_set_normal_mode
* DESCRIPTION
*   Set Normal mode
* PARAMETERS
*   none
* RETURNS
*   none
* GLOBALS AFFECTED
*   none
*****************************************************************************/
void wndrv_test_set_normal_mode(void)
{
    kal_uint32 setInfoLen;

    rftestSetAbortTestMode(wndrv_context.adpt_p, NULL, 0, (PUINT_32) &setInfoLen);

#if IS_BB_CHIP_MT5911

    kal_int32 status;
    kal_uint32 setInfoLen;
    kal_uint32 i;

    write_bbcr_reg8(81,  wndrv_BBCRSave[0]);
    write_bbcr_reg8(82,  wndrv_BBCRSave[1]);
    write_bbcr_reg8(83,  wndrv_BBCRSave[2]);
    write_bbcr_reg8(84,  wndrv_BBCRSave[3]);
    write_bbcr_reg8(85,  wndrv_BBCRSave[4]);
    write_bbcr_reg8(88,  wndrv_BBCRSave[5]);
    write_bbcr_reg8(89,  wndrv_BBCRSave[6]);
    write_bbcr_reg8(90,  wndrv_BBCRSave[7]);

    for (i = 0 ; i < 6 ; i++) {
        write_bbcr_reg8(43, i);
        write_bbcr_reg8(44, wndrv_CCKTxFilter[i]);
    }

    for (i = 0 ; i < 10 ; i++) {
        write_bbcr_reg8(41, i);
        write_bbcr_reg8(42, wndrv_OFDMTxFilter[i]);
    }

    write_mcr_reg32(MCR_PPMCR, wndrv_PPMCRSave);

    rftestSetAbortTestMode(wndrv_context.adpt_p, NULL, 0, &setInfoLen);

    wlanAdapterRemove((PVOID) wndrv_context.adpt_p);

    status = wlanAdapterStart((PVOID) wndrv_context.adpt_p);
    WNDRV_EXT_ASSERT(status == 0, "wndrv_test_set_normal_mode: wlanAdapterStart fail", status, 0, 0);

#if defined(WNDRV_RADIO_CTRL)
#ifdef __WIFI_CHIP_DYM_POWERON__
    if (TRUE == wndrv_context.adpt_p->radioOn)
#else
    if (FALSE != wndrv_context.adpt_p->swRadioState)
#endif
    {
        wndrv_context.adpt_p->swRadioState = FALSE;
        nicRadioXMITOff(wndrv_context.adpt_p);
    }
#endif

#endif
}
void RFTool_WiFi_EnterTestMode(void)
{
    //kal_uint32 uSavedMask;
    WNDRV_TRACE(TRACE_INFO, "%s\n", __func__);
    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    wndrv_context.glue_p->meta_mode = KAL_TRUE; /* Set to META mode */
    wndrv_context.glue_p->engineer_mode = KAL_TRUE;

    if (wndrv_context.glue_p->fgIsAdaptStopped == FALSE) {
        nicDisableInterrupt(wndrv_context.adpt_p);
    }

    wndrv_test_set_test_mode();

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_EnterNormalMode(void)
{
    //kal_uint32 uSavedMask;
    WNDRV_TRACE(TRACE_INFO, "%s\n", __func__);
    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif

    wndrv_context.glue_p->meta_mode = KAL_FALSE; /* Stop to META mode */
    wndrv_context.glue_p->engineer_mode = KAL_FALSE;

    wndrv_test_set_normal_mode();

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_Stop(void)
{
    WNDRV_TRACE(TRACE_INFO, "%s\n", __func__);
#if 1
    //kal_uint32 uSavedMask;
    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    set_stop_handler();

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
#else
    ilm_struct  *ilm_p;
    wndrv_test_set_req_struct     *req_p;

    req_p = (wndrv_test_set_req_struct *)
            construct_local_para(sizeof(wndrv_test_set_req_struct), TD_CTRL);
    req_p->token = 0;
    req_p->type = WNDRV_TEST_SET_STOP;

    ilm_p                 = allocate_ilm(MOD_FT);       //MOD_L1
    ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;  //MSG_ID_WNDRV_TEST_SET_REQ
    ilm_p->local_para_ptr = (local_para_struct *) req_p;
    ilm_p->peer_buff_ptr  = NULL;

    SEND_ILM(MOD_FT, MOD_WNDRV, MAP_FT_WNDRV_SAP, ilm_p);
#endif
}
void RFTool_WiFi_DacDCOffset(kal_uint8 *i_ch, kal_uint8 *q_ch)
{
#if 0
    kal_uint32 uSavedMask;
    uSavedMask = WiFi_EintMask();

    wndrv_bus_power_control(1);


    getDACOffset(i_ch, q_ch);

    wndrv_bus_power_control(0);

    WiFi_EintUnMask();
#endif
}

void RFTool_WiFi_ContTx(kal_uint8 country_code[2], wndrv_test_tx_struct cont_tx)
{
    wndrv_test_tx_bw_struct tx_bw;
    wndrv_test_tx_gi_struct tx_gi;
    wndrv_test_tx_mode_sel_struct tx_mode_sel;
    //kal_uint32 uSavedMask;

    WNDRV_TRACE(TRACE_INFO, "%s -- txBw:%d, txGI:%d, txMode:%d, ch_freq:%d, tx_gain_dac:%d, txAnt:%d country_code:%c%c\n", __func__, \
                cont_tx.txBw, cont_tx.txGI, cont_tx.txMode, cont_tx.ch_freq, cont_tx.tx_gain_dac, \
                cont_tx.txAnt, country_code[0], country_code[1]);

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // WNDRV_TEST_SET_TX_BW
    tx_bw.txBw = cont_tx.txBw;
    set_bandwidth((wndrv_test_rx_bw_struct *)&tx_bw);

    // WNDRV_TEST_SET_TX_GI
    tx_gi.txGi = cont_tx.txGI;
    set_tx_gi(&tx_gi);

    // WNDRV_TEST_SET_TX_MODE_SEL
    tx_mode_sel.txMode = cont_tx.txMode;
    set_tx_preemble(&tx_mode_sel);

    // WNDRV_TEST_SET_REG_DOMAIN
    set_reg_domain_handler(country_code);

    // WNDRV_TEST_SET_CONT_TX
    set_channel(cont_tx.ch_freq);
    set_gain(cont_tx.tx_gain_dac);
    set_tx_antenna(cont_tx.txAnt);
    set_cont_tx_handler(&cont_tx);

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void RFTool_WiFi_CarrierSupp(kal_uint8 country_code[2], wndrv_test_tx_struct carrier_supp)
{
    wndrv_test_tx_bw_struct tx_bw;
    wndrv_test_tx_gi_struct tx_gi;
    wndrv_test_tx_mode_sel_struct tx_mode_sel;
    //kal_uint32 uSavedMask;
    WNDRV_TRACE(TRACE_INFO, "%s -- txBw:%d, txGI:%d, txMode:%d, ch_freq:%d, tx_gain_dac:%d, txAnt:%d country_code:%c%c\n", __func__, \
                carrier_supp.txBw, carrier_supp.txGI, carrier_supp.txMode, carrier_supp.ch_freq, carrier_supp.tx_gain_dac, \
                carrier_supp.txAnt, country_code[0], country_code[1]);

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // WNDRV_TEST_SET_TX_BW
    tx_bw.txBw = carrier_supp.txBw;
    set_bandwidth((wndrv_test_rx_bw_struct *)&tx_bw);

    // WNDRV_TEST_SET_TX_GI
    tx_gi.txGi = carrier_supp.txGI;
    set_tx_gi(&tx_gi);

    // WNDRV_TEST_SET_TX_MODE_SEL
    tx_mode_sel.txMode = carrier_supp.txMode;
    set_tx_preemble(&tx_mode_sel);

    // WNDRV_TEST_SET_REG_DOMAIN
    set_reg_domain_handler(country_code);

    // WNDRV_TEST_SET_CARRIER_SUPP
    set_channel(carrier_supp.ch_freq);
    set_gain(carrier_supp.tx_gain_dac);
    set_tx_antenna(carrier_supp.txAnt);
    set_carrier_supp_handler(&carrier_supp);

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void RFTool_WiFi_LocalFreq(kal_uint8 country_code[2], wndrv_test_tx_struct local_freq)
{
    wndrv_test_tx_bw_struct tx_bw;
    wndrv_test_tx_gi_struct tx_gi;
    wndrv_test_tx_mode_sel_struct tx_mode_sel;
    //kal_uint32 uSavedMask;

    WNDRV_TRACE(TRACE_INFO, "%s -- txBw:%d, txGI:%d, txMode:%d, ch_freq:%d, tx_gain_dac:%d, txAnt:%d country_code:%c%c\n", __func__, \
                local_freq.txBw, local_freq.txGI, local_freq.txMode, local_freq.ch_freq, local_freq.tx_gain_dac, \
                local_freq.txAnt, country_code[0], country_code[1]);

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // WNDRV_TEST_SET_TX_BW
    tx_bw.txBw = local_freq.txBw;
    set_bandwidth((wndrv_test_rx_bw_struct *)&tx_bw);

    // WNDRV_TEST_SET_TX_GI
    tx_gi.txGi = local_freq.txGI;
    set_tx_gi(&tx_gi);

    // WNDRV_TEST_SET_TX_MODE_SEL
    tx_mode_sel.txMode = local_freq.txMode;
    set_tx_preemble(&tx_mode_sel);

    // WNDRV_TEST_SET_REG_DOMAIN
    set_reg_domain_handler(country_code);

    // WNDRV_TEST_SET_LOCAL_FREQ
    set_channel(local_freq.ch_freq);
    set_gain(local_freq.tx_gain_dac);
    set_tx_antenna(local_freq.txAnt);
    set_local_freq_handler();

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void RFTool_WiFi_PktTx(kal_uint8 country_code[2], wndrv_test_pkt_tx_struct pkt_tx)
{
    wndrv_test_tx_bw_struct tx_bw;
    wndrv_test_tx_gi_struct tx_gi;
    wndrv_test_tx_mode_sel_struct tx_mode_sel;
    wndrv_test_pkt_tx_structEX  pkt_tx_ex;
    //kal_uint32 uSavedMask;

    WNDRV_TRACE(TRACE_INFO, "%s -- txBw:%d, txGI:%d, txMode:%d, ch_freq:%d, tx_gain_dac:%d, txAnt:%d country_code:%c%c\n", __func__, \
                pkt_tx.txBw, pkt_tx.txGI, pkt_tx.txMode, pkt_tx.ch_freq, pkt_tx.tx_gain_dac, \
                pkt_tx.txAnt, country_code[0], country_code[1]);
    WNDRV_TRACE(TRACE_INFO, "%pktLength:%d, pktCount:%d, pktInterval:%d, is_short_preamble:%d, tx_rate:%d, pattern:%d\n", \
                pkt_tx.pktLength, pkt_tx.pktCount, pkt_tx.pktInterval, pkt_tx.is_short_preamble, pkt_tx.tx_rate, \
                pkt_tx.pattern);

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // WNDRV_TEST_SET_TX_BW
    tx_bw.txBw = pkt_tx.txBw;
    set_bandwidth((wndrv_test_rx_bw_struct *)&tx_bw);

    // WNDRV_TEST_SET_TX_GI
    tx_gi.txGi = pkt_tx.txGI;
    set_tx_gi(&tx_gi);

    // WNDRV_TEST_SET_TX_MODE_SEL
    tx_mode_sel.txMode = pkt_tx.txMode;
    set_tx_preemble(&tx_mode_sel);

    // WNDRV_TEST_SET_REG_DOMAIN
    set_reg_domain_handler(country_code);

    // WNDRV_TEST_SET_PKT_TX
    set_channel(pkt_tx.ch_freq);

    pkt_tx_ex.pktLength   = pkt_tx.pktLength;   /* 24~1500 */
    pkt_tx_ex.pktCount    = pkt_tx.pktCount;
    pkt_tx_ex.pktInterval = pkt_tx.pktInterval;
    pkt_tx_ex.tx_gain_dac = (kal_uint8) pkt_tx.tx_gain_dac;
    pkt_tx_ex.txAnt       = pkt_tx.txAnt;
    pkt_tx_ex.is_short_preamble = pkt_tx.is_short_preamble;
    pkt_tx_ex.tx_rate     = (wndrv_test_rate_enum)pkt_tx.tx_rate;
    memcpy(pkt_tx_ex.mac_header, pkt_tx.mac_header, 24);
    pkt_tx_ex.pattern     = pkt_tx.pattern;
    pkt_tx_ex.txFlags     = 0;                  /* default */
    pkt_tx_ex.targetAlc   = 0;                  /* default */
    set_pkt_tx_handlerEX(KAL_FALSE, &pkt_tx_ex);

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void RFTool_WiFi_PktTxEX(kal_uint8 country_code[2], wndrv_test_pkt_tx_structEX pkt_tx)
{
#if 0
    wndrv_bus_power_control(1);

    set_reg_domain_handler(country_code);
    set_channel(pkt_tx.ch_freq);
    set_pkt_tx_handlerEX(KAL_FALSE, &pkt_tx);
#endif
}

void RFTool_WiFi_PktRx(kal_uint32 ch_freq, void (* callback)(wndrv_test_rx_status_struct *result))
{
    //kal_uint32 uSavedMask;
    WNDRV_TRACE(TRACE_INFO, "%s -- ch_freq:%d\n", __func__, ch_freq);

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // WNDRV_TEST_SET_PKT_RX
    set_channel(ch_freq);     /* META tool: Channel freq (KHz) */
    set_rx_antenna(WNDRV_RX_ANT_DIVERSITY_AGC);
    set_pkt_rx_handler(WNDRV_RF_RX_TEST_MODE);
    RFTool_WiFi_PktRx_Callback = callback;

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);

    kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER],
                          KAL_TICKS_1_SEC);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void RFTool_WiFi_BW(kal_uint8 bandwidth)
{
    wndrv_test_rx_bw_struct rx_bw;

    //kal_uint32 uSavedMask;
    WNDRV_TRACE(TRACE_INFO, "%s -- bandwidth:%d\n", __func__, bandwidth);

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // WNDRV_TEST_SET_TX_BW
    rx_bw.rxBw = bandwidth;
    set_bandwidth((wndrv_test_rx_bw_struct *)&rx_bw);

    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void RFTool_WiFi_Query_PktRx(void)
{
    wndrv_test_rx_status_struct result;
    //kal_uint32 uSavedMask;
    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    query_pkt_rx_handler(&result);
    (* RFTool_WiFi_PktRx_Callback)(&result);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_PwrMgt(wndrv_test_pwr_mgt_enum pwr_mgt)
{
    //kal_uint32 uSavedMask;
    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    set_pwr_mgt_handler(pwr_mgt);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_MCR32_Write(kal_uint32 mcr_index, kal_uint32 mcr32)
{
    //kal_uint32 uSavedMask;
    peer_buff_struct                    *set_peer_p;
    wndrv_test_peer_buff_mcr32_struct   *set_p;
    kal_uint16  pdu_len;

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    set_peer_p = construct_peer_buff(sizeof(wndrv_test_peer_buff_mcr32_struct), 0, 0, TD_UL);
    set_p = (wndrv_test_peer_buff_mcr32_struct *)get_pdu_ptr(set_peer_p, &pdu_len);

    set_p->mcr_index = mcr_index;
    set_p->mcr32 = mcr32;
    set_mcr32_handler(set_peer_p);

    free_peer_buff(set_peer_p);

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_MCR32_Read(kal_uint32 mcr_index, kal_uint32 *mcr32)
{
    //kal_uint32 uSavedMask;
    peer_buff_struct                    *query_peer_p, *cnf_peer_p;
    wndrv_test_peer_buff_mcr32_struct   *query_p, *cnf_p;
    kal_uint16  pdu_len;

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    query_peer_p = construct_peer_buff(sizeof(wndrv_test_peer_buff_mcr32_struct), 0, 0, TD_UL);
    query_p = (wndrv_test_peer_buff_mcr32_struct *)get_pdu_ptr(query_peer_p, &pdu_len);

    query_p->mcr_index = mcr_index;
    query_mcr32_handler(query_peer_p, &cnf_peer_p);

    cnf_p = (wndrv_test_peer_buff_mcr32_struct *)get_pdu_ptr(cnf_peer_p, &pdu_len);
    *mcr32 = cnf_p->mcr32;      /* Save MCR32 read result to mcr32 */

    free_peer_buff(query_peer_p);
    free_peer_buff(cnf_peer_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_EFUSE_Write(kal_uint32 efuse_index, kal_uint32 efuse)
{
}
void RFTool_WiFi_EFUSE_Read(kal_uint32 efuse_index, kal_uint32 *efuse)
{
    //kal_uint32 uSavedMask;
    peer_buff_struct                    *query_peer_p, *cnf_peer_p;
    wndrv_test_peer_buff_efuse_struct   *query_p, *cnf_p;
    //kal_uint32  *efuse_result_p;
    kal_uint16  pdu_len;

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    query_peer_p = construct_peer_buff(sizeof(wndrv_test_peer_buff_efuse_struct), 0, 0, TD_UL);
    query_p = (wndrv_test_peer_buff_efuse_struct *)get_pdu_ptr(query_peer_p, &pdu_len);

    query_p->u2Offset = efuse_index;
    query_efuse_handler(query_peer_p, &cnf_peer_p);

    cnf_p = (wndrv_test_peer_buff_efuse_struct *)get_pdu_ptr(cnf_peer_p, &pdu_len);
    *efuse = cnf_p->u4Value;      /* Save MCR32 read result to mcr32 */

    free_peer_buff(query_peer_p);
    free_peer_buff(cnf_peer_p);
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}
void RFTool_WiFi_EEPROM_Write(kal_uint32 eeprom_index, kal_uint16 eeprom)
{
#if 0
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;
    PARAM_CUSTOM_EEPROM_RW_STRUC_T data;
    kal_uint32                    pdu_len;

    WNDRV_ASSERT(eeprom_index < 256);

    wndrv_bus_power_control(1);

    data.ucEepromIndex = (UINT_8) eeprom_index;
    data.u2EepromData = (UINT_16) eeprom;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       wlanoidSetEepromWrite,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
#endif
}
void RFTool_WiFi_EEPROM_Read(kal_uint32 eeprom_index, kal_uint16 *eeprom)
{
#if 0
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
    P_GLUE_INFO_T prGlueInfo = wndrv_context.glue_p;
    PARAM_CUSTOM_EEPROM_RW_STRUC_T data;
    kal_uint32                    pdu_len;

    WNDRV_ASSERT(eeprom_index < 256);

    wndrv_bus_power_control(1);

    data.ucEepromIndex = (UINT_8) eeprom_index;
    data.ucEepromMethod = PARAM_EEPROM_READ_METHOD_READ;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanSetInformation(prAdapter,
                       wlanoidQueryEepromRead,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    *eeprom = data.u2EepromData;
#endif
}
void RFTool_WiFi_MCR16_Write(kal_uint32 mcr_index, kal_uint16 mcr16)
{
}
void RFTool_WiFi_MCR16_Read(kal_uint32 mcr_index, kal_uint16 *mcr16)
{
}
void RFTool_WiFi_BBCR_Write(kal_uint32 bbcr_index, kal_uint8 bbcr)
{
}
void RFTool_WiFi_BBCR_Read(kal_uint32 bbcr_index, kal_uint8 *bbcr)
{
}

