/*
 * EAP-WSC peer for Wi-Fi Protected Setup
 * Copyright (c) 2007-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef MTK_WIFI_CHIP_USE_MT5931
#pragma import(__use_no_semihosting_swi)
#endif

//#include "kal_release.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
//#include "supc_trc.h"
#include "supc_common.h"
#include "wndrv_supc_types.h"
#include "wndrv_supc_msg.h"

#include "wpa_supplicant.h"
#include "wpa.h"

#include "wpa_supplicant_i.h"


#include "config_ssid.h"
#include "supc_config.h"
#include "l2_packet.h"

#include "eap_wsc_common.h"
#include "eap.h"
#include "eap_i.h"
#include "eap_common.h"
#include "supc_wps_defs.h"
#include "supc_wps.h"
#include "eap_wsc_common.h"
#include "config_ssid.h"
#include "wpabuf.h"



struct eap_wsc_data {
    enum wsc_data_state { WAIT_START, MESG,
                          FRAG_ACK, WAIT_FRAG_ACK,
                          DONE, FAIL
                        } state;
    int registrar;
    struct wpabuf *in_buf;
    struct wpabuf *out_buf;
    enum wsc_op_code in_op_code, out_op_code;
    size_t out_used;
    size_t fragment_size;
    struct wps_data *wps;
    struct wps_context *wps_ctx;
};

static const char *eap_wsc_state_txt(int state)
{
    switch (state) {
        case WAIT_START:
            return "WAIT_START";
        case MESG:
            return "MESG";
        case FRAG_ACK:
            return "FRAG_ACK";
        case WAIT_FRAG_ACK:
            return "WAIT_FRAG_ACK";
        case DONE:
            return "DONE";
        case FAIL:
            return "FAIL";
        default:
            return "?";
    }
}


const u8 *eap_get_config_identity(struct eap_sm *sm, wpa_size_t *len)
{
    struct wpa_ssid *config = eap_get_config(sm);
    if (config == NULL) {
        return NULL;
    }
    *len = config->identity_len;
    return config->identity;
}

static void eap_wsc_state(struct eap_wsc_data *data, int state)
{
//  kal_wap_trace(MAP_MOD_SUPC,DBG_EAP_METHOD,"EAP-WSC: %s -> %s",
//         eap_wsc_state_txt(data->state),
//         eap_wsc_state_txt(state));
    data->state = (enum wsc_data_state) state;
}


static void  *eap_wsc_init(struct eap_sm *sm)
{
    struct eap_wsc_data *data;
    const u8 *identity;
    wpa_size_t identity_len;
    int registrar;
    struct wps_config cfg;
    const char *pos;
    const char *phase1;
    struct wps_context *wps;
    //struct wpa_ssid *ssid_cfg;

    INNER_TRACE_STEP(0);
    wps = sm->wps;
    sm->wps_done = FALSE;
    if (wps == NULL) {
        wpa_printf(TRACE_ERROR, "EAP-WSC: WPS context not available\n");
//      kal_trace(DBG_WPS,INFO_WPS_CONTEXT_NOT_AVAILABLE);
        return NULL;
    }
    INNER_TRACE_STEP(1);
    identity = eap_get_config_identity(sm, &identity_len);

    if (identity && identity_len == WSC_ID_REGISTRAR_LEN &&
            memcmp(identity, WSC_ID_REGISTRAR, WSC_ID_REGISTRAR_LEN) == 0) {
        registrar = 1;    /* Supplicant is Registrar */
    } else if (identity && identity_len == WSC_ID_ENROLLEE_LEN &&
               memcmp(identity, WSC_ID_ENROLLEE, WSC_ID_ENROLLEE_LEN) == 0) {
        registrar = 0;    /* Supplicant is Enrollee */
    } else {
        if (identity) {
            wpa_hexdump_ascii(MSG_INFO, "EAP-WSC: Unexpected identity",
                              (char *)identity, identity_len);
        }
        return NULL;
    }
    INNER_TRACE_STEP(2);

    data = SUPC_ALLOC_BUFF(sizeof(*data));
    if (NULL == data) {
        INNER_TRACE_STEP(3);
        return NULL;
    }
    memset(data, 0, sizeof(*data));
    data->state = registrar ? MESG : WAIT_START;
    data->registrar = registrar;
    data->wps_ctx = wps;

    memset(&cfg, 0, sizeof(cfg));
    cfg.wps = wps;
    cfg.registrar = registrar;

    phase1 = (char *)eap_get_config(sm)->phase1;
    INNER_TRACE_STEP(4);
    if (phase1 == NULL) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: phase1 configuration data not set\n");
