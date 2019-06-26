# Copyright Statement:
#
# (C) 2005-2016  MediaTek Inc. All rights reserved.
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
# Without the prior written permission of MediaTek and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
# if you have agreed to and been bound by the applicable license agreement with
# MediaTek ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of MediaTek Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#

SOURCE_DIR = ../../../../..
BINPATH = $(SOURCE_DIR)/tools/gcc/gcc-arm-none-eabi/bin

PWD= $(shell pwd)
SDK_PATH    = $(abspath $(PWD)/$(SOURCE_DIR))

FEATURE ?= feature.mk
include $(FEATURE)
# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

export WITH_BOOTLOADER=n
#CC=$(BINPATH)/arm-none-eabi-gcc
#CXX=$(BINPATH)/arm-none-eabi-g++
#OBJCOPY=$(BINPATH)/arm-none-eabi-objcopy
#SIZE=$(BINPATH)/arm-none-eabi-size

BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME=mt7686_bootloader
OUTPATH = $(PWD)/Build
PROJ_PATH = $(PWD)

APP_PATH        = $(patsubst $(SDK_PATH)/%,%,$(abspath $(dir $(PWD))))

# HAL driver files
include $(SOURCE_DIR)/driver/chip/mt7686/module.mk

# lzma files
include $(SOURCE_DIR)/middleware/third_party/lzma_decoder/module.mk

ifeq ($(MTK_BOOTLOADER_XIP),y)
CFLAGS += -DMTK_BOOTLOADER_XIP
S_FILES += $(APP_PATH)/GCC/startup_bootloader_xip.s
else
S_FILES += $(APP_PATH)/GCC/startup_bootloader.s
endif

C_FILES += $(APP_PATH)/GCC/syscalls.c
C_FILES += $(APP_PATH)/src/bl_dbgprint.c
C_FILES += $(APP_PATH)/src/custom_blconfig.c
C_FILES += $(APP_PATH)/src/main.c

ifeq ($(MTK_FOTA_ENABLE),y)
ifeq ($(MTK_BL_FOTA_CUST_ENABLE),y)
C_FILES += $(APP_PATH)/src/custom_fota.c
else
C_FILES += $(APP_PATH)/src/bl_fota.c
C_FILES += middleware/third_party/mbedtls/library/sha1.c
CFLAGS += -DMBEDTLS_CONFIG_FILE='<config-mtk-sha1.h>'
endif
endif

# fatfs files
ifeq ($(MTK_FOTA_FS_ENABLE),y)
include $(SOURCE_DIR)/middleware/third_party/fatfs/module.mk
endif

ifeq ($(MTK_BL_DCXO_KVALUE_SW),y)
CFLAGS += -DBL_DCXO_KVALUE_SW
endif

ifeq ($(MTK_CAL_DCXO_CAPID),0)
CFLAGS += -DBL_RUN_DCXO_CAL
  ifneq ($(MTK_BL_DCXO_KVALUE_SW),y)
  CFLAGS += -DBL_CAL_DCXO_CAPID0
  endif
endif

ifeq ($(MTK_CAL_DCXO_CAPID),1)
CFLAGS += -DBL_RUN_DCXO_CAL
  ifneq ($(MTK_BL_DCXO_KVALUE_SW),y)
  CFLAGS += -DBL_CAL_DCXO_CAPID1
  endif
endif

ifeq ($(MTK_CAL_DCXO_CAPID),2)
CFLAGS += -DBL_RUN_DCXO_CAL
  ifneq ($(MTK_BL_DCXO_KVALUE_SW),y)
  CFLAGS += -DBL_CAL_DCXO_CAPID2
  endif
endif

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################
# CC Flags
ALLFLAGS = -g -Os
ALLFLAGS += -Wall -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += $(ALLFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin

ifeq ($(FLOAT_TYPE), hard)
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
FPUFLAGS = -msoft-float
endif

# Definitions
CFLAGS += $(FPUFLAGS)

ifeq ($(MTK_BL_FPGA_LOAD_ENABLE),y)
CFLAGS += -D__FPGA_TARGET__
endif

ifeq ($(MTK_BL_DEBUG_ENABLE),y)
CFLAGS += -DBL_DEBUG
endif

ifeq ($(MTK_FOTA_ENABLE),y)
CFLAGS += -DBL_FOTA_ENABLE
CFLAGS += -DMOD_CFG_FOTA_BL_RESERVED
endif

ifeq ($(MTK_NO_PSRAM_ENABLE),y)
CFLAGS += -DMTK_NO_PSRAM_ENABLE
endif


ifeq ($(MTK_BL_LOAD_ENABLE),y)
CFLAGS += -DBL_LOAD_ENABLE
endif

# LOG level, please refer to bl_log_level_t at bl_common.h
ifeq ($(MTK_USER_FAST_TX_ENABLE),y)
CFLAGS += -DBL_LOG_LEVEL=LOG_INFO
endif

CFLAGS += -D__UBL__ -D__EXT_BOOTLOADER__

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles

ifeq ($(MTK_BOOTLOADER_XIP),y)
LDFLAGS += -Wl,-Tbootloader_xip.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-Tbootloader.ld -Wl,--gc-sections
endif

ifeq ($(DEBUG), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
endif
#LDFLAGS = $(CFLAGS)

###################################################
CFLAGS += -I$(SOURCE_DIR)/$(RTOS_SRC)/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lzma_decoder/inc

###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: cleanlog proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)

MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).dis" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).dis"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@$(CC) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(OUTPATH)/$(PROJ_NAME).map -lm -o $@ 2>>$(ERR_LOG)
	@$(OBJDUMP) -D $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).dis
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)


