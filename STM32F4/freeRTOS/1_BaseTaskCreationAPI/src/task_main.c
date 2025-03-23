#include "include/FreeRTOS.h"
#include "include/task.h"
#include "task_main.h"
#include "user_init.h"

TaskHandle_t 
reverse_led0_state_tskhandle, 
reverse_led1_state_tskhandle;

void reverse_gpio(void* params)
{
    CCGPIOType* paramsLedHandle = (CCGPIOType*)params;
    while(1)
    {
        reverse_ccgpio_pinstate(paramsLedHandle);
        send_message(&uart1, 
            (paramsLedHandle == &led0) ? 
            "led0 is reversed gpio\n" :
            "led1 is reversed gpio\n"
        );
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // if the task quit anyway, we need to delete it
    vTaskDelete(NULL);
}

// create task
void start_task(void* pvParams)
{
    (void)pvParams; // oh, we don't use it
    // to let the task running always the first
    // or, set the tasks with less priorities, that's all ok
    taskENTER_CRITICAL();   
    send_message(&uart1, "\nTask shell create two tasks with reversing leds!\n");
    reverse_ccgpio_pinstate(&led0);
    xTaskCreate(
        (TaskFunction_t)reverse_gpio,
        "reverse_led_state_worker1",
        (const StackType_t)configMINIMAL_STACK_SIZE, 
        (void* const)&led0,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &reverse_led0_state_tskhandle
    );
    send_message(&uart1, "creating the task1 done!\n");

    xTaskCreate(
        (TaskFunction_t)reverse_gpio,
        "reverse_led_state_worker2",
        (const StackType_t)configMINIMAL_STACK_SIZE,
        (void* const)&led1,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &reverse_led1_state_tskhandle
    );
    send_message(&uart1, "creating the task2 done!\n");
    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}   