#ifndef ESP8266_UART_H
#define ESP8266_UART_H
#include "esp8266_config.h"

typedef struct __esp8266_uart_frame_protocol    ESP8288_UARTFrameProtocol;
typedef struct __esp8266_uart_handle            ESP8266_UARTHandle;
typedef struct {
    uint8_t*(*get_internal_frame)(ESP8266_UARTHandle* handle);
    uint16_t(*get_internal_frame_len)(ESP8266_UARTHandle* handle);
    void(*reset)(ESP8266_UARTHandle* handle);
}ESP8266_UARTHandleOperations;

typedef struct __esp8266_uart_handle{
    ESP8288_UARTFrameProtocol*      pvt_frame;
    UART_HandleTypeDef              uart_handle;
    ESP8266_UARTHandleOperations*   operations;
}ESP8266_UARTHandle;

void send_esp8266_uart_info(ESP8266_UARTHandle* handle, char* format, ...);
void init_esp8266_uart_handle(
    ESP8266_UARTHandle* handle, uint32_t    uart_baudrate);


#endif