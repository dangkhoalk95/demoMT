PRODUCT_VERSION=7682

# Default common setting
MTK_BUILD_SMT_LOAD ?= n
MTK_LWIP_ENABLE                             ?= y
MTK_WIFI_CONFIGURE_FREE_ENABLE              ?= n
MTK_WIFI_PROFILE_ENABLE                     ?= n
MTK_CLI_TEST_MODE_ENABLE                    ?= n
ifeq ($(MTK_WIFI_TGN_VERIFY_ENABLE),y)
MTK_HIF_GDMA_ENABLE                         ?= y
else
MTK_HIF_GDMA_ENABLE                         ?= n
endif
MTK_MAIN_CONSOLE_UART2_ENABLE               ?= n
MTK_BSP_LOOPBACK_ENABLE                     ?= n
MTK_OS_CPU_UTILIZATION_ENABLE               ?= n
MTK_SUPPORT_HEAP_DEBUG                      ?= n
MTK_HEAP_SIZE_GUARD_ENABLE                  ?= n
MTK_MPERF_ENABLE                            ?= n
MTK_FW_DW_BY_CM4                            ?= n
MTK_FLASH_DIRECT_DL                         ?= n
MTK_HIF_HEADER_2DW                          ?= y
MTK_CM4_N9_SINGLE_IMG                       ?= y
MTK_PATCH_DL_ENABLE                         ?= n
MTK_CM4_WIFI_TASK_ENABLE                    ?= y
MTK_WIFI_ROM_ENABLE                         ?= y
MTK_SECURE_BOOT_ENABLE                      ?= n
MTK_BOOTLOADER_USE_MBEDTLS                  ?= n
MTK_NVDM_ENABLE                             ?= n
MT7682_E2_ENABLE                            ?= y
MTK_USER_FAST_TX_ENABLE                     ?= n

ifeq ($(LP_DEMO),y)
LP_DEMO                                     ?= y
else
LP_DEMO                                     ?= n
endif

AR      = $(BINPATH)/arm-none-eabi-ar
CC      = $(BINPATH)/arm-none-eabi-gcc
CXX     = $(BINPATH)/arm-none-eabi-g++
OBJCOPY = $(BINPATH)/arm-none-eabi-objcopy
SIZE    = $(BINPATH)/arm-none-eabi-size
OBJDUMP = $(BINPATH)/arm-none-eabi-objdump


ALLFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m4
FPUFLAGS = -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard

COM_CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -ffunction-sections -fdata-sections -fno-builtin -Wimplicit-function-declaration
COM_CFLAGS += -gdwarf-2 -Os -Wall -fno-strict-aliasing -fno-common
COM_CFLAGS += -Wall -Wimplicit-function-declaration -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type
COM_CFLAGS += -DPCFG_OS=2 -D_REENT_SMALL -Wno-error -Wno-switch
COM_CFLAGS += -DPRODUCT_VERSION=$(PRODUCT_VERSION)

## MT7682_E2_ENABLE
## Brief:  E2 version
## Usage: Default is y after e2 MP
## Path: middleware/MTK/wifi_stack/module.mk
## Dependency:  N/A
##
ifeq ($(MT7682_E2_ENABLE),y)
MTK_ROM_VER = e2
COM_CFLAGS         += -DMTK_ROM_VER=e2
COM_CFLAGS         += -DMT7682_E2_ENABLE
COM_CFLAGS         += -DE2_ROM_7682
else
MTK_ROM_VER = e1
COM_CFLAGS         += -DMTK_ROM_VER=e1
endif


##
## MTK_WIFI_ROM_ENABLE
## Brief:  Use the WiFi ROM in CM4 to offload some code in SYSRAM/XIP
## Usage:  Need to enable in MT7682/32 only
## Path:   middleware/MTK/wifi_service/combo/src/wifi_profile.c
##         middleware/MTK/wifi_service/combo/src/wifi_firmware_task/wifi_firmware_task.c
##         middleware/MTK/wifi_service/combo/src/wifi_firmware_task/wifi_os_freertos.c
##         middleware/MTK/wifi_service/combo/src_protected/inband_queue.c
##         middleware/MTK/wifi_service/combo/src_protected/wifi_api.c
## Dependency:  N/A
##
ifeq ($(MTK_WIFI_ROM_ENABLE),y)
COM_CFLAGS         += -DMTK_WIFI_ROM_ENABLE
COM_CFLAGS         += -DMT5932_SINGLE_CONTEXT
endif


##
## MTK_CM4_WIFI_TASK_ENABLE
## Brief:  Execute WiFi Task in CM4, instead of N9
## Usage:  Need to enable in MT7686/82/32
## Path:   middleware/MTK/connsys/src/connsys_util.c
##         middleware/MTK/connsys/src/connsys_driver.c
## Dependency:  N/A
##
ifeq ($(MTK_CM4_WIFI_TASK_ENABLE),y)
COM_CFLAGS         += -DMTK_CM4_WIFI_TASK_ENABLE
endif


##
## MTK_WIFI_REPEATER_ENABLE
## Brief:       This option is to enable/disable MTK Wi-Fi Repeator mode. 
## Path:        middleware/MTK/wifi_stack
## Dependency:  libwifi.a libwifi_mt7682_ram.a
## Related doc: Please refer to wifi dev guide under the doc folder for more detail.
##
ifeq ($(MTK_WIFI_REPEATER_ENABLE),y)
COM_CFLAGS         += -DMTK_WIFI_REPEATER_ENABLE
CONFIG_REPEATER     = y
endif

##
## MTK_SINGLE_SKU_SUPPORT
## Brief:       This option is to enable/disable Wi-Fi Single SKU feature.
## Usage:       If the value is "y", the MTK_SINGLE_SKU_SUPPORT will be defined, when enable must enable MTK_NVDM_ENABLE to include NVDM module.
## Path:        None
## Dependency:  NVDM.
## Related doc: None.
##
ifeq ($(MTK_SINGLE_SKU_SUPPORT),y)
  CFLAGS += -DMTK_SINGLE_SKU_SUPPORT
endif

##
## MTK_WIFI_DHCPD_DHCP_COEXIST_ENABLE
## Brief:       This option is to let dhcpd and dhcp coexist in repeater mode, thus the Device 's ap port can allocate the IP address to
##				the station which connect to the device instead of allocating the IP address by the target AP 
## Path:        middleware/MTK/wifi_stack
## Dependency:  libwifi.a , MTK_WIFI_REPEATER_ENABLE
##
ifeq ($(MTK_WIFI_DHCPD_DHCP_COEXIST_ENABLE),y)
COM_CFLAGS         += -DMTK_WIFI_DHCPD_DHCP_COEXIST_ENABLE
endif


