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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/common/wlan_lib.c#84 $
*/
/*! \file   wlan_lib.c
    \brief  Internal driver stack will export the required procedures here for GLUE Layer.

    This file contains all routines which are exported from MediaTek 802.11 Wireless
    LAN driver stack to GLUE Layer.
*/



/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"
#include "mgmt/ais_fsm.h"
#include "gl_kal.h"

#include "wndrv_context.h"
#include "wndrv_kal.h"
//#include "wndrv_map.h"
#include "wndrv_supc_msg.h"
#include "wndrv_abm_msg.h"
#include "wmt_struct.h"
#include "wndrv_os_private.h"

#ifdef __WIFI_HIF_SDIO__
//#include "dcl.h"
#endif

/*
extern BOOLEAN kalIsCardRemoved( IN P_GLUE_INFO_T    prGlueInfo );
extern VOID kalSecurityFrameSendComplete (IN P_GLUE_INFO_T prGlueInfo,
    IN PVOID pvPacket, IN WLAN_STATUS rStatus );
extern VOID kalOidCmdClearance(IN P_GLUE_INFO_T prGlueInfo );
extern BOOLEAN kalRetrieveNetworkAddress( IN P_GLUE_INFO_T prGlueInfo,
    IN OUT PARAM_MAC_ADDRESS * prMacAddr );
extern VOID kalEnqueueCommand( IN P_GLUE_INFO_T prGlueInfo, IN P_QUE_ENTRY_T prQueueEntry );
extern ENUM_PARAM_MEDIA_STATE_T kalGetMediaStateIndicated( IN P_GLUE_INFO_T    prGlueInfo );
extern VOID kalSendComplete( IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket, IN INT_32 i4Status );
extern UINT_32 kalGetTxPendingFrameCount( IN P_GLUE_INFO_T    prGlueInfo );
extern VOID kalSetEvent (P_GLUE_INFO_T pr);
extern VOID kalFlushPendingTxPackets( IN P_GLUE_INFO_T    prGlueInfo );
extern kal_uint8 WiFi_GetCoclockMode(kal_uint8 numCoclk);
extern void WiFi_SetupGPIO(char val, unsigned char pin);
extern void LCD_Physical_Interface_Lock(void);
extern void get_MTD_lock(void);
extern void free_MTD_lock(void);
extern void LCD_Physical_Interface_Unlock(void);
*/
extern void wndrv_supc_site_survey_ind_handler(void);
extern kal_bool Wifi_Query_Dynamic_Switch_Cachability(void);
extern unsigned char LB_WIFI_Chip_PowerOn(void);
extern void LB_WIFI_Chip_PowerOff(void);

extern QUE_HEADER  wndrv_int_queue_list[WNDRV_INT_QUEUE_TYPE];
extern kal_uint32  wndrv_int_queue_num;
extern wndrv_context_struct wndrv_context;
extern UINT_8 const *wndrv_firmware_p;
extern UINT_32 wndrv_firmware_size;
extern kal_bool    WNDRVPauseFlag;

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* 6.1.1.2 Interpretation of priority parameter in MAC service primitives */
/* Static convert the Priority Parameter/TID(User Priority/TS Identifier) to Traffic Class */
const UINT_8 aucPriorityParam2TC[] = {
    TC1_INDEX,
    TC0_INDEX,
    TC0_INDEX,
    TC1_INDEX,
    TC2_INDEX,
    TC2_INDEX,
    TC3_INDEX,
    TC3_INDEX
};

#if QM_TEST_MODE
extern QUE_MGT_T g_rQM;
#endif
#ifdef __WIFI_HIF_SDIO__
//todo
typedef signed long DCL_HANDLE;
extern DCL_HANDLE handle_sdio;
#endif

/* Set power on sequence timing */
UINT_32     g_pmu_en_delay = 0;
UINT_32     g_sys_rst_delay = 0;


#define HW_WORKAROUND_PMU_EN_LOCKTIME_US    (1500)
#define HW_WORKAROUND_PMU_EN_SLEEPTIME_MS   (2)

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
extern bool fgIsBusAccessFailed;

extern void wndrv_hisr_handler(void);

extern UINT_8 g_hif_rx_event_port_num;
extern kal_uint32     wndrv_UAPSD_isEnable;

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
// TODO: Check
/* OID set handlers without the need to access HW register */
PFN_OID_HANDLER_FUNC apfnOidSetHandlerWOHwAccess[] = {
    wlanoidSetChannel,
    wlanoidSetBeaconInterval,
    wlanoidSetAtimWindow,
    wlanoidSetFrequency,
};

// TODO: Check
/* OID query handlers without the need to access HW register */
PFN_OID_HANDLER_FUNC apfnOidQueryHandlerWOHwAccess[] = {
    wlanoidQueryBssid,
    wlanoidQuerySsid,
    wlanoidQueryInfrastructureMode,
    wlanoidQueryAuthMode,
    wlanoidQueryEncryptionStatus,
    wlanoidQueryPmkid,
    wlanoidQueryNetworkTypeInUse,
    wlanoidQueryBssidList,
    wlanoidQueryAcpiDevicePowerState,
    wlanoidQuerySupportedRates,
    wlanoidQueryDesiredRates,
    wlanoidQuery802dot11PowerSaveProfile,
    wlanoidQueryBeaconInterval,
    wlanoidQueryAtimWindow,
    wlanoidQueryFrequency,
};

/* OID set handlers allowed in RF test mode */
PFN_OID_HANDLER_FUNC apfnOidSetHandlerAllowedInRFTest[] = {
    wlanoidRftestSetTestMode,
    wlanoidRftestSetAbortTestMode,
    wlanoidRftestSetAutoTest,
    wlanoidSetMcrWrite,
    wlanoidSetEepromWrite
};

/* OID query handlers allowed in RF test mode */
PFN_OID_HANDLER_FUNC apfnOidQueryHandlerAllowedInRFTest[] = {
    wlanoidRftestQueryAutoTest,
    wlanoidQueryMcrRead,
    wlanoidQueryEepromRead
}
;

PFN_OID_HANDLER_FUNC apfnOidWOTimeoutCheck[] = {
    wlanoidRftestSetTestMode,
    wlanoidRftestSetAbortTestMode,
    wlanoidSetAcpiDevicePowerState,
};
#if ((!defined(__WIFI_SLIM__))||defined(__WIFI_HIF_SDIO__))
#if __WIFI_HIF_SDIO__
//#pragma arm section rwdata="DYNAMICCACHEABLERW_C", zidata="DYNAMICCACHEABLEZI_C"
//__align(32) UINT_8 wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
//#pragma arm section
//#else
//#pragma arm section rwdata="DYNAMICCACHEABLERW_C", zidata="DYNAMICCACHEABLEZI_C"
//UINT_8 wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
//#pragma arm section
#endif
#endif

#ifdef __ICCARM__
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN extern unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#else
extern ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
#endif

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
kal_uint8 ucTxPendingFrameNum = 4;


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/*!
* \brief This is a private routine, which is used to check if HW access is needed
*        for the OID query/ set handlers.
*
* \param[IN] pfnOidHandler Pointer to the OID handler.
* \param[IN] fgSetInfo     It is a Set information handler.
*
* \retval TRUE This function needs HW access
* \retval FALSE This function does not need HW access
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanIsHandlerNeedHwAccess(
    IN PFN_OID_HANDLER_FUNC pfnOidHandler,
    IN BOOLEAN              fgSetInfo
)
{
    PFN_OID_HANDLER_FUNC *apfnOidHandlerWOHwAccess;
    UINT_32 i;
    UINT_32 u4NumOfElem;

    if (fgSetInfo) {
        apfnOidHandlerWOHwAccess = apfnOidSetHandlerWOHwAccess;
        u4NumOfElem = sizeof(apfnOidSetHandlerWOHwAccess) / sizeof(PFN_OID_HANDLER_FUNC);
    } else {
        apfnOidHandlerWOHwAccess = apfnOidQueryHandlerWOHwAccess;
        u4NumOfElem = sizeof(apfnOidQueryHandlerWOHwAccess) / sizeof(PFN_OID_HANDLER_FUNC);
    }

    for (i = 0; i < u4NumOfElem; i++) {
        if (apfnOidHandlerWOHwAccess[i] == pfnOidHandler) {
            return FALSE;
        }
    }

    return TRUE;
}   /* wlanIsHandlerNeedHwAccess */


/*----------------------------------------------------------------------------*/
/*!
* \brief This routine is called to set flag for later handling card
*        ejected event.
*
* \param[in] prAdapter Pointer to the Adapter structure.
*
* \return (none)
*
* \note When surprised removal happens, Glue layer should invoke this
*       function to notify WPDD not to do any hw access.
*/
/*----------------------------------------------------------------------------*/
VOID
wlanCardEjected(
    IN P_ADAPTER_T         prAdapter
)
{
    DEBUGFUNC("wlanCardEjected");
    //INITLOG(("\n"));

    ASSERT(prAdapter);

    /* mark that the card is being ejected, NDIS will shut us down soon */
    nicTxRelease(prAdapter);

} /* wlanCardEjected */


/*----------------------------------------------------------------------------*/
/*!
* \brief Create adapter object
*
* \param prAdapter This routine is call to allocate the driver software objects.
*                  If fails, return NULL.
* \retval NULL If it fails, NULL is returned.
* \retval NOT NULL If the adapter was initialized successfully.
*/
/*----------------------------------------------------------------------------*/
/*WCP modification*/ struct _ADAPTER_T wndrv_adapter_pool;
P_ADAPTER_T
wlanAdapterCreate(
    IN P_GLUE_INFO_T prGlueInfo
)
{
    P_ADAPTER_T prAdpater = (P_ADAPTER_T)NULL;

    DEBUGFUNC("wlanAdapterCreate");

    do {
        /* prAdpater = (P_ADAPTER_T) kalMemAlloc(sizeof(ADAPTER_T), VIR_MEM_TYPE); */
        prAdpater = (P_ADAPTER_T)&wndrv_adapter_pool; /*WCP modification*/

        if (!prAdpater) {
            DBGLOG(INIT, ERROR, ("Allocate ADAPTER memory ==> FAILED\n"));
            break;
        }

#if QM_TEST_MODE
        g_rQM.prAdapter = prAdpater;
#endif
        kalMemZero(prAdpater, sizeof(ADAPTER_T));
        prAdpater->prGlueInfo = prGlueInfo;

    } while (FALSE);

    return prAdpater;
} /* wlanAdapterCreate */


/*----------------------------------------------------------------------------*/
/*!
* \brief Destroy adapter object
*
* \param prAdapter This routine is call to destroy the driver software objects.
*                  If fails, return NULL.
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanAdapterDestroy(
    IN P_ADAPTER_T prAdapter
)
{

    if (!prAdapter) {
        return;
    }

    kalMemFree(prAdapter, VIR_MEM_TYPE, sizeof(ADAPTER_T));

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Initialize the adapter. The sequence is
*        1. Disable interrupt
*        2. Read adapter configuration from EEPROM and registry, verify chip ID.
*        3. Create NIC Tx/Rx resource.
*        4. Initialize the chip
*        5. Initialize the protocol
*        6. Enable Interrupt
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \retval WLAN_STATUS_SUCCESS: Success
* \retval WLAN_STATUS_FAILURE: Failed
*/
/*----------------------------------------------------------------------------*/
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code = "SECONDARY_ROCODE"
#endif

WLAN_STATUS
wlanAdapterStart(
    IN P_ADAPTER_T  prAdapter,
    IN P_REG_INFO_T prRegInfo,
    IN PVOID        pvFwImageMapFile,
    IN UINT_32      u4FwImageFileLength
)
{
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
    kal_uint32     i, u4Value = 0;
    UINT_32     u4WHISR = 0;
    UINT_8      aucTxCount[8];
#if CFG_ENABLE_FW_DOWNLOAD
    UINT_32     u4FwLoadAddr, u4ImgSecSize;
#endif

    PARAM_PTA_IPC_T rBwcsPta;
    UINT_32 u4SetInfoLen;

    UINT_32     reg_value = 0;
    UINT_8      aucCountryCode[2];
    PARAM_POWER_MODE pm = Param_PowerModeMAX_PSP;
#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    static      BOOL sys_on = FALSE;
#endif

    /* Init power on sequence timing */
    g_pmu_en_delay = 3;    // ms
    g_sys_rst_delay = 12;   // ms

    ASSERT(prAdapter);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "wlanAdapterStart");

    MT5931_TRACE(TRACE_INIT, MT5931_INFO_213, "===== MT5931 E%d =====", (prAdapter->ucRevID + 1));

    kal_task_enter_critical();

    // DCXO co-clock configuration
    if (wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DCXO_CONFIG] & WIFI_SUPPORT_DCXO_CO_CLOCK) {
        //WiFi_co_clock_config(1);
    }

    //4 <0> Reset variable in ADAPTER_T
    prAdapter->fgIsFwOwn = TRUE;
    prAdapter->u4RxDataResvTime = 0;
    prAdapter->fgIsEnterD3ReqIssued = FALSE;
    QUEUE_INITIALIZE(&(prAdapter->rPendingCmdQueue));

    /* Initialize GLUE INFO */
    for (i = 0; i < PARAM_WNDRV_ES_NUM; i ++) {
        prAdapter->prGlueInfo->event_id[i] = (eventid) 0;
    }

    prAdapter->prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
    prAdapter->prGlueInfo->ePowerState = ParamDeviceStateD0;
    prAdapter->prGlueInfo->u4ReadyFlag = 0;
    HAL_CLEAR_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR);
    fgIsBusAccessFailed = FALSE;

    QUEUE_INITIALIZE(&prAdapter->prGlueInfo->rCmdQueue);
    QUEUE_INITIALIZE(&prAdapter->prGlueInfo->rTxQueue);

    /* Initialize rWlanInfo */
    kalMemSet(&(prAdapter->rWlanInfo), 0, sizeof(WLAN_INFO_T));

    do {
        if ((u4Status = nicAllocateAdapterMemory(prAdapter)) != WLAN_STATUS_SUCCESS) {
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "nicAllocateAdapterMemory Error!\n");
            u4Status = WLAN_STATUS_FAILURE;
            ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
            break;
        }

        prAdapter->u4OsPacketFilter = PARAM_PACKET_FILTER_SUPPORTED;

        //4 <2> Initialize System Service (MGMT Memory pool and STA_REC)
        nicInitSystemService(prAdapter);

        //4 <3> Initialize Tx
        nicTxInitialize(prAdapter);
        wlanDefTxPowerCfg(prAdapter);

        //4 <4> Initialize Rx
        nicRxInitialize(prAdapter);


#if defined(MT5931) && defined(WCN_MAUI)

        //Power on MT5931
        LB_WIFI_Chip_PowerOn();

        // SDIO init
        glSdioOpen();
        if(!glSdioInit()) {
            goto fail2;
        }

        /* Get MT5931 RAM code revision */
        wndrv_firmware_init();
        pvFwImageMapFile = (PVOID)wndrv_firmware_p;
        u4FwImageFileLength = wndrv_firmware_size;

        MT5931_NEW_TRACE0(MT5931_INFO_430,  TRACE_INIT, "[SDIO test]1. Chip ID/Revision ID");
        HAL_MCR_RD(prAdapter, WCIR, &reg_value);
        MT5931_NEW_TRACE(MT5931_INFO_431,  TRACE_INIT, "[SDIO test]Chip ID/Revision ID = %x", reg_value);

        if ((reg_value & 0xffff) != 0x5931) {
            if (prAdapter->prGlueInfo->meta_mode) {  /* META mode */
                goto fail1;
            } else { /* normal mode */
                WNDRV_ASSERT((reg_value & 0xffff) == 0x5931);
            }
        }

        /* register EINT */
        WiFi_EintRegister((void *)wndrv_hisr_handler);

        /* because register eint handler unmask eint, need mask again to keep atomic */
        WiFi_EintMask();
#endif

#if CFG_ENABLE_FW_DOWNLOAD
#if defined(MT5931)
        if (pvFwImageMapFile) {
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "Download Address: 0x%x\n", prRegInfo->u4LoadAddress);
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "Firmware Length:  0x%x\n", u4FwImageFileLength);

            do {

                /* 1.1 wait for INIT_RDY */
                i = 0;
                while (1) {
                    HAL_MCR_RD(prAdapter, MCR_WMCSR, &u4Value);

                    if (u4Value & WMCSR_INI_RDY) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "INIT-RDY detected\n");
                        break;
                    } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                               || fgIsBusAccessFailed == TRUE) {
                        u4Status = WLAN_STATUS_FAILURE;
                        ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                        break;
                    } else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Waiting for Init Ready bit: Timeout\n");
                        u4Status = WLAN_STATUS_FAILURE;
                        goto fail2;
                        ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                        break;
                    } else {
                        i++;
                        kalMsleep(10);
                    }
                }

                /* Try to get FW from SD memory */
                /* 1.2 set KSEL/FLEN */
                HAL_MCR_WR(prAdapter, MCR_FWCFG, u4FwImageFileLength >> 6);
                /* 1.3 enable FWDL_EN */
                HAL_MCR_WR(prAdapter, MCR_WMCSR, WMCSR_FWDLEN);

                /* 1.4 wait for PLL_RDY */
                i = 0;
                while (1) {
                    HAL_MCR_RD(prAdapter, MCR_WMCSR, &u4Value);

                    if (u4Value & WMCSR_PLLRDY) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "PLL-RDY detected\n");
                        break;
                    } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                               || fgIsBusAccessFailed == TRUE) {
                        u4Status = WLAN_STATUS_FAILURE;
                        ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                        break;
                    } else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
                        //DBGLOG(INIT, ERROR, ("Waiting for PLL Ready bit: Timeout\n"));
                        MT5931_TRACE0(TRACE_ERROR, MT5931_INFO_215, "Waiting for PLL Ready bit: Timeout");
                        //MT5931_TRACE(TRACE_WARNING, MT5931_INFO_216, "*(MT6255_CLKSQ_CON1)=%x", *((volatile unsigned short*)(0x801A0004)));
                        //MT5931_TRACE(TRACE_WARNING, MT5931_INFO_217, "gpio_wifi_coclock_pin reg=%x", *((volatile unsigned short*)(0x801D0610)));
                        u4Status = WLAN_STATUS_FAILURE;
                        goto fail2;
                        ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                        break;
                    } else {
                        i++;
                        kalMsleep(10);
                    }
                }

                /* 2.1 turn on HIFSYS firmware download mode */
                HAL_MCR_WR(prAdapter, MCR_FWDLSR, FWDLSR_FWDL_MODE);

                /* 2.2 set starting address */
                u4FwLoadAddr = prRegInfo->u4LoadAddress;
                HAL_MCR_WR(prAdapter, MCR_FWDLDSAR, u4FwLoadAddr);

                /* 3. upload firmware */
                /* FW get from binary array */
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "Firmware get from binary array\n");
                for (i = 0; i < u4FwImageFileLength ; i += CMD_PKT_SIZE_FOR_IMAGE) {
                    if (i + CMD_PKT_SIZE_FOR_IMAGE < u4FwImageFileLength) {
                        u4ImgSecSize = CMD_PKT_SIZE_FOR_IMAGE;
                    } else {
                        u4ImgSecSize = u4FwImageFileLength - i;
                    }

                    if (wlanImageSectionDownload(prAdapter,
                                                 u4FwLoadAddr + i,
                                                 u4ImgSecSize,
                                                 (PUINT_8)pvFwImageMapFile + i) != WLAN_STATUS_SUCCESS) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Firmware scatter download failed!\n");
                        u4Status = WLAN_STATUS_FAILURE;
                        goto fail2;
                        ASSERT(0); //TODO(Nelson): error handling for this
                        break;
                    }
                }
                if (u4Status != WLAN_STATUS_SUCCESS) {
                    break;
                }

                /* 4.1 poll FWDL_OK & FWDL_FAIL bits */
                i = 0;
                while (1) {
                    HAL_MCR_RD(prAdapter, MCR_WMCSR, &u4Value);

                    if (u4Value & WMCSR_DL_OK) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "DL_OK detected\n");
                        break;
                    } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                               || fgIsBusAccessFailed == TRUE
                               || (u4Value & WMCSR_DL_FAIL)) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "DL_FAIL detected: 0x%08X\n", u4Value);
                        u4Status = WLAN_STATUS_FAILURE;
                        goto fail2;
                        ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                        break;
                    } else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
                        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Waiting for DL_OK/DL_FAIL bit: Timeout\n");
                        u4Status = WLAN_STATUS_FAILURE;
                        goto fail2;
                        ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                        break;
                    } else {
                        i++;
                        kalMsleep(10);
                    }
                }

                /* 4.2 turn off HIFSYS download mode */
                HAL_MCR_WR(prAdapter, MCR_FWDLSR, 0);

            } while (FALSE);

            /* 5. disable interrupt */
            nicDisableInterrupt(prAdapter);
        } else {
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "No Firmware found!\n");
            u4Status = WLAN_STATUS_FAILURE;
            break;
        }
#endif
#endif

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "wlanAdapterStart(): Waiting for Ready bit..\n");
        //4 <5> check Wi-Fi FW asserts ready bit
        i = 0;
        while (1) {
            HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

            if (u4Value & WCIR_WLAN_READY) {
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "Ready bit asserted\n");
                break;
            } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                       || fgIsBusAccessFailed == TRUE) {
                u4Status = WLAN_STATUS_FAILURE;
                ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                break;
            } else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
                UINT_32     u4MailBox0;

                nicGetMailbox(prAdapter, 0, &u4MailBox0);
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Waiting for Ready bit: Timeout, ID=%d\n",
                                 (u4MailBox0 & 0x0000FFFF));
                u4Status = WLAN_STATUS_FAILURE;
                goto fail2;
                ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                break;
            } else {
                i++;
                kalMsleep(10);
            }
        }

#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
        if (sys_on == TRUE || prAdapter->prGlueInfo->meta_mode) {
#endif
#if defined(MT5931)
            // Acquire LP-OWN
            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "wlanAdapterStart(): Acquiring LP-OWN\n");
            ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);
#if !CFG_ENABLE_FULL_PM
            nicpmSetDriverOwn(prAdapter);
#endif

            if (prAdapter->fgIsFwOwn == TRUE) {
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "nicpmSetDriverOwn() failed!\n");
                u4Status = WLAN_STATUS_FAILURE;
                ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                break;
            }

            //4 <1> Initialize the Adapter
            if ((u4Status = nicInitializeAdapter(prAdapter)) != WLAN_STATUS_SUCCESS) {
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "nicInitializeAdapter failed!\n");
                u4Status = WLAN_STATUS_FAILURE;
                ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                break;
            }

            /* post initialization for MT5931 due to some CR is only accessible after driver own */
            nicRxPostInitialize(prAdapter);
