#include <memory.h>
#include "watchdog.h"

WindowWatchDogHandler*      pHandler[MAX_WWDOG_MEMORIZE];
uint8_t                     current_wwatchdog_boarding;

void init_watchdog_handler(
    WatchDogHandler*        handler, 
    IWDG_TypeDef*           raw_handle,
    uint8_t                 prescalar,
    uint16_t                reload_value)
{
    memset(handler, 0, sizeof(WatchDogHandler));
    handler->handle.Init.Prescaler = prescalar;
    handler->handle.Init.Reload = reload_value;
    handler->handle.Instance = raw_handle;
    HAL_IWDG_Init(&handler->handle);
}

void feed_watchdog(WatchDogHandler* handler)
{
    HAL_IWDG_Refresh(&handler->handle);
}

void init_window_watchdog_handler(
    WindowWatchDogHandler*  handler,
    WWDG_TypeDef*           raw_handle,
    uint32_t                prescalar,
    uint16_t                counter,
    uint16_t                window_counter,
    EarlyCallback           callback)
{
    memset(handler, 0, sizeof(WindowWatchDogHandler));
    handler->handle.Instance = raw_handle;
    handler->handle.Init.Counter = counter;
    handler->handle.Init.Window = window_counter;
    handler->handle.Init.EWIMode = WWDG_EWI_ENABLE;
    handler->handle.Init.Prescaler = prescalar;
    handler->callback = callback;
    HAL_WWDG_Init(&handler->handle);
    pHandler[current_wwatchdog_boarding] = handler;
    current_wwatchdog_boarding++;
}

