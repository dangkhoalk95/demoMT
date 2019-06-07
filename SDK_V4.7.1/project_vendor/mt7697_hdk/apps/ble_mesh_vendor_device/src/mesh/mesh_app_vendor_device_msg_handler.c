/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#include <stdio.h>
#include <math.h>

#include "bt_mesh.h"
#include "bt_mesh_debug.h"
#include "bt_mesh_flash.h"

#include "bt_mesh_model_generic_opcodes.h"
#include "bt_mesh_model_generic_server.h"
#include "bt_mesh_model_lighting_opcodes.h"
#include "bt_mesh_model_lighting_server.h"

#include "mesh_app_util.h"
#include "mesh_app_vendor_device_def.h"
#include "mesh_app_vendor_device_msg_handler.h"
#include "mesh_app_vendor_device_pwm.h"

static void _bind_generic_on_off(lighting_lightness_server_t *lightness_server,
                                 uint16_t state, uint8_t binding);
static void _bind_generic_level(lighting_lightness_server_t *lightness_server,
                                uint16_t state, uint8_t binding);
static void _bind_lightness_actual(lighting_lightness_server_t *lightness_server,
                                   uint16_t state, uint8_t binding);
static void _bind_lightness_linear(lighting_lightness_server_t *lightness_server,
                                   uint16_t state, uint8_t binding);
static void _bind_lightness_range(lighting_lightness_server_t *lightness_server,
                                  uint16_t state, uint8_t binding);
static void _bind_ctl_lightness(lighting_ctl_server_t *ctl_server,
                                uint16_t state, uint8_t binding);
static void _bind_hsl_lightness(lighting_hsl_server_t *hsl_server,
                                uint16_t state, uint8_t binding);


typedef enum {
    LED_1_HSL, /* for HSL control */
    LED_2_CTL, /* for CTL control */
} vendordevice_led_type_t;
/******************************************************************************/
/* global variables                                                           */
/******************************************************************************/
lighting_ctl_server_t *gCTL_server = NULL;
lighting_ctl_temperature_server_t *gCTL_temperature_server = NULL;
lighting_hsl_server_t *gHSL_server = NULL;
lighting_hsl_hue_server_t *gHSL_hue_server = NULL;
lighting_hsl_saturation_server_t *gHSL_saturation_server = NULL;

uint32_t gTransitionTick;
uint32_t gCTL_transition_tick;
uint32_t gHSL_transition_tick;
/******************************************************************************/
/* variables                                                                  */
/******************************************************************************/
static float hue_2_rgb(float v1, float v2, float vH)
{
    float a, b;
    if ( vH < 0 ) {
        vH += 1;
    }
    if ( vH > 1 ) {
        vH -= 1;
    }
    if (( 6 * vH ) < 1 ) {
        return ( v1 + ( v2 - v1 ) * 6 * vH );
    }
    if (( 2 * vH ) < 1 ) {
        return ( v2 );
    }
    if (( 3 * vH ) < 2 ) {
        a = v2 - v1;
        //b = 2/3 -vH;
        b = (float)0.667 - vH;
        //return ( v1 + ( v2 - v1 ) * ( ( 2/3 ) - vH ) * 6 );
        return ( v1 + a * b * 6 );
    }
    return ( v1 );
}

static void led_set_hsl(uint16_t hue, uint16_t saturation, uint16_t ligntness)
{
    uint16_t R_PWM, G_PWM, B_PWM;
    float H, S, L, R, G, B;
    float var_1, var_2;

    H = (float)hue / 65535;
    S = (float)saturation / 65535;
    L = (float)ligntness / 65535;

    LOG_I(mesh_app, "led_set_hsl H:%f,S:%f,L:%f\n", (double)H, (double)S, (double)L);

    if ( S == 0) {
        R = L;
        G = L;
        B = L;
    } else {
        if ( L < (float)0.5 ) {
            var_2 = L * ( 1 + S );
        } else {
            var_2 = ( L + S ) - ( S * L );
        }

        var_1 = 2 * L - var_2;
        LOG_I(mesh_app, "var_1:%f,var_2:%f\n", (double)var_1, (double)var_2);
        /*
        R = hue_2_rgb( var_1, var_2, (H + (1/3)));
        G = hue_2_rgb( var_1, var_2, H );
        B = hue_2_rgb( var_1, var_2, (H - (1/3)));
        */
        float vvh;
        //vvh = H + 1/3;
        vvh = H + (float)0.333;
        R = hue_2_rgb(var_1, var_2, vvh);
        vvh = H ;
        G = hue_2_rgb(var_1, var_2, vvh);
        //vvh = H -1/3;
        vvh = H - (float)0.333;
        B = hue_2_rgb(var_1, var_2, vvh);
    }
    LOG_I(mesh_app, "R:%f,G:%f,B:%f\n", (double)R, (double)G, (double)B);

    R_PWM = (uint16_t)(R * 255);
    G_PWM = (uint16_t)(G * 255);
    B_PWM = (uint16_t)(B * 255);

    LOG_I(mesh_app, "R_PWM:%d,G_PWM:%d,B_PWM:%d\n", R_PWM, G_PWM, B_PWM);
#if 0
    DRV_TMR16_Disable();
    DRV_TMR16_Reset(true);
    DRV_TMR16_Reset(false);
#endif

    if (R_PWM == 0) {
        MY_DRV_TMR16_PWM1_Set_Period_And_Set_Off(255 - R_PWM, 255);
    } else {
        MY_DRV_TMR16_PWM1_Set_Period(255 - R_PWM, 255);
    }

    if (B_PWM == 0) {
        MY_DRV_TMR16_PWM2_Set_Off(255 - B_PWM, 255);
    } else {
        MY_DRV_TMR16_PWM2(255 - B_PWM, 255);
    }

    if (G_PWM == 0) {
        MY_DRV_TMR16_PWM3_Set_Off(255 - G_PWM, 255);
    } else {
        MY_DRV_TMR16_PWM3(255 - G_PWM, 255);
    }
#if 0
    DRV_TMR16_PWM_Enable( 15 );
#endif
}

static void led_set_onoff(bool on)
{
    LOG_I(mesh_app, "led_set_onoff %d\n", on);
    if (!on) {
#if 0
        DRV_TMR16_Disable();
        DRV_TMR16_Reset(true);
        DRV_TMR16_Reset(false);
#endif
        MY_DRV_TMR16_PWM1_Set_Period_And_Set_Off(255, 255);
        MY_DRV_TMR16_PWM2_Set_Off(255, 255);
        MY_DRV_TMR16_PWM3_Set_Off(255, 255);
#if 0
        DRV_TMR16_PWM_Enable( 15 );
#endif
    } else {
        led_set_hsl(gHSL_server->present_hsl_hue, gHSL_server->present_hsl_saturation, gHSL_server->lightness_server.present_lightness);
    }
}

static void led_set_level(int16_t level)
{
    LOG_I(mesh_app, "led_set_level %d\n", level);
    if (level == -32768) {
        led_set_onoff(false);
    } else {
        led_set_hsl(gHSL_server->present_hsl_hue,
                    gHSL_server->present_hsl_saturation,
                    gHSL_server->lightness_server.present_lightness);
    }
}

static void led2_set_ctl(uint16_t temperature, uint16_t delta_uv, uint16_t ligntness)
{
    LOG_I(mesh_app, "led2_set_ctl temp:0x%x, deltaUV:0x%x, lightness:0x%x\n", temperature, delta_uv, ligntness);
    // TODO: add led2 CTL control logic
}

static void led2_set_onoff(bool on)
{
    LOG_I(mesh_app, "led2_set_onoff %d\n", on);
    if (!on) {
        // TODO: add led2 on/off control logic
    } else {
        led2_set_ctl(gCTL_server->present_ctl_temperature,
                     gCTL_server->present_ctl_delta_uv,
                     gCTL_server->lightness_server.present_lightness);
    }
}

static void led2_set_level(int16_t level)
{
    LOG_I(mesh_app, "led2_set_level %d\n", level);
    if (level == -32768) {
        led2_set_onoff(false);
    } else {
        led2_set_ctl(gCTL_server->present_ctl_temperature,
                     gCTL_server->present_ctl_delta_uv,
                     gCTL_server->lightness_server.present_lightness);
    }
}

static uint32_t _get_time_in_ms(uint8_t time)
{
    uint8_t resolution = (time & 0xC0) >> 6;
    uint8_t steps = (time & 0x3F);
    uint32_t ms = 0;

    switch (resolution) {
        case 0:
            ms = steps * 100;
            break;
        case 1:
            ms = steps * 1000;
            break;
        case 2:
            ms = steps * 10000;
            break;
        case 3:
            ms = steps * 3600000;
            break;
    }
    return ms;
}

static uint8_t _get_time_in_transition_time_state_format(uint32_t time_in_ms)
{
    uint8_t time;

    if (time_in_ms <= 62000) {
        time = 1 << 6 | ((time_in_ms / 1000) & 0x3F);
    } else if (time_in_ms > 62000 && time_in_ms <= 620000) {
        time = 2 << 6 | ((time_in_ms / 10000) & 0x3F);
    } else {
        time = 0x3F;
    }

    return time;
}

static TimerHandle_t mesh_generic_on_off_timer_hsl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_on_off_delay_timer_hsl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_generic_level_timer_hsl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_level_delay_timer_hsl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_actual_lightness_delay_timer_hsl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_actual_transition_timer_hsl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_linear_lightness_delay_timer_hsl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_linear_transition_timer_hsl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_generic_on_off_timer_ctl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_on_off_delay_timer_ctl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_generic_level_timer_ctl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_level_delay_timer_ctl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_actual_lightness_delay_timer_ctl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_actual_transition_timer_ctl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_linear_lightness_delay_timer_ctl = NULL; /**< Timer handler. */
static TimerHandle_t mesh_linear_transition_timer_ctl = NULL; /**< Timer handler. */

static TimerHandle_t mesh_hsl_delay_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_hsl_transition_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_hsl_hue_delay_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_hsl_hue_transition_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_hsl_saturation_delay_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_hsl_saturation_transition_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_generic_level_hsl_h_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_level_hsl_h_delay_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_generic_level_hsl_s_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_level_hsl_s_delay_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_ctl_delay_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_ctl_transition_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_ctl_temperature_transition_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_ctl_temperature_delay_timer = NULL; /**< Timer handler. */

static TimerHandle_t mesh_generic_level_ctl_t_timer = NULL; /**< Timer handler. */
static TimerHandle_t mesh_generic_level_ctl_t_delay_timer = NULL; /**< Timer handler. */

#define MESH_GENERIC_ON_OFF_DELAY_TIMER_ID_HSL       mesh_generic_on_off_delay_timer_hsl
#define MESH_GENERIC_LEVEL_DELAY_TIMER_ID_HSL        mesh_generic_level_delay_timer_hsl

#define MESH_GENERIC_ON_OFF_DELAY_TIMER_ID_CTL       mesh_generic_on_off_delay_timer_ctl
#define MESH_GENERIC_LEVEL_DELAY_TIMER_ID_CTL        mesh_generic_level_delay_timer_ctl

#define MESH_ACTUAL_LIGHTNESS_DELAY_TIMER_ID_HSL     mesh_actual_lightness_delay_timer_hsl
#define MESH_LINEAR_LIGHTNESS_DELAY_TIMER_ID_HSL     mesh_linear_lightness_delay_timer_hsl

#define MESH_ACTUAL_LIGHTNESS_DELAY_TIMER_ID_CTL    mesh_actual_lightness_delay_timer_ctl
#define MESH_LINEAR_LIGHTNESS_DELAY_TIMER_ID_CTL     mesh_linear_lightness_delay_timer_ctl

#define MESH_GENERIC_ON_OFF_TIMER_ID_HSL             mesh_generic_on_off_timer_hsl
#define MESH_GENERIC_LEVEL_TIMER_ID_HSL              mesh_generic_level_timer_hsl
#define MESH_ACTUAL_TRANSITION_TIMER_ID_HSL          mesh_actual_transition_timer_hsl
#define MESH_LINEAR_TRANSITION_TIMER_ID_HSL          mesh_linear_transition_timer_hsl

#define MESH_GENERIC_ON_OFF_TIMER_ID_CTL             mesh_generic_on_off_timer_ctl
#define MESH_GENERIC_LEVEL_TIMER_ID_CTL              mesh_generic_level_timer_ctl
#define MESH_ACTUAL_TRANSITION_TIMER_ID_CTL          mesh_actual_transition_timer_ctl
#define MESH_LINEAR_TRANSITION_TIMER_ID_CTL          mesh_linear_transition_timer_ctl

#define MESH_CTL_DELAY_TIMER_ID                  mesh_ctl_delay_timer
#define MESH_CTL_TEMPERATURE_DELAY_TIMER_ID      mesh_ctl_temperature_delay_timer
#define MESH_HSL_DELAY_TIMER_ID                  mesh_hsl_delay_timer
#define MESH_HSL_HUE_DELAY_TIMER_ID              mesh_hsl_hue_delay_timer
#define MESH_HSL_SATURATION_DELAY_TIMER_ID       mesh_hsl_saturation_delay_timer

#define MESH_CTL_TRANSITION_TIMER_ID             mesh_ctl_transition_timer
#define MESH_CTL_TEMPERATURE_TRANSITION_TIMER_ID mesh_ctl_temperature_transition_timer
#define MESH_HSL_TRANSITION_TIMER_ID             mesh_hsl_transition_timer
#define MESH_HSL_HUE_TRANSITION_TIMER_ID         mesh_hsl_hue_transition_timer
#define MESH_HSL_SATURATION_TRANSITION_TIMER_ID  mesh_hsl_saturation_transition_timer

#define MESH_GENERIC_LEVEL_HSL_H_DELAY_TIMER_ID        mesh_generic_level_hsl_h_delay_timer
#define MESH_GENERIC_LEVEL_HSL_S_DELAY_TIMER_ID        mesh_generic_level_hsl_s_delay_timer
#define MESH_GENERIC_LEVEL_CTL_T_DELAY_TIMER_ID        mesh_generic_level_ctl_t_delay_timer

#define MESH_GENERIC_LEVEL_HSL_H_TIMER_ID              mesh_generic_level_hsl_h_timer
#define MESH_GENERIC_LEVEL_HSL_S_TIMER_ID              mesh_generic_level_hsl_s_timer
#define MESH_GENERIC_LEVEL_CTL_T_TIMER_ID              mesh_generic_level_ctl_t_timer

