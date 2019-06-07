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
//#include "kal_release.h"
#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"
#include "syscomp_config.h"
#include "task_config.h"
#include "task_main_func.h"
#include "stack_ltlcom.h"

/*----------------------------*/
#include "stack_timer.h"  /* Stack timer */

/*----------------------------*/

#include "fctycomp_config.h"



/*----------------------------*/
//20081114 add by saker for nvram
#include "nvram_struct.h"           /* for NVRAM read/write */
#include "Nvram_user_defs.h"
#include "custom_nvram_editor_data_item.h"
/*----------------------------*/
#include "kal_trace.h"
#endif
//#include "syscomp_config.h" /*comptask_info_struct*/
//#include "multiboot_config.h"/*MTK_FACTORY_MODE*/
/*----------------------------*/
#if 0
#include "wndrv_cnst.h"
#include "wndrv_cid.h"
#include "wndrv_cal.h"
#endif
#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"

#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"

#include "wndrv_private.h"
#include "wndrv_os_private.h"

/*----------------------------*/
#include "precomp.h"

/*----------------------------*/
/* include after precomp.h due to MP_ADAPTER */
#include "wndrv_context.h"
//todo
//#if defined(__WIFI_SUPPORT__)&&(__MTK_TARGET__)
#include "wndrv_api.h"
//#endif
#include "wndrv_kal.h"
#include "wifi_common_config.h"
//#include "l1_interface.h"
//#include "dcl.h"


/* ------------------------------------------------------------------------- */
wndrv_context_struct wndrv_context;
GLUE_INFO_T wndrv_glue_info;

ENUM_PARAM_WNDRV_ES_T eParamWndrvEs[PARAM_WNDRV_ES_NUM];

extern struct _ADAPTER_T wndrv_adapter_pool;
/*
extern kal_bool wndrv_get_eint_polarity( void );
extern void WiFi_WriteGPIO(char val, unsigned char pin);
extern void LCD_Physical_Interface_Lock(void);
extern void get_MTD_lock(void);
extern void free_MTD_lock(void);
extern void LCD_Physical_Interface_Unlock(void);
extern void WiFi_InitGPIO(char direction, unsigned char pin);
extern void EINT_RestoreMask(kal_uint8 eintno, kal_uint32 val);
*/
extern char INFORM_WIFI_DEGRADE_POWER(void);
extern kal_uint32 WiFi_Query_Customer_Seting_Version(void);
extern kal_bool wndrv_get_eeprom_existence(void);
extern kal_uint32 wndrv_get_eeprom_size(void);
extern kal_uint16 INT_BootMode(void);
extern kal_int32 wifi_config_get_mac_address(kal_uint8 port, kal_uint8 *address);

extern kal_bool       WNDRV_CHIP_isOK;
extern kal_uint32     Protocol_Status4WLAN;
#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
extern UINT_8 const *wndrv_firmware_p;
extern UINT_32 wndrv_firmware_size;
#endif
void wndrv_ps_ctl_handler(void);

#if DBG
UINT_8  aucDebugModule[DBG_MODULE_NUM];
UINT_32 u4DebugModule = 0;
#endif /* DBG */
/* customer setting */
#define CUSTOM_SETTING_VER1 9
extern kal_int32 wifi_driver_customer_setting[];
extern DOMAIN_INFO_ENTRY arSupportedRegDomains[];
void wndrv_load_customer_setting(void);

/*******************************************************************************
*
*  Global Functions Exported to WNDRV Task only
*
*******************************************************************************/

void wndrv_evshed_timer_handler(void *msg_ptr)
{

    P_ENUM_PARAM_WNDRV_ES es = (P_ENUM_PARAM_WNDRV_ES)msg_ptr;
    P_ADAPTER_T prAdapter = wndrv_context.adpt_p;
#if CFG_SAWLESS_SW_WORKAROUND
    static BOOL fgIsPowerDec = 0;
#endif
    P_AIS_BSS_INFO_T prAisBssInfo;

    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_TXRX, "Timer timeout ID = %x", *es);
    if (1 /* WNDRVTimerEnable */) {
        switch (*es) {
            case PARAM_WNDRV_ES_TEST_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                //MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_314, "PARAM_WNDRV_ES_TEST_TIMER timeout");
                break;

            case PARAM_WNDRV_ES_SCAN_COMPLETE_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                wndrv_supc_site_survey_ind_handler();

                break;

            case PARAM_WNDRV_ES_CONNETION_FAIL_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                if (wndrv_context.glue_p->eParamMediaStateIndicated != PARAM_MEDIA_STATE_CONNECTED) {
                    wndrv_supc_network_connect_fail_ind_handler();
                }
                break;

            case PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                RFTool_WiFi_Query_PktRx();

                kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_RFTEST_RX_STAT_TIMER],
                                      KAL_TICKS_1_SEC);
                break;

            case PARAM_WNDRV_ES_STAT_IND_TIMER:

            {
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                wndrv_supc_network_statistics_ind_handler();
                kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_STAT_IND_TIMER],
                                      KAL_TICKS_1_SEC * 5);
            }
            break;


            case PARAM_WNDRV_ES_PS_CTL_TIMER:

            {
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;

                wndrv_ps_ctl_handler();

                kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_PS_CTL_TIMER],
                                      KAL_TICKS_500_MSEC);
            }
            break;


            /* MT5931 module timers */
            case PARAM_WNDRV_ES_CNM_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                //MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_315, "PARAM_WNDRV_ES_CNM_TIMER timeout");
                cnmTimerDoTimeOutCheck(wndrv_context.adpt_p);
                break;
#if 0
            case PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                /* Set PS mode to config in adapter */
                MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "PARAM_WNDRV_ES_ADD_KEY_FAIL_TIMER timeout");
                nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX,
                                          wndrv_context.adpt_p->u4PowerMode, FALSE);
                break;
#endif
            case PARAM_WNDRV_ES_KEEP_CAM_TIMER:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                /* Set PS mode to config in adapter */
                nicConfigPowerSaveProfile(wndrv_context.adpt_p, NETWORK_TYPE_AIS_INDEX,
                                          (PARAM_POWER_MODE)(wndrv_context.adpt_p->u4PowerMode), FALSE);
                break;
#if CFG_SAWLESS_SW_WORKAROUND
            case PARAM_WNDRV_ES_PWR_CTRL_TIMR:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                if (wndrv_context.glue_p->fgIsAdaptStopped == FALSE) {
                    // MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_316, "PARAM_WNDRV_ES_PWR_CTRL_TIMR");
#if 1
                    if (INFORM_WIFI_DEGRADE_POWER()) {
                        if (fgIsPowerDec == 0) {
                            /* decrease power */
                            nicTxPollingResource(prAdapter, TC4_INDEX);
                            wlanUpdateTxPower(
                                prAdapter,
                                (prAdapter->prGlueInfo->c11b_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]),
                                (prAdapter->prGlueInfo->c11g_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]),
                                (prAdapter->prGlueInfo->c11n_bw20_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]),
                                (prAdapter->prGlueInfo->c11n_bw40_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR])
                            );
                            fgIsPowerDec = 1;
                            // MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_317, "WIFI_DEGRADE_POWER: degrade %d dB!", wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]);
                        }
                    } else {
                        if (fgIsPowerDec == 1) {
                            /* resume power */
                            nicTxPollingResource(prAdapter, TC4_INDEX);
                            wlanUpdateTxPower(
                                prAdapter,
                                prAdapter->prGlueInfo->c11b_tx_pwr,
                                prAdapter->prGlueInfo->c11g_tx_pwr,
                                prAdapter->prGlueInfo->c11n_bw20_tx_pwr,
                                prAdapter->prGlueInfo->c11n_bw40_tx_pwr
                            );
                            fgIsPowerDec = 0;
                            // MT5931_TRACE0(TRACE_GROUP_10, MT5931_INFO_318, "WIFI_DEGRADE_POWER: resume!");
                        }
                    }
