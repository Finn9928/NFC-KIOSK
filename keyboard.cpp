#include "keyboard.h"

#include "config.h"
#include "display.h"
#include "encoder.h"


// ============================================================
// Keyboard layout
// ============================================================

static const char *KEY_LABELS[] = {

    "A", "B", "C", "D", "E",
    "F", "G", "H", "I", "J",
    "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y",
    "Z", "SP", "BS", "OK"
};

static const uint8_t KEY_COUNT =
    sizeof(KEY_LABELS) /
    sizeof(KEY_LABELS[0]);


// ============================================================
// Keyboard state
// ============================================================

static String keyboardText;

static uint8_t selectedKey = 0;


// ============================================================
// Reset keyboard
// ============================================================

void keyboardReset() {

    keyboardText = "";

    selectedKey = 0;
}


// ============================================================
// Draw keyboard
// ============================================================

void keyboardDraw() {

    displayKeyboard(
        keyboardText,
        KEY_LABELS,
        KEY_COUNT,
        selectedKey
    );
}


// ============================================================
// Update keyboard
// ============================================================

KeyboardResult keyboardUpdate() {

    // --------------------------------------------------------
    // Rotary encoder
    // --------------------------------------------------------

    int8_t delta =
        encoderGetDelta();


    if (delta != 0) {

        int16_t newSelection =
            selectedKey + delta;


        if (newSelection < 0) {

            newSelection =
                KEY_COUNT - 1;
        }


        if (newSelection >= KEY_COUNT) {

            newSelection = 0;
        }


        selectedKey =
            newSelection;


        keyboardDraw();
    }


    // --------------------------------------------------------
    // Encoder button
    // --------------------------------------------------------

    if (encoderWasPressed()) {

        // ----------------------------------------------------
        // A-Z
        // ----------------------------------------------------

        if (selectedKey < 26) {

            if (
                keyboardText.length() <
                CARD_NAME_MAX_LENGTH
            ) {

                char letter =
                    'A' + selectedKey;

                keyboardText +=
                    letter;

                keyboardDraw();
            }
        }


        // ----------------------------------------------------
        // Space
        // ----------------------------------------------------

        else if (selectedKey == 26) {

            if (
                keyboardText.length() <
                CARD_NAME_MAX_LENGTH
            ) {

                keyboardText +=
                    ' ';

                keyboardDraw();
            }
        }


        // ----------------------------------------------------
        // Backspace
        // ----------------------------------------------------

        else if (selectedKey == 27) {

            if (
                keyboardText.length() > 0
            ) {

                keyboardText.remove(
                    keyboardText.length() - 1
                );

                keyboardDraw();
            }
        }


        // ----------------------------------------------------
        // Done
        // ----------------------------------------------------

        else if (selectedKey == 28) {

            return KEYBOARD_DONE;
        }
    }


    return KEYBOARD_NONE;
}


// ============================================================
// Get current text
// ============================================================

const String &keyboardGetText() {

    return keyboardText;
}