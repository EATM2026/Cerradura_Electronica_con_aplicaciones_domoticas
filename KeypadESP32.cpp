#include "KeypadESP32.h"

KeypadESP32::KeypadESP32() {
    // Constructor
}

void KeypadESP32::init() {
    // Configurar filas como entradas con resistencia pull-up
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], INPUT_PULLUP);
    }
    // Configurar columnas como salidas y ponerlas en ALTO
    for (int i = 0; i < 4; i++) {
        pinMode(colPins[i], OUTPUT);
        digitalWrite(colPins[i], HIGH);
    }
}

char KeypadESP32::readKey() {
    char pressedKey = '\0';
    for (int c = 0; c < 4; c++) {
        // Poner la columna actual en BAJO
        digitalWrite(colPins[c], LOW);
        
        for (int r = 0; r < 4; r++) {
            // Leer la fila
            if (digitalRead(rowPins[r]) == LOW) {
                delay(50); // Anti-rebote (debounce)
                if (digitalRead(rowPins[r]) == LOW) { 
                    pressedKey = keys[r][c];
                    // Esperar a que se suelte la tecla
                    while (digitalRead(rowPins[r]) == LOW); 
                }
            }
        }
        // Devolver la columna a ALTO
        digitalWrite(colPins[c], HIGH);
        
        if (pressedKey != '\0') break; // Si se encontró una tecla, salir
    }
    return pressedKey;
}

int KeypadESP32::getNumber() {
    char key = readKey();
    if (key >= '0' && key <= '9') {
        return key - '0'; // Convertir de char a int restando el valor ASCII
    }
    return -1; // -1 indica que no se presionó un número o no hay tecla
}

char KeypadESP32::getCharacter() {
    char key = readKey();
    if ((key >= 'A' && key <= 'D') || key == '*' || key == '#') {
        return key;
    }
    return '\0'; // '\0' (NULO) indica que no se presionó un caracter válido
}
