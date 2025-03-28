#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_psram.h"
#include "esp_flash.h"
#include "esp_log.h"
#include <stdio.h>


const char* TAG = "main";

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    esp_err_t ret;
    uint32_t flash_size;
    esp_chip_info_t chip_info;  /* 芯片相关信息 */

    ret = nvs_flash_init();     /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* 获取flash大小 */
    ESP_ERROR_CHECK(esp_flash_get_size(NULL, &flash_size));
    /* 芯片信息 */
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "| %-12s | %-10s |", "describe", "explain");
    ESP_LOGI(TAG, "|--------------|------------|");

    if (chip_info.model == CHIP_ESP32S3)
    {
        ESP_LOGI(TAG, "| %-12s | %-10s |", "model", "ESP32S3");                                     /* 芯片型号 */
    }
    
    ESP_LOGI(TAG, "| %-12s | %-d          |", "cores",      chip_info.cores);                       /* 获取芯片的内核数量 */
    ESP_LOGI(TAG, "| %-12s | %-d          |", "revision",   chip_info.revision);                    /* 获取芯片的内核数量 */
    ESP_LOGI(TAG, "| %-12s | %-ld         |", "FLASH size", flash_size / (1024 * 1024));            /* 获取FLASH大小(MB) */
    ESP_LOGI(TAG, "| %-12s | %-2d         |", "PSRAM size", esp_psram_get_size() / (1024 * 1024));  /* 获取PSRAM的大小(MB) */
    ESP_LOGI(TAG, "|--------------|------------|");

    while(1)
    {
        ESP_LOGI(TAG, "Hello-ESP32S3");
        vTaskDelay(pdMS_TO_TICKS(1000));            /* 延时1s */
    }
}
