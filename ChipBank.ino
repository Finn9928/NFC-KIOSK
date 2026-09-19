#include <Wire.h>

#include "config.h"
#include "display.h"
#include "encoder.h"
#include "nfc.h"
#include "storage.h"
#include "card.h"


// ============================================================
// Temporary test settings
// ============================================================
//
// Change these manually while we are testing.
//
// ============================================================

const char *TEMP_PLAYER_NAME =
    "Max Verstappen";

const uint64_t TEMP_PLAYER_START_BALANCE =
    1000;

const uint64_t TEMP_BALANCE_STEP =
    100;

const uint64_t TEMP_BALANCE_MAX =
    10000;


// ============================================================
// Kiosk state
// ============================================================

enum KioskState {

    SCANNING,
    MAIN_MENU,
    CARD_MENU,
    NEW_PLAYER,
    BALANCE_ADJUST

};

KioskState kioskState = SCANNING;


// ============================================================
// Current card
// ============================================================

Card currentCard;


// ============================================================
// Main menu
// ============================================================

int8_t mainMenuPage = 0;

const uint8_t MAIN_MENU_PAGES = 4;


// ============================================================
// Card menu
// ============================================================

int8_t currentPage = 0;

const uint8_t CARD_MENU_PAGES = 3;


// ============================================================
// Balance adjustment
// ============================================================

bool balanceAddMode = true;

bool balanceCardLoaded = false;

uint64_t balanceAmount =
    TEMP_BALANCE_STEP;


// ============================================================
// Scan / Home screen
// ============================================================

void showScanScreen() {

    displayRows({
        "CHIP KIOSK",
        "READY",
        "",
        "SCAN CARD",
        "",
        "PRESS FOR MENU"
    });
}


// ============================================================
// Main menu display
// ============================================================

void showMainMenu() {

    switch (mainMenuPage) {

        // ----------------------------------------------------
        // Page 1 - New Player
        // ----------------------------------------------------

        case 0:

            displayMenu(
                {
                    "NEW PLAYER",
                    "",
                    "Create a new",
                    "player card"
                },
                mainMenuPage,
                MAIN_MENU_PAGES
            );

            break;


        // ----------------------------------------------------
        // Page 2 - Add Balance
        // ----------------------------------------------------

        case 1:

            displayMenu(
                {
                    "ADD BALANCE",
                    "",
                    "Add chips to",
                    "a player card"
                },
                mainMenuPage,
                MAIN_MENU_PAGES
            );

            break;


        // ----------------------------------------------------
        // Page 3 - Remove Balance
        // ----------------------------------------------------

        case 2:

            displayMenu(
                {
                    "REMOVE BALANCE",
                    "",
                    "Remove chips",
                    "from a card"
                },
                mainMenuPage,
                MAIN_MENU_PAGES
            );

            break;


        // ----------------------------------------------------
        // Page 4 - Back
        // ----------------------------------------------------

        case 3:

            displayMenu(
                {
                    "BACK",
                    "",
                    "Return to",
                    "home screen"
                },
                mainMenuPage,
                MAIN_MENU_PAGES
            );

            break;
    }
}


// ============================================================
// Main menu update
// ============================================================

