#include "weather/rain.h"
#include <stdio.h>
#include <string.h>

extern uint8_t raindepthData[];
extern uint8_t rainPercentageData[];

void fetch_rain_package_options(
    RainPackage *rain_package,
    WeatherInfoPackage *packages, RainOptions op)
{
    CCGraphic_Size s = {
        .height = 32,
        .width = 32};
    rain_package->image.image_size = s;
    char temp[10];
    switch (op)
    {
    case RainPercentageOp:
        rain_package->image.sources_register = rainPercentageData;
        snprintf(temp, 10, "%s %%", packages->rainy_info.rainPrecip);
        strncpy(rain_package->text, temp, 10);
        break;
    case RainDepthOp:
        rain_package->image.sources_register = raindepthData;
        snprintf(temp, 10, "%smm", packages->rainy_info.rainfallContainer);
        strncpy(rain_package->text, temp, 10);
        break;
    }
}