#else
                    if (fgIsPowerDec == 0) {
                        /* decrease power */
                        nicTxPollingResource(prAdapter, TC4_INDEX);
                        wlanUpdateTxPower(
                            prAdapter,
                            (prAdapter->prGlueInfo->c11b_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]),
                            (prAdapter->prGlueInfo->c11g_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]),
                            (prAdapter->prGlueInfo->c11n_bw20_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]),
                            (prAdapter->prGlueInfo->c11n_bw40_tx_pwr - wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR])
                        );
                        fgIsPowerDec = 1;
                        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "WIFI_DEGRADE_POWER: degrade %d dB!", wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR]);
                    } else {
                        /* resume power */
                        nicTxPollingResource(prAdapter, TC4_INDEX);
                        wlanUpdateTxPower(
                            prAdapter,
                            prAdapter->prGlueInfo->c11b_tx_pwr,
                            prAdapter->prGlueInfo->c11g_tx_pwr,
                            prAdapter->prGlueInfo->c11n_bw20_tx_pwr,
                            prAdapter->prGlueInfo->c11n_bw40_tx_pwr
                        );
                        fgIsPowerDec = 0;
                        MT5931_TRACE(TRACE_GROUP_10, MT5931_INFO_, "WIFI_DEGRADE_POWER: resume!");
                    }
#endif
                    kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_PWR_CTRL_TIMR],
                                          wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_PWR_CTRL_TIMEOUT] * KAL_TICKS_100_MSEC);
                }
                break;
#endif

            case PARAM_WNDRV_ES_BEACON_TIMR:
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;
                MT5931_TRACE0(TRACE_INFO, MT5931_INFO_319, "PARAM_WNDRV_ES_BEACON_TIMR timeout");

                prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
                if (prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {
                    aisBssBeaconTimeout(prAdapter);
                }

                break;
            case PARAM_WNDRV_ES_SMART_CONFIG_TIMER: {
#ifdef __WIFI_SNIFFER_SUPPORT__

                CMD_SET_SNIFFER_MODE_T rSnifferInfo;
                wndrv_context.glue_p->event_id[*es] = (eventid) 0;

                //1. change sniffer channel
                if (wndrv_context.adpt_p->u1SnifferOnOff != (UINT_8)PARA_SNIFFER_MODE_ON_WITHOUT_PARA) {
                    break;
                }

                if (prAdapter->smart_config.u1ListenChannelIndex < sizeof(prAdapter->smart_config.u1ListenChannelTable) - 1) {
                    prAdapter->smart_config.u1ListenChannelIndex++;
                } else {
                    prAdapter->smart_config.u1ListenChannelIndex = 0;
                }

                //2. set sniffer mode
                rSnifferInfo.ucOnOffSwitch = 1;
                rSnifferInfo.u2MaxPacketLength = 100;
                rSnifferInfo.u2MinPacketLength = 25;
                rSnifferInfo.ucChannelNum = prAdapter->smart_config.u1ListenChannelTable[prAdapter->smart_config.u1ListenChannelIndex];
                rSnifferInfo.ucSnifferFilterOptions = 0x00100000;//muticast
                rSnifferInfo.cMinRssi = 25;
                rSnifferInfo.cMaxRssi = 215;


                MT5931_NEW_TRACE(MT5931_INFO_610, TRACE_INFO, "smart_connect:OnOff(%d),Channel(%d),FilterOptions(0x%x),MinRcpi(%d),MaxRssi(%d),MinLen(%d),MaxLen(%d)", rSnifferInfo.ucOnOffSwitch, \
                                 rSnifferInfo.ucChannelNum, rSnifferInfo.ucSnifferFilterOptions, rSnifferInfo.cMinRssi, rSnifferInfo.cMaxRssi, \
                                 rSnifferInfo.u2MinPacketLength, rSnifferInfo.u2MaxPacketLength);

                wlanSendSetQueryCmd(prAdapter,
                                    CMD_ID_SNIFF_REQ,
                                    TRUE,
                                    FALSE,
                                    TRUE,
                                    nicCmdEventSetCommon,
                                    nicOidCmdTimeoutCommon,
                                    sizeof(rSnifferInfo),
                                    (PUINT_8)& rSnifferInfo,
                                    NULL,
                                    0
                                   );

                //3.start timer
                kal_event_start_timer(&wndrv_context.glue_p->prParamWndrvEs[PARAM_WNDRV_ES_SMART_CONFIG_TIMER], 4 * KAL_TICKS_50_MSEC);
#endif
            }
            break;
            default:
                WNDRV_ASSERT(0);
        }

        //wndrv_context.glue_p->event_id[*es] =(eventid) 0;  lcko: cannot set to 0 here or all eventid will be cleaned
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_ps_ctl_handler
*
* DESCRIPTION
*   Check if we can enter PS mode
*   Check if we should indicate BT to turn off PTA function
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
//extern kal_uint8 force_reset;
//extern void wlanDevMonitorFun(void);
void wndrv_ps_ctl_handler(void)
{
    //OS_SYSTIME currentTime;
    P_BSS_INFO_T prAisBssInfo = &(wndrv_context.adpt_p->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    UINT_32                        u4Len;
    PTA_IPC_T                      PtaIpc;
    UINT_32                        fgWiFiCoexParam;

    if (prAisBssInfo->eConnectionState != PARAM_MEDIA_STATE_CONNECTED) {
        return;
    }

    if (wndrv_context.adpt_p->rPtaInfo.fgSingleAntenna == FALSE) {
        /* NOTE(lcko): We use the same timer to enable/disable PTA */

        if ((Protocol_Status4WLAN & PROTOCOL_SOCKETOPEN) == PROTOCOL_SOCKETOPEN) {
            // BT-Coexistence
            wndrv_WiFi_channel_ind_handler(KAL_FALSE, 0, DEFAULT_HB_VALUE, KAL_TRUE, KAL_TRUE);

            //4 <0> set 1 wire mode to mode7
            PtaIpc.ucCmd = BT_CMD_PROFILE;
            PtaIpc.ucLen = sizeof(PTA_IPC_T);

            fgWiFiCoexParam = (VAR_WMT_CONFIG_WIFI_1WIRE_MODE | BT_EXT_1_WIRE_MODE << 8);

            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8)(fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8)(fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8)(fgWiFiCoexParam >> 24);

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                               (PVOID)&PtaIpc,
                               sizeof(PtaIpc),
                               (PUINT_32)&u4Len);
            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);

            //4 <1> set PTA
            PtaIpc.ucCmd = BT_CMD_PROFILE;
            PtaIpc.ucLen = sizeof(PTA_IPC_T);

            fgWiFiCoexParam = (VAR_WMT_CONFIG_COEX_SETTING | WMT_COEX_CONFIG_BT_ON | WMT_COEX_CONFIG_ENABLE_PTA);   // WIFI_PTA_ENABLE
            fgWiFiCoexParam |= WMT_COEX_CONFIG_DUAL_ANT_MODE;

            GLUE_ACQUIRE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
            PtaIpc.u.aucBTPParams[3] = (UINT_8) fgWiFiCoexParam;
            PtaIpc.u.aucBTPParams[2] = (UINT_8)(fgWiFiCoexParam >> 8);
            PtaIpc.u.aucBTPParams[1] = (UINT_8)(fgWiFiCoexParam >> 16);
            PtaIpc.u.aucBTPParams[0] = (UINT_8)(fgWiFiCoexParam >> 24);

            wlanSetInformation(wndrv_context.adpt_p,
                               (PFN_OID_HANDLER_FUNC)wlanoidSetBT,
                               (PVOID)&PtaIpc,
                               sizeof(PtaIpc),
                               (PUINT_32)&u4Len);
            GLUE_RELEASE_SPIN_LOCK(wndrv_context.glue_p, SPIN_LOCK_FSM);
        } else {
            // BT-Coexistence
            wndrv_WiFi_channel_ind_handler(KAL_FALSE, 0, DEFAULT_HB_VALUE, KAL_TRUE, KAL_FALSE);
        }

