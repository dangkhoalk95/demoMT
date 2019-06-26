ifeq ($(PROJ_PATH),)
include feature.mk
else
include $(PROJ_PATH)/feature.mk
endif

MTK_MBEDTLS_CONFIG_FILE             = config-mtk-multiple-ciphersuits-hw-nc-mem.h
MTK_MBEDTLS_HW_CRYPTO               = y
