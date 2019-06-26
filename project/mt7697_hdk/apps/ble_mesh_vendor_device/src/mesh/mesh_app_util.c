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

#include "bt_type.h"
#include "mesh_app_util.h"

#include "bt_gap_le.h"
#include "bt_gattc.h"
#include "bt_type.h"
#include "bt_mesh.h"

#include "nvdm.h"
#include "hal_trng.h"

/* hal header*/
#include "hal_flash.h"
#include "hal_sys.h"
#include "hal_cache.h"

#define VENDOR_COMPANY_ID 0x000a
#define VENDOR_MODEL_ID1 0x002A
#define VENDOR_MODEL_ID2 0x002B
#define VENDOR_OPCODE_1 0xC1
#define VENDOR_OPCODE_2 0xC2

extern uint16_t g_appkey_index;

log_create_module(mesh_app, PRINT_LEVEL_INFO);
log_create_module(mesh_app_must, PRINT_LEVEL_INFO);

void mesh_app_util_log_off()
{
    log_config_print_switch(mesh_network, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_transport, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_access, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_model, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_config, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_bearer, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_provision, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_beacon, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_proxy, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_utils, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_bearer_gatt, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_middleware, DEBUG_LOG_OFF);
    log_config_print_switch(mesh_friend, DEBUG_LOG_OFF);
}

void mesh_app_util_copy_str_to_addr(uint8_t *addr, const char *str)
{
    unsigned int i, value;
    int using_long_format = 0;
    int using_hex_sign = 0;

    if (str[2] == ':' || str[2] == '-') {
        using_long_format = 1;
    }

    if (str[1] == 'x') {
        using_hex_sign = 2;
    }

    for (i = 0; i < 6; i++) {
        sscanf(str + using_hex_sign + i * (2 + using_long_format), "%02x", &value);
        addr[5 - i] = (uint8_t) value;
    }
}

void mesh_app_util_copy_str_to_byte(uint8_t *des, const char *str, uint32_t len)
{
    unsigned int i, value = 0;
    int using_long_format = 0;
    int using_hex_sign = 0;

    if (str[2] == ':' || str[2] == '-') {
        using_long_format = 1;
    }

    if (str[1] == 'x') {
        using_hex_sign = 2;
    }

    for (i = 0; i < len; i++) {
        sscanf(str + using_hex_sign + i * (2 + using_long_format), "%02x", &value);
        des[i] = (uint8_t) value;
        value = 0;
    }
}

void bt_mesh_app_util_init_timer(TimerHandle_t *timer_id, char *name, TimerCallbackFunction_t pxCallbackFunction)
{
    if (*timer_id == NULL) {
        *timer_id = xTimerCreate((const char *const)name, 0xffff, pdFALSE, NULL, pxCallbackFunction);
    }
}

void bt_mesh_app_util_deinit_timer(TimerHandle_t timer_id)
{
    if (timer_id != NULL) {
        xTimerDelete(timer_id, 0);
        timer_id = NULL;
    }
}

void bt_mesh_app_util_stop_timer(TimerHandle_t timer_id)
{
    if  ((timer_id != NULL) && (bt_mesh_app_util_is_timer_active(timer_id) == 1)) {
        xTimerStop(timer_id, portMAX_DELAY);
    }
}

uint32_t bt_mesh_app_util_is_timer_active(TimerHandle_t timer_id)
{
    if ((timer_id != NULL) && (xTimerIsTimerActive(timer_id) != pdFALSE)) {
        return 1;
    } else {
        return 0;
    }
}

void bt_mesh_app_util_start_timer(TimerHandle_t timer_id, uint32_t ms)
{
    uint32_t time_length = ms / portTICK_PERIOD_MS + 1;
    if (timer_id == NULL) {
        return;
    }
    if (bt_mesh_app_util_is_timer_active(timer_id) == 1) {
        bt_mesh_app_util_stop_timer(timer_id);
    }
    xTimerChangePeriod(timer_id, time_length, portMAX_DELAY);
    xTimerReset(timer_id, portMAX_DELAY);
}

