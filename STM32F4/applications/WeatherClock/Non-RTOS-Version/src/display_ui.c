#include "ccgraphic/widgets/base/CCGraphic_Image/CCGraphic_Image.h"
#include "ccgraphic/widgets/components/CCGraphic_TextEdit/CCGraphic_TextEdit.h"
#include "ccgraphic/widgets/components/CCGraphic_Frame/CCGraphic_Frame.h"
#include "ccgraphic/CCGraphic_device_adapter.h"
#include "display_ui.h"
#include "tools.h"
#include "weather/weather.h"

typedef struct
{
    char *sources;
    int offsetX;
} BaseMenuPack;

static void on_create_the_base_menu(CCDeviceHandler *handler,
                                    BaseMenuPack *pack)
{
    CCGraphic_Frame frame;
    // mappings the title
    CCGraphic_AsciiTextItem item;
    CCGraphic_Point p;
    p.x = pack->offsetX;
    p.y = 6;
    CCGraphic_Size acceptablesize =
        CCGraphicWidget_MaxAcceptable_Size(handler);
    CCGraphicWidget_init_AsciiTextItem(
        &item, p, acceptablesize, ASCII_6x8);
    CCGraphic_createDeviceFrame(&frame, handler, 1, 2, 2);
    frame.operation.operation.show(&frame);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(
        &item, &p);
    CCGraphicWidget_AsciiTextItem_setAsciiText(&item, pack->sources);
    CCGraphicWidget_drawAsciiTextItem(handler, &item);
    handler->operations.update_device_function(handler);
}

static void on_place_weather_package(
    CCDeviceHandler *handler,
    WeatherDisplayPackage *package,
    CCGraphic_Point tl_text, CCGraphic_Point tl_image)
{
    CCGraphic_AsciiTextItem item;
    CCGraphic_Point p = tl_text;
    CCGraphic_Size acceptablesize =
        CCGraphicWidget_MaxAcceptable_Size(handler);
    CCGraphicWidget_init_AsciiTextItem(
        &item, p, acceptablesize, ASCII_6x8);
    package->image_src.point = tl_image;
    CCGraphicWidget_draw_image(handler, &package->image_src);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(
        &item, &p);
    CCGraphicWidget_AsciiTextItem_setAsciiText(&item, package->text);
    CCGraphicWidget_drawAsciiTextItem(handler, &item);
    handler->operations.update_device_function(handler);
}

static void handling_weather_page_mappings(CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    WeatherDisplayPackage disp_pack;
    WeatherStatus status = fetch_status_from_package(package->dayWeather);
    on_gain_weather_package_display(status, &disp_pack);
    CCGraphic_Point tl_text = {
        .x = 20,
        .y = 52};
    CCGraphic_Point tl_image = {
        .x = 20,
        .y = 18};
    on_place_weather_package(handler, &disp_pack, tl_text, tl_image);
    status = fetch_status_from_package(package->nightWeather);
    on_gain_weather_package_display(status, &disp_pack);
    tl_text.x = 70;
    tl_image.x = 76;
    on_place_weather_package(handler, &disp_pack, tl_text, tl_image);
}

static void on_turn_to_weather_page(CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    handler->operations.clear_device_function(handler);
    BaseMenuPack pack = {
        .offsetX = 26,
        .sources = "Day     Night"};
    on_create_the_base_menu(handler, &pack);
    handling_weather_page_mappings(handler, package);
    handler->operations.update_device_function(handler);
}

#include "weather/temperature.h"
static void on_place_temperature_package(
    CCDeviceHandler *handler,
    TemperaturePackage *package,
    CCGraphic_Point tl_text, CCGraphic_Point tl_image)
{
    CCGraphic_AsciiTextItem item;
    CCGraphic_Point p = tl_text;
    CCGraphic_Size acceptablesize =
        CCGraphicWidget_MaxAcceptable_Size(handler);
    CCGraphicWidget_init_AsciiTextItem(
        &item, p, acceptablesize, ASCII_6x8);
    package->image.point = tl_image;
    CCGraphicWidget_draw_image(handler, &package->image);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(
        &item, &p);
    CCGraphicWidget_AsciiTextItem_setAsciiText(&item, package->text);
    CCGraphicWidget_drawAsciiTextItem(handler, &item);
    handler->operations.update_device_function(handler);
}

static void handling_temperature_page_mappings(
    CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    TemperaturePackage disp_pack;
    fetch_temperature_package(
        &disp_pack, package, LOW_TEMP);
    CCGraphic_Point tl_text = {
        .x = 20,
        .y = 52};
    CCGraphic_Point tl_image = {
        .x = 20,
        .y = 18};
    on_place_temperature_package(handler, &disp_pack, tl_text, tl_image);
    fetch_temperature_package(
        &disp_pack, package, HIGH_TEMP);
    tl_text.x = 70;
    tl_image.x = 65;
    on_place_temperature_package(handler, &disp_pack, tl_text, tl_image);
}

