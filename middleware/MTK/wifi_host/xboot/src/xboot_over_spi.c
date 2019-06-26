/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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


#include <stdio.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "memory_attribute.h"
#include "hal_spi_master.h"
#include "hal_gpio_internal.h"
#include "hal_gpio.h"
#include "host_os_utils.h"


extern int wfcm_if_init(void);
extern void spi_master_read_request(uint16_t cmdset, uint8_t *buffer, uint32_t length, uint32_t wait_ms);
extern void spi_master_write_data(uint16_t cmdset, uint8_t *buffer, uint32_t length);

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    BIN_LOAD_START = 0,
    ACK_BIN_LOAD_START = 1,
    GET_BIN = 2,
    ACK_GET_BIN = 3,
    BIN_LOAD_END = 4,
    ACK_BIN_LOAD_END = 5,
    MD_BOOT_END = 6,
    ACK_MD_BOOT_END = 7,
    MD_MSG_OUTPUT = 8,
    ACK_MD_MSG_OUTPUT = 9,
    MD_MSG_FLUSH = 10,
    ACK_MD_MSG_FLUSH = 11,
    XBOOT_CMD_ID_END = 0x0FFFFFFF
} xboot_cmd_id_t;

typedef struct {
    uint32_t m_magic;
    xboot_cmd_id_t m_msg_id;
    uint32_t m_status;
    uint32_t m_reserve[5];
} xboot_cmd_status_t;

typedef struct {
    uint32_t m_magic;
    xboot_cmd_id_t m_msg_id;
    uint32_t m_offset;
    uint32_t m_len;
    uint32_t m_reserve[4];
} xboot_cmd_getbin_t;

typedef struct {
    uint32_t m_magic;
    xboot_cmd_id_t m_msg_id;
    uint32_t m_str_len;
    uint8_t m_str[20];
} xboot_cmd_dbgprint_t;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#if (PRODUCT_VERSION == 2523)

#define SPIM_PIN_NUMBER_CS            HAL_GPIO_25
#define SPIM_PIN_NUMBER_MT5932_RESET  HAL_GPIO_48  /* The GPIO pin to control reset pin of MT5932 */

#elif (PRODUCT_VERSION == 2625)

/* Host relative macros define */

#define SPIM_TEST_FREQUENCY     500000

#define SPIM_TEST_PORT          HAL_SPI_MASTER_1

#define SPIM_PIN_NUMBER_CS      HAL_GPIO_19  // SPI_WIFI_CS // from Peter Pan
#define SPIM_PIN_FUNC_CS        2

#define SPIM_PIN_NUMBER_CLK     HAL_GPIO_22  // SPI_WIFI_SCK
#define SPIM_PIN_FUNC_CLK       2

#define SPIM_PIN_NUMBER_MOSI    HAL_GPIO_21  // SPI_WIFI_MOSI
#define SPIM_PIN_FUNC_MOSI      2

#define SPIM_PIN_NUMBER_MISO    HAL_GPIO_20  // SPI_WIFI_MISO
#define SPIM_PIN_FUNC_MISO      2

#define SPIM_PIN_NUMBER_MT5932_RESET  HAL_GPIO_11 //HAL_GPIO_31  // GPIO (WIFI_LDO_EN) // from Peter Pan  /* The GPIO pin to control reset pin of MT5932 */

#define SPIM_PIN_NUMBER_MT5932_32K_CLK  HAL_GPIO_12


/* MT5932 relative macros define  */
#define SPIS_CFG_RD_CMD         0x02
#define SPIS_RS_CMD             0x0a
#define SPIS_RD_CMD             0x81
#define SPIS_CFG_WR_CMD         0x04
#define SPIS_WR_CMD             0x06


#define MT5932_STA_TXRX_FIFO_RDY_OFFSET    (2)
#define MT5932_STA_TXRX_FIFO_RDY_MASK      (0x1 << MT5932_STA_TXRX_FIFO_RDY_OFFSET)
#define MT5932_STA_RDWR_FINISH_OFFSET      (5)
#define MT5932_STA_RDWR_FINISH_MASK        (0x1 << MT5932_STA_RDWR_FINISH_OFFSET)

#else // #elif (PRODUCT_VERSION == 2625)

