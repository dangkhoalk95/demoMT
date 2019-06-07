/*
 * Dynamic data buffer
 * Copyright (c) 2007-2009, Jouni Malinen <j@w1.fi>
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
#include "wndrv_supc_types.h"
#include "supc_common.h"
#include "wpa.h"

#include "wpa_supplicant_i.h"
#include "supc_wpa_common.h"
#include "wpabuf.h"

static void wpabuf_overflow(const struct wpabuf *buf, size_t len)
{
    wpa_printf(TRACE_ERROR, "wpabuf %p (size=%lu used=%lu) overflow len=%lu\n",
               buf, (unsigned long) buf->size, (unsigned long) buf->used,
               (unsigned long) len);
//  kal_trace(DBG_MEM_MGMT,INFO_SUPC_WPA_BUF_OVERFLOW,buf, (unsigned long) buf->size, (unsigned long) buf->used,
//         (unsigned long) len);
    SUPC_ASSERT(KAL_FALSE);
}


int _wpabuf_resize(struct wpabuf **_buf, size_t add_len, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *buf = *_buf;
    if (buf == NULL) {
        *_buf = _wpabuf_alloc(add_len, file_name, line);
        return *_buf == NULL ? -1 : 0;
    }
    if (buf->used + add_len > buf->size) {
        unsigned char *nbuf;
        if (buf->ext_data) {
            /*
            nbuf = os_realloc(buf->ext_data, buf->used + add_len);
            if (nbuf == NULL)
                return -1;
            os_memset(nbuf + buf->used, 0, add_len);
            */
            if ((nbuf = supc_get_ctrl_buffer(buf->used + add_len,
                                             file_name, line, KAL_TRUE)) == NULL) {
                return -1;
            }

            kal_mem_set(nbuf, 0, (buf->used + add_len));
            kal_mem_cpy(nbuf, buf->ext_data, buf->used);
            if (buf->ext_data) {
                supc_free_ctrl_buffer(buf->ext_data, file_name, line, KAL_TRUE);
            }

            buf->ext_data = nbuf;
        } else {
            /*
            nbuf = os_realloc(buf, sizeof(struct wpabuf) +
                      buf->used + add_len);
            if (nbuf == NULL)
                return -1;
            os_memset(nbuf + sizeof(struct wpabuf) + buf->used, 0, add_len);
            */
            if ((nbuf = supc_get_ctrl_buffer(sizeof(struct wpabuf) +
                                             buf->used + add_len, file_name, line, KAL_TRUE)) == NULL) {
                return -1;
            }

            kal_mem_set(nbuf, 0, (sizeof(struct wpabuf) +
                                  buf->used + add_len));
            kal_mem_cpy(nbuf, buf, (sizeof(struct wpabuf) + buf->used));
            if (buf) {
                supc_free_ctrl_buffer(buf, file_name, line, KAL_TRUE);
            }

            buf = (struct wpabuf *) nbuf;
            *_buf = buf;
        }
        buf->size = buf->used + add_len;
    }

    return 0;
}


/**
 * wpabuf_alloc - Allocate a wpabuf of the given size
 * @len: Length for the allocated buffer
 * Returns: Buffer to the allocated wpabuf or %NULL on failure
 */
struct wpabuf *_wpabuf_alloc(size_t len, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *buf = NULL;
    buf = supc_get_ctrl_buffer(sizeof(struct wpabuf) + len, file_name, line, KAL_TRUE);
    if (buf == NULL) {
        return NULL;
    }
    kal_mem_set(buf, 0, sizeof(struct wpabuf) + len);
    buf->size = len;
    return buf;
}


struct wpabuf *_wpabuf_alloc_ext_data(u8 *data, size_t len, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *buf = NULL;
    buf = supc_get_ctrl_buffer(sizeof(struct wpabuf), file_name, line, KAL_TRUE);
    if (buf == NULL) {
        return NULL;
    }
    kal_mem_set(buf, 0, sizeof(struct wpabuf));
    buf->size = len;
    buf->used = len;
    buf->ext_data = data;

    return buf;
}