#endif

            if (u4Status == WLAN_STATUS_SUCCESS) {
                // 1. reset interrupt status
                HAL_READ_INTR_STATUS(prAdapter, 4, &u4WHISR);
                if (HAL_IS_TX_DONE_INTR(u4WHISR)) {
                    HAL_READ_TX_RELEASED_COUNT(prAdapter, aucTxCount);
                }

                /* 2. reset TX Resource for normal operation */
                nicTxResetResource(prAdapter);

#if CFG_SUPPORT_OSC_SETTING && defined(MT5931)
                /* The unit of u2OscStableTime is 1/32K
                   160 unit = 5ms
                */
                wlanSetMcuOscStableTime(prAdapter, 160);
                MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "wlanSetMcuOscStableTime,Time = 5ms");
#endif


                /* 3. query for permanent address by polling */
                if (wlanQueryPermanentAddress(prAdapter) == WLAN_STATUS_FAILURE) {
                    goto fail2;
                }

#if (CFG_SUPPORT_NIC_CAPABILITY == 1)
                /* 4. query for NIC capability */
                if (wlanQueryNicCapability(prAdapter) == WLAN_STATUS_FAILURE) {
                    goto fail2;
                }
#endif

                /* 5. Override network address */
                wlanUpdateNetworkAddress(prAdapter);

                /* 6. indicate disconnection as default status */
#if 1
                prAdapter->prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
#else
                kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                                             WLAN_STATUS_MEDIA_DISCONNECT,
                                             NULL,
                                             0);
#endif
            }

            RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);

            if (u4Status != WLAN_STATUS_SUCCESS) {
                ASSERT(u4Status == WLAN_STATUS_SUCCESS); //TODO(Nelson): error handling for this
                break;
            }

            /* OID timeout timer initialize */
            cnmTimerInitTimer(prAdapter,
                              &prAdapter->rOidTimeoutTimer,
                              (PFN_MGMT_TIMEOUT_FUNC)wlanReleasePendingOid,
                              (UINT_32)NULL);

            /* Power state initialization */
            prAdapter->fgWiFiInSleepyState = FALSE;
            prAdapter->rAcpiState = ACPI_STATE_D0;

            /* Online scan option */
            if (prRegInfo->fgDisOnlineScan == 0) {
                prAdapter->fgEnOnlineScan = TRUE;
            } else {
                prAdapter->fgEnOnlineScan = FALSE;
            }

            /* Beacon lost detection option */
            if (prRegInfo->fgDisBcnLostDetection != 0) {
                prAdapter->fgDisBcnLostDetection = TRUE;
            }

            /* Load compile time constant */
            prAdapter->rWlanInfo.u2BeaconPeriod = CFG_INIT_ADHOC_BEACON_INTERVAL;
            prAdapter->rWlanInfo.u2AtimWindow = CFG_INIT_ADHOC_ATIM_WINDOW;

#if 1// set PM parameters
            prAdapter->fgEnArpFilter = prRegInfo->fgEnArpFilter;
            prAdapter->u4PsCurrentMeasureEn = prRegInfo->u4PsCurrentMeasureEn;

            prAdapter->u4UapsdAcBmp = prRegInfo->u4UapsdAcBmp;

            prAdapter->u4MaxSpLen = prRegInfo->u4MaxSpLen;

            MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "[1] fgEnArpFilter:0x%x, u4UapsdAcBmp:0x%x, u4MaxSpLen:0x%x",
                             prAdapter->fgEnArpFilter,
                             prAdapter->u4UapsdAcBmp,
                             prAdapter->u4MaxSpLen);

            prAdapter->fgEnCtiaPowerMode = FALSE;

#endif

            /* MGMT Initialization */
            nicInitMGMT(prAdapter, prRegInfo);

            /* Enable WZC Disassociation */
            prAdapter->rWifiVar.fgSupportWZCDisassociation = TRUE;

            /* Apply Rate Setting */
            if ((ENUM_REGISTRY_FIXED_RATE_T)(prRegInfo->u4FixedRate) < FIXED_RATE_NUM) {
                prAdapter->rWifiVar.eRateSetting = (ENUM_REGISTRY_FIXED_RATE_T)(prRegInfo->u4FixedRate);
            } else {
                prAdapter->rWifiVar.eRateSetting = FIXED_RATE_NONE;
            }

            if (prAdapter->rWifiVar.eRateSetting == FIXED_RATE_NONE) {
                /* Enable Auto (Long/Short) Preamble */
                prAdapter->rWifiVar.ePreambleType = PREAMBLE_TYPE_AUTO;
            } else if ((prAdapter->rWifiVar.eRateSetting >= FIXED_RATE_MCS0_20M_400NS &&
                        prAdapter->rWifiVar.eRateSetting <= FIXED_RATE_MCS7_20M_400NS)
                       || (prAdapter->rWifiVar.eRateSetting >= FIXED_RATE_MCS0_40M_400NS &&
                           prAdapter->rWifiVar.eRateSetting <= FIXED_RATE_MCS32_400NS)) {
                /* Force Short Preamble */
                prAdapter->rWifiVar.ePreambleType = PREAMBLE_TYPE_SHORT;
            } else {
                /* Force Long Preamble */
                prAdapter->rWifiVar.ePreambleType = PREAMBLE_TYPE_LONG;
            }

            /* Disable Hidden SSID Join */
            prAdapter->rWifiVar.fgEnableJoinToHiddenSSID = FALSE;

            /* Enable Short Slot Time */
            prAdapter->rWifiVar.fgIsShortSlotTimeOptionEnable = TRUE;

            /* configure available PHY type set */
            nicSetAvailablePhyTypeSet(prAdapter);

#if 1// set PM parameters
            {
#if  CFG_SUPPORT_PWR_MGT
                prAdapter->u4PowerMode = prRegInfo->u4PowerMode;
                prAdapter->rWlanInfo.arPowerSaveMode[NETWORK_TYPE_P2P_INDEX].ucNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
                prAdapter->rWlanInfo.arPowerSaveMode[NETWORK_TYPE_P2P_INDEX].ucPsProfile = ENUM_PSP_FAST_SWITCH;
#else
                prAdapter->u4PowerMode = ENUM_PSP_CONTINUOUS_ACTIVE;
#endif

                nicConfigPowerSaveProfile(
                    prAdapter,
                    NETWORK_TYPE_AIS_INDEX, //FIXIT
                    (PARAM_POWER_MODE)prAdapter->u4PowerMode,
                    FALSE);
            }

#endif

#if CFG_SUPPORT_NVRAM
            /* load manufacture data */
            wlanLoadManufactureData(prAdapter, prRegInfo);  //TODO(Nelson): check nvram load
#endif

            /* Default Country Code setting */
            aucCountryCode[0] = (UINT_8)(prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_REG_DOMAIN] >> 8);
            aucCountryCode[1] = (UINT_8)(prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_REG_DOMAIN] & 0xff);

            /* 1. If Wi-Fi set to use channel list index / 2. If Wi-Fi set to use subbandinfo */
            if ((prAdapter->prGlueInfo->ucCh_set_mode == 1) || (prAdapter->prGlueInfo->ucCh_set_mode == 2)) {
                prAdapter->prDomainInfo = &prAdapter->prGlueInfo->rRegInfo.rDomainInfo;
                wlanoidSetCountryCode(prAdapter,
                                      (PVOID)&aucCountryCode[0],
                                      2,
                                      &u4SetInfoLen,
                                      FALSE);
            }
            /* 3. If Wi-Fi set to use country code */
            else {
                wlanoidSetCountryCode(prAdapter,
                                      (PVOID)&aucCountryCode[0],
                                      2,
                                      &u4SetInfoLen,
                                      TRUE);
            }

            /* Default power saving mode setting */
            if (prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_PS_MODE] == WNDRV_POWER_MODE_CTIA) {
                //wndrv_ctia_mode_enable_handler();
                PARAM_CUSTOM_SW_CTRL_STRUC_T rSwCtrlInfo;
                UINT_32 u4SetInfoLen;

                /* Enable ACK_WHEN_FIFO_FULL & Disable ONLINE_SCAN */
                rSwCtrlInfo.u4Id = 0xFFFF1234;
                wlanoidSetSwCtrlWrite(wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen);

                pm = (PARAM_POWER_MODE)Param_PowerModeCAM;
            } else {
                pm = (PARAM_POWER_MODE)prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_PS_MODE];
            }
            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSet802dot11PowerSaveProfile,
                               &pm,
                               sizeof(PARAM_POWER_MODE),
                               (PUINT_32)&u4SetInfoLen);

            /* Default WMMPS setting */
            if (prAdapter->prGlueInfo->as4Cust_config[PARAM_CUST_SET_WMMPS_CONFIG]) {
                P_PARAM_CUSTOM_WMM_PS_TEST_STRUC_T set = &wndrv_context.glue_p->UAPSDSetting;
                PARAM_CUSTOM_SW_CTRL_STRUC_T rSwCtrlInfo;
                UINT_32 u4SetInfoLen;

                /* Enable BE/BK/VI/VO for UAPSD */
                set->bmfgApsdEnAc = UAPSD_AC0 | UAPSD_AC1 | UAPSD_AC2 | UAPSD_AC3;
                set->ucIsEnterPsAtOnce = 1;
                set->ucIsDisableUcTrigger = 0;
                wndrv_UAPSD_isEnable = TRUE;

                /* Enable/disable UPSD support */
                rSwCtrlInfo.u4Id = 0x1001 << 16;
                rSwCtrlInfo.u4Id |= 0x2;
                rSwCtrlInfo.u4Data = set->ucIsEnterPsAtOnce;
                wlanoidSetSwCtrlWrite(wndrv_context.adpt_p, &rSwCtrlInfo, sizeof(rSwCtrlInfo), &u4SetInfoLen);
            }

#if (!CFG_SINGLE_ANTENNA_ENABLED)
            //Enable DPD calibration.
            rBwcsPta.u.aucBTPParams[0] = 0x00;
            rBwcsPta.u.aucBTPParams[1] = 0x01;
            rBwcsPta.u.aucBTPParams[2] = 0x00;
            rBwcsPta.u.aucBTPParams[3] = 0x80;

            wlanoidSetBT(prAdapter,
                         (PVOID)&rBwcsPta,
                         sizeof(PARAM_PTA_IPC_T),
                         &u4SetInfoLen);
#else
            /* Always grand channel for WIFi DPD cal. in meta mode */
            if (prAdapter->prGlueInfo->meta_mode) {
                //Enable DPD calibration.
                rBwcsPta.u.aucBTPParams[0] = 0x00;
                rBwcsPta.u.aucBTPParams[1] = 0x01;
                rBwcsPta.u.aucBTPParams[2] = 0x00;
                rBwcsPta.u.aucBTPParams[3] = 0x80;

                wlanoidSetBT(prAdapter,
                             (PVOID)&rBwcsPta,
                             sizeof(PARAM_PTA_IPC_T),
                             &u4SetInfoLen);
            }
#endif

#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
            kalMainThread(wndrv_context.glue_p);
#endif

#if 0//CFG_SAWLESS_SW_WORKAROUND
            if (!prAdapter->prGlueInfo->meta_mode) {  /* Normal mode */
                /* set power control timer */
                kal_event_stop_timer(PARAM_WNDRV_ES_PWR_CTRL_TIMR);
                kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_PWR_CTRL_TIMR],
                                      (wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_PWR_CTRL_TIMEOUT]) * KAL_TICKS_100_MSEC);
            }
#endif


#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
            /* clock gating workaround */
            prAdapter->fgIsClockGatingEnabled = FALSE;
#endif
#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
        } /* sys_on == TURE || prAdapter->prGlueInfo->meta_mode */
        else {
#if !CFG_ENABLE_FULL_PM
            nicpmSetFWOwn(prAdapter, FALSE);
#endif
            nicRxUninitialize(prAdapter);

            nicTxRelease(prAdapter);

            /* System Service Uninitialization */
            nicUninitSystemService(prAdapter);

            nicReleaseAdapterMemory(prAdapter);
        }
#endif

    } while (FALSE);

#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    if (sys_on == TRUE || prAdapter->prGlueInfo->meta_mode) {
#endif
        if (u4Status == WLAN_STATUS_SUCCESS) {
            // restore to hardware default
            HAL_SET_INTR_STATUS_READ_CLEAR(prAdapter);
            HAL_SET_MAILBOX_READ_CLEAR(prAdapter, FALSE);

            if (!prAdapter->prGlueInfo->meta_mode) {
                /* Enable interrupt */
                nicEnableInterrupt(prAdapter);
            }
#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
            else {
                sys_on = TRUE;
            }
#endif
        } else {
            // release allocated memory
            nicReleaseAdapterMemory(prAdapter);
        }
#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    } else {
        sys_on = TRUE;
        MT5931_TRACE0(TRACE_INFO, MT5931_INFO_218, "[wlanAdapterStart] first time init");
    }
#endif

    kal_task_exit_critical();
    return u4Status;

fail1:
    kal_task_exit_critical();
    return WLAN_STATUS_NOT_ACCEPTED;

fail2:
    kal_task_exit_critical();
    return WLAN_STATUS_FAILURE;

} /* wlanAdapterStart */

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Uninitialize the adapter
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \retval WLAN_STATUS_SUCCESS: Success
* \retval WLAN_STATUS_FAILURE: Failed
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanAdapterStop(
    IN P_ADAPTER_T prAdapter
)
{
    //UINT_32 i, u4Value = 0;
    //WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
    ASSERT(prAdapter);
#if CFG_SAWLESS_SW_WORKAROUND
    if (!prAdapter->prGlueInfo->meta_mode) {  /* Normal mode */
        kal_event_stop_timer(PARAM_WNDRV_ES_PWR_CTRL_TIMR);
    }
#endif

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
    if (prAdapter->fgIsClockGatingEnabled == TRUE) {
        nicDisableClockGating(prAdapter);
    }
#endif

    /* flush the pending TX packets */
    if (prAdapter->prGlueInfo->i4TxPendingFrameNum > 0) {
        kalFlushPendingTxPackets(prAdapter->prGlueInfo);
    }

    /* flush pending security frames */
    if (prAdapter->prGlueInfo->i4TxPendingSecurityFrameNum > 0) {
        kalClearSecurityFrames(prAdapter->prGlueInfo);
    }

    /* flush pending management frames */
    kalClearMgmtFrames(prAdapter->prGlueInfo);

    /* remove pending oid */
    wlanReleasePendingOid(prAdapter->prGlueInfo->prAdapter, 0);

    /* MGMT - unitialization */
    nicUninitMGMT(prAdapter);

#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND   /* NOTE(Nelson): MT5931 will be powered off for feature phone, so return ownback fo FW is not needed */
    if (prAdapter->rAcpiState == ACPI_STATE_D0 &&
#if (CFG_CHIP_RESET_SUPPORT == 1)
            kalIsResetting() == FALSE &&
#endif
            kalIsCardRemoved(prAdapter->prGlueInfo) == FALSE) {
        UINT_32 i, u4Value = 0;
        /* 0. Disable interrupt, this can be done without Driver own */
        nicDisableInterrupt(prAdapter);

        ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);

        /* 1. Set CMD to FW to tell WIFI to stop (enter power off state) */
        if (prAdapter->fgIsFwOwn == FALSE &&
                wlanSendNicPowerCtrlCmd(prAdapter, 1) == WLAN_STATUS_SUCCESS) {
            /* 2. Clear pending interrupt */
            i = 0;
            while (i < CFG_IST_LOOP_COUNT && nicProcessIST(prAdapter) != WLAN_STATUS_NOT_INDICATING) {
                i++;
            };

            /* 3. Wait til RDY bit has been cleaerd */
            i = 0;
            while (1) {
                HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

                if ((u4Value & WCIR_WLAN_READY) == 0) {
                    break;
                } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                           || fgIsBusAccessFailed == TRUE
                           || i >= CFG_RESPONSE_POLLING_TIMEOUT) {
                    break;
                } else {
                    i++;
                    kalMsleep(10);
                }
            }
        }

        /* 4. Set Onwership to F/W */
        nicpmSetFWOwn(prAdapter, FALSE);

        RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);
    }
#endif

    nicRxUninitialize(prAdapter);

    nicTxRelease(prAdapter);

    /* System Service Uninitialization */
    nicUninitSystemService(prAdapter);

    nicReleaseAdapterMemory(prAdapter);

#if defined(_HIF_SPI)
    /* Note: restore the SPI Mode Select from 32 bit to default */
    nicRestoreSpiDefMode(prAdapter);
#endif
#if 0//__WIFI_HIF_SDIO__  do not close due to sdioclose times > sdioopen times make sdio lock rule assert when open/close wifi
    Status = glSdioClose();
    if (Status == FALSE) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "glSdioClose() reports error: %x", Status);
    }
#endif
    WiFi_EintMask();
#if !CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    if (prAdapter->ucRevID != MT5931_E2_REVERSION_ID) {
        LB_WIFI_Chip_PowerOff();
    }
#else
    //nicpmSetFWOwn(prAdapter, FALSE);
    MT5931_TRACE0(TRACE_DEINIT, MT5931_INFO_219, "[wlanAdapterStop] always on");
#endif
    /* Reset MAC address in adapter */
    kalMemSet(&(prAdapter->rMyMacAddr), 0, MAC_ADDR_LEN);

    // DCXO co-clock configuration
    if (wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DCXO_CONFIG] & WIFI_SUPPORT_DCXO_CO_CLOCK) {
        //WiFi_co_clock_config(0);
    }
    return WLAN_STATUS_SUCCESS;
} /* wlanAdapterStop */


/*----------------------------------------------------------------------------*/
/*!
* \deinit ap or sta mode for ap&sta concurrent
*  AP mode: release buffer, must deactive network beacuse network is active when MMI close tethering with sta mode on
*  sta mode: release buffer, reset fsm
*
* \param prAdapter      Pointer of Adapter Data Structure
* \param fgIsAPmode      TRUE: AP mode ; FALSE :STA mode
*
* \retval WLAN_STATUS_SUCCESS: Success
* \retval WLAN_STATUS_FAILURE: Failed
*/
/*----------------------------------------------------------------------------*/

WLAN_STATUS
wlanAdapterStopForAPorSTA(
    IN P_ADAPTER_T  prAdapter,
    IN kal_bool     fgIsAPmode
)
{

#if CFG_ENABLE_WIFI_TETHERING
    if (fgIsAPmode) { //deinit ap mode

        P_BSS_INFO_T prP2pBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_FUNC, "******Deinit AP mode driver for concurrent");

        /*1. release pending  MGMT in glue layer,
               do not release driver cmd(COMMAND_TYPE_GENERAL_IOCTL,COMMAND_TYPE_NETWORK_IOCTL)*/
        kalClearMgmtFramesByNetType(prAdapter->prGlueInfo, NETWORK_TYPE_P2P_INDEX);

        /*2. release pending  Security in glue layer,
               do not release driver cmd(COMMAND_TYPE_GENERAL_IOCTL,COMMAND_TYPE_NETWORK_IOCTL)*/
        kalClearSecurityFramesByNetType(prAdapter->prGlueInfo, NETWORK_TYPE_P2P_INDEX);

        //3. release pengding CMD/IOD in adapter layer due to waiting fw event
        wlanReleasePendingCMDbyNetwork(prAdapter, NETWORK_TYPE_P2P_INDEX);

        //4. release pending TX packets in glue layer
        if (prAdapter->prGlueInfo->i4TxPendingFrameNum > 0) {
            kalFlushPendingTxPacketsByNetwork(prAdapter->prGlueInfo, NETWORK_TYPE_P2P_INDEX);
        }

        //5. release pending TX packets in adapter layer->>need check
        nicFreePendingTxMsduInfoByNetwork(prAdapter, NETWORK_TYPE_P2P_INDEX);

        //6. release pending RX packets in adapter layer
        nicReleaseRxRFBByNetwork(prAdapter, NETWORK_TYPE_P2P_INDEX);

        //7. release beacon buffer
        if (prP2pBssInfo->prBeacon) {
            cnmMgtPktFree(prAdapter, prP2pBssInfo->prBeacon);
            prP2pBssInfo->prBeacon = NULL;
        }


        wtChangeMediaState(prAdapter, PARAM_MEDIA_STATE_DISCONNECTED);

        /* **************
              *1. Update Disconnected state to FW.
          *2. bssClearClientList
          *3. free starec,
        ***********/
        nicUpdateBss(prAdapter, NETWORK_TYPE_P2P_INDEX);


        nicPmIndicateBssAbort(prAdapter, NETWORK_TYPE_P2P_INDEX);


        /************
        *1. deactive network,
        *2. free starec (include prStaRec->arTxQueue[0~5], ->>RX BA queues )
        *3. free tx_packet,
        *4. free qm(prQM->arTxQueue[0])
        *5. pending  security without resource
        ******************************/
        if (!IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_P2P_INDEX)) {
            kal_wap_trace(MOD_WNDRV, TRACE_ERROR, "!!!!!!!!!NETWORK_TYPE_P2P_INDEX is inactiv!!!!!!");
            ASSERT(0);
        } else if (IS_NET_PWR_STATE_IDLE(prAdapter, NETWORK_TYPE_P2P_INDEX)) {
            kal_wap_trace(MOD_WNDRV, TRACE_ERROR, "!!!!!!!!NETWORK_TYPE_P2P_INDEX PWR is idle!!!!!!");
            ASSERT(0);
        }
        UNSET_NET_ACTIVE(prAdapter, NETWORK_TYPE_P2P_INDEX);
        SET_NET_PWR_STATE_IDLE(prAdapter, NETWORK_TYPE_P2P_INDEX);
        nicDeactivateNetwork(prAdapter, NETWORK_TYPE_P2P_INDEX);

    } else //deinit sta mode