#if 0
        GET_CURRENT_SYSTIME(&currentTime);
        // James modify for PTA on/off
        if (CHECK_FOR_TIMEOUT(currentTime,
                              wndrv_glue_info.lastConnectCheckTime,
                              MSEC_TO_SYSTIME(WNDRV_SELF_PTA_CONNECT_KEEP_TIME))) {
            wndrv_glue_info.lastConnectCheckTime = currentTime;
            if (wndrv_glue_info.SelfPTAOnScore < WNDRV_SELF_PTA_ON_THRESHOLD - WNDRV_SELF_PTA_CONNECT_SCORE) {
                WNDRV_TRACE1(TRACE_GROUP_1, INFO_WNDRV_RX_DEAUTH_PTA_SCORE,
                             "SelfPTAOnScore = %d",
                             wndrv_glue_info.SelfPTAOnScore);
                wndrv_glue_info.SelfPTAOnScore += WNDRV_SELF_PTA_CONNECT_SCORE;
            } else if (wndrv_glue_info.SelfPTAOnScore >= WNDRV_SELF_PTA_ON_THRESHOLD - WNDRV_SELF_PTA_CONNECT_SCORE) {
                wndrv_glue_info.SelfPTAOnScore = WNDRV_SELF_PTA_ON_THRESHOLD;
            }

            if ((wndrv_glue_info.bSelfPTAOn == FALSE) &&
                    (wndrv_glue_info.SelfPTAOnScore == WNDRV_SELF_PTA_ON_THRESHOLD)) {
                //NIC_MCR_WR(adapter_p, MCR_CWBR, WBR_BLUETOOTH_1ST_PRIORITY_EN | WBR_COEXIST_EN);

                {
                    PARAM_CUSTOM_BT_COEXIST_T set;
                    kal_uint32             setInfoLen;

                    set.rPtaParam.u4BtCR0 = wndrv_context.adpt_p->rConnSettings.rPtaParam.u4BtCR0;//DEFAULT_BTCR0;//DEFAULT_BTCR0_ON_VALUE;  // enable PTA
                    set.rPtaParam.u4BtCR1 = wndrv_context.adpt_p->rConnSettings.rPtaParam.u4BtCR1;//DEFAULT_BTCR1_VALUE;
                    set.rPtaParam.u4BtCR2 = wndrv_context.adpt_p->rConnSettings.rPtaParam.u4BtCR2;//DEFAULT_BTCR2_VALUE;
                    set.rPtaParam.u4BtCR3 = wndrv_context.adpt_p->rConnSettings.rPtaParam.u4BtCR3;//DEFAULT_BTCR3_VALUE;
                    set.u4IsEnableTxAutoFragmentForBT = 0;//DEFAULT_TX_AUTO_FRAGMENT_FOR_BT;
                    set.eBTCoexistWindowType = 0;//DEFAULT_BT_COEXIST_WINDOW_TYPE;

                    wlanSetInformation(wndrv_context.adpt_p,
                                       (PFN_OID_HANDLER_FUNC)wlanoidSetBtCoexistCtrl,
                                       &set,
                                       sizeof(PARAM_CUSTOM_BT_COEXIST_T),
                                       (PUINT_32)&setInfoLen);
                }

                wndrv_glue_info.bSelfPTAOn = TRUE;
                WNDRV_TRACE3(TRACE_GROUP_1, INFO_WNDRV_SWITCH_PTA,
                             "SelfPTAOnScore = %d, Threshold = %d, Turn on PTA = %d",
                             wndrv_glue_info.SelfPTAOnScore,
                             WNDRV_SELF_PTA_OFF_THRESHOLD,
                             1);
            }
        }
#endif
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_get_poweron_mode
*
* DESCRIPTION
*   Query the power-on mode, normal or meta mode.
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
//todo
#define MTK_NORMAL_MODE     0
#define MTK_FACTORY_MODE    1


kal_uint16 INT_BootMode(void)
{
    return MTK_NORMAL_MODE;
}

void wndrv_get_poweron_mode(void)
{
    wndrv_glue_info.meta_mode = (INT_BootMode() == MTK_FACTORY_MODE) ? KAL_TRUE : KAL_FALSE;

    WNDRV_TRACE1(TRACE_INIT, INFO_WNDRV_MISC_META_MODE,
                 "WNDRV_MISC: meta_mode=%d", wndrv_glue_info.meta_mode);
}

/*******************************************************************************
* FUNCTION
*   wndrv_init
*
* DESCRIPTION
*   WiFi task initialization.
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
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code = "SECONDARY_ROCODE"
#endif
void wndrv_init(void)
{
    //static kal_int32 status;
    kal_int32 i;
    //kal_int32 u4PsMode;
    /* Eint Output setting */
    /* LED setting */
    //kal_uint32   u4LedSetting;
    P_REG_INFO_T prRegInfo;
    //UINT_32     reg_value = 0;

    /****************************************************************************
    *  1. Timer initialization
    ****************************************************************************/
#ifdef __WAPI_SUPPORT__
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "WNDRV LIB : WCN_INTERNAL MT5931 WAPI %s %s", __DATE__, __TIME__);
#else
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "WNDRV LIB : WCN_INTERNAL MT5931 %s %s", __DATE__, __TIME__);
#endif

    //stack_init_timer (&wndrv_glue_info.evsched_timer, "WiFi event schdule timer", MAP_MOD_WNDRV );


    /****************************************************************************
    *  2. Glue Info initialization
    ****************************************************************************/
    /*
    wndrv_glue_info.event_scheduler_ptr = new_evshed(&wndrv_glue_info.evsched_timer,
                                                   wndrv_evsched_start_base_timer,
                                                   wndrv_evsched_stop_base_timer,
                                                   0,
                                                   kal_evshed_get_mem,
                                                   kal_evshed_free_mem,
                                                   255);
     */
    wndrv_glue_info.event_scheduler_ptr = (event_scheduler *)evshed_create("WiFi event schdule timer", MOD_WNDRV, 0, 0);

    /* Event schedule timer init */
    for (i = PARAM_WNDRV_ES_TEST_TIMER; i < PARAM_WNDRV_ES_NUM; i++) {
        eParamWndrvEs[i] = (ENUM_PARAM_WNDRV_ES_T)i;
    }

    wndrv_glue_info.prParamWndrvEs = &eParamWndrvEs[0];

