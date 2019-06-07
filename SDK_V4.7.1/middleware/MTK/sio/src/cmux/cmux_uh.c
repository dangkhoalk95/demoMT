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

#include "cmux_uh.h"
#include "cmux_def.h"
#include "cmux_struct.h"
#include "cmux_hdlr.h"
#ifndef __CMUX_UT__
#include "msm.h"
#endif

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_uh.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_uh.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_uh.c] "fmt,##arg)

static CmuxUartHandlerT cmux_uart_handler_context;
CmuxUartHandlerT *const cmux_uh_ptr = & cmux_uart_handler_context;

static void cmux_uh_convert_to_printf_char(kal_uint8 *buffaddr, kal_uint16 length, uint8_t *buffer)
{
    uint32_t i;
    uint8_t ch;

    for (i = 0; i < length && i < 100 /* printf first 100 characters only */; i++) {
        ch = (buffaddr[i] & 0xF0) >> 4;
        if (ch >= 0 && ch <= 9) {
            buffer[3 * i] = ch + '0';
        } else if (ch >= 0x0A && ch <= 0x0F) {
            buffer[3 * i] = ch - 0x0A + 'A';
        }

        ch = (buffaddr[i] & 0x0F);
        if (ch >= 0 && ch <= 9) {
            buffer[3 * i + 1] = ch + '0';
        } else if (ch >= 0x0A && ch <= 0x0F) {
            buffer[3 * i + 1] = ch - 0x0A + 'A';
        }

        if (i == length - 1) {
            buffer[3 * i + 2] = '\0';
        } else {
            buffer[3 * i + 2] = ' ';
        }
    }
    buffer[3 * i] = '\0';
}

static kal_uint16 cmux_uh_GetBytes(hal_uart_port_t port, kal_uint8 *buffaddr, kal_uint16 length)
{
    uint32_t read_data_len;
    uint8_t *buffer;
    read_data_len = hal_uart_receive_dma(port, buffaddr, length);
    buffer = (kal_uint8 *) pvPortMalloc(3 * (read_data_len + 1));
    cmux_uh_convert_to_printf_char(buffaddr, read_data_len, buffer);
    LOGI("read data **********[%s]**********, read_data_len = %d", buffer, read_data_len);
    vPortFree(buffer);
    return read_data_len;
}

static kal_uint16 cmux_uh_PutBytes(hal_uart_port_t port, kal_uint8 *buffaddr, kal_uint16 length)
{
    uint32_t write_data_len;
    uint8_t *buffer;
#ifndef __CMUX_UT__
    if (msm_query_status_from_modem() == MSM_STATUS_INACTIVE) {
        if (msm_trigger_to_modem(MSM_TRIGGER_TYPE_WAKEUP) != MSM_RET_OK) {
            LOGE("wakeup modem failed!");
        }
    }
#endif
    write_data_len = hal_uart_send_dma(port, (const uint8_t *)buffaddr, length);
    buffer = (kal_uint8 *) pvPortMalloc(3 * (write_data_len + 1));
    cmux_uh_convert_to_printf_char(buffaddr, write_data_len, buffer);
    LOGI("write data **********[%s]**********, write_data_len = %d", buffer, write_data_len);
    vPortFree(buffer);
    return write_data_len;
}

static void cmux_uh_readUart(void)
{
    kal_uint16 u2DataRead;
    kal_uint8  *pPdu;
#ifdef __CMUX_UT__
    const char *closedownCmd = "AT+CMUX=2";
    const char *closedownRsp = "OK";
    uint32_t write_data_len;
#endif

    pPdu = (kal_uint8 *) pvPortMalloc(CMUX_MAX_UH_READ_BUFFER);

    if (cmux_ptr->fSendCLD == true) {
        cmux_ptr->fSendCLD = false;
        u2DataRead = cmux_uh_GetBytes(cmux_uh_ptr->port, pPdu, 8);
        LOGW("read only 8 bytes because urc data may be sent by modem in this time");
    } else {
    u2DataRead = cmux_uh_GetBytes(cmux_uh_ptr->port, pPdu, CMUX_MAX_UH_READ_BUFFER);
    }

    if (u2DataRead == 0) {
        vPortFree(pPdu);
        return;
    }

#ifdef __CMUX_UT__
    if (strncasecmp((const char *)pPdu, closedownCmd, strlen(closedownCmd)) == 0) {
        write_data_len = strlen(closedownRsp);
        hal_uart_send_dma(cmux_uh_ptr->port, (const uint8_t *)closedownRsp, write_data_len);
        LOGI("send data (len: %d) to uart: %s", write_data_len, closedownRsp);
        cmux_send_message(MSG_ID_CMUX_CLOSE_DOWN_REQ, (local_para_struct *) NULL);
        vPortFree(pPdu);
        return;
    }
#endif

    if (u2DataRead < CMUX_MAX_UH_READ_BUFFER) {
    } else {
        cmux_uh_ptr->fUartReadContinue = KAL_TRUE;
    }

    cmux_handleUartDataInd(pPdu, u2DataRead);
    vPortFree(pPdu);
}

