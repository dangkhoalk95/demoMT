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
 * @addtogroup mt7686_hdk mt7686_hdk
 * @{
 * @addtogroup mt7686_hdk_templates templates
 * @{
 * @addtogroup mt7686_hdk_templates_no_rtos_initialize_system no_rtos_initialize_system
 * @{

@par Overview
  - Example description
    - Potential users of this project are developers who want to create their
      own project without FreeRTOS and the purpose of this project is to
      demonstrate how to achieve this goal on the basis of this project.
    - This example demonstrates how to create a bare minimum system.
  - How to create your own project based on this project
    - Clone this project to apps folder and rename it to your own project
      name.
    - To add your own application code, please follow comments described in
      the main.c of this project.
  - Features of the example project
    - The project initializes the hardware and demonstrates how to create
      user-defined applications.
  - Result
    - A welcome message will display in the log on the terminal window.

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
    - \b src/main.c:            Main program.
    - \b src/system_mt7686.c:   MT7686x clock configuration file.
    - \b inc/hal_feature_config.h:
                                MT7686x feature configuration file.
    - \b inc/memory_map.h:      MT7686x memory layout symbol file.
    - \b GCC/startup_mt7686.s:  MT7686x start up file for GCC.
    - \b GCC/syscalls.c:        MT7686x syscalls for GCC.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile.:   Makefile.

@par Run the example
  - Build the project with a command "./build.sh mt7686_hdk
    no_rtos_initialize_system bl" from the SDK root folder and download the
    binary file to LinkIt 7686 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example, the terminal window will show "welcome to main()"
    message and this indicates a successful operation.
*/
/**
 * @}
 * @}
 * @}
 */