#if 0   //TODO(Nelson): Set OID timeout checker timer
    /* initialize timer for OID timeout checker */
    kalOsTimerInitialize(prGlueInfo, kalTimeoutHandler);
#endif

    for (i = 0; i < PARAM_WNDRV_ES_NUM; i ++) {
        wndrv_glue_info.event_id[i] = (eventid) 0;
    }

    wndrv_glue_info.eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
    wndrv_glue_info.ePowerState = ParamDeviceStateD0;
    //wndrv_glue_info.fgIsMacAddrOverride = FALSE;   //NOTE(Nelson): Assign to TRUE while get MAC address from NVRAM
    wndrv_glue_info.u4ReadyFlag = 0;

    QUEUE_INITIALIZE(&wndrv_glue_info.rCmdQueue);
    QUEUE_INITIALIZE(&wndrv_glue_info.rTxQueue);

    wndrv_glue_info.engineer_mode = KAL_FALSE;

    /****************************************************************************
    *  2. Load Customer Setting
    ****************************************************************************/
    wndrv_context.glue_p = &wndrv_glue_info;
    wndrv_load_customer_setting();

    /****************************************************************************
    *  3. Adapter Create
    ****************************************************************************/
    wndrv_context.adpt_p = (P_ADAPTER_T) wlanAdapterCreate(wndrv_context.glue_p);
    wndrv_context.adpt_p->prGlueInfo = wndrv_context.glue_p;
    wndrv_context.glue_p->prAdapter = wndrv_context.adpt_p;
    WNDRV_EXT_ASSERT(wndrv_context.adpt_p, "wndrv_context.adpt_p can't be NULL", 0, 0, 0);

    wndrv_context.glue_p->fgEintPolarity = (kal_bool)(wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_EINT_POLARITY]);

#if CFG_TCP_IP_CHKSUM_OFFLOAD
    wndrv_context.adpt_p->u4CSUMFlags = (CSUM_OFFLOAD_EN_TX_TCP | CSUM_OFFLOAD_EN_TX_UDP | CSUM_OFFLOAD_EN_TX_IP);
#endif


    /****************************************************************************
    *  4. WNDRV software initialization
    ****************************************************************************/
    /* Memory pre-allocation */
    kalInitIOBuffer();

    prRegInfo = &wndrv_context.glue_p->rRegInfo;
    //kalMemSet(prRegInfo, 0, sizeof(REG_INFO_T));
    kalMemSet(prRegInfo, 0, OFFSET_OF(REG_INFO_T, rDomainInfo));
    prRegInfo->uc2G4BwFixed20M = 0;
    prRegInfo->uc5GBwFixed20M = 0;
    prRegInfo->ucEnable5GBand = 0;
#if CFG_ENABLE_FW_DOWNLOAD
    prRegInfo->u4StartAddress = CFG_FW_START_ADDRESS;
    prRegInfo->u4LoadAddress =  CFG_FW_LOAD_ADDRESS;
#endif
//    prRegInfo->u4PowerMode = CFG_INIT_POWER_SAVE_PROF;

#if DBG
    /* Initialize DEBUG CLASS of each module */
    for (i = 0; i < DBG_MODULE_NUM; i++) {
        aucDebugModule[i] = DBG_CLASS_ERROR | \
                            DBG_CLASS_WARN | \
                            DBG_CLASS_STATE | \
                            DBG_CLASS_TRACE | \
                            DBG_CLASS_EVENT | \
                            DBG_CLASS_INFO | \
                            DBG_CLASS_LOUD | \
                            DBG_CLASS_TEMP;
    }
#endif

    /* Set adapter is stop */
    wndrv_context.glue_p->fgIsAdaptStopped = TRUE;

    /****************************************************************************
    *  5. Read NVRAM data to Adapter
    ****************************************************************************/
    /* Load NVRAM content to REG_INFO_T */
#if 0   //TODO(Nelson): Read NVRAM content
    glLoadNvram(wndrv_context.glue_p, prRegInfo);
#else
    /* Default values */
    prRegInfo->u4StartFreq = 2462000;   //channel 11
#endif
    prRegInfo->u4PowerMode = wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_PS_MODE];

    /****************************************************************************
     *  6. Check WLAN chip is ready
     ****************************************************************************/
#if 0
    LB_WIFI_Chip_PowerOn();
    glSdioOpen();
    glSdioInit();

    HAL_MCR_RD(wndrv_context.adpt_p, WCIR, &reg_value);

    /* Verify chip ID */
    if ((reg_value & 0xffff) != 0x5931) {
        WNDRV_CHIP_isOK = KAL_FALSE;

        MT5931_WAP_TRACE(MOD_WNDRV, TRACE_ERROR, "WNDRV_CHIP_isOK is FALSE\n");
    } else {
        WNDRV_CHIP_isOK = KAL_TRUE;
    }

    /* Get Revision ID */
    wndrv_context.adpt_p->ucRevID = (UINT_8)(((reg_value & WCIR_REVISION_ID) >> 16) & 0xF) ;
#if !CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    if (wndrv_context.adpt_p->ucRevID != MT5931_E2_REVERSION_ID) {
        //WiFi_WriteGPIO(0, gpio_wifi_enable_pin);  /* GPIO40 output 0: disable PMU_EN */
        //WiFi_WriteGPIO(0, gpio_wifi_ext_rst_pin);  /* GPIO43 output 0: disable SYS_RST */
    }
#endif
#endif

#ifdef __WAPI_SUPPORT__
    /* WAPI Flag default is off*/
    wndrv_context.adpt_p->fgUseWapi = KAL_FALSE;
#endif /*__WAPI_SUPPORT__*/


    /****************************************************************************
     *  7. Enter WNDRV test mode if power-on mode is meta mode
     ****************************************************************************/
    wndrv_glue_info.meta_inject_mode = KAL_FALSE;    /*Nelson(Note): For meta inject msg use: ture:send SAP to ABM, false:send SAP to L1 */
    if (wndrv_glue_info.meta_mode) {
        wndrv_test_set_test_mode();
    }
#if CFG_ALWAYS_PWR_ON_SW_WORKAROUND
    else {
        if (WNDRV_CHIP_isOK == KAL_TRUE) {
            wlanAdapterStart(wndrv_context.adpt_p, prRegInfo, (PVOID)wndrv_firmware_p, wndrv_firmware_size);
            //wndrv_context.glue_p->fgIsAdaptStopped = FALSE;
        }
    }
#endif
}
#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma arm section code
#endif
/*******************************************************************************
* FUNCTION
*   wndrv_interrupt_handler
*
* DESCRIPTION
*   Handler interrupt in deferred task.
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
void wndrv_interrupt_handler(ilm_struct *ilm_ptr)
{
    P_GLUE_INFO_T   prGlueInfo = wndrv_context.glue_p;
    BOOLEAN         fgNeedHwAccess = FALSE;

#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("Rin", SA_start);
#endif

    GLUE_SPIN_LOCK_DECLARATION();

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo,
                           SPIN_LOCK_FSM);

    nicDisableInterrupt(prGlueInfo->prAdapter);//HISR improve for throughput

    if (fgNeedHwAccess == FALSE) {
        fgNeedHwAccess = TRUE;
        wlanAcquirePowerControl(prGlueInfo->prAdapter);
    }

    /* (NOTE): the Wi-Fi interrupt is already disabled in wlanISR(),
                        so we set the flag only to enable the interrupt later  */
    prGlueInfo->prAdapter->fgIsIntEnable = FALSE;

    wlanIST(prGlueInfo->prAdapter);

    /* Release ownership to enter power save mode */
    if (fgNeedHwAccess == TRUE) {
        wlanReleasePowerControl(wndrv_context.adpt_p);
    }

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo,
                           SPIN_LOCK_FSM);

