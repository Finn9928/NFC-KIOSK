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
#define CARD_FORMAT_VERSION 1
#define KIOSK_FORMAT_VERSION 1