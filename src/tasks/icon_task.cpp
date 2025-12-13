#include "icon_task.h"

IconColorTask::IconColorTask()
    : leds(nullptr), iconCount(0) {}

void IconColorTask::begin(CRGB* buffer, int icons) {
    leds = buffer;
    iconCount = icons;
    clear();
}

void IconColorTask::clear() {
    if (!leds) return;
    for (int i = 0; i < iconCount; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void IconColorTask::setIcon(const String& iconName) {
    if (!leds) return;

    clear();

    if (iconName == "meanF0") leds[0] = CRGB::White;
    else if (iconName == "F0std") leds[2] = CRGB::White;
    else if (iconName == "meanF1") leds[4] = CRGB::White;
    else if (iconName == "H1minusA3") leds[6] = CRGB::White;
    else if (iconName == "pauseCount") leds[8] = CRGB::White;

    FastLED.show();
}
