#ifndef FINGERPRINT_MANAGER_H
#define FINGERPRINT_MANAGER_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include "screen.h"

class FingerprintManager {
private:
    Adafruit_Fingerprint& finger;

public:
    FingerprintManager(Adafruit_Fingerprint& sensor);
    bool setupSensor(uint32_t baudRate = 57600);
    bool verificarHuella(uint16_t* idEncontrado = nullptr);
    int registrarHuella(uint16_t id, DisplayOLED& display);
    
    // Escanea la memoria y devuelve el primer ID vacío
    int16_t obtenerIDLibre(); 
    
    // NUEVA FUNCIÓN: Borra la huella físicamente del sensor
    bool borrarHuella(uint16_t id);
};

#endif