#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class BodyColorTask {
public:
    BodyColorTask();
    void begin(Adafruit_NeoPixel* strip, uint8_t iconCount);
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint16_t duration = 1000);
    void update();

private:
    Adafruit_NeoPixel* strip;
    uint8_t iconCount;
    
    // Current color (actual displayed color)
    uint8_t currentR;
    uint8_t currentG;
    uint8_t currentB;
    
    // Target color (desired color)
    uint8_t targetR;
    uint8_t targetG;
    uint8_t targetB;
    
    // Transition state
    uint32_t transitionStart;
    uint16_t transitionDuration;
    bool transitioning;
};
