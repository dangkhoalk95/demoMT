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
 
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory_attribute.h"

/* hal includes */
#include "hal.h"
#include "hal_msdc.h"
#include "sdio_hal.h"
#include "sdio_reg.h"

/*hal pinmux define*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SDIO_SWINT_MB0_BROMSEND   0x00020000
#define SDIO_SWINT_MB0_BROMRECV   0x00010000

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

bool sdio_hif_get_driver_own(void)
{
    uint32_t value ;
    uint32_t cnt = 50;

    printf("[sdio_hif_get_driver_own]<==========>\r\n") ;

    //Set driver own
    value = W_FW_OWN_REQ_CLR ;
    if (sdio_func1_wr(SDIO_IP_WHLPCR, &value, 4) != 0) {
        return false;
    }

    hal_gpt_delay_ms(10);

    while (cnt--) {
        if (sdio_func1_rd(SDIO_IP_WHLPCR, &value, 4) != 0) {
            return false ;
        }

        if (value & W_DRV_OWN_STATUS) {
            return true;
        }
        hal_gpt_delay_ms(50);
    }

    return false;
}

bool sdio_hif_enable_interrupt(void)
{
    uint32_t value ;

    value = W_INT_EN_SET; 

    if (0 != sdio_func1_wr(SDIO_IP_WHLPCR, &value, 4)){
        return false ; 
    }

    value = 0x000000087 ; 

    if (0 != sdio_func1_wr(SDIO_IP_WHIER, &value, 4)){
        return false ; 
    }

    return true;
}

bool h2d_send_mailbox(uint32_t cmd)
{
    uint32_t h2d_mb;

    if (0 != sdio_func1_wr(SDIO_IP_H2DSM0R, &cmd, 4)) {
        return false;
    }

    h2d_mb = SDIO_SWINT_MB0_BROMSEND;
    if (0 != sdio_func1_wr(SDIO_IP_WSICR, &h2d_mb, 4)) {
        return false;
    }

    return true;
}

bool h2d_receive_mailbox(uint32_t *cmd)
{
    uint32_t whisr;
    uint32_t wait_count = 0;

    do {
        wait_count++;
        msdc_wait(1);
        sdio_func1_rd(SDIO_IP_WHISR, &whisr, 4);
    } while ((0 == (whisr & SDIO_SWINT_MB0_BROMRECV)) && (wait_count < 50));

    if (wait_count >= 50) {
        printf("wait receive mailbox timeout!\r\n");
        return false;
    }

    if (0 != sdio_func1_rd(SDIO_IP_D2HRM0R, cmd, 4)) {
        return false;
    }

    whisr = SDIO_SWINT_MB0_BROMRECV;
    sdio_func1_wr(SDIO_IP_WHISR, &whisr, 4);

    return true;
}

bool sdio_send_pkt(unsigned char tx_que, uint8_t *tx_buf, int data_length)
{
    uint32_t wtsr0_reg = 0;

    sdio_func1_rd(SDIO_IP_WTSR0, &wtsr0_reg, 4);

    if (sdio_func1_wr(SDIO_IP_WTDR1, tx_buf, data_length)) {
        return false;
    }

    return true;
}

bool sdio_receive_pkt(uint8_t *rx_buf)
{
    uint32_t total_len = 0;

    sdio_func1_rd(SDIO_IP_WRPLR,&total_len,4);

    if (sdio_func1_rd(SDIO_IP_WRDR0, rx_buf, total_len & 0x0000ffff)) {
        return false;
    }

    return true;
}


