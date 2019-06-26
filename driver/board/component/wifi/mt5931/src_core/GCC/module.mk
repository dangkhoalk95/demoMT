# Copyright Statement:
#
# (C) 2005-2016  MediaTek Inc. All rights reserved.
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
# Without the prior written permission of MediaTek and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
# if you have agreed to and been bound by the applicable license agreement with
# MediaTek ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of MediaTek Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#

MT5931_DRIVER_SRC_CORE = driver/board/component/wifi/mt5931/src_core
MT5931_DRIVER_FILES = $(MT5931_DRIVER_SRC_CORE)/release_mt5931/wndrv_api.c \
                      $(MT5931_DRIVER_SRC_CORE)/release_mt5931/wndrv_kal.c\
                      $(MT5931_DRIVER_SRC_CORE)/common/smart_conn_decoder.c\
                      $(MT5931_DRIVER_SRC_CORE)/common/wlan_bow.c\
                      $(MT5931_DRIVER_SRC_CORE)/common/wlan_lib.c\
                      $(MT5931_DRIVER_SRC_CORE)/common/wlan_oid.c\
                      $(MT5931_DRIVER_SRC_CORE)/common/wndrv_crypt_aes.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/auth.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/bss.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/cnm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/cnm_mem.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/cnm_timer.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/hem_mbox.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/mib.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/privacy.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/rate.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/rlm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/rlm_domain.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/rlm_obss.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/roaming_fsm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/rsn.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/saa_fsm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/scan.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/scan_fsm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/sec_fsm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/sms4.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/swcr.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/wapi.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/aaa_fsm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/ais_fsm.c\
                      $(MT5931_DRIVER_SRC_CORE)/mgmt/assoc.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/cmd_buf.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/nic.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/nic_cmd_event.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/nic_pwr_mgt.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/nic_rx.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/nic_tx.c\
                      $(MT5931_DRIVER_SRC_CORE)/nic/que_mgt.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/gl_hpi.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/gl_kal.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/gl_rftest.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/loopback.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/mt5931_firmware.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wlan_kal.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wlan_wt.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_create.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_hostap_func.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_inject_msg.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_main.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_misc.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_rftest_func.c\
                      $(MT5931_DRIVER_SRC_CORE)/os/wndrv_supc_func.c\

C_FILES += $(MT5931_DRIVER_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(MT5931_DRIVER_SRC_CORE)/../inc
CFLAGS += -I$(SOURCE_DIR)/$(MT5931_DRIVER_SRC_CORE)/../inc/mgmt
CFLAGS += -I$(SOURCE_DIR)/$(MT5931_DRIVER_SRC_CORE)/../inc/nic
CFLAGS += -I$(SOURCE_DIR)/$(MT5931_DRIVER_SRC_CORE)/../inc/os
CFLAGS += -I$(SOURCE_DIR)/$(MT5931_DRIVER_SRC_CORE)/../inc/release_mt5931
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/wifi/mt5931/inc
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -Iinc

include $(SOURCE_DIR)/$(MT5931_DRIVER_SRC_CORE)/GCC/cflags.mk
