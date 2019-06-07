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

//todo
/*************************************************************************
* Include Statements
 *************************************************************************/
/* 1) ???
      kal\inlcude\kal_release.h */
//#include "kal_release.h"
//#include "app_ltlcom.h"
#include "stack_ltlcom.h"

//#include "nvram_data_items.h"
//#include "nvram_struct.h"

#include "wndrv_cnst.h"
#include "wndrv_cal.h"
#include "wndrv_ft_types.h"
#include "wndrv_ft_msg.h"
//#include "wndrv_private.h"
#include "wndrv_os_private.h"
#include "precomp.h"
#include "wndrv_context.h"
#include "wifi_common_config.h"

extern BOOL wndrv_task_init(void);
extern void wndrv_cal_tx_ALC_power_2400M(wndrv_cal_tx_ALC_2400M_struct *tx_alc);


wndrv_int_queue_info_struct wndrv_int_queue_pool[WNDRV_FREE_INT_QUEUE_SIZE];
QUE_HEADER  wndrv_int_queue_list[WNDRV_INT_QUEUE_TYPE];
kal_uint32  wndrv_int_queue_num = 0;
kal_bool    WNDRVPauseFlag = KAL_FALSE;
kal_bool    WNDRVNoFLC2Flag = KAL_FALSE;


