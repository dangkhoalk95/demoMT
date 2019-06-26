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

/* hal includes */
#include "hal_sdio.h"
#include "hal_msdc.h"
#include "sdio_hal.h"
#include "memory_attribute.h"
#include "hal_log.h"
#include "assert.h"
#include "hal.h"
#include "sdio_reg.h"

/*hal pinmux define*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define WIFI_BLK_SIZE (256)
#define WIFI_SDIO_PORT HAL_SDIO_PORT_0

/* Private variables ---------------------------------------------------------*/
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t value[4];

/* Private functions ---------------------------------------------------------*/
extern hal_sdio_status_t sdio_interrupt_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback,void *user_data);

volatile bool sdio_interrupt_flg = 0;

void sdio_interrupt_callback(hal_sdio_callback_event_t event, void *user_data)
{
    if (HAL_SDIO_EVENT_SUCCESS == event) {
        sdio_interrupt_flg = true;    
    }
}

hal_sdio_status_t sdio_init(void)
{
    hal_sdio_status_t status = HAL_SDIO_STATUS_OK;
    hal_sdio_config_t sdio_config = {HAL_SDIO_BUS_WIDTH_1,26000};
    uint32_t blk_size;
    uint32_t count = 0;

    /* Excute SDIO SW hook flow */
    do {
        status = hal_sdio_init(WIFI_SDIO_PORT, &sdio_config);
        printf("hal_sdio_init status = %d \r\n", status);
        if (HAL_SDIO_STATUS_OK != status) {
            printf("sdio init error. \r\n");
        }
        hal_gpt_delay_ms(500);
        count++;
    } while ((HAL_SDIO_STATUS_OK != status) && (count < 10));

    if (count >= 10) {
        printf("sdio init timeout! status = %d\r\n", status);
        return HAL_SDIO_STATUS_ERROR;
    }

    /*register sdio interrupt callback*/
    status = sdio_interrupt_register_callback(WIFI_SDIO_PORT,sdio_interrupt_callback,NULL);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio register callback error. \r\n");
        return status;
    }

    status = hal_sdio_set_block_size(WIFI_SDIO_PORT, HAL_SDIO_FUNCTION_1, WIFI_BLK_SIZE);
    printf("hal_sdio_set_block_size status = %d \r\n", status);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio set block size error. \r\n");
        return status;
    }

    status = hal_sdio_get_block_size(WIFI_SDIO_PORT, HAL_SDIO_FUNCTION_1, &blk_size);
    printf("hal_sdio_get_block_size status = %d \r\n", status);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio get block size error. \r\n");
        return status;
    } else {
        printf("sdio get block size, block size is %d. \r\n", (int)blk_size);
    }

    return HAL_SDIO_STATUS_OK;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Read device I/O port
*
* \param[in] port_offset             I/O port offset
* \param[in] length              Length to be read
* \param[out] read_buffer            Pointer to read buffer
*
* \retval TRUE          operation success
* \retval FALSE         operation fail
*/
/*----------------------------------------------------------------------------*/
static bool sdio_read_device_port(uint32_t port_offset, uint16_t length, uint32_t *read_buffer, hal_sdio_function_id_t func)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t  func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;

    /*addr 4-byte align check*/
    assert((((uint32_t)read_buffer) & 0x03) == 0);

    if (((uint32_t)read_buffer) & 0x03) {
        printf("Error!!!kalDevPortRead:Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_READ;
    cmd_cfg.function = func;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = port_offset;
    cmd_cfg.buffer = (uint32_t)read_buffer;

    func1_block_size = WIFI_BLK_SIZE;
    while (length >= func1_block_size) {
        length -= func1_block_size;
        block_number++;
    }
    if (length > 0 && block_number > 0) {
        block_number++;
    }

    if (block_number > 0) { /* block mode*/
        cmd_cfg.block = true;
        cmd_cfg.count = block_number;
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = length;//ALIGN_32(length);
    }

    status = hal_sdio_execute_command53(WIFI_SDIO_PORT, &cmd_cfg);

    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio read error. %d\r\n",status);
        return false;
    } else {
        return true;
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Write device I/O port
*
* \param[in] port_offset             I/O port offset
* \param[in] length              Length to be write
* \param[in] write_buffer             Pointer to write buffer
*
* \retval TRUE          operation success
* \retval FALSE         operation fail
*/
/*----------------------------------------------------------------------------*/
static bool sdio_write_device_port(uint32_t  port_offset, uint16_t length, uint8_t *write_buffer, hal_sdio_function_id_t func)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;
    uint32_t total_length;
    uint32_t length_backup;

    /*addr 4-byte align check*/
    assert((((uint32_t)write_buffer) & 0x03) == 0);

    if (((uint32_t)write_buffer) & 0x03) {
        printf("Error!!!Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_WRITE;
    cmd_cfg.function = func;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = port_offset;
    cmd_cfg.buffer = (uint32_t)write_buffer;

    length_backup = length;

    func1_block_size = WIFI_BLK_SIZE;
    while (length >= func1_block_size) {
        length -= func1_block_size;
        block_number++;
    }
    if (length > 0 && block_number > 0) {
        block_number++;
    }

    if (block_number > 0) { // block mode
        cmd_cfg.block = true;
        cmd_cfg.count = block_number;
        total_length = block_number * func1_block_size;//ALIGN_32(block_number * func1_block_size);
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = length;// ALIGN_32(length);
        total_length = length; //ALIGN_32(length);
    }

    if ((total_length - length_backup) >= sizeof(uint32_t)) {
        /*set unisgned int to Zero for 4-byte unalign address, will shift to align address*/
        uint32_t aliglen = length_backup; //ALIGN_4(length_backup);
        write_buffer[aliglen] = 0;
        write_buffer[aliglen + 1] = 0;
        write_buffer[aliglen + 2] = 0;
        write_buffer[aliglen + 3] = 0;
    }

    status = hal_sdio_execute_command53(WIFI_SDIO_PORT, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio write error. \r\n");
        return false;
    } else {
        return true;
    }
}

int sdio_func1_wr(unsigned int u4Register, void *pBuffer,  unsigned int Length)
{
    int ret, i;

    assert((pBuffer != NULL));

    ret = sdio_write_device_port(u4Register, Length, pBuffer, HAL_SDIO_FUNCTION_1);
    if (!ret) {
        printf("[ERR] function 1 write fail : addr : 0x%x , size : %d, err_ret: 0x%x\r\n", u4Register, Length, ret);
        printf("Error packet content = ") ;

        for (i = 0; i < Length; i++) {
            //printf("%x ", *(unsigned char *)(pBuffer + i));
        }

        printf(" \r\n ");
    }

    return (ret) ? 0 : 1 ;
}

int sdio_func1_rd(unsigned int u4Register, void *pBuffer,  unsigned int Length)
{
    int ret ;

    assert((pBuffer != NULL)) ;

    ret = sdio_read_device_port(u4Register, Length, pBuffer, HAL_SDIO_FUNCTION_1);
    if (!ret) {
        printf("[ERR] function 1 read fail : addr : 0x%x , size : %d, err_ret: 0x%x\r\n", u4Register, Length, ret) ;
    }

    return (ret) ? 0 : 1 ;
}


