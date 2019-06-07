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

 /* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdarg.h"
#include "memory_attribute.h"
#include "exception_handler.h"
#include "hal_dwt.h"
#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
#endif

#ifdef MTK_MINI_DUMP_ENABLE
#include "kiminidump.h"
#include "kiostask.h"
#include "kiosmem.h"
#include "kioslow.h"
#include "gkimem.h"
#include "portmacro.h"

/*Record the minidump buffer address*/
uint8_t *minidump_buffer = NULL;

/*Signal Send param*/
MiniDump_SignalSendT MiniDump_SignalSend[MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM];
uint32_t MiniDump_SignalSendIndex = 0;

/*Context Switch param*/
MiniDump_ContextSwitchT MiniDump_ContextSwitch[MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM];
uint32_t MiniDump_ContextSwitchIndex = 0;

/*Task info param*/
MiniDump_TaskInfoHistoryT MiniDump_TaskInfoHistory[KI_NUM_OF_REAL_TASKS];
uint32_t MiniDump_TaskTimeStamp[KI_NUM_OF_REAL_TASKS];
MiniDump_AllocateEmptyT MiniDump_AllocateEmpty[MINIDUMP_BLOCK_MAX_NUM];

/*Modem dump param*/
bool MiniDump_ModemDebugDumpFlag = false;
MiniDump_ModemUserIdT MiniDump_ModemDebugDumpUserId = MINIDUMP_MODEM_MAX_USER_ID;
const uint8_t MiniDump_ModemCheckValue[MINIDUMP_MODEM_CHECKVALUE_SIZE] = {0xFF, 0xEE, 0xDD, 0xCC};
MiniDump_ModemT MiniDump_ModemDump[(MAX_USERID <= MINIDUMP_MODEM_MAX_USER_ID)?MAX_USERID:-1];
uint32_t MiniDump_ModemDumpRemainingSize = MINIDUMP_MODEM_MAX_SIZE;

/*Assert Info Type*/
MiniDump_AssertInfoT MiniDump_AssertInfo;

extern KiTaskSplitDef kiTaskSplitTable[KI_NUM_UNIQUE_TASK_IDS_DEFINED];
extern KiTaskInitDef kiTaskInitTable[KI_NUM_OF_REAL_TASKS];
extern const KiMemoryPoolEntry kiMemoryPoolTable[KI_POOL_NUM_USER];
extern uint32_t magic_check[10];

/*Mini dump macro*/
#define MINIDUMP_GKIDUMP_START_ADDR   (minidump_buffer + COREDUMP_BUFFER_SIZE)
#define MINIDUMP_MODEMDUMP_BASE_ADDR  (MINIDUMP_GKI_BUFFER_SIZE - MINIDUMP_ASSERT_MAX_SIZE - MINIDUMP_BOOT_UP_TIME_SIZE)
#define MINIDUMP_MODEMDUMP_END_ADDR   (minidump_buffer + MINIDUMP_BUFFER_SIZE - MINIDUMP_ASSERT_MAX_SIZE - MINIDUMP_BOOT_UP_TIME_SIZE)
#define MINIDUMP_ASSERT_BASE_ADDR     (MINIDUMP_GKI_BUFFER_SIZE - MINIDUMP_ASSERT_MAX_SIZE)
#define MINIDUMP_ASSERT_START_ADDR    (minidump_buffer + MINIDUMP_BUFFER_SIZE - MINIDUMP_ASSERT_MAX_SIZE)
#define MINIDUMP_BOOTUP_BASE_ADDR     (MINIDUMP_ASSERT_BASE_ADDR - MINIDUMP_BOOT_UP_TIME_SIZE)
#define MINIDUMP_BOOTUP_START_ADDR    (MINIDUMP_MODEMDUMP_END_ADDR)

#endif  /* MTK_MINI_DUMP_ENABLE */

/*mark the system has been entered the exception flow*/
bool IsSystemEnterException = false;


#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#include "string.h"
#include "fota_76x7.h"
#include "fota_config.h" 
#include "hal_flash.h"
#include "serial_nor_flash.h"
#include "hal_gpio.h"

extern void *xTaskGetCurrentTaskHandle( void );
#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
#include "frhsl.h"
#endif

#if defined(MTK_MINI_DUMP_ENABLE)
#include "string.h"
#include "nvdm_modem.h"
#include "nvdm_modem_minidump.h"
extern void *xTaskGetCurrentTaskHandle( void );
#endif /* MTK_MINI_DUMP_ENABLE */

#define MAX_EXCEPTION_CONFIGURATIONS 6

#if defined(MTK_NO_PSRAM_ENABLE)

unsigned int *pxExceptionStack = 0;

#else /* ! MTK_NO_PSRAM_ENABLE */

#define CHECK_EXCEPTION_STACK_USAGE 0
#if (CHECK_EXCEPTION_STACK_USAGE == 1)
#include <string.h>
#endif

#define EXCEPTION_STACK_WORDS 384

static unsigned int xExceptionStack[EXCEPTION_STACK_WORDS] = {0}; /* reserved as exception handler's stack */
unsigned int *pxExceptionStack = &xExceptionStack[EXCEPTION_STACK_WORDS-1];

#endif /* MTK_NO_PSRAM_ENABLE */

typedef struct
{
  int items;
  exception_config_type configs[MAX_EXCEPTION_CONFIGURATIONS];
} exception_config_t;

typedef struct
{
  bool is_valid;
  const char *expr;
  const char *file;
  int line;
} assert_expr_t;

static exception_config_t exception_config = {0};

static assert_expr_t assert_expr = {0};

static int reboot_flag = 0;

extern memory_region_type memory_regions[];


#if (PRODUCT_VERSION == 2625)
static int auto_reboot_flag = 0;

#define EXCEPTION_AUTO_REBOOT_MAGIC  0x4F545541 //AUTO
static bool auto_reboot_check(void);

#ifdef HAL_WDT_MODULE_ENABLED
static void exception_config_wdt()
{
    if(auto_reboot_check() == true)
    {
       hal_wdt_config_t wdt_config;
       wdt_config.mode = HAL_WDT_MODE_RESET;
       wdt_config.seconds = 30;
       hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
       hal_wdt_init(&wdt_config);
       hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
    }
    else
    {
       hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    }
}

static void exception_feed_wdt()
{
    if(auto_reboot_check() == true)
    {
       hal_wdt_feed(HAL_WDT_FEED_MAGIC);
    }
}
#else
#define exception_config_wdt()
#define exception_feed_wdt()
#endif /* HAL_WDT_MODULE_ENABLED */

#else
#define exception_config_wdt()
#define exception_feed_wdt()
#endif /* PRODUCT_VERSION == 2625 */

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
void abort(void)
{
    __asm("cpsid i");
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);
}

void platform_assert(const char *expr, const char *file, int line)
{
    __asm("cpsid i");
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = true;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
}

void exception_get_assert_expr(const char **expr, const char **file, int *line)
{
    if (assert_expr.is_valid) {
        if(assert_expr.expr == NULL)
            assert_expr.expr = "(NULL)";
        *expr = assert_expr.expr;
        *file = assert_expr.file;
        *line = assert_expr.line;
    } else {
        *expr = NULL;
        *file = NULL;
        *line = 0;
    }

}

void exception_dump_config(int flag)
{
    reboot_flag = flag;
}

void exception_reboot_config(bool auto_reboot)
{
#if (PRODUCT_VERSION == 2625)
    if(auto_reboot)
        auto_reboot_flag = EXCEPTION_AUTO_REBOOT_MAGIC;
    else
        auto_reboot_flag = 0;
#endif
}

#if defined (__CC_ARM) || defined (__ICCARM__)

void __aeabi_assert(const char *expr, const char *file, int line)
{
    platform_assert(expr, file, line);
}

#endif /* __CC_ARM */

bool exception_register_callbacks(exception_config_type *cb)
{
    int i;

    if (exception_config.items >= MAX_EXCEPTION_CONFIGURATIONS) {
       return false;
    } else {
       /* check if it is already registered */
       for (i = 0; i < exception_config.items; i++) {
           if ( exception_config.configs[i].init_cb == cb->init_cb
             && exception_config.configs[i].dump_cb == cb->dump_cb) {
                return false;
           }
       }
       exception_config.configs[exception_config.items].init_cb = cb->init_cb;
       exception_config.configs[exception_config.items].dump_cb = cb->dump_cb;
       exception_config.items++;
       return true;
    }
}

ATTR_TEXT_IN_RAM void exception_init(void)
{
    int i;

    SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;
    
    exception_config_wdt();

#if (configUSE_FLASH_SUSPEND == 1)
    Flash_ReturnReady();
#endif

#if (CHECK_EXCEPTION_STACK_USAGE == 1)
    memset(xExceptionStack, (int)0xa5, (EXCEPTION_STACK_WORDS - 16)*4);
#endif

    for (i = 0; i < exception_config.items; i++) {
        if (exception_config.configs[i].init_cb) {
            exception_config.configs[i].init_cb();
            exception_feed_wdt();
        }
    }

    if (assert_expr.is_valid) {
        platform_printf("assert failed: %s, file: %s, line: %d\n\r",
                        assert_expr.expr,
                        assert_expr.file,
                        assert_expr.line);
    }
}

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

void printUsageErrorMsg(uint32_t CFSRValue)
{
    platform_printf("Usage fault: ");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "Usage fault: ");
