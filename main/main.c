#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdbool.h>
#include "xmc_flash_fix.h"

static const char* TAG = "MAIN";

void app_main() 
{
    
    ESP_LOGI(TAG, "Starting program XMC_Flash_Fix");

    esp_ipc_call_blocking(PRO_CPU_NUM, xmc_flash_fix, NULL);

    ESP_LOGI(TAG, "Finished XMC_Flash_Fix");

    while(true) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    vTaskDelete(NULL);
}