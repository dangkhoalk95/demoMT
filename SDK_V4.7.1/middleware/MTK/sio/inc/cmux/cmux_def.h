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

#ifndef __CMUX_DEF_H__
#define __CMUX_DEF_H__

#include "cmux_porting.h"
#include "cmux_lib.h"
#include "cmux_vp_num.h"
#include "cmux_msg_id.h"
#include "cmux_custom.h"

#define CMUX_CHANNEL_NUM        ( 1 + CMUX_DLC_NUM )
#define CMUX_SUSPEND_PRIORITY   0xFF

// TIMER
#define CMUX_TOT_TIMER          3
#define DEFAULT_T1_VALUE        100
#define DEFAULT_T2_VALUE        300
#define DEFAULT_T3_VALUE        10000

// PROTOCOL
#define CMUX_HEADER_MAX_SIZE    7
#define CMUX_DEFAULT_N1         512
#define CMUX_MAX_N1_VALUE       1510
#define CMUX_MIN_N1_VALUE       16

#define CMUX_MAX_UH_READ_BUFFER 512

// KAL
#define CMUX_ASSERT(expr)   {if(!(expr)) { configASSERT(0); }else {}}

// Common
#define CMUX_INIT_CONTEXT(ptr) (memset(ptr, 0, sizeof(*ptr)))
#define CMUX_MOVE_STATE(ptr,b) (ptr->state = b)

// DLCI
#define CMUX_DLCI_IS_CCH(dlci)  ((dlci) == 0)

// Validation
#define CMUX_VALIDATE_ID(id)        ((id < CMUX_CHANNEL_NUM))
#define CMUX_VALIDATE_VP(port)      ((port >= 1) && (port < (1 + CMUX_VP_NUM) ))
#define CMUX_VALIDATE_DLCI(dlci)    ((dlci > 0) && (dlci < 32))

// Mapping
#define CMUX_MAP_PDLC2ID(pDlc)      ((kal_uint8)(pDlc - cmux_ptr->dlcs))
#define CMUX_MAP_VP2ID(port)        (cmux_custom_getChannelIndex((kal_uint8)port))

// Mode
#define CMUX_MODE_SETNORMAL(p)      ((p->mode) = CMUX_MODE_NORMAL)
#define CMUX_MODE_SETEM(p)          ((p->mode) = CMUX_MODE_ENGINEER)
#define CMUX_MODE_SETTEST(p)        ((p->mode) = CMUX_MODE_TEST)
#define CMUX_MODE_ISNORMAL(p)       ((p->mode) == CMUX_MODE_NORMAL)
#define CMUX_MODE_ISEM(p)           ((p->mode) > CMUX_MODE_NORMAL)
#define CMUX_MODE_ISTEST(p)         ((p->mode) == CMUX_MODE_TEST)

typedef enum {
    CMUX_INACTIVE,
    CMUX_ACTIVE,
    CMUX_STARTUP,
    CMUX_CLOSING
} CmuxStateE;

typedef enum {
    CMUX_RETRANSMIT
} CmuxTransmitStateE;

typedef enum {
    CMUX_MODE_NORMAL,
    CMUX_MODE_ENGINEER,
    CMUX_MODE_TEST
} CmuxModeE;

typedef enum {
    CMUX_T1,
    CMUX_T2,
    CMUX_T3
} CmuxTimerIdE;

typedef enum {
    CMUX_FRMAE_UIH = 0,     // UIH frame
    CMUX_FRAME_UI  = 1,     // UI frame
    CMUX_FRAME_I   = 2      // I frame
} CmuxFrameTypeE;

typedef enum {
    CMUX_CL1 = 0,           // Type 1 - unstructured stream
    CMUX_CL2 = 1,           // Type 2 - unstructured stream with modem status
    CMUX_CL3 = 2,           // Type 3 - uninterruptible Framed Data
    CMUX_CL4 = 3            // Type 4 - Interruptible Framed data
} CmuxClTypeE;

/* Convergence Layer 4
 *          F   B   0   0   0   0   0   EA
 * First    0   1   0   0   0   0   0   1
 * Middle   0   0   0   0   0   0   0   1
 * Last     1   0   0   0   0   0   0   1
 * Entire   1   1   0   0   0   0   0   1
 */
