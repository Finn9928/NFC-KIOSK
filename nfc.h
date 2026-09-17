#pragma once

#include <Arduino.h>

bool nfcBegin();

bool nfcCardPresent();

bool nfcReadUID(
    String &uid
);

// =========================
// Raw NTAG page access
// =========================

bool nfcReadPage(
    uint8_t page,
    uint8_t data[4]
);

bool nfcWritePage(
    uint8_t page,
    const uint8_t data[4]
);