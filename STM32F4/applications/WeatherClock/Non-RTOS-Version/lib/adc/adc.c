#include "adc.h"

void(*adc_post_init)(void);
CCADC_Handle* cur_init;
void init_adc_handle(CCADC_Handle *handle,
    ADC_TypeDef* raw, ADC_InitTypeDef *initer,
    uint32_t channel, void(*__adc_post_init)(void))
{
    handle->channel = channel;
    handle->handle.Instance=ADC1;
    handle->handle.Init = *initer;
    adc_post_init = __adc_post_init;
    cur_init = handle;
    HAL_ADC_Init(&handle->handle);   
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if(&cur_init->handle == hadc)
        adc_post_init();
}

uint16_t fetch_adc_raw_value(CCADC_Handle *handle)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel = handle->channel;                                   
    ADC1_ChanConf.Rank=1;                                      
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        
    ADC1_ChanConf.Offset=0;                 
    HAL_ADC_ConfigChannel(&handle->handle, &ADC1_ChanConf);
	
    HAL_ADC_Start(&handle->handle);                         
	
    HAL_ADC_PollForConversion(&handle->handle, 10);               
 
	return (uint16_t)HAL_ADC_GetValue(&handle->handle);
}

#include <memory.h>
void        reset_avg_policy(AverageADC_Policy* p)
{
    memset(p, 0, sizeof(AverageADC_Policy));
}

static void __def_policy(AverageADC_Policy* p, uint16_t value)
{
    p->cached_sum += value;
}

#include "system_clock.h"
double    fetch_adc_avg_raw_value(CCADC_Handle *handle, AverageADC_Policy* p)
{
    if(p->policy == NULL){
        p->policy = __def_policy;
    }
    for(uint16_t i = 0; i < p->avg_times; i++)
    {
        p->policy(p, fetch_adc_raw_value(handle));
        system_delay_ms(p->delay_ms);
    }
    double res = p->avg_times ? ((double)p->cached_sum) / p->avg_times : 0;
    p->cached_sum = 0;
    return res;
}

static AverageADC_Policy   __internal_temp_policy = {
    .avg_times = 10,
    .cached_sum = 0,
    .delay_ms = 10,
    .policy = NULL
};

double fetch_current_temperature(CCADC_Handle* handle)
{
    uint32_t channel_cached = handle->channel;
    handle->channel = ADC_CHANNEL_TEMPSENSOR;
    double temperate = fetch_adc_avg_raw_value(handle, &__internal_temp_policy);
    temperate = temperate*(3.3/4096);		
	temperate = (temperate-0.76)/0.0025 + 25; 
    handle->channel = channel_cached;
    return temperate;
}