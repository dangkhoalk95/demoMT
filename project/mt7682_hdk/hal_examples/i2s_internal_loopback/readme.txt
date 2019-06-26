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
 * @addtogroup mt7682_hdk_templates templates
 * @{
 * @addtogroup mt7682_hdk_hal_examples_i2s_internal_loopback i2s_internal_loopback
 * @{

@par Overview
  - Example description
    - This example is a reference application to demonstrate the usage of I2S
      module APIs. This example project does not require FreeRTOS.
    - Features of the example project
    - This project is a reference application to use the I2S APIs to apply an
      internal loopback. The data sent from the I2S TX link will be looped
      back to the I2S RX link.
  - Results
    - The input and output of transmitted data are described below.
    - Input to the example.
      - Data in the audio_Tone2k_16kSR array is shown below: 0xffff, 0xcd1a,
        0xb805, 0xcd1b, 0x0000, 0x32e6, 0x47fb, 0x32e5, 0x0000, 0xcd1a,
        0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5, 0x0000, 0xcd1b,
        0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5, 0x0000, 0xcd1b,
        0xb806, 0xcd1a, 0xffff, 0x32e5, 0x47f9, 0x32e6
    - Output from the example.
      - Log will show the data received by I2S RX link: 0xffff, 0xcd1a,
        0xb805, 0xcd1b, 0x0000, 0x32e6, 0x47fb, 0x32e5, 0x0000, 0xcd1a,
        0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5, 0x0000, 0xcd1b,
        0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5, 0x0000, 0xcd1b,
        0xb806, 0xcd1a, 0xffff, 0x32e5, 0x47f9, 0x32e6

@par Hardware and software environment
  - Supported platform
    - LinkIt 7682 HDK.
  - HDK switches and pin configuration
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
    - \b src/system_mt7682.c:   MT7682x clock configuration file.
    - \b inc/hal_feature_config.h:
                                MT7682x feature configuration file.
    - \b inc/memory_map.h:      MT7682x memory layout symbol file.
    - \b GCC/startup_mt7682.s:  MT7682x start up file for GCC.
    - \b GCC/syscalls.c:        MT7682x syscalls for GCC.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile.:   Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using IAR
    - \b EWARM/i2s_internal_loopback.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/i2s_internal_loopback.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/i2s_internal_loopback.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt7686_hdk
    i2s_internal_loopback" from the SDK root folder and download the binary
    file to the LinkIt 7686 development board.
  - Connect your board to the PC with a micro-USB cable.
  - Run the example. The transaction result is displayed in the log. Please
    refer to "Input to the example" and "Output from the example" sections.
*/
/**
 * @}
 * @}
 * @}
 */