struct wpabuf *_wpabuf_alloc_copy(const void *data, size_t len, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *buf = _wpabuf_alloc(len, file_name, line);
    if (buf) {
        wpabuf_put_data(buf, data, len);
    }
    return buf;
}


struct wpabuf *_wpabuf_dup(const struct wpabuf *src, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *buf = _wpabuf_alloc(wpabuf_len(src), file_name, line);
    if (buf) {
        wpabuf_put_data(buf, wpabuf_head(src), wpabuf_len(src));
    }
    return buf;
}


/**
 * wpabuf_free - Free a wpabuf
 * @buf: wpabuf buffer
 */
void _wpabuf_free(struct wpabuf *buf, kal_char *file_name, kal_uint32 line)
{
    if (buf == NULL) {
        return;
    }
    if (buf->ext_data) {
        supc_free_ctrl_buffer(buf->ext_data, file_name, line, KAL_TRUE);
    }
    supc_free_ctrl_buffer(buf, file_name, line, KAL_TRUE);
}


void *wpabuf_put(struct wpabuf *buf, size_t len)
{
    void *tmp = wpabuf_mhead_u8(buf) + wpabuf_len(buf);
    buf->used += len;
    if (buf->used > buf->size) {
        wpabuf_overflow(buf, len);
    }
    return tmp;
}


/**
 * wpabuf_concat - Concatenate two buffers into a newly allocated one
 * @a: First buffer
 * @b: Second buffer
 * Returns: wpabuf with concatenated a + b data or %NULL on failure
 *
 * Both buffers a and b will be freed regardless of the return value. Input
 * buffers can be %NULL which is interpreted as an empty buffer.
 */
struct wpabuf *_wpabuf_concat(struct wpabuf *a, struct wpabuf *b, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *n = NULL;
    size_t len = 0;

    if (b == NULL) {
        return a;
    }

    if (a) {
        len += wpabuf_len(a);
    }
    if (b) {
        len += wpabuf_len(b);
    }

    n = _wpabuf_alloc(len, file_name, line);
    if (n) {
        if (a) {
            wpabuf_put_buf(n, a);
        }
        if (b) {
            wpabuf_put_buf(n, b);
        }
    }

    _wpabuf_free(a, file_name, line);
    _wpabuf_free(b, file_name, line);

    return n;
}


/**
 * wpabuf_zeropad - Pad buffer with 0x00 octets (prefix) to specified length
 * @buf: Buffer to be padded
 * @len: Length for the padded buffer
 * Returns: wpabuf padded to len octets or %NULL on failure
 *
 * If buf is longer than len octets or of same size, it will be returned as-is.
 * Otherwise a new buffer is allocated and prefixed with 0x00 octets followed
 * by the source data. The source buffer will be freed on error, i.e., caller
 * will only be responsible on freeing the returned buffer. If buf is %NULL,
 * %NULL will be returned.
 */
struct wpabuf *_wpabuf_zeropad(struct wpabuf *buf, size_t len, kal_char *file_name, kal_uint32 line)
{
    struct wpabuf *ret;
    size_t blen;

    if (buf == NULL) {
        return NULL;
    }

    blen = wpabuf_len(buf);
    if (blen >= len) {
        return buf;
    }

    ret = _wpabuf_alloc(len, file_name, line);
    if (ret) {
        memset(wpabuf_put(ret, len - blen), 0, len - blen);
        wpabuf_put_buf(ret, buf);
    }
    _wpabuf_free(buf, file_name, line);

    return ret;
}

#if 0
void wpabuf_printf(struct wpabuf *buf, char *fmt, ...)
{
    va_list ap;
    void *tmp = wpabuf_mhead_u8(buf) + wpabuf_len(buf);
    int res;

    va_start(ap, fmt);
    res = vsnprintf(tmp, buf->size - buf->used, fmt, ap);
    va_end(ap);
    if (res < 0 || (size_t) res >= buf->size - buf->used) {
        wpabuf_overflow(buf, res);
    }
    buf->used += res;
}
#endif
