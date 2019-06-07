ifeq ($(PROJ_PATH),)
include feature.mk
else
include $(PROJ_PATH)/feature.mk
endif

BMA255_ORIENTATION =
MT2511_E1 = n
MT2511_INTERFACE = I2C
CFLAGS += -ULCD_MIRROR_DISPLAY

# bt module enable
MTK_BT_ENABLE                       = y
MTK_BLE_ONLY_ENABLE                 = n
MTK_BT_HFP_ENABLE                   = n
MTK_BT_AVRCP_ENABLE                 = n
MTK_BT_AVRCP_ENH_ENABLE             = n
MTK_BT_A2DP_ENABLE                  = n
MTK_BT_PBAP_ENABLE                  = n
MTK_BT_SPP_ENABLE                   = y
