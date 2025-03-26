#include "watchdog.h"
extern WindowWatchDogHandler*       pHandler[MAX_WWDOG_MEMORIZE];
extern uint8_t                      current_wwatchdog_boarding;

void WWDG_IRQHandler(void){
    for(uint8_t i = 0; i < current_wwatchdog_boarding; i++){
        if(pHandler[i]->handle.Instance == WWDG){
            HAL_WWDG_IRQHandler(&pHandler[i]->handle);
            return;
        }
    }
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
    for(uint8_t i = 0; i < current_wwatchdog_boarding; i++){
        if(&pHandler[i]->handle == hwwdg){
            pHandler[i]->callback(pHandler[i]);
        }
    } 
}