#ifndef POWERON_LOGGY_H
#define POWERON_LOGGY_H

typedef struct __DeviceProperty CCDeviceHandler;

void on_start_loggy(CCDeviceHandler* handler);
void on_log_infos(CCDeviceHandler* handler, char* loggy_info, ...);
void on_clear_loggy(CCDeviceHandler* handler);

void send_logger_to_uart(const char* loggy_info);

#endif