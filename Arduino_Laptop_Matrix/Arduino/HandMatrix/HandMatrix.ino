#include <Arduino.h>
#include "matrix.h"
#include "animations.h"
#include "serial_handler.h"

void setup() {
    Serial.begin(115200);
    setupMatrix();
    
    // Start with a '?' or 'X' symbol waiting for python
    setTargetDigit('X');
}

void loop() {
    processSerial();
    updateAnimations();
}
