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

/* ****************************************************************************
* Alternatively, this software may be distributed under the terms of BSD
* license.
*****************************************************************************/


#ifndef SUPC_COMMON_H
#define SUPC_COMMON_H
#include "supc_map.h"


// Ivan
//#include "app_ltlcom.h"
#include "kal_general_types.h"
//#include "cbm_consts.h"
//#include "tcm_api.h"
//#include "stack_config.h"
#include "stack_msgs.h"

/* Start - Jau add to eliminate too much warning { */
#include "kal_trace.h"
/* Stop  - Jau add to eliminate too much warning } */
//#include "supc_trc.h"


//#define SUPC_ASSERT ASSERT

#ifndef __KAL_INLINE__

#if defined( __CC_ARM )
#define __KAL_INLINE__                  static __inline
#elif defined( __ICCARM__ )
#define __KAL_INLINE__                  static inline
#else
#define __KAL_INLINE__                  static __inline
#endif

#endif/*__KAL_INLINE__*/

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"


#define ntohs(a)    \
        ((((a)&0xFF)<<8) + ((((a)>>8)&0xFF)))

#define htons(a) ntohs(a)

char *print_ssid(kal_uint8 *input_ssid, kal_int16 ssid_len);

extern kal_uint16 wpa_swap_16(unsigned short v);
extern kal_uint32 wpa_swap_32(unsigned int v);

#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define be_to_host16(n) wpa_swap_16(n)
#define host_to_be16(n) wpa_swap_16(n)
#define le_to_host32(n) (n)
#define be_to_host32(n) wpa_swap_32(n)
#define host_to_be32(n) wpa_swap_32(n)


#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/* Joseph:
 * Modify to fit kal
 */
/*
#include <stdint.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
*/

typedef kal_uint64 u64;
typedef kal_uint32 u32;
typedef kal_uint16 u16;
typedef kal_uint8 u8;
//typedef int64_t s64;
typedef kal_int32 s32;
typedef kal_int16 s16;
typedef kal_int8 s8;
typedef kal_uint16 wpa_size_t;
typedef kal_uint32 size_t;

int hostapd_get_rand(u8 *buf, wpa_size_t len);
void hostapd_hexdump(const char *title, const u8 *buf, wpa_size_t len);
int hwaddr_aton(const char *txt, u8 *addr);
int hexstr2bin(const char *hex, u8 *buf, wpa_size_t len);
char *rel2abs_path(const char *rel_path);
void inc_byte_array(u8 *counter, wpa_size_t len);

/*SUPC_LIB USAGE*/
extern msg_type supc_msg_mapping_table[];
//extern module_type supc_module_mapping_table[];
extern sap_type supc_sap_mapping_table[];
//extern void* construct_local_para(kal_uint16 local_para_size, kal_uint32 direction);
//extern void* construct_peer_buff(kal_uint16 pdu_len, kal_uint16  header_len,
//                              kal_uint16 tail_len, kal_uint32 direction);
//extern void free_local_para(local_para_struct *local_para_ptr);
//extern void free_peer_buff(peer_buff_struct *pdu_ptr);
//extern void *wifi_get_ctrl_buffer(kal_uint32 buff_size);
//extern void wifi_free_ctrl_buffer(void *usr_buff);
//extern void wifi_free_ilm(ilm_struct *ilm_ptr);
//extern void kal_assert_fail(char* expr, char* file, kal_uint32 line, kal_uint32 ex1, kal_uint32 ex2, kal_uint32 ex3);

/*******************************************************************************
*  Macros
*******************************************************************************/
#define SUPC_ASSERT ASSERT
//#define SUPC_EXT_ASSERT( expr, str, x, y, z ) do {if(!(expr)) kal_assert_fail((kal_char *)#str, (kal_char *)__FILE__, __LINE__, KAL_TRUE, x, y, z, NULL); } while (0)

/* Macros for handling unaligned memory accesses */

#define WPA_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define WPA_PUT_BE16(a, val)            \
    do {                    \
        (a)[0] = ((u16) (val)) >> 8;    \
        (a)[1] = ((u16) (val)) & 0xff;  \
    } while (0)

#define WPA_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define WPA_PUT_LE16(a, val)            \
    do {                    \
        (a)[1] = ((u16) (val)) >> 8;    \
        (a)[0] = ((u16) (val)) & 0xff;  \
    } while (0)

#define WPA_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
             ((u32) (a)[2]))
