
WIFI_HOST_COMMON_SRC = middleware/MTK/wifi_host/common
C_FILES  += $(WIFI_HOST_COMMON_SRC)/src/wifi_host_init.c
C_FILES  += $(WIFI_HOST_COMMON_SRC)/src/wifi_host_event.c
C_FILES  += $(WIFI_HOST_COMMON_SRC)/src/wifi_host_api.c
C_FILES  += $(WIFI_HOST_COMMON_SRC)/src/wifi_host_private_api.c
C_FILES  += $(WIFI_HOST_COMMON_SRC)/src/host_os_utils.c
C_FILES  += $(WIFI_HOST_COMMON_SRC)/src/wifi_host_default_config.c

#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_host/common/inc

