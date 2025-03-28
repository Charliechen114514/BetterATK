# FreeRTOS的任务管理抽象2——源码级分析任务的挂起和恢复

​	我们这一篇博客的目的是理解FreeRTOS的挂起和恢复。这里，我们说的挂起指的是将我们的任务状态切换到挂起态。这个时候，我们的任务就会放弃竞争CPU，成为一个佛系的，不干活挂死的任务，这个挂死是我们主动让他挂起来的，啥时候恢复是我们说的算的。

​	FreeRTOS的任务挂起和恢复函数，就是两个：

```c
// 任务的挂起
void vTaskSuspend( TaskHandle_t xTaskToSuspend ) PRIVILEGED_FUNCTION;
// 任务的恢复
void vTaskResume( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;
```

​	可以看到，我们只需要传递任务句柄就好了，这就说明了我们要把哪一个任务挂起来，哪一个任务恢复就绪。

## vTaskSuspend：挂起任务，先别干活！

> The `vTaskSuspend()` function in FreeRTOS allows suspending any task, preventing it from receiving microcontroller processing time regardless of its priority. For this function to be available, `INCLUDE_vTaskSuspend` must be set to 1 in the FreeRTOS configuration.
>
> When a task is suspended, it remains in that state until explicitly resumed using `vTaskResume()`. Suspending a task is not cumulative, meaning that multiple calls to `vTaskSuspend()` on the same task still require only a single call to `vTaskResume()` to reactivate it.
>
> The function takes a single parameter, `xTaskToSuspend`, which is the handle of the task to be suspended. If `NULL` is passed, the function will suspend the calling task.
>
> FreeRTOS 中的 `vTaskSuspend()` 函数允许暂停任何任务，从而阻止其接收微控制器处理时间，无论其优先级如何。要使用此功能，必须在 FreeRTOS 配置中将 `INCLUDE_vTaskSuspend` 设置为 1。
>
> 当任务被暂停时，它会保持该状态，直到使用 `vTaskResume()` 明确恢复。暂停任务不是累积的，这意味着对同一任务多次调用 `vTaskSuspend()` 仍然只需要一次调用 `vTaskResume()` 即可重新激活它。
>
> 该函数采用单个参数 `xTaskToSuspend`，它是要暂停的任务的句柄。如果传递了 `NULL`，该函数将暂停调用任务。

​	这个需要注意人家说的——FreeRTOS的挂起不是累计的，一个vTaskResume就能让他重新下去干活，当然，想要累计也很简单，自己薄记一个变量嘛！

​	还是一样，传递NULL可以挂起他自己。但是注意，这就的保证别人可以找到你，把你再恢复下来，不然任务就会死在那里！

## vTaskResume:任务的恢复，滚去干活！

> The `vTaskResume()` function in FreeRTOS resumes a task that was previously suspended using `vTaskSuspend()`. For this function to be available, `INCLUDE_vTaskSuspend` must be set to 1 in the FreeRTOS configuration.
>
> When a task is suspended, it remains inactive until explicitly resumed. A single call to `vTaskResume()` is sufficient to make the task available for execution again, even if it was suspended multiple times. The function takes one parameter, `xTaskToResume`, which is the handle of the task that should be resumed.
>
> FreeRTOS 中的 `vTaskResume()` 函数可恢复之前使用 `vTaskSuspend()` 暂停的任务。要使用此功能，必须在 FreeRTOS 配置中将 `INCLUDE_vTaskSuspend` 设置为 1。
>
> 任务暂停后，将保持非活动状态，直到明确恢复。一次调用 `vTaskResume()` 即可使任务再次可执行，即使该任务被暂停多次。该函数采用一个参数 `xTaskToResume`，它是应恢复的任务的句柄。

​	这个没啥好说的。

## 但是一个特殊的是...

​	如果你看FreeRTOS的源码，你会发现一个新的vTaskResume，叫：xTaskResumeFromISR

- **`xTaskResumeFromISR`**：用于在中断服务例程（ISR）中恢复被挂起的任务。当任务恢复时，它可能会导致任务切换（如果需要）。由于 ISR 的特殊性，`xTaskResumeFromISR` 是设计成适应中断上下文的。
- **`xTaskResume`**：用于在正常的任务上下文中恢复被挂起的任务。它是一个 API 函数，允许从任务的执行代码中恢复挂起的任务，而不涉及 ISR。

​	也就是说，如果我们需要在中断的时候发生任务恢复，就是用xTaskResumeFromISR，反之xTaskResume就足够。值得注意的是，`xTaskResumeFromISR` 会通过返回值 `xYieldRequired` 来指示是否需要进行任务切换。如果恢复的任务优先级更高，可能需要触发上下文切换。返回值 `xYieldRequired` 被用来标识是否需要在 ISR 后立即执行任务切换。所以一个标准的写法是：

