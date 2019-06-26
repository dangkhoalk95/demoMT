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

#include "hal_spm.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include <stdio.h>
#include <string.h>
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "assert.h"
#include "hal_gpt.h"
#include "hal_pmu.h"
#include "hal_clock.h"
#include "hal_platform.h"

/* SPM Code Rev : 20170725 - pcm_suspend.spm */
const unsigned int pcm_event_vector_parameter[8] = {0x31,0x170032,0x580033,0x6f0034,0x7d0035,0x0,0x0,0x0};
#define PCM_IM_MAX_LENGTH 620
static const unsigned int spm_im[PCM_IM_MAX_LENGTH] = {
0x1910001f,0xa20f0810,0x81000404,0x1950001f,0xa20f0810,0x81421401,0xa1401005,0xd82001a5,0x17c07c1f,
0x1b00001f,0x40000001,0xf0000000,0x17c07c1f,0x18c0001f,0xa20f0810,0xe0e00010,0x18c0001f,0xa20f03a0,
0xe0e00000,0x1b00001f,0x80000001,0xf0000000,0x17c07c1f,0x18c0001f,0xa20f0810,0xe0e0000a,0x18c0001f,
0xa20f03a8,0x1910001f,0xa20f03a8,0x89000004,0xfffffeff,0xe0c00004,0x18c0001f,0xa21f0010,0x1900001f,
0x00010001,0xe0c00004,0x18c0001f,0xa20f02c0,0xe0e00001,0x18c0001f,0xa20f0810,0xe0e0000e,0x18d0001f,
0xa21f0010,0x814c0c01,0x1100141f,0xd8200584,0x17c07c1f,0x18c0001f,0xa20f0810,0xe0e0000c,0x1990001f,
0xa20f02c0,0x81841801,0x1910001f,0xa20f03ac,0x81081001,0xa1a01006,0x1100181f,0xd82006a4,0x17c07c1f,
0x18c0001f,0xa20f0810,0xe0e00006,0x88d00001,0xa0050000,0xd8200843,0x17c07c1f,0x18c0001f,0xa20f0810,
0xe0e00004,0x18c0001f,0xa20f0204,0x1940001f,0x00000001,0x1910001f,0xa20f03ac,0x81041001,0xc8e01ec4,
0x17c07c1f,0xa0118400,0xa0110400,0x1b00001f,0x042a2001,0xf0000000,0x17c07c1f,0x18c0001f,0xa20f0810,
0xe0e00008,0x18c0001f,0xa20f03a8,0x1910001f,0xa20f03a8,0x89000004,0xfffffefe,0xe0c00004,0x18c0001f,
0xa20f03a0,0xe0e00001,0x1b00001f,0x04122001,0xe8208000,0xa20f0300,0x0b16ff0c,0xe8208000,0xa20f0300,
0x0b16ff08,0xf0000000,0x17c07c1f,0x18c0001f,0xa20f0810,0xe0e00004,0x18c0001f,0xa20f03a8,0x1910001f,
0xa20f03a8,0xa9000004,0x00000001,0xe0c00004,0x1b00001f,0x042a2001,0xf0000000,0x17c07c1f,0x18c0001f,
0xa20f0810,0xe0e00002,0x18c0001f,0xa20f0204,0x1940001f,0x00000001,0x1910001f,0xa20f0808,0x81009001,
0xc8e01464,0x17c07c1f,0x18c0001f,0xa21f0010,0x1900001f,0x00000001,0xe0c00004,0x18c0001f,0xa20f02c0,
0xe0e00000,0x80328400,0x1b80001f,0x2000259f,0x80310400,0x1b80001f,0x20000cda,0x80318400,0x18c0001f,
0xa20f03a8,0x1910001f,0xa20f03a8,0xa9000004,0x00000101,0xe0c00004,0x1b00001f,0x04062001,0xf0000000,
0x17c07c1f,0xe0e00016,0x1b80001f,0x20000001,0xe0e0001e,0x1b80001f,0x20000001,0x81801401,0xd8001686,
0x17c07c1f,0x81809401,0xd8001926,0x17c07c1f,0x81811401,0xd8001ce6,0x17c07c1f,0xd0001da0,0x17c07c1f,
0x1900001f,0xa20f0254,0x1980001f,0x00010101,0xe1000006,0x1b80001f,0x2000000e,0x1980001f,0x00000101,
0xe1000006,0x1b80001f,0x2000000e,0xe1200100,0x1900001f,0xa20f02b0,0x1980001f,0x00010100,0xe1000006,
0xe1200100,0xd0001da0,0x17c07c1f,0x1900001f,0xa20f0230,0xe120003f,0xe120001f,0xe120000f,0xe1200007,
0xe1200003,0xe1200001,0xe1200000,0x1b80001f,0x20000122,0x1900001f,0xa20f0224,0x1980001f,0x00010101,
0xe1000006,0x1b80001f,0x2000000e,0x1980001f,0x00000101,0xe1000006,0x1b80001f,0x2000000e,0xe1200100,
0x1900001f,0xa20f0234,0xe1200100,0xe1200000,0xd0001da0,0x17c07c1f,0x1900001f,0xa20f0260,0x1980001f,
0x00010100,0xe1000006,0xe1200100,0xe0e0000e,0xe0e0000c,0xe0e0001c,0x1b80001f,0x20000004,0xe0e0001d,
0xe0e0000d,0xf0000000,0x17c07c1f,0xe0e0001d,0xe0e0001f,0x81801401,0xd8002066,0x17c07c1f,0x81809401,
0xd8002326,0x17c07c1f,0x81811401,0xd80025c6,0x17c07c1f,0xd00026c0,0x17c07c1f,0x1900001f,0xa20f0254,
0xe1200101,0x1980001f,0x00010101,0xe1000006,0x1b80001f,0x2000000e,0x1980001f,0x00010001,0xe1000006,
0x1b80001f,0x2000000e,0x1900001f,0xa20f02b0,0x1980001f,0x00010100,0xe1000006,0x1980001f,0x00010101,
0xe1000006,0xd00026c0,0x1900001f,0xa20f0230,0xe120007f,0x1900001f,0xa20f0224,0xe1200101,0x1980001f,
0x00010101,0xe1000006,0x1b80001f,0x2000000e,0x1980001f,0x00010001,0xe1000006,0x1b80001f,0x2000000e,
0x1900001f,0xa20f0234,0xe1200100,0xe120010f,0xd00026c0,0x1900001f,0xa20f0260,0x1980001f,0x00010100,
0xe1000006,0x1980001f,0x00010101,0xe1000006,0xe0e0001e,0xe0e0001a,0xe0e00012,0xf0000000,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x1840001f,
0x00000001,0x18c0001f,0xa20f0810,0xe0e00001,0xa0100400,0xa1d00407,0x18c0001f,0xa20f0380,0xe0f07fff,
0x1b00001f,0x04032001,0x81f10407,0x1b80001f,0xd0100001,0x810f3001,0xd8004cc4,0x17c07c1f,0xe8208000,
0xa20f0300,0x0b16ff08,0x18c0001f,0xa20f0810,0xe0e00002,0x18c0001f,0xa21f0010,0xe0e00001,0x18d0001f,
0xa21f0010,0x81040c01,0xd8204364,0x17c07c1f,0x18c0001f,0xa20f0200,0x1940001f,0x00000002,0xc0c01ec0,
0x17c07c1f,0x18c0001f,0xc0070140,0x1900001f,0x90904bed,0xe0c00004,0xa0120400,0x18c0001f,0xa20f0810,
0xe0e00006,0x1b00001f,0x04062001,0x1b80001f,0x20000003,0x1b80001f,0x90100001,0x18c0001f,0xa20f0810,
0xe0e0000e,0x1b80001f,0x2000259f,0x80310400,0x1b80001f,0x20000cda,0x80318400,0x80320400,0x80328400,
0x1910001f,0xa20f0808,0x81009001,0x18c0001f,0xa20f0204,0x1940001f,0x00000001,0xc8e01464,0x17c07c1f,
0x18c0001f,0xa20f03a8,0x1910001f,0xa20f03a8,0xa9000004,0x00000101,0xe0c00004,0x18c0001f,0xa20f0200,
0x1940001f,0x00000002,0xc0c01460,0x17c07c1f,0x18c0001f,0xa21f0010,0x1900001f,0x00000000,0xe0c00004,
0x18c0001f,0xa20f02c0,0xe0e00000,0x18c0001f,0xc0070140,0x1900001f,0xfee1900d,0xe0c00004,0xe8208000,
0xa20f0300,0x0b16ff08,0x18c0001f,0xa20f0810,0xe0e00000,0x81f00407,0xa1d10407,0xf0000000};

