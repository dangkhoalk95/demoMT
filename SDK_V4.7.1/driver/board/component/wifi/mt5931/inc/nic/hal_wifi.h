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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/nic/hal.h#5 $
*/

/*! \file   "hal.h"
    \brief  The declaration of hal functions

    N/A
*/




#ifndef _HAL_H
#define _HAL_H

#include "mt593x_reg.h"
#include "wndrv_kal.h"
/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

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

/* Macros for flag operations for the Adapter structure */
#define HAL_SET_FLAG(_M, _F)             ((_M)->u4HwFlags |= (_F))
#define HAL_CLEAR_FLAG(_M, _F)           ((_M)->u4HwFlags &= ~(_F))
#define HAL_TEST_FLAG(_M, _F)            ((_M)->u4HwFlags & (_F))
#define HAL_TEST_FLAGS(_M, _F)           (((_M)->u4HwFlags & (_F)) == (_F))

#if defined(__WIFI_HIF_SDIO__)
#define HAL_MCR_RD(_prAdapter, _u4Offset, _pu4Value) \
    { \
        if (!fgIsBusAccessFailed) { \
            if (wifi_read_device_register((uint32_t)_u4Offset, (uint32_t *)_pu4Value) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                WNDRV_TRACE(TRACE_ERROR, "HAL_MCR_RD access fail! 0x%x: 0x%x ", _u4Offset, *_pu4Value); \
            } \
        } else { \
             WNDRV_TRACE(TRACE_ERROR, "ignore HAL_MCR_RD access! 0x%x", _u4Offset); \
        } \
    }

#define HAL_MCR_WR(_prAdapter, _u4Offset, _u4Value) \
    { \
        if (!fgIsBusAccessFailed) { \
            if (wifi_write_device_register((uint32_t)_u4Offset, (uint32_t)_u4Value) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                 WNDRV_TRACE(TRACE_ERROR, "HAL_MCR_WR access fail! 0x%x: 0x%x ", _u4Offset, _u4Value); \
            } \
        } else { \
             WNDRV_TRACE(TRACE_ERROR, "ignore HAL_MCR_WR access! 0x%x: 0x%x ", _u4Offset, _u4Value); \
        } \
     }

#define HAL_PORT_RD(_prAdapter, _u4Port, _u4Len, _pucBuf, _u4ValidBufSize) \
    { \
        if (!fgIsBusAccessFailed) { \
            if(_u4Len > 4) \
            { \
                HAL_MCR_WR(_prAdapter, WEHTCR, ALIGN_4(_u4Len)); \
            } \
            if (wifi_read_device_port((uint32_t)_u4Port, (uint16_t)_u4Len, (uint32_t *)_pucBuf) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                 WNDRV_TRACE(TRACE_ERROR, "HAL_PORT_RD access fail! 0x%x", _u4Port); \
            } \
            HAL_MCR_WR(_prAdapter, WEHTCR, 4); \
        } else { \
             WNDRV_TRACE(TRACE_ERROR, "ignore HAL_PORT_RD access! 0x%x", _u4Port); \
        } \
    }

#define HAL_PORT_WR(_prAdapter, _u4Port, _u4Len, _pucBuf, _u4ValidBufSize) \
    { \
        if (!fgIsBusAccessFailed) { \
            if(_u4Len > 4) \
            { \
                HAL_MCR_WR(_prAdapter, WEHTCR, ALIGN_4(_u4Len)); \
            } \
            if (wifi_write_device_port((uint32_t)_u4Port, (uint16_t)_u4Len, (uint8_t *)_pucBuf) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                 WNDRV_TRACE(TRACE_ERROR, "HAL_PORT_WR access fail! 0x%x", _u4Port); \
            } \
            HAL_MCR_WR(_prAdapter, WEHTCR, 4); \
        } else { \
             WNDRV_TRACE(TRACE_ERROR, "ignore HAL_PORT_WR access! 0x%x", _u4Port); \
        } \
    }

#endif /* #if defined(_HIF_SDIO) */


#define HAL_READ_RX_PORT(prAdapter, u4PortId, u4Len, pvBuf, _u4ValidBufSize) \
    { \
    ASSERT(u4PortId < 2); \
    HAL_PORT_RD(prAdapter, \
        ((u4PortId == 0) ? MCR_WRDR0 : MCR_WRDR1), \
        u4Len, \
        pvBuf, \
        _u4ValidBufSize/*temp!!*//*4Kbyte*/) \
    }

