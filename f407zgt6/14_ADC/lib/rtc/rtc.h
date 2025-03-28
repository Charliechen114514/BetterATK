#ifndef RTC_H
#define RTC_H

#include "stm32f4xx_hal.h"

typedef struct
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} CCDateTime;

typedef struct __CCRTClock
{
    RTC_HandleTypeDef rtc_handle;
} CCRTClock;

#define RTC_INIT_SUCCESS (1)
#define RTC_INIT_FAILED (0)

#define CACHED_RTC_N        (1)

uint8_t rtc_clk_init(
    CCRTClock *rthandle,
    CCDateTime *datetime,
    RTC_InitTypeDef *initer);

void blank_rtc_init(RTC_InitTypeDef *initer);

#define INIT_DATE(dateStructPointer, weekday, day, month, year) \
    do                                                          \
    {                                                           \
        dateStructPointer->WeekDay = weekday;                   \
        dateStructPointer->Month = month;                       \
        dateStructPointer->Date = day;                          \
        dateStructPointer->Year = year;                         \
    } while (0)

#define INIT_TIME(timeStructPointer, hour, minute, second)            \
    do                                                                \
    {                                                                 \
        timeStructPointer->Hours = hour;                              \
        timeStructPointer->Minutes = minute;                          \
        timeStructPointer->Seconds = second;                          \
        timeStructPointer->TimeFormat = RTC_HOURFORMAT12_PM;          \
        timeStructPointer->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;  \
        timeStructPointer->StoreOperation = RTC_STOREOPERATION_RESET; \
    } while (0)

uint8_t setRTCDate(CCRTClock *clk, RTC_DateTypeDef *date);
uint8_t setRTCTime(CCRTClock *clk, RTC_TimeTypeDef *time);
uint8_t setRTCDateTime(CCRTClock *clk, CCDateTime *datetime);

void get_rtc_dateTime(CCRTClock *clk, CCDateTime *datetime);

typedef enum
{
    TIME_ONLY,
    DATE_ONLY,
    DATETIME
} PrintPolicy;

void on_provide_printable_datetime(
    CCDateTime *datetime, 
    char* array_src, uint8_t arr_length,
    PrintPolicy policy);


void config_wakeup_rtc(CCRTClock* clk, uint32_t wk_sel, uint32_t cnt);
void config_alarm(CCRTClock* clk, uint8_t weekday, RTC_TimeTypeDef* time);


#endif