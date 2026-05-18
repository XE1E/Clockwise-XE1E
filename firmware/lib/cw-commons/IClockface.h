#pragma once

#include "CWDateTime.h"

class IClockface {

public:
    virtual void setup(CWDateTime *dateTime, bool showSplash = true) = 0;
    virtual void update() = 0;

};