static void _hsl_device_timer_handler(TimerHandle_t idx)
{
    if (MESH_GENERIC_ON_OFF_DELAY_TIMER_ID_HSL == idx) {
        if (gHSL_server->lightness_server.onoff_server.target_on_off == 1) {
            gHSL_server->lightness_server.onoff_server.present_on_off = gHSL_server->lightness_server.onoff_server.target_on_off;
        }

        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;

        bt_mesh_app_util_start_timer_ext("mesh goo hsl", MESH_GENERIC_ON_OFF_TIMER_ID_HSL, false, _get_time_in_ms(gHSL_server->lightness_server.onoff_server.transition_time), _hsl_device_timer_handler);
    }
    if (MESH_GENERIC_LEVEL_DELAY_TIMER_ID_HSL == idx) {
        bt_mesh_app_util_start_timer_ext("mesh gl hsl", MESH_GENERIC_LEVEL_TIMER_ID_HSL, false, _get_time_in_ms(gHSL_server->lightness_server.level_server.transition_time), _hsl_device_timer_handler);
    } else if (MESH_GENERIC_LEVEL_HSL_H_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh gl hslh", MESH_GENERIC_LEVEL_HSL_H_TIMER_ID, false,
                                         _get_time_in_ms(gHSL_hue_server->gLevel_server.transition_time), _hsl_device_timer_handler);
    } else if (MESH_GENERIC_LEVEL_HSL_S_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh gl hsls", MESH_GENERIC_LEVEL_HSL_S_TIMER_ID, false,
                                         _get_time_in_ms(gHSL_saturation_server->gLevel_server.transition_time), _hsl_device_timer_handler);
    } else if (MESH_ACTUAL_LIGHTNESS_DELAY_TIMER_ID_HSL == idx) {
        bt_mesh_app_util_start_timer_ext("mesh at hsl", MESH_ACTUAL_TRANSITION_TIMER_ID_HSL, false, _get_time_in_ms(gHSL_server->lightness_server.transition_time), _hsl_device_timer_handler);
    } else if (MESH_LINEAR_LIGHTNESS_DELAY_TIMER_ID_HSL == idx) {
        bt_mesh_app_util_start_timer_ext("mesh lt hsl", MESH_LINEAR_TRANSITION_TIMER_ID_HSL, false, _get_time_in_ms(gHSL_server->lightness_server.transition_time), _hsl_device_timer_handler);
    } else if (MESH_HSL_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh t hsl", MESH_HSL_TRANSITION_TIMER_ID, false, _get_time_in_ms(gHSL_server->transition_time), _hsl_device_timer_handler);
    } else if (MESH_HSL_HUE_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh ht hsl", MESH_HSL_HUE_TRANSITION_TIMER_ID, false, _get_time_in_ms(gHSL_hue_server->transition_time), _hsl_device_timer_handler);
    } else if (MESH_HSL_SATURATION_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh st hsl", MESH_HSL_SATURATION_TRANSITION_TIMER_ID, false, _get_time_in_ms(gHSL_saturation_server->transition_time), _hsl_device_timer_handler);
    } else if (MESH_GENERIC_ON_OFF_TIMER_ID_HSL == idx) {
        gHSL_server->lightness_server.onoff_server.present_on_off = gHSL_server->lightness_server.onoff_server.target_on_off;
        gHSL_server->lightness_server.onoff_server.transition_time = 0;
        gHSL_server->lightness_server.onoff_server.delay = 0;
        _bind_generic_on_off(&gHSL_server->lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_GENERIC_LEVEL_TIMER_ID_HSL == idx) {
        gHSL_server->lightness_server.level_server.present_level = gHSL_server->lightness_server.level_server.target_level;
        gHSL_server->lightness_server.level_server.transition_time = 0;
        gHSL_server->lightness_server.level_server.delay = 0;
        _bind_generic_level(&gHSL_server->lightness_server, MESH_MODEL_STATE_GENERIC_LEVEL, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_GENERIC_LEVEL_HSL_H_TIMER_ID == idx) {
        gHSL_hue_server->gLevel_server.present_level = gHSL_hue_server->gLevel_server.target_level;
        gHSL_hue_server->gLevel_server.transition_time = 0;
        gHSL_hue_server->gLevel_server.delay = 0;
        /* binding */
        gHSL_hue_server->present_hsl_hue = gHSL_hue_server->gLevel_server.present_level + 32768;
        gHSL_server->present_hsl_hue = gHSL_hue_server->present_hsl_hue;
        led_set_hsl(gHSL_server->present_hsl_hue, gHSL_server->present_hsl_saturation, gHSL_server->lightness_server.present_lightness);
    } else if (MESH_GENERIC_LEVEL_HSL_S_TIMER_ID == idx) {
        gHSL_saturation_server->gLevel_server.present_level = gHSL_saturation_server->gLevel_server.target_level;
        gHSL_saturation_server->gLevel_server.transition_time = 0;
        gHSL_saturation_server->gLevel_server.delay = 0;
        /* binding */
        gHSL_saturation_server->present_hsl_saturation = gHSL_hue_server->gLevel_server.present_level + 32768;
        gHSL_server->present_hsl_saturation = gHSL_saturation_server->present_hsl_saturation;
        led_set_hsl(gHSL_server->present_hsl_hue, gHSL_server->present_hsl_saturation, gHSL_server->lightness_server.present_lightness);
    } else if (MESH_ACTUAL_TRANSITION_TIMER_ID_HSL == idx) {
        gHSL_server->lightness_server.present_lightness = gHSL_server->lightness_server.target_lightness;
        gHSL_server->lightness_server.transition_time = 0;
        gHSL_server->lightness_server.delay = 0;
        _bind_lightness_actual(&gHSL_server->lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_LINEAR_TRANSITION_TIMER_ID_HSL == idx) {
        //bind linear lightness
        gHSL_server->lightness_server.present_linear_lightness = gHSL_server->lightness_server.target_linear_lightness;
        gHSL_server->lightness_server.transition_time = 0;
        gHSL_server->lightness_server.delay = 0;
        _bind_lightness_linear(&gHSL_server->lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_HSL_TRANSITION_TIMER_ID == idx) {
        gHSL_server->lightness_server.present_lightness = gHSL_server->lightness_server.target_lightness;
        gHSL_server->present_hsl_hue = gHSL_server->target_hsl_hue;
        gHSL_server->present_hsl_saturation = gHSL_server->target_hsl_saturation;
        gHSL_server->transition_time = 0;
        gHSL_server->delay = 0;
        _bind_hsl_lightness(gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_HSL_HUE_TRANSITION_TIMER_ID == idx) {
        gHSL_hue_server->present_hsl_hue = gHSL_hue_server->target_hsl_hue;
        gHSL_hue_server->transition_time = 0;
        gHSL_hue_server->delay = 0;
        bind_hsl_hue(gHSL_hue_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_HUE, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_HSL_SATURATION_TRANSITION_TIMER_ID == idx) {
        gHSL_saturation_server->present_hsl_saturation = gHSL_saturation_server->target_hsl_saturation;
        gHSL_saturation_server->transition_time = 0;
        gHSL_saturation_server->delay = 0;
        bind_hsl_saturation(gHSL_saturation_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_SATURATION, MESH_MODEL_BINDING_PRESENT_VALUE);
    }
}

static void _ctl_device_timer_handler(TimerHandle_t idx)
{
    if (MESH_GENERIC_ON_OFF_DELAY_TIMER_ID_CTL == idx) {
        if (gCTL_server->lightness_server.onoff_server.target_on_off == 1) {
            gCTL_server->lightness_server.onoff_server.present_on_off = gCTL_server->lightness_server.onoff_server.target_on_off;
        }

        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;

        bt_mesh_app_util_start_timer_ext("mesh goo ctl", MESH_GENERIC_ON_OFF_TIMER_ID_CTL, false, _get_time_in_ms(gCTL_server->lightness_server.onoff_server.transition_time), _ctl_device_timer_handler);
    }
    /* delay timer */
    if (MESH_GENERIC_LEVEL_DELAY_TIMER_ID_CTL == idx) {
        bt_mesh_app_util_start_timer_ext("mesh gl ctl", MESH_GENERIC_LEVEL_TIMER_ID_CTL, false, _get_time_in_ms(gCTL_server->lightness_server.level_server.transition_time), _ctl_device_timer_handler);
    } else if (MESH_GENERIC_LEVEL_CTL_T_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh glt ctl", MESH_GENERIC_LEVEL_CTL_T_TIMER_ID, false,
                                         _get_time_in_ms(gCTL_temperature_server->gLevel_server.transition_time), _ctl_device_timer_handler);
    } else if (MESH_ACTUAL_LIGHTNESS_DELAY_TIMER_ID_CTL == idx) {
        bt_mesh_app_util_start_timer_ext("mesh at ctl", MESH_ACTUAL_TRANSITION_TIMER_ID_CTL, false, _get_time_in_ms(gCTL_server->lightness_server.transition_time), _ctl_device_timer_handler);
    } else if (MESH_LINEAR_LIGHTNESS_DELAY_TIMER_ID_CTL == idx) {
        bt_mesh_app_util_start_timer_ext("mesh lt ctl", MESH_LINEAR_TRANSITION_TIMER_ID_CTL, false, _get_time_in_ms(gCTL_server->lightness_server.transition_time), _ctl_device_timer_handler);
    } else if (MESH_CTL_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh t ctl", MESH_CTL_TRANSITION_TIMER_ID, false, _get_time_in_ms(gCTL_server->transition_time), _ctl_device_timer_handler);
    } else if (MESH_CTL_TEMPERATURE_DELAY_TIMER_ID == idx) {
        bt_mesh_app_util_start_timer_ext("mesh t ctl", MESH_CTL_TRANSITION_TIMER_ID, false, _get_time_in_ms(gCTL_temperature_server->transition_time), _ctl_device_timer_handler);
    }
    /* transition timer */
    else if (MESH_GENERIC_ON_OFF_TIMER_ID_CTL == idx) {
        gCTL_server->lightness_server.onoff_server.present_on_off = gCTL_server->lightness_server.onoff_server.target_on_off;
        gCTL_server->lightness_server.onoff_server.transition_time = 0;
        gCTL_server->lightness_server.onoff_server.delay = 0;
        _bind_generic_on_off(&gCTL_server->lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_GENERIC_LEVEL_TIMER_ID_CTL == idx) {
        gCTL_server->lightness_server.level_server.present_level = gCTL_server->lightness_server.level_server.target_level;
        gCTL_server->lightness_server.level_server.transition_time = 0;
        gCTL_server->lightness_server.level_server.delay = 0;
        _bind_generic_level(&gCTL_server->lightness_server, MESH_MODEL_STATE_GENERIC_LEVEL, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_GENERIC_LEVEL_CTL_T_TIMER_ID == idx) {
        gCTL_temperature_server->gLevel_server.present_level = gCTL_temperature_server->gLevel_server.target_level;
        gCTL_temperature_server->gLevel_server.transition_time = 0;
        gCTL_temperature_server->gLevel_server.delay = 0;
        /* binding */
        gCTL_temperature_server->present_ctl_temperature = gCTL_temperature_server->gLevel_server.present_level + 32768;
        gCTL_server->present_ctl_temperature = gCTL_temperature_server->present_ctl_temperature;
        led2_set_ctl(gCTL_server->present_ctl_temperature, gCTL_server->present_ctl_delta_uv, gCTL_server->lightness_server.present_lightness);
    } else if (MESH_ACTUAL_TRANSITION_TIMER_ID_CTL == idx) {
        gCTL_server->lightness_server.present_lightness = gCTL_server->lightness_server.target_lightness;
        gCTL_server->lightness_server.transition_time = 0;
        gCTL_server->lightness_server.delay = 0;
        _bind_lightness_actual(&gCTL_server->lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_LINEAR_TRANSITION_TIMER_ID_CTL == idx) {
        //bind linear lightness
        gCTL_server->lightness_server.present_linear_lightness = gCTL_server->lightness_server.target_linear_lightness;
        gCTL_server->lightness_server.transition_time = 0;
        gCTL_server->lightness_server.delay = 0;
        _bind_lightness_linear(&gCTL_server->lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_CTL_TRANSITION_TIMER_ID == idx) {
        gCTL_server->lightness_server.present_lightness = gCTL_server->lightness_server.target_lightness;
        gCTL_server->present_ctl_temperature = gCTL_server->target_ctl_temperature;
        gCTL_server->present_ctl_delta_uv = gCTL_server->target_ctl_delta_uv;
        gCTL_server->transition_time = 0;
        gCTL_server->delay = 0;
        _bind_ctl_lightness(gCTL_server, MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS, MESH_MODEL_BINDING_PRESENT_VALUE);
    } else if (MESH_CTL_TEMPERATURE_TRANSITION_TIMER_ID == idx) {
        gCTL_temperature_server->present_ctl_temperature = gCTL_temperature_server->target_ctl_temperature;
        gCTL_temperature_server->present_ctl_delta_uv = gCTL_temperature_server->target_ctl_delta_uv;
        gCTL_temperature_server->transition_time = 0;
        gCTL_temperature_server->delay = 0;
        bind_ctl_temperature(gCTL_server, gCTL_temperature_server,
                             MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE, MESH_MODEL_BINDING_PRESENT_VALUE);
    }
}

/******************************************************************************
* main
******************************************************************************/
static void _bind_generic_on_off(lighting_lightness_server_t *lightness_server,
                                 uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "_bind_generic_on_off, present onoff: %d\n", lightness_server->onoff_server.present_on_off);
    /* 6.1.2.2.3 Binding with the Generic OnOff state
        Light Lightness Actual = 0
            for value of the Generic OnOff state equal to 0x00, or
        Light Lightness Actual = Light Lightness Last
            for value of the Generic OnOff state equal to 0x01,
            when value of the Light Lightness Default state is equal to 0x0000, or
        Light Lightness Actual = Light Lightness Default
            for value of the Generic OnOff state equal to 0x01,
            when value of the Light Lightness Default state is not equal to 0x0000.
    */
    /* not caused by lightness actual changed */
    if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
        if (lightness_server->onoff_server.present_on_off == 0) {
            lightness_server->present_lightness = lightness_server->target_lightness = 0;
        } else {
            if (lightness_server->default_lightness != 0) {
                lightness_server->present_lightness = lightness_server->target_lightness = lightness_server->default_lightness;
            } else {
                lightness_server->present_lightness = lightness_server->target_lightness = lightness_server->last_lightness;
            }

        }
        LOG_I(mesh_app, "\tlightness present: 0x%x, target: 0x%x\n", lightness_server->present_lightness, lightness_server->target_lightness);
        _bind_lightness_actual(lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF | state, binding);
    }
}

static void _bind_generic_level(lighting_lightness_server_t *lightness_server,
                                uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "_bind_generic_level, present level: 0x%x\n", lightness_server->level_server.present_level);
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            lightness_server->present_lightness = lightness_server->level_server.present_level + 32768;
            _bind_lightness_actual(lightness_server,
                                   MESH_MODEL_STATE_GENERIC_LEVEL | state, MESH_MODEL_BINDING_PRESENT_VALUE);
        }
        /*
                if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE)) {
                    gCTL_temperature_server->present_ctl_temperature = gCTL_server->range_min + (gCTL_temperature_server->gLevel_server.present_level + 32768) * (gCTL_server->range_max - gCTL_server->range_min) / 65535;
                }

                if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_HSL_HUE)) {
                    gHSL_hue_server->present_hsl_hue = gHSL_hue_server->gLevel_server.present_level + 32768;
                }

                if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_HSL_SATURATION)) {
                    gHSL_saturation_server->present_hsl_saturation = gHSL_saturation_server->gLevel_server.present_level + 32768;
                }
        */
    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            lightness_server->target_lightness = lightness_server->level_server.target_level + 32768;
            _bind_lightness_actual(lightness_server,
                                   MESH_MODEL_STATE_GENERIC_LEVEL | state, MESH_MODEL_BINDING_TARGET_VALUE);
        }
        /*
                if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE)) {
                    gCTL_temperature_server->target_ctl_temperature = gCTL_server->range_min + (gCTL_temperature_server->gLevel_server.target_level + 32768) * (gCTL_server->range_max - gCTL_server->range_min) / 65535;
                }
        */
    }
}

static void _bind_generic_power_up_lightness(lighting_lightness_server_t *lightness_server,
        uint16_t state, uint8_t binding)
{
    /* Generic OnOff server */
    {
        lightness_server->onoff_server.transition_time = 0;
        lightness_server->onoff_server.delay = 0;

        if (lightness_server->onpowerup_server.on_power_up == GENERIC_ON_POWERUP_OFF) {
            lightness_server->onoff_server.present_on_off = lightness_server->onoff_server.target_on_off = 0;
        } else if (lightness_server->onpowerup_server.on_power_up == GENERIC_ON_POWERUP_DEFAULT) {
            lightness_server->onoff_server.present_on_off = lightness_server->onoff_server.target_on_off = 1;
        } else if (lightness_server->onpowerup_server.on_power_up == GENERIC_ON_POWERUP_RESTORE) {
            lightness_server->onoff_server.present_on_off = lightness_server->onoff_server.target_on_off;
        }
    }

    /* Light Lightness server */
    /* Spec 6.1.2.2.4 Binding with the Generic OnPowerUp state
        Light Lightness Actual = 0
            for value of the Generic OnPowerUp state equal to 0x00, or
        Light Lightness Actual = Light Lightness Default
            for value of the Generic OnPowerUp state equal to 0x01 and Light Lightness Default not equal to zero, or
        Light Lightness Actual = Light Lightness Last (see Section 6.1.2.3)
            for value of the Generic OnPowerUp state equal to 0x01 and Light Lightness Default equal to zero, or
        Light Lightness Actual = last known value (before powered down) of the Light Lightness Actual
            for value of the Generic OnPowerUp state equal to 0x02.
    */
    {
        lightness_server->transition_time = 0;
        lightness_server->delay = 0;

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            if (lightness_server->onpowerup_server.on_power_up == GENERIC_ON_POWERUP_OFF) {
                lightness_server->present_lightness = lightness_server->target_lightness = 0;
            } else if (lightness_server->onpowerup_server.on_power_up == GENERIC_ON_POWERUP_DEFAULT) {
                if (lightness_server->default_lightness == 0) {
                    lightness_server->present_lightness = lightness_server->target_lightness = lightness_server->last_lightness;
                } else {
                    lightness_server->present_lightness = lightness_server->target_lightness = lightness_server->default_lightness;
                }
            } else if (lightness_server->onpowerup_server.on_power_up == GENERIC_ON_POWERUP_RESTORE) {
                lightness_server->present_lightness = lightness_server->target_lightness;
            }
        }
        _bind_lightness_actual(lightness_server, state | MESH_MODEL_STATE_GENERIC_ON_POWER_UP, binding);
    }
}

void bind_generic_power_up(uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "bind_generic_power_up\n");
    /* Light CTL server */
    {
        _bind_generic_power_up_lightness(&gCTL_server->lightness_server, state, binding);
        LOG_I(mesh_app, "bind_generic_onpowerup gCTL_server %d\n", gCTL_server->lightness_server.onpowerup_server.on_power_up);
        gCTL_temperature_server->transition_time = 0;
        gCTL_temperature_server->delay = 0;

        if (gCTL_server->lightness_server.onpowerup_server.on_power_up == GENERIC_ON_POWERUP_OFF || gCTL_server->lightness_server.onpowerup_server.on_power_up == GENERIC_ON_POWERUP_DEFAULT) {
            gCTL_server->present_ctl_delta_uv = gCTL_server->target_ctl_delta_uv = gCTL_server->default_delta_uv;

            gCTL_temperature_server->present_ctl_temperature = gCTL_temperature_server->target_ctl_temperature = gCTL_server->default_temperature;
            gCTL_temperature_server->present_ctl_delta_uv = gCTL_temperature_server->target_ctl_delta_uv = gCTL_server->default_delta_uv;
        } else if (gCTL_server->lightness_server.onpowerup_server.on_power_up == GENERIC_ON_POWERUP_RESTORE) {
            gCTL_server->present_ctl_delta_uv = gCTL_server->target_ctl_delta_uv;

            gCTL_temperature_server->present_ctl_temperature = gCTL_temperature_server->target_ctl_temperature;
            gCTL_temperature_server->present_ctl_delta_uv = gCTL_temperature_server->target_ctl_delta_uv;
        }
        bind_ctl_temperature(gCTL_server, gCTL_temperature_server, MESH_MODEL_STATE_GENERIC_ON_POWER_UP | state, binding);
        LOG_I(mesh_app, "present Temperature: 0x%04x, DeltaUV: 0x%04x, L: 0x%04x\n",
              gCTL_server->present_ctl_temperature,
              gCTL_server->present_ctl_delta_uv,
              gCTL_server->lightness_server.present_lightness);
        led2_set_onoff(gCTL_server->lightness_server.onoff_server.present_on_off);
    }

    /* Light HSL server */
    {
        _bind_generic_power_up_lightness(&gHSL_server->lightness_server, state, binding);
        LOG_I(mesh_app, "bind_generic_onpowerup gHSL_server %d\n", gHSL_server->lightness_server.onpowerup_server.on_power_up);
        gHSL_server->transition_time = 0;
        gHSL_server->delay = 0;

        if (gHSL_server->lightness_server.onpowerup_server.on_power_up == GENERIC_ON_POWERUP_OFF || gHSL_server->lightness_server.onpowerup_server.on_power_up == GENERIC_ON_POWERUP_DEFAULT) {
            gHSL_hue_server->present_hsl_hue = gHSL_hue_server->target_hsl_hue = gHSL_server->default_hue;
            gHSL_saturation_server->present_hsl_saturation = gHSL_saturation_server->target_hsl_saturation = gHSL_server->default_saturation;
        } else if (gHSL_server->lightness_server.onpowerup_server.on_power_up == GENERIC_ON_POWERUP_RESTORE) {
            gHSL_hue_server->present_hsl_hue = gHSL_hue_server->target_hsl_hue;
            gHSL_saturation_server->present_hsl_saturation = gHSL_saturation_server->target_hsl_saturation;
        }

        bind_hsl_hue(gHSL_hue_server, gHSL_server, MESH_MODEL_STATE_GENERIC_ON_POWER_UP | state, binding);
        bind_hsl_saturation(gHSL_saturation_server, gHSL_server, MESH_MODEL_STATE_GENERIC_ON_POWER_UP | state, binding);
        LOG_I(mesh_app, "present H: 0x%04x, S: 0x%04x, L: 0x%04x\n",
              gHSL_server->present_hsl_hue,
              gHSL_server->present_hsl_saturation,
              gHSL_server->lightness_server.present_lightness);
        led_set_onoff(gHSL_server->lightness_server.onoff_server.present_on_off);
    }
}