#endif
    {

        //P_BSS_INFO_T prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_FUNC, "******Deinit STA mode driver for concurrent");

        /*1. release pending  MGMT in glue layer,
               do not release driver cmd(COMMAND_TYPE_GENERAL_IOCTL,COMMAND_TYPE_NETWORK_IOCTL)*/
        kalClearMgmtFramesByNetType(prAdapter->prGlueInfo, NETWORK_TYPE_AIS_INDEX);

        /*2. release pending  Security in glue layer,
               do not release driver cmd(COMMAND_TYPE_GENERAL_IOCTL,COMMAND_TYPE_NETWORK_IOCTL)*/
        kalClearSecurityFramesByNetType(prAdapter->prGlueInfo, NETWORK_TYPE_AIS_INDEX);

        //3. release pengding CMD/IOD in adapter layer due to waiting fw event
        wlanReleasePendingCMDbyNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);

        //4. release pending TX packets in glue layer
        if (prAdapter->prGlueInfo->i4TxPendingFrameNum > 0) {
            kalFlushPendingTxPacketsByNetwork(prAdapter->prGlueInfo, NETWORK_TYPE_AIS_INDEX);
        }

        //5. release pending TX packets in adapter layer->>need check
        nicFreePendingTxMsduInfoByNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);

        //6. release pending RX packets in adapter layer
        nicReleaseRxRFBByNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);

        /*7. ***********
        *7.1  deactive network,
        *7.2  free starec (include prStaRec->arTxQueue[0~5], ->>RX BA queues )
        *7.3  free tx_packet,
        *7.4  free qm(prQM->arTxQueue[0])
        *7.5  pending  security without resource
        ******************************/
        if ((IS_NET_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX)) \
                || (IS_NET_PWR_STATE_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX))) {

            kal_wap_trace(MOD_WNDRV, TRACE_WARNING, "!!!!!!!!NETWORK_TYPE_AIS_INDEX is active!!!!!!!!!!!!!");
            ASSERT(0);
            UNSET_NET_ACTIVE(prAdapter, NETWORK_TYPE_AIS_INDEX);
            SET_NET_PWR_STATE_IDLE(prAdapter, NETWORK_TYPE_AIS_INDEX);
            nicDeactivateNetwork(prAdapter, NETWORK_TYPE_AIS_INDEX);
        }

        //8 reset  fsm of sta mode
        nicUninitMGMT(prAdapter);
        nicInitMGMT(prAdapter, NULL);

    }

    return WLAN_STATUS_SUCCESS;

}


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is called by ISR (interrupt).
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \retval TRUE: NIC's interrupt
* \retval FALSE: Not NIC's interrupt
*/
/*----------------------------------------------------------------------------*/
BOOL
wlanISR(
    IN P_ADAPTER_T prAdapter,
    IN BOOLEAN fgGlobalIntrCtrl
)
{
    ASSERT(prAdapter);

    if (fgGlobalIntrCtrl) {
#if __WIFI_HIF_SDIO__
        /* This function is used to Protect if  ISR  is unmask by other function */
        if (!(prAdapter->fgIsIntEnable)) {
            return FALSE;
        }
        prAdapter->fgIsIntEnable = FALSE;
        //WiFi_GPIO_ModeSetup(1, 0);
#else
        nicDisableInterrupt(prAdapter);
#endif
        //wlanIST(prAdapter);
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is called by IST (task_let).
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanIST(
    IN P_ADAPTER_T prAdapter
)
{
    ASSERT(prAdapter);

    ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);
#if __WIFI_HIF_SDIO__
    nicDisableInterrupt(prAdapter);
#endif
    nicProcessIST(prAdapter);

    nicEnableInterrupt(prAdapter);

    RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will check command queue to find out if any could be dequeued
*        and/or send to HIF to MT6620
*
* \param prAdapter      Pointer of Adapter Data Structure
* \param prCmdQue       Pointer of Command Queue (in Glue Layer)
*
* \retval WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanProcessCommandQueue(
    IN P_ADAPTER_T  prAdapter,
    IN P_QUE_T      prCmdQue
)
{
    WLAN_STATUS rStatus;
    QUE_T rTempCmdQue, rMergeCmdQue, rStandInCmdQue;
    P_QUE_T prTempCmdQue, prMergeCmdQue, prStandInCmdQue;
    P_QUE_ENTRY_T prQueueEntry;
    P_CMD_INFO_T prCmdInfo;
    P_MSDU_INFO_T prMsduInfo;
    ENUM_FRAME_ACTION_T eFrameAction = FRAME_ACTION_DROP_PKT;
    //UINT_32 loop_count = 0;
    //UINT_32 u4IntStatus = 0;

    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);
    ASSERT(prCmdQue);
    //ILMTraceCheck(80);
    prTempCmdQue = &rTempCmdQue;
    prMergeCmdQue = &rMergeCmdQue;
    prStandInCmdQue = &rStandInCmdQue;

    QUEUE_INITIALIZE(prTempCmdQue);
    QUEUE_INITIALIZE(prMergeCmdQue);
    QUEUE_INITIALIZE(prStandInCmdQue);

    //4 <1> Move whole list of CMD_INFO to temp queue
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_QUE);

    //4 <2> Dequeue from head and check it is able to be sent
    QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    while (prQueueEntry) {
        prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

        switch (prCmdInfo->eCmdType) {
            case COMMAND_TYPE_GENERAL_IOCTL:
            case COMMAND_TYPE_NETWORK_IOCTL:
                /* command packet will be always sent */
                eFrameAction = FRAME_ACTION_TX_PKT;
                break;

            case COMMAND_TYPE_SECURITY_FRAME:
                /* inquire with QM */
                eFrameAction = qmGetFrameAction(prAdapter,
                                                prCmdInfo->eNetworkType,
                                                prCmdInfo->ucStaRecIndex,
                                                NULL,
                                                FRAME_TYPE_802_1X);
                // MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_220, "COMMAND_TYPE_SECURITY_FRAME: eFrameAction %d", eFrameAction);
                break;

            case COMMAND_TYPE_MANAGEMENT_FRAME:
                /* inquire with QM */
                prMsduInfo = (P_MSDU_INFO_T)(prCmdInfo->prPacket);

                eFrameAction = qmGetFrameAction(prAdapter,
                                                (ENUM_NETWORK_TYPE_INDEX_T)prMsduInfo->ucNetworkType,
                                                prMsduInfo->ucStaRecIndex,
                                                prMsduInfo,
                                                FRAME_TYPE_MMPDU);
                break;

            default:
                ASSERT(0);
                break;
        }

        //4 <3> handling upon dequeue result
        if (eFrameAction == FRAME_ACTION_DROP_PKT) {
            wlanReleaseCommand(prAdapter, prCmdInfo);
        } else if (eFrameAction == FRAME_ACTION_QUEUE_PKT) {
            QUEUE_INSERT_TAIL(prMergeCmdQue, prQueueEntry);
        } else if (eFrameAction == FRAME_ACTION_TX_PKT) {
            //4 <4> Send the command
            rStatus = wlanSendCommand(prAdapter, prCmdInfo);

            if (rStatus == WLAN_STATUS_RESOURCES) {
                // no more TC4 resource for further transmission
                QUEUE_INSERT_TAIL(prMergeCmdQue, prQueueEntry);
                break;
            } else if (rStatus == WLAN_STATUS_PENDING) {
                // command packet which needs further handling upon response
                KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_PENDING);
                QUEUE_INSERT_TAIL(&(prAdapter->rPendingCmdQueue), prQueueEntry);
                KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_PENDING);
            } else {
                P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

                if (rStatus == WLAN_STATUS_SUCCESS) {
                    if (prCmdInfo->pfCmdDoneHandler) {
                        prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prCmdInfo->pucInfoBuffer);
                    }
                } else {
                    if (prCmdInfo->fgIsOid) {
                        kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, prCmdInfo->u4SetInfoLen, rStatus);
                    }
                }

                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
            }
        } else {
            ASSERT(0);
        }

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
    }

    //4 <3> Merge back to original queue
    //4 <3.1> Merge prMergeCmdQue & prTempCmdQue
    QUEUE_CONCATENATE_QUEUES(prMergeCmdQue, prTempCmdQue);

    //4 <3.2> Move prCmdQue to prStandInQue, due to prCmdQue might differ due to incoming 802.1X frames
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_QUE);
    QUEUE_MOVE_ALL(prStandInCmdQue, prCmdQue);

    //4 <3.3> concatenate prStandInQue to prMergeCmdQue
    QUEUE_CONCATENATE_QUEUES(prMergeCmdQue, prStandInCmdQue);

    //4 <3.4> then move prMergeCmdQue to prCmdQue
    QUEUE_MOVE_ALL(prCmdQue, prMergeCmdQue);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_QUE);
    //ILMTraceCheck(89);
    return WLAN_STATUS_SUCCESS;
} /* end of wlanProcessCommandQueue() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will take CMD_INFO_T which carry some informations of
*        incoming OID and notify the NIC_TX to send CMD.
*
* \param prAdapter      Pointer of Adapter Data Structure
* \param prCmdInfo      Pointer of P_CMD_INFO_T
*
* \retval WLAN_STATUS_SUCCESS   : CMD was written to HIF and be freed(CMD Done) immediately.
* \retval WLAN_STATUS_RESOURCE  : No resource for current command, need to wait for previous
*                                 frame finishing their transmission.
* \retval WLAN_STATUS_FAILURE   : Get failure while access HIF or been rejected.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSendCommand(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
)
{
    //P_TX_CTRL_T prTxCtrl;
    UINT_8 ucTC; /* "Traffic Class" SW(Driver) resource classification */
    WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
    kal_uint32 u4IntStatus = 0;
    UINT_32 loop_count = 0;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    //prTxCtrl = &prAdapter->rTxCtrl;

    //DEBUGFUNC("wlanSendCommand");
    //
    //
#if 0
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INFO, "wlanSendCommand(),CMD ID = %x\n", prCmdInfo->ucCID);
    LOG_FUNC("CmdType %u NetworkType %u StaRecIndex %u Oid %u CID 0x%x SetQuery %u NeedResp %u CmdSeqNum %u\n",
             prCmdInfo->eCmdType,
             prCmdInfo->eNetworkType,
             prCmdInfo->ucStaRecIndex,
             prCmdInfo->fgIsOid,
             prCmdInfo->ucCID,
             prCmdInfo->fgSetQuery,
             prCmdInfo->fgNeedResp,
             prCmdInfo->ucCmdSeqNum);
#endif

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
    if (prAdapter->fgIsClockGatingEnabled == TRUE) {
        nicDisableClockGating(prAdapter);
    }
#endif

    do {
        // <0> card removal check
        if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            rStatus = WLAN_STATUS_FAILURE;
            break;
        }

        // <1> Normal case of sending CMD Packet
        if (!prCmdInfo->fgDriverDomainMCR) {
            // <1.1> Assign Traffic Class(TC) = TC4.
            ucTC = TC4_INDEX;

            // <1.2> Check if pending packet or resource was exhausted
            if ((rStatus = nicTxAcquireResource(prAdapter, ucTC)) == WLAN_STATUS_RESOURCES) {
                if (prAdapter->prGlueInfo->meta_mode) {
                    /* try to polling interrupt release Resource */
                    while (1) {
                        HAL_MCR_RD(prAdapter, MCR_WHISR, &u4IntStatus);
                        if (u4IntStatus & WHISR_TX_DONE_INT) {
                            nicProcessTxInterrupt(prAdapter);
                            /* reacquire resource */
                            nicTxAcquireResource(prAdapter, ucTC);
                            break; /* leave while(1) */
                        }

                        loop_count++;
                        kalUdelay(500);
                        if (loop_count > 51200) {
                            MT5931_NEW_TRACE0(MT5931_INFO_432,  TRACE_ERROR, "no resource");
                            WNDRV_EXT_ASSERT(0, "CMD %d CNT %d S %x", prCmdInfo->ucCID, loop_count, u4IntStatus);
                            break;
                        }
                    }
                } else {
                    //DbgPrint("NO Resource:%d\n", ucTC);
                    MT5931_NEW_TRACE(MT5931_INFO_433,  TRACE_WARNING, "wlanSendCommand: WLAN_STATUS_RESOURCES:NO Resource:%d\n", ucTC);
                    break;
                }
            }

            // <1.3> Forward CMD_INFO_T to NIC Layer
            rStatus = nicTxCmd(prAdapter, prCmdInfo, ucTC);

            // <1.4> Set Pending in response to Query Command/Need Response
            if (rStatus == WLAN_STATUS_SUCCESS) {
                if ((!prCmdInfo->fgSetQuery) || (prCmdInfo->fgNeedResp)) {
                    rStatus = WLAN_STATUS_PENDING;
                }

                /* Nelson: poll TX done interrupt in meta mode start */
#if 0
                prAdapter->fgIsTxIntPoll = FALSE;
                prAdapter->fgIsRxIntPoll = TRUE;
#endif

                if (prAdapter->prGlueInfo->meta_mode) {
                    //if ( prAdapter->rTxCtrl.rTc.aucFreeBufferCount[ucTC] == 0 )    /* CMD queue full */
                    if (prAdapter->prGlueInfo->fgIsAdaptStopped == FALSE && prAdapter->rTxCtrl.rTc.aucFreeBufferCount[ucTC] == 0) {
                        while (1) {
                            HAL_MCR_RD(prAdapter, MCR_WHISR, &u4IntStatus);
                            if (u4IntStatus & WHISR_TX_DONE_INT) {
                                nicProcessTxInterrupt(prAdapter);
                                break;
                            }

                            loop_count++;
                            kalUdelay(500);
                            if (loop_count > 51200) {
                                WNDRV_EXT_ASSERT(0, "CMD %d CNT %d S %x", prCmdInfo->ucCID, loop_count, u4IntStatus);
                                break;
                            }
                        }
                    }
                }
                /* Nelson: poll TX done interrupt in meta mode end */

            }
        }
        // <2> Special case for access Driver Domain MCR
        else {
            P_CMD_ACCESS_REG prCmdAccessReg;
            prCmdAccessReg = (P_CMD_ACCESS_REG)(prCmdInfo->pucInfoBuffer + CMD_HDR_SIZE);

            if (prCmdInfo->fgSetQuery) {
                HAL_MCR_WR(prAdapter,
                           (prCmdAccessReg->u4Address & BITS(2, 31)), //address is in DWORD unit
                           prCmdAccessReg->u4Data);
            } else {
                P_CMD_ACCESS_REG prEventAccessReg;
                UINT_32 u4Address;

                u4Address = prCmdAccessReg->u4Address;
                prEventAccessReg = (P_CMD_ACCESS_REG)prCmdInfo->pucInfoBuffer;
                prEventAccessReg->u4Address = u4Address;

                HAL_MCR_RD(prAdapter,
                           prEventAccessReg->u4Address & BITS(2, 31), //address is in DWORD unit
                           &prEventAccessReg->u4Data);
            }
        }

    } while (FALSE);

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
    if (prAdapter->fgIsClockGatingEnabled == FALSE) {
        nicEnableClockGating(prAdapter);
    }
#endif

    return rStatus;
} /* end of wlanSendCommand() */


/*----------------------------------------------------------------------------*/
/*!
 * \brief This function will release thd CMD_INFO upon its attribution
 *
 * \param prAdapter  Pointer of Adapter Data Structure
 * \param prCmdInfo  Pointer of CMD_INFO_T
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
VOID
wlanReleaseCommand(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
)
{
    P_TX_CTRL_T prTxCtrl;
    P_MSDU_INFO_T prMsduInfo;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prTxCtrl = &prAdapter->rTxCtrl;

    switch (prCmdInfo->eCmdType) {
        case COMMAND_TYPE_GENERAL_IOCTL:
        case COMMAND_TYPE_NETWORK_IOCTL:
            if (prCmdInfo->fgIsOid) {
                kalOidComplete(prAdapter->prGlueInfo,
                               prCmdInfo->fgSetQuery,
                               prCmdInfo->u4SetInfoLen,
                               WLAN_STATUS_FAILURE);
            }
            break;

        case COMMAND_TYPE_SECURITY_FRAME:
            kalSecurityFrameSendComplete(prAdapter->prGlueInfo,
                                         prCmdInfo->prPacket,
                                         WLAN_STATUS_FAILURE);
            break;

        case COMMAND_TYPE_MANAGEMENT_FRAME:
            prMsduInfo = (P_MSDU_INFO_T)prCmdInfo->prPacket;

            /* invoke callbacks */
            if (prMsduInfo->pfTxDoneHandler != NULL) {
                prMsduInfo->pfTxDoneHandler(prAdapter, prMsduInfo, TX_RESULT_DROPPED_IN_DRIVER);
            }

            GLUE_DEC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);
            cnmMgtPktFree(prAdapter, prMsduInfo);
            break;

        default:
            ASSERT(0);
            break;
    }

    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

} /* end of wlanReleaseCommand() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief This function will search CMD Queue to look for the pengind CMD/OID for specific
    network type and compelete it
 *
 * \param prAdapter  Pointer of Adapter Data Structure
 * \param eNetworkType
 *
 * \return (none)
 */
/*----------------------------------------------------------------------------*/
VOID
wlanReleasePendingCMDbyNetwork(
    IN P_ADAPTER_T  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;
    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;

    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_FUNC, "wlanReleasePendingCMDbyNetwork, eNetworkTypeIdx=%d", eNetworkTypeIdx);

    do {

        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_PENDING);

        prCmdQue = &prAdapter->rPendingCmdQueue;

        QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);

        while (prQueueEntry) {
            prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

            if (prCmdInfo->eNetworkType == eNetworkTypeIdx) {
                if (prCmdInfo->pfCmdTimeoutHandler) {
                    prCmdInfo->pfCmdTimeoutHandler(prAdapter, prCmdInfo);
                } else
                    kalOidComplete(prAdapter->prGlueInfo,
                                   prCmdInfo->fgSetQuery,
                                   0,
                                   WLAN_STATUS_FAILURE);

                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

            } else {
                QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
            }

            QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
        }

        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_PENDING);

    } while (FALSE);

    return;
} /* end of wlanReleaseCommand() */

/*----------------------------------------------------------------------------*/
/*!
* \brief This function will search the CMD Queue to look for the pending OID and
*        compelete it immediately when system request a reset.
*
* \param prAdapter  ointer of Adapter Data Structure
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanReleasePendingOid(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4Data
)
{
    P_QUE_T prCmdQue;
    QUE_T rTempCmdQue;
    P_QUE_T prTempCmdQue = &rTempCmdQue;
    P_QUE_ENTRY_T prQueueEntry = (P_QUE_ENTRY_T)NULL;
    P_CMD_INFO_T prCmdInfo = (P_CMD_INFO_T)NULL;

    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    do {
        // 1: Clear Pending OID in prAdapter->rPendingCmdQueue
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_PENDING);

        prCmdQue = &prAdapter->rPendingCmdQueue;
        QUEUE_MOVE_ALL(prTempCmdQue, prCmdQue);

        QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
        while (prQueueEntry) {
            prCmdInfo = (P_CMD_INFO_T)prQueueEntry;

            if (prCmdInfo->fgIsOid) {
                if (prCmdInfo->pfCmdTimeoutHandler) {
                    prCmdInfo->pfCmdTimeoutHandler(prAdapter, prCmdInfo);
                } else
                    kalOidComplete(prAdapter->prGlueInfo,
                                   prCmdInfo->fgSetQuery,
                                   0,
                                   WLAN_STATUS_FAILURE);

                cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
            } else {
                QUEUE_INSERT_TAIL(prCmdQue, prQueueEntry);
            }

            QUEUE_REMOVE_HEAD(prTempCmdQue, prQueueEntry, P_QUE_ENTRY_T);
        }

        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_PENDING);

        // 2: Clear pending OID in glue layer command queue
        kalOidCmdClearance(prAdapter->prGlueInfo);

        // 3: Clear pending OID queued in pvOidEntry with REQ_FLAG_OID set
        kalOidClearance(prAdapter->prGlueInfo);

    } while (FALSE);

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Return the packet buffer and reallocate one to the RFB
*
* \param prAdapter      Pointer of Adapter Data Structure
* \param pvPacket       Pointer of returned packet
*
* \retval WLAN_STATUS_SUCCESS: Success
* \retval WLAN_STATUS_FAILURE: Failed
*/
/*----------------------------------------------------------------------------*/
VOID
wlanReturnPacket(
    IN P_ADAPTER_T prAdapter,
    IN PVOID pvPacket
)
{
    P_RX_CTRL_T prRxCtrl;
    //P_SW_RFB_T prSwRfb = NULL;
    KAL_SPIN_LOCK_DECLARATION();


    ASSERT(prAdapter);

    prRxCtrl = &prAdapter->rRxCtrl;
    ASSERT(prRxCtrl);

    if (pvPacket) {
        kalPacketFree(prAdapter->prGlueInfo, pvPacket);
        RX_ADD_CNT(prRxCtrl, RX_DATA_RETURNED_COUNT, 1);
#if CFG_NATIVE_802_11
        if (GLUE_TEST_FLAG(prAdapter->prGlueInfo, GLUE_FLAG_HALT)) {
        }
#endif
    }

#if !defined(WCN_MAUI)
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
    QUEUE_REMOVE_HEAD(&prRxCtrl->rIndicatedRfbList, prSwRfb, P_SW_RFB_T);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_RX_QUE);
    if (!prSwRfb) {
        ASSERT(0);
        return;
    }

    if (nicRxSetupRFB(prAdapter, prSwRfb)) {
        ASSERT(0);
        return;
    }
    nicRxReturnRFB(prAdapter, prSwRfb);
#endif /* !WCN_MAUI */
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is a required function that returns information about
*        the capabilities and status of the driver and/or its network adapter.
*
* \param[IN] prAdapter        Pointer to the Adapter structure.
* \param[IN] pfnOidQryHandler Function pointer for the OID query handler.
* \param[IN] pvInfoBuf        Points to a buffer for return the query information.
* \param[IN] u4QueryBufferLen Specifies the number of bytes at pvInfoBuf.
* \param[OUT] pu4QueryInfoLen  Points to the number of bytes it written or is needed.
*
* \retval WLAN_STATUS_xxx Different WLAN_STATUS code returned by different handlers.
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryInformation(
    IN P_ADAPTER_T          prAdapter,
    IN PFN_OID_HANDLER_FUNC pfnOidQryHandler,
    IN PVOID                pvInfoBuf,
    IN UINT_32              u4InfoBufLen,
    OUT PUINT_32            pu4QryInfoLen
)
{
    WLAN_STATUS status = WLAN_STATUS_FAILURE;

    ASSERT(prAdapter);
    ASSERT(pu4QryInfoLen);

    // ignore any OID request after connected, under PS current measurement mode
    if (prAdapter->u4PsCurrentMeasureEn &&
            (prAdapter->prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED)) {
        return WLAN_STATUS_SUCCESS; // note: return WLAN_STATUS_FAILURE or WLAN_STATUS_SUCCESS for blocking OIDs during current measurement ??
    }

#if 1
    /* most OID handler will just queue a command packet */
    status = pfnOidQryHandler(prAdapter,
                              pvInfoBuf,
                              u4InfoBufLen,
                              pu4QryInfoLen);
#else
    if (wlanIsHandlerNeedHwAccess(pfnOidQryHandler, FALSE)) {
        ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);

        /* Reset sleepy state */
        if (prAdapter->fgWiFiInSleepyState == TRUE) {
            prAdapter->fgWiFiInSleepyState = FALSE;
        }

        status = pfnOidQryHandler(prAdapter,
                                  pvInfoBuf,
                                  u4InfoBufLen,
                                  pu4QryInfoLen);

        RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);
    } else {
        status = pfnOidQryHandler(prAdapter,
                                  pvInfoBuf,
                                  u4InfoBufLen,
                                  pu4QryInfoLen);
    }
