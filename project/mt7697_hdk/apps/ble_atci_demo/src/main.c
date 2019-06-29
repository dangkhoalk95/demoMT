#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt7687.h"
#include "system_mt7687.h"

#include "sys_init.h"
#include "task_def.h"

static void vDemoTask(void *pvParameters);
log_create_module(main, PRINT_LEVEL_INFO);

#define mainCHECK_DELAY ( ( portTickType ) 1000 / portTICK_RATE_MS )

static void vDemoTask(void *pvParameters){
    LOG_I(main,"Hello word");    
    for (;;);
}

int main(void){
    int idx;
    system_init();
    log_init(NULL, NULL, NULL);
    xTaskCreate(vDemoTask,"DemoTask",512,NULL,1,NULL);
    SysInitStatus_Set();
    vTaskStartScheduler();
    for (;;);
}

