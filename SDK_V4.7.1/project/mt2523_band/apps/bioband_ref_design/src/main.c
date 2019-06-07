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

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_gpio.h"
#include "hal_dvfs.h"
#include "hal_sys_topsm.h"
#include "hal_pdma_internal.h"
#include "hal_gpio.h"
#include "hal_dcxo.h"
#include "bt_log.h"
#include "ble_dtp.h"
#include "hal_log.h"
#include "hal_keypad_table.h"
#include "ept_keypad_drv.h"
#include "keypad_custom.h"
#include "hal_keypad_powerkey_internal.h"
#include "hal_pmu.h"
#include "hal_sleep_manager.h"
#include "hal_charger.h"
#include "bsp_led.h"
#include "hal_display_dsi.h"
#ifdef GT_PROJECT_ENABLE
#include "app_main.h"
#include "watch.h"
#endif //GT_PROJECT_ENABLE
#if defined(FOTA_UBIN_DECOUPLING)
#include "fota_common_wrapper.h"
#endif

#ifdef MTK_SMART_BATTERY_ENABLE
#include "battery_management.h"
#endif

#include "sensor_alg_interface.h"

#include "sensor_bt_spp_server.h"
#include "bt_init.h"
#include "memory_attribute.h"
#include "hal_uart.h"
#include "ble_app_common.h"
//extern void GPIO_setting_init(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//#define BT_DEMO

#include "sensor_demo.h"

/* Private variables ---------------------------------------------------------*/
#define DTP_ENABLE
//#define BLE_HCI_UART1_IO

#if defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif

#include "bsp_gpio_ept_config.h"
#include "memory_map.h"
#include "hal_rtc.h"

#define ATCI_TASK_STACK_SIZE 2048

#include "task_def.h"
#include "nvdm.h"
#include "fota_dl.h"
#include "bt_callback_manager.h"
#include "bioband_bt_common.h"

#ifdef ATCMD_VIA_SPPS
extern void bt_spps_atci_main(void);
#endif

#define UI_MSG_SENSOR 1
#define UI_MSG_BP_BT_TRANS 2
typedef struct ui_task_message_struct {
    int32_t message_id;
    int32_t param1;
    void *param2;
} ui_task_message_struct_t;

static TaskHandle_t ui_main_task_handler;
QueueHandle_t ui_main_event_queue;

#ifdef BLE_HCI_UART1_IO
#define VFIFO_SIZE_RX_BT (512)
#define VFIFO_SIZE_TX_BT (1024)
#define VFIFO_ALERT_LENGTH_BT (20)
unsigned char  g_uart_rx_buff[1024];

static uint8_t  ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_bt_cmd_tx_vfifo[VFIFO_SIZE_TX_BT];
static uint8_t  ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_bt_cmd_rx_vfifo[VFIFO_SIZE_RX_BT];
#endif
/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware(void);


void vApplicationTickHook(void)
{

}

int __io_putchar(int ch)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    log_putchar(ch);
    return ch;
}




#ifdef FORCE_DL_ENABLE

#define BL_RTC_CON      ((volatile uint16_t *)(0xA21E0078))
#define BL_RTC_BBPU     ((volatile uint16_t *)(0xA21E0000)) /* [6] CBUSY */
#define BL_RTC_WRTGR    ((volatile uint16_t *)(0xA21E0074)) /* [0] WRTGR */

static void rtc_write_trigger(void)
{
    uint32_t count = 0;

    // RTC write trigger
    *BL_RTC_WRTGR = 1;
    // RTC wait busy
    while (count < 0x6EEEEE) {
        if ((0x40 & (*BL_RTC_BBPU)) == 0) {
            break;
        }
        count++;
    }
     if (count >= 0x6EEEEE) {
         printf("RTC write error\r\n");
    }
}