#define SPIM_PIN_NUMBER_CS      HAL_GPIO_17
#define SPIM_PIN_NUMBER_MT5932_RESET  HAL_GPIO_18   /* The GPIO pin to control reset pin of MT5932 */
//#define SPIM_PIN_NUMBER_MT5932_RESET  HAL_GPIO_11   /* The GPIO pin to control reset pin of MT5932 */
#endif

#define SIPS_CMDSET_XBOOT       0
#define SIPS_CMDSET_FW          1

#define MT5932_MAX_LOAD_SIZE           (378*1024) /* size for the load of MT5932 */

#define XBOOT_MAGIC_BIN_LOAD_START      0x54535053
#define XBOOT_MAGIC_ACK_BIN_LOAD_START  0x4B415053
#define XBOOT_MAGIC_GET_BIN             0x444D434D
#define XBOOT_MAGIC_ACK_GET_BIN         0x4D4B4341
#define XBOOT_MAGIC_BIN_LOAD_END        0x444D434D
#define XBOOT_MAGIC_ACK_BIN_LOAD_END    0x4D4B4341

/* Private variables ---------------------------------------------------------*/
static volatile bool g_spi_transaction_finish;

#define __XBOOT_SPI_SUPPORT__

#ifdef __XBOOT_SPI_SUPPORT__
#define MAX_TX_BUFFER_SIZE      1024    /* Tx buffer size decided by user's request */
#define MAX_RX_BUFFER_SIZE      64      /* Rx buffer size decided by user's request */

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_tx_buffer[MAX_TX_BUFFER_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_rx_buffer[MAX_RX_BUFFER_SIZE];

static void spi_master_dma_callback(hal_spi_master_callback_event_t event, void *user_data)
{
    g_spi_transaction_finish = true;
}

static void spi_query_slave_status(uint8_t *status)
{
    uint8_t status_cmd = SPIS_RS_CMD;
    uint8_t status_receive[2];
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(SPIM_TEST_PORT, &spi_send_and_receive_config)) {
        return;
    }

    *status = status_receive[1];
}

bool wifi_bus_spi_init(void)
{
    hal_spi_master_config_t spi_config;
    hal_spi_master_advanced_config_t advanced_config;

    /* Reset MT5932, XTAL Select traping share with SPI CS pin, so we set to 0 before reset MT5932 */
    hal_gpio_init(SPIM_PIN_NUMBER_CS);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, 0);
    hal_gpio_set_direction(SPIM_PIN_NUMBER_CS, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(SPIM_PIN_NUMBER_CS, HAL_GPIO_DATA_LOW);

    /* Reset MT5932, JTAG Select traping share with SPI MOSI pin, so we set to 1 before reset MT5932 */
    hal_gpio_init(SPIM_PIN_NUMBER_MOSI);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MOSI, 0);
    hal_gpio_set_direction(SPIM_PIN_NUMBER_MOSI, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(SPIM_PIN_NUMBER_MOSI, HAL_GPIO_DATA_HIGH);

    printf("Configure 26MHz\r\n");
    hal_clock_FREF_force_output(1);
    
    printf("Configure 32KHz\r\n");
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MT5932_32K_CLK, HAL_GPIO_12_CLKO1);
    hal_gpio_set_clockout(HAL_GPIO_CLOCK_1, HAL_GPIO_CLOCK_MODE_32K);

    hal_gpio_init(SPIM_PIN_NUMBER_MT5932_RESET);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MT5932_RESET, 0);
    hal_gpio_set_direction(SPIM_PIN_NUMBER_MT5932_RESET, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(SPIM_PIN_NUMBER_MT5932_RESET, HAL_GPIO_DATA_LOW);
    hal_gpt_delay_ms(200);
    hal_gpio_set_output(SPIM_PIN_NUMBER_MT5932_RESET, HAL_GPIO_DATA_HIGH);

    /* wait MT5932 finish straping */
    hal_gpt_delay_ms(50);

    /* Step1: GPIO configuration for SPI master */
    hal_gpio_init(SPIM_PIN_NUMBER_CS);
    hal_gpio_init(SPIM_PIN_NUMBER_CLK);
    hal_gpio_init(SPIM_PIN_NUMBER_MOSI);
    hal_gpio_init(SPIM_PIN_NUMBER_MISO);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, SPIM_PIN_FUNC_CS);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CLK, SPIM_PIN_FUNC_CLK);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MOSI, SPIM_PIN_FUNC_MOSI);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MISO, SPIM_PIN_FUNC_MISO);

    /* Step2: Initializes SPI master with normal mode */
    spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
    spi_config.clock_frequency = SPIM_TEST_FREQUENCY;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(SPIM_TEST_PORT, &spi_config)) {
        return false;
    }

    advanced_config.byte_order = HAL_SPI_MASTER_LITTLE_ENDIAN;
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
    advanced_config.get_tick = HAL_SPI_MASTER_NO_GET_TICK_MODE;
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_advanced_config(SPIM_TEST_PORT, &advanced_config)) {
        return false;
    }

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_register_callback(SPIM_TEST_PORT, spi_master_dma_callback, NULL)) {
        return false;
    }

    return true;
}

