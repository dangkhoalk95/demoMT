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
 
#ifndef __WFC_SDIO_H__
#define __WFC_SDIO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_sdio.h"
#include "wfc_stub.h"

hal_sdio_status_t wfc_if_sdio_init(void);
void wfc_sdio_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize);
void wfc_sdio_send_data(uint8_t *txptr, uint32_t txsize);

void wfc_sido_htx_cp(T_WFC_QBUF *qbuf, uint8_t *pbuf, uint32_t offset);
void wfc_sdio_hrx_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize);
void wfc_sdio_hevt_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize);

int wfc_sdio_set_cmd_buf(void);
int wfc_sdio_set_htx_buf(void);

void wfc_sdio_free_cmd_buf(uint32_t qidx);
uint8_t *wfc_sdio_get_cmd_buf(uint32_t qidx);

void wfc_sdio_mq_reset(uint32_t ctrl);

void wfc_mq_push_hrx(T_WFC_QBUF *qbuf);
T_WFC_QBUF  *wfc_mq_pop_hrx(void);

int32_t wfc_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx);
void wfc_mq_clear(T_WFC_QBUF *qbuf);
void wfc_mq_free(T_WFC_QBUF *qbuf);
uint8_t wfc_mq_opcmd_process(T_WFC_QBUF *qbuf, uint8_t *isDTW);

void wfc_dump_buf(uint8_t *pbuf, uint32_t bufSize);

#ifdef __cplusplus
}
#endif


#endif  /* __WFC_SDIO_H__ */
