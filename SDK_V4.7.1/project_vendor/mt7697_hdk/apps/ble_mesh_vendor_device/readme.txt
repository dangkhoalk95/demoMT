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
 * @addtogroup mt7697_hdk mt7697_hdk
 * @{
 * @addtogroup mt7697_hdk_apps apps
 * @{
 * @addtogroup mt7697_hdk_apps_ble_mesh_vendor_device ble_mesh_vendor_device
 * @{
 
@par Overview
  - Application description
    - This is a reference application to show the Bluetooth mesh vendor device feature of the LinkIt 7697 HDK
      through the following:
      - How to use the Bluetooth mesh device functions.
  - Application features
    - Act as a Bluetooth low-energy (LE) device with mesh features. All Bluetooth Mesh Command Line
      Interface (CLI) commands are supported in this project.

@par Hardware and software environment
  - Supported platform
    - MediaTek LinkIt 7697 HDK
  - HDK switches and pin configuration
    - J36 provides the pins for GPIOs, PWMs, SPI master chip select 0, SPI
      master, and UART1 RX/TX.
    - J35 provides the pins for GPIOs, PWMs, UART2 RX/TX, UART1 RTS/CTS, SPI
      master chip select 1, IR TX, and IR RX.
    - J34 provides the pins for GPIOs, PWMs, UART2 RTS/CTS, I2S, SPI slave,
      and I2C0.
    - J33 provides the pins for GPIOs, PWMs, I2C0, and ADC0~3.
    - J32 provides the pins for GND, 5V, 3.3V, and the reset pin.
    - J25 sets the HDK to either Flash Normal mode or Flash Recovery mode. To
      update the firmware on the LinkIt 7697 HDK:
      - Set the Jumper J25 to FLASH Recovery mode; Jumpers J23, J26, J27,
        and J30 must be on.
      - In this mode, if the power is on, the board loads the ROM code and
        starts the ATE Daemon or Firmware Upgrade Daemon according to the
        MT76x7 Flash Tool's behavior on the PC. To run the project on the
        LinkIt 7697 HDK:
      - Set the Jumper J25 off to switch to FLASH Normal mode; Jumpers
        J23, J26, J27, and J30 must be on.
      - In this mode, if the power is on, the board loads firmware from
        the flash and reboots.
    - There are three buttons on the board:
      - RST - Reset
      - EINT - External interrupt
      - RTC_INT - RTC interrupt
  - Environment configuration
    - A serial tool is required, such as TeraTerm for UART logging.
    - COM port settings -baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none, and flow control: off.

@par Directory contents
  - This file
    - \b readme.txt.                                      Overview of the project
  - Source and header files
    - \b src/bt/bt_init.c:                                Bluetooth memory initialization and task creation.
    - \b src/bt/bt_app_callback_handler.c:                Implement the Bluetooth event handler.
    - \b src/bt/bt_connection_info.c:                     Store the Bluetooth bonding info and connection info.
    - \b src/bt/bt_gatt_service.c:                        Implement the Bluetooth GATT service.
    - \b src/bt/hci_log.c:                                Bluetooth HCI log file. The user must initiate the UART port for HCI logging.
    - \b src/mesh/mesh_app_vendor_device.c:               Bluetooth mesh CLI command handler for mesh vendor device. It is only used by ble_mesh_vendor_device project.
    - \b src/mesh/mesh_app_vendor_device_msg_handler.c:   Bluetooth mesh CLI command handler for mesh vendor device. It is only used by ble_mesh_vendor_device project.
    - \b src/mesh/mesh_app_vendor_device_pwm.c:           Bluetooth mesh CLI command handler for mesh vendor device. It is only used by ble_mesh_vendor_device project.
    - \b src/mesh/mesh_app_switch.c:                      Bluetooth mesh CLI command handler for mesh switch device. It is only used by ble_mesh_switch project.
    - \b src/mesh/mesh_app_util.c:                        Bluetooth mesh util file. 
    - \b src/mesh/mesh_app_gateway_demo.c:                WI-FI and bluetooth gateway function demo. The LinkIt 7697 HDK connects to the same WI-FI AP as the PC, 
                                                          then the user can input commands via the UART tool on the PC to control the LinkIt 7697 HDK.
                                                          It is only used by ble_mesh_switch project.
    - \b src/cli_cmds.c:                                  CLI commands of this project.
    - \b src/cli_def.c:                                   CLI initialization sequence code.
    - \b src/ept_eint_var.c:                              EINT configuration file generated by Easy
                                                          Pinmux Tool (EPT). Please do not make any 
                                                          changes to this file.
    - \b src/ept_gpio_var.c:                              GPIO configuration file generated by Easy
                                                          Pinmux Tool (EPT). Please do not make any 
                                                          changes to this file.
    - \b src/main.c:                                      Entry point of the application program.
    - \b src/sys_init.c:                                  Aggregated initialization routines.
    - \b src/system_mt7687.c:                             MT7697 system clock configuration file.
    - \b src/wifi_lwip_helper.c:                          lwIP configuration.
    - \b src/wifi_nvdm_config.c:                          Default user configuration file.
    - \b inc/bt/bt_init.h:                                Declare the Bluetooth task create API.
    - \b inc/bt/hci_log.h:                                Declare the Bluetooth HCI log API.
    - \b inc/bt/project_config.h:                         Declare the Bluetooth configuration of the memory and link number.
    - \b inc/bt/bt_gatt_service.h:                        Declare the Bluetooth GATT service API.
    - \b inc/bt/bt_connection_info.h:                     Declare the Bluetooth connection info API.
    - \b inc/bt/bt_app_callback_handler.h:                Declare the Bluetooth APP callback handler API. 
    - \b inc/mesh/mesh_app.h:                             Declare the Bluetooth mesh APP instructure.
    - \b inc/mesh/mesh_app_util.h:                        Declare the Bluetooth mesh APP util API.   
    - \b inc/cli_cmds.h:                                  Declare the reference point of CLI commands
                                                          of cli_cmds.c. To be used by cli_def.c.
    - \b inc/default_config.h:                            Default user configuration.
    - \b inc/ept_eint_drv.h:                              The EINT configuration file generated by Easy
                                                          Pinmux Tool (EPT). Please do not make any 
                                                          changes to this file.
    - \b inc/ept_gpio_drv.h:                              The GPIO configuration file generated by Easy
                                                          Pinmux Tool (EPT). Please do not make any 
                                                          changes to this file.
    - \b inc/flash_map.h:                                 MT7697 memory layout symbol file.
    - \b inc/FreeRTOSConfig.h:                            MT7697 FreeRTOS configuration file.
    - \b inc/task_def.h:                                  Define the task stack size, queue length,
                                                          project name, and priority for the
                                                          application to create tasks.
    - \b inc/hal_feature_config.h:
                                                          MT7697 HAL API feature configuration file.
    - \b inc/lwipopts.h:                                  lwIP configuration.
    - \b inc/project_config.h:                            Define the maximum number of Bluetooth
                                                          connections and timer and define the buffersize of the TX/RX, timer, and connection.
    - \b inc/sys_init.h:                                  Prototype declaration for the system configuration.
    - \b inc/task_def.h:                                  The configuration of running tasks of the
                                                          project.
    - \b inc/wifi_lwip_helper.h:                          Prototype declaration for Wi-Fi Lwip.
    - \b inc/wifi_nvdm_config.h:                          Prototype declaration for the default user
                                                          configuration file.
  - Project configuration files using GCC.
    - \b GCC/Makefile.:                                   GNU Makefile for this project.
    - \b GCC/feature.mk:                                  Generic feature options configuration file.
    - \b GCC/feature_ble_mesh_switch.mk:                  Generic feature options configuration file for BLE mesh switch project.
    - \b GCC/mt7687_flash.ld:                             Linker script.
    - \b GCC/startup_mt7687.s:                            MT7697 startup file.
    - \b GCC/syscalls.c:                                  MT7697 syscalls implementation.

@par Run the application
  - Build the example project with the command "./build.sh mt7697_hdk
    ble_mesh_vendor_device" from the SDK root folder and download the binary file to the
    LinkIt 7697 development board.
  - Reboot the HDK. The console shows the "FreeRTOS Running" message to
    indicate that the HDK is booting up.
  - Use '?' and ENTER to query the available command line options. Note that
    the command line options are still under development and subject to
    change without notice.
  - Reference applications run and the LinkIt 7697 HDK can join the Bluetooth LE mesh that is enabled nearby.
  - Example 1. Using the BLE mesh feature
    - This example creates a BLE mesh device with six mesh elements. There are five types of models that are covered in these
      elements: configuration server model; health server model; 
      generic on/off server model; vendor server model; and lighting server model.
      The structure of this vendor device is shown below:
      - It supports Relay feature, Proxy feature and Friend feature.
      - It has six elements and each element has different models, as shown below:
        - Element 0 includes three SIG models: "Configuration Server"; "Health 
          Server"; "Generic OnOff Server"; and one vendor model with ID 0x002A.
        - Element 1 includes nine SIG models: "Generic Level Server"; "Generic OnOff 
          Server"; "Generic Default Transition Time Server"; "Generic Power OnOff 
          Server"; "Generic Power OnOff Setup Server"; "Light Lightness Server"; "Light 
          Ligntness Setup Server"; "Light CTL Server"; and "Light CTL Setup Server". 
        - Element 2 includes two SIG models: "Generic OnOff Server"; and "Light CTL Temperature Server".  
        - Element 3 includes nine SIG models: "Generic Level Server"; "Generic OnOff 
          Server"; "Generic Default Transition Time Server"; "Generic Power OnOff 
          Server"; "Generic Power OnOff Setup Server"; "Light Lightness Server"; "Light 
          Ligntness Setup Server"; "Light HSL Server"; and "Light HSL Setup Server".
        - Element 5 includes two SIG models: "Generic OnOff Server"; and "Light HSL HUE Server".  
        - Element 5 includes two SIG models: "Generic OnOff Server"; and "Light HSL Saturation Server".  
      - Element 0 is addressed using the first unicast address assigned to the node during provisioning. 
        Other elements are addressed using the subsequent addresses. For example, if the unicast of the node is 0004, 
        element 0 is addressed 0004, and element 1 is addressed 0005.  
    - The mesh feature is enabled when Bluetooth successfully powers on.
      A similar log is written to the output:
      \code
      [T: xxxx M: mesh_app C: info F: bt_mesh_app_event_callback L: 272]: BLE_MESH_EVT_INIT_DONE
      \endcode
    - When this device is successfully added to a mesh network.
      A similar log is written to the output:
      \code
      [T: xxxx M: mesh_app C: info F: bt_mesh_app_event_callback L: 287]: BLE_MESH_EVT_PROV_DONE
      \endcode
    - If the device receives a message from a different device using the same mesh network and application key, 
      a similar log is written to the output:
      \code
      Receive a SIG message in which xxxx is the SIG message id.
      [Mesh]rece SigMsg[xxxx]
      If the SIG message is to turn on/off LED on this device.
      [Mesh]led is on or [Mesh]led is off
      Receive a vendor message in which xx is the company id and xxxx is the vendor message id.
      [Mesh]rece VenrodMsg[xx:xxxx]      
      \endcode
    - To delete the mesh data, use the command "mesh delete". It takes effect after rebooting again.

*/
/**
 * @}
 * @}
 * @}
 */