#if CFG_ENABLE_THROUGHPUT_DBG
    SLA_CustomLogging("Rin", SA_stop);
#endif
}


/*******************************************************************************
* FUNCTION
*   wndrv_timer_expiry_handler
*
* DESCRIPTION
*   Timer expiry handler.
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
void wndrv_timer_expiry_handler(ilm_struct *ilm_ptr)
{
#if 0
    stack_timer_struct *stack_timer_msg_ptr;

    stack_timer_msg_ptr = (stack_timer_struct *) ilm_ptr->local_para_ptr;

    /* call the corresponding timeout handler acoording to the timer index */
    switch (stack_timer_msg_ptr->timer_indx) {
#if 0
        case WNDRV_TIMER_EXPIRY:
            if (stack_is_time_out_valid(&wndrv_context.timer)) {
                wlanDoTimeOutCheck(wndrv_context.adpt_p);
            }

            stack_process_time_out(&wndrv_context.timer);
            break;
#endif
        case WNDRV_EVSHED_EXPIRY:
            if (stack_is_time_out_valid(&wndrv_context.glue_p->evsched_timer)) {
                evshed_timer_handler(wndrv_context.glue_p->event_scheduler_ptr);
            }

            stack_process_time_out(&wndrv_context.glue_p->evsched_timer);
            break;

        default:
            WNDRV_EXT_ASSERT(0, "wndrv_timer_expiry_handler: Unexpected stack_timer_msg_ptr->timer_indx",
                             stack_timer_msg_ptr->timer_indx, 0, 0);
            break;
    }
#endif
    evshed_timer_handler(wndrv_context.glue_p->event_scheduler_ptr);
}

/*******************************************************************************
*
*  Global Functions Exported to FT & WNDRV Tasks
*
*******************************************************************************/
/*******************************************************************************
* FUNCTION
*   wndrv_cal_macaddr
*
* DESCRIPTION
*   Calibration function to write MAC address.
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
kal_uint8  osl_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};


kal_int32 osl_ext_read_mac(kal_uint8 *pMac)
{
    if (wndrv_glue_info.fgIsMacAddrOverride == FALSE) {
        osl_mac[4] = (kal_uint8)(random() & 0xff);
        osl_mac[5] = (kal_uint8)(random() & 0xff);
        wndrv_glue_info.fgIsMacAddrOverride = TRUE;
    }
    kal_mem_cpy(pMac, osl_mac, 6);
    return 0;
}

void wndrv_cal_macaddr(wndrv_cal_mac_addr_struct *mac)
{
    kal_uint8  *data = mac->mac_addr;
    kal_uint8  flash_data[6];
    kal_int32  ret = 0;

    WNDRV_TRACE6(TRACE_INIT, INFO_WNDRV_MISC_MACADDR,
                 "WNDRV_MISC: cal_macaddr=0x%x %x %x %x %x %x",
                 data[0],
                 data[1],
                 data[2],
                 data[3],
                 data[4],
                 data[5]);
    //ret = osl_ext_read_mac(&flash_data[0]);
    ret = wifi_config_get_mac_address(0, &flash_data[0]);
    WNDRV_TRACE6(TRACE_INIT, INFO_WNDRV_MISC_MACADDR,
                 "WNDRV_MISC: cal_macaddr=0x%x %x %x %x %x %x",
                 flash_data[0],
                 flash_data[1],
                 flash_data[2],
                 flash_data[3],
                 flash_data[4],
                 flash_data[5]);

    if (ret == 0) { //use mac add read from flash
        data = &flash_data[0];
    }

    memcpy(wndrv_glue_info.rMacAddrOverride, data, MAC_ADDR_LEN);
    wndrv_glue_info.fgIsMacAddrOverride = TRUE;
}


/*******************************************************************************
* FUNCTION
*   wndrv_cal_txpwr_2400M
*
* DESCRIPTION
*   Calibration function to write 2.4G band TX power level.
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
void wndrv_cal_txpwr_2400M(wndrv_cal_txpwr_2400M_struct *txpwr)
{
    WNDRV_TRACE0(TRACE_INIT, INFO_WNDRV_MISC_TXPWR_2400M, "WNDRV_MISC: cal_2.4g_pwr");

    /* MT5931 tx target power control */
    wndrv_glue_info.c11b_tx_pwr = txpwr->CCKTxPWR[ 0 ] & BITS(0, 6);
    wndrv_glue_info.c11g_tx_pwr = txpwr->OFDMTxPWR[ 0 ] & BITS(0, 6);
    wndrv_glue_info.c11n_bw20_tx_pwr = txpwr->OFDMTxPWR[ 2 ] & BITS(0, 6);
    wndrv_glue_info.c11n_bw40_tx_pwr = txpwr->OFDMTxPWR[ 4 ] & BITS(0, 6);

    /* MT5931 band edge tx target power control */
    wndrv_glue_info.cCck_band_edge_tx_pwr = txpwr->CCKTxPWR[ 1 ] & BITS(0, 6);
    wndrv_glue_info.cOfdm_band_edge_tx_pwr = txpwr->OFDMTxPWR[ 1 ] & BITS(0, 6);
    wndrv_glue_info.cOfdm_bw20_band_edge_tx_pwr = txpwr->OFDMTxPWR[ 3 ] & BITS(0, 6);
    wndrv_glue_info.cOfdm_bw40_band_edge_tx_pwr = txpwr->OFDMTxPWR[ 5 ] & BITS(0, 6);

    /* MT5931 tx channel power offset */
    if (txpwr->CCKTxPWR[ 2 ] & BIT(6)) {    /* positive */
        wndrv_glue_info.cCh_offset_L = txpwr->CCKTxPWR[ 2 ] & BITS(0, 5);
    } else { /* negative */
        wndrv_glue_info.cCh_offset_L = 0 - (txpwr->CCKTxPWR[ 2 ] & BITS(0, 5));
    }

    if (txpwr->CCKTxPWR[ 3 ] & BIT(6)) {
        wndrv_glue_info.cCh_offset_M = txpwr->CCKTxPWR[ 3 ] & BITS(0, 5);
    } else {
        wndrv_glue_info.cCh_offset_M = 0 - (txpwr->CCKTxPWR[ 3 ] & BITS(0, 5));
    }

    if (txpwr->CCKTxPWR[ 4 ] & BIT(6)) {
        wndrv_glue_info.cCh_offset_H = txpwr->CCKTxPWR[ 4 ] & BITS(0, 5);
    } else {
        wndrv_glue_info.cCh_offset_H = 0 - (txpwr->CCKTxPWR[ 4 ] & BITS(0, 5));
    }
}

