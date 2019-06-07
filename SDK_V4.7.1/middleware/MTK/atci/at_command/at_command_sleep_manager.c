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

#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533 )
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "syslog.h"
#include "hal_cm4_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_bsi.h"
#include "hal_sleep_manager.h"
#include "hal_pmu_internal.h"
#include "hal_dsp_topsm.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_sleep_driver.h"
#include "hal_nvic_internal.h"
#include "hal_pmu.h"
#include "memory_attribute.h"
#include "hal_feature_config.h"
#include <ctype.h>
#include "hal_nvic.h"
#include "hal_dvfs.h"
#include "timers.h"

#ifdef MTK_SMART_BATTERY_ENABLE
#include "battery_management.h"
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;
extern uint8_t sleep_manager_handle;

#ifdef MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
#include "mt25x3_hdk_backlight.h"
#include "mt25x3_hdk_lcd.h"
#include "task_def.h"
#endif

char *end_pos = NULL;
int input_value = 0;
int input_1 = 0;
int input_2 = 0;
char *mid_pos = NULL;
char str[20] = { 0 };

extern void mtcmos_bt(void);
extern void mtcmos_mm(void);
extern void mtcmos_md2g(void);

ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;

extern uint8_t sleep_manager_handle;
#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATCMD: "fmt,##arg)


static TimerHandle_t xTimerofTest = NULL;

void at_power_off_task(TimerHandle_t pxTimer){
hal_sleep_manager_enter_power_off_mode();
}

void at_force_to_sleep(TimerHandle_t pxTimer){
    extern sleep_driver_struct sleepdrv;
    sleepdrv.sleep_lock_index=0;
}

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);

int sm_htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')){
        i = 2;
    }else{
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i){
        if (tolower((unsigned char)s[i]) > '9'){
            n = 16 * n + (10 + tolower((unsigned char)s[i]) - 'a');
        }else{
            n = 16 * n + (tolower((unsigned char)s[i]) - '0');
        }
    }
    return n;
}

atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    response.response_flag = 0;
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif

    if(strstr((char *) parse_cmd->string_ptr, "AT+SM=0") != NULL){
        strcpy((char *)response.response_buf, "Test Command Pass!!\r\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=MTCMOS") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        LOGW("[MTCMOS : %d %d ]",input_1,input_2);
        if (input_1 > 2 || input_2 > 1) {
            LOGW("[Error Conmand]\n");
            return ATCI_STATUS_OK;
        }
        sys_topsm_mtcmos_control((hal_sys_topsm_mtcmos_enum_t) input_1,input_2);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=SLEEP") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        switch (input_1) {
            case 1:
                if (input_2 == 1) {
                    sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
                    cm4_topsm_debug_option(true);
                } else if (input_2 == 0) {
                    cm4_topsm_debug_option(false);
                } else {
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&response);
                    return ATCI_STATUS_ERROR;
                }
                break;
            case 3:
                sleepdrv_get_lock_module();
                break;
            case 7:
                if (input_2 == 0) {
                    LOGW("[BT MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_BT_MTCMOS();
                } else if (input_2 == 1) {
                    LOGW("[MM MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MM_MTCMOS();
                } else if (input_2 == 2) {
                    LOGW("[MD2G MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MD2G_MTCMOS();
                } else if (input_2 == 3) {
                    LOGW("[BT MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_BT_MTCMOS();
                } else if (input_2 == 4) {
                    LOGW("[MM MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MM_MTCMOS();
                } else if (input_2 == 5) {
                    LOGW("[MD2G MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MD2G_MTCMOS();
                }
                break;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,0") != NULL) {
            LOGW("[Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_4, HAL_EINT_NUMBER_3);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,1") != NULL) {
            LOGW("[EVB Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_3,HAL_EINT_NUMBER_14);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,3") != NULL) {
            LOGW("[Fast_wakeup]\n");
            FAST_WAKEUP_VERIFICATION = true;
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_L);
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_H);
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(sleep_manager_handle);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,4") != NULL) {
            LOGW("[Normail_wakeup] \n");
            FAST_WAKEUP_VERIFICATION = true;
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(sleep_manager_handle);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,5") != NULL) {
            FAST_WAKEUP_VERIFICATION = true;
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,6") != NULL) {
            xTimerofTest = xTimerCreate("TimerofTest", /* Just a text name, not used by the kernel. */
            (5 * 1000 / portTICK_PERIOD_MS), /* The timer period in ticks. */
            pdTRUE, /* The timers will auto-reload themselves when they expire. */
            NULL, /* Assign each timer a unique id equal to its array index. */
            at_force_to_sleep /* Each timer calls the same callback when it expires. */
            );
            xTimerStart(xTimerofTest, 0);

        } else {
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=3") != NULL) {
        sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4") != NULL) {
        /*Sleep 15s*/
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4,0") != NULL) {
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            __asm volatile("cpsid i");
            hal_sleep_manager_set_sleep_time(500000);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
            __asm volatile("cpsie i");
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4,1") != NULL) {
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            __asm volatile("cpsid i");
            hal_sleep_manager_set_sleep_time(5000);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
            __asm volatile("cpsie i");
        } else {
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    }  else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=5") != NULL) {
        /*Power off*/
        xTimerofTest = xTimerCreate("TimerofTest",       /* Just a text name, not used by the kernel. */
                                       (3 *1000 / portTICK_PERIOD_MS),    /* The timer period in ticks. */
                                       pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                       NULL,   /* Assign each timer a unique id equal to its array index. */
                                       at_power_off_task /* Each timer calls the same callback when it expires. */
                                      );
        xTimerStart(xTimerofTest, 0);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,0") != NULL) {
            hal_sleep_manager_lock_sleep(sleep_manager_handle);
        }
        else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,1") != NULL) {
            hal_sleep_manager_unlock_sleep(sleep_manager_handle);
        }
        else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,2") != NULL) {
#ifdef MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
            bsp_lcd_display_off();
            bsp_backlight_deinit();
#endif
            task_def_delete_wo_curr_task();
#endif
        }else{
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,0") != NULL) {
            mtcmos_bt();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,1") != NULL) {
            mtcmos_mm();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,2") != NULL) {
            mtcmos_md2g();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,3") != NULL) {
#ifdef MTK_SMART_BATTERY_ENABLE
            battery_management_deinit();
#endif
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,4") != NULL) {
            PMIC_VR_CONTROL(PMIC_VA28,PMIC_VR_CTL_DISABLE);
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,5") != NULL) {
            uint8_t A_sleep_manager_handle;
            A_sleep_manager_handle = hal_sleep_manager_set_sleep_handle("A_sleep_manager_handle_array");
            hal_sleep_manager_lock_sleep(A_sleep_manager_handle);
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,6") != NULL) {
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,7") != NULL) {
            sleepdrv_get_lock_module();
        }else {
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else {
        strcpy((char *) response.response_buf, "Not Support\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&response);
        return ATCI_STATUS_ERROR;
    }
    return ATCI_STATUS_OK;
}
#elif (PRODUCT_VERSION == 2625)
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "at_command.h"
#include "syslog.h"

#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"
#include "n1_md_sleep_api.h"
#include "hal_spm.h"
#include "hal_pmu.h"
#include "nvdm.h"
#include "hal_rtc.h"
#include "hal_rtc_external.h"
#include "hal_clock_internal.h"
#include <stdlib.h>
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif

static uint8_t host_ds_lock = 0xff;
static uint8_t host_lock = 0xff;

extern void os_gpt0_pause(void);
extern void os_gpt0_resume(bool update, uint32_t new_compare);
extern void tickless_system_status(void);

extern uint8_t sys_lock_handle;

#define RTC_WAKEUP_DEEP_SLEEP   0

#if RTC_WAKEUP_DEEP_SLEEP
static uint8_t param[7] = {15, /*year*/
                           11, /*month*/
                           06, /*day*/
                           5,  /*day of week*/
                           15, /*hour*/
                           26, /*minute*/
                           15  /*second*/
                          };

static uint8_t alarm_param[7] = {15, /*year*/
                                 11, /*month*/
                                 06, /*day*/
                                 5,  /*day of week*/
                                 15, /*hour*/
                                 26, /*minute*/
                                 20  /*second*/
                                };

/**
*@brief  Clear RTC time structure.
*@param[in] rtc_time: pointer to a hal_rtc_time_t structure that contains the data and time setting that will be set to 0.
*@return none.
*/
static void rtc_clear_time(hal_rtc_time_t *rtc_time)
{
    rtc_time->rtc_year = 0;
    rtc_time->rtc_mon = 0;
    rtc_time->rtc_day = 0;
    rtc_time->rtc_week = 0;
    rtc_time->rtc_hour = 0;
    rtc_time->rtc_min = 0;
    rtc_time->rtc_sec = 0;
}

/**
*@brief  In this function, we fill in RTC time structure by passing in parameters.
*@param[in] rtc_time: pointer to a hal_rtc_time_t structure that contains the data and time setting that will be set.
*@param[in] param[]: the data that will pass to rtc_time setting.
*@return None.
*/

static void rtc_fill_time(hal_rtc_time_t *rtc_time, uint8_t param[])
{
    rtc_time->rtc_year = param[0];
    rtc_time->rtc_mon = param[1];
    rtc_time->rtc_day = param[2];
    rtc_time->rtc_week = param[3];
    rtc_time->rtc_hour = param[4];
    rtc_time->rtc_min = param[5];
    rtc_time->rtc_sec = param[6];
}
#endif

void sm_set_register_value(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value)
{
    uint32_t mask_buffer,target_value;
    mask_buffer = (~(mask << shift));
    target_value = *((volatile uint32_t *)(address));
    target_value &= mask_buffer;
    target_value |= (value << shift);
    *((volatile uint32_t *)(address)) = target_value;
}

atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    response.response_flag = 0;
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif
    uint32_t result = 0;
    uint32_t sys_default_lock = false;
    char cmd[256] = {0};
    uint8_t  i = 0;

    strncpy(cmd, (char *)parse_cmd->string_ptr, sizeof(cmd));
    for (i = 0; i < strlen((char *)parse_cmd->string_ptr); i++) {
        cmd[i] = (char)toupper((unsigned char)cmd[i]);
    }

    if (strstr(cmd, "AT+SM=0") != NULL) {
        strcpy((char *)response.response_buf, "Test Command Pass!!\r\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=SHIP") != NULL) {
        __asm volatile("cpsid i");
        spm_control_mtcmos(SPM_MTCMOS_SDIO_SLV,SPM_MTCMOS_PWR_DISABLE);
        spm_control_mtcmos(SPM_MTCMOS_MDSYS,SPM_MTCMOS_PWR_DISABLE);
        pmu_enter_ship_mode();
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=LS") != NULL) {
#ifdef MTK_NBIOT_TARGET_BUILD
        N1MdSleepPowerMdsysPDOff();
#endif
        if (strstr(cmd, "AT+SM=LS,1") != NULL) {
        } else if (strstr(cmd, "AT+SM=LS,2") != NULL) {
            sm_set_register_value(0xA2110250, 0x1, 0, 1);
        }

        __asm volatile("cpsid i");
        os_gpt0_pause();
        spm_control_mtcmos(SPM_MTCMOS_MDSYS, SPM_MTCMOS_PWR_DISABLE);
        spm_control_mtcmos(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_DISABLE);
        pmu_ctrl_power(PMU_VPA, PMU_CTL_DISABLE);
        hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_LIGHT_SLEEP);

        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=DS") != NULL) {
#if RTC_WAKEUP_DEEP_SLEEP
        hal_rtc_time_t rtc_time;
        hal_rtc_time_t alarm_time;

        rtc_fill_time(&rtc_time, param);
        rtc_fill_time(&alarm_time, alarm_param);
        hal_rtc_set_time(&rtc_time);
	hal_rtc_set_alarm(&alarm_time);
        hal_rtc_enable_alarm();
        //wait milli_sec is 0
        while(1) {
            hal_rtc_get_time(&rtc_time);
            if (rtc_time.rtc_sec == 16)
                break;
        }
#endif

        if (strstr(cmd, "AT+SM=DS,1") != NULL) {
            hal_rtc_retention_sram_config(0xf, HAL_RTC_SRAM_PD_MODE);
            // need to modify hal_rtc.c if you want to see LDO on current.
            // mask this line: rtc_set_register_value(0xa207080c, 0x1, 0, 0);
        } else if (strstr(cmd, "AT+SM=DS,2") != NULL) {
            hal_rtc_retention_sram_config(0xc, HAL_RTC_SRAM_PD_MODE);
            hal_rtc_retention_sram_config(0x3, HAL_RTC_SRAM_SLEEP_MODE);
        } else if(strstr(cmd, "AT+SM=DS,3") != NULL) {
            hal_rtc_retention_sram_config(0xf, HAL_RTC_SRAM_SLEEP_MODE);
        }

        __asm volatile("cpsid i");
        hal_rtc_enter_retention_mode();
//        hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
        __asm volatile("cpsie i");
        strcpy((char *)response.response_buf, "Test Command Pass!!\r\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=DP") != NULL) {
        __asm volatile("cpsid i");
        hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEPER_SLEEP);
        __asm volatile("cpsie i");
        strcpy((char *)response.response_buf, "Test Command Pass!!\r\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=HQA") != NULL) {
#ifdef MTK_NBIOT_TARGET_BUILD
        N1MdSleepPowerMdsysPDOff();
#endif
        __asm volatile("cpsid i");
        os_gpt0_pause();
        //hal_sleep_manager_set_sleep_time(30000000);
        if (strstr(cmd, "AT+SM=HQA,1") != NULL) {
            spm_control_mtcmos(SPM_MTCMOS_SDIO_SLV, SPM_MTCMOS_PWR_DISABLE);
            pmu_ctrl_power(PMU_VPA, PMU_CTL_DISABLE);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_LIGHT_SLEEP);
        } else if (strstr(cmd, "AT+SM=HQA,2") != NULL) {
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);
        } else if (strstr(cmd, "AT+SM=HQA,3") != NULL) {
            hal_clock_set_switch_to_0P9V();
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);
        }
        os_gpt0_resume(false, 0);
        __asm volatile("cpsie i");
        strcpy((char *)response.response_buf, "Test Command Pass!!\r\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=BOOT") != NULL) {
        result = rtc_power_on_result_external();
        if (result == 0) {
            strcpy((char *)response.response_buf, "BOOT 0!!\r\n");
        } else if (result == 1) {
           strcpy((char *)response.response_buf, "BOOT 1!!\r\n");
        } else if (result == 1) {
           strcpy((char *)response.response_buf, "BOOT 2!!\r\n");
        }
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=H_DS_L") != NULL) {
        if (host_ds_lock == 0xFF) {
            host_ds_lock = hal_sleep_manager_set_sleep_handle("host_ds_lock");
        }
        hal_sleep_manager_acquire_sleeplock(host_ds_lock, HAL_SLEEP_LOCK_DEEP);

        response.response_len = 0;
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=H_DS_U") != NULL) {
        if (host_ds_lock == 0xFF) {
            host_ds_lock = hal_sleep_manager_set_sleep_handle("host_ds_lock");
        }
        hal_sleep_manager_release_sleeplock(host_ds_lock, HAL_SLEEP_LOCK_DEEP);

        response.response_len = 0;
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=H_L") != NULL) {
        if (host_lock == 0xFF) {
            host_lock = hal_sleep_manager_set_sleep_handle("host_lock");
        }
        hal_sleep_manager_acquire_sleeplock(host_lock, HAL_SLEEP_LOCK_ALL);

        response.response_len = 0;
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=H_U") != NULL) {
        if (host_lock == 0xFF) {
            host_lock = hal_sleep_manager_set_sleep_handle("host_lock");
        }
        hal_sleep_manager_release_sleeplock(host_lock, HAL_SLEEP_LOCK_ALL);

        response.response_len = 0;
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=D_STA") != NULL) {
        uint32_t lock_low;
        uint32_t lock_high;
        sleep_management_get_lock_sleep_info(HAL_SLEEP_LOCK_DEEP, &lock_high, &lock_low);
        snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "Lock status 0x%x-%x\r\n", (unsigned int)lock_high, (unsigned int)lock_low);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=L_STA") != NULL) {
        uint32_t lock_low;
        uint32_t lock_high;
        sleep_management_get_lock_sleep_info(HAL_SLEEP_LOCK_ALL, &lock_high, &lock_low);
        snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "Lock status 0x%x-%x\r\n", (unsigned int)lock_high, (unsigned int)lock_low);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=CG_V") != NULL) {
        snprintf((char *)response.response_buf, 1024,"\r\n [PDN_COND0]:%lx\r\n [XO_PDN_COND0]:%lx\r\n [0xC0002008]:%lx\r\n",*((volatile uint32_t*) (0xA21D0300)),*((volatile uint32_t*) (0xA2030B00)),*((volatile uint32_t*) (0xC0002008)));
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
             atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=ENABLE_VSIM") != NULL) {
        pmu_ctrl_power(PMU_VSIM,PMU_CTL_ENABLE);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=DISABLE_VSIM") != NULL) {
        pmu_ctrl_power(PMU_VSIM,PMU_CTL_DISABLE);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=ENABLE_VFEM") != NULL) {
        pmu_ctrl_power(PMU_VFEM,PMU_CTL_DISABLE);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=DISABLE_VFEM") != NULL) {
        pmu_ctrl_power(PMU_VFEM,PMU_CTL_DISABLE);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
