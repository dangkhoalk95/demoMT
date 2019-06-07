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

#include "hal.h"
#include "mtk_hw_adapter.h"
#include "assert.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "wifi_custom_config.h"
#include "hal_eint_internal.h"

/*-<<<<<<<<<<<<<<<<<<<<<<< SDIO related <<<<<<<<<<<<<<<<<<<<<<<<<<*/
#define WIFI_BLK_SIZE (512)

extern const unsigned char BSP_WIFI_EINT;
extern const char BSP_WIFI_32K_PIN;
extern const char BSP_WIFI_26M_PIN;
extern const char BSP_WIFI_RESET_PIN;
extern const char BSP_WIFI_ENABLE_PIN;



hal_sdio_port_t sdio_port;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t value[4];

void wifi_sdio_open(void)
{
    return;
}

void wifi_sdio_close(void)
{
    /*do nothing.*/
    return;
}

/*******************************************************************************
* FUNCTION
*   glSdioInit
*
* DESCRIPTION
*   initialize SDIO bus operation and set block size
*
* CALLS
*   None
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*   None
*******************************************************************************/
bool wifi_sdio_init(void)
{
    hal_sdio_status_t status = 0;
    hal_sdio_config_t sdio_config;

    sdio_port = wifi_get_msdc_port();

    wifi_get_sdio_config(&sdio_config);

    /* Excute SDIO SW hook flow */
    status = hal_sdio_init(sdio_port, &sdio_config);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio init error. \r\n");
        return false;
    }

    status = hal_sdio_set_block_size(sdio_port, HAL_SDIO_FUNCTION_1, WIFI_BLK_SIZE);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio set block size error. \r\n");
        return false;
    }
    return true;
}


/*----------------------------------------------------------------------------*/
/*
* \brief Read a 32-bit device register
*
* \param[in] register_offset Register offset
* \param[in] register_value Pointer to variable used to store read value
*
* \retval TRUE          operation success
* \retval FALSE         operation fail
*/
/*----------------------------------------------------------------------------*/
bool wifi_read_device_register(uint32_t register_offset, uint32_t *register_value)
{
    hal_sdio_command53_config_t cmd_cfg;
    hal_sdio_status_t status;

    cmd_cfg.direction = HAL_SDIO_DIRECTION_READ;
    cmd_cfg.function = HAL_SDIO_FUNCTION_1;
    cmd_cfg.block = false;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.count = 4;
    cmd_cfg.address = register_offset;
    cmd_cfg.buffer = (uint32_t)value;

    status = hal_sdio_execute_command53(sdio_port, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio read error. \r\n");
        return false;
    } else {
        *register_value = ((uint32_t)value[3] << 24) + ((uint32_t)value[2] << 16) + ((uint32_t)value[1] << 8) + value[0];
        return true;
    }
}


/*----------------------------------------------------------------------------*/
/*
* \brief Write a 32-bit device register
*
* \param[in] register Register offset
* \param[in] register_value Value to be written
*
* \retval TRUE          operation success
* \retval FALSE         operation fail
*/
/*----------------------------------------------------------------------------*/
bool wifi_write_device_register(uint32_t register_offset, uint32_t register_value)
{
    hal_sdio_command53_config_t cmd_cfg;
    hal_sdio_status_t status;

    value[0] = (uint8_t)register_value & 0xff;
    value[1] = (uint8_t)((register_value >> 8) & 0xff);
    value[2] = (uint8_t)((register_value >> 16) & 0xff);
    value[3] = (uint8_t)((register_value >> 24) & 0xff);

    cmd_cfg.direction = HAL_SDIO_DIRECTION_WRITE;
    cmd_cfg.function = HAL_SDIO_FUNCTION_1;
    cmd_cfg.block = false;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.count = 4;
    cmd_cfg.address = register_offset;
    cmd_cfg.buffer = (uint32_t)value;

    status = hal_sdio_execute_command53(sdio_port, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio read error. \r\n");
        return false;
    } else {
        return true;
    }
}

/*----------------------------------------------------------------------------*/
/*
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
bool wifi_read_device_port(uint32_t port_offset, uint16_t length, uint32_t *read_buffer)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t  func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;

    /*addr 4-byte align check*/
    assert((((uint32_t)read_buffer) & 0x03) == 0);

    if (((uint32_t)read_buffer) & 0x03) {
        log_hal_error("Error!!!kalDevPortRead:Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_READ;
    cmd_cfg.function = HAL_SDIO_FUNCTION_1;
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
        cmd_cfg.count = ALIGN_32(length);
    }

    status = hal_sdio_execute_command53_dma_blocking(sdio_port, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio read error. \r\n");
        return false;
    } else {
        return true;
    }
}

