#ifndef KEYPAD_ESP32_H
#define KEYPAD_ESP32_H

#include <Arduino.h>

class KeypadESP32 {
private:
    // Configuración de los pines según la secuencia del ESP32 
    // (Pin 1 a 8 del teclado en GPIOs consecutivos)
    uint8_t rowPins[4] = {32, 33, 25, 26}; // Pines 1, 2, 3, 4 del teclado
    uint8_t colPins[4] = {27, 14, 12, 13}; // Pines 5, 6, 7, 8 del teclado (Pin 8 es GPIO 13)
    
    char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    
    

public:
    KeypadESP32();
    char readKey();
    void init();
    int getNumber();
    char getCharacter();
};

#endif