```
BaseType_t	required_yield;
required_yield = xTaskResumeFromISR(anyHandler);
if(required_yield == pdTRUE)
{
	portYIELD_FROM_ISR(required_yield);
}
```

​	portYIELD_FROM_ISR是一个保证在中断服务函数中安全的进行任务调度的函数。



## 源码分析

### vTaskSuspend

```c
    void vTaskSuspend( TaskHandle_t xTaskToSuspend )
    {
        TCB_t * pxTCB;

        traceENTER_vTaskSuspend( xTaskToSuspend );
		// 临界区操作
        taskENTER_CRITICAL();
        {
            /* If null is passed in here then it is the running task that is
             * being suspended. */
            pxTCB = prvGetTCBFromHandle( xTaskToSuspend );

            traceTASK_SUSPEND( pxTCB );

            /* Remove task from the ready/delayed list and place in the
             * suspended list. */
            // 下面的代码让我们的任务从其他状态中被移除出来
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

            vListInsertEnd( &xSuspendedTaskList, &( pxTCB->xStateListItem ) );

            #if ( configUSE_TASK_NOTIFICATIONS == 1 )
            {
                BaseType_t x;

                for( x = ( BaseType_t ) 0; x < ( BaseType_t ) configTASK_NOTIFICATION_ARRAY_ENTRIES; x++ )
                {
                    if( pxTCB->ucNotifyState[ x ] == taskWAITING_NOTIFICATION )
                    {
                        /* The task was blocked to wait for a notification, but is
                         * now suspended, so no notification was received. */
                        pxTCB->ucNotifyState[ x ] = taskNOT_WAITING_NOTIFICATION;
                    }
                }
            }
            #endif /* if ( configUSE_TASK_NOTIFICATIONS == 1 ) */

            /* In the case of SMP, it is possible that the task being suspended
             * is running on another core. We must evict the task before
             * exiting the critical section to ensure that the task cannot
             * take an action which puts it back on ready/state/event list,
             * thereby nullifying the suspend operation. Once evicted, the
             * task won't be scheduled before it is resumed as it will no longer
             * be on the ready list. */
			// SMP的部分我们不分析，对于大部分单片机，都是单核的多

        #if ( configNUMBER_OF_CORES == 1 )
        {
            UBaseType_t uxCurrentListLength;

            if( xSchedulerRunning != pdFALSE )
            {
                /* Reset the next expected unblock time in case it referred to the
                 * task that is now in the Suspended state. */
                taskENTER_CRITICAL();
                {
                    // 防止调度器错误地使用即将被挂起任务的解除阻塞时间。
                    prvResetNextTaskUnblockTime();
                }
                taskEXIT_CRITICAL();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            if( pxTCB == pxCurrentTCB )
            {
                if( xSchedulerRunning != pdFALSE )
                {
                    /* The current task has just been suspended. */
                    // 当挂起的任务是当前正在运行的任务时，函数会强制执行任务切换
                    configASSERT( uxSchedulerSuspended == 0 );
                    portYIELD_WITHIN_API();
                }
                else
                {
                    /* The scheduler is not running, but the task that was pointed
                     * to by pxCurrentTCB has just been suspended and pxCurrentTCB
                     * must be adjusted to point to a different task. */

                    /* Use a temp variable as a distinct sequence point for reading
                     * volatile variables prior to a comparison to ensure compliance
                     * with MISRA C 2012 Rule 13.2. */
                    uxCurrentListLength = listCURRENT_LIST_LENGTH( &xSuspendedTaskList );

                    if( uxCurrentListLength == uxCurrentNumberOfTasks )
                    {
                        // 这里全部挂起了，我们设置为NULL, pxCurrentTCB实际上标记了当前正在执行的任务，
                        // 他当然是一个全局变量，不难猜测对于SMP，这是一个数组
                        /* No other tasks are ready, so set pxCurrentTCB back to
                         * NULL so when the next task is created pxCurrentTCB will
                         * be set to point to it no matter what its relative priority
                         * is. */
                        pxCurrentTCB = NULL;
                    }
                    else
                    {
                        // 
                        vTaskSwitchContext();
                    }
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        #endif /* #if ( configNUMBER_OF_CORES == 1 ) */

        traceRETURN_vTaskSuspend();
    }
```

​	首先，函数使用 `prvGetTCBFromHandle` 获取任务的 TCB。如果 `xTaskToSuspend` 为空，则默认挂起当前任务。接着，它会调用 `traceTASK_SUSPEND` 进行额外的跟踪。

