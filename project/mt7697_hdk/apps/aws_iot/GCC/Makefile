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

##############################################################################
#
# Project settings
#
##############################################################################


SOURCE_DIR  = ../../../../..
PWD        = $(shell pwd)
SDK_PATH    = $(abspath $(PWD)/$(SOURCE_DIR))
DATIME      = $(shell date --iso=seconds)
V          ?= 0

##############################################################################
#
# build log output
#
##############################################################################


ifeq ($(V),0)
Q           = @
else
Q           =
endif


##############################################################################
#
# config files
#
##############################################################################


FEATURE    ?= feature.mk
include $(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk
# HAL driver files
include $(SOURCE_DIR)/driver/chip/mt7687/module.mk
# EPT Config
-include $(SOURCE_DIR)/driver/board/mt76x7_hdk/ept/module.mk
# Minisupp Config
ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/minisupp/),)
include $(SOURCE_DIR)/middleware/MTK/minisupp/module.mk
else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/minisupp/module.mk
endif


###################################################
# Sources or Lib
#include lwip
include $(SOURCE_DIR)/middleware/third_party/lwip/module.mk

# add AWS IoT files
ifeq ($(AWS_IOT_SUPPORT),y)
include $(SOURCE_DIR)/middleware/third_party/aws_iot/module.mk

#include mbedtls
include $(SOURCE_DIR)/middleware/third_party/mbedtls/module.mk

CFLAGS += -DAWS_IOT_TEST_ENABLE

ifeq ($(AWS_IOT_DEBUG_ENABLE), y)
CFLAGS += -DENABLE_IOT_DEBUG
endif

ifeq ($(AWS_IOT_ERROR_ENABLE), y)
CFLAGS += -DENABLE_IOT_ERROR
endif

ifeq ($(AWS_IOT_TRACE_ENABLE), y)
CFLAGS += -DENABLE_IOT_TRACE
endif

ifeq ($(AWS_IOT_INFO_ENABLE), y)
CFLAGS += -DENABLE_IOT_INFO
endif

ifeq ($(AWS_IOT_WARN_ENABLE), y)
CFLAGS += -DENABLE_IOT_WARN
endif

endif   # end AWS_IOT_SUPPORT

#include dhcpd
include $(SOURCE_DIR)/middleware/MTK/dhcpd/module.mk

# wifi src
include $(SOURCE_DIR)/middleware/MTK/wifi_service/combo/module.mk
#connsys_module
include $(SOURCE_DIR)/middleware/MTK/connsys/module.mk

ifeq ($(findstring y,$(MTK_SMTCN_V4_ENABLE)$(MTK_SMTCN_V5_ENABLE)),y)
-include $(SOURCE_DIR)/middleware/MTK/smtcn/module.mk
endif

ifeq ($(MTK_CLI_EXAMPLE_MODE_ENABLE),y)
CFLAGS         += -DMTK_CLI_EXAMPLE_MODE_ENABLE
endif


ifeq ($(MTK_FOTA_DUAL_IMAGE_ENABLE),y)
export MTK_FOTA_DUAL_IMAGE_ENABLE
FOTA_PARTITION_B_BUILD    ?= n
ifeq ($(FOTA_PARTITION_B_BUILD),y)

CFLAGS += -DFOTA_PARTITION_B_BUILD
endif
PARTITION_B_EXTRA ?= FOTA_PARTITION_B_BUILD=y BUILD_DIR=$(BUILD_DIR)/binary_B OUTPATH=$(OUTPATH)/binary_B PROJ_PATH=$(PROJ_PATH)
endif

RAM_BOOTING=0

DEBUG      = 0
FLOAT_TYPE = hard
BUILD_DIR  = $(PWD)/Build

# Project name
PROJ_NAME       = $(notdir $(abspath $(dir $(PWD))))
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

