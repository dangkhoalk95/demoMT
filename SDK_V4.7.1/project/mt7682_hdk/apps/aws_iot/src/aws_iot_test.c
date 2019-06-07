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

#include "sample_common_header.h"
#include "aws_iot_test_integration_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"
#include "wifi_lwip_helper.h"
#include "task_def.h"

//#define TEST

int already_started = 0;
const char *aws_iot_task_name = "AWS_IOT";
static const int AWS_IOT_TASK_STACK_SIZE = 1024 * 10;

void aws_iot_testing_task(void *param)
{
#ifdef TEST
    int basic_conn_ret = 0;
    int auto_reconnect_ret = 0;
    int multiple_clients_ret = 0;
    int multi_thread_validation_ret = 0;
    int shadow_sample_console_ret = 0;
    int sub_pub_lib_sample_ret = 0;
    int sub_pub_sample = 0;
#endif
    int shadow_sample_ret = 0;

    printf(".....AWS IoT Test Task Running.....\r\n");

#ifdef TEST
    printf("\n=========================================================\r\n");
    printf( "= Start To Run Basic Connectivity\r\n");
    printf( "=========================================================\r\n");
    basic_conn_ret = aws_iot_mqtt_tests_basic_connectivity();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Basic Connectivity Result = %d\r\n", basic_conn_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);

    printf("\n=========================================================\r\n");
    printf( "= Start To Run Auto Reconnect\r\n");
    printf( "=========================================================\r\n");
    auto_reconnect_ret = aws_iot_mqtt_tests_auto_reconnect();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Auto Reconnect Result = %d\r\n", auto_reconnect_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);

    printf("\n=========================================================\r\n");
    printf( "= Start To Run Multi Clients\r\n");
    printf( "=========================================================\r\n");
    multiple_clients_ret = aws_iot_mqtt_tests_multiple_clients();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Multi Clients Result = %d\r\n", multiple_clients_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);

    printf("\n=========================================================\r\n");
    printf( "= Start To Run Multi Threading Validation\r\n");
    printf( "=========================================================\r\n");
    multi_thread_validation_ret = aws_iot_mqtt_tests_multi_threading_validation();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Multi Threading Validation Result = %d\r\n", multi_thread_validation_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);
#endif

    printf("\n=========================================================\r\n");
    printf( "= Start To Run Shadow Sample\r\n");
    printf( "=========================================================\r\n");
    shadow_sample_ret = shadow_sample_main();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Shadow Sample Result = %d\r\n", shadow_sample_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);

#ifdef TEST
    printf( "\n=========================================================\r\n");
    printf( "= Start To Run Shadow Sample Console Echo\r\n");
    printf( "=========================================================\r\n");
    shadow_sample_console_ret = shadow_sample_console_echo_main();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Shadow Sample Console Echo Result = %d\r\n", shadow_sample_console_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);

    printf( "\n=========================================================\r\n");
    printf( "- Start To Run Subscribe Publish Library Sample\r\n");
    printf( "=========================================================\r\n");
    sub_pub_lib_sample_ret = subscribe_publish_library_sample_main();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Subscribe Publish Library Sample Result = %d\r\n", sub_pub_lib_sample_ret);
    printf( "---------------------------------------------------------\r\n");

    vTaskDelay(1000);

    printf( "\n=========================================================\r\n");
    printf( "= Start To Run Subscribe Publish Sample\r\n");
    printf( "=========================================================\r\n");
    sub_pub_sample = subscribe_publish_sample_main();
    printf( "\n---------------------------------------------------------\r\n");
    printf( "- Subscribe Publish Sample Result = %d\r\n", sub_pub_sample);
    printf( "---------------------------------------------------------\r\n");


    printf("*********************************************************************\r\n");
    printf("* Sample Result:\r\n");
    printf("* Basic Connectivity Result             : %s\r\n", (basic_conn_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Auto Reconnect Result                 : %s\r\n", (auto_reconnect_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Multi Clients Result                  : %s\r\n", (multiple_clients_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Multi Thread Validation Result        : %s\r\n", (multi_thread_validation_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Shadow Sample Result                  : %s\r\n", (shadow_sample_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Shadow Console Echo Result            : %s\r\n", (shadow_sample_console_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Subscribe Publish Library Result      : %s\r\n", (sub_pub_lib_sample_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Subscribe Publish Result              : %s\r\n", (sub_pub_sample == 0 ? "SUCCESS" : "FAILURE"));
    printf("*********************************************************************\r\n");
#else
    printf("*********************************************************************\r\n");
    printf("* Sample Result:\r\n");
    printf("* Shadow Sample Result                  : %s\r\n", (shadow_sample_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("*********************************************************************\n");
    if (shadow_sample_ret == 0) {
        printf("example project test success.\r\n");
    }
#endif
    vTaskDelay(1000);

    vTaskDelete(NULL);
}

void ip_ready_task(void *param)
{
    lwip_net_ready();

    vTaskDelay(1000);

    if (already_started == 0) {
        printf(".....Start AWS IoT Test Task.....\r\n");
        already_started = 1;
        xTaskCreate(aws_iot_testing_task, aws_iot_task_name,
                        AWS_IOT_TASK_STACK_SIZE / ((uint32_t)sizeof(StackType_t)), NULL, TASK_PRIORITY_NORMAL, NULL);
    } else {
        printf(".....AWS IoT Testing Task Already Started.....\r\n");
    }

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

void start_aws_iot_tests(void)
{
    xTaskCreate(ip_ready_task, "ip_ready_task",
                        1024 / ((uint32_t)sizeof(StackType_t)), NULL, TASK_PRIORITY_NORMAL, NULL);
}

