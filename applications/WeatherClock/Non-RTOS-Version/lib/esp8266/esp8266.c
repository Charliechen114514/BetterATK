#include "esp8266.h"
#include "system_clock.h"
#include <string.h>
#include <stdio.h>

static uint8_t send_basic_command(ESP8266_Handle *handle, ESP8266_CMDPackage *pack)
{
    handle->handle.operations->reset(&handle->handle);
    send_esp8266_uart_info(&handle->handle, "%s\r\n", pack->cmd);

    if (!pack->ack || pack->timeout_try == 0)
    {
        return ESP8266_OK;
    }

    uint32_t timeout_try = pack->timeout_try;
    uint8_t *receivings = NULL;
    while (timeout_try > 0)
    {
        receivings = handle->handle.operations->get_internal_frame(&handle->handle);
        if (receivings)
        {
            if (strstr((const char *)receivings, pack->ack) != NULL)
            {
                return ESP8266_OK;
            }
            else
            {
                handle->handle.operations->reset(&handle->handle);
            }
        }
        timeout_try--;
        system_delay_ms(1);
    }

    return ESP8266_TIMEOUT;
}

static void fetch_from_remote(
    ESP8266_Handle *handle, const char *cmd, char *buffer, uint16_t buflen)
{
    handle->handle.operations->reset(&handle->handle);
    send_esp8266_uart_info(&handle->handle, "%s\r\n", cmd);
    
    uint32_t timeout_try = 10000;
    uint8_t *receivings = NULL;
    while (timeout_try > 0)
    {
        receivings = handle->handle.operations->get_internal_frame(&handle->handle);
        if (receivings)
        {
            uint16_t rx_buffer_len = handle->handle.operations->get_internal_frame_len(&handle->handle);
            uint16_t fin_len = ((rx_buffer_len > buflen) ? buflen : rx_buffer_len);
            strncpy(buffer, (const char*)receivings, fin_len);
            return;
        }
        timeout_try--;
        system_delay_ms(1);
    }
}

static uint8_t reset_hard(ESP8266_Handle *handle)
{
    ESP8266_CMDPackage package = {
        .ack = "ready",
        .cmd = "AT+RESTORE",
        .timeout_try = 3000};
    return send_basic_command(handle, &package);
}

static uint8_t reset_soft(ESP8266_Handle *handle)
{
    ESP8266_ErrorCode code;
    ESP8266_CMDPackage pack = {
        .ack = "OK",
        .cmd = "AT+RST",
        .timeout_try = 500};
    code = send_basic_command(handle, &pack);
    if (code == ESP8266_OK)
    {
        system_delay_ms(1000);
    }
    return code;
}

static uint8_t reset_method(ESP8266_Handle *handle, ESP8266_ResetMethod method)
{
    ESP8266_ErrorCode code = ESP8266_ERROR;
    switch (method)
    {
    case ESP8266_HardReset:
        code = reset_hard(handle);break;
    case ESP8266_SoftReset:
        code = reset_soft(handle);break;
    }
    return code;
}

static uint8_t test_enable(ESP8266_Handle *handle)
{
    ESP8266_CMDPackage package = {
        .ack = "OK",
        .cmd = "AT",
        .timeout_try = 500};
    ESP8266_ErrorCode code;
    for (uint8_t i = 0; i < 10; i++)
    {
        code = send_basic_command(handle, &package);
        if (code == ESP8266_OK)
        {
            return code;
        }
    }
    return code;
}

static uint8_t set_echo_status(ESP8266_Handle *handle, uint8_t status)
{
    char *cmd = (status) ? "ATE0" : "ATE1";
    ESP8266_CMDPackage pack = {
        .ack = "OK",
        .cmd = cmd,
        .timeout_try = 500};
    return send_basic_command(handle, &pack);
}

static uint8_t join_wifi(ESP8266_Handle *handle, WiFi_Package *pack)
{
    char final_cmd[128];
    snprintf(final_cmd, 128, "AT+CWJAP=\"%s\",\"%s\"", pack->ssid, pack->pwd);
    ESP8266_CMDPackage __pack = {
        .ack = "WIFI GOT IP",
        .cmd = final_cmd,
        .timeout_try = 10000};
    ESP8266_ErrorCode code = send_basic_command(handle, &__pack);
    // must a delay, else we won't get next operations
    system_delay_ms(100);
    return code;
}

