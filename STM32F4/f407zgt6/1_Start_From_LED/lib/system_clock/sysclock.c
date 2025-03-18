#include "sysclock.h"
/*
    Author Referenced
        For configs, i
*/

void SysTick_Handler(void){
    HAL_IncTick();
}

static inline void __clk_die(void){
    while(1){}
}

void sysclk_init(CCCLK_SysConfigType* config)
{

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  
    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = config->m;
    RCC_OscInitStruct.PLL.PLLN = config->n;
    RCC_OscInitStruct.PLL.PLLP = config->p;
    RCC_OscInitStruct.PLL.PLLQ = config->q;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      __clk_die();
    }
    
    // set the ppl as the sysclock
    RCC_ClkInitStruct.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
    // AHB
    RCC_ClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;
    // APB1
    RCC_ClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV4; 
    // APB2
    RCC_ClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV2;
		
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_5)!=HAL_OK) __clk_die();

	 //STM32F405x/407x/415x/417x Z版本的器件支持预取功能
	if (HAL_GetREVID() == 0x1001)
	{
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();  //使能flash预取
	}
}

uint64_t fetch_sysclk(void){
    return HAL_RCC_GetSysClockFreq();
}

static CCCLK_SysConfigType clk_168MHZ = {
    .m = 8,
    .n = 168,
    .p = RCC_PLLP_DIV2,
    .q = 4
};

void sysclk_set_as_common168MHz()
{
    sysclk_init(&clk_168MHZ);
}