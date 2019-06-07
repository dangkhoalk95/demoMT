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

#ifndef _WNDRV_PRIVATE_H
#define _WNDRV_PRIVATE_H

#include "kal_public_api.h"
#include <stdio.h>  //for the following sprintf

//#include "kal_release.h"

#include "stack_ltlcom.h"

//#include "app_ltlcom.h"

/*******************************************************************************
*
*  Enums
*
*******************************************************************************/
typedef enum {
    WNDRV_TIMER_EXPIRY,
    WNDRV_EVSHED_EXPIRY
} wndrv_timer_enums;

/* --- Wndrv_configure.c ----------------------------------------------------------- */
#if 0
void wndrv_cal_tx_ALC_power_2400M(wndrv_cal_tx_ALC_2400M_struct *tx_alc);
kal_bool   wndrv_get_eeprom_existence(void);
kal_uint32 wndrv_get_eeprom_size(void);
void       wndrv_get_nvram_default_mac_addr(wndrv_cal_mac_addr_struct *m);
void       wndrv_get_nvram_default_txpwr_2400M(wndrv_cal_txpwr_2400M_struct *t);
void       wndrv_get_nvram_default_txpwr_5000M(wndrv_cal_txpwr_5000M_struct *t);
void       wndrv_get_nvram_default_dac_dc_offset(wndrv_cal_dac_dc_offset_struct *d);
void       wndrv_get_nvram_default_tx_ALC_pwr_2400M(wndrv_cal_tx_ALC_2400M_struct *t);
void       wndrv_get_nvram_default_ALC_Slope_2400M(wndrv_cal_ALC_Slope_2400M_struct *t);
void       wndrv_get_nvram_default_txpwr_cal_free_flow(wndrv_cal_txpwr_cal_free_flow_struct *t);

kal_uint16 wndrv_get_led_and_phy_configure(void);
kal_uint16 wndrv_get_reg_domain(void);
kal_uint16 wndrv_get_ant_mode(void);
kal_uint16 wndrv_get_osc_stable_time(void);
kal_uint32 wndrv_get_LDO_state(void);
kal_bool wndrv_get_ALC_enable(void);
kal_bool   wndrv_get_tx_power_mark_valid(void);
void       wndrv_advance_led_configure(kal_uint32 *blinking_on_time1,
                                       kal_uint32 *blinking_off_time1,
                                       kal_uint32 *blinking_on_time2,
                                       kal_uint32 *blinking_off_time2);
kal_uint16 wndrv_get_connection_keep_time(void);
void       wndrv_get_RSSI_update_time(kal_uint32 *pRSSIQueryPeriod, kal_uint8 *pCheckLCD);
kal_uint32 wndrv_get_connection_retry_limit(void);
kal_uint8  wndrv_get_lcd_channel(void);
void       wndrv_get_sleep_period(kal_uint32 *uSleep_Time, kal_uint32 *uMax_sleep_Time);

//20081112 add by saker for MT5921 setting
kal_bool wndrv_get_LNA_enable(void);
kal_int8 wndrv_get_abs_temp(void);
kal_uint8 wndrv_get_thermo_val(void);
kal_uint8 wndrv_get_xtal_trim(void);
kal_uint8 wndrv_get_slow_clk_cfg(void);
kal_uint8 wndrv_get_daisy_chain(void);
void wndrv_get_nvram_default_Trim_Thermo_value(wndrv_cal_setting_trim_thermo_struct *t);
#endif

//extern void* wifi_get_ctrl_buffer(kal_uint32 buff_size);
//extern void wifi_free_ctrl_buffer(void * usr_buff);

/*******************************************************************************
*
*  Macros
*
*******************************************************************************/


#define WNDRV_ASSERT ASSERT
#define WNDRV_EXT_ASSERT( expr, str, x, y, z ) EXT_ASSERT(expr, x, y, z )

