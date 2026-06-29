//from including library to setup are required everytime
#include <FastLED.h> //the Library

const int TOTAL_LIGHTS =  64; //number of LEDS
const int DATA_PIN = 5; // Arduino Data line
CRGB leds[TOTAL_LIGHTS]; //rgb LEDs count

void setup() { //setup
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, TOTAL_LIGHTS); //telling arduino the model, which line to send output and number leds
  FastLED.setBrightness(50); //brightness
}

//the code is below
void loop() {
  for (int i = 0; i < 64; i++) { //loops from 0 to 63
    if (i < 32) { //if 0 to 31
      leds[i] = CRGB::Red; //flash red
    }
    else { // if not 0 to 31
      leds[i] = CRGB::Blue; // flash blue
    }
  }
  FastLED.show(); //showing colors
  delay(500); //delay means delay itself

  for (int i = 0; i < 64; i++) { //same loop, reversed color
    if (i < 32) {
      leds[i] = CRGB::Blue;
    }
    else {
      leds[i] = CRGB::Red;
    }
  }
  FastLED.show();
  delay(500);
}

//change color if u want, increase or decrease speed by adjusting delay() and increase or decrease brightness. Do as u wish, programmer
