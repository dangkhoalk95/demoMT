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

#ifndef _SOC2TCPIP_STRUCT_H
#define _SOC2TCPIP_STRUCT_H

//#include "kal_release.h"
//#include "soc_api.h"

#ifdef __FLC2__
/* convert between SID and pool ID */
#define SID2SPDU_UL_ID(_s_)     (FLC2_POOL_ID_SPDU_UL_0+(_s_))
/* convert between pool ID and SID */
#define SPDU_UL_ID2SID(_flcid_) ((_flcid_)-FLC2_POOL_ID_SPDU_UL_0)
#endif /* __FLC2__ */

/* compute mbuf length */
#define MBUF_LENGTH (((sizeof(struct mbuf) + 3) >> 2) << 2)

/* TCPIP option data length */
#define TCPIP_OPT_DATA_LEN (52)

/* convert peer buf into mbuf */
#define ptom(p)  ((mbuf*)((kal_uint8 *)p + (((sizeof(struct peer_buff_struct) + 3) >> 2) << 2)))
/* convert mbuf into peer buf */
#define mtop(m)  ((peer_buff_struct*)((kal_uint8 *)m - (((sizeof(struct peer_buff_struct) + 3) >> 2) << 2)))
/* get the data in mbuf */
#define mtod(m, t)  ((t)((m)->m_data))


/* define unsigned long as u_long */
typedef unsigned long   u_long;
/* define unsigned short as u_short */
typedef unsigned short  u_short;
/* define unsigned int as u_int */
typedef unsigned int    u_int;
/* define unsigned char as u_char */
typedef unsigned char   u_char;
/* define unsigned char as u_int8_t */
typedef unsigned char   u_int8_t;
/* define char * as caddr_t */
typedef char           *caddr_t;
/* define unsigned long as u_int32_t */
typedef unsigned long   u_int32_t;
/* define unsigned short as u_int16_t */
typedef unsigned short  u_int16_t;


/* TCPIP option kind */
typedef enum {
    TCPIP_OPT_GETADDR, /* get socket name */
    TCPIP_OPT_NODELAY, /* disable Nagle algorithm or not */
    TCPIP_OPT_KEEPALIVE, /* keep alive option */
    TCPIP_OPT_RCVBUF, /* receive buffer size */
    TCPIP_OPT_SENDBUF, /* send buffer size */
    TCPIP_OPT_QOS, /* quality of service */
    TCPIP_OPT_TCP_MAXSEG, /* max segment size */
    TCPIP_OPT_IP_TTL, /* time to life */
#if 0 /* TODO: Add support for linger option */
    /* under construction !*/
#endif /* TODO: Add support for linger option */
    TCPIP_OPT_LISTEN_BEARER, /* listen bearer */
    TCPIP_OPT_UDP_ANY_FPORT, /* any foreign port */
    TCPIP_OPT_WIFI_NOWAKEUP, /* sent data but not wake up */
    TCPIP_OPT_IP_HDRINCL, /* IP header included in raw socket */
    TCPIP_OPT_IPSEC_POLICY, /* security policy */
    TCPIP_OPT_ACKED_DATA, /* acked data */
    TCPIP_OPT_DELAYED_ACK, /* delayed ack */
    TCPIP_OPT_SACK,        /* selective ack */
    TCPIP_OPT_ACK_MSEG,    /* ack many segment */
    TCPIP_OPT_TIME_STAMP   /* time stamp */
} soc_tcpip_option_enum;


/* close status */
typedef enum {
    CLOSED = 0, /* closed by user */
    FINED  = 1 /* closed by peer */
} pcb_status;

/* connection status */
typedef enum {
    CONN_SUCCESS = 0, /* connect successfully */
    CONN_FAIL /* fail to connect */
} connect_status;

/* detach cause */
typedef enum {
    DETACH_SUCCESS = 0, /* detach successfully */
    DETACH_FAIL /* detach failure */
} detach_status;



/* The structure is used to notify TCPIP layer to create a protocol control block with type (proto_type)
   and attached to socket (socket_id). */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_uint8 sock_type; /* socket type */
    kal_uint8 protocol; /* protocol */
    kal_uint8 profile_id; /* profile id */
} soc_tcpip_attach_req_struct;

/* The structure is used to notify TCPIP layer the newly created socket identification (o_socket_id) and
   original listening socket identification (n_socket_id).  */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 o_socket_id; /* old socket identifier */
    kal_int8 n_socket_id; /* new socket identifier */
    kal_uint8 profile_id; /* profile identifier */
} soc_tcpip_accept_res_struct;

