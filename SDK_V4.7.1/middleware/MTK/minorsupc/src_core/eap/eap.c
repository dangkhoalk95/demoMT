/**
 * Copyright (c) 2004-2005, Jouni Malinen <jkmaline@cc.hut.fi>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  - Neither the name of the the name(s) of the above-listed copyright holder(s) nor the names of
 *    its contributors may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /*
 * WPA Supplicant / EAP state machines
 * Copyright (c) 2004-2005, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif

#include <stdio.h>
//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
/* Start - Jau add to eliminate too much warning { */
//#include "kal_trace.h"
/* Stop  - Jau add to eliminate too much warning } */

#if 0
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"
#endif
#include "supc_common.h"

#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "supc_abm_msgs.h"
#include "config_ssid.h"
#include "wpa.h"
#include "wpa_supplicant.h"
#include "wpa_supplicant_i.h"

#include "eap_i.h"
//#include "tls.h"
#include "supc_md5.h"


//#include "ssl_api.h"
//#include "supc_ssl.h"


#include "eapol_sm.h"

extern wpa_supplicant *wpa_s;

#define EAP_MAX_AUTH_ROUNDS 50

#ifdef EAP_MD5
//extern kal_uint8 test;
extern const struct eap_method eap_method_md5;
#endif
#ifdef EAP_TLS
extern const struct eap_method eap_method_tls;
#endif
#ifdef EAP_MSCHAPv2
extern const struct eap_method eap_method_mschapv2;
#endif
#ifdef EAP_PEAP
extern const struct eap_method eap_method_peap;
#endif
#ifdef EAP_TTLS
extern const struct eap_method eap_method_ttls;
#endif
#ifdef EAP_GTC
extern const struct eap_method eap_method_gtc;
#endif
#ifdef EAP_OTP
extern const struct eap_method eap_method_otp;
#endif
#ifdef EAP_SIM
extern const struct eap_method eap_method_sim;
#endif
#ifdef EAP_LEAP
extern const struct eap_method eap_method_leap;
#endif
#ifdef EAP_PSK
extern const struct eap_method eap_method_psk;
#endif
#ifdef EAP_AKA
extern const struct eap_method eap_method_aka;
#endif
#ifdef EAP_FAST
extern const struct eap_method eap_method_fast;
#endif
#ifdef __WPS_SUPPORT__
extern const struct eap_method eap_method_wsc;
#endif

void *eap_dummy_init(struct eap_sm *sm)
{
    return NULL;
}

void eap_dummy_deinit(struct eap_sm *sm, void *priv)
{
    return;
}

u8 *eap_dummy_process(struct eap_sm *sm, void *priv,
                      struct eap_method_ret *ret,
                      u8 *reqData, wpa_size_t reqDataLen,
                      wpa_size_t *respDataLen)
{
    return NULL;
}

const struct eap_method eap_method_dummy = {
    EAP_TYPE_NONE,
    "EAP_DUMMY",
    eap_dummy_init,
    eap_dummy_deinit,
    eap_dummy_process,
    NULL, /*isKeyAvailable*/
    NULL, /*getKey*/
    NULL, /*get_status*/
    NULL, /*has_reauth_data*/
    NULL, /*deinit_for_reauth*/
    NULL, /*init_for_reauth*/
    NULL, /*get_identity*/
    0
};

static const struct eap_method *eap_methods[] = {
#ifdef EAP_MD5
    &eap_method_md5,
#endif
#ifdef EAP_TLS
    &eap_method_tls,
#endif
#ifdef EAP_MSCHAPv2
    &eap_method_mschapv2,
#endif
#ifdef EAP_PEAP
    &eap_method_peap,
#endif
#ifdef EAP_TTLS
    &eap_method_ttls,
#endif
#ifdef EAP_GTC
    &eap_method_gtc,
#endif
#ifdef EAP_OTP
    &eap_method_otp,
#endif
#ifdef EAP_SIM
    &eap_method_sim,
#endif
#ifdef EAP_LEAP
    &eap_method_leap,
#endif
#ifdef EAP_PSK
    &eap_method_psk,
#endif
#ifdef EAP_AKA
    &eap_method_aka,
#endif
#ifdef EAP_FAST
    &eap_method_fast,
#endif
#ifdef __WPS_SUPPORT__
    &eap_method_wsc,
#endif
    /*padding, if all eap method is disable*/
    &eap_method_dummy
};
#define NUM_EAP_METHODS (sizeof(eap_methods) / sizeof(eap_methods[0]))

/*mtk80707, modify for support WPS's feature*/
const struct eap_method *eap_sm_get_eap_methods(int vendor, EapType method)
{
    int i;
    for (i = 0; i < NUM_EAP_METHODS; i++) {
        if (0 == vendor) {
            if (eap_methods[i]->method == method) {
                return eap_methods[i];
            }
        } else {
            if (eap_methods[i]->method == method &&
                    eap_methods[i]->vendor == vendor) {
                return eap_methods[i];
            }
        }
    }
    return NULL;
}


static Boolean eap_sm_allowMethod(struct eap_sm *sm, int vendor, EapType method);
static u8 *eap_sm_buildNak(struct eap_sm *sm, int id, wpa_size_t *len);
static void eap_sm_processIdentity(struct eap_sm *sm, u8 *req, wpa_size_t len);
static void eap_sm_processNotify(struct eap_sm *sm, u8 *req, wpa_size_t len);
static u8 *eap_sm_buildNotify(struct eap_sm *sm, int id, wpa_size_t *len);
static void eap_sm_parseEapReq(struct eap_sm *sm, u8 *req, wpa_size_t len);
static const char *eap_sm_method_state_txt(int state);
static const char *eap_sm_decision_txt(int decision);


/* Definitions for clarifying state machine implementation */
#define SM_STATE(machine, state) \
static void sm_ ## machine ## _ ## state ## _Enter(struct eap_sm *sm, \
    int global)

