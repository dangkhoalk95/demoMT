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

#ifndef _NAU8810_H_
#define _NAU8810_H_

#include "hal_platform.h"
#include "hal_i2c_master.h"
#include "hal_log.h"
#include <stdio.h>
#include <string.h>

#define AUCODEC_I2C_ADDR                               0x1a

#define AUCODEC_REG_NUM                                0x3A

#define AUCODEC_R00H_SW_RESET_REG                      0x00
#define AUCODEC_R01H_PWR_MANAGE1_REG                   0x01
#define AUCODEC_R01H_REFIMP                        (0x03<<0)
#define AUCODEC_R01H_REFIMP_80K                    (0x01<<0)
#define AUCODEC_R01H_REFIMP_300K                   (0x02<<0)
#define AUCODEC_R01H_REFIMP_3K                     (0x03<<0)
#define AUCODEC_R01H_IOBUFEN                       (0x01<<2)
#define AUCODEC_R01H_ABIASEN                       (0x01<<3)
#define AUCODEC_R01H_MICBIASEN                     (0x01<<4)
#define AUCODEC_R01H_PLLEN                         (0x01<<5)
#define AUCODEC_R01H_DCBUFEN                       (0x01<<8)
#define AUCODEC_R02H_PWR_MANAGE2_REG                   0x02
#define AUCODEC_R02H_ADCEN                         (0x01<<0)
#define AUCODEC_R02H_PGAEN                         (0x01<<2)
#define AUCODEC_R02H_BSTEN                         (0x01<<4)
#define AUCODEC_R03H_PWR_MANAGE3_REG                   0x03
#define AUCODEC_R03H_DACEN                         (0x01<<0)
#define AUCODEC_R03H_SPKMXEN                       (0x01<<2)
#define AUCODEC_R03H_MOUTMXEN                      (0x01<<3)
#define AUCODEC_R03H_BIASGEN                       (0x01<<4)
#define AUCODEC_R03H_PSPKEN                        (0x01<<5)
#define AUCODEC_R03H_NSPKEN                        (0x01<<6)
#define AUCODEC_R03H_MOUTEN                        (0x01<<7)
#define AUCODEC_R04H_AU_INTERFACE_CTRL_REG             0x04
#define AUCODEC_R04H_ADCPHS                        (0x01<<1)
#define AUCODEC_R04H_DACPHS                        (0x01<<2)
#define AUCODEC_R04H_AIFMT                         (0x03<<3)
#define AUCODEC_R04H_AIFMT_RJ                      (0x00<<3)
#define AUCODEC_R04H_AIFMT_LJ                      (0x01<<3)
#define AUCODEC_R04H_AIFMT_I2S                     (0x02<<3)
#define AUCODEC_R04H_AIFMT_PCMA                    (0x03<<3)
#define AUCODEC_R04H_WLEN                          (0x03<<5)
#define AUCODEC_R04H_WLEN_16BIT                    (0x00<<5)
#define AUCODEC_R04H_WLEN_20BIT                    (0x01<<5)
#define AUCODEC_R04H_WLEN_24BIT                    (0x02<<5)
#define AUCODEC_R04H_WLEN_32BIT                    (0x03<<5)
#define AUCODEC_R04H_FSP                           (0x01<<7)
#define AUCODEC_R04H_BCLKP                         (0x01<<8)
#define AUCODEC_R05H_COMPANDING_CTRL_REG               0x05
#define AUCODEC_R05H_ADDAP                         (0x01<<0)
#define AUCODEC_R05H_ADCCM                         (0x03<<1)
#define AUCODEC_R05H_ADCCM_OFF                     (0x00<<1)
#define AUCODEC_R05H_ADCCM_ULAW                    (0x02<<1)
#define AUCODEC_R05H_ADCCM_ALAW                    (0x03<<1)
#define AUCODEC_R05H_DACCM                         (0x03<<3)
#define AUCODEC_R05H_DACCM_OFF                     (0x00<<3)
#define AUCODEC_R05H_DACCM_ULAW                    (0x02<<3)
#define AUCODEC_R05H_DACCM_ALAW                    (0x03<<3)
#define AUCODEC_R05H_CMB8                          (0x01<<5)
#define AUCODEC_R06H_CLK_CTRL1_REG                     0x06
#define AUCODEC_R06H_CLKIOEN                       (0x01<<0)
#define AUCODEC_R06H_BCLKSEL                       (0x07<<2)
#define AUCODEC_R06H_BCLKSEL_DIV1                  (0x00<<2)
#define AUCODEC_R06H_BCLKSEL_DIV2                  (0x01<<2)
#define AUCODEC_R06H_BCLKSEL_DIV4                  (0x02<<2)
#define AUCODEC_R06H_BCLKSEL_DIV8                  (0x03<<2)
#define AUCODEC_R06H_BCLKSEL_DIV16                 (0x04<<2)
#define AUCODEC_R06H_BCLKSEL_DIV32                 (0x05<<2)
#define AUCODEC_R06H_MCLKSEL                       (0x07<<5)
#define AUCODEC_R06H_MCLKSEL_DIV1                  (0x00<<5)
#define AUCODEC_R06H_MCLKSEL_DIV1D5                (0x01<<5)
#define AUCODEC_R06H_MCLKSEL_DIV2                  (0x02<<5)
#define AUCODEC_R06H_MCLKSEL_DIV3                  (0x03<<5)
#define AUCODEC_R06H_MCLKSEL_DIV4                  (0x04<<5)
#define AUCODEC_R06H_MCLKSEL_DIV6                  (0x05<<5)
#define AUCODEC_R06H_MCLKSEL_DIV8                  (0x06<<5)
#define AUCODEC_R06H_MCLKSEL_DIV12                 (0x07<<5)
#define AUCODEC_R06H_CLKM                          (0x01<<8)
#define AUCODEC_R07H_CLK_CTRL2_REG                     0x07
#define AUCODEC_R07H_SCLKEN                        (0x01<<0)
#define AUCODEC_R07H_SMPLR                         (0x07<<1)
#define AUCODEC_R07H_SMPLR_48KHZ                   (0x00<<1)
#define AUCODEC_R07H_SMPLR_32KHZ                   (0x01<<1)
#define AUCODEC_R07H_SMPLR_24KHZ                   (0x02<<1)
#define AUCODEC_R07H_SMPLR_16KHZ                   (0x03<<1)
#define AUCODEC_R07H_SMPLR_12KHZ                   (0x04<<1)
#define AUCODEC_R07H_SMPLR_8KHZ                    (0x05<<1)
#define AUCODEC_R0AH_DAC_CTRL_REG                      0x0A
#define AUCODEC_R0AH_DACPL                         (0x01<<0)
#define AUCODEC_R0AH_AUTOMT                        (0x01<<2)
#define AUCODEC_R0AH_DACOS                         (0x01<<3)
#define AUCODEC_R0AH_DEEMP                         (0x03<<4)
#define AUCODEC_R0AH_DEEMP_OFF                     (0x00<<4)
#define AUCODEC_R0AH_DEEMP_32K                     (0x01<<4)
#define AUCODEC_R0AH_DEEMP_44_1K                   (0x02<<4)
#define AUCODEC_R0AH_DEEMP_48K                     (0x03<<4)
#define AUCODEC_R0AH_DACMUTE                       (0x01<<6)//DAC mute
#define AUCODEC_R0BH_DAC_VOL_REG                       0x0B
#define AUCODEC_R0BH_DACGAIN                       (0xFF<<0)
#define AUCODEC_R0BH_DACGAIN_0DB                   (0xFF<<0)
#define AUCODEC_R0EH_ADC_CTRL_REG                      0x0E
#define AUCODEC_R0EH_ADCPL                         (0x01<<0)
#define AUCODEC_R0EH_ADCOS                         (0x01<<3)
#define AUCODEC_R0EH_HPF                           (0x07<<4)
#define AUCODEC_R0EH_HPFAM                         (0x01<<7)
#define AUCODEC_R0EH_HPFEN                         (0x01<<8)
#define AUCODEC_R0FH_ADC_VOL_REG                      0x0F
#define AUCODEC_R0FH_ADCGAIN                       (0xFF<<0)
#define AUCODEC_R0FH_ADCGAIN_0DB                   (0xFF<<0)
#define AuCodec_R12H_EQ1_REG                           0x12
#define AUCODEC_R12H_EQ1GC                         (0x1F<<0)
#define AUCODEC_R12H_EQ1CF                         (0x03<<5)
#define AUCODEC_R12H_EQM                           (0x01<<8)
#define AUCODEC_R13H_EQ2_REG                           0x13
#define AUCODEC_R13H_EQ2GC                         (0x1F<<0)
#define AUCODEC_R13H_EQ2CF                         (0x03<<5)
#define AUCODEC_R13H_EQ2BW                         (0x01<<8)
#define AUCODEC_R14H_EQ3_REG                           0x14
#define AUCODEC_R14H_EQ3GC                         (0x1F<<0)
#define AUCODEC_R14H_EQ3CF                         (0x03<<5)
#define AUCODEC_R14H_EQ3BW                         (0x01<<8)
#define AUCODEC_R15H_EQ4_REG                           0x15
#define AUCODEC_R15H_EQ4GC                         (0x1F<<0)
#define AUCODEC_R15H_EQ4CF                         (0x03<<5)
#define AUCODEC_R15H_EQ4BW                         (0x01<<8)
#define AUCODEC_R16H_EQ5_REG                           0x16
#define AUCODEC_R16H_EQ5GC                         (0x1F<<0)
#define AUCODEC_R16H_EQ5CF                         (0x03<<5)
#define AUCODEC_R24H_PLLN_REG                          0x24
#define AUCODEC_R24H_PLLN                          (0x0F<<0)
#define AUCODEC_R24H_PLLMCLK                       (0x01<<4)
#define AUCODEC_R25H_PLLK1_REG                         0x25
#define AUCODEC_R26H_PLLK2_REG                         0x26
#define AUCODEC_R27H_PLLK3_REG                         0x27
#define AUCODEC_R2CH_INPUT_CTRL_REG                    0x2C
#define AUCODEC_R2CH_PMICPGA                       (0x01<<0)
#define AUCODEC_R2CH_NMICPGA                       (0x01<<1)
#define AUCODEC_R2CH_MICBIASV                      (0x03<<7)
#define AUCODEC_R2DH_IN_PGA_VOL_REG                    0x2D
#define AUCODEC_R2DH_PGAGAIN                       (0x3F<<0)
#define AUCODEC_R2DH_PGAGAIN_35_25DB               (0x3F<<0)
#define AUCODEC_R2DH_PGAGAIN_0_00DB                (0x10<<0)
#define AUCODEC_R2DH_PGAMT                         (0x01<<6)
#define AUCODEC_R2DH_PGAZC                         (0x01<<7)
#define AUCODEC_R2FH_ADC_BST_REG                       0x2F
#define AUCODEC_R2FH_PGABSTGAIN                    (0x07<<4)
#define AUCODEC_R2FH_PGABSTGAIN_DISCONNENT         (0x00<<4)
#define AUCODEC_R2FH_PGABSTGAIN_NEG_12DB           (0x01<<4)
#define AUCODEC_R2FH_PGABSTGAIN_NEG_9DB            (0x02<<4)
#define AUCODEC_R2FH_PGABSTGAIN_NEG_6DB            (0x03<<4)
#define AUCODEC_R2FH_PGABSTGAIN_NEG_3DB            (0x04<<4)
#define AUCODEC_R2FH_PGABSTGAIN_0DB                (0x05<<4)
#define AUCODEC_R2FH_PGABSTGAIN_3DB                (0x06<<4)
#define AUCODEC_R2FH_PGABSTGAIN_6DB                (0x07<<4)
#define AUCODEC_R2FH_PGABST                        (0x01<<8)
#define AUCODEC_R31H_OUTPUT_CTRL_REG                   0x31
#define AUCODEC_R31H_AOUTIMP                       (0x01<<0)
#define AUCODEC_R31H_TSEN                          (0x01<<1)
#define AUCODEC_R31H_SPKBST                        (0x01<<2)
#define AUCODEC_R31H_MOUTBST                       (0x01<<3)
#define AUCODEC_R32H_SPK_MIXER_REG                     0x32
#define AUCODEC_R32H_DACSPK                        (0x01<<0)
#define AUCODEC_R32H_BYPSPK                        (0x01<<1)
#define AUCODEC_R36H_SPK_VOL_REG                       0x36
#define AUCODEC_R36H_SPKGAIN                       (0x3F<<0)
#define AUCODEC_R36H_SPKMUTE                       (0x01<<6)
#define AUCODEC_R36H_SPKZC                         (0x01<<7)
#define AUCODEC_R38H_MOUT_MIXER_REG                    0x38
#define AUCODEC_R38H_DACMOUT                       (0x01<<0)
#define AUCODEC_R38H_BYPMOUT                       (0x01<<1)
#define AUCODEC_R38H_MOUTNUTE                      (0x01<<6)


