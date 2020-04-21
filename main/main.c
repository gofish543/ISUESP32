#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "../../components/nvs_component.h"
#include "../../components/sd_component.h"
#include "../../components/csi_component.h"
#include "../../components/time_component.h"
#include "../../components/input_component.h"

void passive_init() {
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());

    const wifi_promiscuous_filter_t filt = {
            .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA
    };

    esp_wifi_set_promiscuous(true);
//    esp_wifi_set_promiscuous_filter(&filt);
    esp_wifi_set_channel(36, WIFI_SECOND_CHAN_NONE);
}

void app_main() {
    nvs_init();
    sd_init();
    passive_init();
    csi_init("PASSIVE");
    input_loop();
}
