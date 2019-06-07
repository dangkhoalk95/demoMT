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


#ifndef EAPOL_SM_H
#define EAPOL_SM_H
#include "wpa.h"
#include "defs.h"
#include "eap.h" // Kinki change for the blacklist

typedef enum { Unauthorized, Authorized } PortStatus;
typedef enum { Auto, ForceUnauthorized, ForceAuthorized } PortControl;

struct eapol_config {
    int accept_802_1x_keys;
#define EAPOL_REQUIRE_KEY_UNICAST BIT(0)
#define EAPOL_REQUIRE_KEY_BROADCAST BIT(1)
    int required_keys; /* which EAPOL-Key packets are required before
                * marking connection authenticated */
    int fast_reauth; /* whether fast EAP reauthentication is enabled */
    int workaround; /* whether EAP workarounds are enabled */
};

/* IEEE 802.1aa/D6.1 - Supplicant */

struct eapol_sm {
    /* Timers */
    eventid   event_id;
    unsigned int authWhile;
    unsigned int heldWhile;
    unsigned int startWhen;
    unsigned int idleWhile; /* for EAP state machine */

    /* Global variables */
    Boolean eapFail;
    Boolean eapolEap;
    Boolean eapSuccess;
    Boolean initialize;
    Boolean keyDone;
    Boolean keyRun;
    PortControl portControl;
    Boolean portEnabled;
    PortStatus suppPortStatus;  /* dot1xSuppControlledPortStatus */
    Boolean portValid;
    Boolean suppAbort;
    Boolean suppFail;
    Boolean suppStart;
    Boolean suppSuccess;
    Boolean suppTimeout;

    /* Supplicant PAE state machine */
    enum {
        SUPP_PAE_UNKNOWN = 0,
        SUPP_PAE_DISCONNECTED = 1,
        SUPP_PAE_LOGOFF = 2,
        SUPP_PAE_CONNECTING = 3,
        SUPP_PAE_AUTHENTICATING = 4,
        SUPP_PAE_AUTHENTICATED = 5,
        /* unused(6) */
        SUPP_PAE_HELD = 7,
        SUPP_PAE_RESTART = 8,
        SUPP_PAE_S_FORCE_AUTH = 9,
        SUPP_PAE_S_FORCE_UNAUTH = 10
    } SUPP_PAE_state; /* dot1xSuppPaeState */
    /* Variables */
    Boolean userLogoff;
    Boolean logoffSent;
    unsigned int startCount;
    Boolean eapRestart;
    PortControl sPortMode;
    /* Constants */
    unsigned int heldPeriod; /* dot1xSuppHeldPeriod */
    unsigned int startPeriod; /* dot1xSuppStartPeriod */
    unsigned int maxStart; /* dot1xSuppMaxStart */

    /* Key Receive state machine */
    enum {
        KEY_RX_UNKNOWN = 0,
        KEY_RX_NO_KEY_RECEIVE, KEY_RX_KEY_RECEIVE
    } KEY_RX_state;
    /* Variables */
    Boolean rxKey;

    /* Supplicant Backend state machine */
    enum {
        SUPP_BE_UNKNOWN = 0,
        SUPP_BE_INITIALIZE = 1,
        SUPP_BE_IDLE = 2,
        SUPP_BE_REQUEST = 3,
        SUPP_BE_RECEIVE = 4,
        SUPP_BE_RESPONSE = 5,
        SUPP_BE_FAIL = 6,
        SUPP_BE_TIMEOUT = 7,
        SUPP_BE_SUCCESS = 8
    } SUPP_BE_state; /* dot1xSuppBackendPaeState */
    /* Variables */
    Boolean eapNoResp;
    Boolean eapReq;
    Boolean eapResp;
    /* Constants */
    unsigned int authPeriod; /* dot1xSuppAuthPeriod */

#ifdef WIFI_PORT_TIMER
    /* Port Timer state machine */
    enum {
        PORT_TIMER_UNKNOWN = 0,
        PORT_TIMER_ONE_SECOND = 1,
        PORT_TIMER_TICK = 2
    } PORT_TIMER_state; /* dot1xSuppPortTimerState */
    /* Variables */
    Boolean tick;
#endif