#endif

    return status;

}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is a required function that allows bound protocol drivers,
*        or NDIS, to request changes in the state information that the miniport
*        maintains for particular object identifiers, such as changes in multicast
*        addresses.
*
* \param[IN] prAdapter     Pointer to the Glue info structure.
* \param[IN] pfnOidSetHandler     Points to the OID set handlers.
* \param[IN] pvInfoBuf     Points to a buffer containing the OID-specific data for the set.
* \param[IN] u4InfoBufLen  Specifies the number of bytes at prSetBuffer.
* \param[OUT] pu4SetInfoLen Points to the number of bytes it read or is needed.
*
* \retval WLAN_STATUS_xxx Different WLAN_STATUS code returned by different handlers.
*
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSetInformation(
    IN P_ADAPTER_T          prAdapter,
    IN PFN_OID_HANDLER_FUNC pfnOidSetHandler,
    IN PVOID                pvInfoBuf,
    IN UINT_32              u4InfoBufLen,
    OUT PUINT_32            pu4SetInfoLen
)
{
    WLAN_STATUS status = WLAN_STATUS_FAILURE;

    ASSERT(prAdapter);
    ASSERT(pu4SetInfoLen);

    // ignore any OID request after connected, under PS current measurement mode
    if (prAdapter->u4PsCurrentMeasureEn &&
            (prAdapter->prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED)) {
        return WLAN_STATUS_SUCCESS; // note: return WLAN_STATUS_FAILURE or WLAN_STATUS_SUCCESS for blocking OIDs during current measurement ??
    }

#if 1
    /* most OID handler will just queue a command packet
     * for power state transition OIDs, handler will acquire power control by itself
     */
    status = pfnOidSetHandler(prAdapter,
                              pvInfoBuf,
                              u4InfoBufLen,
                              pu4SetInfoLen);
#else
    if (wlanIsHandlerNeedHwAccess(pfnOidSetHandler, TRUE)) {
        ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);

        /* Reset sleepy state */
        if (prAdapter->fgWiFiInSleepyState == TRUE) {
            prAdapter->fgWiFiInSleepyState = FALSE;
        }

        status = pfnOidSetHandler(prAdapter,
                                  pvInfoBuf,
                                  u4InfoBufLen,
                                  pu4SetInfoLen);

        RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);
    } else {
        status = pfnOidSetHandler(prAdapter,
                                  pvInfoBuf,
                                  u4InfoBufLen,
                                  pu4SetInfoLen);
    }
#endif

    return status;
}


#ifdef __WAPI_SUPPORT__
/*----------------------------------------------------------------------------*/
/*!
* \brief This function is a used to query driver's config wapi mode or not
*
* \param[IN] prAdapter     Pointer to the Glue info structure.
*
* \retval TRUE for use wapi mode
*
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanQueryWapiMode(
    IN P_ADAPTER_T          prAdapter
)
{
    ASSERT(prAdapter);

    return prAdapter->rWifiVar.rConnSettings.fgWapiMode;
}
#endif


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is called to set RX filter to Promiscuous Mode.
*
* \param[IN] prAdapter        Pointer to the Adapter structure.
* \param[IN] fgEnablePromiscuousMode Enable/ disable RX Promiscuous Mode.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanSetPromiscuousMode(
    IN P_ADAPTER_T  prAdapter,
    IN BOOLEAN      fgEnablePromiscuousMode
)
{
    ASSERT(prAdapter);

}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is called to set RX filter to allow to receive
*        broadcast address packets.
*
* \param[IN] prAdapter        Pointer to the Adapter structure.
* \param[IN] fgEnableBroadcast Enable/ disable broadcast packet to be received.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanRxSetBroadcast(
    IN P_ADAPTER_T  prAdapter,
    IN BOOLEAN      fgEnableBroadcast
)
{
    ASSERT(prAdapter);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is called to send out CMD_NIC_POWER_CTRL command packet
*
* \param[IN] prAdapter        Pointer to the Adapter structure.
* \param[IN] ucPowerMode      refer to CMD/EVENT document
*
* \return WLAN_STATUS_SUCCESS
* \return WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSendNicPowerCtrlCmd(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucPowerMode
)
{
    WLAN_STATUS status = WLAN_STATUS_SUCCESS;
    //P_GLUE_INFO_T prGlueInfo;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    UINT_8 ucTC, ucCmdSeqNum;

    ASSERT(prAdapter);

    //prGlueInfo = prAdapter->prGlueInfo;

    /* 1. Prepare CMD */
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, (CMD_HDR_SIZE + sizeof(CMD_NIC_POWER_CTRL)));
    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    /* 2.1 increase command sequence number */
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);
    DBGLOG(REQ, TRACE, ("ucCmdSeqNum =%d\n", ucCmdSeqNum));

    /* 2.2 Setup common CMD Info Packet */
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = (UINT_16)(CMD_HDR_SIZE + sizeof(CMD_NIC_POWER_CTRL));
    prCmdInfo->pfCmdDoneHandler = NULL;
    prCmdInfo->pfCmdTimeoutHandler = NULL;
    prCmdInfo->fgIsOid = TRUE;
    prCmdInfo->ucCID = CMD_ID_NIC_POWER_CTRL;
    prCmdInfo->fgSetQuery = TRUE;
    prCmdInfo->fgNeedResp = FALSE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_NIC_POWER_CTRL);

    /* 2.3 Setup WIFI_CMD_T */
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    kalMemZero(prWifiCmd->aucBuffer, sizeof(CMD_NIC_POWER_CTRL));
    ((P_CMD_NIC_POWER_CTRL)(prWifiCmd->aucBuffer))->ucPowerMode = ucPowerMode;

    /* 3. Issue CMD for entering specific power mode */
    ucTC = TC4_INDEX;

    while (1) {
        // 3.0 Removal check
        if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            status = WLAN_STATUS_FAILURE;
            break;
        }

        // 3.1 Acquire TX Resource
        if (nicTxAcquireResource(prAdapter, ucTC) == WLAN_STATUS_RESOURCES) {
            if (nicTxPollingResource(prAdapter, ucTC) != WLAN_STATUS_SUCCESS) {
                DBGLOG(INIT, ERROR, ("Fail to get TX resource return within timeout\n"));
                status = WLAN_STATUS_FAILURE;
                break;
            } else {
                continue;
            }
        }

        // 3.2 Send CMD Info Packet
        if (nicTxCmd(prAdapter, prCmdInfo, ucTC) != WLAN_STATUS_SUCCESS) {
            DBGLOG(INIT, ERROR, ("Fail to transmit CMD_NIC_POWER_CTRL command\n"));
            status = WLAN_STATUS_FAILURE;
        }

        break;
    };

    // 4. Free CMD Info Packet.
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    // 5. Add flag
    if (ucPowerMode == 1) {
        prAdapter->fgIsEnterD3ReqIssued = TRUE;
    }

    return status;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is called to check if it is RF test mode and
*        the OID is allowed to be called or not
*
* \param[IN] prAdapter        Pointer to the Adapter structure.
* \param[IN] fgEnableBroadcast Enable/ disable broadcast packet to be received.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanIsHandlerAllowedInRFTest(
    IN PFN_OID_HANDLER_FUNC pfnOidHandler,
    IN BOOLEAN              fgSetInfo
)
{
    PFN_OID_HANDLER_FUNC *apfnOidHandlerAllowedInRFTest;
    UINT_32 i;
    UINT_32 u4NumOfElem;

    if (fgSetInfo) {
        apfnOidHandlerAllowedInRFTest = apfnOidSetHandlerAllowedInRFTest;
        u4NumOfElem = sizeof(apfnOidSetHandlerAllowedInRFTest) / sizeof(PFN_OID_HANDLER_FUNC);
    } else {
        apfnOidHandlerAllowedInRFTest = apfnOidQueryHandlerAllowedInRFTest;
        u4NumOfElem = sizeof(apfnOidQueryHandlerAllowedInRFTest) / sizeof(PFN_OID_HANDLER_FUNC);
    }

    for (i = 0; i < u4NumOfElem; i++) {
        if (apfnOidHandlerAllowedInRFTest[i] == pfnOidHandler) {
            return TRUE;
        }
    }

    return FALSE;
}

#if CFG_ENABLE_FW_DOWNLOAD
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to download FW image.
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanImageSectionDownload(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4DestAddr,
    IN UINT_32      u4ImgSecSize,
    IN PUINT_8      pucImgSecBuf
)
{
#if defined(MT6620)

    P_CMD_INFO_T prCmdInfo;
    P_INIT_HIF_TX_HEADER_T prInitHifTxHeader;
    P_INIT_CMD_DOWNLOAD_BUF prInitCmdDownloadBuf;
    UINT_8 ucTC, ucCmdSeqNum;
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;

    ASSERT(prAdapter);
    ASSERT(pucImgSecBuf);
    ASSERT(u4ImgSecSize <= CMD_PKT_SIZE_FOR_IMAGE);

    DEBUGFUNC("wlanImageSectionDownload");

    if (u4ImgSecSize == 0) {
        return WLAN_STATUS_SUCCESS;
    }

    // 1. Allocate CMD Info Packet and its Buffer.
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter,
                                      sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_DOWNLOAD_BUF) + CMD_PKT_SIZE_FOR_IMAGE);

    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    prCmdInfo->u2InfoBufLen =
        sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_DOWNLOAD_BUF) + CMD_PKT_SIZE_FOR_IMAGE;

    // 2. Use TC0's resource to download image. (only TC0 is allowed)
    ucTC = TC0_INDEX;

    // 3. increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // 4. Setup common CMD Info Packet
    prInitHifTxHeader = (P_INIT_HIF_TX_HEADER_T)(prCmdInfo->pucInfoBuffer);
    prInitHifTxHeader->rInitWifiCmd.ucCID = INIT_CMD_ID_DOWNLOAD_BUF;
    prInitHifTxHeader->rInitWifiCmd.ucSeqNum = ucCmdSeqNum;

    // 5. Setup CMD_DOWNLOAD_BUF
    prInitCmdDownloadBuf = (P_INIT_CMD_DOWNLOAD_BUF)(prInitHifTxHeader->rInitWifiCmd.aucBuffer);
    prInitCmdDownloadBuf->u4Address = u4DestAddr;
    prInitCmdDownloadBuf->u4Length = u4ImgSecSize;
    prInitCmdDownloadBuf->u4CRC32 = wlanCRC32(pucImgSecBuf, u4ImgSecSize);
    prInitCmdDownloadBuf->u4DataMode = 0
#if CFG_ENABLE_FW_DOWNLOAD_ACK
                                       | DOWNLOAD_BUF_ACK_OPTION // ACK needed
#endif
#if CFG_ENABLE_FW_ENCRYPTION
                                       | DOWNLOAD_BUF_ENCRYPTION_MODE
#endif
                                       ;
    kalMemCopy(prInitCmdDownloadBuf->aucBuffer, pucImgSecBuf, u4ImgSecSize);

    // 6. Send FW_Download command
    while (1) {
        // 6.1 Acquire TX Resource
        if (nicTxAcquireResource(prAdapter, ucTC) == WLAN_STATUS_RESOURCES) {
            if (nicTxPollingResource(prAdapter, ucTC) != WLAN_STATUS_SUCCESS) {
                u4Status = WLAN_STATUS_FAILURE;
                DBGLOG(INIT, ERROR, ("Fail to get TX resource return within timeout\n"));
                break;
            } else {
                continue;
            }
        }

        // 6.2 Send CMD Info Packet
        if (nicTxInitCmd(prAdapter, prCmdInfo, ucTC) != WLAN_STATUS_SUCCESS) {
            u4Status = WLAN_STATUS_FAILURE;
            DBGLOG(INIT, ERROR, ("Fail to transmit image download command\n"));
        }

        break;
    };

#if CFG_ENABLE_FW_DOWNLOAD_ACK
    // 7. Wait for INIT_EVENT_ID_CMD_RESULT
    u4Status = wlanImageSectionDownloadStatus(prAdapter, ucCmdSeqNum);
#endif

    // 8. Free CMD Info Packet.
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return u4Status;

#elif defined(MT5931)

    UINT_32 i, u4Value;
    P_HIF_HW_TX_HEADER_T prHifTxHeader;

    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
#if !defined(__WIFI_SLIM__)
    UINT_32 u4TotalLen;
#if !__WIFI_HIF_SDIO__
    PUINT_8 pucDmaBuf;
    UINT_32 u4DmaLen;
    UINT_32 u4DmaOffset;
#endif
#endif
    ASSERT(prAdapter);
    ASSERT(pucImgSecBuf);
    ASSERT(u4ImgSecSize <= CMD_PKT_SIZE_FOR_IMAGE);

    DEBUGFUNC("wlanImageSectionDownload");
    //DBGLOG(INIT, TRACE, ("Destination: 0x%08x / Length: 0x%08x\n", u4DestAddr, u4ImgSecSize));
    MT5931_NEW_TRACE(MT5931_INFO_434,  TRACE_INIT, "Destination: 0x%08x / Length: 0x%08x", u4DestAddr, u4ImgSecSize);

    if (u4ImgSecSize == 0) {
        return WLAN_STATUS_SUCCESS;
    }

    // 1. Use TX coalescing buffer
#if !defined(__WIFI_SLIM__)
    prHifTxHeader = (P_HIF_HW_TX_HEADER_T)(&wndrv_cmd_temp_buf[0]);
#else
    prHifTxHeader = (P_HIF_HW_TX_HEADER_T)prAdapter->pucCoalescingBufCached;
#endif
    // 2. Setup HIF_TX_HEADER
    //prHifTxHeader->u2TxByteCount = (UINT_16)(ALIGN_4(sizeof(HIF_HW_TX_HEADER_T) + u4ImgSecSize));
    prHifTxHeader->u2TxByteCount = (UINT_16)(ALIGN_4(HIF_HW_TX_HDR_SIZE + u4ImgSecSize));
    prHifTxHeader->ucEtherTypeOffset = 0;
    prHifTxHeader->ucCSflags = 0;

    // 3. Copy payload
    kalMemCopy(prHifTxHeader->aucBuffer, pucImgSecBuf, u4ImgSecSize);

    // 3.1 add 4-bytes zero tail
    //kalMemZero(&(prHifTxHeader->aucBuffer[ALIGN_4(u4ImgSecSize)]), sizeof(HIF_HW_TX_HEADER_T));
    kalMemZero(&(prHifTxHeader->aucBuffer[ALIGN_4(u4ImgSecSize)]), HIF_HW_TX_HDR_SIZE);

    // 4. Poll til FWDL_RDY = 1
    i = 0;
    while (1) {
        HAL_MCR_RD(prAdapter, MCR_FWDLSR, &u4Value);

        if (u4Value & FWDLSR_FWDL_RDY) {
            //DBGLOG(INIT, TRACE, ("FWDL_RDY detected\n"));
            MT5931_NEW_TRACE(MT5931_INFO_435,  TRACE_INIT, "FWDL_RDY detected", u4DestAddr, u4ImgSecSize);
            break;
        } else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                   || fgIsBusAccessFailed == TRUE) {
            u4Status = WLAN_STATUS_FAILURE;
            break;
        } else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
            //DBGLOG(INIT, ERROR, ("Waiting for FWDL_RDY: Timeout (0x%08X)\n", u4Value));
            MT5931_NEW_TRACE(MT5931_INFO_436,  TRACE_INIT, "Waiting for FWDL_RDY: Timeout (0x%x)", u4Value);
            u4Status = WLAN_STATUS_FAILURE;
            break;
        } else {
            i++;
            kalMsleep(10);
        }
    }

    // 5. Send firmware
#if !defined(__WIFI_SLIM__)
    HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(prHifTxHeader->u2TxByteCount));
    if (Wifi_Query_Dynamic_Switch_Cachability()) {  //NOTE(Nelson): dest buffer address & size align for dma
        u4TotalLen = ALIGN_4(prHifTxHeader->u2TxByteCount);
#if __WIFI_HIF_SDIO__
        HAL_PORT_WR(prAdapter,
                    MCR_FWDLDR,
                    u4TotalLen,
                    (PUINT_8)prHifTxHeader,
                    prAdapter->u4CoalescingBufCachedSize);
#else
        if (u4TotalLen > DMA_THRESHOLD) {
            //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX_DMA: len %d buf %x", u4TotalLen, pucDmaBuf);
            kalDevPortWriteAddr(MCR_FWDLDR);

            pucDmaBuf = (PUINT_8)prHifTxHeader;
            u4DmaLen = u4TotalLen;
            u4DmaOffset = CPU_CACHE_LINE_SIZE - ((UINT_32)pucDmaBuf & CPU_CACHE_LINE_SIZE_MASK);

            //1. 32byte non-ailgn block
            if (u4DmaOffset != 0) {
                //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX1: Buf %x len %d", pucDmaBuf, u4DmaOffset);
                kalDevPortWriteData(MCR_FWDLDR, u4DmaOffset, pucDmaBuf);
                pucDmaBuf += u4DmaOffset;
                u4DmaLen -= u4DmaOffset;
            }

            //2. 32byte-align block
            u4DmaOffset = u4DmaLen & CPU_CACHE_LINE_SIZE_MASK;
            u4DmaLen -= u4DmaOffset;

            //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX2: Buf %x len %d", pucDmaBuf, u4DmaLen);
            kalDevPortWriteData(MCR_FWDLDR, u4DmaLen, pucDmaBuf);

            //3. remain 32byte non-align block
            pucDmaBuf += u4DmaLen;
            u4DmaLen = u4DmaOffset;

            if (u4DmaLen != 0) {
                //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX3: Buf %x len %d", pucDmaBuf, u4DmaLen);
                kalDevPortWriteData(MCR_FWDLDR, u4DmaLen, pucDmaBuf);
            }
        } else {
            HAL_PORT_WR(prAdapter,
                        MCR_FWDLDR,
                        u4TotalLen,
                        (PUINT_8)prHifTxHeader,
                        prAdapter->u4CoalescingBufCachedSize);
        }
#endif
    } else {
        HAL_PORT_WR(prAdapter,
                    MCR_FWDLDR,
                    ALIGN_4(prHifTxHeader->u2TxByteCount),
                    (PUINT_8)prHifTxHeader,
                    prAdapter->u4CoalescingBufCachedSize);
    }
    HAL_MCR_WR(prAdapter, WEHTCR, 4);
#else
    HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(prHifTxHeader->u2TxByteCount));
    HAL_PORT_WR(prAdapter,
                MCR_FWDLDR,
                prHifTxHeader->u2TxByteCount,
                (PUINT_8)prHifTxHeader,
                prAdapter->u4CoalescingBufCachedSize);
    HAL_MCR_WR(prAdapter, WEHTCR, 4);
#endif
    return u4Status;

#endif
}

/* FW DL stress test */
WLAN_STATUS
wlanImageSectionDownload_Temp(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4DestAddr,
    IN UINT_32      u4ImgSecSize,
    IN PUINT_8      pucImgSecBuf
)
{
    //UINT_32 i, u4Value;
    P_HIF_HW_TX_HEADER_T prHifTxHeader;

    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;

    //PUINT_8 pucDmaBuf;
    UINT_32 u4TotalLen;
    //UINT_32 u4DmaLen;
    //UINT_32 u4DmaOffset;

    ASSERT(prAdapter);
    ASSERT(pucImgSecBuf);
    ASSERT(u4ImgSecSize <= CMD_PKT_SIZE_FOR_IMAGE);

    DEBUGFUNC("wlanImageSectionDownload");
    //DBGLOG(INIT, TRACE, ("Destination: 0x%08x / Length: 0x%08x\n", u4DestAddr, u4ImgSecSize));
    MT5931_NEW_TRACE(MT5931_INFO_437,  TRACE_INIT, "Destination: 0x%08x / Length: 0x%08x", u4DestAddr, u4ImgSecSize);

    if (u4ImgSecSize == 0) {
        return WLAN_STATUS_SUCCESS;
    }

    // 1. Use TX coalescing buffer
#if !defined(__WIFI_SLIM__)
    prHifTxHeader = (P_HIF_HW_TX_HEADER_T)(&wndrv_cmd_temp_buf[0]);
#else
    prHifTxHeader = (P_HIF_HW_TX_HEADER_T)prAdapter->pucCoalescingBufCached;
#endif

    // 2. Setup HIF_TX_HEADER
    //prHifTxHeader->u2TxByteCount = (UINT_16)(ALIGN_4(sizeof(HIF_HW_TX_HEADER_T) + u4ImgSecSize));
    prHifTxHeader->u2TxByteCount = (UINT_16)(ALIGN_4(HIF_HW_TX_HDR_SIZE + u4ImgSecSize));
    prHifTxHeader->ucEtherTypeOffset = 0;
    prHifTxHeader->ucCSflags = 0;

    // 3. Copy payload
    kalMemCopy(prHifTxHeader->aucBuffer, pucImgSecBuf, u4ImgSecSize);

    // 3.1 add 4-bytes zero tail
    //kalMemZero(&(prHifTxHeader->aucBuffer[ALIGN_4(u4ImgSecSize)]), sizeof(HIF_HW_TX_HEADER_T));
    kalMemZero(&(prHifTxHeader->aucBuffer[ALIGN_4(u4ImgSecSize)]), HIF_HW_TX_HDR_SIZE);

    // 5. Send firmware
    HAL_MCR_WR(prAdapter, WEHTCR, ALIGN_4(prHifTxHeader->u2TxByteCount));
    if (Wifi_Query_Dynamic_Switch_Cachability()) {  //NOTE(Nelson): dest buffer address & size align for dma
        u4TotalLen = ALIGN_4(prHifTxHeader->u2TxByteCount);
#if __WIFI_HIF_SDIO__

        HAL_PORT_WR(prAdapter,
                    MCR_FWDLDR,
                    u4TotalLen,
                    (PUINT_8)prHifTxHeader,
                    prAdapter->u4CoalescingBufCachedSize);

#else
        if (u4TotalLen > DMA_THRESHOLD) {
            //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX_DMA: len %d buf %x", u4TotalLen, pucDmaBuf);
            kalDevPortWriteAddr(MCR_FWDLDR);

            pucDmaBuf = (PUINT_8)prHifTxHeader;
            u4DmaLen = u4TotalLen;
            u4DmaOffset = CPU_CACHE_LINE_SIZE - ((UINT_32)pucDmaBuf & CPU_CACHE_LINE_SIZE_MASK);

            //1. 32byte non-ailgn block
            if (u4DmaOffset != 0) {
                //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX1: Buf %x len %d", pucDmaBuf, u4DmaOffset);
                kalDevPortWriteData(MCR_FWDLDR, u4DmaOffset, pucDmaBuf);
                pucDmaBuf += u4DmaOffset;
                u4DmaLen -= u4DmaOffset;
            }

            //2. 32byte-align block
            u4DmaOffset = u4DmaLen & CPU_CACHE_LINE_SIZE_MASK;
            u4DmaLen -= u4DmaOffset;

            //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX2: Buf %x len %d", pucDmaBuf, u4DmaLen);
            kalDevPortWriteData(MCR_FWDLDR, u4DmaLen, pucDmaBuf);

            //3. remain 32byte non-align block
            pucDmaBuf += u4DmaLen;
            u4DmaLen = u4DmaOffset;

            if (u4DmaLen != 0) {
                //kal_wap_trace(MAP_MOD_WNDRV, TRACE_GROUP_3, "TX3: Buf %x len %d", pucDmaBuf, u4DmaLen);
                kalDevPortWriteData(MCR_FWDLDR, u4DmaLen, pucDmaBuf);
            }
        } else {
            HAL_PORT_WR(prAdapter,
                        MCR_FWDLDR,
                        u4TotalLen,
                        (PUINT_8)prHifTxHeader,
                        prAdapter->u4CoalescingBufCachedSize);
        }
#endif
    } else {
        HAL_PORT_WR(prAdapter,
                    MCR_FWDLDR,
                    ALIGN_4(prHifTxHeader->u2TxByteCount),
                    (PUINT_8)prHifTxHeader,
                    prAdapter->u4CoalescingBufCachedSize);
    }
    HAL_MCR_WR(prAdapter, WEHTCR, 4);

    return u4Status;
}

