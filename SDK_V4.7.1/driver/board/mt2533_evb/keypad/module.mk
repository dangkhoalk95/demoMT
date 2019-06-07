
KEYPAD_BOARD_SRC     = driver/board/mt2533_evb
KEYPAD_COMPONENT_SRC = driver/board/component

#keypad custome file
C_FILES  += $(KEYPAD_BOARD_SRC)/keypad/src/keypad_custom.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc

#ctp driver include path
CFLAGS  += -I$(SOURCE_DIR)/$(KEYPAD_BOARD_SRC)/keypad/inc
CFLAGS  += -DMTK_KEYPAD_ENABLE
