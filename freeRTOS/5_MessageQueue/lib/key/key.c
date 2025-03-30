#include "key.h"
#include "system_clock.h"

void init_key(
    KeyPort* port, KeyCallBack callback, 
    KeyDetect detect, CCGPIOInitTypeDef* init)
{
    init->type.Mode = GPIO_MODE_INPUT;
    configure_ccgpio(&(port->gpio_core), init);
    port->key_callback = callback;
    port->key_detect = detect;
}

uint8_t isKeyPress(KeyPort* key, KeyDetectMode mode)
{
    static KeyPressStatus status = KEY_UNPRESS;
    if(mode == KEY_LONG_PRESS) status = KEY_UNPRESS;
    if(status == KEY_UNPRESS && key->key_detect(key)){
        system_delay_ms(10);
        status = KEY_PRESS;
        if(key->key_detect(key)){
            return KEY_PRESS;
        }
    }else if(!key->key_detect(key)){
        status = KEY_UNPRESS;
    }
    return KEY_UNPRESS;
}

void doKeyCallbackIfDone(KeyPort* key, KeyDetectMode mode)
{
    if(isKeyPress(key, mode)){
        key->key_callback(key);
    }
}


KeyPort* isKeyPressInGroup(KeyPort* keys[],uint8_t key_n, KeyDetectMode mode)
{
    for(uint8_t i = 0; i < key_n; i++){
        if(isKeyPress(keys[i], mode)){
            return keys[i];
        }
    }
    return NULL;
}

void doCallback_KeyPressInGroup(KeyPort* keys[],uint8_t key_n, KeyDetectMode mode)
{
    for(uint8_t i = 0; i < key_n; i++){
        if(isKeyPress(keys[i], mode)){
            keys[i]->key_callback(keys[i]);
            return;
        }
    }
}
