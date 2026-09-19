#include "card.h"
#include "nfc.h"
#include "config.h"

#include <cstring>

// =========================
// Card layout
// =========================

// NTAG215 user memory starts at page 4.

static const uint8_t CARD_START_PAGE = 4;

// Payload:
// page 4      = 4 bytes
// pages 5-9  = 20 bytes name
// pages 10-11 = 8 bytes balance
// pages 12-13 = 8 bytes transactions
// pages 14-15 = 8 bytes games won
//
// Total payload = 48 bytes
//
// page 16 = CRC32

static const uint8_t CARD_PAYLOAD_PAGES = 12;
static const uint8_t CARD_CRC_PAGE = 16;

// 19 characters + null terminator
static const uint8_t CARD_NAME_SIZE = 20;

// Magic bytes: "CB"
static const uint8_t CARD_MAGIC_0 = 'C';
static const uint8_t CARD_MAGIC_1 = 'B';

// =========================
// 64-bit serialization
// =========================

static void writeU64(
    uint8_t *buffer,
    uint64_t value
) {

    for (uint8_t i = 0; i < 8; i++) {

        buffer[i] =
            (uint8_t)(value >> (i * 8));
    }
}

static uint64_t readU64(
    const uint8_t *buffer
) {

    uint64_t value = 0;

    for (uint8_t i = 0; i < 8; i++) {

        value |=
            ((uint64_t)buffer[i]) << (i * 8);
    }

    return value;
}

// =========================
// CRC32
// =========================

static uint32_t crc32(
    const uint8_t *data,
    size_t length
) {

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {

        crc ^= data[i];

        for (uint8_t bit = 0; bit < 8; bit++) {

            if (crc & 1) {

                crc =
                    (crc >> 1) ^
                    0xEDB88320;

            } else {

                crc >>= 1;
            }
        }
    }

    return ~crc;
}

// =========================
// Card presence
// =========================

bool cardPresent() {

    return nfcCardPresent();
}

// =========================
// Build card payload
// =========================

static bool buildPayload(
    const Card &card,
    uint8_t payload[48]
) {

    if (card.formatVersion != CARD_FORMAT_VERSION) {
        return false;
    }

    if (card.playerName.length() >= CARD_NAME_SIZE) {
        return false;
    }

    memset(
        payload,
        0,
        48
    );

    // ---------------------
    // Header
    // ---------------------

    payload[0] = CARD_MAGIC_0;
    payload[1] = CARD_MAGIC_1;
    payload[2] = card.formatVersion;
    payload[3] = 0; // reserved flags

    // ---------------------
    // Player name
    // ---------------------

    card.playerName.getBytes(
        payload + 4,
        CARD_NAME_SIZE
    );

    // ---------------------
    // Chip balance
    // ---------------------

    writeU64(
        payload + 24,
        card.chipBalance
    );

    // ---------------------
    // Transaction count
    // ---------------------

    writeU64(
        payload + 32,
        card.transactionCount
    );

    // ---------------------
    // Games won
    // ---------------------

    writeU64(
        payload + 40,
        card.gamesWon
    );

    return true;
}

// =========================
// Parse card payload
// =========================

static bool parsePayload(
    const uint8_t payload[48],
    uint32_t storedCRC,
    Card &card
) {

    // ---------------------
    // Check magic
    // ---------------------

    if (payload[0] != CARD_MAGIC_0 ||
        payload[1] != CARD_MAGIC_1) {

        return false;
    }

    // ---------------------
    // Check format version
    // ---------------------

    if (payload[2] != CARD_FORMAT_VERSION) {

        return false;
    }

    // ---------------------
    // Check CRC
    // ---------------------

    uint32_t calculatedCRC =
        crc32(
            payload,
            48
        );

    if (calculatedCRC != storedCRC) {

        return false;
    }

    // ---------------------
    // Player name
    // ---------------------

    char nameBuffer[
        CARD_NAME_SIZE + 1
    ];

    memcpy(
        nameBuffer,
        payload + 4,
        CARD_NAME_SIZE
    );

    nameBuffer[CARD_NAME_SIZE] = '\0';

    card.playerName =
        String(nameBuffer);

    // ---------------------
    // Values
    // ---------------------

    card.formatVersion =
        payload[2];

    card.chipBalance =
        readU64(payload + 24);

    card.transactionCount =
        readU64(payload + 32);

    card.gamesWon =
        readU64(payload + 40);

    return true;
}

// =========================
// Read card
// =========================