#endif
    CFSRValue >>= 16; /* right shift to lsb */
    if ((CFSRValue & (1 << 9)) != 0) {
        platform_printf("Divide by zero\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Divide by zero\n\r");
#endif
    }
    if ((CFSRValue & (1 << 8)) != 0) {
        platform_printf("Unaligned access\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Unaligned access\n\r");
#endif
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        platform_printf("Coprocessor error\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Coprocessor error\n\r");
#endif
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        platform_printf("Invalid EXC_RETURN\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Invalid EXC_RETURN\n\r");
#endif
    }
    if ((CFSRValue & (1 << 1)) != 0) {
        platform_printf("Invalid state\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Invalid state\n\r");
#endif
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        platform_printf("Undefined instruction\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Undefined instruction\n\r");
#endif
    }
}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    platform_printf("Memory Management fault: ");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "Memory Management fault: ");
#endif
    CFSRValue &= 0x000000FF; /* mask mem faults */
    if ((CFSRValue & (1 << 5)) != 0) {
        platform_printf("A MemManage fault occurred during FP lazy state preservation\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "A MemManage fault occurred during FP lazy state preservation\n\r");
#endif
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        platform_printf("A derived MemManage fault occurred on exception entry\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "A derived MemManage fault occurred on exception entry\n\r");
#endif
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        platform_printf("A derived MemManage fault occurred on exception return\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "A derived MemManage fault occurred on exception return\n\r");
#endif
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        platform_printf("Data access violation @0x%08x\n\r", (unsigned int)SCB->MMFAR);
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        snprintf(trace_buffer, 80, "Data access violation @0x%08x\n\r", (unsigned int)SCB->MMFAR);
        FrHslString (0x000A, trace_buffer);
#endif
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        platform_printf("MPU or Execute Never (XN) default memory map access violation\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "MPU or Execute Never (XN) default memory map access violation\n\r");
#endif
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        platform_printf("SCB->MMFAR = 0x%08x\n\r", (unsigned int)SCB->MMFAR );
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        snprintf(trace_buffer, 80, "SCB->MMFAR = 0x%08x\n\r", (unsigned int)SCB->MMFAR );
        FrHslString (0x000A, trace_buffer);
#endif
    }
}

void printBusFaultErrorMsg(uint32_t CFSRValue)
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    platform_printf("Bus fault: ");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "Bus fault: ");
#endif
    CFSRValue &= 0x0000FF00; /* mask bus faults */
    CFSRValue >>= 8;
    if ((CFSRValue & (1 << 5)) != 0) {
        platform_printf("A bus fault occurred during FP lazy state preservation\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "A bus fault occurred during FP lazy state preservation\n\r");
#endif
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        platform_printf("A derived bus fault has occurred on exception entry\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A,"A derived bus fault has occurred on exception entry\n\r");
#endif
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        platform_printf("A derived bus fault has occurred on exception return\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "A derived bus fault has occurred on exception return\n\r");
#endif
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        platform_printf("Imprecise data access error has occurred\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Imprecise data access error has occurred\n\r");
#endif
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        platform_printf("A precise data access error has occurred @x%08x\n\r", (unsigned int)SCB->BFAR);
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        snprintf(trace_buffer, 80, "A precise data access error has occurred @x%08x\n\r", (unsigned int)SCB->BFAR);
        FrHslString (0x000A, trace_buffer);
#endif
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        platform_printf("A bus fault on an instruction prefetch has occurred\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A,"A bus fault on an instruction prefetch has occurred\n\r");
#endif
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        platform_printf("SCB->BFAR = 0x%08x\n\r", (unsigned int)SCB->BFAR );
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        snprintf(trace_buffer, 80, "SCB->BFAR = 0x%08x\n\r", (unsigned int)SCB->BFAR );
        FrHslString (0x000A, trace_buffer);
#endif
    }
}

enum { r0, r1, r2, r3, r12, lr, pc, psr,
       s0, s1, s2, s3, s4, s5, s6, s7,
       s8, s9, s10, s11, s12, s13, s14, s15,
       fpscr
     };

typedef struct TaskContextType {
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int sp;              /* after pop r0-r3, lr, pc, xpsr                   */
    unsigned int lr;              /* lr before exception                             */
    unsigned int pc;              /* pc before exception                             */
    unsigned int psr;             /* xpsr before exeption                            */
    unsigned int control;         /* nPRIV bit & FPCA bit meaningful, SPSEL bit = 0  */
    unsigned int exc_return;      /* current lr                                      */
    unsigned int msp;             /* msp                                             */
    unsigned int psp;             /* psp                                             */
    unsigned int fpscr;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int s12;
    unsigned int s13;
    unsigned int s14;
    unsigned int s15;
    unsigned int s16;
    unsigned int s17;
    unsigned int s18;
    unsigned int s19;
    unsigned int s20;
    unsigned int s21;
    unsigned int s22;
    unsigned int s23;
    unsigned int s24;
    unsigned int s25;
    unsigned int s26;
    unsigned int s27;
    unsigned int s28;
    unsigned int s29;
    unsigned int s30;
    unsigned int s31;
} TaskContext;

static TaskContext taskContext = {0};
TaskContext *pTaskContext = &taskContext;

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

#define MAX_MEMORY_REGIONS 8 /* sync with memory_regions.c */

static memory_region_type load_regions[MAX_MEMORY_REGIONS];

static unsigned int sort_memory_regions(memory_region_type *load_regions)
{
    memory_region_type swap;
    int i, j, entries;
    bool swapped = false;
    for (i=0, entries=0; i < MAX_MEMORY_REGIONS; i++) {

        if (!memory_regions[i].region_name) {
            break;
        }

        if (!memory_regions[i].is_dumped) {
            continue;
        }

        load_regions[entries++] = memory_regions[i];
    }

    for (i=0; i < (entries-1); i++) {
        for (j = 0; j < (entries-1)-i; j++) {
            if (load_regions[j].start_address > load_regions[j+1].start_address) {
                swap = load_regions[j];
                load_regions[j] = load_regions[j+1];
                load_regions[j+1] = swap;
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }

    return entries;
}

static bool crash_dump_status=true;

void crash_dump_save_to_flash_enable (bool enable) 
{
  crash_dump_status=enable;
}

bool is_crash_dump_save_to_flash(void)
{
  return  crash_dump_status;
}


bool do_write_to_ext_flash(void)
{
   
	 if ( fota_is_saved_on_ext_flash() && (fota_is_triggered() != true)){
           	return true;
	 } else {
           return false;
	 }
	 	
}

static uint32_t gpio7_mode;
static uint32_t gpio24_mode;
static uint32_t gpio25_mode;
static uint32_t gpio26_mode;
void crash_ext_flash_init()
{
    printf("serial_nor_init\r\n");
    gpio7_mode =  (*(volatile uint32_t*)0x81023020 >> 28) & 0x0f;
    gpio24_mode = (*(volatile uint32_t*)0x81023020 >> 0) & 0x0f;
    gpio25_mode = (*(volatile uint32_t*)0x81023020 >> 4) & 0x0f;
    gpio26_mode = (*(volatile uint32_t*)0x81023020 >> 8) & 0x0f;

    hal_gpio_init(HAL_GPIO_7);
    hal_gpio_init(HAL_GPIO_24);
    hal_gpio_init(HAL_GPIO_25);
    hal_gpio_init(HAL_GPIO_26);

    /* Call hal_pinmux_set_function() to set GPIO pinmux*/
    hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_SPI_MOSI_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_SPI_MISO_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_SPI_SCK_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_GPIO7);

    serial_nor_init(HAL_SPI_MASTER_0, 1000000);
}


void crash_ext_flash_deinit()
{   
    printf("serial_nor_deinit\r\n");
    serial_nor_deinit();

    /* restore pinmux setting */
    hal_pinmux_set_function(HAL_GPIO_24, gpio24_mode);
    hal_pinmux_set_function(HAL_GPIO_25, gpio25_mode);
    hal_pinmux_set_function(HAL_GPIO_26, gpio26_mode);
    hal_pinmux_set_function(HAL_GPIO_7, gpio7_mode);
}



hal_flash_status_t hal_flash_erase_wrap(uint32_t start_address, hal_flash_block_t block)
{

  hal_flash_status_t status;
	if (do_write_to_ext_flash()==true) {

	status = serial_nor_erase(start_address, block);
	}
	else {
	status = hal_flash_erase(start_address, block);

	}

	return status;
}

hal_flash_status_t hal_flash_write_wrap(uint32_t address, const uint8_t *data, uint32_t length)
{
	hal_flash_status_t status;
	  if (do_write_to_ext_flash()==true) {
	
	  status = serial_nor_write(address, data, length);
	  printf("\r\n serial_nor_write address %x, length is %x",address,length);
	  }
	  else {
	  status = hal_flash_write(address, data, length);
	
	  }
	
	  return status;	  
}

hal_flash_status_t hal_flash_read_wrap(uint32_t start_address, uint8_t *buffer, uint32_t length)

{
	hal_flash_status_t status;
	  if (do_write_to_ext_flash()==true) {
	
	  status = serial_nor_read(start_address, buffer, length);
	 // printf("\r\n serial_nor_read address %x, length is %x",start_address,length);
	  }
	  else {
	  status = hal_flash_read(start_address, buffer, length);
	
	  }
	
	  return status;	  
}

static uint32_t core_get_crash_size()
{

static uint32_t crash_context_length = 0;
 if (do_write_to_ext_flash()==true) {

 crash_context_length =  CRASH_CONTEXT_RESERVED_SIZE;
 	} else
 		{
	crash_context_length = CRASH_CONTEXT_EXT_RESERVED_SIZE;

	}

printf("\n\r core_get_crash_size  crash_context_length = 0x%08x\n\r", crash_context_length);

return  crash_context_length;
}

static uint32_t core_get_crash_address()
{

static uint32_t crash_context_base = 0;
if (do_write_to_ext_flash()==true) {

    crash_context_base = CRASH_CONTEXT_EXT_FLASH_BASE;
} else {

	crash_context_base = CRASH_CONTEXT_FLASH_BASE;
}



printf("\n\r core_get_crash_address  crash_context_base = 0x%08x\n\r", crash_context_base);

return  crash_context_base;

}

static unsigned int coredump_elf_hdr(unsigned int offset, int e_phnum)
{
    uint8_t buf[52] = {0};
    hal_flash_status_t status;
	uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();

    buf[0] = 0x7f;                       /* e_ident[EI_MAG0] = 0x7f */
    buf[1] = 0x45;                       /* e_ident[EI_MAG1] = 'E'  */
    buf[2] = 0x4c;                       /* e_ident[EI_MAG2] = 'L'  */
    buf[3] = 0x46;                       /* e_ident[EI_MAG3] = 'F'  */
    buf[4] = 0x1;                        /* 32-bit ELF              */
    buf[5] = 0x1;                        /* little endian           */
    buf[6] = 0x1;                        /* e_ident[EI_VERSION]     */
    *((uint16_t*)(buf+16)) = 0x0004;     /* e_type = core file      */
    *((uint16_t*)(buf+18)) = 0x0028;     /* e_machine = ARM         */
    *((uint32_t*)(buf+20)) = 0x00000001; /* e_version = 1           */
    *((uint32_t*)(buf+28)) = 0x00000034; /* e_phoff                 */
    *((uint16_t*)(buf+40)) = 0x0034;     /* e_ehsize                */
    *((uint16_t*)(buf+42)) = 0x0020;     /* e_phentsize             */
    *((uint16_t*)(buf+44)) = e_phnum;    /* e_phnum                 */

    status = hal_flash_write_wrap(crash_context_base + offset, buf, 52);
    (void)status;
    /* printf("write coredump header: offset=%d, length=52, status=%d\n\r", offset, status); */

    return 52;
}

static unsigned int coredump_pt_note(unsigned int offset, int e_phnum)
{
    uint8_t buf[32] = {0};
    hal_flash_status_t status;
    uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();
	
    *((uint32_t*)(buf+0))  = 0x00000004; /* p_type = PT_NOTE  */
    *((uint32_t*)(buf+4))  = 52 + e_phnum*32;  /* p_offset */

    /* NT_PRPSINFO: (20 + 124)
     * NT_PRSTATUS: (20 + 148)
     */
    *((uint32_t*)(buf+16)) = 0x00000138;       /* p_filesz                */

    status = hal_flash_write_wrap(crash_context_base + offset, buf, 32);
    (void)status;
    /* printf("write coredump pt_note: offset=%d, length=32, status=%d\n\r", offset, status); */

    return 32;
}

static unsigned int coredump_pt_load(unsigned int offset, memory_region_type *load_regions, int entries)
{
    uint8_t buf[MAX_MEMORY_REGIONS * 32] = {0};
    unsigned int p_offset, p_addr, p_size, i;
    uint8_t *p;
    hal_flash_status_t status;
    uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();
	
    p_offset =  52                     /* ELF Header */
                + (1 + entries) * 32   /* Program header table */
                + (20 + 124)           /* NT_PRPSINFO */
                + (20 + 148);          /* NT_PRSTATUS */

    p = buf;

    for(i = 0; i < entries; i++) {
        *((uint32_t*)(p+0)) = 0x00000001; /* p_type = PT_LOAD */
        *((uint32_t*)(p+4)) = p_offset;   /* p_offset */

        p_addr = (uint32_t)load_regions[i].start_address;
        *((uint32_t*)(p+8))  = p_addr; /* p_vaddr */
        *((uint32_t*)(p+12)) = p_addr; /* p_paddr */

        p_size = (uint32_t)load_regions[i].end_address - (uint32_t)load_regions[i].start_address;
        *((uint32_t*)(p+16)) = p_size; /* p_filesz */
        *((uint32_t*)(p+20)) = p_size; /* p_memsz  */

        *((uint32_t*)(p+24)) = 0x00000007; /* p_flags = RWX  */

        p_offset += p_size;
        p = p + 32;
    }

    status = hal_flash_write_wrap(crash_context_base + offset, buf, entries * 32);
    (void)status;
    /* printf("write coredump pt_load: offset=%d, length=%d, status=%d\n\r", offset, entries*32, status); */

    return (entries * 32);
}

__weak void *xTaskGetCurrentTaskHandle( void )
{
    return 0;
}

static unsigned int coredump_note(unsigned int offset, TaskContext *pTaskContext)
{
    uint8_t buf[320] = {0}; /* (20 + 124) + (20 + 148) + 8 (reserved) */
    uint8_t *p;
    hal_flash_status_t status;
     uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();
	
    /* Fill NT_PRPSINFO */
    p = buf;
    *((uint32_t*)(p+0)) = 0x00000005;  /* namesz                 */
    *((uint32_t*)(p+4)) = 0x0000007c;  /* descsz                 */
    *((uint32_t*)(p+8)) = 0x00000003;  /* type = 3 (NT_PRPSINFO) */
    p[12]  = 'C';                      /* name                   */
    p[13]  = 'O';
    p[14]  = 'R';
    p[15]  = 'E';
    *((uint32_t*)(p+16)) = 0x00000000; /* 0 terminator + padding */

    p = p + 20;

    /* descriptor (struct elf_prpsifo) */
    p[1]  = 'R';                       /* pr_sname = 'R'         */
    *((uint16_t*)(p+8))  = 0x0001;     /* pr_uid                 */
    *((uint16_t*)(p+10)) = 0x0001;     /* pr_uid                 */
    p[28]  = 'f';                      /* pr_fname               */
    p[29]  = 'r';
    p[30]  = 'e';
    p[31]  = 'e';
    p[32]  = 'r';
    p[33]  = 't';
    p[34]  = 'o';
    p[35]  = 's';
    p[36]  = '8';
    p[37]  =  0 ;
    p[44]  = 'f';                      /* pr_psargs              */
    p[45]  = 'r';
    p[46]  = 'e';
    p[47]  = 'e';
    p[48]  = 'r';
    p[49]  = 't';
    p[50]  = 'o';
    p[51]  = 's';
    p[52]  = '8';
    p[53]  =  0 ;

    p = p + 124;                  /* sizeof(struct elf_prpsinfo) */

    /* Fill NT_PRSTATUS */
    *((uint32_t*)(p+0)) = 0x00000005;  /* namesz                 */
    *((uint32_t*)(p+4)) = 0x00000094;  /* descsz                 */
    *((uint32_t*)(p+8)) = 0x00000001;  /* type = 1 (NT_PRSTATUS) */
    p[12]  = 'C';                      /* name                   */
    p[13]  = 'P';
    p[14]  = 'U';
    p[15]  = '0';

    p = p + 20;

    /* descriptor (struct elf_prstatus) */
    *((uint32_t*)(p+24)) = (uint32_t)xTaskGetCurrentTaskHandle(); /* pr_pid */

    p = p + 72; /* 72 = offset of 'pr_reg' in 'struct elf_prstatus' */

    /* elf_prstatus.pr_reg */
    *((unsigned int*)(p+0))  = pTaskContext->r0;
    *((unsigned int*)(p+4))  = pTaskContext->r1;
    *((unsigned int*)(p+8))  = pTaskContext->r2;
    *((unsigned int*)(p+12)) = pTaskContext->r3;
    *((unsigned int*)(p+16)) = pTaskContext->r4;
    *((unsigned int*)(p+20)) = pTaskContext->r5;
    *((unsigned int*)(p+24)) = pTaskContext->r6;
    *((unsigned int*)(p+28)) = pTaskContext->r7;
    *((unsigned int*)(p+32)) = pTaskContext->r8;
    *((unsigned int*)(p+36)) = pTaskContext->r9;
    *((unsigned int*)(p+40)) = pTaskContext->r10;
    *((unsigned int*)(p+44)) = pTaskContext->r11;
    *((unsigned int*)(p+48)) = pTaskContext->r12;
    *((unsigned int*)(p+52)) = pTaskContext->sp;
    *((unsigned int*)(p+56)) = pTaskContext->lr;
    *((unsigned int*)(p+60)) = pTaskContext->pc;
    *((unsigned int*)(p+64)) = pTaskContext->psr;
    *((unsigned int*)(p+68)) = 0; /* ORIG_r0 */

    status = hal_flash_write_wrap(crash_context_base + offset, buf, 312);
    (void)status;
    /* printf("write coredump note: offset=%d, length=%d, status=%d\n\r", offset, 312, status); */

    return 312;
}

static void coredump_load(uint32_t offset, uint8_t *address, uint32_t len)
{
    hal_flash_status_t status;
	uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();

    /* Write 4KB at one time. Data may change while dumping self. Reduce the data loss. */
    while (len) {

        if (len < 0x1000) {
            /* write the last 4KB byte data */
            status = hal_flash_write_wrap(crash_context_base + offset, address, len);
            (void)status;
            /* printf("write coredump_load: address=0x%08x, status=%d\n\r", (unsigned int)address, status); */
            break;
        }

        status = hal_flash_write_wrap(crash_context_base + offset, address, 0x1000);
        (void)status;
        /* printf("write coredump_load: address=0x%08x, status=%d\n\r", (unsigned int)address, status); */
        offset  += 0x1000;
        address += 0x1000;
        len     -= 0x1000;
    }
}


static void coredump(TaskContext *pTaskContext, memory_region_type *memory_regions)
{
    unsigned int entries, offset, i; 
    uint32_t len;
    hal_flash_status_t status;
    uint32_t crash_context_base = 0;
	uint32_t crash_context_length = 0;

 crash_context_length = core_get_crash_size();
  crash_context_base =  core_get_crash_address();
    /* erase the flash blocks reserved for coredump */
    for (offset = 0; offset < crash_context_length; offset += 0x1000) {
        status  = hal_flash_erase_wrap(crash_context_base + offset, HAL_FLASH_BLOCK_4K);
        (void)status;
        /* printf("coredump erase flash: offset=0x%08x, status=%d\n\r", offset, status); */
    }

    entries = sort_memory_regions(load_regions);

    offset = 0;
    offset += coredump_elf_hdr(offset, entries+1); /* 1 note + 'entries' load */
    offset += coredump_pt_note(offset, entries+1);
    offset += coredump_pt_load(offset, load_regions, entries);
    offset += coredump_note(offset, pTaskContext);

    /* memory data starts from (coredump_header + offset) */
    for (i = 0; i < entries; i++) {
        len = (uint32_t)load_regions[i].end_address - (uint32_t)load_regions[i].start_address;
        /* printf("coredump write load region %d\n\r", i); */
        coredump_load(offset, (uint8_t*)load_regions[i].start_address, len);
        offset += len;
    }
}

#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

#if defined(MTK_MINI_DUMP_ENABLE) || defined(HAL_DWT_MODULE_ENABLED) && defined (KI_RLG_ENABLE_EVENT_OVER_HSL)

__weak void *xTaskGetCurrentTaskHandle( void )
{
    return 0;
}

__weak char *pcTaskGetTaskName(void *taskHandle)
{
    return NULL;
}
#endif /* MTK_MINI_DUMP_ENABLE || HAL_DWT_MODULE_ENABLED && KI_RLG_ENABLE_EVENT_OVER_HSL) */

#if defined(MTK_MINI_DUMP_ENABLE)
#define COREDUMP_REG_NUMBER   8     /* The maximum number of regsiter neighbor area to be dumped */
#define COREDUMP_REG_RANGE    256   /* Register's neighbor area : -REG_RANGE/8 .. +REG_RANGE*7/8 */
#define COREDUMP_BUFFER_SIZE  4096  /* The size of CM4 part within one minidump                  */
#define MINIDUMP_GKI_BUFFER_SIZE (4096 - 8) /*Gki dump buffer size*/
#define MINIDUMP_BUFFER_SIZE (COREDUMP_BUFFER_SIZE + MINIDUMP_GKI_BUFFER_SIZE) /*mini dump buffer size*/
#define COREDUMP_STACK_SIZE  (COREDUMP_BUFFER_SIZE - (COREDUMP_REG_RANGE+32)*COREDUMP_REG_NUMBER - 496)
#define MAX_MEMORY_REGIONS (COREDUMP_REG_NUMBER + 2) /* 2 for sp, scs */

static memory_region_type load_regions[MAX_MEMORY_REGIONS];

/*****************************************/
/*FreeRTOS API used in mini dump*/
/*****************************************/
UBaseType_t uxQueueGetTasksWaitingToReceive(QueueHandle_t xQueue);
UBaseType_t uxTaskGetEventListItemContainer(TaskHandle_t xTaskHandle);

/*
 Function: GKI Mini dump parameter initialization
 parameter: none
 return value: none
*/
void MiniDump_Initialise(void)
{
    memset(MiniDump_SignalSend,     0, sizeof(MiniDump_SignalSend));
    memset(MiniDump_ContextSwitch,  0, sizeof(MiniDump_ContextSwitch));
    memset(MiniDump_TaskInfoHistory,0, sizeof(MiniDump_TaskInfoHistory));
    memset(MiniDump_AllocateEmpty,  0, sizeof(MiniDump_AllocateEmpty));
    memset(MiniDump_TaskTimeStamp,  0, sizeof(MiniDump_TaskTimeStamp));
    memset(MiniDump_ModemDump,      0, sizeof(MiniDump_ModemDump));

    IsSystemEnterException = false;
}

/*
 Function: Convert gki task Id to real task Id
 parameter: 
 TaskID: task ID of current task
 return value:  the real task id
*/
static uint8_t MiniDump_GetRealTaskID(uint32_t TaskID)
{
    return ((uint8_t)(M_KiTaskIdToInitIndex(TaskID)));
}

/*
 Function: Current GPT count value
 parameter: none
 return value: current tick count
*/
ATTR_MD_TEXT_IN_TCM static uint32_t MiniDump_GetTickCount(void)
{
    uint32_t TickCount = 0;
    /*get the gpt 1M timer current tick value*/
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &TickCount);

    return TickCount;
}

/*Function: Check the system' state is in exception flow or in task and interrupt
pararmeter: none
return value: If the system is in exception flow, this function will return true, otherwise
return false.
*/
extern uint32_t get_current_irq();
static bool MiniDump_CheckSystemEnterException(void)
{
    hal_nvic_irq_t Excetpion_Number = 0;
    /*Get the exception id to determine whether the system is system exception*/
    Excetpion_Number = (hal_nvic_irq_t)get_current_irq();

    if((Excetpion_Number >= Reset_IRQn) && (Excetpion_Number <= SysTick_IRQn))
    {
        return true;
    }
    return false;
}

/*Function: Record the mini dump function assert error info. If the system state
is not in exception, the system will triger an assert error, otherwise just record the 
assert info and do not triger the assert fail exception.
paramter: 
AssertType : the system assert type
Arg1: mini dump assert fail argument
Arg2: system arg1
Arg3: system arg2
Arg4: system arg3
*/
static void MiniDump_RecordAssertInfo(MiniDump_AssertT AssertType, uint32_t Arg1, uint32_t Arg2, uint32_t Arg3, uint32_t Arg4)
{
    /*Current system is in exception, we just record the assert info*/
    if(MiniDump_CheckSystemEnterException())
    {
        MiniDump_AssertInfo.Type    = AssertType;
        MiniDump_AssertInfo.DumpArg = Arg1;

        MiniDump_AssertInfo.SysArg1 = Arg2;
        MiniDump_AssertInfo.SysArg2 = Arg3;
        MiniDump_AssertInfo.SysArg3 = Arg4;
    }
    else
    {
        /*Current system is not in exception, triger an assert fail exception*/
        SysParam(AssertType, Arg1, Arg2, Arg3, Arg4);
    }
}

/*
 Function: check whether the history infomation sanity in different structures. From this 
 function, we obtain the history newest index and the total history number.
 parameter:
 CheckType: check history structure type
 Startindex: start dump index
 Num: History number
 TaskIndex: current task index (the real task id)
 return value:  
    HISTORY_INFO_ERROR: parameter error
    HISTORY_INFO_WRAP: history info is warpped
    HISTORY_INFO_NOWRAP: history info is not warpped
*/
static uint8_t MiniDump_CheckHistoryInfo(uint8_t CheckType, uint32_t *Startindex, uint32_t *Num, uint8_t TaskIndex)
{
    uint8_t Result, index = 0;

    switch(CheckType)
    {
    case MINIDUMP_SIGNAL_SEND_CHECK:
        {
           if(MiniDump_SignalSendIndex >= MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM)
           {
               Result = HISTORY_INFO_ERROR;
               *Num = 0;
               *Startindex = 0;

               return Result;
           }
                
           if((MiniDump_SignalSend[MiniDump_SignalSendIndex].TimeStamp == 0) 
               && (MiniDump_SignalSend[MiniDump_SignalSendIndex].SignalId == 0) 
               && (MiniDump_SignalSend[MiniDump_SignalSendIndex].DestTaskId == 0) 
               && (MiniDump_SignalSend[MiniDump_SignalSendIndex].ThreadId == 0)
              )
           {
               Result = HISTORY_INFO_NOWRAP;
               if(MiniDump_SignalSendIndex == 0)
               {
                    *Num = *Startindex = 0;
               }
               else
               {
                    *Num = MiniDump_SignalSendIndex;
                    *Startindex = MiniDump_SignalSendIndex - 1;
               }
           }
           else
           {
              Result = HISTORY_INFO_WRAP;
              *Num = MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM;
              if(MiniDump_SignalSendIndex == 0)
              {
                    *Startindex = MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM - 1;
              }
              else
              {
                    *Startindex = MiniDump_SignalSendIndex - 1;
              }
              
           }
        }
        break;
    case MINIDUMP_TASKSWITCH_CHECK:
        {
            if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
            {
                Result = HISTORY_INFO_ERROR;
                *Num = 0;
                *Startindex = 0;
                
                return Result;
            }
                
            if((MiniDump_ContextSwitch[MiniDump_ContextSwitchIndex].TimeStamp == 0) 
                && (MiniDump_ContextSwitch[MiniDump_ContextSwitchIndex].ContextArg == 0) 
                && (MiniDump_ContextSwitch[MiniDump_ContextSwitchIndex].Type == 0)
              )
            {
               Result = HISTORY_INFO_NOWRAP;
               if(MiniDump_ContextSwitchIndex == 0)
               {
                    *Num = *Startindex = 0;
               }
               else
               {
                    *Num = MiniDump_ContextSwitchIndex;
                    *Startindex = MiniDump_ContextSwitchIndex - 1;
               }
               
            }
            else
            {
               Result = HISTORY_INFO_WRAP;
               *Num = MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM;
               if(MiniDump_ContextSwitchIndex == 0)
               {
                    *Startindex = MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM - 1;
               }
               else
               {
                    *Startindex = MiniDump_ContextSwitchIndex - 1;
               }
            }
        }
        break;
    case MINIDUMP_TASKINFO_CHECK:
        {

            if(((MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex) >= MINIDUMP_TASK_HISTORY_INFO_NUM)
                || (TaskIndex >= KI_NUM_OF_REAL_TASKS)
               )
            {
                Result = HISTORY_INFO_ERROR;
                *Num = 0;
                *Startindex = 0;
                
                return Result;
            }
                
            index = MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex;
            
            if((MiniDump_TaskInfoHistory[TaskIndex].MiniDump_TaskHistory[index].TimeStamp == 0) 
                && (MiniDump_TaskInfoHistory[TaskIndex].MiniDump_TaskHistory[index].InfoTag == 0) 
                && (MiniDump_TaskInfoHistory[TaskIndex].MiniDump_TaskHistory[index].SignalId == 0) 
                && (MiniDump_TaskInfoHistory[TaskIndex].MiniDump_TaskHistory[index].TaskId == 0)
               )
            {
                Result = HISTORY_INFO_NOWRAP;
                if(MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex == 0)
                {
                    *Num = *Startindex = 0;
                }
                else
                {
                    *Num = MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex;
                    *Startindex = MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex - 1;
                }
            }
            else
            {
                Result = HISTORY_INFO_WRAP;
                *Num = MINIDUMP_TASK_HISTORY_INFO_NUM;
                if(MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex == 0)
                {
                    *Startindex = MINIDUMP_TASK_HISTORY_INFO_NUM - 1;
                }
                else
                {
                    *Startindex = MiniDump_TaskInfoHistory[TaskIndex].MiniDump_HistoryIndex - 1;
                }
            }
         }
         break;
     }

    return Result;
}

/*
 Function: Distiguish the gki task id and interrupt id. 
 Convert the gki task id and interrupt id to corresponding real thread id
 (interrupt id should plus the interrupt id offset)
 parameter:
    Flag: section id
    Index: history info index
    HistroyIndex: task info's history info index
return value: real thread id
*/
static uint8_t MiniDump_HistoryInfoGetRealThreadID(uint8_t Flag, uint8_t Index, uint8_t HistroyIndex)
{
    uint8_t Id = 0;
    
    switch(Flag)
    {
    case SIG_SECTION_ID:
        {
            /*Array bound check*/
            if(Index >= MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM)
            {
                MiniDump_RecordAssertInfo(SYSTEM_DEV_PARAM_SIG_SEND_INVALID_INDEX, 0, Index, 0, 0);
                return 0xFF; 
            }
                
            /*This is interrupt Id, id 0~TIMER_TASK_ID is reserved for interrupt ID*/
            if(MiniDump_SignalSend[Index].ThreadId < TIMER_TASK_ID)
            {
                Id = (uint8_t)MiniDump_SignalSend[Index].ThreadId + MINIDUMP_INT_OFFSET;
            }
            else    /*This is gki task Id*/
            {
                Id = MiniDump_GetRealTaskID(MiniDump_SignalSend[Index].ThreadId);
            }
        }
        break;
    case TASK_INFO_SECTION_ID:
        {
            /*Array bound check*/
            if((Index >= KI_NUM_OF_REAL_TASKS) || (HistroyIndex >= MINIDUMP_TASK_HISTORY_INFO_NUM))
            {    
                MiniDump_RecordAssertInfo(SYSTEM_DEV_PARAM_TASK_INFO_INVALID_INDEX, 0, Index, HistroyIndex, 0);
                return 0xFF;
            }
            
            if((MiniDump_TaskInfoHistory[Index].MiniDump_TaskHistory[HistroyIndex].InfoTag 
                & MINIDUMP_SIG_SEND_FLAG) != MINIDUMP_SIG_SEND_FLAG
              )
            {
                //receive signal from the interrupt
                if(MiniDump_TaskInfoHistory[Index].MiniDump_TaskHistory[HistroyIndex].TaskId < TIMER_TASK_ID)
                {
                    Id = (uint8_t)MiniDump_TaskInfoHistory[Index].MiniDump_TaskHistory[HistroyIndex].TaskId + MINIDUMP_INT_OFFSET;
                }
                else
                {
                    Id = MiniDump_GetRealTaskID(MiniDump_TaskInfoHistory[Index].MiniDump_TaskHistory[HistroyIndex].TaskId);
                }
            }
            else //send signal
            {
                Id = MiniDump_GetRealTaskID(MiniDump_TaskInfoHistory[Index].MiniDump_TaskHistory[HistroyIndex].TaskId);
            }   
        }
        break;
    }
    
    return Id;
}

/*******************************/
/*L1 modem mini dump API functions*/
/*******************************/
/*Function: check the modem user register inforamtion is correct
parameter: UserId: modem user Id value
return value: true: modem dump register is okay
                  false: modem dump register is error
*/
static bool MiniDump_ModemDumpCheckRegister(MiniDump_ModemUserIdT UserId)
{
    if((UserId < MAX_USERID) 
       && (MiniDump_ModemDump[UserId].ReqSize != 0)
       && (MiniDump_ModemDump[UserId].CallBack != NULL)
      )
    {
        return true;
    }
        
    return false;
}

/*Function: For every modem registered user, add the check value at the 
    bottom of this modem user memory
parameter: UserId: modem user Id value
return value: none
*/
static void MiniDump_ModemDumpSetCheckValue(MiniDump_ModemUserIdT UserId)
{
    void *p = NULL;

    if(UserId >= MAX_USERID)
    {
        MiniDump_RecordAssertInfo(MODEMDUMP_INVALID_PARAM, UserId, 0, 0, 0);
        return ;
    }
    
    p = MiniDump_ModemDump[UserId].StartAddr + MiniDump_ModemDump[UserId].ReqSize;

    memcpy(p, MiniDump_ModemCheckValue, MINIDUMP_MODEM_CHECKVALUE_SIZE);       
}

/*Function: For every modem registered user, check the user foot value is correct
parameter: UserId: modem user Id value
return value: true: foot value is right, no overwrite
                    fasle: foot value is error, overwrite
*/
static bool MiniDump_ModemDumpCheckUserValue(MiniDump_ModemUserIdT UserId)
{
    void *p = NULL;

    if((UserId >= MAX_USERID))
    {
        MiniDump_RecordAssertInfo(MODEMDUMP_INVALID_PARAM, UserId, 0, 0, 0);
        return false;
    }
    
    p = MiniDump_ModemDump[UserId].StartAddr + MiniDump_ModemDump[UserId].ReqSize;

    if(memcmp(p, MiniDump_ModemCheckValue, MINIDUMP_MODEM_CHECKVALUE_SIZE) != 0)
    {
        MiniDump_RecordAssertInfo(MODEMDUMP_FOOTER_OVERWRITE, UserId, 0, 0, 0);
        return false;
    }
    return true;
}

static void MiniDump_ModemDumpUpdateUserStartAddress(void)
{
    uint8_t i = 0;
    uint32_t ModemUsedSize = 0;
    MiniDump_ModemUserIdT UserId = 0;
    for(i = 0; i < MAX_USERID; i++)
    {
        UserId = (MiniDump_ModemUserIdT)i;
        if(MiniDump_ModemDumpCheckRegister(UserId))
        {
            ModemUsedSize += MiniDump_ModemDump[UserId].ReqSize 
                            + MINIDUMP_MODEM_USER_HEADER_SIZE
                            + MINIDUMP_MODEM_CHECKVALUE_SIZE;
            
            MiniDump_ModemDump[UserId].StartAddr = MINIDUMP_MODEMDUMP_END_ADDR 
                                           - ModemUsedSize
                                           + MINIDUMP_MODEM_USER_HEADER_SIZE;

            MiniDump_ModemDumpSetCheckValue(UserId);
        }
    }
}

void MiniDump_ModemDumpRegister(MiniDump_ModemUserIdT UserId, uint32_t ReqSize, MiniDump_CallBackFun CallBackFun)
{
    uint32_t size = 0;
    KiTgtInterruptContext interruptContext;
    
    size = ReqSize + MINIDUMP_MODEM_USER_HEADER_SIZE + MINIDUMP_MODEM_CHECKVALUE_SIZE;
    
    if((UserId >= MAX_USERID) || (CallBackFun == NULL))
    {
       MiniDump_RecordAssertInfo(MODEMDUMP_INVALID_PARAM, UserId, ReqSize, (uint32_t)CallBackFun, 0);
       return ;
    }
    if(MiniDump_ModemDump[UserId].ReqSize != 0)
    {
       MiniDump_RecordAssertInfo(MODEMDUMP_RE_REG_NORMAL_DUMP, UserId, 0, 0, 0);
       return ;
    }
    
    M_KiTgtIntContextDisable(interruptContext);
    
    if(MiniDump_ModemDebugDumpFlag == true)
    {
        M_KiTgtIntContextRestore(interruptContext);
        return ;
    }

    if(size > MiniDump_ModemDumpRemainingSize)
    {
       M_KiTgtIntContextRestore(interruptContext);
       MiniDump_RecordAssertInfo(MODEMDUMP_NO_ENOUGH_MEMORY, UserId, ReqSize, (uint32_t)CallBackFun, 0);
       return ;
    }
    
    MiniDump_ModemDumpRemainingSize = MiniDump_ModemDumpRemainingSize - size;
    MiniDump_ModemDump[UserId].StartAddr = NULL;
    MiniDump_ModemDump[UserId].ReqSize  = ReqSize;
    MiniDump_ModemDump[UserId].CallBack = CallBackFun;  
    M_KiTgtIntContextRestore(interruptContext); 
}

static bool MiniDump_ModemDumpDeRegister(MiniDump_ModemUserIdT UserId)
{
    if(MiniDump_ModemDumpCheckRegister(UserId))
    {
        MiniDump_RecordAssertInfo(MODEMDUMP_INVALID_PARAM, UserId, 0, 0, 0);
        return false;
    }
    if((MiniDump_ModemDebugDumpFlag == true) && (MiniDump_ModemDebugDumpUserId == UserId))
    {
        return false;
    }
        
    MiniDump_ModemDump[UserId].ReqSize   = 0;
    MiniDump_ModemDump[UserId].StartAddr = NULL;
    MiniDump_ModemDump[UserId].CallBack  = NULL;
    
    return true;
}

void MiniDump_ModemDumpDebugRegister(MiniDump_ModemUserIdT UserId, MiniDump_CallBackFun CallBackFun)
{
    uint8_t i = 0;
    KiTgtInterruptContext interruptContext;
    
    if((UserId >= MAX_USERID))
    {
        MiniDump_RecordAssertInfo(MODEMDUMP_INVALID_PARAM, UserId, MINIDUMP_MODEM_DUMP_SIZE, (uint32_t)CallBackFun, 0);
        return ;
    }

    M_KiTgtIntContextDisable(interruptContext);
    if(MiniDump_ModemDebugDumpFlag == false)
    {
        /*Clear all the modem user dump information*/
        for(i = 0; i < MAX_USERID; i++)
        {
            MiniDump_ModemDumpDeRegister(i);
        }
    
        MiniDump_ModemDebugDumpUserId = UserId;
        MiniDump_ModemDebugDumpFlag   = true;
        M_KiTgtIntContextRestore(interruptContext);

        MiniDump_ModemDump[UserId].ReqSize   = MINIDUMP_MODEM_MAX_SIZE 
                                               - MINIDUMP_MODEM_USER_HEADER_SIZE 
                                               - MINIDUMP_MODEM_CHECKVALUE_SIZE;
        MiniDump_ModemDump[UserId].StartAddr = NULL;
        MiniDump_ModemDump[UserId].CallBack  = CallBackFun;
        
        return ;
    }
    else
    {
        M_KiTgtIntContextRestore(interruptContext);
        MiniDump_RecordAssertInfo(MODEMDUMP_RE_REG_FORCE_DUMP, UserId, 0, 0, 0);
        return ;
    }
}

void MiniDump_ModemDumpWriteData(MiniDump_ModemUserIdT UserId, uint32_t offset, uint32_t size, void *data)
{
    /*modem write function is not allowed to be called not in exception flow*/
    if(MiniDump_CheckSystemEnterException() == false)
    {
        SysParam(MODEMDUMP_CALL_WRITE_FUN_NOT_IN_EXCEPTION, UserId, 0, 0, 0); 
    }
    
    if((MiniDump_ModemDebugDumpFlag == true) && (UserId != MiniDump_ModemDebugDumpUserId))
    {
        return;
    }

    if(MiniDump_ModemDumpCheckRegister(UserId) == false)
    {
        MiniDump_RecordAssertInfo(MODEMDUMP_INVALID_PARAM, UserId, 0, 0, 0);
        return;   
    }
    
    if((offset + size) > MiniDump_ModemDump[UserId].ReqSize)
    {
        memcpy(MiniDump_ModemDump[UserId].StartAddr + offset, data, (MiniDump_ModemDump[UserId].ReqSize - offset));
        MiniDump_RecordAssertInfo(MODEMDUMP_FOOTER_OVERWRITE, UserId, offset, size, 0);
        return;
    }

    memcpy(MiniDump_ModemDump[UserId].StartAddr + offset, data, size);

    MiniDump_ModemDumpCheckUserValue(UserId);

}

/*Function: Get the modem section start address from the gki mini dump start address
parameter:none
return value: modem dump start address
*/
static uint32_t MiniDump_ModemDumpGetStartAddr(void)
{
    uint32_t offset = 0, UsedSize = 0;

    /*obtain the modem total used size(user registered size)*/
    UsedSize = MINIDUMP_MODEM_MAX_SIZE - MiniDump_ModemDumpRemainingSize;

    offset = MINIDUMP_MODEMDUMP_BASE_ADDR - UsedSize;
    
    return offset;
}

/*Function: GKI mini dump modem dump section entry
parameter:none
return value: modem dump section's size
*/
static uint32_t MiniDump_SaveModemDumpInfo(void)
{
    uint32_t i = 0, NumWriteBytes = 0;
    MiniDump_ModemUserIdT UserId = 0;           
    MiniDump_BIN_ModemUserHeaderT *pModemDump = NULL;

    /*update the modem dump start address*/
    MiniDump_ModemDumpUpdateUserStartAddress();
    
    for(i = 0; i < MAX_USERID; i++)
    {
        UserId = i;
        if(MiniDump_ModemDumpCheckRegister(UserId) == true)
        {
            pModemDump = (MiniDump_BIN_ModemUserHeaderT *)((uint8_t *)MiniDump_ModemDump[UserId].StartAddr  
                                                            - MINIDUMP_MODEM_USER_HEADER_SIZE
                                                           );
            pModemDump->UserId  = UserId;
            pModemDump->ReqSize = MiniDump_ModemDump[UserId].ReqSize;

            /*Call the User call back function*/
            MiniDump_ModemDump[UserId].CallBack(UserId);

            NumWriteBytes = NumWriteBytes + (MiniDump_ModemDump[UserId].ReqSize  
                                             + MINIDUMP_MODEM_CHECKVALUE_SIZE  
                                             + MINIDUMP_MODEM_USER_HEADER_SIZE
                                             );
            if(MiniDump_ModemDumpCheckUserValue(UserId) == false)
                break;
        }
    }

    return NumWriteBytes;
}

/*************************/
/*gki mini dump API functions*/
/*************************/
/*function :Filter some signal, do not record these signal send history*/
/*parameters:
SignalId:  signal ID value
*/
bool Minidump_SignalFilter(SignalId SigId)
{
    if(SigId == SIG_EMMI_FREE_TX_BLOCK)
    {
        return true;
    }

    return false;
}

/*function :save the signal send history information(task send signal or interrupt sned signal)*/
/*parameters:
SignalId:  signal ID value
ThreadId : the source task ID or exception ID who send the signal
DestTaskId: the destination task ID
*/
void Minidump_RecordSignalSendHistory(KiSigHeader *SigHeader)
{ 
    MiniDump_SignalSendT *SigSend = NULL;
    SignalStructure *sig = NULL;
    SignalId sigId;
    if(SigHeader == NULL)
    {
        return ;
    }
    
    if(SigHeader->format == KI_SIGNAL_FORMAT)
    {
        sig = (SignalStructure *)SigHeader;
        sigId = sig->record.id;
        /*Filter some signal, do not record these signals send history*/
        if(Minidump_SignalFilter(sigId))
        {
            return ;
        }
        SigSend = &MiniDump_SignalSend[MiniDump_SignalSendIndex];
    
        SigSend->TimeStamp  = MiniDump_GetTickCount();
        SigSend->ThreadId   = sig->record.directives.source;
        SigSend->DestTaskId = sig->record.directives.dest;
        SigSend->SignalId   = sigId;

        //increament the global value
        MiniDump_SignalSendIndex++;
        //edge check
        if(MiniDump_SignalSendIndex >= MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM)
        {
            MiniDump_SignalSendIndex = 0;
        }
    }
    else
    {
        return ;
    }
}

/*function :Save the gki task switch history information
parmeters :
priority: current task's priority
*/
ATTR_MD_TEXT_IN_TCM void MiniDump_RecordGkiTaskSwitchHistory(uint32_t Priority)
{
    uint32_t index = 0;
    MiniDump_ContextSwitchT *GkiTsk = NULL;
    KiTgtInterruptContext interruptContext;
    
    M_KiTgtIntContextDisable(interruptContext);
    index = MiniDump_ContextSwitchIndex;
    MiniDump_ContextSwitchIndex++;
    if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
    {
        MiniDump_ContextSwitchIndex = 0;
    }
      
    M_KiTgtIntContextRestore(interruptContext);
    GkiTsk = &MiniDump_ContextSwitch[index];
    
    GkiTsk->TimeStamp  = MiniDump_GetTickCount();
    GkiTsk->ContextArg = (uint32_t)Modem_task_list[Priority];
    GkiTsk->Type       = MINIDUMP_GKI_TASK_FLAG;
    GkiTsk->SleepTime  = 0;
    memset(GkiTsk->TaskName, 0, MINIDUMP_AP_TASK_NAME_SIZE);
    
}

/*function :Save the ap task switch history information
parmeters :
NameAddr: Ap task's name address
*/
ATTR_MD_TEXT_IN_TCM void MiniDump_RecordAPTaskSwitchHistory(char *NameAddr)
{
    uint32_t index = 0;
    MiniDump_ContextSwitchT *ApTsk = NULL;
    KiTgtInterruptContext interruptContext;
    
    M_KiTgtIntContextDisable(interruptContext);
    index = MiniDump_ContextSwitchIndex;
    MiniDump_ContextSwitchIndex++;
    if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
    {
        MiniDump_ContextSwitchIndex = 0;
    }
      
    M_KiTgtIntContextRestore(interruptContext);

    ApTsk = &MiniDump_ContextSwitch[index];
    
    ApTsk->TimeStamp  = MiniDump_GetTickCount();
    ApTsk->Type       = MINIDUMP_AP_TASK_FLAG;
    ApTsk->ContextArg = 0;
    ApTsk->SleepTime  = 0;
    memcpy(ApTsk->TaskName, NameAddr, MINIDUMP_AP_TASK_NAME_SIZE);
}


/*function :Save the interrupt switch history information
parmeters
irqID: the interrupt ID
*/
ATTR_MD_TEXT_IN_TCM void MiniDump_RecordEnterInterruptInfo(uint32_t IrqID)
{
    uint32_t index = 0;
    MiniDump_ContextSwitchT *IsrEnter = NULL;
    KiTgtInterruptContext interruptContext;
    
    M_KiTgtIntContextDisable(interruptContext);
    index = MiniDump_ContextSwitchIndex;
    MiniDump_ContextSwitchIndex++;
    if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
    {
        MiniDump_ContextSwitchIndex = 0;
    }
          
    M_KiTgtIntContextRestore(interruptContext);

    IsrEnter = &MiniDump_ContextSwitch[index];
    
    IsrEnter->TimeStamp  = MiniDump_GetTickCount();
    IsrEnter->ContextArg = IrqID;
    IsrEnter->Type       = MINIDUMP_INTERRUPT_ENTER_FLAG;
    IsrEnter->SleepTime  = 0;
    memset(IsrEnter->TaskName, 0, MINIDUMP_AP_TASK_NAME_SIZE);
}

ATTR_MD_TEXT_IN_TCM void MiniDump_RecordExitInterruptInfo(void)
{
    uint32_t index = 0;
    MiniDump_ContextSwitchT *IsrExit = NULL;
    KiTgtInterruptContext interruptContext;
    
    M_KiTgtIntContextDisable(interruptContext);
    index = MiniDump_ContextSwitchIndex;
    MiniDump_ContextSwitchIndex++;
    if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
    {
        MiniDump_ContextSwitchIndex = 0;
    }
          
    M_KiTgtIntContextRestore(interruptContext);    

    IsrExit = &MiniDump_ContextSwitch[index];
    
    IsrExit->TimeStamp  = MiniDump_GetTickCount();
    IsrExit->ContextArg = MINIDUMP_ISR_END;
    IsrExit->Type       = MINIDUMP_INTERRUPT_EXIT_FLAG;
    IsrExit->SleepTime  = 0;
    memset(IsrExit->TaskName, 0, MINIDUMP_AP_TASK_NAME_SIZE);
}

/*function : Record the system enter light sleep
parmeters:none
return value: none
*/
void MiniDump_RecordEnterLightSleepInfo(void)
{
    uint32_t index = 0;
    MiniDump_ContextSwitchT *EnterSleep = NULL;
    KiTgtInterruptContext interruptContext;
    
    M_KiTgtIntContextDisable(interruptContext);
    index = MiniDump_ContextSwitchIndex;
    MiniDump_ContextSwitchIndex++;
    if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
    {
        MiniDump_ContextSwitchIndex = 0;
    }
          
    M_KiTgtIntContextRestore(interruptContext);    

    EnterSleep = &MiniDump_ContextSwitch[index];
    
    EnterSleep->TimeStamp  = MiniDump_GetTickCount();
    EnterSleep->ContextArg = MINIDUMP_ENTER_LIGHT_SLEEP;
    EnterSleep->Type       = MINIDUMP_LIGHT_SLEEP_FLAG;
    EnterSleep->SleepTime  = 0;
    memset(EnterSleep->TaskName, 0, MINIDUMP_AP_TASK_NAME_SIZE);
}

/*function : Record the system exit light sleep
parmeters:none
return value: none
*/
void MiniDump_RecordExitLightSleepInfo(uint32_t SleepTime)
{
    uint32_t index = 0;
    MiniDump_ContextSwitchT *ExitSleep = NULL;
    KiTgtInterruptContext interruptContext;
    
    M_KiTgtIntContextDisable(interruptContext);
    index = MiniDump_ContextSwitchIndex;
    MiniDump_ContextSwitchIndex++;
    if(MiniDump_ContextSwitchIndex >= MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM)
    {
        MiniDump_ContextSwitchIndex = 0;
    }
          
    M_KiTgtIntContextRestore(interruptContext);    

    ExitSleep = &MiniDump_ContextSwitch[index];
    
    ExitSleep->TimeStamp  = MiniDump_GetTickCount();
    ExitSleep->ContextArg = MINIDUMP_EXIT_LIGHT_SLEEP;
    ExitSleep->Type       = MINIDUMP_LIGHT_SLEEP_FLAG;
    ExitSleep->SleepTime  = SleepTime;
    memset(ExitSleep->TaskName, 0, MINIDUMP_AP_TASK_NAME_SIZE);
}

void Minidump_RecordTaskWaitSemaTime(void)
{
    uint8_t RealTskId = 0;
    RealTskId = MiniDump_GetRealTaskID(KiThisTask());
    if(RealTskId >= KI_NUM_OF_REAL_TASKS)
    {
        return ;
    }
        
    MiniDump_TaskTimeStamp[RealTskId] = MiniDump_GetTickCount();
}

/*
function :Save every task history inforamtion(signal send or receive signal)
parmeters:
SignalId :signal ID
SendOrReceive: mark the current state is signal send or signal receive
ThreadId: signalsend: save the destination task id
    signalreceive:save the source task ID
*/
void MiniDump_RecordTaskInfoHistory(KiSigHeader *SigHeader, uint8_t SendOrReceive)
{  
    uint8_t Id = 0;
    MiniDump_TaskInfoHistoryT *TaskInfo = NULL;
    SignalStructure *sig = NULL;
    uint8_t HisIndex = 0;

    if(SigHeader == NULL)
    {
        return ;
    }  

    if(SigHeader->format == KI_SIGNAL_FORMAT)
    {
        sig = (SignalStructure *)SigHeader;

        if(SendOrReceive == MINIDUMP_SIGNAL_SEND_FLAG)
        {
            Id = MiniDump_GetRealTaskID(sig->record.directives.source);
        }
        else
        {
            Id = MiniDump_GetRealTaskID(sig->record.directives.dest);
        }

        if(Id >= KI_NUM_OF_REAL_TASKS)
        {
            return;
        }
        
        TaskInfo = &MiniDump_TaskInfoHistory[Id];
        HisIndex = TaskInfo->MiniDump_HistoryIndex;

        TaskInfo->MiniDump_TaskHistory[HisIndex].SignalId  = sig->record.id;
        TaskInfo->MiniDump_TaskHistory[HisIndex].TimeStamp = MiniDump_GetTickCount();
        
        MiniDump_TaskTimeStamp[Id] = TaskInfo->MiniDump_TaskHistory[HisIndex].TimeStamp;
        
        if(SendOrReceive == MINIDUMP_SIGNAL_SEND_FLAG)
        {
            TaskInfo->MiniDump_TaskHistory[HisIndex].TaskId   = sig->record.directives.dest;
            TaskInfo->MiniDump_TaskHistory[HisIndex].InfoTag |= MINIDUMP_SIG_SEND_FLAG;
        }
        else
        {
            TaskInfo->MiniDump_TaskHistory[HisIndex].TaskId   = sig->record.directives.source;
            TaskInfo->MiniDump_TaskHistory[HisIndex].InfoTag &= MINIDUMP_SIG_RECEIVE_FLAG;
        }   
        (TaskInfo->MiniDump_HistoryIndex)++;
       
        if((TaskInfo->MiniDump_HistoryIndex) >= MINIDUMP_TASK_HISTORY_INFO_NUM)
        {
            TaskInfo->MiniDump_HistoryIndex = 0;
        }
    }
    else
    {
        return ;
    }
      
}

/*Function: Save the system assert header information
parameter;
Type: system assert type
PoolNum: pool id
retrun value: none
*/
static void MiniDump_SaveAssertHeader(MiniDump_AssertT Type, uint8_t PoolNum)
{
    char *TaskName = NULL;
    uint32_t Exception_Id = 0;
    MiniDump_BIN_AssertHeaderT *pAssertHeader = NULL;
    pAssertHeader = (MiniDump_BIN_AssertHeaderT *)MINIDUMP_ASSERT_START_ADDR;       

    //save the system assert type in the assert section header
    pAssertHeader->AssertType = (uint32_t)Type;
    
    //save the memory pool id in the assert section header
    pAssertHeader->PoolNum = PoolNum;  
    
    /*obtain the previous system xpsr register value*/
    Exception_Id = taskContext.psr & MINIDUMP_EXCEPTION_ID_MASK;

    /*Previous system is in task*/
    if(Exception_Id == MINIDUMP_NOT_EXCEPTION)
    {
        /*Get the current task name(gki task or ap task)*/
        TaskName = pcTaskGetTaskName(xTaskGetCurrentTaskHandle());
        
        pAssertHeader->ThreadId = 0;
        memcpy(pAssertHeader->TaskName, TaskName, 16);
        pAssertHeader->TaskName[15] = '\0';
    }
    else
    {
        pAssertHeader->ThreadId = (uint8_t)(Exception_Id - 16) + MINIDUMP_INT_OFFSET;
        memset(pAssertHeader->TaskName, 0x00, 16);
    }
    
    //save the system assert info in the assert section header
    snprintf(pAssertHeader->AssertInfo,
             MINIDUMP_ASSERT_INFO_MAX_SIZE,
             "Assert failed:%s,file:%s,line:%d,arg1:%d,arg2:%d,arg3:%d",
             (char *)assert_expr.expr,
             (char *)assert_expr.file,
             (int)assert_expr.line,
             (int)MiniDump_AssertInfo.SysArg1,
             (int)MiniDump_AssertInfo.SysArg2,
             (int)MiniDump_AssertInfo.SysArg3
             );
    pAssertHeader->AssertInfo[MINIDUMP_ASSERT_INFO_MAX_SIZE - 1] = '\0';
}

/*
function :Get current task state (suspend task, ready task and blocked task)
parmeters:
TaskID : current real task id
return value: current task state
*/
static uint8_t MiniDump_UpdateTaskStateInfo(uint8_t TaskID)
{
    uint8_t TaskState = 0;
    KiOsTaskHandle TCB = NULL;

    if(TaskID >= KI_NUM_OF_REAL_TASKS)
    {
        return 0xFF;
    }  
    //realtaskid = MiniDump_GetRealTaskID(kiTaskInitTable[TaskID].taskId);
    TCB = kiTaskSplitTable[TaskID].osTaskId;
    //Get the task state(running, blocked, suspend or ready)
    TaskState = eTaskGetState(TCB);
    
    return TaskState;
}


/*function: Get the semaphore ID of the blocked(suspend) task
parmeters:
TaskID :the real task id of suspend task
return value: the waiting semaphore id of blocked(suspend) task
*/
static uint8_t MiniDump_GetSemaphoreID(uint8_t TaskID)
{
    uint32_t TcbSema = 0, QueueSema = 0, RealTskId = 0;
    uint8_t i = 0, SemaId = 0;

    KiOsTaskHandle TaskTCB = NULL;
    
    //minidump_current_taskid=(uint32_t)M_KiTaskGetIndex(kiTaskInitTable[Real_Task_ID].taskId);
    RealTskId = TaskID;
    if(RealTskId >= KI_NUM_UNIQUE_TASK_IDS_DEFINED)
    {
        return MINIDUMP_NO_GKI_SEMA;
    }   
    
    TaskTCB = (void *)kiTaskSplitTable[RealTskId].osTaskId;
    /*Get the task waiting semaphore address*/
    TcbSema = (uint32_t)uxTaskGetEventListItemContainer(TaskTCB);
    
    for(i = 0; i < KI_NUM_SEMAPHORES; i++)
    {
       if(kiSemDefTable[i].osHandle != NULL && kiInitSemCount[i] != -1)
       {
           /*Get the queue semaphore address*/
           QueueSema = (uint32_t)uxQueueGetTasksWaitingToReceive((void *)(kiSemDefTable[i].osHandle));     
       }    
       else
           continue;
       if(TcbSema == QueueSema)
       {
           SemaId = i;  
           break;
       }
    }
    if(i >= KI_NUM_SEMAPHORES)
    {
        SemaId = MINIDUMP_NO_GKI_SEMA;
    }

    return SemaId;
}

/*
Function: according to the number of each signal ID,select the most frequently occuring signal IDs
parameter:
TotalNum: the number of most frequently occuring signal IDs we want wo obtain
*/
static void MiniDump_MemoryAllocateEmptySortSignalNum(uint32_t TotalNum)
{
    uint32_t i = 0, j = 0, index=0;
    MiniDump_AllocateEmptyT temp;
    
    for(i = 0; i < TotalNum; i++)
    {
        index = i;
        for(j = i + 1; j < TotalNum; j++)
        {
            if(MiniDump_AllocateEmpty[j].Num >= MiniDump_AllocateEmpty[index].Num)
            {
                index = j;
            }
                
        }

        if(index != i)
        {
            temp = MiniDump_AllocateEmpty[i];

            MiniDump_AllocateEmpty[i] = MiniDump_AllocateEmpty[index];

            MiniDump_AllocateEmpty[index] = temp;
        }
    }
}

/*
Function: Get pool id
parameter: block header address
return value: current block's pool id
*/
static uint8_t MiniDump_GetMemoryPoolNum(void * addr)
{
    uint8_t PoolNum = 0, i = 0;
    void *PoolStartAddr = NULL, *PoolEndAddr = NULL;

    if(addr == NULL)
    {
        return 0xFF;
    } 
    
    for(i = 0; i < KI_POOL_NUM_USER; i++)
    {
        PoolStartAddr = (void *)kiMemoryPoolTable[i].poolPtr;
        PoolEndAddr   = (void *)((uint8_t *)kiMemoryPoolTable[i].poolPtr + kiMemoryPoolTable[i].poolSize);
        
        if((addr >= PoolStartAddr) && (addr < PoolEndAddr))
        {
            PoolNum = i;

            return PoolNum;
        }
    }

    return 0xFF;
}
/*Function: This function is used to get the allocate empty array's minest num item index. This item
   will be replaced to the new item
parameter: none
return value: the allocate empty array index.
*/
static uint8_t MiniDump_AllocateEmptyGetReplaceItemIndex(void)
{
    uint8_t i = 0, MinNum = 0, index = 0;
    MinNum = MiniDump_AllocateEmpty[0].Num;

    for(i = 0; i < MINIDUMP_BLOCK_MAX_NUM; i++)
    {
        if(MiniDump_AllocateEmpty[i].Num < MinNum)
        {
            MinNum = MiniDump_AllocateEmpty[i].Num;
            index = i;
        }
    }

    return index;
}

/*Function: initialise the strcture of MiniDump_AllocateEmpty,different type of SignalId in current pool blocks have different 
    items in this structure
parameter:
PoolNum: current block's pool num
*/
static uint8_t MiniDump_SaveMemoryAllocateEmptyInfo(uint8_t PoolNum)
{
    uint32_t i = 0, j = 0, BlockIndex = 0, SignalId = 0, AllocSource = 0, CallerAddr = 0;
    SignalStructure *SigBlock = NULL, *SigLastAddr = NULL;
    pFrtBlock block = NULL;
    uint8_t RealId = 0, poolnum = 0;
    bool issig = false;

    poolnum = PoolNum;

    if(poolnum >= KI_POOL_NUM_USER)
    {
        return 0;
    }
        
    
    /*Get the block start address*/
    block = (pFrtBlock)kiMemoryPoolTable[poolnum].poolPtr;
    SigBlock = (SignalStructure *)((pFrtBlock)block + 1);
    
    if(SigBlock == NULL)
    {
        return 0;
    }
       

    /*Get the pool end address*/
    SigLastAddr = (SignalStructure *)((uint8_t *)(kiMemoryPoolTable[poolnum].poolPtr) + kiMemoryPoolTable[poolnum].poolSize);

    for(i = 0; (( i < kiMemoryPoolTable[poolnum].numBlocks) && (SigBlock < SigLastAddr)); i++)
    {
        block = (pFrtBlock)((uint8_t *)(kiMemoryPoolTable[poolnum].poolPtr) + 
                                                   i * (kiMemoryPoolTable[poolnum].blockSize + KI_OS_MEM_BLOCK_OVERHEAD));

        SigBlock = (SignalStructure *)(block + 1);

        issig = KiOsIsMemorySignal((void *)SigBlock);
        
        AllocSource = (uint32_t)(SigBlock->header.genHeader.debugInfo.allocSource);
        CallerAddr  = SigBlock->header.genHeader.debugInfo.callerAddress;

        /*Compared ot gki task id, interrupt id is less than TIMER_TASK_ID,  so determine the 
                system interrupt id from 0 ~ TIMER_TASK_ID.*/
        if(SigBlock->header.genHeader.debugInfo.taskId < TIMER_TASK_ID)
        {
            /*this can be considered as the interrupt id*/
            RealId = (uint8_t)SigBlock->header.genHeader.debugInfo.taskId + MINIDUMP_INT_OFFSET;
        }
            
        else
        {
            /*this can be considered as the gki task id*/
            RealId = (uint8_t)MiniDump_GetRealTaskID(SigBlock->header.genHeader.debugInfo.taskId);
        }
            

        //current block is used for signal send
        if(issig == true)
        {
            SignalId = (uint32_t)SigBlock->record.id;
            
            for(j = 0; j < BlockIndex; j++)
            {
               if((MiniDump_AllocateEmpty[j].SignalId == SignalId) 
                  && (MiniDump_AllocateEmpty[j].TaskId == RealId) 
                  && (MiniDump_AllocateEmpty[j].AllocSource == AllocSource) 
                  && (MiniDump_AllocateEmpty[j].CallerAddress == CallerAddr)
                  )
               {
                    //This is same signal allocate case, we increase the number of this value
                    MiniDump_AllocateEmpty[j].Num++;
                    break;
               }
            }  
        }
        else
        {
           for(j = 0; j < BlockIndex; j++)
            {
               if((MiniDump_AllocateEmpty[j].TaskId == RealId) 
                  && (MiniDump_AllocateEmpty[j].AllocSource == AllocSource) 
                  && (MiniDump_AllocateEmpty[j].CallerAddress == CallerAddr)
                  )
               {
                    //This is same memory allocate case, we increase the number of this value
                    MiniDump_AllocateEmpty[j].Num++;
                    break;
               }
            } 
        }
        
        //no match,allocate a new item to mark this new type of signal allocate or memory allocate
        if(j >= BlockIndex)
        {
            if(BlockIndex >= MINIDUMP_BLOCK_MAX_NUM)
            {
                BlockIndex = MiniDump_AllocateEmptyGetReplaceItemIndex();
            }
 
            
            if(issig == true)
            {
                MiniDump_AllocateEmpty[BlockIndex].SignalId = SignalId;
            } 
            else
            {
                MiniDump_AllocateEmpty[BlockIndex].SignalId = MINIDUMP_NO_SIG_ID;
            }
                
            
            MiniDump_AllocateEmpty[BlockIndex].AllocSource   = AllocSource;
            MiniDump_AllocateEmpty[BlockIndex].TaskId        = RealId;
            MiniDump_AllocateEmpty[BlockIndex].CallerAddress = CallerAddr;
            MiniDump_AllocateEmpty[BlockIndex].Num           = 1;

            BlockIndex++;
        }
    }

    MiniDump_MemoryAllocateEmptySortSignalNum(BlockIndex);

    if(BlockIndex >= MEMORY_ALLOCATE_EMPTY_ITEM_NUM)
    {
        return MEMORY_ALLOCATE_EMPTY_ITEM_NUM;
    }
    else
    {
        return BlockIndex;
    }
        
}

/*Function: check the memory os header is damaged or not
parameter:
PoolNum: current block's pool num
Block: current block address
return value: true represents not damaged, false represents current block's header is damaged
*/
static bool MiniDump_MemoryCheckOSHeaderSanity(uint8_t PoolNum, pFrtBlock Block)
{
    bool CheckResult = false;
    pFrtBlock BlockStartAddr = NULL, BlockEndAddr = NULL;
    uint8_t *PreBlock = NULL;
    uint16_t BlockSize = 0;

    if(PoolNum >= KI_POOL_NUM_USER || Block == NULL)
    {
        return false;
    }     
    
    BlockStartAddr = (pFrtBlock)(kiMemoryPoolTable[PoolNum].poolPtr);
    BlockEndAddr   = (pFrtBlock)((uint8_t *)kiMemoryPoolTable[PoolNum].poolPtr + kiMemoryPoolTable[PoolNum].poolSize);
    BlockSize = kiMemoryPoolTable[PoolNum].blockSize;
    /*Check the block is the first pool block*/
    if((Block >= BlockStartAddr) 
       && (Block < (pFrtBlock)((uint8_t *)BlockStartAddr + BlockSize + KI_OS_MEM_BLOCK_OVERHEAD))
      )
    {
        if((uint32_t)Block->pool == magic_check[PoolNum])
        {
            if(Block->next == MINIDUMP_FRT_INVALID_PTR || (Block->next == NULL))
            {
                CheckResult = true;
            }
        }
    }
    
    else
    {
        PreBlock = (uint8_t *)Block - KI_OS_MEM_BLOCK_OVERHEAD - BlockSize;
        if((uint32_t)Block->pool == magic_check[PoolNum])
        {
            if((Block->next == MINIDUMP_FRT_INVALID_PTR) 
               || ((Block->next >= BlockStartAddr) && (Block->next < BlockEndAddr)) 
               || (Block->next == NULL))
            {   
                if(*(uint32_t *)(PreBlock + sizeof(FrtBlock) + BlockSize) == KI_OS_CHECK_TAIL_VALUE)
                {
                    CheckResult = true;
                }
                
            }
        }
    }
    return CheckResult;
}

/*
Function: Process the memory corruption, memory block's header or footer is damaged
parameter:
PoolNum: current block's pool num
*/
static uint32_t MiniDump_SaveMemoryCorruptionInfo(uint8_t PoolNum)
{
    bool isfind = false;
    MiniDump_AssertT Type;
    uint32_t NumWritebytes = 0, SignalOffset = 0, DumpSize = 0;
    uint8_t *StartDumpP = NULL, *p = NULL, *DamageP = NULL, poolnum = 0;
    pFrtBlock UndamageBlock = NULL, block = NULL, DamageBlock = NULL;
    MiniDump_BIN_CorruptionHeaderT *pCorrupHeader = NULL;
    SignalStructure *SignalBlock = NULL;
    
    Type = (MiniDump_AssertT)MiniDump_AssertInfo.Type;
    poolnum = PoolNum;
    p   = MINIDUMP_ASSERT_START_ADDR;
    block   = (pFrtBlock)(MiniDump_AssertInfo.DumpArg);
    DamageBlock = block;
    pCorrupHeader = (MiniDump_BIN_CorruptionHeaderT *)(p + sizeof(MiniDump_BIN_AssertHeaderT));

            
    if(block == NULL || poolnum >= KI_POOL_NUM_USER)
    {
        MiniDump_SaveAssertHeader(Type, poolnum);
        NumWritebytes = sizeof(MiniDump_BIN_AssertHeaderT);
        return NumWritebytes;
    }
    
    //find the undamage block
    while(block >= (pFrtBlock)(kiMemoryPoolTable[poolnum].poolPtr))
    {
        if(MiniDump_MemoryCheckOSHeaderSanity(poolnum, block) == false)
        {
            block = (pFrtBlock)((uint8_t *)block - (kiMemoryPoolTable[poolnum].blockSize + KI_OS_MEM_BLOCK_OVERHEAD));
        }
        else
        {
            UndamageBlock = block;
            isfind = true;
            break;
        }

    }

    if(Type == MEMORY_OSHEADER_NEXT_VALUE_OVERWRITE_BY_PREV_BLOCK)
    {
        if(isfind == true)
            Type = MEMORY_OSHEADER_NEXT_VALUE_OVERWRITE_BY_PREV_BLOCK;
        else
            Type = MEMORY_OSHEADER_NEXT_VALUE_OVERWRITE_ABOVE_POOL;
        /*Current damaged place is os header next value*/
        DamageP = (uint8_t *)(DamageBlock);
    }
    else if(Type == MEMORY_OSHEADER_POOL_VALUE_OVERWRITE_BY_PREV_BLOCK)
    {
        if(isfind == true)
            Type = MEMORY_OSHEADER_POOL_VALUE_OVERWRITE_BY_PREV_BLOCK;
        else
            Type = MEMORY_OSHEADER_POOL_VALUE_OVERWRITE_ABOVE_POOL;
         /*Current damaged place is os header pool value*/
        DamageP = (uint8_t *)((uint8_t *)DamageBlock + 4);
    }
    else if(Type == MEMORY_MEM_FOOTER_OVERWRITE_BY_PREV_BLOCK)
    {
        if(isfind == true)
            Type = MEMORY_MEM_FOOTER_OVERWRITE_BY_PREV_BLOCK;
        else
            Type = MEMORY_MEM_FOOTER_OVERWRITE_ABOVE_POOL;

        DamageP = (uint8_t *)((uint8_t *)DamageBlock + sizeof(FrtBlock) + 
                                                kiMemoryPoolTable[poolnum].blockSize);
    }
    else if(Type == MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK)
    {
        if(isfind == true)
            Type = MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK;
        else
            Type = MEMORY_SIG_FOOTER_OVERWRITE_ABOVE_POOL;

        SignalBlock  = (SignalStructure *)((uint8_t *)DamageBlock + sizeof(FrtBlock));
        SignalOffset = SignalBlock->record.length + M_GetSignalHeaderLength ();
        DamageP = (uint8_t *)SignalBlock + SignalOffset;
    }
    else if(Type == MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK_INT)
    {
        if(isfind == true)
            Type = MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK_INT;
        else
            Type = MEMORY_SIG_FOOTER_OVERWRITE_ABOVE_POOL_INT;

        SignalBlock  = (SignalStructure *)((uint8_t *)DamageBlock + sizeof(FrtBlock));
        SignalOffset = SignalBlock->record.length + M_GetSignalHeaderLength ();
        DamageP = (uint8_t *)SignalBlock + SignalOffset;
    }
    
    if( (Type == MEMORY_OSHEADER_NEXT_VALUE_OVERWRITE_BY_PREV_BLOCK)
       || (Type == MEMORY_OSHEADER_POOL_VALUE_OVERWRITE_BY_PREV_BLOCK)
       || (Type == MEMORY_MEM_FOOTER_OVERWRITE_BY_PREV_BLOCK) 
       || (Type == MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK)  
       || (Type == MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK_INT)
      )
    {
        StartDumpP = (uint8_t *)UndamageBlock;
    }
    else
    {
        StartDumpP = (uint8_t *)((uint8_t *)block - MEMORY_CORRUPTION_FORESEARCH_SIZE);
    }

    MiniDump_SaveAssertHeader(Type, poolnum);
    
    pCorrupHeader->StartAddr  = (uint32_t)StartDumpP;
    pCorrupHeader->DamageAddr = (uint32_t)DamageP;
    pCorrupHeader->DamageData = *((uint32_t *)DamageP);

    p = p + sizeof(MiniDump_BIN_AssertHeaderT) + sizeof(MiniDump_BIN_CorruptionHeaderT);
    DumpSize = MINIDUMP_ASSERT_MAX_SIZE - sizeof(MiniDump_BIN_AssertHeaderT) - sizeof(MiniDump_BIN_CorruptionHeaderT);
    memcpy(p, StartDumpP, DumpSize);
    NumWritebytes = MINIDUMP_ASSERT_MAX_SIZE;
    return NumWritebytes;
    
}

/*
Function: gki magic and version information
parameter:
offset: the section end address value relative to file head
*/
static uint32_t MiniDump_SaveMagicAndVersion(uint32_t offset)
{
    uint32_t NumWritebytes = 0;
    MiniDump_BIN_HeaderT *p = NULL;
    
    p = (MiniDump_BIN_HeaderT *)(MINIDUMP_GKIDUMP_START_ADDR + offset);

    p->Magic      = (uint32_t)MINIDUMP_MAGIC;
    p->Version    = (uint32_t)MINIDUMP_VERSION;
    p->SectionNum = 0x00;

    NumWritebytes = sizeof(MiniDump_BIN_HeaderT);

    return NumWritebytes;
}


/*
Function: gki mini dump information header, to mange the dynamic data information
parameter:
offset: the section end address value relative to file head
*/
static uint32_t MiniDump_SaveSectionHeader(uint32_t offset)
{
    uint8_t i = 0, SectionNum = 0;
    uint32_t NumWriteBytes = 0;
    MiniDump_BIN_HeaderT *pSectionNum = NULL;
    MiniDump_BIN_SectionT *p = NULL;
    
    p = (MiniDump_BIN_SectionT *)(MINIDUMP_GKIDUMP_START_ADDR + offset);
    pSectionNum = (MiniDump_BIN_HeaderT *)(MINIDUMP_GKIDUMP_START_ADDR);
    
    for(i = 0; i < SECTION_NUM; i++)
    {
        p->SectionId           = i;
        p->SectionStartAddress = 0;
        p->SectionSize         = 0;

        p = (MiniDump_BIN_SectionT *)((uint8_t *)p + (sizeof(MiniDump_BIN_SectionT)));

        SectionNum++;
    }

    pSectionNum->SectionNum = SectionNum;

    NumWriteBytes = sizeof(MiniDump_BIN_SectionT) * SectionNum;

    return NumWriteBytes;
}

/* Function: update the section header information
parameter:
SectionID: which section, the type of section
StartAddress:section start address
SectionSize: secton size
*/
static void MiniDump_SectionHeaderUpdate(MiniDump_SectionIDT SectionId, uint32_t StartAddress, uint32_t SectionSize)
{
    /*Section Header Information*/
    MiniDump_BIN_SectionT *p = NULL;
    
    if(SectionId >= SECTION_NUM)
    {
        return ;
    } 
    
    p = (MiniDump_BIN_SectionT *)(MINIDUMP_GKIDUMP_START_ADDR + sizeof(MiniDump_BIN_HeaderT) + SectionId * sizeof(MiniDump_BIN_SectionT));

    p->SectionId           = SectionId;
    p->SectionStartAddress = StartAddress;
    p->SectionSize         = SectionSize;
}

/* Function: save the signal send inormation in minidump
parameter:
offset: the section end address value relative to file head
*/
static uint32_t MiniDump_SaveSignalSendInfo(uint32_t offset)
{
    uint32_t StartIndex = 0, HisCount = 0, HisNum = 0, NumWritebytes = 0;
    MiniDump_BIN_SignalSendT *p = NULL;
    
    p = (MiniDump_BIN_SignalSendT *)(MINIDUMP_GKIDUMP_START_ADDR + offset);
    MiniDump_CheckHistoryInfo(MINIDUMP_SIGNAL_SEND_CHECK, &StartIndex, &HisNum, 0);

    /*Save the signal send info, from the newest to oldest*/
    for(HisCount = 0; HisCount < MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM; HisCount++)
    {
        if(HisCount >= HisNum)
            break;
        
        p->TimeStamp      = (uint16_t)MiniDump_SignalSend[StartIndex].TimeStamp;
        p->SignalId       = MiniDump_SignalSend[StartIndex].SignalId;
        p->SourceThreadId = MiniDump_HistoryInfoGetRealThreadID(SIG_SECTION_ID, StartIndex, 0);
        p->DestTaskId     = MiniDump_GetRealTaskID(MiniDump_SignalSend[StartIndex].DestTaskId);

        p = (MiniDump_BIN_SignalSendT *)((uint8_t *)p + sizeof(MiniDump_BIN_SignalSendT));

        if(StartIndex <= 0)
        {
            StartIndex = MINIDUMP_SIGNAL_SEND_HISTORY_MAX_NUM;
        }
        StartIndex--;
    }
    
    NumWritebytes = sizeof(MiniDump_BIN_SignalSendT) * HisCount;

    return NumWritebytes;
    
}

/* Function: obtain the ap task's name index in mini dump
parameter:
StartPtr: ap task's name start address
EndPtr: ap task's name end address
TaskName: task's name want to obtain
return value: the index of current task name
*/
static uint8_t MiniDump_TaskSwitchGetApTaskNameIndex(uint8_t *StartPtr, const uint8_t * const EndPtr, const char * const TaskName)
{
    uint8_t NameIndex = 0;
    uint8_t *ptr = NULL;
    
    ptr = StartPtr;

    if(StartPtr == NULL || EndPtr == NULL || TaskName == NULL)
    {
       return 0xFF; 
    }
        
    while(ptr < EndPtr)
    {
        if(memcmp(ptr, TaskName, MINIDUMP_AP_TASK_NAME_SIZE) == 0)
        {
            return NameIndex;
        }

        NameIndex++;
        ptr += MINIDUMP_AP_TASK_NAME_SIZE;
    }

    return 0xFF;
}

/* Function: save the ap task's name in mini dump data
parameter:
offset: the section end address value relative to file head
*/
static uint32_t MiniDump_TaskSwitchSaveAPTaskName(uint32_t offset)
{
    char *TaskName = NULL;
    uint8_t  *p = NULL, *EndP = NULL, *TskNameStartP = NULL;
    uint32_t RemainSize = 0, StartIndex = 0, WriteNum = 0, HistoryCount = 0, HistoryNum = 0;
    MiniDump_BIN_ContextSwitchHeaderT *pHeader = NULL;
    
    p = MINIDUMP_GKIDUMP_START_ADDR + offset; 
    EndP = MINIDUMP_GKIDUMP_START_ADDR + MiniDump_ModemDumpGetStartAddr();
    RemainSize = (uint32_t)(EndP - p);
    MiniDump_CheckHistoryInfo(MINIDUMP_TASKSWITCH_CHECK, &StartIndex, &HistoryNum, 0);
    pHeader = (MiniDump_BIN_ContextSwitchHeaderT *)p;

    /*Record the rewest conext switch time stamp info (the highest 16 bit)*/
    pHeader->TimeStamp = (uint16_t)((MiniDump_ContextSwitch[StartIndex].TimeStamp & 0xFFFF0000) >> 16);
    pHeader->StartAddr = 0;
    
    p += sizeof(MiniDump_BIN_ContextSwitchHeaderT);
    TskNameStartP = p;

    /*Save the context switch info, from the newest to oldest*/
    while(HistoryCount < HistoryNum)
    {
        if((MiniDump_ContextSwitch[StartIndex].Type == MINIDUMP_GKI_TASK_FLAG) 
          || (MiniDump_ContextSwitch[StartIndex].Type == MINIDUMP_INTERRUPT_ENTER_FLAG)
          || (MiniDump_ContextSwitch[StartIndex].Type == MINIDUMP_INTERRUPT_EXIT_FLAG)
        )
        {
            if(RemainSize >= MINIDUMP_CONTEXT_SWITCH_SIZE)
            {
                //GKI task or interrupt, just alloc 3-bytes
                RemainSize -= MINIDUMP_CONTEXT_SWITCH_SIZE;
            }
            else
            {
                /*have no enough memory to allocate, just break stop save the context switch info*/
                break;
            }
        }
        else if(MiniDump_ContextSwitch[StartIndex].Type == MINIDUMP_LIGHT_SLEEP_FLAG)
        {
            if(RemainSize >= MINIDUMP_CONTEXT_SWITCH_SLEEP_SIZE)
            {
                //light sleep, just alloc 6-bytes
                RemainSize -= MINIDUMP_CONTEXT_SWITCH_SLEEP_SIZE;
            }
            else
            {
                /*have no enough memory to allocate, just break stop save the context switch info*/
                break;
            }
        }
        else if(MiniDump_ContextSwitch[StartIndex].Type == MINIDUMP_AP_TASK_FLAG)
        {
            //AP task, we should store AP task name if it's first recorded
            TaskName = MiniDump_ContextSwitch[StartIndex].TaskName;
            
            if(MiniDump_TaskSwitchGetApTaskNameIndex(TskNameStartP, p, TaskName) != 0xFF)
            {
                if(RemainSize >= MINIDUMP_CONTEXT_SWITCH_SIZE)
                {
                    //AP task, it's task name has been recored, just need 3-bytes
                    RemainSize -= MINIDUMP_CONTEXT_SWITCH_SIZE;
                }
                else
                {
                    /*have no enough memory to allocate, just break stop save the context switch info*/
                    break;
                }  
            }
            else if(RemainSize >= MINIDUMP_AP_TASK_SWITCH_SIZE)
            {
                //AP task, it's task name has not been recored, need 10-bytes
                memcpy(p, TaskName, MINIDUMP_AP_TASK_NAME_SIZE);
                p += MINIDUMP_AP_TASK_NAME_SIZE;
                WriteNum += MINIDUMP_AP_TASK_NAME_SIZE;
                RemainSize -= MINIDUMP_AP_TASK_SWITCH_SIZE;
            }
            else
            {
                /*have no enough memory to allocate, just break stop save the context switch info*/
                break;
            }
        }
        else
        {
            //Empty record, jump out
            break;
        }
        if(StartIndex <= 0)
            StartIndex = MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM;
        StartIndex--;
        HistoryCount++;
    }

    pHeader->StartAddr = (WriteNum + sizeof(MiniDump_BIN_ContextSwitchHeaderT));
    return WriteNum;
}

/* Function: save the context switch inforamtion
parameter:
offset: the section end address value relative to file head
NameSize: ap task's name size
*/
static uint32_t MiniDump_TaskSwitchSaveContextSwitchInfo(uint32_t offset, uint32_t NameSize)
{
   char *TaskName = NULL;
   uint8_t *p = NULL, *EndP = NULL, *TskNameP = NULL, *TskNameEndP = NULL, ApTaskNameIndex = 0;
   uint32_t StartIndex = 0, RemainSize = 0, num = 0, HistoryCount = 0, HistoryNum = 0;
   MiniDump_BIN_ContextSwitchT *Context = NULL;
   
   TskNameP   = MINIDUMP_GKIDUMP_START_ADDR + offset + sizeof(MiniDump_BIN_ContextSwitchHeaderT);
   p = TskNameP + NameSize;
   TskNameEndP = p;
   EndP = MINIDUMP_GKIDUMP_START_ADDR + MiniDump_ModemDumpGetStartAddr();
   RemainSize = (uint32_t)(EndP - p);
   Context = (MiniDump_BIN_ContextSwitchT *)p;
   
   MiniDump_CheckHistoryInfo(MINIDUMP_TASKSWITCH_CHECK, &StartIndex, &HistoryNum, 0);
    
   while((HistoryCount < HistoryNum) && (RemainSize >= MINIDUMP_CONTEXT_SWITCH_SIZE))
   {
       /*Current context is light sleep info, we need 6 bytes to save*/
       if(MiniDump_ContextSwitch[StartIndex].Type == MINIDUMP_LIGHT_SLEEP_FLAG)
       {
            /*have no enough memory to save this context switch, just stop*/
            if(RemainSize < MINIDUMP_CONTEXT_SWITCH_SLEEP_SIZE)
            {
                break;
            }
       }

       /*Save the system context switch time stamp*/
       Context->TimeStamp = (uint16_t)(MiniDump_ContextSwitch[StartIndex].TimeStamp);
    
       switch(MiniDump_ContextSwitch[StartIndex].Type)
       {
       case MINIDUMP_GKI_TASK_FLAG:
           {
               Context->ContextArg = MiniDump_GetRealTaskID(MiniDump_ContextSwitch[StartIndex].ContextArg);
           }
           break;
           
       case MINIDUMP_AP_TASK_FLAG:
           {
               TaskName = (char *)(MiniDump_ContextSwitch[StartIndex].TaskName);
               ApTaskNameIndex = MiniDump_TaskSwitchGetApTaskNameIndex(TskNameP, TskNameEndP, TaskName);
               if(ApTaskNameIndex != 0xFF)
                   Context->ContextArg = ApTaskNameIndex + MINIDUMP_APTASK_OFFSET;
               else
                   Context->ContextArg = 0xFF;
           }
           break;
           
       case MINIDUMP_INTERRUPT_ENTER_FLAG:
           {
               Context->ContextArg = (uint8_t)(MiniDump_ContextSwitch[StartIndex].ContextArg + MINIDUMP_INT_OFFSET);
           }
           break;
           
       case MINIDUMP_INTERRUPT_EXIT_FLAG:
           {
               Context->ContextArg = (uint8_t)MINIDUMP_ISR_END;
           }
           break;
           
       case MINIDUMP_LIGHT_SLEEP_FLAG:
           {
                
               if(MiniDump_ContextSwitch[StartIndex].ContextArg == MINIDUMP_ENTER_LIGHT_SLEEP)
               {
                    /*If the system is enter the light sleep, we should record the highest 16bit time stap value*/
                    Context->ContextArg = (uint8_t)MINIDUMP_ENTER_LIGHT_SLEEP;
                    
                    Context = (MiniDump_BIN_ContextSwitchT *)((uint8_t *)Context + sizeof(MiniDump_BIN_ContextSwitchT));
                    Context->ContextArg = (uint8_t)MINIDUMP_ENTER_LIGHT_SLEEP;
                    Context->TimeStamp  = (uint16_t)((MiniDump_ContextSwitch[StartIndex].TimeStamp& 0xFFFF0000) >> 16);
               }
               else
               {
                    /*If the system is exit the light sleep, we should record light sleep time 24bit*/
                    Context->ContextArg = (uint8_t)MINIDUMP_EXIT_LIGHT_SLEEP;

                    Context = (MiniDump_BIN_ContextSwitchT *)((uint8_t *)Context + sizeof(MiniDump_BIN_ContextSwitchT));
                    Context->ContextArg = (uint8_t)MINIDUMP_EXIT_LIGHT_SLEEP;
                    Context->TimeStamp  = (uint16_t)MiniDump_ContextSwitch[StartIndex].SleepTime;
               }
               
               RemainSize -= sizeof(MiniDump_BIN_ContextSwitchT);
               num += sizeof(MiniDump_BIN_ContextSwitchT);
           }
           break;
       }

       if(StartIndex <= 0)
       {
           StartIndex = MINIDUMP_CONTEXT_SWITCH_HISTORY_MAX_NUM; 
       }
           
       StartIndex--;
       HistoryCount++;        
       
       RemainSize -= sizeof(MiniDump_BIN_ContextSwitchT);
       Context = (MiniDump_BIN_ContextSwitchT *)((uint8_t *)Context + sizeof(MiniDump_BIN_ContextSwitchT));
       num += sizeof(MiniDump_BIN_ContextSwitchT);
   }

   num += sizeof(MiniDump_BIN_ContextSwitchHeaderT);
   return num;
}

static uint32_t MiniDump_SaveContextSwitchInfo(uint32_t offset)
{
    uint32_t NameSize = 0, SwitchSize = 0, NumWriteBytes = 0;

    NameSize   = MiniDump_TaskSwitchSaveAPTaskName(offset);
    SwitchSize = MiniDump_TaskSwitchSaveContextSwitchInfo(offset, NameSize);

    NumWriteBytes = NameSize + SwitchSize;

    return NumWriteBytes;
}

static uint32_t MiniDump_SaveTaskInfo(uint32_t offset)
{
    uint8_t StartIndex = 0, TskState = 0, HisNum = 0, check = 0;
    uint32_t i = 0, SemaId = 0, HisCount = 0, NumWritebytes = 0;
    
    uint8_t *p = (uint8_t *)(MINIDUMP_GKIDUMP_START_ADDR + offset);
    
    MiniDump_BIN_SuspendTaskT *pSuspend = NULL;
    MiniDump_BIN_TaskInfoT   *pInfo  = NULL;
    
    //process the task (suspend or ready task) history information
    /*For suspend  or block task, we record the 4B time stamp and the waiting semaphore id
            for ready task, we record the task latest 5 signal send and receive actions
        */
    for(i = 0; i < KI_NUM_OF_REAL_TASKS; i++)
    {
        //update the every task state (ready, blocked or suspend)
        TskState = MiniDump_UpdateTaskStateInfo(i);
        
        //suspend task
        /*if the task state is suspend,we mark the info tag bit6-bit5 as 11 in all histroy information
             for example:
                the suspend task with send signal, the infoTag value is 111+real taskid (bit0~bit4)
                the suspend task with receive signal, the infoTag value is 011+real taskid (bit0~bit4)
                the ready task with send signal, the infoTag value is 100+real taskid(bit0~bit4)
                the raedy task with receive signal, the infoTag value is 000+real taskid
            */
        HisNum = StartIndex = 0;
        check = MiniDump_CheckHistoryInfo(MINIDUMP_TASKINFO_CHECK, (uint32_t *)&StartIndex, (uint32_t *)&HisNum, i);
        if(TskState == eSuspended || TskState == eBlocked)
        {   
            pSuspend = (MiniDump_BIN_SuspendTaskT *)(p);
            //Get the suspend task semaphore ID
            SemaId = MiniDump_GetSemaphoreID(i);

            if(check == HISTORY_INFO_WRAP)
            {
                HisNum = MINIDUMP_BLOCK_TASK_HISTORY_NUM;
            }
            
            if(TskState == eSuspended)
            {
                pSuspend->InfoTag = MINIDUMP_TASKSTATE_SUSPEND_FLAG | (HisNum + 1);
            }   
            else
            {
                pSuspend->InfoTag = MINIDUMP_TASKSTATE_BLOCKED_FLAG | (HisNum + 1);
            }     

            pSuspend->TimeStamp = MiniDump_TaskTimeStamp[i];
            pSuspend->SemaphoreId = SemaId;

            p = p + sizeof(MiniDump_BIN_SuspendTaskT);

            NumWritebytes += sizeof(MiniDump_BIN_SuspendTaskT);      
        }
        else if(TskState == eReady || TskState == eRunning)
        {
            MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].InfoTag &= MINIDUMP_TASKSTATE_NUM_MASK;
            if(TskState == eReady)
            {
                MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].InfoTag &= MINIDUMP_TASKSTATE_READY_FLAG;
            }
            else
            {
                MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].InfoTag |= MINIDUMP_TASKSTATE_RUNNING_FLAG;
            }
            MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].InfoTag |= HisNum;
        }
        /*Save the gki task history info, from the newest to oldest*/
        HisCount = 0;
        pInfo = (MiniDump_BIN_TaskInfoT *)p;
        do
        {
            if((HisNum == 0) && (TskState == eSuspended || TskState == eBlocked))
            {
                break;
            }
            pInfo->InfoTag   = MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].InfoTag;
            pInfo->TimeStamp = (uint16_t)MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].TimeStamp;
            pInfo->TaskID    = MiniDump_HistoryInfoGetRealThreadID(TASK_INFO_SECTION_ID, i, StartIndex);
            pInfo->SignalID  = MiniDump_TaskInfoHistory[i].MiniDump_TaskHistory[StartIndex].SignalId;

            p = p + sizeof(MiniDump_BIN_TaskInfoT);

            pInfo = (MiniDump_BIN_TaskInfoT *)((uint8_t *)pInfo + sizeof(MiniDump_BIN_TaskInfoT));
            if(StartIndex <= 0)
                StartIndex = MINIDUMP_TASK_HISTORY_INFO_NUM;
            StartIndex--;
            HisCount++;

            NumWritebytes += sizeof(MiniDump_BIN_TaskInfoT);
        }while(HisCount < HisNum);
    }

    return NumWritebytes;
}