/*******************************************************************************
* FUNCTION
*   wndrv_cal_txpwr_5000M
*
* DESCRIPTION
*   Calibration function to write 5.0G band TX power level.
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
void wndrv_cal_txpwr_5000M(wndrv_cal_txpwr_5000M_struct *txpwr)
{
#if 0
#if SUPPORT_802_11A
    NIC_PER_CH_CONFIG  *p = wndrv_adapter_pool.nicPerChannelConfig5;

    WNDRV_ASSERT(MAX_NUM_5G_RF_CHS == 34);
    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_MISC_TXPWR_5000M,
                 "WNDRV_MISC: cal_5.0g_pwr");

    /* In nicReadPerChannelConfig function, we mark nicReadTxPowerSetting.
       So the tx gain is set from this function. */

    for (i = 0; i < 34; i++) {
        p[i].txPowerGainOFDM.maxGain = txpwr->TxPWR[ i ];
    }
#endif
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_cal_init_dac_dc_offset
*
* DESCRIPTION
*   Calibration function to write DAC DC Offset at power-on stage.
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
void wndrv_cal_init_dac_dc_offset(wndrv_cal_dac_dc_offset_struct *dcOffset)
{
#if 0
    wndrv_glue_info.dacID = dcOffset->i_ch_offset;
    wndrv_glue_info.dacQD = dcOffset->q_ch_offset;
#endif
}

/*******************************************************************************
* FUNCTION
*   wndrv_cal_dac_dc_offset
*
* DESCRIPTION
*   On-line change DAC DC Offset.
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
void wndrv_cal_dac_dc_offset(wndrv_cal_dac_dc_offset_struct *dcOffset)
{
#if 0
    phySetDACOffset(wndrv_context.adpt_p,
                    dcOffset->i_ch_offset,
                    dcOffset->q_ch_offset);

    wndrv_glue_info.dacID = dcOffset->i_ch_offset;
    wndrv_glue_info.dacQD = dcOffset->q_ch_offset;
#endif
}


/*******************************************************************************
* FUNCTION
*   wndrv_cal_query_chip_capability
*
* DESCRIPTION
*   Calibration function to query chip capability.
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
void wndrv_cal_query_chip_capability(wndrv_query_chip_capability_struct *cap)
{
    //2008116 modify by saker for add MT5921 definition
#if( IS_BB_CHIP_MT5911 )
    {
        cap->bb_chip_id = WNDRV_BB_MT5911;
        strcpy(cap->bb_chip_ver, "UNKNOWN");
        cap->support_802_11b = KAL_TRUE;
    }
#elif( IS_BB_CHIP_MT5921 )
    {
        cap->bb_chip_id = WNDRV_BB_MT5921;
        strcpy(cap->bb_chip_ver, "UNKNOWN");
        cap->support_802_11b = KAL_TRUE;
    }
#elif( IS_BB_CHIP_MT5931 )
    {
        cap->bb_chip_id = WNDRV_BB_MT5931;    /* Nelson: workaround for MT5921 meta tool test */
        strcpy(cap->bb_chip_ver, "UNKNOWN");
        cap->support_802_11b = KAL_TRUE;
    }
#else
#error "BB error"
#endif

    //2008116 modify by saker for add MT5921 definition
#if ( IS_RF_CHIP_AL2236 )
    {
        cap->rf_chip_id = WNDRV_RF_AL2236;
        strcpy(cap->rf_chip_ver, "UNKNOWN");
    }
#elif ( IS_RF_CHIP_MAX2827 )
    {
        cap->rf_chip_id = WNDRV_RF_MAX2827;
        strcpy(cap->rf_chip_ver, "UNKNOWN");
    }
#elif ( IS_RF_CHIP_MT5921 )
    {
        cap->rf_chip_id = WNDRV_RF_MT5921;
        strcpy(cap->rf_chip_ver, "UNKNOWN");
    }
#elif ( IS_RF_CHIP_MT5931 )
    {
        cap->rf_chip_id = WNDRV_RF_MT5931;    /* Nelson: workaround for MT5921 meta tool test */
        strcpy(cap->rf_chip_ver, "UNKNOWN");
    }
#else
#error "RF error"
#endif

#if ( BUILD_11A )
    cap->support_802_11a = KAL_TRUE;
#else
    cap->support_802_11a = KAL_FALSE;
#endif

#if ( BUILD_11G )
    cap->support_802_11g = KAL_TRUE;
#else
    cap->support_802_11g = KAL_FALSE;
#endif

    cap->support_802_11n = KAL_TRUE;

#if defined ( __WIFI_HIF_SDIO__ )
    {
        cap->host_interface = WNDRV_HOST_INTERFACE_SDIO;
    }
#elif defined ( _HIF_HPI )
    {
        cap->host_interface = WNDRV_HOST_INTERFACE_HPI;
    }
#else
#error "IF error"
#endif

    cap->eeprom_exist = wndrv_get_eeprom_existence();
    cap->eeprom_size  = wndrv_get_eeprom_size();
    WNDRV_TRACE3(TRACE_GROUP_10, INFO_WNDRV_MISC_CHIP_CAP,
                 "WNDRV_MISC: bb_chip=%d, rf_chip=%d, eeprom_exist=%d",
                 cap->bb_chip_id,
                 cap->rf_chip_id,
                 cap->eeprom_exist);
}

/*******************************************************************************
* FUNCTION
*   wndrv_cal_tx_ALC_power_2400M
*
* DESCRIPTION
*   Calibration function to write 5.0G band TX power level.
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
void wndrv_cal_tx_ALC_power_2400M(wndrv_cal_tx_ALC_2400M_struct *txALC)
{
#if 0
    NIC_PER_CH_CONFIG  *config_p = &(wndrv_glue_info.nicPerChannelConfig24[0]);
    kal_uint8 i = 0, j = 0;

    WNDRV_ASSERT(MAX_NUM_2G_RF_CHS == 14);
    //WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_MISC_TX_ALC_POWER,
    //             "WNDRV_MISC: cal_tx_alc_power");

    for (i = 0 ; i < MAX_NUM_2G_RF_CHS; i ++) {
        config_p[i].txAlcCCK = txALC->txAlcCCK[i];
        config_p[i].txOutputPowerCCK = txALC->txOutputPowerDBCCK[i];
        config_p[i].alcValid = TRUE;
    }

#if SUPPORT_802_11G
    for (j = 0 ; j < MAX_NUM_OFDM_DATA_RATES ; j ++) {
        for (i = 0 ; i < MAX_NUM_2G_RF_CHS ; i ++) {
            config_p[i].txAlcOFDM[j] = txALC->txAlcOFDM[j][i];
            config_p[i].txOutputPowerOFDM[j] = txALC->txOutputPowerDBOFDM[j][i];
        }
    }
#endif
#endif
}

#if 0
/*******************************************************************************
* FUNCTION
*   wndrv_cal_txpwr_5000M
*
* DESCRIPTION
*   Calibration function to write 5.0G band TX power level.
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
void wndrv_cal_txpwr_5000M(wndrv_cal_txpwr_5000M_struct *txpwr)
{
#if 0 /* SUPPORT_802_11A */
    NIC_PER_CH_CONFIG  *p = wndrv_adapter_pool.nicPerChannelConfig5;

    WNDRV_ASSERT(MAX_NUM_5G_RF_CHS == 34);
    WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_MISC_TXPWR_5000M,
                 "WNDRV_MISC: cal_5.0g_pwr");

    /* In nicReadPerChannelConfig function, we mark nicReadTxPowerSetting.
       So the tx gain is set from this function. */

    for (i = 0; i <= 34; i++) {
        p[i].txPowerGainOFDM.maxGain = txpwr->TxPWR[ i ];
    }
