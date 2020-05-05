#ifndef NVS_COMPONENT_H
#define NVS_COMPONENT_H

#include <nvs_flash.h>
#include <esp_err.h>

void nvs_init() {
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        error = nvs_flash_init();
    }
    ESP_ERROR_CHECK(error);
}

#endif // NVS_COMPONENT_H
