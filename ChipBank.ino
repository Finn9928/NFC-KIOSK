#include <Wire.h>

#include "config.h"
#include "display.h"
#include "encoder.h"
#include "nfc.h"
#include "storage.h"
#include "card.h"


// ============================================================
// Kiosk state
// ============================================================

enum KioskState {
    SCANNING,
    CARD_MENU
};

KioskState kioskState = SCANNING;


// ============================================================
// Current card
// ============================================================

Card currentCard;


// ============================================================
// Card menu
// ============================================================

int8_t currentPage = 0;

const uint8_t CARD_MENU_PAGES = 3;


// ============================================================
// Scan screen
// ============================================================

void showScanScreen() {

    displayRows({
        "CHIP KIOSK",
        "READY",
        "",
        "SCAN CARD"
    });
}


// ============================================================
// Card menu display
// ============================================================

void showCardMenu() {
    switch (currentPage) {
        // ----------------------------------------------------
        // Page 1 - Player and Balance
        // ----------------------------------------------------
        case 0:
            displayMenu(
                {
                    "PLAYER",
                    "",
                    currentCard.playerName,
                    "BALANCE",
                    "",
                    String(currentCard.chipBalance) + " CHIPS"
                },
                currentPage,
                CARD_MENU_PAGES
            );

            break;


        // ----------------------------------------------------
        // Page 2 - Transactions and Games Won
        // ----------------------------------------------------

        case 1:

            displayMenu(
                {
                    "TRANSACTIONS",
                    "",
                    String(currentCard.transactionCount),
                    "GAMES WON",
                    "",
                    String(currentCard.gamesWon)
                },
                currentPage,
                CARD_MENU_PAGES
            );

            break;

        // ----------------------------------------------------
        // Page 3 - Back
        // ----------------------------------------------------

        case 2:

            displayMenu(
                {
                    "BACK",
                    "",
                    "PRESS TO RETURN"
                },
                currentPage,
                CARD_MENU_PAGES
            );

            break;
    }
}


// ============================================================
// Handle card menu
// ============================================================

void updateCardMenu() {

    // --------------------------------------------------------
    // Rotary movement
    //
    // This follows your tested encoder usage exactly.
    // --------------------------------------------------------

    int8_t delta =
        encoderGetDelta();

    if (delta != 0) {

        currentPage += delta;


        // Wrap around the menu.

        if (currentPage >= CARD_MENU_PAGES) {
            currentPage = 0;
        }

        if (currentPage < 0) {
            currentPage = CARD_MENU_PAGES - 1;
        }


        // Only redraw when the page changes.

        showCardMenu();
    }


    // --------------------------------------------------------
    // Encoder button
    // --------------------------------------------------------

    if (encoderWasPressed()) {

        Serial.print("Menu button pressed on page ");
        Serial.println(currentPage + 1);


        switch (currentPage) {

            case 0:

                // Player page.
                // Action can be added later.

                break;


            case 1:

                // Balance page.
                // Action can be added later.

                break;


            case 2:

                // BACK

                Serial.println(
                    "Returning to card scan."
                );

                kioskState = SCANNING;

                currentPage = 0;

                showScanScreen();

                break;
        }
    }
}


// ============================================================
// Setup
// ============================================================

void setup() {

    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("====================");
    Serial.println("     CHIP KIOSK");
    Serial.println("====================");


    // --------------------------------------------------------
    // I2C
    // --------------------------------------------------------

    Wire.begin(
        I2C_SDA,
        I2C_SCL
    );


    // --------------------------------------------------------
    // Hardware
    // --------------------------------------------------------

    displayBegin();

    storageBegin();

    encoderBegin();


    // --------------------------------------------------------
    // Startup screen
    // --------------------------------------------------------

    displayRows({
        "CHIP KIOSK",
        "STARTING..."
    });

    delay(500);


    // --------------------------------------------------------
    // NFC reader
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Ready
    // --------------------------------------------------------

    showScanScreen();

    Serial.println("System ready.");
}


// ============================================================
// Main loop
// ============================================================

void loop() {

    // ========================================================
    // IMPORTANT
    //
    // Your encoder driver must be updated constantly.
    // This stays at the top of the loop just like your
    // working encoder test.
    // ========================================================

    encoderUpdate();


    // ========================================================
    // Scanning mode
    // ========================================================

    if (kioskState == SCANNING) {

        Card card;

        if (cardRead(card)) {

            // Save the card we just read.

            currentCard = card;


            // Serial information.

            Serial.println();
            Serial.println("Card found!");

            Serial.print("UID: ");
            Serial.println(currentCard.uid);

            Serial.print("Player: ");
            Serial.println(currentCard.playerName);

            Serial.print("Balance: ");
            Serial.println(currentCard.chipBalance);

            Serial.print("Transactions: ");
            Serial.println(currentCard.transactionCount);

            Serial.print("Games won: ");
            Serial.println(currentCard.gamesWon);


            // Enter card menu.

            currentPage = 0;

            kioskState = CARD_MENU;


            // Show first page immediately.

            showCardMenu();
        }
    }


    // ========================================================
    // Card menu
    // ========================================================

    else if (kioskState == CARD_MENU) {

        updateCardMenu();
    }


    // Keep the loop fast for the encoder.

    delay(1);
}