/**  This enum define the HAL interface return value. */
typedef enum {
    AUCODEC_STATUS_ERROR = -1,   /**<  An error occurred and the transaction failed. */
    AUCODEC_STATUS_OK = 0        /**<  No error occurred during the function call. */
} AUCODEC_STATUS_e;

/**  This enum defines the nau8810 DAI mode. */
typedef enum  {
    eRightJustified = 0,  /**<  Right Justified. */
    eLeftJustified = 1,   /**<  Left Justified  . */
    eI2S = 2,             /**<  I2S . */
    ePCMA = 3,            /**< PCM mode A.*/
    ePCMB = 4,            /**< PCM mode B.*/
} AUCODEC_DAI_FORMAT_e;

/**  This enum defines the nau8810 DAI word length. */
typedef enum  {
    e16Bit = 0,          /**<  16-bit . */
    e20Bit = 1,          /**<  20-bit . */
    e24Bit = 2,          /**<  24-bit . */
    e32Bit = 3,          /**<  32-bit . */
} AUCODEC_DAI_WLEN_e;

/**  This enum defines the nau8810 output path for DAI input. */
typedef enum  {
    eLineOut = 0,       /**<  Line output . */
    eSpkOut = 1,        /**<  Speaker output . */
    eNoneOut = 2,       /**<  No output . */
} AUCODEC_OUTPUT_SEL_e;


