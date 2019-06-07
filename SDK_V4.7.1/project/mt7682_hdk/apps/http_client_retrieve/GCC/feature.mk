IC_CONFIG                           = mt7682
BOARD_CONFIG                        = mt7682_hdk
MTK_HAL_LOWPOWER_ENABLE             = y
MTK_IPERF_ENABLE                    = y
MTK_PING_OUT_ENABLE                 = y
MTK_WIFI_TGN_VERIFY_ENABLE          = n
MTK_WIFI_WPS_ENABLE                 = n
MTK_WIFI_DIRECT_ENABLE              = n
MTK_WIFI_REPEATER_ENABLE            = n
MTK_NVDM_ENABLE                     = y
MTK_WIFI_PROFILE_ENABLE             = y


MTK_CM4_WIFI_TASK_ENABLE            = y
MTK_WIFI_ROM_ENABLE                 = y
MTK_ROM_VER                         = e1

MTK_NO_PSRAM_ENABLE                 = y
# 3 options with psram/flash or not, only 1 option is y, the others should be n 
MTK_MEMORY_WITH_PSRAM_FLASH         = n
MTK_MEMORY_WITHOUT_PSRAM            = y
MTK_MEMORY_WITHOUT_PSRAM_FLASH      = n


# debug level: none, error, warning, and info
MTK_DEBUG_LEVEL                     = info



MTK_MBEDTLS_CONFIG_FILE             = config-mtk-basic.h
MTK_HTTPCLIENT_SSL_ENABLE           = y