#if CFG_SDIO_TX_AGG
#define HAL_WRITE_TX_PORT(_prAdapter, _ucTxPortIdx, _u4Len, _pucBuf, _u4ValidBufSize) \
    { \
    ASSERT(_ucTxPortIdx < 2); \
    if((_u4ValidBufSize - _u4Len) >= sizeof(UINT_32)) { \
        /* fill with single dword of zero as TX-aggregation termination */ \
        *(PUINT_32) (&((_pucBuf)[ALIGN_4(_u4Len)])) = 0;  \
    } \
    HAL_PORT_WR(_prAdapter, \
                (_ucTxPortIdx == 0) ? MCR_WTDR0 : MCR_WTDR1, \
                _u4Len, \
                _pucBuf, \
                _u4ValidBufSize/*temp!!*//*4KByte*/) \
    }
#else
#define HAL_WRITE_TX_PORT(_prAdapter, _ucTxPortIdx, _u4Len, _pucBuf, _u4ValidBufSize) \
    { \
    ASSERT(_ucTxPortIdx < 2); \
    HAL_PORT_WR(_prAdapter, \
                (_ucTxPortIdx == 0) ? MCR_WTDR0 : MCR_WTDR1, \
                _u4Len, \
                _pucBuf, \
                _u4ValidBufSize/*temp!!*//*4KByte*/) \
    }
#endif

/* The macro to read the given MCR several times to check if the wait
   condition come true. */
#define HAL_MCR_RD_AND_WAIT(_pAdapter, _offset, _pReadValue, _waitCondition, _waitDelay, _waitCount, _status) \
        { \
            UINT_32 count; \
            (_status) = FALSE; \
            for (count = 0; count < (_waitCount); count++) { \
                HAL_MCR_RD((_pAdapter), (_offset), (_pReadValue)); \
                if ((_waitCondition)) { \
                    (_status) = TRUE; \
                    break; \
                } \
                kalUdelay((_waitDelay)); \
            } \
        }


/* The macro to write 1 to a R/S bit and read it several times to check if the
   command is done */
#define HAL_MCR_WR_AND_WAIT(_pAdapter, _offset, _writeValue, _busyMask, _waitDelay, _waitCount, _status) \
        { \
            UINT_32 u4Temp; \
            UINT_32 u4Count = _waitCount; \
            (_status) = FALSE; \
            HAL_MCR_WR((_pAdapter), (_offset), (_writeValue)); \
            do { \
                kalUdelay((_waitDelay)); \
                HAL_MCR_RD((_pAdapter), (_offset), &u4Temp); \
                if (!(u4Temp & (_busyMask))) { \
                    (_status) = TRUE; \
                    break; \
                } \
                u4Count--; \
            } while (u4Count); \
        }

#define HAL_GET_CHIP_ID_VER(_prAdapter, pu2ChipId, pu2Version) \
    { \
    UINT_32 u4Value; \
    HAL_MCR_RD(_prAdapter, \
        MCR_WCIR, \
        &u4Value); \
    *pu2ChipId = (UINT_16)(u4Value & WCIR_CHIP_ID); \
    *pu2Version = (UINT_16)(u4Value & WCIR_REVISION_ID) >> 16; \
    }

#define HAL_WAIT_WIFI_FUNC_READY(_prAdapter) \
    { \
    UINT_32 u4Value; \
    UINT_32 i; \
    for (i = 0; i < 100; i++) { \
        HAL_MCR_RD(_prAdapter, \
            MCR_WCIR, \
            &u4Value); \
         if (u4Value & WCIR_WLAN_READY) { \
            break; \
         } \
        NdisMSleep(10); \
    } \
    }

#define HAL_INTR_DISABLE(_prAdapter) \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHLPCR, \
        WHLPCR_INT_EN_CLR)

#define HAL_INTR_ENABLE(_prAdapter) \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHLPCR, \
        WHLPCR_INT_EN_SET)

#define HAL_INTR_ENABLE_AND_LP_OWN_SET(_prAdapter) \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHLPCR, \
        (WHLPCR_INT_EN_SET | WHLPCR_FW_OWN_REQ_SET))

#define HAL_LP_OWN_SET(_prAdapter) \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHLPCR, \
        WHLPCR_FW_OWN_REQ_SET)

