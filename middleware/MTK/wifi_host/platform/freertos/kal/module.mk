
WIFI_HOST_KAL_SRC = middleware/MTK/wifi_host/platform/freertos/kal
C_FILES  += $(WIFI_HOST_KAL_SRC)/src/wifi_host_os_api.c

#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_host/platform/freertos/kal/inc

