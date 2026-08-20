#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h> 
#include "UserManager.h" 
#include "FingerprintManager.h" 

class WebServerManager {
private:
    AsyncWebServer server;
    const char* ssid;
    const char* password;
    const String adminPin = "0403";
    
    UserManager* userManager; 
    FingerprintManager* fingerprintManager; 

    typedef void (*UnlockCallback)();
    UnlockCallback onUnlockRequested;
    
    // Función específica para el relé por rostro (700ms)
    typedef void (*FaceUnlockCallback)(int idRostro);
    FaceUnlockCallback onFaceUnlockRequested;

    bool habilitarBotonRostro = false;

    void setupRoutes();

public:
    WebServerManager(const char* ap_ssid, const char* ap_password, UserManager* um, FingerprintManager* fm);
    
    void begin();
    void setUnlockCallback(UnlockCallback callback);
    void setFaceUnlockCallback(FaceUnlockCallback callback);
    
    void activarModoRegistroRostro();
};

#endif