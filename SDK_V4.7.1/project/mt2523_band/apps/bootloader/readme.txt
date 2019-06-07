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
 * @addtogroup mt2523_band mt2523_band
 * @{
 * @addtogroup mt2523_band_apps apps
 * @{
 * @addtogroup mt2523_band_apps_bootloader bootloader
 * @{

@par Overview
  - This project is a bootloader for a wristband based on MediaTek MT2523 and
    MT2511 SOC. The developer can refer this project to understand how to build
    a bootloader.

@par Hardware and software environment
  - Please refer to bioband_ref_design project.

@par Directory contents
  - Source and header files
    - \b \<sdk_root\>/driver/board/mt25x3_hdk/bootloader/core/src/bl_main.c:
                                  Main program
    - \b inc/hal_feature_config.h:
                                  MT2523x's feature configuration file.
    - \b MDK-ARM/hal_log.h:       Delcalre HAL log function in bootloader for
                                  Keil.
    - \b src/custom_fota.c:       Fota update function customization.
    - \b GCC/startup_bootloader.s:
                                  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:          The minimal implementation of the system
                                  calls.
    - \b MDK-ARM/startup_bootloader.s:
                                  MT2523x startup file for Keil IDE.
    - \b EWARM/startup_mt2523.s:  MT2523x startup file for IAR IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:     Feature configuration.
    - \b GCC/Makefile:       Makefile.
    - \b GCC/bootloader.ld:  Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/bootloader.uvprojx:
                           uVision5 project File. Contains the project
                           structure in XML format.
    - \b MDK-ARM/bootloader.uvoptx:
                           uVision5 project options. Contains the settings for
                           the debugger, trace configuration, breakpoints,
                           currently open files, etc.
    - \b MDK-ARM/ram.sct:  Scatter file.
  - Project configuration files using IAR IDE
    - \b EWARM/bootloader.ewd:     IAR project options. Contains the settings
                                   for the debugger.
    - \b EWARM/bootloader.ewp:     IAR project file. Contains the project
                                   structure in XML format
    - \b EWARM/bootloader.eww:     IAR workspace file. Contains project
                                   information.
    - \b EWARM/mtk2523_flash.icf:  Linker script.

@par Run the example
  - Connect the wristband to the PC with serial port cable.
  - Build bootloader and bioband_ref_design project with the command,
    "./build.sh mt2523_band bioband_ref_design bl" under the SDK root folder
    and download the binary file to wristband by using Flash Tool.
  - Press power key o power on the wristband.
 * @}
 * @}
 * @}
*/
