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
#include <stdlib.h>
#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
/*
 * sample code
*/


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_wdt(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EWDT=<op>                |   "OK"
AT+EWDT=?                   |   "+EWDT=(1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_hdlr_wdt(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    char *param = NULL;
    char param_val;

    printf("atci_cmd_hdlr_wdt \r\n");

    response.response_flag = 0; /*    Command Execute Finish.  */
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+EWDT=?   */
            strcpy((char *)response.response_buf, "+EWDT=(\"1: trigger wdt reset\")\r\nOK\r\n");
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+EWDT=<op>  the handler need to parse the parameters  */
            param = parse_cmd->string_ptr + parse_cmd->parse_pos;
            param_val = atoi(param);

            if (1 != param_val) {
                strcpy((char *)response.response_buf, "ERROR\r\n");
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                response.response_len = strlen((char *)response.response_buf);
                atci_send_response(&response);
                break;
            }

            if (1 == param_val) {
                hal_wdt_config_t wdt_config;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                wdt_config.mode = HAL_WDT_MODE_RESET;
                wdt_config.seconds = 1;
                hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
                hal_wdt_init(&wdt_config);
                hal_wdt_software_reset();
                while(1); /* waiting the system reboot */
            }

            //strcpy((char *)response.response_buf, "OK\r\n");
            //response.response_len = strlen((char *)response.response_buf);
            //atci_send_response(&response);

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

