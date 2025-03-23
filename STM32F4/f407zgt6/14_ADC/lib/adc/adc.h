#ifndef ADC_H
#define ADC_H
#include "stm32f4xx_hal.h"
typedef struct
{
    ADC_HandleTypeDef   handle;
    uint32_t            channel;
} CCADC_Handle;

typedef struct __avg_pol AverageADC_Policy;
typedef struct __avg_pol{
    uint16_t        delay_ms;
    uint64_t        cached_sum;
    uint16_t        avg_times;
    // if NULL, means the default policy
    void(*policy)(AverageADC_Policy*, uint16_t value);
}AverageADC_Policy;

void init_adc_handle(CCADC_Handle *handle,ADC_TypeDef* raw,  
        ADC_InitTypeDef *initer, uint32_t channel,  void(*adc_post_init)(void));
uint16_t fetch_adc_raw_value(CCADC_Handle *handle);
double    fetch_adc_avg_raw_value(CCADC_Handle *handle, AverageADC_Policy* p);

void        reset_avg_policy(AverageADC_Policy* p);

#endif