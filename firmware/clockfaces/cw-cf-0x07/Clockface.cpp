
#include "Clockface.h"
#include <CWPreview.h>

unsigned long lastMillis = 0;

// Reduced size to free memory for SSL
static DynamicJsonDocument doc(16384);

Clockface::Clockface(Adafruit_GFX *display)
{
  _display = display;
  Locator::provide(display);
}

void Clockface::setup(CWDateTime *dateTime)
{
  this->_dateTime = dateTime;
  Serial.println("[Canvas] setup() called");
  Serial.flush();
  drawSplashScreen(0xFFE0, "Downloading");

  _clockfaceLoaded = false;
  if (deserializeDefinition()) {
    Serial.println("[Canvas] deserialize OK, calling clockfaceSetup");
    _clockfaceLoaded = true;
    clockfaceSetup();
  } else {
    Serial.println("[Canvas] deserialize FAILED - showing fallback");
    drawFallbackClock();
  }
}

void Clockface::drawFallbackClock()
{
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0);
  Locator::getDisplay()->setFont(&hour8pt7b);
  Locator::getDisplay()->setTextColor(0xF800); // Red
  Locator::getDisplay()->setCursor(8, 38);
  Locator::getDisplay()->print(_dateTime->getFormattedTime("H:i"));
}

void Clockface::drawSplashScreen(uint16_t color, const char *msg) {
  
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0);
  Locator::getDisplay()->drawBitmap(19, 18, CW_ICON_CANVAS, 27, 32, color);
  
  StatusController::getInstance()->printCenter("- Canvas -", 7);
  StatusController::getInstance()->printCenter(msg, 61);
}

void Clockface::update()
{
  if (!_clockfaceLoaded) {
    // Fallback: just update time every second
    if (millis() - lastMillis >= 1000) {
      drawFallbackClock();
      lastMillis = millis();
    }
    return;
  }

  // Render animation
  clockfaceLoop();

  // Update Date/Time - Using a fixed interval (1000 milliseconds)
  if (millis() - lastMillis >= 1000)
  {
    refreshDateTime();
    lastMillis = millis();
  }
}

void Clockface::setFont(const char *fontName)
{

  if (strcmp(fontName, "picopixel") == 0)
  {
    Locator::getDisplay()->setFont(&Picopixel);
  }
  else if (strcmp(fontName, "square") == 0)
  {
    Locator::getDisplay()->setFont(&atariFont);
  }
  else if (strcmp(fontName, "big") == 0)
  {
    Locator::getDisplay()->setFont(&hour8pt7b);
  }
  else if (strcmp(fontName, "medium") == 0)
  {
    Locator::getDisplay()->setFont(&minute7pt7b);
  }
  else if (strcmp(fontName, "tomthumb") == 0)
  {
    Locator::getDisplay()->setFont(&TomThumb);
  }
  else if (strcmp(fontName, "bold") == 0)
  {
    Locator::getDisplay()->setFont(&FreeSansBold9pt7b);
  }
  else
  {
    Locator::getDisplay()->setFont();
  }
}

uint16_t Clockface::resolveColor(int32_t color)
{
  if (color == -1) {
    return ClockwiseParams::getInstance()->nightColor;
  }
  return (uint16_t)color;
}

void Clockface::renderText(String text, JsonVariantConst value)
{
  int16_t x1, y1;
  uint16_t w, h;

  setFont(value["font"].as<const char *>());

  Locator::getDisplay()->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  // Use -1 as special value for dynamic night color
  uint16_t fgColor = resolveColor(value["fgColor"].as<int32_t>());
  uint16_t bgColor = resolveColor(value["bgColor"].as<int32_t>());

  // Calculate max width using "8" as reference (widest digit)
  // This ensures the background clears the full area when digits change
  int16_t ref_x1, ref_y1;
  uint16_t ref_w, ref_h;
  String refText = text;
  for (int i = 0; i < refText.length(); i++) {
    if (refText[i] >= '0' && refText[i] <= '9') refText[i] = '8';
  }
  Locator::getDisplay()->getTextBounds(refText, 0, 0, &ref_x1, &ref_y1, &ref_w, &ref_h);

  // Use the reference dimensions and position for clearing
  // This covers the maximum possible area regardless of actual text content
  int16_t clearX = min(x1, ref_x1);
  int16_t clearY = min(y1, ref_y1);
  uint16_t clearW = max(w, ref_w) + abs(x1 - ref_x1);
  uint16_t clearH = max(h, ref_h) + abs(y1 - ref_y1);

  // BG Color - clear area based on widest possible text
  Locator::getDisplay()->fillRect(
      value["x"].as<const uint16_t>() + clearX,
      value["y"].as<const uint16_t>() + clearY,
      clearW,
      clearH,
      bgColor);

  Locator::getDisplay()->setTextColor(fgColor);
  Locator::getDisplay()->setCursor(value["x"].as<const uint16_t>(), value["y"].as<const uint16_t>());
  Locator::getDisplay()->print(text);
}