/*
Function: Save the gki task unprocessed signal inforamtion in input queue
(note: we just record the signal in task internel queue, signal in unit queue
    will be not recored)
prarmeter: this section offset value
return value: this section's size
*/
extern Int32 KiOsGetBufferSignalCount(void);
static uint32_t MiniDump_SaveTaskUnProcessedMesageInfo(uint32_t offset)
{
    uint8_t i = 0, MessageNum = 0;
    uint32_t NumWritebytes = 0, SignalId = 0, SourceTaskId = 0;
    KiSigHeader *sigHeader = NULL;
    KiTaskQueue  *taskQueue = NULL;
    SignalStructure *SignalBlock = NULL;
    MiniDump_BIN_UnProcessedMessageT *MsgP = NULL;
    MiniDump_BIN_UnProcessedMessageHeaderT *MsgHeader = NULL; 
    
    MsgP = (MiniDump_BIN_UnProcessedMessageT *)(MINIDUMP_GKIDUMP_START_ADDR + offset);

    /*Save the system buffer queue's unprocessed signal count*/
    *((uint32_t *)MsgP) = KiOsGetBufferSignalCount();
    MsgP = (MiniDump_BIN_UnProcessedMessageT *)((uint8_t *)MsgP + MINIDUMP_BUFFER_QUEUE_MSG_COUNT_SIZE);
    NumWritebytes += MINIDUMP_BUFFER_QUEUE_MSG_COUNT_SIZE;
    /*Save the gki task info, from the oldest to newest*/
    for(i = 0; i < KI_NUM_OF_REAL_TASKS; i++)
    {
        /*Get the task internel queue pointer*/
        taskQueue = &(kiTaskQueues[M_KiTaskGetIndex(kiTaskInitTable[i].taskId)]);
        MsgHeader = (MiniDump_BIN_UnProcessedMessageHeaderT *)MsgP;
        MsgP = (MiniDump_BIN_UnProcessedMessageT *)((uint8_t *)MsgHeader + sizeof(MiniDump_BIN_UnProcessedMessageHeaderT));

        MsgHeader->MessageNum = 0;
        NumWritebytes += sizeof(MiniDump_BIN_UnProcessedMessageHeaderT);
        if(taskQueue == NULL)
        {
            continue;
        }
        
        if(taskQueue->queue.tail != NULL)
        {
            /*Current queue is not empty*/
            if(taskQueue->queue.head != NULL)
            {
                MessageNum = 0;
                sigHeader = (KiSigHeader *)(taskQueue->queue.head);
                /*foreach unprocessed signal in internel queue, we record them*/
                while(sigHeader != NULL)
                {
                    SignalBlock = (SignalStructure *)sigHeader;

                    SignalId = SignalBlock->record.id;
                    SourceTaskId = SignalBlock->header.genHeader.debugInfo.taskId;
                    
                    MsgP->SignalID     = SignalId;
                    MsgP->SourceTaskId = MiniDump_GetRealTaskID(SourceTaskId);
                    
                    MsgP = (MiniDump_BIN_UnProcessedMessageT *)((uint8_t *)MsgP + sizeof(MiniDump_BIN_UnProcessedMessageT));
                    NumWritebytes += sizeof(MiniDump_BIN_UnProcessedMessageT);

                    MessageNum++;
                    if(MessageNum > MINIDUMP_TASK_UNPROCESSED_MESSAGE_NUM)
                    {
                        MsgHeader->MessageNum = MINIDUMP_TASK_UNPROCESSED_MESSAGE_NUM;
                        break;
                    }
                    sigHeader = (KiSigHeader *)(sigHeader->queue);
                }
                MsgHeader->MessageNum = MessageNum;
            }
        }
        else
        {
            MiniDump_RecordAssertInfo(SYSTEM_DEV_PARAM_TASK_INTERNEL_QUEUE_ERROR, i, 0, 0, 0);
        }
    }
    
    return NumWritebytes;
}

