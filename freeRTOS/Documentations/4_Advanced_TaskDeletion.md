# 更加进一步的理解任务的互相操作

​	我们上面的演示实际上已经把FreeRTOS的常见的任务创建给说明白了，但是我们发现，我们似乎一直没有理睬TaskHandle_t，这个任务句柄函数。在最开始的纯粹理论分析的（就是讲解任务抽象的那个代码）文档中，我们指出过，FreeRTOS的任务是有任务句柄的。

```c
typedef struct tskTaskControlBlock         * TaskHandle_t;
```

​	这个tskTaskControlBlock就是我们之前说过的TCB。所以，拿到这个，就当与拿到了进程。

```c
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
```

​	这个demo对应的部分是freeRTOS/2_AdvamcedTaskDelete文件夹的demo，这个demo仍然派发了两个任务，task_main_1和task_main_2。task_main_1当中干的事情就是翻转电平，然后发送信息表达自己的运行状态，同时，如果我们Task_main_1满足了一个特定的条件，比如说我这里就是运行了5次后，就会删除task_main_2，这里我们用到了老朋友vTaskDelete，但是，我们这次指定了任务的句柄就是task_main_2的句柄，删掉他就可以把task_main_2删掉了。

​	上电，程序就会运行，等到我们的led0电平翻转了5次之后，你会发现led1就会停止闪烁了，这就是task_main_2结束了，你也可以使用自己习惯的串口助手查看串口信息。