##
## MTK_PATCH_DL_ENABLE
## Brief:  Need to download HW Patch before N9 FW
## Usage:Only need to enable in MT7687/97, MT7686/82/32 needn't  (Not BT Combo)
## Path:  middleware/MTK/connsys/src/connsys_util.c
## Dependency:  N/A
##
ifeq ($(MTK_PATCH_DL_ENABLE),y)
COM_CFLAGS         += -DMTK_PATCH_DL_ENABLE
endif

##
## MTK_CM4_N9_SINGLE_IMG
## Brief:  CM4 and N9 are merged in the same image
## Usage:  only support in MT7686/82/32, MT7687/97 not support (flash layout need to modify)
## Path:  middleware/MTK/connsys/src/connsys_util.c
##          middleware/MTK/connsys/inc/connsys_util.h
## Dependency:  N/A
##
ifeq ($(MTK_CM4_N9_SINGLE_IMG),y)
COM_CFLAGS         += -DMTK_CM4_N9_SINGLE_IMG
endif

##
## MTK_FW_DW_BY_CM4
## Brief:       Download FW by CM4 MCU via connsys, instead DMA from flash directly
## Usage:       DMA + scramble Flash has H/W issue, if need scramble, turn on this feature
## Path:        middleware/MTK/connsys/src/connsys_util.c
##              driver/chip/mt7687/inc/connsys_util.h
## Dependency:  N/A
##
ifeq ($(MTK_FW_DW_BY_CM4),y)
COM_CFLAGS         += -DMTK_FW_DW_BY_CM4
endif

##
## MTK_FLASH_DIRECT_DL
## Brief:       Download FW of which format is for flash-direct-download
## Usage:     MT7686 not support flash-direct-download anymore, default need to turn off this feature
## Path:       middleware/MTK/connsys/src/connsys_util.c
##               middleware/MTK/connsys/inc/connsys_util.h
## Dependency:  N/A
##
ifeq ($(MTK_FLASH_DIRECT_DL),y)
COM_CFLAGS         += -DMTK_FLASH_DIRECT_DL
endif

##
## MTK_HIF_HEADER_2DW
## Brief:       Use 2DW format of HIF header , instead of 3DW
## Usage:     MT7686 PDA not support 3DW HIF header, default turn on this flag
## Path:       middleware/MTK/connsys/src/connsys_util.c
##               driver/chip/mt7687/inc/connsys_util.h
## Dependency:  N/A
##
ifeq ($(MTK_HIF_HEADER_2DW),y)
COM_CFLAGS         += -DMTK_HIF_HEADER_2DW
endif


##
## MTK_AIRKISS_ENABLE
## Brief:       This option is to enable wechat cloud platform - airkiss.
## Usage:       If the value is "y", the MTK_AIRKISS_ENABLE compile option will be defined. You must also include the middleware/third_party/cloud/Tencent_weixin/module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/cloud/Tencent_weixin
## Dependency:  LWIP module.
##
ifeq ($(MTK_AIRKISS_ENABLE),y)
COM_CFLAGS         += -DMTK_AIRKISS_ENABLE
endif

##
## MTK_ALINK_ENABLE
## Brief:       This option is to enable alibaba cloud platform - alink.
## Usage:       If the value is "y", the MTK_ALINK_ENABLE compile option will be defined. You must also include the middleware/third_party/cloud/ali_alink/module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/cloud/ali_alink
## Dependency:  LWIP and mbedTLS module.
##
ifeq ($(MTK_ALINK_ENABLE),y)
COM_CFLAGS         += -DMTK_ALINK_ENABLE
COM_CFLAGS         += -DMTK_AWS_ENABLE
endif

##
## MTK_BLE_SMTCN_ENABLE
## Brief:       This option is to enable BLE WIFI smart connection under project/mt7697_hdk/apps/iot_sdk folder.
## Usage:       If the value is "y", the MTK_BLE_SMTCN_ENABLE compile option will be defined, the sources and header files under project/mt7697_hdk/apps/iot_sdk and middleware/MTK/ble_smtcn will be included by iot_sdk project.
## Path:        project/mt7697_hdk/apps/iot_sdk, middleware/MTK/ble_smtcn
## Dependency:  None.
## Notice:      The GATT service of this feature is registered in project/mt7697_hdk/apps/iot_sdk/src/ut_app/gatt_service.c.
##              For more information, please refer to middleware/MTK/ble_smtcn.
## Related doc: None
##
ifeq ($(MTK_BLE_SMTCN_ENABLE),y)
COM_CFLAGS         += -DMTK_BLE_SMTCN_ENABLE
endif


##
## MTK_BSPEXT_ENABLE
## Brief:       This option is to enable and disable the Wi-Fi relevant CLI (command line interface).
## Usage:       If the value is "y", the MTK_BSPEXT_ENABLE  compile option will be defined. You must also include the project/common/bsp_ex/module.mk
##              in your Makefile before setting the option to "y".
## Path:        middleware/MTK/wifi_service/combo     project/common/bsp_ex
## Dependency:  MTK_MINICLI_ENABLE must also defined in feature.mk in your project.
##              HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
##              libwifi
##
ifeq ($(MTK_BSPEXT_ENABLE),y)
COM_CFLAGS         += -DMTK_BSPEXT_ENABLE
endif


##
## MTK_CLI_FORK_ENABLE
## Brief:       This option enables the CLI command 'fork' to allow users to run their commands in a separate task.
## Usage:       If the value is "y", the MTK_CLI_FORK_ENABLE compile option will be defined. You must also include the middleware/MTK/minicli/module.mk in your Makefile before setting the option to "y".
## Path:        middleware/MTK/minicli
## Dependency:  MTK_MINICLI_ENABLE must also defined in feature.mk under project inc folder.
## Notice:      N/A
## Related doc: N/A
##
ifeq ($(MTK_CLI_FORK_ENABLE), y)
COM_CFLAGS         += -DMTK_CLI_FORK_ENABLE
endif


