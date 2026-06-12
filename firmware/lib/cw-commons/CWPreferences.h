#pragma once

#include <Preferences.h>

#ifndef CW_PREF_DB_NAME
    #define CW_PREF_DB_NAME "clockwise"
#endif

// Pin LDR por defecto segun la placa (definido en platformio.ini).
// ESP32 clasico: GPIO35 (ADC1_CH7). ESP32-S3: GPIO1 (ADC1_CH0), ya que
// GPIO35 NO es un pin ADC valido en el S3.
#ifndef CW_LDR_PIN_DEFAULT
    #define CW_LDR_PIN_DEFAULT 35
#endif

// Pines I2C por defecto del sensor ambiental (BME280), segun la placa
// (definidos en platformio.ini). ESP32 clasico: SDA=21/SCL=22 (I2C clasico).
// ESP32-S3: SDA=10/SCL=9 (el GPIO8 lo usa el display HUB75 como linea B).
#ifndef CW_SENSOR_SDA_DEFAULT
    #define CW_SENSOR_SDA_DEFAULT 21
#endif
#ifndef CW_SENSOR_SCL_DEFAULT
    #define CW_SENSOR_SCL_DEFAULT 22
#endif


struct ClockwiseParams
{
    Preferences preferences;

    const char* const PREF_SWAP_BLUE_GREEN = "swapBlueGreen";
    const char* const PREF_USE_24H_FORMAT = "use24hFormat";
    const char* const PREF_USE_SPANISH = "useSpanish";
    const char* const PREF_DISPLAY_BRIGHT = "displayBright";
    const char* const PREF_DISPLAY_ABC_MIN = "autoBrightMin";
    const char* const PREF_DISPLAY_ABC_MAX = "autoBrightMax";
    const char* const PREF_LDR_PIN = "ldrPin";
    const char* const PREF_TIME_ZONE = "timeZone";
    // WiFi multi-red (3 redes)
    const char* const PREF_WIFI_SSID = "wifiSsid";      // Red 1 (legacy/default)
    const char* const PREF_WIFI_PASSWORD = "wifiPwd";
    const char* const PREF_WIFI_SSID_2 = "wifiSsid2";   // Red 2
    const char* const PREF_WIFI_PASSWORD_2 = "wifiPwd2";
    const char* const PREF_WIFI_SSID_3 = "wifiSsid3";   // Red 3
    const char* const PREF_WIFI_PASSWORD_3 = "wifiPwd3";
    const char* const PREF_NTP_SERVER = "ntpServer";
    const char* const PREF_CANVAS_FILE = "canvasFile";
    const char* const PREF_MANUAL_POSIX = "manualPosix";
    const char* const PREF_DISPLAY_ROTATION = "displayRotation";
    const char* const PREF_NIGHT_MODE_ENABLED = "nightEnabled";
    const char* const PREF_NIGHT_MODE_START = "nightStart";
    const char* const PREF_NIGHT_MODE_END = "nightEnd";
    const char* const PREF_NIGHT_BRIGHTNESS = "nightBright";
    const char* const PREF_NIGHT_COLOR = "nightColor";
    const char* const PREF_NIGHT_CLOCKFACE = "nightClock";
    const char* const PREF_ROTATION_ENABLED = "rotEnabled";
    const char* const PREF_ROTATION_LIST = "rotList";
    const char* const PREF_ROTATION_INDEX = "rotIndex";
    const char* const PREF_ROTATION_INTERVAL = "rotInterval";
    const char* const PREF_LOCAL_SERVER_HOST = "localHost";
    const char* const PREF_LOCAL_SERVER_PORT = "localPort";
    // Sensor ambiental (BME280)
    const char* const PREF_SENSOR_ENABLED = "sensorEnabled";
    const char* const PREF_SENSOR_SDA = "sensorSda";
    const char* const PREF_SENSOR_SCL = "sensorScl";
    const char* const PREF_SENSOR_ADDR = "sensorAddr";
    const char* const PREF_TEMP_FAHRENHEIT = "tempFahrenheit";

