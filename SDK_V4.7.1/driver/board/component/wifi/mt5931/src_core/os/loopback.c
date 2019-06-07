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

#define __WNDRV_OS_FILES_C__
#include "hal.h"
#include "mtk_hw_adapter.h"
#include "mt593x_reg.h"
#include "memory_attribute.h"
#include <stdio.h>
#include "mt5931_reg.h"
#include "kal_public_api.h"
#include "wlan_config.h"


#define CMD_PKT_SIZE_FOR_IMAGE 2048
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char wndrv_cmd_temp_buf[CMD_PKT_SIZE_FOR_IMAGE + 512];
bool fgIsBusAccessFailed = false;

/*
 * Return a new random number.
 */
uint32_t random(void)
{
    uint32_t newRand;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &newRand);

    return newRand;
}

#if CFG_SUPPORT_LOOPBACK

#define LB_DelayMS(MS)  hal_gpt_delay_ms(MS)

#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
#endif /* ALIGN_4 */

#ifndef ALIGN_32
#define ALIGN_32(_value)             (((_value) + 31) & ~31u)
#endif /* ALIGN_32*/

#ifndef OFFSET_OF
#define OFFSET_OF(_type, _field)    ((unsigned int)&(((_type *)0)->_field))
#endif /* OFFSET_OF */

#ifndef BIT
#define BIT(n)                          ((unsigned int) 1UL << (n))
#endif /* BIT */

#ifndef BITS
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

#if 1

#define HAL_MCR_RD(_u4Offset, _pu4Value) \
    { \
        if (!fgIsBusAccessFailed) { \
            if (wifi_read_device_register((uint32_t)_u4Offset, (uint32_t *)_pu4Value) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                log_hal_error("HAL_MCR_RD access fail! 0x%x: 0x%x \r\n", _u4Offset, *_pu4Value); \
            } \
        } else { \
             log_hal_error("ignore HAL_MCR_RD access! 0x%x\r\n", _u4Offset); \
        } \
    }

#define HAL_MCR_WR(_u4Offset, _u4Value) \
    { \
        if (!fgIsBusAccessFailed) { \
            if (wifi_write_device_register((uint32_t)_u4Offset, (uint32_t)_u4Value) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                 log_hal_error("HAL_MCR_WR access fail! 0x%x: 0x%x \r\n", _u4Offset, _u4Value); \
            } \
        } else { \
             log_hal_error("ignore HAL_MCR_WR access! 0x%x: 0x%x \r\n", _u4Offset, _u4Value); \
        } \
     }

#define HAL_PORT_RD(_u4Port, _u4Len, _pucBuf, _u4ValidBufSize) \
    { \
        if (!fgIsBusAccessFailed) { \
            if(_u4Len > 4) \
            { \
                HAL_MCR_WR(WEHTCR, ALIGN_4(_u4Len)); \
            } \
            if (wifi_read_device_port((uint32_t)_u4Port, (uint16_t)_u4Len, (uint32_t *)_pucBuf) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                 log_hal_error("HAL_PORT_RD access fail! 0x%x\r\n", _u4Port); \
            } \
            HAL_MCR_WR(WEHTCR, 4); \
        } else { \
             log_hal_error("ignore HAL_PORT_RD access! 0x%x\r\n", _u4Port); \
        } \
    }

#define HAL_PORT_WR(_u4Port, _u4Len, _pucBuf, _u4ValidBufSize) \
    { \
        if (!fgIsBusAccessFailed) { \
            if(_u4Len > 4) \
            { \
                HAL_MCR_WR(WEHTCR, ALIGN_4(_u4Len)); \
            } \
            if (wifi_write_device_port((uint32_t)_u4Port, (uint16_t)_u4Len, (uint8_t *)_pucBuf) == FALSE) {\
                fgIsBusAccessFailed = TRUE; \
                 log_hal_error("HAL_PORT_WR access fail! 0x%x\r\n", _u4Port); \
            } \
            HAL_MCR_WR(WEHTCR, 4); \
        } else { \
             log_hal_error("ignore HAL_PORT_WR access! 0x%x\r\n", _u4Port); \
        } \
    }