##
## MTK_CLI_TEST_MODE_ENABLE
##
## Brief:       This option enables testing and under development features.
##              related CLI commands.
##
## Usage:       If the value is "y", the macro MTK_CLI_TEST_MODE_ENABLE will
##              be defined. In general, this provides CLI commands "en" and
##              "back" in normal mode and test mode, respectively. Developers
##              can provides different sets of CLI commands to users. And if
##              product is going to mass production and the "test commands"
##              are not supposed to be released, this test mode macro can be
##              turned off to remove them.
##
## Path:        middleware/MTK/minicli/inc/minicli_cmd_table.h
##
## Dependency:  MTK_MINICLI_ENABLE must also defined in feature.mk under
##              project inc folder.
##
## Related doc: See MINICLI_TEST_MODE_CLI_CMDS in
##              middleware/MTK/minicli/inc/minicli_cmd_table.h for the list of
##              test commands.
##
ifeq ($(MTK_CLI_TEST_MODE_ENABLE),y)
COM_CFLAGS         += -DMTK_CLI_TEST_MODE_ENABLE
endif


##
## MTK_DEBUG_LEVEL
## Brief:       This option is to configure system log debug level.
## Usage:       The valid values are empty, error, warning, info, debug, and none.
##              The setting will determine whether a debug log will be compiled.
##              However, the setting has no effect on the prebuilt library.
##              empty   : All debug logs are compiled.
##              error   : Only error logs are compiled.
##              warning : Only warning and error logs are compiled.
##              info    : Only info, warning, and error logs are compiled.
##              debug   : All debug logs are compiled.
##              none    : All debugs are disabled.
## Path:        kernel/service
## Dependency:  None
## Notice:      None
## Realted doc: Please refer to doc/Airoha_IoT_SDK_for_RTOS_System_Log_Developers_Guide.pdf
##
ifeq ($(MTK_DEBUG_LEVEL),)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_DEBUG
COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),error)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),warning)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),info)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),debug)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_DEBUG
COM_CFLAGS += -DMTK_DEBUG_LEVEL_INFO
COM_CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
COM_CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),none)
COM_CFLAGS += -DMTK_DEBUG_LEVEL_NONE
endif


##
## MTK_HAL_LOWPOWER_ENABLE
## Brief:       This option is to enable CM4 deep low power support.
## Usage:       If the value is "y", the MTK_HAL_LOWPOWER_ENABLE compile option will be defined, the related code under middleware\MTK\connsys\src, middleware\third_party\lwip\ports will be included.
## Path:        middleware\MTK\connsys\src, middleware\third_party\lwip\ports
## Dependency:  None
## Notice:      The default setting is ON. Enable this option will handle CM4 low power related interrupt and wakeup flow to protect CM4 when N9 in sleep state.
##              When this option is OFF, CM4 disallow enter deep low power mode.
## Realted doc: None
##
ifeq ($(MTK_HAL_LOWPOWER_ENABLE),y)
COM_CFLAGS         += -DMTK_HAL_LOWPOWER_ENABLE
endif



##
## MTK_HIF_GDMA_ENABLE
## Brief:       This option is to enable/disable MTK HIF GDMA feature.
## Usage:       If the value is "y", the MTK_HIF_GDMA_ENABLE will be defined. Data transportation between CM4 and N9 will use DMA mode.
##              It's for internal use and development. Default should be disabled, should enable when MTK_WIFI_TGN_VERIFY_ENABLE defined.
## Path:        N/A
## Dependency:  N/A.
## Related doc: N/A.
ifeq ($(MTK_HIF_GDMA_ENABLE), y)
COM_CFLAGS     += -DMTK_HIF_GDMA_ENABLE
endif

ifeq ($(LP_DEMO), y)
COM_CFLAGS     += -DLP_DEMO
endif

##
## MTK_HOMEKIT_ENABLE
## Brief:       This option is to enable homekit under middleware/MTK/homekit folder.
## Usage:       If the value is "y", the MTK_HOMEKIT_ENABLE compile option will be defined, the sources and header files under middleware/MTK/homekit/inc be included by middleware/MTK/homekit/MakeFile.
## Path:        /middleware/MTK/homekit
## Dependency:  None, more dependency for lower module be defined in middleware/MTK/homekit/readme.txt.
## Notice:      middleware/MTK/homekit is only for SLA customer with MFi license
## Realted doc:  pls refer to middleware/MTK/homekit/doc/Getting_Started_with_IoT_Homekit_v1.2_on_MT7687 about how to start homekit
##
ifeq ($(MTK_HOMEKIT_ENABLE),y)
COM_CFLAGS     += -DMTK_HOMEKIT_ENABLE
export HOMEKIT_DIR = middleware/MTK/homekit
endif


##
## MTK_HOMEKIT_HAP_MOCK
## Brief:       This option is to enable homekit profile using a mocked implementation under middleware/MTK/homekit folder.
## Usage:       If the value is "y", the MTK_HOMEKIT_HAP_MOCK compile option will be defined, the sources and header files under
##              middleware/MTK/homekit/src_protected/hkap be included by middleware/MTK/homekit/src_protected/hkap/module.mk.
## Path:        /middleware/MTK/homekit/src_protected/hkap/
## Dependency:  MTK_HOMEKIT_ENABLE.
## Notice:      middleware/MTK/homekit is only for SLA customer with MFi license
## Realted doc:  none
##
ifeq ($(MTK_HOMEKIT_HAP_MOCK),y)
COM_CFLAGS     += -DMTK_HOMEKIT_HAP_MOCK
endif


##
## MTK_HTTPCLIENT_SSL_ENABLE
## Brief:       This option is to switch SSL/TLS support in SSL client module.
## Usage:       To enable HTTP client support in a project, use "LIBS += $(OUTPATH)/libhttpclient.a" to include the middleware.
##              Additionally, switch this option to configure SSL support in HTTP client.
## Path:        middleware/third_party/httpclient
## Dependency:  LWIP and mbedTLS module must be enabled.
## Related doc: Please refer to internet and open source software guide under the doc folder for more detail.
##
ifeq ($(MTK_HTTPCLIENT_SSL_ENABLE),y)
CFLAGS += -DMTK_HTTPCLIENT_SSL_ENABLE
endif


##
## MTK_IPERF_ENABLE
## Brief:       It supports supports iperf client and server service run on CM4. other peer of the connection. It supports almost all the major iPerf v2.0 features and most used for RD development and debug.
## Usage:       If the value is "y", the MTK_IPERF_ENABLE compile option will be defined. You must also include the middleware\third_party\ping\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/MTK/iperf
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
##
ifeq ($(MTK_IPERF_ENABLE),y)
COM_CFLAGS         += -DMTK_IPERF_ENABLE
endif


