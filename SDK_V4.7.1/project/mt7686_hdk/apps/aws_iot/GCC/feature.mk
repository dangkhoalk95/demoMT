IC_CONFIG                           = mt7686
BOARD_CONFIG                        = mt7686_hdk

# debug level: none, error, warning, and info
MTK_DEBUG_LEVEL                       = info
# 3 options with psram/flash or not, only 1 option is y, the others should be n 
MTK_MEMORY_WITH_PSRAM_FLASH         = y
MTK_MEMORY_WITHOUT_PSRAM            = n
MTK_MEMORY_WITHOUT_PSRAM_FLASH      = n

#NVDM
MTK_NVDM_ENABLE                     = y

#WIFI features
MTK_WIFI_TGN_VERIFY_ENABLE          = n
MTK_WIFI_WPS_ENABLE                 = n
MTK_WIFI_DIRECT_ENABLE              = n
MTK_WIFI_REPEATER_ENABLE            = n
MTK_WIFI_PROFILE_ENABLE             = y

MTK_CM4_WIFI_TASK_ENABLE            = y
MTK_WIFI_ROM_ENABLE                 = y

#LWIP features
MTK_IPERF_ENABLE                    = y
MTK_PING_OUT_ENABLE                 = n
MTK_USER_FAST_TX_ENABLE             = n

#CLI features
MTK_MINICLI_ENABLE                  = y
MTK_CLI_TEST_MODE_ENABLE            = y

#HAL
MTK_HAL_LOWPOWER_ENABLE             = n
MTK_HIF_GDMA_ENABLE                 = n

#CLI, ATCMD Feature
MTK_AT_CMD_DISABLE                  = n
MTK_ATCI_ENABLE                     = n
MTK_WIFI_AT_COMMAND_ENABLE          = n

#Demo Feature
LP_DEMO                             = n

########################################################
# Add for AWS IOT Feature

AWS_IOT_SUPPORT                     = y
MTK_MBEDTLS_CONFIG_FILE             = config-aws-iot.h

# AWS IoT Log level define
AWS_IOT_DEBUG_ENABLE                = y
AWS_IOT_ERROR_ENABLE                = y
AWS_IOT_TRACE_ENABLE                = n
AWS_IOT_INFO_ENABLE                 = y
AWS_IOT_WARN_ENABLE                 = y

