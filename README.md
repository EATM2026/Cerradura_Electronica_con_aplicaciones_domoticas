# Smart-Convenient-Lock
Repository for the assignment on embedded systems

## 1. Introduction

Electronics are ubiquitous in the modern world; it is genuinely difficult to find a daily task or scenario that does not currently rely on an electronic solution. Consider, for instance, residential complexes where entry is managed by an electric intercom system of varying quality, or hospitals and offices where medical staff areas are restricted by facial recognition access control. Electronic access devices are prevalent across numerous sectors, employing a wide variety of sensors and actuators. However, the most prominent characteristic of these high-end systems is often the precise reason average consumers avoid them: they are prohibitively expensive.

When considering an electronic lock for a small residential complex, property owners must navigate several challenges. They must evaluate the most convenient type of access for residents, purchase additional specific hardware (such as dedicated transformers and control panels), and hire trained personnel for the installation. Costs scale rapidly with the sophistication of the lock. Consequently, clients often default to conventional locks or basic electric strikes that operate solely with physical keys or a simple intercom signal. This compromise fails to resolve the issue of key management among users and leaves property owners with no way to identify who enters the premises.

The proposed solution consists of an embedded device that connects to these basic electric strikes. The system is encapsulated within a compact panel housing the access interfaces, which may include a facial recognition camera, a fingerprint reader, a keypad, or a combination of all three. Powered by the lock's existing electrical supply, the device operates intelligently by connecting to a proprietary server, enabling users to access the camera stream or control the lock remotely.

Additionally, the device features a continuity loop signal linked to its enclosure. If tampering is detected, this loop triggers an emergency lockdown, instantly alerting users and denying access to unauthorized individuals.

Given this context, the **objectives** of this project are as follows:
* **Evaluate** the convenience of installing a single, all-in-one smart device compared to the invasive and costly installation of standard electronic locks currently on the market.
* **Verify** the effectiveness of the proprietary server's commands and functions used to control the lock and transmit real-time alerts to users.

---

## 2. Scope and Limitations

The device is housed within a compact, reinforced plastic enclosure containing all necessary control circuitry—specifically, an embedded system capable of internet connectivity to host its own server and receive instructions from authorized users. 

For this specific project prototype, the system architecture includes:
* **Microcontrollers:** An **ESP32** manages the primary functions and server data, while an **ATmega328P** chip, communicating directly with the ESP32, handles the actuators.
* **Inputs/Sensors:** A fingerprint reader, a camera for recognition, an access keypad, and a temperature sensor.
* **Outputs/Actuators:** A small LCD screen for user feedback/administrator configuration, and an auxiliary actuator to control lights or air conditioning in rooms or offices.

### Limitations & Technical Considerations

1. **Security Vulnerability (All-in-One Design):** Although the device includes a continuity-sensing protective loop to keep the door locked during a tampering attempt, the entire apparatus is encapsulated within the external panel. This makes it susceptible to theft or physical damage during a breach. A straightforward solution would be to expose only the essential communication interfaces externally while housing the rest of the circuitry in an internal secure box. *Since this is a demonstrative project, this solution is noted but will not be implemented.*
2. **Emergency Operation:** While the device includes a temperature sensor, it lacks a smoke detector to automatically unlock the door and sound an alarm in the event of a fire. 
3. **Emergency Lockout vs. Natural Disasters:** During a tampering attempt, the door remains locked until an administrator arrives. However, if an earthquake were to occur simultaneously, there is no hardware mechanism to release the door. A viable workaround would be implementing a remote unlock command via the server, triggered by internet-based earthquake alerts. *Again, this feature falls outside the scope of this project proposal and will not be fully implemented, though it could be provisionally enabled to test its functionality due to its simple deployment.*

### Initial code for MAIN controller - ESP32

