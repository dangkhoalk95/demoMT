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
#include "cmux_prot.h"
#include "cmux_utl.h"
#include "cmux_hdlr.h"
#include "cmux_lib.h"
#include "cmux_vp_num.h"
#include "cmux_uh.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_prot.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_prot.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_prot.c] "fmt,##arg)

const unsigned char cmux_crctable[256] = { //reversed, 8-bit, poly=0x07
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

void cmux_sendCtrlFrame(kal_uint32 dlci, CmuxControlByteE frameType)
{
    kal_uint8   *pBuff;
    kal_uint16  u2Written = 0;

    kal_uint8   cr;
    kal_uint8   fcs = CMUX_INITIAL_FCS;

    LOGI("dlci = %d, frameType = 0x%x", dlci, frameType);

    //mtk02683 02.04.2010
    //Diffrent frame type should put different CR bit in ADDRESS
    switch (frameType) {
        case UA_FINAL:
        case DM_FINAL:
        case DM_N_FINAL:
            if (cmux_ptr->role == CMUX_ROLE_SERVER) {
                cr = CR_UE_RESPONSE;
            } else {
                cr = CR_TE_RESPONSE;
            }
            break;

        case SABM_POLL:
        case SABM_N_POLL:
        case DISC_POLL:
        case DISC_N_POLL:
            if (cmux_ptr->role == CMUX_ROLE_SERVER) {
                cr = CR_UE_COMMAND;
            } else {
                cr = CR_TE_COMMAND;
            }
            break;

        default:
            return;
    }

    pBuff = (kal_uint8 *) pvPortMalloc(MAX_CTRL_FRAME_SIZE);

    // Constrcut control frame
    *pBuff = CMUX_FLAG_BASIC;
    u2Written++;

    *(pBuff + u2Written) = (kal_uint8) CONSTRUCT_ADDRESS( dlci, cr, 1);
    fcs = CMUX_CALCULATE_FCS(fcs, *(pBuff + u2Written));
    u2Written++;

    *(pBuff + u2Written) = frameType;
    fcs = CMUX_CALCULATE_FCS(fcs, *(pBuff + u2Written));
    u2Written++;

    *(pBuff + u2Written) = (kal_uint8) CONSTRUCT_LENGTH_1(0);
    fcs = CMUX_CALCULATE_FCS(fcs, *(pBuff + u2Written));
    u2Written++;

    *(pBuff + u2Written) = ( kal_uint8) CMUX_OUTPUT_FCS(fcs);
    u2Written++;

    *(pBuff + u2Written) = CMUX_FLAG_BASIC;
    u2Written++;

    cmux_writeToUart(pBuff, u2Written);
}

static void cmux_processDiscFrame(kal_uint32 dlci)
{
    CmuxDlcT *dlc;

    LOGI("dlci = %d", dlci);

    dlc = cmux_findDlcByDlciState(dlci, DLC_ACTIVE);

    if (dlc == NULL) {
        cmux_sendCtrlFrame(dlci, DM_FINAL); // The DLC is inactive, response with DM
        return;
    } else { // MAUI_01660740, mtk02285, 20090407 ~
        cmux_sendCtrlFrame(dlci, UA_FINAL); // The DLC is active, response with UA
        cmux_notifyCPortPlugout(dlc);
        cmux_releaseDlc(dlc);
    }// ~ MAUI_01660740, mtk02285, 20090407
}

static void cmux_processDmFrame(kal_uint32 dlci)
{
    CmuxDlcT *dlc;

    LOGI("dlci = %d", dlci);

    dlc = cmux_findDlcByDlciState(dlci, DLC_ACTIVE);
    if (dlc == NULL) {
        return;
    }

    cmux_notifyCPortPlugout(dlc);
    cmux_releaseDlc(dlc);
}

static void cmux_processUaFrame(kal_uint32 dlci)
{
    CmuxDlcT *pDlc, *dlc;

    LOGI("dlci = %d", dlci);

    pDlc = cmux_findDlcByDlciState(dlci, DLC_RELEASE_PENDING);
    if (pDlc == NULL) {
        // The DLC is not in release pending state, or the DLC does not exist, ignore this UA
        dlc = cmux_findDlcByDlciState(dlci, DLC_ACTIVE);
        if (dlc != NULL) {
            // The DLC is alread avtive, response with UA.
            // and reset all ERM parameters.
            LOGI("find an active DLC");
            LOGI("pn_dlci: %d, pn_cltype: %d, pn_priority = %d, pn_n1 = %d", dlc->pn_dlci, dlc->pn_cltype, dlc->pn_priority, dlc->pn_n1);
            return;
        }

        // the DLCI is not yet active
        // find an DCL whose parameters are already negotiated to use
        dlc = cmux_findDlcByDlciState(dlci, DLC_PARNEG);
        if (dlc == NULL) {
            // no DLCI is associated DLC
            // This DLCI is never used. try to find one free
            dlc = cmux_locateSuitableDlc(dlci);

            CMUX_ASSERT (dlc != NULL);
            LOGI("find a DLC");

            // initialize DLC channel context
            cmux_initializeDlc(dlc, dlci);
        } else {
            LOGI("find a PARNEG DLC");
        }

        // the DLC is ready to be activated.
        dlc->state = DLC_ACTIVE;

        LOGI("pn_dlci: %d, pn_cltype: %d, pn_priority = %d, pn_n1 = %d", dlc->pn_dlci, dlc->pn_cltype, dlc->pn_priority, dlc->pn_n1);

        cmux_allocateBufferSpace(dlc);

        cmux_notifyDlcConnectInd(dlc);

#ifdef __CMUX_MODEM_STATUS_SUPPORT__
        cmux_sendDlcMscStatus(dlci);
#endif // __CMUX_MODEM_STATUS_SUPPORT__

        if (cmux_ptr->retry_state >= CMUX_RETRANSMIT) {
            cmux_stopTimer(CMUX_T2);
            cmux_stopTimer(CMUX_T3);
            cmux_continue_establish_connection();
        }
        return;
    }

    cmux_releaseDlc(pDlc);
    cmux_stopTimer(CMUX_T1);

    if (cmux_ptr->state == CMUX_CLOSING) {
        cmux_closeDownProcedure();
    }
}

static void cmux_processSabmFrame(kal_uint32 dlci)
{
    CmuxDlcT *dlc;

    LOGI("dlci = %d", dlci);

    dlc = cmux_findDlcByDlciState(dlci, DLC_ACTIVE);
    if (dlc != NULL) {
        // The DLC is alread avtive, response with UA.
        // and reset all ERM parameters.
        LOGI("find an active DLC");
        LOGI("pn_dlci: %d, pn_cltype: %d, pn_priority = %d, pn_n1 = %d", dlc->pn_dlci, dlc->pn_cltype, dlc->pn_priority, dlc->pn_n1);
        cmux_sendCtrlFrame(dlci, UA_FINAL);
        return;
    }

    // the DLCI is not yet active
    // find an DCL whose parameters are already negotiated to use
    dlc = cmux_findDlcByDlciState(dlci, DLC_PARNEG);
    if (dlc == NULL) {
        // no DLCI is associated DLC
        // This DLCI is never used. try to find one free
        dlc = cmux_locateSuitableDlc(dlci);

        if (dlc == NULL) {
            // all DLC context is used. reject by DM
            LOGI("didn't find a DLC");
            cmux_sendCtrlFrame(dlci, DM_FINAL);
            return;
        } else {
            LOGI("find a DLC");
        }

        // initialize DLC channel context
        cmux_initializeDlc(dlc, dlci);
    } else {
        LOGI("find a PARNEG DLC");
    }

    // the DLC is ready to be activated.
    dlc->state = DLC_ACTIVE;

    LOGI("pn_dlci: %d, pn_cltype: %d, pn_priority = %d, pn_n1 = %d", dlc->pn_dlci, dlc->pn_cltype, dlc->pn_priority, dlc->pn_n1);

    cmux_allocateBufferSpace(dlc);

    cmux_notifyDlcConnectInd(dlc);

    // response with UA with P/F set to 1
    cmux_sendCtrlFrame(dlci, UA_FINAL);

#ifdef __CMUX_MODEM_STATUS_SUPPORT__
    cmux_sendDlcMscStatus(dlci);
#endif // __CMUX_MODEM_STATUS_SUPPORT__

}

static void cmux_processUihFrame(CmuxDlcT *pDlc)
{
    kal_uint32 rxbuf_usage = cmux_getBufferDataAvail(&(pDlc->rxBuffer));

    LOGI("pn_dlci = %d", pDlc->pn_dlci);

    if ( pDlc->maxRxBufferUsage < rxbuf_usage ) {
        pDlc->maxRxBufferUsage = rxbuf_usage;
    }

    if ( (pDlc->fLocalBusy == KAL_FALSE) && ( rxbuf_usage >= pDlc->highThreshold) ) {

        pDlc->fLocalBusy = KAL_TRUE;

        // WM_DUN_FLC, MAUI_01679336, mtk02285, 20090430
        cmux_suspendPeerDlc(pDlc->pn_dlci); // send MSC command for NOT CL4 channel

        // Send flow control message by uih frame for CL2 only - nothing matter with flow control
        if (pDlc->pn_cltype == CMUX_CL2) {
            cmux_schedulerDlcReady(pDlc);
        }
    }

    // mtk02683 03.30.2010 :
    // Cmux will be suspended here if the priority of owner is higher than cmux
    // The flag, fLocalBusy, may be set and suspending MSC may be sent after
    // owner read data in rx buffer. In this case, peer will never be resumed.
    // This scenario is found during development of WinMo VT.
    if ( pDlc->fNotifyReadyToRead == KAL_TRUE ) {
#if 0
        pDlc->fNotifyReadyToRead = KAL_FALSE;
#endif
        LOGI("pn_cltype = %d, stateCL4Rx = %d", pDlc->pn_dlci, pDlc->stateCL4Rx);
        if (pDlc->pn_cltype == CMUX_CL4) {
            if (pDlc->stateCL4Rx == CMUX_CL4_BEGIN) {
                cmux_notifyReadyToRead(pDlc);
            }
        } else {
            cmux_notifyReadyToRead(pDlc);
        }
    }
}

static void cmux_processDlcFrame(void)
{
    kal_uint32 dlci;

    dlci = GET_DLCI_OF_ADDR(cmux_ptr->addressByte);
    LOGI("dlci = %d, controlByte = 0x%x", dlci, cmux_ptr->controlByte);
    // should we check EA bit ? current, only one byte
    // of address should be present
    switch (cmux_ptr->controlByte) {
        case SABM_POLL:
            // response with UA with P/F set to 1
            // and set the Channel active, notify L4C
            if (GET_CR_OF_ADDR(cmux_ptr->addressByte) == CR_TE_COMMAND) {
                cmux_processSabmFrame(dlci);
            }
            break;
        case UA_FINAL:
            cmux_processUaFrame(dlci);
            break;
        case DISC_POLL:
            // response with UA with P/F set to 1
            // and set the cnaeel inactive, notify owner module
            cmux_processDiscFrame(dlci);
            break;
        case SABM_N_POLL:
        case DISC_N_POLL:
            // ignore it, as per 27.010 clause 5.4.4.1
            break;
        case DM_FINAL:
        case DM_N_FINAL:
            // unsolicited DM should be process irrespective of the P/F setting
            cmux_processDmFrame(dlci);
            break;
        case UIH_FRAME_POLL:
        case UIH_FRAME:
            if (cmux_ptr->readingDlc != NULL) {
                cmux_admitReservedBuffer(&cmux_ptr->readingDlc->rxBuffer,
                                         cmux_ptr->writePtr);

                if (CMUX_DLCI_IS_CCH(dlci)) {
                    /* Save if UIH POLL here */
                    cmux_ptr->cchUIHPoll = cmux_ptr->controlByte;
                    cmux_handleControlMessage();
                } else {
                    cmux_processUihFrame(cmux_ptr->readingDlc);
                }
            }
            break;
        default:
            // ignore the frame.
            break;
    }
}

static void cmux_processFrame(void)
{
    kal_uint32 dlci;

    dlci = GET_DLCI_OF_ADDR( cmux_ptr->addressByte);

    LOGI("dlci = %d", dlci);

    if ( dlci == 0 && cmux_ptr->controlByte == DISC_POLL ) {
        cmux_restoreTask();
        return;
    }

    // should we check EA bit ? current,
    // only one byte of address should be present
    switch (cmux_ptr->state) {
        case CMUX_STARTUP:
            /* accept only SABM with DLCI 0 */
            if (dlci == 0 && cmux_ptr->controlByte == SABM_POLL) {
                // initialize CCH context
                cmux_initializeDlc( &cmux_ptr->dlcs[0], 0 );
                cmux_allocateBufferSpace( &cmux_ptr->dlcs[0] );
                cmux_ptr->dlcs[0].state = DLC_ACTIVE;
                cmux_ptr->dlcs[0].pn_cltype = CMUX_CL1;

                cmux_ptr->state = CMUX_ACTIVE;

                cmux_sendCtrlFrame(0, UA_FINAL);
            } else if (dlci == 0 && cmux_ptr->controlByte == UA_FINAL) {
                // initialize CCH context
                cmux_initializeDlc( &cmux_ptr->dlcs[0], 0 );
                cmux_allocateBufferSpace( &cmux_ptr->dlcs[0] );
                cmux_ptr->dlcs[0].state = DLC_ACTIVE;
                cmux_ptr->dlcs[0].pn_cltype = CMUX_CL1;

                cmux_ptr->state = CMUX_ACTIVE;

                cmux_stopTimer(CMUX_T2);
                cmux_stopTimer(CMUX_T3);
                cmux_negotiate_and_set_parameter();
            }
            break;
        case CMUX_ACTIVE:
            cmux_processDlcFrame();
            break;
        case CMUX_INACTIVE:
            CMUX_ASSERT(0);
            break;
        case CMUX_CLOSING:
            /* handle only DLCI 0 */
            cmux_processDlcFrame();
            break;
    }
}

void cmux_processUartData(kal_uint8 *pU1Buff, kal_uint32 u4Length)
{

    kal_uint8 *pU1Read = NULL; /* Current processed data ptr*/
    kal_uint32 u4SpaceLeft = 0;
    kal_uint32  u4DataLeft = 0;

    for (pU1Read = pU1Buff ;
            pU1Read < ( pU1Buff + u4Length) ;
            pU1Read++) {
        LOGI("readState = %d", cmux_ptr->readState);
        switch (cmux_ptr->readState) {
            case CMUX_WAIT_FLAG:
                if (*pU1Read == CMUX_FLAG_BASIC) {
                    cmux_ptr->readState  = CMUX_WAIT_ADDRESS;
                }
                break;

            case CMUX_WAIT_ADDRESS:
                if (*pU1Read != CMUX_FLAG_BASIC) {
                    /* Do sync here */
                    if (GET_DLCI_OF_ADDR( *pU1Read) <= 32) { // DLCI <= 32 is just a simple check to lost sync or not !
                        cmux_ptr->addressByte = *pU1Read;
                        cmux_ptr->u1Fcs = CMUX_CALCULATE_FCS(CMUX_INITIAL_FCS, *pU1Read);
                        cmux_ptr->readingDlc  = cmux_findDlcByDlciState(GET_DLCI_OF_ADDR(*pU1Read), DLC_ACTIVE);

                        cmux_ptr->readState  = CMUX_WAIT_CONTROL;
                    } else {
                        cmux_ptr->readState  = CMUX_WAIT_FLAG;
                    }
                }
                break;

            case CMUX_WAIT_CONTROL:
                if (((*pU1Read & 0x03) == 0x03)) {
                    cmux_ptr->controlByte = *pU1Read;
                    cmux_ptr->u1Fcs = CMUX_CALCULATE_FCS(cmux_ptr->u1Fcs, *pU1Read);
                    // To note if first byte of Length or second byte of length
                    cmux_ptr->u2FrameLength = INVALID_LENGTH_PATTERN;

                    cmux_ptr->readState  = CMUX_WAIT_LENGTH;
                } else { /* Not a valid control byte */
                    cmux_ptr->readState = CMUX_WAIT_FLAG;
                }
                break;

            case CMUX_WAIT_LENGTH:

                if (cmux_ptr->u2FrameLength == INVALID_LENGTH_PATTERN) {
                    cmux_ptr->u2FrameLength = LENGTH_ONE_OCTET(*pU1Read);
                    if (IS_SINGLE_LENGTH_OCTET(*pU1Read)) {
                        cmux_ptr->readState  = CMUX_INFORMATION;
                    }
                    // else wait for next length byte, the following code
                } else {
                    cmux_ptr->u2FrameLength |= (*pU1Read) << 7;
                    cmux_ptr->readState  = CMUX_INFORMATION;
                }
                cmux_ptr->u1Fcs = CMUX_CALCULATE_FCS(cmux_ptr->u1Fcs, *pU1Read);

                if (cmux_ptr->readState  == CMUX_INFORMATION) {
                    cmux_ptr->u2FrameRead = 0;

                    if ( cmux_ptr->u2FrameLength == 0 ) {
                        cmux_ptr->readState = CMUX_WAIT_FCS;
                    } else {
                        // special handlig of CL2 and CL4
                        if (cmux_ptr->readingDlc != NULL) {
                            if (cmux_ptr->readingDlc->pn_cltype == CMUX_CL2) {
                                cmux_ptr->u2FrameLength --;
                                cmux_ptr->readState = CMUX_WAIT_CL2_HEADER;
                            } else if (cmux_ptr->readingDlc->pn_cltype == CMUX_CL4) {
                                cmux_ptr->u2FrameLength --;
                                cmux_ptr->readState = CMUX_WAIT_CL4_HEADER;
                            }

                            // let's check available here!
                            cmux_ptr->writePtr = cmux_reserveBufferSpace(&cmux_ptr->readingDlc->rxBuffer, &u4SpaceLeft);

                            if (u4SpaceLeft < cmux_ptr->u2FrameLength) {
                                // Data overrun
                                cmux_ptr->u4DataOverrun ++;
                            }
                        }// end of if cmux_ptr->readingDlc != NULL
                    }
                }// end of State into INFORMATION

                break;

            case CMUX_WAIT_CL2_HEADER:

                // Decode the CL2 Header byte.
                cmux_protocol_decodeV24(cmux_ptr->readingDlc, *pU1Read);

                cmux_ptr->readState = CMUX_INFORMATION;
                break;

            case CMUX_WAIT_CL4_HEADER:

                switch (*pU1Read) {
                    case CL4_FIRST_FRAME:
                        cmux_ptr->readingDlc->stateCL4Rx = CMUX_CL4_PROCESS;
                        break;
                    case CL4_MIDDLE_FRAME:
                        break;
                    case CL4_LAST_FRAME:
                        cmux_ptr->readingDlc->stateCL4Rx = CMUX_CL4_BEGIN;
                        break;
                    case CL4_ENTIRE_FRAME:
                        cmux_ptr->readingDlc->stateCL4Rx = CMUX_CL4_BEGIN;
                        break;
                    default:
                        break;
                }

                cmux_ptr->readState = CMUX_INFORMATION;
                break;

            case CMUX_INFORMATION:
                /*
                * copy all left data into data buffer
                * we should try to get cmux_ptr->frameLength of data
                * Only for UIH type, this state will be
                * entered, or there should be something wrong
                * and the space is enough for this frame
                */
                /* Remaining data size in read buffer */
                u4DataLeft  = u4Length - ( pU1Read - pU1Buff);

                if (u4DataLeft > (kal_uint32) (cmux_ptr->u2FrameLength - cmux_ptr->u2FrameRead)) {
                    // This is at least one entire frame in the read buffer.
                    // We can process it at once.
                    u4DataLeft = (kal_uint32) cmux_ptr->u2FrameLength - cmux_ptr->u2FrameRead;

                    cmux_ptr->readState  = CMUX_WAIT_FCS;
                }

                if (cmux_ptr->readingDlc != NULL) {
                    cmux_ptr->writePtr = cmux_writeReservedBuffer(
                                             &cmux_ptr->readingDlc->rxBuffer, cmux_ptr->writePtr, pU1Read,
                                             u4DataLeft);
                } else {
                }

                cmux_ptr->u2FrameRead += (kal_uint16) u4DataLeft;
                // for pU1Read will be increased in the loop control
                pU1Read +=  u4DataLeft - 1;

                break;

            case CMUX_WAIT_FCS:
                if (CMUX_IS_FCS_CORRECT(cmux_ptr->u1Fcs, *pU1Read)) {
                    // One correct frame. process this
                    cmux_processFrame();
                } else {
                    LOGE("invalid frame");
                    // Incorrect frame, discard it.
                }
                cmux_ptr->readState  = CMUX_WAIT_FLAG;
                break;

        }// end of switch
    }// end of for loop
    // data in the buffer is all processed.
    return;
}

static kal_uint8 cmux_getCL4Flag(CmuxDlcT *pDlc, kal_bool is_end)
{
    switch (pDlc->stateCL4Tx) {
        case CMUX_CL4_BEGIN:
            if (KAL_TRUE == is_end) {
                return CL4_ENTIRE_FRAME;
            } else {
                pDlc->stateCL4Tx = CMUX_CL4_PROCESS;
                return CL4_FIRST_FRAME;
            }

        case CMUX_CL4_PROCESS:
            if (KAL_TRUE == is_end) {
                pDlc->stateCL4Tx = CMUX_CL4_BEGIN;
                return CL4_LAST_FRAME;
            } else {
                return CL4_MIDDLE_FRAME;
            }

        default:
            CMUX_ASSERT(0);
            return CL4_ENTIRE_FRAME; /* Avoid compile warning */
    }
}

static void cmux_writeUihFrame(CmuxDlcT *pDlc)
{
    kal_uint8 *pHeader;
    kal_uint8 *pPdu;
    kal_uint16 u2ToWrite;
    kal_uint32 fcs;
    kal_uint32 u4IfieldLength;
    kal_uint32 u4DataLength;
    kal_uint8 u1CL4Flag = CL4_ENTIRE_FRAME;

    LOGI("pn_dlci = %d", pDlc->pn_dlci);

    // before header can be construct, length should be known.
    // We support CL type 2, which will use first byte of I field
    if (pDlc->fPeerBusy == KAL_TRUE) {
        if (pDlc->pn_cltype == CMUX_CL2) {
            u4IfieldLength = 1;
            // if peer busy, we only write one byte
            u4DataLength = 0;
        } else {
            // only CL2 we can transmit Modem status to peer
            return;
        }
    } else {
        // peer not busy
        u4DataLength = cmux_getBufferDataAvail(& pDlc->txBuffer);

        /* CL2 and CL4 has one status byte */
        if ((pDlc->pn_cltype == CMUX_CL2) || (pDlc->pn_cltype == CMUX_CL4)) {
            if (u4DataLength > ((kal_uint32)(pDlc->pn_n1 - 1))) {
                u4DataLength = pDlc->pn_n1 - 1;
                if (pDlc->pn_cltype == CMUX_CL4) {
                    u1CL4Flag = cmux_getCL4Flag(pDlc, KAL_FALSE);
                }
            } else if (pDlc->pn_cltype == CMUX_CL4) {
                u1CL4Flag = cmux_getCL4Flag(pDlc, KAL_TRUE);
            }

            u4IfieldLength = u4DataLength + 1;
        } else {
            if (u4DataLength > pDlc->pn_n1) {
                u4DataLength = pDlc->pn_n1;
            }
            u4IfieldLength = u4DataLength;
        }
    }

    //mtk02683 01.28.2010
    //Return if cmux try to write an empty frames
    if (u4IfieldLength == 0) {
        return;
    }

    pHeader = pPdu = (kal_uint8 *) pvPortMalloc(u4IfieldLength + CMUX_HEADER_MAX_SIZE);

    fcs = CMUX_INITIAL_FCS;

    *pHeader = CMUX_FLAG_BASIC;
    pHeader++;

    // per 5.4.3.1  responding station set CR to 0 and P-bit to 0
    if (cmux_ptr->role == CMUX_ROLE_SERVER) {
        *pHeader = CONSTRUCT_ADDRESS(pDlc->pn_dlci, 0, 1);
    } else {
        *pHeader = CONSTRUCT_ADDRESS(pDlc->pn_dlci, 1, 1);
    }
    fcs = CMUX_CALCULATE_FCS(fcs, *pHeader);
    pHeader++;

    if ((pDlc->pn_dlci == 0) && (cmux_ptr->cchUIHPoll == UIH_FRAME_POLL)) {
        *pHeader = UIH_FRAME_POLL;
        cmux_ptr->cchUIHPoll = 0; /* Reset the POLL bit */
    } else {
        *pHeader = UIH_FRAME;
    }

    fcs = CMUX_CALCULATE_FCS(fcs, *pHeader);
    pHeader++;
    // Header

    if (u4IfieldLength > MAX_ONE_BYTE_LENGTH) {
        *pHeader = (kal_uint8) CONSTRUCT_LENGTH_1_of_2(u4IfieldLength);
        fcs = CMUX_CALCULATE_FCS(fcs, *pHeader);
        pHeader++;
        *pHeader = (kal_uint8) CONSTRUCT_LENGTH_2_of_2(u4IfieldLength);
        fcs = CMUX_CALCULATE_FCS(fcs, *pHeader);
        pHeader++;

    } else {
        *pHeader = (kal_uint8) CONSTRUCT_LENGTH_1(u4IfieldLength);
        fcs = CMUX_CALCULATE_FCS(fcs, *pHeader);
        pHeader++;
    }

    // Constuct Fill MSC byte
    if (pDlc->pn_cltype == CMUX_CL2) {
        *pHeader = cmux_protocol_encodeV24( pDlc );
        pHeader++;
    } else if (pDlc->pn_cltype == CMUX_CL4) {
        *pHeader = u1CL4Flag;
        pHeader++;
    }

    // Finish Header construction.

    /********************************************************
    *  Copy Information field
    */
    if (u4DataLength > 0) {
        cmux_readBuffer(& pDlc->txBuffer, pHeader, u4DataLength);
        pHeader += u4DataLength;
    }

    *pHeader = CMUX_OUTPUT_FCS(fcs);
    pHeader ++;
    *pHeader = CMUX_FLAG_BASIC;
    pHeader ++;

    u2ToWrite = (kal_uint16)(pHeader - pPdu);

    cmux_writeToUart(pPdu, u2ToWrite);
}

void cmux_scheduleWrite(void)
{
    /* Two possibilites here
    *   First, the is no "currently processed" frame
    *   Second, previous write attempt is not fullfiled, and we must
    *   process it first.
    */
    CmuxDlcT *pDlc;

    // Choose the channel with highest priority
    pDlc = cmux_schedulerPickReadyDlc();

    if (pDlc == NULL) {
        /* All priority marked suspend. We double check   *
         * if one DLC has data but in suspend             */
        kal_uint32 i;
        CmuxDlcT *target = NULL;

        for (i = 0; i < CMUX_CHANNEL_NUM; i++) {
            target = & cmux_ptr->dlcs[i];

            if ((target->state == DLC_ACTIVE) && (target->fPeerBusy == KAL_FALSE) &&
                    (cmux_isBufferEmpty(& target->txBuffer) == KAL_FALSE)) {
                pDlc = target;
                break;
            }
        }
    }

    if (pDlc == NULL) {
        /* do nothing */
        return;
    }

    // The DLC is scheduled to write

    // Suspend the DLC
    cmux_schedulerDlcSuspend(pDlc);

    // Construct UIH frame and write
    cmux_writeUihFrame(pDlc);

    if ( cmux_isBufferEmpty( &pDlc->txBuffer) == KAL_FALSE ) {
        cmux_schedulerDlcReady(pDlc);
    }

    if (pDlc->fNotifyReadyToWrite == KAL_TRUE) {
        pDlc->fNotifyReadyToWrite = KAL_FALSE;
        cmux_notifyReadyToWrite(pDlc);
    }

    return;
}

void cmux_closeDownProcedure(void)
{
    CmuxDlcT *pDlc = NULL;
    kal_uint32 i;

    for ( i = 1; i < CMUX_CHANNEL_NUM; i++) {
        if ( cmux_ptr->dlcs[i].state == DLC_ACTIVE) {
            pDlc = & (cmux_ptr->dlcs[i]);
            break;
        }
    }

    if (pDlc == NULL) {
        if ( cmux_ptr->dlcs[0].state == DLC_ACTIVE) {
            // There is no active DLC. Start CCH close down procedure.
            cmux_sendCldMessage(KAL_TRUE);
            //Start T2 & T3
            cmux_ptr->state = CMUX_CLOSING;
            cmux_startTimer(CMUX_T2, DEFAULT_T2_VALUE);
            cmux_startTimer(CMUX_T3, DEFAULT_T3_VALUE);
        } else {
            cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_REQ, NULL);
        }
    } else {
        cmux_notifyCPortPlugout(pDlc);
        cmux_schedulerDlcSuspend(pDlc);
        cmux_sendCtrlFrame(pDlc->pn_dlci, DISC_POLL);
        // should we release the buffer here ?
        // I guess not, but until UA response is received.
        cmux_startTimer(CMUX_T1, DEFAULT_T1_VALUE);
        pDlc->state = DLC_RELEASE_PENDING;
    }
}

