#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include "json_parser.h"

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}Timeinfo;

void on_reset_rtc_time(TimeInfoPackage* package);
void on_fetch_month_engstr(int month_day, char* buffer, uint8_t len);
#endif