#pragma once
#include <Arduino.h>

// Sensor ambiental BME280 (temperatura / humedad / presion).
// Mismo codigo para ESP32 clasico y ESP32-S3; solo cambian los pines I2C
// por defecto (ver CW_SENSOR_SDA_DEFAULT / CW_SENSOR_SCL_DEFAULT).
//
// Singleton: main.cpp llama begin()/update(); el web server y las caratulas
// consultan los getters. getIaq() queda como stub para un futuro upgrade a
// BME680 + BSEC2 (ver docs/PLAN-SENSOR-AMBIENTAL.md).

class Adafruit_BME280;  // fwd-decl: el include pesado vive en el .cpp

class CWSensor
{
public:
  static CWSensor *getInstance();

  // Inicializa el bus I2C y detecta el sensor. addr=0 -> autodeteccion 0x76/0x77.
  // Devuelve true si el sensor responde.
  bool begin(uint8_t sdaPin, uint8_t sclPin, uint8_t addr = 0);

  // Relee el sensor si ya paso el intervalo. Barato de llamar en cada loop().
  void update();

  bool available() const { return _available; }
  uint8_t getAddress() const { return _addr; }

  float getTemperature() const { return _temperature; }  // grados C
  float getHumidity() const { return _humidity; }         // %
  float getPressure() const { return _pressure; }         // hPa
  float getIaq() const { return NAN; }                    // stub (futuro BME680+BSEC)

private:
  CWSensor() {}

  Adafruit_BME280 *_bme = nullptr;
  bool _available = false;
  bool _firstRead = true;
  uint8_t _addr = 0;
  uint8_t _sda = 0;
  uint8_t _scl = 0;

  float _temperature = NAN;
  float _humidity = NAN;
  float _pressure = NAN;

  unsigned long _lastRead = 0;
  unsigned long _readInterval = 30000;  // ms entre lecturas
};
