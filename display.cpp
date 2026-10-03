#include "display.h"
#include "config.h"
#include "encoder.h"

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

void displayMenu(
    std::initializer_list<String> rows,
    uint8_t currentPage,
    uint8_t totalPages
) {

    display.clearDisplay();

    // --------------------------------------------------------
    // Draw menu content
    // --------------------------------------------------------

    uint8_t row = 0;

    for (const String &text : rows) {

        if (row >= 7) {
            break;
        }

        display.setCursor(
            0,
            row * 8
        );

        display.print(text);

        row++;
    }


    // --------------------------------------------------------
    // Page counter
    // --------------------------------------------------------

    String pageText =
        String(currentPage + 1) +
        "/" +
        String(totalPages);


    int16_t x =
        OLED_WIDTH -
        (pageText.length() * 6);


    if (x < 0) {
        x = 0;
    }


    display.setCursor(
        x,
        56
    );

    display.print(pageText);


    display.display();
}

// ============================================================
// Keyboard display
// ============================================================

void displayKeyboard(
    const String &preview,
    const char *const keys[],
    uint8_t keyCount,
    uint8_t selectedKey
) {

    display.clearDisplay();


    // --------------------------------------------------------
    // Name preview
    // --------------------------------------------------------

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setCursor(
        0,
        0
    );

    display.print(
        preview
    );


    // Separator
    display.drawFastHLine(
        0,
        7,
        OLED_WIDTH,
        SSD1306_WHITE
    );


    // --------------------------------------------------------
    // Keyboard
    //
    // 5 columns
    // 6 rows
    //
    // 24 x 8 pixels per key
    // --------------------------------------------------------

    const uint8_t KEY_WIDTH = 24;
    const uint8_t KEY_HEIGHT = 8;
    const uint8_t KEY_COLUMNS = 5;


    for (
        uint8_t i = 0;
        i < keyCount;
        i++
    ) {

        uint8_t column =
            i % KEY_COLUMNS;

        uint8_t row =
            i / KEY_COLUMNS;


        uint8_t x =
            column * KEY_WIDTH;

        uint8_t y =
            8 +
            row * 8;


        bool selected =
            (i == selectedKey);


        // ----------------------------------------------------
        // Key background / border
        // ----------------------------------------------------

        if (selected) {

            display.fillRect(
                x,
                y,
                KEY_WIDTH,
                KEY_HEIGHT,
                SSD1306_WHITE
            );

            display.setTextColor(
                SSD1306_BLACK
            );

        } else {

            display.drawRect(
                x,
                y,
                KEY_WIDTH + 1,
                KEY_HEIGHT + 1,
                SSD1306_WHITE
            );

            display.setTextColor(
                SSD1306_WHITE
            );
        }


        // ----------------------------------------------------
        // Centre key label
        // ----------------------------------------------------

        String label =
            keys[i];

        uint8_t textWidth =
            label.length() * 6;


        int16_t textX =
            x +
            ((KEY_WIDTH - textWidth) / 2);


        display.setCursor(
            textX,
            y + 1
        );

        display.print(
            label
        );
    }


    // --------------------------------------------------------
    // Character count
    // --------------------------------------------------------

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setCursor(
        0,
        56
    );

    display.print(
        "LENGTH "
    );

    display.print(
        preview.length()
    );

    display.print(
        "/"
    );

    display.print(
        CARD_NAME_MAX_LENGTH
    );


    display.display();
}