//todo
#define MAX_SOCK_ADDR_LEN           (28)
#define  IP_ADDR_LEN    (4)
/* Socket Type */
typedef enum {
    SOC_SOCK_STREAM = 0,  /* stream socket, TCP */
    SOC_SOCK_DGRAM,       /* datagram socket, UDP */
    SOC_SOCK_SMS,         /* SMS bearer */
    SOC_SOCK_RAW          /* raw socket */
} socket_type_enum;
/* socket address structure */
typedef struct {
    socket_type_enum    sock_type; /* socket type */
    kal_int16   addr_len; /* address length */
    kal_uint16  port; /* port number */
    kal_uint8   addr[MAX_SOCK_ADDR_LEN];
    /* IP address. For keep the 4-type boundary,
     * please do not declare other variables above "addr"
    */
} sockaddr_struct;

/* The structure is used to notify TCPIP layer the binding socket identification (socket_id) and binding
   socket address (so_addr). */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_uint8 sock_type; /* socket type */
    sockaddr_struct so_addr; /* socket address */
    kal_uint8 profile_id; /* profile identifier */
} soc_tcpip_bind_req_struct;

/* The structure is used to notify TCPIP layer the sending socket identification (socket_id), destination
   socket address (so_addr) and application defined flag (flags). */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_uint8 sock_type; /* socket type */
    sockaddr_struct so_addr; /* socket address */
    kal_uint8 flags; /* flag */
    kal_uint8 profile_id; /* profile identifier */
} soc_tcpip_send_req_struct;

/* The structure is used to notify TCPIP layer the active bearer identification (bearer), maximum
   transmission unit (MTU) and assigned IP address (ip_addr). */
typedef struct {
    kal_uint8    ref_count; /* reference count */
    kal_uint16   msg_len; /* message length */
    /*kal_uint8    profile_id;*/
    kal_uint32    profile_id; /* profile identifier */
    kal_bool     is_activated; /* if activated */
    kal_uint8    bearer; /* soc_bearer_enum */
    kal_uint16   mtu; /* max transfer unit */
    kal_uint8    ip_addr[4]; /* IP address */
    kal_uint8    netmask[4]; /* net mask */
    kal_uint8    gateway[4]; /* gate way address */
    kal_uint8    mac_addr[6]; /* mac address */
} soc_tcpip_bearer_req_struct;

/* The structure is used to notify TCPIP layer the listening socket identification (bearer),
   and maximum accepted connection number (max_socket_no). */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_uint8 sock_type; /* socket type */
    kal_uint8 max_socket_no; /* max socket number */
    kal_uint8 profile_id; /* profile identifier */
} soc_tcpip_listen_req_struct;

/* option request struct */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */

    kal_uint8 profile_id; /* profile identifier */
    kal_int8 socket_id; /* socket identifier */
    kal_uint8 sock_type; /* socket type */
    kal_uint16 option; /* option */

    kal_bool   is_set; /* set or get option */
    kal_uint8 data_length; /* data length */
    kal_uint32 data[TCPIP_OPT_DATA_LEN / 4]; /* option data */
} soc_tcpip_option_req_struct;

/* define soc_tcpip_attach_req_struct as soc_tcpip_close_req_struct */
typedef soc_tcpip_attach_req_struct soc_tcpip_close_req_struct;
/* define soc_tcpip_attach_req_struct as soc_tcpip_shutdown_req_struct */
typedef soc_tcpip_attach_req_struct soc_tcpip_shutdown_req_struct;
/* define soc_tcpip_attach_req_struct as soc_tcpip_detach_req_struct */
typedef soc_tcpip_attach_req_struct soc_tcpip_detach_req_struct;
/* define soc_tcpip_attach_req_struct as soc_tcpip_disconnect_req_struct */
typedef soc_tcpip_attach_req_struct soc_tcpip_disconnect_req_struct;
/* define soc_tcpip_attach_req_struct as soc_tcpip_connect_req_struct */
typedef soc_tcpip_bind_req_struct soc_tcpip_connect_req_struct;


/* The structure is used to notify SOC layer the result of socket attachment (result). */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_int8 result; /* operation result */
} soc_tcpip_attach_cnf_struct;

/* The structure is used to notify SOC layer the newly sync request is connected with listening socket (socket_id)
    and the foreign socket address is so_addr. */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 l_socket_id; /* listen socket */
    kal_int8 a_socket_id; /* temp socket to accept connection */
    sockaddr_struct so_addr; /* socket address */
} soc_tcpip_accept_ind_struct;

/* The structure is used to send incoming datagram to SOC layer with socket (socket_id) and special data
   type(flags). */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_uint8 flags; /*  flag */
} soc_tcpip_receive_ind_struct;

/*  option confirm struct */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 result; /* operation result */
    kal_int8 socket_id; /* socket identifier */
    kal_uint16 option; /* option */
    kal_uint8 data_length; /* data length  */
    kal_uint32 data[TCPIP_OPT_DATA_LEN / 4]; /* option data */
} soc_tcpip_option_cnf_struct;