#define SM_ENTRY(machine, state) \
if (!global || sm->machine ## _state != machine ## _ ## state) { \
    sm->changed = TRUE; \
} \
sm->machine ## _state = machine ## _ ## state;
#if 0
kal_wap_trace(MAP_MOD_SUPC, DBG_EAP_SM, "%s", "EAP: " #machine " entering state " #state);
#endif
#define SM_ENTER(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm, 0)
#define SM_ENTER_GLOBAL(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm, 1)

#define SM_STEP(machine) \
static void sm_ ## machine ## _Step(struct eap_sm *sm)

#define SM_STEP_RUN(machine) sm_ ## machine ## _Step(sm)


Boolean eapol_get_bool(struct eap_sm *sm, enum eapol_bool_var var) {
    return sm->eapol_cb->get_bool(sm->eapol_ctx, var);
}


static void eapol_set_bool(struct eap_sm *sm, enum eapol_bool_var var,
                           Boolean value) {
    sm->eapol_cb->set_bool(sm->eapol_ctx, var, value);
}


static unsigned int eapol_get_int(struct eap_sm *sm, enum eapol_int_var var) {
    return sm->eapol_cb->get_int(sm->eapol_ctx, var);
}


static void eapol_set_int(struct eap_sm *sm, enum eapol_int_var var,
                          unsigned int value) {
    sm->eapol_cb->set_int(sm->eapol_ctx, var, value);
}


static u8 *eapol_get_eapReqData(struct eap_sm *sm, wpa_size_t *len)
{
    return sm->eapol_cb->get_eapReqData(sm->eapol_ctx, len);
}


static void eap_deinit_prev_method(struct eap_sm *sm, const char *txt)
{
    if (sm->m == NULL || sm->eap_method_priv == NULL) {
        return;
    }

    wpa_printf(TRACE_GROUP_1, "EAP: deinitialize previously used EAP method "
               "(%d, %s) at %s\n", sm->selectedMethod, sm->m->name, txt);
//  kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_DEINIT_PRE_METHOD);
//  kal_wap_trace(MAP_MOD_SUPC,DBG_EAP_SM,"(%d, %s) at %s", sm->selectedMethod, sm->m->name, txt);
    sm->m->deinit(sm, sm->eap_method_priv);
    sm->eap_method_priv = NULL;
    sm->m = NULL;
}


SM_STATE(EAP, INITIALIZE)
{
    SM_ENTRY(EAP, INITIALIZE);
    if (sm->fast_reauth && sm->m && sm->m->has_reauth_data &&
            sm->m->has_reauth_data(sm, sm->eap_method_priv)) {
        wpa_printf(MSG_DEBUG, "EAP: maintaining EAP method data for "
                   "fast reauthentication\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_HAVE_FAST_REAUTH_DATA);
        sm->m->deinit_for_reauth(sm, sm->eap_method_priv);
    } else {
        eap_deinit_prev_method(sm, "INITIALIZE");
    }
    sm->selectedMethod = EAP_TYPE_NONE;
    sm->methodState = METHOD_NONE;
    sm->allowNotifications = TRUE;
    sm->decision = DECISION_FAIL;
    eapol_set_int(sm, EAPOL_idleWhile, sm->ClientTimeout);
    eapol_set_bool(sm, EAPOL_eapSuccess, FALSE);
    eapol_set_bool(sm, EAPOL_eapFail, FALSE);
    if (sm->eapKeyData != NULL) {
        SUPC_DEALLOC_BUFF(sm->eapKeyData);
    }
    sm->eapKeyData = NULL;
    sm->eapKeyAvailable = FALSE;
    eapol_set_bool(sm, EAPOL_eapRestart, FALSE);
    sm->lastId = -1; /* new session - make sure this does not match with
              * the first EAP-Packet */
    /* draft-ietf-eap-statemachine-02.pdf does not reset eapResp and
     * eapNoResp here. However, this seemed to be able to trigger cases
     * where both were set and if EAPOL state machine uses eapNoResp first,
     * it may end up not sending a real reply correctly. This occurred
     * when the workaround in FAIL state set eapNoResp = TRUE.. Maybe that
     * workaround needs to be fixed to do something else(?) */
    eapol_set_bool(sm, EAPOL_eapResp, FALSE);
    eapol_set_bool(sm, EAPOL_eapNoResp, FALSE);
    sm->num_rounds = 0;
}


SM_STATE(EAP, DISABLED)
{
    SM_ENTRY(EAP, DISABLED);
    sm->num_rounds = 0;
}


SM_STATE(EAP, IDLE)
{
    SM_ENTRY(EAP, IDLE);
}


SM_STATE(EAP, RECEIVED)
{
    u8 *eapReqData = NULL;
    wpa_size_t eapReqDataLen = 0;

    SM_ENTRY(EAP, RECEIVED);
    eapReqData = eapol_get_eapReqData(sm, &eapReqDataLen);
    /* parse rxReq, rxSuccess, rxFailure, reqId, reqMethod */
    eap_sm_parseEapReq(sm, eapReqData, eapReqDataLen);
    sm->num_rounds++;
}

#define EAP_AKA_UNIT_TEST       0
#if EAP_AKA_UNIT_TEST
extern void eap_aka_test_main(struct eap_sm *sm);  /* Jau add */
unsigned int eap_aka_flag = 0;
#endif /* EAP_AKA_UNIT_TEST */

SM_STATE(EAP, GET_METHOD)   /* Jau : sm_EAP_GET_METHOD_Enter */
{
    int reinit = 0;
    EapType method = EAP_TYPE_NONE;
    SM_ENTRY(EAP, GET_METHOD);

    if (sm->reqMethod == EAP_TYPE_EXPANDED) {
        method = (EapType)sm->reqVendorMethod;    /* Jau add casting to avoid warning */
    } else {
        method = sm->reqMethod;
    }

#if EAP_AKA_UNIT_TEST
    if (eap_aka_flag++) {
        /* Jau : try here to add some code to change method type to AKA */
        /*       and then call  AKA test main in SM_STATE(EAP, METHOD)  */
        method = sm->reqMethod = EAP_TYPE_AKA;
        eap_aka_test_main(sm);
    }
#endif /* EAP_AKA_UNIT_TEST */


    if (eap_sm_allowMethod(sm, sm->reqVendor, method)) {
        if (sm->fast_reauth &&
                sm->m && sm->m->method == method &&
                sm->m->vendor == sm->reqVendor &&
                sm->m->has_reauth_data &&
                sm->m->has_reauth_data(sm, sm->eap_method_priv)) {
            wpa_printf(TRACE_GROUP_1, "EAP: using previous method data"
                       " for fast re-authentication\n");
//kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_USE_PRE_DATA_FAST_REAUTH);
            reinit = 1;
        } else {
            eap_deinit_prev_method(sm, "GET_METHOD");
        }
        sm->selectedMethod = sm->reqMethod;
        if (sm->m == NULL) {
            sm->m = eap_sm_get_eap_methods(sm->reqVendor, method);
        }

        if (sm->m) {
            wpa_printf(TRACE_GROUP_1, "EAP: initialize selected EAP "
                       "method (%d, %s)\n",
                       sm->selectedMethod, sm->m->name);
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_INIT_SELECTED_METHOD);
//          kal_wap_trace(MAP_MOD_SUPC,DBG_EAP_SM,"%s",sm->m->name);
            if (reinit) {
                sm->eap_method_priv = sm->m->init_for_reauth(
                                          sm, sm->eap_method_priv);
            } else {
                sm->eap_method_priv = sm->m->init(sm);
                //add for WiFi UE enhancement to remember EAP setting
                wpa_s->last_eap_method = sm->selectedMethod;
                wpa_printf(TRACE_GROUP_1, "wpa_s->last_eap_method (%d)\n", wpa_s->last_eap_method);
//              kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_LAST_METHOD, wpa_s->last_eap_method);
            }

            if (sm->eap_method_priv == NULL) {
                wpa_printf(TRACE_GROUP_1, "EAP: Failed to "
                           "initialize EAP method %d\n",
                           sm->selectedMethod);
//              kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_INIT_METHOD_FAIL,sm->selectedMethod);
                sm->m = NULL;
                sm->methodState = METHOD_NONE;
                sm->selectedMethod = EAP_TYPE_NONE;
            } else {
                sm->methodState = METHOD_INIT;
                return;
            }
        }
    }

    SUPC_DEALLOC_BUFF(sm->eapRespData);
    sm->eapRespData = eap_sm_buildNak(sm, sm->reqId, &sm->eapRespDataLen);
}


SM_STATE(EAP, METHOD)
{
    u8 *eapReqData = NULL;
    wpa_size_t eapReqDataLen = 0;
    struct eap_method_ret ret;

    SM_ENTRY(EAP, METHOD);
    if (sm->m == NULL) {
        wpa_printf(TRACE_GROUP_1, "EAP::METHOD - method not selected\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_METHOD_NOT_SELECTED);
        return;
    }

    eapReqData = eapol_get_eapReqData(sm, &eapReqDataLen);

    /* Get ignore, methodState, decision, allowNotifications, and
     * eapRespData. */
    memset(&ret, 0, sizeof(ret));
    ret.ignore = sm->ignore;
    ret.methodState = sm->methodState;
    ret.decision = sm->decision;
    ret.allowNotifications = sm->allowNotifications;
    SUPC_DEALLOC_BUFF(sm->eapRespData);
    sm->eapRespData = sm->m->process(sm, sm->eap_method_priv, &ret,
                                     eapReqData, eapReqDataLen,
                                     &sm->eapRespDataLen);
//  kal_wap_trace(MAP_MOD_SUPC,DBG_EAP_SM,"EAP: method process -> ignore=%s "
//         "methodState=%s decision=%s",
//         ret.ignore ? "TRUE" : "FALSE",
//         eap_sm_method_state_txt(ret.methodState),
//         eap_sm_decision_txt(ret.decision));

    wpa_hexdump(MSG_MSGDUMP, "EAP_SEND_RESPONSE",
                (const char *)sm->eapRespData, sm->eapRespDataLen); /* Jau add casting to avoid warning */

    sm->ignore = ret.ignore;
    if (sm->ignore) {
        return;
    }
    sm->methodState = ret.methodState;
    sm->decision = ret.decision;
    sm->allowNotifications = ret.allowNotifications;

    if (sm->m->isKeyAvailable && sm->m->getKey &&
            sm->m->isKeyAvailable(sm, sm->eap_method_priv)) {
        SUPC_DEALLOC_BUFF(sm->eapKeyData);
        sm->eapKeyData = sm->m->getKey(sm, sm->eap_method_priv,
                                       &sm->eapKeyDataLen);
    }
}


SM_STATE(EAP, SEND_RESPONSE)
{
    SM_ENTRY(EAP, SEND_RESPONSE);
    SUPC_DEALLOC_BUFF(sm->lastRespData);
    if (sm->eapRespData) {
        if (sm->workaround) {
            memcpy(sm->last_md5, sm->req_md5, 16);
        }
        sm->lastId = sm->reqId;
        sm->lastRespData = SUPC_ALLOC_BUFF(sm->eapRespDataLen);
        if (sm->lastRespData) {
            memcpy(sm->lastRespData, sm->eapRespData,
                   sm->eapRespDataLen);
            sm->lastRespDataLen = sm->eapRespDataLen;
        }
        eapol_set_bool(sm, EAPOL_eapResp, TRUE);
    } else {
        sm->lastRespData = NULL;
    }

    eapol_set_bool(sm, EAPOL_eapReq, FALSE);
    eapol_set_int(sm, EAPOL_idleWhile, sm->ClientTimeout);
}


SM_STATE(EAP, DISCARD)
{
    SM_ENTRY(EAP, DISCARD);
    eapol_set_bool(sm, EAPOL_eapReq, FALSE);
    eapol_set_bool(sm, EAPOL_eapNoResp, TRUE);
}


SM_STATE(EAP, IDENTITY)
{
    u8 *eapReqData = NULL;
    wpa_size_t eapReqDataLen = 0;

    SM_ENTRY(EAP, IDENTITY);
    eapReqData = eapol_get_eapReqData(sm, &eapReqDataLen);
    eap_sm_processIdentity(sm, eapReqData, eapReqDataLen);
    SUPC_DEALLOC_BUFF(sm->eapRespData);
    sm->eapRespData = eap_sm_buildIdentity(sm, sm->reqId,
                                           &sm->eapRespDataLen, 0);
}


SM_STATE(EAP, NOTIFICATION)
{
    u8 *eapReqData = NULL;
    wpa_size_t eapReqDataLen = 0;

    SM_ENTRY(EAP, NOTIFICATION);
    eapReqData = eapol_get_eapReqData(sm, &eapReqDataLen);
    eap_sm_processNotify(sm, eapReqData, eapReqDataLen);
    SUPC_DEALLOC_BUFF(sm->eapRespData);
    sm->eapRespData = eap_sm_buildNotify(sm, sm->reqId,
                                         &sm->eapRespDataLen);
}


SM_STATE(EAP, RETRANSMIT)
{
    SM_ENTRY(EAP, RETRANSMIT);
    SUPC_DEALLOC_BUFF(sm->eapRespData);
    if (sm->lastRespData) {
        sm->eapRespData = SUPC_ALLOC_BUFF(sm->lastRespDataLen);
        if (sm->eapRespData) {
            memcpy(sm->eapRespData, sm->lastRespData,
                   sm->lastRespDataLen);
            sm->eapRespDataLen = sm->lastRespDataLen;
        }
    } else {
        sm->eapRespData = NULL;
    }
}


SM_STATE(EAP, SUCCESS)
{
    SM_ENTRY(EAP, SUCCESS);
    if (sm->eapKeyData != NULL) {
        sm->eapKeyAvailable = TRUE;
    }
    eapol_set_bool(sm, EAPOL_eapSuccess, TRUE);
    /* draft-ietf-eap-statemachine-02.pdf does not clear eapReq here, but
     * this seems to be required to avoid processing the same request
     * twice when state machine is initialized. */
    eapol_set_bool(sm, EAPOL_eapReq, FALSE);
    /* draft-ietf-eap-statemachine-02.pdf does not set eapNoResp here, but
     * this seems to be required to get EAPOL Supplicant backend state
     * machine into SUCCESS state. In addition, either eapResp or eapNoResp
     * is required to be set after processing the received EAP frame. */
    eapol_set_bool(sm, EAPOL_eapNoResp, TRUE);

    /*MAUI_03055418. supc support pre-auth.
    it may keep two ssl connection.  but serucity lib doesn't allow this operation.
    it may eat up all of memory in Seurity lib. so supc must free ssl connection
    after authen pass*/
    if (wpa_s->support_pre_auth) {
        eap_deinit_prev_method(sm, "eap success");
    }
}


SM_STATE(EAP, FAILURE)
{
    wpa_printf(TRACE_GROUP_1, "previouse EAP_state : %d\n", sm->EAP_state);
//  kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_PREV_STA, sm->EAP_state);

    //add for WiFi UE enhancement
    wpa_printf(TRACE_GROUP_1, "selectedMethod : %d reqMethod : %d\n", sm->selectedMethod, sm->reqMethod);
//  kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_METHOD_SEL_REQ, sm->selectedMethod, sm->reqMethod);
    if (wpa_s->eap_fail_cause != SUPC_ABM_UNSUPPORTED_EAP_TYPE &&
            wpa_s->eap_fail_cause != SUPC_ABM_WRONG_USER_INFO) {
        if ((sm->selectedMethod != sm->reqMethod) &&
                (sm->reqMethod != EAP_TYPE_IDENTITY)) {
            wpa_s->eap_fail_cause = SUPC_ABM_UNSUPPORTED_EAP_TYPE;
            wpa_printf(TRACE_GROUP_1, "eap_fail_cause : (Phase1) SUPC_ABM_UNSUPPORTED_EAP_TYPE\n");
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_PHASE1_FAIL_CAUSE);
        } else {
            if (wpa_s->phase2_nak == 1) {
                wpa_s->eap_fail_cause = SUPC_ABM_UNSUPPORTED_EAP_TYPE;
                wpa_printf(TRACE_GROUP_1, "eap_fail_cause : (Phase2) SUPC_ABM_UNSUPPORTED_EAP_TYPE\n");
//              kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_PHASE2_FAIL_CAUSE);
                wpa_s->phase2_nak = 0;
            } else {
                wpa_s->eap_fail_cause = SUPC_ABM_WRONG_USER_INFO;
                wpa_printf(TRACE_GROUP_1, "eap_fail_cause : SUPC_ABM_WRONG_USER_INFO\n");
//              kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_FAIL_WRONG_USER);
            }
        }
    } else {
        wpa_printf(TRACE_GROUP_1, "eap_fail_cause : %d already updated.\n", wpa_s->eap_fail_cause);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_FAIL_CAUSE_UPDATE, wpa_s->eap_fail_cause);
    }

    SM_ENTRY(EAP, FAILURE);
    eapol_set_bool(sm, EAPOL_eapFail, TRUE);
    /* draft-ietf-eap-statemachine-02.pdf does not clear eapReq here, but
     * this seems to be required to avoid processing the same request
     * twice when state machine is initialized. */
    eapol_set_bool(sm, EAPOL_eapReq, FALSE);
    /* draft-ietf-eap-statemachine-02.pdf does not set eapNoResp here.
     * However, either eapResp or eapNoResp is required to be set after
     * processing the received EAP frame. */
    eapol_set_bool(sm, EAPOL_eapNoResp, TRUE);
}


