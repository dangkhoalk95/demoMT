/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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

#if defined(MTK_USIMSMT_ENABLED) && defined(MTK_BUILD_SMT_LOAD)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "at_command.h"
#include "hal_log.h"

/*--- Function ---*/
static int usimsmt_do_test(int hwInterfaceNo);


/*
AT+EUSIMSMT=<sim_slot_id>  
+EUSIMSMT: <result>           0:OK else:Error
*/


// AT command handler
atci_status_t atci_cmd_hdlr_usimsmt(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int param_val[4] = {0, 0, 0, 0};
    int i = 0;
    
    log_hal_info(">> atci_cmd_hdlr_usimsmt \r\n");

    memset(&resonse, 0, sizeof(atci_response_t));
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    resonse.cmd_id = parse_cmd->cmd_id;
#endif
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)resonse.response_buf, "AT+EUSIMSMT:<sim_slot_id>\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EUSIMSMT=<sim_slot_id>  the handler need to parse the parameters

            param = strtok(parse_cmd->string_ptr, "=,\n\r");
            while (NULL != (param = strtok(NULL, ",\n\r")) && i<4) {
                param_val[i++] = atoi(param);
            }
            
            log_hal_info("atci_cmd_hdlr_sim sim_slot=%d\r\n", param_val[0]);

            if (param_val[0] != 0) {
                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(&resonse);
            } else {
                int result = usimsmt_do_test(0);
                
                /* check if SIM detection is OK */
                log_hal_info("usim_iftest_for_smt(0) return %d", result);

                sprintf((char *)resonse.response_buf, "+EUSIMSMT:%d\r\n", result);

                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }
            break;
        default :
            // others are invalid command format
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    log_hal_info("<< atci_cmd_hdlr_usimsmt \r\n");
    return ATCI_STATUS_OK;
}

#define BASE_MADDR_USIM0         0xA21C0000
#define BASE_MADDR_USIM1         0x00000000      // To make compile pass
#define BASE_ADDR_SENSOR_GDMA    0xA0150000

#define SIM0_base             BASE_MADDR_USIM0
#define SIM1_base             BASE_MADDR_USIM1

/** SIM Registers **/
#define SIM_VERSION_MTK     0x0
#define SIM_CTRL_MTK        0x10
#define SIM_CONF_MTK        0x14
#define SIM_CONFSTA_MTK     0x18
#define SIM_BRR_MTK         0x1C
#define SIM_IRQEN_MTK       0x20
#define SIM_STS_MTK         0x24

#define SIM_RETRY_MTK       0x30
#define SIM_TIDE_MTK        0x34

#define SIM_DATA_MTK        0x40
#define SIM_COUNT_MTK       0x44

#define SIM_ATIME_MTK       0x50
#define SIM_DTIME_MTK       0x54
#define SIM_TOUT_MTK        0x58

#define SIM_GTIME_MTK       0x5C
#define SIM_ETIME_MTK       0x60
#define SIM_EXT_TIME_MTK    0x64
#define SIM_CGTIME_MTK      0x68

#define SIM_COMDCTRL_MTK    0x70
#define SIM_COMDLEN_MTK     0x74
#define SIM_LEFTLEN_MTK     0x78

#define SIM_SW1_MTK         0x7C
#define SIM_SW2_MTK         0x80

#define SIM_ATRSTA_MTK      0x90
#define SIM_STATUS_MTK      0x94
#define SIM_DBG_MTK         0x98

#define SIM_CTRL_SIMON  0x0001
#define SIM_CTRL_IFCLR  0x0020
#define SIM_STS_NATR    0x0020

#define SIM_CONF_RXHSK  0x0001
#define SIM_CONF_TXHSK  0x0002
#define SIM_CONF_CLKPO  0x0004
#define SIM_CONF_CONV   0x0008
#define SIM_CONF_TOUTEN 0x0020
#define SIM_CONF_T1EN   0x0040
#define SIM_CONF_T0EN   0x0080
#define SIM_CONF_HFEN   0x0100
#define SIM_CONFSTA_IFCLR_ON   0x0001


typedef unsigned char kal_uint8;
typedef unsigned short kal_uint16;
typedef unsigned int kal_uint32;
typedef enum
{
    KAL_FALSE,
    KAL_TRUE
} kal_bool;


