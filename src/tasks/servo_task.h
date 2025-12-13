#pragma once
#include <Arduino.h>
#include <Servo.h>


enum class NodState {
    IDLE,
    OUT,
    BACK,
    PAUSE
};

class ServoTask {
public:
    ServoTask();
    void begin(Servo *servo, uint8_t minAng=0, uint8_t maxAng=180);
    void update();
    void setTarget(uint8_t angle, uint16_t duration=0);
    void nod(uint8_t angle, uint16_t moveMs = 250);

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
    NodState nodState = NodState::IDLE;
    uint16_t nodMoveTime;
    bool moving = false;
};
