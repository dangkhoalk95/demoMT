/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
#include "mesh_app_vendor_device_pwm.h"

#define GPIO_PWM_1			19
#define GPIO_PWM_1_MASK		(1<<GPIO_PWM_1)
#define GPIO_PWM_2			20
#define GPIO_PWM_2_MASK		(1<<GPIO_PWM_2)
#define GPIO_PWM_3			21
#define GPIO_PWM_3_MASK		(1<<GPIO_PWM_3)

#define TMR16B0_TICK_1US		(16-1)
#define TMR16B0_TICK_100US		(16*100-1)

#if 0
void pwm_init_123(void)
{
    DRV_PinMux_SigSet( GPIO_PWM_1_MASK, PIN_PWM1 );
    DRV_PinMux_SigSet( GPIO_PWM_2_MASK, PIN_PWM2 );
    DRV_PinMux_SigSet( GPIO_PWM_3_MASK, PIN_PWM3 );
    OS_delay_ms( 10 );
}

void MY_DRV_TMR16_PWM1_Set_Period ( uint16_t mr0, uint16_t mr0_2 )
{
    //Using match control 0
    TMR16B0.MR0.reg = mr0;
    TMR16B0.MR0_2.reg = mr0_2;

    TMR16B0.EMR.field.EMC0 = TMR16_PWM_ON;
    TMR16B0.EMR.field.EMC0_2 = TMR16_PWM_OFF;

    TMR16B0.MCR.field.MR0R_2 = 1;
}

void MY_DRV_TMR16_PWM1_Set_Period_And_Set_Off ( uint16_t mr0, uint16_t mr0_2 )
{
    //Using match control 0
    TMR16B0.MR0.reg = mr0;
    TMR16B0.MR0_2.reg = mr0_2;

    TMR16B0.EMR.field.EMC0 = TMR16_PWM_OFF;
    TMR16B0.EMR.field.EMC0_2 = TMR16_PWM_OFF;

    TMR16B0.MCR.field.MR0R_2 = 1;
}


void MY_DRV_TMR16_PWM2 ( uint16_t mr1, uint16_t mr1_2 )
{
    //Using match control 1
    TMR16B0.MR1.reg = mr1;
    TMR16B0.MR1_2.reg = mr1_2;

    TMR16B0.EMR.field.EMC1 = TMR16_PWM_ON;
    TMR16B0.EMR.field.EMC1_2 = TMR16_PWM_OFF;

    // TMR16B0.MCR.field.MR1R_2 = 1;
}

void MY_DRV_TMR16_PWM2_Set_Off ( uint16_t mr1, uint16_t mr1_2 )
{
    //Using match control 1
    TMR16B0.MR1.reg = mr1;
    TMR16B0.MR1_2.reg = mr1_2;

    TMR16B0.EMR.field.EMC1 = TMR16_PWM_OFF;
    TMR16B0.EMR.field.EMC1_2 = TMR16_PWM_OFF;

    // TMR16B0.MCR.field.MR1R_2 = 1;
}


void MY_DRV_TMR16_PWM3 ( uint16_t mr2, uint16_t mr2_2 )
{
    //Using match control 2
    TMR16B0.MR2.reg = mr2;
    TMR16B0.MR2_2.reg = mr2_2;

    TMR16B0.EMR.field.EMC2 = TMR16_PWM_ON;
    TMR16B0.EMR.field.EMC2_2 = TMR16_PWM_OFF;

    // TMR16B0.MCR.field.MR2R_2 = 1;
}

void MY_DRV_TMR16_PWM3_Set_Off ( uint16_t mr2, uint16_t mr2_2 )
{
    //Using match control 2
    TMR16B0.MR2.reg = mr2;
    TMR16B0.MR2_2.reg = mr2_2;

    TMR16B0.EMR.field.EMC2 = TMR16_PWM_OFF;
    TMR16B0.EMR.field.EMC2_2 = TMR16_PWM_OFF;

    // TMR16B0.MCR.field.MR2R_2 = 1;
}
#else
void pwm_init_123(void)
{
}
void MY_DRV_TMR16_PWM1_Set_Period ( uint16_t mr0, uint16_t mr0_2 )
{
}
void MY_DRV_TMR16_PWM1_Set_Period_And_Set_Off ( uint16_t mr0, uint16_t mr0_2 )
{
}
void MY_DRV_TMR16_PWM2 ( uint16_t mr1, uint16_t mr1_2 )
{
}
void MY_DRV_TMR16_PWM2_Set_Off ( uint16_t mr1, uint16_t mr1_2 )
{
}
void MY_DRV_TMR16_PWM3 ( uint16_t mr2, uint16_t mr2_2 )
{
}
void MY_DRV_TMR16_PWM3_Set_Off ( uint16_t mr2, uint16_t mr2_2 )
{
}
#endif
