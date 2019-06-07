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

#ifdef __MTK_TARGET__
#define __WNDRV_OS_FILES_C__

//#include "kal_release.h"

//#include "stack_common.h"
#include "stack_msgs.h"
//#include "app_ltlcom.h"
//#include "syscomp_config.h"
//#include "task_config.h"
//#include "task_main_func.h"
#include "stack_ltlcom.h"

/*----------------------------*/
//#include "stack_timer.h"  /* Stack timer */

/*----------------------------*/
//#include "multiboot_config.h"
//#include "fctycomp_config.h"

/*----------------------------*/
#include "kal_trace.h"

/*----------------------------*/
#include "wndrv_cnst.h"
#include "wndrv_cal.h"
#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"

#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"


#include "wifi_common_config.h"

//#include "nvram_data_items.h"

/*----------------------------*/
#if defined(WIFI_BB_MT5911)
#include "precomp.h"
/* include after precomp.h due to MP_ADAPTER */
#include "wndrv_context.h"
#endif

/*******************************************************************************
*
*  MT5931 driver customer setting
*
*******************************************************************************/

#define DCL_SDIO_FLAGS_SDIO1                0x100
#define WNDRV_SDIO_SEL_CARD1 DCL_SDIO_FLAGS_SDIO1

#if defined (MT5931)
kal_int32 wifi_driver_customer_setting[] = {
    WIFI_DEFAULT_REG_DOMAIN,                                /* 0. Country code */
    PS_FAST_PSP ,                                           /* 1. Power saving mode */
    WIFI_DEFAULT_OSC_STABLE_TIME,                           /* 2. OSC stable time */
    WNDRV_DEFAULT_WMMPS_CONFIG,                             /* 3. WMM PS config */
    WNDRV_EINT_POLARITY,                                    /* 4. WIFI EINT polarity */
    (WNDRV_DEFAULT_LOWEST_RSSI + WNDRV_DEFAULT_PATH_LOSS),  /* 5. RSSI Low threshold */
    WIFI_DEFAULT_CONNECTION_KEEP_TIME,                      /* 6. Connection keep time */
    WIFI_DEFAULT_CONNECTION_RETRY_LIMIT,                    /* 7. Connection retry limit */
    WNDRV_DEFAULT_DTIM_PERIOD,                              /* 8. Sleep Period */
    WNDRV_BT_1WIRE_MODE_T6,                                 /* 9. 1 wire mode T6 */
    WNDRV_BT_1WIRE_MODE_T8,                                 /* 10. 1 wire mode T8 */
    WNDRV_BT_1WIRE_MODE_BT_DELAY,                           /* 11. 1 wire mode BT delay */
    1,                                                      /* 12. HW custom configuration flag */
    3,                                                      /* 13. dec power*/
    20,                                                     /* 14. polling time*/
    WNDRV_SDIO_SEL_CARD1,                                   /* 15. SDIO card select */
    WNDRV_SDIO_CLK_FREQ_22M,                                /* 16. SDIO clock frequency */
    WNDRV_FUNC1_SDIO_BLK_SIZE_512                           /* 17. SDIO WIFI function block size */
};

kal_uint32 WiFi_Query_Customer_Seting_Version(void)
{
    return sizeof(wifi_driver_customer_setting) / sizeof(kal_uint32);
}
#endif /* MT5931 */


/*******************************************************************************
*
*  EEPROM Related Functions
*
*******************************************************************************/
kal_bool wndrv_get_eeprom_existence(void)
{
    return KAL_FALSE;
}


/* ------------------------------------------------------------------------- */
kal_uint32 wndrv_get_eeprom_size(void)
{
    return WIFI_DEFAULT_EEPROM_SIZE;
}


/*******************************************************************************
*
*  NVRAM Related Functions
*
*******************************************************************************/

#endif //#ifdef __MTK_TARGET__

