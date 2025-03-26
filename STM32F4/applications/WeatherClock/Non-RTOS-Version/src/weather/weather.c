#include "weather/weather.h"
#include "ccgraphic/CCGraphic_device_adapter.h"
#include "tools.h"
#include <string.h>
extern uint8_t sunnyData[];
extern uint8_t cloudyData[];
extern uint8_t overcastData[];
extern uint8_t rainData[];
extern uint8_t snowData[];

static void __set_datas(WeatherDisplayPackage *displayPackage,
                        uint8_t *data, char *text)
{
    displayPackage->image_src.sources_register = data;
    strncpy(displayPackage->text, text, WeatherDisplayTEXT_LENGTH);
}

void on_gain_weather_package_display(
    WeatherStatus status,
    WeatherDisplayPackage *displayPackage)
{
    displayPackage->image_src.image_size.height = 32;
    displayPackage->image_src.image_size.width = 32;
    switch (status)
    {
    case WEATHER_SUNNY:
        __set_datas(displayPackage, sunnyData, "sunny");
        break;
    case WEATHER_RAINY:
        __set_datas(displayPackage, rainData, "Rainy");
        break;
    case WEATHER_SNOWY:
        __set_datas(displayPackage, snowData, "Snowy");
        break;
    case WEATHER_OVERCAST:
        __set_datas(displayPackage, overcastData, "Overcast");
        break;
    case WEATHER_CLOUDY:
        __set_datas(displayPackage, cloudyData, "Cloudy");
        break;
    default:
        __set_datas(displayPackage, NULL, "Unknown");
        break;
    }
}