void bt_mesh_app_util_start_timer_ext(char *name, TimerHandle_t *timer_id, bool is_repeat, uint32_t ms, TimerCallbackFunction_t pxCallbackFunction)
{
    if (*timer_id == NULL) {
        *timer_id = xTimerCreate((const char *const)name, 0xffff, is_repeat, NULL, pxCallbackFunction);
    }
    uint32_t time_length = ms / portTICK_PERIOD_MS + 1;
    if (*timer_id == NULL) {
        return;
    }
    if (bt_mesh_app_util_is_timer_active(*timer_id) == 1) {
        bt_mesh_app_util_stop_timer(*timer_id);
    }
    xTimerChangePeriod(*timer_id, time_length, portMAX_DELAY);
    xTimerReset(*timer_id, portMAX_DELAY);
}

uint8_t *bt_mesh_app_util_alloc(uint16_t size)
{
    uint8_t *p = (uint8_t *)pvPortMalloc(size);
    return ((p == NULL) ? NULL : p);
}

void bt_mesh_app_util_free(uint8_t *p)
{
    vPortFree(p);
    p = NULL;
}

bt_status_t bt_mesh_app_util_str2u8HexNum(const char *src, uint8_t *dst)
{
    uint32_t temp;
    bt_mesh_app_util_str2u32HexNum(src, &temp);
    *dst = (uint8_t)temp;
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_mesh_app_util_str2u16HexNum(const char *src, uint16_t *dst)
{
    uint32_t temp;
    bt_mesh_app_util_str2u32HexNum(src, &temp);
    *dst = (uint16_t)temp;
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_mesh_app_util_str2u32HexNum(const char *src, uint32_t *dst)
{
    char *end;
    *dst = (uint32_t)strtoul(src, &end, 10);
    if (*end) {
        *dst = (uint32_t)strtoul(src, &end, 16);
        if (*end) {
            return BT_STATUS_FAIL;
        }
    }
    return BT_STATUS_SUCCESS;
}

uint8_t bt_mesh_app_util_str2u8HexNumArray(const char *src, uint8_t *dst)
{
    uint8_t len = strlen(src) / 2;
    mesh_app_util_copy_str_to_byte(dst, src, (uint32_t)len);
    return len;
}

void bt_mesh_app_util_display_hex_log(const uint8_t *buffer, uint16_t len)
{
    uint16_t i, k, j = 0;
    do {
        for (i = 0; i < 8; i++) {
            printf("%02x", buffer[j * 8 + i]);
        }
        printf("\n");
        j++;
    } while (j < len / 8);

    if (j == len / 8 && (k = len % 8) != 0) {
        printf("k = %x\n", k);
        for (i = 0; i < k; i++) {
            printf("%02x", buffer[i]);
        }
        printf("\n");
    }
}

void mesh_app_util_copy_str_to_2byte(uint16_t *des, const char *str, uint32_t len)
{
    unsigned int i, value = 0;
    int using_long_format = 0;
    int using_hex_sign = 0;

    if (str[2] == ':' || str[2] == '-') {
        using_long_format = 1;
    }

    if (str[1] == 'x') {
        using_hex_sign = 2;
    }

    for (i = 0; i < len; i++) {
        sscanf(str + using_hex_sign + i * (2 + using_long_format), "%04x", &value);
        //LOG_I(mesh_app, "copy_str_to_2byte value = %x\n", value);
        des[i] = (uint16_t)value;
        //LOG_I(mesh_app, "copy_str_to_2byte des = %x\n", des[i]);
        value = 0;
    }
}

uint8_t bt_mesh_app_util_str2u16HexNumArray(uint16_t argc, char **argv, uint16_t *dst_list)
{
    for (int i = 0; i < argc; i++) {
        uint8_t len = strlen(argv[i]);
        mesh_app_util_copy_str_to_2byte(dst_list + i, argv[i], (uint32_t)len);
    }
    return 0;
}

void bt_mesh_app_util_str2u8HexAddrArray(const char *src, uint8_t *dst)
{
    mesh_app_util_copy_str_to_addr(dst, src);
}

void bt_mesh_app_util_dump_pdu(char *pdu_name, uint8_t pdu_len, uint8_t *pdu)
{
    uint8_t i;
    printf("%s[%d] :", pdu_name, pdu_len);
    for (i = 0; i < pdu_len; i++) {
        printf(" %02x", pdu[i]);
    }
    printf("\n");
}

void bt_mesh_app_util_dump_u16_pdu(char *pdu_name, uint8_t pdu_len, uint16_t *pdu)
{
    uint8_t i;
    printf("%s[%d] :", pdu_name, pdu_len);
    for (i = 0; i < pdu_len; i++) {
        printf(" 0x%04x", pdu[i]);
    }
    printf("\n");
}


/*why bt_mesh_app_util_free by app*/
void mesh_app_check_and_free(uint8_t *value)
{
    if (value != NULL) {
        bt_mesh_app_util_free(value);
        value = NULL;
    }
}

void bt_mesh_app_util_check_device_uuid(uint8_t *device_uuid)
{
    nvdm_status_t status;
    int8_t i;
    uint32_t random_seed;
    uint32_t size = BT_MESH_UUID_SIZE;
    hal_trng_status_t ret = HAL_TRNG_STATUS_ERROR;
    char uuid_buf[BT_MESH_UUID_SIZE * 2 + 5] = {0};

    //LOG_I(mesh_app, "[MESH]%s Try to read from NVDM.\n", __FUNCTION__);
    status = nvdm_read_data_item("Mesh", "device_uuid", device_uuid, &size);
    if (NVDM_STATUS_OK == status && size == BT_MESH_UUID_SIZE) {
        for (i = 0; i < BT_MESH_UUID_SIZE; ++i) {
            snprintf(uuid_buf + 2 * i, 3, "%02x", device_uuid[i]);
        }
        LOG_I(mesh_app, "[MESH]Device UUID from NVDM: %s", uuid_buf);
    } else {
        LOG_I(mesh_app, "[MESH]Failed to Read from NVDM:%x.\n", status);
        ret = hal_trng_init();
        if (HAL_TRNG_STATUS_OK != ret) {
            LOG_I(mesh_app, "[MESH]generate uuid--error 1");
        }
        for (i = 0; i < 30; ++i) {
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(mesh_app, "[MESH]generate uuid--error 2");
            }
            LOG_I(mesh_app, "[MESH]generate uuid--trn: 0x%x", random_seed);
        }
        for (i = 0; i < 4; i++) {
            /* uuid generate */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(mesh_app, "[MESH]generate uuid--error 3");
            }
            LOG_I(mesh_app, "[MESH]generate uuid--trn %d: 0x%x", i, random_seed);
            device_uuid[i * 4] = random_seed & 0xFF;
            device_uuid[i * 4 + 1] = (random_seed >> 8) & 0xFF;
            device_uuid[i * 4 + 2] = (random_seed >> 16) & 0xFF;
            device_uuid[i * 4 + 3] = (random_seed >> 24) & 0xFF;
        }
        hal_trng_deinit();

        /* save address to NVDM */
        status = nvdm_write_data_item("Mesh", "device_uuid", NVDM_DATA_ITEM_TYPE_RAW_DATA, device_uuid, BT_MESH_UUID_SIZE);
        if (NVDM_STATUS_OK != status) {
            LOG_I(mesh_app, "[MESH]Failed to store uuid.\n");
        } else {
            for (i = 0; i < BT_MESH_UUID_SIZE; ++i) {
                snprintf(uuid_buf + 2 * i, 3, "%02x", device_uuid[i]);
            }
            LOG_I(mesh_app, "[MESH]Successfully store device uuid to NVDM: %s", uuid_buf);
        }
    }
    return;
}


