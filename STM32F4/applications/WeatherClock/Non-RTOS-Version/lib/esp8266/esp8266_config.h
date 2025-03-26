#ifndef ESP8266_CONFIG_H
#define ESP8266_CONFIG_H

#ifdef STM32F4
    #include "stm32f4xx_hal.h"
#else
    #error  "Undefined Chips, not supported!"
#endif

// link with ESP8266's RX
#define ESP8266_UART_TX_GPIO_PORT           GPIOD
#define ESP8266_UART_TX_GPIO_PIN            GPIO_PIN_5
#define ESP8266_UART_TX_GPIO_AF             GPIO_AF7_USART2
#define ESP8266_UART_TX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* PB口时钟使能 */

// link with ESP8266's TX
#define ESP8266_UART_RX_GPIO_PORT           GPIOD
#define ESP8266_UART_RX_GPIO_PIN            GPIO_PIN_6
#define ESP8266_UART_RX_GPIO_AF             GPIO_AF7_USART2
#define ESP8266_UART_RX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* PB口时钟使能 */

/* UART Sources Settings */
#define ESP8266_UART_NAME       (USART2)
#define ESP8266_UART_IRQn       USART2_IRQn
#define ESP8266_UART_IRQHandler USART2_IRQHandler
#define ESP8266_UART_CLK_Enable() do{ __HAL_RCC_USART2_CLK_ENABLE(); }while(0)


/* Frame Settings */
#define INTERNAL_ESP8266UART_RX_BUFFER  (1000)
#define INTERNAL_ESP8266UART_TX_BUFFER  (128)




#endif