//      kal_trace(DBG_WPS,INFO_WPS_PHASE1_CONFIG_DATA_NOT_SET);
        SUPC_DEALLOC_BUFF(data);
        return NULL;
    }

    pos = strstr(phase1, "pin=");
    if (pos) {
        pos += 4;
        cfg.pin = (const u8 *) pos;
        while (*pos != '\0' && *pos != ' ') {
            pos++;
        }
        cfg.pin_len = pos - (const char *) cfg.pin;
    } else {
        pos = strstr(phase1, "pbc=1");
        if (pos) {
            cfg.pbc = 1;
        }
    }
    INNER_TRACE_STEP(5);
    if (cfg.pin == NULL && !cfg.pbc) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: PIN or PBC not set in phase1 "
                   "configuration data\n");
//      kal_trace(DBG_WPS,INFO_WPS_PIN_PBC_NOT_SET_IN_PHASE1_DATA);
        SUPC_DEALLOC_BUFF(data);
        return NULL;
    }
    INNER_TRACE_STEP(6);
    data->wps = wps_init(&cfg);
    if (data->wps == NULL) {
        SUPC_DEALLOC_BUFF(data);
        return NULL;
    }
    INNER_TRACE_STEP(7);
    data->fragment_size = WSC_FRAGMENT_SIZE;

    if (registrar && cfg.pin) {
        /* wps_registrar_add_pin(data->wps_ctx->registrar, NULL,
                      cfg.pin, cfg.pin_len, 0); */
        SUPC_ASSERT(0);
    }
    INNER_TRACE_STEP(8);
    return data;
}

static void eap_wsc_deinit(struct eap_sm *sm, void *priv)
{
    struct eap_wsc_data *data = priv;
    INNER_TRACE_STEP(1);
    wpabuf_free(data->in_buf);
    INNER_TRACE_STEP(2);
    wpabuf_free(data->out_buf);
    INNER_TRACE_STEP(3);
    wps_deinit(data->wps);
    INNER_TRACE_STEP(4);
    SUPC_DEALLOC_BUFF(data->wps_ctx->network_key);
    INNER_TRACE_STEP(5);
    data->wps_ctx->network_key = NULL;
    os_free(data);
    INNER_TRACE_STEP(6);
}

static struct wpabuf *eap_wsc_build_msg(struct eap_wsc_data *data,
                                        struct eap_method_ret *ret, u8 id)

{
    struct wpabuf *resp;
    u8 flags;
    size_t send_len, plen;

    ret->ignore = FALSE;
    wpa_printf(TRACE_GROUP_1, "EAP-WSC: Generating Response\n");
//  kal_trace(DBG_WPS,INFO_WPS_GEN_RSP);
    ret->allowNotifications = TRUE;

    flags = 0;
    INNER_TRACE_STEP(1);
    send_len = wpabuf_len(data->out_buf) - data->out_used;
    if (2 + send_len > data->fragment_size) {
        send_len = data->fragment_size - 2;
        flags |= WSC_FLAGS_MF;
        if (data->out_used == 0) {
            flags |= WSC_FLAGS_LF;
            send_len -= 2;
        }
    }
    INNER_TRACE_STEP(2);
    plen = 2 + send_len;
    if (flags & WSC_FLAGS_LF) {
        plen += 2;
    }
    resp = eap_msg_alloc(EAP_VENDOR_WFA, (EapType)EAP_VENDOR_TYPE_WSC, plen,
                         EAP_CODE_RESPONSE, id);
    if (resp == NULL) {
        return NULL;
    }

    INNER_TRACE_STEP(3);

    wpabuf_put_u8(resp, data->out_op_code); /* Op-Code */
    wpabuf_put_u8(resp, flags); /* Flags */
    if (flags & WSC_FLAGS_LF) {
        wpabuf_put_be16(resp, wpabuf_len(data->out_buf));
    }

    wpabuf_put_data(resp, wpabuf_head_u8(data->out_buf) + data->out_used,
                    send_len);
    data->out_used += send_len;

    INNER_TRACE_STEP(4);

    ret->methodState = METHOD_MAY_CONT;
    ret->decision = DECISION_FAIL;

    if (data->out_used == wpabuf_len(data->out_buf)) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Sending out %d bytes "
                   "(message sent completely)\n",
                   (unsigned long) send_len);
//      kal_trace(DBG_WPS,INFO_WPS_SEND_OUT_COMPLETELY,(unsigned long) send_len);
        wpabuf_free(data->out_buf);
        data->out_buf = NULL;
        data->out_used = 0;

        INNER_TRACE_STEP(5);
        wpa_printf(TRACE_GROUP_1, "data->state=%d, out_op_code=%d\n",
                   data->state, data->out_op_code);
