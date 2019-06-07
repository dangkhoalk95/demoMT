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

#include "cmux_porting.h"
#include "cmux_public_def.h"

#ifdef __CMUX_SUPPORT__

static kal_uint8 g_channel_num =
#define CMUX_CH_BUFFER( Type, txBuf, rxBuf, rxLThreshold, rxHThreshold) 1 +
#define CMUX_CH_CONFIG( DLCI, CL, P, N1)
#include "cmux_setting.h"
#undef CMUX_CH_BUFFER
#undef CMUX_CH_CONFIG
    0 ;

static kal_uint8 g_rx_buffer[
#define CMUX_CH_BUFFER( Type, txBuf, rxBuf, rxLThreshold, rxHThreshold) rxBuf +
#define CMUX_CH_CONFIG( DLCI, CL, P, N1)
#include "cmux_setting.h"
#undef CMUX_CH_BUFFER
#undef CMUX_CH_CONFIG
    0
];

static kal_uint8 g_tx_buffer[
#define CMUX_CH_BUFFER( Type, txBuf, rxBuf, rxLThreshold, rxHThreshold) txBuf +
#define CMUX_CH_CONFIG( DLCI, CL, P, N1)
#include "cmux_setting.h"
#undef CMUX_CH_BUFFER
#undef CMUX_CH_CONFIG
    0
];

typedef struct {

    kal_uint8   channel_type;
    kal_uint32  tx_buffer_size;
    kal_uint32  rx_buffer_size;
    kal_uint32  rx_low_threshold;
    kal_uint32  rx_high_threshold;

    kal_uint8   *tx_buffer;
    kal_uint8   *rx_buffer;

} DlcBufStructT;

static DlcBufStructT g_dlc_buffer_config[] = {
#define CMUX_CH_BUFFER( Type, txBuf, rxBuf, rxLThreshold, rxHThreshold) \
        { Type, txBuf, rxBuf, rxLThreshold, rxHThreshold, NULL, NULL },
#define CMUX_CH_CONFIG( DLCI, CL, P, N1)
#include "cmux_setting.h"
#undef CMUX_CH_BUFFER
#undef CMUX_CH_CONFIG
};

typedef struct {

    kal_uint8   dlci;
    kal_uint8   cltype;
    kal_uint8   priority;
    kal_uint16  n1;

} DlcConfigStructT;

static DlcConfigStructT g_dlc_pn_config[] = {
#define CMUX_CH_BUFFER( Type, txBuf, rxBuf, rxLThreshold, rxHThreshold)
#define CMUX_CH_CONFIG( DLCI, CL, P, N1) { (DLCI), (CL), (P), (N1)},
#include "cmux_setting.h"
#undef CMUX_CH_BUFFER
#undef CMUX_CH_CONFIG
};

kal_uint8 cmux_getDlcType(kal_uint8 channel)
{
    if ( channel >= g_channel_num ) {
        configASSERT(0);
    }

    return g_dlc_buffer_config[channel].channel_type;
}

kal_uint8 *cmux_getDlcRxBuffer(kal_uint8 channel, kal_uint32 *size,
                               kal_uint32 *low_threshold, kal_uint32 *high_threshold)
{
    if ( channel >= g_channel_num ) {
        configASSERT(0);
    }

    *size = g_dlc_buffer_config[channel].rx_buffer_size;
    *low_threshold  = g_dlc_buffer_config[channel].rx_low_threshold;
    *high_threshold  = g_dlc_buffer_config[channel].rx_high_threshold;

    if ( g_dlc_buffer_config[channel].rx_buffer == NULL ) {
        kal_uint8 *rx_buf = g_rx_buffer;
        kal_uint8 i;

        for (i = 0; i < channel; i++) {
            rx_buf += g_dlc_buffer_config[i].rx_buffer_size;
        }
        g_dlc_buffer_config[channel].rx_buffer = rx_buf;
    }
    return g_dlc_buffer_config[channel].rx_buffer;
}

kal_uint8 *cmux_getDlcTxBuffer( kal_uint8 channel, kal_uint32 *size)
{
    if ( channel >= g_channel_num ) {
        configASSERT(0);
    }

    *size = g_dlc_buffer_config[channel].tx_buffer_size;

    if ( g_dlc_buffer_config[channel].tx_buffer == NULL ) {
        kal_uint8 *tx_buf = g_tx_buffer;
        kal_uint8 i;

        for (i = 0; i < channel; i++) {
            tx_buf += g_dlc_buffer_config[i].tx_buffer_size;
        }
        g_dlc_buffer_config[channel].tx_buffer = tx_buf;
    }
    return g_dlc_buffer_config[channel].tx_buffer;
}

kal_bool cmux_custom_isPrePn(void)
{
    return CMUX_CHANNEL_PRE_PN;
}

kal_uint8 cmux_custom_getChannelDlci(kal_uint8 channel)
{
    if ( (channel >= g_channel_num) || ( g_dlc_pn_config[channel].dlci > 32 )) {
        configASSERT(0);
    }

    return g_dlc_pn_config[channel].dlci;
}

kal_uint8 cmux_custom_getChannelClType(kal_uint8 channel)
{
    if ( channel >= g_channel_num || ( g_dlc_pn_config[channel].cltype == CMUX_CL3 ) ) {
        configASSERT(0);
    }

    return g_dlc_pn_config[channel].cltype;
}

kal_uint8 cmux_custom_getChannelPriority(kal_uint8 channel)
{
    if ( channel >= g_channel_num ) {
        configASSERT(0);
    }

    return g_dlc_pn_config[channel].priority;
}

kal_uint16 cmux_custom_getChannelN1(kal_uint8 channel)
{
    if ( channel >= g_channel_num || ( g_dlc_pn_config[channel].n1 < 16 )
            || ( g_dlc_pn_config[channel].n1 > 1510 ) ) {
        configASSERT(0);
    }

    return g_dlc_pn_config[channel].n1;
}

kal_uint8 cmux_custom_getChannelIndex(kal_uint8 dlci)
{
    kal_uint8 i;

    if ( dlci > 32 ) {
        configASSERT(0);
    }

    for (i = 0; i < g_channel_num; i++) {
        if (dlci == g_dlc_pn_config[i].dlci) {
            return i;
        }
    }

    configASSERT(0);
    return -1;
}

#endif /* __CMUX_SUPPORT__ */

