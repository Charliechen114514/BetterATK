#ifndef WEATHER_FETCHER_H
#define WEATHER_FETCHER_H
#include "esp8266.h"


ESP8266_Handle* init_weather_fetcher();
ESP8266_Handle* get_weather_fetcher_drive();
/* We has one ESP8266 to fetch the weather info */
uint8_t weather_info_session(ESP8266_Handle* who, 
    char* container, int container_len);

uint8_t time_info_session(ESP8266_Handle* who, 
    char* container, int container_len);

#endif