//      kal_trace(DBG_WPS,INFO_WPS_STATE_AND_OPCODE,data->state, data->out_op_code);

        if ((data->state == FAIL && data->out_op_code == WSC_ACK) ||
                data->out_op_code == WSC_NACK ||
                data->out_op_code == WSC_Done) {
            eap_wsc_state(data, FAIL);
            ret->methodState = METHOD_DONE;
        } else {
            eap_wsc_state(data, MESG);
        }
    } else {
        wpa_printf(MSG_DEBUG, "EAP-WSC: Sending out %d bytes "
                   "(%d more to send)\n", (unsigned long) send_len,
                   (unsigned long) wpabuf_len(data->out_buf) -
                   data->out_used);
//      kal_trace(DBG_WPS,INFO_WPS_SEND_FRAGMENT_OUT, (unsigned long) send_len,
//             (unsigned long) wpabuf_len(data->out_buf) -
//             data->out_used);
        eap_wsc_state(data, WAIT_FRAG_ACK);
    }

    return resp;
}

static int eap_wsc_process_cont(struct eap_wsc_data *data,
                                const u8 *buf, size_t len, u8 op_code)
{
    /* Process continuation of a pending message */
    if (op_code != data->in_op_code) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Unexpected Op-Code %d in "
                   "fragment (expected %d)\n",
                   op_code, data->in_op_code);
//      kal_trace(DBG_WPS,INFO_WPS_UNEXPECTED_OPCODE_IN_FRAGMENT, op_code, data->in_op_code);
        return -1;
    }

    if (len > wpabuf_tailroom(data->in_buf)) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Fragment overflow\n");
//      kal_trace(DBG_WPS,INFO_WPS_FRAGMENT_OVERFLOW);
        eap_wsc_state(data, FAIL);
        return -1;
    }

    wpabuf_put_data(data->in_buf, buf, len);
    wpa_printf(TRACE_GROUP_1, "EAP-WSC: Received %lu bytes, waiting "
               "for %lu bytes more\n", (unsigned long) len,
               (unsigned long) wpabuf_tailroom(data->in_buf));
//  kal_trace(DBG_WPS,INFO_WPS_RX_BYTES_NEED_MORE, (unsigned long) len,
//         (unsigned long) wpabuf_tailroom(data->in_buf));

    return 0;
}

static struct wpabuf *eap_wsc_process_fragment(struct eap_wsc_data *data,
        struct eap_method_ret *ret,
        u8 id, u8 flags, u8 op_code,
        u16 message_length,
        const u8 *buf, size_t len)
{
    /* Process a fragment that is not the last one of the message */
    if (data->in_buf == NULL && !(flags & WSC_FLAGS_LF)) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: No Message Length field in a "
                   "fragmented packet\n");
//      kal_trace(DBG_WPS,INFO_WPS_NO_LEN_FLAG_IN_FRAGMENT);
        ret->ignore = TRUE;
        return NULL;
    }

    if (data->in_buf == NULL) {
        /* First fragment of the message */
        data->in_buf = wpabuf_alloc(message_length);
        if (data->in_buf == NULL) {
            wpa_printf(MSG_DEBUG, "EAP-WSC: No memory for "
                       "message\n");
//          kal_trace(DBG_WPS,INFO_WPS_NO_MEM_FOR_MSG);
            ret->ignore = TRUE;
            return NULL;
        }
        data->in_op_code = (enum wsc_op_code)op_code;
        wpabuf_put_data(data->in_buf, buf, len);
        wpa_printf(MSG_DEBUG, "EAP-WSC: Received %lu bytes in first "
                   "fragment, waiting for %lu bytes more\n",
                   (unsigned long) len,
                   (unsigned long) wpabuf_tailroom(data->in_buf));
//      kal_trace(DBG_WPS,INFO_WPS_RX_FRAGMENT_WAIT_MORE,
//             (unsigned long) len,
//             (unsigned long) wpabuf_tailroom(data->in_buf));
    }

    return eap_wsc_build_frag_ack(id, EAP_CODE_RESPONSE);
}

static struct wpabuf *__eap_wsc_process(struct eap_sm *sm, void *priv,
                                        struct eap_method_ret *ret,
                                        const struct wpabuf *reqData);

