#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <stdio.h>

void app_main(void)
{
    esp_err_t ret;
    
    ret = nvs_flash_init();     /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    led_init();                 /* 初始化LED */

    while(1)
    {
        led_toggle_pin();
        vTaskDelay(pdMS_TO_TICKS(500));    /* 延时500ms */
    }
}