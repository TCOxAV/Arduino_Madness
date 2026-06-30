#include "animations.h"

// State machine for animations
enum AnimState {
    STATE_IDLE,       // Just breathing
    STATE_FADE_OUT,   // Fading old number out
    STATE_GROW,       // New number growing (or just fade in)
    STATE_PULSE       // Brief bright pulse on new number
};

AnimState currentState = STATE_IDLE;

char currentDigit = 'X';
char targetDigit = 'X';

// Timing
unsigned long lastUpdate = 0;
int animProgress = 0; // 0 to 255
int breathVal = 0;
bool breathUp = true;

// Helper to get bitmap
const byte* getBitmap(char c) {
    switch (c) {
        case '0': return digit_0;
        case '1': return digit_1;
        case '2': return digit_2;
        case '3': return digit_3;
        case '4': return digit_4;
        case '5': return digit_5;
        case '6': return digit_6;
        case '7': return digit_7;
        case '8': return digit_8;
        case '9': return digit_9;
        case 'A': return digit_10;
        case 'X': return digit_question;
        default:  return digit_dash;
    }
}

// Helper to draw a bitmap with a specific color and brightness multiplier
void drawBitmap(const byte* bitmap, CRGB color, float brightnessMult) {
    FastLED.clear();
    for (int y = 0; y < MATRIX_H; y++) {
        for (int x = 0; x < MATRIX_W; x++) {
            if (bitRead(bitmap[y], 7 - x)) {
                CRGB c = color;
                c.nscale8(255 * brightnessMult);
                leds[XY(x, y)] = c;
            }
        }
    }
}

CRGB getColor(char c) {
    if (c == 'X') return CRGB::Blue;
    return CRGB::Amethyst; // Base color for numbers
}

void setTargetDigit(char digit) {
    if (digit != targetDigit) {
        targetDigit = digit;
        if (targetDigit != currentDigit) {
            currentState = STATE_FADE_OUT;
            animProgress = 255; // start at full brightness for fade out
        }
    }
}

void updateAnimations() {
    unsigned long now = millis();
    if (now - lastUpdate < 15) { // ~60fps animation update
        return;
    }
    lastUpdate = now;

    switch (currentState) {
        case STATE_IDLE:
            {
                // Subtle breathing
                if (breathUp) {
                    breathVal += 2;
                    if (breathVal >= 100) breathUp = false;
                } else {
                    breathVal -= 2;
                    if (breathVal <= 40) breathUp = true;
                }
                float mult = breathVal / 255.0 + 0.3; // between 0.3 and 0.7 approx
                drawBitmap(getBitmap(currentDigit), getColor(currentDigit), mult);
            }
            break;

        case STATE_FADE_OUT:
            animProgress -= 15;
            if (animProgress <= 0) {
                animProgress = 0;
                currentDigit = targetDigit;
                currentState = STATE_GROW;
            }
            drawBitmap(getBitmap(currentDigit == targetDigit ? 'X' : currentDigit), getColor(currentDigit), animProgress / 255.0);
            break;

        case STATE_GROW:
            animProgress += 20;
            if (animProgress >= 255) {
                animProgress = 255;
                currentState = STATE_PULSE;
            }
            drawBitmap(getBitmap(currentDigit), getColor(currentDigit), animProgress / 255.0);
            break;

        case STATE_PULSE:
            animProgress -= 5;
            if (animProgress <= 180) { // Fall back down to idle brightness
                currentState = STATE_IDLE;
                breathVal = 80;
                breathUp = false;
            } else {
                drawBitmap(getBitmap(currentDigit), CRGB::White, 1.0); // Bright flash
            }
            break;
    }

    FastLED.show();
}