    bool swapBlueGreen;
    bool use24hFormat;
    bool useSpanish;
    uint8_t displayBright;
    uint16_t autoBrightMin;
    uint16_t autoBrightMax;
    uint8_t ldrPin;
    String timeZone;
    // WiFi multi-red (3 redes)
    String wifiSsid;    // Red 1
    String wifiPwd;
    String wifiSsid2;   // Red 2
    String wifiPwd2;
    String wifiSsid3;   // Red 3
    String wifiPwd3;
    String ntpServer;
    String canvasFile;
    String manualPosix;
    uint8_t displayRotation;
    bool nightModeEnabled;
    String nightModeStart;
    String nightModeEnd;
    uint8_t nightBrightness;
    uint16_t nightColor;
    String nightClockface;
    bool rotationEnabled;
    String rotationList;
    uint8_t rotationIndex;
    uint16_t rotationInterval;
    String localServerHost;  // IP/hostname for local dev server
    uint16_t localServerPort; // Port for local dev server
    // Sensor ambiental (BME280)
    bool sensorEnabled;
    uint8_t sensorSdaPin;
    uint8_t sensorSclPin;
    uint8_t sensorAddr;     // 0 = autodeteccion (0x76/0x77)
    bool tempFahrenheit;    // false = grados C, true = grados F


    ClockwiseParams() {
        preferences.begin("clockwise", false); 
        //preferences.clear();
    }

    static ClockwiseParams* getInstance() {
        static ClockwiseParams base;
        return &base;
    }

   
    void save()
    {
        preferences.putBool(PREF_SWAP_BLUE_GREEN, swapBlueGreen);
        preferences.putBool(PREF_USE_24H_FORMAT, use24hFormat);
        preferences.putBool(PREF_USE_SPANISH, useSpanish);
        preferences.putUInt(PREF_DISPLAY_BRIGHT, displayBright);
        preferences.putUInt(PREF_DISPLAY_ABC_MIN, autoBrightMin);
        preferences.putUInt(PREF_DISPLAY_ABC_MAX, autoBrightMax);
        preferences.putUInt(PREF_LDR_PIN, ldrPin);
        preferences.putString(PREF_TIME_ZONE, timeZone);
        // WiFi multi-red
        preferences.putString(PREF_WIFI_SSID, wifiSsid);
        preferences.putString(PREF_WIFI_PASSWORD, wifiPwd);
        preferences.putString(PREF_WIFI_SSID_2, wifiSsid2);
        preferences.putString(PREF_WIFI_PASSWORD_2, wifiPwd2);
        preferences.putString(PREF_WIFI_SSID_3, wifiSsid3);
        preferences.putString(PREF_WIFI_PASSWORD_3, wifiPwd3);
        preferences.putString(PREF_NTP_SERVER, ntpServer);
        preferences.putString(PREF_CANVAS_FILE, canvasFile);
        preferences.putString(PREF_MANUAL_POSIX, manualPosix);
        preferences.putUInt(PREF_DISPLAY_ROTATION, displayRotation);
        preferences.putBool(PREF_NIGHT_MODE_ENABLED, nightModeEnabled);
        preferences.putString(PREF_NIGHT_MODE_START, nightModeStart);
        preferences.putString(PREF_NIGHT_MODE_END, nightModeEnd);
        preferences.putUInt(PREF_NIGHT_BRIGHTNESS, nightBrightness);
        preferences.putUInt(PREF_NIGHT_COLOR, nightColor);
        preferences.putString(PREF_NIGHT_CLOCKFACE, nightClockface);
        preferences.putBool(PREF_ROTATION_ENABLED, rotationEnabled);
        preferences.putString(PREF_ROTATION_LIST, rotationList);
        preferences.putUInt(PREF_ROTATION_INDEX, rotationIndex);
        preferences.putUInt(PREF_ROTATION_INTERVAL, rotationInterval);
        preferences.putString(PREF_LOCAL_SERVER_HOST, localServerHost);
        preferences.putUInt(PREF_LOCAL_SERVER_PORT, localServerPort);
        // Sensor ambiental (BME280)
        preferences.putBool(PREF_SENSOR_ENABLED, sensorEnabled);
        preferences.putUInt(PREF_SENSOR_SDA, sensorSdaPin);
        preferences.putUInt(PREF_SENSOR_SCL, sensorSclPin);
        preferences.putUInt(PREF_SENSOR_ADDR, sensorAddr);
        preferences.putBool(PREF_TEMP_FAHRENHEIT, tempFahrenheit);
    }

