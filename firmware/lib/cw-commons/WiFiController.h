#pragma once

// Increase WiFi connection timeout for weak signals
#define MAX_ATTEMPTS_WIFI_CONNECTION 40

#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "ImprovWiFiLibrary.h"
#include "CWWebServer.h"
#include "StatusController.h"

ImprovWiFi improvSerial(&Serial);

// Estructura para redes WiFi ordenables por señal
struct WiFiNetwork {
  String ssid;
  String password;
  int32_t rssi;
  bool available;
};

struct WiFiController
{
  long elapsedTimeOffline = 0;
  long lastReconnectAttempt = 0;
  bool connectionSucessfulOnce = false;
  bool apModeActive = false;
  bool reconnecting = false;
  WiFiNetwork networks[3];
  DNSServer dnsServer;

  static const unsigned long RECONNECT_INTERVAL = 30000;  // 30 segundos entre intentos
  static const unsigned long RESTART_TIMEOUT = 300000;    // 5 minutos sin conexión = restart

  static void onImprovWiFiErrorCb(ImprovTypes::Error err)
  {
    ClockwiseWebServer::getInstance()->stopWebServer();
    StatusController::getInstance()->blink_led(2000, 3);
  }

  static void onImprovWiFiConnectedCb(const char *ssid, const char *password)
  {
    // Guardar nueva red en slot 1 (principal)
    ClockwiseParams::getInstance()->wifiSsid = String(ssid);
    ClockwiseParams::getInstance()->wifiPwd = String(password);
    ClockwiseParams::getInstance()->save();

    ClockwiseWebServer::getInstance()->startWebServer();

    if (MDNS.begin("clockwise-xe1e"))
    {
      MDNS.addService("http", "tcp", 80);
      Serial.println("[WiFi] mDNS: http://clockwise-xe1e.local");
    }
  }

  bool isConnected()
  {
    if (apModeActive) {
      return true;
    }

    if (WiFi.status() == WL_CONNECTED) {
      if (reconnecting) {
        reconnecting = false;
        Serial.println("[WiFi] Reconnected!");
      }
      elapsedTimeOffline = 0;
      return true;
    }

    // Desconectado
    if (elapsedTimeOffline == 0) {
      elapsedTimeOffline = millis();
      Serial.println("[WiFi] Connection lost");
    }

    // Si nunca conectó y lleva 5 min offline, reiniciar
    if (!connectionSucessfulOnce && (millis() - elapsedTimeOffline) > RESTART_TIMEOUT) {
      StatusController::getInstance()->forceRestart();
    }

    return false;
  }

  void checkReconnect()
  {
    if (apModeActive || WiFi.status() == WL_CONNECTED) {
      if (reconnecting) {
        reconnecting = false;
        StatusController::getInstance()->clearReconnecting();
      }
      return;
    }
    if (!connectionSucessfulOnce) return;

    unsigned long now = millis();
    if (now - lastReconnectAttempt < RECONNECT_INTERVAL) return;

    lastReconnectAttempt = now;
    reconnecting = true;
    Serial.println("[WiFi] Attempting reconnect...");
    StatusController::getInstance()->wifiReconnecting();

    // Intento rápido: reconectar a la misma red
    WiFi.reconnect();
    delay(3000);

    if (WiFi.status() == WL_CONNECTED) {
      reconnecting = false;
      elapsedTimeOffline = 0;
      StatusController::getInstance()->clearReconnecting();
      Serial.printf("[WiFi] Reconnected to %s\n", WiFi.SSID().c_str());
      return;
    }

    // Si falla, intentar mejor red disponible
    Serial.println("[WiFi] Quick reconnect failed, scanning...");
    if (connectToBestNetwork()) {
      reconnecting = false;
      elapsedTimeOffline = 0;
      StatusController::getInstance()->clearReconnecting();
      ClockwiseWebServer::getInstance()->startWebServer();
    }
  }

  void handleImprovWiFi()
  {
    improvSerial.handleSerial();
    if (apModeActive) {
      dnsServer.processNextRequest();
    }
  }

  // Cargar redes guardadas desde preferencias
  void loadSavedNetworks()
  {
    networks[0].ssid = ClockwiseParams::getInstance()->wifiSsid;
    networks[0].password = ClockwiseParams::getInstance()->wifiPwd;
    networks[0].rssi = -100;
    networks[0].available = false;

    networks[1].ssid = ClockwiseParams::getInstance()->wifiSsid2;
    networks[1].password = ClockwiseParams::getInstance()->wifiPwd2;
    networks[1].rssi = -100;
    networks[1].available = false;

    networks[2].ssid = ClockwiseParams::getInstance()->wifiSsid3;
    networks[2].password = ClockwiseParams::getInstance()->wifiPwd3;
    networks[2].rssi = -100;
    networks[2].available = false;
  }

