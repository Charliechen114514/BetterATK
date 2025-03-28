#ifndef WIND_HUMI_H
#define WIND_HUMI_H

#include "ccgraphic/widgets/base/CCGraphic_Image/CCGraphic_Image.h"
#include "json_parser.h"
#include "tools.h"

typedef enum {
    HumiOp,
    WindSpeedOp,
    WindDirectOP,
    WindSpeedLevelOp
}WindHumiOptions;

typedef struct {
    CCGraphic_Image         image;
    char                    text[10];
}WindHumiPack;

void fetch_windhumi_package_options(
    WindHumiPack* rain_package,
    WeatherInfoPackage* packages, WindHumiOptions op);


#endif