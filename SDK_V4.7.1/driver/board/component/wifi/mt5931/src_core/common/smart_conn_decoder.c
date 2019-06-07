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

#include "smart_conn_decoder.h"
#include "precomp.h"


static struct parser_fsm pfsm;
static unsigned char syn[3] = {0x12, 0x13, 0x14};
static unsigned char eth_bc_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static void _pfsm_lock(void)
{
    return;
}

static void _pfsm_unlock(void)
{
    return;
}

static int _pfsm_trylock(void)
{
    return 1;
}

static int _pfsm_rst(void)
{
    MT5931_NEW_TRACE(MT5931_INFO_603, TRACE_PEER, "PFSM _pfsm_rst %d.%d-->0.0", pfsm.state, pfsm.rc);
    pfsm.state = PARSER_STATE_IDLE;
    memset((char *)&pfsm.sa, 0, ETH_MAC_LEN);
    pfsm.rc = 0;
    pfsm.reported = 0;
    pfsm.data_len = 0;
    memset((char *)&pfsm.data, 0, sizeof(struct smart_conn_body));
    pfsm.bitmap = 0;

    return 0;
}

static int rx_done(struct smart_conn_body *scb, BITMAP_T b)
{
    int blen, idx_max;
    BITMAP_T mask;
    unsigned char AESBlockSize = 16;
    unsigned char SSIDPWDLen = 0;

    /* check if len received. */
    if ((b & BIT_MASK_LEN) != BIT_MASK_LEN) {
        return 0;
    }

    SSIDPWDLen = scb->len[0] + scb->len[1];
    if (SSIDPWDLen % AESBlockSize) {
        SSIDPWDLen = AESBlockSize * (SSIDPWDLen / AESBlockSize + 1);
    }

    blen = ETH_MAC_LEN + 4 + SSIDPWDLen;
    blen = (blen + 1) & ~1; /* 2 byte align. */
    idx_max = blen / 2;

    mask = bits_mask((sizeof(BITMAP_T) * 8 - idx_max), idx_max);

    if ((mask & b) == mask) {
        return 1;
    }

    return 0;
}

static void _pfsm_goto_state(enum parser_fsm_state state) {
    MT5931_NEW_TRACE(MT5931_INFO_604, TRACE_PEER, "PFSM state %d.%d --> %d.0\n",
    pfsm.state, pfsm.rc, state);
    pfsm.rc = 0;
    pfsm.state = state;
    return;
}

int parser_fsm_init(char *la, status_cb f)
{
    int ret;

    if (la == NULL || f == NULL) {
        return -EPARAM;
    }

    _pfsm_lock();
    memcpy(pfsm.la, la, ETH_MAC_LEN);
    pfsm.sts_cb = f;
    ret = _pfsm_rst();
    _pfsm_unlock();

    return ret;
}

