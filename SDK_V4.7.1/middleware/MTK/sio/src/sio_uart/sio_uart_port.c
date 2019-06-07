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


#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "syslog.h"
#include "memory_attribute.h"
#include "sio_uart_port.h"
#include "atci.h"
#include "hal_uart.h"
#include "hal_pdma_internal.h"
#include "hal_uart_internal.h"
#include "hal_gpio.h"
#include "sio_gprot.h"
#include "hal_gpt.h"
#include "sio_uart_adapter.h"

//#define SIO_UART_PORT_PRINTF
#ifdef SIO_UART_PORT_PRINTF
#define LOGE(fmt,arg...)   printf(("[SIO UART]: "fmt), ##arg)
#define LOGW(fmt,arg...)   printf(("[SIO UART]: "fmt), ##arg)
#define LOGI(fmt,arg...)   printf(("[SIO UART]: "fmt), ##arg)
#else
log_create_module(sio_port, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(sio_port, "[SIO UART]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(sio_port, "[SIO UART]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(sio_port ,"[SIO UART]: "fmt,##arg)
#endif


// TODO: TCPIP data transmit from uart AT cmd. The data length is 2048.
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static sio_uart_context_buf_struct g_sio_uart_cntx;
sio_uart_input_cmd_msg_t g_sio_uart_msg[SIO_UART_MSG_NUM];
sio_reass_buff_t g_reass_buff;

void sio_uart_port_irq(hal_uart_callback_event_t event, void *parameter);
#ifdef MODEM_SUPPORT_DTE
static void sio_uart_dte_irq(hal_uart_dte_callback_event_t event, void *user_data);
static void sio_uart_dte_irq(hal_uart_dte_callback_event_t event, void *user_data)
{
    LOGI("%d", event);
    
	switch (event) {
        case HAL_UART_DTE_EVENT_DSR_ACTIVE: {
            break;
        }
        case HAL_UART_DTE_EVENT_DSR_DEACTIVE: {
            /* user's handler code after receive DSR event */
             break;
        }
        case HAL_UART_DTE_EVENT_DCD_ACTIVE:
        case HAL_UART_DTE_EVENT_DCD_DEACTIVE:
            /* user's handler code after receive DCD event */
            break;
        case HAL_UART_DTE_EVENT_RI_HIGH: {
            LOGI("6280 RI 2523 SLEEP");
            /*if (sio_uart_get_status() == SIO_UART_STATUS_READY)
                sio_uart_eint_modem_sleep_handler();*/
            break;
        }

        case HAL_UART_DTE_EVENT_RI_LOW:
            /* RI pattern judgement(MT_CALL/NEW MSG/URC) code */
            /* user's handler code after receive RI event(MT_CALL/NEW MSG/URC) */
            LOGI("6280 RI 2523 WAKEUP");
            /*if (sio_uart_get_status() == SIO_UART_STATUS_READY)
                sio_uart_eint_modem_wakeup_handler();*/
            break;
        default:
            break;
    }
}
#endif

/*------ Extern function Entity ------*/
sio_uart_ret_t sio_uart_port_init(hal_uart_port_t uart_port)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;
    uint8_t num;
#ifdef MODEM_SUPPORT_DTE
    hal_uart_dte_config_t uart_dte_config;
#endif

    SIO_UART(modem_port) = uart_port;

    LOGI("sio_uart_port_init, port=%d \r\n", SIO_UART(modem_port));

    for (num = 0; num < SIO_UART_MSG_NUM; num ++) {
        g_sio_uart_msg[num].is_used = 0;
    }
    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;//HAL_UART_BAUDRATE_921600;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    if (HAL_UART_STATUS_OK != hal_uart_init(SIO_UART(modem_port), &uart_config)) {
        LOGW("hal_uart_init fail\r\n");
        return SIO_UART_RET_ERROR;
    } else {
        LOGW("hal_uart_init success \n");
        dma_config.receive_vfifo_alert_size = SIO_UART_RX_FIFO_ALERT_SIZE;
        dma_config.receive_vfifo_buffer = g_sio_uart_cntx.rx_buff;
        dma_config.receive_vfifo_buffer_size = SIO_UART_RX_FIFO_BUFFER_SIZE;
        dma_config.receive_vfifo_threshold_size = SIO_UART_RX_FIFO_THRESHOLD_SIZE;
        dma_config.send_vfifo_buffer = g_sio_uart_cntx.tx_buff;
        dma_config.send_vfifo_buffer_size = SIO_UART_TX_FIFO_BUFFER_SIZE;
        dma_config.send_vfifo_threshold_size = SIO_UART_TX_FIFO_THRESHOLD_SIZE;

        if (HAL_UART_STATUS_OK != hal_uart_set_dma(SIO_UART(modem_port), &dma_config)) {
            return SIO_UART_RET_ERROR;
        }

        hal_uart_set_software_flowcontrol(SIO_UART(modem_port), 0x11, 0x13, 0x00);

        if (HAL_UART_STATUS_OK != hal_uart_register_callback(SIO_UART(modem_port), sio_uart_port_irq, NULL)) {
            return SIO_UART_RET_ERROR;
        }
    }
#ifdef MODEM_SUPPORT_DTE
    uart_dte_config.dcd_debounce_time = 0;
	uart_dte_config.dcd_is_active_high = false;
	uart_dte_config.dsr_debounce_time = 0;
	uart_dte_config.dsr_is_active_high = false;
	uart_dte_config.ri_debounce_time = 0;
    uart_dte_config.dtr_is_active_high = false;
	uart_dte_config.user_callback = sio_uart_dte_irq;
	uart_dte_config.user_data = NULL;
    if (hal_uart_set_dte_config(SIO_UART(modem_port), &uart_dte_config) != HAL_UART_STATUS_OK) {
		LOGW("hal_uart_set_dte_config fail \n");
        return SIO_RET_ERROR;
	}
 #endif
    return SIO_UART_RET_OK;
}

sio_uart_ret_t sio_uart_port_deinit(void)
{
    hal_uart_status_t uart_ret = HAL_UART_STATUS_ERROR;
    sio_uart_ret_t ret = SIO_UART_RET_ERROR;

    uart_ret = hal_uart_deinit(SIO_UART(modem_port));

    LOGW("sio_uart_port_deinit() uart_ret = %d \r\n", uart_ret);

    if (uart_ret == HAL_UART_STATUS_OK) {
        memset(g_sio_uart_msg, 0, sizeof(g_sio_uart_msg));
        ret = SIO_UART_RET_OK;
    }

    return ret;
}

sio_uart_ret_t sio_copy_data(char *dest, char *src, int len)
{
    int i;
    if (len >= SIO_UART_RX_FIFO_BUFFER_SIZE) {
        LOGW("sio_copy_data, len > SIO_UART_RX_FIFO_BUFFER_SIZE");
        return SIO_UART_RET_ERROR;
    }

    for(i = 0; i < len; i++) {
        dest[i] = src[i];
    }

    return SIO_UART_RET_OK;
}

void sio_emit_uart_data(char *data, int len)
{
    int num;
    sio_uart_input_cmd_msg_t *input = NULL;
    sio_uart_general_msg_t msg_queue_item;

    if (SIO_UART(is_trigger_modem_ready) == 1) {
        if (0 == strncasecmp(data, "OK", strlen("OK"))){
            SIO_UART(is_trigger_modem_ready) = 0;
            msg_queue_item.msg_id = MSG_ID_SIO_MODEM_READY_IND;
            msg_queue_item.msg_data = NULL;
            LOGI("modem ready by at response");
            sio_uart_queue_send_from_isr(SIO_UART(input_queue), (void *)&msg_queue_item);
            sio_uart_trigger_modem_sleep();
            return;
        }
    }

    if(data[0] != '+' && SIO_UART(rtx_data) == NULL) {
        LOGE("garbage data %s, drop it \r\n", data);
        return;
    }

    for (num = 0; num < SIO_UART_MSG_NUM; num ++) {
        if (g_sio_uart_msg[num].is_used == 0) {
            input = &g_sio_uart_msg[num];
            memset(input, 0, sizeof(sio_uart_input_cmd_msg_t));
            break;
        }
    }

    if (num == SIO_UART_MSG_NUM) {
        LOGE("uart message buffer full, drop package \r\n");
        return;
    }

    if (SIO_UART_RET_OK == sio_copy_data((char*)input->buf, data, len)) {
        input->len = len;
        input->is_used = 1;
        msg_queue_item.msg_id = MSG_ID_SIO_UART_IND;
        msg_queue_item.msg_data = (void *)input;
        LOGI("[input] emit data len %d %s", len, data);
        sio_uart_queue_send_from_isr(SIO_UART(input_queue), (void *)&msg_queue_item);
    }
}


void sio_uart_port_irq(hal_uart_callback_event_t event, void *parameter)
{
    sio_uart_event_callback_struct *h = SIO_UART(uart_event_cb_list);
    
    LOGI("event %d", event);

    // user can receive UART EVENT directly to call sio_uart_set_uart_event_callback
    while (h) {
        LOGI("event id = %d, ptr = %p -- \r\n", event, h->func);
        
        if (h->func(event, parameter) == SIO_UART_RET_CONTINUE) {
            h = h->next;
        }
        else {
            LOGI("-------  finish event id = %d, ptr = %p stop by user ------- \r\n", event, h->func);
            return;
        }
    }

    if (HAL_UART_EVENT_READY_TO_READ == event || HAL_UART_EVENT_TRANSACTION_ERROR == event) {
        char *data;
        int16_t i;
        int16_t count = 0;
        int16_t start = 0;
        int16_t end = 0;
        int16_t write_len;
        int16_t len = 0;

        if (g_reass_buff.state == INPUT_STATE_START) {
            data = g_reass_buff.buf + g_reass_buff.len;
            g_reass_buff.state = INPUT_STATE_NONE;
        } else {
            data = g_reass_buff.buf;
            g_reass_buff.len = 0;
        }

        write_len = sio_uart_port_read_data(SIO_UART(modem_port), (uint8_t*)data, SIO_UART_RX_FIFO_BUFFER_SIZE - g_reass_buff.len);
        g_reass_buff.len += write_len;
        len = g_reass_buff.len;
        data = g_reass_buff.buf;
        i = 0;

        while (i < len) {
            if(g_reass_buff.state == INPUT_STATE_NONE) {
                for(; i < len; i++) {
                    if (data[i] > 31 && data[i] < 127) {
                        g_reass_buff.state = INPUT_STATE_START;
                        start = i;
                        break;
                    }
                }
            }

            if(g_reass_buff.state == INPUT_STATE_START) {
                for(; i < len; i++) {
                    if (data[i] < 32 || data[i] > 126) {
                        g_reass_buff.state = INPUT_STATE_END;
                        end = i;
                        break;
                    }
                }
            }

            if (g_reass_buff.state == INPUT_STATE_END && end - start > 0) {
                sio_emit_uart_data(&data[start],  end - start);
                g_reass_buff.state = INPUT_STATE_NONE;
                start = 0;
                g_reass_buff.send_flag = 1;
            } else {
                if (g_reass_buff.state == INPUT_STATE_END && end - start == 0) {
                    LOGI("error 1 !!!! end %d start %d", end, start);
                    g_reass_buff.state = INPUT_STATE_NONE;
                    g_reass_buff.send_flag = 0;
                    memset(g_reass_buff.buf, 0, sizeof(g_reass_buff.buf));
                    g_reass_buff.len = 0;
                    break;
                }
            }

            count++;
            
            if (count > 20) {
                LOGI("error 2 !!!!");
                g_reass_buff.state = INPUT_STATE_NONE;
                g_reass_buff.send_flag = 0;
                memset(g_reass_buff.buf, 0, sizeof(g_reass_buff.buf));
                g_reass_buff.len = 0;
                break;
            }
        }

        if (g_reass_buff.send_flag == 1 && g_reass_buff.state == INPUT_STATE_START) {
            int16_t left_len = len - end;
            if(left_len > 0) {
                sio_copy_data(&data[0], &data[end], left_len);
            }
            data[left_len] = '\0';
            g_reass_buff.len = left_len;
            g_reass_buff.send_flag = 0;
        }

        if (g_reass_buff.state == INPUT_STATE_NONE && start == 0) {
            memset(data, 0, len);
            g_reass_buff.len = 0;
        }

        if (HAL_UART_EVENT_TRANSACTION_ERROR == event) {            
            hal_uart_status_t uart_ret = HAL_UART_STATUS_ERROR;    
            hal_uart_config_t uart_config;
            hal_uart_dma_config_t dma_config; 
        #ifdef MODEM_SUPPORT_DTE
            hal_uart_dte_config_t uart_dte_config;
        #endif   
            
            uart_ret = hal_uart_deinit(SIO_UART(modem_port));

            if (uart_ret != HAL_UART_STATUS_OK) {
                LOGW("hal_uart_deinit fail\r\n");
                return;
            }
            
            /* Configure UART PORT */
            uart_config.baudrate = HAL_UART_BAUDRATE_921600;
            uart_config.parity = HAL_UART_PARITY_NONE;
            uart_config.stop_bit = HAL_UART_STOP_BIT_1;
            uart_config.word_length = HAL_UART_WORD_LENGTH_8;

            if (HAL_UART_STATUS_OK != hal_uart_init(SIO_UART(modem_port), &uart_config)) {
                LOGW("hal_uart_init fail\r\n");
                return;
            } else {
                LOGI("hal_uart_init success \n");
                dma_config.receive_vfifo_alert_size = SIO_UART_RX_FIFO_ALERT_SIZE;
                dma_config.receive_vfifo_buffer = g_sio_uart_cntx.rx_buff;
                dma_config.receive_vfifo_buffer_size = SIO_UART_RX_FIFO_BUFFER_SIZE;
                dma_config.receive_vfifo_threshold_size = SIO_UART_RX_FIFO_THRESHOLD_SIZE;
                dma_config.send_vfifo_buffer = g_sio_uart_cntx.tx_buff;
                dma_config.send_vfifo_buffer_size = SIO_UART_TX_FIFO_BUFFER_SIZE;
                dma_config.send_vfifo_threshold_size = SIO_UART_TX_FIFO_THRESHOLD_SIZE;

                if (HAL_UART_STATUS_OK != hal_uart_set_dma(SIO_UART(modem_port), &dma_config)) {
                    LOGW("hal_uart_set_dma fail\r\n");
                    return;
                }

                if (HAL_UART_STATUS_OK != hal_uart_register_callback(SIO_UART(modem_port), sio_uart_port_irq, NULL))
                    LOGW("hal_uart_register_callback fail\r\n"); {
                    return;
                }
            }
        #ifdef MODEM_SUPPORT_DTE
            uart_dte_config.dcd_debounce_time = 1;
        	uart_dte_config.dcd_is_active_high = false;
        	uart_dte_config.dsr_debounce_time = 1;
        	uart_dte_config.dsr_is_active_high = false;
        	uart_dte_config.ri_debounce_time = 1;
            uart_dte_config.dtr_is_active_high = false;
        	uart_dte_config.user_callback = sio_uart_dte_irq;
        	uart_dte_config.user_data = NULL;
            if (hal_uart_set_dte_config(SIO_UART(modem_port), &uart_dte_config) != HAL_UART_STATUS_OK) {
        		LOGW("hal_uart_set_dte_config fail \n");
                return;
        	}
         #endif
        }
    } else if (HAL_UART_EVENT_READY_TO_WRITE == event) {
        // do nothing
    }
}


uint32_t sio_uart_port_read_data(hal_uart_port_t uart_index, uint8_t *buf, uint32_t buf_len)
{
    return hal_uart_receive_dma(uart_index, (uint8_t *)buf, (uint32_t) buf_len);
}

uint32_t sio_uart_port_write_data(hal_uart_port_t uart_index, uint8_t *buf, uint32_t buf_len)
{
    uint32_t ret_len = 0;
    uint32_t send_len = 0;
    uint8_t* send_buf = NULL;
    uint8_t* context = NULL;
    int context_len = 40;
    
    LOGI("write data via port:%d \r\n", uart_index);
    
    if (context_len > buf_len)
        context_len = buf_len;

    if (buf_len > SIO_TX_DATA_BUFFER_SIZE) {
        send_len = SIO_TX_DATA_BUFFER_SIZE;
    } else {
        send_len = buf_len;
    }
    send_buf = buf;

    while (send_len) {
        ret_len = hal_uart_send_dma(uart_index, (const uint8_t *)send_buf, send_len);
        if (send_len > ret_len) {
            send_len -= ret_len;
            send_buf += ret_len;
        } else {
            if (send_len < ret_len) {
                LOGE("[output] error send_len < ret_len");
                return -1;
            }
            
            break;
        } 
    }

    context = pvPortMalloc(context_len + 1);
    strncpy((char*)context, (char*)buf, context_len);
    context[context_len] = '\0';
    LOGI("[output] buf %s, buf_len %d \r\n", context, buf_len);
    vPortFree(context);

    return buf_len;
}

/* Parse input command */
sio_uart_ret_t sio_uart_port_msg_handler(sio_uart_input_cmd_msg_t *input)
{
    sio_uart_ret_t ret = SIO_UART_RET_OK;
    const char *modemReadyCmd = "+EIND: 128";
    char *temp = (char *)input->buf;
    uint8_t is_end;

    sio_uart_mutex_lock(SIO_UART(mutex));

    if (0 == strncasecmp(temp, modemReadyCmd, strlen(modemReadyCmd))) {
        /* if modem exception timer is running, delete it */
        sio_uart_timer_delete(SIO_UART_TIMER_MODEM_EXCEPTION);
        /* modem ready, event notification to register */
        sio_uart_set_status(SIO_UART_STATUS_READY);
        sio_uart_trigger_modem_sleep();
        ret = sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_READY, NULL);
    } else if (1 == sio_uart_port_parse_modem_response(temp, &is_end)) {
        LOGI("[input] response: %s %d", temp, is_end);
        /* Setup bearer at command response */
        ret = sio_uart_at_response_handler(input->buf, input->len, is_end);
    } else if (*temp == '+') {
        if (SIO_UART(urc_cb) != NULL) {
            char* context = NULL;
            context = pvPortMalloc(40);
            strncpy((char*)context, (char*)input->buf, 39);
            context[39] = '\0';
            LOGI("[input] URC (buf:%s len:%d) \r\n", context, input->len);
            ret = sio_uart_event_proxy_handler(SIO_UART_EVENT_MODEM_URC, NULL);
            if (SIO_UART(urc_cb)) {
                SIO_UART(urc_cb)(input->buf, input->len);
            }
            vPortFree(context);
        }
    } else {
        /* AT command response */
        ret = sio_uart_at_response_handler(input->buf, input->len, 1);
    }

    /* free memory */
    input->is_used = 0;

    sio_uart_mutex_unlock(SIO_UART(mutex));

    return ret;
}

