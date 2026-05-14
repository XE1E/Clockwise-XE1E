#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <CWPreferences.h>
#include "StatusController.h"
#include "WebUI.h"

#ifndef CLOCKFACE_NAME
  #define CLOCKFACE_NAME "UNKNOWN"
#endif

struct ClockwiseWebServer
{
  AsyncWebServer server;
  bool force_restart = false;
  bool needs_reload = false;

  ClockwiseWebServer() : server(80) {}

  static ClockwiseWebServer *getInstance()
  {
    static ClockwiseWebServer base;
    return &base;
  }

  void startWebServer()
  {
    setupRoutes();
    server.begin();
    Serial.println("[Web] Async server started on port 80");
  }

  void stopWebServer()
  {
    server.end();
  }

  void setupRoutes()
  {
    // Página principal
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", WEB_UI_HTML);
      request->send(response);
    });

    // API: obtener configuración
    server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
      String json = "{";
      // WiFi
      json += "\"wifiSsid\":\"" + ClockwiseParams::getInstance()->wifiSsid + "\",";
      json += "\"wifiSsid2\":\"" + ClockwiseParams::getInstance()->wifiSsid2 + "\",";
      json += "\"wifiSsid3\":\"" + ClockwiseParams::getInstance()->wifiSsid3 + "\",";
      json += "\"wifiConnected\":\"" + WiFi.SSID() + "\",";
      json += "\"wifiRssi\":" + String(WiFi.RSSI()) + ",";
      // Display
      json += "\"displayBright\":" + String(ClockwiseParams::getInstance()->displayBright) + ",";
      json += "\"displayRotation\":" + String(ClockwiseParams::getInstance()->displayRotation) + ",";
      json += "\"swapBlueGreen\":" + String(ClockwiseParams::getInstance()->swapBlueGreen ? 1 : 0) + ",";
      json += "\"autoBrightMin\":" + String(ClockwiseParams::getInstance()->autoBrightMin) + ",";
      json += "\"autoBrightMax\":" + String(ClockwiseParams::getInstance()->autoBrightMax) + ",";
      // Time
      json += "\"timeZone\":\"" + ClockwiseParams::getInstance()->timeZone + "\",";
      json += "\"ntpServer\":\"" + ClockwiseParams::getInstance()->ntpServer + "\",";
      json += "\"use24hFormat\":" + String(ClockwiseParams::getInstance()->use24hFormat ? 1 : 0) + ",";
      json += "\"useSpanish\":" + String(ClockwiseParams::getInstance()->useSpanish ? 1 : 0) + ",";
      // Night mode
      json += "\"nightEnabled\":" + String(ClockwiseParams::getInstance()->nightModeEnabled ? 1 : 0) + ",";
      json += "\"nightStart\":\"" + ClockwiseParams::getInstance()->nightModeStart + "\",";
      json += "\"nightEnd\":\"" + ClockwiseParams::getInstance()->nightModeEnd + "\",";
      json += "\"nightBright\":" + String(ClockwiseParams::getInstance()->nightBrightness) + ",";
      json += "\"nightClock\":\"" + ClockwiseParams::getInstance()->nightClockface + "\",";
      // Clockface
      json += "\"canvasServer\":\"" + ClockwiseParams::getInstance()->canvasServer + "\",";
      json += "\"canvasFile\":\"" + ClockwiseParams::getInstance()->canvasFile + "\",";
      json += "\"rotationEnabled\":" + String(ClockwiseParams::getInstance()->rotationEnabled ? 1 : 0) + ",";
      json += "\"rotationInterval\":" + String(ClockwiseParams::getInstance()->rotationInterval) + ",";
      json += "\"rotationList\":\"" + ClockwiseParams::getInstance()->rotationList + "\",";
      // System
      json += "\"version\":\"" CW_FW_VERSION "\",";
      json += "\"name\":\"" CW_FW_NAME "\"";
      json += "}";
      request->send(200, "application/json", json);
    });

    // API: establecer parámetro
    server.on("/api/set", HTTP_POST, [](AsyncWebServerRequest *request) {
      if (request->params() > 0) {
        const AsyncWebParameter* p = request->getParam((size_t)0);
        String key = p->name();
        String value = p->value();

        // WiFi
        if (key == "wifiSsid") ClockwiseParams::getInstance()->wifiSsid = value;
        else if (key == "wifiPwd") ClockwiseParams::getInstance()->wifiPwd = value;
        else if (key == "wifiSsid2") ClockwiseParams::getInstance()->wifiSsid2 = value;
        else if (key == "wifiPwd2") ClockwiseParams::getInstance()->wifiPwd2 = value;
        else if (key == "wifiSsid3") ClockwiseParams::getInstance()->wifiSsid3 = value;
        else if (key == "wifiPwd3") ClockwiseParams::getInstance()->wifiPwd3 = value;
        // Display
        else if (key == "displayBright") ClockwiseParams::getInstance()->displayBright = value.toInt();
        else if (key == "displayRotation") ClockwiseParams::getInstance()->displayRotation = value.toInt();
        else if (key == "swapBlueGreen") ClockwiseParams::getInstance()->swapBlueGreen = (value == "1");
        else if (key == "autoBrightMin") ClockwiseParams::getInstance()->autoBrightMin = value.toInt();
        else if (key == "autoBrightMax") ClockwiseParams::getInstance()->autoBrightMax = value.toInt();
        // Time
        else if (key == "timeZone") ClockwiseParams::getInstance()->timeZone = value;
        else if (key == "ntpServer") ClockwiseParams::getInstance()->ntpServer = value;
        else if (key == "use24hFormat") ClockwiseParams::getInstance()->use24hFormat = (value == "1");
        else if (key == "useSpanish") ClockwiseParams::getInstance()->useSpanish = (value == "1");
        // Night mode
        else if (key == "nightEnabled") ClockwiseParams::getInstance()->nightModeEnabled = (value == "1");
        else if (key == "nightStart") ClockwiseParams::getInstance()->nightModeStart = value;
        else if (key == "nightEnd") ClockwiseParams::getInstance()->nightModeEnd = value;
        else if (key == "nightBright") ClockwiseParams::getInstance()->nightBrightness = value.toInt();
        else if (key == "nightClock") ClockwiseParams::getInstance()->nightClockface = value;
        // Clockface
        else if (key == "canvasServer") ClockwiseParams::getInstance()->canvasServer = value;
        else if (key == "canvasFile") ClockwiseParams::getInstance()->canvasFile = value;
        else if (key == "rotationEnabled") ClockwiseParams::getInstance()->rotationEnabled = (value == "1");
        else if (key == "rotationInterval") ClockwiseParams::getInstance()->rotationInterval = value.toInt();
        else if (key == "rotationList") ClockwiseParams::getInstance()->rotationList = value;

        ClockwiseParams::getInstance()->save();
      }
      request->send(204);
    });

    // API: reiniciar
    server.on("/api/restart", HTTP_POST, [this](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Restarting...");
      force_restart = true;
    });

    // API: recargar caratula sin reiniciar
    server.on("/api/reload", HTTP_POST, [this](AsyncWebServerRequest *request) {
      request->send(204);
      needs_reload = true;
    });

    // API: reset de fábrica
    server.on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
      ClockwiseParams::getInstance()->preferences.clear();
      request->send(200, "text/plain", "Reset complete, restarting...");
      force_restart = true;
    });

    // API: leer pin (para calibración LDR)
    server.on("/api/pin", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (request->hasParam("pin")) {
        int pin = request->getParam("pin")->value().toInt();
        int value = analogRead(pin);
        request->send(200, "application/json", "{\"value\":" + String(value) + "}");
      } else {
        request->send(400);
      }
    });
  }

  void handleRestart()
  {
    if (force_restart) {
      delay(500);
      ESP.restart();
    }
  }
};