static void _bind_lightness_actual(lighting_lightness_server_t *lightness_server,
                                   uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "_bind_lightness_actual, present lightness: %x, range_min: %x, range_max: %x\n",
          lightness_server->present_lightness, lightness_server->range_min, lightness_server->range_max);
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        //binding with lightness range
        if (lightness_server->present_lightness < lightness_server->range_min &&
                lightness_server->present_lightness != 0) {
            lightness_server->present_lightness = lightness_server->range_min;
        } else if (lightness_server->present_lightness > lightness_server->range_max) {
            lightness_server->present_lightness = lightness_server->range_max;
        }

        //binding with lightness last
        if (lightness_server->present_lightness != 0) {
            lightness_server->last_lightness = lightness_server->present_lightness;
        }

        //binding with lightness linear
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR)) {
            double linear_lightness = pow(lightness_server->present_lightness, 2);
            linear_lightness /= 65535;
            lightness_server->present_linear_lightness = ceil(linear_lightness);
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            //binding with generic level
            lightness_server->level_server.present_level = lightness_server->present_lightness - 32768;
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_ON_OFF)) {
            //binding with generic onOff server
            if (lightness_server->present_lightness == 0) {
                lightness_server->onoff_server.present_on_off = 0;
            } else {
                lightness_server->onoff_server.present_on_off = 1;
            }
        }

        // determin CTL or HSL
        if (&gCTL_server->lightness_server == lightness_server) {
            if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS)) {
                //gCTL_server->present_ctl_lightness = gCTL_server->lightness_server.present_lightness;
                _bind_ctl_lightness(gCTL_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL | state, MESH_MODEL_BINDING_PRESENT_VALUE);
            }
        } else if (&gHSL_server->lightness_server == lightness_server) {
            if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS)) {
                //gHSL_server->present_hsl_lightness = gHSL_server->lightness_server.present_lightness;
                _bind_hsl_lightness(gHSL_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL | state, MESH_MODEL_BINDING_PRESENT_VALUE);
            }
        }
    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        //binding with lightness linear
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR)) {
            double linear_lightness = pow(lightness_server->target_lightness, 2);
            linear_lightness /= 65535;
            lightness_server->target_linear_lightness = ceil(linear_lightness);

        }
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            //binding with generic level
            lightness_server->level_server.target_level = lightness_server->target_lightness - 32768;
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_ON_OFF)) {
            //binding with generic onOff server
            if (lightness_server->target_lightness == 0) {
                lightness_server->onoff_server.target_on_off = 0;
            } else {
                lightness_server->onoff_server.target_on_off = 1;
            }
        }

        // determin CTL or HSL
        if (&gCTL_server->lightness_server == lightness_server) {
            if ((!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS))) {
                //gCTL_server->target_ctl_lightness = gCTL_server->lightness_server.target_lightness;
                _bind_ctl_lightness(gCTL_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL | state, MESH_MODEL_BINDING_TARGET_VALUE);
            }
        } else if (&gHSL_server->lightness_server == lightness_server) {
            if ((!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS))) {
                //gHSL_server->target_hsl_lightness = gHSL_server->lightness_server.target_lightness;
                _bind_hsl_lightness(gHSL_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL | state, MESH_MODEL_BINDING_TARGET_VALUE);
            }
        }
    }

}

static void _bind_lightness_linear(lighting_lightness_server_t *lightness_server,
                                   uint16_t state, uint8_t binding)
{
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            //binding with lightness actual
            double actual_lightness = lightness_server->present_linear_lightness * 65535;
            lightness_server->present_lightness = sqrt(actual_lightness);
            _bind_lightness_actual(lightness_server,
                                   MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR | state, MESH_MODEL_BINDING_PRESENT_VALUE);
        }
    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            //binding with lightness actual
            double actual_lightness = lightness_server->target_linear_lightness * 65535;
            lightness_server->target_lightness = sqrt(actual_lightness);

            _bind_lightness_actual(lightness_server,
                                   MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR | state, MESH_MODEL_BINDING_TARGET_VALUE);
        }
    }
}

static void _bind_lightness_range(lighting_lightness_server_t *lightness_server,
                                  uint16_t state, uint8_t binding)
{
    if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
        if (lightness_server->present_lightness < lightness_server->range_min &&
                lightness_server->present_lightness != 0) {
            lightness_server->present_lightness = lightness_server->range_min;
        } else if (lightness_server->present_lightness > lightness_server->range_max) {
            lightness_server->present_lightness = lightness_server->range_max;
        }

        _bind_lightness_actual(lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_RANGE | state, binding);
    }
}

static void _bind_ctl_lightness(lighting_ctl_server_t *ctl_server, uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "_bind_ctl_lightness, present ctl lightness: %x\n", ctl_server->lightness_server.present_lightness);

    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            //ctl_server->lightness_server.present_lightness = ctl_server->present_ctl_lightness;
            _bind_lightness_actual(&ctl_server->lightness_server, MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS | state, MESH_MODEL_BINDING_PRESENT_VALUE);
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE)) {
            gCTL_temperature_server->present_ctl_temperature = ctl_server->present_ctl_temperature;
        }
    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            //ctl_server->lightness_server.target_lightness = ctl_server->target_ctl_lightness;
            _bind_lightness_actual(&ctl_server->lightness_server, MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS | state, MESH_MODEL_BINDING_TARGET_VALUE);
        }
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE)) {
            gCTL_temperature_server->target_ctl_temperature = ctl_server->target_ctl_temperature;
        }
    }
}

void bind_ctl_temperature(lighting_ctl_server_t *ctl_server,
                          lighting_ctl_temperature_server_t *ctl_temp_server, uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "bind_ctl_temperature\n");

    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (ctl_temp_server->present_ctl_temperature < ctl_server->range_min) {
            ctl_temp_server->present_ctl_temperature = ctl_server->range_min;
        } else if (ctl_temp_server->present_ctl_temperature > ctl_server->range_max) {
            ctl_temp_server->present_ctl_temperature = ctl_server->range_max;
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            ctl_temp_server->gLevel_server.present_level = ((ctl_temp_server->present_ctl_temperature - ctl_server->range_min) * 65535) / (ctl_server->range_max - ctl_server->range_min) - 32768;
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS)) {
            ctl_server->present_ctl_temperature = ctl_temp_server->present_ctl_temperature;
        }

    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        if (ctl_temp_server->target_ctl_temperature < ctl_server->range_min) {
            ctl_temp_server->target_ctl_temperature = ctl_server->range_min;
        } else if (ctl_temp_server->target_ctl_temperature > ctl_server->range_max) {
            ctl_temp_server->target_ctl_temperature = ctl_server->range_max;
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            ctl_temp_server->gLevel_server.target_level = ((ctl_temp_server->target_ctl_temperature - ctl_server->range_min) * 65535) / (ctl_server->range_max - ctl_server->range_min) - 32768;
        }

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS)) {
            ctl_server->target_ctl_temperature = ctl_temp_server->target_ctl_temperature;
        }
    }
}

