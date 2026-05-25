#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <CWPreferences.h>
#include <CWPreview.h>
#include "StatusController.h"
#include "WebUI_gz.h"
#include "WebUI_minimal.h"

#ifndef CLOCKFACE_NAME
  #define CLOCKFACE_NAME "UNKNOWN"
#endif

struct ClockwiseWebServer
{
  AsyncWebServer server;
  bool force_restart = false;
  bool needs_reload = false;
  bool needs_brightness_update = false;
  uint8_t pending_brightness = 0;
  bool needs_night_brightness_update = false;
  uint8_t pending_night_brightness = 0;
  bool needs_display_rotation_update = false;
  uint8_t pending_display_rotation = 0;
  bool rotation_changed = false;
  bool serverStarted = false;

  ClockwiseWebServer() : server(80) {}

  static ClockwiseWebServer *getInstance()
  {
    static ClockwiseWebServer base;
    return &base;
  }

  void startWebServer()
  {
    if (serverStarted) {
      Serial.println("[Web] Server already running");
      return;
    }
    Serial.printf("[Web] Starting server, free heap: %d\n", ESP.getFreeHeap());
    if (!SPIFFS.begin(true)) {
      Serial.println("[Web] SPIFFS mount failed!");
      return;
    }
    setupRoutes();
    server.begin();
    serverStarted = true;
    Serial.printf("[Web] Server started on port 80, free heap: %d\n", ESP.getFreeHeap());
  }

  void stopWebServer()
  {
    server.end();
  }

