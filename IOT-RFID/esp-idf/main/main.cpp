#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "MFRC522.h"

#define SS_PIN    5
#define RST_PIN   22
#define RELAY_PIN 21

const char* ssid     = "inwi Home 4G E2F379";
const char* password = "46389619";
const char* serverUrl = "http://192.168.0.143/api.php";
const unsigned long timeout = 5000;
const unsigned long cardDetectDelay = 500;

mfrc522_t rfid;
spi_device_handle_t spi_handle;
uint32_t lastCardTime = 0;

static const char *TAG = "RFID_ACCESS";

void app_main(void) {
    ESP_LOGI(TAG, "Starting RFID Access System...");

    // Initialize GPIO for relay
    gpio_pad_select_gpio(RELAY_PIN);
    gpio_set_direction(RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RELAY_PIN, 0);

    // Initialize SPI
    spi_bus_config_t buscfg = {
        .mosi_io_num = 23,
        .miso_io_num = 19,
        .sclk_io_num = 18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = SS_PIN,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle));

    // Initialize RFID
    rfid.spi = spi_handle;
    rfid.pin_rst = RST_PIN;
    gpio_pad_select_gpio(RST_PIN);
    gpio_set_direction(RST_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RST_PIN, 1);

    // Connect to WiFi
    mfrc522_connect_to_wifi(ssid, password);

    // Initialize RFID
    mfrc522_pcd_init(&rfid);

    ESP_LOGI(TAG, "System ready");

    while (1) {
        // Check WiFi
        if (esp_wifi_get_state() != WIFI_STATE_STA_CONNECTED) {
            ESP_LOGI(TAG, "WiFi disconnected. Reconnecting...");
            mfrc522_connect_to_wifi(ssid, password);
            continue;
        }

        // Check for new card
        if (!mfrc522_picc_is_new_card_present(&rfid)) {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            continue;
        }

        // Read card serial
        if (!mfrc522_picc_read_card_serial(&rfid)) {
            ESP_LOGI(TAG, "Error reading card serial");
            vTaskDelay(50 / portTICK_PERIOD_MS);
            continue;
        }

        // Debounce
        uint32_t currentTime = esp_timer_get_time() / 1000;
        if (currentTime - lastCardTime < cardDetectDelay) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }
        lastCardTime = currentTime;

        // Get UID as string
        char uid[21];
        mfrc522_get_uid_as_string(&rfid, uid);
        ESP_LOGI(TAG, "UID detected: %s", uid);

        // Send to server
        mfrc522_send_uid_to_server(uid);

        // Halt card
        // Simplified, no halt implemented

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}