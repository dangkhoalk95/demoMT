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
#include "hif_sdio.h"
#include "assert.h"
#include "hal_log.h"
#include "msdc_custom_config.h"
#include "hal_eint_internal.h"
#include "sdio_reg.h"
#include "wifi_host_os_api.h"

hal_sdio_port_t sdio_port;

volatile uint8_t done_flag = 0;

os_semaphore_t g_sdio_dma_sem = NULL;

/*callbcak*/
void wifi_sdio_callback(hal_sdio_callback_event_t event, void *user_data)
{
    if(event == HAL_SDIO_EVENT_SUCCESS) {
        wifi_os_semphr_give_from_isr(g_sdio_dma_sem);
        wifi_os_switch_context_from_isr();
    }else {
        printf("wifi_sdio_callback:transfer error!\r\n");
    }
}


bool wifi_bus_init(void)
{
    hal_sdio_status_t status = 0;
    hal_sdio_config_t sdio_config;
    uint32_t count = 0;
    uint32_t whcr = 0x10000;

    sdio_port = (hal_sdio_port_t)WIFI_USE_MSDC_PORT_NUMBER;

	sdio_config.bus_width = (hal_sdio_bus_width_t)WIFI_MSDC_BUS_WIDTH;
	sdio_config.clock = (uint32_t)WIFI_MSDC_BUS_CLOCK;

    /* Excute SDIO SW hook flow */
    do {
        status = hal_sdio_init(sdio_port, &sdio_config);
        if (HAL_SDIO_STATUS_OK != status) {
            printf("sdio init error. \r\n");
            //return false;
        }
        hal_gpt_delay_ms(500);
        count++;
    } while ((HAL_SDIO_STATUS_OK != status) && (count < 10));

    g_sdio_dma_sem = wifi_os_semphr_create_binary();

	/*register callback*/
	status = hal_sdio_register_callback(sdio_port,wifi_sdio_callback,NULL);
	if (HAL_SDIO_STATUS_OK != status) {
            printf("sdio init error. \r\n");
            return false;
    }

    if (count >= 10) {
        printf("sdio init timeout!\r\n");
        return false;
    }

    status = hal_sdio_set_block_size(sdio_port, HAL_SDIO_FUNCTION_1, WIFI_BLK_SIZE);
    if (HAL_SDIO_STATUS_OK != status) {
        log_hal_error("sdio set block size error. \r\n");
        return false;
    }    

    

    wifi_write_bus(SDIO_IP_WHCR,4,(uint8_t *)&whcr);
    
    return true;
}

bool wifi_bus_deinit(void)
{
    hal_sdio_deinit(sdio_port);

	return true;
}

bool wifi_read_bus(uint32_t offset, uint16_t length, uint8_t *read_buffer)
{
	hal_sdio_status_t status;
	uint8_t block_number = 0;
	uint32_t  func1_block_size;
	hal_sdio_command53_config_t cmd_cfg;

    if (((uint32_t)read_buffer) & 0x03) {
        printf("Error,wifi read buffer address not 4-byte alignment.\r\n");
        return false;
    }
	
    cmd_cfg.direction = HAL_SDIO_DIRECTION_READ;
    cmd_cfg.function = HAL_SDIO_FUNCTION_1;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = offset;
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

    if (block_number > 0) {
        status = hal_sdio_execute_command53_dma(sdio_port, &cmd_cfg);
        if(status == HAL_SDIO_STATUS_OK) {
            wifi_os_semphr_take(g_sdio_dma_sem, NULL);
        }
    } else {
        status = hal_sdio_execute_command53(sdio_port, &cmd_cfg);
	}
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio read error. \r\n");
        return false;
    } else {
        return true;
    }
}

bool wifi_write_bus(uint32_t offset, uint16_t length, uint8_t *write_buffer)
{
	hal_sdio_status_t status;
	uint8_t block_number = 0;
	uint32_t func1_block_size;
	hal_sdio_command53_config_t cmd_cfg;
	uint32_t total_length;
	uint32_t length_backup;

	if (((uint32_t)write_buffer) & 0x03) {
		printf("Error,wifi write buffer address not 4-byte alignment.\r\n");
		return false;
	}

	cmd_cfg.direction = HAL_SDIO_DIRECTION_WRITE;
	cmd_cfg.function = HAL_SDIO_FUNCTION_1;
	cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
	cmd_cfg.address = offset;
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

	if (block_number > 0) {
		cmd_cfg.block = true;
		cmd_cfg.count = block_number;
		total_length = block_number * func1_block_size;
	} else {
		cmd_cfg.block = false;
		cmd_cfg.count = length;
		total_length = length;
	}

	if ((total_length - length_backup) >= sizeof(uint32_t)) {
		/*set unisgned int to Zero for 4-byte unalign address, will shift to align address*/
		uint32_t aliglen = length_backup;
		write_buffer[aliglen] = 0;
		write_buffer[aliglen + 1] = 0;
		write_buffer[aliglen + 2] = 0;
		write_buffer[aliglen + 3] = 0;
	}

    if (block_number > 0) {
#if 0
        status = hal_sdio_execute_command53_dma_blocking(sdio_port, &cmd_cfg);
#else
   	    status = hal_sdio_execute_command53_dma(sdio_port, &cmd_cfg);
        if(status == HAL_SDIO_STATUS_OK) {
            wifi_os_semphr_take(g_sdio_dma_sem, NULL);
        }
#endif
    } else {
   	    status = hal_sdio_execute_command53(sdio_port, &cmd_cfg);
	}
	if (HAL_SDIO_STATUS_OK != status) {
		printf("sdio write error. \r\n");
		return false;
	} 
	return true;
}

