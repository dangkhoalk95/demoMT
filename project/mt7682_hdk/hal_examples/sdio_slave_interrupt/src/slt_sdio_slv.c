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
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "memory_attribute.h"
#include "mt7686.h"
#include "hal.h"
#include "hal_log.h"
#include "hal_sdio_slave.h"

#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED

#define SLT_TEST_BLOCK_NUMBER  (4)
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t slt_sdio_slave_send_buf[128 * SLT_TEST_BLOCK_NUMBER * 2];  /*512byte * SLT_TEST_BLOCK_NUMBER = 64K bytes*/
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t slt_sdio_slave_receive_buf[128 * SLT_TEST_BLOCK_NUMBER * 2];  /*512byte * SLT_TEST_BLOCK_NUMBER = 64K bytes*/


volatile hal_sdio_slave_callback_event_t test_event = HAL_SDIO_SLAVE_EVENT_NONE;
volatile uint32_t sw_int_data = 0;
hal_sdio_slave_callback_tx_length_parameter_t tx_length;

void sdio_slave_test_callback(hal_sdio_slave_callback_event_t event,  void *data, void *user_data)
{
    test_event = event;
}

int sdio_slave_test(void)
{
    uint32_t i = 0;

    hal_pinmux_set_function(HAL_GPIO_11, 5);
    hal_pinmux_set_function(HAL_GPIO_12, 5);
    hal_pinmux_set_function(HAL_GPIO_13, 5);
    hal_pinmux_set_function(HAL_GPIO_14, 5);
    hal_pinmux_set_function(HAL_GPIO_15, 5);
    hal_pinmux_set_function(HAL_GPIO_16, 5);

    /*step1: init SDIO slave.*/
    if (0 > hal_sdio_slave_init(HAL_SDIO_SLAVE_PORT_0)) {
        printf("sdio slave init error. \r\n");
        return -1;
    }else {
        printf("sdio slave init ok. \r\n");
     }

    hal_sdio_slave_register_callback(HAL_SDIO_SLAVE_PORT_0, sdio_slave_test_callback, NULL);

    memset(&slt_sdio_slave_send_buf, 0, (2 * 512 * SLT_TEST_BLOCK_NUMBER));
    for (i = 0; i < (128 * SLT_TEST_BLOCK_NUMBER); i++) {
        slt_sdio_slave_send_buf[i] = 0x5a5a5a5a + i;
    }

    if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_send_dma(HAL_SDIO_SLAVE_PORT_0, HAL_SDIO_SLAVE_RX_QUEUE_0, slt_sdio_slave_send_buf, 512 * SLT_TEST_BLOCK_NUMBER)) {
        printf("sdio slave send error.\r\n");
        return -2;
    }

    while(test_event == HAL_SDIO_SLAVE_EVENT_NONE);
    test_event = HAL_SDIO_SLAVE_EVENT_NONE;

    printf("sdio slave send ok.\r\n");

    memset(&slt_sdio_slave_receive_buf, 0, (2 * 512 * SLT_TEST_BLOCK_NUMBER));
    if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_receive_dma(HAL_SDIO_SLAVE_PORT_0, HAL_SDIO_SLAVE_TX_QUEUE_1, slt_sdio_slave_receive_buf, 512 * SLT_TEST_BLOCK_NUMBER)) {
        printf("sdio slave receive error.\r\n");
        return -3;
    }

    while(test_event == HAL_SDIO_SLAVE_EVENT_NONE);
    test_event = HAL_SDIO_SLAVE_EVENT_NONE;
    printf("sdio slave receive ok.\r\n");
    
    return 0;
}

#endif