  // Escanear redes y actualizar RSSI de las guardadas
  void scanAndUpdateNetworks()
  {
    StatusController::getInstance()->wifiScanning();
    Serial.println("[WiFi] Scanning networks...");
    int n = WiFi.scanNetworks();
    Serial.printf("[WiFi] Found %d networks\n", n);

    for (int i = 0; i < n; i++) {
      String scannedSSID = WiFi.SSID(i);
      int32_t scannedRSSI = WiFi.RSSI(i);

      for (int j = 0; j < 3; j++) {
        if (!networks[j].ssid.isEmpty() && networks[j].ssid == scannedSSID) {
          networks[j].rssi = scannedRSSI;
          networks[j].available = true;
          Serial.printf("[WiFi] Found saved network: %s (RSSI: %d dBm)\n",
                        scannedSSID.c_str(), scannedRSSI);
        }
      }
    }
    WiFi.scanDelete();
  }

  // Ordenar redes por RSSI (mejor señal primero)
  void sortNetworksByRSSI()
  {
    for (int i = 0; i < 2; i++) {
      for (int j = i + 1; j < 3; j++) {
        if (networks[j].available &&
            (!networks[i].available || networks[j].rssi > networks[i].rssi)) {
          WiFiNetwork temp = networks[i];
          networks[i] = networks[j];
          networks[j] = temp;
        }
      }
    }
  }

  // Intentar conectar a una red específica
  bool tryConnect(const char* ssid, const char* password)
  {
    Serial.printf("[WiFi] Trying to connect to: %s\n", ssid);

    for (int retry = 0; retry < 2; retry++) {
      if (retry > 0) {
        Serial.printf("[WiFi] Retry %d...\n", retry + 1);
        delay(500);
      }

      if (improvSerial.tryConnectToWifi(ssid, password)) {
        return true;
      }
    }
    return false;
  }

  // Conectar a la mejor red disponible
  bool connectToBestNetwork()
  {
    loadSavedNetworks();

    // Verificar si hay al menos una red configurada
    bool hasNetworks = false;
    for (int i = 0; i < 3; i++) {
      if (!networks[i].ssid.isEmpty()) {
        hasNetworks = true;
        break;
      }
    }

    if (!hasNetworks) {
      Serial.println("[WiFi] No saved networks found");
      return false;
    }

    scanAndUpdateNetworks();
    sortNetworksByRSSI();

    // Intentar conectar a redes disponibles (ordenadas por señal)
    for (int i = 0; i < 3; i++) {
      if (networks[i].available && !networks[i].ssid.isEmpty()) {
        if (tryConnect(networks[i].ssid.c_str(), networks[i].password.c_str())) {
          Serial.printf("[WiFi] Connected to %s (RSSI: %d dBm)\n",
                        networks[i].ssid.c_str(), networks[i].rssi);
          return true;
        }
        Serial.printf("[WiFi] Failed to connect to %s\n", networks[i].ssid.c_str());
      }
    }

    // Si ninguna disponible funcionó, intentar las no detectadas
    Serial.println("[WiFi] Trying networks not found in scan...");
    for (int i = 0; i < 3; i++) {
      if (!networks[i].available && !networks[i].ssid.isEmpty()) {
        if (tryConnect(networks[i].ssid.c_str(), networks[i].password.c_str())) {
          Serial.printf("[WiFi] Connected to %s\n", networks[i].ssid.c_str());
          return true;
        }
      }
    }

    return false;
  }

  // Modo AP propio (sin WiFiManager)
  bool startAPMode()
  {
    Serial.println("[WiFi] Starting AP mode...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ClockWise-XE1E", "");  // Sin contraseña

    delay(100);
    IPAddress apIP = WiFi.softAPIP();
    Serial.printf("[WiFi] AP IP: %s\n", apIP.toString().c_str());

    // DNS captive portal - redirige todo a nuestra IP
    dnsServer.start(53, "*", apIP);

    // Iniciar servidor web para configuración
    ClockwiseWebServer::getInstance()->startWebServer();

    apModeActive = true;

    // Mostrar modo AP en pantalla
    StatusController::getInstance()->showAPMode(apIP.toString().c_str());

    return true;
  }

  void onConnected()
  {
    connectionSucessfulOnce = true;
    ClockwiseWebServer::getInstance()->startWebServer();

    if (MDNS.begin("clockwise-xe1e"))
    {
      MDNS.addService("http", "tcp", 80);
      Serial.println("[WiFi] mDNS: http://clockwise-xe1e.local");
    }

    int rssi = WiFi.RSSI();
    Serial.printf("[WiFi] IP: %s, RSSI: %d dBm\n",
                  WiFi.localIP().toString().c_str(), rssi);

    // Show IP and signal on display for 3 seconds
    StatusController::getInstance()->showIPAddress(
      WiFi.localIP().toString().c_str(), rssi);
    delay(3000);
  }

  bool begin()
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    improvSerial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32,
                                CW_FW_NAME, CW_FW_VERSION, "ClockWise-XE1E");
    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);

    ClockwiseParams::getInstance()->load();

    // Intentar conectar a la mejor red disponible
    if (connectToBestNetwork())
    {
      onConnected();
      return true;
    }

    // No se pudo conectar - iniciar modo AP propio
    StatusController::getInstance()->wifiConnectionFailed("Config: 192.168.4.1");
    delay(2000);
    startAPMode();
    return false;
  }
};
