#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoTask {
public:
    ServoTask();
    void begin(Servo *srv, uint8_t minAng, uint8_t maxAng, unsigned long periodMs=2000);
    void update();
private:
    Servo *s = nullptr;
    uint8_t minA = 0;
    uint8_t maxA = 180;
    unsigned long period = 2000;
};
