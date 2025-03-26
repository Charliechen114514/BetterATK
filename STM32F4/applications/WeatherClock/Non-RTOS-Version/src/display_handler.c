#include <stdio.h>
#include "ccgraphic/fast_setup/fast_setup.h"
#include "ccgraphic/sim_app/simple_text.h"
#include "ccgraphic/CCGraphic_device_adapter.h"
#include "running_loggy.h"
#include "ccuart.h"
#include "json_parser.h"

CCDeviceHandler private_handler;
CCGraphicTextEdit text;

WeatherInfoPackage weather_package;
TimeInfoPackage time_package;

CCDeviceHandler *setup_display_device()
{
    oled_soft_iic_setup(&private_handler);
    return &private_handler;
}

CCDeviceHandler *get_private_handler()
{
    return &private_handler;
}

#define DISP_BUFFER (60)
void on_setup_weather_page(CCDeviceHandler *handler,
                                        WeatherInfoPackage *weather_package)
{
    handler->operations.clear_device_function(handler);
    char buffer_contains[DISP_BUFFER];
    snprintf(buffer_contains, DISP_BUFFER, "Day : %s", weather_package->dayWeather);
    set_simple_text(&text, handler, buffer_contains);
    snprintf(buffer_contains, DISP_BUFFER, "Night: %s", weather_package->nightWeather);
    set_newline_text(&text, handler, buffer_contains);
    snprintf(buffer_contains, DISP_BUFFER, "HTemp: %s LTemp: %s", weather_package->highTemp, weather_package->lowTemp);
    set_newline_text(&text, handler, buffer_contains);
    snprintf(buffer_contains, DISP_BUFFER, "RainFall: %s",
             weather_package->rainy_info.rainfallContainer);
    set_newline_text(&text, handler, buffer_contains);
    snprintf(buffer_contains, DISP_BUFFER, "RainFall Posi: %s",
             weather_package->rainy_info.rainPrecip);
    set_newline_text(&text, handler, buffer_contains);
    snprintf(buffer_contains, DISP_BUFFER, "WDr: %s WDegr: %s",
             weather_package->windy_info.direction,
             weather_package->windy_info.degree);
    set_newline_text(&text, handler, buffer_contains);
    snprintf(buffer_contains, DISP_BUFFER, "WScal: %s Spd: %s",
             weather_package->windy_info.scale,
             weather_package->windy_info.speed);
    set_newline_text(&text, handler, buffer_contains);

    snprintf(buffer_contains, DISP_BUFFER, "Humi: %s", weather_package->humidity);
    set_newline_text(&text, handler, buffer_contains);
}

extern CCUART uart1;
void on_weatherinfo_send_uart_info(WeatherInfoPackage *weather_package)
{
    send_runninglogger_to_uart("Day : %s Night: %s\r\n", weather_package->dayWeather, weather_package->nightWeather);
    send_runninglogger_to_uart("HTemp: %s LTemp: %s\r\n", weather_package->highTemp, weather_package->lowTemp);
    send_runninglogger_to_uart("RainFall: %s Perc: %s\r\n",
                               weather_package->rainy_info.rainfallContainer,
                               weather_package->rainy_info.rainPrecip);
    send_runninglogger_to_uart("WDr: %s WDegr: %s WScal: %s Spd: %s\r\n",
                               weather_package->windy_info.direction,
                               weather_package->windy_info.degree,
                               weather_package->windy_info.scale,
                               weather_package->windy_info.speed);

    send_runninglogger_to_uart("Humi: %s\r\n", weather_package->humidity);
}
