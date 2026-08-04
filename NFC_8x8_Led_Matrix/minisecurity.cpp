#include <Wire.h>
#include <Adafruit_PN532.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

#define MATRIX_PIN  6
#define MATRIX_W    8
#define MATRIX_H    8

Adafruit_PN532 nfc(-1, -1);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    MATRIX_W, MATRIX_H, MATRIX_PIN,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800);

uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t BLOCK_NUM = 4;

enum Mode { MODE_IDLE, MODE_WRITE_PENDING, MODE_SECURITY };
Mode currentMode = MODE_IDLE;

uint8_t pendingWriteData[16];
uint8_t expectedData[16];
bool haveExpected = false;

unsigned long lastTapTime = 0;
const unsigned long TAP_COOLDOWN = 2000;

void setup() {
  Serial.begin(9600);
  delay(500); // PN532 needs a moment to boot before it'll respond

  Wire.begin();

  matrix.begin();
  matrix.setBrightness(20);
  matrix.fillScreen(0);
  matrix.show();
  delay(50);

  nfc.begin();

  Serial.println("Initializing NFC reader...");

  uint32_t versiondata = 0;
  int attempts = 0;
  while (!versiondata && attempts < 10) {
    versiondata = nfc.getFirmwareVersion();
    if (!versiondata) { delay(300); attempts++; }
  }
  if (!versiondata) {
    Serial.println("Didn't find PN532 board — check wiring/power.");
    while (1);
  }
  nfc.SAMConfig();
  delay(50);

  Serial.println("PN532 found.");
  printHelp();
}

void printHelp() {
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  w <text>   - write <text> to tag (max 16 chars), then back to idle");
  Serial.println("  set        - security mode (green = correct tag, red = wrong)");
  Serial.println("  idle       - back to idle/read mode");
  Serial.println();
  Serial.println("Current mode: IDLE");
}

void loop() {
  handleSerialCommands();

  switch (currentMode) {
    case MODE_IDLE:          loopIdle();          break;
    case MODE_WRITE_PENDING: loopWritePending();   break;
    case MODE_SECURITY:      loopSecurity();       break;
  }
}

void handleSerialCommands() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  if (line.startsWith("w ") || line.startsWith("W ")) {
    String text = line.substring(2);
    text.trim();

    if (text.length() > 16) {
      Serial.println("Error: text too long (max 16 characters). Not queued.");
      return;
    }

    memset(pendingWriteData, 0x00, 16); // pad to full 16 bytes
    for (int i = 0; i < text.length(); i++) {
      pendingWriteData[i] = (uint8_t)text[i];
    }

    currentMode = MODE_WRITE_PENDING;
    matrix.fillScreen(0);
    matrix.show();

    Serial.print("Queued write: \"");
    Serial.print(text);
    Serial.println("\" -> Tap the tag now...");
  }
  else if (line.equalsIgnoreCase("set")) {
    if (!haveExpected) {
      Serial.println("No code has been written yet — use 'w <text>' first.");
      return;
    }
    currentMode = MODE_SECURITY;
    matrix.fillScreen(0);
    matrix.show();
    Serial.println("Mode: SECURITY. Tap a tag to verify...");
  }
  else if (line.equalsIgnoreCase("idle")) {
    currentMode = MODE_IDLE;
    matrix.fillScreen(0);
    matrix.show();
    Serial.println("Mode: IDLE.");
  }
  else {
    Serial.println("Unknown command.");
    printHelp();
  }
}

bool authenticateWithRetry(uint8_t *uid, uint8_t uidLength, int maxAttempts = 3) {
  for (int i = 0; i < maxAttempts; i++) {
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, BLOCK_NUM, 0, keyA)) {
      return true;
    }
    delay(150);
  }
  return false;
}

void loopIdle() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500)) {
    if (millis() - lastTapTime < TAP_COOLDOWN) return;
    lastTapTime = millis();

    Serial.print("Tag detected. UID: ");
    for (int i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) Serial.print("0");
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void loopWritePending() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500)) {
    Serial.println("Tag detected. Writing...");
    delay(100);

    if (authenticateWithRetry(uid, uidLength)) {
      if (nfc.mifareclassic_WriteDataBlock(BLOCK_NUM, pendingWriteData)) {
        Serial.println("Write successful!");

        memcpy(expectedData, pendingWriteData, 16); // this becomes the new correct code
        haveExpected = true;

        flashMatrix(matrix.Color(0, 150, 255), 500);
      } else {
        Serial.println("Write failed.");
        flashMatrix(matrix.Color(255, 0, 0), 500);
      }
    } else {
      Serial.println("Authentication failed.");
      flashMatrix(matrix.Color(255, 0, 0), 500);
    }

    currentMode = MODE_IDLE;
    Serial.println("Mode: IDLE.");
  }
}

void loopSecurity() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500)) {
    if (millis() - lastTapTime < TAP_COOLDOWN) return;
    lastTapTime = millis();

    delay(100);
    bool match = false;

    if (authenticateWithRetry(uid, uidLength)) {
      uint8_t readData[16];
      if (nfc.mifareclassic_ReadDataBlock(BLOCK_NUM, readData)) {
        match = (memcmp(readData, expectedData, 16) == 0);

        Serial.print("Read: \"");
        for (int i = 0; i < 16 && readData[i] != 0; i++) Serial.print((char)readData[i]);
        Serial.print("\" -> ");
        Serial.println(match ? "MATCH" : "NO MATCH");
      } else {
        Serial.println("Read failed.");
      }
    } else {
      Serial.println("Authentication failed (wrong tag or unwritten sector).");
    }

    showResult(match);
    delay(1500);
    matrix.fillScreen(0);
    matrix.show();
  }
}

void showResult(bool correct) {
  uint16_t color = correct ? matrix.Color(0, 255, 0) : matrix.Color(255, 0, 0);
  matrix.fillScreen(color);
  matrix.show();
}

void flashMatrix(uint16_t color, int durationMs) {
  matrix.fillScreen(color);
  matrix.show();
  delay(durationMs);
  matrix.fillScreen(0);
  matrix.show();
}