##
## MTK_JOYLINK_ENABLE
## Brief:       This option is to enable jingdong cloud platform - joylink.
## Usage:       If the value is "y", the MTK_JOYLINK_ENABLE compile option will be defined. You must also include the middleware/third_party/cloud/jd_joylink/module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/cloud/jd_joylink
## Dependency:  LWIP module.
##
ifeq ($(MTK_JOYLINK_ENABLE),y)
COM_CFLAGS += -DMTK_JOYLINK_ENABLE
endif


##
## MTK_LED_ENABLE
## Brief:       This option is to enable BSP LED APIs under driver/board/component/led folder.
## Usage:       If the value is "y", the MTK_LED_ENABLE compile option will be defined, the sources and header files under driver/board/component/led and driver/board/mt76x7_hdk/led will be included by driver/board/mt76x7_hdk/led/module.mk. You must also include the driver/board/mt76x7_hdk/led/module.mk in your Makefile and bsp_led.h in the file which uses the led function before setting the option to "y".
## Path:        driver/board/component/led, driver/board/mt76x7_hdk/led.
## Dependency:  Must enable HAL_GPIO_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of LED under driver/board/mt76x7_hdk/led is none.
##              Please configure the GPIO number for LED and maximan number of LED under driver/board/mt76x7_hdk/led. For more information, please refer to the files under the corresponding path.
## Relative doc:None
##


## MTK_LWIP_DYNAMIC_DEBUG_ENABLE
## Brief:       This option provides debug information when its in running state.
## Usage:       If the value is "y", the MTK_LWIP_DYNAMIC_DEBUG_ENABLE compile option will be defined. You must also include the middleware\third_party\lwip\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/lwip.
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
## Notice:      N/A.
## Relative doc:N/A.
##
ifeq ($(MTK_MINICLI_ENABLE),y)
ifeq ($(MTK_LWIP_DYNAMIC_DEBUG_ENABLE),y)
COM_CFLAGS         += -DMTK_LWIP_DYNAMIC_DEBUG_ENABLE
endif
endif


##
## MTK_LWIP_ENABLE
## Brief:       This option provide the LWIPs CLI for user.
## Usage:       If the value is "y", the MTK_LWIP_ENABLE compile option will be defined. You must also include the middleware\third_party\lwip\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/lwip.
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
## Notice:      N/A.
## Relative doc:N/A.
##
ifeq ($(MTK_LWIP_ENABLE),y)
COM_CFLAGS         += -DMTK_LWIP_ENABLE
endif


##
## MTK_LWIP_STATISTICS_ENABLE
## Brief:       MTK_LWIP_STATISTICS_ENABLE provide the LWIP statistics collection. Such as memory usage. Packets sent and received.
## Usage:       If the value is "y", the MTK_LWIP_STATISTICS_ENABLE compile option will be defined. You must also include the middleware\third_party\lwip\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/lwip.
## Dependency:  N/A.
## Notice:      N/A.
## Relative doc:N/A.
##
ifeq ($(MTK_LWIP_STATISTICS_ENABLE),y)
COM_CFLAGS         += -DMTK_LWIP_STATISTICS_ENABLE
endif


##
## MTK_MAIN_CONSOLE_UART2_ENABLE
## Brief:       This option supports CM4 console output via UART2 (UART1 in
##              HW spec.) instead of UART1 (UART0 in HW spec). It needs the
##              board circuit supports, and only applied in MTK EVB only so
##              far. Default should be off.
## Path:        N/A
## Dependency:  N/A
##
ifeq ($(MTK_MAIN_CONSOLE_UART2_ENABLE),y)
COM_CFLAGS         += -DMTK_MAIN_CONSOLE_UART2_ENABLE
endif


##
## MTK_MBEDTLS_CONFIG_FILE
## Brief:       This option is to configure mbedTLS features.
## Usage:       If the value is "*.h", mbedTLS module will use *.h as the configuration file. For example, if its value is "config-mtk-basic.h",
##              config-mtk-basic.h will be used as the configuration file. MTK_MBEDTLS_CONFIG_FILE compile option will be defined. You must also
##              include the /middleware/third_party/mbedtls/module.mk in your Makefile before setting the option to "*.h".
## Path:        middleware/third_party/mbedtls
## Dependency:  LWIP module must be enabled.
## Related doc: Please refer to internet and open source software guide under the doc folder for more detail.
##
ifneq ($(MTK_MBEDTLS_CONFIG_FILE),)
COM_CFLAGS         += -DMBEDTLS_CONFIG_FILE=\"$(MTK_MBEDTLS_CONFIG_FILE)\"
endif


##
## MTK_MINICLI_ENABLE
## Brief:       This option is to enable and disable CLI (command line interface) engine.
## Usage:       If the value is "y", the MTK_MINICLI_ENABLE compile option will be defined. You must also include the \middleware\MTK\minicli\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/MTK/minicli
## Dependency:  HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
## Notice:      N/A
## Related doc: N/A
##
ifeq ($(MTK_MINICLI_ENABLE),y)
COM_CFLAGS         += -DMTK_MINICLI_ENABLE
endif


##
## MTK_OS_CLI_ENABLE
## Brief:  Enable OS CLI commands.
## Usage:  If the value is "y", the MTK_OS_CLI_ENABLE compile option will be defined.
## Path:   N/A
## Dependency:  N/A
##
ifeq ($(MTK_OS_CLI_ENABLE),y)
COM_CFLAGS         += -DMTK_OS_CLI_ENABLE
endif


##
## MTK_OS_CPU_UTILIZATION_ENABLE
## Brief:       This option is to enable and disable cpu utilization function.
## Usage:     If the value is "y", the MTK_OS_CPU_UTILIZATION_ENABLE compile option will be defined and supports the 'os 2' MTK CLI commands to show/get statistics of CM4 CPU utilizations of all the tasks running on.You must also include the \kernel\service\module.mk in your Makefile before setting the option to "y".
## Path:       kernel/service
## Dependency:  MTK_MINICLI_ENABLE must be enabled in this file.
## Notice:      None
## Realted doc: None
##
ifeq ($(MTK_OS_CPU_UTILIZATION_ENABLE),y)
COM_CFLAGS         += -DMTK_OS_CPU_UTILIZATION_ENABLE
endif

##
## MTK_SWLA_ENABLE
## Brief:       This option is to enable and disable the Software Logical Analyzer service, Each event(task/isr activity) is recorded while CPU context switching, also support customization tag
## Usage:     If the value is "y", the MTK_SWLA_ENABLE compile option will be defined. You must also include the gva\kernel\service\module.mk in your Makefile before setting the option to "y".
## Path:       kernel/service
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SWLA_ENABLE),y)
CFLAGS += -DMTK_SWLA_ENABLE
endif