static int eap_success_workaround(struct eap_sm *sm, int reqId, int lastId)
{
    /* At least Microsoft IAS and Meetinghouse Aegis seem to be sending
     * EAP-Success/Failure with lastId + 1 even though RFC 3748 and
     * draft-ietf-eap-statemachine-05.pdf require that reqId == lastId.
     * Accept this kind of Id if EAP workarounds are enabled. These are
     * unauthenticated plaintext messages, so this should have minimal
     * security implications (bit easier to fake EAP-Success/Failure). */
    if (sm->workaround && reqId == ((lastId + 1) & 0xff)) {
        wpa_printf(MSG_DEBUG, "EAP: Workaround for unexpected "
                   "identifier field in EAP Success: "
                   "reqId=%d lastId=%d (these are supposed to be "
                   "same)\n", reqId, lastId);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_SUCCESS_WA_ID, reqId, lastId);
        return 1;
    }
    return 0;
}


SM_STEP(EAP)
{
    int duplicate;

    if (eapol_get_bool(sm, EAPOL_eapRestart) &&
            eapol_get_bool(sm, EAPOL_portEnabled)) {
        SM_ENTER_GLOBAL(EAP, INITIALIZE);
    } else if (!eapol_get_bool(sm, EAPOL_portEnabled)) {
        SM_ENTER_GLOBAL(EAP, DISABLED);
    } else if (sm->num_rounds > EAP_MAX_AUTH_ROUNDS) {
        if (sm->num_rounds == EAP_MAX_AUTH_ROUNDS + 1) {
            wpa_printf(MSG_DEBUG, "EAP: more than %d "
                       "authentication rounds - abort\n",
                       EAP_MAX_AUTH_ROUNDS);
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_AUTH_ROUNDS_TOO_MUCH, EAP_MAX_AUTH_ROUNDS);
            sm->num_rounds++;
            SM_ENTER_GLOBAL(EAP, FAILURE);
        }
    } else switch (sm->EAP_state) {
            case EAP_INITIALIZE:
                SM_ENTER(EAP, IDLE);
                break;
            case EAP_DISABLED:
                if (eapol_get_bool(sm, EAPOL_portEnabled)) {
                    SM_ENTER(EAP, INITIALIZE);
                }
                break;
            case EAP_IDLE:
                if (eapol_get_bool(sm, EAPOL_eapReq)) {
                    SM_ENTER(EAP, RECEIVED);
                } else if ((eapol_get_bool(sm, EAPOL_altAccept) &&
                            sm->decision != DECISION_FAIL) ||
                           (eapol_get_int(sm, EAPOL_idleWhile) == 0 &&
                            sm->decision == DECISION_UNCOND_SUCC)) {
                    SM_ENTER(EAP, SUCCESS);
                } else if (eapol_get_bool(sm, EAPOL_altReject) ||
                           (eapol_get_int(sm, EAPOL_idleWhile) == 0 &&
                            sm->decision != DECISION_UNCOND_SUCC) ||
                           (eapol_get_bool(sm, EAPOL_altAccept) &&
                            sm->methodState != METHOD_CONT &&
                            sm->decision == DECISION_FAIL)) {
                    SM_ENTER(EAP, FAILURE);
                } else if (sm->selectedMethod == EAP_TYPE_LEAP &&
                           sm->leap_done && sm->decision != DECISION_FAIL &&
                           sm->methodState == METHOD_DONE) {
                    SM_ENTER(EAP, SUCCESS);
                } else if (sm->selectedMethod == EAP_TYPE_PEAP &&
                           sm->peap_done && sm->decision != DECISION_FAIL &&
                           sm->methodState == METHOD_DONE) {
                    SM_ENTER(EAP, SUCCESS);
                }
                break;
            case EAP_RECEIVED:
                duplicate = sm->reqId == sm->lastId;
                if (sm->workaround && duplicate &&
                        memcmp(sm->req_md5, sm->last_md5, 16) != 0) {
                    /* draft-ietf-eap-statemachine-05.txt uses
                     * (reqId == lastId) as the only verification for
                     * duplicate EAP requests. However, this misses cases
                     * where the AS is incorrectly using the same id again;
                     * and unfortunately, such implementations exist. Use
                     * MD5 hash as an extra verification for the packets
                     * being duplicate to workaround these issues. */
                    wpa_printf(TRACE_GROUP_1, "EAP: AS used the same Id again,"
                               " but EAP packets were not identical\n");
//kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_AS_USE_SAME_IDENTIFIER);
                    wpa_printf(TRACE_GROUP_1, "EAP: workaround - assume this "
                               "is not a duplicate packet\n");
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_SAME_IDENTIFIER_WA);
                    duplicate = 0;
                }

                if (sm->rxSuccess &&
                        (sm->reqId == sm->lastId ||
                         eap_success_workaround(sm, sm->reqId, sm->lastId)) &&
                        sm->decision != DECISION_FAIL) {
                    SM_ENTER(EAP, SUCCESS);
                } else if (sm->methodState != METHOD_CONT &&
                           ((sm->rxFailure &&
                             sm->decision != DECISION_UNCOND_SUCC) ||
                            (sm->rxSuccess && sm->decision == DECISION_FAIL)) &&
                           (sm->reqId == sm->lastId ||
                            eap_success_workaround(sm, sm->reqId, sm->lastId))) {
                    SM_ENTER(EAP, FAILURE);
                } else if (sm->rxReq && duplicate) {
                    SM_ENTER(EAP, RETRANSMIT);
                } else if (sm->rxReq && !duplicate &&
                           sm->reqMethod == EAP_TYPE_NOTIFICATION &&
                           sm->allowNotifications) {
                    SM_ENTER(EAP, NOTIFICATION);
                } else if (sm->rxReq && !duplicate &&
                           sm->selectedMethod == EAP_TYPE_NONE &&
                           sm->reqMethod == EAP_TYPE_IDENTITY) {
                    SM_ENTER(EAP, IDENTITY);
                } else if (sm->rxReq && !duplicate &&
                           sm->selectedMethod == EAP_TYPE_NONE &&
                           sm->reqMethod != EAP_TYPE_IDENTITY &&
                           sm->reqMethod != EAP_TYPE_NOTIFICATION) {
                    SM_ENTER(EAP, GET_METHOD);
                } else if (sm->rxReq && !duplicate &&
                           sm->reqMethod == sm->selectedMethod &&
                           sm->methodState != METHOD_DONE) {
                    SM_ENTER(EAP, METHOD);
                } else if (sm->selectedMethod == EAP_TYPE_LEAP &&
                           (sm->rxSuccess || sm->rxResp)) {
                    SM_ENTER(EAP, METHOD);
                } else {
                    wpa_printf(MSG_DEBUG, "sm->methodState=%d, sm->rxFailure=%d, sm->decision=%d, sm->rxSuccess=%d, sm->decision=%d, sm->reqId=%d, sm->lastId=%d\n",
                               sm->methodState, sm->rxFailure, sm->decision, sm->rxSuccess, sm->decision, sm->reqId, sm->lastId);
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_SM_DUMP,sm->methodState, sm->rxFailure, sm->decision, sm->rxSuccess, sm->decision, sm->reqId, sm->lastId);
                    SM_ENTER(EAP, DISCARD);
                }
                break;
            case EAP_GET_METHOD:
                if (sm->selectedMethod == sm->reqMethod) {
                    SM_ENTER(EAP, METHOD);
                } else {
                    SM_ENTER(EAP, SEND_RESPONSE);
                }
                break;
            case EAP_METHOD:
                if (sm->ignore) {
                    SM_ENTER(EAP, DISCARD);
                }
                /*wps need to send WSC_DONE even method is done*/
                else if (sm->methodState == METHOD_DONE &&
                         sm->decision == DECISION_FAIL &&
                         TRUE == sm->wps_done) {
                    SM_ENTER(EAP, SEND_RESPONSE);
                } else if (sm->methodState == METHOD_DONE && sm->decision == DECISION_FAIL) { //Kinki change for match RFC4137 Aug,2005
                    SM_ENTER(EAP, FAILURE);    //Kinki change for match RFC4137 Aug,2005
                } else {
                    SM_ENTER(EAP, SEND_RESPONSE);
                }
                break;
            case EAP_SEND_RESPONSE:
                SM_ENTER(EAP, IDLE);
                break;
            case EAP_DISCARD:
                SM_ENTER(EAP, IDLE);
                break;
            case EAP_IDENTITY:
                SM_ENTER(EAP, SEND_RESPONSE);
                break;
            case EAP_NOTIFICATION:
                SM_ENTER(EAP, SEND_RESPONSE);
                break;
            case EAP_RETRANSMIT:
                SM_ENTER(EAP, SEND_RESPONSE);
                break;
            case EAP_SUCCESS:
                break;
            case EAP_FAILURE:
                break;
        }
}

