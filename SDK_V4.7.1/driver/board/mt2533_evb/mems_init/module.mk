
MEMS_INIT_SRC = driver/board/mt2533_evb/mems_init

C_FILES  += $(MEMS_INIT_SRC)/mems_init.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt2533_evb/mems_init


