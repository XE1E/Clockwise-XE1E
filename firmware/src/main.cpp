#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Clockfaces
#include <JsonClockface.h>
#include <PacmanClockface.h>
#include <MarioClockface.h>
#include <BME680Clockface.h>
#include <IClockface.h>

// Commons
#include <WiFiController.h>
#include <CWDateTime.h>
#include <CWPreferences.h>
#include <CWWebServer.h>
#include <StatusController.h>

#define MIN_BRIGHT_DISPLAY_ON 4
#define MIN_BRIGHT_DISPLAY_OFF 0

#ifdef CONFIG_IDF_TARGET_ESP32S3
  #define ESP32_LED_BUILTIN -1  // ESP32-S3 DevKit uses RGB LED (WS2812), not simple GPIO
#else
  #define ESP32_LED_BUILTIN 2
#endif

MatrixPanel_I2S_DMA *dma_display = nullptr;

IClockface *clockface = nullptr;
JsonClockface *jsonClockface = nullptr;
PacmanClockface *pacmanClockface = nullptr;
MarioClockface *marioClockface = nullptr;
BME680Clockface *bme680Clockface = nullptr;

// Native clockface names
const char* PACMAN_CLOCKFACE = "_pacman";
const char* MARIO_CLOCKFACE = "_mario";
const char* BME680_CLOCKFACE = "_bme680";

WiFiController wifi;
CWDateTime cwDateTime;

bool autoBrightEnabled;
long autoBrightMillis = 0;
uint8_t currentBrightSlot = 255;

bool nightModeActive = false;
long rotationMillis = 0;
bool needsClockfaceReload = false;
String currentClockface = "";

// Select the appropriate clockface based on name
void selectClockface(const String& name) {
  if (name == PACMAN_CLOCKFACE) {
    clockface = pacmanClockface;
    Serial.println("[Clockface] Using Pac-Man native clockface");
  } else if (name == MARIO_CLOCKFACE) {
    clockface = marioClockface;
    Serial.println("[Clockface] Using Mario Bros native clockface");
  } else if (name == BME680_CLOCKFACE) {
    clockface = bme680Clockface;
    Serial.println("[Clockface] Using BME680 environmental clockface");
  } else {
    clockface = jsonClockface;
    Serial.println("[Clockface] Using JSON clockface");
  }
}

// Check if a clockface name is a native (built-in) clockface
bool isNativeClockface(const String& name) {
  return name == PACMAN_CLOCKFACE || name == MARIO_CLOCKFACE || name == BME680_CLOCKFACE;
}

// Migrate old nightClockface values to new format
String getNightClockface() {
  String val = ClockwiseParams::getInstance()->nightClockface;
  // Migrate old values
  if (val == "cw-cf-0x05") return "_pacman";
  if (val == "cw-cf-0x01") return "_mario";
  return val;
}

int parseHour(String timeStr) {
  int colonPos = timeStr.indexOf(':');
  if (colonPos > 0) {
    return timeStr.substring(0, colonPos).toInt();
  }
  return 0;
}

int parseMinute(String timeStr) {
  int colonPos = timeStr.indexOf(':');
  if (colonPos > 0) {
    return timeStr.substring(colonPos + 1).toInt();
  }
  return 0;
}

bool isNightTime() {
  if (!ClockwiseParams::getInstance()->nightModeEnabled) return false;

  int currentHour = cwDateTime.getHour24();
  int currentMinute = cwDateTime.getMinute();
  int currentTime = currentHour * 60 + currentMinute;

  String startStr = ClockwiseParams::getInstance()->nightModeStart;
  String endStr = ClockwiseParams::getInstance()->nightModeEnd;

  int startTime = parseHour(startStr) * 60 + parseMinute(startStr);
  int endTime = parseHour(endStr) * 60 + parseMinute(endStr);

  if (startTime > endTime) {
    return (currentTime >= startTime || currentTime < endTime);
  } else {
    return (currentTime >= startTime && currentTime < endTime);
  }
}

unsigned long lastNightModeCheck = 0;

