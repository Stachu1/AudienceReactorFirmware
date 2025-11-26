#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class FadeTask {
public:
    FadeTask();
    void begin(Adafruit_NeoPixel *t, uint8_t c0[3], uint8_t c1[3], uint8_t c2[3], unsigned long dur, uint16_t startIdx=0, uint16_t cnt=0);
    void update();
private:
    Adafruit_NeoPixel *target = nullptr;
    uint8_t colors[3][3];
    uint8_t curIdx = 0;
    unsigned long startMs = 0;
    unsigned long durationMs = 2000;
    bool running = false;
    uint16_t startIndex = 0;
    uint16_t length = 0;
    void applyColorToTarget(uint8_t r, uint8_t g, uint8_t b);
    void startNext();
};
