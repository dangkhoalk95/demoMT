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


#ifndef EAP_I_H
#define EAP_I_H

#include "eap.h"

/* draft-ietf-eap-statemachine-05.pdf - Peer state machine */

typedef enum {
    DECISION_FAIL, DECISION_COND_SUCC, DECISION_UNCOND_SUCC
} EapDecision;

typedef enum {
    METHOD_NONE, METHOD_INIT, METHOD_CONT, METHOD_MAY_CONT, METHOD_DONE
} EapMethodState;

struct eap_method_ret {
    Boolean ignore;
    EapMethodState methodState;
    EapDecision decision;
    Boolean allowNotifications;
};


typedef struct eap_method {
    EapType method;
    const char *name;

    void *(*init)(struct eap_sm *sm);
    void (*deinit)(struct eap_sm *sm, void *priv);
    u8 *(*process)(struct eap_sm *sm, void *priv,
                   struct eap_method_ret *ret,
                   u8 *reqData, wpa_size_t reqDataLen,
                   wpa_size_t *respDataLen);
    Boolean(*isKeyAvailable)(struct eap_sm *sm, void *priv);
    u8 *(*getKey)(struct eap_sm *sm, void *priv, wpa_size_t *len);
    int (*get_status)(struct eap_sm *sm, void *priv, char *buf,
                      wpa_size_t buflen, int verbose);

    /* Optional handlers for fast re-authentication */
    Boolean(*has_reauth_data)(struct eap_sm *sm, void *priv);
    void (*deinit_for_reauth)(struct eap_sm *sm, void *priv);
    void *(*init_for_reauth)(struct eap_sm *sm, void *priv);
    const u8 *(*get_identity)(struct eap_sm *sm, void *priv, wpa_size_t *len);
    /*add for support WPS and more WFA method in the feature
     in wps, this vendor is 0x00372a, */
    u32 vendor;
} eap_method ;


struct eap_sm {
    enum {
        EAP_INITIALIZE, EAP_DISABLED, EAP_IDLE, EAP_RECEIVED,
        EAP_GET_METHOD, EAP_METHOD, EAP_SEND_RESPONSE, EAP_DISCARD,
        EAP_IDENTITY, EAP_NOTIFICATION, EAP_RETRANSMIT, EAP_SUCCESS,
        EAP_FAILURE
    } EAP_state;
    /* Long-term local variables */
    EapType selectedMethod;
    EapMethodState methodState;
    int lastId;
    u8 *lastRespData;
    wpa_size_t lastRespDataLen;
    EapDecision decision;
    /* Short-term local variables */
    Boolean rxReq;
    Boolean rxSuccess;
    Boolean rxFailure;
    int reqId;
    EapType reqMethod;
    int reqVendor;
    u32 reqVendorMethod;
    Boolean ignore;
    /* Constants */
    int ClientTimeout;

    /* Miscellaneous variables */
    Boolean allowNotifications; /* peer state machine <-> methods */
    u8 *eapRespData; /* peer to lower layer */
    wpa_size_t eapRespDataLen; /* peer to lower layer */
    Boolean eapKeyAvailable; /* peer to lower layer */
    u8 *eapKeyData; /* peer to lower layer */
    wpa_size_t eapKeyDataLen; /* peer to lower layer */
    const struct eap_method *m; /* selected EAP method */
    /* not defined in draft-ietf-eap-statemachine-02 */
    Boolean changed;
    /*it point a eapol_sm instance*/
    void *eapol_ctx;
    struct eapol_callbacks *eapol_cb;
    void *eap_method_priv;
    int init_phase2;
    int fast_reauth;

    Boolean rxResp /* LEAP only */;
    Boolean leap_done;
    Boolean peap_done;
    u8 req_md5[16]; /* MD5() of the current EAP packet */
    u8 last_md5[16]; /* MD5() of the previously received EAP packet; used
              * in duplicate request detection. */

    void *msg_ctx;
    void *scard_ctx;
    /* Joseph csolution SSL Modification */
    //void *ssl_ctx;
    void *globalCtx;
    void *sessionDB;

    unsigned int workaround;

    /* Optional challenges generated in Phase 1 (EAP-FAST) */
    u8 *peer_challenge, *auth_challenge;

    int num_rounds;
    struct wps_context *wps;
    Boolean wps_done;
};

#endif /* EAP_I_H */