static uint32_t MiniDump_SaveOtherAssertInfo(MiniDump_AssertT Type)
{
    uint32_t NumWritebytes = 0;

    MiniDump_SaveAssertHeader(Type, 0xFF);
    NumWritebytes = sizeof(MiniDump_BIN_AssertHeaderT);
    return NumWritebytes;
}

static uint32_t MiniDump_SaveModemDumpAssertInfo(MiniDump_AssertT Type)
{
    uint8_t i = 0, *ptr = NULL;
    uint32_t NumWritebytes = 0;
    MiniDump_BIN_ModemAssertT *pModem = NULL;
    MiniDump_ModemUserIdT id;
    
    id = (MiniDump_ModemUserIdT)MiniDump_AssertInfo.DumpArg;
    ptr = MINIDUMP_ASSERT_START_ADDR;
    
    MiniDump_SaveAssertHeader(Type, 0xFF);
    
    pModem = (MiniDump_BIN_ModemAssertT *)(ptr + sizeof(MiniDump_BIN_AssertHeaderT));

    NumWritebytes += sizeof(MiniDump_BIN_AssertHeaderT);

    /*nmodem no enough memory assert type, 
       we should dump all registed User ingormation*/
    if(Type == MODEMDUMP_NO_ENOUGH_MEMORY)
    {
        for(i = 0; i < MAX_USERID; i++)
        {
            if(MiniDump_ModemDumpCheckRegister(i))
            {
                pModem->UserId = i;
                pModem->ReqSize = (uint16_t)MiniDump_ModemDump[i].ReqSize;
                pModem->CallBackAddr = (uint32_t)MiniDump_ModemDump[i].CallBack;

                NumWritebytes += sizeof(MiniDump_BIN_ModemAssertT);
                pModem = (MiniDump_BIN_ModemAssertT *)(((uint8_t *)pModem + sizeof(MiniDump_BIN_ModemAssertT)));
            }
        }

        /*Save the invalid modem info*/
        /*the user id of invalid modem info*/
        pModem->UserId = id;
        /*the request size of invalid modem info*/
        pModem->ReqSize = MiniDump_AssertInfo.SysArg1;
        /*the call back function address of invalid modem info*/
        pModem->CallBackAddr = MiniDump_AssertInfo.SysArg2;
        
        NumWritebytes += sizeof(MiniDump_BIN_ModemAssertT);
        pModem = (MiniDump_BIN_ModemAssertT *)(((uint8_t *)pModem + sizeof(MiniDump_BIN_ModemAssertT)));
    }
    else
    {
        pModem->UserId = id;
        /*the request size of invalid modem info*/
        pModem->ReqSize = (uint16_t)MiniDump_AssertInfo.SysArg1;
        /*the call back function address of invalid modem info*/
        pModem->CallBackAddr = MiniDump_AssertInfo.SysArg2;

        NumWritebytes += sizeof(MiniDump_BIN_ModemAssertT);
    }
    
    return NumWritebytes;
}