#endif


#define WLAN_LB_STATUS_SUCCESS  0
#define WLAN_LB_STATUS_FAILURE 1

#define LOOPBACK_TX_BODY_MAX_SIZE        CMD_PKT_SIZE_FOR_IMAGE /*max test length*/
#define HIF_HW_TX_HDR_SIZE               OFFSET_OF(HIF_HW_TX_HEADER_T, aucBuffer[0])
#define LOOPBACK_TX_HEADER_SZIE          sizeof(LB_NIC_HIF_TX_HEADER_T)

#define CFG_FW_LOAD_ADDRESS                     0xFF900000
#define CFG_FW_START_ADDRESS                    0x00000000


/*LoopBack TX Data Header */
typedef struct _LB_NIC_HIF_TX_HEADER_T {
    unsigned short  u2TxByteCount;
    unsigned char       ucEtherTypeOffset;
    unsigned char       ucCSUMFlags;
} LB_NIC_HIF_TX_HEADER_T, *P_LB_NIC_HIF_TX_HEADER_T;

/*LoopBack RX Data Header */
typedef struct _LB_NIC_HIF_RX_HEADER_T {
    unsigned short  u2PacketLen;
    unsigned short  u2PacketType;
} LB_NIC_HIF_RX_HEADER_T, *P_LB_NIC_HIF_RX_HEADER_T;

typedef struct _HIF_HW_TX_HEADER_T {
    uint16_t     u2TxByteCount;
    uint8_t      ucEtherTypeOffset;
    uint8_t      ucCSflags;
    uint8_t      aucBuffer[4];
} HIF_HW_TX_HEADER_T, *P_HIF_HW_TX_HEADER_T;


//extern wndrv_context_struct wndrv_context;
extern unsigned char const *wndrv_firmware_p;
extern unsigned int wndrv_firmware_size;
extern void wndrv_firmware_init(void);

void
dumpMemory8(
    unsigned char  *pucStartAddr,
    unsigned int   u4Length
)
{


    log_hal_info("DUMP8 ADDRESS: %08x, Length: %d \r\n\r\n", (unsigned int)pucStartAddr, u4Length);

    while (u4Length > 0) {
        if (u4Length >= 16) {
            log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x %02x %02x\r\n",
                         (unsigned int)pucStartAddr,
                         pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                         pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                         pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                         pucStartAddr[12], pucStartAddr[13], pucStartAddr[14], pucStartAddr[15]);
            u4Length -= 16;
            pucStartAddr += 16;
        } else {
            switch (u4Length) {
                case 1:
                    log_hal_info("(%08x) %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0]);
                    break;
                case 2:
                    log_hal_info("(%08x) %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1]);
                    break;
                case 3:
                    log_hal_info("(%08x) %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2]);
                    break;
                case 4:
                    log_hal_info("(%08x) %02x %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3]);
                    break;
                case 5:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4]);
                    break;
                case 6:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5]);
                    break;
                case 7:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6]);
                    break;
                case 8:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7]);
                    break;
                case 9:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8]);
                    break;
                case 10:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8], pucStartAddr[ 9]);
                    break;
                case 11:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10]);
                    break;
                case 12:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11]);
                    break;
                case 13:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                                 pucStartAddr[12]);
                    break;
                case 14:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                                 pucStartAddr[12], pucStartAddr[13]);
                    break;
                case 15:
                    log_hal_info("(%08x) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x %02x\r\n",
                                 (unsigned int)pucStartAddr,
                                 pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                                 pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                                 pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                                 pucStartAddr[12], pucStartAddr[13], pucStartAddr[14]);
                    break;
                default:
                    break;
            }
            u4Length = 0;
        }
    }
}

