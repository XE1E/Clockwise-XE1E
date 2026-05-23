
#include "MarioClockface.h"

EventBus eventBus;

const char* FORMAT_TWO_DIGITS = "%02d";

// LED caminante - color mas brillante que el cielo
const unsigned short LED_COLOR = 0x07FF;  // Cyan brillante
const uint8_t LED_START = 2;   // Posicion inicial del LED
const uint8_t LED_END = 61;    // Posicion final del LED

// Graphical elements
Tile ground(GROUND, 8, 8);

Object bush(BUSH, 21, 9);
Object cloud1(CLOUD1, 13, 12);
Object cloud2(CLOUD2, 13, 12);
Object hill(HILL, 20, 22);

Mario mario(23, 40);
Block hourBlock(13, 8);
Block minuteBlock(32, 8);

unsigned long lastMillis = 0;
unsigned long cloudAnimMillis = 0;
int lastSecond = -1;
int8_t cloudAnimOffset = 0;
int8_t cloudAnimDir = 1;  // 1 = up, -1 = down
const uint16_t CLOUD_HIGHLIGHT = 0x3DFF;  // Light blue highlight

MarioClockface::MarioClockface(Adafruit_GFX* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void MarioClockface::setup(CWDateTime *dateTime, bool showSplash) {
  _dateTime = dateTime;

  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, SKY_COLOR);

  ground.fillRow(DISPLAY_HEIGHT - ground._height);

  bush.draw(43, 47);
  hill.draw(0, 34);
  cloud1.draw(0, 21);
  cloud2.draw(51, 7);

  // Draw pipe in bottom right corner
  Locator::getDisplay()->drawRGBBitmap(54, 40, PIPE, PIPE_SIZE[0], PIPE_SIZE[1]);

  updateTime();

  hourBlock.init();
  minuteBlock.init();
  mario.init();

  // Inicializar LED caminante con el segundo actual
  lastSecond = _dateTime->getSecond();
  if (lastSecond > 0 && lastSecond < 60) {
    int pos = LED_START + lastSecond;
    if (pos <= LED_END) {
      Locator::getDisplay()->drawPixel(pos, 0, LED_COLOR);
    }
  }
}

void MarioClockface::update() {
  hourBlock.update();
  minuteBlock.update();
  mario.update();

  int currentSecond = _dateTime->getSecond();

  // Animacion de lineas azules en nubes (cada 800ms, mas lento)
  if (millis() - cloudAnimMillis >= 800) {
    // Borrar highlight anterior en cloud1 - usar blanco (color de nube)
    Locator::getDisplay()->drawPixel(4, 24 + cloudAnimOffset, 0xFFFF);
    Locator::getDisplay()->drawPixel(5, 24 + cloudAnimOffset, 0xFFFF);
    // Borrar highlight anterior en cloud2 - usar blanco (color de nube)
    Locator::getDisplay()->drawPixel(55, 10 + cloudAnimOffset, 0xFFFF);
    Locator::getDisplay()->drawPixel(56, 10 + cloudAnimOffset, 0xFFFF);

    // Mover offset
    cloudAnimOffset += cloudAnimDir;
    if (cloudAnimOffset >= 3) cloudAnimDir = -1;
    if (cloudAnimOffset <= 0) cloudAnimDir = 1;

    // Dibujar nuevo highlight en cloud1
    Locator::getDisplay()->drawPixel(4, 24 + cloudAnimOffset, CLOUD_HIGHLIGHT);
    Locator::getDisplay()->drawPixel(5, 24 + cloudAnimOffset, CLOUD_HIGHLIGHT);
    // Dibujar nuevo highlight en cloud2
    Locator::getDisplay()->drawPixel(55, 10 + cloudAnimOffset, CLOUD_HIGHLIGHT);
    Locator::getDisplay()->drawPixel(56, 10 + cloudAnimOffset, CLOUD_HIGHLIGHT);

    cloudAnimMillis = millis();
  }

  // LED caminante en la fila superior
  if (currentSecond != lastSecond) {
    // Borrar LED anterior
    if (lastSecond >= 0 && lastSecond < 60) {
      int lastPos = LED_START + lastSecond;
      if (lastPos <= LED_END) {
        Locator::getDisplay()->drawPixel(lastPos, 0, SKY_COLOR);
      }
    }

    // Dibujar nuevo LED (excepto en segundo 0 que es el salto)
    if (currentSecond > 0 && currentSecond < 60) {
      int newPos = LED_START + currentSecond;
      if (newPos <= LED_END) {
        Locator::getDisplay()->drawPixel(newPos, 0, LED_COLOR);
      }
    }

    // Segundo 0: Mario salta y actualiza la hora
    if (currentSecond == 0 && lastSecond == 59) {
      // Borrar toda la fila de LEDs
      Locator::getDisplay()->drawFastHLine(LED_START, 0, LED_END - LED_START + 1, SKY_COLOR);
      mario.jump();
      updateTime();
    }

    lastSecond = currentSecond;
    lastMillis = millis();
  }
}

void MarioClockface::updateTime() {
  hourBlock.setText(String(_dateTime->getHour()));
  minuteBlock.setText(String(_dateTime->getMinute(FORMAT_TWO_DIGITS)));
}

void MarioClockface::externalEvent(int type) {
  if (type == 0) {  //TODO create an enum
    mario.jump();
    updateTime();
  }
}
