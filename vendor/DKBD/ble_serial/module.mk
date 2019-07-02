BLE_SERIAL_SRC := vendor/DKBD/ble_serial/src

C_FILES  += $(BLE_SERIAL_SRC)/bt_notify_conversion.c \
            $(BLE_SERIAL_SRC)/bt_notify_data_parse.c \
            $(BLE_SERIAL_SRC)/bt_notify_list.c \
            $(BLE_SERIAL_SRC)/xml_main.c \
            $(BLE_SERIAL_SRC)/bt_notify_task.c \
            $(BLE_SERIAL_SRC)/dogp/ble_dogp_adp_service.c \
            $(BLE_SERIAL_SRC)/dogp/ble_dogp_common.c \
            $(BLE_SERIAL_SRC)/dogp/ble_dogp_service.c


ifeq ($(MTK_BLE_ONLY_ENABLE), y)
C_FILES +=  $(BLE_SERIAL_SRC)/bt_spp_dummy.c
endif
#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/vendor/DKBD/ble_serial/inc
CFLAGS 	+= -I$(SOURCE_DIR)/vendor/DKBD/ble_serial/inc/dogp
CFLAGS 	+= -I$(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 

## Need lib, try to write the lib by your own
# ifneq ($(wildcard $(strip $(SOURCE_DIR))/vendor/MTK/bt_notify_protected/),)
# include $(SOURCE_DIR)/middleware/MTK/bt_notify_protected/src_protected/GCC/module.mk
# else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/bt_notify/module.mk
# endif

CFLAGS += -DMTK_BT_NOTIFY_ENABLE
