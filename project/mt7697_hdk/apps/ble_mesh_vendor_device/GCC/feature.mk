# Copyright Statement:                                                                                               
#                                                                                                                    
# (C) 2017  Airoha Technology Corp. All rights reserved.                                                             
#                                                                                                                    
# This software/firmware and related documentation ("Airoha Software") are                                           
# protected under relevant copyright laws. The information contained herein                                          
# is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.                        
# Without the prior written permission of Airoha and/or its licensors,                                               
# any reproduction, modification, use or disclosure of Airoha Software,                                              
# and information contained herein, in whole or in part, shall be strictly prohibited.                               
# You may only use, reproduce, modify, or distribute (as applicable) Airoha Software                                 
# if you have agreed to and been bound by the applicable license agreement with                                      
# Airoha ("License Agreement") and been granted explicit permission to do so within                                  
# the License Agreement ("Permitted User").  If you are not a Permitted User,                                        
# please cease any access or use of Airoha Software immediately.                                                     
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES                                        
# THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES                                               
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL                          
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF                             
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.                                              
# NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE                                            
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR                                              
# SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH                                            
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES                               
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES                       
# CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA                                  
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR                                   
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND                               
# CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,                                   
# AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,                                                 
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO                                          
# AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.                                                                          
#                                                                                                                    

IC_CONFIG                           = mt7697
BOARD_CONFIG                        = mt7697_hdk
MTK_HAL_LOWPOWER_ENABLE             = y
MTK_BSPEXT_ENABLE                   = y
MTK_IPERF_ENABLE                    = y
MTK_PING_OUT_ENABLE                 = y
MTK_MINISUPP_ENABLE                 = y
MTK_WIFI_TGN_VERIFY_ENABLE          = n
MTK_MINICLI_ENABLE                  = y
MTK_CLI_TEST_MODE_ENABLE            = y
MTK_CLI_EXAMPLE_MODE_ENABLE         = n
MTK_SMTCN_V5_ENABLE                    = y
MTK_BLE_SMTCN_ENABLE                = n
MTK_WIFI_WPS_ENABLE                 = n
MTK_WIFI_DIRECT_ENABLE              = n
MTK_WIFI_REPEATER_ENABLE            = y
MTK_WIFI_CONFIGURE_FREE_ENABLE      = n
MTK_WIFI_PROFILE_ENABLE             = y
MTK_WIFI_PRIVILEGE_ENABLE           = y
MTK_SINGLE_SKU_SUPPORT              = y
MTK_WIFI_AT_COMMAND_ENABLE          = n
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-homekit.h
MTK_FOTA_ENABLE                     = y
MTK_FOTA_CLI_ENABLE                 = y
MTK_LED_ENABLE                      = y
MTK_AT_CMD_DISABLE                  = y
MTK_ATCI_ENABLE                     = n
# debug level: none, error, warning, and info
MTK_DEBUG_LEVEL                     = info
MTK_CLI_FORK_ENABLE                 = y

# System service debug feature for internal use
MTK_SUPPORT_HEAP_DEBUG              = n
MTK_HEAP_SIZE_GUARD_ENABLE          = n
MTK_OS_CPU_UTILIZATION_ENABLE       = n
MTK_MULTI_SMART_CONFIG_ENABLE       = n
MTK_ALINK_ENABLE                    = n
MTK_HTTPCLIENT_SSL_ENABLE           = y
MTK_ANT_DIV_ENABLE                  = n

MTK_HCI_CONSOLE_MIX_ENABLE          = y
MTK_BLE_BQB_TEST_ENABLE             = n
MTK_BLE_CLI_ENABLE                  = n
MTK_BLE_BQB_CLI_ENABLE              = n
MTK_NVDM_ENABLE                     = y

# bt module enable
MTK_BT_ENABLE                       = y
MTK_BLE_ONLY_ENABLE                 = y
MTK_BT_HFP_ENABLE                   = n
MTK_BT_AVRCP_ENABLE                 = n
MTK_BT_AVRCP_ENH_ENABLE             = n
MTK_BT_A2DP_ENABLE                  = n
MTK_BT_PBAP_ENABLE                  = n
MTK_BT_SPP_ENABLE                   = n
MTK_BT_MESH_ENABLE                  = y
# mesh example project: none, pts_device, test, pts_provisioner, switch, vendor_device, pts_lighting_client, pts_lighting_server
MTK_BT_MESH_EXAMPLE_PROJECT         = vendor_device
MTK_BT_MESH_CLI_ENABLE              = y
