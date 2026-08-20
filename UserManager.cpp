#include "UserManager.h"

UserManager::UserManager() {}

bool UserManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Error: Fallo al montar LittleFS");
        return false;
    }
    
    if (!LittleFS.exists(filePath)) {
        File file = LittleFS.open(filePath, "w");
        file.print("[]"); 
        file.close();
    }
    return true;
}

String UserManager::getUsersJson() {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        return "[]";
    }
    String json = file.readString();
    file.close();
    return json;
}

bool UserManager::addUser(int id, String nombre, String clave) {
    File file = LittleFS.open(filePath, "r");
    JsonDocument doc; 
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Error al leer el JSON");
        return false;
    }

    JsonArray array = doc.as<JsonArray>();
    bool encontrado = false;
    for (JsonObject user : array) {
        if (user["id"] == id) {
            // CORREGIDO: Usamos "name" y "pin" consistentemente
            user["name"] = nombre;
            user["pin"] = clave;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        JsonObject newUser = array.add<JsonObject>();
        newUser["id"] = id;
        newUser["name"] = nombre; // CORREGIDO
        newUser["pin"] = clave;   // CORREGIDO
    }

    file = LittleFS.open(filePath, "w");
    serializeJson(doc, file);
    file.close();
    return true;
}

bool UserManager::deleteUser(int id) {
    File file = LittleFS.open(filePath, "r");
    JsonDocument doc;
    deserializeJson(doc, file);
    file.close();

    JsonArray array = doc.as<JsonArray>();
    for (int i = 0; i < array.size(); i++) {
        if (array[i]["id"] == id) {
            array.remove(i);
            break;
        }
    }

    file = LittleFS.open(filePath, "w");
    serializeJson(doc, file);
    file.close();
    return true;
}

String UserManager::obtenerNombrePorID(int idBuscado) {
    File file = LittleFS.open("/users.json", "r");
    if (!file) return "Usuario " + String(idBuscado); 

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) return "Usuario " + String(idBuscado);

    JsonArray users = doc.as<JsonArray>();
    for (JsonObject user : users) {
        if (user["id"] == idBuscado || String(user["id"].as<const char*>()) == String(idBuscado)) {
            return String(user["name"].as<const char*>());
        }
    }
    
    return "Desconocido"; 
}

String UserManager::obtenerNombrePorClave(String claveBuscada) {
    File file = LittleFS.open("/users.json", "r");
    if (!file) return ""; 

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) return "";

    JsonArray users = doc.as<JsonArray>();
    for (JsonObject user : users) {
        if (String(user["pin"].as<const char*>()) == claveBuscada) {
            return String(user["name"].as<const char*>());
        }
    }
    
    return ""; 
}