/**  This enum defines the nau8810 input path for DAI output. */
typedef enum  {
    eMicIn = 0,         /**<  Microphone input . */
    eNoneIn = 1,        /**<  No input . */
} AUCODEC_INPUT_SEL_e;

/**  This enum defines the nau8810 soft mute. */
typedef enum  {
    eUnmute = 0,  // Soft unmute DAC
    eMute = 1,  // Soft mute DAC
} AUCODEC_MUTE_e;

/**  This enum defines the nau8810 BCLK Polarity. */
typedef enum  {
    eBCLK_NO_INV = 0,   /**<  BCLK Polarity Normal . */
    eBCLK_INV = 1,      /**<  BCLK Polarity Inverted . */
} AUCODEC_BCLK_INV_e;

/**  This enum defines the nau8810 sample rate. */
typedef enum  {
    eSR48KHz = 0,  // Sampling frequency 48KHz
    eSR44K1Hz = 1,  // Sampling frequency 44.1KHz
    eSR32KHz = 2,  // Sampling frequency 32KHz
    eSR16KHz = 3,  // Sampling frequency 16KHz
    eSR8KHz = 4,  // Sampling frequency 8KHz
    eSR22_05KHz = 5, // Sampling frequency 22.05KHz mp3
    eSR11_025KHz = 6, // Sampling frequency 11.025KHz mp3
    eSR24KHz = 7, // Sampling frequency 24KHz mp3
    eSR12KHz = 8, // Sampling frequency 8KHz mp3
} AUCODEC_SAMPLERATE_SEL_e;