int wpa_config_allowed_eap_method(struct wpa_ssid *ssid, int method)
{
    u8 *pos;

    if (ssid == NULL || ssid->eap_methods == NULL) {
        return 1;
    }

    pos = ssid->eap_methods;
    while (*pos != EAP_TYPE_NONE) {
        if (*pos == method) {
            return 1;
        }
        pos++;
    }
    return 0;
}


static Boolean eap_sm_allowMethod(struct eap_sm *sm, int vendor,
                                  EapType method)
{
    struct wpa_ssid *config = eap_get_config(sm);
    int i;

    if (!wpa_config_allowed_eap_method(config, method)) {
        return FALSE;
    }
    for (i = 0; i < NUM_EAP_METHODS; i++) {
        if (eap_methods[i]->method == method
                && eap_methods[i]->vendor == vendor);
        {
            return TRUE;
        }
    }
    return FALSE;
}


static u8 *eap_sm_buildNak(struct eap_sm *sm, int id, wpa_size_t *len)
{
    struct wpa_ssid *config = eap_get_config(sm);
    struct eap_hdr *resp;
    u8 *pos;
    kal_uint16 i, found = 0;

    wpa_printf(MSG_DEBUG, "EAP: Building EAP-Nak (requested type %d not "
               "allowed)\n", sm->reqMethod);
//  kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_BUILD_NAK, sm->reqMethod);
    *len = sizeof(struct eap_hdr) + 1;
    resp = SUPC_ALLOC_BUFF((kal_uint16)(*len + NUM_EAP_METHODS));
    if (resp == NULL) {
        return NULL;
    }

    resp->code = EAP_CODE_RESPONSE;
    resp->identifier = id;
    pos = (u8 *)(resp + 1);
    *pos++ = EAP_TYPE_NAK;

    for (i = 0; i < NUM_EAP_METHODS; i++) {
        if (wpa_config_allowed_eap_method(config,
                                          eap_methods[i]->method)) {
            *pos++ = eap_methods[i]->method;
            (*len)++;
            found++;
        }
    }
    if (!found) {
        *pos = EAP_TYPE_NONE;
        (*len)++;
    }
    wpa_hexdump(MSG_DEBUG, "EAP: allowed methods",
                ((char *)(resp + 1)) + 1, found);

    resp->length = host_to_be16(*len);

    return (u8 *) resp;
}