static void on_turn_to_temperature_page(CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    handler->operations.clear_device_function(handler);
    BaseMenuPack pack = {
        .offsetX = 26,
        .sources = "Low     High"};
    on_create_the_base_menu(handler, &pack);
    handling_temperature_page_mappings(handler, package);
    handler->operations.update_device_function(handler);
}

#include "weather/rain.h"
static void on_place_rainy_package(
    CCDeviceHandler *handler,
    RainPackage *package,
    CCGraphic_Point tl_text, CCGraphic_Point tl_image)
{
    CCGraphic_AsciiTextItem item;
    CCGraphic_Point p = tl_text;
    CCGraphic_Size acceptablesize =
        CCGraphicWidget_MaxAcceptable_Size(handler);
    CCGraphicWidget_init_AsciiTextItem(
        &item, p, acceptablesize, ASCII_6x8);
    package->image.point = tl_image;
    CCGraphicWidget_draw_image(handler, &package->image);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(
        &item, &p);
    CCGraphicWidget_AsciiTextItem_setAsciiText(&item, package->text);
    CCGraphicWidget_drawAsciiTextItem(handler, &item);
    handler->operations.update_device_function(handler);
}

static void handling_rainy_page_mappings(
    CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    RainPackage disp_pack;
    fetch_rain_package_options(
        &disp_pack, package, RainPercentageOp);
    CCGraphic_Point tl_text = {
        .x = 20,
        .y = 52};
    CCGraphic_Point tl_image = {
        .x = 20,
        .y = 18};
    on_place_rainy_package(handler, &disp_pack, tl_text, tl_image);
    fetch_rain_package_options(
        &disp_pack, package, RainDepthOp);
    tl_text.x = 70;
    tl_image.x = 66;
    on_place_rainy_package(handler, &disp_pack, tl_text, tl_image);
}

static void on_turn_to_rainy_page(
    CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    handler->operations.clear_device_function(handler);
    BaseMenuPack pack = {
        .offsetX = 26,
        .sources = "Per     Dept"};
    on_create_the_base_menu(handler, &pack);
    handling_rainy_page_mappings(handler, package);
    handler->operations.update_device_function(handler);
}

#include "weather/wind_humi.h"
static void on_place_windhumi_package(
    CCDeviceHandler *handler,
    WindHumiPack *package,
    CCGraphic_Point tl_text, CCGraphic_Point tl_image)
{
    CCGraphic_AsciiTextItem item;
    CCGraphic_Point p = tl_text;
    CCGraphic_Size acceptablesize =
        CCGraphicWidget_MaxAcceptable_Size(handler);
    CCGraphicWidget_init_AsciiTextItem(
        &item, p, acceptablesize, ASCII_6x8);
    package->image.point = tl_image;
    CCGraphicWidget_draw_image(handler, &package->image);
    CCGraphicWidget_AsciiTextItem_setIndexedPoint(
        &item, &p);
    CCGraphicWidget_AsciiTextItem_setAsciiText(&item, package->text);
    CCGraphicWidget_drawAsciiTextItem(handler, &item);
    handler->operations.update_device_function(handler);
}

static void handling_windhumi_page_mappings(
    CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    WindHumiPack disp_pack;
    fetch_windhumi_package_options(
        &disp_pack, package, WindDirectOP);
    CCGraphic_Point tl_text = {
        .x = 10,
        .y = 52};
    CCGraphic_Point tl_image = {
        .x = 10,
        .y = 18};
    on_place_windhumi_package(handler, &disp_pack, tl_text, tl_image);
    fetch_windhumi_package_options(
        &disp_pack, package, WindSpeedOp);
    tl_text.x = 50;
    tl_image.x = 45;
    on_place_windhumi_package(handler, &disp_pack, tl_text, tl_image);
    fetch_windhumi_package_options(
        &disp_pack, package, HumiOp);
    tl_text.x = 90;
    tl_image.x = 82;
    on_place_windhumi_package(handler, &disp_pack, tl_text, tl_image);
}

static void on_turn_to_windhumi_page(
    CCDeviceHandler *handler, WeatherInfoPackage *package)
{
    handler->operations.clear_device_function(handler);
    BaseMenuPack pack = {
        .offsetX = 26,
        .sources = "Dir Spd  Humi"};
    on_create_the_base_menu(handler, &pack);
    handling_windhumi_page_mappings(handler, package);
    handler->operations.update_device_function(handler);
}


void turn_to_page(
    CCDeviceHandler *handler,
    WeatherInfoPackage *package,
    PageIndex pageIndex)
{
    switch (pageIndex)
    {
    case WeatherPage:
        on_turn_to_weather_page(handler, package);
        break;
    case TemperaturePage:
        on_turn_to_temperature_page(handler, package);
        break;
    case RainyPage:
        on_turn_to_rainy_page(handler, package);
        break;
    case WindyPage:
        on_turn_to_windhumi_page(handler, package);
        break;
    default:
        break;
    }
}