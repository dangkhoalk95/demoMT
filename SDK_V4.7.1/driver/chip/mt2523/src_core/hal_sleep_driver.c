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

#include "hal_sleep_manager.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_driver.h"
#include "hal_sys_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "hal_log.h"
#include "hal_cm4_topsm.h"
#include "hal_gpt.h"
#include "hal_eint.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "string.h"
#include "hal_gpio.h"
#include "stdio.h"
#include "hal_dvfs.h"
#include "stdlib.h"
sleep_driver_struct sleepdrv={.handleCount =0,
                              .sleep_lock_index=0,
                              .sleep_set_handle_index=0};
static int SDMutex = 1; //hal layer can not use OS mutex.
extern cm4_topsm_mtcmos_control mtcmos_control;
extern uint32_t SaveAndSetIRQMask(void);
extern void RestoreIRQMask(uint32_t x);
uint32_t sleep_driver_handle_array[MAX_SLEEP_HANDLE];
uint32_t sleep_driver_handle_ref_count[MAX_SLEEP_HANDLE];
uint32_t sleep_handle_total_lock_sleep_time[MAX_SLEEP_HANDLE]; //fix
uint32_t sleep_handle_total_lock_sleep_count[MAX_SLEEP_HANDLE];//fix
hal_gpio_pin_t GPIO_INDEX;
hal_eint_number_t EINT_INDEX;
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;

void fast_wakeup_eint_handler(void *parameter)
{
    hal_eint_unmask(EINT_INDEX);
    log_hal_info("[Eint Wakeup]\n");
}
void sleep_driver_fast_wakeup_eint_setting(hal_gpio_pin_t gpio_index, hal_eint_number_t eint_index)
{
    GPIO_INDEX = gpio_index;
    EINT_INDEX = eint_index;
    hal_eint_config_t config;
    uint32_t status ;
    status = hal_gpio_init(GPIO_INDEX);
    if(status == 0){
        hal_pinmux_set_function(GPIO_INDEX, 1);
        config.trigger_mode = HAL_EINT_EDGE_FALLING;
        config.debounce_time = 0;

        hal_eint_init(EINT_INDEX, &config);    //set EINT trigger mode and debounce time.
        hal_eint_register_callback(EINT_INDEX, fast_wakeup_eint_handler, NULL); // register a user callback.
        hal_eint_unmask(EINT_INDEX);
    }else{
        log_hal_info("[EINT Setting Error]\n");
    }
}

void sleep_driver_init()
{
    sleepdrv.handleInit=true;
}

bool sleepdrv_get_mutex(void)
{
    int32_t savedMask;
    savedMask = SaveAndSetIRQMask();
    if (SDMutex == 1) {
        SDMutex--;
        RestoreIRQMask(savedMask);
        return true;
    } else if (SDMutex == 0) {
        RestoreIRQMask(savedMask);
        return false;
    } else {
        RestoreIRQMask(savedMask);
    }
    return false;
}

void sleepdrv_release_mutex(void)
{
    int32_t savedMask;
    savedMask = SaveAndSetIRQMask();
    if (SDMutex == 0) {
        SDMutex++;
        RestoreIRQMask(savedMask);
    } else {
        RestoreIRQMask(savedMask);
    }
}

uint8_t sleepdrv_get_handle(const char *handle_name, uint8_t size)
{
    int n;
    char sleep_handle_name[30];
    uint8_t handle_index = 0;
    memset(sleep_handle_name, 0, size + 1);
    memcpy(&sleep_handle_name, handle_name, size);
    for (n = 0; n < MAX_SLEEP_HANDLE; n++) {
        if (sleepdrv.sleep_set_handle_index & (1 << n)) {
            char *name = (char *)sleep_driver_handle_array[n];
            if ((strcmp(sleep_handle_name, name) == 0)) {
                log_hal_info("Error!! Sleep handle duplicate naming , return original index\r\n");
                handle_index = n;
            }
        }
    }
    if (handle_index == 0) {
        for (handle_index = 0; handle_index < MAX_SLEEP_HANDLE; handle_index++) {
            if (sleep_driver_handle_array[handle_index] == 0) {
                sleepdrv.sleep_set_handle_index |= (1 << handle_index);
                sleepdrv.handleCount++;
                sleep_driver_handle_array[handle_index] = (uint32_t) handle_name;
                break;
            }
        }
    }
    if (handle_index >= MAX_SLEEP_HANDLE) {
        log_hal_info("Fatal error, cannot get sleep handle\n");
        handle_index = INVALID_SLEEP_HANDLE;
    }
    return handle_index;
}

void sleepdrv_release_handle(uint8_t handle)
{
    uint32_t release_t=0;
    sleepdrv_get_mutex();
    sleepdrv.handleCount--;
    sleep_driver_handle_array[handle] = 0;
    release_t |= (1 << handle);
    sleepdrv.sleep_set_handle_index &= ~(1 << handle);
    sleepdrv_release_mutex();
}

