#include <string.h>
#include "ccuart.h"

static void __die(void)
{
    while (1)
        ;
}

static UART_AFGPIO_Info *cur_infos;
CCUART*                 cur_uart;

extern CCUART* trace_array[MAX_TRACE_UART_LEN];
extern uint8_t trace_n;

static void __pvt_af_uart_gpio(UART_AFGPIO_Info *infos)
{
    cur_infos = infos;
}

static void __post_init(CCUART *uart, CCUART_Type types)
{
    uart->type = types;
    trace_array[trace_n] = uart;
    trace_n++;
    switch (types)
    {
    case UART_SYNC:
        break;
    case UART_IT:
#if ENABLE_USART_INTR
    HAL_NVIC_EnableIRQ(cur_infos->irq_n);
    HAL_NVIC_SetPriority(cur_infos->irq_n, 3, 3);
#else
        __die();
#endif
        break;
    }
}

void makeup_uartsend_buffer(UARTSendBuffer *buffer, char *src, uint16_t len)
{
    if (len == UART_EXTSRC_ALL)
        len = strlen(src);
    if (len > UART_RX_BUFFER_LEN)
        len = UART_RX_BUFFER_LEN;
    memset(buffer->sources, 0, UART_RX_BUFFER_LEN);
    strncpy(buffer->sources, src, len);
    buffer->indication_len = len;
}

void makeup_uartsendview(UARTSendBufferView *buffer, char *src, uint16_t len)
{
    buffer->sources = src;
    buffer->src_len = len;
}

void configure_common_uart(
    CCUART *uart,
    USART_TypeDef *uart_sources,
    uint32_t baud_rate,
    UART_AFGPIO_Info *info,
    CCUART_Type uart_type,
    uint32_t time_out)
{
    memset(&(uart->handles), 0, sizeof(uart->handles));
    uart->handles.Instance = uart_sources;
    uart->handles.Init.BaudRate = baud_rate;
    uart->handles.Init.WordLength = UART_WORDLENGTH_8B;
    uart->handles.Init.StopBits = UART_STOPBITS_1;
    uart->handles.Init.Parity = UART_PARITY_NONE;
    uart->handles.Init.Mode = UART_MODE_TX_RX;
    uart->handles.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart->handles.Init.OverSampling = UART_OVERSAMPLING_16;
    uart->time_out = time_out;
    __pvt_af_uart_gpio(info);
    if (HAL_UART_Init(&(uart->handles)) != HAL_OK)
    {
        __die();
    }
    __post_init(uart, uart_type);
    cur_uart = uart;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    /*
        TODO: This MspInit is seemingly thread unsafe,
        I hereby use the thread_local varables to prevent this
    */
    UART_AFGPIO_Info *current = cur_infos;
    if (!current->uart_self_enabled)
        __die();
    current->uart_self_enabled();
    CCGPIOInitTypeDef tx = {
        .open_clock = current->uart_gpioclk_enabled,
        .port = current->tx.port,
        .type = {
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_PULLUP,
            .Alternate = current->alternatives,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Pin = current->tx.pinType},
        .post_init = NULL};

    CCGPIOInitTypeDef rx = {
        .open_clock = current->uart_gpioclk_enabled,
        .port = current->rx.port,
        .type = {
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_NOPULL,
            .Alternate = current->alternatives,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Pin = current->rx.pinType},
        .post_init = NULL};

    configure_ccgpio(&(current->rx), &rx);
    configure_ccgpio(&(current->tx), &tx);
}

void send_src_uart_sync(CCUART *uart, UARTSendBuffer *buffer)
{
    HAL_UART_Transmit(
        &(uart->handles),
        (uint8_t *)(buffer->sources),
        buffer->indication_len,
        uart->time_out);
}

void send_srcview_uart_sync(CCUART *uart, UARTSendBufferView *buffer_view)
{
    HAL_UART_Transmit(
        &(uart->handles),
        (uint8_t *)(buffer_view->sources),
        buffer_view->src_len,
        uart->time_out);
}

void receive_src_uart_sync(CCUART *uart, UARTSendBuffer *buffer)
{
    HAL_UART_Receive(&(uart->handles),
                     (uint8_t *)(buffer->sources),
                     buffer->indication_len,
                     uart->time_out);
}

void receive_srcview_uart_sync(CCUART *uart, UARTSendBufferView *buffer_view)
{
    HAL_UART_Receive(&(uart->handles), 
    (uint8_t *)(buffer_view->sources), 
    buffer_view->src_len, uart->time_out);
}

void configure_uart_itcallback(
    CCUART*             uart,
    void(*it_tx_callback)(struct __ccuart*),
    void(*it_rx_callback)(struct __ccuart*)
)
{
    if(uart->type != UART_IT){
        __die();
    }

    uart->it_rx_callback = it_rx_callback;
    uart->it_tx_callback = it_tx_callback;
}

void send_src_uart(CCUART *uart, UARTSendBuffer *buffer)
{
    switch (uart->type)
    {
    case UART_SYNC:
        HAL_UART_Transmit(
            &(uart->handles),
            (uint8_t *)(buffer->sources),
            buffer->indication_len,
            uart->time_out);
        break;
    case UART_IT:
        HAL_UART_Transmit_IT(&(uart->handles),
                             (uint8_t *)(buffer->sources),
                             buffer->indication_len);
        break;
    }
}

void send_srcview_uart(CCUART *uart, UARTSendBufferView *buffer_view)
{
    switch (uart->type)
    {
    case UART_SYNC:
        HAL_UART_Transmit(
            &(uart->handles),
            (uint8_t *)(buffer_view->sources),
            buffer_view->src_len,
            uart->time_out);
        break;
    case UART_IT:
        HAL_UART_Transmit_IT(&(uart->handles),
                             (uint8_t *)(buffer_view->sources),
                             buffer_view->src_len);
        break;
    }
}

void receive_src_uart(CCUART *uart, UARTSendBuffer *buffer)
{
    switch (uart->type)
    {
    case UART_SYNC:
        HAL_UART_Receive(
            &(uart->handles),
            (uint8_t *)(buffer->sources),
            buffer->indication_len,
            uart->time_out);
        break;
    case UART_IT:
        HAL_UART_Receive_IT(&(uart->handles),
                            (uint8_t *)(buffer->sources),
                            buffer->indication_len);
        break;
    }
}

void receive_srcview_uart(CCUART *uart, UARTSendBufferView *buffer_view)
{
    switch (uart->type)
    {
    case UART_SYNC:
        HAL_UART_Receive(
            &(uart->handles),
            (uint8_t *)(buffer_view->sources),
            buffer_view->src_len,
            uart->time_out);
        break;
    case UART_IT:
        HAL_UART_Receive_IT(&(uart->handles),
                            (uint8_t *)(buffer_view->sources),
                            buffer_view->src_len);
        break;
    }
}

#include <string.h>
void send_message(CCUART* uart, const char* message)
{
    HAL_UART_Transmit(&uart->handles, (uint8_t*)message, strlen(message), uart->time_out);
}