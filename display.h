#pragma once

#include <Arduino.h>

void displayBegin();

void displayClear();

void displayShow();

void displayRow(uint8_t row, const String &text);

void displayRows(std::initializer_list<String> rows);