static void rtc_mode_control(int32_t set)
{
    uint16_t rtc_con = 0;

    rtc_con = *BL_RTC_CON;
    printf("1 RTC_CON yo %x\n\r", rtc_con);

    if (set != 0) {
        /* set RTC mode */
        if ((rtc_con & 0x100) == 0) {
            rtc_con |= 0x100; // set bit 8
            *BL_RTC_CON = rtc_con;
            rtc_write_trigger();
            printf("2 Set RTC mode (%x)\n\r", *BL_RTC_CON);
        }
    } else {
        /* clear RTC mode */
        if (rtc_con & 0x100) {
            rtc_con &= 0xFEFF; // clear bit 8
            *BL_RTC_CON = rtc_con;
            rtc_write_trigger();
            printf("3 Clear RTC mode (%x)\n\r", *BL_RTC_CON);
        }
    }
    //hal_rtc_disable_alarm();
}

#endif

#ifdef SENSOR_DEMO

#ifdef DTP_ENABLE
bool dtp_enabled;
bool dtp_sent_done;
void sensor_dtp_event_callback(dtp_event_t *evt)
{
    //warning: make sure not too much operations in this function, or make bt task stack overflow
    switch (evt->event_id) {
        case DTP_EVENT_ENABLED:
            dtp_enabled = true;
            dtp_sent_done = true;
            break;
        case DTP_EVENT_DISABLED:
            dtp_enabled = false;
            break;
        case DTP_EVENT_DATA_SENT:
            if (evt->result == DTP_SUCCESS) {
                dtp_sent_done = true;
            }
            break;
        case DTP_EVENT_DATA_RECEIVED:
            /*LOG_I(DTP, "length = %d, data = %x %x %x %x %x\n", evt->length,
                evt->data[0], evt->data[1], evt->data[2], evt->data[3], evt->data[4]);*/
            break;
        default:
            break;
    }
}
#endif /*DTP_ENABLE*/

#ifdef RECEIVE_DATA_ENABLE
void spps_receive_data_callback(uint16_t data_len, uint8_t *data)
{
    //LOG_I(SPPS, "data_len = %d, data = %x %x %x %x %x\r\n", data_len, data[0], data[1], data[2], data[3], data[4]);

    /*Don't do too much in this callback. It runs in bt task.*/
    return;
}
#endif

static int32_t ui_main_timer_id;
TimerHandle_t xtimer_ui_main;
static uint32_t key_processing;

/* running at timer task*/
void vuimain_timeout(TimerHandle_t pxTimer)
{
    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
    key_processing = 0;
}

#endif /*SENSOR_DEMO*/

/**
* @brief       This function is to initialize cache controller.
* @param[in]   None.
* @return      None.
*/
static void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */
/* UBIN length */
#ifdef CM4_UBIN_LENGTH
        {BL_BASE, BL_LENGTH + CM4_LENGTH + CM4_UBIN_LENGTH},
#else
        {BL_BASE, BL_LENGTH + CM4_LENGTH},
#endif
        /* virtual memory */
        {VRAM_BASE, VRAM_LENGTH}
    };

    region_number = (hal_cache_region_t) (sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));

    hal_cache_init();
    hal_cache_set_size(HAL_CACHE_SIZE_32KB);
    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }
    for ( ; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }
    hal_cache_enable();
}

/**
* @brief       caculate actual bit value of region size.
* @param[in]   region_size: actual region size.
* @return      corresponding bit value of region size for MPU setting.
*/
static uint32_t caculate_mpu_region_size( uint32_t region_size )
{
    uint32_t count;

    if (region_size < 32) {
        return 0;
    }
    for (count = 0; ((region_size  & 0x80000000) == 0); count++, region_size  <<= 1);
    return 30 - count;
}