/*******************************************************************************
*
*  Local Functions
*
*******************************************************************************/
void wndrv_get_cal_data(void)
{
    void               *pdu_ptr;
    nvram_lid            nvram_id = 0;

    for (nvram_id = 0; nvram_id < NVRAM_EF_WNDRV_TOTAL; nvram_id++) {

        pdu_ptr = nvram_data_ptr[nvram_id];

        switch (nvram_id) {
            case NVRAM_EF_WNDRV_MAC_ADDRESS_LID: {
                wndrv_cal_mac_addr_struct mac;

                memcpy(&mac, pdu_ptr, sizeof(wndrv_cal_mac_addr_struct));
                wndrv_cal_macaddr(&mac);
            }
            break;

            case NVRAM_EF_WNDRV_TX_POWER_2400M_LID: {
                uint8_t cal_data = 0;
                wndrv_cal_txpwr_2400M_struct *txpwr = (wndrv_cal_txpwr_2400M_struct *)
                                                      WNDRV_ALLOC_BUF(sizeof(wndrv_cal_txpwr_2400M_struct));
                memcpy(txpwr, pdu_ptr, sizeof(wndrv_cal_txpwr_2400M_struct));
                //0. CCKTXPWR0 is c11b_tx_pwr
                //1. CCKTXPWR1 is cCck_band_edge_tx_pwr
                //2. CCKTXPWR2 is cCh_offset_L
                //3. CCKTXPWR3 is cCh_offset_M
                //4. CCKTXPWR4 is cCh_offset_H
                for (uint8_t cck_pwr_index = 0; cck_pwr_index < CCKTXPWR_NUM; cck_pwr_index++) {
                    if (0 == wifi_get_cal_data(cck_pwr_index, &cal_data, sizeof(cal_data))) {
                        txpwr->CCKTxPWR[cck_pwr_index] = cal_data;
                    }
                    cal_data = 0;
                }
                printc("[DBG] cck0=%d,cck1=%d,cck2=%d,cck3=%d,cck4=%d\n", txpwr->CCKTxPWR[0], txpwr->CCKTxPWR[1], txpwr->CCKTxPWR[2], txpwr->CCKTxPWR[3], txpwr->CCKTxPWR[4]);

                for (uint8_t ofdm_pwr_index = 0; ofdm_pwr_index < OFDMTXPWR_NUM; ofdm_pwr_index++) {
                    if (0 == wifi_get_cal_data(CCKTXPWR_NUM + ofdm_pwr_index, &cal_data, sizeof(cal_data))) {
                        txpwr->OFDMTxPWR[ofdm_pwr_index] = cal_data;
                    }
                    cal_data = 0;
                }
                printc("[DBG] ofdm0=%d,ofdm1=%d,ofdm2=%d,ofdm3=%d,ofdm4=%d,ofdm5=%d\n", txpwr->OFDMTxPWR[0], txpwr->OFDMTxPWR[1], txpwr->OFDMTxPWR[2], txpwr->OFDMTxPWR[3], txpwr->OFDMTxPWR[4], txpwr->OFDMTxPWR[5]);

                wndrv_cal_txpwr_2400M(txpwr);

                WNDRV_FREE_BUF(txpwr);
            }
            break;

            case NVRAM_EF_WNDRV_TX_POWER_5000M_LID: {
                wndrv_cal_txpwr_5000M_struct *txpwr = (wndrv_cal_txpwr_5000M_struct *)
                                                      WNDRV_ALLOC_BUF(sizeof(wndrv_cal_txpwr_5000M_struct));

                memcpy(txpwr, pdu_ptr, sizeof(wndrv_cal_txpwr_5000M_struct));
                wndrv_cal_txpwr_5000M(txpwr);

                WNDRV_FREE_BUF(txpwr);
            }
            break;

            case NVRAM_EF_WNDRV_DAC_DC_OFFSET_LID: {
                wndrv_cal_dac_dc_offset_struct dc_offset;

                memcpy(&dc_offset, pdu_ptr, sizeof(wndrv_cal_dac_dc_offset_struct));
                wndrv_cal_init_dac_dc_offset(&dc_offset);
            }
            break;

            case NVRAM_EF_WNDRV_TX_ALC_POWER_LID: {
                wndrv_cal_tx_ALC_2400M_struct *tx_alc = (wndrv_cal_tx_ALC_2400M_struct *)

                                                        WNDRV_ALLOC_BUF(sizeof(wndrv_cal_tx_ALC_2400M_struct));

                memcpy(tx_alc, pdu_ptr, sizeof(wndrv_cal_tx_ALC_2400M_struct));

                wndrv_cal_tx_ALC_power_2400M(tx_alc);

                WNDRV_FREE_BUF(tx_alc);
            }
            break;

            case NVRAM_EF_WNDRV_ALC_SLOPE_LID: {
                wndrv_cal_ALC_Slope_2400M_struct *ALCSlope = (wndrv_cal_ALC_Slope_2400M_struct *)

                        WNDRV_ALLOC_BUF(sizeof(wndrv_cal_ALC_Slope_2400M_struct));

                memcpy(ALCSlope, pdu_ptr, sizeof(wndrv_cal_ALC_Slope_2400M_struct));

                wndrv_cal_ALC_Slope_2400M(ALCSlope);

                WNDRV_FREE_BUF(ALCSlope);
            }
            break;

            //20081117 add by saker
            case NVRAM_EF_WNDRV_EXT_SETTING_TRIMVAL_THERMOVAL_LID: {
                uint8_t cal_data = 0;
                wndrv_cal_setting_trim_thermo_struct *set = (wndrv_cal_setting_trim_thermo_struct *)
                        WNDRV_ALLOC_BUF(sizeof(wndrv_cal_setting_trim_thermo_struct));

                memcpy(set, pdu_ptr, sizeof(wndrv_cal_setting_trim_thermo_struct));

                if (0 == wifi_get_cal_data(0x0B, &cal_data, sizeof(cal_data))) {
                    set->ucXtalTrim = cal_data;
                }
                cal_data = 0;

                wndrv_cal_Trim_Thermo_value(set);

                WNDRV_FREE_BUF(set);
            }
            break;

            case NVRAM_EF_WNDRV_TPCFF_LID: {
                wndrv_cal_txpwr_cal_free_flow_struct *TxPwrCal = (wndrv_cal_txpwr_cal_free_flow_struct *)

                        WNDRV_ALLOC_BUF(sizeof(wndrv_cal_txpwr_cal_free_flow_struct));

                memcpy(TxPwrCal, pdu_ptr, sizeof(wndrv_cal_txpwr_cal_free_flow_struct));

                wndrv_cal_txpwr_cal_free_flow(TxPwrCal);

                WNDRV_FREE_BUF(TxPwrCal);
            }
            break;

            default:
                WNDRV_EXT_ASSERT(KAL_FALSE, "wndrv_get_cal_data: Unexpected msg_ptr->file_idx",
                                 nvram_id, 0, 0);
                break;
        }
    }
}


