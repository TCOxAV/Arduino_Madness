#ifndef MATRIX_H
#define MATRIX_H

#include <FastLED.h>

// Matrix Config
#define LED_PIN     6
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    64
#define MATRIX_W    8
#define MATRIX_H    8

#define BRIGHTNESS  100

// External array holding LED data
extern CRGB leds[NUM_LEDS];

// Initializes the FastLED matrix
void setupMatrix();

// Converts (x,y) coordinates to the correct LED index
// Uses the standard typewriter / progressive layout: 0 at top left, 63 at bottom right
int XY(int x, int y);

#endif
