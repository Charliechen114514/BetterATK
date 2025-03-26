#ifndef ESP8266_H
#define ESP8266_H
#include "esp8266_config.h"
#include "esp8266_uart.h"
#include "gpio.h"
typedef enum {
    ESP8266_OK      = 0,
    ESP8266_ERROR   = 1,
    ESP8266_TIMEOUT = 2,
    ESP8266_EINVAL  = 3
}ESP8266_ErrorCode;

typedef enum {
    ESP8266_APMode = 2,
    ESP8266_StationMode = 1,
    ESP8266_MixMode = 3
}ESP8266_WorkMode;

typedef enum {
    ESP8266_SoftReset,
    ESP8266_HardReset
}ESP8266_ResetMethod;

typedef struct __esp8266_handle ESP8266_Handle;

typedef struct {
    char*           cmd;
    char*           ack;
    uint32_t        timeout_try;
}ESP8266_CMDPackage;

typedef struct {
    char*           ssid;
    char*           pwd;
}WiFi_Package;

typedef struct {
    char*           ip;
    char*           port;
}TCPServerInfo;

typedef struct {
    /* send command promised the base method */
    uint8_t     (*send_command)(ESP8266_Handle* handle, ESP8266_CMDPackage* pack);
    uint8_t     (*reset)(ESP8266_Handle* handle, ESP8266_ResetMethod method);
    uint8_t     (*test_enable)(ESP8266_Handle* handle);
    uint8_t     (*set_mode)(ESP8266_Handle* handle, ESP8266_WorkMode mode);
    uint8_t     (*set_as_echo)(ESP8266_Handle* handle, uint8_t status_wish);
    uint8_t     (*join_wifi)(ESP8266_Handle* handle, WiFi_Package* package);
    uint8_t     (*fetch_ip_from_wifi)(ESP8266_Handle* handle, char* container, uint8_t buffer_len);
    uint8_t     (*connect_tcp_server)(ESP8266_Handle* handle, TCPServerInfo* server_info);
    uint8_t     (*enter_unvarnished)(ESP8266_Handle* handle);
    uint8_t     (*leave_unvarnished)(ESP8266_Handle* handle);
    void        (*fetch_from_remote)(ESP8266_Handle* handle,const char* cmd, char* buffer, uint16_t buflen);
    void        (*refresh_fetch)(ESP8266_Handle* handle);
}ESP8266_Operations;

typedef struct __esp8266_handle{
    ESP8266_UARTHandle      handle;
    ESP8266_Operations*     operations;
    CCGPIOInitTypeDef       rst_gpio;
}ESP8266_Handle;

uint8_t init_esp8266_handle(
    ESP8266_Handle*         handle, 
    uint32_t                baudrate,
    CCGPIOInitTypeDef*      rst_package);



#endif