//#define WNDRV_ALLOC_BUF(byte_size)     wifi_get_ctrl_buffer(byte_size)

//#define WNDRV_FREE_BUF(buf)            wifi_free_ctrl_buffer(buf)
#define WNDRV_ALLOC_BUF(byte_size)     wifi_get_ctrl_buffer(byte_size, (void *)__func__)
#define WNDRV_FREE_BUF(buf)            wifi_free_ctrl_buffer(buf, (void *)__func__)


#define WNDRV_SEND_MSG_TO_SUPC( _ilm_ptr )                            \
{                                                                     \
   if ( !wndrv_context.glue_p->meta_mode )                                    \
   {                                                                  \
      SEND_ILM( MOD_WNDRV, MOD_SUPC, WNDRV_SUPC_SAP, _ilm_ptr );      \
   }                                                                  \
   else                                                               \
   {                                                                  \
      WNDRV_EXT_ASSERT( 0, 0, 0, 0, 0 );                                     \
   }                                                                  \
}
#if 0
#define WNDRV_SEND_MSG_TO_FT( _ilm_ptr )                              \
  {                                                                     \
    if ( wndrv_context.glue_p->meta_mode ) \
    {                                                                 \
      SEND_ILM( MOD_WNDRV, MOD_L1, FT_WNDRV_SAP, _ilm_ptr );\
    }                                                                  \
   else                                                             \
   {                                                                  \
   WNDRV_EXT_ASSERT( 0, 0, 0, 0, 0 );                                     \
   }                                                                  \
}
#endif

#define WNDRV_SEND_MSG_TO_FT( _ilm_ptr )                           \
{                                                                     \
   if ( wndrv_context.glue_p->meta_mode )                             \
   {                                                                  \
        if ( wndrv_context.glue_p->meta_inject_mode )                           \
        {                                                                       \
            SEND_ILM( MOD_WNDRV, MOD_ABM, FT_WNDRV_SAP, _ilm_ptr ); \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SEND_ILM( MOD_WNDRV, MOD_FT, FT_WNDRV_SAP, _ilm_ptr );  \
        }                                                                       \
   }                                                                  \
   else                                                               \
   {                                                                  \
      WNDRV_EXT_ASSERT( 0, 0, 0, 0, 0 );                                  \
   }                                                                  \
}


#define WNDRV_SEND_MSG_TO_TCPIP( _ilm_ptr )                           \
{                                                                     \
   if ( !wndrv_context.glue_p->meta_mode )                                    \
   {                                                                  \
      SEND_ILM( MOD_WNDRV, MOD_TCPIP, WNDRV_TCPIP_SAP, _ilm_ptr );    \
   }                                                                  \
   else                                                               \
   {                                                                  \
      WNDRV_EXT_ASSERT( 0, 0, 0, 0, 0 );                                  \
   }                                                                  \
}

#define WNDRV_SEND_MSG_TO_MMI( _ilm_ptr )                             \
{                                                                     \
   if ( !wndrv_context.glue_p->meta_mode )                                    \
   {                                                                  \
      SEND_ILM(MOD_WNDRV,  MOD_MMI, WNDRV_MMI_SAP, _ilm_ptr );      \
   }                                                                  \
   else                                                               \
   {                                                                  \
      WNDRV_EXT_ASSERT( 0, 0, 0, 0, 0 );                                  \
   }                                                                  \
}

#define WNDRV_SEND_MSG_TO_BT( _ilm_ptr )                              \
{                                                                     \
   if ( !wndrv_context.glue_p->meta_mode )                                    \
   {                                                                  \
      SEND_ILM( MOD_WNDRV, MOD_BT, WNDRV_BT_SAP, _ilm_ptr );          \
   }                                                                  \
   else                                                               \
   {                                                                  \
      WNDRV_EXT_ASSERT( 0, 0, 0, 0, 0 );                                  \
   }                                                                  \
}
#endif /* _WNDRV_PRIVATE_H */
