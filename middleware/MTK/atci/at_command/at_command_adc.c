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
 
// For Register AT command handler
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include <stdlib.h>

#if (PRODUCT_VERSION == 2625)
#ifdef MTK_BUILD_SMT_LOAD
#ifdef HAL_ADC_MODULE_ENABLED
#include "hal_adc.h"
/*
 * sample code
*/


static uint16_t adc_sample_to_voltage(uint32_t source_code)
{
    uint16_t voltage = (source_code * 1400) / 4095;
    return voltage;
}

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_auxadc(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EWDT=<op>                |   "OK"
AT+EWDT=?                   |   "+EAUXADC=(1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_hdlr_auxadc(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    char *param = NULL;
    char param_val;

    uint32_t adc_data;
    uint32_t adc_voltage;

    printf("atci_cmd_hdlr_auxadc \r\n");

    response.response_flag = 0; /*    Command Execute Finish.  */
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
#endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+EAUXADC=?   */
            strcpy((char *)response.response_buf, "+EAUXADC:1, measure voltage of CH2.");
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+EAUXADC=<op>  the handler need to parse the parameters  */
            if (parse_cmd->parse_pos < parse_cmd->string_len) {
                if ((*(parse_cmd->string_ptr + parse_cmd->parse_pos)) == '1') {
                    //hal_pinmux_set_function(HAL_GPIO_32, 5);
                    hal_adc_init();
                    hal_adc_get_data_polling(HAL_ADC_CHANNEL_2, &adc_data);
                    hal_adc_deinit();

                    adc_voltage = adc_sample_to_voltage(adc_data);
                    if ((adc_voltage > 800) && (adc_voltage < 1000)) {
                        sprintf((char *)response.response_buf, "+EAUXADC:adc_data = 0x%04x, adc_voltage = %dmV", adc_data, adc_voltage);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        sprintf((char *)response.response_buf, "+EAUXADC:adc_data = 0x%04x, adc_voltage = %dmV", adc_data, adc_voltage);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                } else {
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    response.response_len = 0;
                    atci_send_response(&response);
                }

            } else {
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                response.response_len = 0;
                atci_send_response(&response);
            }
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)response.response_buf, "ERROR Command.\r\n");
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
#endif
#else
atci_status_t atci_cmd_hdlr_auxadc(atci_parse_cmd_param_t *parse_cmd)
{
		parse_cmd = parse_cmd;
		return ATCI_STATUS_OK;
}
#endif

