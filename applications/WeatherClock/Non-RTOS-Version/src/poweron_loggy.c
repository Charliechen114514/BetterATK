#include "poweron_loggy.h"
#include "parser_config.h"
#include "ccgraphic/CCGraphic_device_adapter.h"
#include "ccgraphic/sim_app/simple_text.h"
#include <stdarg.h>
#include <stdio.h>
extern CCGraphicTextEdit text;
char poweron_loggy_buffer[POWERON_LOGGY_N];


void on_start_loggy(CCDeviceHandler* handler){
    set_simple_text(&text, handler, "Successfully Early init!");
}

void on_log_infos(CCDeviceHandler* handler, char* loggy_info, ...)
{
    va_list ap;
    va_start(ap, loggy_info);
    vsprintf(poweron_loggy_buffer, loggy_info, ap);
    va_end(ap);
    
    set_newline_text(&text, handler, poweron_loggy_buffer);
}

void on_clear_loggy(CCDeviceHandler* handler)
{
    handler->operations.clear_device_function(handler);
    reset_to_header(&text);
}

#include "ccuart.h"
extern CCUART uart1;
void send_logger_to_uart(const char* loggy_info)
{
    send_message(&uart1, loggy_info);
}