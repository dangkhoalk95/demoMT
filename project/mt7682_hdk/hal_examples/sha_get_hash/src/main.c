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
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t *data = (uint8_t *)"abcdefghijklmnopqrstwxyz1234567";
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t digest[64] = {0};
uint8_t sha160_digest[] = {
    0x7f, 0xbc, 0xb9, 0xf3, 0xe7, 0xcc, 0xc6, 0x23, 0x07, 0xfc, 0x8f, 0xd6, 0x44, 0xde, 0xd0, 0x64,
    0x60, 0x89, 0xa8, 0x1c

};
uint8_t sha224_digest[] = {
    0xa4, 0x7c, 0xae, 0x07, 0x62, 0x2e, 0xf2, 0x09, 0xd7, 0x0e, 0x89, 0xc4, 0x90, 0x59, 0x60, 0x36,
    0x55, 0xd2, 0xc2, 0x2e, 0xb7, 0xe4, 0xc2, 0x9a, 0x80, 0x68, 0xc9, 0xe3
};
uint8_t sha256_digest[] = {
    0x6e, 0x12, 0xb7, 0x12, 0x6b, 0xf0, 0x89, 0x0d, 0x9f, 0x79, 0xb9, 0x1f, 0x35, 0xce, 0x3d, 0xec,
    0x32, 0xf0, 0xf3, 0x53, 0x42, 0xba, 0x80, 0xb2, 0xb0, 0x76, 0x06, 0x09, 0xf3, 0x61, 0x13, 0xe0
};
uint8_t sha384_digest[] = {
    0x80, 0x15, 0xb0, 0x26, 0xfa, 0x19, 0xa6, 0x62, 0x4d, 0x49, 0x83, 0x16, 0x31, 0x84, 0x72, 0x65,
    0xda, 0x9d, 0xb2, 0x3e, 0x75, 0x8e, 0x65, 0x34, 0x07, 0x62, 0x0e, 0x9d, 0xf4, 0xd2, 0x98, 0x5f,
    0x5f, 0xee, 0x64, 0x2a, 0x27, 0x09, 0x7a, 0x6a, 0xf3, 0x71, 0xcf, 0x78, 0x8e, 0xa9, 0xa9, 0x7b
};
uint8_t sha512_digest[] = {
    0x58, 0xe6, 0x16, 0x0a, 0xcf, 0x5e, 0xbd, 0xe7, 0xce, 0x4b, 0x1c, 0x84, 0x13, 0x85, 0x9c, 0x6b,
    0xb1, 0x19, 0x1a, 0x1b, 0xba, 0x0e, 0x2a, 0xf9, 0x4e, 0x51, 0x7f, 0xf9, 0x16, 0x1d, 0x15, 0x7b,
    0x34, 0x2e, 0x22, 0x07, 0x50, 0xda, 0x4d, 0x0d, 0xd8, 0x8e, 0x29, 0xea, 0x51, 0x87, 0x1f, 0x05,
    0x11, 0xf1, 0x41, 0x65, 0x3b, 0xd4, 0x3b, 0x56, 0xdf, 0x59, 0xca, 0x6a, 0xe0, 0x25, 0xed, 0x2a
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
static void sha_result_dump(uint8_t *result, uint8_t length)
{
#ifdef HAL_SHA_DBG
    uint8_t i;

    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf(" %02x ", result[i]);
    }
    printf("\r\n");
#else
    // dump nothing 
#endif

}
/**
*@brief Example of SHA1/224/256/384/512 hash calculation.
*@param None.
*@return None.
*/
static void sha_get_hash_example(void)
{
    uint32_t size = (uint32_t)strlen((char *)data);

    printf("SHA1/SHA256 data: %s\r\n", (char *)data);

    hal_sha1_context_t sha1_context;
    hal_sha1_init(&sha1_context);
    hal_sha1_append(&sha1_context, data, size);
    hal_sha1_end(&sha1_context, digest);
    printf("SHA1 result:");
    sha_result_dump(digest, HAL_SHA1_DIGEST_SIZE);
    if(memcmp(digest, sha160_digest, HAL_SHA1_DIGEST_SIZE)){
        printf("SHA1 test fail\r\n");
    }else {
        printf("SHA1 test success\r\n");
    }

    hal_sha224_context_t sha224_context;
    hal_sha224_init(&sha224_context);
    hal_sha224_append(&sha224_context, data, size);
    hal_sha224_end(&sha224_context, digest);
    printf("SHA224 result:");
    sha_result_dump(digest, HAL_SHA224_DIGEST_SIZE);
    if(memcmp(digest, sha224_digest, HAL_SHA224_DIGEST_SIZE)){
        printf("SHA224 test fail\r\n");
    }else {
        printf("SHA224 test success\r\n");
    }

    hal_sha256_context_t sha256_context;
    hal_sha256_init(&sha256_context);
    hal_sha256_append(&sha256_context, data, size);
    hal_sha256_end(&sha256_context, digest);
    printf("SHA256 result:");
    sha_result_dump(digest, HAL_SHA256_DIGEST_SIZE);
    if(memcmp(digest, sha256_digest, HAL_SHA256_DIGEST_SIZE)){
        printf("SHA256 test fail\r\n");
    }else {
        printf("SHA256 test success\r\n");
    }

    hal_sha384_context_t sha384_context;
    hal_sha384_init(&sha384_context);
    hal_sha384_append(&sha384_context, data, size);
    hal_sha384_end(&sha384_context, digest);
    printf("SHA384 result:");
    sha_result_dump(digest, HAL_SHA384_DIGEST_SIZE);
    if(memcmp(digest, sha384_digest, HAL_SHA384_DIGEST_SIZE)){
        printf("SHA384 test fail\r\n");
    }else {
        printf("SHA384 test success\r\n");
    }

    hal_sha512_context_t sha512_context;
    hal_sha512_init(&sha512_context);
    hal_sha512_append(&sha512_context, data, size);
    hal_sha512_end(&sha512_context, digest);
    printf("SHA512 result:");
    sha_result_dump(digest, HAL_SHA512_DIGEST_SIZE);
    if(memcmp(digest, sha512_digest, HAL_SHA512_DIGEST_SIZE)){
        printf("SHA512 test fail\r\n");
    }else {
        printf("SHA512 test success\r\n");
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

    sha_get_hash_example();
    while (1);
}

