#include <Wire.h>

#include "config.h"
#include "display.h"
#include "nfc.h"
#include "storage.h"
#include "card.h"

void setup() {

    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("====================");
    Serial.println("     CHIP KIOSK");
    Serial.println("====================");

    // Start I2C
    Wire.begin(
        I2C_SDA,
        I2C_SCL
    );

    // Start hardware
    displayBegin();
    storageBegin();

    displayRows({
        "CHIP KIOSK",
        "STARTING..."
    });

    delay(500);

    // Start NFC
    if (!nfcBegin()) {

        displayRows({
            "CHIP KIOSK",
            "NFC ERROR",
            "CHECK READER"
        });

        while (true) {
            delay(1000);
        }
    }

    displayRows({
        "CHIP KIOSK",
        "READY",
        "",
        "SCAN CARD"
    });

    Serial.println("System ready.");
}

void loop() {

    Card card;

    if (cardRead(card)) {

        Serial.print("Card UID: ");
        Serial.println(card.uid);

        displayRows({
            "CARD FOUND",
            "",
            card.uid
        });

        delay(1500);

        displayRows({
            "CHIP KIOSK",
            "READY",
            "",
            "SCAN CARD"
        });
    }

    delay(50);
}