#define DRV_WriteReg(addr,data)     ((*(volatile kal_uint16 *)(addr)) = (kal_uint16)(data))
#define DRV_Reg(addr)               (*(volatile kal_uint16 *)(addr))
#define DRV_WriteReg32(addr,data)     ((*(volatile kal_uint32 *)(addr)) = (kal_uint32)(data))
#define DRV_Reg32(addr)               (*(volatile kal_uint32 *)(addr))
#define DRV_WriteReg8(addr,data)     ((*(volatile kal_uint8 *)(addr)) = (kal_uint8)(data))
#define DRV_Reg8(addr)               (*(volatile kal_uint8 *)(addr))

#define DRV_SetReg8(addr, data)     (DRV_WriteReg8(addr, DRV_Reg8(addr)|(data)))
#define DRV_ClrReg8(addr, data)     (DRV_WriteReg8(addr, DRV_Reg8(addr)&~(data)))
#define DRV_SetReg(addr, data)      (DRV_WriteReg(addr, DRV_Reg(addr)|(data)))
#define DRV_ClrReg(addr, data)      (DRV_WriteReg(addr, DRV_Reg(addr)&~(data)))
#define DRV_SetReg32(addr, data)    (DRV_WriteReg32(addr, DRV_Reg32(addr)|(data)))
#define DRV_ClrReg32(addr, data)    (DRV_WriteReg32(addr, DRV_Reg32(addr)&~(data)))



#define USIM_3_3V KAL_TRUE

#define USIM_SMT_NO_ERROR       0x0
#define USIM_SMT_NO_TS          0x1
#define USIM_SMT_INVALID_TS     0x2

#include "hal_gpt.h"

// macro
#define USIM_SMT_UPDATE_USIM_IF_FLAG(_if) {                    \
        if((_if) == 0)                                \
        {                                    \
            usim_base = SIM0_base;                        \
            usimsmt_PDNDisable(0);\
        } else if((_if) == 1)                            \
        {                                    \
            usim_base = SIM1_base;                        \
            usimsmt_PDNDisable(1);\
        } else {; }                           \
    } while(0);


#define USIM_SMT_DEACTIVATION(_if)    {        \
        DRV_Reg32(usim_base + SIM_CTRL_MTK) = (DRV_Reg32(usim_base + SIM_CTRL_MTK) & (~SIM_CTRL_SIMON)); \
        while((DRV_Reg32(usim_base + SIM_ATRSTA_MTK) & 0x0001) == 0); \
    } while(0);


#define USIM_SMT_CLR_FIFO() \
    do {\
        DRV_WriteReg32((usim_base + SIM_CTRL_MTK), DRV_Reg32(usim_base + SIM_CTRL_MTK) | SIM_CTRL_IFCLR); \
        while((DRV_Reg32(usim_base + SIM_CONFSTA_MTK) &SIM_CONFSTA_IFCLR_ON) !=0); \
    } while(0)
    

#define usimsmt_printf(fmt...)  log_hal_info(fmt)
#define usimsmt_udelay(_n)      hal_gpt_delay_us(_n)

#define REG_BASE_CKSYS_XO_CLK   0xA2030000
#define REG_XO_PDN_CON0  (REG_BASE_CKSYS_XO_CLK+0x0B00)
#define REG_XO_PDN_SET0  (REG_BASE_CKSYS_XO_CLK+0x0B10)
#define REG_XO_PDN_CLR0  (REG_BASE_CKSYS_XO_CLK+0x0B20)

#define REG_SW_SENSOR_DMA_CG    (0x1<<1)
#define REG_SW_USIM_BCLK_CG     (0x1<<9)

#define REG_BASE_CKSYS_BUS_CLK  0xA21D0000
#define REG_PDN_CON0     (REG_BASE_CKSYS_BUS_CLK+0x300)
#define REG_PDN_SET0     (REG_BASE_CKSYS_BUS_CLK+0x310)
#define REG_PDN_CLR0     (REG_BASE_CKSYS_BUS_CLK+0x320)

#define REG_SW_USIM_26M_CG  (0x1<<10)

static void usimsmt_PDNDisable(kal_uint32 simIf)
{
    switch (simIf)
    {
        case 0:
            // SENSOR_DMA
            DRV_WriteReg32(REG_XO_PDN_CLR0, REG_SW_SENSOR_DMA_CG);
            // USIM
            DRV_WriteReg32(REG_XO_PDN_CLR0, REG_SW_USIM_BCLK_CG);
            DRV_WriteReg32(REG_PDN_CLR0, REG_SW_USIM_26M_CG);
            break;
        default:
            break;
    }
}

