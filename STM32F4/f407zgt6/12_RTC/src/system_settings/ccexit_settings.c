#include "external_it.h"
EXTI_Handler*       memorizingTypes[MAX_MEMORIZE_TYPE_N];
uint8_t             actual_memorize;
/*
    defines here, handling can be rewrites if like
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for(uint8_t i = 0; i < actual_memorize; i++){
        if(memorizingTypes[i]->gpio_info.pinType == GPIO_Pin){
            if(memorizingTypes[i]->callback)
                memorizingTypes[i]->callback(memorizingTypes[i]);
        }
    }
}
