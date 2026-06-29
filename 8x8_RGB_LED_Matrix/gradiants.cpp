#include <Adafruit_NeoPixel.h>

#define LED_PIN        6   // The pin where the matrix data wire is connected
#define NUM_LEDS      64   // Total number of lights on the 8x8 matrix
#define MATRIX_ROWS    8
#define MATRIX_COLS    8

// Setup the matrix connection
Adafruit_NeoPixel matrix(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  matrix.begin();           // Turn on the matrix system
  matrix.setBrightness(25); // Set brightness to a low, safe level
  matrix.show();            // Make sure all lights start turned off
}

void loop() {
  static uint16_t colorShift = 0; // This number changes over time to move the colors

  // Go through every single light row by row, column by column
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      
      // Calculate a color for this specific spot based on its position and time
      uint32_t hue = colorShift + (row * 3000) + (col * 3000);
      
      // Convert that calculation into a bright rainbow color
      uint32_t pixelColor = matrix.ColorHSV(hue, 255, 255);
      
      // Find the exact number of the light (0 to 63)
      int pixelIndex = (row * MATRIX_COLS) + col;
      
      // Apply the color to that light
      matrix.setPixelColor(pixelIndex, pixelColor);
    }
  }

  // Send the colors to the actual physical matrix
  matrix.show();
  
  // Change the number slightly so the colors shift in the next frame
  colorShift += 250; 
  
  // A tiny pause to keep the animation smooth
  delay(10); 
}