void updateMainMenu() {

    // --------------------------------------------------------
    // Rotary movement
    // --------------------------------------------------------

    int8_t delta =
        encoderGetDelta();

    if (delta != 0) {

        mainMenuPage += delta;


        // Wrap around.

        if (
            mainMenuPage >=
            MAIN_MENU_PAGES
        ) {

            mainMenuPage = 0;
        }


        if (mainMenuPage < 0) {

            mainMenuPage =
                MAIN_MENU_PAGES - 1;
        }


        showMainMenu();
    }


    // --------------------------------------------------------
    // Button
    // --------------------------------------------------------

    if (encoderWasPressed()) {

        switch (mainMenuPage) {

            // ------------------------------------------------
            // New Player
            // ------------------------------------------------

            case 0:

                Serial.println(
                    "New Player selected."
                );

                kioskState =
                    NEW_PLAYER;

                showNewPlayerScreen();

                break;


            // ------------------------------------------------
            // Add Balance
            // ------------------------------------------------

            case 1:

                Serial.println(
                    "Add Balance selected."
                );

                balanceAddMode = true;

                beginBalanceAdjust();

                break;


            // ------------------------------------------------
            // Remove Balance
            // ------------------------------------------------

            case 2:

                Serial.println(
                    "Remove Balance selected."
                );

                balanceAddMode = false;

                beginBalanceAdjust();

                break;


            // ------------------------------------------------
            // Back
            // ------------------------------------------------

            case 3:

                Serial.println(
                    "Returning home."
                );

                mainMenuPage = 0;

                kioskState =
                    SCANNING;

                showScanScreen();

                break;
        }
    }
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
                    String(currentCard.chipBalance) +
                        " CHIPS"
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
                    String(
                        currentCard.transactionCount
                    ),
                    "GAMES WON",
                    "",
                    String(
                        currentCard.gamesWon
                    )
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
// Card menu update
// ============================================================

void updateCardMenu() {

    // --------------------------------------------------------
    // Rotary movement
    // --------------------------------------------------------

    int8_t delta =
        encoderGetDelta();

    if (delta != 0) {

        currentPage += delta;


        if (
            currentPage >=
            CARD_MENU_PAGES
        ) {

            currentPage = 0;
        }


        if (currentPage < 0) {

            currentPage =
                CARD_MENU_PAGES - 1;
        }


        showCardMenu();
    }


    // --------------------------------------------------------
    // Button
    // --------------------------------------------------------

    if (encoderWasPressed()) {

        switch (currentPage) {

            case 0:

                // Future action.

                break;


            case 1:

                // Future action.

                break;


            case 2:

                Serial.println(
                    "Returning to home."
                );

                kioskState =
                    SCANNING;

                currentPage = 0;

                showScanScreen();

                break;
        }
    }
}


// ============================================================
// New player screen
// ============================================================

void showNewPlayerScreen() {

    displayRows({
        "NEW PLAYER",
        "",
        TEMP_PLAYER_NAME,
        "START BALANCE",
        String(TEMP_PLAYER_START_BALANCE),
        "",
        "SCAN NEW CARD"
    });
}


// ============================================================
// New player update
// ============================================================

void updateNewPlayer() {

    // --------------------------------------------------------
    // Look for a card
    // --------------------------------------------------------

    if (!cardPresent()) {
        return;
    }


    Serial.println();
    Serial.println(
        "Card detected for new player."
    );


    // --------------------------------------------------------
    // Build new card
    // --------------------------------------------------------

    Card newCard;

    newCard.playerName =
        TEMP_PLAYER_NAME;

    newCard.chipBalance =
        TEMP_PLAYER_START_BALANCE;

    newCard.transactionCount =
        0;

    newCard.gamesWon =
        0;

    newCard.formatVersion =
        CARD_FORMAT_VERSION;


    displayRows({
        "NEW PLAYER",
        "",
        "WRITING CARD...",
        "",
        TEMP_PLAYER_NAME
    });


    // --------------------------------------------------------
    // Write
    // --------------------------------------------------------

    if (!cardWrite(newCard)) {

        Serial.println(
            "Failed to write new player card."
        );

        displayRows({
            "NEW PLAYER",
            "",
            "WRITE FAILED",
            "",
            "TRY AGAIN"
        });

        delay(1500);

        showNewPlayerScreen();

        return;
    }


    Serial.println(
        "New player card written."
    );


    // --------------------------------------------------------
    // Read back
    // --------------------------------------------------------

    delay(100);

    Card verifyCard;

    if (!cardRead(verifyCard)) {

        Serial.println(
            "Card write succeeded but "
            "verification failed."
        );

        displayRows({
            "NEW PLAYER",
            "",
            "VERIFY FAILED"
        });

        delay(1500);

        kioskState =
            SCANNING;

        showScanScreen();

        return;
    }


    currentCard =
        verifyCard;


    Serial.println(
        "New player card verified."
    );


    // --------------------------------------------------------
    // Enter card menu
    // --------------------------------------------------------

    currentPage = 0;

    kioskState =
        CARD_MENU;

    showCardMenu();
}


// ============================================================
// Begin balance adjustment
// ============================================================

void beginBalanceAdjust() {

    balanceCardLoaded = false;

    balanceAmount =
        TEMP_BALANCE_STEP;

    kioskState =
        BALANCE_ADJUST;

    showBalanceAdjustScreen();
}


// ============================================================
// Balance adjustment display
// ============================================================

void showBalanceAdjustScreen() {

    if (!balanceCardLoaded) {

        if (balanceAddMode) {

            displayRows({
                "ADD BALANCE",
                "",
                "SCAN PLAYER CARD"
            });

        } else {

            displayRows({
                "REMOVE BALANCE",
                "",
                "SCAN PLAYER CARD"
            });
        }

        return;
    }


    // --------------------------------------------------------
    // Loaded card
    // --------------------------------------------------------

    String action;

    if (balanceAddMode) {
        action = "ADD";
    } else {
        action = "REMOVE";
    }


    displayRows({
        action + " BALANCE",
        "",
        currentCard.playerName,
        "BALANCE " +
            String(currentCard.chipBalance),
        "AMOUNT " +
            String(balanceAmount),
        "",
        "TURN=AMOUNT",
        "PRESS=APPLY"
    });
}


// ============================================================
// Balance adjustment update
// ============================================================

void updateBalanceAdjust() {

    // ========================================================
    // Step 1 - Scan card
    // ========================================================

    if (!balanceCardLoaded) {

        Card card;

        if (!cardRead(card)) {
            return;
        }


        currentCard =
            card;

        balanceCardLoaded =
            true;

        balanceAmount =
            TEMP_BALANCE_STEP;

        Serial.println();
        Serial.println(
            "Player card loaded."
        );

        Serial.print(
            "Player: "
        );

        Serial.println(
            currentCard.playerName
        );

        Serial.print(
            "Balance: "
        );

        Serial.println(
            currentCard.chipBalance
        );


        showBalanceAdjustScreen();

        return;
    }


    // ========================================================
    // Step 2 - Change amount
    // ========================================================

    int8_t delta =
        encoderGetDelta();


    if (delta != 0) {

        // Clockwise / positive
        if (delta > 0) {

            if (
                balanceAmount <=
                TEMP_BALANCE_MAX -
                TEMP_BALANCE_STEP
            ) {

                balanceAmount +=
                    TEMP_BALANCE_STEP;
            }

        }

        // Counter-clockwise / negative
        else {

            if (
                balanceAmount >=
                TEMP_BALANCE_STEP
            ) {

                balanceAmount -=
                    TEMP_BALANCE_STEP;
            }
        }


        showBalanceAdjustScreen();
    }


    // ========================================================
    // Step 3 - Apply
    // ========================================================

    if (encoderWasPressed()) {

        // ----------------------------------------------------
        // Zero means cancel
        // ----------------------------------------------------

        if (balanceAmount == 0) {

            Serial.println(
                "Balance adjustment cancelled."
            );

            kioskState =
                SCANNING;

            showScanScreen();

            return;
        }


        // ----------------------------------------------------
        // Work on a copy
        // ----------------------------------------------------

        Card updatedCard =
            currentCard;


        bool changed;


        if (balanceAddMode) {

            changed =
                cardAddBalance(
                    updatedCard,
                    balanceAmount
                );

        } else {

            changed =
                cardRemoveBalance(
                    updatedCard,
                    balanceAmount
                );
        }


        // ----------------------------------------------------
        // Operation failed
        // ----------------------------------------------------

        if (!changed) {

            if (!balanceAddMode) {

                displayRows({
                    "REMOVE BALANCE",
                    "",
                    "NOT ENOUGH",
                    "CHIPS"
                });

            } else {

                displayRows({
                    "ADD BALANCE",
                    "",
                    "OPERATION",
                    "FAILED"
                });
            }

            delay(1500);

            showBalanceAdjustScreen();

            return;
        }


        // ----------------------------------------------------
        // Write new card data
        // ----------------------------------------------------

        if (!cardWrite(updatedCard)) {

            Serial.println(
                "Failed to write balance change."
            );

            displayRows({
                "BALANCE UPDATE",
                "",
                "WRITE FAILED"
            });

            delay(1500);

            kioskState =
                SCANNING;

            showScanScreen();

            return;
        }


        // ----------------------------------------------------
        // Commit in-memory change
        // ----------------------------------------------------

        currentCard =
            updatedCard;


        Serial.println();

        if (balanceAddMode) {

            Serial.print(
                "Added "
            );

        } else {

            Serial.print(
                "Removed "
            );
        }

        Serial.print(
            balanceAmount
        );

        Serial.println(
            " chips."
        );

        Serial.print(
            "New balance: "
        );

        Serial.println(
            currentCard.chipBalance
        );


        // ----------------------------------------------------
        // Success
        // ----------------------------------------------------

        if (balanceAddMode) {

            displayRows({
                "BALANCE UPDATED",
                "",
                "ADDED",
                String(balanceAmount),
                "NEW BALANCE",
                String(currentCard.chipBalance)
            });

        } else {

            displayRows({
                "BALANCE UPDATED",
                "",
                "REMOVED",
                String(balanceAmount),
                "NEW BALANCE",
                String(currentCard.chipBalance)
            });
        }


        delay(1500);


        // ----------------------------------------------------
        // Return home
        // ----------------------------------------------------

        kioskState =
            SCANNING;

        showScanScreen();
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
    // Startup
    // --------------------------------------------------------

    displayRows({
        "CHIP KIOSK",
        "STARTING..."
    });

    delay(500);


    // --------------------------------------------------------
    // NFC
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
    // Home
    // --------------------------------------------------------

    showScanScreen();

    Serial.println(
        "System ready."
    );
}


// ============================================================
// Main loop
// ============================================================

void loop() {

    // ========================================================
    // Encoder MUST be updated constantly
    // ========================================================

    encoderUpdate();


    // ========================================================
    // Home / scanning
    // ========================================================

    if (
        kioskState ==
        SCANNING
    ) {

        // ----------------------------------------------------
        // Card scan
        // ----------------------------------------------------

        Card card;

        if (cardRead(card)) {

            currentCard =
                card;


            Serial.println();
            Serial.println(
                "Card found!"
            );


            Serial.print(
                "UID: "
            );

            Serial.println(
                currentCard.uid
            );


            Serial.print(
                "Player: "
            );

            Serial.println(
                currentCard.playerName
            );


            Serial.print(
                "Balance: "
            );

            Serial.println(
                currentCard.chipBalance
            );


            Serial.print(
                "Transactions: "
            );

            Serial.println(
                currentCard.transactionCount
            );


            Serial.print(
                "Games won: "
            );

            Serial.println(
                currentCard.gamesWon
            );


            currentPage = 0;

            kioskState =
                CARD_MENU;

            showCardMenu();
        }


        // ----------------------------------------------------
        // Main menu button
        // ----------------------------------------------------

        if (encoderWasPressed()) {

            mainMenuPage = 0;

            kioskState =
                MAIN_MENU;

            showMainMenu();
        }
    }


    // ========================================================
    // Main menu
    // ========================================================

    else if (
        kioskState ==
        MAIN_MENU
    ) {

        updateMainMenu();
    }


    // ========================================================
    // Card menu
    // ========================================================

    else if (
        kioskState ==
        CARD_MENU
    ) {

        updateCardMenu();
    }


    // ========================================================
    // New player
    // ========================================================

    else if (
        kioskState ==
        NEW_PLAYER
    ) {

        updateNewPlayer();
    }


    // ========================================================
    // Balance adjustment
    // ========================================================

    else if (
        kioskState ==
        BALANCE_ADJUST
    ) {

        updateBalanceAdjust();
    }


    // ========================================================
    // Keep encoder responsive
    // ========================================================

    delay(1);
}