static unsigned char LB_wlanImageSectionDownload(
    unsigned int     u4DestAddr,
    unsigned int     u4ImgSecSize,
    unsigned char    *pucImgSecBuf
)
{

    unsigned int  i, u4Value;
    P_HIF_HW_TX_HEADER_T prHifTxHeader;
    unsigned char  ucstatus = WLAN_LB_STATUS_SUCCESS;

    //log_hal_info( "Destination: 0x%08x / Length: 0x%08x\r\n", u4DestAddr, u4ImgSecSize);

    // 1. Setup buffer
    prHifTxHeader = (P_HIF_HW_TX_HEADER_T)(&wndrv_cmd_temp_buf[0]);

    // 2. Setup HIF_TX_HEADER
    prHifTxHeader->u2TxByteCount = (unsigned short)(ALIGN_4(HIF_HW_TX_HDR_SIZE + u4ImgSecSize));
    prHifTxHeader->ucEtherTypeOffset = 0;
    prHifTxHeader->ucCSflags = 0;

    // 3. Copy payload
    memcpy(prHifTxHeader->aucBuffer, pucImgSecBuf, u4ImgSecSize);

    // 3.1 add 4-bytes zero tail
    memset(&(prHifTxHeader->aucBuffer[ALIGN_4(u4ImgSecSize)]), 0, HIF_HW_TX_HDR_SIZE);

    // 4. Poll til FWDL_RDY = 1
    i = 0;
    while (1) {
        HAL_MCR_RD(MCR_FWDLSR, &u4Value);

        if (u4Value & FWDLSR_FWDL_RDY) {
            //log_hal_info( "FWDL_RDY detected\r\n", u4DestAddr, u4ImgSecSize);
            break;
        } else if (i >= 512) {
            log_hal_error("Waiting for FWDL_RDY: Timeout (0x%x)\r\n", u4Value);
            ucstatus = WLAN_LB_STATUS_FAILURE;
            break;
        } else {
            i++;
            LB_DelayMS(10);
        }
    }
    // 5. Send firmware
    HAL_MCR_WR(MCR_EHTCR, ALIGN_4(prHifTxHeader->u2TxByteCount));
    HAL_PORT_WR(MCR_FWDLDR, ALIGN_4(prHifTxHeader->u2TxByteCount), (unsigned char *)prHifTxHeader, 0);
    HAL_MCR_WR(MCR_EHTCR, 4);

    return ucstatus;
}

