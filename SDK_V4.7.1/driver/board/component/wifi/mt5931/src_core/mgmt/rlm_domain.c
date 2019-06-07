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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/rlm_domain.c#6 $
*/

/*! \file   "rlm_domain.c"
    \brief

*/




/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/* The following country or domain shall be set from host driver.
 * And host driver should pass specified DOMAIN_INFO_ENTRY to MT6620 as
 * the channel list of being a STA to do scanning/searching AP or being an
 * AP to choose an adequate channel if auto-channel is set.
 */

/* Define mapping tables between country code and its channel set
 */
static const UINT_16     g_u2CountryGroup0[] = {
    COUNTRY_CODE_US, COUNTRY_CODE_BS, COUNTRY_CODE_BB, COUNTRY_CODE_BO, COUNTRY_CODE_DM,
    COUNTRY_CODE_DO, COUNTRY_CODE_HT, COUNTRY_CODE_PR, COUNTRY_CODE_TH, COUNTRY_CODE_TW,
    COUNTRY_CODE_AI, COUNTRY_CODE_AG, COUNTRY_CODE_AW, COUNTRY_CODE_BM, COUNTRY_CODE_CU,
    COUNTRY_CODE_GD, COUNTRY_CODE_GY, COUNTRY_CODE_JM, COUNTRY_CODE_KN, COUNTRY_CODE_LC,
    COUNTRY_CODE_VC, COUNTRY_CODE_TT, COUNTRY_CODE_SR
};
static const UINT_16     g_u2CountryGroup1[] = {
    COUNTRY_CODE_BR, COUNTRY_CODE_EC, COUNTRY_CODE_HK, COUNTRY_CODE_MX, COUNTRY_CODE_PE,
    COUNTRY_CODE_CR, COUNTRY_CODE_MD, COUNTRY_CODE_NI, COUNTRY_CODE_PZ
};
static const UINT_16     g_u2CountryGroup2[] = {COUNTRY_CODE_CO, COUNTRY_CODE_PY};
static const UINT_16     g_u2CountryGroup3[] = {COUNTRY_CODE_JP};
static const UINT_16     g_u2CountryGroup4[] = {
    COUNTRY_CODE_CN, COUNTRY_CODE_UY, COUNTRY_CODE_MA
};
static const UINT_16     g_u2CountryGroup5[] = {COUNTRY_CODE_AR};
static const UINT_16     g_u2CountryGroup6[] = {COUNTRY_CODE_AU, COUNTRY_CODE_NZ};
static const UINT_16     g_u2CountryGroup7[] = {COUNTRY_CODE_RU};
static const UINT_16     g_u2CountryGroup8[] = {
    COUNTRY_CODE_ID, COUNTRY_CODE_HN, COUNTRY_CODE_PG, COUNTRY_CODE_PK
};
static const UINT_16     g_u2CountryGroup9[] = {COUNTRY_CODE_CA};
static const UINT_16     g_u2CountryGroup10[] = {
    COUNTRY_CODE_CL, COUNTRY_CODE_IN, COUNTRY_CODE_SA, COUNTRY_CODE_SG, COUNTRY_CODE_VE,
    COUNTRY_CODE_MY, COUNTRY_CODE_VN, COUNTRY_CODE_EG
};
static const UINT_16     g_u2CountryGroup11[] = {COUNTRY_CODE_IL, COUNTRY_CODE_UA};
static const UINT_16     g_u2CountryGroup12[] = {COUNTRY_CODE_JO, COUNTRY_CODE_KW};
static const UINT_16     g_u2CountryGroup13[] = {COUNTRY_CODE_KR, COUNTRY_CODE_OM};
static const UINT_16     g_u2CountryGroup14[] = {
    COUNTRY_CODE_EU
    /* When country code is not found, this domain info will be used.
     * So mark all country codes to reduce search time. 20110908
     */
    /*, COUNTRY_CODE_PH, COUNTRY_CODE_TR, COUNTRY_CODE_IR, COUNTRY_CODE_BY,
    COUNTRY_CODE_LK */
};


