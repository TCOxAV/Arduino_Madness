#include <FastLED.h>

#define LED_PIN 6
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS 64

CRGB leds[NUM_LEDS];

struct Drop {
  int x;
  int y;
};

Drop drops[8];

int XY(int x, int y) {
  return y * WIDTH + x;
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(80);

  randomSeed(analogRead(A0));

  for (int i = 0; i < 8; i++) {
    drops[i].x = random(0, WIDTH);
    drops[i].y = random(-HEIGHT, 0);
  }
}

void loop() {

  fadeToBlackBy(leds, NUM_LEDS, 80);

  for (int i = 0; i < 8; i++) {

    drops[i].y++;

    if (drops[i].y >= HEIGHT + 3) {
      drops[i].x = random(0, WIDTH);
      drops[i].y = random(-5, 0);
    }

    // Head
    if (drops[i].y >= 0 && drops[i].y < HEIGHT)
      leds[XY(drops[i].x, drops[i].y)] = CRGB::White;

    // Trail
    for (int t = 1; t <= 3; t++) {
      int yy = drops[i].y - t;

      if (yy >= 0 && yy < HEIGHT) {
        leds[XY(drops[i].x, yy)] =
          CHSV(96, 255, 255 / (t + 1));
      }
    }
  }

  FastLED.show();
  delay(100);
}
