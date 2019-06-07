MT2523_CHIP = driver/chip/mt7686
CFLAGS   += $(FPUFLAGS) -DUSE_HAL_DRIVER
           
C_FILES  += $(MT2523_CHIP)/src_core/hal_efuse.c   
C_FILES  += $(MT2523_CHIP)/src_core/hal_sleep_manager_internal.c  
C_FILES  += $(MT2523_CHIP)/src_core/hal_sleep_manager.c       
C_FILES  += $(MT2523_CHIP)/src_core/hal_spm.c   

#################################################################################
#include path
CFLAGS  += -I../../inc
CFLAGS  += -Iinc
CFLAGS 	+= -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt7686/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include

