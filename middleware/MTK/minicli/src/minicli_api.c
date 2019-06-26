/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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


#include <string.h>

#include "cli.h"


/****************************************************************************
 *
 * Forward Declarations
 *
 ****************************************************************************/


uint8_t _cli_getline(cli_t    *cb,
                     char     *buf,
                     uint16_t max,
                     char     *prompt);


void _cli_do_cmd(cli_t *cb, char *line);


void _cli_do_tok(cli_t *cb, int argc, char *argv[]);




/****************************************************************************
 *
 * Constants.
 *
 ****************************************************************************/


#define MAX_ID                              (16)


#ifndef MTK_MINICLI_EXAMPLE_LOGIN_ID
#define MTK_MINICLI_EXAMPLE_LOGIN_ID        "admin"
#endif


#ifndef MTK_MINICLI_EXAMPLE_LOGIN_PASSWD
#define MTK_MINICLI_EXAMPLE_LOGIN_PASSWD    "password"
#endif


/****************************************************************************
 *
 * Types.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Static variables.
 *
 ****************************************************************************/


/**
 * The only static variable in MiniCLI engine.
 */
static cli_t        *cb;


/****************************************************************************
 *
 * Local functions.
 *
 ****************************************************************************/


/**
 * The login function.
 *
 * The login function returns only if the login has succeed. Otherwise, the
 * user is stucked in login loop.
 */
static void _cli_login(void)
{
    char    id   [MAX_ID + 1];
    char    pass [MAX_ID + 1];

    /* already logged in */
    if (cb->state) return;

    while (1)
    {
        cb->echo = 0;
        if (!_cli_getline(cb, id,   MAX_ID, "login: "   )) continue;

        cb->echo = '*';
        if (!_cli_getline(cb, pass, MAX_ID, "password: ")) continue;

        /* validate */
        if (cb->knock(id, pass)) {
            cb->echo  = 0;
            cb->state = 1;
            break;
        }

        cli_puts("login failed\n");
    }
}


/****************************************************************************
 *
 * Utilities that depends on CLI engine been initialized properly.
 *
 ****************************************************************************/


void cli_putc(char c)
{
    cb->put(c);
}


void cli_puts(char *s)
{
    while (*s) {
        cb->put(*s);
        s++;
    }
}


/****************************************************************************
 *
 * API functions.
 *
 ****************************************************************************/


void cli_init(cli_t *cli)
{
    cb = cli;
}


uint8_t cli_logout(uint8_t len, char *param[])
{
    cb->state = 0;
    cli_puts("logout\n");
    return 0;
}


void cli_line(char *line)
{
    _cli_do_cmd(cb, line);
}


void cli_tokens(uint8_t argc, char *argv[])
{
    _cli_do_tok(cb, argc, argv);
}


/*
 * enable hardcode login credentials by default
 */
uint8_t cli_login_example(char *id, char *pass)
{
    return (!strcmp(id,   MTK_MINICLI_EXAMPLE_LOGIN_ID) &&
            !strcmp(pass, MTK_MINICLI_EXAMPLE_LOGIN_PASSWD));
}


#if !defined(MTK_MINICLI_LINE_EDIT_DISABLE) && !defined(MTK_MINICLI_HISTORY_DISABLE)
void cli_task(void)
{
    int             index;
    cli_history_t   *h = &cb->history;

    _cli_login();

    do {
        index = h->index;

        /* used when history commands are full and circulate */
        memset(h->history[index], 0, h->line_max);

        if (_cli_getline(cb, h->history[index], h->line_max - 1, "$ ")) {
            memcpy(h->parse_token, h->history[index], h->line_max);

            _cli_do_cmd(cb, h->parse_token);
        }
    } while (cb->state);
}
#else
void cli_task(void)
{
    static char     line[128];

    _cli_login();

    do {
        if (_cli_getline(cb, line, 127, "$ ")) {
            _cli_do_cmd(line);
        }
    } while (cb->state);
}
#endif