    void load()
    {
        swapBlueGreen = preferences.getBool(PREF_SWAP_BLUE_GREEN, false);
        use24hFormat = preferences.getBool(PREF_USE_24H_FORMAT, true);
        useSpanish = preferences.getBool(PREF_USE_SPANISH, true);
        displayBright = preferences.getUInt(PREF_DISPLAY_BRIGHT, 32);
        autoBrightMin = preferences.getUInt(PREF_DISPLAY_ABC_MIN, 0);
        autoBrightMax = preferences.getUInt(PREF_DISPLAY_ABC_MAX, 0);
        ldrPin = preferences.getUInt(PREF_LDR_PIN, CW_LDR_PIN_DEFAULT);
        timeZone = preferences.getString(PREF_TIME_ZONE, "America/Mexico_City");
        // WiFi multi-red
        wifiSsid = preferences.getString(PREF_WIFI_SSID, "");
        wifiPwd = preferences.getString(PREF_WIFI_PASSWORD, "");
        wifiSsid2 = preferences.getString(PREF_WIFI_SSID_2, "");
        wifiPwd2 = preferences.getString(PREF_WIFI_PASSWORD_2, "");
        wifiSsid3 = preferences.getString(PREF_WIFI_SSID_3, "");
        wifiPwd3 = preferences.getString(PREF_WIFI_PASSWORD_3, "");
        ntpServer = preferences.getString(PREF_NTP_SERVER, "time.cloudflare.com");
        canvasFile = preferences.getString(PREF_CANVAS_FILE, "_mario");
        manualPosix = preferences.getString(PREF_MANUAL_POSIX, "");
        displayRotation = preferences.getUInt(PREF_DISPLAY_ROTATION, 0);
        nightModeEnabled = preferences.getBool(PREF_NIGHT_MODE_ENABLED, false);
        nightModeStart = preferences.getString(PREF_NIGHT_MODE_START, "22:00");
        nightModeEnd = preferences.getString(PREF_NIGHT_MODE_END, "07:00");
        nightBrightness = preferences.getUInt(PREF_NIGHT_BRIGHTNESS, 8);
        nightColor = preferences.getUInt(PREF_NIGHT_COLOR, 63488);
        nightClockface = preferences.getString(PREF_NIGHT_CLOCKFACE, "_builtin");
        rotationEnabled = preferences.getBool(PREF_ROTATION_ENABLED, true);
        rotationList = preferences.getString(PREF_ROTATION_LIST, "_mario,_pacman");
        rotationIndex = preferences.getUInt(PREF_ROTATION_INDEX, 0);
        rotationInterval = preferences.getUInt(PREF_ROTATION_INTERVAL, 5);
        localServerHost = preferences.getString(PREF_LOCAL_SERVER_HOST, "192.168.1.100");
        localServerPort = preferences.getUInt(PREF_LOCAL_SERVER_PORT, 8080);
        // Sensor ambiental (BME280)
        sensorEnabled = preferences.getBool(PREF_SENSOR_ENABLED, false);
        sensorSdaPin = preferences.getUInt(PREF_SENSOR_SDA, CW_SENSOR_SDA_DEFAULT);
        sensorSclPin = preferences.getUInt(PREF_SENSOR_SCL, CW_SENSOR_SCL_DEFAULT);
        sensorAddr = preferences.getUInt(PREF_SENSOR_ADDR, 0);
        tempFahrenheit = preferences.getBool(PREF_TEMP_FAHRENHEIT, false);
    }

};
