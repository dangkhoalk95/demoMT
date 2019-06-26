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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
     
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "hal_aes.h"
#include "smt_conn.h"
#include "smt_api.h"

#include "wifi_scan.h"
#include "wifi_api.h"

#include "wifi_api.h"


typedef enum
{
    RECE_FROM_APK,
    GET_AP_CH
}SMT_STAT_E;

#undef BIT
#define BIT(n) (0x0001 << (n))


extern smtcn_proto_ops elian_proto_ops;
extern smtcn_proto_ops bsmtcn_proto_ops;

static SMT_STAT_E smt_stat = RECE_FROM_APK;

#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)

static int32_t cur_protocol = USE_BOTH ;

static bool need_recover_from_timeout = TRUE;

void smt_ops_set_protocol(int32_t protocol)
{
    cur_protocol = protocol;

    need_recover_from_timeout = (protocol == USE_BOTH ? true:false);
}
#endif
#if defined(MTK_SMTCN_V4_ENABLE) || defined(MTK_SMTCN_V5_ENABLE)

static void smt_env_report(e_multi_protocol_evt_type type)
{
    uint32_t rx_filter;
    switch(type)
    {
        case EVT_BC_SYNC:
#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
            cur_protocol = USE_BC_PROTOCOL;
#endif
            smt_stat = GET_AP_CH;
            break;
        case EVT_MC_SYNC:
#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
            cur_protocol = USE_MC_PROTOCOL;
#endif
            smt_stat = GET_AP_CH;
            break;
        case EVT_SYNC_TIME_OUT:
#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
            if(need_recover_from_timeout)
                cur_protocol = USE_BOTH;
#endif
            wifi_config_get_rx_filter(&rx_filter);
            rx_filter |= BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);
            wifi_config_set_rx_filter(rx_filter);

            smt_stat = RECE_FROM_APK;
            break;
        case EVT_FINISHED:
#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
            cur_protocol = USE_BOTH;
#endif
            break;
        default:
            break;
    }
}
#endif
static int32_t multi_ops_init(const uint8_t *key, const uint8_t key_length)
{
    smt_stat = RECE_FROM_APK;

#if defined(MTK_SMTCN_V4_ENABLE)
    elian_proto_ops.init(key,key_length);
#endif

#if defined(MTK_SMTCN_V5_ENABLE)
    bsmtcn_proto_ops.init(key,key_length);
#endif
    return 0;
}


static void multi_ops_cleanup(void)
{
#if defined(MTK_SMTCN_V4_ENABLE)
    elian_proto_ops.cleanup();
#endif

#if defined(MTK_SMTCN_V5_ENABLE)
    bsmtcn_proto_ops.cleanup();
#endif
}

static int32_t multi_ops_reset(void)
{
#if defined(MTK_SMTCN_V4_ENABLE)
    elian_proto_ops.switch_channel_rst();
#endif

#if defined(MTK_SMTCN_V5_ENABLE)
    bsmtcn_proto_ops.switch_channel_rst();
#endif
    return 0;
}

static char g_ap_mac[6] = {0};

static bool get_ap_mac_by_sync_packet(char * data,int len)
{
    if((data[0] & 0x0C) == 0x08)
    {
        char * src_addr = NULL;
        char * ap_mac = NULL;
        
        if ((data[1] & 3) == 2)
        {
            /* from DS. */
            src_addr = data + 16;  
            ap_mac = data + 10;
        } 
        else if ((data[1] & 3) == 1)
        {
            /* To DS. */
            src_addr = data + 10;
            ap_mac = data + 4;
        }

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_PRINT(mac) mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]
#define IS_MULTICAST_ADDR(Addr) (((((Addr[0]) & 0x01) == 0x01) && ((Addr[0]) != 0xff)) || (Addr[0] == 0x33 && Addr[1] == 0x33))

        if(ap_mac != NULL && src_addr!= NULL)
        {
            LOG_I(smnt," src addr " MAC_FMT" ap mac "MAC_FMT"\n",MAC_PRINT(src_addr),MAC_PRINT(ap_mac));
        }
        if(ap_mac == NULL)
            return FALSE;
        
        memcpy(g_ap_mac,ap_mac,6);
        return TRUE;
    }
    return FALSE;

}


