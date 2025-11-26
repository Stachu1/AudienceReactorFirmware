#include "icon_task.h"

IconTask::IconTask() {}

void IconTask::begin(Adafruit_NeoPixel *t, uint8_t iconCount, unsigned long interval, uint32_t col) {
    target = t;
    count = iconCount;
    intervalMs = interval;
    color = col;
    lastMs = millis() - intervalMs;
    cur = -1;
}

void IconTask::update() {
    if (!target || count == 0) return;
    unsigned long now = millis();
    if (now - lastMs < intervalMs) return;
    int next = (cur + 1) % count;
    if (cur >= 0) target->setPixelColor(cur, target->Color(0,0,0));
    target->setPixelColor(next, color);
    target->show();
    cur = next;
    lastMs = now;
}

void IconTask::setColor(uint8_t r, uint8_t g, uint8_t b) { if (target) color = target->Color(r,g,b); }

void IconTask::setIconColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (!target) return;
    if (index >= count) return;
    target->setPixelColor(index, target->Color(r,g,b));
    target->show();
}

void IconTask::clearIcon(uint8_t index) {
    if (!target) return;
    if (index >= count) return;
    target->setPixelColor(index, target->Color(0,0,0));
    target->show();
}
