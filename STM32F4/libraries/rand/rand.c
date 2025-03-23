#include "rand.h"
#include "system_clock.h"
RNG_HandleTypeDef RNG_Handler;

uint8_t rand_init()
{
    uint16_t retry = 0;

    RNG_Handler.Instance = RNG;
    HAL_RNG_Init(&RNG_Handler);                                                       // 初始化RNG
    while (__HAL_RNG_GET_FLAG(&RNG_Handler, RNG_FLAG_DRDY) == RESET && retry < 10000) // 等待RNG准备就绪
    {
        retry++;
        system_delay_us(10);
    }
    if (retry >= 10000)
        return 1;
    return 0;
}

uint32_t fetch_rand_res()
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler);
}

uint32_t fetch_rand_inrange(uint32_t min, uint32_t max)
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler) % (max - min + 1) + min;
}