static u8 *eap_wsc_process(struct eap_sm *sm, void *priv,
                           struct eap_method_ret *ret,
                           u8 *reqData, wpa_size_t reqDataLen,
                           wpa_size_t *respDataLen)
{
    struct wpabuf *inbuf = wpabuf_alloc_copy(reqData, reqDataLen);
    struct wpabuf *outbuf = NULL;
    kal_uint8 *resp = NULL;
    //wpa_size_t len = 0;

    outbuf = __eap_wsc_process(sm, priv, ret, inbuf);
    wpabuf_free(inbuf);
    if (NULL == outbuf) {
        wpa_printf(MSG_DEBUG, "EAP-WSC: process msg fail\n");
//      kal_trace(DBG_WPS,INFO_WPS_PROCESS_MSG_FAIL);
        goto done;
    }
    wpa_printf(MSG_DEBUG, "EAP-WSC: generate msg %d bytes\n", outbuf->used);
//  kal_trace(DBG_WPS,INFO_WPS_GEN_MSG_WITH_LEN, outbuf->used);
    resp = SUPC_ALLOC_BUFF(outbuf->used);
    kal_mem_cpy(resp, (kal_uint8 *)wpabuf_head(outbuf), wpabuf_len(outbuf));
    *respDataLen = wpabuf_len(outbuf);
    wpabuf_free(outbuf);
done:
    return resp;
}

static struct wpabuf *__eap_wsc_process(struct eap_sm *sm, void *priv,
                                        struct eap_method_ret *ret,
                                        const struct wpabuf *reqData)
{
    struct eap_wsc_data *data = priv;
    const u8 *start, *pos, *end;
    size_t len;
    u8 op_code, flags, id;
    u16 message_length = 0;
    enum wps_process_res res;
    struct wpabuf tmpbuf;

    pos = eap_hdr_validate(EAP_VENDOR_WFA, (EapType)EAP_VENDOR_TYPE_WSC, reqData,
                           &len);
    if (pos == NULL || len < 2) {
        ret->ignore = TRUE;
        return NULL;
    }

    id = eap_get_id(reqData);

    start = pos;
    end = start + len;

    op_code = *pos++;
    flags = *pos++;
    if (flags & WSC_FLAGS_LF) {
        if (end - pos < 2) {
            wpa_printf(TRACE_GROUP_1, "EAP-WSC: Message underflow\n");
//          kal_trace(DBG_WPS,INFO_WPS_MSG_UNDERFLOW);
            ret->ignore = TRUE;
            return NULL;
        }
        message_length = WPA_GET_BE16(pos);
        pos += 2;

        if (message_length < end - pos) {
            wpa_printf(MSG_DEBUG, "EAP-WSC: Invalid Message "
                       "Length\n");
//          kal_trace(DBG_WPS,INFO_WPS_INVALID_MSG_LEN);
            ret->ignore = TRUE;
            return NULL;
        }
    }
    wpa_printf(TRACE_GROUP_1, "EAP-WSC: Received packet: Op-Code %d "
               "Flags 0x%x Message Length %d\n",
               op_code, flags, message_length);
//  kal_trace(DBG_WPS,INFO_WPS_RX_OPCODE_FLAGS_LEN,op_code, flags, message_length);

    if (data->state == WAIT_FRAG_ACK) {
        if (op_code != WSC_FRAG_ACK) {
            wpa_printf(TRACE_GROUP_1, "EAP-WSC: Unexpected Op-Code %d "
                       "in WAIT_FRAG_ACK state\n", op_code);
//          kal_trace(DBG_WPS,INFO_WPS_UNEXPECTED_OP_IN_WAIT_FRAG_ACK_STA, op_code);
            ret->ignore = TRUE;
            return NULL;
        }
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Fragment acknowledged\n");
//      kal_trace(DBG_WPS,INFO_WPS_FRAGMENT_ACKED);
        eap_wsc_state(data, MESG);
        return eap_wsc_build_msg(data, ret, id);
    }

    if (op_code != WSC_ACK && op_code != WSC_NACK && op_code != WSC_MSG &&
            op_code != WSC_Done && op_code != WSC_Start) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Unexpected Op-Code %d\n",
                   op_code);
