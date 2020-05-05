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

#define WIFI_PRIMARY_CHANNEL 7

void passive_init();

void passive_init() {
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());

    /** @link https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_wifi_types.h#L374 */
    const wifi_promiscuous_filter_t filt = {
            .filter_mask = WIFI_PROMIS_FILTER_MASK_ALL
    };

    // Put wifi device into promiscuous mode, and attach the filter to it
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_filter(&filt);

    // Because the ESP device has WiFi on 2.4 GHz - 2.5 GHz these values must be between 1 and 14
    esp_wifi_set_channel(WIFI_PRIMARY_CHANNEL, WIFI_SECOND_CHAN_NONE);
}

void app_main() {
    nvs_init();
    sd_init(); // My ESP32 does not have an sd card slot, so this code has been left as is. There are no certainties it works
    passive_init();
    csi_init("PASSIVE");
    input_loop();
}