static uint8_t fetch_ip(ESP8266_Handle *handle, char *container, uint8_t buffer_len)
{
    ESP8266_ErrorCode ret;
    char *p_start = NULL;
    char *p_end = NULL;

    ESP8266_CMDPackage pack = {
        .ack = "OK",
        .cmd = "AT+CIFSR",
        .timeout_try = 500};

    ret = send_basic_command(handle, &pack);
    if (ret != ESP8266_OK)
    {
        return ESP8266_ERROR;
    }
    system_delay_ms(100);
    p_start = strstr((const char *)handle->handle.operations->get_internal_frame(&handle->handle), "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    snprintf(container, buffer_len, "%s", p_start + 1);
    return ESP8266_OK;
}

static uint8_t enter_unvarnished(ESP8266_Handle *handle)
{
    ESP8266_CMDPackage pack = {
        .ack = "OK",
        .cmd = "AT+CIPMODE=1",
        .timeout_try = 500};
    ESP8266_ErrorCode code = send_basic_command(handle, &pack);
    if (code != ESP8266_OK)
    {
        return code;
    }
    pack.cmd = "AT+CIPSEND";
    pack.ack = ">";
    return send_basic_command(handle, &pack);
}

static uint8_t leave_unvarnished(ESP8266_Handle *handle)
{
    send_esp8266_uart_info(&handle->handle, "+++");
    system_delay_ms(100);
    return ESP8266_OK;
}

static uint8_t connect_tcp(ESP8266_Handle *handle, TCPServerInfo *info)
{
    char cmd[128];
    snprintf(cmd, 128, "AT+CIPSTART=\"TCP\",\"%s\",%s", info->ip, info->port);
    ESP8266_CMDPackage pack = {
        .ack = "CONNECT",
        .cmd = cmd,
        .timeout_try = 500};
    return send_basic_command(handle, &pack);
}

static uint8_t set_mode(ESP8266_Handle* handle, ESP8266_WorkMode mode)
{
    int _mode = (int)mode;
    char buffer[128];
    snprintf(buffer, 128, "AT+CWMODE=%d", _mode);
    ESP8266_CMDPackage pack = {
        .ack = "OK",
        .cmd = buffer,
        .timeout_try = 500};    
    return send_basic_command(handle, &pack);
}

static void refresh_fetch(ESP8266_Handle* handle)
{
    handle->handle.operations->reset(&handle->handle);
}

static ESP8266_Operations op = {
    .send_command = send_basic_command,
    .reset = reset_method,
    .test_enable = test_enable,
    .set_mode = set_mode,
    .set_as_echo = set_echo_status,
    .join_wifi = join_wifi,
    .fetch_ip_from_wifi = fetch_ip,
    .connect_tcp_server = connect_tcp,
    .enter_unvarnished = enter_unvarnished,
    .leave_unvarnished = leave_unvarnished,
    .fetch_from_remote = fetch_from_remote,
    .refresh_fetch = refresh_fetch
};

static void __init_rst_gpio(ESP8266_Handle *handle, CCGPIOInitTypeDef *gpio)
{
    CCGPIOType tmp;
    configure_ccgpio(&tmp, gpio);
}

static void __hardware_reset(ESP8266_Handle *handle, CCGPIOInitTypeDef *gpio)
{
    CCGPIOType type;
    type.pinType = gpio->type.Pin;
    type.port = gpio->port;
    set_ccgpio_state(&type, CCGPIO_LOW);
    system_delay_ms(100);
    set_ccgpio_state(&type, CCGPIO_HIGH);
    system_delay_ms(500);
}

uint8_t init_esp8266_handle(
    ESP8266_Handle *handle,
    uint32_t baudrate,
    CCGPIOInitTypeDef *rst_package)
{
    memset(handle, 0, sizeof(ESP8266_Handle));
    __init_rst_gpio(handle, rst_package);
    __hardware_reset(handle, rst_package);
    init_esp8266_uart_handle(&handle->handle, baudrate);
    handle->operations = &op;
    handle->rst_gpio = *rst_package;
    return handle->operations->test_enable(handle);
}