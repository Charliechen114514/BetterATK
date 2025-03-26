#include "lcd.h"
#include "type_adapter/type_adapter.h"

static void __config_soft_gpios(SoftLCDInitGPIOPack* packages){
    CCGPIOInitTypeDef init;
    init.open_clock = packages->gpio_clk_open;
    init.type.Mode = GPIO_MODE_OUTPUT_PP;
    init.type.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    init.type.Pull = GPIO_PULLUP;
    init.post_init = NULL;
    for(uint8_t i = 0; i < SOFT_GPIO_INIT_ARRAY_N; i++){
        init.port = packages->gpios[i].port;
        init.type.Pin = packages->gpios[i].pinType;
        configure_ccgpio(&packages->gpios[i], &init);
    }

    init.port = packages->gpios[GPIO_MISO].port;
    init.type.Mode =  GPIO_MODE_INPUT;
    init.type.Pin = packages->gpios[GPIO_MISO].pinType;
    configure_ccgpio(&packages->gpios[GPIO_MISO], &init);
}

void init_lcdhandle_soft(
    LCDHandleType* initer,
    LCD_DeviceType device_type,
    LCDSoftTypeHandlePrivate* packages)
{    
    // init the device property
    initer->private_handle = packages;
    __config_soft_gpios(packages->packages_cached);
    setup_decrete_lcd(initer, device_type);
    initer->operations.switch_directions(initer, LCD_Direction_180);
    initer->operations.light_up_backlight(initer, LCD_BackLightStatus_On);
}

