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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "sys_init.h"
#include "task_def.h"

#include "syslog.h"


/****************************************************************************
 *
 * CLI Example Code
 *
 ****************************************************************************/


/* Include this file for CLI engine prototypes definition. */
#include "cli.h"

/* Include this file for I/O interface API default implementation */
#include "io_def.h"

/* Include this file for integer parsing */
#include "toi.h"


/****************************************************************************
 *
 * Command History Buffer
 *
 ****************************************************************************/


#define HISTORY_LINE_MAX        (128)
#define HISTORY_LINES           (20)


static char s_history_lines      [ HISTORY_LINES ][ HISTORY_LINE_MAX ];
static char *s_history_ptrs      [ HISTORY_LINES ];

static char s_history_input      [ HISTORY_LINE_MAX ];
static char s_history_parse_token[ HISTORY_LINE_MAX ];


/****************************************************************************
 *
 * Commands - 2nd level
 *
 ****************************************************************************/


static unsigned int     _g_counter;

static uint8_t do_cnt_show(uint8_t argc, char *argv[])
{
    printf("%u\n", _g_counter);
    return 0;
}


static uint8_t do_cnt_tick(uint8_t argc, char *argv[])
{
    _g_counter++;
    return 0;
}


static uint8_t do_cnt_reset(uint8_t argc, char *argv[])
{
    _g_counter = 0;
    return 0;
}


/****************************************************************************
 *
 * Commands
 *
 ****************************************************************************/


/*
 * Example of user custom CLI command, note that it's parameters and return
 * value matches the declaration of cli_cmd_handler_t, which is the callback
 * handler type used by CLI engine.
 */
static uint8_t do_hello(uint8_t argc, char *argv[])
{
    printf("Hello world\n");
    return 0;
}


static uint8_t do_echo(uint8_t argc, char *argv[])
{
    int i;

    for (i = 0; i < argc; i++)
        printf("%s\n", argv[i]);

    return 0;
}


static uint8_t do_math(uint8_t argc, char *argv[])
{
    int     opval1, opval2;
    uint8_t type;

    if (argc != 3) {
        printf("need something like: 1 + 1 or 1 - 1\n");
        return 1;
    }

    opval1 = toi(argv[0], &type);
    if (type == TOI_ERR) {
        printf("first operand error\n");
        return 2;
    }

    opval2 = toi(argv[2], &type);
    if (type == TOI_ERR) {
        printf("second operand error\n");
        return 2;
    }

    if (!strcmp(argv[1], "+"))
        printf("outcome %d\n", opval1 + opval2);
    else if (!strcmp(argv[1], "-"))
        printf("outcome %d\n", opval1 - opval2);
    else {
        printf("unknown operator\n");
        return 3;
    }

    return 0;
}


static uint8_t do_line(uint8_t argc, char *argv[])
{
    if (argc != 1) {
        printf("expect a single line of command and its parameters\n");
        return 1;
    }

    printf("beware of stack overflow because this can be abused recursively\n");
    cli_line(argv[0]);

    return 0;
}


static uint8_t do_tokens(uint8_t argc, char *argv[])
{
    if (argc < 1) {
        printf("no more token\n");
        return 1;
    }

    printf("beware of stack overflow because this can be abused recursively\n");
    cli_tokens(argc, argv);

    return 0;
}


/****************************************************************************
 *
 * Command Table - 2nd level
 *
 ****************************************************************************/


static cmd_t  _cmds_cnt[] =
{
    { "show",       "show counter",                 do_cnt_show,    NULL },
    { "tick",       "increase counter",             do_cnt_tick,    NULL },
    { "reset",      "reset counter",                do_cnt_reset,   NULL },

    { NULL, NULL, NULL, NULL }
};


/****************************************************************************
 *
 * Command Table
 *
 ****************************************************************************/


/*
 * CLI command list. Format:
 *
 * { <command name>, <help message>, <handler function>, <sub-command (cmd_t *)> }
 *
 * NOTE:
 *  The last one must be {NULL, NULL, NULL, NULL}
 */
static cmd_t  _cmds_normal[] =
{
    { "hello",  "hello world",               do_hello,   NULL          },
    { "echo",   "show the supplied params",  do_echo,    NULL          },
    { "cal",    "do simple math",            do_math,    NULL          },
    { "line",   "pass a line to cli engine", do_line,    NULL          },
    { "tokens", "pass tokens to cli engine", do_tokens,  NULL          },
    { "count",  "counter operations",        NULL,       &_cmds_cnt[0] },
    { "logout", "end this session",          cli_logout, NULL          },

    /*
     * Add your custom commands here.
     */

    { NULL, NULL, NULL, NULL }
};


/****************************************************************************
 *
 * CLI control block
 *
 ****************************************************************************/


static cli_t _cli_cb = {
    .state  = 0, // 1 to logged-in-by-defualt
    .echo   = 0,
    .knock  = cli_login_example,
    .get    = bsp_io_def_uart_getchar,
    .put    = bsp_io_def_uart_putchar,
    .cmd    = &_cmds_normal[0]
};


/****************************************************************************
 *
 * CLI task
 *
 ****************************************************************************/


/*
 * FreeRTOS task of CLI.
 *
 * The cli_task() function contain one login -> multiple commands -> logout
 * cycle.
 */
static void cli_def_task(void *param)
{
    for (;;) {
        printf("MediaTek IoT Solution\n");
        cli_task();
    }
}


/****************************************************************************
 *
 * Entry
 *
 ****************************************************************************/


int main(void)
{
    system_init();

    /**
     *  Setup CLI history
     */
    cli_history_t *hist = &_cli_cb.history;
    int i;
    for (i = 0; i < HISTORY_LINES; i++) {
        s_history_ptrs[i] = s_history_lines[i];
    }

    hist->history           = &s_history_ptrs[0];
    hist->input             = s_history_input;
    hist->parse_token       = s_history_parse_token;
    hist->history_max       = HISTORY_LINES;
    hist->line_max          = HISTORY_LINE_MAX;
    hist->index             = 0;
    hist->position          = 0;
    hist->full              = 0;

    /**
     *  Init CLI control block
     */
    cli_init(&_cli_cb);

    /*
     * Create a user task for demo when and how to use wifi config API to change WiFI settings,
     * Most WiFi APIs must be called in task scheduler, the system will work wrong if called in main(),
     * For which API must be called in task, please refer to wifi_api.h or WiFi API reference.
     * xTaskCreate(user_wifi_app_entry,
     *             UNIFY_USR_DEMO_TASK_NAME,
     *             UNIFY_USR_DEMO_TASK_STACKSIZE / 4,
     *             NULL, UNIFY_USR_DEMO_TASK_PRIO, NULL);
     * user_wifi_app_entry is user's task entry function, which may be defined in another C file to do application job.
     * UNIFY_USR_DEMO_TASK_NAME, UNIFY_USR_DEMO_TASK_STACKSIZE and UNIFY_USR_DEMO_TASK_PRIO should be defined
     * in task_def.h. User needs to refer to example in task_def.h, then makes own task MACROs defined.
     */

    xTaskCreate(cli_def_task,
                APP_TASK_NAME,
                APP_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                NULL,
                APP_TASK_PRIO,
                NULL);

    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

