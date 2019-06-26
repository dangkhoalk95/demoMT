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
#include "hal_gpt.h"

/*hal pinmux define*/
#include "hal_pinmux_define.h"
#include "memory_attribute.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2S_TX_BUFFER_LENGTH   32
#define I2S_RX_BUFFER_LENGTH   2048
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint32_t I2S_TxBuf[I2S_TX_BUFFER_LENGTH];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint32_t I2S_RxBuf[I2S_RX_BUFFER_LENGTH];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint32_t USER_Rx_Buf[I2S_TX_BUFFER_LENGTH];

static uint16_t audio_Tone2k_16kSR[32] = {
    0xffff, 0xcd1a, 0xb805, 0xcd1b, 0x0000, 0x32e6, 0x47fb, 0x32e5,
    0x0000, 0xcd1a, 0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5,
    0x0000, 0xcd1b, 0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5,
    0x0000, 0xcd1b, 0xb806, 0xcd1a, 0xffff, 0x32e5, 0x47f9, 0x32e6
};

/* Private functions ---------------------------------------------------------*/

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
*@brief  Initialize the periperal driver in this function. In this example, we initialize UART, Flash, and NVIC drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();

}


/* Example of i2s_configure */
static int8_t i2s_configure(void)
{
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;

    /* Set I2S as internal loopback mode */
    result = hal_i2s_init(HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_init failed---\r\n");
        return -1;
    }

    /* Configure I2S  */
    i2s_config.clock_mode = HAL_I2S_MASTER;
    i2s_config.sample_width = HAL_I2S_SAMPLE_WIDTH_16BIT;
    i2s_config.frame_sync_width = HAL_I2S_FRAME_SYNC_WIDTH_32;
    i2s_config.tx_mode = HAL_I2S_TX_DUPLICATE_DISABLE;

    i2s_config.i2s_in.channel_number = HAL_I2S_MONO;
    i2s_config.i2s_out.channel_number = HAL_I2S_MONO;

    i2s_config.i2s_in.sample_rate = HAL_I2S_SAMPLE_RATE_192K;
    i2s_config.i2s_out.sample_rate = HAL_I2S_SAMPLE_RATE_192K;
    i2s_config.i2s_in.msb_offset = 0;
    i2s_config.i2s_out.msb_offset = 0;
    i2s_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    i2s_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;

    result = hal_i2s_set_config(&i2s_config);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_set_config failed---\r\n");
        return -1;
    }

    result = hal_i2s_setup_tx_vfifo(I2S_TxBuf, I2S_TX_BUFFER_LENGTH / 2, I2S_TX_BUFFER_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_setup_tx_vfifo failed---\r\n");
        return -1;
    }

    result = hal_i2s_setup_rx_vfifo(I2S_RxBuf, I2S_RX_BUFFER_LENGTH / 2, I2S_RX_BUFFER_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_setup_rx_vfifo failed---\r\n");
        return -1;
    }

    return 1;
}


/* Example of i2s_open */
static void i2s_open(void)
{
    hal_i2s_enable_tx();
    hal_i2s_enable_rx();
    hal_i2s_enable_audio_top();
}


/* Example of i2s_write
*  hal_i2s_status_t hal_i2s_tx_write(uint32_t data);
*  The data format for I2S TX is as shown below:
*	          Byte 3      Byte 2    Byte 1    Byte 0
*  Stereo    R[15:8]    R[7:0]    L[15:8]    L[7:0]
*  Mono      8'b0         8'b0       L[15:8]    L[7:0]
*/
static void i2s_write(void)
{
    uint32_t write_sample_count = 0;
    uint32_t i = 0;
    //GPT
    //uint32_t count1, count2, duration_count;
    //uint32_t time;
    hal_i2s_get_tx_sample_count(&write_sample_count);
    printf("i2s_write write_sample_count=%d\r\n", (unsigned int)write_sample_count);
    if (write_sample_count >= I2S_TX_BUFFER_LENGTH) {
        for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
            //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count1);
            hal_i2s_tx_write(audio_Tone2k_16kSR[i]);
            //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count2);
            //hal_gpt_get_duration_count(count1, count2, &duration_count);
            //printf("write time=%d us\r\n",duration_count);
        }
    }

}


/* Example of i2s_read */
static void i2s_read(void)
{
    uint32_t read_sample_count = 0;
    uint32_t rx_data_temp = 0;
    uint32_t i = 0;

    while (1) {
        hal_i2s_get_rx_sample_count(&read_sample_count);
        if (read_sample_count > 1024) {
            break;
        }
    }

    hal_i2s_get_rx_sample_count(&read_sample_count);
    while (read_sample_count != 0) {
        hal_i2s_rx_read(&rx_data_temp);
        if (rx_data_temp != 0) {
            USER_Rx_Buf[i] = rx_data_temp;
            i++;
            break;
        }
        hal_i2s_get_rx_sample_count(&read_sample_count);
    }

    hal_i2s_get_rx_sample_count(&read_sample_count);
    while (read_sample_count != 0) {
        hal_i2s_rx_read(&rx_data_temp);
        if (i < (I2S_TX_BUFFER_LENGTH)) {
            USER_Rx_Buf[i] = rx_data_temp;
            i++;
        } else {
            break;
        }
        hal_i2s_get_rx_sample_count(&read_sample_count);
    }

}


/* Example of i2s_close */
static void i2s_close(void)
{

    hal_i2s_disable_tx();
    hal_i2s_disable_rx();
    hal_i2s_disable_audio_top();
    hal_i2s_deinit();
    hal_i2s_stop_tx_vfifo();
    hal_i2s_stop_rx_vfifo();
}


static int8_t i2s_compare(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
        if(USER_Rx_Buf[i] != I2S_TxBuf[i])
            return -1;           
    }
    return 0;
}

static void i2s_dump(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
        printf("I2S_TxBuf[%u]=0x%08x\r\n", (int)i, (unsigned int)I2S_TxBuf[i]);
    }

    //for(i=0;i<I2S_RX_BUFFER_LENGTH;i++)
    //    printf("I2S_RxBuf[%d]=%08x\r\n", i, I2S_RxBuf[i]);

    for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
        printf("USER_Rx_Buf[%u]=0x%08x\r\n", (int)i, (unsigned int)USER_Rx_Buf[i]);
    }

}


void i2s_write_data_example(void)
{
    int8_t result = 0;

    printf("\r\n ---i2s_example begin---\r\n");

    result = i2s_configure();
    if (result == -1) {
        printf("\r\n ---i2s:    i2s_configure failed---\r\n");
    }

    i2s_open();
    i2s_write();
    i2s_read();
    //hal_gpt_delay_ms(100);
    i2s_close();
    if(i2s_compare()!=0)
        i2s_dump();
    else{
        /* This is for hal_examples auto-test */
        printf("example project test success.\n");
    }
        
    printf("\r\n ---i2s_example finished!!!---\r\n");
}

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
    printf("\r\n\r\n");
    /* The output UART used by printf is set by plain_log_uart_init() */
    printf("welcome to main()\r\n");
    printf("pls add your own code from here\r\n");
    printf("\r\n\r\n");

    i2s_write_data_example();

    while (1);
}