static void _bind_hsl_lightness(lighting_hsl_server_t *hsl_server,
                                uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "_bind_hsl_lightness, present hsl lightness: %x\n", hsl_server->lightness_server.present_lightness);
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            //hsl_server->lightness_server.present_lightness = hsl_server->present_hsl_lightness;
            _bind_lightness_actual(&hsl_server->lightness_server,
                                   MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS | state, MESH_MODEL_BINDING_PRESENT_VALUE);
        }

        if (hsl_server->present_hsl_hue < hsl_server->hue_range_min &&
                hsl_server->present_hsl_hue != 0) {
            hsl_server->present_hsl_hue = hsl_server->hue_range_min;
        } else if (hsl_server->present_hsl_hue > hsl_server->hue_range_max) {
            hsl_server->present_hsl_hue = hsl_server->hue_range_max;
        }

        gHSL_hue_server->present_hsl_hue = hsl_server->present_hsl_hue;

        if (hsl_server->present_hsl_saturation < hsl_server->saturation_range_min &&
                hsl_server->present_hsl_saturation != 0) {
            hsl_server->present_hsl_saturation = hsl_server->saturation_range_min;
        } else if (hsl_server->present_hsl_saturation > hsl_server->saturation_range_max) {
            hsl_server->present_hsl_saturation = hsl_server->saturation_range_max;
        }

        gHSL_saturation_server->present_hsl_saturation = hsl_server->present_hsl_saturation;
    }

    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL)) {
            hsl_server->lightness_server.target_lightness = hsl_server->lightness_server.target_lightness;
            _bind_lightness_actual(&hsl_server->lightness_server,
                                   MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS | state, MESH_MODEL_BINDING_TARGET_VALUE);
        }

        if (hsl_server->target_hsl_hue < hsl_server->hue_range_min &&
                hsl_server->target_hsl_hue != 0) {
            hsl_server->target_hsl_hue = hsl_server->hue_range_min;
        } else if (hsl_server->target_hsl_hue > hsl_server->hue_range_max) {
            hsl_server->target_hsl_hue = hsl_server->hue_range_max;
        }

        gHSL_hue_server->target_hsl_hue = hsl_server->target_hsl_hue;

        if (hsl_server->target_hsl_saturation < hsl_server->saturation_range_min &&
                hsl_server->target_hsl_saturation != 0) {
            hsl_server->target_hsl_saturation = hsl_server->saturation_range_min;
        } else if (hsl_server->target_hsl_saturation > hsl_server->saturation_range_max) {
            hsl_server->target_hsl_saturation = hsl_server->saturation_range_max;
        }

        gHSL_saturation_server->target_hsl_saturation = hsl_server->target_hsl_saturation;
    }
}

void bind_hsl_hue(lighting_hsl_hue_server_t *hsl_hue_server,
                  lighting_hsl_server_t *hsl_server, uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "bind_hsl_hue, present hue: 0x%x\n", hsl_hue_server->present_hsl_hue);
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (hsl_hue_server->present_hsl_hue < hsl_server->hue_range_min &&
                hsl_hue_server->present_hsl_hue != 0) {
            hsl_hue_server->present_hsl_hue = hsl_server->hue_range_min;
        } else if (hsl_hue_server->present_hsl_hue > hsl_server->hue_range_max) {
            hsl_hue_server->present_hsl_hue = hsl_server->hue_range_max;
        }

        hsl_server->present_hsl_hue = hsl_hue_server->present_hsl_hue;

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            hsl_hue_server->gLevel_server.present_level = hsl_hue_server->present_hsl_hue - 32768;
        }
    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        if (hsl_hue_server->target_hsl_hue < hsl_server->hue_range_min &&
                hsl_hue_server->target_hsl_hue != 0) {
            hsl_hue_server->target_hsl_hue = hsl_server->hue_range_min;
        } else if (hsl_hue_server->target_hsl_hue > hsl_server->hue_range_max) {
            hsl_hue_server->target_hsl_hue = hsl_server->hue_range_max;
        }

        hsl_server->target_hsl_hue = hsl_hue_server->target_hsl_hue;

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            hsl_hue_server->gLevel_server.target_level = hsl_hue_server->target_hsl_hue - 32768;
        }
    }
}

void bind_hsl_saturation(lighting_hsl_saturation_server_t *hsl_saturation_server,
                         lighting_hsl_server_t *hsl_server, uint16_t state, uint8_t binding)
{
    LOG_I(mesh_app, "bind_hsl_saturation, present saturation 0x%x\n", hsl_saturation_server->present_hsl_saturation);

    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_PRESENT_VALUE) {
        if (hsl_saturation_server->present_hsl_saturation < hsl_server->saturation_range_min &&
                hsl_saturation_server->present_hsl_saturation != 0) {
            hsl_saturation_server->present_hsl_saturation = hsl_server->saturation_range_min;
        } else if (hsl_saturation_server->present_hsl_saturation > hsl_server->saturation_range_max) {
            hsl_saturation_server->present_hsl_saturation = hsl_server->saturation_range_max;
        }

        hsl_server->present_hsl_saturation = hsl_saturation_server->present_hsl_saturation;

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            hsl_saturation_server->gLevel_server.present_level = hsl_saturation_server->present_hsl_saturation - 32768;
        }

    }
    if ((binding & MESH_MODEL_BINDING_MASK) & MESH_MODEL_BINDING_TARGET_VALUE) {
        if (hsl_saturation_server->target_hsl_saturation < hsl_server->saturation_range_min &&
                hsl_saturation_server->target_hsl_saturation != 0) {
            hsl_saturation_server->target_hsl_saturation = hsl_server->saturation_range_min;
        } else if (hsl_saturation_server->target_hsl_saturation > hsl_server->saturation_range_max) {
            hsl_saturation_server->target_hsl_saturation = hsl_server->saturation_range_max;
        }

        hsl_server->target_hsl_saturation = hsl_saturation_server->target_hsl_saturation;

        if (!((state & MESH_MODEL_STATE_MASK) & MESH_MODEL_STATE_GENERIC_LEVEL)) {
            hsl_saturation_server->gLevel_server.target_level = hsl_saturation_server->target_hsl_saturation - 32768;
        }
    }
}