void cmux_sendSabmProcedure(cmux_channel_id_t channel_id)
{
    cmux_sendCtrlFrame(channel_id, SABM_POLL);
    //Start T2 & T3
    cmux_ptr->retry_state = CMUX_RETRANSMIT + channel_id;
    cmux_startTimer(CMUX_T2, DEFAULT_T2_VALUE);
    cmux_startTimer(CMUX_T3, DEFAULT_T3_VALUE);
}

void cmux_restoreTask( void )
{
    kal_uint8 i;

    // Stop all the timers
    cmux_stopTimer(CMUX_T1);
    cmux_stopTimer(CMUX_T2);
    cmux_stopTimer(CMUX_T3);

    // Free all the TX buffers
    while (! lcd_lqueue_is_empty(cmux_ptr->writeQueue)) {
        peer_buff_struct *pPeer =
            (peer_buff_struct *) lcd_lqueue_remove(cmux_ptr->writeQueue, NULL);
        vPortFree(pPeer);
    }

    // Disconnect all DLC, if they are not disconnected
    for ( i = 1; i < CMUX_CHANNEL_NUM; i++ ) {
        CmuxDlcT *pDlc = &cmux_ptr->dlcs[i];

        if (pDlc->state == DLC_ACTIVE) {
            cmux_notifyCPortPlugout(pDlc);
            cmux_schedulerDlcSuspend(pDlc);
            cmux_releaseDlc(pDlc);
        }
    }

    // Notify UART handler about to release of UART.
    // UART Handler should finish writing and then response
    cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_REQ, NULL);

    return;

}

