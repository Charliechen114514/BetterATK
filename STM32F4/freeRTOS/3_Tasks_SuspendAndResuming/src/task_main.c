#include "include/FreeRTOS.h"
#include "include/task.h"
#include "task_main.h"
#include "user_init.h"
#include <stdio.h>
TaskHandle_t 
reverse_led0_state_tskhandle, 
reverse_led1_state_tskhandle,
key_handling_tskhandle;

// detect the keys
void key_detection_callbacks(void* params)
{
    (void)params;

    while(1)
    {
        if(isKeyPress(&key1, KEY_LONG_PRESS))
        {
            send_message(&uart1, "key1 is pressed!\n");
            vTaskSuspend(reverse_led0_state_tskhandle);
        }
        else if(isKeyPress(&key2, KEY_LONG_PRESS))
        {
            send_message(&uart1, "key2 is pressed!\n");
            vTaskResume(reverse_led0_state_tskhandle);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}


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

    xTaskCreate(
        (TaskFunction_t)key_detection_callbacks,
        "key_detection_worker",
        (const StackType_t)configMINIMAL_STACK_SIZE,
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        &key_handling_tskhandle
    );
    send_message(&uart1, "creating the key task done!\n");    
    

    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}   