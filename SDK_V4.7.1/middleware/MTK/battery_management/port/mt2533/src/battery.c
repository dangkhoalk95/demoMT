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

#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "syslog.h"
#include "hal.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "task_def.h"
#include "battery_management_interface.h"
#include "battery.h"
#include "pmic_max14745.h"
#include "max17048.h"
#include "external_ntc.h"
#include "cust_battery_meter.h"

#ifndef BATTERY_TIMER_PERIOD
#define BATTERY_TIMER_PERIOD    (60)
#endif

#ifndef SOC_CHARGING_TRACKING_TIME_SEC
#define SOC_CHARGING_TRACKING_TIME_SEC (10)
#endif

#ifndef SOC_DISCHARGING_TRACKING_TIME_SEC
#define SOC_DISCHARGING_TRACKING_TIME_SEC (10)
#endif

#ifndef SOC_100_PERCENT_LOCK_TIME_SEC
#define SOC_100_PERCENT_LOCK_TIME_SEC (90)
#endif

static TimerHandle_t xTimerofBattery = NULL;
static TaskHandle_t battery_task_handle = NULL;
static QueueHandle_t battery_queue_handle = NULL;

static bmt_battery_t battery_status = {0};
static bmt_context_t battery_config = {0};
static battery_soc_data_t battery_soc = {0};

static TimerHandle_t xTimerofSocTracking = NULL;

static void battery_send_message(battery_msg_type_t msg)
{
    battery_msg_t msgs;
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t ret;

    if (battery_queue_handle == NULL) {
        return;
    }

    // We have not woken a task at the start of the ISR.
    xHigherPriorityTaskWoken = pdFALSE;

    msgs.msg_id = msg;

    if (0 == hal_nvic_query_exception_number()) {
        ret = xQueueSend(battery_queue_handle, &msgs, 0);
    } else {
        ret = xQueueSendFromISR(battery_queue_handle, &msgs, &xHigherPriorityTaskWoken);
    }

    if (ret != pdTRUE) {
        BMT_ERR("[BATTERY] Send Queue fail!! Queue size = %d ", BMT_QUEUE_LENGTH);
    }
    // Now the buffer is empty we can switch context if necessary.
    if (xHigherPriorityTaskWoken) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }
}

static void battery_show_charging_status(void)
{
    uint32_t charger_mode = (uint32_t) pmic_charger_get_charging_status();

    switch (charger_mode) {
        case EXT_PMIC_MAXIM_PRE_CHARGE_MODE:
        case EXT_PMIC_MAXIM_FAST_CHARGE_CC_MODE:
        case EXT_PMIC_MAXIM_FAST_CHARGE_CV_MODE:
        case EXT_PMIC_MAXIM_MAINTAIN_CHARGE_MODE:
            /* Charging state */
            BMT_DBG("Charging state, charger_mode = %d", charger_mode);
            break;
        case EXT_PMIC_MAXIM_CHARGER_OFF_MODE:
            /* Charging state */
            BMT_DBG("Charger off state, charger_mode = %d", charger_mode);
            break;
        case EXT_PMIC_MAXIM_CHARGING_SUSPEND_MODE:
        case EXT_PMIC_MAXIM_CHARGER_FAULT_MODE:
            /* Charger off state */
            BMT_DBG("Charging suspend or fault state, charger_mode = %d", charger_mode);
            break;
        case EXT_PMIC_MAXIM_MAINTAIN_CHARGE_DONE_MODE:
            /* Charge done state */
            BMT_DBG("Charge done state, charger_mode = %d", charger_mode);
            break;
        default :
            /* Unknown charge state */
            BMT_WARN("Unknown charge state!\n");
            break;
    }
}

void battery_charger_status_handler(void)
{
    battery_send_message(BAT_CHARGER_STATUS_MSG);
}

void battery_charger_detect_handler(void)
{
    battery_send_message(BAT_USB_MSG);
}

static void battery_timeout_callback(TimerHandle_t pxTimer)
{
    battery_send_message(BAT_NTC_UPDATE_MSG);
}

static void battery_soc_tracking_timeout_callback(TimerHandle_t pxTimer)
{
    BMT_INFO("battery_soc_tracking_timeout_callback");
    battery_soc.ui_soc_tracking_flag = false;
    battery_send_message(BAT_SOC_TRAKING_MSG);
}

