#ifndef UART_H
#define UART_H

/* uart typedef */
#include "gpio.h"
#include "stm32f4xx_hal_uart.h"

#define UART_RX_BUFFER_LEN      (6)

typedef enum {
    UART_SYNC
}CCUART_Type;

typedef struct __ccuart CCUART;
typedef struct __ccuart{
    UART_HandleTypeDef      handles;
    uint32_t                time_out;
    CCUART_Type             type;
}CCUART;

typedef struct __sendeebufview {
    char*       sources;
    uint32_t    src_len;
}UARTSendBufferView;

typedef struct __sendbuffer {
    char        sources[UART_RX_BUFFER_LEN];
    uint16_t    indication_len;
}UARTSendBuffer;
#define UART_EXTSRC_ALL     (0)
void makeup_uartsend_buffer(UARTSendBuffer* buffer, char* src, uint16_t len);
void makeup_uartsendview(UARTSendBufferView* buffer, char* src, uint16_t len);

typedef struct {
    CCGPIOType         tx;
    CCGPIOType         rx;
    OpenClockPrivate    uart_gpioclk_enabled;
    /* using for gpio settings */
    uint32_t            alternatives;
    OpenClockPrivate    uart_self_enabled;
}UART_AFGPIO_Info;

void configure_common_uart(
    CCUART*             uart,
    USART_TypeDef*      uart_sources,
    uint32_t            baud_rate,
    UART_AFGPIO_Info*   info,
    CCUART_Type         uart_type,
    uint32_t            time_out
);

void send_src_uart_sync(CCUART* uart, UARTSendBuffer* buffer);
void send_srcview_uart_sync(CCUART* uart, UARTSendBufferView* buffer_view);
void receive_src_uart_sync(CCUART* uart, UARTSendBuffer* buffer);
void receive_srcview_uart_sync(CCUART* uart, UARTSendBufferView* buffer_view);

#endif