void sleepdrv_release_sleep_lock(uint8_t handle_index)
{
    if (handle_index < MAX_SLEEP_HANDLE) {
        uint32_t _savedMask;
        _savedMask = SaveAndSetIRQMask();
        if (sleep_driver_handle_ref_count[handle_index] != 0) {
            sleep_driver_handle_ref_count[handle_index]--;
            if (sleep_driver_handle_ref_count[handle_index] == 0) {
                sleepdrv.sleep_lock_index &= ~(1 << handle_index);
                sleep_driver_debug_lock_sleep_timelog(false, handle_index);
            }
        } else {
            log_hal_info("lock has already released[index:%d][Name:%s]\n",handle_index,(char *)sleep_driver_handle_array[(int)handle_index]);
        }
        RestoreIRQMask(_savedMask);
    }
}

void sleepdrv_hold_sleep_lock(uint8_t handle_index)
{
    if(handle_index >= MAX_SLEEP_HANDLE){
        log_hal_error("[Sleep Management]sleep handle index error\r\n");
        return;
    }
    if (handle_index < MAX_SLEEP_HANDLE) {
        uint32_t _savedMask;
        _savedMask = SaveAndSetIRQMask();
        sleep_driver_handle_ref_count[handle_index]++;
        sleepdrv.sleep_lock_index |= (1 << handle_index);
        sleep_driver_debug_lock_sleep_timelog(true, handle_index);
        RestoreIRQMask(_savedMask);
    }
}

bool sleepdrv_get_handle_status(uint8_t handle)
{
    if (sleepdrv.sleep_set_handle_index & (1 << handle)) {
        return true;
    } else {
        return false;
    }
}

bool sleepdrv_check_sleep_locks()
{
    uint32_t _savedMask;
    bool lock;
    _savedMask = SaveAndSetIRQMask();
    lock = sleepdrv.sleep_lock_index != 0 ? true : false;
    RestoreIRQMask(_savedMask);
    return lock;
}

uint32_t sleepdrv_get_lock_index(void)
{
    return sleepdrv.sleep_lock_index;
}

uint32_t sleepdrv_get_lock_module(void)
{
    int i;
    for (i = 0; i < MAX_SLEEP_HANDLE; i++) {
        if (sleepdrv.sleep_lock_index & (1 << i)) {
            printf("Lock index:[%d]: %s\r\n", i,(char *)sleep_driver_handle_array[i]);
        }
    }
    return sleepdrv.sleep_lock_index;
}

void sleep_driver_mtcmos_control(hal_sys_topsm_mtcmos_enum_t mtcmos, bool config)
{
    sys_topsm_mtcmos_control(mtcmos, config);
}

/*
 * Debug Function
 * */
void sleep_driver_debug_dump_lock_sleep_time(void)
{
    uint32_t i;
    float lock_time;
    printf("\r\n[===dump lock sleep time===]\r\n");
    for (i = 0; i < MAX_SLEEP_HANDLE; i++) {
        if (sleep_handle_total_lock_sleep_count[i] > 0) {
            printf("sleep handle=[%d],[%s],[Times:%d]\r\n", (int)i, (char *)sleep_driver_handle_array[i],(int)sleep_handle_total_lock_sleep_count[i]);
            lock_time = ((float)sleep_handle_total_lock_sleep_time[i]);
            lock_time = (lock_time) / (float)CLOCK_SOURCE_32K_FREQ;
            if(lock_time < 1) {
                printf("total lock time : %d us,%d\r\n"   ,(int)lock_time*1000,(int)sleep_handle_total_lock_sleep_time[i]);
            } else if(lock_time >= 1){
                printf("total lock time : %d ms,%d\r\n"   ,(int)lock_time,(int)sleep_handle_total_lock_sleep_time[i]);
            }
            printf("\r\n");
        }
    }
}

void sleep_driver_debug_lock_sleep_timelog(bool lock, uint8_t handle_index)
{
    static uint32_t lock_sleep_time[MAX_SLEEP_HANDLE], unlock_sleep_time;
    if (lock == true) {
        if (sleep_handle_total_lock_sleep_count[handle_index] == 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &lock_sleep_time[handle_index]);
        }
        sleep_handle_total_lock_sleep_count[handle_index]++;
    } else {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &unlock_sleep_time);

        if (unlock_sleep_time >= lock_sleep_time[handle_index]) {
            sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time - lock_sleep_time[handle_index];
        } else {
            sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time + (0xFFFFFFFF - lock_sleep_time[handle_index]);
        }
    }
}

/*
 * PSI Change Mode
 * */
void PSI_S0_Run_AT_LP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xC0000000; // [27]=0 turn off sw_sta_hld. [31][30]=3 ,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON1 = (*PSI_MST_CON1 & 0xFE00FFFF) | (PSI_S0_LP_SETTLE_TIME << 16);  // [24:16] : s0_lp_settle
    if (is_clk_use_lfosc()) {
        *PSI_MST_CON1 = (*PSI_MST_CON1 & 0xFE00FFFF) | (1 << 16);  // fast wakeup
    }
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 & 0x69; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1; //[0]=1, enable
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF;
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0x9; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[0]=1, enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S1 & 0x9; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S2 = *PSI_MST_S1 | 0x1; //[0]=1, enable
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0x9; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[0]=1, enable
            break;
    }
}

