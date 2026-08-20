#include "FingerprintManager.h"

FingerprintManager::FingerprintManager(Adafruit_Fingerprint& sensor) : finger(sensor) {}

bool FingerprintManager::setupSensor(uint32_t baudRate) {
    finger.begin(baudRate);
    delay(100);
    if (finger.verifyPassword()) {
        return true;
    } else {
        return false;
    }
}

bool FingerprintManager::verificarHuella(uint16_t* idEncontrado) {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) return false;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return false;

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
        if (idEncontrado != nullptr) {
            *idEncontrado = finger.fingerID;
        }
        return true; 
    }
    return false; 
}

int FingerprintManager::registrarHuella(uint16_t id, DisplayOLED& display) {
    int p = -1;

    display.mostrarTexto("Registro Huella", "Coloque dedo");
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        if (p == FINGERPRINT_NOFINGER) {
            delay(50);
            continue;
        }
        if (p != FINGERPRINT_OK) return 0; 
    }

    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK) return 0;

    display.mostrarTexto("Paso 1 listo", "Retire el dedo");
    
    delay(1000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
        delay(50);
    }

    display.mostrarTexto("Confirme", "Coloque de nuevo");
    p = -1;
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        if (p == FINGERPRINT_NOFINGER) {
            delay(50);
            continue;
        }
        if (p != FINGERPRINT_OK) return 0;
    }

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK) return 0;

    p = finger.createModel();
    if (p != FINGERPRINT_OK) return 0;

    p = finger.storeModel(id);
    if (p != FINGERPRINT_OK) return 0;

    return 2;
}

int16_t FingerprintManager::obtenerIDLibre() {
    for (int id = 1; id <= 127; id++) {
        uint8_t p = finger.loadModel(id);
        if (p != FINGERPRINT_OK) {
            return id; 
        }
    }
    return -1; 
}

// NUEVA FUNCIÓN: Envía el comando de borrado al chip JM-101B
bool FingerprintManager::borrarHuella(uint16_t id) {
    uint8_t p = finger.deleteModel(id);
    return (p == FINGERPRINT_OK);
}