    /* Statistics */
    unsigned int dot1xSuppEapolFramesRx;
    unsigned int dot1xSuppEapolFramesTx;
    unsigned int dot1xSuppEapolStartFramesTx;
    unsigned int dot1xSuppEapolLogoffFramesTx;
    unsigned int dot1xSuppEapolRespFramesTx;
    unsigned int dot1xSuppEapolReqIdFramesRx;
    unsigned int dot1xSuppEapolReqFramesRx;
    unsigned int dot1xSuppInvalidEapolFramesRx;
    unsigned int dot1xSuppEapLengthErrorFramesRx;
    unsigned int dot1xSuppLastEapolFrameVersion;
    unsigned char dot1xSuppLastEapolFrameSource[6];

    /* Miscellaneous variables (not defined in IEEE 802.1aa/D6.1) */
    Boolean changed;
    struct eap_sm *eap;
    struct wpa_ssid *config;
    Boolean initial_req;
    u8 *last_rx_key;
    wpa_size_t last_rx_key_len;
    u8 *eapReqData; /* for EAP */
    wpa_size_t eapReqDataLen; /* for EAP */
    Boolean altAccept; /* for EAP */
    Boolean altReject; /* for EAP */
    Boolean replay_counter_valid;
    u8 last_replay_counter[16];
    struct eapol_config conf;
    struct eapol_ctx *ctx;
    enum { EAPOL_CB_IN_PROGRESS = 0, EAPOL_CB_SUCCESS, EAPOL_CB_FAILURE }
    cb_status;
    Boolean cached_pmk;
#ifdef WIFI_PORT_TIMER
    Boolean txStart_enable; // Kinki change for the port timer
#endif

    Boolean unicast_key_received, broadcast_key_received;
};

struct eapol_ctx {
    void *ctx; /* pointer to arbitrary upper level context */
    int preauth; /* This EAPOL state machine is used for IEEE 802.11i/RSN
              * pre-authentication */
    void (*cb)(struct eapol_sm *eapol, int success, void *ctx);
    void *cb_ctx, *msg_ctx, *scard_ctx;
    void (*eapol_done_cb)(void *ctx);
    int (*eapol_send)(void *ctx, int type, u8 *buf, wpa_size_t len);
    int (*set_wep_key)(void *ctx, int unicast, int keyidx,
                       u8 *key, wpa_size_t keylen
                      );
    /**
     * wps - WPS context data
     *
     * This is only used by EAP-WSC and can be left %NULL if not available.
     */
    struct wps_context *wps;
};


struct wpa_ssid;

#ifdef IEEE8021X_EAPOL
struct eapol_sm *eapol_sm_init(struct eapol_ctx *ctx);
void eapol_sm_deinit(struct eapol_sm *sm);
void eapol_sm_step(struct eapol_sm *sm);
int eapol_sm_get_status(struct eapol_sm *sm, char *buf, wpa_size_t buflen,
                        int verbose);
int eapol_sm_get_mib(struct eapol_sm *sm, char *buf, wpa_size_t buflen);
void eapol_sm_configure(struct eapol_sm *sm, int heldPeriod, int authPeriod,
                        int startPeriod, int maxStart);
Boolean eapol_get_bool(struct eap_sm *sm, enum eapol_bool_var var); // Kinki change for the blacklist
void eapol_sm_rx_eapol(struct eapol_sm *sm, u8 *src, u8 *buf, wpa_size_t len);
void eapol_sm_notify_tx_eapol_key(struct eapol_sm *sm);
void eapol_sm_notify_portEnabled(struct eapol_sm *sm, Boolean enabled);
void eapol_sm_notify_portValid(struct eapol_sm *sm, Boolean valid);
void eapol_sm_notify_eap_success(struct eapol_sm *sm, Boolean success);
void eapol_sm_notify_eap_fail(struct eapol_sm *sm, Boolean fail);
void eapol_sm_notify_config(struct eapol_sm *sm, struct wpa_ssid *config,
                            struct eapol_config *conf);
