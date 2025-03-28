#ifndef TOOLS_H
#define TOOLS_H
#include "json_parser.h"

typedef enum {
    WEATHER_SUNNY,
    WEATHER_CLOUDY,
    WEATHER_OVERCAST,
    WEATHER_RAINY,
    WEATHER_SNOWY
}WeatherStatus;

WeatherStatus fetch_status_from_package(char* weather_string);



#endif