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

#include "cmux_def.h"
#include "cmux_lib.h"
#include "cmux_utl.h"
#include "cmux_vp_num.h"
#include "cmux_hdlr.h"
#include "cmux_prot.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_cch.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_cch.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_cch.c] "fmt,##arg)

#define GET_MESSAGE_TYPE(b) ((b) &0xFC)
#define GET_MESSAGE_LENGTH(b) ((b)>>1)
#define CR_BIT_OF_MSG(b) ((b) &0x02)
#define RESPONSE_CR  0
#define ORIGINATOR_CR 2
#define EA_BIT_OF_MSG(b) ((b) &0x01)
#define EA_END   1

#define PN_MESSAGE      0x80
#define PSC_MESSAGE     0x40
#define CLD_MESSAGE     0xC0
#define TEST_MESSAGE    0x20
#define FCON_MESSAGE    0xA0
#define FCOFF_MESSAGE   0x60
#define MSC_MESSAGE     0xE0
#define NSC_MESSAGE     0x10
#define RPN_MESSAGE     0x90
#define SNC_MESSAGE     0xD0

#define PN_MSG_LENGTH   8
#define CLD_MSG_LENGTH  0
#define MSC_MSG_LENGTH  2

#define DEFAULT_N1_VALUE 31
#define PN_UIH_FRAME    0
#define PN_CL_TYPE2     1

#define MSC_DV_BIT      0x80
#define MSC_IC_BIT      0x40
#define MSC_RTR_BIT     0x08
#define MSC_RTC_BIT     0x04
#define MSC_FC_BIT      0x02

static void cmux_sendNscMessage(kal_uint8 msg)
{
    CmuxDlcT *pDlcCch = & cmux_ptr->dlcs[0];
    kal_uint8  nscResponse[4];
    kal_uint8 *ptr = nscResponse;

    *ptr = (kal_uint8)(EA_END | RESPONSE_CR | NSC_MESSAGE);
    ptr++;
    *ptr = (1 << 1) | EA_END;
    ptr++;
    *ptr = msg;
    // write to data
    cmux_writeBuffer(& pDlcCch->txBuffer, nscResponse, 3);
    // make it ready
    cmux_schedulerDlcReady(pDlcCch);
}

void cmux_sendPnMessage(kal_uint32 dlci)
{
    CmuxDlcT *pDlcCch = & cmux_ptr->dlcs[0];
    kal_uint8 pn[10];
    kal_uint8 *ptr = pn;

    CmuxDlcT   *pDlcDest;
    kal_uint32  negPriority = 0;
    kal_uint32  negN1 = 0;
    kal_uint8   negCL = 0;

    LOGI("dlci = %d", dlci);

    /* PN should be done before the DLC is active */
    pDlcDest = cmux_findDlcByDlciState( dlci, DLC_ACTIVE );
    if ( pDlcDest != NULL ) {
        // response with current value
        negPriority = pDlcDest->pn_priority;
        negN1 = pDlcDest->pn_n1;
        negCL = pDlcDest->pn_cltype;
    } else {
        pDlcDest = cmux_findDlcByDlciState( dlci, DLC_PARNEG );

        if ( pDlcDest == NULL ) {
            pDlcDest = cmux_locateSuitableDlc(dlci);

            if (pDlcDest != NULL) {
                cmux_initializeDlc(pDlcDest, dlci);
                pDlcDest->state = DLC_PARNEG;
            }
        }

        if ( pDlcDest != NULL) {
            // We have no preference for Priority
            negPriority = pDlcDest->pn_priority;

            // But we may check N1 for a reasonable value.
            negN1 = pDlcDest->pn_n1;
            negCL = pDlcDest->pn_cltype;
        } else {
            // No available DLC for use. set to default value
            CMUX_ASSERT(0);
        }
    }

    LOGI("negPriority = %d, negN1 = %d, negCL = %d", negPriority, negN1, negCL);

    *ptr = EA_END | ORIGINATOR_CR | PN_MESSAGE;
    ptr++;
    *ptr = (PN_MSG_LENGTH << 1) | EA_END; /* Length */
    ptr++;
    *ptr = (kal_uint8) (dlci);
    ptr++;
    // CL should be alwayws CL2 and Frame type shoudl be always UIH
    *ptr = (kal_uint8)( (negCL << 4) | PN_UIH_FRAME);
    ptr++;
    // priority
    *ptr = (kal_uint8) negPriority;
    ptr++;
    // T1 unchanged
    *ptr = DEFAULT_T1_VALUE / 10;
    ptr++;
    // N1
    *ptr = (kal_uint8) negN1;
    ptr++;
    *ptr = (kal_uint8) (negN1 >> 8);
    ptr++;
    // N2 and k unchanged
    *ptr = 3;
    ptr++;
    *ptr = 2;

    // write to data
    cmux_writeBuffer( &pDlcCch->txBuffer, pn, 10 );
    // make it ready
    cmux_schedulerDlcReady(pDlcCch);
}

