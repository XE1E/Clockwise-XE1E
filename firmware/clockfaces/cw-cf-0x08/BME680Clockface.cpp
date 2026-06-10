#include "BME680Clockface.h"

// Colores (RGB565)
static const uint16_t COLOR_BG      = 0x0000;  // negro
static const uint16_t COLOR_TIME    = 0xFFFF;  // blanco
static const uint16_t COLOR_SEP     = 0x2104;  // gris oscuro
static const uint16_t COLOR_VALUE   = 0xFFFF;  // blanco
static const uint16_t COLOR_TEMP    = 0xFD20;  // naranja
static const uint16_t COLOR_HUM     = 0x07FF;  // cyan
static const uint16_t COLOR_PRES    = 0x07E0;  // verde
static const uint16_t COLOR_GAS     = 0xFFE0;  // amarillo
static const uint16_t COLOR_ERROR   = 0xF800;  // rojo

// Posiciones verticales de las 4 filas de datos
static const int ROW_TEMP = 17;
static const int ROW_HUM  = 28;
static const int ROW_PRES = 39;
static const int ROW_GAS  = 50;

BME680Clockface::BME680Clockface(Adafruit_GFX* display) {
  _display = display;
}

void BME680Clockface::setup(CWDateTime* dateTime, bool showSplash) {
  _dateTime = dateTime;

  _display->setFont(NULL);  // fuente por defecto 6x8
  _display->setTextSize(1);
  _display->setTextWrap(false);

  _sensor.begin();

  _lastMinute = -1;
  _hadData = false;
  _errorShown = false;

  drawStaticLayout();
  drawTime();

  // Forzar primera lectura/dibujo si el sensor responde
  if (_sensor.isReady()) {
    _sensor.update();
    drawValues();
  } else {
    drawError();
  }
}

void BME680Clockface::update() {
  // Hora: redibujar cuando cambia el minuto
  int currentMinute = _dateTime->getMinute();
  if (currentMinute != _lastMinute) {
    _lastMinute = currentMinute;
    drawTime();
  }

  // Sensor
  bool hasNewData = _sensor.update();

  if (_sensor.isReady()) {
    if (_errorShown) {
      // El sensor se recupero: limpiar mensaje y reconstruir layout
      _errorShown = false;
      drawStaticLayout();
      drawTime();
    }
    if (hasNewData || !_hadData) {
      _hadData = true;
      drawValues();
    }
  } else if (!_errorShown) {
    drawError();
  }
}

void BME680Clockface::drawStaticLayout() {
  _display->fillRect(0, 0, 64, 64, COLOR_BG);
  _display->drawFastHLine(0, 13, 64, COLOR_SEP);
}

void BME680Clockface::drawTime() {
  // Limpiar banda superior
  _display->fillRect(0, 0, 64, 12, COLOR_BG);

  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", _dateTime->getHour(), _dateTime->getMinute());

  // Centrar (5 caracteres * 6 px = 30 px)
  int x = (64 - 30) / 2;
  _display->setFont(NULL);
  _display->setTextSize(1);
  _display->setTextColor(COLOR_TIME, COLOR_BG);
  _display->setCursor(x, 3);
  _display->print(buf);
}

// Dibuja una fila: letra etiqueta (en color) + valor (en blanco)
static void drawRow(Adafruit_GFX* d, int y, char label, uint16_t labelColor, const String& value) {
  d->fillRect(0, y, 64, 9, COLOR_BG);

  d->setTextColor(labelColor, COLOR_BG);
  d->setCursor(2, y);
  d->print(label);

  d->setTextColor(COLOR_VALUE, COLOR_BG);
  d->setCursor(12, y);
  d->print(value);
}

void BME680Clockface::drawValues() {
  char buf[12];
  _display->setFont(NULL);
  _display->setTextSize(1);

  if (isnan(_sensor.temperature)) {
    drawRow(_display, ROW_TEMP, 'T', COLOR_TEMP, "--");
  } else {
    snprintf(buf, sizeof(buf), "%.1fC", _sensor.temperature);
    drawRow(_display, ROW_TEMP, 'T', COLOR_TEMP, buf);
  }

  if (isnan(_sensor.humidity)) {
    drawRow(_display, ROW_HUM, 'H', COLOR_HUM, "--");
  } else {
    snprintf(buf, sizeof(buf), "%.0f%%", _sensor.humidity);
    drawRow(_display, ROW_HUM, 'H', COLOR_HUM, buf);
  }

  if (isnan(_sensor.pressure)) {
    drawRow(_display, ROW_PRES, 'P', COLOR_PRES, "--");
  } else {
    snprintf(buf, sizeof(buf), "%.0fhP", _sensor.pressure);
    drawRow(_display, ROW_PRES, 'P', COLOR_PRES, buf);
  }

  if (isnan(_sensor.gasResistance)) {
    drawRow(_display, ROW_GAS, 'G', COLOR_GAS, "--");
  } else {
    snprintf(buf, sizeof(buf), "%.0fk", _sensor.gasResistance);
    drawRow(_display, ROW_GAS, 'G', COLOR_GAS, buf);
  }
}

void BME680Clockface::drawError() {
  _errorShown = true;
  _display->fillRect(0, 14, 64, 50, COLOR_BG);
  _display->setFont(NULL);
  _display->setTextSize(1);
  _display->setTextColor(COLOR_ERROR, COLOR_BG);
  _display->setCursor(2, 28);
  _display->print("BME680");
  _display->setCursor(2, 38);
  _display->print("no det.");
}
