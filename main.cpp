#include <Arduino.h>
#include "WebServerManager.h"
#include "FingerprintManager.h"
#include "KeypadESP32.h"
#include "proximity.h"
#include "screen.h"
#include "temp.h"

// Configuración de los relés
#define RELAY_PIN_1 19 // Relé para la cerradura
#define RELAY_PIN_2 18 // Relé extra, solo definido por ahora

#define RX_FINGER 16
#define TX_FINGER 17

UserManager gestorUsuarios;

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
FingerprintManager fingerprintManager(finger);
KeypadESP32 keypad;
DisplayOLED display;

WebServerManager webServer("Cerradura_IoT", "12345678", &gestorUsuarios, &fingerprintManager);

enum EstadoSistema {
    ESTADO_NORMAL,
    ESTADO_INGRESO_CLAVE,
    ESTADO_ESPERA_ADMIN,
    ESTADO_REGISTRO_HUELLA,
    ESTADO_BLOQUEADO
};

EstadoSistema estadoActual = ESTADO_NORMAL;
String bufferEntrada = "";
int contadorErrores = 0;

const String CLAVE_ACCESO = "1234";
const String CLAVE_ADMIN = "0403";

unsigned long tiempoUltimaActividad = 0;
bool pantallaEncendida = true;

void procesarTecla(char tecla);
void registrarNuevaHuella();

void abrirPuertaRemota() {
    display.mostrarTexto("Apertura Remota", "Bienvenido");
    digitalWrite(RELAY_PIN_1, LOW);
    delay(1000); // 1 segundo de activación
    digitalWrite(RELAY_PIN_1, HIGH);
    display.mostrarTexto("Sistema Listo", "Bienvenido");
}

void setup() {
    Serial.begin(115200);
    
    // Inicializar ambos relés
    pinMode(RELAY_PIN_1, OUTPUT);
    digitalWrite(RELAY_PIN_1, HIGH);
    pinMode(RELAY_PIN_2, OUTPUT);
    digitalWrite(RELAY_PIN_2, HIGH);

    keypad.init();
    proximity_init(); 
    initTempSensor();

    if (display.inicializar()) {
        display.mostrarTexto("Sistema Listo", "Bienvenido");
    }

    mySerial.begin(57600, SERIAL_8N1, RX_FINGER, TX_FINGER);
    fingerprintManager.setupSensor(57600);
    gestorUsuarios.begin();
    
    tiempoUltimaActividad = millis();
    webServer.setUnlockCallback(abrirPuertaRemota);
    webServer.begin();
}

void loop() {
    char tecla = keypad.readKey(); 
    bool hayMovimiento = proximity_detect(); 

    if (tecla != '\0' || hayMovimiento) {
        tiempoUltimaActividad = millis(); 
        if (!pantallaEncendida) {
            display.despertar();
            pantallaEncendida = true;
            if (estadoActual == ESTADO_NORMAL) display.mostrarTexto("Sistema Listo", "Bienvenido");
        }
    }

    if (pantallaEncendida && estadoActual == ESTADO_NORMAL) {
        if (millis() - tiempoUltimaActividad > 5000) {
            display.dormir();
            pantallaEncendida = false;
        }
    }

    if (tecla != '\0') procesarTecla(tecla);

    if (estadoActual == ESTADO_NORMAL && pantallaEncendida) {
        uint16_t idHuella = 0;
        if (fingerprintManager.verificarHuella(&idHuella)) {
            String nombreAsociado = gestorUsuarios.obtenerNombrePorID(idHuella);
            if (nombreAsociado == "" || nombreAsociado == "Desconocido") nombreAsociado = "ID: " + String(idHuella);
            
            display.mostrarTexto("Acceso concedido", nombreAsociado);
            digitalWrite(RELAY_PIN_1, LOW);
            delay(1000); // 1 segundo de activación
            digitalWrite(RELAY_PIN_1, HIGH);
            display.mostrarTexto("Sistema Listo", "Bienvenido");
            tiempoUltimaActividad = millis(); 
        }
    }
}

