#ifndef WATCH_DOG_H
#define WATCH_DOG_H

#include "stm32f4xx_hal.h"

typedef struct {
    IWDG_HandleTypeDef handle;
}WatchDogHandler;

void init_watchdog_handler(
    WatchDogHandler*        handler, 
    IWDG_TypeDef*           raw_handle,
    uint8_t                 prescalar,
    uint16_t                reload_value);

void feed_watchdog(WatchDogHandler* handler);


#endif