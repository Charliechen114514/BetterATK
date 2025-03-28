#include "include/FreeRTOS.h"
#include "include/task.h"
#include "task_main.h"
#include "user_init.h"
#include <stdio.h>
TaskHandle_t 
reverse_led0_state_tskhandle, 
reverse_led1_state_tskhandle;

void task_main_1(void* params)
{
    (void)params;
    int times = 0;
    char message_box[50];
    while(1)
    {
        reverse_ccgpio_pinstate(&led0);
        times++;
        snprintf(message_box, 50, "task1: I have been run for %d times!\n", times);
        send_message(&uart1, message_box);
        if(times > 5 && reverse_led1_state_tskhandle){
            send_message(&uart1, "now delete the task2!\n");
            set_ccgpio_state(&led1, CCGPIO_HIGH);
            vTaskDelete(reverse_led1_state_tskhandle);
            reverse_led1_state_tskhandle = NULL;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    // if the task quit anyway, we need to delete it
    vTaskDelete(NULL);
}

void task_main_2(void* params)
{
    (void)params;
    int times = 0;
    char message_box[50];
    while(1)
    {
        reverse_ccgpio_pinstate(&led1);
        times++;
        snprintf(message_box, 50, "task2: I have been run for %d times!\n", times);
        send_message(&uart1, message_box);
        vTaskDelay(pdMS_TO_TICKS(200));
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
        (TaskFunction_t)task_main_1,
        "reverse_led_state_worker1",
        (const StackType_t)configMINIMAL_STACK_SIZE, 
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &reverse_led0_state_tskhandle
    );
    send_message(&uart1, "creating the task1 done!\n");

    xTaskCreate(
        (TaskFunction_t)task_main_2,
        "reverse_led_state_worker2",
        (const StackType_t)configMINIMAL_STACK_SIZE,
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &reverse_led1_state_tskhandle
    );
    send_message(&uart1, "creating the task2 done!\n");
    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}   