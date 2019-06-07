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

PWD= $(shell pwd)
SDK_PATH    = $(abspath $(PWD)/$(SOURCE_DIR))
FEATURE ?= feature.mk
include $(FEATURE)
# Gloabl Config
include $(SOURCE_DIR)/.config
# IC Config
include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk

READELF=$(BINPATH)/arm-none-eabi-readelf

RAM_BOOTING=0

DEBUG = 0
FLOAT_TYPE = hard
BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME       = $(notdir $(abspath $(dir $(PWD))))
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build


ifeq ($(CHECK_DEP),full)

# find all cross reference
include Makefile_ubin.src

else

# check decoupling result before build mbin, ubin
ifeq ($(CHECK_DEP),prebuild)

include Makefile_ubin.src
FOTA_UBIN_DECOUPLING = 1
CFLAGS += -DFOTA_UBIN_DECOUPLING

else

# build mbin, and then ubin.
FOTA_UBIN_DECOUPLING = 1
CFLAGS += -DFOTA_UBIN_DECOUPLING

endif
endif

###################################################
# Sources or Lib
# RTOS source files
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk

# mems driver files
include $(SOURCE_DIR)/driver/board/component/mems/module.mk
include $(SOURCE_DIR)/driver/board/mt2523_band/mems_init/module.mk

# sensor subsys files
include $(SOURCE_DIR)/middleware/MTK/sensor_subsys/module.mk
include sensor_demo.mk

# clover driver files
include $(SOURCE_DIR)/driver/board/component/biosensor/mt2511/src/module.mk

# EPT Config
include $(SOURCE_DIR)/driver/board/mt2523_band/ept/module.mk

# notification service
include $(SOURCE_DIR)/middleware/MTK/bt_notify/module.mk

# bt callback manager service
include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk

# FOTA
include $(SOURCE_DIR)/middleware/MTK/fota/module.mk

# GDI files
include $(SOURCE_DIR)/middleware/MTK/gdi/module.mk

# HAL driver files

include $(SOURCE_DIR)/driver/chip/mt2523/module.mk


# NVDM files
include $(SOURCE_DIR)/middleware/MTK/nvdm/module.mk

# kernel service files
include $(SOURCE_DIR)/kernel/service/module.mk