​	任务被挂起的第一步是将其从就绪或延迟列表中移除。`uxListRemove` 负责这一操作，并在任务属于就绪队列时重置优先级状态。若任务在事件列表中等待某些事件，则它也会被从事件列表移除，以确保任务不会因为等待事件而重新进入调度状态。然后，任务被插入到 `xSuspendedTaskList`，表示其处于挂起状态。**若 FreeRTOS 启用了任务通知（`configUSE_TASK_NOTIFICATIONS == 1`），则函数会遍历任务的通知数组，并将任何处于等待状态的通知标记为未等待状态，避免任务在挂起后仍错误地保持等待状态。**

​	我们直接分析一个单核的系统（`configNUMBER_OF_CORES == 1`），函数会在适当情况下调用 `prvResetNextTaskUnblockTime`，防止调度器错误地使用即将被挂起任务的解除阻塞时间。

​	当挂起的任务是当前正在运行的任务时，函数会强制执行任务切换（`portYIELD_WITHIN_API`）。如果调度器尚未运行，则 `pxCurrentTCB` 可能需要调整，以确保调度器能够正确找到新的当前任务。

​	这就是一个标准的挂起任务的流程。

- 找到进程
- 先移除出所在的状态队列（一个任务只可能有一个状态，直接移除所在的队列就完事了（断开链表指针））
- 然后将所有的等待事件和其他资源请求全部清掉，表达自己无欲无求，成为一个佛系的进程
- 最后就是让我们的调度器调度到正确的进程上，特别的，对于全挂起来了，那就指向空，这样其他子系统就明白了当前所有的进程都挂起了。

### vTaskResume

```c
    void vTaskResume( TaskHandle_t xTaskToResume )
    {
        TCB_t * const pxTCB = xTaskToResume;

        traceENTER_vTaskResume( xTaskToResume );

        /* It does not make sense to resume the calling task. */
        configASSERT( xTaskToResume );

        #if ( configNUMBER_OF_CORES == 1 )

            /* The parameter cannot be NULL as it is impossible to resume the
             * currently executing task. */
        	// 确实👆 
            if( ( pxTCB != pxCurrentTCB ) && ( pxTCB != NULL ) )
        #else

            /* The parameter cannot be NULL as it is impossible to resume the
             * currently executing task. It is also impossible to resume a task
             * that is actively running on another core but it is not safe
             * to check their run state here. Therefore, we get into a critical
             * section and check if the task is actually suspended or not. */
            if( pxTCB != NULL )
        #endif
        {
            taskENTER_CRITICAL();
            {
                // 如果当前的进程就是挂起的
                if( prvTaskIsTaskSuspended( pxTCB ) != pdFALSE )
                {
                    traceTASK_RESUME( pxTCB );

                    /* The ready list can be accessed even if the scheduler is
                     * suspended because this is inside a critical section. */
                    // 无所谓，现在全局中断被关闭了，可以安全的移除
                    ( void ) uxListRemove( &( pxTCB->xStateListItem ) );
                    // 加入就绪队列
                    prvAddTaskToReadyList( pxTCB );

                    /* This yield may not cause the task just resumed to run,
                     * but will leave the lists in the correct state for the
                     * next yield. */
                    taskYIELD_ANY_CORE_IF_USING_PREEMPTION( pxTCB );
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            taskEXIT_CRITICAL();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_vTaskResume();
    }
```

​	在单核系统中，恢复的任务不能是当前运行的任务，因此需要额外检查 `pxTCB` 是否等于 `pxCurrentTCB`。而在多核系统中，由于无法直接判断任务是否正在其他核心上运行，因此会先进入临界区，再检查任务是否确实处于挂起状态，以保证操作的安全性。进入临界区后，若 `pxTCB` 处于挂起状态，则从 `xSuspendedTaskList` 中移除该任务，并将其加入就绪列表。随后，执行 `taskYIELD_ANY_CORE_IF_USING_PREEMPTION`，该操作不会立即让恢复的任务运行，但会确保任务列表处于正确状态，以便在下次调度时执行适当的任务切换。如果任务未处于挂起状态，则执行 `mtCOVERAGE_TEST_MARKER()`，用于测试覆盖情况，以确保系统行为符合预期。

### vTaskResumeFromISR

