#include "weather_fetcher.h"
#include "parser_config.h"
#include "running_loggy.h"
#include <stdio.h>
/* Request Chips */
ESP8266_Handle esp8266_handle;

ESP8266_Handle *get_weather_fetcher_drive()
{
    return &esp8266_handle;
}

static uint8_t make_strict_app_request(
    ESP8266_Handle *handle,
    const char *on_tcp_connection, /* make tcp request */
    const char *get_url,
    char *container, int container_len)
{
    char tcp_connection_request[TCP_CONNECTION_REQUEST_BUFFER_N];
    snprintf(tcp_connection_request, TCP_CONNECTION_REQUEST_BUFFER_N,
        "AT+CIPSTART=\"TCP\",\"%s\",80", on_tcp_connection);
    ESP8266_CMDPackage package = {
        .ack = "OK",
        .cmd = tcp_connection_request,
        .timeout_try = TIMEOUT_TRY_MAX
    };
    uint8_t res = handle->operations->send_command(handle, &package);
    if(res != ESP8266_OK){
        send_runninglogger_to_uart("Error Occurs! "
            "Can not enter send tcp request on command: %s\r\n", on_tcp_connection);
        return 0;
    }
    send_runninglogger_to_uart("Success in sending TCP Request: %s", on_tcp_connection);

    res = handle->operations->enter_unvarnished(handle);
    if(res != ESP8266_OK){
        send_runninglogger_to_uart("Can not enter unvarnished! Repowered the System!\n");
        return 0;
    }

    send_runninglogger_to_uart("Successfully enter unvarnished!\r\n");

    snprintf(tcp_connection_request, TCP_CONNECTION_REQUEST_BUFFER_N,
        "GET %s", get_url);

    send_runninglogger_to_uart("About send request with command: %s\r\n", tcp_connection_request);
    handle->operations->fetch_from_remote(
        handle, tcp_connection_request , container, container_len);
    
    send_runninglogger_to_uart(
        "Done! Requests are followings: \r\n%s\r\n", container);

    handle->operations->leave_unvarnished(handle);
    package.cmd = "AT+CIPCLOSE";
    
    res = handle->operations->send_command(handle, &package);
    if(res != ESP8266_OK){
        send_runninglogger_to_uart("Failed To Quit the unvarnished!");
        return 0;
    }
    send_runninglogger_to_uart("session success!\r\n");
    return 1;
}

/* We has one ESP8266 to fetch the weather info */
uint8_t weather_info_session(ESP8266_Handle *who,
                          char *container, int container_len)
{
    return make_strict_app_request(
        who, WEATHER_REQUEST_TCP_URL,
        WEATHER_REQUEST_TCP_CMD_URL, 
        container, container_len
    );
}

uint8_t time_info_session(ESP8266_Handle *who,
                       char *container, int container_len)
{
    return make_strict_app_request(
        who, TIME_REQUEST_TCP_URL,
        TIME_REQUEST_TCP_CMD_URL, 
        container, container_len
    );  
}

#include <stdio.h>
#include "poweron_loggy.h"
#include "display_handler.h"
#include "parser_config.h"
#include "ccuart.h"
static void __openclk()
{
    __HAL_RCC_GPIOF_CLK_ENABLE();
}

static CCGPIOInitTypeDef rst = {
    .open_clock = __openclk,
    .port = GPIOF,
    .post_init = NULL,
    .type = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = GPIO_PIN_6,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH}};

ESP8266_Handle *init_weather_fetcher()
{
    CCDeviceHandler *display_handler = get_private_handler();
    if (init_esp8266_handle(&esp8266_handle, 115200, &rst) != ESP8266_OK)
    {
        on_log_infos(display_handler,
                     "Can not init the chips! See UART Helper for further informations!");
        send_logger_to_uart(
            "Can not init the chips! "
            "You possible forget link RST\r\n");
        while (1)
            ;
    }

    WiFi_Package packageOfMyWifi = {
        .pwd = WIFI_PASSWD,
        .ssid = WIFI_SSID};

    esp8266_handle.operations->reset(&esp8266_handle, ESP8266_HardReset);
    esp8266_handle.operations->set_mode(&esp8266_handle, ESP8266_StationMode);
    esp8266_handle.operations->reset(&esp8266_handle, ESP8266_SoftReset);
    esp8266_handle.operations->set_as_echo(&esp8266_handle, 0);
    on_log_infos(display_handler,
                 "About to Join WIFI...");
    send_logger_to_uart("About to Join WIFI...\r\n");
    ESP8266_ErrorCode code = esp8266_handle.operations->join_wifi(&esp8266_handle, &packageOfMyWifi);
    if (code != ESP8266_OK)
    {
        on_log_infos(display_handler,
                     "Failed To Join WIFI...");
        send_logger_to_uart(
            "Failed To Join WIFI!\r\n"
            "make sure the WIFI works in 2.4GHz and "
            "provide correct SSID and Password!\r\n");
        while (1)
            ;
    }

    char buffer[20];
    esp8266_handle.operations->fetch_ip_from_wifi(&esp8266_handle, buffer, 20);
    char display_buffer[40];
    snprintf(display_buffer, 40, "Fetch IP From Wifi: %s!", buffer);
    on_log_infos(display_handler, display_buffer);
    send_logger_to_uart(display_buffer);
    return &esp8266_handle;
}