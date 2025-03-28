#include "rtc.h"

#include "ccuart.h"
extern CCUART uart1;
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    send_message(&uart1, "Alarm A Occurs!!!");
}

CCRTClock*      cached_clk[CACHED_RTC_N];
uint8_t         current_cached_rtc;

void RTC_Alarm_IRQHandler(void)
{
    for(uint8_t i = 0; i < current_cached_rtc; i++){
        if(cached_clk[i]->rtc_handle.Instance == RTC){
            HAL_RTC_AlarmIRQHandler(&cached_clk[i]->rtc_handle);
        }
    }
}

void RTC_WKUP_IRQHandler(void)
{
    for(uint8_t i = 0; i < current_cached_rtc; i++){
        if(cached_clk[i]->rtc_handle.Instance == RTC){
            HAL_RTCEx_WakeUpTimerIRQHandler(&cached_clk[i]->rtc_handle);
        }
    }
}
#include "gpio.h"
extern CCGPIOType   led0;
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    reverse_ccgpio_pinstate(&led0); 
}

