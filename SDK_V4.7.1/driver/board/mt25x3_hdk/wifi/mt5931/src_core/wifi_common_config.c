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

//#include "kal_release.h"          /* Basic data type */

#include "wndrv_cal.h"
#include "wifi_common_config.h"
#include "kal_public_api.h"
#include "nvdm.h"

wndrv_cal_mac_addr_struct wndrv_default_mac_addr = {WIFI_DEFAULT_MAC_ADDERSS};

wndrv_cal_txpwr_2400M_struct wndrv_default_txpwr_2400M = WIFI_DEFAULT_TXPWR_2400M;

wndrv_cal_txpwr_5000M_struct wndrv_default_txpwr_5000M = {
    {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
    }
};

wndrv_cal_dac_dc_offset_struct wndrv_default_dac_dc_offset = WIFI_DEFAULT_DAC_DC_OFFSET;

wndrv_cal_tx_ALC_2400M_struct wndrv_default_tx_ALC_pwr = WIFI_DEFAULT_ALC_POWER;

wndrv_cal_ALC_Slope_2400M_struct wndrv_default_ALC_Slope = WIFI_DEFAULT_ALC_SLOPE;

wndrv_cal_txpwr_cal_free_flow_struct wndrv_default_TPCFF = WIFI_DEFAULT_TPCFF;

wndrv_cal_setting_trim_thermo_struct wndrv_cal_setting_trim_thermo = WIFI_DEFAULT_TRIM_THERMO_VAL;


void *nvram_data_ptr[NVRAM_EF_WNDRV_TOTAL] = {
    &wndrv_default_mac_addr,
    &wndrv_default_txpwr_2400M,
    &wndrv_default_txpwr_5000M,
    &wndrv_default_dac_dc_offset,
    &wndrv_default_tx_ALC_pwr,
    &wndrv_cal_setting_trim_thermo,
    &wndrv_default_ALC_Slope,
    &wndrv_default_TPCFF,
};

WIFI_CAL_ITEM wifi_cal_table[] = {
    {0x00,      "CCKTXPWR0"    },
    {0x01,      "CCKTXPWR1"    },
    {0x02,      "CCKTXPWR2"    },
    {0x03,      "CCKTXPWR3"    },
    {0x04,      "CCKTXPWR4"    },
    {0x05,      "OFDMTXPWR0"   },
    {0x06,      "OFDMTXPWR1"   },
    {0x07,      "OFDMTXPWR2"   },
    {0x08,      "OFDMTXPWR3"   },
    {0x09,      "OFDMTXPWR4"   },
    {0x0a,      "OFDMTXPWR5"   },
    {0x0b,      "FREQOFFSET"   }
};

#define WIFI_CAL_TABLE_SIZE     ( sizeof(wifi_cal_table)/sizeof(WIFI_CAL_ITEM) )

void WiFi_ConfigCalData(WIFI_CAL_TYPE calibrateType, void *data)
{
    switch (calibrateType) {
        case WIFI_CAL_MAC:
            kal_mem_cpy(&wndrv_default_mac_addr, data, sizeof(wndrv_cal_mac_addr_struct));
            break;

        case WIFI_CAL_TX_POWER:
            kal_mem_cpy(&wndrv_default_txpwr_2400M, data, sizeof(wndrv_cal_txpwr_2400M_struct));
            break;

        default:
            ASSERT(calibrateType < WIFI_CAL_TOTAL);
            break;
    }
}

int wifi_get_cal_data(uint8_t calibrateType, uint8_t *data, uint32_t len)
{
    int ret = 0;
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t nvdm_ret = 0;
    char *group_name = "CAL";

    if (calibrateType >= WIFI_CAL_TABLE_SIZE) {
        printc("can't find the cal data item %d\n", calibrateType);
        return -1;
    }

    nvdm_ret = nvdm_read_data_item(group_name, wifi_cal_table[calibrateType].item_name, data, &len);

    if (nvdm_ret == 0) {
        printc("nvdm read cal data %d succeed %d\n", calibrateType, nvdm_ret);
    } else {
        printc("nvdm read cal data %d fail %d\n", calibrateType, nvdm_ret);
        ret = (int)nvdm_ret;
    }
#else
    printc("nvdm not enable\n");
    ret = -1;
#endif

#if 0
    //cal data read from NVDM fail, then we read it from default config.
    if (ret != 0) {
        if (calibrateType < CCKTXPWR_NUM) {
            *data = wndrv_default_txpwr_2400M.CCKTxPWR[calibrateType];
            printc("Read CCKTXPWR ID=%d from default config %d\n", calibrateType, *data);
        } else if (calibrateType < (CCKTXPWR_NUM + OFDMTXPWR_NUM)) {
            *data = wndrv_default_txpwr_2400M.OFDMTxPWR[calibrateType - CCKTXPWR_NUM];
            printc("Read OFDMTXPWR ID=%d from default config %d\n", calibrateType, *data);
        } else {
            *data = wndrv_cal_setting_trim_thermo.ucXtalTrim;
            printc("Read FREQOFFSET ID=%d from default config %d\n", calibrateType, *data);
        }
        ret = 0;
    }
#endif

    return ret;
}

int wifi_set_cal_data(uint8_t calibrateType, uint8_t *data)
{
    int ret = 0;
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t nvdm_ret = 0;
    uint32_t len = 0;
    char *group_name = "CAL";

    if (calibrateType >= WIFI_CAL_TABLE_SIZE) {
        printc("can't find the cal data item %d\n", calibrateType);
        return -1;
    }

    len = strlen((char *) data);

    nvdm_ret = nvdm_write_data_item(group_name, wifi_cal_table[calibrateType].item_name, NVDM_DATA_ITEM_TYPE_STRING, data, len);

    if (nvdm_ret == 0) {
        printc("nvdm write cal data %d succeed %d\n", calibrateType, nvdm_ret);
    } else {
        printc("nvdm write cal data %d fail %d\n", calibrateType, nvdm_ret);
        ret = (int)nvdm_ret;
    }
#else
    printc("nvdm not enable\n");
    ret = -1;
#endif

    return ret;
}

