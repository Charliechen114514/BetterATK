#include <memory.h>
#include "watchdog.h"

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