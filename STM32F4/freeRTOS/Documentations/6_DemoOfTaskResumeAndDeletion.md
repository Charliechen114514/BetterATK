# FreeRTOS：关于任务恢复和调度的demo说明

​	结合上一个demo，笔者这里设计了一个简单的任务挂起和恢复的小demo。

```c
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
```

​	我们这次创建三个任务，一个任务是频繁的按钮检测，key_detection_callbacks，他轮循我们按钮的状态，按钮1嗯下，我们挂起任务，反之我们恢复。可以看到我们就使用了按钮来管理我们的任务状态了。