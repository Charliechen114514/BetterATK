#include "ccuart.h"

CCUART *trace_array[MAX_TRACE_UART_LEN];
uint8_t trace_n;

#if ENABLE_USART_INTR

void USART1_IRQHandler(void)
{
    for (uint8_t i = 0; i < trace_n; i++)
    {
        if (trace_array[i]->handles.Instance == USART1)
        {
            HAL_UART_IRQHandler(&trace_array[i]->handles);
        }
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < trace_n; i++)
    {
        if (huart == &trace_array[i]->handles)
        {
            if (trace_array[i]->it_rx_callback)
                trace_array[i]->it_rx_callback(trace_array[i]);
            return;
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < trace_n; i++)
    {
        if (huart == &trace_array[i]->handles)
        {
            if (trace_array[i]->it_tx_callback)
                trace_array[i]->it_tx_callback(trace_array[i]);
            return;
        }
    }
}

#include "esp8266.h"
#include "esp8266_config.h"
extern ESP8266_UARTHandle*          current;
void esp8266_uart_handle();

void ESP8266_UART_IRQHandler(void)
{
    esp8266_uart_handle();
}


extern UART_AFGPIO_Info *cur_infos;
extern CCUART *cur_uart;
void uart1_post_init();
void esp8266_uart_post_init(UART_HandleTypeDef *huart);

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance != ESP8266_UART_NAME)
    {
        uart1_post_init();
    }
    else
    {
        esp8266_uart_post_init(huart);
    }
}

#endif