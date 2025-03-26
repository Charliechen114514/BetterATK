#include <stdio.h>
#include <string.h>
#include "time/time_utils.h"
#include "rtc.h"
extern CCRTClock   rtc_clk;
static void init_datetime(CCDateTime* datetime, Timeinfo* t)
{
    RTC_DateTypeDef*    date = &(datetime->date);
    INIT_DATE(date, 1, t->day, t->month, (t->year - 2000));
    RTC_TimeTypeDef*    time = &(datetime->time);
    INIT_TIME(time, t->hour, t->minute, t->second);
}

static void parse_string(const char* timestamp, CCDateTime* datetime)
{   
    int year;
    int month, day, hour, minute, second;
    
    sscanf(timestamp, "%d-%d-%d %d:%d:%d", 
           &year, &month, &day, &hour, &minute, &second);
    
    Timeinfo info = {
        .year = year,
        .day = day,
        .month = month,
        .minute = minute,
        .hour = hour,
        .second = second
    };

    init_datetime(datetime, &info);
}

void on_reset_rtc_time(TimeInfoPackage* package)
{
    CCDateTime      dateTime;
    parse_string(package->timestamp, &dateTime);
    setRTCDateTime(&rtc_clk, &dateTime);
}

const char* months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", 
    "Jun", "Jul", "Aug", "Sep", "Oct",
    "Nov", "Dec"
};

void on_fetch_month_engstr(int month_day, char* buffer, uint8_t len)
{
    if(month_day >= 12 || month_day < 0){
        strncpy(buffer,"", len);
    }else{
        strncpy(buffer, months[month_day], len);
    }
}

