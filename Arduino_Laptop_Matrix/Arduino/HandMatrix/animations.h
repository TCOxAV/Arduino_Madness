#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "matrix.h"
#include "digits.h"

// Set the target character to be displayed
// '0'-'5', 'X' for missing hand
void setTargetDigit(char digit);

// Main animation loop, must be called repeatedly in loop()
void updateAnimations();

#endif
