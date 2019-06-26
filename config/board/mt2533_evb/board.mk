
CFLAGS += -DLXR_NVDM_SUP

ifeq ($(MTK_EXTERNAL_PMIC),y)
CFLAGS += -DMTK_EXTERNAL_PMIC
endif

CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/lcd
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/common
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/lcm/ST7789H2
