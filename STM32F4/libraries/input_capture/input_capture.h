#ifndef INPUT_CAPTURE_H
#define INPUT_CAPTURE_H
#include "stm32f4xx_hal.h"
#define BOARDING_ICCAP_N            (1)

typedef struct __ic InputCapture;
typedef struct __ic
{
    TIM_HandleTypeDef handle;
    uint32_t channel;
    void(*callbacks)(InputCapture*);
} InputCapture;

typedef struct
{
    TIM_IC_InitTypeDef ic_config;
    uint32_t channel;
} InputCapturePack;

typedef enum
{
    IC_CAP_RISING,
    IC_CAP_FALLING,
    IC_CAP_BOTH
} CaptureTriggerMode;

void init_inputCapture(
    InputCapture *ic, TIM_TypeDef *who,
    TIM_Base_InitTypeDef *base_info,
    InputCapturePack *pack,
    void (*ic_post_init)(void));

void start_inputCapture(InputCapture *ic);
void stop_inputCapture(InputCapture *ic);
void config_capture_triggure(
    InputCapture *ic,
    CaptureTriggerMode mode);

uint32_t ic_captured_value(InputCapture *ic);
void register_ic_callback(InputCapture* ic, void(*callbacks)(InputCapture*));
#endif