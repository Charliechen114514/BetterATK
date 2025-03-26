/*
    Main Entry for the key detections
*/
#include "ccuart.h"
#include "system_clock.h"
#include "gpio.h"

static void __enable_uartgpio()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

static void __enable_uart()
{
    __HAL_RCC_USART1_CLK_ENABLE();
}

UART_AFGPIO_Info info = {
    .alternatives = GPIO_AF7_USART1,
    .tx = {
        .pinType = GPIO_PIN_9,
        .port = GPIOA},
    .rx = {.pinType = GPIO_PIN_10, .port = GPIOA},
    .uart_gpioclk_enabled = __enable_uartgpio,
    .uart_self_enabled = __enable_uart,
    .irq_n = USART1_IRQn,
};

UARTSendBuffer buffer;
CCGPIOType led1;
UARTSendBuffer trigger_receive;
UARTSendBufferView view;
int receive = 0;
char sender[10] = {0};

#include "esp8266.h"

static void __openclk()
{
    __HAL_RCC_GPIOF_CLK_ENABLE();
}
#define PLACE_GPIOTypeDef(name, __port, __pin, clk) \
    CCGPIOInitTypeDef name = {                      \
        .open_clock = clk,                          \
        .post_init = NULL,                          \
        .port = __port,                             \
        .type = {                                   \
            .Pin = __pin,                           \
            .Mode = GPIO_MODE_OUTPUT_PP,            \
            .Pull = GPIO_NOPULL,                    \
            .Speed = GPIO_SPEED_FREQ_HIGH}};

// PLACE_GPIOTypeDef(rst, GPIO_PIN_6, GPIOF, __openclk);

CCGPIOInitTypeDef rst = {
    .open_clock = __openclk,
    .port = GPIOF,
    .post_init = NULL,
    .type = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = GPIO_PIN_6,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH}};

static char messages[1500];
char weather_command[] = "GET https://api.seniverse.com/v3/weather/daily.json?key=SfRRwRAYLfFfTKQoi&location=changchun&language=en&unit=c&start=0&days=1";
ESP8266_Handle handle;

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);

    CCUART uart1;
    
    configure_common_uart(&uart1, USART1, 115200, &info, UART_SYNC, HAL_MAX_DELAY);
    send_message(&uart1, "Try to press something!\n");
    if (init_esp8266_handle(&handle, 115200, &rst) != ESP8266_OK)
    {
        send_message(&uart1, "Error in initings!\n");
        while (1)
            ;
    }

    send_message(&uart1, "OK\n");

    WiFi_Package packageOfMyWifi = {
        .pwd = "chen0303",
        .ssid = "Charlie"};

    send_message(&uart1, "Trying to engage in the Wifi in phone...");

    handle.operations->reset(&handle, ESP8266_HardReset);
    handle.operations->set_mode(&handle, ESP8266_StationMode);
    handle.operations->reset(&handle, ESP8266_SoftReset);
    handle.operations->set_as_echo(&handle, 0);
    handle.operations->join_wifi(&handle, &packageOfMyWifi);

    char buffer[20];
    handle.operations->fetch_ip_from_wifi(&handle, buffer, 20);
    send_message(&uart1, "\n");
    send_message(&uart1, buffer);

    ESP8266_CMDPackage pack = {
        .ack = "OK",
        .cmd = "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80",
        .timeout_try = 10000
    };
    uint8_t res = handle.operations->send_command(&handle, &pack);
    if(res != ESP8266_OK){
        send_message(&uart1, "Can not enter send tcp request\n");
        while(1);
    }
    send_message(&uart1, "OK in tcp setup: ");
    send_message(&uart1, pack.cmd);
    send_message(&uart1, "\n");
    
    res = handle.operations->enter_unvarnished(&handle);
    if(res != ESP8266_OK){
        send_message(&uart1, "Can not enter unvarnished!\n");
        while(1);
    }

    handle.operations->fetch_from_remote(
        &handle, weather_command , messages, 1500);
    send_message(&uart1, "send: ");
    send_message(&uart1, weather_command);
    send_message(&uart1, "\n");
    send_message(&uart1, messages);
    send_message(&uart1, "done!\n");
    res = handle.operations->leave_unvarnished(&handle);
    pack.cmd = "AT+CIPCLOSE";
    res = handle.operations->send_command(&handle, &pack);
    if(res != ESP8266_OK){
        send_message(&uart1, "Can not quit\n");
        while(1);
    }
    send_message(&uart1, "quit success\n");
    while (1)
    {
    }

    return 0;
}