#if defined(HAL_PMU_MODULE_ENABLED)
#include <hal_pmu.h>
static void usimsmt_PMU_setVolt(kal_uint32 simIf, int volt)
{
    if (volt == 1800) {
        pmu_set_vsim_voltage(PMU_VSIM_VOL_1P8V);
    } else if (volt == 3000) {
        pmu_set_vsim_voltage(PMU_VSIM_VOL_2P8V);
    } else {
        ;
    }

}

static void usimsmt_PMU_switch(kal_uint32 simIf, kal_bool on)
{
    if (on) {
        pmu_ctrl_power(PMU_VSIM, PMU_CTL_ENABLE);
    } else {
        pmu_ctrl_power(PMU_VSIM, PMU_CTL_DISABLE); 
    }
}
#else
#define usimsmt_PMU_setVolt(...)
#define usimsmt_PMU_switch(...)
#endif

static int usimsmt_do_test(int hwInterfaceNo)
{
    // We just check first byte of ATR here.
    kal_uint32 usim_base;
    kal_uint8 TS = 0;
    int test_result = USIM_SMT_NO_ERROR;

    USIM_SMT_UPDATE_USIM_IF_FLAG(hwInterfaceNo);

    // Deactivation if have activated
    if (DRV_Reg32(usim_base + SIM_CTRL_MTK) & SIM_CTRL_SIMON) {
        USIM_SMT_DEACTIVATION(hwInterfaceNo);
    }

    // Make sure power if turned off
    usimsmt_PMU_switch(hwInterfaceNo, KAL_FALSE);
    usimsmt_udelay(30*1000);
    
    // Power On & Reset

    // 1.1 Set CLK and 1ETU=372CLKs
    DRV_Reg32(usim_base + SIM_BRR_MTK) = (DRV_Reg32(usim_base + SIM_BRR_MTK) & 0x3) | (372 << 2);
    
    // 1.2 Use default configuraton, T0EN/T1EN=0, direct convention, no character reception handshaking
    DRV_Reg32(usim_base + SIM_IRQEN_MTK) = 0;
    
    // 1.3 Disable & Clear All Interrupt
    DRV_Reg32(usim_base + SIM_IRQEN_MTK) = 0;
    DRV_Reg32(usim_base + SIM_STS_MTK) = DRV_Reg32(usim_base + SIM_STS_MTK);

    // 1.4 Clear FIFO
    USIM_SMT_CLR_FIFO();

    // Turn on power
    usimsmt_PMU_setVolt(hwInterfaceNo, 3000);
    usimsmt_PMU_switch(hwInterfaceNo, KAL_TRUE);

    // 1.5 Set SIMON, Activate SIM
    DRV_Reg32(usim_base + SIM_CTRL_MTK) = DRV_Reg32(usim_base + SIM_CTRL_MTK) | SIM_CTRL_SIMON;

    // 1.6 Wait first bytes of ATR
    while (DRV_Reg32(usim_base + SIM_COUNT_MTK) == 0) {
        if (DRV_Reg32(usim_base + SIM_STS_MTK) & SIM_STS_NATR) {
            usimsmt_printf("[SIM_SMT]: NO TS, %x, %d\n\r", usim_base, hwInterfaceNo);
            test_result = USIM_SMT_NO_TS;
            goto end;
        }
    }

    // 1.7 Get TS and check if it's valid
    TS = (kal_uint8)DRV_Reg32(usim_base + SIM_DATA_MTK);

    if (TS != 0x3B && TS != 0x03) {
        // 0x3B means direct convention (L HHLHHHLL H),  0x03 should be inverse convention (L HHLLLLLL H)
        usimsmt_printf("[SIM_SMT]: Invalid TS=%02x, %x, %d\n\r", TS, usim_base, hwInterfaceNo);
        test_result = USIM_SMT_INVALID_TS;
    } else {
        usimsmt_printf("[SIM_SMT]: Got TS=%02x, %x, %d\n\r", TS, usim_base, hwInterfaceNo);
    }

end:
    // Deactivate SIM and power Off
    USIM_SMT_DEACTIVATION(hwInterfaceNo);
    usimsmt_PMU_switch(hwInterfaceNo, KAL_FALSE);
    return test_result;
}

#endif

