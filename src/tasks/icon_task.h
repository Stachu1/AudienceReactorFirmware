#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class IconTask {
public:
    IconTask();
    void begin(Adafruit_NeoPixel *t, uint8_t iconCount, unsigned long intervalMs, uint32_t col);
    void update();
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setIconColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    void clearIcon(uint8_t index);
private:
    Adafruit_NeoPixel *target = nullptr;
    uint8_t count = 0;
    unsigned long intervalMs = 500;
    unsigned long lastMs = 0;
    int cur = -1;
    uint32_t color = 0;
};