static uint32_t MiniDump_SaveGkiAssertInfo(MiniDump_AssertT Type)
{
    uint8_t i = 0, poolnum, *p = NULL;
    uint32_t SignalNum = 0, NumWritebytes = 0;
    
    p = MINIDUMP_ASSERT_START_ADDR;
    
    switch(Type)
    {
    case MEMORY_ALLOCATE_EMPTY:
        {
            poolnum = MiniDump_GetMemoryPoolNum((void *)MiniDump_AssertInfo.DumpArg);
            
            MiniDump_SaveAssertHeader(Type, poolnum);      
            
            p += sizeof(MiniDump_BIN_AssertHeaderT);
            MiniDump_BIN_AllocateEmptyT *pSignal = (MiniDump_BIN_AllocateEmptyT *)p;
            
            //obtain the total block signal id, num and task id value
            SignalNum = MiniDump_SaveMemoryAllocateEmptyInfo(poolnum);
            if(SignalNum > MEMORY_ALLOCATE_EMPTY_ITEM_NUM)
                SignalNum = MEMORY_ALLOCATE_EMPTY_ITEM_NUM;
            
            for(i = 0; i < SignalNum; i++)
            {
                pSignal->AllocSource   = MiniDump_AllocateEmpty[i].AllocSource;
                pSignal->TaskId        = MiniDump_AllocateEmpty[i].TaskId;
                pSignal->CallerAddress = MiniDump_AllocateEmpty[i].CallerAddress;
                pSignal->SignalId      = MiniDump_AllocateEmpty[i].SignalId;
                pSignal->Num           = MiniDump_AllocateEmpty[i].Num;

                pSignal = (MiniDump_BIN_AllocateEmptyT *)((uint8_t *)pSignal + sizeof(MiniDump_BIN_AllocateEmptyT));
                p = (uint8_t *)((uint8_t *)p + sizeof(MiniDump_BIN_AllocateEmptyT));
            }

            NumWritebytes = SignalNum * sizeof(MiniDump_BIN_AllocateEmptyT) + sizeof(MiniDump_BIN_AssertHeaderT);
        }
        break;
    case MEMORY_OSHEADER_NEXT_VALUE_OVERWRITE_BY_PREV_BLOCK:
    case MEMORY_OSHEADER_POOL_VALUE_OVERWRITE_BY_PREV_BLOCK:
    case MEMORY_MEM_FOOTER_OVERWRITE_BY_PREV_BLOCK:
    case MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK:
    case MEMORY_SIG_FOOTER_OVERWRITE_BY_PREV_BLOCK_INT:
        {
            poolnum = MiniDump_GetMemoryPoolNum((void *)MiniDump_AssertInfo.DumpArg);
            NumWritebytes = MiniDump_SaveMemoryCorruptionInfo(poolnum);
        }
        break;
    }

    return NumWritebytes;
}

