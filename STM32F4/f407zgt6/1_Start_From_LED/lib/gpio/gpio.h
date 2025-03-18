#ifndef GPIO_H
#define GPIO_H
#include "precheck.h"
#define NULL_POST_GPIO_INIT     ((void*)0)

typedef uint16_t    GPIO_PinType;
typedef struct __ccgpio{
    GPIO_TypeDef*       port;
    GPIO_PinType        pinType;
}CCGPIOType;

typedef void(*OpenClockPrivate)(void);
typedef void(*PostInit)(CCGPIOType*);

typedef struct __ccpgioinit{
    GPIO_InitTypeDef    type;
    GPIO_TypeDef*       port;
    PostInit            post_init;
    OpenClockPrivate    open_clock;
}CCGPIOInitTypeDef;

typedef enum {
    CCGPIO_LOW,
    CCGPIO_HIGH
}CCGPIOState;

void configure_ccgpio(
    CCGPIOType* type,  CCGPIOInitTypeDef* initer);

void set_ccgpio_state(CCGPIOType* type, CCGPIOState state);
void reverse_ccgpio_pinstate(CCGPIOType* type);
CCGPIOState fetch_sync_gpiostate(CCGPIOType* type);

#endif