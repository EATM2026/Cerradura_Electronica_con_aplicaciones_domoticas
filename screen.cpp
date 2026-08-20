#include "screen.h"

// Constructor: Configura la pantalla de 128x64 usando el bus I2C de hardware
DisplayOLED::DisplayOLED() : display(128, 64, &Wire, -1) {
}

bool DisplayOLED::inicializar() {
    // Forzar el uso de los pines específicos para SDA (21) y SCL (22) en el ESP32
    Wire.begin(21, 22);

    // Inicializar la pantalla con la dirección I2C 0x3C
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Error: No se encuentra la pantalla OLED");
        return false;
    }

    // Limpiar pantalla y configurar color por defecto
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();
    
    return true;
}

void DisplayOLED::mostrarDato(String texto, float valor, String unidad) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(texto);
    
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print(valor, 2);
    
    // Si se envía una unidad, la imprime junto con el símbolo °
    if (unidad != "") {
        display.print((char)247); // 247 es el código ASCII del símbolo '°' en esta librería
        display.print(unidad);
    }
    
    display.display();
}
// ... código anterior ...
void DisplayOLED::mostrarTexto(String linea1, String linea2) {
    display.clearDisplay();
    
    // 1. Mostrar la primera línea (título)
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(linea1);
    
    // 2. Mostrar la segunda línea (asteriscos o subtítulo)
    display.setTextSize(2);  
    display.setCursor(0, 20);
    display.println(linea2); 
    
    display.display();
}
void DisplayOLED::dormir() {
    // Envía el comando I2C para apagar el panel
    display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void DisplayOLED::despertar() {
    // Envía el comando I2C para encender el panel
    display.ssd1306_command(SSD1306_DISPLAYON);
}
void DisplayOLED::mostrarAcceso(String nombreUsuario) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("Acceso Concedido");
    
    display.setCursor(0, 16);
    display.setTextSize(2); // Letra más grande para el nombre
    display.println(nombreUsuario);
    
    display.display();
}