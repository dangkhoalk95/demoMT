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

/*
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/debug.h#5 $
*/

/*! \file   debug.h
    \brief  Definition of SW debugging level.

    In this file, it describes the definition of various SW debugging levels and
    assert functions.
*/




#ifndef _DEBUG_H
#define _DEBUG_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/
#ifndef BUILD_QA_DBG
#define BUILD_QA_DBG 0
#endif

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "gl_typedef.h"
#include "kal_trace.h"
#if CFG_ENABLE_THROUGHPUT_DBG
#include "cache_sw.h"
#include "init.h"
#include "SST_sla.h"
#endif
extern UINT_8   aucDebugModule[];
extern UINT_32  u4DebugModule;

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* Define debug category (class):
 * (1) ERROR (2) WARN (3) STATE (4) EVENT (5) TRACE (6) INFO (7) LOUD (8) TEMP
 */
#define DBG_CLASS_ERROR         BIT(0)
#define DBG_CLASS_WARN          BIT(1)
#define DBG_CLASS_STATE         BIT(2)
#define DBG_CLASS_EVENT         BIT(3)
#define DBG_CLASS_TRACE         BIT(4)
#define DBG_CLASS_INFO          BIT(5)
#define DBG_CLASS_LOUD          BIT(6)
#define DBG_CLASS_TEMP          BIT(7)
#define DBG_CLASS_MASK          BITS(0,7)

#if defined(WCN_MAUI)
#define DBG_PRINTF_64BIT_DEC    "lld"
extern kal_uint8              wndrv_in_ISR;

#elif defined(LINUX)
#define DBG_PRINTF_64BIT_DEC    "lld"

#else  //Windows
#define DBG_PRINTF_64BIT_DEC    "I64d"

#endif
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* Define debug module index */
typedef enum _ENUM_DBG_MODULE_T {
    DBG_INIT_IDX = 0,       /* For driver initial */
    DBG_HAL_IDX,            /* For HAL(HW) Layer */
    DBG_INTR_IDX,           /* For Interrupt */
    DBG_REQ_IDX,
    DBG_TX_IDX,
    DBG_RX_IDX,
    DBG_RFTEST_IDX,         /* For RF test mode*/
    DBG_EMU_IDX,            /* Developer specific */

    DBG_SW1_IDX,            /* Developer specific */
    DBG_SW2_IDX,            /* Developer specific */
    DBG_SW3_IDX,            /* Developer specific */
    DBG_SW4_IDX,            /* Developer specific */

    DBG_HEM_IDX,            /* HEM */
    DBG_AIS_IDX,            /* AIS */
    DBG_RLM_IDX,            /* RLM */
    DBG_MEM_IDX,            /* RLM */
    DBG_CNM_IDX,            /* CNM */
    DBG_RSN_IDX,            /* RSN */
    DBG_BSS_IDX,            /* BSS */
    DBG_SCN_IDX,            /* SCN */
    DBG_SAA_IDX,            /* SAA */
    DBG_AAA_IDX,            /* AAA */
    DBG_P2P_IDX,            /* P2P */
    DBG_QM_IDX,             /* QUE_MGT */
    DBG_SEC_IDX,            /* SEC */
    DBG_BOW_IDX,            /* BOW */
    DBG_WAPI_IDX,           /* WAPI */
    DBG_ROAMING_IDX,        /* ROAMING */

    DBG_MODULE_NUM
} ENUM_DBG_MODULE_T;

/* Define who owns developer specific index */
#define DBG_YARCO_IDX           DBG_SW1_IDX
#define DBG_KEVIN_IDX           DBG_SW2_IDX
#define DBG_CMC_IDX             DBG_SW3_IDX
#define DBG_GEORGE_IDX          DBG_SW4_IDX

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
/* Debug print format string for the OS system time */
#define OS_SYSTIME_DBG_FORMAT               "0x%08x"

/* Debug print argument for the OS system time */
#define OS_SYSTIME_DBG_ARGUMENT(systime)    (systime)

