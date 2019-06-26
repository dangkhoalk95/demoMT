
MT2523_CHIP = driver/chip/mt2523
CFLAGS   += $(FPUFLAGS) -DUSE_HAL_DRIVER -DPRODUCT_VERSION=2523

C_FILES  += $(MT2523_CHIP)/src_core/hal_clock.c
C_FILES  += $(MT2523_CHIP)/src_core/hal_cm4_topsm.c
C_FILES  += $(MT2523_CHIP)/src_core/hal_cm4_topsm_internal.c
C_FILES  += $(MT2523_CHIP)/src_core/hal_sys_topsm.c
C_FILES  += $(MT2523_CHIP)/src_core/hal_dsp_topsm.c
C_FILES  += $(MT2523_CHIP)/src_core/hal_sleep_driver.c
C_FILES  += $(MT2523_CHIP)/src_core/hal_sleep_manager.c
#################################################################################
#include path
CFLAGS  += -I../../inc
CFLAGS  += -Isrc/common/include
CFLAGS  += -Isrc/sdio_gen3/include
CFLAGS  += -Iinc
CFLAGS 	+= -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/lcm/ST7789H2
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/lcm/ST7789H2
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/lcd
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/common