int eapol_sm_get_key(struct eapol_sm *sm, u8 *key, wpa_size_t len);
void eapol_sm_notify_logoff(struct eapol_sm *sm, Boolean logoff);
void eapol_sm_notify_cached(struct eapol_sm *sm);
void eapol_sm_notify_pmkid_attempt(struct eapol_sm *sm, int attempt);
void eapol_sm_register_scard_ctx(struct eapol_sm *sm, void *ctx);
void eapol_sm_notify_portControl(struct eapol_sm *sm, PortControl portControl);

#ifdef WIFI_PORT_TIMER
extern void supc_stop_eapol_port_timer(struct eapol_sm *sm); // Kinki change for the port timer

eventid supc_start_eapol_port_timer(struct eapol_sm *sm, kal_uint32 delay);
#endif/* WIFI_PORT_TIMER*/

#else
static inline struct eapol_sm *eapol_sm_init(struct eapol_ctx *ctx)
{
    os_free(ctx);
    return (struct eapol_sm *) 1;
}
static inline void eapol_sm_deinit(struct eapol_sm *sm)
{
}
static inline void eapol_sm_step(struct eapol_sm *sm)
{
}
static inline int eapol_sm_get_status(struct eapol_sm *sm, char *buf,
                                      size_t buflen, int verbose)
{
    return 0;
}
static inline int eapol_sm_get_mib(struct eapol_sm *sm, char *buf,
                                   size_t buflen)
{
    return 0;
}
static inline void eapol_sm_configure(struct eapol_sm *sm, int heldPeriod,
                                      int authPeriod, int startPeriod,
                                      int maxStart)
{
}
static inline int eapol_sm_rx_eapol(struct eapol_sm *sm, const u8 *src,
                                    const u8 *buf, size_t len)
{
    return 0;
}
static inline void eapol_sm_notify_tx_eapol_key(struct eapol_sm *sm)
{
}
static inline void eapol_sm_notify_portEnabled(struct eapol_sm *sm,
        Boolean enabled)
{
}
static inline void eapol_sm_notify_portValid(struct eapol_sm *sm,
        Boolean valid)
{
}
static inline void eapol_sm_notify_eap_success(struct eapol_sm *sm,
        Boolean success)
{
}
static inline void eapol_sm_notify_eap_fail(struct eapol_sm *sm, Boolean fail)
{
}
static inline void eapol_sm_notify_config(struct eapol_sm *sm,
        struct wpa_ssid *config,
        struct eapol_config *conf)
{
}
static inline int eapol_sm_get_key(struct eapol_sm *sm, u8 *key, size_t len)
{
    return -1;
}
static inline void eapol_sm_notify_logoff(struct eapol_sm *sm, Boolean logoff)
{
}
static inline void eapol_sm_notify_cached(struct eapol_sm *sm)
{
}
#define eapol_sm_notify_pmkid_attempt(sm, attempt) do { } while (0)
#define eapol_sm_register_scard_ctx(sm, ctx) do { } while (0)
static inline void eapol_sm_notify_portControl(struct eapol_sm *sm,
        PortControl portControl)
{
}
static inline void eapol_sm_notify_ctrl_attached(struct eapol_sm *sm)
{
}
static inline void eapol_sm_notify_ctrl_response(struct eapol_sm *sm)
{
}
static inline void eapol_sm_request_reauth(struct eapol_sm *sm)
{
}
static inline void eapol_sm_notify_lower_layer_success(struct eapol_sm *sm,
        int in_eapol_sm)
{
}
static inline void eapol_sm_invalidate_cached_session(struct eapol_sm *sm)
{
}
static inline const char *eapol_sm_get_method_name(struct eapol_sm *sm)
{
    return NULL;
}

#endif /* IEEE8021X_EAPOL */

#endif /* EAPOL_SM_H */
