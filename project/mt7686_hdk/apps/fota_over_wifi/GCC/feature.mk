IC_CONFIG                           = mt7686
BOARD_CONFIG                        = mt7686_hdk
# debug level: none, error, warning, and info
MTK_DEBUG_LEVEL                       = info
# 3 options with psram/flash or not, only 1 option is y, the others should be n 
MTK_MEMORY_WITH_PSRAM_FLASH         = y
MTK_MEMORY_WITHOUT_PSRAM            = n
MTK_MEMORY_WITHOUT_PSRAM_FLASH      = n
# System service debug feature for internal use
MTK_SUPPORT_HEAP_DEBUG              = n
MTK_HEAP_SIZE_GUARD_ENABLE          = n
MTK_OS_CPU_UTILIZATION_ENABLE       = y

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

#MTK system hang issue debug feauture option
MTK_SYSTEM_HANG_CHECK_ENABLE        = n

#LWIP features
MTK_PING_OUT_ENABLE                 = y
#CLI features
MTK_MINICLI_ENABLE                  = y
MTK_CLI_TEST_MODE_ENABLE            = y
MTK_HAL_LOWPOWER_ENABLE             = y
MTK_HIF_GDMA_ENABLE                 = y

#AT command
MTK_WIFI_AT_COMMAND_ENABLE          = y
MTK_AT_CMD_DISABLE                  = n
MTK_ATCI_ENABLE                     = y

MTK_FOTA_ENABLE                     = y
MTK_FOTA_AT_COMMAND_ENABLE          = y
MTK_FOTA_CLI_ENABLE                 = y
