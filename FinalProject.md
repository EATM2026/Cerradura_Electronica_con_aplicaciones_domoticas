# Resumen: Cerradura Electrónica con Aplicaciones Domóticas

Desarrollado por Felix Javier Garate Rivera y Emilio Alejandro Tomala Mena.

## 1. Introducción y Justificación
El proyecto surge de la necesidad de mejorar el control de ingreso a espacios delicados (como laboratorios u oficinas), reemplazando las cerraduras convencionales por un sistema electrónico más seguro. Esto evita problemas asociados con las llaves físicas, como la pérdida, duplicación o uso no autorizado, permitiendo identificar eficientemente a las personas autorizadas.

## 2. Objetivos del Proyecto
* **Objetivo General:** Desarrollar e implementar una cerradura inteligente basada en un sistema embebido que gestione el acceso mediante contraseña y reconocimiento de huella dactilar.
* **Objetivos Específicos:**
  * Integrar componentes electrónicos (ESP32, Arduino UNO, teclado, lector de huellas y pantalla).
  * Implementar mecanismos de autenticación y establecer la comunicación entre microcontroladores.
  * Desarrollar una interfaz de usuario para el estado del sistema.
  * Validar el sistema mediante pruebas funcionales.

## 3. Especificaciones y Hardware
El prototipo físico y la arquitectura (que pasó de un protoboard a una PCB dedicada) integran los siguientes componentes principales:
* **Microcontroladores:** Placa ESP32 (controlador principal con Wi-Fi/Bluetooth) y Arduino UNO R3 (controlador auxiliar).
* **Mecanismos de Ingreso:** Teclado matricial 4x4 y Lector de huellas JM101B.
* **Sensores:** Sensor de proximidad (para activación) y Sensor DHT11 (temperatura y humedad).
* **Interfaz y Actuadores:** Pantalla OLED de 0.96", módulo relé de 5V (2 canales) y cerradura eléctrica izquierda.

**Costo total referencial:** $120.00 USD.

## 4. Especificaciones Técnicas
* **Alimentación y Autonomía:** Utiliza dos celdas de litio de 12000 mAh en serie con regulador a 5V. El consumo típico es de 160 mA (0.8 W), lo que otorga una autonomía aproximada de al menos 90 horas.
* **Gestión de Energía:** El sensor de proximidad apaga la pantalla cuando no hay usuarios cerca.
* **Memoria:** El lector JM101B almacena las huellas localmente, reduciendo la carga en la ESP32 (que consume ~1.0 MB de flash y 320 KB de RAM para el resto de funciones).
* **Desarrollo de PCB y Software:** PCB diseñada en software como Proteus, Flatcam y GBRL Candle (manufactura propia). Programación realizada en Visual Studio Code con PlatformIO.

## 5. Conclusiones y Recomendaciones
### Conclusiones
* Se logró construir un sistema funcional capaz de distinguir entre credenciales válidas e inválidas mediante huella y contraseña.
* El sistema es automatizado y proporciona datos adicionales del entorno gracias al sensor de temperatura/humedad.

### Recomendaciones
* Revisar los diagramas y voltajes antes del ensamblaje para evitar daños (datasheets).
* Garantizar una fuente de alimentación robusta, sobre todo para el actuador eléctrico.
* Probar cada módulo (keypad, pantalla, lector, etc.) de forma individual antes de la integración total.
