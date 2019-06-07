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


#ifndef __BT_MESH_MODEL_LIGHTING_OPCODES_H__
#define __BT_MESH_MODEL_LIGHTING_OPCODES_H__

/*!
     @defgroup PAGE_MESH_MODEL_LIGHTING_OPCODES bt_mesh_model_lighting_opcodes.h
     @{
        @page PAGE_MESH_MODEL_LIGHTING_OPCODES bt_mesh_model_lighting_opcodes.h
        bt_mesh_model_lighting_opcodes.h defines the SIG Mesh Lighting Model operation codes.
*/


/*!
     @name Lighting lightness model message opcode.
     @brief Opcode for lighting lightness models.
     @{
 */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_GET	                             0x824B /**< opcode for Light Lightness Get */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_SET                               0x824C /**< opcode for Light Lightness Set */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED                0x824D /**< opcode for Light Lightness Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_STATUS                            0x824E /**< opcode for Light Lightness Status */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_GET                        0x824F /**< opcode for Light Lightness Linear Get */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_SET                        0x8250 /**< opcode for Light Lightness Linear Set */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED         0x8251 /**< opcode for Light Lightness Linear Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_STATUS                     0x8252 /**< opcode for Light Lightness Linear Status */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LAST_GET                          0x8253 /**< opcode for Light Lightness Last Get */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LAST_STATUS                       0x8254 /**< opcode for Light Lightness Last Status */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_GET                       0x8255 /**< opcode for Light Lightness Default Get */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_STATUS                    0x8256 /**< opcode for Light Lightness Default Status */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_GET                         0x8257 /**< opcode for Light Lightness Range Get */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_STATUS                      0x8258 /**< opcode for Light Lightness Range Status */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_SET                       0x8259 /**< opcode for Light Lightness Default Set */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_SET_UNACKNOWLEDGED        0x825A /**< opcode for Light Lightness Default Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_SET                         0x825B /**< opcode for Light Lightness Range Set */
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_SET_UNACKNOWLEDGED          0x825C /**< opcode for Light Lightness Range Set Unacknowledged*/
/*!  @} */

/*!
     @name Lighting CTL model message opcode.
     @brief Opcode for lighting CTL models.
     @{
 */
#define BT_MESH_MODEL_LIGHT_CTL_GET                                     0x825D /**< opcode for Light CTL Get */
#define BT_MESH_MODEL_LIGHT_CTL_SET                                     0x825E /**< opcode for Light CTL Set */
#define BT_MESH_MODEL_LIGHT_CTL_SET_UNACKNOWLEDGED                      0x825F /**< opcode for Light CTL Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_CTL_STATUS                                  0x8260 /**< opcode for Light CTL Status */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_GET                         0x8261 /**< opcode for Light CTL Temperature Get */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_GET                   0x8262 /**< opcode for Light CTL Temperature Range Get */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_STATUS                0x8263 /**< opcode for Light CTL Temperature Range Status */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SET                         0x8264 /**< opcode for Light CTL Temperature Set */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED          0x8265 /**< opcode for Light CTL Temperature Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_STATUS                      0x8266 /**< opcode for Light CTL Temperature Status */
#define BT_MESH_MODEL_LIGHT_CTL_DEFAULT_GET                             0x8267 /**< opcode for Light CTL Default Get */
#define BT_MESH_MODEL_LIGHT_CTL_DEFAULT_STATUS                          0x8268 /**< opcode for Light CTL Default Status */
#define BT_MESH_MODEL_LIGHT_CTL_DEFAULT_SET                             0x8269 /**< opcode for Light CTL Default Set */
#define BT_MESH_MODEL_LIGHT_CTL_DEFAULT_SET_UNACKNOWLEDGED              0x826A /**< opcode for Light CTL Default Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_SET                   0x826B /**< opcode for Light CTL Temperature Range Set */
#define BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED    0x826C /**< opcode for Light CTL Temperature Range Set Unacknowledged */
/*!  @} */