static uint32_t MiniDump_SaveBootUpTimeInfo(void)
{
    uint8_t *p = NULL;
    uint32_t NumWritebytes = 0;

    /*Boot up start dump address*/
    p = MINIDUMP_BOOTUP_START_ADDR;
    
    NumWritebytes = MINIDUMP_BOOT_UP_TIME_SIZE;
    
    return NumWritebytes;
}


static uint32_t MiniDump_SaveAssertInfo(void)
{
    uint32_t NumWritebytes = 0;
    
    if(MiniDump_AssertInfo.Type >= MEMORY_ASSERT_TYPE &&
         MiniDump_AssertInfo.Type < MODEMDUMP_ASSERT_TYPE)
    {
         NumWritebytes = MiniDump_SaveGkiAssertInfo(MiniDump_AssertInfo.Type);
    }
    else if((MiniDump_AssertInfo.Type >= MODEMDUMP_ASSERT_TYPE) && 
                 (MiniDump_AssertInfo.Type < SYSTEM_DEV_CHECK_TYPE))
    {
        NumWritebytes = MiniDump_SaveModemDumpAssertInfo(MiniDump_AssertInfo.Type);
    }
    else
       NumWritebytes = MiniDump_SaveOtherAssertInfo(MiniDump_AssertInfo.Type);

    return NumWritebytes;
}

/*Function: Get the mini dump start saving memory address
If the gki memory pool #4 has free block, we can use this free 
block as the mini dump memory. If all the memory block are 
allocated, we ues the first block as the mini dump memory. 
Due to the every pool #4 block have about 14K memory size, 
we use its 8k memory as the mini dump info memory.
In order to avoid covering the first allocated block info which 
may be saved in the mini dump, we set the mini dump start
address as the block body address + 1K (MINIDUMP_MEMORY_BLOCK_OFFSET).
*/
static void MiniDump_GetMemoryAddr(void)
{
    uint8_t poolIndex = 0, blockNum = 0;
    pFrtBlock blockHeader = NULL;
    KiMemoryBlock *block = NULL;
    poolIndex = KI_POOL_NUM_USER - 1;
    blockHeader = (pFrtBlock)(kiMemoryPoolTable[poolIndex].poolPtr);

    /*find the free block*/
    while(blockNum < kiMemoryPoolTable[poolIndex].numBlocks)
    {
        if(blockHeader > (pFrtBlock)((uint8_t *)kiMemoryPoolTable[poolIndex].poolPtr + kiMemoryPoolTable[poolIndex].poolSize))
        {
            break;
        }
        /*Check whether the memory block is damaged or not*/
        if(MiniDump_MemoryCheckOSHeaderSanity(poolIndex, blockHeader))
        {
            if(blockHeader->next != MINIDUMP_FRT_INVALID_PTR)
            {
                block = (KiMemoryBlock *)(blockHeader + 1);
                minidump_buffer = (uint8_t *)&(block->body);
                /*Check the memory is valid*/
                if((minidump_buffer > ((uint8_t *)kiMemoryPoolTable[poolIndex].poolPtr 
                   + kiMemoryPoolTable[poolIndex].poolSize))
                   || ((uint32_t *)minidump_buffer < kiMemoryPoolTable[poolIndex].poolPtr)
                  )
                {
                    break;
                }
                
                return ;
            }
        }
        blockHeader = (pFrtBlock)((uint8_t *)blockHeader + kiMemoryPoolTable[poolIndex].blockSize + KI_OS_MEM_BLOCK_OVERHEAD);
        blockNum++;
    }
    /*All the memory block has been allocated, so replace 
        one allocated block memory as the gki mini dump memory.
        We use the first block as the gki mini dump memory*/
    minidump_buffer = (uint8_t *)kiMemoryPoolTable[poolIndex].poolPtr 
                                 + sizeof(pFrtBlock) 
                                 + KI_BLOCK_HEADER_LENGTH
                                 + MINIDUMP_MEMORY_BLOCK_OFFSET;
}