static void _generic_on_off_set_msg_handler(lighting_lightness_server_t *lightness_server,
        uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged, vendordevice_led_type_t type)
{
    uint8_t *reply;
    uint8_t length = 0;

    if (lightness_server->onoff_server.TID == msg->buffer[1]) {
        return;
    }

    if (msg->buffer[0] != 0 && msg->buffer[0] != 1) {
        LOG_I(mesh_app, "invalid value for on_off state\n");
        return;
    }

    lightness_server->onoff_server.TID = msg->buffer[1];

    memcpy(&lightness_server->onoff_server.target_on_off, msg->buffer, 1);

    if (msg->length > 2) {
        lightness_server->onoff_server.transition_time = (msg->buffer[2] & 0x3F) ? msg->buffer[2] : lightness_server->dtt_server.default_transition_time;
        lightness_server->onoff_server.delay = msg->buffer[3];
    } else {
        lightness_server->onoff_server.transition_time = lightness_server->dtt_server.default_transition_time;
    }

    if (lightness_server->onoff_server.transition_time == 0) {
        length = 1;
        reply = bt_mesh_app_util_alloc(length);

        lightness_server->onoff_server.present_on_off = msg->buffer[0];
        reply[0] = lightness_server->onoff_server.present_on_off;
        _bind_generic_on_off(lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_BOTH_VALUE);
    } else {
        length = 3;
        reply = bt_mesh_app_util_alloc(length);

        reply[0] = lightness_server->onoff_server.present_on_off;
        reply[1] = lightness_server->onoff_server.target_on_off;
        reply[2] = lightness_server->onoff_server.transition_time;

        if (type == LED_1_HSL) {
            bt_mesh_app_util_start_timer_ext("mesh good hsl", mesh_generic_on_off_delay_timer_hsl, false,
                                             _get_time_in_ms(lightness_server->onoff_server.delay * 5), _hsl_device_timer_handler);
        } else if (type == LED_2_CTL) {
            bt_mesh_app_util_start_timer_ext("mesh good ctl", mesh_generic_on_off_delay_timer_ctl, false,
                                             _get_time_in_ms(lightness_server->onoff_server.delay * 5), _ctl_device_timer_handler);
        }

        _bind_generic_on_off(lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_TARGET_VALUE);
    }

    /* onoff control logic */
    if (type == LED_1_HSL) {
        led_set_onoff(lightness_server->onoff_server.present_on_off == 1);
    } else if (type == LED_2_CTL) {
        led2_set_onoff(lightness_server->onoff_server.present_on_off == 1);
    }

    if (!is_unacknowledged) {
        bt_mesh_model_generic_on_off_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_level_set_msg_handler(lighting_lightness_server_t *lightness_server,
        uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged, vendordevice_led_type_t type)
{
    uint8_t *reply;
    uint8_t length = 0;

    lightness_server->level_server.TID = msg->buffer[2];

    if (msg->length > 3) {
        lightness_server->level_server.transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] : lightness_server->dtt_server.default_transition_time;
        lightness_server->level_server.delay = msg->buffer[3];
    } else {
        lightness_server->level_server.transition_time = lightness_server->dtt_server.default_transition_time;
    }

    //target
    memcpy(&lightness_server->level_server.target_level, msg->buffer, 2);
    memcpy(&lightness_server->level_server.original_target_level, msg->buffer, 2);

    if (lightness_server->level_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(&lightness_server->level_server.present_level, msg->buffer, 2);
        memcpy(&lightness_server->level_server.original_present_level, msg->buffer, 2);
        memcpy(reply, &lightness_server->level_server.present_level, 2);

        _bind_generic_level(lightness_server, MESH_MODEL_STATE_GENERIC_LEVEL, MESH_MODEL_BINDING_BOTH_VALUE);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &lightness_server->level_server.present_level, 2);
        memcpy(&reply[2], &lightness_server->level_server.target_level, 2);
        reply[4] = lightness_server->level_server.transition_time;

        if (type == LED_1_HSL) {
            _hsl_device_timer_handler(mesh_generic_level_delay_timer_hsl);
        } else if (type == LED_2_CTL) {
            _ctl_device_timer_handler(mesh_generic_level_delay_timer_ctl);
        }
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        _bind_generic_level(lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_TARGET_VALUE);
    }

    /* level control logic */
    if (type == LED_1_HSL) {
        led_set_level(lightness_server->level_server.present_level);
    } else if (type == LED_2_CTL) {
        led2_set_level(lightness_server->level_server.present_level);
    }

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_hsl_hue_level_set_msg_handler(
    uint16_t model_handle, const bt_mesh_access_message_rx_t *msg,
    const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;

    gHSL_hue_server->gLevel_server.TID = msg->buffer[2];

    if (msg->length > 3) {
        gHSL_hue_server->gLevel_server.transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] :
                gHSL_server->lightness_server.dtt_server.default_transition_time;
        gHSL_hue_server->gLevel_server.delay = msg->buffer[4];
    } else {
        gHSL_hue_server->gLevel_server.transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    //target
    memcpy(&gHSL_hue_server->gLevel_server.target_level, msg->buffer, 2);
    memcpy(&gHSL_hue_server->gLevel_server.original_target_level, msg->buffer, 2);

    if (gHSL_hue_server->gLevel_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(&gHSL_hue_server->gLevel_server.present_level, msg->buffer, 2);
        memcpy(&gHSL_hue_server->gLevel_server.original_present_level, msg->buffer, 2);
        memcpy(reply, &gHSL_hue_server->gLevel_server.present_level, 2);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &gHSL_hue_server->gLevel_server.present_level, 2);
        memcpy(&reply[2], &gHSL_hue_server->gLevel_server.target_level, 2);
        reply[4] = gHSL_hue_server->gLevel_server.transition_time;

        _hsl_device_timer_handler(mesh_generic_level_hsl_h_delay_timer);
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
    }
    /* binding */
    gHSL_hue_server->present_hsl_hue = gHSL_hue_server->gLevel_server.present_level + 32768;
    gHSL_server->present_hsl_hue = gHSL_hue_server->present_hsl_hue;

    /* HSL hue level control logic */
    led_set_level(gHSL_server->lightness_server.level_server.present_level);

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_hsl_saturation_level_set_msg_handler(
    uint16_t model_handle, const bt_mesh_access_message_rx_t *msg,
    const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;

    gHSL_saturation_server->gLevel_server.TID = msg->buffer[2];

    if (msg->length > 3) {
        gHSL_saturation_server->gLevel_server.transition_time = (msg->buffer[3] & 0x3F) ?
                msg->buffer[3] : gHSL_server->lightness_server.dtt_server.default_transition_time;
        gHSL_saturation_server->gLevel_server.delay = msg->buffer[4];
    } else {
        gHSL_saturation_server->gLevel_server.transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    //target
    memcpy(&gHSL_saturation_server->gLevel_server.target_level, msg->buffer, 2);
    memcpy(&gHSL_saturation_server->gLevel_server.original_target_level, msg->buffer, 2);

    if (gHSL_saturation_server->gLevel_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(&gHSL_saturation_server->gLevel_server.present_level, msg->buffer, 2);
        memcpy(&gHSL_saturation_server->gLevel_server.original_present_level, msg->buffer, 2);
        memcpy(reply, &gHSL_saturation_server->gLevel_server.present_level, 2);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &gHSL_saturation_server->gLevel_server.present_level, 2);
        memcpy(&reply[2], &gHSL_saturation_server->gLevel_server.target_level, 2);
        reply[4] = gHSL_saturation_server->gLevel_server.transition_time;

        _hsl_device_timer_handler(mesh_generic_level_hsl_s_delay_timer);
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
    }
    /* binding */
    gHSL_saturation_server->present_hsl_saturation = gHSL_saturation_server->gLevel_server.present_level + 32768;
    gHSL_server->present_hsl_saturation = gHSL_saturation_server->present_hsl_saturation;

    /* HSL saturation level control logic */
    led_set_level(gHSL_server->lightness_server.level_server.present_level);

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_ctl_temperature_level_set_msg_handler(
    uint16_t model_handle, const bt_mesh_access_message_rx_t *msg,
    const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;

    gCTL_temperature_server->gLevel_server.TID = msg->buffer[2];

    if (msg->length > 3) {
        gCTL_temperature_server->gLevel_server.transition_time = (msg->buffer[3] & 0x3F) ?
                msg->buffer[3] : gCTL_server->lightness_server.dtt_server.default_transition_time;
        gCTL_temperature_server->gLevel_server.delay = msg->buffer[4];
    } else {
        gCTL_temperature_server->gLevel_server.transition_time = gCTL_server->lightness_server.dtt_server.default_transition_time;
    }

    //target
    memcpy(&gCTL_temperature_server->gLevel_server.target_level, msg->buffer, 2);
    memcpy(&gCTL_temperature_server->gLevel_server.original_target_level, msg->buffer, 2);

    if (gCTL_temperature_server->gLevel_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(&gCTL_temperature_server->gLevel_server.present_level, msg->buffer, 2);
        memcpy(&gCTL_temperature_server->gLevel_server.original_present_level, msg->buffer, 2);
        memcpy(reply, &gCTL_temperature_server->gLevel_server.present_level, 2);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &gCTL_temperature_server->gLevel_server.present_level, 2);
        memcpy(&reply[2], &gCTL_temperature_server->gLevel_server.target_level, 2);
        reply[4] = gCTL_temperature_server->gLevel_server.transition_time;

        //todo
        _ctl_device_timer_handler(MESH_GENERIC_LEVEL_CTL_T_DELAY_TIMER_ID);
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
    }
    /* binding */
    gCTL_temperature_server->present_ctl_temperature = gCTL_server->range_min + (gCTL_temperature_server->gLevel_server.present_level + 32768) * (gCTL_server->range_max - gCTL_server->range_min) / 65535;
    gCTL_temperature_server->target_ctl_temperature = gCTL_server->range_min + (gCTL_temperature_server->gLevel_server.target_level + 32768) * (gCTL_server->range_max - gCTL_server->range_min) / 65535;
    gCTL_server->present_ctl_temperature = gCTL_temperature_server->present_ctl_temperature;

    /* CTL temprature level control logic */
    led2_set_level(gCTL_server->lightness_server.level_server.present_level);

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_delta_set_msg_handler(lighting_lightness_server_t *lightness_server,
        uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged, vendordevice_led_type_t type)
{
    uint8_t *reply;
    uint8_t length = 0;
    int32_t delta;

    if (msg->length > 5) {
        lightness_server->level_server.transition_time = (msg->buffer[5] & 0x3F) ? msg->buffer[5] : lightness_server->dtt_server.default_transition_time;
        lightness_server->level_server.delay = msg->buffer[5];
    } else {
        lightness_server->level_server.transition_time = lightness_server->dtt_server.default_transition_time;
    }

    memcpy(&delta, msg->buffer, 4);

    if (lightness_server->level_server.TID == msg->buffer[4]) {
        LOG_I(mesh_app, "old TID\n");
        lightness_server->level_server.present_level = lightness_server->level_server.original_present_level;
        lightness_server->level_server.target_level = lightness_server->level_server.original_target_level;
    }

    lightness_server->level_server.TID = msg->buffer[4];

    lightness_server->level_server.target_level += delta;

    if (lightness_server->level_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        lightness_server->level_server.present_level += delta;
        memcpy(reply, &lightness_server->level_server.present_level, 2);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &lightness_server->level_server.present_level, 2);
        memcpy(&reply[2], &lightness_server->level_server.target_level, 2);
        reply[4] = lightness_server->level_server.transition_time;

        if (type == LED_1_HSL) {
            _hsl_device_timer_handler(mesh_generic_level_delay_timer_hsl);
        } else if (type == LED_2_CTL) {
            _ctl_device_timer_handler(mesh_generic_level_delay_timer_ctl);
        }

        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        _bind_generic_level(lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_TARGET_VALUE);
    }

    /* level delta control logic */
    if (type == LED_1_HSL) {
        led_set_level(lightness_server->level_server.present_level);
    } else if (type == LED_2_CTL) {
        led2_set_level(lightness_server->level_server.present_level);
    }

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_hsl_hue_delta_set_msg_handler(
    uint16_t model_handle, const bt_mesh_access_message_rx_t *msg,
    const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;
    uint16_t delta;

    if (msg->length > 5) {
        gHSL_hue_server->gLevel_server.transition_time = (msg->buffer[5] & 0x3F) ?
                msg->buffer[5] : gHSL_server->lightness_server.dtt_server.default_transition_time;
        gHSL_hue_server->gLevel_server.delay = msg->buffer[5];
    } else {
        gHSL_hue_server->gLevel_server.transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&delta, msg->buffer, 2);

    if (gHSL_hue_server->gLevel_server.TID == msg->buffer[4]) {
        LOG_I(mesh_app, "old TID\n");
        gHSL_hue_server->gLevel_server.present_level = gHSL_hue_server->gLevel_server.original_present_level;
        gHSL_hue_server->gLevel_server.target_level = gHSL_hue_server->gLevel_server.original_target_level;
    }

    gHSL_hue_server->gLevel_server.TID = msg->buffer[4];

    gHSL_hue_server->gLevel_server.target_level += delta;

    if (gHSL_hue_server->gLevel_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        gHSL_hue_server->gLevel_server.present_level += delta;
        memcpy(reply, &gHSL_hue_server->gLevel_server.present_level, 2);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &gHSL_hue_server->gLevel_server.present_level, 2);
        memcpy(&reply[2], &gHSL_hue_server->gLevel_server.target_level, 2);
        reply[4] = gHSL_hue_server->gLevel_server.transition_time;

        _hsl_device_timer_handler(mesh_generic_level_hsl_h_delay_timer);
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
    }

    /* binding */
    gHSL_hue_server->present_hsl_hue = gHSL_hue_server->gLevel_server.present_level + 32768;
    gHSL_server->present_hsl_hue = gHSL_hue_server->present_hsl_hue;

    led_set_level(gHSL_server->lightness_server.level_server.present_level);

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_hsl_saturation_delta_set_msg_handler(
    uint16_t model_handle, const bt_mesh_access_message_rx_t *msg,
    const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;
    uint16_t delta;

    if (msg->length > 5) {
        gHSL_saturation_server->gLevel_server.transition_time = (msg->buffer[5] & 0x3F) ?
                msg->buffer[5] : gHSL_server->lightness_server.dtt_server.default_transition_time;
        gHSL_saturation_server->gLevel_server.delay = msg->buffer[5];
    } else {
        gHSL_saturation_server->gLevel_server.transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&delta, msg->buffer, 2);

    if (gHSL_saturation_server->gLevel_server.TID == msg->buffer[4]) {
        LOG_I(mesh_app, "old TID\n");
        gHSL_saturation_server->gLevel_server.present_level = gHSL_saturation_server->gLevel_server.original_present_level;
        gHSL_saturation_server->gLevel_server.target_level = gHSL_saturation_server->gLevel_server.original_target_level;
    }

    gHSL_saturation_server->gLevel_server.TID = msg->buffer[4];

    gHSL_saturation_server->gLevel_server.target_level += delta;

    if (gHSL_saturation_server->gLevel_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);

        gHSL_saturation_server->gLevel_server.present_level += delta;
        memcpy(reply, &gHSL_saturation_server->gLevel_server.present_level, 2);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        memcpy(reply, &gHSL_saturation_server->gLevel_server.present_level, 2);
        memcpy(&reply[2], &gHSL_saturation_server->gLevel_server.target_level, 2);
        reply[4] = gHSL_saturation_server->gLevel_server.transition_time;

        _hsl_device_timer_handler(mesh_generic_level_hsl_s_delay_timer);
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
    }

    /* binding */
    gHSL_saturation_server->present_hsl_saturation = gHSL_saturation_server->gLevel_server.present_level + 32768;
    gHSL_server->present_hsl_saturation = gHSL_saturation_server->present_hsl_saturation;

    led_set_level(gHSL_server->lightness_server.level_server.present_level);

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _generic_move_set_msg_handler(lighting_lightness_server_t *lightness_server,
        uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged, vendordevice_led_type_t type)
{
    uint8_t *reply;
    uint8_t length = 0;
    uint16_t delta;

    if (lightness_server->level_server.TID == msg->buffer[2]) {
        return;
    }

    lightness_server->level_server.TID = msg->buffer[2];

    if (msg->length > 3) {
        lightness_server->level_server.transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] : lightness_server->dtt_server.default_transition_time;
        lightness_server->level_server.delay = msg->buffer[3];
    } else {
        lightness_server->level_server.transition_time = lightness_server->dtt_server.default_transition_time;
    }

    memcpy(&delta, msg->buffer, 2);
    lightness_server->level_server.target_level += delta;

    if (lightness_server->level_server.transition_time == 0) {
        length = 2;
        reply = bt_mesh_app_util_alloc(length);
    } else {
        length = 5;
        reply = bt_mesh_app_util_alloc(length);

        if (type == LED_1_HSL) {
            _hsl_device_timer_handler(mesh_generic_level_delay_timer_hsl);
        } else if (type == LED_2_CTL) {
            _ctl_device_timer_handler(mesh_generic_level_delay_timer_ctl);
        }

        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;

        if (lightness_server->level_server.present_level >= 0) {
            reply[2] = 0xFF;
            reply[3] = 0x7F;
            lightness_server->level_server.target_level = 0x7FFF;
        } else if (lightness_server->level_server.present_level < 0) {
            lightness_server->level_server.target_level = 0xFFFE;
        }
        memcpy(&reply[2], &lightness_server->level_server.target_level, 2);

        if ((lightness_server->level_server.transition_time & 0xC0) == 0xC0) {
            reply[4] = 0x3F;
        } else {
            reply[4] = lightness_server->level_server.transition_time;
        }
    }

    memcpy(reply, &lightness_server->level_server.present_level, 2);

    _bind_generic_level(lightness_server, MESH_MODEL_STATE_GENERIC_ON_OFF, MESH_MODEL_BINDING_TARGET_VALUE);

    if (type == LED_1_HSL) {
        led_set_level(lightness_server->level_server.present_level);
    } else if (type == LED_2_CTL) {
        led2_set_level(lightness_server->level_server.present_level);
    }

    /* TODO: level transition control logic */

    if (!is_unacknowledged) {
        bt_mesh_model_generic_level_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _light_actual_set_msg_handler(lighting_lightness_server_t *lightness_server,
        uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged, vendordevice_led_type_t type)
{
    uint8_t *reply;
    uint8_t length;

    if (lightness_server->TID == msg->buffer[2]) {
        return;
    }

    lightness_server->TID = msg->buffer[2];

    if (msg->length > 3) {
        lightness_server->transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] : lightness_server->dtt_server.default_transition_time;
    } else {
        lightness_server->transition_time = lightness_server->dtt_server.default_transition_time;
    }

    memcpy(&lightness_server->target_lightness, msg->buffer, 2);

    if ((lightness_server->transition_time & 0x3F) != 0) {
        length = 5;
        lightness_server->delay = msg->buffer[4];
        if (type == LED_1_HSL) {
            _hsl_device_timer_handler(mesh_actual_lightness_delay_timer_hsl);
        } else if (type == LED_2_CTL) {
            _ctl_device_timer_handler(mesh_actual_lightness_delay_timer_ctl);
        }

        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        _bind_lightness_actual(lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL, MESH_MODEL_BINDING_TARGET_VALUE);
    } else {
        length = 2;
        memcpy(&lightness_server->present_lightness, msg->buffer, 2);
        _bind_lightness_actual(lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL, MESH_MODEL_BINDING_BOTH_VALUE);
        if (type == LED_1_HSL) {
            led_set_onoff(lightness_server->present_lightness != 0);
        } else if (type == LED_2_CTL) {
            led2_set_onoff(lightness_server->present_lightness != 0);
        }
    }

    reply = bt_mesh_app_util_alloc(length);

    VD_DEBUG_DUMP_PDU("lightness load:", length, msg->buffer);

    memcpy(reply, &lightness_server->present_lightness, 2);

    if (length == 5) {
        memcpy(&reply[2], &lightness_server->target_lightness, 2);
        reply[4] = lightness_server->transition_time;
    }

    if (!is_unacknowledged) {
        bt_mesh_model_lighting_lightness_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _light_linear_set_msg_handler(lighting_lightness_server_t *lightness_server,
        uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged, vendordevice_led_type_t type)
{
    uint8_t *reply;
    uint8_t length;

    if (lightness_server->TID == msg->buffer[2]) {
        return;
    }

    lightness_server->TID = msg->buffer[2];

    if (msg->length > 3) {
        lightness_server->transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] : lightness_server->dtt_server.default_transition_time;
    } else {
        lightness_server->transition_time = lightness_server->dtt_server.default_transition_time;
    }

    memcpy(&lightness_server->target_linear_lightness, msg->buffer, 2);

    if ((lightness_server->transition_time & 0x3F) != 0) {
        length = 5;
        lightness_server->delay = msg->buffer[4];
        if (type == LED_1_HSL) {
            _hsl_device_timer_handler(mesh_linear_lightness_delay_timer_hsl);
        } else if (type == LED_2_CTL) {
            _ctl_device_timer_handler(mesh_linear_lightness_delay_timer_ctl);
        }
        gTransitionTick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        _bind_lightness_linear(lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR, MESH_MODEL_BINDING_TARGET_VALUE);

    } else {
        length = 2;
        memcpy(&lightness_server->present_linear_lightness, msg->buffer, 2);
        _bind_lightness_linear(lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR, MESH_MODEL_BINDING_BOTH_VALUE);
    }

    reply = bt_mesh_app_util_alloc(length);

    memcpy(reply, &lightness_server->present_linear_lightness, 2);

    if (length == 5) {
        memcpy(&reply[2], &lightness_server->target_linear_lightness, 2);
        reply[4] = lightness_server->transition_time;
    }

    if (!is_unacknowledged) {
        bt_mesh_model_lighting_lightness_linear_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _light_ctl_set_msg_handler(uint16_t model_handle,
                                       const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;
    uint16_t temperature;


    memcpy(&temperature, &msg->buffer[2], 2);

    /*yaming print buf content*/
    VD_DEBUG_DUMP_PDU("ctl_set load:", msg->length, msg->buffer);

    if (temperature > 0x4E20 || temperature < 0x0320) {
        LOG_I(mesh_app, "invalid temp value:0x%04x\n", temperature);
        return;
    }

    //check TID
    if (gCTL_server->TID == msg->buffer[6]) {
        return;
    }

    gCTL_server->TID = msg->buffer[6];

    if (msg->length > 7) {
        gCTL_server->transition_time = (msg->buffer[7] & 0x3F) ? msg->buffer[7] : gCTL_server->lightness_server.dtt_server.default_transition_time;
    } else {
        gCTL_server->transition_time = gCTL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&(gCTL_server->lightness_server.target_lightness), msg->buffer, 2);
    memcpy(&(gCTL_server->target_ctl_temperature), &msg->buffer[2], 2);
    memcpy(&gCTL_server->target_ctl_delta_uv, &msg->buffer[4], 2);

    LOG_I(mesh_app, "ctl_lightness:0x%04x\n", (gCTL_server->lightness_server.target_lightness));
    LOG_I(mesh_app, "ctl_temperature:0x%04x\n", (gCTL_server->target_ctl_temperature));
    LOG_I(mesh_app, "ctl_delta_uv:0x%04x (%d)\n", (gCTL_server->target_ctl_delta_uv), (gCTL_server->target_ctl_delta_uv));

    if ((gCTL_server->transition_time & 0x3F) != 0) {
        length = 9;
        gCTL_server->delay = msg->buffer[8];
        _ctl_device_timer_handler(mesh_ctl_delay_timer);
        gCTL_transition_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        _bind_ctl_lightness(gCTL_server, MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS, MESH_MODEL_BINDING_TARGET_VALUE);
    } else {
        length = 4;
        memcpy(&gCTL_server->lightness_server.present_lightness, msg->buffer, 2);
        memcpy(&gCTL_server->present_ctl_temperature, &msg->buffer[2], 2);
        memcpy(&gCTL_server->present_ctl_delta_uv, &msg->buffer[4], 2);

        _bind_ctl_lightness(gCTL_server, MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS, MESH_MODEL_BINDING_BOTH_VALUE);
    }

    led2_set_onoff(gCTL_server->lightness_server.present_lightness != 0);

    reply = bt_mesh_app_util_alloc(length);

    if (reply != NULL) {
        memcpy(reply, &gCTL_server->lightness_server.present_lightness, 2);
        memcpy(&reply[2], &gCTL_server->present_ctl_temperature, 2);

        if (length == 9) {
            memcpy(&reply[4], &gCTL_server->lightness_server.target_lightness, 2);
            memcpy(&reply[6], &gCTL_server->target_ctl_temperature, 2);

            reply[8] = msg->buffer[7];
        }

        VD_DEBUG_DUMP_PDU("hsl_set_status load:", length, reply);

        if (!is_unacknowledged) {
            bt_mesh_model_lighting_ctl_status(model_handle, reply, length, msg);
        }

        bt_mesh_app_util_free(reply);
    } else {
        LOG_I(mesh_app, "not enough memory\n");
    }
}

static void _light_ctl_temperature_set_msg_handler(uint16_t model_handle,
        const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;
    uint16_t temperature;

    memcpy(&temperature, msg->buffer, 2);
    if (temperature > 0x4E20 || temperature < 0x0320) {
        LOG_I(mesh_app, "invalid temperature value\n");
        return;
    }

    if (gCTL_temperature_server->TID == msg->buffer[4]) {
        return;
    }

    gCTL_temperature_server->TID = msg->buffer[4];

    if (msg->length > 5) {
        gCTL_temperature_server->transition_time = (msg->buffer[5] & 0x3F) ? msg->buffer[5] : gCTL_server->lightness_server.dtt_server.default_transition_time;
    } else {
        gCTL_temperature_server->transition_time = gCTL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&gCTL_temperature_server->target_ctl_temperature, msg->buffer, 2);
    memcpy(&gCTL_temperature_server->target_ctl_delta_uv, &msg->buffer[2], 2);

    if ((gCTL_temperature_server->transition_time & 0x3F) != 0) {
        length = 9;
        gCTL_temperature_server->delay = msg->buffer[6];
        _ctl_device_timer_handler(mesh_ctl_delay_timer);
        gCTL_transition_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        bind_ctl_temperature(gCTL_server, gCTL_temperature_server, MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE, MESH_MODEL_BINDING_TARGET_VALUE);
    } else {
        length = 4;
        memcpy(&gCTL_temperature_server->present_ctl_temperature, msg->buffer, 2);
        memcpy(&gCTL_temperature_server->present_ctl_delta_uv, &msg->buffer[2], 2);

        bind_ctl_temperature(gCTL_server, gCTL_temperature_server, MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE, MESH_MODEL_BINDING_BOTH_VALUE);
    }

    led2_set_onoff(gCTL_server->lightness_server.present_lightness != 0);

    reply = bt_mesh_app_util_alloc(length);

    if (reply != NULL) {
        memcpy(reply, &gCTL_temperature_server->present_ctl_temperature, 2);
        memcpy(&reply[2], &gCTL_temperature_server->present_ctl_delta_uv, 2);

        if (length == 9) {
            memcpy(&reply[4], &gCTL_temperature_server->target_ctl_temperature, 2);
            memcpy(&reply[6], &gCTL_temperature_server->target_ctl_delta_uv, 2);

            reply[8] = gCTL_temperature_server->transition_time;
        }

        if (!is_unacknowledged) {
            bt_mesh_model_lighting_ctl_temperature_status(model_handle, reply, length, msg);
        }

        bt_mesh_app_util_free(reply);
    } else {
        LOG_I(mesh_app, "not enough memory\n");
    }
}

static void _light_hsl_set_msg_handler(uint16_t model_handle,
                                       const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;

    uint16_t temp_lightness;
    uint16_t temp_hue;
    uint16_t temp_saturation;

    if (gHSL_server->TID == msg->buffer[6]) {
        return;
    }

    gHSL_server->TID = msg->buffer[6];

    if (msg->length > 7) {
        gHSL_server->transition_time = (msg->buffer[7] & 0x3F) ? msg->buffer[7] : gHSL_server->lightness_server.dtt_server.default_transition_time;
    } else {
        gHSL_server->transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&gHSL_server->lightness_server.target_lightness, msg->buffer, 2);
    memcpy(&gHSL_server->target_hsl_hue, &msg->buffer[2], 2);
    memcpy(&gHSL_server->target_hsl_saturation, &msg->buffer[4], 2);

    if ((gHSL_server->transition_time & 0x3F) != 0) {
        length = 7;
        gHSL_server->delay = msg->buffer[8];
        _hsl_device_timer_handler(mesh_hsl_delay_timer);
        gHSL_transition_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        _bind_hsl_lightness(gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS, MESH_MODEL_BINDING_TARGET_VALUE);
    } else {
        length = 6;
        memcpy(&gHSL_server->lightness_server.present_lightness, msg->buffer, 2);
        memcpy(&gHSL_server->present_hsl_hue, &msg->buffer[2], 2);
        memcpy(&gHSL_server->present_hsl_saturation, &msg->buffer[4], 2);
        _bind_hsl_lightness(gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS, MESH_MODEL_BINDING_BOTH_VALUE);

    }

    VD_DEBUG_DUMP_PDU("hsl_set load:", msg->length, msg->buffer);

    memcpy(&temp_lightness, msg->buffer, 2);
    memcpy(&temp_hue, &msg->buffer[2], 2);
    memcpy(&temp_saturation, &msg->buffer[4], 2);
    LOG_I(mesh_app, "temp_lightness:0x%x,temp_hue:0x%x,temp_saturation:0x%x\n", temp_lightness, temp_hue, temp_saturation);

    led_set_hsl(temp_hue, temp_saturation, temp_lightness);

    reply = bt_mesh_app_util_alloc(length);

    memcpy(reply, &gHSL_server->lightness_server.present_lightness, 2);
    memcpy(&reply[2], &gHSL_server->present_hsl_hue, 2);
    memcpy(&reply[4], &gHSL_server->present_hsl_saturation, 2);

    if (length == 7) {
        reply[6] = gHSL_server->transition_time;
    }

    VD_DEBUG_DUMP_PDU("hsl_set_status load:", length, reply);

    if (!is_unacknowledged) {
        bt_mesh_model_lighting_hsl_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _light_hsl_hue_set_msg_handler(uint16_t model_handle,
        const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;

    if (gHSL_hue_server->TID == msg->buffer[2]) {
        return;
    }

    gHSL_hue_server->TID = msg->buffer[2];

    if (msg->length > 3) {
        gHSL_hue_server->transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] : gHSL_server->lightness_server.dtt_server.default_transition_time;
    } else {
        gHSL_hue_server->transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&gHSL_hue_server->target_hsl_hue, msg->buffer, 2);

    if ((gHSL_hue_server->transition_time & 0x3F) != 0) {
        length = 5;
        gHSL_hue_server->delay = msg->buffer[4];
        _hsl_device_timer_handler(mesh_hsl_hue_delay_timer);
        gHSL_transition_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        bind_hsl_hue(gHSL_hue_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_HUE, MESH_MODEL_BINDING_TARGET_VALUE);
    } else {
        length = 2;
        memcpy(&gHSL_hue_server->present_hsl_hue, msg->buffer, 2);
        bind_hsl_hue(gHSL_hue_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_HUE, MESH_MODEL_BINDING_BOTH_VALUE);
    }

    led_set_onoff(gHSL_server->lightness_server.present_lightness != 0);

    reply = bt_mesh_app_util_alloc(length);

    memcpy(reply, &gHSL_hue_server->present_hsl_hue, 2);

    if (length == 5) {
        memcpy(&reply[2], &gHSL_hue_server->target_hsl_hue, 2);
        reply[4] = gHSL_hue_server->transition_time;
    }

    if (!is_unacknowledged) {
        bt_mesh_model_lighting_hsl_hue_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

static void _light_hsl_saturation_set_msg_handler(uint16_t model_handle,
        const bt_mesh_access_message_rx_t *msg, const void *arg, bool is_unacknowledged)
{
    uint8_t *reply;
    uint8_t length = 0;

    if (gHSL_saturation_server->TID == msg->buffer[2]) {
        return;
    }

    gHSL_saturation_server->TID = msg->buffer[2];

    if (msg->length > 3) {
        gHSL_saturation_server->transition_time = (msg->buffer[3] & 0x3F) ? msg->buffer[3] : gHSL_server->lightness_server.dtt_server.default_transition_time;
    } else {
        gHSL_saturation_server->transition_time = gHSL_server->lightness_server.dtt_server.default_transition_time;
    }

    memcpy(&gHSL_saturation_server->target_hsl_saturation, msg->buffer, 2);

    if ((gHSL_saturation_server->transition_time & 0x3F) != 0) {
        length = 5;
        gHSL_saturation_server->delay = msg->buffer[4];
        _hsl_device_timer_handler(mesh_hsl_saturation_delay_timer);
        gHSL_transition_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        bind_hsl_saturation(gHSL_saturation_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_SATURATION, MESH_MODEL_BINDING_TARGET_VALUE);
    } else {
        length = 2;
        memcpy(&gHSL_saturation_server->present_hsl_saturation, msg->buffer, 2);
        bind_hsl_saturation(gHSL_saturation_server, gHSL_server, MESH_MODEL_STATE_LIGHTING_HSL_SATURATION, MESH_MODEL_BINDING_BOTH_VALUE);
    }

    led_set_onoff(gHSL_server->lightness_server.present_lightness != 0);

    reply = bt_mesh_app_util_alloc(length);

    memcpy(reply, &gHSL_saturation_server->present_hsl_saturation, 2);

    if (length == 5) {
        memcpy(&reply[2], &gHSL_saturation_server->target_hsl_saturation, 2);
        reply[4] = gHSL_saturation_server->transition_time;
    }

    if (!is_unacknowledged) {
        bt_mesh_model_lighting_hsl_saturation_status(model_handle, reply, length, msg);
    }

    bt_mesh_app_util_free(reply);
}

void light_hsl_server_msg_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg)
{
    lighting_lightness_server_t *p_lightness_server = &gHSL_server->lightness_server;
    uint16_t element_idx = bt_mesh_model_get_element_index_by_model_handle(model_handle);

    switch (msg->opcode.opcode) {
        case BT_MESH_MODEL_GENERIC_ONOFF_GET: {
            uint8_t *reply;
            uint8_t length = 0;

            VD_DEBUG_EVT_PRINTF("GenericOnOffGet", 2);

            if (p_lightness_server->onoff_server.transition_time == 0) {
                length = 1;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->onoff_server.present_on_off, 1);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_lightness_server->onoff_server.transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_lightness_server->onoff_server.transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);

                length = 3;
                reply = bt_mesh_app_util_alloc(length);

                reply[0] = p_lightness_server->onoff_server.present_on_off;
                reply[1] = p_lightness_server->onoff_server.target_on_off;
                reply[2] = remaining_time;
            }

            bt_mesh_model_generic_on_off_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONOFF_SET: {

            VD_DEBUG_EVT_PRINTF("GenericOnOffSet", 2);

            _generic_on_off_set_msg_handler(p_lightness_server,
                                            model_handle, msg, arg, false, LED_1_HSL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONOFF_SET_UNACKNOWLEDGED: {

            VD_DEBUG_EVT_PRINTF("GenericOnOffSetUnacknowledged", 2);

            _generic_on_off_set_msg_handler(p_lightness_server,
                                            model_handle, msg, arg, true, LED_1_HSL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_LEVEL_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("GenericLevelGet", 2);

            generic_level_server_model_t *p_level_server = NULL;

            if (element_idx == gHSL_hue_server->element_index) {
                p_level_server = &gHSL_hue_server->gLevel_server;
            } else if (element_idx == gHSL_saturation_server->element_index) {
                p_level_server = &gHSL_saturation_server->gLevel_server;
            } else {
                p_level_server = &gHSL_server->lightness_server.level_server;
            }

            if (p_level_server->transition_time == 0) {
                length = 2;
            } else {
                length = 5;
            }

            reply = bt_mesh_app_util_alloc(length);
            memcpy(reply, &p_level_server->present_level, 2);

            if (length == 5) {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_level_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_level_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                memcpy(&reply[2], &p_level_server->target_level, 2);
                reply[4] = remaining_time;
            }

            bt_mesh_model_generic_level_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);
            break;
        }
        case BT_MESH_MODEL_GENERIC_LEVEL_SET: {
            VD_DEBUG_EVT_PRINTF("GenericLevelSet", 2);

            if (element_idx == gHSL_hue_server->element_index) {
                _generic_hsl_hue_level_set_msg_handler(model_handle, msg, arg, false);
            } else if (element_idx == gHSL_saturation_server->element_index) {
                _generic_hsl_saturation_level_set_msg_handler(model_handle, msg, arg, false);
            } else {
                _generic_level_set_msg_handler(p_lightness_server,
                                               model_handle, msg, arg, false, LED_1_HSL);
            }
            break;
        }
        case BT_MESH_MODEL_GENERIC_LEVEL_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericLevelSetUnacknowledged", 2);

            if (element_idx == gHSL_hue_server->element_index) {
                _generic_hsl_hue_level_set_msg_handler(model_handle, msg, arg, true);
            } else if (element_idx == gHSL_saturation_server->element_index) {
                _generic_hsl_saturation_level_set_msg_handler(model_handle, msg, arg, true);
            } else {
                _generic_level_set_msg_handler(p_lightness_server,
                                               model_handle, msg, arg, true, LED_1_HSL);
            }
            break;
        }
        case BT_MESH_MODEL_GENERIC_DELTA_SET: {
            VD_DEBUG_EVT_PRINTF("GenericDeltaSet", 2);

            if (element_idx == gHSL_hue_server->element_index) {
                _generic_hsl_hue_delta_set_msg_handler(model_handle, msg, arg, false);
            } else if (element_idx == gHSL_saturation_server->element_index) {
                _generic_hsl_saturation_delta_set_msg_handler(model_handle, msg, arg, false);
            } else {
                _generic_delta_set_msg_handler(p_lightness_server,
                                               model_handle, msg, arg, false, LED_1_HSL);
            }
            break;
        }
        case BT_MESH_MODEL_GENERIC_DELTA_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericDeltaSetUnacknowledged", 2);

            if (element_idx == gHSL_hue_server->element_index) {
                _generic_hsl_hue_delta_set_msg_handler(model_handle, msg, arg, true);
            } else if (element_idx == gHSL_saturation_server->element_index) {
                _generic_hsl_saturation_delta_set_msg_handler(model_handle, msg, arg, true);
            } else {
                _generic_delta_set_msg_handler(p_lightness_server,
                                               model_handle, msg, arg, true, LED_1_HSL);
            }
            break;
        }
        case BT_MESH_MODEL_GENERIC_MOVE_SET: {
            VD_DEBUG_EVT_PRINTF("GenericMoveSet", 2);

            _generic_move_set_msg_handler(p_lightness_server,
                                          model_handle, msg, arg, false, LED_1_HSL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_MOVE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericMoveSetUnacknowledged", 2);

            _generic_move_set_msg_handler(p_lightness_server,
                                          model_handle, msg, arg, true, LED_1_HSL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_GET: {
            VD_DEBUG_EVT_PRINTF("GenericDefaultTransitionTimeGet", 2);

            bt_mesh_model_generic_default_transition_time_status(model_handle,
                    p_lightness_server->dtt_server.default_transition_time, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SET: {
            if ((msg->buffer[0] & 0x3f) == 0x3F) {
                LOG_I(mesh_app, "invalid value [%02x]\n", msg->buffer[0]);
                return;
            }

            p_lightness_server->dtt_server.default_transition_time = msg->buffer[0];
            VD_DEBUG_EVT_PRINTF("GenericDefaultTransitionTimeSet", 2);
            LOG_I(mesh_app, "time  : %x\n", p_lightness_server->dtt_server.default_transition_time);

            bt_mesh_model_generic_default_transition_time_status(model_handle,
                    p_lightness_server->dtt_server.default_transition_time, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACKNOWLEDGED: {
            if ((msg->buffer[0] & 0x3f) == 0x3F) {
                LOG_I(mesh_app, "invalid value [%02x]\n", msg->buffer[0]);
                return;
            }

            p_lightness_server->dtt_server.default_transition_time = msg->buffer[0];
            VD_DEBUG_EVT_PRINTF("GenericDefaultTransitionTimeSetUnacknowledged", 2);
            LOG_I(mesh_app, "time  : %x\n", p_lightness_server->dtt_server.default_transition_time);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONPOWERUP_GET: {
            VD_DEBUG_EVT_PRINTF("GenericOnPowerupGet", 2);

            bt_mesh_model_generic_onpowerup_status(model_handle,
                                                   p_lightness_server->onpowerup_server.on_power_up, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONPOWERUP_SET: {
            VD_DEBUG_EVT_PRINTF("GenericOnPowerupSet", 2);

            if (msg->buffer[0] > 2) {
                return;
            }

            p_lightness_server->onpowerup_server.on_power_up = msg->buffer[0];

            bt_mesh_model_generic_onpowerup_status(model_handle,
                                                   p_lightness_server->onpowerup_server.on_power_up, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONPOWERUP_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericOnPowerupSetUnacknowledged", 2);

            if (msg->buffer[0] > 2) {
                return;
            }

            p_lightness_server->onpowerup_server.on_power_up = msg->buffer[0];
            break;
        }

        case BT_MESH_MODEL_LIGHT_LIGHTNESS_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessSet", 2);

            _light_actual_set_msg_handler(p_lightness_server, model_handle, msg, arg, false, LED_1_HSL);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessSetUnacknowledged", 2);

            _light_actual_set_msg_handler(p_lightness_server, model_handle, msg, arg, true, LED_1_HSL);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightLightnessGet", 2);

            if (p_lightness_server->transition_time == 0) {
                length = 2;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->present_lightness, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_lightness_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_lightness_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);

                length = 5;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->present_lightness, 2);
                memcpy(&reply[2], &p_lightness_server->target_lightness, 2);
                reply[4] = remaining_time;
            }

            bt_mesh_model_lighting_lightness_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessLinearSet", 2);

            _light_linear_set_msg_handler(p_lightness_server, model_handle, msg, arg, false, LED_1_HSL);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessLinearSetUnacknowledged", 2);

            _light_linear_set_msg_handler(p_lightness_server, model_handle, msg, arg, true, LED_1_HSL);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightLightnessLinearGet", 2);

            if (p_lightness_server->target_linear_lightness == p_lightness_server->present_linear_lightness &&
                    p_lightness_server->transition_time == 0) {
                length = 2;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->present_linear_lightness, length);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_lightness_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_lightness_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 5;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &p_lightness_server->present_linear_lightness, 2);
                memcpy(&reply[2], &p_lightness_server->target_linear_lightness, 2);
                reply[4] = remaining_time;

            }

            bt_mesh_model_lighting_lightness_linear_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessDefaultSet", 2);

            memcpy(&p_lightness_server->default_lightness, msg->buffer, 2);

            bt_mesh_model_lighting_lightness_default_status(model_handle, p_lightness_server->default_lightness, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessDefaultSetUnacknowledged", 2);

            memcpy(&p_lightness_server->default_lightness, msg->buffer, 2);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_GET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessDefaultGet", 2);

            bt_mesh_model_lighting_lightness_default_status(model_handle, p_lightness_server->default_lightness, msg);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessRangeSet", 2);

            uint16_t min, max;

            memcpy(&min, msg->buffer, 2);
            memcpy(&max, &msg->buffer[2], 2);

            if (min < max) {
                memcpy(&p_lightness_server->range_min, msg->buffer, 2);
                memcpy(&p_lightness_server->range_max, &msg->buffer[2], 2);

                _bind_lightness_range(p_lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_RANGE, MESH_MODEL_BINDING_BOTH_VALUE);

                bt_mesh_model_lighting_lightness_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, p_lightness_server->range_min, p_lightness_server->range_max, msg);
            }

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessRangeSetUnacknowledged", 2);

            memcpy(&p_lightness_server->range_min, msg->buffer, 2);
            memcpy(&p_lightness_server->range_max, &msg->buffer[2], 2);

            _bind_lightness_range(p_lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_RANGE, MESH_MODEL_BINDING_BOTH_VALUE);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_GET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessRangeGet", 2);

            bt_mesh_model_lighting_lightness_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, p_lightness_server->range_min, p_lightness_server->range_max, msg);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LAST_GET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessLastGet", 2);

            bt_mesh_model_lighting_lightness_last_status(model_handle, p_lightness_server->last_lightness, msg);
            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightHSLGet", 2);

            if (gHSL_server->lightness_server.target_lightness == gHSL_server->lightness_server.present_lightness &&
                    gHSL_server->target_hsl_hue == gHSL_server->present_hsl_hue &&
                    gHSL_server->target_hsl_saturation == gHSL_server->present_hsl_saturation &&
                    gHSL_server->transition_time == 0) {
                length = 6;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &gHSL_server->lightness_server.present_lightness, 2);
                memcpy(&reply[2], &gHSL_server->present_hsl_hue, 2);
                memcpy(&reply[4], &gHSL_server->present_hsl_saturation, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gHSL_transition_tick;

                if (_get_time_in_ms(gHSL_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(gHSL_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 7;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &gHSL_server->lightness_server.present_lightness, 2);
                memcpy(&reply[2], &gHSL_server->present_hsl_hue, 2);
                memcpy(&reply[4], &gHSL_server->present_hsl_saturation, 2);
                reply[6] = remaining_time;
            }

            bt_mesh_model_lighting_hsl_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_SET: {
            VD_DEBUG_EVT_PRINTF("LightHSLSet", 2);

            _light_hsl_set_msg_handler(model_handle, msg, arg, false);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightHSLSetUnacknowledged", 2);

            _light_hsl_set_msg_handler(model_handle, msg, arg, true);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_HUE_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightHSLHueGet", 2);

            if (gHSL_hue_server->target_hsl_hue == gHSL_hue_server->present_hsl_hue &&
                    gHSL_hue_server->transition_time == 0) {
                length = 2;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &gHSL_hue_server->present_hsl_hue, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gHSL_transition_tick;

                if (_get_time_in_ms(gHSL_hue_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(gHSL_hue_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 5;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &gHSL_hue_server->present_hsl_hue, 2);
                memcpy(&reply[2], &gHSL_hue_server->target_hsl_hue, 2);
                reply[4] = remaining_time;
            }

            bt_mesh_model_lighting_hsl_hue_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_HUE_SET: {
            VD_DEBUG_EVT_PRINTF("LightHSLHueSet", 2);

            _light_hsl_hue_set_msg_handler(model_handle, msg, arg, false);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_HUE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightHSLSetUnacknowledged", 2);

            _light_hsl_hue_set_msg_handler(model_handle, msg, arg, true);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_SATURATION_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightHSLSaturationGet", 2);

            if (gHSL_saturation_server->target_hsl_saturation == gHSL_saturation_server->present_hsl_saturation &&
                    gHSL_saturation_server->transition_time == 0) {
                length = 2;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &gHSL_saturation_server->present_hsl_saturation, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gHSL_transition_tick;

                if (_get_time_in_ms(gHSL_saturation_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(gHSL_saturation_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 5;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &gHSL_saturation_server->present_hsl_saturation, 2);
                memcpy(&reply[2], &gHSL_saturation_server->target_hsl_saturation, 2);
                reply[4] = remaining_time;
            }

            bt_mesh_model_lighting_hsl_saturation_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_SATURATION_SET: {
            VD_DEBUG_EVT_PRINTF("LightHSLSaturationSet", 2);

            _light_hsl_saturation_set_msg_handler(model_handle, msg, arg, false);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_SATURATION_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightHSLSaturationSetUnacknowledged", 2);

            _light_hsl_saturation_set_msg_handler(model_handle, msg, arg, true);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_TARGET_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightHSLTargetGet", 2);

            if (gHSL_server->transition_time == 0) {
                length = 6;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &gHSL_server->lightness_server.target_lightness, 2);
                memcpy(&reply[2], &gHSL_server->target_hsl_hue, 2);
                memcpy(&reply[4], &gHSL_server->target_hsl_saturation, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gHSL_transition_tick;

                if (_get_time_in_ms(gHSL_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(gHSL_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 7;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &gHSL_server->lightness_server.target_lightness, 2);
                memcpy(&reply[2], &gHSL_server->target_hsl_hue, 2);
                memcpy(&reply[4], &gHSL_server->target_hsl_saturation, 2);
                reply[6] = remaining_time;
            }

            bt_mesh_model_lighting_hsl_target_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_DEFAULT_GET: {
            VD_DEBUG_EVT_PRINTF("LightHSLDefaultGet", 2);

            bt_mesh_model_lighting_hsl_default_status(model_handle, gHSL_server->lightness_server.default_lightness, gHSL_server->default_hue,
                    gHSL_server->default_saturation, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_DEFAULT_SET: {
            VD_DEBUG_EVT_PRINTF("LightHSLDefaultSet", 2);

            memcpy(&gHSL_server->lightness_server.default_lightness, msg->buffer, 2);

            memcpy(&gHSL_server->default_hue, &msg->buffer[2], 2);
            memcpy(&gHSL_server->default_saturation, &msg->buffer[4], 2);

            bt_mesh_model_lighting_hsl_default_status(model_handle, gHSL_server->lightness_server.default_lightness, gHSL_server->default_hue,
                    gHSL_server->default_saturation, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_DEFAULT_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightHSLDefaultSetUnacknowledged", 2);

            memcpy(&gHSL_server->lightness_server.default_lightness, msg->buffer, 2);
            memcpy(&gHSL_server->default_hue, &msg->buffer[2], 2);
            memcpy(&gHSL_server->default_saturation, &msg->buffer[4], 2);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_RANGE_GET: {
            VD_DEBUG_EVT_PRINTF("LightHSLRangeGet", 2);

            bt_mesh_model_lighting_hsl_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, gHSL_server->hue_range_min,
                                                    gHSL_server->hue_range_max, gHSL_server->saturation_range_min,
                                                    gHSL_server->saturation_range_max, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_RANGE_SET: {
            uint16_t min, max;
            VD_DEBUG_EVT_PRINTF("LightHSLRangeSet", 2);

            memcpy(&min, msg->buffer, 2);
            memcpy(&max, &msg->buffer[2], 2);

            if (min > max) {
                return;
            }

            memcpy(&min, &msg->buffer[4], 2);
            memcpy(&max, &msg->buffer[6], 2);

            if (min > max) {
                return;
            }

            memcpy(&gHSL_server->hue_range_min, msg->buffer, 2);
            memcpy(&gHSL_server->hue_range_max, &msg->buffer[2], 2);
            memcpy(&gHSL_server->saturation_range_min, &msg->buffer[4], 2);
            memcpy(&gHSL_server->saturation_range_max, &msg->buffer[6], 2);

            bt_mesh_model_lighting_hsl_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, gHSL_server->hue_range_min,
                                                    gHSL_server->hue_range_max, gHSL_server->saturation_range_min,
                                                    gHSL_server->saturation_range_max, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_HSL_RANGE_SET_UNACKNOWLEDGED: {
            uint16_t min, max;
            VD_DEBUG_EVT_PRINTF("LightHSLRangeSetUnacknowledged", 2);

            memcpy(&min, msg->buffer, 2);
            memcpy(&max, &msg->buffer[2], 2);

            if (min > max) {
                return;
            }

            memcpy(&min, &msg->buffer[4], 2);
            memcpy(&max, &msg->buffer[6], 2);

            if (min > max) {
                return;
            }

            memcpy(&gHSL_server->hue_range_min, msg->buffer, 2);
            memcpy(&gHSL_server->hue_range_max, &msg->buffer[2], 2);
            memcpy(&gHSL_server->saturation_range_min, &msg->buffer[4], 2);
            memcpy(&gHSL_server->saturation_range_max, &msg->buffer[6], 2);

            break;
        }

        default:
            VD_DEBUG_EVT_SIG(msg->opcode.opcode, 2);
            bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);
            break;

    }
}

void light_ctl_server_msg_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg)
{
    lighting_lightness_server_t *p_lightness_server = &gCTL_server->lightness_server;
    uint16_t element_idx = bt_mesh_model_get_element_index_by_model_handle(model_handle);

    switch (msg->opcode.opcode) {
        case BT_MESH_MODEL_GENERIC_ONOFF_GET: {
            uint8_t *reply;
            uint8_t length = 0;

            VD_DEBUG_EVT_PRINTF("GenericOnOffGet", 2);

            if (p_lightness_server->onoff_server.transition_time == 0) {
                length = 1;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->onoff_server.present_on_off, 1);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_lightness_server->onoff_server.transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_lightness_server->onoff_server.transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);

                length = 3;
                reply = bt_mesh_app_util_alloc(length);

                reply[0] = p_lightness_server->onoff_server.present_on_off;
                reply[1] = p_lightness_server->onoff_server.target_on_off;
                reply[2] = remaining_time;
            }

            bt_mesh_model_generic_on_off_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONOFF_SET: {

            VD_DEBUG_EVT_PRINTF("GenericOnOffSet", 2);

            _generic_on_off_set_msg_handler(p_lightness_server,
                                            model_handle, msg, arg, false, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONOFF_SET_UNACKNOWLEDGED: {

            VD_DEBUG_EVT_PRINTF("GenericOnOffSetUnacknowledged", 2);

            _generic_on_off_set_msg_handler(p_lightness_server,
                                            model_handle, msg, arg, true, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_LEVEL_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("GenericLevelGet", 2);
            generic_level_server_model_t *p_level_server = NULL;

            if (element_idx == gCTL_temperature_server->element_index) {
                p_level_server = &gCTL_temperature_server->gLevel_server;
            } else {
                p_level_server = &gCTL_server->lightness_server.level_server;
            }

            if (p_level_server->transition_time == 0) {
                length = 2;
            } else {
                length = 5;
            }

            reply = bt_mesh_app_util_alloc(length);
            memcpy(reply, &p_level_server->present_level, 2);

            if (length == 5) {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_level_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_level_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                memcpy(&reply[2], &p_level_server->target_level, 2);
                reply[4] = remaining_time;
            }

            bt_mesh_model_generic_level_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);
            break;
        }
        case BT_MESH_MODEL_GENERIC_LEVEL_SET: {
            VD_DEBUG_EVT_PRINTF("GenericLevelSet", 2);
            if (element_idx == gCTL_temperature_server->element_index) {
                _generic_ctl_temperature_level_set_msg_handler(model_handle, msg, arg, false);
            } else {
                _generic_level_set_msg_handler(p_lightness_server,
                                               model_handle, msg, arg, false, LED_2_CTL);
            }

            break;
        }
        case BT_MESH_MODEL_GENERIC_LEVEL_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericLevelSetUnacknowledged", 2);

            _generic_level_set_msg_handler(p_lightness_server,
                                           model_handle, msg, arg, true, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DELTA_SET: {
            VD_DEBUG_EVT_PRINTF("GenericDeltaSet", 2);

            _generic_delta_set_msg_handler(p_lightness_server,
                                           model_handle, msg, arg, false, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DELTA_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericDeltaSetUnacknowledged", 2);

            _generic_delta_set_msg_handler(p_lightness_server,
                                           model_handle, msg, arg, true, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_MOVE_SET: {
            VD_DEBUG_EVT_PRINTF("GenericMoveSet", 2);

            _generic_move_set_msg_handler(p_lightness_server,
                                          model_handle, msg, arg, false, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_MOVE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericMoveSetUnacknowledged", 2);

            _generic_move_set_msg_handler(p_lightness_server,
                                          model_handle, msg, arg, true, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_GET: {
            VD_DEBUG_EVT_PRINTF("GenericDefaultTransitionTimeGet", 2);

            bt_mesh_model_generic_default_transition_time_status(model_handle,
                    p_lightness_server->dtt_server.default_transition_time, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SET: {
            if ((msg->buffer[0] & 0x3f) == 0x3F) {
                LOG_I(mesh_app, "invalid value [%02x]\n", msg->buffer[0]);
                return;
            }

            p_lightness_server->dtt_server.default_transition_time = msg->buffer[0];
            VD_DEBUG_EVT_PRINTF("GenericDefaultTransitionTimeSet", 2);

            bt_mesh_model_generic_default_transition_time_status(model_handle,
                    p_lightness_server->dtt_server.default_transition_time, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACKNOWLEDGED: {
            if ((msg->buffer[0] & 0x3f) == 0x3F) {
                LOG_I(mesh_app, "invalid value [%02x]\n", msg->buffer[0]);
                return;
            }

            p_lightness_server->dtt_server.default_transition_time = msg->buffer[0];
            VD_DEBUG_EVT_PRINTF("GenericDefaultTransitionTimeSetUnacknowledged", 2);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONPOWERUP_GET: {
            VD_DEBUG_EVT_PRINTF("GenericOnPowerupGet", 2);

            bt_mesh_model_generic_onpowerup_status(model_handle,
                                                   p_lightness_server->onpowerup_server.on_power_up, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONPOWERUP_SET: {
            VD_DEBUG_EVT_PRINTF("GenericOnPowerupSet", 2);

            if (msg->buffer[0] > 2) {
                return;
            }

            p_lightness_server->onpowerup_server.on_power_up = msg->buffer[0];

            bt_mesh_model_generic_onpowerup_status(model_handle,
                                                   p_lightness_server->onpowerup_server.on_power_up, msg);
            break;
        }
        case BT_MESH_MODEL_GENERIC_ONPOWERUP_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("GenericOnPowerupSetUnacknowledged", 2);

            if (msg->buffer[0] > 2) {
                return;
            }

            p_lightness_server->onpowerup_server.on_power_up = msg->buffer[0];
            break;
        }

        case BT_MESH_MODEL_LIGHT_LIGHTNESS_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessSet", 2);

            _light_actual_set_msg_handler(p_lightness_server, model_handle, msg, arg, false, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessSetUnacknowledged", 2);

            _light_actual_set_msg_handler(p_lightness_server, model_handle, msg, arg, true, LED_2_CTL);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightLightnessGet", 2);

            if (p_lightness_server->transition_time == 0) {
                length = 2;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->present_lightness, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_lightness_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_lightness_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);

                length = 5;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->present_lightness, 2);
                memcpy(&reply[2], &p_lightness_server->target_lightness, 2);
                reply[4] = remaining_time;
            }

            bt_mesh_model_lighting_lightness_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessLinearSet", 2);

            _light_linear_set_msg_handler(p_lightness_server, model_handle, msg, arg, false, LED_2_CTL);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessLinearSetUnacknowledged", 2);

            _light_linear_set_msg_handler(p_lightness_server, model_handle, msg, arg, true, LED_2_CTL);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightLightnessLinearGet", 2);

            if (p_lightness_server->target_linear_lightness == p_lightness_server->present_linear_lightness &&
                    p_lightness_server->transition_time == 0) {
                length = 2;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &p_lightness_server->present_linear_lightness, length);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gTransitionTick;

                if (_get_time_in_ms(p_lightness_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(p_lightness_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 5;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &p_lightness_server->present_linear_lightness, 2);
                memcpy(&reply[2], &p_lightness_server->target_linear_lightness, 2);
                reply[4] = remaining_time;

            }

            bt_mesh_model_lighting_lightness_linear_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessDefaultSet", 2);

            memcpy(&p_lightness_server->default_lightness, msg->buffer, 2);

            bt_mesh_model_lighting_lightness_default_status(model_handle, p_lightness_server->default_lightness, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessDefaultSetUnacknowledged", 2);

            memcpy(&p_lightness_server->default_lightness, msg->buffer, 2);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_GET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessDefaultGet", 2);

            bt_mesh_model_lighting_lightness_default_status(model_handle, p_lightness_server->default_lightness, msg);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_SET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessRangeSet", 2);

            uint16_t min, max;

            memcpy(&min, msg->buffer, 2);
            memcpy(&max, &msg->buffer[2], 2);

            if (min < max) {
                memcpy(&p_lightness_server->range_min, msg->buffer, 2);
                memcpy(&p_lightness_server->range_max, &msg->buffer[2], 2);

                _bind_lightness_range(p_lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_RANGE, MESH_MODEL_BINDING_BOTH_VALUE);

                bt_mesh_model_lighting_lightness_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, p_lightness_server->range_min, p_lightness_server->range_max, msg);
            }

            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightLightnessRangeSetUnacknowledged", 2);

            memcpy(&p_lightness_server->range_min, msg->buffer, 2);
            memcpy(&p_lightness_server->range_max, &msg->buffer[2], 2);

            _bind_lightness_range(p_lightness_server, MESH_MODEL_STATE_LIGHTING_LIGHTNESS_RANGE, MESH_MODEL_BINDING_BOTH_VALUE);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_GET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessRangeGet", 2);

            bt_mesh_model_lighting_lightness_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, p_lightness_server->range_min, p_lightness_server->range_max, msg);
            break;
        }
        case BT_MESH_MODEL_LIGHT_LIGHTNESS_LAST_GET: {
            VD_DEBUG_EVT_PRINTF("LightLightnessLastGet", 2);

            bt_mesh_model_lighting_lightness_last_status(model_handle, p_lightness_server->last_lightness, msg);
            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightCTLGet", 2);

            if (gCTL_server->lightness_server.target_lightness == gCTL_server->lightness_server.present_lightness &&
                    gCTL_server->target_ctl_temperature == gCTL_server->present_ctl_temperature &&
                    gCTL_server->transition_time == 0) {
                length = 4;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &gCTL_server->lightness_server.present_lightness, 2);
                memcpy(&reply[2], &gCTL_server->present_ctl_temperature, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gCTL_transition_tick;

                if (_get_time_in_ms(gCTL_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(gCTL_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 9;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &gCTL_server->lightness_server.present_lightness, 2);
                memcpy(&reply[2], &gCTL_server->present_ctl_temperature, 2);
                memcpy(&reply[4], &gCTL_server->lightness_server.target_lightness, 2);
                memcpy(&reply[6], &gCTL_server->target_ctl_temperature, 2);
                reply[8] = remaining_time;
            }

            bt_mesh_model_lighting_ctl_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);
            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_SET: {
            VD_DEBUG_EVT_PRINTF("LightCTLSet", 2);

            _light_ctl_set_msg_handler(model_handle, msg, arg, false);
            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightCTLSetUnacknowledged", 2);

            _light_ctl_set_msg_handler(model_handle, msg, arg, true);
            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_GET: {
            uint8_t *reply;
            uint8_t length = 0;
            VD_DEBUG_EVT_PRINTF("LightCTLTemperatureGet", 2);

            if (gCTL_temperature_server->TID == msg->buffer[4]) {
                return;
            }

            gCTL_temperature_server->TID = msg->buffer[4];

            if (gCTL_temperature_server->target_ctl_temperature == gCTL_temperature_server->present_ctl_temperature &&
                    gCTL_temperature_server->target_ctl_delta_uv == gCTL_temperature_server->present_ctl_delta_uv &&
                    gCTL_temperature_server->transition_time == 0) {
                length = 4;
                reply = bt_mesh_app_util_alloc(length);

                memcpy(reply, &gCTL_temperature_server->present_ctl_temperature, 2);
                memcpy(&reply[2], &gCTL_temperature_server->present_ctl_delta_uv, 2);
            } else {
                uint32_t ticks = xTaskGetTickCount() * portTICK_PERIOD_MS - gCTL_transition_tick;

                if (_get_time_in_ms(gCTL_temperature_server->transition_time) > ticks) {
                    ticks = _get_time_in_ms(gCTL_temperature_server->transition_time) - ticks;
                } else {
                    ticks = 0;
                }

                uint32_t remaining_time = _get_time_in_transition_time_state_format(ticks);
                length = 9;

                reply = bt_mesh_app_util_alloc(length);
                memcpy(reply, &gCTL_temperature_server->present_ctl_temperature, 2);
                memcpy(&reply[2], &gCTL_temperature_server->present_ctl_delta_uv, 2);
                memcpy(&reply[4], &gCTL_temperature_server->target_ctl_temperature, 2);
                memcpy(&reply[6], &gCTL_temperature_server->target_ctl_delta_uv, 2);
                reply[8] = remaining_time;
            }

            bt_mesh_model_lighting_ctl_temperature_status(model_handle, reply, length, msg);

            bt_mesh_app_util_free(reply);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SET: {
            VD_DEBUG_EVT_PRINTF("LightCTLTemperatureSet", 2);

            _light_ctl_temperature_set_msg_handler(model_handle, msg, arg, false);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightCTLTemperatureSetUnacknowledged", 2);

            _light_ctl_temperature_set_msg_handler(model_handle, msg, arg, true);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_GET: {
            VD_DEBUG_EVT_PRINTF("LightCTLTemperatureRangeGet", 2);

            bt_mesh_model_lighting_ctl_temperature_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, gCTL_server->range_min, gCTL_server->range_max, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_SET: {
            uint16_t min, max;
            VD_DEBUG_EVT_PRINTF("LightCTLTemperatureRangeSet", 2);

            memcpy(&min, msg->buffer, 2);
            memcpy(&max, &msg->buffer[2], 2);

            if ((min > max) || (max > 0x4E20) || (min < 0x0320)) {
                LOG_I(mesh_app, "invalid temperature value\n");
                return;
            }

            memcpy(&gCTL_server->range_min, msg->buffer, 2);
            memcpy(&gCTL_server->range_max, &msg->buffer[2], 2);

            bt_mesh_model_lighting_ctl_temperature_range_status(model_handle, BT_MESH_MODEL_STATUS_SUCCESS, gCTL_server->range_min, gCTL_server->range_max, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED: {
            VD_DEBUG_EVT_PRINTF("LightCTLTemperatureRangeSetUnacknowledged", 2);

            memcpy(&gCTL_server->range_min, msg->buffer, 2);
            memcpy(&gCTL_server->range_max, &msg->buffer[2], 2);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_DEFAULT_GET: {
            VD_DEBUG_EVT_PRINTF("LightCTLDefaultGet", 2);

            bt_mesh_model_lighting_ctl_default_status(model_handle, gCTL_server->lightness_server.default_lightness, gCTL_server->default_temperature, gCTL_server->default_delta_uv, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_DEFAULT_SET: {
            uint16_t default_temperature;
            VD_DEBUG_EVT_PRINTF("LightCTLDefaultSet", 2);

            memcpy(&default_temperature, &msg->buffer[2], 2);
            if (default_temperature > 0x4E20 || default_temperature < 0x320) {
                return;
            }

            memcpy(&gCTL_server->lightness_server.default_lightness, msg->buffer, 2);
            memcpy(&gCTL_server->default_temperature, &msg->buffer[2], 2);
            memcpy(&gCTL_server->default_delta_uv, &msg->buffer[4], 2);

            bt_mesh_model_lighting_ctl_default_status(model_handle, gCTL_server->lightness_server.default_lightness, gCTL_server->default_temperature, gCTL_server->default_delta_uv, msg);

            break;
        }
        case BT_MESH_MODEL_LIGHT_CTL_DEFAULT_SET_UNACKNOWLEDGED: {
            uint16_t default_temperature;
            VD_DEBUG_EVT_PRINTF("LightCTLDefaultSetUnacknowledged", 2);

            memcpy(&default_temperature, &msg->buffer[2], 2);
            if (default_temperature > 0x4E20 || default_temperature < 0x320) {
                return;
            }

            memcpy(&gCTL_server->lightness_server.default_lightness, msg->buffer, 2);
            memcpy(&gCTL_server->default_temperature, &msg->buffer[2], 2);
            memcpy(&gCTL_server->default_delta_uv, &msg->buffer[4], 2);

            break;
        }
        default:
            VD_DEBUG_EVT_SIG(msg->opcode.opcode, 2);
            bt_mesh_app_util_display_hex_log(&msg->buffer[0], msg->length);
            break;

    }
}

