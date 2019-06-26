/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors. Without the prior written permission of MediaTek and/or its
 * licensors, any reproduction, modification, use or disclosure of MediaTek
 * Software, and information contained herein, in whole or in part, shall be
 * strictly prohibited. You may only use, reproduce, modify, or distribute
 * (as applicable) MediaTek Software if you have agreed to and been bound by
 * the applicable license agreement with MediaTek ("License Agreement") and
 * been granted explicit permission to do so within the License Agreement
 * ("Permitted User"). If you are not a Permitted User, please cease any
 * access or use of MediaTek Software immediately.
 */

/**
 * @addtogroup mt7682_hdk mt7682_hdk
 * @{
 * @addtogroup mt7682_hdk_hal_examples hal_examples
 * @{
 * @addtogroup mt7682_hdk_hal_examples_rtc_set_alarm rtc_set_alarm
 * @{

@par Overview
  - Example description
    - This example is a reference application to use RTC API to control the
      RTC module on the MT7682 HDK, including set time, get time, set alarm
      and enable alarm functions.
    - This example does not require FreeRTOS.
  - Results
    - The alarm setting and result are printed in the log.

@par Hardware and Software environment
  - Supported platform
    - LinkIt 7682 HDK.
  - HDK swicthes, pin configuration
    - N/A.
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 7682
      HDK drivers on Microsoft Windows" on the "LinkIt 7682 HDK User Guide"
      in [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the LinkIt 7682 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "Airoha IoT SDK for RTOS Get Started Guide" in [sdk_root]/doc folder.
    - COM port settings. baudrate: 115200, data: 8 bits, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:            Main program.
    - \b src/system_mt7682.c:   MT7682 system clock configuration file.
    - \b inc/hal_feature_config.h:
                                MT7682 feature configuration file.
    - \b inc/memory_map.h:      MT7682 memory layout symbol file.
    - \b GCC/startup_mt7682.s:  MT7682 startup file for GCC.
    - \b GCC/syscalls.c:        MT7682 syscalls for GCC.
  - Project configuration files using GCC
    - \b GCC/feature.mk:        Feature configuration file.
    - \b GCC/Makefile:          Makefile.
    - \b GCC/mt7682_flash.ld:   Linker script.
  - Project configuration files using IAR
    - \b EWARM/rtc_set_alarm.ewd:  IAR project options. Contains the settings
                                   for the debugger.
    - \b EWARM/rtc_set_alarm.ewp:  IAR project file. Contains the project
                                   structure in XML format.
    - \b EWARM/rtc_set_alarm.eww:  IAR workspace file. Contains project
                                   information.
    - \b EWARM/flash.icf:          Linker script.

@par Run the example
  - Build the example project with a command "./build.sh mt7682_hdk
    rtc_set_alarm bl" under the SDK root folder and download the binary file
    to MediaTek LinkIt MT7682 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example. The log will show the current time and alarm time, and
    the result displayed in the log "---RTC example end---" indicates a
    successful alarm setting.
*/
/**
 * @}
 * @}
 * @}
 */