unsigned char LB_WIFI_Chip_DownLoad_FW(void *pvFwImageMapFile, unsigned long u4FwImageFileLength)
{
    unsigned int  u4FwLoadAddr = CFG_FW_LOAD_ADDRESS;
    unsigned int reg_value, u4Value, u4ImgSecSize;
    unsigned int i;
    unsigned char u4Status = WLAN_LB_STATUS_SUCCESS;


    HAL_MCR_RD(MCR_WCIR, &reg_value);
    log_hal_info(">>>>>>>>> 1. Get Chip ID/Revision ID = 0x%08x\r\n", reg_value);
    if ((reg_value & 0x0000ffff) != 0x00005931) {
        log_hal_error("[ERROR]Can not Get Right ChipID!!!! \r\n");
    }

    if (pvFwImageMapFile) {
        do {
#if 0 //CFG_SUPPORT_WHOLE_CHIP_RESET
            //4 <1.0> whole-chip reset except HIFSYS
            HAL_MCR_WR(wndrv_context.adpt_p, MCR_WMCSR, WMCSR_CHIP_RST);
            HAL_MCR_WR(wndrv_context.adpt_p, MCR_WMCSR, 0);

            //4 <1.0.1> delay for EEIF ready
            LB_DelayMS(120);
#endif

            //4 <1.1> wait for INIT_RDY
            log_hal_info(">>>>>>>>> 2.Waiting for Init Ready bit..\r\n");
            i = 0;
            while (1) {
                HAL_MCR_RD(MCR_WMCSR, &u4Value);

                if (u4Value & WMCSR_INI_RDY) {
                    log_hal_info("Init Ready OK\r\n");
                    break;
                } else if (i >= 512) {
                    log_hal_error("[ERROR]Waiting for Init Ready bit: Timeout\r\n");
                    u4Status = WLAN_LB_STATUS_FAILURE;
                    break;
                } else {
                    i++;
                    LB_DelayMS(10);
                }
            }

            //4 <1.2> set KSEL/FLEN
            HAL_MCR_WR(MCR_FWCFG, u4FwImageFileLength >> 6);


            //4 <1.3> enable FWDL_EN
            HAL_MCR_WR(MCR_WMCSR, WMCSR_FWDLEN);

            //4 <1.4> wait for PLL_RDY
            log_hal_info(">>>>>>>>> 3.Waiting for PLL Ready bit..\r\n");
            i = 0;
            while (1) {
                HAL_MCR_RD(MCR_WMCSR, &u4Value);

                if (u4Value & WMCSR_PLLRDY) {
                    log_hal_info("PLL Ready OK\r\n");
                    break;
                } else if (i >= 512) {
                    //DBGLOG(INIT, ERROR, ("Waiting for PLL Ready bit: Timeout"));
                    log_hal_error("[ERROR]Waiting for PLL Ready bit: Timeout\r\n");
                    u4Status = WLAN_LB_STATUS_FAILURE;
                    break;
                } else {
                    i++;
                    LB_DelayMS(10);
                }
            }

            //4 <2.1> turn on HIFSYS firmware download mode
            HAL_MCR_WR(MCR_FWDLSR, FWDLSR_FWDL_MODE);

            //4 < 2.2> set starting address

            HAL_MCR_WR(MCR_FWDLDSAR, u4FwLoadAddr);

            //4 < 3> upload firmware
            log_hal_info(">>>>>>>>> 4. Firmware scatter download...\r\n");
            log_hal_info("LB_WIFI_Chip_DownLoad_FW\r\n");
            log_hal_info("Download Address: 0x%08x\r\n", u4FwLoadAddr);
            log_hal_info("Firmware Length:	0x%08x\r\n", u4FwImageFileLength);
            for (i = 0; i < u4FwImageFileLength ; i += CMD_PKT_SIZE_FOR_IMAGE) {
                if (i + CMD_PKT_SIZE_FOR_IMAGE < u4FwImageFileLength) {
                    u4ImgSecSize = CMD_PKT_SIZE_FOR_IMAGE;
                } else {
                    u4ImgSecSize = u4FwImageFileLength - i;
                }

                if (LB_wlanImageSectionDownload(
                            u4FwLoadAddr + i,
                            u4ImgSecSize,
                            (unsigned char *)pvFwImageMapFile + i) != WLAN_LB_STATUS_SUCCESS) {
                    log_hal_error("[ERROR]Firmware scatter download failed11!\r\n");
                    u4Status = WLAN_LB_STATUS_FAILURE;
                    break;
                }
            }

            if (u4Status != WLAN_LB_STATUS_SUCCESS) {
                log_hal_error("[ERROR]Firmware scatter download failed22!\r\n");
                break;
            } else {
                log_hal_info("Firmware scatter download OK\r\n");
            }

            //4 <4.1>  poll FWDL_OK & FWDL_FAIL bits

            log_hal_info(">>>>>>>>> 5. Waiting for DL_OK/DL_FAIL bit..\r\n");
            i = 0;
            while (1) {
                HAL_MCR_RD(MCR_WMCSR, &u4Value);

                if (u4Value & WMCSR_DL_OK) {
                    log_hal_info("DL_OK detected\r\n");
                    break;
                } else if (i >= 512) {
                    log_hal_error("[ERROR]Waiting for DL_OK/DL_FAIL bit: Timeout\r\n");
                    u4Status = WLAN_LB_STATUS_FAILURE;
                    break;
                } else {
                    i++;
                    LB_DelayMS(10);
                }
            }

            //4 <4.2>  turn off HIFSYS download mode
            HAL_MCR_WR(MCR_FWDLSR, 0);

        } while (0);

#if 1
        //4 < 4.3>  disable interrupt
        HAL_MCR_WR(MCR_WHLPCR, WHLPCR_INT_EN_CLR);

#endif


        log_hal_info(">>>>>>>>> 6. Waiting for Ready bit..\r\n");
        //4 <5> check Wi-Fi FW asserts ready bit
        i = 0;
        while (1) {
            HAL_MCR_RD(MCR_WCIR, &u4Value);

            if (u4Value & WCIR_WLAN_READY) {
                log_hal_info("Ready bit OK\r\n");
                break;
            } else if (i >= 512) {
                log_hal_error("[ERROR]Waiting for Ready bit: Timeout\r\n");
                u4Status = WLAN_LB_STATUS_FAILURE;
                break;
            } else {
                i++;
                LB_DelayMS(10);
            }
        }

        //4 <6.0> Driver Get LP OwnerShip
        i = 0;
        while (1) {
            HAL_MCR_RD(MCR_WHLPCR, &u4Value);

            if (u4Value & WHLPCR_FW_OWN_REQ_SET) {
                u4Status = WLAN_LB_STATUS_SUCCESS;
                break;
            } else {
                if ((i & 255) == 0) {
                    HAL_MCR_WR(MCR_WHLPCR, WHLPCR_FW_OWN_REQ_CLR);
                }
                LB_DelayMS(1);
                i++;
            }
        }
        if (u4Status == WLAN_LB_STATUS_SUCCESS) {
            log_hal_info(">>>>>>>>> 7. Driver LP ownership OK\r\n");
        }

        //4 <6.1> Set default  ISR event
        HAL_MCR_WR(MCR_WHIER, WHIER_DEFAULT);

        //4 <6.2>Set some CR again after get LP OWN
        i = 1 ;
        u4Value = 0;
        HAL_MCR_RD(MCR_WHCR, &u4Value);
        u4Value &= ~WHCR_MAX_HIF_RX_LEN_NUM;
        u4Value |= ((((unsigned int)i) << 4) & WHCR_MAX_HIF_RX_LEN_NUM);
        HAL_MCR_WR(MCR_WHCR, u4Value);

        //4 <6.3> release tx resource
        HAL_MCR_RD(MCR_WHISR, &u4Value); //
        if (u4Value & WHISR_TX_DONE_INT) {
            HAL_MCR_RD(MCR_WTSR0, &u4Value);
            HAL_MCR_RD(MCR_WTSR1, &u4Value);
        }
    } else {
        log_hal_error("No Firmware found!\r\n");
        u4Status = WLAN_LB_STATUS_FAILURE;
    }
    return u4Status;
}

