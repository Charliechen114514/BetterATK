#include "weather/wind_humi.h"
#include <stdio.h>
#include <string.h>
extern uint8_t wind_speedData[];
extern uint8_t wind_directionData[];
extern uint8_t humiData[];

void fetch_windhumi_package_options(
    WindHumiPack *rain_package,
    WeatherInfoPackage *packages, WindHumiOptions op)
{
    CCGraphic_Size s = {
        .height = 32,
        .width = 32};
    rain_package->image.image_size = s;
    char temp_format[10];
    switch (op)
    {
    case HumiOp:
        rain_package->image.sources_register = humiData;
        snprintf(temp_format, 10, "%s %%", packages->humidity);
        strncpy(rain_package->text, temp_format, 10);
        break;
    case WindSpeedOp:
        rain_package->image.sources_register = wind_speedData;
        snprintf(temp_format, 10, "%s", packages->windy_info.speed);
        strncpy(rain_package->text, temp_format, 10);
        break;
    case WindDirectOP:
        rain_package->image.sources_register = wind_directionData;
        snprintf(temp_format, 10, "%s %s", packages->windy_info.direction, packages->windy_info.degree);
        strncpy(rain_package->text, temp_format, 10);
        break;
    case WindSpeedLevelOp:
        rain_package->image.sources_register = NULL;
        snprintf(temp_format, 10, "Lv: %s", packages->windy_info.scale);
        strncpy(rain_package->text, temp_format, 10);
    }
}