uint32_t manual_spm_read_im(uint32_t addr);
void manual_spm_write_im(uint32_t addr, uint32_t data);
void spm_control_mtcmos_internal(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl);

ATTR_TEXT_IN_TCM void spm_kick_start(void)
{
    hal_clock_enable(HAL_CLOCK_CG_SW_SPM);

    //Need reset SPM ,after wakeup from sleep
    /* [4]: Resets PCM */
    *SPM_PCM_CON0 = 0x0B160000 | 0x10;
    hal_gpt_delay_us(10);
    *SPM_PCM_CON0 = 0x0B160000;
    hal_gpt_delay_us(10);

    //*SPM_SIDEBAND_CONTROL_0 |= (0x0F << 16); // wait UART_SLEEP_ACK = 0xF

    *SPM_POWER_ON_VAL0 = *SPM_PCM_REG0_DATA;
    *SPM_POWER_ON_VAL1 = *SPM_PCM_REG7_DATA;
    *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL0 | 0x1;   // [0]: sc_xo_cg_en default =1

    *SPM_PCM_PWR_IO_EN = 0x00010000;                    // for R0
    *SPM_PCM_PWR_IO_EN = 0x00000000;
    *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL1;
    *SPM_PCM_PWR_IO_EN = 0x00800000;                    // for R7
    *SPM_PCM_PWR_IO_EN = 0x00000000;

    *SPM_PCM_PWR_IO_EN = 0x81;

    *SPM_SLEEP_WAKEUP_EVENT_MASK = 0xFFFFFFFF;          //mask all AP wakeup event

    *SPM_PCM_CON0 = 0x0B160103;
}