//      kal_trace(DBG_WPS,INFO_WPS_UNEXPECTED_OP_CODE, op_code);
        ret->ignore = TRUE;
        return NULL;
    }

    if (data->state == WAIT_START) {
        if (op_code != WSC_Start) {
            wpa_printf(TRACE_GROUP_1, "EAP-WSC: Unexpected Op-Code %d "
                       "in WAIT_START state\n", op_code);
//          kal_trace(DBG_WPS,INFO_WPS_UNEXPECTED_OP_CODE_IN_WAIT_START_STA, op_code);
            ret->ignore = TRUE;
            return NULL;
        }
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Received start\n");
//      kal_trace(DBG_WPS,INFO_WPS_RX_START);
        eap_wsc_state(data, MESG);
        /* Start message has empty payload, skip processing */
        goto send_msg;
    } else if (op_code == WSC_Start) {
        wpa_printf(TRACE_GROUP_1, "EAP-WSC: Unexpected Op-Code %d\n",
                   op_code);
//      kal_trace(DBG_WPS,INFO_WPS_UNEXPECTED_OP_CODE, op_code);
        ret->ignore = TRUE;
        return NULL;
    }

    if (data->in_buf &&
            eap_wsc_process_cont(data, pos, end - pos, op_code) < 0) {
        ret->ignore = TRUE;
        return NULL;
    }

    if (flags & WSC_FLAGS_MF) {
        return eap_wsc_process_fragment(data, ret, id, flags, op_code,
                                        message_length, pos,
                                        end - pos);
    }

    if (data->in_buf == NULL) {
        /* Wrap unfragmented messages as wpabuf without extra copy */
        wpabuf_set(&tmpbuf, pos, end - pos);
        data->in_buf = &tmpbuf;
    }

    res = wps_process_msg(data->wps, (enum wsc_op_code)op_code, data->in_buf);
    switch (res) {
        case WPS_DONE:
            wpa_printf(TRACE_GROUP_1, "EAP-WSC: WPS processing completed "
                       "successfully - wait for EAP failure\n");
//      kal_trace(DBG_WPS,INFO_WPS_DONE_WAIT_EAP_FAIL);
            eap_wsc_state(data, FAIL);
            break;
        case WPS_CONTINUE:
            eap_wsc_state(data, MESG);
            break;
        case WPS_FAILURE:
        case WPS_PENDING:
            wpa_printf(TRACE_GROUP_1, "EAP-WSC: WPS processing failed\n");
//      kal_trace(DBG_WPS,INFO_WPS_PROCESS_FAIL);
            eap_wsc_state(data, FAIL);
            break;
    }

    if (data->in_buf != &tmpbuf) {
        wpabuf_free(data->in_buf);
    }
    data->in_buf = NULL;

send_msg:
    if (data->out_buf == NULL) {
        data->out_buf = wps_get_msg(data->wps, &data->out_op_code);
        if (data->out_buf == NULL) {
            wpa_printf(TRACE_GROUP_1, "EAP-WSC: Failed to receive "
                       "message from WPS\n");
//          kal_trace(DBG_WPS,INFO_WPS_FAIL_RX_MSG_FROM_WPS);
            return NULL;
        }
        data->out_used = 0;
    }

    eap_wsc_state(data, MESG);
    INNER_TRACE_STEP(1);
    /*after methodState is set to DONE, WSC cannot receive EAP_Request.
       it must tell EAP SM to enter METHOD_DONE state*/
    if (data->out_op_code == WSC_Done) {
        sm->wps_done = TRUE;
    }
    return eap_wsc_build_msg(data, ret, id);
}


const struct eap_method eap_method_wsc = {
    (EapType)EAP_VENDOR_TYPE_WSC,
    "WSC",
    eap_wsc_init,
    eap_wsc_deinit,
    eap_wsc_process,
    NULL, /*isKeyAvailable*/
    NULL, /*getKey*/
    NULL, /*get_status*/
    NULL, /*has_reauth_data*/
    NULL, /*deinit_for_reauth*/
    NULL, /*init_for_reauth*/
    NULL, /*get_identity*/
    EAP_VENDOR_WFA
};

struct wpabuf *eap_wsc_build_frag_ack(u8 id, u8 code)
{
    struct wpabuf *msg;

    msg = eap_msg_alloc(EAP_VENDOR_WFA, (EapType)EAP_VENDOR_TYPE_WSC, 2, code, id);
    if (msg == NULL) {
        wpa_printf(TRACE_ERROR, "EAP-WSC: Failed to allocate memory for "
                   "FRAG_ACK\n");
//      kal_trace(DBG_WPS,INFO_WPS_ALLOC_MEM_FAIL_FRO_FRAG_ACK);
        return NULL;
    }

    wpa_printf(TRACE_ERROR, "EAP-WSC: Send WSC/FRAG_ACK\n");
//  kal_trace(DBG_WPS,INFO_WPS_SEND_WSC_OR_ACK);
    wpabuf_put_u8(msg, WSC_FRAG_ACK); /* Op-Code */
    wpabuf_put_u8(msg, 0); /* Flags */

    return msg;
}


