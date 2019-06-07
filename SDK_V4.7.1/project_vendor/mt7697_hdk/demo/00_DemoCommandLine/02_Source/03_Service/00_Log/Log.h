#ifndef LOGFILE_SERVICE_H
#define LOGFILE_SERVICE_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <stdio.h>

// Hard code here
#define DCL_LOG_ON

#define ENABLE_DCL_LOG_MSG_DEBUG     1
#define ENABLE_DCL_LOG_MSG_LOW       0
#define ENABLE_DCL_LOG_MSG_MED       0
#define ENABLE_DCL_LOG_MSG_HIGH      0
#define ENABLE_DCL_LOG_MSG_ERROR     1
#define ENABLE_DCL_LOG_MSG_FATAL     0 
#define ENABLE_DCL_LOG_MSG_VERBO     0

#define DCL_LOG_MSG_DEBUG    (0)
#define DCL_LOG_MSG_LOW      (1)
#define DCL_LOG_MSG_MED      (2)
#define DCL_LOG_MSG_HIGH     (3)
#define DCL_LOG_MSG_ERROR    (4)
#define DCL_LOG_MSG_FATAL    (5)

#define DCL_LOG_DEBUG(xx_fmt, ...) \
  DCL_APPS_LOG(DCL_LOG_MSG_DEBUG, xx_fmt, ##__VA_ARGS__)
#define DCL_LOG_LOW(xx_fmt, ...) \
  DCL_APPS_LOG(DCL_LOG_MSG_LOW, xx_fmt, ##__VA_ARGS__)
#define DCL_LOG_MED(xx_fmt, ...) \
  DCL_APPS_LOG(DCL_LOG_MSG_MED, xx_fmt, ##__VA_ARGS__)
#define DCL_LOG_HIGH(xx_fmt, ...) \
  DCL_APPS_LOG(DCL_LOG_MSG_HIGH, xx_fmt, ##__VA_ARGS__)
#define DCL_LOG_ERROR(xx_fmt, ...) \
  DCL_APPS_LOG(DCL_LOG_MSG_ERROR, xx_fmt, ##__VA_ARGS__)
#define DCL_LOG_FATAL(xx_fmt, ...) \
  DCL_APPS_LOG(DCL_LOG_MSG_FATAL, xx_fmt, ##__VA_ARGS__)

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#undef DCL_APPS_LOG
#define DCL_APPS_LOG(xx_prio, xx_fmt, ...) \
  DCL_APPS_RAW(xx_prio, "[%s:%04d] " xx_fmt , __FILENAME__,__LINE__, ##__VA_ARGS__)

#ifdef DCL_LOG_ON
    #ifdef DCL_APPS_RAW
    #undef DCL_APPS_RAW
    #endif
    #define DCL_APPS_RAW(xx_prio, xx_fmt, ...)                                       \
    do {                                                                             \
	    if(xx_prio == DCL_LOG_MSG_DEBUG && ENABLE_DCL_LOG_MSG_DEBUG){                \
            dcl_log("[D]"xx_fmt"\r\n",##__VA_ARGS__);                                \
        } else if(xx_prio == DCL_LOG_MSG_LOW && ENABLE_DCL_LOG_MSG_LOW){         \
            dcl_log("[L]"xx_fmt"\r\n",##__VA_ARGS__);                                \
        } else if(xx_prio == DCL_LOG_MSG_MED && ENABLE_DCL_LOG_MSG_MED){         \
            dcl_log("[M]"xx_fmt"\r\n",##__VA_ARGS__);                                \
        } else if(xx_prio == DCL_LOG_MSG_HIGH && ENABLE_DCL_LOG_MSG_HIGH){         \
            dcl_log("[H]"xx_fmt"\r\n",##__VA_ARGS__);                                \
        } else if(xx_prio == DCL_LOG_MSG_ERROR && ENABLE_DCL_LOG_MSG_ERROR){         \
            dcl_log("[E]"xx_fmt"\r\n",##__VA_ARGS__);                                \
        } else if(xx_prio == DCL_LOG_MSG_FATAL && ENABLE_DCL_LOG_MSG_FATAL){         \
            dcl_log("[F]"xx_fmt"\r\n",##__VA_ARGS__);                                \
        };                                                                           \
	  } while (0)
#else
    #define DCL_APPS_RAW(xx_prio, xx_fmt, ...)
#endif // DCL_LOG_ON

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

#define TEMP_BUFFER_SIZE   1024

#define __do_div(n, base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

//Prototype ( must have )
int dcl_log(const char *fmt, ...);

#endif