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



#ifndef EAP_H
#define EAP_H
#include "wpa.h"
#include "defs.h"
#include "eap_defs.h"

struct eap_sm;
struct wpa_ssid;

#ifdef IEEE8021X_EAPOL

enum eapol_bool_var {
    EAPOL_eapSuccess, EAPOL_eapRestart, EAPOL_eapFail, EAPOL_eapResp,
    EAPOL_eapNoResp, EAPOL_eapReq, EAPOL_portEnabled, EAPOL_altAccept,
    EAPOL_altReject
};

enum eapol_int_var {
    EAPOL_idleWhile
};

struct eapol_callbacks {
    struct wpa_ssid *(*get_config)(void *ctx);
    Boolean(*get_bool)(void *ctx, enum eapol_bool_var variable);
    void (*set_bool)(void *ctx, enum eapol_bool_var variable,
                     Boolean value);
    unsigned int (*get_int)(void *ctx, enum eapol_int_var variable);
    void (*set_int)(void *ctx, enum eapol_int_var variable,
                    unsigned int value);
    u8 *(*get_eapReqData)(void *ctx, wpa_size_t *len);
};

struct eap_sm *eap_sm_init(void *sm_ctx, struct eapol_callbacks *eapol_cb,
                           void *msg_ctx);
void eap_sm_deinit(struct eap_sm *sm);
int eap_sm_step(struct eap_sm *sm);
void eap_sm_abort(struct eap_sm *sm);
int eap_sm_get_status(struct eap_sm *sm, char *buf, wpa_size_t buflen,
                      int verbose);
u8 *eap_sm_buildIdentity(struct eap_sm *sm, int id, wpa_size_t *len,
                         int encrypted);
const struct eap_method *eap_sm_get_eap_methods(int vendor, EapType method);
void eap_sm_request_identity(struct eap_sm *sm, struct wpa_ssid *config);
void eap_sm_request_password(struct eap_sm *sm, struct wpa_ssid *config);
void eap_sm_request_otp(struct eap_sm *sm, struct wpa_ssid *config,
                        char *msg, wpa_size_t msg_len);
void eap_sm_notify_ctrl_attached(struct eap_sm *sm);
u8 eap_get_type(const char *name);
u8 eap_get_phase2_type(const char *name);
u8 *eap_get_phase2_types(struct wpa_ssid *config, wpa_size_t *count);
void eap_set_fast_reauth(struct eap_sm *sm, int enabled);
void eap_set_workaround(struct eap_sm *sm, unsigned int workaround);
struct wpa_ssid *eap_get_config(struct eap_sm *sm);
int eap_key_available(struct eap_sm *sm);
void eap_notify_success(struct eap_sm *sm);
u8 *eap_get_eapKeyData(struct eap_sm *sm, wpa_size_t *len);
u8 *eap_get_eapRespData(struct eap_sm *sm, wpa_size_t *len);
void eap_register_scard_ctx(struct eap_sm *sm, void *ctx);

#else /* IEEE8021X_EAPOL */

__KAL_INLINE__ u8 eap_get_type(const char *name)
{
    return EAP_TYPE_NONE;
}

#endif /* IEEE8021X_EAPOL */

#endif /* EAP_H */