bool wifi_bus_spi_deinit(void)
{
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(SPIM_TEST_PORT)) {
        return false;
    }

    hal_gpio_deinit(SPIM_PIN_NUMBER_CS);
    hal_gpio_deinit(SPIM_PIN_NUMBER_CLK);
    hal_gpio_deinit(SPIM_PIN_NUMBER_MOSI);
    hal_gpio_deinit(SPIM_PIN_NUMBER_MISO);

    return true;
}

bool wifi_read_bus_spi(uint32_t offset, uint32_t length, uint8_t *read_buffer)
{
    uint8_t cfg_rd_cmd[9];
    uint8_t status_receive = 0;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    spi_query_slave_status(&status_receive);

    cfg_rd_cmd[0] = SPIS_CFG_RD_CMD;
    cfg_rd_cmd[1] = offset & 0xff;
    cfg_rd_cmd[2] = (offset >> 8) & 0xff;
    cfg_rd_cmd[3] = (offset >> 16) & 0xff;
    cfg_rd_cmd[4] = (offset >> 24) & 0xff;
    cfg_rd_cmd[5] = (length - 1) & 0xff;
    cfg_rd_cmd[6] = ((length - 1) >> 8) & 0xff;
    cfg_rd_cmd[7] = ((length - 1) >> 16) & 0xff;
    cfg_rd_cmd[8] = ((length - 1) >> 24) & 0xff;
    do {
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_rd_cmd, 9)) {
            return false;
        }
        hal_gpt_delay_ms(100);
        
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)MT5932_STA_TXRX_FIFO_RDY_MASK) != (uint8_t)MT5932_STA_TXRX_FIFO_RDY_MASK);
#if 1
    g_spi_transaction_finish = false;
    g_transfer_tx_buffer[0] = SPIS_RD_CMD;
    memset(read_buffer, 0, (length + 1));
    spi_send_and_receive_config.receive_length = length + 1;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = g_transfer_tx_buffer;
    spi_send_and_receive_config.receive_buffer = g_transfer_rx_buffer;
    hal_spi_master_status_t status;
    status = hal_spi_master_send_and_receive_dma(SPIM_TEST_PORT, &spi_send_and_receive_config);
    if (HAL_SPI_MASTER_STATUS_OK != status) { // if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_dma(SPIM_TEST_PORT, &spi_send_and_receive_config)) {
        return false;
    }

    while (g_spi_transaction_finish == false);

    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)MT5932_STA_RDWR_FINISH_MASK) != (uint8_t)MT5932_STA_RDWR_FINISH_MASK);

    os_memcpy(read_buffer, &(g_transfer_rx_buffer[1]), length);
#endif
    return true;
}