static void cmux_uh_writeUart(void)
{
    kal_uint16  bufLength;
    kal_uint16  writenLength;
    kal_uint8   *dataPtr;

    LOGI("writingBuffer = 0x%x, u2Written = %d, u2ToWrite = %d", cmux_uh_ptr->writingBuffer, cmux_uh_ptr->u2Written, cmux_uh_ptr->u2ToWrite);

    dataPtr = cmux_uh_ptr->writingBuffer + cmux_uh_ptr->u2Written;
    bufLength = cmux_uh_ptr->u2ToWrite;

    writenLength = cmux_uh_PutBytes(cmux_uh_ptr->port, dataPtr, bufLength);

    if (writenLength < bufLength) {
        cmux_uh_ptr->u2Written = writenLength;
        cmux_uh_ptr->u2ToWrite = (kal_uint16)(bufLength - writenLength);
        return;
    }

    /* finish writing */
    vPortFree(cmux_uh_ptr->writingBuffer);
    cmux_uh_ptr->writingBuffer = NULL;

    switch ( cmux_uh_ptr->state ) {
        case CMUXUH_ACTIVE:
            cmux_handleReadyToSendInd();
            break;

        case CMUXUH_DETACHING:
            cmux_uh_ptr->state = CMUXUH_INACTIVE;
            cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_CNF, NULL);
            break;

        default:
            break;
    }
}

void cmux_uh_handleStartupReq(local_para_struct *local_ptr)
{
    cmux_startup_req_struct *startUp = (cmux_startup_req_struct *) local_ptr;

    // Context Initialization
    cmux_uh_ptr->port = startUp->port;

    cmux_uh_ptr->fUartReadContinue = KAL_FALSE;
    cmux_uh_ptr->writingBuffer = NULL;

    // Start to read
    cmux_uh_ptr->state = CMUXUH_ACTIVE;

    cmux_send_message( CMUX_EVENT_START_UP, NULL );
}

void cmux_uh_handleCmuxUartCloseDownReq(void)
{
    cmux_uh_ptr->state = CMUXUH_DETACHING;

    if (cmux_uh_ptr->writingBuffer == NULL) {
        cmux_uh_ptr->state = CMUXUH_INACTIVE;
        cmux_send_message(MSG_ID_CMUX_UART_CLOSE_DOWN_CNF, NULL);
    }
}

void cmux_uh_handleCmuxReadyToSendInd(void)
{
    if ( cmux_uh_ptr->state != CMUXUH_INACTIVE ) {
        LOGI("fUartReadContinue = %d", cmux_uh_ptr->fUartReadContinue);
        if ( cmux_uh_ptr->fUartReadContinue == KAL_TRUE ) {
            cmux_uh_readUart();
        }
    }
}

void cmux_uh_handleCmuxDataReq(kal_uint8 *pPdu, kal_uint16 u2ToWrite)
{
    // WritingBuffer should be NULL
    CMUX_ASSERT(cmux_uh_ptr->writingBuffer == NULL);

    cmux_uh_ptr->writingBuffer = pPdu;
    cmux_uh_ptr->u2Written = 0;
    cmux_uh_ptr->u2ToWrite = u2ToWrite;

    cmux_uh_writeUart();

}

void cmux_uh_handleUartReadyToReadInd(void)
{
    cmux_uh_ptr->fUartReadContinue = KAL_FALSE;

    if ( cmux_uh_ptr->state != CMUXUH_INACTIVE ) {
        cmux_uh_readUart();
    } else {
        cmux_check_and_handle_startup_req();
    }
}

void cmux_uh_handleUartReadyToWriteInd(void)
{
    if ( cmux_uh_ptr->writingBuffer != NULL ) {
        cmux_uh_writeUart();
    }
}

void cmux_uh_init(void)
{
    CMUX_INIT_CONTEXT(cmux_uh_ptr);
}

