#include <include/FreeRTOS.h>
#include <include/queue.h>
#include <include/task.h>
#include <stdio.h>
#include "task_main.h"
#include "user_init.h"
#include "semphr.h"

xQueueHandle production_handle;
// this is a binary handle of a SemaphoreHandle
SemaphoreHandle_t my_semaphore;

static void send_sema_value_uart()
{
    int sema_value = uxSemaphoreGetCount(my_semaphore);
    uart_printf(&uart1, "current semavalue: %d\r\n", sema_value);
}

void task_main_1(void *pvParams)
{
    (void)pvParams;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        // give off the semaphore
        xSemaphoreGive(my_semaphore);
        send_sema_value_uart();
        reverse_ccgpio_pinstate(&led0);
        // send a message to show the queue
        send_message(&uart1, "Given off the semaphore in task1\r\n");
    }

    vTaskDelete(NULL);
}



void task_main_2(void *pvParams)
{
    (void)pvParams;
    while (1)
    {
        if (xSemaphoreTake(my_semaphore, portMAX_DELAY) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            send_message(&uart1, "take away the semaphore\r\n");
        }
        reverse_ccgpio_pinstate(&led1);
        send_sema_value_uart();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}

void task_main_3(void *pvParams)
{
    (void)pvParams;
    while (1)
    {
        if (xSemaphoreTake(my_semaphore, portMAX_DELAY) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            send_message(&uart1, "take away the semaphore\r\n");
        }
        // reverse_ccgpio_pinstate(&led1);
        send_sema_value_uart();
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
    my_semaphore = xSemaphoreCreateCounting(10, 0);

    xTaskCreate(
        task_main_1, "given off",
        configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    send_message(&uart1, "task_main_1 successfully send away\r\n");

    xTaskCreate(
        task_main_2, "take away",
        configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    send_message(&uart1, "task_main_2 successfully send away\r\n");

    xTaskCreate(
        task_main_3, "take away2",
        configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    send_message(&uart1, "task_main_3 successfully send away\r\n");

    // at the end, we removed the task depatcher
    vTaskDelete(NULL);

    taskEXIT_CRITICAL();
}