#ifdef MTK_SWLA_ENABLE
    } else if (strstr(cmd, "AT+SM=SWLAS") != NULL) {
        SLA_CustomLogging("SMG", SA_START);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=SWLAE") != NULL) {
        SLA_CustomLogging("SMG", SA_STOP);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
#endif
    } else if (strstr(cmd, "AT+SM=CLOSE_UART0") != NULL) {
        hal_uart_deinit(0);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=CLOSE_UART1") != NULL) {
        hal_uart_deinit(1);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=CLOSE_UART2") != NULL) {
        hal_uart_deinit(2);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=CLOSE_UART3") != NULL) {
        hal_uart_deinit(3);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=CLOSE_UART") != NULL) {
        hal_uart_deinit(0);
        hal_uart_deinit(1);
        hal_uart_deinit(2);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=LOCK") != NULL) {
        sys_default_lock = true;
        uint32_t lock_low;
        uint32_t lock_high;
        nvdm_write_data_item("sleep_manager", "sys_default_lock", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&sys_default_lock, sizeof(sys_default_lock));
        hal_sleep_manager_get_sleep_lock_status(HAL_SLEEP_LOCK_ALL, &lock_high, &lock_low);
        if (sys_lock_handle < 32) {
            if (!(lock_low & (1 << sys_lock_handle))) {
                hal_sleep_manager_acquire_sleeplock(sys_lock_handle, HAL_SLEEP_LOCK_ALL);
            }
        } else {
            if (!(lock_high & (1 << (sys_lock_handle-32)))) {
                hal_sleep_manager_acquire_sleeplock(sys_lock_handle, HAL_SLEEP_LOCK_ALL);
            }
        }
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr(cmd, "AT+SM=UNLOCK") != NULL) {
        sys_default_lock = false;
        nvdm_write_data_item("sleep_manager", "sys_default_lock", NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&sys_default_lock, sizeof(sys_default_lock));
        hal_sleep_manager_release_sleeplock(sys_lock_handle, HAL_SLEEP_LOCK_ALL);
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
#if configUSE_TICKLESS_IDLE == 2
    } else if (strstr(cmd, "AT+SM=STATUS") != NULL) {
        tickless_system_status();
        printf("CLOCK 0x%x\r\n", *((volatile unsigned int *)0xA21D0300));
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
#endif
    } else {
        strcpy((char *) response.response_buf, "Not Support\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&response);
        return ATCI_STATUS_ERROR;
    }

    return ATCI_STATUS_OK;
}
#endif