unsigned char loopback_init(void)
{

    unsigned char rStatus = WLAN_LB_STATUS_SUCCESS;
    //1. Power on MT5931
    wifi_chip_hw_power_on();

    // 2. SDIO init
    wifi_sdio_open();
    wifi_sdio_init();

    //3. Init FW pointer:wndrv_firmware_p,wndrv_firmware_size
    wndrv_firmware_init();

    //4. Download MT5931 FW
    rStatus = LB_WIFI_Chip_DownLoad_FW((void *)wndrv_firmware_p, wndrv_firmware_size);
    if (rStatus != WLAN_LB_STATUS_SUCCESS) {
        return WLAN_LB_STATUS_FAILURE;
    }

    return rStatus;
}

void wndrv_test_interrupt_handler(void)
{
    static unsigned int count = 0;

    count++;
    log_hal_info("wndrv_test_interrupt_handler at %d\r\n", count);
}

void wndrv_test_interrupt_test(char *inject_string)
{
    unsigned char flag;

    sscanfl(inject_string, "%d\r\n", &flag);

    if (flag == 1) {
        wifi_register_eint_callback((void *)wndrv_test_interrupt_handler);
        log_hal_info("WiFi_EintRegister\n");
    } else if (flag == 2) {
        wifi_eint_mask();
        log_hal_info("WiFi_EintMask\n");
    } else if (flag == 3) {
        wifi_eint_unmask();
        log_hal_info("WiFi_EintUnMask\n");
    }
    return;
}