#if !CFG_ENABLE_FW_DOWNLOAD_ACK
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to confirm previously firmware download is done without error
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanImageQueryStatus(
    IN P_ADAPTER_T  prAdapter
)
{
    P_CMD_INFO_T prCmdInfo;
    P_INIT_HIF_TX_HEADER_T prInitHifTxHeader;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    UINT_32 u4RxPktLength;
    P_INIT_HIF_RX_HEADER_T prInitHifRxHeader;
    P_INIT_EVENT_PENDING_ERROR prEventPendingError;
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
    UINT_8 ucTC, ucCmdSeqNum;

    ASSERT(prAdapter);

    DEBUGFUNC("wlanImageQueryStatus");

    // 1. Allocate CMD Info Packet and it Buffer.
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, sizeof(INIT_HIF_TX_HEADER_T));

    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    kalMemZero(prCmdInfo, sizeof(INIT_HIF_TX_HEADER_T));
    prCmdInfo->u2InfoBufLen = sizeof(INIT_HIF_TX_HEADER_T);

    // 2. Use TC0's resource to download image. (only TC0 is allowed)
    ucTC = TC0_INDEX;

    // 3. increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // 4. Setup common CMD Info Packet
    prInitHifTxHeader = (P_INIT_HIF_TX_HEADER_T)(prCmdInfo->pucInfoBuffer);
    prInitHifTxHeader->rInitWifiCmd.ucCID = INIT_CMD_ID_QUERY_PENDING_ERROR;
    prInitHifTxHeader->rInitWifiCmd.ucSeqNum = ucCmdSeqNum;

    // 5. Send command
    while (1) {
        // 5.1 Acquire TX Resource
        if (nicTxAcquireResource(prAdapter, ucTC) == WLAN_STATUS_RESOURCES) {
            if (nicTxPollingResource(prAdapter, ucTC) != WLAN_STATUS_SUCCESS) {
                u4Status = WLAN_STATUS_FAILURE;
                DBGLOG(INIT, ERROR, ("Fail to get TX resource return within timeout\n"));
                break;
            } else {
                continue;
            }
        }

        // 5.2 Send CMD Info Packet
        if (nicTxInitCmd(prAdapter, prCmdInfo, ucTC) != WLAN_STATUS_SUCCESS) {
            u4Status = WLAN_STATUS_FAILURE;
            DBGLOG(INIT, ERROR, ("Fail to transmit image download command\n"));
        }

        break;
    };

    // 6. Wait for INIT_EVENT_ID_PENDING_ERROR
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    do {
        if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            u4Status = WLAN_STATUS_FAILURE;
        } else if (nicRxWaitResponse(prAdapter,
                                     0,
                                     aucBuffer,
                                     sizeof(INIT_HIF_RX_HEADER_T) + sizeof(INIT_EVENT_PENDING_ERROR),
                                     &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
            u4Status = WLAN_STATUS_FAILURE;
        } else {
            prInitHifRxHeader = (P_INIT_HIF_RX_HEADER_T) aucBuffer;

            // EID / SeqNum check
            if (prInitHifRxHeader->rInitWifiEvent.ucEID != INIT_EVENT_ID_PENDING_ERROR) {
                u4Status = WLAN_STATUS_FAILURE;
            } else if (prInitHifRxHeader->rInitWifiEvent.ucSeqNum != ucCmdSeqNum) {
                u4Status = WLAN_STATUS_FAILURE;
            } else {
                prEventPendingError = (P_INIT_EVENT_PENDING_ERROR)(prInitHifRxHeader->rInitWifiEvent.aucBuffer);
                if (prEventPendingError->ucStatus != 0) { // 0 for download success
                    u4Status = WLAN_STATUS_FAILURE;
                } else {
                    u4Status = WLAN_STATUS_SUCCESS;
                }
            }
        }
    } while (FALSE);

    // 7. Free CMD Info Packet.
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return u4Status;
}


#else
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to confirm the status of
*        previously downloaded firmware scatter
*
* @param prAdapter      Pointer to the Adapter structure.
*        ucCmdSeqNum    Sequence number of previous firmware scatter
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanImageSectionDownloadStatus(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucCmdSeqNum
)
{
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_INIT_HIF_RX_HEADER_T prInitHifRxHeader;
    P_INIT_EVENT_CMD_RESULT prEventCmdResult;
    UINT_32 u4RxPktLength;
    WLAN_STATUS u4Status;

    ASSERT(prAdapter);
    memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    do {
        if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            u4Status = WLAN_STATUS_FAILURE;
        } else if (nicRxWaitResponse(prAdapter,
                                     0,
                                     aucBuffer,
                                     sizeof(INIT_HIF_RX_HEADER_T) + sizeof(INIT_EVENT_CMD_RESULT),
                                     &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
            u4Status = WLAN_STATUS_FAILURE;
        } else {
            prInitHifRxHeader = (P_INIT_HIF_RX_HEADER_T) aucBuffer;

            // EID / SeqNum check
            if (prInitHifRxHeader->rInitWifiEvent.ucEID != INIT_EVENT_ID_CMD_RESULT) {
                u4Status = WLAN_STATUS_FAILURE;
            } else if (prInitHifRxHeader->rInitWifiEvent.ucSeqNum != ucCmdSeqNum) {
                u4Status = WLAN_STATUS_FAILURE;
            } else {
                prEventCmdResult = (P_INIT_EVENT_CMD_RESULT)(prInitHifRxHeader->rInitWifiEvent.aucBuffer);
                if (prEventCmdResult->ucStatus != 0) { // 0 for download success
                    u4Status = WLAN_STATUS_FAILURE;
                } else {
                    u4Status = WLAN_STATUS_SUCCESS;
                }
            }
        }
    } while (FALSE);

    return u4Status;
}


#endif
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to start FW normal operation.
*
* @param prAdapter      Pointer to the Adapter structure.
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanConfigWifiFunc(
    IN P_ADAPTER_T  prAdapter,
    IN BOOLEAN      fgEnable,
    IN UINT_32      u4StartAddress
)
{
    P_CMD_INFO_T prCmdInfo;
    P_INIT_HIF_TX_HEADER_T prInitHifTxHeader;
    P_INIT_CMD_WIFI_START prInitCmdWifiStart;
    UINT_8 ucTC, ucCmdSeqNum;
    WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;

    ASSERT(prAdapter);

    DEBUGFUNC("wlanConfigWifiFunc");

    // 1. Allocate CMD Info Packet and its Buffer.
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter,
                                      sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_WIFI_START));

    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    //kalMemZero(prCmdInfo, sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_WIFI_START));
    prCmdInfo->u2InfoBufLen =
        sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_WIFI_START);

    // 2. Always use TC0
    ucTC = TC0_INDEX;

    // 3. increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // 4. Setup common CMD Info Packet
    prInitHifTxHeader = (P_INIT_HIF_TX_HEADER_T)(prCmdInfo->pucInfoBuffer);
    prInitHifTxHeader->rInitWifiCmd.ucCID = INIT_CMD_ID_WIFI_START;
    prInitHifTxHeader->rInitWifiCmd.ucSeqNum = ucCmdSeqNum;

    prInitCmdWifiStart = (P_INIT_CMD_WIFI_START)(prInitHifTxHeader->rInitWifiCmd.aucBuffer);
    prInitCmdWifiStart->u4Override = (fgEnable == TRUE ? 1 : 0);
    prInitCmdWifiStart->u4Address = u4StartAddress;

    // 5. Seend WIFI start command
    while (1) {
        // 5.1 Acquire TX Resource
        if (nicTxAcquireResource(prAdapter, ucTC) == WLAN_STATUS_RESOURCES) {
            if (nicTxPollingResource(prAdapter, ucTC) != WLAN_STATUS_SUCCESS) {
                u4Status = WLAN_STATUS_FAILURE;
                DBGLOG(INIT, ERROR, ("Fail to get TX resource return within timeout\n"));
                break;
            } else {
                continue;
            }
        }

        // 5.2 Send CMD Info Packet
        if (nicTxInitCmd(prAdapter, prCmdInfo, ucTC) != WLAN_STATUS_SUCCESS) {
            u4Status = WLAN_STATUS_FAILURE;
            DBGLOG(INIT, ERROR, ("Fail to transmit WIFI start command\n"));
        }

        break;
    };

    // 6. Free CMD Info Packet.
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return u4Status;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is used to generate CRC32 checksum
*
* @param buf Pointer to the data.
* @param len data length
*
* @return crc32 value
*/
/*----------------------------------------------------------------------------*/
UINT_32 wlanCRC32(
    PUINT_8 buf,
    UINT_32 len)
{
    UINT_32 i, crc32 = 0xFFFFFFFF;
    const UINT_32 crc32_ccitt_table[256] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
        0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
        0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
        0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
        0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
        0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
        0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
        0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
        0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
        0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
        0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
        0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
        0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
        0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
        0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
        0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
        0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
        0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
        0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
        0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
        0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
        0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
        0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
        0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
        0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
        0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
        0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
        0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
        0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
        0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
        0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
        0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
        0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
        0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
        0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
        0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
        0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
        0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
        0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
        0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
        0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
        0x2d02ef8d
    };

    for (i = 0; i < len; i++) {
        crc32 = crc32_ccitt_table[(crc32 ^ buf[i]) & 0xff] ^ (crc32 >> 8);
    }

    return (~crc32);
}
#endif


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to process queued RX packets
*
* @param prAdapter          Pointer to the Adapter structure.
*        prSwRfbListHead    Pointer to head of RX packets link list
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanProcessQueuedSwRfb(
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfbListHead
)
{
    P_SW_RFB_T prSwRfb, prNextSwRfb;
    //P_TX_CTRL_T prTxCtrl;
#if CFG_HIF_RX_STARVATION_WARNING
    P_RX_CTRL_T prRxCtrl;
#endif

    ASSERT(prAdapter);
    ASSERT(prSwRfbListHead);

    //prTxCtrl = &prAdapter->rTxCtrl;
    //prRxCtrl = &prAdapter->rRxCtrl;

    prSwRfb = prSwRfbListHead;

    do {
        // save next first
        prNextSwRfb = (P_SW_RFB_T)QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T)prSwRfb);

        switch (prSwRfb->eDst) {
            case RX_PKT_DESTINATION_HOST:
                nicRxProcessPktWithoutReorder(prAdapter, prSwRfb);
                break;

            case RX_PKT_DESTINATION_FORWARD:
                nicRxProcessForwardPkt(prAdapter, prSwRfb);
                break;

            case RX_PKT_DESTINATION_HOST_WITH_FORWARD:
                nicRxProcessGOBroadcastPkt(prAdapter, prSwRfb);
                break;

            case RX_PKT_DESTINATION_NULL:
                nicRxReturnRFB(prAdapter, prSwRfb);
                break;

            default:
                break;
        }

#if CFG_HIF_RX_STARVATION_WARNING
        prRxCtrl = &prAdapter->rRxCtrl;
        prRxCtrl->u4DequeuedCnt++;
#endif
        prSwRfb = prNextSwRfb;
    } while (prSwRfb);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to purge queued TX packets
*        by indicating failure to OS and returned to free list
*
* @param prAdapter          Pointer to the Adapter structure.
*        prMsduInfoListHead Pointer to head of TX packets link list
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanProcessQueuedMsduInfo(
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfoListHead
)
{
    ASSERT(prAdapter);
    ASSERT(prMsduInfoListHead);

    nicTxFreeMsduInfoPacket(prAdapter, prMsduInfoListHead);
    nicTxReturnMsduInfo(prAdapter, prMsduInfoListHead);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to check if the OID handler needs timeout
*
* @param prAdapter          Pointer to the Adapter structure.
*        pfnOidHandler      Pointer to the OID handler
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanoidTimeoutCheck(
    IN P_ADAPTER_T prAdapter,
    IN PFN_OID_HANDLER_FUNC pfnOidHandler
)
{
    PFN_OID_HANDLER_FUNC *apfnOidHandlerWOTimeoutCheck;
    UINT_32 i;
    UINT_32 u4NumOfElem;

    apfnOidHandlerWOTimeoutCheck = apfnOidWOTimeoutCheck;
    u4NumOfElem = sizeof(apfnOidWOTimeoutCheck) / sizeof(PFN_OID_HANDLER_FUNC);

    for (i = 0; i < u4NumOfElem; i++) {
        if (apfnOidHandlerWOTimeoutCheck[i] == pfnOidHandler) {
            return FALSE;
        }
    }

    // set timer if need timeout check
    cnmTimerStartTimer(prAdapter,
                       &(prAdapter->rOidTimeoutTimer),
                       2000);

    return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to clear any pending OID timeout check
*
* @param prAdapter          Pointer to the Adapter structure.
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanoidClearTimeoutCheck(
    IN P_ADAPTER_T prAdapter
)
{
    ASSERT(prAdapter);

    cnmTimerStopTimer(prAdapter, &(prAdapter->rOidTimeoutTimer));
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to set up the MCUSYS's OSC stable time
*
* @param prAdapter          Pointer to the Adapter structure.
*
* @return none
*/
/*----------------------------------------------------------------------------*/

#if CFG_SUPPORT_OSC_SETTING && defined(MT5931)
WLAN_STATUS
wlanSetMcuOscStableTime(
    IN P_ADAPTER_T      prAdapter,
    IN UINT_16          u2OscStableTime
)
{
    UINT_8                  ucCmdSeqNum = 0;
    P_CMD_INFO_T            prCmdInfo = NULL;
    P_WIFI_CMD_T            prWifiCmd = NULL;
    P_CMD_MCU_LP_PARAM_T    prMcuSetOscCmd = NULL;
    WLAN_STATUS             status = WLAN_STATUS_SUCCESS;

    ASSERT(prAdapter);

    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter,
                                      CMD_HDR_SIZE + sizeof(CMD_MCU_LP_PARAM_T));

    if (!prCmdInfo) {
        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "Allocate CMD_INFO_T ==> FAILED.\n");
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // compose CMD_MCU_LP_PARAM_T cmd pkt
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(CMD_MCU_LP_PARAM_T);
    prCmdInfo->pfCmdDoneHandler = NULL;
    prCmdInfo->pfCmdTimeoutHandler = NULL;
    prCmdInfo->fgIsOid = FALSE;
    prCmdInfo->ucCID = CMD_ID_SET_OSC;
    prCmdInfo->fgSetQuery = TRUE;
    prCmdInfo->fgNeedResp = FALSE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_MCU_LP_PARAM_T);

    // Setup WIFI_CMD_T
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    // configure CMD_MCU_LP_PARAM_T
    prMcuSetOscCmd = (P_CMD_MCU_LP_PARAM_T)(prWifiCmd->aucBuffer);
    prMcuSetOscCmd->u2OscStableTime = u2OscStableTime;

    status = wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

    return status;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to update network address in firmware domain
*
* @param prAdapter          Pointer to the Adapter structure.
*
* @return WLAN_STATUS_FAILURE   The request could not be processed
*         WLAN_STATUS_PENDING   The request has been queued for later processing
*         WLAN_STATUS_SUCCESS   The request has been processed
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanUpdateNetworkAddress(
    IN P_ADAPTER_T prAdapter
)
{
    PARAM_MAC_ADDRESS rZeroMacAddr;
    PARAM_MAC_ADDRESS rMacAddr;
    UINT_8 ucCmdSeqNum;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    P_CMD_BASIC_CONFIG prCmdBasicConfig;
    UINT_32 u4SysTime;

    ASSERT(prAdapter);
    DBGPRINTF("wlanUpdateNetworkAddress\n");

    kalMemSet(rZeroMacAddr, 0x00, MAC_ADDR_LEN);

    if (kalRetrieveNetworkAddress(prAdapter->prGlueInfo, &rMacAddr) == FALSE
            || IS_BMCAST_MAC_ADDR(rMacAddr)
            || EQUAL_MAC_ADDR(rZeroMacAddr, rMacAddr)) {
        // eFUSE has a valid address, don't do anything
        if (prAdapter->fgIsEmbbededMacAddrValid == TRUE) {
            return WLAN_STATUS_SUCCESS;
        } else {
            // dynamic generate
            u4SysTime = (UINT_32) kalGetTimeTick();

            rMacAddr[0] = 0x00;
            rMacAddr[1] = 0x08;
            rMacAddr[2] = 0x22;

            kalMemCopy(&rMacAddr[3], &u4SysTime, 3);
        }
    }

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "wlanUpdateNetworkAddress:["MACSTR"]",MAC2STR(rMacAddr));
    printc("[DBG]wlanUpdateNetworkAddress:["MACSTR"]",MAC2STR(rMacAddr));
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter,
            CMD_HDR_SIZE + sizeof(CMD_BASIC_CONFIG));

    if (!prCmdInfo) {
        MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"Allocate CMD_INFO_T ==> FAILED.\n");
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // compose CMD_BUILD_CONNECTION cmd pkt
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(CMD_BASIC_CONFIG);
    prCmdInfo->pfCmdDoneHandler = NULL;
    prCmdInfo->pfCmdTimeoutHandler = NULL;
    prCmdInfo->fgIsOid = FALSE;
    prCmdInfo->ucCID = CMD_ID_BASIC_CONFIG;
    prCmdInfo->fgSetQuery = TRUE;
    prCmdInfo->fgNeedResp = FALSE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_BASIC_CONFIG);

    // Setup WIFI_CMD_T
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    // configure CMD_BASIC_CONFIG
    prCmdBasicConfig = (P_CMD_BASIC_CONFIG)(prWifiCmd->aucBuffer);
    kalMemCopy(&(prCmdBasicConfig->rMyMacAddr), &rMacAddr, PARAM_MAC_ADDR_LEN);
    prCmdBasicConfig->ucNative80211 = 0;
    prCmdBasicConfig->rCsumOffload.u2RxChecksum = 0;
    prCmdBasicConfig->rCsumOffload.u2TxChecksum = 0;

#if CFG_TCP_IP_CHKSUM_OFFLOAD
    if(prAdapter->u4CSUMFlags & CSUM_OFFLOAD_EN_TX_TCP)
        prCmdBasicConfig->rCsumOffload.u2TxChecksum |= BIT(2);

    if(prAdapter->u4CSUMFlags & CSUM_OFFLOAD_EN_TX_UDP)
        prCmdBasicConfig->rCsumOffload.u2TxChecksum |= BIT(1);

    if(prAdapter->u4CSUMFlags & CSUM_OFFLOAD_EN_TX_IP)
        prCmdBasicConfig->rCsumOffload.u2TxChecksum |= BIT(0);

    if(prAdapter->u4CSUMFlags & CSUM_OFFLOAD_EN_RX_TCP)
        prCmdBasicConfig->rCsumOffload.u2RxChecksum |= BIT(2);

    if(prAdapter->u4CSUMFlags & CSUM_OFFLOAD_EN_RX_UDP)
        prCmdBasicConfig->rCsumOffload.u2RxChecksum |= BIT(1);

    if(prAdapter->u4CSUMFlags & (CSUM_OFFLOAD_EN_RX_IPv4 | CSUM_OFFLOAD_EN_RX_IPv6))
        prCmdBasicConfig->rCsumOffload.u2RxChecksum |= BIT(0);