void Clockface::refreshDateTime()
{
  // Check both "setup" and "loop" arrays for datetime elements
  JsonArrayConst setupElements = doc["setup"].as<JsonArrayConst>();
  for (JsonVariantConst value : setupElements)
  {
    const char *type = value["type"].as<const char *>();
    if (strcmp(type, "datetime") == 0)
    {
      renderText(_dateTime->getFormattedTime(value["content"].as<const char *>()), value);
    }
  }

  JsonArrayConst loopElements = doc["loop"].as<JsonArrayConst>();
  for (JsonVariantConst value : loopElements)
  {
    const char *type = value["type"].as<const char *>();
    if (strcmp(type, "datetime") == 0)
    {
      renderText(_dateTime->getFormattedTime(value["content"].as<const char *>()), value);
    }
  }
}

void Clockface::clockfaceSetup()
{

  // Clear screen
  Locator::getDisplay()->fillRect(0, 0, 64, 64, doc["bgColor"].as<const uint16_t>());

  delay = doc["delay"].as<const uint16_t>();

  // Draw static elements
  renderElements(doc["setup"].as<JsonArrayConst>());

  // Draw Date/Time
  refreshDateTime();

  // Create sprites
  createSprites();
}

void Clockface::createSprites()
{
  // Clear existing sprites to prevent memory leak on reload
  sprites.clear();

  JsonArrayConst elements = doc["loop"].as<JsonArrayConst>();
  uint8_t width = 0;
  uint8_t height = 0;
  uint16_t defaultDelay = delay;

  for (JsonVariantConst value : elements)
  {
    const char *type = value["type"].as<const char *>();

    if (strcmp(type, "sprite") == 0)
    {
      uint8_t ref = value["sprite"].as<const uint8_t>();
      int8_t x = value["x"].as<const int8_t>();
      int8_t y = value["y"].as<const int8_t>();

      std::shared_ptr<CustomSprite> s = std::make_shared<CustomSprite>(x, y);

      getImageDimensions(doc["sprites"][ref][0]["image"].as<const char *>(), width, height);

      s.get()->_spriteReference = ref;
      s.get()->_totalFrames = doc["sprites"][ref].size();
      s.get()->setDimensions(width, height);

      // Cache JSON values to avoid repeated parsing
      s.get()->_cachedLoopDelay = value["loopDelay"].as<uint32_t>() ?: defaultDelay;
      s.get()->_cachedFrameDelay = value["frameDelay"].as<uint16_t>() ?: defaultDelay;
      s.get()->_cachedMoveStartTime = value["moveStartTime"].as<uint32_t>() ?: 1;
      s.get()->_cachedMoveDuration = value["moveDuration"].as<uint32_t>() ?: 0;
      s.get()->_cachedMoveTargetX = value["moveTargetX"].as<int8_t>() ?: -1;
      s.get()->_cachedMoveTargetY = value["moveTargetY"].as<int8_t>() ?: -1;
      s.get()->_cachedOriginX = x;
      s.get()->_cachedOriginY = y;
      s.get()->_cachedShouldReturn = value["shouldReturnToOrigin"].as<bool>() ?: false;

      sprites.push_back(s);
    }
  }
}

