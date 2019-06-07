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


#ifndef WPA_SUPPLICANT_H
#define WPA_SUPPLICANT_H

#include"config_ssid.h"

/* Driver wrappers are not supposed to directly touch the internal data
 * structure used in wpa_supplicant, so that definition is not provided here.
 */
struct wpa_supplicant;

typedef enum {
    EVENT_ASSOC, EVENT_DISASSOC, EVENT_MICHAEL_MIC_FAILURE,
    EVENT_SCAN_RESULTS, EVENT_ASSOCINFO, EVENT_INTERFACE_STATUS,
    EVENT_PMKID_CANDIDATE
} wpa_event_type;

union wpa_event_data {
    struct {
        /* Optional request information data: IEs included in AssocReq
         * and AssocResp. If these are not returned by the driver,
         * WPA Supplicant will generate the WPA/RSN IE. */
        u8 *req_ies, *resp_ies;
        wpa_size_t req_ies_len, resp_ies_len;

        /* Optional Beacon/ProbeResp data: IEs included in Beacon or
         * Probe Response frames from the current AP (i.e., the one
         * that the client just associated with). This information is
         * used to update WPA/RSN IE for the AP. If this field is not
         * set, the results from previous scan will be used. If no
         * data for the new AP is found, scan results will be requested
         * again (without scan request). At this point, the driver is
         * expected to provide WPA/RSN IE for the AP (if WPA/WPA2 is
         * used). */
        u8 *beacon_ies; /* beacon or probe resp IEs */
        wpa_size_t beacon_ies_len;
    } assoc_info;
    struct {
        int unicast;
    } michael_mic_failure;
    struct {
        char ifname[20];
        enum {
            EVENT_INTERFACE_ADDED, EVENT_INTERFACE_REMOVED
        } ievent;
    } interface_status;
    struct {
        u8 bssid[ETH_ALEN];
        int index; /* smaller the index, higher the priority */
        int preauth;
    } pmkid_candidate;
};

/**
 * wpa_supplicant_event - report a driver event for wpa_supplicant
 * @wpa_s: pointer to wpa_supplicant data; this is the @ctx variable registered
 *  with wpa_driver_events_init()
 * @event: event type (defined above)
 * @data: possible extra data for the event
 *
 * Driver wrapper code should call this function whenever an event is received
 * from the driver.
 */
void wpa_supplicant_event(struct wpa_supplicant *wpa_s, wpa_event_type event,
                          union wpa_event_data *data);

/**
 * wpa_msg - conditional printf for default target and ctrl_iface monitors
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. This function is like wpa_printf(), but it also sends the
 * same message to all attached ctrl_iface monitors.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
void wpa_msg(struct wpa_supplicant *wpa_s, int level, char *fmt, ...);
//__attribute__ ((format (printf, 3, 4)));


const char *wpa_ssid_txt(u8 *ssid, wpa_size_t ssid_len);

/* Joseph */
extern int wpa_eapol_set_wep_key(void *ctx, int unicast, int keyidx,
                                 u8 *key, wpa_size_t keylen);

extern wndrv_supc_bss_info_struct *
wpa_supplicant_select_bss(struct wpa_supplicant *wpa_s, struct wpa_ssid *group,
                          struct wpa_ssid **selected_ssid);
extern int wpa_blacklist_add(struct wpa_supplicant *wpa_s, const u8 *bssid);
extern void pmksa_cache_del(u8 *aa);
extern void rsn_preauth_candidate_process(struct wpa_supplicant *wpa_s);
extern void wpa_find_assoc_pmkid(struct wpa_supplicant *wpa_s);
extern void wpa_supplicant_cancel_handover_timer(void);

#endif /* WPA_SUPPLICANT_H */