void checkNightMode() {
  // Only check every 5 seconds to reduce CPU usage
  if (millis() - lastNightModeCheck < 5000) return;
  lastNightModeCheck = millis();

  if (!ClockwiseParams::getInstance()->nightModeEnabled) {
    if (nightModeActive) {
      nightModeActive = false;
      clockface->setBuiltinNightMode(false);
      dma_display->setBrightness8(ClockwiseParams::getInstance()->displayBright);
      ClockwiseParams::getInstance()->canvasFile = currentClockface;
      needsClockfaceReload = true;
    }
    return;
  }

  bool shouldBeNight = isNightTime();

  if (shouldBeNight && !nightModeActive) {
    nightModeActive = true;
    currentClockface = ClockwiseParams::getInstance()->canvasFile;
    dma_display->setBrightness8(ClockwiseParams::getInstance()->nightBrightness);

    String nightClock = getNightClockface();
    if (nightClock == "_builtin") {
      // Builtin night mode always uses jsonClockface
      clockface = jsonClockface;
      clockface->setBuiltinNightMode(true, ClockwiseParams::getInstance()->nightColor);
    } else {
      ClockwiseParams::getInstance()->canvasFile = nightClock;
      needsClockfaceReload = true;
    }
  } else if (!shouldBeNight && nightModeActive) {
    nightModeActive = false;
    clockface->setBuiltinNightMode(false);
    dma_display->setBrightness8(ClockwiseParams::getInstance()->displayBright);
    ClockwiseParams::getInstance()->canvasFile = currentClockface;
    needsClockfaceReload = true;
  }
}

unsigned long lastRotationCheck = 0;

void checkClockfaceRotation() {
  // Only check every 10 seconds to reduce CPU usage
  if (millis() - lastRotationCheck < 10000) return;
  lastRotationCheck = millis();

  if (!ClockwiseParams::getInstance()->rotationEnabled || nightModeActive) return;

  String rotList = ClockwiseParams::getInstance()->rotationList;
  if (rotList.length() == 0) return;

  uint16_t interval = ClockwiseParams::getInstance()->rotationInterval;
  if (interval == 0) return;

  int count = 1;
  for (int i = 0; i < rotList.length(); i++) {
    if (rotList[i] == ',') count++;
  }
  if (count < 2) return;

  unsigned long intervalMs = interval * 60000UL;

  if (millis() - rotationMillis >= intervalMs) {

    uint8_t nextIndex = (ClockwiseParams::getInstance()->rotationIndex + 1) % count;
    ClockwiseParams::getInstance()->rotationIndex = nextIndex;
    // Note: Not saving to flash here to avoid slowdown - index resets on restart

    int currentPos = 0;
    int commaCount = 0;
    int startPos = 0;

    for (int i = 0; i <= rotList.length(); i++) {
      if (i == rotList.length() || rotList[i] == ',') {
        if (commaCount == nextIndex) {
          String nextClockface = rotList.substring(startPos, i);
          nextClockface.trim();
          ClockwiseParams::getInstance()->canvasFile = nextClockface;
          currentClockface = nextClockface;
          needsClockfaceReload = true;
          break;
        }
        commaCount++;
        startPos = i + 1;
      }
    }

    rotationMillis = millis();
  }
}

void displaySetup(bool swapBlueGreen, uint8_t displayBright, uint8_t displayRotation)
{
  HUB75_I2S_CFG mxconfig(64, 64, 1);

#ifdef CONFIG_IDF_TARGET_ESP32S3
  // ESP32-S3: uses library defaults for pins, only need to set pin E for 64x64 panels
  mxconfig.gpio.e = 38;

  if (swapBlueGreen)
  {
    // Swap Blue and Green pins for ESP32-S3 pinout
    mxconfig.gpio.b1 = 5;
    mxconfig.gpio.b2 = 15;
    mxconfig.gpio.g1 = 6;
    mxconfig.gpio.g2 = 16;
  }
#else
  // ESP32 original pinout
  if (swapBlueGreen)
  {
    // Swap Blue and Green pins because the panel is RBG instead of RGB.
    mxconfig.gpio.b1 = 26;
    mxconfig.gpio.b2 = 12;
    mxconfig.gpio.g1 = 27;
    mxconfig.gpio.g2 = 13;
  }
  mxconfig.gpio.e = 18;
#endif

  mxconfig.clkphase = false;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(displayBright);
  dma_display->clearScreen();
  dma_display->setRotation(displayRotation);
}

