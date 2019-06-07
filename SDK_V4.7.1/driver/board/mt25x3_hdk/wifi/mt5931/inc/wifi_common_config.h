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

#if defined(__WIFI_SUPPORT__)

#include <stdlib.h>
#include <stdint.h>

#ifndef WIFI_COMMON_CONFIG_H
#define WIFI_COMMON_CONFIG_H

#if defined(MT5931) /* MT5931 default NVRAM setting */
//TODO(Nelson): MT5931 NVRAM default setting

#define WIFI_DEFAULT_EEPROM_SIZE 512

#define WIFI_DEFAULT_TXPWR_2400M \
{ {0x13, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
  {0x10, 0x10, 0x10, 0x10, 0x0E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }

#define WIFI_DEFAULT_MAC_ADDERSS \
{ 0x00, 0x0C, 0xE7, 0x12, 0x34, 0x56 }

#define WIFI_DEFAULT_DAC_DC_OFFSET \
{ 0x00, 0x00 }

#define WIFI_DEFAULT_ALC_POWER \
{      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
       {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
       { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }, \
       { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }, \
}

#define WIFI_DEFAULT_ALC_SLOPE \
{0x02,0,0x0A,0}

#define WIFI_DEFAULT_TPCFF_ENABLE 0
#define SST_PA
//#define MURATA_FEM_PA

#define WIFI_DEFAULT_TPCFF \
{      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
      { {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00}, \
         {0x00, 0x00, 0x00} } \
}

#define WIFI_DEFAULT_LED_AND_PHY_CONFIGURE 0

//#define WIFI_DEFAULT_REG_DOMAIN 0
#define WIFI_DEFAULT_REG_DOMAIN (('U' << 8) | 'S') /* USA         */

#define WIFI_DEFAULT_ANT_MODE 5

#define WIFI_DEFAULT_OSC_STABLE_TIME 1900

#define WIFI_DEFAULT_CONNECTION_KEEP_TIME 20

#define WIFI_DEFAULT_RSSI_UPDATE_TIME 10

#define WIFI_DEFAULT_CHECKLCD 1

#define WIFI_DEFAULT_CONNECTION_RETRY_LIMIT 2

#define WIFI_DEFAULT_LDO_STATE 0

#define WIFI_DEFAULT_ALC_ENABLE 0
#define WIFI_DEFAULT_LNA_ENABLE 0
#define WIFI_DEFAULT_SLOW_CLK_CFG   1
#define WIFI_DEFAULT_DAISY_CHAIN    0x00
#define WIFI_DEFAULT_TRIM_THERMO_VAL \
{0, 0, 0}

#define PS_CONTINUOUS_ACTIVE 0
#define PS_MAX_PSP 1
#define PS_FAST_PSP 2
#define PS_CTIA_TEST 3
#define PS_VOIP_OPTIMIZED_FAST_PSP 2
/* NORMAL MODE ATE */
#define WNDRV_EINT_POLARITY 0
#if defined (MT6268) || defined (MT6236) || defined (MT6236B)
#define WNDRV_EHPI_CE2WR_SETUP_TIME 2
#define WNDRV_EHPI_WR_WAIT_STATE_TIME   3
#define WNDRV_EHPI_RD_LATENCY_TIME 1
#elif defined(MT6253)
#define WNDRV_EHPI_CE2WR_SETUP_TIME   1
#define WNDRV_EHPI_WR_WAIT_STATE_TIME 4
#define WNDRV_EHPI_RD_LATENCY_TIME    3
#elif defined(MT6253E) || defined(MT6252H)
#define WNDRV_EHPI_CE2WR_SETUP_TIME   2
#define WNDRV_EHPI_WR_WAIT_STATE_TIME 6
#define WNDRV_EHPI_RD_LATENCY_TIME    4
#elif defined(MT6252)
#define WNDRV_EHPI_CE2WR_SETUP_TIME   1
#define WNDRV_EHPI_WR_WAIT_STATE_TIME 4
#define WNDRV_EHPI_RD_LATENCY_TIME    3
#elif defined (MT6276) || defined(MT6256)|| defined(MT6255)
#define WNDRV_EHPI_C2RH_TIME 0x2
#define WNDRV_EHPI_C2RS_TIME 0x3
#define WNDRV_EHPI_RLT_TIME  0xD
#define WNDRV_EHPI_C2WH_TIME 0x3
#define WNDRV_EHPI_C2WS_TIME 0x2
#define WNDRV_EHPI_WST_TIME  0xD
#define WNDRV_EHPI_PERIOD    0x0
#else
#define WNDRV_EHPI_CE2WR_SETUP_TIME 3
#define WNDRV_EHPI_WR_WAIT_STATE_TIME   4
#define WNDRV_EHPI_RD_LATENCY_TIME 3
#endif


#if defined (MT6236) || defined (MT6236B) || defined (MT6268) || defined (MT6252H) || defined(MT6252)
#define WNDRV_SET_LCD_DRIVING_CURRENT 1
#else
#define WNDRV_SET_LCD_DRIVING_CURRENT 0
#endif
#define WNDRV_DEFAULT_EINT_SELECTION 0
#define WNDRV_DEFAULT_LED_SETTING  0   /* 0:NONE  1:TX  2:RX  3:TX_RX */
#define WNDRV_DEFAULT_LED_ON_TIME  80  /* ms, range : 0 ~ 1020 */
#define WNDRV_DEFAULT_LED_OFF_TIME 24  /* ms, range : 0 ~ 1020 */
#define WNDRV_DEFAULT_WMMPS_CONFIG 0

/* RSSI Low Threshold */
#define WNDRV_DEFAULT_LOWEST_RSSI (-85)
#define WNDRV_DEFAULT_PATH_LOSS   (0)

/* Sleep period */
#define WNDRV_DEFAULT_DTIM_PERIOD (2)

// BT 1-wire mode PTA timing
#if defined(__BTMODULE_MT6236__) || defined(__BTMODULE_MT6276__)
#define WNDRV_BT_1WIRE_MODE_T6                            (0x8)
#define WNDRV_BT_1WIRE_MODE_T8                            (0x94)
#define WNDRV_BT_1WIRE_MODE_BT_DELAY                      (0x1C)
#elif defined(__BTMODULE_MT6622__) || defined(__BTMODULE_MT6626__) || defined(__BTMODULE_MT6255__) || defined(__BTMODULE_MT6256__) || defined(__BTMODULE_MT6250__)
#define WNDRV_BT_1WIRE_MODE_T6                            (0x8)
#define WNDRV_BT_1WIRE_MODE_T8                            (0x6E)
#define WNDRV_BT_1WIRE_MODE_BT_DELAY                      (0x1A)
#elif defined(__BTMODULE_MT6261__)
//#define WNDRV_BT_1WIRE_MODE_T6                            (8)
//#define WNDRV_BT_1WIRE_MODE_T8                            (14)
//#define WNDRV_BT_1WIRE_MODE_BT_DELAY                      (14)
#define WNDRV_BT_1WIRE_MODE_T6                            (0x8)
#define WNDRV_BT_1WIRE_MODE_T8                            (0x6E)
#define WNDRV_BT_1WIRE_MODE_BT_DELAY                      (0x1A)
#else
// default setting
#define WNDRV_BT_1WIRE_MODE_T6                            (8)
#define WNDRV_BT_1WIRE_MODE_T8                            (110)
#define WNDRV_BT_1WIRE_MODE_BT_DELAY                      (26)
#endif


/* WIFI INTERFACE TYPE */
#define WNDRV_BUS_EHPI_8BIT     0x00000000
#define WNDRV_BUS_EHPI_16BIT    0x00000001

/* HIF SDIO setting */
#define WNDRV_FUNC1_SDIO_BLK_SIZE_512 (512) /* Block Size of WIFI function */
#define WNDRV_SDIO_CLK_FREQ_22M (22000) /* 22MHz */
#define WNDRV_SDIO_CLK_FREQ_44M (44000) /* 44MHz */
#define WNDRV_SDIO_CLK_FREQ_48M (48000) /* 48MHz */
#define WNDRV_SDIO_SEL_CARD1 DCL_SDIO_FLAGS_SDIO1   //DCL_SDIO_FLAGS_DEVICE_CARD1
#define WNDRV_SDIO_SEL_CARD2 DCL_SDIO_FLAGS_SDIO2   //DCL_SDIO_FLAGS_DEVICE_CARD2


#endif /*MT5931*/

/*---- handover timeout ------------- ---------------------------------------*/
#define WNDRV_HANDOVER_TIMEOUT      (30)           /* in second(s), for normal, WPA/WPA2 connection */
#define WNDRV_HANDOVER_TIMEOUT_WPS  (30)          /* in second(s), for WPS connection */
#define WNDRV_HANDOVER_TIMEOUT_WAPI (30)          /* in second(s), for WAPI connection */

//20081110 modify by saker for MT5921 support
#if defined(WIFI_BB_MT5911) || defined(WIFI_BB_MT5921) || defined(MT5931)

typedef  struct {
    unsigned char num_poweronWiFi;
    unsigned char pol_poweronWiFi;
    unsigned char num_lpoclk;
    unsigned char mode_lpoclk;
    unsigned char num_IndicateWiFi;
    unsigned char num_resetWiFi;
} sWiFi_IO;

#endif

typedef enum {
    NVRAM_EF_WNDRV_MAC_ADDRESS_LID = 0,
    NVRAM_EF_WNDRV_TX_POWER_2400M_LID,
    NVRAM_EF_WNDRV_TX_POWER_5000M_LID,
    NVRAM_EF_WNDRV_DAC_DC_OFFSET_LID,
    NVRAM_EF_WNDRV_TX_ALC_POWER_LID,
    NVRAM_EF_WNDRV_EXT_SETTING_TRIMVAL_THERMOVAL_LID,
    NVRAM_EF_WNDRV_ALC_SLOPE_LID,
    NVRAM_EF_WNDRV_TPCFF_LID,
    NVRAM_EF_WNDRV_TOTAL
} nvram_lid;
extern void *nvram_data_ptr[NVRAM_EF_WNDRV_TOTAL];

#define CCKTXPWR_NUM    (5)
#define OFDMTXPWR_NUM   (6)

typedef enum _WIFI_CAL_TYPE {
    WIFI_CAL_MAC = 0,
    WIFI_CAL_TX_POWER,
    WIFI_CAL_TOTAL
} WIFI_CAL_TYPE;

typedef struct _WIFI_CAL_ITEM {
    uint8_t item_index;
    char    item_name[32];
} WIFI_CAL_ITEM;

int wifi_get_cal_data(uint8_t calibrateType, uint8_t *data, uint32_t len);

int wifi_set_cal_data(uint8_t calibrateType, uint8_t *data);

void WiFi_ConfigCalData(WIFI_CAL_TYPE calibrateType, void *data);

#endif // WIFI_COMMON_CONFIG_H
#endif //(__WIFI_SUPPORT__)

