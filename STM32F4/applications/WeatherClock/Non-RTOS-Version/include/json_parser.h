#ifndef JSON_PARSER_H
#define JSON_PARSER_H

// WeatherInfoPackage 结构体的数组大小约束宏
#define DAY_WEATHER_SIZE        15
#define NIGHT_WEATHER_SIZE      15
#define HIGH_TEMP_SIZE          6
#define LOW_TEMP_SIZE           6
#define RAINFALL_CONTAINER_SIZE 8
#define RAIN_PRECIP_SIZE        6
#define WIND_DIRECTION_SIZE     5
#define WIND_DEGREE_SIZE        5
#define WIND_SPEED_SIZE         5
#define WIND_SCALE_SIZE         5
#define HUMIDITY_SIZE           5

// TimeInfoPackage 结构体的数组大小约束宏
#define TIMESTAMP_SIZE          30

typedef struct {
    char            dayWeather[DAY_WEATHER_SIZE];
    char            nightWeather[NIGHT_WEATHER_SIZE];
    char            highTemp[HIGH_TEMP_SIZE];
    char            lowTemp[LOW_TEMP_SIZE];
    struct {
        char            rainfallContainer[RAINFALL_CONTAINER_SIZE];
        char            rainPrecip[RAIN_PRECIP_SIZE];
    }rainy_info;
    struct {
        char            direction[WIND_DIRECTION_SIZE];
        char            degree[WIND_DEGREE_SIZE];
        char            speed[WIND_SPEED_SIZE];
        char            scale[WIND_SCALE_SIZE];
    }windy_info;      
    char            humidity[HUMIDITY_SIZE];   
} WeatherInfoPackage;

typedef struct {
    char    timestamp[TIMESTAMP_SIZE];
} TimeInfoPackage;

int weather_pack_from_json_string(const char* json_string, WeatherInfoPackage* pack);
int time_pack_from_json_string(const char* json_string, TimeInfoPackage* pack);


#endif