#define  CONTINUE_PARSE_BEACON -2
#define  BEACON_WITHOU_DSSSPARM -1

static int get_ch_info(char * pbuf,int len,uint8_t * channel)
{
    PFRAME_802_11 pFrame;
    uint8_t *Ptr = NULL;
    PEID_STRUCT pEid;
    int Length = 0;

    pFrame = (PFRAME_802_11)pbuf;

    Ptr = pFrame->Octet;
    Length += 24;
    
    Ptr += 8;
    Length += 8;
    

    /* get beacon interval from payload and advance the pointer*/
    Ptr += 2;
    Length += 2; 
    /* get capability info from payload and advance the pointer*/
    Ptr += 2;
    Length += 2;

    pEid = (PEID_STRUCT) Ptr;
    while ((Length + 2 + pEid->Len) <= len)  
    {
        switch(pEid->Eid)
        {
            case IE_DS_PARM:
               if(pEid->Len == 1)
               {
                   * channel = *pEid->Octet;
                   return 0;
               }
           default:
                break;
                
        }
		Length = Length + 2 + pEid->Len;  /* Eid[1] + Len[1]+ content[Len]*/
		pEid = (PEID_STRUCT)((unsigned char*)pEid + 2 + pEid->Len);  
    }

    return BEACON_WITHOU_DSSSPARM;
}
 
static int get_ap_ch(char * data,int len,uint8_t *channel)
{
    if((data[0] & 0x0C) == 0)  //management frame
    {
        if((data[0] & 0xF0) == 0x80)  //beacon frame
        {
            char * ap_mac = data + 10;
            if(memcmp(g_ap_mac,ap_mac, 6 ) == 0)
            {
                return get_ch_info(data, len,  channel);  
            }
            return CONTINUE_PARSE_BEACON;
        }
        else
        {
            return CONTINUE_PARSE_BEACON;
        }
    }
    else
        return CONTINUE_PARSE_BEACON;
}


static int32_t multi_ops_input(char *p, int32_t len, uint8_t channel)
{
    if(smt_stat == RECE_FROM_APK)
    {
#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
        switch(cur_protocol)
        {
            case USE_BOTH:
            {
                elian_proto_ops.rx_handler(p,len,channel);

                if(cur_protocol!= USE_BOTH)
                    break;
                
                bsmtcn_proto_ops.rx_handler(p,len,channel);
                break;
            }

            case USE_MC_PROTOCOL:
                elian_proto_ops.rx_handler(p,len,channel);
                break;

            case USE_BC_PROTOCOL:
                bsmtcn_proto_ops.rx_handler(p,len,channel);
                break;
            default:
                
                break;
        }
#elif defined(MTK_SMTCN_V4_ENABLE)
            elian_proto_ops.rx_handler(p,len,channel);
#else
            bsmtcn_proto_ops.rx_handler(p,len,channel);
#endif
        if(smt_stat == GET_AP_CH)
        {
            if(get_ap_mac_by_sync_packet(p,len)) 
            {
                uint32_t rx_filter;
                wifi_config_get_rx_filter(&rx_filter);
                rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);
                wifi_config_set_rx_filter(rx_filter);
            }
            else 
            {
                LOG_E(smnt,"sync packet not data frame,go to the normal flow\n");
                smt_stat = RECE_FROM_APK;
            }
        }
    }
    else if(smt_stat == GET_AP_CH)
    {
        uint32_t rx_filter;
        int ret = 0;
        uint8_t channel = 0;

        ret = get_ap_ch(p,len,&channel);
        
        if(ret == CONTINUE_PARSE_BEACON)
            return 0;
        
        if(ret == BEACON_WITHOU_DSSSPARM)
        {
            LOG_W(smnt,"can not get the AP channel infor\n");
        }
        if(ret == 0)
        {
            uint8_t org_channel;
            wifi_config_get_channel(0,&org_channel);
            if(org_channel != channel)
            {
                LOG_I(smnt,"org channel  %d , ap channel is %d\n",org_channel,channel);
                wifi_config_set_channel(0,channel);
            }
        }
        wifi_config_get_rx_filter(&rx_filter);
        rx_filter |= BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);
        wifi_config_set_rx_filter(rx_filter);
        smt_stat = RECE_FROM_APK;
    }
    return 0;
}