/**
* @brief       This function is to initialize MPU.
* @param[in]   None.
* @return      None.
*/
static void mpu_init(void)
{
    hal_mpu_region_t region, region_number;
    hal_mpu_region_config_t region_config;
    typedef struct {
        uint32_t mpu_region_base_address;/**< MPU region start address */
        uint32_t mpu_region_end_address;/**< MPU region end address */
        hal_mpu_access_permission_t mpu_region_access_permission; /**< MPU region access permission */
        uint8_t mpu_subregion_mask; /**< MPU sub region mask*/
        bool mpu_xn;/**< XN attribute of MPU, if set TRUE, execution of an instruction fetched from the corresponding region is not permitted */
    } mpu_region_information_t;

#if defined (__GNUC__) || defined (__CC_ARM)

    //RAM: VECTOR TABLE+RAM CODE+RO DATA
    extern uint32_t Image$$RAM_TEXT$$Base;
    extern uint32_t Image$$RAM_TEXT$$Limit;
    //TCM: TCM CODE+RO DATA
    extern uint32_t Image$$TCM$$RO$$Base;
    extern uint32_t Image$$TCM$$RO$$Limit;
    //STACK END
    extern unsigned int Image$$STACK$$ZI$$Base[];

    /* MAX region number is 8 */
    mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t) &Image$$RAM_TEXT$$Base, (uint32_t) &Image$$RAM_TEXT$$Limit, HAL_MPU_READONLY, 0x0, FALSE}, //Vector table+RAM code+RAM rodata
        {(uint32_t) &Image$$RAM_TEXT$$Base + VRAM_BASE, (uint32_t) &Image$$RAM_TEXT$$Limit + VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t) &Image$$TCM$$RO$$Base, (uint32_t) &Image$$TCM$$RO$$Limit, HAL_MPU_READONLY, 0x0, FALSE},//TCM code+TCM rodata
        {(uint32_t) &Image$$STACK$$ZI$$Base, (uint32_t) &Image$$STACK$$ZI$$Base + 32, HAL_MPU_READONLY, 0x0, TRUE} //Stack end check for stack overflow
    };

#elif defined (__ICCARM__)

#pragma section = ".intvec"
#pragma section = ".ram_rodata"
#pragma section = ".tcm_code"
#pragma section = ".tcm_rwdata"
#pragma section = "CSTACK"

    /* MAX region number is 8, please DO NOT modify memory attribute of this structure! */
    _Pragma("location=\".ram_rodata\"") static mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t)__section_begin(".intvec"), (uint32_t)__section_end(".ram_rodata"), HAL_MPU_READONLY, 0x0, FALSE},//Vector table+RAM code+RAM rodata
        {(uint32_t)__section_begin(".intvec") + VRAM_BASE, (uint32_t)__section_end(".ram_rodata") + VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t)__section_begin(".tcm_code"), (uint32_t)__section_begin(".tcm_rwdata"), HAL_MPU_READONLY, 0x0, FALSE},//TCM code+TCM rodata
        {(uint32_t)__section_begin("CSTACK"), (uint32_t)__section_begin("CSTACK") + 32, HAL_MPU_READONLY, 0x0, TRUE} //Stack end check for stack overflow
    };

#endif

    hal_mpu_config_t mpu_config = {
        /* PRIVDEFENA, HFNMIENA */
        TRUE, TRUE
    };

    region_number = (hal_mpu_region_t)(sizeof(region_information) / sizeof(region_information[0]));

    hal_mpu_init(&mpu_config);
    for (region = HAL_MPU_REGION_0; region < region_number; region++) {
        /* Updata region information to be configured */
        region_config.mpu_region_address = region_information[region].mpu_region_base_address;
        region_config.mpu_region_size = (hal_mpu_region_size_t) caculate_mpu_region_size(region_information[region].mpu_region_end_address - region_information[region].mpu_region_base_address);
        region_config.mpu_region_access_permission = region_information[region].mpu_region_access_permission;
        region_config.mpu_subregion_mask = region_information[region].mpu_subregion_mask;
        region_config.mpu_xn = region_information[region].mpu_xn;

        hal_mpu_region_configure(region, &region_config);
        hal_mpu_region_enable(region);
    }
    /* make sure unused regions are disabled */
    for (; region < HAL_MPU_REGION_MAX; region++) {
        hal_mpu_region_disable(region);
    }
    hal_mpu_enable();
}

