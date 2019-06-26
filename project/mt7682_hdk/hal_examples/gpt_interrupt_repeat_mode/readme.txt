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
 * @addtogroup mt7682_hdk_hal_examples_gpt_interrupt_repeat_mode gpt_interrupt_repeat_mode
 * @{

@par Overview
  - Example description
    - This example is a reference application to use GPT driver APIs in
      repeat mode.
    - This example does not require FreeRTOS.
  - Results
    - The system logs at a repetitive timer fires at an interval of 1000ms.

@par Hardware and software environment
  - Supported platform
    - LinkIt 7686 HDK.
  - HDK switches and pin configuration
    - N/A.
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 7686
      HDK drivers on Microsoft Windows" on the "LinkIt 7686 HDK User Guide"
      in [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the LinkIt 7686 HDK. For more information
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
    - \b src/main.c:              Main program.
    - \b src/system_mt7682.c:     mt7682x clock configuration file.
    - \b inc/hal_feature_config.h:
                                  mt7682x feature configuration file.
    - \b inc/memory_map.h:        mt7682x memory layout symbol file.
    - \b GCC/startup_mt7682.s:    mt7682x startup file for GCC.
    - \b GCC/syscalls.c:          mt7682x syscalls for GCC.
    - \b EWARM/startup_mt7682.s:  mt7682x startup file for IAR.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile.:   Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using IAR
    - \b EWARM/gpt_interrupt_repeat_mode.ewd:
      IAR project options. Contains the settings for the debugger.
    - \b EWARM/gpt_interrupt_repeat_mode.ewp:
      IAR project file. Contains the project structure in XML format.
    - \b EWARM/gpt_interrupt_repeat_mode.eww:
      IAR workspace file. Contains project information.

@par Run the example
  - Build the example project with a command "./build.sh mt7682_hdk
    gpt_interrupt_repeat_mode" from the SDK root folder and download the
    binary file to LinkIt 7686 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example. The log of "Expired at XXX ms." shows the repetitive
    timer fires at 1000ms intervals and indicates successful operation.
*/
/**
 * @}
 * @}
 * @}
 */

