/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
/**
 * @addtogroup mt7682_hdk mt7682_hdk
 * @{
 * @addtogroup mt7682_hdk_apps apps
 * @{
 * @addtogroup mt7682_hdk_apps_bootloader bootloader
 * @{

@par Overview
  - This is not an example project. This project is a bootloader for MT7682 HDK.
    The developer can refer this project to understand how
    to build a bootloader.

@par Hardware and software environment
  - Environment configuration
    - The output logs are communicated through a micro USB cable to the PC
      from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the MT7682
      HDK drivers on Microsoft Windows" on the "MT7682_HDK_Users_Guide.pdf"
      in [sdk_root]/doc/board/mt7682_hdk folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the MT7682 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "Airoha IoT SDK for RTOS Get Started Guide" in [sdk_root]/doc folder.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b main.c:                  Main program
    - \b inc/hal_feature_config.h:
                                  MT7682's feature configuration file.
    - \b src/custom_fota.c:       Fota update function customization.
    - \b GCC/startup_bootloader.s:
                                  MT7682 startup file for GCC.
    - \b GCC/syscalls.c:          The minimal implementation of the system
                                  calls.
    - \b EWARM/startup_mt7686.s:  MT7682 startup file for IAR IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:     Feature configuration.
    - \b GCC/Makefile:       Makefile.
    - \b GCC/bootloader.ld:  Linker script.
  - Project configuration files using IAR IDE
    - \b EWARM/bootloader.ewp:     IAR project file. Contains the project
                                   structure in XML format
    - \b EWARM/bootloader.eww:     IAR workspace file. Contains project
                                   information.
    - \b EWARM/flash.icf:          Linker script.

@par Run the example
  - Connect board to the PC with serial port cable.
  - Build bootloader and an example project with the command, "./build.sh
    mt7682_hdk example_project_name bl" under the SDK root folder and
    download the binary file to MT7682 HDK.
  - Power on MT7682 HDK and bootloader log are showen on serial tool.
 * @}
 * @}
 * @}
*/