static void eap_sm_processIdentity(struct eap_sm *sm, u8 *req, wpa_size_t len)
{
    struct eap_hdr *hdr = (struct eap_hdr *) req;
    u8 *pos = (u8 *)(hdr + 1);
    pos++;
    /* TODO: could save displayable message so that it can be shown to the
     * user in case of interaction is required */
    wpa_hexdump_ascii(MSG_DEBUG, "EAP: EAP-Request Identity data",
                      (char *)pos, (wpa_size_t)(be_to_host16(hdr->length) - 5));
}


u8 *eap_sm_buildIdentity(struct eap_sm *sm, int id, wpa_size_t *len,
                         int encrypted)
{
    struct wpa_ssid *config = eap_get_config(sm);
    struct eap_hdr *resp;
    u8 *pos;
    const u8 *identity;
    wpa_size_t identity_len;

    if (config == NULL) {
        wpa_printf(TRACE_WARNING, "EAP: buildIdentity: configuration "
                   "was not available\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_BUILD_ID_NO_CONFIG);
        return NULL;
    }

    if (sm->m && sm->m->get_identity &&
            (identity = sm->m->get_identity(sm, sm->eap_method_priv,
                                            &identity_len)) != NULL) {
        wpa_hexdump_ascii(MSG_DEBUG, "EAP: using method re-auth "
                          "identity", (char *)identity, identity_len);
    } else if (!encrypted && config->anonymous_identity) {
        identity = config->anonymous_identity;
        identity_len = config->anonymous_identity_len;
        wpa_hexdump_ascii(MSG_DEBUG, "EAP: using anonymous identity",
                          (char *)identity, identity_len);
    } else {
        identity = config->identity;
        identity_len = config->identity_len;
        wpa_hexdump_ascii(MSG_DEBUG, "EAP: using real identity",
                          (char *)identity, identity_len);
    }

    if (identity == NULL) {
        wpa_printf(TRACE_WARNING, "EAP: buildIdentity: identity "
                   "configuration was not available\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_BUILD_ID_NO_ID_IN_CONFIG);
        identity_len = 0; // Kinki add
        //eap_sm_request_identity(sm, config);
        //return NULL;
    }


    *len = sizeof(struct eap_hdr) + 1 + identity_len;
    resp = SUPC_ALLOC_BUFF(*len);
    if (resp == NULL) {
        return NULL;
    }

    resp->code = EAP_CODE_RESPONSE;
    resp->identifier = id;
    resp->length = host_to_be16(*len);
    pos = (u8 *)(resp + 1);
    *pos++ = EAP_TYPE_IDENTITY;
    memcpy(pos, identity, identity_len);

    return (u8 *) resp;
}


static void eap_sm_processNotify(struct eap_sm *sm, u8 *req, wpa_size_t len)
{
    struct eap_hdr *hdr = (struct eap_hdr *) req;
    u8 *pos = (u8 *)(hdr + 1);
    pos++;
    /* TODO: log the Notification Request and make it available for UI */
    wpa_hexdump_ascii(MSG_DEBUG, "EAP: EAP-Request Notification data",
                      (char *)pos, (wpa_size_t)(be_to_host16(hdr->length) - 5));
}


static u8 *eap_sm_buildNotify(struct eap_sm *sm, int id, wpa_size_t *len)
{
    struct eap_hdr *resp;
    u8 *pos;

    wpa_printf(TRACE_GROUP_1, "EAP: Generating EAP-Response Notification\n");
//  kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_BUILD_NOTIFY);
    *len = sizeof(struct eap_hdr) + 1;
    resp = SUPC_ALLOC_BUFF(*len);
    if (resp == NULL) {
        return NULL;
    }

    resp->code = EAP_CODE_RESPONSE;
    resp->identifier = id;
    resp->length = host_to_be16(*len);
    pos = (u8 *)(resp + 1);
    *pos = EAP_TYPE_NOTIFICATION;

    return (u8 *) resp;
}


static void eap_sm_parseEapReq(struct eap_sm *sm, u8 *req, wpa_size_t len)
{
    struct eap_hdr *hdr;
    wpa_size_t plen;
    const u8 *pos = NULL;

    sm->rxReq = sm->rxSuccess = sm->rxFailure = FALSE;
    sm->reqId = 0;

    if (req == NULL || len < sizeof(*hdr)) {
        return;
    }

    hdr = (struct eap_hdr *) req;
    plen = be_to_host16(hdr->length);
    if (plen > len) {
        wpa_printf(TRACE_GROUP_1, "EAP: Ignored truncated EAP-Packet "
                   "(len=%u plen=%u)\n",
                   (unsigned long) len, (unsigned long) plen);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_IGNORE_TRUNCATED_PACKET,(unsigned long) len, (unsigned long) plen);
        return;
    }

    sm->reqId = hdr->identifier;

    if (sm->workaround) {
        /*
        Supc_MD5Init(&context);
        Supc_MD5Update(&context, req, len);
        Supc_MD5Final(sm->req_md5, &context);
        */
        md5(sm->req_md5, req, len);
    }

    switch (hdr->code) {
        case EAP_CODE_REQUEST:
            sm->reqMethod = EAP_TYPE_NONE; //add for WiFi UE enhancement
            sm->reqVendor = 0; // only in wps, this field is meaning
            sm->rxReq = TRUE;
            pos = (const u8 *)(hdr + 1);
            wpa_s->rx_identity = 0; //add for WiFi UE enhancement
            if (plen > sizeof(*hdr)) {
                sm->reqMethod = (EapType) * pos; /* Jau add casting to avoid warning */
                pos ++;
            }
            if (sm->reqMethod == EAP_TYPE_EXPANDED) {
                if (plen < sizeof(*hdr) + 8) {
                    wpa_printf(MSG_DEBUG, "EAP: Ignored truncated "
                               "expanded EAP-Packet (plen=%lu)\n",
                               (unsigned long) plen);
//              kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_IGNORE_TRUNCATED_EXPANDED_PACKET,(unsigned long) plen);
                    return;
                }
                sm->reqVendor = WPA_GET_BE24(pos);
                pos += 3;
                sm->reqVendorMethod = WPA_GET_BE32(pos);
            }
            wpa_printf(MSG_DEBUG, "EAP: Received EAP-Request id=%d "
                       "method=%u vendor=%u vendorMethod=%u\n",
                       sm->reqId, sm->reqMethod, sm->reqVendor,
                       sm->reqVendorMethod);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_RECV_REQ, sm->reqId, sm->reqMethod, sm->reqVendor,sm->reqVendorMethod);
            break;
        case EAP_CODE_RESPONSE:
            sm->reqMethod = EAP_TYPE_NONE; //add for WiFi UE enhancement
            wpa_s->rx_identity = 0; //add for WiFi UE enhancement
            if (sm->selectedMethod == EAP_TYPE_LEAP) {
                sm->rxResp = TRUE;
                if (plen > sizeof(*hdr)) {
                    sm->reqMethod = (EapType) * ((u8 *)(hdr + 1));    /* Jau add casting to avoid warning */
                }
                wpa_printf(TRACE_GROUP_1, "EAP: Received EAP-Response for "
                           "LEAP method=%d id=%d\n",
                           sm->reqMethod, sm->reqId);
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_RECV_LEAP_RSP,sm->reqMethod, sm->reqId);
                break;
            }
            wpa_printf(TRACE_GROUP_1, "EAP: Ignored EAP-Response\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_IGNORE_EAP_RSP);
            break;
        case EAP_CODE_SUCCESS:
            wpa_s->rx_identity = 0; //add for WiFi UE enhancement
            wpa_printf(TRACE_GROUP_1, "EAP: Received EAP-Success\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_RECV_SUCCESS);
            sm->rxSuccess = TRUE;
            break;
        case EAP_CODE_FAILURE:
            //add for WiFi UE enhancement
            wpa_printf(MSG_DEBUG, "eap_sm_parseEapReq : reqMethod %d\n", sm->reqMethod);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_PARSE_EAPREQ, sm->reqMethod);
            if (wpa_s->rx_identity == 1) {
                wpa_s->rx_identity = 0;
                wpa_s->eap_fail_cause = SUPC_ABM_WRONG_USER_INFO;
                wpa_printf(TRACE_GROUP_1, "EAP_CODE_FAILURE : Eeap_fail_cause : SUPC_ABM_WRONG_USER_INFO\n");
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_FAIL_CAUSE_WRONG_USER_INFO);
            } else if (wpa_s->phase2_nak == 1) {
                wpa_s->phase2_nak = 0;
                wpa_s->eap_fail_cause = SUPC_ABM_UNSUPPORTED_EAP_TYPE;
                wpa_printf(TRACE_GROUP_1, "EAP_CODE_FAILURE : eap_fail_cause : SUPC_ABM_UNSUPPORTED_EAP_TYPE\n");
//          kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_FAIL_CAUSE_UNSUPPORTED_EAP);
            }

            wpa_printf(TRACE_GROUP_1, "EAP: Received EAP-Failure\n");
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_RECV_FAILURE);
            sm->rxFailure = TRUE;
            break;
        default:
            sm->reqMethod = EAP_TYPE_NONE; //add for WiFi UE enhancement
            wpa_s->rx_identity = 0; //add for WiFi UE enhancement
            wpa_printf(TRACE_GROUP_1, "EAP: Ignored EAP-Packet with unknown "
                       "code %d\n", hdr->code);
