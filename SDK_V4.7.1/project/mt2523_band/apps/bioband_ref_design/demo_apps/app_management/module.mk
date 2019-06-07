
PROJ_SRC = project/mt2523_band/apps/bioband_ref_design/demo_apps/app_management/src
PROJ_FILES = $(PROJ_SRC)/app_main.c \
				$(PROJ_SRC)/app_manager.c \
				$(PROJ_SRC)/app_status_bar.c \
				$(PROJ_SRC)/apps/app_watch.c \
				$(PROJ_SRC)/apps/app_pedometer.c \
				$(PROJ_SRC)/apps/app_bloodp.c \
				$(PROJ_SRC)/apps/app_hrv.c \
				$(PROJ_SRC)/apps/app_sleep.c \
				$(PROJ_SRC)/apps/app_heart.c \
				$(PROJ_SRC)/apps/app_btinfo.c \
                                $(PROJ_SRC)/app_util.c 
				

C_FILES += $(PROJ_FILES)

# Include path
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_band/apps/bioband_ref_design/demo_apps/app_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/inc
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include

