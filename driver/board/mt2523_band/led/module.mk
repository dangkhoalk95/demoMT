
###################################################
BOARD_SRC = driver/board/mt2523_band
BOARD_LED_SRC = $(BOARD_SRC)/led/src
BOARD_COMPONENT_LED_SRC = driver/board/component/led/src


C_FILES += $(BOARD_LED_SRC)/bsp_led_internal.c
C_FILES += $(BOARD_COMPONENT_LED_SRC)/bsp_led.c

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt2523_band/led/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/common
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/led/inc

#################################################################################
CFLAGS         += -DMTK_LED_ENABLE
