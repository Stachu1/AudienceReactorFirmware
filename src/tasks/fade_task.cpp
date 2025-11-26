#include "fade_task.h"

FadeTask::FadeTask() {}

void FadeTask::begin(Adafruit_NeoPixel *t, uint8_t c0[3], uint8_t c1[3], uint8_t c2[3], unsigned long dur, uint16_t startIdx, uint16_t cnt) {
    target = t;
    durationMs = dur;
    for (int i=0;i<3;i++) { colors[0][i]=c0[i]; colors[1][i]=c1[i]; colors[2][i]=c2[i]; }
    curIdx = 0;
    startMs = millis();
    running = true;
    startIndex = startIdx;
    if (cnt==0) length = target->numPixels() - startIndex; else length = cnt;
}

void FadeTask::startNext() {
    startMs = millis();
    running = true;
}

void FadeTask::applyColorToTarget(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t n = length;
    for (uint16_t i=0;i<n;i++) {
        uint16_t p = startIndex + i;
        target->setPixelColor(p, target->Color(r,g,b));
    }
    target->show();
}

void FadeTask::update() {
    if (!running || target==nullptr) return;
    unsigned long now = millis();
    unsigned long elapsed = now - startMs;
    uint8_t fromIdx = curIdx;
    uint8_t toIdx = (curIdx+1)%3;
    if (elapsed >= durationMs) {
        applyColorToTarget(colors[toIdx][0], colors[toIdx][1], colors[toIdx][2]);
        curIdx = toIdx;
        startNext();
    } else {
        float t = (float)elapsed / (float)durationMs;
        int r = (int)(colors[fromIdx][0] + t * (colors[toIdx][0] - colors[fromIdx][0]));
        int g = (int)(colors[fromIdx][1] + t * (colors[toIdx][1] - colors[fromIdx][1]));
        int b = (int)(colors[fromIdx][2] + t * (colors[toIdx][2] - colors[fromIdx][2]));
        applyColorToTarget((uint8_t)r, (uint8_t)g, (uint8_t)b);
    }
}
