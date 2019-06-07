
EPT_SRC = driver/board/mt2523_band/ept


C_FILES  += $(EPT_SRC)/src/bsp_gpio_ept_config.c


#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt2523_band/ept/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include


