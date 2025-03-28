#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H
#include "json_parser.h"
typedef struct __DeviceProperty CCDeviceHandler;

CCDeviceHandler *setup_display_device();
CCDeviceHandler *get_private_handler();
void on_setup_weather_page(CCDeviceHandler *handler,
                           WeatherInfoPackage *weather_package);
void on_weatherinfo_send_uart_info(WeatherInfoPackage *weather_package);
#endif