/*----------------------------------------------------------------------------*/
/*
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
bool wifi_write_device_port(uint32_t port_offset, uint16_t length, uint8_t *write_buffer)
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
        log_hal_error("Error!!!kalDevPortRead:Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_WRITE;
    cmd_cfg.function = HAL_SDIO_FUNCTION_1;
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
        total_length = ALIGN_32(block_number * func1_block_size);
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = ALIGN_32(length);
        total_length = ALIGN_32(length);
    }

    if ((total_length - length_backup) >= sizeof(uint32_t)) {
        /*set unisgned int to Zero for 4-byte unalign address, will shift to align address*/
        uint32_t AligLen = ALIGN_4(length_backup);
        write_buffer[AligLen] = 0;
        write_buffer[AligLen + 1] = 0;
        write_buffer[AligLen + 2] = 0;
        write_buffer[AligLen + 3] = 0;
    }

    status = hal_sdio_execute_command53_dma_blocking(sdio_port, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio write error. \r\n");
        return false;
    } else {
        return true;
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* brief Register wifi eint callcack------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void wifi_register_eint_callback(void *call_back)
{
    hal_eint_config_t config;

    if (0xff == BSP_WIFI_EINT) {
        log_hal_error("wifi eint pin not config. \r\n");
        assert(0);
    }

    config.debounce_time = 2;
    //config.debounce_time = 0;
    config.trigger_mode = HAL_EINT_LEVEL_LOW;

    hal_eint_mask((hal_eint_number_t)BSP_WIFI_EINT);
    hal_eint_init((hal_eint_number_t)BSP_WIFI_EINT, &config);

    hal_eint_register_callback((hal_eint_number_t)BSP_WIFI_EINT, (hal_eint_callback_t)call_back, NULL);

    /*Set new debounce value to 2*(1/32K) = 0.0625ms.*/
    hal_eint_set_debounce_count((hal_eint_number_t)BSP_WIFI_EINT, 2);

    hal_eint_unmask((hal_eint_number_t)BSP_WIFI_EINT);

}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-brief Mask wifi eint-------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void wifi_eint_mask(void)
{
    hal_eint_mask((hal_eint_number_t)BSP_WIFI_EINT);
    return;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* -brief Unmask wifi eint-----------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void wifi_eint_unmask(void)
{
    hal_eint_unmask((hal_eint_number_t)BSP_WIFI_EINT);
    return;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*--brief Config GPIO and clock-----------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void wifi_chip_hw_power_on(void)
{
    if ((0xff == BSP_WIFI_ENABLE_PIN) ||
            (0xff == BSP_WIFI_RESET_PIN)) {
        log_hal_error("wifi pin not config. \r\n");
        assert(0);
    }

    /*BSP_WIFI_32K_PIN may be not configured if external crystal supply 32K.*/
    if (0xff != BSP_WIFI_32K_PIN) {
        hal_pinmux_set_function(BSP_WIFI_32K_PIN, HAL_GPIO_14_CLKO4);// 32K
    }

    /*BSP_WIFI_26M_PIN may not be not configured if external crystal supply 26M.*/
    if (0xff != BSP_WIFI_26M_PIN) {
        hal_pinmux_set_function(BSP_WIFI_26M_PIN, HAL_GPIO_45_SRCLKENAI);// 26M
    }

    hal_gpt_delay_ms(10);

    hal_gpio_set_output(BSP_WIFI_ENABLE_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW

    hal_gpio_set_output(BSP_WIFI_RESET_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW

    hal_gpio_set_output(BSP_WIFI_ENABLE_PIN, HAL_GPIO_DATA_HIGH); //OUTPUT HIGH
    hal_gpt_delay_ms(15);

    hal_gpio_set_output(BSP_WIFI_RESET_PIN, HAL_GPIO_DATA_HIGH); //OUTPUT HIGH
    hal_gpt_delay_ms(10);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-----brief Wifi chip power off----------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void wifi_chip_hw_power_off(void)
{
    wifi_eint_mask();

    hal_gpio_set_output(BSP_WIFI_RESET_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW
    hal_gpio_set_output(BSP_WIFI_ENABLE_PIN, HAL_GPIO_DATA_LOW); //OUTPUT LOW

    if (0xff != BSP_WIFI_32K_PIN) {
        hal_pinmux_set_function(BSP_WIFI_32K_PIN, HAL_GPIO_14_GPIO14);// 32K
    }

    if (0xff != BSP_WIFI_26M_PIN) {
        hal_pinmux_set_function(BSP_WIFI_26M_PIN, HAL_GPIO_45_GPIO45);// 26M
    }

}