  void setupRoutes()
  {
    // Página principal (gzipped para transferencia más rápida)
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", WEB_UI_HTML_GZ, WEB_UI_HTML_GZ_LEN);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });

    // API: obtener configuración (usando ArduinoJson para evitar fragmentación de heap)
    server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
      StaticJsonDocument<1024> doc;
      ClockwiseParams* p = ClockwiseParams::getInstance();

      // WiFi
      doc["wifiSsid"] = p->wifiSsid;
      doc["wifiHasPwd"] = p->wifiPwd.length() > 0 ? 1 : 0;
      doc["wifiSsid2"] = p->wifiSsid2;
      doc["wifiHasPwd2"] = p->wifiPwd2.length() > 0 ? 1 : 0;
      doc["wifiSsid3"] = p->wifiSsid3;
      doc["wifiHasPwd3"] = p->wifiPwd3.length() > 0 ? 1 : 0;
      doc["wifiConnected"] = WiFi.SSID();
      doc["wifiRssi"] = WiFi.RSSI();
      // Display
      doc["displayBright"] = p->displayBright;
      doc["displayRotation"] = p->displayRotation;
      doc["swapBlueGreen"] = p->swapBlueGreen ? 1 : 0;
      doc["autoBrightMin"] = p->autoBrightMin;
      doc["autoBrightMax"] = p->autoBrightMax;
      doc["ldrPin"] = p->ldrPin;
      // Time
      doc["timeZone"] = p->timeZone;
      doc["ntpServer"] = p->ntpServer;
      doc["manualPosix"] = p->manualPosix;
      doc["use24hFormat"] = p->use24hFormat ? 1 : 0;
      doc["useSpanish"] = p->useSpanish ? 1 : 0;
      // Night mode
      doc["nightEnabled"] = p->nightModeEnabled ? 1 : 0;
      doc["nightStart"] = p->nightModeStart;
      doc["nightEnd"] = p->nightModeEnd;
      doc["nightBright"] = p->nightBrightness;
      doc["nightColor"] = p->nightColor;
      doc["nightClock"] = p->nightClockface;
      // Clockface
      doc["canvasFile"] = p->canvasFile;
      doc["rotationEnabled"] = p->rotationEnabled ? 1 : 0;
      doc["rotationInterval"] = p->rotationInterval;
      doc["rotationList"] = p->rotationList;
      doc["localServerHost"] = p->localServerHost;
      doc["localServerPort"] = p->localServerPort;
      // System
      doc["version"] = CW_FW_VERSION;
      doc["name"] = CW_FW_NAME;

      String output;
      serializeJson(doc, output);
      request->send(200, "application/json", output);
    });

    // API: establecer parámetro
    server.on("/api/set", HTTP_POST, [this](AsyncWebServerRequest *request) {
      if (request->params() > 0) {
        const AsyncWebParameter* p = request->getParam((size_t)0);
        String key = p->name();
        String value = p->value();
        ClockwiseParams* params = ClockwiseParams::getInstance();

        // WiFi
        if (key == "wifiSsid") params->wifiSsid = value;
        else if (key == "wifiPwd") params->wifiPwd = value;
        else if (key == "wifiSsid2") params->wifiSsid2 = value;
        else if (key == "wifiPwd2") params->wifiPwd2 = value;
        else if (key == "wifiSsid3") params->wifiSsid3 = value;
        else if (key == "wifiPwd3") params->wifiPwd3 = value;
        // Display
        else if (key == "displayBright") {
          params->displayBright = value.toInt();
          pending_brightness = value.toInt();
          needs_brightness_update = true;
        }
        else if (key == "displayRotation") {
          params->displayRotation = value.toInt();
          pending_display_rotation = value.toInt();
          needs_display_rotation_update = true;
        }
        else if (key == "swapBlueGreen") params->swapBlueGreen = (value == "1");
        else if (key == "autoBrightMin") params->autoBrightMin = value.toInt();
        else if (key == "autoBrightMax") params->autoBrightMax = value.toInt();
        else if (key == "ldrPin") params->ldrPin = value.toInt();
        // Time
        else if (key == "timeZone") params->timeZone = value;
        else if (key == "ntpServer") params->ntpServer = value;
        else if (key == "manualPosix") params->manualPosix = value;
        else if (key == "use24hFormat") params->use24hFormat = (value == "1");
        else if (key == "useSpanish") params->useSpanish = (value == "1");
        // Night mode
        else if (key == "nightEnabled") params->nightModeEnabled = (value == "1");
        else if (key == "nightStart") params->nightModeStart = value;
        else if (key == "nightEnd") params->nightModeEnd = value;
        else if (key == "nightBright") {
          params->nightBrightness = value.toInt();
          pending_night_brightness = value.toInt();
          needs_night_brightness_update = true;
        }
        else if (key == "nightColor") params->nightColor = value.toInt();
        else if (key == "nightClock") params->nightClockface = value;
        // Clockface (clear preview when changing from web UI)
        else if (key == "canvasFile") {
          params->canvasFile = value;
          CWPreview::getInstance()->clearPreview();
        }
        else if (key == "rotationEnabled") {
          params->rotationEnabled = (value == "1");
          rotation_changed = true;
        }
        else if (key == "rotationInterval") params->rotationInterval = value.toInt();
        else if (key == "rotationList") params->rotationList = value;
        else if (key == "localServerHost") params->localServerHost = value;
        else if (key == "localServerPort") params->localServerPort = value.toInt();

        params->save();
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

    // API: limpiar preview y volver a caratula normal
    server.on("/api/preview/clear", HTTP_POST, [this](AsyncWebServerRequest *request) {
      CWPreview::getInstance()->clearPreview();
      request->send(200, "text/plain", "Preview cleared");
      needs_reload = true;
    });

    // API: recibir clockface JSON para preview directo
    server.on("/api/clockface", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK");
      },
      NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        static char* bodyBuffer = nullptr;
        static size_t bufferPos = 0;
        if (index == 0) {
          if (bodyBuffer) free(bodyBuffer);
          bodyBuffer = (char*)malloc(total + 1);
          bufferPos = 0;
        }
        if (bodyBuffer) {
          memcpy(bodyBuffer + bufferPos, data, len);
          bufferPos += len;
        }
        if (index + len == total && bodyBuffer) {
          bodyBuffer[total] = '\0';
          CWPreview::getInstance()->setPreview(String(bodyBuffer));
          Serial.printf("[Preview] Received clockface JSON (%d bytes)\n", total);
          free(bodyBuffer);
          bodyBuffer = nullptr;
        }
      }
    );

    // API: iniciar escaneo WiFi (asíncrono)
    server.on("/api/scan/start", HTTP_POST, [](AsyncWebServerRequest *request) {
      WiFi.scanDelete();
      WiFi.scanNetworks(true, true);
      request->send(200, "application/json", "{\"status\":\"scanning\"}");
    });

    // API: obtener resultado del escaneo
    server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
      int n = WiFi.scanComplete();
      StaticJsonDocument<1024> doc;

      if (n == WIFI_SCAN_RUNNING) {
        doc["status"] = "scanning";
      } else if (n == WIFI_SCAN_FAILED || n < 0) {
        doc["status"] = "error";
        WiFi.scanDelete();
      } else {
        doc["status"] = "done";
        JsonArray networks = doc.createNestedArray("networks");
        for (int i = 0; i < n && i < 15; i++) {
          String ssid = WiFi.SSID(i);
          if (ssid.length() == 0) continue;
          JsonObject net = networks.createNestedObject();
          net["ssid"] = ssid;
          net["rssi"] = WiFi.RSSI(i);
          net["open"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 1 : 0;
        }
        WiFi.scanDelete();
      }

      String output;
      serializeJson(doc, output);
      request->send(200, "application/json", output);
    });

    // API: reset WiFi config
    server.on("/api/resetwifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
      ClockwiseParams* params = ClockwiseParams::getInstance();
      params->wifiSsid = "";
      params->wifiPwd = "";
      params->wifiSsid2 = "";
      params->wifiPwd2 = "";
      params->wifiSsid3 = "";
      params->wifiPwd3 = "";
      params->save();
      request->send(200, "text/plain", "WiFi reset, restarting...");
      force_restart = true;
    });

    // API: obtener info del sistema (RAM, uptime, etc)
    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request) {
      StaticJsonDocument<256> doc;
      doc["freeHeap"] = ESP.getFreeHeap();
      doc["totalHeap"] = ESP.getHeapSize();
      doc["minFreeHeap"] = ESP.getMinFreeHeap();
      doc["chipModel"] = ESP.getChipModel();
      doc["chipCores"] = ESP.getChipCores();
      doc["cpuFreqMHz"] = ESP.getCpuFreqMHz();
      doc["mac"] = WiFi.macAddress();
      doc["uptimeMs"] = millis();
      String output;
      serializeJson(doc, output);
      request->send(200, "application/json", output);
    });

    // API: obtener espacio de almacenamiento
    server.on("/api/storage", HTTP_GET, [](AsyncWebServerRequest *request) {
      StaticJsonDocument<128> doc;
      size_t total = SPIFFS.totalBytes();
      size_t used = SPIFFS.usedBytes();
      doc["total"] = total;
      doc["used"] = used;
      doc["free"] = total - used;
      String output;
      serializeJson(doc, output);
      request->send(200, "application/json", output);
    });

    // API: listar carátulas guardadas (solo nombre y tamaño - rápido)
    server.on("/api/clockfaces/list", HTTP_GET, [](AsyncWebServerRequest *request) {
      String json = "[";
      File root = SPIFFS.open("/");
      if (!root) {
        request->send(200, "application/json", "[]");
        return;
      }
      File file = root.openNextFile();
      bool first = true;
      while (file) {
        String fname = String(file.name());
        if (!file.isDirectory() && fname.endsWith(".json")) {
          if (!first) json += ",";
          first = false;
          String name = fname;
          if (name.startsWith("/")) name = name.substring(1);
          name = name.substring(0, name.length() - 5);
          json += "{\"name\":\"" + name + "\",\"size\":" + String(file.size()) + "}";
        }
        file.close();
        file = root.openNextFile();
      }
      root.close();
      json += "]";
      request->send(200, "application/json", json);
    });

    // API: verificar si carátula existe (para confirmar antes de sobrescribir)
    server.on("/api/clockfaces/exists", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (!request->hasParam("name")) {
        request->send(400, "application/json", "{\"error\":\"Missing name\"}");
        return;
      }
      String name = request->getParam("name")->value();
      String path = "/" + name + ".json";

      if (SPIFFS.exists(path)) {
        File f = SPIFFS.open(path, FILE_READ);
        size_t size = f.size();
        f.close();
        request->send(200, "application/json", "{\"exists\":true,\"size\":" + String(size) + "}");
      } else {
        request->send(200, "application/json", "{\"exists\":false}");
      }
    });

    // API: ping para verificar que el servidor responde
    server.on("/api/ping", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "pong");
    });

    // API: subir carátula (recibe JSON) - escribe directamente a SPIFFS sin buffering
    server.on("/api/clockfaces/upload", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK");
      },
      NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        static File uploadFile;
        static String filePath;
        static bool uploadError = false;

        if (index == 0) {
          uploadError = false;
          String fileName;
          if (request->hasParam("name", true)) {
            fileName = request->getParam("name", true)->value();
          } else if (request->hasParam("name")) {
            fileName = request->getParam("name")->value();
          } else {
            fileName = "uploaded";
          }

          filePath = "/" + fileName + ".json";
          uploadFile = SPIFFS.open(filePath, FILE_WRITE);
          if (!uploadFile) {
            Serial.printf("[Storage] Failed to create: %s\n", filePath.c_str());
            uploadError = true;
            return;
          }
          Serial.printf("[Storage] Starting upload: %s (%d bytes), heap: %d\n", filePath.c_str(), total, ESP.getFreeHeap());
        }

        if (uploadError) return;

        // Write chunk directly to file
        if (uploadFile) {
          size_t written = uploadFile.write(data, len);
          if (written != len) {
            Serial.printf("[Storage] Write error: %d/%d bytes\n", written, len);
            uploadError = true;
            uploadFile.close();
            return;
          }
        }

        // Close file when done
        if (index + len == total) {
          if (uploadFile) {
            uploadFile.close();
            Serial.printf("[Storage] Saved: %s (%d bytes), heap: %d\n", filePath.c_str(), total, ESP.getFreeHeap());
          }
        }
      }
    );

    // API: thumbnail ligero — lee solo primeros 4KB para extraer imagen sin bajar JSON completo
    server.on("/api/clockfaces/thumb", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (!request->hasParam("name")) { request->send(400); return; }
      String name = request->getParam("name")->value();
      String path = "/" + name + ".json";
      if (!SPIFFS.exists(path)) { request->send(404); return; }

      File f = SPIFFS.open(path, FILE_READ);
      size_t fileSize = f.size();
      const size_t READ_SIZE = fileSize < 4096 ? fileSize : 4096;
      char* buf = (char*)malloc(READ_SIZE + 1);
      if (!buf) {
        f.close();
        request->send(503, "text/plain", "Retry");
        return;
      }
      f.read((uint8_t*)buf, READ_SIZE);
      buf[READ_SIZE] = '\0';
      f.close();

      int bg = 0;
      char* bgPos = strstr(buf, "\"bgColor\":");
      if (bgPos) bg = atoi(bgPos + 10);

      String imgStr = "";
      const char* keys[] = {"\"thumbnail\":", "\"image\":"};
      for (const char* key : keys) {
        char* pos = strstr(buf, key);
        if (pos) {
          const char* p = pos + strlen(key);
          while (*p == ' ' || *p == '\t') p++;
          if (*p == '"') {
            const char* start = p + 1;
            const char* end = strchr(start, '"');
            if (end && end > start) { imgStr = String(start, (unsigned int)(end - start)); break; }
          }
        }
      }
      free(buf);

      request->send(200, "application/json",
        "{\"bg\":" + String(bg) + ",\"img\":\"" + imgStr + "\"}");
    });

    // API: eliminar carátula
    server.on("/api/clockfaces/delete", HTTP_POST, [](AsyncWebServerRequest *request) {
      String name;
      if (request->hasParam("name", true)) {
        name = request->getParam("name", true)->value();
      } else if (request->hasParam("name")) {
        name = request->getParam("name")->value();
      } else {
        request->send(400, "text/plain", "Missing name parameter");
        return;
      }
      String path = "/" + name + ".json";

      if (SPIFFS.exists(path)) {
        SPIFFS.remove(path);
        Serial.printf("[Storage] Deleted: %s\n", path.c_str());
        request->send(200, "text/plain", "Deleted");
      } else {
        request->send(404, "text/plain", "Not found");
      }
    });

    // API: descargar carátula guardada
    server.on("/api/clockfaces/get", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (!request->hasParam("name")) {
        request->send(400, "text/plain", "Missing name parameter");
        return;
      }
      String name = request->getParam("name")->value();
      String path = "/" + name + ".json";

      if (SPIFFS.exists(path)) {
        request->send(SPIFFS, path, "application/json");
      } else {
        request->send(404, "text/plain", "Not found");
      }
    });

    // CORS headers for editor
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

    server.on("/api/clockface", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
      request->send(204);
    });

    server.on("/api/clockfaces/upload", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
      request->send(204);
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
