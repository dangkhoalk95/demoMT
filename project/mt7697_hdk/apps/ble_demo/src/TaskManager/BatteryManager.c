#include "ble_serial.h"

#include "FreeRTOS.h"
#include "task.h"

log_create_module(PWRMng, PRINT_LEVEL_INFO);

static int getSimulateData(int max, int min){
    static int SimData = 0;
    static bool isRising = true;
    if(SimData >= max){
        isRising = false;
    }
    if(SimData <= min){
        isRising = true;
    }
    if(isRising){
        SimData++;
    } else {
        SimData--;
    }

    return SimData;
}

//TODO: Change this hard code to real function
static int getCurrentBattery(void){
    return getSimulateData(0,100);
}
static void vPwrMngTask(void *pvParameters){
    uint8_t BatValue = (uint8_t)getCurrentBattery();
    // LOG_I(PWRMng,"Start power manager success !");
    for(;;) {
        BatValue++; 
        LOG_I(PWRMng, "Update battery : %i", BatValue);
        // ble_bas_notify_battery_level(512,BatValue);
        vTaskDelay(1000);
    }
}

//TODO: make a return define file
int PWMng_StarTask(void){
    int ret = -1;

    // Config for log
    log_config_print_switch(PWRMng,DEBUG_LOG_ON);

    if(pdPASS != xTaskCreate(vPwrMngTask, 
                             "PowerManger", 
                             512, 
                             NULL, 
                             1, 
                             NULL)){
        LOG_E(PWRMng,"Create batter task failed, check heap stack of freeRTOS.");
    } else {
        ret = 0;
    }
    return ret;
}