static smtcn_proto_ops multi_proto_ops = {
    .init               =   multi_ops_init,
    .cleanup            =   multi_ops_cleanup,
    .switch_channel_rst =   multi_ops_reset,
    .rx_handler         =   multi_ops_input,  
};



#ifdef MTK_WIFI_ROM_ENABLE
extern void RT_AES_Decrypt (
    uint8_t *CipherBlock,
    uint32_t CipherBlockSize,
    uint8_t *Key,
    uint32_t KeyLength,
    uint8_t *PlainBlock,
    uint32_t *PlainBlockSize);
#endif

void smtcn_aes_decrypt(uint8_t *cipher_blk, uint32_t cipher_blk_size,
                              uint8_t *key, uint8_t key_len,
                              uint8_t *plain_blk, uint32_t *plain_blk_size)
{
 
#ifdef MTK_WIFI_ROM_ENABLE
    //RT_AES_Decrypt(cipher_blk,cipher_blk_size,key,key_len,plain_blk,plain_blk_size);

    hal_aes_buffer_t * plain_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));
    hal_aes_buffer_t * cipher_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));
    hal_aes_buffer_t * key_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));

    uint8_t * cpt = (uint8_t *)pvPortMallocNC(cipher_blk_size);
    uint8_t * plt = (uint8_t *)pvPortMallocNC(*plain_blk_size);
    uint8_t * p_key = (uint8_t *)pvPortMallocNC(key_len);
    
    if(cpt == NULL || plt == NULL || p_key == NULL || plain_buf == NULL || cipher_buf == NULL || key_buf == NULL)
    {
        configASSERT(0);
        return ;
    }

    memcpy(cpt,cipher_blk,cipher_blk_size);
    memcpy(p_key,key,key_len);

    
    plain_buf->buffer = plt;
    plain_buf->length = *plain_blk_size;
    cipher_buf->buffer = cpt;
    cipher_buf->length = cipher_blk_size;
    key_buf->buffer = p_key;
    key_buf->length = key_len;
    hal_aes_ecb_decrypt(plain_buf, cipher_buf, key_buf); 
    memcpy(plain_blk,plt,*plain_blk_size);
    vPortFreeNC(cpt);
    vPortFreeNC(plt);
    vPortFreeNC(p_key);
    vPortFreeNC(plain_buf);
    vPortFreeNC(cipher_buf);
    vPortFreeNC(key_buf);
    
#else
    hal_aes_buffer_t plain_buf, cipher_buf, key_buf;
    plain_buf.buffer = plain_blk;
    plain_buf.length = *plain_blk_size;
    cipher_buf.buffer = cipher_blk;
    cipher_buf.length = cipher_blk_size;
    key_buf.buffer = key;
    key_buf.length = key_len;
    hal_aes_ecb_decrypt(&plain_buf, &cipher_buf, &key_buf); 
#endif
}


smtcn_proto_ops * smt_get_proto_ops()
{
    #if defined(MTK_SMTCN_V4_ENABLE) 
        elian_register_evt_cb(smt_env_report);
    #endif

    #if defined(MTK_SMTCN_V5_ENABLE)
        bcsmtcn_register_evt_cb(smt_env_report);
    #endif
        return &multi_proto_ops; 
}



