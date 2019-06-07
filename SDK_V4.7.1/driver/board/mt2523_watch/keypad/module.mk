
MT2523_WATCH_SRC = driver/board/mt2523_watch
COMPONENT_SRC = driver/board/component

#keypad custome file
C_FILES  += $(MT2523_WATCH_SRC)/keypad/src/keypad_custom.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(MT2523_WATCH_SRC)/lcd

#ctp driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt2523_watch/keypad/inc

