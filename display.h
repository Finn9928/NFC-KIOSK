#pragma once

#include <Arduino.h>
#include <initializer_list>

void displayBegin();

void displayClear();

void displayShow();

void displayRow(uint8_t row, const String &text);

void displayRows(std::initializer_list<String> rows);

void displayMenu(
    std::initializer_list<String> rows,
    uint8_t currentPage,
    uint8_t totalPages
);