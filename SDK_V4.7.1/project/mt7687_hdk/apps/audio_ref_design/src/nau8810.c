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




#include "nau8810.h"
#include "hal_gpt.h"

// Registers default values
static const uint16_t nau8810_reg_default[AUCODEC_REG_NUM] = {
    0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,    /* 0x00 */
    0x000, 0x000, 0x000, 0x0ff, 0x0ff, 0x000, 0x100, 0x0ff,    /* 0x08 */
    0x0ff, 0x000, 0x12c, 0x02c, 0x02c, 0x02c, 0x02c, 0x000,    /* 0x10 */
    0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,    /* 0x18 */
    0x038, 0x00b, 0x032, 0x000, 0x008, 0x00c, 0x093, 0x0e9,    /* 0x20 */
    0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,    /* 0x28 */
    0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,    /* 0x30 */
    0x001, 0x001                                               /* 0x38 */
};

// Registers cache
uint16_t nau8810_reg_cache[AUCODEC_REG_NUM] = {
    0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,    /* 0x00 */
    0x000, 0x000, 0x000, 0x0ff, 0x0ff, 0x000, 0x100, 0x0ff,    /* 0x08 */
    0x0ff, 0x000, 0x12c, 0x02c, 0x02c, 0x02c, 0x02c, 0x000,    /* 0x10 */
    0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,    /* 0x18 */
    0x038, 0x00b, 0x032, 0x000, 0x008, 0x00c, 0x093, 0x0e9,    /* 0x20 */
    0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,    /* 0x28 */
    0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,    /* 0x30 */
    0x001, 0x001                                               /* 0x38 */

};

static hal_i2c_port_t nau8810_i2c_port;

/*****************************************************************************************************************
 * @Function:
 *      aucodec_i2c_write
 *
 * @Routine Description:
 *      Audio codec write register
 *
 * @Parameter:
 *      u16Addr:    Register address
 *      u16Data:    Register data
 *
 * @Return:
 *      AUCODEC_STATUS_OK:    Success
 *       AUCODEC_STATUS_ERROR:    Failure
 *
 *****************************************************************************************************************/
AUCODEC_STATUS_e aucodec_i2c_write(
    uint16_t u16Addr,
    uint16_t u16Data)
{
    uint16_t u16Temp;
    uint8_t u8TxDataBuf[2];
    AUCODEC_STATUS_e codec_status;
    hal_i2c_status_t i2c_status;

    codec_status = AUCODEC_STATUS_OK;
    i2c_status = HAL_I2C_STATUS_OK;

    memset(u8TxDataBuf, 0, 2);

    if (u16Addr < AUCODEC_REG_NUM) {
        nau8810_reg_cache[u16Addr] = u16Data;  // sync registers cache

        //log_hal_info("[nau8810]           write: RegAddress:0x%02x, Data:0x%02x\n", u16Addr, u16Data);

        u16Temp = u16Data & 0x01ff;
        u16Temp = u16Temp >> 8;
        u16Addr = (u16Addr << 1 | u16Temp);
        u8TxDataBuf[0] = (unsigned char)u16Addr;
        u8TxDataBuf[1] = (unsigned char)u16Data;

        i2c_status = hal_i2c_master_send_polling(nau8810_i2c_port, AUCODEC_I2C_ADDR, u8TxDataBuf, 2);
        if (i2c_status == HAL_I2C_STATUS_OK) {
            //log_hal_info("[nau8810]       dubug: aucodec_i2c_write ok: RegAddress:0x%02x, Data:0x%02x\n", (unsigned char)u8TxDataBuf[0], (unsigned char)u8TxDataBuf[1]);
        } else {
            codec_status = AUCODEC_STATUS_ERROR;
            log_hal_error("[nau8810]   aucodec_i2c_write fail: RegAddress=%02x  Data=%u\n", (unsigned char)u8TxDataBuf[0], (unsigned char)u8TxDataBuf[1]);
        }

        //hal_gpt_delay_ms(5);
    } else {
        codec_status = AUCODEC_STATUS_ERROR;
    }

    return codec_status;
}


/*****************************************************************************************************************
 * @Function:
 *      aucodec_i2c_read
 *
 * @Routine Description:
 *      Audio Codec read register
 *
 * @Parameter:
 *      u16Addr:    Register address
 *
 * @Return:
 * @Return:
 *      AUCODEC_STATUS_OK:    Success
 *       AUCODEC_STATUS_ERROR:    Failure
 *
 *****************************************************************************************************************/