void procesarTecla(char tecla) {
    if (tecla == '*') {
        bufferEntrada = ""; 
        if (estadoActual != ESTADO_BLOQUEADO) {
            estadoActual = ESTADO_NORMAL;
            display.mostrarTexto("Cancelado", "Bienvenido");
            delay(1000);
            display.mostrarTexto("Sistema Listo", "Bienvenido");
        }
        return; 
    }

    switch (estadoActual) {
        case ESTADO_NORMAL:
            if (tecla == 'A') {
                estadoActual = ESTADO_ESPERA_ADMIN;
                bufferEntrada = "";
                display.mostrarTexto("Modo Admin", "Clave: ");
            } else if (tecla >= '0' && tecla <= '9') {
                estadoActual = ESTADO_INGRESO_CLAVE;
                bufferEntrada = String(tecla);
                display.mostrarTexto("Ingreso:", bufferEntrada); 
            }
            break;

        case ESTADO_INGRESO_CLAVE:
            if (tecla >= '0' && tecla <= '9') {
                bufferEntrada += tecla;
                display.mostrarTexto("Ingreso:", bufferEntrada); 
            } else if (tecla == '#') { 
                String nombreAsociado = gestorUsuarios.obtenerNombrePorClave(bufferEntrada);
                if (nombreAsociado != "" || bufferEntrada == CLAVE_ACCESO) { 
                    if (nombreAsociado == "") nombreAsociado = "Admin Local";
                    display.mostrarTexto("Acceso concedido", nombreAsociado);
                    delay(1500); 
                    digitalWrite(RELAY_PIN_1, LOW);
                    delay(1000); // 1 segundo de activación
                    digitalWrite(RELAY_PIN_1, HIGH);
                    display.mostrarTexto("Sistema Listo", "Bienvenido");
                    contadorErrores = 0;
                } else {
                    contadorErrores++;
                    if (contadorErrores >= 3) {
                        estadoActual = ESTADO_BLOQUEADO;
                        display.mostrarTexto("Bloqueado", "Esperar admin");
                    } else {
                        display.mostrarTexto("Error", "Clave fallida");
                        delay(1000);
                        display.mostrarTexto("Sistema Listo", "Bienvenido");
                    }
                }
                bufferEntrada = ""; 
                if (estadoActual != ESTADO_BLOQUEADO) estadoActual = ESTADO_NORMAL;
            }
            break;

        case ESTADO_ESPERA_ADMIN:
             if (tecla >= '0' && tecla <= '9') {
                bufferEntrada += tecla;
                display.mostrarTexto("Modo Admin", bufferEntrada); 
            } else if (tecla == '#') { 
                if (bufferEntrada == CLAVE_ADMIN) {
                    estadoActual = ESTADO_REGISTRO_HUELLA;
                    registrarNuevaHuella(); 
                } else {
                    estadoActual = ESTADO_NORMAL;
                    display.mostrarTexto("Error Admin", "Cancelado");
                    delay(1000);
                    display.mostrarTexto("Sistema Listo", "Bienvenido");
                }
                bufferEntrada = "";
            }
            break;

        case ESTADO_BLOQUEADO:
             if (tecla >= '0' && tecla <= '9') {
                bufferEntrada += tecla;
                display.mostrarTexto("Desbloqueo adm:", bufferEntrada); 
            } else if (tecla == '#') { 
                if (bufferEntrada == CLAVE_ADMIN) { 
                    contadorErrores = 0;
                    estadoActual = ESTADO_NORMAL;
                    display.mostrarTexto("Desbloqueado", "Bienvenido");
                } else {
                    display.mostrarTexto("Bloqueado", "Esperar admin");
                }
                bufferEntrada = "";
            }
            break;
            
        case ESTADO_REGISTRO_HUELLA:
            break;
    }
}

void registrarNuevaHuella() {
    int idLibre = fingerprintManager.obtenerIDLibre();
    if (idLibre != -1) {
        int resultado = fingerprintManager.registrarHuella(idLibre, display);
        if (resultado == 2) {
            display.mostrarDato("Guardada ID:", (float)idLibre);
        } else {
            display.mostrarTexto("Error", "Fallo lectura");
        }
    } else {
        display.mostrarTexto("Error", "Memoria llena");
    }
    
    delay(2000); 
    estadoActual = ESTADO_NORMAL;
    display.mostrarTexto("Sistema Listo", "Bienvenido");
    tiempoUltimaActividad = millis(); 
}