/*
    Main Entry for the LED Lighten Up
*/
#include "system_clock.h"
#include "include/FreeRTOS.h"
#include "include/task.h"
#include "user_init.h"
#include "task_main.h"

int main(void)
{
    // we need to set the HAL NVIC Settings for the group4
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK, TIM6);
    // system level init
    init_bsp_led();
    init_uart();
    init_bsp_key();

    // init the task
    xTaskCreate(
        (TaskFunction_t)start_task, // task functions
        (const char*)"start_task",  // name handle
        (StackType_t)configMINIMAL_STACK_SIZE,  // stack size using as min
        NULL,   // NULL Params are required
        (UBaseType_t)configMAX_PRIORITIES - 1,  // the max priority
        NULL    // we don't need the handle, it creates things and die :)
    );

    // start the schedular
    vTaskStartScheduler();

    while (1);
    
    return 0;
}