DOMAIN_INFO_ENTRY arSupportedRegDomains[] = {
    {
        (PUINT_16) g_u2CountryGroup0, sizeof(g_u2CountryGroup0) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  11,  FALSE }, /* CH_SET_2G4_1_11 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup1, sizeof(g_u2CountryGroup1) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup2, sizeof(g_u2CountryGroup2) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_UPPER_NA */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup3, sizeof(g_u2CountryGroup3) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_14 */
            {  82, BAND_2G4, CHNL_SPAN_5,   14,   1,  FALSE },

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }  /* CH_SET_UNII_UPPER_NA */
        }
    },
    {
        (PUINT_16) g_u2CountryGroup4, sizeof(g_u2CountryGroup4) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_LOW_NA */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup5, sizeof(g_u2CountryGroup5) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_LOW_NA */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_WW_NA */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup6, sizeof(g_u2CountryGroup6) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  11,  FALSE }, /* CH_SET_2G4_1_11 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /* CH_SET_UNII_UPPER_149_161 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup7, sizeof(g_u2CountryGroup7) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /* CH_SET_UNII_UPPER_149_161 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup8, sizeof(g_u2CountryGroup8) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_LOW_NA */
            { 118, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_MID_NA */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_WW_NA */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   4,  FALSE }, /* CH_SET_UNII_UPPER_149_161 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup9, sizeof(g_u2CountryGroup9) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  11,  FALSE }, /* CH_SET_2G4_1_11 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,   5,  FALSE }, /* CH_SET_UNII_WW_100_116_132_140 */
            { 121, BAND_5G,  CHNL_SPAN_20, 132,   5,  FALSE },
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
        }
    },
    {
        (PUINT_16) g_u2CountryGroup10, sizeof(g_u2CountryGroup10) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_WW_NA */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup11, sizeof(g_u2CountryGroup11) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_WW_NA */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_UPPER_NA */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup12, sizeof(g_u2CountryGroup12) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_MID_NA */
            { 121, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_WW_NA */
            { 125, BAND_NULL,           0,   0,   0,  FALSE }, /* CH_SET_UNII_UPPER_NA */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        (PUINT_16) g_u2CountryGroup13, sizeof(g_u2CountryGroup13) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,   8,  FALSE }, /* CH_SET_UNII_WW_100_128 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   5,  FALSE }, /* CH_SET_UNII_UPPER_149_165 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    },
    {
        /* Note: The final one is for Europe union now. */
        (PUINT_16) g_u2CountryGroup14, sizeof(g_u2CountryGroup14) / 2,
        {
            {  81, BAND_2G4, CHNL_SPAN_5,    1,  13,  FALSE }, /* CH_SET_2G4_1_13 */

            { 115, BAND_5G,  CHNL_SPAN_20,  36,   4,  FALSE }, /* CH_SET_UNII_LOW_36_48 */
            { 118, BAND_5G,  CHNL_SPAN_20,  52,   4,  FALSE }, /* CH_SET_UNII_MID_52_64 */
            { 121, BAND_5G,  CHNL_SPAN_20, 100,  11,  FALSE }, /* CH_SET_UNII_WW_100_140 */
            { 125, BAND_5G,  CHNL_SPAN_20, 149,   7,  FALSE }, /* CH_SET_UNII_UPPER_149_173 */
            {   0, BAND_NULL,           0,   0,   0,  FALSE }
        }
    }
};

