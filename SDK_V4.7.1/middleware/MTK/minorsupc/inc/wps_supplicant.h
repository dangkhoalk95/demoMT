/*
 * wpa_supplicant / WPS integration
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

#ifndef WPS_SUPPLICANT_H
#define WPS_SUPPLICANT_H

#ifdef __WPS_SUPPORT__

#include "supc_wps.h"
#include "supc_wps_defs.h"

#if defined( __CC_ARM )
#define __KAL_INLINE__                  static __inline
#elif defined( __ICCARM__ )
#define __KAL_INLINE__                  static inline
#else
#define __KAL_INLINE__                  static __inline
#endif

kal_int32 wpas_wps_init(struct wpa_supplicant *wpa_s);
void wpas_wps_deinit(struct wpa_supplicant *wpa_s);
kal_int32 wpas_wps_eapol_cb(struct wpa_supplicant *wpa_s);
enum wps_request_type wpas_wps_get_req_type(struct wpa_ssid *ssid);

int wpas_wps_ssid_wildcard_ok(struct wpa_supplicant *wpa_s,
                              struct wpa_ssid *ssid,
                              wndrv_supc_bss_info_struct *bss);


int wpas_wps_start_pbc(struct wpa_supplicant *wpa_s, const u8 *bssid,
                       const u8 *ssid_name, wpa_size_t ssid_len);
int wpas_wps_start_pin(struct wpa_supplicant *wpa_s, const u8 *bssid,
                       const u8 *ssid_name, wpa_size_t ssid_len,
                       const char *pin);

int wpas_wps_start_reg(struct wpa_supplicant *wpa_s, const u8 *bssid,
                       const char *pin);
int wpas_wps_ssid_bss_match(struct wpa_supplicant *wpa_s,
                            struct wpa_ssid *ssid, struct wndrv_supc_bss_info_struct *bss);
int wpas_wps_scan_pbc_overlap(struct wpa_supplicant *wpa_s,
                              struct wndrv_supc_bss_info_struct *selected,
                              struct wpa_ssid *ssid);
void wpas_wps_notify_scan_results(struct wpa_supplicant *wpa_s);
int wpas_wps_searching(struct wpa_supplicant *wpa_s);
void wpa_config_set_network_defaults(struct wpa_ssid *ssid);
void supc_wps_walk_timer_expiry_handler(void *para_hf);
void supc_wps_registrar_pbc_timer_expiry_handler(void *para_hf);
void supc_wps_reg_selected_timer_expiry_handler(void *para_hf);

#else /* __WPS_SUPPORT__ */

__KAL_INLINE__ int wpas_wps_init(struct wpa_supplicant *wpa_s)
{
    return 0;
}

__KAL_INLINE__ void wpas_wps_deinit(struct wpa_supplicant *wpa_s)
{
}

__KAL_INLINE__ int wpas_wps_eapol_cb(struct wpa_supplicant *wpa_s)
{
    return 0;
}

__KAL_INLINE__ u8 wpas_wps_get_req_type(struct wpa_ssid *ssid)
{
    return 0;
}

__KAL_INLINE__ int wpas_wps_ssid_bss_match(struct wpa_supplicant *wpa_s,
        struct wpa_ssid *ssid,
        struct wndrv_supc_bss_info_struct *bss)
{
    return -1;
}

int wpas_wps_ssid_wildcard_ok(struct wpa_supplicant *wpa_s,
                              struct wpa_ssid *ssid,
                              wndrv_supc_bss_info_struct *bss)
{
    return 0;
}

__KAL_INLINE__ int wpas_wps_scan_pbc_overlap(struct wpa_supplicant *wpa_s,
        struct wndrv_supc_bss_info_struct *selected,
        struct wpa_ssid *ssid)
{
    return 0;
}

__KAL_INLINE__ void wpas_wps_notify_scan_results(struct wpa_supplicant *wpa_s)
{
}

__KAL_INLINE__ int wpas_wps_searching(struct wpa_supplicant *wpa_s)
{
    return 0;
}

#endif /* __WPS_SUPPORT__ */

#endif /* WPS_SUPPLICANT_H */
