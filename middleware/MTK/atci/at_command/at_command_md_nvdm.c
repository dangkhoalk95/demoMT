
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

#ifdef MTK_NVDM_MODEM_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"
#include "at_command.h"
#include "nvdm_modem.h"
#include "nvdm_tool.h"
#include "nvdm_modem_minidump.h"

/********************************************************
 * Macro & Define
 *
 ********************************************************/

/********************************************************
 * Enum & Structures & Global variable
 *
 ********************************************************/

/********************************************************
 * Function declaration
 *
 ********************************************************/
static uint32_t hexstring_to_bytearray(char *s, uint8_t *bits)
{
    uint32_t i, n = 0;

    for (i = 0; s[i]; i += 2) {
        if (s[i] >= 'A' && s[i] <= 'F') {
            bits[n] = s[i] - 'A' + 10;
        } else if (s[i] >= 'a' && s[i] <= 'f') {
            bits[n] = s[i] - 'a' + 10;
        } else {
            bits[n] = s[i] - '0';
        }
        if (s[i + 1] >= 'A' && s[i + 1] <= 'F') {
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        } else if (s[i + 1] >= 'a' && s[i + 1] <= 'f') {
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'a' + 10);
        } else {
            bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
        }
        ++n;
    }

    return n;
}

static uint32_t hexstring_to_integer(char *s, uint32_t bits)
{
    uint32_t i, j, tmp, sum;

    sum = 0;
    for (i = 0; i < bits; i++) {
        if ((s[i] < '0') || (s[i] > '9')) {
            return 0;
        }
        tmp = s[i] - '0';
        for (j = 0; j < bits - i - 1; j++) {
            tmp *= 10;
        }
        sum += tmp;
    }

    return sum;
}

static void dump_data_item_info(nvdm_modem_data_item_info_t *info_list, uint32_t count)
{
    uint32_t i;


    for (i = 0; i < count; i++) {
        LOG_I(common, "Dump data item:%d(%d)", i, count);
        LOG_I(common, " [AREA]=0x%x", info_list[i].area);
        LOG_I(common, " [GROUP_NAME]=%s", info_list[i].group_name);
        LOG_I(common, " [DATAITEM_NAME]=%s", info_list[i].data_item_name);
    }
}

#ifdef MTK_MINI_DUMP_ENABLE
static int32_t mini_dump_data_verify(uint8_t *buf, uint16_t size)
{
    for (; size > 0; size--) {
        if (buf[0] != buf[size - 1]) {
            return -1;
        }
    }
    return 0;
}
#endif

