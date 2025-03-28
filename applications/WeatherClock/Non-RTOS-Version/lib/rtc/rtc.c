#include "rtc.h"
#include <memory.h>
uint8_t setRTCDate(CCRTClock *clk, RTC_DateTypeDef *date)
{
    return HAL_RTC_SetDate(&clk->rtc_handle, date, RTC_FORMAT_BIN);
}

uint8_t setRTCTime(CCRTClock *clk, RTC_TimeTypeDef *time)
{
    return HAL_RTC_SetTime(&clk->rtc_handle, time, RTC_FORMAT_BIN);
}

uint8_t setRTCDateTime(CCRTClock *clk, CCDateTime *datetime)
{
    uint8_t result = HAL_OK;
    result = setRTCTime(clk, &datetime->time);
    if (result == HAL_ERROR)
    {
        return 0;
    }
    result = setRTCDate(clk, &datetime->date);
    if (result == HAL_ERROR)
    {
        return 0;
    }

    return HAL_OK;
}

void blank_rtc_init(RTC_InitTypeDef *initer)
{
    memset(initer, 0, sizeof(RTC_InitTypeDef));
}



#define RTC_MAGIC (0x1145)
uint8_t rtc_clk_init(
    CCRTClock *rthandle,
    CCDateTime *datetime,
    RTC_InitTypeDef *initer)
{
    rthandle->rtc_handle.Instance = RTC;
    rthandle->rtc_handle.Init = *initer;
    if (HAL_RTC_Init(&rthandle->rtc_handle) != HAL_OK)
        return RTC_INIT_FAILED;

    if (HAL_RTCEx_BKUPRead(&rthandle->rtc_handle, RTC_BKP_DR0) != RTC_MAGIC) // 是否第一次配置
    {
        setRTCDateTime(rthandle, datetime);
        HAL_RTCEx_BKUPWrite(&rthandle->rtc_handle, RTC_BKP_DR0, RTC_MAGIC);
    }
    return RTC_INIT_SUCCESS;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    __HAL_RCC_RTC_ENABLE();
}

void get_rtc_dateTime(CCRTClock *clk, CCDateTime *datetime)
{
    HAL_RTC_GetTime(&clk->rtc_handle, &datetime->time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&clk->rtc_handle, &datetime->date, RTC_FORMAT_BIN);
}

#include <stdio.h>
void on_provide_printable_datetime(
    CCDateTime *datetime,
    char *array_src, uint8_t arr_length,
    PrintPolicy policy)
{
    switch (policy)
    {
    case TIME_ONLY:
        snprintf(array_src, arr_length, "TIME: %02d:%02d:%02d",
                 datetime->time.Hours, datetime->time.Minutes, datetime->time.Seconds);
        break;
    case DATE_ONLY:
        snprintf(array_src, arr_length, "Date:20%02d-%02d-%02d",
                 datetime->date.Year, datetime->date.Month, datetime->date.Date);
        break;
    default:
        snprintf(array_src, arr_length, "TIME: %02d:%02d:%02d Date:20%02d-%02d-%02d",
                 datetime->time.Hours, datetime->time.Minutes, datetime->time.Seconds,
                 datetime->date.Year, datetime->date.Month, datetime->date.Date);
        break;
    }
}

extern CCRTClock*      cached_clk[CACHED_RTC_N];
extern uint8_t         current_cached_rtc;

void config_wakeup_rtc(CCRTClock *clk, uint32_t wk_sel, uint32_t cnt)
{
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&clk->rtc_handle, RTC_FLAG_WUTF);
	
	HAL_RTCEx_SetWakeUpTimer_IT(&clk->rtc_handle,cnt,wk_sel);
	cached_clk[current_cached_rtc] = clk;
    current_cached_rtc++;
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x02,0x02);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void config_alarm(CCRTClock *clk, uint8_t weekday, RTC_TimeTypeDef *time)
{
    RTC_AlarmTypeDef RTC_AlarmSturuct;
    RTC_AlarmSturuct.AlarmTime.Hours = time->Hours;     // 小时
    RTC_AlarmSturuct.AlarmTime.Minutes = time->Minutes; // 分钟
    RTC_AlarmSturuct.AlarmTime.Seconds = time->Seconds; // 秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds = 0;
    RTC_AlarmSturuct.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;

    RTC_AlarmSturuct.AlarmMask = RTC_ALARMMASK_NONE;
    RTC_AlarmSturuct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; 
    RTC_AlarmSturuct.AlarmDateWeekDay = weekday;                              
    RTC_AlarmSturuct.Alarm = RTC_ALARM_A;                                   
    HAL_RTC_SetAlarm_IT(&clk->rtc_handle, &RTC_AlarmSturuct, RTC_FORMAT_BIN);

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x01, 0x02);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}