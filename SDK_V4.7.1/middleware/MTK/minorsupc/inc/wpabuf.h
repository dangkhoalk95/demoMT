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

#ifndef WPABUF_H
#define WPABUF_H

#if defined( __CC_ARM )
#define __KAL_INLINE__                  static __inline
#elif defined( __ICCARM__ )
#define __KAL_INLINE__                  static inline
#else
#define __KAL_INLINE__                  static __inline
#endif

/*
 * Internal data structure for wpabuf. Please do not touch this directly from
 * elsewhere. This is only defined in header file to allow inline functions
 * from this file to access data.
 */
struct wpabuf {
    size_t size; /* total size of the allocated buffer */
    size_t used; /* length of data in the buffer */
    u8 *ext_data; /* pointer to external data; NULL if data follows
               * struct wpabuf */
    /* optionally followed by the allocated buffer */
};

/*for debug usage*/
#define wpabuf_alloc(size)  _wpabuf_alloc(size, __FILE__, __LINE__)
#define wpabuf_free(pbuf)  _wpabuf_free(pbuf, __FILE__, __LINE__)
#define wpabuf_resize(ppbuf, addr_len) _wpabuf_resize(ppbuf, addr_len, __FILE__, __LINE__)
#define wpabuf_alloc_ext_data(data, len) _wpabuf_alloc_ext_data(data,len, __FILE__, __LINE__);
#define wpabuf_alloc_copy(data, len) _wpabuf_alloc_copy(data, len, __FILE__, __LINE__)
#define wpabuf_dup(src) _wpabuf_dup(src, __FILE__, __LINE__)
#define wpabuf_concat(a, b)  _wpabuf_concat(a, b, __FILE__, __LINE__)
#define wpabuf_zeropad(buf, len) _wpabuf_zeropad(buf, len, __FILE__, __LINE__)
/*internal*/
int _wpabuf_resize(struct wpabuf **buf, size_t add_len, kal_char *file_name, kal_uint32 line);
struct wpabuf *_wpabuf_alloc(size_t len, kal_char *file_name, kal_uint32 line);
struct wpabuf *_wpabuf_alloc_ext_data(u8 *data, size_t len, kal_char *file_name, kal_uint32 line);
struct wpabuf *_wpabuf_alloc_copy(const void *data, size_t len, kal_char *file_name, kal_uint32 line);
struct wpabuf *_wpabuf_dup(const struct wpabuf *src, kal_char *file_name, kal_uint32 line);
void _wpabuf_free(struct wpabuf *buf, kal_char *file_name, kal_uint32 line);

/*read/write*/
void *wpabuf_put(struct wpabuf *buf, size_t len);
struct wpabuf *_wpabuf_concat(struct wpabuf *a, struct wpabuf *b, kal_char *file_name, kal_uint32 line);
struct wpabuf *_wpabuf_zeropad(struct wpabuf *buf, size_t len, kal_char *file_name, kal_uint32 line);


/**
 * wpabuf_size - Get the currently allocated size of a wpabuf buffer
 * @buf: wpabuf buffer
 * Returns: Currently allocated size of the buffer
 */
__KAL_INLINE__ size_t wpabuf_size(const struct wpabuf *buf)
{
    return buf->size;
}

/**
 * wpabuf_len - Get the current length of a wpabuf buffer data
 * @buf: wpabuf buffer
 * Returns: Currently used length of the buffer
 */
__KAL_INLINE__ size_t wpabuf_len(const struct wpabuf *buf)
{
    return buf->used;
}

/**
 * wpabuf_tailroom - Get size of available tail room in the end of the buffer
 * @buf: wpabuf buffer
 * Returns: Tail room (in bytes) of available space in the end of the buffer
 */
__KAL_INLINE__ size_t wpabuf_tailroom(const struct wpabuf *buf)
{
    return buf->size - buf->used;
}

/**
 * wpabuf_head - Get pointer to the head of the buffer data
 * @buf: wpabuf buffer
 * Returns: Pointer to the head of the buffer data
 */
__KAL_INLINE__ const void *wpabuf_head(const struct wpabuf *buf)
{
    if (buf->ext_data) {
        return buf->ext_data;
    }
    return buf + 1;
}

__KAL_INLINE__ const u8 *wpabuf_head_u8(const struct wpabuf *buf)
{
    return wpabuf_head(buf);
}

/**
 * wpabuf_mhead - Get modifiable pointer to the head of the buffer data
 * @buf: wpabuf buffer
 * Returns: Pointer to the head of the buffer data
 */
__KAL_INLINE__ void *wpabuf_mhead(struct wpabuf *buf)
{
    if (buf->ext_data) {
        return buf->ext_data;
    }
    return buf + 1;
}

__KAL_INLINE__ u8 *wpabuf_mhead_u8(struct wpabuf *buf)
{
    return wpabuf_mhead(buf);
}

__KAL_INLINE__ void wpabuf_put_u8(struct wpabuf *buf, u8 data)
{
    u8 *pos = wpabuf_put(buf, 1);
    *pos = data;
}

__KAL_INLINE__ void wpabuf_put_be16(struct wpabuf *buf, u16 data)
{
    u8 *pos = wpabuf_put(buf, 2);
    WPA_PUT_BE16(pos, data);
}

__KAL_INLINE__ void wpabuf_put_be24(struct wpabuf *buf, u32 data)
{
    u8 *pos = wpabuf_put(buf, 3);
    WPA_PUT_BE24(pos, data);
}

__KAL_INLINE__ void wpabuf_put_be32(struct wpabuf *buf, u32 data)
{
    u8 *pos = wpabuf_put(buf, 4);
    WPA_PUT_BE32(pos, data);
}

__KAL_INLINE__ void wpabuf_put_data(struct wpabuf *buf, const void *data,
                                     size_t len)
{
    if (data) {
        memcpy(wpabuf_put(buf, len), data, len);
    }
}

__KAL_INLINE__ void wpabuf_put_buf(struct wpabuf *dst,
                                    const struct wpabuf *src)
{
    wpabuf_put_data(dst, wpabuf_head(src), wpabuf_len(src));
}

__KAL_INLINE__ void wpabuf_set(struct wpabuf *buf, const void *data, size_t len)
{
    buf->ext_data = (u8 *) data;
    buf->size = buf->used = len;
}

__KAL_INLINE__ void wpabuf_put_str(struct wpabuf *dst, const char *str)
{
    wpabuf_put_data(dst, str, os_strlen(str));
}

__KAL_INLINE__ void wpa_hexdump_buf(int level, const char *title,
                                     const struct wpabuf *buf)
{
    wpa_hexdump(level, title, wpabuf_head(buf), wpabuf_len(buf));
}

__KAL_INLINE__ void wpa_hexdump_buf_key(int level, const char *title,
        const struct wpabuf *buf)
{
    wpa_hexdump_key(level, title, (char *)wpabuf_head(buf), wpabuf_len(buf));
}


#endif /* WPABUF_H */
