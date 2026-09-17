#pragma once

#include <Arduino.h>

struct Card {

    String uid;

    String playerName;

    uint64_t chipBalance;

    uint64_t transactionCount;

    uint64_t gamesWon;

    uint8_t formatVersion;
};

bool cardRead(
    Card &card
);

bool cardWrite(
    const Card &card
);

bool cardFormat(
    Card &card
);

bool cardPresent();