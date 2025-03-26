#include "tools.h"
#include <string.h>
#include <ctype.h>

static void __strlwr(char *str) {
    if (str == NULL) return;  // 安全检查
    
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}

static unsigned char inline __matches(
    const char* weather_string, const char* target)
{
    return !!(strstr(weather_string, target));
}

WeatherStatus fetch_status_from_package(
    char* weather_string)
{
    __strlwr(weather_string);
    if(__matches(weather_string, "sunny")){
        return WEATHER_SUNNY;
    }else if(__matches(weather_string, "cloudy")){
        return WEATHER_CLOUDY;
    }else if(__matches(weather_string, "overcast")){
        return WEATHER_OVERCAST;
    }else if(
        __matches(weather_string, "shower") || 
        __matches(weather_string, "rain")){
        return WEATHER_RAINY;
    }else if(__matches(weather_string, "snow")){
        return WEATHER_SNOWY;
    }

    return WEATHER_CLOUDY;
}