#endif

    if(wlanSendCommand(prAdapter, prCmdInfo) == WLAN_STATUS_RESOURCES) {
        prCmdInfo->pfCmdDoneHandler = nicCmdEventQueryAddress;
        kalEnqueueCommand(prAdapter->prGlueInfo, (P_QUE_ENTRY_T)prCmdInfo);

        return WLAN_STATUS_PENDING;
    }
    else {
        nicCmdEventQueryAddress(prAdapter, prCmdInfo, (PUINT_8)prCmdBasicConfig);
        cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

        return WLAN_STATUS_SUCCESS;
    }
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to check if the device is in RF test mode
*
* @param pfnOidHandler      Pointer to the OID handler
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanQueryTestMode(
    IN P_ADAPTER_T          prAdapter
    )
{
    ASSERT(prAdapter);

    return prAdapter->fgTestMode;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to identify 802.1x and Bluetooth-over-Wi-Fi
*        security frames, and queued into command queue for strict ordering
*        due to 802.1x frames before add-key OIDs are not to be encrypted
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param prPacket       Pointer of native packet
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanProcessSecurityFrame(
    IN P_ADAPTER_T      prAdapter,
    IN P_NATIVE_PACKET  prPacket
    )
{
    UINT_8          ucPriorityParam;
    UINT_8          aucEthDestAddr[PARAM_MAC_ADDR_LEN];
    BOOLEAN         fgIs1x = FALSE;
    BOOLEAN         fgIsPAL = FALSE;
    UINT_32         u4PacketLen;
    ULONG           u4SysTime;
    UINT_8          ucNetworkType;
    P_CMD_INFO_T    prCmdInfo;


    ASSERT(prAdapter);
    ASSERT(prPacket);

    if (kalQoSFrameClassifierAndPacketInfo(prAdapter->prGlueInfo,
                prPacket,
                &ucPriorityParam,
                &u4PacketLen,
                aucEthDestAddr,
                &fgIs1x,
                &fgIsPAL,
                &ucNetworkType) == TRUE) {
        if(fgIs1x == FALSE) {
            return FALSE;
        }
        else {
            KAL_SPIN_LOCK_DECLARATION();
            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
            QUEUE_REMOVE_HEAD(&prAdapter->rFreeCmdList, prCmdInfo, P_CMD_INFO_T);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);

            if (prCmdInfo) {
                P_STA_RECORD_T          prStaRec;
                // fill arrival time
                u4SysTime = (OS_SYSTIME)kalGetTimeTick();
                GLUE_SET_PKT_ARRIVAL_TIME(prPacket, u4SysTime);

                kalMemZero(prCmdInfo, sizeof(CMD_INFO_T));

                prCmdInfo->eCmdType             = COMMAND_TYPE_SECURITY_FRAME;
                prCmdInfo->u2InfoBufLen         = (UINT_16)u4PacketLen;
                prCmdInfo->pucInfoBuffer        = NULL;
                prCmdInfo->prPacket             = prPacket;
#if 0
                prCmdInfo->ucStaRecIndex        = qmGetStaRecIdx(prAdapter,
                                                                    aucEthDestAddr,
                                                                    (ENUM_NETWORK_TYPE_INDEX_T)ucNetworkType);
#endif
                prStaRec                        = cnmGetStaRecByAddress(prAdapter,
                                                                        (ENUM_NETWORK_TYPE_INDEX_T)ucNetworkType,
                                                                        aucEthDestAddr);
                if(prStaRec) {
                    prCmdInfo->ucStaRecIndex =  prStaRec->ucIndex;
                }
                else {
                    prCmdInfo->ucStaRecIndex =  STA_REC_INDEX_NOT_FOUND;
                }

                prCmdInfo->eNetworkType         = (ENUM_NETWORK_TYPE_INDEX_T)ucNetworkType;
                prCmdInfo->pfCmdDoneHandler     = wlanSecurityFrameTxDone;
                prCmdInfo->pfCmdTimeoutHandler  = wlanSecurityFrameTxTimeout;
                prCmdInfo->fgIsOid              = FALSE;
                prCmdInfo->fgSetQuery           = TRUE;
                prCmdInfo->fgNeedResp           = FALSE;

                kalEnqueueCommand(prAdapter->prGlueInfo, (P_QUE_ENTRY_T)prCmdInfo);

                return TRUE;
            }
            else {
                ASSERT(0);
                return FALSE;
            }
        }
    }
    else {
        return FALSE;
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when 802.1x or Bluetooth-over-Wi-Fi
*        security frames has been sent to firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param prCmdInfo      Pointer of CMD_INFO_T
* @param pucEventBuf    meaningless, only for API compatibility
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanSecurityFrameTxDone(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    if (prCmdInfo->eNetworkType == NETWORK_TYPE_AIS_INDEX &&
        prAdapter->rWifiVar.rAisSpecificBssInfo.fgCounterMeasure) {
        P_STA_RECORD_T prSta = cnmGetStaRecByIndex(prAdapter, prCmdInfo->ucStaRecIndex);
        if (prSta) {
            kalMsleep(10);
            secFsmEventEapolTxDone(prAdapter, prSta, TX_RESULT_SUCCESS);
        }
    }

    kalSecurityFrameSendComplete(prAdapter->prGlueInfo,
            prCmdInfo->prPacket,
            WLAN_STATUS_SUCCESS);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when 802.1x or Bluetooth-over-Wi-Fi
*        security frames has failed sending to firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param prCmdInfo      Pointer of CMD_INFO_T
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanSecurityFrameTxTimeout(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
    )
{
    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    kalSecurityFrameSendComplete(prAdapter->prGlueInfo,
            prCmdInfo->prPacket,
            WLAN_STATUS_FAILURE);
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called before AIS is starting a new scan
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanClearScanningResult(
    IN P_ADAPTER_T  prAdapter
    )
{
    BOOLEAN fgKeepCurrOne = FALSE;
    UINT_32 i;

    ASSERT(prAdapter);
    DEBUGFUNC("wlanClearScanningResult()");
    // clear scanning result
    if(kalGetMediaStateIndicated(prAdapter->prGlueInfo) == PARAM_MEDIA_STATE_CONNECTED) {
        for(i = 0 ; i < prAdapter->rWlanInfo.u4ScanResultNum ; i++) {
            if(EQUAL_MAC_ADDR(prAdapter->rWlanInfo.rCurrBssId.arMacAddress,
                        prAdapter->rWlanInfo.arScanResult[i].arMacAddress)) {
                fgKeepCurrOne = TRUE;

                if(i != 0) {
                    // copy structure
                    kalMemCopy(&(prAdapter->rWlanInfo.arScanResult[0]),
                            &(prAdapter->rWlanInfo.arScanResult[i]),
                            OFFSET_OF(PARAM_BSSID_EX_T, aucIEs));
                }

                if(prAdapter->rWlanInfo.arScanResult[i].u4IELength > 0) {
                    if(prAdapter->rWlanInfo.apucScanResultIEs[i] != &(prAdapter->rWlanInfo.aucScanIEBuf[0])) {
                        // move IEs to head
                        kalMemCopy(prAdapter->rWlanInfo.aucScanIEBuf,
                                prAdapter->rWlanInfo.apucScanResultIEs[i],
                                prAdapter->rWlanInfo.arScanResult[i].u4IELength);
                    }

                    // modify IE pointer
                    prAdapter->rWlanInfo.apucScanResultIEs[0] = &(prAdapter->rWlanInfo.aucScanIEBuf[0]);
                }
                else {
                    prAdapter->rWlanInfo.apucScanResultIEs[0] = NULL;
                }

                break;
            }
        }
    }

    if(fgKeepCurrOne == TRUE) {
        prAdapter->rWlanInfo.u4ScanResultNum = 1;
        prAdapter->rWlanInfo.u4ScanIEBufferUsage =
            ALIGN_4(prAdapter->rWlanInfo.arScanResult[0].u4IELength);
    }
    else {
        prAdapter->rWlanInfo.u4ScanResultNum = 0;
        prAdapter->rWlanInfo.u4ScanIEBufferUsage = 0;
    }

    return;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when AIS received a beacon timeout event
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param arBSSID        MAC address of the specified BSS
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanClearBssInScanningResult(
    IN P_ADAPTER_T      prAdapter,
    IN PUINT_8          arBSSID
    )
{
    UINT_32 i, j, u4IELength = 0, u4IEMoveLength;
    PUINT_8 pucIEPtr;

    ASSERT(prAdapter);

    // clear scanning result
    i = 0;
    while(1) {
        if(i >= prAdapter->rWlanInfo.u4ScanResultNum) {
            break;
        }

        if(EQUAL_MAC_ADDR(arBSSID, prAdapter->rWlanInfo.arScanResult[i].arMacAddress)) {
            // backup current IE length
            u4IELength = ALIGN_4(prAdapter->rWlanInfo.arScanResult[i].u4IELength);
            pucIEPtr = prAdapter->rWlanInfo.apucScanResultIEs[i];

            // removed from middle
            for(j = i + 1 ; j < prAdapter->rWlanInfo.u4ScanResultNum ; j++) {
                kalMemCopy(&(prAdapter->rWlanInfo.arScanResult[j-1]),
                        &(prAdapter->rWlanInfo.arScanResult[j]),
                        OFFSET_OF(PARAM_BSSID_EX_T, aucIEs));

                prAdapter->rWlanInfo.apucScanResultIEs[j-1] =
                    prAdapter->rWlanInfo.apucScanResultIEs[j];
            }

            prAdapter->rWlanInfo.u4ScanResultNum--;

            // remove IE buffer if needed := move rest of IE buffer
            if(u4IELength > 0) {
                u4IEMoveLength = prAdapter->rWlanInfo.u4ScanIEBufferUsage -
                    (((UINT_32)pucIEPtr) + u4IELength - ((UINT_32)(&(prAdapter->rWlanInfo.aucScanIEBuf[0]))));

                kalMemCopy(pucIEPtr,
                        (PUINT_8)(((UINT_32)pucIEPtr) + u4IELength),
                        u4IEMoveLength);

                prAdapter->rWlanInfo.u4ScanIEBufferUsage -= u4IELength;

                // correction of pointers to IE buffer
                for(j = 0 ; j < prAdapter->rWlanInfo.u4ScanResultNum ; j++) {
                    if(prAdapter->rWlanInfo.apucScanResultIEs[j] > pucIEPtr) {
                        prAdapter->rWlanInfo.apucScanResultIEs[j] =
                            (PUINT_8)((UINT_32)(prAdapter->rWlanInfo.apucScanResultIEs[j]) - u4IELength);
                    }
                }
            }
        }

        i++;
    }

    return;
}


#if CFG_TEST_WIFI_DIRECT_GO
VOID
wlanEnableP2pFunction (
    IN P_ADAPTER_T prAdapter
    )
{

    P_MSG_P2P_FUNCTION_SWITCH_T prMsgFuncSwitch = (P_MSG_P2P_FUNCTION_SWITCH_T)NULL;

    prMsgFuncSwitch = (P_MSG_P2P_FUNCTION_SWITCH_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_P2P_FUNCTION_SWITCH_T));
    if (!prMsgFuncSwitch) {
        ASSERT(FALSE);
        return;
    }


    prMsgFuncSwitch->rMsgHdr.eMsgId = MID_MNY_P2P_FUN_SWITCH;
    prMsgFuncSwitch->fgIsFuncOn = TRUE;


    mboxSendMsg(prAdapter,
                    MBOX_ID_0,
                    (P_MSG_HDR_T)prMsgFuncSwitch,
                    MSG_SEND_METHOD_BUF);

    return;
}

VOID
wlanEnableATGO (
    IN P_ADAPTER_T prAdapter
    )
{

    P_MSG_P2P_CONNECTION_REQUEST_T prMsgConnReq = (P_MSG_P2P_CONNECTION_REQUEST_T)NULL;
    UINT_8 aucTargetDeviceID[MAC_ADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    prMsgConnReq = (P_MSG_P2P_CONNECTION_REQUEST_T)cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_P2P_CONNECTION_REQUEST_T));
    if (!prMsgConnReq) {
        ASSERT(FALSE);
        return;
    }

    prMsgConnReq->rMsgHdr.eMsgId = MID_MNY_P2P_CONNECTION_REQ;

    /*=====Param Modified for test=====*/
    COPY_MAC_ADDR(prMsgConnReq->aucDeviceID, aucTargetDeviceID);
    prMsgConnReq->fgIsTobeGO = TRUE;
    prMsgConnReq->fgIsPersistentGroup = FALSE;

    /*=====Param Modified for test=====*/

    mboxSendMsg(prAdapter,
                    MBOX_ID_0,
                    (P_MSG_HDR_T)prMsgConnReq,
                    MSG_SEND_METHOD_BUF);

    return;
}
#endif


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to retrieve permanent address from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryPermanentAddress(
    IN P_ADAPTER_T  prAdapter
    )
{
    UINT_8 ucCmdSeqNum;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_HIF_RX_HEADER_T prHifRxHdr;
    P_WIFI_EVENT_T prEvent;
    P_EVENT_BASIC_CONFIG prEventBasicConfig;

    ASSERT(prAdapter);

    DEBUGFUNC("wlanQueryPermanentAddress");

    // 1. Allocate CMD Info Packet and its Buffer
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, CMD_HDR_SIZE + sizeof(CMD_BASIC_CONFIG));
    if (!prCmdInfo) {
        MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"Allocate CMD_INFO_T ==> FAILED.\n");
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // compose CMD_BUILD_CONNECTION cmd pkt
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(CMD_BASIC_CONFIG);
    prCmdInfo->pfCmdDoneHandler = NULL;
    prCmdInfo->fgIsOid = FALSE;
    prCmdInfo->ucCID = CMD_ID_BASIC_CONFIG;
    prCmdInfo->fgSetQuery = FALSE;
    prCmdInfo->fgNeedResp = TRUE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_BASIC_CONFIG);

    // Setup WIFI_CMD_T
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

	memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if(nicRxWaitResponse(prAdapter,
                g_hif_rx_event_port_num,
                aucBuffer,
                sizeof(WIFI_EVENT_T) + sizeof(EVENT_BASIC_CONFIG),
                &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        return WLAN_STATUS_FAILURE;
    }

    // header checking ..
    prHifRxHdr = (P_HIF_RX_HEADER_T)aucBuffer;
    if(prHifRxHdr->u2PacketType != HIF_RX_PKT_TYPE_EVENT) {
        return WLAN_STATUS_FAILURE;
    }

    prEvent = (P_WIFI_EVENT_T)aucBuffer;
    if(prEvent->ucEID != EVENT_ID_BASIC_CONFIG) {
        return WLAN_STATUS_FAILURE;
    }

    prEventBasicConfig = (P_EVENT_BASIC_CONFIG)(prEvent->aucBuffer);

    COPY_MAC_ADDR(prAdapter->rWifiVar.aucPermanentAddress, &(prEventBasicConfig->rMyMacAddr));
    COPY_MAC_ADDR(prAdapter->rWifiVar.aucMacAddress, &(prEventBasicConfig->rMyMacAddr));

    MT5931_TRACE(TRACE_INIT, MT5931_INFO_221, "rWifiVar.aucPermanentAddress: "MACSTR,
        MAC2STR(prAdapter->rWifiVar.aucPermanentAddress));
    MT5931_TRACE(TRACE_INIT, MT5931_INFO_222, "rWifiVar.aucMacAddress: "MACSTR,
        MAC2STR(prAdapter->rWifiVar.aucMacAddress));

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to retrieve NIC capability from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryNicCapability(
    IN P_ADAPTER_T prAdapter
    )
{
    UINT_8 ucCmdSeqNum;
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    UINT_32 u4RxPktLength;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_HIF_RX_HEADER_T prHifRxHdr;
    P_WIFI_EVENT_T prEvent;
    P_EVENT_NIC_CAPABILITY prEventNicCapability;

    ASSERT(prAdapter);

    MT5931_WAP_TRACE(MOD_WNDRV,TRACE_INIT,"wlanQueryNicCapability");

    // 1. Allocate CMD Info Packet and its Buffer
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, CMD_HDR_SIZE + sizeof(EVENT_NIC_CAPABILITY));
    if (!prCmdInfo) {
        MT5931_WAP_TRACE(MOD_WNDRV,TRACE_ERROR,"Allocate CMD_INFO_T ==> FAILED.\n");
        return WLAN_STATUS_FAILURE;
    }

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);

    // compose CMD_BUILD_CONNECTION cmd pkt
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(EVENT_NIC_CAPABILITY);
    prCmdInfo->pfCmdDoneHandler = NULL;
    prCmdInfo->fgIsOid = FALSE;
    prCmdInfo->ucCID = CMD_ID_GET_NIC_CAPABILITY;
    prCmdInfo->fgSetQuery = FALSE;
    prCmdInfo->fgNeedResp = TRUE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = 0;

    // Setup WIFI_CMD_T
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);

	memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if(nicRxWaitResponse(prAdapter,
                g_hif_rx_event_port_num,
                aucBuffer,
                sizeof(WIFI_EVENT_T) + sizeof(EVENT_NIC_CAPABILITY),
                &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        return WLAN_STATUS_FAILURE;
    }

    // header checking ..
    prHifRxHdr = (P_HIF_RX_HEADER_T)aucBuffer;
    if(prHifRxHdr->u2PacketType != HIF_RX_PKT_TYPE_EVENT) {
        return WLAN_STATUS_FAILURE;
    }

    prEvent = (P_WIFI_EVENT_T)aucBuffer;
    if(prEvent->ucEID != EVENT_ID_NIC_CAPABILITY) {
        return WLAN_STATUS_FAILURE;
    }

    prEventNicCapability = (P_EVENT_NIC_CAPABILITY)(prEvent->aucBuffer);

    prAdapter->rVerInfo.u2FwProductID     = prEventNicCapability->u2ProductID;
    prAdapter->rVerInfo.u2FwOwnVersion    = prEventNicCapability->u2FwVersion;
    prAdapter->rVerInfo.u2FwPeerVersion   = prEventNicCapability->u2DriverVersion;
    prAdapter->fgIsHw5GBandDisabled       = (BOOLEAN)prEventNicCapability->ucHw5GBandDisabled;
    prAdapter->fgIsEepromUsed             = (BOOLEAN)prEventNicCapability->ucEepromUsed;
    prAdapter->fgIsEfuseValid             = (BOOLEAN)prEventNicCapability->ucEfuseValid;
    prAdapter->fgIsEmbbededMacAddrValid   = (BOOLEAN)prEventNicCapability->ucMacAddrValid;
    MT5931_TRACE(TRACE_INIT, MT5931_INFO_223, "u2FwProductID %x u2FwOwnVersion%x", prAdapter->rVerInfo.u2FwProductID, prAdapter->rVerInfo.u2FwOwnVersion);
    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to retrieve Crystal Trimming from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanQueryCrystalTrim(
    IN P_ADAPTER_T prAdapter
    )
{
    PARAM_MTK_WIFI_TEST_STRUC_T   data;
    //UINT_32                       pdu_len;
    P_WIFI_EVENT_T                prEvent;
    UINT_32                       u4RxPktLength;
    UINT_8                        *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_TEST_STATUS           prTestStatus;

#if 1
    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    P_CMD_TEST_CTRL_T pCmdTestCtrl;
    UINT_8 ucCmdSeqNum;

    ASSERT(prAdapter);
    
    prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, (CMD_HDR_SIZE + sizeof(CMD_TEST_CTRL_T)));

    if (!prCmdInfo) {
        DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
        return WLAN_STATUS_FAILURE;
    }

    data.u4FuncIndex = RF_AT_FUNCID_EFUSE;
    data.u4FuncData = EFUSE_BLOCK_OFFSET_7_0 >> 2;

    // increase command sequence number
    ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);
    MT5931_TRACE(TRACE_GROUP_6, MT5931_INFO_224, "ucCmdSeqNum =%d / CmdID=%x\n", ucCmdSeqNum, CMD_ID_TEST_MODE);

    // Setup common CMD Info Packet
    prCmdInfo->eCmdType = COMMAND_TYPE_GENERAL_IOCTL;
    prCmdInfo->u2InfoBufLen = CMD_HDR_SIZE + sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pfCmdDoneHandler = nicCmdEventQueryRfTestATInfo;
    prCmdInfo->pfCmdTimeoutHandler = nicOidCmdTimeoutCommon;
    prCmdInfo->fgIsOid = TRUE;
    prCmdInfo->ucCID = CMD_ID_TEST_MODE;
    prCmdInfo->fgSetQuery = FALSE;
    prCmdInfo->fgNeedResp = TRUE;
    prCmdInfo->fgDriverDomainMCR = FALSE;
    prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
    prCmdInfo->u4SetInfoLen = sizeof(CMD_TEST_CTRL_T);
    prCmdInfo->pvInformationBuffer = &data;
    prCmdInfo->u4InformationBufferLength = sizeof(data);

    // Setup WIFI_CMD_T (payload = CMD_TEST_CTRL_T)
    prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
    prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
    prWifiCmd->ucCID = prCmdInfo->ucCID;
    prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
    prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;

    pCmdTestCtrl = (P_CMD_TEST_CTRL_T)(prWifiCmd->aucBuffer);
    pCmdTestCtrl->ucAction = 2; // Get ATInfo
    pCmdTestCtrl->u.rRfATInfo.u4FuncIndex = RF_AT_FUNCID_EFUSE;
    pCmdTestCtrl->u.rRfATInfo.u4FuncData = EFUSE_BLOCK_OFFSET_7_0 >> 2;

    wlanSendCommand(prAdapter, prCmdInfo);
    cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
#else
    data.u4FuncIndex = RF_AT_FUNCID_EFUSE;
    data.u4FuncData = EFUSE_BLOCK_OFFSET_7_0 >> 2;

    GLUE_ACQUIRE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
    wlanSetInformation(prAdapter,
                       (PFN_OID_HANDLER_FUNC)wlanoidRftestQueryAutoTest,
                       (PVOID)&data,
                       sizeof(data),
                       (PUINT_32)&pdu_len);
    GLUE_RELEASE_SPIN_LOCK(prAdapter->prGlueInfo, SPIN_LOCK_FSM);
#endif    

    /* Polling Query Efuse read result */
	memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
    if(nicRxWaitResponse(prAdapter,
                g_hif_rx_event_port_num,
                aucBuffer,
                EVENT_HDR_SIZE + sizeof(EVENT_TEST_STATUS),
                &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
        prAdapter->prGlueInfo->cCystal_trim = 0;
        return WLAN_STATUS_FAILURE;
    }

    prEvent = (P_WIFI_EVENT_T)aucBuffer;
    prTestStatus = (P_EVENT_TEST_STATUS)(prEvent->aucBuffer);

    /* Save Query Efuse Event info */
    prAdapter->prGlueInfo->cCystal_trim = prTestStatus->rATInfo.u4FuncData;

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to load manufacture data from NVRAM
* if available and valid
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param prRegInfo      Pointer of REG_INFO_T
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_FAILURE
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanLoadManufactureData (
    IN P_ADAPTER_T prAdapter,
    IN P_REG_INFO_T prRegInfo
    )
{
    INT_8   cTx_ch_offset_L = 0;
    INT_8   cTx_ch_offset_M = 0;
    INT_8   cTx_ch_offset_H = 0;
    INT_8   cTx_ch_offset_L_temp;
    INT_8   cTx_ch_offset_M_temp;
    INT_8   cTx_ch_offset_H_temp;
    INT_32  c4Tx_ch_offset_temp;
    INT_32  c4Pd_slope;
    UINT_8  count;

    ASSERT(prAdapter);    

    /* 0. Initialization */
    memset( prRegInfo->aucEFUSE, 0 , sizeof(CMD_PHY_PARAM_T) );
    prRegInfo->ucTxPwrValid = 1;
    prRegInfo->fg2G4BandEdgePwrUsed = 1;

    /* NOTE(Nelson): host driver do not need to read efuse result,
     *               the f/w would add the CMD value and efuse value and then set to CR
     */
#if 0
    /* 1. query for cystal trimming in efuse */
    wlanQueryCrystalTrim(prAdapter);
#endif
    

    /* 2. Load TX power gain parameters if valid */
    if( prRegInfo->ucTxPwrValid != 0 )
    {
        /* polling CMD TX resource */
        nicTxPollingResource(prAdapter,TC4_INDEX);
        
        wlanUpdateTxPower (
            prAdapter, 
            prAdapter->prGlueInfo->c11b_tx_pwr,
            prAdapter->prGlueInfo->c11g_tx_pwr,
            prAdapter->prGlueInfo->c11n_bw20_tx_pwr,
            prAdapter->prGlueInfo->c11n_bw40_tx_pwr
            );
        kalMainThread(prAdapter->prGlueInfo);
    }

    /* 3.1. Tx channel power offset conversion */
        // 3.1.1. Calculate the Power Error (unit: 0.1 db) 
    cTx_ch_offset_L = prAdapter->prGlueInfo->cCh_offset_L;
    cTx_ch_offset_M = prAdapter->prGlueInfo->cCh_offset_M;
    cTx_ch_offset_H = prAdapter->prGlueInfo->cCh_offset_H;
        // 3.1.2. Re-define the Power Error
    cTx_ch_offset_L_temp = (cTx_ch_offset_L < 0) ? (0 - cTx_ch_offset_L) : cTx_ch_offset_L;  /* abs(power_error_ch) */
    cTx_ch_offset_M_temp = (cTx_ch_offset_M < 0) ? (0 - cTx_ch_offset_M) : cTx_ch_offset_M;
    cTx_ch_offset_H_temp = (cTx_ch_offset_H < 0) ? (0 - cTx_ch_offset_H) : cTx_ch_offset_H;

    cTx_ch_offset_L = ((cTx_ch_offset_L_temp*2) < 5) ? 0 : cTx_ch_offset_L;  /* if ( abs(power_error_ch) < 2.5 ) */
    cTx_ch_offset_M = ((cTx_ch_offset_M_temp*2) < 5) ? 0 : cTx_ch_offset_M;
    cTx_ch_offset_H = ((cTx_ch_offset_H_temp*2) < 5) ? 0 : cTx_ch_offset_H;
        // 3.1.3. Read the PD slope
    /* polling CMD TX resource */
    nicTxPollingResource(prAdapter,TC4_INDEX);
    c4Pd_slope = wlanQueryMcr( prAdapter, PD_SLOPE_CR_ADDR ) & BITS(0,6);
    
        // 3.1.4. Check power_errors are all 0 or not
    count = 3;
    while( count > 0 )
    {
        if( count == 3 )
            c4Tx_ch_offset_temp = (UINT_32)cTx_ch_offset_L;
        else if ( count == 2 )
            c4Tx_ch_offset_temp = (UINT_32)cTx_ch_offset_M;
        else
            c4Tx_ch_offset_temp = (UINT_32)cTx_ch_offset_H;
            
        if ( c4Tx_ch_offset_temp != 0 )
        {
            // 3.1.5. Calculate the Power Offset
            c4Tx_ch_offset_temp = c4Tx_ch_offset_temp * c4Pd_slope;
            c4Tx_ch_offset_temp = ((c4Tx_ch_offset_temp % 10) != 0) ? 
                (0 - ((c4Tx_ch_offset_temp+(10-(c4Tx_ch_offset_temp%10)))/10)) : (0 - c4Tx_ch_offset_temp/10);
            // 3.1.6. Convert to NVRAM Format
            if( ( c4Tx_ch_offset_temp < -256 ) || (c4Tx_ch_offset_temp > 255) )
                c4Tx_ch_offset_temp = 256;
            else if( c4Tx_ch_offset_temp < 0 )
                c4Tx_ch_offset_temp += 512;

            if( count == 3 )
                cTx_ch_offset_L = (INT_8)c4Tx_ch_offset_temp;
            else if ( count == 2 )
                cTx_ch_offset_M = (INT_8)c4Tx_ch_offset_temp;
            else
                cTx_ch_offset_H = (INT_8)c4Tx_ch_offset_temp;
        }
        count--;
    }

        // 3.1.7. Protect compensation valuses between 0x7F ~ 0x80
    if( 
            (prAdapter->prGlueInfo->cCh_offset_L < 0) && 
            ( ((UINT_8)cTx_ch_offset_L & 0xff) > 0x7F )
        )
    {
        cTx_ch_offset_L = 0x7F;
    }
    else if( 
            (prAdapter->prGlueInfo->cCh_offset_L > 0) && 
            ( ((UINT_8)cTx_ch_offset_L & 0xff) < 0x80 )
        )
    {
        *(UINT_8 *)&cTx_ch_offset_L = 0x80;	// why set 0x80 to signed char?
    }      
    if( 
            (prAdapter->prGlueInfo->cCh_offset_M < 0) && 
            ( ((UINT_8)cTx_ch_offset_M & 0xff) > 0x7F )
        )
    {
        cTx_ch_offset_M = 0x7F;
    }
    else if( 
            (prAdapter->prGlueInfo->cCh_offset_M > 0) && 
            ( ((UINT_8)cTx_ch_offset_M & 0xff) < 0x80 )
        )
    {
        *(UINT_8 *)&cTx_ch_offset_M = 0x80;	// why set 0x80 to signed char?
    }
    if( 
            (prAdapter->prGlueInfo->cCh_offset_H < 0) && 
            ( ((UINT_8)cTx_ch_offset_H & 0xff) > 0x7F )
        )
    {
        cTx_ch_offset_H = 0x7F;
    }
    else if( 
            (prAdapter->prGlueInfo->cCh_offset_H > 0) && 
            ( ((UINT_8)cTx_ch_offset_H & 0xff) < 0x80 )
        )
    {
        *(UINT_8 *)&cTx_ch_offset_H = 0x80;	// why set 0x80 to signed char?
    }


    /* 3.2. Update tx channel power offset */
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_3_1 ] = cTx_ch_offset_L;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_3_1 + 1 ] = cTx_ch_offset_M;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_3_1 + 2 ] = cTx_ch_offset_H;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_3_1 + 3 ] = (TX_ALL_CH_OFFSET << 1) | EFUSE_CH_OFFSET_VLD_1;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_4_0 ] = cTx_ch_offset_L;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_4_0 + 1 ] = cTx_ch_offset_M;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_4_0 + 2 ] = cTx_ch_offset_H;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_4_0 + 3 ] = EFUSE_CH_OFFSET_VLD_2;

    /* 4. Update tx frequency offset */
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_7_0 + 1 ] = 
        (EFUSE_CRYSTAL_TRIM_1_VLD << 7) | prAdapter->prGlueInfo->cFreq_offset;
    prRegInfo->aucEFUSE[ EFUSE_BLOCK_OFFSET_7_0 + 2 ] = 
        (EFUSE_CRYSTAL_TRIM_2_VLD << 7) | prAdapter->prGlueInfo->cFreq_offset;
    
    /* polling CMD TX resource */
    nicTxPollingResource(prAdapter,TC4_INDEX);

    /* 5. Send EFUSE data */
    wlanSendSetQueryCmd(prAdapter,
            CMD_ID_SET_PHY_PARAM,
            TRUE,
            FALSE,
            FALSE,
            NULL,
            NULL,
            sizeof(CMD_PHY_PARAM_T),
            (PUINT_8)(prRegInfo->aucEFUSE),
            NULL,
            0);
    kalMainThread(prAdapter->prGlueInfo);

    /* 6. set band edge tx power if available */
    if( (prRegInfo->fg2G4BandEdgePwrUsed) && (!(prAdapter->prGlueInfo->meta_mode)) ) 
    {
        CMD_EDGE_TXPWR_LIMIT_T rCmdEdgeTxPwrLimit;

        /* NOTE(Nelson): band edge power setting is 0.5 dBm */
        rCmdEdgeTxPwrLimit.cBandEdgeMaxPwrCCK = prAdapter->prGlueInfo->cCck_band_edge_tx_pwr << 1;
        rCmdEdgeTxPwrLimit.cBandEdgeMaxPwrOFDM20 = prAdapter->prGlueInfo->cOfdm_bw20_band_edge_tx_pwr << 1;
        rCmdEdgeTxPwrLimit.cBandEdgeMaxPwrOFDM40 = prAdapter->prGlueInfo->cOfdm_bw40_band_edge_tx_pwr << 1;

        /* polling CMD TX resource */
        nicTxPollingResource(prAdapter,TC4_INDEX);

        wlanSendSetQueryCmd(prAdapter,
                CMD_ID_SET_EDGE_TXPWR_LIMIT,
                TRUE,
                FALSE,
                FALSE,
                NULL,
                NULL,
                sizeof(CMD_EDGE_TXPWR_LIMIT_T),
                (PUINT_8)&rCmdEdgeTxPwrLimit,
                NULL,
                0);
        kalMainThread(prAdapter->prGlueInfo);
    }
    else
    {
        /* set band edge tx power to default values */
        CMD_EDGE_TXPWR_LIMIT_T rCmdEdgeTxPwrLimit;

        /* NOTE(Nelson): band edge power to the max values */
        rCmdEdgeTxPwrLimit.cBandEdgeMaxPwrCCK = DefaultBandEdgeMaxPwrCCK;
        rCmdEdgeTxPwrLimit.cBandEdgeMaxPwrOFDM20 = DefaultBandEdgeMaxPwrOFDM20;
        rCmdEdgeTxPwrLimit.cBandEdgeMaxPwrOFDM40 = DefaultBandEdgeMaxPwrOFDM40;

        /* polling CMD TX resource */
        nicTxPollingResource(prAdapter,TC4_INDEX);

        wlanSendSetQueryCmd(prAdapter,
                CMD_ID_SET_EDGE_TXPWR_LIMIT,
                TRUE,
                FALSE,
                FALSE,
                NULL,
                NULL,
                sizeof(CMD_EDGE_TXPWR_LIMIT_T),
                (PUINT_8)&rCmdEdgeTxPwrLimit,
                NULL,
                0);
        kalMainThread(prAdapter->prGlueInfo);
    }

