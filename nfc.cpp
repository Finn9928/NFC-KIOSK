#include "nfc.h"
#include "config.h"

#include <Wire.h>
#include <Adafruit_PN532.h>

static Adafruit_PN532 nfc(
    PN532_IRQ,
    PN532_RESET,
    &Wire
);

bool nfcBegin() {

    nfc.begin();

    uint32_t versiondata =
        nfc.getFirmwareVersion();

    if (!versiondata) {

        Serial.println(
            "PN532 not found!"
        );

        return false;
    }

    Serial.print("PN532 found. Firmware ");

    Serial.print(
        (versiondata >> 16) & 0xFF
    );

    Serial.print(".");

    Serial.println(
        (versiondata >> 8) & 0xFF
    );

    nfc.SAMConfig();

    return true;
}

bool nfcCardPresent() {

    uint8_t uid[7];
    uint8_t uidLength;

    return nfc.readPassiveTargetID(
        PN532_MIFARE_ISO14443A,
        uid,
        &uidLength,
        50
    );
}

bool nfcReadUID(String &uid) {

    uint8_t rawUID[7];
    uint8_t uidLength;

    bool success = nfc.readPassiveTargetID(
        PN532_MIFARE_ISO14443A,
        rawUID,
        &uidLength,
        100
    );

    if (!success) {
        return false;
    }

    uid = "";

    for (uint8_t i = 0; i < uidLength; i++) {

        if (rawUID[i] < 0x10) {
            uid += "0";
        }

        uid += String(
            rawUID[i],
            HEX
        );
    }

    uid.toUpperCase();

    return true;
}

// =========================
// Raw NTAG page access
// =========================

bool nfcReadPage(
    uint8_t page,
    uint8_t data[4]
) {

    return nfc.ntag2xx_ReadPage(
        page,
        data
    );
}

bool nfcWritePage(
    uint8_t page,
    const uint8_t data[4]
) {

    uint8_t buffer[4];

    for (uint8_t i = 0; i < 4; i++) {
        buffer[i] = data[i];
    }

    return nfc.ntag2xx_WritePage(
        page,
        buffer
    );
}