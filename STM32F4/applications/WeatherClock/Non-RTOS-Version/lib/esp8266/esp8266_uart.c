#include "esp8266_uart.h"
#include "esp8266_config.h"
#include "gpio.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct __esp8266_uart_frame_protocol {
    uint8_t rx_buffer[INTERNAL_ESP8266UART_RX_BUFFER];
    uint8_t tx_buffer[INTERNAL_ESP8266UART_TX_BUFFER];
    struct 
    {
        uint32_t    frame_len;
        uint8_t     is_finished;
    }frame_status;
}ESP8288_UARTFrameProtocol;

static ESP8288_UARTFrameProtocol    global_frame;
static ESP8266_UARTHandle*          current;

uint8_t* __pvt_get_frame_self(ESP8266_UARTHandle* handle){
    
    if(handle->pvt_frame->frame_status.is_finished){
        handle->pvt_frame->rx_buffer[handle->pvt_frame->frame_status.frame_len] = '\0';
        return handle->pvt_frame->rx_buffer;
    }
    return NULL;
}

uint16_t __pvt_get_frame_len(ESP8266_UARTHandle* handle)
{
    return handle->pvt_frame->frame_status.is_finished ? 
        handle->pvt_frame->frame_status.frame_len : 0;
}

static void reset_the_handle(ESP8266_UARTHandle* handle)
{
    handle->pvt_frame->frame_status.frame_len = 0;
    handle->pvt_frame->frame_status.is_finished = 0;
}

static ESP8266_UARTHandleOperations operations = {
    .get_internal_frame = __pvt_get_frame_self,
    .get_internal_frame_len = __pvt_get_frame_len,
    .reset = reset_the_handle
};

static void allocate_global_frame(ESP8266_UARTHandle* handle){
    handle->pvt_frame   = &global_frame;
}



void init_esp8266_uart_handle(
    ESP8266_UARTHandle* handle, uint32_t    uart_baudrate)
{
    memset(handle, 0, sizeof(ESP8266_UARTHandle));
    allocate_global_frame(handle);
    current = handle;
    handle->operations =    &operations;
    handle->uart_handle.Instance            = ESP8266_UART_NAME;
    handle->uart_handle.Init.BaudRate       = uart_baudrate;
    handle->uart_handle.Init.WordLength     = UART_WORDLENGTH_8B;          
    handle->uart_handle.Init.StopBits       = UART_STOPBITS_1;             
    handle->uart_handle.Init.Parity         = UART_PARITY_NONE;             
    handle->uart_handle.Init.Mode           = UART_MODE_TX_RX;             
    handle->uart_handle.Init.HwFlowCtl      = UART_HWCONTROL_NONE;          
    handle->uart_handle.Init.OverSampling   = UART_OVERSAMPLING_16;

    HAL_UART_Init(&handle->uart_handle);
}

void send_esp8266_uart_info(ESP8266_UARTHandle* handle, char* format, ...)
{
    va_list ap;

    va_start(ap, format);
    vsprintf((char *)handle->pvt_frame->tx_buffer, format, ap);
    va_end(ap);

    uint32_t len = strlen((const char*)handle->pvt_frame->tx_buffer);
    HAL_UART_Transmit(&handle->uart_handle, handle->pvt_frame->tx_buffer, len, HAL_MAX_DELAY);
}

static void __open_tx_clk(){
    ESP8266_UART_TX_GPIO_CLK_ENABLE();
}

static void __open_rx_clk(){
    ESP8266_UART_RX_GPIO_CLK_ENABLE();
}

/* post init */
void esp8266_uart_post_init(UART_HandleTypeDef *huart)
{
    CCGPIOInitTypeDef tx = {
        .open_clock = __open_tx_clk,
        .post_init = NULL,
        .port = ESP8266_UART_TX_GPIO_PORT,
        .type = {
          .Pin = ESP8266_UART_TX_GPIO_PIN,
          .Mode = GPIO_MODE_AF_PP,
          .Alternate = ESP8266_UART_TX_GPIO_AF,
          .Speed = GPIO_SPEED_FREQ_HIGH,
          .Pull = GPIO_NOPULL
        }
      };

      CCGPIOInitTypeDef rx = tx;
      rx.port = ESP8266_UART_RX_GPIO_PORT;
      rx.type.Pin = ESP8266_UART_RX_GPIO_PIN;
      rx.open_clock = __open_rx_clk;
      CCGPIOType  _rx, _tx;
      configure_ccgpio(&_tx, &tx);
      configure_ccgpio(&_rx, &rx);

      ESP8266_UART_CLK_Enable();   
      HAL_NVIC_SetPriority(ESP8266_UART_IRQn, 0, 0); 
      HAL_NVIC_EnableIRQ(ESP8266_UART_IRQn);         

      __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE); 
      __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE); 
}

/* interrpt handle */
void esp8266_uart_handle(void)
{
    uint8_t tmp;
    
    if (__HAL_UART_GET_FLAG(&current->uart_handle, UART_FLAG_ORE) != RESET)        /* UART接收过载错误中断 */
    {
        __HAL_UART_CLEAR_OREFLAG(&current->uart_handle);                           /* 清除接收过载错误中断标志 */
        (void)current->uart_handle.Instance->SR;                                   /* 先读SR寄存器，再读DR寄存器 */
        (void)current->uart_handle.Instance->DR;
    }
    
    if (__HAL_UART_GET_FLAG(&current->uart_handle, UART_FLAG_RXNE) != RESET)       /* UART接收中断 */
    {
        HAL_UART_Receive(&current->uart_handle, &tmp, 1, HAL_MAX_DELAY);           /* UART接收数据 */
        
        if (current->pvt_frame->frame_status.frame_len >= (INTERNAL_ESP8266UART_RX_BUFFER - 1))
        {
            current->pvt_frame->frame_status.frame_len = 0;    
        }
        current->pvt_frame->rx_buffer[
            current->pvt_frame->frame_status.frame_len] = tmp;
        current->pvt_frame->frame_status.frame_len++;
    }

    if (__HAL_UART_GET_FLAG(&current->uart_handle, UART_FLAG_IDLE) != RESET)       
    {
        current->pvt_frame->frame_status.is_finished = 1;                                      
        __HAL_UART_CLEAR_IDLEFLAG(&current->uart_handle);                          
    }
}







