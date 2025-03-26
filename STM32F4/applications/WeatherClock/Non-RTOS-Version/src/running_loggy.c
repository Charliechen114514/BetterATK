#include "poweron_loggy.h"
#include "ccgraphic/CCGraphic_device_adapter.h"
#include "ccgraphic/sim_app/simple_text.h"
#include "parser_config.h"
#include "ccuart.h"
#include <stdarg.h>
#include <stdio.h>

extern CCUART uart1;
char running_loggy_buffer[RUNNING_LOGGY_N];

void send_runninglogger_to_uart(const char* loggy_info, ...)
{
    va_list ap;

    va_start(ap, loggy_info);
    vsprintf(running_loggy_buffer, loggy_info, ap);
    va_end(ap);

    send_message(&uart1, running_loggy_buffer);
}