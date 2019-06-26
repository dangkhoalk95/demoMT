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

MINORSUPC_SRC_CORE := middleware/MTK/minorsupc/src_core

C_FILES  += 	$(MINORSUPC_SRC_CORE)/aes/aes_engine.c\
                $(MINORSUPC_SRC_CORE)/aes/aes_wrap.c\
                $(MINORSUPC_SRC_CORE)/dhm/bignum.c\
                $(MINORSUPC_SRC_CORE)/dhm/dhm.c\
                $(MINORSUPC_SRC_CORE)/che_api.c\
                $(MINORSUPC_SRC_CORE)/common.c\
                $(MINORSUPC_SRC_CORE)/crypto.c\
                $(MINORSUPC_SRC_CORE)/driver_mtk.c\
                $(MINORSUPC_SRC_CORE)/drivers.c\
                $(MINORSUPC_SRC_CORE)/l2_packet.c\
                $(MINORSUPC_SRC_CORE)/md4.c\
                $(MINORSUPC_SRC_CORE)/md5.c\
                $(MINORSUPC_SRC_CORE)/ms_funcs.c\
                $(MINORSUPC_SRC_CORE)/rc4.c\
                $(MINORSUPC_SRC_CORE)/sha1.c\
                $(MINORSUPC_SRC_CORE)/sha256.c\
                $(MINORSUPC_SRC_CORE)/supc_adpt_functions.c\
                $(MINORSUPC_SRC_CORE)/supc_create.c\
                $(MINORSUPC_SRC_CORE)/supc_init.c\
                $(MINORSUPC_SRC_CORE)/supc_main.c\
                $(MINORSUPC_SRC_CORE)/supc_wpa_common.c\
                $(MINORSUPC_SRC_CORE)/supc_wpa_functions.c\
                $(MINORSUPC_SRC_CORE)/wpa.c\
                $(MINORSUPC_SRC_CORE)/wpabuf.c\
                $(MINORSUPC_SRC_CORE)/wpa_supplicant.c\
                $(MINORSUPC_SRC_CORE)/eap/milenage_dummy.c
                #$(MINORSUPC_SRC_CORE)/eap/eap.c\
                #$(MINORSUPC_SRC_CORE)/eap/eapol_sm.c\
                #$(MINORSUPC_SRC_CORE)/eap/eap_common.c\
                #$(MINORSUPC_SRC_CORE)/eap/eap_wsc.c\
                #$(MINORSUPC_SRC_CORE)/eap/supc_base64.c\
                #$(MINORSUPC_SRC_CORE)/eap/uuid.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_attr_build.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_attr_parse.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_attr_process.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_common.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_dev_attr.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_enrollee.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_registrar.c\
                #$(MINORSUPC_SRC_CORE)/eap/wps_supplicant.c\
                #$(MINORSUPC_SRC_CORE)/eap/dh_groups.c\
		 

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minorsupc/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minorsupc/inc/aes
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minorsupc/inc/dhm
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 
CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/wifi/mt5931/inc
CFLAGS 	+= -Iinc

include $(SOURCE_DIR)/${MINORSUPC_SRC_CORE}/GCC/cflags.mk