static bool battery_soc_tracking_timer_init(void)
{

    xTimerofSocTracking = xTimerCreate("xTimerofSocTracking",       /* Just a text name, not used by the kernel. */
                                       (BATTERY_TIMER_PERIOD * 1000 / portTICK_PERIOD_MS),     /* The timer period in ticks. */
                                       pdFALSE,        /* The timers will auto-reload themselves when they expire. */
                                       NULL,   /* Assign each timer a unique id equal to its array index. */
                                       battery_soc_tracking_timeout_callback /* Each timer calls the same callback when it expires. */
                                      );

    if (xTimerofSocTracking == NULL) {
        BMT_ERR("xTimerofSocTracking create fail");
        return false;
    }

    return true;
}

static void battery_soc_tracking_timer_start(uint32_t timer_sec)
{
    const TickType_t xTicksToWait = 2000 / portTICK_PERIOD_MS;

    xTimerChangePeriod(xTimerofSocTracking, timer_sec * 1000 / portTICK_PERIOD_MS, xTicksToWait);
    xTimerReset(xTimerofSocTracking, 1000 / portTICK_PERIOD_MS);

    xTimerStart(xTimerofSocTracking, xTicksToWait);

}

static bool battery_soc_tracking_timer_deinit(void)
{
    if (xTimerofSocTracking != NULL) {
        xTimerDelete(xTimerofSocTracking, (3000 / portTICK_PERIOD_MS));
        return true;
    }
    return false;
}

static void battery_notification(battery_management_event_t event)
{
    if ((battery_config.callback_init == true) && (battery_config.callback_func != NULL)) {
        switch (event) {
            case BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE: {

                battery_config.callback_func(event, NULL);
            }
            break;
            default: {
            }
            break;
        }
    }
}

static int32_t battery_get_battery_capacity_level(int32_t percentage)
{
    int32_t capacity_level = 0;

    if (percentage >= BATTERY_CAPACITY_LEVEL_5) {
        capacity_level = 5;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_4) {
        capacity_level = 4;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_3) {
        capacity_level = 3;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_2) {
        capacity_level = 2;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_1) {
        capacity_level = 1;
    } else {
        capacity_level = 0;
    }

    return capacity_level;
}

static int32_t battery_get_gauge_soc(void)
{
    int32_t soc = 0;

    /* Get MAX17048 gauge SOC */
    soc = max17048_get_soc();
    battery_soc.original_soc = soc;

    if (soc >= 100) {
        soc = 100;
    } else if (soc  <= 0) {
        soc = 0;
    }

    BMT_DBG("orignal_soc %d return_soc %d", battery_soc.original_soc, soc);

    return soc;
}

static bool battery_is_charging_done(void)
{
    /* check charger status */
    if (battery_soc.ui_charger_status == EXT_PMIC_MAXIM_MAINTAIN_CHARGE_DONE_MODE) {
        /* charging done */
        return true;
    } else {
        /* others */
        return false;
    }

}

static bool battery_is_charging(void)
{
    /* check charger status */
    if ((battery_soc.ui_charger_status != EXT_PMIC_MAXIM_CHARGER_OFF_MODE) &&
            (battery_soc.ui_charger_status != EXT_PMIC_MAXIM_CHARGING_SUSPEND_MODE) &&
            (battery_soc.ui_charger_status != EXT_PMIC_MAXIM_CHARGER_FAULT_MODE)) {
        /* charger in progress */
        return true;
    } else {
        /* discharging */
        return false;
    }

}

