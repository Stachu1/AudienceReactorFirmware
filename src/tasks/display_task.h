#pragma once
#include <Arduino.h>
#include <TM1637Display.h>

class DisplayTask {    
public:
    DisplayTask();
    void begin(TM1637Display* display, uint8_t brightness=7);
    void update();
    void show(uint32_t seconds);
    void setTime(uint32_t seconds);
    void start();
    void stop();
    void setBrightness(uint8_t brightness);
    bool timer_running;

private:
    TM1637Display* display = nullptr;
    uint32_t targetSeconds;
    uint32_t remainingSeconds;
    uint32_t startTime;
    uint32_t lastBlinkTime;
    bool colonOn;
};
