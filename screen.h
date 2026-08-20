#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayOLED {
  private:
    // Instancia privada de la pantalla
    Adafruit_SSD1306 display;

  public:
    // Constructor
    DisplayOLED();
    
    // Función para el setup()
    bool inicializar();
    
    void dormir();
    void despertar();
    
    // Se añade un parámetro opcional String para la unidad
    void mostrarDato(String texto, float valor, String unidad = "");
    void mostrarTexto(String linea1, String linea2);
    void mostrarAcceso(String nombreUsuario);
};
#endif