```c
    BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume )
    {
        BaseType_t xYieldRequired = pdFALSE;
        TCB_t * const pxTCB = xTaskToResume;
        UBaseType_t uxSavedInterruptStatus;

        traceENTER_xTaskResumeFromISR( xTaskToResume );

        configASSERT( xTaskToResume );

        /* RTOS ports that support interrupt nesting have the concept of a
         * maximum  system call (or maximum API call) interrupt priority.
         * Interrupts that are  above the maximum system call priority are keep
         * permanently enabled, even when the RTOS kernel is in a critical section,
         * but cannot make any calls to FreeRTOS API functions.  If configASSERT()
         * is defined in FreeRTOSConfig.h then
         * portASSERT_IF_INTERRUPT_PRIORITY_INVALID() will result in an assertion
         * failure if a FreeRTOS API function is called from an interrupt that has
         * been assigned a priority above the configured maximum system call
         * priority.  Only FreeRTOS functions that end in FromISR can be called
         * from interrupts  that have been assigned a priority at or (logically)
         * below the maximum system call interrupt priority.  FreeRTOS maintains a
         * separate interrupt safe API to ensure interrupt entry is as fast and as
         * simple as possible.  More information (albeit Cortex-M specific) is
         * provided on the following link:
         * https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
        // 这里下断言——是不是合法的中断,这是为了防止高级别的中断打断了我们系统底层的中断，毕竟任务切换了
        // 这就必须保证这个中断不可以被嵌套，否则会死的很惨
        portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

        /* MISRA Ref 4.7.1 [Return value shall be checked] */
        /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#dir-47 */
        /* coverity[misra_c_2012_directive_4_7_violation] */
        uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
        {
            if( prvTaskIsTaskSuspended( pxTCB ) != pdFALSE )
            {
                traceTASK_RESUME_FROM_ISR( pxTCB );

                /* Check the ready lists can be accessed. */
                if( uxSchedulerSuspended == ( UBaseType_t ) 0U )
                {
                    // 这一部分的裸机没啥好说的，跟上面的一样
                    #if ( configNUMBER_OF_CORES == 1 )
                    {
                        /* Ready lists can be accessed so move the task from the
                         * suspended list to the ready list directly. */
                        if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                        {
                            xYieldRequired = pdTRUE;

                            /* Mark that a yield is pending in case the user is not
                             * using the return value to initiate a context switch
                             * from the ISR using the port specific portYIELD_FROM_ISR(). */
                            xYieldPendings[ 0 ] = pdTRUE;
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                    #endif /* #if ( configNUMBER_OF_CORES == 1 ) */

                    ( void ) uxListRemove( &( pxTCB->xStateListItem ) );
                    prvAddTaskToReadyList( pxTCB );
                }
                else
                {
                    /* The delayed or ready lists cannot be accessed so the task
                     * is held in the pending ready list until the scheduler is
                     * unsuspended. */
                    vListInsertEnd( &( xPendingReadyList ), &( pxTCB->xEventListItem ) );
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

        traceRETURN_xTaskResumeFromISR( xYieldRequired );

        return xYieldRequired;
    }
```

​	我们的第一步是 `configASSERT( xTaskToResume )`，用于确保传入的任务句柄有效，防止空指针错误。接着调用 `portASSERT_IF_INTERRUPT_PRIORITY_INVALID()`，检查中断优先级是否合法，以防止在不允许的高优先级中断中调用 FreeRTOS API。

​	进入临界区后，函数首先检查 `pxTCB` 是否处于挂起状态。如果任务确实被挂起，则 `traceTASK_RESUME_FROM_ISR` 记录恢复操作。然后，函数判断调度器是否被挂起，即 `uxSchedulerSuspended` 是否为零。如果调度器没有被挂起，表示任务列表可访问。在单核系统中，如果恢复的任务优先级高于当前运行任务的优先级，则需要触发任务切换，因此 `xYieldRequired` 被置为 `pdTRUE`，并标记 `xYieldPendings[0]` 以确保稍后进行任务切换。接着，从挂起列表中移除该任务，并将其加入就绪列表。如果调度器处于挂起状态，则不能直接修改就绪列表，因此任务会被插入 `xPendingReadyList`，等待调度器恢复后再进行处理。

​	问题来了，为什么要这样做，我们为什么不可以直接梭哈就序列表呢？这是因为：**调度器挂起意味着系统进入一个不稳定或受控的状态，任务的切换被暂时暂停。**在这种情况下，修改就绪列表可能会破坏任务恢复的顺序，导致任务错乱。通过推迟任务的状态修改，直到调度器恢复，我们可以确保系统的一致性和稳定性。如果一个任务的状态在调度器挂起时被修改，可能会发生竞态条件。即，任务的状态可能与调度器的状态不一致，这会导致调度的错误行为。

​	调度器挂起不意外着我们的其他的修改没有发生，如果我们的其他进程正在提交事务的时候，发生了这个中断，这个时候，我们的就绪队列很可能就在修改，我们直接把一个旧世界的任务塞进去，你知道的。很可能会触发错误的运行（举个例子，之前挂起前是老大，现在不是了，你直接塞到了就绪队列让他运行，本来他应该在低优先级的阻塞，这下整个调度就乱掉了！）也就是说，调度器这边存储的任务的状态和任务自身的状态不一致，很有可能造成错误的assert，把系统毙掉了。所以我们就先插入到一个悬起的xPendingReadyList，这个队列将会在我们调度器恢复的时候，插入到我们的就绪队列里，这样整个系统就会防止竞争了