uint16_t aucodec_i2c_read(
    uint16_t u16Addr)
{
    uint8_t RegAddress;
    uint8_t u8TxDataBuf[2];
    uint16_t u16Data;
    AUCODEC_STATUS_e codec_status;
    hal_i2c_status_t i2c_status;

    RegAddress = (uint8_t)u16Addr;
    memset(u8TxDataBuf, 0, 2);

    u16Data = 0;
    codec_status = AUCODEC_STATUS_OK;
    i2c_status = HAL_I2C_STATUS_OK;

    RegAddress = RegAddress << 1 | 0x0;

    i2c_status = hal_i2c_master_send_polling(nau8810_i2c_port, AUCODEC_I2C_ADDR, &RegAddress, 1);
    if (i2c_status == HAL_I2C_STATUS_OK) {
    } else {
        codec_status = AUCODEC_STATUS_ERROR;
        log_hal_error("[nau8810] aucodec_i2c_read prepare fail: RegAddress=%02x\n", (unsigned char)u16Addr);
        return codec_status;
    }

    i2c_status = hal_i2c_master_receive_polling(nau8810_i2c_port, AUCODEC_I2C_ADDR, u8TxDataBuf, 2);
    if (i2c_status == HAL_I2C_STATUS_OK) {
        //log_hal_info("[nau8810]       dubug aucodec_i2c_read ok:  RegAddress:0x%02x, Data:0x%02x\n", (unsigned char)u16Addr, *u8TxDataBuf);
        u16Data = (u8TxDataBuf[0] << 8) | u8TxDataBuf[1];
    } else {
        codec_status = AUCODEC_STATUS_ERROR;
        log_hal_error("[nau8810] aucodec_i2c_read fail: RegAddress=%02x\n", (unsigned char)u16Addr);
    }

    return u16Data;

}


/*****************************************************************************************************************
 * @Function:
 *      aucodec_readcache
 *
 * @Routine Description:
 *      Audio Codec read register
 *
 * @Parameter:
 *      u16Addr:    Register address
 *
 * @Return:
 *      Others:    Value of register
 *      -1:    Failure
 *
 *****************************************************************************************************************/
static int aucodec_readcache(
    uint16_t u16Addr)
{
    if (u16Addr < AUCODEC_REG_NUM) {
        return nau8810_reg_cache[u16Addr];
    } else {
        log_hal_info("[nau8810] aucodec_read %x failed", (unsigned short)u16Addr);
        return (-1);
    }
}


