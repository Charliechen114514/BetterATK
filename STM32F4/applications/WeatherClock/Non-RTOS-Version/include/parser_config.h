#ifndef PARSER_CONFIG_H
#define PARSER_CONFIG_H

#define WEATHER_APP_KEY
#define TIME_APP_KEY

#define WIFI_SSID           "Charlie"
#define WIFI_PASSWD         "chen0303"

#define WEATHER_REQUEST_TCP_URL               ("api.seniverse.com")
#define WEATHER_REQUEST_TCP_CMD_URL           ("https://api.seniverse.com/v3/weather/daily.json?key=SfRRwRAYLfFfTKQoi&location=changchun&language=en&unit=c&start=0&days=1")

#define TIME_REQUEST_TCP_URL                  ("api.k780.com")
#define TIME_REQUEST_TCP_CMD_URL              ("https://api.k780.com/?app=life.time&appkey=75871&sign=bf49082622ae0d1551c4496ebeb13b42&format=json")

#define RUNNING_LOGGY_N                       (1000)
#define POWERON_LOGGY_N                       (100)

#define TCP_CONNECTION_REQUEST_BUFFER_N       (200)


#define ERROR_BUFFER_N                        (100)
#define TIMEOUT_TRY_MAX                       (10000)



#endif