void spm_init(uint32_t spm_auto_load)
{
    uint32_t i,j, status;
    uint32_t wdt_status;

    if (HAL_CLOCK_STATUS_OK != hal_clock_enable(HAL_CLOCK_CG_SW_SPM)) {
        log_hal_error("\r\n [SPM] Clock enable failed!");
    }
    /* Workaround for WDT wakeup source signal active after WDT reset happened.
       Do the dummy read to fix warning that the variable is set but never used.
    */
    wdt_status = *WDT_INT_REG;
    wdt_status = wdt_status;

    /* [4]: Resets PCM */
    *SPM_PCM_CON0 = 0x0B160000 | 0x10;
    hal_gpt_delay_us(10);
    *SPM_PCM_CON0 = 0x0B160000;
    hal_gpt_delay_us(10);

    //*SPM_SIDEBAND_CONTROL_0 |= (0x0F << 16); // wait UART_SLEEP_ACK = 0xF

    /* Load PCM_EVENT_VECTOR[0-7] */
    *SPM_PCM_EVENT_VECTOR0 = pcm_event_vector_parameter[0];
    *SPM_PCM_EVENT_VECTOR1 = pcm_event_vector_parameter[1];
    *SPM_PCM_EVENT_VECTOR2 = pcm_event_vector_parameter[2];
    *SPM_PCM_EVENT_VECTOR3 = pcm_event_vector_parameter[3];
    *SPM_PCM_EVENT_VECTOR4 = pcm_event_vector_parameter[4];
    *SPM_PCM_EVENT_VECTOR5 = pcm_event_vector_parameter[5];
    *SPM_PCM_EVENT_VECTOR6 = pcm_event_vector_parameter[6];
    *SPM_PCM_EVENT_VECTOR7 = pcm_event_vector_parameter[7];

    *SPM_POWER_ON_VAL0 = 0;
    *SPM_POWER_ON_VAL1 = 0x44;

    *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL0 | 0x1; // [0]: sc_xo_cg_en default =1

    *SPM_PCM_PWR_IO_EN = 0x00010000; // for R0
    *SPM_PCM_PWR_IO_EN = 0x00000000;
    *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL1;
    *SPM_PCM_PWR_IO_EN = 0x00800000; // for R7
    *SPM_PCM_PWR_IO_EN = 0x00000000;
    *SPM_CLK_SETTLE = 0x3;

    /* Init IM Length and pointer */
    *SPM_PCM_IM_LEN = PCM_IM_MAX_LENGTH;
    *SPM_PCM_IM_PTR = (uint32_t)spm_im;
    if (spm_auto_load != 0) {
        *SPM_PCM_CON1 = 0x0B160001; // Enables IM slave mode
        /* Kick IM process */
        *SPM_PCM_CON0 = 0x0B160002;
    } else {
        /* manual load spm code */
        for (i = 0; i < PCM_IM_MAX_LENGTH; i++) {
            manual_spm_write_im(i, spm_im[i]);
        }

        *SPM_PCM_CON1 = 0x0B163C49;
        /* Kick IM process */
        *SPM_PCM_CON0 = 0x0B160002;
    }

    /* Wait ready state */
    do {
        status = (*SPM_PCM_FSM_STA >> 9) & 0x1;
    } while (status != 0x01);

    /* Read back spm code */
    uint32_t im_check_buf[16];
    for (i = 0; i < PCM_IM_MAX_LENGTH; i+=16) {
        for (j = 0; j < 16; j++) {
            if (i+j < PCM_IM_MAX_LENGTH) {
                im_check_buf[j] = manual_spm_read_im(i+j);
            } else {
                break;
            }
        }
        if (memcmp(im_check_buf, spm_im+i, j * 4) != 0) {
            printf("SPM Code loading Fail\r\n");
            while(1);
        }
    }    
    
    *SPM_PCM_IM_HOST_RW_PTR = 0; 
    *SPM_PCM_PWR_IO_EN = 0x0081;                // enable R0 & R7 output
    *SPM_SEQUENCER_26M_REG_1 = 0x1B;
    *SPM_SEQUENCER_32K_REG_3 = (0x18);          //pmu guard time need check!!!
    
    *SPM_RESOURCE_CONTROL_0 |= 0x00010000;      //Force on CONN MTCMOS
    *SPM_RESOURCE_CONTROL_0 |= 0x01000000;      //Force on SDIO MTCMOS

    *SPM_WAKEUP_SOURCE_MASK = 0x80;             //enable all wakeup source,without debugsys

    if (HAL_CLOCK_STATUS_OK != hal_clock_disable(HAL_CLOCK_CG_SW_SPM)) {
        log_hal_error("\r\n [SPM] Clock disable failed!");
    }
}

