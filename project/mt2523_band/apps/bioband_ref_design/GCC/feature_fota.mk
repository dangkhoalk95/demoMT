ifeq ($(PROJ_PATH),)
include feature.mk
else
include $(PROJ_PATH)/feature.mk
endif

# FOTA option
MTK_FOTA_ENABLE = y
MTK_FOTA_UBIN_ENABLE = y
MTK_MU_ENABLE = y