#define HAL_LP_OWN_CLR_OK(_prAdapter, _pfgResult) \
    { \
        UINT_32 i; \
        UINT_32 u4RegValue; \
        UINT_32 u4LoopCnt = 2048 / 8; \
        *_pfgResult = TRUE; \
        /* Software get LP ownership */ \
        HAL_MCR_WR(_prAdapter, \
            MCR_WHLPCR, \
            WHLPCR_FW_OWN_REQ_CLR) \
        for (i = 0; i < u4LoopCnt; i++) { \
            HAL_MCR_RD(_prAdapter, MCR_WHLPCR, &u4RegValue); \
            if (u4RegValue & WHLPCR_IS_DRIVER_OWN) { \
                break; \
            } \
            else { \
                kalUdelay(8); \
            } \
        } \
        if (i == u4LoopCnt) { \
            *_pfgResult = FALSE; \
            /*ERRORLOG(("LP cannot be own back (%ld)", u4LoopCnt));*/ \
            /* check the time of LP instructions need to perform from Sleep to On */ \
            /*ASSERT(0); */ \
        } \
    }

#define HAL_GET_ABNORMAL_INTERRUPT_REASON_CODE(_prAdapter, pu4AbnormalReason) \
    { \
    HAL_MCR_RD(_prAdapter, \
        MCR_WASR, \
        pu4AbnormalReason); \
    }


#define HAL_DISABLE_RX_ENHANCE_MODE(_prAdapter) \
    { \
    UINT_32 u4Value; \
    HAL_MCR_RD(_prAdapter, \
        MCR_WHCR, \
        &u4Value); \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHCR, \
        u4Value & ~WHCR_RX_ENHANCE_MODE_EN); \
    }

#define HAL_ENABLE_RX_ENHANCE_MODE(_prAdapter) \
    { \
    UINT_32 u4Value; \
    HAL_MCR_RD(_prAdapter, \
        MCR_WHCR, \
        &u4Value); \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHCR, \
        u4Value | WHCR_RX_ENHANCE_MODE_EN); \
    }

#define HAL_CFG_MAX_HIF_RX_LEN_NUM(_prAdapter, _ucNumOfRxLen) \
    { \
    UINT_32 u4Value, ucNum; \
    ucNum = ((_ucNumOfRxLen >= 16) ? 0 : _ucNumOfRxLen); \
    u4Value = 0; \
    HAL_MCR_RD(_prAdapter, \
        MCR_WHCR, \
        &u4Value); \
    u4Value &= ~WHCR_MAX_HIF_RX_LEN_NUM; \
    u4Value |= ((((UINT_32)ucNum) << 4) & WHCR_MAX_HIF_RX_LEN_NUM); \
    HAL_MCR_WR(_prAdapter, \
        MCR_WHCR, \
        u4Value); \
    }

#define HAL_SET_INTR_STATUS_READ_CLEAR(prAdapter) \
    { \
    UINT_32 u4Value; \
    HAL_MCR_RD(prAdapter, \
        MCR_WHCR, \
        &u4Value); \
    HAL_MCR_WR(prAdapter, \
        MCR_WHCR, \
        u4Value & ~WHCR_W_INT_CLR_CTRL); \
    prAdapter->prGlueInfo->rHifInfo.fgIntReadClear = TRUE;\
    }

#define HAL_SET_INTR_STATUS_WRITE_1_CLEAR(prAdapter) \
    { \
    UINT_32 u4Value; \
    HAL_MCR_RD(prAdapter, \
        MCR_WHCR, \
        &u4Value); \
    HAL_MCR_WR(prAdapter, \
        MCR_WHCR, \
        u4Value | WHCR_W_INT_CLR_CTRL); \
    prAdapter->prGlueInfo->rHifInfo.fgIntReadClear = FALSE;\
    }

/* Note: enhance mode structure may also carried inside the buffer,
         if the length of the buffer is long enough */
#if 1
#define HAL_READ_INTR_STATUS(prAdapter, length, pvBuf) \
    HAL_MCR_RD(prAdapter, \
        MCR_WHISR, \
        pvBuf);
#else
#define HAL_READ_INTR_STATUS(prAdapter, length, pvBuf) \
    HAL_PORT_RD(prAdapter, \
        MCR_WHISR, \
        length, \
        pvBuf, \
        length)
#endif
#define HAL_READ_TX_RELEASED_COUNT(_prAdapter, aucTxReleaseCount) \
    { \
    PUINT_32 pu4Value = (PUINT_32)aucTxReleaseCount; \
    HAL_MCR_RD(_prAdapter, \
        MCR_WTSR0, \
        &pu4Value[0]); \
    HAL_MCR_RD(_prAdapter, \
        MCR_WTSR1, \
        &pu4Value[1]); \
    }

