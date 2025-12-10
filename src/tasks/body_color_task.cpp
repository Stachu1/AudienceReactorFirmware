#include "body_color_task.h"

BodyColorTask::BodyColorTask() 
    : strip(nullptr), iconCount(0), currentR(0), currentG(0), currentB(0),
      startR(0), startG(0), startB(0), targetR(0), targetG(0), targetB(0), 
      transitionStart(0), transitionDuration(0), transitioning(false) {}

void BodyColorTask::begin(Adafruit_NeoPixel* strip, uint8_t iconCount) {
    this->strip = strip;
    this->iconCount = iconCount;
    this->currentR = 0;
    this->currentG = 0;
    this->currentB = 0;
    this->targetR = 0;
    this->targetG = 0;
    this->targetB = 0;
    this->transitioning = false;
}

void BodyColorTask::setColor(uint8_t r, uint8_t g, uint8_t b, uint16_t duration) {
    if (strip == nullptr) {
        return;
    }
    
    targetR = r;
    targetG = g;
    targetB = b;
    
    if (duration == 0) {
        // Instant transition
        currentR = r;
        currentG = g;
        currentB = b;
        transitioning = false;
        
        for (uint16_t i = iconCount; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, strip->Color(r, g, b));
        }
        strip->show();
    } else {
        // Start smooth transition
        startR = currentR;
        startG = currentG;
        startB = currentB;
        transitionStart = millis();
        transitionDuration = duration;
        transitioning = true;
    }
}

void BodyColorTask::update() {
    if (!transitioning || strip == nullptr) {
        return;
    }

    uint32_t now = millis();
    uint32_t elapsed = now - transitionStart;

    float progress = (float)elapsed / (float)transitionDuration;
    if (progress >= 1.0f) {
        progress = 1.0f;
        transitioning = false;
    }

    // Smooth interpolation using float math
    currentR = (uint8_t)(startR + (float)(targetR - startR) * progress);
    currentG = (uint8_t)(startG + (float)(targetG - startG) * progress);
    currentB = (uint8_t)(startB + (float)(targetB - startB) * progress);

    // Update strip LEDs (excluding icons)
    for (uint16_t i = iconCount; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, strip->Color(currentR, currentG, currentB));
    }
    strip->show();
}
