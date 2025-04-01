#ifndef KEY_H
#define KEY_H

#include "gpio.h"
#define     KEY_DELAY       (20)

typedef struct __key_handle KeyHandle;
/**
 * @brief       The Function Handle type of Key Detections
 *              Functions should indicate if key is pressed
 */
typedef uint8_t(*KeyDetections)(KeyHandle* handle);

/**
 * @brief   The key handle, when you fetch the handle
 *          You fetch the key :)
 */
typedef struct __key_handle{
    CCGPIOType      type;
    KeyDetections   func_detect;
}KeyHandle;

/**
 * @brief functions init the key
 * 
 * @param[in]   handle blank handle, with the detect way
 * @param[in]   clk_func open_clk_functions
 */
void init_bsp_key(KeyHandle* handle, OpenClock clk_func);

/**
 * @brief 
 * 
 * @param handle    key handles
 * @return uint8_t  returns the bool if the key is pressed!
 */
uint8_t isKeyPressed(KeyHandle* handle);




#endif