/* AT command handler */
atci_status_t atci_cmd_hdlr_nvdm_modem(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *group_name = NULL;
    char *data_item_name = NULL;
    char *parameter;
    char *saveptr = NULL;
    uint32_t length;
    uint8_t buffer[256];
    nvdm_modem_status_t status = NVDM_MODEM_STATUS_ERROR;
    nvdm_modem_area_t area;
    nvdm_modem_attr_enum_t attr;
    char delim[] = {',', 0x0D, 0x0A};

    LOG_I(common, "atci_cmd_hdlr_nvdm_modem\n");

    resonse.response_flag = 0; // Command Execute Finish.
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    resonse.cmd_id = parse_cmd->cmd_id;
    #endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)(resonse.response_buf), "+ENVDMMD:(0~5),<area>,>[<group>,<data_item,<attribute>,<length>,<data>]\r\nOK\r\n");
            resonse.response_len = strlen((char *)(resonse.response_buf));
            resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+ENVDMMD=<op>  the handler need to parse the parameters
            /*===============================================================READ */
            if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=0") != NULL) {
                /* read data item from NVDM */
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);

                /* get area */
                parameter = strtok_r(NULL, delim, &saveptr);
                area = hexstring_to_integer(parameter, strlen(parameter));
                if (area <= NVDM_MODEM_AREA_NONE || area > NVDM_MODEM_AREA_END) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "AREA = %d", area);

                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);

                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);

                /* end */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }

                length = 1024;
                if (area == NVDM_MODEM_AREA_NORMAL)
                    status = nvdm_modem_read_normal_data_item(group_name,
                             data_item_name,
                             NULL,
                             resonse.response_buf,
                             &length);
                else if (area == NVDM_MODEM_AREA_PROTECTED)
                    status = nvdm_modem_read_protected_data_item(group_name,
                             data_item_name,
                             NULL,
                             resonse.response_buf,
                             &length);

                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_read_data_item status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "nvdm_read_data_item length = %d", length);
                {
                    uint32_t i;

                    LOG_I(common, "data = ");
                    for (i = 0; i < length; i++) {
                        LOG_I(common, "0x%x", resonse.response_buf[i]);
                    }
                }
                resonse.response_buf[length] = '\r';
                resonse.response_buf[length + 1] = '\n';
                resonse.response_len = length + 2;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=1") != NULL) {
                /*===============================================================WRITE */
                /* write data item into NVDM */
                LOG_I(common, "atci_cmd_hdlr_nvdm_modem: write\n");
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);

                /* get area */
                parameter = strtok_r(NULL, delim, &saveptr);
                area = hexstring_to_integer(parameter, strlen(parameter));
                if (area <= NVDM_MODEM_AREA_NONE || area > NVDM_MODEM_AREA_END) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "AREA = %d", area);

                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);

                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);

                /* get attribute */
                parameter = strtok_r(NULL, delim, &saveptr);
                attr = hexstring_to_integer(parameter, strlen(parameter));
                if (NVDM_MODEM_ATTR_AVERAGE != attr && NVDM_MODEM_ATTR_BACKUP != attr) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "attribute = %d", attr);

                /* get length of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = hexstring_to_integer(parameter, strlen(parameter));
                LOG_I(common, "length = %d", length);

                /* get content of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = hexstring_to_bytearray(parameter, buffer);
                LOG_I(common, "parse length = %d", resonse.response_len);
                if (length != resonse.response_len) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                {
                    uint32_t i;

                    LOG_I(common, "data = ");
                    for (i = 0; i < length; i++) {
                        LOG_I(common, "0x%x", buffer[i]);
                    }
                }
                if (area == NVDM_MODEM_AREA_NORMAL)
                    status = nvdm_modem_write_normal_data_item(group_name,
                             data_item_name,
                             NVDM_MODEM_DATA_ITEM_TYPE_STRING,
                             buffer,
                             resonse.response_len,
                             attr);
                else if (area == NVDM_MODEM_AREA_PROTECTED)
                    status = nvdm_modem_write_protected_data_item(group_name,
                             data_item_name,
                             NVDM_MODEM_DATA_ITEM_TYPE_STRING,
                             buffer,
                             resonse.response_len,
                             attr);

                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_write_data_item status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=2") != NULL) {
                /*===============================================================INVALIDATE */
                /* invalidate data item into NVDM */
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                /* get area */
                parameter = strtok_r(NULL, delim, &saveptr);
                area = hexstring_to_integer(parameter, strlen(parameter));
                if (area <= NVDM_MODEM_AREA_NONE || area > NVDM_MODEM_AREA_END) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "AREA = %d", area);
                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);
                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);

                if (area == NVDM_MODEM_AREA_NORMAL) {
                    status = nvdm_modem_invalidate_normal_data_item(group_name, data_item_name);
                } else if (area == NVDM_MODEM_AREA_PROTECTED) {
                    status = nvdm_modem_invalidate_protected_data_item(group_name, data_item_name);
                }

                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_delete_data_item status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=3") != NULL) {
                /*===============================================================RESET */
                /* invalidate data item into NVDM */
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                /* get area */
                parameter = strtok_r(NULL, delim, &saveptr);
                area = hexstring_to_integer(parameter, strlen(parameter));
                if (area <= NVDM_MODEM_AREA_NONE || area > NVDM_MODEM_AREA_END) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "AREA = %d", area);
                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);
                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);

                if (area == NVDM_MODEM_AREA_NORMAL) {
                    status = nvdm_modem_reset_normal_data_item(group_name, data_item_name);
                } else if (area == NVDM_MODEM_AREA_PROTECTED) {
                    status = nvdm_modem_reset_protected_data_item(group_name, data_item_name);
                }

                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_reset_data_item status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=4") != NULL) {
                /*===============================================================QUERY */
                /* query all data item information */
                uint32_t data_number, i, length = 0;
                nvdm_modem_data_item_info_t *p_info = NULL;

                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);

                /* get area */
                parameter = strtok_r(NULL, delim, &saveptr);

                area = hexstring_to_integer(parameter, strlen(parameter));
                if (area <= NVDM_MODEM_AREA_NONE || area > NVDM_MODEM_AREA_END) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "AREA = %d", area);

                nvdm_modem_query_data_item_number((nvdm_modem_area_t)area, &data_number);
                LOG_I(common, "[QUERY][AREA:%d] data_number = %d", area, data_number);
                p_info = pvPortMalloc(data_number * sizeof(nvdm_modem_data_item_info_t));
                status = nvdm_modem_query_all_data_item_info((nvdm_modem_area_t)area, p_info, data_number);
                if (status == NVDM_MODEM_STATUS_OK) {
                    dump_data_item_info(p_info, data_number);

                    for (i=0; i<data_number; i++) {
                        length += sprintf((char *)resonse.response_buf + length, "[%d]0x%x,%s,%s\r\n", (int)i, p_info[i].area, p_info[i].group_name, p_info[i].data_item_name);
                        if (length > 512) break;
                    }
                    resonse.response_buf[length] = '\r';
                    resonse.response_buf[length + 1] = '\n';
                    resonse.response_len = length + 2;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    LOG_I(common, "[ERROR][QUERY][AREA=%d]query data item error:%d", area, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                vPortFree(p_info);

            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=5") != NULL) {
                /*===============================================================QUERY */
                /* query data item area */
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);

                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);

                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);

                status = nvdm_modem_query_data_item_area(group_name, data_item_name, &area);
                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_query_data_item_area status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "(%s,%s)AREA=0x%x", group_name, data_item_name, area);
                resonse.response_len = sprintf((char *)resonse.response_buf, "(%s,%s)AREA=0x%x\r\n", group_name, data_item_name, area);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=6") != NULL) {
                #ifdef MTK_MINI_DUMP_ENABLE
                /*===============================================================mini-dump Write*/
                /* mini-dump WR */
                int len, dump_num = 0;
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                uint8_t *minidump_buf;

                /* get length of mini-dump content */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = hexstring_to_integer(parameter, strlen(parameter));
                LOG_I(common, "length = %d", length);

                /* get content of mini-dump data */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }

                hexstring_to_bytearray(parameter, buffer);
                minidump_buf = pvPortMalloc(length);
                if (!minidump_buf) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                memset(minidump_buf, buffer[0], length);
                LOG_I(common, "dump_data_cont=0x%x, ...", buffer[0]);

                status = nvdm_modem_write_mini_dump_data(minidump_buf, length);
                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_write_mini_dump_data status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    vPortFree(minidump_buf);
                    break;
                } else {
                    nvdm_modem_query_mini_dump_number((uint8_t *)&dump_num);
                    LOG_I(common, "write success:len=%d,dump_num=%d", length, dump_num);
                    resonse.response_len = sprintf((char *)resonse.response_buf, "\r\nwrite success:len=%d,dump_num=%d\r\n", length, dump_num);
                }
                vPortFree(minidump_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                #else
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                #endif
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=7") != NULL) {
                #ifdef MTK_MINI_DUMP_ENABLE
                /*===============================================================read mini-dump */
                /* mini-dump RD */
                uint8_t dump_idx, dump_num;
                uint16_t offset, buf_size;
                uint32_t length = 0;

                nvdm_modem_query_mini_dump_number(&dump_num);
                LOG_I(common, "Total dump_num=%d", dump_num);
                if (dump_num == 0) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    break;
                }

                for (dump_idx = 0; dump_idx < dump_num; dump_idx++) {
                    offset = 0;
                    buf_size = sizeof(buffer);
                    length += sprintf((char *)resonse.response_buf + length, "\r\n");
                    do {
                        status = nvdm_modem_read_mini_dump_data(dump_idx, offset, buffer, &buf_size);
                        if (status != NVDM_MODEM_STATUS_OK || mini_dump_data_verify(buffer, buf_size) < 0) {
                            LOG_I(common, "[ERROR] read/verify mini-dump error idx=%d", dump_idx);
                            length += sprintf((char *)resonse.response_buf + length, "[ERROR] read/verify mini-dump error idx=%d\r\n", dump_idx);
                            break;
                        }
                        offset += buf_size;
                    } while (buf_size != 0 && status == NVDM_MODEM_STATUS_OK);
                    LOG_I(common, "[DUMP_%d] data={0x%x,0x%x,...0x%x}, len=%d", dump_idx, buffer[0], buffer[1], buffer[(offset - 1) % sizeof(buffer)], offset);
                    length += sprintf((char *)resonse.response_buf + length, "[DUMP_%d] data={0x%x,0x%x,...0x%x}, len=%d\r\n", dump_idx, buffer[0], buffer[1], buffer[(offset - 1) % sizeof(buffer)], offset);
                }

                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_read_mini_dump_data status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = length;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                #else
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                #endif
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDMMD=8") != NULL) {
                #ifdef MTK_MINI_DUMP_ENABLE
                /*===============================================================delete mini-dump item*/
                /* mini-dump delete */
                int len;
                uint8_t dump_idx, dump_num;
                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);

                /* get mini-dump idx */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }

                dump_idx = hexstring_to_integer(parameter, strlen(parameter));
                LOG_I(common, "dump_idx = %d", dump_idx);

                status = nvdm_modem_clean_mini_dump_data(dump_idx);
                if (status != NVDM_MODEM_STATUS_OK) {
                    LOG_I(common, "[ERROR]nvdm_modem_clean_mini_dump status=%d, idx=%d", status, dump_idx);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                } else {
                    nvdm_modem_query_mini_dump_number(&dump_num);
                    LOG_I(common, "clean success:idx=%d,dump_num=%d", dump_idx, dump_num);
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                #else
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                #endif
            } else {
                /*=============================================================== Others */
                /* invalid AT command */
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            break;
        default :
            // others are invalid command format
            strcpy((char *)(resonse.response_buf), "ERROR\r\n");
            resonse.response_len = strlen((char *)(resonse.response_buf));
            break;
    }

    atci_send_response(&resonse);

    return ATCI_STATUS_OK;
}




#endif

