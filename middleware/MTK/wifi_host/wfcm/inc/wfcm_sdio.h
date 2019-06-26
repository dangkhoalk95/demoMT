/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#ifndef __WFCM_SDIO_H__
#define __WFCM_SDIO_H__

#include <stdint.h>
#include "wfcm_stub.h"

#ifdef __cplusplus
extern "C" {
#endif

int wfcm_if_sdio_init(void);
void wfcm_if_sdio_deinit(void);

void wfcm_mq_free(T_WFC_QBUF *qbuf);
int32_t wfcm_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx);

void wfcm_mem_align_clr(uint8_t *pbuf, uint32_t bufSize);
void wfcm_sdio_cmd_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *param, uint32_t paramSize, uint8_t forISR);
void wfcm_memq_get_qbuf(T_WFC_QBUF *qbuf);
bool wfcm_sdio_send_data(T_WFC_QBUF *qbuf, uint8_t qbuf_release);
bool wfcm_sdio_receive_cmdrsp(uint8_t *rx_buf, uint32_t *bufSize);
bool wfcm_sdio_receive_data(uint8_t *pbuf, uint32_t bufSize);
void wfcm_sdio_htx_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *htxptr, uint32_t htxSize);
void wfcm_dump_buf(uint8_t *pbuf, uint32_t bufSize);
bool wfcm_sdio_receive_data_size(uint32_t *bufSize);
bool wfcm_sdio_receive_data_payload(uint8_t *pbuf, uint32_t bufSize);


#ifdef __cplusplus
}
#endif


#endif  /* __WFCM_SDIO_H__ */
