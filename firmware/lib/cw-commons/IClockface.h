#pragma once

#include "CWDateTime.h"

class IClockface {

public:
    virtual void setup(CWDateTime *dateTime, bool showSplash = true) = 0;
    virtual void update() = 0;

    // Night mode methods (optional - only JsonClockface implements these)
    virtual void setupNightMode(CWDateTime *dateTime, uint16_t color) {}
    virtual void setBuiltinNightMode(bool enabled, uint16_t color = 0xF800) {}
    virtual bool supportsNightMode() { return false; }
};