/*****************************************************************************************************************
 * @Function:
 *      aucodec_line_out_enable
 *
 * @Routine Description:
 *      DACIN to line out enable
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_line_out_enable(void)
{
    uint16_t u16Data;

#if 1
        u16Data =
            aucodec_readcache(AUCODEC_R0BH_DAC_VOL_REG)&
        (~((unsigned short)AUCODEC_R0BH_DACGAIN_0DB)); 
        
        u16Data |=
            ((unsigned char)220);  // speaker gain setting, Speaker gain= (-127+ pga_gain*0.5)dB
        aucodec_i2c_write(AUCODEC_R0BH_DAC_VOL_REG, u16Data);
#endif

    // AUXOUT source selection
    u16Data =
        aucodec_readcache(AUCODEC_R38H_MOUT_MIXER_REG) |
        ((uint16_t)AUCODEC_R38H_DACMOUT);  // Connected DAC to line out
    aucodec_i2c_write(AUCODEC_R38H_MOUT_MIXER_REG, u16Data);

    // Enable line out mixer
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) |
        ((uint16_t)AUCODEC_R03H_MOUTMXEN) ;  // Enable line out mixer
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);

    // Enable line out driver
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) |
        ((uint16_t)AUCODEC_R03H_MOUTEN) ;  // Enable line out driver
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_line_out_disable
 *
 * @Routine Description:
 *      DACIN to line out disable
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_line_out_disable(void)
{
    uint16_t u16Data;

    // Disable ine out source
    u16Data =
        aucodec_readcache(AUCODEC_R38H_MOUT_MIXER_REG) |
        (~((uint16_t)AUCODEC_R38H_DACMOUT)) ; // Disconnect DAC to ine out
    aucodec_i2c_write(AUCODEC_R38H_MOUT_MIXER_REG, u16Data);

    // Disable ine out driver
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) &
        (~((uint16_t)AUCODEC_R03H_MOUTEN)) ;  // Disable ine out driver
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);

    // Disable ine out mixer
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) &
        (~((uint16_t)AUCODEC_R03H_MOUTMXEN)) ;  // Disable ine out mixer
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);
}


/*****************************************************************************************************************
 * @Function:
 *      aucodec_spk_out_enable
 *
 * @Routine Description:
 *      DACIN to Speaker output enable
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_spk_out_enable(void)
{
    uint16_t u16Data;
#if 0
    u16Data =
        aucodec_readcache(AUCODEC_R36H_SPK_VOL_REG)&
    (~((unsigned short)AUCODEC_R36H_SPKGAIN)); 
    
    u16Data |=
        ((unsigned char)40);  // speaker gain setting, Speaker gain= (-57+ pga_gain*1)dB
    aucodec_i2c_write(AUCODEC_R36H_SPK_VOL_REG, u16Data);
#endif
    // SPEAK mixer source selection
    u16Data =
        aucodec_readcache(AUCODEC_R32H_SPK_MIXER_REG) |
        ((uint16_t)AUCODEC_R32H_DACSPK);  // Connected  DAC to SPEAK mixer
    aucodec_i2c_write(AUCODEC_R32H_SPK_MIXER_REG, u16Data);

    // Enable speaker mixer
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) |
        ((uint16_t)AUCODEC_R03H_SPKMXEN) ;  // Enable  speaker mixer
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);

    // Enable speaker driver
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) |
        ((uint16_t)AUCODEC_R03H_PSPKEN) |  // Enable positive speaker driver
        ((uint16_t)AUCODEC_R03H_NSPKEN);  // Enable negative speaker driver
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_spk_out_disable
 *
 * @Routine Description:
 *      DACIN to Speaker output disable
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_spk_out_disable(void)
{
    uint16_t u16Data;

    // Disconnect SPEAK mixer source
    u16Data =
        aucodec_readcache(AUCODEC_R32H_SPK_MIXER_REG) |
        (~((uint16_t)AUCODEC_R32H_DACSPK)) ;  // Disconnect  DAC to SPEAK mixer
    aucodec_i2c_write(AUCODEC_R32H_SPK_MIXER_REG, u16Data);

    // Disable speaker mixer
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) |
        (~((uint16_t)AUCODEC_R03H_SPKMXEN)) ;   // Disable  speaker mixer
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);

    // Disable speaker driver
    u16Data =
        aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) &
        (~((uint16_t)AUCODEC_R03H_PSPKEN)) &    // Disable positive speaker driver
        (~((uint16_t)AUCODEC_R03H_NSPKEN));    // Disable negative speaker driver
    aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);
}


/*****************************************************************************************************************
 * @Function:
 *      aucodec_mic_in_enable
 *
 * @Routine Description:
 *      Microphone input to ADCOUT enable
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_mic_in_enable(void)
{
    uint16_t u16Data;

    // Enable Micbias
    u16Data =
        aucodec_readcache(AUCODEC_R01H_PWR_MANAGE1_REG) |
        ((uint16_t)AUCODEC_R01H_MICBIASEN);
    aucodec_i2c_write(AUCODEC_R01H_PWR_MANAGE1_REG, u16Data);

    // Enable PGA
    u16Data =
        aucodec_readcache(AUCODEC_R02H_PWR_MANAGE2_REG) |
        ((uint16_t)AUCODEC_R02H_PGAEN) ;  // Enable PGA
    aucodec_i2c_write(AUCODEC_R02H_PWR_MANAGE2_REG, u16Data);

#if 0
    // Enable PGA boost +20dB
    u16Data =
        aucodec_readcache(AUCODEC_R2FH_ADC_BST_REG) |
        ((uint16_t)AUCODEC_R2FH_PGABST);  // Set PGA boost = +20dB
    aucodec_i2c_write(AUCODEC_R2FH_ADC_BST_REG, u16Data);
#endif

    // Enable INPUT PGA path
    u16Data =
        aucodec_readcache(AUCODEC_R2DH_IN_PGA_VOL_REG) &
        (~((uint16_t)AUCODEC_R2DH_PGAGAIN)) &  // Clear PGA gain setting
        (~((uint16_t)AUCODEC_R2DH_PGAMT));  // Unmute PGA path
    u16Data |=
        ((uint16_t)AUCODEC_R2DH_PGAGAIN_35_25DB);  // PGA gain setting
    aucodec_i2c_write(AUCODEC_R2DH_IN_PGA_VOL_REG, u16Data);

    // Enable the path between the MIC and PGA
    u16Data =
        aucodec_readcache(AUCODEC_R2CH_INPUT_CTRL_REG) |
        ((uint16_t)AUCODEC_R2CH_PMICPGA) |  // Connected MICP to PGA
        ((uint16_t)AUCODEC_R2CH_NMICPGA) ;  // Connected MICN to PGA
    aucodec_i2c_write(AUCODEC_R2CH_INPUT_CTRL_REG, u16Data);
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_mic_in_disable
 *
 * @Routine Description:
 *      Microphone input to ADCOUT disable
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_mic_in_disable(void)
{
    uint16_t u16Data;

    // Disable PGA
    u16Data =
        aucodec_readcache(AUCODEC_R02H_PWR_MANAGE2_REG) &
        (~((uint16_t)AUCODEC_R02H_PGAEN)) ; // Enable PGA
    aucodec_i2c_write(AUCODEC_R02H_PWR_MANAGE2_REG, u16Data);

    // Set PGA boost 0dB
    u16Data =
        aucodec_readcache(AUCODEC_R2FH_ADC_BST_REG) &
        (~((uint16_t)AUCODEC_R2FH_PGABST));    // Set PGA boost = 0dB
    aucodec_i2c_write(AUCODEC_R2FH_ADC_BST_REG, u16Data);

    // Disconnected PGA path
    u16Data =
        aucodec_readcache(AUCODEC_R2DH_IN_PGA_VOL_REG) &
        (~((uint16_t)AUCODEC_R2DH_PGAGAIN));  // Clear PGA gain setting
    u16Data |=
        ((uint16_t)AUCODEC_R2DH_PGAGAIN_0_00DB) |  // PGA gain setting
        ((uint16_t)AUCODEC_R2DH_PGAMT);  // Mute left PGA
    aucodec_i2c_write(AUCODEC_R2DH_IN_PGA_VOL_REG, u16Data);

    // Disable the path between the MIC and PGA
    u16Data =
        aucodec_readcache(AUCODEC_R2CH_INPUT_CTRL_REG) &
        (~((uint16_t)AUCODEC_R2CH_PMICPGA)) &  // Disconnect MICP to PGA
        (~((uint16_t)AUCODEC_R2CH_NMICPGA)) ;  // Disconnect MICN to PGA
    aucodec_i2c_write(AUCODEC_R2CH_INPUT_CTRL_REG, u16Data);

    // Disable Micbias
    u16Data =
        aucodec_readcache(AUCODEC_R01H_PWR_MANAGE1_REG) &
        (~((uint16_t)AUCODEC_R01H_MICBIASEN));
    aucodec_i2c_write(AUCODEC_R01H_PWR_MANAGE1_REG, u16Data);
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_set_output
 *
 * @Routine Description:
 *      Audio codec set analog output path for DACIN
 *
 * @Parameter:
 *      AUCODEC_OUTPUT_SEL_e:    eAuxOut
 *                               eHpOut
 *                               eSpkOut
 *                               eNoneOut
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_set_output(
    AUCODEC_OUTPUT_SEL_e eOutputMode)
{
    uint16_t u16Data;

    if ((eOutputMode == eLineOut) || (eOutputMode == eSpkOut)) {
        // Enable DAC
        u16Data =
            aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) |
            ((uint16_t)AUCODEC_R03H_DACEN) ;  // Enable  DAC
        aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);

        if (eOutputMode == eLineOut) {
            aucodec_line_out_enable();
        } else { // if (eOutputMode == eSpkOut)
            aucodec_spk_out_enable();
        }
    } else { // if (eOutputMode == eNoneOut)
        // Disable output driver
        aucodec_line_out_disable();
        aucodec_spk_out_disable();

        // Disable DAC
        u16Data =
            aucodec_readcache(AUCODEC_R03H_PWR_MANAGE3_REG) &
            (~((uint16_t)AUCODEC_R03H_DACEN)) ;  // Disable  DAC
        aucodec_i2c_write(AUCODEC_R03H_PWR_MANAGE3_REG, u16Data);
    }
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_set_input
 *
 * @Routine Description:
 *      Audio codec set analog input path for ADCOUT
 *
 * @Parameter:
 *      AUCODEC_INPUT_SEL_e:    eAuxIn
 *                              eMicIn
 *                              eNoneIn
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_set_input(
    AUCODEC_INPUT_SEL_e eInputMode)
{
    uint16_t u16Data;

    if (eInputMode == eMicIn) {
        // Enable ADC and Mix/Boost Stage
        u16Data =
            aucodec_readcache(AUCODEC_R02H_PWR_MANAGE2_REG) |
            ((uint16_t)AUCODEC_R02H_BSTEN) |  // Enable ADC Mix/Boost Stage
            ((uint16_t)AUCODEC_R02H_ADCEN) ;  // Enable  ADC
        aucodec_i2c_write(AUCODEC_R02H_PWR_MANAGE2_REG, u16Data);

        aucodec_mic_in_enable();

    } else { // if (InputMode == eNoneIn)
        aucodec_mic_in_disable();

        // Disable ADC and Mix/Boost Stage
        u16Data =
            aucodec_readcache(AUCODEC_R02H_PWR_MANAGE2_REG) &
            (~((uint16_t)AUCODEC_R02H_BSTEN)) &  // Disable  ADC Mix/Boost Stage
            (~((uint16_t)AUCODEC_R02H_ADCEN)) ;  // Disable ADC
        aucodec_i2c_write(AUCODEC_R02H_PWR_MANAGE2_REG, u16Data);
    }
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_set_mute
 *
 * @Routine Description:
 *      Audio codec software reset
 *
 * @Parameter:
 *      AUCODEC_MUTE_e:    eMute
 *                         eUnmute
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_set_mute(
    AUCODEC_MUTE_e eMuteEnable)
{
    uint16_t u16Data;

    // DAC soft-mute control
    if (eMuteEnable == eMute) {
        u16Data =
            aucodec_readcache(AUCODEC_R0AH_DAC_CTRL_REG) |
            ((uint16_t)AUCODEC_R0AH_DACMUTE);  // Soft mute DAC
        aucodec_i2c_write(AUCODEC_R0AH_DAC_CTRL_REG, u16Data);
    } else {
        u16Data =
            aucodec_readcache(AUCODEC_R0AH_DAC_CTRL_REG) &
            (~((uint16_t)AUCODEC_R0AH_DACMUTE));  // Soft unmute DAC
        aucodec_i2c_write(AUCODEC_R0AH_DAC_CTRL_REG, u16Data);
    }
}


/*****************************************************************************************************************
 * @Function:
 *      aucodec_set_dai_fmt
 *
 * @Routine Description:
 *      Audio codec digital audio interface format configuration
 *
 * @Parameter:
 *      AUCODEC_DAI_FORMAT_e:    eRightJustified
 *                               eLeftJustified
 *                               eI2S
 *                               ePCMA
 *                               ePCMB
 *      AUCODEC_DAI_WLEN_e:    e16Bit
 *                             e20Bit
 *                             e24Bit
 *                             e32Bit
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_set_dai_fmt(
    AUCODEC_DAI_FORMAT_e eFormat,
    AUCODEC_DAI_WLEN_e eWLEN,
    AUCODEC_BCLK_INV_e eBCLKP)
{
    uint16_t u16Data;

    u16Data =
        aucodec_readcache(AUCODEC_R04H_AU_INTERFACE_CTRL_REG) &
        (~((uint16_t)AUCODEC_R04H_AIFMT)) &
        (~((uint16_t)AUCODEC_R04H_WLEN)) &
        (~((uint16_t)AUCODEC_R04H_FSP)) &
        (~((uint16_t)AUCODEC_R04H_BCLKP));

    // DAI format configuration
    switch (eFormat) {
        case eRightJustified:
            u16Data |= ((uint16_t)AUCODEC_R04H_AIFMT_RJ);
            break;
        case eLeftJustified:
            u16Data |= ((uint16_t)AUCODEC_R04H_AIFMT_LJ);
            break;
        case eI2S:
            u16Data |= ((uint16_t)AUCODEC_R04H_AIFMT_I2S);
            break;
        case ePCMA:
            u16Data |= ((uint16_t)AUCODEC_R04H_AIFMT_PCMA);
            break;
        default: // case ePCMB:
            u16Data |= ((uint16_t)AUCODEC_R04H_AIFMT_PCMA);
            u16Data |= ((uint16_t)AUCODEC_R04H_FSP);
            break;
    }

    // Data resolution of data stream
    switch (eWLEN) {
        case e16Bit:
            u16Data |= ((uint16_t)AUCODEC_R04H_WLEN_16BIT);
            break;
        case e20Bit:
            u16Data |= ((uint16_t)AUCODEC_R04H_WLEN_20BIT);
            break;
        case e24Bit:
            u16Data |= ((uint16_t)AUCODEC_R04H_WLEN_24BIT);
            break;
        default:  // case e32Bit:
            u16Data |= ((uint16_t)AUCODEC_R04H_WLEN_32BIT);
            break;
    }

    switch (eBCLKP) {
        case eBCLK_INV:
            u16Data |= ((uint16_t)AUCODEC_R04H_BCLKP);
            break;
        default: //BCLK Polarity Normal
            u16Data &= (~((uint16_t)AUCODEC_R04H_BCLKP));
            break;
    }
    aucodec_i2c_write(AUCODEC_R04H_AU_INTERFACE_CTRL_REG, u16Data);
}

/*****************************************************************************************************************
* @Function:
*      aucodec_pll_factors
*
* @Routine Description:
*      Audio codec PLL factors calculate
*
* @Parameter:
*
* @Return:
*      0:    Success
*      -1:    Failure
*****************************************************************************************************************/
// The size in bits of the pll divide multiplied by 10 to allow rounding later
#define FIXED_PLL_SIZE ((1 << 24) * 10)
static int32_t aucodec_pll_factors(struct pll_ *pll_div, uint32_t source, uint32_t target)
{
    unsigned long long Kpart;
    uint32_t K, Ndiv, Nmod;

    if ((source > 33000000) || (source < 8000000)) {
        return (-1);
    }
    Ndiv = target / source;
    if (Ndiv < 6) {
        source /= 2;
        pll_div->pre_div = 1;
        Ndiv = target / source;
    } else {
        pll_div->pre_div = 0;
    }

    pll_div->n = Ndiv;
    Nmod = target % source;
    Kpart = FIXED_PLL_SIZE * (long long)Nmod;
    K = (uint32_t)(Kpart / source);

    // Check if we need to round
    if ((K % 10) >= 5) {
        K += 5;
    }
    // Move down to proper range now rounding is done
    K /= 10;
    pll_div->k = K;

    return (0);
}

