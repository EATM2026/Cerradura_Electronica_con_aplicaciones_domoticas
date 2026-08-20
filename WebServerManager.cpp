#include "WebServerManager.h"
#include <HTTPClient.h> // Necesario para hablar con la ESP32-CAM

WebServerManager::WebServerManager(const char* ap_ssid, const char* ap_password, UserManager* um, FingerprintManager* fm) 
    : server(80), ssid(ap_ssid), password(ap_password), userManager(um), fingerprintManager(fm), onUnlockRequested(nullptr), onFaceUnlockRequested(nullptr) {}

void WebServerManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Error montando LittleFS");
        return;
    }

    Serial.println("Iniciando Punto de Acceso...");
    WiFi.softAP(ssid, password);
    setupRoutes();
    server.begin();
}

void WebServerManager::setUnlockCallback(UnlockCallback callback) {
    onUnlockRequested = callback;
}

void WebServerManager::setFaceUnlockCallback(FaceUnlockCallback callback) {
    onFaceUnlockRequested = callback;
}

void WebServerManager::activarModoRegistroRostro() {
    habilitarBotonRostro = true;
}

void WebServerManager::setupRoutes() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.on("/login", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("pin", true)) {
            String inputPin = request->getParam("pin", true)->value();
            if (inputPin == this->adminPin) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(401, "text/plain", "No autorizado");
            }
        } else {
            request->send(400, "text/plain", "Falta el PIN");
        }
    });

    server.on("/unlock", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (this->onUnlockRequested != nullptr) {
            this->onUnlockRequested();
            request->send(200, "text/plain", "Puerta Abierta");
        } else {
            request->send(500, "text/plain", "Error de hardware");
        }
    });

    // --- RUTAS NUEVAS PARA RECONOCIMIENTO FACIAL ---

    // 1. La página web pregunta si debe mostrar el botón
    server.on("/status_rostro", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (this->habilitarBotonRostro) {
            request->send(200, "text/plain", "ACTIVO");
            this->habilitarBotonRostro = false; // Se desactiva tras leerlo para que el botón no quede pegado
        } else {
            request->send(200, "text/plain", "INACTIVO");
        }
    });

    // 2. La ESP32-CAM llama a esta ruta cuando reconoce un rostro
    server.on("/unlock_face", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id")) {
            int idRostro = request->getParam("id")->value().toInt();
            if (this->onFaceUnlockRequested != nullptr) {
                this->onFaceUnlockRequested(idRostro);
                request->send(200, "text/plain", "Acceso Facial Concedido");
            }
        } else {
            request->send(400, "text/plain", "Falta ID");
        }
    });

    // --- RUTAS DE GESTIÓN DE USUARIOS ACTUALIZADAS ---

    server.on("/users", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (this->userManager != nullptr) {
            String jsonResponse = this->userManager->getUsersJson();
            request->send(200, "application/json", jsonResponse);
        } else {
            request->send(500, "text/plain", "Error interno");
        }
    });

    server.on("/users", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id", true) && request->hasParam("name", true) && request->hasParam("pin", true)) {
            int id = request->getParam("id", true)->value().toInt();
            String name = request->getParam("name", true)->value();
            String pin = request->getParam("pin", true)->value();
            
            if (this->userManager->addUser(id, name, pin)) {
                request->send(200, "text/plain", "Usuario guardado en JSON");
            } else {
                request->send(500, "text/plain", "Error al guardar en JSON");
            }
        } else {
            request->send(400, "text/plain", "Faltan parametros");
        }
    });

    server.on("/users", HTTP_DELETE, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id")) {
            int id = request->getParam("id")->value().toInt();
            
            bool jsonDeleted = this->userManager->deleteUser(id);
            bool hwDeleted = this->fingerprintManager->borrarHuella(id);
            
            // Enviamos petición HTTP a la cámara para borrar el rostro también
            HTTPClient http;
            String urlCamara = "http://192.168.4.10:81/delete_face?id=" + String(id);
            http.begin(urlCamara);
            int httpCode = http.GET();
            http.end();
            
            if (jsonDeleted) {
                request->send(200, "text/plain", "Usuario, huella y rostro eliminados");
            } else {
                request->send(500, "text/plain", "Error al eliminar");
            }
        } else {
            request->send(400, "text/plain", "Falta ID");
        }
    });
}