void cmux_sendCldMessage(kal_bool command_cr)
{
    CmuxDlcT *pDlcCch = & cmux_ptr->dlcs[0];
    kal_uint8 cldResponse[4];
    kal_uint8 *ptr;

    ptr = cldResponse;

    if (command_cr == KAL_TRUE) {
        *ptr = (kal_uint8)(EA_END | ORIGINATOR_CR | CLD_MESSAGE);
    } else {
        *ptr = (kal_uint8)(EA_END | RESPONSE_CR | CLD_MESSAGE);
    }
    ptr++;
    *ptr = 0 | EA_END;

    // write to data
    cmux_writeBuffer(& pDlcCch->txBuffer, cldResponse, 2);
    // make it ready
    cmux_schedulerDlcReady(pDlcCch);
    cmux_ptr->fSendCLD = true;
}

static void cmux_sendMscMessage(kal_uint32 dlci, kal_uint8 option, kal_uint32 cr)
{
#if 0
    CmuxDlcT *pDlcCch = & cmux_ptr->dlcs[0];
#endif
    kal_uint8 msc[4]; /* Not support Break Signals */
    kal_uint8 *ptr = msc;

    *ptr = (kal_uint8)(EA_END | cr | MSC_MESSAGE);
    ptr++;
    *ptr = (MSC_MSG_LENGTH << 1) | EA_END; /* Length */
    ptr++;
    *ptr = (kal_uint8) ((dlci << 2) | ORIGINATOR_CR | EA_END);
    ptr++;
    *ptr = option;

#if 0
    // write to data
    cmux_writeBuffer( &pDlcCch->txBuffer, msc, 4 );
    // make it ready
    cmux_schedulerDlcReady(pDlcCch);
#endif
}

