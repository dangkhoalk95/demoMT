/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef CONFIG_CTRL_IFACE
#ifndef CONFIG_CTRL_IFACE_UDP
#endif /* CONFIG_CTRL_IFACE_UDP */
#endif /* CONFIG_CTRL_IFACE */

#include "config_ssid.h"

typedef struct wpa_config {
    struct wpa_ssid *ssid; /* global network list */
    struct wpa_ssid *pssid[SUPC_MAX_SSID_CONFIG_PRIORITY]; /* per priority network lists (in priority
                  * order) */
    /* J: define SUPC_MAX_SSID_CONFIG_PRIORITY instead */
    int eapol_version;
    int fast_reauth;
#if 1 //#ifdef __WPS_SUPPORT__
    /**
     * uuid - Universally Unique IDentifier (UUID; see RFC 4122) for WPS
     */
    u8 uuid[16];

    /**
     * device_name - Device Name (WPS)
     * User-friendly description of device; up to 32 octets encoded in
     * UTF-8
     */
    char *device_name;

    /**
     * manufacturer - Manufacturer (WPS)
     * The manufacturer of the device (up to 64 ASCII characters)
     */
    char *manufacturer;

    /**
     * model_name - Model Name (WPS)
     * Model of the device (up to 32 ASCII characters)
     */
    char *model_name;

    /**
     * model_number - Model Number (WPS)
     * Additional device description (up to 32 ASCII characters)
     */
    char *model_number;

    /**
     * serial_number - Serial Number (WPS)
     * Serial number of the device (up to 32 characters)
     */
    char *serial_number;

    /**
     * device_type - Primary Device Type (WPS)
     * Used format: categ-OUI-subcateg
     * categ = Category as an integer value
     * OUI = OUI and type octet as a 4-octet hex-encoded value;
     *  0050F204 for default WPS OUI
     * subcateg = OUI-specific Sub Category as an integer value
     * Examples:
     *   1-0050F204-1 (Computer / PC)
     *   1-0050F204-2 (Computer / Server)
     *   5-0050F204-1 (Storage / NAS)
     *   6-0050F204-1 (Network Infrastructure / AP)
     */
    char *device_type;
    /**
     * country - Country code
     *
     * This is the ISO/IEC alpha2 country code for which we are operating
     * in
     */
    char country[2];

    /**
     * wps_cred_processing - Credential processing
     *
     *   0 = process received credentials internally
     *   1 = do not process received credentials; just pass them over
     *  ctrl_iface to external program(s)
     *   2 = process received credentials internally and pass them over
     *  ctrl_iface to external program(s)
     */
    int wps_cred_processing;
#endif
} wpa_config;


struct wpa_config *wpa_config_read(const char *config_file);
void wpa_config_free(struct wpa_config *ssid);

#endif /* CONFIG_H */
