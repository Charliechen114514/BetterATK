# FreeRTOS的任务管理抽象1——源码级别分析任务的创建和删除

## 前言

​	任务的管理上存在六个非常常见的函数。我们这里先说前三个：这三个函数包揽了我们的任务的创建和删除

- 动态分配的任务创建：

  ```c
      BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                              const char * const pcName,
                              const configSTACK_DEPTH_TYPE uxStackDepth,
                              void * const pvParameters,
                              UBaseType_t uxPriority,
                              TaskHandle_t * const pxCreatedTask ) PRIVILEGED_FUNCTION;
  ```

- 静态分配的任务创建：

  ```c
      TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                      const char * const pcName,
                                      const configSTACK_DEPTH_TYPE uxStackDepth,
                                      void * const pvParameters,
                                      UBaseType_t uxPriority,
                                      StackType_t * const puxStackBuffer,
                                      StaticTask_t * const pxTaskBuffer ) PRIVILEGED_FUNCTION;
  ```

- 任务删除的函数

  ```c
  void vTaskDelete( TaskHandle_t xTaskToDelete ) PRIVILEGED_FUNCTION;
  ```

​	我们的工作就是把这些个函数搞明白，先会用在说。

## 我们的第一个FreeRTOS函数，从xTaskCreate说起

```c
    BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName,
                            const configSTACK_DEPTH_TYPE uxStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask ) PRIVILEGED_FUNCTION;
```

> Create a new task and add it to the list of tasks that are ready to run. Internally, within the FreeRTOS implementation, tasks use two blocks of memory. The first block is used to hold the task's data structures, and the second block is used by the task as its stack. If a task is created using xTaskCreate(), both blocks of memory are automatically dynamically allocated inside the xTaskCreate() function. For more details, refer to [https://www.FreeRTOS.org/a00111.html](https://www.freertos.org/a00111.html). If a task is created using xTaskCreateStatic(), the application writer must provide the required memory, which allows the task to be created without using any dynamic memory allocation. For a version that does not rely on dynamic memory, see xTaskCreateStatic(). The xTaskCreate() function can only be used to create tasks with unrestricted access to the entire microcontroller memory map. Systems with MPU (Memory Protection Unit) support can alternatively create an MPU-constrained task using xTaskCreateRestricted().
>
> The pxTaskCode parameter is a pointer to the task entry function, which must be implemented as a non-returning function, typically containing a continuous loop. The pcName parameter is a descriptive name for the task, mainly used to aid debugging, with a maximum length defined by configMAX_TASK_NAME_LEN (default is 16). The uxStackDepth parameter specifies the size of the task stack as the number of variables it can hold, not the number of bytes. For example, if the stack is 16 bits wide and uxStackDepth is defined as 100, then 200 bytes will be allocated for stack storage. The pvParameters parameter is a pointer passed as a parameter to the created task. The uxPriority parameter sets the priority at which the task should run. For systems with MPU support, tasks can optionally be created in privileged (system) mode by setting the portPRIVILEGE_BIT in the priority parameter. For instance, to create a privileged task with priority 2, set uxPriority to (2 | portPRIVILEGE_BIT). The pxCreatedTask parameter is used to return a handle by which the created task can be referenced. The function returns pdPASS if the task was successfully created and added to a ready list, otherwise it returns an error code defined in the file projdefs.h.