static void cmux_handlePnMessage( void )
{
    CmuxDlcT   *pDlcCch = &cmux_ptr->dlcs[0];

    CmuxDlcT   *pDlcDest;
    kal_uint32  dlci;

    kal_uint8 pnContent[(2 + PN_MSG_LENGTH)];

    kal_uint32  negPriority;
    kal_uint32  negN1;
    kal_uint8   negCL = 0;

    /* decode the Parameter, and process it
    *  the format of PN is
    *  Bit   8  7  6  5  4  3  2  1
    *  byte
    *   0      0  0  [  DLCI        ]
    *   1      [  CL    ]  [  type  ]
    *   2      0  0  [   Priority    ]
    *   3      [   T1                  ]
    *   4      [   N1-low           ]
    *   5      [   N1-high          ]
    *   6      [   N2                  ]
    *   7      0  0  0  0  0 [k     ]
    */

    // Read PN message from rx buffer of CCH
    cmux_readBuffer( &pDlcCch->rxBuffer, (pnContent + 2), PN_MSG_LENGTH);

    /*
    *  In current implementation, we care only about
    *  Priority and N1
    *  Other values are not concerned.
    */
    dlci = pnContent[0 + 2] & 0x3F;

    LOGI("dlci = %d", dlci);

    if (cmux_ptr->role == CMUX_ROLE_CLIENT) {
        /* PN should be done before the DLC is active */
        pDlcDest = cmux_findDlcByDlciState( dlci, DLC_ACTIVE );
        if ( pDlcDest != NULL ) {
            LOGW("already in active DLC");
        } else {
            pDlcDest = cmux_findDlcByDlciState( dlci, DLC_PARNEG );

            if ( pDlcDest != NULL) {
                // We have no preference for Priority
                negPriority = pnContent[2 + 2] & 0x3F;
                if (pDlcDest->pn_priority != negPriority) {
                    LOGW("pn_priority %d changed to negPriority %d", pDlcDest->pn_priority, negPriority);
                }
                pDlcDest->pn_priority = negPriority;

                // But we may check N1 for a reasonable value.
                negN1 = pnContent[4 + 2] | (pnContent[5 + 2] << 8);
                if (negN1 < CMUX_MIN_N1_VALUE) {
                    LOGW("negN1 %d changed to CMUX_MIN_N1_VALUE %d", negN1, CMUX_MIN_N1_VALUE);
                    negN1 = CMUX_MIN_N1_VALUE;
                }
                if (negN1 > CMUX_MAX_N1_VALUE) {
                    LOGW("negN1 %d changed to CMUX_MAX_N1_VALUE %d", negN1, CMUX_MAX_N1_VALUE);
                    negN1 = CMUX_MAX_N1_VALUE;
                }
                if (pDlcDest->pn_n1 != negN1) {
                    LOGW("pn_n1 %d changed to negN1 %d", pDlcDest->pn_n1, negN1);
                }
                pDlcDest->pn_n1 = (kal_uint32) negN1;

                // Negotiat Convergence Layer
                negCL = (pnContent[1 + 2] & 0xF0) >> 4;
                if ( negCL == CMUX_CL2 || negCL == CMUX_CL3) {
                    if (negCL != pDlcDest->pn_cltype) {
                        LOGW("negCL %d changed to pn_cltype %d", negCL, pDlcDest->pn_cltype);
                    }
                    negCL = pDlcDest->pn_cltype;
                } else {
                    if (pDlcDest->pn_cltype != negCL) {
                        LOGW("pn_cltype %d changed to negCL %d", pDlcDest->pn_cltype, negCL);
                    }
                    pDlcDest->pn_cltype = (CmuxClTypeE) negCL;
                }

                LOGI("negPriority = %d, negN1 = %d, negCL = %d", negPriority, negN1, negCL);
            } else {
                // No available DLC for use. set to default value
                LOGE("PN failed!");
            }
        }

        cmux_continue_negotiate_and_set_parameter();
        return;
    }

    /* PN should be done before the DLC is active */
    pDlcDest = cmux_findDlcByDlciState( dlci, DLC_ACTIVE );
    if ( pDlcDest != NULL ) {
        // response with current value
        negPriority = pDlcDest->pn_priority;
        negN1 = pDlcDest->pn_n1;
        negCL = pDlcDest->pn_cltype;
    } else {
        pDlcDest = cmux_findDlcByDlciState( dlci, DLC_PARNEG );

        if ( pDlcDest == NULL ) {
            pDlcDest = cmux_locateSuitableDlc(dlci);

            if (pDlcDest != NULL) {
                cmux_initializeDlc(pDlcDest, dlci);
                pDlcDest->state = DLC_PARNEG;
            }
        }

        if ( pDlcDest != NULL) {
            // We have no preference for Priority
            negPriority = pnContent[2 + 2] & 0x3F;
            if (pDlcDest->pn_priority != negPriority) {
                LOGW("pn_priority %d changed to negPriority %d", pDlcDest->pn_priority, negPriority);
            }
            pDlcDest->pn_priority = negPriority;

            // But we may check N1 for a reasonable value.
            negN1 = pnContent[4 + 2] | (pnContent[5 + 2] << 8);
            if (negN1 < CMUX_MIN_N1_VALUE) {
                LOGW("negN1 %d changed to CMUX_MIN_N1_VALUE %d", negN1, CMUX_MIN_N1_VALUE);
                negN1 = CMUX_MIN_N1_VALUE;
            }
            if (negN1 > CMUX_MAX_N1_VALUE) {
                LOGW("negN1 %d changed to CMUX_MAX_N1_VALUE %d", negN1, CMUX_MAX_N1_VALUE);
                negN1 = CMUX_MAX_N1_VALUE;
            }
            if (pDlcDest->pn_n1 != negN1) {
                LOGW("pn_n1 %d changed to negN1 %d", pDlcDest->pn_n1, negN1);
            }
            pDlcDest->pn_n1 = (kal_uint32) negN1;

            // Negotiat Convergence Layer
            negCL = (pnContent[1 + 2] & 0xF0) >> 4;
            if ( negCL == CMUX_CL2 || negCL == CMUX_CL3) {
                if (negCL != pDlcDest->pn_cltype) {
                    LOGW("negCL %d changed to pn_cltype %d", negCL, pDlcDest->pn_cltype);
                }
                negCL = pDlcDest->pn_cltype;
            } else {
                if (pDlcDest->pn_cltype != negCL) {
                    LOGW("pn_cltype %d changed to negCL %d", pDlcDest->pn_cltype, negCL);
                }
                pDlcDest->pn_cltype = (CmuxClTypeE) negCL;
            }

            LOGI("negPriority = %d, negN1 = %d, negCL = %d", negPriority, negN1, negCL);
        } else {
            // No available DLC for use. set to default value
            negPriority = pnContent[2 + 2] & 0x3F;
            negN1 = pnContent[4 + 2] | (pnContent[5 + 2] << 8);
            negCL = (pnContent[1 + 2] & 0xF0) >> 4;
        }
    }

    // Header
    pnContent[0] = EA_END | RESPONSE_CR | PN_MESSAGE;
    pnContent[1] = (PN_MSG_LENGTH << 1) | EA_END;
    // DLCI unchanged
    // CL should be alwayws CL2 and Frame type shoudl be always UIH
    pnContent[2 + 1] =  (kal_uint8)( (negCL << 4) | PN_UIH_FRAME);
    // priority
    pnContent[2 + 2] = (kal_uint8) negPriority;
    // T1 unchanged
    // N1
    pnContent[2 + 4] = (kal_uint8) negN1;
    pnContent[2 + 5] = (kal_uint8) (negN1 >> 8);
    // N2 and k unchanged

    // write to data
    cmux_writeBuffer(& pDlcCch->txBuffer, pnContent, 10);
    // make it ready
    cmux_schedulerDlcReady(pDlcCch);
}