bool cardRead(
    Card &card
) {

    // First select/read the card
    String uid;

    if (!nfcReadUID(uid)) {
        return false;
    }

    card.uid = uid;

    // ---------------------
    // Read payload
    // ---------------------

    uint8_t payload[48];

    for (
        uint8_t pageIndex = 0;
        pageIndex < CARD_PAYLOAD_PAGES;
        pageIndex++
    ) {

        uint8_t pageData[4];

        uint8_t page =
            CARD_START_PAGE + pageIndex;

        if (!nfcReadPage(
            page,
            pageData
        )) {

            return false;
        }

        memcpy(
            payload + (pageIndex * 4),
            pageData,
            4
        );
    }

    // ---------------------
    // Read CRC
    // ---------------------

    uint8_t crcData[4];

    if (!nfcReadPage(
        CARD_CRC_PAGE,
        crcData
    )) {

        return false;
    }

    uint32_t storedCRC =
        ((uint32_t)crcData[0]) |
        ((uint32_t)crcData[1] << 8) |
        ((uint32_t)crcData[2] << 16) |
        ((uint32_t)crcData[3] << 24);

    // ---------------------
    // Validate + decode
    // ---------------------

    return parsePayload(
        payload,
        storedCRC,
        card
    );
}

// =========================
// Write card
// =========================

bool cardWrite(
    const Card &card
) {

    // Card must already be a valid
    // version-1 record.
    if (card.formatVersion != CARD_FORMAT_VERSION) {
        return false;
    }

    // Maximum 19 characters.
    if (card.playerName.length() >= CARD_NAME_SIZE) {
        return false;
    }

    // Make sure a card is actually present.
    String uid;

    if (!nfcReadUID(uid)) {
        return false;
    }

    // ---------------------
    // Build payload
    // ---------------------

    uint8_t payload[48];

    if (!buildPayload(
        card,
        payload
    )) {

        return false;
    }

    // ---------------------
    // Calculate CRC
    // ---------------------

    uint32_t crc =
        crc32(
            payload,
            48
        );

    // ---------------------
    // Write payload
    // ---------------------

    for (
        uint8_t pageIndex = 0;
        pageIndex < CARD_PAYLOAD_PAGES;
        pageIndex++
    ) {

        uint8_t pageData[4];

        memcpy(
            pageData,
            payload + (pageIndex * 4),
            4
        );

        uint8_t page =
            CARD_START_PAGE + pageIndex;

        if (!nfcWritePage(
            page,
            pageData
        )) {

            return false;
        }

        delay(10);
    }

    // ---------------------
    // Write CRC LAST
    // ---------------------

    uint8_t crcData[4];

    crcData[0] =
        (uint8_t)(crc & 0xFF);

    crcData[1] =
        (uint8_t)((crc >> 8) & 0xFF);

    crcData[2] =
        (uint8_t)((crc >> 16) & 0xFF);

    crcData[3] =
        (uint8_t)((crc >> 24) & 0xFF);

    if (!nfcWritePage(
        CARD_CRC_PAGE,
        crcData
    )) {

        return false;
    }

    return true;
}

// =========================
// Format new card
// =========================

bool cardFormat(
    Card &card
) {

    // Find the physical card.
    String uid;

    if (!nfcReadUID(uid)) {
        return false;
    }

    card.uid =
        uid;

    // Set up a brand-new record.

    card.formatVersion =
        CARD_FORMAT_VERSION;

    card.transactionCount =
        0;

    card.gamesWon =
        0;

    return cardWrite(card);
}

// ============================================================
// Balance operations
// ============================================================

bool cardSetBalance(
    Card &card,
    uint64_t newBalance
) {

    // No change means no transaction.
    if (newBalance == card.chipBalance) {
        return true;
    }

    // Prevent transaction counter overflow.
    if (card.transactionCount == UINT64_MAX) {
        return false;
    }

    card.chipBalance = newBalance;
    card.transactionCount++;

    return true;
}


// ============================================================
// Add chips
// ============================================================

bool cardAddBalance(
    Card &card,
    uint64_t amount
) {

    // Nothing to add.
    if (amount == 0) {
        return true;
    }

    // Prevent balance overflow.
    if (amount > UINT64_MAX - card.chipBalance) {
        return false;
    }

    return cardSetBalance(
        card,
        card.chipBalance + amount
    );
}


// ============================================================
// Remove chips
// ============================================================

bool cardRemoveBalance(
    Card &card,
    uint64_t amount
) {

    // Not enough chips.
    if (amount > card.chipBalance) {
        return false;
    }

    // Nothing to remove.
    if (amount == 0) {
        return true;
    }

    return cardSetBalance(
        card,
        card.chipBalance - amount
    );
}


// ============================================================
// Record a win
// ============================================================

bool cardRecordWin(
    Card &card
) {

    // Prevent counter overflow.
    if (card.gamesWon == UINT64_MAX) {
        return false;
    }

    card.gamesWon++;

    return true;
}