#if 0
COUNTRY_CH_SET_T arCountryChSets[] = {
    /* idx=0: US, Bahamas, Barbados, Bolivia(Voluntary), Dominica (the Commonwealth of Dominica),
       The Dominican Republic, Haiti */
    {
        CH_SET_2G4_1_11,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_165
    },
    /* idx=1: Brazil, Ecuador, Hong Kong, Mexico, Peru */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_165
    },
    /* idx=2: JP1, Colombia(Voluntary), Paraguay */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_NA
    },
    /* idx=3: JP2 */
    {
        CH_SET_2G4_1_14,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_NA
    },
    /* idx=4: CN, Uruguay, Morocco */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_NA,
        CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_165
    },
    /* idx=5: Argentina */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_165
    },
    /* idx=6: Australia, New Zealand */
    {
        CH_SET_2G4_1_11,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_161
    },
    /* idx=7: Russia */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_161
    },
    /* idx=8: Indonesia */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_NA,
        CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_161
    },
    /* idx=9: Canada */
    {
        CH_SET_2G4_1_11,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_116_132_140,    CH_SET_UNII_UPPER_149_165
    },
    /* idx=10: Chile, India, Saudi Arabia, Singapore */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_149_165
    },
    /* idx=11: Israel, Ukraine */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_NA
    },
    /* idx=12: Jordan, Kuwait */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_NA,
        CH_SET_UNII_WW_NA,         CH_SET_UNII_UPPER_NA
    },
    /* idx=13: South Korea */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_128,    CH_SET_UNII_UPPER_149_165
    },
    /* idx=14: Taiwan */
    {
        CH_SET_2G4_1_11,           CH_SET_UNII_LOW_NA,             CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_165
    },
    /* idx=15: EU all countries */
    {
        CH_SET_2G4_1_13,           CH_SET_UNII_LOW_36_48,          CH_SET_UNII_MID_52_64,
        CH_SET_UNII_WW_100_140,    CH_SET_UNII_UPPER_149_173
    }
};
#endif


/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in/out]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
P_DOMAIN_INFO_ENTRY
rlmDomainGetDomainInfo(
    P_ADAPTER_T     prAdapter
)
{
    UINT_16                 i, j;
    P_DOMAIN_INFO_ENTRY     prDomainInfo;
    UINT_16                 u2TargetCountryCode;

    ASSERT(prAdapter);

    if (prAdapter->prDomainInfo) {
        return prAdapter->prDomainInfo;
    }

    prDomainInfo = &prAdapter->prGlueInfo->rRegInfo.rDomainInfo;
    if (prDomainInfo->u4CountryNum > 0) {
        goto L_set_domain_info;
    }

    u2TargetCountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;

    /* If no matched country code, the final one will be used
     * The final one is for Europe union now.
     */
    for (i = 0;
            i < sizeof(arSupportedRegDomains) / sizeof(DOMAIN_INFO_ENTRY); i++) {

        prDomainInfo = &arSupportedRegDomains[i];
        for (j = 0; j < prDomainInfo->u4CountryNum; j++) {
            if (prDomainInfo->pu2CountryGroup[j] == u2TargetCountryCode) {
                break;
            }
        }
        if (j < prDomainInfo->u4CountryNum) {
            break; /* Found */
        }
    }

L_set_domain_info:

    prAdapter->prDomainInfo = prDomainInfo;
    return prDomainInfo;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in/out] The input variable pointed by pucNumOfChannel is the max
