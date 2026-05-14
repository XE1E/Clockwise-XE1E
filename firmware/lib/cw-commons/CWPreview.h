#pragma once

#include <Arduino.h>

class CWPreview {
public:
    static CWPreview* getInstance() {
        static CWPreview instance;
        return &instance;
    }

    String jsonData;
    bool hasPreview = false;

    void setPreview(const String& json) {
        jsonData = json;
        hasPreview = true;
    }

    void clearPreview() {
        jsonData = "";
        hasPreview = false;
    }
};
