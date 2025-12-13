#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoTask {
public:
    ServoTask();
    void begin(Servo *servo, uint8_t minAng=0, uint8_t maxAng=180);
    void update();
    void setTarget(uint8_t angle, uint16_t duration=0);
  //void nodding(uint8_t angle);
    void nodOnce(uint8_t angle, uint16_t moveMs = 250, uint16_t pauseMs = 100);

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
    enum NodState : uint8_t { NOD_IDLE, NOD_OUT, NOD_BACK, NOD_PAUSE };
    NodState nodState = NOD_IDLE;
    uint8_t nodAngle = 90;
    uint16_t nodMoveMs = 250;
    uint16_t nodPauseMs = 150;
    uint32_t nodT0 = 0;
    bool moving = false;
};