#define WPA_PUT_BE24(a, val)                    \
    do {                            \
        (a)[0] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[2] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
             (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define WPA_PUT_BE32(a, val)                    \
    do {                            \
        (a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[3] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
             (((u32) (a)[1]) << 8) | ((u32) (a)[0]))
#define WPA_PUT_LE32(a, val)                    \
    do {                            \
        (a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);   \
        (a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[0] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
             (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
             (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
             (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define WPA_PUT_BE64(a, val)                \
    do {                        \
        (a)[0] = (u8) (((u64) (val)) >> 56);    \
        (a)[1] = (u8) (((u64) (val)) >> 48);    \
        (a)[2] = (u8) (((u64) (val)) >> 40);    \
        (a)[3] = (u8) (((u64) (val)) >> 32);    \
        (a)[4] = (u8) (((u64) (val)) >> 24);    \
        (a)[5] = (u8) (((u64) (val)) >> 16);    \
        (a)[6] = (u8) (((u64) (val)) >> 8); \
        (a)[7] = (u8) (((u64) (val)) & 0xff);   \
    } while (0)

#define WPA_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
             (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
             (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
             (((u64) (a)[1]) << 8) | ((u64) (a)[0]))


#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/*SUPC_LIB USAGE*/


#define DBG_FUNC                TRACE_FUNC
#define DBG_EAP_SM              TRACE_STATE
#define DBG_TIMER               TRACE_INFO
#define DBG_MEM_MGMT           TRACE_WARNING
#define DBG_ERROR                 TRACE_ERROR

#define DBG_INIT                     TRACE_GROUP_1  //supc init and deinit
#define DBG_SCAN                   TRACE_GROUP_2
#define DBG_CONN                 TRACE_GROUP_3
#define DBG_KEY_HANDSHAKE       TRACE_GROUP_4
#define DBG_EAP_METHOD          TRACE_GROUP_5
#define DBG_RESEVED             TRACE_GROUP_6
#define DBG_TST          TRACE_GROUP_7
#define DBG_DUMP                TRACE_GROUP_8
#define DBG_WAPI                TRACE_GROUP_9
#define DBG_WPS                 TRACE_GROUP_10
#define DBG_EAPOL_SM            TRACE_PEER















/* Debugging function - conditional printf and hex dump. Driver wrappers can
 *  use these for debugging purposes. */

enum {MSG_BEGIN = 0, MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR, MSG_STRING,
      MSG_FUNC, MSG_STATE, MSG_DBG_SCAN, MSG_DBG_CONN, MSG_DBG_AUTH, MSG_DBG_TIMER, MSG_TOTAL
     };

/* Start - Jau add to eliminate too much warning { */
#if 0
#ifndef MSG_MSGDUMP
#define MSG_MSGDUMP (module_type)supc_module_mapping_table[0]
#endif

#ifndef MSG_DEBUG
#define MSG_DEBUG   (module_type)supc_module_mapping_table[0]
#endif

#ifndef MSG_INFO
#define MSG_INFO    (module_type)supc_module_mapping_table[0]
#endif

#ifndef MSG_WARNING
#define MSG_WARNING (module_type)supc_module_mapping_table[0]
#endif

#ifndef MSG_ERROR
#define MSG_ERROR   (module_type)supc_module_mapping_table[0]
#endif

#ifndef MSG_STRING
#define MSG_STRING  (module_type)supc_module_mapping_table[0]
#endif
#endif

/* Stop  - Jau add to eliminate too much warning } */

/**
 * wpa_printf - conditional printf
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
//#define wpa_printf kal_prompt_trace
void wpa_printf(kal_uint32 level, const char *fmt, ...);

//void wpa_printf(int level, char *fmt, ...);
//__attribute__ ((format (printf, 2, 3)));

/**
 * wpa_hexdump - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the @buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of @buf is printed out has hex dump.
 */
void wpa_hexdump(int level, const char *title, const char *buf, wpa_size_t len);

/**
 * wpa_hexdump_key - conditional hex dump, hide keys
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the @buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of @buf is printed out has hex dump. This works
 * like wpa_hexdump(), but by default, does not include secret keys (passwords,
 * etc.) in debug output.
 */
void wpa_hexdump_key(int level, const char *title, char *buf, wpa_size_t len);
int wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len);

/**
 * wpa_hexdump_ascii - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the @buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of @buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown.
 */
void wpa_hexdump_ascii(int level, const char *title, const char *buf,
                       wpa_size_t len);

/**
 * wpa_hexdump_ascii_key - conditional hex dump, hide keys
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the @buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of @buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown. This works like wpa_hexdump_ascii(), but by
 * default, does not include secret keys (passwords, etc.) in debug output.
 */
void wpa_hexdump_ascii_key(int level, const char *title, const char *buf,
                           wpa_size_t len);

#ifdef EAPOL_TEST
#define WPA_ASSERT(a)                              \
    do {                                   \
        if (!(a)) {                        \
            wpa_printf(MSG_DEBUG,"WPA_ASSERT FAILED '" #a "' "         \
                   "%s %s:%d\n",                   \
                   __FUNCTION__, __FILE__, __LINE__);      \
            exit(1);                       \
        }                              \
    } while (0)
#else
#define WPA_ASSERT(a) do { } while (0)
#endif

extern char *strdup(const char *);
extern void *supc_int_get_buffer(kal_uint16 buff_size);
extern void supc_int_free_buffer(void *buff_ptr);

extern void *supc_get_ctrl_buffer(kal_uint16 buff_size, kal_char *file_name, kal_uint32 line, kal_bool is_supc);
extern void supc_free_ctrl_buffer(void *buff_ptr, kal_char *file_name, kal_uint32 line, kal_bool is_supc);

#ifdef _SUPC_CHECK_MEM_LEAK

#define SUPC_MEM_INFO_FN_LEN 32
#define SUPC_MEM_INFO_MAX_REC 300

#define SUPC_ALLOC_BUFF(_buf_size) supc_get_ctrl_buffer(_buf_size, __FILE__, __LINE__, KAL_TRUE)
#define SUPC_DEALLOC_BUFF(_buf_ptr) {if(_buf_ptr != NULL) supc_free_ctrl_buffer(_buf_ptr, __FILE__, __LINE__, KAL_TRUE);}

#define SSL_ALLOC_BUFF(_buf_size) supc_get_ctrl_buffer(_buf_size, __FILE__, __LINE__,KAL_FALSE)
#define SSL_DEALLOC_BUFF(_buf_ptr) {if(_buf_ptr != NULL) supc_free_ctrl_buffer(_buf_ptr, __FILE__, __LINE__,KAL_FALSE);}
extern void supc_check_mem_leak(void);

typedef struct {
    kal_uint16  alloc_seqno;
    kal_uint16  free_seqno;
    kal_uint32  mem_addr;
    kal_char *file_name;
    kal_uint32  line;
    kal_uint16 buff_size;

} supc_mem_check_strcut;


extern supc_mem_check_strcut supc_mem_info[SUPC_MEM_INFO_MAX_REC];
extern kal_uint16                      supc_mem_info_index;


#else
#define SUPC_ALLOC_BUFF supc_int_get_buffer
#define SUPC_DEALLOC_BUFF supc_int_free_buffer
#define SSL_ALLOC_BUFF supc_int_get_buffer
#define SSL_DEALLOC_BUFF supc_int_free_buffer
#endif

#ifdef _SUPC_MEM_ADM

extern kal_uint32 supc_mem_adm_pool_g[];
extern KAL_ADM_ID supc_mem_adm_id;

#endif /* _SUPC_MEM_ADM */

/*****port c-lib functions******/
#ifndef os_malloc
#define os_malloc(s) SUPC_ALLOC_BUFF((s))
#endif

#ifndef os_free
#define os_free(p) SUPC_DEALLOC_BUFF((p))
#endif

#ifndef os_memcpy
#define os_memcpy(d, s, n) kal_mem_cpy((d), (s), (n))
#endif

#ifndef os_memset
#define os_memset(s, c, n) kal_mem_set(s, c, n)
#endif

#ifndef os_memcmp
#define os_memcmp(s1, s2, n) kal_mem_cmp((s1), (s2), (n))
#endif

#ifndef os_strlen
#define os_strlen(s) strlen(s)
#endif


#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif

#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif

#ifndef os_strncmp
#define os_strncmp(s1, s2, n) strncmp((s1), (s2), (n))
#endif

#ifndef os_strncpy
#define os_strncpy(d, s, n) strncpy((d), (s), (n))
#endif

#ifndef os_strrchr
#define os_strrchr(s, c) strrchr((s), (c))
#endif

#ifndef os_strstr
#define os_strstr(h, n) strstr((h), (n))
#endif

//static __inline void *os_zalloc(kal_uint32 size)
__KAL_INLINE__ void *os_zalloc(kal_uint32 size)
{
    void *m = SUPC_ALLOC_BUFF(size);

    if (NULL == m) {
        return NULL;
    }
    kal_mem_set(m, 0, size);
    return m;
}

__KAL_INLINE__ kal_char *
os_strdup(kal_char *string)
{
    kal_uint8 length = 0, ctr = 0;
    kal_char *ptr = string;

    if (string == NULL) {
        return (kal_char *)NULL;
    }

    /* Calculate the memory requirement */
    while (*ptr++) {
        length++;
    }

    length++;

    ptr = (kal_char *) SUPC_ALLOC_BUFF(length);

    for (ctr = 0; ctr < length; ctr++) {
        *(ptr + ctr) = *string++;
    }

    return ptr;
}

typedef void (*PFN_TST_INJECT_HANDLER)(ilm_struct *ilm_ptr);

#define DIM(x)    ((sizeof(x))/(sizeof(x[0])))
//#define INNER_TRACE_STEP(x)  kal_wap_trace(MAP_MOD_SUPC,DBG_FUNC,"%s step" #x , __FUNCTION__)
#define INNER_TRACE_STEP(x)

#endif /* SUPC_COMMON_H */