static void cmux_handleCldMessage(kal_uint8 command)
{
    kal_uint32 i;

    switch (cmux_ptr->state) {
        case CMUX_ACTIVE:
        case CMUX_STARTUP:
            /* disconnect all DLC, if not disconnected */
            if (CR_BIT_OF_MSG(command) != ORIGINATOR_CR) {
                // ignore this message
                return;
            }

            for (i = 1; i < CMUX_CHANNEL_NUM; i++) {
                CmuxDlcT *pDlc = & cmux_ptr->dlcs[i];

                if (pDlc->state == DLC_ACTIVE) {
                    cmux_notifyCPortPlugout(pDlc);
                    cmux_schedulerDlcSuspend(pDlc);
                    cmux_releaseDlc(pDlc);
                }
            }

            // response with CLD resp
            cmux_sendCldMessage(KAL_FALSE);
            // Notify UART handler about to release of UART.
            // UART Handler should finish writing and then response
            cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_REQ, NULL);
            break;

        case CMUX_CLOSING:
            if (CR_BIT_OF_MSG(command) != RESPONSE_CR) {
                // ignore this message
                return;
            }

            cmux_stopTimer(CMUX_T2);
            cmux_stopTimer(CMUX_T3);
            cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_REQ, NULL);
            break;
        default:
            break;
    }
}

static void cmux_handleMscMessage(kal_uint8 length)
{
    CmuxDlcT *pDlcCch = & cmux_ptr->dlcs[0];
    CmuxDlcT *pDlcDest = NULL;
    kal_uint8 mscContent[MSC_MSG_LENGTH + 1]; /* include header */
    kal_uint32 dlci;

    cmux_readBuffer(& pDlcCch->rxBuffer, mscContent, length);

    dlci = mscContent[0] >> 2;

    pDlcDest = cmux_findDlcByDlciState(dlci, DLC_ACTIVE);

    if ( pDlcDest != NULL ) {
        cmux_protocol_decodeV24(pDlcDest, mscContent[1]);
    }

    cmux_sendMscMessage(dlci, mscContent[1], RESPONSE_CR);
}