bool wifi_write_bus_spi(uint32_t offset, uint32_t length, uint8_t *write_buffer)
{
    uint8_t cfg_wr_cmd[9];
    uint8_t status_receive = 0;
    uint32_t remain_length;
    uint32_t count = 0;

    cfg_wr_cmd[0] = SPIS_CFG_WR_CMD;
    cfg_wr_cmd[1] = offset & 0xff;
    cfg_wr_cmd[2] = (offset >> 8) & 0xff;
    cfg_wr_cmd[3] = (offset >> 16) & 0xff;
    cfg_wr_cmd[4] = (offset >> 24) & 0xff;
    cfg_wr_cmd[5] = (length - 1) & 0xff;
    cfg_wr_cmd[6] = ((length - 1) >> 8) & 0xff;
    cfg_wr_cmd[7] = ((length - 1) >> 16) & 0xff;
    cfg_wr_cmd[8] = ((length - 1) >> 24) & 0xff;

    while ((status_receive & (uint8_t)MT5932_STA_TXRX_FIFO_RDY_MASK) != (uint8_t)MT5932_STA_TXRX_FIFO_RDY_MASK) {
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_wr_cmd, 9)) {
            return false;
        }
        spi_query_slave_status(&status_receive);
    }

    remain_length = length;
    // Send ROM data 1K/per transmit
    if(remain_length > 1023)
    {
        printf("  SPI master starts to send wr_cmd command to slave using DMA mode\r\n");
        g_spi_transaction_finish = false;
        
        // Send only 1 byte, SPIS_WR_CMD
        printf("Send only 1 byte, SPIS_WR_CMD +\r\n");
        g_transfer_tx_buffer[0] = SPIS_WR_CMD;
        
        hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, 0);
        hal_gpio_set_direction(SPIM_PIN_NUMBER_CS, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output(SPIM_PIN_NUMBER_CS, HAL_GPIO_DATA_LOW);

        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_buffer, 1)) {
            return false;
        }
        while (g_spi_transaction_finish == false);
        printf("Send only 1 byte, SPIS_WR_CMD -\r\n");
        
        while(remain_length > 1024)
        {
            g_spi_transaction_finish = false;
            os_memcpy(&(g_transfer_tx_buffer[0]), &(write_buffer[count*1024]), 1024);
            if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_buffer, 1024)) {
                return false;
            }
            while (g_spi_transaction_finish == false);
            remain_length -= 1024;
            count++;
            //printf("count = %d, remain_length = %d in loop\r\n", count, remain_length);
        }

        //printf("count1 = %d, remain_length = %d\r\n", count, remain_length);
        g_spi_transaction_finish = false;
        os_memcpy(&(g_transfer_tx_buffer[0]), &(write_buffer[count*1024]), remain_length);
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_buffer, remain_length)) {
            return false;
        }
        //printf("count2 = %d, remain_length = %d\r\n", count, remain_length);
        printf("hal_spi_master_send_dma return HAL_SPI_MASTER_STATUS_OK +\r\n");
        while (g_spi_transaction_finish == false);
        printf("Leave while (g_spi_transaction_finish == false);\r\n");
        hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, SPIM_PIN_FUNC_CS);
    }else {
        printf("  Original\r\n");
        g_spi_transaction_finish = false;
        
        // Send only 1 byte, SPIS_WR_CMD
        printf("Send less then 1024 bytes +\r\n");
        g_transfer_tx_buffer[0] = SPIS_WR_CMD;
        os_memcpy(&(g_transfer_tx_buffer[1]), write_buffer, length);
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_buffer, length+1)) {
            return false;
        }
        while (g_spi_transaction_finish == false);
        printf("Send less then 1024 bytes %d-\r\n", length);
    }

    spi_query_slave_status(&status_receive);
    printf("status_receive = 0x%X +\r\n", status_receive);
    
    do {
        spi_query_slave_status(&status_receive);

        uint32_t counter = 0;
        counter++;
        if (counter >= 10000) {
            counter = 0;
            printf("status_receive = 0x%X\r\n", status_receive);
        }

    } while ((status_receive & (uint8_t)MT5932_STA_RDWR_FINISH_MASK) != (uint8_t)MT5932_STA_RDWR_FINISH_MASK);

    printf("status_receive = 0x%X -\r\n", status_receive);

    return true;
}


