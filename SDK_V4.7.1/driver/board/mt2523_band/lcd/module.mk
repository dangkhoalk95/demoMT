
###################################################
BOARD_SRC = driver/board/mt2523_band
BOARD_LCD_SRC = driver/board/mt2523_band/lcd
COMPONENT_SRC = driver/board/component/lcm/SH1107_band


C_FILES += $(BOARD_LCD_SRC)/mt25x3_hdk_lcd.c
C_FILES += $(COMPONENT_SRC)/lcd.c
C_FILES += $(BOARD_SRC)/backlight/mt25x3_hdk_backlight.c

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/lcm/SH1107_band
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt2523_band/lcd
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt2523_band/backlight
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/common
