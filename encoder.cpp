#include "encoder.h"
#include "config.h"

// =========================
// Rotary encoder state
// =========================

static uint8_t encoderState = 0;
static int8_t encoderAccumulator = 0;

// A full quadrature cycle is normally
// four transitions.
static int8_t encoderDelta = 0;

// =========================
// Button state
// =========================

static bool buttonRawState = HIGH;
static bool buttonStableState = HIGH;

static unsigned long buttonLastChange = 0;

static bool buttonPressedEvent = false;

static const unsigned long BUTTON_DEBOUNCE_MS = 30;

// =========================
// Quadrature lookup table
// =========================
//
// Index:
// previous state << 2 | new state
//
// Invalid transitions return 0.
// This makes the encoder much less
// sensitive to contact bounce.

static const int8_t encoderTable[16] = {

     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

// =========================
// Begin
// =========================

void encoderBegin() {

    pinMode(
        ENCODER_CLK,
        INPUT_PULLUP
    );

    pinMode(
        ENCODER_DT,
        INPUT_PULLUP
    );

    pinMode(
        ENCODER_SW,
        INPUT_PULLUP
    );

    // Read the initial quadrature state.

    uint8_t clk =
        digitalRead(ENCODER_CLK);

    uint8_t dt =
        digitalRead(ENCODER_DT);

    encoderState =
        (clk << 1) | dt;

    // Initial button state.

    buttonRawState =
        digitalRead(ENCODER_SW);

    buttonStableState =
        buttonRawState;

    buttonLastChange =
        millis();
}

// =========================
// Update
// =========================

void encoderUpdate() {

    // -------------------------
    // Rotary encoder
    // -------------------------

    uint8_t clk =
        digitalRead(ENCODER_CLK);

    uint8_t dt =
        digitalRead(ENCODER_DT);

    uint8_t newState =
        (clk << 1) | dt;

    if (newState != encoderState) {

        uint8_t tableIndex =
            (encoderState << 2) | newState;

        int8_t movement =
            encoderTable[tableIndex];

        encoderAccumulator += movement;

        encoderState =
            newState;

        // Only generate a step after a
        // complete detent's worth of movement.

        if (encoderAccumulator >= 4) {

            encoderDelta++;

            encoderAccumulator = 0;

        } else if (encoderAccumulator <= -4) {

            encoderDelta--;

            encoderAccumulator = 0;
        }
    }

    // -------------------------
    // Button
    // -------------------------

    bool newButtonState =
        digitalRead(ENCODER_SW);

    if (newButtonState != buttonRawState) {

        buttonRawState =
            newButtonState;

        buttonLastChange =
            millis();
    }

    // Has the new state stayed stable
    // for the debounce period?

    if (
        buttonRawState != buttonStableState &&
        millis() - buttonLastChange >=
            BUTTON_DEBOUNCE_MS
    ) {

        bool oldState =
            buttonStableState;

        buttonStableState =
            buttonRawState;

        // Active-low button:
        // HIGH -> LOW = press

        if (
            oldState == HIGH &&
            buttonStableState == LOW
        ) {

            buttonPressedEvent = true;
        }
    }
}

// =========================
// Get encoder movement
// =========================

int8_t encoderGetDelta() {

    int8_t delta =
        encoderDelta;

    encoderDelta = 0;

    if (ENCODER_REVERSED) {
        delta = -delta;
    }

    return delta;
}

// =========================
// Get button event
// =========================

bool encoderWasPressed() {

    if (!buttonPressedEvent) {
        return false;
    }

    buttonPressedEvent = false;

    return true;
}