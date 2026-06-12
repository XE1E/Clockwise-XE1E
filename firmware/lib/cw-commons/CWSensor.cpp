#include "CWSensor.h"
#include <Wire.h>
#include <Adafruit_BME280.h>

CWSensor *CWSensor::getInstance()
{
  static CWSensor base;
  return &base;
}

bool CWSensor::begin(uint8_t sdaPin, uint8_t sclPin, uint8_t addr)
{
  _sda = sdaPin;
  _scl = sclPin;
  _available = false;
  _firstRead = true;

  if (_bme == nullptr) {
    _bme = new Adafruit_BME280();
  }

  Wire.begin(sdaPin, sclPin);

  if (addr == 0) {
    // Autodeteccion de direccion I2C (0x76 o 0x77)
    if (_bme->begin(0x76, &Wire)) {
      _addr = 0x76;
      _available = true;
    } else if (_bme->begin(0x77, &Wire)) {
      _addr = 0x77;
      _available = true;
    }
  } else {
    _available = _bme->begin(addr, &Wire);
    _addr = addr;
  }

  if (_available) {
    // Modo forzado: el sensor duerme entre lecturas, asi se evita el
    // autocalentamiento que inflaria la temperatura.
    _bme->setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1,   // temperatura
                      Adafruit_BME280::SAMPLING_X1,   // presion
                      Adafruit_BME280::SAMPLING_X1,   // humedad
                      Adafruit_BME280::FILTER_OFF);
    Serial.printf("[Sensor] BME280 detectado en 0x%02X (SDA=%d SCL=%d)\n", _addr, _sda, _scl);
    update();
  } else {
    Serial.printf("[Sensor] BME280 NO detectado (SDA=%d SCL=%d)\n", _sda, _scl);
  }

  return _available;
}

void CWSensor::update()
{
  if (!_available) return;

  unsigned long now = millis();
  if (!_firstRead && (now - _lastRead) < _readInterval) return;
  _firstRead = false;
  _lastRead = now;

  _bme->takeForcedMeasurement();
  _temperature = _bme->readTemperature();     // grados C
  _humidity = _bme->readHumidity();            // %
  _pressure = _bme->readPressure() / 100.0F;   // Pa -> hPa
}
