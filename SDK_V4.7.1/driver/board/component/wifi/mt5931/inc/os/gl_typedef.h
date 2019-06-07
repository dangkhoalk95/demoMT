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

/*
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/os/linux/include/gl_typedef.h#3 $
*/

/*! \file   gl_typedef.h
    \brief  Definition of basic data type(os dependent).

    In this file we define the basic data type.
*/




#ifndef _GL_TYPEDEF_H
#define _GL_TYPEDEF_H

#if defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* Define HZ of timer tick for function kalGetTimeTick() */
#if defined(WCN_MAUI)
//#define KAL_HZ                  217 // 1/KAL_HZ = 4.615ms (GSM frame tick)
#define KAL_HZ                  KAL_TICKS_PER_SEC   //for ucos
#else
#define KAL_HZ                  (HZ)
#endif

/* Miscellaneous Equates */
#ifndef FALSE
#define FALSE               ((BOOL) 0)
#define TRUE                ((BOOL) 1)
#endif /* FALSE */

#ifndef NULL
#if defined(__cplusplus)
#define NULL            0
#else
#define NULL            ((void *) 0)
#endif
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
typedef int (*early_suspend_callback)(struct early_suspend *h);
typedef int (*late_resume_callback)(struct early_suspend *h);
#endif


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

//todo
/* Type definition for void */
#define VOID void
typedef char           INT8;
typedef unsigned char  UINT8;
typedef signed short   INT16;
typedef unsigned short UINT16;
//typedef signed long    INT32;
//typedef unsigned long  UINT32;

typedef int                     INT;



typedef void                    *PVOID;
typedef void                    **PPVOID;

/* Type definition for Boolean */
typedef unsigned char           BOOL;
typedef unsigned char           *PBOOL;
typedef unsigned char           BOOLEAN;
typedef unsigned char           *PBOOLEAN;

/* Type definition for signed integers */
typedef signed char             *PCHAR;
typedef signed char             **PPCHAR;
typedef signed char             INT_8;
typedef signed char             *PINT_8;
typedef signed char             **PPINT_8;
typedef signed short            INT_16;
typedef signed short            *PINT_16;
typedef signed short            **PPINT_16;
typedef signed long             INT_32;
typedef signed long             *PINT_32;
typedef signed long             **PPINT_32;
typedef signed long long        INT_64;
typedef signed long long        *PINT_64;
typedef signed long long        **PPINT_64;

/* Type definition for unsigned integers */
typedef unsigned char           UCHAR;
typedef unsigned char           *PUCHAR;
typedef unsigned char           **PPUCHAR;
typedef unsigned char           UINT_8;
typedef unsigned char           *PUINT_8;
typedef unsigned char           **PPUINT_8;
typedef unsigned char           *P_UINT_8;
typedef unsigned short          UINT_16;
typedef unsigned short          *PUINT_16;
typedef unsigned short          **PPUINT_16;
typedef unsigned long           ULONG;
#if 0
typedef unsigned long           UINT_32;
typedef unsigned long           *PUINT_32;
#else
typedef unsigned int           UINT_32;
typedef unsigned int           *PUINT_32;
#endif
typedef unsigned long           **PPUINT_32;
typedef unsigned long long      UINT_64;
typedef unsigned long long      *PUINT_64;
typedef unsigned long long      **PPUINT_64;

#if 0
typedef unsigned long           OS_SYSTIME;
#else
typedef unsigned int           OS_SYSTIME;
#endif

typedef unsigned long           *POS_SYSTIME;
typedef unsigned long           **PPOS_SYSTIME;

/* Type definition of large integer (64bits) union to be comptaible with
 * Windows definition, so we won't apply our own coding style to these data types.
 * NOTE: LARGE_INTEGER must NOT be floating variable.
 * <TODO>: Check for big-endian compatibility.
 */
typedef union _LARGE_INTEGER {
    struct {
        UINT_32  LowPart;
        INT_32   HighPart;
    } u;
    INT_64       QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        UINT_32  LowPart;
        UINT_32  HighPart;
    } u;
    UINT_64      QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;


