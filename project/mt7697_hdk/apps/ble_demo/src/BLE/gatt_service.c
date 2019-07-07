#include <string.h>

#include "bt_uuid.h"
#include "bt_system.h"
#include "bt_gattc.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "bt_debug.h"

#include "project_config.h"
#include "syslog.h"

#include "ble_serial.h"


/* Create the log control block as user wishes. Here we use 'BLE_GATT' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(BLE_GATT, PRINT_LEVEL_INFO);

//Declare every record here
//service collects all bt_gatts_service_rec_t
//IMPORTAMT: handle:0x0000 is reserved, please start your handle from 0x0001

#if 0
//GATT 0x0014 - 0x0017
/*---------------------------------------------*/
const bt_uuid_t APP_BLE_FOTA_CHAR_UUID128 = BT_UUID_INIT_WITH_UUID16(APP_BLE_FOTA_CHAR_UUID);

extern uint32_t app_ble_fota_charc_value_callback (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
extern uint32_t app_ble_fota_client_config_callback (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

BT_GATTS_NEW_PRIMARY_SERVICE_16(_bt_if_dtp_primary_service, APP_BLE_FOTA_SERVICE_UUID);

BT_GATTS_NEW_CHARC_16(_bt_if_dtp_char,
                      BT_GATT_CHARC_PROP_WRITE | BT_GATT_CHARC_PROP_INDICATE, 
                      APP_BLE_FOTA_CHAR_VALUE_HANDLE, APP_BLE_FOTA_CHAR_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(_bt_if_dtp_char_value, APP_BLE_FOTA_CHAR_UUID128,
                                  BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, 
                                  app_ble_fota_charc_value_callback);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(_bt_if_dtp_client_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 app_ble_fota_client_config_callback);

static const bt_gatts_service_rec_t *_bt_if_ble_fota_service_rec[] = {
    (const bt_gatts_service_rec_t *) &_bt_if_dtp_primary_service,
    (const bt_gatts_service_rec_t *) &_bt_if_dtp_char,
    (const bt_gatts_service_rec_t *) &_bt_if_dtp_char_value,
    (const bt_gatts_service_rec_t *) &_bt_if_dtp_client_config
};

static const bt_gatts_service_t _bt_if_ble_fota_service = {
    .starting_handle = 0x0014,
    .ending_handle = 0x0017,
    .required_encryption_key_size = 0,
    .records = _bt_if_ble_fota_service_rec
};
#endif

// from bt_notify middleware
extern const bt_gatts_service_t ble_bas_service;

//server collects all service
static const bt_gatts_service_t * _bt_bas_server[] = {
    &ble_bas_service,
    NULL
    };

uint8_t ble_bas_read_callback(ble_bas_event_t event, bt_handle_t conn_handle){
    uint8_t ret = 0;
    LOG_I(BLE_GATT, "ble_bas_read_callback, event:%d, conn_handle:%d \r\n", event, conn_handle);
    return ret;
}
void ble_bas_write_callback(ble_bas_event_t event, bt_handle_t conn_handle, void *data){
    LOG_I(BLE_GATT, "ble_bas_write_callback, event:%d, conn_handle:%d \r\n", event, conn_handle);
};

//When GATTS get req from remote client, GATTS will call bt_get_gatt_server() to get application's gatt service DB.
//You have to return the DB(bt_gatts_service_t pointer) to gatts stack.
const bt_gatts_service_t** bt_get_gatt_server()
{
    return _bt_bas_server;
}