/*****************************************************************************************************************
* @Function:
*      aucodec_set_pll
*
* @Routine Description:
*      Audio codec PLL setting
*
* @Parameter:
*
* @Return:
*      0:    Success
*      -1:    Failure
*****************************************************************************************************************/
static int32_t aucodec_set_pll(uint32_t source, uint32_t target)
{
    struct pll_ pll_div;
    uint16_t u16Data;
    int32_t status;

    status = aucodec_pll_factors(&pll_div, source, target);
    if (status == -1) {
        return (-1);
    }

    // PLL parameters setting
    aucodec_i2c_write(AUCODEC_R24H_PLLN_REG, (pll_div.pre_div << 4) | pll_div.n);
    aucodec_i2c_write(AUCODEC_R25H_PLLK1_REG, pll_div.k >> 18);
    aucodec_i2c_write(AUCODEC_R26H_PLLK2_REG, (pll_div.k >> 9) & 0x1ff);
    aucodec_i2c_write(AUCODEC_R27H_PLLK3_REG, pll_div.k & 0x1ff);

    u16Data =
        aucodec_readcache(AUCODEC_R01H_PWR_MANAGE1_REG) |
        ((uint16_t)AUCODEC_R01H_PLLEN);
    aucodec_i2c_write(AUCODEC_R01H_PWR_MANAGE1_REG, u16Data);

    return (0);
}