##
## MTK_PING_OUT_ENABLE
## Brief:       It supports MTK lite-ping tool to issue ping request toward the other peer of the connection. It's used by RD for debugging.
## Usage:       If the value is "y", the MTK_PING_OUT_ENABLE compile option will be defined. You must also include the middleware\third_party\ping\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/third_party/ping
## Dependency:  MTK_MINICLI_ENABLE must be defined in the project.
##
ifeq ($(MTK_PING_OUT_ENABLE),y)
COM_CFLAGS         += -DMTK_PING_OUT_ENABLE
endif


##
## MTK_WIFI_WPS_ENABLE
## Brief:       This option is to enable/disable Wi-Fi WPS features.
## Usage:       If the value is "y", the MTK_WIFI_WPS_ENABLE will be defined,You must include some module in your Makefile before setting
##              the option to "y".
##              include the middleware/MTK/wifi_service/combo/src/module.mk
##              include the libwifi.a libwifi_7682_ram.a
## Path:        middleware/MTK/wifi_stack/src
## Dependency:  libwifi libwifi_7682_ram
## Related doc: Please refer to wifi dev guide under the doc folder for more detail.
##
ifeq ($(MTK_WIFI_WPS_ENABLE),y)
COM_CFLAGS += -DMTK_WIFI_WPS_ENABLE
COM_CFLAGS += -DWSC_V2_SUPPORT
endif

##
## MTK_SMTCN_V4_ENABLE
## Brief:       This option is to enable/disable WIFI smart connection which use multicast protocol
## Usage:       If the value is "y", the MTK_SMTCN_V4_ENABLE compile option will be defined. You must also include the middleware/MTK/smtcn/module.mk
##              and libsmtcn.a, libwifi.a in your Makefile before setting the option to "y".
## Path:        middleware/MTK/smtcn/src
## Dependency:  libwifi, libwifi_ram
## Related doc: Please refer to wifi dev guide under the doc folder for more detail.
##
ifeq ($(MTK_SMTCN_V4_ENABLE),y)
COM_CFLAGS         += -DMTK_SMTCN_V4_ENABLE
endif

##
## MTK_SMTCN_V5_ENABLE
## Brief:       This option is to enable/disable WIFI smart connection which use broadcast protocol
## Usage:       If the value is "y", the MTK_SMTCN_V5_ENABLE compile option will be defined. You must also include the middleware/MTK/smtcn/module.mk
##              and libsmtcn.a, libwifi.a in your Makefile before setting the option to "y".
## Path:        middleware/MTK/smtcn/src
## Dependency:  libwifi, libwifi_ram
## Related doc: Please refer to wifi dev guide under the doc folder for more detail.
##
ifeq ($(MTK_SMTCN_V5_ENABLE),y)
COM_CFLAGS         += -DMTK_SMTCN_V5_ENABLE
endif


##
## MTK_AUDIO_MP3_ENABLED
## Brief:       This option is to enable middleware audio mp3 functions under middleware/MTK/audio/mp3_codec/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_MP3_ENABLED compile option will be defined, the MP3 sources files, header files and library under middleware/MTK/audio/mp3_codec/src, middleware/    MTK/audio/mp3_codec/inc and middleware/MTK/audio/mp3_codec/lib  will be included by middleware/MTK/audio/module.mk.
##              You must also include the /middleware/MTK/audio/module.mk, /middleware/third_party/fatfs/module.mk and /middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a in your Make    file before setting the option to "y".
## Path:        middleware/MTK/audio/mp3_codec/
## Dependency:  Must enable MTK_FATFS_ENABLE in the feature.mk under the project GCC folder..
## Notice:      The default implementation of audio mp3 functions under middleware/MTK/audio/ is none.
## Relative doc:None
##
ifeq ($(MTK_AUDIO_MP3_ENABLED), y)
CFLAGS += -DMTK_AUDIO_MP3_ENABLED
endif

##
## MTK_AUDIO_AAC_DECODER_ENABLED
## Brief:       This option is to enable middleware audio AAC functions under middleware/MTK/audio/aac_codec/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_AAC_DECODER_ENABLED compile option will be defined, the AAC sources files, header files and library under middleware/MTK/audio/aac_codec/src, middleware/MTK/audio/aac_codec/inc and prebuilt/middleware/MTK/audio/aac_codec/lib/  will be included by middleware/MTK/audio/module.mk.
##              You must also include the gva/middleware/MTK/audio/module.mk, gva/middleware/third_party/fatfs/module.mk and gva/prebuilt/middleware/MTK/audio/aac_codec/lib/libheaac_decoder.a in your Makefile before setting the option to "y".
## Path:        middleware/MTK/audio/aac_codec/
## Dependency:  Must enable MTK_FATFS_ENABLE in the feature.mk under the project GCC folder..
## Notice:      The default implementation of audio AAC functions under middleware/MTK/audio/ is none.
## Relative doc:None
##
ifeq ($(MTK_AUDIO_AAC_DECODER_ENABLED), y)
CFLAGS	+= -DMTK_AUDIO_AAC_DECODER_ENABLED
endif


##
## MTK_AUDIO_AMR_ENABLED
## Brief:       This option is to enable middleware audio amr functions under middleware/MTK/audio/amr_codec/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_AMR_ENABLED compile option will be defined, the AMR sources files, header files and library under middleware/MTK/audio/amr_codec/src, middleware/MTK/audio/amr_codec/inc and middleware/MTK/audio/amr_codec/lib  will be included by middleware/MTK/audio/module.mk.
##              You must also include the /middleware/MTK/audio/module.mk, /middleware/third_party/fatfs/module.mk and /middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a in your Make    file before setting the option to "y".
## Path:        middleware/MTK/audio/amr_codec/
## Dependency:  Must enable MTK_FATFS_ENABLE in the feature.mk under the project GCC folder.
## Notice:      The default implementation of audio AMR functions under middleware/MTK/audio/ is none.
##
ifeq ($(MTK_AUDIO_AMR_ENABLED), y)
CFLAGS += -DMTK_AUDIO_AMR_ENABLED
endif

