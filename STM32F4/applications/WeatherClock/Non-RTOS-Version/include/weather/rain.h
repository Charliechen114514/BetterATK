#ifndef RAIN_H
#define RAIN_H
#include "ccgraphic/widgets/base/CCGraphic_Image/CCGraphic_Image.h"
#include "json_parser.h"
#include "tools.h"

typedef enum {
    RainPercentageOp,
    RainDepthOp
}RainOptions;

typedef struct {
    CCGraphic_Image         image;
    char                    text[10];
}RainPackage;


void fetch_rain_package_options(
    RainPackage* rain_package,
    WeatherInfoPackage* packages, RainOptions op);

#endif