//      kal_trace(DBG_EAP_SM,INFO_SUPC_EAP_IGNORE_UNKNOWN_CODE, hdr->code);
            break;
    }
}


struct eap_sm *eap_sm_init(void *sm_ctx, struct eapol_callbacks *eapol_cb,
                           void *msg_ctx)
{
    struct eap_sm *sm;
#ifndef WIFI_AUTH_PSK_ONLY
#ifdef __CERTMAN_SUPPORT__
    kal_int32     err = SEC_ERROR_NONE;
#endif
#endif
    // struct eapol_ctx* p = (struct eapol_ctx*)ctx;
    sm = SUPC_ALLOC_BUFF(sizeof(*sm));
    kal_mem_set(sm, 0, sizeof(*sm));
    sm->eapol_ctx = sm_ctx;
    sm->eapol_cb = eapol_cb;
    sm->msg_ctx = msg_ctx;
    sm->ClientTimeout = 60;

    /* Joseph Csolution Modification */
    /*sm->ssl_ctx = tls_init();
    if (sm->ssl_ctx == NULL) {
        wpa_printf(MSG_WARNING, "SSL: Failed to initialize TLS "
        "context.");
        SUPC_DEALLOC_BUFF(sm);
        return NULL;
    }
    */
#ifndef WIFI_AUTH_PSK_ONLY
#ifdef __CERTMAN_SUPPORT__

    /*
     MAUI_03055418 preauth eapol & eapol use the same ssl context
     err = supc_ssl_init((ssl_ctx**)&(sm->globalCtx),
                         (supc_ssl_SessionDB **)&(sm->sessionDB));
     */
    if (wpa_s->support_pre_auth) {
        if (wpa_s->g_ssl_ctx == NULL) {
            wpa_printf(MSG_WARNING, "SSL: Failed to initialize TLS "
                       "context.\n");
            kal_trace(DBG_EAP_SM, INFO_SUPC_EAP_INIT_TLS_CTX_FAIL);

            SUPC_DEALLOC_BUFF(sm);
            return NULL;
        } else {
            sm->globalCtx = wpa_s->g_ssl_ctx;
            sm->sessionDB = wpa_s->g_ssl_db;
        }
    } else {
        err = supc_ssl_init((ssl_ctx **) & (sm->globalCtx),
                            (supc_ssl_SessionDB **) & (sm->sessionDB));
        if (err != 0) {
            wpa_printf(MSG_WARNING, "SSL: Failed to initialize TLS "
                       "context.\n");
            kal_trace(DBG_EAP_SM, INFO_SUPC_EAP_INIT_TLS_CTX_FAIL);
            SUPC_DEALLOC_BUFF(sm);
            return NULL;
        }

    }
#endif
#endif
    return sm;
}


