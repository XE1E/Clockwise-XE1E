
#include "Clockface.h"

unsigned long lastMillis = 0;

// TODO document size
static DynamicJsonDocument doc(32768);

Clockface::Clockface(Adafruit_GFX *display)
{
  _display = display;
  Locator::provide(display);
}

void Clockface::setup(CWDateTime *dateTime)
{
  this->_dateTime = dateTime;
  drawSplashScreen(0xFFE0, "Downloading");

  if (deserializeDefinition()) {
    clockfaceSetup();
  }
}

void Clockface::drawSplashScreen(uint16_t color, const char *msg) {
  
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0);
  Locator::getDisplay()->drawBitmap(19, 18, CW_ICON_CANVAS, 27, 32, color);
  
  StatusController::getInstance()->printCenter("- Canvas -", 7);
  StatusController::getInstance()->printCenter(msg, 61);
}

void Clockface::update()
{
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

void Clockface::renderText(String text, JsonVariantConst value)
{
  int16_t x1, y1;
  uint16_t w, h;

  setFont(value["font"].as<const char *>());

  Locator::getDisplay()->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  // BG Color
  Locator::getDisplay()->fillRect(
      value["x"].as<const uint16_t>() + x1,
      value["y"].as<const uint16_t>() + y1,
      w,
      h,
      value["bgColor"].as<const uint16_t>());

  Locator::getDisplay()->setTextColor(value["fgColor"].as<const uint16_t>());
  Locator::getDisplay()->setCursor(value["x"].as<const uint16_t>(), value["y"].as<const uint16_t>());
  Locator::getDisplay()->print(text);
}

void Clockface::refreshDateTime()
{

  JsonArrayConst elements = doc["setup"].as<JsonArrayConst>();
  for (JsonVariantConst value : elements)
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
  JsonArrayConst elements = doc["loop"].as<JsonArrayConst>();
  uint8_t width = 0;
  uint8_t height = 0;

  for (JsonVariantConst value : elements)
  {
    const char *type = value["type"].as<const char *>();

    if (strcmp(type, "sprite") == 0)
    {
      uint8_t ref = value["sprite"].as<const uint8_t>();

      std::shared_ptr<CustomSprite> s = std::make_shared<CustomSprite>(value["x"].as<const int8_t>(), value["y"].as<const int8_t>());

      getImageDimensions(doc["sprites"][ref][0]["image"].as<const char *>(), width, height);

      s.get()->_spriteReference = value["sprite"].as<const uint8_t>();
      s.get()->_totalFrames = doc["sprites"][ref].size();
      s.get()->setDimensions(width, height);
      sprites.push_back(s);
    }
  }
}

void Clockface::handleSpriteAnimation(std::shared_ptr<CustomSprite>& sprite) {
    uint8_t totalFrames = sprite->_totalFrames;
    uint32_t loopDelay = doc["loop"][sprite->_spriteReference]["loopDelay"].as<uint32_t>() ?: delay;
    uint16_t frameDelay = doc["loop"][sprite->_spriteReference]["frameDelay"].as<uint16_t>() ?: delay;

    if (millis() - sprite->_lastMillisSpriteFrames >= frameDelay && sprite->_currentFrameCount < totalFrames) {
        sprite->incFrame();

        // handle sprite movement
        handleSpriteMovement(sprite);

        // Render the frame of the sprite
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
    unsigned long moveStartTime = doc["loop"][sprite->_spriteReference]["moveStartTime"].as<unsigned long>() ?: 1;
    unsigned long moveDuration = doc["loop"][sprite->_spriteReference]["moveDuration"].as<unsigned long>() ?: 0;
    int8_t moveInitialX = doc["loop"][sprite->_spriteReference]["x"].as<int8_t>() ?:0;
    int8_t moveInitialY = doc["loop"][sprite->_spriteReference]["y"].as<int8_t>() ?: 0;
    int8_t moveTargetX = doc["loop"][sprite->_spriteReference]["moveTargetX"].as<int8_t>() ?: -1;
    int8_t moveTargetY = doc["loop"][sprite->_spriteReference]["moveTargetY"].as<int8_t>() ?: -1;
    bool shouldReturnToOrigin = doc["loop"][sprite->_spriteReference]["shouldReturnToOrigin"].as<bool>() ?: false;

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
  WiFiClientSecure client;

  //WiFiClient client;
  //ClockwiseHttpClient::getInstance()->httpGet(&client, "raw.githubusercontent.com", "/jnthas/clock-club/v1/pac-man.json", 443);
  //ClockwiseHttpClient::getInstance()->httpGet(&client, "192.168.3.19", "/nyan-cat.json", 4443);

  if (ClockwiseParams::getInstance()->canvasServer.isEmpty() || ClockwiseParams::getInstance()->canvasFile.isEmpty()) {
    drawSplashScreen(0xC904, "Params werent set");
    return false;
  }


  String server = ClockwiseParams::getInstance()->canvasServer;
  String file = String("/" + ClockwiseParams::getInstance()->canvasFile + ".json");
  uint16_t port = 4443;

  if (server.startsWith("raw.")) {
    port = 443;
    file = String("/jnthas/clock-club/main/shared" + file);
  }

  ClockwiseHttpClient::getInstance()->httpGet(&client, server.c_str(), file.c_str(), port);
  
  DeserializationError error = deserializeJson(doc, client);
  if (error)
  {
    drawSplashScreen(0xC904, "Error! Check logs");

    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    client.stop();
    return false;
  }

  //TODO check if json is valid

  Serial.printf("[Canvas] Building clockface '%s' by %s, version %d\n", doc["name"].as<const char *>(), doc["author"].as<const char *>(), doc["version"].as<const uint16_t>());
  client.stop();
  return true;
}