#if 0   /* TODO(Nelson): check if needed */
    /* 3. Check if needs to support 5GHz */
    if(prRegInfo->ucEnable5GBand) {
        // check if it is disabled by hardware
        if(prAdapter->fgIsHw5GBandDisabled
                || prRegInfo->ucSupport5GBand == 0) {
            prAdapter->fgEnable5GBand = FALSE;
        }
        else {
            prAdapter->fgEnable5GBand = TRUE;
        }
    }
    else {
        prAdapter->fgEnable5GBand = FALSE;
    }

    /* 5. Get 16-bits Country Code and Bandwidth */
    prAdapter->rWifiVar.rConnSettings.u2CountryCode =
        (((UINT_16) prRegInfo->au2CountryCode[0]) << 8) |
        (((UINT_16) prRegInfo->au2CountryCode[1]) & BITS(0,7));

#if 0 /* Bandwidth control will be controlled by GUI. 20110930
       * So ignore the setting from registry/NVRAM
       */
    prAdapter->rWifiVar.rConnSettings.uc2G4BandwidthMode =
            prRegInfo->uc2G4BwFixed20M ? CONFIG_BW_20M : CONFIG_BW_20_40M;
    prAdapter->rWifiVar.rConnSettings.uc5GBandwidthMode =
            prRegInfo->uc5GBwFixed20M ? CONFIG_BW_20M : CONFIG_BW_20_40M;
#endif

    /* 6. Set domain and channel information to chip */
    rlmDomainSendCmd(prAdapter, FALSE);    