typedef enum {
    CL4_MIDDLE_FRAME = 0x01,
    CL4_FIRST_FRAME = 0x41,
    CL4_LAST_FRAME = 0x81,
    CL4_ENTIRE_FRAME = 0xC1
} CmuxCl4FrameTypeE;

typedef  enum  {
    CMUX_WAIT_FLAG,
    CMUX_WAIT_ADDRESS,
    CMUX_WAIT_CONTROL,
    CMUX_WAIT_LENGTH,
    CMUX_WAIT_CL2_HEADER,
    CMUX_WAIT_CL4_HEADER,
    CMUX_INFORMATION,
    CMUX_WAIT_FCS
} CmuxReadStateE;

typedef  enum  {
    CMUX_CL4_BEGIN,
    CMUX_CL4_PROCESS
} CmuxCL4StateE;

typedef  enum  {
    CMUX_WRITE_HEADER,
    CMUX_WRITE_INFORMATION,
    CMUX_WRITE_FCS,
    CMUX_WRITE_TAIL_FLAG
} CmuxWriteStateE;

typedef enum {
    DLC_INACTIVE,
    DLC_PARNEG,
    DLC_ACTIVE,
    DLC_RELEASE_PENDING
} CmuxDlcStateE;

typedef enum {
    UI_FRAME        = 0x03,
    DM_N_FINAL      = 0x0F,
    DM_FINAL        = 0x1f,
    SABM_N_POLL     = 0x2f,
    SABM_POLL       = 0x3f,
    DISC_N_POLL     = 0x43,
    DISC_POLL       = 0x53,
    UA_N_FINAL      = 0x63,
    UA_FINAL        = 0x73,
    UIH_FRAME       = 0xEF,
    UIH_FRAME_POLL  = 0xFF
} CmuxControlByteE;

typedef struct {

    /* DLC related parameters */
    kal_uint8           pn_dlci;        // DLCI value
    CmuxClTypeE         pn_cltype;      // Convergence Layer Type
    kal_uint8           pn_priority;    // Priority
    kal_uint16          pn_n1;          // N1

    CmuxDlcStateE       state;              // Flag to specify if this DLC is active
    kal_uint8           u1AssignedPriority; // priority used for scheduling

    /* Buffer */
    CmuxRingBufferT     txBuffer;       // TX RingBuffer
    CmuxRingBufferT     rxBuffer;       // RX RingBuffer
    kal_uint32          highThreshold;  // Low threshold of buffer
    kal_uint32          lowThreshold;   // High Threshold of buffer

    /* Flag */
    kal_bool            fNotifyReadyToRead;
    kal_bool            fNotifyReadyToWrite;

    kal_bool            fPeerBusy;          // UE->TE map to FLOW Control RTS
    kal_bool            fLocalBusy;         // UE<-TE map to FLOW Control RTS
    kal_bool            fSignalDcd;         // UE<-TE DCD
    kal_bool            fSignalRi;          // UE<-TE RI
    kal_bool            fSignalBreak;

    CmuxCL4StateE       stateCL4Tx;
    CmuxCL4StateE       stateCL4Rx;

    /* Statistics */
    kal_uint32          maxRxBufferUsage;
} CmuxDlcT;

typedef struct {

    /* MUX related parameters */
    CmuxStateE          state;
    CmuxModeE           mode;
    CmuxTransmitStateE  retry_state;

    hal_uart_port_t     port; // The physical port multiplexer over.
    // If CMUX is not yet active, its value
    // should be set to INVALID_PORT.
    cmux_role_t         role;

    /* RX: UART Read Context */
    CmuxReadStateE      readState;
    kal_uint8           addressByte;
    kal_uint8           controlByte;
    kal_uint16          u2FrameLength;
    kal_uint8           u1Fcs;

    kal_uint8           cchUIHPoll;

    CmuxDlcT            *readingDlc;
    kal_uint16          u2FrameRead;
    kal_uint8           *writePtr; // for rxBuffer

    /* TX: UART Write Context */
    lcd_lqueue          *writeQueue;

    kal_bool            fUartSendSuspend;
    kal_bool            fSendCLD;

    /* Statistics */
    kal_uint32          u4DataOverrun;

    /* Sub-Context */
    TimerHandle_t       timer[CMUX_TOT_TIMER];
    CmuxDlcT            dlcs[CMUX_CHANNEL_NUM];

} CmuxContextT;

extern CmuxContextT *const cmux_ptr;

#endif /* __CMUX_DEF_H__ */

