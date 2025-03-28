#ifndef WATCH_DOG_H
#define WATCH_DOG_H

#include "stm32f4xx_hal.h"
#define MAX_WWDOG_MEMORIZE      (1)
typedef struct {
    IWDG_HandleTypeDef  handle;
}WatchDogHandler;
typedef struct _wwdog_handler WindowWatchDogHandler;
typedef void(*EarlyCallback)(WindowWatchDogHandler* raw);

typedef struct _wwdog_handler{
    WWDG_HandleTypeDef  handle;
    EarlyCallback       callback;
}WindowWatchDogHandler;

void init_watchdog_handler(
    WatchDogHandler*        handler, 
    IWDG_TypeDef*           raw_handle,
    uint8_t                 prescalar,
    uint16_t                reload_value);

void feed_watchdog(WatchDogHandler* handler);

void init_window_watchdog_handler(
    WindowWatchDogHandler*  handle,
    WWDG_TypeDef*           raw_handle,
    uint32_t                prescalar,
    uint16_t                counter,
    uint16_t                window_counter,
    EarlyCallback           callback
);

#endif