###################################################
# Sources
###################################################

BLE_BAS_SOURCE = vendor/DKBD/ble_serial/src

C_FILES  += $(BLE_BAS_SOURCE)/ble_serial.c

###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/vendor/DKBD/ble_serial/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
