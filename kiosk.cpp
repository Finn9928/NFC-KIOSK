#include "kiosk.h"
#include "config.h"
#include "storage.h"

#include "esp_mac.h"

static Kiosk kiosk;

static const char *KEY_KIOSK_ID = "k_id";
static const char *KEY_KIOSK_VER = "k_ver";
static const char *KEY_KIOSK_TXN = "k_txn";

// =========================
// Generate kiosk ID
// =========================

static String generateKioskId() {

    uint8_t mac[6];

    if (esp_read_mac(
        mac,
        ESP_MAC_WIFI_STA
    ) != ESP_OK) {

        return "KIOSK-UNKNOWN";
    }

    char id[32];

    snprintf(
        id,
        sizeof(id),
        "KIOSK-%02X%02X%02X%02X%02X%02X",
        mac[0],
        mac[1],
        mac[2],
        mac[3],
        mac[4],
        mac[5]
    );

    return String(id);
}

// =========================
// Start kiosk backend
// =========================

bool kioskBegin() {

    String storedId =
        storageReadString(
            KEY_KIOSK_ID,
            ""
        );

    uint32_t storedVersion =
        storageReadUInt(
            KEY_KIOSK_VER,
            0
        );

    uint32_t storedTransactions =
        storageReadUInt(
            KEY_KIOSK_TXN,
            0
        );

    // -------------------------
    // First boot
    // -------------------------

    if (storedId.length() == 0) {

        kiosk.kioskId =
            generateKioskId();

        kiosk.formatVersion =
            KIOSK_FORMAT_VERSION;

        kiosk.transactionCount =
            0;

        return kioskSave();
    }

    // -------------------------
    // Existing kiosk
    // -------------------------

    if (storedVersion != KIOSK_FORMAT_VERSION) {

        Serial.println(
            "Unsupported kiosk format version."
        );

        return false;
    }

    kiosk.kioskId =
        storedId;

    kiosk.formatVersion =
        (uint8_t)storedVersion;

    kiosk.transactionCount =
        storedTransactions;

    return true;
}

// =========================
// Get kiosk
// =========================

const Kiosk &kioskGet() {

    return kiosk;
}

// =========================
// Save kiosk
// =========================

bool kioskSave() {

    bool success = true;

    if (!storageWriteString(
        KEY_KIOSK_ID,
        kiosk.kioskId
    )) {
        success = false;
    }

    if (!storageWriteUInt(
        KEY_KIOSK_VER,
        kiosk.formatVersion
    )) {
        success = false;
    }

    if (!storageWriteUInt(
        KEY_KIOSK_TXN,
        kiosk.transactionCount
    )) {
        success = false;
    }

    return success;
}

// =========================
// Next transaction number
// =========================

uint32_t kioskNextTransaction() {

    kiosk.transactionCount++;

    kioskSave();

    return kiosk.transactionCount;
}