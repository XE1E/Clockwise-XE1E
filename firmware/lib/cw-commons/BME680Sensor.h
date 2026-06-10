#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME680.h>

// Pines I2C segun la tarjeta. En el ESP32-S3 el LDR ocupa GPIO 1 y HUB75
// ocupa 2-8,15,16,18,38,40-42, dejando 9/10 libres para el bus I2C.
// Ver docs/ESP32-S3-MIGRATION.md (esquematico de conexion BME680).
#ifdef CONFIG_IDF_TARGET_ESP32S3
  #define BME680_SDA_PIN 9
  #define BME680_SCL_PIN 10
#else
  #define BME680_SDA_PIN 21
  #define BME680_SCL_PIN 22
#endif

// Lector del sensor ambiental Bosch BME680 por I2C.
// Auto-detecta la direccion (0x76 con SDO->GND, 0x77 con SDO->3V3) y
// expone las ultimas lecturas. update() limita la frecuencia de lectura
// internamente (performReading() bloquea ~150ms por el calentador de gas).
class BME680Sensor {
  private:
    Adafruit_BME680 _bme;
    bool _ready = false;
    unsigned long _lastRead = 0;
    unsigned long _lastBeginTry = 0;
    static const uint32_t READ_INTERVAL_MS = 2000;
    static const uint32_t RETRY_INTERVAL_MS = 5000;

  public:
    // Ultimas lecturas (NAN hasta la primera lectura valida).
    float temperature = NAN;    // grados Celsius
    float humidity = NAN;       // % humedad relativa
    float pressure = NAN;       // hPa
    float gasResistance = NAN;  // kOhm (gas crudo; menor = mas COVs)

    // Inicializa el bus I2C y el sensor. Devuelve true si responde.
    bool begin() {
      Wire.begin(BME680_SDA_PIN, BME680_SCL_PIN);

      // Auto-deteccion de direccion: primero 0x76, luego 0x77.
      if (!_bme.begin(0x76) && !_bme.begin(0x77)) {
        _ready = false;
        return false;
      }

      _bme.setTemperatureOversampling(BME680_OS_8X);
      _bme.setHumidityOversampling(BME680_OS_2X);
      _bme.setPressureOversampling(BME680_OS_4X);
      _bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
      _bme.setGasHeater(320, 150);  // 320 C durante 150 ms

      _ready = true;
      return true;
    }

    bool isReady() const { return _ready; }

    // Llamar a menudo. Reintenta begin() si el sensor no respondio y
    // lee periodicamente. Devuelve true cuando hay valores nuevos.
    bool update() {
      if (!_ready) {
        if (millis() - _lastBeginTry >= RETRY_INTERVAL_MS) {
          _lastBeginTry = millis();
          begin();
        }
        return false;
      }

      if (millis() - _lastRead < READ_INTERVAL_MS) return false;
      _lastRead = millis();

      if (!_bme.performReading()) return false;

      temperature = _bme.temperature;
      humidity = _bme.humidity;
      pressure = _bme.pressure / 100.0f;          // Pa -> hPa
      gasResistance = _bme.gas_resistance / 1000.0f;  // Ohm -> kOhm
      return true;
    }
};