##
## MTK_AUDIO_AMR_FEATURE_ENCODE_DEMO
## Brief:       This option is to enable audio amr demo under gva/project/mt7682_hdk/apps/audio_ref_design folder.
## Usage:       If the value is "y", the MTK_AUDIO_AMR_FEATURE_ENCODE_DEMO compile option will be defined, the AMR demo sources files, header files under gva/project/mt7682_hdk/apps/audio_ref_design will be included.
## Path:         gva/project/mt7682_hdk/apps/audio_ref_design
## Dependency:  Must enable MTK_FATFS_ENABLE, MTK_AUDIO_AMR_ENABLED in the feature.mk under the project GCC folder.
## Notice:      None.
##
ifeq ($(MTK_AUDIO_AMR_FEATURE_ENCODE_DEMO), y)
CFLAGS += -DMTK_AUDIO_AMR_FEATURE_ENCODE_DEMO
endif

##
## MTK_AUDIO_AMR_FEATURE_DECODE_DEMO
## Brief:       This option is to enable audio amr demo under gva/project/mt7682_hdk/apps/audio_ref_design folder.
## Usage:       If the value is "y", the MTK_AUDIO_AMR_FEATURE_DECODE_DEMO compile option will be defined, the AMR demo sources files, header files under gva/project/mt7682_hdk/apps/audio_ref_design will be included.
## Path:         gva/project/mt7682_hdk/apps/audio_ref_design
## Dependency:  Must enable MTK_FATFS_ENABLE, MTK_AUDIO_AMR_ENABLED in the feature.mk under the project GCC folder.
## Notice:      None.
##
ifeq ($(MTK_AUDIO_AMR_FEATURE_DECODE_DEMO), y)
CFLAGS += -DMTK_AUDIO_AMR_FEATURE_DECODE_DEMO
endif

##
## MTK_I2S_ENABLED
## Brief:       This option is temp used and it is to enable audio playback with I2S.
## Path:        middleware/MTK/audio/mp3_codec/ and middleware/MTK/audio/amr_codec/
## Dependency:  HAL_I2S_MODULE_ENABLED must be defined in the project.
##
ifeq ($(MTK_I2S_ENABLED), y)
CFLAGS += -DMTK_I2S_ENABLED
endif



##
## MTK_SYS_TRNG_ENABLE
## Brief:       This option is to enable the system TRNG feature
##              Seed the system random number generator using True Random
##              Number Generator (TRNG) hardware.
## Usage:       Enable the feature by configuring it as y.
## Path:        driver/chip/mt7686/src
## Dependency:  None.
## Notice:      None.
## Relative doc:None
##
ifeq ($(MTK_SYS_TRNG_ENABLE),y)
COM_CFLAGS         += -DMTK_SYS_TRNG_ENABLE
endif




##
## MTK_WIFI_CONFIGURE_FREE_ENABLE
## Brief:     To enable WiFi provisioning w/o any manual configure operation
## Usage:   If the value is "y", and the NVRAM/NVDM CONFIGURE_FREE_ENABLE =1, the N9 FW will sniff MediaTek proprietary IE in probe request/response
##              Once matched, the credentials handshaking will be performed automatically and save to NVRAM/NVDM to finish WiFi Provisioning
## Path:      project/mt7687_hdk/apps/iot_sdk/src/network_init.c
## Dependency:  None
## Notice:   If no NVRAM/NVDM, this feature cann't work.
## Realted doc: None
##
ifeq ($(MTK_WIFI_CONFIGURE_FREE_ENABLE),y)
COM_CFLAGS          += -DMTK_WIFI_CONFIGURE_FREE_ENABLE
endif


## MTK_WIFI_TGN_VERIFY_ENABLE
## Brief:       This option is used to build an image dedicated for TGn ASD cerifitication.
## Usage:       If the value is "y", the SYSRAM will be used for packet buffer (64KB) , and there will be less SYSRAM for customers
##              Default "n", packet buffer will locate at TCM, instead of SYSRAM
## Path:        project/mt7687_hdk/apps/iot_sdk/inc/lwipopts.h
##              project/mt7697_hdk/apps/iot_sdk/inc/lwipopts.h
##              project/mt7687_hdk/apps/iot_sdk/inc/FreeRTOSConfig.h
##              project/mt7697_hdk/apps/iot_sdk/inc/FreeRTOSConfig.h
## Dependency:  MTK_HIF_GDMA_ENABLE must disable, if this compile option is off
## Notice:      If turn on this option SYSRAM may not enough for user application.
## Realted doc: None
##
ifeq ($(MTK_WIFI_TGN_VERIFY_ENABLE),y)
COM_CFLAGS         += -DMTK_WIFI_TGN_VERIFY_ENABLE
endif


##
## MTK_NVDM_ENABLE
## Brief:       This option is to enable NVDM feature.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/nvdm
## Dependency:  Flash driver must be enabled.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_NVDM_ENABLE),y)
  CFLAGS += -DMTK_NVDM_ENABLE
endif


##
## MTK_SECURE_BOOT_ENABLE
## Brief:       This option is to enable secure boot feature in bootloader.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/secure_boot
## Dependency:  libmbedtls
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SECURE_BOOT_ENABLE),y)
  CFLAGS += -DMTK_SECURE_BOOT_ENABLE
endif


##
## MTK_BOOTLOADER_USE_MBEDTLS
## Brief:       This option is to enable using mbedtls in bootloader.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/third_party/mbedtls
## Dependency:  libmbedtls
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_BOOTLOADER_USE_MBEDTLS),y)
  CFLAGS += -DMTK_BOOTLOADER_USE_MBEDTLS
endif


##
##
## MTK_ATCI_ENABLE
## Brief:       This option is to enable the ATCI feature
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/atci/src, middleware/MTK/atci/inc/
## Dependency:  None.
## Notice:      None.
## Relative doc:None
##

ifeq ($(MTK_ATCI_ENABLE),y)
COM_CFLAGS         += -DMTK_ATCI_ENABLE
endif

##
## MTK_AT_CMD_DISABLE
## Brief:       This option is to disable the default AT command under middleware/MTK/atci/at_command/
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/atci/at_command/, middleware/MTK/atci/inc/
## Dependency:  None.
## Notice:      None.
## Relative doc:None
##

ifeq ($(MTK_AT_CMD_DISABLE),y)
COM_CFLAGS         += -DMTK_AT_CMD_DISABLE
endif


##
## MTK_FATFS_ON_SPI_SD
## Brief:       This option is to enable FatFS feature on SD based on SPI.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/third_party/fatfs
## Dependency:  The hardware of SD based on SPI must be valid.
## Notice:      None
## Relative doc:None
##

##
## MTK_SYSTEM_HANG_CHECK_ENABLE
## Brief:       This option is to enable system hang issue debug feature .
## Usage:       Enable the feature by configuring it as y.
## Path:        kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F       
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SYSTEM_HANG_CHECK_ENABLE),y)
COM_CFLAGS         += -DMTK_SYSTEM_HANG_CHECK_ENABLE
endif