/*************************************************************************
* FUNCTION
*  wndrv_task_main
*
* DESCRIPTION
*  The entry function called by OS task scheduler.
*
* PARAMETERS
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void wndrv_task_main(void)
{
    ilm_struct *current_ilm;
    kal_uint32 msg_count;
    kal_int32  i;
    kal_status status;
    kal_int32  queue_index = 0;
    kal_int32  tx_rx_diff = 0;

    /********************************************
     *  1. Query normal or meta mode
     ********************************************/
    wndrv_get_poweron_mode();

    /********************************************
     *  2. Get Calibration Data from NVRAM task
     ********************************************/
    wndrv_get_cal_data();

    /********************************************
     *  3. Initialization
     ********************************************/
    wndrv_init();

    /********************************************
     *  4. Initialize Internal Queue
     ********************************************/
    for (i = 0; i < WNDRV_INT_QUEUE_TYPE; i++) {
        QUE_INIT(&wndrv_int_queue_list[i]);
    }
    for (i = 0; i < WNDRV_FREE_INT_QUEUE_SIZE; i++) {
        QUE_INS_TAIL(&wndrv_int_queue_list[WNDRV_INT_QUEUE_FREE_INDEX], &wndrv_int_queue_pool[i].q_entry);
    }

    while (1) {
        status = receive_msg_ext_q_for_stack(mt5931_msgqid[MOD_WNDRV], &current_ilm);
#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceRecord(current_ilm);
#endif
        if (status == KAL_SUCCESS) {
            wndrv_int_queue_info_struct *int_queue_p;

            WNDRV_EXT_ASSERT(wndrv_int_queue_list[WNDRV_INT_QUEUE_FREE_INDEX].numElem > 0,
                             "wndrv_task_main: No free internal queue element",
                             0, 0, 0);

            int_queue_p = (wndrv_int_queue_info_struct *)Q_REMOVE_HEAD(&wndrv_int_queue_list[WNDRV_INT_QUEUE_FREE_INDEX]);
            kal_mem_cpy(&int_queue_p->ilm, current_ilm, sizeof(ilm_struct));
            int_queue_p->src_ilm_ptr = current_ilm;
            // Insert ilm to different queues according to different message IDs
            if (int_queue_p->ilm.msg_id == MSG_ID_WNDRV_INTERRUPT_IND) { //MSG_ID_WNDRV_INTERRUPT_IND
                QUE_INS_TAIL(&wndrv_int_queue_list[WNDRV_INT_QUEUE_RX_INDEX], &int_queue_p->q_entry);
            } else if (int_queue_p->ilm.msg_id == MSG_ID_TIMER_EXPIRY) { //MSG_ID_TIMER_EXPIRY
                QUE_INS_TAIL(&wndrv_int_queue_list[WNDRV_INT_QUEUE_TIMER_INDEX], &int_queue_p->q_entry);
            } else {
                QUE_INS_TAIL(&wndrv_int_queue_list[WNDRV_INT_QUEUE_TX_INDEX], &int_queue_p->q_entry);
            }
            wndrv_int_queue_num++;
        } else {
            WNDRV_EXT_ASSERT(KAL_FALSE, "wndrv_task_main: receive_msg_ext_q fail", status, 0, 0);
        }

        while (wndrv_int_queue_num != 0) {
            QUE_ENTRY                    *entry_p;
            wndrv_int_queue_info_struct  *int_queue_p;

            /* peek queue size */
            msg_get_ext_queue_info(mt5931_msgqid[MOD_WNDRV], &msg_count);
            if (msg_count > 0) {
                break;
            }
            // Rounb-robin machanism is applied to all the internal modules, except FREE_INT_QUEUE
            if (++queue_index >= WNDRV_INT_QUEUE_TYPE - 1) {
                queue_index = 0;
            }

            //20080930 modify by saker for balance TX/RX in a. heavy traffic b. RX using DMA
            //note : after dispatch ext Q to int Q and round-robin machanism
            tx_rx_diff = (kal_int32)(wndrv_int_queue_list[WNDRV_INT_QUEUE_TX_INDEX].numElem - wndrv_int_queue_list[WNDRV_INT_QUEUE_RX_INDEX].numElem);

            if (TX_RX_DIFFERENCE_TO_BALANCE < tx_rx_diff) {
                //when int. TX Q size larger than RX Q size in TX_RX_DIFFERENCE_TO_BALANCE, force to TX
                queue_index = WNDRV_INT_QUEUE_TX_INDEX;
            } else if (TX_RX_DIFFERENCE_TO_BALANCE < (0 - tx_rx_diff)) {
                //when int. RX Q size larger than TX Q size in TX_RX_DIFFERENCE_TO_BALANCE, force to RX
                queue_index = WNDRV_INT_QUEUE_RX_INDEX;
            }

            entry_p = QUE_GET_HEAD(&wndrv_int_queue_list[queue_index]);

            if (entry_p) {
                entry_p = Q_REMOVE_HEAD(&wndrv_int_queue_list[queue_index]);
                wndrv_int_queue_num--;

                int_queue_p = (wndrv_int_queue_info_struct *)entry_p;

                wndrv_main(&int_queue_p->ilm);

#if (TRACE_BUFF_DEBUG == 1)
                ILMTraceCheck(2);
#endif

                if (int_queue_p->ilm.src_mod_id != MOD_TIMER) {
                    //free_ilm( &int_queue_p->ilm );
                    int_queue_p->src_ilm_ptr->local_para_ptr = int_queue_p->ilm.local_para_ptr;
                    int_queue_p->src_ilm_ptr->peer_buff_ptr = int_queue_p->ilm.peer_buff_ptr;
                    free_ilm(int_queue_p->src_ilm_ptr);
                }

                QUE_INS_TAIL(&wndrv_int_queue_list[WNDRV_INT_QUEUE_FREE_INDEX], &int_queue_p->q_entry);
            }
        }
    }
}