/*!
     @name Lighting HSL model message opcode.
     @brief Opcode for lighting HSL models.
     @{
 */
#define BT_MESH_MODEL_LIGHT_HSL_GET                                     0x826D /**< opcode for Light HSL Get */
#define BT_MESH_MODEL_LIGHT_HSL_HUE_GET                                 0x826E /**< opcode for Light HSL Hue Get */
#define BT_MESH_MODEL_LIGHT_HSL_HUE_SET                                 0x826F /**< opcode for Light HSL Hue Set */
#define BT_MESH_MODEL_LIGHT_HSL_HUE_SET_UNACKNOWLEDGED                  0x8270 /**< opcode for Light HSL Hue Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_HSL_HUE_STATUS                              0x8271 /**< opcode for Light HSL Hue Status */
#define BT_MESH_MODEL_LIGHT_HSL_SATURATION_GET                          0x8272 /**< opcode for Light HSL Saturation Get */
#define BT_MESH_MODEL_LIGHT_HSL_SATURATION_SET                          0x8273 /**< opcode for Light HSL Saturation Set */
#define BT_MESH_MODEL_LIGHT_HSL_SATURATION_SET_UNACKNOWLEDGED           0x8274 /**< opcode for Light HSL Saturation Set Unacknowledged*/
#define BT_MESH_MODEL_LIGHT_HSL_SATURATION_STATUS                       0x8275 /**< opcode for Light HSL Saturation Status */
#define BT_MESH_MODEL_LIGHT_HSL_SET                                     0x8276 /**< opcode for Light HSL Set */
#define BT_MESH_MODEL_LIGHT_HSL_SET_UNACKNOWLEDGED                      0x8277 /**< opcode for Light HSL Set Unacknowledged*/
#define BT_MESH_MODEL_LIGHT_HSL_STATUS                                  0x8278 /**< opcode for Light HSL Status */
#define BT_MESH_MODEL_LIGHT_HSL_TARGET_GET                              0x8279 /**< opcode for Light HSL Target Get */
#define BT_MESH_MODEL_LIGHT_HSL_TARGET_STATUS                           0x827A /**< opcode for Light HSL Target Status */
#define BT_MESH_MODEL_LIGHT_HSL_DEFAULT_GET                             0x827B /**< opcode for Light HSL Default Get */
#define BT_MESH_MODEL_LIGHT_HSL_DEFAULT_STATUS                          0x827C /**< opcode for Light HSL Default Status */
#define BT_MESH_MODEL_LIGHT_HSL_RANGE_GET                               0x827D /**< opcode for Light HSL Get */
#define BT_MESH_MODEL_LIGHT_HSL_RANGE_STATUS                            0x827E /**< opcode for Light HSL Status */
#define BT_MESH_MODEL_LIGHT_HSL_DEFAULT_SET                             0x827F /**< opcode for Light HSL Set */
#define BT_MESH_MODEL_LIGHT_HSL_DEFAULT_SET_UNACKNOWLEDGED              0x8280 /**< opcode for Light HSL Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_HSL_RANGE_SET                               0x8281 /**< opcode for Light HSL Set */
#define BT_MESH_MODEL_LIGHT_HSL_RANGE_SET_UNACKNOWLEDGED                0x8282 /**< opcode for Light HSL Set Unacknowledged */
/*!  @} */

/*!
     @name Lighting xyL model message opcode.
     @brief Opcode for lighting xyL models.
     @{
 */
