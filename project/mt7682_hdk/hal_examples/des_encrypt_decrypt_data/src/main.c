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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"

/*hal pinmux define*/
#include "hal_pinmux_define.h"
#include "memory_attribute.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t hardware_id[8] = {
    0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t des_cbc_iv[HAL_DES_CBC_IV_LENGTH] = {
    0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t plain[] = {
    0x00, 0x0b, 0x16, 0x21, 0x2c, 0x37, 0x42, 0x4d, 0x58, 0x63, 0x00, 0x0b, 0x16, 0x21, 0x2c, 0x37,
    0x42, 0x4d, 0x58, 0x63, 0x00, 0x0b, 0x16, 0x21, 0x2c, 0x37, 0x42, 0x4d, 0x58, 0x63
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted_cbc[] = {
    0x82, 0xdc, 0xdf, 0x11, 0xce, 0xbf, 0x97, 0x6a, 0xd9, 0xb2, 0x4f, 0x07, 0x2c, 0x47, 0x6f, 0x16,
    0x52, 0xfe, 0x34, 0xfa, 0x35, 0x8a, 0x2d, 0x4e, 0x41, 0x0e, 0xea, 0x6c, 0x29, 0xb2, 0x88, 0x08
};
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted_ecb[] = {
    0x33, 0x57, 0x2f, 0x59, 0x95, 0xe3, 0x3e, 0xa3, 0x29, 0xde, 0x8f, 0xd2, 0x9e, 0xc9, 0x6b, 0x6b,
    0x53, 0xa8, 0x7e, 0xa9, 0x88, 0x4a, 0x3f, 0x66, 0x06, 0xb9, 0xda, 0xf1, 0x18, 0xfd, 0xa0, 0xc0
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t encrypted[32] = {0};
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t decrypted[32] = {0};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t origin_text = {
    .buffer = plain,
    .length = sizeof(plain)
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t key = {
    .buffer = hardware_id,
    .length = sizeof(hardware_id)
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t encrypted_text = {
    .buffer = encrypted,
    .length = sizeof(encrypted)
};

ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_des_buffer_t decrypted_text = {
    .buffer = decrypted,
    .length = sizeof(decrypted)
};

/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware(void);

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

/**
*@brief Configure and initialize UART hardware initialization for logging.
*@param None.
*@return None.
*/
static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* gpio config for uart0 */
    hal_gpio_init(HAL_GPIO_21);
    hal_gpio_init(HAL_GPIO_22);
    hal_pinmux_set_function(HAL_GPIO_21, HAL_GPIO_21_URXD0);
    hal_pinmux_set_function(HAL_GPIO_22, HAL_GPIO_22_UTXD0);

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
}
/**
*@brief Configure and initialize the systerm clock. In this example, we invoke hal_clock_init to initialize clock driver and clock gates.
*@param None.
*@return None.
*/
static void SystemClock_Config(void)
{
    hal_clock_init();
}

/**
*@brief  Initialize the peripheral driver in this function. In this example, we initialize UART, Flash, and NVIC drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* System HW initialization */

    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();

    /* Board HW initialization */
}

/**
*@brief Log the data in the format of 16 bytes per line.
*@param[in] result: pointer to the data that will be logged out.
*@param[in] length: indicates the length of the data which will be logged out.
*@return None.
*/
static void des_result_dump(uint8_t *result, uint8_t length)
{
    uint8_t i;

    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf(" %02x ", result[i]);
    }
    printf("\r\n");

}
/**
*@brief Example of DES encryption/decryption including CBC and ECB modes.
*@param None.
*@return None.
*/
static void des_encrypt_decrypt_data_example(void)
{
    /*Step1: Prepare the data that will be used in DES encryption/decryption*/
    /* Allocate data in physical memory region, do not use  VRAM or VSYSRAM */

    /*Step2: Log out the data during DES encryption/decryption operation including CBC and ECB modes.*/
    printf("Origin:");
    des_result_dump(origin_text.buffer, origin_text.length);
    printf("Key:");
    des_result_dump(key.buffer, key.length);
    printf("IV:");
    des_result_dump(des_cbc_iv, sizeof(des_cbc_iv));

    hal_des_cbc_encrypt(&encrypted_text,
                        &origin_text,
                        &key,
                        des_cbc_iv);
    printf("Encrypted(DES CBC):");
    //des_result_dump(encrypted_text.buffer, encrypted_text.length);
    if(memcmp(encrypted_text.buffer, encrypted_cbc, encrypted_text.length)) {
        printf("DES CBC encryption fail\r\n");
    }
    else {
        printf("DES CBC encryption success\r\n");
    }

    hal_des_cbc_decrypt(&decrypted_text,
                        &encrypted_text,
                        &key,
                        des_cbc_iv);
    printf("Decrypted(DES CBC):");
    //des_result_dump(decrypted_text.buffer, decrypted_text.length);
    if(memcmp(decrypted_text.buffer, plain, decrypted_text.length)) {
        printf("DES CBC decryption fail\r\n");
    }
    else {
        printf("DES CBC decryption success\r\n");
    }


    hal_des_ecb_encrypt(&encrypted_text,
                        &origin_text,
                        &key);
    printf("Encrypted(DES ECB):");
    //des_result_dump(encrypted_text.buffer, encrypted_text.length);
    if(memcmp(encrypted_text.buffer, encrypted_ecb, encrypted_text.length)) {
        printf("DES ECB encryption fail\r\n");
    }
    else {
        printf("DES ECB encryption success\r\n");
    }

    hal_des_ecb_decrypt(&decrypted_text,
                        &encrypted_text,
                        &key);
    printf("Decrypted(DES ECB):");
    //des_result_dump(decrypted_text.buffer, decrypted_text.length);
    if(memcmp(decrypted_text.buffer, plain, decrypted_text.length)) {
        printf("DES ECB decryption fail\r\n");
    }
    else {
        printf("DES ECB decryption success\r\n");
    }
}

/* main function */
int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    printf("\r\n\r\n");/* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
    printf("\r\n\r\n");

    des_encrypt_decrypt_data_example();
    while (1);
}

