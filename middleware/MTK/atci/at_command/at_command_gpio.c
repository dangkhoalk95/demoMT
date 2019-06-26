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

// For Register AT command handler
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "hal_gpt.h"
#include "syslog.h"
#include <stdlib.h>

#ifdef HAL_GPIO_MODULE_ENABLED

#include "hal_gpio.h"
#include "hal_gpio_internal.h"

/*
 * sample code
 * AT+EGPIO=GPIO_GET
 * AT+EGPIO=GPIO_SET:<pin number>:<mode><dir><pull><output>
 * AT+EGPIO=GPIO_SET_MODE:<pin number>:<mode>
 * ... ...
*/

log_create_module(atci_gpio, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(atci_gpio, "[GPIO]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_gpio, "[GPIO]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_gpio ,"[GPIO]"fmt,##arg)

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_gpio(atci_parse_cmd_param_t *parse_cmd);


/* AT command handler  */

void atci_gpio_send_response(int32_t* len1, atci_response_t* resonse)
{
        resonse->response_len = (uint16_t)(* len1);
        resonse->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
        atci_send_response(resonse);
}


atci_status_t atci_cmd_hdlr_gpio(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};

    char *param1 = NULL;
    char *param2 = NULL;
    char *param3 = NULL;
    char *param4 = NULL;
    char *param5 = NULL;
    char *param6 = NULL;
    char *param7 = NULL;

    const char *direct[2] = {"input ", \
                             "output"}; 

    const char *current_state[4] = {"4ma ",\
                              "8ma ",\
                              "12ma",\
                              "16ma"}; 
    
    const char *pull_state[8] = {"NO_PULL ", \
                                 "PU_75K  ", \
                                 "PD_75K  ", \
                                 "PU_47K  ", \
                                 "PD_47K  ", \
                                 "PU_23.5K", \
                                 "PD_23.5K", \
                                 "PUPD_Err"};
    
    uint8_t i, error_flag;
    uint8_t temp_buf[256],temp;
    uint8_t temp1[20],temp2[20];
    int32_t ret_len1,ret_len2;
    uint32_t config_data[10];
    
    hal_gpio_pin_t pin_index[2];
    gpio_state_t gpio_state;
    hal_gpio_status_t ret_state;    

    resonse.response_flag = 0; /*    Command Execute Finish.  */
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    resonse.cmd_id = parse_cmd->cmd_id;
    #endif

/*
    sprintf((char *)resonse.response_buf, "mode=%d\r\n",parse_cmd->mode);
    resonse.response_len = strlen((char *)resonse.response_buf);
    resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
    atci_send_response(&resonse);
    */
    
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:

        strcpy((char *)resonse.response_buf, "ATCI_CMD_MODE_TESTING");
        resonse.response_len = strlen((char *)resonse.response_buf);
        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&resonse);

        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "\r\nAT+EGPIO=GPIO_GET:m~n get all  GPIO information\r\n n must >= m, n<%d\r\n",HAL_GPIO_MAX);
        atci_gpio_send_response(&ret_len1,&resonse);

        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "\r\nAT+EGPIO=GPIO_SET_MODE:g,m  set mode to one pin\r\n g:gpio pin number\r\n m:mode number\r\n");   
        atci_gpio_send_response(&ret_len1,&resonse);

        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "\r\nAT+EGPIO=GPIO_SET_DIR:g,d  set dir to one pin\r\n g:gpio pin number\r\n d:direction value, 1-ouput, 0-input\r\n");  
        atci_gpio_send_response(&ret_len1,&resonse);

        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                     "\r\nAT+EGPIO=GPIO_SET_PULL:g,p  set pull state to one pin\r\n g:gpio pin number\r\n p:pull state value, 0-pullup, 1-pulldown,2-disable pull\r\n");   
        atci_gpio_send_response(&ret_len1,&resonse);

        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                     "\r\nAT+EGPIO=GPIO_SET_PUPD:g,pupd,r0,r1  set resistor to one pin\r\n g:gpio pin number\r\n pupd:pull select, 0-pullup, 1-pulldown\r\n r0 or r1:1-select, 0-not select\r\n");  
        atci_gpio_send_response(&ret_len1,&resonse);
        
        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                     "\r\nAT+EGPIO=GPIO_SET_OD:g,d  set output data to one pin\r\n g:gpio pin number\r\n d:0-low, 1-high\r\n"); 
        atci_gpio_send_response(&ret_len1,&resonse);
        
        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                     "\r\nAT+EGPIO=GPIO_SET_DRV:g,drv  set drving current to one pin\r\n g:gpio pin number\r\n drv:0-4ma, 1-8ma, 2-12ma, 3-12ma\r\n");  
        atci_gpio_send_response(&ret_len1,&resonse);
        
        ret_len1 =  snprintf((char *)(resonse.response_buf), ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                     "\r\nAT+EGPIO=GPIO_SET_ALL:g,m,dir,do,p,drv set several configuration to one pin\r\n g:gpio pin number\r\n dir:direction value, 1-ouput, 0-input\r\n do:0-low, 1-high\r\n p:pull state value, 0-pullup, 1-pulldown,2-disable pull\r\n drv:0-4ma, 1-8ma, 2-12ma, 3-12ma\r\n"); 
        atci_gpio_send_response(&ret_len1,&resonse);     

        break;

        case ATCI_CMD_MODE_EXECUTION:
            strcpy((char *)resonse.response_buf, "ATCI_CMD_MODE_EXECUTION ");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&resonse);
            
            /*get specify gpio number state*/
            if (strncmp(parse_cmd->string_ptr, "AT+EGPIO=GPIO_GET:", strlen("AT+EGPIO=GPIO_GET:")) == 0) {

                param1 = strtok(parse_cmd->string_ptr, ":");
                param2 = strtok(NULL, "~");
                param3 = strtok(NULL, "~");

                pin_index[0] = (hal_gpio_pin_t)strtoul(param2,NULL,10);
                pin_index[1] = (hal_gpio_pin_t)strtoul(param3,NULL,10);
     

                if ((pin_index[1] < pin_index[0]) || (pin_index[1]>=HAL_GPIO_MAX)) {
                    snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                        "GPIO get sate input error parameter, your input pin is %d~%d\r\n",pin_index[0],pin_index[1]);
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    break;
                }

                ret_len1 = 0;
                ret_len2 = 0;
                resonse.response_len = 0;
                
                for(i=pin_index[0]; i<=pin_index[1];i++){
                    gpio_get_state((hal_gpio_pin_t)i,&gpio_state);

                    temp = (uint8_t)gpio_state.current_type;

                    strcpy((char *)temp1, (char*)current_state[temp]);
                    strcpy((char *)temp2, (char*)(pull_state[gpio_state.pull_type]));
                            
                    ret_len1 = snprintf((char *)(&temp_buf[0]),\
                                       ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                                       "GPIO%-2d, md=%d, %s, di=%d, do=%d, ", \
                                       (int)i, (int)gpio_state.mode, (char*)direct[gpio_state.dir], (int)gpio_state.din,(int)gpio_state.dout);
                    
                    ret_len2 = snprintf((char *)(&temp_buf[ret_len1]),\
                                       ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                                       "%s, %s\r\n",(char*)temp1,(char*)temp2);
                    
                    /*check buffer if enough, send it*/
                    if ((ATCI_UART_TX_FIFO_BUFFER_SIZE - (resonse.response_len+ret_len1+ret_len2+1)) <= 0) {
                        resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                        atci_send_response(&resonse);
                        resonse.response_len = 0;   
                    }

                    strncpy((char *)(resonse.response_buf + resonse.response_len),(char*)(&temp_buf[0]),(ret_len1+ret_len2));
                    resonse.response_len += (uint16_t)(ret_len1+ret_len2);  
                }

                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                
                break;
            }
            
            param1 = strtok(parse_cmd->string_ptr, ":");
            param2 = strtok(NULL, ",");
            param3 = strtok(NULL, ",");

            strcpy((char *)temp1, (char*)param2);
            strcpy((char *)temp2, (char*)param3);

            pin_index[0]  = (hal_gpio_pin_t)atoi(param2);
            config_data[0] = (hal_gpio_pin_t)atoi(param3);

            if (pin_index[0]>=HAL_GPIO_MAX) {
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "input error parameter, your input pin is %d, GPIO must be 0~%d\r\n",pin_index[0],HAL_GPIO_MAX-1);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                break;
            }
                

            /*  set GPIO mode. */
            if (strncmp(param1, "AT+EGPIO=GPIO_SET_MODE", strlen("AT+EGPIO=GPIO_SET_MODE")) == 0) {

                hal_pinmux_set_function(pin_index[0], config_data[0]);

                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO%d to mode%d done!\r\n",(int)pin_index[0],(int)config_data[0]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                    
                break;
            }
            /*  set GPIO direction. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_DIR", strlen("AT+EGPIO=GPIO_SET_DIR")) == 0) {

                if (config_data[0]>1) {
                    strcpy((char *)resonse.response_buf, "[error] inupt diretion is errror!\r\n 0:intput\r\n 1:output\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    break;
                }
                
                hal_gpio_set_direction(pin_index[0], (hal_gpio_direction_t)config_data[0]);
                
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO%d to %s done!\r\n",pin_index[0],direct[config_data[0]]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                break;
            }

            /*  set GPIO pull state. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_PULL", strlen("AT+EGPIO=GPIO_SET_PULL")) == 0) {
                if (config_data[0]>2) {
                    strcpy((char *)resonse.response_buf, "[error] inupt pull state is errror!\r\n 0:pull up\r\n 1:pull down\r\n 2:disable pull\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    break;
                }
                                
               if (config_data[0] == 0) {
            hal_gpio_pull_up((hal_gpio_pin_t)pin_index[0]);
            }
            else if(config_data[0] == 1){
            hal_gpio_pull_down((hal_gpio_pin_t)pin_index[0]);
         }
         else {
            hal_gpio_disable_pull((hal_gpio_pin_t)pin_index[0]);
            }
                
                gpio_get_state(pin_index[0],&gpio_state);
                
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO%d to %s done!\r\n",pin_index[0],pull_state[gpio_state.pull_type]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                break;
            }

                    
            /*  set GPIO pupd r0 r1 state. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_PUPD", strlen("AT+EGPIO=GPIO_SET_PUPD")) == 0) {

                param4 = strtok(NULL, ",");
                param5 = strtok(NULL, ",");

                config_data[1] = (uint32_t)atoi(param4);
                config_data[2] = (uint32_t)atoi(param5);

                if ((config_data[0]>1)||(config_data[1]>1)||(config_data[2]>1)) {
                    strcpy((char *)resonse.response_buf, \
                        "[error] inupt pupd-r0-r1 is errror!\r\n 0-0-0/1-0-0: Hi-Z\r\n 0-0-1/0-1-0:pull-up 47k\r\n 0-1-1:pull-up 23.5k\r\n 1-0-1/1-1-0:pull-down 47k\r\n 1-1-1:pull-down 23.5K\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    break;
                }
                
                ret_state = hal_gpio_set_pupd_register(pin_index[0],config_data[0],config_data[1],config_data[2]);
                if (ret_state == HAL_GPIO_STATUS_ERROR_PIN) {
                        snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                        "[error]you input pin %d is not the pupd-r0-r1 pin, please refer GPIO datasheet to check\r\n",pin_index[0]);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                        atci_send_response(&resonse);
               break;      
                } 


                gpio_get_state(pin_index[0],&gpio_state);
                
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO%d to %s done!\r\n",pin_index[0],pull_state[gpio_state.pull_type]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                break;
            }
            /*  set GPIO output data. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_OD", strlen("AT+EGPIO=GPIO_SET_OD")) == 0) {
                if (config_data[0]>1) {
                    strcpy((char *)resonse.response_buf, "[error] inupt output data is errror!\r\n 0:low\r\n 1:high\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    break;
                }
                hal_gpio_set_output(pin_index[0], (hal_gpio_data_t)config_data[0]);
                gpio_get_state(pin_index[0],&gpio_state);
                
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO%d output %d done!\r\n",(int)pin_index[0],(int)config_data[0]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                
                break;
            }
            /*  set GPIO current driving. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_DRV", strlen("AT+EGPIO=GPIO_SET_DRV")) == 0) {
                 if (config_data[0]>3) {
                    strcpy((char *)resonse.response_buf, "[error] inupt driving type is errror!\r\n 0:4ma\r\n 1:8ma\r\n 2:12ma\r\n 3:16ma\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    break;
                }
                hal_gpio_set_driving_current(pin_index[0], (hal_gpio_driving_current_t)config_data[0]);
                gpio_get_state(pin_index[0],&gpio_state);
                
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO%d current driving %s done!\r\n",pin_index[0],current_state[config_data[0]]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                
                break;
            }
            /*  set GPIO RG. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_RG", strlen("AT+EGPIO=GPIO_SET_RG")) == 0) {
        
                config_data[0] = (uint32_t)strtoul((char*)temp1,NULL,16);
                config_data[1] = (uint32_t)strtoul((char*)temp2,NULL,16);
                
                *((uint32_t*)(config_data[0])) = config_data[1];
    
                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                    "SET GPIO adress:0x%x = 0x%x done!\r\n",(int)config_data[0],(int)config_data[1]);
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                
                break;
            }      
         
            /* Set serveral configurations to one GPIO. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_ALL", strlen("AT+EGPIO=GPIO_SET_ALL")) == 0) {
                param4 = strtok(NULL, ",");
                param5 = strtok(NULL, ",");
                param6 = strtok(NULL, ",");
                param7 = strtok(NULL, ",");

                config_data[1] = (uint32_t)atoi(param4);  //direction
                config_data[2] = (uint32_t)atoi(param5);  //output data
                config_data[3] = (uint32_t)atoi(param6);  //pull state
                config_data[4] = (uint32_t)atoi(param7);  //current driving

                error_flag = 0;
                if (config_data[1]>1) {
                    strcpy((char *)resonse.response_buf, "[error] inupt diretion is errror!\r\n 0:intput\r\n 1:output\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    error_flag++;
                }

                if (config_data[2]>1) {
                    strcpy((char *)resonse.response_buf, "[error] inupt output data is errror!\r\n 0:low\r\n 1:high\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    error_flag++;
                }

                if (config_data[3]>2) {
                    strcpy((char *)resonse.response_buf, "[error] inupt pull state is errror!\r\n 0:pull up\r\n 1:pull down\r\n 2:disable pull\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    error_flag++;
                }

                if (config_data[4]>3) {
                    strcpy((char *)resonse.response_buf, "[error] inupt driving type is errror!\r\n 0:4ma\r\n 1:8ma\r\n 2:12ma\r\n 3:16ma\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                    error_flag++;
                }

                if (error_flag>0){break;}

                hal_pinmux_set_function(pin_index[0], config_data[0]);
                hal_gpio_set_direction(pin_index[0], (hal_gpio_direction_t)config_data[1]);
                hal_gpio_set_output(pin_index[0], (hal_gpio_data_t)config_data[2]);

                if (config_data[3] == 0) {
                    hal_gpio_pull_up(pin_index[0]);
                }
                else if (config_data[3] == 1) {
                    hal_gpio_pull_down(pin_index[0]);
                }
                else {
                    hal_gpio_disable_pull(pin_index[0]);
                }       
  
                hal_gpio_set_driving_current(pin_index[0], (hal_gpio_driving_current_t)config_data[4]);

                gpio_get_state(pin_index[0],&gpio_state);                           //get specify gpio current configuration

                temp = (uint8_t)gpio_state.current_type;                            //change current type value              
                strcpy((char *)temp1, (char*)current_state[temp]);                  //get current state string
                strcpy((char *)temp2, (char*)(pull_state[gpio_state.pull_type]));   //get pull state string
                    
                ret_len1 = snprintf((char *)(&temp_buf[0]),\
                                   ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                                   "set GPIO%-2d, md=%d, %s, di=%d, do=%d, ", \
                                   (int)pin_index[0], (int)gpio_state.mode, (char*)direct[gpio_state.dir], (int)gpio_state.din,(int)gpio_state.dout);
                
                ret_len2 = snprintf((char *)(&temp_buf[ret_len1]),\
                                   ATCI_UART_TX_FIFO_BUFFER_SIZE,\
                                   "%s, %s done!\r\n",(char*)temp1,(char*)temp2);

                strncpy((char *)(resonse.response_buf),(char*)(&temp_buf[0]),(ret_len1+ret_len2));
                resonse.response_len  = (uint16_t)(ret_len1+ret_len2);  
                resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);

                break;
            }

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "Wrong command ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&resonse);
            break;
    }

   snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,\
        "/*****************next command line*********************\r\n");
    resonse.response_len = strlen((char *)resonse.response_buf);
    resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    atci_send_response(&resonse);
    return ATCI_STATUS_OK;
}

#endif