> 创建一个新任务并将其添加到就绪任务列表中。在 FreeRTOS 实现内部，任务使用两个内存块。第一个内存块用于存储任务的数据结构，第二个内存块用于任务的堆栈。如果使用 xTaskCreate() 创建任务，则两个内存块都会在 xTaskCreate() 函数内部自动动态分配。更多信息可参考 [https://www.FreeRTOS.org/a00111.html](https://www.freertos.org/a00111.html。如果使用)，如果使用xTaskCreateStatic() 创建任务，则应用程序编写者必须提供所需的内存，因此 xTaskCreateStatic() 允许创建任务而无需进行任何动态内存分配。关于不使用动态内存分配的版本，可以参考 xTaskCreateStatic()。xTaskCreate() 只能用于创建具有对整个微控制器内存映射无限制访问权限的任务。对于包含 MPU 支持的系统，可以使用 xTaskCreateRestricted() 创建受 MPU 约束的任务。
>
> 参数 pxTaskCode 是指向任务入口函数的指针，任务必须实现为永不返回的形式，即一个持续循环。参数 pcName 是任务的描述性名称，主要用于调试，最大长度由 configMAX_TASK_NAME_LEN 定义，默认为 16。参数 uxStackDepth 指定任务堆栈的大小，表示堆栈可容纳的变量数量，而非字节数。例如，如果堆栈宽度为 16 位且 uxStackDepth 定义为 100，则会分配 200 字节的堆栈存储空间。参数 pvParameters 是一个指针，用作创建任务时传递的参数。参数 uxPriority 指定任务运行的优先级，对于包含 MPU 支持的系统，可以通过设置优先级参数的 portPRIVILEGE_BIT 位来创建特权模式（系统模式）任务。例如，若要以优先级 2 创建特权任务，应将 uxPriority 参数设置为 (2 | portPRIVILEGE_BIT)。参数 pxCreatedTask 用于返回一个任务句柄，以便后续引用该任务。函数返回值为 pdPASS，表示任务创建成功并已添加到就绪列表，否则返回 projdefs.h 文件中定义的错误代码。

​	回忆一下上一篇我说的话——请看任务当中包含了啥呢？答案是——任务的执行流函数，这就是函数签名的第一个参数，TaskFunction_t标识这个是一个我们的任务流函数。第四个参数则是传递我们的函数的参数，笔者等会的demo会演示一下如何传递。还有任务堆栈，这个就是第三个参数uxStackDepth。用来标识我们的堆栈的深度。一般而言，简单的任务可以不用配置的很大，否则会占用不少的空间。

​	还有一些参数，那自然是FreeRTOS相关的配置了。第二个参数就是说明这个任务叫啥，关于为什么要有任务名称这个事情，笔者在后面的附录上有一定的回答，看官自行评判。需要注意的是，我们的任务名称不能太长，你必须保证你的任务名称长度不能超过`configMAX_TASK_NAME_LEN`，这个配置可以自己在FreeRTOSConfig.h中做修正，适当的进行裁剪

```c
#define configMAX_TASK_NAME_LEN                  ( 16 )
```

​	第五个参数uxPriority说明的是我们的任务的优先级。注意的是——笔者自己当时糊涂过，你需要注意的是——任务的优先级的范围，**是`0 ~ configMAX_PRIORITIES - 1`**这个范畴，大于这个范畴的话，你会发现你的程序就会卡死在一个汇编函数出不来，这个是因为非法的访问导致的。

​	第六个参数是一个出参数，也就是我们传递一个空白的TaskHandle来让FreeRTOS填写我们的任务句柄参数。关于这里具体是啥，不是我们这个文档讨论的重点，感兴趣的朋友，可以先自己看源码分析。我们后面会讲述的。

## xTaskCreateStatic:静态创建我们的任务

​	这个函数是需要我们注意的，你需要在你的FreeRTOSConfig中，定义好configSUPPORT_STATIC_ALLOCATION的宏定义，而且要定义为1.

```c
    TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                    const char * const pcName,
                                    const configSTACK_DEPTH_TYPE uxStackDepth,
                                    void * const pvParameters,
                                    UBaseType_t uxPriority,
                                    StackType_t * const puxStackBuffer,
                                    StaticTask_t * const pxTaskBuffer ) PRIVILEGED_FUNCTION
```

> This is the declaration and detailed explanation of the `xTaskCreateStatic()` function in FreeRTOS, which creates a new task and adds it to the list of tasks that are ready to run. Internally, FreeRTOS uses two blocks of memory for each task. The first block stores the task’s internal data structures, while the second block serves as the task’s stack. When using `xTaskCreate()`, both blocks are dynamically allocated within the function. However, with `xTaskCreateStatic()`, the application developer must provide both blocks of memory explicitly, thus eliminating the need for dynamic memory allocation.
>
> The `pxTaskCode` parameter is a pointer to the task’s entry function, which must never return and should be structured as a continuous loop. The `pcName` parameter provides a descriptive name for the task, primarily for debugging purposes, with its maximum length defined by `configMAX_TASK_NAME_LEN` in the `FreeRTOSConfig.h` configuration file. The `uxStackDepth` parameter defines the size of the stack in terms of the number of variables it can contain, not bytes. For instance, if the stack is 32 bits wide and `uxStackDepth` is set to 100, the allocated stack size will be 400 bytes. The `pvParameters` parameter is a pointer passed as the argument to the task being created. The `uxPriority` parameter specifies the priority at which the task will execute.
>
> The `puxStackBuffer` must point to an array of type `StackType_t` with at least `uxStackDepth` elements. This array is used as the task’s stack, avoiding the need for dynamic stack allocation. The `pxTaskBuffer` must point to a variable of type `StaticTask_t`, which will be used to store the task’s internal control structures, also avoiding dynamic allocation.
>
> If both `puxStackBuffer` and `pxTaskBuffer` are not `NULL`, the task will be created successfully and a handle to the task will be returned. If either pointer is `NULL`, the task will not be created and `NULL` will be returned.

> 这是 FreeRTOS 中 `xTaskCreateStatic()` 函数的声明和详细说明，该函数创建一个新任务并将其添加到准备运行的任务列表中。在内部，FreeRTOS 为每个任务使用两个内存块。第一个块存储任务的内部数据结构，而第二个块用作任务的堆栈。使用 `xTaskCreate()` 时，两个块都在函数内动态分配。但是，使用 `xTaskCreateStatic()` 时，应用程序开发人员必须明确提供两个内存块，从而无需动态内存分配。
>
> `pxTaskCode` 参数是指向任务入口函数的指针，该函数绝不能返回，并且应构造为连续循环。 `pcName` 参数为任务提供了一个描述性名称，主要用于调试目的，其最大长度由 `FreeRTOSConfig.h` 配置文件中的 `configMAX_TASK_NAME_LEN` 定义。 `uxStackDepth` 参数根据堆栈可以包含的变量数量（而不是字节数）来定义堆栈的大小。例如，如果堆栈宽度为 32 位，并且 `uxStackDepth` 设置为 100，则分配的堆栈大小将为 400 字节。`pvParameters` 参数是作为参数传递给正在创建的任务的指针。`uxPriority` 参数指定任务执行的优先级。
>
> `puxStackBuffer` 必须指向至少包含 `uxStackDepth` 元素的 `StackType_t` 类型的数组。此数组用作任务的堆栈，避免了动态堆栈分配的需要。`pxTaskBuffer` 必须指向 `StaticTask_t` 类型的变量，该变量将用于存储任务的内部控制结构，同样避免了动态分配。
>
> 如果 `puxStackBuffer` 和 `pxTaskBuffer` 都不为 `NULL`，则任务创建成功并返回任务句柄。如果其中一个指针为 `NULL`，则不会创建任务并返回 `NULL`。

​	xTaskCreateStatic也是创建任务，这个没啥毛病，

​	这个函数上，则是强调静态的分配更多一些。您可以看到puxStackBuffer指向的内存就被认为是一个堆栈，所以，这个场景更加适合的是那些非常固定的场景。他们的分配始终不会超过一个界限。

​	比较起来，`xTaskCreate` 使用动态内存分配方式创建任务，即在运行时通过动态内存分配函数分配任务内存。这种方式可以灵活地适应不同大小和数量的任务，但需要注意内存管理，以防止内存泄漏和堆碎片等问题。**如果任务的数量和大小在设计时已知，且希望避免动态内存分配带来的潜在问题，可以使用静态方法创建任务**，即使用 `xTaskCreateStatic` 函数

## 释放我们的任务：vTaskDelete

​	有创建就有释放，这个实在是没啥好说的。vTaskDelete就是充当的黑脸，当我们的任务结束的时候，我们就要使用vTaskDelete把任务从任务列表中移除，然后将他从占用的内存中释放掉。

```c
void vTaskDelete( TaskHandle_t xTaskToDelete ) PRIVILEGED_FUNCTION;
```

> This is the declaration and description of the `vTaskDelete()` function in FreeRTOS, which removes a task from the RTOS kernel’s management. The function is only available if `INCLUDE_vTaskDelete` is defined as 1 in the FreeRTOS configuration. When a task is deleted using this function, it is removed from all internal kernel lists including the ready list, blocked list, suspended list, and event lists.
>
> It is important to note that although the task is removed from kernel management, the actual memory allocated to it by the kernel is not immediately released. That responsibility lies with the idle task, which will reclaim the memory of deleted tasks during its execution. Therefore, the idle task must be given CPU time to ensure memory cleanup can occur. Furthermore, memory allocated manually by the task itself is not automatically freed and should be released explicitly by the task code before the task is deleted.
>
> The function takes a single parameter, `xTaskToDelete`, which is the handle of the task to be deleted. If `NULL` is passed, the function will delete the task that is currently executing.
>
> An example illustrates how the function can be used. A task is first created using `xTaskCreate()`, and its handle is stored in `xHandle`. Later, this handle is passed to `vTaskDelete()` to delete that specific task.
>
> This function belongs to the `vTaskDelete` group and is categorized under task management in FreeRTOS.

> 这是 FreeRTOS 中 `vTaskDelete()` 函数的声明和描述，该函数从 RTOS 内核管理中删除任务。仅当 FreeRTOS 配置中的 `INCLUDE_vTaskDelete` 定义为 1 时，该函数才可用。使用此函数删除任务时，它会从所有内部内核列表中删除，包括就绪列表、阻止列表、暂停列表和事件列表。
>
> 需要注意的是，虽然任务已从内核管理中删除，但内核分配给它的实际内存不会立即释放。该责任在于空闲任务，它将在执行期间回收已删除任务的内存。因此，必须为空闲任务提供 CPU 时间以确保可以进行内存清理。此外，任务本身手动分配的内存不会自动释放，应在删除任务之前由任务代码明确释放。
>
> 该函数采用单个参数 `xTaskToDelete`，它是要删除的任务的句柄。如果传递 `NULL`，该函数将删除当前正在执行的任务。
>
> 示例说明了如何使用该函数。首先使用 `xTaskCreate()` 创建任务，并将其句柄存储在 `xHandle` 中。稍后，将此句柄传递给 `vTaskDelete()` 以删除该特定任务。
>
> 此函数属于 `vTaskDelete` 组，在 FreeRTOS 中归类为任务管理。

​	vTaskDelete函数就是一个简单的删除任务的函数，值得注意的是——**他不负责释放开辟的内存本身**，这不是他的职责，是你的职责。特别注意的是，如果我们提供的是参数NULL，说明删除的是我们自己。

## 分析源码部分（额外）

​	每一个讲解函数API的文档呢，笔者都会带上一个分析源码的部分文档，这样，我们出现了问题，才知道潜在的原因是什么，不会两眼抓瞎，昏过去了。所有的代码分析的都是2024年的V11.1.0版本，感兴趣的朋友也可以下载下来进行分析。如果你懒得分析，可以看笔者的分析，笔者会给出源代码的

### xTaskCreate函数在做什么？

​	下面的代码是笔者从task.c中摘选出来的

```c
    BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName,
                            const configSTACK_DEPTH_TYPE uxStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask )
    {
        TCB_t * pxNewTCB;
        BaseType_t xReturn;
		// 这个代码用于日志调试使用，我们可以重定义自己的traceENTER_xTaskCreate来看看参数
        traceENTER_xTaskCreate( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, pxCreatedTask );
		// 下面的这个代码的部分是核心的函数
        pxNewTCB = prvCreateTask( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, pxCreatedTask );

        if( pxNewTCB != NULL )
        { 	// 如果我们的MCU是多核的，那么我们必须设置一下绑定的核心是哪一个
            #if ( ( configNUMBER_OF_CORES > 1 ) && ( configUSE_CORE_AFFINITY == 1 ) )
            {
                /* Set the task's affinity before scheduling it. */
                pxNewTCB->uxCoreAffinityMask = configTASK_DEFAULT_CORE_AFFINITY;
            }
            #endif
		// OK，我们添加到我们的就绪队列里，现在我们的任务就绪，可以准备进行调度了
            prvAddNewTaskToReadyList( pxNewTCB );
            xReturn = pdPASS;
        }
        else
        {
            xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
        }

        traceRETURN_xTaskCreate( xReturn );

        return xReturn;
    }
```

​	所以，它实际上就做了两件事情：第一步，给我们的任务构造环境，这是prvCreateTask的任务，下一步，就是将构造好的TCB放到我们的就绪队列里。没了。

​	值得注意的是新版FreeRTOS堆多核MCU的支持：

```c
#if ( ( configNUMBER_OF_CORES > 1 ) && ( configUSE_CORE_AFFINITY == 1 ) )
{
    pxNewTCB->uxCoreAffinityMask = configTASK_DEFAULT_CORE_AFFINITY;
}
#endif
```

​	这是 FreeRTOS 在支持多核处理器架构时的扩展逻辑。若用户启用了多核调度并允许设置核亲和性，那么刚创建的任务将被赋予默认的亲和性掩码，用于限制该任务在哪些核心上调度执行。这种设计提供了对多核资源更细致的调度控制能力。

​	下面，我们就来看看底层的两个函数是怎么回事：

#### 分配和填写我们的TCB：prvCreateTask

​	整个函数实际上分成了两个非常大的部分，一种情况是，我们的栈是向上增长的（这是针对一些DSP芯片而言的，arm的逻辑不是这样的），我们先摆出来所有的细节。

```c
    static TCB_t * prvCreateTask( TaskFunction_t pxTaskCode,
                                  const char * const pcName,
                                  const configSTACK_DEPTH_TYPE uxStackDepth,
                                  void * const pvParameters,
                                  UBaseType_t uxPriority,
                                  TaskHandle_t * const pxCreatedTask )
    {
        TCB_t * pxNewTCB;

        /* If the stack grows down then allocate the stack then the TCB so the stack
         * does not grow into the TCB.  Likewise if the stack grows up then allocate
         * the TCB then the stack. */
        #if ( portSTACK_GROWTH > 0 )
        {
            /* Allocate space for the TCB.  Where the memory comes from depends on
             * the implementation of the port malloc function and whether or not static
             * allocation is being used. */
            /* MISRA Ref 11.5.1 [Malloc memory assignment] */
            /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
            /* coverity[misra_c_2012_rule_11_5_violation] */
            // 我们这里分配了TCB的大小空间
            pxNewTCB = ( TCB_t * ) pvPortMalloc( sizeof( TCB_t ) );

            if( pxNewTCB != NULL )
            {
                // 分配成功，否则我门的分配就失败了
                ( void ) memset( ( void * ) pxNewTCB, 0x00, sizeof( TCB_t ) );

                /* Allocate space for the stack used by the task being created.
                 * The base of the stack memory stored in the TCB so the task can
                 * be deleted later if required. */
                /* MISRA Ref 11.5.1 [Malloc memory assignment] */
                /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
                /* coverity[misra_c_2012_rule_11_5_violation] */
                // 在笔者使用的Demo种，pvPortMallocStack和pvPortMalloc是一个东西，所以这里就是分配我们的栈
                pxNewTCB->pxStack = ( StackType_t * ) pvPortMallocStack( ( ( ( size_t ) uxStackDepth ) * sizeof( StackType_t ) ) );
			   // oh，栈没被分配成功，我们释放我们的东西准备回家
                if( pxNewTCB->pxStack == NULL )
                {
                    /* Could not allocate the stack.  Delete the allocated TCB. */
                    vPortFree( pxNewTCB );
                    pxNewTCB = NULL;
                }
            }
        }
        // 这里才是我们关心的逻辑，arm架构体系要求我们的栈是向下增长的
        #else /* portSTACK_GROWTH */
        {
            StackType_t * pxStack;

            /* Allocate space for the stack used by the task being created. */
            /* MISRA Ref 11.5.1 [Malloc memory assignment] */
            /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
            /* coverity[misra_c_2012_rule_11_5_violation] */
            pxStack = pvPortMallocStack( ( ( ( size_t ) uxStackDepth ) * sizeof( StackType_t ) ) );

            if( pxStack != NULL )
            {
                /* Allocate space for the TCB. */
                /* MISRA Ref 11.5.1 [Malloc memory assignment] */
                /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
                /* coverity[misra_c_2012_rule_11_5_violation] */
                pxNewTCB = ( TCB_t * ) pvPortMalloc( sizeof( TCB_t ) );

                if( pxNewTCB != NULL )
                {
                    ( void ) memset( ( void * ) pxNewTCB, 0x00, sizeof( TCB_t ) );

                    /* Store the stack location in the TCB. */
                    pxNewTCB->pxStack = pxStack;
                }
                else
                {
                    /* The stack cannot be used as the TCB was not created.  Free
                     * it again. */
                    vPortFreeStack( pxStack );
                }
            }
            else
            {
                pxNewTCB = NULL;
            }
        }
        #endif /* portSTACK_GROWTH */

        if( pxNewTCB != NULL )
        {
            #if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
            {
                /* Tasks can be created statically or dynamically, so note this
                 * task was created dynamically in case it is later deleted. */
                pxNewTCB->ucStaticallyAllocated = tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB;
            }
            #endif /* tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE */

            prvInitialiseNewTask( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, pxCreatedTask, pxNewTCB, NULL );
        }

        return pxNewTCB;
    }
```

​	这段代码非常的长。但是却设计到一个非常根本的一个问题。那就是TCB和栈，谁先分配呢？答案是——整个问题需要我们分类讨论。正如我们的FreeRTOS源码那样。

​	首先，我的整个问题的潜在的含义是——如何保证我们的栈不会压到其他的薄记成员变量上。这件事情上，就分出两个严肃的问题

- 问题1：我们的栈增长的方向是如何的？
- 问题2：我们的内存分配如何保证高低地址的先后顺序？

​	我们现在来聊的是ARM平台的场景，使用的内存分配模型是heap4.c提供的。我们先不分析heap4.c的源码，不然整个事情就没完没了了，直接说结论：为了安全，**我们需要做的是先分配栈（这样就会将栈低扔到低地址上，他咋长，栈都是在往低的地方上，不会踩到TCB的部分，事后收尸，拿到的都是一手资料，保真的！）**

```
地址高 ↑
        ┌──────────────┐
        │   pxNewTCB   │ ← 高地址，安全区域
        └──────────────┘
        │              │
        │   其他空闲    │
        │              │
        ┌──────────────┐
        │   pxStack    │ ← 栈底，SP 从这开始
        └──────────────┘
        ↓  栈增长方向（向低地址）
地址低 ↓
```

​	现在我们的进程视图就是这样的。这样就一定可以保证栈的增长不会摧毁TCB重要的数据，至少收尸的时候好收尸。如果栈是向上增长的，事情也很简单，那就是先分配TCB然后分配的是栈。反正一句话：TCB等数据放到栈相反方向的位置。

​	当然，最后的最后就是标记一下我们的任务的创建方法，笔者即支持静态创建又支持动态创建，所以， `tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE` 的整个宏定义被启用，系统会记录任务是通过动态内存方式创建的。这个标志在任务删除或系统清理时非常关键，防止释放那些是静态分配的任务资源，确保系统稳定性。

#### 初始化我们的TCB：prvInitialiseNewTask

​	初始化我们的TCB之前，我们需要仔细的看看源代码，笔者这里针对了ARM架构的特化，简化了用不到的部分（以及，我们暂时步分析带有MPU的代码，还是那句话，带上了分析不完了，我们分析基础的先）

```c
static void prvInitialiseNewTask( TaskFunction_t pxTaskCode,
                                  const char * const pcName,
                                  const configSTACK_DEPTH_TYPE uxStackDepth,
                                  void * const pvParameters,
                                  UBaseType_t uxPriority,
                                  TaskHandle_t * const pxCreatedTask,
                                  TCB_t * pxNewTCB,
                                  const MemoryRegion_t * const xRegions )
{
    StackType_t * pxTopOfStack;
    UBaseType_t x;

    /* Calculate the top of stack address.  This depends on whether the stack
     * grows from high memory to low (as per the 80x86) or vice versa.
     * portSTACK_GROWTH is used to make the result positive or negative as required
     * by the port. */
    #if ( portSTACK_GROWTH < 0 )
    {
        pxTopOfStack = &( pxNewTCB->pxStack[ uxStackDepth - ( configSTACK_DEPTH_TYPE ) 1 ] );
        pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );

        /* Check the alignment of the calculated top of stack is correct. */
        configASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0U ) );

        #if ( configRECORD_STACK_HIGH_ADDRESS == 1 )
        {
            /* Also record the stack's high address, which may assist
             * debugging. */
            pxNewTCB->pxEndOfStack = pxTopOfStack;
        }
        #endif /* configRECORD_STACK_HIGH_ADDRESS */
    }
    #else /* portSTACK_GROWTH */
    {
        pxTopOfStack = pxNewTCB->pxStack;
        pxTopOfStack = ( StackType_t * ) ( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) + portBYTE_ALIGNMENT_MASK ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );

        /* Check the alignment of the calculated top of stack is correct. */
        configASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0U ) );

        /* The other extreme of the stack space is required if stack checking is
         * performed. */
        pxNewTCB->pxEndOfStack = pxNewTCB->pxStack + ( uxStackDepth - ( configSTACK_DEPTH_TYPE ) 1 );
    }
    #endif /* portSTACK_GROWTH */

    /* Store the task name in the TCB. */
    if( pcName != NULL )
    {
        for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
        {
            pxNewTCB->pcTaskName[ x ] = pcName[ x ];

            /* Don't copy all configMAX_TASK_NAME_LEN if the string is shorter than
             * configMAX_TASK_NAME_LEN characters just in case the memory after the
             * string is not accessible (extremely unlikely). */
            if( pcName[ x ] == ( char ) 0x00 )
            {
                break;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        /* Ensure the name string is terminated in the case that the string length
         * was greater or equal to configMAX_TASK_NAME_LEN. */
        pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1U ] = '\0';
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    /* This is used as an array index so must ensure it's not too large. */
    configASSERT( uxPriority < configMAX_PRIORITIES );

    if( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
    {
        uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    pxNewTCB->uxPriority = uxPriority;
    #if ( configUSE_MUTEXES == 1 )
    {
        pxNewTCB->uxBasePriority = uxPriority;
    }
    #endif /* configUSE_MUTEXES */

    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
    vListInitialiseItem( &( pxNewTCB->xEventListItem ) );

    /* Set the pxNewTCB as a link back from the ListItem_t.  This is so we can get
     * back to  the containing TCB from a generic item in a list. */
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );

    /* Event lists are always in priority order. */
    listSET_LIST_ITEM_VALUE( &( pxNewTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxPriority );
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xEventListItem ), pxNewTCB );


    /* Initialize the TCB stack to look as if the task was already running,
     * but had been interrupted by the scheduler.  The return address is set
     * to the start of the task function. Once the stack has been initialised
     * the top of stack variable is updated. */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );

    if( pxCreatedTask != NULL )
    {
        /* Pass the handle out in an anonymous way.  The handle can be used to
         * change the created task's priority, delete the created task, etc.*/
        *pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}
```

​	首先还是老生常谈的栈增长问题。FreeRTOS 支持多种平台，有的架构栈向下增长，有的向上增长，因此栈顶地址的计算与 `portSTACK_GROWTH` 宏有关。在 `portSTACK_GROWTH < 0` 的平台（如 ARM Cortex-M），栈是向下增长的，因此初始的栈顶地址指向数组最后一个元素，并通过掩码实现字节对齐：

```c
pxTopOfStack = &( pxNewTCB->pxStack[ uxStackDepth - 1 ] );
pxTopOfStack = (StackType_t *) ( ((portPOINTER_SIZE_TYPE) pxTopOfStack) & ~( (portPOINTER_SIZE_TYPE) portBYTE_ALIGNMENT_MASK ) );
```

FreeRTOS 要求所有堆栈指针必须满足特定字节对齐要求，否则汇编上下文切换代码可能出错。对齐后，栈顶地址还会被保存到 `pxNewTCB->pxEndOfStack` 成员中，以便进行溢出检测等调试用途。对于栈向上增长的架构（如 8051），分支逻辑则不同，这里略去先！任务名是用于调试和任务识别的字符串，FreeRTOS 不要求它唯一，只需长度不超过 `configMAX_TASK_NAME_LEN`。复制时采用逐字节拷贝方式，并在越界前手动补 '\0' 以避免非法内存访问：

```c
for( x = 0; x < configMAX_TASK_NAME_LEN; x++ )
{
    pxNewTCB->pcTaskName[ x ] = pcName[ x ];
    if( pcName[ x ] == '\0' ) break;
}
pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1U ] = '\0';
```

​	为了防止用户传入非法优先级值，FreeRTOS 会将其限定在 `0 ~ configMAX_PRIORITIES - 1` 之间，并在启用互斥量优先级继承功能时记录 `uxBasePriority`：

```c
configASSERT( uxPriority < configMAX_PRIORITIES );
if( uxPriority >= configMAX_PRIORITIES )
{
    uxPriority = configMAX_PRIORITIES - 1;
}
pxNewTCB->uxPriority = uxPriority;
pxNewTCB->uxBasePriority = uxPriority;
```

​	`uxBasePriority` 在后续的互斥量实现中用于恢复原始优先级。FreeRTOS 使用链表管理所有就绪、阻塞、延迟等状态下的任务，因此每个 TCB 内部都包含两个 `ListItem_t` 类型的成员：

```c
vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
vListInitialiseItem( &( pxNewTCB->xEventListItem ) );
```

接着将当前 TCB 设置为这两个 ListItem 的“所有者”：

```c
listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
listSET_LIST_ITEM_OWNER( &( pxNewTCB->xEventListItem ), pxNewTCB );
```

其中 `xStateListItem` 用于放入就绪队列或延时队列，`xEventListItem` 用于事件同步或阻塞列表。后者还要设置值以用于按优先级排序：

```c
listSET_LIST_ITEM_VALUE( &( pxNewTCB->xEventListItem ), configMAX_PRIORITIES - uxPriority );
```

​	下面就是比较关键的一步了——栈帧构造是任务初始化中最关键的步骤。FreeRTOS 要让调度器在首次切换到该任务时能够“看起来像是从中断恢复”，于是会预先将 CPU 的寄存器、程序计数器等内容压入栈中：

```c
pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );
```

`pxPortInitialiseStack` 是一个由具体移植层实现的函数，其核心逻辑是在 `pxTopOfStack` 上模拟一次任务中断恢复场景，使调度器切换到该任务时直接运行 `pxTaskCode` 函数，并自动传入 `pvParameters` 参数。如果用户传入了 `pxCreatedTask` 指针，就把新任务的 TCB 转换为任务句柄返回：

```c
if( pxCreatedTask != NULL )
{
    *pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
}
```

#### pxPortInitialiseStack：初始化我们的栈

​	这一步，我们则是伪造的我们的创建现场来干活。

```c
/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* Simulate the stack frame as it would be created by a context switch
     * interrupt. */

    /* Offset added to account for the way the MCU uses the stack on entry/exit
     * of interrupts, and to ensure alignment. */
    pxTopOfStack--;

    *pxTopOfStack = portINITIAL_XPSR;                                    /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK; /* PC */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;             /* LR */

    /* Save code space by skipping register initialisation. */
    pxTopOfStack -= 5;                            /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */

    /* A save method is being used that requires each task to maintain its
     * own exec return value. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_EXC_RETURN;

    pxTopOfStack -= 8; /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    return pxTopOfStack;
}
```

​	布置下来，就是这样的：

```
|----------------------| ← 原始 pxTopOfStack
| 对齐保留            |
| xPSR (0x01000000)    |
| PC (任务函数地址)    |
| LR (返回地址)        |
| R12                  |
| R3                   |
| R2                   |
| R1                   |
| R0 (任务参数)        |
| EXC_RETURN           |
| R11                  |
| R10                  |
| R9                   |
| R8                   |
| R7                   |
| R6                   |
| R5                   |
| R4                   | ← 返回值（新的 pxTopOfStack）
```

ARM Cortex-M 架构在发生异常（如 SVC、PendSV 等）时，硬件会自动将一组寄存器压入当前任务的栈中。这组寄存器包括 xPSR、PC、LR、R12、R3、R2、R1、R0。FreeRTOS 正是借用了这一机制：在任务刚被创建时，它会手动将这些值写入任务的栈上，使调度器在首次调度到该任务时可以通过异常返回机制恢复这段上下文，并让处理器自动跳转执行任务函数。这个想法巧妙地避免了复杂的初始化跳转逻辑，极大地利用了硬件的自动上下文恢复能力。

函数在开始时对 pxTopOfStack 进行一次减法操作，这是因为 Cortex-M 的栈是向下增长的，而所有的数据压栈操作都需要预留空间。这一步也常常被用来确保栈的对齐要求被满足。接着，它在栈顶首先写入一个初始的 xPSR 值，通常是 0x01000000，这个值开启了 Thumb 状态。Cortex-M 处理器不支持 ARM 模式，只有 Thumb 模式，因此这是一个必要设置。随后写入的是程序计数器 PC，其值为任务函数地址。这一地址将作为任务首次恢复时的执行起点。然后写入链接寄存器 LR，其值通常指向一个任务返回处理函数，这样一旦任务函数执行完毕并“返回”，控制流会跳转到一个死循环或者触发断言，以防止任务意外退出。

再往下是寄存器 R12、R3、R2、R1 的空间，这些可以略过，因为 Cortex-M 会在任务恢复时自动覆盖它们。这里唯一需要显式设置的是 R0，它被用于传递参数。因此 R0 被写入用户传递进来的任务参数指针。继续向下栈写入的是一个特殊值 EXC_RETURN，用于指示异常返回时使用哪种堆栈模式以及哪些寄存器需要被恢复。这个值常常是 0xFFFFFFFD，表示在异常返回时恢复到线程模式，使用 PSP，并返回基本帧结构。最后还需要为另外八个寄存器预留空间，即 R4 到 R11。这些寄存器虽然不会在异常时被硬件自动保存，但在 FreeRTOS 的任务切换代码中，它们会被手动保存和恢复，因此在栈中也必须预留出相应空间。

至此，一个完整的任务初始化栈帧就构建完毕了。函数最终返回的是新的栈顶位置，这个位置会被写入到任务控制块中的 pxTopOfStack 成员里，供调度器后续使用。值得注意的是，整个函数并不会涉及实际的上下文切换，仅仅是做好了“伪装”，让任务在第一次被切换入时表现得像是“曾经运行过”，从而让调度机制无缝运转。

到这里，我们的任务就正儿八经的谈完了！

## xTaskCreateStatic，相似的处理过程

​	如果你大致理解了上面的内容，那么下面的静态创建，无非就是换成了我们用户提供的内容而已。

```c
    TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                    const char * const pcName,
                                    const configSTACK_DEPTH_TYPE uxStackDepth,
                                    void * const pvParameters,
                                    UBaseType_t uxPriority,
                                    StackType_t * const puxStackBuffer,
                                    StaticTask_t * const pxTaskBuffer )
    {
        TaskHandle_t xReturn = NULL;
        TCB_t * pxNewTCB;

        traceENTER_xTaskCreateStatic( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );

        pxNewTCB = prvCreateStaticTask( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer, &xReturn );

        if( pxNewTCB != NULL )
        {
            prvAddNewTaskToReadyList( pxNewTCB );
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();	// 注意这里是空的
        }
        traceRETURN_xTaskCreateStatic( xReturn );
        return xReturn;
    }
```

​	这里的traceENTER_xTaskCreateStatic和traceRETURN_xTaskCreateStatic单纯的就是准备记录和追踪发生的情况，也没有啥特殊的含义。

​	`prvCreateStaticTask` 实际负责任务控制块（TCB）的创建和初始化工作，确保任务能够正确地与提供的栈缓冲区和 TCB 缓冲区进行绑定。如果任务创建成功，`pxNewTCB` 将被赋值为任务控制块的指针。任务控制块被传递到 `prvAddNewTaskToReadyList`，该函数将新任务添加到就绪任务列表中，意味着该任务已经准备好执行，并等待调度器的调度。此时，任务已经完成了所有初始化工作，可以开始在系统中进行调度。如果任务控制块创建失败，函数则会调用 `mtCOVERAGE_TEST_MARKER()`，通常这行代码是用来标记覆盖率，指示函数未能成功创建任务。这样就完成了静态创建的前端调用

## 撤销任务：vTaskDelete

```c
    void vTaskDelete( TaskHandle_t xTaskToDelete )
    {
        TCB_t * pxTCB;
        BaseType_t xDeleteTCBInIdleTask = pdFALSE;
        BaseType_t xTaskIsRunningOrYielding;

        traceENTER_vTaskDelete( xTaskToDelete );

        taskENTER_CRITICAL();
        {
            /* If null is passed in here then it is the calling task that is
             * being deleted. */
            pxTCB = prvGetTCBFromHandle( xTaskToDelete );

            /* Remove task from the ready/delayed list. */
            if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
            {
                taskRESET_READY_PRIORITY( pxTCB->uxPriority );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            /* Is the task waiting on an event also? */
            if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
            {
                ( void ) uxListRemove( &( pxTCB->xEventListItem ) );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            /* Increment the uxTaskNumber also so kernel aware debuggers can
             * detect that the task lists need re-generating.  This is done before
             * portPRE_TASK_DELETE_HOOK() as in the Windows port that macro will
             * not return. */
            uxTaskNumber++;

            /* Use temp variable as distinct sequence points for reading volatile
             * variables prior to a logical operator to ensure compliance with
             * MISRA C 2012 Rule 13.5. */
            xTaskIsRunningOrYielding = taskTASK_IS_RUNNING_OR_SCHEDULED_TO_YIELD( pxTCB );

            /* If the task is running (or yielding), we must add it to the
             * termination list so that an idle task can delete it when it is
             * no longer running. */
            if( ( xSchedulerRunning != pdFALSE ) && ( xTaskIsRunningOrYielding != pdFALSE ) )
            {
                /* A running task or a task which is scheduled to yield is being
                 * deleted. This cannot complete when the task is still running
                 * on a core, as a context switch to another task is required.
                 * Place the task in the termination list. The idle task will check
                 * the termination list and free up any memory allocated by the
                 * scheduler for the TCB and stack of the deleted task. */
                vListInsertEnd( &xTasksWaitingTermination, &( pxTCB->xStateListItem ) );

                /* Increment the ucTasksDeleted variable so the idle task knows
                 * there is a task that has been deleted and that it should therefore
                 * check the xTasksWaitingTermination list. */
                ++uxDeletedTasksWaitingCleanUp;

                /* Call the delete hook before portPRE_TASK_DELETE_HOOK() as
                 * portPRE_TASK_DELETE_HOOK() does not return in the Win32 port. */
                traceTASK_DELETE( pxTCB );

                /* Delete the task TCB in idle task. */
                xDeleteTCBInIdleTask = pdTRUE;

                /* The pre-delete hook is primarily for the Windows simulator,
                 * in which Windows specific clean up operations are performed,
                 * after which it is not possible to yield away from this task -
                 * hence xYieldPending is used to latch that a context switch is
                 * required. */
                #if ( configNUMBER_OF_CORES == 1 )
                    portPRE_TASK_DELETE_HOOK( pxTCB, &( xYieldPendings[ 0 ] ) );
                #else
                    portPRE_TASK_DELETE_HOOK( pxTCB, &( xYieldPendings[ pxTCB->xTaskRunState ] ) );
                #endif

                /* In the case of SMP, it is possible that the task being deleted
                 * is running on another core. We must evict the task before
                 * exiting the critical section to ensure that the task cannot
                 * take an action which puts it back on ready/state/event list,
                 * thereby nullifying the delete operation. Once evicted, the
                 * task won't be scheduled ever as it will no longer be on the
                 * ready list. */
                #if ( configNUMBER_OF_CORES > 1 )
                {
                    if( taskTASK_IS_RUNNING( pxTCB ) == pdTRUE )
                    {
                        if( pxTCB->xTaskRunState == ( BaseType_t ) portGET_CORE_ID() )
                        {
                            configASSERT( uxSchedulerSuspended == 0 );
                            taskYIELD_WITHIN_API();
                        }
                        else
                        {
                            prvYieldCore( pxTCB->xTaskRunState );
                        }
                    }
                }
                #endif /* #if ( configNUMBER_OF_CORES > 1 ) */
            }
            else
            {
                --uxCurrentNumberOfTasks;
                traceTASK_DELETE( pxTCB );

                /* Reset the next expected unblock time in case it referred to
                 * the task that has just been deleted. */
                prvResetNextTaskUnblockTime();
            }
        }
        taskEXIT_CRITICAL();

        /* If the task is not deleting itself, call prvDeleteTCB from outside of
         * critical section. If a task deletes itself, prvDeleteTCB is called
         * from prvCheckTasksWaitingTermination which is called from Idle task. */
        if( xDeleteTCBInIdleTask != pdTRUE )
        {
            prvDeleteTCB( pxTCB );
        }

        /* Force a reschedule if it is the currently running task that has just
         * been deleted. */
        #if ( configNUMBER_OF_CORES == 1 )
        {
            if( xSchedulerRunning != pdFALSE )
            {
                if( pxTCB == pxCurrentTCB )
                {
                    configASSERT( uxSchedulerSuspended == 0 );
                    taskYIELD_WITHIN_API();
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        }
        #endif /* #if ( configNUMBER_OF_CORES == 1 ) */

        traceRETURN_vTaskDelete();
    }
```

​	vTaskDelete函数薄记完进入信息后就会进入临界区，确保任务删除过程中不发生中断。然后，它根据传入的任务句柄（`xTaskToDelete`）获取对应的任务控制块（TCB）。如果 `xTaskToDelete` 为 `NULL`，则意味着当前任务正在删除自己，因此它会根据当前任务句柄获取对应的 TCB。一旦获取到任务控制块，删除操作就开始。首先，任务会被从就绪或延迟列表中移除。这是通过 `uxListRemove` 完成的。如果任务在就绪列表中，移除后会重置其优先级状态；如果任务在其他列表中，删除操作会跳过。接着，任务是否在事件列表中等待也会进行检查，如果任务正在等待事件，它也会被从事件列表中移除。

​	然后，`uxTaskNumber` 变量会递增，这样调试器就可以识别出任务列表需要重新生成。`xTaskIsRunningOrYielding` 变量用来检查任务是否正在运行或者准备切换任务。如果任务正在运行或即将运行，那么它将被添加到一个等待终止的任务列表中，等待空闲任务来清理其资源。此时，删除任务的内存管理工作不会立刻进行，而是交由空闲任务在空闲时处理。

​	在处理任务删除操作时，删除钩子（`traceTASK_DELETE`）会被调用，允许用户或调试工具插入特定的清理逻辑。然后，函数会决定是否在空闲任务中删除任务控制块（TCB）。如果任务是在运行状态，它会被标记为待删除状态，等待系统空闲时进行资源清理。对于单核系统，删除操作会尽量避免在删除任务时进行任务切换，而多核系统则会更加复杂，因为任务可能正在另一个核心上运行。如果任务不在运行中，且删除的任务并非当前任务，那么它会直接调用 `prvDeleteTCB` 函数删除任务的 TCB 和栈内存。如果当前任务被删除，则会通过强制上下文切换来重新调度任务，确保系统继续正常运行。



## 附录

### 回答问题：为什么要有任务名称？

​	首先，方便调试排查，你也不想对着一个TaskHandle发呆，然后找Map文件看这个TaskHandle（如果是一个全局的）在哪里是谁吧！至于动态的局部变量那可就更麻烦了，你甚至不知道这个是啥！一个简单的标记就能让我们的排查变得非常的轻松！

​	咱们之后调试的时候使能串口，这个任务名称也能方便的给我们的日志带上更详细的信息：

```
[xxxTask]:	message from task: xxx, ...
```

​	这样看着是不是好多了？另外，我们的系统记录某个任务的启动、停止或阻塞时，任务名称可以清晰地标识出是哪个任务，而不是仅仅记录一个任务句柄或优先级。

​	另外，我们自己写代码的时候，也好方便看我们的Handle的含义（虽然笔者更习惯的是在变量名就说明白这个事情）