/* keypad handler */
void keypad_user_powerkey_handler(void)
{
    hal_keypad_status_t             ret;
    hal_keypad_powerkey_event_t powekey_event;
    char *string[5] = {"release", "press", "longpress", "repeat", "pmu_longpress"};

    ret = hal_keypad_powerkey_get_key(&powekey_event);

    if (ret == HAL_KEYPAD_STATUS_ERROR) {
        log_hal_info("[keypad][test]powerkey no key in buffer\r\n\r\n");
        return;
    }

    log_hal_info("[keypad][test]powerkey data:[%d], state:[%s]\r\n", powekey_event.key_data, string[powekey_event.state]);

    if (powekey_event.state == 2) {

        log_hal_info("[keypad][test]start to power down\r\n");
        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
        hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW);
        hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_HIGH);


        hal_gpt_delay_ms(3 * 1000);
        hal_sleep_manager_enter_power_off_mode();
    }

}

void keypad_user_key_handler(void)
{
    hal_keypad_status_t             ret;
    hal_keypad_event_t key_event;
    char *string[3] = {"release", "press", "longpress"};

    ret = hal_keypad_get_key(&key_event);

    if (ret == HAL_KEYPAD_STATUS_ERROR) {
        log_hal_info("[keypad][test]key no key in buffer\r\n\r\n");
        return;
    }

    log_hal_info("[keypad][test]key data:[%d], state:[%s]\r\n", key_event.key_data, string[key_event.state]);

    if (key_event.state == 1) { /* key release */

        if (key_processing  == 0) {
            key_processing = 1;

            //add trigger HRV/BP here

            BaseType_t xHigherPriorityTaskWoken;
            ui_task_message_struct_t message;
            message.message_id = UI_MSG_SENSOR;

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
            log_hal_info("[keypad][test] Trigger HRV \r\n");
            message.param1 = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
            log_hal_info("[keypad][test] Trigger B.P. \r\n");
            message.param1 = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
#endif
            message.param2 = NULL;

            xQueueSendFromISR(ui_main_event_queue, &message, &xHigherPriorityTaskWoken);
        } else {
            log_hal_info("ker processing\r\n");
        }
    }
}

/*use powerkey*/
void hal_key_enable(void)
{
    bool                            ret_bool;
    hal_keypad_status_t ret_state;


    /*init keypad and powerkey*/
    ret_bool = keypad_custom_powerkey_init();
    if (ret_bool == false) {
        log_hal_info("[keypad][test]keypad_custom_init init fail\r\n");
    }

    ret_state = hal_keypad_powerkey_register_callback((hal_keypad_callback_t)keypad_user_powerkey_handler, NULL);
    if (ret_state != HAL_KEYPAD_STATUS_OK) {
        log_hal_info("[keypad][test]hal_keypad_powerkey_register_callback fail, state = %d\r\n", ret_state);

        hal_sleep_manager_enter_power_off_mode();

    }

    ret_bool = keypad_custom_normal_init();
    if (ret_bool == false) {
        log_hal_info("[keypad][test]keypad_custom_init init fail\r\n");
    }


    ret_state = hal_keypad_register_callback((hal_keypad_callback_t)keypad_user_key_handler, NULL);
    if (ret_state != HAL_KEYPAD_STATUS_OK) {
        log_hal_info("[keypad][test]hal_keypad_key_register_callback fail, state = %d\r\n", ret_state);
    }


    ret_state = hal_keypad_enable();
    if (ret_state != HAL_KEYPAD_STATUS_OK) {
        log_hal_info("[keypad][test]hal_keypad_enable fail, state = %d\r\n", ret_state);
    }



}