void cmux_handleControlMessage(void)
{
    CmuxDlcT *pDlcCch = & cmux_ptr->dlcs[0];
    kal_uint8 msgHeader[2];
    kal_uint32 u4DataAvail;
    kal_uint32 msgLength;

    while (cmux_isBufferEmpty(& pDlcCch->rxBuffer) == KAL_FALSE) {
        /* the Message format is
        *   EA CR MSG
        *   Length
        *   para....
        */
        u4DataAvail =  cmux_getBufferDataAvail(& pDlcCch->rxBuffer);
        if (u4DataAvail < 2) {
            // abnormal case... ignore this frame
            cmux_discardDataInBuffer( &pDlcCch->rxBuffer, u4DataAvail);
            // break through the while loop
            break;
        }

        cmux_readBuffer(& pDlcCch->rxBuffer, msgHeader, 2);
        // UE will not initiate any procedure except CLD_MESSAGE.
        msgLength = GET_MESSAGE_LENGTH(msgHeader[1]);
        if (u4DataAvail < msgLength) {
            cmux_discardDataInBuffer( &pDlcCch->rxBuffer, u4DataAvail);
            // break through the while loop
            break;
        }

        if (EA_BIT_OF_MSG(msgHeader[0]) != EA_END ) {
            cmux_discardDataInBuffer( &pDlcCch->rxBuffer, msgLength);
            // continue through the while loop
            continue;
        }

        // available data is at least more than msgHeader[1] from here
        switch (GET_MESSAGE_TYPE(msgHeader[0])) {
            case PN_MESSAGE:
                if (msgLength == PN_MSG_LENGTH
                        && CR_BIT_OF_MSG(msgHeader[0]) == ORIGINATOR_CR && cmux_ptr->role == CMUX_ROLE_SERVER) {
                    cmux_handlePnMessage();
                } else if (msgLength == PN_MSG_LENGTH
                           && CR_BIT_OF_MSG(msgHeader[0]) == RESPONSE_CR && cmux_ptr->role == CMUX_ROLE_CLIENT) {
                    cmux_handlePnMessage();
                } else {
                    cmux_discardDataInBuffer(& pDlcCch->rxBuffer, msgLength);
                }
                break;

            case CLD_MESSAGE:
                if (msgLength == CLD_MSG_LENGTH) {
                    cmux_handleCldMessage(msgHeader[0]);
                } else {
                    cmux_discardDataInBuffer(& pDlcCch->rxBuffer, msgLength);
                }
                break;

            case MSC_MESSAGE:
                if (((msgLength == MSC_MSG_LENGTH) ||
                        (msgLength == MSC_MSG_LENGTH + 1)) &&
                        (CR_BIT_OF_MSG(msgHeader[0]) == ORIGINATOR_CR)) {
                    cmux_handleMscMessage((kal_uint8) msgLength);
                } else {
                    cmux_discardDataInBuffer(& pDlcCch->rxBuffer, msgLength);
                }
                break;

            default:
                cmux_sendNscMessage(msgHeader[0]);
                cmux_discardDataInBuffer(&pDlcCch->rxBuffer, msgLength);
                break;
        }
    }
}

void cmux_handleT2Expiry(void)
{
    // retransmit CLD
    if (cmux_ptr->state == CMUX_CLOSING) {
        cmux_sendCldMessage(KAL_TRUE);

        if ( cmux_ptr->fUartSendSuspend == KAL_FALSE ) {
            LOGI("schedule to write");
            cmux_scheduleWrite();
        }

        cmux_startTimer(CMUX_T2, DEFAULT_T2_VALUE);
    } else if (cmux_ptr->retry_state >= CMUX_RETRANSMIT) {
        LOGE("retry_state = %d", cmux_ptr->retry_state);
    
        cmux_sendCtrlFrame(cmux_ptr->retry_state - CMUX_RETRANSMIT, SABM_POLL);

        cmux_startTimer(CMUX_T2, DEFAULT_T2_VALUE);
    }
}

void cmux_handleT3Expiry(void)
{
    if (cmux_ptr->state == CMUX_CLOSING) {
        cmux_stopTimer(CMUX_T2);

        cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_REQ, NULL);
    } else if (cmux_ptr->retry_state >= CMUX_RETRANSMIT) {
        LOGE("retry_state = %d", cmux_ptr->retry_state);

        cmux_stopTimer(CMUX_T2);

        cmux_send_message(CMUX_EVENT_MODEM_ABNORMAL, NULL);
    }
}

void cmux_resumePeerDlc(kal_uint32 dlci)
{
    kal_uint8 option = (EA_END | MSC_RTC_BIT | MSC_RTR_BIT | MSC_DV_BIT);

    // Send MSC suspend message
    cmux_sendMscMessage(dlci, option, ORIGINATOR_CR);
}

void cmux_suspendPeerDlc(kal_uint32 dlci)
{
#ifdef __CMUX_MODEM_STATUS_SUPPORT__
    kal_uint8 option = (EA_END | MSC_FC_BIT | MSC_RTC_BIT | MSC_DV_BIT);
#else
    kal_uint8 option = (EA_END | MSC_FC_BIT | MSC_RTC_BIT | MSC_RTR_BIT | MSC_DV_BIT); // cmux_bug: RTR and FC shall be conflicted to each other
#endif

    cmux_sendMscMessage(dlci, option, ORIGINATOR_CR);
}

#ifdef __CMUX_MODEM_STATUS_SUPPORT__
void cmux_sendDlcMscStatus(kal_uint32 dlci)
{
    CmuxDlcT *pDlc = cmux_findDlcByDlciState(dlci, DLC_ACTIVE);

    if (pDlc != NULL) {
        kal_uint8 option = cmux_protocol_encodeV24( pDlc );
        cmux_sendMscMessage(dlci, option, ORIGINATOR_CR);
    }
}
#endif /* __CMUX_MODEM_STATUS_SUPPORT__ */

