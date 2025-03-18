#ifndef SYSCLK_PREFERENCE_H
#define SYSCLK_PREFERENCE_H

typedef enum {
    USE_HSI_AS_SYSCLK,
    USE_HSE_AS_SYSCLK
}ClockSourcePreference;

/*
    if you have preference already, define the followings
*/

// #define DEPRECATED_HSE_INIT
// #define DEPRECATED_HSI_INIT

#endif