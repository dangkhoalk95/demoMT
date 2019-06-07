/*
 * Universally Unique IDentifier (UUID)
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
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

//#include "kal_release.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "kal_public_api.h"
//#include "supc_trc.h"

#include "supc_common.h"
#include "wpa.h"
#include "wndrv_supc_types.h"

#include "wpa_supplicant_i.h"
#include "wpa_supplicant.h"
#include "config_ssid.h"
#include "supc_config.h"
#include "l2_packet.h"
#include "supc_common.h"
#include "supc_wpa_common.h"
#include "wps_dev_attr.h"
#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "supc_abm_msgs.h"

#include "crypto.h"
#include "sha1.h"
#include "uuid.h"

int uuid_str2bin(const char *str, u8 *bin)
{
    const char *pos;
    u8 *opos;

    pos = str;
    opos = bin;

    if (hexstr2bin(pos, opos, 4)) {
        return -1;
    }
    pos += 8;
    opos += 4;

    if (*pos++ != '-' || hexstr2bin(pos, opos, 2)) {
        return -1;
    }
    pos += 4;
    opos += 2;

    if (*pos++ != '-' || hexstr2bin(pos, opos, 2)) {
        return -1;
    }
    pos += 4;
    opos += 2;

    if (*pos++ != '-' || hexstr2bin(pos, opos, 2)) {
        return -1;
    }
    pos += 4;
    opos += 2;

    if (*pos++ != '-' || hexstr2bin(pos, opos, 6)) {
        return -1;
    }

    return 0;
}


int uuid_bin2str(const u8 *bin, char *str, size_t max_len)
{
    int len;
    len = snprintf(str, max_len, "%02x%02x%02x%02x-%02x%02x-%02x%02x-"
                   "%02x%02x-%02x%02x%02x%02x%02x%02x",
                   bin[0], bin[1], bin[2], bin[3],
                   bin[4], bin[5], bin[6], bin[7],
                   bin[8], bin[9], bin[10], bin[11],
                   bin[12], bin[13], bin[14], bin[15]);
    if (len < 0 || (size_t) len >= max_len) {
        return -1;
    }
    return 0;
}


int is_nil_uuid(const u8 *uuid)
{
    int i;
    for (i = 0; i < UUID_LEN; i++)
        if (uuid[i]) {
            return 0;
        }
    return 1;
}


void uuid_gen_mac_addr(const u8 *mac_addr, u8 *uuid)
{
    const u8 *addr[2];
    kal_uint16 len[2];
    u8 hash[SHA1_MAC_LEN];
    u8 nsid[16] = {
        0x52, 0x64, 0x80, 0xf8,
        0xc9, 0x9b,
        0x4b, 0xe5,
        0xa6, 0x55,
        0x58, 0xed, 0x5f, 0x5d, 0x60, 0x84
    };

    addr[0] = nsid;
    len[0] = sizeof(nsid);
    addr[1] = mac_addr;
    len[1] = 6;
    sha1_vector(2, (kal_uint8 **)addr, len, hash);
    os_memcpy(uuid, hash, 16);

    /* Version: 5 = named-based version using SHA-1 */
    uuid[6] = (5 << 4) | (uuid[6] & 0x0f);

    /* Variant specified in RFC 4122 */
    uuid[8] = 0x80 | (uuid[8] & 0x3f);
}
