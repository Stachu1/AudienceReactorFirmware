#pragma once
#include <Arduino.h>
#include <FastLED.h>

class BodyColorTask {
public:
    BodyColorTask();

    void begin(CRGB* leds, int totalLEDs, int iconCount);
    void setColor(const String& colorName);

private:
    CRGB* leds;
    int numLEDs;
    int iconCount;
};