/*GKI Mini Dump Entry function*/
static void MiniDump_SaveEntry(void)
{
    uint32_t offset = 0, SectionSize = 0;

    /*GKI mini dump function*/
    SectionSize = MiniDump_SaveMagicAndVersion(offset);
    offset += SectionSize;
    
    SectionSize = MiniDump_SaveSectionHeader(offset);
    offset += SectionSize;
    
    SectionSize = MiniDump_SaveSignalSendInfo(offset);
    offset += SectionSize;
    MiniDump_SectionHeaderUpdate(SIG_SECTION_ID, offset - SectionSize, SectionSize);
    
    SectionSize = MiniDump_SaveTaskInfo(offset);
    offset += SectionSize;
    MiniDump_SectionHeaderUpdate(TASK_INFO_SECTION_ID, offset - SectionSize, SectionSize);

    SectionSize = MiniDump_SaveTaskUnProcessedMesageInfo(offset);
    offset += SectionSize;
    MiniDump_SectionHeaderUpdate(TASK_UNPROCESSED_MESSAGE_SECTION_ID, offset - SectionSize, SectionSize);

    SectionSize = MiniDump_SaveContextSwitchInfo(offset);
    offset += SectionSize;
    MiniDump_SectionHeaderUpdate(CONTEXT_SWITCH_SECTION_ID, offset - SectionSize, SectionSize);
    
    SectionSize = MiniDump_SaveModemDumpInfo();
    MiniDump_SectionHeaderUpdate(MODEM_SECTION_ID, MiniDump_ModemDumpGetStartAddr(), SectionSize);

    SectionSize = MiniDump_SaveBootUpTimeInfo();
    MiniDump_SectionHeaderUpdate(BOOT_UP_TIME_SECTION_ID, MINIDUMP_BOOTUP_BASE_ADDR, SectionSize);
    
    SectionSize = MiniDump_SaveAssertInfo();
    MiniDump_SectionHeaderUpdate(ASSERT_SECTION_ID, MINIDUMP_ASSERT_BASE_ADDR, SectionSize);

}


static unsigned int coredump_elf_hdr(unsigned int offset, int e_phnum)
{
    uint8_t *p = minidump_buffer + offset;

    p[0] = 0x7f;                       /* e_ident[EI_MAG0] = 0x7f */
    p[1] = 0x45;                       /* e_ident[EI_MAG1] = 'E'  */
    p[2] = 0x4c;                       /* e_ident[EI_MAG2] = 'L'  */
    p[3] = 0x46;                       /* e_ident[EI_MAG3] = 'F'  */
    p[4] = 0x1;                        /* 32-bit ELF              */
    p[5] = 0x1;                        /* little endian           */
    p[6] = 0x1;                        /* e_ident[EI_VERSION]     */
    *((uint16_t*)(p+16)) = 0x0004;     /* e_type = core file      */
    *((uint16_t*)(p+18)) = 0x0028;     /* e_machine = ARM         */
    *((uint32_t*)(p+20)) = 0x00000001; /* e_version = 1           */
    *((uint32_t*)(p+28)) = 0x00000034; /* e_phoff                 */
    *((uint16_t*)(p+40)) = 0x0034;     /* e_ehsize                */
    *((uint16_t*)(p+42)) = 0x0020;     /* e_phentsize             */
    *((uint16_t*)(p+44)) = e_phnum;    /* e_phnum                 */

    return 52;
}

static unsigned int coredump_pt_note(unsigned int offset, int e_phnum)
{
    uint8_t *p = minidump_buffer + offset;

    *((uint32_t*)(p+0))  = 0x00000004; /* p_type = PT_NOTE  */
    *((uint32_t*)(p+4))  = 52 + e_phnum*32; /* p_offset */

    /* NT_PRPSINFO: (20 + 124)
     * NT_PRSTATUS: (32 + 148)
     */
    *((uint32_t*)(p+16)) = 0x00000144; /* p_filesz */

    return 32;
}

static unsigned int coredump_pt_load(unsigned int offset, memory_region_type *load_regions, int entries)
{
    unsigned int p_offset, p_addr, p_size, i;
    uint8_t *p = minidump_buffer + offset;

    p_offset =  52                     /* ELF Header */
                + (1 + entries) * 32   /* Program header table */
                + (20 + 124)           /* NT_PRPSINFO */
                + (32 + 148);          /* NT_PRSTATUS */

    for(i = 0; i < entries; i++) {
        *((uint32_t*)(p+0)) = 0x00000001; /* p_type = PT_LOAD */
        *((uint32_t*)(p+4)) = p_offset;   /* p_offset */

        p_addr = (uint32_t)load_regions[i].start_address;
        *((uint32_t*)(p+8))  = p_addr; /* p_vaddr */
        *((uint32_t*)(p+12)) = p_addr; /* p_paddr */

        p_size = (uint32_t)load_regions[i].end_address - (uint32_t)load_regions[i].start_address;
        *((uint32_t*)(p+16)) = p_size; /* p_filesz */
        *((uint32_t*)(p+20)) = p_size; /* p_memsz  */

        *((uint32_t*)(p+24)) = 0x00000007; /* p_flags = RWX  */

        p_offset += p_size;
        p = p + 32;
    }

    return (entries * 32);
}

#define MAX_BUILD_INFO_LEN 14
extern const char build_date_time_str[];
static char coredump_build_info[MAX_BUILD_INFO_LEN+1]={0};
static const char *coredump_task_name = NULL;

static void populate_build_info(char *info, int maxLen)
{
    unsigned int i, j;
    char c;

    /* 2017/04/19 16:48:24 -> 20170419164824 */
    if (maxLen > MAX_BUILD_INFO_LEN) maxLen = MAX_BUILD_INFO_LEN;
    for (i=0, j=0; j < maxLen; i++) {
        c = build_date_time_str[i];
        /* strip non-numerical chars */
        if (!(c >= '0' && c <= '9')) {
            continue;
        }
        info[j++] = c;
    }
    info[j] = 0;
}

static void populate_assert_info(char *info, int maxLen)
{
    if (assert_expr.is_valid) {
        snprintf(info,
                 maxLen,
                 "assert failed: %s, file: %s, line: %d",
                 assert_expr.expr,
                 assert_expr.file,
                 assert_expr.line);
    }
}

static unsigned int coredump_note(unsigned int offset, TaskContext *pTaskContext)
{
    uint8_t *p = minidump_buffer + offset;
    int i;

    /* Fill NT_PRPSINFO */
    *((uint32_t*)(p+0)) = 0x00000005;  /* namesz                 */
    *((uint32_t*)(p+4)) = 0x0000007c;  /* descsz                 */
    *((uint32_t*)(p+8)) = 0x00000003;  /* type = 3 (NT_PRPSINFO) */
    p[12]  = 'C';                      /* name                   */
    p[13]  = 'O';
    p[14]  = 'R';
    p[15]  = 'E';
    *((uint32_t*)(p+16)) = 0x00000000; /* 0 terminator + padding */

    p = p + 20;

    /* descriptor (struct elf_prpsifo) */
    p[1]  = 'R';                       /* pr_sname = 'R'         */
    *((uint16_t*)(p+8))  = 0x0001;     /* pr_uid                 */
    *((uint16_t*)(p+10)) = 0x0001;     /* pr_uid                 */

    memcpy((char*)(p+28), coredump_build_info, 15);  /* pr_fname */
    populate_assert_info((char*)(p+44), 79); /* pr_psargs        */

    p = p + 124;                  /* sizeof(struct elf_prpsinfo) */

    /* Fill NT_PRSTATUS */
    *((uint32_t*)(p+0)) = 0x00000014;  /* namesz                 */
    *((uint32_t*)(p+4)) = 0x00000094;  /* descsz                 */
    *((uint32_t*)(p+8)) = 0x00000001;  /* type = 1 (NT_PRSTATUS) */
    /* name (current task's name) */
    if (coredump_task_name != NULL) {
        for (i = 0; coredump_task_name[i] && (i < 20); i++) {
            p[12 + i] = coredump_task_name[i];
        }
    }
    else {
        p[12]  = 'C';
        p[13]  = 'P';
        p[14]  = 'U';
        p[15]  = '0';
    }

    p = p + 32;

    /* descriptor (struct elf_prstatus) */
    *((uint32_t*)(p+24)) = (uint32_t)xTaskGetCurrentTaskHandle(); /* pr_pid */

    p = p + 72; /* 72 = offset of 'pr_reg' in 'struct elf_prstatus' */

    /* elf_prstatus.pr_reg */
    memcpy(p, pTaskContext, 4 * 17); /* r0-r15, psr */

    return 324; /* 20 + 124 + 32 + 148 */
}

#define ALIGN(x) ((x) - (x)%4)
#define DISTANCE(x, y) ((x) > (y) ? (x) - (y) : (y) - (x))

