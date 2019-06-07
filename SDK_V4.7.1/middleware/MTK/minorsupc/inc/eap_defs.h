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


#ifndef EAP_DEFS_H
#define EAP_DEFS_H

/* RFC 3748 - Extensible Authentication Protocol (EAP) */

typedef struct eap_hdr {
    u8 code;
    u8 identifier;
    u16 length; /* including code and identifier */
    /* followed by length-4 octets of data */
} eap_hdr;

enum { EAP_CODE_REQUEST = 1, EAP_CODE_RESPONSE = 2, EAP_CODE_SUCCESS = 3,
       EAP_CODE_FAILURE = 4
     };

/* EAP Request and Response data begins with one octet Type. Success and
 * Failure do not have additional data. */

typedef enum {
    EAP_TYPE_NONE = 0,
    EAP_TYPE_IDENTITY = 1,
    EAP_TYPE_NOTIFICATION = 2,
    EAP_TYPE_NAK = 3 /* Response only */,
    EAP_TYPE_MD5 = 4,
    EAP_TYPE_OTP = 5 /* RFC 2284 */,
    EAP_TYPE_GTC = 6, /* RFC 2284 */
    EAP_TYPE_TLS = 13 /* RFC 2716 */,
    EAP_TYPE_LEAP = 17 /* Cisco proprietary */,
    EAP_TYPE_SIM = 18 /* draft-haverinen-pppext-eap-sim-12.txt */,
    EAP_TYPE_TTLS = 21 /* draft-ietf-pppext-eap-ttls-02.txt */,
    EAP_TYPE_AKA = 23 /* draft-arkko-pppext-eap-aka-12.txt */,
    EAP_TYPE_PEAP = 25 /* draft-josefsson-pppext-eap-tls-eap-06.txt */,
    EAP_TYPE_MSCHAPV2 = 26 /* draft-kamath-pppext-eap-mschapv2-00.txt */,
    EAP_TYPE_TLV = 33 /* draft-josefsson-pppext-eap-tls-eap-07.txt */,
    EAP_TYPE_FAST = 43 /* draft-cam-winget-eap-fast-00.txt */,
    EAP_TYPE_EXPANDED = 254 /* RFC 3748 */,
    EAP_TYPE_PSK = 255 /* EXPERIMENTAL - type not yet allocated
                * draft-bersani-eap-psk-03 */
} EapType;

/* SMI Network Management Private Enterprise Code for vendor specific types */
enum {
    EAP_VENDOR_IETF = 0,
    EAP_VENDOR_MICROSOFT = 0x000137 /* Microsoft */,
    EAP_VENDOR_WFA = 0x00372A /* Wi-Fi Alliance */
};

/* Start - Jau porting from JK SUPC 0.7.3 { */
#define EAP_MSK_LEN 64
#define EAP_EMSK_LEN 64
/* Stop  - Jau porting from JK SUPC 0.7.3 } */


#endif /* EAP_DEFS_H */