void eap_sm_deinit(struct eap_sm *sm)
{
#if 0 /* remove session DB */
    kal_int32             err = SEC_ERROR_NONE;
#endif /* remove session DB */

    if (sm == NULL) {
        return;
    }
    eap_deinit_prev_method(sm, "EAP deinit");
    SUPC_DEALLOC_BUFF(sm->lastRespData);
    SUPC_DEALLOC_BUFF(sm->eapRespData);
    SUPC_DEALLOC_BUFF(sm->eapKeyData);
    sm->lastRespData = NULL;
    sm->eapRespData = NULL;
    sm->eapKeyData = NULL;

#ifndef  WIFI_AUTH_PSK_ONLY
#ifdef __CERTMAN_SUPPORT__

    /*
    MAUI_03055418 preauth eapol & eapol use the same ssl context*/
    if (!wpa_s->support_pre_auth) {
        sec_ssl_ctx_free((ssl_ctx *)(sm->globalCtx));
    }


    // sec_ssl_ctx_free((ssl_ctx *) (sm->globalCtx) );
#endif
#endif

#if 0 /* remove session DB */
    err = supc_ssl_DestroySessionDB((supc_ssl_SessionDB **) & (sm->sessionDB));
    if (err != SEC_ERROR_NONE) {
        supc_ssl_error("sslrad_DestroySessionDB", err, 0);
    }
#endif /* remove session DB */

    //tls_deinit(sm->ssl_ctx);
    SUPC_DEALLOC_BUFF(sm);
    sm = NULL;
}


int eap_sm_step(struct eap_sm *sm)
{
    int res = 0;
    do {
        sm->changed = FALSE;
        SM_STEP_RUN(EAP);
        if (sm->changed) {
            res = 1;
        }
    } while (sm->changed);
    return res;
}


void eap_sm_abort(struct eap_sm *sm)
{
    /* release system resources that may have been allocated for the
     * authentication session */
    if (sm->eapRespData) {
        SUPC_DEALLOC_BUFF(sm->eapRespData);
        sm->eapRespData = NULL;
    }
    if (sm->eapKeyData) {
        SUPC_DEALLOC_BUFF(sm->eapKeyData);
        sm->eapKeyData = NULL;
    }
}

#if 0
static const char *eap_sm_state_txt(int state)
{
    switch (state) {
        case EAP_INITIALIZE:
            return "INITIALIZE";
        case EAP_DISABLED:
            return "DISABLED";
        case EAP_IDLE:
            return "IDLE";
        case EAP_RECEIVED:
            return "RECEIVED";
        case EAP_GET_METHOD:
            return "GET_METHOD";
        case EAP_METHOD:
            return "METHOD";
        case EAP_SEND_RESPONSE:
            return "SEND_RESPONSE";
        case EAP_DISCARD:
            return "DISCARD";
        case EAP_IDENTITY:
            return "IDENTITY";
        case EAP_NOTIFICATION:
            return "NOTIFICATION";
        case EAP_RETRANSMIT:
            return "RETRANSMIT";
        case EAP_SUCCESS:
            return "SUCCESS";
        case EAP_FAILURE:
            return "FAILURE";
        default:
            return "UNKNOWN";
    }
}
#endif

static const char *eap_sm_method_state_txt(int state)
{
    switch (state) {
        case METHOD_NONE:
            return "NONE";
        case METHOD_INIT:
            return "INIT";
        case METHOD_CONT:
            return "CONT";
        case METHOD_MAY_CONT:
            return "MAY_CONT";
        case METHOD_DONE:
            return "DONE";
        default:
            return "UNKNOWN";
    }
}


static const char *eap_sm_decision_txt(int decision)
{
    switch (decision) {
        case DECISION_FAIL:
            return "FAIL";
        case DECISION_COND_SUCC:
            return "COND_SUCC";
        case DECISION_UNCOND_SUCC:
            return "UNCOND_SUCC";
        default:
            return "UNKNOWN";
    }
}


