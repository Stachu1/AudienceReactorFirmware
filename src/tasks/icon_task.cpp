#include "icon_task.h"

IconTask::IconTask()
    : leds(nullptr), iconCount(0) {}

void IconTask::begin(CRGB* buffer, int icons) {
    leds = buffer;
    iconCount = icons;
    clear();
}

void IconTask::clear() {
    if (!leds) return;
    for (int i = 0; i < iconCount; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void IconTask::setIcon(const String& iconName) {
    if (!leds) return;

    clear();

    if (iconName == "meanF0") leds[0] = CRGB::Cyan;
    else if (iconName == "F0std") leds[2] = CRGB::Cyan;
    else if (iconName == "meanF1") leds[4] = CRGB::Cyan;
    else if (iconName == "H1minusA3") leds[6] = CRGB::Cyan;
    else if (iconName == "pauseCount") leds[8] = CRGB::Cyan;

    FastLED.show();
}