#endif
}
#endif

/*******************************************************************************
* FUNCTION
*   wndrv_cal_ALC_Slope_2400M
*
* DESCRIPTION
*   Calibration function to write 2.4G band TX ALC slope
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
void wndrv_cal_ALC_Slope_2400M(wndrv_cal_ALC_Slope_2400M_struct *pALCSlope)
{
#if 0
    //WNDRV_TRACE0(TRACE_GROUP_10, INFO_WNDRV_MISC_TX_ALC_POWER,
    //             "WNDRV_MISC: cal_tx_alc_power");


    /*only two slope value in MT5921*/
    wndrv_glue_info.alcControlInfo.alcVgaSlop = pALCSlope->alcSlop1Divider;
    //wndrv_glue_info.alcControlInfo.alcSlop1Dividend = pALCSlope->alcSlop1Dividend;
    wndrv_glue_info.alcControlInfo.alcThermoSensorSlop = pALCSlope->alcSlop2Divider;
    //wndrv_glue_info.alcControlInfo.alcSlop2Dividend = pALCSlope->alcSlop2Dividend;
#endif
}

//20081117 add by saker
void wndrv_cal_Trim_Thermo_value(wndrv_cal_setting_trim_thermo_struct *set)
{
    /* MT5931 tx frequency offset */
    if (set->ucXtalTrim & BIT(7)) { /* negative */
        wndrv_glue_info.cFreq_offset = 0 - (set->ucXtalTrim & BITS(0, 6));
    } else { /* positeive */
        wndrv_glue_info.cFreq_offset = set->ucXtalTrim & BITS(0, 6);
    }
    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_INIT, "wndrv_cal_Trim_Thermo_value =%d", wndrv_glue_info.cFreq_offset);
}

void wndrv_cal_txpwr_cal_free_flow(wndrv_cal_txpwr_cal_free_flow_struct *TxPwrCal)
{
    UINT_8              i, j;
    UINT_8              ucReg_ch_list_index;
    P_DOMAIN_INFO_ENTRY prDomainInfo = &wndrv_glue_info.rRegInfo.rDomainInfo;

    wndrv_glue_info.ucCh_set_mode = TxPwrCal->ucAlcAdcSwing;

    if (wndrv_glue_info.ucCh_set_mode == 1) {   /* By Regulatory Channel List Index */
        ucReg_ch_list_index = TxPwrCal->ucAlcAdcDCbias;
        memcpy(prDomainInfo, &arSupportedRegDomains[ucReg_ch_list_index], sizeof(DOMAIN_INFO_ENTRY));
    } else if (wndrv_glue_info.ucCh_set_mode == 2) { /* By Regulatory SubBand Info field */
        i = 0;
        j = 0;
        while (i < 12) {
            prDomainInfo->rSubBand[j].ucRegClass = TxPwrCal->PAOutAlcAdcValue[i][0];
            prDomainInfo->rSubBand[j].ucBand = (ENUM_BAND_T)TxPwrCal->PAOutAlcAdcValue[i][1];
            prDomainInfo->rSubBand[j].ucChannelSpan = TxPwrCal->PAOutAlcAdcValue[i][2];
            i++;
            prDomainInfo->rSubBand[j].ucFirstChannelNum = TxPwrCal->PAOutAlcAdcValue[i][0];
            prDomainInfo->rSubBand[j].ucNumChannels = TxPwrCal->PAOutAlcAdcValue[i][1];
            i++;
            j++;
        }
    } else {
        prDomainInfo->u4CountryNum = 0;
        wndrv_glue_info.ucCh_set_mode = 0;  /* default mode: By Country code */
    }

    /* DEBUG LOG */
    MT5931_NEW_TRACE(MT5931_INFO_442, TRACE_INIT, "wndrv_glue_info.ucCh_set_mode=%d", wndrv_glue_info.ucCh_set_mode);
    for (i = 0; i < 6; i++) {
        MT5931_NEW_TRACE(MT5931_INFO_443, TRACE_INIT, "i=%d", i);
        MT5931_NEW_TRACE(MT5931_INFO_444, TRACE_INIT, "ucRegClass=%d, ucBand=%d, ucChannelSpan=%d, ucFirstChannelNum=%d, ucNumChannels=%d",
                         prDomainInfo->rSubBand[i].ucRegClass, prDomainInfo->rSubBand[i].ucBand, prDomainInfo->rSubBand[i].ucChannelSpan,
                         prDomainInfo->rSubBand[i].ucFirstChannelNum, prDomainInfo->rSubBand[i].ucNumChannels);
    }
}

#if 0
void wndrv_read_cal_data(P_EEPROM_CTRL_T prEEPROMCtrl)
{
//#if !WNDRV_EEPROM_USAGE /*20081002 modify by saker*/

    //P_EEPROM_CTRL_T prEEPROMCtrl = &(wndrv_glue_info.prAdapter->rEEPROMCtrl);
    P_EEPROM_CHANNEL_CFG_ENTRY prEepChCfg;
    NIC_PER_CH_CONFIG  *prConfig = &(wndrv_glue_info.nicPerChannelConfig24[0]);
    UINT_16 u2Rate, u2Ch;

    /*wndrv_cal_macaddr*/
    memcpy(prEEPROMCtrl->aucMacAddress, wndrv_glue_info.permanentMacAddress, MAC_ADDR_LEN);

    /*wndrv_cal_txpwr_2400M & wndrv_cal_tx_ALC_power_2400M*/
    prEEPROMCtrl->pu4EepromChCfgTable_24 = (PUINT_32) &arNicEepromChannelTable[0];
    prEepChCfg = (P_EEPROM_CHANNEL_CFG_ENTRY)prEEPROMCtrl->pu4EepromChCfgTable_24;
    for (u2Rate = EEPROM_RATE_GROUP_CCK; u2Rate <= EEPROM_RATE_GROUP_OFDM_48_54M; u2Rate++) {
        for (u2Ch = CH_1; u2Ch <= CH_14; u2Ch++) {
            if (u2Rate == EEPROM_RATE_GROUP_CCK) {
                prEepChCfg[u2Ch].rTxCfg[u2Rate].ucPowerGain = prConfig[u2Ch - 1].txPowerGainCCK.maxGain;
                prEepChCfg[u2Ch].rTxCfg[u2Rate].ucThermoVal = prConfig[u2Ch - 1].txAlcCCK;
            } else {
                prEepChCfg[u2Ch].rTxCfg[u2Rate].ucPowerGain = prConfig[u2Ch - 1].txPowerGainOFDM.maxGain;
                prEepChCfg[u2Ch].rTxCfg[u2Rate].ucThermoVal = prConfig[u2Ch - 1].txAlcOFDM[u2Rate];
            }
        }
    }

    /* DAC offset */
    prEEPROMCtrl->ucID = wndrv_glue_info.dacID;
    prEEPROMCtrl->ucQD = wndrv_glue_info.dacQD;

    //20081118 add by saker for NVRAM
    prEEPROMCtrl->cAbsTemp = wndrv_glue_info.cAbsTemp;
    prEEPROMCtrl->ucThermoValue = wndrv_glue_info.ucThermoValue;
    prEEPROMCtrl->ucXtalTrim = wndrv_glue_info.ucXtalTrim;

    /* ALC Slope*/
    /* Note : load default value not in NVRAM now */
    /* work around without modify NVRAM */
    halGetDefaultEEPROMCfg(prEEPROMCtrl);

    prEEPROMCtrl->ucVgaGainSlop =
        wndrv_glue_info.alcControlInfo.alcVgaSlop;

    prEEPROMCtrl->ucThermoSensorSlop =
        wndrv_glue_info.alcControlInfo.alcThermoSensorSlop;
//#endif

    prEEPROMCtrl->ucTargetGainCCKInHalfDBm = wndrv_glue_info.ucTargetGainCCKInHalfDBm;
    prEEPROMCtrl->ucTargetGainOFDMInHalfDBm = wndrv_glue_info.ucTargetGainOFDMInHalfDBm;
    prEEPROMCtrl->ucBiasCCK2OFDMInHalfDBm = wndrv_glue_info.ucBiasCCK2OFDMInHalfDBm;
    prEEPROMCtrl->ucInitalRefCodeword = wndrv_glue_info.ucInitalRefCodeword;
    prEEPROMCtrl->ucFirstRowPoutInHalfDBm = wndrv_glue_info.ucFirstRowPoutInHalfDBm;
    prEEPROMCtrl->ucTotalValidRows = wndrv_glue_info.ucTotalValidRows;
    prEEPROMCtrl->ucAlcAdcSwing = wndrv_glue_info.ucAlcAdcSwing;
    prEEPROMCtrl->ucAlcAdcDCbias = wndrv_glue_info.ucAlcAdcDCbias;

    memcpy(prEEPROMCtrl->PAOutAlcAdcValue, wndrv_glue_info.PAOutAlcAdcValue, 30 * 3);
}
#endif