#if 0
int eap_sm_get_status(struct eap_sm *sm, char *buf, wpa_size_t buflen, int verbose)
{
    wpa_size_t len;

    if (sm == NULL) {
        return 0;
    }

    len = snprintf(buf, buflen,
                   "EAP state=%s\n",
                   eap_sm_state_txt(sm->EAP_state));

    if (sm->selectedMethod != EAP_TYPE_NONE) {
        const char *name;
        if (sm->m) {
            name = sm->m->name;
        } else {
            const struct eap_method *m =
                eap_sm_get_eap_methods(sm->selectedMethod);
            if (m) {
                name = m->name;
            } else {
                name = "?";
            }
        }
        len += snprintf(buf + len, buflen - len,
                        "selectedMethod=%d (EAP-%s)\n",
                        sm->selectedMethod, name);

        if (sm->m && sm->m->get_status) {
            len += sm->m->get_status(sm,
                                     sm->eap_method_priv,
                                     buf + len,
                                     buflen - len,
                                     verbose);
        }
    }

    if (verbose) {
        len += snprintf(buf + len, buflen - len,
                        "reqMethod=%d\n"
                        "methodState=%s\n"
                        "decision=%s\n"
                        "ClientTimeout=%d\n",
                        sm->reqMethod,
                        eap_sm_method_state_txt(sm->methodState),
                        eap_sm_decision_txt(sm->decision),
                        sm->ClientTimeout);
    }

    return len;
}
#endif

typedef enum { TYPE_IDENTITY, TYPE_PASSWORD, TYPE_OTP } eap_ctrl_req_type;

static void eap_sm_request(struct eap_sm *sm, struct wpa_ssid *config,
                           eap_ctrl_req_type type, char *msg, wpa_size_t msglen)
{
    char *buf;
    wpa_size_t buflen;
    int len;
    char *field;
    char *txt, *tmp;

    if (config == NULL || sm == NULL) {
        return;
    }

    switch (type) {
        case TYPE_IDENTITY:
            field = "IDENTITY";
            txt = "Identity";
            config->pending_req_identity++;
            break;
        case TYPE_PASSWORD:
            field = "PASSWORD";
            txt = "Password";
            config->pending_req_password++;
            break;
        case TYPE_OTP:
            field = "OTP";
            if (msg) {
                tmp = SUPC_ALLOC_BUFF((kal_uint16)(msglen + 3));
                if (tmp == NULL) {
                    return;
                }
                tmp[0] = '[';
                memcpy(tmp + 1, msg, msglen);
                tmp[msglen + 1] = ']';
                tmp[msglen + 2] = '\0';
                txt = tmp;
                SUPC_DEALLOC_BUFF(config->pending_req_otp);
                config->pending_req_otp = tmp;
                config->pending_req_otp_len = msglen + 3;
            } else {
                if (config->pending_req_otp == NULL) {
                    return;
                }
                txt = config->pending_req_otp;
            }
            break;
        default:
            return;
    }

    buflen = 100 + strlen(txt) + config->ssid_len;
    buf = SUPC_ALLOC_BUFF(buflen);
    if (buf == NULL) {
        return;
    }
    len = sprintf(buf, "CTRL-REQ-%s-%d:%s needed for SSID ",
                  field, config->id, txt);
    SUPC_ASSERT(buflen >= len);
    if (config->ssid && buflen > len + config->ssid_len) {
        memcpy(buf + len, config->ssid, config->ssid_len);
        len += config->ssid_len;
        buf[len] = '\0';
    }
    //wpa_msg(sm->msg_ctx, MSG_INFO, buf);
//  kal_wap_trace(MAP_MOD_SUPC,DBG_EAP_SM,"%s",buf);
    SUPC_DEALLOC_BUFF(buf);
}


void eap_sm_request_identity(struct eap_sm *sm, struct wpa_ssid *config)
{
    eap_sm_request(sm, config, TYPE_IDENTITY, NULL, 0);
}


void eap_sm_request_password(struct eap_sm *sm, struct wpa_ssid *config)
{
    eap_sm_request(sm, config, TYPE_PASSWORD, NULL, 0);
}


void eap_sm_request_otp(struct eap_sm *sm, struct wpa_ssid *config,
                        char *msg, wpa_size_t msg_len)
{
    eap_sm_request(sm, config, TYPE_OTP, msg, msg_len);
}


void eap_sm_notify_ctrl_attached(struct eap_sm *sm)
{
    struct wpa_ssid *config = eap_get_config(sm);

    if (config == NULL) {
        return;
    }

    /* Re-send any pending requests for user data since a new control
     * interface was added. This handles cases where the EAP authentication
     * starts immediately after system startup when the user interface is
     * not yet running. */
    if (config->pending_req_identity) {
        eap_sm_request_identity(sm, config);
    }
    if (config->pending_req_password) {
        eap_sm_request_password(sm, config);
    }
    if (config->pending_req_otp) {
        eap_sm_request_otp(sm, config, NULL, 0);
    }
}


u8 eap_get_type(const char *name)
{
    int i;
    for (i = 0; i < NUM_EAP_METHODS; i++) {
        if (strcmp(eap_methods[i]->name, name) == 0) {
            return eap_methods[i]->method;
        }
    }
    return EAP_TYPE_NONE;
}


static int eap_allowed_phase2_type(int type)
{
    return type != EAP_TYPE_PEAP && type != EAP_TYPE_TTLS &&
           type != EAP_TYPE_FAST;
}


u8 eap_get_phase2_type(const char *name)
{
    u8 type = eap_get_type(name);
    if (eap_allowed_phase2_type(type)) {
        return type;
    }
    return EAP_TYPE_NONE;
}


u8 *eap_get_phase2_types(struct wpa_ssid *config, wpa_size_t *count)
{
    u8 *buf, method;
    int i;

    *count = 0;
    buf = SUPC_ALLOC_BUFF(NUM_EAP_METHODS);
    if (buf == NULL) {
        return NULL;
    }

    for (i = 0; i < NUM_EAP_METHODS; i++) {
        method = eap_methods[i]->method;
        if (eap_allowed_phase2_type(method)) {
            if (method == EAP_TYPE_TLS && config &&
                    config->private_key2 == NULL) {
                continue;
            }
            buf[*count] = method;
            (*count)++;
        }
    }

    return buf;
}


void eap_set_fast_reauth(struct eap_sm *sm, int enabled)
{
    sm->fast_reauth = enabled;
}


void eap_set_workaround(struct eap_sm *sm, unsigned int workaround)
{
    sm->workaround = workaround;
}


struct wpa_ssid *eap_get_config(struct eap_sm *sm)
{
    return sm->eapol_cb->get_config(sm->eapol_ctx);
}


int eap_key_available(struct eap_sm *sm)
{
    return sm ? sm->eapKeyAvailable : 0;
}


void eap_notify_success(struct eap_sm *sm)
{
    if (sm) {
        sm->decision = DECISION_COND_SUCC;
        sm->EAP_state = EAP_SUCCESS;
    }
}


u8 *eap_get_eapKeyData(struct eap_sm *sm, wpa_size_t *len)
{
    if (sm == NULL || sm->eapKeyData == NULL) {
        *len = 0;
        return NULL;
    }

    *len = sm->eapKeyDataLen;
    return sm->eapKeyData;
}


u8 *eap_get_eapRespData(struct eap_sm *sm, wpa_size_t *len)
{
    u8 *resp;

    if (sm == NULL || sm->eapRespData == NULL) {
        *len = 0;
        return NULL;
    }

    resp = sm->eapRespData;
    *len = sm->eapRespDataLen;
    sm->eapRespData = NULL;
    sm->eapRespDataLen = 0;

    return resp;
}


void eap_register_scard_ctx(struct eap_sm *sm, void *ctx)
{
    if (sm) {
        sm->scard_ctx = ctx;
    }
}
