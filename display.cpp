#include "display.h"
#include "config.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 display(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

void displayBegin() {

    if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
    )) {
        Serial.println("OLED initialization failed!");

        while (true) {
            delay(100);
        }
    }

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.display();
}

void displayClear() {

    display.clearDisplay();
}

void displayShow() {

    display.display();
}

void displayRow(uint8_t row, const String &text) {

    // 8 pixels per text row at text size 1
    display.setCursor(0, row * 8);

    display.print(text);
}

void displayRows(std::initializer_list<String> rows) {

    display.clearDisplay();

    uint8_t row = 0;

    for (const String &text : rows) {

        if (row >= 8) {
            break;
        }

        display.setCursor(0, row * 8);
        display.print(text);

        row++;
    }

    display.display();
}