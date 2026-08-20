#include "proximity.h"
#include <Arduino.h>

const int PIN_PROXIMIDAD = 39;

void proximity_init() {
    pinMode(PIN_PROXIMIDAD, INPUT);
}

bool proximity_detect() {
    return digitalRead(PIN_PROXIMIDAD) == HIGH;
}