#endif

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to check
*        Media Stream Mode is set to non-default value or not,
*        and clear to default value if above criteria is met
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return TRUE
*           The media stream mode was non-default value and has been reset
*         FALSE
*           The media stream mode is default value
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
wlanResetMediaStreamMode(
    IN P_ADAPTER_T prAdapter
    )
{
    ASSERT(prAdapter);

    if(prAdapter->rWlanInfo.eLinkAttr.ucMediaStreamMode != 0) {
        prAdapter->rWlanInfo.eLinkAttr.ucMediaStreamMode = 0;

        return TRUE;
    }
    else {
        return FALSE;
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to check if any pending timer has expired
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanTimerTimeoutCheck(
    IN P_ADAPTER_T prAdapter
    )
{
    ASSERT(prAdapter);

    cnmTimerDoTimeOutCheck(prAdapter);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to check if any pending mailbox message
*        to be handled
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanProcessMboxMessage(
    IN P_ADAPTER_T prAdapter
    )
{
    UINT_32 i;

    ASSERT(prAdapter);

    for(i = 0 ; i < MBOX_ID_TOTAL_NUM ; i++) {
       mboxRcvAllMsg(prAdapter , (ENUM_MBOX_ID_T)i);
    }

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to enqueue a single TX packet into CORE
*
* @param prAdapter      Pointer of Adapter Data Structure
*        prNativePacket Pointer of Native Packet
*
* @return WLAN_STATUS_SUCCESS
*         WLAN_STATUS_RESOURCES
*         WLAN_STATUS_INVALID_PACKET
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanEnqueueTxPacket (
    IN P_ADAPTER_T      prAdapter,
    IN P_NATIVE_PACKET  prNativePacket
    )
{
    P_TX_CTRL_T prTxCtrl;
    P_MSDU_INFO_T prMsduInfo;
#if	CFG_ENABLE_THROUGHPUT_DBG
	SLA_CustomLogging("WEP", SA_start);
#endif
    KAL_SPIN_LOCK_DECLARATION();

   // DEBUGFUNC("wlanEnqueueTxPacket");

    ASSERT(prAdapter);
	//ILMTraceCheck(60);
    prTxCtrl = &prAdapter->rTxCtrl;

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
    QUEUE_REMOVE_HEAD(&prTxCtrl->rFreeMsduInfoList, prMsduInfo, P_MSDU_INFO_T);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);

    if(prMsduInfo == NULL) {
#if	CFG_ENABLE_THROUGHPUT_DBG
		SLA_CustomLogging("WEP", SA_stop);
#endif
        return WLAN_STATUS_RESOURCES;
    }
    else {
        prMsduInfo->eSrc = TX_PACKET_OS;

        if(nicTxFillMsduInfo(prAdapter,
                    prMsduInfo,
                    prNativePacket) == FALSE) { // packet is not extractable
            kalSendComplete(prAdapter->prGlueInfo,
                    prNativePacket,
                    WLAN_STATUS_INVALID_PACKET);

            nicTxReturnMsduInfo(prAdapter, prMsduInfo);
#if	CFG_ENABLE_THROUGHPUT_DBG
			SLA_CustomLogging("WEP", SA_stop);
#endif
            MT5931_WAP_TRACE(MOD_WNDRV,TRACE_WARNING, "wlanEnqueueTxPacket: MSDU %x fail", prMsduInfo);
            return WLAN_STATUS_INVALID_PACKET;
        }
        else {
            // enqueue to QM
            nicTxEnqueueMsdu(prAdapter, prMsduInfo);
            //ILMTraceCheck(69);
#if	CFG_ENABLE_THROUGHPUT_DBG
			SLA_CustomLogging("WEP", SA_stop);
#endif
            return WLAN_STATUS_SUCCESS;
        }
    }
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to flush pending TX packets in CORE
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanFlushTxPendingPackets(
    IN P_ADAPTER_T prAdapter
    )
{
    ASSERT(prAdapter);

    return nicTxFlush(prAdapter);
}


/*----------------------------------------------------------------------------*/
/*!
* \brief this function sends pending MSDU_INFO_T to MT6620
*
* @param prAdapter      Pointer to the Adapter structure.
* @param pfgHwAccess    Pointer for tracking LP-OWN status
*
* @retval WLAN_STATUS_SUCCESS   Reset is done successfully.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanTxPendingPackets (
    IN      P_ADAPTER_T prAdapter,
    IN OUT  PBOOLEAN    pfgHwAccess
    )
{
    P_TX_CTRL_T prTxCtrl;
    P_MSDU_INFO_T prMsduInfo;
#if	CFG_ENABLE_THROUGHPUT_DBG
	SLA_CustomLogging("WTP", SA_start);
#endif

    KAL_SPIN_LOCK_DECLARATION();


    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;
	//ILMTraceCheck(70);
    ASSERT(pfgHwAccess);

    // <1> dequeue packet by txDequeuTxPackets()
    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);
    prMsduInfo = qmDequeueTxPackets(prAdapter, &prTxCtrl->rTc);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);

    if(prMsduInfo != NULL) {
        if(kalIsCardRemoved(prAdapter->prGlueInfo) == FALSE) {
            /* <2> Acquire LP-OWN if necessary */
            if(*pfgHwAccess == FALSE) {
                *pfgHwAccess = TRUE;

                wlanAcquirePowerControl(prAdapter);
            }

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
            if(prAdapter->fgIsClockGatingEnabled == TRUE) {
                nicDisableClockGating(prAdapter);
            }
#endif
            // <3> send packets
            nicTxMsduInfoList(prAdapter, prMsduInfo);

            // <4> update TC by txAdjustTcQuotas()
            nicTxAdjustTcq(prAdapter);
        }
        else {
            wlanProcessQueuedMsduInfo(prAdapter, prMsduInfo);
        }
    }

#if (MT6620_E1_ASIC_HIFSYS_WORKAROUND == 1)
    if(prAdapter->fgIsClockGatingEnabled == FALSE) {
        nicEnableClockGating(prAdapter);
    }
#endif
#if	CFG_ENABLE_THROUGHPUT_DBG
	SLA_CustomLogging("WTP", SA_stop);
#endif
	//ILMTraceCheck(79);
    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to acquire power control from firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanAcquirePowerControl(
    IN P_ADAPTER_T prAdapter
    )
{
    ASSERT(prAdapter);

    ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);

    /* Reset sleepy state */
    if(prAdapter->fgWiFiInSleepyState == TRUE) {
        prAdapter->fgWiFiInSleepyState = FALSE;
    }

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to release power control to firmware
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return WLAN_STATUS_SUCCESS
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanReleasePowerControl(
    IN P_ADAPTER_T prAdapter
    )
{
    ASSERT(prAdapter);

    RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);

    return WLAN_STATUS_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to report currently pending TX frames count
*        (command packets are not included)
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return number of pending TX frames
*/
/*----------------------------------------------------------------------------*/
UINT_32
wlanGetTxPendingFrameCount (
    IN P_ADAPTER_T prAdapter
    )
{
    P_TX_CTRL_T prTxCtrl;
    UINT_32 u4Num;

    ASSERT(prAdapter);
    prTxCtrl = &prAdapter->rTxCtrl;

    u4Num = kalGetTxPendingFrameCount(prAdapter->prGlueInfo) + (UINT_32)(prTxCtrl->i4PendingFwdFrameCount);

    return u4Num;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is to report current ACPI state
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return ACPI_STATE_D0 Normal Operation Mode
*         ACPI_STATE_D3 Suspend Mode
*/
/*----------------------------------------------------------------------------*/
ENUM_ACPI_STATE_T
wlanGetAcpiState (
    IN P_ADAPTER_T prAdapter
    )
{
    ASSERT(prAdapter);

    return prAdapter->rAcpiState;
}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is to update current ACPI state only
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param ePowerState    ACPI_STATE_D0 Normal Operation Mode
*                       ACPI_STATE_D3 Suspend Mode
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanSetAcpiState (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_ACPI_STATE_T ePowerState
    )
{
    ASSERT(prAdapter);
    ASSERT(ePowerState <= ACPI_STATE_D3);

    prAdapter->rAcpiState = ePowerState;

    return;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function will take KAL_MSDU_INFO_T which carry some informations of
*        incoming frame from GLUE Layer and notify the ARBITER to send packet.
*
* \param prAdapter      Pointer of Adapter Data Structure
* \param prKalMsduInfo  Pointer of KAL_MSDU_INFO_T
*
* \retval WLAN_STATUS_SUCCESS   : MSDU was written to HW TX FIFO and be freed(SendComplete) immediately.
* \retval WLAN_STATUS_PENDING   : MSDU is pending in SW Queues to wait for previous frame finishing
                                  their transmission and release buffer.
* \retval WLAN_STATUS_FAILURE   : Get failure while access HW TX FIFO or been rejected.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSendPacket (
    IN P_GLUE_INFO_T                prGlueInfo,
    IN wndrv_pkt_descriptor_struct* wndrv_pkt_p
    )
{
        P_QUE_ENTRY_T prQueueEntry = NULL;
        P_QUE_T prTxQueue = NULL;
        UINT_16 u2QueueIdx = 0;
        peer_buff_struct  *peer_p = NULL;

		#if CFG_ENABLE_TXRX_PATH_LOG
        MT5931_WAP_TRACE(MOD_WNDRV,TRACE_TXRX,"wlanSendPacket");
        #endif
        GLUE_SPIN_LOCK_DECLARATION();

        ASSERT(prGlueInfo);
        ASSERT(wndrv_pkt_p);

        peer_p = wndrv_pkt_p->tx_peer_p;

        if (prGlueInfo->u4Flag & GLUE_FLAG_HALT) {
            //printk(KERN_INFO DRV_NAME"GLUE_FLAG_HALT skip tx\n");
            //dev_kfree_skb(prSkb);
#if defined (WLAN_ZERO_COPY)
            if( wndrv_pkt_p->flc_mode )
            {
                flc2_free_peer_buff((flc2_pool_id_enum) wndrv_pkt_p->pool_id, peer_p);
                prGlueInfo->tx_flc_peer_num--;
            }
            else
#endif
            {
                free_peer_buff(peer_p);
                prGlueInfo->tx_sys_peer_num--;
            }
            return WLAN_STATUS_SUCCESS;
        }

        prQueueEntry = (P_QUE_ENTRY_T)GLUE_GET_PKT_QUEUE_ENTRY(peer_p);
        prTxQueue = &prGlueInfo->rTxQueue;

        if (wlanProcessSecurityFrame(prGlueInfo->prAdapter, (P_NATIVE_PACKET)peer_p) == FALSE) {

//#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 26)
#if 0
            u2QueueIdx = skb_get_queue_mapping(prSkb);
            ASSERT(u2QueueIdx < CFG_MAX_TXQ_NUM);
#endif

            GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);
            QUEUE_INSERT_TAIL(prTxQueue, prQueueEntry);
            GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);

            GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingFrameNum);
            GLUE_INC_REF_CNT(prGlueInfo->ai4TxPendingFrameNumPerQueue[NETWORK_TYPE_AIS_INDEX][u2QueueIdx]);

          #if CFG_TCP_IP_FLOW_CTRL
	          if((prGlueInfo->i4TxPendingFrameNum == 1)&&(!prGlueInfo->fgPending_Ctrl)){ //first packet before pending control	                 
			   prGlueInfo->fgPending_Ctrl = 0;
		          prGlueInfo->fgPending_Frame0 = 1;		
	          }
	          else if(prGlueInfo->i4TxPendingFrameNum < ucTxPendingFrameNum){
		          prGlueInfo->fgPending_Ctrl = 1;

	          }
	          else{
		          prGlueInfo->fgPending_Ctrl = 0;
	          }
          #endif
//#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 26)
#if 0
            if (prGlueInfo->ai4TxPendingFrameNumPerQueue[NETWORK_TYPE_AIS_INDEX][u2QueueIdx] >= CFG_TX_STOP_NETIF_PER_QUEUE_THRESHOLD) {
                netif_stop_subqueue(prDev, u2QueueIdx);
            }
#else
            if (prGlueInfo->i4TxPendingFrameNum >= CFG_TX_STOP_NETIF_QUEUE_THRESHOLD) {
                //netif_stop_queue(prDev);
                ASSERT(0);  //TODO(Nelson): large pending frame handling
            }
#endif
        } else {
            //printk("is security frame\n");
			
			MT5931_WAP_TRACE(MOD_WNDRV,TRACE_TXRX,"wlanSendPacket():802.1x Data");
            GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingSecurityFrameNum);
        }

		#if CFG_ENABLE_TXRX_PATH_LOG
        MT5931_TRACE(TRACE_PEER, MT5931_INFO_226, "\n+++++ pending frame %d len = %d +++++\n", prGlueInfo->i4TxPendingFrameNum,
            wndrv_pkt_p->prPacketInfo->u2PayloadLength);
		#endif
        //pr->u4Flag |= GLUE_FLAG_TXREQ;
        //wake_up_interruptible(&prGlueInfo->waitq);
        kalSetEvent(prGlueInfo);

        /* For Linux, we'll always return OK FLAG, because we'll free this skb by ourself */
        return WLAN_STATUS_SUCCESS;
} /* end of wlanSendPacket() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will update TX target power.
*
* \param prAdapter      Pointer of Adapter Data Structure
* \param c11b_TxPower   11b TX power
* \param c11b_TxPower   11g TX power
* \param c11b_TxPower   11n BW20 TX power
* \param c11b_TxPower   11n BW40 TX power
*
* \retval WLAN_STATUS_SUCCESS   : MSDU was written to HW TX FIFO and be freed(SendComplete) immediately.
* \retval WLAN_STATUS_PENDING   : MSDU is pending in SW Queues to wait for previous frame finishing
                                  their transmission and release buffer.
* \retval WLAN_STATUS_FAILURE   : Get failure while access HW TX FIFO or been rejected.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanUpdateTxPower (
    IN P_ADAPTER_T  prAdapter,
    IN INT_8        c11b_TxPower,
    IN INT_8        c11g_TxPower,
    IN INT_8        c11n_bw20_TxPower,
    IN INT_8        c11n_bw40_TxPower
    )
{
    WLAN_STATUS status;
    CMD_TX_PWR_T rTxPwrParam;

    rTxPwrParam.cTxPwr2G4Dsss = (c11b_TxPower << 1);
    rTxPwrParam.cTxPwr2G4Cck = (c11b_TxPower << 1);

    rTxPwrParam.cTxPwr2G4OFDM_BPSK =  (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr2G4OFDM_QPSK =  (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr2G4OFDM_16QAM = (c11g_TxPower << 1);

    rTxPwrParam.cTxPwr2G4OFDM_48Mbps = (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr2G4OFDM_54Mbps = (c11g_TxPower << 1);

    rTxPwrParam.cTxPwr2G4HT20_BPSK =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_QPSK =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_16QAM = (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_MCS5 =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_MCS6 =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT20_MCS7 =  (c11n_bw20_TxPower << 1);

    rTxPwrParam.cTxPwr2G4HT40_BPSK =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_QPSK =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_16QAM = (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_MCS5 =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_MCS6 =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr2G4HT40_MCS7 =  (c11n_bw40_TxPower << 1);

    rTxPwrParam.cTxPwr5GOFDM_BPSK =   (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_QPSK =   (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_16QAM =  (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_48Mbps = (c11g_TxPower << 1);
    rTxPwrParam.cTxPwr5GOFDM_54Mbps = (c11g_TxPower << 1);

    rTxPwrParam.cTxPwr5GHT20_BPSK =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_QPSK =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_16QAM = (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_MCS5 =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_MCS6 =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT20_MCS7 =  (c11n_bw20_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_BPSK =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_QPSK =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_16QAM = (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_MCS5 =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_MCS6 =  (c11n_bw40_TxPower << 1);
    rTxPwrParam.cTxPwr5GHT40_MCS7 =  (c11n_bw40_TxPower << 1);

    status =  nicUpdateTxPower(prAdapter, &rTxPwrParam);    /* Send to F/W */
    return status;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function will do whole chip reset.
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \retval WLAN_STATUS_SUCCESS   : MSDU was written to HW TX FIFO and be freed(SendComplete) immediately.
* \retval WLAN_STATUS_PENDING   : MSDU is pending in SW Queues to wait for previous frame finishing
*                                 their transmission and release buffer.
* \retval WLAN_STATUS_FAILURE   : Get failure while access HW TX FIFO or been rejected.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanWholeChipReset (
    IN P_ADAPTER_T  prAdapter
    )
{
    WLAN_STATUS status = WLAN_STATUS_SUCCESS;
    P_REG_INFO_T prRegInfo = &wndrv_context.adpt_p->prGlueInfo->rRegInfo;
    ilm_struct  *ilm_p;
    //kal_int32  i;
    kal_int32  queue_index = 0;
    //kal_int32  tx_rx_diff = 0;
    kal_int32  fgIsWlanStop = 0;
    wndrv_supc_network_connect_fail_ind_struct *con_fail_ind_p;
    wndrv_supc_network_disconnect_ind_struct *disjoin_ind_p;
    wndrv_abm_set_ps_mode_cnf_struct *cnf_ptr;
    wmt_wifi_query_rssi_ind_struct *out_p;   

	WNDRV_TRACE(TRACE_WARNING, "%s\n", __func__);
    /* Pending SAP handling */
    while (wndrv_int_queue_num != 0)
    {
        QUE_ENTRY                    *entry_p;
        wndrv_int_queue_info_struct  *int_queue_p;

        /* round robin */
        if ( ++queue_index >= WNDRV_INT_QUEUE_TYPE-1 )
        {
            queue_index = 0;
        }

        entry_p = QUE_GET_HEAD( &wndrv_int_queue_list[queue_index] );

        if((KAL_TRUE == WNDRVPauseFlag) &&
            (entry_p == NULL) &&
            (queue_index == WNDRV_INT_QUEUE_TIMER_INDEX) )
        {
            break;
        }

        if (entry_p)
        {
            entry_p = Q_REMOVE_HEAD( &wndrv_int_queue_list[queue_index] );
            wndrv_int_queue_num--;

            int_queue_p = (wndrv_int_queue_info_struct *)entry_p;

            switch ( int_queue_p->ilm.msg_id )
            {
                case MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ:
                    wndrv_supc_site_survey_ind_handler();
                    break;

                case MSG_ID_WNDRV_SUPC_JOIN_NETWORK_REQ:
                    con_fail_ind_p = (wndrv_supc_network_connect_fail_ind_struct *)
                        construct_local_para( sizeof( wndrv_supc_network_connect_fail_ind_struct ), TD_CTRL );
                    con_fail_ind_p->session_id = 0;
#ifdef WIFI_BLACKLIST // zheng.zhou add to avoid build error
                    con_fail_ind_p->bssid_list_num = 0;
#endif
                    ilm_p                 = allocate_ilm ( MOD_WNDRV );
                    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_CONNECT_FAIL_IND;
                    ilm_p->local_para_ptr = (local_para_struct *) con_fail_ind_p;
                    ilm_p->peer_buff_ptr  = NULL;
                    WNDRV_SEND_MSG_TO_SUPC( ilm_p );
                    break;

                case MSG_ID_WNDRV_SUPC_DISJOIN_NETWORK_REQ:
                    disjoin_ind_p = (wndrv_supc_network_disconnect_ind_struct *)
                        construct_local_para( sizeof( wndrv_supc_network_disconnect_ind_struct ), TD_RESET );
                    disjoin_ind_p->session_id = 0;					
#ifdef WIFI_BLACKLIST // zheng.zhou add to avoid build error
                    disjoin_ind_p->bssid_list_num = 0;
#endif
                    ilm_p                 = allocate_ilm ( MOD_WNDRV );
                    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND;
                    ilm_p->local_para_ptr = (local_para_struct *) disjoin_ind_p;
                    ilm_p->peer_buff_ptr  = NULL;
                    WNDRV_SEND_MSG_TO_SUPC( ilm_p );
                    break;

                case MSG_ID_WNDRV_SUPC_DEINIT_REQ:
                    ilm_p                 = allocate_ilm ( MOD_WNDRV );
                    ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_DEINIT_CNF;
                    ilm_p->local_para_ptr = NULL;
                    ilm_p->peer_buff_ptr  = NULL;
                    WNDRV_SEND_MSG_TO_SUPC( ilm_p );
                    
                    fgIsWlanStop = 1;
                    break;
                    
                case MSG_ID_WNDRV_ABM_SET_PS_MODE_REQ:
                    cnf_ptr = (wndrv_abm_set_ps_mode_cnf_struct *)
                      construct_local_para(sizeof(wndrv_abm_set_ps_mode_cnf_struct), TD_CTRL);
                    cnf_ptr->status = KAL_TRUE;
                    cnf_ptr->ps_mode = WNDRV_POWER_MODE_MAX_PS;
                    ilm_p = allocate_ilm (MOD_WNDRV);
                    ilm_p->src_mod_id = MOD_WNDRV;
                    ilm_p->dest_mod_id = MOD_ABM;
                    ilm_p->msg_id = MSG_ID_WNDRV_ABM_SET_PS_MODE_CNF;
                    ilm_p->local_para_ptr = (local_para_struct *)cnf_ptr;
                    ilm_p->peer_buff_ptr = NULL;
                    msg_send_ext_queue(ilm_p);
                    break;
                    
                case MSG_ID_WMT_WNDRV_QUERY_RSSI_REQ:
                    out_p = (wmt_wifi_query_rssi_ind_struct *)
                        construct_local_para( sizeof( wmt_wifi_query_rssi_ind_struct ), TD_CTRL );    
                    out_p->wifi_rssi = PARAM_WHQL_RSSI_MAX_DBM;
                    ilm_p                 = allocate_ilm( MOD_WNDRV );
                    ilm_p->src_mod_id     = MOD_WNDRV;
                    ilm_p->dest_mod_id    = MOD_BWCS;
                    ilm_p->msg_id         = MSG_ID_WNDRV_WMT_QUERY_RSSI_IND;//MSG_ID_WNDRV_WMT_QUERY_RSSI_IND
                    ilm_p->local_para_ptr = (local_para_struct *) out_p;
                    ilm_p->peer_buff_ptr  = NULL;
                    msg_send_ext_queue(ilm_p);
                    break;
            }

            if ( int_queue_p->ilm.src_mod_id != MOD_TIMER )
            {
                free_ilm( &int_queue_p->ilm );
            }
            QUE_INS_TAIL(&wndrv_int_queue_list[WNDRV_INT_QUEUE_FREE_INDEX], &int_queue_p->q_entry);
        }
    }

    if ( prAdapter->prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED )
    {
        disjoin_ind_p = (wndrv_supc_network_disconnect_ind_struct *)
                        construct_local_para( sizeof( wndrv_supc_network_disconnect_ind_struct ), TD_CTRL );
        disjoin_ind_p->session_id = 0;		
#ifdef WIFI_BLACKLIST // zheng.zhou add to avoid build error
        disjoin_ind_p->bssid_list_num = 0;
#endif
        ilm_p                 = allocate_ilm ( MOD_WNDRV );
        ilm_p->msg_id         = MSG_ID_WNDRV_SUPC_NETWORK_DISCONNECT_IND;
        ilm_p->local_para_ptr = (local_para_struct *) disjoin_ind_p;
        ilm_p->peer_buff_ptr  = NULL;
        WNDRV_SEND_MSG_TO_SUPC( ilm_p );
    }
    
    /* Whole chip reset */
    wlanAdapterStop( prAdapter );
    if ( fgIsWlanStop == 0 )
    {
        status = wlanAdapterStart( prAdapter, prRegInfo,(PVOID)wndrv_firmware_p, wndrv_firmware_size );
    }

    return status;
}


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will do MCR query.
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \retval MCR_VALUE   : MCR read value.
*/
/*----------------------------------------------------------------------------*/
UINT_32 
wlanQueryMcr( 
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32 mcr_offset 
    )
{
    //UINT_32 u4BufLen;    
    //P_GLUE_INFO_T prGlueInfo = prAdapter->prGlueInfo;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;    
    P_WIFI_EVENT_T prEvent;
    UINT_8 *aucBuffer = wndrv_cmd_temp_buf;
    P_EVENT_ACCESS_REG prRegStatus;
    UINT_32 u4RxPktLength;    
    UINT_32 count = 0;

    P_CMD_INFO_T prCmdInfo;
    P_WIFI_CMD_T prWifiCmd;
    UINT_8 ucCmdSeqNum;

    DEBUGFUNC("wlanQueryMcr");

    /* Be sure MCR address is 4 byte boundary. */
    if ( (mcr_offset & 0x03) != 0x00 )
    {
        return 0;
    }

#if 1
    rMcrInfo.u4McrOffset = mcr_offset;

    /* 0x9000 - 0x9EFF reserved for FW */
#if CFG_SUPPORT_SWCR
    if( (mcr_offset >>16) == 0x9F00 ) {
           swCrReadWriteCmd(prAdapter,
                    SWCR_READ,
                    (UINT_16) (rMcrInfo.u4McrOffset & BITS(0,15)),
                    (UINT_32 *)&rMcrInfo.u4McrData);
    }
#endif /* CFG_SUPPORT_SWCR */

    /* Check if access F/W Domain MCR (due to WiFiSYS is placed from 0x6000-0000*/
    if ( mcr_offset & 0xFFFF0000 ){
        prCmdInfo = cmdBufAllocateCmdInfo(prAdapter, (CMD_HDR_SIZE + sizeof(CMD_ACCESS_REG)));

        if (!prCmdInfo) {
            DBGLOG(INIT, ERROR, ("Allocate CMD_INFO_T ==> FAILED.\n"));
            return WLAN_STATUS_FAILURE;
        }
        
        // increase command sequence number
        ucCmdSeqNum = nicIncreaseCmdSeqNum(prAdapter);
        MT5931_TRACE(TRACE_GROUP_6, MT5931_INFO_227, "ucCmdSeqNum =%d / CmdID=%x\n", ucCmdSeqNum, CMD_ID_ACCESS_REG);

        // Setup common CMD Info Packet
        prCmdInfo->eCmdType = COMMAND_TYPE_NETWORK_IOCTL;
        prCmdInfo->eNetworkType = NETWORK_TYPE_AIS_INDEX;
        prCmdInfo->u2InfoBufLen = (UINT_16)(CMD_HDR_SIZE + sizeof(CMD_ACCESS_REG));
        prCmdInfo->pfCmdDoneHandler = nicCmdEventQueryMcrRead;
        prCmdInfo->pfCmdTimeoutHandler = nicOidCmdTimeoutCommon;
        prCmdInfo->fgIsOid = TRUE;
        prCmdInfo->ucCID = CMD_ID_ACCESS_REG;
        prCmdInfo->fgSetQuery = FALSE;
        prCmdInfo->fgNeedResp = TRUE;
        prCmdInfo->fgDriverDomainMCR = FALSE;
        prCmdInfo->ucCmdSeqNum = ucCmdSeqNum;
        prCmdInfo->u4SetInfoLen = sizeof(CMD_ACCESS_REG);
        prCmdInfo->pvInformationBuffer = &rMcrInfo;
        prCmdInfo->u4InformationBufferLength = sizeof(rMcrInfo);

        // Setup WIFI_CMD_T (no payload)
        prWifiCmd = (P_WIFI_CMD_T)(prCmdInfo->pucInfoBuffer);
        prWifiCmd->u2TxByteCount_UserPriority = prCmdInfo->u2InfoBufLen;
        prWifiCmd->ucCID = prCmdInfo->ucCID;
        prWifiCmd->ucSetQuery = prCmdInfo->fgSetQuery;
        prWifiCmd->ucSeqNum = prCmdInfo->ucCmdSeqNum;    
        
        kalMemCopy(prWifiCmd->aucBuffer, &rMcrInfo, sizeof(rMcrInfo));

        wlanSendCommand(prAdapter, prCmdInfo);
        cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
    }
    else {
        HAL_MCR_RD(prAdapter,
               mcr_offset & BITS(2,31), //address is in DWORD unit
               &rMcrInfo.u4McrData);

        DBGLOG(INIT, TRACE, ("MCR Read: Offset = %#08lx, Data = %#08lx\n",
                    rMcrInfo.u4McrOffset, rMcrInfo.u4McrData));
    }    
#else
    rMcrInfo.u4McrOffset = mcr_offset;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanQueryInformation(prAdapter,
                     wlanoidQueryMcrRead,
                     (PVOID)&rMcrInfo,
                     sizeof(rMcrInfo),
                     &u4BufLen);
    
    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
#endif

    /* Check if access F/W Domain MCR (due to WiFiSYS is placed from 0x6000-0000 */
    if ( rMcrInfo.u4McrOffset & 0xFFFF0000 ){
		memset(aucBuffer, 0 , sizeof(wndrv_cmd_temp_buf));
        while(1)
        {
            /* Polling Query Transmitted count Event */
            if( nicRxWaitResponse(prAdapter,
                        g_hif_rx_event_port_num,
                        aucBuffer,
                        EVENT_HDR_SIZE + sizeof(EVENT_ACCESS_REG),
                        &u4RxPktLength) != WLAN_STATUS_SUCCESS) {
                MT5931_NEW_TRACE( MT5931_INFO_501, TRACE_INIT, "[READ]cr_offset_1: %x Fail!", mcr_offset );
                return 0;
            }
            else
            {
                prEvent = (P_WIFI_EVENT_T)aucBuffer;
                prRegStatus = (P_EVENT_ACCESS_REG)(prEvent->aucBuffer);                
                if( prEvent->ucEID == EVENT_ID_ACCESS_REG ) /* Ignore other EVENT besides EVENT_ACCESS_REG */
                {
                    MT5931_NEW_TRACE( MT5931_INFO_502, TRACE_INIT, "[READ]cr_offset: %x, cr_value: %x", mcr_offset, prRegStatus->u4Data );
                    return prRegStatus->u4Data;
                }
                else
                {
                    kalUdelay(50);
                    count++;
                }
                
                if ( count > CFG_RESPONSE_POLLING_TIMEOUT )
                {
                    MT5931_NEW_TRACE( MT5931_INFO_503, TRACE_INIT, "[READ]cr_offset_2: %x Fail!", mcr_offset );
                    return 0;
                }
            }
        }
    }
    else
    {
        MT5931_NEW_TRACE( MT5931_INFO_504, TRACE_INIT, "[READ]cr_offset: %x, cr_value: %x", mcr_offset, rMcrInfo.u4McrData );
        return rMcrInfo.u4McrData;
    }    
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This function will do MCR write.
*
* \param prAdapter      Pointer of Adapter Data Structure
*
* \retval WLAN_STATUS_SUCCESS   : MSDU was written to HW TX FIFO and be freed(SendComplete) immediately.
* \retval WLAN_STATUS_PENDING   : MSDU is pending in SW Queues to wait for previous frame finishing
*                                 their transmission and release buffer.
* \retval WLAN_STATUS_FAILURE   : Get failure while access HW TX FIFO or been rejected.
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS
wlanSetMcr(
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32 mcr_offset,
    IN UINT_32 mcr_value
    )
{
    WLAN_STATUS status;
    UINT_32 u4BufLen; 
	#if !defined(WCN_MAUI)
    P_GLUE_INFO_T prGlueInfo = prAdapter->prGlueInfo;
	#endif
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;

    DEBUGFUNC("wlanSetMcr");

    rMcrInfo.u4McrOffset = mcr_offset;
    rMcrInfo.u4McrData= mcr_value;

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    status = wlanSetInformation(prAdapter,
                       wlanoidSetMcrWrite,
                       (PVOID)&rMcrInfo,
                       sizeof(rMcrInfo),
                       &u4BufLen);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    MT5931_NEW_TRACE( MT5931_INFO_505, TRACE_INIT, "[WRITE]mcr_offset: %x, mcr_value: %x", mcr_offset, mcr_value );

    return status;
}

UINT_16 
wlanCopyWPSAttr(
    IN PUINT_8 pWpsIE, 
    IN PUINT_8 Dest, 
    IN UINT_16 size
    )
{
    const UINT_8* pos = NULL, *end = NULL;
    UINT_16 type = 0, len = 0;
    UINT_16 written = 0;
    UINT_8 ie_len = WPS_PROBE_RESP_IE(pWpsIE)->ucLength;

    pos = pWpsIE+ ELEM_HDR_LEN;
    end = pos + ie_len;
    pos += VENDOR_OUI_LEN;
    while (pos < end && written< size)
    {
       type = WPA_GET_BE16(pos);
       if (type <= 0x1000 || type >= 0x2000)
       {
           MT5931_TRACE(TRACE_PEER, MT5931_INFO_228, "bad WPS ATTR type=%x", type);
           break;
       }
       pos += 2;
       len = WPA_GET_BE16(pos);
       /*in theorey, it may be large than 255,
       but no AP use so large attribute*/
       if (len > 0xff)
       {
           MT5931_TRACE(TRACE_PEER, MT5931_INFO_229, "bad WPS ATTR len=%x", len);
           break;
       }
       pos += 2;
       if (type == ATTR_DEV_PASSWORD_ID ||
           type == ATTR_SELECTED_REGISTRAR ||
           type == ATTR_UUID_E)
       {
           if (size >= len + WPS_ATTR_HDR_LEN + written)
           {
               MT5931_TRACE(TRACE_PEER, MT5931_INFO_230, "copy attr=%x, len=%d", type, len);
               kal_mem_cpy(Dest + written, pos - 4, len + WPS_ATTR_HDR_LEN);
               written += len + WPS_ATTR_HDR_LEN;
           }
           else
           {
               MT5931_TRACE0(TRACE_PEER, MT5931_INFO_231, "attr is too large, discard");
           }
       }
       pos += len;
    }
    return written;
}
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is to setting the default Tx Power configuration
*
* @param prAdapter      Pointer of Adapter Data Structure
*
* @return zero      Unable to retrieve ECO version information
*         non-zero  ECO version (1-based)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanDefTxPowerCfg (
    IN P_ADAPTER_T      prAdapter
    )
{
    UINT_8 i;
    P_GLUE_INFO_T       prGlueInfo = prAdapter->prGlueInfo;
    P_SET_TXPWR_CTRL_T  prTxpwr;

    ASSERT(prGlueInfo);

    prTxpwr = &prGlueInfo->rTxPwr;

    prTxpwr->c2GLegacyStaPwrOffset = 0;
    prTxpwr->c2GHotspotPwrOffset = 0;
    prTxpwr->c2GP2pPwrOffset = 0;
    prTxpwr->c2GBowPwrOffset = 0;
    prTxpwr->c5GLegacyStaPwrOffset = 0;
    prTxpwr->c5GHotspotPwrOffset = 0;
    prTxpwr->c5GP2pPwrOffset = 0;
    prTxpwr->c5GBowPwrOffset = 0;
    prTxpwr->ucConcurrencePolicy = 0;
    for (i=0; i<3;i++)
        prTxpwr->acReserved1[i] = 0;

    for (i=0; i<14;i++)
        prTxpwr->acTxPwrLimit2G[i] = 63;

    for (i=0; i<4;i++)
        prTxpwr->acTxPwrLimit5G[i] = 63;

    for (i=0; i<2;i++)
        prTxpwr->acReserved2[i] = 0;

}


/*----------------------------------------------------------------------------*/
/*!
* @brief This function is to
*        set preferred band configuration corresponding to network type
*
* @param prAdapter      Pointer of Adapter Data Structure
* @param eBand          Given band 
* @param eNetTypeIndex  Given Network Type
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID
wlanSetPreferBandByNetwork (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BAND_T eBand,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
    )
{
    ASSERT(prAdapter);
    ASSERT(eBand <= BAND_NUM);
    ASSERT(eNetTypeIndex <= NETWORK_TYPE_INDEX_NUM);

    /* 1. set prefer band according to network type */
    prAdapter->aePreferBand[eNetTypeIndex] = eBand;

    /* 2. remove buffered BSS descriptors correspondingly */
    if(eBand == BAND_2G4) {
        scanRemoveBssDescByBandAndNetwork(prAdapter, BAND_5G, eNetTypeIndex);
    }
    else if(eBand == BAND_5G) {
        scanRemoveBssDescByBandAndNetwork(prAdapter, BAND_2G4, eNetTypeIndex);
    }

    return;
}

