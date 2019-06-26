
###################################################

CFLAGS += -DGT_KEY_MANAGER_SUPPORT

KEY_MGR_SRC = project/mt2523_band/apps/bioband_ref_design/demo_apps/key_management
KEY_MGR_FILES = $(KEY_MGR_SRC)/src/key_manager.c

C_FILES += $(KEY_MGR_FILES)
###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_band/apps/bioband_ref_design/demo_apps/key_management/inc