static void sort(unsigned int *p, unsigned int entries)
{
    unsigned int i, j, swap;
    unsigned int swapped = 0;

    if (entries == 0) return;

    for (i=0; i < (entries-1); i++) {
        for (j = 0; j < (entries-1)-i; j++) {
            if (p[j] > p[j+1]) {
                swap = p[j];
                p[j] = p[j+1];
                p[j+1] = swap;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

static bool sanitize_address(unsigned int *pStart, unsigned int *pSize, unsigned int skip)
{
    unsigned int i;

    if (skip) {
        *pStart = ALIGN(*pStart);
    }

    for (i = 0; ; i++) {
        if ( !memory_regions[i].region_name ) {
            *pSize = 0; /* not within a valid memory region */
            break;
        }
        if (memory_regions[i].is_dumped == 0) {
            continue; /* either a sub-region or rom region */
        }
        if (strcmp(memory_regions[i].region_name, "scs") == 0) {
            continue; /* skip system control register memory space */
        }

        if (*pStart >= (unsigned int)memory_regions[i].start_address && *pStart < (unsigned int)memory_regions[i].end_address) {

            /* check the memory region's start and adjust *pStart if necessary */
            if ((unsigned int)memory_regions[i].start_address + skip > *pStart) {
                *pStart = (unsigned int)memory_regions[i].start_address;
            } else {
                *pStart -= skip;
            }

            if (*pSize) {
                /* check the memory region's end and adjust *pSize if necessary */
                if (*pStart + *pSize > (unsigned int)memory_regions[i].end_address) {
                    *pSize = (unsigned int)memory_regions[i].end_address - *pStart;
                }
            }
            return true;
        }

    }

    return false;
}

static unsigned int *check_regs(unsigned int *first, unsigned int *last)
{
    unsigned int *result = first;
    unsigned int size = 0;

    if (first == last) return last;

    while (first != last)
    {
        if (sanitize_address(first, &size, COREDUMP_REG_RANGE/8))
            *(result++) = *first;
        first++;
    }

    return result;
}

static unsigned int *check_neighbor(unsigned int *first, unsigned int *last)
{
    unsigned int *result = first;

    if (first == last) return last;

    while (++first != last)
    {
        if (ALIGN(*result) + COREDUMP_REG_RANGE <= ALIGN(*first))
           *(++result) = *first;
    }

    return ++result;
}

static unsigned int check_sp(unsigned int *first, unsigned int *last, unsigned int anchor)
{
    unsigned int *result = first;
    unsigned int count = 0;

    while (first != last)
    {
        if (DISTANCE(ALIGN(*first), anchor) > (COREDUMP_REG_RANGE+COREDUMP_STACK_SIZE)/2) {
            *(result++) = *first;
            count++;
        }
        first++;
    }

    return count;
}

static unsigned int populate_load_regions(unsigned int *first, unsigned int *last, unsigned int sp)
{
    unsigned int *anchor = last, *p = first;
    unsigned int count = 0, index = 0, start, size, stack_start, stack_size, reg_regions;
    bool sp_done = false;

    /* check if sp is valid */
    stack_start = sp;
    stack_size  = COREDUMP_STACK_SIZE;
    reg_regions = last - first;
    if (reg_regions < COREDUMP_REG_NUMBER) {
        /* each load region occupies space: 32(PT_LOAD) + COREDUMP_REG_RANGE */
        stack_size += (COREDUMP_REG_NUMBER - reg_regions) * (COREDUMP_REG_RANGE + 32);
    }

    if (sanitize_address(&stack_start, &stack_size, 0)) {
        while (p < last)
        {
            if (*p > sp) {
                anchor = p;
                break;
            }
            p++;
        }
    } else {
        sp_done = true;
        anchor = last;
    }

    while ((first < last) && (count <= COREDUMP_REG_NUMBER))
    {
        if (first == anchor) {
            /* sp region */
            load_regions[index].start_address = (unsigned int *)stack_start;
            load_regions[index].end_address   = (unsigned int *)(stack_start + stack_size);
            sp_done = true;
            index++;
        }
        start = *first;
        size  = COREDUMP_REG_RANGE;
        if (sanitize_address(&start, &size, 0)) {
            load_regions[index].start_address = (unsigned int *)start;
            load_regions[index].end_address   = (unsigned int *)(start + size);
            index++;
            count++;
        }
        first++;
    }

    if (!sp_done) {
        /* sp region */
        load_regions[index].start_address = (unsigned int *)stack_start;
        load_regions[index].end_address   = (unsigned int *)(stack_start + stack_size);
        sp_done = true;
        index++;
    }

    load_regions[index].start_address = (unsigned int *)0xe000ed28;
    load_regions[index].end_address   = (unsigned int *)0xe000ed40;
    index++;

    return index;
}

static unsigned int build_memory_regions(memory_region_type *load_regions, int maxRegs)
{
    unsigned int regs[15], sp; /* r0-r12, lr, pc */
    unsigned int entries;

    memcpy(regs, pTaskContext, 4*13); /* r0-r12 */
    regs[13] = pTaskContext->lr;
    regs[14] = pTaskContext->pc;
    sp = pTaskContext->sp;

    entries = check_regs(regs, regs+sizeof(regs)/sizeof(unsigned int)) - regs;
    sort(regs, entries);
    entries = check_neighbor(regs, regs + entries) - regs;
    entries = check_sp(regs, regs + entries, ALIGN(sp + COREDUMP_STACK_SIZE/2));
    entries = populate_load_regions(regs, regs + entries, sp);

    return entries;
}

static bool is_duplicated_coredump(void)
{
#if defined(MTK_NVDM_MODEM_ENABLE)
    nvdm_modem_status_t status;
    uint8_t i, dump_num;
    uint16_t dump_length, e_phnum, build_offset, task_offset, lr_offset, pc_offset, assert_offset;
    char coredump_assert_info[80];
#endif

    /* Get the converted build info */
    populate_build_info(coredump_build_info, MAX_BUILD_INFO_LEN+1);

    /* Get the assert info.
     * For the cases: foo(.) -> AssertHandlerNoReturn(.) -> platform_assert(.)
     * pc, lr cannot identify the crash context.
     */
    populate_assert_info(coredump_assert_info, 79);

    /* Get the task name */
    coredump_task_name = pcTaskGetTaskName(xTaskGetCurrentTaskHandle());

#if defined(MTK_NVDM_MODEM_ENABLE)
    status = nvdm_modem_query_mini_dump_number(&dump_num);

    if ((status != NVDM_MODEM_STATUS_OK) || (dump_num == 0)) {
        return false;
    }

    for (i = 0; i < dump_num; i++) {
        dump_length = 1024; /* read partial data, note is required */
        memset(minidump_buffer, 0, dump_length);
        status = nvdm_modem_read_mini_dump_data(i, 0, minidump_buffer, &dump_length);
        if ((status != NVDM_MODEM_STATUS_OK) || (dump_length < 496)) {
            return false;
        }
        e_phnum = *((uint16_t*)(minidump_buffer + 0x2c));
        if (e_phnum > 17) {
            return false;
        }

        build_offset  = e_phnum * 32 + 100;
        assert_offset = e_phnum * 32 + 116;
        task_offset   = e_phnum * 32 + 208;
        lr_offset     = e_phnum * 32 + 356;
        pc_offset     = e_phnum * 32 + 360;

        if (  (strcmp(coredump_build_info, (char*)(minidump_buffer + build_offset)) == 0)
           && (strcmp(coredump_assert_info, (char*)(minidump_buffer + assert_offset)) == 0)
           && (strcmp(coredump_task_name, (char*)(minidump_buffer + task_offset)) == 0)
           && (taskContext.lr == *((unsigned int*)(minidump_buffer + lr_offset)))
           && (taskContext.pc == *((unsigned int*)(minidump_buffer + pc_offset))))
        {
            return true;
        }
    }
#endif

    return false;
}


static void coredump_entry(TaskContext *pTaskContext, memory_region_type *memory_regions)
{
    unsigned int entries, offset, i;
    uint32_t len;
#if defined(MTK_NVDM_MODEM_ENABLE)
    nvdm_modem_status_t status;
#endif

    /*Obtain the mini dump memory address
       Mini dump memory is duplicated use os kimemory pool.
       Because when system enter the exception,gki static memory pool
       can be used as mini dump memory.
        */
    MiniDump_GetMemoryAddr();

    if (minidump_buffer == NULL)
    {
        return ;
    }

    memset(minidump_buffer, 0, MINIDUMP_BUFFER_SIZE);

    if(is_duplicated_coredump())
    {
        return ;
    }

    entries = build_memory_regions(load_regions, COREDUMP_REG_NUMBER);

    offset = 0;
    offset += coredump_elf_hdr(offset, entries+1); /* 1 note + 'entries' load */
    offset += coredump_pt_note(offset, entries+1);
    offset += coredump_pt_load(offset, load_regions, entries);
    offset += coredump_note(offset, pTaskContext);

    /* memory data starts from (coredump_header + offset) */
    for (i = 0; i < entries; i++) {
        len = (uint32_t)load_regions[i].end_address - (uint32_t)load_regions[i].start_address;
        memcpy(minidump_buffer + offset, (uint8_t*)load_regions[i].start_address, len);
        offset += len;
    }

    /*GKI mini dump inforamtion*/
    MiniDump_SaveEntry();
    
#if defined(MTK_NVDM_MODEM_ENABLE)
    status = nvdm_modem_write_mini_dump_data(minidump_buffer, MINIDUMP_BUFFER_SIZE);
    (void)status;
#endif
}

#endif /* MTK_MINI_DUMP_ENABLE */

/* It is defined as a weak function.
 * It needs to be implemented in project. 
 * The default behvior is NOP, and the memory dump continues.
 */
__weak void exception_reboot(void)
{
#ifdef HAL_WDT_MODULE_ENABLED
    hal_wdt_config_t wdt_config;
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 10;
    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_init(&wdt_config);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
    // hal_wdt_software_reset();
    while(1);
#endif
}

static int reboot_check(void)
{
    return reboot_flag;
}

#if (PRODUCT_VERSION == 2625)
static bool auto_reboot_check(void)
{
    return (auto_reboot_flag == EXCEPTION_AUTO_REBOOT_MAGIC);
}

//TODO: need log owner offer genie connect status
bool is_genie_connected(void)
{
     return true;
}
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#define EXT_FLASH_READ_SIZE 4

uint32_t context_base = 0x0;
uint8_t context_buffer[EXT_FLASH_READ_SIZE] ={0};
uint32_t context_count = 0;

uint32_t flash_index =0;
#endif

void stackDump(uint32_t stack[])
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    taskContext.r0   = stack[r0];
    taskContext.r1   = stack[r1];
    taskContext.r2   = stack[r2];
    taskContext.r3   = stack[r3];
    taskContext.r12  = stack[r12];
    taskContext.sp   = ((uint32_t)stack) + 0x20;
    taskContext.lr   = stack[lr];
    taskContext.pc   = stack[pc];
    taskContext.psr  = stack[psr];

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.s0 = stack[s0];
        taskContext.s1 = stack[s1];
        taskContext.s2 = stack[s2];
        taskContext.s3 = stack[s3];
        taskContext.s4 = stack[s4];
        taskContext.s5 = stack[s5];
        taskContext.s6 = stack[s6];
        taskContext.s7 = stack[s7];
        taskContext.s8 = stack[s8];
        taskContext.s9 = stack[s9];
        taskContext.s10 = stack[s10];
        taskContext.s11 = stack[s11];
        taskContext.s12 = stack[s12];
        taskContext.s13 = stack[s13];
        taskContext.s14 = stack[s14];
        taskContext.s15 = stack[s15];
        taskContext.fpscr = stack[fpscr];
        taskContext.sp += 72; /* s0-s15, fpsr, reserved */
    }

    /* if CCR.STKALIGN=1, check PSR[9] to know if there is forced stack alignment */
    if ( (SCB->CCR & SCB_CCR_STKALIGN_Msk) && (taskContext.psr & 0x200)) {
        taskContext.sp += 4;
    }

    platform_printf("r0  = 0x%08x\n\r", taskContext.r0);
    platform_printf("r1  = 0x%08x\n\r", taskContext.r1);
    platform_printf("r2  = 0x%08x\n\r", taskContext.r2);
    platform_printf("r3  = 0x%08x\n\r", taskContext.r3);
    platform_printf("r4  = 0x%08x\n\r", taskContext.r4);
    platform_printf("r5  = 0x%08x\n\r", taskContext.r5);
    platform_printf("r6  = 0x%08x\n\r", taskContext.r6);
    platform_printf("r7  = 0x%08x\n\r", taskContext.r7);
    platform_printf("r8  = 0x%08x\n\r", taskContext.r8);
    platform_printf("r9  = 0x%08x\n\r", taskContext.r9);
    platform_printf("r10 = 0x%08x\n\r", taskContext.r10);
    platform_printf("r11 = 0x%08x\n\r", taskContext.r11);
    platform_printf("r12 = 0x%08x\n\r", taskContext.r12);
    platform_printf("lr  = 0x%08x\n\r", taskContext.lr);
    platform_printf("pc  = 0x%08x\n\r", taskContext.pc);
    platform_printf("psr = 0x%08x\n\r", taskContext.psr);
    platform_printf("EXC_RET = 0x%08x\n\r", taskContext.exc_return);

#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    snprintf(trace_buffer, 80, "r0  = 0x%08x\n\r", taskContext.r0);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r1  = 0x%08x\n\r", taskContext.r1);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r2  = 0x%08x\n\r", taskContext.r2);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r3  = 0x%08x\n\r", taskContext.r3);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r4  = 0x%08x\n\r", taskContext.r4);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r5  = 0x%08x\n\r", taskContext.r5);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r6  = 0x%08x\n\r", taskContext.r6);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r7  = 0x%08x\n\r", taskContext.r7);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r8  = 0x%08x\n\r", taskContext.r8);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r9  = 0x%08x\n\r", taskContext.r9);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r10 = 0x%08x\n\r", taskContext.r10);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r11 = 0x%08x\n\r", taskContext.r11);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "r12 = 0x%08x\n\r", taskContext.r12);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "lr  = 0x%08x\n\r", taskContext.lr);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "pc  = 0x%08x\n\r", taskContext.pc);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "psr = 0x%08x\n\r", taskContext.psr);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "EXC_RET = 0x%08x\n\r", taskContext.exc_return);
    FrHslString (0x000A, trace_buffer);
#endif

    /* update CONTROL.SPSEL and psp if returning to thread mode */
    if (taskContext.exc_return & 0x4) {
        taskContext.control |= 0x2; /* CONTROL.SPSel */
        taskContext.psp = taskContext.sp;
    } else { /* update msp if returning to handler mode */
        taskContext.msp = taskContext.sp;
    }

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.control |= 0x4; /* CONTROL.FPCA */
        platform_printf("s0  = 0x%08x\n\r", taskContext.s0);
        platform_printf("s1  = 0x%08x\n\r", taskContext.s1);
        platform_printf("s2  = 0x%08x\n\r", taskContext.s2);
        platform_printf("s3  = 0x%08x\n\r", taskContext.s3);
        platform_printf("s4  = 0x%08x\n\r", taskContext.s4);
        platform_printf("s5  = 0x%08x\n\r", taskContext.s5);
        platform_printf("s6  = 0x%08x\n\r", taskContext.s6);
        platform_printf("s7  = 0x%08x\n\r", taskContext.s7);
        platform_printf("s8  = 0x%08x\n\r", taskContext.s8);
        platform_printf("s9  = 0x%08x\n\r", taskContext.s9);
        platform_printf("s10 = 0x%08x\n\r", taskContext.s10);
        platform_printf("s11 = 0x%08x\n\r", taskContext.s11);
        platform_printf("s12 = 0x%08x\n\r", taskContext.s12);
        platform_printf("s13 = 0x%08x\n\r", taskContext.s13);
        platform_printf("s14 = 0x%08x\n\r", taskContext.s14);
        platform_printf("s15 = 0x%08x\n\r", taskContext.s15);
        platform_printf("s16 = 0x%08x\n\r", taskContext.s16);
        platform_printf("s17 = 0x%08x\n\r", taskContext.s17);
        platform_printf("s18 = 0x%08x\n\r", taskContext.s18);
        platform_printf("s19 = 0x%08x\n\r", taskContext.s19);
        platform_printf("s20 = 0x%08x\n\r", taskContext.s20);
        platform_printf("s21 = 0x%08x\n\r", taskContext.s21);
        platform_printf("s22 = 0x%08x\n\r", taskContext.s22);
        platform_printf("s23 = 0x%08x\n\r", taskContext.s23);
        platform_printf("s24 = 0x%08x\n\r", taskContext.s24);
        platform_printf("s25 = 0x%08x\n\r", taskContext.s25);
        platform_printf("s26 = 0x%08x\n\r", taskContext.s26);
        platform_printf("s27 = 0x%08x\n\r", taskContext.s27);
        platform_printf("s28 = 0x%08x\n\r", taskContext.s28);
        platform_printf("s29 = 0x%08x\n\r", taskContext.s29);
        platform_printf("s30 = 0x%08x\n\r", taskContext.s30);
        platform_printf("s31 = 0x%08x\n\r", taskContext.s31);
        platform_printf("fpscr = 0x%08x\n\r", taskContext.fpscr);
    }

    platform_printf("CONTROL = 0x%08x\n\r", taskContext.control);
    platform_printf("MSP     = 0x%08x\n\r", taskContext.msp);
    platform_printf("PSP     = 0x%08x\n\r", taskContext.psp);
    platform_printf("sp      = 0x%08x\n\r", taskContext.sp);

#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    snprintf(trace_buffer, 80, "CONTROL = 0x%08x\n\r", taskContext.control);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "MSP     = 0x%08x\n\r", taskContext.msp);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "PSP     = 0x%08x\n\r", taskContext.psp);
    FrHslString (0x000A, trace_buffer);
    snprintf(trace_buffer, 80, "sp      = 0x%08x\n\r", taskContext.sp);
    FrHslString (0x000A, trace_buffer);
#endif

#if defined(MTK_MINI_DUMP_ENABLE)
    coredump_entry(pTaskContext, memory_regions);
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

 context_base =  core_get_crash_address();

 if (do_write_to_ext_flash()==true) {
	 crash_ext_flash_init(); 
	 }

 if(is_crash_dump_save_to_flash()==true) {
    coredump(pTaskContext, memory_regions);

	for (flash_index =0; flash_index <512; flash_index++) {
	for(context_count =0; context_count <EXT_FLASH_READ_SIZE;context_count++) {
		hal_flash_read_wrap(flash_index*4+context_count,context_buffer,1);
        printf(" \r\n %x \r\n", context_buffer[0]);
 	}
    }
}

 
 if (do_write_to_ext_flash()==true) {
	 crash_ext_flash_deinit();	 
	}

 
#endif

    if (reboot_check() == DISABLE_MEMDUMP_MAGIC) {
        exception_reboot();
    }
    
#if (PRODUCT_VERSION == 2625)    
    if(auto_reboot_check() && (is_genie_connected() == false)){
        // don't dump if genie tool is not connect
        exception_reboot();
    }
#endif
}

void memoryDumpAll(void)
{
    unsigned int *current, *end;
    unsigned int i;

    for (i = 0; i < exception_config.items; i++) {
        if (exception_config.configs[i].dump_cb) {
            exception_config.configs[i].dump_cb();
        }
    }

    for (i = 0; ; i++) {

        if ( !memory_regions[i].region_name ) {
           break;
        }

        if ( !memory_regions[i].is_dumped ) {
           continue;
        }

        exception_feed_wdt();

        current = memory_regions[i].start_address;
        end     = memory_regions[i].end_address;

        for (; current < end; current += 4) {
#if (PRODUCT_VERSION == 2625)
            //feed wdt every 64k
            if((((unsigned int)current)& 0xFFFFFFF0) % 65536 == 0)
                exception_feed_wdt();
#endif

            if (*(current + 0) == 0 &&
                *(current + 1) == 0 &&
                *(current + 2) == 0 &&
                *(current + 3) == 0 ) {
                    continue;
            }

            platform_printf("0x%08x: %08x %08x %08x %08x\n\r",
                            (unsigned int)current,
                            *(current + 0),
                            *(current + 1),
                            *(current + 2),
                            *(current + 3));
         }
    }

    platform_printf("\n\rmemory dump completed.\n\r");

#if (CHECK_EXCEPTION_STACK_USAGE == 1)
    for (i = 0; i < EXCEPTION_STACK_WORDS; i++) {
        if (xExceptionStack[i] != 0xa5a5a5a5) {
            break;
        }
    }
    platform_printf("\n\rException Stack: used = %d, left = %d\n\r", (EXCEPTION_STACK_WORDS - i)*4, i*4);
#endif

    /* Genie complete message */
    platform_printf("<<<<<<<< LOG END LOG END LOG END LOG END LOG END <<<<<<<<\n");

    if (reboot_check() == DISABLE_WHILELOOP_MAGIC) {
        exception_reboot();
    }

#if (PRODUCT_VERSION == 2625)
    if(auto_reboot_check())
    {
        exception_reboot();
    }
#endif
}

/*
 * Debug scenarios:
 *
 * (1) debug with debugger, stop in first exception.
 *     Print the exception context, and halt cpu.
 *
 *     DEBUGGER_ON: 1
 *
 * (2) debug with uart, stop in first exception.
 *     Print the exception context, and enter an infinite loop.
 *
 *     DEBUGGER_ON: 0
 */

#define DEBUGGER_ON    0

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void Hard_Fault_Handler(uint32_t stack[])
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    platform_printf("\n\rIn Hard Fault Handler\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "\n\rIn Hard Fault Handler\n\r");
#endif
    platform_printf("SCB->HFSR = 0x%08x\n\r", (unsigned int)SCB->HFSR);
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    snprintf(trace_buffer, 80, "SCB->HFSR = 0x%08x\n\r", (unsigned int)SCB->HFSR);
    FrHslString (0x000A, trace_buffer);
#endif
    if ((SCB->HFSR & (1 << 30)) != 0) {
        platform_printf("Forced Hard Fault\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        FrHslString (0x000A, "Forced Hard Fault\n\r");
#endif
        platform_printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
        snprintf(trace_buffer, 80, "SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
        FrHslString (0x000A, trace_buffer);
#endif
        if ((SCB->CFSR & 0xFFFF0000) != 0) {
            printUsageErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x0000FF00) != 0 ) {
            printBusFaultErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x000000FF) != 0 ) {
            printMemoryManagementErrorMsg(SCB->CFSR);
        }
    }

    IsSystemEnterException = true;
    
    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void MemManage_Fault_Handler(uint32_t stack[])
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    platform_printf("\n\rIn MemManage Fault Handler\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "\n\rIn MemManage Fault Handler\n\r");
#endif
    platform_printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    snprintf(trace_buffer, 80, "SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    FrHslString (0x000A, trace_buffer);
#endif
    if ((SCB->CFSR & 0xFF) != 0) {
        printMemoryManagementErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void Bus_Fault_Handler(uint32_t stack[])
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    platform_printf("\n\rIn Bus Fault Handler\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "\n\rIn Bus Fault Handler\n\r");
#endif
    platform_printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    snprintf(trace_buffer, 80, "SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    FrHslString (0x000A, trace_buffer);
#endif
    if ((SCB->CFSR & 0xFF00) != 0) {
        printBusFaultErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void Usage_Fault_Handler(uint32_t stack[])
{
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    char trace_buffer[80];
#endif
    platform_printf("\n\rIn Usage Fault Handler\n\r");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "\n\rIn Usage Fault Handler\n\r");
#endif
    platform_printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR);
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    snprintf(trace_buffer, 80, "SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    FrHslString (0x000A, trace_buffer);
#endif
    if ((SCB->CFSR & 0xFFFF0000) != 0) {
        printUsageErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

#ifdef HAL_DWT_MODULE_ENABLED
void Debug_Monitor_Handler(uint32_t stack[])
{
    uint32_t offset, stack_end, is_match;
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
	const char *task_name = NULL;
#endif

    platform_printf("\n\rIn Debug Monitor Fault Handler\r\n");
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
    FrHslString (0x000A, "\n\rIn Debug Monitor Fault Handler\r\n");
#endif
    /* is task stack overflow? */
    {
        offset = (0x10 * HAL_DWT_3) / 4;
        is_match = ((*(&DWT->FUNCTION0 + offset))& DWT_FUNCTION_MATCHED_Msk)>>DWT_FUNCTION_MATCHED_Pos;
        stack_end = *(&DWT->COMP0 + offset);
        platform_printf("Task stack overflow:%c, stack end:0x%x \r\n",((is_match)? 'Y':'N'),(unsigned int)stack_end);
#if defined (KI_RLG_ENABLE_EVENT_OVER_HSL)
		char trace_buffer[80];
		snprintf(trace_buffer, 80, "Task stack overflow:%c, stack end:0x%x \r\n",((is_match)? 'Y':'N'),(unsigned int)stack_end);
		FrHslString(0x000A, trace_buffer);

		task_name = pcTaskGetTaskName(xTaskGetCurrentTaskHandle());
		if (task_name)
		{
			snprintf(trace_buffer, 80, "Task that overflowed: %s\n\r", task_name);
			FrHslString(0x000A, trace_buffer);
		}
#endif

    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}
#endif /* HAL_DWT_MODULE_ENABLED */

/******************************************************************************/
/*                   Toolchain Dependent Part                                 */
/******************************************************************************/
#if defined(__GNUC__)

#define __EXHDLR_ATTR__ __attribute__((naked)) ATTR_TEXT_IN_RAM

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "ldr r3, =pxExceptionStack     \n"
        "ldr r3, [r3]                  \n"     /* r3 := pxExceptionStack      */
        "ldr r0, =pTaskContext         \n"
        "ldr r0, [r0]                  \n"     /* r0 := pTaskContext          */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0]                  \n"     /* store PSP                   */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "cmp r3, #0                    \n"     /* if (!pxExceptionStack)      */
        "it ne                         \n"
        "movne sp, r3                  \n"     /* update msp                  */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "moveq r0, r4                  \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Hard_Fault_Handler         \n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Bus_Fault_Handler          \n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Usage_Fault_Handler        \n"
    );
}

#ifdef HAL_DWT_MODULE_ENABLED
__EXHDLR_ATTR__  void DebugMon_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Debug_Monitor_Handler      \n"
    );
}
#endif /* HAL_DWT_MODULE_ENABLED */

#endif /* __GNUC__ */

#if defined (__CC_ARM)

#define __EXHDLR_ATTR__ __asm ATTR_TEXT_IN_RAM

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    extern pTaskContext
    extern pxExceptionStack

    PRESERVE8

    cpsid i                       /* disable irq                  */
    ldr r3, =pxExceptionStack
    ldr r3, [r3]                  /* r3 := pxExceptionStack       */
    ldr r0, =pTaskContext
    ldr r0, [r0]                  /* r0 := pTaskContext           */
    add r0, r0, #16               /* point to context.r4          */
    stmia r0!, {r4-r11}           /* store r4-r11                 */
    mov r5, r12                   /* r5 := EXC_RETURN             */
    add r0, r0, #20               /* point to context.control     */
    mrs r1, control               /* move CONTROL to r1           */
    str r1, [r0], #4              /* store CONTROL                */
    str r5, [r0], #4              /* store EXC_RETURN             */
    mrs r4, msp                   /* r4 := MSP                    */
    str r4, [r0], #4              /* store MSP                    */
    mrs r1, psp                   /* move PSP to r1               */
    str r1, [r0]                  /* store PSP                    */
    tst r5, #0x10                 /* FPU context?                 */
    itt eq
    addeq r0, r0, #68             /* point to contex.s16          */
    vstmeq r0, {s16-s31}          /* store r16-r31                */
    cmp r3, #0                    /* if (!pxExceptionStack)       */
    it ne
    movne sp, r3                  /* update msp                   */
    push {lr}
    bl __cpp(exception_init)
    pop  {lr}
    tst r5, #4                    /* thread or handler mode?      */
    ite eq
    moveq r0, r4
    mrsne r0, psp
    bx lr
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Hard_Fault_Handler)
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(MemManage_Fault_Handler)
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Bus_Fault_Handler)
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Usage_Fault_Handler)
}

#ifdef HAL_DWT_MODULE_ENABLED
__EXHDLR_ATTR__  void DebugMon_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Debug_Monitor_Handler)
}
#endif /* HAL_DWT_MODULE_ENABLED */

#endif /* __CC_ARM */

#if defined (__ICCARM__)

#define __EXHDLR_ATTR__  __stackless

ATTR_TEXT_IN_RAM TaskContext* get_pTaskContext(void);
ATTR_TEXT_IN_RAM void CommonFault_Handler(void);
ATTR_TEXT_IN_RAM void HardFault_Handler(void);
ATTR_TEXT_IN_RAM void MemManage_Handler(void);
ATTR_TEXT_IN_RAM void BusFault_Handler(void);
ATTR_TEXT_IN_RAM void UsageFault_Handler(void);
ATTR_TEXT_IN_RAM void DebugMon_Handler(void);

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "mov r3, %0                    \n"     /* r3 := pxExceptionStack      */
        "mov r0, %1                    \n"     /* r0 := pTaskContext          */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0]                  \n"     /* store PSP                   */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "cmp r3, #0                    \n"     /* if (!pxExceptionStack)      */
        "it ne                         \n"
        "movne sp, r3                  \n"     /* update msp                  */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "moveq r0, r4                  \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
        ::"r"(pxExceptionStack), "r"(pTaskContext)
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Hard_Fault_Handler         \n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Bus_Fault_Handler          \n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Usage_Fault_Handler        \n"
    );
}

#ifdef HAL_DWT_MODULE_ENABLED
__EXHDLR_ATTR__  void DebugMon_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Debug_Monitor_Handler      \n"
    );
}
#endif /* HAL_DWT_MODULE_ENABLED */

#endif /* __ICCARM__ */