ATTR_ZIDATA_IN_TCM uint32_t mtcmos_conn_resource_cnt = 0;
ATTR_ZIDATA_IN_TCM uint32_t mtcmos_sdio_resource_cnt = 0;

uint32_t spm_control_mtcmos(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl)
{
    if (mtcmos == SPM_MTCMOS_CONN) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_conn_resource_cnt == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_conn_resource_cnt--;
            if (mtcmos_conn_resource_cnt == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_conn_resource_cnt++;
            if (mtcmos_conn_resource_cnt != 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_conn_resource_cnt);
    }

    if (mtcmos == SPM_MTCMOS_SDIO_SLV) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_sdio_resource_cnt == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_sdio_resource_cnt--;
            if (mtcmos_sdio_resource_cnt == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_sdio_resource_cnt++;
            if (mtcmos_sdio_resource_cnt != 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_sdio_resource_cnt);
    }
    return 0;
}

void spm_control_mtcmos_internal(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl)
{
    if (mtcmos == SPM_MTCMOS_CONN) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if ((*SPM_PWR_STATUS & 0x4) == 0) {
                return;
            }

            hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
            *SPM_RESOURCE_CONTROL_0 &= 0xFFFEFFFF;   //Force off CONN MTCMOS
            //CONN MTCMOS OFF
            *SPM_CONN_PWR_CON = 0x1D;
            hal_gpt_delay_us(1);
            *SPM_CONN_PWR_CON = 0x1F;
            hal_gpt_delay_us(1);
            *SPM_CONN_PWR_CON = 0x1E;
            hal_gpt_delay_us(1);
            *SPM_CONN_PWR_CON = 0x1A;
            hal_gpt_delay_us(1);
            *SPM_CONN_PWR_CON = 0x12;
            hal_gpt_delay_us(1);
            hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        } else {
            if ((*SPM_PWR_STATUS & 0x4) == 0) {
                if ((*SPM_PWR_STATUS & 0x4) != 0) {
                    return;
                }

                hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
                *SPM_RESOURCE_CONTROL_0 |= 0x00010000;   //Force on CONN MTCMOS
                //CONN MTCMOS ON
                *SPM_CONN_PWR_CON = 0x16;
                hal_gpt_delay_us(1);
                *SPM_CONN_PWR_CON = 0x1E;
                hal_gpt_delay_us(1);
                *SPM_CONN_PWR_CON = 0x0E;
                hal_gpt_delay_us(1);
                *SPM_CONN_PWR_CON = 0x0C;
                hal_gpt_delay_us(1);
                *SPM_CONN_PWR_CON = 0x1C;
                hal_gpt_delay_us(1);
                *SPM_CONN_PWR_CON = 0x1D;
                hal_gpt_delay_us(1);
                *SPM_CONN_PWR_CON = 0x0D;
                hal_gpt_delay_us(1);
                hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
            }
        }
        return;
    }

    if (mtcmos == SPM_MTCMOS_SDIO_SLV) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if ((*SPM_PWR_STATUS & 0x8) == 0) {
                return;
            }
            hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
            *SPM_RESOURCE_CONTROL_0 &= 0xFEFFFFFF;   //Force off SDIO MTCMOS
            //SDIO_SLV MTCMOS OFF
            *SPM_SDIO_SLV_PWR_CON = 0x1D;
            hal_gpt_delay_us(1);
            *SPM_SDIO_SLV_PWR_CON = 0x1F;
            hal_gpt_delay_us(1);

            *SPM_SDIO_SLV_SRAM_PD |= (1 << 16); /* SDIO_SLV_SRAM_MEM_ISO_EN bit : 16 */
            hal_gpt_delay_us(1);
            *SPM_SDIO_SLV_SRAM_PD |= (1 << 0);  /* SDIO_SLV_SRAM_PD bit : 0 */
            hal_gpt_delay_us(1);

            *SPM_SDIO_SLV_PWR_CON = 0x1E;
            hal_gpt_delay_us(1);
            *SPM_SDIO_SLV_PWR_CON = 0x1A;
            hal_gpt_delay_us(1);
            *SPM_SDIO_SLV_PWR_CON = 0x12;
            hal_gpt_delay_us(1);
            hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
        } else {
            if ((*SPM_PWR_STATUS & 0x8) == 0) {
                if ((*SPM_PWR_STATUS & 0x8) != 0) {
                    return;
                }
                hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
                *SPM_RESOURCE_CONTROL_0 |= 0x01000000;   //Force on SDIO MTCMOS
                //SDIO_SLV MTCMOS ON
                *SPM_SDIO_SLV_PWR_CON = 0x12;
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_PWR_CON = 0x16;
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_PWR_CON = 0x1E;
                hal_gpt_delay_us(1);

                *SPM_SDIO_SLV_SRAM_PD &= (0xFFFEFFFF);   /* SDIO_SLV_SRAM_MEM_ISO_EN bit : 16 */
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_SRAM_PD &= (0xFFFFFFFE);    /* SDIO_SLV_SRAM_PD bit : 0 */
                hal_gpt_delay_us(1);

                *SPM_SDIO_SLV_PWR_CON = 0x0E;
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_PWR_CON = 0x0C;
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_PWR_CON = 0x1C;
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_PWR_CON = 0x1D;
                hal_gpt_delay_us(1);
                *SPM_SDIO_SLV_PWR_CON = 0x0D;
                hal_gpt_delay_us(1);
                hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
            }
        }
        return;
    }
}

