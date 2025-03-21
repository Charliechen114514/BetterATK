#ifndef KEY_H
#define KEY_H


/* Key Ports */
#include "gpio.h"

#define DEFAULT_KEYMODE     (KEY_LONG_PRESS)

typedef enum{
    KEY_SHORT_PRESS,
    KEY_LONG_PRESS
}KeyDetectMode;

typedef enum{
    KEY_UNPRESS,
    KEY_PRESS
}KeyPressStatus;

typedef struct __keyport KeyPort;

typedef void(*KeyCallBack)(KeyPort*);
typedef KeyPressStatus(*KeyDetect)(KeyPort*);

typedef struct __keyport {
    CCGPIOType      gpio_core;
    KeyCallBack     key_callback;
    KeyDetect       key_detect;
}KeyPort;

void init_key(
    KeyPort* port, KeyCallBack callback, 
    KeyDetect detect, CCGPIOInitTypeDef* init);


uint8_t isKeyPress(KeyPort* key, KeyDetectMode mode);

void doKeyCallbackIfDone(KeyPort* key, KeyDetectMode mode);


KeyPort* isKeyPressInGroup(KeyPort* keys[],uint8_t key_n, KeyDetectMode mode);

void doCallback_KeyPressInGroup(KeyPort* keys[],uint8_t key_n, KeyDetectMode mode);

#endif

