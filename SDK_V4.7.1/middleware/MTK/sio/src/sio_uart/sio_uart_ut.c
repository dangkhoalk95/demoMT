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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "syslog.h"

#include "sio_uart_internal.h"

#include <toi.h>

//#define SIO_UART_UT_PRINTF

#ifdef SIO_UART_UT_PRINTF
#define LOGE(fmt,arg...)   printf(("sio uart ut: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("sio uart ut: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("sio uart ut: "fmt), ##arg)
#else
#define LOGE(fmt,arg...)   LOG_E(sio, "sio uart ut: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(sio, "sio uart ut: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(sio ,"sio uart ut: "fmt,##arg)
#endif

#ifdef SIO_UART_UT
static sio_uart_input_cmd_msg_t g_sio_uart_ut_uart_msg[SIO_UART_MSG_NUM];
int host_status = 0;

sio_uart_ret_t sio_uart_ut_uart_handle(void)
{
    sio_uart_status_t status;

    sio_uart_mutex_lock(SIO_UART(mutex));

    status = sio_uart_get_status();

    LOGI("sio_uart_ut_uart_handle status:%d\r\n", status);

    switch (status) {
        case SIO_UART_STATUS_EXCEPTION:
            // msm_ut_exception_status();
            break;
        case SIO_UART_STATUS_INIT:
            sio_uart_ut_init_status();
            break;
        case SIO_UART_STATUS_READY:
            sio_uart_ut_ready_status();
            break;
    }

    sio_uart_mutex_unlock(SIO_UART(mutex));

    return SIO_UART_RET_OK;
}

void sio_uart_ut_init_status(void)
{
    const char *modemReadyCmd = "\r\n+EIND: 128";

    sio_uart_general_msg_t msg_queue_item;
    sio_uart_input_cmd_msg_t *input;
    uint8_t num = 0;

    sio_uart_timer_delete(SIO_UART_TIMER_MODEM_EXCEPTION);

    for (num = 0; num < SIO_UART_MSG_NUM; num ++) {
        if (g_sio_uart_ut_uart_msg[num].is_used == 0) {
            input = &g_sio_uart_ut_uart_msg[num];
            memset(input, 0, sizeof(sio_uart_input_cmd_msg_t));
            break;
        }
    }

    if (num == SIO_UART_MSG_NUM) {
        LOGE("uart message buffer full, discard package\r\n");
        configASSERT(0);
    }

    memcpy(input->buf, modemReadyCmd, strlen(modemReadyCmd));
    input->len = strlen(modemReadyCmd);
    input->is_used = 1;

    LOGI("sio_uart_ut_init_status input cmd (len:%d):%s\r\n", input->len, input->buf);

    msg_queue_item.msg_id = MSG_ID_MSM_UART_IND;
    msg_queue_item.msg_data = (void *)input;
    LOGI("sio_uart_ut_init_status, send message(%d) to MSM task \r\n", msg_queue_item.msg_id);
    sio_uart_queue_send(SIO_UART(input_queue), (void *)&msg_queue_item);
    sio_uart_semaphore_give(SIO_UART(semaphore));
}

