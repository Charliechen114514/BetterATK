#include "weather/temperature.h"
#include <string.h>
#include <stdio.h>

extern uint8_t temperatureData[];
void fetch_temperature_package(
    TemperaturePackage* blank,
    WeatherInfoPackage* packages, TemperatureOptions op)
{
    CCGraphic_Size s = {
        .height = 32,
        .width = 32
    };
    blank->image.image_size = s;
    blank->image.sources_register = temperatureData;
    char temp[10];
    switch(op)
    {
        case LOW_TEMP:
            snprintf(temp, 10, "L: %s", packages->lowTemp);
            strncpy(blank->text,temp , 10);
        break;
        case HIGH_TEMP:
            snprintf(temp, 10, "H: %s", packages->highTemp);
            strncpy(blank->text, temp, 10);
        break;
    }
}