static void ui_soc_smooth_tracking(void)
{
    BMT_INFO("ui_soc_smooth_tracking ui_soc_tracking_flag %d", (int)battery_soc.ui_soc_tracking_flag);

    if (battery_soc.ui_soc_tracking_flag == false) {

        if (abs(battery_soc.gauge_soc - battery_soc.ui_soc) < 2) {
            if (battery_is_charging()) {
                if (battery_soc.gauge_soc > battery_soc.ui_soc) {
                    battery_soc.ui_soc = battery_soc.gauge_soc;
                    BMT_INFO("battery_charger ui_soc %d = gauge_soc %d ", battery_soc.ui_soc, battery_soc.gauge_soc);
                }
            } else {
                if (battery_soc.gauge_soc < battery_soc.ui_soc) {
                    battery_soc.ui_soc = battery_soc.gauge_soc;
                    BMT_INFO("battery_discharger ui_soc %d = gauge_soc %d ", battery_soc.ui_soc, battery_soc.gauge_soc);
                }
            }
        } else if (battery_soc.gauge_soc > battery_soc.ui_soc) {
            if (battery_is_charging()) {
                battery_soc.ui_soc++;
                battery_soc.ui_soc_tracking_flag = true;
                battery_soc_tracking_timer_start(SOC_CHARGING_TRACKING_TIME_SEC);
                BMT_INFO("battery_charger soc_tracking_timer %d s ui_soc %d gauge_soc %d ", SOC_CHARGING_TRACKING_TIME_SEC, battery_soc.ui_soc, battery_soc.gauge_soc);

            }

        } else if (battery_soc.ui_soc > battery_soc.gauge_soc) {
            if (!battery_is_charging()) {
                battery_soc.ui_soc--;
                battery_soc.ui_soc_tracking_flag = true;
                battery_soc_tracking_timer_start(SOC_DISCHARGING_TRACKING_TIME_SEC);
                BMT_INFO("battery_discharger soc_tracking time %d s ui_soc %d gauge_soc %d ", SOC_DISCHARGING_TRACKING_TIME_SEC, battery_soc.ui_soc, battery_soc.gauge_soc);

            }
        }
    }

}

static int32_t battery_get_soc_mapping(void)
{
    if (battery_status.capacity == BATTERY_INVALID_VALUE) {

        /* Get gauge SOC */
        battery_soc.gauge_soc = battery_get_gauge_soc();
        battery_soc.ui_soc = battery_soc.gauge_soc;
        BMT_INFO("[soc_init]gauge_soc %d ui_soc %d", battery_soc.gauge_soc, battery_soc.ui_soc);

        return  battery_soc.ui_soc;
    }

    /* Get gauge SOC */
    battery_soc.gauge_soc = battery_get_gauge_soc();

    battery_soc.ui_charger_status = (uint32_t) pmic_charger_get_charging_status();

    if (battery_is_charging_done()) {
        BMT_INFO("battery_is_charging_done ui_soc_tracking_flag %d", (int)battery_soc.ui_soc_tracking_flag);
        if (battery_soc.ui_soc_tracking_flag == false) {
            if (abs(100 - battery_soc.ui_soc) < 2) {
                battery_soc.ui_soc = 100;
                battery_soc.ui_soc_100_percent_lock_flag = true;
                BMT_INFO("battery_done ui_soc = 100");
            } else if (100 > battery_soc.ui_soc) {
                battery_soc.ui_soc++;
                battery_soc.ui_soc_tracking_flag = true;
                battery_soc_tracking_timer_start(SOC_CHARGING_TRACKING_TIME_SEC);
                BMT_INFO("battery_done soc_tracking_timer %d s", SOC_CHARGING_TRACKING_TIME_SEC);
            }
        }
        return battery_soc.ui_soc;
    } else if (battery_soc.ui_soc_100_percent_lock_flag == true && battery_is_charging() == false) {
        battery_soc.ui_soc_100_percent_lock_flag = false;
        battery_soc.ui_soc_tracking_flag = true;
        battery_soc_tracking_timer_start(SOC_100_PERCENT_LOCK_TIME_SEC);
        BMT_INFO("ui_soc_100_percent_lock_flag  soc_tracking_timer %d s", SOC_100_PERCENT_LOCK_TIME_SEC);
        return battery_soc.ui_soc;
    }

    ui_soc_smooth_tracking();

    return battery_soc.ui_soc;
}

static void battery_get_battery_data(void)
{
    uint8_t charger_status;
    uint32_t thermal_status;

    /* Get NTC temperature */
    battery_status.temperature = external_ntc_get_temperature();

    /* Get MAX17048 gauge SOC */
    battery_status.capacity = battery_get_soc_mapping();
    BMT_INFO("[SOC] original_soc = %d gauge_soc = %d ui_soc = %d", battery_soc.original_soc, battery_soc.gauge_soc, battery_soc.ui_soc);

    /* Get MAX17048 gauge vcell */
    battery_status.voltage = max17048_get_vcell();

    /* Get battery capacity level */
    battery_status.capacity_level = battery_get_battery_capacity_level(battery_status.capacity);

    BMT_INFO("[Gauge] SOC = %d Vbat = %d Temperature = %d Level = %d", battery_status.capacity, battery_status.voltage, battery_status.temperature, battery_status.capacity_level);

    /* Get charger charger status */
    charger_status = pmic_charger_get_charging_status();

    /* Get charger detect */
    battery_status.charger_exist = pmic_charger_get_charger_exist();

    BMT_INFO("[Charger]ChgStat = %d charger exist = %d", charger_status, battery_status.charger_exist);


    /* Get charger thermal status */
    thermal_status = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x02, 0x07, 0x03);

    BMT_INFO("[Charger]ThermStat  = %d", thermal_status);

}