#define HAL_READ_RX_LENGTH(prAdapter, pu2Rx0Len, pu2Rx1Len) \
    { \
    UINT_32 u4Value; \
    u4Value = 0; \
    HAL_MCR_RD(prAdapter, \
        MCR_WRPLR, \
        &u4Value); \
    *pu2Rx0Len = (UINT_16)u4Value; \
    *pu2Rx1Len = (UINT_16)(u4Value >> 16); \
    }

#define HAL_GET_INTR_STATUS_FROM_ENHANCE_MODE_STRUCT(pvBuf, u2Len, pu4Status) \
    { \
    PUINT_32 pu4Buf = (PUINT_32)pvBuf; \
    *pu4Status = pu4Buf[0]; \
    }

#define HAL_GET_TX_STATUS_FROM_ENHANCE_MODE_STRUCT(pvInBuf, pu4BufOut, u4LenBufOut) \
    { \
    PUINT_32 pu4Buf = (PUINT_32)pvInBuf; \
    ASSERT(u4LenBufOut >= 8); \
    pu4BufOut[0] = pu4Buf[1]; \
    pu4BufOut[1] = pu4Buf[2]; \
    }

#define HAL_GET_RX_LENGTH_FROM_ENHANCE_MODE_STRUCT(pvInBuf, pu2Rx0Num, au2Rx0Len, pu2Rx1Num, au2Rx1Len) \
    { \
    PUINT_32 pu4Buf = (PUINT_32)pvInBuf; \
    ASSERT((sizeof(au2Rx0Len) / sizeof(UINT_16)) >= 16); \
    ASSERT((sizeof(au2Rx1Len) / sizeof(UINT_16)) >= 16); \
    *pu2Rx0Num = (UINT_16)pu4Buf[3]; \
    *pu2Rx1Num = (UINT_16)(pu4Buf[3] >> 16); \
    kalMemCopy(au2Rx0Len, &pu4Buf[4], 8); \
    kalMemCopy(au2Rx1Len, &pu4Buf[12], 8); \
    }

#define HAL_GET_MAILBOX_FROM_ENHANCE_MODE_STRUCT(pvInBuf, pu4Mailbox0, pu4Mailbox1) \
    { \
    PUINT_32 pu4Buf = (PUINT_32)pvInBuf; \
    *pu4Mailbox0 = (UINT_16)pu4Buf[21]; \
    *pu4Mailbox1 = (UINT_16)pu4Buf[22]; \
    }

#define HAL_IS_TX_DONE_INTR(u4IntrStatus) \
   ((u4IntrStatus & WHISR_TX_DONE_INT) ? TRUE : FALSE)

#define HAL_IS_RX_DONE_INTR(u4IntrStatus) \
    ((u4IntrStatus & (WHISR_RX0_DONE_INT | WHISR_RX1_DONE_INT)) ? TRUE : FALSE)

#define HAL_IS_ABNORMAL_INTR(u4IntrStatus) \
    ((u4IntrStatus & WHISR_ABNORMAL_INT) ? TRUE : FALSE)

#define HAL_IS_FW_OWNBACK_INTR(u4IntrStatus) \
    ((u4IntrStatus & WHISR_FW_OWN_BACK_INT) ? TRUE : FALSE)

#define HAL_PUT_MAILBOX(prAdapter, u4MboxId, u4Data) \
    { \
    ASSERT(u4MboxId < 2); \
    HAL_MCR_WR(prAdapter, \
        ((u4MboxId == 0) ? MCR_H2DSM0R : MCR_H2DSM1R), \
        u4Data); \
    }

#define HAL_GET_MAILBOX(prAdapter, u4MboxId, pu4Data) \
    { \
    ASSERT(u4MboxId < 2); \
    HAL_MCR_RD(prAdapter, \
        ((u4MboxId == 0) ? MCR_D2HRM0R : MCR_D2HRM1R), \
        pu4Data); \
    }

#define HAL_SET_MAILBOX_READ_CLEAR(prAdapter, fgEnableReadClear) \
    { \
        UINT_32 u4Value; \
        HAL_MCR_RD(prAdapter, MCR_WHCR, &u4Value);\
        HAL_MCR_WR(prAdapter, MCR_WHCR, \
                    (fgEnableReadClear) ? \
                        (u4Value | WHCR_W_MAILBOX_RD_CLR_EN) : \
                        (u4Value & ~WHCR_W_MAILBOX_RD_CLR_EN)); \
        prAdapter->prGlueInfo->rHifInfo.fgMbxReadClear = fgEnableReadClear;\
    }

#define HAL_GET_MAILBOX_READ_CLEAR(prAdapter) (prAdapter->prGlueInfo->rHifInfo.fgMbxReadClear)


/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/



/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _HAL_H */

