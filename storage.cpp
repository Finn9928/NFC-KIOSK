#include "storage.h"
#include "config.h"

#include <Preferences.h>

static Preferences preferences;

void storageBegin() {

    preferences.begin(
        STORAGE_NAMESPACE,
        false
    );
}

bool storageWriteString(
    const char *key,
    const String &value
) {

    return preferences.putString(
        key,
        value
    ) > 0;
}

String storageReadString(
    const char *key,
    const String &defaultValue
) {

    return preferences.getString(
        key,
        defaultValue
    );
}

bool storageWriteInt(
    const char *key,
    int32_t value
) {

    return preferences.putInt(
        key,
        value
    );
}

int32_t storageReadInt(
    const char *key,
    int32_t defaultValue
) {

    return preferences.getInt(
        key,
        defaultValue
    );
}

bool storageDelete(
    const char *key
) {

    return preferences.remove(key);
}

void storageClear() {

    preferences.clear();
}

bool storageWriteUInt(
    const char *key,
    uint32_t value
) {

    return preferences.putUInt(
        key,
        value
    );
}

uint32_t storageReadUInt(
    const char *key,
    uint32_t defaultValue
) {

    return preferences.getUInt(
        key,
        defaultValue
    );
}