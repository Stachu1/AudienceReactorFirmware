#pragma once
#include <FastLED.h>

class IconTask {
public:
    IconTask();

    void begin(CRGB* leds, int iconCount);
    void setIcon(const String& iconName);
    void clear();

private:
    CRGB* leds;
    int iconCount;
};
