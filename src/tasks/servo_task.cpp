#include "servo_task.h"

ServoTask::ServoTask() {}

void ServoTask::begin(Servo *srv, uint8_t minAng, uint8_t maxAng, unsigned long periodMs) {
    s = srv;
    minA = minAng;
    maxA = maxAng;
    period = periodMs;
}

void ServoTask::update() {
    if (!s) return;
    unsigned long now = millis();
    unsigned long t = now % period;
    float phase = (float)t / (float)period; // 0..1
    float tri;
    if (phase < 0.5f) tri = phase * 2.0f; else tri = (1.0f - phase) * 2.0f; // 0..1 triangular
    int angle = (int)(minA + tri * (float)(maxA - minA));
    s->write(angle);
}
