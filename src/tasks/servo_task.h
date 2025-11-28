#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoTask {
public:
    ServoTask();
    void begin(Servo *srv, uint8_t minAng=0, uint8_t maxAng=180);
    void update();
    void setTarget(uint8_t angle, uint16_t duration=0);

private:
    Servo *s = nullptr;
    uint8_t minA = 0;
    uint8_t maxA = 180;

    // movement state
    float currentAngle = 0.0f;
    uint8_t targetAngle = 0;
    float startAngle = 0.0f;
    uint32_t moveStart = 0;
    uint32_t moveDuration = 0;
    bool moving = false;
};
