#pragma once

#include <Arduino.h>

enum KeyboardResult {

    KEYBOARD_NONE,
    KEYBOARD_DONE
};

void keyboardReset();

void keyboardDraw();

KeyboardResult keyboardUpdate();

const String &keyboardGetText();