void automaticBrightControl()
{
  if (autoBrightEnabled) {
    if (millis() - autoBrightMillis > 3000)
    {
      int16_t currentValue = analogRead(ClockwiseParams::getInstance()->ldrPin);

      uint16_t ldrMin = ClockwiseParams::getInstance()->autoBrightMin;
      uint16_t ldrMax = ClockwiseParams::getInstance()->autoBrightMax;

      const uint8_t minBright = (currentValue < ldrMin ? MIN_BRIGHT_DISPLAY_OFF : MIN_BRIGHT_DISPLAY_ON);
      uint8_t maxBright = ClockwiseParams::getInstance()->displayBright;

      uint8_t slots = 10;
      uint8_t mapLDR = map(currentValue > ldrMax ? ldrMax : currentValue, ldrMin, ldrMax, 1, slots);
      uint8_t mapBright = map(mapLDR, 1, slots, minBright, maxBright);

      if (abs(currentBrightSlot - mapLDR) >= 2 || mapBright == 0) {
        dma_display->setBrightness8(mapBright);
        currentBrightSlot = mapLDR;
      }
      autoBrightMillis = millis();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  if (ESP32_LED_BUILTIN >= 0) {
    pinMode(ESP32_LED_BUILTIN, OUTPUT);
  }

  StatusController::getInstance()->blink_led(5, 100);

  ClockwiseParams::getInstance()->load();

  pinMode(ClockwiseParams::getInstance()->ldrPin, INPUT);

  displaySetup(ClockwiseParams::getInstance()->swapBlueGreen, ClockwiseParams::getInstance()->displayBright, ClockwiseParams::getInstance()->displayRotation);

  // Create all clockface instances
  jsonClockface = new JsonClockface(dma_display);
  pacmanClockface = new PacmanClockface(dma_display);
  marioClockface = new MarioClockface(dma_display);
  bme680Clockface = new BME680Clockface(dma_display);

  // Default to JSON clockface
  clockface = jsonClockface;

  autoBrightEnabled = (ClockwiseParams::getInstance()->autoBrightMax > 0);

  // If rotation is enabled, start with first item from rotation list
  if (ClockwiseParams::getInstance()->rotationEnabled) {
    String rotList = ClockwiseParams::getInstance()->rotationList;
    if (rotList.length() > 0) {
      int commaPos = rotList.indexOf(',');
      String firstClockface = (commaPos > 0) ? rotList.substring(0, commaPos) : rotList;
      firstClockface.trim();
      ClockwiseParams::getInstance()->canvasFile = firstClockface;
      ClockwiseParams::getInstance()->rotationIndex = 0;
    }
  }

  currentClockface = ClockwiseParams::getInstance()->canvasFile;
  rotationMillis = millis();

  StatusController::getInstance()->clockwiseLogo();
  delay(1000);

  StatusController::getInstance()->wifiConnecting();
  if (wifi.begin())
  {
    StatusController::getInstance()->ntpConnecting();
    cwDateTime.begin(ClockwiseParams::getInstance()->timeZone.c_str(),
        ClockwiseParams::getInstance()->use24hFormat,
        ClockwiseParams::getInstance()->ntpServer.c_str(),
        ClockwiseParams::getInstance()->manualPosix.c_str());
    cwDateTime.setSpanish(ClockwiseParams::getInstance()->useSpanish);

    // Check night mode BEFORE loading clockface to avoid flash
    bool shouldBeNight = ClockwiseParams::getInstance()->nightModeEnabled && isNightTime();
    if (shouldBeNight) {
      nightModeActive = true;
      dma_display->setBrightness8(ClockwiseParams::getInstance()->nightBrightness);
      String nightClock = getNightClockface();
      if (nightClock == "_builtin") {
        // Builtin night mode always uses jsonClockface
        clockface = jsonClockface;
        clockface->setupNightMode(&cwDateTime, ClockwiseParams::getInstance()->nightColor);
      } else {
        ClockwiseParams::getInstance()->canvasFile = nightClock;
        selectClockface(nightClock);
        clockface->setup(&cwDateTime);
      }
    } else {
      selectClockface(ClockwiseParams::getInstance()->canvasFile);
      clockface->setup(&cwDateTime);
    }
  }
}

void loop()
{
  wifi.handleImprovWiFi();
  if (wifi.checkReconnect()) {
    ezt::updateNTP();
  }

  // Cache instances to avoid repeated getInstance() calls
  ClockwiseWebServer* webServer = ClockwiseWebServer::getInstance();
  ClockwiseParams* params = ClockwiseParams::getInstance();

  if (wifi.isConnected())
  {
    webServer->handleRestart();

    // Apply brightness change from web UI immediately
    if (webServer->needs_brightness_update && !nightModeActive) {
      webServer->needs_brightness_update = false;
      dma_display->setBrightness8(webServer->pending_brightness);
    }

    // Apply night brightness change from web UI immediately (only if night mode is active)
    if (webServer->needs_night_brightness_update) {
      webServer->needs_night_brightness_update = false;
      if (nightModeActive) {
        dma_display->setBrightness8(webServer->pending_night_brightness);
      }
    }

    // Apply display rotation change from web UI immediately
    if (webServer->needs_display_rotation_update) {
      webServer->needs_display_rotation_update = false;
      dma_display->setRotation(webServer->pending_display_rotation);
      dma_display->clearScreen();
      needsClockfaceReload = true;
    }

    // Handle rotation enable/disable (skip if needs_reload pending - it will handle everything)
    if (webServer->rotation_changed && !webServer->needs_reload) {
      webServer->rotation_changed = false;
      // Skip if night mode is active
      if (params->rotationEnabled && !nightModeActive) {
        String rotList = params->rotationList;
        if (rotList.length() > 0) {
          int commaPos = rotList.indexOf(',');
          String firstClockface = (commaPos > 0) ? rotList.substring(0, commaPos) : rotList;
          firstClockface.trim();
          params->canvasFile = firstClockface;
          params->rotationIndex = 0;
          currentClockface = firstClockface;
          rotationMillis = millis();
          needsClockfaceReload = true;
        }
      }
    }

    // ezTime events - only process every 100ms
    static unsigned long lastEztEvents = 0;
    if (millis() - lastEztEvents >= 100) {
      ezt::events();
      lastEztEvents = millis();
    }
  }

  if (wifi.connectionSucessfulOnce)
  {
    // Check for web-triggered reload
    if (webServer->needs_reload) {
      webServer->needs_reload = false;
      webServer->rotation_changed = false;  // Clear rotation flag too
      needsClockfaceReload = false;
      Serial.println("[Web] Settings updated, applying...");

      // Check if we should be in night mode
      bool shouldBeNight = params->nightModeEnabled && isNightTime();

      if (shouldBeNight) {
        // Save the normal clockface BEFORE changing canvasFile
        currentClockface = params->canvasFile;
        // Apply night mode settings
        nightModeActive = true;
        dma_display->setBrightness8(params->nightBrightness);
        String nightClock = getNightClockface();
        if (nightClock == "_builtin") {
          // Builtin night mode always uses jsonClockface
          clockface = jsonClockface;
          clockface->setupNightMode(&cwDateTime, params->nightColor);
        } else {
          // Use selected night clockface - must set canvasFile for jsonClockface to load it
          params->canvasFile = nightClock;
          selectClockface(nightClock);
          clockface->setup(&cwDateTime, false);
        }
        Serial.printf("[Web] Night mode: using %s\n", nightClock.c_str());
      } else {
        // Not in night mode - load normal clockface
        nightModeActive = false;
        currentClockface = params->canvasFile;
        dma_display->setBrightness8(params->displayBright);
        selectClockface(currentClockface);
        clockface->setBuiltinNightMode(false);
        clockface->setup(&cwDateTime, false);
        Serial.printf("[Web] Normal mode: using %s\n", currentClockface.c_str());
      }
      Serial.println("[Web] Reload complete");
    } else {
      checkNightMode();
      checkClockfaceRotation();

      if (needsClockfaceReload) {
        needsClockfaceReload = false;
        selectClockface(params->canvasFile);
        clockface->setup(&cwDateTime, false);  // No splash on rotation
      }
    }

    clockface->update();
  }

  if (!nightModeActive) {
    automaticBrightControl();
  }

  delay(1);  // Give time to WiFi/web server
}