# Bluetooth module
include $(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk

ifeq ($(MTK_PORT_SERVICE_ENABLE), y)
# add PORT SERVICE files
include $(SOURCE_DIR)/middleware/MTK/port_service/module.mk
endif

# USB MTK_USB_DEMO_ENABLED = y
ifeq ($(MTK_USB_DEMO_ENABLED), y)
# add PORT SERVICE files
include $(SOURCE_DIR)/middleware/MTK/usb/module.mk
endif

#add atci to 2511 hdk project
include $(SOURCE_DIR)/middleware/third_party/kiss_fft/module.mk
ATCI_SRC = project/mt2523_band/apps/bioband_ref_design/src/atci
ATCI_FILES = $(ATCI_SRC)/src/atci_adapter.c \
             $(ATCI_SRC)/src/atci_handler.c \
             $(ATCI_SRC)/src/atci_main.c \
             $(ATCI_SRC)/at_command/at_command.c \
             $(ATCI_SRC)/at_command/at_command_biogui.c \
             $(ATCI_SRC)/at_command/at_command_nvdm.c \
             $(ATCI_SRC)/at_command/at_command_rtc.c \
             $(ATCI_SRC)/at_command/at_command_bt.c
C_FILES += $(ATCI_FILES)

# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/kiss_fft

ifeq ($(MT2511_E1),y)
CFLAGS	+= -DUSE_EXTERNAL_BOOST
endif

CFLAGS  += -DSENSOR_BTSPP
CFLAGS	+= -DATCMD_VIA_SPPS

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(ATCI_SRC)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/biosensor/mt2511/inc
# Main APP files
APP_PATH        = $(patsubst $(SDK_PATH)/%,%,$(abspath $(dir $(PWD))))
APP_PATH_SRC = $(APP_PATH)/src
APP_FILES = $(APP_PATH_SRC)/main.c \
            $(APP_PATH_SRC)/regions_init.c \
			$(APP_PATH_SRC)/hci_log.c \
            $(APP_PATH)/GCC/syscalls.c  \
            $(APP_PATH_SRC)/system_mt2523.c \
            $(APP_PATH_SRC)/resource/custom_image_data_resource.c \
            $(APP_PATH_SRC)/resource/custom_image_map.c \
            $(APP_PATH_SRC)/resource/custom_image_resource.c \
            $(APP_PATH_SRC)/resource/FontRes.c

FOTA_DL_FILES = $(APP_PATH)/src/fota/fota_dl.c \
                $(APP_PATH)/src/fota/fota_bt_common.c
                
C_FILES += $(FOTA_DL_FILES)
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/fota
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/fota/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/bootloader/core/inc

ifeq ($(FOTA_UBIN_DECOUPLING),1)
C_FILES += $(APP_PATH)/wrapper/src/fota_common_wrapper.c
C_FILES += $(APP_PATH)/wrapper/src/pedometer_wrapper.c
C_FILES += $(APP_PATH)/wrapper/src/heart_rate_wrapper.c
C_FILES += $(APP_PATH)/wrapper/src/heart_rate_variability_wrapper.c
C_FILES += $(APP_PATH)/wrapper/src/sleep_tracker_wrapper.c
C_FILES += $(APP_PATH)/wrapper/src/ppg_control_wrapper.c
C_FILES += $(APP_PATH)/wrapper/src/blood_pressure_wrapper.c

CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/wrapper/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fusion_algo/pedometer/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fusion_algo/sleep_tracker/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fusion_algo/heart_rate/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fusion_algo/ppg_control/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fusion_algo/heart_rate_variability/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fusion_algo/blood_pressure/inc
endif


#Project config
# third_party source & library
ifeq ($(GT_PROJECT_ENABLE),y)
CFLAGS          += -DGT_PROJECT_ENABLE
include $(SOURCE_DIR)/driver/board/mt2523_band/lcd/module.mk
include $(SOURCE_DIR)/project/mt2523_band/apps/bioband_ref_design/demo_apps/key_management/module.mk
include $(SOURCE_DIR)/project/mt2523_band/apps/bioband_ref_design/demo_apps/lcd_subsys/module.mk
include $(SOURCE_DIR)/project/mt2523_band/apps/bioband_ref_design/demo_apps/app_management/module.mk
endif

include $(SOURCE_DIR)/driver/board/mt2523_band/led/module.mk

CFLAGS         	+= -DDEVICE_BAND -I$(SOURCE_DIR)/$(APP_PATH)/inc/band
CFLAGS 		+= -I$(SOURCE_DIR)/driver/board/mt2523_band/keypad/inc
APP_FILES	+= driver/board/mt2523_band/ept/src/bsp_gpio_ept_config.c
APP_FILES 	+= $(APP_PATH_SRC)/band/ept_gpio_var.c \
	           $(APP_PATH_SRC)/band/ept_eint_var.c
APP_FILES 	+= driver/board/mt2523_band/keypad/src/keypad_custom.c

# MTK_SMART_BATTERY_ENABLE define
ifeq ($(MTK_SMART_BATTERY_ENABLE), y)
CFLAGS	+= -DMTK_SMART_BATTERY_ENABLE
endif

CFLAGS += -D__INTERNAL_UI_SUPPORT__
# battery_management files
include $(SOURCE_DIR)/middleware/MTK/battery_management/module.mk

C_FILES += $(APP_FILES)
C_FILES += $(APP_PATH_SRC)/system_mt2523.c
C_FILES += $(APP_PATH_SRC)/bt_init.c
C_FILES += $(APP_PATH_SRC)/sensor_bt_spp_server.c
C_FILES += $(APP_PATH_SRC)/bt_spps_atci.c
C_FILES += $(APP_PATH_SRC)/ble_dtp.c
C_FILES += $(APP_PATH_SRC)/bioband_bt_common.c
C_FILES += $(APP_PATH_SRC)/bioband_bt_gap.c
C_FILES += $(APP_PATH_SRC)/gatt_service.c
C_FILES += $(APP_PATH_SRC)/ble_app_common.c
C_FILES += $(APP_PATH_SRC)/sync_time.c

# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt2523.s

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
ifeq ($(DEBUG), 1)
ALLFLAGS = -g -O0
else
ALLFLAGS = -g -Os
endif
ALLFLAGS += -Wall -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += $(ALLFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin

ifeq ($(FLOAT_TYPE), hard)
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
FPUFLAGS = -msoft-float
endif


ifeq ($(FORCE_DL_ENABLE), y)
CFLAGS += -DFORCE_DL_ENABLE
endif


# Definitions
CFLAGS += -D_REENT_SMALL
CFLAGS += -DARM_MATH_CM4
CFLAGS += -D__FPU_PRESENT
CFLAGS += $(FPUFLAGS)

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS += -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free
ifeq ($(RAM_BOOTING), 1)
LDFLAGS += -Wl,-Tsram.ld -Wl,--gc-sections
else
LDFLAGS += -Wl,-Tflash.ld -Wl,--gc-sections
endif
ifeq ($(DEBUG), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
endif
#LDFLAGS = $(CFLAGS)


###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt2523_band/ept/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/resource


# Board Config
include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

LDFLAGS += -u _printf_float
ifeq ($(FOTA_UBIN_DECOUPLING),1)
include mbin_keep_sym.mk
endif

CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
CXX_OBJS        = $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)
###################################################
# Module folder path

###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: cleanlog proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin
ifeq ($(CHECK_DEP),full)
	#add perl execution full
	@$(READELF) -sW $(OUTPATH)/$(PROJ_NAME).elf  > $(OUTPATH)/symbol_list.txt
	@perl ../decoupling/read_obj_list.pl full $(PROJ_NAME) $(OUTPATH)
else ifeq ($(CHECK_DEP),prebuild)
	#add perl execution prebuild
	@$(READELF) -sW $(OUTPATH)/$(PROJ_NAME).elf  > $(OUTPATH)/symbol_list.txt
	@perl ../decoupling/read_obj_list.pl prebuild $(PROJ_NAME) $(OUTPATH)
	@if [ -s "$(OUTPATH)/decoupling/m_reference_u_func.txt" ];then \
		echo "Error! Invalid reference, please check $(OUTPATH)/decoupling/m_reference_u_func.txt."; \
		exit 1; \
	elif [ -s "$(OUTPATH)/decoupling/m_reference_u_data.txt" ];then \
		echo "Error! Invalid reference, pleae to check $(OUTPATH)/decoupling/m_reference_u_data.txt."; \
		exit 1; \
	else \
		make -f mbin_ubin.mk CHECK_DEP=0 OUTPATH=$(OUTPATH) BUILD_DIR=$(BUILD_DIR); \
	fi
else
	@echo Make UBIN...
	@make -f ubin.mk OUTPATH=$(OUTPATH) BUILD_DIR=$(BUILD_DIR)/ubin
	@echo "Combine mbin + ubin => combin"
	@./cat.sh $(PROJ_NAME) $(OUTPATH)
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
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).dis" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).dis"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@echo $(patsubst $(SDK_PATH)/%,$(SOURCE_DIR)/%, $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(patsubst $(SDK_PATH)/%,$(SOURCE_DIR)/%,$(OUTPATH)/$(PROJ_NAME).map) -lm -Wl,--cref -o $@) > $(OUTPATH)/link_option.tmp
	@$(CC) @$(OUTPATH)/link_option.tmp 2>>$(ERR_LOG)
	@rm -f $(OUTPATH)/link_option.tmp
	@$(OBJDUMP) -D $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).dis
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
ifeq ($(filter full,$(CHECK_DEP)),)
	@$(READELF) -sW $(OUTPATH)/$(PROJ_NAME).elf | grep "FUNC\|OBJECT" | grep -f mbin_keep_sym.txt | awk '{printf $$8" = 0x"$$2";\n"}' > $(OUTPATH)/mbin.lis
endif
	@echo Done


include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)
	make -f ubin.mk clean

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
endif