ifeq ($(MTK_FATFS_ON_SPI_SD),y)
  CFLAGS += -DMTK_FATFS_ON_SPI_SD
endif

COM_CFLAGS += -DMTK_MT7686_ENABLE



##
## MTK_NO_PSRAM_ENABLE
## Brief:       This option is to enable/disable PSRAM.
## Usage:       If the value is "y", the MTK_HAL_PLAIN_LOG_ENABLE will be defined, PSRAM will be initialized 
##              in the initialization phase the option to "y" include the driver/chip/mtxxxx/module.mk.
## Path:        driver/chip/mtxxxx       
## Dependency:  None
## Notice:      None
## Relative doc:None
##

ifeq ($(MTK_NO_PSRAM_ENABLE),y)
CFLAGS         += -DMTK_NO_PSRAM_ENABLE
endif

##
## MTK_USER_FAST_TX_ENABLE
## Brief:       This option is to support sending out customized data as fast as possible
## Usage:       Enable the feature by configuring it as y.
## Path:        project/mt7682_hdk/apps/iot_sdk_demo/src/sys_init.c, middleware/MTK/connsys/src/connsys_util.c
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_USER_FAST_TX_ENABLE),y)
CFLAGS         += -DMTK_USER_FAST_TX_ENABLE
endif

##
## MTK_TF_WIFI_AUTO_TEST
## Brief:       1.This option is only used to enable the wifi auto test use test framework project.
## Usage:       In iot_testframework project, enable MTK_TF_WIFI_AUTO_TEST=y in feature.mk
## Path:        
## Dependency:  
## Notice:      Because of iot_testframework limited. Tickless, heapsize will be change in auto test.
##              In 7686/7682, this feature will prolong the minicli enter sleep mode time from 3 seconds to 10s.
##              In iot_testframework project, this feature will enlarge heap to 200k
##              And it only used in iot_testframework auto test. For build iot_testframework and iot_sdk/iot_sdk_demo project
## Related doc: None.
##
ifeq ($(MTK_TF_WIFI_AUTO_TEST),y)
COM_CFLAGS += -DMTK_TF_WIFI_AUTO_TEST
endif

###############################################################################
##
## The following makefile options are not configurable or only for internal user. They may be removed in the future. 

##
## MTK_MET_TRACE_ENABLE
## The following makefile options are not configurable and may be removed in the future.
## Please do not set the makefile options to "y" in your GCC feature configuration.
##
ifeq ($(MTK_MET_TRACE_ENABLE),y)
COM_CFLAGS         += -DMET_TRACE_ENABLE
endif