void Clockface::handleSpriteAnimation(std::shared_ptr<CustomSprite>& sprite) {
    uint8_t totalFrames = sprite->_totalFrames;
    uint32_t loopDelay = sprite->_cachedLoopDelay;
    uint16_t frameDelay = sprite->_cachedFrameDelay;

    if (millis() - sprite->_lastMillisSpriteFrames >= frameDelay && sprite->_currentFrameCount < totalFrames) {
        sprite->incFrame();

        handleSpriteMovement(sprite);

        renderImage(doc["sprites"][sprite->_spriteReference][sprite->_currentFrame]["image"].as<const char *>(), sprite->getX(), sprite->getY());

        sprite->_currentFrameCount += 1;
        sprite->_lastMillisSpriteFrames = millis();
    }

    if (millis() - sprite->_lastResetTime >= loopDelay) {
        unsigned long currentMillis = millis();
        unsigned long currentSecond = _dateTime->getSecond();

        if ((currentSecond * 1000) % loopDelay == 0) {
            sprite->_currentFrameCount = 0;
            sprite->_lastResetTime = currentMillis;
        }
    }
}

void Clockface::handleSpriteMovement(std::shared_ptr<CustomSprite>& sprite) {
    unsigned long moveStartTime = sprite->_cachedMoveStartTime;
    unsigned long moveDuration = sprite->_cachedMoveDuration;
    int8_t moveInitialX = sprite->_cachedOriginX;
    int8_t moveInitialY = sprite->_cachedOriginY;
    int8_t moveTargetX = sprite->_cachedMoveTargetX;
    int8_t moveTargetY = sprite->_cachedMoveTargetY;
    bool shouldReturnToOrigin = sprite->_cachedShouldReturn;

    // Check if the sprite is moving
    if (sprite->isMoving()) {
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - sprite->_moveStartTime;
        float progress = (static_cast<float>(elapsedTime) / sprite->_moveDuration);

        int8_t oldX = sprite->getX();
        int8_t oldY = sprite->getY();
        int8_t newX = sprite->lerp(sprite->_moveInitialX, sprite->_moveTargetX, progress);
        int8_t newY = sprite->lerp(sprite->_moveInitialY, sprite->_moveTargetY, progress);
        int8_t originX = min(oldX, newX);
        int8_t originY = min(oldY, newY);
        int8_t drawWidth = sprite->getWidth() + max(oldX, newX) - originX;
        int8_t drawHeight = sprite->getHeight() + max(oldY, newY) - originY;

        // Erase the previous position
        Locator::getDisplay()->fillRect(
            originX,
            originY,
            drawWidth,
            drawHeight,
            doc["bgColor"].as<const uint16_t>());

        if (progress <= 1) {
            // Update the sprite's position
            sprite->setX(newX);
            sprite->setY(newY);

        } else if (sprite->shouldReturnToOrigin()) {
            // Movement is complete
            sprite->setX(sprite->_moveTargetX);
            sprite->setY(sprite->_moveTargetY);

            if (!sprite->_isReversing) {
                sprite->reverseMoving(moveInitialX, moveInitialY);
            }
        } else {
            sprite->stopMoving();
        }
    }

    if ((moveDuration > 0 && (moveTargetX > -1 || moveTargetY > -1)) && (millis() - sprite->_lastResetMoveTime >= moveStartTime)) {
        unsigned long currentMillis = millis();
        unsigned long currentSecond = _dateTime->getSecond();

        if ((currentSecond * 1000) % moveStartTime == 0) {
            sprite->_lastResetMoveTime = currentMillis;
            sprite->startMoving(moveTargetX, moveTargetY, moveDuration, shouldReturnToOrigin);
        }
    }
}

void Clockface::clockfaceLoop() {
    if (sprites.empty()) {
        return;
    }

    for (auto& sprite : sprites) {
        handleSpriteAnimation(sprite);
    }
}

