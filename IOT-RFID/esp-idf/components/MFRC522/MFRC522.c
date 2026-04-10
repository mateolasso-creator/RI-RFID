#include "MFRC522.h"

static const char *TAG = "MFRC522";

void mfrc522_pcd_write_register(mfrc522_t *dev, PCD_Register reg, byte value) {
    uint8_t tx_buffer[2] = {(reg << 1) & 0x7E, value};
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx_buffer,
        .rx_buffer = NULL
    };
    spi_device_transmit(dev->spi, &t);
}

byte mfrc522_pcd_read_register(mfrc522_t *dev, PCD_Register reg) {
    uint8_t tx_buffer[2] = {((reg << 1) & 0x7E) | 0x80, 0};
    uint8_t rx_buffer[2];
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer
    };
    spi_device_transmit(dev->spi, &t);
    return rx_buffer[1];
}

void mfrc522_pcd_init(mfrc522_t *dev) {
    // Reset
    gpio_set_level(dev->pin_rst, 0);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(dev->pin_rst, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // Soft reset
    mfrc522_pcd_write_register(dev, CommandReg, PCD_SoftReset);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // Configure
    mfrc522_pcd_write_register(dev, TxModeReg, 0x00);
    mfrc522_pcd_write_register(dev, RxModeReg, 0x00);
    mfrc522_pcd_write_register(dev, ModWidthReg, 0x26);

    mfrc522_pcd_write_register(dev, TModeReg, 0x80);
    mfrc522_pcd_write_register(dev, TPrescalerReg, 0xA9);
    mfrc522_pcd_write_register(dev, TReloadRegH, 0x03);
    mfrc522_pcd_write_register(dev, TReloadRegL, 0xE8);

    mfrc522_pcd_write_register(dev, TxASKReg, 0x40);
    mfrc522_pcd_write_register(dev, ModeReg, 0x3D);

    // Antenna on
    byte value = mfrc522_pcd_read_register(dev, TxControlReg);
    if ((value & 0x03) != 0x03) {
        mfrc522_pcd_write_register(dev, TxControlReg, value | 0x03);
    }
}

bool mfrc522_picc_is_new_card_present(mfrc522_t *dev) {
    mfrc522_pcd_write_register(dev, TxModeReg, 0x00);
    mfrc522_pcd_write_register(dev, RxModeReg, 0x00);
    mfrc522_pcd_write_register(dev, ModWidthReg, 0x26);

    byte bufferATQA[2];
    StatusCode result = mfrc522_picc_request_a(dev, bufferATQA, sizeof(bufferATQA));
    return (result == STATUS_OK || result == STATUS_COLLISION);
}

StatusCode mfrc522_picc_request_a(mfrc522_t *dev, byte *bufferATQA, byte *bufferSize) {
    return mfrc522_picc_reqa_or_wupa(dev, PICC_CMD_REQA, bufferATQA, bufferSize);
}

StatusCode mfrc522_picc_reqa_or_wupa(mfrc522_t *dev, byte command, byte *bufferATQA, byte *bufferSize) {
    byte validBits;
    StatusCode status;
    if (bufferATQA == NULL || *bufferSize < 2) {
        return STATUS_NO_ROOM;
    }
    mfrc522_pcd_clear_register_bit_mask(dev, CollReg, 0x80);
    validBits = 7;
    status = mfrc522_pcd_transceive_data(dev, &command, 1, bufferATQA, bufferSize, &validBits);
    if (status != STATUS_OK) {
        return status;
    }
    if (*bufferSize != 2 || validBits != 0) {
        return STATUS_ERROR;
    }
    return STATUS_OK;
}

StatusCode mfrc522_pcd_transceive_data(mfrc522_t *dev, byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC) {
    byte waitIRq = 0x30;
    return mfrc522_pcd_communicate_with_picc(dev, PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
}

StatusCode mfrc522_pcd_communicate_with_picc(mfrc522_t *dev, byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC) {
    // Simplified implementation
    // This is a basic version, full implementation would be longer
    mfrc522_pcd_write_register(dev, CommandReg, PCD_Idle);
    mfrc522_pcd_write_register(dev, ComIrqReg, 0x7F);
    mfrc522_pcd_write_register(dev, FIFOLevelReg, 0x80);
    mfrc522_pcd_write_register(dev, FIFODataReg, sendLen, sendData);
    mfrc522_pcd_write_register(dev, BitFramingReg, (rxAlign << 4) + (validBits ? *validBits : 0));
    mfrc522_pcd_write_register(dev, CommandReg, command);
    if (command == PCD_Transceive) {
        mfrc522_pcd_set_register_bit_mask(dev, BitFramingReg, 0x80);
    }
    // Wait for completion
    uint32_t deadline = esp_timer_get_time() / 1000 + 36;
    bool completed = false;
    do {
        byte n = mfrc522_pcd_read_register(dev, ComIrqReg);
        if (n & waitIRq) {
            completed = true;
            break;
        }
        if (n & 0x01) {
            return STATUS_TIMEOUT;
        }
        taskYIELD();
    } while (esp_timer_get_time() / 1000 < deadline);
    if (!completed) {
        return STATUS_TIMEOUT;
    }
    // Check errors
    byte errorRegValue = mfrc522_pcd_read_register(dev, ErrorReg);
    if (errorRegValue & 0x13) {
        return STATUS_ERROR;
    }
    // Get data back
    if (backData && backLen) {
        byte n = mfrc522_pcd_read_register(dev, FIFOLevelReg);
        if (n > *backLen) {
            return STATUS_NO_ROOM;
        }
        *backLen = n;
        mfrc522_pcd_read_register(dev, FIFODataReg, n, backData, rxAlign);
        if (validBits) {
            *validBits = mfrc522_pcd_read_register(dev, ControlReg) & 0x07;
        }
    }
    return STATUS_OK;
}

void mfrc522_pcd_clear_register_bit_mask(mfrc522_t *dev, PCD_Register reg, byte mask) {
    byte tmp = mfrc522_pcd_read_register(dev, reg);
    mfrc522_pcd_write_register(dev, reg, tmp & (~mask));
}

void mfrc522_pcd_set_register_bit_mask(mfrc522_t *dev, PCD_Register reg, byte mask) {
    byte tmp = mfrc522_pcd_read_register(dev, reg);
    mfrc522_pcd_write_register(dev, reg, tmp | mask);
}

void mfrc522_pcd_read_register(mfrc522_t *dev, PCD_Register reg, byte count, byte *values, byte rxAlign) {
    // Simplified
    for (byte index = 0; index < count; index++) {
        values[index] = mfrc522_pcd_read_register(dev, reg);
    }
}

void mfrc522_pcd_write_register(mfrc522_t *dev, PCD_Register reg, byte count, byte *values) {
    for (byte index = 0; index < count; index++) {
        mfrc522_pcd_write_register(dev, reg, values[index]);
    }
}

bool mfrc522_picc_read_card_serial(mfrc522_t *dev) {
    StatusCode result = mfrc522_picc_select(dev, &dev->uid);
    return (result == STATUS_OK);
}

StatusCode mfrc522_picc_select(mfrc522_t *dev, Uid *uid, byte validBits) {
    // Simplified PICC_Select implementation
    // This is a complex function, here is a basic version
    bool uidComplete;
    bool selectDone;
    StatusCode result;
    byte cascadeLevel = 1;
    uidComplete = false;
    while (!uidComplete) {
        // Basic selection logic
        byte buffer[9];
        buffer[0] = PICC_CMD_SEL_CL1;
        buffer[1] = 0x70;
        // BCC
        buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
        // CRC
        mfrc522_pcd_calculate_crc(dev, buffer, 7, &buffer[7]);
        byte responseBuffer[3];
        byte responseLength = 3;
        result = mfrc522_pcd_transceive_data(dev, buffer, 9, responseBuffer, &responseLength, NULL, 0, true);
        if (result == STATUS_OK) {
            uid->sak = responseBuffer[0];
            uid->size = 4;
            uidComplete = true;
        } else {
            return result;
        }
    }
    return STATUS_OK;
}

StatusCode mfrc522_pcd_calculate_crc(mfrc522_t *dev, byte *data, byte length, byte *result) {
    mfrc522_pcd_write_register(dev, CommandReg, PCD_Idle);
    mfrc522_pcd_write_register(dev, DivIrqReg, 0x04);
    mfrc522_pcd_write_register(dev, FIFOLevelReg, 0x80);
    mfrc522_pcd_write_register(dev, FIFODataReg, length, data);
    mfrc522_pcd_write_register(dev, CommandReg, PCD_CalcCRC);
    uint32_t deadline = esp_timer_get_time() / 1000 + 89;
    do {
        byte n = mfrc522_pcd_read_register(dev, DivIrqReg);
        if (n & 0x04) {
            mfrc522_pcd_write_register(dev, CommandReg, PCD_Idle);
            result[0] = mfrc522_pcd_read_register(dev, CRCResultRegL);
            result[1] = mfrc522_pcd_read_register(dev, CRCResultRegH);
            return STATUS_OK;
        }
        taskYIELD();
    } while (esp_timer_get_time() / 1000 < deadline);
    return STATUS_TIMEOUT;
}

void mfrc522_get_uid_as_string(mfrc522_t *dev, char *uid_str) {
    uid_str[0] = '\0';
    for (byte i = 0; i < dev->uid.size; i++) {
        char temp[3];
        sprintf(temp, "%02X", dev->uid.uidByte[i]);
        strcat(uid_str, temp);
    }
}

void mfrc522_send_uid_to_server(const char *uid) {
    char url[100];
    sprintf(url, "http://192.168.0.143/api.php?uid=%s", uid);
    esp_http_client_config_t config = {
        .url = url,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void mfrc522_connect_to_wifi(const char *ssid, const char *password) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };
    strcpy((char*)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, password);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
    // Wait for connection
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}

void mfrc522_activate_relay(int duration_ms) {
    gpio_set_level(21, 1);
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    gpio_set_level(21, 0);
}

void mfrc522_blink_relay(int times) {
    for (int i = 0; i < times; i++) {
        gpio_set_level(21, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(21, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}