##
## MTK_HEAP_GUARD_ENABLE
## Brief:       The following makefile options are not configurable and may
##              be removed in the future. Please do not set the makefile
##              options to "y" in your GCC feature configuration.
##
ifeq ($(MTK_HEAP_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
COM_CFLAGS     += -DHEAP_GUARD_ENABLE
endif

##
## MTK_AP_SNIFFER_ENABLE enables built-in MTK AP-sniffer concurrent mode.
## Default should be enabled.
## Internal Use
##
ifeq ($(MTK_AP_SNIFFER_ENABLE),y)
COM_CFLAGS         += -DMTK_AP_SNIFFER_ENABLE
endif

##
## MTK_BSP_LOOPBACK_ENABLE
## Brief:       This option is to enable/disable MTK WiFi throughput loopback test.
## Usage:       If the value is "y", the MTK_BSP_LOOPBACK_ENABLE will be defined. Enable will supports WiFi throughput loopback
##              test from CM4 to N9, without round trip to LMAC and the air.
##              It's for RD internal development and debug. Default should be disabled.
## Path:        N/A
## Dependency:  N/A.
## Related doc: N/A.
ifeq ($(MTK_BSP_LOOPBACK_ENABLE),y)
COM_CFLAGS         += -DMTK_BSP_LOOPBACK_ENABLE
endif

##
## MTK_HEAP_SIZE_GUARD_ENABLE
## Brief:       Internal use.
##
ifeq ($(MTK_HEAP_SIZE_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
COM_CFLAGS     += -DMTK_HEAP_SIZE_GUARD_ENABLE
endif

##
## MTK_MPERF_ENABLE
## Brief:       This option is to enable and disable profiling tool that is intended for internal use, it supports only GCC based environment and is not to be used by developers/users. Default should be off.
##              MTK_MPERF_ENABLE provides an experimental profiling tool that is intended
##              for internal use, it supports only GCC based environment and is not to be
##              used by developers/users. Default should be off.
## Usage:       If the value is "y", the MTK_MPERF_ENABLE compile option will be defined. You must also include the middleware/protected/mperf/module.mk in your Makefile before setting the option to "y".
## Path:        middleware/protected/mperf
## Dependency:  N/A
## Notice:      Despite there is no feature option dependency, MTK_MPERF_ENABLE relies on CMSIS header files.
## Related doc: N/A
##
## Internal use.
##
ifeq ($(MTK_MPERF_ENABLE),y)
COM_CFLAGS         += -DMTK_MPERF_ENABLE
COM_CFLAGS         += -I$(SOURCE_DIR)/middleware/protected/mperf/inc
endif

##
## MTK_SUPPORT_HEAP_DEBUG
## MTK_SUPPORT_HEAP_DEBUG is a option to show heap status (alocatted or free),
## and will print debug info if any heap crash or heap use overflow, It's
## for RD internal development and debug. Default should be disabled.
##
ifeq ($(MTK_SUPPORT_HEAP_DEBUG),y)
COM_CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG
endif

##
## MTK_LOAD_MAC_ADDR_FROM_EFUSE
## Brief:       This option is to configure the wifi MAC address initialization source .
##              when the feature is enabled, if wifi MAC address in NVDM is NULL, system will read it from EFUSE then calculating STA MAC address
##              and AP MAC address, then stored them into NVDM.   if Efuse also has no valid MAC address. system will read the MAC address from
##              the array table in wifi_nvdm_config.c
##              when the feature is disabled, if wifi MAC address in NVDM is NULL, system will read the MAC address from the array table in wifi_nvdm_config.c
## Usage:       If the value is "y", the MTK_LOAD_MAC_ADDR_FROM_EFUSE will be defined.
## Path:        middleware/MTK/wifi_service/combo/src
## Dependency:  NVDM
## Notice:      This feature is deprecated, it only used in internal project now.
## Related doc: None.
##
ifeq ($(MTK_LOAD_MAC_ADDR_FROM_EFUSE),y)
COM_CFLAGS += -DMTK_LOAD_MAC_ADDR_FROM_EFUSE
endif

##
## MTK_WIFI_PROFILE_ENABLE
## Brief:       1.This option is to enable/disable the wifi profile API and CLI features.
##              when the feature is enabled, the wifi profile APIs are available to get and set the setting into NVDM.
##              when the feature is disabled, the wifi profile APIs are unavailable
##              2.This option is to enable/disable wifi settings initialization from NVDM
## Usage:       If the value is "y", the MTK_WIFI_PROFILE_ENABLE will be defined,You must include the middleware/MTK/wifi_service/combo/src/module.mk
##              in your Makefile before setting the option to "y".
## Path:        middleware/MTK/wifi_service/combo/src
## Dependency:  NVDM
## Notice:      This feature is deprecated,  The wifi profile API and CLI feature will no longer work.
##              And it only used in internal project now for wifi settings initialization from NVDM.
##              The options enable need NVDM enable
## Related doc: None.
##
ifeq ($(MTK_WIFI_PROFILE_ENABLE),y)
COM_CFLAGS += -DMTK_WIFI_PROFILE_ENABLE
endif


##
## IC_CONFIG
## DO NOT USE, software not available.
##

ifeq ($(MTK_SUPPORT_SFC_DVT_ENABLE),y)
  CFLAGS += -D__SF_DVT__
endif


##
## Code Test Coverage option.
##
ifeq ($(MTK_CODE_COVERAGE_ENABLE),y)
AR = $(BINPATH)/arm-none-eabi-cov-ar
CC = $(BINPATH)/arm-none-eabi-cov-gcc
CXX= $(BINPATH)/arm-none-eabi-cov-g++
export GCOV_DIR=$(SOURCE_DIR)
endif
###############################################################################

#Incldue Path
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
COM_CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt7686/Include
COM_CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
COM_CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7686/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/connsys/inc
COM_CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
COM_CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt7686/src/common/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/tftp/inc

COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/cjson/inc

ifeq ($(findstring y,$(MTK_JOYLINK_ENABLE)$(MTK_ALINK_ENABLE)$(MTK_AIRKISS_ENABLE)),y)
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/cloud/multi_smart_config/inc
endif
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/dhcpd/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/configs
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src/mediatek
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTPacket/src
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes/nghttp2
ifeq ($(MTK_NVDM_ENABLE),y)
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
endif
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/smtcn/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/sntp/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/xml/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/iperf/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/ping/inc
COM_CFLAGS += -I$(SOURCE_DIR)/project/common/bsp_ex/inc
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
COM_CFLAGS += -I$(SOURCE_DIR)/kernel/service/src_protected/inc
#Homekit & Security
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/curve25519/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/ed25519/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/chacha20poly1305/inc
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/srp/inc
ifeq ($(MTK_SECURE_BOOT_ENABLE),y)
COM_CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/secure_boot/inc
endif

CFLAGS     += -std=gnu99 $(COM_CFLAGS)
CXXFLAGS   += -std=c++11 $(COM_CFLAGS)

#Middleware Module Path
MID_TFTP_PATH 		= $(SOURCE_DIR)/middleware/MTK/tftp
MID_FOTA_PATH 		= $(SOURCE_DIR)/middleware/MTK/fota
MID_GNSS_PATH 		= $(SOURCE_DIR)/middleware/MTK/gnss
MID_LWIP_PATH 		= $(SOURCE_DIR)/middleware/third_party/lwip
MID_DHCPD_PATH 		= $(SOURCE_DIR)/middleware/MTK/dhcpd
MID_HTTPCLIENT_PATH = $(SOURCE_DIR)/middleware/third_party/httpclient
MID_MBEDTLS_PATH 	= $(SOURCE_DIR)/middleware/third_party/mbedtls
MID_MINICLI_PATH 	= $(SOURCE_DIR)/middleware/MTK/minicli
MID_MQTT_PATH 		= $(SOURCE_DIR)/middleware/third_party/mqtt
MID_NGHTTP2_PATH 	= $(SOURCE_DIR)/middleware/third_party/nghttp2
ifeq ($(MTK_NVDM_ENABLE),y)
MID_NVDM_PATH 		= $(SOURCE_DIR)/middleware/MTK/nvdm
endif

MID_CJSON_PATH 		= $(SOURCE_DIR)/middleware/third_party/cjson
MID_SNTP_PATH 		= $(SOURCE_DIR)/middleware/third_party/sntp
MID_XML_PATH 		= $(SOURCE_DIR)/middleware/third_party/xml
MID_HTTPD_PATH 		= $(SOURCE_DIR)/middleware/third_party/httpd
MID_PING_PATH 		= $(SOURCE_DIR)/middleware/third_party/ping
MID_IPERF_PATH 		= $(SOURCE_DIR)/middleware/MTK/iperf
MID_FATFS_PATH 		= $(SOURCE_DIR)/middleware/third_party/fatfs
DRV_CHIP_PATH 		= $(SOURCE_DIR)/driver/chip/mt7686
DRV_BSP_PATH 		= $(SOURCE_DIR)/driver/board/mt7682_hdk
DRV_G2D_PATH 		= $(SOURCE_DIR)/driver/board/component/g2d
KRL_OS_PATH 		= $(SOURCE_DIR)/kernel/rtos/FreeRTOS
KRL_SRV_PATH		= $(SOURCE_DIR)/kernel/service
ifeq ($(MTK_SECURE_BOOT_ENABLE),y)
MID_SECURE_BOOT_PATH    = $(SOURCE_DIR)/middleware/MTK/secure_boot
endif
#Homekit & Security
MID_HOMEKIT_PATH    = $(SOURCE_DIR)/$(HOMEKIT_DIR)
MID_CURVE25519_PATH = $(SOURCE_DIR)/middleware/third_party/curve25519
MID_ED25519_PATH    = $(SOURCE_DIR)/middleware/third_party/ed25519
MID_CHACHA20POLY1305_PATH =  $(SOURCE_DIR)/middleware/third_party/chacha20poly1305
MID_SRP_PATH        = $(SOURCE_DIR)/middleware/third_party/srp
ifeq ($(MTK_MPERF_ENABLE),y)
MID_MPERF_PATH      = $(SOURCE_DIR)/middleware/protected/mperf
endif


MID_LZMA_PATH           = $(SOURCE_DIR)/middleware/third_party/lzma_decoder
