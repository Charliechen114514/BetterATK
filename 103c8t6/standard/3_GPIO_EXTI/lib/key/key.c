/**
 * @file key.c
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2025-04-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "key.h"
#include "system_clock.h"
void init_bsp_key(KeyHandle* handle, OpenClock clk_func)
{
    GPIO_InitTypeDef initer;
    clk_func(&handle->type);
    initer.GPIO_Speed = GPIO_Speed_50MHz;
    initer.GPIO_Mode = GPIO_Mode_IPU;
    initer.GPIO_Pin = handle->type.pin;
    GPIO_Init(handle->type.port, &initer);
}

uint8_t isKeyPressed(KeyHandle* handle){
    if(handle->func_detect(handle)){
        system_delay_ms(KEY_DELAY);
        return 1;
    }
    return 0;
}



