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

#define __WNDRV_OS_FILES_C__


//#include "stack_common.h"
#include "stack_msgs.h"
//#include "app_ltlcom.h"
//#include "task_config.h"
#include "stack_ltlcom.h"

/*----------------------------*/

/*----------------------------*/

/*----------------------------*/
#include "kal_trace.h"
//#include "wndrv_trc.h"

/*----------------------------*/
//#include "wndrv_cid.h"
#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"

#include "wndrv_api.h"
//#include "wndrv_private.h"

/*----------------------------*/
//#include "precomp.h"

/*----------------------------*/
/* include after precomp.h due to MP_ADAPTER */
//#include "wndrv_context.h"
#include "string.h"
//#include "stack_config.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "wndrv_trc.h"
//#include "wndrv_map.h"
//#include "ucos_ii.h"
#include "freertos_ii.h"

extern kal_uint32 SaveAndSetIRQMask(void);
extern void RestoreIRQMask(kal_uint32 irq);
//extern void wifi_assert_fail(char* expr, char* file, kal_uint32 line, kal_uint32 ex1, kal_uint32 ex2, kal_uint32 ex3);
/* -------------------------------------------------------------------------- */
kal_uint8      WNDRV_PTA_isOFF = KAL_TRUE;        /* PTA Default is disable */
kal_uint32     BT_STATE_Report2WLAN = 0;      /* BT State default is disable */
kal_uint32     Protocol_Status4WLAN = 0;      /* Protocol States default is disable */
kal_uint8      WNDRV_isTalkingMode = KAL_FALSE;       /* Default is in idle mode */

WLAN_PS_CTL    wndrv_ps_ctl;
kal_uint8      ps_ctl_mask[8] = {0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};
kal_uint32     wndrv_UAPSD_isEnable = KAL_FALSE;     /* indicate WMM UAPSD test is running */
kal_bool       WNDRV_CHIP_isOK = KAL_FALSE;
/* -------------------------------------------------------------------------- */
//extern wndrv_context_struct wndrv_context;
extern kal_mutexid wifi_mutex;

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _PARAM_POWER_MODE {
    Param_PowerModeCAM,
    Param_PowerModeMAX_PSP,
    Param_PowerModeFast_PSP,
    Param_PowerModeMax,                      /* Upper bound, not real case */
    Param_PowerModeMax_DWMAX = 65536
} PARAM_POWER_MODE, *PPARAM_POWER_MODE;

/*******************************************************************************
*  Macros
*******************************************************************************/

/* -------------------------------------------------------------------------- */
/*******************************************************************************
*
*  LCD status Related Functions
*
*******************************************************************************/
/*******************************************************************************
* FUNCTION
*   wndrv_LCD_isOFF
*
* DESCRIPTION
*   Set LCD status to OFF
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
#if 0
/*******************************************************************************
* FUNCTION
*   wndrv_isTalking
*
* DESCRIPTION
*   Set phone status to talking mode
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_isTalkingMode(void)
{
    //kal_uint32 _savedMask;

    //_savedMask = SaveAndSetIRQMask();
    kal_take_mutex(wifi_mutex);

    WNDRV_isTalkingMode = KAL_TRUE;

    kal_give_mutex(wifi_mutex);
    //RestoreIRQMask(_savedMask);
}

/*******************************************************************************
* FUNCTION
*   wndrv_isIdle
*
* DESCRIPTION
*   Set phone status to idle mode
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_isIdleMode(void)
{
    //kal_uint32 _savedMask;

    //_savedMask = SaveAndSetIRQMask();
    kal_take_mutex(wifi_mutex);

    WNDRV_isTalkingMode = KAL_FALSE;

    kal_give_mutex(wifi_mutex);
    //RestoreIRQMask(_savedMask);
}
/*******************************************************************************
*
*  802.11 power saving mode Related Functions
*
*******************************************************************************/
/*******************************************************************************
* FUNCTION
*   wndrv_PowerSave_Ctrl
*
* DESCRIPTION
*   enable or disable 802.11 power saving mode
*
* CALLS
*   None
*
* PARAMETERS
*   bEnable - TRUE, enable 802.11 power saving mode
*             FASLE, disable 802.11 power saving mode
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_PowerSave_Ctrl(kal_uint8 bCtl)
{
#if 0
    kal_uint32   uPowerMode; //, ulen;

    if (0 == bCtl) {
        uPowerMode = Param_PowerModeCAM;
    } else if (1 == bCtl) {
        uPowerMode = Param_PowerModeMAX_PSP;
    } else {
        uPowerMode = Param_PowerModeFast_PSP;
    }
    wlanpmSetPowerMode(wndrv_context.adpt_p, &uPowerMode, sizeof(PARAM_POWER_MODE), &ulen);
#endif
}


/*******************************************************************************
* FUNCTION
*   wndrv_PS_GetHandle
*
* DESCRIPTION
*   Get the handle for 802.11 power saving mode enable or disable function
*
* CALLS
*   None
*
* PARAMETERS
*   psName - The name of the module
*
* RETURNS
*   Handle counter
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
kal_uint8 wndrv_PS_GetHandle(kal_int8 *psName)
{
    static kal_bool  bStautSet = KAL_FALSE;
    ASSERT(wndrv_ps_ctl.HandleCount < 32);

    if (!bStautSet) {
        /* lcko(NOTE): When bStautSet is FALSE, we meed to make sure
        wndrv_ps_ctl.StautHandle = 32.
        */
        wndrv_ps_ctl.StautHandle = 32;
    }

    if (0 == strcmp((char *)psName, "STAUT")) {
        /* for STAUT, we will let it sleep so we can have a chance to enter PS */
        if (bStautSet) {
            return wndrv_ps_ctl.StautHandle;
        } else {
            bStautSet = KAL_TRUE;
            wndrv_ps_ctl.StautHandle = wndrv_ps_ctl.HandleCount;
        }

    }
