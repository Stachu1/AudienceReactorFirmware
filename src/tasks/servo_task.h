#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoTask {
public:
    ServoTask();
    void begin(Servo *servo, uint8_t minAng=0, uint8_t maxAng=180);
    void update();
    void setTarget(uint8_t angle, uint16_t duration=0);
    void nodding(uint8_t angle);

private:
    Servo *servo = nullptr;
    uint8_t minA;
    uint8_t maxA;

    // movement state
    uint8_t currentAngle = 0.0f;
    uint8_t targetAngle = 0;
    uint8_t startAngle = 0.0f;
    uint32_t moveStart = 0;
    uint16_t moveDuration = 0;
    uint32_t lastUpdate;
    uint16_t UPDATE_INTERVAL = 2000;
    bool moving = false;
    bool nodOut = true;      // toggle for nod angle and 90
    uint32_t nodLast = 0;    // timing for nod steps
};
