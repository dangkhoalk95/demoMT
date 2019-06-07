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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include <lwip/tcpip.h>
#include <ethernetif.h>
#include <lwip/sockets.h>

#include "os.h"
#include "task_def.h"
#include "bt_type.h"
//#define MESH_GATEWAY_DEMO_ENABLE

#define IS_WS(c)        (c == ' ')
/*valid command charc*/
#define IS_VM(c)        ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '_'))

#define SOCK_TCP_SRV_PORT        23
static int wifi_console = 0;
static uint8_t data_start = 0;
static uint8_t len = 0;
static char *param[10];

extern bt_status_t mesh_app_io_callback(uint8_t len, char *param[]);

static void mesh_gateway_tcp_server_console(void)
{
    int s;

    int ret;
    int rlen;
    int i;
    int index = 0;
    int rev_cmd = 0;
    struct sockaddr_in addr;
    char srv_buf[32] = {0};
    char cmd_buf[100 + 1] = {0};
    uint16_t offset;
    LOG_I(common, "mesh gateway tcp_server_test start\n");

    os_memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_TCP_SRV_PORT);
    addr.sin_addr.s_addr = lwip_htonl(IPADDR_ANY);

    /* create the socket */
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        LOG_I(common, "mesh gateway tcp server create fail\n");
        goto done;
    }

    ret = lwip_bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_I(common, "mesh gateway tcp server bind fail\n");
        goto clean;
    }

    ret = lwip_listen(s, 0);
    if (ret < 0) {
        LOG_I(common, "mesh gateway tcp server listen fail\n");
        goto clean;
    }

    while ( 1 ) {
        socklen_t sockaddr_len = sizeof(addr);
        wifi_console = 0;
        LOG_I(common, "mesh gateway tcp server waiting for accept connection...\n");
        wifi_console = lwip_accept(s, (struct sockaddr *)&addr, &sockaddr_len);
        if (wifi_console < 0) {
            LOG_I(common, "mesh gateway tcp server accept error\n");
            break;   // or go on waiting for connect requestion?
        }

        LOG_I(common, "mesh gateway tcp server waiting for data...\n");
        while ((rlen = lwip_read(wifi_console, srv_buf, sizeof(srv_buf) - 1)) != 0) {
            if (rlen < 0) {
                LOG_I(common, "mesh gateway read error.\n");
                break;
            }
            srv_buf[rlen] = 0; //for the next statement - printf string.
            //LOG_I(mesh_app, "tcp server received data:%s\n", srv_buf);
            for (i = 0; i < rlen; i++) {
                if (srv_buf[i] == '\n' || srv_buf[i] == 13) {
                    rev_cmd = 1;
                    break;
                }

                if ((srv_buf[i] & 0xf0) == 0xf0) {
                    continue;
                }
                if (i < 100) {
                    cmd_buf[index++] = srv_buf[i];
                }
            }

            if (rev_cmd) {
                LOG_I(common, "mesh gateway tcp server received cmd_buf :%s\n", cmd_buf);
                LOG_I(common, "mesh gateway tcp server received transfer data:%s\n", srv_buf);
                //LOG_I(mesh_app, ':%d:\n', '\0');
                while (offset < index) {
                    //LOG_I(mesh_app, ",%d,", cmd_buf[offset]);
                    if (!data_start) {
#if 0
                        if (IS_WS(cmd_buf[offset])) {
                            //cmd_buf[offset] = '\0';
                            offset++;
                            continue;
                        }
#endif
                        if (!IS_VM(cmd_buf[offset])) {
                            offset++;
                            continue;
                        }

                        data_start = 1;
                        param[len] = &cmd_buf[offset];
                        len += 1;
                    } else {
                        if (IS_WS(cmd_buf[offset])) {
                            data_start = 0;
                            cmd_buf[offset] = '\0';
                        }
                    }
                    offset++;
                }
                cmd_buf[offset] = '\0';

                LOG_I(common, "mesh gateway parsed cmd :%d\n", len);
                if (len != 0) {
                    mesh_app_io_callback(len - 1, &(param[1]));
                }

                index = 0 ;
                rev_cmd = 0;
                offset = 0;
                data_start = 0;
                len = 0;
                os_memset(cmd_buf, 0, 100);
                os_memset(param, 0, 10);
                //bt_app_io_callback(cmd_buf, NULL);
            }
            //lwip_write(wifi_console, srv_buf, rlen);      // sonar server
        }

        lwip_close(wifi_console);
    }

clean:
    lwip_close(s);

done:
    LOG_I(common, "mesh gateway tcp server test done\n");

}

void mesh_gateway_tcp_server_thread(void *not_used)
{

    mesh_gateway_tcp_server_console();

    //keep the task alive
    for (;;) {
    }
}

void mesh_gateway_lwip_send(const void *data, size_t size)
{
    lwip_send(wifi_console, data, size, MSG_DONTWAIT);
}

void mesh_gateway_init(void)
{
    LOG_I(common, "mesh gateway begin to create tcp server task\n");
    if (pdPASS != xTaskCreate(mesh_gateway_tcp_server_thread, MESH_GATEWAY_TCP_TASK_NAME, MESH_GATEWAY_TCP_TASK_STACKSIZE / sizeof(portSTACK_TYPE), NULL, MESH_GATEWAY_TCP_TASK_PRIO, NULL)) {
        LOG_E(common, "mesh gateway  cannot create tcp_server_thread.");
    }

}