kal_bool bwcs_query_antenna_path(void)
{
    if (TRUE == wndrv_context.adpt_p->fgIsAntBT) {
        return KAL_TRUE; // antenna is BT
    } else {
        return KAL_FALSE; // antenna is WiFi
    }
}

void bwcs_set_antenna_path(kal_bool fgAntForBT)
{
    //kal_uint32                  uSavedMask;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrWrInfo;
    UINT_32                     u4InfoLen;
    //WLAN_STATUS                 status;

    kalMdelay(100); //Delay for wifi delay

    WiFi_EintMask();
#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(1);
#endif
    // Set PIN mux for ANT_SEL
    if (wndrv_context.adpt_p->ucRevID >= MT5931_E3_REVERSION_ID) {
        rMcrWrInfo.u4McrOffset = 0x80050180;
    } else {
        rMcrWrInfo.u4McrOffset = 0x80050190;
    }
    rMcrWrInfo.u4McrData   = 0x00100000;
    wlanoidSetMcrWrite(wndrv_context.adpt_p, &rMcrWrInfo, sizeof(rMcrWrInfo), &u4InfoLen);
    kalUdelay(500);

    // Switch ANT path
    rMcrWrInfo.u4McrOffset = 0x60000150;
    if (KAL_TRUE == fgAntForBT) {
        rMcrWrInfo.u4McrData = 0x00000004; // Path for BT
    } else {
        rMcrWrInfo.u4McrData = 0x00000001; // Path for WiFi
    }
    wlanoidSetMcrWrite(wndrv_context.adpt_p, &rMcrWrInfo, sizeof(rMcrWrInfo), &u4InfoLen);
    kalUdelay(500);

    /* NOTE: Engineer mode is called by function directly, not by SAP handlers.
        So handling mailbox, CMD, packet tx here */
    kalMainThread(wndrv_context.glue_p);

#if !defined(__WIFI_HIF_SDIO__)
    wndrv_bus_power_control(0);
#endif
    WiFi_EintUnMask();
}

void wndrv_load_customer_setting(void)
{
    kal_uint32 version;
    //kal_uint32 setting1, setting2; //dummy
    kal_uint32 i = 0;

    /* Set custom default values */
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_REG_DOMAIN] = WNDRV_CUST_DEFAULT_REG_DOMAIN;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_PS_MODE] = WNDRV_CUST_DEFAULT_PS_MODE;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_OSC_STABLE_TIME] = WNDRV_CUST_DEFAULT_OSC_STABLE_TIME;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_WMMPS_CONFIG] = WNDRV_CUST_DEFAULT_WMMPS_CONFIG;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_EINT_POLARITY] = WNDRV_CUST_DEFAULT_EINT_POLARITY;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_LOWEST_RSSI] = (WNDRV_CUST_DEFAULT_LOWEST_RSSI + WNDRV_CUST_DEFAULT_PATH_LOSS);
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_CONNECTION_KEEP_TIME] = WNDRV_CUST_DEFAULT_CONNECTION_KEEP_TIME;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_CONNECTION_RETRY_LIMIT] = WNDRV_CUST_DEFAULT_CONNECTION_RETRY_LIMIT;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DTIM_PERIOD] = WNDRV_CUST_DEFAULT_DTIM_PERIOD;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T6] = WNDRV_CUST_BT_1WIRE_MODE_T6;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_T8] = WNDRV_CUST_BT_1WIRE_MODE_T8;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_BT_1WIRE_MODE_BT_DELAY] = WNDRV_CUST_BT_1WIRE_MODE_BT_DELAY;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DCXO_CONFIG] = WNDRV_CUST_SET_DCXO_CONFIG;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DEC_PWR] = WNDRV_CUST_DEC_PWR;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_PWR_CTRL_TIMEOUT] = WNDRV_CUST_PWR_CTRL_TIMEOUT;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_SDIO_SEL_CARD] = WNDRV_CUST_SDIO_SEL_CARD;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_SDIO_CLK_FREQ] = WNDRV_CUST_SDIO_CLK_FREQ;
    wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_FUNC1_SDIO_BLK_SIZE] = WNDRV_CUST_FUNC1_SDIO_BLK_SIZE;

    version = WiFi_Query_Customer_Seting_Version();

    if (version >= PARAM_CUST_SET_MAX_NUM) {
        //setting1 = wifi_driver_customer_setting[0]; // dummy
        //setting2 = wifi_driver_customer_setting[1]; // dummy

        for (i = 0; i < PARAM_CUST_SET_MAX_NUM; i++) {
            wndrv_context.glue_p->as4Cust_config[i] = wifi_driver_customer_setting[i];
        }
    } else {
        MT5931_TRACE(TRACE_WARNING, MT5931_INFO_322, "Custom Setting Ver %d < %d. set to default", version, PARAM_CUST_SET_MAX_NUM);

        for (i = 0; i < version; i++) {
            wndrv_context.glue_p->as4Cust_config[i] = wifi_driver_customer_setting[i];
        }
    }

    /* Dump customization values */
    for (i = 0; i < PARAM_CUST_SET_MAX_NUM; i++) {
        MT5931_TRACE(TRACE_INIT, MT5931_INFO_323, "wndrv_context.glue_p->as4Cust_config[%d]=%d", i, wndrv_context.glue_p->as4Cust_config[i]);
    }

    /*------------------------------------------------------------------------------
     * HW custom configuration
     *------------------------------------------------------------------------------*/

    // DCXO co-clock configuration
    //if (wndrv_context.glue_p->as4Cust_config[PARAM_CUST_SET_DCXO_CONFIG] & WIFI_SUPPORT_DCXO_CO_CLOCK) {
    //    WiFi_co_clock_config();
    //}

}




