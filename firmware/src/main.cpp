#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Clockface
#include <Clockface.h>
// Commons
#include <WiFiController.h>
#include <CWDateTime.h>
#include <CWPreferences.h>
#include <CWWebServer.h>
#include <StatusController.h>

#define MIN_BRIGHT_DISPLAY_ON 4
#define MIN_BRIGHT_DISPLAY_OFF 0

#define ESP32_LED_BUILTIN 2

MatrixPanel_I2S_DMA *dma_display = nullptr;

Clockface *clockface;

WiFiController wifi;
CWDateTime cwDateTime;

bool autoBrightEnabled;
long autoBrightMillis = 0;
uint8_t currentBrightSlot = -1;

bool nightModeActive = false;
long rotationMillis = 0;
bool needsClockfaceReload = false;
String currentClockface = "";

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

void checkNightMode() {
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

    String nightClock = ClockwiseParams::getInstance()->nightClockface;
    if (nightClock == "_builtin") {
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

void checkClockfaceRotation() {
  if (!ClockwiseParams::getInstance()->rotationEnabled || nightModeActive) return;

  String rotList = ClockwiseParams::getInstance()->rotationList;
  if (rotList.length() == 0) return;

  unsigned long intervalMs = ClockwiseParams::getInstance()->rotationInterval * 60000UL;

  if (millis() - rotationMillis >= intervalMs) {
    int count = 1;
    for (int i = 0; i < rotList.length(); i++) {
      if (rotList[i] == ',') count++;
    }

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

  if (swapBlueGreen)
  {
    // Swap Blue and Green pins because the panel is RBG instead of RGB.
    mxconfig.gpio.b1 = 26;
    mxconfig.gpio.b2 = 12;
    mxconfig.gpio.g1 = 27;
    mxconfig.gpio.g2 = 13;
  }

  mxconfig.gpio.e = 18;
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

      uint8_t slots = 10; //10 slots
      uint8_t mapLDR = map(currentValue > ldrMax ? ldrMax : currentValue, ldrMin, ldrMax, 1, slots);
      uint8_t mapBright = map(mapLDR, 1, slots, minBright, maxBright);

      // Serial.printf("LDR: %d, mapLDR: %d, Bright: %d\n", currentValue, mapLDR, mapBright);
      if(abs(currentBrightSlot - mapLDR ) >= 2 || mapBright == 0){
           dma_display->setBrightness8(mapBright);
           currentBrightSlot=mapLDR;
          //  Serial.printf("setBrightness: %d , Update currentBrightSlot to %d\n", mapBright, mapLDR);
      }
      autoBrightMillis = millis();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ESP32_LED_BUILTIN, OUTPUT);

  StatusController::getInstance()->blink_led(5, 100);

  ClockwiseParams::getInstance()->load();

  pinMode(ClockwiseParams::getInstance()->ldrPin, INPUT);

  displaySetup(ClockwiseParams::getInstance()->swapBlueGreen, ClockwiseParams::getInstance()->displayBright, ClockwiseParams::getInstance()->displayRotation);
  clockface = new Clockface(dma_display);

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
    clockface->setup(&cwDateTime);
  }
}

void loop()
{
  wifi.handleImprovWiFi();
  wifi.checkReconnect();

  if (wifi.isConnected())
  {
    ClockwiseWebServer::getInstance()->handleRestart();

    // Apply brightness change from web UI immediately
    if (ClockwiseWebServer::getInstance()->needs_brightness_update && !nightModeActive) {
      ClockwiseWebServer::getInstance()->needs_brightness_update = false;
      dma_display->setBrightness8(ClockwiseWebServer::getInstance()->pending_brightness);
    }

    // Handle rotation enable/disable
    if (ClockwiseWebServer::getInstance()->rotation_changed) {
      ClockwiseWebServer::getInstance()->rotation_changed = false;
      if (ClockwiseParams::getInstance()->rotationEnabled) {
        String rotList = ClockwiseParams::getInstance()->rotationList;
        if (rotList.length() > 0) {
          int commaPos = rotList.indexOf(',');
          String firstClockface = (commaPos > 0) ? rotList.substring(0, commaPos) : rotList;
          firstClockface.trim();
          ClockwiseParams::getInstance()->canvasFile = firstClockface;
          ClockwiseParams::getInstance()->rotationIndex = 0;
          currentClockface = firstClockface;
          rotationMillis = millis();
          needsClockfaceReload = true;
        }
      }
    }

    ezt::events();
  }

  if (wifi.connectionSucessfulOnce)
  {
    // Check for web-triggered reload FIRST, skip night/rotation checks
    if (ClockwiseWebServer::getInstance()->needs_reload) {
      ClockwiseWebServer::getInstance()->needs_reload = false;
      currentClockface = ClockwiseParams::getInstance()->canvasFile;
      Serial.printf("[Web] Reloading clockface: %s\n", currentClockface.c_str());
      Serial.flush();
      clockface->setup(&cwDateTime);
      Serial.println("[Web] Reload complete");
    } else {
      checkNightMode();
      checkClockfaceRotation();

      if (needsClockfaceReload) {
        needsClockfaceReload = false;
        clockface->setup(&cwDateTime);
      }
    }

    clockface->update();
  }

  if (!nightModeActive) {
    automaticBrightControl();
  }
}
