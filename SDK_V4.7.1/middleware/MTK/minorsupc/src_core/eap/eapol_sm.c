
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
 * WPA Supplicant / EAPOL state machines
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

//#include "kal_release.h"          /* Basic data type */
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
//#include "stacklib.h"         /* Basic type for dll, evshed, stacktimer */
//#include "event_shed.h"           /* Event scheduler */
//#include "stack_timer.h"          /* Stack timer */
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

#include "eap_i.h"
#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "wpa_supplicant.h"

#include "l2_packet.h"

#include "supc_md5.h"
#include "rc4.h"
#include "eapol_sm.h"

#include "wpa.h"
#include "supc_abm_msgs.h"
#include "config_ssid.h"
#include "wpa_supplicant.h"
#include "wpa_supplicant_i.h"
#include "supc_wpa_common.h"




static void eapol_sm_txLogoff(struct eapol_sm *sm);
static void eapol_sm_txStart(struct eapol_sm *sm);
static void eapol_sm_processKey(struct eapol_sm *sm);
static void eapol_sm_getSuppRsp(struct eapol_sm *sm);
static void eapol_sm_txSuppRsp(struct eapol_sm *sm);
static void eapol_sm_abortSupp(struct eapol_sm *sm);
static void eapol_sm_abort_cached(struct eapol_sm *sm);
// static void eapol_sm_step_timeout(void *eloop_ctx, void *timeout_ctx);

static struct eapol_callbacks eapol_cb_list;


/* Definitions for clarifying state machine implementation */
#define SM_STATE(machine, state) \
static void sm_ ## machine ## _ ## state ## _Enter(struct eapol_sm *sm, \
    int global)

#define SM_ENTRY(machine, state) \
if (!global || sm->machine ## _state != machine ## _ ## state) { \
    sm->changed = TRUE; \
} \
sm->machine ## _state = machine ## _ ## state;
#if 0
kal_wap_trace(MAP_MOD_SUPC, DBG_EAPOL_SM, "%s", "EAPOL: " #machine " entering state " #state);
#endif
#define SM_ENTER(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm, 0)
#define SM_ENTER_GLOBAL(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm, 1)

#define SM_STEP(machine) \
static void sm_ ## machine ## _Step(struct eapol_sm *sm)

#define SM_STEP_RUN(machine) sm_ ## machine ## _Step(sm)

#ifdef WIFI_PORT_TIMER
/* Port Timers state machine - implemented as a function that will be called
 * once a second as a registered event loop timeout */
static void eapol_port_timers_tick(void *timeout_ctx)
{
    struct eapol_sm *sm = wpa_s->eapol;

    sm->tick = TRUE;

    // J: Clean previous event id
    sm->event_id = NULL;

    sm->event_id = supc_start_eapol_port_timer(sm, 1);//Kinki change for the port timer

    eapol_sm_step(sm);
    //eloop_register_timeout(1, 0, eapol_port_timers_tick, eloop_ctx, sm);
}

void supc_stop_eapol_port_timer(struct eapol_sm *sm)
{
    if (NULL != sm->event_id) {
        wpa_printf(TRACE_GROUP_1, "Stop port timer event id: 0x%x\n", sm->event_id);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_STOP_PORT_TIMER, sm->event_id);
        evshed_cancel_event(wpa_timer_context_g.wpa_event_scheduler_ptr, &(sm->event_id));

        sm->event_id = NULL;
    }

}

eventid supc_start_eapol_port_timer(struct eapol_sm *sm, kal_uint32 delay)
{
    // The unit of delay is in seconds
#define EVENT_PARAM_EAPOL_TIKER         0x0c
    eventid evid = 0;
    if (delay > 0) {
        supc_stop_eapol_port_timer(sm);
        evid = (evshed_set_event((wpa_timer_context_g.wpa_event_scheduler_ptr),
                                 eapol_port_timers_tick,
                                 (void *)EVENT_PARAM_EAPOL_TIKER,
                                 (delay * KAL_TICKS_1_SEC)));   // Kinki change for the port timer (TICK=5msec)
        wpa_printf(TRACE_GROUP_1, "Start port timer event id: 0x%x\n", evid);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_START_PORT_TIMER, evid);
    }
    return evid;
}
#endif


SM_STATE(SUPP_PAE, LOGOFF)
{
    SM_ENTRY(SUPP_PAE, LOGOFF);
    eapol_sm_txLogoff(sm);
    sm->logoffSent = TRUE;
    sm->suppPortStatus = Unauthorized;
}


SM_STATE(SUPP_PAE, DISCONNECTED)
{
    SM_ENTRY(SUPP_PAE, DISCONNECTED);
    sm->sPortMode = Auto;
    sm->startCount = 0;
    sm->logoffSent = FALSE;
    sm->suppPortStatus = Unauthorized;
    sm->suppAbort = TRUE;

    sm->unicast_key_received = FALSE;
    sm->broadcast_key_received = FALSE;
#ifdef WIFI_PORT_TIMER
    sm->txStart_enable = FALSE; //Kinki change for the port timer
#endif
}