void ui_task_main()
{
    ui_task_message_struct_t queue_item;

    ui_main_timer_id = 0;
    xtimer_ui_main = xTimerCreate("uimain",       /* Just a text name, not used by the kernel. */
                                  (10 * 1000 / portTICK_PERIOD_MS),  /* The timer period in ticks. */
                                  pdFALSE,        /* The timers will auto-reload themselves when they expire. */
                                  (void *) ui_main_timer_id,    /* Assign each timer a unique id equal to its array index. */
                                  vuimain_timeout /* Each timer calls the same callback when it expires. */
                                 );

    if (xtimer_ui_main == NULL) {
        SENSOR_DEMO_LOGI("xTimerofMems create fail \r\n");
    }

    while (1) {
        if (xQueueReceive(ui_main_event_queue, &queue_item, portMAX_DELAY)) {
            switch (queue_item.message_id) {
                case UI_MSG_SENSOR:


#ifdef FUSION_HEART_RATE_VARIABILITY_USE
                    if (queue_item.param1 == SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR) {
                        enable_hrv();
                    }
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
                    if (queue_item.param1 == SENSOR_TYPE_BLOOD_PRESSURE_MONITOR) {
                        enable_bp();
                    }
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
                    if (queue_item.param1 == SENSOR_TYPE_HEART_RATE_MONITOR) {
                        enable_hr();
                    }
#endif

#ifdef FUSION_SLEEP_TRACKER_USE
                    if (queue_item.param1 == SENSOR_TYPE_SLEEP) {
                        app_enable_sleep_tracker();
                    }
#endif

#ifdef FUSION_PEDOMETER_USE
                    if (queue_item.param1 == SENSOR_TYPE_PEDOMETER) {
                        enable_pedometer();
                    }
#endif
                    break;

                default:
                    break;
            }
        }
    }
}

#ifdef BLE_HCI_UART1_IO

bool bt_hci_log_enabled(void)
{
    return true;
}


void ble_io_uart_irq(hal_uart_callback_event_t event, void *parameter)
{
    uint32_t length = 0;

    length = hal_uart_get_available_receive_bytes(HAL_UART_1);
    if (HAL_UART_EVENT_READY_TO_READ == event) {
        length = hal_uart_get_available_receive_bytes(HAL_UART_1);
        if (length > 1024) {
            length = 1024;
        }
        hal_uart_receive_dma(HAL_UART_1, g_uart_rx_buff, length);
    }
}

static hal_uart_status_t ble_io_uart_init(hal_uart_port_t port)
{
    hal_uart_status_t ret;
    /* Configure UART PORT */
    hal_uart_config_t uart_config = {
        .baudrate = HAL_UART_BAUDRATE_115200,
        .word_length = HAL_UART_WORD_LENGTH_8,
        .stop_bit = HAL_UART_STOP_BIT_1,
        .parity = HAL_UART_PARITY_NONE
    };


    LOG_I(DTP, "UART INIT!!!");

    ret = hal_uart_init(port, &uart_config);

    if (HAL_UART_STATUS_OK == ret) {

        hal_uart_dma_config_t   dma_config = {
            .send_vfifo_buffer              = g_bt_cmd_tx_vfifo,
            .send_vfifo_buffer_size         = VFIFO_SIZE_TX_BT,
            .send_vfifo_threshold_size      = 128,
            .receive_vfifo_buffer           = g_bt_cmd_rx_vfifo,
            .receive_vfifo_buffer_size      = VFIFO_SIZE_RX_BT,
            .receive_vfifo_threshold_size   = 128,
            .receive_vfifo_alert_size       = VFIFO_ALERT_LENGTH_BT
        };

        ret = hal_uart_set_dma(port, &dma_config);

        ret = hal_uart_register_callback(port, ble_io_uart_irq, NULL);

    }
    return ret;
}

#endif

extern bt_status_t bioband_bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);


/* port service related */
#if defined(MTK_PORT_SERVICE_ENABLE)
static void syslog_port_service_init(void)
{
    serial_port_dev_t syslog_port;
    serial_port_setting_uart_t uart_setting;

    if (serial_port_config_read_dev_number("syslog", &syslog_port) != SERIAL_PORT_STATUS_OK) {
        //syslog_port = SERIAL_PORT_DEV_UART_0;
        syslog_port = SERIAL_PORT_DEV_USB_COM2;
        serial_port_config_write_dev_number("syslog", syslog_port);
        uart_setting.baudrate = HAL_UART_BAUDRATE_115200;
        serial_port_config_write_dev_setting(syslog_port, (serial_port_dev_setting_t *)&uart_setting);
    }
}
#endif


