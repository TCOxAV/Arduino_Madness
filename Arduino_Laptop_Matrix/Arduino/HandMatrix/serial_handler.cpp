#include <Arduino.h>
#include "serial_handler.h"
#include "animations.h"

void processSerial() {
    while (Serial.available() > 0) {
        char inChar = Serial.read();
        
        // We expect '0'-'9', 'A' (for 10), or 'X'
        if ((inChar >= '0' && inChar <= '9') || inChar == 'A' || inChar == 'X') {
            setTargetDigit(inChar);
        }
    }
}
