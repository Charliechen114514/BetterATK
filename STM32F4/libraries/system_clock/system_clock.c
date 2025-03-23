#include "system_clock.h"
/*
    handling the init error when the clk init went wrong
*/



static void __clk_init_die()
{
    while (1)
        ;
}

static uint32_t __sysclk_freq;

/*
    post init makes a promise of initing the clocks interval
    making it possible for provide clock service
*/
static void inline clk_post_init()
{
    __sysclk_freq = HAL_RCC_GetSysClockFreq() / 1000000L;
}

static void __init_hse(const CCCLK_SysConfigType *config)
{
#ifndef DEPRECATED_HSE_INIT
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure;
    RCC_ClkInitTypeDef RCC_ClkInitStructure;

    __HAL_RCC_PWR_CLK_ENABLE(); // Enable the PWR clock

    // The following setting is used to set the regulator output voltage level to balance performance and power consumption
    // when the device is not running at maximum frequency.
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1); // Set regulator output voltage level to scale 1

    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE; // Set clock source to HSE
    RCC_OscInitStructure.HSEState = RCC_HSE_ON;                   // Enable HSE
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;               // Enable PLL
    RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;       // Set PLL clock source to HSE
    RCC_OscInitStructure.PLL.PLLM = config->m;                    // PLL input divider coefficient (PLL before division), range: 2~63
    RCC_OscInitStructure.PLL.PLLN = config->n;                    // PLL multiplier coefficient (PLL multiplication), range: 64~432
    RCC_OscInitStructure.PLL.PLLP = config->p;                    // System clock PLL divider coefficient (PLL after division), range: 2, 4, 6, 8 (only these 4 values)
    RCC_OscInitStructure.PLL.PLLQ = config->q;                    // PLL divider coefficient for USB/SDIO/random number generator, range: 2~15
    ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);               // Initialize PLL

    if (ret != HAL_OK)
    {
        __clk_init_die();
    }; // Error handling: enter infinite loop if failed

    // Select PLL as system clock source and configure HCLK, PCLK1, and PCLK2
    RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // Set PLL as system clock source
    RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB clock divider set to 1
    RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1 clock divider set to 4
    RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2 clock divider set to 2
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_5); // Configure clocks and set FLASH latency to 5WS (6 CPU cycles)

    if (ret != HAL_OK)
    {
        __clk_init_die(); // Error handling: enter infinite loop if failed
    }

    // STM32F405x/407x/415x/417x Z-series devices support prefetch buffer feature
    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE(); // Enable flash prefetch buffer
    }
    clk_post_init();
#else
    // wrong calls
    __clk_init_die();
#endif
}

void __init_hsi(const CCCLK_SysConfigType *config)
{
#ifndef DEPRECATED_HSI_INIT
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();                                    // Enable power clock
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1); // Set voltage scaling to level 1

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;          // Set oscillator type to HSI
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;                            // Enable HSI oscillator
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // Use default calibration for HSI
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                        // Enable PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;                // Use HSI as PLL source
    RCC_OscInitStruct.PLL.PLLM = config->m;                             // Set PLL M divider
    RCC_OscInitStruct.PLL.PLLN = config->n;                             // Set PLL N multiplier
    RCC_OscInitStruct.PLL.PLLP = config->p;                             // Set PLL P divider
    RCC_OscInitStruct.PLL.PLLQ = config->q;                             // Set PLL Q divider

    // Check for errors in the RCC oscillator initialization
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        __clk_init_die(); // If initialization fails, enter an infinite loop
    }

    // Configure PLL as the system clock
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2); // Set clock types for SYS, AHB, APB1, and APB2
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;                                                              // Set PLL as the system clock source
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                                                     // Set AHB clock divider to 1
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;                                                                      // Set APB1 clock divider to 4
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;                                                                      // Set APB2 clock divider to 2

    // Apply the clock configuration and check for errors
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        __clk_init_die(); // If configuration fails, enter an infinite loop
    }

    // STM32F405x/407x/415x/417x Z-series devices support prefetch buffer feature
    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE(); // Enable flash prefetch buffer
    }
    clk_post_init();
#else
    // wrong calls
    __clk_init_die();
#endif
}

void system_clock_init(
    const CCCLK_SysConfigType *sysconfig,
    const ClockSourcePreference source)
{
    switch (source)
    {
    case USE_HSI_AS_SYSCLK:
        __init_hsi(sysconfig);
        break;
    case USE_HSE_AS_SYSCLK:
        __init_hse(sysconfig);
        break;
    default:
        __clk_init_die();
    }
}

static CCCLK_SysConfigType hseclk_168MHZ = {
    .m = 8,
    .n = 336,
    .p = RCC_PLLP_DIV2,
    .q = 7};

static CCCLK_SysConfigType hsiclk_168MHZ = {
    .m = 8,
    .n = 168,
    .p = RCC_PLLP_DIV2,
    .q = 4};

void system_clock_initAs168MHz(const ClockSourcePreference source)
{
    switch (source)
    {
    case USE_HSI_AS_SYSCLK:
        __init_hsi(&hsiclk_168MHZ);
        break;
    case USE_HSE_AS_SYSCLK:
        __init_hse(&hseclk_168MHZ);
        break;
    default:
        __clk_init_die();
    }
}

uint32_t provide_application_sysfreq()
{
    return HAL_RCC_GetSysClockFreq();
}

void system_delay_us(uint32_t us)
{
    uint32_t ticks;                                     // Total number of ticks required for the delay
    uint32_t prevValue, currentValue, elapsedTicks = 0; // Variables for SysTick counter values and elapsed ticks
    uint32_t reloadValue = SysTick->LOAD;               // Value of the SysTick reload register

    ticks = us * __sysclk_freq; // Calculate the number of ticks needed for the desired delay in microseconds

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

void system_delay_ms(uint32_t delay_ms)
{
    for (uint32_t i = 0; i < delay_ms; i++)
    {
        system_delay_us(1000);
    }
}