#pragma once

// =========================
// I2C
// =========================

#define I2C_SDA 8
#define I2C_SCL 9

// =========================
// OLED
// =========================

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDRESS 0x3C

// =========================
// PN532
// =========================

// PN532 V3 switches:
// I2C = 1, 0

#define PN532_IRQ   -1
#define PN532_RESET -1

// =========================
// Device storage
// =========================

#define STORAGE_NAMESPACE "chipbank"
#define KIOSK_FORMAT_VERSION 1

// =========================
// Rotary encoder
// =========================

#define ENCODER_CLK 3
#define ENCODER_DT  4
#define ENCODER_SW  5

// =========================
// Card Format
// =========================

#define CARD_NAME_MAX_LENGTH 19
#define DEFAULT_PLAYER_START_BALANCE 100
#define CARD_FORMAT_VERSION 1

// Change to true if clockwise/counter-clockwise
// ends up backwards on your particular encoder.
#define ENCODER_REVERSED false