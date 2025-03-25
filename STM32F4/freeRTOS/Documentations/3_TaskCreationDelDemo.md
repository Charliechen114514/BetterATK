# FreeRTOS: 任务创建和删除Demo

​	下面我们就来简单的聊一聊FreeRTOS的创建的demo。代码在freeRTOS/1_BaseTaskCreationAPI当中。如果你下载了PlaformIO的话，只需要在当前文件夹打开即可。

​	笔者的这个实验现象非常的简单，就是两个灯交替闪烁。这里为了说明代码，我们按照程序的执行流一步一步说明

## Main函数

```c
int main(void)
{
    // we need to set the HAL NVIC Settings for the group4
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK, TIM6);
    // system level init
    init_bsp_led();
    init_uart();

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
```

​	Main函数的代码非常的简单，再上一篇博客中，我们已经提到了xTaskCreate的作用就是动态的创建一个任务，笔者的习惯是——对于这种函数调用强转类型，我们按照上一篇博客所说的内容，依次放入start_task：任务创建的函数代码，"start_task"：任务创建的名称，configMINIMAL_STACK_SIZE，这是最小可以允许的栈深度，参数（咱们的实验中没有参数，或者说，如果你有参数，需要保证参数在整个程序周期中都可以被访问到），configMAX_PRIORITIES - 1：我们给予我们线程创建的最大优先级的调用。和我们不需要任务句柄信息（因为创建结束后我们就会删除。）

​	任务一旦创建结束，我们就立刻开始调度。

## start_task

​	笔者设计的start_task就是一个非常纯粹的进入FREERTOS任务派发的函数，不过，为了演示更加全面的代码，笔者还多加了一部分的东西。你也可以偷懒，把start_task的代码直接CV到调用处直接调用。

```c
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
```

​	熟悉操作系统的朋友，应该不会对“临界区”这个概念陌生，在FreeRTOS当中，提供了两个粒度非常非常大的临界区的声明手段，就是一对宏的调用：

> - taskENTER_CRITICAL宏让代码直接进入了临界区，这个时候，全局的中断都会被关闭，所以，实际上这样的临界区操作是非常的干扰实时性的
> - taskEXIT_CRITICAL宏让代码出临界区，此时全局中断会被再一次开启。

​	这里顺手说一嘴，类似于send_message, reverse_ccgpio_pinstate代码跟我们的FreeRTOS半毛钱关系没有，函数名称就是我们的工作：向串口发送信息和反转LED电平。

​	我们创建好了任务之后，就会去删除“start_task”任务自身，vTaskDelete(NULL)的作用，在前一篇的文档中咱们说过了，内部的vTaskDelete检测传入的任务句柄是自身的时候，就会转向自己的TCB操作。

​	上下的被创建的两个任务，笔者为了演示携带参数特性的FreeRTOS任务创建，这里依次将我们的LED抽象放到了全局中（偷懒，笔者发现放到局部上，这个变量下一次会被回收，这是因为我们的创建的start_task线程已经被delete，下一次访问自然会得到一个非法的LED结构体，这个事情没啥好说的。）

## 翻转LED电平：reverse_gpio函数

```c
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

```

​	我们的任务函数当中出现了一个新的面孔，那就是vTaskDelay函数，这个函数的声明如下：

```c
void vTaskDelay( const TickType_t xTicksToDelay ) PRIVILEGED_FUNCTION;
```

​	注意，vTaskDelay的是FreeRTOS的节拍，而不是毫秒数，笔者为了保证严格的毫秒级延时，使用了一个重要的宏叫做pdMS_TO_TICKS，我们提供的参数中就有一个目标的毫秒数，比如说，`vTaskDelay(pdMS_TO_TICKS(500));`的目的就是为了延时精准的500ms。我们这里简单的说明一下，pdMS_TO_TICKS在干什么。

```c
#define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( uint64_t ) ( xTimeInMs ) * ( uint64_t ) configTICK_RATE_HZ ) / ( uint64_t ) 1000U ) )
```

​	看到了吗，configTICK_RATE_HZ宏的作用就是约束了一个操作系统的节拍频率。这个事情在笔者的FreeRTOSConfig.h中已经有所定义。熟悉操作系统的朋友知道，操作系统中很多工作是中断驱动的，我们的进程的时间片等概念，实际上就是基于我们的时间基configTICK_RATE_HZ。定义的大，时钟整体看来工作的就快，反之就慢，表达时钟的频率低。

​	所以，我们做的事情就是数清楚给定的毫秒个数的节拍多少个，我们知道，configTICK_RATE_HZ是一个1/T量纲的常数，得到一个无量纲的常常数，那自然是T * 1/T了，实际上configTICK_RATE_HZ就在说一个秒内多少个节拍，那xMS自然是上面的那个格式。理解这个事情就好办多了。

​	现在你可以拿出你的F407ZGT6的板子，打开VSCode把代码烧录进去了。