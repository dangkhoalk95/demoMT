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


#ifndef __BT_MESH_MODEL_GENERIC_OPCODES_H__
#define __BT_MESH_MODEL_GENERIC_OPCODES_H__

/*!
     @defgroup PAGE_MESH_MODEL_GENERIC_OPCODES bt_mesh_model_generic_opcodes.h
     @{
        @page PAGE_MESH_MODEL_GENERIC_OPCODES bt_mesh_model_generic_opcodes.h
        bt_mesh_model_generic_opcodes.h defines the SIG Mesh Generic Model operation codes.
*/

/*!
     @name Generic OnOff model message opcode.
     @brief Opcode for generic onoff model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_ONOFF_GET	                                   0x8201
#define BT_MESH_MODEL_GENERIC_ONOFF_SET                                       0x8202
#define BT_MESH_MODEL_GENERIC_ONOFF_SET_UNACKNOWLEDGED                        0x8203
#define BT_MESH_MODEL_GENERIC_ONOFF_STATUS                                    0x8204
/*!  @} */

/*!
     @name Generic Level model message opcode.
     @brief Opcode for generic level model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_LEVEL_GET                                       0x8205
#define BT_MESH_MODEL_GENERIC_LEVEL_SET                                       0x8206
#define BT_MESH_MODEL_GENERIC_LEVEL_SET_UNACKNOWLEDGED                        0x8207
#define BT_MESH_MODEL_GENERIC_LEVEL_STATUS                                    0x8208
#define BT_MESH_MODEL_GENERIC_DELTA_SET                                       0x8209
#define BT_MESH_MODEL_GENERIC_DELTA_SET_UNACKNOWLEDGED                        0x820A
#define BT_MESH_MODEL_GENERIC_MOVE_SET                                        0x820B
#define BT_MESH_MODEL_GENERIC_MOVE_SET_UNACKNOWLEDGED                         0x820C

/*!  @} */

/*!
     @name Generic default transition time model message opcode.
     @brief Opcode for generic default transition time model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_GET                     0x820D
#define BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SET                     0x820E
#define BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACKNOWLEDGED      0x820F
#define BT_MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_STATUS                  0x8210
/*!  @} */

/*!
     @name Generic OnPowerUp model message opcode.
     @brief Opcode for generic on power up model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_ONPOWERUP_GET                                   0x8211
#define BT_MESH_MODEL_GENERIC_ONPOWERUP_STATUS                                0x8212
#define BT_MESH_MODEL_GENERIC_ONPOWERUP_SET                                   0x8213
#define BT_MESH_MODEL_GENERIC_ONPOWERUP_SET_UNACKNOWLEDGED                    0x8214
/*!  @} */

/*!
     @name Generic power model message opcode.
     @brief Opcode for generic power model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_POWER_LEVEL_GET                                 0x8215
#define BT_MESH_MODEL_GENERIC_POWER_LEVEL_SET                                 0x8216
#define BT_MESH_MODEL_GENERIC_POWER_LEVEL_SET_UNACKNOWLEDGED                  0x8217
#define BT_MESH_MODEL_GENERIC_POWER_LEVEL_STATUS                              0x8218
#define BT_MESH_MODEL_GENERIC_POWER_LAST_GET                                  0x8219
#define BT_MESH_MODEL_GENERIC_POWER_LAST_STATUS                               0x821A
#define BT_MESH_MODEL_GENERIC_POWER_DEFAULT_GET                               0x821B
#define BT_MESH_MODEL_GENERIC_POWER_DEFAULT_STATUS                            0x821C
#define BT_MESH_MODEL_GENERIC_POWER_RANGE_GET                                 0x821D
#define BT_MESH_MODEL_GENERIC_POWER_RANGE_STATUS                              0x821E
#define BT_MESH_MODEL_GENERIC_POWER_DEFAULT_SET                               0x821F
#define BT_MESH_MODEL_GENERIC_POWER_DEFAULT_SET_UNACKNOWLEDGED                0x8220
#define BT_MESH_MODEL_GENERIC_POWER_RANGE_SET                                 0x8221
#define BT_MESH_MODEL_GENERIC_POWER_RANGE_SET_UNACKNOWLEDGED                  0x8222
/*!  @} */

