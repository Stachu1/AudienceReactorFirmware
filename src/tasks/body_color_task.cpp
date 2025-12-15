#include "body_color_task.h"

BodyColorTask::BodyColorTask()
    : leds(nullptr), numLEDs(0), iconCount(0) {}

void BodyColorTask::begin(CRGB* buffer, int total, int icons) {
    leds = buffer;
    numLEDs = total;
    iconCount = icons;
}

void BodyColorTask::setColor(const String& colorName) {
    if (!leds) return;

    CRGB color = CRGB::White;

    if (colorName == "average") color = CRGB::Yellow;
    else if (colorName == "good") color = CRGB::Green;
    else if (colorName == "extreme") color = CRGB::Red;
    else if (colorName == "idle") {
        for(int i=0; i<iconCount;i++){
            leds[i] = CRGB::Black;
        }
        color = CRGB::White;
    }
    for (int i = iconCount; i < numLEDs; i++) {
        leds[i] = color;
    }

    FastLED.show();
}