/* Debug print format string for the MAC Address */
#define MACSTR          "%02x:%02x:%02x:%02x:%02x:%02x"

/* Debug print argument for the MAC Address */
#define MAC2STR(a)      ((PUINT_8)a)[0], ((PUINT_8)a)[1], ((PUINT_8)a)[2], \
                        ((PUINT_8)a)[3], ((PUINT_8)a)[4], ((PUINT_8)a)[5]

/* The pre-defined format to dump the value of a varaible with its name shown. */
#define DUMPVAR(variable, format)           (#variable " = " format "\n", variable)

/* The pre-defined format to dump the MAC type value with its name shown. */
#define DUMPMACADDR(addr)                   (#addr " = " MACSTR "\n", MAC2STR(addr))


/* Basiclly, we just do renaming of KAL functions although they should
 * be defined as "Nothing to do" if DBG=0. But in some compiler, the macro
 * syntax does not support  #define LOG_FUNC(x,...)
 *
 * A caller shall not invoke these three macros when DBG=0.
 */
#define kalPrint(...)       /*WCN Modification*/ kal_wap_trace(MOD_WNDRV, TRACE_GROUP_10, __VA_ARGS__)
#define kalBreakPoint()     /*WCP Modification*/ WNDRV_ASSERT(0) //panic("Oops") /* Or BUG() */

#define LOG_FUNC_TIME           kalPrint
#define LOG_FUNC                kalPrint

//#define DEBUGFUNC(_Func) kal_wap_trace(MOD_WNDRV, TRACE_FUNC, _Func);
#define INITLOG(_Fmt)
#define ERRORLOG(_Fmt)
#define WARNLOG(_Fmt)
#define DBGLOG(_Module, _Class, _Fmt)
#define DBGLOG_MEM8(_Module, _Class, _StartAddr, _Length)
#define DBGLOG_MEM32(_Module, _Class, _StartAddr, _Length)
#define DISP_STRING(_str)       ""
#define DBGPRINTF   wndrvPrintf



#if defined(WCN_MAUI)
#define WNDRV_TRACE0( tst_cls, tst_id, str )                           \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id );                            \
     }                                                                 \
 }

#define WNDRV_TRACE1( tst_cls, tst_id, str, a )                        \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a );                              \
     }                                                                 \
 }

#define WNDRV_TRACE2( tst_cls, tst_id, str, a, b )                     \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b );                           \
     }                                                                 \
 }

#define WNDRV_TRACE3( tst_cls, tst_id, str, a, b, c )                  \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c );                        \
     }                                                                 \
 }

#define WNDRV_TRACE4( tst_cls, tst_id, str, a, b, c, d )               \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c, d );                     \
     }                                                                 \
 }

#define WNDRV_TRACE5( tst_cls, tst_id, str, a, b, c, d, e )            \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c, d, e );                  \
     }                                                                 \
 }

#define WNDRV_TRACE6( tst_cls, tst_id, str, a, b, c, d, e, f )         \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c, d, e, f );               \
     }                                                                 \
 }

#define WNDRV_TRACE7( tst_cls, tst_id, str, a, b, c, d, e, f, g )      \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c, d, e, f, g );            \
     }                                                                 \
 }

#define WNDRV_TRACE8( tst_cls, tst_id, str, a, b, c, d, e, f, g, h )   \
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c, d, e, f, g, h );         \
     }                                                                 \
 }

#define WNDRV_TRACE9( tst_cls, tst_id, str, a, b, c, d, e, f, g, h, i )\
 {                                                                     \
     if (!wndrv_in_ISR)                                                \
     {                                                                 \
         kal_trace( tst_cls, tst_id, a, b, c, d, e, f, g, h, i );      \
     }                                                                 \
 }

#endif

/* The following macro is used for debugging packed structures. */
#define DATA_STRUC_INSPECTING_ASSERT(expr)      switch (0) {case 0: case (expr): default:;}


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#endif /* _DEBUG_H */

