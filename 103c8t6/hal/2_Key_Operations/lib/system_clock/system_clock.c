/**
 * @file system_clock.c
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "system_clock.h"

static void inline __clk_bad_init_hook()
{
    while (1)
        ;
}

/**
 * @brief  provide the current frequency of system clock 
 * 
 * @return uint32_t , the raw Systicks Frequency
 */
uint32_t provide_application_sysfreq()
{
    return HAL_RCC_GetSysClockFreq();
}

/**
 * @brief delay the global system for given museconds
 * 
 * @param us 
 */
void system_delay_us(uint32_t us)
{
    uint64_t ticks;                                     // Total number of ticks required for the delay
    uint32_t prevValue, currentValue, elapsedTicks = 0; // Variables for SysTick counter values and elapsed ticks
    uint32_t reloadValue = SysTick->LOAD;               // Value of the SysTick reload register
    uint32_t sysclk_freq = HAL_RCC_GetSysClockFreq() / 1000000L;
    ticks = us * sysclk_freq; // Calculate the number of ticks needed for the desired delay in microseconds

    prevValue = SysTick->VAL; // Store the initial SysTick counter value

    while (1)
    {
        currentValue = SysTick->VAL; // Get the current SysTick counter value
        if (currentValue != prevValue)
        {
            if (currentValue < prevValue)
            {
                elapsedTicks += prevValue - currentValue; // Count the elapsed ticks (SysTick is a decrementing counter)
            }
            else
            {
                elapsedTicks += reloadValue - currentValue + prevValue; // Handle the rollover case
            }
            prevValue = currentValue; // Update the previous value to the current value

            if (elapsedTicks >= ticks) // If the elapsed time is greater than or equal to the requested delay
            {
                break; // Exit the loop to complete the delay
            }
        }
    }
}

/**
 * @brief   delay the global system for given miliseconds
 * @note    we must use for loop for delay, as the large decrement makes the
 *          slow deviation for the delay
 * @param delay_ms 
 */
void system_delay_ms(uint32_t delay_ms)
{
    for (uint32_t i = 0; i < delay_ms; i++)
    {
        system_delay_us(1000);
    }
}


/**
 * @brief Configure the system clock using HSE and PLL
 *
 * @param plln PLL multiplication factor (determines final system clock frequency)
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
static void __init_as_clk_72MHz(uint32_t plln)
{
    HAL_StatusTypeDef ret = HAL_ERROR;     /* Initialize return status to error state */
    RCC_OscInitTypeDef rcc_osc_init = {0}; /* Initialize oscillator structure with zeros */
    RCC_ClkInitTypeDef rcc_clk_init = {0}; /* Initialize clock structure with zeros */

    /* Configure the High Speed External (HSE) oscillator and Phase Locked Loop (PLL) */
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE; /* Select HSE as oscillator type */
    rcc_osc_init.HSEState = RCC_HSE_ON;                   /* Enable HSE oscillator */
    rcc_osc_init.HSEPredivValue = RCC_HSE_PREDIV_DIV1;    /* Set HSE prescaler to 1 (no division) */
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;               /* Enable PLL */
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;       /* Select HSE as PLL input source */
    rcc_osc_init.PLL.PLLMUL = plln;                       /* Set PLL multiplication factor */
    ret = HAL_RCC_OscConfig(&rcc_osc_init);               /* Apply oscillator configuration */

    /**
     * @note Critical section: If oscillator configuration fails, the system cannot proceed
     *       as clock settings are fundamental to all microcontroller operations.
     *       The infinite loop should be replaced with proper error handling in production code.
     */
    if (ret != HAL_OK)
    {
        /* Clock initialization failed - system cannot function properly */
        __clk_bad_init_hook();
    }

    /* Configure system clock sources and bus prescalers */
    rcc_clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                              RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2); /* Select all clock domains to configure */
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;                  /* Use PLL output as system clock */
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;                         /* AHB bus clock = SYSCLK/1 */
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV2;                          /* APB1 (low-speed) bus clock = HCLK/2 */
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;                          /* APB2 (high-speed) bus clock = HCLK/1 */

    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_2);

    if (ret != HAL_OK)
    {
        /* Clock configuration failed - critical error condition */
        __clk_bad_init_hook();
    }
}

// clock init
void system_clock_init()
{
    __init_as_clk_72MHz(DEFAULT_RCC_PLL_VALUE);
}

