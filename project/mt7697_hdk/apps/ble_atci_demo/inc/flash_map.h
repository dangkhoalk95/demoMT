/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#ifdef MTK_FOTA_DUAL_IMAGE_ENABLE

#ifndef FOTA_PARTITION_B_BUILD
/* flash layout on partition A */
#define LOADER_LENGTH           0x8000        /*  32KB */
#define FOTA_CONTROL_LENGTH     0x8000        /*  32KB */
#define N9_RAMCODE_LENGTH       0x50000       /* 320KB */
#define CM4_CODE_LENGTH         0x1A0000      /* 1664KB*/
#define FOTA_LENGTH             0x1F0000      /* 1984KB*/
#define NVDM_LENGTH             0x10000       /*  64KB */

#define LOADER_BASE             0x0
#define FOTA_CONTROL_BASE       (LOADER_BASE       + LOADER_LENGTH)
#define N9_RAMCODE_BASE         (FOTA_CONTROL_BASE + FOTA_CONTROL_LENGTH)
#define CM4_CODE_BASE           (N9_RAMCODE_BASE   + N9_RAMCODE_LENGTH)
#define FOTA_BASE               (CM4_CODE_BASE     + CM4_CODE_LENGTH)
#define NVDM_BASE               (FOTA_BASE         + FOTA_LENGTH)

#define FLASH_BASE		        0x10000000

#define TCM_BASE                0x00100000
#define TCM_LENGTH              0x00010000  /* 64kB */

#else /* FOTA_PARTITION_B_BUILD */
/* flash layout on partition B */
#define LOADER_LENGTH           0x8000        /*  32KB */
#define FOTA_CONTROL_LENGTH     0x8000        /*  32KB */
#define FOTA_LENGTH             0x1F0000      /* 1984KB*/
#define N9_RAMCODE_LENGTH       0x50000       /* 320KB */
#define CM4_CODE_LENGTH         0x1A0000      /* 1664KB*/
#define NVDM_LENGTH             0x10000       /*  64KB */

#define LOADER_BASE             0x0
#define FOTA_CONTROL_BASE       (LOADER_BASE       + LOADER_LENGTH)
#define FOTA_BASE               (FOTA_CONTROL_BASE + FOTA_CONTROL_LENGTH)
#define N9_RAMCODE_BASE         (FOTA_BASE         + FOTA_LENGTH)
#define CM4_CODE_BASE           (N9_RAMCODE_BASE   + N9_RAMCODE_LENGTH)
#define NVDM_BASE               (CM4_CODE_BASE     + CM4_CODE_LENGTH)

#define FLASH_BASE		        0x10000000

#define TCM_BASE                0x00100000
#define TCM_LENGTH              0x00010000  /* 64kB */

#endif /* FOTA_PARTITION_B_BUILD */

#else /* origin layout setting */

#define LOADER_LENGTH           0x8000        /*  32KB */
#define RESERVED_LENGTH         0x8000        /*  32KB */
#define N9_RAMCODE_LENGTH       0x69000       /* 420KB */
#define CM4_CODE_LENGTH         0x1ED000      /* 1972KB */
#define FOTA_LENGTH             0x18A000      /* 1576KB */
#define NVDM_LENGTH             0x10000       /*  64KB */

#define LOADER_BASE             0x0
#define RESERVED_BASE           (LOADER_BASE     + LOADER_LENGTH)
#define N9_RAMCODE_BASE         (RESERVED_BASE   + RESERVED_LENGTH)
#define CM4_CODE_BASE           (N9_RAMCODE_BASE + N9_RAMCODE_LENGTH)
#define FOTA_BASE               (CM4_CODE_BASE   + CM4_CODE_LENGTH)
#define NVDM_BASE               (FOTA_BASE       + FOTA_LENGTH)

#define FLASH_BASE		        0x10000000

#define TCM_BASE                0x00100000
#define TCM_LENGTH              0x00010000  /* 64kB */

#endif

#endif // __FLASH_MAP_H__

