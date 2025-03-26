#include "input_capture.h"

InputCapture*    current_capture;
void(*ic_post_init)(void);

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if(&current_capture->handle == htim)
    {
        ic_post_init();
    }
}

InputCapture*        boarding_captures[BOARDING_ICCAP_N];
uint8_t              current_boarding_ic;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    for(uint8_t i = 0; i < current_boarding_ic; i++){
        if(&boarding_captures[i]->handle == htim){
            if(boarding_captures[i]->callbacks)
                boarding_captures[i]->callbacks(boarding_captures[i]);
        }
    }
}

void TIM5_IRQHandler(void)
{
    for(uint8_t i = 0; i < current_boarding_ic; i++){
        if(boarding_captures[i]->handle.Instance == TIM5){
            HAL_TIM_IRQHandler(&boarding_captures[i]->handle);
        }
    }
}
