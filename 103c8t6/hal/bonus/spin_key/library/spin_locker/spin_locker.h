#ifndef SPIN_LOCKER_H
#define SPIN_LOCKER_H
#include "gpio_exti.h"

#define CCSpinLocker_SPIN_LEFT_INDEX        (0)
#define CCSpinLocker_SPIN_RIGHT_INDEX       (1)
#define CCSpinLocker_SPIN_KEY_INDEX         (2)
#define CCSpinLocker_SPIN_HOOK_N            (3)
typedef struct {
    void (*hooks[CCSpinLocker_SPIN_HOOK_N])(void);
    CCGPIOType  handling_types[CCSpinLocker_SPIN_HOOK_N];
}CCSpinLockerHandle;

typedef struct {
#define CCSpinLockerHandle_PORTS        (3)
    struct {
        struct {
            uint8_t     irq_lines;
            uint8_t     main_preepmt;
            uint8_t     sub_preempt;
        } irq_settings;
        CCGPIOType      types;
        OpenClock       open_clk;
    } internal_infos[CCSpinLockerHandle_PORTS];
}CCSpinLockerGPIOInfo;


typedef struct {
    void (*hooks[CCSpinLocker_SPIN_HOOK_N])(void);
    CCSpinLockerGPIOInfo        info;
}CCSpinLockerHandleInitType;

/**
 * @brief init a spinlocker
 * 
 * @param[out]  blank_hooks 
 * @param[in]   initer 
 */

void initCCSpinLockerHandle(CCSpinLockerHandle* blank_hooks, CCSpinLockerHandleInitType* initer);
void change_monitor_handle(CCSpinLockerHandle* blank_hooks);
#endif