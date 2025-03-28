#ifndef WEATHER_H
#define WEATHER_H
#include "ccgraphic/widgets/base/CCGraphic_Image/CCGraphic_Image.h"
#include "json_parser.h"
#include "tools.h"

#define WeatherDisplayTEXT_LENGTH         (10)
typedef struct {
    CCGraphic_Image     image_src;
    char                text[WeatherDisplayTEXT_LENGTH];
}WeatherDisplayPackage;

/*
    To rewrite the issue, you need to see the weather documentations
    for the meanings of the package info
*/
void on_gain_weather_package_display(
    WeatherStatus           status,
    WeatherDisplayPackage*  displayPackage);




#endif