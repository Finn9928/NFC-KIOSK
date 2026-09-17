#pragma once

#include <Arduino.h>

struct Kiosk {

    String kioskId;

    uint8_t formatVersion;

    uint32_t transactionCount;
};

bool kioskBegin();

const Kiosk &kioskGet();

bool kioskSave();

uint32_t kioskNextTransaction();