```cpp
#include <Arduino.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>

const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  {'1','2','3','A'}, {'4','5','6','B'}, {'7','8','9','C'}, {'*','0','#','D'}
};
byte pinesFilas[FILAS] = {21, 22, 19, 23};
byte pinesColumnas[COLUMNAS] = {26, 27, 14, 12};
Keypad keypad = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);
String claveIngresada = "";
String claveValida = "1234"; 
HardwareSerial serialHuella(1); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialHuella);
unsigned long tiempoUltimaLectura = 0;
void setup() {
  Serial.begin(115200);  
  Serial2.begin(9600);   
  serialHuella.begin(57600, SERIAL_8N1, 16, 4); 
  delay(500);
  if (finger.verifyPassword()) {
    Serial.println("¡Sensor de huellas detectado en la ESP32!");
  } else {
    Serial.println("No se encontró el sensor de huellas.");
  }
  finger.getTemplateCount();
}

void loop() {
 
  char tecla = keypad.getKey();
  if (tecla) {
    if (tecla == '#') {
      if (claveIngresada == claveValida) {
        Serial2.print("<OK>"); 
      } else {
        Serial2.print("<ERROR>"); 
      }
      claveIngresada = "";
    } 
    else if (tecla == '*') {
      if (claveIngresada.length() > 0) {
        claveIngresada.remove(claveIngresada.length() - 1);
        Serial2.print("<B>"); 
      }
    } 
    else {
      if (claveIngresada.length() < 8) {
        claveIngresada += tecla;
        Serial2.print("<*>"); 
      }
    }
  }

  if (millis() - tiempoUltimaLectura > 100) {
    tiempoUltimaLectura = millis();
    
    uint8_t p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      p = finger.image2Tz();
      if (p == FINGERPRINT_OK) {
        p = finger.fingerFastSearch();
        if (p == FINGERPRINT_OK) {
          Serial.println("Huella válida encontrada.");
          Serial2.print("<USER_OK>"); 
          delay(1000); 
        } else if (p == FINGERPRINT_NOTFOUND) {
          Serial.println("Huella no registrada.");
          Serial2.print("<USER_UNKNOWN>"); 
          delay(1000);
        }
      }
    }
  }
}
```
### Initial code for ATMEGA
```cpp
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// Configuración de pines paralelos del LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Escucha a la ESP32 en el Pin 11 (RX)
SoftwareSerial espSerial(11, 12); 

int longitudAsteriscos = 0;

void setup() {
  Serial.begin(115200);  
  espSerial.begin(9600); 
  lcd.begin(16, 2);
  mostrarPantallaInicial();
}

void loop() {
  if (espSerial.available() > 0) {
    char c = espSerial.read();
    
    if (c == '<') {
      String comando = espSerial.readStringUntil('>');
      
      if (comando == "OK") {
        mostrarMensajeLCD("Acceso", "Autorizado");
      } 
      else if (comando == "ERROR") {
        mostrarMensajeLCD("Clave", "Incorrecta");
      } 
      else if (comando == "USER_OK") {
        mostrarMensajeLCD("Usuario", "Aceptado");
      } 
      else if (comando == "USER_UNKNOWN") {
        mostrarMensajeLCD("Usuario no", "registrado");
      } 
      else if (comando == "*") {
        if (longitudAsteriscos < 8) {
          lcd.setCursor(longitudAsteriscos, 1);
          lcd.print("*");
          longitudAsteriscos++;
        }
      } 
      else if (comando == "B") {
        if (longitudAsteriscos > 0) {
          longitudAsteriscos--;
          lcd.setCursor(longitudAsteriscos, 1);
          lcd.print(" ");
        }
      }
    }
  }
}

void mostrarPantallaInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ingrese clave:");
  lcd.setCursor(0, 1);
}

void mostrarMensajeLCD(String linea1, String linea2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
  delay(3000); 
  longitudAsteriscos = 0;
  mostrarPantallaInicial();
}
```
### Considerations
Note that the project only uses de ATMEGA chip and not the Arduino Uno Module, this is for keeping the final product as compact as it can.
The ESP32 is the master, the one who gives the orders to the Atmega and the one who comunicates with the users.
The ATmega is mostly used for controlling actuators. 
