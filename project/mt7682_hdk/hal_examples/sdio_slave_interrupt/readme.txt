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
 * @addtogroup mt7682_hdk_hal_examples hal_examples
 * @{
 * @addtogroup mt7682_hdk_hal_examples_sdio_slave_interrupt sdio_slave_interrupt
 * @{

@par Overview
  - Example description
    - This example is a reference application to demonstrate how to use SDIO
      master API to communicate with an SDIO slave with two LinkIt 7682 HDKs.
      The example is the SDIO slave part.
    - This example does not require FreeRTOS.
  - Results
    - The system result is in the log.
  - Features of the example project
    - The example project has two parts: 1. LinkIt 7682 HDK operates as SDIO
      Master device (sdio_host_interrupt). 2. LinkIt 7682 HDK operates as SDIO
      Slave device (sdio_slave_interrupt). The communication between master
      and slave devices is shown below.
      \code
      _________________________                       __________________________
      |           ______________|                     |______________            |
      |         |SDIO Master    |                     |    SDIO Slave|           |
      |         |               |                     |              |           |
      |         |      J2101 G11|_____________________|J2101 G11     |           |
      |         |      J2101 G12|_____________________|J2101 G12     |           |
      |         |      J2101 G13|_____________________|J2101 G13     |           |
      |         |      J2101 G14|_____________________|J2101 G14     |           |
      |         |      J2101 G15|_____________________|J2101 G15     |           |
      |         |      J2101 G16|_____________________|J2101 G16     |           |
      |         |_______________|                     |______________|           |
      |                         |                     |                          |
      |                         |                     |                          |
      |                         |                     |                          |
      |                         |                     |                          |
      |                J2111 GND|_____________________|J2111 GND                 |
      |                         |                     |                          |
      |_MT7682__________________|                     |_MT7682___________________|
      \endcode

@par Hardware and software environment
  - Supported platform
      - Linkit 7682 HDK should Remove R4101/R4102/R4103/R4104/R4105 and R5 on the sub-board.
      - Master SDIO need put jumper to high( J2201、J2202、J2203、J2205、J2206).
  - HDK switches and pin configuration
    - Connect J2101.G11 to J2101.G11
    - Connect J2101.G12 to J2101.G12
    - Connect J2101.G13 to J2101.G13
    - Connect J2101.G14 to J2101.G14
    - Connect J2101.G15 to J2101.G15
    - Connect J2101.G16 to J2101.G16
    - SDIO master module pin mapping table is shown below.
      | SDIO Pin| GPIOx     |    PINx     |
      |-------  |---------  |-----------  |
      |  CLK    | GPIO_11   | J2101.G11   |
      |  CMD    | GPIO_12   | J2101.G12   |
      |  DAT0   | GPIO_13   | J2101.G13   |
      |  DAT1   | GPIO_14   | J2101.G14   |
      |  DAT2   | GPIO_15   | J2101.G15   |
      |  DAT3   | GPIO_16   | J2101.G16   |
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
    - \b src/slt_sdio_slv.c:    SDIO slave test program.
    - \b src/system_mt7682.c:   MT7682 system clock configuration file.
    - \b inc/hal_feature_config.h:    MT7682 feature configuration file.
    - \b inc/memory_map.h:      MT7682 memory layout symbol file.
    - \b GCC/startup_mt7682.s:  MT7682 startup file for GCC.
    - \b GCC/syscalls.c:        MT7682 syscalls for GCC.
    - \b EWARM/startup_mt7682.s:
                                MT7682 startup file for IAR.
  - Project configuration files using GCC
    - \b GCC/feature.mk:        Feature configuration file.
    - \b GCC/Makefile:          Makefile.
    - \b GCC/mt7682_flash.ld:   Linker script.
  - Project configuration files using IAR
    - \b EWARM/sdio_slave_interrupt.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/sdio_slave_interrupt.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/sdio_slave_interrupt.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project for Master with a command "./build.sh
    mt7682_hdk sdio_slave_interrupt bl" from the SDK root folder and download
    the binary file to the LinkIt 7682 HDK that operates as SDIO Slave.
  - Build the example project for Host with a command "./build.sh mt7682_hdk
    sdio_host_interrupt bl" from the SDK root folder and download the
    binary file to the 7682 HDK that operates as SDIO Host.
  - Connect the related pins of the two LinkIt 7682 HDKs as shown in the
    "Features of the example project" and "HDK switches and pin
    configuration" sections.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example. The log will show the communication result with the SDIO
    Slave. The log of "example project test success." for
    master board and "example project test success." for
    slave board indicates a successful communication. Please make sure the
    LinkIt 7682 HDK boards (Slave) are powered up at the first.
*/
/**
 * @}
 * @}
 * @}
 */
