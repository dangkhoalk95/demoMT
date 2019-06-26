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
#include <stdbool.h>

/* hal includes */
#include "memory_attribute.h"
#include "mt7686.h"
#include "hal.h"
#include "hal_log.h"
#include "sdio_reg.h"
#include "sdio_hal.h"

/*hal pinmux define*/
/* Private typedef -----------------------------------------------------------*/
typedef struct {
    uint32_t  length: 16;
    uint32_t  reserved: 13;
    uint32_t  tx_type: 3;
} tx_header;

typedef struct {
    tx_header header;
    uint32_t *data;
} sdio_tx_data_t;  // 4byte tx header

/* Private define ------------------------------------------------------------*/
#define TEST_BLOCK_NUMBER  (4)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned int slt_msdc_tx_buf[128 * TEST_BLOCK_NUMBER * 2];  /*512byte * SLT_TEST_BLOCK_NUMBER = 64K bytes*/
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned int slt_msdc_rx_buf[128 * TEST_BLOCK_NUMBER * 2];  /*512byte * SLT_TEST_BLOCK_NUMBER = 64K bytes*/

/* Private functions ---------------------------------------------------------*/

extern volatile bool sdio_interrupt_flg;
extern bool sdio_hif_get_driver_own(void);
extern bool sdio_hif_enable_interrupt(void);
extern bool h2d_send_mailbox(uint32_t cmd);
extern bool h2d_receive_mailbox(uint32_t *cmd);
extern bool sdio_send_pkt(unsigned char tx_que, uint8_t *tx_buf, int data_length);
extern bool sdio_receive_pkt(uint8_t *rx_buf);


int sdio_host_test(void)
{
    int i = 0;
    hal_sdio_status_t status;
    tx_header *tx_buffer_header;
    uint32_t *tx_buffer;
    uint32_t whisr_reg;

    //io config
    hal_pinmux_set_function(HAL_GPIO_11, 4);
    hal_pinmux_set_function(HAL_GPIO_12, 4);
    hal_pinmux_set_function(HAL_GPIO_13, 4);
    hal_pinmux_set_function(HAL_GPIO_14, 4);
    hal_pinmux_set_function(HAL_GPIO_15, 4);
    hal_pinmux_set_function(HAL_GPIO_16, 4);

    status = sdio_init();
    if (HAL_SDIO_STATUS_OK != status) {
        return -1;
    }

    if (false == sdio_hif_get_driver_own()) {
        printf("get driver own fail. \r\n");
        return -2;
    } else {
        printf("get driver own success. \r\n");
    }

    /*enable sdio slave interrupt*/
    sdio_hif_enable_interrupt();

    while(!sdio_interrupt_flg);
    sdio_interrupt_flg = false;

    printf("sdio receive data start....\r\n");

    /*read whisr*/
    sdio_func1_rd(SDIO_IP_WHISR, &whisr_reg, 4);

    if (whisr_reg & 0x00000002) {
       /*rx done*/
        memset(&slt_msdc_rx_buf, 0, sizeof(slt_msdc_rx_buf));
        if (false == sdio_receive_pkt((uint8_t *)&slt_msdc_rx_buf)) {
            printf("sdio host receive error!\r\n");
            return -3;
        }

        for (i = 0; i < (128 * 4); i++) {
            if (slt_msdc_rx_buf[i] != (0x5a5a5a5a + i)) {
                printf("receive data error, data = %x, i = %d\r\n", slt_msdc_rx_buf[i], i);
                return -4;
            }
        }
       printf("sdio host receive ok.\r\n");       
    }

    while(!sdio_interrupt_flg);
    sdio_interrupt_flg = false;
    
    printf("sdio send data start....\r\n");

    /*read whisr*/
    sdio_func1_rd(SDIO_IP_WHISR, &whisr_reg, 4);
    
    if (whisr_reg & 0x00000001) {
        /*tx done*/
        memset(&slt_msdc_tx_buf, 0, sizeof(slt_msdc_tx_buf));
        tx_buffer_header = (tx_header *)slt_msdc_tx_buf;
        tx_buffer = (uint32_t *)slt_msdc_tx_buf;
        tx_buffer_header->length = (512 * TEST_BLOCK_NUMBER) + sizeof(tx_header);
        tx_buffer += (sizeof(tx_header) / 4);
        for (i = 0; i < (128 * TEST_BLOCK_NUMBER); i++) {
            *tx_buffer++ = (0xaa55aa55 + i);
        }
        if(false == sdio_send_pkt(0, (uint8_t *)slt_msdc_tx_buf, (512 * TEST_BLOCK_NUMBER) + sizeof(tx_header))) {
            printf("sdio host send error!\r\n");
            return -5;
        }

        printf("sdio host send ok.\r\n");
    }

    return 0;
}

