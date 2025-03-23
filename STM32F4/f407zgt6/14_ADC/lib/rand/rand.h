#ifndef RAND_H
#define RAND_H

#include "stm32f4xx_hal.h"

#define RAND_INIT_FAILED        (1)
#define RAND_INIT_SUCCESS       (0)


uint8_t     rand_init();
uint32_t    fetch_rand_res();
uint32_t    fetch_rand_inrange(uint32_t min, uint32_t max);

#endif