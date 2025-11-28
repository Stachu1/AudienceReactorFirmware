#include "icon_task.h"

IconTask::IconTask() {}

void IconTask::begin(Adafruit_NeoPixel *target, uint8_t count, uint16_t interval, uint32_t color) {
    this->target = target;
    this->count = count;
    this->interval = interval;
    this->color = color;
    last = millis() - interval;
    cur = -1;
    // initialize blink states
    for (uint8_t i = 0; i < MAX_ICONS; i++) {
        blinkStates[i].blinking = false;
        blinkStates[i].isOn = false;
    }
}

void IconTask::update() {
    if (!target || count == 0) return;
    uint32_t now = millis();
    
    // handle blinking icons
    for (uint8_t i = 0; i < count && i < MAX_ICONS; i++) {
        if (blinkStates[i].blinking) {
            if (now - blinkStates[i].lastToggle >= blinkStates[i].interval) {
                blinkStates[i].isOn = !blinkStates[i].isOn;
                blinkStates[i].lastToggle = now;
                if (blinkStates[i].isOn) {
                    target->setPixelColor(i, target->Color(blinkStates[i].r, blinkStates[i].g, blinkStates[i].b));
                } else {
                    target->setPixelColor(i, target->Color(0, 0, 0));
                }
                target->show();
            }
        }
    }
    
    // handle cycling animation (skip blinking icons)
    if (now - last < interval) return;
    int next = (cur + 1) % count;
    if (cur >= 0 && !blinkStates[cur].blinking) {
        target->setPixelColor(cur, target->Color(0,0,0));
    }
    if (!blinkStates[next].blinking) {
        target->setPixelColor(next, color);
    }
    target->show();
    cur = next;
    last = now;
}

void IconTask::setIconColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (!target) return;
    if (index >= count || index >= MAX_ICONS) return;
    // stop blinking if active
    blinkStates[index].blinking = false;
    target->setPixelColor(index, target->Color(r,g,b));
    target->show();
}

void IconTask::setIconBlink(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint16_t blinkInterval) {
    if (!target) return;
    if (index >= count || index >= MAX_ICONS) return;
    blinkStates[index].blinking = true;
    blinkStates[index].r = r;
    blinkStates[index].g = g;
    blinkStates[index].b = b;
    blinkStates[index].interval = blinkInterval;
    blinkStates[index].lastToggle = millis();
    blinkStates[index].isOn = true;
    // start with LED on
    target->setPixelColor(index, target->Color(r, g, b));
    target->show();
}

void IconTask::clearIcon(uint8_t index) {
    if (!target) return;
    if (index >= count || index >= MAX_ICONS) return;
    // stop blinking if active
    blinkStates[index].blinking = false;
    target->setPixelColor(index, target->Color(0,0,0));
    target->show();
}