#define BT_MESH_MODEL_LIGHT_XYL_GET                                     0x8283 /**< opcode for Light xyL Get */
#define BT_MESH_MODEL_LIGHT_XYL_SET                                     0x8284 /**< opcode for Light xyL Set */
#define BT_MESH_MODEL_LIGHT_XYL_SET_UNACKNOWLEDGED                      0x8285 /**< opcode for Light xyL Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_XYL_STATUS                                  0x8286 /**< opcode for Light xyL Status */
#define BT_MESH_MODEL_LIGHT_XYL_TARGET_GET                              0x8287 /**< opcode for Light xyL Target Get */
#define BT_MESH_MODEL_LIGHT_XYL_TARGET_STATUS                           0x8288 /**< opcode for Light xyL Target Status */
#define BT_MESH_MODEL_LIGHT_XYL_DEFAULT_GET                             0x8289 /**< opcode for Light xyL Default Get */
#define BT_MESH_MODEL_LIGHT_XYL_DEFAULT_STATUS                          0x828A /**< opcode for Light xyL Default Status */
#define BT_MESH_MODEL_LIGHT_XYL_RANGE_GET                               0x828B /**< opcode for Light xyL Range Get */
#define BT_MESH_MODEL_LIGHT_XYL_RANGE_STATUS                            0x828C /**< opcode for Light xyL Range Status */
#define BT_MESH_MODEL_LIGHT_XYL_DEFAULT_SET                             0x828D /**< opcode for Light xyL Default Set */
#define BT_MESH_MODEL_LIGHT_XYL_DEFAULT_SET_UNACKNOWLEDGED              0x828E /**< opcode for Light xyL Default Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_XYL_RANGE_SET                               0x828F /**< opcode for Light xyL Range Set */
#define BT_MESH_MODEL_LIGHT_XYL_RANGE_SET_UNACKNOWLEDGED                0x8290 /**< opcode for Light xyL Range Set Unacknowledged */
/*!  @} */

/*!
     @name Lighting LC model message opcode.
     @brief Opcode for lighting LC models.
     @{
 */
#define BT_MESH_MODEL_LIGHT_LC_MODE_GET                                 0x8291 /**< opcode for Light LC Mode Get */
#define BT_MESH_MODEL_LIGHT_LC_MODE_SET                                 0x8292 /**< opcode for Light LC Mode Set */
#define BT_MESH_MODEL_LIGHT_LC_MODE_SET_UNACKNOWLEDGED                  0x8293 /**< opcode for Light LC Mode Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LC_MODE_STATUS                              0x8294 /**< opcode for Light LC Mode Status */
#define BT_MESH_MODEL_LIGHT_LC_OM_GET                                   0x8295 /**< opcode for Light LC OM Get */
#define BT_MESH_MODEL_LIGHT_LC_OM_SET                                   0x8296 /**< opcode for Light LC OM Set */
#define BT_MESH_MODEL_LIGHT_LC_OM_SET_UNACKNOWLEDGED                    0x8297 /**< opcode for Light LC OM Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LC_OM_STATUS                                0x8298 /**< opcode for Light LC OM Status */
#define BT_MESH_MODEL_LIGHT_LC_LIGHT_ONOFF_GET                          0x8299 /**< opcode for Light LC Light OnOff Get */
#define BT_MESH_MODEL_LIGHT_LC_LIGHT_ONOFF_SET                          0x829A /**< opcode for Light LC Light OnOff Set */
#define BT_MESH_MODEL_LIGHT_LC_LIGHT_ONOFF_SET_UNACKNOWLEDGED           0x829B /**< opcode for Light LC Light OnOff Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LC_LIGHT_ONOFF_STATUS                       0x829C /**< opcode for Light LC Light OnOff Status */
#define BT_MESH_MODEL_LIGHT_LC_PROPERTY_GET                             0x829D /**< opcode for Light LC Property Get */
#define BT_MESH_MODEL_LIGHT_LC_PROPERTY_SET                             0x62   /**< opcode for Light LC Property Set */
#define BT_MESH_MODEL_LIGHT_LC_PROPERTY_SET_UNACKNOWLEDGED              0x63   /**< opcode for Light LC Property Set Unacknowledged */
#define BT_MESH_MODEL_LIGHT_LC_PROPERTY_STATUS                          0x64   /**< opcode for Light LC Property Status */
/*!  @} */

/*!
@}
*/

#endif // __BT_MESH_MODEL_LIGHTING_OPCODES_H__