typedef INT_32(*probe_card)(PVOID pvData);
typedef VOID (*remove_card)(VOID);

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#define IN  //volatile
#define OUT //volatile


//#define __KAL_INLINE__                  static __inline
#if defined( __CC_ARM )
#define __KAL_INLINE__                  static __inline
#elif defined( __ICCARM__ )
#define __KAL_INLINE__                  static inline
#else
#define __KAL_INLINE__                  static __inline
#endif

#ifndef __ICCARM__
#define __KAL_ATTRIB_PACKED__        __attribute__((__packed__))
#define __KAL_ATTRIB_ALIGN_4__       __attribute__ ((__aligned__(4)))
#else
#define __KAL_ATTRIB_PACKED__        __packed
#define __KAL_ATTRIB_ALIGN_4__       _Pragma("data_alignment=4")
#endif

#ifndef BIT
#define BIT(n)                          ((UINT_32) 1UL << (n))
#endif /* BIT */

#ifndef BITS
/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */


/* This macro returns the byte offset of a named field in a known structure
   type.
   _type - structure name,
   _field - field name of the structure */
#ifndef OFFSET_OF
#define OFFSET_OF(_type, _field)    ((UINT_32)&(((_type *)0)->_field))
#endif /* OFFSET_OF */


/* This macro returns the base address of an instance of a structure
 * given the type of the structure and the address of a field within the
 * containing structure.
 * _addrOfField - address of current field of the structure,
 * _type - structure name,
 * _field - field name of the structure
 */
#ifndef ENTRY_OF
#define ENTRY_OF(_addrOfField, _type, _field) \
        ((_type *)((PINT_8)(_addrOfField) - (PINT_8)OFFSET_OF(_type, _field)))
#endif /* ENTRY_OF */


/* This macro align the input value to the DW boundary.
 * _value - value need to check
 */
#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
#endif /* ALIGN_4 */

#ifndef ALIGN_32
#define ALIGN_32(_value)             (((_value) + 31) & ~31u)
#endif /* ALIGN_32*/

/* This macro check the DW alignment of the input value.
 * _value - value of address need to check
 */
#ifndef IS_ALIGN_4
#define IS_ALIGN_4(_value)          (((_value) & 0x3) ? FALSE : TRUE)
#endif /* IS_ALIGN_4 */

#ifndef IS_NOT_ALIGN_4
#define IS_NOT_ALIGN_4(_value)      (((_value) & 0x3) ? TRUE : FALSE)
#endif /* IS_NOT_ALIGN_4 */


/* This macro evaluate the input length in unit of Double Word(4 Bytes).
 * _value - value in unit of Byte, output will round up to DW boundary.
 */
#ifndef BYTE_TO_DWORD
#define BYTE_TO_DWORD(_value)       ((_value + 3) >> 2)
#endif /* BYTE_TO_DWORD */

/* This macro evaluate the input length in unit of Byte.
 * _value - value in unit of DW, output is in unit of Byte.
 */
#ifndef DWORD_TO_BYTE
#define DWORD_TO_BYTE(_value)       ((_value) << 2)
#endif /* DWORD_TO_BYTE */

#if 1 // Little-Endian
#define CONST_NTOHS(_x)     __constant_ntohs(_x)

#define CONST_HTONS(_x)     __constant_htons(_x)

#if 0
#define NTOHS(_x)           ntohs(_x)

#define HTONS(_x)           htons(_x)

#define NTOHL(_x)           ntohl(_x)

#define HTONL(_x)           htonl(_x)
#endif

#else // Big-Endian

#define CONST_NTOHS(_x)

#define CONST_HTONS(_x)

#define NTOHS(_x)

#define HTONS(_x)

#endif

#ifdef WCN_MAUI
//todo
//#define INT_MIN     (-2147483647-1)
//#define INT_MAX     (2147483647)
#define INT_MIN   (~0x7fffffff)  /* -2147483648 and 0x80000000 are unsigned */
#define INT_MAX   0x7fffffff
#endif

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _GL_TYPEDEF_H */