unsigned char loopback_entry(unsigned char *input, unsigned short InOut_len, unsigned char *output)
{
    LB_NIC_HIF_TX_HEADER_T txHeader;
    unsigned char rStatus = WLAN_LB_STATUS_SUCCESS;
    unsigned int i, txtotallen, rxtotallen, reg_value, offset;
    unsigned int ucPortIdx = MCR_WRDR0;
    unsigned char *pucTxRxBuf = NULL; /*TX RX BUFFER */

    pucTxRxBuf = &wndrv_cmd_temp_buf[0];

    if (InOut_len > LOOPBACK_TX_BODY_MAX_SIZE) {
        log_hal_error("packet len(%d) > maxmum(%d)\r\n", InOut_len, LOOPBACK_TX_BODY_MAX_SIZE);
        rStatus = WLAN_LB_STATUS_FAILURE;
        return rStatus;
    }

    //1. tx: send data
    txtotallen = ALIGN_4(InOut_len + LOOPBACK_TX_HEADER_SZIE);
    memset(pucTxRxBuf, 0, txtotallen);

    txHeader.u2TxByteCount = (unsigned short)txtotallen;
    txHeader.ucEtherTypeOffset = (sizeof(LB_NIC_HIF_TX_HEADER_T) + 12) / 2;
    txHeader.ucCSUMFlags = (0x2 << 6);

    offset = 0;
    memcpy(&pucTxRxBuf[offset], &txHeader, sizeof(txHeader));
    offset = sizeof(txHeader);
    memcpy(&pucTxRxBuf[offset], input, InOut_len);

    log_hal_info("loopback_test TX\r\n");
    dumpMemory8(pucTxRxBuf, txtotallen);

    HAL_PORT_WR(MCR_WTDR0, ALIGN_4(txtotallen), (unsigned char *)pucTxRxBuf, 0);

    //2. rx: receive data
    memset(pucTxRxBuf, 0, txtotallen);
    for (i = 512; i != 0; i--) {
        HAL_MCR_RD(MCR_WHISR, &reg_value);

        log_hal_info("i=%d,WHISR:0x%x\r\n", i, reg_value);
        if (reg_value & WHIER_TX_DONE_INT_EN) {  /* check TX0 TX1 done */
            HAL_MCR_RD(MCR_WTSR0, &reg_value); //release TX0
            if (reg_value) {
                log_hal_info("loopback TX done,release MCR_WTSR0:0x%x\r\n", reg_value);
            }

            HAL_MCR_RD(MCR_WTSR1, &reg_value); //release TX1
            if (reg_value) {
                log_hal_info("loopback TX done,release MCR_WTSR1:0x%x\r\n", reg_value);
            }
        } else if (reg_value & (WHIER_RX0_DONE_INT_EN | WHIER_RX1_DONE_INT_EN)) { /* check RX0/RX1  done */
            HAL_MCR_RD(MCR_WRPLR, &reg_value);
            rxtotallen = reg_value & BITS(0, 15);
            if (rxtotallen != 0) {
                ucPortIdx = MCR_WRDR0;
                log_hal_info("RX0 packet len=%d\r\n", rxtotallen);
            } else {
                ucPortIdx = MCR_WRDR1;
                rxtotallen = (reg_value & BITS(16, 31)) >> 16;
                log_hal_info("RX1 packet len=%d\r\n", rxtotallen);
            }

            HAL_PORT_RD(ucPortIdx, ALIGN_4(rxtotallen), (unsigned char *)pucTxRxBuf, 0);

            if ((((P_LB_NIC_HIF_RX_HEADER_T)(&pucTxRxBuf[0]))->u2PacketType) == 2) {
                log_hal_info("RX LOOPBACK DATA\r\n");
                break;
            }
        } else {
            LB_DelayMS(10);
        }
    }

    if (i == 0) {
        log_hal_error("[ERROR]>> loopback test fail\r\n");
        rStatus = WLAN_LB_STATUS_FAILURE;
        return rStatus;
    }
    dumpMemory8(pucTxRxBuf, rxtotallen);

    //3. indicate to caller
    for (i = 0; i < InOut_len; i++) {
        output[i] = pucTxRxBuf[i + LOOPBACK_TX_HEADER_SZIE];
    }
    return rStatus;
}


