#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class UserManager {
private:
    const char* filePath = "/users.json";

public:
    UserManager();
    
    // Monta el sistema de archivos
    bool begin();
    
    // Lee el archivo y lo devuelve como un String de formato JSON
    String getUsersJson();
    
    // Agrega o actualiza un usuario y guarda el archivo
    bool addUser(int id, String nombre, String clave);
    
    // Elimina un usuario por su ID
    bool deleteUser(int id);
    
    String obtenerNombrePorID(int idBuscado);

    String obtenerNombrePorClave(String claveBuscada);
};

#endif