# Main APP files
APP_PATH        = $(patsubst $(SDK_PATH)/%,%,$(abspath $(dir $(PWD))))
APP_PATH_SRC = $(APP_PATH)/src
APP_FILES =    $(APP_PATH_SRC)/sys_init.c \
               $(APP_PATH_SRC)/regions_init.c \
               $(APP_PATH_SRC)/wifi_lwip_helper.c \
               $(APP_PATH_SRC)/wifi_nvdm_config.c  \
               $(APP_PATH)/GCC/syscalls.c \

C_FILES += $(APP_FILES)
C_FILES += $(APP_PATH_SRC)/system_mt7687.c

# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt7687.s
C_FILES += $(APP_PATH_SRC)/main.c
ifeq ($(AWS_IOT_SUPPORT),y)
C_FILES        += $(APP_PATH_SRC)/aws_iot_test.c
endif
C_FILES += $(APP_PATH_SRC)/ept_gpio_var.c
C_FILES += $(APP_PATH_SRC)/ept_eint_var.c


C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
CXX_OBJS        = $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)



###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/customer
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/ept/inc
ifeq ($(AWS_IOT_SUPPORT),y)
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/aws_iot/samples/freertos
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/aws_iot/tests/freertos_integration/include
endif
USE_TEST_CERT := $(strip $(wildcard $(SOURCE_DIR)/project/common/certs_protected/aws))
ifneq ($(USE_TEST_CERT),)
CFLAGS += -I$(SOURCE_DIR)/project/common/certs_protected/aws
else
CFLAGS += -I$(SOURCE_DIR)/project/common/certs/aws
endif

###################################################
# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS += -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free
ifeq ($(RAM_BOOTING), 1)
LDFLAGS += -Wl,-T$(patsubst $(SDK_PATH)/%,%,$(PWD))/mt7687_sram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-T$(patsubst $(SDK_PATH)/%,%,$(PWD))/mt7687_flash.ld -Wl,--gc-sections
endif

ifeq ($(AWS_IOT_SUPPORT),y)
LDFLAGS += -u _printf_float -u _scanf_float
endif

###################################################
# Libraries

include $(SOURCE_DIR)/project/common/bsp_ex/module.mk
# NVDM files
include $(SOURCE_DIR)/middleware/MTK/nvdm/module.mk
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk
include $(SOURCE_DIR)/kernel/service/module.mk
include $(SOURCE_DIR)/driver/board/mt76x7_hdk/util/module.mk

###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: cleanlog proj
	$(Q)echo $(OUTPATH)
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	$(Q)echo "Generate Assembly from elf:"
	#$(Q)$(OBJDUMP) -S $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).elf.s
ifneq ($(MTK_FOTA_DUAL_IMAGE_ENABLE),y)
	$(Q)$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)
else
	$(Q)$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)
ifneq ($(FOTA_PARTITION_B_BUILD),y)
else
	export FOTA_PARTITION_B_BUILD=y
	$(Q)$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)
endif
endif
	$(Q)cp *.cmm $(OUTPATH)/
ifeq ($(MTK_FOTA_DUAL_IMAGE_ENABLE),y)
ifneq ($(FOTA_PARTITION_B_BUILD),y)
	$(Q)echo "build partition B binary."
	$(Q)mkdir -p $(BUILD_DIR)/binary_B
	$(Q)mkdir -p $(OUTPATH)/binary_B
	$(Q)make -C $(PWD) $(PARTITION_B_EXTRA) $($@_EXTRA)
	$(Q)./hash_gen.sh $(OUTPATH) $(PROJ_NAME).bin
endif
endif


MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@echo $(patsubst $(SOURCE_DIR)/%,%,$(patsubst $(SDK_PATH)/%,%, $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(patsubst $(SDK_PATH)/%,%,$(OUTPATH)/$(PROJ_NAME).map) -lm -o $@)) > $(OUTPATH)/link_option.tmp
	@cd $(SOURCE_DIR) && \
	$(patsubst $(SOURCE_DIR)/%,%,$(CC)) @$(OUTPATH)/link_option.tmp 2>>$(ERR_LOG)
	@rm -f $(OUTPATH)/link_option.tmp
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)


print-%  : ; @echo $* = $($*)