bt_status_t cmd_provision_remove(void *no, uint16_t argc, char **argv)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t cmd_access_attention(void *no, uint16_t argc, char **argv)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t cmd_adv(void *no, uint16_t argc, char **argv)
{
    if (argc < 2) {
        LOG_I(mesh_app, "       adv set param [adv_period] [adv_min_interval] [adv_max_interval] [adv_resend] [scan_period] [scan_interval]\n");
        return BT_STATUS_FAIL;
    }

    if (!strcmp(argv[1], "set")) {
        if (!strcmp(argv[2], "param")) {
            bt_mesh_bearer_adv_params_t adv_params;
            bt_mesh_bearer_scan_params_t scan_params;

            bt_mesh_app_util_str2u32HexNum(argv[3], &adv_params.adv_period);
            bt_mesh_app_util_str2u16HexNum(argv[4], &adv_params.min_interval);
            bt_mesh_app_util_str2u16HexNum(argv[5], &adv_params.max_interval);
            bt_mesh_app_util_str2u8HexNum(argv[6], &adv_params.resend);
            bt_mesh_app_util_str2u32HexNum(argv[7], &scan_params.scan_period);
            bt_mesh_app_util_str2u16HexNum(argv[8], &scan_params.scan_interval);
            bt_mesh_app_util_str2u16HexNum(argv[9], &scan_params.scan_window);

            bt_mesh_bearer_adv_set_params(&adv_params, &scan_params);
        }
    }
    return BT_STATUS_SUCCESS;
}

