#include "icon_task.h"

IconTask::IconTask() {}

void IconTask::begin(Adafruit_NeoPixel *strip, uint8_t iconCount) {
    this->strip = strip;
    this->iconCount = iconCount;
}

void IconTask::update() {
    if (!strip || iconCount == 0) return;
    uint32_t now = millis();
    // handle blinking icons
    for (uint8_t i = 0; i < iconCount && i < MAX_ICONS; i++) {
        if (blinkStates[i].blinking) {
            if (now - blinkStates[i].lastToggle >= blinkStates[i].interval) {
                blinkStates[i].isOn = !blinkStates[i].isOn;
                blinkStates[i].lastToggle = now;
                if (blinkStates[i].isOn) {
                    strip->setPixelColor(i, blinkStates[i].color);
                } else {
                    strip->setPixelColor(i, 0);
                }
                strip->show();
            }
        }
    }
}

void IconTask::setIcon(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint16_t blinkInterval) {
    if (!strip) return;
    if (index >= iconCount || index >= MAX_ICONS) return;
    // set blinking state
    if (blinkInterval == 0) {
        blinkStates[index].blinking = false;
    }
    else {
        blinkStates[index].blinking = true;
        blinkStates[index].color = strip->Color(r,g,b);
        blinkStates[index].interval = blinkInterval;
        blinkStates[index].lastToggle = millis();
    }
    strip->setPixelColor(index, strip->Color(r,g,b));
    strip->show();
}

void IconTask::clearIcon(uint8_t index) {
    if (!strip) return;
    if (index >= iconCount || index >= MAX_ICONS) return;
    // stop blinking if active
    blinkStates[index].blinking = false;
    strip->setPixelColor(index, 0);
    strip->show();
}
