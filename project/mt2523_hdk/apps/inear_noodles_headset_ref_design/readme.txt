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
 * @addtogroup mt2523_hdk mt2523_hdk
 * @{
 * @addtogroup mt2523_hdk_apps apps
 * @{
 * @addtogroup mt2523_hdk_apps_inear_noodles_headset_ref_design inear_noodles_headset_ref_design
 * @{

@par Overview
  - Application description
    - This application demonstrates headset features based on MediaTek MT2523
      SOC for RTOS development platform. The architecture diagram is shown 
      below.
      @image html headset_ref_design_architecture.png
    - Application features
      - Sink. Enables to connect with a remote device, handle calls and play music
        from the remote device.
      - Battery Service (BAS). Provides the battery status of MT2523 to a remote
        device.
      - Apple Notification Center Service (ANCS). Provides a simple and convenient
        way to access the notifications generated on the iOS devices.
      - Bluetooth Notification. Receives and displays notifications from Android
        devices.
      - Audio Player. Plays local MP3 music.
      - Firmware update Over The Air (FOTA). Uses Bluetooth notification service to 
        receive FOTA packages and trigger an update on the target board.
      - Find Me Profile (FMP). Allows users to find the misplaced device using
        Bluetooth LE.
      - Low Power demo. Provides the method to measure the current consumption
        of different scenario by AT command. User should strictly follow the 
        measuring steps provided in the readme file of low power folder
        (../headset_ref_design/src/low_power_demo).
      - The application features are independent, if not otherwise mentioned.
        
@par Hardware and software environment
  - Supported platform
    - Inear-noodles headset.
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 2523
      HDK drivers on Microsoft Windows" on the "LinkIt 2523 HDK User Guide"
      in [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the LinkIt 2523 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "Airoha IoT SDK for RTOS Get Started Guide" in [sdk_root]/doc folder.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.
    - By default, the application uses UART0 for logging.
    - By default, the application uses MK20 for AT command input, corresponds 
       to "mbed Serial Port" under Device Manager of the PC.
    - Please contact MediaTek customer support team to find more information
      regarding the pin configuration, hardware peripheral setup, debugger setup and
      more.
  - Customization options and usage
    - No compile option is available to disable an application feature.
    - To disable a feature, delete the feature initialization function in
       the main.c.
        
@par Directory contents
  - Source and header files. Most of code are co-used with headset_ref_design project. 
    - \b ../headset_ref_design/inc:                     Common header files.
    - \b ../headset_ref_design/src/ble_bas_app:         Bluetooth LE Battery Service source code.
    - \b ../headset_ref_design/src/Notification:        Android notifications source code.
    - \b ../headset_ref_design/src/ble_ancs_ios_notification:
                                                        ANCS iOS notifications source code.
    - \b ../headset_ref_design/src/low_power_demo:      Low Power demo source code.
    - \b ../headset_ref_design/src/sink:                Sink source code.
    - \b ../headset_ref_design/src/AudioPlayer:         Local audio player source code.
    - \b ../headset_ref_design/src/fota:                FOTA source code.
    - \b ../headset_ref_design/src/ble_find_me_server:  Find Me server source code.
    - \b ../headset_ref_design/src/main.c:              Main program.
    - \b ../headset_ref_design/src/ept_gpio_var_inear.c:
                                                        The GPIO configuration file generated by the
                                                        Easy Pinmux Tool (EPT).
    - \b ../headset_ref_design/src/ept_eint_var_inear.c:
                                                        The EINT configuration file generated by the
                                                        EPT.
    - \b ../headset_ref_design/src/system_mt2523.c:     The configuration file of the Cortex-M4 with
                                                        floating unit MCU core registers and system
                                                        clock.
    - \b ../headset_ref_design/src/at_command_serial_port.c:
                                                        AT command of port service file.
    - \b ../headset_ref_design/src/bt_app_common.c:     Bluetooth application common file.
    - \b ../headset_ref_design/src/bt_gattc_discovery.c:
                                                        GATT service discovery file.
    - \b ../headset_ref_design/src/task_def.c:          Task definition file.
    - \b ../headset_ref_design/src/bt_init.c:           Bluetooth initialization file.
    - \b ../headset_ref_design/src/gatt_service.c:      GATT and GAP service file.
    - \b ../headset_ref_design/src/hci_log.c:           Display HCI log.
  - Project configuration files using GCC
    - \b ../headset_ref_design/GCC/feature_inear.mk:    Feature configuration file.
    - \b Makefile:                                      Makefile.
    - \b ../headset_ref_design/GCC/flash_inear.ld:      Linker script.
    - \b ../headset_ref_design/GCC/syscalls.c:          MT2523 syscalls for GCC.
    - \b ../headset_ref_design/GCC/startup_mt2523.s:    MT2523 startup file for GCC.
    
@par Run the application
  - How to build the inear_noodles_headset_ref_design application
    - GCC version
      - make command "./build.sh mt2523_hdk inear_noodles_headset_ref_design" under
        the SDK root folder.
  - How to download the headset_ref_design application
    - Install and use Flash Tool ("sdk_root/tools/PC_tool_Win.zip")
      to download the application binary headset_ref_design.bin.
  - How to run each feature
    - Power on the inear-noodles headset to initialize and run the features in the
      background.
    - Users can use keypad to trigger sink and audio player actions.
    - BAS, ANCS, Blueooth Notification, FOTA and FMP server can also be 
      triggered on the smartphone. Find more details in each sub-feature
      project's readme file.
*/
/**
 * @}
 * @}
 * @}
 */
