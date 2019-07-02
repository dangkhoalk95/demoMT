# Copyright Statement:
# Author: Dang Khoa
# Email: 41301836@hcmut.edu.vn

SOURCE_DIR = ../../..
BINPATH     = ~/gcc-arm-none-eabi/bin
PROJ_PATH   ?= $(SOURCE_DIR)/project/mt7697_hdk/apps/bt_demo/GCC
CONFIG_PATH ?= .

CFLAGS += -I$(PROJ_PATH)/../inc
CFLAGS += -I$(SOURCE_DIR)/$(CONFIG_PATH)

FEATURE    ?= feature.mk

include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

# Project name
TARGET_LIB=libble_serial

BUILD_DIR = Build
OUTPATH = Build


# Sources
include module.mk


C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)


.PHONY: $(TARGET_LIB).a

all: $(TARGET_LIB).a
	@echo Build $< Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)/$(TARGET_LIB).a
	rm -rf $(BUILD_DIR)