static void _dump_lpn_poll_timeout(uint32_t count, bt_mesh_lpn_poll_timeout_t *data)
{
    uint32_t i = 0;
    LOG_I(mesh_app, "dump lpn poll timeout list: num=0x%02x\n", count);
    for (; i < count; i++) {
        LOG_I(mesh_app, "lpn_address: 0x%02x\n", ((bt_mesh_lpn_poll_timeout_t)data[i]).lpn_address);
        LOG_I(mesh_app, "poll_timeout: 0x%02x\n", ((bt_mesh_lpn_poll_timeout_t)data[i]).poll_timeout);
    }
}

static void _dump_heartbeat_publication_state(bt_mesh_heartbeat_publication_t *data)
{
    LOG_I(mesh_app, "dest: 0x%02x\n", data->dest);
    LOG_I(mesh_app, "count: 0x%02x\n", data->count);
    LOG_I(mesh_app, "period_log: 0x%x\n", data->period_log);
    LOG_I(mesh_app, "ttl: 0x%x\n", data->ttl);
    LOG_I(mesh_app, "features: 0x%02x\n", data->features);
    LOG_I(mesh_app, "netkey_index: 0x%02x\n", data->netkey_index);
}

static void _dump_publication_state(bt_mesh_model_publication_t *data)
{
    LOG_I(mesh_app, "addr: 0x%02x\n", data->addr);
    LOG_I(mesh_app, "appkey_index: 0x%02x\n", data->appkey_index);
    LOG_I(mesh_app, "flag: 0x%02x\n", data->flag);
    LOG_I(mesh_app, "rfu: 0x%02x\n", data->rfu);
    LOG_I(mesh_app, "ttl: 0x%x\n", data->ttl);
    LOG_I(mesh_app, "period: 0x%x\n", data->period);
    LOG_I(mesh_app, "retransmit_count: 0x%x\n", data->retransmit_count);
    LOG_I(mesh_app, "retransmit_interval_steps: 0x%x\n", data->retransmit_interval_steps);
}

typedef struct {
    uint16_t loc;
    uint8_t numS;
    uint8_t numV;
    void *sig_models;  // double-linked list
    void *vendor_models;  // double-lined list
} mesh_composition_element_t;

static void _dump_composition_data(bt_mesh_composition_data_t *data)
{
    LOG_I(mesh_app, "CID: 0x%04x\n", data->cid);
    LOG_I(mesh_app, "PID: 0x%04x\n", data->pid);
    LOG_I(mesh_app, "VID: 0x%04x\n", data->vid);
    LOG_I(mesh_app, "CRPL: 0x%04x\n", data->crpl);
    LOG_I(mesh_app, "Features: 0x%04x\n", data->features);
    LOG_I(mesh_app, "count: 0x%04x\n", bt_mesh_os_layer_ds_dlist_count(data->elements));
    if (data->elements != NULL && bt_mesh_os_layer_ds_dlist_count(data->elements) > 0) {
        uint8_t i = 0;
        uint32_t *model_id = NULL;
        mesh_composition_element_t **entry = (mesh_composition_element_t **)bt_mesh_os_layer_ds_dlist_first(data->elements);
        while (entry != NULL) {
            LOG_I(mesh_app, "\tLOC: 0x%04x\n", (*entry)->loc);
            LOG_I(mesh_app, "\tNumS: %d\n", (*entry)->numS);

            if ((*entry)->numS > 0) {
                model_id = (uint32_t *)bt_mesh_os_layer_ds_dlist_first((*entry)->sig_models);

                while (model_id != NULL) {
                    LOG_I(mesh_app, "\t[%d] %04x\n", i, *model_id);
                    i++;
                    model_id = (uint32_t *)bt_mesh_os_layer_ds_dlist_next((*entry)->sig_models, (void *)model_id);
                }
            }

            LOG_I(mesh_app, "\tNumV: %d\n", (*entry)->numV);
            if ((*entry)->numV > 0) {
                i = 0;
                model_id = (uint32_t *)bt_mesh_os_layer_ds_dlist_first((*entry)->vendor_models);

                while (model_id != NULL) {
                    LOG_I(mesh_app, "\t[%d] %08x\n", i, *model_id);
                    i++;
                    model_id = (uint32_t *)bt_mesh_os_layer_ds_dlist_next((*entry)->vendor_models, (void *)model_id);
                }
            }
            entry = bt_mesh_os_layer_ds_dlist_next(data->elements, (void *)entry);
        }
    }
}

