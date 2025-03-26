#include "json_parser.h"
#include "cJSON.h"
#include "running_loggy.h"
#include <string.h>
int weather_pack_from_json_string(const char *json_string, WeatherInfoPackage *pack)
{
    // parse if is Json
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        send_runninglogger_to_uart(error_ptr);
        return 0;
    }

    // fetch resules
    cJSON *results = cJSON_GetObjectItemCaseSensitive(root, "results");
    if (results == NULL || !cJSON_IsArray(results))
    {
        send_runninglogger_to_uart("Results not found or not an array\n");
        cJSON_Delete(root);
        return 0;
    }

    // iterate the result
    cJSON *result = cJSON_GetArrayItem(results, 0);
    if (result == NULL)
    {
        send_runninglogger_to_uart("No result found in array\n");
        cJSON_Delete(root);
        return 0;
    }

    // parse daily
    cJSON *daily = cJSON_GetObjectItemCaseSensitive(result, "daily");
    if (daily != NULL && cJSON_IsArray(daily))
    {
        int daily_count = cJSON_GetArraySize(daily);
        for (int i = 0; i < daily_count; i++)
        {
            cJSON *day = cJSON_GetArrayItem(daily, i);

            cJSON *text_day = cJSON_GetObjectItemCaseSensitive(day, "text_day");
            cJSON *text_night = cJSON_GetObjectItemCaseSensitive(day, "text_night");

            cJSON *high = cJSON_GetObjectItemCaseSensitive(day, "high");
            cJSON *low = cJSON_GetObjectItemCaseSensitive(day, "low");
            cJSON *rainfall = cJSON_GetObjectItemCaseSensitive(day, "rainfall");
            cJSON *precip = cJSON_GetObjectItemCaseSensitive(day, "precip");
            cJSON *wind_direction =
                cJSON_GetObjectItemCaseSensitive(day, "wind_direction");
            cJSON *wind_direction_degree =
                cJSON_GetObjectItemCaseSensitive(day, "wind_direction_degree");
            cJSON *wind_speed = cJSON_GetObjectItemCaseSensitive(day, "wind_speed");
            cJSON *wind_scale = cJSON_GetObjectItemCaseSensitive(day, "wind_scale");
            cJSON *humidity = cJSON_GetObjectItemCaseSensitive(day, "humidity");

            strncpy(pack->dayWeather, text_day->valuestring, DAY_WEATHER_SIZE);
            strncpy(pack->nightWeather, text_night->valuestring, NIGHT_WEATHER_SIZE);
            strncpy(pack->highTemp, high->valuestring, HIGH_TEMP_SIZE);
            strncpy(pack->lowTemp, low->valuestring, LOW_TEMP_SIZE);
            strncpy(pack->rainy_info.rainfallContainer, rainfall->valuestring, RAINFALL_CONTAINER_SIZE);
            strncpy(pack->rainy_info.rainPrecip, precip->valuestring, RAIN_PRECIP_SIZE);
            strncpy(pack->windy_info.direction, wind_direction->valuestring, WIND_DIRECTION_SIZE);
            strncpy(pack->windy_info.speed, wind_speed->valuestring, WIND_SPEED_SIZE);
            strncpy(pack->windy_info.degree, wind_direction_degree->valuestring, WIND_DEGREE_SIZE);
            strncpy(pack->windy_info.scale, wind_scale->valuestring, WIND_SCALE_SIZE);
            strncpy(pack->humidity, humidity->valuestring, HUMIDITY_SIZE);
        }
    }

    cJSON_Delete(root);
    return 1;
}

int time_pack_from_json_string(const char *json_string, TimeInfoPackage *pack)
{
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            send_runninglogger_to_uart("Error before: %s\n", error_ptr);
        }
        return 0;
    }

    cJSON *success = cJSON_GetObjectItemCaseSensitive(root, "success");
    if (success == NULL || !cJSON_IsString(success))
    {
        send_runninglogger_to_uart("Success field not found or not a string\n");
        cJSON_Delete(root);
        return 0;
    }

    send_runninglogger_to_uart("success status: %s\n", success->valuestring);
    if (strcmp(success->valuestring, "1") != 0)
    {
        send_runninglogger_to_uart("Failed To Fetch the time issue!");
    }

    cJSON *result = cJSON_GetObjectItemCaseSensitive(root, "result");
    if (result == NULL || !cJSON_IsObject(result))
    {
        send_runninglogger_to_uart("Can not fetch result!\r\n");
        cJSON_Delete(root);
        return 0;
    }

    cJSON *datetime_1 = cJSON_GetObjectItemCaseSensitive(result, "datetime_1");

    send_runninglogger_to_uart("fetch time: %s\n", datetime_1->valuestring);
    strncpy(pack->timestamp, datetime_1->valuestring, TIMESTAMP_SIZE);
    cJSON_Delete(root);
    return 1;
}




