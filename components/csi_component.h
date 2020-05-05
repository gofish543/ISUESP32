#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"
#include "math.h"

char* projectType;

#define CSI_RAW 1
#define CSI_AMPLITUDE 0
#define CSI_PHASE 0

char* types[] = {
        "Management", "Control", "Data", "Misc"
};

void print_byte_as_bits(unsigned char val) {
    for (int i = 7; 0 <= i; i--) {
        outprintf("%c", (val & (1 << i)) ? '1' : '0');
    }
}

void print_bits(unsigned char* bytes, size_t num_bytes) {
    outprintf("[ ");
    for (size_t i = 0; i < num_bytes; i++) {
        print_byte_as_bits(bytes[i]);
        outprintf(" ");
    }
    outprintf("]\n");
}

void wifi_promiscuous_sniffer(void* recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* buffer = (wifi_promiscuous_pkt_t*) recv_buf;

    outprintf("Packet Found\n");
    outprintf("Frame Type %s\n", types[type]);
    outprintf("Size: %d\n", buffer->rx_ctrl.sig_len);
    outprintf("Channel: %d\n", buffer->rx_ctrl.channel);

    // More work needs to be done here @todo
}

void wifi_csi_sniffer(void* ctx, wifi_csi_info_t* data) {
    // This was taken from the main sniffer repository
    // More work can be done to improve this @todo

    wifi_csi_info_t d = data[0];
    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);

    outprintf("CSI_DATA,");
    outprintf("%s,", projectType);
    outprintf("%s,", mac);

    // https://github.com/espressif/esp-idf/blob/9d0ca60398481a44861542638cfdc1949bb6f312/components/esp_wifi/include/esp_wifi_types.h#L314
    outprintf("%d,", d.rx_ctrl.rssi);
    outprintf("%d,", d.rx_ctrl.rate);
    outprintf("%d,", d.rx_ctrl.sig_mode);
    outprintf("%d,", d.rx_ctrl.mcs);
    outprintf("%d,", d.rx_ctrl.cwb);
    outprintf("%d,", d.rx_ctrl.smoothing);
    outprintf("%d,", d.rx_ctrl.not_sounding);
    outprintf("%d,", d.rx_ctrl.aggregation);
    outprintf("%d,", d.rx_ctrl.stbc);
    outprintf("%d,", d.rx_ctrl.fec_coding);
    outprintf("%d,", d.rx_ctrl.sgi);
    outprintf("%d,", d.rx_ctrl.noise_floor);
    outprintf("%d,", d.rx_ctrl.ampdu_cnt);
    outprintf("%d,", d.rx_ctrl.channel);
    outprintf("%d,", d.rx_ctrl.secondary_channel);
    outprintf("%d,", d.rx_ctrl.timestamp);
    outprintf("%d,", d.rx_ctrl.ant);
    outprintf("%d,", d.rx_ctrl.sig_len);
    outprintf("%d,", d.rx_ctrl.rx_state);

    char* resp = time_string_get();
    outprintf("%d,", real_time_set);
    outprintf("%s,", resp);
    free(resp);

    int8_t* my_ptr;

#if CSI_RAW
    outprintf("%d,[", data->len);
    my_ptr = data->buf;

    for (int i = 0; i < 128; i++) {
        outprintf("%d ", my_ptr[i]);
    }
    outprintf("]");
#endif
#if CSI_AMPLITUDE
    outprintf("%d,[", data->len);
    my_ptr = data->buf;

    for (int i = 0; i < 64; i++) {
        outprintf("%.4f ", sqrt(pow(my_ptr[i * 2], 2) + pow(my_ptr[(i * 2) + 1], 2)));
    }
    outprintf("]");
#endif
#if CSI_PHASE
    outprintf("%d,[", data->len);
    my_ptr = data->buf;

    for (int i = 0; i < 64; i++) {
                outprintf("%.4f ", atan2(my_ptr[i*2], my_ptr[(i*2)+1]));
            }
    outprintf("]");
#endif
    outprintf("\n");
    sd_flush();
    vTaskDelay(0);
}

void csi_init(char* type) {
    projectType = type;

#ifdef CONFIG_SHOULD_COLLECT_CSI
    ESP_ERROR_CHECK(esp_wifi_set_csi(1));

    // @Link https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_wifi_types.h#L401
    wifi_csi_config_t configuration_csi;
    configuration_csi.lltf_en = 1;
    configuration_csi.htltf_en = 1;
    configuration_csi.stbc_htltf2_en = 1;
    configuration_csi.ltf_merge_en = 1;
    configuration_csi.channel_filter_en = 0;
    configuration_csi.manu_scale = 0;

    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&configuration_csi));

    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&wifi_csi_sniffer, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous_sniffer));

#endif
}

#endif //ESP32_CSI_CSI_COMPONENT_H