/**  This enum defines the nau8810 sample rate. */
typedef enum  {
    eMCLK8KBASE = 24576000,
    eMCLK11_025KBASE = 22579200,
} AUCODEC_MCLKRATE_e;


/**  This enum defines the nau8810 clock mode. */
typedef enum  {
    eSLAVE = 0,   /**<  Digital audio interface slave mode . */
    eMASTER = 1,  /**<  Digital audio interface master mode . */
} AUCODEC_DAI_ROLE_e;

/**  This enum defines the nau8810 BCLK rate. */
typedef enum  {
    e256xFS = 0,  /**<  the frequency of BCLK = 256xFS  . */
    e128xFS = 1,  /**<  the frequency of BCLK = 128xFS  . */
    e64xFS = 2,   /**<  the frequency of BCLK = 64xFS  . */
    e32xFS = 3,   /**<  the frequency of BCLK = 32xFS  . */
    e16xFS = 4,   /**<  the frequency of BCLK = 16xFS  . */
    e8xFS = 5,    /**<  the frequency of BCLK = 8xFS  . */
} AUCODEC_BCLK_RATE_e;

/**  This enum defines the nau8810 PLL mode. */
typedef enum  {
    ePLLDisable = 0,  /**<   PLL enable . */
    ePLLEnable = 1,   /**<   PLL disable . */
} AUCODEC_PLL_e;

struct pll_ {
    uint32_t pre_div : 1;
    uint32_t n : 4;
    uint32_t k;
};

AUCODEC_STATUS_e aucodec_softreset(void);
AUCODEC_STATUS_e aucodec_init(void);
AUCODEC_STATUS_e aucodec_i2c_init(hal_i2c_port_t i2c_port, hal_i2c_frequency_t frequency);
AUCODEC_STATUS_e aucodec_i2c_deinit(void);
AUCODEC_STATUS_e aucodec_set_dai_sysclk(
    AUCODEC_SAMPLERATE_SEL_e eSampleRate,
    AUCODEC_DAI_ROLE_e eDAIRole,
    AUCODEC_BCLK_RATE_e eBCLKRate,
    uint32_t u32MclkRate,
    AUCODEC_PLL_e ePLLCtrl);

AUCODEC_STATUS_e aucodec_set_dai_fmt(AUCODEC_DAI_FORMAT_e eFormat, AUCODEC_DAI_WLEN_e eWLEN, AUCODEC_BCLK_INV_e eBCLKP);
AUCODEC_STATUS_e aucodec_set_mute(AUCODEC_MUTE_e eMuteEnable);
AUCODEC_STATUS_e aucodec_set_input(AUCODEC_INPUT_SEL_e eInputMode);
AUCODEC_STATUS_e aucodec_set_output(AUCODEC_OUTPUT_SEL_e eOutputMode);

/*
//add aux bypass mode
void aucodec_set_aux_bypass(
	AUCODEC_OUTPUT_SEL_e eOutputMode);
*/


#endif /* _NAU8822_H_ */

