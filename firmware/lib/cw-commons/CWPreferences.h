#pragma once

#include <Preferences.h>

#ifndef CW_PREF_DB_NAME
    #define CW_PREF_DB_NAME "clockwise"
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
    const char* const PREF_CANVAS_SERVER = "canvasServer";
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
    const char* const PREF_CLOCKFACE_SOURCE = "cfSource";
    const char* const PREF_LOCAL_SERVER_HOST = "localHost";
    const char* const PREF_LOCAL_SERVER_PORT = "localPort";

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
    String canvasServer;
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
    String clockfaceSource;  // "cdn", "github", or "local"
    String localServerHost;  // IP/hostname for local dev server
    uint16_t localServerPort; // Port for local dev server


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
        preferences.putString(PREF_CANVAS_SERVER, canvasServer);
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
        preferences.putString(PREF_CLOCKFACE_SOURCE, clockfaceSource);
        preferences.putString(PREF_LOCAL_SERVER_HOST, localServerHost);
        preferences.putUInt(PREF_LOCAL_SERVER_PORT, localServerPort);
    }

    void load()
    {
        swapBlueGreen = preferences.getBool(PREF_SWAP_BLUE_GREEN, false);
        use24hFormat = preferences.getBool(PREF_USE_24H_FORMAT, true);
        useSpanish = preferences.getBool(PREF_USE_SPANISH, true);
        displayBright = preferences.getUInt(PREF_DISPLAY_BRIGHT, 32);
        autoBrightMin = preferences.getUInt(PREF_DISPLAY_ABC_MIN, 0);
        autoBrightMax = preferences.getUInt(PREF_DISPLAY_ABC_MAX, 0);
        ldrPin = preferences.getUInt(PREF_LDR_PIN, 35);
        timeZone = preferences.getString(PREF_TIME_ZONE, "America/Mexico_City");
        // WiFi multi-red
        wifiSsid = preferences.getString(PREF_WIFI_SSID, "");
        wifiPwd = preferences.getString(PREF_WIFI_PASSWORD, "");
        wifiSsid2 = preferences.getString(PREF_WIFI_SSID_2, "");
        wifiPwd2 = preferences.getString(PREF_WIFI_PASSWORD_2, "");
        wifiSsid3 = preferences.getString(PREF_WIFI_SSID_3, "");
        wifiPwd3 = preferences.getString(PREF_WIFI_PASSWORD_3, "");
        ntpServer = preferences.getString(PREF_NTP_SERVER, "time.google.com");
        canvasFile = preferences.getString(PREF_CANVAS_FILE, "");
        canvasServer = preferences.getString(PREF_CANVAS_SERVER, "raw.githubusercontent.com");
        manualPosix = preferences.getString(PREF_MANUAL_POSIX, "");
        displayRotation = preferences.getUInt(PREF_DISPLAY_ROTATION, 0);
        nightModeEnabled = preferences.getBool(PREF_NIGHT_MODE_ENABLED, false);
        nightModeStart = preferences.getString(PREF_NIGHT_MODE_START, "22:00");
        nightModeEnd = preferences.getString(PREF_NIGHT_MODE_END, "07:00");
        nightBrightness = preferences.getUInt(PREF_NIGHT_BRIGHTNESS, 8);
        nightColor = preferences.getUInt(PREF_NIGHT_COLOR, 63488);
        nightClockface = preferences.getString(PREF_NIGHT_CLOCKFACE, "nigth-clock-1");
        rotationEnabled = preferences.getBool(PREF_ROTATION_ENABLED, false);
        rotationList = preferences.getString(PREF_ROTATION_LIST, "");
        rotationIndex = preferences.getUInt(PREF_ROTATION_INDEX, 0);
        rotationInterval = preferences.getUInt(PREF_ROTATION_INTERVAL, 1440);
        clockfaceSource = preferences.getString(PREF_CLOCKFACE_SOURCE, "ghpages");
        localServerHost = preferences.getString(PREF_LOCAL_SERVER_HOST, "192.168.1.100");
        localServerPort = preferences.getUInt(PREF_LOCAL_SERVER_PORT, 8080);
    }

};
