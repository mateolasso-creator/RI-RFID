#ifndef MFRC522_H
#define MFRC522_H

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

typedef uint8_t byte;

typedef enum {
    CommandReg = 0x01 << 1,
    ComIEnReg = 0x02 << 1,
    DivIEnReg = 0x03 << 1,
    ComIrqReg = 0x04 << 1,
    DivIrqReg = 0x05 << 1,
    ErrorReg = 0x06 << 1,
    Status1Reg = 0x07 << 1,
    Status2Reg = 0x08 << 1,
    FIFODataReg = 0x09 << 1,
    FIFOLevelReg = 0x0A << 1,
    WaterLevelReg = 0x0B << 1,
    ControlReg = 0x0C << 1,
    BitFramingReg = 0x0D << 1,
    CollReg = 0x0E << 1,
    ModeReg = 0x11 << 1,
    TxModeReg = 0x12 << 1,
    RxModeReg = 0x13 << 1,
    TxControlReg = 0x14 << 1,
    TxASKReg = 0x15 << 1,
    TxSelReg = 0x16 << 1,
    RxSelReg = 0x17 << 1,
    RxThresholdReg = 0x18 << 1,
    DemodReg = 0x19 << 1,
    MfTxReg = 0x1C << 1,
    MfRxReg = 0x1D << 1,
    SerialSpeedReg = 0x1F << 1,
    CRCResultRegH = 0x21 << 1,
    CRCResultRegL = 0x22 << 1,
    ModWidthReg = 0x24 << 1,
    RFCfgReg = 0x26 << 1,
    GsNReg = 0x27 << 1,
    CWGsPReg = 0x28 << 1,
    ModGsPReg = 0x29 << 1,
    TModeReg = 0x2A << 1,
    TPrescalerReg = 0x2B << 1,
    TReloadRegH = 0x2C << 1,
    TReloadRegL = 0x2D << 1,
    TCounterValueRegH = 0x2E << 1,
    TCounterValueRegL = 0x2F << 1,
    TestSel1Reg = 0x31 << 1,
    TestSel2Reg = 0x32 << 1,
    TestPinEnReg = 0x33 << 1,
    TestPinValueReg = 0x34 << 1,
    TestBusReg = 0x35 << 1,
    AutoTestReg = 0x36 << 1,
    VersionReg = 0x37 << 1,
    AnalogTestReg = 0x38 << 1,
    TestDAC1Reg = 0x39 << 1,
    TestDAC2Reg = 0x3A << 1,
    TestADCReg = 0x3B << 1
} PCD_Register;

typedef enum {
    PCD_Idle = 0x00,
    PCD_Mem = 0x01,
    PCD_GenerateRandomID = 0x02,
    PCD_CalcCRC = 0x03,
    PCD_Transmit = 0x04,
    PCD_NoCmdChange = 0x07,
    PCD_Receive = 0x08,
    PCD_Transceive = 0x0C,
    PCD_MFAuthent = 0x0E,
    PCD_SoftReset = 0x0F
} PCD_Command;

typedef enum {
    PICC_CMD_REQA = 0x26,
    PICC_CMD_WUPA = 0x52,
    PICC_CMD_CT = 0x88,
    PICC_CMD_SEL_CL1 = 0x93,
    PICC_CMD_SEL_CL2 = 0x95,
    PICC_CMD_SEL_CL3 = 0x97,
    PICC_CMD_HLTA = 0x50,
    PICC_CMD_MF_AUTH_KEY_A = 0x60,
    PICC_CMD_MF_AUTH_KEY_B = 0x61,
    PICC_CMD_MF_READ = 0x30,
    PICC_CMD_MF_WRITE = 0xA0,
    PICC_CMD_MF_DECREMENT = 0xC0,
    PICC_CMD_MF_INCREMENT = 0xC1,
    PICC_CMD_MF_RESTORE = 0xC2,
    PICC_CMD_MF_TRANSFER = 0xB0,
    PICC_CMD_UL_WRITE = 0xA2
} PICC_Command;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_COLLISION = 2,
    STATUS_TIMEOUT = 3,
    STATUS_NO_ROOM = 4,
    STATUS_INTERNAL_ERROR = 5,
    STATUS_INVALID = 6,
    STATUS_CRC_WRONG = 7,
    STATUS_MIFARE_NACK = 8
} StatusCode;

typedef struct {
    byte size;
    byte uidByte[10];
    byte sak;
} Uid;

typedef struct {
    spi_device_handle_t spi;
    gpio_num_t pin_rst;
    Uid uid;
} mfrc522_t;

void mfrc522_pcd_write_register(mfrc522_t *dev, PCD_Register reg, byte value);
byte mfrc522_pcd_read_register(mfrc522_t *dev, PCD_Register reg);
void mfrc522_pcd_init(mfrc522_t *dev);
bool mfrc522_picc_is_new_card_present(mfrc522_t *dev);
bool mfrc522_picc_read_card_serial(mfrc522_t *dev);
void mfrc522_get_uid_as_string(mfrc522_t *dev, char *uid_str);
void mfrc522_send_uid_to_server(const char *uid);
void mfrc522_connect_to_wifi(const char *ssid, const char *password);
void mfrc522_activate_relay(int duration_ms);
void mfrc522_blink_relay(int times);

#endif