void sio_uart_ut_ready_status(void)
{
    const char *CGPADDR = "+CGPADDR: 1, \"10.186.49.243\"";
    const char *CGPRCO = "+CGPRCO: 1, \"183.221.253.100\", \"223.87.253.100\", \"\", \"\"";
    const char *OK = "OK";
    const char *PINGRESULT = "454950444154413a312c36302c224500003c00000000ff01244ac0a80101b461216c0800574eafaf0001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f22";
    sio_uart_general_msg_t msg_queue_item;
    sio_uart_input_cmd_msg_t *input;
    uint8_t num = 0;

    sio_uart_timer_delete(SIO_UART_TIMER_MODEM_EXCEPTION);

    for (num = 0; num < SIO_UART_MSG_NUM; num ++) {
        if (g_sio_uart_ut_uart_msg[num].is_used == 0) {
            input = &g_sio_uart_ut_uart_msg[num];
            memset(input, 0, sizeof(sio_uart_input_cmd_msg_t));
            break;
        }
    }

    if (num == SIO_UART_MSG_NUM) {
        LOGE("uart message buffer full, discard package \r\n");
        configASSERT(0);
    }
#if 0
    sio_uart_tx_data_struct *tx_data = SIO_UART(tx_data);

    if (NULL == tx_data) {
        LOGE("sio_uart_ut_ready_status tx_data is NULL \r\n");
        configASSERT(0);
    } else {
        LOGI("sio_uart_ut_ready_status output cmd (len:%d):%s\r\n", tx_data->len, tx_data->buf);
    }

    if (strstr(tx_data->buf, "at+cgerep") ||
            strstr(tx_data->buf, "at+cgact") ||
            strstr(tx_data->buf, "at+cgdcont") ||
            strstr(tx_data->buf, "at+cgdata") ||
            strstr(tx_data->buf, "at+erat")  ||
            strstr(tx_data->buf, "ate")) {
        memcpy(input->buf, OK, strlen(OK));
        input->len = strlen(OK);
    } else if (strstr(tx_data->buf, "at+cgpaddr")) {
        memcpy(input->buf, CGPADDR, strlen(CGPADDR));
        input->len = strlen(CGPADDR);
    } else if (strstr(tx_data->buf, "at+cgprco")) {
        memcpy(input->buf, CGPRCO, strlen(CGPRCO));
        input->len = strlen(CGPRCO);
    } else if (strstr(tx_data->buf, "AT+EIPDATA")) {
        memcpy(input->buf, OK, strlen(OK));
        input->len = strlen(OK);
    } else {
        configASSERT(0);
    }
    input->is_used = 1;

    LOGI("sio_uart_ut_ready_status input cmd (len:%d):%s \r\n", input->len, input->buf);

    /* send the message to msm_local_common_queue */
    msg_queue_item.msg_id = MSG_ID_MSM_UART_IND;
    msg_queue_item.msg_data = (void *)input;
    LOGI("sio_uart_ut_ready_status, send message(%d) to MSM task \r\n", msg_queue_item.msg_id);
    sio_uart_queue_send(SIO_UART(input_queue), (void *)&msg_queue_item);
    sio_uart_semaphore_give(SIO_UART(semaphore));

    // Monitor ping result after receiving "AT+EIPDATA" response "OK"
    if (strstr(tx_data->buf, "AT+EIPDATA")) {
        for (num = 0; num < SIO_UART_MSG_NUM; num ++) {
            if (g_sio_uart_ut_uart_msg[num].is_used == 0) {
                input = &g_sio_uart_ut_uart_msg[num];
                memset(input, 0, sizeof(sio_uart_input_cmd_msg_t));
                break;
            }
        }

        if (num == SIO_UART_MSG_NUM) {
            LOGE("uart message buffer full, discard package \r\n");
            configASSERT(0);
        }

        memcpy(input->buf, PINGRESULT, strlen(PINGRESULT));
        input->len = strlen(PINGRESULT);
        input->is_used = 1;

        /* send the message to msm_local_common_queue */
        msg_queue_item.msg_id = MSG_ID_MSM_UART_IND;
        msg_queue_item.msg_data = (void *)input;
        LOGI("sio_uart_ut_ready_status, send message(%d) to MSM task \r\n", msg_queue_item.msg_id);
        sio_uart_queue_send(SIO_UART(input_queue), (void *)&msg_queue_item);
        sio_uart_semaphore_give(SIO_UART(semaphore));
    }
    #endif
}

void sio_uart_ut_reset_md(void)
{
    LOGI("sio_uart_ut_reset_md\r\n");
    msm_trigger_to_modem(MSM_TRIGGER_TYPE_RESET);
}


void sio_uart_ut_notify_host_status(void)
{
    msm_status_t status = (msm_status_t)host_status % 2;
    LOGI("sio_uart_ut_notify_host_status status(%d)\r\n", status);
    msm_notify_host_status_to_modem(status);
    host_status++;
}

void sio_uart_ut_wakeup_md(void)
{
    LOGI("sio_uart_ut_wakeup_md\r\n");
    msm_trigger_to_modem(MSM_TRIGGER_TYPE_WAKEUP)
}


void sio_uart_ut_sleep_md(void)
{
    LOGI("sio_uart_ut_sleep_md\r\n");
    //msm_gpio_init();
}

void sio_uart_ut_query_md_status(void)
{
    msm_status_t status;
    LOGI("sio_uart_ut_query_md_status\r\n");
    status = msm_query_status_from_modem();
    LOGI("sio_uart_ut_query_md_status status(%d)\r\n", status);
}

void sio_uart_ut_start_timer(void)
{
    sio_uart_timer_create_start(SIO_UART_TIMER_UT);
}