static void battery_core_thread(void)
{
    battery_get_battery_data();

    battery_notification(BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE);

}

static bool battery_timer_init(void)
{
    const TickType_t xTicksToWait = 2000 / portTICK_PERIOD_MS;


    xTimerofBattery = xTimerCreate("TimerofBattery",       /* Just a text name, not used by the kernel. */
                                   (BATTERY_TIMER_PERIOD * 1000 / portTICK_PERIOD_MS),     /* The timer period in ticks. */
                                   pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                   NULL,   /* Assign each timer a unique id equal to its array index. */
                                   battery_timeout_callback /* Each timer calls the same callback when it expires. */
                                  );

    if (xTimerofBattery == NULL) {
        BMT_ERR("xTimerofBattery create fail");
        return false;
    }

    xTimerStart(xTimerofBattery, xTicksToWait);

    return true;
}

static bool battery_timer_deinit(void)
{
    if (xTimerofBattery != NULL) {
        xTimerDelete(xTimerofBattery, (3000 / portTICK_PERIOD_MS));
        return true;
    }
    return false;
}

static void battery_task_main(void *pvParameters)
{
    battery_msg_t msgs;
    uint8_t charger_exist;
    int32_t ntc_temperature;

    static uint32_t stack_max = 0;
    uint32_t stack_cur = 0;

    battery_send_message(BAT_USB_MSG);

    while (1) {
        if (battery_queue_handle != NULL) {
            if (xQueueReceive(battery_queue_handle, &msgs, portMAX_DELAY)) {

                BMT_INFO("QueueReceive ID  = %d", msgs.msg_id);

                if (msgs.msg_id == BAT_USB_MSG) {
                    charger_exist = pmic_charger_get_charger_exist();
                    BMT_INFO("Charger detect  = %d", charger_exist);

                    if (charger_exist == 1) {
                        /* Restore charger register setup
                           The register is reset to default value upon CHGIN rising edge */
                        pmic_charger_restore_setup();
                    }
                }

                if (msgs.msg_id == BAT_CHARGER_STATUS_MSG) {
                    battery_show_charging_status();
                }

                if (msgs.msg_id == BAT_NTC_UPDATE_MSG) {
                    /* Update MAX17048 gauge temperature */
                    ntc_temperature = external_ntc_get_temperature();
                    max17048_update_temperature(ntc_temperature);

                }

                battery_core_thread();

                stack_cur = (BMT_TASK_STACKSIZE - uxTaskGetStackHighWaterMark(battery_task_handle) * sizeof(portSTACK_TYPE));
                if (stack_cur > stack_max) {
                    stack_max = stack_cur;
                    BMT_DBG("Battery task max-usage:%d \r\n", stack_max);
                }

            }
        }
    }
}

static bool battery_task_deinit(void)
{

    if (battery_task_handle != NULL) {
        vTaskDelete(battery_task_handle);
        battery_task_handle = NULL;
    }

    if (battery_queue_handle != NULL) {
        vQueueDelete(battery_queue_handle);
        battery_queue_handle = NULL;
    }

    return true;
}

static bool battery_task_init(void)
{
    portBASE_TYPE xReturn = pdFALSE;
    /* Queue creation */
    battery_queue_handle = xQueueCreate(BMT_QUEUE_LENGTH, sizeof(battery_msg_t));

    if (battery_queue_handle == NULL) {
        BMT_ERR("battery_queue_handle create fail!");
        return false;
    }

    xReturn = xTaskCreate(battery_task_main, BMT_TASK_NAME, BMT_TASK_STACKSIZE / sizeof(portSTACK_TYPE), NULL, BMT_TASK_PRIO, &battery_task_handle);

    if (xReturn == pdFALSE) {
        vQueueDelete(battery_queue_handle);
        battery_queue_handle = NULL;
        BMT_ERR("battery_task_main task create fail!");
        return false;
    }

    return true;
}

