#include "body_color_task.h"

BodyColorTask::BodyColorTask() 
    : strip(nullptr), iconCount(0), currentR(0), currentG(0), currentB(0),
      targetR(0), targetG(0), targetB(0), transitionStart(0), 
      transitionDuration(0), transitioning(false) {}

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
    
    if (elapsed >= transitionDuration) {
        // Transition complete
        currentR = targetR;
        currentG = targetG;
        currentB = targetB;
        transitioning = false;
    } else {
        // Interpolate between current and target
        float progress = (float)elapsed / (float)transitionDuration;
        currentR = currentR + (int16_t)((targetR - currentR) * progress);
        currentG = currentG + (int16_t)((targetG - currentG) * progress);
        currentB = currentB + (int16_t)((targetB - currentB) * progress);
    }
    
    // Update strip LEDs (excluding icons)
    for (uint16_t i = iconCount; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, strip->Color(currentR, currentG, currentB));
    }
    strip->show();
}