/*************************************************************************
* FUNCTION
*  wndrv_create
*
* DESCRIPTION
*  This function implements WiFi entity's create handler.
*
* PARAMETERS
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void wndrv_create(void *argv)
{
    DBGPRINTF("%s\n", __func__);
    wndrv_task_init();
    wndrv_task_main();
}

#if 0
#define TASK_WNDRV_PRIORITY     37

typedef unsigned char   MMP_UBYTE;
typedef unsigned short  MMP_USHORT;
typedef unsigned int    MMP_ULONG;

typedef struct {
    MMP_ULONG   pbos;           // lower address
    MMP_ULONG   ptos;           // higer address
    MMP_UBYTE   ubPriority;
} MMPF_TASK_CFG;

unsigned char  MMPF_OS_CreateTask(void (*taskproc)(void *param), MMPF_TASK_CFG *task_cfg,
                                  void *param);


#define TEST_MAGIC_SIZE     200
#define WNDRV_TASK_STACK_SIZE   8192*6
typedef struct wndrv_array {
    unsigned int wndrv_befor[TEST_MAGIC_SIZE];
    unsigned int wndrv_stack[WNDRV_TASK_STACK_SIZE];
    unsigned int wndrv_after[TEST_MAGIC_SIZE];
} WNDRV_ARRAY;

void wndrv_create_task(void)
{
    MMPF_TASK_CFG task_cfg;
    unsigned char taskid = 0;
    unsigned int i = 0;

    //memset(&wndrv_stack[0], 0x00, sizeof(wndrv_stack));
    //task_cfg.pbos = (unsigned int)&wndrv_stack[0];
    //task_cfg.ptos = (unsigned int)&wndrv_stack[WNDRV_TASK_STACK_SIZE - 1];
    //DBGPRINTF("sizeof(wndrv_stack):%d\n",sizeof(wndrv_stack));
    memset(&wndrv_array.wndrv_stack[0], 0x00, sizeof(wndrv_array.wndrv_stack));
    task_cfg.pbos = (unsigned int)&wndrv_array.wndrv_stack[0];
    task_cfg.ptos = (unsigned int)&wndrv_array.wndrv_stack[WNDRV_TASK_STACK_SIZE - 1];
    DBGPRINTF("sizeof(unsigned long):%d, sizeof(unsigned int):%d, sizeof(unsigned short):%d sizeof(unsigned short int):%d\n", sizeof(unsigned long), sizeof(unsigned int), sizeof(unsigned short), sizeof(unsigned short int));
    DBGPRINTF("sizeof(wndrv_stack):%d\n", sizeof(wndrv_array.wndrv_stack));
    for (i = 0; i < TEST_MAGIC_SIZE; i++) {
        wndrv_array.wndrv_befor[i] = 0x12AB;
        wndrv_array.wndrv_after[i] = 0x12AB;
    }

    task_cfg.ubPriority = TASK_WNDRV_PRIORITY;
    taskid = MMPF_OS_CreateTask(wndrv_create, &task_cfg, (void *) NULL);
    DBGPRINTF("taskid:%d\n", taskid);
}

static void test_supc_send_wndrv_init_req(void)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = allocate_ilm(MOD_SUPC);

    ilm_ptr->msg_id = MSG_ID_WNDRV_SUPC_INIT_REQ;//MSG_ID_WNDRV_SUPC_INIT_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;


    SEND_ILM(MOD_SUPC, MOD_WNDRV, WNDRV_SUPC_SAP, ilm_ptr);

    return;
}

#include "wndrv_supc_msg.h"
static void test_supc_send_wndrv_site_survey_req(kal_uint8 *ssid, kal_uint16 ssid_len)
{
    ilm_struct *ilm_ptr = NULL;
    wndrv_supc_site_survey_req_struct *local_para_ptr = NULL;

    //local_para_ptr = construct_local_para(sizeof(wndrv_supc_site_survey_req_struct), TD_UL);
    local_para_ptr = construct_local_para(sizeof(wndrv_supc_site_survey_req_struct), TD_UL);
    /*has_wps_ie is always contained in site_survey_struct,
       must set wps relative members*/
    local_para_ptr->ssid_len = 0;
    local_para_ptr->has_wps_ie = KAL_FALSE;
    kal_mem_set(&(local_para_ptr->vendor_ie), 0, sizeof(wndrv_supc_wps_probe_ie));

    kal_mem_set(local_para_ptr->ssid, 0,  WNDRV_SSID_MAX_LEN);
    kal_mem_cpy(local_para_ptr->ssid, ssid, ssid_len);
    local_para_ptr->ssid_len = ssid_len;

    ilm_ptr = allocate_ilm(MOD_SUPC);

    ilm_ptr->msg_id = MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ;//MSG_ID_WNDRV_SUPC_SITE_SURVEY_REQ;
    ilm_ptr->local_para_ptr = (local_para_struct *)local_para_ptr;
    ilm_ptr->peer_buff_ptr = NULL;
    //wpa_hexdump(MSG_INFO, "to scan ssid:", (char*)ssid, ssid_len);
    if (ssid_len != 0) {
        kal_wap_trace(MOD_SUPC, TRACE_SCAN, "to scan ssid:%s,len:%d", ssid, ssid_len);
    }
    SEND_ILM(MOD_SUPC, MOD_WNDRV, WNDRV_SUPC_SAP, ilm_ptr);

    return;
}


void wndrv_test_wndrv_create(char *inject_string)
{
    static unsigned char initflag = FALSE;
    unsigned short param1, param2, param3;

    sscanfl(inject_string, "%d %d %d", &param1, &param2, &param3);
    WNDRV_TRACE(TRACE_INFO, "param1=%d,param2=%d,param3=%d\n", param1, param2, param3);

    if (initflag == FALSE) {
        initflag = TRUE;
        mt5931_create_msg_ext_q();
#if 0
        wndrv_create_task();
#else
        test_supc_send_wndrv_init_req();
        wndrv_create(NULL);
#endif
    }

    WNDRV_TRACE(TRACE_INFO, "ready to test\n");
    switch (param1) {
        case 0:
            break;

        case 1:
            test_supc_send_wndrv_init_req();
            break;

        case 2:
            test_supc_send_wndrv_site_survey_req(NULL, 0);
            break;

        default:
            break;
    }

}

#endif


