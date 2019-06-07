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

#include "exception_handler.h"
#include "memory_map.h"

/******************************************************************************/
/*            Memory Regions Definition                                       */
/******************************************************************************/
#if defined(__GNUC__)

///TODO: MOVE THESE MEMORY REGIONS INTO 2625 PROJECTS

#if  (PRODUCT_VERSION == 2625)

extern unsigned int Image$$RAM_TEXT$$Base[];
extern unsigned int Image$$RAM_TEXT$$Limit[];
extern unsigned int Image$$NONCACHED_DATA$$Base[];
extern unsigned int Image$$NONCACHED_ZI$$Limit[];
extern unsigned int Image$$MD_NONCACHED_RAM_DATA$$Base[];
extern unsigned int Image$$MD_NONCACHED_RAM_ZI$$Limit[];
extern unsigned int Image$$MD_CACHED_RAM_DATA$$RW$$Base[];
extern unsigned int Image$$MD_CACHED_RAM_DATA$$ZI$$Limit[];
extern unsigned int Image$$CACHED_DATA$$RW$$Base[];
extern unsigned int Image$$CACHED_DATA$$ZI$$Limit[];
extern unsigned int Image$$TCM$$RO$$Base[];
extern unsigned int Image$$TCM$$ZI$$Limit[];
extern unsigned int Image$$MD_TCM$$RO$$Base[];
extern unsigned int Image$$MD_TCM$$ZI$$Limit[];
extern unsigned int Image$$STACK$$ZI$$Base[];
extern unsigned int Image$$STACK$$ZI$$Limit[];
extern unsigned int Image$$MD_RETSRAM_DATA$$RW$$Base[];
extern unsigned int Image$$MD_RETSRAM_DATA$$RW$$Limit[];

const memory_region_type memory_regions[] =
{
    {"ram_text", Image$$RAM_TEXT$$Base, Image$$RAM_TEXT$$Limit, 1},
    {"noncached_data", Image$$NONCACHED_DATA$$Base, Image$$NONCACHED_ZI$$Limit, 1},
    {"md_noncached_data", Image$$MD_NONCACHED_RAM_DATA$$Base, Image$$MD_NONCACHED_RAM_ZI$$Limit, 1},
    {"cached_data", Image$$CACHED_DATA$$RW$$Base, Image$$CACHED_DATA$$ZI$$Limit, 1},
    {"cached_md_data", Image$$MD_CACHED_RAM_DATA$$RW$$Base, Image$$MD_CACHED_RAM_DATA$$ZI$$Limit, 1},
    {"tcm", Image$$TCM$$RO$$Base, Image$$TCM$$ZI$$Limit, 1},
    {"md_tcm", Image$$MD_TCM$$RO$$Base, Image$$MD_TCM$$ZI$$Limit, 1},  
    {"md_retsram", Image$$MD_RETSRAM_DATA$$RW$$Base, Image$$MD_RETSRAM_DATA$$RW$$Limit, 1},
    {"stack", Image$$TCM$$ZI$$Limit, Image$$STACK$$ZI$$Limit, 1},
    {0}
};

#ifdef MTK_SWLA_ENABLE
extern unsigned int Image$$SWLA$$Base[];
extern unsigned int Image$$SWLA$$Limit[];

void SLA_get_region(uint32_t *pxBase, uint32_t *pxLen)
{
    *pxBase = (((uint32_t)Image$$NONCACHED_ZI$$Limit | VRAM_BASE) + 0x20) & ~(0x20 - 1);  /* align up to 32Byte */
    *pxLen = (uint32_t)(((uint32_t)Image$$SWLA$$Limit - *pxBase) ) & ~(0x20 - 1);  /* swla buffer area is free ram + swla  reserve area */
}
#endif /* MTK_SWLA_ENABLE */

#endif /*PRODUCT_VERSION == 2625 */




#endif /* __GNUC__ */

#if defined (__CC_ARM)





#endif /* __CC_ARM */

#if defined(__ICCARM__)



#endif /* __ICCARM__ */

