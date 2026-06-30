#include "matrix.h"

CRGB leds[NUM_LEDS];

void setupMatrix() {
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

int XY(int x, int y) {
    // Typewriter layout: Row by row from top to bottom.
    // 0 is top-left.
    // Ensure bounds are respected.
    if (x < 0 || x >= MATRIX_W || y < 0 || y >= MATRIX_H) {
        return 0; // Return 0 or handle out of bounds safely
    }
    return (y * MATRIX_W) + x;
}
