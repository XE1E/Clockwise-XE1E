#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

// Commons
#include <IClockface.h>
#include <CWDateTime.h>
#include <BME680Sensor.h>

// Caratula que muestra hora + datos ambientales del sensor BME680
// (temperatura, humedad, presion y resistencia de gas).
class BME680Clockface : public IClockface {
  private:
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    BME680Sensor _sensor;

    int _lastMinute = -1;
    bool _hadData = false;
    bool _errorShown = false;

    void drawStaticLayout();
    void drawTime();
    void drawValues();
    void drawError();

  public:
    BME680Clockface(Adafruit_GFX* display);
    void setup(CWDateTime* dateTime, bool showSplash = true) override;
    void update() override;
};
