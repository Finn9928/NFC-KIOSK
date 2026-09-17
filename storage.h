#pragma once

#include <Arduino.h>

void storageBegin();

bool storageWriteString(
    const char *key,
    const String &value
);

String storageReadString(
    const char *key,
    const String &defaultValue = ""
);

bool storageWriteInt(
    const char *key,
    int32_t value
);

int32_t storageReadInt(
    const char *key,
    int32_t defaultValue = 0
);

bool storageDelete(
    const char *key
);

void storageClear();