int parser_fsm_go(char *p, int len)
{
    char *s_addr;
    unsigned char idx, c0, c1;
    unsigned char ftrans, *scb;
    int lock = 0;

    if (p == NULL || len < M80211_HEADER_LEN_MIN) {
        return -EPARAM;
    }

    /*
     *TO DS:     | FC(2B) | DI(2B) |   BSSID   |    SA       |     DA    | ... |
     *FROM DS: | FC(2B) | DI(2B) |    DA       |   BSSID   |     SA    | ... |
    */
    if ((p[1] & 3) == 2) {
        /* from DS. */
        idx = p[7] & 0x7f;
        c0 = p[8];
        c1 = p[9];
        s_addr = p + 16;
    } else if ((p[1] & 3) == 1) {
        /* To DS. */
        idx = p[19] & 0x7f;
        c0 = p[20];
        c1 = p[21];
        s_addr = p + 10;
    } else {
        MT5931_NEW_TRACE(MT5931_INFO_605, TRACE_ERROR, "PFSM IBSS/WDS data not support. %x", p[1]);
        return -EPARAM;
    }

    scb = (unsigned char *) & (pfsm.data);

    lock = _pfsm_trylock();
    if (!lock) {
        return -ELOCK;
    }

    /* check if we are at the correct state. */
    if (pfsm.state < PARSER_STATE_IDLE || pfsm.state > PARSER_STATE_MAX) {
        if (lock) {
            _pfsm_unlock();
        }

        return -ESTATE;
    }

    /* check if SRC MAC addr and data len match. */
    if (pfsm.state > PARSER_STATE_SYNC0) {
        if (memcmp(pfsm.sa, s_addr, ETH_MAC_LEN) || pfsm.data_len != len) {
            /* if syn0's src MAC addr was wrong. we can reset it. */
            if (++pfsm.rc > PFSM_RST_CNTER) {
                _pfsm_rst();
            }

            if (lock) {
                _pfsm_unlock();
            }

            return -EMACLEN;
        }
    }
    do {
        ftrans = 0;
        switch (pfsm.state) {
            case PARSER_STATE_IDLE:
                if (idx == syn[0] && c0 == syn[0] && c1 == syn[0]) {
                    ftrans = 1;
                    _pfsm_goto_state(PARSER_STATE_SYNC0);
                }
                break;
            case PARSER_STATE_SYNC0:
                if (idx == syn[0] && c0 == syn[0] && c1 == syn[0]) {
                    /* record SRC MAC addr and data len. */
                    memcpy(pfsm.sa, s_addr, ETH_MAC_LEN);
                    pfsm.data_len = len;
                } else if (idx == syn[1] && c0 == syn[1] && c1 == syn[1]) {
                    ftrans = 1;
                    _pfsm_goto_state(PARSER_STATE_SYNC1);
                } else if (++pfsm.rc > PFSM_RST_CNTER) {
                    _pfsm_rst();
                }
                break;
            case PARSER_STATE_SYNC1:
                if (idx == syn[1] && c0 == syn[1] && c1 == syn[1]) {
                    /* do nothing. */
                } else if (idx == syn[2] && c0 == syn[2] && c1 == syn[2]) {
                    ftrans = 1;
                    _pfsm_goto_state(PARSER_STATE_SYNC2);
                } else if (++pfsm.rc > PFSM_RST_CNTER) {
                    _pfsm_rst();
                }
                break;
            case PARSER_STATE_SYNC2:
                if (idx == syn[2] && c0 == syn[2] && c1 == syn[2]) {
                    /* do nothing. */
                } else {
                    ftrans = 1;
                    _pfsm_goto_state(PARSER_STATE_DATA);
                }
                break;
            case PARSER_STATE_DATA:
                if (idx < PFSM_IDX_MIN || idx > PFSM_IDX_MAX) {
                    break;
                }

                pfsm.bitmap |= (BITMAP_T)1 << ((sizeof(BITMAP_T) * 8) - (idx - PFSM_IDX_MIN) - 1);
                {
                    char *pbmap = (char *)&pfsm.bitmap;
                    MT5931_NEW_TRACE(MT5931_INFO_606, TRACE_PEER,
                                     "PFSM %02x %02x %02x %02x %02x %02x %02x %02x",
                                     pbmap[7], pbmap[6], pbmap[5], pbmap[4],
                                     pbmap[3], pbmap[2], pbmap[1], pbmap[0]);
                }
                scb[(idx - PFSM_IDX_MIN) * 2] = c0;
                scb[(idx - PFSM_IDX_MIN) * 2 + 1] = c1;

                /* check if MAC addr received done. */
                if ((pfsm.bitmap & BIT_MASK_TA) == BIT_MASK_TA) {
                    /* check if configing me. */
                    if (!memcmp(pfsm.la, (void *)pfsm.data.ta, ETH_MAC_LEN)
                            || !memcmp((void *)pfsm.data.ta, eth_bc_addr, ETH_MAC_LEN)) {
                        if (!pfsm.reported) {
                            pfsm.reported = 1;
                            (*pfsm.sts_cb)(S_SYNCSUC, 0, 0, NULL, 0);
                        }
                    } else {
                        (*pfsm.sts_cb)(S_SYNFAIL, 0, 0, NULL, 0);
                        _pfsm_rst();
                        break;
                    }
                }
                /* check if we received all body.*/
                if (rx_done(&pfsm.data, pfsm.bitmap)) {
                    MT5931_WAP_TRACE(MOD_WNDRV, TRACE_PEER, "PFSM sl:%d, pl:%d, m:%d, s_p:%s",
                                     pfsm.data.len[0], pfsm.data.len[1], pfsm.data.mode[1], pfsm.data.s_p);
                    (*pfsm.sts_cb)(S_INFOGET, pfsm.data.len[0],
                                   pfsm.data.len[1],
                                   pfsm.data.s_p,
                                   pfsm.data.mode[1]);
                    _pfsm_rst();
                }
                break;
            default:
                break;
        }
    } while (ftrans);

    if (lock) {
        _pfsm_unlock();
    }

    return 0;
}