/*****************************************************************************************************************
* @Function:
*      aucodec_set_dai_sysclk
*
* @Routine Description:
*      Audio codec system clock configuration
*
* @Parameter:
*      AUCODEC_SAMPLERATE_SEL_e:    eSR48KHz
*                                   eSR44K1Hz
*                                   eSR32KHz
*                                   eSR16KHz
*                                   eSR8KHz
*      AUCODEC_DAI_ROLE_e:     eSLAVE
*                              eMASTER
*      AUCODEC_BCLK_RATE_e:     e256xFS
*                               e128xFS
*                               e64xFS
*                               e32xFS
*                               e16xFS
*                               e8xFS
*      u32MclkRate:    The frequency of mclk input (Hz)
*      AUCODEC_PLL_e:    ePLLDisable
*                        ePLLEnable
*
* @Return:
*      AUCODEC_STATUS_OK:    Success
*      AUCODEC_STATUS_ERROR:    Failure
*****************************************************************************************************************/
AUCODEC_STATUS_e aucodec_set_dai_sysclk(
    AUCODEC_SAMPLERATE_SEL_e eSampleRate,
    AUCODEC_DAI_ROLE_e eDAIRole,
    AUCODEC_BCLK_RATE_e eBCLKRate,
    uint32_t u32MclkRate,
    AUCODEC_PLL_e ePLLCtrl)
{
    uint16_t u16Data;
    uint8_t  u8Master;
    uint8_t u8MclkDiv;
    uint8_t u8BclkDiv;
    int32_t status;

    AUCODEC_STATUS_e codec_status;
    codec_status = AUCODEC_STATUS_OK;

    // Codec act as dai master or slave
    if (eDAIRole == eMASTER) {
        u8Master = 1;
    } else { // (eDAIRole == eSLAVE)
        u8Master = 0;
    }

    // BCLK rate
    //Only available for master mode, because master nau8814 will generate blck and Frame sync
    if (eBCLKRate == e256xFS) {
        u8BclkDiv = 0;
    } else if (eBCLKRate == e128xFS) {
        u8BclkDiv = 1;
    } else if (eBCLKRate == e64xFS) {
        u8BclkDiv = 2;
    } else if (eBCLKRate == e32xFS) {
        u8BclkDiv = 3;
    } else if (eBCLKRate == e16xFS) {
        u8BclkDiv = 4;
    } else { // if (eBCLKRate == e8xFS)
        u8BclkDiv = 5;
    }

    // Configure system clock
    if (ePLLCtrl == ePLLDisable) {
        if (eSampleRate == eSR48KHz) {
            if (u32MclkRate == 12288000) {
                u8MclkDiv = 0;
            } else if (u32MclkRate == 18432000) {
                u8MclkDiv = 1;
            } else if (u32MclkRate == 24576000) {
                u8MclkDiv = 2;
            } else {
                return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR44K1Hz) {
            if (u32MclkRate == 11289600) {
                u8MclkDiv = 0;
            } else if (u32MclkRate == 16934400) {
                u8MclkDiv = 1;
            } else if (u32MclkRate == 22579200) {
                u8MclkDiv = 2;
            } else {
                return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR32KHz) {
            if (u32MclkRate == 8192000) {
                u8MclkDiv = 0;
            } else if (u32MclkRate == 12288000) {
                u8MclkDiv = 1;
            } else if (u32MclkRate == 16384000) {
                u8MclkDiv = 2;
            } else if (u32MclkRate == 24576000) {
                u8MclkDiv = 3;
            } else {
                return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR16KHz) {
            if (u32MclkRate == 4096000) {
                u8MclkDiv = 0;
            } else if (u32MclkRate == 6144000) {
                u8MclkDiv = 1;
            } else if (u32MclkRate == 8192000) {
                u8MclkDiv = 2;
            } else if (u32MclkRate == 12288000) {
                u8MclkDiv = 3;
            } else if (u32MclkRate == 16384000) {
                u8MclkDiv = 4;
            } else if (u32MclkRate == 24576000) {
                u8MclkDiv = 5;
            } else {
                return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR8KHz) {
            if (u32MclkRate == 2048000) {
                u8MclkDiv = 0;
            } else if (u32MclkRate == 3072000) {
                u8MclkDiv = 1;
            } else if (u32MclkRate == 4096000) {
                u8MclkDiv = 2;
            } else if (u32MclkRate == 6144000) {
                u8MclkDiv = 3;
            } else if (u32MclkRate == 8192000) {
                u8MclkDiv = 4;
            } else if (u32MclkRate == 12288000) {
                u8MclkDiv = 5;
            } else if (u32MclkRate == 24576000) {
                u8MclkDiv = 6;
            } else {
                return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR22_05KHz) {
			if (u32MclkRate == 11289600) {
				u8MclkDiv = 2;
            } else if (u32MclkRate == 16934400) {
				u8MclkDiv = 3;
            } else if (u32MclkRate == 22579200) {
				u8MclkDiv = 4;
            } else {
				return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR11_025KHz)	{
			if (u32MclkRate == 11289600) {
				u8MclkDiv = 4;
            } else if (u32MclkRate == 16934400) {
				u8MclkDiv = 5;
            } else if (u32MclkRate == 22579200) {
				u8MclkDiv = 6;
            } else {
                return (AUCODEC_STATUS_ERROR);
            }
        } else if (eSampleRate == eSR24KHz)	{
			if (u32MclkRate == 12288000) {
				u8MclkDiv = 2;
            } else if (u32MclkRate == 18432000) {
				u8MclkDiv = 3;
            } else if (u32MclkRate == 24576000) {
				u8MclkDiv = 4;
            } else {
				return (AUCODEC_STATUS_ERROR);
            }
		} else {    //sample rate = 12KHz
			if (u32MclkRate == 12288000) {
				u8MclkDiv = 4;
            } else if (u32MclkRate == 18432000) {
				u8MclkDiv = 5;
            } else if (u32MclkRate == 24576000) {
				u8MclkDiv = 6;
            } else {
				return (AUCODEC_STATUS_ERROR);
            }
		}

        u16Data =
            (((uint16_t)u8Master) << 0) |
            (((uint16_t)u8BclkDiv) << 2) |
            (((uint16_t)u8MclkDiv) << 5);
        aucodec_i2c_write(AUCODEC_R06H_CLK_CTRL1_REG, u16Data);
    } else { // if (ePLLCtrl == ePLLEnable)
        if ((eSampleRate == eSR48KHz) || (eSampleRate == eSR32KHz) || (eSampleRate == eSR16KHz) || (eSampleRate == eSR8KHz)|| (eSampleRate == eSR24KHz) || (eSampleRate == eSR12KHz)) {
            status = aucodec_set_pll(u32MclkRate, 98304000);//98304000=48k*256*4*2
            if (status == -1) {
                codec_status = AUCODEC_STATUS_ERROR;
                return codec_status;
            }
        } else {    // if (eSampleRate == eSR44K1Hz)/(eSampleRate == eSR22_05KHz)/(eSampleRate == eSR11_025KHz)
            status = aucodec_set_pll(u32MclkRate, 90316800);
            if (status == -1) {
                codec_status = AUCODEC_STATUS_ERROR;
                return codec_status;
            }

        }

		if ((eSampleRate == eSR48KHz) || (eSampleRate == eSR44K1Hz)) {
			u8MclkDiv = 2;
		} else if (eSampleRate == eSR32KHz) {
			u8MclkDiv = 3;
		} else if ((eSampleRate == eSR24KHz) || (eSampleRate == eSR22_05KHz)) {
			u8MclkDiv = 4;
		} else if (eSampleRate == eSR16KHz) {
			u8MclkDiv = 5;
		} else if ((eSampleRate == eSR12KHz) || (eSampleRate == eSR11_025KHz)) {
			u8MclkDiv = 6;
		} else {    // if (eSampleRate == eSR8KHz)
			u8MclkDiv = 7;
        }

        u16Data =
            ((uint16_t)AUCODEC_R06H_CLKM) |
            (((uint16_t)u8Master) << 0) |
            (((uint16_t)u8BclkDiv) << 2) |
            (((uint16_t)u8MclkDiv) << 5);
        aucodec_i2c_write(AUCODEC_R06H_CLK_CTRL1_REG, u16Data);
    }


    // Sets up scaling for internal filter coefficients
    u16Data =
        aucodec_readcache(AUCODEC_R07H_CLK_CTRL2_REG) &
        (~((uint16_t)AUCODEC_R07H_SMPLR));

    switch (eSampleRate) {
        case eSR48KHz:
        case eSR44K1Hz:
            u16Data |= ((uint16_t)AUCODEC_R07H_SMPLR_48KHZ);
            break;
        case eSR32KHz:
            u16Data |= ((uint16_t)AUCODEC_R07H_SMPLR_32KHZ);
            break;
        case eSR24KHz:
        case eSR22_05KHz:
            u16Data |= ((uint16_t)AUCODEC_R07H_SMPLR_24KHZ);
            break;
        case eSR16KHz:
            u16Data |= ((uint16_t)AUCODEC_R07H_SMPLR_16KHZ);
            break;
        case eSR12KHz:
        case eSR11_025KHz:
            u16Data |= ((uint16_t)AUCODEC_R07H_SMPLR_12KHZ);
            break;
        default:  // case eSR8KHz:
            u16Data |= ((uint16_t)AUCODEC_R07H_SMPLR_8KHZ);
            break;
    }
    aucodec_i2c_write(AUCODEC_R07H_CLK_CTRL2_REG, u16Data);

    return codec_status;
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_softreset
 *
 * @Routine Description:
 *      Audio codec software reset
 *
 * @Parameter:
 *
 * @Return:
 *
 *****************************************************************************************************************/
void aucodec_softreset(void)
{
    uint16_t u16Loop;

    // Software Reset
    aucodec_i2c_write(AUCODEC_R00H_SW_RESET_REG, 0);
    aucodec_i2c_write(AUCODEC_R00H_SW_RESET_REG, 0);
    aucodec_i2c_write(AUCODEC_R00H_SW_RESET_REG, 0);

    // Write default value to the instance
    for (u16Loop = 0; u16Loop < AUCODEC_REG_NUM; u16Loop++) {
        nau8810_reg_cache[u16Loop] = nau8810_reg_default[u16Loop];     // Sync nau8822_reg_cache and nau8822_reg_default
    }
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_init
 *
 * @Routine Description:
 *      Audio codec initial
 *
 * @Parameter:
 *
 * @Return:
 *      AUCODEC_STATUS_OK:    Success
 *      AUCODEC_STATUS_ERROR:    Failure
 *****************************************************************************************************************/
AUCODEC_STATUS_e aucodec_init(void)
{
    AUCODEC_STATUS_e codec_status;
    uint16_t u16Data;
    codec_status = AUCODEC_STATUS_OK;
    u16Data = 0;

    // Set IOBUFEN = 1 and REFIMP = 80kohm
    u16Data =
        aucodec_readcache(AUCODEC_R01H_PWR_MANAGE1_REG) &
        (~((uint16_t)AUCODEC_R01H_REFIMP));
    u16Data |=
        ((uint16_t)AUCODEC_R01H_REFIMP_80K) |
        ((uint16_t)AUCODEC_R01H_IOBUFEN) |
        ((uint16_t)AUCODEC_R01H_ABIASEN);
    aucodec_i2c_write(AUCODEC_R01H_PWR_MANAGE1_REG, u16Data);

    // DAC OSR 128x
    u16Data =
        aucodec_readcache(AUCODEC_R0AH_DAC_CTRL_REG) |
        ((uint16_t)AUCODEC_R0AH_DACOS);
    aucodec_i2c_write(AUCODEC_R0AH_DAC_CTRL_REG, u16Data);

    // ADC OSR 128x and high pass filter enable
    u16Data =
        aucodec_readcache(AUCODEC_R0EH_ADC_CTRL_REG) |
        ((uint16_t)AUCODEC_R0EH_ADCOS) |
        ((uint16_t)AUCODEC_R0EH_HPFEN);
    aucodec_i2c_write(AUCODEC_R0EH_ADC_CTRL_REG, u16Data);

    // AUXOUT and SPKOUT 1.5x boost enable
    // VDDSPK should greater than 1.5 x VDDA
    //u16Data =
    //	aucodec_read(AUCODEC_R31H_OUTPUT_CTRL_REG) |
    //	((uint16_t)AUCODEC_R31H_SPKBST);  // SPKOUT 1.5x enable
    //   ((uint16_t)AUCODEC_R31H_AUX2BST);  // AUXOUT 1.5x enable
    //    ((uint16_t)AUCODEC_R31H_AUX1BST);  // AUXOUT 1.5x enable
    //aucodec_i2c_write(AUCODEC_R31H_OUTPUT_CTRL_REG, u16Data);  // output DC offset to 1.5x(VDDA/2)
    //u16Data =
    //	aucodec_read(AUCODEC_R01H_PWR_MANAGE1_REG) |
    //	((uint16_t)AUCODEC_R01H_DCBUFEN);
    //aucodec_i2c_write(AUCODEC_R01H_PWR_MANAGE1_REG, u16Data);  // output DC offset to 1.5x(VDDA/2)

    return codec_status;
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_i2c_init
 *
 * @Routine Description:
 *      i2c initial
 *
 * @Parameter:
 *                  hal_i2c_port_t i2c_port
 *                              HAL_I2C_MASTER_0 = 0
 *                              HAL_I2C_MASTER_1 = 1
 *                              HAL_I2C_MASTER_2 = 2
 *
 *                  hal_i2c_frequency_t frequency
 *                              HAL_I2C_FREQUENCY_50K  = 0,
 *                              HAL_I2C_FREQUENCY_100K = 1,
 *                              HAL_I2C_FREQUENCY_200K = 2,
 *                              HAL_I2C_FREQUENCY_400K = 3,
 * @Return:
 *      AUCODEC_STATUS_OK:    Success
 *      AUCODEC_STATUS_ERROR:    Failure
 *****************************************************************************************************************/
AUCODEC_STATUS_e aucodec_i2c_init(hal_i2c_port_t i2c_port, hal_i2c_frequency_t frequency)
{
    AUCODEC_STATUS_e codec_status;
    codec_status = AUCODEC_STATUS_OK;

    /*I2C*/
    hal_i2c_config_t i2c_config;
    hal_i2c_status_t i2c_status;
    i2c_status = HAL_I2C_STATUS_OK;
    i2c_config.frequency = (hal_i2c_frequency_t)frequency;
    nau8810_i2c_port = (hal_i2c_port_t)i2c_port;

    i2c_status = hal_i2c_master_init(nau8810_i2c_port, &i2c_config);
    if (i2c_status == HAL_I2C_STATUS_OK) {
        log_hal_info("[nau8810] aucodec_i2c_init ok\n");
    } else {
        log_hal_error("[nau8810] aucodec_i2c_init error\n");
        codec_status = AUCODEC_STATUS_ERROR;
    }

    return codec_status;
}

/*****************************************************************************************************************
 * @Function:
 *      aucodec_i2c_deinit
 *
 * @Routine Description:
 *      i2c deinitial
 *
 * @Parameter:
 *
 * @Return:
 *      AUCODEC_STATUS_OK:    Success
 *      AUCODEC_STATUS_ERROR:    Failure
 *****************************************************************************************************************/

AUCODEC_STATUS_e aucodec_i2c_deinit()
{
    AUCODEC_STATUS_e codec_status;
    hal_i2c_status_t i2c_status;

    codec_status = AUCODEC_STATUS_OK;
    i2c_status = HAL_I2C_STATUS_OK;

    i2c_status = hal_i2c_master_deinit(nau8810_i2c_port);

    if (i2c_status == HAL_I2C_STATUS_OK) {
        log_hal_info("[nau8810] aucodec_i2c_deinit ok\n");
    } else {
        codec_status = AUCODEC_STATUS_ERROR;
        log_hal_error("[nau8810] aucodec_i2c_deinit error\n");

    }

    return codec_status;
}




