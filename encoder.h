#pragma once

#include <Arduino.h>

// Start the encoder hardware.
void encoderBegin();

// Must be called regularly from loop().
void encoderUpdate();

// Returns the number of encoder steps since
// the previous call, normally -1, 0, or +1.
int8_t encoderGetDelta();

// Returns true once when the encoder button
// has been pressed.
bool encoderWasPressed();