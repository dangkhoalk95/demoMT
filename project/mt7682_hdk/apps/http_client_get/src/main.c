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
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "syslog.h"

#include "sys_init.h"
#include "wifi_api.h"
#include "wifi_lwip_helper.h"
#include "httpclient.h"

#define WIFI_SSID                ("SQA_TEST_AP")
#define WIFI_PASSWORD            ("77777777")

#define BUF_SIZE        (1024 * 1)
#define HTTP_GET_URL    "http://www.aliyun.com/"
#define HTTPS_GET_URL   "https://www.baidu.com/"
#define BAIDUIE_CER														\
"-----BEGIN CERTIFICATE-----\r\n"                                       \
"MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n"  \
"A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"  \
"b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n"  \
"MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"  \
"YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n"  \
"aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n"  \
"jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n"  \
"xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n"  \
"1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n"  \
"snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n"  \
"U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n"  \
"9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n"  \
"BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n"  \
"AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n"  \
"yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n"  \
"38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n"  \
"AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n"  \
"DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n"  \
"HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n"                              \
"-----END CERTIFICATE-----\r\n"

static const char baiduie_cer[] = BAIDUIE_CER;

log_create_module(http_client_get_example, PRINT_LEVEL_INFO);

/**
  * @brief      Http client "get" method working flow.
  * @param      None
  * @return     None
  */
static void httpclient_test_get(void)
{
    char *get_url = HTTP_GET_URL;    
    char *post_url = HTTPS_GET_URL;
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf, *header;
    HTTPCLIENT_RESULT ret;
    int val_pos, val_len;
    
    LOG_I(http_client_get_example, "httpclient_test_get()");

    buf = pvPortMalloc(BUF_SIZE);    
    header = pvPortMalloc(BUF_SIZE);
    if (buf == NULL || header == NULL) {        
        LOG_I(http_client_get_example, "memory malloc failed.");
        return;
    }
    
    // Http "get"
    client_data.header_buf = header;    
    client_data.header_buf_len = BUF_SIZE;
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;    
    client_data.response_buf[0] = '\0';
    ret = httpclient_get(&client, get_url, &client_data);
    if (ret < 0)
        goto fail;   
    LOG_I(http_client_get_example, "data received: %s", client_data.response_buf);

    // get response header
    if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-length", &val_pos, &val_len))
        LOG_I(http_client_get_example, "Content-length: %.*s", val_len, client_data.header_buf + val_pos);
    
    // Https "get"
    client_data.header_buf = header;    
    client_data.header_buf_len = BUF_SIZE;
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;    
    client_data.response_buf[0] = '\0';    

#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    client.server_cert = baiduie_cer;
    client.server_cert_len = sizeof(baiduie_cer);
#endif

    ret = httpclient_get(&client, post_url, &client_data);  
    if (ret < 0)
        goto fail;        
    LOG_I(http_client_get_example, "data received: %s", client_data.response_buf);

    // get response header
    if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-length", &val_pos, &val_len))
        LOG_I(http_client_get_example, "Content-length: %.*s", val_len, client_data.header_buf + val_pos);

fail:
    vPortFree(buf);
    vPortFree(header);

    // Print final log
    if (ret >= 0)    
        LOG_I(http_client_get_example, "example project test success.");
    else        
        LOG_I(http_client_get_example, "httpclient_get fail, reason:%d.", ret);
}

/**
  * @brief      Create a task for http client get example
  * @param[in]  void *args: Not used
  * @return     None
  */
static void app_entry(void *args)
{
    lwip_net_ready();

    // Httpclient "get" feature test
    httpclient_test_get();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

/**
  * @brief      Main program
  * @param      None
  * @return     0
  */
int main(void)
{
    /* Do system initialization, eg: hardware, nvdm, logging and random seed. */
    system_init();

    /* system log initialization.
        * This is the simplest way to initialize system log, that just inputs three NULLs
        * as input arguments. User can use advanved feature of system log along with NVDM.
        * For more details, please refer to the log dev guide under /doc folder or projects
        * under project/mtxxxx_hdk/apps/.
        */
    log_init(NULL, NULL, NULL);

    LOG_I(http_client_get_example, "FreeRTOS Running");

    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
        * will be started , and adopt which settings for the specific mode while wifi initial process is running
        */
    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)config.sta_config.ssid, WIFI_SSID);
    strcpy((char *)config.sta_config.password, WIFI_PASSWORD);
    config.sta_config.ssid_length = strlen(WIFI_SSID);
    config.sta_config.password_length = strlen(WIFI_PASSWORD);

    /* Initialize wifi stack and register wifi init complete event handler,
        * notes:  the wifi initial process will be implemented and finished while system task scheduler is running.
        */
    wifi_init(&config, NULL);

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization. */
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);

    /* Create a user task for demo when and how to use wifi config API  to change WiFI settings,
        * Most WiFi APIs must be called in task scheduler, the system will work wrong if called in main(),
        * For which API must be called in task, please refer to wifi_api.h or WiFi API reference.
        * xTaskCreate(user_wifi_app_entry,
        *       UNIFY_USR_DEMO_TASK_NAME,
        *       UNIFY_USR_DEMO_TASK_STACKSIZE / 4,
        *       NULL, UNIFY_USR_DEMO_TASK_PRIO, NULL);
        */
    xTaskCreate(app_entry, APP_TASK_NAME, APP_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, APP_TASK_PRIO, NULL);


    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
        * will never be reached.  If the following line does execute, then there was
        * insufficient FreeRTOS heap memory available for the idle and/or timer tasks
        * to be created.  See the memory management section on the FreeRTOS web site
        * for more details.
        */
    for ( ;; );
}


