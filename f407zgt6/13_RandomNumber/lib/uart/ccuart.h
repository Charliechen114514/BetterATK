#ifndef UART_H
#define UART_H

/* uart typedef */
#include "gpio.h"
#include "stm32f4xx_hal_uart.h"

#define ENABLE_USART_INTR       1
#define MAX_TRACE_UART_LEN      1

#define UART_RX_BUFFER_LEN      (64)

typedef enum {
    UART_SYNC,
    UART_IT
}CCUART_Type;

typedef struct __ccuart CCUART;
typedef struct __ccuart{
    UART_HandleTypeDef      handles;
    uint32_t                time_out;
    CCUART_Type             type;
    void(*it_tx_callback)(struct __ccuart*);
    void(*it_rx_callback)(struct __ccuart*);
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

#define NO_USART_IRQ    (114514)

typedef struct {
    CCGPIOType         tx;
    CCGPIOType         rx;
    OpenClockPrivate    uart_gpioclk_enabled;
    /* using for gpio settings */
    uint32_t            alternatives;
    OpenClockPrivate    uart_self_enabled;
    IRQn_Type           irq_n;
}UART_AFGPIO_Info;

void configure_common_uart(
    CCUART*             uart,
    USART_TypeDef*      uart_sources,
    uint32_t            baud_rate,
    UART_AFGPIO_Info*   info,
    CCUART_Type         uart_type,
    uint32_t            time_out
);

void configure_uart_itcallback(
    CCUART*             uart,
    void(*it_tx_callback)(struct __ccuart*),
    void(*it_rx_callback)(struct __ccuart*)
);

void send_src_uart_sync(CCUART* uart, UARTSendBuffer* buffer);
void send_srcview_uart_sync(CCUART* uart, UARTSendBufferView* buffer_view);
void receive_src_uart_sync(CCUART* uart, UARTSendBuffer* buffer);
void receive_srcview_uart_sync(CCUART* uart, UARTSendBufferView* buffer_view);

void send_src_uart(CCUART* uart, UARTSendBuffer* buffer);
void send_srcview_uart(CCUART* uart, UARTSendBufferView* buffer_view);
void receive_src_uart(CCUART* uart, UARTSendBuffer* buffer);
void receive_srcview_uart(CCUART* uart, UARTSendBufferView* buffer_view);

void send_message(CCUART* uart, const char* message);

#endif