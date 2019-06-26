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
//#include "os.h"
#include "semphr.h"
//#include "mtk_wifi_adapter.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "mt5932_netif.h"
#include "portmacro.h"
#ifdef __HOTSPOT_SUPPORT__
#include "dhcpd.h"
#endif

#ifdef MTK_WIFI_STUB_CONF_ENABLE
#include "wfcm_stub.h"
#endif

/**
* @brief Station operation mode. In this mode the device works as a Wi-Fi client.
*/
#define WIFI_MODE_STA_ONLY      (1)

/**
* @brief SoftAP operation mode. In Access Point (AP) mode, other client devices can connect to the Wi-Fi AP.
*/
#define WIFI_MODE_AP_ONLY       (2)


/**
* @brief Specifies the AP operation.
*/
#define WIFI_PORT_AP            (1)

/**
* @brief Repeater mode. This mode isn't supported yet.
*/
#define WIFI_MODE_REPEATER          (3)


#define wifi_event_t uint32_t

/*
    Description: use DHCP client in Wi-Fi STA mode to get IP address from server.
    Option:
        0: disable DHCP client procedure, set static IP address instead
        1: enable DHCP client to get IP address
    Dependency: No.
*/
#define USE_DHCP                 1

#define STA_IPADDR    ("192.168.1.101")
#define STA_NETMASK   ("255.255.255.0")
#define STA_GATEWAY   ("192.168.1.1")

#define AP_IPADDR     ("10.10.10.1")
#define AP_NETMASK    ("255.255.255.0")
#define AP_GATEWAY    ("10.10.10.1")

#define PRIMARY_DNS   ("8.8.8.8")
#define SECONDARY_DNS ("8.8.4.4")
#define IP_POOL_START ("10.10.10.2")
#define IP_POOL_END   ("10.10.10.10")

SemaphoreHandle_t wifi_connected;
#if USE_DHCP
SemaphoreHandle_t ip_ready;
#endif

#if USE_DHCP
static void ip_ready_callback(struct netif *netif);
#endif
int32_t wifi_station_port_secure_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
//static int32_t wifi_station_disconnected_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
void dhcpd_stop(void);

int g_ip_ready = 0;

/**
  * @brief  dhcp got ip will callback this function.
  * @param[in] struct netif *netif: which network interface got ip.
  * @retval None
  */
#if USE_DHCP
static void ip_ready_callback(struct netif *netif)
{
    if (!ip4_addr_isany_val(netif->ip_addr)) {
        char ip_addr[17] = {0};
        if (NULL != inet_ntoa(netif->ip_addr)) {
            strcpy(ip_addr, inet_ntoa(netif->ip_addr));
            printf("\r\n************************\r\n");
            printf("DHCP got IP:%s\r\n", ip_addr);
            printf("************************\r\n");
            //wifi_config_set_sta_power_saving_mode(Param_PowerModeFast_PSP); //Castro+

            //g_ip_ready = 1;
            wfcm_to_slave(WFC_CMD_OS_INFO, NULL, 0, NULL, NULL, 0);  //Castro+
        } else {
            //LOG_E(common, "DHCP got Failed");
        }
    }
    xSemaphoreGive(ip_ready);
    //LOG_I(common, "ip ready");
}
#endif

/**
  * @brief  wifi connected will call this callback function. set lwip status in this function
  * @param[in] wifi_event_t event: not used.
  * @param[in] uint8_t *payload: not used.
  * @param[in] uint32_t length: not used.
  * @retval None
  */
int32_t wifi_station_port_secure_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_link_up(sta_if);
    g_ip_ready = 1; //Castro+             //WiFi Connected

    xSemaphoreGive(wifi_connected);
    //LOG_I(common, "wifi connected");
    return 0;
}

#if 1
/**
  * @brief  wifi disconnected will call this callback function. set lwip status in this function
  * @param[in] wifi_event_t event: not used.
  * @param[in] uint8_t *payload: not used.
  * @param[in] uint32_t length: not used.
  * @retval None
  */
int32_t wifi_station_disconnected_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_link_down(sta_if);
    
    g_ip_ready = 0;
#if USE_DHCP
    netif_set_addr(sta_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY);
