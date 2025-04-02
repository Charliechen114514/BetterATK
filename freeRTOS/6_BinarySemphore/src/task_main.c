#include <include/FreeRTOS.h>
#include <include/queue.h>
#include <include/task.h>
#include <stdio.h>
#include "task_main.h"
#include "user_init.h"
#include "semphr.h"

xQueueHandle    production_handle;
// this is a binary handle of a SemaphoreHandle
SemaphoreHandle_t my_semaphore;

void task_main_1(void* pvParams)
{
    (void)pvParams;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        // give off the semaphore
        xSemaphoreGive(my_semaphore);

        reverse_ccgpio_pinstate(&led0);
        // send a message to show the queue
        send_message(&uart1, "Given off the semaphore\r\n");
    }

    vTaskDelete(NULL);
}

void task_main_2(void* pvParams)
{
    (void)pvParams;
    while(1)
    {
        if(xSemaphoreTake(my_semaphore, portMAX_DELAY) == pdTRUE){
            vTaskDelay(pdMS_TO_TICKS(100));
            send_message(&uart1, "take away the semaphore\r\n");
        }
        reverse_ccgpio_pinstate(&led1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}


// create task
void start_task(void *pvParams)
{
    (void)pvParams; // oh, we don't use it
    // to let the task running always the first
    // or, set the tasks with less priorities, that's all ok
    taskENTER_CRITICAL();
    
    // get the handle
    my_semaphore = xSemaphoreCreateBinary();

    xTaskCreate(
        task_main_1, "given off", 
        configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        
    send_message(&uart1, "task_main_1 successfully send away");
  
    xTaskCreate(
        task_main_2, "take away", 
        configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    send_message(&uart1, "task_main_2 successfully send away");
        
    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}