#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "ccgraphic/widgets/base/CCGraphic_Image/CCGraphic_Image.h"
#include "json_parser.h"
#include "tools.h"

typedef enum {
    LOW_TEMP,
    HIGH_TEMP
}TemperatureOptions;

typedef struct {
    CCGraphic_Image         image;
    char                    text[10];
}TemperaturePackage;

void fetch_temperature_package(
    TemperaturePackage* blank,
    WeatherInfoPackage* packages, TemperatureOptions op);



#endif