void bp_nvdm_control() {

    uint8_t buf[11];
    uint32_t buf_size;
    nvdm_status_t nvdm_status;

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "userid", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {

        nvdm_write_data_item("2511", "userid", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0000000000", 11);

        LOG_I(hal, "read nvdm userid fail \n");

    } else {
        LOG_I(hal, "userid = %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "height", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {

        nvdm_write_data_item("2511", "height", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"173", 4);
        LOG_I(hal, "read nvdm gender fail \n");

    } else {
        LOG_I(hal, "height %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "weight", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {

        nvdm_write_data_item("2511", "weight", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"65", 4);

    } else {
        LOG_I(hal, "weight %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "gender", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "gender", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"1", 2);

    } else {
        LOG_I(hal, "gender %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "age", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {

        nvdm_write_data_item("2511", "age", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"35", 4);

    } else {
        LOG_I(hal, "age %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "handlen", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "handlen", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"80", 4);
    } else {
        LOG_I(hal, "handlen %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "mode", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "mode", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "mode %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "sbp1", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "sbp1", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "sbp1 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "dbp1", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "dbp1", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "dbp1 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "sbp2", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "sbp2", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "sbp2 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "dbp2", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "dbp2", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "dbp2 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "sbp3", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "sbp3", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "sbp3 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "dbp3", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "dbp3", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
    } else {
        LOG_I(hal, "dbp3 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para1", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para1", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para1 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para2", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para2", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para2 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para3", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para3", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para3 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para4", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para4", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para4 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para5", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para5", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para5 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para6", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para6", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para6 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para7", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para7", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para7 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para8", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para8", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para8 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para9", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para9", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para9 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para10", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para10", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para10 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para11", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para11", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para11 %s\n", buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para12", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        nvdm_write_data_item("2511", "para12", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 11);
    } else {
        LOG_I(hal, "para12 %s\n", buf);
    }


}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    nvdm_status_t nvdm_init_status;

    /* SystemClock Config */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    log_uart_init(HAL_UART_0);
    
    nvdm_init_status = nvdm_init();


#if defined(MTK_PORT_SERVICE_ENABLE)
    syslog_port_service_init();
#else
#endif

#if defined (MTK_PORT_SERVICE_ENABLE)
    /* Reduce bt-spp log to make syslog over bt-spp work. */
    //log_config_print_level(bt_spp, PRINT_LEVEL_WARNING);
    //log_config_print_level(SPP_PORT, PRINT_LEVEL_WARNING);
#endif

#ifdef MTK_USB_DEMO_ENABLED
    usb_boot_init();
#endif
    /* init sys log */

    log_init(NULL, NULL, NULL);

#if defined(FOTA_UBIN_DECOUPLING)
    /* For separate bin fota, initialize ubin region */
    printf("[FOTA UBIN] ustartup enter\r\n");
    ustartup();
    printf("[FOTA UBIN] ustartup exit\r\n");
#endif

    hal_sleep_manager_init();

    hal_dcxo_init();

    clock_dump_info();
    
    /* turn off MIPI TX analog power */
    hal_display_dsi_init(false);
    hal_display_dsi_deinit();

#ifdef FORCE_DL_ENABLE
    rtc_mode_control(0);
#endif

   
#ifdef MTK_LED_ENABLE
    if (BSP_LED_OK != bsp_led_init(0)) LOG_I(hal, "led 0 init fail \n\r");
    if (BSP_LED_OK != bsp_led_init(1)) LOG_I(hal, "led 1 init fail \n\r");
    if (BSP_LED_OK != bsp_led_init(2)) LOG_I(hal, "led 2 init fail \n\r");
#else
    if (HAL_GPIO_STATUS_OK != hal_gpio_set_direction(HAL_GPIO_4, HAL_GPIO_DIRECTION_OUTPUT)) LOG_I(hal, "gpio 4 set out fail\n\r");
    if (HAL_GPIO_STATUS_OK != hal_gpio_set_direction(HAL_GPIO_5, HAL_GPIO_DIRECTION_OUTPUT)) LOG_I(hal, "gpio 5 set out fail\n\r");
    if (HAL_GPIO_STATUS_OK != hal_gpio_set_direction(HAL_GPIO_6, HAL_GPIO_DIRECTION_OUTPUT)) LOG_I(hal, "gpio 6 set out fail\n\r");
#endif
    
    while (1) {
        int32_t battery_voltage;
        bool charger_status;

        hal_charger_meter_get_battery_voltage_sense(&battery_voltage);
        LOG_I(hal, "Check battery_voltage = %d mV \n\r", battery_voltage);

        if (battery_voltage >= 3400) {
            if ( HAL_GPIO_STATUS_OK != hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_LOW)) log_hal_info("GPIO set out6 low fail");
            break;
        } else {
            hal_charger_get_charger_detect_status(&charger_status);

            if (charger_status == true) {

                if ( HAL_GPIO_STATUS_OK !=hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_HIGH)) log_hal_info("GPIO set out6 high fail");

                LOG_I(hal, "SW charging battery_voltage = %d mV \n\r", battery_voltage);
                hal_charger_init();
                hal_charger_set_charging_current(HAL_CHARGE_CURRENT_120_MA);
                hal_charger_enable(true);
                hal_charger_reset_watchdog_timer();
                hal_gpt_delay_ms(10 * 1000);

            } else {
                LOG_I(hal, "Low battery power off battery_voltage = %d mV \n\r", battery_voltage);

                hal_gpt_delay_ms(1000);
                hal_sleep_manager_enter_power_off_mode();
            }

        }
    }


    /* for to enable green led for boot-up check without LCM */

#ifdef BOOTUP_GREEN_LED
    if ( HAL_GPIO_STATUS_OK !=hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_HIGH)) log_hal_info("GPIO set out5 high fail");
#endif


    if (nvdm_init_status != NVDM_STATUS_OK) {

        LOG_I(hal, "========== nvdm_init wrong wrong wrong ====================== \n");
    } else {

	bp_nvdm_control();
    
    }


#ifdef BLE_HCI_UART1_IO
    ble_io_uart_init(HAL_UART_1);
#endif

#ifdef MTK_SMART_BATTERY_ENABLE
    battery_management_init();
#endif

#ifdef SENSOR_DEMO

#ifdef DTP_ENABLE
    ble_dtp_register(sensor_dtp_event_callback);
#endif

#ifdef RECEIVE_DATA_ENABLE
    sensor_bt_spp_register(spps_receive_data_callback);
#endif
    enable_all_sensors();
#endif /* SENSOR_DEMO*/

#ifdef ATCMD_VIA_SPPS
    bt_spps_atci_main();
#endif

    /* Main UI task */
    ui_main_event_queue = xQueueCreate(10 , sizeof(ui_task_message_struct_t));
    xTaskCreate(ui_task_main, UI_MAIN_TASK_NAME, UI_MAIN_TASK_STACK_SIZE / sizeof(StackType_t), NULL, UI_MAIN_TASK_PRIO, &ui_main_task_handler);

#ifdef GT_PROJECT_ENABLE
    xTaskCreate(vGTMain_Task, APP_MAIN_TASK_NAME, APP_MAIN_TASK_STACK_SIZE / sizeof(StackType_t), NULL, APP_MAIN_TASK_PRIO, NULL);
#else
    hal_key_enable();

#endif //GT_PROJECT_ENABLE  

    log_config_print_level(NOTIFY, PRINT_LEVEL_ERROR);
    log_config_print_level(NOTIFY_SRV, PRINT_LEVEL_ERROR);
#if defined(FOTA_UBIN_DECOUPLING)
    log_config_print_level(fota_ubin, PRINT_LEVEL_ERROR);
#endif

    bioband_bt_callback_init();
    ble_app_common_init();
    fota_dl_init();
    bt_task_init();

#ifndef BOOTUP_GREEN_LED
    if ( HAL_GPIO_STATUS_OK !=hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW)) log_hal_info("GPIO set out5 high fail");
#endif



    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

static void prvSetupHardware(void)
{

    /* system HW init */
    cache_init();

    mpu_init();

    /* peripherals init */
    hal_flash_init(); /* flash init */

    hal_nvic_init();  /* nvic init */

    bsp_ept_gpio_setting_init();


    hal_rtc_init();


}

static void SystemClock_Config(void)
{

    hal_clock_init();

    hal_dvfs_init();
    hal_dvfs_target_cpu_frequency(104000, HAL_DVFS_FREQ_RELATION_H);
}