#endif //#ifdef __XBOOT_SPI_SUPPORT__
/**
*@brief  Example of spi send datas. In this function, SPI master send various commond to slave.
*@param  None.
*@return None.
*/
bool xboot_over_spi_fw_download(uint8_t *address, uint32_t length)
{
    uint32_t i, checksum, checksum_received;
    xboot_cmd_status_t *xboot_status;
    xboot_cmd_getbin_t *xboot_getbin;
    uint32_t *p_buffer;
    uint8_t buffer[32];

    printf("---xboot_over_spi_example begins---\r\n\r\n");
    printf("Example will begin to run after 5 secound....\r\n\r\n");
    //hal_gpt_delay_ms(5000);

    // Check parameter
    if (length > MT5932_MAX_LOAD_SIZE) {
        printf("Invalid length = %d \r\n", (int)length);
        return false;
    }
    length = (length + 3) & ~(0x3);

    wifi_bus_spi_init();

    // wfcm_if_init();

    /* Step4: Wait XOOT start command */
    printf("Wait XOOT start command\r\n");
    wifi_read_bus_spi(SIPS_CMDSET_XBOOT, 4, buffer);
    p_buffer = (uint32_t *)buffer;
    if (p_buffer[0] != XBOOT_MAGIC_BIN_LOAD_START) {
        printf("Wait XOOT start command fail\r\n\r\n");
        return false;
    }

    /* Step5: Send ACK to slave for XOOT start command */
    printf("Send ACK to slave for XOOT start command\r\n");
    p_buffer = (uint32_t *)buffer;
    p_buffer[0] = XBOOT_MAGIC_ACK_BIN_LOAD_START;
    wifi_write_bus_spi(SIPS_CMDSET_XBOOT, 4, buffer); // spi_master_write_data(SIPS_CMDSET_XBOOT, buffer, 4);

    /* Step6: Wait XOOT Get Bin command */
    printf("Wait XOOT Get Bin command\r\n");
    wifi_read_bus_spi(SIPS_CMDSET_XBOOT, 32, buffer);
    xboot_getbin = (xboot_cmd_getbin_t *)buffer;
    if ((xboot_getbin->m_magic != XBOOT_MAGIC_GET_BIN) || (xboot_getbin->m_msg_id != GET_BIN)) {
        printf("Wait XOOT Get Bin command fail\r\n");
        return false;
    }

    /* Step7: Send ACK to slave for XOOT Get Bin command */
    printf("Send ACK to slave for XOOT Get Bin command\r\n");
    if (length > xboot_getbin->m_len) {
        printf("Size mismatch. Allowable size = %d \r\n", (int)xboot_getbin->m_len);
        return false;
    }
    os_memset(buffer, 0, 32);
    xboot_status = (xboot_cmd_status_t *)buffer;
    xboot_status->m_magic = XBOOT_MAGIC_ACK_GET_BIN;
    xboot_status->m_msg_id = ACK_GET_BIN;
    xboot_status->m_status = 0;
    xboot_status->m_reserve[0] = length;
    wifi_write_bus_spi(SIPS_CMDSET_XBOOT, 32, buffer);

    /* Step8: Send load of MT5932 to SPI slave */
    printf("Send load of MT5932 to SPI slave\r\n");
    checksum = 0;
    p_buffer = (uint32_t *)address;
    for (i = 0; i < length / 4; i++) {
        checksum ^= *p_buffer;
        p_buffer++;
    }
    printf("  Calculate Checksum = 0x%x\r\n", (unsigned int)checksum);
    wifi_write_bus_spi(SIPS_CMDSET_XBOOT, length, (uint8_t *)address);

    /* Step9: Wait XOOT Bin Load End command */
    printf("Wait XOOT Bin Load End command\r\n");
    wifi_read_bus_spi(SIPS_CMDSET_XBOOT, 32, buffer);
    xboot_getbin = (xboot_cmd_getbin_t *)buffer;
    if ((xboot_getbin->m_magic != XBOOT_MAGIC_BIN_LOAD_END) || (xboot_getbin->m_msg_id != BIN_LOAD_END)) {
        printf("Wait XOOT Bin Load End command fail\r\n");
        return false;
    }
    checksum_received = xboot_status->m_reserve[0];
    printf("received Checksum = 0x%x\r\n", (unsigned int)(xboot_status->m_reserve[0]));

    /* Step10: Send ACK to slave for XOOT Bin Load End command */
    printf("Send ACK to slave for XOOT Bin Load End command\r\n");
    os_memset(buffer, 0, 32);
    xboot_status = (xboot_cmd_status_t *)buffer;
    xboot_status->m_magic = XBOOT_MAGIC_ACK_BIN_LOAD_END;
    xboot_status->m_msg_id = ACK_BIN_LOAD_END;
    xboot_status->m_status = 0;
    wifi_write_bus_spi(SIPS_CMDSET_XBOOT, 32, buffer);

    printf("\r\nXBoot complete\r\n\r\n");

    wifi_bus_spi_deinit();

    // Compare the checksum
    if (checksum == checksum_received) {
        return true;
    } else {
        return false;
    }
}