SM_STATE(SUPP_PAE, CONNECTING)
{
    int send_start = sm->SUPP_PAE_state == SUPP_PAE_CONNECTING;
    SM_ENTRY(SUPP_PAE, CONNECTING);
    /*
        // modify follwing bad code. it may cause AP fall
        // in long delay status, because AP start authen immediately.
        // this change comply with wpa_supplicant_6 standard version
        sm->startWhen = sm->startPeriod;
        sm->eapolEap = FALSE;
    */

    if (send_start) {
        sm->startWhen = sm->startPeriod;
        // increase StartCount, only in the case of
        // entering CONNECTING from CONNECTING
        sm->startCount++;
    } else {
        /*
         * Do not send EAPOL-Start immediately since in most cases,
         * Authenticator is going to start authentication immediately
         * after association and an extra EAPOL-Start is just going to
         * delay authentication. Use a short timeout to send the first
         * EAPOL-Start if Authenticator does not start authentication.
         */
#ifdef __WPS_SUPPORT__
        /* Reduce latency on starting WPS negotiation. */
        sm->startWhen = 1;
#else /* CONFIG_WPS */
        sm->startWhen = 1;
#endif /* CONFIG_WPS */
        wpa_printf(TRACE_GROUP_1, "skip a EAPOL Start, wait next time\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_SKIP_TX_START);

    }
    sm->eapolEap = FALSE;

#ifdef WIFI_PORT_TIMER
    sm->event_id = supc_start_eapol_port_timer(sm, 1); //Kinki change for the port timer *start*

    if (sm->ctx->preauth == 1) {
        // mtk80707 comply with wpa_supplicant_6 standard version
        //sm->startCount++;
        if (send_start) {
            eapol_sm_txStart(sm);
        }
    } else if (sm->ctx->preauth == 0 && !(sm->txStart_enable)) {
        sm->txStart_enable = TRUE;
    }
    //Kinki change for entering CONNECTING state from AUTHENTICATING state but startCount = 0
    else if (sm->ctx->preauth == 0 && sm->txStart_enable) {
        // mtk80707 comply with wpa_supplicant_6 standard version
        //sm->startCount++;
        if (send_start) {
            eapol_sm_txStart(sm);
        }
    } else {
        wpa_printf(TRACE_GROUP_1, "EAPOL: Error occurs when trying to "
                   "send EAPOL-start\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_TX_START_ERROR);
    }//Kinki change for the port timer *end*
#else
    //sm->startCount++;
    if (send_start) {
        eapol_sm_txStart(sm);
    }
#endif

}

SM_STATE(SUPP_PAE, AUTHENTICATING)
{
    SM_ENTRY(SUPP_PAE, AUTHENTICATING);
    sm->startCount = 0;
    sm->suppSuccess = FALSE;
    sm->suppFail = FALSE;
    sm->suppTimeout = FALSE;
    sm->keyRun = FALSE;
    sm->keyDone = FALSE;
    sm->suppStart = TRUE;
}


SM_STATE(SUPP_PAE, HELD)
{
    SM_ENTRY(SUPP_PAE, HELD);
    sm->heldWhile = sm->heldPeriod;
#ifdef WIFI_PORT_TIMER
    sm->event_id = supc_start_eapol_port_timer(sm, 1); //Kinki change for the port timer
#endif
    sm->suppPortStatus = Unauthorized;
    sm->cb_status = EAPOL_CB_FAILURE;
}


SM_STATE(SUPP_PAE, AUTHENTICATED)
{
    SM_ENTRY(SUPP_PAE, AUTHENTICATED);
    sm->suppPortStatus = Authorized;
    sm->cb_status = EAPOL_CB_SUCCESS;
}


SM_STATE(SUPP_PAE, RESTART)
{
    SM_ENTRY(SUPP_PAE, RESTART);
    sm->eapRestart = TRUE;
}


SM_STATE(SUPP_PAE, S_FORCE_AUTH)
{
    SM_ENTRY(SUPP_PAE, S_FORCE_AUTH);
    sm->suppPortStatus = Authorized;
    sm->sPortMode = ForceAuthorized;
}


SM_STATE(SUPP_PAE, S_FORCE_UNAUTH)
{
    SM_ENTRY(SUPP_PAE, S_FORCE_UNAUTH);
    sm->suppPortStatus = Unauthorized;
    sm->sPortMode = ForceUnauthorized;
    eapol_sm_txLogoff(sm);
}


SM_STEP(SUPP_PAE)
{
    if ((sm->userLogoff && !sm->logoffSent) &&
            !(sm->initialize || !sm->portEnabled)) {
        SM_ENTER_GLOBAL(SUPP_PAE, LOGOFF);
    } else if (((sm->portControl == Auto) &&
                (sm->sPortMode != sm->portControl)) ||
               sm->initialize || !sm->portEnabled) {
        SM_ENTER_GLOBAL(SUPP_PAE, DISCONNECTED);
    } else if ((sm->portControl == ForceAuthorized) &&
               (sm->sPortMode != sm->portControl) &&
               !(sm->initialize || !sm->portEnabled)) {
        SM_ENTER_GLOBAL(SUPP_PAE, S_FORCE_AUTH);
    } else if ((sm->portControl == ForceUnauthorized) &&
               (sm->sPortMode != sm->portControl) &&
               !(sm->initialize || !sm->portEnabled)) {
        SM_ENTER_GLOBAL(SUPP_PAE, S_FORCE_UNAUTH);
    } else switch (sm->SUPP_PAE_state) {
            case SUPP_PAE_UNKNOWN:
                break;
            case SUPP_PAE_LOGOFF:
                if (!sm->userLogoff) {
                    SM_ENTER(SUPP_PAE, DISCONNECTED);
                }
                break;
            case SUPP_PAE_DISCONNECTED:
                SM_ENTER(SUPP_PAE, CONNECTING);
                break;
            case SUPP_PAE_CONNECTING:
                if (sm->startWhen == 0 && sm->startCount < sm->maxStart) {
                    SM_ENTER(SUPP_PAE, CONNECTING);
                } else if (sm->startWhen == 0 &&
                           sm->startCount >= sm->maxStart &&
                           sm->portValid) {
                    SM_ENTER(SUPP_PAE, AUTHENTICATED);
                } else if (sm->eapSuccess || sm->eapFail) {
                    SM_ENTER(SUPP_PAE, AUTHENTICATING);
                } else if (sm->eapolEap) {
                    SM_ENTER(SUPP_PAE, RESTART);
                } else if (sm->startWhen == 0 &&
                           sm->startCount >= sm->maxStart &&
                           !sm->portValid) {
                    SM_ENTER(SUPP_PAE, HELD);
                }
                break;
            case SUPP_PAE_AUTHENTICATING:
                if (sm->eapSuccess && !sm->portValid &&
                        sm->conf.accept_802_1x_keys &&
                        sm->conf.required_keys == 0) {
                    wpa_printf(TRACE_GROUP_1, "EAPOL: IEEE 802.1X for "
                               "plaintext connection; no EAPOL-Key frames "
                               "required\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_NO_KEY_REQUIRED);
                    sm->portValid = TRUE;
                    if (sm->ctx->eapol_done_cb) {
                        sm->ctx->eapol_done_cb(sm->ctx->ctx);
                    }
                }
                if (sm->suppSuccess && sm->portValid) {
                    SM_ENTER(SUPP_PAE, AUTHENTICATED);
                } else if (sm->suppFail || (sm->keyDone && !sm->portValid)) {
                    SM_ENTER(SUPP_PAE, HELD);
                } else if (sm->suppTimeout) {
                    SM_ENTER(SUPP_PAE, CONNECTING);
                }
                break;
            case SUPP_PAE_HELD:
                if (sm->heldWhile == 0) {
                    SM_ENTER(SUPP_PAE, CONNECTING);
                } else if (sm->eapolEap) {
                    SM_ENTER(SUPP_PAE, RESTART);
                }
                break;
            case SUPP_PAE_AUTHENTICATED:
                if (sm->eapolEap && sm->portValid) {
                    SM_ENTER(SUPP_PAE, RESTART);
                } else if (!sm->portValid) {
                    SM_ENTER(SUPP_PAE, DISCONNECTED);
                }
                break;
            case SUPP_PAE_RESTART:
                if (!sm->eapRestart) {
                    SM_ENTER(SUPP_PAE, AUTHENTICATING);
                }
                break;
            case SUPP_PAE_S_FORCE_AUTH:
                break;
            case SUPP_PAE_S_FORCE_UNAUTH:
                break;
        }
}


SM_STATE(KEY_RX, NO_KEY_RECEIVE)
{
    SM_ENTRY(KEY_RX, NO_KEY_RECEIVE);
}


SM_STATE(KEY_RX, KEY_RECEIVE)
{
    SM_ENTRY(KEY_RX, KEY_RECEIVE);
    eapol_sm_processKey(sm);
    sm->rxKey = FALSE;
}


SM_STEP(KEY_RX)
{
    if (sm->initialize || !sm->portEnabled) {
        SM_ENTER_GLOBAL(KEY_RX, NO_KEY_RECEIVE);
    }
    switch (sm->KEY_RX_state) {
        case KEY_RX_UNKNOWN:
            break;
        case KEY_RX_NO_KEY_RECEIVE:
            if (sm->rxKey) {
                SM_ENTER(KEY_RX, KEY_RECEIVE);
            }
            break;
        case KEY_RX_KEY_RECEIVE:
            if (sm->rxKey) {
                SM_ENTER(KEY_RX, KEY_RECEIVE);
            }
            break;
    }
}


SM_STATE(SUPP_BE, REQUEST)
{
    SM_ENTRY(SUPP_BE, REQUEST);
    sm->authWhile = 0;
#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(sm); //Kinki change for the port timer
#endif
    sm->eapReq = TRUE;
    eapol_sm_getSuppRsp(sm);
}


SM_STATE(SUPP_BE, RESPONSE)
{
    SM_ENTRY(SUPP_BE, RESPONSE);
    eapol_sm_txSuppRsp(sm);
    sm->eapResp = FALSE;
}


SM_STATE(SUPP_BE, SUCCESS)
{
    SM_ENTRY(SUPP_BE, SUCCESS);
    sm->keyRun = TRUE;
    sm->suppSuccess = TRUE;

    if (eap_key_available(sm->eap)) {
        /* New key received - clear IEEE 802.1X EAPOL-Key replay
         * counter */
        sm->replay_counter_valid = FALSE;
    }

    if (sm->ctx->preauth == 1) { // Kinki change for pre-auth port timer *start*
        wpa_printf(TRACE_GROUP_1, "Pre-auth: Internal notification - "
                   "portValid=TRUE\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_PRE_AUTH_PORT_VALID);
        sm->portValid = TRUE;
    }// Kinki change for pre-auth port timer *end*

}


SM_STATE(SUPP_BE, FAIL)
{
    SM_ENTRY(SUPP_BE, FAIL);
    sm->suppFail = TRUE;
}


SM_STATE(SUPP_BE, TIMEOUT)
{
    SM_ENTRY(SUPP_BE, TIMEOUT);
    sm->suppTimeout = TRUE;
}


SM_STATE(SUPP_BE, IDLE)
{
    SM_ENTRY(SUPP_BE, IDLE);
    sm->suppStart = FALSE;
    sm->initial_req = TRUE;
}


SM_STATE(SUPP_BE, INITIALIZE)
{
    SM_ENTRY(SUPP_BE, INITIALIZE);
    eapol_sm_abortSupp(sm);
    sm->suppAbort = FALSE;
}


SM_STATE(SUPP_BE, RECEIVE)
{
    SM_ENTRY(SUPP_BE, RECEIVE);
    sm->authWhile = sm->authPeriod;
#ifdef WIFI_PORT_TIMER
    sm->event_id = supc_start_eapol_port_timer(sm, 1);  //Kinki change for the port timer
#endif
    sm->eapolEap = FALSE;
    sm->eapNoResp = FALSE;
    sm->initial_req = FALSE;
}


SM_STEP(SUPP_BE)
{
    if (sm->initialize || sm->suppAbort) {
        SM_ENTER_GLOBAL(SUPP_BE, INITIALIZE);
    } else switch (sm->SUPP_BE_state) {
            case SUPP_BE_UNKNOWN:
                break;
            case SUPP_BE_REQUEST:
                if (sm->eapResp && sm->eapNoResp) {
                    wpa_printf(MSG_DEBUG, "EAPOL: SUPP_BE REQUEST: both "
                               "eapResp and eapNoResp set?!\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_BE_RESP_NO_RESP_SET);
                }
                if (sm->eapResp) {
                    SM_ENTER(SUPP_BE, RESPONSE);
                } else if (sm->eapNoResp) {
                    SM_ENTER(SUPP_BE, RECEIVE);
                }
                break;
            case SUPP_BE_RESPONSE:
                SM_ENTER(SUPP_BE, RECEIVE);
                break;
            case SUPP_BE_SUCCESS:
                SM_ENTER(SUPP_BE, IDLE);
                break;
            case SUPP_BE_FAIL:
                SM_ENTER(SUPP_BE, IDLE);
                break;
            case SUPP_BE_TIMEOUT:
                SM_ENTER(SUPP_BE, IDLE);
                break;
            case SUPP_BE_IDLE:
                if (sm->eapFail && sm->suppStart) {
                    SM_ENTER(SUPP_BE, FAIL);
                } else if (sm->eapolEap && sm->suppStart) {
                    SM_ENTER(SUPP_BE, REQUEST);
                } else if (sm->eapSuccess && sm->suppStart) {
                    SM_ENTER(SUPP_BE, SUCCESS);
                }
                break;
            case SUPP_BE_INITIALIZE:
                SM_ENTER(SUPP_BE, IDLE);
                break;
            case SUPP_BE_RECEIVE:
                if (sm->eapolEap) {
                    SM_ENTER(SUPP_BE, REQUEST);
                } else if (sm->eapFail) {
                    SM_ENTER(SUPP_BE, FAIL);
                } else if (sm->authWhile == 0) {
                    SM_ENTER(SUPP_BE, TIMEOUT);
                } else if (sm->eapSuccess) {
                    SM_ENTER(SUPP_BE, SUCCESS);
                }
                break;
        }
}

#ifdef WIFI_PORT_TIMER
SM_STATE(PORT_TIMER, ONE_SECOND)
{
    SM_ENTRY(PORT_TIMER, ONE_SECOND);
}


SM_STATE(PORT_TIMER, TICK)
{
    SM_ENTRY(PORT_TIMER, TICK);

    if (sm->authWhile > 0) {
        sm->authWhile--;
    }
    if (sm->heldWhile > 0) {
        sm->heldWhile--;
    }
    if (sm->startWhen > 0) {
        sm->startWhen--;
    }
    if (sm->idleWhile > 0) {
        sm->idleWhile--;
    }
    wpa_printf(MSG_MSGDUMP, "EAPOL: Port Timers tick - authWhile=%d "
               "heldWhile=%d startWhen=%d idleWhile=%d\n",
               sm->authWhile, sm->heldWhile, sm->startWhen, sm->idleWhile);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_PORT_TIMER_TICK, sm->authWhile, sm->heldWhile, sm->startWhen,sm->idleWhile);

    sm->tick = FALSE;
}

SM_STEP(PORT_TIMER)
{
    if (sm->initialize) {
        SM_ENTER_GLOBAL(PORT_TIMER, ONE_SECOND);
    } else switch (sm->PORT_TIMER_state) {
            case PORT_TIMER_UNKNOWN:
                break;
            case PORT_TIMER_ONE_SECOND:
                if (sm->tick) {
                    SM_ENTER(PORT_TIMER, TICK);
                }
                break;
            case PORT_TIMER_TICK:
                SM_ENTER(PORT_TIMER, ONE_SECOND);
                break;
        }
}
#endif

static void eapol_sm_txLogoff(struct eapol_sm *sm)
{
    wpa_printf(TRACE_GROUP_1, "EAPOL: txLogoff\n");
//kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_TX_LOGOFF);
    sm->ctx->eapol_send(sm->ctx->ctx, IEEE802_1X_TYPE_EAPOL_LOGOFF,
                        (u8 *) "", 0);
    sm->dot1xSuppEapolLogoffFramesTx++;
    sm->dot1xSuppEapolFramesTx++;
}


static void eapol_sm_txStart(struct eapol_sm *sm)
{
    wpa_printf(TRACE_GROUP_1, "EAPOL: txStart\n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_TX_START);
    sm->ctx->eapol_send(sm->ctx->ctx, IEEE802_1X_TYPE_EAPOL_START,
                        (u8 *) "", 0);
    sm->dot1xSuppEapolStartFramesTx++;
    sm->dot1xSuppEapolFramesTx++;
}


#define IEEE8021X_ENCR_KEY_LEN 32
#define IEEE8021X_SIGN_KEY_LEN 32

struct eap_key_data {
    u8 encr_key[IEEE8021X_ENCR_KEY_LEN];
    u8 sign_key[IEEE8021X_SIGN_KEY_LEN];
};


static void eapol_sm_processKey(struct eapol_sm *sm)
{
    struct ieee802_1x_hdr *hdr;
    struct ieee802_1x_eapol_key *key, eapol_key;
    struct eap_key_data keydata;
    u8 orig_key_sign[IEEE8021X_KEY_SIGN_LEN], datakey[32];
    u8 ekey[IEEE8021X_KEY_IV_LEN + IEEE8021X_ENCR_KEY_LEN];
    int key_len, res, sign_key_len, encr_key_len;
    u8 *p, *key_signature;

    wpa_printf(TRACE_GROUP_1, "EAPOL: processKey\n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_PROCESS_KEY);
    if (sm->last_rx_key == NULL) {
        return;
    }

    if (!sm->conf.accept_802_1x_keys) {
        wpa_printf(TRACE_WARNING, "EAPOL: Received IEEE 802.1X EAPOL-Key"
                   " even though this was not accepted - "
                   "ignoring this packet\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_RECEIVE_KEY_IGNOR);
        return;
    }

    hdr = (struct ieee802_1x_hdr *) sm->last_rx_key;
    p = (u8 *)(hdr + 1);
    key = &eapol_key;
    key->type = *p;
    p += sizeof(key->type);
    memcpy(&key->key_length, p, sizeof(key->key_length));
    p += sizeof(key->key_length);
    memcpy(key->replay_counter, p, sizeof(key->replay_counter));
    p += sizeof(key->replay_counter);
    memcpy(key->key_iv, p, sizeof(key->key_iv));
    p += sizeof(key->key_iv);
    key->key_index = *p;
    p += sizeof(key->key_index);
    key_signature = p;
    memcpy(key->key_signature, p, sizeof(key->key_signature));
    p += sizeof(key->key_signature);
    if (sizeof(*hdr) + be_to_host16(hdr->length) > sm->last_rx_key_len) {
        wpa_printf(TRACE_WARNING, "EAPOL: Too short EAPOL-Key frame\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_TOO_SHORT_KEY);
        return;
    }
    wpa_printf(TRACE_GROUP_1, "EAPOL: RX IEEE 802.1X ver=%d type=%d len=%d "
               "EAPOL-Key: type=%d key_length=%d key_index=0x%x\n",
               hdr->version, hdr->type, be_to_host16(hdr->length),
               key->type, be_to_host16(key->key_length), key->key_index);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_KEY_INFO_DUMP,  hdr->version, hdr->type, be_to_host16(hdr->length),
//         key->type, be_to_host16(key->key_length), key->key_index);

    sign_key_len = IEEE8021X_SIGN_KEY_LEN;
    encr_key_len = IEEE8021X_ENCR_KEY_LEN;
    res = eapol_sm_get_key(sm, (u8 *) &keydata, sizeof(keydata));
    if (res < 0) {
        wpa_printf(TRACE_GROUP_1, "EAPOL: Could not get master key for "
                   "decrypting EAPOL-Key keys\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_CAN_NOT_GET_MSK_FROM_KEY_FRAME);
        return;
    }
    if (res == 16) {
        /* LEAP derives only 16 bytes of keying material. */
        res = eapol_sm_get_key(sm, (u8 *) &keydata, 16);
        if (res) {
            wpa_printf(TRACE_GROUP_1, "EAPOL: Could not get LEAP "
                       "master key for decrypting EAPOL-Key keys\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_CAN_NOT_GET_LEAP_MSK);
            return;
        }
        sign_key_len = 16;
        encr_key_len = 16;
        memcpy(keydata.sign_key, keydata.encr_key, 16);
    } else if (res) {
        wpa_printf(TRACE_GROUP_1, "EAPOL: Could not get enough master key "
                   "data for decrypting EAPOL-Key keys (res=%d)\n", res);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_CAN_NOT_GET_MSK_KEY_DATA, res);
        return;
    }

    /* The key replay_counter must increase when same master key */
    if (sm->replay_counter_valid &&
            memcmp(sm->last_replay_counter, key->replay_counter,
                   IEEE8021X_REPLAY_COUNTER_LEN) >= 0) {
        wpa_printf(TRACE_WARNING, "EAPOL: EAPOL-Key replay counter did "
                   "not increase - ignoring key\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_KEY_IGNORE_WRONG_REPLAY_COUNTER);
        wpa_hexdump(MSG_DEBUG, "EAPOL: last replay counter",
                    (char *)sm->last_replay_counter,
                    IEEE8021X_REPLAY_COUNTER_LEN);
        wpa_hexdump(MSG_DEBUG, "EAPOL: received replay counter",
                    (char *)key->replay_counter, IEEE8021X_REPLAY_COUNTER_LEN);
        return;
    }

    /* Verify key signature (HMAC-MD5) */
    memcpy(orig_key_sign, key->key_signature, IEEE8021X_KEY_SIGN_LEN);
    memset(key_signature, 0, IEEE8021X_KEY_SIGN_LEN);
    hmac_md5(keydata.sign_key, sign_key_len,
             sm->last_rx_key, sizeof(*hdr) + be_to_host16(hdr->length),
             key->key_signature);
    if (memcmp(orig_key_sign, key->key_signature, IEEE8021X_KEY_SIGN_LEN)
            != 0) {
        wpa_printf(TRACE_GROUP_1, "EAPOL: Invalid key signature in "
                   "EAPOL-Key packet\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_KEY_INVALID_SIGNATURE);
        memcpy(key->key_signature, orig_key_sign,
               IEEE8021X_KEY_SIGN_LEN);
        return;
    }
    wpa_printf(TRACE_GROUP_1, "EAPOL: EAPOL-Key key signature verified\n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_KEY_SIGNATURE_VERIFIED);

    key_len = be_to_host16(hdr->length) - IEEE8021X_EAPOL_KEY_LEN;
    if (key_len > 32 || be_to_host16(key->key_length) > 32) {
        wpa_printf(TRACE_WARNING, "EAPOL: Too long key data length %d\n",
                   key_len ? key_len : be_to_host16(key->key_length));
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_KEY_DATA_TOO_LONG, key_len ? key_len : be_to_host16(key->key_length));
        return;
    }
    if (key_len == be_to_host16(key->key_length)) {
        memcpy(ekey, key->key_iv, IEEE8021X_KEY_IV_LEN);
        memcpy(ekey + IEEE8021X_KEY_IV_LEN, keydata.encr_key,
               encr_key_len);
        memcpy(datakey, sm->last_rx_key + sizeof(ieee802_1x_hdr) + IEEE8021X_EAPOL_KEY_LEN, key_len);
        rc4(datakey, key_len, ekey,
            IEEE8021X_KEY_IV_LEN + encr_key_len);
        wpa_hexdump_key(MSG_DEBUG, "EAPOL: Decrypted(RC4) key",
                        (char *)datakey, key_len);
    } else if (key_len == 0) {
        /* IEEE 802.1X-REV specifies that least significant Key Length
         * octets from MS-MPPE-Send-Key are used as the key if the key
         * data is not present. This seems to be meaning the beginning
         * of the MS-MPPE-Send-Key. In addition, MS-MPPE-Send-Key in
         * Supplicant corresponds to MS-MPPE-Recv-Key in Authenticator.
         * Anyway, taking the beginning of the keying material from EAP
         * seems to interoperate with Authenticators. */
        key_len = be_to_host16(key->key_length);
        memcpy(datakey, keydata.encr_key, key_len);
        wpa_hexdump_key(MSG_DEBUG, "EAPOL: using part of EAP keying "
                        "material data encryption key",
                        (char *)datakey, key_len);
    } else {
        wpa_printf(TRACE_GROUP_1, "EAPOL: Invalid key data length %d "
                   "(key_length=%d)\n", key_len,
                   be_to_host16(key->key_length));
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_INVALID_LEN, key_len, be_to_host16(key->key_length));
        return;
    }

    sm->replay_counter_valid = TRUE;
    memcpy(sm->last_replay_counter, key->replay_counter,
           IEEE8021X_REPLAY_COUNTER_LEN);

    wpa_printf(TRACE_GROUP_1, "EAPOL: Setting dynamic WEP key: %s keyidx %d "
               "len %d",
               key->key_index & IEEE8021X_KEY_INDEX_FLAG ?
               "unicast" : "broadcast\n",
               key->key_index & IEEE8021X_KEY_INDEX_MASK, key_len);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_SET_DYNAMIC_WEP_KEY,key->key_index & IEEE8021X_KEY_INDEX_MASK, key_len,key->key_index & IEEE8021X_KEY_INDEX_FLAG);

    if (sm->ctx->set_wep_key &&
            sm->ctx->set_wep_key(sm->ctx->ctx,
                                 key->key_index & IEEE8021X_KEY_INDEX_FLAG,
                                 key->key_index & IEEE8021X_KEY_INDEX_MASK,
                                 datakey, key_len) < 0) {

        wpa_printf(TRACE_WARNING, "EAPOL: Failed to set WEP key to the "
                   " driver.\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_SET_WEP2DRV_FAIL);
    } else {
        if (key->key_index & IEEE8021X_KEY_INDEX_FLAG) {
            sm->unicast_key_received = TRUE;
        } else {
            sm->broadcast_key_received = TRUE;
        }

        if ((sm->unicast_key_received ||
                !(sm->conf.required_keys & EAPOL_REQUIRE_KEY_UNICAST)) &&
                (sm->broadcast_key_received ||
                 !(sm->conf.required_keys & EAPOL_REQUIRE_KEY_BROADCAST))) {
            wpa_printf(TRACE_GROUP_1, "EAPOL: all required EAPOL-Key "
                       "frames received\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_ALL_KEY_RECEIVED);
            sm->portValid = TRUE;
            if (sm->ctx->eapol_done_cb) {
                sm->ctx->eapol_done_cb(sm->ctx->ctx);
            }
        }
    }
}


static void eapol_sm_getSuppRsp(struct eapol_sm *sm)
{
//  wpa_printf(MSG_DEBUG, "EAPOL: getSuppRsp");
    /* EAP layer processing; no special code is needed, since Supplicant
     * Backend state machine is waiting for eapNoResp or eapResp to be set
     * and these are only set in the EAP state machine when the processing
     * has finished. */
}


static void eapol_sm_txSuppRsp(struct eapol_sm *sm)
{
    u8 *resp;
    wpa_size_t resp_len;

    wpa_printf(TRACE_GROUP_1, "EAPOL: txSuppRsp\n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_TX_SUPPRSP);
    resp = eap_get_eapRespData(sm->eap, &resp_len);
    if (resp == NULL) {
        wpa_printf(TRACE_WARNING, "EAPOL: txSuppRsp - EAP response data "
                   "not available\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_TX_SUPPRSP_NO_RSP_DATA);
        return;
    }

    /* Send EAP-Packet from the EAP layer to the Authenticator */
    sm->ctx->eapol_send(sm->ctx->ctx, IEEE802_1X_TYPE_EAP_PACKET,
                        resp, resp_len);

    /* eapRespData is not used anymore, so free it here */
    SUPC_DEALLOC_BUFF(resp);

    if (sm->initial_req) {
        sm->dot1xSuppEapolReqIdFramesRx++;
    } else {
        sm->dot1xSuppEapolReqFramesRx++;
    }
    sm->dot1xSuppEapolRespFramesTx++;
    sm->dot1xSuppEapolFramesTx++;
}


static void eapol_sm_abortSupp(struct eapol_sm *sm)
{
    /* release system resources that may have been allocated for the
     * authentication session */
    if (sm->last_rx_key != NULL) {
        SUPC_DEALLOC_BUFF(sm->last_rx_key);
        sm->last_rx_key = NULL;
    }
    if (sm->eapReqData != NULL) {
        SUPC_DEALLOC_BUFF(sm->eapReqData);
        sm->eapReqData = NULL;
    }
    eap_sm_abort(sm->eap);
}


struct eapol_sm *eapol_sm_init(struct eapol_ctx *ctx)
{
    struct eapol_sm *sm;
    sm = SUPC_ALLOC_BUFF(sizeof(*sm));
    kal_mem_set(sm, 0, sizeof(*sm));
    sm->ctx = ctx;

    sm->portControl = Auto;

    /* Supplicant PAE state machine */
    sm->heldPeriod = 60;

    /*eapol_start's period*/
    sm->startPeriod = 2;

    sm->maxStart = 3; //Kinki change for the port timer

    /* Supplicant Backend state machine */
    /* mtk80707, change form 2 to 30, learn form wpa_supplicant
       official version.
    */
    sm->authPeriod = 30;

    sm->eap = eap_sm_init(sm, &eapol_cb_list, ctx->msg_ctx);
    if (sm->eap == NULL) {
        SUPC_DEALLOC_BUFF(sm);
        return NULL;
    }
    sm->eap->wps = NULL;
#ifdef __WPS_SUPPORT__
    if (NULL != ctx->wps || NULL != wpa_s->wps) {
        sm->eap->wps = ctx->wps;
        wpa_printf(TRACE_GROUP_1, "in eap, set ctx->wps=%x, wpa_s->wps=%x\n",
                   ctx->wps, wpa_s->wps);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_WPS_CTX_INFO,ctx->wps, wpa_s->wps);
    } else {
        wpa_printf(MSG_DEBUG, "in eap, set sm->eap->wps = NULL\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_WPS_CTX_NULL);
        sm->eap->wps = NULL;
    }
#endif

#ifdef WIFI_PORT_TIMER
    sm->tick = FALSE; //Kinki change for the port timer
    sm->event_id = NULL; //Kinki change for the port timer
#endif
    /* Initialize EAPOL state machines */
    sm->initialize = TRUE;
    eapol_sm_step(sm);
    sm->initialize = FALSE;
    eapol_sm_step(sm);
    return sm;
}


void eapol_sm_deinit(struct eapol_sm *sm)
{
    if (sm == NULL) {
        return;
    }
    //eloop_cancel_timeout(eapol_sm_step_timeout, NULL, sm);
    //eloop_cancel_timeout(eapol_port_timers_tick, NULL, sm);
#ifdef WIFI_PORT_TIMER
    supc_stop_eapol_port_timer(sm); // Kinki change for the port timer
    //sm->event_id = NULL;
#endif
    eap_sm_deinit(sm->eap);
    sm->eap = NULL;
    SUPC_DEALLOC_BUFF(sm->last_rx_key);
    SUPC_DEALLOC_BUFF(sm->eapReqData);
    SUPC_DEALLOC_BUFF(sm->ctx);
    sm->last_rx_key = NULL;
    sm->eapReqData = NULL;
    sm->ctx = NULL;
    SUPC_DEALLOC_BUFF(sm);
}

#if 0
static void eapol_sm_step_timeout(void *eloop_ctx, void *timeout_ctx)
{
    eapol_sm_step(timeout_ctx);
}
#endif

void eapol_sm_step(struct eapol_sm *sm)
{
    int i;

    /* In theory, it should be ok to run this in loop until !changed.
    * However, it is better to use a limit on number of iterations to
    * allow events (e.g., SIGTERM) to stop the program cleanly if the
    * state machine were to generate a busy loop. */

    /* Joseph:
    * By now, I have no idea why they should be run more than one time, I'll
    * remove the for loop here and let the codes to run only one time and add
    * assert to observe.
    */
    /* Joseph, change to 20 times. I found it should be running more than once
     * to make effect
     */
    for (i = 0; i < 20; i++) {
        sm->changed = FALSE;
        SM_STEP_RUN(SUPP_PAE);
        SM_STEP_RUN(KEY_RX);
        SM_STEP_RUN(SUPP_BE);
#ifdef WIFI_PORT_TIMER
        SM_STEP_RUN(PORT_TIMER);
#endif
        if (eap_sm_step(sm->eap)) {
            sm->changed = TRUE;
        }
    }

    /* Joseph:
     * The registered timer is a periodic trigger of the state machine. We remove the trigger
     * here and call this sm_step every time a l2 packet of this state machine is processed.
     */
    //if (sm->changed)
    //{
    /* restart EAPOL state machine step from timeout call in order
     * to allow other events to be processed. */
    //    eloop_cancel_timeout(eapol_sm_step_timeout, NULL, sm);
    //    eloop_register_timeout(0, 0, eapol_sm_step_timeout, NULL, sm);
    //}


    if (sm->ctx->cb && sm->cb_status != EAPOL_CB_IN_PROGRESS) {
        int success = sm->cb_status == EAPOL_CB_SUCCESS ? 1 : 0;
        sm->cb_status = EAPOL_CB_IN_PROGRESS;
        sm->ctx->cb(sm, success, sm->ctx->cb_ctx);
    }
}

/* Karen:
  * We may use it later
  */
/*
static const char *eapol_supp_pae_state(int state)
{
    switch (state) {
    case SUPP_PAE_LOGOFF:
        return "LOGOFF";
    case SUPP_PAE_DISCONNECTED:
        return "DISCONNECTED";
    case SUPP_PAE_CONNECTING:
        return "CONNECTING";
    case SUPP_PAE_AUTHENTICATING:
        return "AUTHENTICATING";
    case SUPP_PAE_HELD:
        return "HELD";
    case SUPP_PAE_AUTHENTICATED:
        return "AUTHENTICATED";
    case SUPP_PAE_RESTART:
        return "RESTART";
    default:
        return "UNKNOWN";
    }
}


static const char *eapol_supp_be_state(int state)
{
    switch (state) {
    case SUPP_BE_REQUEST:
        return "REQUEST";
    case SUPP_BE_RESPONSE:
        return "RESPONSE";
    case SUPP_BE_SUCCESS:
        return "SUCCESS";
    case SUPP_BE_FAIL:
        return "FAIL";
    case SUPP_BE_TIMEOUT:
        return "TIMEOUT";
    case SUPP_BE_IDLE:
        return "IDLE";
    case SUPP_BE_INITIALIZE:
        return "INITIALIZE";
    case SUPP_BE_RECEIVE:
        return "RECEIVE";
    default:
        return "UNKNOWN";
    }
}


static const char * eapol_port_status(PortStatus status)
{
    if (status == Authorized)
        return "Authorized";
    else
        return "Unauthorized";
}
*/

static const char *eapol_port_control(PortControl ctrl)
{
    switch (ctrl) {
        case Auto:
            return "Auto";
        case ForceUnauthorized:
            return "ForceUnauthorized";
        case ForceAuthorized:
            return "ForceAuthorized";
        default:
            return "Unknown";
    }
}

/*
void eapol_sm_configure(struct eapol_sm *sm, int heldPeriod, int authPeriod,
            int startPeriod, int maxStart)
{
    if (sm == NULL)
        return;
    if (heldPeriod >= 0)
        sm->heldPeriod = heldPeriod;
    if (authPeriod >= 0)
        sm->authPeriod = authPeriod;
    if (startPeriod >= 0)
        sm->startPeriod = startPeriod;
    if (maxStart >= 0)
        sm->maxStart = maxStart;
}

int eapol_sm_get_status(struct eapol_sm *sm, char *buf, wpa_size_t buflen,
            int verbose)
{
    int len;
    if (sm == NULL)
        return 0;

    len = snprintf(buf, buflen,
               "Supplicant PAE state=%s\n"
               "suppPortStatus=%s\n",
               eapol_supp_pae_state(sm->SUPP_PAE_state),
               eapol_port_status(sm->suppPortStatus));

    if (verbose) {
        len += snprintf(buf + len, buflen - len,
               "heldPeriod=%d\n"
               "authPeriod=%d\n"
               "startPeriod=%d\n"
               "maxStart=%d\n"
               "portControl=%s\n"
               "Supplicant Backend state=%s\n",
               sm->heldPeriod,
               sm->authPeriod,
               sm->startPeriod,
               sm->maxStart,
               eapol_port_control(sm->portControl),
               eapol_supp_be_state(sm->SUPP_BE_state));
    }

    len += eap_sm_get_status(sm->eap, buf + len, buflen - len, verbose);

    return len;
}
*/


void eapol_sm_rx_eapol(struct eapol_sm *sm, u8 *src, u8 *buf, wpa_size_t len)
{
    struct ieee802_1x_hdr *hdr;
    struct ieee802_1x_eapol_key *key;
    int plen, data_len;
    Boolean preauth = FALSE;

//    wpa_printf(TRACE_GROUP_1, "in eap.lib, wpa_s: %x eapol: %x, but sm: %x\n", wpa_s, wpa_s->eapol, sm);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_WAP_CTX_INFO, wpa_s, wpa_s->eapol, sm);
    //wpa_printf(MSG_DEBUG, "in eap lib, sizeof wpa_s: %d ",
    //sizeof(struct wpa_supplicant));
//kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_SIZE_OF_SUPC_STRUCT,sizeof(struct wpa_supplicant));
    //wpa_printf(MSG_DEBUG, "offset drv_priv: %d",
    //((int)(&((wpa_supplicant *)0)->drv_priv)));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_OFFSET_OF_DRV_PRIV,((int)(&((wpa_supplicant *)0)->drv_priv)));
    //wpa_printf(MSG_DEBUG, "offset eapol: %d",
    //((int)(&((wpa_supplicant *)0)->eapol)));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_OFFSET_OF_EAPOL,((int)(&((wpa_supplicant *)0)->eapol)));

    if (sm == NULL) {
        return;
    }
    sm->dot1xSuppEapolFramesRx++;
    if (len < sizeof(*hdr)) {
        sm->dot1xSuppInvalidEapolFramesRx++;
        return;
    }
    hdr = (struct ieee802_1x_hdr *) buf;
    sm->dot1xSuppLastEapolFrameVersion = hdr->version;
    memcpy(sm->dot1xSuppLastEapolFrameSource, src, ETH_ALEN);
    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    plen = be_to_host16(hdr->length);
    if (plen > len - sizeof(*hdr)) {
        sm->dot1xSuppEapLengthErrorFramesRx++;
        return;
    }
    data_len = plen + sizeof(*hdr);

    switch (hdr->type) {
        case IEEE802_1X_TYPE_EAP_PACKET:
            if (sm->cached_pmk) {
                /* Trying to use PMKSA caching, but Authenticator did
                 * not seem to have a matching entry. Need to restart
                 * EAPOL state machines.
                 */
#ifdef WIFI_PORT_TIMER
                if (wpa_s->cur_pmksa) { //Kinki change for the port timer *start*
                    wpa_printf(TRACE_GROUP_1, "RSN: Authenticator did not accept PMKID, cancelled PMKSA caching attempt\n");
//              kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_AC_CANCEL_PMKSA_ATTEMP);
                    wpa_s->cur_pmksa = NULL;
                }  //Kinki change for the port timer *end*
#endif
                eapol_sm_abort_cached(sm);
            }
            SUPC_DEALLOC_BUFF(sm->eapReqData);
            sm->eapReqDataLen = plen;
            sm->eapReqData = SUPC_ALLOC_BUFF(sm->eapReqDataLen);
            if (sm->eapReqData) {
                wpa_printf(TRACE_GROUP_1, "EAPOL: Received EAP-Packet "
                           "frame\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_RX_EAP);
                memcpy(sm->eapReqData, (u8 *)(hdr + 1),
                       sm->eapReqDataLen);
                sm->eapolEap = TRUE;
                if (sm->ctx->preauth == 1) {
                    preauth = TRUE;
                }
                eapol_sm_step(sm);
            }
            wpa_printf(TRACE_GROUP_1, "after eapol_sm process, handle other cases.\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_HANDLE_OTHER_CASE);

#ifdef WIFI_BLACKLIST
            if (!preauth && eapol_get_bool(sm->eap, EAPOL_eapFail) == TRUE) { // Kinki change for the blacklist
                // pre-auth fail will not add into blacklist
#ifdef WIFI_PORT_TIMER
                supc_stop_eapol_port_timer(wpa_s->eapol); // Kinki change for the port timer, stop port timer before disjoin
#endif
#ifdef __WPS_SUPPORT__
                /*if EAP_FAIL caused by WSC M2D or WSC DONE, skip blacklist.*/
                if (wpa_s->wps_eapol_cb_ret || wpa_s->wps_success) {
                    wpa_printf(TRACE_GROUP_1, "wps_eapol_cb handle it or wsc done, ignore EAP_Fail handler\n");
//              kal_trace(DBG_EAPOL_SM,INFO_SUPC_WPS_IGNORE_EAP_FAIL);
                    goto skip;
                }
#endif

                if (!wpa_listed_success(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id)) { // Kinki add for MAUI_00401655
                    wpa_auth_timeout_list_del(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
                    wpa_blacklist_add_permanent(wpa_s, wpa_s->bssid, wpa_s->current_ssid->profile_id);
                    supc_blacklist_update(wpa_s);
                }

                pmksa_cache_del(wpa_s->bssid);
                wpa_supplicant_disassociate(wpa_s, REASON_DEAUTH_LEAVING);
                wpa_s->reassociate = 1;
                wpa_s->wpa_state = WPA_SCANNING;
                wpa_supplicant_req_scan(wpa_s, 0, 0); // change it to 0!!
            }
#endif
#ifdef __WPS_SUPPORT__
skip:
#endif
            break;
        case IEEE802_1X_TYPE_EAPOL_KEY:
            if (plen < IEEE8021X_EAPOL_KEY_LEN) {
                wpa_printf(TRACE_GROUP_1, "EAPOL: Too short EAPOL-Key "
                           "frame received\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_KEY_TOO_SHORT);
                break;
            }
            key = (struct ieee802_1x_eapol_key *)(hdr + 1);
            if (key->type == EAPOL_KEY_TYPE_WPA ||
                    key->type == EAPOL_KEY_TYPE_RSN) {
                /* WPA Supplicant takes care of this frame. */
                wpa_printf(TRACE_GROUP_1, "EAPOL: Ignoring WPA EAPOL-Key "
                           "frame in EAPOL state machines\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_IGNORE_WPA_KEY);

                break;
            }
            if (key->type != EAPOL_KEY_TYPE_RC4) {
                wpa_printf(TRACE_GROUP_1, "EAPOL: Ignored unknown "
                           "EAPOL-Key type %d\n", key->type);
//kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_IGNORE_UNKNOWN_KEY, key->type);
                break;
            }
            SUPC_DEALLOC_BUFF(sm->last_rx_key);
            sm->last_rx_key = SUPC_ALLOC_BUFF(data_len);
            if (sm->last_rx_key) {
                wpa_printf(TRACE_GROUP_1, "EAPOL: Received EAPOL-Key "
                           "frame\n");
//          kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_KEY_FRAME);
                memcpy(sm->last_rx_key, buf, data_len);
                sm->last_rx_key_len = data_len;
                sm->rxKey = TRUE;
                eapol_sm_step(sm);
            }
            break;
        default:
            wpa_printf(TRACE_GROUP_1, "EAPOL: Received unknown EAPOL type %d\n",
                       hdr->type);
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_RX_EAPOL_UNKNOWN, hdr->type);
            sm->dot1xSuppInvalidEapolFramesRx++;
            break;
    }
}


void eapol_sm_notify_tx_eapol_key(struct eapol_sm *sm)
{
    if (sm) {
        sm->dot1xSuppEapolFramesTx++;
    }
}


void eapol_sm_notify_portEnabled(struct eapol_sm *sm, Boolean enabled)
{
    if (sm == NULL) {
        return;
    }
    wpa_printf(TRACE_GROUP_1, "EAPOL: External notification - "
               "portEnabled=%d\n", enabled);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_PORT_ENABLE, enabled);
    sm->portEnabled = enabled;
    eapol_sm_step(sm);
}


void eapol_sm_notify_portValid(struct eapol_sm *sm, Boolean valid)
{
    if (sm == NULL) {
        return;
    }
    wpa_printf(TRACE_GROUP_1, "EAPOL: External notification - "
               "portValid=%d\n", valid);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_PORT_VALID, valid);
    sm->portValid = valid;
    eapol_sm_step(sm);
}


void eapol_sm_notify_eap_success(struct eapol_sm *sm, Boolean success)
{
    if (sm == NULL) {
        return;
    }
    wpa_printf(TRACE_GROUP_1, "EAPOL: External notification - "
               "EAP success=%d\n", success);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_EAP_SUCCESS, success);
    sm->eapSuccess = success;
    sm->altAccept = success;
    if (success) {
        eap_notify_success(sm->eap);
    }
    eapol_sm_step(sm);
}


void eapol_sm_notify_eap_fail(struct eapol_sm *sm, Boolean fail)
{
    if (sm == NULL) {
        return;
    }
    wpa_printf(TRACE_GROUP_1, "EAPOL: External notification - "
               "EAP fail=%d\n", fail);
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_EAP_FAIL, fail);
    sm->eapFail = fail;
    sm->altReject = fail;
    eapol_sm_step(sm);
}


void eapol_sm_notify_config(struct eapol_sm *sm, struct wpa_ssid *config,
                            struct eapol_config *conf)
{
    if (sm == NULL) {
        return;
    }

    sm->config = config;

    if (conf == NULL) {
        return;
    }

    sm->conf.accept_802_1x_keys = conf->accept_802_1x_keys;
    sm->conf.required_keys = conf->required_keys;
    sm->conf.fast_reauth = conf->fast_reauth;
    if (sm->eap) {
        eap_set_fast_reauth(sm->eap, conf->fast_reauth);
        eap_set_workaround(sm->eap, conf->workaround);
    }
}


int eapol_sm_get_key(struct eapol_sm *sm, u8 *key, wpa_size_t len)
{
    u8 *eap_key;
    wpa_size_t eap_len;

    if (sm == NULL || !eap_key_available(sm->eap)) {
        return -1;
    }
    eap_key = eap_get_eapKeyData(sm->eap, &eap_len);
    if (eap_key == NULL) {
        return -1;
    }
    if (len > eap_len) {
        return eap_len;
    }
    kal_mem_cpy(key, eap_key, len);
    return 0;
}


void eapol_sm_notify_logoff(struct eapol_sm *sm, Boolean logoff)
{
    if (sm) {
        sm->userLogoff = logoff;
        eapol_sm_step(sm);
    }
}


void eapol_sm_notify_cached(struct eapol_sm *sm)
{
    if (sm == NULL) {
        return;
    }
    sm->SUPP_PAE_state = SUPP_PAE_AUTHENTICATED;
    sm->suppPortStatus = Authorized;
    eap_notify_success(sm->eap);
}


void eapol_sm_notify_pmkid_attempt(struct eapol_sm *sm, int attempt)
{
    if (sm == NULL) {
        return;
    }
    if (attempt) {
        wpa_printf(TRACE_GROUP_1, "RSN: Trying to use cached PMKSA\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_PMKID_ATTEMPT);
        sm->cached_pmk = TRUE;
    } else {
        wpa_printf(TRACE_GROUP_1, "RSN: Do not try to use cached PMKSA\n");
//      kal_trace(DBG_EAPOL_SM,INFO_SUPC_NOTIFY_NO_PMKID_ATTEMPT);
        sm->cached_pmk = FALSE;
    }
}


static void eapol_sm_abort_cached(struct eapol_sm *sm)
{
    wpa_printf(TRACE_GROUP_1, "RSN: Authenticator did not accept PMKID, "
               "doing full EAP authentication\n");
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_AC_NOT_ACCEPT_PMKID);
    if (sm == NULL) {
        return;
    }
    sm->cached_pmk = FALSE;
    sm->SUPP_PAE_state = SUPP_PAE_CONNECTING;
    sm->suppPortStatus = Unauthorized;
    sm->eapRestart = TRUE;
}


void eapol_sm_register_scard_ctx(struct eapol_sm *sm, void *ctx)
{
    if (sm) {
        sm->ctx->scard_ctx = ctx;
        eap_register_scard_ctx(sm->eap, ctx);
    }
}


void eapol_sm_notify_portControl(struct eapol_sm *sm, PortControl portControl)
{
    if (sm == NULL) {
        return;
    }
    wpa_printf(MSG_DEBUG, "EAPOL: External notification - "
               "portControl=%s\n", eapol_port_control(portControl));
//  kal_trace(DBG_EAPOL_SM,INFO_SUPC_EAPOL_NOTIFY_PORTCONTROL);
//  kal_wap_trace(MAP_MOD_SUPC,DBG_EAPOL_SM,"%s",eapol_port_control(portControl));
    sm->portControl = portControl;
    eapol_sm_step(sm);
}

/*
void eapol_sm_notify_ctrl_attached(struct eapol_sm *sm)
{
    if (sm == NULL)
        return;
    eap_sm_notify_ctrl_attached(sm->eap);
}


void eapol_sm_notify_ctrl_response(struct eapol_sm *sm)
{
    if (sm == NULL)
        return;
    if (sm->eapReqData && !sm->eapReq) {
        wpa_printf(MSG_DEBUG, "EAPOL: received control response (user "
               "input) notification - retrying pending EAP "
               "Request");
        sm->eapolEap = TRUE;
        sm->eapReq = TRUE;
        eapol_sm_step(sm);
    }
}
*/

static struct wpa_ssid *eapol_sm_get_config(void *ctx)
{
    struct eapol_sm *sm = ctx;
    return sm ? sm->config : NULL;
}

static u8 *eapol_sm_get_eapReqData(void *ctx, wpa_size_t *len)
{
    struct eapol_sm *sm = ctx;
    if (sm == NULL || sm->eapReqData == NULL) {
        *len = 0;
        return NULL;
    }

    *len = sm->eapReqDataLen;
    return sm->eapReqData;
}


static Boolean eapol_sm_get_bool(void *ctx, enum eapol_bool_var variable) {
    struct eapol_sm *sm = ctx;
    if (sm == NULL)
        return FALSE;
    switch (variable)
    {
        case EAPOL_eapSuccess:
            return sm->eapSuccess;
        case EAPOL_eapRestart:
            return sm->eapRestart;
        case EAPOL_eapFail:
            return sm->eapFail;
        case EAPOL_eapResp:
            return sm->eapResp;
        case EAPOL_eapNoResp:
            return sm->eapNoResp;
        case EAPOL_eapReq:
            return sm->eapReq;
        case EAPOL_portEnabled:
            return sm->portEnabled;
        case EAPOL_altAccept:
            return sm->altAccept;
        case EAPOL_altReject:
            return sm->altReject;
    }
    return FALSE;
}


static void eapol_sm_set_bool(void *ctx, enum eapol_bool_var variable,
                              Boolean value) {
    struct eapol_sm *sm = ctx;
    if (sm == NULL)
        return;
    switch (variable)
    {
        case EAPOL_eapSuccess:
            sm->eapSuccess = value;
            break;
        case EAPOL_eapRestart:
            sm->eapRestart = value;
            break;
        case EAPOL_eapFail:
            sm->eapFail = value;
            break;
        case EAPOL_eapResp:
            sm->eapResp = value;
            break;
        case EAPOL_eapNoResp:
            sm->eapNoResp = value;
            break;
        case EAPOL_eapReq:
            sm->eapReq = value;
            break;
        case EAPOL_portEnabled:
            sm->portEnabled = value;
            break;
        case EAPOL_altAccept:
            sm->altAccept = value;
            break;
        case EAPOL_altReject:
            sm->altReject = value;
            break;
    }
}


static unsigned int eapol_sm_get_int(void *ctx, enum eapol_int_var variable) {
    struct eapol_sm *sm = ctx;
    if (sm == NULL)
        return 0;
    switch (variable)
    {
        case EAPOL_idleWhile:
            return sm->idleWhile;
    }
    return 0;
}


static void eapol_sm_set_int(void *ctx, enum eapol_int_var variable,
                             unsigned int value) {
    struct eapol_sm *sm = ctx;
    if (sm == NULL)
        return;
    switch (variable)
    {
        case EAPOL_idleWhile:
            sm->idleWhile = value;
            break;
    }
}


static struct eapol_callbacks eapol_cb_list = {
    eapol_sm_get_config,
    eapol_sm_get_bool,
    eapol_sm_set_bool,
    eapol_sm_get_int,
    eapol_sm_set_int,
    eapol_sm_get_eapReqData
};
