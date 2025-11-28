#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct IconBlinkState {
    uint32_t color = 0;
    uint32_t lastToggle = 0;
    uint16_t interval = 500;
    bool blinking = false;
    bool isOn = false;
};

class IconTask {
public:
    IconTask();
    void begin(Adafruit_NeoPixel *strip, uint8_t iconCount);
    void update();
    void setIcon(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint16_t blinkInterval=0);
    void clearIcon(uint8_t index);
private:
    Adafruit_NeoPixel *strip = nullptr;
    uint8_t iconCount = 0;
    
    static const uint8_t MAX_ICONS = 10;
    IconBlinkState blinkStates[MAX_ICONS];
};
