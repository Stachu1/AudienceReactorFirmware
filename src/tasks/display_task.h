#pragma once
#include <Arduino.h>
#include <TM1637Display.h>

class TimerDisplayTask {
public:
    TimerDisplayTask();

    void begin(TM1637Display* display);
    void start();
    void show(unsigned long durationMs);
    void stop();
    void update();


private:
    TM1637Display* display;
    unsigned long start_Time;
    unsigned long duration;
    unsigned long lastUpdate;
    unsigned long Timer;
    unsigned long last_current_time;
    int8_t TimeDisp[4]= {0,0,0,0};
};