void manual_spm_write_im(uint32_t addr, uint32_t data)
{
    *SPM_PCM_IM_HOST_RW_PTR = 0x01010000 | addr;
    *SPM_PCM_IM_HOST_RW_DAT = data;
    return;
}

uint32_t manual_spm_read_im(uint32_t addr)
{
    uint32_t data;
    *SPM_PCM_IM_HOST_RW_PTR = 0x01000000 | addr;
    data = *SPM_PCM_IM_HOST_RW_DAT;
    *SPM_PCM_IM_HOST_RW_PTR = 0;
    return (data);
}

void spm_mask_wakeup_source(uint32_t wakeup_source)
{
    hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
    if (wakeup_source == HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL) {
        *SPM_WAKEUP_SOURCE_MASK = 0xFF;
    } else {
        *SPM_WAKEUP_SOURCE_MASK |= (1 << wakeup_source);
    }
    hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
}

void spm_unmask_wakeup_source(uint32_t wakeup_source)
{
    hal_clock_enable(HAL_CLOCK_CG_SW_SPM);
    if (wakeup_source == HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL) {
        *SPM_WAKEUP_SOURCE_MASK = 0x00;
    } else {
        *SPM_WAKEUP_SOURCE_MASK &= ~(1 << wakeup_source);
    }
    hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
}

#endif