void cmux_handleT1Expiry(void)
{
    kal_uint32 i;

    for (i = 1; i < CMUX_CHANNEL_NUM; i++) {
        CmuxDlcT *pDlc = & cmux_ptr->dlcs[i];

        if (pDlc->state == DLC_RELEASE_PENDING) {
            cmux_releaseDlc(pDlc);
            break;
        }
    }

    if ( cmux_ptr->state == CMUX_CLOSING ) {
        cmux_closeDownProcedure();

        if ( cmux_ptr->fUartSendSuspend == KAL_FALSE ) {
            LOGI("schedule to write");
            cmux_scheduleWrite();
        }
    }
}

void cmux_protocol_decodeV24( CmuxDlcT *pDlc, kal_uint8 signal )
{
    // Decode Flow Control bit
    if ( CMUX_V24_BIT_ISON ( signal, CMUX_V24_FC_BIT )) {
        // 1 means FLow control OFF
        if ( pDlc->fPeerBusy == KAL_FALSE ) {
            pDlc->fPeerBusy = KAL_TRUE;

            // Put the DLC into suspend state
            cmux_schedulerDlcSuspend(pDlc);
        }
    } else {
        // 0 means Flow control ON
        if ( pDlc->fPeerBusy == KAL_TRUE ) {
            pDlc->fPeerBusy = KAL_FALSE;

            // Put the DLC into READY state if it is necessary
            if ( cmux_schedulerIsDlcReady(pDlc) == KAL_FALSE ) {
                if ( cmux_isBufferEmpty( &(pDlc->txBuffer)) == KAL_FALSE ) {
                    cmux_schedulerDlcReady(pDlc);
                }
            }
        }
    }
}

kal_uint8 cmux_protocol_encodeV24( CmuxDlcT *pDlc )
{
    kal_uint8 signal = CMUX_V24_EA_BIT | CMUX_V24_RTC_BIT;

    if ( pDlc->fLocalBusy == KAL_FALSE ) {
        signal |= CMUX_V24_RTR_BIT;
    } else {
        signal |= CMUX_V24_FC_BIT;
    }

    if ( pDlc->fSignalRi ) {
        signal |= CMUX_V24_IC_BIT;
    }

    if ( pDlc->fSignalDcd ) {
        signal |= CMUX_V24_DV_BIT;
    }

    return signal;
}

