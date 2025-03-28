#include "input_capture.h"

extern InputCapture*    current_capture;
extern void(*ic_post_init)(void);

extern InputCapture*        boarding_captures[BOARDING_ICCAP_N];
extern uint8_t              current_boarding_ic;
extern InputCapture*        boarding_captures[BOARDING_ICCAP_N];
extern uint8_t              current_boarding_ic;
void init_inputCapture(
    InputCapture* ic, TIM_TypeDef* who, 
    TIM_Base_InitTypeDef* base_info, 
    InputCapturePack*   pack,
    void(*__ic_post_init)(void))
{
    ic->handle.Instance = who;
    ic->handle.Init = *base_info;
    ic->channel = pack->channel;
    current_capture = ic;
    ic_post_init = __ic_post_init;
    boarding_captures[current_boarding_ic] = ic;
    current_boarding_ic++;
    HAL_TIM_IC_Init(&ic->handle);
    HAL_TIM_IC_ConfigChannel(&ic->handle, &pack->ic_config ,pack->channel);
}

void start_inputCapture(InputCapture* ic)
{
    HAL_TIM_IC_Start_IT(&ic->handle, ic->channel);
}

void stop_inputCapture(InputCapture* ic)
{
    HAL_TIM_IC_Stop_IT(&ic->handle, ic->channel);
}

void config_capture_triggure(
    InputCapture* ic,
    CaptureTriggerMode  mode)
{
    uint32_t val = 0;
    switch (mode)
    {
    case IC_CAP_RISING:
        val = TIM_ICPOLARITY_RISING;
        break;
    case IC_CAP_FALLING:
        val = TIM_ICPOLARITY_FALLING;
        break;
    case IC_CAP_BOTH:
        val = TIM_ICPOLARITY_BOTHEDGE;
        break;
    default:
        break;
    }
    __HAL_TIM_SET_CAPTUREPOLARITY(&ic->handle, ic->channel, val);
}

uint32_t ic_captured_value(InputCapture* ic)
{
    return HAL_TIM_ReadCapturedValue(&ic->handle, ic->channel);
}

void register_ic_callback(InputCapture* ic, void(*callbacks)(InputCapture*))
{
    ic->callbacks = callbacks;
}