void PSI_S0_RUN_AT_HP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON1 = (*PSI_MST_CON1 & 0xFFFFFE00) | PSI_S0_HP_SETTLE_TIME; // [8:0] s0_hp_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x4;  //[2]=1 , enable hp_sel
            *PSI_MST_S0 = *PSI_MST_S0 & 0x6D; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1 , enable
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF;
            break;
        case 0x1:
            *PSI_MST_S1 = *PSI_MST_S1 | 0x4; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[1], mask:idle

            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable

            *PSI_MST_S2 = *PSI_MST_S2 | 0x4; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1; //[1], mask:idle
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;  //[0]=1 , enable

            *PSI_MST_S4 = *PSI_MST_S4 | 0x4; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[1], mask:idle

            break;
    }
}

void PSI_S1_RUN_AT_LP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON2 = (*PSI_MST_CON2 & 0xFE00FFFF) | PSI_S1_LP_SETTLE_TIME << 16; // [8:0] s1_hp_settle
    //*PSI_MST_CON2 = 0x01;
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;

            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40;
            *PSI_MST_S0 = *PSI_MST_S0 & 0x69;
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF; // [6] = 1 idle, [5]=1 SW enable
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;

            *PSI_MST_S1 = *PSI_MST_S1 & 0x9;
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1;
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2;
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;

            *PSI_MST_S2 = *PSI_MST_S2 & 0x9;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1;
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2;
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;

            *PSI_MST_S4 = *PSI_MST_S4 & 0x9;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1;
            break;
    }
}

void PSI_S1_RUN_AT_HP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xC0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON2 = (*PSI_MST_CON2 & 0xFFFFFE00) | PSI_S1_HP_SETTLE_TIME; // [8:0] s1_hp_settle
    //*PSI_MST_CON2 = 0x01;
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x4;  //[2]=1 , enable hp_sel
            *PSI_MST_S0 = *PSI_MST_S0 & 0x6D; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1 , enable
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable

            *PSI_MST_S1 = *PSI_MST_S1 | 0x400000004; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[1], mask:idle

            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable

            *PSI_MST_S2 = *PSI_MST_S2 | 0x400000004; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1; //[1], mask:idle

            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;  //[0]=1 , enable
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle

            *PSI_MST_S4 = *PSI_MST_S4 | 0x400000004; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[1], mask:idle

            break;
    }
}

void PSI_HP_RUN_AT_LP(const uint32_t master_id)
{
//  //log_hal_info("PSI_HP_RUN_AT_LP start\n");
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0xFFFF00FF) | (PSI_HP_LP_SETTLE_TIME << 8);  // [15:8] hp_lp_settle

    switch (master_id) {
        case 0x0:
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S0 = *PSI_MST_S0 & 0x69; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1 , enable
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0x9; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1;  //[0]=1 , enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0x9; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1;  //[0]=1 , enable
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0x9; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1;  //[0]=1 , enable
            break;

    }
}

void PSI_LP_RUN_AT_HP(const uint32_t master_id)
{
//  //log_hal_info("PSI_LP_RUN_AT_HP start\n");
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0xFFFFFF00) | PSI_LP_HP_SETTLE_TIME; // [24:16] s1_lp_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S0 = *PSI_MST_S0 & 0xD; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1, enable
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1;  //[0]=1, enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[5]=1 SW
            *PSI_MST_S2 = *PSI_MST_S2 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1;  //[0]=1, enable
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[5]=1 SW
            *PSI_MST_S4 = *PSI_MST_S4 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1;  //[0]=1, enable
            break;
    }
}

void PSI_SLEEP_AT_S0(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON3 = (*PSI_MST_CON3 & 0xFF00FFFF) | (PSI_T0_S0_SETTLE_TIME << 16); // [24:16] to_s0_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x20; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 & 0x6D; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x9;  //[0]=1, enable

            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            }

            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[1], mask:idle
            if (is_clk_use_lfosc()) {
                *PSI_MST_S1 = *PSI_MST_S1 | 0x10; //[1], mask:idle //fast wakeup
            }
            *PSI_MST_S1 = *PSI_MST_S1 | 0x9; //[1], mask:idle
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            }
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x9; //[1], mask:idle
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x9; //[1], mask:idle
            break;
    }
}

void PSI_SLEEP_AT_S1(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON3 = (*PSI_MST_CON3 & 0xFFFFFF00) | PSI_T0_S1_SETTLE_TIME; // [24:16] to_s0_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x20; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 & 0x65; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1, enable

            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            }
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0x5; //[1], mask:idle
            if (is_clk_use_lfosc()) {
                *PSI_MST_S1 = *PSI_MST_S1 | 0x10; //[1], mask:idle //fast wakeup
            }
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[1], mask:idle
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            }
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0x5; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1; //[1], mask:idle
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0x5; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[1], mask:idle
            break;
    }
}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