void sio_send_ready_evt_ut()
{
    msm_set_status(SIO_STATUS_READY);
    msm_event_proxy_handler(SIO_EVENT_MODEM_READY, NULL);
#if 0
    while(1) {
        hal_gpt_delay_ms(100);
        hal_uart_set_dte_dtr_active(MSM_UART_PORT);
        hal_gpt_delay_ms(100);
        hal_uart_set_dte_dtr_deactive(MSM_UART_PORT);
    }
#endif
}

void sio_ut_send_data()
{
    const uint8_t *data = "at\r\n";
    uint32_t size = strlen(data);
    LOGI("-----------------------------sio_ut_send_data\r\n");
    LOGI("sio_ut_send_data:%s\r\n", data);
    msm_uart_write_data(MSM_UART_PORT, data, size);
    LOGI("sio_ut_send_data Done\r\n", data);
    LOGI("-----------------------------sio_ut_send_data\r\n");
}


void sio_ut_active_md()
{
    LOGI("-----------------------------sio_ut_active_md\r\n");
    hal_uart_set_dte_dtr_active(MSM_UART_PORT);
    LOGI("MODE1=0x%08x,DIR0=0x%08x,DOUT0=0x%08x,\r\n",
              *(uint32_t *)0xa2020c10,
              *(uint32_t *)0xa2020000,
              *(uint32_t *)0xa2020300);
    vTaskDelay(1000);
    LOGI("-----------------------------sio_ut_active_md\r\n");
}

void sio_ut_deactive_md()
{
    LOGI("-----------------------------sio_ut_deactive_md\r\n");
    hal_uart_set_dte_dtr_deactive(MSM_UART_PORT);
    LOGI("MODE1=0x%08x,DIR0=0x%08x,DOUT0=0x%08x,\r\n",
              *(uint32_t *)0xa2020c10,
              *(uint32_t *)0xa2020000,
              *(uint32_t *)0xa2020300);
    vTaskDelay(1000);
    LOGI("-----------------------------sio_ut_deactive_md\r\n");
}

void sio_ut_reset_md()
{
    LOGI("sio_ut_reset_md\r\n");
    msm_gpio_trigger_reset_to_modem();
    vTaskDelay(1000);
    LOGI("sio_ut_reset_md done\r\n");
}

sio_ret_t sio_wakeup_modem_by_atcmd(void)
{
    //send uart data to activate 6280
    char *data = "at+eslp=0\r\n";
    //char *data = "at\r\n";
    int len = strlen(data);
    int i  = 0;
    g_wake_up_flag = 0;

    //vTaskDelay(2000);
    LOGI("-----------------------------sio_wakeup_modem_by_atcmd\r\n");
    LOGI("wake up md by uart.\r\n");
    while (i < 100) {
    	//LOGI("Send wake up at cmd i:%d.\r\n", i);
        msm_uart_write_data (MSM_UART_PORT, data, len);
        vTaskDelay(20);
        if (g_wake_up_flag == 1) {
            return SIO_RET_OK;
        }
        i++;
    }
    LOGI("-----------------------------sio_wakeup_modem_by_atcmd\r\n");
    return SIO_RET_ERROR;
}


void sio_wakeup_modem_ex(void)
{
    //send uart data to activate 6280
    char *data = "at\r\n";
    int len = strlen(data);
    int i  = 0;

    LOGI("-----------------------------sio_wakeup_modem_ex\r\n");
    LOGI("wake up md by uart.\r\n");
    while (i < 20) {
        LOGI("Send wake up at cmd i:%d.\r\n", i);
        msm_uart_write_data (MSM_UART_PORT, data, len);
        vTaskDelay(50);
    }
    LOGI("-----------------------------sio_wakeup_modem_ex\r\n");
}


void sio_switch_data_mode(void)
{
    //send uart data to activate 6280
    char *data = "at+cuarts=0\r\n";
    int len = strlen(data);

    LOGI("-----------------------------sio_switch_data_mode\r\n");
    LOGI("switch data mode as usb.\r\n");
    msm_uart_write_data (MSM_UART_PORT, data, len);
    vTaskDelay(50);
    LOGI("-----------------------------sio_switch_data_mode\r\n");
}

void sio_sleep_modem()
{
    const uint8_t *data = "at+eslp=1\r\n";
    uint32_t size = strlen(data);
    LOGI("----------sio_sleep_modem:%s----------\r\n", data);
    msm_uart_write_data(MSM_UART_PORT, data, size);
    LOGI("-----------------------------sleep_modem\r\n");
}


#endif /* SIO_UART_UT */

