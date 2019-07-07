#ifndef __BLE_SERIAL_H__
#define __BLE_SERIAL_H__

#include "bt_type.h"
#include "bt_system.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "bt_gap_le.h"
#include "bt_platform.h"


BT_EXTERN_C_BEGIN

typedef uint16_t ble_bas_event_t;   /**< The event type of BAS. */
#define BLE_BAS_EVENT_BATTRY_LEVEL_READ         0x00 /**< Battery Level Characteristic Read event. */
#define BLE_BAS_EVENT_CCCD_READ                 0x01 /**< Client Characteristic Configuration Read event. */
#define BLE_BAS_EVENT_CCCD_WRITE                0x02 /**< Client Characteristic Configuration Write event. */

/**
 * @}
 */

/**
 * @brief An BAS Service. 
 */
extern const bt_gatts_service_t ble_bas_service;

/**
 * @brief   This function sends a notification to a peer device about the current battery level.
 * @param[in] conn_handle        is the connection handle.
 * @param[in] battery_level      is the current battery level.
 * @return    BT_STATUS_SUCCESS, the notification is sent successfully.
 *            BT_STATUS_FAIL, the notification failed to send.
 */
bt_status_t ble_bas_notify_battery_level(bt_handle_t conn_handle, uint8_t battery_level);


/**
 * @brief   This function is a user-defined callback for the application to listen to the read event of Battery Level Characteristic and Client Configuration Characteristic Descriptor.
 * @param[in] event           is the event of type @ref ble_bas_event_t.
 * @param[in] conn_handle     is the connection handle.
 * @return                    the response to the read event of Battery Level Characteristic or Client Configuration Characteristic Descriptor. If the read event type is @ref BLE_BAS_EVENT_BATTRY_LEVEL_READ, 
 *                            the return value is the corresponding battery level. If the read event type is @ref BLE_BAS_EVENT_CCCD_READ, the return value is either 0 or 1.
 */
uint8_t ble_bas_read_callback(ble_bas_event_t event, bt_handle_t conn_handle);

/**
 * @brief   This function is a user-defined callback for the application to listen to the write event of Client Configuration Characteristic Descriptor.
 * @param[in] event           is the event of type @ref ble_bas_event_t.
 * @param[in] conn_handle     is the connection handle.
 * @param[in] data            is the data of write event from a remote device.
 * @return    void.
 */
void ble_bas_write_callback(ble_bas_event_t event, bt_handle_t conn_handle, void *data);


BT_EXTERN_C_END
/**
 * @}
 * @}
 */

#endif /*__BLE_BAS_H__*/