*                arrary size. The return value indciates meaning list size.
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
rlmDomainGetChnlList(
    P_ADAPTER_T             prAdapter,
    ENUM_BAND_T             eSpecificBand,
    PUINT_8                 pucNumOfChannel,
    P_RF_CHANNEL_INFO_T     paucChannelList
)
{
    UINT_8                  i, j, ucNum;
    P_DOMAIN_SUBBAND_INFO   prSubband;
    P_DOMAIN_INFO_ENTRY     prDomainInfo;

    ASSERT(prAdapter);
    ASSERT(paucChannelList);
    ASSERT((*pucNumOfChannel) > 0);

    /* If no matched country code, the final one will be used */
    prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
    ASSERT(prDomainInfo);

    ucNum = 0;
    for (i = 0; i < MAX_SUBBAND_NUM; i++) {
        prSubband = &prDomainInfo->rSubBand[i];

        if (prSubband->ucBand == BAND_5G && !prAdapter->fgEnable5GBand) {
            continue;
        }

        if (eSpecificBand == BAND_NULL || prSubband->ucBand == eSpecificBand) {
            for (j = 0; j < prSubband->ucNumChannels; j++) {
                if (ucNum >= (*pucNumOfChannel)) {
                    break;
                }
                paucChannelList[ucNum].eBand = prSubband->ucBand;
                paucChannelList[ucNum].ucChannelNum =
                    prSubband->ucFirstChannelNum + j * prSubband->ucChannelSpan;
                ucNum++;
            }
        }
    }

    *pucNumOfChannel = ucNum;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief
*
* @param[in]
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
rlmDomainSendCmd(
    P_ADAPTER_T     prAdapter,
    BOOLEAN         fgIsOid
)
{
    P_DOMAIN_INFO_ENTRY     prDomainInfo;
    P_CMD_SET_DOMAIN_INFO_T prCmd;
    WLAN_STATUS             rStatus;
    UINT_8                  i;
    P_DOMAIN_SUBBAND_INFO   prSubBand;

    prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
    ASSERT(prDomainInfo);

    prCmd = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_SET_DOMAIN_INFO_T));
    //ASSERT(prCmd);

    /* To do: exception handle */
    if (!prCmd) {
        DBGLOG(MEM, ERROR, ("rlmDomainSendCmd(): cnmMemAlloc() fail"));
        return;
    }
    kalMemZero(prCmd, sizeof(CMD_SET_DOMAIN_INFO_T));

    prCmd->u2CountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;
    prCmd->u2Reserved = 0;
    prCmd->uc2G4Bandwidth =
        prAdapter->rWifiVar.rConnSettings.uc2G4BandwidthMode;
    prCmd->uc5GBandwidth =
        prAdapter->rWifiVar.rConnSettings.uc5GBandwidthMode;

    for (i = 0; i < 6; i++) {
        prSubBand = &prDomainInfo->rSubBand[i];

        prCmd->rSubBand[i].ucRegClass = prSubBand->ucRegClass;
        prCmd->rSubBand[i].ucBand = prSubBand->ucBand;
        prCmd->rSubBand[i].ucChannelSpan = prSubBand->ucChannelSpan;
        prCmd->rSubBand[i].ucFirstChannelNum = prSubBand->ucFirstChannelNum;
        prCmd->rSubBand[i].ucNumChannels = prSubBand->ucNumChannels;
        prCmd->rSubBand[i].aucReserved[0] = 0;
        prCmd->rSubBand[i].aucReserved[1] = 0;
        prCmd->rSubBand[i].aucReserved[2] = 0;
    }

    /* Update domain info to chip */
    rStatus = wlanSendSetQueryCmd(
                  prAdapter,                  /* prAdapter */
                  CMD_ID_SET_DOMAIN_INFO,     /* ucCID */
                  TRUE,                       /* fgSetQuery */
                  FALSE,                      /* fgNeedResp */
                  fgIsOid,                    /* fgIsOid */
                  NULL,                       /* pfCmdDoneHandler*/
                  NULL,                       /* pfCmdTimeoutHandler */
                  sizeof(CMD_SET_DOMAIN_INFO_T),    /* u4SetQueryInfoLen */
                  (PUINT_8) prCmd,            /* pucInfoBuffer */
                  NULL,                       /* pvSetQueryBuffer */
                  0                           /* u4SetQueryBufferLen */
              );

    ASSERT(rStatus == WLAN_STATUS_PENDING);

    cnmMemFree(prAdapter, prCmd);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in/out]
*
* \return TRUE  Legal channel
*         FALSE Illegal channel for current regulatory domain
*/
/*----------------------------------------------------------------------------*/
BOOLEAN
rlmDomainIsLegalChannel(
    P_ADAPTER_T     prAdapter,
    ENUM_BAND_T     eBand,
    UINT_8          ucChannel
)
{
    UINT_8                  i, j;
    P_DOMAIN_SUBBAND_INFO   prSubband;
    P_DOMAIN_INFO_ENTRY     prDomainInfo;

    prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
    ASSERT(prDomainInfo);

    for (i = 0; i < MAX_SUBBAND_NUM; i++) {
        prSubband = &prDomainInfo->rSubBand[i];

        if (prSubband->ucBand == BAND_5G && !prAdapter->fgEnable5GBand) {
            continue;
        }

        if (prSubband->ucBand == eBand) {
            for (j = 0; j < prSubband->ucNumChannels; j++) {
                if ((prSubband->ucFirstChannelNum + j * prSubband->ucChannelSpan)
                        == ucChannel) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}



