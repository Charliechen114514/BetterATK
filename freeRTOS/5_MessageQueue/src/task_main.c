#include <include/FreeRTOS.h>
#include <include/queue.h>
#include <include/task.h>
#include <stdio.h>
#include "task_main.h"
#include "user_init.h"

xQueueHandle    production_handle;

void task_main_1(void* pvParams)
{
    (void)pvParams;
    int producing_product = 0;
    char uart_buffer[40];
    while(1)
    {
        if(xQueueSend(  production_handle, 
                        &producing_product, 
                        portMAX_DELAY) == pdPASS){
            // then we send the issue success
            snprintf(uart_buffer, 40, "Successfully send the datas: %d\r\n", producing_product);
            send_message(&uart1, uart_buffer);
        }else{
            send_message(&uart1, "Queues are full out!");
        }
        producing_product++;
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    vTaskDelete(NULL);
}

void task_main_2(void* pvParams)
{
    (void)pvParams;
    int consuming_product = 0;
    char uart_buffer[40];
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        if(xQueueReceive(
            production_handle, 
            &consuming_product, portMAX_DELAY) == pdPASS)
        {
            snprintf(uart_buffer, 40, "Successfully recv the datas: %d\r\n", consuming_product);
            send_message(&uart1, uart_buffer);
        }else{
            send_message(&uart1, "Queues are empty!\r\n");
        }
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
    production_handle = xQueueCreate(5, sizeof(int));
    send_message(&uart1, "\nTask shell create one demo task to show lists\n");

    xTaskCreate(
        (TaskFunction_t)task_main_1,
        "displaylists",
        (const StackType_t)(configMINIMAL_STACK_SIZE * 2),
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        NULL);
    send_message(&uart1, "creating the task1 done!\n");

    xTaskCreate(
        (TaskFunction_t)task_main_2,
        "flipled",
        (const StackType_t)(configMINIMAL_STACK_SIZE * 2),
        NULL,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        NULL);
    send_message(&uart1, "creating the task2 done!\n");

    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}