void Clockface::renderElements(JsonArrayConst elements)
{
  for (JsonVariantConst value : elements)
  {
    const char *type = value["type"].as<const char *>();

    if (strcmp(type, "text") == 0)
    {
      renderText(value["content"].as<const char *>(), value);
    }
    else if (strcmp(type, "fillrect") == 0)
    {
      Locator::getDisplay()->fillRect(
          value["x"].as<const uint16_t>(),
          value["y"].as<const uint16_t>(),
          value["width"].as<const uint16_t>(),
          value["height"].as<const uint16_t>(),
          value["color"].as<const uint16_t>());
    }
    else if (strcmp(type, "rect") == 0)
    {
      Locator::getDisplay()->drawRect(
          value["x"].as<const uint16_t>(),
          value["y"].as<const uint16_t>(),
          value["width"].as<const uint16_t>(),
          value["height"].as<const uint16_t>(),
          value["color"].as<const uint16_t>());
    }
    else if (strcmp(type, "line") == 0)
    {
      Locator::getDisplay()->drawLine(
          value["x"].as<const uint16_t>(),
          value["y"].as<const uint16_t>(),
          value["x1"].as<const uint16_t>(),
          value["y1"].as<const uint16_t>(),
          value["color"].as<const uint16_t>());
    }
    else if (strcmp(type, "image") == 0)
    {
      renderImage(value["image"].as<const char *>(), value["x"].as<const uint8_t>(), value["y"].as<const uint8_t>());
    }
  }
}

bool Clockface::deserializeDefinition()
{
  // Check for preview mode first (temporary, from editor)
  // Preview takes priority and is cleared on restart
  if (CWPreview::getInstance()->hasPreview) {
    Serial.println("[Canvas] Loading from preview JSON (temporary)");
    doc.clear();
    DeserializationError error = deserializeJson(doc, CWPreview::getInstance()->jsonData);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      CWPreview::getInstance()->clearPreview();
      drawSplashScreen(0xC904, "Preview error");
      return false;
    }
    Serial.printf("[Canvas] Preview clockface '%s' loaded\n", doc["name"].as<const char *>());
    return true;
  }

  if (ClockwiseParams::getInstance()->canvasServer.isEmpty() || ClockwiseParams::getInstance()->canvasFile.isEmpty()) {
    drawSplashScreen(0xC904, "Params werent set");
    return false;
  }

  String server = ClockwiseParams::getInstance()->canvasServer;
  String file = String("/" + ClockwiseParams::getInstance()->canvasFile + ".json");
  uint16_t port = 443;
  bool useSSL = true;

  // Handle clockface source selection
  String source = ClockwiseParams::getInstance()->clockfaceSource;
  if (source == "local") {
    // Use local development server (HTTP, no SSL)
    server = ClockwiseParams::getInstance()->localServerHost;
    port = ClockwiseParams::getInstance()->localServerPort;
    useSSL = false;
  } else if (server.startsWith("raw.")) {
    if (source == "github") {
      // Use GitHub directly (may fail on some ESP32)
      file = String("/jnthas/clock-club/main/shared" + file);
    } else {
      // Use XE1E CDN (default, more compatible)
      server = "cdn.itaqui.to";
      file = String("/xe1e/clockfaces" + file);
    }
  }

  if (server.indexOf(":") > 0) {
    int colonPos = server.indexOf(":");
    port = server.substring(colonPos + 1).toInt();
    server = server.substring(0, colonPos);
    useSSL = (port == 443 || port == 4443);
  }

  Serial.printf("[Canvas] Downloading from %s%s:%d (SSL:%d)\n", server.c_str(), file.c_str(), port, useSSL);

  doc.clear();
  DeserializationError error;

  if (useSSL) {
    WiFiClientSecure client;
    ClockwiseHttpClient::getInstance()->httpGet(&client, server.c_str(), file.c_str(), port);
    error = deserializeJson(doc, client);
    client.stop();
  } else {
    WiFiClient client;
    ClockwiseHttpClient::getInstance()->httpGetPlain(&client, server.c_str(), file.c_str(), port);
    error = deserializeJson(doc, client);
    client.stop();
  }

  if (error) {
    drawSplashScreen(0xC904, "Error! Check logs");
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return false;
  }

  Serial.printf("[Canvas] Building clockface '%s' by %s, version %d\n", doc["name"].as<const char *>(), doc["author"].as<const char *>(), doc["version"].as<const uint16_t>());
  return true;
}
