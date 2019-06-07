/**
 * Copyright (c) 2004, Jouni Malinen <jkmaline@cc.hut.fi>
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
 * WPA Supplicant / driver interface list
 * Copyright (c) 2004, Jouni Malinen <jkmaline@cc.hut.fi>
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

#include <stdlib.h>

#ifdef CONFIG_DRIVER_MTK
extern struct wpa_driver_ops wpa_driver_mtk_ops; /* driver_mtk.c */
#endif/* CONFIG_DRIVER_HOSTAP */
#ifdef CONFIG_DRIVER_HOSTAP
extern struct wpa_driver_ops wpa_driver_hostap_ops; /* driver_hostap.c */
#endif /* CONFIG_DRIVER_HOSTAP */
#ifdef CONFIG_DRIVER_PRISM54
extern struct wpa_driver_ops wpa_driver_prism54_ops; /* driver_prism54.c */
#endif /* CONFIG_DRIVER_PRISM54 */
#ifdef CONFIG_DRIVER_HERMES
extern struct wpa_driver_ops wpa_driver_hermes_ops; /* driver_hermes.c */
#endif /* CONFIG_DRIVER_HERMES */
#ifdef CONFIG_DRIVER_MADWIFI
extern struct wpa_driver_ops wpa_driver_madwifi_ops; /* driver_madwifi.c */
#endif /* CONFIG_DRIVER_MADWIFI */
#ifdef CONFIG_DRIVER_ATMEL
extern struct wpa_driver_ops wpa_driver_atmel_ops; /* driver_atmel.c */
#endif /* CONFIG_DRIVER_ATMEL */
#ifdef CONFIG_DRIVER_WEXT
extern struct wpa_driver_ops wpa_driver_wext_ops; /* driver_wext.c */
#endif /* CONFIG_DRIVER_WEXT */
#ifdef CONFIG_DRIVER_NDISWRAPPER
/* driver_ndiswrapper.c */
extern struct wpa_driver_ops wpa_driver_ndiswrapper_ops;
#endif /* CONFIG_DRIVER_NDISWRAPPER */
#ifdef CONFIG_DRIVER_BROADCOM
extern struct wpa_driver_ops wpa_driver_broadcom_ops; /* driver_broadcom.c */
#endif /* CONFIG_DRIVER_BROADCOM */
#ifdef CONFIG_DRIVER_IPW
extern struct wpa_driver_ops wpa_driver_ipw_ops; /* driver_ipw.c */
#endif /* CONFIG_DRIVER_IPW */
#ifdef CONFIG_DRIVER_BSD
extern struct wpa_driver_ops wpa_driver_bsd_ops; /* driver_bsd.c */
#endif /* CONFIG_DRIVER_BSD */
#ifdef CONFIG_DRIVER_NDIS
extern struct wpa_driver_ops wpa_driver_ndis_ops; /* driver_ndis.c */
#endif /* CONFIG_DRIVER_NDIS */
#ifdef CONFIG_DRIVER_TEST
extern struct wpa_driver_ops wpa_driver_test_ops; /* driver_test.c */
#endif /* CONFIG_DRIVER_TEST */


struct wpa_driver_ops *wpa_supplicant_drivers[] = {
#ifdef CONFIG_DRIVER_MTK
    &wpa_driver_mtk_ops,
#endif /* CONFIG_DRIVER_MTK */
#ifdef CONFIG_DRIVER_HOSTAP
    &wpa_driver_hostap_ops,
#endif /* CONFIG_DRIVER_HOSTAP */
#ifdef CONFIG_DRIVER_PRISM54
    &wpa_driver_prism54_ops,
#endif /* CONFIG_DRIVER_PRISM54 */
#ifdef CONFIG_DRIVER_HERMES
    &wpa_driver_hermes_ops,
#endif /* CONFIG_DRIVER_HERMES */
#ifdef CONFIG_DRIVER_MADWIFI
    &wpa_driver_madwifi_ops,
#endif /* CONFIG_DRIVER_MADWIFI */
#ifdef CONFIG_DRIVER_ATMEL
    &wpa_driver_atmel_ops,
#endif /* CONFIG_DRIVER_ATMEL */
#ifdef CONFIG_DRIVER_WEXT
    &wpa_driver_wext_ops,
#endif /* CONFIG_DRIVER_WEXT */
#ifdef CONFIG_DRIVER_NDISWRAPPER
    &wpa_driver_ndiswrapper_ops,
#endif /* CONFIG_DRIVER_NDISWRAPPER */
#ifdef CONFIG_DRIVER_BROADCOM
    &wpa_driver_broadcom_ops,
#endif /* CONFIG_DRIVER_BROADCOM */
#ifdef CONFIG_DRIVER_IPW
    &wpa_driver_ipw_ops,
#endif /* CONFIG_DRIVER_IPW */
#ifdef CONFIG_DRIVER_BSD
    &wpa_driver_bsd_ops,
#endif /* CONFIG_DRIVER_BSD */
#ifdef CONFIG_DRIVER_NDIS
    &wpa_driver_ndis_ops,
#endif /* CONFIG_DRIVER_NDIS */
#ifdef CONFIG_DRIVER_TEST
    &wpa_driver_test_ops,
#endif /* CONFIG_DRIVER_TEST */
    NULL
};
