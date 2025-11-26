#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoTask {
public:
    ServoTask();
    void begin(Servo *srv, int16_t minAng, int16_t maxAng);
    void update();
    // command a move to angle (0..180). If durationMs==0 move instantly, otherwise interpolate over durationMs milliseconds.
    void setTarget(int16_t angle, uint16_t durationMs=0);

private:
    Servo *s = nullptr;
    int16_t minA = 0;
    int16_t maxA = 180;
    unsigned long period = 2000;
    unsigned long phaseOffset = 0;

    // movement state
    float currentAngle = 0.0f;
    int16_t targetAngle = 0;
    float startAngle = 0.0f;
    unsigned long moveStartMs = 0;
    unsigned long moveDurationMs = 0;
    bool moving = false;
};