int8_t sio_uart_port_parse_modem_response(char *temp, uint8_t* is_end)
{    
    sio_modem_response_struct* current_modem_response = SIO_UART(modem_response_list);
     
    *is_end = 0;
     
    while (current_modem_response) {
        if (0 == strncasecmp(temp, current_modem_response->modem_response, 
                strlen(current_modem_response->modem_response))) {
            *is_end = current_modem_response->is_end;
            return 1;
        }

        current_modem_response = current_modem_response->next;
    }

    return 0;
}

void sio_uart_add_modem_response(const char* modem_response, uint8_t is_end)
{
    char* data = NULL;
    sio_modem_response_struct* modem_response_t =  SIO_UART(modem_response_list);
    sio_modem_response_struct* new_modem_response_t;

    data = sio_uart_mem_alloc(strlen(modem_response));
    if (data == NULL)        
        configASSERT(0);
    memset(data, 0, strlen(modem_response));
    memcpy(data, modem_response, strlen(modem_response));
    
    new_modem_response_t = sio_uart_mem_alloc(sizeof(sio_modem_response_struct));
    if (new_modem_response_t == NULL)        
        configASSERT(0);
    memset(new_modem_response_t, 0, sizeof(sio_modem_response_struct));

    new_modem_response_t->is_end = is_end;
    new_modem_response_t->modem_response = data;
    new_modem_response_t->next = NULL;

    if (SIO_UART(modem_response_list)) {
        while(modem_response_t->next) {
            modem_response_t = modem_response_t->next;
        }

        modem_response_t->next = new_modem_response_t;
    } else {
        SIO_UART(modem_response_list) = new_modem_response_t;
    }    
}