/*!
     @name Generic battery model message opcode.
     @brief Opcode for generic battery model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_BATTERY_GET                                     0x8223
#define BT_MESH_MODEL_GENERIC_BATTERY_STATUS                                  0x8224
/*!  @} */

/*!
     @name Generic location model message opcode.
     @brief Opcode for generic location model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_LOCATION_GLOBAL_STATUS                          0x40
#define BT_MESH_MODEL_GENERIC_LOCATION_GLOBAL_SET                             0x41
#define BT_MESH_MODEL_GENERIC_LOCATION_GLOBAL_SET_UNACKNOWLEDGED              0x42
#define BT_MESH_MODEL_GENERIC_LOCATION_GLOBAL_GET                             0x8225
#define BT_MESH_MODEL_GENERIC_LOCATION_LOCAL_GET                              0x8226
#define BT_MESH_MODEL_GENERIC_LOCATION_LOCAL_STATUS                           0x8227
#define BT_MESH_MODEL_GENERIC_LOCATION_LOCAL_SET                              0x8228
#define BT_MESH_MODEL_GENERIC_LOCATION_LOCAL_SET_UNACKNOWLEDGED               0x8229
/*!  @} */

/*!
     @name Generic property model message opcode.
     @brief Opcode for generic property model.
     @{
 */
#define BT_MESH_MODEL_GENERIC_ADMIN_PROPERTY_SET                              0x48
#define BT_MESH_MODEL_GENERIC_ADMIN_PROPERTY_SET_UNACKNOWLEDGED               0x49
#define BT_MESH_MODEL_GENERIC_ADMIN_PROPERTY_STATUS                           0x4A
#define BT_MESH_MODEL_GENERIC_USER_PROPERTIES_STATUS                          0x4B
#define BT_MESH_MODEL_GENERIC_MANUFACTURER_PROPERTIES_STATUS                  0x43
#define BT_MESH_MODEL_GENERIC_MANUFACTURER_PROPERTY_SET                       0x44
#define BT_MESH_MODEL_GENERIC_MANUFACTURER_PROPERTY_SET_UNACKNOWLEDGED        0x45
#define BT_MESH_MODEL_GENERIC_MANUFACTURER_PROPERTY_STATUS                    0x46
#define BT_MESH_MODEL_GENERIC_ADMIN_PROPERTIES_STATUS                         0x47
#define BT_MESH_MODEL_GENERIC_USER_PROPERTY_SET                               0x4C
#define BT_MESH_MODEL_GENERIC_USER_PROPERTY_SET_UNACKNOWLEDGED                0x4D
#define BT_MESH_MODEL_GENERIC_USER_PROPERTY_STATUS                            0x4E
#define BT_MESH_MODEL_GENERIC_CLIENT_PROPERTIES_GET                           0x4F
#define BT_MESH_MODEL_GENERIC_CLIENT_PROPERTIES_STATUS                        0x50
#define BT_MESH_MODEL_GENERIC_MANUFACTURER_PROPERTIES_GET                     0x822A
#define BT_MESH_MODEL_GENERIC_MANUFACTURER_PROPERTY_GET                       0x822B
#define BT_MESH_MODEL_GENERIC_ADMIN_PROPERTIES_GET                            0x822C
#define BT_MESH_MODEL_GENERIC_ADMIN_PROPERTY_GET                              0x822D
#define BT_MESH_MODEL_GENERIC_USER_PROPERTIES_GET                             0x822E
#define BT_MESH_MODEL_GENERIC_USER_PROPERTY_GET                               0x822F
/*!  @} */

/*!
     @brief Generic OnPowerUp state
 */
typedef enum {
    GENERIC_ON_POWERUP_OFF = 0,        /**< Off. After being powered up, the element is in an off state. */
    GENERIC_ON_POWERUP_DEFAULT = 1,    /**< After being powered up, the element is in an On state and uses default state values. */
    GENERIC_ON_POWERUP_RESTORE = 2,    /**< If a transition was in progress when powered down, the element restores the target state when powered up. Otherwise the element restores the state it was in when powered down. */
} bt_mesh_model_generic_on_powerup_t;

/*!
@}
*/

#endif // __BT_MESH_MODEL_GENERIC_OPCODES_H__