#endif
    //LOG_I(common, "wifi disconnected");
    return 1;
}
#endif

/**
  * @brief  network init function. initial wifi and lwip config
  * @param None
  * @retval None
  */

void lwip_network_init(uint8_t opmode)
{
    lwip_tcpip_config_t tcpip_config = {0, {0}, {0}, {0}, {0}, {0}};

    //ip4addr_aton(STA_IPADDR, &(tcpip_config.sta_addr));
    ip4addr_aton("192.168.1.100", &(tcpip_config.sta_addr));
    ip4addr_aton(STA_NETMASK, &tcpip_config.sta_mask);
    ip4addr_aton(STA_GATEWAY, &tcpip_config.sta_gateway);
    ip4addr_aton(AP_IPADDR, &(tcpip_config.ap_addr));
    ip4addr_aton(AP_NETMASK, &tcpip_config.ap_mask);
    ip4addr_aton(AP_GATEWAY, &tcpip_config.ap_gateway);
    wifi_connected = xSemaphoreCreateBinary();
#if USE_DHCP
    ip_ready = xSemaphoreCreateBinary();
#endif
    lwip_tcpip_init(&tcpip_config, opmode);
}


void lwip_net_start(uint8_t opmode)
{
    struct netif *sta_if;
#ifdef __HOTSPOT_SUPPORT__
    struct netif *ap_if;
#endif
    switch(opmode) {
        case WIFI_MODE_STA_ONLY:
        case WIFI_MODE_REPEATER:
            //wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, (wifi_event_handler_t)wifi_station_port_secure_event_handler));
            //wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, (wifi_event_handler_t)wifi_station_disconnected_event_handler);
        #if USE_DHCP
            sta_if = netif_find_by_type(NETIF_TYPE_STA);
            netif_set_default(sta_if);
            netif_set_status_callback(sta_if, ip_ready_callback);
            dhcp_start(sta_if);
        #else
            sta_if = netif_find_by_type(NETIF_TYPE_STA);
            netif_set_default(sta_if);
            netif_set_link_up(sta_if);
            xSemaphoreGive(wifi_connected);            	
        #endif
            break;
    #ifdef __HOTSPOT_SUPPORT__
        case WIFI_MODE_AP_ONLY: {
            dhcpd_settings_t dhcpd_settings = {{0},{0},{0},{0},{0},{0},{0}};
            strcpy((char *)dhcpd_settings.dhcpd_server_address, AP_IPADDR);
            strcpy((char *)dhcpd_settings.dhcpd_netmask, AP_NETMASK);
            strcpy((char *)dhcpd_settings.dhcpd_gateway, AP_GATEWAY);
            strcpy((char *)dhcpd_settings.dhcpd_primary_dns, PRIMARY_DNS);
            strcpy((char *)dhcpd_settings.dhcpd_secondary_dns, SECONDARY_DNS);
            strcpy((char *)dhcpd_settings.dhcpd_ip_pool_start, IP_POOL_START);
            strcpy((char *)dhcpd_settings.dhcpd_ip_pool_end, IP_POOL_END);
            ap_if = netif_find_by_type(NETIF_TYPE_AP);
            netif_set_default(ap_if);
            netif_set_link_up(ap_if);
            dhcpd_start(&dhcpd_settings);
            break;
        }
    #endif
    }
}

void lwip_net_stop(uint8_t opmode)
{
    struct netif *sta_if;
    struct netif *ap_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    ap_if = netif_find_by_type(NETIF_TYPE_AP);
    switch (opmode) {
        case WIFI_MODE_AP_ONLY:
            dhcpd_stop();
            netif_set_link_down(ap_if);
            break;
        case WIFI_MODE_STA_ONLY:
        case WIFI_MODE_REPEATER:
            netif_set_status_callback(sta_if, NULL);
            dhcp_release(sta_if);
            dhcp_stop(sta_if);
            netif_set_link_down(sta_if);
            break;
    }
}

/**
  * @brief  when wifi and ip ready will return, only used in station and repeater mode.
  * @param None
  * @retval None
  */
void lwip_net_ready()
{
    xSemaphoreTake(wifi_connected, portMAX_DELAY);
#if USE_DHCP
    xSemaphoreTake(ip_ready, portMAX_DELAY);
#endif
}