static void battery_data_init(void)
{
    memset(&battery_config, 0, sizeof(bmt_context_t));

    memset(&battery_status, 0, sizeof(bmt_battery_t));

    memset(&battery_soc, 0, sizeof(battery_soc_data_t));

    battery_status.capacity = BATTERY_INVALID_VALUE;
    battery_status.charging_current = BATTERY_INVALID_VALUE;
    battery_status.charger_exist = BATTERY_INVALID_VALUE;
    battery_status.charger_type = BATTERY_INVALID_VALUE;
    battery_status.temperature = BATTERY_INVALID_VALUE;
    battery_status.voltage = BATTERY_INVALID_VALUE;
    battery_status.capacity_level = BATTERY_INVALID_VALUE;

}

static void battery_data_deinit(void)
{
    memset(&battery_config, 0, sizeof(bmt_context_t));
    memset(&battery_status, 0, sizeof(bmt_battery_t));
    memset(&battery_soc, 0, sizeof(battery_soc_data_t));
}

static bool battery_init(void)
{
    if (battery_config.initialized) {
        BMT_DBG("bmt already init");
        return false;
    }

    battery_data_init();

    if (battery_timer_init() == false) {
        BMT_ERR("battery_timer_init fail!");
        return false;
    }

    if (battery_soc_tracking_timer_init() == false) {
        BMT_ERR("battery_soc_tracking_timer_init fail!");
        battery_timer_deinit();
        return false;
    }

    if (battery_task_init() == false) {
        BMT_ERR("battery_task_init fail!");
        battery_timer_deinit();
        battery_soc_tracking_timer_deinit();
        return false;
    }

    /* MAX17048 gauge init */
    if (max17048_init() == false) {
        BMT_ERR("max17048_init fail!");
        battery_task_deinit();
        battery_timer_deinit();
        battery_soc_tracking_timer_deinit();

        return false;

    }

    battery_config.initialized = true;

    return true;
}

static bool battery_deinit(void)
{

    if (!battery_config.initialized) {
        BMT_DBG("bmt not init");
        return false;
    }

    if (battery_timer_deinit() == false) {
        BMT_DBG("battery_timer_deinit fail!");
    }

    if (battery_soc_tracking_timer_deinit() == false) {
        BMT_DBG("battery_soc_tracking_timer_deinit fail!");
    }

    if (battery_task_deinit() == false) {
        BMT_DBG("battery_task_deinit fail!");
    }

    battery_data_deinit();

    return true;
}

static battery_management_status_t battery_management_init_mt2533(void)
{
    if (battery_init() == true) {
        return BATTERY_MANAGEMENT_STATUS_OK;
    } else {
        BMT_ERR("battery_management_init_mt2533 fail!");
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }
}

static int32_t battery_management_get_battery_property_mt2533(battery_property_t property)
{
    int32_t property_value;

    if (!battery_config.initialized) {
        return BATTERY_INVALID_VALUE;
    }

    switch (property) {
        case BATTERY_PROPERTY_CAPACITY:
            if (battery_status.capacity >= 100) {
                property_value = 100;
            } else if (battery_status.capacity <= 0) {
                property_value = 0;
            } else {
                property_value = battery_status.capacity;
            }
            break;

        case BATTERY_PROPERTY_CHARGER_EXIST:
            property_value = battery_status.charger_exist;
            break;

        case BATTERY_PROPERTY_TEMPERATURE:
            property_value = battery_status.temperature;
            break;
        case BATTERY_PROPERTY_VOLTAGE:
            property_value = battery_status.voltage;
            break;
        case BATTERY_PROPERTY_CAPACITY_LEVEL:
            property_value = battery_status.capacity_level;
            break;
        default:
            property_value = BATTERY_INVALID_VALUE;
            break;
    }
    return property_value;
}

static battery_management_status_t battery_management_register_callback_mt2533(battery_management_callback_t callback)
{

    if (!battery_config.initialized) {
        BMT_ERR("battery_management_register_callback_mt2533 fail!");
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

    battery_config.callback_func = callback;
    battery_config.callback_init = true;

    return BATTERY_MANAGEMENT_STATUS_OK;
}

static battery_management_status_t battery_management_deinit_mt2533(void)
{
    if (battery_deinit() == true) {
        return BATTERY_MANAGEMENT_STATUS_OK;
    } else {
        BMT_ERR("battery_management_deinit_mt2533 fail!");
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }
}

bmt_function_t battery_func = {
    battery_management_init_mt2533,
    battery_management_get_battery_property_mt2533,
    battery_management_register_callback_mt2533,
    battery_management_deinit_mt2533
};

