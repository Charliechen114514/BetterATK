/**
 * @file system_clock.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SYSCLK_H
#define SYSCLK_H
#include <stm32f1xx_hal.h>

/**
 * @brief   DEFAULT_RCC_PLL_VALUE promised the default CLK, 
 *          for RCC_PLL_MUL9, it's 72MHz
 */
#define DEFAULT_RCC_PLL_VALUE           (RCC_PLL_MUL9)

/**
 * @brief   this is a simple clock initializations
 *          As default, we use the 
 */
void system_clock_init();

/**
 * @brief delay the global system for given miliseconds
 * 
 * @param delay_ms 
 */
void system_delay_ms(uint32_t delay_ms);

/**
 * @brief delay the global system for given museconds
 * 
 * @param us 
 */
void system_delay_us(uint32_t us);

/**
 * @brief  provide the current frequency of system clock 
 * 
 * @return uint32_t , the raw Systicks Frequency
 */
uint32_t provide_application_sysfreq();

#endif