#include "display_task.h"

DisplayTask::DisplayTask() {}

void DisplayTask::begin(TM1637Display* display, uint8_t brightness) {
    this->display = display;
    timer_running = false;
    targetSeconds = 0;
    remainingSeconds = 0;
    startTime = 0;
    lastBlinkTime = 0;
    colonOn = true;
    display->clear();
    display->setBrightness(brightness);
}

void DisplayTask::update() {
    if (!timer_running || display == nullptr) {
        return;
    }
    
    uint32_t now = millis();
    uint32_t elapsed = (now - startTime) / 1000;
    
    // Toggle colon every 500ms
    if (now - lastBlinkTime >= 500) {
        colonOn = !colonOn;
        lastBlinkTime = now;
    }
    
    if (elapsed >= targetSeconds) {
        // Timer finished
        display->showNumberDecEx(0, 0b01000000, true);
        timer_running = false;
    } else {
        uint32_t remaining = targetSeconds - elapsed;
        if (remaining != remainingSeconds || (now - lastBlinkTime < 50)) {
            remainingSeconds = remaining;
            uint8_t minutes = remainingSeconds / 60;
            uint8_t seconds = remainingSeconds % 60;
            uint8_t colonMask = colonOn ? 0b01000000 : 0b00000000;
            display->showNumberDecEx((minutes * 100) + seconds, colonMask, true);
        }
    }
}

void DisplayTask::setTime(uint32_t seconds) {
    targetSeconds = seconds;
    remainingSeconds = seconds;
}

void DisplayTask::start() {
    startTime = millis();
    timer_running = true;
}

void DisplayTask::stop() {
    timer_running = false;
    if (display != nullptr) {
        display->clear();
    }
}

void DisplayTask::setBrightness(uint8_t brightness) {
    if (display != nullptr) {
        display->setBrightness(brightness);
    }
}