/*  error message struct */
typedef struct {
#define TCPIP_ERRMSG_LEN    (16)
    kal_uint8       ref_count; /* reference count */
    kal_uint16  msg_len; /* message length */

    kal_int8    profile_id; /* profile identifier */
    kal_int8    socket_id; /* socket identifier */
    kal_uint8   protocol; /* IP, TCP, UDP, ICMP, ... */
    kal_uint16  cause; /* IP conflict, ... */

    kal_uint8       errmsg1_len; /* errmsg1 length  */
    kal_uint32  errmsg1[TCPIP_ERRMSG_LEN / 4]; /* error message 1 for 4 byte aligmnent */
    kal_uint8       errmsg2_len; /* errmsg2 length  */
    kal_uint32  errmsg2[TCPIP_ERRMSG_LEN / 4]; /* error message 2 for 4 byte aligmnent */
} soc_tcpip_errmsg_ind_struct;

/* IP conflict */
typedef enum {
    IP_CONFLICT     /* IP conflict in LAN */
} tcpip_errmsg_cause_enum;

/* define soc_tcpip_attach_cnf_struct as soc_tcpip_bind_cnf_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_bind_cnf_struct;
/* define soc_tcpip_attach_cnf_struct as soc_tcpip_connect_cnf_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_connect_cnf_struct;
/* define soc_tcpip_attach_cnf_struct as soc_tcpip_listen_cnf_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_listen_cnf_struct;
/* define soc_tcpip_attach_cnf_struct as soc_tcpip_shutdown_cnf_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_shutdown_cnf_struct;
/* define soc_tcpip_attach_cnf_struct as soc_tcpip_detach_cnf_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_detach_cnf_struct;
/* define soc_tcpip_attach_cnf_struct as soc_tcpip_close_ind_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_close_ind_struct;
/* define soc_tcpip_attach_cnf_struct as soc_tcpip_disconnect_cnf_struct */
typedef soc_tcpip_attach_cnf_struct soc_tcpip_disconnect_cnf_struct;

/* The structure is used to notify TCPIP layer the amount of newly released SRPDU. */
typedef struct {
    kal_uint8 ref_count; /* reference count */
    kal_uint16 msg_len; /* message length */
    kal_int8 socket_id; /* socket identifier */
    kal_uint16 release_amount; /* release buffer size */
} soc_tcpip_srpdu_release_res_struct;

/* packet header struct */
struct PKTHDR {
    kal_uint32  len; /* total packet length */
    kal_uint8  *header; /* header pointer */
} ;

/* The structure describes the ipv4 address information. */
typedef struct {
    kal_int16   addr_len; /* address length  */
    kal_uint8   addr[IP_ADDR_LEN]; /* address */
    kal_uint16  port; /* port number */
    kal_uint32  account; /* account ID */
} ipv4_sockaddr_struct;

/* This structure stores all processing data for TCPIP task, and is simplified from the difference between peer_buffer
   and mbuf in free open source code */
typedef struct mbuf {
    struct mbuf    *m_nextpkt;  /* next packet */
    kal_uint8      *m_data;     /* location of data */
    kal_uint16      m_len;      /* amount of data in this mbuf */
    kal_uint16      m_flags;    /* masking, ref M_SOC_ID, M_TPDU, M_FRAG, M_SMALL */
    struct mbuf    *m_next;     /* next fragment */
    struct PKTHDR   m_pkthdr;   /* packet header */
    kal_int8        socket_id;  /* identifiers for SOC */
    kal_uint8       flags;      /* flag */
    ipv4_sockaddr_struct      sockaddr; /* reserved for ipv4_sockaddr_struct */
} mbuf;

/* The structure describes the ipv4 address information. */
typedef struct {
    kal_uint8    ref_count; /* reference count */
    kal_uint16   msg_len; /* message length */
    kal_int8     socket_id; /* socket identifier */
    kal_uint16   mtu; /* max transfer unit */
} soc_tcpip_mtu_update_ind_struct;

/* define for CBM to set account */
typedef struct {
    kal_uint8    ref_count; /* reference count */
    kal_uint16   msg_len; /* message length */
    kal_int8     socket_id; /* socket identifier */
    kal_uint8    profile_id; /* profile identifier */
} soc_tcpip_set_acct_req_struct;


/* bearer context struct */
typedef struct {
    kal_uint8    ref_count; /* reference count */
    kal_uint16   msg_len; /* message length */
    kal_int8     socket_id; /* socket identifier */
    kal_uint8    context_id; /* GPRS context id*/
} soc_tcpip_bearer_ctx_req_struct;

#endif /* ~_TCPIP2SOC_STRUCT_H */

