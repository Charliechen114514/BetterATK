#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H
#include "json_parser.h"
typedef struct __DeviceProperty CCDeviceHandler;

typedef enum
{
    TimePage = 1,
    WeatherPage,
    TemperaturePage,
    WindyPage,
    RainyPage,
    PageIndexMAX
} PageIndex;

void turn_to_page(CCDeviceHandler *handler, WeatherInfoPackage *package, PageIndex pageIndex);

#endif