#define loopback_test_example_length 2000 //must less than LOOPBACK_TX_BODY_MAX_SIZE
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char loopback_TXbuffer[loopback_test_example_length] = {0};
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char loopback_RXbuffer[loopback_test_example_length] = {0};



/*
*  Input:    "<loopback_times>,<loopback_pattern>,<loopback_length>"
 * Return:  0:succeed, 1:fail
 */
uint8_t wndrv_test_ehpi_loopback_test(char *inject_string)
{

    unsigned short i = 0;
    unsigned char fgpass = 1;
    unsigned char rStatus = WLAN_LB_STATUS_SUCCESS;
    unsigned int loopback_times = 0;
    unsigned int loopback_length = 0;
    unsigned int loopback_pattern = 0;
    unsigned int ii = 1;
    unsigned int fail_count = 0;
    unsigned char testvalue = 0;
    unsigned char fgLoopbackInit = 0;

    sscanf(inject_string, "%d,%d,%d", &loopback_times, &loopback_pattern, &loopback_length);
    log_hal_info("loopback_times=%d,loopback_pattern=%d,loopback_length=%d\r\n", loopback_times, loopback_pattern, loopback_length);

    while ((loopback_times == 0) || (ii <= loopback_times)) {

        //=====================1.init===============
        if (!fgLoopbackInit) {
            rStatus = loopback_init();
            if (rStatus != WLAN_LB_STATUS_SUCCESS) {
                log_hal_error("Fail at loopback_init()\r\n");
                fail_count++;
                ii++;
                continue;
            } else {
                fgLoopbackInit = 1;
            }
        }
        //==================2. Prepare test data============

        if (loopback_pattern == 1) { //random length, increased data
            testvalue++;
            loopback_length = ((unsigned short)random()) % loopback_test_example_length + 1;
            for (i = 0; i < loopback_length; i++) {
                loopback_TXbuffer[i] = testvalue;
            }


        } else if (loopback_pattern == 2) { //random length ,random data
            loopback_length = ((unsigned short)random()) % loopback_test_example_length + 1; //max 2000

            for (i = 0; i < loopback_length; i++) {

                testvalue = (unsigned short)(random() & 0xff);
                loopback_TXbuffer[i] = testvalue;
            }
        } else { //fixed legnth, increased data
            testvalue++;
            for (i = 0; i < loopback_length; i++) {
                loopback_TXbuffer[i] = testvalue;
            }
        }




        //=============3. Loopback TX->RX ===================
        rStatus = loopback_entry(&loopback_TXbuffer[0], loopback_length, &loopback_RXbuffer[0]);

        if (rStatus != WLAN_LB_STATUS_SUCCESS) {
            log_hal_error("Fail at loopback_entry()\r\n");
            fail_count++;
            ii++;
            continue;
        }

        //=============4. check the result======================
        fgpass = 1;
        for (i = 0; i < loopback_length; i++) {
            if (loopback_TXbuffer[i] != loopback_RXbuffer[i]) {
                log_hal_error("len=%d Fail(%d/%d) at Index:0x%x,input:0x%x,ouput:0x%x\r\n", loopback_length, ii, loopback_times, i, loopback_TXbuffer[i], loopback_RXbuffer[i]);
                fgpass = 0;
                rStatus = WLAN_LB_STATUS_FAILURE;
            }

        }

        if (fgpass) {
            log_hal_info("len=%d Success(%d/%d)\r\n", loopback_length, ii, loopback_times);
            rStatus = WLAN_LB_STATUS_SUCCESS;
        } else {
            fail_count++;
        }


        log_hal_info("Total Count %d, success Count %d, Fail count %d\r\n", ii, (ii - fail_count), fail_count);
        ii++;
        LB_DelayMS(5);
    }
    return rStatus;
}


#endif /* CFG_SUPPORT_LOOPBACK */

