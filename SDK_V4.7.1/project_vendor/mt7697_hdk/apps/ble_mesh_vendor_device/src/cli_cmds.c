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
 
#include <stdint.h>


#if defined(MTK_MINICLI_ENABLE)


#include "minicli_cmd_table.h"


static cli_t *_cli_ptr;


#ifdef MTK_CLI_TEST_MODE_ENABLE
static uint8_t _sdk_cli_test_mode(uint8_t len, char *param[]);
#endif


#ifdef MTK_CLI_EXAMPLE_MODE_ENABLE
static uint8_t _sdk_cli_example_mode(uint8_t len, char *param[]);
#endif


#if defined(MTK_CLI_TEST_MODE_ENABLE) || defined(MTK_CLI_EXAMPLE_MODE_ENABLE)
static uint8_t _sdk_cli_normal_mode(uint8_t len, char *param[]);
#endif

#ifdef MTK_CLI_TEST_MODE_ENABLE
#define GOTO_TEST_MODE_CLI_ENTRY    { "en",   "enter test mode",     _sdk_cli_test_mode    },
#endif

#ifdef MTK_CLI_EXAMPLE_MODE_ENABLE
#define GOTO_EXAMPLE_MODE_CLI_ENTRY { "ex",   "enter example mode",  _sdk_cli_example_mode },
#else
#define GOTO_EXAMPLE_MODE_CLI_ENTRY
#endif

#if defined(MTK_CLI_TEST_MODE_ENABLE) || defined(MTK_CLI_EXAMPLE_MODE_ENABLE)
#define GOTO_NORMAL_MODE_CLI_ENTRY  { "back", "back to normal mode", _sdk_cli_normal_mode  },
#endif


#ifndef GOTO_TEST_MODE_CLI_ENTRY
#define GOTO_TEST_MODE_CLI_ENTRY
#endif

#ifndef GOTO_EXAMPLE_MODE_CLI_ENTRY
#define GOTO_EXAMPLE_MODE_CLI_ENTRY
#endif

#ifndef GOTO_NORMAL_MODE_CLI_ENTRY
#define GOTO_NORMAL_MODE_CLI_ENTRY
#endif


/****************************************************************************
 *
 * EXAMPLE MODE
 *
 ****************************************************************************/


#ifdef MTK_CLI_EXAMPLE_MODE_ENABLE

static cmd_t   _cmds_example[] = {
    GOTO_NORMAL_MODE_CLI_ENTRY
    MINICLI_EXAMPLE_MODE_CLI_CMDS
    { NULL, NULL, NULL, NULL }
};

#endif /* MTK_CLI_EXAMPLE_MODE_ENABLE */


/****************************************************************************
 *
 * TEST MODE
 *
 ****************************************************************************/


#ifdef MTK_CLI_TEST_MODE_ENABLE

static cmd_t   _cmds_test[] = {
    GOTO_NORMAL_MODE_CLI_ENTRY
    MINICLI_TEST_MODE_CLI_CMDS
    { NULL, NULL, NULL, NULL }
};

#endif /* MTK_CLI_TEST_MODE_ENABLE */


/****************************************************************************
 *
 * NORMAL MODE
 *
 ****************************************************************************/


static cmd_t   _cmds_normal[] = {
    GOTO_TEST_MODE_CLI_ENTRY
    GOTO_EXAMPLE_MODE_CLI_ENTRY
    MINICLI_NORMAL_MODE_CLI_CMDS
    { NULL, NULL, NULL, NULL }
};


/****************************************************************************
 *
 * TOGGLE commands
 *
 ****************************************************************************/


#ifdef MTK_CLI_TEST_MODE_ENABLE
static uint8_t _sdk_cli_test_mode(uint8_t len, char *param[])
{
    _cli_ptr->cmd = &_cmds_test[0];
    return 0;
}
#endif


#ifdef MTK_CLI_EXAMPLE_MODE_ENABLE
static uint8_t _sdk_cli_example_mode(uint8_t len, char *param[])
{
    _cli_ptr->cmd = &_cmds_example[0];
    return 0;
}
#endif


#if defined(MTK_CLI_TEST_MODE_ENABLE) || defined(MTK_CLI_EXAMPLE_MODE_ENABLE)
static uint8_t _sdk_cli_normal_mode(uint8_t len, char *param[])
{
    _cli_ptr->cmd = &_cmds_normal[0];
    return 0;
}
#endif


/****************************************************************************
 *
 * PUBLIC functions
 *
 ****************************************************************************/


void cli_cmds_init(cli_t *cli)
{
    _cli_ptr = cli;
    _cli_ptr->cmd = &_cmds_normal[0];
}


#endif /* #if defined(MTK_MINICLI_ENABLE) */