extern void mesh_dump_model_handle(void);
extern void mesh_network_dump( void );
extern void mesh_transport_dump( void );
extern void mesh_config_dump( void );
extern void mesh_model_dump( void );
extern void mesh_lpn_dump( void );
extern void mesh_bearer_adv_dump( void );

bt_status_t cmd_dump(void *no, uint16_t argc, char **argv)
{
    if (argc < 2) {
        mesh_network_dump();
        mesh_transport_dump();
        mesh_config_dump();
        mesh_model_dump();
        mesh_lpn_dump();
        mesh_bearer_adv_dump();
    }

    if (!strcmp(argv[1], "net")) {
        mesh_network_dump();
    } else if (!strcmp(argv[1], "trans")) {
        mesh_transport_dump();
    } else if (!strcmp(argv[1], "access")) {
        //mesh_access_dump();
    } else if (!strcmp(argv[1], "config")) {
        mesh_config_dump();
    } else if (!strcmp(argv[1], "model")) {
        mesh_model_dump();
    } else if (!strcmp(argv[1], "lpn")) {
        mesh_lpn_dump();
#if MESH_PROXY
    } else if (!strcmp(argv[1], "proxy")) {
        mesh_proxy_dump();
#endif
    } else if (!strcmp(argv[1], "adv")) {
        mesh_bearer_adv_dump();
    } else if (!strcmp(argv[1], "composition_data")) {
        bt_mesh_composition_data_t *data;
        data = bt_mesh_model_get_composition_data();
        _dump_composition_data(data);
    } else if (!strcmp(argv[1], "UUID")) {
        uint8_t *uuid;
        uuid = bt_mesh_config_get_uuid();
        bt_mesh_app_util_dump_pdu("UUID", BT_MESH_UUID_SIZE, uuid);
    } else if (!strcmp(argv[1], "unicast_addr")) {
        uint16_t element_index;
        bt_mesh_app_util_str2u16HexNum(argv[2], &element_index);
        uint16_t addr = bt_mesh_model_get_element_address(element_index);
        LOG_I(mesh_app, "element address is %x\n", addr);
    } else if (!strcmp(argv[1], "device_key")) {
        uint16_t addr;
        bt_mesh_app_util_str2u16HexNum(argv[2], &addr);
        bt_mesh_config_devkey_entry_t *deventry = bt_mesh_config_get_device_key(addr);
        if (deventry) {
            bt_mesh_app_util_dump_pdu("DEVKEY", BT_MESH_DEVKEY_SIZE, deventry->device_key);
        } else {
            LOG_I(mesh_app, "no device key of remote device is available\n");
        }
    } else if (!strcmp(argv[1], "appkey_index_list")) {
        uint16_t netkey_idx;
        uint32_t keyCount;
        uint16_t *key_list;
        bt_mesh_app_util_str2u16HexNum(argv[2], &netkey_idx);

        key_list = (void *)bt_mesh_config_get_application_key_index_list(netkey_idx, &keyCount);
        if (key_list) {
            bt_mesh_app_util_dump_u16_pdu("APP_KEY_INDEX_LIST", keyCount, key_list);
        } else {
            LOG_I(mesh_app, "no appkey index of remote device is available\n");
        }
    } else if (!strcmp(argv[1], "netkey_index_list")) {
        uint32_t keyCount;
        uint16_t *key_list;
        key_list = bt_mesh_config_get_network_key_index_list(&keyCount);
        if (key_list) {
            bt_mesh_app_util_dump_u16_pdu("NET_KEY_INDEX_LIST", keyCount, key_list);
        } else {
            LOG_I(mesh_app, "no netkey index of remote device is available\n");
        }
    } else if (!strcmp(argv[1], "bound_appkey_index_list")) {
        uint16_t model_handle;
        uint32_t keyCount;
        uint16_t *key_list;
        bt_mesh_app_util_str2u16HexNum(argv[2], &model_handle);
        key_list = bt_mesh_config_get_bound_application_key_index_list(model_handle, &keyCount);
        if (key_list) {
            bt_mesh_app_util_dump_u16_pdu("BOUND_APP_KEY_INDEX_LIST", keyCount, key_list);
        } else {
            LOG_I(mesh_app, "no bound appkey index of remote device is available\n");
        }
    } else if (!strcmp(argv[1], "ttl")) {
        uint8_t ttl = bt_mesh_config_get_default_ttl();
        LOG_I(mesh_app, "default ttl is %x\n", ttl);
    } else if (!strcmp(argv[1], "network_transmit")) {
        bt_mesh_network_transmit_t *data;
        data = bt_mesh_config_get_network_transmit();
        LOG_I(mesh_app, "network transmit count is %x, interval_steps %x\n",
              data->network_transmit_count,
              data->network_transmit_interval_steps);
    } else if (!strcmp(argv[1], "relay_transmit")) {
        bt_mesh_relay_retransmit_t *data;
        data = bt_mesh_config_get_relay_retransmit();
        LOG_I(mesh_app, "relay transmit count is %x, interval_steps %x\n",
              data->relay_retransmit_count,
              data->relay_retransmit_interval_steps);
    } else if (!strcmp(argv[1], "sn_beacon_state")) {
        uint8_t state = bt_mesh_config_get_secure_network_beacon_state();
        LOG_I(mesh_app, "secure network beacon state is %x\n", state);
    } else if (!strcmp(argv[1], "sublist")) {
        uint16_t model_handle;
        uint32_t addr_count;
        uint16_t *addr_list;
        bt_mesh_app_util_str2u16HexNum(argv[2], &model_handle);
        addr_list = bt_mesh_model_get_subscription_list(model_handle, &addr_count);
        if (addr_list) {
            bt_mesh_app_util_dump_u16_pdu("SUB_LIST", addr_count, addr_list);
        } else {
            LOG_I(mesh_app, "no bound appkey index of remote device is available\n");
        }
    } else if (!strcmp(argv[1], "pub_state")) {
        uint16_t model_handle;
        bt_mesh_app_util_str2u16HexNum(argv[2], &model_handle);
        bt_mesh_model_publication_t *publication_state;
        publication_state = bt_mesh_model_get_publication_state(model_handle);
        if (publication_state) {
            _dump_publication_state(publication_state);
        } else {
            LOG_I(mesh_app, "no publication state is available\n");
        }
    } else if (!strcmp(argv[1], "heartbeat_pub_state")) {
        bt_mesh_heartbeat_publication_t *heartbeat_pub_state;
        heartbeat_pub_state = bt_mesh_config_get_heartbeat_publication_state();
        if (heartbeat_pub_state) {
            _dump_heartbeat_publication_state(heartbeat_pub_state);
        } else {
            LOG_I(mesh_app, "no heartbeat publication state is available\n");
        }
    } else if (!strcmp(argv[1], "lpn_poll_timeout")) {
        bt_mesh_lpn_poll_timeout_t *data;
        uint32_t count;
        data = bt_mesh_config_get_lpn_poll_timeout(&count);
        if (data) {
            _dump_lpn_poll_timeout(count, data);
        } else {
            LOG_I(mesh_app, "no lpn poll timeout is available\n");
        }
    } else if (!strcmp(argv[1], "model_handle")) {
        mesh_dump_model_handle();
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t cmd_power(void *no, uint16_t argc, char **argv)
{
    if (argc < 2) {
        LOG_I(mesh_app, "USAGE: power reset\n");
        return BT_STATUS_FAIL;
    }

    if (!strcmp(argv[1], "reset")) {
        LOG_I(mesh_app, "SW reset!!\n");
        hal_cache_disable();
        hal_cache_deinit();
        hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
    }

    return BT_STATUS_SUCCESS;
}


