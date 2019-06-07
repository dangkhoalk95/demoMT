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

#include <string.h>

#include "hal_platform.h"
#include "hal_efuse.h"
#include "hal_gpt.h"
#include "type_def.h"

#define PMU_ELDO (PMU_BASE+0x0118)

#define HAL_EFUSE_WRITE_MAGIC (0xA07923B6)

typedef union {
    struct {
        uint32_t VLD:  1;
        uint32_t rsv0: 7;
        uint32_t BUSY: 1;
        uint32_t rsv1: 7;
        uint32_t RD:   1;
        uint32_t rsv2: 7;
        uint32_t RST:  1;
        uint32_t rsv3: 7;
    } b;
    uint32_t w;
} EFUSE_CON1_REGISTER_T;

typedef struct {
    __IO EFUSE_CON1_REGISTER_T EFUSEC_CON1;       /* EFUSEC Control1 */
    __IO uint32_t EFUSEC_CON2;       /* EFUSEC Control2 */
    __IO uint32_t EFUSEC_BPKEY;      /* EFUSE Blowing Protection Key */
    __IO uint32_t EFUSEC_PGMT;       /* EFUSE Blowing Time */
} EFUSE_REGISTER_T;

EFUSE_REGISTER_T *EFUSE_REGISTER = (EFUSE_REGISTER_T *)EFUSE_BASE;

hal_efuse_status_t hal_efuse_write(uint32_t magic, uint16_t addr, const uint8_t *buf, uint8_t len)
{
    uint32_t eldo_en, real_addr, value, i;

    if (buf == NULL || len % 4 || addr & 0x3) {
        return HAL_EFUSE_INVALID_PARAMETER;
    }

    if (magic != HAL_EFUSE_WRITE_MAGIC) {
        return HAL_EFUSE_INVALID_MAGIC;
    }

    real_addr = EFUSE_BASE + addr;

    /* ELDO_EN = 1 */
    eldo_en = DRV_Reg32(PMU_ELDO) | 0x1;
    DRV_WriteReg32(PMU_ELDO, eldo_en);
    hal_gpt_delay_ms(1);

    /* Polling until EFUSE data is valid */
    while(EFUSE_REGISTER->EFUSEC_CON1.b.VLD != 1);

    /* Unlock */
    EFUSE_REGISTER->EFUSEC_BPKEY = HAL_EFUSE_WRITE_MAGIC;

    for (i = 0;i < len;i += 4) {
        /* Write EFUSE data */
        memcpy(&value, buf + i, sizeof(uint32_t));
        DRV_WriteReg32(real_addr + i, value);

        /* Wait until not busy */
        while(EFUSE_REGISTER->EFUSEC_CON1.b.BUSY != 0);
    }

    /* Lock */
    EFUSE_REGISTER->EFUSEC_BPKEY = 0;
    hal_gpt_delay_ms(1);

    /* Re-initialize */
    EFUSE_REGISTER->EFUSEC_CON1.b.RD = 1;

    /* Polling until EFUSE data is valid */
    while(EFUSE_REGISTER->EFUSEC_CON1.b.VLD != 1);

    /* ELDO_EN = 0 */
    eldo_en = DRV_Reg32(PMU_ELDO) & (~0x1);
    DRV_WriteReg32(PMU_ELDO, eldo_en);
    hal_gpt_delay_ms(1);

    return HAL_EFUSE_OK;
}

hal_efuse_status_t hal_efuse_read(uint16_t addr, uint8_t *buf, uint8_t len)
{
    uint32_t real_addr, value, i;

    if (buf == NULL || len % 4 || addr & 0x3) {
        return HAL_EFUSE_INVALID_PARAMETER;
    }

    real_addr = EFUSE_BASE + addr;

    /* Polling until EFUSE data is valid */
    while(EFUSE_REGISTER->EFUSEC_CON1.b.VLD != 1);

    for (i = 0;i < len;i += 4) {
        /* Read EFUSE data */
        value = DRV_Reg32(real_addr + i);
        memcpy(buf + i, &value, sizeof(uint32_t));
    }

    return HAL_EFUSE_OK;
}
