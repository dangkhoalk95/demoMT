PLATFORM_CHIP = driver/chip/mt7686
CFLAGS += -I$(SOURCE_DIR)/$(PLATFORM_CHIP)/inc
CFLAGS += -I$(SOURCE_DIR)/$(WIFI_RAM_PATH)/inc
CFLAGS += -DDATA_PATH_87
CFLAGS += -DFIRST_DAY_LINK_MT7682
CFLAGS += -DRA_CTRL

include $(SOURCE_DIR)/prebuilt/driver/chip/mt5932/rom/rom_flags.mk

CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/chip/mt5932/rom/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/chip/mt5932/ram/inc

LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/mt5932/ram/lib/libwifi_ram.a