//#ifdef WNDRV_PS_DBG
    strcpy((char *) & (wndrv_ps_ctl.name[wndrv_ps_ctl.HandleCount][0]), (char *)psName);
//#endif
    /* Using handle number from 0, not 1 */
    return wndrv_ps_ctl.HandleCount ++;
}

/*******************************************************************************
* FUNCTION
*   wndrv_PS_Enable
*
* DESCRIPTION
*   802.11 power saving mode enable
*
* CALLS
*   None
*
* PARAMETERS
*   handle - The handle of the module
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_PS_Enable(kal_uint8 handle)
{
    //kal_uint32 _savedMask;

    //_savedMask = SaveAndSetIRQMask();
    kal_take_mutex(wifi_mutex);

    wndrv_ps_ctl.Enable |= (1 << handle);

    kal_give_mutex(wifi_mutex);
    //RestoreIRQMask(_savedMask);
}

/*******************************************************************************
* FUNCTION
*   wndrv_PS_Disble
*
* DESCRIPTION
*   802.11 power saving mode disable
*
* CALLS
*   None
*
* PARAMETERS
*   handle - The handle of the module
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_PS_Disable(kal_uint8 handle)
{
    //kal_uint32 _savedMask;

    //_savedMask = SaveAndSetIRQMask();
    kal_take_mutex(wifi_mutex);

    wndrv_ps_ctl.Enable &= ~(1 << handle);

    kal_give_mutex(wifi_mutex);
    //RestoreIRQMask(_savedMask);
}

/*******************************************************************************
* FUNCTION
*   wndrv_set_protocol_status
*
* DESCRIPTION
*   Set protocol status
*
* CALLS
*   None
*
* PARAMETERS
*   kal_uint32 value, kal_uint32 mask
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
void wndrv_set_Protocol_Status(kal_uint32 value, kal_uint32 mask)
{
    //kal_uint32 _savedMask;
    //_savedMask = SaveAndSetIRQMask();
    kal_take_mutex(wifi_mutex);

    Protocol_Status4WLAN &= ~mask;
    Protocol_Status4WLAN |= value;

    kal_give_mutex(wifi_mutex);
    //RestoreIRQMask(_savedMask);
}

/*******************************************************************************
* FUNCTION
*   wndrv_chip_isOK
*
* DESCRIPTION
*   Return the status of WiFi chip
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
kal_bool wndrv_chip_isOK(void)
{
    return WNDRV_CHIP_isOK;
}

/*******************************************************************************
* FUNCTION
*   wndrv_query_chip_capability
*
* DESCRIPTION
*   Return WLAN chip capability bit map
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
kal_uint32 wndrv_query_chip_capability(void)
{
    kal_uint32   wndrv_capability = 0;

#if defined (WIFI_BB_MT5921)
    wndrv_capability |= (WLAN_SUPPORT_11b_MODE | WLAN_SUPPORT_11g_MODE);
#elif defined (MT5931)
    wndrv_capability |= (WLAN_SUPPORT_11b_MODE | WLAN_SUPPORT_11g_MODE | WLAN_SUPPORT_11n_MODE | WLAN_SUPPORT_TX_PWR_0_5_DBM);
#endif

    return wndrv_capability;
}

/*****************************************************************************
* FUNCTION
*  disable_PS_mode
* DESCRIPTION
*  Disable PS mode for RF test
* PARAMETERS
* module_id
* RETURNS
*
*****************************************************************************/
void disable_PS_mode(module_type module_id)
{
    ilm_struct                    *ilm_p;
    wndrv_test_set_req_struct     *req_p;

    MT5931_WAP_TRACE(module_id, TRACE_PEER, "In disable_PS_mode()");

    req_p = (wndrv_test_set_req_struct *)
            construct_local_para(sizeof(wndrv_test_set_req_struct), TD_CTRL);

    req_p->token = 0;
    req_p->type = WNDRV_TEST_SET_PWR_MGT;
    req_p->data.pwr_mgt = WNDRV_POWER_MODE_CONTINUOUS_ACTIVE;

    ilm_p                 = allocate_ilm(module_id);
    ilm_p->msg_id         = MSG_ID_WNDRV_TEST_SET_REQ;
    ilm_p->local_para_ptr = (local_para_struct *) req_p;
    ilm_p->peer_buff_ptr  = NULL;

    SEND_ILM(module_id, MOD_WNDRV, FT_WNDRV_SAP, ilm_p)
}
#endif
